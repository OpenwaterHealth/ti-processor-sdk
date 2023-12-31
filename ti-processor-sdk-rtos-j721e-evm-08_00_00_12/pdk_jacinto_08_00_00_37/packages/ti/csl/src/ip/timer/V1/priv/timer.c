/*
 *  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
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
 *  \file  timer.c
 *
 *  \brief Timer APIs.
 *
 *   This file contains the device abstraction layer APIs for Timer.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <ti/csl/csl.h>
#include <ti/csl/cslr_timer.h>
#include <ti/csl/src/ip/timer/V1/timer.h>
#include <ti/csl/hw_types.h>

/* ========================================================================== */
/*                      Internal Function Declarations                        */
/* ========================================================================== */

/**
 * \brief   This function will check for write POSTED status
 *
 * \param   reg          Register whose status has to be checked
 *
 *    'reg' can take the following values \n
 *    TIMER_WRITE_POST_TCLR - Timer Control register \n
 *    TIMER_WRITE_POST_TCRR - Timer Counter register \n
 *    TIMER_WRITE_POST_TLDR - Timer Load register \n
 *    TIMER_WRITE_POST_TTGR - Timer Trigger register \n
 *    TIMER_WRITE_POST_TMAR - Timer Match register \n
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 *
 * \return  None.
 *
 **/
static inline int32_t TimerWaitForWrite(uint32_t reg, uint32_t baseAddr);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */
int32_t TIMEREnable(uint32_t baseAddr)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        /* Wait for previous write to complete */
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);
        if (retVal == CSL_PASS)
        {
            /* Start the timer */
            HW_WR_FIELD32(
                baseAddr + TIMER_TCLR,
                TIMER_TCLR_ST,
                TIMER_TCLR_ST_ST_VALUE_1);
        }
    }

    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERDisable(uint32_t baseAddr)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        /* Wait for previous write to complete */
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);
        if (retVal == CSL_PASS)
        {
            /* Stop the timer */
            HW_WR_FIELD32(
                baseAddr + TIMER_TCLR,
                TIMER_TCLR_ST,
                TIMER_TCLR_ST_ST_VALUE_0);
        }
    }
    return(retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERModeConfigure(uint32_t baseAddr, uint32_t timerMode)
{
    uint32_t regVal;
    int32_t  retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        if ( (timerMode == TIMER_ONESHOT_CMP_ENABLE)    ||
             (timerMode == (uint32_t) (TIMER_ONESHOT_NOCMP_ENABLE))  ||
             (timerMode == TIMER_AUTORLD_CMP_ENABLE)    ||
             (timerMode == TIMER_AUTORLD_NOCMP_ENABLE) )
        {
            /* Wait for previous write to complete */
            retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);

            if (retVal == CSL_PASS)
            {
                /* Clear the AR and CE field of TCLR */
                HW_WR_FIELD32(baseAddr + TIMER_TCLR,
                              TIMER_TCLR_AR,
                              TIMER_TCLR_AR_AR_VALUE_0);

                HW_WR_FIELD32(baseAddr + TIMER_TCLR,
                              TIMER_TCLR_CE,
                              TIMER_TCLR_CE_CE_VALUE_0);

                /* Wait for previous write to complete */
                retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);

                if (retVal == CSL_PASS)
                {
                    /* Set the timer mode in TCLR register */
                    regVal = HW_RD_REG32(baseAddr + TIMER_TCLR);

                    regVal |= (timerMode & (TIMER_TCLR_AR_MASK | TIMER_TCLR_CE_MASK));

                    HW_WR_REG32(baseAddr + TIMER_TCLR, regVal);
                }
            }
        }
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERModeConfigureGet(uint32_t baseAddr, uint32_t *pTimerMode)
{
    uint32_t regVal;
    int32_t  retVal = CSL_EBADARGS;
    uint32_t mask =  (TIMER_TCLR_AR_MASK | TIMER_TCLR_CE_MASK);

    if ((baseAddr   != (uint32_t)(0u)) &&
        (pTimerMode != NULL_PTR) )
    {

        /* read the timer mode in TCLR register */
        regVal = HW_RD_REG32(baseAddr + TIMER_TCLR);

        regVal &= mask;

        /*
        After the mask, the timer has to have one of the below 4 modes
            TIMER_ONESHOT_CMP_ENABLE
            TIMER_ONESHOT_NOCMP_ENABLE
            TIMER_AUTORLD_CMP_ENABLE
            TIMER_AUTORLD_NOCMP_ENABLE
        */
        *pTimerMode = regVal;
        retVal = CSL_PASS;
    }
    return (retVal);
}


/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERPreScalerClkEnable(uint32_t baseAddr, uint32_t ptv)
{
    uint32_t regVal;
    int32_t  retVal = CSL_EBADARGS;
    uint32_t ptvClkTimerMask, ptvClkTimerValue;
    uint32_t isPtvBad = ((uint32_t) (0U));

    /* check the input ptv value for premask set */
    ptvClkTimerValue = ptv & TIMER_TCLR_PRE_MASK;

    /* the pre mask needs to be always set for valid ptv */
    if (ptvClkTimerValue == TIMER_TCLR_PRE_MASK)
    {
        ptvClkTimerMask  = ~(TIMER_TCLR_PTV_MASK | TIMER_TCLR_PRE_MASK);
        ptvClkTimerValue = ptv & ptvClkTimerMask;
        /* if non zero value, then the arg is bad */
        if (ptvClkTimerValue != ((uint32_t)(0U)))
        {
             isPtvBad = ((uint32_t) (1U));
        }
    }
    else
    {
        /* ptvLoc has to be all zeros here, hence bad arg */
       isPtvBad = ((uint32_t) (1U));
    }

    if ((baseAddr         != (uint32_t)(0u)) &&
        (isPtvBad          == (uint32_t)(0u)))
    {
        /* Wait for previous write to complete */
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);

        if (retVal == CSL_PASS)
        {
            /* Clear the PTV field of TCLR */
            HW_WR_FIELD32(baseAddr + TIMER_TCLR, TIMER_TCLR_PTV, 0U);

            /* Wait for previous write to complete */
            retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);

            if (retVal == CSL_PASS)
            {
                /* Set the PTV field and enable the pre-scaler clock */
                regVal = HW_RD_REG32(baseAddr + TIMER_TCLR);

                regVal |= (ptv & (TIMER_TCLR_PTV_MASK | TIMER_TCLR_PRE_MASK));

                HW_WR_REG32(baseAddr + TIMER_TCLR, regVal);
            }
        }
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERPreScalerClkDisable(uint32_t baseAddr)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        /* Wait for previous write to complete */
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);

        if (retVal == CSL_PASS)
        {
            /* Disable Pre-scaler clock */
            HW_WR_FIELD32(baseAddr + TIMER_TCLR,
                          TIMER_TCLR_PRE,
                          TIMER_TCLR_PRE_PRE_VALUE_0);
        }
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERCounterSet(uint32_t baseAddr, uint32_t counter)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        /* Wait for previous write to complete */
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCRR, baseAddr);

        if (retVal == CSL_PASS)
        {
            /* Set the counter value */
            HW_WR_REG32(baseAddr + TIMER_TCRR, counter);
        }
    }
    return (retVal);
}

/* This is kept for backwards compatibility
 * is replaced by new TIMERCounterGet2 API */
uint32_t TIMERCounterGet(uint32_t baseAddr)
{
    /* Wait for previous write to complete */
    (void)TimerWaitForWrite(TIMER_WRITE_POST_TCRR, baseAddr);

    /* Read the counter value from TCRR */
    return (HW_RD_REG32(baseAddr + TIMER_TCRR));
}

/**
 * Requirement: REQ_TAG(PDK-6052)
 * Design: did_csl_dmtimer
 */

int32_t TIMERCounterGet2(uint32_t baseAddr, uint32_t *pCntrVal)
{
    int32_t     retVal = CSL_EBADARGS;

    if ((baseAddr    != (uint32_t) (0u)) ||
        (pCntrVal    != NULL_PTR))
    {
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCRR, baseAddr);
        if (retVal == CSL_PASS)
        {
            *pCntrVal = HW_RD_REG32(baseAddr + TIMER_TCRR);
        }
    }
    return (retVal);

}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERReloadSet(uint32_t baseAddr, uint32_t reload)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        /* Wait for previous write to complete */
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TLDR, baseAddr);

        if (retVal == CSL_PASS)
        {
            /* Load the register with the re-load value */
            HW_WR_REG32(baseAddr + TIMER_TLDR, reload);
        }
    }
    return (retVal);
}

/* This is kept for backwards compatibility
 * is replaced by new TIMERReloadGet2 API */
uint32_t TIMERReloadGet(uint32_t baseAddr)
{
    /* Wait for previous write to complete */
    (void) TimerWaitForWrite(TIMER_WRITE_POST_TLDR, baseAddr);

    /* Return the contents of TLDR */
    return (HW_RD_REG32(baseAddr + TIMER_TLDR));
}

/**
 * Requirement: REQ_TAG(PDK-6052)
 * Design: did_csl_dmtimer
 */

int32_t TIMERReloadGet2(uint32_t baseAddr, uint32_t *pReloadVal)
{
    int32_t     retVal = CSL_EBADARGS;

    if ((baseAddr    != (uint32_t) (0u)) ||
        (pReloadVal  != NULL_PTR))
    {
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TLDR, baseAddr);
        if (retVal == CSL_PASS)
        {
            *pReloadVal = HW_RD_REG32(baseAddr + TIMER_TLDR);
        }
    }
    return (retVal);
}



/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERGPOConfigureGet(uint32_t baseAddr, uint32_t *pGPOVal)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        /* Loading the Positive increment Value in TPIR register */
        if (pGPOVal != NULL_PTR)
        {
            /* Wait for previous write to complete */
            retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);

            if (retVal == CSL_PASS)
            {
                /* read  gpo field of TCLR */
                *pGPOVal = HW_RD_FIELD32(baseAddr + TIMER_TCLR,
                              TIMER_TCLR_GPO_CFG);
            }
        }
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERGPOConfigure(uint32_t baseAddr, uint32_t gpoCfg)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        if ((gpoCfg == ((uint32_t) (TIMER_GPO_CFG_0)))         ||
            (gpoCfg == ((uint32_t) (TIMER_GPO_CFG_1))))
        {
            /* Wait for previous write to complete */
            retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);

            if (retVal == CSL_PASS)
            {
                /* Clear the GPO_CFG field of TCLR */
                HW_WR_FIELD32(baseAddr + TIMER_TCLR,
                              TIMER_TCLR_GPO_CFG,
                              TIMER_TCLR_GPO_CFG_GPO_CFG_0);

                /* Wait for previous write to complete */
                retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);

                if (retVal == CSL_PASS)
                {
                    /* Write to the gpoCfg field of TCLR */
                    HW_WR_FIELD32(baseAddr + TIMER_TCLR,
                                  TIMER_TCLR_GPO_CFG,
                                  gpoCfg >> TIMER_TCLR_GPO_CFG_SHIFT);
                }
           }
        }
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERCompareSet(uint32_t baseAddr, uint32_t compareVal)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        /* Wait for previous write to complete */
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TMAR, baseAddr);

        if (retVal == CSL_PASS)
        {
            /* Write the compare value to TMAR */
            HW_WR_REG32(baseAddr + TIMER_TMAR, compareVal);
        }
    }
    return (retVal);
}

/* This is kept for backwards compatibility
 * is replaced by new TIMERCompareGet2 API */
uint32_t TIMERCompareGet(uint32_t baseAddr)
{
    /* Wait for previous write to complete */
    (void) TimerWaitForWrite(TIMER_WRITE_POST_TMAR, baseAddr);

    /* Return the TMAR value */
    return (HW_RD_REG32(baseAddr + TIMER_TMAR));
}

/**
 * Requirement: REQ_TAG(PDK-6052)
 * Design: did_csl_dmtimer
 */

int32_t TIMERCompareGet2(uint32_t baseAddr, uint32_t *pCmpVal)
{
    int32_t     retVal = CSL_EBADARGS;
    if ((baseAddr    != (uint32_t) (0u)) ||
        (pCmpVal     != NULL_PTR))
    {
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TMAR, baseAddr);
        if (retVal == CSL_PASS)
        {
            *pCmpVal = HW_RD_REG32(baseAddr + TIMER_TMAR);
        }
    }
    return (retVal);
}


/**
 * Requirement: REQ_TAG(PDK-6052)
 * Design: did_csl_dmtimer
 */

int32_t TIMERIntRawStatusSet(uint32_t baseAddr, uint32_t intFlags)
{
    int32_t retVal = CSL_EBADARGS;
    uint32_t flags = intFlags & (TIMER_IRQSTATUS_TCAR_IT_FLAG_MASK |
                                 TIMER_IRQSTATUS_OVF_IT_FLAG_MASK |
                                 TIMER_IRQSTATUS_MAT_IT_FLAG_MASK);

    if ( (baseAddr != (uint32_t)(0u)) &&
         (flags    != (uint32_t)(0u)))
    {
        /* Trigger the events in IRQSTATUS_RAW register */
        HW_WR_REG32(baseAddr + TIMER_IRQSTATUS_RAW, flags);
        retVal = CSL_PASS;
    }
    return (retVal);
}

/* This is kept for backwards compatibility
 * is replaced by new TIMERIntRawStatusGet2 API */

uint32_t TIMERIntRawStatusGet(uint32_t baseAddr)
{
    /* Return the status of IRQSTATUS_RAW register */
    return (HW_RD_REG32(baseAddr + TIMER_IRQSTATUS_RAW));
}

/**
 * Requirement: REQ_TAG(PDK-6052)
 * Design: did_csl_dmtimer
 */

int32_t TIMERIntRawStatusGet2(uint32_t baseAddr, uint32_t *pIntrRawStatus)
{
    int32_t     retVal = CSL_EBADARGS;

    if ((baseAddr           != (uint32_t) (0u)) ||
        (pIntrRawStatus     != NULL_PTR))
    {
        /* Return the status of IRQSTATUS_RAW register */
        *pIntrRawStatus = TIMERIntRawStatusGet(baseAddr);
        retVal = CSL_PASS;
    }
    return (retVal);
}


/* This is kept for backwards compatibility
 * is replaced by new TIMERIntStatusGet2 API */

uint32_t TIMERIntStatusGet(uint32_t baseAddr)
{
    /* Return the status of IRQSTATUS register */
    return (HW_RD_REG32(baseAddr + TIMER_IRQSTATUS));
}

/**
 * Requirement: REQ_TAG(PDK-6052)
 * Design: did_csl_dmtimer
 */

int32_t TIMERIntStatusGet2(uint32_t baseAddr, uint32_t *pIntStat)
{
    int32_t     retVal = CSL_EBADARGS;

    if ((baseAddr           != (uint32_t) (0u)) ||
        (pIntStat           != NULL_PTR))
    {

        /* Return the status of IRQSTATUS register */
        *pIntStat = TIMERIntStatusGet(baseAddr);
        retVal    = CSL_PASS;
    }

    return (retVal);
}


/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERIntStatusClear(uint32_t baseAddr, uint32_t intFlags)
{
    int32_t retVal = CSL_EBADARGS;
    uint32_t flags = intFlags & (TIMER_IRQSTATUS_TCAR_IT_FLAG_MASK |
                                 TIMER_IRQSTATUS_OVF_IT_FLAG_MASK |
                                 TIMER_IRQSTATUS_MAT_IT_FLAG_MASK);

    if ( (baseAddr != (uint32_t)(0u)) &&
         (flags    != (uint32_t)(0u)))
    {
        /* Clear the interrupt status from IRQSTATUS register */
        HW_WR_REG32(baseAddr + TIMER_IRQSTATUS, flags);
        retVal = CSL_PASS;
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERIntEnable(uint32_t baseAddr, uint32_t intFlags)
{
    int32_t retVal = CSL_EBADARGS;
    uint32_t flags = intFlags & (TIMER_INT_TCAR_EN_FLAG |
                                 TIMER_INT_OVF_EN_FLAG |
                                 TIMER_INT_MAT_EN_FLAG);

    if ( (baseAddr != (uint32_t)(0u)) &&
         (flags    != (uint32_t)(0u)))
    {
        /* Enable the Timer interrupts represented by intFlags */
        HW_WR_REG32(baseAddr + TIMER_IRQENABLE_SET, flags);
        retVal = CSL_PASS;
    }
    return(retVal);
}
/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERIntDisable(uint32_t baseAddr, uint32_t intFlags)
{
    int32_t retVal = CSL_EBADARGS;
    uint32_t flags = intFlags & (TIMER_INT_TCAR_EN_FLAG |
                                 TIMER_INT_OVF_EN_FLAG |
                                 TIMER_INT_MAT_EN_FLAG);

    if ( (baseAddr != (uint32_t)(0u)) &&
         (flags    != (uint32_t)(0u)))
    {
        /* Disable the Timer interrupts represented by intFlags */
        HW_WR_REG32(baseAddr + TIMER_IRQENABLE_CLR, flags);
        retVal = CSL_PASS;
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERWakeEnable(uint32_t baseAddr, uint32_t wakeFlags)
{
    uint32_t regVal;
    int32_t retVal = CSL_EBADARGS;
    uint32_t flags = wakeFlags & (TIMER_IRQWAKEEN_TCAR_WUP_ENA_MASK |
                                 TIMER_IRQWAKEEN_OVF_WUP_ENA_MASK |
                                 TIMER_IRQWAKEEN_MAT_WUP_ENA_MASK);

    if ( (baseAddr != (uint32_t)(0u)) &&
         (flags    != (uint32_t)(0u)))
    {
        regVal = HW_RD_REG32(baseAddr + TIMER_IRQWAKEEN);
        regVal |= flags;
        /* Enable the Timer Wakeup events represented by wakeFlags */
        HW_WR_REG32(baseAddr + TIMER_IRQWAKEEN, regVal);
        retVal = CSL_PASS;
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERWakeDisable(uint32_t baseAddr, uint32_t wakeFlags)
{
    uint32_t regVal, mask;
    int32_t retVal = CSL_EBADARGS;
    uint32_t flags = wakeFlags & (TIMER_IRQWAKEEN_TCAR_WUP_ENA_MASK |
                                 TIMER_IRQWAKEEN_OVF_WUP_ENA_MASK |
                                 TIMER_IRQWAKEEN_MAT_WUP_ENA_MASK);

    if ( (baseAddr != (uint32_t)(0u)) &&
         (flags    != (uint32_t)(0u)))
    {
        mask = ~(flags);
        regVal = HW_RD_REG32(baseAddr + TIMER_IRQWAKEEN) & mask;
        /* Disable the Timer Wakeup events represented by wakeFlags */
        HW_WR_REG32(baseAddr + TIMER_IRQWAKEEN, regVal);
        retVal = CSL_PASS;
    }
    return(retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERTriggerSet(uint32_t baseAddr)
{
    int32_t     retVal = CSL_EBADARGS;

    if ((baseAddr != (uint32_t)(0u)))
    {
        /* Wait for previous write to complete */
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TTGR, baseAddr);

        if (retVal == CSL_PASS)
        {
            /* Write a value to the register */
            HW_WR_REG32(baseAddr + TIMER_TTGR, TIMER_TTGR_TTGR_VALUE_MASK);
        }
    }
    return (retVal);
}

/* This is kept for backwards compatibility
 * is replaced by new TIMERIntEnableGet2 API */

uint32_t TIMERIntEnableGet(uint32_t baseAddr)
{
    /* Return the status of register IRQENABLE_SET */
    return (HW_RD_REG32(baseAddr + TIMER_IRQENABLE_SET));
}

/**
 * Requirement: REQ_TAG(PDK-6052)
 * Design: did_csl_dmtimer
 */

int32_t TIMERIntEnableGet2(uint32_t baseAddr, uint32_t *pIntEn)
{
    int32_t     retVal = CSL_EBADARGS;

    if ((baseAddr           != (uint32_t) (0u)) ||
        (pIntEn             != NULL_PTR))
    {
        /* Return the status of register IRQENABLE_SET */
        *pIntEn = TIMERIntEnableGet(baseAddr);
        retVal  = CSL_PASS;
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */
int32_t TIMERResetConfigure(uint32_t baseAddr, uint32_t rstOption)
{
    int32_t     retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        if ( (rstOption == TIMER_SFT_RESET_ENABLE) ||
             (rstOption == TIMER_SFT_RESET_DISABLE) )
        {
            /* Write the option sent by user to SFT field of TSICR */
            HW_WR_FIELD32(baseAddr + TIMER_TSICR,
                          TIMER_TSICR_SFT,
                          rstOption >> TIMER_TSICR_SFT_SHIFT);
            retVal = CSL_PASS;
        }
    }
    return(retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERResetConfigureGet(uint32_t baseAddr, uint32_t *pRstOption)
{
    int32_t     retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        if (pRstOption != NULL_PTR)
        {
            /* Read the SFT field of TSICR */
            *pRstOption = HW_RD_FIELD32(baseAddr + TIMER_TSICR,
                          TIMER_TSICR_SFT);
            retVal = CSL_PASS;
        }
    }
    return(retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERReset(uint32_t baseAddr)
{
    int32_t     retVal = CSL_EBADARGS;
    volatile    uint32_t  delay;

    if ((baseAddr != (uint32_t)(0u)))
    {
        /* Reset the Timer module */
        HW_WR_FIELD32(baseAddr + TIMER_TIOCP_CFG,
                      TIMER_TIOCP_CFG_SOFTRESET,
                      TIMER_TIOCP_CFG_SOFTRESET_SOFTRESET_VALUE_1);

        for (delay = ((uint32_t)1U); delay < ((uint32_t)101U); )
        {
           /* delay loop */
           delay += ((uint32_t)1U);
        }

        while (TIMER_TIOCP_CFG_SOFTRESET_SOFTRESET_VALUE_1 ==
               HW_RD_FIELD32(baseAddr + TIMER_TIOCP_CFG,
                             TIMER_TIOCP_CFG_SOFTRESET))
        {
            /* Do nothing - Busy wait until Reset complete */
        }

        retVal = CSL_PASS;
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERContextSave(uint32_t baseAddr, TIMERCONTEXT *pContextPtr)
{
    int32_t     retVal = CSL_EBADARGS;

    if ((baseAddr != (uint32_t)(0u)) &&
        (pContextPtr != NULL_PTR))
    {
        pContextPtr->tldr         = HW_RD_REG32(baseAddr + TIMER_TLDR);
        pContextPtr->tmar         = HW_RD_REG32(baseAddr + TIMER_TMAR);
        pContextPtr->irqenableset = HW_RD_REG32(baseAddr + TIMER_IRQENABLE_SET);

        /* Wait for previous write to complete */
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCRR, baseAddr);

        if (retVal == CSL_PASS)
        {
            pContextPtr->tcrr = HW_RD_REG32(baseAddr + TIMER_TCRR);
            pContextPtr->tclr = HW_RD_REG32(baseAddr + TIMER_TCLR);
        }
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERContextRestore(uint32_t baseAddr, const TIMERCONTEXT *pContextPtr)
{
    int32_t     retVal = CSL_EBADARGS;

    if ((baseAddr != (uint32_t)(0u)) &&
        (pContextPtr != NULL_PTR))
    {
        /* Wait for previous write to complete */
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TLDR, baseAddr);

        if (retVal == CSL_PASS)
        {
            HW_WR_REG32(baseAddr + TIMER_TLDR, pContextPtr->tldr);

            /* Wait for previous write to complete */
            retVal = TimerWaitForWrite(TIMER_WRITE_POST_TMAR, baseAddr);

            if (retVal == CSL_PASS)
            {
                HW_WR_REG32(baseAddr + TIMER_TMAR, pContextPtr->tmar);
                HW_WR_REG32(baseAddr + TIMER_IRQENABLE_SET, pContextPtr->irqenableset);

                /* Wait for previous write to complete */
                retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCRR, baseAddr);

                if (retVal == CSL_PASS)
                {
                    HW_WR_REG32(baseAddr + TIMER_TCRR, pContextPtr->tcrr);

                    /* Wait for previous write to complete */
                    retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);

                    if (retVal == CSL_PASS)
                    {
                        HW_WR_REG32(baseAddr + TIMER_TCLR, pContextPtr->tclr);
                    }
                }
            }
        }
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERPostedModeConfig(uint32_t baseAddr, uint32_t postMode)
{
    int32_t     retVal = CSL_EBADARGS;

    if ((baseAddr != (uint32_t)(0u)))
    {
        if ( (postMode == TIMER_POSTED) ||
             (postMode == TIMER_NONPOSTED) )
        {
        /* Write to the POSTED field of TSICR */
        HW_WR_FIELD32(baseAddr + TIMER_TSICR,
                      TIMER_TSICR_POSTED,
                      postMode >> TIMER_TSICR_POSTED_SHIFT);
        retVal = CSL_PASS;
        }
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERPostedModeConfigGet(uint32_t baseAddr, uint32_t *pPostMode)
{
    int32_t     retVal = CSL_EBADARGS;
    uint32_t    postMode;

    if ((baseAddr != (uint32_t)(0u)) &&
        (pPostMode != NULL_PTR) )
    {
        /* Read to the POSTED field of TSICR */
        postMode = HW_RD_FIELD32(baseAddr + TIMER_TSICR,
                      TIMER_TSICR_POSTED);
        postMode = postMode << TIMER_TSICR_POSTED_SHIFT;
        *pPostMode = postMode;
        retVal = CSL_PASS;
   }
    return (retVal);
}


/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERReadModeConfig(uint32_t baseAddr, uint32_t readMode)
{
    int32_t     retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        if ( (readMode == TIMER_READ_MODE_POSTED) ||
             (readMode == TIMER_READ_MODE_NONPOSTED) )
        {
            /* Write to the READ_MODE field of TSICR */
            HW_WR_FIELD32(baseAddr + TIMER_TSICR,
                          TIMER_TSICR_READ_MODE,
                          readMode >> TIMER_TSICR_READ_MODE_SHIFT);
            retVal = CSL_PASS;
        }
    }
    return (retVal);
}


/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERReadModeConfigGet(uint32_t baseAddr, uint32_t *pReadMode)
{
    int32_t     retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        if (pReadMode != NULL_PTR)
        {
            /* Read to the READ_MODE field of TSICR */
            *pReadMode = HW_RD_FIELD32(baseAddr + TIMER_TSICR,
                          TIMER_TSICR_READ_MODE);
            retVal = CSL_PASS;
        }
    }
    return (retVal);
}


/* This is kept for backwards compatibility
 * is replaced by new TIMERWritePostedStatusGet2 API */

uint32_t TIMERWritePostedStatusGet(uint32_t baseAddr)
{
    /* Return the status of TWPS register */
    return (HW_RD_REG32(baseAddr + TIMER_TWPS));
}

/**
 * Requirement: REQ_TAG(PDK-6052)
 * Design: did_csl_dmtimer
 */

int32_t TIMERWritePostedStatusGet2(uint32_t baseAddr, uint32_t *pPostedStat)
{
    int32_t     retVal = CSL_EBADARGS;

    if ((baseAddr           != (uint32_t) (0u)) ||
        (pPostedStat        != NULL_PTR))
    {
        /* Return the status of TWPS register */
        *pPostedStat = TIMERWritePostedStatusGet(baseAddr);
        retVal       = CSL_PASS;
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERIdleModeConfigure(uint32_t baseAddr, uint32_t idleModeOption)
{
    int32_t     retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        if ( (idleModeOption == TIMER_FORCE_IDLE) ||
             (idleModeOption == TIMER_NO_IDLE)    ||
             (idleModeOption == TIMER_SMART_IDLE) ||
             (idleModeOption == TIMER_SMART_IDLE_WAKEUP))
        {
            /* Write to the IDLEMODE field of TIOCP_CFG */
            HW_WR_FIELD32(baseAddr + TIMER_TIOCP_CFG,
                          TIMER_TIOCP_CFG_IDLEMODE,
                          idleModeOption);
            retVal = CSL_PASS;
        }
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERIdleModeConfigureGet(uint32_t baseAddr, uint32_t *pIdleMode)
{
    int32_t     retVal = CSL_EBADARGS;

    if ((baseAddr != (uint32_t)(0u)))
    {
        if (pIdleMode != NULL_PTR)
        {
        /* Read the IDLEMODE field of TIOCP_CFG */
        *pIdleMode = HW_RD_FIELD32(baseAddr + TIMER_TIOCP_CFG,
                      TIMER_TIOCP_CFG_IDLEMODE);
         retVal = CSL_PASS;
        }
    }
    return (retVal);
}


/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMEREmuModeConfigure(uint32_t baseAddr, uint32_t emuModeOption)
{
    int32_t     retVal = CSL_EBADARGS;

    if ((baseAddr != (uint32_t)(0u)))
    {
        /* Write to the EMUFREE field of TIOCP_CFG */
        HW_WR_FIELD32(baseAddr + TIMER_TIOCP_CFG,
                      TIMER_TIOCP_CFG_EMUFREE,
                      emuModeOption);
        retVal = CSL_PASS;
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMEREmuModeConfigureGet(uint32_t baseAddr, uint32_t *pEmuMode)
{
    int32_t     retVal = CSL_EBADARGS;

    if ((baseAddr != (uint32_t)(0u)))
    {
        if (pEmuMode != NULL_PTR)
        {
            /* Read to the EMUFREE field of TIOCP_CFG */
            *pEmuMode = HW_RD_FIELD32(baseAddr + TIMER_TIOCP_CFG,
                          TIMER_TIOCP_CFG_EMUFREE);
             retVal = CSL_PASS;
        }
    }
    return (retVal);
}


/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERTPIRSet(uint32_t baseAddr, uint32_t tpirVal)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        /* Wait for previous write to complete */
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TPIR, baseAddr);

        if (retVal == CSL_PASS)
        {
            /* Loading the Positive increment Value in TPIR register */
            HW_WR_REG32(baseAddr + CSL_DMTIMER1MS_TPIR, tpirVal);
        }
    }
    return (retVal);
}


/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERTPIRGet(uint32_t baseAddr, uint32_t *pTpirVal)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        /* Loading the Positive increment Value in TPIR register */
        if (pTpirVal != NULL_PTR)
        {
            /* Wait for previous write to complete */
            retVal = TimerWaitForWrite(TIMER_WRITE_POST_TPIR, baseAddr);

            if (retVal == CSL_PASS)
            {
                *pTpirVal = HW_RD_REG32(baseAddr + CSL_DMTIMER1MS_TPIR);
            }
        }
    }
    return (retVal);
}


/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERTNIRSet(uint32_t baseAddr, uint32_t tnirVal)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        /* Wait for previous write to complete */
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TNIR, baseAddr);

        if (retVal == CSL_PASS)
        {
            /* Loading the Negative increment Value in TNIR register */
            HW_WR_REG32(baseAddr + CSL_DMTIMER1MS_TNIR, tnirVal);
        }
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERTNIRGet(uint32_t baseAddr, uint32_t *pTNIR)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        /* Read the Negative increment Value in TPIR register */
        if (pTNIR != NULL_PTR)
        {
            /* Wait for previous write to complete */
            retVal = TimerWaitForWrite(TIMER_WRITE_POST_TNIR, baseAddr);

            if (retVal == CSL_PASS)
            {
                *pTNIR = HW_RD_REG32(baseAddr + CSL_DMTIMER1MS_TNIR);
            }
        }
    }
    return (retVal);
}



/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERTCVRSet(uint32_t baseAddr, uint32_t val)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {

        /* Wait for previous write to complete */
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCVR, baseAddr);

        if (retVal == CSL_PASS)
        {
            /*
            ** Loading the value to decide the next TCRR value will be sub period or
            ** the over period value.
            */
            HW_WR_REG32(baseAddr + CSL_DMTIMER1MS_TCVR, val);
        }
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERTCVRGet(uint32_t baseAddr, uint32_t *pTCVR)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        /* Read TCVR register */
        if (pTCVR != NULL_PTR)
        {
            /* Wait for previous write to complete */
            retVal = TimerWaitForWrite(TIMER_WRITE_POST_TCVR, baseAddr);

            if (retVal == CSL_PASS)
            {
                /* read  tcvr field */
                *pTCVR = HW_RD_REG32(baseAddr + CSL_DMTIMER1MS_TCVR);
            }
        }
    }
    return (retVal);
}


/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERTOCRSet(uint32_t baseAddr, uint32_t val)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {

        /* Wait for previous write to complete */
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TOCR, baseAddr);

        if (retVal == CSL_PASS)
        {
            /* Loading the value to mask the tick interrupt for 'val' no of ticks */
            HW_WR_FIELD32(baseAddr + CSL_DMTIMER1MS_TOCR,
                          CSL_DMTIMER1MS_TOCR_OVF_COUNTER_VALUE,
                          val);
        }
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERTOCRGet(uint32_t baseAddr, uint32_t *pTOCR)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        if (pTOCR != NULL_PTR)
        {
            /* Wait for previous write to complete */
            retVal = TimerWaitForWrite(TIMER_WRITE_POST_TOCR, baseAddr);

            if (retVal == CSL_PASS)
            {
                /* Read TOCR value */
                *pTOCR = HW_RD_FIELD32(baseAddr + CSL_DMTIMER1MS_TOCR,
                              CSL_DMTIMER1MS_TOCR_OVF_COUNTER_VALUE);
            }
        }
    }
    return (retVal);
}


/* This is kept for backwards compatibility
 * is replaced by new TIMERTOWRGet2 API */

uint32_t TIMERTOWRGet(uint32_t baseAddr)
{

    /* Wait for previous write to complete */
    (void) TimerWaitForWrite(TIMER_WRITE_POST_TOWR, baseAddr);

    /* Gets the number of masked overflow interrupts. */
    return (HW_RD_FIELD32(baseAddr + CSL_DMTIMER1MS_TOWR,
            CSL_DMTIMER1MS_TOWR_OVF_WRAPPING_VALUE));
}

/**
 * Requirement: REQ_TAG(PDK-6052)
 * Design: did_csl_dmtimer
 */

int32_t TIMERTOWRGet2(uint32_t baseAddr, uint32_t *pTOWRVal)
{
    int32_t     retVal = CSL_EBADARGS;

    if ((baseAddr           != (uint32_t) (0u)) ||
        (pTOWRVal           != NULL_PTR))
    {
        retVal    = TimerWaitForWrite(TIMER_WRITE_POST_TOWR, baseAddr);
        if (retVal == CSL_PASS)
        {
            *pTOWRVal = HW_RD_FIELD32(baseAddr + CSL_DMTIMER1MS_TOWR,
                                      CSL_DMTIMER1MS_TOWR_OVF_WRAPPING_VALUE);
        }
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6041)
 * Design: did_csl_dmtimer
 */

int32_t TIMERTOWRSet(uint32_t baseAddr, uint32_t val)
{
    int32_t retVal = CSL_EBADARGS;

    if (baseAddr != (uint32_t)(0u))
    {
        /* Wait for previous write to complete */
        retVal = TimerWaitForWrite(TIMER_WRITE_POST_TOWR, baseAddr);

        if (retVal == CSL_PASS)
        {
            /* Setting the overflow count mask value in TOWR register*/
            HW_WR_FIELD32(baseAddr + CSL_DMTIMER1MS_TOWR,
                          CSL_DMTIMER1MS_TOWR_OVF_WRAPPING_VALUE,
                          val);
        }
    }
    return (retVal);
}

/**
 * Requirement: REQ_TAG(PDK-6024)
 * Design: did_csl_dmtimer
 */

int32_t TIMERGetStaticRegs(uint32_t baseAddr,TIMERSTATICREGS * pStaticRegs)
{
    int32_t     retVal = CSL_EBADARGS;
    uint32_t    tidrmask, tldrmask, tiocpmask, tmarmask;
    uint32_t    irqmask,  tclrmask, busyMask;
    TIMERSTATICREGS     *pLocPtr = pStaticRegs;

    /* Prepare the masks */
    tidrmask = TIMER_TIDR_REVISION_MASK;
    tldrmask = TIMER_TLDR_LOAD_VALUE_MASK;
    tiocpmask = ( TIMER_TIOCP_CFG_SOFTRESET_MASK |
                  TIMER_TIOCP_CFG_EMUFREE_MASK   |
                  TIMER_TIOCP_CFG_IDLEMODE_MASK);
    tmarmask  = (TIMER_TMAR_COMPARE_VALUE_MASK);
    irqmask   = (TIMER_IRQSTATUS_MAT_IT_FLAG_MASK |
                 TIMER_IRQSTATUS_OVF_IT_FLAG_MASK |
                 TIMER_IRQSTATUS_TCAR_IT_FLAG_MASK );
    tclrmask  = ~(TIMER_TCLR_RESERVED_MASK);

    busyMask =  (   TIMER_WRITE_POST_TLDR |
                    TIMER_WRITE_POST_TMAR |
                    TIMER_WRITE_POST_TCLR );

    if ( (baseAddr != (uint32_t) (0u)) &&
         (pLocPtr  != NULL_PTR))
    {
        /* Check any previous write pending */
        busyMask &= TIMERWritePostedStatusGet(baseAddr);

        if (busyMask != (uint32_t) (0u))
        {
            retVal = CSL_ETIMEOUT;
        }
        else
        {
            retVal = CSL_PASS;
        }

        if (retVal == CSL_PASS)
        {
            pLocPtr->tidr   = HW_RD_REG32(baseAddr + TIMER_TIDR) & tidrmask;
            pLocPtr->tldr   = HW_RD_REG32(baseAddr + TIMER_TLDR) & tldrmask;
            pLocPtr->tiocp  = HW_RD_REG32(baseAddr + TIMER_TIOCP_CFG) & tiocpmask;
            pLocPtr->tmar   = HW_RD_REG32(baseAddr + TIMER_TMAR) & tmarmask;
            pLocPtr->irqenableset   = HW_RD_REG32(baseAddr + TIMER_IRQENABLE_SET) & irqmask;
            pLocPtr->tclr   = HW_RD_REG32(baseAddr + TIMER_TCLR) & tclrmask;
        }
    }

    return (retVal);
}

/* ========================================================================== */
/*                      Internal Function Definitions                         */
/* ========================================================================== */
static inline int32_t TimerWaitForWrite(uint32_t reg, uint32_t baseAddr)
{
    int32_t          retVal = CSL_PASS;
    volatile uint32_t exit_count = (uint32_t) 0U;
    uint32_t step_size           = (uint32_t) 1U;

    if (0U != HW_RD_FIELD32(baseAddr + TIMER_TSICR, TIMER_TSICR_POSTED))
    {
        while ((uint32_t) 0U != (reg & TIMERWritePostedStatusGet(baseAddr)))
        {
             exit_count += step_size;
            /* Do nothing - Busy wait,
             * quit the loop if posted transations are not complete 
             * by one full cycle of counting
             * This check and break prevents getting stuck in the loop
             */
             if (exit_count == 0U)
             {
                retVal = CSL_ETIMEOUT;
                break;
             }
        }
    }
    return (retVal);
}
/********************************* End of file ******************************/
