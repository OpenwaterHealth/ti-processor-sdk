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
 *  ======== MailboxP_tirtos.c ========
 */

#include <ti/osal/MailboxP.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <ti/osal/src/tirtos/tirtos_config.h>

void MailboxP_Params_init(MailboxP_Params *params)
{
    if (params != NULL_PTR)
    {
        params->pErrBlk =  NULL_PTR;
        params->name    =  NULL_PTR;
        params->size    =  0U;
        params->count   =  0U;
        params->bufsize =  0U;
        params->buf     =  NULL_PTR;
    }
    return;
}

MailboxP_Handle MailboxP_create(const MailboxP_Params *params)
{
    Mailbox_Handle mboxHandle = (Mailbox_Handle) NULL_PTR;
    Mailbox_Params mboxParams;
    Error_Block  *pErrBlk = (Error_Block *) NULL_PTR;

    Mailbox_Params_init(&mboxParams);

    if (params != NULL_PTR)
    {
        mboxParams.instance->name = (char *)(params->name);
        pErrBlk = (Error_Block *) params->pErrBlk;

        if((params->buf != NULL_PTR) &&
           (params->bufsize >= (params->count * (params->size + sizeof(Mailbox_MbxElem)))))
        {
            mboxParams.buf = (Ptr)params->buf;
        }

        if (pErrBlk !=  NULL_PTR)
        {
            Error_init(pErrBlk);
        }

        mboxHandle = Mailbox_create(params->size, params->count, &mboxParams, pErrBlk);
    }

    return ((MailboxP_Handle)mboxHandle);
}

MailboxP_Status MailboxP_delete(MailboxP_Handle handle)
{
    Mailbox_delete((Mailbox_Handle *)&handle);
    
    return MailboxP_OK;
}

MailboxP_Status MailboxP_post(MailboxP_Handle handle,
                              void * msg,
                              uint32_t timeout)
{
    Bool flag;
    MailboxP_Status ret_val;

    flag = Mailbox_post((Mailbox_Handle)handle, msg, timeout);
    if (FALSE == flag) {
        ret_val = MailboxP_TIMEOUT;
    }
    else
    {
        ret_val = MailboxP_OK;
    }

    return (ret_val);
}

MailboxP_Status MailboxP_pend(MailboxP_Handle handle,
                              void * msg,
                              uint32_t timeout)
{
    Bool flag;
    MailboxP_Status ret_val;

    flag = Mailbox_pend((Mailbox_Handle)handle, msg, timeout);
    if (FALSE == flag) {
        ret_val = MailboxP_TIMEOUT;
    }
    else
    {
        ret_val = MailboxP_OK;
    }

    return (ret_val);
}


int32_t MailboxP_getNumPendingMsgs(MailboxP_Handle handle)
{
    return Mailbox_getNumPendingMsgs((Mailbox_Handle)handle);
}

/* Nothing past this point */
