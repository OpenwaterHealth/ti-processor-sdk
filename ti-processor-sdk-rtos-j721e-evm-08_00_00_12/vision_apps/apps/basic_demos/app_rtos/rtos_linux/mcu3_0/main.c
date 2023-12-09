/*
 *
 * Copyright (c) 2018 Texas Instruments Incorporated
 *
 * All rights reserved not granted herein.
 *
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
 * license under copyrights and patents it now or hereafter owns or controls to make,
 * have made, use, import, offer to sell and sell ("Utilize") this software subject to the
 * terms herein.  With respect to the foregoing patent license, such license is granted
 * solely to the extent that any such patent is necessary to Utilize the software alone.
 * The patent license shall not apply to any combinations which include this software,
 * other than combinations with devices manufactured by or for TI ("TI Devices").
 * No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this license
 * (including the above copyright notice and the disclaimer and (if applicable) source
 * code license limitations below) in the documentation and/or other materials provided
 * with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted provided
 * that the following conditions are met:
 *
 * *       No reverse engineering, decompilation, or disassembly of this software is
 * permitted with respect to any software provided in binary form.
 *
 * *       any redistribution and use are licensed by TI for use only with TI Devices.
 *
 * *       Nothing shall obligate TI to provide you with source code for the software
 * licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the
 * source code are permitted provided that the following conditions are met:
 *
 * *       any redistribution and use of the source code, including any resulting derivative
 * works, are licensed by TI for use only with TI Devices.
 *
 * *       any redistribution and use of any object code compiled from the source code
 * and any resulting derivative works, are licensed by TI for use only with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers
 *
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <app.h>
#include <utils/console_io/include/app_log.h>
#include <stdio.h>
#include <string.h>
#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>
#include <app_ipc_rsctable.h>
#include <ti/drv/i2c/I2C.h>
#include <ti/drv/i2c/soc/I2C_soc.h>
#include "gpio_utils.h"

#define MILLISEC 1000u
#define MICROSEC 1u

//#define ENABLE_TIMER

#ifdef ENABLE_TIMER

volatile static uint32_t ranAtLeastOnce= 0;
volatile static uint64_t frameCount= 0;
volatile static uint64_t firstFrameTimerStamp= 0;

static TimerP_Handle startfSyncTimer= NULL;

static void startfSyncFxn(uintptr_t a0)
{
    uint64_t timerStamp;

    /* Code to turn on GPIO to start frame sync */
    timerStamp = TimerP_getTimeInUsecs();

    if (ranAtLeastOnce == 0)
    {
        ranAtLeastOnce = 1;
        firstFrameTimerStamp= TimerP_getTimeInUsecs();
    }

    timerStamp = TimerP_getTimeInUsecs();
    appLogPrintf("%llu: Start frame sync #%llu\n", timerStamp, frameCount++);
}

static void appEnableHwTimer(uint32_t fSyncPeriod)
{
    TimerP_Params timerParams;

    /* Initialize the periodic start frame sync timer params */
    TimerP_Params_init(&timerParams);

    timerParams.startMode = TimerP_StartMode_USER;
    timerParams.periodType = TimerP_PeriodType_MICROSECS;
    timerParams.period = fSyncPeriod;
    timerParams.arg = NULL;
    timerParams.runMode = TimerP_RunMode_CONTINUOUS;

    startfSyncTimer = TimerP_create(TimerP_ANY, startfSyncFxn, &timerParams);

    if(NULL != startfSyncTimer)
    {
        TimerP_start(startfSyncTimer);
    }
}
#endif

static void appMain(void* arg0, void* arg1)
{
    appInit();
#ifdef ENABLE_TIMER
    appEnableHwTimer(10000u*MILLISEC);
#endif
    appRun();
    #if 1
    while(1)
    {
        appLogWaitMsecs(100u);
    }
    #else
    appDeInit();
    #endif
}

void StartupEmulatorWaitFxn (void)
{
    volatile uint32_t enableDebug = 0;
    do
    {
    }while (enableDebug);
}

static uint8_t gTskStackMain[8*1024]
__attribute__ ((section(".bss:taskStackSection")))
__attribute__ ((aligned(8192)))
    ;

int main(void)
{
    TaskP_Params tskParams;
    TaskP_Handle task;

    /* This is for debug purpose - see the description of function header */
    StartupEmulatorWaitFxn();

    OS_init();

    TaskP_Params_init(&tskParams);
    tskParams.priority = 8u;
    tskParams.stack = gTskStackMain;
    tskParams.stacksize = sizeof (gTskStackMain);
    task = TaskP_create(appMain, &tskParams);
    if(NULL == task)
    {
        OS_stop();
    }
    OS_start();

    return 0;
}

uint32_t appGetDdrSharedHeapSize()
{
    return DDR_SHARED_MEM_SIZE;

}
