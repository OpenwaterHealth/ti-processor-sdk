/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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
/*
 *  ======== ClockP_tirtos.c ========
 */

#include <ti/osal/ClockP.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <ti/osal/src/tirtos/tirtos_config.h>


void ClockP_Params_init(ClockP_Params *params)
{
    if (params != NULL_PTR)
    {
        params->pErrBlk   = NULL_PTR;
        params->startMode = ClockP_StartMode_USER;
        params->period    = 0U;
        params->runMode   = ClockP_RunMode_ONESHOT;
        params->arg       = NULL_PTR;
    }
    return;
}

ClockP_Handle ClockP_create(void *clockfxn,
                            const ClockP_Params *params)
{
    Clock_Handle clockHandle = (Clock_Handle) NULL_PTR;
    Clock_Params clockParams;
    Error_Block  *pErrBlk = (Error_Block *) NULL_PTR;

    Clock_Params_init(&clockParams);


    if (params != NULL_PTR)
    {
        if(params->startMode == ClockP_StartMode_AUTO)
        {
            clockParams.startFlag = 1U;
        }
        if(params->runMode == ClockP_RunMode_CONTINUOUS)
        {
            clockParams.period = params->period;
        }
        clockParams.arg = (xdc_UArg)params->arg;
        pErrBlk = (Error_Block *) params->pErrBlk;

        if (pErrBlk !=  NULL_PTR)
        {
            Error_init(pErrBlk);
        }

        clockHandle = Clock_create((Clock_FuncPtr)clockfxn, params->period, &clockParams, pErrBlk);
    }

    return ((ClockP_Handle)clockHandle);
}

ClockP_Status ClockP_delete(ClockP_Handle handle)
{
    Clock_delete((Clock_Handle *)&handle);
    return ClockP_OK;
}

ClockP_Status ClockP_start(ClockP_Handle handle)
{
    Clock_start((Clock_Handle)handle);
    return ClockP_OK;
}

ClockP_Status ClockP_stop(ClockP_Handle handle)
{
    Clock_stop((Clock_Handle)handle);
    return ClockP_OK;
}

/* Nothing past this point */
