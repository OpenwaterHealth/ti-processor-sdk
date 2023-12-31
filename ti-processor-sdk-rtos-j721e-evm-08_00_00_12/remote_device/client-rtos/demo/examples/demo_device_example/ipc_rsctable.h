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

#include <ti/drv/ipc/include/ipc_rsctypes.h>

#define R5F_MEM_RPMSG_VRING0     0xA0000000
#define R5F_MEM_RPMSG_VRING1     0xA0010000
#define MCU1_0_R5F_MEM_RPMSG_VRING0    0xA0000000
#define MCU1_0_R5F_MEM_RPMSG_VRING1    0xA0010000
#define MCU1_1_R5F_MEM_RPMSG_VRING0    0xA1000000
#define MCU1_1_R5F_MEM_RPMSG_VRING1    0xA1010000
#define MCU2_0_R5F_MEM_RPMSG_VRING0    0xA2000000
#define MCU2_0_R5F_MEM_RPMSG_VRING1    0xA2010000
#define MCU2_1_R5F_MEM_RPMSG_VRING0    0xA3000000
#define MCU2_1_R5F_MEM_RPMSG_VRING1    0xA3010000
#define MCU3_0_R5F_MEM_RPMSG_VRING0    0xA4000000
#define MCU3_0_R5F_MEM_RPMSG_VRING1    0xA4010000
#define MCU3_1_R5F_MEM_RPMSG_VRING0    0xA5000000
#define MCU3_1_R5F_MEM_RPMSG_VRING1    0xA5010000
#define C66X_1_MEM_RPMSG_VRING0        0xA7000000 /* Different than expected for caching */
#define C66X_1_MEM_RPMSG_VRING1        0xA7010000
#define C66X_2_MEM_RPMSG_VRING0        0xA6000000 /* Different than expected for caching */
#define C66X_2_MEM_RPMSG_VRING1        0xA6010000
#define C7X_1_MEM_RPMSG_VRING0         0xA8000000
#define C7X_1_MEM_RPMSG_VRING1         0xA8010000

#define SZ_1M                          0x00100000

#define R5F_MEM_IPC_VRING_SIZE  SZ_1M

#define R5F_NUM_ENTRIES 2

/*
 * Assign fixed RAM addresses to facilitate a fixed MMU table.
 * PHYS_MEM_IPC_VRING & PHYS_MEM_IPC_DATA MUST be together.
 */
/* See CMA BASE addresses in Linux side: arch/arm/mach-omap2/remoteproc.c */
#define PHYS_MEM_IPC_VRING        0xA0000000
#define MCU1_0_PHYS_MEM_IPC_VRING 0xA0000000
#define MCU1_1_PHYS_MEM_IPC_VRING 0xA1000000
#define MCU2_0_PHYS_MEM_IPC_VRING 0xA2000000
#define MCU2_1_PHYS_MEM_IPC_VRING 0xA3000000
#define MCU3_0_PHYS_MEM_IPC_VRING 0xA4000000
#define MCU3_1_PHYS_MEM_IPC_VRING 0xA5000000
#define C66X_1_PHYS_MEM_IPC_VRING 0xA7000000 /* Different than expected for caching purpose */
#define C66X_2_PHYS_MEM_IPC_VRING 0xA6000000 /* Different than expected for caching purpose */
#define C7X_1_PHYS_MEM_IPC_VRING  0xA8000000

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
  #include <xdc/runtime/SysMin.h>
  #define IPC_TRACE_BUFFER_MAX_SIZE     (0x80000)
  extern __T1_xdc_runtime_SysMin_Module_State__outbuf xdc_runtime_SysMin_Module_State_0_outbuf__A[];
  #define TRACEBUFADDR ((uintptr_t)&xdc_runtime_SysMin_Module_State_0_outbuf__A)
#else
  #include "ipc_trace.h"
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
#if defined (BUILD_C66X_1) || defined (BUILD_C66X_2)
        RPMSG_C66_DSP_FEATURES, 0, 0, 0, 2, { 0, 0 },
#elif defined (BUILD_C7X_1)
        RPMSG_C7X_DSP_FEATURES, 0, 0, 0, 2, { 0, 0 },
#else
        RPMSG_R5F_C0_FEATURES, 0, 0, 0, 2, { 0, 0 },
#endif
        /* no config data */
    },
    /* the two vrings */
#if defined (BUILD_MCU1_0)
    { MCU1_0_R5F_MEM_RPMSG_VRING0, 4096, R5F_RPMSG_VQ0_SIZE, 1, 0 },
    { MCU1_0_R5F_MEM_RPMSG_VRING1, 4096, R5F_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (BUILD_MCU1_1)
    { MCU1_1_R5F_MEM_RPMSG_VRING0, 4096, R5F_RPMSG_VQ0_SIZE, 1, 0 },
    { MCU1_1_R5F_MEM_RPMSG_VRING1, 4096, R5F_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (BUILD_MCU2_0)
    { MCU2_0_R5F_MEM_RPMSG_VRING0, 4096, R5F_RPMSG_VQ0_SIZE, 1, 0 },
    { MCU2_0_R5F_MEM_RPMSG_VRING1, 4096, R5F_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (BUILD_MCU2_1)
    { MCU2_1_R5F_MEM_RPMSG_VRING0, 4096, R5F_RPMSG_VQ0_SIZE, 1, 0 },
    { MCU2_1_R5F_MEM_RPMSG_VRING1, 4096, R5F_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (BUILD_MCU3_0)
    { MCU3_0_R5F_MEM_RPMSG_VRING0, 4096, R5F_RPMSG_VQ0_SIZE, 1, 0 },
    { MCU3_0_R5F_MEM_RPMSG_VRING1, 4096, R5F_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (BUILD_MCU3_1)
    { MCU3_1_R5F_MEM_RPMSG_VRING0, 4096, R5F_RPMSG_VQ0_SIZE, 1, 0 },
    { MCU3_1_R5F_MEM_RPMSG_VRING1, 4096, R5F_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (BUILD_C66X_1)
    { C66X_1_MEM_RPMSG_VRING0, 4096, C66_RPMSG_VQ0_SIZE, 1, 0 },
    { C66X_1_MEM_RPMSG_VRING1, 4096, C66_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (BUILD_C66X_2)
    { C66X_2_MEM_RPMSG_VRING0, 4096, C66_RPMSG_VQ0_SIZE, 1, 0 },
    { C66X_2_MEM_RPMSG_VRING1, 4096, C66_RPMSG_VQ1_SIZE, 2, 0 },
#elif defined (BUILD_C7X_1)
    { C7X_1_MEM_RPMSG_VRING0, 4096, C7X_RPMSG_VQ0_SIZE, 1, 0 },
    { C7X_1_MEM_RPMSG_VRING1, 4096, C7X_RPMSG_VQ1_SIZE, 2, 0 },
#else
    { R5F_MEM_RPMSG_VRING0, 4096, R5F_RPMSG_VQ0_SIZE, 1, 0 },
    { R5F_MEM_RPMSG_VRING1, 4096, R5F_RPMSG_VQ1_SIZE, 2, 0 },
#endif

    {
#ifdef BUILD_C7X_1
        (TRACE_INTS_VER1 | TYPE_TRACE), TRACEBUFADDR, IPC_TRACE_BUFFER_MAX_SIZE, 0, "trace:r5f0",
#else
        (TRACE_INTS_VER0 | TYPE_TRACE), TRACEBUFADDR, IPC_TRACE_BUFFER_MAX_SIZE, 0, "trace:r5f0",
#endif
    },
};


#ifdef __cplusplus
}
#endif

#endif /* IPC_RSCTABLE_H_ */
