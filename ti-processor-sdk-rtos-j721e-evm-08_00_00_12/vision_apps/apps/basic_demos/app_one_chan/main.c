/*
 *
 * Copyright (c) 2022 Openwater
 *
 */

#include <TI/tivx.h>
#include <TI/j7.h>
#include <tivx_utils_file_rd_wr.h>

#include <utils/draw2d/include/draw2d.h>
#include <utils/perf_stats/include/app_perf_stats.h>
#include <utils/console_io/include/app_get.h>
#include <utils/grpx/include/app_grpx.h>
#include <utils/iss/include/app_iss.h>
#include <utils/console_io/include/app_log.h>
#include <VX/vx_khr_pipelining.h>
#include <utils/remote_service/include/app_remote_service.h>

#include <TI/tivx_ow_algos.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>

#include <libtar.h>
#include <signal.h>

#include <fcntl.h>
#include <pthread.h> /* POSIX Threads */

#include "app_one_chan_common.h"
#include "app_sensor_module.h"
#include "app_cap_module.h"
#include "app_viss_module.h"
#include "app_aewb_module.h"
#include "app_ldc_module.h"
#include "app_histo_module.h"
#include "app_commands.h"
#include "app_messages.h"
#include "network.h"
#include "system_state.h"
#include "include/headSensorService.h"
#include "../../../git_version.h"


#define HISTOGRAM_ENABLE       (1U)


typedef struct {

    SensorObj     sensorObj;
    CaptureObj    captureObj;

#if (HISTOGRAM_ENABLE == 1U)
    /* Histogram object */
    HistoObj      histoObj;
#endif
    vx_char output_file_path[APP_MAX_FILE_PATH];

    /* OpenVX references */
    vx_context context;
    vx_graph   graph;
    vx_uint32  en_ignore_graph_errors;
    vx_uint32  en_ignore_error_state;

    vx_int32 test_mode;

    vx_uint32 is_interactive;
    vx_int32 input_streaming_type;

    vx_uint32 num_histo_bins;

    vx_uint32 num_frames_to_run;

    vx_uint32 num_frames_to_write;
    vx_uint32 num_frames_to_skip;

    vx_uint32 num_histo_frames_to_write;
    vx_uint32 num_histo_frames_to_skip;

    tivx_task task;
    vx_uint32 stop_task;
    vx_uint32 stop_task_done;

    app_perf_point_t total_perf;
    app_perf_point_t fileio_perf;
    app_perf_point_t draw_perf;

    int32_t pipeline;

    int32_t enqueueCnt;
    int32_t dequeueCnt;

    int32_t write_file;
    int32_t write_histo;
    int32_t scan_type;
    int32_t wait_histo;
    int32_t wait_file;
    int32_t wait_err;
    char wait_msg[256];
    char g_patient_path[64];
    char g_sub_path[32];
    char g_deviceID[24];

    uint32_t frame_sync_period;
    uint32_t frame_sync_duration;
    uint32_t strobe_delay_start;
    uint32_t strobe_duration;
    uint32_t timer_resolution;

    uint32_t dark_frame_count;
    uint32_t light_frame_count;
    uint32_t test_scan_frame_count;
    uint32_t full_scan_frame_count;

    uint32_t contact_thr_low_no_gain;
    uint32_t contact_thr_low_16_gain;
    uint32_t contact_thr_room_light;

    bool camera_pair_contact_result[4];

    uint32_t screendIdx; // Can be 0 or 1. 0 to show the first 4 cameras to the monitor and 1 to show the next 4 cameras

    int32_t currentActivePair; // 0,1,2,3 or -1 for all cameras active
    int32_t previousActivePair; // 0,1,2,3 or -1 for all cameras active

    int32_t currentActiveScanType; 
    int32_t previousActiveScanType;

    uint32_t num_histo_captures_long_scan;

    uint8_t camera_config_set[4];
    uint8_t camera_config_test;
    uint8_t camera_config_long;

} AppObj;

AppObj gAppObj;

static void app_parse_cmd_line_args(AppObj *obj, vx_int32 argc, vx_char *argv[]);
static vx_status app_init(AppObj *obj);
static void app_deinit(AppObj *obj);
static vx_status app_create_graph(AppObj *obj);
static vx_status app_verify_graph(AppObj *obj);
static vx_status app_run_graph(AppObj *obj, uint64_t phys_ptr, void *virt_ptr);
static vx_status app_run_graph_interactive(AppObj *obj);
static void app_delete_graph(AppObj *obj);
static void app_default_param_set(AppObj *obj);

static void app_run_task_delete(AppObj *obj);
static int32_t app_run_task_create(AppObj *obj);
static void app_run_task(void *app_var);
volatile sig_atomic_t sigkill_active = 0;
static bool reset_state(AppObj *obj);
static void app_pipeline_params_defaults(AppObj *obj);
static void set_sensor_defaults(SensorObj *sensorObj);
static void add_graph_parameter_by_node_index(vx_graph graph, vx_node node, vx_uint32 node_parameter_index);
static vx_status app_run_graph_for_one_frame_pipeline(AppObj *obj, vx_int32 frame_id);
static void app_parse_cfg_file(AppObj *obj, vx_char *cfg_file_name);
static void app_set_cfg_default(AppObj *obj);

static int32_t app_switch_camera_pair(AppObj *obj, int32_t pair_num, int32_t scan_type);
static uint32_t app_get_pair_mask(AppObj *obj, int32_t pair_num, int32_t scan_type);

static bool set_cam_pair(AppObj *obj, int pair);
static bool perform_scan(AppObj *obj, uint32_t num_frames, int32_t scan_type);

static bool run_scan_sequence(AppObj *obj, uint32_t dark_frames, uint32_t light_frames, uint32_t histo_frames, int32_t scan_type, uint32_t camera_pair);
int32_t appRemoteServiceHeadSensorHandler(char *service_name, uint32_t cmd, void *prm, uint32_t prm_size, uint32_t flags);
// static float fx(float f) { return isnan(f) ? 0 : f; }

static uint32_t goodSensorContact[8];
static uint32_t imageMean[4];

char usbPath[128];
char deviceName[512];
char s_generic_message[2048];

time_t current_time;
char time_string[50]={0};
char temp_string[50]={0};
struct tm* time_info;

// TODO: consider putting this back in uart.c and abstracting the file descriptor away from uart calls
int fd_serial_port = -1;

static void app_show_usage(vx_int32 argc, vx_char* argv[])
{
    printf("\n");
    printf(" Openwater Usecase - (c) Openwater 2022\n");
    printf(" ========================================================\n");
    printf("\n");
    printf(" Usage,\n");
    printf("  %s --cfg <config file>\n", argv[0]);
    printf("\n");
}

static char usecase_menu[] = {
    "\n"
    "\n ========================="
    "\n Openwater : Use Case"
    "\n Build Date: "
    GIT_BUILD_DATE
    "\n Version: "
    GIT_BUILD_VERSION
    "\n ========================="
    "\n"
    "\n 1-4: Switch to camera pair 1 through 4."
    "\n     Cameras are paired adjacently based on port index. Histogram enabled for the selected pair only."
    "\n"
    "\n t: Test Patient Scan"
    "\n f: Full Patient Scan"
    "\n h: Save Histogram"
    "\n s: Save Raw Image"
    "\n a: Save Raw Image and Histogram"
    "\n l: Enable Laser GPO"
    "\n k: Disable Laser GPO"
    "\n w: Change Laser Switch Channel"
    "\n"
    "\n v: view temp of camera sensors"
    "\n c: Read personality board temperature"
    "\n p: Print performance statistics"
    "\n u: D3 Utility Menu"
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

void set_usb_path(char device_letter, int8_t partition_number)
{
    if (partition_number > 0)
    {
        sprintf(usbPath, "/run/media/sd%c%u", device_letter, partition_number);
    }
    else
    {
        sprintf(usbPath, "/run/media/sd%c", device_letter);
    }
}

bool find_usb_stick()
{
    char device_letter;
    int8_t partition_number;
    char command[256];
    FILE *f = NULL;

    for (device_letter = 'a'; device_letter < 'g'; device_letter++)
    {
        // This iterates in reverse order because for example a USB stick may
        // mount as either /run/media/sda or /run/media/sda1. If mounted as
        // sda1 but we first look for sda we will think we found it on
        // sda rather than sda1
        for (partition_number = 7; partition_number >= 0; partition_number--)
        {
            set_usb_path(device_letter, partition_number);
            sprintf(command, "mount | grep %s", usbPath);
            f = popen(command, "r");
            if (NULL != f && EOF != fgetc(f))
            {
                pclose(f);
                return true;
            }
            if (NULL != f)
            {
                pclose(f);
            }
        }
    }
    return false;
}

int32_t appRemoteServiceHeadSensorHandler(char *service_name, uint32_t cmd,
    void *prm, uint32_t prm_size, uint32_t flags)
{
    int32_t status = 0;
    gAppObj.wait_err = 0;
    memset(gAppObj.wait_msg, 0, 256);

    if (NULL != prm)
    {
        typeHEAD_SENSOR_MESSAGE* pHSM = prm;
        switch(cmd)
        {
            case HS_CMD_UPDATE_SENSOR_STATE:            
                syslog(LOG_NOTICE, "[ONE_CHAN::appRemoteServiceHeadSensorHandler] HS_CMD_UPDATE_SENSOR_STATE called STATE: %d Mean: [%d,%d,%d,%d]", pHSM->state, pHSM->imageMean[0], pHSM->imageMean[1], pHSM->imageMean[2], pHSM->imageMean[3]);                        
                if(pHSM->state == 0)
                {
                    imageMean[0] = pHSM->imageMean[0];
                    imageMean[1] = pHSM->imageMean[1];
                    imageMean[2] = pHSM->imageMean[2];
                    imageMean[3] = pHSM->imageMean[3];
                    gAppObj.wait_file = 0;
                }
                else
                {               
                    syslog(LOG_ERR, "[ONE_CHAN::appRemoteServiceHeadSensorHandler] Update Sensor State Failure: %s", pHSM->msg);
                    gAppObj.wait_file = 0;
                    gAppObj.wait_err = 1;
                }

                break;
            case HS_CMD_HISTO_DONE_STATE:
                if(pHSM->state != 0)
                {
                    printf("HISTO Write Failure: %s\n", pHSM->msg);
                    strcpy(gAppObj.wait_msg, pHSM->msg);
                    gAppObj.wait_err = 1;
                    syslog(LOG_ERR, "[ONE_CHAN::appRemoteServiceHeadSensorHandler] HISTO Write Failure: %s", pHSM->msg);
                }
                gAppObj.wait_histo = 0;
                break;
            case HS_CMD_FILE_DONE_STATE:
                if(pHSM->state != 0)
                {
                    printf("IMAGE Write Failure: %s\n", pHSM->msg);
                    strcpy(gAppObj.wait_msg, pHSM->msg);
                    gAppObj.wait_err = 1;
                    syslog(LOG_ERR, "[ONE_CHAN::appRemoteServiceHeadSensorHandler] IMAGE Write Failure: %s", pHSM->msg);
                }
                gAppObj.wait_file = 0;

                break;
            case HS_CMD_STAT_MSG:
                break;
            case HS_CMD_CAMERA_STATUS:
                break;
            default:
                syslog(LOG_NOTICE, "[ONE_CHAN::appRemoteServiceHeadSensorHandler] Unhandled Command ID Recieved: %d", cmd);
                break;
        }

    }
    else
    {
        printf("ContactHandler: Invalid parameters passed !!!\n");
    }

    return status;
}

// Handler for system signals (such as SIGKILL)
void sigkill_handler(int signum)
{
    sigkill_active = 1;
}

vx_int32 app_one_chan_main(vx_int32 argc, vx_char* argv[])
{
    AppObj *obj = &gAppObj;
    vx_status status = VX_SUCCESS;
    struct stat st = {0};

    setlogmask (LOG_UPTO (LOG_NOTICE));
    openlog ("app_one_chan_main", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog(LOG_NOTICE, "[ONE_CHAN] Application Start Openwater Build Date: %s Version: %s", GIT_BUILD_DATE, GIT_BUILD_VERSION);

    printf("\n\n[ONE_CHAN] App Openwater\n");
    printf("[ONE_CHAN] Build Date: %s\n", GIT_BUILD_DATE);
    printf("[ONE_CHAN] GIT Version: %s\n\n", GIT_BUILD_VERSION);

    printf("[ONE_CHAN] Initialize Python\n");

    init_state(); // current state is startup

    /*Default parameter settings*/
    app_default_param_set(obj);

    /*Config parameter reading*/
    app_parse_cmd_line_args(obj, argc, argv);

#if ENABLE_VERBOSE
    printf("[ONE_CHAN] Number Of Cameras = [%d]\n",obj->sensorObj.num_cameras_enabled);
    printf("[ONE_CHAN] Camera Config TEST = [0x%02x]\n", obj->camera_config_test);
    printf("[ONE_CHAN] Camera Config LONG = [0x%02x]\n", obj->camera_config_long);
    printf("[ONE_CHAN] Camera SET 1 = [0x%02x]\n", obj->camera_config_set[0]);
    printf("[ONE_CHAN] Camera SET 2 = [0x%02x]\n", obj->camera_config_set[1]);
    printf("[ONE_CHAN] Camera SET 3 = [0x%02x]\n", obj->camera_config_set[2]);
    printf("[ONE_CHAN] Camera SET 4 = [0x%02x]\n", obj->camera_config_set[3]);
    printf("[ONE_CHAN] Camera channel mask set to 0x%02X\n", (uint8_t)obj->sensorObj.ch_mask);
    
    printf("[ONE_CHAN] frame_sync_period = [%d]\n", obj->frame_sync_period);
    printf("[ONE_CHAN] frame_sync_duration = [%d]\n", obj->frame_sync_duration);
    printf("[ONE_CHAN] strobe_delay_start = [%d]\n", obj->strobe_delay_start);
    printf("[ONE_CHAN] strobe_duration = [%d]\n", obj->strobe_duration);
    printf("[ONE_CHAN] timer_resolution = [%d]\n", obj->timer_resolution);

    printf("[ONE_CHAN] dark_frame_count = [%d]\n", obj->dark_frame_count);
    printf("[ONE_CHAN] light_frame_count = [%d]\n", obj->light_frame_count);
    printf("[ONE_CHAN] test_scan_frame_count = [%d]\n", obj->test_scan_frame_count);
    printf("[ONE_CHAN] full_scan_frame_count = [%d]\n", obj->full_scan_frame_count);
    printf("[ONE_CHAN] num_histo_captures_long_scan = [%d]\n", obj->num_histo_captures_long_scan);

    printf("[ONE_CHAN] contact_thr_low_light_no_gain = [%d]\n", obj->contact_thr_low_no_gain);
    printf("[ONE_CHAN] contact_thr_low_light_16_gain = [%d]\n", obj->contact_thr_low_16_gain);
    printf("[ONE_CHAN] contact_thr_room_light = [%d]\n", obj->contact_thr_room_light);

    printf("[ONE_CHAN] device_id = [%s]\n", obj->g_deviceID);
#endif

    // Setup up handler for SIGKILL signal to take the app down gracefully
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sigkill_handler;
    sigaction(SIGTERM, &action, NULL);

    // configure output directory with device id
    memset(obj->output_file_path, 0, APP_MAX_FILE_PATH);
    sprintf(obj->output_file_path, "%s/%s", obj->captureObj.output_file_path, obj->g_deviceID);

    printf("[ONE_CHAN] Creating output directory %s\n", obj->output_file_path);
    if (stat(obj->output_file_path, &st) == -1)
    {
        if (mkpath(obj->output_file_path, 0755) == -1) {            
            syslog(LOG_ERR, "[ONE_CHAN] Error trying to create output directory Error: %s\n", strerror(errno));
            printf("[ONE_CHAN] Some error trying to create output directory Error: %s\n", strerror(errno));
        }
    }
    
    memset(obj->captureObj.output_file_path, 0, APP_MAX_FILE_PATH);
    strcpy(obj->captureObj.output_file_path, obj->output_file_path);
#if (HISTOGRAM_ENABLE == 1U)
    memset(obj->histoObj.output_file_path, 0, APP_MAX_FILE_PATH);
    strcpy(obj->histoObj.output_file_path, obj->output_file_path);
#endif

#if ENABLE_VERBOSE
    printf("[ONE_CHAN] Created directory\n");
    printf("[ONE_CHAN] Device ID: %s\n", obj->g_deviceID);
    printf("[ONE_CHAN] Patient DIR: %s\n", obj->g_patient_path);
    printf("[ONE_CHAN] output path: %s\n", obj->output_file_path);
    printf("[ONE_CHAN] capture obj path: %s\n", obj->captureObj.output_file_path);
#if (HISTOGRAM_ENABLE == 1U)
    printf("[ONE_CHAN] histo obj path: %s\n", obj->histoObj.output_file_path);
#endif
#endif

    reset_state(obj);

    if(obj->en_ignore_error_state)
    {
        syslog(LOG_NOTICE, "[ONE_CHAN] Application set for IGNORING all Laser Errors");
    }

    if(current_state() == OW_STATE_ESTOP)
    {
        syslog(LOG_ERR, "[ONE_CHAN::Startup] E-Stop detected");
        return VX_FAILURE;
    }
    
    if(obj->en_ignore_error_state)
    {
        // ensuring we clear any laser errors before continuing
        if(!change_state(OW_STATE_IDLE, OW_UNDEFINED))
        {
            // check if it is an estop issue
        }
    }

    /* Querry sensor parameters */
    status = app_querry_sensor(&obj->sensorObj);
    if(status != VX_SUCCESS){
        syslog(LOG_ERR, "[ONE_CHAN] app_query_sensor Failed");      
    }
    
    /* Register App Remote Service */

#ifdef ENABLE_VERBOSE
    printf("[ONE_CHAN] Registering Service \n");
#endif
    status = appRemoteServiceRegister((char*)HEAD_SENSOR_SERVICE_NAME, appRemoteServiceHeadSensorHandler);
    if(status!=0)
    {
        printf("[ONE_CHAN] appRemoteServiceRegister: ERROR: Unable to register remote service Head Sensor handler\n");
        syslog(LOG_ERR, "[ONE_CHAN] appRemoteServiceRegister: ERROR: Unable to register remote service Head Sensor handler");        
    }

    if(current_state() != OW_STATE_IDLE){
        printf("[ONE_CHAN] Error state should be IDLE before going into interactive or headless mode SHUTTING DOWN\n");
        syslog(LOG_ERR, "Error state should be IDLE before going into interactive or headless mode SHUTTING DOWN");     
    }
    else
    {
        syslog(LOG_NOTICE, "[ONE_CHAN] Starting in interactive mode!");
        status = app_run_graph_interactive(obj);
    }


    /* UnRegister APP Remote Service */
#ifdef ENABLE_VERBOSE
    printf("[ONE_CHAN] UNRegistering Service \n");
#endif

    if(appRemoteServiceUnRegister((char*)HEAD_SENSOR_SERVICE_NAME)!=0)
    {
        printf("[ONE_CHAN] appRemoteServiceUnRegister: ERROR: Unable to un-register remote service Head Sensor handler\n");
        syslog(LOG_ERR, "[ONE_CHAN] appRemoteServiceUnRegister: ERROR: Unable to un-register remote service Head Sensor handler");
    }

    
    // exiting application no need to check if it passes or not
    change_state(OW_STATE_SHUTDOWN, OW_UNDEFINED);

    printf ("[ONE_CHAN] App Openwater application EXIT\n");
    syslog(LOG_NOTICE, "[ONE_CHAN] Application Exit");
    closelog ();
    return status;
}

static void add_graph_parameter_by_node_index(vx_graph graph, vx_node node, vx_uint32 node_parameter_index)
{
    vx_parameter parameter = vxGetParameterByIndex(node, node_parameter_index);

    vxAddParameterToGraph(graph, parameter);
    vxReleaseParameter(&parameter);
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

static vx_status app_run_graph(AppObj *obj, uint64_t phys_ptr, void *virt_ptr)
{
    vx_status status = VX_SUCCESS;

    SensorObj *sensorObj = &obj->sensorObj;
    vx_int32 frame_id;

#if 0        
    uint32_t cam_temp_vals = 0;
    uint32_t pair_mask = 0;
#endif

    app_pipeline_params_defaults(obj);

    if(NULL == sensorObj->sensor_name)
    {
        printf("[ONE_CHAN] sensor name is NULL \n");
        return VX_FAILURE;
    }

    /* set to pair 1 at startup */
    status |= app_switch_camera_pair(obj, 1, 0);

    for(frame_id = 0; ; frame_id++)
    {
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
        if(obj->write_file == 1)
        {
            printf("[ONE_CHAN] write file start \n");
            uint32_t enableChanBitFlag = 0x07; // for 3 channels
            struct stat st = {0};
            /* D3: Create output directory if it doesn't already exists */
            printf("[ONE_CHAN] checking for directory %s!\n", obj->captureObj.output_file_path);
            if (stat(obj->captureObj.output_file_path, &st) == -1)
            {
                printf("[ONE_CHAN] Creating directory %s!\n", obj->captureObj.output_file_path);
                mkdir(obj->captureObj.output_file_path, 0777);
            }

            if ((obj->captureObj.en_out_capture_write == 1) && (status == VX_SUCCESS))
            {
                printf("[ONE_CHAN] app_send_cmd_capture_write_node PatientPath: %s SubPath: %s \n", obj->g_patient_path, obj->g_sub_path);
                status = app_send_cmd_cap_write_node(&obj->captureObj, enableChanBitFlag, frame_id + 1, obj->num_frames_to_write, obj->num_frames_to_skip,  obj->scan_type, obj->g_patient_path, obj->g_sub_path, CAPT1_INST_ID); /* we use frame_id+1 because we observed there could be a delay of 1 frame before the command is received by the node */
            }

            if(status != VX_SUCCESS)
            {
                    printf("[ONE_CHAN] Error sending capture write node command ERROR: %d \n", status);
            }

            obj->write_file = 0;
            printf("[ONE_CHAN] write file finished \n");
        }

#if (HISTOGRAM_ENABLE == 1U)
        if(obj->write_histo == 1)
        {
            printf("[ONE_CHAN] write histo start \n");
            uint32_t enableChanBitFlag = 0x07; // for four channels
            status = app_send_cmd_histo_write_node(&obj->histoObj, enableChanBitFlag, frame_id+1, obj->num_histo_frames_to_write, obj->num_histo_frames_to_skip, obj->currentActivePair, obj->scan_type, obj->g_patient_path, obj->g_sub_path, virt_ptr, CAPT1_INST_ID); /* we use frame_id+1 because we observed there could be a delay of 1 frame before the command is received by the node */

            obj->write_histo = 0;
            printf("[ONE_CHAN] write histo finished \n");
        }
#endif
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

    if (status == VX_SUCCESS)
    {
        // pair 0 to shut off
        status |= app_switch_camera_pair(obj, 0, 0);
    }

    return status;
}

static vx_status app_run_graph_for_one_frame_pipeline(AppObj *obj, vx_int32 frame_id)
{
    vx_status status = VX_SUCCESS;

    appPerfPointBegin(&obj->total_perf);

    CaptureObj *captureObj = &obj->captureObj;


    /* This is the number of frames required for the pipeline AWB and AE algorithms to stabilize
        (note that 15 is only required for the 6-8 camera use cases - others converge quicker) */

    if(obj->pipeline < 0)
    {

        /* Enqueue inputs during pipeup dont execute */
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, captureObj->graph_parameter_index, (vx_reference *)&obj->captureObj.raw_image_arr[obj->enqueueCnt], 1);            
        }
#if (HISTOGRAM_ENABLE == 1U)
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_distribution_index, (vx_reference*)&obj->histoObj.outHistoCh0[obj->enqueueCnt], 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_mean_index, (vx_reference*)&obj->histoObj.outMeanCh0[obj->enqueueCnt], 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_sd_index, (vx_reference*)&obj->histoObj.outSdCh0[obj->enqueueCnt], 1);
        }
#endif        
        obj->enqueueCnt++;
        obj->enqueueCnt   = (obj->enqueueCnt  >= APP_BUFFER_Q_DEPTH)? 0 : obj->enqueueCnt;
        obj->pipeline++;
    }

    if((obj->pipeline == 0) && (status == VX_SUCCESS))
    {
        /* Execute 1st frame */
        if(status == VX_SUCCESS)
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, captureObj->graph_parameter_index, (vx_reference *)&obj->captureObj.raw_image_arr[obj->enqueueCnt], 1);            
        }
#if (HISTOGRAM_ENABLE == 1U)
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_distribution_index, (vx_reference*)&obj->histoObj.outHistoCh0[obj->enqueueCnt], 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_mean_index, (vx_reference*)&obj->histoObj.outMeanCh0[obj->enqueueCnt], 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_sd_index, (vx_reference*)&obj->histoObj.outSdCh0[obj->enqueueCnt], 1);
        }
#endif

        obj->enqueueCnt++;
        obj->enqueueCnt   = (obj->enqueueCnt  >= APP_BUFFER_Q_DEPTH)? 0 : obj->enqueueCnt;
        obj->pipeline++;
    }

    if((obj->pipeline > 0) && (status == VX_SUCCESS))
    {
        vx_object_array capture_input_arr;
#if (HISTOGRAM_ENABLE == 1U)
        vx_distribution output_histo;
        vx_scalar output_mean;
        vx_scalar output_sd;
#endif        
        int32_t raw_input_graph_param_index;
        uint32_t num_refs;

        raw_input_graph_param_index = captureObj->graph_parameter_index;

        /* Dequeue input */

        status = vxGraphParameterDequeueDoneRef(obj->graph, raw_input_graph_param_index, (vx_reference *)&capture_input_arr, 1, &num_refs);

#if (HISTOGRAM_ENABLE == 1U)
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterDequeueDoneRef(obj->graph, obj->histoObj.graph_parameter_distribution_index, (vx_reference *)&output_histo, 1, &num_refs);
            status |= vxGraphParameterDequeueDoneRef(obj->graph, obj->histoObj.graph_parameter_mean_index, (vx_reference *)&output_mean, 1, &num_refs);
            status |= vxGraphParameterDequeueDoneRef(obj->graph, obj->histoObj.graph_parameter_sd_index, (vx_reference *)&output_sd, 1, &num_refs);
         
        }
#endif

        /* Enqueue input - start execution */
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, raw_input_graph_param_index, (vx_reference *)&capture_input_arr, 1);
        }
        else
        {
            printf("[ONE_CHAN] Error dequeing..\n");
            syslog(LOG_ERR, "[ONE_CHAN::app_run_graph_for_one_frame_pipeline] Error dequeing");
        }

#if (HISTOGRAM_ENABLE == 1U)        
        if (status == VX_SUCCESS)
        {
            status = vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_distribution_index, (vx_reference*)&output_histo, 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_mean_index, (vx_reference*)&output_mean, 1);
            status |= vxGraphParameterEnqueueReadyRef(obj->graph, obj->histoObj.graph_parameter_sd_index, (vx_reference*)&output_sd, 1);
        }
        else
        {
            printf("[ONE_CHAN] Error enqueuing..\n");
            syslog(LOG_ERR, "[ONE_CHAN::app_run_graph_for_one_frame_pipeline] Error enqueuing raw input captures");
        }
#endif
        if (status != VX_SUCCESS)
        {
            printf("[ONE_CHAN] Error enqueuing histo data \n");
            syslog(LOG_ERR, "[ONE_CHAN::app_run_graph_for_one_frame_pipeline] Error enqueuing histo data");
        }

        obj->enqueueCnt++;
        obj->dequeueCnt++;

        obj->enqueueCnt = (obj->enqueueCnt >= APP_BUFFER_Q_DEPTH)? 0 : obj->enqueueCnt;
        obj->dequeueCnt = (obj->dequeueCnt >= APP_BUFFER_Q_DEPTH)? 0 : obj->dequeueCnt;
    }

    appPerfPointEnd(&obj->total_perf);
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
    
    printf("MAIN Temp PHYS_PTR: %p\n", (void*)phys_ptr);
    while((obj->stop_task == 0) && (status == VX_SUCCESS))
    {
        status = app_run_graph(obj, phys_ptr, virt_ptr);
    }

    appMemFree(APP_MEM_HEAP_DDR, virt_ptr, mem_size);

    obj->stop_task_done = 1;
}

static int32_t app_run_task_create(AppObj *obj)
{
    tivx_task_create_params_t params;
    vx_status status;

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

static bool int_token(vx_char *token, const char *tok_name, int *val)
{
    vx_char s[]=" \n";
    if(strcmp(token, tok_name) == 0)
    {
        token = strtok(NULL, s);
        if(token != NULL)
        {
            *val = atoi(token);
            return true;
        }
    }
    return false;
}

#define GET_NUM_TOKEN(tok_name, val) \
if (int_token(token, tok_name, &tok_val))\
{\
    val = tok_val;\
    continue;\
}

#define GET_BIT_TOKEN(tok_name, val) \
if (int_token(token, tok_name, &tok_val))\
{\
    if (tok_val > 1) tok_val = 1;\
    val = tok_val;\
    continue;\
}

#define GET_STR_TOKEN(tok_name, val) \
if(strcmp(token, tok_name) == 0)\
{\
    token = strtok(NULL, s);\
    if(token != NULL)\
    {\
        token[strlen(token)-1] = 0;\
        strcpy(val, token);\
    }\
    continue;\
}\

static int count_bits_enabled(uint8_t register_val) {
    int count = 0;
    for (int i = 0; i < 4; i++) {
        if ((register_val & (1 << i)) != 0) {
            count++;
        }
    }
    return count;
}

static bool test_half_bytes(uint8_t value) {
    int lower_count = 0, upper_count = 0;
    // Count set bits in lower half
    for (int i = 0; i < 4; i++) {
        if ((value & (1 << i)) != 0) {
            lower_count++;
        }
    }
    // Count set bits in upper half
    for (int i = 4; i < 8; i++) {
        if ((value & (1 << i)) != 0) {
            upper_count++;
        }
    }
    // Return true if both counts are less than or equal to 2
    return (lower_count <= 3) && (upper_count <= 3);
}

static void app_parse_cfg_file(AppObj *obj, vx_char *cfg_file_name)
{
    printf("[ONE_CHAN] Parse config file [%s]\n", cfg_file_name);
    FILE *fp = fopen(cfg_file_name, "r");
    vx_char line_str[1024];
    vx_char *token;

    obj->sensorObj.ch_mask = 0;

    if(fp==NULL)
    {
        printf("[ONE_CHAN] # ERROR: Unable to open config file [%s]\n", cfg_file_name);
        syslog(LOG_ERR, "[ONE_CHAN::app_parse_cfg_file] Unable to open config file %s", cfg_file_name);
        exit(0);
    }

    while(fgets(line_str, sizeof(line_str), fp)!=NULL)
    {
        int tok_val;
        vx_char s[]=" \t";

        if (strchr(line_str, '#'))
        {
            continue;
        }

        /* get the first token */
        token = strtok(line_str, s);
        if(token != NULL)
        {
            // printf("[ONE_CHAN] Process Token [%s]\n", token);
            GET_NUM_TOKEN("sensor_index", obj->sensorObj.sensor_index)
            GET_NUM_TOKEN("num_frames_to_run", obj->num_frames_to_run)
            GET_BIT_TOKEN("enable_error_detection", obj->captureObj.enable_error_detection)
            GET_BIT_TOKEN("enable_ldc", obj->sensorObj.enable_ldc)
            GET_BIT_TOKEN("en_ignore_graph_errors", obj->en_ignore_graph_errors)
            GET_BIT_TOKEN("en_out_capture_write", obj->captureObj.en_out_capture_write)
            GET_BIT_TOKEN("is_interactive", obj->is_interactive)
            GET_BIT_TOKEN("ignore_error_state", obj->en_ignore_error_state)
            GET_NUM_TOKEN("usecase_option", obj->sensorObj.usecase_option)
            GET_NUM_TOKEN("num_frames_to_write", obj->num_frames_to_write)
            GET_NUM_TOKEN("num_frames_to_skip", obj->num_frames_to_skip)
            GET_NUM_TOKEN("frame_sync_period", obj->frame_sync_period)
            GET_NUM_TOKEN("frame_sync_duration", obj->frame_sync_duration)
            GET_NUM_TOKEN("strobe_delay_start", obj->strobe_delay_start)
            GET_NUM_TOKEN("strobe_duration", obj->strobe_duration)
            GET_NUM_TOKEN("timer_resolution", obj->timer_resolution)
            GET_NUM_TOKEN("num_histo_bins", obj->num_histo_bins)
            GET_NUM_TOKEN("dark_frame_count", obj->dark_frame_count)
            GET_NUM_TOKEN("light_frame_count", obj->light_frame_count)
            GET_NUM_TOKEN("test_scan_frame_count", obj->test_scan_frame_count)
            GET_NUM_TOKEN("full_scan_frame_count", obj->full_scan_frame_count)
            GET_NUM_TOKEN("num_histo_frames_to_write", obj->num_histo_frames_to_write)
            GET_NUM_TOKEN("num_histo_frames_to_skip", obj->num_histo_frames_to_skip)
            GET_NUM_TOKEN("contact_thr_low_light_no_gain", obj->contact_thr_low_no_gain)
            GET_NUM_TOKEN("contact_thr_low_light_16_gain", obj->contact_thr_low_16_gain)
            GET_NUM_TOKEN("contact_thr_room_light", obj->contact_thr_room_light)
            GET_STR_TOKEN("device_id", obj->g_deviceID)
            GET_NUM_TOKEN("num_histo_captures_long_scan", obj->num_histo_captures_long_scan)

            if(strcmp(token, "output_file_path") == 0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    token[strlen(token)-1]=0;
                    strcpy(obj->captureObj.output_file_path, token);
#if (HISTOGRAM_ENABLE == 1U)
                    strcpy(obj->histoObj.output_file_path, token);
#endif
                    strcpy(obj->output_file_path, token);
                }
                continue;
            }

            if(strcmp(token, "camera_config_test") == 0)
            {
                GET_NUM_TOKEN("camera_config_test", obj->camera_config_test)     
            }

            if(strcmp(token, "camera_config_long") == 0)
            {
                GET_NUM_TOKEN("camera_config_long", obj->camera_config_long)     
            }
            
            if(strcmp(token, "camera_set_1") == 0)
            {
                GET_NUM_TOKEN("camera_set_1", obj->camera_config_set[0])     
            }
            
            if(strcmp(token, "camera_set_2") == 0)
            {
                GET_NUM_TOKEN("camera_set_2", obj->camera_config_set[1])
            }
                        
            if(strcmp(token, "camera_set_3") == 0)
            {
                GET_NUM_TOKEN("camera_set_3", obj->camera_config_set[2])
            }
                        
            if(strcmp(token, "camera_set_4") == 0)
            {
                GET_NUM_TOKEN("camera_set_4", obj->camera_config_set[3])
            }

            if(strcmp(token, "test_mode") == 0)
            {
                token = strtok(NULL, s);
                if(token != NULL)
                {
                    obj->test_mode = atoi(token);
                    obj->captureObj.test_mode = atoi(token);
                }
                continue;
            }

        }
    }

    // get just left channel
   
    obj->sensorObj.ch_mask = 0; // Initialize channel mask
    obj->sensorObj.num_cameras_enabled = 0; // Initialize count of enabled cameras

    // calculate channel mask and number of cameras enabled
    for (int cam_check = 0; cam_check < 4; cam_check++){
        if(!test_half_bytes(obj->camera_config_set[cam_check])){
            printf("Camera SET %d has an invalid configuration 0x%02x\n", cam_check + 1, obj->camera_config_set[cam_check]);
            obj->camera_config_set[cam_check] = 0x00; // set it to disabled
        }
        obj->sensorObj.ch_mask |= (uint8_t)(obj->camera_config_set[cam_check] & 0x0F);
    }

    obj->sensorObj.num_cameras_enabled = count_bits_enabled((uint8_t)obj->sensorObj.ch_mask);

    fclose(fp);

}

static void app_set_cfg_default(AppObj *obj)
{
    memset(obj->g_deviceID, 0, 24);
    strcpy(obj->g_deviceID, "device000");

    snprintf(obj->captureObj.output_file_path,APP_MAX_FILE_PATH, ".");

    obj->captureObj.en_out_capture_write = 0;
    obj->num_frames_to_write = 0;
    obj->num_frames_to_skip = 0;

    obj->num_histo_frames_to_write = 0;
    obj->num_histo_frames_to_skip = 0;

    obj->camera_config_set[0] = 0x1B;
    obj->camera_config_set[1] = 0x99;
    obj->camera_config_set[2] = 0xAA;
    obj->camera_config_set[3] = 0x33;
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
        if(strcmp(argv[i], "--cfg") == 0)
        {
            i++;
            if(i>=argc)
            {
                app_show_usage(argc, argv);
            }
            app_parse_cfg_file(obj, argv[i]);
        }
        else
        if(strcmp(argv[i], "--help") == 0)
        {
            app_show_usage(argc, argv);
            exit(0);
        }
        else
        if(strcmp(argv[i], "--test") == 0)
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
        if(strcmp(argv[i], "--sensor") == 0)
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
        obj->is_interactive = 1;
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
    else
    {
        syslog(LOG_ERR, "[ONE_CHAN::app_init] Error creating context");
        APP_PRINTF("Error creating context!\n");
    }

    /* Initialize modules */

    if (status == VX_SUCCESS)
    {
        tivxOwAlgosLoadKernels(obj->context);
        if(status == VX_SUCCESS)
        {
            APP_PRINTF("tivxOwAlgosLoadKernels done!\n");
        }
        else
        {
            syslog(LOG_ERR, "[ONE_CHAN::app_init] Error loading c66 Kernels");
            APP_PRINTF("Error loading c66 Kernels!\n");
        }
    }

    AppD3boardInitCmdParams cmdPrms;

    cmdPrms.moduleId = APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC;
    cmdPrms.fSyncPeriod = obj->frame_sync_period;
    cmdPrms.fSyncDuration = obj->frame_sync_duration;
    cmdPrms.strobeLightDelayStart = obj->strobe_delay_start;
    cmdPrms.strobeLightDuration = obj->strobe_duration;
    cmdPrms.timerResolution = obj->timer_resolution;

    // TODO: Determine which personality board here
    int32_t d3board_init_status = appInitD3board(&cmdPrms);
    if (0 != d3board_init_status)
    {
        /* Not returning failure because application may be waiting for
        error/test frame */
        syslog(LOG_ERR, "[ONE_CHAN::app_init] Error initializing D3 board");
        printf("[ONE_CHAN] Error initializing D3 board\n");
        status = VX_FAILURE;
    }

    if (status == VX_SUCCESS)
    {
        status = app_init_sensor(&obj->sensorObj, "sensor_obj");
        APP_PRINTF("Sensor init done!\n");
    }
    else
    {
        syslog(LOG_ERR, "[ONE_CHAN::app_init] tivxOwAlgosLoadKernels load Failed");
    }

    if (status == VX_SUCCESS)
    {
        printf("==================> Sensor Image Size %dx%d\n", obj->sensorObj.image_width, obj->sensorObj.image_height);
        status = app_init_cap(obj->context, &obj->captureObj, &obj->sensorObj, "capture_obj", APP_BUFFER_Q_DEPTH, CAPT1_INST_ID);
        APP_PRINTF("Capt init done!\n");
    }
    else
    {
        syslog(LOG_ERR, "[ONE_CHAN::app_init] Sensor init Failed");
    }

#if (HISTOGRAM_ENABLE == 1U)
    if (status == VX_SUCCESS)
    {
        /* Enable histogram processing of channel #0 and channel #1
           Note that actual swapping of camera pair is done by function app_switch_camera_pair()
           The settings below remain constant throughout the application
        */
        obj->histoObj.params.enableChanBitFlag= 0; /* Disable histogram computation for all channels at the beginning */
        status = app_init_histo(obj->context, &obj->histoObj, &obj->sensorObj, "histo_obj", APP_BUFFER_Q_DEPTH, obj->num_histo_bins, count_bits_enabled((uint8_t)obj->sensorObj.ch_mask));
        APP_PRINTF("Histo init done!\n");
    }
    else
    {
        syslog(LOG_ERR, "[ONE_CHAN::app_init] Capture init Failed");
    }
#endif

    appPerfPointSetName(&obj->total_perf , "TOTAL");
    appPerfPointSetName(&obj->fileio_perf, "FILEIO");
    return status;
}

static void app_deinit(AppObj *obj)
{
    appDeInitD3board();

    app_deinit_sensor(&obj->sensorObj);
    printf("Sensor deinit done!\n");

    app_deinit_cap(&obj->captureObj, APP_BUFFER_Q_DEPTH);
    printf("Capture deinit done!\n");
#if (HISTOGRAM_ENABLE == 1U)
    app_deinit_histo(&obj->histoObj, APP_BUFFER_Q_DEPTH);
    printf("Histo deinit done!\n");
#endif
    tivxHwaUnLoadKernels(obj->context);
    tivxImagingUnLoadKernels(obj->context);
    tivxFileIOUnLoadKernels(obj->context);
    tivxOwAlgosUnLoadKernels(obj->context);
    printf("Kernels unload done!\n");

    vxReleaseContext(&obj->context);
    printf("Release context done!\n");
}

static vx_status app_create_graph(AppObj *obj)
{
    vx_status status = VX_SUCCESS;

#if (HISTOGRAM_ENABLE == 1U)
    vx_object_array raw_input;
#endif

    vx_graph_parameter_queue_params_t graph_parameters_queue_params_list[5];
    vx_int32 graph_parameter_index;

    obj->graph = vxCreateGraph(obj->context);
    status = vxGetStatus((vx_reference)obj->graph);
    if (status == VX_SUCCESS)
    {
        status = vxSetReferenceName((vx_reference)obj->graph, "app_openwater_graph");
        APP_PRINTF("Graph create done!\n");
    }

    if (status == VX_SUCCESS)
    {
        status = app_create_cap_graph_capture(obj->graph, &obj->captureObj, CAPT1_INST_ID);
        APP_PRINTF("Capture graph done!\n");
    }

#if (HISTOGRAM_ENABLE == 1U)
    raw_input = obj->captureObj.raw_image_arr[0];
    if(status == VX_SUCCESS)
    {
        status = app_create_graph_histo(obj->graph, &obj->histoObj, raw_input, CAPT1_INST_ID);
        APP_PRINTF("Histo graph done!\n");
    }
#endif
    if(status == VX_SUCCESS)
    {
        graph_parameter_index = 0;
        
        add_graph_parameter_by_node_index(obj->graph, obj->captureObj.node, 1);
        obj->captureObj.graph_parameter_index = graph_parameter_index;
        graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
        graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
        graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference *)&obj->captureObj.raw_image_arr[0];
        graph_parameter_index++;

#if (HISTOGRAM_ENABLE == 1U)        
        add_graph_parameter_by_node_index(obj->graph, obj->histoObj.node, TIVX_KERNEL_OW_CALC_HISTO_HISTO_IDX);
        obj->histoObj.graph_parameter_distribution_index = graph_parameter_index;
        graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
        graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
        graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference*)&obj->histoObj.outHistoCh0[0];
        graph_parameter_index++;

        add_graph_parameter_by_node_index(obj->graph, obj->histoObj.node, TIVX_KERNEL_OW_CALC_HISTO_MEAN_IDX);
        obj->histoObj.graph_parameter_mean_index = graph_parameter_index;
        graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
        graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
        graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference*)&obj->histoObj.outMeanCh0[0];
        graph_parameter_index++;

        add_graph_parameter_by_node_index(obj->graph, obj->histoObj.node, TIVX_KERNEL_OW_CALC_HISTO_SD_IDX);
        obj->histoObj.graph_parameter_sd_index = graph_parameter_index;
        graph_parameters_queue_params_list[graph_parameter_index].graph_parameter_index = graph_parameter_index;
        graph_parameters_queue_params_list[graph_parameter_index].refs_list_size = APP_BUFFER_Q_DEPTH;
        graph_parameters_queue_params_list[graph_parameter_index].refs_list = (vx_reference *)&obj->histoObj.outSdCh0[0];
        graph_parameter_index++;
#endif

        status = vxSetGraphScheduleConfig(obj->graph,
                VX_GRAPH_SCHEDULE_MODE_QUEUE_AUTO,
                graph_parameter_index,
                graph_parameters_queue_params_list);

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
      status = tivxExportGraphToDot(obj->graph,".", "vx_app_one_chan");
    }
    #endif

    if (((obj->captureObj.enable_error_detection) || (obj->test_mode)) && (status == VX_SUCCESS))
    {
        status = app_send_err_frame(&obj->captureObj);
    }

    /* wait a while for prints to flush */
    tivxTaskWaitMsecs(100);

    return status;
}


static bool set_cam_pair(AppObj *obj, int pair)
{
    if(current_state() != OW_STATE_READY){
        syslog(LOG_ERR, "[ONE_CHAN::set_cam_pair] Must be in ready state to switch cameras, current state: %s", OW_SYSTEM_STATE_STRING[current_state()]);
        printf("[ONE_CHAN::set_cam_pair] Must be in ready state to switch cameras, current state: %s\n", OW_SYSTEM_STATE_STRING[current_state()]);        
        return false;
    }

    if(pair < 1 || pair > 4)
    {
        // error
        printf("[ONE_CHAN] Selected camera pair must be in the range of 1-4");        
       return false;
    }
    else
    {
        if(app_switch_camera_pair(obj, pair, 0)==0)
        {
            // success            
        }
        else
        {
            printf("[ONE_CHAN::set_cam_pair] %s\n", OW_APP_ERR_MESSAGE_STRING[OW_MSG_ERR_SWITCH_CAMERA]);
            return false;
        }
    }

    return true;
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
            printf("[ONE_CHAN::owWriteImageToFile] Timed out during frame capture to disk\n");
            syslog(LOG_ERR, "[ONE_CHAN::owWriteImageToFile] Timed out during frame capture to disk");
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
                syslog(LOG_ERR, "[GateKeeper] Timed out during wait for file");
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
        syslog(LOG_ERR, "[GateKeeper] Error writing to file: %s", obj->wait_msg);
    }


    return bSuccess;
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
            syslog(LOG_ERR, "Timed out during frame histogram capture to disk");
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
                syslog(LOG_ERR, "[GateKeeper] Timed out during wait for histo");
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
        syslog(LOG_ERR, "[GateKeeper] Error writing histogram: %s", obj->wait_msg);
    }

    return bSuccess;

}

static bool run_scan_sequence(AppObj *obj, uint32_t dark_frames, uint32_t light_frames, uint32_t histo_frames, int32_t scan_type, uint32_t camera_pair)
{
    bool bFailed = false;
    printf("enter run sequence\n");

    // set scan type
    obj->scan_type = scan_type;

    // dark capture
    // ensure laser is off (camera pair selected before running test scan)
    printf("turn laser off\n");
    if(scan_type==0 || scan_type == 2) // full scan 
    {
        syslog(LOG_NOTICE, "full scan write %d dark frames", dark_frames);
        if(!owWriteImageToFile(obj, dark_frames, 0, true, 60)){
            // error
            printf("[ONE_CHAN] Failure writing dark frames.\n");
            syslog(LOG_ERR, "[ONE_CHAN::run_scan_sequence] Failure writing dark frames");
            bFailed = true;
            return bFailed;
        }
    }
    else
    {
        //Test scan
        syslog(LOG_NOTICE, "test scan write 2 dark frames");
        if(!owWriteImageToFile(obj, 2, 0, true, 60)){
            // error
            printf("[ONE_CHAN] Failure writing dark frames.\n");
            syslog(LOG_ERR, "[ONE_CHAN::run_scan_sequence] Failure writing dark frames");
            bFailed = true;
            return bFailed;
        }

        //Test the sensor for ambient light
        if( imageMean[0]!=UINT32_MAX && imageMean[1]!=UINT32_MAX  && imageMean[2]!=UINT32_MAX  && imageMean[3]!=UINT32_MAX )
        {
            printf("[ONE_CHAN] Checking ambient light for pair %u, mean ch0 is %u, mean ch1 is %u, mean ch2 is %u, mean ch3 is %u  and the threshold is %u\n",
                    camera_pair, imageMean[0], imageMean[1], imageMean[2], imageMean[3], obj->contact_thr_room_light);
            if( imageMean[0]>=obj->contact_thr_room_light )
                goodSensorContact[0] |= 1;
            if( imageMean[1]>=obj->contact_thr_room_light )
                goodSensorContact[1] |= 1;
            if( imageMean[2]>=obj->contact_thr_room_light )
                goodSensorContact[2] |= 1;
            if( imageMean[3]>=obj->contact_thr_room_light )
                goodSensorContact[3] |= 1;
        }
        imageMean[0] = UINT32_MAX; imageMean[1] = UINT32_MAX; imageMean[2] = UINT32_MAX; imageMean[3] = UINT32_MAX;
    }

    // light capture
    // ensure laser is on (camera pair selected before running test scan)
    printf("turn laser on\n");
    usleep(100000);
    
    if(scan_type==0 || scan_type == 2) // full scan 
    {
        syslog(LOG_NOTICE, "full scan write %d light frames", light_frames);
        if(!bFailed && !owWriteImageToFile(obj, light_frames, 0, true, 60)){
            // error
            printf("[ONE_CHAN] Failure writing light frames.\n");
            syslog(LOG_ERR, "[ONE_CHAN::run_scan_sequence] Failure writing light frames");
            bFailed = true;
            return bFailed;
        }
    }
    else
    {
        syslog(LOG_NOTICE, "test scan write 2 light frames");
        if(!bFailed && !owWriteImageToFile(obj, 2, 0, true, 60)){
            // error
            printf("[ONE_CHAN] Failure writing light frames.\n");
            syslog(LOG_ERR, "[ONE_CHAN::run_scan_sequence] Failure writing light frames");
            bFailed = true;
            return bFailed;
        }

    }

    syslog(LOG_NOTICE, "write histo frames");
    if(!bFailed && !owWriteHistoToFile(obj, histo_frames, 0, true, 60))
    {
        // error
        printf("[ONE_CHAN] Failure writing histogram frames.\n");
        syslog(LOG_ERR, "[ONE_CHAN::run_scan_sequence] Failure writing histogram frames");
        bFailed = true;
        return bFailed;
    }
    
    if(scan_type==1) //Test scan - check for laser light
    {
        uint32_t laserLightThresholdHorizontal = obj->contact_thr_low_16_gain;
        uint32_t laserLightThresholdNear = obj->contact_thr_low_no_gain;
        if( imageMean[0]!=UINT32_MAX && imageMean[1]!=UINT32_MAX  && imageMean[2]!=UINT32_MAX  && imageMean[3]!=UINT32_MAX )
        {
            printf("[ONE_CHAN] Checking ambient light for pair %u, mean ch0 is %u, mean ch1 is %u, mean ch2 is %u, mean ch3 is %u  and the threshold is %u (ch0,ch2) %u (ch1,ch3)\n",
                    camera_pair, imageMean[0], imageMean[1], imageMean[2], imageMean[3], laserLightThresholdHorizontal, laserLightThresholdNear );
            if( imageMean[0]<laserLightThresholdHorizontal )
                goodSensorContact[0] |= 2;
            if( imageMean[1]<laserLightThresholdHorizontal )
                goodSensorContact[1] |= 2;
            if( imageMean[2]<laserLightThresholdHorizontal )
                goodSensorContact[2] |= 2;
            if( imageMean[3]<laserLightThresholdHorizontal )
                goodSensorContact[3] |= 2;
        }
        imageMean[0] = UINT32_MAX; imageMean[1] = UINT32_MAX; imageMean[2] = UINT32_MAX; imageMean[3] = UINT32_MAX;
    }

    // ensure laser is off
    printf("turn laser off\n");
    usleep(100000);

    syslog(LOG_NOTICE, "write 2 dark histo frames");
    obj->scan_type = 3;  // set to dark histo scan type
    if(!bFailed && !owWriteHistoToFile(obj, 2, 0, true, 10))
    {
        // error
        printf("[ONE_CHAN] Failure writing histogram frames.\n");
        syslog(LOG_ERR, "[ONE_CHAN::run_scan_sequence] Failure writing dark histogram frames");
        bFailed = true;
        return bFailed;
    }

    // set scan type back just in case
    obj->scan_type = scan_type;
    
    printf("exit run sequence\n");
    return bFailed;
}

static bool perform_scan(AppObj *obj, uint32_t num_frames, int32_t scan_type)
{
    struct stat st = {0};
    char current_scan_path[1024] = {0};
    memset(current_scan_path, 0, 1024);

    /* Obtain current time. */
    time_t now = time(NULL);
    if (now == -1)
    {
        // error
        printf("[ONE_CHAN] Failure to obtain the current time.\n");
        syslog(LOG_ERR, "[ONE_CHAN::perform_scan] Failure to obtain the current time");
        return false;
    }

    syslog(LOG_NOTICE, "[ONE_CHAN::perform_scan] CURRENT STATE: %s", OW_SYSTEM_STATE_STRING[current_state()]);

    if (current_state() != OW_STATE_READY)
    {        
        syslog(LOG_ERR, "[ONE_CHAN::perform_scan] Failed, scan not allowed in current state");
        // error
        change_state(OW_STATE_READY, OW_UNDEFINED);
        return false;
    }

    if(!change_state(OW_STATE_BUSY, OW_STATE_READY))
    {
        printf("[ONE_CHAN::perform_scan] Failed to change state to STATE: %s FROM STATE: %s\n", OW_SYSTEM_STATE_STRING[OW_STATE_BUSY], OW_SYSTEM_STATE_STRING[OW_STATE_IDLE]);
        syslog(LOG_ERR, "[ONE_CHAN::perform_scan] Failed to change state current state: %s", OW_SYSTEM_STATE_STRING[current_state()]);
        // error
        change_state(OW_STATE_READY, OW_UNDEFINED);
        return false;
    }

    syslog(LOG_NOTICE, "[ONE_CHAN::perform_scan] CURRENT STATE: %s", OW_SYSTEM_STATE_STRING[current_state()]);

    // create scan directory
    memset(obj->g_sub_path,0,32);
    struct tm *ptm = localtime(&now);
    char scan_type_string[12] = {0};
    switch(scan_type){
        case 1: // test scan
            strcpy(scan_type_string, "TESTSCAN_3C");
            break;
        case 2: // long scan
            strcpy(scan_type_string, "LONGSCAN_3C");
            break;
        case 0: // full scan
        default:
            strcpy(scan_type_string, "FULLSCAN_3C");
            break;
    }
    printf("[ONE_CHAN] Current TIME: %02d/%02d/%04d %02d:%02d:%02d\n", ptm->tm_mon + 1,  ptm->tm_mday, ptm->tm_year + 1900, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    snprintf((char*)obj->g_sub_path, 31, "%.11s_%04d_%02d_%02d_%02d%02d%02d", scan_type_string, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    sprintf(current_scan_path, "%s/%s/%s", obj->captureObj.output_file_path, obj->g_patient_path, obj->g_sub_path);
    if (stat(current_scan_path, &st) == -1)
    {
        if (mkdir(current_scan_path, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
            printf("Error: %s\n", strerror(errno));
            syslog(LOG_ERR, "[ONE_CHAN::perform_scan] Error trying to create patient directory Error: %s\n", strerror(errno));
            printf("[ONE_CHAN] Some error trying to create patient directory Error: %s\n", strerror(errno));
            change_state(OW_STATE_READY, OW_UNDEFINED);
            return false;
        }
    }
    
    syslog(LOG_NOTICE, "[ONE_CHAN::perform_scan] CURRENT STATE: %s CURRENT SCAN PATH: %s", OW_SYSTEM_STATE_STRING[current_state()], current_scan_path);

    if(scan_type==1) //Test scan
    {
        for(uint32_t i=0; i<8; ++i)
            goodSensorContact[i] = 0;
        imageMean[0] = 0; imageMean[1] = 0; imageMean[2] = 0; imageMean[3] = 0;
    }

    for(int pair_count = 0; pair_count < 4; pair_count++)
    {
        if((scan_type==1 || scan_type==2) && pair_count > 0)
            break; // finished only doing pair 0 for long and test scan skip the remaining pairs

        // check E-Stop
        if(current_state() == OW_STATE_ESTOP)
        {
            // abort current process
            syslog(LOG_ERR, "[ONE_CHAN::perform_scan] E-Stop detected aborting scan process");
            return false;
        }

        if(app_get_pair_mask(obj, pair_count + 1, scan_type) == 0x00) // camera setting is disabled in config with 0
        {
            printf("Camera config %d disabled in configuration file\n", pair_count + 1);
            syslog(LOG_WARNING, "[ONE_CHAN::perform_scan] Camera config %d disabled in configuration file", pair_count + 1);
            continue;
        }

        // set camera pair
       if(app_switch_camera_pair(obj, pair_count + 1, scan_type) < 0) 
        {
            // error
            change_state(OW_STATE_READY, OW_UNDEFINED);
            return false;
        }

        usleep(100000);
        
        if(scan_type==2){ // iff a long scan
            
            printf("[ONE_CHAN] Scanning in LONG mode, TOTAL CAPTURES: %d\n", obj->num_histo_captures_long_scan);

            for(uint32_t scan_round =0;scan_round < obj->num_histo_captures_long_scan;scan_round++){
                printf("[ONE_CHAN] Scanning in LONG mode, round %d\n", scan_round);
                
                memset(s_generic_message, 0, sizeof(s_generic_message));
                sprintf(s_generic_message, "Scanning... (%d/%d)", scan_round+1,obj->num_histo_captures_long_scan);

                if(run_scan_sequence(obj, obj->dark_frame_count, obj->light_frame_count, num_frames, scan_type, scan_round))
                {
                    change_state(OW_STATE_READY, OW_UNDEFINED);
                    return false;
                }
                
                if(current_state() == OW_STATE_ESTOP)
                {
                    // abort current process
                    syslog(LOG_ERR, "[ONE_CHAN::perform_scan] E-Stop detected aborting scan process");
                    return false;
                }
            }
        }
        else {

            if(run_scan_sequence(obj, obj->dark_frame_count, obj->light_frame_count, num_frames, scan_type, pair_count))
            {
                change_state(OW_STATE_READY, OW_UNDEFINED);
                return false;
            }
        }

    }

    if(scan_type==1) //Test scan
    {
        char s_contact_data[64] = {0};
        sprintf(s_contact_data, "[%d,%d,%d,%d]", goodSensorContact[1], goodSensorContact[3], goodSensorContact[0],  goodSensorContact[2]);
        printf("!!!!!!!!Contact data: %s",s_contact_data);    
    }

    if(!change_state(OW_STATE_READY, OW_UNDEFINED))
    {
        syslog(LOG_ERR, "[ONE_CHAN::perform_scan] Failed, scan was not able to set back to ready state after scan");
        // error
        return false;
    }
    
    return true;
}

static bool reset_state(AppObj *obj)
{
    if (!(current_state() == OW_STATE_IDLE || current_state() == OW_STATE_ESTOP))
    {
        // cleeanup close anything in process
        printf("[ONE_CHAN] IDLE Complete!\n");

        // TODO - what should the current expected state be?
        if(!change_state(OW_STATE_IDLE, OW_UNDEFINED))
        {
            return false;
        }
    }

    return true;
}

static uint32_t app_get_pair_mask(AppObj *obj, int32_t pair_num, int32_t scan_type){
    uint32_t mask = 0x00;

    if(scan_type == 1) // test scan
    {
        mask = obj->camera_config_test;
        return mask;
    }
    else if(scan_type == 2) // long scan
    {
        mask = obj->camera_config_long;
        return mask;
    }

    // full scan
    switch(pair_num){
        case -1: // all on (maybe or all the config sets)
            mask = obj->sensorObj.ch_mask;
            break;
        case 0: // all off
            mask = 0x00;
            break;
        case 1:
            // mask = 0xC3;  // port 0 and 1 on CSI0
            mask = obj->camera_config_set[0];
            break;
        case 2:
            // mask = 0x33;  // port 0 and 1 on CSI0  and port 0 and 1 on CSI1
            mask = obj->camera_config_set[1];
            break;
        case 3:
            // mask = 0xC3;
            mask = obj->camera_config_set[2];
            break;
        case 4:
            // mask = 0x33;
            mask = obj->camera_config_set[3];
            break;
        default:
            printf("===============>>>>>>>> Unkown pair %i\n", pair_num);
            syslog(LOG_WARNING, "[ONE_CHAN::app_get_pair_mask] Camera Pair Unkown, Requested pair: %i", pair_num);
            break;
    }
    
    return mask;
}

static int32_t app_switch_camera_pair(AppObj *obj, int32_t pair_num, int32_t scan_type)
{
    int32_t status = 0;
    uint32_t pair_mask = 0x00;
    if (obj->sensorObj.num_cameras_enabled > 0)
    {
        if(pair_num == 0)
        {
            // turn off streams and return
        }

        obj->previousActivePair = obj->currentActivePair;
        obj->currentActivePair = pair_num;
        pair_mask = app_get_pair_mask(obj, obj->previousActivePair, obj->previousActiveScanType);
        
        // no need to switch
        printf("[ONE_CHAN] prevPair: %d currPair: %d pairMask: 0x%02x\n", obj->previousActivePair, obj->currentActivePair, pair_mask);
        printf("[ONE_CHAN] Stopping FrameSync\n");
        appStopFrameSync();
        // stop forwarding
        printf("[ONE_CHAN] Setting mask to 0\n");
        status |= appActivateCameraMask(0);
        // stop current
        printf("[ONE_CHAN] Stopping image sensors pairMask: 0x%02x\n", pair_mask);
        status = appStopImageSensor(obj->sensorObj.sensor_name, pair_mask);
        
        // switching to new camera pair
        obj->previousActiveScanType = obj->currentActiveScanType;
        obj->currentActiveScanType = scan_type;
        pair_mask = app_get_pair_mask(obj, obj->currentActivePair, scan_type);
        if(pair_mask !=0){
            printf("[ONE_CHAN] Starting FrameSync\n");
            appStartFrameSync();
        }

        printf("[ONE_CHAN] Starting image sensors pairMask: 0x%02x\n", pair_mask);
        status= appStartImageSensor(obj->sensorObj.sensor_name, pair_mask);
        printf("[ONE_CHAN] Setting pairMask: 0x%02x\n", pair_mask);
        status |= appActivateCameraMask(pair_mask);
#if (HISTOGRAM_ENABLE == 1U)        
        printf("[ONE_CHAN] histogram\n");
        status |= app_send_cmd_histo_change_pair(&obj->histoObj, pair_num);
        status |= app_send_cmd_histo_enableChan(&obj->histoObj, 0x07);
#endif        
    }
    else
    {
        // no need to switch
        printf("[ONE_CHAN] Number of cameras enabled %d can't switch\n", obj->sensorObj.num_cameras_enabled );
        status = 1;
    }

    return status;
}

static vx_status app_run_graph_interactive(AppObj *obj)
{
    vx_status status = VX_SUCCESS;

    printf("[ONE_CHAN] app_run_graph_interactive \n");
    syslog(LOG_NOTICE, "[ONE_CHAN::app_run_graph_interactive] Interactive mode starting");

    uint32_t done = 0;
    uint32_t cam_temp_output = 0;  
    float temp =0;
 
    // uint32_t pair_mask = 0;
    char ch;

    // initialize graph and start task

    printf("[ONE_CHAN] Initializing APP\n");
    status = app_init(obj);
    if (status != VX_SUCCESS)
    {
        printf("[ONE_CHAN] Failed to Initialize APP\n");
        syslog (LOG_ERR, "[ONE_CHAN::app_run_graph_interactive] Failed to Initialize APP");
        if (obj->en_ignore_graph_errors == 0)
        {
            return status;
        }
    }

    printf("[ONE_CHAN] APP INIT DONE! Creating Graph\n");
    status = app_create_graph(obj);
    if (status != VX_SUCCESS)
    {
        syslog(LOG_ERR, "[ONE_CHAN::app_run_graph_interactive] Failed to Create Graph");
        printf("[ONE_CHAN] Failed to Create Graph\n");
        if (obj->en_ignore_graph_errors == 0)
        {
            return status;
        }
    }

    printf("[ONE_CHAN] App Create Graph Done!\n");
    status = app_verify_graph(obj);
    if (status != VX_SUCCESS)
    {
        syslog(LOG_ERR, "[ONE_CHAN::app_run_graph_interactive] Failed to Verify Graph");
        printf("[ONE_CHAN] Failed to Verify Graph\n");
        if (obj->en_ignore_graph_errors == 0)
        {
            return status;
        }
    }

    printf("[ONE_CHAN] App Verify Graph Done! Creating Task\n");
    status = app_run_task_create(obj);
    if (status != VX_SUCCESS)
    {
        syslog(LOG_ERR, "[ONE_CHAN::app_run_graph_interactive] Failed to Create Task");
        printf("[ONE_CHAN] Failed to create Task\n");
        if (obj->en_ignore_graph_errors == 0)
        {
            return status;
        }
    }

    printf("[ONE_CHAN] Task started\n");

    appPerfStatsResetAll();
    if(!change_state(OW_STATE_READY, OW_UNDEFINED))
    {
        // send message and shutdown if estop
    }

    // Wait 2 sec before displaying the menu so that MCU has finished displaying its messages
    sleep(2);

    while(!done)
    {
        printf("Build Date: %s\n", GIT_BUILD_DATE);
        printf("GIT Version: %s\n\n", GIT_BUILD_VERSION);
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
                printf("switch camera to pair: %d requested\n", ((int)ch - (int)48));
                if(set_cam_pair(obj, ((int)ch - (int)48)))
                {
                    printf("set camera pair to: %d\n", ((int)ch - (int)48));
                }
                else
                {
                    printf("Failed to set camera pair, response was NULL\n");
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
                
                vx_reference refs[1];
                refs[0] = (vx_reference)obj->captureObj.raw_image_arr[0];
                if (status == VX_SUCCESS)
                {
                    status = tivxNodeSendCommand(obj->captureObj.node, 0u,
                                                TIVX_CAPTURE_PRINT_STATISTICS,
                                                refs, 1u);
                }
            
                break;
            case 'f':
                printf("[ONE_CHAN] Perform full scan requested\n");
                if(perform_scan(obj, obj->full_scan_frame_count, 0))
                {
                    printf("[ONE_CHAN] Full scan completed successfully\n");
                }
                else
                {
                    printf("[ONE_CHAN] Failed to perform full scan, response was NULL\n");
                }
                break;
            case 'h':
                printf("[ONE_CHAN] Perform single histogram capture requested\n");
                owWriteHistoToFile(obj, 1, 0, true, 60);
                break;
            case 't':
                printf("[ONE_CHAN] Perform test scan requested\n");
                if(perform_scan(obj, obj->test_scan_frame_count, 1))
                {
                    printf("[ONE_CHAN] Test scan completed successfully\n");
                }
                else
                {
                    printf("[ONE_CHAN] Failed to perform test scan, response was NULL\n");
                }
                break;
            case 's':
                printf("[ONE_CHAN] Perform single image capture requested\n");
                owWriteImageToFile(obj, 1, 0, true, 60);
                break;
            case 'a':
                owWriteImageToFile(obj, 1, 0, true, 60);
                owWriteHistoToFile(obj, 1, 0, true, 60);
                break;
            case 'l':
                printf("[ONE_CHAN] Enable Laser requested\n");
                // enable laser
                break;
            case 'k':
                printf("[ONE_CHAN] Disable Laser requested\n");
                // disable laser
                break;
            case 'u':
                run_d3_utils_menu();
                break;
            case 'v':
                printf("[ONE_CHAN] View Camera sensor tempseratures\n");
                // pair_mask = app_get_pair_mask(obj->currentActivePair);
                // cam_temp_output = appGetTempCameraPair(pair_mask);
                printf("Temperatures PAIR: %d\tCAM %d: 0x%02x\tCAM %d: 0x%02x\n", obj->currentActivePair, (uint8_t)(cam_temp_output>>24), (uint8_t)(cam_temp_output>>16), (uint8_t)(cam_temp_output>>8), (uint8_t)cam_temp_output);                
                break;
            case 'w':
                // change laser channel
                printf("\n\nEnter channel number:\n");
                do {
                    ch = appGetChar();
                } while(ch == '\n' || ch == '\r');
                break;
            case 'z':
                // printf("[ONE_CHAN] send histo time sync\n");            
                // app_send_cmd_histo_time_sync(&obj->histoObj, 0);
                printf("[ONE_CHAN] Local Time\n");   
                {                    

                    // Get the current time
                    time(&current_time);
                    // Convert it to local time
                    time_info = localtime(&current_time);
                    
                    // Format the time as a string
                    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);
                    
                    printf("CURRENT STATE: %s SYSTEM TIME: %s\n", OW_SYSTEM_STATE_STRING[current_state()], time_string);

                }
                break;

            case 'c':
                temp = appGetPersonalityBoardTemp();
                printf("Temp: %f", temp);
                break;
            case 'x':
                printf("[ONE_CHAN] Exiting Application\n");
                obj->stop_task = 1;
                done = 1;
                break;

        }

        // Escape from event loop if process has been killed
        if(sigkill_active){
            obj->stop_task = 1;
            done = 1;
            change_state(OW_STATE_ESTOP, OW_UNDEFINED);
        }
    }

    printf("[ONE_CHAN] Run Task Delete\n");
    app_run_task_delete(obj);

    app_delete_graph(obj);
    printf("[ONE_CHAN] App Delete Graph Done! \n");

    app_deinit(obj);
    printf("[ONE_CHAN] App De-init Done! \n");

    // exiting no reason to check
    change_state(OW_STATE_IDLE, OW_UNDEFINED);
    syslog(LOG_NOTICE, "[ONE_CHAN::app_run_graph_interactive] Interactive mode exiting");

    return status;
}

static void app_delete_graph(AppObj *obj)
{
    
    app_delete_cap(&obj->captureObj);
    APP_PRINTF("Capture delete done!\n");
    
#if (HISTOGRAM_ENABLE == 1U)
    app_delete_histo(&obj->histoObj);
    APP_PRINTF("Histo delete done!\n");
#endif
    vxReleaseGraph(&obj->graph);
    APP_PRINTF("Graph delete done!\n");
}

static void app_default_param_set(AppObj *obj)
{

    set_sensor_defaults(&obj->sensorObj);


    app_pipeline_params_defaults(obj);

    obj->is_interactive = 1;
    obj->test_mode = 0;
    obj->write_file = 0;
    obj->write_histo = 0;
    obj->scan_type = 0;
    obj->wait_histo = 0;
    obj->wait_file = 0;
    obj->num_histo_bins= 1024;
#if (HISTOGRAM_ENABLE == 1U)
    /* Disable histogram calculation for all channels */
    obj->histoObj.params.enableChanBitFlag= 0;
#endif
    obj->sensorObj.enable_ldc = 0;
    obj->sensorObj.num_cameras_enabled = 8;
    obj->sensorObj.ch_mask = 0x00;
    obj->sensorObj.usecase_option = APP_SENSOR_FEATURE_CFG_UC0;
    obj->sensorObj.is_interactive= 0;
    obj->screendIdx= 0;
    obj->currentActivePair= -1; // Activate all cameras initially
    obj->previousActivePair= -1;
    obj->captureObj.enable_error_detection= 1;
    
    obj->currentActiveScanType = 0;
    obj->previousActiveScanType = 0;
}
