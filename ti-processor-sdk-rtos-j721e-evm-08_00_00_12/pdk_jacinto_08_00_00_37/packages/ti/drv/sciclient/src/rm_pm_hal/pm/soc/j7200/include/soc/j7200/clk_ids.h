/*
 * Data version: 200730_091422
 *
 * Copyright (C) 2017-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
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
#ifndef SOC_J7200_CLK_IDS_H
#define SOC_J7200_CLK_IDS_H

#define CLK_J7200_GLUELOGIC_HFOSC0_CLKOUT 1
#define CLK_J7200_J7VC_WAKEUP_16FF_WKUP_0_WKUP_RCOSC_12P5M_CLK 2
#define CLK_J7200_WKUP_FREF_CLKSEL_OUT0 3
#define CLK_J7200_PLLFRACF_SSMOD_16FFT_MCU_0_FOUTVCOP_CLK 4
#define CLK_J7200_HSDIV1_16FFT_MCU_0_HSDIVOUT0_CLK 5
#define CLK_J7200_K3_PLL_CTRL_WRAP_WKUP_0_SYSCLKOUT_CLK 6
#define CLK_J7200_K3_PLL_CTRL_WRAP_WKUP_0_CHIP_DIV1_CLK_CLK 7
#define CLK_J7200_MCU_R5_CORE0_CLKSEL_OUT0 8
#define CLK_J7200_MCU_R5_CORE1_CLKSEL_OUT0 9
#define CLK_J7200_MCU_R5_PHASE0_CLKSEL_OUT0 10
#define CLK_J7200_MCU_R5_PHASE1_CLKSEL_OUT0 11
#define CLK_J7200_BOARD_0_HFOSC1_CLK_OUT 12
#define CLK_J7200_BOARD_0_CPTS0_RFT_CLK_OUT 13
#define CLK_J7200_BOARD_0_EXT_REFCLK1_OUT 14
#define CLK_J7200_BOARD_0_MCU_CPTS0_RFT_CLK_OUT 15
#define CLK_J7200_BOARD_0_MCU_EXT_REFCLK0_OUT 16
#define CLK_J7200_BOARD_0_MCU_I2C0_SCL_OUT 17
#define CLK_J7200_BOARD_0_MCU_I2C1_SCL_OUT 18
#define CLK_J7200_BOARD_0_MCU_I3C0_SCL_OUT 19
#define CLK_J7200_BOARD_0_MCU_OSPI0_DQS_OUT 20
#define CLK_J7200_BOARD_0_MCU_RGMII1_RXC_OUT 21
#define CLK_J7200_BOARD_0_MCU_RMII1_REF_CLK_OUT 22
#define CLK_J7200_BOARD_0_WKUP_I2C0_SCL_OUT 23
#define CLK_J7200_BOARD_0_WKUP_LF_CLKIN_OUT 24
#define CLK_J7200_CPSW_2GUSS_MCU_0_CPTS_GENF0 25
#define CLK_J7200_CPSW_2GUSS_MCU_0_MDIO_MDCLK_O 26
#define CLK_J7200_CPSW_2GUSS_MCU_0_RGMII1_TXC_O 27
#define CLK_J7200_DMTIMER_DMC1MS_MCU_0_TIMER_PWM 28
#define CLK_J7200_DMTIMER_DMC1MS_MCU_2_TIMER_PWM 29
#define CLK_J7200_DMTIMER_DMC1MS_MCU_4_TIMER_PWM 30
#define CLK_J7200_DMTIMER_DMC1MS_MCU_6_TIMER_PWM 31
#define CLK_J7200_DMTIMER_DMC1MS_MCU_8_TIMER_PWM 32
#define CLK_J7200_FSS_MCU_0_HYPERBUS1P0_0_HPB_OUT_CLK_N 33
#define CLK_J7200_FSS_MCU_0_HYPERBUS1P0_0_HPB_OUT_CLK_P 34
#define CLK_J7200_FSS_MCU_0_OSPI_0_OSPI_OCLK_CLK 35
#define CLK_J7200_HSDIV1_16FFT_MCU_0_HSDIVOUT1_CLK 36
#define CLK_J7200_I3C_MCU_0_I3C_SCL_DO 37
#define CLK_J7200_J7VC_WAKEUP_16FF_WKUP_0_WKUP_RCOSC_32K_CLK 38
#define CLK_J7200_MCU_OSPI0_ICLK_SEL_OUT0 39
#define CLK_J7200_MCU_WWD_CLKSEL_OUT0 40
#define CLK_J7200_MCU_WWD_CLKSEL_OUT1 41
#define CLK_J7200_MSHSI2C_MCU_1_PORSCL 42
#define CLK_J7200_MSHSI2C_WKUP_0_PORSCL 43
#define CLK_J7200_PLLFRACF_SSMOD_16FFT_MCU_1_FOUTVCOP_CLK 44
#define CLK_J7200_PLLFRACF_SSMOD_16FFT_MCU_2_FOUTVCOP_CLK 45
#define CLK_J7200_SPI_MAIN_3_IO_CLKSPIO_CLK 46
#define CLK_J7200_SPI_MCU_0_IO_CLKSPIO_CLK 47
#define CLK_J7200_SPI_MCU_1_IO_CLKSPIO_CLK 48
#define CLK_J7200_SPI_MCU_2_IO_CLKSPIO_CLK 49
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN0_TXMCLK 50
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN1_TXMCLK 51
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN2_TXMCLK 52
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN3_TXMCLK 53
#define CLK_J7200_WKUP_GPIO0_CLKSEL_OUT0 54
#define CLK_J7200_MAIN_PLL_HFOSC_SEL_OUT0 55
#define CLK_J7200_MAIN_PLL_HFOSC_SEL_OUT1 56
#define CLK_J7200_MAIN_PLL_HFOSC_SEL_OUT3 57
#define CLK_J7200_MCU_SPI1_CLK_MUX_OUT0 58
#define CLK_J7200_HSDIV4_16FFT_MCU_1_HSDIVOUT0_CLK 59
#define CLK_J7200_HSDIV4_16FFT_MCU_1_HSDIVOUT1_CLK 60
#define CLK_J7200_HSDIV4_16FFT_MCU_1_HSDIVOUT2_CLK 61
#define CLK_J7200_HSDIV4_16FFT_MCU_1_HSDIVOUT3_CLK 62
#define CLK_J7200_HSDIV4_16FFT_MCU_1_HSDIVOUT4_CLK 63
#define CLK_J7200_HSDIV4_16FFT_MCU_2_HSDIVOUT0_CLK 64
#define CLK_J7200_HSDIV4_16FFT_MCU_2_HSDIVOUT1_CLK 65
#define CLK_J7200_HSDIV4_16FFT_MCU_2_HSDIVOUT2_CLK 66
#define CLK_J7200_HSDIV4_16FFT_MCU_2_HSDIVOUT3_CLK 67
#define CLK_J7200_HSDIV4_16FFT_MCU_2_HSDIVOUT4_CLK 68
#define CLK_J7200_MCU_OSPI_REF_CLK_SEL_OUT0 69
#define CLK_J7200_MCU_MCAN_CLK_SEL_OUT0 70
#define CLK_J7200_MCU_MCAN_CLK_SEL_OUT1 71
#define CLK_J7200_PLLFRACF_SSMOD_16FFT_MAIN_0_FOUTVCOP_CLK 72
#define CLK_J7200_PLLFRACF_SSMOD_16FFT_MAIN_0_FOUTPOSTDIV_CLK 73
#define CLK_J7200_PLLFRACF_SSMOD_16FFT_MAIN_1_FOUTVCOP_CLK 74
#define CLK_J7200_PLLFRACF_SSMOD_16FFT_MAIN_1_FOUTPOSTDIV_CLK 75
#define CLK_J7200_PLLFRACF_SSMOD_16FFT_MAIN_3_FOUTVCOP_CLK 76
#define CLK_J7200_POSTDIV2_16FFT_MAIN_0_HSDIVOUT6_CLK 77
#define CLK_J7200_POSTDIV2_16FFT_MAIN_1_HSDIVOUT5_CLK 78
#define CLK_J7200_WKUP_I2C0_MCUPLL_BYPASS_CLKSEL_OUT0 79
#define CLK_J7200_WKUPUSART_CLK_SEL_OUT0 80
#define CLK_J7200_MCU_ADC_CLK_SEL_OUT0 81
#define CLK_J7200_MCU_ADC_CLK_SEL_OUT1 82
#define CLK_J7200_MCU_TIMER_CLKSEL_OUT0 83
#define CLK_J7200_MCU_TIMER_CLKSEL_OUT1 84
#define CLK_J7200_MCU_TIMER_CLKSEL_OUT2 85
#define CLK_J7200_MCU_TIMER_CLKSEL_OUT3 86
#define CLK_J7200_MCU_TIMER_CLKSEL_OUT4 87
#define CLK_J7200_MCU_TIMER_CLKSEL_OUT5 88
#define CLK_J7200_MCU_TIMER_CLKSEL_OUT6 89
#define CLK_J7200_MCU_TIMER_CLKSEL_OUT7 90
#define CLK_J7200_MCU_TIMER_CLKSEL_OUT8 91
#define CLK_J7200_MCU_TIMER_CLKSEL_OUT9 92
#define CLK_J7200_HSDIV4_16FFT_MAIN_3_HSDIVOUT1_CLK 93
#define CLK_J7200_MCUUSART_CLK_SEL_OUT0 94
#define CLK_J7200_WKUP_USART_MCUPLL_BYPASS_CLKSEL_OUT0 95
#define CLK_J7200_CPSW2G_CPTS_RCLK_MUX_OUT0 96
#define CLK_J7200_MCU_TIMER1_CASCADE_OUT0 97
#define CLK_J7200_MCU_TIMER3_CASCADE_OUT0 98
#define CLK_J7200_MCU_TIMER5_CASCADE_OUT0 99
#define CLK_J7200_MCU_TIMER7_CASCADE_OUT0 100
#define CLK_J7200_MCU_TIMER9_CASCADE_OUT0 101
#define CLK_J7200_MAIN_PLL_HFOSC_SEL_OUT12 102
#define CLK_J7200_MAIN_PLL_HFOSC_SEL_OUT14 103
#define CLK_J7200_MAIN_PLL_HFOSC_SEL_OUT2 104
#define CLK_J7200_MAIN_PLL_HFOSC_SEL_OUT4 105
#define CLK_J7200_MAIN_PLL_HFOSC_SEL_OUT7 106
#define CLK_J7200_MAIN_PLL_HFOSC_SEL_OUT8 107
#define CLK_J7200_MAIN_WWDT_CLKSEL_OUT0 108
#define CLK_J7200_MAIN_WWDT_CLKSEL_OUT1 109
#define CLK_J7200_MAIN_WWDT_CLKSEL_OUT6 110
#define CLK_J7200_MAIN_WWDT_CLKSEL_OUT7 111
#define CLK_J7200_MCU_CLKOUT_MUX_OUT0 112
#define CLK_J7200_SPI3_CLK_MUX_OUT0 113
#define CLK_J7200_USB0_REFCLK_SEL_OUT0 114
#define CLK_J7200_ATL_MAIN_0_ATL_IO_PORT_ATCLK_OUT 115
#define CLK_J7200_ATL_MAIN_0_ATL_IO_PORT_ATCLK_OUT_1 116
#define CLK_J7200_ATL_MAIN_0_ATL_IO_PORT_ATCLK_OUT_2 117
#define CLK_J7200_ATL_MAIN_0_ATL_IO_PORT_ATCLK_OUT_3 118
#define CLK_J7200_BOARD_0_AUDIO_EXT_REFCLK0_OUT 119
#define CLK_J7200_BOARD_0_AUDIO_EXT_REFCLK1_OUT 120
#define CLK_J7200_BOARD_0_GPMC0_CLK_OUT 121
#define CLK_J7200_BOARD_0_I2C0_SCL_OUT 122
#define CLK_J7200_BOARD_0_I2C1_SCL_OUT 123
#define CLK_J7200_BOARD_0_I2C2_SCL_OUT 124
#define CLK_J7200_BOARD_0_I2C3_SCL_OUT 125
#define CLK_J7200_BOARD_0_I2C4_SCL_OUT 126
#define CLK_J7200_BOARD_0_I2C5_SCL_OUT 127
#define CLK_J7200_BOARD_0_I2C6_SCL_OUT 128
#define CLK_J7200_BOARD_0_I3C0_SCL_OUT 129
#define CLK_J7200_BOARD_0_LED_CLK_OUT 130
#define CLK_J7200_BOARD_0_MCASP0_ACLKR_OUT 131
#define CLK_J7200_BOARD_0_MCASP0_ACLKX_OUT 132
#define CLK_J7200_BOARD_0_MCASP1_ACLKR_OUT 133
#define CLK_J7200_BOARD_0_MCASP1_ACLKX_OUT 134
#define CLK_J7200_BOARD_0_MCASP2_ACLKR_OUT 135
#define CLK_J7200_BOARD_0_MCASP2_ACLKX_OUT 136
#define CLK_J7200_BOARD_0_RGMII1_RXC_OUT 137
#define CLK_J7200_BOARD_0_RGMII2_RXC_OUT 138
#define CLK_J7200_BOARD_0_RGMII3_RXC_OUT 139
#define CLK_J7200_BOARD_0_RGMII4_RXC_OUT 140
#define CLK_J7200_BOARD_0_RMII_REF_CLK_OUT 141
#define CLK_J7200_BOARD_0_TCK_OUT 142
#define CLK_J7200_CPSW_5XUSS_MAIN_0_CPTS_GENF0 143
#define CLK_J7200_CPSW_5XUSS_MAIN_0_MDIO_MDCLK_O 144
#define CLK_J7200_CPSW_5XUSS_MAIN_0_PRE_RGMII1_TCLK 145
#define CLK_J7200_CPSW_5XUSS_MAIN_0_PRE_RGMII2_TCLK 146
#define CLK_J7200_CPSW_5XUSS_MAIN_0_PRE_RGMII3_TCLK 147
#define CLK_J7200_CPSW_5XUSS_MAIN_0_PRE_RGMII4_TCLK 148
#define CLK_J7200_CPSW_5XUSS_MAIN_0_SERDES1_TXCLK 149
#define CLK_J7200_CPSW_5XUSS_MAIN_0_SERDES2_TXCLK 150
#define CLK_J7200_CPSW_5XUSS_MAIN_0_SERDES3_TXCLK 151
#define CLK_J7200_CPSW_5XUSS_MAIN_0_SERDES4_TXCLK 152
#define CLK_J7200_DEBUGSS_K3_WRAP_CV0_MAIN_0_CSTPIU_TRACECLK 153
#define CLK_J7200_DMTIMER_DMC1MS_MAIN_0_TIMER_PWM 154
#define CLK_J7200_DMTIMER_DMC1MS_MAIN_10_TIMER_PWM 155
#define CLK_J7200_DMTIMER_DMC1MS_MAIN_12_TIMER_PWM 156
#define CLK_J7200_DMTIMER_DMC1MS_MAIN_14_TIMER_PWM 157
#define CLK_J7200_DMTIMER_DMC1MS_MAIN_16_TIMER_PWM 158
#define CLK_J7200_DMTIMER_DMC1MS_MAIN_18_TIMER_PWM 159
#define CLK_J7200_DMTIMER_DMC1MS_MAIN_2_TIMER_PWM 160
#define CLK_J7200_DMTIMER_DMC1MS_MAIN_4_TIMER_PWM 161
#define CLK_J7200_DMTIMER_DMC1MS_MAIN_6_TIMER_PWM 162
#define CLK_J7200_DMTIMER_DMC1MS_MAIN_8_TIMER_PWM 163
#define CLK_J7200_EMMCSD4SS_MAIN_0_EMMCSDSS_IO_CLK_O 164
#define CLK_J7200_GPMC_MAIN_0_PO_GPMC_DEV_CLK 165
#define CLK_J7200_HSDIV4_16FFT_MAIN_0_HSDIVOUT0_CLK 166
#define CLK_J7200_HSDIV4_16FFT_MAIN_0_HSDIVOUT1_CLK 167
#define CLK_J7200_HSDIV4_16FFT_MAIN_0_HSDIVOUT2_CLK 168
#define CLK_J7200_HSDIV4_16FFT_MAIN_0_HSDIVOUT3_CLK 169
#define CLK_J7200_HSDIV4_16FFT_MAIN_0_HSDIVOUT4_CLK 170
#define CLK_J7200_HSDIV4_16FFT_MAIN_1_HSDIVOUT0_CLK 171
#define CLK_J7200_HSDIV4_16FFT_MAIN_1_HSDIVOUT1_CLK 172
#define CLK_J7200_HSDIV4_16FFT_MAIN_1_HSDIVOUT2_CLK 173
#define CLK_J7200_HSDIV4_16FFT_MAIN_1_HSDIVOUT3_CLK 174
#define CLK_J7200_HSDIV4_16FFT_MAIN_3_HSDIVOUT0_CLK 175
#define CLK_J7200_HSDIV4_16FFT_MAIN_3_HSDIVOUT2_CLK 176
#define CLK_J7200_HSDIV4_16FFT_MAIN_3_HSDIVOUT3_CLK 177
#define CLK_J7200_HSDIV4_16FFT_MAIN_3_HSDIVOUT4_CLK 178
#define CLK_J7200_I3C_MAIN_0_I3C_SCL_DO 179
#define CLK_J7200_J7VCL_CPSW_TX_RGMII_WRAP_MAIN_0_IO__RGMII1_TXC__A 180
#define CLK_J7200_J7VCL_CPSW_TX_RGMII_WRAP_MAIN_0_IO__RGMII2_TXC__A 181
#define CLK_J7200_J7VCL_CPSW_TX_RGMII_WRAP_MAIN_0_IO__RGMII3_TXC__A 182
#define CLK_J7200_J7VCL_CPSW_TX_RGMII_WRAP_MAIN_0_IO__RGMII4_TXC__A 183
#define CLK_J7200_K3_PLL_CTRL_WRAP_MAIN_0_SYSCLKOUT_CLK 184
#define CLK_J7200_K3_PLL_CTRL_WRAP_MAIN_0_CHIP_DIV1_CLK_CLK 185
#define CLK_J7200_K3_PLL_CTRL_WRAP_WKUP_0_CHIP_DIV24_CLK_CLK 186
#define CLK_J7200_MCASP_MAIN_0_MCASP_ACLKR_POUT 187
#define CLK_J7200_MCASP_MAIN_0_MCASP_ACLKX_POUT 188
#define CLK_J7200_MCASP_MAIN_0_MCASP_AHCLKR_POUT 189
#define CLK_J7200_MCASP_MAIN_0_MCASP_AHCLKX_POUT 190
#define CLK_J7200_MCASP_MAIN_1_MCASP_ACLKR_POUT 191
#define CLK_J7200_MCASP_MAIN_1_MCASP_ACLKX_POUT 192
#define CLK_J7200_MCASP_MAIN_1_MCASP_AHCLKR_POUT 193
#define CLK_J7200_MCASP_MAIN_1_MCASP_AHCLKX_POUT 194
#define CLK_J7200_MCASP_MAIN_2_MCASP_ACLKR_POUT 195
#define CLK_J7200_MCASP_MAIN_2_MCASP_ACLKX_POUT 196
#define CLK_J7200_MCASP_MAIN_2_MCASP_AHCLKR_POUT 197
#define CLK_J7200_MCASP_MAIN_2_MCASP_AHCLKX_POUT 198
#define CLK_J7200_MSHSI2C_MAIN_0_PORSCL 199
#define CLK_J7200_MSHSI2C_MAIN_1_PORSCL 200
#define CLK_J7200_MSHSI2C_MAIN_2_PORSCL 201
#define CLK_J7200_MSHSI2C_MAIN_3_PORSCL 202
#define CLK_J7200_MSHSI2C_MAIN_4_PORSCL 203
#define CLK_J7200_MSHSI2C_MAIN_5_PORSCL 204
#define CLK_J7200_MSHSI2C_MAIN_6_PORSCL 205
#define CLK_J7200_NAVSS256VCL_MAIN_0_CPTS0_GENF2 206
#define CLK_J7200_NAVSS256VCL_MAIN_0_CPTS0_GENF3 207
#define CLK_J7200_NAVSS256VCL_MAIN_0_CPTS0_GENF4 208
#define CLK_J7200_NAVSS256VCL_MAIN_0_CPTS_0_TS_GENF0 209
#define CLK_J7200_NAVSS256VCL_MAIN_0_CPTS_0_TS_GENF1 210
#define CLK_J7200_PCIE_G3X4_128_MAIN_1_PCIE_LANE0_TXCLK 211
#define CLK_J7200_PCIE_G3X4_128_MAIN_1_PCIE_LANE1_TXCLK 212
#define CLK_J7200_PCIE_G3X4_128_MAIN_1_PCIE_LANE2_TXCLK 213
#define CLK_J7200_PCIE_G3X4_128_MAIN_1_PCIE_LANE3_TXCLK 214
#define CLK_J7200_PLLFRACF_SSMOD_16FFT_MAIN_12_FOUTVCOP_CLK 215
#define CLK_J7200_PLLFRACF_SSMOD_16FFT_MAIN_14_FOUTVCOP_CLK 216
#define CLK_J7200_PLLFRACF_SSMOD_16FFT_MAIN_2_FOUTVCOP_CLK 217
#define CLK_J7200_PLLFRACF_SSMOD_16FFT_MAIN_2_FOUTPOSTDIV_CLK 218
#define CLK_J7200_PLLFRACF_SSMOD_16FFT_MAIN_7_FOUTVCOP_CLK 219
#define CLK_J7200_PLLFRACF_SSMOD_16FFT_MAIN_8_FOUTVCOP_CLK 220
#define CLK_J7200_POSTDIV2_16FFT_MAIN_0_HSDIVOUT5_CLK 221
#define CLK_J7200_POSTDIV2_16FFT_MAIN_0_HSDIVOUT7_CLK 222
#define CLK_J7200_POSTDIV2_16FFT_MAIN_1_HSDIVOUT7_CLK 223
#define CLK_J7200_POSTDIV2_16FFT_MAIN_2_HSDIVOUT6_CLK 224
#define CLK_J7200_SPI_MAIN_0_IO_CLKSPIO_CLK 225
#define CLK_J7200_SPI_MAIN_1_IO_CLKSPIO_CLK 226
#define CLK_J7200_SPI_MAIN_2_IO_CLKSPIO_CLK 227
#define CLK_J7200_SPI_MAIN_4_IO_CLKSPIO_CLK 228
#define CLK_J7200_SPI_MAIN_5_IO_CLKSPIO_CLK 229
#define CLK_J7200_SPI_MAIN_6_IO_CLKSPIO_CLK 230
#define CLK_J7200_SPI_MAIN_7_IO_CLKSPIO_CLK 231
#define CLK_J7200_USB3P0SS_16FFC_MAIN_0_PIPE_TXCLK 232
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN0_REFCLK 233
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN0_RXCLK 234
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN0_RXFCLK 235
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN0_TXFCLK 236
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN0_TXMCLK 237
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN1_REFCLK 238
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN1_RXCLK 239
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN1_RXFCLK 240
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN1_TXFCLK 241
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN1_TXMCLK 242
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN2_REFCLK 243
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN2_RXCLK 244
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN2_RXFCLK 245
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN2_TXFCLK 246
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN2_TXMCLK 247
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN3_REFCLK 248
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN3_RXCLK 249
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN3_RXFCLK 250
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN3_TXFCLK 251
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP1_LN3_TXMCLK 252
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN0_REFCLK 253
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN0_RXCLK 254
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN0_RXFCLK 255
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN0_TXFCLK 256
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN1_REFCLK 257
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN1_RXCLK 258
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN1_RXFCLK 259
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN1_TXFCLK 260
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN2_REFCLK 261
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN2_RXCLK 262
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN2_RXFCLK 263
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN2_TXFCLK 264
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN3_REFCLK 265
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN3_RXCLK 266
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN3_RXFCLK 267
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP2_LN3_TXFCLK 268
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP3_LN1_REFCLK 269
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP3_LN1_RXCLK 270
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP3_LN1_RXFCLK 271
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP3_LN1_TXFCLK 272
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP3_LN1_TXMCLK 273
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP3_LN3_REFCLK 274
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP3_LN3_RXCLK 275
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP3_LN3_RXFCLK 276
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP3_LN3_TXFCLK 277
#define CLK_J7200_WIZ16B8M4CT2_MAIN_1_IP3_LN3_TXMCLK 278
#define CLK_J7200_WKUP_OBSCLK_MUX_OUT0 279
#define CLK_J7200_CLKOUT_MUX_OUT0 280
#define CLK_J7200_CPSW5X_CPTS_RFT_MUX_OUT0 281
#define CLK_J7200_EMMCSD_REFCLK_SEL_OUT0 282
#define CLK_J7200_EMMCSD_REFCLK_SEL_OUT1 283
#define CLK_J7200_GTC_CLK_MUX_OUT0 284
#define CLK_J7200_MAIN_PLL4_XREF_SEL_OUT0 285
#define CLK_J7200_MCAN_CLK_MUX_OUT0 286
#define CLK_J7200_MCAN_CLK_MUX_OUT1 287
#define CLK_J7200_MCAN_CLK_MUX_OUT10 288
#define CLK_J7200_MCAN_CLK_MUX_OUT11 289
#define CLK_J7200_MCAN_CLK_MUX_OUT12 290
#define CLK_J7200_MCAN_CLK_MUX_OUT13 291
#define CLK_J7200_MCAN_CLK_MUX_OUT14_0 292
#define CLK_J7200_MCAN_CLK_MUX_OUT15_0 293
#define CLK_J7200_MCAN_CLK_MUX_OUT16_0 294
#define CLK_J7200_MCAN_CLK_MUX_OUT17_0 295
#define CLK_J7200_MCAN_CLK_MUX_OUT2 296
#define CLK_J7200_MCAN_CLK_MUX_OUT3 297
#define CLK_J7200_MCAN_CLK_MUX_OUT4 298
#define CLK_J7200_MCAN_CLK_MUX_OUT5 299
#define CLK_J7200_MCAN_CLK_MUX_OUT6 300
#define CLK_J7200_MCAN_CLK_MUX_OUT7 301
#define CLK_J7200_MCAN_CLK_MUX_OUT8 302
#define CLK_J7200_MCAN_CLK_MUX_OUT9 303
#define CLK_J7200_MCASP_AHCLKR_MUX_OUT0 304
#define CLK_J7200_MCASP_AHCLKR_MUX_OUT1 305
#define CLK_J7200_MCASP_AHCLKR_MUX_OUT2 306
#define CLK_J7200_MCASP_AHCLKX_MUX_OUT0 307
#define CLK_J7200_MCASP_AHCLKX_MUX_OUT1 308
#define CLK_J7200_MCASP_AHCLKX_MUX_OUT2 309
#define CLK_J7200_NAVSS_CPTS_RCLK_MUX_OUT0 310
#define CLK_J7200_PCIE1_CPTS_RCLK_MUX_OUT0 311
#define CLK_J7200_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT0 312
#define CLK_J7200_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT1 313
#define CLK_J7200_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT2 314
#define CLK_J7200_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT3 315
#define CLK_J7200_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT4 316
#define CLK_J7200_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT5 317
#define CLK_J7200_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT6 318
#define CLK_J7200_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT7 319
#define CLK_J7200_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT8 320
#define CLK_J7200_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT9 321
#define CLK_J7200_USB0_SERDES_REFCLK_MUX_OUT0 322
#define CLK_J7200_USB0_SERDES_RXCLK_MUX_OUT0 323
#define CLK_J7200_USB0_SERDES_RXFCLK_MUX_OUT0 324
#define CLK_J7200_USB0_SERDES_TXFCLK_MUX_OUT0 325
#define CLK_J7200_USB0_SERDES_TXMCLK_MUX_OUT0 326
#define CLK_J7200_HSDIV0_16FFT_MAIN_12_HSDIVOUT0_CLK 327
#define CLK_J7200_HSDIV0_16FFT_MAIN_7_HSDIVOUT0_CLK 328
#define CLK_J7200_HSDIV0_16FFT_MAIN_8_HSDIVOUT0_CLK 329
#define CLK_J7200_HSDIV1_16FFT_MAIN_14_HSDIVOUT0_CLK 330
#define CLK_J7200_HSDIV4_16FFT_MAIN_2_HSDIVOUT0_CLK 331
#define CLK_J7200_HSDIV4_16FFT_MAIN_2_HSDIVOUT1_CLK 332
#define CLK_J7200_HSDIV4_16FFT_MAIN_2_HSDIVOUT2_CLK 333
#define CLK_J7200_HSDIV4_16FFT_MAIN_2_HSDIVOUT3_CLK 334
#define CLK_J7200_HSDIV4_16FFT_MAIN_2_HSDIVOUT4_CLK 335
#define CLK_J7200_MCU_OBSCLK_DIV_OUT0 336
#define CLK_J7200_MCU_OBSCLK_OUTMUX_OUT0 337
#define CLK_J7200_PLLFRACF_SSMOD_16FFT_MAIN_4_FOUTVCOP_CLK 338
#define CLK_J7200_OBSCLK1_MUX_OUT0 339
#define CLK_J7200_SERDES0_CORE_REFCLK_OUT0 340
#define CLK_J7200_GPMC_FCLK_SEL_OUT0 341
#define CLK_J7200_HSDIV2_16FFT_MAIN_4_HSDIVOUT0_CLK 342
#define CLK_J7200_HSDIV2_16FFT_MAIN_4_HSDIVOUT1_CLK 343
#define CLK_J7200_HSDIV2_16FFT_MAIN_4_HSDIVOUT2_CLK 344
#define CLK_J7200_ATL_PCLKMUX_OUT0 345
#define CLK_J7200_AUDIO_REFCLKO_MUX_OUT0 346
#define CLK_J7200_AUDIO_REFCLKO_MUX_OUT1 347
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT0 348
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT1 349
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT10 350
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT11 351
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT12 352
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT13 353
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT14 354
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT15 355
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT16 356
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT17 357
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT18 358
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT19 359
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT2 360
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT3 361
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT4 362
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT5 363
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT6 364
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT7 365
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT8 366
#define CLK_J7200_MAIN_TIMER_CLKSEL_OUT9 367
#define CLK_J7200_MCASP_AUXCLK_SEL_OUT0 368
#define CLK_J7200_MCASP_AUXCLK_SEL_OUT1 369
#define CLK_J7200_MCASP_AUXCLK_SEL_OUT2 370
#define CLK_J7200_OBSCLK0_MUX_OUT0 371
#define CLK_J7200_TIMER11_CASCADE_OUT0 372
#define CLK_J7200_TIMER13_CASCADE_OUT0 373
#define CLK_J7200_TIMER15_CASCADE_OUT0 374
#define CLK_J7200_TIMER17_CASCADE_OUT0 375
#define CLK_J7200_TIMER19_CASCADE_OUT0 376
#define CLK_J7200_TIMER1_CASCADE_OUT0 377
#define CLK_J7200_TIMER3_CASCADE_OUT0 378
#define CLK_J7200_TIMER5_CASCADE_OUT0 379
#define CLK_J7200_TIMER7_CASCADE_OUT0 380
#define CLK_J7200_TIMER9_CASCADE_OUT0 381
#define CLK_J7200_OSBCLK0_DIV_OUT0 382
#define CLK_J7200_K3_PLL_CTRL_WRAP_MAIN_0_CHIP_DIV24_CLK_CLK 383

#endif /* SOC_J7200_CLK_IDS_H */