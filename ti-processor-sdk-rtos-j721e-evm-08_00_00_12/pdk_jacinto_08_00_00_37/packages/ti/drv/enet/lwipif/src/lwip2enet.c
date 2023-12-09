/**
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/**
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
 *
 * This file is dervied from the ``ethernetif.c'' skeleton Ethernet network
 * interface driver for lwIP.
 */

/* Standard language headers */
#include <stdio.h>
#include <assert.h>
#include <string.h>

/* xdc header - should be first file included as due to order dependency */

/* OS/Posix headers */

/**
 * lwIP specific header files
 */
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip2enet.h"

/**
 * PDK header files
 */
#include <ti/drv/enet/enet.h>
#include <ti/drv/enet/include/core/enet_utils.h>
#include <ti/drv/enet/enet_cfg.h>

#include "lwip2lwipif_priv.h"


/*---------------------------------------------------------------------------*\
 |                             Extern Declarations                             |
 \*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*\
 |                            Local Macros/Defines                             |
 \*---------------------------------------------------------------------------*/

#define OS_TASKPRIHIGH              8

#define LWIPIF_RX_PACKET_TASK_PRI      (OS_TASKPRIHIGH)

#define LWIPIF_TX_PACKET_TASK_PRI      (OS_TASKPRIHIGH)

#define LWIP2ENET_DIVIDER_US_TO_MS  (1000U)

/*---------------------------------------------------------------------------*\
 |                         Local Function Declarations                         |
 \*---------------------------------------------------------------------------*/

static void Lwip2Enet_initRxObj(Lwip2Enet_Handle hLwip2Enet,
                                Lwip2Enet_RxObj *rx,
                                uint32_t numPkts);

static void Lwip2Enet_deinitRxObj(Lwip2Enet_Handle hLwip2Enet,
                                  Lwip2Enet_RxObj *rx);

static void Lwip2Enet_saveAppIfRxCfg(Lwip2Enet_RxObj *rx,
                                     LwipifEnetAppIf_RxHandleInfo *rxInfo);

static void Lwip2Enet_prepRxQs(Lwip2Enet_RxObj *rx);

static void Lwip2Enet_stopRxTask(Lwip2Enet_RxObj *rx);

static void Lwip2Enet_initTxObj(Lwip2Enet_Handle hLwip2Enet,
                                Lwip2Enet_TxObj *tx,
                                uint32_t numPkts);

static void Lwip2Enet_saveAppIfTxCfg(Lwip2Enet_TxObj *tx,
                                     LwipifEnetAppIf_TxHandleInfo *txInfo);

static void Lwip2Enet_prepTxQs(Lwip2Enet_TxObj *tx);

static void Lwip2Enet_stopTxTask(Lwip2Enet_TxObj *tx);

static void Lwip2Enet_deinitTxObj(Lwip2Enet_Handle hLwip2Enet,
                                  Lwip2Enet_TxObj *tx);

static void Lwip2Enet_notifyRxPackets(void *cbArg);

static void Lwip2Enet_notifyTxPackets(void *cbArg);

static void Lwip2Enet_rxPacketTask(void *arg0);

static void Lwip2Enet_pbufQ2PktInfoQ(Lwip2Enet_TxObj *tx,
                                     pbufQ *pbufPktQ,
                                     EnetDma_PktQ *pDmaPktInfoQ);

static void Lwip2Enet_pktInfoQ2PbufQ(EnetDma_PktQ *pDmaPktInfoQ,
                                     pbufQ *pbufPktQ);

static void Lwip2Enet_allocRxPackets(Lwip2Enet_RxObj *rx);

static uint32_t Lwip2Enet_prepRxPktQ(Lwip2Enet_RxObj *rx,
                                     EnetDma_PktQ *pPktQ);

static void Lwip2Enet_submitRxPktQ(Lwip2Enet_RxObj *rx);

static void Lwip2Enet_txPacketTask(void *arg0);

static uint32_t Lwip2Enet_prepTxPktQ(Lwip2Enet_TxObj *tx,
                                    EnetDma_PktQ *pPktQ);

static void Lwip2Enet_freePbufPackets(EnetDma_PktQ *tempQueue);

static void Lwip2Enet_updateRxNotifyStats(Lwip2Enet_PktTaskStats *pktTaskStats,
                                          uint32_t packetCount,
                                          uint32_t timeDiff);

static void Lwip2Enet_updateTxNotifyStats(Lwip2Enet_PktTaskStats *pktTaskStats,
                                          uint32_t packetCount,
                                          uint32_t timeDiff);

static void Lwip2Enet_print(const char *prnStr,
                           ...);

static int32_t Lwip2Enet_startRxTx(Lwip2Enet_Handle hLwip2Enet);

static void Lwip2Enet_stopRxTx(Lwip2Enet_Handle hLwip2Enet);

static void Lwip2Enet_submitTxPackets(Lwip2Enet_TxObj *tx,
                                      EnetDma_PktQ *pSubmitQ);

static void Lwip2Enet_submitRxPackets(Lwip2Enet_RxObj *rx,
                                      EnetDma_PktQ *pSubmitQ);

static void Lwip2Enet_retrieveTxPkts(Lwip2Enet_TxObj *tx);

static void Lwip2Enet_timerCb(void * arg);

static void Lwip2Enet_createTimer(Lwip2Enet_Handle hLwip2Enet);

static void Lwip2Enet_initGetHandleInArgs(Lwip2Enet_Handle hLwip2Enet,
                                         LwipifEnetAppIf_GetHandleInArgs *inArgs);

static void Lwip2Enet_initReleaseHandleInArgs(Lwip2Enet_Handle hLwip2Enet,
                                             LwipifEnetAppIf_ReleaseHandleInfo *inArgs);

/*---------------------------------------------------------------------------*\
 |                         Local Variable Declarations                         |
 \*---------------------------------------------------------------------------*/

static Lwip2Enet_Object gLwip2EnetObj;

/*---------------------------------------------------------------------------*\
 |                         Global Variable Declarations                        |
 \*---------------------------------------------------------------------------*/

Lwip2Enet_Stats gLwip2EnetStats;

/**
 * Initializes Ethernet peripheral hardware
 */
Lwip2Enet_Handle Lwip2Enet_open(struct netif *netif)
{
    LwipifEnetAppIf_GetHandleInArgs getHandleInArgs;
    Lwip2Enet_Handle hLwip2Enet;
    uint32_t i;
    int32_t status;

    hLwip2Enet = &gLwip2EnetObj;

    /* Clear instrumentation statistics structure */
    memset(&gLwip2EnetStats, 0, sizeof(Lwip2Enet_Stats));

    /* Initialize the allocated memory block. */
    memset(hLwip2Enet, 0, sizeof(Lwip2Enet_Object));

    /* lwIP interface relevant for this adaptation layer */
    hLwip2Enet->netif = netif;

    /* First init tasks, semaphores and clocks. This is required because
     * EnetDma event cb ISR can happen immediately after opening rx flow
     * in LwipifEnetAppCb_getHandle and all semaphores, clocks and tasks should
     * be valid at that point
     */

    /* Create semaphore objects, init shutDownFlag status */
    hLwip2Enet->shutDownFlag = false;

    hLwip2Enet->hShutDownSem = SemaphoreP_create(0U, NULL);
    Lwip2Enet_assert(NULL != hLwip2Enet->hShutDownSem);

    /* Initialize RX object, start RX task, create semaphores, etc */
    for (i = 0U; i < ENET_ARRAYSIZE(hLwip2Enet->rx); i++)
    {
        Lwip2Enet_initRxObj(hLwip2Enet, &hLwip2Enet->rx[i], LWIP2ENET_RX_PACKETS);
    }

    /* Initialize TX object, start TX task, create semaphores, etc */
    Lwip2Enet_initTxObj(hLwip2Enet, &hLwip2Enet->tx, LWIP2ENET_TX_PACKETS);

    /* Clear channel/flow handles so app populates only those it actually uses */
    for (i = 0U; i < ENET_ARRAYSIZE(hLwip2Enet->rx); i++)
    {
        hLwip2Enet->appInfo.rxInfo[i].hRxFlow = NULL;
    }

    /* Get Enet & DMA Drv Handle */
    Lwip2Enet_initGetHandleInArgs(hLwip2Enet, &getHandleInArgs);
    LwipifEnetAppCb_getHandle(&getHandleInArgs, &hLwip2Enet->appInfo);

    /* Save params received from application interface */
    for (i = 0U; i < ENET_ARRAYSIZE(hLwip2Enet->rx); i++)
    {
        Lwip2Enet_saveAppIfRxCfg(&hLwip2Enet->rx[i], &hLwip2Enet->appInfo.rxInfo[i]);
    }
    Lwip2Enet_saveAppIfTxCfg(&hLwip2Enet->tx, &hLwip2Enet->appInfo.txInfo);

    Lwip2Enet_assert(hLwip2Enet->appInfo.hEnet != NULL);
#if !(defined(SOC_TPR12) || defined(SOC_AWR294X))
    Lwip2Enet_assert(hLwip2Enet->appInfo.hUdmaDrv != NULL);
#endif
    Lwip2Enet_assert(hLwip2Enet->appInfo.isPortLinkedFxn != NULL);

    if (NULL != hLwip2Enet->appInfo.print)
    {
        /* set the print function callback if not null */
        hLwip2Enet->print = hLwip2Enet->appInfo.print;
    }
    else
    {
        hLwip2Enet->print = (Enet_Print) &EnetUtils_printf;
    }

    /* Open DMA driver */
    status = Lwip2Enet_startRxTx(hLwip2Enet);
    if (status != ENET_SOK)
    {
        Lwip2Enet_print("Failed to open DMA: %d\n", status);
    }

    /* Get initial link/interface status from the driver */
    hLwip2Enet->linkIsUp = hLwip2Enet->appInfo.isPortLinkedFxn(hLwip2Enet->appInfo.hEnet);

    if (hLwip2Enet->tx.disableEvent)
    {
        EnetDma_disableTxEvent(hLwip2Enet->tx.hCh);
    }

    for (i = 0U; i < ENET_ARRAYSIZE(hLwip2Enet->rx); i++)
    {
        if (hLwip2Enet->rx[i].enabled &&
            hLwip2Enet->rx[i].disableEvent)
        {
            EnetDma_disableRxEvent(hLwip2Enet->rx[i].hFlow);
        }
    }

    /* assert if clk period is not valid  */
    Lwip2Enet_assert(0U != hLwip2Enet->appInfo.timerPeriodUs);
    Lwip2Enet_createTimer(hLwip2Enet);

    hLwip2Enet->initDone = TRUE;

    return hLwip2Enet;
}

/*!
 *  @b Lwip2Enet_close
 *  @n
 *      Closes Ethernet peripheral and disables interrupts.
 *
 *  \param[in]  hLwip2Enet
 *      Lwip2Enet_object structure pointer.
 *
 *  \retval
 *      void
 */
void Lwip2Enet_close(Lwip2Enet_Handle hLwip2Enet)
{
    LwipifEnetAppIf_ReleaseHandleInfo releaseHandleInfo;
    uint32_t i;

    Lwip2Enet_assert(NULL != hLwip2Enet);
    Lwip2Enet_assert(NULL != hLwip2Enet->tx.hPktSem);
    Lwip2Enet_assert(NULL != hLwip2Enet->hShutDownSem);

    for (i = 0U; i < ENET_ARRAYSIZE(hLwip2Enet->rx); i++)
    {
        Lwip2Enet_assert(NULL != hLwip2Enet->rx[i].hPktSem);
    }

    /* Set the translation layer shutdown flag */
    hLwip2Enet->shutDownFlag = true;

    /* Stop and delete the tick timer */
    Lwip2Enet_assert(NULL != hLwip2Enet->hPacingTimer);
    ClockP_stop(hLwip2Enet->hPacingTimer);
    ClockP_delete(hLwip2Enet->hPacingTimer);

    Lwip2Enet_stopRxTx(hLwip2Enet);

    /* Close RX flow and TX channel */
    Lwip2Enet_initReleaseHandleInArgs(hLwip2Enet, &releaseHandleInfo);
    LwipifEnetAppCb_releaseHandle(&releaseHandleInfo);

    if (hLwip2Enet->allocPktInfo > 0U)
    {
        Lwip2Enet_print("Lwip2Enet_close() failed to retrieve all PktInfo\n");
    }

    /* Deinit TX and RX objects, delete task, semaphore, etc */
    for (i = 0U; i < ENET_ARRAYSIZE(hLwip2Enet->rx); i++)
    {
        Lwip2Enet_deinitRxObj(hLwip2Enet, &hLwip2Enet->rx[i]);
    }
    Lwip2Enet_deinitTxObj(hLwip2Enet, &hLwip2Enet->tx);

    SemaphoreP_delete(&hLwip2Enet->hShutDownSem);

    /* Clear the allocated translation */
    memset(hLwip2Enet, 0, sizeof(Lwip2Enet_Object));
}

static void Lwip2Enet_initRxObj(Lwip2Enet_Handle hLwip2Enet,
                                Lwip2Enet_RxObj *rx,
                                uint32_t numPkts)
{
    SemaphoreP_Params semParams;
    TaskP_Params taskParams;

    rx->hLwip2Enet = hLwip2Enet;
    rx->numPkts = numPkts;
    Lwip2Enet_assert(rx->numPkts <= ENET_ARRAYSIZE(rx->pktInfoMem));

    /* Create RX packet semaphore */
    SemaphoreP_Params_init(&semParams);
    semParams.mode = SemaphoreP_Mode_BINARY;

    rx->hPktSem = SemaphoreP_create(0U, &semParams);
    Lwip2Enet_assert(NULL != rx->hPktSem);

    /* Create RX packet task */
    TaskP_Params_init(&taskParams);
    taskParams.name      = (uint8_t *)"Lwip2Enet_RxPacketTask";
    taskParams.priority  = LWIPIF_RX_PACKET_TASK_PRI;
    taskParams.stack     = &rx->pktTaskStack[0U];
    taskParams.stacksize = sizeof(rx->pktTaskStack);
    taskParams.arg0      = rx;

    rx->hPktTask = TaskP_create((void *)&Lwip2Enet_rxPacketTask, &taskParams);
    Lwip2Enet_assert(NULL != rx->hPktTask);
}

static void Lwip2Enet_saveAppIfRxCfg(Lwip2Enet_RxObj *rx,
                                     LwipifEnetAppIf_RxHandleInfo *rxInfo)
{
    rx->flowStartIdx = rxInfo->rxFlowStartIdx;
    rx->flowIdx      = rxInfo->rxFlowIdx;
    rx->hFlow        = rxInfo->hRxFlow;
    rx->enabled      = (rx->hFlow != NULL);
    rx->disableEvent = rxInfo->disableEvent;
    EnetUtils_copyMacAddr(&rx->macAddr[0U], &rxInfo->macAddr[0U]);

#if defined(LWIPIF_APP_RX_PKT_HANDLING)
    rx->handlePktFxn = rxInfo->handlePktFxn;
#endif
}

static void Lwip2Enet_prepRxQs(Lwip2Enet_RxObj *rx)
{
    Lwip2Enet_Handle hLwip2Enet = rx->hLwip2Enet;
    EnetDma_Pkt *pPktInfo;
    uint32_t i;

    /* Initialize queue of RX packet info */
    EnetQueue_initQ(&rx->freePktInfoQ);

    /* Allocate the free pkt info Q. This is used to exchange buffer info from
     * DMA basically for submitting free packets and retrieving ready packets */
    for (i = 0U; i < rx->numPkts; i++)
    {
        pPktInfo = &rx->pktInfoMem[i];

        EnetDma_initPktInfo(pPktInfo);
        EnetQueue_enq(&rx->freePktInfoQ, &pPktInfo->node);

        Lwip2EnetStats_addOne(&rx->stats.freeAppPktEnq);
        hLwip2Enet->allocPktInfo++;
    }

    /* Initialize the RX pbuf queue */
    pbufQ_init(&rx->freePbufQ);

    /* Allocate the pbufs for the RX channel, primes freePbufQ */
    Lwip2Enet_allocRxPackets(rx);
}

static void Lwip2Enet_stopRxTask(Lwip2Enet_RxObj *rx)
{
    /* Post to rx packet task so that it will terminate (shutDownFlag flag is already set) */
    if (rx->hPktTask != NULL)
    {
        SemaphoreP_post(rx->hPktSem);

        while (1 != TaskP_isTerminated(rx->hPktTask))
        {
            /* Sleep so Rx packet task gets scheduled & terminates itself */
            TaskP_sleep(1U);
        }
    }
}

static void Lwip2Enet_deinitRxObj(Lwip2Enet_Handle hLwip2Enet,
                                  Lwip2Enet_RxObj *rx)
{
    rx->hFlow = NULL;

    hLwip2Enet->allocPktInfo -= EnetQueue_getQCount(&rx->freePktInfoQ);

    SemaphoreP_pend(hLwip2Enet->hShutDownSem, SemaphoreP_WAIT_FOREVER);

    SemaphoreP_delete(&rx->hPktSem);
}

static void Lwip2Enet_initTxObj(Lwip2Enet_Handle hLwip2Enet,
                                Lwip2Enet_TxObj *tx,
                                uint32_t numPkts)
{
    SemaphoreP_Params semParams;
    TaskP_Params taskParams;

    tx->hLwip2Enet = hLwip2Enet;
    tx->numPkts = numPkts;
    Lwip2Enet_assert(tx->numPkts <= ENET_ARRAYSIZE(tx->pktInfoMem));

    /* Create TX packet semaphore */
    SemaphoreP_Params_init(&semParams);
    semParams.mode = SemaphoreP_Mode_BINARY;

    tx->hPktSem = SemaphoreP_create(0U, &semParams);
    Lwip2Enet_assert(NULL != tx->hPktSem);

    /* Create TX packet task */
    TaskP_Params_init(&taskParams);
    taskParams.name      = (uint8_t *)"Lwip2Enet_TxPacketTask";
    taskParams.priority  = LWIPIF_TX_PACKET_TASK_PRI;
    taskParams.stack     = &tx->pktTaskStack[0U];
    taskParams.stacksize = sizeof(tx->pktTaskStack);
    taskParams.arg0      = tx;

    tx->hPktTask = TaskP_create((void *)&Lwip2Enet_txPacketTask, &taskParams);
    Lwip2Enet_assert(NULL != tx->hPktTask);
}

static void Lwip2Enet_saveAppIfTxCfg(Lwip2Enet_TxObj *tx,
                                     LwipifEnetAppIf_TxHandleInfo *txInfo)
{
    tx->hCh   = txInfo->hTxChannel;
    tx->chNum = txInfo->txChNum;
    tx->disableEvent = txInfo->disableEvent;

    Lwip2Enet_assert(tx->hCh != NULL);
}

static void Lwip2Enet_prepTxQs(Lwip2Enet_TxObj *tx)
{
    Lwip2Enet_Handle hLwip2Enet = tx->hLwip2Enet;
    EnetDma_Pkt *pPktInfo;
    uint32_t i;

    /* Initialize the DMA free queue */
    EnetQueue_initQ(&tx->freePktInfoQ);

    /* Allocate the free pkt info Q. This is used to exchange buffer info from
     * DMA basically for submitting free packets and retrieving ready packets */
    for (i = 0U; i < tx->numPkts; i++)
    {
        /* Initialize Pkt info Q from allocated memory */
        pPktInfo = &tx->pktInfoMem[i];

        EnetDma_initPktInfo(pPktInfo);
        EnetQueue_enq(&tx->freePktInfoQ, &pPktInfo->node);

        Lwip2EnetStats_addOne(&tx->stats.freeAppPktEnq);
        hLwip2Enet->allocPktInfo++;
    }

    /* Initialize the TX pbuf queues */
    pbufQ_init(&tx->readyPbufQ);
    pbufQ_init(&tx->unusedPbufQ);
}

static void Lwip2Enet_stopTxTask(Lwip2Enet_TxObj *tx)
{
    /* Post to tx packet task so that it will terminate (shutDownFlag flag is already set) */
    if (tx->hPktTask != NULL)
    {
        SemaphoreP_post(tx->hPktSem);

        while (1 != TaskP_isTerminated(tx->hPktTask))
        {
            /* Sleep so Tx packet task gets scheduled & terminates itself */
            TaskP_sleep(1U);
        }
    }
}

static void Lwip2Enet_deinitTxObj(Lwip2Enet_Handle hLwip2Enet,
                                  Lwip2Enet_TxObj *tx)
{
    struct pbuf *pbuf;

    tx->hCh = NULL;

    /* Free pbuf in ready queue */
    while (pbufQ_count(&tx->readyPbufQ) != 0U)
    {
        pbuf = pbufQ_deQ(&tx->readyPbufQ);
        Lwip2Enet_assert(NULL != pbuf);
        pbuf_free(pbuf);
    }

    hLwip2Enet->allocPktInfo -= EnetQueue_getQCount(&tx->freePktInfoQ);

    SemaphoreP_pend(hLwip2Enet->hShutDownSem, SemaphoreP_WAIT_FOREVER);

    SemaphoreP_delete(&tx->hPktSem);
}

/*!
 *  @b Lwip2Enet_sendTxPackets
 *  @n
 *      Routine to send out queued Tx packets to the hardware driver
 *
 *  \param[in]  hLwip2Enet
 *      Lwip2Enet_object structure pointer.
 *
 *  \retval
 *      void
 */
void Lwip2Enet_sendTxPackets(Lwip2Enet_TxObj *tx)
{
    Lwip2Enet_Handle hLwip2Enet = tx->hLwip2Enet;
    EnetDma_Pkt *pCurrDmaPacket;
    struct pbuf *hPbufPkt;

    /* If link is not up, simply return */
    if (hLwip2Enet->linkIsUp)
    {
        EnetDma_PktQ txSubmitQ;

        EnetQueue_initQ(&txSubmitQ);

        if (pbufQ_count(&tx->unusedPbufQ))
        {
            /* send any pending TX Q's */
            Lwip2Enet_pbufQ2PktInfoQ(tx, &tx->unusedPbufQ, &txSubmitQ);
        }

        /* Check if there is anything to transmit, else simply return */
        while (pbufQ_count(&tx->readyPbufQ) != 0)
        {
            /* Dequeue one free TX Eth packet */
            pCurrDmaPacket = (EnetDma_Pkt *)EnetQueue_deq(&tx->freePktInfoQ);

            if (pCurrDmaPacket == NULL)
            {
                /* If we run out of packet info Q, retrieve packets from HW
                * and try to dequeue free packet again */
                Lwip2Enet_retrieveTxPkts(tx);
                pCurrDmaPacket = (EnetDma_Pkt *)EnetQueue_deq(&tx->freePktInfoQ);
            }

            if (NULL != pCurrDmaPacket)
            {
                hPbufPkt = pbufQ_deQ(&tx->readyPbufQ);
                EnetDma_initPktInfo(pCurrDmaPacket);

                pCurrDmaPacket->bufPtr     = (uint8_t *) hPbufPkt->payload;
                pCurrDmaPacket->appPriv    = hPbufPkt;
                pCurrDmaPacket->orgBufLen  = PBUF_POOL_BUFSIZE;
                pCurrDmaPacket->userBufLen = hPbufPkt->len;
                pCurrDmaPacket->node.next  = NULL;

#ifdef LWIPIF_CHECKSUM_SUPPORT
                /* TODO: Add checksum support */
#endif
                ENET_UTILS_COMPILETIME_ASSERT(offsetof(EnetDma_Pkt, node) == 0);
                EnetQueue_enq(&txSubmitQ, &(pCurrDmaPacket->node));

                Lwip2EnetStats_addOne(&tx->stats.freeAppPktDeq);
                Lwip2EnetStats_addOne(&tx->stats.readyPbufPktDeq);
            }
            else
            {
                break;
            }
        }

        /* Submit the accumulated packets to the hardware for transmission */
        Lwip2Enet_submitTxPackets(tx, &txSubmitQ);
    }
}

/*---------------------------------------------------------------------------*\
 |                           Local Function Definitions                        |
 \*---------------------------------------------------------------------------*/

 void Lwip2Enet_periodicFxn(Lwip2Enet_Handle hLwip2Enet)
{
    uint32_t prevLinkState     = hLwip2Enet->linkIsUp;
    uint32_t prevLinkInterface = hLwip2Enet->currLinkedIf;
    uint32_t i;

#if (1U == ENET_CFG_DEV_ERROR)
    for (i = 0U; i < ENET_ARRAYSIZE(hLwip2Enet->rx); i++)
    {
        if (hLwip2Enet->rx[i].enabled)
        {
            EnetQueue_verifyQCount(&hLwip2Enet->rx[i].freePktInfoQ);
        }
    }
    EnetQueue_verifyQCount(&hLwip2Enet->tx.freePktInfoQ);

#if !(defined(SOC_TPR12) || defined(SOC_AWR294X))
    {
    	int32_t status;

        for (i = 0U; i < ENET_ARRAYSIZE(hLwip2Enet->rx); i++)
        {
            if (hLwip2Enet->rx[i].enabled)
            {
                status = EnetUdma_checkRxFlowSanity(hLwip2Enet->rx[i].hFlow, 5U);
                if (status != ENET_SOK)
                {
                    Lwip2Enet_print("EnetUdma_checkRxFlowSanity Failed\n");
                }
            }
        }

        status = EnetUdma_checkTxChSanity(hLwip2Enet->tx.hCh, 5U);
        if (status != ENET_SOK)
        {
            Lwip2Enet_print("EnetUdma_checkTxChSanity Failed\n");
    	}
    }
#endif
#endif

    /*
     * Return the same DMA packets back to the DMA channel (but now
     * associated with a new PBUF Packet and buffer)
     */
    for (i = 0U; i < ENET_ARRAYSIZE(hLwip2Enet->rx); i++)
    {
        if (hLwip2Enet->rx[i].enabled)
        {
            if (pbufQ_count(&hLwip2Enet->rx[i].freePbufQ) != 0U)
            {
                Lwip2Enet_submitRxPktQ(&hLwip2Enet->rx[i]);
            }
        }
    }

#if defined(LWIPIF_INSTRUMENTATION_ENABLED)
    static uint32_t loadCount = 0;
    Load_Stat stat;
    gLwip2EnetStats.gCpuLoad[loadCount] = Load_getCPULoad();

    if (Load_getGlobalHwiLoad(&stat))
    {
        gLwip2EnetStats.gHwiLoad[loadCount] = Load_calculateLoad(&stat);
    }

    for (i = 0U; i < ENET_ARRAYSIZE(hLwip2Enet->rx); i++)
    {
        if (Load_getTaskLoad(hLwip2Enet->rx[i].hPktTask, &stat))
        {
            hLwip2Enet->rx[i].stats.pktStats.taskLoad[loadCount] = Load_calculateLoad(&stat);
        }
    }

    if (Load_getTaskLoad(hLwip2Enet->tx.hPktTask, &stat))
    {
        hLwip2Enet->tx.stats.pktStats.taskLoad[loadCount] = Load_calculateLoad(&stat);
    }

    loadCount = (loadCount + 1U) & (HISTORY_CNT - 1U);
#endif

    /* Get current link status as reported by the hardware driver */
    hLwip2Enet->linkIsUp = hLwip2Enet->appInfo.isPortLinkedFxn(hLwip2Enet->appInfo.hEnet);

    /* If the interface changed, discard any queue packets (since the MAC would now be wrong) */
    if (prevLinkInterface != hLwip2Enet->currLinkedIf)
    {
        /* ToDo: Discard all queued packets */
    }

    /* If link status changed from down->up, then send any queued packets */
    if ((prevLinkState == 0) && (hLwip2Enet->linkIsUp))
    {
        Lwip2Enet_sendTxPackets(&hLwip2Enet->tx);
    }
}

static void Lwip2Enet_processRxUnusedQ(Lwip2Enet_RxObj *rx,
                                       EnetDma_PktQ *unUsedQ)
{
    EnetDma_Pkt *pDmaPacket;

    pDmaPacket = (EnetDma_Pkt *)EnetQueue_deq(unUsedQ);
    while (pDmaPacket != NULL)
    {
        /* Get the full PBUF packet that needs to be returned to the rx.freePbufQ */
        struct pbuf* hPbufPacket = (struct pbuf *)pDmaPacket->appPriv;
        if (hPbufPacket)
        {
            /* Enqueue the received packet to rx.freePbufQ*/
            pbufQ_enQ(&rx->freePbufQ, hPbufPacket);

            /* Put packet info into free Q as we have removed the Pbuf buffers
             * from the it */
            EnetQueue_enq(&rx->freePktInfoQ, &pDmaPacket->node);
            Lwip2EnetStats_addOne(&rx->stats.freeAppPktEnq);

            pDmaPacket = (EnetDma_Pkt *)EnetQueue_deq(unUsedQ);
        }
        else
        {
            /* should never happen as this is received from HW */
            Lwip2Enet_assert(FALSE);
        }
    }
}

static void Lwip2Enet_submitRxPackets(Lwip2Enet_RxObj *rx,
                                      EnetDma_PktQ *pSubmitQ)
{
    int32_t retVal;

    retVal = EnetDma_submitRxPktQ(rx->hFlow, pSubmitQ);
    if (ENET_SOK != retVal)
    {
        Lwip2Enet_print("EnetDma_submitRxPktQ: failed to submit pkts: %d\n", retVal);
    }

    if (EnetQueue_getQCount(pSubmitQ))
    {
        /* Copy unused packets to back to readyQ */
        Lwip2Enet_processRxUnusedQ(rx, pSubmitQ);
    }
}

/* May lead to infinite loop if no free memory
 * available*/
static void Lwip2Enet_pbufQ2PktInfoQ(Lwip2Enet_TxObj *tx,
                                     pbufQ *pbufPktQ,
                                     EnetDma_PktQ *pDmaPktInfoQ)
{
    EnetDma_Pkt *pCurrDmaPacket;
    struct pbuf *hPbufPkt = NULL;

    while(pbufQ_count(pbufPktQ) != 0U)
    {

        /* Dequeue one free TX Eth packet */
        pCurrDmaPacket = (EnetDma_Pkt *)EnetQueue_deq(&tx->freePktInfoQ);
        if (pCurrDmaPacket == NULL)
        {
        /* If we run out of packet info Q, retrieve packets from HW
            * and try to dequeue free packet again */
        Lwip2Enet_retrieveTxPkts(tx);
        pCurrDmaPacket = (EnetDma_Pkt *)EnetQueue_deq(&tx->freePktInfoQ);
        }

        if (NULL != pCurrDmaPacket)
        {
            hPbufPkt = pbufQ_deQ(pbufPktQ);
            EnetDma_initPktInfo(pCurrDmaPacket);

            pCurrDmaPacket->bufPtr = (uint8_t *) hPbufPkt->payload;
            pCurrDmaPacket->appPriv    = hPbufPkt;
            pCurrDmaPacket->orgBufLen  = PBUF_POOL_BUFSIZE;
            pCurrDmaPacket->userBufLen = hPbufPkt->len;
            pCurrDmaPacket->node.next = NULL;

            ENET_UTILS_COMPILETIME_ASSERT(offsetof(EnetDma_Pkt, node) == 0);
            EnetQueue_enq(pDmaPktInfoQ, &(pCurrDmaPacket->node));

            Lwip2EnetStats_addOne(&tx->stats.freeAppPktDeq);
        }
        else
        {
            break;
        }
    }
}

static void Lwip2Enet_pktInfoQ2PbufQ(EnetDma_PktQ *pDmaPktInfoQ,
                                     pbufQ *pbufPktQ)
{
    EnetDma_Pkt *pDmaPacket;
    struct pbuf *hPbufPacket;

    while (EnetQueue_getQCount(pDmaPktInfoQ) != 0U)
    {
        pDmaPacket = (EnetDma_Pkt *)EnetQueue_deq(pDmaPktInfoQ);
        hPbufPacket = (struct pbuf *)(pDmaPacket->appPriv);

        Lwip2Enet_assert(hPbufPacket != NULL);
        /*Don't want to make a copy, since it would cause waste memory*/
        pbufQ_enQ(pbufPktQ, hPbufPacket);
    }
}

static void Lwip2Enet_submitTxPackets(Lwip2Enet_TxObj *tx,
                                      EnetDma_PktQ *pSubmitQ)
{
    int32_t retVal;

    retVal = EnetDma_submitTxPktQ(tx->hCh, pSubmitQ);
    if (ENET_SOK != retVal)
    {
        Lwip2Enet_print("EnetDma_submitTxPktQ: failed to submit pkts: %d\n", retVal);
    }

    if (EnetQueue_getQCount(pSubmitQ))
    {
        /* TODO: txUnUsedPBMPktQ is needed for packets that were not able to be
         *       submitted to driver.  It can be removed if stack supported any
         *       mechanism to enqueue them to the head of the queue. */
        Lwip2Enet_pktInfoQ2PbufQ(pSubmitQ, &tx->unusedPbufQ);
        EnetQueue_append(&tx->freePktInfoQ, pSubmitQ);
        Lwip2EnetStats_addNum(&tx->stats.freeAppPktEnq, EnetQueue_getQCount(pSubmitQ));
    }
}

static void Lwip2Enet_freePbufPackets(EnetDma_PktQ *tempQueue)
{
    EnetDma_Pkt *pCurrDmaPacket;
    struct pbuf* hPbufPacket;

    while (EnetQueue_getQCount(tempQueue))
    {
        pCurrDmaPacket = (EnetDma_Pkt *)EnetQueue_deq(tempQueue);
        hPbufPacket     = (struct pbuf *)pCurrDmaPacket->appPriv;
        pbuf_free(hPbufPacket);
    }
}

static void Lwip2Enet_notifyRxPackets(void *cbArg)
{
    Lwip2Enet_RxObj *rx = (Lwip2Enet_RxObj *)cbArg;
    Lwip2Enet_Handle hLwip2Enet = rx->hLwip2Enet;

    /* do not post events if init not done or shutdown in progress */
    if ((hLwip2Enet->initDone) && (hLwip2Enet->shutDownFlag == false))
    {
        if (rx->enabled)
        {
            EnetDma_disableRxEvent(rx->hFlow);

            /* Post semaphore to RX handling task */
            SemaphoreP_post(rx->hPktSem);
        }
    }
}

static void Lwip2Enet_notifyTxPackets(void *cbArg)
{
    Lwip2Enet_TxObj *tx = (Lwip2Enet_TxObj *)cbArg;
    Lwip2Enet_Handle hLwip2Enet = tx->hLwip2Enet;

    /* do not post events if init not done or shutdown in progress */
    if ((hLwip2Enet->initDone) && (hLwip2Enet->shutDownFlag == false))
    {
        /* Post semaphore to TX handling task */
        SemaphoreP_post(tx->hPktSem);
    }
}

static void Lwip2Enet_rxPacketTask(void *arg0)
{
    Lwip2Enet_RxObj *rx = (Lwip2Enet_RxObj *)arg0;
    Lwip2Enet_Handle hLwip2Enet = rx->hLwip2Enet;
    EnetDma_PktQ tempQueue;
    int32_t retVal;
    uint32_t pktCnt;

    while (!hLwip2Enet->shutDownFlag)
    {
        /* Wait for the Rx ISR to notify us that packets are available with data */
        SemaphoreP_pend(rx->hPktSem, SemaphoreP_WAIT_FOREVER);

        if (hLwip2Enet->shutDownFlag)
        {
            /* This translation layer is shutting down, don't give anything else to the stack */
            break;
        }

        Lwip2EnetStats_addOne(&rx->stats.pktStats.rawNotifyCnt);
        pktCnt = 0;

        /* Retrieve the used (filled) packets from the channel */
        {
            EnetQueue_initQ(&tempQueue);
            retVal = EnetDma_retrieveRxPktQ(rx->hFlow, &tempQueue);
            if (ENET_SOK != retVal)
            {
                Lwip2Enet_print("Lwip2Enet_rxPacketTask: failed to retrieve RX pkts: %d\n", retVal);
            }
        }
        if (tempQueue.count == 0)
        {
            Lwip2EnetStats_addOne(&rx->stats.pktStats.zeroNotifyCnt);
        }

        /*
         * Call Lwip2Enet_prepRxPktQ() even if no packets were received.
         * This allows new packets to be submitted if PBUF buffers became
         * newly available and there were outstanding free packets.
         */
        {
            /*
             * Get all used Rx DMA packets from the hardware, then send the buffers
             * of those packets on to the LwIP stack to be parsed/processed.
             */
            pktCnt = Lwip2Enet_prepRxPktQ(rx, &tempQueue);
        }

        /*
         * We don't want to time the semaphore post used to notify the LwIP stack as that may cause a
         * task transition. We don't want to time the semaphore pend, since that would time us doing
         * nothing but waiting.
         */
        if (pktCnt != 0)
        {
            Lwip2Enet_updateRxNotifyStats(&rx->stats.pktStats, pktCnt, 0U);
        }

        if (!rx->disableEvent)
        {
            EnetDma_enableRxEvent(rx->hFlow);
        }
    }

    /* We are shutting down, notify that we are done */
    SemaphoreP_post(hLwip2Enet->hShutDownSem);
}

static void Lwip2Enet_txPacketTask(void *arg0)
{
    Lwip2Enet_TxObj *tx = (Lwip2Enet_TxObj *)arg0;
    Lwip2Enet_Handle hLwip2Enet = tx->hLwip2Enet;

    while (!hLwip2Enet->shutDownFlag)
    {
        /*
         * Wait for the Tx ISR to notify us that empty packets are available
         * that were used to send data
         */
        SemaphoreP_pend(tx->hPktSem, SemaphoreP_WAIT_FOREVER);
        Lwip2Enet_retrieveTxPkts(tx);
    }

    /* We are shutting down, notify that we are done */
    SemaphoreP_post(hLwip2Enet->hShutDownSem);
}

static void Lwip2Enet_allocRxPackets(Lwip2Enet_RxObj *rx)
{
    Lwip2Enet_Handle hLwip2Enet = rx->hLwip2Enet;
    struct pbuf* hPbufPacket;
    uint32_t bufSize;
    uint32_t i;

    Lwip2Enet_assert(hLwip2Enet->appInfo.hostPortRxMtu != 0);

    /*
     * Pre-allocate twice as many lwIP stack packets as we plan to give to/get from the hardware.
     * The idea here is that even if we fill up all the DMA descriptors submit queue,
     * we will have another complete set to swap in right away.
     */
    for (i = 0U; i < (2U * rx->numPkts); i++)
    {
        bufSize = ENET_UTILS_ALIGN(PBUF_POOL_BUFSIZE, ENETDMA_CACHELINE_ALIGNMENT);

        hPbufPacket = pbuf_alloc(PBUF_RAW, bufSize, PBUF_POOL);
        if (hPbufPacket != NULL)
        {
            Lwip2Enet_assert(hPbufPacket->payload != NULL);

            /* Ensures that the ethernet frame is always on a fresh cacheline */
            Lwip2Enet_assert(ENET_UTILS_IS_ALIGNED(hPbufPacket->payload, ENETDMA_CACHELINE_ALIGNMENT));

            /* Enqueue to the free queue */
            pbufQ_enQ(&rx->freePbufQ, hPbufPacket);

            Lwip2EnetStats_addOne(&rx->stats.freePbufPktEnq);
        }
        else
        {
            Lwip2Enet_print("ERROR: Pbuf_alloc() failure...exiting!\n");
            Lwip2Enet_assert(FALSE);
        }
    }
}

/*
 * Enqueue a new packet and make sure that buffer descriptors are properly linked.
 * NOTE: Not thread safe
 */
static void Lwip2Enet_submitRxPktQ(Lwip2Enet_RxObj *rx)
{
    EnetDma_PktQ resubmitPktQ;
    struct pbuf* hPbufPacket;
    EnetDma_Pkt *pCurrDmaPacket;

    EnetQueue_initQ(&resubmitPktQ);

    /*
     * Fill in as many packets as we can with new PBUF buffers so they can be
     * returned to the stack to be filled in.
     */
    pCurrDmaPacket = (EnetDma_Pkt *)EnetQueue_deq(&rx->freePktInfoQ);

    while (NULL != pCurrDmaPacket)
    {
        hPbufPacket = pbufQ_deQ(&rx->freePbufQ);
        if (hPbufPacket)
        {
            Lwip2EnetStats_addOne(&rx->stats.freePbufPktDeq);
            Lwip2EnetStats_addOne(&rx->stats.freeAppPktDeq);

            EnetDma_initPktInfo(pCurrDmaPacket);
            pCurrDmaPacket->bufPtr = (uint8_t *) hPbufPacket->payload;
            pCurrDmaPacket->orgBufLen = hPbufPacket->len;
            pCurrDmaPacket->userBufLen = hPbufPacket->len;

            /* Save off the PBM packet handle so it can be handled by this layer later */
            pCurrDmaPacket->appPriv = (void *)hPbufPacket;
            EnetQueue_enq(&resubmitPktQ, &pCurrDmaPacket->node);

            pCurrDmaPacket = (EnetDma_Pkt *)EnetQueue_deq(&rx->freePktInfoQ);
        }
        else
        {
            EnetQueue_enq(&rx->freePktInfoQ, &pCurrDmaPacket->node);
            break;
        }
    }

    /*
     * Return the same DMA packets back to the DMA channel (but now
     * associated with a new PBM Packet and buffer)
     */
    if (EnetQueue_getQCount(&resubmitPktQ))
    {
        Lwip2Enet_submitRxPackets(rx, &resubmitPktQ);
    }
}

static uint32_t Lwip2Enet_prepRxPktQ(Lwip2Enet_RxObj *rx,
                                     EnetDma_PktQ *pPktQ)
{
    Lwip2Enet_Handle hLwip2Enet = rx->hLwip2Enet;
    uint32_t packetCount = 0;
    EnetDma_Pkt *pCurrDmaPacket;
#ifdef LWIPIF_CHECKSUM_SUPPORT
    uint32_t csumInfo;
#endif
    bool chkSumErr = false;
#if defined(LWIPIF_APP_RX_PKT_HANDLING)
    bool handled = false;
#endif

    pCurrDmaPacket = (EnetDma_Pkt *)EnetQueue_deq(pPktQ);
    while (pCurrDmaPacket)
    {
        /* Get the full PBUF packet that needs to be returned to the LwIP stack */
        struct pbuf* hPbufPacket = (struct pbuf *)pCurrDmaPacket->appPriv;
        if (hPbufPacket)
        {
            uint32_t validLen = pCurrDmaPacket->userBufLen;

            /* Fill in PBUF packet length field */
            hPbufPacket->len = validLen;
            hPbufPacket->tot_len = validLen;
            Lwip2Enet_assert(hPbufPacket->payload != NULL);

#ifdef LWIPIF_CHECKSUM_SUPPORT
            /* We don't check if HW checksum offload is enabled while checking for checksum error
             * as default value of this field when offload not enabled is false */
            csumInfo = pCurrDmaPacket->chkSumInfo;
#if !(defined(SOC_TPR12) || defined(SOC_AWR294X))
            if ( ENETUDMA_CPPIPSI_GET_IPV4_FLAG(csumInfo)||
                 ENETUDMA_CPPIPSI_GET_IPV6_FLAG(csumInfo) )
            {
                chkSumErr = ENETUDMA_CPPIPSI_GET_CHKSUM_ERR_FLAG(csumInfo);
            }
#else
            chkSumErr = false;
#endif
#endif
            if (!chkSumErr)
            {
#if !defined(LWIPIF_APP_RX_PKT_HANDLING)
                /* Pass the received packet to the LwIP stack */
                LWIPIF_LWIP_input(hLwip2Enet->netif, rx, hPbufPacket);
                packetCount++;
#else
                if (rx->handlePktFxn != NULL)
                {
                    handled = rx->handlePktFxn(hLwip2Enet->netif, hPbufPacket);
                }

                if (!handled)
                {
                    /* Pass the received packet to the LwIP stack */
                    LWIPIF_LWIP_input(hLwip2Enet->netif, rx, hPbufPacket);
                    packetCount++;
                }
                else
                {
                    /* Free old pbuf, allocate a fresh new one. Can we recycle same as is? */
                    pbuf_free(hPbufPacket);
                    hPbufPacket = pbuf_alloc(PBUF_RAW, PBUF_POOL_BUFSIZE, PBUF_POOL);
                    pbufQ_enQ(&rx->freePbufQ, hPbufPacket);
                }
#endif
            }
            else
            {
                /* Put PBUF buffer in free Q as we are not passing to stack */
                pbufQ_enQ(&rx->freePbufQ, hPbufPacket);
                Lwip2EnetStats_addOne(&rx->stats.chkSumErr);
            }

            /* Put packet info into free Q as we have removed the PBUF buffers
             * from the it */
            EnetQueue_enq(&rx->freePktInfoQ, &pCurrDmaPacket->node);
            Lwip2EnetStats_addOne(&rx->stats.freeAppPktEnq);

            pCurrDmaPacket = (EnetDma_Pkt *)EnetQueue_deq(pPktQ);
        }
        else
        {
            /* Should never happen as this is received from HW */
            Lwip2Enet_assert(FALSE);
        }
    }

    /* return as many packets to driver as we can */
    Lwip2Enet_submitRxPktQ(rx);

    return packetCount;
}

static uint32_t Lwip2Enet_prepTxPktQ(Lwip2Enet_TxObj *tx,
                                     EnetDma_PktQ *pPktQ)
{
    uint32_t packetCount;
    EnetDma_Pkt *pCurrDmaPacket;
    struct pbuf* hPbufPacket;

    packetCount = EnetQueue_getQCount(pPktQ);

    pCurrDmaPacket = (EnetDma_Pkt *)EnetQueue_deq(pPktQ);
    while (pCurrDmaPacket)
    {
        hPbufPacket = (struct pbuf *)pCurrDmaPacket->appPriv;

        Lwip2Enet_assert(hPbufPacket != NULL);
        /* Free PBUF buffer as it is transmitted by DMA now */
        pbuf_free(hPbufPacket);
        /* Return packet info to free pool */
        EnetQueue_enq(&tx->freePktInfoQ, &pCurrDmaPacket->node);
        pCurrDmaPacket = (EnetDma_Pkt *)EnetQueue_deq(pPktQ);
    }

    Lwip2EnetStats_addNum(&tx->stats.freeAppPktEnq, packetCount);

    return packetCount;
}

static void Lwip2Enet_updateTxNotifyStats(Lwip2Enet_PktTaskStats *pktTaskStats,
                                          uint32_t packetCount,
                                          uint32_t timeDiff)
{
#if defined(LWIPIF_INSTRUMENTATION_ENABLED)
    uint32_t notificationCount;
    uint32_t timePerPacket = timeDiff / packetCount;

    notificationCount = pktStats->dataNotifyCnt & (HISTORY_CNT - 1U);
    pktStats->dataNotifyCnt++;

    pktStats->totalPktCnt   += packetCount;
    pktStats->totalCycleCnt += timeDiff;

    pktStats->cycleCntPerNotify[notificationCount] = timeDiff;
    if (timeDiff > pktStats->cycleCntPerNotifyMax)
    {
        pktStats->cycleCntPerNotifyMax = timeDiff;
    }

    pktStats->pktsPerNotify[notificationCount] = packetCount;
    if (packetCount > pktStats->pktsPerNotifyMax)
    {
        pktStats->pktsPerNotifyMax = packetCount;
    }

    pktStats->cycleCntPerPkt[notificationCount] = timePerPacket;
    if (timePerPacket > pktStats->cycleCntPerPktMax)
    {
        pktStats->cycleCntPerPktMax = timePerPacket;
    }
#endif
}

static void Lwip2Enet_updateRxNotifyStats(Lwip2Enet_PktTaskStats *pktTaskStats,
                                          uint32_t packetCount,
                                          uint32_t timeDiff)
{
#if defined(LWIPIF_INSTRUMENTATION_ENABLED)
    uint32_t notificationCount;
    uint32_t timePerPacket = timeDiff / packetCount;

    notificationCount = pktTaskStats->dataNotifyCnt & (HISTORY_CNT - 1U);
    pktTaskStats->dataNotifyCnt++;

    pktTaskStats->totalPktCnt   += packetCount;
    pktTaskStats->totalCycleCnt += timeDiff;

    pktTaskStats->cycleCntPerNotify[notificationCount] = timeDiff;
    if (timeDiff > pktTaskStats->cycleCntPerNotifyMax)
    {
        pktTaskStats->cycleCntPerNotifyMax = timeDiff;
    }

    pktTaskStats->pktsPerNotify[notificationCount] = packetCount;
    if (packetCount > pktTaskStats->pktsPerNotifyMax)
    {
        pktTaskStats->pktsPerNotifyMax = packetCount;
    }

    pktTaskStats->cycleCntPerPkt[notificationCount] = timePerPacket;
    if (timePerPacket > pktTaskStats->cycleCntPerPktMax)
    {
        pktTaskStats->cycleCntPerPktMax = timePerPacket;
    }
#endif
}

static void Lwip2Enet_print(const char *prnStr,
                           ...)
{
    // TODO fix me
    if (NULL != gLwip2EnetObj.print)
    {
        /* Function is non- reentrant */
        va_list vaArgPtr;
        char *buf;

        buf = &gLwip2EnetObj.printBuf[0];
        va_start(vaArgPtr, prnStr);
        vsnprintf(buf, ENET_CFG_PRINT_BUF_LEN, (const char *)prnStr, vaArgPtr);
        va_end(vaArgPtr);

        (*gLwip2EnetObj.print)("[LWIP2ENET] ");
        (*gLwip2EnetObj.print)(gLwip2EnetObj.printBuf);
    }
}

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static int32_t Lwip2Enet_startRxTx(Lwip2Enet_Handle hLwip2Enet)
{
    uint32_t i;
    int32_t status = ENET_SOK;

    Lwip2Enet_assert(NULL != hLwip2Enet->tx.hCh);

    status = EnetDma_enableTxEvent(hLwip2Enet->tx.hCh);

    /* Initialize free DMA packet queues */
    Lwip2Enet_prepTxQs(&hLwip2Enet->tx);

    for (i = 0U; i < ENET_ARRAYSIZE(hLwip2Enet->rx); i++)
    {
        if (hLwip2Enet->rx[i].enabled)
        {
            /* Initialize free DMA packet queues */
            Lwip2Enet_prepRxQs(&hLwip2Enet->rx[i]);

            /* Submit all allocated packets to DMA so it can use for packet RX */
            Lwip2Enet_submitRxPktQ(&hLwip2Enet->rx[i]);
        }
    }

    return status;
}

static void Lwip2Enet_stopRxTx(Lwip2Enet_Handle hLwip2Enet)
{
    uint32_t i;

    /* Stop RX packet task */
    for (i = 0U; i < ENET_ARRAYSIZE(hLwip2Enet->rx); i++)
    {
        if (hLwip2Enet->rx[i].enabled)
        {
            Lwip2Enet_stopRxTask(&hLwip2Enet->rx[i]);
        }
    }

    /* Stop TX packet task */
    Lwip2Enet_stopTxTask(&hLwip2Enet->tx);
}
static void Lwip2Enet_freeTxPktCb(void *cbArg,
                                  EnetDma_PktQ *fqPktInfoQ,
                                  EnetDma_PktQ *cqPktInfoQ)
{
    Lwip2Enet_TxObj *tx = (Lwip2Enet_TxObj *)cbArg;

    EnetQueue_append(&tx->freePktInfoQ, fqPktInfoQ);
    Lwip2EnetStats_addNum(&tx->stats.freeAppPktEnq, EnetQueue_getQCount(fqPktInfoQ));
    Lwip2Enet_freePbufPackets(fqPktInfoQ);

    EnetQueue_append(&tx->freePktInfoQ, cqPktInfoQ);
    Lwip2EnetStats_addNum(&tx->stats.freeAppPktEnq, EnetQueue_getQCount(cqPktInfoQ));
    Lwip2Enet_freePbufPackets(cqPktInfoQ);
}

static void Lwip2Enet_freeRxPktCb(void *cbArg,
                                  EnetDma_PktQ *fqPktInfoQ,
                                  EnetDma_PktQ *cqPktInfoQ)
{
    Lwip2Enet_RxObj *rx = (Lwip2Enet_RxObj *)cbArg;

    /* Now that we free PBUF buffers, push all freed pktInfo's into Rx
     * free Q */
    EnetQueue_append(&rx->freePktInfoQ, fqPktInfoQ);
    Lwip2EnetStats_addNum(&rx->stats.freeAppPktEnq, EnetQueue_getQCount(fqPktInfoQ));
    Lwip2Enet_freePbufPackets(fqPktInfoQ);

    EnetQueue_append(&rx->freePktInfoQ, cqPktInfoQ);
    Lwip2EnetStats_addNum(&rx->stats.freeAppPktEnq, EnetQueue_getQCount(cqPktInfoQ));
    Lwip2Enet_freePbufPackets(cqPktInfoQ);

    /* Flush out our pending receive queues */
    while (pbufQ_count(&rx->freePbufQ) != 0)
    {
        pbuf_free(pbufQ_deQ(&rx->freePbufQ));
        Lwip2EnetStats_addOne(&rx->stats.freePbufPktDeq);
    }
}

static void Lwip2Enet_retrieveTxPkts(Lwip2Enet_TxObj *tx)
{
    EnetDma_PktQ tempQueue;
    uint32_t packetCount = 0;
    int32_t retVal;

    Lwip2EnetStats_addOne(&tx->stats.pktStats.rawNotifyCnt);
    packetCount = 0;

    /* Retrieve the used (sent/empty) packets from the channel */
    {
        EnetQueue_initQ(&tempQueue);
        /* Retrieve all TX packets and keep them locally */
        retVal = EnetDma_retrieveTxPktQ(tx->hCh, &tempQueue);
        if (ENET_SOK != retVal)
        {
            Lwip2Enet_print("Lwip2Enet_retrieveTxPkts: Failed to retrieve TX pkts: %d\n", retVal);
        }
    }

    if (tempQueue.count != 0U)
    {
        /*
         * Get all used Tx DMA packets from the hardware, then return those
         * buffers to the txFreePktQ so they can be used later to send with.
         */
        packetCount = Lwip2Enet_prepTxPktQ(tx, &tempQueue);
    }
    else
    {
        Lwip2EnetStats_addOne(&tx->stats.pktStats.zeroNotifyCnt);
    }

    if (packetCount != 0)
    {
        Lwip2Enet_updateTxNotifyStats(&tx->stats.pktStats, packetCount, 0U);
    }
}

static void Lwip2Enet_timerCb(void * arg)
{
    /* Post semaphore to rx handling task */
    Lwip2Enet_Handle hLwip2Enet = (Lwip2Enet_Handle)arg;
    uint32_t i;

    if ((hLwip2Enet->initDone) && (hLwip2Enet->shutDownFlag == false))
    {
        for (i = 0U; i < ENET_ARRAYSIZE(hLwip2Enet->rx); i++)
        {
            if (hLwip2Enet->rx[i].enabled)
            {
                SemaphoreP_post(hLwip2Enet->rx[i].hPktSem);
            }
        }
    }
}

static void Lwip2Enet_createTimer(Lwip2Enet_Handle hLwip2Enet)
{
    ClockP_Params clkParams;
    ClockP_Params_init(&clkParams);
    clkParams.startMode = ClockP_StartMode_AUTO;
    clkParams.runMode = ClockP_RunMode_CONTINUOUS;
    clkParams.period    = (hLwip2Enet->appInfo.timerPeriodUs + 999)/1000;
    clkParams.arg       = hLwip2Enet;

    /* Creating timer and setting timer callback function*/
    hLwip2Enet->hPacingTimer = ClockP_create((void *)&Lwip2Enet_timerCb,
                         &clkParams);
    Lwip2Enet_assert(hLwip2Enet->hPacingTimer != NULL);
}

static void  Lwip2Enet_initGetHandleInArgs(Lwip2Enet_Handle hLwip2Enet,
                                          LwipifEnetAppIf_GetHandleInArgs *inArgs)
{
    uint32_t i;

    inArgs->txCfg.cbArg      = &hLwip2Enet->tx;
    inArgs->txCfg.notifyCb   = &Lwip2Enet_notifyTxPackets;
    inArgs->txCfg.numPackets = hLwip2Enet->tx.numPkts;

    for (i = 0U; i < ENET_ARRAYSIZE(hLwip2Enet->rx); i++)
    {
        inArgs->rxCfg[i].cbArg      = &hLwip2Enet->rx[i];
        inArgs->rxCfg[i].notifyCb   = &Lwip2Enet_notifyRxPackets;
        inArgs->rxCfg[i].numPackets = hLwip2Enet->rx[i].numPkts;
    }
}

static void Lwip2Enet_initReleaseHandleInArgs(Lwip2Enet_Handle hLwip2Enet,
                                             LwipifEnetAppIf_ReleaseHandleInfo *inArgs)
{
    uint32_t i;

    inArgs->coreId       = hLwip2Enet->appInfo.coreId;
    inArgs->coreKey      = hLwip2Enet->appInfo.coreKey;
    inArgs->hEnet        = hLwip2Enet->appInfo.hEnet;
#if !(defined(SOC_TPR12) || defined(SOC_AWR294X))
    inArgs->hUdmaDrv     = hLwip2Enet->appInfo.hUdmaDrv;
#endif

    for (i = 0U; i < ENET_ARRAYSIZE(hLwip2Enet->rx); i++)
    {
        if (hLwip2Enet->rx[i].enabled)
        {
            inArgs->rxFreePkt[i].cb    = &Lwip2Enet_freeRxPktCb;
            inArgs->rxFreePkt[i].cbArg = &hLwip2Enet->rx[i];
            inArgs->rxInfo[i] = hLwip2Enet->appInfo.rxInfo[i];
        }
    }

    inArgs->txFreePkt.cb    = &Lwip2Enet_freeTxPktCb;
    inArgs->txFreePkt.cbArg = &hLwip2Enet->tx;
    inArgs->txInfo = hLwip2Enet->appInfo.txInfo;
}
