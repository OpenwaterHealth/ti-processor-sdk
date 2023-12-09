/*
 * System Firmware Source File
 *
 * Host IDs for AM6 device
 *
 * Copyright (C) 2017-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
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

/**
 * \file am65x_sr2/hosts.h
 *
 * \brief Host IDs for AM6 device
 */

#ifndef AM6_HOSTS_H
#define AM6_HOSTS_H

/* Host IDs for AM6 Device */

/** DMSC(Secure): Device Management and Security Control */
#define HOST_ID_DMSC (0U)
/** r5_0(Non Secure): Cortex R5 Context 0 on MCU island */
#define HOST_ID_R5_0 (3U)
/** r5_1(Secure): Cortex R5 Context 1 on MCU island(Boot) */
#define HOST_ID_R5_1 (4U)
/** r5_2(Non Secure): Cortex R5 Context 2 on MCU island */
#define HOST_ID_R5_2 (5U)
/** r5_3(Secure): Cortex R5 Context 3 on MCU island */
#define HOST_ID_R5_3 (6U)
/** a53_0(Secure): Cortex A53 context 0 on Main island */
#define HOST_ID_A53_0 (10U)
/** a53_1(Secure): Cortex A53 context 1 on Main island */
#define HOST_ID_A53_1 (11U)
/** a53_2(Non Secure): Cortex A53 context 2 on Main island */
#define HOST_ID_A53_2 (12U)
/** a53_3(Non Secure): Cortex A53 context 3 on Main island */
#define HOST_ID_A53_3 (13U)
/** a53_4(Non Secure): Cortex A53 context 4 on Main island */
#define HOST_ID_A53_4 (14U)
/** a53_5(Non Secure): Cortex A53 context 5 on Main island */
#define HOST_ID_A53_5 (15U)
/** a53_6(Non Secure): Cortex A53 context 6 on Main island */
#define HOST_ID_A53_6 (16U)
/** a53_7(Non Secure): Cortex A53 context 7 on Main island */
#define HOST_ID_A53_7 (17U)
/** gpu_0(Non Secure): SGX544 Context 0 on Main island */
#define HOST_ID_GPU_0 (30U)
/** gpu_1(Non Secure): SGX544 Context 1 on Main island */
#define HOST_ID_GPU_1 (31U)
/** icssg_0(Non Secure): ICSS Context 0 on Main island */
#define HOST_ID_ICSSG_0 (50U)
/** icssg_1(Non Secure): ICSS Context 1 on Main island */
#define HOST_ID_ICSSG_1 (51U)
/** icssg_2(Non Secure): ICSS Context 2 on Main island */
#define HOST_ID_ICSSG_2 (52U)

/**
 * Host catch all. Used in board configuration resource assignments to
 * define resource ranges useable by all hosts. Cannot be used
 */
#define HOST_ID_ALL (128U)

/** Number of unique hosts on the SoC */
#define HOST_ID_CNT (19U)

#endif /* AM6_HOSTS_H */
