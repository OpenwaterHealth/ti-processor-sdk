#include <VX/vx.h>
#include <TI/tivx.h>
#include <TI/j7.h>
#include <TI/j7_tidl.h>
#include <TI/j7_imaging_aewb.h>
#include <tivx_utils_graph_perf.h>
#include <TI/tivx_img_proc.h>
#include <TI/tivx_fileio.h>
#include <TI/tivx_ow_algos.h>
#include <TI/tivx_task.h>
#include <TI/tivx_event.h>
#include <tivx_utils_file_rd_wr.h>
#include <utils/perf_stats/include/app_perf_stats.h>
#include "app_sensor_module.h"
#include "app_capture_module.h"
#include "app_histo_module.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#define CAPTURE1_ENABLE        (1U)
#define CAPTURE2_ENABLE        (1U)

#define MAX_NUM_BUF             (8u)
#define CAPTURE_MIN_PIPEUP_BUFS (3u)

#define NUM_CHANNELS        (1U)
#define CAPT1_INST_ID       (0U)
#define CAPT2_INST_ID       (1U)

#define CAPT1_INST_SENSOR_MASK      0x01 // ((1 << NUM_CHANNELS) - 1U)
#define CAPT2_INST_SENSOR_MASK      0x10 // (((1 << NUM_CHANNELS) - 1U) << 4U)

#define CAPTURE1_NODE_ERROR  (1U)
#define CAPTURE2_NODE_ERROR  (1U)

/* Default is RAW image */
#define CAPTURE_FORMAT         (TIVX_TYPE_RAW_IMAGE)

#define TIVX_TARGET_DEFAULT_STACK_SIZE      (256U * 1024U)
#define TIVX_TARGET_DEFAULT_TASK_PRIORITY1   (8u)

#define VX_KERNEL_MERGE_HISTOGRAMS (VX_KERNEL_BASE(VX_ID_DEFAULT, 0x3F0) + 0x0)

#define IN_OW_LEFT_FILE_NAME     "/opt/vision_apps/csix_raw_output_ch_0_fr_1048_exp0_2720x1450_10bit_bayer.y"
#define IN_OW_RIGHT_FILE_NAME    "/opt/vision_apps/csix_raw_output_ch_2_fr_1048_exp0_2720x1450_10bit_bayer.y"


#define ERROR_CHECK_STATUS( status ) { \
        vx_status status_ = (status); \
        if(status_ != VX_SUCCESS) { \
            printf("ERROR: failed with status = (%d) at " __FILE__ "#%d\n", status_, __LINE__); \
        } \
}

#define ERROR_CHECK_OBJECT( obj ) { \
        vx_status status_ = vxGetStatus((vx_reference)(obj)); \
        if(status_ != VX_SUCCESS) { \
            printf("ERROR: failed with status = (%d) at " __FILE__ "#%d\n", status_, __LINE__); \
        } \
}


#define ERROR_CHECK_STATUS2( status ) { \
        vx_status status_ = (status); \
        if(status_ != VX_SUCCESS) { \
            printf("ERROR: failed with status = (%d) at " __FILE__ "#%d\n", status_, __LINE__); \
            return VX_FAILURE; \
        } \
}

#define ERROR_CHECK_OBJECT2( obj ) { \
        vx_status status_ = vxGetStatus((vx_reference)(obj)); \
        if(status_ != VX_SUCCESS) { \
            printf("ERROR: failed with status = (%d) at " __FILE__ "#%d\n", status_, __LINE__); \
            return VX_FAILURE; \
        } \
}

#define MAX_NUM_BUF             (8u)
#define CAPTURE_BUFFER_Q_DEPTH  (4)
#define APP_BUFFER_Q_DEPTH      (4)
#define APP_PIPELINE_DEPTH      (7)
#define MAX_ATTRIBUTE_NAME (32u)

#define MAX_FNAME (256)
#define APP_MAX_FILE_PATH           (256u)
#define APP_ASSERT(x)               assert((x))
#define APP_ASSERT_VALID_REF(ref)   (APP_ASSERT(vxGetStatus((vx_reference)(ref))==VX_SUCCESS));

typedef struct {
    uint32_t statusFlag;
} tivxHistoParams;

typedef struct {
    /* OpenVX references */
    vx_context context;
    vx_graph left_graph;
    vx_graph right_graph;    

    tivx_task left_capture_task;
    tivx_task right_capture_task;
    tivx_task_create_params_t left_taskParams_capture;
    tivx_task_create_params_t right_taskParams_capture;

    tivx_event  eventHandle_Left_RxFinished;
    tivx_event  eventHandle_Right_RxFinished;
    tivx_event  eventHandle_SensorCfgDone;
    tivx_event  eventHandle_SensorCfgDone1;
    tivx_event  left_graph_setupComplete;
    tivx_event  right_graph_setupComplete;

    uint32_t sensorCfgDone;

    SensorObj     left_sensor;
    SensorObj     right_sensor;
    CaptureObj    left_capture;
    CaptureObj    right_capture;
    HistoObj      left_histo;
    HistoObj      right_histo;

    vx_node  merge_node;
    vx_kernel merge_kernel;

    tivx_raw_image left_input_img;
    tivx_raw_image right_input_img;

    tivx_task task;
    vx_uint32 stop_task;
    vx_uint32 stop_task_done;

    int32_t pipeline;
    int32_t enqueueCnt;
    int32_t dequeueCnt;

    vx_char output_file_path[APP_MAX_FILE_PATH];

    app_perf_point_t total_perf;

} AppObj;

AppObj gAppObj;
uint32_t g_num_bins = 1024;
static uint32_t width, height, loop_cnt, num_buf;

static vx_status app_c66_kernels_load(vx_context context);
static vx_status app_c66_kernels_unload(vx_context context);
static vx_status initialize_merge_histograms_kernel(AppObj *obj);
static vx_status VX_CALLBACK merge_histograms_validator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[]);
static vx_status VX_CALLBACK merge_histograms_kernel(vx_node node, const vx_reference *parameters, vx_uint32 num);
static void add_graph_parameter_by_node_index(vx_graph graph, vx_node node, vx_uint32 node_parameter_index);
static void VX_CALLBACK tivxTask_left_capture(void *app_var);
static void VX_CALLBACK tivxTask_right_capture(void *app_var);

static void add_graph_parameter_by_node_index(vx_graph graph, vx_node node, vx_uint32 node_parameter_index)
{
    vx_parameter parameter = vxGetParameterByIndex(node, node_parameter_index);

    vxAddParameterToGraph(graph, parameter);
    vxReleaseParameter(&parameter);
}

static void VX_CALLBACK tivxTask_left_capture(void *app_var)
{

    printf("tivxTask_left_capture\n");
    vx_status status = VX_SUCCESS;
    vx_node csirx_node = 0;
    vx_object_array capture_frames[MAX_NUM_BUF];
    vx_user_data_object capture_config;
    tivx_capture_params_t local_capture_config;
    uint32_t buf_id, loop_id, loopCnt;
    vx_int32 graph_param_index;
    vx_graph_parameter_queue_params_t csirx_graph_parameters_queue_params_list[5];
    
    printf("create left graph\n");
    AppObj *obj = (AppObj *)app_var;
    vx_graph csirx_graph = (vx_graph)obj->left_graph;

    IssSensor_CreateParams *sensorParams = &obj->left_sensor.sensorParams;
    tivx_raw_image raw_image = tivxCreateRawImage(obj->context, &sensorParams->sensorInfo.raw_params);
    ERROR_CHECK_STATUS(vxGetStatus((vx_reference)raw_image));
    
    if(num_buf == 0)
    {
        printf("Error number of buffers set to zero\n");
        return;
    }

    /* allocate Input and Output refs, multiple refs created to allow pipelining of left_graph */
    for(buf_id=0; buf_id<num_buf; buf_id++)
    {
        capture_frames[buf_id] = vxCreateObjectArray(obj->context, (vx_reference)raw_image, NUM_CHANNELS);    
        status = vxGetStatus((vx_reference)capture_frames[buf_id] );    
        if(status != VX_SUCCESS)
        {
            printf("[CAPTURE-LEFT] Unable to create RAW image object array! \n");
        }
        else
        {
            vx_char name[VX_MAX_REFERENCE_NAME];
            snprintf(name, VX_MAX_REFERENCE_NAME, "left_capture_raw_image_arr_%d", buf_id);
            vxSetReferenceName((vx_reference)capture_frames[buf_id], name);
        }
    }
    tivxReleaseRawImage(&raw_image);
    
    printf("create left capture\n");
    tivx_capture_params_init(&local_capture_config);
    local_capture_config.numInst                          = 1U;
    local_capture_config.numCh                            = NUM_CHANNELS;  
    local_capture_config.instId[0U]                       = CAPT1_INST_ID;
    local_capture_config.instCfg[0U].enableCsiv2p0Support = (uint32_t)vx_true_e;
    local_capture_config.instCfg[0U].numDataLanes         = 4U;
    for (loopCnt = 0U ;
        loopCnt < local_capture_config.instCfg[0U].numDataLanes ;
        loopCnt++)
    {
        local_capture_config.instCfg[0U].dataLanesMap[loopCnt] = (loopCnt + 1u);
    }

    for (loopCnt = 0U; loopCnt < NUM_CHANNELS; loopCnt++)
    {
        local_capture_config.chVcNum[loopCnt]   = loopCnt;
        local_capture_config.chInstMap[loopCnt] = CAPT1_INST_ID;
    }

    capture_config = vxCreateUserDataObject(obj->context, "tivx_capture_params_t", sizeof(tivx_capture_params_t), &local_capture_config);
    ERROR_CHECK_OBJECT(capture_config);
    csirx_node = tivxCaptureNode(csirx_graph, capture_config, capture_frames[0]);
    ERROR_CHECK_OBJECT(csirx_node);
        
    ERROR_CHECK_STATUS(app_create_graph_histo(csirx_graph, &obj->left_histo, capture_frames[0], 1));

    /* input @ node index 0, becomes csirx_graph parameter 1 */
    printf("adding left capture to parameter list\n");
    graph_param_index = 0;
    add_graph_parameter_by_node_index(csirx_graph, csirx_node, 1);
    csirx_graph_parameters_queue_params_list[graph_param_index].graph_parameter_index = graph_param_index;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list_size = num_buf;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list = (vx_reference*)&capture_frames[0];

    graph_param_index++;

    printf("adding left histo distribution to parameter list\n");
    add_graph_parameter_by_node_index(csirx_graph, obj->left_histo.node, TIVX_KERNEL_OW_CALC_HISTO_HISTO_IDX);
    obj->left_histo.graph_parameter_distribution_index = graph_param_index;
    csirx_graph_parameters_queue_params_list[graph_param_index].graph_parameter_index = graph_param_index;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list_size = APP_BUFFER_Q_DEPTH;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list = (vx_reference*)&obj->left_histo.outHistoCh0[0];

    graph_param_index++;

    printf("adding left histo mean to parameter list\n");
    add_graph_parameter_by_node_index(csirx_graph, obj->left_histo.node, TIVX_KERNEL_OW_CALC_HISTO_MEAN_IDX);
    obj->left_histo.graph_parameter_mean_index = graph_param_index;
    csirx_graph_parameters_queue_params_list[graph_param_index].graph_parameter_index = graph_param_index;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list_size = APP_BUFFER_Q_DEPTH;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list = (vx_reference*)&obj->left_histo.outMeanCh0[0];
    
    graph_param_index++;

    printf("adding left histo sd to parameter list\n");
    add_graph_parameter_by_node_index(csirx_graph, obj->left_histo.node, TIVX_KERNEL_OW_CALC_HISTO_SD_IDX);
    obj->left_histo.graph_parameter_sd_index = graph_param_index;
    csirx_graph_parameters_queue_params_list[graph_param_index].graph_parameter_index = graph_param_index;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list_size = APP_BUFFER_Q_DEPTH;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list = (vx_reference *)&obj->left_histo.outSdCh0[0];

    graph_param_index++;

    printf("create left schedule graph\n");
    /* Schedule mode auto is used, here we dont need to call vxScheduleGraph
    * Graph gets scheduled automatically as refs are enqueued to it
    */
    vxSetGraphScheduleConfig(csirx_graph,
            VX_GRAPH_SCHEDULE_MODE_QUEUE_AUTO,
            graph_param_index,
            csirx_graph_parameters_queue_params_list
            );

    printf("set left node target and name\n");
    ERROR_CHECK_STATUS(vxSetNodeTarget(csirx_node, VX_TARGET_STRING, TIVX_TARGET_CAPTURE1));
    vxSetReferenceName((vx_reference)csirx_node, "left_capture_node");


    printf("verify and output graph\n");
    ERROR_CHECK_STATUS(vxVerifyGraph(csirx_graph));
    tivxExportGraphToDot(csirx_graph,".", "left_graph");

    printf("left posting graph setup complete\n");
    tivxEventPost(obj->left_graph_setupComplete);
    
    printf("left waiting for sensor startup\n");
    tivxEventWait(obj->eventHandle_SensorCfgDone, TIVX_EVENT_TIMEOUT_WAIT_FOREVER);
    
    printf("enqueue left capture buffers\n");
    /* enqueue capture buffers for pipeup but dont trigger graph executions */
    for(buf_id=0; buf_id<num_buf-1; buf_id++)
    {
        status = tivxGraphParameterEnqueueReadyRef(csirx_graph, 0, (vx_reference*)&capture_frames[buf_id], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP); 
        status |= tivxGraphParameterEnqueueReadyRef(csirx_graph, obj->left_histo.graph_parameter_distribution_index, (vx_reference*)&obj->left_histo.outHistoCh0[buf_id], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP); 
        status |= tivxGraphParameterEnqueueReadyRef(csirx_graph, obj->left_histo.graph_parameter_mean_index, (vx_reference*)&obj->left_histo.outMeanCh0[buf_id], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP); 
        status |= tivxGraphParameterEnqueueReadyRef(csirx_graph, obj->left_histo.graph_parameter_sd_index, (vx_reference*)&obj->left_histo.outSdCh0[buf_id], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP); 
    }

    if (status != VX_SUCCESS)
    {
        printf("left channel enqueue buffers failed\n");
        goto end_left_graph;
    }

    printf("after pipeup for left capture\n");
    /* after pipeup, now enqueue a buffer to trigger graph scheduling */
    status = vxGraphParameterEnqueueReadyRef(csirx_graph, 0, (vx_reference*)&capture_frames[buf_id], 1);
    status |= vxGraphParameterEnqueueReadyRef(csirx_graph, obj->left_histo.graph_parameter_distribution_index, (vx_reference*)&obj->left_histo.outHistoCh0[buf_id], 1);
    status |= vxGraphParameterEnqueueReadyRef(csirx_graph, obj->left_histo.graph_parameter_mean_index, (vx_reference*)&obj->left_histo.outMeanCh0[buf_id], 1);
    status |= vxGraphParameterEnqueueReadyRef(csirx_graph, obj->left_histo.graph_parameter_sd_index, (vx_reference*)&obj->left_histo.outSdCh0[buf_id], 1);

    if (status != VX_SUCCESS)
    {
        printf("after pipeup for left channel enqueue buffers failed\n");
        goto end_left_graph;
    }
    
    printf("left compare output and recycle data buffers\n");
    /* wait for csirx_graph instances to complete, compare output and recycle data buffers, schedule again */
    for(loop_id=0; loop_id<loop_cnt; loop_id++)
    {
        uint32_t num_refs;
        vx_object_array captured_frames = NULL;
        vx_distribution output_histo = NULL;
        vx_scalar output_mean = NULL;
        vx_scalar output_sd = NULL;

        /* Get captured frame reference, waits until a reference is available */
        status = vxGraphParameterDequeueDoneRef(csirx_graph, 0, (vx_reference*)&captured_frames, 1, &num_refs);
        status |= vxGraphParameterDequeueDoneRef(csirx_graph, obj->left_histo.graph_parameter_distribution_index, (vx_reference *)&output_histo, 1, &num_refs);
        status |= vxGraphParameterDequeueDoneRef(csirx_graph, obj->left_histo.graph_parameter_mean_index, (vx_reference *)&output_mean, 1, &num_refs);
        status |= vxGraphParameterDequeueDoneRef(csirx_graph, obj->left_histo.graph_parameter_sd_index, (vx_reference *)&output_sd, 1, &num_refs);
      
        if (status != VX_SUCCESS)
        {
            printf("left vxGraphParameterDequeueDoneRef failed\n");
            goto end_left_graph;
        }
    
        status = vxGraphParameterEnqueueReadyRef(csirx_graph, 0, (vx_reference*)&captured_frames, 1);
        status |= vxGraphParameterEnqueueReadyRef(csirx_graph, obj->left_histo.graph_parameter_distribution_index, (vx_reference*)&output_histo, 1);
        status |= vxGraphParameterEnqueueReadyRef(csirx_graph, obj->left_histo.graph_parameter_mean_index, (vx_reference*)&output_mean, 1);
        status |= vxGraphParameterEnqueueReadyRef(csirx_graph, obj->left_histo.graph_parameter_sd_index, (vx_reference*)&output_sd, 1);
    
        if (status != VX_SUCCESS)
        {
            printf("left vxGraphParameterEnqueueReadyRef failed\n");
            goto end_left_graph;
        }

    }

    printf("wait for left graph\n");
    /* ensure all csirx_graph processing is complete */
    vxWaitGraph(csirx_graph);

end_left_graph:
    //printf("stop left sensor\n");
    ERROR_CHECK_STATUS(appStopImageSensor(obj->left_sensor.sensor_name, CAPT1_INST_SENSOR_MASK)); /*Mask for 2 cameras*/

    ERROR_CHECK_STATUS(vxReleaseNode(&csirx_node));
    for(buf_id=0; buf_id<num_buf; buf_id++)
    {
        ERROR_CHECK_STATUS(vxReleaseObjectArray(&capture_frames[buf_id]));
    }

    ERROR_CHECK_STATUS(vxReleaseUserDataObject(&capture_config));

    /*Signal the completion of csirx graph processing*/
    tivxEventPost(obj->eventHandle_Left_RxFinished);
}

static void VX_CALLBACK tivxTask_right_capture(void *app_var)
{

    printf("tivxTask_right_capture\n");
    vx_status status = VX_SUCCESS;
    vx_node csirx_node = 0;
    vx_object_array capture_frames[MAX_NUM_BUF];
    vx_user_data_object capture_config;
    tivx_capture_params_t local_capture_config;
    uint32_t buf_id, loop_id, loopCnt;
    vx_int32 graph_param_index;
    vx_graph_parameter_queue_params_t csirx_graph_parameters_queue_params_list[5];
    
    printf("create right graph\n");
    AppObj *obj = (AppObj *)app_var;
    vx_graph csirx_graph = (vx_graph)obj->right_graph;

    IssSensor_CreateParams *sensorParams = &obj->right_sensor.sensorParams;
    tivx_raw_image raw_image = tivxCreateRawImage(obj->context, &sensorParams->sensorInfo.raw_params);
    ERROR_CHECK_STATUS(vxGetStatus((vx_reference)raw_image));
    
    if(num_buf == 0)
    {
        printf("Error number of buffers set to zero\n");
        return;
    }

    /* allocate Input and Output refs, multiple refs created to allow pipelining of right_graph */
    for(buf_id=0; buf_id<num_buf; buf_id++)
    {
        capture_frames[buf_id] = vxCreateObjectArray(obj->context, (vx_reference)raw_image, NUM_CHANNELS);   
        status = vxGetStatus((vx_reference)capture_frames[buf_id] );    
        if(status != VX_SUCCESS)
        {
            printf("[CAPTURE-RIGHT] Unable to create RAW image object array! \n");
        }
        else
        {
            vx_char name[VX_MAX_REFERENCE_NAME];
            snprintf(name, VX_MAX_REFERENCE_NAME, "right_capture_raw_image_arr_%d", buf_id);
            vxSetReferenceName((vx_reference)capture_frames[buf_id], name);
        }
    }
    tivxReleaseRawImage(&raw_image);

    printf("create right capture\n");
    tivx_capture_params_init(&local_capture_config);
    local_capture_config.numInst                          = 1U;
    local_capture_config.numCh                            = NUM_CHANNELS;  
    local_capture_config.instId[0U]                       = CAPT2_INST_ID;
    local_capture_config.instCfg[0U].enableCsiv2p0Support = (uint32_t)vx_true_e;
    local_capture_config.instCfg[0U].numDataLanes         = 4U;
    for (loopCnt = 0U ;
        loopCnt < local_capture_config.instCfg[0U].numDataLanes ;
        loopCnt++)
    {
        local_capture_config.instCfg[0U].dataLanesMap[loopCnt] = (loopCnt + 1u);
    }

    for (loopCnt = 0U; loopCnt < NUM_CHANNELS; loopCnt++)
    {
        local_capture_config.chVcNum[loopCnt]   = loopCnt;
        local_capture_config.chInstMap[loopCnt] = CAPT2_INST_ID;
    }

    capture_config = vxCreateUserDataObject(obj->context, "tivx_capture_params_t", sizeof(tivx_capture_params_t), &local_capture_config);
    ERROR_CHECK_OBJECT(capture_config);
    csirx_node = tivxCaptureNode(csirx_graph, capture_config, capture_frames[0]);
    ERROR_CHECK_OBJECT(csirx_node);
        
    ERROR_CHECK_STATUS(app_create_graph_histo(csirx_graph, &obj->right_histo, capture_frames[0], 2));

    /* set csirx_graph schedule config such that csirx_graph parameter enqueuable */
    printf("adding right capture to parameter list\n");
    graph_param_index = 0;
    add_graph_parameter_by_node_index(csirx_graph, csirx_node, 1);
    csirx_graph_parameters_queue_params_list[graph_param_index].graph_parameter_index = graph_param_index;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list_size = num_buf;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list = (vx_reference*)&capture_frames[0];

    graph_param_index++;

    printf("adding right histo distribution to parameter list\n");
    add_graph_parameter_by_node_index(csirx_graph, obj->right_histo.node, TIVX_KERNEL_OW_CALC_HISTO_HISTO_IDX);
    obj->right_histo.graph_parameter_distribution_index = graph_param_index;
    csirx_graph_parameters_queue_params_list[graph_param_index].graph_parameter_index = graph_param_index;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list_size = APP_BUFFER_Q_DEPTH;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list = (vx_reference*)&obj->right_histo.outHistoCh0[0];

    graph_param_index++;

    printf("adding right histo mean to parameter list\n");
    add_graph_parameter_by_node_index(csirx_graph, obj->right_histo.node, TIVX_KERNEL_OW_CALC_HISTO_MEAN_IDX);
    obj->right_histo.graph_parameter_mean_index = graph_param_index;
    csirx_graph_parameters_queue_params_list[graph_param_index].graph_parameter_index = graph_param_index;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list_size = APP_BUFFER_Q_DEPTH;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list = (vx_reference*)&obj->right_histo.outMeanCh0[0];

    graph_param_index++;

    printf("adding right histo sd to parameter list\n");
    add_graph_parameter_by_node_index(csirx_graph, obj->right_histo.node, TIVX_KERNEL_OW_CALC_HISTO_SD_IDX);
    obj->right_histo.graph_parameter_sd_index = graph_param_index;
    csirx_graph_parameters_queue_params_list[graph_param_index].graph_parameter_index = graph_param_index;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list_size = APP_BUFFER_Q_DEPTH;
    csirx_graph_parameters_queue_params_list[graph_param_index].refs_list = (vx_reference *)&obj->right_histo.outSdCh0[0];

    graph_param_index++;

    printf("create right schedule graph\n");
    /* Schedule mode auto is used, here we dont need to call vxScheduleGraph
    * Graph gets scheduled automatically as refs are enqueued to it
    */
    vxSetGraphScheduleConfig(csirx_graph,
            VX_GRAPH_SCHEDULE_MODE_QUEUE_AUTO,
            graph_param_index,
            csirx_graph_parameters_queue_params_list
            );

    printf("set right node target and name\n");
    ERROR_CHECK_STATUS(vxSetNodeTarget(csirx_node, VX_TARGET_STRING, TIVX_TARGET_CAPTURE2));
    vxSetReferenceName((vx_reference)csirx_node, "right_capture_node");

    printf("verify and output graph\n");
    ERROR_CHECK_STATUS(vxVerifyGraph(csirx_graph));
    tivxExportGraphToDot(csirx_graph,".", "right_graph");

    printf("right posting graph setup complete\n");
    tivxEventPost(obj->right_graph_setupComplete);

    printf("right waiting for sensor startup\n");
    tivxEventWait(obj->eventHandle_SensorCfgDone1, TIVX_EVENT_TIMEOUT_WAIT_FOREVER);
    
    printf("enqueue right capture buffers\n");
    /* enqueue capture buffers for pipeup but dont trigger graph executions */
    for(buf_id=0; buf_id<num_buf-1; buf_id++)
    {
        status = tivxGraphParameterEnqueueReadyRef(csirx_graph, 0, (vx_reference*)&capture_frames[buf_id], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP);
        status |= tivxGraphParameterEnqueueReadyRef(csirx_graph, obj->right_histo.graph_parameter_distribution_index, (vx_reference*)&obj->right_histo.outHistoCh0[buf_id], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP); 
        status |= tivxGraphParameterEnqueueReadyRef(csirx_graph, obj->right_histo.graph_parameter_mean_index, (vx_reference*)&obj->right_histo.outMeanCh0[buf_id], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP); 
        status |= tivxGraphParameterEnqueueReadyRef(csirx_graph, obj->right_histo.graph_parameter_sd_index, (vx_reference*)&obj->right_histo.outSdCh0[buf_id], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP); 
    }

    if (status != VX_SUCCESS)
    {
        printf("right channel enqueue buffers failed\n");
        goto end_right_graph;
    }

    printf("after pipeup for right capture\n");
    /* after pipeup, now enqueue a buffer to trigger graph scheduling */
    status = vxGraphParameterEnqueueReadyRef(csirx_graph, 0, (vx_reference*)&capture_frames[buf_id], 1);
    status |= vxGraphParameterEnqueueReadyRef(csirx_graph, obj->right_histo.graph_parameter_distribution_index, (vx_reference*)&obj->right_histo.outHistoCh0[buf_id], 1);
    status |= vxGraphParameterEnqueueReadyRef(csirx_graph, obj->right_histo.graph_parameter_mean_index, (vx_reference*)&obj->right_histo.outMeanCh0[buf_id], 1);
    status |= vxGraphParameterEnqueueReadyRef(csirx_graph, obj->right_histo.graph_parameter_sd_index, (vx_reference*)&obj->right_histo.outSdCh0[buf_id], 1);

    if (status != VX_SUCCESS)
    {
        printf("after pipeup for right channel enqueue buffers failed\n");
        goto end_right_graph;
    }
    
    printf("right compare output and recycle data buffers\n");
    /* wait for csirx_graph instances to complete, compare output and recycle data buffers, schedule again */
    for(loop_id=0; loop_id<loop_cnt; loop_id++)
    {
        uint32_t num_refs;
        vx_object_array captured_frames = NULL;
        vx_distribution output_histo = NULL;
        vx_scalar output_mean = NULL;
        vx_scalar output_sd = NULL;

        /* Get captured frame reference, waits until a reference is available */
        status = vxGraphParameterDequeueDoneRef(csirx_graph, 0, (vx_reference*)&captured_frames, 1, &num_refs);
        status |= vxGraphParameterDequeueDoneRef(csirx_graph, obj->right_histo.graph_parameter_distribution_index, (vx_reference *)&output_histo, 1, &num_refs);
        status |= vxGraphParameterDequeueDoneRef(csirx_graph, obj->right_histo.graph_parameter_mean_index, (vx_reference *)&output_mean, 1, &num_refs);
        status |= vxGraphParameterDequeueDoneRef(csirx_graph, obj->right_histo.graph_parameter_sd_index, (vx_reference *)&output_sd, 1, &num_refs);
      
        if (status != VX_SUCCESS)
        {
            printf("left vxGraphParameterDequeueDoneRef failed\n");
            goto end_right_graph;
        }

        status = vxGraphParameterEnqueueReadyRef(csirx_graph, 0, (vx_reference*)&captured_frames, 1);
        status |= vxGraphParameterEnqueueReadyRef(csirx_graph, obj->right_histo.graph_parameter_distribution_index, (vx_reference*)&output_histo, 1);
        status |= vxGraphParameterEnqueueReadyRef(csirx_graph, obj->right_histo.graph_parameter_mean_index, (vx_reference*)&output_mean, 1);
        status |= vxGraphParameterEnqueueReadyRef(csirx_graph, obj->right_histo.graph_parameter_sd_index, (vx_reference*)&output_sd, 1);
    
        if (status != VX_SUCCESS)
        {
            printf("left vxGraphParameterEnqueueReadyRef failed\n");
            goto end_right_graph;
        }

    }

    printf("wait for right graph\n");
    /* ensure all csirx_graph processing is complete */
    vxWaitGraph(csirx_graph);

end_right_graph:
    //printf("stop right sensor\n");
    ERROR_CHECK_STATUS(appStopImageSensor(obj->right_sensor.sensor_name, CAPT2_INST_SENSOR_MASK));

    ERROR_CHECK_STATUS(vxReleaseNode(&csirx_node));
    for(buf_id=0; buf_id<num_buf; buf_id++)
    {
        ERROR_CHECK_STATUS(vxReleaseObjectArray(&capture_frames[buf_id]));
    }

    ERROR_CHECK_STATUS(vxReleaseUserDataObject(&capture_config));

    /*Signal the completion of csirx graph processing*/
    tivxEventPost(obj->eventHandle_Right_RxFinished);
}

int app_split_graph_main(int argc, char* argv[])
{
    AppObj *obj = &gAppObj;
    vx_status status = VX_SUCCESS;
    
    num_buf = 4;
    loop_cnt = 40;

    printf("Executing app_split_graph_main\n");
    
    memset(obj->output_file_path, 0, APP_MAX_FILE_PATH);
    sprintf(obj->output_file_path, "%s", "/opt/vision_apps/output");

    // set histo output path
    memset(obj->left_histo.output_file_path, 0, APP_MAX_FILE_PATH);
    sprintf(obj->left_histo.output_file_path, "%s", "/home/root");
    memset(obj->right_histo.output_file_path, 0, APP_MAX_FILE_PATH);
    sprintf(obj->right_histo.output_file_path, "%s", "/home/root");

    obj->left_sensor.sensor_index = 7;
    obj->left_sensor.enable_ldc = 0;
    obj->left_sensor.num_cameras_enabled = 1;
    obj->left_sensor.ch_mask = CAPT1_INST_SENSOR_MASK;
    obj->left_sensor.usecase_option = APP_SENSOR_FEATURE_CFG_UC0;
    ERROR_CHECK_STATUS(app_querry_sensor(&obj->left_sensor));

    obj->right_sensor.sensor_index = 7;
    obj->right_sensor.enable_ldc = 0;
    obj->right_sensor.num_cameras_enabled = 1;
    obj->right_sensor.ch_mask = CAPT2_INST_SENSOR_MASK;
    obj->right_sensor.usecase_option = APP_SENSOR_FEATURE_CFG_UC0;
    ERROR_CHECK_STATUS(app_querry_sensor(&obj->right_sensor));

    obj->pipeline       = -APP_BUFFER_Q_DEPTH + 1;
    obj->enqueueCnt     = 0;
    obj->dequeueCnt     = 0;
    obj->sensorCfgDone  = 0U;

    
    printf("creating context\n");
    obj->context = vxCreateContext();    
    ERROR_CHECK_STATUS2(vxGetStatus((vx_reference)obj->context));
    
    printf("loading kernels\n");
    ERROR_CHECK_STATUS2(app_c66_kernels_load(obj->context));
    tivxHwaLoadKernels(obj->context);
    tivxImagingLoadKernels(obj->context);
    tivxFileIOLoadKernels(obj->context);

    // initialize merge kernel
    ERROR_CHECK_STATUS2(initialize_merge_histograms_kernel(obj));
    ERROR_CHECK_OBJECT2(obj->merge_kernel);
    
    // initialize frame sync    
    AppD3boardInitCmdParams cmdPrms;

    cmdPrms.moduleId = APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC;
    cmdPrms.fSyncPeriod = 25000;
    cmdPrms.fSyncDuration = 1;
    cmdPrms.strobeLightDelayStart = 24350;
    cmdPrms.strobeLightDuration = 105;
    cmdPrms.timerResolution = 225;
    
    int32_t d3board_init_status = appInitD3board(&cmdPrms);
    if (0 != d3board_init_status)
    {
        /* Not returning failure because application may be waiting for
        error/test frame */
        printf("[OW_MAIN] Error initializing D3 board\n");
        return VX_FAILURE;
    }

    obj->left_graph = vxCreateGraph(obj->context);
    ERROR_CHECK_OBJECT2(obj->left_graph);
    obj->right_graph = vxCreateGraph(obj->context);
    ERROR_CHECK_OBJECT2(obj->right_graph);

    printf("init sensor object\n");
    ERROR_CHECK_STATUS2(app_init_sensor(&obj->left_sensor, "left_sensor"));
    ERROR_CHECK_STATUS2(app_init_sensor(&obj->right_sensor, "right_sensor"));

    width = obj->left_sensor.image_width;
    height = obj->left_sensor.image_height;
    
    printf("Captuer Image %dx%d\n", width, height);

    ERROR_CHECK_STATUS(app_init_histo(obj->context, &obj->left_histo, &obj->left_sensor, "left_histo", APP_BUFFER_Q_DEPTH, 1024, NUM_CHANNELS));
    ERROR_CHECK_STATUS(app_init_histo(obj->context, &obj->right_histo, &obj->right_sensor, "right_histo", APP_BUFFER_Q_DEPTH, 1024, NUM_CHANNELS));

    //Create events for Sync		
    ERROR_CHECK_STATUS2(tivxEventCreate(&obj->eventHandle_Left_RxFinished));
    ERROR_CHECK_STATUS2(tivxEventCreate(&obj->left_graph_setupComplete));
    ERROR_CHECK_STATUS2(tivxEventCreate(&obj->right_graph_setupComplete));
    
    // Setting up task params for left capture task
    tivxTaskSetDefaultCreateParams(&obj->left_taskParams_capture);
    obj->left_taskParams_capture.task_main = &tivxTask_left_capture;
    obj->left_taskParams_capture.app_var = obj;
    obj->left_taskParams_capture.stack_ptr = NULL;
    obj->left_taskParams_capture.stack_size = TIVX_TARGET_DEFAULT_STACK_SIZE;
    obj->left_taskParams_capture.core_affinity = TIVX_TASK_AFFINITY_ANY;
    obj->left_taskParams_capture.priority = TIVX_TARGET_DEFAULT_TASK_PRIORITY1;
    ERROR_CHECK_STATUS2(tivxEventCreate(&obj->eventHandle_Right_RxFinished));
    ERROR_CHECK_STATUS2(tivxEventCreate(&obj->eventHandle_SensorCfgDone));
    ERROR_CHECK_STATUS2(tivxEventCreate(&obj->eventHandle_SensorCfgDone1));

    // Setting up task params for right capture task
    tivxTaskSetDefaultCreateParams(&obj->right_taskParams_capture);
    obj->right_taskParams_capture.task_main = &tivxTask_right_capture;
    obj->right_taskParams_capture.app_var = obj;
    obj->right_taskParams_capture.stack_ptr = NULL;
    obj->right_taskParams_capture.stack_size = TIVX_TARGET_DEFAULT_STACK_SIZE;
    obj->right_taskParams_capture.core_affinity = TIVX_TASK_AFFINITY_ANY;
    obj->right_taskParams_capture.priority = TIVX_TARGET_DEFAULT_TASK_PRIORITY1;

    printf("Starting framesync\n");    
    appStartFrameSync();
    
    printf("creating threads\n");    
    ERROR_CHECK_STATUS2(tivxTaskCreate(&obj->left_capture_task, &obj->left_taskParams_capture));
    ERROR_CHECK_STATUS2(tivxTaskCreate(&obj->right_capture_task, &obj->right_taskParams_capture));

    printf("waiting for right and left graph\n");
    tivxEventWait(obj->left_graph_setupComplete, TIVX_EVENT_TIMEOUT_WAIT_FOREVER);
    tivxEventWait(obj->right_graph_setupComplete, TIVX_EVENT_TIMEOUT_WAIT_FOREVER);
    
    printf("Start sensor streams\n");
    ERROR_CHECK_STATUS(appStartImageSensor(obj->left_sensor.sensor_name, CAPT1_INST_SENSOR_MASK));
    ERROR_CHECK_STATUS(appStartImageSensor(obj->right_sensor.sensor_name, CAPT2_INST_SENSOR_MASK));

    printf("Unblock tasks\n");
    tivxEventPost(obj->eventHandle_SensorCfgDone);
    tivxEventPost(obj->eventHandle_SensorCfgDone1);

    sleep(0.1);
    
    printf("Starting framesync\n");    
    appStartFrameSync();

    printf("waiting on left channel to finish\n");    
    //Wait for both Graph Processing to complete
    tivxEventWait(obj->eventHandle_Left_RxFinished, TIVX_EVENT_TIMEOUT_WAIT_FOREVER);
    printf("waiting on right channel to finish\n"); 
    tivxEventWait(obj->eventHandle_Right_RxFinished, TIVX_EVENT_TIMEOUT_WAIT_FOREVER);

    printf("Stopping framesync\n"); 
    appStopFrameSync();

    printf("deinit left histo object\n");
    app_deinit_histo(&obj->left_histo, APP_BUFFER_Q_DEPTH);
    printf("deinit right histo object\n");
    app_deinit_histo(&obj->right_histo, APP_BUFFER_Q_DEPTH);
    printf("deinit left sensor object\n");
    app_deinit_sensor(&obj->left_sensor);
    printf("deinit right sensor object\n");
    app_deinit_sensor(&obj->right_sensor);
    
    printf("release graphs\n");
    ERROR_CHECK_STATUS2(vxReleaseGraph(&obj->left_graph));
    ERROR_CHECK_STATUS2(vxReleaseGraph(&obj->right_graph));

    printf("release events\n");
    ERROR_CHECK_STATUS(tivxEventDelete(&obj->eventHandle_Left_RxFinished));
    tivxTaskDelete(&obj->left_capture_task);
   
    ERROR_CHECK_STATUS(tivxEventDelete(&obj->left_graph_setupComplete));
    ERROR_CHECK_STATUS(tivxEventDelete(&obj->right_graph_setupComplete));
    ERROR_CHECK_STATUS(tivxEventDelete(&obj->eventHandle_Right_RxFinished));
    ERROR_CHECK_STATUS(tivxEventDelete(&obj->eventHandle_SensorCfgDone));
    ERROR_CHECK_STATUS(tivxEventDelete(&obj->eventHandle_SensorCfgDone1));
    tivxTaskDelete(&obj->right_capture_task);

    printf("unload kernels\n");
    // remove merge kernel 
    ERROR_CHECK_STATUS2(vxRemoveKernel(obj->merge_kernel));
    tivxFileIOUnLoadKernels(obj->context);
    tivxImagingUnLoadKernels(obj->context);
    tivxHwaUnLoadKernels(obj->context);
    ERROR_CHECK_STATUS2(app_c66_kernels_unload(obj->context));

    ERROR_CHECK_STATUS(vxReleaseContext(&obj->context));
    
    printf("goodbye!\n");
    return status;
}

static vx_status VX_CALLBACK merge_histograms_kernel(vx_node node, const vx_reference *parameters, vx_uint32 num)
{
    printf("VXUINT: %d\n", num);
    if (num == 2) 
    {
        // Save to a file
        FILE* file = fopen("merged_histogram.txt", "w");
        vx_distribution left_histogram = (vx_distribution)parameters[0];
        vx_distribution right_histogram = (vx_distribution)parameters[1];

        // TODO: Merge the histograms as required. For demonstration, I'll simply write left histogram to a file.
        // You can modify this to merge histograms as needed.

        vx_size left_bins;
        vx_map_id left_map_id;
        vx_uint32 *left_hist_ptr = NULL;
        vxQueryDistribution(left_histogram, VX_DISTRIBUTION_BINS, &left_bins, sizeof(left_bins));

        vxMapDistribution(left_histogram, &left_map_id, (void**)&left_hist_ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);

        for (vx_size i = 0; i < left_bins; i++) {
            //printf("%lu: %u\n", i, left_hist_ptr[i]);
            fprintf(file, "%u\n", left_hist_ptr[i]);
        }

        vxUnmapDistribution(left_histogram, left_map_id);
        
        vx_size right_bins;
        vx_map_id right_map_id;
        vx_uint32 *right_hist_ptr = NULL;
        vxQueryDistribution(right_histogram, VX_DISTRIBUTION_BINS, &right_bins, sizeof(right_bins));

        // Access the right histogram
        vxMapDistribution(right_histogram, &right_map_id, (void**)&right_hist_ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);

        for (vx_size i = 0; i < right_bins; i++) {
            //printf("%lu: %u\n", i, right_hist_ptr[i]);
            fprintf(file, "%u\n", right_hist_ptr[i]);
        }

        vxUnmapDistribution(left_histogram, right_map_id);

        fclose(file);
    }
    
    return VX_SUCCESS;
}

static vx_status VX_CALLBACK merge_histograms_validator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
{
    // We can add validation logic here if needed. For now, simply returning success.
    return VX_SUCCESS;
}

static vx_status initialize_merge_histograms_kernel(AppObj *obj)
{
    obj->merge_kernel = vxAddUserKernel(
        obj->context,
        "org.openwater.merge_histograms",
        VX_KERNEL_MERGE_HISTOGRAMS,
        merge_histograms_kernel,
        2,
        merge_histograms_validator,
        NULL,
        NULL
    );

    ERROR_CHECK_OBJECT(obj->merge_kernel);

    ERROR_CHECK_STATUS(vxAddParameterToKernel(obj->merge_kernel, 0, VX_INPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED));
    ERROR_CHECK_STATUS(vxAddParameterToKernel(obj->merge_kernel, 1, VX_INPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED));

    ERROR_CHECK_STATUS(vxFinalizeKernel(obj->merge_kernel));
    return VX_SUCCESS;
}

static vx_status app_c66_kernels_load(vx_context context)
{
    vx_status status = VX_SUCCESS;
    #ifdef x86_64
    {
    }
    #endif
    
    tivxOwAlgosLoadKernels(context);
    return status;
}

static vx_status app_c66_kernels_unload(vx_context context)
{
    vx_status status = VX_SUCCESS;
    #ifdef x86_64
    
    #endif
    if(status == VX_SUCCESS)
    {
    }
    tivxOwAlgosUnLoadKernels(context);
    return status;
}
