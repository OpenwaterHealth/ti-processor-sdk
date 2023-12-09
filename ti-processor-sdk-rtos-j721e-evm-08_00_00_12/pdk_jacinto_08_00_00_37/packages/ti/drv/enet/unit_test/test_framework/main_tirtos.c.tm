/*
 *  Copyright (c) Texas Instruments Incorporated 2020
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
 * s *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 *  \file main_tirtos.c
 *
 *  \brief Main file for TI-RTOS build
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/board/board.h>
#include <ti/drv/sciclient/sciclient.h>
#include <ti/drv/enet/enet.h>
#include <ti/drv/enet/examples/utils/include/enet_apputils.h>

#include "trace.h"
#include "enet_test_base.h"
#include "enet_test_board.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Test application stack size */
#define APP_TSK_STACK_MAIN              (20U * 1024U)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* Task function */
static Void taskFxn(UArg a0,
                    UArg a1);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Test application stack */
static uint8_t gAppTskStackMain[APP_TSK_STACK_MAIN]
__attribute__ ((aligned(32)));

/** \brief Log enable for CPSW Unit Test  application */
uint32_t gAppTrace = ((uint32_t)GT_INFO | \
                      (uint32_t)GT_TraceState_Enable);
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    Task_Handle task;
    Error_Block eb;
    Task_Params taskParams;

    Error_init(&eb);

    /* Initialize the task params */
    Task_Params_init(&taskParams);
    /* Set the task priority higher than the default priority (1) */
    taskParams.priority       = 2;
    taskParams.stack          = gAppTskStackMain;
    taskParams.stackSize      = sizeof(gAppTskStackMain);
    taskParams.instance->name = "UT Task";

    task = Task_create(taskFxn, &taskParams, &eb);
    EnetAppUtils_assert(Error_check(&eb) == FALSE);

    if (NULL == task)
    {
        BIOS_exit(0);
    }

    BIOS_start();    /* does not return */

    return(0);
}

static Void taskFxn(UArg a0,
                    UArg a1)
{
    int32_t testPassed = ENET_SOK;

    EnetTestBoard_init();

    if (testPassed == ENET_SOK)
    {
        testPassed = enetTestParser();
    }

    if (testPassed != ENET_SOK)
    {
        GT_0trace(gAppTrace, GT_INFO1, " CPSW Unit Test Failed!!!\n");
    }

    EnetTestBoard_deInit();

    return;
}
