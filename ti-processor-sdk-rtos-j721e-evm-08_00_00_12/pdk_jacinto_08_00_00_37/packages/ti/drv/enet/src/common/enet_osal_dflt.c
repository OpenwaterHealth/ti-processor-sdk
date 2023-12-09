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

/*!
 * \file  enet_osal_dflt.c
 *
 * \brief This file contains a default OSAL implementation.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <stdarg.h>
#include <ti/osal/osal.h>
#include <ti/drv/enet/include/common/enet_osal_dflt.h>

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

static uintptr_t EnetOsalDflt_disableAllIntr(void);

static void EnetOsalDflt_restoreAllIntr(uintptr_t cookie);

static void *EnetOsalDflt_registerIntr(EnetOsal_Isr isrFxn,
                                       uint32_t coreIntrNum,
                                       uint32_t intrPriority,
                                       uint32_t intrTrigType,
                                       void *arg);

static void EnetOsalDflt_unregisterIntr(void *hHwi);

static void EnetOsalDflt_enableIntr(uint32_t coreIntrNum);

static void EnetOsalDflt_disableIntr(uint32_t coreIntrNum);

static void *EnetOsalDflt_createMutex(void);

static void EnetOsalDflt_deleteMutex(void *hMutex);

static void EnetOsalDflt_lockMutex(void *hMutex);

static void EnetOsalDflt_unlockMutex(void *hMutex);

static bool EnetOsalDflt_isCacheCoherent(void);

static void EnetOsalDflt_cacheInv(const void *addr,
                                  int32_t size);

static void EnetOsalDflt_cacheWb(const void *addr,
                                 int32_t size);

static void EnetOsalDflt_cacheWbInv(const void *addr,
                                    int32_t size);

static uint32_t EnetOsalDflt_timerRead();

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void EnetOsalDflt_initCfg(EnetOsal_Cfg *cfg)
{
    cfg->disableAllIntr  = &EnetOsalDflt_disableAllIntr;;
    cfg->restoreAllIntr  = &EnetOsalDflt_restoreAllIntr;
    cfg->disableIntr     = &EnetOsalDflt_disableIntr;
    cfg->restoreIntr     = &EnetOsalDflt_enableIntr;
    cfg->registerIntr    = &EnetOsalDflt_registerIntr;
    cfg->unregisterIntr  = &EnetOsalDflt_unregisterIntr;
    cfg->createMutex     = &EnetOsalDflt_createMutex;
    cfg->deleteMutex     = &EnetOsalDflt_deleteMutex;
    cfg->lockMutex       = &EnetOsalDflt_lockMutex;
    cfg->unlockMutex     = &EnetOsalDflt_unlockMutex;
    cfg->isCacheCoherent = &EnetOsalDflt_isCacheCoherent;
    cfg->cacheInv        = &EnetOsalDflt_cacheInv;
    cfg->cacheWb         = &EnetOsalDflt_cacheWb;
    cfg->cacheWbInv      = &EnetOsalDflt_cacheWbInv;
    cfg->timerRead       = &EnetOsalDflt_timerRead;
}

static uintptr_t EnetOsalDflt_disableAllIntr(void)
{
    return HwiP_disable();
}

static void EnetOsalDflt_restoreAllIntr(uintptr_t cookie)
{
    HwiP_restore(cookie);
}

static void *EnetOsalDflt_registerIntr(EnetOsal_Isr isrFxn,
                                       uint32_t coreIntrNum,
                                       uint32_t intrPriority,
                                       uint32_t intrTrigType,
                                       void *arg)
{
    OsalRegisterIntrParams_t intrPrms;
    OsalInterruptRetCode_e status;
    HwiP_Handle hHwi = NULL;

    Osal_RegisterInterrupt_initParams(&intrPrms);

    /* Populate the interrupt parameters */
    intrPrms.corepacConfig.arg             = (uintptr_t)arg;
    intrPrms.corepacConfig.isrRoutine      = isrFxn;
    intrPrms.corepacConfig.priority        = intrPriority;
    intrPrms.corepacConfig.triggerSensitivity = intrTrigType;
    intrPrms.corepacConfig.corepacEventNum = 0U;
    intrPrms.corepacConfig.intVecNum       = coreIntrNum;

    /* Register interrupts */
    status = Osal_RegisterInterrupt(&intrPrms, &hHwi);
    if (status != OSAL_INT_SUCCESS)
    {
        hHwi = NULL;
    }

    return hHwi;
}

static void EnetOsalDflt_unregisterIntr(void *hHwi)
{
    int32_t corepacEventNum = 0U;

    /* Delete interrupts */
    Osal_DeleteInterrupt((HwiP_Handle)hHwi, corepacEventNum);
}

static void EnetOsalDflt_enableIntr(uint32_t coreIntrNum)
{
    int32_t corepacEventNum = 0U;

    Osal_EnableInterrupt(corepacEventNum, coreIntrNum);
}

static void EnetOsalDflt_disableIntr(uint32_t coreIntrNum)
{
    int32_t corepacEventNum = 0U;

    Osal_DisableInterrupt(corepacEventNum, coreIntrNum);
}

static void *EnetOsalDflt_createMutex(void)
{
    SemaphoreP_Params prms;
    SemaphoreP_Handle hMutex;

    SemaphoreP_Params_init(&prms);
    prms.mode = SemaphoreP_Mode_BINARY;
    hMutex = (void *)SemaphoreP_create(1U, &prms);

    return hMutex;
}

static void EnetOsalDflt_deleteMutex(void *hMutex)
{
    SemaphoreP_delete((SemaphoreP_Handle)hMutex);
}

static void EnetOsalDflt_lockMutex(void *hMutex)
{
    SemaphoreP_pend((SemaphoreP_Handle)hMutex, SemaphoreP_WAIT_FOREVER);
}

static void EnetOsalDflt_unlockMutex(void *hMutex)
{
    SemaphoreP_post((SemaphoreP_Handle)hMutex);
}

static bool EnetOsalDflt_isCacheCoherent(void)
{
    bool isCoherent = false;

#if defined(SOC_J721E) || defined(SOC_J7200) || defined(SOC_AM65XX)
#if defined(BUILD_MPU1_0)
    isCoherent = true;
#endif
#endif

    return isCoherent;
}

static void EnetOsalDflt_cacheInv(const void *addr,
                                  int32_t size)
{
    CacheP_Inv(addr, size);
}

static void EnetOsalDflt_cacheWb(const void *addr,
                                 int32_t size)
{
    CacheP_wb(addr, size);
}

static void EnetOsalDflt_cacheWbInv(const void *addr,
                                    int32_t size)
{
    CacheP_wbInv(addr, size);
}

static uint32_t EnetOsalDflt_timerRead()
{
    return 0;
}
