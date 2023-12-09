/*
 *  Copyright (c) Texas Instruments Incorporated 2020
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

/*!
 * \file  main_tirtos.c
 *
 * \brief Main file for TI-RTOS build
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

/* BIOS Header files */
#include <ti/sysbios/utils/Load.h>

#include <ti/drv/sciclient/sciclient.h>

/* OSAL Header files */
#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>
#include <ti/osal/SemaphoreP.h>
#include <ti/osal/ClockP.h>

#include <ti/drv/enet/enet.h>

#include <ti/drv/enet/examples/utils/include/enet_apputils.h>
#include <ti/drv/enet/examples/utils/include/enet_apputils_rtos.h>
#include <ti/drv/enet/examples/utils/include/enet_appmemutils.h>
#include <ti/drv/enet/examples/utils/include/enet_appmemutils_cfg.h>
#if defined(SOC_J721E) || defined(SOC_J7200)
#include <ti/drv/enet/examples/utils/include/enet_board_j7xevm.h>
#elif defined(SOC_AM65XX)
#include <ti/drv/enet/examples/utils/include/enet_board_am65xevm.h>
#elif defined(SOC_TPR12)
#include <ti/drv/enet/examples/utils/include/enet_board_tpr12evm.h>
#elif defined(SOC_AWR294X)
#include <ti/drv/enet/examples/utils/include/enet_board_awr294xevm.h>
#endif

#if defined(ENET_ENABLE_ICSSG)
#include <ti/drv/enet/include/per/icssg.h>
#endif

#include <ti/drv/enet/examples/utils/include/enet_appboardutils.h>
#include <ti/drv/enet/examples/utils/include/enet_mcm.h>
#include <ti/drv/enet/examples/utils/include/enet_appsoc.h>
#include <ti/drv/enet/examples/utils/include/enet_apprm.h>

#include <ti/drv/enet/include/core/enet_per.h>
#include <ti/drv/enet/include/core/enet_soc.h>
#include <ti/drv/enet/include/per/cpsw.h>
#include <ti/drv/enet/include/mod/cpsw_ale.h>
#include <ti/drv/enet/include/mod/cpsw_macport.h>

#include <ti/drv/enet/nimuenet/nimu_ndk.h>
#include <ti/drv/enet/nimuenet/ndk2enet_appif.h>

/* NDK headers */
#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/inc/stkmain.h>
#include <ti/ndk/inc/socket.h>
#include <ti/ndk/inc/_stack.h>
#include <ti/ndk/inc/tools/servers.h>
#include <ti/ndk/inc/tools/console.h>

#if defined(ENET_ENABLE_TIMESYNC)
/* Timesync header files */
#include <ti/transport/timeSync/v2/include/timeSync.h>
#include <ti/transport/timeSync/v2/protocol/ptp/include/timeSync_ptp.h>
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* NDK restart test count */
#define ENETNIMUAPP_NUM_RESTART         (5U)
#define ENETNIMUAPP_RESTART_PERIOD      (1000U * 10U)

/* Test application stack size */
#define ENETNIMUAPP_TSK_STACK_MAIN      (5U * 1024U)

/* CPU load task stack size */
#define ENETNIMUAPP_TSK_STACK_CPU_LOAD  (2U * 1024U)

#define ENETNIMUAPP_PACKET_POLL_PERIOD_US (500U)

#define APP_ENABLE_STATIC_CFG             (0U)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
    /* ENET instance type */
    Enet_Type enetType;

    /* ENET instance id */
    uint32_t instId;

    /* MAC port number */
    Enet_MacPort macPort;

    /* MII interface type */
    EnetMacPort_Interface mii;

    /* Id of the board where PHY is located */
    uint32_t boardId;

    bool useDfltRxFlow;
} EnetNimu_AppCfg;

typedef struct
{
    /* Host MAC address */
    uint8_t macAddr[ENET_MAC_ADDR_LEN];

    EnetMcm_CmdIf hMcmCmdIf[ENET_TYPE_NUM];

#if !(defined(SOC_TPR12) || defined(SOC_AWR294X))
    Udma_DrvHandle hUdmaDrv;
#endif

    bool useTimeSyncPtp;

#if defined(ENET_ENABLE_TIMESYNC)
    /* Handle to PTP stack */
    TimeSyncPtp_Handle hTimeSyncPtp;
#endif
} EnetNimu_AppObj;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

#if (0U == APP_ENABLE_STATIC_CFG)
static void EnetApp_getTestConfig(Enet_Type *enetType,
                                  uint32_t *instId,
                                  Enet_MacPort *macPort,
                                  EnetMacPort_Interface *mii,
                                  uint32_t *boardId);
#endif

/* Task functions */
static void    EnetNimuApp_ndkRestartTask(void *arg0,
                                          void *arg1);

static void    EnetNimuApp_timerCallback(void *arg);

static int32_t EnetNimuApp_init(Enet_Type enetType);

void           EnetNimuApp_deInit(void);

static void EnetNimuApp_cpuLoadTask(void *a0,
                                    void *a1);

#if defined(ENET_ENABLE_TIMESYNC)
void EnetNimuApp_initTimeSyncPtp(Enet_Type enetType,
                                 Enet_MacPort macPort,
                                 const uint8_t *hostMacAddr,
                                 uint32_t ipAddr);

void EnetNimuApp_deinitTimeSyncPtp(void);
#endif

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

extern void ti_ndk_config_Global_stackThread(uintptr_t, uintptr_t);

/*!
 * \brief
 *   NIMUDeviceTable
 *
 * \details
 *  This is the NIMU Device Table for the Platform.
 *  This should be defined for each platform. Since the current platform
 *  has a single network Interface; this has been defined here. If the
 *  platform supports more than one network interface this should be
 *  defined to have a list of "initialization" functions for each of the
 *  interfaces.
 */
NIMU_DEVICE_TABLE_ENTRY NIMUDeviceTable[2U] =
{
    /*!
     * \brief  NIMU_NDK_Init for this network device
     */
    {&NIMU_NDK_init},
    {NULL}
};

static void *hEcho    = 0;
static void *hEchoUdp = 0;
static void *hData    = 0;
static void *hNull    = 0;
static void *hOob     = 0;

#ifndef __cplusplus
/* App string used by Telnet server */
char *VerStr = "NIMU ENET Example";
#endif

static uint8_t gEnetNimuAppTskStackMain[ENETNIMUAPP_TSK_STACK_MAIN]
__attribute__ ((aligned(32)));
static uint8_t gEnetLoadTskStack[ENETNIMUAPP_TSK_STACK_CPU_LOAD]
__attribute__ ((aligned(32)));

/* Semaphore to sync the NSP/NDK restart activity */
static SemaphoreP_Handle gNdkRestartSem = NULL;

/* gNdkRestartCnt needs to be initialized to 1 to avoid compilation error
 * "error: pointless comparison of unsigned integer with zero" when ENETNIMUAPP_NUM_RESTART is zero */
volatile uint32_t gNdkRestartCnt = 1U;

static EnetNimu_AppCfg gEnetNimuAppCfg =
{
    .useDfltRxFlow = true,
};

static EnetNimu_AppObj gEnetNimuAppObj =
{
    .hMcmCmdIf =
    {
        [ENET_CPSW_2G] = {.hMboxCmd = NULL, .hMboxResponse = NULL},
        [ENET_CPSW_9G] = {.hMboxCmd = NULL, .hMboxResponse = NULL},
    },
    .useTimeSyncPtp = false,
};

static TaskP_Handle hNdkRestartTask;
static TaskP_Handle hCpuLoadTask;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    TaskP_Params params;
    SemaphoreP_Params semParams;
    ClockP_Params clkParams;

    EnetBoard_init();

    EnetAppUtils_print("==========================\n");
    EnetAppUtils_print("      ENET NIMU App       \n");
    EnetAppUtils_print("==========================\n");
#if (1U == APP_ENABLE_STATIC_CFG)
#if defined(BUILD_MCU1_0)
    gEnetNimuAppCfg.enetType = ENET_ICSSG_DUALMAC;
    gEnetNimuAppCfg.instId   = 0U;
    gEnetNimuAppCfg.macPort  = ENET_MAC_PORT_1; /* RGMII port */
#if defined(SOC_AM65XX)
    gEnetNimuAppCfg.boardId  = ENETBOARD_AM65XX_EVM;
#else
    gEnetNimuAppCfg.boardId  = ENETBOARD_GESI_ID;
#endif
    gEnetNimuAppCfg.mii.layerType    = ENET_MAC_LAYER_GMII;
    gEnetNimuAppCfg.mii.sublayerType = ENET_MAC_SUBLAYER_REDUCED;
#elif (defined(BUILD_MCU2_1) || defined(BUILD_MPU1_0))
    gEnetNimuAppCfg.enetType = ENET_CPSW_2G;
    gEnetNimuAppCfg.instId   = 0U;
    gEnetNimuAppCfg.macPort  = ENET_MAC_PORT_1; /* RGMII port */
    gEnetNimuAppCfg.boardId  = ENETBOARD_CPB_ID;
    gEnetNimuAppCfg.mii.layerType    = ENET_MAC_LAYER_GMII;
    gEnetNimuAppCfg.mii.sublayerType = ENET_MAC_SUBLAYER_REDUCED;
#else
#if defined(SOC_J721E)
    gEnetNimuAppCfg.enetType = ENET_CPSW_9G;
    gEnetNimuAppCfg.instId   = 0U;
    gEnetNimuAppCfg.macPort  = ENET_MAC_PORT_3; /* RGMII port */
    gEnetNimuAppCfg.boardId  = ENETBOARD_GESI_ID;
    gEnetNimuAppCfg.mii.layerType    = ENET_MAC_LAYER_GMII;
    gEnetNimuAppCfg.mii.sublayerType = ENET_MAC_SUBLAYER_REDUCED;
#elif defined(SOC_J7200)
    gEnetNimuAppCfg.enetType = ENET_CPSW_5G;
    gEnetNimuAppCfg.instId   = 0U;
    gEnetNimuAppCfg.macPort  = ENET_MAC_PORT_1; /* Q/SGMII port */
    gEnetNimuAppCfg.boardId  = ENETBOARD_QPENET_ID;
    gEnetNimuAppCfg.mii.layerType    = ENET_MAC_LAYER_GMII;
    gEnetNimuAppCfg.mii.sublayerType = ENET_MAC_SUBLAYER_QUAD_SERIAL_MAIN;
#endif
#endif
    gEnetNimuAppObj.useTimeSyncPtp = false;
#else
    EnetApp_getTestConfig(&gEnetNimuAppCfg.enetType,
                          &gEnetNimuAppCfg.instId,
                          &gEnetNimuAppCfg.macPort,
                          &gEnetNimuAppCfg.mii,
                          &gEnetNimuAppCfg.boardId);
#endif

    EnetAppUtils_enableClocks(gEnetNimuAppCfg.enetType, gEnetNimuAppCfg.instId);

    TaskP_Params_init(&params);
    params.name           = (uint8_t *)"CPU_LOAD";
    params.priority       = 15U;
    params.stack          = gEnetLoadTskStack;
    params.stacksize      = sizeof(gEnetLoadTskStack);
    hCpuLoadTask          = TaskP_create(EnetNimuApp_cpuLoadTask, &params);
    if (hCpuLoadTask == NULL)
    {
        OS_stop();
    }

    /* ETHFW-1767 - TimeSync PTP doesn't support restart, hence skip NIMU app restart
     * if TimeSync has been selected by user or via static config */
    if ((ENETNIMUAPP_NUM_RESTART > 0U) &&
        !gEnetNimuAppObj.useTimeSyncPtp)
    {
        SemaphoreP_Params_init(&semParams);
        semParams.mode = SemaphoreP_Mode_BINARY;
        gNdkRestartSem = SemaphoreP_create(0, &semParams);
        EnetAppUtils_assert(NULL != gNdkRestartSem);
        /* Create NDK restart task.  */
        TaskP_Params_init(&params);

        /* Priority of restart NDK task should be lower that NDK stack thread,
         * otherwise NDK shutdown might not complete correctly as restart would kick
         * start before that */
        params.priority       = 2U;
        params.stack          = gEnetNimuAppTskStackMain;
        params.stacksize      = sizeof(gEnetNimuAppTskStackMain);
        params.name           = (uint8_t *)"NDK_RestartTask";
        hNdkRestartTask       = TaskP_create(EnetNimuApp_ndkRestartTask, &params);
        if (hNdkRestartTask == NULL)
        {
            OS_stop();
        }

        ClockP_Handle hTimer;
        ClockP_Params_init(&clkParams);
        clkParams.startMode = ClockP_StartMode_USER;
        clkParams.period    = ENETNIMUAPP_RESTART_PERIOD;
        clkParams.runMode   = ClockP_RunMode_CONTINUOUS;
        clkParams.arg       = (void *)NULL;

        /* Creating timer and setting timer callback function*/
        hTimer = ClockP_create(EnetNimuApp_timerCallback,
                               &clkParams);
        if (hTimer == NULL)
        {
            OS_stop();
        }
    }

    OS_start();    /* does not return */

    return(0);
}

#if (0U == APP_ENABLE_STATIC_CFG)
#if defined(BUILD_MCU1_0) || defined(BUILD_MCU2_1) || defined(BUILD_MPU1_0)
static void EnetApp_getTestMacPort(Enet_MacPort *macPort,
                                   EnetMacPort_Interface *mii,
                                   uint32_t *boardId)
{
    uint32_t choice = 0U;
    bool retry;
    static const char *enetMacPortSetting =
    {
        "0: ENET_MAC_PORT_1 - RGMII\n"
    };

    do
    {
        EnetAppUtils_print("Select ENET MAC Port\n");
        EnetAppUtils_print("%s\n", enetMacPortSetting);
        retry = false;
        choice = EnetAppUtils_getNum();
        switch (choice)
        {
            case 0:
                *macPort = ENET_MAC_PORT_1;
                *boardId = ENETBOARD_CPB_ID;
                mii->layerType    = ENET_MAC_LAYER_GMII;
                mii->sublayerType = ENET_MAC_SUBLAYER_REDUCED;
                break;

            default:
                EnetAppUtils_print("Wrong ENET MAC Port, enter again\n");
                retry = true;
                break;
        }
    }
    while (retry);
}
#endif

#if defined(SOC_J7200) && defined(BUILD_MCU2_0)
static void EnetApp_getTestMacPort(Enet_MacPort *macPort,
                                   EnetMacPort_Interface *mii,
                                   uint32_t *boardId)
{
    uint32_t choice = 0U;
    int32_t enetCard;
    bool retry;
    static const char *enetMacPortSetting =
    {
        "0: ENET_MAC_PORT_1 - Q/SGMII (based on which board is attached)\n" \
        "1: ENET_MAC_PORT_2 - RGMII\n"
    };

    do
    {
        EnetAppUtils_print("Select ENET MAC Port\n");
        EnetAppUtils_print("%s\n", enetMacPortSetting);
        retry = false;
        choice = EnetAppUtils_getNum();
        switch (choice)
        {
            case 0:
                *macPort = ENET_MAC_PORT_1;
                enetCard = Board_detectEnetCard();
                if (enetCard == BOARD_ENET_QSGMII)
                {
                    *boardId = ENETBOARD_QPENET_ID;
                    mii->layerType    = ENET_MAC_LAYER_GMII;
                    mii->sublayerType = ENET_MAC_SUBLAYER_QUAD_SERIAL_MAIN;
                }
                else if (enetCard == BOARD_ENET_SGMII)
                {
                    *boardId = ENETBOARD_SGMII_ID;
                    mii->layerType    = ENET_MAC_LAYER_GMII;
                    mii->sublayerType = ENET_MAC_SUBLAYER_SERIAL;
                }
                else
                {
                    EnetAppUtils_print("SGMII/QSGMII DB not detected, enter again\n");
                    retry = true;
                }
                break;
            case 2:
                *macPort = ENET_MAC_PORT_2;
                *boardId = ENETBOARD_GESI_ID;
                mii->layerType    = ENET_MAC_LAYER_GMII;
                mii->sublayerType = ENET_MAC_SUBLAYER_REDUCED;
                break;
            default:
                EnetAppUtils_print("Wrong ENET MAC Port, enter again\n");
                retry = true;
                break;
        }
    }
    while (retry);
}
#endif

#if defined(SOC_J721E) && defined(BUILD_MCU2_0)
static void EnetApp_getTestMacPort(Enet_MacPort *macPort,
                                   EnetMacPort_Interface *mii,
                                   uint32_t *boardId)
{
    uint32_t choice = 0U;
    int32_t enetCard;
    bool retry;
    static const char *enetMacPortSetting =
    {
        "0: ENET_MAC_PORT_1 - RGMII\n" \
        "1: ENET_MAC_PORT_2 - SGMII/QSGMII\n" \
        "2: ENET_MAC_PORT_3 - RGMII\n" \
        "3: ENET_MAC_PORT_4 - RGMII\n" \
        "7: ENET_MAC_PORT_8 - RMII\n"
    };

    do
    {
        EnetAppUtils_print("Select ENET MAC Port\n");
        EnetAppUtils_print("%s \n", enetMacPortSetting);
        retry = false;
        choice = EnetAppUtils_getNum();
        switch (choice)
        {
            case 0:
            case 2:
            case 3:
                *macPort = ENET_MACPORT_DENORM(choice);
                *boardId = ENETBOARD_GESI_ID;
                mii->layerType    = ENET_MAC_LAYER_GMII;
                mii->sublayerType = ENET_MAC_SUBLAYER_REDUCED;
                break;

            case 7:
                *macPort = ENET_MAC_PORT_8;
                *boardId = ENETBOARD_GESI_ID;
                mii->layerType    = ENET_MAC_LAYER_MII;
                mii->sublayerType = ENET_MAC_SUBLAYER_REDUCED;
                break;

            case 1:
                *macPort = ENET_MAC_PORT_2;
                enetCard = Board_detectEnetCard();
                if (enetCard == BOARD_ENET_QSGMII)
                {
                    *boardId = ENETBOARD_QPENET_ID;
                    mii->layerType    = ENET_MAC_LAYER_GMII;
                    mii->sublayerType = ENET_MAC_SUBLAYER_QUAD_SERIAL_MAIN;
                }
                else if (enetCard == BOARD_ENET_SGMII)
                {
                    *boardId = ENETBOARD_SGMII_ID;
                    mii->layerType    = ENET_MAC_LAYER_GMII;
                    mii->sublayerType = ENET_MAC_SUBLAYER_SERIAL;
                }
                else
                {
                    EnetAppUtils_print("SGMII/QSGMII DB not detected, enter again\n");
                    retry = true;
                }
                break;

            default:
                EnetAppUtils_print("Wrong ENET MAC Port, enter again\n");
                retry = true;
                break;
        }
    }
    while (retry);
}
#endif

static void EnetApp_getTestConfig(Enet_Type *enetType,
                                  uint32_t *instId,
                                  Enet_MacPort *macPort,
                                  EnetMacPort_Interface *mii,
                                  uint32_t *boardId)
{
    uint32_t choice = 0U;
    bool retry;
#if defined(ENET_ENABLE_ICSSG)
    bool retry1;
#endif
#if defined(ENET_ENABLE_TIMESYNC)
    bool timeSyncEnabled = true;
#endif
    static const char enetTypeSetting[] =
    {
#if defined(BUILD_MCU1_0) || defined(BUILD_MCU2_1) || defined(BUILD_MPU1_0)
        "0: ENET2G\n"
#endif
#if defined(SOC_J7200) && defined(BUILD_MCU2_0)
        "1: ENET5G\n"
#endif
#if defined(SOC_J721E) && defined(BUILD_MCU2_0)
        "2: ENET9G\n"
#endif
#if defined(ENET_ENABLE_ICSSG)
        "3: ENET ICSSG Dual MAC\n"
#endif
    };

#if defined(ENET_ENABLE_ICSSG)
    static const char enetIcssgInstSetting[] =
    {
        "0: ICSSG0 Slice0(MAC port 1)\n"
        "1: ICSSG0 Slice1(MAC port 2)\n"
        "2: ICSSG1 Slice0(MAC port 1)\n"
        "3: ICSSG1 Slice1(MAC port 2)\n"
#if defined(SOC_AM65XX)
        "4: ICSSG2 Slice0(MAC port 1)\n"
        "5: ICSSG2 Slice1(MAC port 2)\n"
#endif
    };
#endif

    do
    {
        EnetAppUtils_print("Select ENET Type\n");
        EnetAppUtils_print("%s\n", enetTypeSetting);
        retry = false;
        choice = EnetAppUtils_getNum();
        switch (choice)
        {
#if defined(BUILD_MCU1_0) || defined(BUILD_MCU2_1) || defined(BUILD_MPU1_0)
            case 0:
                *enetType = ENET_CPSW_2G;
                *instId   = 0U;
                EnetApp_getTestMacPort(macPort, mii, boardId);

#if defined(SOC_AM65XX) && defined(ENET_ENABLE_TIMESYNC)
                /* TimeSync PTP v2 only supports one peripheral,
                 * ICSSG is enabled by default for AM65xx, CPSW is disabled */
                timeSyncEnabled = false;
#endif
                break;
#endif
#if defined(SOC_J7200) && defined(BUILD_MCU2_0)
            case 1:
                *enetType = ENET_CPSW_5G;
                *instId   = 0U;
                EnetApp_getTestMacPort(macPort, mii, boardId);
                break;
#endif
#if defined(SOC_J721E) && defined(BUILD_MCU2_0)
            case 2:
                *enetType = ENET_CPSW_9G;
                *instId   = 0U;
                EnetApp_getTestMacPort(macPort, mii, boardId);
                break;
#endif
#if defined(ENET_ENABLE_ICSSG)
            case 3:
                *enetType = ENET_ICSSG_DUALMAC;
                *instId = 0xFF;
                do
                {
                    EnetAppUtils_print("Select slice/port number\n");
                    EnetAppUtils_print("%s\n", enetIcssgInstSetting);
                    retry1 = false;
                    *instId = EnetAppUtils_getNum();
                    switch (*instId)
                    {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
#if defined(SOC_AM65XX)
                        case 4:
                        case 5:
#endif
                            *macPort = ENET_MAC_PORT_1;
#if defined(SOC_AM65XX)
                            *boardId = ENETBOARD_AM65XX_EVM;
#else
                            *boardId = ENETBOARD_GESI_ID;
#endif
                            mii->layerType    = ENET_MAC_LAYER_GMII;
                            mii->sublayerType = ENET_MAC_SUBLAYER_REDUCED;
                            break;

                        default:
                            EnetAppUtils_print("Wrong ICSSG instance, enter again\n");
                            retry1 = true;
                            break;
                    }
                }
                while (retry1);
                break;
#endif
           default:
                EnetAppUtils_print("Wrong ENET Type, enter again\n");
                retry = true;
                break;
        }
    }
    while (retry);

#if defined(ENET_ENABLE_TIMESYNC)
    if (timeSyncEnabled)
    {
        do
        {
            EnetAppUtils_print("Enable TimeSync PTP:\n");
            EnetAppUtils_print("0: No\n");
            EnetAppUtils_print("1: Yes\n");
            retry = false;
            choice = EnetAppUtils_getNum();
            switch (choice)
            {
                case 0:
                    gEnetNimuAppObj.useTimeSyncPtp = false;
                    break;
                case 1:
                    gEnetNimuAppObj.useTimeSyncPtp = true;
                    break;
                default:
                    EnetAppUtils_print("Invalid option, try again\n");
                    retry = true;
                    break;
            }
        }
        while (retry);
    }
#endif
}
#endif /* (0U == APP_ENABLE_STATIC_CFG) */

void EnetApp_initAleConfig(CpswAle_Cfg *aleCfg)
{
    aleCfg->modeFlags = CPSW_ALE_CFG_MODULE_EN;

    aleCfg->agingCfg.autoAgingEn     = true;
    aleCfg->agingCfg.agingPeriodInMs = 1000;

    aleCfg->nwSecCfg.vid0ModeEn = true;

    aleCfg->vlanCfg.aleVlanAwareMode           = FALSE;
    aleCfg->vlanCfg.cpswVlanAwareMode          = FALSE;
    aleCfg->vlanCfg.unknownUnregMcastFloodMask = CPSW_ALE_ALL_PORTS_MASK;
    aleCfg->vlanCfg.unknownRegMcastFloodMask   = CPSW_ALE_ALL_PORTS_MASK;
    aleCfg->vlanCfg.unknownVlanMemberListMask  = CPSW_ALE_ALL_PORTS_MASK;

    aleCfg->policerGlobalCfg.policingEn         = true;
    aleCfg->policerGlobalCfg.yellowDropEn       = false;
    aleCfg->policerGlobalCfg.redDropEn          = false;
    aleCfg->policerGlobalCfg.policerNoMatchMode = CPSW_ALE_POLICER_NOMATCH_MODE_GREEN;
}

void EnetApp_initLinkArgs(EnetPer_PortLinkCfg *linkArgs,
                          Enet_MacPort macPort)
{
    EnetPhy_Cfg *phyCfg = &linkArgs->phyCfg;
    EnetMacPort_LinkCfg *linkCfg = &linkArgs->linkCfg;
    EnetMacPort_Interface *mii = &linkArgs->mii;
    EnetBoard_EthPort ethPort;
    const EnetBoard_PhyCfg *boardPhyCfg;
    int32_t status;

    /* Setup board for requested Ethernet port */
    ethPort.enetType = gEnetNimuAppCfg.enetType;
    ethPort.instId   = gEnetNimuAppCfg.instId;
    ethPort.macPort  = gEnetNimuAppCfg.macPort;
    ethPort.boardId  = gEnetNimuAppCfg.boardId;
    ethPort.mii      = gEnetNimuAppCfg.mii;

    status = EnetBoard_setupPorts(&ethPort, 1U);
    EnetAppUtils_assert(status == ENET_SOK);

    if (Enet_isCpswFamily(gEnetNimuAppCfg.enetType))
    {
        CpswMacPort_Cfg *macCfg = (CpswMacPort_Cfg *)linkArgs->macCfg;
        CpswMacPort_initCfg(macCfg);
    }
#if defined(ENET_ENABLE_ICSSG)
    else
    {
        IcssgMacPort_Cfg *macCfg = (IcssgMacPort_Cfg *)linkArgs->macCfg;

        IcssgMacPort_initCfg(macCfg);
        macCfg->specialFramePrio = 1U;
    }
#endif

    boardPhyCfg = EnetBoard_getPhyCfg(&ethPort);
    if (boardPhyCfg != NULL)
    {
        EnetPhy_initCfg(phyCfg);
        phyCfg->phyAddr     = boardPhyCfg->phyAddr;
        phyCfg->isStrapped  = boardPhyCfg->isStrapped;
        phyCfg->loopbackEn  = false;
        phyCfg->skipExtendedCfg = boardPhyCfg->skipExtendedCfg;
        phyCfg->extendedCfgSize = boardPhyCfg->extendedCfgSize;
        memcpy(phyCfg->extendedCfg, boardPhyCfg->extendedCfg, phyCfg->extendedCfgSize);
    }
    else
    {
        EnetAppUtils_print("No PHY configuration found for MAC port %u\n",
                           ENET_MACPORT_ID(ethPort.macPort));
        EnetAppUtils_assert(false);
    }

    mii->layerType     = ethPort.mii.layerType;
    mii->sublayerType  = ethPort.mii.sublayerType;
    mii->variantType   = ENET_MAC_VARIANT_FORCED;
    linkCfg->speed     = ENET_SPEED_AUTO;
    linkCfg->duplexity = ENET_DUPLEX_AUTO;

    if (Enet_isCpswFamily(gEnetNimuAppCfg.enetType))
    {
        CpswMacPort_Cfg *macCfg = (CpswMacPort_Cfg *)linkArgs->macCfg;

        if (EnetMacPort_isSgmii(mii) || EnetMacPort_isQsgmii(mii))
        {
            macCfg->sgmiiMode = ENET_MAC_SGMIIMODE_SGMII_WITH_PHY;
        }
        else
        {
            macCfg->sgmiiMode = ENET_MAC_SGMIIMODE_INVALID;
        }
    }
}

static void EnetNimuApp_portLinkStatusChangeCb(Enet_MacPort macPort,
                                               bool isLinkUp,
                                               void *appArg)
{
    EnetAppUtils_print("MAC Port %u: link %s\n",
                       ENET_MACPORT_ID(macPort), isLinkUp ? "up" : "down");
}

static void EnetNimuApp_mdioLinkStatusChange(Cpsw_MdioLinkStateChangeInfo *info,
                                             void *appArg)
{
    static uint32_t linkUpCount = 0;
    if ((info->linkChanged) && (info->isLinked))
    {
        linkUpCount++;
    }
}

static void EnetNimuApp_initEnetLinkCbPrms(Cpsw_Cfg *cpswCfg)
{

    cpswCfg->mdioLinkStateChangeCb     = EnetNimuApp_mdioLinkStatusChange;
    cpswCfg->mdioLinkStateChangeCbArg  = &gEnetNimuAppObj;

    cpswCfg->portLinkStatusChangeCb    = &EnetNimuApp_portLinkStatusChangeCb;
    cpswCfg->portLinkStatusChangeCbArg = &gEnetNimuAppObj;
}

static int32_t EnetNimuApp_init(Enet_Type enetType)
{
    int32_t status = ENET_SOK;
    EnetMcm_InitConfig enetMcmCfg;
    Cpsw_Cfg cpswCfg;
#if defined(ENET_ENABLE_ICSSG)
    Icssg_Cfg icssgCfg;
#endif
    EnetRm_ResCfg *resCfg;
    EnetUdma_Cfg dmaCfg;
    Enet_MacPort macPortList[] = {gEnetNimuAppCfg.macPort};
    uint8_t numMacPorts        = (sizeof(macPortList) / sizeof(macPortList[0U]));
    EnetAppUtils_assert(numMacPorts <=
                        Enet_getMacPortMax(gEnetNimuAppCfg.enetType, gEnetNimuAppCfg.instId));

    /* Open UDMA */
    gEnetNimuAppObj.hUdmaDrv = EnetAppUtils_udmaOpen(gEnetNimuAppCfg.enetType, NULL);
    EnetAppUtils_assert(NULL != gEnetNimuAppObj.hUdmaDrv);
    dmaCfg.rxChInitPrms.dmaPriority = UDMA_DEFAULT_RX_CH_DMA_PRIORITY;
    dmaCfg.hUdmaDrv = gEnetNimuAppObj.hUdmaDrv;

    /* Set configuration parameters */
    if (Enet_isCpswFamily(enetType))
    {
        Enet_initCfg(gEnetNimuAppCfg.enetType, gEnetNimuAppCfg.instId, &cpswCfg, sizeof(cpswCfg));
        cpswCfg.vlanCfg.vlanAware          = false;
        cpswCfg.hostPortCfg.removeCrc      = true;
        cpswCfg.hostPortCfg.padShortPacket = true;
        cpswCfg.hostPortCfg.passCrcErrors  = true;
        EnetNimuApp_initEnetLinkCbPrms(&cpswCfg);
        resCfg = &cpswCfg.resCfg;
        EnetApp_initAleConfig(&cpswCfg.aleCfg);
        cpswCfg.dmaCfg = (void *)&dmaCfg;

        enetMcmCfg.perCfg = &cpswCfg;
    }
#if defined(ENET_ENABLE_ICSSG)
    else
    {
        Enet_initCfg(gEnetNimuAppCfg.enetType, gEnetNimuAppCfg.instId, &icssgCfg, sizeof(icssgCfg));

        /* Currently we only support one ICSSG port in NIMU */
        EnetAppUtils_assert(numMacPorts == 1U);

        resCfg = &icssgCfg.resCfg;
        icssgCfg.dmaCfg = (void *)&dmaCfg;

        enetMcmCfg.perCfg = &icssgCfg;
    }
#endif

    EnetAppUtils_assert(NULL != enetMcmCfg.perCfg);
    EnetAppUtils_initResourceConfig(gEnetNimuAppCfg.enetType, EnetSoc_getCoreId(), resCfg);

    enetMcmCfg.enetType           = gEnetNimuAppCfg.enetType;
    enetMcmCfg.instId             = gEnetNimuAppCfg.instId;
    enetMcmCfg.setPortLinkCfg     = EnetApp_initLinkArgs;
    enetMcmCfg.numMacPorts        = numMacPorts;
    enetMcmCfg.periodicTaskPeriod = ENETPHY_FSM_TICK_PERIOD_MS; /* msecs */
    enetMcmCfg.print              = EnetAppUtils_print;

    memcpy(&enetMcmCfg.macPortList[0U], &macPortList[0U], sizeof(macPortList));
    status = EnetMcm_init(&enetMcmCfg);

    return status;
}

void EnetNimuApp_stackInitHook(void *hCfg)
{
    uint32_t rc;

    /* increase stack size */
    rc = 16384;
    CfgAddEntry(hCfg, CFGTAG_OS, CFGITEM_OS_TASKSTKBOOT,
                CFG_ADDMODE_UNIQUE, sizeof(uint32_t), (uint8_t *)&rc, 0);

#if (ENET_TRACE_CFG_TRACE_LEVEL >= ENET_TRACE_CFG_LEVEL_DEBUG)
    /* We do not want to see debug messages less than WARNINGS */
    rc = DBG_INFO;
    CfgAddEntry(hCfg, CFGTAG_OS, CFGITEM_OS_DBGPRINTLEVEL,
                CFG_ADDMODE_UNIQUE, sizeof(uint32_t), (uint8_t *)&rc, 0);
#endif
}

void EnetNimuApp_stackDeleteHook(void *hCfg)
{
    // TODO enable restart via PC command

    /* As NDK shutdown is happening, we post the NDK restart task so it can
     * start NDK again. As EnetNimuApp_ndkRestartTask is at lower priority than
     * the NDK stack thread, we don't interrupt the NDK shutdown */
    SemaphoreP_post(gNdkRestartSem);
}

void EnetNimuApp_ipAddrHookFxn(uint32_t IPAddr,
                               uint32_t IfIdx,
                               uint32_t fAdd)
{
    char ipAddrStr[20];

    NtIPN2Str(IPAddr, ipAddrStr);

    if (1U == fAdd)
    {
        EnetAppUtils_print("ENET NIMU App: Added Network IP address I/F %d: %s\n",
                           IfIdx, ipAddrStr);

#if defined(ENET_ENABLE_TIMESYNC)
        if (gEnetNimuAppObj.useTimeSyncPtp)
        {
            EnetNimuApp_initTimeSyncPtp(gEnetNimuAppCfg.enetType,
                                        gEnetNimuAppCfg.macPort,
                                        &gEnetNimuAppObj.macAddr[0U],
                                        IPAddr);
        }
#endif
    }
    else
    {
        EnetAppUtils_print("ENET NIMU App: Removed Network IP address I/F %d: %s\n",
                           IfIdx, ipAddrStr);
#if defined(ENET_ENABLE_TIMESYNC)
        if (gEnetNimuAppObj.useTimeSyncPtp)
        {
            EnetNimuApp_deinitTimeSyncPtp();
        }
#endif
    }

}

void EnetNimuApp_netOpenHook(void)
{
    /* Create our local servers */
    hEcho = DaemonNew(SOCK_STREAMNC, 0, 7, dtask_tcp_echo,
                      OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3);
    hEchoUdp = DaemonNew(SOCK_DGRAM, 0, 7, dtask_udp_echo,
                         OS_TASKPRINORM, OS_TASKSTKNORM, 0, 1);
    hData = DaemonNew(SOCK_STREAM, 0, 1000, dtask_tcp_datasrv,
                      OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3);
    hNull = DaemonNew(SOCK_STREAMNC, 0, 1001, dtask_tcp_nullsrv,
                      OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3);
    hOob = DaemonNew(SOCK_STREAMNC, 0, 999, dtask_tcp_oobsrv,
                     OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3);
}

void EnetNimuApp_netCloseHook(void)
{
    DaemonFree(hOob);
    DaemonFree(hNull);
    DaemonFree(hData);
    DaemonFree(hEchoUdp);
    DaemonFree(hEcho);
}

void EnetNimuApp_timerCallback(void *arg)
{
    if (gNdkRestartCnt <= ENETNIMUAPP_NUM_RESTART)
    {
        /* Stop the network stack. */
        NC_NetStop(0);
    }
}

static void EnetNimuApp_ndkRestartTask(void *arg0,
                                       void *arg1)
{
    /* We complete restart test during init first ENETNIMUAPP_NUM_RESTART runs
     * and then keep NDK enabled for NDK apps (send/recv) test */
    while (true)
    {
        SemaphoreP_pend(gNdkRestartSem, SemaphoreP_WAIT_FOREVER);
        EnetAppUtils_printTaskStackUsage();
        EnetAppUtils_print("\nRestarting the NDK/ENET, %d!\n", gNdkRestartCnt);
        gNdkRestartCnt++;

        /* Deinit the app as it would be again initialized in get handle call from next ndk2enet_open */
        EnetNimuApp_deInit();

        /* NDK Stack will be started. */
        {
            TaskP_Handle hNetStackTask = NULL;
            TaskP_Params taskParams;

            TaskP_Params_init(&taskParams);
            taskParams.name           = (uint8_t *)"ti_ndk_config_Global_stackThread";
            taskParams.priority       = 0x5;
            taskParams.arg0           = 0u;
            taskParams.arg1           = 0u;
            taskParams.stacksize      = 0x2000;

            hNetStackTask = TaskP_create(&ti_ndk_config_Global_stackThread, &taskParams);
            if (hNetStackTask == NULL)
            {
                EnetAppUtils_print("Restarting ti_ndk_config_Global_stackThread failed\n");
                break;
            }
            else
            {
                /* Register with NDK. Only then socket open will succeed.*/
                fdOpenSession(hNetStackTask);
            }
        }
    }
}

static bool EnetApp_isPortLinked(Enet_Handle hEnet)
{
    uint32_t coreId = EnetSoc_getCoreId();

    return EnetAppUtils_isPortLinkUp(hEnet, coreId, gEnetNimuAppCfg.macPort);
}

void NimuEnetAppCb_getHandle(NimuEnetAppIf_GetHandleInArgs *inArgs,
                             NimuEnetAppIf_GetHandleOutArgs *outArgs)
{
    int32_t status;
    EnetMcm_HandleInfo handleInfo;
    EnetPer_AttachCoreOutArgs attachInfo;
    EnetUdma_OpenRxFlowPrms enetRxFlowCfg;
    EnetUdma_OpenTxChPrms enetTxChCfg;
    uint32_t coreId          = EnetSoc_getCoreId();
    bool useDfltFlow      = gEnetNimuAppCfg.useDfltRxFlow;
    EnetMcm_CmdIf *pMcmCmdIf = &gEnetNimuAppObj.hMcmCmdIf[gEnetNimuAppCfg.enetType];
    bool useRingMon          = true;

    if (pMcmCmdIf->hMboxCmd == NULL)
    {
        status = EnetNimuApp_init(gEnetNimuAppCfg.enetType);

        if (status != ENET_SOK)
        {
            EnetAppUtils_print("Failed to open ENET: %d\n", status);
        }
        EnetAppUtils_assert(status == ENET_SOK);
        EnetMcm_getCmdIf(gEnetNimuAppCfg.enetType, pMcmCmdIf);
    }
    EnetAppUtils_assert(pMcmCmdIf->hMboxCmd != NULL);
    EnetAppUtils_assert(pMcmCmdIf->hMboxResponse != NULL);
    EnetMcm_acquireHandleInfo(pMcmCmdIf, &handleInfo);
    EnetMcm_coreAttach(pMcmCmdIf, coreId, &attachInfo);

    /* Confirm HW checksum offload is enabled as NIMU enables it by default */
    if (Enet_isCpswFamily(gEnetNimuAppCfg.enetType))
    {
        Enet_IoctlPrms prms;
        bool csumOffloadFlg;
        ENET_IOCTL_SET_OUT_ARGS(&prms, &csumOffloadFlg);
        status = Enet_ioctl(handleInfo.hEnet,
                            coreId,
                            ENET_HOSTPORT_IS_CSUM_OFFLOAD_ENABLED,
                            &prms);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("() Failed to get checksum offload info: %d\n", status);
        }

        EnetAppUtils_assert(true == csumOffloadFlg);
    }

    /* Open TX channel */
    EnetDma_initTxChParams(&enetTxChCfg);

    enetTxChCfg.hUdmaDrv  = handleInfo.hUdmaDrv;
    enetTxChCfg.numTxPkts = inArgs->txCfg.numPackets;
    enetTxChCfg.cbArg     = inArgs->txCfg.cbArg;
    enetTxChCfg.notifyCb  = inArgs->txCfg.notifyCb;
    enetTxChCfg.useProxy  = true;
    EnetAppUtils_setCommonTxChPrms(&enetTxChCfg);

    EnetAppUtils_openTxCh(handleInfo.hEnet,
                          attachInfo.coreKey,
                          coreId,
                          &outArgs->txInfo.txChNum,
                          &outArgs->txInfo.hTxChannel,
                          &enetTxChCfg);
    /* Open RX Flow */
    EnetDma_initRxChParams(&enetRxFlowCfg);
    enetRxFlowCfg.notifyCb  = inArgs->rxCfg.notifyCb;
    enetRxFlowCfg.numRxPkts = inArgs->rxCfg.numPackets;
    enetRxFlowCfg.hUdmaDrv  = handleInfo.hUdmaDrv;
    enetRxFlowCfg.cbArg     = inArgs->rxCfg.cbArg;
    enetRxFlowCfg.useProxy  = true;

    /* Use ring monitor for the CQ ring of RX flow */
    EnetUdma_UdmaRingPrms *pFqRingPrms = &enetRxFlowCfg.udmaChPrms.fqRingPrms;
    pFqRingPrms->useRingMon = useRingMon;
    pFqRingPrms->ringMonCfg.mode = TISCI_MSG_VALUE_RM_MON_MODE_THRESHOLD;
    /* Ring mon low threshold */

#if defined _DEBUG_
    /* In debug mode as CPU is processing lesser packets per event, keep threshold more */
    pFqRingPrms->ringMonCfg.data0 = (inArgs->rxCfg.numPackets - 10U);
#else
    pFqRingPrms->ringMonCfg.data0 = (inArgs->rxCfg.numPackets - 20U);
#endif
    /* Ring mon high threshold - to get only low  threshold event, setting high threshold as more than ring depth*/
    pFqRingPrms->ringMonCfg.data1 = inArgs->rxCfg.numPackets;
    EnetAppUtils_setCommonRxFlowPrms(&enetRxFlowCfg);

#if defined(ENET_ENABLE_ICSSG)
    if(Enet_isIcssFamily(gEnetNimuAppCfg.enetType))
    {
        enetRxFlowCfg.flowPrms.sizeThreshEn = 0U;
    }
#endif

    EnetAppUtils_openRxFlow(gEnetNimuAppCfg.enetType,
                            handleInfo.hEnet,
                            attachInfo.coreKey,
                            coreId,
                            useDfltFlow,
                            &outArgs->rxInfo.rxFlowStartIdx,
                            &outArgs->rxInfo.rxFlowIdx,
                            &outArgs->rxInfo.macAddr[0U],
                            &outArgs->rxInfo.hRxFlow,
                            &enetRxFlowCfg);
    outArgs->coreId        = coreId;
    outArgs->coreKey       = attachInfo.coreKey;
    outArgs->hEnet         = handleInfo.hEnet;
    outArgs->hostPortRxMtu = attachInfo.rxMtu;
    ENET_UTILS_ARRAY_COPY(outArgs->txMtu, attachInfo.txMtu);
    outArgs->hUdmaDrv       = handleInfo.hUdmaDrv;
    outArgs->print          = &EnetAppUtils_print;
    outArgs->isPortLinkedFxn = &EnetApp_isPortLinked;

    outArgs->isRingMonUsed = useRingMon;
    outArgs->timerPeriodUs   = ENETNIMUAPP_PACKET_POLL_PERIOD_US;

    /* Let NIMU use optimized processing where TX packets are relinquished in next
     * TX submit call */
    outArgs->disableTxEvent = true;
    EnetAppUtils_print("Host MAC address: ");
    EnetAppUtils_printMacAddr(&outArgs->rxInfo.macAddr[0U]);

    EnetUtils_copyMacAddr(&gEnetNimuAppObj.macAddr[0U], &outArgs->rxInfo.macAddr[0U]);

#if defined(ENET_ENABLE_ICSSG)
    /* Add port MAC entry in case of ICSSG dual MAC */
    if (ENET_ICSSG_DUALMAC == gEnetNimuAppCfg.enetType)
    {
        Enet_IoctlPrms prms;
        IcssgMacPort_SetMacAddressInArgs inArgs;

        memset(&inArgs, 0, sizeof(inArgs));
        memcpy(&inArgs.macAddr[0U], &outArgs->rxInfo.macAddr[0U], sizeof(inArgs.macAddr));
        inArgs.macPort = gEnetNimuAppCfg.macPort;

        ENET_IOCTL_SET_IN_ARGS(&prms, &inArgs);
        status = Enet_ioctl(handleInfo.hEnet, coreId, ICSSG_MACPORT_IOCTL_SET_MACADDR, &prms);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("EnetAppUtils_addHostPortEntry() failed ICSSG_MACPORT_IOCTL_ADD_INTERFACE_MACADDR: %d\n",
                               status);
        }
        EnetAppUtils_assert(status == ENET_SOK);
    }
#endif
}

void NimuEnetAppCb_releaseHandle(NimuEnetAppIf_ReleaseHandleInfo *releaseInfo)
{
    EnetDma_PktQ fqPktInfoQ;
    EnetDma_PktQ cqPktInfoQ;
    bool useDfltFlow      = gEnetNimuAppCfg.useDfltRxFlow;
    EnetMcm_CmdIf *pMcmCmdIf = &gEnetNimuAppObj.hMcmCmdIf[gEnetNimuAppCfg.enetType];
    EnetAppUtils_assert(pMcmCmdIf->hMboxCmd != NULL);
    EnetAppUtils_assert(pMcmCmdIf->hMboxResponse != NULL);

    /* Close TX channel */
    {
        EnetQueue_initQ(&fqPktInfoQ);
        EnetQueue_initQ(&cqPktInfoQ);
        EnetAppUtils_closeTxCh(releaseInfo->hEnet,
                               releaseInfo->coreKey,
                               releaseInfo->coreId,
                               &fqPktInfoQ,
                               &cqPktInfoQ,
                               releaseInfo->txInfo.hTxChannel,
                               releaseInfo->txInfo.txChNum);
        releaseInfo->txFreePktCb(releaseInfo->freePktCbArg, &fqPktInfoQ, &cqPktInfoQ);
    }

    {
        /* Close RX Flow */
        EnetQueue_initQ(&fqPktInfoQ);
        EnetQueue_initQ(&cqPktInfoQ);
        EnetAppUtils_closeRxFlow(gEnetNimuAppCfg.enetType,
                                 releaseInfo->hEnet,
                                 releaseInfo->coreKey,
                                 releaseInfo->coreId,
                                 useDfltFlow,
                                 &fqPktInfoQ,
                                 &cqPktInfoQ,
                                 releaseInfo->rxInfo.rxFlowStartIdx,
                                 releaseInfo->rxInfo.rxFlowIdx,
                                 releaseInfo->rxInfo.macAddr,
                                 releaseInfo->rxInfo.hRxFlow);
        releaseInfo->rxFreePktCb(releaseInfo->freePktCbArg, &fqPktInfoQ, &cqPktInfoQ);
    }
    EnetMcm_coreDetach(pMcmCmdIf, releaseInfo->coreId, releaseInfo->coreKey);
    EnetMcm_releaseHandleInfo(pMcmCmdIf);
}

void EnetNimuApp_deInit(void)
{
    EnetAppUtils_udmaclose(gEnetNimuAppObj.hUdmaDrv);
    EnetMcm_deInit(gEnetNimuAppCfg.enetType);
    memset(&gEnetNimuAppObj, 0U, sizeof(EnetNimu_AppObj));
}

static void EnetNimuApp_cpuLoadTask(void *a0,
                                    void *a1)
{
    volatile uint32_t enableLoad = 1U;

    while (enableLoad)
    {
        EnetAppUtils_print("CPU Load: %u%%\n", Load_getCPULoad());
        TaskP_sleep(10000U);
    }
}

void EnetNimuApp_putch(char c)
{
    EnetAppUtils_print("%c", c);
}

/* PTP related functions */

#if defined(ENET_ENABLE_TIMESYNC)
void EnetNimuApp_initTimeSyncPtp(Enet_Type enetType,
                                 Enet_MacPort macPort,
                                 const uint8_t *hostMacAddr,
                                 uint32_t ipAddr)
{
    TimeSyncPtp_Config ptpCfg;
    int32_t status = ENET_SOK;

    /* Initialize and enable PTP stack */
    TimeSyncPtp_setDefaultPtpConfig(&ptpCfg);
    ptpCfg.vlanCfg.vlanType     = TIMESYNC_VLAN_TYPE_NONE;
    ptpCfg.deviceMode           = TIMESYNC_ORDINARY_CLOCK;
    ptpCfg.portMask             = ENET_MACPORT_MASK(macPort);

#if defined(SOC_AM65XX)
    ptpCfg.socConfig.socVersion = TIMESYNC_SOC_AM65xx;
#elif defined(SOC_J721E)
    ptpCfg.socConfig.socVersion = TIMESYNC_SOC_J721E;
#elif defined(SOC_J7200)
    ptpCfg.socConfig.socVersion = TIMESYNC_SOC_J7200;
#else
    EnetAppUtils_print("TimeSync PTP is not supported on this SoC\n");
    EnetAppUtils_assert(false);
#endif

    switch (enetType)
    {
        case ENET_CPSW_2G:
            ptpCfg.socConfig.ipVersion = TIMESYNC_IP_VER_CPSW_2G;
            break;

        case ENET_CPSW_5G:
            ptpCfg.socConfig.ipVersion = TIMESYNC_IP_VER_CPSW_5G;
            break;

        case ENET_CPSW_9G:
            ptpCfg.socConfig.ipVersion = TIMESYNC_IP_VER_CPSW_9G;
            break;

        case ENET_ICSSG_DUALMAC:
            ptpCfg.socConfig.ipVersion = TIMESYNC_IP_VER_ICSSG_DUALMAC;
            /* PPS is currently not enabled for ICSSG Dual-MAC */
            ptpCfg.processToDSync2PPS  = false;
            break;

        default:
            EnetAppUtils_print("TimeSync not supported on this peripheral type %u\n", enetType);
            status = ENET_ENOTSUPPORTED;
            break;
    }

    if (status == ENET_SOK)
    {
        ptpCfg.socConfig.instId = gEnetNimuAppCfg.instId;

        /* Save host port IP address and MAC address */
        memcpy(&ptpCfg.ipAddr[0U], &ipAddr, ENET_IPv4_ADDR_LEN);
        memcpy(&ptpCfg.ifMacID[0U], hostMacAddr, ENET_MAC_ADDR_LEN);

        gEnetNimuAppObj.hTimeSyncPtp = TimeSyncPtp_init(&ptpCfg);
        EnetAppUtils_assert(gEnetNimuAppObj.hTimeSyncPtp != NULL);

        TimeSyncPtp_enable(gEnetNimuAppObj.hTimeSyncPtp);
        EnetAppUtils_print("TimeSync PTP enabled\n");
    }
}

void EnetNimuApp_deinitTimeSyncPtp(void)
{
    if (gEnetNimuAppObj.hTimeSyncPtp != NULL)
    {
        TimeSyncPtp_disable(gEnetNimuAppObj.hTimeSyncPtp);

        /* ETHFW-1767 - TimeSync PTP doesn't support deinit */
        //TimeSyncPtp_deInit(gEnetNimuAppObj.hTimeSyncPtp);
    }
}
#endif
