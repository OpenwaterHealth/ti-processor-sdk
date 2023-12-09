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
 * \file  enet_loopback.c
 *
 * \brief This file contains the implementation of the Enet loopback example.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>
#include <ti/osal/SemaphoreP.h>
#include <ti/osal/ClockP.h>

#include <ti/board/board.h>
#if defined(SOC_J721E)
#include <ti/board/src/j721e_evm/include/board_ethernet_config.h>
#elif defined(SOC_J7200)
#include <ti/board/src/j7200_evm/include/board_ethernet_config.h>
#elif defined(SOC_AM65XX)
#include <ti/board/src/am65xx_evm/include/board_ethernet_config.h>
#elif defined(SOC_TPR12)
#include <ti/board/src/tpr12_evm/include/board_ethernet_config.h>
#elif defined(SOC_AWR294X)
#include <ti/board/src/awr294x_evm/include/board_ethernet_config.h>
#endif

#if !(defined(SOC_TPR12) || defined(SOC_AWR294X))
#include <ti/drv/ipc/ipc.h>
#include <ti/drv/pm/pmlib.h>
#include <ti/drv/sciclient/sciclient.h>
#endif
#include <ti/drv/enet/enet.h>
#include <ti/drv/enet/enet_cfg.h>
#include <ti/drv/enet/include/core/enet_dma.h>
#include <ti/drv/enet/include/per/cpsw.h>

#include <ti/drv/enet/examples/utils/include/enet_apputils.h>
#include <ti/drv/enet/examples/utils/include/enet_apputils_rtos.h>
#include <ti/drv/enet/examples/utils/include/enet_appmemutils.h>
#include <ti/drv/enet/examples/utils/include/enet_appmemutils_cfg.h>
#include <ti/drv/enet/examples/utils/include/enet_appboardutils.h>
#include <ti/drv/enet/examples/utils/include/enet_apputils_rtos.h>
#if defined(SOC_J721E) || defined(SOC_J7200)
#include <ti/drv/enet/examples/utils/include/enet_board_j7xevm.h>
#elif defined(SOC_AM65XX)
#include <ti/drv/enet/examples/utils/include/enet_board_am65xevm.h>
#elif defined(SOC_TPR12)
#include <ti/drv/enet/examples/utils/include/enet_board_tpr12evm.h>
#elif defined(SOC_AWR294X)
#include <ti/drv/enet/examples/utils/include/enet_board_awr294xevm.h>
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Loopback test iteration count */
#ifndef SIM_BUILD
#define ENETLPBK_NUM_ITERATION                     (5U)
#else
#define ENETLPBK_NUM_ITERATION                     (2U)
#endif

/* 100-ms periodic tick */
#define ENETLPBK_PERIODIC_TICK_MS                  (100U)

/* Task stack size */
#define ENETLPBK_TASK_STACK_SZ                     (10U * 1024U)

/* Helper macro used to create loopback port menu options */
#define ENETLPBK_PORT_OPT(macPort, macMode, boardId) { #macPort " - "  #macMode, (macPort), (macMode), (boardId) }

#define APP_ENABLE_STATIC_CFG                      (0U)

#ifndef SIM_BUILD
#define ENETLPBK_TEST_PKT_NUM                      (1000U)
#else
#define ENETLPBK_TEST_PKT_NUM                      (20U)
#endif

#define ENETLPBK_TEST_PKT_LEN                      (500U)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct EnetLpbk_Obj_s
{
    /* Enet driver */
    Enet_Handle hEnet;
    Enet_Type enetType;
    uint32_t instId;
    uint32_t coreId;
    uint32_t coreKey;
    uint32_t boardId;
    Enet_MacPort macPort;
    emac_mode macMode;      /* MAC mode (defined in board library) */

#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
    /* UDMA driver handle */
    Udma_DrvHandle hUdmaDrv;
    uint32_t rxFlowIdx;
    uint32_t rxStartFlowIdx;
#elif defined (ENET_SOC_HOSTPORT_DMA_TYPE_CPDMA)
    uint32_t rxChNum;
#endif
    EnetDma_RxChHandle hRxCh;
    EnetDma_PktQ rxFreeQ;
    EnetDma_PktQ rxReadyQ;
    EnetDma_TxChHandle hTxCh;
    EnetDma_PktQ txFreePktInfoQ;
    uint32_t txChNum;
    uint8_t hostMacAddr[ENET_MAC_ADDR_LEN];

    /* Test config params */
    bool testExtLoopback;   /* TODO: replace testPhyLoopback as testLoopBackType (MAC, PHY and External) */
    bool testPhyLoopback;   /* PHY loopback or MAC loopback? */
    bool printFrame;        /* Print received Ethernet frames? */

    /* Test runtime params */
    volatile bool exitFlag; /* Exit test? */


    /* Periodic tick */
    ClockP_Handle hTickTimer;
    TaskP_Handle hTickTask;
    SemaphoreP_Handle hTimerSem;

    /* Packet transmission */
    TaskP_Handle hTxTask;
    SemaphoreP_Handle hTxSem;
    SemaphoreP_Handle hTxDoneSem;
    uint32_t totalTxCnt;

    /* Packet reception */
    TaskP_Handle hRxTask;
    SemaphoreP_Handle hRxSem;
    SemaphoreP_Handle hRxDoneSem;
    uint32_t totalRxCnt;
} EnetLpbk_Obj;

typedef struct EnetLpbk_EnetTypeMenu_s
{
    const char *text;
    Enet_Type enetType;
    uint32_t instId;
} EnetLpbk_EnetTypeMenu;

typedef struct EnetLpbk_PortMenu_s
{
    const char *text;
    Enet_MacPort macPort;
    emac_mode macMode;
    uint32_t boardId;
} EnetLpbk_PortMenu;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void EnetLpbk_mainTask(void* a0, void* a1);

static void EnetLpbk_createClock(void);

static void EnetLpbk_deleteClock(void);

static void EnetLpbk_timerCallback(void* arg);

static void EnetLpbk_tickTask(void* a0, void* a1);

static void EnetLpbk_txTask(void* a0, void* a1);

static void EnetLpbk_rxTask(void* a0, void* a1);

static void EnetLpbk_createRxTxTasks(void);

static void EnetLpbk_deleteRxTxTasks(void);

static int32_t EnetLpbk_loopbackTest(void);

static void EnetLpbk_initCpswCfg(Cpsw_Cfg *cpswCfg);

static int32_t EnetLpbk_setupCpswAle(void);

static int32_t EnetLpbk_openEnet(void);

static void EnetLpbk_closeEnet(void);

static int32_t EnetLpbk_showAlivePhys(void);

static int32_t EnetLpbk_waitForLinkUp(void);

static void EnetLpbk_showCpswStats(void);

#if 0 //TODO - NEED TO BE PORTED
static uint32_t EnetLpbk_getSystemHeapFreeSpace(void);
#endif

#if (0U == APP_ENABLE_STATIC_CFG)
static void EnetLpbk_showMenu(void);

static void EnetLpbk_showEnetTypeMenu(Enet_Type *enetType,
                                      uint32_t *instId);

static void EnetLpbk_showLpbkMenu(bool *testPhyLoopback);

static int32_t EnetLpbk_showPortMenu(Enet_Type enetType,
                                     uint32_t instId,
                                     bool testPhyLoopback,
                                     Enet_MacPort *macPort,
                                     emac_mode *macMode,
                                     uint32_t *boardId);
#endif

void EnetLpbk_waitForDebugger(void);

static int32_t EnetLpbk_macMode2PhyMii(emac_mode macMode,
                                    EnetPhy_Mii *mii);

static void EnetLpbk_macMode2MacMii(emac_mode macMode,
                                    EnetMacPort_Interface *mii);

int32_t EnetLpbk_openDma();

void EnetLpbk_closeDma();

#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
Udma_DrvHandle EnetLpbkUtils_udmaOpen(Enet_Type enetType,
                                     Udma_InitPrms *pInitPrms);
#endif
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Enet loopback test object */
EnetLpbk_Obj gEnetLpbk;

/* Test application stack */
static uint8_t gEnetLpbkTaskStackMain[ENETLPBK_TASK_STACK_SZ] __attribute__ ((aligned(32)));
static uint8_t gEnetLpbkTaskStackTick[ENETLPBK_TASK_STACK_SZ] __attribute__ ((aligned(32)));
static uint8_t gEnetLpbkTaskStackTx[ENETLPBK_TASK_STACK_SZ] __attribute__ ((aligned(32)));
static uint8_t gEnetLpbkTaskStackRx[ENETLPBK_TASK_STACK_SZ] __attribute__ ((aligned(32)));

#if (0U == APP_ENABLE_STATIC_CFG)
#if (defined(SOC_J721E) || defined(SOC_J7200) || defined(SOC_AM65XX) || defined(SOC_TPR12) || defined(SOC_AWR294X))
static EnetLpbk_EnetTypeMenu gEnetLpbk_EnetTypeMenu[] =
{
#if defined(BUILD_MCU1_0) || defined(BUILD_MCU1_1) || defined(BUILD_MCU2_1) || defined(BUILD_MPU1_0)
    { "CPSW 2G", ENET_CPSW_2G, 0U },
#elif defined(SOC_J7200) && defined(BUILD_MCU2_0)
    { "CPSW 5G", ENET_CPSW_5G, 0U },
#elif defined(SOC_J721E) && defined(BUILD_MCU2_0)
    { "CPSW 9G", ENET_CPSW_9G, 0U },
#endif
};

static EnetLpbk_PortMenu gEnetLpbk_MacLpbkMenu[] =
{
    ENETLPBK_PORT_OPT(ENET_MAC_PORT_1, RGMII, ENETBOARD_LOOPBACK_ID),
};

static EnetLpbk_PortMenu gEnetLpbk_cpsw2gPhyLpbkMenu[] =
{
#if defined(BUILD_MCU1_0) || defined(BUILD_MCU1_1) || defined(BUILD_MCU2_1) || defined(BUILD_MPU1_0)
    ENETLPBK_PORT_OPT(ENET_MAC_PORT_1, RGMII, ENETBOARD_CPB_ID),
#endif
};

static EnetLpbk_PortMenu gEnetLpbk_cpsw5gPhyLpbkMenu[] =
{
#if defined(SOC_J7200) && defined(BUILD_MCU2_0)
    ENETLPBK_PORT_OPT(ENET_MAC_PORT_2, RGMII,  ENETBOARD_GESI_ID),
#endif
};

static EnetLpbk_PortMenu gEnetLpbk_cpsw9gPhyLpbkMenu[] =
{
#if defined(SOC_J721E) && defined(BUILD_MCU2_0)
    ENETLPBK_PORT_OPT(ENET_MAC_PORT_1, RGMII,  ENETBOARD_GESI_ID),
    ENETLPBK_PORT_OPT(ENET_MAC_PORT_3, RGMII,  ENETBOARD_GESI_ID),
    ENETLPBK_PORT_OPT(ENET_MAC_PORT_4, RGMII,  ENETBOARD_GESI_ID),
    ENETLPBK_PORT_OPT(ENET_MAC_PORT_8, RGMII,  ENETBOARD_GESI_ID),
    ENETLPBK_PORT_OPT(ENET_MAC_PORT_8, RMII,   ENETBOARD_GESI_ID),
#endif
};
#endif
#endif

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    TaskP_Handle hTask;
    TaskP_Params params;
	OS_init();

    /* Initialize loopback test config */
    memset(&gEnetLpbk, 0, sizeof(gEnetLpbk));
    gEnetLpbk.exitFlag = false;
#if defined(SOC_AM65XX)
    gEnetLpbk.enetType = ENET_CPSW_2G;
#else
    gEnetLpbk.enetType = ENET_CPSW_9G;
#endif
    gEnetLpbk.instId = 0U;

    /* Initialize the main task params */
    TaskP_Params_init(&params);
    params.priority       = 2U;
    params.stack          = gEnetLpbkTaskStackMain;
    params.stacksize      = sizeof(gEnetLpbkTaskStackMain);
    params.name           = (uint8_t *)"Loopback main task";

    hTask = TaskP_create(EnetLpbk_mainTask, &params);
    if (hTask == NULL)
    {
        EnetAppUtils_print("main() failed to create main task\n");
        OS_stop();
    }

    /* Does not return */
    OS_start();

    return 0;
}

static void EnetLpbk_mainTask(void* a0,
                              void* a1)
{
#if 0 //TODO - NEED TO BE PORTED
    uint32_t freeSize0, freeSize1;
#endif
    uint32_t i;
    int32_t status;

    EnetBoard_init();

#if (1U == APP_ENABLE_STATIC_CFG)
    gEnetLpbk.enetType        = ENET_CPSW_9G;
    gEnetLpbk.instId          = 0U;
    gEnetLpbk.testPhyLoopback = true;
    gEnetLpbk.macPort         = ENET_MAC_PORT_1;
    gEnetLpbk.macMode         = RGMII;
#if defined(SOC_AM65XX) || defined(SOC_TPR12) || defined(SOC_AWR294X)
    gEnetLpbk.enetType        = ENET_CPSW_2G;
    gEnetLpbk.boardId         = ENETBOARD_CPB_ID;
#else
    gEnetLpbk.boardId         = ENETBOARD_GESI_ID;
#endif
#else
    EnetLpbk_showMenu();
#endif

    for (i = 0U; i < ENETLPBK_NUM_ITERATION; i++)
    {
#if 0 //TODO - NEED TO BE PORTED
        freeSize0 = EnetLpbk_getSystemHeapFreeSpace();
#endif

        /* Note: Clock create/delete must be done per iteration to account for
         * memory allocation (from heap) done in snprintf for code reentrancy.
         * Moving this out will result in heap memory leak error in external
         * loopback mode on A72. */
        EnetLpbk_createClock();

        EnetAppUtils_print("=============================\n");
        EnetAppUtils_print(" Enet Loopback: Iteration %u \n", i + 1);
        EnetAppUtils_print("=============================\n");

        /* Run the loopback test */
        status = EnetLpbk_loopbackTest();

        EnetLpbk_deleteClock();

#if 0 //TODO - NEED TO BE PORTED
        freeSize1 = EnetLpbk_getSystemHeapFreeSpace();

        EnetAppUtils_assert(freeSize0 == freeSize1);
#endif
        /* Sleep at end of each iteration to allow idle task to delete all terminated tasks */
        TaskP_sleep(1000);
    }

    if (status == ENET_SOK)
    {
        EnetAppUtils_print("Loopback application completed\n");
    }
    else
    {
        EnetAppUtils_print("Loopback application failed to complete\n");
    }

    EnetBoard_deinit();
}

static void EnetLpbk_createClock(void)
{
    TaskP_Params taskParams;
    SemaphoreP_Params semParams;
    ClockP_Params clkParams;

    /* Initialize timer semaphore params */
    SemaphoreP_Params_init(&semParams);
    semParams.mode = SemaphoreP_Mode_COUNTING;

    /* Create timer semaphore */
    gEnetLpbk.hTimerSem = SemaphoreP_create(0, &semParams);

    /* Reset the exitFlag */
    gEnetLpbk.exitFlag = false;

    /* Initialize the periodic tick task params */
    TaskP_Params_init(&taskParams);
    taskParams.priority       = 7U;
    taskParams.stack          = gEnetLpbkTaskStackTick;
    taskParams.stacksize      = sizeof(gEnetLpbkTaskStackTick);
    taskParams.arg0           = (void*)gEnetLpbk.hTimerSem;
    taskParams.name           = (uint8_t *)"Periodic tick task";

    /* Create periodic tick task */
    gEnetLpbk.hTickTask = TaskP_create(EnetLpbk_tickTask, &taskParams);
    if (gEnetLpbk.hTickTask == NULL)
    {
        EnetAppUtils_print("EnetLpbk_createClock() failed to create tick task\n");
        OS_stop();
    }

    ClockP_Params_init(&clkParams);
    clkParams.startMode = ClockP_StartMode_USER;
    clkParams.period    = ENETLPBK_PERIODIC_TICK_MS;
    clkParams.runMode   = ClockP_RunMode_CONTINUOUS;
    clkParams.arg       = (void*)gEnetLpbk.hTimerSem;

    /* Creating timer and setting timer callback function*/
    gEnetLpbk.hTickTimer = ClockP_create(EnetLpbk_timerCallback, &clkParams);
    if (gEnetLpbk.hTickTimer == NULL)
    {
        EnetAppUtils_print("EnetLpbk_createClock() failed to create clock\n");
        OS_stop();
    }
}

static void EnetLpbk_deleteClock(void)
{
    gEnetLpbk.exitFlag = true;

    /* Delete periodic tick timer */
    if (gEnetLpbk.hTickTimer != NULL)
    {
        ClockP_delete(gEnetLpbk.hTickTimer);
        gEnetLpbk.hTickTimer = NULL;
    }

    /* Delete periodic tick task */
    if (gEnetLpbk.hTickTask != NULL)
    {
#if !defined (FREERTOS)
        TaskP_delete(&gEnetLpbk.hTickTask);
#endif
        gEnetLpbk.hTickTask = NULL;
    }

    /* Delete periodic tick timer */
    if (gEnetLpbk.hTimerSem != NULL)
    {
        SemaphoreP_delete(gEnetLpbk.hTimerSem);
        gEnetLpbk.hTimerSem = NULL;
    }
}

static void EnetLpbk_timerCallback(void* arg)
{
    SemaphoreP_Handle hSem = (SemaphoreP_Handle)arg;

    /* Tick! */
    SemaphoreP_post(hSem);
}

static void EnetLpbk_tickTask(void* a0,
                              void* a1)
{
    SemaphoreP_Handle hSem = (SemaphoreP_Handle)a0;

    while (!gEnetLpbk.exitFlag)
    {
        SemaphoreP_pend(hSem, SemaphoreP_WAIT_FOREVER);

        /* PeriodicTick should be called from non-ISR context */
        Enet_periodicTick(gEnetLpbk.hEnet);
    }
    EnetAppUtils_print("EnetLpbk_tickTask() exiting..\n");
}

static void EnetLpbk_createRxTxTasks(void)
{
    TaskP_Params params;

    TaskP_Params_init(&params);
    params.priority       = 2U;
    params.stack          = gEnetLpbkTaskStackTx;
    params.stacksize      = sizeof(gEnetLpbkTaskStackTx);
    params.name           = (uint8_t *)"Tx Task";

    gEnetLpbk.hTxTask = TaskP_create(EnetLpbk_txTask, &params);
    if (gEnetLpbk.hTxTask == NULL)
    {
        EnetAppUtils_print("EnetLpbk_createRxTxTasks() failed to create tx task\n");
        OS_stop();
    }

    TaskP_Params_init(&params);
    params.priority       = 3U;
    params.stack          = gEnetLpbkTaskStackRx;
    params.stacksize      = sizeof(gEnetLpbkTaskStackRx);
    params.name           = (uint8_t *)"Rx Task";

    gEnetLpbk.hRxTask = TaskP_create(EnetLpbk_rxTask, &params);
    if (gEnetLpbk.hRxTask == NULL)
    {
        EnetAppUtils_print("EnetLpbk_createRxTxTasks() failed to create rx task\n");
        OS_stop();
    }
}

static void EnetLpbk_deleteRxTxTasks(void)
{
    if (gEnetLpbk.hTxTask != NULL)
    {
#if !defined (FREERTOS)
        EnetAppUtils_assert(TaskP_isTerminated(gEnetLpbk.hTxTask) == 1);
        TaskP_delete(&gEnetLpbk.hTxTask);
#endif
        gEnetLpbk.hTxTask = NULL;
    }

    if (gEnetLpbk.hRxTask != NULL)
    {
#if !defined (FREERTOS)
        EnetAppUtils_assert(TaskP_isTerminated(gEnetLpbk.hRxTask) == 1);
        TaskP_delete(&gEnetLpbk.hRxTask);
#endif
        gEnetLpbk.hRxTask = NULL;
    }
    EnetAppUtils_print("EnetLpbk_deleteRxTxTasks() done..\n");
}


uint32_t EnetLpbk_retrieveFreeTxPkts(void)
{
    EnetDma_PktQ txFreeQ;
    EnetDma_Pkt *pktInfo;
    int32_t status;
    uint32_t txFreeQCnt = 0U;

    EnetQueue_initQ(&txFreeQ);

    /* Retrieve any CPSW packets that may be free now */
    status = EnetDma_retrieveTxPktQ(gEnetLpbk.hTxCh, &txFreeQ);
    if (status == ENET_SOK)
    {
        txFreeQCnt = EnetQueue_getQCount(&txFreeQ);

        pktInfo = (EnetDma_Pkt *)EnetQueue_deq(&txFreeQ);
        while (NULL != pktInfo)
        {
            EnetDma_checkPktState(&pktInfo->pktState,
                                    ENET_PKTSTATE_MODULE_APP,
                                    ENET_PKTSTATE_APP_WITH_DRIVER,
                                    ENET_PKTSTATE_APP_WITH_FREEQ);

            EnetQueue_enq(&gEnetLpbk.txFreePktInfoQ, &pktInfo->node);
            pktInfo = (EnetDma_Pkt *)EnetQueue_deq(&txFreeQ);
        }
    }
    else
    {
        EnetAppUtils_print("retrieveFreeTxPkts() failed to retrieve pkts: %d\n",
                           status);
    }

    return txFreeQCnt;
}

static void EnetLpbk_txTask(void* a0,
                            void* a1)
{
    EnetDma_PktQ txSubmitQ;
    EnetDma_Pkt *pktInfo;
    EthFrame *frame;
    uint32_t txRetrievePktCnt;
    uint32_t loopCnt, pktCnt;
    int32_t status = ENET_SOK;
    uint8_t bcastAddr[ENET_MAC_ADDR_LEN] = {0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU};

    gEnetLpbk.totalTxCnt = 0U;
    for (loopCnt = 0U; loopCnt < ENETLPBK_NUM_ITERATION; loopCnt++)
    {
        pktCnt = 0U;
        while (pktCnt < ENETLPBK_TEST_PKT_NUM)
        {
            /* Transmit a single packet */
            EnetQueue_initQ(&txSubmitQ);

            /* Dequeue one free TX Eth packet */
            pktInfo = (EnetDma_Pkt *)EnetQueue_deq(&gEnetLpbk.txFreePktInfoQ);

            while (NULL != pktInfo)
            {
                pktCnt++;
                /* Fill the TX Eth frame with test content */
                frame = (EthFrame *)pktInfo->bufPtr;
                memcpy(frame->hdr.dstMac, bcastAddr, ENET_MAC_ADDR_LEN);
                memcpy(frame->hdr.srcMac, &gEnetLpbk.hostMacAddr[0U], ENET_MAC_ADDR_LEN);
                frame->hdr.etherType = Enet_htons(ETHERTYPE_EXPERIMENTAL1);
                memset(&frame->payload[0U], (uint8_t)(0xA5 + pktCnt), ENETLPBK_TEST_PKT_LEN);
                pktInfo->userBufLen = ENETLPBK_TEST_PKT_LEN + sizeof(EthFrameHeader);
                pktInfo->appPriv    = &gEnetLpbk;
                EnetDma_checkPktState(&pktInfo->pktState,
                                        ENET_PKTSTATE_MODULE_APP,
                                        ENET_PKTSTATE_APP_WITH_FREEQ,
                                        ENET_PKTSTATE_APP_WITH_DRIVER);

                /* Enqueue the packet for later transmission */
                EnetQueue_enq(&txSubmitQ, &pktInfo->node);

                if (pktCnt >= ENETLPBK_TEST_PKT_NUM)
                {
                    break;
                }

                /* Dequeue one free TX Eth packet */
                pktInfo = (EnetDma_Pkt *)EnetQueue_deq(&gEnetLpbk.txFreePktInfoQ);
            }

            while (0U != EnetQueue_getQCount(&txSubmitQ))
            {
                uint32_t txCnt = EnetQueue_getQCount(&txSubmitQ);
                status = EnetDma_submitTxPktQ(gEnetLpbk.hTxCh,
                                                   &txSubmitQ);
                SemaphoreP_pend(gEnetLpbk.hTxSem, SemaphoreP_WAIT_FOREVER);

                /* Retrieve TX free packets */
                if (status == ENET_SOK)
                {
                    txCnt            = txCnt - EnetQueue_getQCount(&txSubmitQ);
                    txRetrievePktCnt = 0U;
                    while (txRetrievePktCnt != txCnt)
                    {
                        /* This is not failure as HW is busy sending packets, we
                         * need to wait and again call retrieve packets */
                        EnetAppUtils_wait(1);
                        txRetrievePktCnt += EnetLpbk_retrieveFreeTxPkts();
                    }
                }
                else
                {
                    break;
                }
            }
        }

        gEnetLpbk.totalTxCnt += pktCnt;
    }

    EnetAppUtils_print("Transmitted %d packets \n", gEnetLpbk.totalTxCnt);

    SemaphoreP_post(gEnetLpbk.hTxDoneSem);
}

uint32_t EnetLpbk_receivePkts(void)
{
    EnetDma_PktQ rxReadyQ;
    EnetDma_Pkt *pktInfo;
    int32_t status;
    uint32_t rxReadyCnt = 0U;

    EnetQueue_initQ(&rxReadyQ);

    /* Retrieve any CPSW packets which are ready */
    status = EnetDma_retrieveRxPktQ(gEnetLpbk.hRxCh, &rxReadyQ);
    if (status == ENET_SOK)
    {
        rxReadyCnt = EnetQueue_getQCount(&rxReadyQ);

        /* Queue the received packet to rxReadyQ and pass new ones from rxFreeQ
        **/
        pktInfo = (EnetDma_Pkt *)EnetQueue_deq(&rxReadyQ);
        while (pktInfo != NULL)
        {
            EnetDma_checkPktState(&pktInfo->pktState,
                                    ENET_PKTSTATE_MODULE_APP,
                                    ENET_PKTSTATE_APP_WITH_DRIVER,
                                    ENET_PKTSTATE_APP_WITH_READYQ);

            EnetQueue_enq(&gEnetLpbk.rxReadyQ, &pktInfo->node);
            pktInfo = (EnetDma_Pkt *)EnetQueue_deq(&rxReadyQ);
        }
    }
    else
    {
        EnetAppUtils_print("receivePkts() failed to retrieve pkts: %d\n",
                           status);
    }

    return rxReadyCnt;
}

static void EnetLpbk_rxTask(void* a0,
                            void* a1)
{
    EnetDma_Pkt *pktInfo;
    EthFrame *frame;
    uint32_t rxReadyCnt;
    uint32_t loopCnt, loopRxPktCnt;
    int32_t status = ENET_SOK;

    gEnetLpbk.totalRxCnt = 0U;

    for (loopCnt = 0U; loopCnt < ENETLPBK_NUM_ITERATION; loopCnt++)
    {
        loopRxPktCnt = 0U;
        /* Wait for packet reception */
        do
        {
            SemaphoreP_pend(gEnetLpbk.hRxSem, SemaphoreP_WAIT_FOREVER);
            /* Get the packets received so far */
            rxReadyCnt = EnetLpbk_receivePkts();
            if (rxReadyCnt > 0U)
            {
                /* Consume the received packets and release them */
                pktInfo = (EnetDma_Pkt *)EnetQueue_deq(&gEnetLpbk.rxReadyQ);
                while (NULL != pktInfo)
                {
                    EnetDma_checkPktState(&pktInfo->pktState,
                                            ENET_PKTSTATE_MODULE_APP,
                                            ENET_PKTSTATE_APP_WITH_READYQ,
                                            ENET_PKTSTATE_APP_WITH_FREEQ);

                    /* Consume the packet by just printing its content */
                    if (gEnetLpbk.printFrame)
                    {
                        frame = (EthFrame *)pktInfo->bufPtr;

                        EnetAppUtils_printFrame(frame,
                                                pktInfo->userBufLen - sizeof(EthFrameHeader));
                    }

                    /* Release the received packet */
                    EnetQueue_enq(&gEnetLpbk.rxFreeQ, &pktInfo->node);
                    pktInfo = (EnetDma_Pkt *)EnetQueue_deq(&gEnetLpbk.rxReadyQ);
                }

                /*Submit now processed buffers */
                if (status == ENET_SOK)
                {
                    EnetAppUtils_validatePacketState(&gEnetLpbk.rxFreeQ,
                                                     ENET_PKTSTATE_APP_WITH_FREEQ,
                                                     ENET_PKTSTATE_APP_WITH_DRIVER);

                    EnetDma_submitRxPktQ(gEnetLpbk.hRxCh,
                                         &gEnetLpbk.rxFreeQ);
                }
            }

            loopRxPktCnt += rxReadyCnt;
        }
        while (loopRxPktCnt < ENETLPBK_TEST_PKT_NUM);

        gEnetLpbk.totalRxCnt += loopRxPktCnt;
    }

    if (status != ENET_SOK)
    {
        EnetAppUtils_print("Failed to transmit/receive packets: %d, transmitted: %d \n", ENETLPBK_TEST_PKT_NUM, gEnetLpbk.totalRxCnt);
    }
    else
    {
        EnetAppUtils_print("Received %d packets\n", gEnetLpbk.totalRxCnt);
    }

    SemaphoreP_post(gEnetLpbk.hRxDoneSem);
}

static int32_t EnetLpbk_loopbackTest(void)
{
    EnetOsal_Cfg osalCfg;
    EnetUtils_Cfg utilsCfg;
    Enet_IoctlPrms prms;
    SemaphoreP_Params params;
    int32_t status;

    EnetAppUtils_enableClocks(gEnetLpbk.enetType, gEnetLpbk.instId);

    /* Create TX/RX semaphores */
    SemaphoreP_Params_init(&params);
    params.mode = SemaphoreP_Mode_BINARY;

    gEnetLpbk.hRxSem = SemaphoreP_create(0, &params);
    EnetAppUtils_assert(gEnetLpbk.hRxSem != NULL);

    gEnetLpbk.hTxSem = SemaphoreP_create(0, &params);
    EnetAppUtils_assert(gEnetLpbk.hTxSem != NULL);

    gEnetLpbk.hTxDoneSem = SemaphoreP_create(0, &params);
    EnetAppUtils_assert(gEnetLpbk.hTxDoneSem != NULL);

    gEnetLpbk.hRxDoneSem = SemaphoreP_create(0, &params);
    EnetAppUtils_assert(gEnetLpbk.hRxDoneSem != NULL);

    /* Local core id */
    gEnetLpbk.coreId = EnetSoc_getCoreId();

    /* Initialize Enet driver (use default OSAL and utils) */
    Enet_initOsalCfg(&osalCfg);
    Enet_initUtilsCfg(&utilsCfg);
    Enet_init(&osalCfg, &utilsCfg);

    /* Open Enet driver */
    status = EnetLpbk_openEnet();
    if (status != ENET_SOK)
    {
        EnetAppUtils_print("Failed to open Enet driver: %d\n", status);
    }

#if !(defined(SOC_TPR12) || defined(SOC_AWR294X))
    if (status == ENET_SOK)
    {
        /* Attach the core with RM */
        uint32_t coreId;
        EnetPer_AttachCoreOutArgs attachCoreOutArgs;
        coreId = gEnetLpbk.coreId;

        ENET_IOCTL_SET_INOUT_ARGS(&prms, &coreId, &attachCoreOutArgs);
        status = Enet_ioctl(gEnetLpbk.hEnet, gEnetLpbk.coreId, ENET_PER_IOCTL_ATTACH_CORE, &prms);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("EnetLpbk_loopbackTest failed ENET_PER_IOCTL_ATTACH_CORE: %d\n", status);
        }
        else
        {
            gEnetLpbk.coreKey = attachCoreOutArgs.coreKey;
        }
    }
#endif

    if (status == ENET_SOK)
    {
        /* memutils open should happen after Cpsw is opened as it uses CpswUtils_Q
         * functions */
        status = EnetMem_init();
        EnetAppUtils_assert(ENET_SOK == status);
    }

    /* Open DMA driver */
    if (status == ENET_SOK)
    {
        status = EnetLpbk_openDma();
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("Failed to open DMA: %d\n", status);
        }
    }

    /* Enable host port */
    if (status == ENET_SOK)
    {
        if (Enet_isCpswFamily(gEnetLpbk.enetType))
        {
            status = EnetLpbk_setupCpswAle();
            if (status != ENET_SOK)
            {
                EnetAppUtils_print("Failed to setup CPSW ALE: %d\n", status);
            }
        }

        if (status == ENET_SOK)
        {
            ENET_IOCTL_SET_NO_ARGS(&prms);
            status = Enet_ioctl(gEnetLpbk.hEnet, gEnetLpbk.coreId, ENET_HOSTPORT_IOCTL_ENABLE, &prms);
            if (status != ENET_SOK)
            {
                EnetAppUtils_print("Failed to enable host port: %d\n", status);
            }
        }
    }

    /* Show alive PHYs */
    if (status == ENET_SOK)
    {
        status = EnetLpbk_showAlivePhys();
    }

    /* Start timer */
    ClockP_start(gEnetLpbk.hTickTimer);

    /* Wait for link up */
    if ((status == ENET_SOK) && gEnetLpbk.testPhyLoopback)
    {
        status = EnetLpbk_waitForLinkUp();
    }

    /* Do packet transmission and reception */
    if (status == ENET_SOK)
    {
        EnetLpbk_createRxTxTasks();

        SemaphoreP_pend(gEnetLpbk.hTxDoneSem, SemaphoreP_WAIT_FOREVER);
        SemaphoreP_pend(gEnetLpbk.hRxDoneSem, SemaphoreP_WAIT_FOREVER);
    }

    /* Print network statistics */
    if (status == ENET_SOK)
    {
        if (Enet_isCpswFamily(gEnetLpbk.enetType))
        {
            EnetLpbk_showCpswStats();
        }
    }

    /* Disable host port */
    if (status == ENET_SOK)
    {
        ENET_IOCTL_SET_NO_ARGS(&prms);
        status = Enet_ioctl(gEnetLpbk.hEnet, gEnetLpbk.coreId, ENET_HOSTPORT_IOCTL_DISABLE, &prms);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("Failed to disable host port: %d\n", status);
        }
    }

    /* Stop periodic tick timer */
    ClockP_stop(gEnetLpbk.hTickTimer);

    /* Print DMA statistics */
    if (status == ENET_SOK)
    {
        EnetAppUtils_showRxChStats(gEnetLpbk.hRxCh);
        EnetAppUtils_showTxChStats(gEnetLpbk.hTxCh);
    }

    /* Show stack usage statistics */
    if (status == ENET_SOK)
    {
        EnetAppUtils_printTaskStackUsage();
    }

    /* Close Enet DMA driver */
    EnetLpbk_closeDma();

    /* Close Enet driver */
    EnetLpbk_closeEnet();

    /* Disable peripheral clocks */
    EnetAppUtils_disableClocks(gEnetLpbk.enetType, gEnetLpbk.instId);

    /* Delete RX and TX tasks */
    EnetLpbk_deleteRxTxTasks();

    /* Deinit Enet driver */
    Enet_deinit();

    /* Delete all TX/RX semaphores */
    SemaphoreP_delete(gEnetLpbk.hRxDoneSem);
    gEnetLpbk.hRxDoneSem = NULL;
    SemaphoreP_delete(gEnetLpbk.hTxDoneSem);
    gEnetLpbk.hTxDoneSem = NULL;
    SemaphoreP_delete(gEnetLpbk.hTxSem);
    gEnetLpbk.hTxSem = NULL;
    SemaphoreP_delete(gEnetLpbk.hRxSem);
    gEnetLpbk.hRxSem = NULL;

    EnetAppUtils_print("Test complete: %s\n", (status == ENET_SOK) ? "PASS" : "FAIL");

    return status;
}

static void EnetLpbk_initCpswCfg(Cpsw_Cfg *cpswCfg)
{
    CpswHostPort_Cfg *hostPortCfg = &cpswCfg->hostPortCfg;
    CpswAle_Cfg *aleCfg = &cpswCfg->aleCfg;
    CpswCpts_Cfg *cptsCfg = &cpswCfg->cptsCfg;

    /* Set initial config */
    Enet_initCfg(gEnetLpbk.enetType, gEnetLpbk.instId, cpswCfg, sizeof(*cpswCfg));

    /* Peripheral config */
    cpswCfg->vlanCfg.vlanAware = false;

    /* Host port config */
    hostPortCfg->removeCrc      = true;
    hostPortCfg->padShortPacket = true;
    hostPortCfg->passCrcErrors  = true;

    /* ALE config */
    aleCfg->modeFlags                          = CPSW_ALE_CFG_MODULE_EN;
    aleCfg->agingCfg.autoAgingEn               = true;
    aleCfg->agingCfg.agingPeriodInMs           = 1000;
    aleCfg->nwSecCfg.vid0ModeEn                = true;
    aleCfg->vlanCfg.aleVlanAwareMode           = false;
    aleCfg->vlanCfg.cpswVlanAwareMode          = false;
    aleCfg->vlanCfg.unknownUnregMcastFloodMask = CPSW_ALE_ALL_PORTS_MASK;
    aleCfg->vlanCfg.unknownRegMcastFloodMask   = CPSW_ALE_ALL_PORTS_MASK;
    aleCfg->vlanCfg.unknownVlanMemberListMask  = CPSW_ALE_ALL_PORTS_MASK;

    /* CPTS config */
    /* Note: Timestamping and MAC loopback are not supported together because of
     * IP limitation, so disabling timestamping for this application */
    cptsCfg->hostRxTsEn = false;

    EnetAppUtils_initResourceConfig(gEnetLpbk.enetType, gEnetLpbk.coreId, &cpswCfg->resCfg);
}

static int32_t EnetLpbk_setupCpswAle(void)
{
    Enet_IoctlPrms prms;
    CpswAle_SetPortStateInArgs setPortStateInArgs;
    CpswAle_SetUcastEntryInArgs setUcastInArgs;
    uint32_t entryIdx;
    int32_t status;

    /* ALE entry with "secure" bit cleared is required for loopback */
    setUcastInArgs.addr.vlanId  = 0U;
    setUcastInArgs.info.portNum = CPSW_ALE_HOST_PORT_NUM;
    setUcastInArgs.info.blocked = false;
    setUcastInArgs.info.secure  = false;
    setUcastInArgs.info.super   = false;
    setUcastInArgs.info.ageable = false;
    setUcastInArgs.info.trunk   = false;
    EnetUtils_copyMacAddr(&setUcastInArgs.addr.addr[0U], gEnetLpbk.hostMacAddr);
    ENET_IOCTL_SET_INOUT_ARGS(&prms, &setUcastInArgs, &entryIdx);

    status = Enet_ioctl(gEnetLpbk.hEnet, gEnetLpbk.coreId, CPSW_ALE_IOCTL_ADD_UCAST, &prms);
    if (status != ENET_SOK)
    {
        EnetAppUtils_print("Failed to add ucast entry: %d\n", status);
    }

    /* Set host port to 'forwarding' state */
    if (status == ENET_SOK)
    {
        setPortStateInArgs.portNum   = CPSW_ALE_HOST_PORT_NUM;
        setPortStateInArgs.portState = CPSW_ALE_PORTSTATE_FORWARD;
        ENET_IOCTL_SET_IN_ARGS(&prms, &setPortStateInArgs);

        status = Enet_ioctl(gEnetLpbk.hEnet, gEnetLpbk.coreId, CPSW_ALE_IOCTL_SET_PORT_STATE, &prms);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("Failed to set ALE port state: %d\n", status);
        }
    }

    return status;
}

static int32_t EnetLpbk_openEnet(void)
{
    Cpsw_Cfg cpswCfg;
#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
    EnetUdma_Cfg dmaCfg;
#elif defined (ENET_SOC_HOSTPORT_DMA_TYPE_CPDMA)
    EnetCpdma_Cfg dmaCfg;
#endif
    Enet_IoctlPrms prms;
    EnetPer_PortLinkCfg portLinkCfg;
    CpswMacPort_Cfg macCfg;
    int32_t status = ENET_SOK;

    cpswCfg.dmaCfg = &dmaCfg;

    /* Initialize peripheral config */
    EnetLpbk_initCpswCfg(&cpswCfg);

    if (gEnetLpbk.enetType == ENET_CPSW_9G)
    {
        EnetAppUtils_print("CPSW_9G Test on MAIN NAVSS\n");
    }
    else if (gEnetLpbk.enetType == ENET_CPSW_5G)
    {
        EnetAppUtils_print("CPSW_5G Test on MAIN NAVSS\n");
    }
    else if (gEnetLpbk.enetType == ENET_CPSW_2G)
    {
        EnetAppUtils_print("CPSW_2G Test on MCU NAVSS\n");
    }

#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
    dmaCfg.rxChInitPrms.dmaPriority = UDMA_DEFAULT_RX_CH_DMA_PRIORITY;

    /* App should open UDMA first as UDMA handle is needed to initialize
     * CPSW RX channel */
    gEnetLpbk.hUdmaDrv = EnetAppUtils_udmaOpen(gEnetLpbk.enetType, NULL);
    EnetAppUtils_assert(NULL != gEnetLpbk.hUdmaDrv);

    dmaCfg.hUdmaDrv = gEnetLpbk.hUdmaDrv;
#endif

    /* Set Enet global runtime log level */
    Enet_setTraceLevel(ENET_TRACE_DEBUG);

    /* Open the Enet driver */
    gEnetLpbk.hEnet = Enet_open(gEnetLpbk.enetType, gEnetLpbk.instId, &cpswCfg, sizeof(cpswCfg));
    if (gEnetLpbk.hEnet == NULL)
    {
        EnetAppUtils_print("Failed to open Enet driver\n");
        status = ENET_EFAIL;
    }

    /* Setup port link open parameters */
    if (status == ENET_SOK)
    {
        EnetBoard_EthPort ethPort;
        EnetMacPort_LinkCfg *linkCfg = &portLinkCfg.linkCfg;
        EnetMacPort_Interface *mii = &portLinkCfg.mii;
        EnetPhy_Cfg *phyCfg = &portLinkCfg.phyCfg;
        EnetPhy_Mii phyMii;

        /* Setup board for requested Ethernet port */
        ethPort.enetType = gEnetLpbk.enetType;
        ethPort.instId   = gEnetLpbk.instId;
        ethPort.macPort  = gEnetLpbk.macPort;
        ethPort.boardId  = gEnetLpbk.boardId;
        EnetLpbk_macMode2MacMii(gEnetLpbk.macMode, &ethPort.mii);

        status = EnetBoard_setupPorts(&ethPort, 1U);
        EnetAppUtils_assert(status == ENET_SOK);

        /* Set port link params */
        portLinkCfg.macPort = gEnetLpbk.macPort;
        portLinkCfg.macCfg = &macCfg;

        CpswMacPort_initCfg(&macCfg);
        EnetLpbk_macMode2MacMii(gEnetLpbk.macMode, mii);

        if (gEnetLpbk.testPhyLoopback)
        {
            const EnetBoard_PhyCfg *boardPhyCfg = NULL;

            /* Set PHY configuration params */
            EnetPhy_initCfg(phyCfg);
            status = EnetLpbk_macMode2PhyMii(gEnetLpbk.macMode, &phyMii);

            if (status == ENET_SOK)
            {
                boardPhyCfg = EnetBoard_getPhyCfg(&ethPort);
                if (boardPhyCfg != NULL)
                {
                    phyCfg->phyAddr     = boardPhyCfg->phyAddr;
                    phyCfg->isStrapped  = boardPhyCfg->isStrapped;
                    phyCfg->skipExtendedCfg = boardPhyCfg->skipExtendedCfg;
                    phyCfg->extendedCfgSize = boardPhyCfg->extendedCfgSize;
                    memcpy(phyCfg->extendedCfg, boardPhyCfg->extendedCfg, phyCfg->extendedCfgSize);
                }
                else
                {
                    EnetAppUtils_print("PHY info not found\n");
                    EnetAppUtils_assert(false);
                }

                if ((phyMii == ENETPHY_MAC_MII_MII) ||
                    (phyMii == ENETPHY_MAC_MII_RMII))
                {
                    linkCfg->speed = ENET_SPEED_100MBIT;
                }
                else
                {
                    /* TODO: TPR12 always 100 Mbits */
                    #if defined(SOC_TPR12) || defined(SOC_AWR294X)
                    linkCfg->speed = ENET_SPEED_100MBIT;
                    #else
                    linkCfg->speed = ENET_SPEED_1GBIT;
                    #endif
                }

                linkCfg->duplexity = ENET_DUPLEX_FULL;
            }
        }
        else
        {
            phyCfg->phyAddr = ENETPHY_INVALID_PHYADDR;

            if (mii->layerType == ENET_MAC_LAYER_MII)
            {
                linkCfg->speed = ENET_SPEED_100MBIT;
            }
            else
            {
                linkCfg->speed = ENET_SPEED_1GBIT;
            }

            linkCfg->duplexity = ENET_DUPLEX_FULL;

#if defined(SOC_J721E) || defined(SOC_J7200)
            if (gEnetLpbk.macMode == SGMII)
            {
                macCfg.sgmiiMode = ENET_MAC_SGMIIMODE_SGMII_FORCEDLINK;
            }
#endif
        }

        /* MAC and PHY loopbacks are mutually exclusive */
        phyCfg->loopbackEn = gEnetLpbk.testPhyLoopback && !gEnetLpbk.testExtLoopback;
        macCfg.loopbackEn = !gEnetLpbk.testPhyLoopback;


#if (defined(SOC_J721E) || defined(SOC_J7200)) && defined(BUILD_MCU2_0)
        /* CPSW5G/CPSW9G supported only on MCU2_0 */
        /* Overwrite the ENET control set by board lib */
        if (status == ENET_SOK)
        {
            EnetBoard_setEnetControl(gEnetLpbk.enetType, gEnetLpbk.instId, gEnetLpbk.macPort, gEnetLpbk.macMode);
        }
#endif
    }

    /* Open port link */
    if (status == ENET_SOK)
    {
        ENET_IOCTL_SET_IN_ARGS(&prms, &portLinkCfg);

        status = Enet_ioctl(gEnetLpbk.hEnet, gEnetLpbk.coreId, ENET_PER_IOCTL_OPEN_PORT_LINK, &prms);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("Failed to open port link: %d\n", status);
        }
    }

    return status;
}

static void EnetLpbk_closeEnet(void)
{
    Enet_IoctlPrms prms;
    int32_t status;

    /* Close port link */
    ENET_IOCTL_SET_IN_ARGS(&prms, &gEnetLpbk.macPort);

    status = Enet_ioctl(gEnetLpbk.hEnet, gEnetLpbk.coreId, ENET_PER_IOCTL_CLOSE_PORT_LINK, &prms);
    if (status != ENET_SOK)
    {
        EnetAppUtils_print("Failed to close port link: %d\n", status);
    }

#if !(defined(SOC_TPR12) || defined(SOC_AWR294X))
    /* Detach core */
    if (status == ENET_SOK)
    {
        ENET_IOCTL_SET_IN_ARGS(&prms, &gEnetLpbk.coreKey);

        status = Enet_ioctl(gEnetLpbk.hEnet, gEnetLpbk.coreId, ENET_PER_IOCTL_DETACH_CORE, &prms);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("Failed to detach core key %u: %d\n", gEnetLpbk.coreKey, status);
        }
    }
#endif

    /* Close Enet driver */
    Enet_close(gEnetLpbk.hEnet);

#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
    /* Close UDMA */
    EnetAppUtils_udmaclose(gEnetLpbk.hUdmaDrv);
#endif
    gEnetLpbk.hEnet = NULL;
}

static int32_t EnetLpbk_showAlivePhys(void)
{
    Enet_IoctlPrms prms;
    bool alive = false;
    int8_t i;
    int32_t status;

    for (i = 0U; i < ENET_MDIO_PHY_CNT_MAX; i++)
    {
        ENET_IOCTL_SET_INOUT_ARGS(&prms, &i, &alive);

        status = Enet_ioctl(gEnetLpbk.hEnet, gEnetLpbk.coreId, ENET_MDIO_IOCTL_IS_ALIVE, &prms);
        if (status == ENET_SOK)
        {
            if (alive == true)
            {
                EnetAppUtils_print("PHY %u is alive\n", i);
            }
        }
        else
        {
            EnetAppUtils_print("Failed to get PHY %u alive status: %d\n", i, status);
        }
    }

    return status;
}

static int32_t EnetLpbk_waitForLinkUp(void)
{
    Enet_IoctlPrms prms;
    bool linked = false;
    int32_t status = ENET_SOK;

    ENET_IOCTL_SET_INOUT_ARGS(&prms, &gEnetLpbk.macPort, &linked);

    while (!linked)
    {
        status = Enet_ioctl(gEnetLpbk.hEnet, gEnetLpbk.coreId, ENET_PER_IOCTL_IS_PORT_LINK_UP, &prms);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("Failed to get port %u's link status: %d\n",
                            ENET_MACPORT_ID(gEnetLpbk.macPort), status);
            linked = false;
            break;
        }

        if (!linked)
        {
            EnetUtils_delay(10U);
        }
    }

    return status;
}

static void EnetLpbk_showCpswStats(void)
{
    Enet_IoctlPrms prms;
    CpswStats_PortStats portStats;
    int32_t status;

    /* Show host port statistics */
    ENET_IOCTL_SET_OUT_ARGS(&prms, &portStats);
    status = Enet_ioctl(gEnetLpbk.hEnet, gEnetLpbk.coreId, ENET_STATS_IOCTL_GET_HOSTPORT_STATS, &prms);
    if (status == ENET_SOK)
    {
        EnetAppUtils_print("\n Port 0 Statistics\n");
        EnetAppUtils_print("-----------------------------------------\n");
        EnetAppUtils_printHostPortStats2G((CpswStats_HostPort_2g *)&portStats);
        EnetAppUtils_print("\n");
    }
    else
    {
        EnetAppUtils_print("Failed to get host stats: %d\n", status);
    }

    /* Show MAC port statistics */
    if (status == ENET_SOK)
    {
        ENET_IOCTL_SET_INOUT_ARGS(&prms, &gEnetLpbk.macPort, &portStats);
        status = Enet_ioctl(gEnetLpbk.hEnet, gEnetLpbk.coreId, ENET_STATS_IOCTL_GET_MACPORT_STATS, &prms);
        if (status == ENET_SOK)
        {
            EnetAppUtils_print("\n Port 1 Statistics\n");
            EnetAppUtils_print("-----------------------------------------\n");
            EnetAppUtils_printMacPortStats2G((CpswStats_MacPort_2g *)&portStats);
            EnetAppUtils_print("\n");
        }
        else
        {
            EnetAppUtils_print("Failed to get MAC stats: %d\n", status);
        }
    }
}

#if 0 //TODO - NEED TO BE PORTED
static uint32_t EnetLpbk_getSystemHeapFreeSpace(void)
{
    Memory_Stats stats;
    HeapMem_Object *obj = NULL;
    HeapMem_Object *nextObj = NULL;
    HeapMem_Object *currObj = NULL;
    uint32_t totalFreeSize = 0U;
    uint32_t totalFreeSizeStatic = 0U;
    uint32_t totalFreeSizeDynamic = 0U;
    uint32_t i;

    for (i = 0U; i < HeapMem_Object_count(); i++)
    {
        obj = HeapMem_Object_get(NULL, i);
        if (NULL != obj)
        {
            HeapMem_getStats(obj, &stats);
            totalFreeSizeStatic += stats.totalFreeSize;
        }
    }

    nextObj = HeapMem_Object_first();
    do
    {
        currObj = nextObj;
        if (NULL != currObj)
        {
            HeapMem_getStats(currObj, &stats);
            totalFreeSizeDynamic += stats.totalFreeSize;

            nextObj = HeapMem_Object_next(currObj);
        }
    }
    while (nextObj != NULL);

    totalFreeSize = totalFreeSizeStatic + totalFreeSizeDynamic;

    return totalFreeSize;
}
#endif

#if (0U == APP_ENABLE_STATIC_CFG)
static void EnetLpbk_showMenu(void)
{
    int32_t status;

    do
    {
        EnetLpbk_showEnetTypeMenu(&gEnetLpbk.enetType,
                                  &gEnetLpbk.instId);

        EnetLpbk_showLpbkMenu(&gEnetLpbk.testPhyLoopback);

        status = EnetLpbk_showPortMenu(gEnetLpbk.enetType,
                                       gEnetLpbk.instId,
                                       gEnetLpbk.testPhyLoopback,
                                       &gEnetLpbk.macPort,
                                       &gEnetLpbk.macMode,
                                       &gEnetLpbk.boardId);

        if (status != ENET_SOK)
        {
            EnetAppUtils_print("Try again...\n\n");
        }
    }
    while (status != ENET_SOK);
}

static void EnetLpbk_showEnetTypeMenu(Enet_Type *enetType,
                                      uint32_t *instId)
{
    bool retry;
    int32_t choice;
    uint32_t i;

    do
    {
        EnetAppUtils_print("Select Enet peripheral type:\n");
        for (i = 0U; i < ENET_ARRAYSIZE(gEnetLpbk_EnetTypeMenu); i++)
        {
            EnetAppUtils_print("%u: %s\n", i, gEnetLpbk_EnetTypeMenu[i].text);
        }

        choice = EnetAppUtils_getNum();

        if ((choice >= 0) &&
            (choice < ENET_ARRAYSIZE(gEnetLpbk_EnetTypeMenu)))
        {
            *enetType = gEnetLpbk_EnetTypeMenu[choice].enetType;
            *instId = gEnetLpbk_EnetTypeMenu[choice].instId;
            retry = false;
        }
        else
        {
            EnetAppUtils_print("Wrong option, try again...\n\n");
            retry = true;
        }
    }
    while (retry);

}

static void EnetLpbk_showLpbkMenu(bool *testPhyLoopback)
{
    bool retry;
    int32_t choice;

    do
    {
        EnetAppUtils_print("Select loopback type:\n");
        EnetAppUtils_print("0: Internal (MAC loopback)\n");
        EnetAppUtils_print("1: External (PHY loopback)\n");
        EnetAppUtils_print("2: External (Ext loopback)\n");

        choice = EnetAppUtils_getNum();

        switch (choice)
        {
            case 0:
                *testPhyLoopback = false;
                retry = false;
                break;
            case 1:
                *testPhyLoopback = true;
        gEnetLpbk.testExtLoopback = false;
                retry = false;
                break;
            case 2:
                *testPhyLoopback = true;
        /* TODO */
        gEnetLpbk.testExtLoopback = true;
                retry = false;
                break;
            default:
                EnetAppUtils_print("Wrong option, try again...\n\n");
                retry = true;
                break;
        }

    }
    while (retry);
}

static int32_t EnetLpbk_showPortMenu(Enet_Type enetType,
                                     uint32_t instId,
                                     bool testPhyLoopback,
                                     Enet_MacPort *macPort,
                                     emac_mode *macMode,
                                     uint32_t *boardId)
{
    EnetLpbk_PortMenu *portMenu = NULL;
    uint32_t portMenuLen = 0U;
    uint32_t i;
    int32_t choice;
    bool retry;
    int32_t status = ENET_SOK;

    if (testPhyLoopback)
    {
        if (enetType == ENET_CPSW_2G)
        {
            portMenu = gEnetLpbk_cpsw2gPhyLpbkMenu;
            portMenuLen = ENET_ARRAYSIZE(gEnetLpbk_cpsw2gPhyLpbkMenu);
        }
        else if (enetType == ENET_CPSW_5G)
        {
            portMenu = gEnetLpbk_cpsw5gPhyLpbkMenu;
            portMenuLen = ENET_ARRAYSIZE(gEnetLpbk_cpsw5gPhyLpbkMenu);
        }
        else if (enetType == ENET_CPSW_9G)
        {
            portMenu = gEnetLpbk_cpsw9gPhyLpbkMenu;
            portMenuLen = ENET_ARRAYSIZE(gEnetLpbk_cpsw9gPhyLpbkMenu);
        }
    }
    else
    {
        portMenu = gEnetLpbk_MacLpbkMenu;
        portMenuLen = ENET_ARRAYSIZE(gEnetLpbk_MacLpbkMenu);
    }

    if ((portMenuLen == 0U) && (portMenu == NULL))
    {
        EnetAppUtils_print("Ethernet periperhal not supported\n");
        status = ENET_EINVALIDPARAMS;
    }
    else if ((portMenuLen == 0U) && (portMenu != NULL))
    {
        EnetAppUtils_print("No ports supported on current core\n");
        status = ENET_EINVALIDPARAMS;
    }
    else
    {
        do
        {
            EnetAppUtils_print("Select MAC port:\n");
            for (i = 0U; i < portMenuLen; i++)
            {
                EnetAppUtils_print("%u: %s\n", i, portMenu[i].text);
            }

            choice = EnetAppUtils_getNum();

            if ((choice >= 0) &&
                (choice < portMenuLen))
            {
                *macPort = portMenu[choice].macPort;
                *macMode = portMenu[choice].macMode;
                *boardId = portMenu[choice].boardId;
                retry = false;
            }
            else
            {
                EnetAppUtils_print("Wrong option, try again...\n\n");
                retry = true;
            }
        }
        while (retry);
    }

    return status;
}
#endif

void EnetLpbk_waitForDebugger(void)
{
    /* Set ccsHaltFlag to 1 for halting core for CCS connection */
    volatile bool ccsHalt = true;

    while (ccsHalt)
    {
        /* Do nothing */
    }
}

static int32_t EnetLpbk_macMode2PhyMii(emac_mode macMode,
                                    EnetPhy_Mii *mii)
{
    int32_t status = ENET_SOK;

    switch (macMode)
    {
#if defined(SOC_AM65XX) || defined(SOC_J7200) || defined(SOC_J721E) || defined(SOC_TPR12) || defined(SOC_AWR294X)
        case RMII:
            *mii = ENETPHY_MAC_MII_RMII;
            break;

        case RGMII:
            *mii = ENETPHY_MAC_MII_RGMII;
            break;
#endif
#if defined(SOC_J7200) || defined(SOC_J721E)
        case GMII:
            *mii = ENETPHY_MAC_MII_GMII;
            break;

        case SGMII:
            *mii = ENETPHY_MAC_MII_SGMII;
            break;

        case QSGMII:
        case QSGMII_SUB:
            *mii = ENETPHY_MAC_MII_QSGMII;
            break;

        case XFI:
            status = ENET_EFAIL;
            EnetAppUtils_print("XFI is not supported by EnetPhy driver\n");
            EnetAppUtils_assert(false);
            break;
#endif
        default:
            status = ENET_EFAIL;
            EnetAppUtils_print("Invalid MAC mode: %u\n", macMode);
            EnetAppUtils_assert(false);
            break;
    }

    return status;
}

static void EnetLpbk_macMode2MacMii(emac_mode macMode,
                                    EnetMacPort_Interface *mii)
{
    switch (macMode)
    {
#if defined(SOC_AM65XX) || defined(SOC_J7200) || defined(SOC_J721E) || defined(SOC_TPR12) || defined(SOC_AWR294X)
        case RMII:
            mii->layerType    = ENET_MAC_LAYER_MII;
            mii->sublayerType = ENET_MAC_SUBLAYER_REDUCED;
            mii->variantType  = ENET_MAC_VARIANT_NONE;
            break;

        case RGMII:
            mii->layerType    = ENET_MAC_LAYER_GMII;
            mii->sublayerType = ENET_MAC_SUBLAYER_REDUCED;
            mii->variantType  = ENET_MAC_VARIANT_FORCED;
            break;
#endif
#if defined(SOC_J7200) || defined(SOC_J721E)
        case GMII:
            mii->layerType    = ENET_MAC_LAYER_GMII;
            mii->sublayerType = ENET_MAC_SUBLAYER_STANDARD;
            mii->variantType  = ENET_MAC_VARIANT_NONE;
            break;

        case SGMII:
            mii->layerType    = ENET_MAC_LAYER_GMII;
            mii->sublayerType = ENET_MAC_SUBLAYER_SERIAL;
            mii->variantType  = ENET_MAC_VARIANT_NONE;
            break;

        case QSGMII:
            mii->layerType    = ENET_MAC_LAYER_GMII;
            mii->sublayerType = ENET_MAC_SUBLAYER_QUAD_SERIAL_MAIN;
            mii->variantType  = ENET_MAC_VARIANT_NONE;
            break;

        case QSGMII_SUB:
            mii->layerType    = ENET_MAC_LAYER_GMII;
            mii->sublayerType = ENET_MAC_SUBLAYER_QUAD_SERIAL_SUB;
            mii->variantType  = ENET_MAC_VARIANT_NONE;
            break;

        case XFI:
            mii->layerType    = ENET_MAC_LAYER_XGMII;
            mii->sublayerType = ENET_MAC_SUBLAYER_STANDARD;
            mii->variantType  = ENET_MAC_VARIANT_NONE;
            break;
#endif
        default:
            EnetAppUtils_print("Invalid MAC mode: %u\n", macMode);
            EnetAppUtils_assert(false);
            break;
    }
}

void EnetLpbk_rxIsrFxn(void *appData)
{
    SemaphoreP_post(gEnetLpbk.hRxSem);
}

void EnetLpbk_txIsrFxn(void *appData)
{
    SemaphoreP_post(gEnetLpbk.hTxSem);
}

void EnetLpbk_initTxFreePktQ(void)
{
    EnetDma_Pkt *pPktInfo;
    uint32_t i;

    /* Initialize all queues */
    EnetQueue_initQ(&gEnetLpbk.txFreePktInfoQ);

    /* Initialize TX EthPkts and queue them to txFreePktInfoQ */
    for (i = 0U; i < ENET_MEM_NUM_TX_PKTS; i++)
    {
        pPktInfo = EnetMem_allocEthPkt(&gEnetLpbk,
                                                  ENET_MEM_LARGE_POOL_PKT_SIZE,
                                                  ENETDMA_CACHELINE_ALIGNMENT);
        EnetAppUtils_assert(pPktInfo != NULL);
        ENET_UTILS_SET_PKT_APP_STATE(&pPktInfo->pktState, ENET_PKTSTATE_APP_WITH_FREEQ);

        EnetQueue_enq(&gEnetLpbk.txFreePktInfoQ, &pPktInfo->node);
    }

    EnetAppUtils_print("initQs() txFreePktInfoQ initialized with %d pkts\n",
                       EnetQueue_getQCount(&gEnetLpbk.txFreePktInfoQ));
}

void EnetLpbk_initRxReadyPktQ(void)
{
    EnetDma_PktQ rxReadyQ;
    EnetDma_Pkt *pPktInfo;
    int32_t status;
    uint32_t i;

    EnetQueue_initQ(&gEnetLpbk.rxFreeQ);
    EnetQueue_initQ(&gEnetLpbk.rxReadyQ);
    EnetQueue_initQ(&rxReadyQ);

    for (i = 0U; i < ENET_MEM_NUM_RX_PKTS; i++)
    {
        pPktInfo = EnetMem_allocEthPkt(&gEnetLpbk,
                                                  ENET_MEM_LARGE_POOL_PKT_SIZE,
                                                  ENETDMA_CACHELINE_ALIGNMENT);
        EnetAppUtils_assert(pPktInfo != NULL);
        ENET_UTILS_SET_PKT_APP_STATE(&pPktInfo->pktState, ENET_PKTSTATE_APP_WITH_FREEQ);
        EnetQueue_enq(&gEnetLpbk.rxFreeQ, &pPktInfo->node);
    }

    /* Retrieve any CPSW packets which are ready */
    status = EnetDma_retrieveRxPktQ(gEnetLpbk.hRxCh, &rxReadyQ);
    EnetAppUtils_assert(status == ENET_SOK);
    /* There should not be any packet with DMA during init */
    EnetAppUtils_assert(EnetQueue_getQCount(&rxReadyQ) == 0U);

    EnetAppUtils_validatePacketState(&gEnetLpbk.rxFreeQ,
                                     ENET_PKTSTATE_APP_WITH_FREEQ,
                                     ENET_PKTSTATE_APP_WITH_DRIVER);

    EnetDma_submitRxPktQ(gEnetLpbk.hRxCh,
                         &gEnetLpbk.rxFreeQ);

    /* Assert here as during init no. of DMA descriptors should be equal to
     * no. of free Ethernet buffers available with app */

    EnetAppUtils_assert(0U == EnetQueue_getQCount(&gEnetLpbk.rxFreeQ));
}

int32_t EnetLpbk_openDma()
{
    int32_t status = ENET_SOK;
#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
    EnetUdma_OpenRxFlowPrms rxChCfg;
    EnetUdma_OpenTxChPrms   txChCfg;
#elif defined (ENET_SOC_HOSTPORT_DMA_TYPE_CPDMA)
    EnetCpdma_OpenRxChPrms rxChCfg;
    EnetCpdma_OpenTxChPrms txChCfg;
#endif

    /* Open the CPSW TX channel  */
    if (status == ENET_SOK)
    {
        EnetDma_initTxChParams(&txChCfg);

#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
        txChCfg.hUdmaDrv = gEnetLpbk.hUdmaDrv;
#endif
        txChCfg.cbArg   = &gEnetLpbk;
        txChCfg.notifyCb = EnetLpbk_txIsrFxn;

        EnetAppUtils_setCommonTxChPrms(&txChCfg);

        EnetAppUtils_openTxCh(gEnetLpbk.hEnet,
                              gEnetLpbk.coreKey,
                              gEnetLpbk.coreId,
                              &gEnetLpbk.txChNum,
                              &gEnetLpbk.hTxCh,
                              &txChCfg);

        EnetLpbk_initTxFreePktQ();

        if (NULL != gEnetLpbk.hTxCh)
        {
            #if !(defined(SOC_TPR12) || defined(SOC_AWR294X))
            status = EnetDma_enableTxEvent(gEnetLpbk.hTxCh);
            #else
            status = ENET_SOK;
            #endif
            if (ENET_SOK != status)
            {
#if FIX_RM
                /* Free the Ch Num if enable event failed */
                EnetAppUtils_freeTxCh(gEnetLpbk.hEnet,
                                      gEnetLpbk.coreKey,
                                      gEnetLpbk.coreId,
                                      gEnetLpbk.txChNum);
#endif
                EnetAppUtils_print("EnetUdma_startTxCh() failed: %d\n", status);
                status = ENET_EFAIL;
            }
        }
        else
        {
#if FIX_RM
            /* Free the Ch Num if open Tx Ch failed */
            EnetAppUtils_freeTxCh(gEnetLpbk.hEnet,
                                  gEnetLpbk.coreKey,
                                  gEnetLpbk.coreId,
                                  gEnetLpbk.txChNum);
#endif
            EnetAppUtils_print("EnetDma_openTxCh() failed to open: %d\n",
                               status);
            status = ENET_EFAIL;
        }
    }

    /* Open the CPSW RX flow  */
    if (status == ENET_SOK)
    {
        EnetDma_initRxChParams(&rxChCfg);

#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
        rxChCfg.hUdmaDrv = gEnetLpbk.hUdmaDrv;
#endif

        rxChCfg.notifyCb = EnetLpbk_rxIsrFxn;
        rxChCfg.cbArg   = &gEnetLpbk;

#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
        EnetAppUtils_setCommonRxFlowPrms(&rxChCfg);
        EnetAppUtils_openRxFlow(gEnetLpbk.enetType,
                                gEnetLpbk.hEnet,
                                gEnetLpbk.coreKey,
                                gEnetLpbk.coreId,
                                true,
                                &gEnetLpbk.rxStartFlowIdx,
                                &gEnetLpbk.rxFlowIdx,
                                &gEnetLpbk.hostMacAddr[0U],
                                &gEnetLpbk.hRxCh,
                                &rxChCfg);
#elif defined (ENET_SOC_HOSTPORT_DMA_TYPE_CPDMA)
        EnetAppUtils_setCommonRxChPrms(&rxChCfg);
        EnetAppUtils_openRxCh(gEnetLpbk.hEnet,
                              gEnetLpbk.coreKey,
                              gEnetLpbk.coreId,
                              &gEnetLpbk.rxChNum,
                              &gEnetLpbk.hRxCh,
                              &rxChCfg);
#endif

        if (NULL == gEnetLpbk.hRxCh)
        {
            EnetAppUtils_print("EnetDma_openRxCh() failed to open: %d\n",
                               status);
            EnetAppUtils_assert(NULL != gEnetLpbk.hRxCh);
        }
        else
        {
#if defined (SOC_TPR12) || defined (SOC_AWR294X)
            //FIXME - TPR12 openRxCh doesn't allocate MAC addresses which seems correct behaviour
            status = EnetAppUtils_allocMac(gEnetLpbk.hEnet,
                                           gEnetLpbk.coreKey,
                                           gEnetLpbk.coreId,
                                           gEnetLpbk.hostMacAddr);
#endif
            EnetAppUtils_print("Host MAC address: ");
            EnetAppUtils_printMacAddr(gEnetLpbk.hostMacAddr);
            /* Submit all ready RX buffers to DMA.*/
            EnetLpbk_initRxReadyPktQ();
        }
    }

    return status;
}

void EnetLpbk_closeDma()
{
    EnetDma_PktQ fqPktInfoQ;
    EnetDma_PktQ cqPktInfoQ;

    EnetQueue_initQ(&fqPktInfoQ);
    EnetQueue_initQ(&cqPktInfoQ);

    /* There should not be any ready packet */
    EnetAppUtils_assert(0U == EnetQueue_getQCount(&gEnetLpbk.rxReadyQ));

    /* Close RX channel */
#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
    EnetAppUtils_closeRxFlow(gEnetLpbk.enetType,
                             gEnetLpbk.hEnet,
                             gEnetLpbk.coreKey,
                             gEnetLpbk.coreId,
                             true,
                             &fqPktInfoQ,
                             &cqPktInfoQ,
                             gEnetLpbk.rxStartFlowIdx,
                             gEnetLpbk.rxFlowIdx,
                             gEnetLpbk.hostMacAddr,
                             gEnetLpbk.hRxCh);
#elif defined (ENET_SOC_HOSTPORT_DMA_TYPE_CPDMA)
    EnetAppUtils_closeRxCh(gEnetLpbk.hEnet,
                           gEnetLpbk.coreKey,
                           gEnetLpbk.coreId,
                           &fqPktInfoQ,
                           &cqPktInfoQ,
                           gEnetLpbk.hRxCh,
                           gEnetLpbk.rxChNum);
#endif

    EnetAppUtils_freePktInfoQ(&fqPktInfoQ);
    EnetAppUtils_freePktInfoQ(&cqPktInfoQ);

    /* Close TX channel */
    EnetQueue_initQ(&fqPktInfoQ);
    EnetQueue_initQ(&cqPktInfoQ);

    EnetAppUtils_closeTxCh(gEnetLpbk.hEnet,
                           gEnetLpbk.coreKey,
                           gEnetLpbk.coreId,
                           &fqPktInfoQ,
                           &cqPktInfoQ,
                           gEnetLpbk.hTxCh,
                           gEnetLpbk.txChNum);
    EnetAppUtils_freePktInfoQ(&fqPktInfoQ);
    EnetAppUtils_freePktInfoQ(&cqPktInfoQ);

    EnetAppUtils_freePktInfoQ(&gEnetLpbk.rxFreeQ);
    EnetAppUtils_freePktInfoQ(&gEnetLpbk.txFreePktInfoQ);

    EnetMem_deInit();
}
