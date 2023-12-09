#include <TI/tivx.h>
#include <TI/tivx_task.h>
#include <TI/j7.h>
#include <TI/j7_imaging_aewb.h>
#include <tivx_utils_graph_perf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>

#include <iss_sensors.h>
#include <iss_sensor_if.h>

#include <utils/perf_stats/include/app_perf_stats.h>
#include <utils/iss/include/app_iss.h>

#include "app_sensor_module.h"
#include "app_cap_module.h"

#define ENABLE_VERBOSE 1
#define MAX_JSON_TOKENS 32

#define CAPT1_INST_ID       (0U)
#define CAPT2_INST_ID       (1U)

#define APP_BUFFER_Q_DEPTH   (4)
#define APP_PIPELINE_DEPTH   (7)
#define APP_MAX_FILE_PATH           (256u)
#define MAX_FNAME 					  (256u)
#define APP_ASSERT(x)               assert((x))
#define APP_ASSERT_VALID_REF(ref)   (APP_ASSERT(vxGetStatus((vx_reference)(ref))==VX_SUCCESS));
#define MAX_NUM_BUF  8
#define NUM_BUFS 4u
#define APP_MAX_BUFQ_DEPTH (4)

#define APP_USE_FILEIO

#define APP_MAX_NUM_CLASSES         (3u)
#define APP_MAX_TENSORS             (4u)
#define APP_MAX_TENSOR_DIMS         (4u)
#define APP_TIDL_MAX_PARAMS         (16u)

#define ABS_FLT(a) ((a) > 0)?(a):(-(a))

#define LDC_TABLE_WIDTH             (1920)
#define LDC_TABLE_HEIGHT            (1080)
#define LDC_DS_FACTOR               (2)
#define LDC_BLOCK_WIDTH             (64)
#define LDC_BLOCK_HEIGHT            (32)
#define LDC_PIXEL_PAD               (1)

/*Define this macro to enable prints*/
//#define APP_DEBUG_SINGLE_CAM

#ifdef APP_DEBUG_SINGLE_CAM
#define APP_PRINTF(f_, ...) printf((f_), ##__VA_ARGS__)
#else
#define APP_PRINTF(f_, ...)     
#endif
