/*
 *  Copyright (c) Texas Instruments Incorporated 2021
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
 * \file  main.c
 *
 * \brief Main file for lwIP example application.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <stdarg.h>

/* lwIP header files */
#include <lwipopts.h>
#include <lwip/api.h>
#include <lwip/netif.h>
#include <arch/sys_arch.h>
#include <ti/drv/enet/lwipif/inc/lwipif2enet_appif.h>

#if !(defined(SOC_TPR12) || defined(SOC_AWR294X))
#include <ti/drv/sciclient/sciclient.h>
#include <ti/drv/udma/udma.h>
#endif

#include <ti/drv/enet/enet.h>
#include <ti/drv/enet/examples/utils/include/enet_apputils.h>
#include <ti/drv/enet/examples/utils/include/enet_apputils_rtos.h>
#include <ti/drv/enet/examples/utils/include/enet_appmemutils.h>
#include <ti/drv/enet/examples/utils/include/enet_appboardutils.h>
#include <ti/drv/enet/examples/utils/include/enet_mcm.h>
#include <ti/drv/enet/examples/utils/include/enet_appsoc.h>
#include <ti/drv/enet/examples/utils/include/enet_apprm.h>
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

#include <ti/drv/enet/include/core/enet_per.h>
#include <ti/drv/enet/include/core/enet_soc.h>
#include <ti/drv/enet/include/per/cpsw.h>
#include <ti/drv/enet/include/mod/cpsw_ale.h>
#include <ti/drv/enet/include/mod/cpsw_macport.h>

/* OSAL Header files */
#include <ti/osal/osal.h>
#include <ti/osal/LoadP.h>

#if defined(ENET_ENABLE_TIMESYNC)
/* Timesync header files */
#include <ti/transport/timeSync/v2/include/timeSync.h>
#include <ti/transport/timeSync/v2/protocol/ptp/include/timeSync_ptp.h>
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Test application stack size */
#define ENETAPP_TSK_STACK_MAIN            (5U * 1024U)

/* CPU load task stack size */
#define ENETAPP_TSK_STACK_CPU_LOAD        (2U * 1024U)

/* How often lwIP adaptation layer will poll for RX packets (pacing timer period) */
#define ENETAPP_PACKET_POLL_PERIOD_US     (1000U)

/* Configuration via menu or statically */
#define APP_ENABLE_STATIC_CFG             (0U)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct EnetApp_Cfg_s
{
    /* Peripheral instance type */
    Enet_Type enetType;

    /* Peripheral instance id */
    uint32_t instId;

    /* MAC port number */
    Enet_MacPort macPort;

    /* MII interface type */
    EnetMacPort_Interface mii;

    /* Id of the board where PHY is located */
    uint32_t boardId;

    /* Whether or not to use RX default flow */
    bool useDfltFlow;

    /* Whether PTP stack will be used for the test or not */
    bool useTimeSyncPtp;
} EnetApp_Cfg;

typedef struct EnetApp_Obj_s
{
    /* Host MAC address */
    uint8_t macAddr[ENET_MAC_ADDR_LEN];

    /* Enet MCM handle */
    EnetMcm_CmdIf hMcmCmdIf[ENET_TYPE_NUM];

#if !(defined(SOC_TPR12) || defined(SOC_AWR294X))
    /* UDMA driver handle */
    Udma_DrvHandle hUdmaDrv;
#endif

#if defined(ENET_ENABLE_TIMESYNC)
    /* Handle to PTP stack */
    TimeSyncPtp_Handle hTimeSyncPtp;
#endif

    /* Handle to task which run stack initialization main loop */
    TaskP_Handle hMainLoopTask;

    /* Handle to task which prints CPU load periodically */
    TaskP_Handle hCpuLoadTask;
} EnetApp_Obj;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void EnetApp_getTestConfig(Enet_Type *enetType,
                                  uint32_t *instId,
                                  Enet_MacPort *macPort,
                                  EnetMacPort_Interface *mii,
                                  uint32_t *boardId);

static int32_t EnetApp_init(Enet_Type enetType);

static void EnetApp_deinit(void);

static void EnetApp_cpuLoadTask(void *a0,
                                void *a1);

#if defined(ENET_ENABLE_TIMESYNC)
void EnetApp_initTimeSyncPtp(Enet_Type enetType,
                                 Enet_MacPort macPort,
                                 const uint8_t *hostMacAddr,
                                 uint32_t ipAddr);

void EnetApp_deinitTimeSyncPtp(void);
#endif

extern void main_loop(void *a0,
                      void *a1);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static uint8_t gEnetAppMainTskStack[ENETAPP_TSK_STACK_MAIN]
__attribute__ ((aligned(32)));

static uint8_t gEnetAppLoadTskStack[ENETAPP_TSK_STACK_CPU_LOAD]
__attribute__ ((aligned(32)));

static EnetApp_Cfg gEnetAppCfg =
{
    .useDfltFlow    = true,
    .useTimeSyncPtp = false,
};

static EnetApp_Obj gEnetAppObj =
{
    .hMcmCmdIf =
    {
        [ENET_ICSSG_DUALMAC] = {.hMboxCmd = NULL, .hMboxResponse = NULL},
        [ENET_ICSSG_SWITCH]  = {.hMboxCmd = NULL, .hMboxResponse = NULL},
        [ENET_GMAC_3G] = {.hMboxCmd = NULL, .hMboxResponse = NULL},
        [ENET_CPSW_2G] = {.hMboxCmd = NULL, .hMboxResponse = NULL},
        [ENET_CPSW_5G] = {.hMboxCmd = NULL, .hMboxResponse = NULL},
        [ENET_CPSW_9G] = {.hMboxCmd = NULL, .hMboxResponse = NULL},
    },

    .hMainLoopTask = NULL,
    .hCpuLoadTask  = NULL,
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    TaskP_Params params;

    OS_init();

    EnetBoard_init();

    EnetAppUtils_print("==========================\n");
    EnetAppUtils_print("      Enet lwIP App       \n");
    EnetAppUtils_print("==========================\n");

    EnetApp_getTestConfig(&gEnetAppCfg.enetType,
                          &gEnetAppCfg.instId,
                          &gEnetAppCfg.macPort,
                          &gEnetAppCfg.mii,
                          &gEnetAppCfg.boardId);

    EnetAppUtils_enableClocks(gEnetAppCfg.enetType, gEnetAppCfg.instId);

    /* Create task to print CPU load periodically */
    TaskP_Params_init(&params);
    params.name      = (uint8_t *)"CPU_LOAD";
    params.priority  = 15U;
    params.stack     = gEnetAppLoadTskStack;
    params.stacksize = sizeof(gEnetAppLoadTskStack);

    gEnetAppObj.hCpuLoadTask = TaskP_create(EnetApp_cpuLoadTask, &params);
    if (gEnetAppObj.hCpuLoadTask == NULL)
    {
        EnetAppUtils_print("Failed to create CPU load task\n");
        EnetAppUtils_assert(false);
    }

    /* Create main loop where lwIP is initialized */
    TaskP_Params_init(&params);
    params.name = (uint8_t *)"Main Loop";
    params.priority  = DEFAULT_THREAD_PRIO;
    params.stack     = gEnetAppMainTskStack;
    params.stacksize = sizeof(gEnetAppMainTskStack);

    gEnetAppObj.hMainLoopTask = TaskP_create((void *)main_loop, &params);
    if (gEnetAppObj.hMainLoopTask == NULL)
    {
        EnetAppUtils_print("Failed to create main loop task\n");
        EnetAppUtils_assert(false);
    }

    OS_start();    /* does not return */

    return(0);
}

#if (0U == APP_ENABLE_STATIC_CFG)
#if defined(BUILD_MCU1_0) || defined(BUILD_MCU1_1) || defined(BUILD_MCU2_1)
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
        EnetAppUtils_print("Select MAC port\n");
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
                EnetAppUtils_print("Wrong MAC port, enter again\n");
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
        EnetAppUtils_print("Select MAC port\n");
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
                EnetAppUtils_print("Wrong MAC port, enter again\n");
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
        EnetAppUtils_print("Select MAC port\n");
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
                EnetAppUtils_print("Wrong MAC port, enter again\n");
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
#if defined(BUILD_MCU1_0) || defined(BUILD_MCU2_1)
        "0: CPSW_2G\n"
#endif
#if defined(SOC_J7200) && defined(BUILD_MCU2_0)
        "1: CPSW_5G\n"
#endif
#if defined(SOC_J721E) && defined(BUILD_MCU2_0)
        "2: CPSW_9G\n"
#endif
#if defined(ENET_ENABLE_ICSSG)
        "3: ICSSG Dual MAC\n"
#endif
    };

#if defined(ENET_ENABLE_ICSSG)
    static const char enetIcssgInstSetting[] =
    {
        "0: ICSSG0 Slice0 (MAC port 1)\n"
        "1: ICSSG0 Slice1 (MAC port 2)\n"
        "2: ICSSG1 Slice0 (MAC port 1)\n"
        "3: ICSSG1 Slice1 (MAC port 2)\n"
#if defined(SOC_AM65XX)
        "4: ICSSG2 Slice0 (MAC port 1)\n"
        "5: ICSSG2 Slice1 (MAC port 2)\n"
#endif
    };
#endif

    do
    {
        EnetAppUtils_print("Select peripheral type\n");
        EnetAppUtils_print("%s\n", enetTypeSetting);
        retry = false;

        choice = EnetAppUtils_getNum();
        switch (choice)
        {
#if defined(BUILD_MCU1_0) || defined(BUILD_MCU1_1) || defined(BUILD_MCU2_1)
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
                EnetAppUtils_print("Wrong peripheral type, enter again\n");
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
                    gEnetAppCfg.useTimeSyncPtp = false;
                    break;
                case 1:
                    gEnetAppCfg.useTimeSyncPtp = true;
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
#else /* (1U == APP_ENABLE_STATIC_CFG) */
static void EnetApp_getTestConfig(Enet_Type *enetType,
                                  uint32_t *instId,
                                  Enet_MacPort *macPort,
                                  EnetMacPort_Interface *mii,
                                  uint32_t *boardId)
{
#if defined(BUILD_MCU1_0) || defined(BUILD_MCU2_1)
    *enetType = ENET_CPSW_2G;
    *instId   = 0U;
    *macPort  = ENET_MAC_PORT_1;
    *boardId  = ENETBOARD_CPB_ID;
    mii->layerType    = ENET_MAC_LAYER_GMII;
    mii->sublayerType = ENET_MAC_SUBLAYER_REDUCED;

#elif defined(BUILD_MCU1_1)
#if (defined(SOC_TPR12) || defined(SOC_AWR294X))
    *enetType = ENET_CPSW_2G;
    *instId   = 0U;
    *macPort  = ENET_MAC_PORT_1;
    *boardId  = ENETBOARD_CPB_ID;
    mii->layerType    = ENET_MAC_LAYER_GMII;
    mii->sublayerType = ENET_MAC_SUBLAYER_REDUCED;
#else
#error "mcu1_1 is not supported in this SoC"
#endif

#elif defined(BUILD_MCU2_0)
#if defined(SOC_J721E)
    *enetType = ENET_CPSW_9G;
    *instId   = 0U;
    *macPort  = ENET_MAC_PORT_3;
    *boardId  = ENETBOARD_GESI_ID;
    mii->layerType    = ENET_MAC_LAYER_GMII;
    mii->sublayerType = ENET_MAC_SUBLAYER_REDUCED;
#elif defined(SOC_J7200)
    *enetType = ENET_CPSW_5G;
    *instId   = 0U;
    *macPort  = ENET_MAC_PORT_1;
    *boardId  = ENETBOARD_QPENET_ID;
    mii->layerType    = ENET_MAC_LAYER_GMII;
    mii->sublayerType = ENET_MAC_SUBLAYER_QUAD_SERIAL_MAIN;
#else
#error "mcu2_0 is not supported in this SoC"
#endif

#else
#error "lwIP example is not supported on this core"
#endif

    gEnetAppCfg.useTimeSyncPtp = false;
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
    ethPort.enetType = gEnetAppCfg.enetType;
    ethPort.instId   = gEnetAppCfg.instId;
    ethPort.macPort  = gEnetAppCfg.macPort;
    ethPort.boardId  = gEnetAppCfg.boardId;
    ethPort.mii      = gEnetAppCfg.mii;

    status = EnetBoard_setupPorts(&ethPort, 1U);
    EnetAppUtils_assert(status == ENET_SOK);

    if (Enet_isCpswFamily(gEnetAppCfg.enetType))
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

    if (Enet_isCpswFamily(gEnetAppCfg.enetType))
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

static void EnetApp_portLinkStatusChangeCb(Enet_MacPort macPort,
                                           bool isLinkUp,
                                           void *appArg)
{
    EnetAppUtils_print("MAC Port %u: link %s\n",
                       ENET_MACPORT_ID(macPort), isLinkUp ? "up" : "down");
}

static void EnetApp_mdioLinkStatusChange(Cpsw_MdioLinkStateChangeInfo *info,
                                         void *appArg)
{
    static uint32_t linkUpCount = 0;
    if ((info->linkChanged) && (info->isLinked))
    {
        linkUpCount++;
    }
}

static void EnetApp_initEnetLinkCbPrms(Cpsw_Cfg *cpswCfg)
{

    cpswCfg->mdioLinkStateChangeCb     = EnetApp_mdioLinkStatusChange;
    cpswCfg->mdioLinkStateChangeCbArg  = &gEnetAppObj;

    cpswCfg->portLinkStatusChangeCb    = &EnetApp_portLinkStatusChangeCb;
    cpswCfg->portLinkStatusChangeCbArg = &gEnetAppObj;
}

static int32_t EnetApp_init(Enet_Type enetType)
{
    int32_t status = ENET_SOK;
    EnetMcm_InitConfig enetMcmCfg;
    Cpsw_Cfg cpswCfg;
#if defined(ENET_ENABLE_ICSSG)
    Icssg_Cfg icssgCfg;
#endif
    EnetRm_ResCfg *resCfg;
    Enet_MacPort macPortList[] = {gEnetAppCfg.macPort};
    uint8_t numMacPorts = (sizeof(macPortList) / sizeof(macPortList[0U]));

    EnetAppUtils_assert(numMacPorts <=
                        Enet_getMacPortMax(gEnetAppCfg.enetType, gEnetAppCfg.instId));

#if (defined(SOC_TPR12) || defined(SOC_AWR294X))
    EnetCpdma_Cfg dmaCfg;

    dmaCfg.rxInterruptPerMSec = 2;
#else
    EnetUdma_Cfg dmaCfg;

    /* Open UDMA */
    gEnetAppObj.hUdmaDrv = EnetAppUtils_udmaOpen(gEnetAppCfg.enetType, NULL);
    EnetAppUtils_assert(NULL != gEnetAppObj.hUdmaDrv);
    dmaCfg.rxChInitPrms.dmaPriority = UDMA_DEFAULT_RX_CH_DMA_PRIORITY;
    dmaCfg.hUdmaDrv = gEnetAppObj.hUdmaDrv;
#endif

    /* Set configuration parameters */
    if (Enet_isCpswFamily(enetType))
    {
        Enet_initCfg(gEnetAppCfg.enetType, gEnetAppCfg.instId, &cpswCfg, sizeof(cpswCfg));
        cpswCfg.vlanCfg.vlanAware          = false;
        cpswCfg.hostPortCfg.removeCrc      = true;
        cpswCfg.hostPortCfg.padShortPacket = true;
        cpswCfg.hostPortCfg.passCrcErrors  = true;

        EnetApp_initEnetLinkCbPrms(&cpswCfg);
        resCfg = &cpswCfg.resCfg;

        EnetApp_initAleConfig(&cpswCfg.aleCfg);
        cpswCfg.dmaCfg = (void *)&dmaCfg;

        enetMcmCfg.perCfg = &cpswCfg;
    }
#if defined(ENET_ENABLE_ICSSG)
    else
    {
        Enet_initCfg(gEnetAppCfg.enetType, gEnetAppCfg.instId, &icssgCfg, sizeof(icssgCfg));

        /* Currently we only support one ICSSG port */
        EnetAppUtils_assert(numMacPorts == 1U);

        resCfg = &icssgCfg.resCfg;
        icssgCfg.dmaCfg = (void *)&dmaCfg;

        enetMcmCfg.perCfg = &icssgCfg;
    }
#endif

    EnetAppUtils_assert(NULL != enetMcmCfg.perCfg);
    EnetAppUtils_initResourceConfig(gEnetAppCfg.enetType, EnetSoc_getCoreId(), resCfg);

    enetMcmCfg.enetType           = gEnetAppCfg.enetType;
    enetMcmCfg.instId             = gEnetAppCfg.instId;
    enetMcmCfg.setPortLinkCfg     = EnetApp_initLinkArgs;
    enetMcmCfg.numMacPorts        = numMacPorts;
    enetMcmCfg.periodicTaskPeriod = ENETPHY_FSM_TICK_PERIOD_MS;
    enetMcmCfg.print              = EnetAppUtils_print;

    memcpy(&enetMcmCfg.macPortList[0U], &macPortList[0U], sizeof(macPortList));
    status = EnetMcm_init(&enetMcmCfg);

    return status;
}

void EnetApp_netIfCb(struct netif *netif)
{
    const ip4_addr_t *ipAddr;
#if defined(ENET_ENABLE_TIMESYNC)
    uint32_t addr;
#endif

    if (netif_is_up(netif))
    {
        ipAddr = netif_ip4_addr(netif);

        if (ipAddr->addr != 0)
        {
            EnetAppUtils_print("Enet lwIP App: Added Network IP address I/F %c%c%d: %s\n",
                               netif->name[0], netif->name[1], netif->num,
                               ip4addr_ntoa(ipAddr));

#if defined(ENET_ENABLE_TIMESYNC)
            if (gEnetAppCfg.useTimeSyncPtp)
            {
                addr = lwip_ntohl(ip_addr_get_ip4_u32(ipAddr));

                EnetApp_initTimeSyncPtp(gEnetAppCfg.enetType,
                                        gEnetAppCfg.macPort,
                                        &gEnetAppObj.macAddr[0U],
                                        addr);
            }
#endif
        }
    }
    else
    {
        EnetAppUtils_print("Enet lwIP App: Removed Network IP address I/F %c%c%u\n",
                           netif->name[0], netif->name[1], netif->num);
#if defined(ENET_ENABLE_TIMESYNC)
        if (gEnetAppCfg.useTimeSyncPtp)
        {
            EnetApp_deinitTimeSyncPtp();
        }
#endif

        /* Deinit the app as it would be again initialized in LwipifEnetAppCb_getHandle */
        EnetApp_deinit();
    }
}

static bool EnetApp_isPortLinked(Enet_Handle hEnet)
{
    uint32_t coreId = EnetSoc_getCoreId();

    return EnetAppUtils_isPortLinkUp(hEnet, coreId, gEnetAppCfg.macPort);
}

void LwipifEnetAppCb_getHandle(LwipifEnetAppIf_GetHandleInArgs *inArgs,
                               LwipifEnetAppIf_GetHandleOutArgs *outArgs)
{
    int32_t status;
    EnetMcm_HandleInfo handleInfo;
    EnetPer_AttachCoreOutArgs attachInfo;
    LwipifEnetAppIf_RxHandleInfo *rxInfo;
    LwipifEnetAppIf_RxConfig *rxCfg;
    EnetMcm_CmdIf *pMcmCmdIf = &gEnetAppObj.hMcmCmdIf[gEnetAppCfg.enetType];
#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
    EnetUdma_OpenRxFlowPrms enetRxFlowCfg;
    EnetUdma_OpenTxChPrms enetTxChCfg;
    bool useRingMon = false;
    bool useDfltFlow = gEnetAppCfg.useDfltFlow;
#elif defined (ENET_SOC_HOSTPORT_DMA_TYPE_CPDMA)
    EnetCpdma_OpenRxChPrms enetRxFlowCfg;
    EnetCpdma_OpenTxChPrms enetTxChCfg;
#endif
    uint32_t coreId = EnetSoc_getCoreId();

    if (pMcmCmdIf->hMboxCmd == NULL)
    {
        status = EnetApp_init(gEnetAppCfg.enetType);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("Failed to open Enet app: %d\n", status);
            EnetAppUtils_assert(status == ENET_SOK);
        }

        EnetMcm_getCmdIf(gEnetAppCfg.enetType, pMcmCmdIf);
    }

    EnetAppUtils_assert(pMcmCmdIf->hMboxCmd != NULL);
    EnetAppUtils_assert(pMcmCmdIf->hMboxResponse != NULL);
    EnetMcm_acquireHandleInfo(pMcmCmdIf, &handleInfo);
    EnetMcm_coreAttach(pMcmCmdIf, coreId, &attachInfo);

    /* Confirm HW checksum offload is enabled as stack enables it by default */
    if (Enet_isCpswFamily(gEnetAppCfg.enetType))
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
            EnetAppUtils_print("Failed to get checksum offload info: %d\n", status);
        }

        EnetAppUtils_assert(true == csumOffloadFlg);
    }

    /* Open TX channel */
    EnetDma_initTxChParams(&enetTxChCfg);

#if defined(ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
    enetTxChCfg.hUdmaDrv  = handleInfo.hUdmaDrv;
    enetTxChCfg.useProxy  = true;
#endif
    enetTxChCfg.numTxPkts = inArgs->txCfg.numPackets;
    enetTxChCfg.cbArg     = inArgs->txCfg.cbArg;
    enetTxChCfg.notifyCb  = inArgs->txCfg.notifyCb;
    EnetAppUtils_setCommonTxChPrms(&enetTxChCfg);

    EnetAppUtils_openTxCh(handleInfo.hEnet,
                          attachInfo.coreKey,
                          coreId,
                          &outArgs->txInfo.txChNum,
                          &outArgs->txInfo.hTxChannel,
                          &enetTxChCfg);

    /* Open first RX channel/flow */
    rxInfo = &outArgs->rxInfo[0U];
    rxCfg = &inArgs->rxCfg[0U];

    EnetDma_initRxChParams(&enetRxFlowCfg);
    enetRxFlowCfg.notifyCb  = rxCfg->notifyCb;
    enetRxFlowCfg.numRxPkts = rxCfg->numPackets;
    enetRxFlowCfg.cbArg     = rxCfg->cbArg;
#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
    enetRxFlowCfg.hUdmaDrv  = handleInfo.hUdmaDrv;
    enetRxFlowCfg.useProxy  = true;

    /* Use ring monitor for the CQ ring of RX flow */
    EnetUdma_UdmaRingPrms *pFqRingPrms = &enetRxFlowCfg.udmaChPrms.fqRingPrms;
    pFqRingPrms->useRingMon = useRingMon;
    pFqRingPrms->ringMonCfg.mode = TISCI_MSG_VALUE_RM_MON_MODE_THRESHOLD;
    /* Ring mon low threshold */

#if defined _DEBUG_
    /* In debug mode as CPU is processing lesser packets per event, keep threshold more */
    pFqRingPrms->ringMonCfg.data0 = (rxCfg->numPackets - 10U);
#else
    pFqRingPrms->ringMonCfg.data0 = (rxCfg->numPackets - 20U);
#endif
    /* Ring mon high threshold - to get only low  threshold event, setting high threshold as more than ring depth*/
    pFqRingPrms->ringMonCfg.data1 = rxCfg->numPackets;
    EnetAppUtils_setCommonRxFlowPrms(&enetRxFlowCfg);

#if defined(ENET_ENABLE_ICSSG)
    if(Enet_isIcssFamily(gEnetAppCfg.enetType))
    {
        enetRxFlowCfg.flowPrms.sizeThreshEn = 0U;
    }
#endif

    EnetAppUtils_openRxFlow(gEnetAppCfg.enetType,
                            handleInfo.hEnet,
                            attachInfo.coreKey,
                            coreId,
                            useDfltFlow,
                            &rxInfo->rxFlowStartIdx,
                            &rxInfo->rxFlowIdx,
                            &rxInfo->macAddr[0U],
                            &rxInfo->hRxFlow,
                            &enetRxFlowCfg);

#elif defined (ENET_SOC_HOSTPORT_DMA_TYPE_CPDMA)
    EnetAppUtils_setCommonRxChPrms(&enetRxFlowCfg);
    EnetAppUtils_openRxCh(handleInfo.hEnet,
                          attachInfo.coreKey,
                          coreId,
                          &rxInfo->rxFlowIdx,
                          &rxInfo->hRxFlow,
                          &enetRxFlowCfg);
#endif

    outArgs->coreId          = coreId;
    outArgs->coreKey         = attachInfo.coreKey;
    outArgs->hEnet           = handleInfo.hEnet;
    outArgs->hostPortRxMtu   = attachInfo.rxMtu;
    ENET_UTILS_ARRAY_COPY(outArgs->txMtu, attachInfo.txMtu);
    outArgs->print           = &EnetAppUtils_print;
    outArgs->isPortLinkedFxn = &EnetApp_isPortLinked;
    outArgs->timerPeriodUs   = ENETAPP_PACKET_POLL_PERIOD_US;

#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
    outArgs->hUdmaDrv = handleInfo.hUdmaDrv;
    rxInfo->disableEvent = !useRingMon;
#else
    rxInfo->disableEvent = true;
#endif
    /* Use optimized processing where TX packets are relinquished in next TX submit call */
    outArgs->txInfo.disableEvent = true;

#if (defined(SOC_TPR12) || defined(SOC_AWR294X))
    //FIXME - TPR12 openRxCh doesn't allocate MAC addresses which seems correct behaviour
    status = EnetAppUtils_allocMac(handleInfo.hEnet,
                                   outArgs->coreKey,
                                   outArgs->coreId,
                                   &rxInfo->macAddr[0U]);
#endif

    EnetAppUtils_print("Host MAC address: ");
    EnetAppUtils_printMacAddr(&rxInfo->macAddr[0U]);

    EnetUtils_copyMacAddr(&gEnetAppObj.macAddr[0U], &rxInfo->macAddr[0U]);

#if defined(ENET_ENABLE_ICSSG)
    /* Add port MAC entry in case of ICSSG dual MAC */
    if (ENET_ICSSG_DUALMAC == gEnetAppCfg.enetType)
    {
        Enet_IoctlPrms prms;
        IcssgMacPort_SetMacAddressInArgs inArgs;

        memset(&inArgs, 0, sizeof(inArgs));
        memcpy(&inArgs.macAddr[0U], &rxInfo->macAddr[0U], sizeof(inArgs.macAddr));
        inArgs.macPort = gEnetAppCfg.macPort;

        ENET_IOCTL_SET_IN_ARGS(&prms, &inArgs);
        status = Enet_ioctl(handleInfo.hEnet, coreId, ICSSG_MACPORT_IOCTL_SET_MACADDR, &prms);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("Failed to set MAC address: %d\n", status);
            EnetAppUtils_assert(status == ENET_SOK);
        }
    }
#endif

    rxInfo->handlePktFxn = NULL;
}

void LwipifEnetAppCb_releaseHandle(LwipifEnetAppIf_ReleaseHandleInfo *releaseInfo)
{
    EnetMcm_CmdIf *pMcmCmdIf = &gEnetAppObj.hMcmCmdIf[gEnetAppCfg.enetType];
    Lwip2EnetAppIf_FreePktInfo *freePktInfo;
    EnetDma_PktQ fqPktInfoQ;
    EnetDma_PktQ cqPktInfoQ;
#if !(defined(SOC_TPR12) || defined(SOC_AWR294X))
    LwipifEnetAppIf_RxHandleInfo *rxInfo;
    bool useDfltFlow = gEnetAppCfg.useDfltFlow;
#endif

    EnetAppUtils_assert(pMcmCmdIf->hMboxCmd != NULL);
    EnetAppUtils_assert(pMcmCmdIf->hMboxResponse != NULL);

    /* Close TX channel */
    EnetQueue_initQ(&fqPktInfoQ);
    EnetQueue_initQ(&cqPktInfoQ);
    EnetAppUtils_closeTxCh(releaseInfo->hEnet,
                           releaseInfo->coreKey,
                           releaseInfo->coreId,
                           &fqPktInfoQ,
                           &cqPktInfoQ,
                           releaseInfo->txInfo.hTxChannel,
                           releaseInfo->txInfo.txChNum);
    releaseInfo->txFreePkt.cb(releaseInfo->txFreePkt.cbArg, &fqPktInfoQ, &cqPktInfoQ);

    /* Close first RX channel/flow */
    freePktInfo = &releaseInfo->rxFreePkt[0U];

    EnetQueue_initQ(&fqPktInfoQ);
    EnetQueue_initQ(&cqPktInfoQ);
#if !(defined(SOC_TPR12) || defined(SOC_AWR294X))
    rxInfo = &releaseInfo->rxInfo[0U];
    EnetAppUtils_closeRxFlow(gEnetAppCfg.enetType,
                             releaseInfo->hEnet,
                             releaseInfo->coreKey,
                             releaseInfo->coreId,
                             useDfltFlow,
                             &fqPktInfoQ,
                             &cqPktInfoQ,
                             rxInfo->rxFlowStartIdx,
                             rxInfo->rxFlowIdx,
                             rxInfo->macAddr,
                             rxInfo->hRxFlow);
#endif
    freePktInfo->cb(freePktInfo->cbArg, &fqPktInfoQ, &cqPktInfoQ);

    EnetMcm_coreDetach(pMcmCmdIf, releaseInfo->coreId, releaseInfo->coreKey);
    EnetMcm_releaseHandleInfo(pMcmCmdIf);
}

static void EnetApp_deinit(void)
{
#if !(defined(SOC_TPR12) || defined(SOC_AWR294X))
    EnetAppUtils_udmaclose(gEnetAppObj.hUdmaDrv);
#endif
    EnetMcm_deInit(gEnetAppCfg.enetType);
    memset(&gEnetAppObj, 0U, sizeof(gEnetAppObj));
}

static void EnetApp_cpuLoadTask(void *a0,
                                void *a1)
{
    volatile uint32_t enableLoad = 1U;

    while (enableLoad)
    {
        /* LoadP_getCPULoad() and LoadP_reset() currently supported only for FreeRTOS */
#if defined(FREERTOS)
        EnetAppUtils_print("CPU Load: %u%%\n", LoadP_getCPULoad());
        LoadP_reset();
#endif
        TaskP_sleep(10000U);
    }
}

/* PTP related functions */

#if defined(ENET_ENABLE_TIMESYNC)
void EnetApp_initTimeSyncPtp(Enet_Type enetType,
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
        ptpCfg.socConfig.instId = gEnetAppCfg.instId;

        /* Save host port IP address and MAC address */
        memcpy(&ptpCfg.ipAddr[0U], &ipAddr, ENET_IPv4_ADDR_LEN);
        memcpy(&ptpCfg.ifMacID[0U], hostMacAddr, ENET_MAC_ADDR_LEN);

        gEnetAppObj.hTimeSyncPtp = TimeSyncPtp_init(&ptpCfg);
        EnetAppUtils_assert(gEnetAppObj.hTimeSyncPtp != NULL);

        TimeSyncPtp_enable(gEnetAppObj.hTimeSyncPtp);
        EnetAppUtils_print("TimeSync PTP enabled\n");
    }
}

void EnetApp_deinitTimeSyncPtp(void)
{
    if (gEnetAppObj.hTimeSyncPtp != NULL)
    {
        TimeSyncPtp_disable(gEnetAppObj.hTimeSyncPtp);

        /* ETHFW-1767 - TimeSync PTP doesn't support deinit */
        //TimeSyncPtp_deInit(gEnetAppObj.hTimeSyncPtp);
    }
}
#endif
