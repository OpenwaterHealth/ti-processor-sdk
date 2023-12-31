/**
 *   @file  MemoryP.h
 *
 *   @brief
 *      Memory module for the RTOS Porting Interface
 *
 *  \par
 *  NOTE:
 *      (C) Copyright 2016-2020 Texas Instruments, Inc.
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
 *  \ingroup DRV_OSAL_MODULE
 *  \defgroup DRV_OSAL_MemoryP MemoryP
 *            MemoryP interface
 *
 *  @{
 */

/** ============================================================================
 *  @file       MemoryP.h
 *
 *  @brief      Memory module for the RTOS Porting Interface
 *
 *
 *  ============================================================================
 */
 
#ifndef ti_osal_MemoryP__include
#define ti_osal_MemoryP__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*!
 *  @brief    Memory Heap stats
 *
 *  Structure that contains the memory heap stats.
 */
typedef struct
{
    /*! Total size of heap in number of bytes */
    uint32_t    totalSize;

    /*! Total size of free memory in the heap in number of bytes */
    uint32_t    totalFreeSize;
}MemoryP_Stats;

/*!
 *  @brief  Function which is used to allocate memory for control operations.
 *
 *  @param  size        Size of the requested memory
 *  @param  alignment   Requested alignment in bytes
 *
 *  @return Pointer to the allocated memory on success or a NULL on an error
 */
extern void* MemoryP_ctrlAlloc(uint32_t size, uint8_t alignment);

/*!
 *  @brief  Function which is used to free previously allocated control memory
 *
 *  @param  ptr         Pointer to the memory to be cleaned up
 *  @param  size        Size of the memory which is being cleaned up
 *
 *  @return None
 */
extern void  MemoryP_ctrlFree(void* ptr, uint32_t size);

/*!
 *  @brief  Function which is used to allocate memory for data operations.
 *
 *  @param  size        Size of the requested memory
 *  @param  alignment   Requested alignment in bytes
 *
 *  @return Pointer to the allocated memory on success or a NULL on an error
 */
extern void* MemoryP_dataAlloc(uint32_t size, uint8_t alignment);

/*!
 *  @brief  Function which is used to free previously allocated data memory
 *
 *  @param  ptr         Pointer to the memory to be cleaned up
 *  @param  size        Size of the memory which is being cleaned up
 *
 *  @return None
 */
extern void  MemoryP_dataFree(void* ptr, uint32_t size);

/*!
 *  @brief  Function which is used to get data memory stats
 *
 *  @param  stats         Pointer to memory stats structure
 *
 *  @return None
 */
extern void MemoryP_getStats(MemoryP_Stats *stats);

#ifdef __cplusplus
}
#endif

#endif /* ti_osal_MemoryP__include */
/* @} */
