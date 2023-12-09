/*
 * Copyright (c) 2017-present, Texas Instruments Incorporated
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
/** ============================================================================
 *  @file       osal_soc.h
 *
 *  @brief      SOC specific includes for Osal
 *
 *
 *  ============================================================================
 */
#ifndef ti_osal_soc_j7_include
#define ti_osal_soc_j7_include

#ifdef __cplusplus
extern "C" {
#endif
#include <ti/osal/osal.h>
#include <ti/csl/soc.h>
#if defined(TimerP_numTimerDevices)
#undef  TimerP_numTimerDevices
#endif

#if defined(TIMERP_ANY_MASK)
#undef  TIMERP_ANY_MASK
#endif

#define  EXTERNAL_CLOCK_KHZ_DEFAULT       (24000)
#define  PMU_CLOCK_KHZ_DEFAULT            (1000000U)
#define  OSAL_CPU_FREQ_KHZ_DEFAULT        (1000000U)

#if defined(TIMERP_TIMER_FREQ_LO)
#undef   TIMERP_TIMER_FREQ_LO
#endif

#if defined(TIMERP_TIMER_FREQ_HI)
#undef   TIMERP_TIMER_FREQ_HI
#endif

#define  TIMERP_TIMER_FREQ_LO   ((int32_t) 19200000)
#define  TIMERP_TIMER_FREQ_HI   ((int32_t) 0)

#define TimerP_numTimerDevices          ((uint32_t) 20U )
#if defined (BUILD_MCU)
  #define TIMERP_ANY_MASK               ((uint32_t) 0x000F)
  #define TIMERP_AVAILABLE_MASK       ((uint32_t)(0x000F))
#else
  #define TIMERP_ANY_MASK               ((uint32_t) 0x0FFF)
  #define TIMERP_AVAILABLE_MASK       ((uint32_t)(0x0FFF))
#endif

/* using the default timer base addresses */
#if defined(__aarch64__)
#define OSAL_DELAY_TIMER_ADDR_DEFAULT   (CSL_TIMER1_CFG_BASE)
#else
#define OSAL_DELAY_TIMER_ADDR_DEFAULT   (CSL_MCU_TIMER1_CFG_BASE)
#endif

#define TIMER_INITIAL_COUNT             (0xFFF00000U)
#define TIMER_RLD_COUNT                 (0xFFF00000U)

/* GTC count clock for A53 */
#define  OSAL_SOC_MPU_GTC_CLK               ((int32_t) 200000)

/* Max number of semaphores for NonOs */
#define OSAL_NONOS_MAX_SEMAPHOREP_PER_SOC   ((uint32_t) 80U)
#define OSAL_NONOS_MAX_HWIP_PER_SOC         ((uint32_t) 40U)
#define OSAL_NONOS_MAX_TIMERP_PER_SOC       (TimerP_numTimerDevices)

#define OSAL_TIRTOS_MAX_SEMAPHOREP_PER_SOC   ((uint32_t) 360U)
#define OSAL_TIRTOS_MAX_HWIP_PER_SOC         ((uint32_t) 40U)
#define OSAL_TIRTOS_MAX_TIMERP_PER_SOC       (TimerP_numTimerDevices)
#define OSAL_TIRTOS_MAX_MUTEXP_PER_SOC       ((uint32_t) 20U)
#define OSAL_TIRTOS_MAX_HEAPP_PER_SOC        ((uint32_t) 20U)

#define OSAL_FREERTOS_MAX_SEMAPHOREP_PER_SOC ((uint32_t) 360U)
#define OSAL_FREERTOS_MAX_HWIP_PER_SOC       ((uint32_t) 40U)
#define OSAL_FREERTOS_MAX_TIMERP_PER_SOC     (TimerP_numTimerDevices)
#define OSAL_FREERTOS_MAX_TASKP_PER_SOC      ((uint32_t) 160U)
#define OSAL_FREERTOS_MAX_CLOCKP_PER_SOC     ((uint32_t) 20U)
#define OSAL_FREERTOS_MAX_MUTEXP_PER_SOC     ((uint32_t) 20U)
#define OSAL_FREERTOS_MAX_MAILBOXP_PER_SOC   ((uint32_t) 20U)
#define OSAL_FREERTOS_MAX_QUEUEP_PER_SOC     ((uint32_t) 40U)
#define OSAL_FREERTOS_MAX_HEAPP_PER_SOC      ((uint32_t) 20U)
#define OSAL_FREERTOS_MAX_EVENTP_PER_SOC     ((uint32_t) 20U)

#define OSAL_ARCH_TIMER_INST_FOR_TS         (TimerP_ANY)
/**< Default timer instance for timer */

/* external references */
extern Osal_HwAttrs  gOsal_HwAttrs;

#ifdef __cplusplus
}
#endif

#endif
/* nothing past this point */
