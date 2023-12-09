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
#include <utils/remote_service/include/app_remote_service.h>
#include "app_sensor_module.h"
#include "app_cust_cap_module.h"
#include "app_histo_module.h"
#include "headSensorService.h"

#include <signal.h>

#include <fcntl.h>
#include <pthread.h> /* POSIX Threads */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#define CAPTURE1_ENABLE        (1U)
#define CAPTURE2_ENABLE        (1U)

#define HISTOGRAM_ENABLE       (1U)

#define MAX_NUM_BUF             (8u)
#define CAPTURE_MIN_PIPEUP_BUFS (3u)

#define NUM_CHANNELS        (3U)

#define APP_BUFFER_Q_DEPTH   (4)
#define APP_PIPELINE_DEPTH   (7)

#define CAPT1_INST_SENSOR_MASK      0x0B // ((1 << NUM_CHANNELS) - 1U)
#define CAPT2_INST_SENSOR_MASK      0xB0 // (((1 << NUM_CHANNELS) - 1U) << 4U)

#define CAPTURE1_NODE_ERROR  (1U)
#define CAPTURE2_NODE_ERROR  (1U)

/* Default is RAW image */
#define CAPTURE_FORMAT         (TIVX_TYPE_RAW_IMAGE)

#define ERROR_CHECK_STATUS( status ) { \
        vx_status status_ = (status); \
        if(status_ != VX_SUCCESS) { \
            printf("ERROR: failed with status = (%d) at " __FILE__ "#%d\n", status_, __LINE__); \
            exit(1); \
        } \
    }

//   ERROR_CHECK_OBJECT     - check whether the object creation is successful
#define ERROR_CHECK_OBJECT( obj ) { \
        vx_status status_ = vxGetStatus((vx_reference)(obj)); \
        if(status_ != VX_SUCCESS) { \
            printf("ERROR: failed with status = (%d) at " __FILE__ "#%d\n", status_, __LINE__); \
            exit(1); \
        } \
    }

static char usecase_menu[] = {
    "\n"
    "\n ========================="
    "\n Openwater : Dual Graph"
    "\n ========================="
    "\n"
    "\n 1-4: Switch to camera pair 1 through 4."
    "\n     Cameras are paired adjacently based on port index. Histogram enabled for the selected pair only."
    "\n"
    "\n h: Save histograms"
    "\n i: Save images"
    "\n p: Print performance statistics"
    "\n"
    "\n x: Exit"
    "\n"
};

typedef struct {
    /* OpenVX references */
    vx_context context;
    vx_graph graph;    
#if (CAPTURE1_ENABLE == 1U)
    SensorObj     left_sensor;
    CaptureObj    left_capture;
#if (HISTOGRAM_ENABLE == 1U)
    HistoObj      left_histo;
#endif
#endif
#if (CAPTURE2_ENABLE == 1U)
    SensorObj     right_sensor;
    CaptureObj    right_capture;
#if (HISTOGRAM_ENABLE == 1U)
    HistoObj      right_histo;
#endif
#endif
    
    tivx_task task;
    vx_uint32 stop_task;
    vx_uint32 stop_task_done;

    int32_t pipeline;
    int32_t enqueueCnt;
    int32_t dequeueCnt;

    vx_char output_file_path[APP_MAX_FILE_PATH];
    
    uint32_t width;
    uint32_t height;

    vx_uint32 num_histo_bins;
    vx_uint32 num_frames_to_write;
    vx_uint32 num_frames_to_skip;

    vx_uint32 num_histo_frames_to_write;
    vx_uint32 num_histo_frames_to_skip;
    int32_t currentActivePair; 
    int32_t previousActivePair;
    int32_t write_file;
    int32_t write_histo;
    int32_t scan_type;
    int32_t wait_histo;
    int32_t wait_file;
    int32_t wait_err;
    int32_t write_histo_l;
    int32_t write_histo_r;
    char wait_msg[256];
    char g_patient_path[64];
    char g_sub_path[32];
    char g_deviceID[24];

    app_perf_point_t total_perf;
    app_perf_point_t fileio_perf;

} AppObj;

AppObj gAppObj;

volatile sig_atomic_t sigkill_active = 0;

static uint32_t imageMean[6];

static vx_status app_init(AppObj *obj);
static vx_status app_create_graph(AppObj *obj);
static vx_status app_run_task_create(AppObj *obj);
static vx_status app_run_graph(AppObj *obj, uint64_t phys_ptr, void *virt_ptr);
static void app_run_task(void *app_var);
static vx_status app_run_graph_for_one_frame_pipeline(AppObj *obj, vx_int32 frame_id);

static void app_run_task_delete(AppObj *obj);
static void app_deinit(AppObj *obj);
static void app_delete_graph(AppObj *obj);

#if (HISTOGRAM_ENABLE == 1U)
static vx_status app_c66_kernels_load(vx_context context);
static vx_status app_c66_kernels_unload(vx_context context);
static int mkpath(const char *path, mode_t mode);
static bool owWriteImageToFile(AppObj *obj, uint32_t cap_frames, uint32_t skip_frames, bool enableGatekeeper, uint32_t timeout);
static bool owWriteHistoToFile(AppObj *obj, uint32_t hist_frames, uint32_t skip_frames, bool enableGatekeeper, uint32_t timeout);

static int mkpath(const char *path, mode_t mode)
{
	char tmp[PATH_MAX];
	char *p = NULL;
	size_t len;

	snprintf(tmp, sizeof(tmp),"%s",path);
	len = strlen(tmp);
	if(tmp[len - 1] == '/')
		tmp[len - 1] = 0;
	for(p = tmp + 1; *p; p++)
		if(*p == '/') {
			*p = 0;
			if (mkdir(tmp, mode) < 0 && errno != EEXIST)
				return -1;
			*p = '/';
		}
	if (mkdir(tmp, mode) < 0 && errno != EEXIST)
		return -1;
	return 0;
}

int32_t appRemoteServiceHeadSensorHandler(char *service_name, uint32_t cmd,
    void *prm, uint32_t prm_size, uint32_t flags)
{
    int32_t status = 0;
    gAppObj.wait_err = 0;
    memset(gAppObj.wait_msg, 0, 256);
    printf("####################### Received appRemoteServiceHeadSensorHandler\n");
    if (NULL != prm)
    {
        typeHEAD_SENSOR_MESSAGE* pHSM = prm;
        switch(cmd)
        {
            case HS_CMD_UPDATE_SENSOR_STATE:            
                if(pHSM->state == 0)
                {
                    imageMean[0] = pHSM->imageMean[0];
                    imageMean[1] = pHSM->imageMean[1];
                    imageMean[2] = pHSM->imageMean[2];
                    imageMean[3] = pHSM->imageMean[3];
                    imageMean[4] = pHSM->imageMean[4];
                    imageMean[5] = pHSM->imageMean[5];
                    gAppObj.wait_file = 0;
                }
                else
                {               
                    gAppObj.wait_file = 0;
                    gAppObj.wait_err = 1;
                }

                break;
            case HS_CMD_HISTO_RIGHT_DONE_STATE:
                gAppObj.write_histo_r = 1;
                if(pHSM->state != 0)
                {
                    printf("HISTO Write Failure: %s\n", pHSM->msg);
                    strcpy(gAppObj.wait_msg, pHSM->msg);
                    gAppObj.wait_err = 1;
                }
                if(gAppObj.write_histo_l && gAppObj.write_histo_r)
                {
                    gAppObj.wait_histo = 0;
                }
                break;
            case HS_CMD_HISTO_LEFT_DONE_STATE:
                gAppObj.write_histo_l = 1;
                if(pHSM->state != 0)
                {
                    printf("HISTO Write Failure: %s\n", pHSM->msg);
                    strcpy(gAppObj.wait_msg, pHSM->msg);
                    gAppObj.wait_err = 1;
                }
                if(gAppObj.write_histo_l && gAppObj.write_histo_r)
                {
                    gAppObj.wait_histo = 0;
                }
                break;
            case HS_CMD_HISTO_DONE_STATE:
                if(pHSM->state != 0)
                {
                    printf("HISTO Write Failure: %s\n", pHSM->msg);
                    strcpy(gAppObj.wait_msg, pHSM->msg);
                    gAppObj.wait_err = 1;
                }
                gAppObj.wait_histo = 0;
                break;
            case HS_CMD_FILE_DONE_STATE:
                if(pHSM->state != 0)
                {
                    printf("IMAGE Write Failure: %s\n", pHSM->msg);
                    strcpy(gAppObj.wait_msg, pHSM->msg);
                    gAppObj.wait_err = 1;
                }
                gAppObj.wait_file = 0;

                break;
            case HS_CMD_STAT_MSG:
                break;
            case HS_CMD_CAMERA_STATUS:
                break;
            default:
                printf("[APP_DUAL_GRAPH::appRemoteServiceHeadSensorHandler] Unhandled Command ID Recieved: %d\r", cmd);
                break;
        }

    }
    else
    {
        printf("ContactHandler: Invalid parameters passed !!!\n");
    }

    return status;
}


static bool owWriteHistoToFile(AppObj *obj, uint32_t hist_frames, uint32_t skip_frames, bool enableGatekeeper, uint32_t timeout)
{
    uint32_t timeout_count = 0;
    bool bSuccess = true;

    obj->num_histo_frames_to_write = hist_frames;  // assuming 25fps and 1sec capture  (capture seconds * framerate)
    obj->num_frames_to_skip = skip_frames;

    if(enableGatekeeper) obj->wait_histo = 1;
    obj->write_histo = 1;

    while(obj->write_histo != 0)
    {
        usleep(1000);
        if( timeout_count >= (timeout*1000))
        {
            // error
            printf("Timed out during frame histogram capture to disk\n");
            bSuccess = false;
            break;
        }

        timeout_count++;
    }

    if(enableGatekeeper){
        timeout_count = 0;
        while(obj->wait_histo != 0)
        {
            usleep(1000);
            if( timeout_count >= (timeout*1000))
            {
                // error
                printf("[GateKeeper] Timed out during wait for histo\n");
                bSuccess = false;
                break;
            }

            timeout_count++;

        }
    }

    if(obj->wait_err == 1)
    {
        // error was thrown during write of histogram data
        printf("[GateKeeper] Error writing histogram: %s\n", obj->wait_msg);
    }

    return bSuccess;

}

static bool owWriteImageToFile(AppObj *obj, uint32_t cap_frames, uint32_t skip_frames, bool enableGatekeeper, uint32_t timeout)
{
    uint32_t timeout_count = 0;
    bool bSuccess = true;

    obj->num_frames_to_write = cap_frames;
    obj->num_frames_to_skip = skip_frames;
    if(enableGatekeeper) obj->wait_file = 1;
    obj->write_file = 1;

    while(obj->write_file != 0)
    {
        usleep(1000);
        if( timeout_count >= (timeout*1000))
        {
            // error
            printf("[APP_DUAL_GRAPH::owWriteImageToFile] Timed out during frame capture to disk\n");
            bSuccess = false;
            break;
        }

        timeout_count++;
    }

    if(enableGatekeeper){

        timeout_count = 0;
        while(obj->wait_file != 0)
        {
            usleep(1000);
            if( timeout_count >= (timeout*1000))
            {
                // error
                printf("[GateKeeper] Timed out during wait for file\n");
                bSuccess = false;
                break;
            }

            timeout_count++;
        }

    }

    if(obj->wait_err == 1)
    {
        // error was thrown during write to file
        printf("[GateKeeper] Error writing to file: %s\n", obj->wait_msg);
    }


    return bSuccess;
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

#endif

static void add_graph_parameter_by_node_index(vx_graph graph, vx_node node, vx_uint32 node_parameter_index);

static void add_graph_parameter_by_node_index(vx_graph graph, vx_node node, vx_uint32 node_parameter_index)
{
    vx_parameter parameter = vxGetParameterByIndex(node, node_parameter_index);

    vxAddParameterToGraph(graph, parameter);
    vxReleaseParameter(&parameter);
}

static vx_status app_init(AppObj *obj)
{
    vx_status status = VX_SUCCESS;

#if (CAPTURE1_ENABLE == 1U)
#if (HISTOGRAM_ENABLE == 1U)
    // set histo output path
    memset(obj->left_histo.output_file_path, 0, APP_MAX_FILE_PATH);
    sprintf(obj->left_histo.output_file_path, "%s", "/home/root/app_dual_graph");
#endif
    // sensor config
    obj->left_sensor.sensor_index = 7;
    obj->left_sensor.enable_ldc = 0;
    obj->left_sensor.num_cameras_enabled = 1;
    obj->left_sensor.ch_mask = CAPT1_INST_SENSOR_MASK;
    obj->left_sensor.usecase_option = APP_SENSOR_FEATURE_CFG_UC0;
    ERROR_CHECK_STATUS(app_querry_sensor(&obj->left_sensor));
#endif

#if (CAPTURE2_ENABLE == 1U)
#if (HISTOGRAM_ENABLE == 1U)
    memset(obj->right_histo.output_file_path, 0, APP_MAX_FILE_PATH);
    sprintf(obj->right_histo.output_file_path, "%s", "/home/root/app_dual_graph");
#endif
    obj->right_sensor.sensor_index = 7;
    obj->right_sensor.enable_ldc = 0;
    obj->right_sensor.num_cameras_enabled = 1;
    obj->right_sensor.ch_mask = CAPT2_INST_SENSOR_MASK;
    obj->right_sensor.usecase_option = APP_SENSOR_FEATURE_CFG_UC0;
    ERROR_CHECK_STATUS(app_querry_sensor(&obj->right_sensor));
#endif

    // init
    obj->pipeline       = -APP_BUFFER_Q_DEPTH + 1;
    obj->enqueueCnt     = 0;
    obj->dequeueCnt     = 0;

    
    printf("creating context\n");
    obj->context = vxCreateContext();    
    ERROR_CHECK_STATUS(vxGetStatus((vx_reference)obj->context));

    printf("loading kernels\n");
#if (HISTOGRAM_ENABLE == 1U)
    ERROR_CHECK_STATUS(app_c66_kernels_load(obj->context));
#endif
    tivxHwaLoadKernels(obj->context);
    tivxImagingLoadKernels(obj->context);
    tivxFileIOLoadKernels(obj->context);

    // initialize frame sync       
    printf("initializing framesync\n"); 
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
        printf("[APP_DUAL_GRAPH] Error initializing D3 board\n");
        return VX_FAILURE;
    }
    
    return status;
}

static vx_status app_create_graph(AppObj *obj)
{
    vx_status status = VX_SUCCESS;

    vx_graph_parameter_queue_params_t graph_parameters_queue_params_list[9];
    vx_int32 graph_parameter_index;

    printf("creating graph\n"); 
    obj->graph = vxCreateGraph(obj->context);
    ERROR_CHECK_OBJECT(obj->graph);
    ERROR_CHECK_STATUS(vxSetReferenceName((vx_reference)obj->graph, "app_dual_graph"));

    appPerfPointSetName(&obj->total_perf , "TOTAL");
    appPerfPointSetName(&obj->fileio_perf, "FILEIO");

    printf("init enabled channels\n");
    graph_parameter_index = 0;

#if (CAPTURE1_ENABLE == 1U)
    ERROR_CHECK_STATUS(app_init_sensor(&obj->left_sensor, "left_sensor"));
    obj->width = obj->left_sensor.image_width;
    obj->height = obj->left_sensor.image_height;
    printf("Sensor Image Size %dx%d\n", obj->width, obj->height);
    printf("init left capture object\n");    
    ERROR_CHECK_STATUS(app_init_custom_capture(obj->context, &obj->left_capture, &obj->left_sensor, "left_capture", APP_BUFFER_Q_DEPTH, CAPT1_INST_ID));
    // Create nodes and add to graph
    printf("create left capture node\n");    
    ERROR_CHECK_STATUS(app_create_custom_graph_capture(obj->graph, &obj->left_capture, CAPT1_INST_ID)); 
    
#if (HISTOGRAM_ENABLE == 1U)
    vx_object_array raw_left_input;
    raw_left_input = obj->left_capture.raw_image_arr[0];
    printf("init left histo object\n");    
    obj->left_histo.params.enableChanBitFlag = CAPT1_INST_SENSOR_MASK;
    ERROR_CHECK_STATUS(app_init_histo(obj->context, &obj->left_histo, &obj->left_sensor, "left_histo", APP_BUFFER_Q_DEPTH, 1024, NUM_CHANNELS, CAPT1_INST_ID));
    printf("create left histo node\n"); 
    ERROR_CHECK_STATUS(app_create_graph_histo(obj->graph, &obj->left_histo, raw_left_input, CAPT1_INST_ID));
#endif

    add_graph_parameter_by_node_index(obj->graph, obj->left_capture.node, 1);
    obj->left_capture.graph_parameter_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference *)&obj->left_capture.raw_image_arr[0];
    graph_parameter_index++;
    
#endif
#if (CAPTURE2_ENABLE == 1U)
    ERROR_CHECK_STATUS(app_init_sensor(&obj->right_sensor, "right_sensor"));
    obj->width = obj->right_sensor.image_width;
    obj->height = obj->right_sensor.image_height;
    printf("Sensor Image Size %dx%d\n", obj->width, obj->height);
    printf("init right capture object\n");    
    ERROR_CHECK_STATUS(app_init_custom_capture(obj->context, &obj->right_capture, &obj->right_sensor, "right_capture", APP_BUFFER_Q_DEPTH, CAPT2_INST_ID));
    // Create nodes and add to graph
    printf("create right capture node\n");    
    ERROR_CHECK_STATUS(app_create_custom_graph_capture(obj->graph, &obj->right_capture, CAPT2_INST_ID));
    
#if (HISTOGRAM_ENABLE == 1U)
    vx_object_array raw_right_input;
    raw_right_input = obj->right_capture.raw_image_arr[0];    
    printf("init right histo object\n");    
    obj->right_histo.params.enableChanBitFlag = ((CAPT2_INST_SENSOR_MASK >> 4)& 0x0F);
    ERROR_CHECK_STATUS(app_init_histo(obj->context, &obj->right_histo, &obj->right_sensor, "right_histo", APP_BUFFER_Q_DEPTH, 1024, NUM_CHANNELS, CAPT2_INST_ID));
    printf("create right histo node\n"); 
    ERROR_CHECK_STATUS(app_create_graph_histo(obj->graph, &obj->right_histo, raw_right_input, CAPT2_INST_ID));
#endif

    add_graph_parameter_by_node_index(obj->graph, obj->right_capture.node, 1);
    obj->right_capture.graph_parameter_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference *)&obj->right_capture.raw_image_arr[0];
    graph_parameter_index++;
    
#endif

#if (CAPTURE1_ENABLE == 1U && HISTOGRAM_ENABLE == 1U) 
    add_graph_parameter_by_node_index(obj->graph, obj->left_histo.node, TIVX_KERNEL_OW_CALC_HISTO_HISTO_IDX);
    obj->left_histo.graph_parameter_distribution_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference*)&obj->left_histo.outHistoCh0[0];
    graph_parameter_index++;

    add_graph_parameter_by_node_index(obj->graph, obj->left_histo.node, TIVX_KERNEL_OW_CALC_HISTO_MEAN_IDX);
    obj->left_histo.graph_parameter_mean_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference*)&obj->left_histo.outMeanCh0[0];
    graph_parameter_index++;

    add_graph_parameter_by_node_index(obj->graph, obj->left_histo.node, TIVX_KERNEL_OW_CALC_HISTO_SD_IDX);
    obj->left_histo.graph_parameter_sd_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference *)&obj->left_histo.outSdCh0[0];
    graph_parameter_index++;
#endif
    
#if (CAPTURE2_ENABLE == 1U && HISTOGRAM_ENABLE == 1U) 
    add_graph_parameter_by_node_index(obj->graph, obj->right_histo.node, TIVX_KERNEL_OW_CALC_HISTO_HISTO_IDX);
    obj->right_histo.graph_parameter_distribution_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference*)&obj->right_histo.outHistoCh0[0];
    graph_parameter_index++;

    add_graph_parameter_by_node_index(obj->graph, obj->right_histo.node, TIVX_KERNEL_OW_CALC_HISTO_MEAN_IDX);
    obj->right_histo.graph_parameter_mean_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference*)&obj->right_histo.outMeanCh0[0];
    graph_parameter_index++;

    add_graph_parameter_by_node_index(obj->graph, obj->right_histo.node, TIVX_KERNEL_OW_CALC_HISTO_SD_IDX);
    obj->right_histo.graph_parameter_sd_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
    graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference *)&obj->right_histo.outSdCh0[0];
    graph_parameter_index++;
#endif    

    printf("schedule graph\n");
    ERROR_CHECK_STATUS(vxSetGraphScheduleConfig(obj->graph,
        VX_GRAPH_SCHEDULE_MODE_QUEUE_AUTO,
        graph_parameter_index,
        graph_parameters_queue_params_list));
    
    printf("verify graph\n");
    ERROR_CHECK_STATUS(vxVerifyGraph(obj->graph));
    
    printf("output graph image\n");
    ERROR_CHECK_STATUS(tivxExportGraphToDot(obj->graph,".", "app_dual_graph"));

#if (CAPTURE1_ENABLE == 1U)
    if ((obj->left_capture.enable_error_detection) )
    {
        status = app_send_error_frame(&obj->left_capture);
    }
#endif
#if (CAPTURE2_ENABLE == 1U)
    if ((obj->right_capture.enable_error_detection) )
    {
        status = app_send_error_frame(&obj->right_capture);
    }
#endif
    return status;
}

static void app_run_task(void *app_var)
{
    AppObj *obj = (AppObj *)app_var;
    volatile vx_status status = VX_SUCCESS;
    void *virt_ptr;
    uint64_t phys_ptr;
    uint32_t mem_size = 1024;

    virt_ptr = appMemAlloc(APP_MEM_HEAP_DDR, mem_size, 32);
    if(NULL == virt_ptr)
    {
        printf("appGetTempCameraPair: failed invalid memory pointer!!!\n");
        obj->stop_task_done = 1;
        return;
    }

    phys_ptr = appMemGetVirt2PhyBufPtr((uint64_t)(uintptr_t)virt_ptr, APP_MEM_HEAP_DDR);
    
    while((obj->stop_task == 0) && (status == VX_SUCCESS))
    {
        status = app_run_graph(obj, phys_ptr, virt_ptr);
    }
    appMemFree(APP_MEM_HEAP_DDR, virt_ptr, mem_size);
    obj->stop_task_done = 1;
}

static void app_run_task_delete(AppObj *obj)
{    
    obj->stop_task = 1;
    printf("-------------> DELETE TASK\n");
    // delete task
    while(obj->stop_task_done==0)
    {
        tivxTaskWaitMsecs(100);
    }

    tivxTaskDelete(&obj->task);
}

static void app_delete_graph(AppObj *obj)
{
#if (CAPTURE1_ENABLE == 1U)
    printf("delete left\n");
    app_delete_custom_capture(&obj->left_capture);
#endif
#if (CAPTURE2_ENABLE == 1U)
    printf("delete right\n");
    app_delete_custom_capture(&obj->right_capture);
#endif

    printf("release graph\n");
    vxReleaseGraph(&obj->graph);
}

static void app_deinit(AppObj *obj)
{
    appDeInitD3board();

#if (CAPTURE1_ENABLE == 1U)
    printf("deinit left\n");
#if (HISTOGRAM_ENABLE == 1U)  
    app_deinit_histo(&obj->left_histo, APP_BUFFER_Q_DEPTH);
#endif
    app_deinit_custom_capture(&obj->left_capture, APP_BUFFER_Q_DEPTH);
    app_deinit_sensor(&obj->left_sensor);
#endif
#if (CAPTURE2_ENABLE == 1U)
    printf("deinit right\n");
#if (HISTOGRAM_ENABLE == 1U)  
    app_deinit_histo(&obj->right_histo, APP_BUFFER_Q_DEPTH);
#endif
    app_deinit_custom_capture(&obj->right_capture,APP_BUFFER_Q_DEPTH);
    app_deinit_sensor(&obj->right_sensor);
#endif

    printf("unload kernels\n");
    tivxFileIOUnLoadKernels(obj->context);
    tivxImagingUnLoadKernels(obj->context);
    tivxHwaUnLoadKernels(obj->context);
#if (HISTOGRAM_ENABLE == 1U)  
    app_c66_kernels_unload(obj->context);
#endif
    
    printf("release context\n");
    vxReleaseContext(&obj->context);
}

static vx_status app_run_task_create(AppObj *obj)
{
    tivx_task_create_params_t params;
    vx_status status = VX_SUCCESS;

    // create and run task
    tivxTaskSetDefaultCreateParams(&params);
    params.task_main = app_run_task;
    params.app_var = obj;

    obj->stop_task_done = 0;
    obj->stop_task = 0;
    
    printf("+++++++++++++> CREATE TASK\n");
    status = tivxTaskCreate(&obj->task, &params);

    return status;
}

static vx_status app_run_graph(AppObj *obj, uint64_t phys_ptr, void *virt_ptr)
{
    vx_status status = VX_SUCCESS;
    vx_int32 frame_id;
    uint16_t des_mask = 0;
    printf("Start sensor streams\n");
#if (CAPTURE1_ENABLE == 1U)
    ERROR_CHECK_STATUS(appStartImageSensor(obj->left_sensor.sensor_name, CAPT1_INST_SENSOR_MASK));
    des_mask = CAPT1_INST_SENSOR_MASK & 0x0F;
#endif
#if (CAPTURE2_ENABLE == 1U)
    ERROR_CHECK_STATUS(appStartImageSensor(obj->right_sensor.sensor_name, CAPT2_INST_SENSOR_MASK));
    des_mask |= (CAPT2_INST_SENSOR_MASK & 0xF0);
#endif

    printf("Set Camera Forwarding and Mask\n");
#if (HISTOGRAM_ENABLE == 1U)
#if (CAPTURE1_ENABLE == 1U)
    ERROR_CHECK_STATUS(app_send_cmd_histo_change_pair(&obj->left_histo, (CAPT1_INST_SENSOR_MASK & 0x0F)));
    ERROR_CHECK_STATUS(app_send_cmd_histo_enableChan(&obj->left_histo, 0x0F));
#endif
#if (CAPTURE2_ENABLE == 1U)
    ERROR_CHECK_STATUS(app_send_cmd_histo_change_pair(&obj->right_histo, ((CAPT2_INST_SENSOR_MASK >> 4 )& 0x0F)));
    ERROR_CHECK_STATUS(app_send_cmd_histo_enableChan(&obj->right_histo, 0x0F));
#endif
#endif
    ERROR_CHECK_STATUS(appActivateCameraMask(des_mask));
    obj->currentActivePair = 1;
    obj->currentActivePair = 1;

    // start framesync pulse
    /* wait a while for prints to flush */
    tivxTaskWaitMsecs(250);
    if(des_mask !=0){
        appStartFrameSync();
    }
    else
    {
        printf("Trying to run graph with no cameras enabled\n");
        status = VX_FAILURE;
    }
    
    for(frame_id = 0; ; frame_id++)
    {

        //TODO: Read Temps
#if 0        
        // every five frames, update the cam_temp_vals
        cam_temp_vals = 0;
        if (frame_id % 5 == 0) {
            if (obj->currentActivePair > 0 && obj->currentActivePair < 5) {
                pair_mask = app_get_pair_mask(obj, obj->currentActivePair, obj->currentActiveScanType);
                cam_temp_vals = appGetTempCameraPair(pair_mask, phys_ptr, virt_ptr);
                if(!cam_temp_vals) printf("Could not read cam temp values\n");
            }
        }
#endif

        //TODO: check for write image
        if(obj->write_file == 1)
        {
            obj->write_file = 0;
            printf("write file started \n");
#if (CAPTURE1_ENABLE == 1U)            
            app_send_cmd_capture_write_node(&obj->left_capture, 0x3F, frame_id + 1, obj->num_frames_to_write, obj->num_frames_to_skip,  obj->scan_type, obj->g_patient_path, obj->g_sub_path);
#endif
#if (CAPTURE2_ENABLE == 1U)            
            app_send_cmd_capture_write_node(&obj->right_capture, 0x3F, frame_id + 1, obj->num_frames_to_write, obj->num_frames_to_skip,  obj->scan_type, obj->g_patient_path, obj->g_sub_path);
#endif
            printf("write file finished \n");
        }

        // check for write histo
#if (HISTOGRAM_ENABLE == 1U)
        if(obj->write_histo == 1)
        {
            obj->write_histo = 0;
            printf("write histo start \n");
#if (CAPTURE1_ENABLE == 1U)            
            status = app_send_cmd_histo_write_node(&obj->left_histo, 0x3F, frame_id+1, obj->num_histo_frames_to_write, obj->num_histo_frames_to_skip, obj->currentActivePair, obj->scan_type, obj->g_patient_path, obj->g_sub_path, virt_ptr); /* we use frame_id+1 because we observed there could be a delay of 1 frame before the command is received by the node */
#endif
#if (CAPTURE2_ENABLE == 1U)   
            status = app_send_cmd_histo_write_node(&obj->right_histo, 0x3F, frame_id+1, obj->num_histo_frames_to_write, obj->num_histo_frames_to_skip, obj->currentActivePair, obj->scan_type, obj->g_patient_path, obj->g_sub_path, virt_ptr); /* we use frame_id+1 because we observed there could be a delay of 1 frame before the command is received by the node */
#endif
            printf("write histo finished \n");
        }
#endif

        // run pipeline for one image
        if (status == VX_SUCCESS)
        {
            status = app_run_graph_for_one_frame_pipeline(obj, frame_id);
        }
        else
        {            
            printf("+++++++++++++++++++++++++++> GRAPH RUN FAILED STOPPING GRAPH\n");
            obj->stop_task = 1;
        }
        /* user asked to stop processing */
        if(obj->stop_task)
          break;
    }

    if (status == VX_SUCCESS)
    {
        status = vxWaitGraph(obj->graph);
    }

    // turn off frame sync pulse
    appStopFrameSync();
    
    // turn off cameras
#if (CAPTURE1_ENABLE == 1U)
    printf("Stopping left sensor\n");
    ERROR_CHECK_STATUS(appStopImageSensor(obj->left_sensor.sensor_name, CAPT1_INST_SENSOR_MASK)); 
#endif
#if (CAPTURE2_ENABLE == 1U)
    printf("Stopping right sensor\n");
    ERROR_CHECK_STATUS(appStopImageSensor(obj->right_sensor.sensor_name, CAPT2_INST_SENSOR_MASK)); 
#endif
    printf("Stopping forwarding\n");
    ERROR_CHECK_STATUS(appActivateCameraMask(0));

    return status;
}

static vx_status app_run_graph_for_one_frame_pipeline(AppObj *obj, vx_int32 frame_id)
{
    vx_status status = VX_SUCCESS;

    appPerfPointBegin(&obj->total_perf);

    if(obj->pipeline < 0)
    {
        /* Enqueue inputs during pipeup dont execute */
#if (CAPTURE1_ENABLE == 1U)
        if (status == VX_SUCCESS)
        {
            status = tivxGraphParameterEnqueueReadyRef(obj->graph, obj->left_capture.graph_parameter_index, (vx_reference *)&obj->left_capture.raw_image_arr[obj->enqueueCnt], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP);             
        }
#if (HISTOGRAM_ENABLE == 1U)
        if (status == VX_SUCCESS)
        {
            status = tivxGraphParameterEnqueueReadyRef(obj->graph, obj->left_histo.graph_parameter_distribution_index, (vx_reference*)&obj->left_histo.outHistoCh0[obj->enqueueCnt], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP); 
            status |= tivxGraphParameterEnqueueReadyRef(obj->graph, obj->left_histo.graph_parameter_mean_index, (vx_reference*)&obj->left_histo.outMeanCh0[obj->enqueueCnt], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP); 
            status |= tivxGraphParameterEnqueueReadyRef(obj->graph, obj->left_histo.graph_parameter_sd_index, (vx_reference*)&obj->left_histo.outSdCh0[obj->enqueueCnt], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP); 
        }
#endif
        if (status != VX_SUCCESS)
        {
            printf("left channel enqueue buffers failed\n");
        }
#endif
#if (CAPTURE2_ENABLE == 1U)    
        if (status == VX_SUCCESS)
        {    
            status = tivxGraphParameterEnqueueReadyRef(obj->graph, obj->right_capture.graph_parameter_index, (vx_reference *)&obj->right_capture.raw_image_arr[obj->enqueueCnt], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP);             
        }
#if (HISTOGRAM_ENABLE == 1U)
        if (status == VX_SUCCESS)
        {
            status = tivxGraphParameterEnqueueReadyRef(obj->graph, obj->right_histo.graph_parameter_distribution_index, (vx_reference*)&obj->right_histo.outHistoCh0[obj->enqueueCnt], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP); 
            status |= tivxGraphParameterEnqueueReadyRef(obj->graph, obj->right_histo.graph_parameter_mean_index, (vx_reference*)&obj->right_histo.outMeanCh0[obj->enqueueCnt], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP); 
            status |= tivxGraphParameterEnqueueReadyRef(obj->graph, obj->right_histo.graph_parameter_sd_index, (vx_reference*)&obj->right_histo.outSdCh0[obj->enqueueCnt], 1, TIVX_GRAPH_PARAMETER_ENQUEUE_FLAG_PIPEUP); 
        }
#endif 
        if (status != VX_SUCCESS)
        {
            printf("right channel enqueue buffers failed\n");
        }
#endif
        
        obj->enqueueCnt++;
        obj->enqueueCnt   = (obj->enqueueCnt  >= APP_BUFFER_Q_DEPTH)? 0 : obj->enqueueCnt;
        obj->pipeline++;
    }

    if((obj->pipeline == 0) && (status == VX_SUCCESS))
    {
        /* Execute 1st frame */
#if (CAPTURE1_ENABLE == 1U)
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, obj->left_capture.graph_parameter_index, (vx_reference *)&obj->left_capture.raw_image_arr[obj->enqueueCnt], 1);             
        }
#if (HISTOGRAM_ENABLE == 1U)
        if (status == VX_SUCCESS)
        {
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->left_histo.graph_parameter_distribution_index, (vx_reference*)&obj->left_histo.outHistoCh0[obj->enqueueCnt], 1); 
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->left_histo.graph_parameter_mean_index, (vx_reference*)&obj->left_histo.outMeanCh0[obj->enqueueCnt], 1); 
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->left_histo.graph_parameter_sd_index, (vx_reference*)&obj->left_histo.outSdCh0[obj->enqueueCnt], 1); 
        }
#endif
        if (status != VX_SUCCESS)
        {
            printf("left channel enqueue buffers failed Execute 1st frame \n");
        }
#endif
#if (CAPTURE2_ENABLE == 1U)    
        if (status == VX_SUCCESS)
        {    
            status = vxGraphParameterEnqueueReadyRef(obj->graph, obj->right_capture.graph_parameter_index, (vx_reference *)&obj->right_capture.raw_image_arr[obj->enqueueCnt], 1);             
        }
#if (HISTOGRAM_ENABLE == 1U)
        if (status == VX_SUCCESS)
        {
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->right_histo.graph_parameter_distribution_index, (vx_reference*)&obj->right_histo.outHistoCh0[obj->enqueueCnt], 1); 
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->right_histo.graph_parameter_mean_index, (vx_reference*)&obj->right_histo.outMeanCh0[obj->enqueueCnt], 1); 
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->right_histo.graph_parameter_sd_index, (vx_reference*)&obj->right_histo.outSdCh0[obj->enqueueCnt], 1); 
        }
#endif 
        if (status != VX_SUCCESS)
        {
            printf("right channel enqueue buffers failed Execute 1st frame \n");
        }
#endif

        obj->enqueueCnt++;
        obj->enqueueCnt   = (obj->enqueueCnt  >= APP_BUFFER_Q_DEPTH)? 0 : obj->enqueueCnt;
        obj->pipeline++;
    }

    if((obj->pipeline > 0) && (status == VX_SUCCESS))
    {
        uint32_t num_refs;
#if (CAPTURE1_ENABLE == 1U)
        vx_object_array captured_left_frames = NULL;
#if (HISTOGRAM_ENABLE == 1U)
        vx_distribution output_left_histo = NULL;
        vx_scalar output_left_mean = NULL;
        vx_scalar output_left_sd = NULL;
#endif
#endif
#if (CAPTURE2_ENABLE == 1U)
        vx_object_array captured_right_frames = NULL;
#if (HISTOGRAM_ENABLE == 1U)
        vx_distribution output_right_histo = NULL;
        vx_scalar output_right_mean = NULL;
        vx_scalar output_right_sd = NULL;
#endif
#endif
        /* Dequeue input */
#if (CAPTURE1_ENABLE == 1U)
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterDequeueDoneRef(obj->graph, obj->left_capture.graph_parameter_index, (vx_reference*)&captured_left_frames, 1, &num_refs);
            // printf("Dequeued Count: %d\n", num_refs);
            // vx_size item_count = 0;
            // status = vxQueryObjectArray(captured_left_frames, VX_OBJECT_ARRAY_NUMITEMS, &item_count, sizeof(item_count));
            // if(status == VX_SUCCESS){
            //     printf("Images %ld\n", item_count);
            //     for (vx_uint32 i = 0; i < item_count; i++) {                    
            //         vx_uint32 width, height;
            //         vx_reference rawImage = vxGetObjectArrayItem(captured_left_frames, i);
            //         tivxQueryRawImage((tivx_raw_image)rawImage, TIVX_RAW_IMAGE_WIDTH, &width, sizeof(vx_uint32));
            //         tivxQueryRawImage((tivx_raw_image)rawImage, TIVX_RAW_IMAGE_HEIGHT, &height, sizeof(vx_uint32));
            //         vxReleaseReference(&rawImage);
            //         printf("Image Size %dx%d\n", width, height);
            //     }
            // }

        }
#if (HISTOGRAM_ENABLE == 1U)        
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterDequeueDoneRef(obj->graph, obj->left_histo.graph_parameter_distribution_index, (vx_reference *)&output_left_histo, 1, &num_refs);
            status |= vxGraphParameterDequeueDoneRef(obj->graph, obj->left_histo.graph_parameter_mean_index, (vx_reference *)&output_left_mean, 1, &num_refs);
            status |= vxGraphParameterDequeueDoneRef(obj->graph, obj->left_histo.graph_parameter_sd_index, (vx_reference *)&output_left_sd, 1, &num_refs);
            // printf("Dequeued Count: %d\n", num_refs);
            // vx_size item_count = 0;
            // status = vxQueryObjectArray(output_left_histo, VX_OBJECT_ARRAY_NUMITEMS, &item_count, sizeof(item_count));
            // if(status == VX_SUCCESS){
            //     printf("Distributions %ld\n", item_count);
            // }

            // vx_size hist_num_bins;
            // status = vxQueryDistribution(output_left_histo, VX_DISTRIBUTION_BINS, &hist_num_bins, sizeof(hist_num_bins));
            // if(status == VX_SUCCESS){
            //     printf("HIST BINS %ld\n", hist_num_bins);
            // }

        }
#endif
        if (status != VX_SUCCESS)
        {
            printf("left vxGraphParameterDequeueDoneRef failed\n");
        }
#endif
#if (CAPTURE2_ENABLE == 1U)        
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterDequeueDoneRef(obj->graph, obj->right_capture.graph_parameter_index, (vx_reference*)&captured_right_frames, 1, &num_refs);    
        }
#if (HISTOGRAM_ENABLE == 1U)  
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterDequeueDoneRef(obj->graph, obj->right_histo.graph_parameter_distribution_index, (vx_reference *)&output_right_histo, 1, &num_refs);
            status |= vxGraphParameterDequeueDoneRef(obj->graph, obj->right_histo.graph_parameter_mean_index, (vx_reference *)&output_right_mean, 1, &num_refs);
            status |= vxGraphParameterDequeueDoneRef(obj->graph, obj->right_histo.graph_parameter_sd_index, (vx_reference *)&output_right_sd, 1, &num_refs);
        }
#endif
        if (status != VX_SUCCESS)
        {
            printf("right vxGraphParameterDequeueDoneRef failed\n");
        }
#endif
        /* Enqueue input - start execution */
#if (CAPTURE1_ENABLE == 1U)
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, obj->left_capture.graph_parameter_index, (vx_reference*)&captured_left_frames, 1);
        }
#if (HISTOGRAM_ENABLE == 1U)  
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, obj->left_histo.graph_parameter_distribution_index, (vx_reference*)&output_left_histo, 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->left_histo.graph_parameter_mean_index, (vx_reference*)&output_left_mean, 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->left_histo.graph_parameter_sd_index, (vx_reference*)&output_left_sd, 1);
        }
#endif
        if (status != VX_SUCCESS)
        {
            printf("left vxGraphParameterEnqueueReadyRef failed\n");
        }
#endif
#if (CAPTURE2_ENABLE == 1U)
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, obj->right_capture.graph_parameter_index, (vx_reference*)&captured_right_frames, 1);
        }
#if (HISTOGRAM_ENABLE == 1U)  
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, obj->right_histo.graph_parameter_distribution_index, (vx_reference*)&output_right_histo, 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->right_histo.graph_parameter_mean_index, (vx_reference*)&output_right_mean, 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->right_histo.graph_parameter_sd_index, (vx_reference*)&output_right_sd, 1);
        }
#endif
        if (status != VX_SUCCESS)
        {
            printf("right vxGraphParameterEnqueueReadyRef failed\n");
        }
#endif

        obj->enqueueCnt++;
        obj->dequeueCnt++;

        obj->enqueueCnt = (obj->enqueueCnt >= APP_BUFFER_Q_DEPTH)? 0 : obj->enqueueCnt;
        obj->dequeueCnt = (obj->dequeueCnt >= APP_BUFFER_Q_DEPTH)? 0 : obj->dequeueCnt;
    }

    appPerfPointEnd(&obj->total_perf);
    return status;
}

// Handler for system signals (such as SIGKILL)
void sigkill_handler(int signum)
{
    sigkill_active = 1;
}

int app_dual_graph_main(int argc, char* argv[])
{
    printf("Executing app_dual_graph_main\n");
    AppObj *obj = &gAppObj;
    vx_status status = VX_SUCCESS;
    struct stat st = {0};
    uint32_t done = 0;
    char current_scan_path[1024] = {0};
    char ch;
    // Setup up handler for SIGKILL signal to take the app down gracefully
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sigkill_handler;
    sigaction(SIGTERM, &action, NULL);

    // set outpur filepath
    memset(obj->output_file_path, 0, APP_MAX_FILE_PATH);
    sprintf(obj->output_file_path, "%s", "/home/root/app_dual_graph");

    printf("Creating output directory %s\n", obj->output_file_path);
    if (stat(obj->output_file_path, &st) == -1)
    {
        if (mkpath(obj->output_file_path, 0755) == -1) {            
            printf("Some error trying to create output directory Error: %s\n", strerror(errno));
        }
    }

    obj->write_file = 0;
    obj->write_histo = 0;
    obj->scan_type = 0;
    obj->wait_histo = 0;
    obj->wait_file = 0;
    obj->num_histo_bins= 1024;
    
    obj->currentActivePair= -1; // Activate all cameras initially
    obj->previousActivePair= -1;

    memset(current_scan_path, 0, 1024);
    
#if (CAPTURE1_ENABLE == 1U)
    obj->left_capture.en_out_capture_write = 1;
    memset(obj->left_capture.output_file_path, 0, APP_MAX_FILE_PATH);
    strcpy(obj->left_capture.output_file_path, obj->output_file_path);
#endif
#if (CAPTURE2_ENABLE == 1U)
    obj->right_capture.en_out_capture_write = 1;
    memset(obj->right_capture.output_file_path, 0, APP_MAX_FILE_PATH);
    strcpy(obj->right_capture.output_file_path, obj->output_file_path);
#endif

    time_t current_time;
	/* Obtain current time. */
    current_time = time(NULL);
    memset(obj->g_patient_path,0,64);
    struct tm tm = *localtime(&current_time);    
    sprintf(obj->g_patient_path, "%04d_%02d_%02d_%02d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);    
    sprintf(obj->g_sub_path, "Interactive");  

    sprintf(current_scan_path, "%s/%s/%s", obj->output_file_path, obj->g_patient_path, obj->g_sub_path);
    
    printf("Creating output directory %s\n", current_scan_path);
    if (stat(current_scan_path, &st) == -1)
    {
        if (mkpath(current_scan_path, 0755) == -1) {
            printf("Error: %s\n", strerror(errno));            
            return false;
        }
    } 
    
    printf("[APP_DUAL_GRAPH::set_patient_id] Directory Path for data: %s\n", current_scan_path);
    
    
    ERROR_CHECK_STATUS(appRemoteServiceRegister((char*)HEAD_SENSOR_SERVICE_NAME, appRemoteServiceHeadSensorHandler));
    ERROR_CHECK_STATUS(app_init(obj));

    /* wait a while for prints to flush */
    tivxTaskWaitMsecs(500);

    ERROR_CHECK_STATUS(app_create_graph(obj));

    /* wait a while for prints to flush */
    tivxTaskWaitMsecs(250);

    printf("Creating Task\n");
    ERROR_CHECK_STATUS(app_run_task_create(obj));

    appPerfStatsResetAll();
    
    tivxTaskWaitMsecs(1000);

    while(!done)
    {
        printf(usecase_menu);
        do {
            ch = appGetChar();
        } while(ch == '\n' || ch == '\r');
        printf("\n");

        switch(ch)
        {
            case '1':
            case '2':
            case '3':
            case '4':
                break;
            case 'h':
                owWriteHistoToFile(obj, 1, 0, true, 5);
                break;
            case 'i':
                owWriteImageToFile(obj, 1, 0, true, 5);
                break;
            case 'p':
                appPerfStatsPrintAll();
                status = tivx_utils_graph_perf_print(obj->graph);
                appPerfPointPrint(&obj->fileio_perf);
                appPerfPointPrint(&obj->total_perf);
                printf("\n");
                appPerfPointPrintFPS(&obj->total_perf);
                appPerfPointReset(&obj->total_perf);
                printf("\n");
                
#if (CAPTURE1_ENABLE == 1U)
                vx_reference left_refs[1];
                left_refs[0] = (vx_reference)obj->left_capture.raw_image_arr[0];
                if (status == VX_SUCCESS)
                {
                    status = tivxNodeSendCommand(obj->left_capture.node, 0u,
                                                TIVX_CAPTURE_PRINT_STATISTICS,
                                                left_refs, 1u);
                }
#endif            
#if (CAPTURE2_ENABLE == 1U)
                vx_reference right_refs[1];
                right_refs[0] = (vx_reference)obj->right_capture.raw_image_arr[0];
                if (status == VX_SUCCESS)
                {
                    status = tivxNodeSendCommand(obj->right_capture.node, 0u,
                                                TIVX_CAPTURE_PRINT_STATISTICS,
                                                right_refs, 1u);
                }
#endif 
                break;
            case 'x':
                printf("Exiting Application\n");
                obj->stop_task = 1;
                done = 1;
                break;

        }

        // Escape from event loop if process has been killed
        if(sigkill_active){
            obj->stop_task = 1;
            done = 1;
        }
    }

    appRemoteServiceUnRegister((char*)HEAD_SENSOR_SERVICE_NAME);

    printf("Run Task Delete\n");
    app_run_task_delete(obj);

    printf("Delete Graph\n");
    app_delete_graph(obj);

    printf("App De-init\n");
    app_deinit(obj);

    printf("goodbye!\n");
    return status;
}