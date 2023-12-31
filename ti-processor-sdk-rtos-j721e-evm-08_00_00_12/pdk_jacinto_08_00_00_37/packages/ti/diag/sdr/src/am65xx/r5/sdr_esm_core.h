/*
 * SDR ESM
 *
 * Software Diagnostics Reference module for Error Signaling Module
 *
 *  Copyright (c) Texas Instruments Incorporated 2018-2020
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
 */

#ifndef INCLUDE_SDR_ESM_CORE_H_
#define INCLUDE_SDR_ESM_CORE_H_

#include <ti/csl/csl_esm.h>

/* Enumerate Interrupt number for the different esm interrupts */
#define SDR_MCU_ESM_HI_INTNO   CSL_MCU0_INTR_ESM1_ESM_INT_HI_LVL
#define SDR_MCU_ESM_LO_INTNO   CSL_MCU0_INTR_ESM1_ESM_INT_LOW_LVL
#define SDR_MCU_ESM_CFG_INTNO  CSL_MCU0_INTR_ESM1_ESM_INT_CFG_LVL

#define SDR_WKUP_ESM_HI_INTNO  CSL_MCU0_INTR_ESM0_ESM_INT_HI_LVL
#define SDR_WKUP_ESM_LO_INTNO  CSL_MCU0_INTR_ESM0_ESM_INT_LOW_LVL
#define SDR_WKUP_ESM_CFG_INTNO CSL_MCU0_INTR_ESM0_ESM_INT_CFG_LVL

#define SDR_MAIN_ESM_HI_INTNO  CSL_MCU0_INTR_ESM2_ESM_INT_HI_LVL
#define SDR_MAIN_ESM_LO_INTNO  CSL_MCU0_INTR_ESM2_ESM_INT_LOW_LVL
#define SDR_MAIN_ESM_CFG_INTNO CSL_MCU0_INTR_ESM2_ESM_INT_CFG_LVL

/* Enumerate ESM events for R5F core */
#define SDR_ESM_MCU_PLL_REF_CLK_LOST           CSL_ESM1_INTR_ADPLLM_HSDIV_WRAP_ADPLLM_HSDIV_WRAP_MCU_0_LOSSREF_IPCFG
#define SDR_ESM_MCU_PLL_PHASE_LK_LOST          CSL_ESM1_INTR_ADPLLM_HSDIV_WRAP_ADPLLM_HSDIV_WRAP_MCU_0_PHASELOCK_N
#define SDR_ESM_MCU_PLL_FREQ_LK_LOST           CSL_ESM1_INTR_ADPLLM_HSDIV_WRAP_ADPLLM_HSDIV_WRAP_MCU_0_FREQLOCK_N
#define SDR_ESM_MCU_PLL_HIGH_JITTER            CSL_ESM1_INTR_ADPLLM_HSDIV_WRAP_ADPLLM_HSDIV_WRAP_MCU_0_HIGHJITTER_IPCFG
#define SDR_ESM_MCU_PLL_RECAL_REQ              CSL_ESM1_INTR_ADPLLM_HSDIV_WRAP_ADPLLM_HSDIV_WRAP_MCU_0_RECAL_IPCFG
#define SDR_ESM_CPSW_PLL_REF_CLK_LOST          CSL_ESM1_INTR_ADPLLM_HSDIV_WRAP_ADPLLM_HSDIV_WRAP_MCU_1_LOSSREF_IPCFG
#define SDR_ESM_CPSW_PLL_PHASE_LK_LOST         CSL_ESM1_INTR_ADPLLM_HSDIV_WRAP_ADPLLM_HSDIV_WRAP_MCU_1_PHASELOCK_N
#define SDR_ESM_CPSW_PLL_FREQ_LK_LOST          CSL_ESM1_INTR_ADPLLM_HSDIV_WRAP_ADPLLM_HSDIV_WRAP_MCU_1_FREQLOCK_N
#define SDR_ESM_CPSW_PLL_HIGH_JITTER           CSL_ESM1_INTR_ADPLLM_HSDIV_WRAP_ADPLLM_HSDIV_WRAP_MCU_1_HIGHJITTER_IPCFG
#define SDR_ESM_CPSW_PLL_RECAL_REQ             CSL_ESM1_INTR_ADPLLM_HSDIV_WRAP_ADPLLM_HSDIV_WRAP_MCU_1_RECAL_IPCFG
#define SDR_ESM_MCU_ADC_0_SEC_INT              CSL_ESM1_INTR_ADC0_ECC_CORRECTED_ERR_LEVEL
#define SDR_ESM_MCU_ADC_0_DED_INT              CSL_ESM1_INTR_ADC0_ECC_UNCORRECTED_ERR_LEVEL
#define SDR_ESM_MCU_ADC_1_SEC_INT              CSL_ESM1_INTR_ADC1_ECC_CORRECTED_ERR_LEVEL
#define SDR_ESM_MCU_ADC_1_DED_INT              CSL_ESM1_INTR_ADC1_ECC_UNCORRECTED_ERR_LEVEL
#define SDR_ESM_MCU_CPSW_ECC_SEC_INT           CSL_ESM1_INTR_CPSW0_ECC_SEC_PEND
#define SDR_ESM_MCU_CPSW_ECC_DED_INT           CSL_ESM1_INTR_CPSW0_ECC_DED_PEND
#define SDR_ESM_MCU_MCAN_0_SEC_INT             CSL_ESM1_INTR_MCAN0_MCANSS_ECC_CORR_LVL_INT
#define SDR_ESM_MCU_MCAN_0_DED_INT             CSL_ESM1_INTR_MCAN0_MCANSS_ECC_UNCORR_LVL_INT
#define SDR_ESM_MCU_MCAN_1_SEC_INT             CSL_ESM1_INTR_MCAN1_MCANSS_ECC_CORR_LVL_INT
#define SDR_ESM_MCU_MCAN_1_DED_INT             CSL_ESM1_INTR_MCAN1_MCANSS_ECC_UNCORR_LVL_INT
#define SDR_ESM_MCU_OSPI_0_SEC_INT             CSL_ESM1_INTR_FSS0_OSPI0_ECC_CORR_LVL_INTR
#define SDR_ESM_MCU_OSPI_0_DED_INT             CSL_ESM1_INTR_FSS0_OSPI0_ECC_UNCORR_LVL_INTR
#define SDR_ESM_MCU_OSPI_1_SEC_INT             CSL_ESM1_INTR_FSS0_OSPI1_ECC_CORR_LVL_INTR
#define SDR_ESM_MCU_OSPI_1_DED_INT             CSL_ESM1_INTR_FSS0_OSPI1_ECC_UNCORR_LVL_INTR
#define SDR_ESM_MCU_HYPFLSH_SEC_INT            CSL_ESM1_INTR_FSS0_HPB_ECC_CORR_LEVEL
#define SDR_ESM_MCU_HYPFLSH_DED_INT            CSL_ESM1_INTR_FSS0_HPB_ECC_UNCORR_LEVEL
#define SDR_ESM_MCU_FSS_ECC_ERR_INT            CSL_ESM1_INTR_FSS0_ECC_INTR_ERR_PEND_0
#define SDR_ESM_MCU_FSAS_ECC_ERR_INT           CSL_ESM1_INTR_FSS0_FSAS_ECC_INTR_ERR_PEND
#define SDR_ESM_MCU_R5_CORE0_SEC_INT           CSL_ESM1_INTR_MCU0_CPU0_ECC_CORRECTED_LEVEL
#define SDR_ESM_MCU_R5_CORE0_DED_INT           CSL_ESM1_INTR_MCU0_CPU0_ECC_UNCORRECTED_LEVEL
#define SDR_ESM_MCU_R5_CORE1_SEC_INT           CSL_ESM1_INTR_MCU0_CPU1_ECC_CORRECTED_LEVEL
#define SDR_ESM_MCU_R5_CORE1_DED_INT           CSL_ESM1_INTR_MCU0_CPU1_ECC_UNCORRECTED_LEVEL
#define SDR_ESM_MCU_R5_CORE0_RAT_EXP_INT       CSL_ESM1_INTR_MCU0_CPU0_EXP_INTR_0
#define SDR_ESM_MCU_R5_CORE1_RAT_EXP_INT       CSL_ESM1_INTR_MCU0_CPU1_EXP_INTR_0
#define SDR_ESM_MCU_CPU_MISCOMPARE             CSL_ESM1_INTR_MCU0_CPU_MISCOMPARE_LEVEL
#define SDR_ESM_MCU_NAVSS_MODULE_SEC_INT       CSL_ESM1_INTR_NAVSS0_MODSS_ECC_SEC_PEND
#define SDR_ESM_MCU_NAVSS_MODULE_DED_INT       CSL_ESM1_INTR_NAVSS0_MODSS_ECC_DED_PEND
#define SDR_ESM_MCU_NAVSS_UDMA_SEC_INT         CSL_ESM1_INTR_NAVSS0_UDMASS_ECC_SEC_PEND
#define SDR_ESM_MCU_NAVSS_UDMA_DED_INT         CSL_ESM1_INTR_NAVSS0_UDMASS_ECC_DED_PEND
#define SDR_ESM_MCU_PDMA_MCU0_SEC_INT          CSL_ESM1_INTR_PDMA_MCU0_PDMA_MCU0_MCU_0_ECC_SEC_PEND
#define SDR_ESM_MCU_PDMA_MCU0_DED_INT          CSL_ESM1_INTR_PDMA_MCU0_PDMA_MCU0_MCU_0_ECC_DED_PEND
#define SDR_ESM_MCU_PDMA_MCU1_SEC_INT          CSL_ESM1_INTR_PDMA_MCU1_PDMA_MCU1_MCU_0_ECC_SEC_PEND
#define SDR_ESM_MCU_PDMA_MCU1_DED_INT          CSL_ESM1_INTR_PDMA_MCU1_PDMA_MCU1_MCU_0_ECC_DED_PEND
#define SDR_ESM_MCU_TIMER_0_INT                CSL_ESM1_INTR_TIMER0_INTR_PEND
#define SDR_ESM_MCU_TIMER_1_INT                CSL_ESM1_INTR_TIMER1_INTR_PEND
#define SDR_ESM_MCU_TIMER_2_INT                CSL_ESM1_INTR_TIMER2_INTR_PEND
#define SDR_ESM_MCU_TIMER_3_INT                CSL_ESM1_INTR_TIMER3_INTR_PEND
#define SDR_ESM_MCU_RTI0_WWD_INT               CSL_ESM1_INTR_RTI0_INTR_WWD
#define SDR_ESM_MCU_RTI1_WWD_INT               CSL_ESM1_INTR_RTI1_INTR_WWD
#define SDR_ESM_MCU_DCC_0_ERR                  CSL_ESM1_INTR_DCC0_INTR_ERR_LEVEL
#define SDR_ESM_MCU_DCC_1_ERR                  CSL_ESM1_INTR_DCC1_INTR_ERR_LEVEL
#define SDR_ESM_MCU_DCC_2_ERR                  CSL_ESM1_INTR_DCC2_INTR_ERR_LEVEL
#define SDR_ESM_MCU_RAM_SEC_INT                CSL_ESM1_INTR_SRAM_MCU0_ECC_CORR_LEVEL
#define SDR_ESM_MCU_RAM_DED_INT                CSL_ESM1_INTR_SRAM_MCU0_ECC_UNCORR_LEVEL
#define SDR_ESM_MCU_PSRAM0_SEC_INT             CSL_ESM1_INTR_PSRAM0_ECC_CORR_LEVEL
#define SDR_ESM_MCU_PSRAM0_DED_INT             CSL_ESM1_INTR_PSRAM0_ECC_UNCORR_LEVEL
#define SDR_ESM_MCU_CLK2_ECC_SEC_INT           CSL_ESM1_INTR_M4_MCUCLK2_ECC_AGGR_M4_MCUCLK2_ECC_AGGR_MCU_0_CORR_LEVEL_0
#define SDR_ESM_MCU_CLK2_ECC_DED_INT           CSL_ESM1_INTR_M4_MCUCLK2_ECC_AGGR_M4_MCUCLK2_ECC_AGGR_MCU_0_UNCORR_LEVEL
#define SDR_ESM_MCU_CLK4_ECC_SEC_INT           CSL_ESM1_INTR_M4_MCUCLK4_ECC_AGGR_M4_MCUCLK4_ECC_AGGR_MCU_0_CORR_LEVEL_0
#define SDR_ESM_MCU_MCU_CLK4_ECC_DED_INT       CSL_ESM1_INTR_M4_MCUCLK4_ECC_AGGR_M4_MCUCLK4_ECC_AGGR_MCU_0_UNCORR_LEVEL
#define SDR_ESM_MAIN_ESM_ERROR_INT             CSL_ESM1_INTR_GLUE_ESM_MAIN_ERR_I_N_GLUE_ESM_MAIN_ERR_I_N_ESM_MAIN_ERR_I_N
#define SDR_ESM_MCU_R5_SELFTEST_ERR_INT        CSL_ESM1_MCU0_SELFTEST_ERR_INTR
#define SDR_ESM_MCU_R5_CPU_BUS_CMP_ERR         CSL_ESM1_MCU0_CPU_CMP_ERR_INTR
#define SDR_ESM_MCU_R5_INACTIVITY_ERR_INT      CSL_ESM1_MCU0_BUS_MON_ERR_INTR
#define SDR_ESM_MCU_R5_VIM_BUS_CMP_ERR_INT     CSL_ESM1_MCU0_VIM_CMP_ERR_INTR
#define SDR_ESM_MCU_R5_CCM_STAT_ERR_INT        CSL_ESM1_MCU0_CCM_CMP_ERR_INTR

#endif /* INCLUDE_SDR_ESM_CORE_H_ */
