/*
 *  Copyright (c) Texas Instruments Incorporated 2020
 *  All rights reserved.
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
 *  \file ex04_linux_baremetal_2core_echo_test.c
 *
 *  \brief 2-core (Linux-to-Baremetal) IPC echo test application performing
 *  basic echo communication using the baremetal IPC driver
 *  The application after initialization of the IPC lld, waits for messages
 *  from the Linux host core and echos each message received, back to the 
 *  source.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <ti/osal/HwiP.h>
#include <ti/osal/osal.h>
/* SCI Client */
#include <ti/drv/sciclient/sciclient.h>

#include <ti/drv/ipc/examples/common/src/ipc_setup.h>
#include <ti/drv/ipc/ipc.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/* Number of cores used in the test */
#if defined (SOC_AM65XX)
#define CORE_IN_TEST            3
#elif defined (SOC_J721E)
#define CORE_IN_TEST            10
#elif defined (SOC_J7200)
#define CORE_IN_TEST            5
#elif defined (SOC_AM64X)
#define CORE_IN_TEST            6
#else
#error "Invalid SOC"
#endif

#define NUM_RESPONDER_TASKS     2

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
uint8_t  gCntrlBuf[RPMSG_DATA_SIZE] __attribute__ ((section("ipc_data_buffer"), aligned (8)));
uint8_t  gSysVqBuf[VQ_BUF_SIZE]  __attribute__ ((section ("ipc_data_buffer"), aligned (8)));
uint8_t  gSendBuf[RPMSG_DATA_SIZE * CORE_IN_TEST]  __attribute__ ((section ("ipc_data_buffer"), aligned (8)));
uint8_t  gRspBuf[RPMSG_DATA_SIZE * NUM_RESPONDER_TASKS]  __attribute__ ((section ("ipc_data_buffer"), aligned (8)));

uint8_t *pCntrlBuf = gCntrlBuf;
uint8_t *pSendTaskBuf = gSendBuf;
uint8_t *pRecvTaskBuf = gRspBuf;
uint8_t *pSysVqBuf = gSysVqBuf;

#ifdef BUILD_MPU1_0
uint32_t selfProcId = IPC_MPU1_0;
uint32_t remoteProc[] =
{
    IPC_MCU1_0
};
#endif

#ifdef BUILD_MCU1_0
uint32_t selfProcId = IPC_MCU1_0;
uint32_t remoteProc[] =
{
    IPC_MPU1_0
};
#elif defined(BUILD_MCU1_1)
uint32_t selfProcId = IPC_MCU1_1;
uint32_t remoteProc[] =
{
    IPC_MPU1_0
};
#elif defined(BUILD_MCU2_0)
uint32_t selfProcId = IPC_MCU2_0;
uint32_t remoteProc[] =
{
    IPC_MPU1_0
};
#elif defined(BUILD_MCU2_1)
uint32_t selfProcId = IPC_MCU2_1;
uint32_t remoteProc[] =
{
    IPC_MPU1_0
};
#elif defined(BUILD_MCU3_0)
uint32_t selfProcId = IPC_MCU3_0;
uint32_t remoteProc[] =
{
    IPC_MPU1_0
};
#elif defined(BUILD_MCU3_1)
uint32_t selfProcId = IPC_MCU3_1;
uint32_t remoteProc[] =
{
    IPC_MPU1_0
};
#elif defined(BUILD_M4F_0)
uint32_t selfProcId = IPC_M4F_0;
uint32_t remoteProc[] =
{
    IPC_MPU1_0
};
#else
/* NOTE: all other cores are not used in this test, but must be built as part of full PDK build */
uint32_t selfProcId = 0;
uint32_t remoteProc[] = {};
#endif

uint32_t *pRemoteProcArray = remoteProc;
uint32_t  gNumRemoteProc = sizeof(remoteProc)/sizeof(uint32_t);

RPMessage_Handle gHandleArray[CORE_IN_TEST];
uint32_t         gEndptArray[CORE_IN_TEST];
uint32_t         gCntPing[CORE_IN_TEST];
uint32_t         gCntPong[CORE_IN_TEST];

RPMessage_Handle *pHandleArray = gHandleArray;
uint32_t *pEndptArray = gEndptArray;
uint32_t *pCntPing = gCntPing;
uint32_t *pCntPong = gCntPong;
