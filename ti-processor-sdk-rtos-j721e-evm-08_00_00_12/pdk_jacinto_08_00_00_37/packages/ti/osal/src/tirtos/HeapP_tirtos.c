/*
 * Copyright (c) 2018, Texas Instruments Incorporated
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
 *  ======== HeapP_tirtos.c ========
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ti/osal/HeapP.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/heaps/HeapMem.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <ti/osal/src/tirtos/tirtos_config.h>

extern void Osal_DebugP_assert(int32_t expression, const char *file, int32_t line);
#define HEAPOSAL_Assert(expression) (Osal_DebugP_assert((int32_t)((expression)?1:0),\
                                                  __FILE__, __LINE__))

extern uint32_t  gOsalHeapAllocCnt, gOsalHeapPeak;

/*!
 *  @brief    Heap structure
 */
typedef struct HeapP_tiRtos_s {
    Bool            used;
    HeapMem_Struct  heapStruct;
    HeapMem_Handle  heapHandle;
} HeapP_tiRtos;

/* global pool of statically allocated heap pools */
static HeapP_tiRtos gOsalHeapPTiRtosPool[OSAL_TIRTOS_CONFIGNUM_HEAP];

/*
 *  ======== HeapP_Params_init ========
 */
void HeapP_Params_init(HeapP_Params *params)
{
    if (params != NULL_PTR)
    {
        params->size    =  0U;
        params->buf     =  NULL_PTR;
    }
    return;
}

/*
 *  ======== HeapP_create ========
 */
HeapP_Handle HeapP_create(const HeapP_Params *params)
{
    HeapP_Handle     ret_handle = NULL_PTR; 
    HeapP_tiRtos     *handle = (HeapP_tiRtos *) NULL_PTR;
    HeapP_tiRtos     *heapPool;
    uint32_t         i;
    uintptr_t        key;
    uint32_t         maxHeap;
    HeapMem_Params   heapMemPrm;

    if ((params != NULL_PTR) && (params->buf != NULL_PTR) && (params->size != 0U))
    {
        /* Pick up the internal static memory block */
        heapPool       = (HeapP_tiRtos *) &gOsalHeapPTiRtosPool[0];
        maxHeap        = OSAL_TIRTOS_CONFIGNUM_HEAP;
        
        if(gOsalHeapAllocCnt==0U) 
        {
            (void)memset( (void *)gOsalHeapPTiRtosPool,0,sizeof(gOsalHeapPTiRtosPool));
        }

        key = HwiP_disable();

        for (i = 0; i < maxHeap; i++)
        {
            if (heapPool[i].used == FALSE)
            {
                heapPool[i].used = TRUE;
                /* Update statistics */
                gOsalHeapAllocCnt++;
                if (gOsalHeapAllocCnt > gOsalHeapPeak)
                {
                    gOsalHeapPeak = gOsalHeapAllocCnt;
                }
                break;
            }
        }
        HwiP_restore(key);

        if (i < maxHeap)
        {
            /* Grab the memory */
            handle = (HeapP_tiRtos *) &heapPool[i];
        }

        if (handle == NULL_PTR) {
            ret_handle = NULL_PTR;
        }
        else
        {
            HeapMem_Params_init(&heapMemPrm);
            heapMemPrm.buf           = params->buf;
            heapMemPrm.size          = params->size;
            heapMemPrm.minBlockAlign = HeapP_BYTE_ALIGNMENT;

            HeapMem_construct(&handle->heapStruct, &heapMemPrm);
            handle->heapHandle = HeapMem_handle(&handle->heapStruct);

            ret_handle = ((HeapP_Handle)handle);
        }
    }

    return ret_handle;
}

/*
 *  ======== HeapP_delete ========
 */
HeapP_Status HeapP_delete(HeapP_Handle handle)
{
    HEAPOSAL_Assert((handle == NULL_PTR));

    uintptr_t   key;
    HeapP_Status ret_val = HeapP_OK;
    HeapP_tiRtos *heap = (HeapP_tiRtos *)handle;

    if((heap != NULL_PTR) && (heap->used==TRUE))
    {
        HeapMem_destruct(&heap->heapStruct);

        key = HwiP_disable();
        heap->used = FALSE;
        heap->heapHandle = NULL;
        /* Found the osal heap object to delete */
        if (gOsalHeapAllocCnt > 0U)
        {
            gOsalHeapAllocCnt--;
        }
        HwiP_restore(key);
    }
    else
    {
       ret_val = HeapP_FAILURE;
    }  

    return ret_val;
}

/*
 *  ======== HeapP_alloc ========
 */
void *HeapP_alloc(HeapP_Handle handle, uint32_t allocSize)
{
    HEAPOSAL_Assert((handle == NULL_PTR));

    void *ptr = NULL_PTR;
    HeapP_tiRtos *heap = (HeapP_tiRtos *)handle;

    if((heap != NULL_PTR) && (heap->used==TRUE))
    {
        ptr = HeapMem_alloc(heap->heapHandle, allocSize, HeapP_BYTE_ALIGNMENT, NULL);
    }

    return ptr;
}

/*
 *  ======== HeapP_free ========
 */
HeapP_Status HeapP_free(HeapP_Handle handle, void *ptr, uint32_t size)
{
    DebugP_assert((handle != NULL_PTR));

    HeapP_Status ret_val = HeapP_OK;
    HeapP_tiRtos *heap = (HeapP_tiRtos *)handle;

    if((heap != NULL_PTR) && (heap->used==TRUE) && (ptr != NULL_PTR))
    {
        HeapMem_free(heap->heapHandle, ptr, size);
    }
    else
    {
       ret_val = HeapP_FAILURE;
    }  

    return ret_val;
}

/*
 *  ======== HeapP_getHeapStats ========
 */
HeapP_Status HeapP_getHeapStats(HeapP_Handle handle, HeapP_MemStats *stats)
{
    DebugP_assert((handle != NULL_PTR));
    
    HeapP_Status ret_val = HeapP_OK;
    HeapP_tiRtos *heap = (HeapP_tiRtos *)handle;
    Memory_Stats heapMemStats;

    if((heap != NULL_PTR) && (heap->used==TRUE))
    {
        HeapMem_getStats(heap->heapHandle, &heapMemStats);

        stats->totalSize        = (uint32_t)heapMemStats.totalSize;
        stats->totalFreeSize    = (uint32_t)heapMemStats.totalFreeSize;
        stats->largestFreeSize  = (uint32_t)heapMemStats.largestFreeSize; 
    }
    else
    {
       ret_val = HeapP_FAILURE;
    }
    
    return ret_val;
}

