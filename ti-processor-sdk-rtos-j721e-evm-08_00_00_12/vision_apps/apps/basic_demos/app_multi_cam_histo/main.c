/*
 *
 * Copyright (c) 2020 Texas Instruments Incorporated
 *
 * All rights reserved not granted herein.
 *
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
 * license under copyrights and patents it now or hereafter owns or controls to make,
 * have made, use, import, offer to sell and sell ("Utilize") this software subject to the
 * terms herein.  With respect to the foregoing patent license, such license is granted
 * solely to the extent that any such patent is necessary to Utilize the software alone.
 * The patent license shall not apply to any combinations which include this software,
 * other than combinations with devices manufactured by or for TI ("TI Devices").
 * No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this license
 * (including the above copyright notice and the disclaimer and (if applicable) source
 * code license limitations below) in the documentation and/or other materials provided
 * with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted provided
 * that the following conditions are met:
 *
 * *       No reverse engineering, decompilation, or disassembly of this software is
 * permitted with respect to any software provided in binary form.
 *
 * *       any redistribution and use are licensed by TI for use only with TI Devices.
 *
 * *       Nothing shall obligate TI to provide you with source code for the software
 * licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the
 * source code are permitted provided that the following conditions are met:
 *
 * *       any redistribution and use of the source code, including any resulting derivative
 * works, are licensed by TI for use only with TI Devices.
 *
 * *       any redistribution and use of any object code compiled from the source code
 * and any resulting derivative works, are licensed by TI for use only with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers
 *
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <utils/draw2d/include/draw2d.h>
#include <utils/perf_stats/include/app_perf_stats.h>
#include <utils/console_io/include/app_get.h>
#include <utils/grpx/include/app_grpx.h>
#include <utils/iss/include/app_iss.h>
#include <utils/d3board/include/app_d3board.h>
#include <utils/console_io/include/app_log.h>
#include <VX/vx_khr_pipelining.h>
#include <sys/stat.h>
#include <unistd.h>

#include "app_common.h"
#include "app_sensor_module.h"
#include "app_capture_module.h"
#include "app_viss_module.h"
#include "app_aewb_module.h"
#include "app_ldc_module.h"
#include "app_img_mosaic_module.h"
#include "app_display_module.h"
#include "app_histo_module.h"
#include "app_c7x_kernel.h"
#include "app_test.h"
#include "network.h"
#include "app_streaming_module.h"
#include "../../../git_version.h"

#define APP_BUFFER_Q_DEPTH   (4)
#define APP_PIPELINE_DEPTH   (7)

typedef struct {

    SensorObj     sensorObj;
    CaptureObj    captureObj;
    VISSObj       vissObj;
    AEWBObj       aewbObj;
    LDCObj        ldcObj;
    ImgMosaicObj  imgMosaicObj;
    DisplayObj    displayObj;
    /* Histogram object */
    HistoObj      histoObj;
    StreamingObj  streamingObj;

    vx_char output_file_path[APP_MAX_FILE_PATH];

    /* OpenVX references */
    vx_context context;
    vx_graph   graph;

    vx_int32 en_out_img_write;
    vx_int32 test_mode;

    vx_uint32 is_interactive;
    vx_int32 input_streaming_type;

    vx_uint32 num_histo_bins;

    vx_uint32 num_frames_to_run;

    vx_uint32 num_frames_to_write;
    vx_uint32 num_frames_to_skip;

    tivx_task task;
    vx_uint32 stop_task;
    vx_uint32 stop_task_done;

    app_perf_point_t total_perf;
    app_perf_point_t fileio_perf;
    app_perf_point_t draw_perf;

    int32_t enable_ldc;
    int32_t enable_viss;
    int32_t enable_aewb;
    int32_t enable_mosaic;

    int32_t pipeline;

    int32_t enqueueCnt;
    int32_t dequeueCnt;

    int32_t write_file;

    uint32_t frame_sync_period;
    uint32_t frame_sync_duration;
    uint32_t strobe_delay_start;
    uint32_t strobe_duration;
    uint32_t timer_resolution;

    uint32_t screendIdx; // Can be 0 or 1. 0 to show the first 4 cameras to the monitor and 1 to show the next 4 cameras

    int32_t currentActivePair; // 0,1,2,3 or -1 for all cameras active
    int32_t previousActivePair; // 0,1,2,3 or -1 for all cameras active

} AppObj;

AppObj gAppObj;

static void app_parse_cmd_line_args(AppObj *obj, vx_int32 argc, vx_char *argv[]);
static vx_status app_init(AppObj *obj);
static void app_deinit(AppObj *obj);
static vx_status app_create_graph(AppObj *obj);
static vx_status app_verify_graph(AppObj *obj);
static vx_status app_run_graph(AppObj *obj);
static vx_status app_run_graph_interactive(AppObj *obj);
static void app_delete_graph(AppObj *obj);
static void app_default_param_set(AppObj *obj);
static void app_update_param_set(AppObj *obj);
static void app_pipeline_params_defaults(AppObj *obj);
static void add_graph_parameter_by_node_index(vx_graph graph, vx_node node, vx_uint32 node_parameter_index);
static vx_int32 calc_grid_size(vx_uint32 ch);
static void set_img_mosaic_params(ImgMosaicObj *imgMosaicObj, vx_uint32 in_width, vx_uint32 in_height, vx_int32 numCh);
static void app_find_object_array_index(vx_object_array object_array[], vx_reference ref, vx_int32 array_size, vx_int32 *array_idx);
static vx_status app_set_cameras_to_display(AppObj *obj);
static void printfWhichActiveCameras(AppObj *obj);

static void app_show_usage(vx_int32 argc, vx_char* argv[])
{
    printf("\n");
    printf(" Camera Demo - (c) Texas Instruments 2020\n");
    printf(" ========================================================\n");
    printf("\n");
    printf(" Usage,\n");
    printf("  %s --cfg <config file>\n", argv[0]);
    printf("\n");
}

static char menu0[] = {
    "\n"
    "\n ========================="
    "\n Demo : Camera Demo"
    "\n ========================="
    "\n"
};

static char menu1[] = {
    "\n 1-4: Switch to camera pair 1 through 4."
    "\n     Cameras are paired adjacently based on port index. Histogram enabled for the selected pair only."
    "\n"
    "\n 5: Activate all cameras and disable histogram (initial state)"
    "\n"
    "\n t: Toggle display screen between cameras (0-3) and (4-7) (does not affect camera or histogram state)"
    "\n"
    "\n s: Save CSIx, VISS and LDC outputs"
    "\n"
    "\n p: Print performance statistics"
    "\n"
    "\n u: D3 Utils I2C Menu"
    "\n"
    "\n x: Exit"
    "\n"
};

static char utils_menu0[] = {
    "\n"
    "\n =================================="
    "\n I2C Utilities Menu"
    "\n =================================="
    "\n"
    "\n i 1: I2C Read"
    "\n o 2: I2C Write"
    "\n   3: Set Mode"
    "\n   4: Set Device Address"
    "\n r 5: Set Register Address"
    "\n d 6: Set Data"
    "\n"
    "\n x: Exit"
    "\n"
};

static char utils_menu1[] = {
    "\n"
    "\n 0: 8A8D"
    "\n 1: 8A16D"
    "\n 2: 16A8D"
    "\n 3: 16A16D"
    "\n"
    "\n Enter Choice: "
    "\n"
};

static void app_run_task(void *app_var)
{
    AppObj *obj = (AppObj *)app_var;
    vx_status status = VX_SUCCESS;
    while((!obj->stop_task) && (status == VX_SUCCESS))
    {
        status = app_run_graph(obj);
    }
    obj->stop_task_done = 1;
}

static int32_t app_run_task_create(AppObj *obj)
{
    tivx_task_create_params_t params;
    vx_status status;

    tivxTaskSetDefaultCreateParams(&params);
    params.task_main = app_run_task;
    params.app_var = obj;

    obj->stop_task_done = 0;
    obj->stop_task = 0;

    status = tivxTaskCreate(&obj->task, &params);

    return status;
}

static void app_run_task_delete(AppObj *obj)
{
    while(obj->stop_task_done==0)
    {
        tivxTaskWaitMsecs(100);
    }

    tivxTaskDelete(&obj->task);
}

static inline const char* modeToStr(D3_UTILS_I2C_TRANSACTION_TYPE type) {
    switch(type) {
        case TRANSACTION_TYPE_8A8D:
            return "8A8D  ";
        case TRANSACTION_TYPE_8A16D:
            return "8A16D ";
        case TRANSACTION_TYPE_16A8D:
            return "16A8D ";
        case TRANSACTION_TYPE_16A16D:
            return "16A16D";
        default:
            return "ERR   ";
    }
}

static uint32_t __readLine(uint32_t maxlen, char *buf) {
    int i = 0;
    char ch = 0;

    if(buf == NULL || maxlen <= 0) {
        return 0;
    }

    buf[0] = '\0'; // start with a null string

    for(i = 0; i < maxlen; i++) {
        ch = getchar();
        buf[i] = ch;
        if(buf[i] == '\n' || buf[i] == '\r') {
            buf[i] = '\0';
            break;
        }
    }

    return strlen(buf);
}

static void run_d3_utils_menu()
{
    D3UtilsI2C_Command *cmd = malloc(sizeof(D3UtilsI2C_Command));
    cmd->type = D3_I2C_READ;
    cmd->regAddr = 0;
    cmd->regData = 0;
    cmd->devAddr = 0;
    uint8_t done = 0;
    char ch;
    char inputBuffer[8];

    while(done == 0) {
        printf(utils_menu0);
        printf(
            " MODE:%s Device: ADDR:0x%02X, REG:0x%04X, Data:0x%04X\n",
            modeToStr(cmd->type),
            cmd->devAddr,
            cmd->regAddr,
            cmd->regData
        );
        printf(" Enter Choice: \n");
        do {
            ch = getchar();
        } while(ch == '\n' || ch == '\r');
        getchar(); // Eat the newline.
        switch(ch) {
            case 'i':
            case '1':
                printf("READ.\n");
                cmd->direction = D3_I2C_READ;
                appD3I2CMenu(cmd);
                break;
            case 'o':
            case '2':
                printf("WRITE.\n");
                cmd->direction = D3_I2C_WRITE;
                appD3I2CMenu(cmd);
                break;
            case '3':
                printf(utils_menu1);
                ch = getchar();
                switch(ch) {
                    case '0':
                        cmd->type = TRANSACTION_TYPE_8A8D;
                        break;
                    case '1':
                        cmd->type = TRANSACTION_TYPE_8A16D;
                        break;
                    case '2':
                        cmd->type = TRANSACTION_TYPE_16A8D;
                        break;
                    case '3':
                        cmd->type = TRANSACTION_TYPE_16A16D;
                        break;
                    default:
                        printf("Invalid type selected.\n");
                        break;
                }
                break;
            case '4':
                printf("Enter Device Address (7b Hex) and press Enter: ");
                __readLine(8, inputBuffer);
                cmd->devAddr = ((uint8_t) strtol(inputBuffer, NULL, 16)) & 0x7FU;
                break;
            case '5':
            case 'r':
                printf("Enter Register Address (Hex) and press Enter: ");
                __readLine(8, inputBuffer);
                cmd->regAddr = ((uint16_t) strtol(inputBuffer, NULL, 16));
                switch(cmd->type) {
                    case TRANSACTION_TYPE_8A16D:
                    case TRANSACTION_TYPE_8A8D:
                        cmd->regAddr &= 0xFFU;
                        break;
                    default:
                        break;
                }
                break;
            case '6':
            case 'd':
                printf("Enter Data (Hex) and press Enter: ");
                __readLine(8, inputBuffer);
                cmd->regData = ((uint16_t) strtol(inputBuffer, NULL, 16));
                switch(cmd->type) {
                    case TRANSACTION_TYPE_8A16D:
                    case TRANSACTION_TYPE_8A8D:
                        cmd->regData &= 0xFFU;
                        break;
                    default:
                        break;
                }
                break;
            case 'x':
                done = 1;
                break;
            default:
                printf("Unrecognized command.\n");
                break;
        }
    }
}

static vx_status app_run_graph_interactive(AppObj *obj)
{
    vx_status status;
    uint32_t done = 0;

    char ch;
    FILE *fp;
    app_perf_point_t *perf_arr[1];

    /* Get the IP address of the device to be displayed on the console */
    char host_ipaddress[NI_MAXHOST];

    getHostIpAddress(host_ipaddress);

    status = app_run_task_create(obj);
    if(status == VX_FAILURE)
    {
        printf("app_tidl: ERROR: Unable to create task\n");
    }
    else
    {
        appPerfStatsResetAll();

        // Wait 2 sec before displaying the menu so that MCU has finished displaying its messages 
        sleep(2);

        while(!done)
        {
            printf(menu0);
            printfWhichActiveCameras(obj);
            printf(menu1);
            printf("\n\n Note that device content can be accessed through sftp://root@%s\n", host_ipaddress);
            ch = getchar();
            getchar();
            printf("\n");

            switch(ch)
            {
                case '1':
                    if (obj->sensorObj.num_cameras_enabled >= 2)
                    {
                        if (obj->screendIdx!=0)
                        {
                            obj->screendIdx= 0;
                            app_set_cameras_to_display(obj);
                        }
                        obj->previousActivePair= obj->currentActivePair;
                        obj->currentActivePair = 0;
                        uint64_t t1= appLogGetTimeInUsec();
                        app_send_cmd_viss_enableChan(&obj->vissObj, (APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE_MASK<<(obj->currentActivePair*APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE)));
                        appActivateCameraPairs(obj->currentActivePair);
                        app_send_cmd_histo_enableChan(&obj->histoObj, (APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE_MASK<<(obj->currentActivePair*APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE)));
                        uint64_t diff= appLogGetTimeInUsec() - t1;
                        printf("\n appActivateCameraPairs() time: %d ms\n", (uint32_t)diff/1000);
                    }
                    break;
                case '2':
                    if (obj->sensorObj.num_cameras_enabled >= 4)
                    {
                        if (obj->screendIdx != 0)
                        {
                            obj->screendIdx = 0;
                            app_set_cameras_to_display(obj);
                        }
                        obj->previousActivePair= obj->currentActivePair;
                        obj->currentActivePair = 1;
                        uint64_t t1= appLogGetTimeInUsec();
                        app_send_cmd_viss_enableChan(&obj->vissObj, (APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE_MASK<<(obj->currentActivePair*APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE)));
                        appActivateCameraPairs(obj->currentActivePair);
                        app_send_cmd_histo_enableChan(&obj->histoObj, (APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE_MASK<<(obj->currentActivePair*APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE)));
                        uint64_t diff= appLogGetTimeInUsec() - t1;
                        printf("\n appActivateCameraPairs() time: %d ms\n", (uint32_t)diff/1000);
                    }
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
                    if (obj->input_streaming_type == INPUT_STREAM_SERDES)
                    {
                        vx_reference refs[1];
                        refs[0] = (vx_reference)obj->captureObj.raw_image_arr[0];
                        if (status == VX_SUCCESS)
                        {
                            status = tivxNodeSendCommand(obj->captureObj.node, 0u,
                                                         TIVX_CAPTURE_PRINT_STATISTICS,
                                                         refs, 1u);
                        }
                    }
                    break;
                case 'e':
                    perf_arr[0] = &obj->total_perf;
                    fp = appPerfStatsExportOpenFile(".", "basic_demos_app_multi_cam");
                    if (NULL != fp)
                    {
                        appPerfStatsExportAll(fp, perf_arr, 1);
                        if (status == VX_SUCCESS)
                        {
                            status = tivx_utils_graph_perf_export(fp, obj->graph);
                        }
                        appPerfStatsExportCloseFile(fp);
                        appPerfStatsResetAll();
                    }
                    else
                    {
                        printf("fp is null\n");
                    }
                    break;
                case 's':
                    obj->write_file = 1;
                    break;
                case 't':
                    obj->screendIdx^=1;
                    app_set_cameras_to_display(obj);
                    break;
                case 'u':
                    run_d3_utils_menu();
                    break;
                case 'x':
                    obj->stop_task = 1;
                    done = 1;
                    break;
            }
        }
        app_run_task_delete(obj);
    }
    return status;
}

static void app_set_cfg_default(AppObj *obj)
{
    snprintf(obj->captureObj.output_file_path,APP_MAX_FILE_PATH, ".");
    snprintf(obj->vissObj.output_file_path,APP_MAX_FILE_PATH, ".");
    snprintf(obj->ldcObj.output_file_path,APP_MAX_FILE_PATH, ".");

    obj->captureObj.en_out_capture_write = 0;
    obj->vissObj.en_out_viss_write = 0;
    obj->ldcObj.en_out_ldc_write = 0;
    obj->input_streaming_type= INPUT_STREAM_SERDES;

    obj->num_frames_to_write = 0;
    obj->num_frames_to_skip = 0;

    /* Initialize input path for file I/O based streaming to default '.' */
    for (int32_t channel= 0; channel < INPUT_STREAM_MAX_CHANNELS; channel++)
    {
        snprintf(obj->streamingObj.visionStreamInitParams._.file.path[channel], INPUT_STREAM_MAX_FILE_PATH, ".");
    }
}

static void app_parse_cfg_file(AppObj *obj, vx_char *cfg_file_name)
{
    FILE *fp = fopen(cfg_file_name, "r");
    vx_char line_str[1024];
    vx_char *token;

    if(fp==NULL)
    {
        printf("# ERROR: Unable to open config file [%s]\n", cfg_file_name);
        exit(0);
    }

    while(fgets(line_str, sizeof(line_str), fp)!=NULL)
    {
        vx_char s[]=" \t";

        if (strchr(line_str, '#'))
        {
            continue;
        }

        /* get the first token */
        token = strtok(line_str, s);
        if(token != NULL)
        {
            if(strcmp(token, "sensor_index")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    obj->sensorObj.sensor_index = atoi(token);
                }
            }
            else
            if(strcmp(token, "num_frames_to_run")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    obj->num_frames_to_run = atoi(token);
                }
            }
            else
            if(strcmp(token, "enable_error_detection")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    obj->captureObj.enable_error_detection = atoi(token);
                }
            }
            else
            if(strcmp(token, "enable_ldc")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    obj->sensorObj.enable_ldc = atoi(token);
                    if(obj->sensorObj.enable_ldc > 1)
                        obj->sensorObj.enable_ldc = 1;
                }
            }
            else
            if(strcmp(token, "en_out_img_write")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    obj->en_out_img_write = atoi(token);
                    if(obj->en_out_img_write > 1)
                        obj->en_out_img_write = 1;
                }
            }
            else
            if(strcmp(token, "en_out_capture_write")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    obj->captureObj.en_out_capture_write = atoi(token);
                    if(obj->captureObj.en_out_capture_write > 1)
                        obj->captureObj.en_out_capture_write = 1;
                }
            }
            else
            if(strcmp(token, "en_out_viss_write")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    obj->vissObj.en_out_viss_write = atoi(token);
                    if(obj->vissObj.en_out_viss_write > 1)
                        obj->vissObj.en_out_viss_write = 1;
                }
            }
            else
            if(strcmp(token, "en_out_ldc_write")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    obj->ldcObj.en_out_ldc_write = atoi(token);
                    if(obj->ldcObj.en_out_ldc_write > 1)
                        obj->ldcObj.en_out_ldc_write = 1;
                }
            }
            else
            if(strcmp(token, "output_file_path")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    token[strlen(token)-1]=0;
                    strcpy(obj->captureObj.output_file_path, token);
                    strcpy(obj->vissObj.output_file_path, token);
                    strcpy(obj->ldcObj.output_file_path, token);
                    strcpy(obj->histoObj.output_file_path, token);
                    strcpy(obj->output_file_path, token);
                }
            }
            else
            if(strcmp(token, "display_option")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    obj->displayObj.display_option = atoi(token);
                    if(obj->displayObj.display_option > 1)
                        obj->displayObj.display_option = 1;
                }
            }
            else
            if(strcmp(token, "is_interactive")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    token[strlen(token)-1]=0;
                    obj->is_interactive = atoi(token);
                    if(obj->is_interactive > 1)
                    {
                        obj->is_interactive = 1;
                    }
                }
                //obj->sensorObj.is_interactive = obj->is_interactive;
            }
            else
            if(strcmp(token, "num_cameras_enabled")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    token[strlen(token)-1]=0;
                    obj->sensorObj.num_cameras_enabled = atoi(token);
                #if 1
                    switch(obj->sensorObj.num_cameras_enabled){
                        case 8:	obj->sensorObj.ch_mask = 0xFF; break;
                        case 7:	obj->sensorObj.ch_mask = 0x7F; break;
                        case 6:	obj->sensorObj.ch_mask = 0x3F; break;    //0x77 symmatric, 0x3F assymatric
                        case 5:	obj->sensorObj.ch_mask = 0x37; break;
                        case 4:	obj->sensorObj.ch_mask = 0x33; break;
                        case 3:	obj->sensorObj.ch_mask = 0x13; break;
                        case 2:	obj->sensorObj.ch_mask = 0x11; break;    //0x11 symmatric, asymmatric 0x41-p3 0x28-p4
                        case 1:	obj->sensorObj.ch_mask = 0x01; break;
                        default:obj->sensorObj.ch_mask = 0xFF; break;
                    }
                #else
                    obj->sensorObj.ch_mask = (1<<obj->sensorObj.num_cameras_enabled)-1;
                #endif
                }
            }
            else
            if(strcmp(token, "usecase_option")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    token[strlen(token)-1]=0;
                    obj->sensorObj.usecase_option = atoi(token);
                }
            }
            else
            if(strcmp(token, "num_frames_to_write")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    token[strlen(token)-1]=0;
                    obj->num_frames_to_write = atoi(token);
                }
            }
            else
            if(strcmp(token, "num_frames_to_skip")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    token[strlen(token)-1]=0;
                    obj->num_frames_to_skip = atoi(token);
                }
            }
            else if (strcmp(token, "frame_sync_period") == 0)
            {
                token = strtok(NULL, s);
                if (NULL != token)
                {
                    obj->frame_sync_period = atoi(token);
                    printf("frame_sync_period = [%d]\n", obj->frame_sync_period);
                }
            }
            else if (strcmp(token, "frame_sync_duration") == 0)
            {
                token = strtok(NULL, s);
                if (NULL != token)
                {
                    obj->frame_sync_duration = atoi(token);
                    printf("frame_sync_duration = [%d]\n", obj->frame_sync_duration);
                }
            }
            else if (strcmp(token, "strobe_delay_start") == 0)
            {
                token = strtok(NULL, s);
                if (NULL != token)
                {
                    obj->strobe_delay_start = atoi(token);
                    printf("strobe_delay_start = [%d]\n", obj->strobe_delay_start);
                }
            }
            else if (strcmp(token, "strobe_duration") == 0)
            {
                token = strtok(NULL, s);
                if (NULL != token)
                {
                    obj->strobe_duration = atoi(token);
                    printf("strobe_duration = [%d]\n", obj->strobe_duration);
                }
            }
            else if (strcmp(token, "timer_resolution") == 0)
            {
                token = strtok(NULL, s);
                if (NULL != token)
                {
                    obj->timer_resolution = atoi(token);
                    printf("timer_resolution = [%d]\n", obj->timer_resolution);
                }
            }
            else
            if(strcmp(token, "test_mode")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    obj->test_mode = atoi(token);
                    obj->captureObj.test_mode = atoi(token);
                }
            }
            if(strcmp(token, "num_histo_bins")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    obj->num_histo_bins = atoi(token);
                }
            }
            else
            if(strcmp(token, "input_streaming_type")==0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    token[strlen(token)-1]=0;
                    obj->input_streaming_type= atoi(token);
                    if ((obj->input_streaming_type != INPUT_STREAM_SERDES) && (obj->input_streaming_type != INPUT_STREAM_FILE) )
                    {
                        printf("File config parser error: Invalid vision_input_streaming_type= %d\n", obj->input_streaming_type);
                    }
                }
            }
            else
            {
                int32_t channel;
                char file_token[sizeof("input_file_path_") + 3];

                for (channel = 0; channel < INPUT_STREAM_MAX_CHANNELS; channel++)
                {
                    sprintf(file_token, "input_file_path_%02d", channel);

                    if (strcmp(token, file_token) == 0)
                    {
                        token = strtok(NULL, s);
                        if (token != NULL)
                        {
                            token[strlen(token) - 1] = 0;
                            strcpy(obj->streamingObj.visionStreamInitParams._.file.path[channel], token);
                            break;
                        }
                    }
                }
            }
        }
    }

    fclose(fp);

}

static void app_parse_cmd_line_args(AppObj *obj, vx_int32 argc, vx_char *argv[])
{
    vx_int32 i;
    vx_int16 num_test_cams = 0xFF, sensor_override = 0xFF;
    vx_bool set_test_mode = vx_false_e;

    app_set_cfg_default(obj);

    if(argc==1)
    {
        app_show_usage(argc, argv);
        exit(0);
    }

    for(i=0; i<argc; i++)
    {
        if(strcmp(argv[i], "--cfg")==0)
        {
            i++;
            if(i>=argc)
            {
                app_show_usage(argc, argv);
            }
            app_parse_cfg_file(obj, argv[i]);
        }
        else
        if(strcmp(argv[i], "--help")==0)
        {
            app_show_usage(argc, argv);
            exit(0);
        }
        else
        if(strcmp(argv[i], "--test")==0)
        {
            set_test_mode = vx_true_e;
            // check to see if there is another argument following --test
            if (argc > i+1)
            {
                num_test_cams = atoi(argv[i+1]);
                // increment i again to avoid this arg
                i++;
            }
        }
        else
        if(strcmp(argv[i], "--sensor")==0)
        {
            // check to see if there is another argument following --sensor
            if (argc > i+1)
            {
                sensor_override = atoi(argv[i+1]);
                // increment i again to avoid this arg
                i++;
            }
        }
    }

    if (set_test_mode == vx_true_e)
    {
        obj->test_mode = 1;
        obj->captureObj.test_mode = 1;
        obj->is_interactive = 0;
        obj->sensorObj.is_interactive = 0;
        // set the number of test cams from cmd line
        if (num_test_cams != 0xFF)
        {
            obj->sensorObj.num_cameras_enabled = num_test_cams;
        }
        if (sensor_override != 0xFF)
        {
            obj->sensorObj.sensor_index = sensor_override;
        }
    }

    return;
}

vx_int32 app_multi_cam_main(vx_int32 argc, vx_char* argv[])
{
    vx_status status = VX_SUCCESS;
    printf("\n\nApp Multi Cam Histogram\n");
    printf("GIT Build Date: %s\n", GIT_BUILD_DATE);
    printf("GIT Commit: %s\n\n", GIT_BUILD_HASH);

    AppObj *obj = &gAppObj;

    /*Optional parameter setting*/
    app_default_param_set(obj);

    /*Config parameter reading*/
    app_parse_cmd_line_args(obj, argc, argv);

    /* Querry sensor parameters */
    status = app_querry_sensor(&obj->sensorObj);

    if(1 == obj->sensorObj.sensor_out_format)
    {
        printf("YUV Input selected. VISS, AEWB and Mosaic nodes will be bypassed. \n");
        obj->enable_viss = 0;
        obj->enable_aewb = 0;
        obj->enable_mosaic = 0;
    }
    else
    {
        obj->enable_viss = 1;
        obj->enable_aewb = 0;
        obj->enable_mosaic = 1;
    }

    if (obj->input_streaming_type== INPUT_STREAM_FILE)
    {
        obj->enable_aewb = 0;
        obj->captureObj.enable_error_detection= 0;
    }

    /*Update of parameters are config file read*/
    app_update_param_set(obj);

    if (status == VX_SUCCESS)
    {
        status = app_init(obj);
    }
    if(status == VX_SUCCESS)
    {
        APP_PRINTF("App Init Done! \n");

        status = app_create_graph(obj);

        if(status == VX_SUCCESS)
        {
            APP_PRINTF("App Create Graph Done! \n");

            status = app_verify_graph(obj);

            if(status == VX_SUCCESS)
            {
                APP_PRINTF("App Verify Graph Done! \n");

                if (status == VX_SUCCESS)
                {
                    APP_PRINTF("App Send Error Frame Done! \n");
                    if(obj->is_interactive)
                    {
                        status = app_run_graph_interactive(obj);
                    }
                    else
                    {
                        status = app_run_graph(obj);
                    }
                }
            }
        }

        APP_PRINTF("App Run Graph Done! \n");
    }

    app_delete_graph(obj);

    APP_PRINTF("App Delete Graph Done! \n");

    app_deinit(obj);

    APP_PRINTF("App De-init Done! \n");

    if(obj->test_mode == 1)
    {
        if((vx_false_e == test_result) || (status != VX_SUCCESS))
        {
            printf("\n\nTEST FAILED\n\n");
            print_new_checksum_structs();
            status = (status == VX_SUCCESS) ? VX_FAILURE : status;
        }
        else
        {
            printf("\n\nTEST PASSED\n\n");
        }
    }
    return status;
}

static vx_status app_c7x_kernels_load(vx_context context)
{
    #ifdef x86_64
    {
        /* trick PC emulation mode to register these kernels on C7x */
        void tivxSetSelfCpuId(vx_enum cpu_id);

        tivxSetSelfCpuId(TIVX_CPU_ID_DSP_C7_1);
        app_c7x_target_kernel_histo_register();
        tivxSetSelfCpuId(TIVX_CPU_ID_DSP1);
    }
    #endif
    return app_c7x_kernel_histo_register(context);
}

static vx_status app_c7x_kernels_unload(vx_context context)
{
    vx_status status = VX_SUCCESS;
    #ifdef x86_64
    status = app_c7x_target_kernel_histo_unregister();
    #endif
    if(status == VX_SUCCESS)
    {
        status = app_c7x_kernel_histo_unregister(context);
    }
    return status;
}

static vx_status app_init(AppObj *obj)
{
    vx_status status = VX_SUCCESS;

    /* Create OpenVx Context */
    obj->context = vxCreateContext();
    status = vxGetStatus((vx_reference)obj->context);
    APP_PRINTF("Creating context done!\n");
    if (status == VX_SUCCESS)
    {
        tivxHwaLoadKernels(obj->context);
        tivxImagingLoadKernels(obj->context);
        tivxFileIOLoadKernels(obj->context);
        APP_PRINTF("Kernel loading done!\n");
    }

    /* Initialize modules */

    if (status == VX_SUCCESS)
    {
        status = app_c7x_kernels_load(obj->context);
        APP_PRINTF("app_c7x_kernels_load done!\n");
    }

    if (obj->input_streaming_type == INPUT_STREAM_FILE)
    {
        if (status == VX_SUCCESS)
        {
            app_init_streaming(obj->context, &obj->streamingObj, &obj->sensorObj, "streaming_obj", APP_BUFFER_Q_DEPTH);
            APP_PRINTF("Streaming init done!\n");
        }
    }
    else
    {

        AppD3boardInitCmdParams cmdPrms;

        cmdPrms.moduleId = APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC;
        cmdPrms.fSyncPeriod = obj->frame_sync_period;
        cmdPrms.fSyncDuration = obj->frame_sync_duration;
        cmdPrms.strobeLightDelayStart = obj->strobe_delay_start;
        cmdPrms.strobeLightDuration = obj->strobe_duration;
        cmdPrms.timerResolution = obj->timer_resolution;

        int32_t d3board_init_status = appInitD3board(&cmdPrms);
        if (0 != d3board_init_status)
        {
            /* Not returning failure because application may be waiting for
            error/test frame */
            printf("Error initializing D3 board\n");
        }

        if (status == VX_SUCCESS)
        {
            app_init_sensor(&obj->sensorObj, "sensor_obj");
            APP_PRINTF("Sensor init done!\n");
        }

        if (status == VX_SUCCESS)
        {
            status = app_init_capture(obj->context, &obj->captureObj, &obj->sensorObj, "capture_obj", APP_BUFFER_Q_DEPTH);
            APP_PRINTF("Capt init done!\n");
        }
    }

    if (status == VX_SUCCESS)
    {
        /* Enable histogram processing of channel #0 and channel #1
           Note that actual swapping of camera pair is done by function appActivateCameraPairs()
           The settings below remain constant throughout the application
        */
        obj->histoObj.params.enableChanBitFlag= 0; /* Disable histogram computation for all channels at the beginning */
        status = app_init_histo(obj->context, &obj->histoObj, &obj->sensorObj, "histo_obj", APP_BUFFER_Q_DEPTH, obj->num_histo_bins);
        APP_PRINTF("Histo init done!\n");
    }

    if((1 == obj->enable_viss) && (status == VX_SUCCESS))
    {
        status = app_init_viss(obj->context, &obj->vissObj, &obj->sensorObj, "viss_obj");
        APP_PRINTF("VISS init done!\n");
    }

    if((1 == obj->enable_aewb) && (status == VX_SUCCESS))
    {
        status = app_init_aewb(obj->context, &obj->aewbObj, &obj->sensorObj, "aewb_obj");
        APP_PRINTF("AEWB init done!\n");
    }

    if((obj->sensorObj.enable_ldc == 1) && (status == VX_SUCCESS))
    {
        status = app_init_ldc(obj->context, &obj->ldcObj, &obj->sensorObj, "ldc_obj");
        APP_PRINTF("LDC init done!\n");
    }


    if((obj->enable_mosaic == 1) && (status == VX_SUCCESS))
    {
        status = app_init_img_mosaic(obj->context, &obj->imgMosaicObj, "img_mosaic_obj", APP_BUFFER_Q_DEPTH);
        APP_PRINTF("Img Mosaic init done!\n");
    }

    if (status == VX_SUCCESS)
    {
        status = app_init_display(obj->context, &obj->displayObj, "display_obj");
        APP_PRINTF("Display init done!\n");
    }

    appPerfPointSetName(&obj->total_perf , "TOTAL");
    appPerfPointSetName(&obj->fileio_perf, "FILEIO");
    return status;
}

static void app_deinit(AppObj *obj)
{

    printf("De-Init D3Board \n");
    appDeInitD3board();

    if (obj->input_streaming_type == INPUT_STREAM_FILE)
    {
        app_deinit_streaming(&obj->streamingObj, APP_BUFFER_Q_DEPTH);
        printf("Streaming deinit done!\n");
    }
    else
    {
        app_deinit_sensor(&obj->sensorObj);
        printf("Sensor deinit done!\n");

        app_deinit_capture(&obj->captureObj, APP_BUFFER_Q_DEPTH);
        printf("Capture deinit done!\n");
    }

    app_deinit_histo(&obj->histoObj, APP_BUFFER_Q_DEPTH);
    printf("Histo deinit done!\n");

    if(1 == obj->enable_viss)
    {
        app_deinit_viss(&obj->vissObj);
        printf("VISS deinit done!\n");
    }

    if(1 == obj->enable_aewb)
    {
        app_deinit_aewb(&obj->aewbObj);
        printf("AEWB deinit done!\n");
    }

    if(obj->sensorObj.enable_ldc == 1)
    {
        app_deinit_ldc(&obj->ldcObj);
        printf("LDC deinit done!\n");
    }

    if(obj->enable_mosaic == 1)
    {
        app_deinit_img_mosaic(&obj->imgMosaicObj, APP_BUFFER_Q_DEPTH);
        printf("Img Mosaic deinit done!\n");
    }

    app_deinit_display(&obj->displayObj);
    printf("Display deinit done!\n");

    tivxTaskWaitMsecs(500);
    tivxHwaUnLoadKernels(obj->context);
    tivxImagingUnLoadKernels(obj->context);
    tivxFileIOUnLoadKernels(obj->context);
    app_c7x_kernels_unload(obj->context);
    printf("Kernels unload done!\n");

    vxReleaseContext(&obj->context);
    printf("Release context done!\n");
}

static void app_delete_graph(AppObj *obj)
{
    if (obj->input_streaming_type == INPUT_STREAM_SERDES)
    {
        app_delete_capture(&obj->captureObj);
        APP_PRINTF("Capture delete done!\n");
    }

    app_delete_histo(&obj->histoObj);
    APP_PRINTF("Histo delete done!\n");

    app_delete_viss(&obj->vissObj);
    APP_PRINTF("VISS delete done!\n");

    app_delete_aewb(&obj->aewbObj);
    APP_PRINTF("AEWB delete done!\n");

    if(obj->sensorObj.enable_ldc == 1)
    {
        app_delete_ldc(&obj->ldcObj);
        APP_PRINTF("LDC delete done!\n");
    }

    app_delete_img_mosaic(&obj->imgMosaicObj);
    APP_PRINTF("Img Mosaic delete done!\n");

    app_delete_display(&obj->displayObj);
    APP_PRINTF("Display delete done!\n");

    vxReleaseGraph(&obj->graph);
    APP_PRINTF("Graph delete done!\n");
}

static vx_status app_create_graph(AppObj *obj)
{
    vx_status status = VX_SUCCESS;
    vx_object_array raw_input;
    vx_graph_parameter_queue_params_t graph_parameters_queue_params_list[5];
    vx_int32 graph_parameter_index;

    obj->graph = vxCreateGraph(obj->context);
    status = vxGetStatus((vx_reference)obj->graph);
    if (status == VX_SUCCESS)
    {
        status = vxSetReferenceName((vx_reference)obj->graph, "app_multi_cam_graph");
        APP_PRINTF("Graph create done!\n");
    }

    if (obj->input_streaming_type == INPUT_STREAM_SERDES)
    {
        if (status == VX_SUCCESS)
        {
            status = app_create_graph_capture(obj->graph, &obj->captureObj);
            APP_PRINTF("Capture graph done!\n");
        }

        raw_input = obj->captureObj.raw_image_arr[0];
    }
    else
    {
        raw_input = obj->streamingObj.raw_image_arr[0];
    }

    if(status == VX_SUCCESS)
    {
        status = app_create_graph_histo(obj->graph, &obj->histoObj, raw_input);
        APP_PRINTF("Histo graph done!\n");
    }

    if(1 == obj->enable_viss)
    {
        if(status == VX_SUCCESS)
        {
            status = app_create_graph_viss(obj->graph, &obj->vissObj, raw_input);
            APP_PRINTF("VISS graph done!\n");
        }
    }

    if(1 == obj->enable_aewb)
    {
        if(status == VX_SUCCESS)
        {
            status = app_create_graph_aewb(obj->graph, &obj->aewbObj, obj->vissObj.h3a_stats_arr);

            APP_PRINTF("AEWB graph done!\n");
        }
    }

    vx_int32 idx = 0;
    if(obj->sensorObj.enable_ldc == 1)
    {
        vx_object_array ldc_in_arr;
        if(1 == obj->enable_viss)
        {
            ldc_in_arr = obj->vissObj.output_arr;
        }
        else
        {
            ldc_in_arr = raw_input;
        }
        if (status == VX_SUCCESS)
        {
            status = app_create_graph_ldc(obj->graph, &obj->ldcObj, ldc_in_arr);
            APP_PRINTF("LDC graph done!\n");
        }
        obj->imgMosaicObj.input_arr[idx++] = obj->ldcObj.output_arr;
    }
    else
    {
        vx_object_array mosaic_in_arr;
        if(1 == obj->enable_viss)
        {
            mosaic_in_arr = obj->vissObj.output_arr;
        }
        else
        {
            mosaic_in_arr = raw_input;
        }

        obj->imgMosaicObj.input_arr[idx++] = mosaic_in_arr;
    }

    vx_image display_in_image;
    if(obj->enable_mosaic == 1)
    {
        obj->imgMosaicObj.num_inputs = idx;
    
        if(status == VX_SUCCESS)
        {
            status = app_create_graph_img_mosaic(obj->graph, &obj->imgMosaicObj);
            APP_PRINTF("Img Mosaic graph done!\n");
        }
        display_in_image = obj->imgMosaicObj.output_image[0];
    }
    else
    {
        display_in_image = (vx_image)vxGetObjectArrayItem(raw_input, 0);
    }

    if(status == VX_SUCCESS)
    {
        status = app_create_graph_display(obj->graph, &obj->displayObj, display_in_image);
        APP_PRINTF("Display graph done!\n");
    }

    if(status == VX_SUCCESS)
    {
        graph_parameter_index = 0;

        if (obj->input_streaming_type == INPUT_STREAM_FILE)
        {
            add_graph_parameter_by_node_index(obj->graph, obj->histoObj.node, APP_C7X_HISTO_IN_RAW_IMG_IDX);
            obj->streamingObj.graph_parameter_index = graph_parameter_index;
            graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
            graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
            graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference *)&obj->streamingObj.raw_image_ch0[0];
            graph_parameter_index++;
        }
        else
        {
            add_graph_parameter_by_node_index(obj->graph, obj->captureObj.node, 1);
            obj->captureObj.graph_parameter_index = graph_parameter_index;
            graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
            graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
            graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference *)&obj->captureObj.raw_image_arr[0];
            graph_parameter_index++;
        }

        add_graph_parameter_by_node_index(obj->graph, obj->histoObj.node, APP_C7X_HISTO_OUT_DISTRIBUTION_IDX);
        obj->histoObj.graph_parameter_distribution_index = graph_parameter_index;
        graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
        graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
        graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference*)&obj->histoObj.outHistoCh0[0];
        graph_parameter_index++;

        add_graph_parameter_by_node_index(obj->graph, obj->histoObj.node, APP_C7X_HISTO_OUT_MEAN_IDX);
        obj->histoObj.graph_parameter_mean_index = graph_parameter_index;
        graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
        graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
        graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference*)&obj->histoObj.outMeanCh0[0];
        graph_parameter_index++;

        add_graph_parameter_by_node_index(obj->graph, obj->histoObj.node, APP_C7X_HISTO_OUT_SD_IDX);
        obj->histoObj.graph_parameter_sd_index = graph_parameter_index;
        graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
        graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
        graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference *)&obj->histoObj.outSdCh0[0];
        graph_parameter_index++;

        if((obj->en_out_img_write == 1) || (obj->test_mode == 1))
        {
            add_graph_parameter_by_node_index(obj->graph, obj->imgMosaicObj.node, 1);
            obj->imgMosaicObj.graph_parameter_index = graph_parameter_index;
            graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
            graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
            graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference*)&obj->imgMosaicObj.output_image[0];
            graph_parameter_index++;
        }

        status = vxSetGraphScheduleConfig(obj->graph,
                VX_GRAPH_SCHEDULE_MODE_QUEUE_AUTO,
                graph_parameter_index,
                graph_parameters_queue_params_list);
#if 0
        if (status == VX_SUCCESS)
        {
            status = tivxSetGraphPipelineDepth(obj->graph, APP_PIPELINE_DEPTH);
        }
        if((obj->enable_viss == 1) && (status == VX_SUCCESS))
        {
            status = tivxSetNodeParameterNumBufByIndex(obj->vissObj.node, 6, APP_BUFFER_Q_DEPTH);

            if (status == VX_SUCCESS)
            {
                status = tivxSetNodeParameterNumBufByIndex(obj->vissObj.node, 9, APP_BUFFER_Q_DEPTH);
            }
            if ((obj->enable_aewb == 1) && (status == VX_SUCCESS))
            {
                status = tivxSetNodeParameterNumBufByIndex(obj->aewbObj.node, 4, APP_BUFFER_Q_DEPTH);
            }
        }
        if((obj->sensorObj.enable_ldc == 1) && (status == VX_SUCCESS))
        {
            status = tivxSetNodeParameterNumBufByIndex(obj->ldcObj.node, 7, APP_BUFFER_Q_DEPTH);
        }
        if((obj->enable_mosaic == 1) && (status == VX_SUCCESS))
        {
            if(!((obj->en_out_img_write == 1) || (obj->test_mode == 1)))
            {
                status = tivxSetNodeParameterNumBufByIndex(obj->imgMosaicObj.node, 1, APP_BUFFER_Q_DEPTH);
                APP_PRINTF("Pipeline params setup done!\n");
            }
        }
#endif
    }

    return status;
}

static vx_status app_verify_graph(AppObj *obj)
{
    vx_status status = VX_SUCCESS;

    status = vxVerifyGraph(obj->graph);

    if(status == VX_SUCCESS)
    {
        APP_PRINTF("Graph verify done!\n");
    }

    #if 1
    if(VX_SUCCESS == status)
    {
      status = tivxExportGraphToDot(obj->graph,".", "vx_app_multi_cam_histo");
    }
    #endif

    if (((obj->captureObj.enable_error_detection) || (obj->test_mode)) && (status == VX_SUCCESS))
    {
        status = app_send_error_frame(&obj->captureObj);
    }

    /* wait a while for prints to flush */
    tivxTaskWaitMsecs(100);

    return status;
}

static vx_status app_run_graph_for_one_frame_pipeline(AppObj *obj, vx_int32 frame_id)
{
    vx_status status = VX_SUCCESS;

    vx_int32 obj_array_idx = -1;

    APP_PRINTF("app_run_graph_for_one_pipeline: frame %d beginning\n", frame_id);
    appPerfPointBegin(&obj->total_perf);

    ImgMosaicObj *imgMosaicObj = &obj->imgMosaicObj;
    CaptureObj *captureObj = &obj->captureObj;
    StreamingObj *streamingObj = &obj->streamingObj;

    /* checksum_actual is the checksum determined by the realtime test
        checksum_expected is the checksum that is expected to be the pipeline output */
    uint32_t checksum_actual = 0;

    /* This is the number of frames required for the pipeline AWB and AE algorithms to stabilize
        (note that 15 is only required for the 6-8 camera use cases - others converge quicker) */
    uint8_t stability_frame = 15;

    if(obj->pipeline < 0)
    {
        /* Enqueue outpus */
        if ((obj->en_out_img_write == 1) || (obj->test_mode == 1))
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, imgMosaicObj->graph_parameter_index, (vx_reference*)&imgMosaicObj->output_image[obj->enqueueCnt], 1);
        }

        /* Enqueue inputs during pipeup dont execute */
        if (status == VX_SUCCESS)
        {
            if (obj->input_streaming_type == INPUT_STREAM_FILE)
            {
                status= app_read_streaming(streamingObj, obj->enqueueCnt);
                if (status == VX_SUCCESS)
                {
                    status = vxGraphParameterEnqueueReadyRef(obj->graph, streamingObj->graph_parameter_index, (vx_reference *)&obj->streamingObj.raw_image_ch0[obj->enqueueCnt], 1);
                }
                else
                {
                    printf("Error reading input files, verify that the input directories contain any valid files. Camera streaming disabled, please press 'x' to exit the application\n");
                    obj->stop_task= 1;
                }
            }
            else
            {
                status = vxGraphParameterEnqueueReadyRef(obj->graph, captureObj->graph_parameter_index, (vx_reference *)&obj->captureObj.raw_image_arr[obj->enqueueCnt], 1);
            }
        }
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_distribution_index, (vx_reference*)&obj->histoObj.outHistoCh0[obj->enqueueCnt], 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_mean_index, (vx_reference*)&obj->histoObj.outMeanCh0[obj->enqueueCnt], 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_sd_index, (vx_reference*)&obj->histoObj.outSdCh0[obj->enqueueCnt], 1);
        }
        obj->enqueueCnt++;
        obj->enqueueCnt   = (obj->enqueueCnt  >= APP_BUFFER_Q_DEPTH)? 0 : obj->enqueueCnt;
        obj->pipeline++;
    }

    if((obj->pipeline == 0) && (status == VX_SUCCESS))
    {
        if((obj->en_out_img_write == 1) || (obj->test_mode == 1))
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, imgMosaicObj->graph_parameter_index, (vx_reference*)&imgMosaicObj->output_image[obj->enqueueCnt], 1);
        }

        /* Execute 1st frame */
        if(status == VX_SUCCESS)
        {
            if (obj->input_streaming_type == INPUT_STREAM_FILE)
            {
                status= app_read_streaming(streamingObj, obj->enqueueCnt);
                if (status == VX_SUCCESS)
                {
                    status = vxGraphParameterEnqueueReadyRef(obj->graph, streamingObj->graph_parameter_index, (vx_reference *)&obj->streamingObj.raw_image_ch0[obj->enqueueCnt], 1);
                }
            }
            else
            {
                status = vxGraphParameterEnqueueReadyRef(obj->graph, captureObj->graph_parameter_index, (vx_reference *)&obj->captureObj.raw_image_arr[obj->enqueueCnt], 1);
            }
        }
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_distribution_index, (vx_reference*)&obj->histoObj.outHistoCh0[obj->enqueueCnt], 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_mean_index, (vx_reference*)&obj->histoObj.outMeanCh0[obj->enqueueCnt], 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_sd_index, (vx_reference*)&obj->histoObj.outSdCh0[obj->enqueueCnt], 1);
        }
        obj->enqueueCnt++;
        obj->enqueueCnt   = (obj->enqueueCnt  >= APP_BUFFER_Q_DEPTH)? 0 : obj->enqueueCnt;
        obj->pipeline++;
    }

    if((obj->pipeline > 0) && (status == VX_SUCCESS))
    {
        vx_object_array capture_input_arr;
        vx_image mosaic_output_image;
        vx_distribution output_histo;
        vx_scalar output_mean;
        vx_scalar output_sd;
        int32_t raw_input_graph_param_index;
        uint32_t num_refs;

        if (obj->input_streaming_type == INPUT_STREAM_FILE)
        {
            raw_input_graph_param_index = streamingObj->graph_parameter_index;
        }
        else
        {
            raw_input_graph_param_index = captureObj->graph_parameter_index;
        }

        /* Dequeue input */

        status = vxGraphParameterDequeueDoneRef(obj->graph, raw_input_graph_param_index, (vx_reference *)&capture_input_arr, 1, &num_refs);

        if (status == VX_SUCCESS)
        {
            if (obj->input_streaming_type == INPUT_STREAM_FILE)
            {
                app_find_object_array_index(streamingObj->raw_image_arr, (vx_reference)capture_input_arr, APP_BUFFER_Q_DEPTH, &obj_array_idx);
                if (obj_array_idx != -1)
                {
                    status= app_read_streaming(streamingObj, obj_array_idx);
                }
                else
                {
                    printf("Graph execution error: Error in dequeuing capture node for streaming, buffer not found !\n");
                }
            }
            if (status == VX_SUCCESS)
            {
                status = vxGraphParameterDequeueDoneRef(obj->graph, obj->histoObj.graph_parameter_distribution_index, (vx_reference *)&output_histo, 1, &num_refs);
                status |= vxGraphParameterDequeueDoneRef(obj->graph, obj->histoObj.graph_parameter_mean_index, (vx_reference *)&output_mean, 1, &num_refs);
                status |= vxGraphParameterDequeueDoneRef(obj->graph, obj->histoObj.graph_parameter_sd_index, (vx_reference *)&output_sd, 1, &num_refs);
            }
            else
            {
                printf("Error app_read_streaming()...\n");
            }
        }

        if((obj->en_out_img_write == 1) || (obj->test_mode == 1))
        {
            vx_char output_file_name[APP_MAX_FILE_PATH];

            /* Dequeue output */
            if (status == VX_SUCCESS)
            {
                status = vxGraphParameterDequeueDoneRef(obj->graph, imgMosaicObj->graph_parameter_index, (vx_reference*)&mosaic_output_image, 1, &num_refs);
            }
            if ((status == VX_SUCCESS) && (obj->test_mode == 1) && (frame_id > TEST_BUFFER))
            {
                /* calculate the checksum of the mosaic output */

                if ((app_test_check_image(mosaic_output_image, checksums_expected[obj->sensorObj.sensor_index][obj->sensorObj.num_cameras_enabled-1],
                                        &checksum_actual) != vx_true_e) && (frame_id > stability_frame))
                {
                    test_result = vx_false_e;
                    /* in case test fails and needs to change */
                    populate_gatherer(obj->sensorObj.sensor_index, obj->sensorObj.num_cameras_enabled-1, checksum_actual);
                }
            }

            if (obj->en_out_img_write == 1) {
                appPerfPointBegin(&obj->fileio_perf);
                snprintf(output_file_name, APP_MAX_FILE_PATH, "%s/mosaic_output_%010d_%dx%d.yuv", obj->output_file_path, (frame_id - APP_BUFFER_Q_DEPTH), imgMosaicObj->out_width, imgMosaicObj->out_height);
                if (status == VX_SUCCESS)
                {
                    status = writeMosaicOutput(output_file_name, mosaic_output_image);
                }
                appPerfPointEnd(&obj->fileio_perf);
            }
            /* Enqueue output */
            if (status == VX_SUCCESS)
            {
                status = vxGraphParameterEnqueueReadyRef(obj->graph, imgMosaicObj->graph_parameter_index, (vx_reference*)&mosaic_output_image, 1);
            }
        }

        /* Enqueue input - start execution */
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, raw_input_graph_param_index, (vx_reference *)&capture_input_arr, 1);
        }
        else
        {
            printf("Error dequeing..\n");
        }
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_distribution_index, (vx_reference*)&output_histo, 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_mean_index, (vx_reference*)&output_mean, 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_sd_index, (vx_reference*)&output_sd, 1);
        }
        else
        {
            printf("Error enqueuing..\n");
        }

        if (status != VX_SUCCESS)
        {
            printf("Error enqueuing..\n");
        }

        obj->enqueueCnt++;
        obj->dequeueCnt++;

        obj->enqueueCnt = (obj->enqueueCnt >= APP_BUFFER_Q_DEPTH)? 0 : obj->enqueueCnt;
        obj->dequeueCnt = (obj->dequeueCnt >= APP_BUFFER_Q_DEPTH)? 0 : obj->dequeueCnt;
    }

    appPerfPointEnd(&obj->total_perf);
    return status;
}

static void app_find_object_array_index(vx_object_array object_array[], vx_reference ref, vx_int32 array_size, vx_int32 *array_idx)
{
    vx_int32 i;

    *array_idx = -1;
    for(i = 0; i < array_size; i++)
    {
        tivx_raw_image img_ref = (tivx_raw_image)vxGetObjectArrayItem((vx_object_array)object_array[i], 0);
        if(ref == (vx_reference)img_ref)
        {
            *array_idx = i;
            tivxReleaseRawImage(&img_ref);
            break;
        }
        tivxReleaseRawImage(&img_ref);
    }
}

static vx_status app_run_graph(AppObj *obj)
{
    vx_status status = VX_SUCCESS;

    SensorObj *sensorObj = &obj->sensorObj;
    vx_int32 frame_id;

    app_pipeline_params_defaults(obj);
    APP_PRINTF("app_pipeline_params_defaults returned\n");

    if(NULL == sensorObj->sensor_name)
    {
        printf("sensor name is NULL \n");
        return VX_FAILURE;
    }

    if (obj->input_streaming_type == INPUT_STREAM_SERDES)
    {
        printf("Starting Sensor with channel mask= 0x%x\n", sensorObj->ch_mask );
        status= appStartImageSensor(sensorObj->sensor_name, sensorObj->ch_mask );
        APP_PRINTF("appStartImageSensor returned with status: %d\n", status);

        /* If number of cameras enabled is > 2, set camera pair # to 0, this will also make the de-serialzier stop streaming the other cameras, saving bandiwdth */
        if (obj->sensorObj.num_cameras_enabled >= 2)
        {
            obj->previousActivePair = obj->currentActivePair;
            obj->currentActivePair = 0;
            
            uint64_t t1= appLogGetTimeInUsec();
            app_send_cmd_viss_enableChan(&obj->vissObj, (APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE_MASK<<(obj->currentActivePair*APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE)));
            appActivateCameraPairs(obj->currentActivePair);
            app_send_cmd_histo_enableChan(&obj->histoObj, (APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE_MASK<<(obj->currentActivePair*APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE)));
            uint64_t diff= appLogGetTimeInUsec() - t1;
            printf("\n appActivateCameraPairs() time: %d ms\n", (uint32_t)diff/1000);
        }
    }

    if(0 == obj->enable_viss)
    {
        obj->vissObj.en_out_viss_write = 0;
    }

    if (obj->test_mode == 1) {
        // The buffer allows AWB/AE algos to converge before checksums are calculated
        obj->num_frames_to_run = TEST_BUFFER + 30;
    }

    for(frame_id = 0; ; frame_id++)
    {
        if(obj->write_file == 1)
        {
            uint32_t enableChanBitFlag;
            struct stat st = {0};
            /* D3: Create output directory if it doesn't already exists */
            if (stat(obj->captureObj.output_file_path, &st) == -1)
            {
                printf("Creating directory %s!\n", obj->captureObj.output_file_path);
                mkdir(obj->captureObj.output_file_path, 0777);
            }

            if (obj->input_streaming_type == INPUT_STREAM_SERDES)
            {
                if ((obj->captureObj.en_out_capture_write == 1) && (status == VX_SUCCESS))
                {
                    if (obj->currentActivePair == -1) /* If all the channels are active, save all the channels raw image*/
                    {
                        enableChanBitFlag= (1 << sensorObj->num_cameras_enabled) - 1;
                    }
                    else  /* otherwise save the raw image of the active channel pair only */
                    {
                        enableChanBitFlag= (APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE_MASK << (obj->currentActivePair * APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE));
                    }
                    status = app_send_cmd_capture_write_node(&obj->captureObj, enableChanBitFlag, frame_id + 1, obj->num_frames_to_write, obj->num_frames_to_skip, 0, NULL, NULL); /* we use frame_id+1 because we observed there could be a delay of 1 frame before the command is received by the node */
                }
            }

            if((obj->vissObj.en_out_viss_write == 1) && (status == VX_SUCCESS))
            {
                status = app_send_cmd_viss_write_node(&obj->vissObj, frame_id+1, obj->num_frames_to_write, obj->num_frames_to_skip); /* we use frame_id+1 because we observed there could be a delay of 1 frame before the command is received by the node */
            }
            if((obj->ldcObj.en_out_ldc_write == 1) && (status == VX_SUCCESS))
            {
                status = app_send_cmd_ldc_write_node(&obj->ldcObj, frame_id+1, obj->num_frames_to_write, obj->num_frames_to_skip); /* we use frame_id+1 because we observed there could be a delay of 1 frame before the command is received by the node */
            }

            if (obj->currentActivePair == -1) /* If all the channels are active, histogram is not running at all so disable write*/
            {
                enableChanBitFlag = 0;
                printf("Below only raw images are saved. Since histogram processing is disabled for all channels, no histogram data is saved. Press key 1, 2, 3, or 4 to enable histogram processing on corresponding channels and select 's' again to save.\n");
            }
            else /* otherwise save the histogram of the active channel pair only */
            {
                enableChanBitFlag = (APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE_MASK << (obj->currentActivePair * APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE));
            }

            status = app_send_cmd_histo_write_node(&obj->histoObj, enableChanBitFlag, frame_id+1, obj->num_frames_to_write, obj->num_frames_to_skip, obj->currentActivePair, 0); /* we use frame_id+1 because we observed there could be a delay of 1 frame before the command is received by the node */

            obj->write_file = 0;
        }

        if (status == VX_SUCCESS)
        {
            status = app_run_graph_for_one_frame_pipeline(obj, frame_id);
        }

        /* user asked to stop processing */
        if(obj->stop_task)
          break;
    }

    if (status == VX_SUCCESS)
    {
        status = vxWaitGraph(obj->graph);
    }
    obj->stop_task = 1;

    if (obj->input_streaming_type == INPUT_STREAM_SERDES)
    {
        if (status == VX_SUCCESS)
        {
            status = appStopImageSensor(obj->sensorObj.sensor_name, ((1 << sensorObj->num_cameras_enabled) - 1));
            APP_PRINTF("appStopImageSensor returned with status: %d\n", status);
        }
    }

    return status;
}

static void set_display_defaults(DisplayObj *displayObj)
{
    displayObj->display_option = 1;
}

static void app_pipeline_params_defaults(AppObj *obj)
{
    obj->pipeline       = -APP_BUFFER_Q_DEPTH + 1;
    obj->enqueueCnt     = 0;
    obj->dequeueCnt     = 0;
}

static void set_sensor_defaults(SensorObj *sensorObj)
{
    strcpy(sensorObj->sensor_name, SENSOR_HIMAX5530_UB953_D3);

    sensorObj->num_sensors_found = 0;
    sensorObj->sensor_features_enabled = 0;
    sensorObj->sensor_features_supported = 0;
    sensorObj->sensor_dcc_enabled = 0;
    sensorObj->sensor_wdr_enabled = 0;
    sensorObj->sensor_exp_control_enabled = 0;
    sensorObj->sensor_gain_control_enabled = 0;
}

static void app_default_param_set(AppObj *obj)
{
    set_sensor_defaults(&obj->sensorObj);

    set_display_defaults(&obj->displayObj);

    app_pipeline_params_defaults(obj);

    obj->is_interactive = 1;
    obj->test_mode = 0;
    obj->write_file = 0;
    obj->num_histo_bins= 1024;

    /* Disable histogram calculation for all channels */
    obj->histoObj.params.enableChanBitFlag= 0;

    obj->sensorObj.enable_ldc = 0;
    obj->sensorObj.num_cameras_enabled = 8;
    obj->sensorObj.ch_mask = 0xFF;
    obj->sensorObj.usecase_option = APP_SENSOR_FEATURE_CFG_UC0;
    obj->sensorObj.is_interactive= 0;
    obj->screendIdx= 0;
    obj->currentActivePair= -1; // Activate all cameras initially
    obj->previousActivePair= -1;
    obj->captureObj.enable_error_detection= 1;
}

static vx_int32 calc_grid_size(vx_uint32 ch)
{
    if(0==ch)
    {
        return -1;
    }
    else if(1==ch)
    {
        return 1;
    }
    else if(4>=ch)
    {
        return 2;
    }
    else if(9>=ch)
    {
        return 3;
    }
    else if(16>=ch)
    {
        return 4;
    }else
    {
        return -1;
    }
}

static void set_img_mosaic_params(ImgMosaicObj *imgMosaicObj, vx_uint32 in_width, vx_uint32 in_height, vx_int32 numCh)
{
    vx_int32 idx, ch;
    vx_int32 grid_size = calc_grid_size(numCh);

    imgMosaicObj->out_width    = DISPLAY_WIDTH;
    imgMosaicObj->out_height   = DISPLAY_HEIGHT;
    imgMosaicObj->num_inputs   = 1;

    idx = 0;

    tivxImgMosaicParamsSetDefaults(&imgMosaicObj->params);

    for(ch = 0; ch < numCh; ch++)
    {
        vx_int32 winX = ch%grid_size;
        vx_int32 winY = ch/grid_size;

        imgMosaicObj->params.windows[idx].startX  = (winX * (in_width/grid_size));
        imgMosaicObj->params.windows[idx].startY  = (winY * (in_height/grid_size));
        imgMosaicObj->params.windows[idx].width   = in_width/grid_size;
        imgMosaicObj->params.windows[idx].height  = in_height/grid_size;
        imgMosaicObj->params.windows[idx].input_select   = 0;
        imgMosaicObj->params.windows[idx].channel_select = ch;
        idx++;
    }

    imgMosaicObj->params.num_windows  = idx;

    /* Number of time to clear the output buffer before it gets reused */
    imgMosaicObj->params.clear_count  = APP_BUFFER_Q_DEPTH;
    imgMosaicObj->params.enable_overlay = 0;
}

static void app_update_param_set(AppObj *obj)
{

    vx_uint16 resized_width, resized_height;
    appIssGetResizeParams(obj->sensorObj.image_width, obj->sensorObj.image_height, DISPLAY_WIDTH, DISPLAY_HEIGHT, &resized_width, &resized_height);

    // set_img_mosaic_params(&obj->imgMosaicObj, resized_width, resized_height, (obj->sensorObj.num_cameras_enabled >= 4) ? 4 : obj->sensorObj.num_cameras_enabled);
    set_img_mosaic_params(&obj->imgMosaicObj, resized_width, resized_height, 2);
}

/*
 * Utility API used to add a graph parameter from a node, node parameter index
 */
static void add_graph_parameter_by_node_index(vx_graph graph, vx_node node, vx_uint32 node_parameter_index)
{
    vx_parameter parameter = vxGetParameterByIndex(node, node_parameter_index);

    vxAddParameterToGraph(graph, parameter);
    vxReleaseParameter(&parameter);
}

static vx_status app_set_cameras_to_display(AppObj *obj)
{
    vx_status status;
    status= VX_SUCCESS;

    if (obj->enable_mosaic)
    {
        vx_map_id map_id;
        tivxImgMosaicParams *pConfig;

        status = vxMapUserDataObject(obj->imgMosaicObj.config, 0, sizeof(tivxImgMosaicParams), &map_id,
                                     (void **)&pConfig, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0);
        pConfig->windows[0].channel_select = 4*obj->screendIdx;
        pConfig->windows[1].channel_select = pConfig->windows[0].channel_select + 1;
        //pConfig->windows[2].channel_select = pConfig->windows[1].channel_select + 1;
        //pConfig->windows[3].channel_select = pConfig->windows[2].channel_select + 1;
        vxUnmapUserDataObject(obj->imgMosaicObj.config, map_id);
    }
    else
    {
        printf("Mosaic display disabled (check camera format is RAW, not YUV)\n");
    }

    return status;
}

static void printfWhichActiveCameras(AppObj *obj)
{
    if (obj->screendIdx == 0)
    {
        if (obj->currentActivePair == -1)
        {
            printf("\n All cameras active with histogram disabled\n");
        }
        else
        {
            switch (obj->currentActivePair)
            {
            case 0:
                printf("\n Cameras 0-1 active with histogram running, cameras 2-3 inactive\n");
                break;

            case 1:
                printf("\n Cameras 0-1 inactive, cameras 2-3 active with histogram running\n");
                break;

            default:
                printf("\n Cameras 0-3 inactive\n");
                break;
            }
        }
    }
    else
    {
        if (obj->currentActivePair == -1)
        {
            printf("\n All cameras active with histogram disabled\n");
        }
        else
        {
            switch (obj->currentActivePair)
            {
            case 2:
                printf("\n Cameras 4-5 active with histogram running, camera 6-7 inactive\n");
                break;

            case 3:
                printf("\n Cameras 4-5 inactive, cameras 6-7 active with histogram running\n");
                break;

            default:
                printf("\n Cameras 4-7 inactive\n");
                break;
            }
        }
    }
}
