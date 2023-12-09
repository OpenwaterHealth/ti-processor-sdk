/*
 *  Copyright (C) 2017-2021 Texas Instruments Incorporated
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
/**
 * \ingroup TISCI
 * \defgroup tisci_devices tisci_devices
 *
 * DMSC controls the power management, security and resource management
 * of the device.
 *
 *
 * @{
 */
/**
 *
 *  \brief  This file contains:
 *
 *          WARNING!!: Autogenerated file from SYSFW. DO NOT MODIFY!!
 * Data version: 210421_084401
 *
 */
#ifndef SOC_TISCI_DEVICES_H
#define SOC_TISCI_DEVICES_H

#define TISCI_DEV_MCU_ADC12_16FFC0 0
#define TISCI_DEV_MCU_ADC12_16FFC1 1
#define TISCI_DEV_ATL0 2
#define TISCI_DEV_COMPUTE_CLUSTER0 3
#define TISCI_DEV_A72SS0 4
#define TISCI_DEV_COMPUTE_CLUSTER0_CFG_WRAP 5
#define TISCI_DEV_COMPUTE_CLUSTER0_CLEC 6
#define TISCI_DEV_COMPUTE_CLUSTER0_CORE_CORE 7
#define TISCI_DEV_COMPUTE_CLUSTER0_DDR32SS_EMIF0_EW 8
#define TISCI_DEV_COMPUTE_CLUSTER0_DEBUG_WRAP 9
#define TISCI_DEV_COMPUTE_CLUSTER0_DIVH2_DIVH0 10
#define TISCI_DEV_COMPUTE_CLUSTER0_DIVP_TFT0 11
#define TISCI_DEV_COMPUTE_CLUSTER0_DMSC_WRAP 12
#define TISCI_DEV_COMPUTE_CLUSTER0_EN_MSMC_DOMAIN 13
#define TISCI_DEV_COMPUTE_CLUSTER0_GIC500SS 14
#define TISCI_DEV_C71SS0 15
#define TISCI_DEV_C71SS0_MMA 16
#define TISCI_DEV_COMPUTE_CLUSTER0_PBIST_WRAP 17
#define TISCI_DEV_MCU_CPSW0 18
#define TISCI_DEV_CPSW0 19
#define TISCI_DEV_CPT2_AGGR0 20
#define TISCI_DEV_CPT2_AGGR1 21
#define TISCI_DEV_WKUP_DMSC0 22
#define TISCI_DEV_CPT2_AGGR2 23
#define TISCI_DEV_MCU_CPT2_AGGR0 24
#define TISCI_DEV_CSI_PSILSS0 25
#define TISCI_DEV_CSI_RX_IF0 26
#define TISCI_DEV_CSI_RX_IF1 27
#define TISCI_DEV_CSI_TX_IF0 28
#define TISCI_DEV_STM0 29
#define TISCI_DEV_DCC0 30
#define TISCI_DEV_DCC1 31
#define TISCI_DEV_DCC2 32
#define TISCI_DEV_DCC3 33
#define TISCI_DEV_DCC4 34
#define TISCI_DEV_MCU_TIMER0 35
#define TISCI_DEV_DCC5 36
#define TISCI_DEV_DCC6 37
#define TISCI_DEV_DCC7 38
#define TISCI_DEV_DCC8 39
#define TISCI_DEV_DCC9 40
#define TISCI_DEV_DCC10 41
#define TISCI_DEV_DCC11 42
#define TISCI_DEV_DCC12 43
#define TISCI_DEV_MCU_DCC0 44
#define TISCI_DEV_MCU_DCC1 45
#define TISCI_DEV_MCU_DCC2 46
#define TISCI_DEV_DDR0 47
#define TISCI_DEV_DMPAC0 48
#define TISCI_DEV_TIMER0 49
#define TISCI_DEV_TIMER1 50
#define TISCI_DEV_TIMER2 51
#define TISCI_DEV_TIMER3 52
#define TISCI_DEV_TIMER4 53
#define TISCI_DEV_TIMER5 54
#define TISCI_DEV_TIMER6 55
#define TISCI_DEV_TIMER7 57
#define TISCI_DEV_TIMER8 58
#define TISCI_DEV_TIMER9 59
#define TISCI_DEV_TIMER10 60
#define TISCI_DEV_GTC0 61
#define TISCI_DEV_TIMER11 62
#define TISCI_DEV_TIMER12 63
#define TISCI_DEV_TIMER13 64
#define TISCI_DEV_TIMER14 65
#define TISCI_DEV_TIMER15 66
#define TISCI_DEV_TIMER16 67
#define TISCI_DEV_TIMER17 68
#define TISCI_DEV_TIMER18 69
#define TISCI_DEV_TIMER19 70
#define TISCI_DEV_MCU_TIMER1 71
#define TISCI_DEV_MCU_TIMER2 72
#define TISCI_DEV_MCU_TIMER3 73
#define TISCI_DEV_MCU_TIMER4 74
#define TISCI_DEV_MCU_TIMER5 75
#define TISCI_DEV_MCU_TIMER6 76
#define TISCI_DEV_MCU_TIMER7 77
#define TISCI_DEV_MCU_TIMER8 78
#define TISCI_DEV_MCU_TIMER9 79
#define TISCI_DEV_ECAP0 80
#define TISCI_DEV_ECAP1 81
#define TISCI_DEV_ECAP2 82
#define TISCI_DEV_EHRPWM0 83
#define TISCI_DEV_EHRPWM1 84
#define TISCI_DEV_EHRPWM2 85
#define TISCI_DEV_EHRPWM3 86
#define TISCI_DEV_EHRPWM4 87
#define TISCI_DEV_EHRPWM5 88
#define TISCI_DEV_ELM0 89
#define TISCI_DEV_EMIF_DATA_0_VD 90
#define TISCI_DEV_MMCSD0 91
#define TISCI_DEV_MMCSD1 92
#define TISCI_DEV_MMCSD2 93
#define TISCI_DEV_EQEP0 94
#define TISCI_DEV_EQEP1 95
#define TISCI_DEV_EQEP2 96
#define TISCI_DEV_ESM0 97
#define TISCI_DEV_MCU_ESM0 98
#define TISCI_DEV_WKUP_ESM0 99
#define TISCI_DEV_MCU_FSS0 100
#define TISCI_DEV_MCU_FSS0_FSAS_0 101
#define TISCI_DEV_MCU_FSS0_HYPERBUS1P0_0 102
#define TISCI_DEV_MCU_FSS0_OSPI_0 103
#define TISCI_DEV_MCU_FSS0_OSPI_1 104
#define TISCI_DEV_GPIO0 105
#define TISCI_DEV_GPIO1 106
#define TISCI_DEV_GPIO2 107
#define TISCI_DEV_GPIO3 108
#define TISCI_DEV_GPIO4 109
#define TISCI_DEV_GPIO5 110
#define TISCI_DEV_GPIO6 111
#define TISCI_DEV_GPIO7 112
#define TISCI_DEV_WKUP_GPIO0 113
#define TISCI_DEV_WKUP_GPIO1 114
#define TISCI_DEV_GPMC0 115
#define TISCI_DEV_I3C0 116
#define TISCI_DEV_MCU_I3C0 117
#define TISCI_DEV_MCU_I3C1 118
#define TISCI_DEV_PRU_ICSSG0 119
#define TISCI_DEV_PRU_ICSSG1 120
#define TISCI_DEV_C66SS0_INTROUTER0 121
#define TISCI_DEV_C66SS1_INTROUTER0 122
#define TISCI_DEV_CMPEVENT_INTRTR0 123
#define TISCI_DEV_GPU0 124
#define TISCI_DEV_GPU0_GPU_0 125
#define TISCI_DEV_GPU0_GPUCORE_0 126
#define TISCI_DEV_LED0 127
#define TISCI_DEV_MAIN2MCU_LVL_INTRTR0 128
#define TISCI_DEV_MAIN2MCU_PLS_INTRTR0 130
#define TISCI_DEV_GPIOMUX_INTRTR0 131
#define TISCI_DEV_WKUP_PORZ_SYNC0 132
#define TISCI_DEV_PSC0 133
#define TISCI_DEV_R5FSS0_INTROUTER0 134
#define TISCI_DEV_R5FSS1_INTROUTER0 135
#define TISCI_DEV_TIMESYNC_INTRTR0 136
#define TISCI_DEV_WKUP_GPIOMUX_INTRTR0 137
#define TISCI_DEV_WKUP_PSC0 138
#define TISCI_DEV_AASRC0 139
#define TISCI_DEV_C66SS0 140
#define TISCI_DEV_C66SS1 141
#define TISCI_DEV_C66SS0_CORE0 142
#define TISCI_DEV_C66SS1_CORE0 143
#define TISCI_DEV_DECODER0 144
#define TISCI_DEV_WKUP_DDPA0 145
#define TISCI_DEV_UART0 146
#define TISCI_DEV_DPHY_RX0 147
#define TISCI_DEV_DPHY_RX1 148
#define TISCI_DEV_MCU_UART0 149
#define TISCI_DEV_DSS_DSI0 150
#define TISCI_DEV_DSS_EDP0 151
#define TISCI_DEV_DSS0 152
#define TISCI_DEV_ENCODER0 153
#define TISCI_DEV_WKUP_VTM0 154
#define TISCI_DEV_MAIN2WKUPMCU_VD 155
#define TISCI_DEV_MCAN0 156
#define TISCI_DEV_BOARD0 157
#define TISCI_DEV_MCAN1 158
#define TISCI_DEV_MCAN2 160
#define TISCI_DEV_MCAN3 161
#define TISCI_DEV_MCAN4 162
#define TISCI_DEV_MCAN5 163
#define TISCI_DEV_MCAN6 164
#define TISCI_DEV_MCAN7 165
#define TISCI_DEV_MCAN8 166
#define TISCI_DEV_MCAN9 167
#define TISCI_DEV_MCAN10 168
#define TISCI_DEV_MCAN11 169
#define TISCI_DEV_MCAN12 170
#define TISCI_DEV_MCAN13 171
#define TISCI_DEV_MCU_MCAN0 172
#define TISCI_DEV_MCU_MCAN1 173
#define TISCI_DEV_MCASP0 174
#define TISCI_DEV_MCASP1 175
#define TISCI_DEV_MCASP2 176
#define TISCI_DEV_MCASP3 177
#define TISCI_DEV_MCASP4 178
#define TISCI_DEV_MCASP5 179
#define TISCI_DEV_MCASP6 180
#define TISCI_DEV_MCASP7 181
#define TISCI_DEV_MCASP8 182
#define TISCI_DEV_MCASP9 183
#define TISCI_DEV_MCASP10 184
#define TISCI_DEV_MCASP11 185
#define TISCI_DEV_MLB0 186
#define TISCI_DEV_I2C0 187
#define TISCI_DEV_I2C1 188
#define TISCI_DEV_I2C2 189
#define TISCI_DEV_I2C3 190
#define TISCI_DEV_I2C4 191
#define TISCI_DEV_I2C5 192
#define TISCI_DEV_I2C6 193
#define TISCI_DEV_MCU_I2C0 194
#define TISCI_DEV_MCU_I2C1 195
#define TISCI_DEV_WKUP_I2C0 197
#define TISCI_DEV_NAVSS0 199
#define TISCI_DEV_NAVSS0_CPTS_0 201
#define TISCI_DEV_A72SS0_CORE0 202
#define TISCI_DEV_A72SS0_CORE1 203
#define TISCI_DEV_NAVSS0_DTI_0 206
#define TISCI_DEV_NAVSS0_MODSS_INTAGGR_0 207
#define TISCI_DEV_NAVSS0_MODSS_INTAGGR_1 208
#define TISCI_DEV_NAVSS0_UDMASS_INTAGGR_0 209
#define TISCI_DEV_NAVSS0_PROXY_0 210
#define TISCI_DEV_NAVSS0_RINGACC_0 211
#define TISCI_DEV_NAVSS0_UDMAP_0 212
#define TISCI_DEV_NAVSS0_INTR_ROUTER_0 213
#define TISCI_DEV_NAVSS0_MAILBOX_0 214
#define TISCI_DEV_NAVSS0_MAILBOX_1 215
#define TISCI_DEV_NAVSS0_MAILBOX_2 216
#define TISCI_DEV_NAVSS0_MAILBOX_3 217
#define TISCI_DEV_NAVSS0_MAILBOX_4 218
#define TISCI_DEV_NAVSS0_MAILBOX_5 219
#define TISCI_DEV_NAVSS0_MAILBOX_6 220
#define TISCI_DEV_NAVSS0_MAILBOX_7 221
#define TISCI_DEV_NAVSS0_MAILBOX_8 222
#define TISCI_DEV_NAVSS0_MAILBOX_9 223
#define TISCI_DEV_NAVSS0_MAILBOX_10 224
#define TISCI_DEV_NAVSS0_MAILBOX_11 225
#define TISCI_DEV_NAVSS0_SPINLOCK_0 226
#define TISCI_DEV_NAVSS0_MCRC_0 227
#define TISCI_DEV_NAVSS0_TBU_0 228
#define TISCI_DEV_NAVSS0_TCU_0 229
#define TISCI_DEV_NAVSS0_TIMERMGR_0 230
#define TISCI_DEV_NAVSS0_TIMERMGR_1 231
#define TISCI_DEV_MCU_NAVSS0 232
#define TISCI_DEV_MCU_NAVSS0_UDMASS_INTA_0 233
#define TISCI_DEV_MCU_NAVSS0_PROXY0 234
#define TISCI_DEV_MCU_NAVSS0_RINGACC0 235
#define TISCI_DEV_MCU_NAVSS0_UDMAP_0 236
#define TISCI_DEV_MCU_NAVSS0_INTR_0 237
#define TISCI_DEV_MCU_NAVSS0_MCRC_0 238
#define TISCI_DEV_PCIE0 239
#define TISCI_DEV_PCIE1 240
#define TISCI_DEV_PCIE2 241
#define TISCI_DEV_PCIE3 242
#define TISCI_DEV_R5FSS0 243
#define TISCI_DEV_R5FSS1 244
#define TISCI_DEV_R5FSS0_CORE0 245
#define TISCI_DEV_R5FSS0_CORE1 246
#define TISCI_DEV_R5FSS1_CORE0 247
#define TISCI_DEV_R5FSS1_CORE1 248
#define TISCI_DEV_MCU_R5FSS0 249
#define TISCI_DEV_MCU_R5FSS0_CORE0 250
#define TISCI_DEV_MCU_R5FSS0_CORE1 251
#define TISCI_DEV_RTI0 252
#define TISCI_DEV_RTI1 253
#define TISCI_DEV_RTI24 254
#define TISCI_DEV_RTI25 255
#define TISCI_DEV_RTI16 256
#define TISCI_DEV_RTI15 257
#define TISCI_DEV_RTI28 258
#define TISCI_DEV_RTI29 259
#define TISCI_DEV_RTI30 260
#define TISCI_DEV_RTI31 261
#define TISCI_DEV_MCU_RTI0 262
#define TISCI_DEV_MCU_RTI1 263
#define TISCI_DEV_SA2_UL0 264
#define TISCI_DEV_MCU_SA2_UL0 265
#define TISCI_DEV_MCSPI0 266
#define TISCI_DEV_MCSPI1 267
#define TISCI_DEV_MCSPI2 268
#define TISCI_DEV_MCSPI3 269
#define TISCI_DEV_MCSPI4 270
#define TISCI_DEV_MCSPI5 271
#define TISCI_DEV_MCSPI6 272
#define TISCI_DEV_MCSPI7 273
#define TISCI_DEV_MCU_MCSPI0 274
#define TISCI_DEV_MCU_MCSPI1 275
#define TISCI_DEV_MCU_MCSPI2 276
#define TISCI_DEV_UFS0 277
#define TISCI_DEV_UART1 278
#define TISCI_DEV_UART2 279
#define TISCI_DEV_UART3 280
#define TISCI_DEV_UART4 281
#define TISCI_DEV_UART5 282
#define TISCI_DEV_UART6 283
#define TISCI_DEV_UART7 284
#define TISCI_DEV_UART8 285
#define TISCI_DEV_UART9 286
#define TISCI_DEV_WKUP_UART0 287
#define TISCI_DEV_USB0 288
#define TISCI_DEV_USB1 289
#define TISCI_DEV_VPAC0 290
#define TISCI_DEV_VPFE0 291
#define TISCI_DEV_SERDES_16G0 292
#define TISCI_DEV_SERDES_16G1 293
#define TISCI_DEV_SERDES_16G2 294
#define TISCI_DEV_SERDES_16G3 295
#define TISCI_DEV_DPHY_TX0 296
#define TISCI_DEV_SERDES_10G0 297
#define TISCI_DEV_WKUPMCU2MAIN_VD 298
#define TISCI_DEV_NAVSS0_MODSS 299
#define TISCI_DEV_NAVSS0_UDMASS 300
#define TISCI_DEV_NAVSS0_VIRTSS 301
#define TISCI_DEV_MCU_NAVSS0_MODSS 302
#define TISCI_DEV_MCU_NAVSS0_UDMASS 303
#define TISCI_DEV_DEBUGSS_WRAP0 304
#define TISCI_DEV_DMPAC0_SDE_0 305
#define TISCI_DEV_GPU0_DFT_PBIST_0 306
#define TISCI_DEV_C66SS0_PBIST0 307
#define TISCI_DEV_C66SS1_PBIST0 308
#define TISCI_DEV_PBIST0 309
#define TISCI_DEV_PBIST1 310
#define TISCI_DEV_PBIST2 311
#define TISCI_DEV_PBIST3 312
#define TISCI_DEV_PBIST4 313
#define TISCI_DEV_PBIST5 314
#define TISCI_DEV_PBIST6 315
#define TISCI_DEV_PBIST7 316
#define TISCI_DEV_PBIST9 317
#define TISCI_DEV_PBIST10 318
#define TISCI_DEV_MCU_PBIST0 319
#define TISCI_DEV_MCU_PBIST1 320
#define TISCI_DEV_C71X_0_PBIST_VD 321
#define TISCI_DEV_MCU_TIMER1_CLKSEL_VD 322
#define TISCI_DEV_MCU_TIMER3_CLKSEL_VD 323
#define TISCI_DEV_MCU_TIMER5_CLKSEL_VD 324
#define TISCI_DEV_MCU_TIMER7_CLKSEL_VD 325
#define TISCI_DEV_MCU_TIMER9_CLKSEL_VD 326
#define TISCI_DEV_TIMER1_CLKSEL_VD 327
#define TISCI_DEV_TIMER3_CLKSEL_VD 328
#define TISCI_DEV_TIMER5_CLKSEL_VD 329
#define TISCI_DEV_TIMER7_CLKSEL_VD 330
#define TISCI_DEV_TIMER9_CLKSEL_VD 331
#define TISCI_DEV_TIMER11_CLKSEL_VD 332
#define TISCI_DEV_TIMER13_CLKSEL_VD 333
#define TISCI_DEV_TIMER15_CLKSEL_VD 334
#define TISCI_DEV_TIMER17_CLKSEL_VD 335
#define TISCI_DEV_TIMER19_CLKSEL_VD 336
#define TISCI_DEV_ASCPCIE_BUFFER0 337
#define TISCI_DEV_ASCPCIE_BUFFER1 338

#endif /* SOC_TISCI_DEVICES_H */

/* @} */