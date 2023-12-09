/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file sciserver_secproxyConfigData.h
 *
 * \brief SoC defines for secure proxy configs for J721S2 device
 *
 */
#ifndef SOC_J721S2_SPROXY_CONFIG_DATA_H
#define SOC_J721S2_SPROXY_CONFIG_DATA_H

/** Number of Secure Proxy Transmit (Tx) thread configurations */
#define SOC_MAX_SPT_RX_CONFIG_INSTANCES (0x4BU)

#define J721S2_NAVSS0_SEC_PROXY_0_RX_TIFS_SEC_HIGH_PRIORITY_RX_THR159_CONF000 (0x00U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_TIFS_SEC_LOW_PRIORITY_RX_THR158_CONF001 (0x01U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_TIFS_SEC_NOTIFY_RESP_RX_THR157_CONF002 (0x02U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_HSM_SEC_HIGH_PRIORITY_RX_THR140_CONF003 (0x03U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_HSM_SEC_LOW_PRIORITY_RX_THR139_CONF004 (0x04U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_HSM_SEC_NOTIFY_RESP_RX_THR138_CONF005 (0x05U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_DM_NONSEC_HIGH_PRIORITY_RX_THR137_CONF006 (0x06U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_DM_NONSEC_LOW_PRIORITY_RX_THR136_CONF007 (0x07U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_DM_NONSEC_NOTIFY_RESP_RX_THR135_CONF008 (0x08U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_A72_0_NOTIFY_THR000_CONF009 (0x09U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_A72_0_RESPONSE_THR001_CONF010 (0x0AU)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_A72_1_NOTIFY_THR005_CONF011 (0x0BU)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_A72_1_RESPONSE_THR006_CONF012 (0x0CU)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_A72_2_NOTIFY_THR010_CONF013 (0x0DU)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_A72_2_RESPONSE_THR011_CONF014 (0x0EU)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_A72_3_NOTIFY_THR015_CONF015 (0x0FU)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_A72_3_RESPONSE_THR016_CONF016 (0x10U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_A72_4_NOTIFY_THR020_CONF017 (0x11U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_A72_4_RESPONSE_THR021_CONF018 (0x12U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_C7X_0_0_NOTIFY_THR025_CONF019 (0x13U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_C7X_0_0_RESPONSE_THR026_CONF020 (0x14U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_C7X_0_1_NOTIFY_THR030_CONF021 (0x15U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_C7X_0_1_RESPONSE_THR031_CONF022 (0x16U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_C7X_1_0_NOTIFY_THR035_CONF023 (0x17U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_C7X_1_0_RESPONSE_THR036_CONF024 (0x18U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_C7X_1_1_NOTIFY_THR040_CONF025 (0x19U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_C7X_1_1_RESPONSE_THR041_CONF026 (0x1AU)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_GPU_0_NOTIFY_THR045_CONF027 (0x1BU)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_GPU_0_RESPONSE_THR046_CONF028 (0x1CU)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_0_R5_0_NOTIFY_THR050_CONF029 (0x1DU)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_0_R5_0_RESPONSE_THR051_CONF030 (0x1EU)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_0_R5_1_NOTIFY_THR055_CONF031 (0x1FU)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_0_R5_1_RESPONSE_THR056_CONF032 (0x20U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_0_R5_2_NOTIFY_THR060_CONF033 (0x21U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_0_R5_2_RESPONSE_THR061_CONF034 (0x22U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_0_R5_3_NOTIFY_THR065_CONF035 (0x23U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_0_R5_3_RESPONSE_THR066_CONF036 (0x24U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_1_R5_0_NOTIFY_THR070_CONF037 (0x25U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_1_R5_0_RESPONSE_THR071_CONF038 (0x26U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_1_R5_1_NOTIFY_THR075_CONF039 (0x27U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_1_R5_1_RESPONSE_THR076_CONF040 (0x28U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_1_R5_2_NOTIFY_THR080_CONF041 (0x29U)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_1_R5_2_RESPONSE_THR081_CONF042 (0x2AU)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_1_R5_3_NOTIFY_THR085_CONF043 (0x2BU)
#define J721S2_NAVSS0_SEC_PROXY_0_RX_MAIN_1_R5_3_RESPONSE_THR086_CONF044 (0x2CU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_TIFS_SEC_HIGH_PRIORITY_RX_THR089_CONF045 (0x2DU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_TIFS_SEC_LOW_PRIORITY_RX_THR088_CONF046 (0x2EU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_TIFS_SEC_NOTIFY_RESP_RX_THR087_CONF047 (0x2FU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_HSM_SEC_HIGH_PRIORITY_RX_THR080_CONF048 (0x30U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_HSM_SEC_LOW_PRIORITY_RX_THR079_CONF049 (0x31U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_HSM_SEC_NOTIFY_RESP_RX_THR078_CONF050 (0x32U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_DM_NONSEC_HIGH_PRIORITY_RX_THR077_CONF051 (0x33U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_DM_NONSEC_LOW_PRIORITY_RX_THR076_CONF052 (0x34U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_DM_NONSEC_NOTIFY_RESP_RX_THR075_CONF053 (0x35U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_MCU_0_R5_0_NOTIFY_THR000_CONF054 (0x36U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_MCU_0_R5_0_RESPONSE_THR001_CONF055 (0x37U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_MCU_0_R5_1_NOTIFY_THR005_CONF056 (0x38U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_MCU_0_R5_1_RESPONSE_THR006_CONF057 (0x39U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_MCU_0_R5_2_NOTIFY_THR010_CONF058 (0x3AU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_MCU_0_R5_2_RESPONSE_THR011_CONF059 (0x3BU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_MCU_0_R5_3_NOTIFY_THR015_CONF060 (0x3CU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_MCU_0_R5_3_RESPONSE_THR016_CONF061 (0x3DU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_DM2TIFS_NOTIFY_THR020_CONF062 (0x3EU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_DM2TIFS_RESPONSE_THR021_CONF063 (0x3FU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_TIFS2DM_NOTIFY_THR025_CONF064 (0x40U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_RX_TIFS2DM_RESPONSE_THR026_CONF065 (0x41U)
#define J721S2_MCU_SA3_SS0_SEC_PROXY_0_RX_TIFS_SEC_HIGH_PRIORITY_RX_THR015_CONF066 (0x42U)
#define J721S2_MCU_SA3_SS0_SEC_PROXY_0_RX_TIFS_SEC_LOW_PRIORITY_RX_THR014_CONF067 (0x43U)
#define J721S2_MCU_SA3_SS0_SEC_PROXY_0_RX_TIFS_SEC_NOTIFY_RESP_RX_THR013_CONF068 (0x44U)
#define J721S2_MCU_SA3_SS0_SEC_PROXY_0_RX_HSM_SEC_HIGH_PRIORITY_RX_THR012_CONF069 (0x45U)
#define J721S2_MCU_SA3_SS0_SEC_PROXY_0_RX_HSM_SEC_LOW_PRIORITY_RX_THR011_CONF070 (0x46U)
#define J721S2_MCU_SA3_SS0_SEC_PROXY_0_RX_HSM_SEC_NOTIFY_RESP_RX_THR010_CONF071 (0x47U)
#define J721S2_MCU_SA3_SS0_SEC_PROXY_0_RX_DM_NONSEC_HIGH_PRIORITY_RX_THR009_CONF072 (0x48U)
#define J721S2_MCU_SA3_SS0_SEC_PROXY_0_RX_DM_NONSEC_LOW_PRIORITY_RX_THR008_CONF073 (0x49U)
#define J721S2_MCU_SA3_SS0_SEC_PROXY_0_RX_DM_NONSEC_NOTIFY_RESP_RX_THR007_CONF074 (0x4AU)

/** Number of Secure Proxy Transmit (Rx) thread configurations */
#define SOC_MAX_SPT_TX_CONFIG_INSTANCES (0x78U)

#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_A72_0_NOTIFY_TX_THR156_CONF000 (0x00U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_A72_0_RESPONSE_TX_THR155_CONF001 (0x01U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_A72_1_NOTIFY_TX_THR154_CONF002 (0x02U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_A72_1_RESPONSE_TX_THR153_CONF003 (0x03U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_C7X_0_0_NOTIFY_TX_THR152_CONF004 (0x04U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_C7X_0_0_RESPONSE_TX_THR151_CONF005 (0x05U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_C7X_1_0_NOTIFY_TX_THR150_CONF006 (0x06U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_C7X_1_0_RESPONSE_TX_THR149_CONF007 (0x07U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_MAIN_0_R5_1_NOTIFY_TX_THR148_CONF008 (0x08U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_MAIN_0_R5_1_RESPONSE_TX_THR147_CONF009 (0x09U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_MAIN_0_R5_3_NOTIFY_TX_THR146_CONF010 (0x0AU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_MAIN_0_R5_3_RESPONSE_TX_THR145_CONF011 (0x0BU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_MAIN_1_R5_1_NOTIFY_TX_THR144_CONF012 (0x0CU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_MAIN_1_R5_1_RESPONSE_TX_THR143_CONF013 (0x0DU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_MAIN_1_R5_3_NOTIFY_TX_THR142_CONF014 (0x0EU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_TIFS_SEC_MAIN_1_R5_3_RESPONSE_TX_THR141_CONF015 (0x0FU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_A72_2_NOTIFY_TX_THR134_CONF016 (0x10U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_A72_2_RESPONSE_TX_THR133_CONF017 (0x11U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_A72_3_NOTIFY_TX_THR132_CONF018 (0x12U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_A72_3_RESPONSE_TX_THR131_CONF019 (0x13U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_A72_4_NOTIFY_TX_THR130_CONF020 (0x14U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_A72_4_RESPONSE_TX_THR129_CONF021 (0x15U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_C7X_0_1_NOTIFY_TX_THR128_CONF022 (0x16U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_C7X_0_1_RESPONSE_TX_THR127_CONF023 (0x17U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_C7X_1_1_NOTIFY_TX_THR126_CONF024 (0x18U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_C7X_1_1_RESPONSE_TX_THR125_CONF025 (0x19U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_GPU_0_NOTIFY_TX_THR124_CONF026 (0x1AU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_GPU_0_RESPONSE_TX_THR123_CONF027 (0x1BU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_MAIN_0_R5_0_NOTIFY_TX_THR122_CONF028 (0x1CU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_MAIN_0_R5_0_RESPONSE_TX_THR121_CONF029 (0x1DU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_MAIN_0_R5_2_NOTIFY_TX_THR120_CONF030 (0x1EU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_MAIN_0_R5_2_RESPONSE_TX_THR119_CONF031 (0x1FU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_MAIN_1_R5_0_NOTIFY_TX_THR118_CONF032 (0x20U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_MAIN_1_R5_0_RESPONSE_TX_THR117_CONF033 (0x21U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_MAIN_1_R5_2_NOTIFY_TX_THR116_CONF034 (0x22U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_DM_NONSEC_MAIN_1_R5_2_RESPONSE_TX_THR115_CONF035 (0x23U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_A72_0_HIGH_PRIORITY_THR002_CONF036 (0x24U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_A72_0_LOW_PRIORITY_THR003_CONF037 (0x25U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_A72_0_NOTIFY_RESP_THR004_CONF038 (0x26U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_A72_1_HIGH_PRIORITY_THR007_CONF039 (0x27U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_A72_1_LOW_PRIORITY_THR008_CONF040 (0x28U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_A72_1_NOTIFY_RESP_THR009_CONF041 (0x29U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_A72_2_HIGH_PRIORITY_THR012_CONF042 (0x2AU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_A72_2_LOW_PRIORITY_THR013_CONF043 (0x2BU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_A72_2_NOTIFY_RESP_THR014_CONF044 (0x2CU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_A72_3_HIGH_PRIORITY_THR017_CONF045 (0x2DU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_A72_3_LOW_PRIORITY_THR018_CONF046 (0x2EU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_A72_3_NOTIFY_RESP_THR019_CONF047 (0x2FU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_A72_4_HIGH_PRIORITY_THR022_CONF048 (0x30U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_A72_4_LOW_PRIORITY_THR023_CONF049 (0x31U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_A72_4_NOTIFY_RESP_THR024_CONF050 (0x32U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_C7X_0_0_HIGH_PRIORITY_THR027_CONF051 (0x33U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_C7X_0_0_LOW_PRIORITY_THR028_CONF052 (0x34U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_C7X_0_0_NOTIFY_RESP_THR029_CONF053 (0x35U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_C7X_0_1_HIGH_PRIORITY_THR032_CONF054 (0x36U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_C7X_0_1_LOW_PRIORITY_THR033_CONF055 (0x37U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_C7X_0_1_NOTIFY_RESP_THR034_CONF056 (0x38U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_C7X_1_0_HIGH_PRIORITY_THR037_CONF057 (0x39U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_C7X_1_0_LOW_PRIORITY_THR038_CONF058 (0x3AU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_C7X_1_0_NOTIFY_RESP_THR039_CONF059 (0x3BU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_C7X_1_1_HIGH_PRIORITY_THR042_CONF060 (0x3CU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_C7X_1_1_LOW_PRIORITY_THR043_CONF061 (0x3DU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_C7X_1_1_NOTIFY_RESP_THR044_CONF062 (0x3EU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_GPU_0_HIGH_PRIORITY_THR047_CONF063 (0x3FU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_GPU_0_LOW_PRIORITY_THR048_CONF064 (0x40U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_GPU_0_NOTIFY_RESP_THR049_CONF065 (0x41U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_0_R5_0_HIGH_PRIORITY_THR052_CONF066 (0x42U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_0_R5_0_LOW_PRIORITY_THR053_CONF067 (0x43U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_0_R5_0_NOTIFY_RESP_THR054_CONF068 (0x44U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_0_R5_1_HIGH_PRIORITY_THR057_CONF069 (0x45U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_0_R5_1_LOW_PRIORITY_THR058_CONF070 (0x46U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_0_R5_1_NOTIFY_RESP_THR059_CONF071 (0x47U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_0_R5_2_HIGH_PRIORITY_THR062_CONF072 (0x48U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_0_R5_2_LOW_PRIORITY_THR063_CONF073 (0x49U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_0_R5_2_NOTIFY_RESP_THR064_CONF074 (0x4AU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_0_R5_3_HIGH_PRIORITY_THR067_CONF075 (0x4BU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_0_R5_3_LOW_PRIORITY_THR068_CONF076 (0x4CU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_0_R5_3_NOTIFY_RESP_THR069_CONF077 (0x4DU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_1_R5_0_HIGH_PRIORITY_THR072_CONF078 (0x4EU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_1_R5_0_LOW_PRIORITY_THR073_CONF079 (0x4FU)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_1_R5_0_NOTIFY_RESP_THR074_CONF080 (0x50U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_1_R5_1_HIGH_PRIORITY_THR077_CONF081 (0x51U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_1_R5_1_LOW_PRIORITY_THR078_CONF082 (0x52U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_1_R5_1_NOTIFY_RESP_THR079_CONF083 (0x53U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_1_R5_2_HIGH_PRIORITY_THR082_CONF084 (0x54U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_1_R5_2_LOW_PRIORITY_THR083_CONF085 (0x55U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_1_R5_2_NOTIFY_RESP_THR084_CONF086 (0x56U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_1_R5_3_HIGH_PRIORITY_THR087_CONF087 (0x57U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_1_R5_3_LOW_PRIORITY_THR088_CONF088 (0x58U)
#define J721S2_NAVSS0_SEC_PROXY_0_TX_MAIN_1_R5_3_NOTIFY_RESP_THR089_CONF089 (0x59U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_TIFS_SEC_MCU_0_R5_1_NOTIFY_TX_THR086_CONF090 (0x5AU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_TIFS_SEC_MCU_0_R5_1_RESPONSE_TX_THR085_CONF091 (0x5BU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_TIFS_SEC_MCU_0_R5_3_NOTIFY_TX_THR084_CONF092 (0x5CU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_TIFS_SEC_MCU_0_R5_3_RESPONSE_TX_THR083_CONF093 (0x5DU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_TIFS_SEC_DM2TIFS_NOTIFY_TX_THR082_CONF094 (0x5EU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_TIFS_SEC_DM2TIFS_RESPONSE_TX_THR081_CONF095 (0x5FU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_DM_NONSEC_MCU_0_R5_0_NOTIFY_TX_THR074_CONF096 (0x60U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_DM_NONSEC_MCU_0_R5_0_RESPONSE_TX_THR073_CONF097 (0x61U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_DM_NONSEC_MCU_0_R5_2_NOTIFY_TX_THR072_CONF098 (0x62U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_DM_NONSEC_MCU_0_R5_2_RESPONSE_TX_THR071_CONF099 (0x63U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_DM_NONSEC_TIFS2DM_NOTIFY_TX_THR070_CONF100 (0x64U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_DM_NONSEC_TIFS2DM_RESPONSE_TX_THR069_CONF101 (0x65U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_MCU_0_R5_0_HIGH_PRIORITY_THR002_CONF102 (0x66U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_MCU_0_R5_0_LOW_PRIORITY_THR003_CONF103 (0x67U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_MCU_0_R5_0_NOTIFY_RESP_THR004_CONF104 (0x68U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_MCU_0_R5_1_HIGH_PRIORITY_THR007_CONF105 (0x69U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_MCU_0_R5_1_LOW_PRIORITY_THR008_CONF106 (0x6AU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_MCU_0_R5_1_NOTIFY_RESP_THR009_CONF107 (0x6BU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_MCU_0_R5_2_HIGH_PRIORITY_THR012_CONF108 (0x6CU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_MCU_0_R5_2_LOW_PRIORITY_THR013_CONF109 (0x6DU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_MCU_0_R5_2_NOTIFY_RESP_THR014_CONF110 (0x6EU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_MCU_0_R5_3_HIGH_PRIORITY_THR017_CONF111 (0x6FU)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_MCU_0_R5_3_LOW_PRIORITY_THR018_CONF112 (0x70U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_MCU_0_R5_3_NOTIFY_RESP_THR019_CONF113 (0x71U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_DM2TIFS_HIGH_PRIORITY_THR022_CONF114 (0x72U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_DM2TIFS_LOW_PRIORITY_THR023_CONF115 (0x73U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_DM2TIFS_NOTIFY_RESP_THR024_CONF116 (0x74U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_TIFS2DM_HIGH_PRIORITY_THR027_CONF117 (0x75U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_TIFS2DM_LOW_PRIORITY_THR028_CONF118 (0x76U)
#define J721S2_MCU_NAVSS0_SEC_PROXY0_TX_TIFS2DM_NOTIFY_RESP_THR029_CONF119 (0x77U)

#endif /* SOC_J721S2_SPROXY_CONFIG_DATA_H */
