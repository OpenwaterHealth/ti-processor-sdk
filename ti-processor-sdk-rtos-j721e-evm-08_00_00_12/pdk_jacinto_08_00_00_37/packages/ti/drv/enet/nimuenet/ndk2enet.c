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
 * \file  ndk2enet.c
 *
 * \brief Translation layer to go from NDK to ENET hardware driver.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <stdarg.h>

/* Standard language headers */
#include <stdio.h>
#include <assert.h>

/* xdc header - should be first file included as due to order dependency */
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

/* OS/Posix headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/utils/Load.h>
#include <ti/sysbios/hal/Cache.h>

/* NDK Dependencies */
#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/inc/stkmain.h>

/* ENET driver dependency headers */
#include <ti/drv/enet/enet.h>
#include <ti/drv/enet/include/core/enet_utils.h>
#include <ti/drv/enet/enet_cfg.h>
#include <ti/drv/enet/include/core/enet_dma.h>
#include <ti/drv/enet/include/per/cpsw.h>

//FIXME
#include <ti/drv/enet/examples/utils/include/enet_apputils.h>
#include <ti/drv/enet/examples/utils/include/enet_appmemutils.h>
#include <ti/drv/enet/examples/utils/include/enet_appmemutils_cfg.h>

#include <ti/drv/uart/UART_stdio.h>

#include <ti/sysbios/interfaces/ITimer.h>

/* This module's header */
#include "ndk2enet.h"
#include "ndk2enet_appif.h"

/* ========================================================================== */
/*                          Extern Declarations                               */
/* ========================================================================== */

extern PBMQ PBMQ_free;

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define NDK_RX_PACKET_TASK_PRI      (OS_TASKPRIHIGH)
#define NDK_RX_PACKET_TASK_STACK    (2048)

#define NDK_TX_PACKET_TASK_PRI      (OS_TASKPRIHIGH)
#define NDK_TX_PACKET_TASK_STACK    (2048)

#define NDK2ENET_DIVIDER_US_TO_MS   (1000U)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void Ndk2Enet_notifyRxPackets(void *cbArg);

static void Ndk2Enet_notifyTxPackets(void *cbArg);

static void Ndk2Enet_rxPacketTask(void *arg0);

static void Ndk2Enet_prepFreePktInfoQ(Ndk2Enet_Handle hNdk2Enet);

static void Ndk2Enet_pbmQ2PktInfoQ(Ndk2Enet_Handle hNdk2Enet,
                                   PBMQ *PBMPktQ,
                                   EnetDma_PktQ *pDmaPktInfoQ);

static void Ndk2Enet_pktInfoQ2PBMQ(Ndk2Enet_Handle hNdk2Enet,
                                   EnetDma_PktQ *pDmaPktInfoQ,
                                   PBMQ *PBMPktQ);

static uint32_t Ndk2Enet_prepRxPktQ(Ndk2Enet_Handle hNdk2Enet,
                                    EnetDma_PktQ *pPktQ);

static void Ndk2Enet_submitRxPktQ(Ndk2Enet_Handle hNdk2Enet);

static void Ndk2Enet_txPacketTask(void *arg0);

static uint32_t Ndk2Enet_prepTxPktQ(Ndk2Enet_Handle hNdk2Enet,
                                    EnetDma_PktQ *pPktQ);

static void Ndk2Enet_freePBMPackets(EnetDma_PktQ *tempQueue);

static void Ndk2Enet_updateRxNotifyStats(uint32_t packetCount,
                                         uint32_t timeDiff);

static void Ndk2Enet_updateTxNotifyStats(uint32_t packetCount,
                                         uint32_t timeDiff);

static void Ndk2Enet_print(const char *prnStr,
                           ...);

static int32_t Ndk2Enet_startRxTx(Ndk2Enet_Handle hNdk2Enet);

static void Ndk2Enet_stopRxTx(Ndk2Enet_Handle hNdk2Enet);

static void Ndk2Enet_submitTxPackets(Ndk2Enet_Handle hNdk2Enet,
                                     EnetDma_PktQ *pSubmitQ);

static void Ndk2Enet_submitRxPackets(Ndk2Enet_Handle hNdk2Enet,
                                     EnetDma_PktQ *pSubmitQ);

static void Ndk2Enet_retrieveTxPkts(Ndk2Enet_Handle hNdk2Enet);

static void Ndk2Enet_timerCb(UArg arg);

static void Ndk2Enet_createTimer(Ndk2Enet_Handle hNdk2Enet);

static void Ndk2Enet_initGetHandleInArgs(Ndk2Enet_Handle hNdk2Enet,
                                         NimuEnetAppIf_GetHandleInArgs *inArgs);

static void Ndk2Enet_initReleaseHandleInArgs(Ndk2Enet_Handle hNdk2Enet,
                                             NimuEnetAppIf_ReleaseHandleInfo *inArgs);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static Ndk2Enet_Object gNdk2EnetObj;
Ndk2Enet_Stats gNdk2EnetStats;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  @b Ndk2Enet_open
 *  @n
 *      Opens and configures Enet. Configures Interrupts, SGMII,
 *      and send and receive queues.
 *
 *  \param[in]  hNdk2Enet
 *      Ndk2Enet_object structure pointer.
 *
 *  \retval
 *      Success -  0
 *  \retval
 *      Error   -  Any non-zero value
 */
Ndk2Enet_Handle Ndk2Enet_open(STKEVENT_Handle hEvent)
{
    int32_t status;
    NimuEnetAppIf_GetHandleInArgs getHandleInArgs;
    Ndk2Enet_Handle hNdk2Enet;
    Semaphore_Params semaphoreParams;
    Task_Params params;
    Error_Block eb;
    uint32_t semInitCnt;

    hNdk2Enet = &gNdk2EnetObj;

    /* Clear instrumentation statistics structure */
    memset(&gNdk2EnetStats, 0, sizeof(Ndk2Enet_Stats));

    /* Initialize the allocated memory block. */
    memset(hNdk2Enet, 0, sizeof(Ndk2Enet_Object));

    /* Store total number of free PBM packets allocated. Used during close to
     * make sure all PBM packets are returned to NDK */
    hNdk2Enet->numAllocPBMPkts = PBMQ_count(&PBMQ_free);

    /* Initialize the Rx Queues & Data */
    PBMQ_init(&hNdk2Enet->rxReadyPBMPktQ);

    /* Initialize the Tx Queues & Data */
    PBMQ_init(&hNdk2Enet->txReadyPBMPktQ);
    PBMQ_init(&hNdk2Enet->txUnUsedPBMPktQ);

    /* Keep stack event handle to use to signal the NDK stack */
    hNdk2Enet->hEvent = hEvent;

    /* MCast List is EMPTY */
    hNdk2Enet->MCastCnt = 0;

    /* Init internal bookkeeping fields */
    hNdk2Enet->oldMCastCnt = 0;

    /* First init tasks, semaphores and clocks. This is required because
     * EnetDma event cb ISR can happen immediately after opening rx flow
     * in NimuEnetAppCb_getHandle and all semaphores, clocks and tasks should
     * be valid at that point
     */
    /* Create semaphore objects, init shutDownFlag status */
    hNdk2Enet->shutDownFlag = false;

    semInitCnt              = 0U;
    hNdk2Enet->hShutDownSem = Semaphore_create(semInitCnt, NULL, NULL);
    Ndk2Enet_assert(NULL != hNdk2Enet->hShutDownSem);

    Semaphore_Params_init(&semaphoreParams);
    semaphoreParams.mode    = Semaphore_Mode_BINARY;
    hNdk2Enet->hRxPacketSem = Semaphore_create(semInitCnt, &semaphoreParams, NULL);
    Ndk2Enet_assert(NULL != hNdk2Enet->hRxPacketSem);

    Semaphore_Params_init(&semaphoreParams);
    semaphoreParams.mode    = Semaphore_Mode_BINARY;
    hNdk2Enet->hTxPacketSem = Semaphore_create(semInitCnt, &semaphoreParams, NULL);
    Ndk2Enet_assert(NULL != hNdk2Enet->hTxPacketSem);
    /* Start the packet processing tasks now that the channels are open */
    Error_init(&eb);
    Task_Params_init(&params);
    params.instance->name = "Ndk2Enet_RxPacketTask";
    params.priority       = NDK_RX_PACKET_TASK_PRI;
    params.stackSize      = NDK_RX_PACKET_TASK_STACK;
    params.arg0           = (uintptr_t)hNdk2Enet;

    hNdk2Enet->hRxPacketTask = Task_create((Task_FuncPtr) & Ndk2Enet_rxPacketTask, &params, &eb);
    Ndk2Enet_assert(NULL != hNdk2Enet->hRxPacketTask);

    Error_init(&eb);
    Task_Params_init(&params);
    params.instance->name = "Ndk2Enet_txPacketTask";
    params.priority       = NDK_TX_PACKET_TASK_PRI;
    params.stackSize      = NDK_TX_PACKET_TASK_STACK;
    params.arg0           = (uintptr_t)hNdk2Enet;

    hNdk2Enet->hTxPacketTask = Task_create((Task_FuncPtr) & Ndk2Enet_txPacketTask, &params, &eb);
    Ndk2Enet_assert(NULL != hNdk2Enet->hTxPacketTask);

    /* Get Enet & UDMA Drv Handle */
    Ndk2Enet_initGetHandleInArgs(hNdk2Enet, &getHandleInArgs);
    NimuEnetAppCb_getHandle(&getHandleInArgs,
                            &hNdk2Enet->appInfo);
    Ndk2Enet_assert(hNdk2Enet->appInfo.hEnet != NULL);
#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
    Ndk2Enet_assert(hNdk2Enet->appInfo.hUdmaDrv != NULL);
#endif
    Ndk2Enet_assert(hNdk2Enet->appInfo.isPortLinkedFxn != NULL);
    Ndk2Enet_assert(hNdk2Enet->appInfo.rxInfo.hRxFlow != NULL);
    Ndk2Enet_assert(hNdk2Enet->appInfo.txInfo.hTxChannel != NULL);
    if (NULL != hNdk2Enet->appInfo.print)
    {
        /* set the print function callback if not null */
        hNdk2Enet->print = hNdk2Enet->appInfo.print;
    }
    else
    {
        hNdk2Enet->print = (Enet_Print)&System_printf;
    }

    /* Open Enet DMA driver */
    status = Ndk2Enet_startRxTx(hNdk2Enet);
    if (status != ENET_SOK)
    {
        Ndk2Enet_print("Failed to open Enet DMA: %d\n", status);
    }

    /* Get initial link/interface status from the driver */
    hNdk2Enet->linkIsUp = hNdk2Enet->appInfo.isPortLinkedFxn(hNdk2Enet->appInfo.hEnet);

    if (hNdk2Enet->appInfo.disableTxEvent)
    {
        EnetDma_disableTxEvent(hNdk2Enet->appInfo.txInfo.hTxChannel);
    }


    if (false == hNdk2Enet->appInfo.isRingMonUsed)
    {
        EnetDma_disableRxEvent(hNdk2Enet->appInfo.rxInfo.hRxFlow);
    }

    /* assert if clk period is not valid  */
    Ndk2Enet_assert(0U != hNdk2Enet->appInfo.timerPeriodUs);
    Ndk2Enet_createTimer(hNdk2Enet);
    Timer_start(hNdk2Enet->hPacingTimer);

    hNdk2Enet->initDone = TRUE;

    return hNdk2Enet;
}

/*!
 *  @b Ndk2Enet_close
 *  @n
 *      Closes Enet and disables interrupts.
 *
 *  \param[in]  hNdk2Enet
 *      Ndk2Enet_object structure pointer.
 *
 *  \retval
 *      void
 */
void Ndk2Enet_close(Ndk2Enet_Handle hNdk2Enet)
{

    uint32_t pktInfoQCnt;
    NimuEnetAppIf_ReleaseHandleInfo releaseHandleInfo;

    Ndk2Enet_assert(NULL != hNdk2Enet);
    Ndk2Enet_assert(NULL != hNdk2Enet->hRxPacketSem);
    Ndk2Enet_assert(NULL != hNdk2Enet->hTxPacketSem);
    Ndk2Enet_assert(NULL != hNdk2Enet->hShutDownSem);

    /* Set the translation layer shutdown flag */
    hNdk2Enet->shutDownFlag = true;

    /* Stop and delete the tick timer */
    Ndk2Enet_assert(NULL != hNdk2Enet->hPacingTimer);
    Timer_stop (hNdk2Enet->hPacingTimer);
    Timer_delete ((Timer_Handle *)&hNdk2Enet->hPacingTimer);

    Ndk2Enet_stopRxTx(hNdk2Enet);

    Ndk2Enet_initReleaseHandleInArgs(hNdk2Enet, &releaseHandleInfo);
    NimuEnetAppCb_releaseHandle(&releaseHandleInfo);

    /* free txReadyPBMPktQ */
    while (PBMQ_count(&hNdk2Enet->txReadyPBMPktQ) != 0U)
    {
        PBM_Handle hPBMPacket = PBMQ_deq(&hNdk2Enet->txReadyPBMPktQ);
        PBM_free(hPBMPacket);
        Ndk2EnetStas_addOne(&gNdk2EnetStats.rxPBMFreeCnt);
    }

    pktInfoQCnt = EnetQueue_getQCount(&hNdk2Enet->txFreePktInfoQ) +
                  EnetQueue_getQCount(&hNdk2Enet->rxFreePktInfoQ);
    if (ENET_ARRAYSIZE(hNdk2Enet->pktInfoMem) != pktInfoQCnt)
    {
        Ndk2Enet_print("Ndk2Enet_close: Failed to retrieve all PktInfo\n");
    }

#if !defined(NDK_MULTIPLE_STACKS)
    /* debug point */

    /* We can't enable below by default as below check will fail if there are
     * two drivers registered with NDK which share PBM buffers */

    /* Check for PBM packets allocated during init are freed correctly and packet
     * count before nimu driver open matches after close. */
    if (hNdk2Enet->numAllocPBMPkts != PBMQ_count(&PBMQ_free))
    {
        Ndk2Enet_print("Ndk2Enet_close: Failed to retrieve all PBM packets\n");
    }
#endif

    memset((void *)&hNdk2Enet->appInfo, 0, sizeof(hNdk2Enet->appInfo));

    /* Pend on hShutDownSem (twice for two sub-tasks) */
    Semaphore_pend(hNdk2Enet->hShutDownSem, BIOS_WAIT_FOREVER);
    Semaphore_pend(hNdk2Enet->hShutDownSem, BIOS_WAIT_FOREVER);

    /* Delete the semaphore objects */
    Semaphore_delete(&hNdk2Enet->hRxPacketSem);
    Semaphore_delete(&hNdk2Enet->hTxPacketSem);
    Semaphore_delete(&hNdk2Enet->hShutDownSem);

    /*
     * We don't do task deletion since the NDK forces the
     * Task.deleteTerminatedTasks option be true, which means
     * the hRxPacketTask and hTxPacketTask are auto-deleted.
     */

    /* Clear the allocated translation */
    memset(hNdk2Enet, 0, sizeof(Ndk2Enet_Object));
}

/*!
 *  @b Ndk2Enet_setRx
 *  @n
 *      Sets the filter for Enet. Sets up the multicast addresses in
 *      the ALE.
 *
 *  \param[in]  hNdk2Enet
 *      Ndk2Enet_object structure pointer.
 *
 *  \retval
 *      void
 */
void Ndk2Enet_setRx(Ndk2Enet_Handle hNdk2Enet)
{
}

/*!
 *  @b Ndk2Enet_sendTxPackets
 *  @n
 *      Routine to send out queued Tx packets to the hardware driver
 *
 *  \param[in]  hNdk2Enet
 *      Ndk2Enet_object structure pointer.
 *
 *  \retval
 *      void
 */
void Ndk2Enet_sendTxPackets(Ndk2Enet_Handle hNdk2Enet)
{
    EnetDma_Pkt *dmaPkt;
    PBM_Handle hPBMPacket;

    /* If link is not up, simply return */
    EnetDma_PktQ txSubmitQ;
    EnetQueue_initQ(&txSubmitQ);
    if (PBMQ_count(&hNdk2Enet->txUnUsedPBMPktQ))
    {
        /* send any pending TX Q's */
        Ndk2Enet_pbmQ2PktInfoQ(hNdk2Enet, &hNdk2Enet->txUnUsedPBMPktQ, &txSubmitQ);
    }

    /* Check if there is anything to transmit, else simply return */
    while (PBMQ_count(&hNdk2Enet->txReadyPBMPktQ) != 0)
    {
        /* Dequeue one free TX Eth packet */
        dmaPkt = (EnetDma_Pkt *)EnetQueue_deq(&hNdk2Enet->txFreePktInfoQ);

        if (dmaPkt == NULL)
        {
            /* If we run out of packet info Q, retrieve packets from HW
             * and try to dequeue free packet again */
            Ndk2Enet_retrieveTxPkts(hNdk2Enet);
            dmaPkt = (EnetDma_Pkt *)EnetQueue_deq(&hNdk2Enet->txFreePktInfoQ);
        }

        if (NULL != dmaPkt)
        {
            hPBMPacket = PBMQ_deq(&hNdk2Enet->txReadyPBMPktQ);
            EnetDma_initPktInfo(dmaPkt);

            dmaPkt->bufPtr = PBM_getDataBuffer(hPBMPacket) +
                                     PBM_getDataOffset(hPBMPacket);
            dmaPkt->appPriv    = hPBMPacket;
            dmaPkt->orgBufLen  = PBM_getBufferLen(hPBMPacket);
            dmaPkt->userBufLen = PBM_getValidLen(hPBMPacket);
            dmaPkt->node.next = NULL;

#ifdef NDK_CHECKSUM_SUPPORT
            PBM_Pkt *pbmPkt = (PBM_Pkt *)hPBMPacket;
            ENETUDMA_CPPIPSI_SET_CHKSUM_RES (dmaPkt->chkSumInfo, (pbmPkt->csInsertPos));
            ENETUDMA_CPPIPSI_SET_CHKSUM_STARTBYTE (dmaPkt->chkSumInfo, (pbmPkt->csStartPos));
            ENETUDMA_CPPIPSI_SET_CHKSUM_INV_FLAG (dmaPkt->chkSumInfo, true);
            ENETUDMA_CPPIPSI_SET_CHKSUM_BYTECNT (dmaPkt->chkSumInfo, (pbmPkt->csNumBytes));
#endif
            ENET_UTILS_COMPILETIME_ASSERT(offsetof(EnetDma_Pkt, node) == 0);
            EnetQueue_enq(&txSubmitQ, &(dmaPkt->node));

            Ndk2EnetStas_addOne(&gNdk2EnetStats.txFreeAppPktDq);
            Ndk2EnetStas_addOne(&gNdk2EnetStats.txReadyPBMPktDq);
        }
        else
        {
            break;
        }
    }

    /* Give the accumulated packets to the hardware */
    /* Submit the Enet packet for transmission */
    Ndk2Enet_submitTxPackets(hNdk2Enet, &txSubmitQ);
}

/*!
 *  @b Ndk2Enet_ioctl
 *  @n
 *  Low level driver Ioctl interface. This interface can be used for
 *  ALE configuration, control, statistics
 *
 *  \param[in]  hNdk2Enet
 *      Ndk2Enet_object structure pointer.
 *  \param[in]  cmd
 *      Ioctl command.
 *  \param[in]  pBuf
 *      Ioctl buffer with commands and params to set/get
 *      configuration from hardware.
 *  \param[in]  size
 *      Size of Ioctl buffer.
 *
 *  \retval
 *      void
 */
int32_t Ndk2Enet_ioctl(Ndk2Enet_Handle hNdk2Enet,
                       uint32_t cmd,
                       void *param,
                       uint32_t size)
{
    int32_t retVal = 0U;

    // TODO - make NIMU IOCTL design consistent with Enet/Enet modules

    /* Decode the command and act on it */
    switch (cmd)
    {
        /* Rx Task Load IOCTL */
        case NDK2ENET_IOCTL_GET_RXTASK_LOAD:
        {
            Load_Stat stat;
            if (Load_getTaskLoad(hNdk2Enet->hRxPacketTask, &stat))
            {
                retVal = Load_calculateLoad(&stat);
            }

            break;
        }

        /* Adding Tx Task Load  */
        case NDK2ENET_IOCTL_GET_TXTASK_LOAD:
        {
            Load_Stat stat;
            if (Load_getTaskLoad(hNdk2Enet->hTxPacketTask, &stat))
            {
                retVal = Load_calculateLoad(&stat);
            }

            break;
        }

        default:
        {
            /* TODO . To pass through IOCTL commands to Enet, we need to know
             *  outArgs type and pass the correct size.
             *  This is not possible with the current NIMU interface
             */
            Ndk2Enet_assert(FALSE);
        }
    }

    return retVal;
}

void Ndk2Enet_periodicFxn(Ndk2Enet_Handle hNdk2Enet)
{
    uint32_t prevLinkState     = hNdk2Enet->linkIsUp;
    uint32_t prevLinkInterface = hNdk2Enet->currLinkedIf;

#if ENET_CFG_IS_ON(DEV_ERROR)

    EnetQueue_verifyQCount(&hNdk2Enet->txFreePktInfoQ);
    EnetQueue_verifyQCount(&hNdk2Enet->rxFreePktInfoQ);

#if !(defined(SOC_TPR12) || defined(SOC_AWR294X))

    int32_t status;

    status = EnetUdma_checkRxFlowSanity(hNdk2Enet->appInfo.rxInfo.hRxFlow, 5U);
    if (status != ENET_SOK)
    {
        Ndk2Enet_print("EnetUdma_checkRxFlowSanity Failed\n");
    }

    status = EnetUdma_checkTxChSanity(hNdk2Enet->appInfo.txInfo.hTxChannel, 5U);
    if (status != ENET_SOK)
    {
        Ndk2Enet_print("EnetUdma_checkTxChSanity Failed\n");
    }
#endif
#endif

    /*
     * Return the same DMA packets back to the DMA channel (but now
     * associated with a new PBM Packet and buffer)
     */
    if ( (EnetQueue_getQCount(&hNdk2Enet->rxFreePktInfoQ) != 0U) &&
         (PBMQ_count(&PBMQ_free) != 0U) )
    {
        Ndk2Enet_submitRxPktQ(hNdk2Enet);
    }

#if defined(NIMU_INSTRUMENTATION_ENABLED)
    static uint32_t loadCount = 0;
    Load_Stat stat;
    gNdk2EnetStats.gCpuLoad[loadCount] = Load_getCPULoad();

    if (Load_getGlobalHwiLoad(&stat))
    {
        gNdk2EnetStats.gHwiLoad[loadCount] = Load_calculateLoad(&stat);
    }

    if (Load_getTaskLoad(hNdk2Enet->hRxPacketTask, &stat))
    {
        gNdk2EnetStats.rxStats.taskLoad[loadCount] = Load_calculateLoad(&stat);
    }

    if (Load_getTaskLoad(hNdk2Enet->hTxPacketTask, &stat))
    {
        gNdk2EnetStats.txStats.taskLoad[loadCount] = Load_calculateLoad(&stat);
    }

    loadCount = (loadCount + 1U) & (HISTORY_CNT - 1U);
#endif

    /* Get current link status as reported by the hardware driver */
    hNdk2Enet->linkIsUp = hNdk2Enet->appInfo.isPortLinkedFxn(hNdk2Enet->appInfo.hEnet);

    /* If the interface changed, discard any queue packets (since the MAC would now be wrong) */
    if (prevLinkInterface != hNdk2Enet->currLinkedIf)
    {
        /* ToDo: Discard all queued packets */
    }

    /* If link status changed from down->up, then send any queued packets */
    if ((prevLinkState == 0) && (hNdk2Enet->linkIsUp))
    {
        Ndk2Enet_sendTxPackets(hNdk2Enet);
    }
}

static void Ndk2Enet_processRxUnusedQ(Ndk2Enet_Handle hNdk2Enet,
                                      EnetDma_PktQ *unUsedQ)
{
    EnetDma_Pkt *dmaPkt;

    dmaPkt = (EnetDma_Pkt *)EnetQueue_deq(unUsedQ);
    while (dmaPkt != NULL)
    {
        /* Get the full PBM packet that needs to be freed */
        PBM_Handle hPBMPacket = (PBM_Handle)dmaPkt->appPriv;
        if (hPBMPacket)
        {
            /* Free the PBM */
            PBM_free(hPBMPacket);
            Ndk2EnetStas_addOne(&gNdk2EnetStats.rxPBMFreeCnt);

            /* Put packet info into free Q as we have removed the PBM buffers
             * from the it */
            EnetQueue_enq(&hNdk2Enet->rxFreePktInfoQ, &dmaPkt->node);
            Ndk2EnetStas_addOne(&gNdk2EnetStats.rxFreeAppPktEnq);

            dmaPkt = (EnetDma_Pkt *)EnetQueue_deq(unUsedQ);
        }
        else
        {
            /* should never happen as this is received from HW */
            Ndk2Enet_assert(FALSE);
        }
    }
}

static void Ndk2Enet_submitRxPackets(Ndk2Enet_Handle hNdk2Enet,
                                     EnetDma_PktQ *pSubmitQ)
{
    int32_t retVal;

    retVal = EnetDma_submitRxPktQ(hNdk2Enet->appInfo.rxInfo.hRxFlow, pSubmitQ);
    if (ENET_SOK != retVal)
    {
        Ndk2Enet_print("EnetDma_submitRxPktQ: failed to submit pkts: %d\n", retVal);
    }

    if (EnetQueue_getQCount(pSubmitQ))
    {
        /* Copy unused packets to back to readyQ */
        Ndk2Enet_processRxUnusedQ(hNdk2Enet, pSubmitQ);
    }
}

static void Ndk2Enet_pbmQ2PktInfoQ(Ndk2Enet_Handle hNdk2Enet,
                                   PBMQ *PBMPktQ,
                                   EnetDma_PktQ *pDmaPktInfoQ)
{
    EnetDma_Pkt *dmaPkt;
    PBM_Handle hPBMPacket;

    while (PBMQ_count(PBMPktQ) != 0U)
    {
        /* Dequeue one free TX Eth packet */
        dmaPkt = (EnetDma_Pkt *)EnetQueue_deq(&hNdk2Enet->txFreePktInfoQ);

        if (dmaPkt == NULL)
        {
            /* If we run out of packet info Q, retrieve packets from HW
             * and try to dequeue free packet again */
            Ndk2Enet_retrieveTxPkts(hNdk2Enet);
            dmaPkt = (EnetDma_Pkt *)EnetQueue_deq(&hNdk2Enet->txFreePktInfoQ);
        }

        if (NULL != dmaPkt)
        {
            hPBMPacket = PBMQ_deq(PBMPktQ);
            EnetDma_initPktInfo(dmaPkt);

            dmaPkt->bufPtr = PBM_getDataBuffer(hPBMPacket) +
                                     PBM_getDataOffset(hPBMPacket);
            dmaPkt->appPriv    = hPBMPacket;
            dmaPkt->orgBufLen  = PBM_getBufferLen(hPBMPacket);
            dmaPkt->userBufLen = PBM_getValidLen(hPBMPacket);
            dmaPkt->node.next = NULL;

            ENET_UTILS_COMPILETIME_ASSERT(offsetof(EnetDma_Pkt, node) == 0);
            EnetQueue_enq(pDmaPktInfoQ, &(dmaPkt->node));

            Ndk2EnetStas_addOne(&gNdk2EnetStats.txFreeAppPktDq);
        }
        else
        {
            break;
        }
    }
}

static void Ndk2Enet_pktInfoQ2PBMQ(Ndk2Enet_Handle hNdk2Enet,
                                   EnetDma_PktQ *pDmaPktInfoQ,
                                   PBMQ *PBMPktQ)
{
    EnetDma_Pkt *dmaPkt;
    PBM_Handle hPBMPacket;

    while (EnetQueue_getQCount(pDmaPktInfoQ) != 0U)
    {
        dmaPkt = (EnetDma_Pkt *)EnetQueue_deq(pDmaPktInfoQ);
        hPBMPacket = (PBM_Handle)(dmaPkt->appPriv);

        PBMQ_enq(PBMPktQ, hPBMPacket);
    }
}

static void Ndk2Enet_submitTxPackets(Ndk2Enet_Handle hNdk2Enet,
                                     EnetDma_PktQ *pSubmitQ)
{
    int32_t retVal;

    retVal = EnetDma_submitTxPktQ(hNdk2Enet->appInfo.txInfo.hTxChannel, pSubmitQ);
    if (ENET_SOK != retVal)
    {
        Ndk2Enet_print("EnetDma_submitTxPktQ: failed to submit pkts: %d\n", retVal);
    }

    if (EnetQueue_getQCount(pSubmitQ))
    {
        /* TODO: txUnUsedPBMPktQ can be removed once PBMQ_enqHead
         *       function in NDK (https://jira.itg.ti.com/browse/NDK-379)
         *       is implemented.
         *       PBMQ_enqHead(&hNdk2Enet->txReadyPBMPktQ, hPBMPacket);
         */
        Ndk2Enet_pktInfoQ2PBMQ(hNdk2Enet, pSubmitQ, &hNdk2Enet->txUnUsedPBMPktQ);
        EnetQueue_append(&hNdk2Enet->txFreePktInfoQ, pSubmitQ);
        Ndk2EnetStats_addNum(&gNdk2EnetStats.txFreeAppPktEnq, EnetQueue_getQCount(pSubmitQ));
    }
}

static void Ndk2Enet_freePBMPackets(EnetDma_PktQ *tempQueue)
{
    EnetDma_Pkt *dmaPkt;
    PBM_Handle hPBMPacket;

    while (EnetQueue_getQCount(tempQueue))
    {
        dmaPkt = (EnetDma_Pkt *)EnetQueue_deq(tempQueue);
        hPBMPacket     = dmaPkt->appPriv;
        PBM_free(hPBMPacket);
    }
}

static void Ndk2Enet_notifyRxPackets(void *cbArg)
{
    /* Post semaphore to rx handling task */
    Ndk2Enet_Handle hNdk2Enet = (Ndk2Enet_Handle)cbArg;

    /* do not post events if init not done or shutdown in progress */
    if ((hNdk2Enet->initDone) && (hNdk2Enet->shutDownFlag == false))
    {
        EnetDma_disableRxEvent(hNdk2Enet->appInfo.rxInfo.hRxFlow);
        Semaphore_post(hNdk2Enet->hRxPacketSem);
    }
}

static void Ndk2Enet_notifyTxPackets(void *cbArg)
{
    /* Post semaphore to tx handling task */
    Ndk2Enet_Handle hNdk2Enet = (Ndk2Enet_Handle)cbArg;

    /* do not post events if init not done or shutdown in progress */
    if ((hNdk2Enet->initDone) && (hNdk2Enet->shutDownFlag == false))
    {
        Semaphore_post(hNdk2Enet->hTxPacketSem);
    }
}

static void Ndk2Enet_rxPacketTask(void *arg0)
{
    Ndk2Enet_Handle hNdk2Enet = (Ndk2Enet_Handle)arg0;
    EnetDma_PktQ tempQueue;
    int32_t retVal;
    uint32_t pktCnt;

    while (!hNdk2Enet->shutDownFlag)
    {
        /* Wait for the Rx ISR to notify us that packets are available with data */
        Semaphore_pend(hNdk2Enet->hRxPacketSem, BIOS_WAIT_FOREVER);

        if (hNdk2Enet->shutDownFlag)
        {
            /* This translation layer is shutting down, don't give anything else to the stack */
            break;
        }

        Ndk2EnetStas_addOne(&gNdk2EnetStats.rxStats.rawNotifyCnt);
        pktCnt = 0;

        /* Retrieve the used (filled) packets from the channel */
        {
            EnetQueue_initQ(&tempQueue);
            retVal = EnetDma_retrieveRxPktQ(hNdk2Enet->appInfo.rxInfo.hRxFlow,
                                               &tempQueue);
            if (ENET_SOK != retVal)
            {
                Ndk2Enet_print("Ndk2Enet_rxPacketTask: failed to retrieve RX pkts: %d\n", retVal);
            }
        }
        if (tempQueue.count == 0)
        {
            Ndk2EnetStas_addOne(&gNdk2EnetStats.rxStats.zeroNotifyCnt);
        }

        /*
         * Call Ndk2Enet_prepRxPktQ() even if no packets were received.
         * This allows new packets to be submitted if PBM buffers became
         * newly available and there were outstanding free packets.
         */
        {
            /*
             * Get all used Rx DMA packets from the hardware, then send the buffers
             * of those packets on to the NDK stack to be parsed/processed.
             */
            pktCnt = Ndk2Enet_prepRxPktQ(hNdk2Enet, &tempQueue);
        }

        /*
         * We don't want to time the semaphore post used to notify the NDK stack as that may cause a
         * task transition. We don't want to time the semaphore pend, since that would time us doing
         * nothing but waiting.
         */
        if (pktCnt != 0)
        {
            Ndk2Enet_updateRxNotifyStats(pktCnt, 0U);
        }

        /* Notify NDK stack that packets are available (this is basically a Semaphore_post) */
        if (PBMQ_count(&hNdk2Enet->rxReadyPBMPktQ) != 0U)
        {
            ((STKEVENT *)(hNdk2Enet->hEvent))->EventCodes[STKEVENT_ETHERNET] = 1;
            if (((STKEVENT *)(hNdk2Enet->hEvent))->hSemEvent != NULL)
            {
                SemPost(((STKEVENT *)(hNdk2Enet->hEvent))->hSemEvent);
            }

            Ndk2EnetStas_addOne(&gNdk2EnetStats.ndkStackNotifyCnt);
        }

        /* As we are operating in one shot mode, the Timer_start should be
         * called for the next timeout period
         */
        Timer_start(hNdk2Enet->hPacingTimer);
    }

    /* We are shutting down, notify that we are done */
    Semaphore_post(hNdk2Enet->hShutDownSem);
}


static void Ndk2Enet_txPacketTask(void *arg0)
{
    Ndk2Enet_Handle hNdk2Enet = (Ndk2Enet_Handle)arg0;

    while (!hNdk2Enet->shutDownFlag)
    {
        /*
         * Wait for the Tx ISR to notify us that empty packets are available
         * that were used to send data
         */
        Semaphore_pend(hNdk2Enet->hTxPacketSem, SEM_FOREVER);
        Ndk2Enet_retrieveTxPkts(hNdk2Enet);
    }

    /* We are shutting down, notify that we are done */
    Semaphore_post(hNdk2Enet->hShutDownSem);
}

static void Ndk2Enet_prepFreePktInfoQ(Ndk2Enet_Handle hNdk2Enet)
{
    uint32_t i;
    EnetDma_Pkt *dmaPkt;

    /* Initialize the DMA Free Queue*/
    EnetQueue_initQ(&hNdk2Enet->txFreePktInfoQ);
    EnetQueue_initQ(&hNdk2Enet->rxFreePktInfoQ);

    /*
     * Allocate the free pkt info Q. This is used to exchange buffer info from
     * DMA basically for submitting free packets and retrieving ready packets.
     */
    for (i = 0U; i < (uint32_t)(NDK2ENET_TX_PACKETS); i++)
    {
        /* Initialize Pkt info Q from allocated memory */
        dmaPkt = &hNdk2Enet->pktInfoMem[i];
        Ndk2Enet_assert(dmaPkt != NULL);
        EnetDma_initPktInfo(dmaPkt);
        EnetQueue_enq(&hNdk2Enet->txFreePktInfoQ, &dmaPkt->node);
        Ndk2EnetStas_addOne(&gNdk2EnetStats.txFreeAppPktEnq);
    }

    for (i = NDK2ENET_TX_PACKETS; i < (uint32_t)(NDK2ENET_TX_PACKETS + NDK2ENET_RX_PACKETS); i++)
    {
        /* Initialize Pkt info Q from allocated memory */
        dmaPkt = &hNdk2Enet->pktInfoMem[i];
        Ndk2Enet_assert(dmaPkt != NULL);
        EnetDma_initPktInfo(dmaPkt);
        EnetQueue_enq(&hNdk2Enet->rxFreePktInfoQ, &dmaPkt->node);
        Ndk2EnetStas_addOne(&gNdk2EnetStats.rxFreeAppPktEnq);
    }
}

/*
 * Enqueue a new packet and make sure that buffer descriptors are properly linked.
 * NOTE: Not thread safe
 */
static void Ndk2Enet_submitRxPktQ(Ndk2Enet_Handle hNdk2Enet)
{
    EnetDma_PktQ resubmitPktQ;
    PBM_Handle hPBMPacket;
    EnetDma_Pkt *dmaPkt;
    uint32_t bufSize;
    bool retryPkt = false;
    int32_t oldPri;

    EnetQueue_initQ(&resubmitPktQ);

    /*
     * Fill in as many packets as we can with new PBM buffers so they can be
     * returned to the stack to be filled in.
     */
    dmaPkt = (EnetDma_Pkt *)EnetQueue_deq(&hNdk2Enet->rxFreePktInfoQ);

    while (NULL != dmaPkt)
    {
        if (retryPkt == false)
        {
            bufSize = NIMU_ENET_GET_PBM_BUFSIZE(hNdk2Enet->appInfo.hostPortRxMtu);
            bufSize = ENET_UTILS_ALIGN(bufSize, ENET_UTILS_CACHELINE_SIZE);
            hPBMPacket = PBM_alloc(bufSize);
        }
        else
        {
            retryPkt = false;
        }

        if (hPBMPacket)
        {
            Ndk2EnetStas_addOne(&gNdk2EnetStats.rxPBMAllocCnt);
            Ndk2EnetStas_addOne(&gNdk2EnetStats.rxFreeAppPktDq);

            /* The PBM packet has a data offset */
            PBM_setDataOffset(hPBMPacket, PKT_PREPAD);
            EnetDma_initPktInfo(dmaPkt);

            dmaPkt->bufPtr    = PBM_getDataBuffer(hPBMPacket) + PKT_PREPAD;
            dmaPkt->orgBufLen = PBM_getBufferLen(hPBMPacket) - PKT_PREPAD;
            /* Save off the PBM packet handle so it can be handled by this layer later */
            dmaPkt->appPriv = (void *)hPBMPacket;
            EnetQueue_enq(&resubmitPktQ, &dmaPkt->node);

            dmaPkt = (EnetDma_Pkt *)EnetQueue_deq(&hNdk2Enet->rxFreePktInfoQ);
        }
        else
        {
            /* let the NDK scheduler run to consume (and free up) some packets */
            oldPri = Task_setPri(Task_self(), 1);
            Task_yield();
            Task_setPri(Task_self(), oldPri);

            /* now, try again */
            hPBMPacket = PBM_alloc(bufSize);
            if (hPBMPacket)
            {
                retryPkt = true;
                continue;
            }
            else
            {
                Ndk2Enet_print("ERROR: PBM_alloc() failure.!\n");
                Ndk2Enet_print("PBMQ_free count = %d\n", PBMQ_count(&PBMQ_free));
                EnetQueue_enq(&hNdk2Enet->rxFreePktInfoQ, &dmaPkt->node);
                break;
            }
            //TODO add stats for this?
        }
    }

    /*
     * Return the same DMA packets back to the DMA channel (but now
     * associated with a new PBM Packet and buffer)
     */
    Ndk2Enet_submitRxPackets(hNdk2Enet, &resubmitPktQ);
}

static uint32_t Ndk2Enet_prepRxPktQ(Ndk2Enet_Handle hNdk2Enet,
                                    EnetDma_PktQ *pPktQ)
{
    uint32_t packetCount = 0;
    EnetDma_Pkt *dmaPkt;
#ifdef NDK_CHECKSUM_SUPPORT
    uint32_t csumInfo;
#endif
    bool chkSumErr = false;

    dmaPkt = (EnetDma_Pkt *)EnetQueue_deq(pPktQ);
    while (dmaPkt)
    {
        /* Get the full PBM packet that needs to be returned to the NDK stack */
        PBM_Handle hPBMPacket = (PBM_Handle)dmaPkt->appPriv;
        if (hPBMPacket)
        {
            uint32_t validLen = dmaPkt->userBufLen;
            /* Fill in PBM packet length field */
            PBM_setValidLen(hPBMPacket, validLen);

#ifdef NDK_CHECKSUM_SUPPORT
            /* We don't check if HW checksum offload is enabled while checking for checksum error
             * as default value of this field when offload not enabled is false */
            csumInfo = dmaPkt->chkSumInfo;
            if ( ENETUDMA_CPPIPSI_GET_IPV4_FLAG(csumInfo)||
                 ENETUDMA_CPPIPSI_GET_IPV6_FLAG(csumInfo) )
            {
                chkSumErr = ENETUDMA_CPPIPSI_GET_CHKSUM_ERR_FLAG(csumInfo);
            }
#endif

            if (!chkSumErr)
            {
                /* Enqueue the received packet so that it can be consumed by the NDK stack*/
                PBMQ_enq(&hNdk2Enet->rxReadyPBMPktQ, hPBMPacket);
                Ndk2EnetStas_addOne(&gNdk2EnetStats.rxReadyPBMPktEnq);
                packetCount++;
            }
            else
            {
                /* Put PBM buffer in free Q as we are not passing to stack */
                PBM_free(hPBMPacket);
                Ndk2EnetStas_addOne(&gNdk2EnetStats.rxPBMFreeCnt);
                Ndk2EnetStas_addOne(&gNdk2EnetStats.rxChkSumErr);
            }

            /* Put packet info into free Q as we have removed the PBM buffers
             * from the it */
            EnetQueue_enq(&hNdk2Enet->rxFreePktInfoQ, &dmaPkt->node);
            Ndk2EnetStas_addOne(&gNdk2EnetStats.rxFreeAppPktEnq);

            dmaPkt = (EnetDma_Pkt *)EnetQueue_deq(pPktQ);
        }
        else
        {
            /* Should never happen as this is received from HW */
            Ndk2Enet_assert(FALSE);
        }
    }

    /* return as many packets to ENET as we can */
    Ndk2Enet_submitRxPktQ(hNdk2Enet);

    return packetCount;
}

static uint32_t Ndk2Enet_prepTxPktQ(Ndk2Enet_Handle hNdk2Enet,
                                    EnetDma_PktQ *pPktQ)
{
    uint32_t packetCount;
    EnetDma_Pkt *dmaPkt;
    PBM_Handle hPBMPacket;

    packetCount = EnetQueue_getQCount(pPktQ);

    dmaPkt = (EnetDma_Pkt *)EnetQueue_deq(pPktQ);
    while (dmaPkt)
    {
        hPBMPacket = dmaPkt->appPriv;

        /* Free PBM buffer as it is transmitted by DMA now */
        PBM_free(hPBMPacket);
        /* Return packet info to free pool */
        EnetQueue_enq(&hNdk2Enet->txFreePktInfoQ, &dmaPkt->node);
        dmaPkt = (EnetDma_Pkt *)EnetQueue_deq(pPktQ);
    }

    Ndk2EnetStats_addNum(&gNdk2EnetStats.txFreeAppPktEnq, packetCount);

    return packetCount;
}

static void Ndk2Enet_updateTxNotifyStats(uint32_t packetCount,
                                         uint32_t timeDiff)
{
#if defined(NIMU_INSTRUMENTATION_ENABLED)
    uint32_t notificationCount;
    uint32_t timePerPacket = timeDiff / packetCount;

    notificationCount = gNdk2EnetStats.txStats.dataNotifyCnt & (HISTORY_CNT - 1U);
    gNdk2EnetStats.txStats.dataNotifyCnt++;

    gNdk2EnetStats.txStats.totalPktCnt   += packetCount;
    gNdk2EnetStats.txStats.totalCycleCnt += timeDiff;

    gNdk2EnetStats.txStats.cycleCntPerNotify[notificationCount] = timeDiff;
    if (timeDiff > gNdk2EnetStats.txStats.cycleCntPerNotifyMax)
    {
        gNdk2EnetStats.txStats.cycleCntPerNotifyMax = timeDiff;
    }

    gNdk2EnetStats.txStats.pktsPerNotify[notificationCount] = packetCount;
    if (packetCount > gNdk2EnetStats.txStats.pktsPerNotifyMax)
    {
        gNdk2EnetStats.txStats.pktsPerNotifyMax = packetCount;
    }

    gNdk2EnetStats.txStats.cycleCntPerPkt[notificationCount] = timePerPacket;
    if (timePerPacket > gNdk2EnetStats.txStats.cycleCntPerPktMax)
    {
        gNdk2EnetStats.txStats.cycleCntPerPktMax = timePerPacket;
    }
#endif
}

static void Ndk2Enet_updateRxNotifyStats(uint32_t packetCount,
                                         uint32_t timeDiff)
{
#if defined(NIMU_INSTRUMENTATION_ENABLED)
    uint32_t notificationCount;
    uint32_t timePerPacket = timeDiff / packetCount;

    notificationCount = gNdk2EnetStats.rxStats.dataNotifyCnt & (HISTORY_CNT - 1U);
    gNdk2EnetStats.rxStats.dataNotifyCnt++;

    gNdk2EnetStats.rxStats.totalPktCnt   += packetCount;
    gNdk2EnetStats.rxStats.totalCycleCnt += timeDiff;

    gNdk2EnetStats.rxStats.cycleCntPerNotify[notificationCount] = timeDiff;
    if (timeDiff > gNdk2EnetStats.rxStats.cycleCntPerNotifyMax)
    {
        gNdk2EnetStats.rxStats.cycleCntPerNotifyMax = timeDiff;
    }

    gNdk2EnetStats.rxStats.pktsPerNotify[notificationCount] = packetCount;
    if (packetCount > gNdk2EnetStats.rxStats.pktsPerNotifyMax)
    {
        gNdk2EnetStats.rxStats.pktsPerNotifyMax = packetCount;
    }

    gNdk2EnetStats.rxStats.cycleCntPerPkt[notificationCount] = timePerPacket;
    if (timePerPacket > gNdk2EnetStats.rxStats.cycleCntPerPktMax)
    {
        gNdk2EnetStats.rxStats.cycleCntPerPktMax = timePerPacket;
    }
#endif
}

static void Ndk2Enet_print(const char *prnStr,
                           ...)
{
    // TODO fix me
    if (NULL != gNdk2EnetObj.print)
    {
        /* Function is non- reentrant */
        va_list vaArgPtr;
        char *buf;

        buf = &gNdk2EnetObj.printBuf[0];
        va_start(vaArgPtr, prnStr);
        vsnprintf(buf, ENET_CFG_PRINT_BUF_LEN, (const char *)prnStr, vaArgPtr);
        va_end(vaArgPtr);

        (*gNdk2EnetObj.print)("[NDK2ENET] ");
        (*gNdk2EnetObj.print)(gNdk2EnetObj.printBuf);
    }
}

static int32_t Ndk2Enet_startRxTx(Ndk2Enet_Handle hNdk2Enet)
{
    int32_t status = ENET_SOK;
    Ndk2Enet_assert(NULL != hNdk2Enet->appInfo.txInfo.hTxChannel);
    Ndk2Enet_assert(NULL != hNdk2Enet->appInfo.rxInfo.hRxFlow);

    status = EnetDma_enableTxEvent(hNdk2Enet->appInfo.txInfo.hTxChannel);

    /* Prepare a Queue of DmaPktInfo pointers pointing to DmaPktInfoMem */
    Ndk2Enet_prepFreePktInfoQ(hNdk2Enet);

    Ndk2Enet_assert(hNdk2Enet->appInfo.hostPortRxMtu != 0);

    if (PBMQ_count(&PBMQ_free) < NDK2ENET_RX_PACKETS * (uint32_t)2U)
    {
        /*
         * PBM buffers should be allocated twice as DMA descriptors.
         * The idea here is that even if we fill up all the DMA descriptors submit queue,
         * we will have another complete set to swap in right away.
         */
        Ndk2Enet_print("Not enough PBM buffers allocated = %d, expected = %d\n",
                       PBMQ_count(&PBMQ_free),
                       NDK2ENET_RX_PACKETS * 2U);
        Ndk2Enet_assert(false);
    }

    /* Submit all allocated packets to DMA so it can use for packet RX */
    Ndk2Enet_submitRxPktQ(hNdk2Enet);

    return status;
}

static void Ndk2Enet_stopRxTx(Ndk2Enet_Handle hNdk2Enet)
{
    /* Close RX channel */
    /* Post to rx packet task so that it will terminate (shutDownFlag flag is already set) */
    if (hNdk2Enet->hRxPacketTask != NULL)
    {
        Semaphore_post(hNdk2Enet->hRxPacketSem);

        while (Task_Mode_TERMINATED != Task_getMode(hNdk2Enet->hRxPacketTask))
        {
            /* Sleep so Rx packet task gets scheduled & terminates itself */
            Task_sleep(1U);
        }
    }

    /* Close TX channel */
    /* Post to tx packet task so that it will terminate (shutDownFlag flag is already set) */
    if (hNdk2Enet->hTxPacketTask != NULL)
    {
        Semaphore_post(hNdk2Enet->hTxPacketSem);

        while (Task_Mode_TERMINATED != Task_getMode(hNdk2Enet->hTxPacketTask))
        {
            /* Sleep so Tx packet task gets scheduled & terminates itself */
            Task_sleep(1U);
        }
    }
}

static void Ndk2Enet_freeTxPktCb(void *cbArg,
                                 EnetDma_PktQ *fqPktInfoQ,
                                 EnetDma_PktQ *cqPktInfoQ)
{
    Ndk2Enet_Handle hNdk2Enet = (Ndk2Enet_Handle)cbArg;

    EnetQueue_append(&hNdk2Enet->txFreePktInfoQ, fqPktInfoQ);
    Ndk2EnetStats_addNum(&gNdk2EnetStats.txFreeAppPktEnq, EnetQueue_getQCount(fqPktInfoQ));
    Ndk2Enet_freePBMPackets(fqPktInfoQ);

    EnetQueue_append(&hNdk2Enet->txFreePktInfoQ, cqPktInfoQ);
    Ndk2EnetStats_addNum(&gNdk2EnetStats.txFreeAppPktEnq, EnetQueue_getQCount(cqPktInfoQ));
    Ndk2Enet_freePBMPackets(cqPktInfoQ);
}

static void Ndk2Enet_freeRxPktCb(void *cbArg,
                                 EnetDma_PktQ *fqPktInfoQ,
                                 EnetDma_PktQ *cqPktInfoQ)
{
    Ndk2Enet_Handle hNdk2Enet = (Ndk2Enet_Handle)cbArg;

    /* Now that we free PBM buffers, push all freed pktInfo's into Rx
     * free Q */
    EnetQueue_append(&hNdk2Enet->rxFreePktInfoQ, fqPktInfoQ);
    Ndk2EnetStats_addNum(&gNdk2EnetStats.rxFreeAppPktEnq, EnetQueue_getQCount(fqPktInfoQ));
    Ndk2EnetStats_addNum(&gNdk2EnetStats.rxPBMFreeCnt, EnetQueue_getQCount(fqPktInfoQ));
    Ndk2Enet_freePBMPackets(fqPktInfoQ);

    EnetQueue_append(&hNdk2Enet->rxFreePktInfoQ, cqPktInfoQ);
    Ndk2EnetStats_addNum(&gNdk2EnetStats.rxFreeAppPktEnq, EnetQueue_getQCount(cqPktInfoQ));
    Ndk2EnetStats_addNum(&gNdk2EnetStats.rxPBMFreeCnt, EnetQueue_getQCount(fqPktInfoQ));
    Ndk2Enet_freePBMPackets(cqPktInfoQ);

    /* Flush out our pending receive queues */
    while (PBMQ_count(&hNdk2Enet->rxReadyPBMPktQ) != 0)
    {
        PBM_free(PBMQ_deq(&hNdk2Enet->rxReadyPBMPktQ));
        Ndk2EnetStas_addOne(&gNdk2EnetStats.rxReadyPBMPktDq);
        Ndk2EnetStas_addOne(&gNdk2EnetStats.rxPBMFreeCnt);
    }
}

static void Ndk2Enet_retrieveTxPkts(Ndk2Enet_Handle hNdk2Enet)
{

    EnetDma_PktQ tempQueue;
    uint32_t packetCount = 0;
    int32_t retVal;

    Ndk2EnetStas_addOne(&gNdk2EnetStats.txStats.rawNotifyCnt);
    packetCount = 0;

    /* Retrieve the used (sent/empty) packets from the channel */
    {
        EnetQueue_initQ(&tempQueue);
        /* Retrieve all TX packets and keep them locally */
        retVal = EnetDma_retrieveTxPktQ(hNdk2Enet->appInfo.txInfo.hTxChannel,
                                               &tempQueue);
        if (ENET_SOK != retVal)
        {
            Ndk2Enet_print("Ndk2Enet_retrieveTxPkts: Failed to retrieve TX pkts: %d\n", retVal);
        }
    }

    if (tempQueue.count != 0U)
    {
        /*
         * Get all used Tx DMA packets from the hardware, then return those
         * buffers to the txFreePktQ so they can be used later to send with.
         */
        packetCount = Ndk2Enet_prepTxPktQ(hNdk2Enet, &tempQueue);
    }
    else
    {
        Ndk2EnetStas_addOne(&gNdk2EnetStats.txStats.zeroNotifyCnt);
    }

    if (packetCount != 0)
    {
        Ndk2Enet_updateTxNotifyStats(packetCount, 0U);
    }

}

static Void Ndk2Enet_timerCb(UArg arg)
{
    /* Post semaphore to rx handling task */
    Ndk2Enet_Handle hNdk2Enet = (Ndk2Enet_Handle)arg;

    if ((hNdk2Enet->initDone) && (hNdk2Enet->shutDownFlag == false))
    {
        Semaphore_post(hNdk2Enet->hRxPacketSem);
    }
}

static void Ndk2Enet_createTimer(Ndk2Enet_Handle hNdk2Enet)
{
    Timer_Params timerPrms;

    Timer_Params_init(&timerPrms);
    timerPrms.periodType = ITimer_PeriodType_MICROSECS;
    timerPrms.period     = hNdk2Enet->appInfo.timerPeriodUs;
    timerPrms.runMode    = Timer_RunMode_ONESHOT;
    timerPrms.startMode  = Timer_StartMode_USER;
    timerPrms.arg        = (UArg)hNdk2Enet;

    hNdk2Enet->hPacingTimer =  Timer_create(Timer_ANY  ,
                                            Ndk2Enet_timerCb,
                                            &timerPrms,
                                            NULL);
    Ndk2Enet_assert(hNdk2Enet->hPacingTimer != NULL);
}

static void  Ndk2Enet_initGetHandleInArgs(Ndk2Enet_Handle hNdk2Enet,
                                          NimuEnetAppIf_GetHandleInArgs *inArgs)
{
    inArgs->txCfg.cbArg      = hNdk2Enet;
    inArgs->txCfg.notifyCb   = &Ndk2Enet_notifyTxPackets;
    inArgs->txCfg.numPackets = NDK2ENET_TX_PACKETS;

    inArgs->rxCfg.cbArg      = hNdk2Enet;
    inArgs->rxCfg.notifyCb   = &Ndk2Enet_notifyRxPackets;
    inArgs->rxCfg.numPackets = NDK2ENET_RX_PACKETS;
}

static void Ndk2Enet_initReleaseHandleInArgs(Ndk2Enet_Handle hNdk2Enet,
                                             NimuEnetAppIf_ReleaseHandleInfo *inArgs)
{
    inArgs->coreId       = hNdk2Enet->appInfo.coreId;
    inArgs->coreKey      = hNdk2Enet->appInfo.coreKey;
    inArgs->hEnet        = hNdk2Enet->appInfo.hEnet;
#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
    inArgs->hUdmaDrv     = hNdk2Enet->appInfo.hUdmaDrv;
#endif
    inArgs->rxFreePktCb  = &Ndk2Enet_freeRxPktCb;
    inArgs->rxInfo       = hNdk2Enet->appInfo.rxInfo;
    inArgs->txFreePktCb  = &Ndk2Enet_freeTxPktCb;
    inArgs->txInfo       = hNdk2Enet->appInfo.txInfo;
    inArgs->freePktCbArg = hNdk2Enet;
}
