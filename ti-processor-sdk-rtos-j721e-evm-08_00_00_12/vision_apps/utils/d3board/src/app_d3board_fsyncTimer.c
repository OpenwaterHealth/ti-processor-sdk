
/*!
 * @file        app_d3board_fsyncTimer.c
 * @brief       d3board frame sync timer
 * @details     This file implements the frame sync timer
 *              that toggles the GPIO which controls the camera frame sync of the D3 personality card
 *
 * @copyright   Copyright (C) 2021 D3 Engineering, LLC. All Rights Reserved.
 */
#include <utils/console_io/include/app_log.h>
#include <stdio.h>
#include <string.h>
#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>
#include <utils/d3board/include/app_d3board.h>
#include <utils/d3board/include/app_d3board_gpio.h>
#include <FreeRTOS.h>

// For base addresses / timer API
#include <ti/csl/soc.h>
#include <ti/csl/csl_timer.h>


#define CTRLMMR_TIMERN_CLKSEL(n)    (CSL_CTRL_MMR0_CFG0_BASE + 0x8100U + (0x04U*n))
#define CTRLMMR_TIMERION_CTRL(n)    (CSL_CTRL_MMR0_CFG0_BASE + 0x4280U + (0x04U*n))
#define TIMERN_CFG_BASE(n)          (CSL_TIMER##n##_CFG_BASE)
// 225MHz clock
#define MAIN_PLL2_HSDIV6_CLKOUT     (0x0A)

const uint32_t TIMER_CFG_BASES[] = {
    TIMERN_CFG_BASE(0),
    TIMERN_CFG_BASE(1),
    TIMERN_CFG_BASE(2),
    TIMERN_CFG_BASE(3),
    TIMERN_CFG_BASE(4),
    TIMERN_CFG_BASE(5),
    TIMERN_CFG_BASE(6),
    TIMERN_CFG_BASE(7),
    TIMERN_CFG_BASE(8),
    TIMERN_CFG_BASE(9),
    TIMERN_CFG_BASE(10),
    TIMERN_CFG_BASE(11),
    TIMERN_CFG_BASE(12),
    TIMERN_CFG_BASE(13),
    TIMERN_CFG_BASE(14),
    TIMERN_CFG_BASE(15),
    TIMERN_CFG_BASE(16),
    TIMERN_CFG_BASE(17),
    TIMERN_CFG_BASE(18),
    TIMERN_CFG_BASE(19)
};

/* Set up a MAIN domain hardware timer to perform PWM w/ a 225MHz clock.
 * The timer must be started manually after configuration
 * 
 * timer = the timer to use
 * timerIO = the output pin that the timer will send output to
 * period = the timer's period (in ticks)
 * duration = the number of ticks the signal will remain high
 * offset = the number of ticks before the signal starts (relative to other PWM outputs)
 */
void appD3board_cfgHwTimer(uint32_t timer, uint32_t period, uint32_t duration, uint32_t offset, uint32_t resolution)
{
    uint32_t baseAddr = TIMER_CFG_BASES[timer];

    // Configure timer clock to be ~225MHz
    CSL_REG32_WR((volatile uint32_t *) CTRLMMR_TIMERN_CLKSEL(timer), MAIN_PLL2_HSDIV6_CLKOUT);

    TIMERReset(baseAddr);
    TIMERPostedModeConfig(baseAddr, TIMER_NONPOSTED);

    uint32_t timer_tclr_cfg = HW_RD_REG32(baseAddr + TIMER_TCLR);
    timer_tclr_cfg &= ~TIMER_TCLR_GPO_CFG_MASK; // use timer io pin as PWM
    // CAPT_MODE = Don't care because it's off
    timer_tclr_cfg |= TIMER_TCLR_PT_MASK;       // set POTIMERPWM pin to toggle (instead of pulse)
    timer_tclr_cfg |= TIMER_TCLR_TRG_TRG_VALUE_0X2 << TIMER_TCLR_TRG_SHIFT;  // Set output to trigger on compare & overflow
    timer_tclr_cfg &= ~TIMER_TCLR_TCM_MASK;     // Turn off capture mode
    timer_tclr_cfg &= ~TIMER_TCLR_SCPWM_MASK;   // Set initial output to 0
    timer_tclr_cfg |= TIMER_TCLR_CE_MASK;       // enable compare
    timer_tclr_cfg &= ~TIMER_TCLR_PRE_MASK;     // disable prescaler because we don't need it
    timer_tclr_cfg |= TIMER_TCLR_AR_MASK;       // Enable auto-reload mode
    timer_tclr_cfg &= ~TIMER_TCLR_PTV_MASK;     // (Unused: Set prescalar divider to 0)
    timer_tclr_cfg &= ~TIMER_TCLR_ST_MASK;      // Keep the timer off
    CSL_REG32_WR((volatile uint32_t *)(baseAddr + TIMER_TCLR), timer_tclr_cfg);

    // Disable interrupts
    //TIMERIntDisable(baseAddr, TIMER_WAKE_TCAR_FLAG|TIMER_WAKE_OVF_FLAG|TIMER_WAKE_MAT_FLAG);
    //TIMERWakeDisable(baseAddr, TIMER_WAKE_TCAR_FLAG|TIMER_WAKE_OVF_FLAG|TIMER_WAKE_MAT_FLAG);

    TIMERCounterSet(baseAddr, 0xFFFFFFFFU-1-(offset*resolution));
    TIMERReloadSet(baseAddr, 0xFFFFFFFFU-(period*resolution));
    TIMERCompareSet(baseAddr, 0xFFFFFFFFU-(period*resolution)+(duration*resolution));

    // Timer is NOT started - must be started after initialization
}

int32_t appD3board_initHwTimer(AppD3board_HwTimerObj *obj, AppD3board_HwTimerParams *params)
{
    int32_t status = 0;
    uint32_t fsyncTimer = 1;
    uint32_t laserTimer = 0;
    obj->fSyncPeriod = params->fSyncPeriod;
    obj->strobeLightOffset = params->strobeLightOffset;
    obj->strobeLightDuration = params->strobeLightDuration;
    obj->fSyncDuration = params->fSyncDuration;
    obj->timerResolution = params->timerResolution;

    // SET UP CONFIGURATIONS
    appLogPrintf("Configuring hardware timers");

    // Drive selected timer IO with selected timer output
    // FSYNC outputs to timerIO 2 and 3
    // Laser outputs to TP6 (timerIO 5)
    CSL_REG32_WR((volatile uint32_t *) CTRLMMR_TIMERION_CTRL(2), fsyncTimer);
    CSL_REG32_WR((volatile uint32_t *) CTRLMMR_TIMERION_CTRL(3), fsyncTimer);
    // CSL_REG32_WR((volatile uint32_t *) CTRLMMR_TIMERION_CTRL(5), laserTimer);

    appD3board_cfgHwTimer(fsyncTimer, obj->fSyncPeriod, obj->fSyncDuration, 0, obj->timerResolution);
    appD3board_cfgHwTimer(laserTimer, obj->fSyncPeriod, obj->strobeLightDuration, obj->strobeLightOffset, obj->timerResolution);

    // Turn on the timers
    TIMEREnable(TIMERN_CFG_BASE(0));
    TIMEREnable(TIMERN_CFG_BASE(1));

    return status;
}

int32_t appD3board_enableLaserOutput(AppD3board_HwTimerObj *obj)
{
    int32_t status = 0;
    uint32_t laserTimer = 0;

    CSL_REG32_WR((volatile uint32_t *) CTRLMMR_TIMERION_CTRL(5), laserTimer);

    return status;
}

int32_t appD3board_disableLaserOutput(AppD3board_HwTimerObj *obj)
{
    int32_t status = 0;

    CSL_REG32_WR((volatile uint32_t *) CTRLMMR_TIMERION_CTRL(5), 0x0A);

    return status;    
}

int32_t appD3board_deInitHwTimer(AppD3board_HwTimerObj *obj)
{
    int32_t status = 0;

    TIMERReset(TIMERN_CFG_BASE(0));
    TIMERReset(TIMERN_CFG_BASE(1));
    TIMERDisable(TIMERN_CFG_BASE(0));
    TIMERDisable(TIMERN_CFG_BASE(1));

    return status;
}
