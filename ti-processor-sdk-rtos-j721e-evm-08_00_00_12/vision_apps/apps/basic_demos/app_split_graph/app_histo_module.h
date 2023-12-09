#ifndef _APP_HISTO_MODULE
#define _APP_HISTO_MODULE

#include <TI/tivx.h>
#include <TI/tivx_task.h>
#include <TI/tivx_target_kernel.h>
#include "tivx_kernels_host_utils.h"

#include <TI/j7.h>
#include <TI/j7_tidl.h>
#include <tivx_utils_file_rd_wr.h>
#include <tivx_utils_graph_perf.h>
#include <utils/iss/include/app_iss.h>
#include <TI/tivx_img_proc.h>
#include <TI/tivx_fileio.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <float.h>
#include <math.h>

#include <utils/console_io/include/app_get.h>
#include "app_sensor_module.h"

#include <tivx_kernel_ow_calc_histo.h>

#define APP_MAX_BUFQ_DEPTH (4)
#define APP_MAX_FILE_PATH  (256u)

#define APP_HISTO_NUM_EXTRA_PARAMS 2
#define APP_ASSERT(x)               assert((x))
#define APP_ASSERT_VALID_REF(ref)   (APP_ASSERT(vxGetStatus((vx_reference)(ref))==VX_SUCCESS));

typedef struct {
    vx_node  node;
    vx_node  write_node;

    vx_user_data_object config;
    tivxC66HistoParams params;

    vx_object_array outHistoArr[APP_MAX_BUFQ_DEPTH];
    vx_distribution outHistoCh0[APP_MAX_BUFQ_DEPTH];

    vx_object_array outMeanArr[APP_MAX_BUFQ_DEPTH];
    vx_scalar outMeanCh0[APP_MAX_BUFQ_DEPTH];

    vx_object_array outSdArr[APP_MAX_BUFQ_DEPTH];
    vx_scalar outSdCh0[APP_MAX_BUFQ_DEPTH];

    vx_int32 graph_parameter_distribution_index;
    vx_int32 graph_parameter_mean_index;
    vx_int32 graph_parameter_sd_index;
    vx_char objName[APP_MAX_FILE_PATH];
    uint32_t num_channels;
    uint32_t range;
    uint32_t numBins;
    vx_array file_path;
    vx_array file_prefix;
    vx_user_data_object write_cmd;
    vx_user_data_object cmd;
    vx_char output_file_path[APP_MODULES_MAX_OBJ_NAME_SIZE];
    vx_user_data_object time_sync;
} HistoObj;

vx_status app_init_histo(vx_context context, HistoObj *histoObj, SensorObj *sensorObj, char *objName, vx_int32 bufq_depth, uint32_t num_histo_bins, uint32_t chan_count);
void app_deinit_histo(HistoObj *histoObj, vx_int32 bufq_depth);
void app_delete_histo(HistoObj *histoObj);
vx_status app_create_graph_histo(vx_graph graph,
                           HistoObj *histoObj,
                           vx_object_array inputRawImageArr,
                           int instID);
vx_status app_create_graph_histo_write_output(vx_graph graph, HistoObj *histoObj);
vx_status app_send_cmd_histo_write_node(HistoObj *histoObj, vx_uint32 enableChanBitFlag, vx_uint32 start_frame, vx_uint32 num_frames, vx_uint32 num_skip, vx_int32 cam_pair, vx_int32 scan_type, const char* patient_path, const char* sub_path, void* virt_ptr);
vx_status app_send_cmd_histo_enableChan(HistoObj *histoObj, uint32_t enableChanBitFlag);
vx_status app_send_cmd_histo_change_pair(HistoObj *histoObj, uint32_t chanelpair);
vx_status app_send_cmd_histo_time_sync(HistoObj *histoObj, uint64_t ref_ts);

#endif
