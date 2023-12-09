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
 * \file timeSync_icss_ecap_edma.c
 *
 * \brief This file contains the implementation of ECAP & EDMA functions
 *        for ICSS, these are used for drift correction in legacy devices
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <string.h>
#include <assert.h>

/* OSAL Header files */
#include <ti/osal/osal.h>
#include <ti/osal/soc/osal_soc.h>

#include <ti/transport/timeSync/v2/src/ip/icss/timeSync_icss_ecap_edma.h>

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/**PTP EDMA scratch memory offsets in L3 OCMC*/
#define IEP_CONFIG_ADJ_OFFSET               0xC200

/**
 * @def ECAP_CLR_CONFIG_OFFSET
 *      L3 OCMC memory offset where
 *      ECAP Config used for IEP adjustment is stored
 *      Size is 4 bytes
 */
#define ECAP_CLR_CONFIG_OFFSET              IEP_CONFIG_ADJ_OFFSET + 0x10

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/**
 *  @internal
 *  @brief Initialize ECAP module
 *
 *  @param args timeSyncHandle Pointer to PTP Handle
 *
 *  @retval Error value
 *
 */
static int32_t TimeSync_edmaConfig(ICSS_EmacHandle emacHandle,
                                   EdmaConfig *edmaConfig);
/**
 *  @internal
 *  @brief Initialize ECAP module
 *
 *  @param args timeSyncHandle Pointer to PTP Handle
 *
 *  @retval None
 *
 */
static void TimeSync_initEcap(EdmaConfig *edmaConfig);

/**
 *  @internal
 *  @brief Writes compensation value to scratch memory and calls API to reconfigure ECAP
 *
 *  @param args timeSyncHandle Pointer to PTP Handle
 *  @param args ecapPeriod ECAP period
 *  @param args compensation IEP compensation value to be written into HW reg
 *
 *  @retval None
 *
 */
static void TimeSync_syncIepAdjustment(ICSS_EmacHandle emacHandle,
                                       EdmaConfig *edmaConfig,
                                       int32_t ecapPeriod,
                                       uint32_t compensation);

/**
 *  @internal
 *  @brief As the name indicates it configures EDMA PaRAM entries
 *  @param args timeSyncHandle Pointer to PTP Handle
 *
 *  @retval None
 *
 */
static void TimeSync_setIepEdmaPaRamEntry(ICSS_EmacHandle emacHandle,
                                          EdmaConfig *edmaConfig);

/**
 *  @internal
 *  @brief As the name indicates it reconfigures the ECAP module
 *  @param args timeSyncHandle Pointer to PTP Handle
 *
 *  @retval None
 *
 */
static void TimeSync_reconfigEcap(EdmaConfig *edmaConfig,
                                  int32_t period);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t TimeSync_ecapEdmaConfig(ICSS_EmacHandle emacHandle,
                                EdmaConfig *edmaConfig)
{
    TimeSync_initEcap(edmaConfig);
    return (TimeSync_edmaConfig(emacHandle, edmaConfig));
}

void TimeSync_adjTimeEcapEdma(ICSS_EmacHandle emacHandle,
                              EdmaConfig *edmaConfig,
                              int32_t adjOffset,
                              uint32_t syncInterval)
{
    int32_t ecapPeriod = 0;

    if (adjOffset == 0)
    {
        /*No adjustment*/
        ecapPeriod = syncInterval;
        TimeSync_syncIepAdjustment(emacHandle, edmaConfig, ecapPeriod, 0x00000551);
    }
    else
    {
        ecapPeriod = MULTIPLICATION_FACTOR * (syncInterval / abs(adjOffset));

        /* slave is faster*/
        if (adjOffset < 0)
        {
            TimeSync_syncIepAdjustment(emacHandle, edmaConfig, ecapPeriod, 0x00000a51);
        }
        /* master is faster*/
        else
        {
            TimeSync_syncIepAdjustment(emacHandle, edmaConfig, ecapPeriod, 0x00000051);
        }
    }
}

/* configure the eCAP0 timer in
   1. sync packet received => deltaT calculation => write 0x0451 /0x0651 in a specific memory location
   2. calculate period for ECAP(i.e. update period) => write ecap period and start ecap
   3. ecap will periocally generate interrupt => EDMA is triggered => writes 451/651 in IEP register and clear ecap bits
   repeate step 1 to 3*/
static void TimeSync_initEcap(EdmaConfig *edmaConfig)
{
    edmaConfig->ecapPwmssBaseAddr = SOC_PWMSS1_IECAP_BASE;
    edmaConfig->edmaEcapEvt0ChNum = 38U;
    edmaConfig->edmaEcapEvt0LinkChNum = 39U;
    edmaConfig->edmaEcapClearChNum = 40U;
    edmaConfig->edmaEcapClearLinkChNum = 41U;

    /*scratch memory for ECAP EDMA IEP compensation scheme
     * the offsets are in L3 OCMC RAM and must not overlap with memory
     * used for queues*/
    edmaConfig->iep_config_adj_offset  = IEP_CONFIG_ADJ_OFFSET;
    edmaConfig->iep_ecap_config_offset = ECAP_CLR_CONFIG_OFFSET;

    /* Putting ECAP in APWM mode, stop TSCTR if running */
    uint32_t pwmssBaseAddress = edmaConfig->ecapPwmssBaseAddr;
    HW_WR_FIELD16(pwmssBaseAddress + CSL_ECAP_ECCTL2,
                  CSL_ECAP_ECCTL2_CAP_APWM, CSL_ECAP_ECCTL2_CAP_APWM_APWM_MODE);

    HW_WR_FIELD16(pwmssBaseAddress + CSL_ECAP_ECCTL2,
                  CSL_ECAP_ECCTL2_STOP_WRAP, CSL_ECAP_ECCTL2_STOP_WRAP_CAPT_EVT4);

    /* Set TSCTR to 0 */
    HW_WR_FIELD32(pwmssBaseAddress + CSL_ECAP_TSCTR,
                  CSL_ECAP_TSCTR_TSCTR, CSL_ECAP_TSCTR_TSCTR_RESETVAL);

    /*Load 2560 ns , 2.56 us Tick Time, 200 = 2560/5 = 512 =hex 200*/
    HW_WR_FIELD32(pwmssBaseAddress + CSL_ECAP_CAP1,
                  CSL_ECAP_CAP1_CAP1, (250000 / 10 / 5 - 1));

    /* Clear the Period Event Flag      0x00c1*/
    HW_WR_FIELD16(pwmssBaseAddress + CSL_ECAP_ECCLR,
                  CSL_ECAP_ECCLR_INT, CSL_ECAP_ECCLR_INT_CLEAR);

    HW_WR_FIELD16(pwmssBaseAddress + CSL_ECAP_ECCLR,
                  CSL_ECAP_ECCLR_PRDEQ, CSL_ECAP_ECCLR_PRDEQ_CLEAR);

    HW_WR_FIELD16(pwmssBaseAddress + CSL_ECAP_ECCLR,
                  CSL_ECAP_ECCLR_CMPEQ, CSL_ECAP_ECCLR_CMPEQ_CLEAR);

    /* Enable Period as source of interrupt */
    HW_WR_FIELD16(pwmssBaseAddress + CSL_ECAP_ECEINT,
                  CSL_ECAP_ECEINT_PRDEQ, CSL_ECAP_ECEINT_PRDEQ_ENABLE);
}

static int32_t TimeSync_edmaConfig(ICSS_EmacHandle emacHandle,
                                   EdmaConfig *edmaConfig)
{
    EDMA3_DRV_Result edmaResult = 0;
    uint32_t ocmcBaseAddress = ((ICSS_EmacHwAttrs *)(emacHandle)->hwAttrs)->emacBaseAddrCfg->l3OcmcBaseAddr;

    edmaConfig->gEdmaHandle = (EDMA3_RM_Handle)edma3init(0, &edmaResult);

    if (edmaResult != EDMA3_DRV_SOK)
    {
        /* Report EDMA Error */
        return TIMESYNC_DMA_INIT_FAILED;
    }

    HWREG(ocmcBaseAddress + edmaConfig->iep_config_adj_offset + 0) =
        0x00000551;
    HWREG(ocmcBaseAddress + edmaConfig->iep_config_adj_offset + 4) =
        0x00000000;
    HWREG(ocmcBaseAddress + edmaConfig->iep_config_adj_offset + 8) =
        0x00000001;

    edmaConfig->edmaEcapEvt0tcc = EDMA3_DRV_TCC_ANY;
    edmaResult = EDMA3_DRV_requestChannel(edmaConfig->gEdmaHandle,
                                          &(edmaConfig->edmaEcapEvt0ChNum),
                                          &(edmaConfig->edmaEcapEvt0tcc),
                                          (EDMA3_RM_EventQueue)0,
                                          NULL, NULL);

    if (edmaResult == 0)
    {
        edmaConfig->edmaEcapEvt0Linktcc = EDMA3_DRV_TCC_ANY;
        edmaResult = EDMA3_DRV_requestChannel(edmaConfig->gEdmaHandle,
                                              &(edmaConfig->edmaEcapEvt0LinkChNum),
                                              &(edmaConfig->edmaEcapEvt0Linktcc), 0,
                                              NULL, NULL);
    }

    if (edmaResult == 0)
    {
        edmaConfig->edmaEcapCleartcc = EDMA3_DRV_TCC_ANY;
        edmaResult = EDMA3_DRV_requestChannel(edmaConfig->gEdmaHandle,
                                              &(edmaConfig->edmaEcapClearChNum),
                                              &(edmaConfig->edmaEcapCleartcc),
                                              (EDMA3_RM_EventQueue)0,
                                              NULL, NULL);
    }

    if (edmaResult == 0)
    {
        edmaConfig->edmaEcapClearLinktcc = EDMA3_DRV_TCC_ANY;
        edmaResult = EDMA3_DRV_requestChannel(edmaConfig->gEdmaHandle,
                                              &(edmaConfig->edmaEcapClearLinkChNum),
                                              &(edmaConfig->edmaEcapClearLinktcc),
                                              (EDMA3_RM_EventQueue)0,
                                              NULL, NULL);
    }

    if (edmaResult == 0)
    {
        TimeSync_setIepEdmaPaRamEntry(emacHandle, edmaConfig);
    }

    return TIMESYNC_OK;
}

static void TimeSync_setIepEdmaPaRamEntry(ICSS_EmacHandle emacHandle,
                                          EdmaConfig *edmaConfig)
{
    ICSS_EmacBaseAddressHandle_T emacBaseAddr = ((ICSS_EmacHwAttrs *)(
                                                                      emacHandle)->hwAttrs)->emacBaseAddrCfg;
    uint32_t ocmcBaseAddress = emacBaseAddr->l3OcmcBaseAddr;
    int32_t result = EDMA3_DRV_SOK;
    EDMA3_DRV_PaRAMRegs paramSet = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    EDMA3_DRV_ChainOptions chain = {EDMA3_DRV_TCCHEN_DIS,
                                    EDMA3_DRV_ITCCHEN_DIS,
                                    EDMA3_DRV_TCINTEN_DIS,
                                    EDMA3_DRV_ITCINTEN_DIS};

    EDMA3_DRV_getPaRAM((EDMA3_DRV_Handle)edmaConfig->gEdmaHandle,
                       (uint32_t)edmaConfig->edmaEcapEvt0ChNum, &paramSet);

    HWREGH(ocmcBaseAddress + edmaConfig->iep_ecap_config_offset) =
        0x0041;  /* PRDEQ (6) and INT (0) bits*/
    paramSet.srcAddr = (ocmcBaseAddress +
                        edmaConfig->iep_config_adj_offset);
    paramSet.destAddr = (uint32_t)emacBaseAddr->prussIepRegs;

    paramSet.aCnt = 3 * 4;
    paramSet.bCnt = 1;
    paramSet.cCnt = 1;

    paramSet.srcBIdx = 0;
    paramSet.srcCIdx = 0;

    paramSet.destBIdx = 0;
    paramSet.destCIdx = 0;

    paramSet.bCntReload = (uint16_t)0;
    paramSet.linkAddr = edmaConfig->edmaEcapEvt0LinkChNum;
    paramSet.bCntReload = (uint16_t)0;
    paramSet.linkAddr = 0xFFFFu;
    paramSet.opt = 0x0u;
    /* Src is in INCR mode & Dest is in FIFO modes                            */

    paramSet.opt &= 0xFFFFFFFCu;
    /* Program the TCC */
    paramSet.opt |= (((edmaConfig->edmaEcapEvt0tcc) <<
                      OPT_TCC_SHIFT) &
                     OPT_TCC_MASK);
    paramSet.opt &= 0xFFFFFFFBu;
    result = EDMA3_DRV_setPaRAM(edmaConfig->gEdmaHandle,
                                edmaConfig->edmaEcapEvt0ChNum, &paramSet);

    if (result == 0)
    {
        paramSet.srcAddr = (ocmcBaseAddress +
                            edmaConfig->iep_config_adj_offset);
        paramSet.destAddr = (uint32_t)emacBaseAddr->prussIepRegs;
        paramSet.aCnt = 3 * 4;
        paramSet.bCnt = 1;
        paramSet.cCnt = 1;

        paramSet.srcBIdx = 0;
        paramSet.srcCIdx = 0;

        paramSet.destBIdx = 0;
        paramSet.destCIdx = 0;

        paramSet.bCntReload = (uint16_t)0;
        paramSet.bCntReload = (uint16_t)0;
        paramSet.linkAddr = 0xFFFFu;
        paramSet.opt = 0x0u;
        /* Src is in INCR mode & Dest is in FIFO modes                            */
        paramSet.opt &= 0xFFFFFFFCu;
        /* Program the TCC */
        paramSet.opt |= (((edmaConfig->edmaEcapEvt0Linktcc) <<
                          OPT_TCC_SHIFT) &
                         OPT_TCC_MASK);
        paramSet.opt &= 0xFFFFFFFBu;
        result = EDMA3_DRV_setPaRAM(edmaConfig->gEdmaHandle,
                                    edmaConfig->edmaEcapEvt0LinkChNum, &paramSet);
    }

    if (result == 0)
    {
        paramSet.srcAddr = (uint32_t)(uint32_t)(ocmcBaseAddress +
                                                edmaConfig->iep_ecap_config_offset);
        paramSet.destAddr = (uint32_t)(edmaConfig->ecapPwmssBaseAddr +
                                       CSL_ECAP_ECCLR);
        paramSet.aCnt = 2;
        paramSet.bCnt = 1;
        paramSet.cCnt = 1;

        paramSet.srcBIdx = 0;
        paramSet.srcCIdx = 0;

        paramSet.destBIdx = 0;
        paramSet.destCIdx = 0;

        paramSet.bCntReload = (uint16_t)0;
        paramSet.bCntReload = (uint16_t)0;
        paramSet.linkAddr = 0xFFFFu;
        paramSet.opt = 0x0u;
        /* Src is in INCR mode & Dest is in FIFO modes                            */
        paramSet.opt &= 0xFFFFFFFCu;
        /* Program the TCC */
        paramSet.opt |= (((edmaConfig->edmaEcapCleartcc) <<
                          OPT_TCC_SHIFT) &
                         OPT_TCC_MASK);

        paramSet.opt |= (1 << OPT_SYNCDIM_SHIFT);
        result = EDMA3_DRV_setPaRAM(edmaConfig->gEdmaHandle,
                                    edmaConfig->edmaEcapClearChNum, &paramSet);
    }

    if (result == 0)
    {
        paramSet.srcAddr = (uint32_t)(uint32_t)(ocmcBaseAddress +
                                                edmaConfig->iep_ecap_config_offset);
        paramSet.destAddr = (uint32_t)(edmaConfig->ecapPwmssBaseAddr +
                                       CSL_ECAP_ECCLR);
        paramSet.aCnt = 2;
        paramSet.bCnt = 1;
        paramSet.cCnt = 1;

        paramSet.srcBIdx = 0;
        paramSet.srcCIdx = 0;

        paramSet.destBIdx = 0;
        paramSet.destCIdx = 0;

        paramSet.bCntReload = (uint16_t)0;
        paramSet.bCntReload = (uint16_t)0;
        paramSet.linkAddr = 0xFFFFu;
        paramSet.opt = 0x0u;
        /* Src is in INCR mode & Dest is in FIFO modes                            */
        paramSet.opt &= 0xFFFFFFFCu;
        /* Program the TCC */
        paramSet.opt |= ((edmaConfig->edmaEcapClearLinktcc <<
                          OPT_TCC_SHIFT) &
                         OPT_TCC_MASK);
        paramSet.opt |= (1 << OPT_SYNCDIM_SHIFT);
        result = EDMA3_DRV_setPaRAM(edmaConfig->gEdmaHandle,
                                    edmaConfig->edmaEcapClearLinkChNum, &paramSet);
    }

    if (result == 0)
    {
        result = EDMA3_DRV_linkChannel((EDMA3_DRV_Handle)
                                       edmaConfig->gEdmaHandle,
                                       (uint32_t)edmaConfig->edmaEcapEvt0ChNum,
                                       (uint32_t)edmaConfig->edmaEcapEvt0LinkChNum);
        result = EDMA3_DRV_linkChannel((EDMA3_DRV_Handle)
                                       edmaConfig->gEdmaHandle,
                                       (uint32_t)edmaConfig->edmaEcapEvt0LinkChNum,
                                       (uint32_t)edmaConfig->edmaEcapEvt0ChNum);
    }

    if (result == 0)
    {
        result = EDMA3_DRV_linkChannel((EDMA3_DRV_Handle)
                                       edmaConfig->gEdmaHandle,
                                       (uint32_t)edmaConfig->edmaEcapClearChNum,
                                       (uint32_t)edmaConfig->edmaEcapClearLinkChNum);
        result = EDMA3_DRV_linkChannel((EDMA3_DRV_Handle)
                                       edmaConfig->gEdmaHandle,
                                       (uint32_t)edmaConfig->edmaEcapClearLinkChNum,
                                       (uint32_t)edmaConfig->edmaEcapClearChNum);
    }

    if (result == 0)
    {
        /* Transfer complete chaining enable. */
        chain.tcchEn = EDMA3_DRV_TCCHEN_EN;
        /* Intermediate transfer complete chaining enable. */
        chain.itcchEn = EDMA3_DRV_ITCCHEN_EN;
        /* Transfer complete interrupt is enabled. */
        chain.tcintEn = EDMA3_DRV_TCINTEN_DIS;
        /* Intermediate transfer complete interrupt is disabled. */
        chain.itcintEn = EDMA3_DRV_ITCINTEN_DIS;

        result = EDMA3_DRV_chainChannel(edmaConfig->gEdmaHandle,
                                        edmaConfig->edmaEcapEvt0ChNum,
                                        edmaConfig->edmaEcapClearChNum,
                                        (EDMA3_DRV_ChainOptions *)&chain);
        result = EDMA3_DRV_chainChannel(edmaConfig->gEdmaHandle,
                                        edmaConfig->edmaEcapEvt0LinkChNum,
                                        edmaConfig->edmaEcapClearChNum,
                                        (EDMA3_DRV_ChainOptions *)&chain);
    }

    if (result == 0)
    {
        result = EDMA3_DRV_enableTransfer(
                                          (EDMA3_DRV_Handle)edmaConfig->gEdmaHandle,
                                          (uint32_t)edmaConfig->edmaEcapEvt0ChNum,
                                          (EDMA3_DRV_TrigMode)EDMA3_DRV_TRIG_MODE_EVENT);

        if (result != ((Int32)EDMA3_DRV_SOK))
        {
            result = -1;
        }
    }
}

static void TimeSync_reconfigEcap(EdmaConfig *edmaConfig,
                                  int32_t period)
{
    uint32_t pwmssBaseAddr = edmaConfig->ecapPwmssBaseAddr;

    /* Stop the counter */
    HW_WR_FIELD16(pwmssBaseAddr + CSL_ECAP_ECCTL2,
                  CSL_ECAP_ECCTL2_TSCTRSTOP, CSL_ECAP_ECCTL2_TSCTRSTOP_STOPPED);

    /* Set TSCTR to 0 */
    HW_WR_FIELD32(pwmssBaseAddr + CSL_ECAP_TSCTR,
                  CSL_ECAP_TSCTR_TSCTR, CSL_ECAP_TSCTR_TSCTR_RESETVAL);

    /* CRYPTO: check whether we can use shadow registers or not?
       set the period */
    HW_WR_FIELD32(pwmssBaseAddr + CSL_ECAP_CAP1,
                  CSL_ECAP_CAP1_CAP1, (((period / 5) / 2) - 1));

    /* Clear the Period Event Flag      0x00c1*/
    HW_WR_FIELD16(pwmssBaseAddr + CSL_ECAP_ECCLR,
                  CSL_ECAP_ECCLR_INT, CSL_ECAP_ECCLR_INT_CLEAR);

    HW_WR_FIELD16(pwmssBaseAddr + CSL_ECAP_ECCLR,
                  CSL_ECAP_ECCLR_PRDEQ, CSL_ECAP_ECCLR_PRDEQ_CLEAR);

    HW_WR_FIELD16(pwmssBaseAddr + CSL_ECAP_ECCLR,
                  CSL_ECAP_ECCLR_CMPEQ, CSL_ECAP_ECCLR_CMPEQ_CLEAR);

    /* Start the counter */
    HW_WR_FIELD16(pwmssBaseAddr + CSL_ECAP_ECCTL2,
                  CSL_ECAP_ECCTL2_TSCTRSTOP, CSL_ECAP_ECCTL2_TSCTRSTOP_FREE_RUNNING);
}

static void TimeSync_syncIepAdjustment(ICSS_EmacHandle emacHandle,
                                       EdmaConfig *edmaConfig,
                                       int32_t ecapPeriod,
                                       uint32_t compensation)
{
    uint32_t ocmcBaseAddress = ((ICSS_EmacHwAttrs *)(emacHandle)->hwAttrs)->emacBaseAddrCfg->l3OcmcBaseAddr;

    HWREG(ocmcBaseAddress + edmaConfig->iep_config_adj_offset + 0) = compensation;
    TimeSync_reconfigEcap(edmaConfig, ecapPeriod);
}
