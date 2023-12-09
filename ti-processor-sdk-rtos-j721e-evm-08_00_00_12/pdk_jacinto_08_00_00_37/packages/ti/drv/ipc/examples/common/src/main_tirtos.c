/*
 *  Copyright (c) Texas Instruments Incorporated 2018
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
 *  \file main_tirtos.c
 *
 *  \brief Main file for TI-RTOS build
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <stdint.h>

#include <ti/drv/ipc/examples/common/src/ipc_setup.h>

#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>

#include "ipc_utils.h"
#include <ti/csl/csl_types.h>
#if defined (__C7100__)
#include <ti/sysbios/family/c7x/Hwi.h>
#include <ti/sysbios/family/c7x/Mmu.h>
#include <ti/csl/soc.h>
#include <ti/csl/csl_clec.h>
#include <ti/csl/arch/csl_arch.h>
#include <ti/sysbios/family/c7x/Mmu.h>
#endif

#include <ti/drv/sciclient/sciclient.h>
#include <ti/board/board.h>

#if (defined (BUILD_MCU1_0) && (defined (SOC_J721E) || defined (SOC_J7200)))
#include <ti/drv/sciclient/sciserver_tirtos.h>
#endif

/* This needs to be enabled only for negative test cases */
#ifdef IPC_NEGATIVE_TEST
#include <ti/drv/ipc/examples/ex05_bios_multicore_echo_negative_test/ipc_neg_setup.h>
#endif
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define APP_TSK_STACK_MAIN              (32U * 1024U)
/**< Test application stack size */

#if (defined (BUILD_MCU1_0) && (defined (SOC_J721E) || defined (SOC_J7200)))
/**< SCI Server Init Task stack size */
#define APP_SCISERVER_INIT_TSK_STACK        (32U * 1024U)
/* SCI Server Init Task Priority - must be higher than High priority Sciserver task */
#define IPC_INIT_SCISERVER_TASK_PRI         (6)
#endif

/* High Priority for SCI Server - must be higher than Low priority task */
#define IPC_SETUP_SCISERVER_TASK_PRI_HIGH   (5)
/*
 * Low Priority for SCI Server - must be higher than IPC echo test tasks
 * to prevent delay in handling Sciserver requests when test is performing
 * multicore ping/pong.
 */
#define IPC_SETUP_SCISERVER_TASK_PRI_LOW    (4)

/* Aligned address at which the X509 header is placed */
#define SCISERVER_COMMON_X509_HEADER_ADDR (0x41cffb00)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void taskFxn(void* a0, void* a1);

#if (defined (BUILD_MCU1_0) && (defined (SOC_J721E) || defined (SOC_J7200)))
void Ipc_setupSciServer(void);
/**< Initialize SCI Server, to process RM/PM Requests by other cores */
#endif

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Test application stack */
static uint8_t  gAppTskStackMain[APP_TSK_STACK_MAIN]
__attribute__ ((aligned(8192)));

#if (defined (BUILD_MCU1_0) && (defined (SOC_J721E) || defined (SOC_J7200)))
/* Sciserver Init TAsk stack */
static uint8_t  gSciserverInitTskStack[APP_SCISERVER_INIT_TSK_STACK]
__attribute__ ((aligned(8192)));
#endif

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


void ipc_initSciclient()
{
    int32_t ret = CSL_PASS;
    Sciclient_ConfigPrms_t        config;

    /* Now reinitialize it as default parameter */
    ret = Sciclient_configPrmsInit(&config);
    if (ret != CSL_PASS)
    {
        App_printf("Sciclient_configPrmsInit Failed\n");
    }

#if (defined (BUILD_MCU1_0) && (defined (SOC_J721E) || defined (SOC_J7200)))
    if (ret == CSL_PASS)
    {
        ret = Sciclient_boardCfgParseHeader(
            (uint8_t *)SCISERVER_COMMON_X509_HEADER_ADDR,
            &config.inPmPrms, &config.inRmPrms);
        if (ret != CSL_PASS)
        {
            App_printf("Sciclient_boardCfgParseHeader Failed\n");
        }
    }
#endif

    if (ret == CSL_PASS)
    {
        ret = Sciclient_init(&config);
        if (ret != CSL_PASS)
        {
            App_printf("Sciclient_init Failed\n");
        }
    }
}

#if !defined(A72_LINUX_OS)
void ipc_boardInit()
{
    Board_initCfg           boardCfg;

    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_UART_STDIO;
    Board_init(boardCfg);

}
#endif

#if defined (__C7100__)
/* To set C71 timer interrupts */
void ipc_timerInterruptInit(void)
{
    CSL_ClecEventConfig   cfgClec;
    CSL_CLEC_EVTRegs     *clecBaseAddr = (CSL_CLEC_EVTRegs*)CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE;

    uint32_t input         = 1248; /* Used for Timer Interrupt */
    uint32_t corepackEvent = 14;

    /* Configure CLEC */
    cfgClec.secureClaimEnable = FALSE;
    cfgClec.evtSendEnable     = TRUE;
    cfgClec.rtMap             = CSL_CLEC_RTMAP_CPU_ALL;
    cfgClec.extEvtNum         = 0;
    cfgClec.c7xEvtNum         = corepackEvent;
    CSL_clecConfigEvent(clecBaseAddr, input, &cfgClec);
    CSL_clecConfigEventLevel(clecBaseAddr, input, 0); /* configure interrupt as pulse */
    Hwi_setPriority(corepackEvent, 1);
}
#endif

#if defined (_TMS320C6X)
/* To set C66 timer interrupts */
void ipc_timerInterruptInit(void)
{
    int32_t status = 0;

    struct tisci_msg_rm_irq_set_req     rmIrqReq;
    struct tisci_msg_rm_irq_set_resp    rmIrqResp;

    /* On C66x builds we define OS timer tick in the configuration file to
     * trigger event #21 for C66x_1 and #20 for C66x_2. Map
     * DMTimer 0 interrupt to these events through DMSC RM API.
     */
    rmIrqReq.valid_params           = TISCI_MSG_VALUE_RM_DST_ID_VALID |
                                      TISCI_MSG_VALUE_RM_DST_HOST_IRQ_VALID;
    rmIrqReq.src_id                 = TISCI_DEV_TIMER0;
    rmIrqReq.src_index              = 0U;
#if defined(BUILD_C66X_1)
    rmIrqReq.dst_id                 = TISCI_DEV_C66SS0_CORE0;
    rmIrqReq.dst_host_irq           = 21U;
#elif defined(BUILD_C66X_2)
    rmIrqReq.dst_id                 = TISCI_DEV_C66SS1_CORE0;
    rmIrqReq.dst_host_irq           = 20U;
#endif
    /* Unused params */
    rmIrqReq.global_event           = 0U;
    rmIrqReq.ia_id                  = 0U;
    rmIrqReq.vint                   = 0U;
    rmIrqReq.vint_status_bit_index  = 0U;
    rmIrqReq.secondary_host         = TISCI_MSG_VALUE_RM_UNUSED_SECONDARY_HOST;

    status = Sciclient_rmIrqSet(&rmIrqReq, &rmIrqResp, SCICLIENT_SERVICE_WAIT_FOREVER);
    if(status != 0)
    {
        App_printf(" ERROR: failed to setup timer interrupt !!!\n" );
    }

    return;
}
#endif

int main(void)
{
    TaskP_Handle task;
    TaskP_Params taskParams;



#if defined ECHO_TEST_BTCM && defined FREERTOS
    /* Relocate FreeRTOS Reset Vectors from BTCM*/
    void _freertosresetvectors (void);  
    memcpy((void *)0x0, (void *)_freertosresetvectors, 0x40);
#endif

    OS_init();

    /* Initialize the task params */
    TaskP_Params_init(&taskParams);
    /* Set the task priority higher than the default priority (1) */
    taskParams.priority = 2;
    taskParams.stack        = gAppTskStackMain;
    taskParams.stacksize    = sizeof (gAppTskStackMain);

    task = TaskP_create(taskFxn, &taskParams);
    if(NULL == task)
    {
        OS_stop();
    }
    OS_start();    /* does not return */

    return(0);
}

static void taskFxn(void* a0, void* a1)
{

    /* Initialize SCI Client - It must be called before board init */
    ipc_initSciclient();

#if (defined (BUILD_MCU1_0) && (defined (SOC_J721E) || defined (SOC_J7200)))
    TaskP_Handle sciserverInitTask;
    TaskP_Params sciserverInitTaskParams;

    /* Initialize SCI Client Server */
    TaskP_Params_init(&sciserverInitTaskParams);
    sciserverInitTaskParams.priority     = IPC_INIT_SCISERVER_TASK_PRI;
    sciserverInitTaskParams.stack        = gSciserverInitTskStack;
    sciserverInitTaskParams.stacksize    = sizeof (gSciserverInitTskStack);

    sciserverInitTask = TaskP_create(Ipc_setupSciServer, &sciserverInitTaskParams);
    if(NULL == sciserverInitTask)
    {
        OS_stop();
    }
#endif

#if !defined(A72_LINUX_OS)
    ipc_boardInit();
#endif

#if defined (__C7100__) ||  defined (_TMS320C6X)
    ipc_timerInterruptInit();
#endif

#ifdef IPC_NEGATIVE_TEST
    Ipc_echo_neg_test();
#else
    Ipc_echo_test();
#endif
}

#if defined(__C7100__)
/* The C7x CLEC should be programmed to allow config/re config either in secure
 * OR non secure mode. This function configures all inputs to given level
 *
 * Instance is hard-coded for J721e only
 *
 */
void IpcCfgClecAccessCtrl (Bool onlyInSecure)
{
    CSL_ClecEventConfig cfgClec;
    CSL_CLEC_EVTRegs   *clecBaseAddr = (CSL_CLEC_EVTRegs*) CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE;
    uint32_t            i, maxInputs = 2048U;

    cfgClec.secureClaimEnable = onlyInSecure;
    cfgClec.evtSendEnable     = FALSE;
    cfgClec.rtMap             = CSL_CLEC_RTMAP_DISABLE;
    cfgClec.extEvtNum         = 0U;
    cfgClec.c7xEvtNum         = 0U;
    for(i = 0U; i < maxInputs; i++)
    {
        CSL_clecConfigEvent(clecBaseAddr, i, &cfgClec);
    }
}

static void IpcInitMmu(Bool isSecure)
{
    Mmu_MapAttrs    attrs;

    Mmu_initMapAttrs(&attrs);
    attrs.attrIndx = Mmu_AttrIndx_MAIR0;

    if(TRUE == isSecure)
    {
        attrs.ns = 0;
    }
    else
    {
        attrs.ns = 1;
    }

    /* Register region */
    (void)Mmu_map(0x00000000U, 0x00000000U, 0x20000000U, &attrs, isSecure);
    (void)Mmu_map(0x20000000U, 0x20000000U, 0x20000000U, &attrs, isSecure);
    (void)Mmu_map(0x40000000U, 0x40000000U, 0x20000000U, &attrs, isSecure);
    (void)Mmu_map(0x60000000U, 0x60000000U, 0x10000000U, &attrs, isSecure);
    (void)Mmu_map(0x78000000U, 0x78000000U, 0x08000000U, &attrs, isSecure); /* CLEC */

    attrs.attrIndx = Mmu_AttrIndx_MAIR7;
    (void)Mmu_map(0x80000000U, 0x80000000U, 0x20000000U, &attrs, isSecure); /* DDR */
    (void)Mmu_map(0xA0000000U, 0xA0000000U, 0x20000000U, &attrs, isSecure); /* DDR */
    (void)Mmu_map(0x70000000U, 0x70000000U, 0x00800000U, &attrs, isSecure); /* MSMC - 8MB */
    (void)Mmu_map(0x41C00000U, 0x41C00000U, 0x00080000U, &attrs, isSecure); /* OCMC - 512KB */

    /*
     * DDR range 0xA0000000 - 0xAA000000 : Used as RAM by multiple
     * remote cores, no need to mmp_map this range.
     * IPC VRing Buffer - uncached
     */
    attrs.attrIndx =  Mmu_AttrIndx_MAIR4;
    (void)Mmu_map(0xAA000000U, 0xAA000000U, 0x02000000U, &attrs, isSecure);
    (void)Mmu_map(0xA8000000U, 0xA8000000U, 0x00100000U, &attrs, isSecure);

    return;
}

void InitMmu(void)
{
    IpcInitMmu(FALSE);
    IpcInitMmu(TRUE);

    /* Setup CLEC access/configure in non-secure mode */
    IpcCfgClecAccessCtrl(FALSE);
}
#endif

#if defined(BUILD_MPU)
extern void Osal_initMmuDefault(void);
void InitMmu(void)
{
    Osal_initMmuDefault();
}
#endif

#if (defined (BUILD_MCU1_0) && (defined (SOC_J721E) || defined (SOC_J7200)))
void Ipc_setupSciServer(void)
{

    Sciserver_TirtosCfgPrms_t appPrms;
    int32_t ret = CSL_PASS;

    ret = Sciserver_tirtosInitPrms_Init(&appPrms);

    appPrms.taskPriority[SCISERVER_TASK_USER_LO] =
                                            IPC_SETUP_SCISERVER_TASK_PRI_LOW;
    appPrms.taskPriority[SCISERVER_TASK_USER_HI] =
                                            IPC_SETUP_SCISERVER_TASK_PRI_HIGH;

    if (ret == CSL_PASS)
    {
        ret = Sciserver_tirtosInit(&appPrms);
    }

    if (ret == CSL_PASS)
    {
        App_printf("Starting Sciserver..... PASSED\n");
    }
    else
    {
        App_printf("Starting Sciserver..... FAILED\n");
    }

    return;
}
#endif
