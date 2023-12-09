/*
 *  Copyright (C) 2014 Texas Instruments Incorporated
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
 *  \file  main_arp32.c
 *
 *  \brief ARP32 CPU Idle Example Application
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "arp32.h"
#include <ti/csl/soc.h>
#include <ti/csl/hw_types.h>
#include <ti/csl/cslr_eve.h>
#include <ti/csl/cslr_edma.h>
#include <ti/csl/csl_timer.h>
#include <ti/csl/arch/csl_arch.h>
#include <ti/drv/pm/include/pmlib_sysconfig.h>
#include <ti/drv/pm/include/pm_utils.h>
#include <ti/drv/pm/include/pm_types.h>
#include <ti/drv/pm/include/pmlib_boardconfig.h>
#include <ti/drv/pm/include/pmlib_cpuidle.h>
#include <ti/drv/pm/include/prcm/hw_pmhal_data_names.h>
#include <ti/drv/pm/include/prcm/pmhal_pdm.h>
#include <ti/drv/pm/include/prcm/pmhal_cm.h>
#include "app_utils.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define TIMER_INITIAL_COUNT             (uint32_t) (0xFFF00000U)
#define TIMER_RLD_COUNT                 (uint32_t) (0xFFF00000U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

static void TimerPRCMConfigure(void);
static void TimerIntcInit(void);
static void TimerIntcDeInit(void);
static void TimerSetUp(void);
static void TimerIsr(void *handle);
static void TimerRun(void);
static void TimerStop(void);
static void CpuIdleTest(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static volatile uint32_t gCntValue = 10;
static volatile uint32_t gXbarInst = 1;
int32_t gTestStatus = PM_SUCCESS;
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static void CpuIdleTest(void)
{
    pmErrCode_t status;
    uint32_t    i;
    uint32_t    coreId = _get_cpunum();

    /*Set PRCM for Timer4 */
    /*This is done in SBL, adding here to make the app standalone */
    TimerPRCMConfigure();
    TimerRun();
    /* Wait to make sure VCOP is Idle */
    while (HW_RD_FIELD32(SOC_EVE_SYSTEM_BASE + EVE_STAT,
                         EVE_STAT_VCOP_STAT) != 0U)
    {; }

    /* Program Force Standby for the EDMA TCs */
    HW_WR_REG32(SOC_EVE_EDMA_TC0_BASE + EDMA_TC_SYSCONFIG, 0x0);
    HW_WR_REG32(SOC_EVE_EDMA_TC1_BASE + EDMA_TC_SYSCONFIG, 0x0);

    /* Wait to make sure EDMA TC0 is Idle */
    while (HW_RD_FIELD32(SOC_EVE_SYSTEM_BASE + EVE_STAT,
                         EVE_STAT_TC0_STAT) != 0U)
    {; }

    /* Wait to make sure EDMA TC1 is Idle */
    while (HW_RD_FIELD32(SOC_EVE_SYSTEM_BASE + EVE_STAT,
                         EVE_STAT_TC1_STAT) != 0U)
    {; }

    if (0U == coreId)
    {
        /* Request EVE PD to be ON and CD to be clock gated */
        status = (pmErrCode_t) PMHALPdmSetPDState(PMHAL_PRCM_PD_EVE1,
                                                  PMHAL_PRCM_PD_STATE_ON_ACTIVE,
                                                  PM_TIMEOUT_NOWAIT);

        status = (pmErrCode_t) PMHALCMSetCdClockMode(
            PMHAL_PRCM_CD_EVE1,
            PMHAL_PRCM_CD_CLKTRNMODES_HW_AUTO,
            PM_TIMEOUT_NOWAIT);
    }
    #if defined (SOC_TDA2XX) || defined (SOC_TDA2PX) || defined (SOC_TDA2EX)
    if (1U == coreId)
    {
        /* Request EVE PD to be ON and CD to be clock gated */
        status = (pmErrCode_t) PMHALPdmSetPDState(PMHAL_PRCM_PD_EVE2,
                                                  PMHAL_PRCM_PD_STATE_ON_ACTIVE,
                                                  PM_TIMEOUT_NOWAIT);

        status = (pmErrCode_t) PMHALCMSetCdClockMode(
            PMHAL_PRCM_CD_EVE2,
            PMHAL_PRCM_CD_CLKTRNMODES_HW_AUTO,
            PM_TIMEOUT_NOWAIT);
    }
    #endif
    #if defined (SOC_TDA2XX) || defined (SOC_TDA2EX)
    if (2U == coreId)
    {
        /* Request EVE PD to be ON and CD to be clock gated */
        status = (pmErrCode_t) PMHALPdmSetPDState(PMHAL_PRCM_PD_EVE3,
                                                  PMHAL_PRCM_PD_STATE_ON_ACTIVE,
                                                  PM_TIMEOUT_NOWAIT);

        status = (pmErrCode_t) PMHALCMSetCdClockMode(
            PMHAL_PRCM_CD_EVE3,
            PMHAL_PRCM_CD_CLKTRNMODES_HW_AUTO,
            PM_TIMEOUT_NOWAIT);
    }
    if (3U == coreId)
    {
        /* Request EVE PD to be ON and CD to be clock gated */
        status = (pmErrCode_t) PMHALPdmSetPDState(PMHAL_PRCM_PD_EVE4,
                                                  PMHAL_PRCM_PD_STATE_ON_ACTIVE,
                                                  PM_TIMEOUT_NOWAIT);

        status = (pmErrCode_t) PMHALCMSetCdClockMode(
            PMHAL_PRCM_CD_EVE4,
            PMHAL_PRCM_CD_CLKTRNMODES_HW_AUTO,
            PM_TIMEOUT_NOWAIT);
    }
    #endif
    for (i = 0; i < 10; i++)
    {
        status = PMLIBCpuIdle(PMHAL_PRCM_PD_STATE_ON_ACTIVE);
        if (status == PM_SUCCESS)
        {
            AppUtils_printf("\r\nEnter Targeted Power State successfully");
        }
        else
        {
            AppUtils_printf("\r\nEnter Targeted Power State failed");
            gTestStatus = PM_FAIL;
            break;
        }
    }
    TimerStop();
    AppUtils_printf("\r\nTest Completed!!");
}

int main(void)
{
    AppUtils_defaultInit();
    AppUtils_printf("\r\nPM CPUIdle Test Application");
    /* Function to Test CPU Idle */
    CpuIdleTest();

    if (gTestStatus == PM_SUCCESS)
    {
        AppUtils_printf("\r\nPM CPUIdle Test Pass");
    }
    else
    {
        AppUtils_printf("\r\nPM CPUIdle Test Fail");
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
/*                 Internal Function Definitions                              */
/* -------------------------------------------------------------------------- */

static void TimerRun(void)
{
    /* Register Timer4 interrupts on to INTC */
    TimerIntcInit();

    /* Perform the necessary configurations for Timer4 */
    TimerSetUp();

    /* Enable the Timer4 interrupts */
    TIMERIntEnable(SOC_TIMER4_BASE, TIMER_INT_OVF_EN_FLAG);

    /* Enable the Timer4 wakeup event */
    TIMERWakeEnable(SOC_TIMER4_BASE, TIMER_WAKE_OVF_FLAG);

    /* Set the Timer4 to Smart-idle mode */
    TIMERIdleModeConfigure(SOC_TIMER4_BASE, TIMER_SMART_IDLE);

    /* Start the Timer */
    TIMEREnable(SOC_TIMER4_BASE);
}

static void TimerStop(void)
{
    /* Stop the Timer */
    TIMERDisable(SOC_TIMER4_BASE);

    /* Disable the Timer4 wakeup event */
    TIMERWakeDisable(SOC_TIMER4_BASE, TIMER_WAKE_OVF_FLAG);

    /* Unregister Timer4 interrupts */
    TimerIntcDeInit();
}

/*
 * Do the necessary Timer configurations on to INTC.
 */
static void TimerIntcInit(void)
{
    uint32_t coreId = _get_cpunum();
    CSL_XbarIrqCpuId cpuId  = CSL_XBAR_IRQ_CPU_ID_EVE1;
    #if defined (SOC_TDA2XX) || defined (SOC_TDA2PX) || defined (SOC_TDA2EX)
    if (1U == coreId)
    {
        cpuId = CSL_XBAR_IRQ_CPU_ID_EVE2;
    }
    if (2U == coreId)
    {
        cpuId = CSL_XBAR_IRQ_CPU_ID_EVE3;
    }
    if (3U == coreId)
    {
        cpuId = CSL_XBAR_IRQ_CPU_ID_EVE4;
    }
    #endif

    /* Connect the Xbar with interrupt source */
    CSL_xbarIrqConfigure(cpuId,gXbarInst,CSL_XBAR_TIMER4_IRQ);

    /* Enable the interrupt */
    EVE_INTH_InterruptSetup(EVE_EVT_INT0,
                            (uint32_t) TimerIsr,
                            INTH_DEFAULT_INTERRUPT_KIND,
                            INTH_INT8);

    /* Actually set Enable bit in the interrupt controller */
    EVE_INTH_InterruptEnable(EVE_EVT_INT0);
    AppUtils_printf("\r\nInterrupt enable done");
}

/*
 * Disable the interrupt configurations on INTC.
 */
static void TimerIntcDeInit(void)
{
    /* Restore the initial state of gCntValue */
    gCntValue = 10;

    /* Disable the timer interrupt */
    EVE_INTH_InterruptDisable(EVE_EVT_INT0);
    AppUtils_printf("\r\nInterrupt disable done");
}

/*
 * Setup the timer for one-shot and compare mode.
 */
static void TimerSetUp(void)
{
    /*Reset the timer module */
    TIMERReset(SOC_TIMER4_BASE);

    /* Enable free run in emulation mode */
    TIMEREmuModeConfigure(SOC_TIMER4_BASE, (uint32_t) TIMER_FREE);

    /* Load the counter with the initial count value */
    TIMERCounterSet(SOC_TIMER4_BASE, (uint32_t) TIMER_INITIAL_COUNT);

    /* Load the load register with the reload count value */
    TIMERReloadSet(SOC_TIMER4_BASE, (uint32_t) TIMER_RLD_COUNT);

    /* Configure the Timer for Auto-reload and compare mode */
    TIMERModeConfigure(SOC_TIMER4_BASE,
                       (uint32_t) TIMER_AUTORLD_NOCMP_ENABLE);
}

/*
 * Timer interrupt service routine. This will send a character to serial
 * console.
 */
static void TimerIsr(void *handle)
{
    int32_t time = 0;
    time = (int32_t) TIMERCounterGet(SOC_TIMER4_BASE);
    AppUtils_printf("\r\nTime Taken to Wakeup:%d",(time - (int32_t) TIMER_RLD_COUNT));
    /* Disable the Timer interrupts */
    TIMERIntDisable(SOC_TIMER4_BASE,
                    (uint32_t) TIMER_INT_OVF_EN_FLAG);

    /* Clear the status of the interrupt flags */
    TIMERIntStatusClear(SOC_TIMER4_BASE,
                        (uint32_t) TIMER_INT_OVF_IT_FLAG);

    /* Enable the Timer interrupts */
    TIMERIntEnable(SOC_TIMER4_BASE, (uint32_t) TIMER_INT_OVF_EN_FLAG);
    AppUtils_printf("\r\nISR done");
}

/*
 * Timer4 PRCM configuration. This will explicitly enable the Timer4 module.
 */
static void TimerPRCMConfigure(void)
{
    /* PMHAL is not supported on EVE. Enabling Timer clocks using direct
     * register writes.
     */
    HW_WR_FIELD32((SOC_L4PER_CM_CORE_BASE + CM_L4PER_TIMER4_CLKCTRL),
                  CM_L4PER_TIMER4_CLKCTRL_MODULEMODE,
                  0x2);
}

