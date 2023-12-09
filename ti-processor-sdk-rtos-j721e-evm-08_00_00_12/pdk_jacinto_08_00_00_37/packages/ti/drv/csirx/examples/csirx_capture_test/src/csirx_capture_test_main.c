/*
 *  Copyright (c) Texas Instruments Incorporated 2018-2019
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 *  \file csirx_capture_test_main.c
 *
 *  \brief CSI RX Capture Example.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdio.h>
#include <string.h>

#include <ti/csl/tistdtypes.h>
#include <ti/csl/csl_types.h>
#include <ti/csl/soc.h>
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/drv/csirx/csirx.h>
#include <ti/drv/i2c/I2C.h>
#include <ti/drv/i2c/soc/I2C_soc.h>
#include <ti/board/src/devices/common/common.h>
#include <ti/board/board.h>
#include <ti/board/src/devices/board_devices.h>
#include "imx390.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/**< Application name */
#define APP_NAME                        "CSIRX_CAPT_APP"

/**< Driver instance to use */
#define APP_CAPT_INST                             (CSIRX_INSTANCE_ID_0)
/**< Number of channels */
#define APP_CAPT_CH_NUM                           ((uint32_t)4U)
/**< Input Image Data format */
#define APP_CAPT_IMAGE_DT                         (FVID2_CSI2_DF_RAW12)
/**< Frame storage format. Only valid for RAW12 DT. */
#define APP_CAPT_IMAGE_STORAGE_FORMAT             (FVID2_CCSF_BITS12_UNPACKED16)
/**< Number of frames to capture. Set it to '0' for indefinite capture */
#define APP_CAPT_FRAMES_CAPT                      ((uint32_t)1000U)
/**< Number of frames per stream */
#define APP_CAPT_FRAMES_PER_CH                    ((uint32_t)4U)
/**< Frame Attribute: Width in pixels */
#define APP_CAPT_FRAME_WIDTH                      ((uint32_t)1936U)
/**< Frame Attribute: Height in pixels */
#define APP_CAPT_FRAME_HEIGHT                     ((uint32_t)1100U)
/**< Frame Attribute: Bytes per pixel */
#define APP_CAPT_FRAME_BPP                        ((uint32_t)2U)

/**< I2C transaction timeout */
#define APP_I2C_TRANSACTION_TIMEOUT               ((uint32_t)2000U)

/**
 * @{
 * Macros to control Fusion board and Camera Sensor version for the capture
 */
/**< Fusion Board Revision.
     '0': for Rev B or older boards.
     '1': for Rev C board. */
#define FUSION_BOARD_VER                     (1U)
/**< D3 IMX390 sensor version.
     '0': for D3_IMX390_CM module.
     '1': for D3_IMX390_RCM module. */
#define D3IMX390_MODULE_VER                  (0U)
/** @} */

/**< Frame Attribute: Pitch in bytes */
#define APP_CAPT_FRAME_PITCH                      ((uint32_t)\
                                (APP_CAPT_FRAME_WIDTH * APP_CAPT_FRAME_BPP))
/**< Frame Attribute: size in bytes */
#define APP_CAPT_FRAME_SIZE                                ((uint32_t)\
            (APP_CAPT_FRAME_HEIGHT * APP_CAPT_FRAME_WIDTH * APP_CAPT_FRAME_BPP))

/* Print buffer character limit for prints- UART or CCS Console */
#define APP_PRINT_BUFFER_SIZE                   ((uint32_t)4000)

/** \brief Log enable for CSIRX Sample application */
#define CsirxAppTrace                       ((uint32_t) GT_INFO   |\
                                             (uint32_t) GT_TraceState_Enable)

/**< Maximum number of error frame logs to store.
     It stores most recent errors.*/
#define APP_ERR_FRAME_LOG_MAX                                  ((uint32_t)500U)

/**< Print Driver Logs. Set to '1' to enable printing. */
#define APP_PRINT_DRV_LOGS                                     (0U)

#define SENSOR_CFG_SIZE  (3075)

/**
 * @{
 * I2C Addresses for serialisers/Sensors attached to the UB960
 */
#define UB960_SERIALISER_ADDR                (0x18)
#if (D3IMX390_MODULE_VER == 0)
#define D3IMX390_SENSOR_ADDR                 (0x21)
#endif
#if (D3IMX390_MODULE_VER == 1)
#define D3IMX390_SENSOR_ADDR                 (0x1A)
#endif
/** @} */

/**
 * @{
 * Generic Alias Addresses for serialisers attached to the UB960
 */
#define D3IMX390_UB960_PORT_0_SER_ADDR       (0x74U)
#define D3IMX390_UB960_PORT_1_SER_ADDR       (0x76U)
#define D3IMX390_UB960_PORT_2_SER_ADDR       (0x78U)
#define D3IMX390_UB960_PORT_3_SER_ADDR       (0x7AU)
/** @} */

/**
 * @{
 * Generic Alias Addresses for sensors attached to the UB960
 */
#define D3IMX390_UB960_PORT_0_SENSOR_ADDR    (0x40U)
#define D3IMX390_UB960_PORT_1_SENSOR_ADDR    (0x42U)
#define D3IMX390_UB960_PORT_2_SENSOR_ADDR    (0x44U)
#define D3IMX390_UB960_PORT_3_SENSOR_ADDR    (0x46U)
/** @} */

/**< Number of channels */
#define APP_CAPT_CH_MAX                           ((uint32_t)4U)
/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */
/**
 *  \brief Capture application object.
 */
typedef struct
{
    uint32_t instId;
   /**< Csirx Drv Instance ID. */
   uint32_t boardCsiInstID;
   /**< Csirx Drv Instance ID. */
    Csirx_InitParams initPrms;
   /**< Csirx init time parameters */
    Csirx_CreateParams createPrms;
   /**< Csirx create time parameters */
    Csirx_CreateStatus createStatus;
   /**< Csirx create time status */
    Fvid2_Handle drvHandle;
   /**< FVID2 capture driver handle. */
    Fvid2_CbParams cbPrms;
   /**< Callback params. */
    uint32_t numFramesToCapture;
   /**< Number of frames to receive for a given configuration */
    volatile uint32_t numFramesRcvd;
   /**< Number of frames received */
    uint32_t frameErrorCnt;
   /**< Number of erroneous frames received */
    uint32_t maxWidth;
   /**< Max width in pixels - used for buffer allocation for all instance. */
    uint32_t maxHeight;
   /**< Max height in lines - used for buffer allocation for all instance. */
    Fvid2_Frame frames[APP_CAPT_FRAMES_PER_CH * APP_CAPT_CH_NUM];
   /**< FVID2 Frames that will be used for capture. */
    Csirx_InstStatus captStatus;
   /**< CSIRX Capture status. */
    uint32_t chFrmCnt[APP_CAPT_CH_NUM];
   /**< Number of frames captured per channel. */
    uint32_t errFrmCh[APP_ERR_FRAME_LOG_MAX];
   /**< Channel to which error frame belongs. */
    uint32_t errFrmNo[APP_ERR_FRAME_LOG_MAX];
   /**< Error frame number for the channel. */
    uint32_t errFrmTs[APP_ERR_FRAME_LOG_MAX];
   /**< TS in ms. */
}appCaptObj;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
/**
 * \brief   This function is wrapper function used to print message on
 *          respective consoles
 *
 * \param   pcString        Print contents.
 *
 * \retval  none.
 */
static void App_consolePrintf(const char *pcString, ...);

/**
 * \brief   This function is used to initialize test parameters
 *
 * \param   appObj          Type of print message.
 *                          Refer to struct #appCaptObj
 *
 * \retval  none.
 */
static void App_initCaptParams(appCaptObj* appObj);

/**
 * \brief   App Init function.
 *
 * \param   appObj          CSI RX Capture Test Object
 *
 * \retval  status          FVID2_SOK on successful
 *                          else otherwise.
 */
static int32_t App_init(appCaptObj* appObj);

/**
 * \brief   App create function.
 *
 * \param   appObj        CSI RX Capture Test Object
 *
 * \retval  status          FVID2_SOK on successful
 *                          else otherwise.
 */
static int32_t App_create(appCaptObj* appObj);

/**
 * \brief   App CSI test function: captures frames.
 *
 * \param   appObj        CSI RX Capture Test Object
 *
 * \retval  status          FVID2_SOK on successful
 *                          else otherwise.
 */
static int32_t App_csiTest(appCaptObj* appObj);

/**
 * \brief   App delete function.
 *
 * \param   appObj        CSI RX Capture Test Object
 *
 * \retval  status          FVID2_SOK on successful
 *                          else otherwise.
 */
static int32_t App_delete(appCaptObj* appObj);

/**
 * \brief   App Init function.
 *
 * \param   appObj        CSI RX Capture Test Object
 *
 * \retval  status          FVID2_SOK on successful
 *                          else otherwise.
 */
static int32_t App_deinit(appCaptObj* appObj);

/**
 * \brief   App Callback function for frame completion.
 *
 * \param   handle        Fvid2 DRV handle
 *
 * \param   appData       App based back by to CB function
 *
 * \param   reserved      reserved, not used
 *
 * \retval  status          FVID2_SOK on successful
 *                          else otherwise.
 */
static int32_t App_frameCompletionCb(Fvid2_Handle handle,
                                     Ptr appData,
                                     Ptr reserved);

/**
 * \brief   App Callback function for frame completion.
 *
 * \param   appObj        CSI RX Capture Test Object
 *
 * \retval  status          FVID2_SOK on successful
 *                          else otherwise.
 */
static int32_t App_allocAndQFrames(appCaptObj *appObj);

/**
 * \brief   App Callback function for frame completion.
 *
 * \param   appObj        CSI RX Capture Test Object
 *
 * \retval  status          FVID2_SOK on successful
 *                          else otherwise.
 */
static int32_t App_captFreeFrames(appCaptObj *appObj);

/**
 * \brief   App print function for FVID2 driver.
 *
 * \param   str             Print string
 *
 * \retval  None.
 */
void App_fvidPrint(const char *str, ...);

/**
 * \brief   App print function for UDMA driver.
 *
 * \param   str             Print string
 *
 * \retval  None.
 */
static void App_dmaPrint(const char *str);

/**
 * \brief   App function to configure remote sensors.
 *
 * \param   appObj        CSI RX Capture Test Object
 *
 * \retval  Sensor configuration status.
 */
static int32_t App_sensorConfig(appCaptObj* appObj);

/**
 * \brief   App function to configure remote sensors.
 *
 * \param   appObj        CSI RX Capture Test Object
 *
 * \retval  I2C instance setup status.
 */
static int32_t App_setupI2CInst(appCaptObj* appObj);

/**
 * \brief   App function to get current time in msec.
 *
 * \param   None.
 *
 * \retval  I2C instance setup status.
 */
uint32_t App_getCurTimeInMsec(void);

/**
 * \brief   App function to calculate the elapsed time from 'startTime' in msec.
 *
 * \param   None.
 *
 * \retval  I2C instance setup status.
 */
uint32_t App_getElapsedTimeInMsec(uint32_t startTime);

extern void App_wait(uint32_t wait_in_ms);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
appCaptObj gAppObj;
/* Memory buffer to hold data */
#ifdef __cplusplus
#pragma DATA_SECTION(".data_buffer")
#pragma DATA_ALIGN(128)
#else
#pragma DATA_SECTION(gFrms, ".data_buffer")
#pragma DATA_ALIGN(gFrms, 128)
#endif  /* #ifdef __cplusplus */
static uint8_t gFrmDropBuf[(APP_CAPT_FRAME_PITCH)] __attribute__(( aligned(128), section(".data_buffer")));
static uint8_t gFrms[(APP_CAPT_FRAMES_PER_CH * APP_CAPT_CH_NUM)][APP_CAPT_FRAME_SIZE] __attribute__(( aligned(128), section(".data_buffer")));

/*
 * UDMA driver objects
 */
struct Udma_DrvObj gUdmaDrvObj;

static SemaphoreP_Handle gLockSem;

I2C_Handle gI2cHandle = NULL;
uint16_t gSensorCfg[SENSOR_CFG_SIZE][3] = IMX390_LINEAR_1920X1080_CONFIG;
uint16_t gUb960SensorCfg[][3] = {
    {0x01, 0x02, 0x100},
    {0x1f, 0x05, 0x1},

    {0xB0, 0x1C,0x1},
    {0xB1, 0x16,0x1},
    {0xB2, 0x00,0x1},
    {0xB1, 0x17,0x1},
    {0xB2, 0x00,0x1},
    {0xB1, 0x18,0x1},
    {0xB2, 0x00,0x1},
    {0xB1, 0x19,0x1},
    {0xB2, 0x00,0x1},
    {0xB0, 0x1C,0x1},
    {0xB1, 0x15,0x1},
    {0xB2, 0x0A,0x1},
    {0xB2, 0x00,0x10},

    {0x0D, 0x90, 0x1}, /*I/O to 3V3 - Options not valid with datashee*/
    {0x0C, 0x0F, 0x1}, /*Enable All ports*/

    /*Select Channel 0*/
    {0x4C, 0x01, 0x1},
    {0x58, 0x5E, 0x1},
    {0x72, 0x00, 0x1}, /*VC map*/

    /*Select Channel 1*/
    {0x4C, 0x12, 0x1},
    {0x58, 0x5D, 0x1},/*Enable Back channel, set to 50Mbs*/

    /*Select Channel 2*/
    {0x4C, 0x24, 0x1},
    {0x58, 0x5D, 0x1},/*Enable Back channel, set to 50Mbs*/

    /*Select Channel 3*/
    {0x4C, 0x38, 0x1},
    {0x58, 0x5D, 0x1},/*Enable Back channel, set to 50Mbs*/

    /*Select Channel 0*/
    {0x4C, 0x01, 0x1},
    {0xB0, 0x04, 0x1},
    {0xB1, 0x03, 0x1},
    {0xB2, 0x20, 0x1},
    {0xB1, 0x13, 0x1},
    {0xB2, 0x20, 0x1},
    {0xB0, 0x04, 0x1},
    {0xB1, 0x04, 0x1},
    {0xB2, 0x3F, 0x1},
    {0xB1, 0x14, 0x1},
    {0xB2, 0x3F, 0x1},
    {0x42, 0x71, 0x1}, /*Unknown*/
    {0x41, 0xF0, 0x1}, /*Unknown*/
    {0xB9, 0x18, 0x1},

    /*Select Channel 1*/
    {0x4C, 0x12, 0x1},
    {0xB0, 0x08, 0x1},
    {0xB1, 0x03, 0x1},
    {0xB2, 0x20, 0x1},
    {0xB1, 0x13, 0x1},
    {0xB2, 0x20, 0x1},
    {0xB0, 0x08, 0x1},
    {0xB1, 0x04, 0x1},
    {0xB2, 0x3F, 0x1},
    {0xB1, 0x14, 0x1},
    {0xB2, 0x3F, 0x1},
    {0xB0, 0x08, 0x1},
    {0x42, 0x71, 0x1}, /*Unknown*/
    {0x41, 0xF0, 0x1}, /*Unknown*/
    {0xB9, 0x18, 0x1},

    /*Select Channel 2*/
    {0x4C, 0x24, 0x1},
    {0xB0, 0x0C, 0x1},
    {0xB1, 0x03, 0x1},
    {0xB2, 0x20, 0x1},
    {0xB1, 0x13, 0x1},
    {0xB2, 0x20, 0x1},
    {0xB0, 0x0C, 0x1},
    {0xB1, 0x04, 0x1},
    {0xB2, 0x3F, 0x1},
    {0xB1, 0x14, 0x1},
    {0xB2, 0x3F, 0x1},
    {0x42, 0x71, 0x1},/*Unknown*/
    {0x41, 0xF0, 0x1},/*Unknown*/
    {0xB9, 0x18, 0x1},

    /*Select Channel 3*/
    {0x4C, 0x38, 0x1},
    {0xB0, 0x10, 0x1},
    {0xB1, 0x03, 0x1},
    {0xB2, 0x20, 0x1},
    {0xB1, 0x13, 0x1},
    {0xB2, 0x20, 0x1},
    {0xB0, 0x10, 0x1},
    {0xB1, 0x04, 0x1},
    {0xB2, 0x3F, 0x1},
    {0xB1, 0x14, 0x1},
    {0xB2, 0x3F, 0x1},
    {0x42, 0x71, 0x1},/*Unknown*/
    {0x41, 0xF0, 0x1},/*Unknown*/
    {0xB9, 0x18, 0x1},

    {0x32, 0x01, 0x1}, /*Enable TX port 0*/
    {0x20, 0x00, 0x1}, /*Forwarding and using CSIport 0 */

    /*Sets GPIOS*/
    {0x10, 0x83, 0x1},
    {0x11, 0xA3, 0x1},
    {0x12, 0xC3, 0x1},
    {0x13, 0xE3, 0x1},

    {0x4C, 0x01, 0x1}, /* 0x01 */
    {0x32, 0x01, 0x1}, /*Enable TX port 0*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0xBC, 0x00, 0x1}, /*Unknown*/
    {0x5D, (UB960_SERIALISER_ADDR << 1U), 0x1}, /*Serializer I2C Address*/
    {0x65, (D3IMX390_UB960_PORT_0_SER_ADDR << 1U), 0x1},
    {0x5E, (D3IMX390_SENSOR_ADDR << 1U), 0x1}, /*Sensor I2C Address*/
    {0x66, (D3IMX390_UB960_PORT_0_SENSOR_ADDR << 1U), 0x1},
    {0x6D, 0x6C,0x1}, /*CSI Mode*/
    {0x72, 0x00,0x1}, /*VC Map - All to 0 */
    {0x7C, 0x20, 0x10}, /*Line Valid active high, Frame Valid active high*/
    {0xD5, 0xF3, 0x10}, /*Auto Attenuation*/
    {0xB0,0x1C, 0x1},
    {0xB1,0x15, 0x1},
    {0xB2,0x0A, 0x1},
    {0xB2,0x00, 0x1},

    {0x4C, 0x12, 0x1}, /* 0x12 */
    {0x32, 0x01, 0x1}, /*Enable TX port 0*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0xBC, 0x00, 0x1}, /*Unknown*/
    {0x5D, (UB960_SERIALISER_ADDR << 1U), 0x1}, /*Serializer I2C Address*/
    {0x65, (D3IMX390_UB960_PORT_1_SER_ADDR << 1U), 0x1},
    {0x5E, (D3IMX390_SENSOR_ADDR << 1U), 0x1}, /*Sensor I2C Address*/
    {0x66, (D3IMX390_UB960_PORT_1_SENSOR_ADDR << 1U), 0x1},
    {0x6D, 0x6C,0x1}, /*CSI Mode*/
    {0x72, 0x55,0x1}, /*VC Map - All to 1 */
    {0x7C, 0x20, 0x10}, /*Line Valid active high, Frame Valid active high*/
    {0xD5, 0xF3, 0x10}, /*Auto Attenuation*/
    {0xB0,0x1C, 0x1},
    {0xB1,0x15, 0x1},
    {0xB2,0x0A, 0x1},
    {0xB2,0x00, 0x1},

    {0x4C, 0x24, 0x1}, /* 0x24 */
    {0x32, 0x01, 0x1}, /*Enable TX port 0*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0xBC, 0x00, 0x1}, /*Unknown*/
    {0x5D, (UB960_SERIALISER_ADDR << 1U), 0x1}, /*Serializer I2C Address*/
    {0x65, (D3IMX390_UB960_PORT_2_SER_ADDR << 1U), 0x1},
    {0x5E, (D3IMX390_SENSOR_ADDR << 1U), 0x1}, /*Sensor I2C Address*/
    {0x66, (D3IMX390_UB960_PORT_2_SENSOR_ADDR << 1U), 0x1},
    {0x6D, 0x6C,0x1}, /*CSI Mode*/
    {0x72, 0xaa,0x1}, /*VC Map - All to 2 */
    {0x7C, 0x20, 0x10}, /*Line Valid active high, Frame Valid active high*/
    {0xD5, 0xF3, 0x10}, /*Auto Attenuation*/
    {0xB0,0x1C, 0x1},
    {0xB1,0x15, 0x1},
    {0xB2,0x0A, 0x1},
    {0xB2,0x00, 0x1},

    {0x4C, 0x38, 0x1}, /* 0x38 */
    {0x32, 0x01, 0x1}, /*Enable TX port 0*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0xBC, 0x00, 0x1}, /*Unknown*/
    {0x5D, (UB960_SERIALISER_ADDR << 1U), 0x1}, /*Serializer I2C Address*/
    {0x65, (D3IMX390_UB960_PORT_3_SER_ADDR << 1U), 0x1},
    {0x5E, (D3IMX390_SENSOR_ADDR << 1U), 0x1}, /*Sensor I2C Address*/
    {0x66, (D3IMX390_UB960_PORT_3_SENSOR_ADDR << 1U), 0x1},
    {0x6D, 0x6C,0x1}, /*CSI Mode*/
    {0x72, 0xFF,0x1}, /*VC Map - All to 3 */
    {0x7C, 0x20, 0x10}, /*Line Valid active high, Frame Valid active high*/
    {0xD5, 0xF3, 0x10}, /*Auto Attenuation*/
    {0xB0,0x1C, 0x1},
    {0xB1,0x15, 0x1},
    {0xB2,0x0A, 0x1},
    {0xB2,0x00, 0x100},
};

uint16_t gUb953SensorCfg[][3] = {
    {0x01, 0x01, 0x80},
    {0x02, 0x72, 0x10},

#if (FUSION_BOARD_VER == 0)
    {0x06, 0x21, 0x1F},
#elif (FUSION_BOARD_VER == 1)
    {0x06, 0x41, 0x1F},
#else
/* Unsupported version */
#endif

#if (D3IMX390_MODULE_VER == 0)
    {0x07, 0x28, 0x1F},
    {0x0D, 0x01, 0x10},
#elif (D3IMX390_MODULE_VER == 1)
    {0x07, 0x15, 0x80},
    {0x0D, 0x03, 0x10},
#else
/* Unsupported version */
#endif
    {0x0E, 0xF0, 0x10},
    {0xB0, 0x04, 0x10},
    {0xB1, 0x08, 0x10},
    {0xB2, 0x07, 0x80},
};

uint32_t gUb953I2CAddr[APP_CAPT_CH_MAX] =
{
    D3IMX390_UB960_PORT_0_SER_ADDR,
    D3IMX390_UB960_PORT_1_SER_ADDR,
    D3IMX390_UB960_PORT_2_SER_ADDR,
    D3IMX390_UB960_PORT_3_SER_ADDR
};

uint32_t gSensorI2CAddr[APP_CAPT_CH_MAX] =
{
    D3IMX390_UB960_PORT_0_SENSOR_ADDR,
    D3IMX390_UB960_PORT_1_SENSOR_ADDR,
    D3IMX390_UB960_PORT_2_SENSOR_ADDR,
    D3IMX390_UB960_PORT_3_SENSOR_ADDR
};
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/*
 * Application main
 */
int Csirx_captureTest(void)
{
    int retVal = FVID2_SOK;
    appCaptObj *appObj;


    appObj = &gAppObj;
    memset(appObj, 0x0, sizeof (appCaptObj));

    /* Initialize application object for current capture context */\
    App_initCaptParams(appObj);
    /* App Init */
    retVal += App_init(appObj);
    GT_0trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Sample Application - STARTS !!!\r\n");
    GT_0trace(CsirxAppTrace, GT_INFO,
              APP_NAME ":===================Setup Details===================\r\n");
    if (appObj->instId == CSIRX_INSTANCE_ID_0)
    {
        GT_0trace(CsirxAppTrace, GT_INFO, APP_NAME ": Capture Instance: 0\r\n");
    }
    else if (appObj->instId == CSIRX_INSTANCE_ID_1)
    {
        GT_0trace(CsirxAppTrace, GT_INFO, APP_NAME ": Capture Instance: 1\r\n");
    }
    else
    {
        GT_0trace(CsirxAppTrace, GT_ERR,
                  APP_NAME ": Invalid Capture Instance\r\n");
    }
    GT_1trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Capture DF:0x%x\r\n", APP_CAPT_IMAGE_DT);
    GT_2trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Capture Resolution:%d x %d\r\n",
              APP_CAPT_FRAME_WIDTH,
              APP_CAPT_FRAME_HEIGHT);
    GT_0trace(CsirxAppTrace, GT_INFO,
              APP_NAME ":===================================================\r\n");
    if (FVID2_SOK != retVal)
    {
        GT_0trace(CsirxAppTrace, GT_ERR,
              APP_NAME ": [ERROR]App_init() FAILED!!!\r\n");
    }

    /* App Init */
    if (FVID2_SOK == retVal)
    {
        retVal += App_create(appObj);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(CsirxAppTrace, GT_ERR,
              APP_NAME ": [ERROR]App_create() FAILED!!!\r\n");
        }
    }

    /* App CSI test function */
    if (FVID2_SOK == retVal)
    {
        retVal += App_csiTest(appObj);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(CsirxAppTrace, GT_ERR,
              APP_NAME ": [ERROR]App_csiTest() FAILED!!!\r\n");
        }
    }

    /* App CSI delete function */
    if (FVID2_SOK == retVal)
    {
        retVal += App_delete(appObj);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(CsirxAppTrace, GT_ERR,
              APP_NAME ": [ERROR]App_delete() FAILED!!!\r\n");
        }
    }

    /* App CSI De-initialization function */
    if (FVID2_SOK == retVal)
    {
        retVal += App_deinit(appObj);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(CsirxAppTrace, GT_ERR,
              APP_NAME ": [ERROR]App_deinit() FAILED!!!\r\n");
        }
    }

    /* using 'App_consolePrintf' instead of GT trace as Fvid2_deInit has happend */
    App_consolePrintf("Sample Application - DONE !!!\r\n");

    return (retVal);
}

static void App_initCaptParams(appCaptObj* appObj)
{
    uint32_t loopCnt = 0U;

    /* set instance to be used for capture */
    appObj->instId = APP_CAPT_INST;
    if (appObj->instId == CSIRX_INSTANCE_ID_0)
    {
        appObj->boardCsiInstID = BOARD_CSI_INST_0;
    }
    else if (appObj->instId == CSIRX_INSTANCE_ID_1)
    {
        appObj->boardCsiInstID = BOARD_CSI_INST_1;
    }
    else
    {
        GT_0trace(CsirxAppTrace, GT_ERR,
                  APP_NAME ": Invalid Capture Instance\r\n");
    }

    /* set instance initialization parameters */
    /* This will be updated once UDMA init is done */
    Csirx_initParamsInit(&appObj->initPrms);
    /* set instance configuration parameters */
    Csirx_createParamsInit(&appObj->createPrms);
    appObj->createPrms.numCh = APP_CAPT_CH_NUM;
    /* set channel configuration parameters */
    for (loopCnt = 0U ; loopCnt < appObj->createPrms.numCh ; loopCnt++)
    {
        appObj->chFrmCnt[loopCnt] = 0U;
        appObj->createPrms.chCfg[loopCnt].chId = loopCnt;
        appObj->createPrms.chCfg[loopCnt].chType = CSIRX_CH_TYPE_CAPT;
        appObj->createPrms.chCfg[loopCnt].vcNum = loopCnt;
        appObj->createPrms.chCfg[loopCnt].inCsiDataType = APP_CAPT_IMAGE_DT;
        appObj->createPrms.chCfg[loopCnt].outFmt.width = APP_CAPT_FRAME_WIDTH;
        appObj->createPrms.chCfg[loopCnt].outFmt.height = APP_CAPT_FRAME_HEIGHT;
        appObj->createPrms.chCfg[loopCnt].outFmt.pitch[0U] =
                                                APP_CAPT_FRAME_PITCH;
        appObj->createPrms.chCfg[loopCnt].outFmt.dataFormat =
                                                FVID2_DF_BGRX32_8888;
        appObj->createPrms.chCfg[loopCnt].outFmt.ccsFormat =
                                                APP_CAPT_IMAGE_STORAGE_FORMAT;
    }
    /* set module configuration parameters */
    appObj->createPrms.instCfg.enableCsiv2p0Support = (uint32_t)TRUE;
    appObj->createPrms.instCfg.numDataLanes = 4U;
    appObj->createPrms.instCfg.enableErrbypass = (uint32_t)FALSE;
    for (loopCnt = 0U ;
         loopCnt < appObj->createPrms.instCfg.numDataLanes ;
         loopCnt++)
    {
        appObj->createPrms.instCfg.dataLanesMap[loopCnt] = (loopCnt + 1U);
    }
    /* set frame drop buffer parameters */
    appObj->createPrms.frameDropBufLen =
                                (APP_CAPT_FRAME_WIDTH * APP_CAPT_FRAME_BPP);
    appObj->createPrms.frameDropBuf = (uint64_t)&gFrmDropBuf;
    /* This will be updated once Fvid2_create() is done */
    appObj->createStatus.retVal = FVID2_SOK;
    appObj->drvHandle = NULL;
    Fvid2CbParams_init(&appObj->cbPrms);
    appObj->cbPrms.cbFxn   = (Fvid2_CbFxn) &App_frameCompletionCb;
    appObj->cbPrms.appData = appObj;

    appObj->numFramesToCapture = APP_CAPT_FRAMES_CAPT;
    appObj->numFramesRcvd = 0U;
    appObj->frameErrorCnt = 0U;
    appObj->maxWidth = APP_CAPT_FRAME_WIDTH;
    appObj->maxHeight = APP_CAPT_FRAME_HEIGHT;

    /* Initialize capture instance status */
    Csirx_instStatusInit(&appObj->captStatus);
}

static int32_t App_init(appCaptObj* appObj)
{
    int32_t retVal = FVID2_SOK, dmaStatus = UDMA_SOK;
    uint32_t instId, loopCnt;
    Fvid2_InitPrms initPrms;
    Udma_InitPrms   udmaInitPrms;
    Udma_DrvHandle drvHandle = &gUdmaDrvObj;
    I2C_HwAttrs i2cConfig;

    appObj->initPrms.drvHandle = drvHandle;
    /* Fvid2 init */
    Fvid2InitPrms_init(&initPrms);
    initPrms.printFxn = &App_fvidPrint;
    retVal = Fvid2_init(&initPrms);
    if (retVal != FVID2_SOK)
    {
        GT_0trace(CsirxAppTrace, GT_ERR,
                  APP_NAME ": Fvid2 Init Failed!!!\r\n");
    }

    /* Do UDMA init before CSIRX Init */
    /* UDMA driver init */
    instId = UDMA_INST_ID_MAIN_0;
    UdmaInitPrms_init(instId, &udmaInitPrms);
    udmaInitPrms.printFxn = &App_dmaPrint;
    dmaStatus = Udma_init(drvHandle, &udmaInitPrms);
    if(UDMA_SOK != dmaStatus)
    {
        retVal = FVID2_EFAIL;
        GT_0trace(CsirxAppTrace, GT_ERR,
                  APP_NAME ": UDMA Init Failed!!!\r\n");
    }

    /* System init */
    retVal = Csirx_init(&appObj->initPrms);
    if (retVal != FVID2_SOK)
    {
        GT_0trace(CsirxAppTrace, GT_ERR,
                  APP_NAME ": System Init Failed!!!\r\n");
    }

    if (retVal == FVID2_SOK)
    {
        /* Initialize I2C Driver */
        for(loopCnt = 0; loopCnt < I2C_HWIP_MAX_CNT; loopCnt++)
        {
            I2C_socGetInitCfg(loopCnt, &i2cConfig);
            i2cConfig.enableIntr = false;
            I2C_socSetInitCfg(loopCnt, &i2cConfig);
        }

        /* Initializes the I2C */
        I2C_init();
    }
    return (retVal);
}

static int32_t App_create(appCaptObj* appObj)
{
    int32_t retVal = FVID2_SOK;
    SemaphoreP_Params semParams;
    Fvid2_TimeStampParams tsParams;
    Csirx_DPhyCfg dphyCfg;
    Csirx_EventPrms eventPrms;

    /* Fvid2_create() */
    appObj->drvHandle = Fvid2_create(
        CSIRX_CAPT_DRV_ID,
        appObj->instId,
        &appObj->createPrms,
        &appObj->createStatus,
        &appObj->cbPrms);

    if ((NULL == appObj->drvHandle) ||
        (appObj->createStatus.retVal != FVID2_SOK))
    {
        GT_0trace(CsirxAppTrace, GT_ERR,
                  APP_NAME ": Capture Create Failed!!!\r\n");
        retVal = appObj->createStatus.retVal;
    }
    if (retVal == FVID2_SOK)
    {
        /* Set CSIRX D-PHY configuration parameters */
        Csirx_initDPhyCfg(&dphyCfg);
        dphyCfg.inst = appObj->instId;
        retVal = Fvid2_control(appObj->drvHandle,
                                IOCTL_CSIRX_SET_DPHY_CONFIG,
                                &dphyCfg,
                                NULL);
        if(FVID2_SOK != retVal)
        {
            GT_0trace(CsirxAppTrace,
                      GT_ERR,
                      APP_NAME
                      ":Set D-PHY Configuration FAILED!!!\r\n");
        }
        else
        {
            GT_0trace(CsirxAppTrace,
                      GT_INFO,
                      APP_NAME
                      ":Set D-PHY Configuration Successful!!!\r\n");
        }
    }
    if (retVal == FVID2_SOK)
    {
        /* Register Error Events */
        Csirx_eventPrmsInit(&eventPrms);
        retVal = Fvid2_control(appObj->drvHandle,
                               IOCTL_CSIRX_REGISTER_EVENT,
                               &eventPrms,
                               NULL);
        if(FVID2_SOK != retVal)
        {
            GT_0trace(CsirxAppTrace,
                      GT_ERR,
                      APP_NAME
                      ":Error Events Registration FAILED!!!\r\n");
        }
        else
        {
            GT_0trace(CsirxAppTrace,
                      GT_INFO,
                      APP_NAME
                      ":Error Events Registration Successful!!!\r\n");
        }
    }
    if (retVal == FVID2_SOK)
    {
        /* Allocate instance semaphore */
        SemaphoreP_Params_init(&semParams);
        semParams.mode = SemaphoreP_Mode_BINARY;
        gLockSem = SemaphoreP_create(1U, &semParams);
        if (gLockSem == NULL)
        {
            GT_0trace(
                CsirxAppTrace, GT_ERR,
                "Instance semaphore create failed!!\r\n");
            retVal = FVID2_EALLOC;
        }
    }
    if (retVal == FVID2_SOK)
    {
        retVal = App_setupI2CInst(appObj);
    }

    if (retVal == FVID2_SOK)
    {
        tsParams.timeStampFxn = (Fvid2_TimeStampFxn)&TimerP_getTimeInUsecs;
        /* register time stamping function */
        retVal = Fvid2_control(appObj->drvHandle,
                               FVID2_REGISTER_TIMESTAMP_FXN,
                               &tsParams,
                               NULL);
    }
    if (retVal == FVID2_SOK)
    {
        GT_0trace(CsirxAppTrace,
                  GT_INFO,
                  APP_NAME ": CSIRX Capture created\r\n");
    }
    return (retVal);
}

static int32_t App_csiTest(appCaptObj* appObj)
{
    int32_t retVal = FVID2_SOK;
    volatile uint32_t lastFrameNo = 0U;
    uint32_t loopCnt;
    uint32_t startTime, elapsedTime, fps;
    uint64_t tempVar;

    /* Allocate and queue all available frames */
    retVal += App_allocAndQFrames(appObj);

    /* Configure sensor here */
    retVal += App_sensorConfig(appObj);

    if (retVal == FVID2_SOK)
    {
        retVal += Fvid2_start(appObj->drvHandle, NULL);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(CsirxAppTrace, GT_ERR,
                      APP_NAME ": Capture Start Failed!!!\r\n");
        }
    }

    startTime = App_getCurTimeInMsec();
    /* Wait for reception completion */
    if (appObj->numFramesToCapture == 0U)
    {
        /* indefinite capture */
        /* TODO: Press any key to stop capture */
    }
    else
    {
        while (appObj->numFramesRcvd < appObj->numFramesToCapture)
        {
            /* Pend on semaphore until last frame is received */
            SemaphoreP_pend(gLockSem, SemaphoreP_WAIT_FOREVER);
            if ((lastFrameNo == 0U) && (appObj->numFramesRcvd))
            {
                lastFrameNo = appObj->numFramesRcvd;
            }
        }
    }
    elapsedTime = App_getElapsedTimeInMsec(startTime);
    /* fps calculation and some x100 for precision */
    tempVar = ((uint64_t)(appObj->numFramesRcvd * 100000U)) / (elapsedTime);
    fps = (uint32_t)tempVar;
    retVal += Fvid2_stop(appObj->drvHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(CsirxAppTrace, GT_ERR,
                  APP_NAME ": Capture Stop Failed!!!\r\n");
        return retVal;
    }
    retVal += App_captFreeFrames(appObj);
    if (retVal != FVID2_SOK)
    {
        GT_0trace(CsirxAppTrace, GT_ERR,
                  APP_NAME ": Capture Stop Failed!!!\r\n");
    }
    Csirx_instStatusInit(&appObj->captStatus);
#if (APP_PRINT_DRV_LOGS == 1U)
    /* print debug logs if enabled */
    retVal += Fvid2_control(appObj->drvHandle,
                            IOCTL_CSIRX_PRINT_DEBUG_LOGS,
                            NULL,
                            NULL);
#endif

    retVal += Fvid2_control(appObj->drvHandle,
                            IOCTL_CSIRX_GET_INST_STATUS,
                            &appObj->captStatus,
                            NULL);
    if(FVID2_SOK != retVal)
    {
        GT_0trace(CsirxAppTrace,
                  GT_INFO,
                  APP_NAME
                  ":Get Capture Status Failed!!!\r\n");
    }
    GT_0trace(CsirxAppTrace, GT_INFO,
        "\n\r==========================================================\r\n");
    GT_0trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Capture Status:\r\n");
    GT_0trace(CsirxAppTrace, GT_INFO,
              "==========================================================\r\n");
    GT_1trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Frames Received: %d\r\n",
              appObj->numFramesRcvd);
    GT_1trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Frames Received with errors: %d\r\n",
              appObj->frameErrorCnt);
    GT_0trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Capture Application Completed!!!\r\n");
    GT_1trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": FIFO Overflow Count: %d\r\n",
              appObj->captStatus.overflowCount);
    GT_1trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Spurious UDMA interrupt count: %d\r\n",
              appObj->captStatus.spuriousUdmaIntrCount);
    GT_1trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Front FIFO Overflow Count: %d\r\n",
              appObj->captStatus.frontFIFOOvflCount);
    GT_1trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": CRC Error Count: %d\r\n",
              appObj->captStatus.crcCount);
    GT_1trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Un-corrected ECC Error Count: %d\r\n",
              appObj->captStatus.eccCount);
    GT_1trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Corrected ECC Error Count: %d\r\n",
              appObj->captStatus.correctedEccCount);
    GT_1trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Data ID Error Count: %d\r\n",
              appObj->captStatus.dataIdErrorCount);
    GT_1trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Invalid Access Error Count: %d\r\n",
              appObj->captStatus.invalidAccessCount);
    GT_1trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Invalid Short Packet Receive Error Count: %d\r\n",
              appObj->captStatus.invalidSpCount);
    for(loopCnt = 0U ; loopCnt < CSIRX_NUM_STREAM ; loopCnt++)
    {
        GT_2trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Stream%d FIFO Overflow Error Count: %d\r\n",
              loopCnt,
              appObj->captStatus.strmFIFOOvflCount[loopCnt]);
    }
    for(loopCnt = 0U ; loopCnt < APP_CAPT_CH_NUM ; loopCnt++)
    {
        GT_4trace(CsirxAppTrace, GT_INFO,
              APP_NAME ":[Channel No: %d] | Frame Queue Count: %d |"
              " Frame De-queue Count: %d | Frame Drop Count: %d |\r\n",
              loopCnt,
              appObj->captStatus.queueCount[loopCnt],
              appObj->captStatus.dequeueCount[loopCnt],
              appObj->captStatus.dropCount[loopCnt]);
    }
#if (APP_PRINT_DRV_LOGS == 1U)
    if (appObj->frameErrorCnt > 0U)
    {
        GT_0trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": Error Frames Info...\r\n");
        tempVar = appObj->frameErrorCnt;
        if (appObj->frameErrorCnt > APP_ERR_FRAME_LOG_MAX)
        {
            tempVar = APP_ERR_FRAME_LOG_MAX;
        }
        for (loopCnt = 0U ; loopCnt < tempVar ; loopCnt++)
        {
            GT_4trace(CsirxAppTrace, GT_INFO,
              APP_NAME ":[Frame No.: %d] | Channel Id: %d |"
              " Ch Error Frame Number: %d | Time-stamp(ms): %d |\r\n",
              loopCnt,
              appObj->errFrmCh[loopCnt],
              appObj->errFrmNo[loopCnt],
              appObj->errFrmTs[loopCnt]);
        }

    }
#endif
    GT_4trace(CsirxAppTrace, GT_INFO,
              APP_NAME ": %d frames captured in %d msec"
              " at the rate of %d.%2d frames/sec.\r\n",
              appObj->numFramesRcvd,
              elapsedTime,
              (fps / 100U),
              (fps % 100U));

    return (retVal);
}

static int32_t App_delete(appCaptObj* appObj)
{
    int32_t retVal = FVID2_SOK;
    static Fvid2_FrameList frmList;

    Fvid2FrameList_init(&frmList);
    /* Dequeue all the request from the driver */
    retVal = Fvid2_dequeue(
                    appObj->drvHandle,
                    &frmList,
                    0U,
                    FVID2_TIMEOUT_NONE);

    if ((FVID2_SOK != retVal) && (FVID2_ENO_MORE_BUFFERS != retVal))
    {
        GT_0trace(CsirxAppTrace, GT_ERR,
                  APP_NAME ": Capture De-queue Failed!!!\r\n");
    }
    if ((FVID2_SOK == retVal) || (FVID2_ENO_MORE_BUFFERS == retVal))
    {
        retVal = FVID2_SOK;
        /* Disable Error Events */
        retVal = Fvid2_control(appObj->drvHandle,
                               IOCTL_CSIRX_UNREGISTER_EVENT,
                               CSIRX_EVENT_GROUP_ERROR,
                               NULL);
        if(FVID2_SOK != retVal)
        {
            GT_0trace(CsirxAppTrace,
                      GT_ERR,
                      APP_NAME
                      ":Error Events un-registration FAILED!!!\r\n");
        }
    }
    if (retVal == FVID2_SOK)
    {
        retVal = Fvid2_delete(appObj->drvHandle, NULL);
    }
    if (FVID2_SOK != retVal)
    {
        GT_0trace(CsirxAppTrace, GT_ERR,
                  APP_NAME ": FVID2 Delete Failed!!!\r\n");
    }
    else
    {
        appObj->drvHandle = NULL;
    }
    /* Close I2C channel */
    I2C_close(gI2cHandle);

    if (FVID2_SOK == retVal)
    {
        /* Delete semaphore */
        SemaphoreP_delete(gLockSem);
        GT_0trace(CsirxAppTrace, GT_INFO, APP_NAME ": Capture Driver deleted\r\n");
    }

    return (retVal);
}

static int32_t App_deinit(appCaptObj* appObj)
{
    int32_t retVal = FVID2_SOK;
    Udma_DrvHandle drvHandle = &gUdmaDrvObj;

    /* TODO: sensor config de-init if needed */
    retVal = Csirx_deInit();
    /* System de-init */
    if(UDMA_SOK != Udma_deinit(drvHandle))
    {
        retVal = FVID2_EFAIL;
        GT_0trace(CsirxAppTrace,
                  GT_ERR,
                  APP_NAME ": UDMA deinit failed!!!\r\n");
    }
    Fvid2_deInit(NULL);

    return (retVal);
}

static void App_consolePrintf(const char *pcString, ...)
{
    char printBuffer[APP_PRINT_BUFFER_SIZE];
    va_list arguments;

    /* Start the var args processing. */
    va_start(arguments, pcString);
    vsnprintf (printBuffer, sizeof(printBuffer), pcString, arguments);
    printf("%s",printBuffer);
#if !defined (QT_BUILD)
    UART_printf(printBuffer);
#endif
    /* End the var args processing. */
    va_end(arguments);
}

void App_fvidPrint(const char *str, ...)
{
    App_consolePrintf(str);

    return;
}

static void App_dmaPrint(const char *str)
{
    App_consolePrintf(str);

    return;
}

static int32_t App_frameCompletionCb(Fvid2_Handle handle,
                                     Ptr appData,
                                     Ptr reserved)
{
    int32_t  retVal = FVID2_SOK;
    uint32_t frmIdx = 0U, idx = 0U;
    static Fvid2_FrameList frmList;
    Fvid2_Frame *pFrm;
    appCaptObj *appObj = (appCaptObj *) appData;
    volatile uint32_t tempVar;

    GT_assert(CsirxAppTrace, (appData != NULL));

    Fvid2FrameList_init(&frmList);
    retVal = Fvid2_dequeue(
        appObj->drvHandle,
        &frmList,
        0U,
        FVID2_TIMEOUT_NONE);
    if (FVID2_SOK == retVal)
    {
        appObj->numFramesRcvd += frmList.numFrames;
        for (frmIdx = 0; frmIdx < frmList.numFrames; frmIdx++)
        {
            pFrm = frmList.frames[frmIdx];
            appObj->chFrmCnt[pFrm->chNum]++;
            if (FVID2_FRAME_STATUS_COMPLETED != pFrm->status)
            {
                idx = (appObj->frameErrorCnt % APP_ERR_FRAME_LOG_MAX);
                appObj->errFrmCh[idx] = pFrm->chNum;
                appObj->errFrmNo[idx] = appObj->chFrmCnt[pFrm->chNum];
                appObj->errFrmTs[idx] = (uint32_t)(pFrm->timeStamp64 / 1000U);
                appObj->frameErrorCnt++;
            }
        }

        /* Queue back de-queued frames,
           last param i.e. streamId is unused in DRV */
        retVal = Fvid2_queue(appObj->drvHandle, &frmList, 0U);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(CsirxAppTrace, GT_ERR,
                      APP_NAME ": Capture Queue Failed!!!\r\n");
        }
    }

    /* always return 'FVID2_SOK' */
    /* Post semaphore here for sending next Image */
    SemaphoreP_post(gLockSem);

    return FVID2_SOK;
}

static int32_t App_allocAndQFrames(appCaptObj *appObj)
{
    int32_t retVal = FVID2_SOK;
    uint32_t chIdx = 0U, frmIdx = 0U;
    static Fvid2_FrameList frmList;
    Fvid2_Frame  *pFrm;

    /* for every channel in a capture handle,
       allocate memory for and queue frames */
    Fvid2FrameList_init(&frmList);
    frmList.numFrames = 0U;
    for (chIdx = 0U; chIdx < appObj->createPrms.numCh ; chIdx++)
    {
        for (frmIdx = 0U; frmIdx < APP_CAPT_FRAMES_PER_CH ; frmIdx++)
        {
            /* assign frames memory */
            /* Only following fields are used in CSIRX DRV */
            pFrm = (Fvid2_Frame *)
                    &appObj->frames[(chIdx * APP_CAPT_FRAMES_PER_CH) + frmIdx];
            pFrm->addr[0U] =
               (uint64_t)&gFrms[(chIdx * APP_CAPT_FRAMES_PER_CH) + frmIdx][0U];
            pFrm->chNum = appObj->createPrms.chCfg[chIdx].chId;
            pFrm->appData = appObj;
            frmList.frames[frmList.numFrames] = pFrm;
            frmList.numFrames++;
        }
    }
    /* queue the frames in frmList
     * All allocated frames are queued here as an example.
     * In general at least 2 frames per stream/channel need to queued
     * before capture can be started.
     * Failing which, frame could be dropped.
     */
    /* last parameter, i.e. streamId is unused in CSIRX DRV */
    retVal = Fvid2_queue(appObj->drvHandle, &frmList, 0U);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(CsirxAppTrace, GT_ERR,
                  APP_NAME ": Capture Queue Failed!!!\r\n");
    }

    return retVal;
}

static int32_t App_captFreeFrames(appCaptObj *appObj)
{
    int32_t retVal = FVID2_SOK;
    static Fvid2_FrameList frmList;

    /* for every stream and channel in a capture handle */
    Fvid2FrameList_init(&frmList);

    /* Deq-queue any frames queued more than needed */
    retVal = Fvid2_dequeue(
                    appObj->drvHandle,
                    &frmList,
                    0U,
                    FVID2_TIMEOUT_NONE);
    if (retVal == FVID2_ENO_MORE_BUFFERS)
    {
        /* All buffer might be de-queued during stop,
           in this case no error shall be returned */
        retVal = FVID2_SOK;
    }
    /* TODO: Free up frame allocated memories here */
    /* it is global variable here so not needed */

    return (retVal);
}

static int32_t App_sensorConfig(appCaptObj* appObj)
{
    int32_t retVal = FVID2_SOK;
    int32_t status;
    uint32_t cnt, sensorIdx;
    uint8_t i2cInst = 0U, i2cAddr = 0U, regAddr8, regVal;
    uint16_t regAddr;
    Board_STATUS ret = BOARD_SOK;

    Board_fpdU960GetI2CAddr(&i2cInst, &i2cAddr, appObj->boardCsiInstID);
    if ((i2cInst == 0U) && (i2cAddr == 0U))
    {
        retVal = FVID2_EFAIL;
    }
    else
    {
        for (cnt = 0;
             cnt < sizeof(gUb960SensorCfg)/(sizeof(gUb960SensorCfg[0]));
             cnt ++)
        {
            regAddr8 = gUb960SensorCfg[cnt][0] & 0xFF;
            regVal = gUb960SensorCfg[cnt][1] & 0xFF;
            status = Board_i2c8BitRegWr(gI2cHandle, i2cAddr, regAddr8, &regVal, 1,
                                        APP_I2C_TRANSACTION_TIMEOUT);
            if (0 != status)
            {
                GT_2trace(CsirxAppTrace,
                          GT_INFO,
                          APP_NAME ": Failed to Set UB960 register %x: Value:%x\n",
                          gUb960SensorCfg[cnt][0],
                          gUb960SensorCfg[cnt][1]);
                break;
            }
            else
            {
                App_wait(gUb960SensorCfg[cnt][2]);
            }
        }

        for (sensorIdx = 0U ; sensorIdx < APP_CAPT_CH_NUM ; sensorIdx++)
        {
            if (0 == status)
            {
                /* UB960 Port configuration */
                i2cAddr = gUb953I2CAddr[sensorIdx];
                for (cnt = 0;
                     cnt < sizeof(gUb953SensorCfg)/(sizeof(gUb953SensorCfg[0]));
                     cnt ++)
                {
                    regAddr8 = gUb953SensorCfg[cnt][0] & 0xFF;
                    regVal = gUb953SensorCfg[cnt][1] & 0xFF;
                    status = Board_i2c8BitRegWr(gI2cHandle,
                                                i2cAddr,
                                                regAddr8,
                                                &regVal,
                                                1,
                                                APP_I2C_TRANSACTION_TIMEOUT);
                    if (0 != status)
                    {
                        GT_2trace(CsirxAppTrace, GT_INFO,
                                  APP_NAME
                                  ": Failed to Set UB953 register %x: Value:%x\n",
                                  gUb953SensorCfg[cnt][0],
                                  gUb953SensorCfg[cnt][1]);
                        break;
                    }
                    else
                    {
                        App_wait(gUb953SensorCfg[cnt][2]);
                    }
                }
            }
            else
            {
                break;
            }
        }

        if (0 == status)
        {
            GT_0trace(CsirxAppTrace, GT_INFO,
                      APP_NAME ": Configuring IMX390 Sensor\r\n");
        }

        for (sensorIdx = 0U ; sensorIdx < APP_CAPT_CH_NUM ; sensorIdx++)
        {
            if (0 == status)
            {
                /* Sensor 0 configuration */
                i2cAddr = gSensorI2CAddr[sensorIdx];
                for (cnt = 0; cnt < SENSOR_CFG_SIZE; cnt ++)
                {
                    regAddr = gSensorCfg[cnt][0];
                    regVal = gSensorCfg[cnt][1];

                    status = Board_i2c16BitRegWr(gI2cHandle,
                                                 i2cAddr,
                                                 regAddr,
                                                 &regVal,
                                                 1,
                                                 BOARD_I2C_REG_ADDR_MSB_FIRST,
                                                 APP_I2C_TRANSACTION_TIMEOUT);
                    if (0 != status)
                    {
                        GT_2trace(CsirxAppTrace, GT_INFO,
                                  APP_NAME
                                  ": Failed to Set Sensor register %x: Value:0x%x\n",
                                  regAddr,
                                  regVal);
                        break;
                    }
                }
            }
            else
            {
                break;
            }
        }

        if (0 == status)
        {
            Board_fpdU960GetI2CAddr(&i2cInst, &i2cAddr, appObj->boardCsiInstID);
            if (appObj->instId == CSIRX_INSTANCE_ID_1)
            {
                i2cAddr = 0x36U;
            }
            regAddr8 = 0x33;
            regVal = 0x3;
            status = Board_i2c8BitRegWr(gI2cHandle,
                                        i2cAddr,
                                        regAddr8,
                                        &regVal,
                                        1,
                                        APP_I2C_TRANSACTION_TIMEOUT);
            if (0 != status)
            {
                GT_0trace(CsirxAppTrace, GT_INFO,
                          APP_NAME ": Failed to enable CSI port\n");
            }

            if (ret != BOARD_SOK)
            {
                GT_0trace(CsirxAppTrace, GT_INFO,
                          APP_NAME ": ERROR in Sensor Configuration!!!\r\n");
            }
            else
            {
                GT_0trace(CsirxAppTrace, GT_INFO,
                          APP_NAME ": Sensor Configuration done!!!\r\n");
            }
        }
        else
        {
            GT_0trace(CsirxAppTrace, GT_INFO,
                      APP_NAME ": Sensor Configuration Failed!!!\r\n");
        }
    }

    return (retVal);
}

static int32_t App_setupI2CInst(appCaptObj* appObj)
{
    int32_t retVal = FVID2_SOK;
    uint8_t i2cInst = 0U, i2cAddr = 0U;
    I2C_Params i2cParams;

    /* Initializes the I2C Parameters */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz; /* 400KHz */

    Board_fpdU960GetI2CAddr(&i2cInst, &i2cAddr, appObj->boardCsiInstID);
    if ((i2cInst == 0U) && (i2cAddr == 0U))
    {
        retVal = FVID2_EFAIL;
    }
    else
    {
        if ((i2cInst == 0U) && (i2cAddr == 0U))
        {
            retVal = FVID2_EFAIL;
        }
        else
        {
            /* Configures the I2C instance with the passed parameters*/
            gI2cHandle = I2C_open(i2cInst, &i2cParams);
            if(gI2cHandle == NULL)
            {
                GT_0trace(CsirxAppTrace,
                          GT_INFO,
                          APP_NAME "\nI2C Open failed!\n");
                retVal = FVID2_EFAIL;
            }
        }
    }

    return retVal;
}

uint32_t App_getCurTimeInMsec(void)
{
    uint64_t curTimeMsec, curTimeUsec;

    curTimeUsec = TimerP_getTimeInUsecs();
    curTimeMsec = (curTimeUsec / 1000U);

    return ((uint32_t) curTimeMsec);
}

uint32_t App_getElapsedTimeInMsec(uint32_t startTime)
{
    uint32_t     elapsedTimeMsec = 0U, currTime;

    currTime = App_getCurTimeInMsec();
    if (currTime < startTime)
    {
        /* Counter overflow occured */
        elapsedTimeMsec = (0xFFFFFFFFU - startTime) + currTime + 1U;
    }
    else
    {
        elapsedTimeMsec = currTime - startTime;
    }

    return (elapsedTimeMsec);
}
