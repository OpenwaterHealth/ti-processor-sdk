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
 *  ======== MutexP_tirtos.c ========
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ti/osal/MutexP.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/gates/GateMutexPri.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <ti/osal/src/tirtos/tirtos_config.h>

extern void Osal_DebugP_assert(int32_t expression, const char *file, int32_t line);
#define MUTEXOSAL_Assert(expression) (Osal_DebugP_assert((int32_t)((expression)?1:0),\
                                                  __FILE__, __LINE__))

extern uint32_t  gOsalMutexAllocCnt, gOsalMutexPeak;

/*!
 *  @brief    Mutex structure
 */
typedef struct MutexP_tiRtos_s {
    Bool                 used;
    GateMutexPri_Struct  mutexStruct;
    GateMutexPri_Handle  mutexHandle;
} MutexP_tiRtos;

/* global pool of statically allocated mutex pools */
static MutexP_tiRtos gOsalMutexPTiRtosPool[OSAL_TIRTOS_CONFIGNUM_MUTEX];


MutexP_Handle MutexP_create(MutexP_Object *mutexObj)
{
    MutexP_Handle ret_handle = NULL_PTR;
    MutexP_tiRtos *handle = (MutexP_tiRtos *) NULL_PTR;
    GateMutexPri_Params mutexParams;
    uint32_t i;
    uintptr_t key;
    MutexP_tiRtos *mutexPool;
    uint32_t maxMutex;

    if (mutexObj != NULL_PTR)
    {
        /* Pick up the internal static memory block */
        mutexPool = (MutexP_tiRtos *) &gOsalMutexPTiRtosPool[0];
        maxMutex  = OSAL_TIRTOS_CONFIGNUM_MUTEX;

        if(gOsalMutexAllocCnt==0U) 
        {
            (void)memset((void *)gOsalMutexPTiRtosPool,0,sizeof(gOsalMutexPTiRtosPool));
        }

        key = HwiP_disable();

        for (i = 0; i < maxMutex; i++)
        {
            if (mutexPool[i].used == FALSE)
            {
                mutexPool[i].used = TRUE;
                /* Update statistics */
                gOsalMutexAllocCnt++;
                if (gOsalMutexAllocCnt > gOsalMutexPeak)
                {
                    gOsalMutexPeak = gOsalMutexAllocCnt;
                }
                break;
            }
        }
        HwiP_restore(key);

        if (i < maxMutex)
        {
            /* Grab the memory */
            handle = (MutexP_tiRtos *) &mutexPool[i];
        }

        if (handle == NULL_PTR) {
            ret_handle = NULL_PTR;
        }
        else
        {
            GateMutexPri_Params_init(&mutexParams);

            GateMutexPri_construct(&handle->mutexStruct, &mutexParams);
            handle->mutexHandle = GateMutexPri_handle(&handle->mutexStruct);

            mutexObj->object = (void *)handle;
            mutexObj->key = 0;
            ret_handle = (MutexP_Handle)mutexObj;
        }
    }

    return ret_handle;
}

MutexP_Status MutexP_delete(MutexP_Handle handle)
{

    uintptr_t   key;    
    MutexP_Status ret = MutexP_FAILURE;
    MutexP_Object *mutexObj;
    MutexP_tiRtos *mutex;

    if (handle != NULL_PTR)
    {
        mutexObj = (MutexP_Object *)handle;
        mutex = (MutexP_tiRtos *)mutexObj->object;

        if ((mutex != NULL_PTR) && (mutex->used == TRUE))
        {
            GateMutexPri_destruct(&mutex->mutexStruct);

            key = HwiP_disable();
            mutex->used = FALSE;
            /* Found the osal semaphore object to delete */
            if (gOsalMutexAllocCnt > 0U)
            {
                gOsalMutexAllocCnt--;
            }
            HwiP_restore(key);
            ret = MutexP_OK;
        }
    }
    return ret;
}

MutexP_Status MutexP_lock(MutexP_Handle handle,
                          uint32_t timeout)
{
    MutexP_Status ret = MutexP_FAILURE;
    MutexP_Object *mutexObj;
    MutexP_tiRtos *mutex;

    if (handle != NULL_PTR)
    {
        mutexObj = (MutexP_Object *)handle;
        mutex = (MutexP_tiRtos *)mutexObj->object;

        if ((mutex != NULL_PTR) && (mutex->used == TRUE))
        {
            /* Note: timeout is not used */
            mutexObj->key = GateMutexPri_enter(mutex->mutexHandle);
            ret = MutexP_OK;
        }
    }
    return ret;
}

MutexP_Status MutexP_unlock(MutexP_Handle handle)
{
    MutexP_Status ret = MutexP_FAILURE;
    MutexP_Object *mutexObj;
    MutexP_tiRtos *mutex;

    if (handle != NULL_PTR)
    {
        mutexObj = (MutexP_Object *)handle;
        mutex = (MutexP_tiRtos *)mutexObj->object;

        if ((mutex != NULL_PTR) && (mutex->used == TRUE))
        {
            GateMutexPri_leave(mutex->mutexHandle, mutexObj->key);
            ret = MutexP_OK;
        }
    }
    return ret;
}

/* Nothing past this point */
