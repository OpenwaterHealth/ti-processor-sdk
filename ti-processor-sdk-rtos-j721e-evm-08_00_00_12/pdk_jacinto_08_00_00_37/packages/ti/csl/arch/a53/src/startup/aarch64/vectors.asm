/*
 *  Copyright (c) 2017, Texas Instruments Incorporated
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 * ======== vectors.asm ========
 */

#include <ti/csl/arch/a53/intMacros.h>

        .global ti_csl_arm_gicv3_vectors
        .global CSL_armGicDefaultExcHandler
        .global CSL_armGicDispatchIRQ
		.global Intc_AbortHandler

        .section .vecs, "ax"

        .align  11
ti_csl_arm_gicv3_vectors:

/*
 *************************************************************************
 * Exception from currentEL, using SP0
 *************************************************************************
 */
VECTOR_ENTRY el1SyncSP0
        PUSH_ALL_CPU_REGS sp        /* save all CPU regs */
        mrs     x0, sp_el0
        mrs     x1, elr_el1
        mrs     x2, spsr_el1
        mrs     x3, esr_el1
        stp     x0, x1, [sp, #-16]! /* save sp & elr */
        stp     x2, x3, [sp, #-16]! /* save spsr and esr */
        mov     x0, sp
        mov     x1, #0
        ldr     x2, =CSL_armGicDefaultExcHandler
        br      x2

VECTOR_ENTRY el1IrqSP0
        PUSH_CALLER_SAVE_CPU_REGS sp /* save cpu scratch regs */
#if defined(__ARM_FP)
        PUSH_CALLER_SAVE_FPU_REGS sp /* save vfp scratch regs */
#endif
        mov     x0, #1               /* pass TRUE argument to dispatchIRQ */
        ldr     x1, =CSL_armGicDispatchIRQ
        br      x1

VECTOR_ENTRY el1FiqSP0
        b       el1FiqSP0

VECTOR_ENTRY el1SErrorSP0
        PUSH_ALL_CPU_REGS sp        /* save all CPU regs */
        mrs     x0, sp_el0
        mrs     x1, elr_el1
        mrs     x2, spsr_el1
        mrs     x3, esr_el1
        stp     x0, x1, [sp, #-16]! /* save sp & elr */
        stp     x2, x3, [sp, #-16]! /* save spsr and esr */
        mov     x0, sp
        mov     x1, #1
        ldr     x2, =CSL_armGicDefaultExcHandler
        br      x2

/*
 *************************************************************************
 * Exception from currentEL, using SPx
 *************************************************************************
 */
VECTOR_ENTRY el1SyncSPx
        PUSH_ALL_CPU_REGS sp        /* save all CPU regs */
        add     x0, sp, #256        /* compute original SP */
        mrs     x1, elr_el1
        mrs     x2, spsr_el1
        mrs     x3, esr_el1
        stp     x0, x1, [sp, #-16]! /* save sp & elr */
        stp     x2, x3, [sp, #-16]! /* save spsr and esr */
        mov     x0, sp
        mov     x1, #0
        ldr     x2, =Intc_AbortHandler
        br      x2

VECTOR_ENTRY el1IrqSPx
        PUSH_CALLER_SAVE_CPU_REGS sp /* save cpu scratch regs */
#if defined(__ARM_FP)
        PUSH_CALLER_SAVE_FPU_REGS sp /* save vfp scratch regs */
#endif
        mov     x0, #0               /* pass FALSE argument to dispatchIRQ */
        ldr     x1, =CSL_armGicDispatchIRQ
        br      x1

VECTOR_ENTRY el1FiqSPx
        b    el1FiqSPx

VECTOR_ENTRY el1SErrorSPx
        PUSH_ALL_CPU_REGS sp        /* save all CPU regs */
        add     x0, sp, #256        /* compute original SP */
        mrs     x1, elr_el1
        mrs     x2, spsr_el1
        mrs     x3, esr_el1
        stp     x0, x1, [sp, #-16]! /* save sp & elr */
        stp     x2, x3, [sp, #-16]! /* save spsr and esr */
        mov     x0, sp
        mov     x1, #1
        ldr     x2, =CSL_armGicDefaultExcHandler
        br      x2

/*
 *************************************************************************
 * Exception from lowerEL, all lowerEL using Aarch64
 *************************************************************************
 */
VECTOR_ENTRY el0SyncAarch64
        b    el0SyncAarch64

VECTOR_ENTRY el0IrqAarch64
        b    el0IrqAarch64

VECTOR_ENTRY el0FiqAarch64
        b    el0FiqAarch64

VECTOR_ENTRY el0SErrorAarch64
        b    el0SErrorAarch64

/*
 *************************************************************************
 * Exception from lowerEL, all lowerEL using Aarch32
 *************************************************************************
 */
VECTOR_ENTRY el0SyncAarch32
        b    el0SyncAarch32

VECTOR_ENTRY el0IrqAarch32
        b    el0IrqAarch32

VECTOR_ENTRY el0FiqAarch32
        b    el0FiqAarch32

VECTOR_ENTRY el0SErrorAarch32
        b    el0SErrorAarch32

        .end
