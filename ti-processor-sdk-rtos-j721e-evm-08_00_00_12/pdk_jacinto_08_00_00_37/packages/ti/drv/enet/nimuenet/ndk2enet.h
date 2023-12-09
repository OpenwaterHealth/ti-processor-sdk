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
 * \file  ndk2enet.h
 *
 * \brief This file contains the type definitions and helper macros for the
 *        interfacing between NDK and ENET.
 */

#ifndef NDK2ENET_H_
#define NDK2ENET_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* Standard language headers */
#include <stdint.h>
#include <assert.h>

/* xdc header - should be first file included as due to order dependency */
#include <xdc/std.h>

/* OS/Posix headers */
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/hal/Timer.h>

/* NDK Dependencies */
#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/inc/stkmain.h>
#include <ti/ndk/inc/usertype.h>

/* Project dependency headers */
#include "ndk2enet_appif.h"
#include "enet.h"
#include "enet_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/*
 * Pre-Pad Packet Data Offset
 *
 *   The TCP/IP stack library requires that every packet device
 *   include enough L2 header room for all supported headers. In
 *   order to support PPPoE, this requires a 22 byte L2 header.
 *   Thus, since standard Ethernet is only 14 bytes, we must add
 *   on an additional 8 byte offset, or PPPoE can not function
 *   with our driver.
 */
#define     PKT_PREPAD                      ((uint32_t)8U)

/* Indicates whether RAM based multicast lists are suported for this
 * ENET peripheral.
 */
#define     RAM_MCAST                       0U

/* Indicates whether HASH based multicasting is suported for this
 * ENET peripheral.
 */
#define     HASH_MCAST                      0U

/* Multicast Address List Size */
#define     PKT_MAX_MCAST                   ((uint32_t)31U)

/*
 * Packet count of packets given to the hardware. We should use twice this number
 * for the number of packets maintained
 */
#define NDK2ENET_RX_PACKETS             (64U)
#define NDK2ENET_TX_PACKETS             (64U)
#define ENET_TX_RING_ENTRIES            (NDK2ENET_TX_PACKETS)
#define ENET_RX_RING_ENTRIES            (NDK2ENET_RX_PACKETS)

#define NIMU_ENET_GET_PBM_BUFSIZE(mtu)  ((mtu) + 4U + PKT_PREPAD)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

enum NDK2ENET_IOCTL_
{
    NDK2ENET_IOCTL_BASE            = 0x00000000,
    /** ndk2enet get hRxPacketTask load function IOCTL value. IOCTL param is empty */
    /* IMP: Taking this value as it should not conflict with ENET IOCTL commands */
    NDK2ENET_IOCTL_GET_RXTASK_LOAD = 0x0000ABAA,
    /** ndk2enet get hTxPacketTask load function IOCTL value. IOCTL param is empty */
    /* IMP: Taking this value as it should not conflict with ENET IOCTL commands */
    NDK2ENET_IOCTL_GET_TXTASK_LOAD = 0x0000ABAB
};

#define HISTORY_CNT ((uint32_t)256U)

typedef NimuEnetAppIf_GetHandleOutArgs Ndk2Enet_AppInfo;

typedef struct NDK2ENET_PKT_TASK_STATS_
{
    uint32_t rawNotifyCnt;
    uint32_t dataNotifyCnt;
    uint32_t zeroNotifyCnt;
    uint32_t totalPktCnt;
    uint32_t totalCycleCnt;

    uint32_t pktsPerNotifyMax;
    uint32_t pktsPerNotify[HISTORY_CNT];
    uint32_t cycleCntPerNotifyMax;
    uint32_t cycleCntPerNotify[HISTORY_CNT];
    uint32_t cycleCntPerPktMax;
    uint32_t cycleCntPerPkt[HISTORY_CNT];
    uint32_t taskLoad[HISTORY_CNT];
}
Ndk2Enet_PktTaskStats;

typedef struct Ndk2Enet_STATS_
{
    Ndk2Enet_PktTaskStats rxStats;
    uint32_t rxReadyPBMPktEnq;
    uint32_t rxReadyPBMPktDq;
    uint32_t rxPBMAllocCnt;
    uint32_t rxPBMFreeCnt;
    uint32_t rxFreeAppPktEnq;
    uint32_t rxFreeAppPktDq;
    uint32_t rxUnderFlowCnt;
    uint32_t rxChkSumErr;
    uint32_t ndkStackNotifyCnt;

    Ndk2Enet_PktTaskStats txStats;
    uint32_t txReadyPBMPktEnq;
    uint32_t txReadyPBMPktDq;
    uint32_t txFreeAppPktEnq;
    uint32_t txFreeAppPktDq;
    uint32_t txDroppedPktCnt;

    uint32_t gCpuLoad[HISTORY_CNT];
    uint32_t gHwiLoad[HISTORY_CNT];
}
Ndk2Enet_Stats;

/**
 * \brief
 *  Packet device information
 *
 * \details
 *  This structure caches the device info.
 */
typedef struct Ndk2Enet_OBJECT_
{
    Ndk2Enet_AppInfo appInfo;
    /** NIMU initialization flag.*/
    uint32_t initDone;
    /** Index of currently connect physical port.*/
    uint32_t currLinkedIf;

    /*
     * Semaphore handle used by NDK stack and driver to communicate any pending
     * Rx events that need to be serviced by NDK ethernet stack.
     */
    STKEVENT_Handle hEvent;
    /** Current RX filter */
    uint32_t ndkRxFilter;
    /** Previous MCast Address Counter */
    uint32_t oldMCastCnt;
    /** Previous Multicast list configured by the Application.*/
    uint8_t bOldMCast[(uint32_t)ENET_MAC_ADDR_LEN * PKT_MAX_MCAST];
    /** Current MCast Address Counter */
    uint32_t MCastCnt;
    /** Multicast list configured by the Application.*/
    uint8_t bMCast[(uint32_t)ENET_MAC_ADDR_LEN * PKT_MAX_MCAST];
    /** Link is up flag. */
    uint32_t linkIsUp;
    /** Device is operating in test digital loopback mode.*/
    uint32_t inDLBMode;
    /** Total number of PBM packets allocated by application - used for debug purpose.*/
    uint32_t numAllocPBMPkts;

    /*!
     * \brief       Eth packet info structure used to exchange data between RX flow
     *              and NIMU app.
     *  Note:       This is equal to no. of DMA descriptors and half of total
     *              PBM packets kept in free reserve. This is done to ensure that
     *              every submitRxPkt call after EnetDma_retrieveRxPkts has empty
     *              PBM packets available.
     */
    EnetDma_Pkt pktInfoMem[NDK2ENET_RX_PACKETS + NDK2ENET_TX_PACKETS];

    /** NDK PBM Rx ready queue (holds packets ready to be received by the NDK stack) */
    PBMQ rxReadyPBMPktQ;

    /*!
     * \brief       DMA Rx free packet info queue (holds packets returned from the
     *              hardware.
     */
    EnetDma_PktQ rxFreePktInfoQ;

    /*
     * Handle to Rx task, whose job it is to receive packets used by the hardware
     * and give them to the NDK stack, and return freed packets back to the hardware.
     */
    Task_Handle hRxPacketTask;

    /*
     * Handle to Rx semaphore, on which the hRxPacketTask awaits for notification
     * of used packets available.
     */
    Semaphore_Handle hRxPacketSem;

    /*
     * Clock handle for triggering the packet Rx notify
     */
    Clock_Handle hPacingClk;

    /*
     * Timer handle for triggering the packet Rx notify
     */
    Timer_Handle                    hPacingTimer;

    /*!
     * \brief       DMA free queue (holds free hardware packets awaiting)
     */
    EnetDma_PktQ txFreePktInfoQ;

    /** NDK PBM Tx ready queue (holds packets ready to be sent to the hardware)*/
    PBMQ txReadyPBMPktQ;

    /** NDK PBM Tx Unused queue (holds packets that were not sent to the hardware
     *  in the previous submit)*/
    PBMQ txUnUsedPBMPktQ;

    /*
     * Handle to Rx task, whose job it is to receive packets used by the hardware
     * and give them to the NDK stack, and return freed packets back to the hardware.
     */
    Task_Handle hTxPacketTask;

    /*
     * Handle to Tx semaphore, on which the hTxPacketTask awaits for notification
     * of used packets available.
     */
    Semaphore_Handle hTxPacketSem;

    /*
     * Handle to counting shutdown semaphore, which all subtasks created in the
     * open function must post before the close operation can complete.
     */
    Semaphore_Handle hShutDownSem;

    /** Boolean to indicate shutDownFlag status of translation layer.*/
    volatile bool shutDownFlag;

    /**< Print buffer */
    char printBuf[ENET_CFG_PRINT_BUF_LEN];

    /**< Print Function */
    Enet_Print print;
}
Ndk2Enet_Object, *Ndk2Enet_Handle;

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

extern Ndk2Enet_Stats gNdk2EnetStats;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/*
 * Functions Provided by our translation layer code
 */
extern Ndk2Enet_Handle Ndk2Enet_open(STKEVENT_Handle hEvent);

extern void Ndk2Enet_close(Ndk2Enet_Handle hndk2enet);

extern void Ndk2Enet_setRx(Ndk2Enet_Handle hndk2enet);

extern void Ndk2Enet_sendTxPackets(Ndk2Enet_Handle hndk2enet);

extern int32_t Ndk2Enet_ioctl(Ndk2Enet_Handle hndk2enet,
                              uint32_t cmd,
                              void *param,
                              uint32_t size);

extern void Ndk2Enet_poll(Ndk2Enet_Handle hndk2enet,
                          uint32_t fTimerTick);

extern void Ndk2Enet_periodicFxn(Ndk2Enet_Handle hNdk2Enet);

/* ========================================================================== */
/*                        Deprecated Function Declarations                    */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */

static inline void Ndk2EnetStas_addOne(uint32_t *statCnt)
{
#if defined(NIMU_INSTRUMENTATION_ENABLED)
    *statCnt += 1U;
#endif
}

static inline void Ndk2EnetStats_addNum(uint32_t *statCnt,
                                        uint32_t addCnt)
{
#if defined(NIMU_INSTRUMENTATION_ENABLED)
    *statCnt += addCnt;
#endif
}

static inline void Ndk2Enet_assert(bool cond)
{
    assert(cond);
}

#ifdef __cplusplus
}
#endif

#endif /* NDK2ENET_H_ */
