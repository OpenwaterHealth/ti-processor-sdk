/********************************************************************
 * Copyright (C) 2013-2014 Texas Instruments Incorporated.
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
#ifndef CSLR_ICSSM_PRU_CTRL_H_
#define CSLR_ICSSM_PRU_CTRL_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include <ti/csl/cslr.h>
#include <ti/csl/tistdtypes.h>


/**************************************************************************
* Register Overlay Structure for pru_ctrladdressBlockTable
**************************************************************************/
typedef struct {
    volatile Uint32 CONTROL;
    volatile Uint32 STATUS;
    volatile Uint32 WAKEUP_EN;
    volatile Uint32 CYCLE;
    volatile Uint32 STALL;
    volatile Uint8  RSVD0[12];
    volatile Uint32 CTBIR0;
    volatile Uint32 CTBIR1;
    volatile Uint32 CTPPR0;
    volatile Uint32 CTPPR1;
} CSL_icssm_pru_ctrlRegs;


/**************************************************************************
* Register Macros
**************************************************************************/
#define CSL_ICSSM_PRU_CTRL_CONTROL                              (0x0U)
#define CSL_ICSSM_PRU_CTRL_STATUS                               (0x4U)
#define CSL_ICSSM_PRU_CTRL_WAKEUP_EN                            (0x8U)
#define CSL_ICSSM_PRU_CTRL_CYCLE                                (0xCU)
#define CSL_ICSSM_PRU_CTRL_STALL                                (0x10U)
#define CSL_ICSSM_PRU_CTRL_CTBIR0                               (0x20U)
#define CSL_ICSSM_PRU_CTRL_CTBIR1                               (0x24U)
#define CSL_ICSSM_PRU_CTRL_CTPPR0                               (0x28U)
#define CSL_ICSSM_PRU_CTRL_CTPPR1                               (0x2CU)


/**************************************************************************
* Field Definition Macros
**************************************************************************/

/* CONTROL */

#define CSL_ICSSM_PRU_CTRL_CONTROL_SOFT_RST_N_MASK              (0x00000001U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_SOFT_RST_N_SHIFT             (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_SOFT_RST_N_RESETVAL          (0x00000001U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_SOFT_RST_N_MAX               (0x00000001U)

#define CSL_ICSSM_PRU_CTRL_CONTROL_ENABLE_MASK                  (0x00000002U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_ENABLE_SHIFT                 (0x00000001U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_ENABLE_RESETVAL              (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_ENABLE_MAX                   (0x00000001U)

#define CSL_ICSSM_PRU_CTRL_CONTROL_SLEEPING_MASK                (0x00000004U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_SLEEPING_SHIFT               (0x00000002U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_SLEEPING_RESETVAL            (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_SLEEPING_MAX                 (0x00000001U)

#define CSL_ICSSM_PRU_CTRL_CONTROL_COUNTER_ENABLE_MASK          (0x00000008U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_COUNTER_ENABLE_SHIFT         (0x00000003U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_COUNTER_ENABLE_RESETVAL      (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_COUNTER_ENABLE_MAX           (0x00000001U)

#define CSL_ICSSM_PRU_CTRL_CONTROL_SINGLE_STEP_MASK             (0x00000100U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_SINGLE_STEP_SHIFT            (0x00000008U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_SINGLE_STEP_RESETVAL         (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_SINGLE_STEP_MAX              (0x00000001U)

#define CSL_ICSSM_PRU_CTRL_CONTROL_BIG_ENDIAN_MASK              (0x00004000U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_BIG_ENDIAN_SHIFT             (0x0000000EU)
#define CSL_ICSSM_PRU_CTRL_CONTROL_BIG_ENDIAN_RESETVAL          (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_BIG_ENDIAN_MAX               (0x00000001U)

#define CSL_ICSSM_PRU_CTRL_CONTROL_RUNSTATE_MASK                (0x00008000U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_RUNSTATE_SHIFT               (0x0000000FU)
#define CSL_ICSSM_PRU_CTRL_CONTROL_RUNSTATE_RESETVAL            (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_RUNSTATE_MAX                 (0x00000001U)

#define CSL_ICSSM_PRU_CTRL_CONTROL_PCOUNTER_RST_VAL_MASK        (0xFFFF0000U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_PCOUNTER_RST_VAL_SHIFT       (0x00000010U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_PCOUNTER_RST_VAL_RESETVAL    (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CONTROL_PCOUNTER_RST_VAL_MAX         (0x0000ffffU)

#define CSL_ICSSM_PRU_CTRL_CONTROL_RESETVAL                     (0x00000001U)

/* STATUS */

#define CSL_ICSSM_PRU_CTRL_STATUS_PCOUNTER_MASK                 (0x0000FFFFU)
#define CSL_ICSSM_PRU_CTRL_STATUS_PCOUNTER_SHIFT                (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_STATUS_PCOUNTER_RESETVAL             (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_STATUS_PCOUNTER_MAX                  (0x0000ffffU)

#define CSL_ICSSM_PRU_CTRL_STATUS_RESETVAL                      (0x00000000U)

/* WAKEUP_EN */

#define CSL_ICSSM_PRU_CTRL_WAKEUP_EN_BITWISE_ENABLES_MASK       (0xFFFFFFFFU)
#define CSL_ICSSM_PRU_CTRL_WAKEUP_EN_BITWISE_ENABLES_SHIFT      (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_WAKEUP_EN_BITWISE_ENABLES_RESETVAL   (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_WAKEUP_EN_BITWISE_ENABLES_MAX        (0xffffffffU)

#define CSL_ICSSM_PRU_CTRL_WAKEUP_EN_RESETVAL                   (0x00000000U)

/* CYCLE */

#define CSL_ICSSM_PRU_CTRL_CYCLE_CYCLECOUNT_MASK                (0xFFFFFFFFU)
#define CSL_ICSSM_PRU_CTRL_CYCLE_CYCLECOUNT_SHIFT               (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CYCLE_CYCLECOUNT_RESETVAL            (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CYCLE_CYCLECOUNT_MAX                 (0xffffffffU)

#define CSL_ICSSM_PRU_CTRL_CYCLE_RESETVAL                       (0x00000000U)

/* STALL */

#define CSL_ICSSM_PRU_CTRL_STALL_STALLCOUNT_MASK                (0xFFFFFFFFU)
#define CSL_ICSSM_PRU_CTRL_STALL_STALLCOUNT_SHIFT               (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_STALL_STALLCOUNT_RESETVAL            (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_STALL_STALLCOUNT_MAX                 (0xffffffffU)

#define CSL_ICSSM_PRU_CTRL_STALL_RESETVAL                       (0x00000000U)

/* CTBIR0 */

#define CSL_ICSSM_PRU_CTRL_CTBIR0_C24_BLK_INDEX_MASK            (0x000000FFU)
#define CSL_ICSSM_PRU_CTRL_CTBIR0_C24_BLK_INDEX_SHIFT           (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CTBIR0_C24_BLK_INDEX_RESETVAL        (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CTBIR0_C24_BLK_INDEX_MAX             (0x000000ffU)

#define CSL_ICSSM_PRU_CTRL_CTBIR0_C25_BLK_INDEX_MASK            (0x00FF0000U)
#define CSL_ICSSM_PRU_CTRL_CTBIR0_C25_BLK_INDEX_SHIFT           (0x00000010U)
#define CSL_ICSSM_PRU_CTRL_CTBIR0_C25_BLK_INDEX_RESETVAL        (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CTBIR0_C25_BLK_INDEX_MAX             (0x000000ffU)

#define CSL_ICSSM_PRU_CTRL_CTBIR0_RESETVAL                      (0x00000000U)

/* CTBIR1 */

#define CSL_ICSSM_PRU_CTRL_CTBIR1_C26_BLK_INDEX_MASK            (0x000000FFU)
#define CSL_ICSSM_PRU_CTRL_CTBIR1_C26_BLK_INDEX_SHIFT           (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CTBIR1_C26_BLK_INDEX_RESETVAL        (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CTBIR1_C26_BLK_INDEX_MAX             (0x000000ffU)

#define CSL_ICSSM_PRU_CTRL_CTBIR1_C27_BLK_INDEX_MASK            (0x00FF0000U)
#define CSL_ICSSM_PRU_CTRL_CTBIR1_C27_BLK_INDEX_SHIFT           (0x00000010U)
#define CSL_ICSSM_PRU_CTRL_CTBIR1_C27_BLK_INDEX_RESETVAL        (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CTBIR1_C27_BLK_INDEX_MAX             (0x000000ffU)

#define CSL_ICSSM_PRU_CTRL_CTBIR1_RESETVAL                      (0x00000000U)

/* CTPPR0 */

#define CSL_ICSSM_PRU_CTRL_CTPPR0_C28_POINTER_MASK              (0x0000FFFFU)
#define CSL_ICSSM_PRU_CTRL_CTPPR0_C28_POINTER_SHIFT             (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CTPPR0_C28_POINTER_RESETVAL          (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CTPPR0_C28_POINTER_MAX               (0x0000ffffU)

#define CSL_ICSSM_PRU_CTRL_CTPPR0_C29_POINTER_MASK              (0xFFFF0000U)
#define CSL_ICSSM_PRU_CTRL_CTPPR0_C29_POINTER_SHIFT             (0x00000010U)
#define CSL_ICSSM_PRU_CTRL_CTPPR0_C29_POINTER_RESETVAL          (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CTPPR0_C29_POINTER_MAX               (0x0000ffffU)

#define CSL_ICSSM_PRU_CTRL_CTPPR0_RESETVAL                      (0x00000000U)

/* CTPPR1 */

#define CSL_ICSSM_PRU_CTRL_CTPPR1_C30_POINTER_MASK              (0x0000FFFFU)
#define CSL_ICSSM_PRU_CTRL_CTPPR1_C30_POINTER_SHIFT             (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CTPPR1_C30_POINTER_RESETVAL          (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CTPPR1_C30_POINTER_MAX               (0x0000ffffU)

#define CSL_ICSSM_PRU_CTRL_CTPPR1_C31_POINTER_MASK              (0xFFFF0000U)
#define CSL_ICSSM_PRU_CTRL_CTPPR1_C31_POINTER_SHIFT             (0x00000010U)
#define CSL_ICSSM_PRU_CTRL_CTPPR1_C31_POINTER_RESETVAL          (0x00000000U)
#define CSL_ICSSM_PRU_CTRL_CTPPR1_C31_POINTER_MAX               (0x0000ffffU)

#define CSL_ICSSM_PRU_CTRL_CTPPR1_RESETVAL                      (0x00000000U)

#ifdef __cplusplus
}
#endif
#endif
