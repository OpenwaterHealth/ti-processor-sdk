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

/**
 *  \file enet_appmemutils.h
 *
 *  \brief Enet DMA memutils header file.
 */

#ifndef ENET_MEM_CFG_H_
#define ENET_MEM_CFG_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/drv/enet/enet_cfg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/*
 * Packet sizes for different pools. LARGE_POOL is largest packet size pool with MEDIUM_POOL
 * second largest and SMALL_POOL smallest
 */
#define ENET_MEM_LARGE_POOL_PKT_SIZE        ENET_UTILS_ALIGN(1536U, ENET_UTILS_CACHELINE_SIZE)
#define ENET_MEM_MEDIUM_POOL_PKT_SIZE       ENET_UTILS_ALIGN(1000U, ENET_UTILS_CACHELINE_SIZE)
#define ENET_MEM_SMALL_POOL_PKT_SIZE        ENET_UTILS_ALIGN(500U, ENET_UTILS_CACHELINE_SIZE)

#if defined(SOC_TPR12) || defined(SOC_AWR294X)
#define ENET_MEM_LARGE_POOL_RX_PACKET_NUM   (16U)
#define ENET_MEM_LARGE_POOL_TX_PACKET_NUM   (16U)
#else
#define ENET_MEM_LARGE_POOL_RX_PACKET_NUM   (128U)
#define ENET_MEM_LARGE_POOL_TX_PACKET_NUM   (128U)
#endif
#define ENET_MEM_LARGE_POOL_PACKET_NUM                             \
    ((ENET_MEM_LARGE_POOL_RX_PACKET_NUM * ENET_CFG_RX_FLOWS_NUM) + \
     (ENET_MEM_LARGE_POOL_TX_PACKET_NUM * ENET_CFG_TX_CHANNELS_NUM))

#if defined(SOC_TPR12) || defined(SOC_AWR294X)
#define ENET_MEM_MEDIUM_POOL_RX_PACKET_NUM  (8U)
#define ENET_MEM_MEDIUM_POOL_TX_PACKET_NUM  (8U)
#else
#define ENET_MEM_MEDIUM_POOL_RX_PACKET_NUM  (64U)
#define ENET_MEM_MEDIUM_POOL_TX_PACKET_NUM  (64U)
#endif

#define ENET_MEM_MEDIUM_POOL_PACKET_NUM                             \
    ((ENET_MEM_MEDIUM_POOL_RX_PACKET_NUM * ENET_CFG_RX_FLOWS_NUM) + \
     (ENET_MEM_MEDIUM_POOL_TX_PACKET_NUM * ENET_CFG_TX_CHANNELS_NUM))

#if defined(SOC_TPR12) || defined(SOC_AWR294X)
#define ENET_MEM_SMALL_POOL_RX_PACKET_NUM   (8U)
#define ENET_MEM_SMALL_POOL_TX_PACKET_NUM   (8U)
#else
#define ENET_MEM_SMALL_POOL_RX_PACKET_NUM   (64U)
#define ENET_MEM_SMALL_POOL_TX_PACKET_NUM   (64U)
#endif

#define ENET_MEM_SMALL_POOL_PACKET_NUM                             \
    ((ENET_MEM_SMALL_POOL_RX_PACKET_NUM * ENET_CFG_RX_FLOWS_NUM) + \
     (ENET_MEM_SMALL_POOL_TX_PACKET_NUM * ENET_CFG_TX_CHANNELS_NUM))

#if defined(SOC_TPR12) || defined(SOC_AWR294X)
#define ENET_MEM_NUM_TX_PKTS                (8U)
#define ENET_MEM_NUM_RX_PKTS                (8U)
#else
#define ENET_MEM_NUM_TX_PKTS                (128U)
#define ENET_MEM_NUM_RX_PKTS                (128U)
#endif

#define ENET_MEM_NUM_TX_RING_ENTRIES        (ENET_MEM_NUM_TX_PKTS)
#define ENET_MEM_NUM_RX_RING_ENTRIES        (ENET_MEM_NUM_RX_PKTS)

#define ENET_MEM_RING_MAX_ELEM_CNT          (256U)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* ENET_MEM_CFG_H_ */
