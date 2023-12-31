/*
 * Copyright (c) 2017-2018, Texas Instruments Incorporated
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
 *  \file ipc_rsctable.h
 *
 *  \brief Define the resource table entries for all R5F cores. This will be
 *  incorporated into corresponding base images, and used by the remoteproc
 *  on the host-side to allocated/reserve resources.
 *
 */

#ifndef IPC_RSCTABLE_H_
#define IPC_RSCTABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CPU_c7x_1)
#define BUILD_C7X_1
#endif

#include <ti/drv/ipc/include/ipc_rsctypes.h>
#include <app_mem_map.h>

#ifdef SYSBIOS
  #include <xdc/runtime/SysMin.h>
#else
  #include "ipc_trace.h"
#endif

#define RPMSG_VRING_SIZE   (0x10000)

#define MCU1_0_R5F_MEM_RPMSG_VRING0    (DDR_MCU1_0_IPC_ADDR)
#define MCU1_0_R5F_MEM_RPMSG_VRING1    (DDR_MCU1_0_IPC_ADDR + RPMSG_VRING_SIZE)
#define MCU1_1_R5F_MEM_RPMSG_VRING0    (DDR_MCU1_1_IPC_ADDR)
#define MCU1_1_R5F_MEM_RPMSG_VRING1    (DDR_MCU1_1_IPC_ADDR + RPMSG_VRING_SIZE)
#define MCU2_0_R5F_MEM_RPMSG_VRING0    (DDR_MCU2_0_IPC_ADDR)
#define MCU2_0_R5F_MEM_RPMSG_VRING1    (DDR_MCU2_0_IPC_ADDR + RPMSG_VRING_SIZE)
#define MCU2_1_R5F_MEM_RPMSG_VRING0    (DDR_MCU2_1_IPC_ADDR)
#define MCU2_1_R5F_MEM_RPMSG_VRING1    (DDR_MCU2_1_IPC_ADDR + RPMSG_VRING_SIZE)
#define MCU3_0_R5F_MEM_RPMSG_VRING0    (DDR_MCU3_0_IPC_ADDR)
#define MCU3_0_R5F_MEM_RPMSG_VRING1    (DDR_MCU3_0_IPC_ADDR + RPMSG_VRING_SIZE)
#define MCU3_1_R5F_MEM_RPMSG_VRING0    (DDR_MCU3_1_IPC_ADDR)
#define MCU3_1_R5F_MEM_RPMSG_VRING1    (DDR_MCU3_1_IPC_ADDR + RPMSG_VRING_SIZE)
#define C66X_1_MEM_RPMSG_VRING0        (DDR_C66x_1_IPC_ADDR)
#define C66X_1_MEM_RPMSG_VRING1        (DDR_C66x_1_IPC_ADDR + RPMSG_VRING_SIZE)
#define C66X_2_MEM_RPMSG_VRING0        (DDR_C66x_2_IPC_ADDR)
#define C66X_2_MEM_RPMSG_VRING1        (DDR_C66x_2_IPC_ADDR + RPMSG_VRING_SIZE)
#define C7X_1_MEM_RPMSG_VRING0         (DDR_C7x_1_IPC_ADDR)
#define C7X_1_MEM_RPMSG_VRING1         (DDR_C7x_1_IPC_ADDR  + RPMSG_VRING_SIZE)

/*
 * Sizes of the virtqueues (expressed in number of buffers supported,
 * and must be power of 2)
 */
#define R5F_RPMSG_VQ0_SIZE      256
#define R5F_RPMSG_VQ1_SIZE      256
#define C66_RPMSG_VQ0_SIZE      256
#define C66_RPMSG_VQ1_SIZE      256
#define C7X_RPMSG_VQ0_SIZE      256
#define C7X_RPMSG_VQ1_SIZE      256

/* flip up bits whose indices represent features we support */
#define RPMSG_R5F_C0_FEATURES   1
#define RPMSG_C66_DSP_FEATURES  1
#define RPMSG_C7X_DSP_FEATURES  1

#ifdef SYSBIOS
  extern __T1_xdc_runtime_SysMin_Module_State__outbuf xdc_runtime_SysMin_Module_State_0_outbuf__A[];
  #define TRACEBUFADDR ((uintptr_t)&xdc_runtime_SysMin_Module_State_0_outbuf__A)
#else
  #define TRACEBUFADDR ((uintptr_t)&Ipc_traceBuffer)
#endif

const Ipc_ResourceTable ti_ipc_remoteproc_ResourceTable __attribute__ ((section (".resource_table"), aligned (4096))) = 
{
    1,                   /* we're the first version that implements this */
    NUM_ENTRIES,         /* number of entries in the table */
    0, 0,                /* reserved, must be zero */

    /* offsets to entries */
    {
        offsetof(Ipc_ResourceTable, rpmsg_vdev),
        offsetof(Ipc_ResourceTable, trace),
    },

    /* rpmsg vdev entry */
    {
        TYPE_VDEV, VIRTIO_ID_RPMSG, 0,
#if defined (CPU_c6x_1) || defined (CPU_c6x_2)
        RPMSG_C66_DSP_FEATURES, 0, 0, 0, 2, { 0, 0 },
#elif defined (CPU_c7x_1)
        RPMSG_C7X_DSP_FEATURES, 0, 0, 0, 2, { 0, 0 },
#else
        RPMSG_R5F_C0_FEATURES, 0, 0, 0, 2, { 0, 0 },
#endif
        /* no config data */
    },
    /* the two vrings */
#if defined (CPU_mcu1_0)
    { MCU1_0_R5F_MEM_RPMSG_VRING0, 4096, R5F_RPMSG_VQ0_SIZE, 1, 0 },
    { MCU1_0_R5F_MEM_RPMSG_VRING1, 4096, R5F_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (CPU_mcu1_1)
    { MCU1_1_R5F_MEM_RPMSG_VRING0, 4096, R5F_RPMSG_VQ0_SIZE, 1, 0 },
    { MCU1_1_R5F_MEM_RPMSG_VRING1, 4096, R5F_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (CPU_mcu2_0)
    { MCU2_0_R5F_MEM_RPMSG_VRING0, 4096, R5F_RPMSG_VQ0_SIZE, 1, 0 },
    { MCU2_0_R5F_MEM_RPMSG_VRING1, 4096, R5F_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (CPU_mcu2_1)
    { MCU2_1_R5F_MEM_RPMSG_VRING0, 4096, R5F_RPMSG_VQ0_SIZE, 1, 0 },
    { MCU2_1_R5F_MEM_RPMSG_VRING1, 4096, R5F_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (CPU_mcu3_0)
    { MCU3_0_R5F_MEM_RPMSG_VRING0, 4096, R5F_RPMSG_VQ0_SIZE, 1, 0 },
    { MCU3_0_R5F_MEM_RPMSG_VRING1, 4096, R5F_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (CPU_mcu3_1)
    { MCU3_1_R5F_MEM_RPMSG_VRING0, 4096, R5F_RPMSG_VQ0_SIZE, 1, 0 },
    { MCU3_1_R5F_MEM_RPMSG_VRING1, 4096, R5F_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (CPU_c6x_1)
    { C66X_1_MEM_RPMSG_VRING0, 4096, C66_RPMSG_VQ0_SIZE, 1, 0 },
    { C66X_1_MEM_RPMSG_VRING1, 4096, C66_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (CPU_c6x_2)
    { C66X_2_MEM_RPMSG_VRING0, 4096, C66_RPMSG_VQ0_SIZE, 1, 0 },
    { C66X_2_MEM_RPMSG_VRING1, 4096, C66_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (CPU_c7x_1)
    { C7X_1_MEM_RPMSG_VRING0, 4096, C7X_RPMSG_VQ0_SIZE, 1, 0 },
    { C7X_1_MEM_RPMSG_VRING1, 4096, C7X_RPMSG_VQ1_SIZE, 2, 0 },
#else
    #error CPU_<cpu name> not defined
#endif 

    {
#ifdef CPU_c7x_1
        (TRACE_INTS_VER1 | TYPE_TRACE), TRACEBUFADDR, 0x80000, 0, "trace:r5f0",
#else
        (TRACE_INTS_VER0 | TYPE_TRACE), TRACEBUFADDR, 0x80000, 0, "trace:r5f0",
#endif
    },
};

void *appGetIpcResourceTable()
{
    return (void*)&ti_ipc_remoteproc_ResourceTable;
}


#ifdef __cplusplus
}
#endif

#endif /* IPC_RSCTABLE_H_ */
