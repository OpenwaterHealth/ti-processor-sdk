/*
 *  Copyright (c) Texas Instruments Incorporated 2020
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
 */

/**
 *  \file csitx_event.c
 *
 *  \brief File containing the CSITX driver event management functions.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/drv/csitx/src/csitx_drvPriv.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
/**
 *  \brief Enable the event in event group
 *
 *  \param instObj      CSITX instance object pointer
 *  \param eventGroup   Event group in which desired event is to be enabled.
 *  \param eventType    Event/s to be enabled.
 *
 *  \return FVID2_SOK on success, error otherwise
 */
static int32_t CsitxDrv_eventEnable(CsitxDrv_InstObj *instObj,
                                    uint32_t eventGroup,
                                    uint32_t eventType);

/**
 *  \brief Disable the event in event group
 *
 *  \param instObj      CSITX instance object pointer
 *  \param eventGroup   Event group in which desired event is to be enabled.
 *  \param eventType    Event/s to be enabled.
 *
 *  \return FVID2_SOK on success, error otherwise
 */
static int32_t CsitxDrv_eventDisable(CsitxDrv_InstObj *instObj,
                                     uint32_t eventGroup,
                                     uint32_t eventType);

static void CsitxDrv_txEventIsrFxn(uintptr_t arg);
static int32_t CsitxDrv_eventCheckParams(const Csitx_EventPrms *eventPrms);
static int32_t CsitxDrv_eventGroupAllocResource(CsitxDrv_InstObj *instObj,
                                                Csitx_EventPrms *eventPrms);
static int32_t CsitxDrv_resetModule(CsitxDrv_InstObj *instObj);
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
void Csitx_eventPrmsInit(Csitx_EventPrms *eventPrms)
{
    if (eventPrms != NULL)
    {
        eventPrms->eventGroup           = CSITX_EVENT_GROUP_TX_IRQ;
        eventPrms->eventMasks           = CSITX_EVENT_TYPE_TX_IRQ_ALL;
        eventPrms->eventCb              = NULL;
        eventPrms->coreIntrNum          = CSITX_INTR_INVALID;
        eventPrms->intrPriority         = 0U;
        eventPrms->appData              = NULL;
    }
}

int32_t CsitxDrv_eventGroupRegister(CsitxDrv_InstObj *instObj,
                                    Csitx_EventPrms *eventPrms)
{
    int32_t retVal = FVID2_SOK;
    CsitxDrv_EventObj *eventObj;
    OsalRegisterIntrParams_t intrPrms;

    /* Error Check: Parameters */
    GT_assert(CsitxTrace, (instObj != NULL_PTR));
    GT_assert(CsitxTrace, (eventPrms != NULL_PTR));
    if (retVal == FVID2_SOK)
    {
        if (instObj->inUse != CSITX_DRV_USAGE_STATUS_IN_USE)
        {
            retVal = FVID2_EFAIL;
        }
    }
    if (instObj->lockSem != NULL)
    {
        /* Pend on the instance semaphore */
        (void)SemaphoreP_pend(instObj->lockSem, SemaphoreP_WAIT_FOREVER);
    }
    if (retVal == FVID2_SOK)
    {
        retVal = CsitxDrv_eventCheckParams(eventPrms);
    }

    if (retVal == FVID2_SOK)
    {
        eventObj = &instObj->eventObj[eventPrms->eventGroup];
        /* Copy event configuration parameters to driver object */
        switch (eventPrms->eventGroup)
        {
            case CSITX_EVENT_GROUP_TX_IRQ:
                Fvid2Utils_memcpy(&eventObj->eventPrms, eventPrms, sizeof(eventPrms));
            break;
            default:
            break;
        }
        /* Copy driver handle to event object */
        eventObj->drvHandle = instObj;
        /* Allocate resource to the event group */
        retVal = CsitxDrv_eventGroupAllocResource(instObj, eventPrms);
    }
    if (retVal == FVID2_SOK)
    {
        /* Enable respective events */
        retVal = CsitxDrv_eventEnable(instObj,
                                      eventPrms->eventGroup,
                                      eventPrms->eventMasks);
    }
    if (retVal == FVID2_SOK)
    {
        /* Register interrupt */
        Osal_RegisterInterrupt_initParams(&intrPrms);
        /* Populate the interrupt parameters */
        intrPrms.corepacConfig.arg              = (uintptr_t) eventObj;
        intrPrms.corepacConfig.isrRoutine       = CsitxDrv_txEventIsrFxn;
        intrPrms.corepacConfig.priority         = eventPrms->intrPriority;
        intrPrms.corepacConfig.corepacEventNum  = 0;
        intrPrms.corepacConfig.intVecNum        = (int32_t)eventObj->coreIntrNum;
        if (Osal_RegisterInterrupt(&intrPrms, &eventObj->hwiHandle) !=
                                                        OSAL_INT_SUCCESS)
        {
            eventObj->hwiHandle = NULL;
            retVal = FVID2_EFAIL;
        }

    }
    if (retVal == FVID2_SOK)
    {
        eventObj->eventInitDone = 1U;
    }
    if (instObj->lockSem != NULL)
    {
        /* Post the instance semaphore */
        (void)SemaphoreP_post(instObj->lockSem);
    }

    return retVal;
}

static int32_t CsitxDrv_eventCheckParams(const Csitx_EventPrms *eventPrms)
{
    int32_t retVal = FVID2_SOK;

    if ((eventPrms->eventMasks == 0U) &&
        (eventPrms->eventGroup >= CSITX_EVENT_GROUP_MAX))
    {
        retVal = FVID2_EBADARGS;
    }

    return retVal;
}

static int32_t CsitxDrv_eventGroupAllocResource(CsitxDrv_InstObj *instObj,
                                                Csitx_EventPrms *eventPrms)
{
    int32_t retVal = FVID2_SOK;

    eventPrms->coreIntrNum = CSITX_INTR_INVALID;
    switch (instObj->drvInstId)
    {
        case CSITX_INSTANCE_ID_0:
            switch (eventPrms->eventGroup)
            {
                case CSITX_EVENT_GROUP_TX_IRQ:
                    eventPrms->coreIntrNum = CSITX_CORE_INTR_NUM_MOD_0_TX_INTR;
                break;
                default:
                    retVal = FVID2_EBADARGS;
                break;
            }
        break;
        default:
            retVal = FVID2_EBADARGS;
        break;
    }
    instObj->eventObj[eventPrms->eventGroup].coreIntrNum =
                                                    eventPrms->coreIntrNum;

    return retVal;
}

static int32_t CsitxDrv_eventEnable(CsitxDrv_InstObj *instObj,
                                    uint32_t eventGroup,
                                    uint32_t eventType)
{
    int32_t retVal = FVID2_SOK;
    uint32_t regVal = 0U;

    switch (eventGroup)
    {
        case CSITX_EVENT_GROUP_TX_IRQ:
            /* Enable Interrupt */
            if (CSITX_SetInterruptsEnable(&instObj->cslObj.cslCfgData,
                                          (bool)TRUE) != CDN_EOK)
            {
                retVal = FVID2_EFAIL;
            }

            if (retVal == FVID2_SOK)
            {
                regVal = 0U;
                /* TODO: Direct register read/write cause current CSL-FL does
                   not support programming of this registers */
                regVal = CSL_REG32_RD(
                    (CSL_CSI_TX_IF0_VBUS2APB_WRAP_VBUSP_APB_CSI2TX_BASE +
                    CSL_CSITX_IRQ_MASK));
            }

            if (retVal == FVID2_SOK)
            {
                regVal |= eventType;
                CSL_REG32_WR(
                    (CSL_CSI_TX_IF0_VBUS2APB_WRAP_VBUSP_APB_CSI2TX_BASE +
                     CSL_CSITX_IRQ_MASK),
                    regVal);
            }
        break;
        default:
            retVal = FVID2_EBADARGS;
        break;
    }

    return retVal;
}

static int32_t CsitxDrv_eventDisable(CsitxDrv_InstObj *instObj,
                                     uint32_t eventGroup,
                                     uint32_t eventType)
{
    int32_t retVal = FVID2_SOK;
    uint32_t regVal;

    switch (eventGroup)
    {
        case CSITX_EVENT_GROUP_TX_IRQ:
            /* Enable Interrupt */
            if (CSITX_SetInterruptsEnable(&instObj->cslObj.cslCfgData,
                                          (bool)FALSE) != CDN_EOK)
            {
                retVal = FVID2_EFAIL;
            }

            if (retVal == FVID2_SOK)
            {
                regVal = 0U;
                /* TODO: Direct register read/write cause current CSL-FL does
                   not support programming of this registers */
                regVal = CSL_REG32_RD(
                    (CSL_CSI_TX_IF0_VBUS2APB_WRAP_VBUSP_APB_CSI2TX_BASE +
                    CSL_CSITX_IRQ_MASK));
            }

            if (retVal == FVID2_SOK)
            {
                regVal &= (~eventType);
                CSL_REG32_WR(
                    (CSL_CSI_TX_IF0_VBUS2APB_WRAP_VBUSP_APB_CSI2TX_BASE +
                     CSL_CSITX_IRQ_MASK),
                    regVal);
            }
        break;
        default:
            retVal = FVID2_EBADARGS;
        break;
    }

    return retVal;
}

static void CsitxDrv_txEventIsrFxn(uintptr_t arg)
{
    CsitxDrv_InstObj *instObj;
    CsitxDrv_EventObj *eventObj;
    Csitx_EventStatus status;
    uint32_t cookie, intrStatus, issueCb = 0U;

    eventObj = (CsitxDrv_EventObj *)arg;
    instObj = (CsitxDrv_InstObj *)eventObj->drvHandle;
    /* Disable HW interrupts here */
    status.eventGroup = CSITX_EVENT_GROUP_TX_IRQ;
    status.drvHandle  = eventObj->drvHandle;
    cookie = HwiP_disable();
    /* TODO: Direct register read/write as current CSL-FL does not support this */
    /* Get the interrupt status */
    intrStatus = CSL_REG32_RD(
                    (CSL_CSI_TX_IF0_VBUS2APB_WRAP_VBUSP_APB_CSI2TX_BASE +
                    CSL_CSITX_IRQ));
    /* Clear Event status */
    CSL_REG32_WR((CSL_CSI_TX_IF0_VBUS2APB_WRAP_VBUSP_APB_CSI2TX_BASE +
                  CSL_CSITX_IRQ),
                  intrStatus);
    /* Get status for enabled interrupt only */
    intrStatus &= CSL_REG32_RD(
                    (CSL_CSI_TX_IF0_VBUS2APB_WRAP_VBUSP_APB_CSI2TX_BASE +
                    CSL_CSITX_IRQ_MASK));
    status.eventMasks = intrStatus;
    /* Service pending events */
    if (((intrStatus & CSITX_EVENT_TYPE_TX_IRQ_STRM_0_FRAME_START) ==
                      CSITX_EVENT_TYPE_TX_IRQ_STRM_0_FRAME_START) ||
        ((intrStatus & CSITX_EVENT_TYPE_TX_IRQ_STRM_0_FRAME_END) ==
                      CSITX_EVENT_TYPE_TX_IRQ_STRM_0_FRAME_END) ||
        ((intrStatus & CSITX_EVENT_TYPE_TX_IRQ_STRM_0_LINE_START) ==
                      CSITX_EVENT_TYPE_TX_IRQ_STRM_0_LINE_START) ||
        ((intrStatus & CSITX_EVENT_TYPE_TX_IRQ_STRM_0_LINE_END) ==
                      CSITX_EVENT_TYPE_TX_IRQ_STRM_0_LINE_END))
    {
        /* Nothing needs to be done here, just issue a Call-back */
        issueCb = 1U;
    }
    if (((intrStatus & CSITX_EVENT_TYPE_TX_IRQ_STRM_0_FIFO_UNDERFLOW) ==
                      CSITX_EVENT_TYPE_TX_IRQ_STRM_0_FIFO_UNDERFLOW) ||
        ((intrStatus & CSITX_EVENT_TYPE_TX_IRQ_STRM_0_DATA_FLOW_ERR) ==
                      CSITX_EVENT_TYPE_TX_IRQ_STRM_0_DATA_FLOW_ERR) ||
        ((intrStatus & CSITX_EVENT_TYPE_TX_IRQ_STRM_0_BYTE_CNT_MISMATCH) ==
                      CSITX_EVENT_TYPE_TX_IRQ_STRM_0_BYTE_CNT_MISMATCH) ||
        ((intrStatus & CSITX_EVENT_TYPE_TX_IRQ_STRM_0_LINE_NUMBER_ERR) ==
                      CSITX_EVENT_TYPE_TX_IRQ_STRM_0_LINE_NUMBER_ERR))
    {
        /* These are error interrupts, module needs soft reset in order to recover*/
        /* reset the controller stream */
        /* Reset streams */
        GT_assert(CsitxTrace,
                  (CsitxDrv_resetModule(instObj) == FVID2_SOK));
        /* Issue Call-back */
        issueCb = 1U;
    }

    /* Enable HW interrupts here */
    HwiP_restore(cookie);

    if ((eventObj->eventPrms.eventCb != NULL) && (issueCb == 1U))
    {
        /* Issue a CB to application */
        eventObj->eventPrms.eventCb(status, eventObj->eventPrms.appData);
    }
}

int32_t CsitxDrv_eventGroupUnRegister(CsitxDrv_InstObj *instObj,
                                      uint32_t eventGroup)
{
    int32_t retVal = FVID2_SOK;
    CsitxDrv_EventObj *eventObj;

    /* Error Check: Parameters */
    GT_assert(CsitxTrace, (instObj != NULL_PTR));
    if (instObj->inUse != CSITX_DRV_USAGE_STATUS_IN_USE)
    {
        retVal = FVID2_EFAIL;
    }
    if (instObj->lockSem != NULL)
    {
        /* Pend on the instance semaphore */
        (void)SemaphoreP_pend(instObj->lockSem, SemaphoreP_WAIT_FOREVER);
    }
    if (retVal == FVID2_SOK)
    {
        /* Disable All HW events for the given group first */
        retVal = CsitxDrv_eventDisable(instObj,
                                       eventGroup,
                                       CSITX_EVENT_TYPE_TX_IRQ_ALL);
    }
    if (retVal == FVID2_SOK)
    {
        eventObj = &instObj->eventObj[eventGroup];
        /* Disable Core interrupts */
        /* Delete interrupts: TODO: 0U in following call should be core based */
        (void) Osal_DeleteInterrupt((HwiP_Handle) eventObj->hwiHandle, 0);
        eventObj = &instObj->eventObj[eventGroup];
        eventObj->coreIntrNum   = CSITX_INTR_INVALID;
        eventObj->eventInitDone = 0U;
        eventObj->hwiHandle     = NULL;
    }
    if (instObj->lockSem != NULL)
    {
        /* Post the instance semaphore */
        (void)SemaphoreP_post(instObj->lockSem);
    }

    return retVal;
}

static int32_t CsitxDrv_resetModule(CsitxDrv_InstObj *instObj)
{
    int32_t retVal = FVID2_SOK;
    uint32_t regVal = 0U, currTimeout = 0U, waitFlag = 1U;

    /* Reset module as stream level reset is not available */
    GT_assert(CsitxTrace,
              (CSITX_SetSoftResetRequest(&instObj->cslObj.cslCfgData,
                                         (bool)TRUE) == CDN_EOK));
    /* Wait till reset goes through */
    currTimeout = 0U;
    waitFlag = 1U;
    while ((waitFlag == 1U) &&
           (retVal == FVID2_SOK))
    {
        GT_assert(CsitxTrace,
                 (CSITX_GetStatusRegister(&instObj->cslObj.cslCfgData,
                                          &regVal) == CDN_EOK));
        if ((regVal & CSL_CSITX_STATUS_SOFT_RESET_ACTIVE_MASK) ==
                      CSL_CSITX_STATUS_SOFT_RESET_ACTIVE_MASK)
        {
            waitFlag = 0U;
        }
        if (currTimeout > CSITX_TIMEOUT_VAL_MS)
        {
            retVal = FVID2_ETIMEOUT;
        }
        else
        {
            (void)Osal_delay(1U);
            currTimeout++;
        }
    }

    return retVal;
}
