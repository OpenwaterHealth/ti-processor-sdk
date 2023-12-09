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
 * \file  ndk2enet_appif.h
 *
 * \brief Header file for App interfaces to the ndk2enet.c.
 */

#ifndef NDK2ENET_APPIF_H_
#define NDK2ENET_APPIF_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <assert.h>
#include <ti/drv/enet/enet.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

typedef void (*NimuEnetAppIf_FreePktCbFxn)(void *cbArg,
                                           EnetDma_PktQ *fqPktInfoQ,
                                           EnetDma_PktQ *cqPktInfoQ);

typedef bool (*NimuEnetAppIf_IsPhyLinkedCbFxn)(Enet_Handle hEnet);


typedef struct NimuEnetAppIf_RxConfig_s
{
    void *cbArg;
    EnetDma_PktNotifyCb notifyCb;
    uint32_t numPackets;
} NimuEnetAppIf_RxConfig;

typedef struct NimuEnetAppIf_TxConfig_s
{
    void *cbArg;
    EnetDma_PktNotifyCb notifyCb;
    uint32_t numPackets;
} NimuEnetAppIf_TxConfig;


typedef struct NimuEnetAppIf_GetHandleInArgs_s
{
    NimuEnetAppIf_TxConfig txCfg;
    NimuEnetAppIf_RxConfig rxCfg;
} NimuEnetAppIf_GetHandleInArgs;


typedef struct NimuEnetAppIf_RxHandleInfo_s
{
    /** ENET DMA receive channel */
    EnetDma_RxChHandle hRxFlow;
    /** Flow index for flow used  */
    uint32_t rxFlowStartIdx;
    /** Flow index for flow used  */
    uint32_t rxFlowIdx;
    /** Mac Address allocated for the flow */
    uint8_t macAddr[ENET_MAC_ADDR_LEN];
} NimuEnetAppIf_RxHandleInfo;

typedef struct NimuEnetAppIf_TxHandleInfo_s
{
    /** ENET DMA transmit channel */
    EnetDma_TxChHandle hTxChannel;
    /** Tx Channel Peer Id */
    uint32_t txChNum;
} NimuEnetAppIf_TxHandleInfo;


typedef struct NimuEnetAppIf_GetHandleOutArgs_s
{
    Enet_Handle hEnet;
#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
    Udma_DrvHandle hUdmaDrv;
#endif
    uint32_t coreId;
    uint32_t coreKey;
    Enet_Print print;
    /** Max Tx packet size per priority */
    uint32_t txMtu[ENET_PRI_NUM];
    /** Max Rx packet size */
    uint32_t hostPortRxMtu;
    NimuEnetAppIf_IsPhyLinkedCbFxn isPortLinkedFxn;
    NimuEnetAppIf_TxHandleInfo txInfo;
    NimuEnetAppIf_RxHandleInfo rxInfo;
    bool isRingMonUsed;
    bool disableTxEvent;
    /** Timer interval for timer based RX pacing */
    uint32_t timerPeriodUs;
} NimuEnetAppIf_GetHandleOutArgs;

typedef struct NimuEnetAppIf_ReleaseHandleInfo_s
{
    Enet_Handle hEnet;
#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_UDMA)
    Udma_DrvHandle hUdmaDrv;
#endif
    uint32_t coreId;
    uint32_t coreKey;
    NimuEnetAppIf_TxHandleInfo txInfo;
    NimuEnetAppIf_RxHandleInfo rxInfo;
    void *freePktCbArg;
    NimuEnetAppIf_FreePktCbFxn txFreePktCb;
    NimuEnetAppIf_FreePktCbFxn rxFreePktCb;
} NimuEnetAppIf_ReleaseHandleInfo;


/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

extern void NimuEnetAppCb_getHandle(NimuEnetAppIf_GetHandleInArgs *inArgs,
                                    NimuEnetAppIf_GetHandleOutArgs *outArgs);

extern void NimuEnetAppCb_releaseHandle(NimuEnetAppIf_ReleaseHandleInfo *releaseInfo);


/* ========================================================================== */
/*                        Deprecated Function Declarations                    */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* NDK2ENET_APPIF_H_ */
