/********************************************************************
 * Copyright (C) 2017 Texas Instruments Incorporated.
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
 *  Name        : cslr_flexcfa.h
*/
#ifndef CSLR_FLEXCFA_H_
#define CSLR_FLEXCFA_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include <ti/csl/cslr.h>
#include <stdint.h>

/**************************************************************************
* Hardware Region  : MMR Registers
**************************************************************************/


/**************************************************************************
* Register Overlay Structure
**************************************************************************/

typedef struct {
    volatile uint32_t COEF;                      /* Coefficient e*2+1 and e*2 */
} CSL_flexcfaRegs_core_dir_phase_row_col;


typedef struct {
    CSL_flexcfaRegs_core_dir_phase_row_col COL[3];
} CSL_flexcfaRegs_core_dir_phase_row;


typedef struct {
    CSL_flexcfaRegs_core_dir_phase_row ROW[6];
} CSL_flexcfaRegs_core_dir_phase;


typedef struct {
    CSL_flexcfaRegs_core_dir_phase PHASE[4];
} CSL_flexcfaRegs_core_dir;


typedef struct {
    CSL_flexcfaRegs_core_dir DIR[3];
} CSL_flexcfaRegs_core;

typedef struct {
    volatile uint32_t THR0_1;               /*  Set0 Thr0_1 */
    volatile uint32_t THR2_3;               /*  Set0 Thr2_3 */
    volatile uint32_t THR4_5;               /*  Set0 Thr4_5 */
    volatile uint32_t THR6;                 /*  Set0 Thr6 */
} CSL_flexcfaRegs_Threshold;

typedef struct {
    volatile uint32_t GRAD_HZ;              /* Gradient Bitfield Hz */
    volatile uint32_t GRAD_VT;              /* Gradient Bitfield Vt */
    volatile uint32_t INTENSITY0;           /* Intensity BitField */
    volatile uint32_t INTENSITY1;           /* Intensity BitField */
} CSL_flexcfaRegs_GradIntensity;

typedef struct {
    volatile uint32_t LUT[320];                  /* LUT Table */
    volatile uint8_t  Resv_4100[2820];
    volatile uint32_t CFG_0;                     /* Config_Size */
    volatile uint32_t CFG_1;                     /* Config_bitwidth */
    CSL_flexcfaRegs_core CORE[4];
    volatile uint32_t GRAD_CFG;                  /* Gradient Configuration */
    CSL_flexcfaRegs_GradIntensity GRAD_INT_SET[2U];  /* Gradient and Intensity configuration */
    CSL_flexcfaRegs_Threshold THR_SET[2U];        /* Thresholds */
    volatile uint32_t INT_STATUS;                /* Int Status/Clear register */
    volatile uint8_t  Resv_8192[556];
    volatile uint32_t DEBUG_CTL;                 /* Debug Control */
    volatile uint32_t DEBUG_STATUS;              /* Debug Event Status */
    volatile uint32_t LINE_SEL;                  /* Line Select */
    volatile uint8_t  Resv_16384[8180];
    volatile uint32_t PIXEL_RAM[2048];           /* Pixel RAM */
} CSL_flexcfaRegs;


/**************************************************************************
* Register Macros
**************************************************************************/

#define CSL_FLEXCFA_LUT(LUT)                                                   (0x00000000U+((LUT)*0x4U))
#define CSL_FLEXCFA_CFG_0                                                      (0x00001004U)
#define CSL_FLEXCFA_CFG_1                                                      (0x00001008U)
#define CSL_FLEXCFA_CORE_DIR_PHASE_ROW_COL_COEF(CORE, DIR, PHASE, ROW, COL)    (0x0000100CU+((CORE)*0x360U)+((DIR)*0x120U)+((PHASE)*0x48U)+((ROW)*0xCU)+((COL)*0x4U))
#define CSL_FLEXCFA_GRAD_CFG                                                   (0x00001D8CU)
#define CSL_FLEXCFA_SET0_GRAD_HZ                                               (0x00001D90U)
#define CSL_FLEXCFA_SET0_GRAD_VT                                               (0x00001D94U)
#define CSL_FLEXCFA_SET0_INTENSITY0                                            (0x00001D98U)
#define CSL_FLEXCFA_SET0_INTENSITY1                                            (0x00001D9CU)
#define CSL_FLEXCFA_SET1_GRAD_HZ                                               (0x00001DA0U)
#define CSL_FLEXCFA_SET1_GRAD_VT                                               (0x00001DA4U)
#define CSL_FLEXCFA_SET1_INTENSITY0                                            (0x00001DA8U)
#define CSL_FLEXCFA_SET1_INTENSITY1                                            (0x00001DACU)
#define CSL_FLEXCFA_SET0_THR0_1                                                (0x00001DB0U)
#define CSL_FLEXCFA_SET0_THR2_3                                                (0x00001DB4U)
#define CSL_FLEXCFA_SET0_THR4_5                                                (0x00001DB8U)
#define CSL_FLEXCFA_SET0_THR6                                                  (0x00001DBCU)
#define CSL_FLEXCFA_SET1_THR0_1                                                (0x00001DC0U)
#define CSL_FLEXCFA_SET1_THR2_3                                                (0x00001DC4U)
#define CSL_FLEXCFA_SET1_THR4_5                                                (0x00001DC8U)
#define CSL_FLEXCFA_SET1_THR6                                                  (0x00001DCCU)
#define CSL_FLEXCFA_INT_STATUS                                                 (0x00001DD0U)
#define CSL_FLEXCFA_DEBUG_CTL                                                  (0x00002000U)
#define CSL_FLEXCFA_DEBUG_STATUS                                               (0x00002004U)
#define CSL_FLEXCFA_LINE_SEL                                                   (0x00002008U)
#define CSL_FLEXCFA_PIXEL_RAM(PIXEL_RAM)                                       (0x00004000U+((PIXEL_RAM)*0x4U))

/**************************************************************************
* Field Definition Macros
**************************************************************************/


/* COEF */

#define CSL_FLEXCFA_CORE_DIR_PHASE_ROW_COL_COEF_COEF_0_MASK                    (0x000001FFU)
#define CSL_FLEXCFA_CORE_DIR_PHASE_ROW_COL_COEF_COEF_0_SHIFT                   (0x00000000U)
#define CSL_FLEXCFA_CORE_DIR_PHASE_ROW_COL_COEF_COEF_0_MAX                     (0x000001FFU)

#define CSL_FLEXCFA_CORE_DIR_PHASE_ROW_COL_COEF_COEF_1_MASK                    (0x01FF0000U)
#define CSL_FLEXCFA_CORE_DIR_PHASE_ROW_COL_COEF_COEF_1_SHIFT                   (0x00000010U)
#define CSL_FLEXCFA_CORE_DIR_PHASE_ROW_COL_COEF_COEF_1_MAX                     (0x000001FFU)

/* LUT */

#define CSL_FLEXCFA_LUT_LUT_ENTRY_LO_MASK                                      (0x00000FFFU)
#define CSL_FLEXCFA_LUT_LUT_ENTRY_LO_SHIFT                                     (0x00000000U)
#define CSL_FLEXCFA_LUT_LUT_ENTRY_LO_MAX                                       (0x00000FFFU)

#define CSL_FLEXCFA_LUT_LUT_ENTRY_HI_MASK                                      (0x0FFF0000U)
#define CSL_FLEXCFA_LUT_LUT_ENTRY_HI_SHIFT                                     (0x00000010U)
#define CSL_FLEXCFA_LUT_LUT_ENTRY_HI_MAX                                       (0x00000FFFU)

/* CFG_0 */

#define CSL_FLEXCFA_CFG_0_HEIGHT_MASK                                          (0x1FFF0000U)
#define CSL_FLEXCFA_CFG_0_HEIGHT_SHIFT                                         (0x00000010U)
#define CSL_FLEXCFA_CFG_0_HEIGHT_MAX                                           (0x00001FFFU)

#define CSL_FLEXCFA_CFG_0_WIDTH_MASK                                           (0x00001FFFU)
#define CSL_FLEXCFA_CFG_0_WIDTH_SHIFT                                          (0x00000000U)
#define CSL_FLEXCFA_CFG_0_WIDTH_MAX                                            (0x00001FFFU)

/* CFG_1 */

#define CSL_FLEXCFA_CFG_1_BYPASS_CORE3_MASK                                    (0x00000800U)
#define CSL_FLEXCFA_CFG_1_BYPASS_CORE3_SHIFT                                   (0x0000000BU)
#define CSL_FLEXCFA_CFG_1_BYPASS_CORE3_MAX                                     (0x00000001U)

#define CSL_FLEXCFA_CFG_1_BYPASS_CORE2_MASK                                    (0x00000400U)
#define CSL_FLEXCFA_CFG_1_BYPASS_CORE2_SHIFT                                   (0x0000000AU)
#define CSL_FLEXCFA_CFG_1_BYPASS_CORE2_MAX                                     (0x00000001U)

#define CSL_FLEXCFA_CFG_1_BYPASS_CORE1_MASK                                    (0x00000200U)
#define CSL_FLEXCFA_CFG_1_BYPASS_CORE1_SHIFT                                   (0x00000009U)
#define CSL_FLEXCFA_CFG_1_BYPASS_CORE1_MAX                                     (0x00000001U)

#define CSL_FLEXCFA_CFG_1_BYPASS_CORE0_MASK                                    (0x00000100U)
#define CSL_FLEXCFA_CFG_1_BYPASS_CORE0_SHIFT                                   (0x00000008U)
#define CSL_FLEXCFA_CFG_1_BYPASS_CORE0_MAX                                     (0x00000001U)

#define CSL_FLEXCFA_CFG_1_LUT_ENABLE_MASK                                      (0x00000020U)
#define CSL_FLEXCFA_CFG_1_LUT_ENABLE_SHIFT                                     (0x00000005U)
#define CSL_FLEXCFA_CFG_1_LUT_ENABLE_MAX                                       (0x00000001U)

#define CSL_FLEXCFA_CFG_1_BITWIDTH_MASK                                        (0x0000001FU)
#define CSL_FLEXCFA_CFG_1_BITWIDTH_SHIFT                                       (0x00000000U)
#define CSL_FLEXCFA_CFG_1_BITWIDTH_MAX                                         (0x0000001FU)

/* GRAD_CFG */

#define CSL_FLEXCFA_GRAD_CFG_BLENDMODECORE3_MASK                               (0x06000000U)
#define CSL_FLEXCFA_GRAD_CFG_BLENDMODECORE3_SHIFT                              (0x00000019U)
#define CSL_FLEXCFA_GRAD_CFG_BLENDMODECORE3_MAX                                (0x00000003U)

#define CSL_FLEXCFA_GRAD_CFG_BITMASKSELCORE3_MASK                              (0x01000000U)
#define CSL_FLEXCFA_GRAD_CFG_BITMASKSELCORE3_SHIFT                             (0x00000018U)
#define CSL_FLEXCFA_GRAD_CFG_BITMASKSELCORE3_MAX                               (0x00000001U)

#define CSL_FLEXCFA_GRAD_CFG_BLENDMODECORE2_MASK                               (0x00060000U)
#define CSL_FLEXCFA_GRAD_CFG_BLENDMODECORE2_SHIFT                              (0x00000011U)
#define CSL_FLEXCFA_GRAD_CFG_BLENDMODECORE2_MAX                                (0x00000003U)

#define CSL_FLEXCFA_GRAD_CFG_BITMASKSELCORE2_MASK                              (0x00010000U)
#define CSL_FLEXCFA_GRAD_CFG_BITMASKSELCORE2_SHIFT                             (0x00000010U)
#define CSL_FLEXCFA_GRAD_CFG_BITMASKSELCORE2_MAX                               (0x00000001U)

#define CSL_FLEXCFA_GRAD_CFG_BLENDMODECORE1_MASK                               (0x00000600U)
#define CSL_FLEXCFA_GRAD_CFG_BLENDMODECORE1_SHIFT                              (0x00000009U)
#define CSL_FLEXCFA_GRAD_CFG_BLENDMODECORE1_MAX                                (0x00000003U)

#define CSL_FLEXCFA_GRAD_CFG_BITMASKSELCORE1_MASK                              (0x00000100U)
#define CSL_FLEXCFA_GRAD_CFG_BITMASKSELCORE1_SHIFT                             (0x00000008U)
#define CSL_FLEXCFA_GRAD_CFG_BITMASKSELCORE1_MAX                               (0x00000001U)

#define CSL_FLEXCFA_GRAD_CFG_BLENDMODECORE0_MASK                               (0x00000006U)
#define CSL_FLEXCFA_GRAD_CFG_BLENDMODECORE0_SHIFT                              (0x00000001U)
#define CSL_FLEXCFA_GRAD_CFG_BLENDMODECORE0_MAX                                (0x00000003U)

#define CSL_FLEXCFA_GRAD_CFG_BITMASKSELCORE0_MASK                              (0x00000001U)
#define CSL_FLEXCFA_GRAD_CFG_BITMASKSELCORE0_SHIFT                             (0x00000000U)
#define CSL_FLEXCFA_GRAD_CFG_BITMASKSELCORE0_MAX                               (0x00000001U)

/* SET0_GRAD_HZ */

#define CSL_FLEXCFA_SET0_GRAD_HZ_PHASE3_MASK                                   (0xFF000000U)
#define CSL_FLEXCFA_SET0_GRAD_HZ_PHASE3_SHIFT                                  (0x00000018U)
#define CSL_FLEXCFA_SET0_GRAD_HZ_PHASE3_MAX                                    (0x000000FFU)

#define CSL_FLEXCFA_SET0_GRAD_HZ_PHASE2_MASK                                   (0x00FF0000U)
#define CSL_FLEXCFA_SET0_GRAD_HZ_PHASE2_SHIFT                                  (0x00000010U)
#define CSL_FLEXCFA_SET0_GRAD_HZ_PHASE2_MAX                                    (0x000000FFU)

#define CSL_FLEXCFA_SET0_GRAD_HZ_PHASE1_MASK                                   (0x0000FF00U)
#define CSL_FLEXCFA_SET0_GRAD_HZ_PHASE1_SHIFT                                  (0x00000008U)
#define CSL_FLEXCFA_SET0_GRAD_HZ_PHASE1_MAX                                    (0x000000FFU)

#define CSL_FLEXCFA_SET0_GRAD_HZ_PHASE0_MASK                                   (0x000000FFU)
#define CSL_FLEXCFA_SET0_GRAD_HZ_PHASE0_SHIFT                                  (0x00000000U)
#define CSL_FLEXCFA_SET0_GRAD_HZ_PHASE0_MAX                                    (0x000000FFU)

/* SET0_GRAD_VT */

#define CSL_FLEXCFA_SET0_GRAD_VT_PHASE3_MASK                                   (0xFF000000U)
#define CSL_FLEXCFA_SET0_GRAD_VT_PHASE3_SHIFT                                  (0x00000018U)
#define CSL_FLEXCFA_SET0_GRAD_VT_PHASE3_MAX                                    (0x000000FFU)

#define CSL_FLEXCFA_SET0_GRAD_VT_PHASE2_MASK                                   (0x00FF0000U)
#define CSL_FLEXCFA_SET0_GRAD_VT_PHASE2_SHIFT                                  (0x00000010U)
#define CSL_FLEXCFA_SET0_GRAD_VT_PHASE2_MAX                                    (0x000000FFU)

#define CSL_FLEXCFA_SET0_GRAD_VT_PHASE1_MASK                                   (0x0000FF00U)
#define CSL_FLEXCFA_SET0_GRAD_VT_PHASE1_SHIFT                                  (0x00000008U)
#define CSL_FLEXCFA_SET0_GRAD_VT_PHASE1_MAX                                    (0x000000FFU)

#define CSL_FLEXCFA_SET0_GRAD_VT_PHASE0_MASK                                   (0x000000FFU)
#define CSL_FLEXCFA_SET0_GRAD_VT_PHASE0_SHIFT                                  (0x00000000U)
#define CSL_FLEXCFA_SET0_GRAD_VT_PHASE0_MAX                                    (0x000000FFU)

/* SET0_INTENSITY0 */

#define CSL_FLEXCFA_SET0_INTENSITY0_SHIFT_PH1_MASK                             (0x70000000U)
#define CSL_FLEXCFA_SET0_INTENSITY0_SHIFT_PH1_SHIFT                            (0x0000001CU)
#define CSL_FLEXCFA_SET0_INTENSITY0_SHIFT_PH1_MAX                              (0x00000007U)

#define CSL_FLEXCFA_SET0_INTENSITY0_BITFIELD_PH1_MASK                          (0x000F0000U)
#define CSL_FLEXCFA_SET0_INTENSITY0_BITFIELD_PH1_SHIFT                         (0x00000010U)
#define CSL_FLEXCFA_SET0_INTENSITY0_BITFIELD_PH1_MAX                           (0x0000000FU)

#define CSL_FLEXCFA_SET0_INTENSITY0_SHIFT_PH0_MASK                             (0x00007000U)
#define CSL_FLEXCFA_SET0_INTENSITY0_SHIFT_PH0_SHIFT                            (0x0000000CU)
#define CSL_FLEXCFA_SET0_INTENSITY0_SHIFT_PH0_MAX                              (0x00000007U)

#define CSL_FLEXCFA_SET0_INTENSITY0_BITFIELD_PH0_MASK                          (0x0000000FU)
#define CSL_FLEXCFA_SET0_INTENSITY0_BITFIELD_PH0_SHIFT                         (0x00000000U)
#define CSL_FLEXCFA_SET0_INTENSITY0_BITFIELD_PH0_MAX                           (0x0000000FU)

/* SET0_INTENSITY1 */

#define CSL_FLEXCFA_SET0_INTENSITY1_SHIFT_PH3_MASK                             (0x70000000U)
#define CSL_FLEXCFA_SET0_INTENSITY1_SHIFT_PH3_SHIFT                            (0x0000001CU)
#define CSL_FLEXCFA_SET0_INTENSITY1_SHIFT_PH3_MAX                              (0x00000007U)

#define CSL_FLEXCFA_SET0_INTENSITY1_BITFIELD_PH3_MASK                          (0x000F0000U)
#define CSL_FLEXCFA_SET0_INTENSITY1_BITFIELD_PH3_SHIFT                         (0x00000010U)
#define CSL_FLEXCFA_SET0_INTENSITY1_BITFIELD_PH3_MAX                           (0x0000000FU)

#define CSL_FLEXCFA_SET0_INTENSITY1_SHIFT_PH2_MASK                             (0x00007000U)
#define CSL_FLEXCFA_SET0_INTENSITY1_SHIFT_PH2_SHIFT                            (0x0000000CU)
#define CSL_FLEXCFA_SET0_INTENSITY1_SHIFT_PH2_MAX                              (0x00000007U)

#define CSL_FLEXCFA_SET0_INTENSITY1_BITFIELD_PH2_MASK                          (0x0000000FU)
#define CSL_FLEXCFA_SET0_INTENSITY1_BITFIELD_PH2_SHIFT                         (0x00000000U)
#define CSL_FLEXCFA_SET0_INTENSITY1_BITFIELD_PH2_MAX                           (0x0000000FU)

/* SET1_GRAD_HZ */

#define CSL_FLEXCFA_SET1_GRAD_HZ_PHASE3_MASK                                   (0xFF000000U)
#define CSL_FLEXCFA_SET1_GRAD_HZ_PHASE3_SHIFT                                  (0x00000018U)
#define CSL_FLEXCFA_SET1_GRAD_HZ_PHASE3_MAX                                    (0x000000FFU)

#define CSL_FLEXCFA_SET1_GRAD_HZ_PHASE2_MASK                                   (0x00FF0000U)
#define CSL_FLEXCFA_SET1_GRAD_HZ_PHASE2_SHIFT                                  (0x00000010U)
#define CSL_FLEXCFA_SET1_GRAD_HZ_PHASE2_MAX                                    (0x000000FFU)

#define CSL_FLEXCFA_SET1_GRAD_HZ_PHASE1_MASK                                   (0x0000FF00U)
#define CSL_FLEXCFA_SET1_GRAD_HZ_PHASE1_SHIFT                                  (0x00000008U)
#define CSL_FLEXCFA_SET1_GRAD_HZ_PHASE1_MAX                                    (0x000000FFU)

#define CSL_FLEXCFA_SET1_GRAD_HZ_PHASE0_MASK                                   (0x000000FFU)
#define CSL_FLEXCFA_SET1_GRAD_HZ_PHASE0_SHIFT                                  (0x00000000U)
#define CSL_FLEXCFA_SET1_GRAD_HZ_PHASE0_MAX                                    (0x000000FFU)

/* SET1_GRAD_VT */

#define CSL_FLEXCFA_SET1_GRAD_VT_PHASE3_MASK                                   (0xFF000000U)
#define CSL_FLEXCFA_SET1_GRAD_VT_PHASE3_SHIFT                                  (0x00000018U)
#define CSL_FLEXCFA_SET1_GRAD_VT_PHASE3_MAX                                    (0x000000FFU)

#define CSL_FLEXCFA_SET1_GRAD_VT_PHASE2_MASK                                   (0x00FF0000U)
#define CSL_FLEXCFA_SET1_GRAD_VT_PHASE2_SHIFT                                  (0x00000010U)
#define CSL_FLEXCFA_SET1_GRAD_VT_PHASE2_MAX                                    (0x000000FFU)

#define CSL_FLEXCFA_SET1_GRAD_VT_PHASE1_MASK                                   (0x0000FF00U)
#define CSL_FLEXCFA_SET1_GRAD_VT_PHASE1_SHIFT                                  (0x00000008U)
#define CSL_FLEXCFA_SET1_GRAD_VT_PHASE1_MAX                                    (0x000000FFU)

#define CSL_FLEXCFA_SET1_GRAD_VT_PHASE0_MASK                                   (0x000000FFU)
#define CSL_FLEXCFA_SET1_GRAD_VT_PHASE0_SHIFT                                  (0x00000000U)
#define CSL_FLEXCFA_SET1_GRAD_VT_PHASE0_MAX                                    (0x000000FFU)

/* SET1_INTENSITY0 */

#define CSL_FLEXCFA_SET1_INTENSITY0_SHIFT_PH1_MASK                             (0x70000000U)
#define CSL_FLEXCFA_SET1_INTENSITY0_SHIFT_PH1_SHIFT                            (0x0000001CU)
#define CSL_FLEXCFA_SET1_INTENSITY0_SHIFT_PH1_MAX                              (0x00000007U)

#define CSL_FLEXCFA_SET1_INTENSITY0_BITFIELD_PH1_MASK                          (0x000F0000U)
#define CSL_FLEXCFA_SET1_INTENSITY0_BITFIELD_PH1_SHIFT                         (0x00000010U)
#define CSL_FLEXCFA_SET1_INTENSITY0_BITFIELD_PH1_MAX                           (0x0000000FU)

#define CSL_FLEXCFA_SET1_INTENSITY0_SHIFT_PH0_MASK                             (0x00007000U)
#define CSL_FLEXCFA_SET1_INTENSITY0_SHIFT_PH0_SHIFT                            (0x0000000CU)
#define CSL_FLEXCFA_SET1_INTENSITY0_SHIFT_PH0_MAX                              (0x00000007U)

#define CSL_FLEXCFA_SET1_INTENSITY0_BITFIELD_PH0_MASK                          (0x0000000FU)
#define CSL_FLEXCFA_SET1_INTENSITY0_BITFIELD_PH0_SHIFT                         (0x00000000U)
#define CSL_FLEXCFA_SET1_INTENSITY0_BITFIELD_PH0_MAX                           (0x0000000FU)

/* SET1_INTENSITY1 */

#define CSL_FLEXCFA_SET1_INTENSITY1_SHIFT_PH3_MASK                             (0x70000000U)
#define CSL_FLEXCFA_SET1_INTENSITY1_SHIFT_PH3_SHIFT                            (0x0000001CU)
#define CSL_FLEXCFA_SET1_INTENSITY1_SHIFT_PH3_MAX                              (0x00000007U)

#define CSL_FLEXCFA_SET1_INTENSITY1_BITFIELD_PH3_MASK                          (0x000F0000U)
#define CSL_FLEXCFA_SET1_INTENSITY1_BITFIELD_PH3_SHIFT                         (0x00000010U)
#define CSL_FLEXCFA_SET1_INTENSITY1_BITFIELD_PH3_MAX                           (0x0000000FU)

#define CSL_FLEXCFA_SET1_INTENSITY1_SHIFT_PH2_MASK                             (0x00007000U)
#define CSL_FLEXCFA_SET1_INTENSITY1_SHIFT_PH2_SHIFT                            (0x0000000CU)
#define CSL_FLEXCFA_SET1_INTENSITY1_SHIFT_PH2_MAX                              (0x00000007U)

#define CSL_FLEXCFA_SET1_INTENSITY1_BITFIELD_PH2_MASK                          (0x0000000FU)
#define CSL_FLEXCFA_SET1_INTENSITY1_BITFIELD_PH2_SHIFT                         (0x00000000U)
#define CSL_FLEXCFA_SET1_INTENSITY1_BITFIELD_PH2_MAX                           (0x0000000FU)

/* SET0_THR0_1 */

#define CSL_FLEXCFA_SET0_THR0_1_THR_1_MASK                                     (0xFFFF0000U)
#define CSL_FLEXCFA_SET0_THR0_1_THR_1_SHIFT                                    (0x00000010U)
#define CSL_FLEXCFA_SET0_THR0_1_THR_1_MAX                                      (0x0000FFFFU)

#define CSL_FLEXCFA_SET0_THR0_1_THR_0_MASK                                     (0x0000FFFFU)
#define CSL_FLEXCFA_SET0_THR0_1_THR_0_SHIFT                                    (0x00000000U)
#define CSL_FLEXCFA_SET0_THR0_1_THR_0_MAX                                      (0x0000FFFFU)

/* SET0_THR2_3 */

#define CSL_FLEXCFA_SET0_THR2_3_THR_3_MASK                                     (0xFFFF0000U)
#define CSL_FLEXCFA_SET0_THR2_3_THR_3_SHIFT                                    (0x00000010U)
#define CSL_FLEXCFA_SET0_THR2_3_THR_3_MAX                                      (0x0000FFFFU)

#define CSL_FLEXCFA_SET0_THR2_3_THR_2_MASK                                     (0x0000FFFFU)
#define CSL_FLEXCFA_SET0_THR2_3_THR_2_SHIFT                                    (0x00000000U)
#define CSL_FLEXCFA_SET0_THR2_3_THR_2_MAX                                      (0x0000FFFFU)

/* SET0_THR4_5 */

#define CSL_FLEXCFA_SET0_THR4_5_THR_5_MASK                                     (0xFFFF0000U)
#define CSL_FLEXCFA_SET0_THR4_5_THR_5_SHIFT                                    (0x00000010U)
#define CSL_FLEXCFA_SET0_THR4_5_THR_5_MAX                                      (0x0000FFFFU)

#define CSL_FLEXCFA_SET0_THR4_5_THR_4_MASK                                     (0x0000FFFFU)
#define CSL_FLEXCFA_SET0_THR4_5_THR_4_SHIFT                                    (0x00000000U)
#define CSL_FLEXCFA_SET0_THR4_5_THR_4_MAX                                      (0x0000FFFFU)

/* SET0_THR6 */

#define CSL_FLEXCFA_SET0_THR6_THR_6_MASK                                       (0x0000FFFFU)
#define CSL_FLEXCFA_SET0_THR6_THR_6_SHIFT                                      (0x00000000U)
#define CSL_FLEXCFA_SET0_THR6_THR_6_MAX                                        (0x0000FFFFU)

/* SET1_THR0_1 */

#define CSL_FLEXCFA_SET1_THR0_1_THR_1_MASK                                     (0xFFFF0000U)
#define CSL_FLEXCFA_SET1_THR0_1_THR_1_SHIFT                                    (0x00000010U)
#define CSL_FLEXCFA_SET1_THR0_1_THR_1_MAX                                      (0x0000FFFFU)

#define CSL_FLEXCFA_SET1_THR0_1_THR_0_MASK                                     (0x0000FFFFU)
#define CSL_FLEXCFA_SET1_THR0_1_THR_0_SHIFT                                    (0x00000000U)
#define CSL_FLEXCFA_SET1_THR0_1_THR_0_MAX                                      (0x0000FFFFU)

/* SET1_THR2_3 */

#define CSL_FLEXCFA_SET1_THR2_3_THR_3_MASK                                     (0xFFFF0000U)
#define CSL_FLEXCFA_SET1_THR2_3_THR_3_SHIFT                                    (0x00000010U)
#define CSL_FLEXCFA_SET1_THR2_3_THR_3_MAX                                      (0x0000FFFFU)

#define CSL_FLEXCFA_SET1_THR2_3_THR_2_MASK                                     (0x0000FFFFU)
#define CSL_FLEXCFA_SET1_THR2_3_THR_2_SHIFT                                    (0x00000000U)
#define CSL_FLEXCFA_SET1_THR2_3_THR_2_MAX                                      (0x0000FFFFU)

/* SET1_THR4_5 */

#define CSL_FLEXCFA_SET1_THR4_5_THR_5_MASK                                     (0xFFFF0000U)
#define CSL_FLEXCFA_SET1_THR4_5_THR_5_SHIFT                                    (0x00000010U)
#define CSL_FLEXCFA_SET1_THR4_5_THR_5_MAX                                      (0x0000FFFFU)

#define CSL_FLEXCFA_SET1_THR4_5_THR_4_MASK                                     (0x0000FFFFU)
#define CSL_FLEXCFA_SET1_THR4_5_THR_4_SHIFT                                    (0x00000000U)
#define CSL_FLEXCFA_SET1_THR4_5_THR_4_MAX                                      (0x0000FFFFU)

/* SET1_THR6 */

#define CSL_FLEXCFA_SET1_THR6_THR_6_MASK                                       (0x0000FFFFU)
#define CSL_FLEXCFA_SET1_THR6_THR_6_SHIFT                                      (0x00000000U)
#define CSL_FLEXCFA_SET1_THR6_THR_6_MAX                                        (0x0000FFFFU)

/* INT_STATUS */

#define CSL_FLEXCFA_INT_STATUS_LUT_CFG_ERR_MASK                                (0x00000001U)
#define CSL_FLEXCFA_INT_STATUS_LUT_CFG_ERR_SHIFT                               (0x00000000U)
#define CSL_FLEXCFA_INT_STATUS_LUT_CFG_ERR_MAX                                 (0x00000001U)

#define CSL_FLEXCFA_INT_STATUS_CFA_PIX_ERR_MASK                                (0x00000002U)
#define CSL_FLEXCFA_INT_STATUS_CFA_PIX_ERR_SHIFT                               (0x00000001U)
#define CSL_FLEXCFA_INT_STATUS_CFA_PIX_ERR_MAX                                 (0x00000001U)

#define CSL_FLEXCFA_INT_STATUS_CFA_MMR_ERR_MASK                                (0x00000004U)
#define CSL_FLEXCFA_INT_STATUS_CFA_MMR_ERR_SHIFT                               (0x00000002U)
#define CSL_FLEXCFA_INT_STATUS_CFA_MMR_ERR_MAX                                 (0x00000001U)

/* DEBUG_CTL */

#define CSL_FLEXCFA_DEBUG_CTL_SOF_EN_MASK                                      (0x00000004U)
#define CSL_FLEXCFA_DEBUG_CTL_SOF_EN_SHIFT                                     (0x00000002U)
#define CSL_FLEXCFA_DEBUG_CTL_SOF_EN_MAX                                       (0x00000001U)

#define CSL_FLEXCFA_DEBUG_CTL_SOL_EN_MASK                                      (0x00000002U)
#define CSL_FLEXCFA_DEBUG_CTL_SOL_EN_SHIFT                                     (0x00000001U)
#define CSL_FLEXCFA_DEBUG_CTL_SOL_EN_MAX                                       (0x00000001U)

#define CSL_FLEXCFA_DEBUG_CTL_DBG_EN_MASK                                      (0x00000001U)
#define CSL_FLEXCFA_DEBUG_CTL_DBG_EN_SHIFT                                     (0x00000000U)
#define CSL_FLEXCFA_DEBUG_CTL_DBG_EN_MAX                                       (0x00000001U)

/* DEBUG_STATUS */

#define CSL_FLEXCFA_DEBUG_STATUS_SOF_EVENT_MASK                                (0x00000004U)
#define CSL_FLEXCFA_DEBUG_STATUS_SOF_EVENT_SHIFT                               (0x00000002U)
#define CSL_FLEXCFA_DEBUG_STATUS_SOF_EVENT_MAX                                 (0x00000001U)

#define CSL_FLEXCFA_DEBUG_STATUS_SOL_EVENT_MASK                                (0x00000002U)
#define CSL_FLEXCFA_DEBUG_STATUS_SOL_EVENT_SHIFT                               (0x00000001U)
#define CSL_FLEXCFA_DEBUG_STATUS_SOL_EVENT_MAX                                 (0x00000001U)

/* LINE_SEL */

#define CSL_FLEXCFA_LINE_SEL_LINE_SELECTOR_MASK                                (0x00000007U)
#define CSL_FLEXCFA_LINE_SEL_LINE_SELECTOR_SHIFT                               (0x00000000U)
#define CSL_FLEXCFA_LINE_SEL_LINE_SELECTOR_MAX                                 (0x00000007U)

/* PIXEL_RAM */

#define CSL_FLEXCFA_PIXEL_RAM_PIXEL_LO_MASK                                    (0x00000FFFU)
#define CSL_FLEXCFA_PIXEL_RAM_PIXEL_LO_SHIFT                                   (0x00000000U)
#define CSL_FLEXCFA_PIXEL_RAM_PIXEL_LO_MAX                                     (0x00000FFFU)

#define CSL_FLEXCFA_PIXEL_RAM_PIXEL_HI_MASK                                    (0x0FFF0000U)
#define CSL_FLEXCFA_PIXEL_RAM_PIXEL_HI_SHIFT                                   (0x00000010U)
#define CSL_FLEXCFA_PIXEL_RAM_PIXEL_HI_MAX                                     (0x00000FFFU)

#ifdef __cplusplus
}
#endif
#endif
