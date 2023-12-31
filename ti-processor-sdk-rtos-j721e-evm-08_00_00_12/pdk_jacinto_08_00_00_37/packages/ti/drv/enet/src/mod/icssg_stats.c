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
 * \file  icssg_stats.c
 *
 * \brief This file contains the implementation of ICSSG statistics.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/csl/cslr_icss.h>
#include <ti/drv/enet/include/core/enet_base.h>
#include <ti/drv/enet/include/core/enet_utils.h>
#include <ti/drv/enet/include/core/enet_mod.h>
#include <ti/drv/enet/priv/mod/icssg_stats_priv.h>
#include <ti/drv/enet/firmware/icssg/fw_mem_map.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void IcssgStats_getPaStats(IcssgStats_Handle hStats,
                                  IcssgStats_Pa *paStats);

static void IcssgStats_getMacPortStats(IcssgStats_Handle hStats,
                                       Enet_MacPort macPort,
                                       IcssgStats_MacPort *stats);

static void IcssgStats_resetMacPortStats(IcssgStats_Handle hStats,
                                         Enet_MacPort macPort);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Register offsets for all PA statistics */
static uint32_t IcssgStats_paStatsOffset[] =
{
    NRT_HOST_RX_PKT_COUNT_PASTATID,
    NRT_HOST_TX_PKT_COUNT_PASTATID,
    NRT_RTU0_PACKET_DROPPED_SLICE0_PASTATID,
    NRT_RTU0_PACKET_DROPPED_SLICE1_PASTATID,
    NRT_PORT1_Q0_OVERFLOW_PASTATID,
    NRT_PORT1_Q1_OVERFLOW_PASTATID,
    NRT_PORT1_Q2_OVERFLOW_PASTATID,
    NRT_PORT1_Q3_OVERFLOW_PASTATID,
    NRT_PORT1_Q4_OVERFLOW_PASTATID,
    NRT_PORT1_Q5_OVERFLOW_PASTATID,
    NRT_PORT1_Q6_OVERFLOW_PASTATID,
    NRT_PORT1_Q7_OVERFLOW_PASTATID,
    NRT_PORT2_Q0_OVERFLOW_PASTATID,
    NRT_PORT2_Q1_OVERFLOW_PASTATID,
    NRT_PORT2_Q2_OVERFLOW_PASTATID,
    NRT_PORT2_Q3_OVERFLOW_PASTATID,
    NRT_PORT2_Q4_OVERFLOW_PASTATID,
    NRT_PORT2_Q5_OVERFLOW_PASTATID,
    NRT_PORT2_Q6_OVERFLOW_PASTATID,
    NRT_PORT2_Q7_OVERFLOW_PASTATID,
    NRT_HOST_Q0_OVERFLOW_PASTATID,
    NRT_HOST_Q1_OVERFLOW_PASTATID,
    NRT_HOST_Q2_OVERFLOW_PASTATID,
    NRT_HOST_Q3_OVERFLOW_PASTATID,
    NRT_HOST_Q4_OVERFLOW_PASTATID,
    NRT_HOST_Q5_OVERFLOW_PASTATID,
    NRT_HOST_Q6_OVERFLOW_PASTATID,
    NRT_HOST_Q7_OVERFLOW_PASTATID,
    NRT_HOST_EGRESS_Q_PRE_OVERFLOW_PASTATID,
    NRT_DROPPED_PKT_SLICE0_PASTATID,
    NRT_DROPPED_PKT_SLICE1_PASTATID,
    NRT_RX_ERROR_SLICE0_PASTATID,
    NRT_RX_ERROR_SLICE1_PASTATID,
    RX_EOF_RTU_DS_INVALID_SLICE0_PASTATID,
    RX_EOF_RTU_DS_INVALID_SLICE1_PASTATID,
    NRT_TX_PORT1_DROPPED_PACKET_PASTATID,
    NRT_TX_PORT2_DROPPED_PACKET_PASTATID,
    NRT_TX_PORT1_TS_DROPPED_PACKET_PASTATID,
    NRT_TX_PORT2_TS_DROPPED_PACKET_PASTATID,
    NRT_INF_PORT_DISABLED_SLICE0_PASTATID,
    NRT_INF_PORT_DISABLED_SLICE1_PASTATID,
    NRT_INF_SAV_SLICE0_PASTATID,
    NRT_INF_SAV_SLICE1_PASTATID,
    NRT_INF_SA_BL_SLICE0_PASTATID,
    NRT_INF_SA_BL_SLICE1_PASTATID,
    NRT_INF_PORT_BLOCKED_SLICE0_PASTATID,
    NRT_INF_PORT_BLOCKED_SLICE1_PASTATID,
    NRT_INF_AFT_DROP_TAGGED_SLICE0_PASTATID,
    NRT_INF_AFT_DROP_TAGGED_SLICE1_PASTATID,
    NRT_INF_AFT_DROP_PRIOTAGGED_SLICE0_PASTATID,
    NRT_INF_AFT_DROP_PRIOTAGGED_SLICE1_PASTATID,
    NRT_INF_AFT_DROP_NOTAG_SLICE0_PASTATID,
    NRT_INF_AFT_DROP_NOTAG_SLICE1_PASTATID,
    NRT_INF_AFT_DROP_NOTMEMBER_SLICE0_PASTATID,
    NRT_INF_AFT_DROP_NOTMEMBER_SLICE1_PASTATID,
    NRT_FDB_NO_SPACE_TO_LEARN,
    NRT_PREEMPT_BAD_FRAG_SLICE0_PASTATID,
    NRT_PREEMPT_BAD_FRAG_SLICE1_PASTATID,
    NRT_PREEMPT_ASSEMBLY_ERROR_SLICE0_PASTATID,
    NRT_PREEMPT_ASSEMBLY_ERROR_SLICE1_PASTATID,
    NRT_PREEMPT_FRAG_COUNT_TX_SLICE0_PASTATID,
    NRT_PREEMPT_FRAG_COUNT_TX_SLICE1_PASTATID,
    NRT_PREEMPT_ASSEMBLY_OK_SLICE0_PASTATID,
    NRT_PREEMPT_ASSEMBLY_OK_SLICE1_PASTATID,
    NRT_PREEMPT_FRAG_COUNT_RX_SLICE0_PASTATID,
    NRT_PREEMPT_FRAG_COUNT_RX_SLICE1_PASTATID,
    RX_EOF_SHORT_FRAMEERR_SLICE0_PASTATID,
    RX_EOF_SHORT_FRAMEERR_SLICE1_PASTATID,
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t IcssgStats_open(EnetMod_Handle hMod,
                        Enet_Type enetType,
                        uint32_t instId,
                        const void *cfg,
                        uint32_t cfgSize)
{
    IcssgStats_Handle hStats = (IcssgStats_Handle)hMod;
    uintptr_t baseAddr = (uintptr_t)hMod->virtAddr;
    uintptr_t cfgRegs = baseAddr + CSL_ICSS_G_PR1_MII_RT_PR1_MII_RT_G_CFG_REGS_G_REGS_BASE;

    /* Statistics addresses need to be set according to ICSSG Enet types:
     *  - Dual-MAC peripheral has single port, so it maps to port 1 or port 2 based on
     *    the peripheral instance id.
     *  - Switch peripheral is one-to-one mapping */
    if (enetType == ENET_ICSSG_SWITCH)
    {
        hStats->port1Addr = cfgRegs + CSL_ICSS_G_PR1_MII_RT_PR1_MII_RT_G_CFG_REGS_G_RX_STAT_GOOD_PRU0;
        hStats->port2Addr = cfgRegs + CSL_ICSS_G_PR1_MII_RT_PR1_MII_RT_G_CFG_REGS_G_RX_STAT_GOOD_PRU1;
    }
    else
    {
        if ((instId % 2U) == 0U)
        {
            hStats->port1Addr = cfgRegs + CSL_ICSS_G_PR1_MII_RT_PR1_MII_RT_G_CFG_REGS_G_RX_STAT_GOOD_PRU0;
        }
        else
        {
            hStats->port1Addr = cfgRegs + CSL_ICSS_G_PR1_MII_RT_PR1_MII_RT_G_CFG_REGS_G_RX_STAT_GOOD_PRU1;
        }

        hStats->port2Addr = 0U;
    }

    /* Set PA stats address */
    hStats->paStatsAddr = baseAddr + CSL_ICSS_G_PA_STAT_WRAP_PA_SLV_QSTAT_REGS_BASE;

    /* Clear MAC port stats */
    IcssgStats_resetMacPortStats(hStats, ENET_MAC_PORT_1);
    if (enetType == ENET_ICSSG_SWITCH)
    {
        IcssgStats_resetMacPortStats(hStats, ENET_MAC_PORT_2);
    }

    /* Enable PA_STAT block for diagnostic counters, 2 vi */
    CSL_REG32_WR((uint32_t *)(baseAddr + CSL_ICSS_G_PA_STAT_WRAP_PA_SLV_REGS_BASE + 8U),
                 (1U << 31U) | 2U);

    return ENET_SOK;
}

void IcssgStats_close(EnetMod_Handle hMod)
{
}

int32_t IcssgStats_rejoin(EnetMod_Handle hMod,
                          Enet_Type enetType,
                          uint32_t instId)
{
    return ENET_ENOTSUPPORTED;
}

int32_t IcssgStats_ioctl(EnetMod_Handle hMod,
                         uint32_t cmd,
                         Enet_IoctlPrms *prms)
{
    IcssgStats_Handle hStats = (IcssgStats_Handle)hMod;
    int32_t status = ENET_SOK;

    switch (cmd)
    {
        case ENET_STATS_IOCTL_GET_HOSTPORT_STATS:
        {
            IcssgStats_Pa *stats = (IcssgStats_Pa *)prms->outArgs;

            IcssgStats_getPaStats(hStats, stats);
        }
        break;

        case ENET_STATS_IOCTL_GET_MACPORT_STATS:
        {
            Enet_MacPort macPort = *(Enet_MacPort *)prms->inArgs;
            IcssgStats_MacPort *stats = (IcssgStats_MacPort *)prms->outArgs;

            IcssgStats_getMacPortStats(hStats, macPort, stats);
        }
        break;

        case ENET_STATS_IOCTL_RESET_MACPORT_STATS:
        {
            Enet_MacPort macPort = *(Enet_MacPort *)prms->inArgs;

            IcssgStats_resetMacPortStats(hStats, macPort);
        }
        break;

        default:
        {
            ENETTRACE_ERR("%s: IOCTL cmd %u not supported\n", hMod->name, cmd);
            status = ENET_EUNKNOWNIOCTL;
        }
        break;
    }

    return status;
}

static void IcssgStats_getPaStats(IcssgStats_Handle hStats,
                                  IcssgStats_Pa *paStats)
{
    uintptr_t baseAddr = (uintptr_t)hStats->paStatsAddr;
    uint32_t *stats32 = (uint32_t *)&paStats->hostRxPktCnt;
    uint32_t i;

    /* First two PA statistics counters are 64-bit values */
    paStats->hostRxByteCnt = CSL_REG64_RD(baseAddr);
    paStats->hostTxByteCnt = CSL_REG64_RD(baseAddr + sizeof(uint64_t));

    /* Rest of PA statistics counters are 32-bit values */
    for (i = 0U; i < ENET_ARRAYSIZE(IcssgStats_paStatsOffset); i++)
    {
        *stats32 = CSL_REG32_RD(baseAddr + IcssgStats_paStatsOffset[i]);
        stats32++;
    }
}

static void IcssgStats_getMacPortStats(IcssgStats_Handle hStats,
                                       Enet_MacPort macPort,
                                       IcssgStats_MacPort *stats)
{
    uint32_t *statsRegs;
    uint32_t *stats32 = (uint32_t *)stats;
    uint32_t i;

    if (macPort == ENET_MAC_PORT_1)
    {
        statsRegs = (uint32_t *)hStats->port1Addr;
    }
    else
    {
        statsRegs = (uint32_t *)hStats->port2Addr;
    }

    for (i = 0U; i < sizeof(IcssgStats_MacPort) / sizeof(uint32_t); i++)
    {
        stats32[i] = CSL_REG32_RD(&statsRegs[i]);
    }
}

static void IcssgStats_resetMacPortStats(IcssgStats_Handle hStats,
                                         Enet_MacPort macPort)
{
    uint32_t *statsRegs;
    uint32_t stats32;
    uint32_t i;

    if (macPort == ENET_MAC_PORT_1)
    {
        statsRegs = (uint32_t *)hStats->port1Addr;
    }
    else
    {
        statsRegs = (uint32_t *)hStats->port2Addr;
    }

    for (i = 0U; i < sizeof(IcssgStats_MacPort) / sizeof(uint32_t); i++)
    {
        stats32 = CSL_REG32_RD(&statsRegs[i]);
        CSL_REG32_WR(&statsRegs[i], stats32);
    }
}
