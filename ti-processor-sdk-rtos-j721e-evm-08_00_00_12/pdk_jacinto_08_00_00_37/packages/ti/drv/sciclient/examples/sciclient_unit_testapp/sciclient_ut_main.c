/*
 *  Copyright (C) 2018-2019 Texas Instruments Incorporated
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
 *
 */

/**
 *  \file sciclient_ut_main.c
 *
 *  \brief Implementation of Sciclient Unit Test
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/osal/osal.h>
#include <ti/osal/TimerP.h>
#include <ti/osal/TaskP.h>

#include <stdint.h>
#include <string.h>
#include <ti/csl/tistdtypes.h>
#include <ti/csl/soc.h>
#include <ti/csl/arch/csl_arch.h>
#include <ti/csl/hw_types.h>
#include <ti/drv/sciclient/examples/common/sciclient_appCommon.h>

#include <ti/drv/sciclient/examples/sciclient_unit_testapp/sciclient_ut_tests.h>
#if defined (__C7100__)
#include <ti/csl/csl_clec.h>
#endif

/* This is only needed as this test case is running back to back Sciclient
 * Init and de-inits.
 */
#include <ti/drv/sciclient/src/sciclient/sciclient_priv.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
void mainTask(void* arg0, void* arg1);
static int32_t App_getRevisionTestPol(void);
static int32_t App_getRevisionTestIntr(void);
static int32_t App_timeoutTest(void);
static int32_t App_invalidReqPrmTest(void);
#if defined (SOC_AM65XX) || defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2)
static int32_t App_msmcQueryTest(void);
#endif
#if defined(SOC_AM65XX)
static int32_t App_rmGetResourceRange(void);
#endif

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static volatile int32_t gTestStatus;
static uint8_t  gAppTskStackMain[16*1024] __attribute__((aligned(8192)));;
/* IMPORTANT NOTE: For C7x,
 * - stack size and stack ptr MUST be 8KB aligned
 * - AND min stack size MUST be 16KB
 * - AND stack assigned for task context is "size - 8KB"
 *       - 8KB chunk for the stack area is used for interrupt handling in this task context
 */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    TaskP_Handle task;
    TaskP_Params taskParams;

    App_SciclientC7xPreInit();

    uint32_t retVal = CSL_PASS;
    #if defined (__C7100__)
    /* 256 CLEC interrupt number mapped to interrupt number 14 from BIOS for
     * timer.
     */
    {
        CSL_CLEC_EVTRegs * regs = (CSL_CLEC_EVTRegs *) CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE;
        CSL_ClecEventConfig evtCfg;
        evtCfg.secureClaimEnable = 0;
        evtCfg.evtSendEnable = 1;
        evtCfg.rtMap = 0x3C;
        evtCfg.extEvtNum = 0x0;
        evtCfg.c7xEvtNum = 14;
        /* Clec interrupt number 1024 is connected to GIC interrupt number 32 in J721E.
        * Due to this for CLEC programming one needs to add an offset of 992 (1024 - 32)
        * to the event number which is shared between GIC and CLEC.
        */
        CSL_clecConfigEvent(regs, 256 + 992, &evtCfg);
    }
    #endif

    OS_init();

    TaskP_Params_init(&taskParams);
    taskParams.stack        = gAppTskStackMain;
    taskParams.stacksize    = sizeof (gAppTskStackMain);
    task = TaskP_create(mainTask, &taskParams);
    if(NULL==task)
    {
        OS_stop();
    }

    OS_start();

    return retVal;
}

void mainTask(void* arg0, void* arg1)
{
    /*To suppress unused variable warning*/
    (void)arg0;
    (void)arg1;

    volatile uint32_t loopForever = 1U;
    
    App_sciclientParser();

    while(loopForever);
}
uint32_t App_sciclientGetNumTests(void)
{
    return SCICLIENT_NUM_TESTCASES;
}

int32_t App_sciclientTestMain(App_sciclientTestParams_t *testParams)
{
    switch (testParams->testcaseId)
    {
        case 1:
            testParams->testResult = App_getRevisionTestPol();
            break;
        case 2:
            testParams->testResult = App_getRevisionTestIntr();
            break;
        case 3:
            testParams->testResult = App_invalidReqPrmTest();
            break;
        case 4:
            testParams->testResult = App_timeoutTest();
            break;
#if defined (SOC_AM65XX) || defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2)
        case 5:
            testParams->testResult = App_msmcQueryTest();
            break;
#endif
#if defined (SOC_AM65XX)
        case 6:
            testParams->testResult = App_rmGetResourceRange();
            break;
#endif
        default:
            break;
    }
    return 0;
}

/* ========================================================================== */
/*                 Internal Function Definitions                              */
/* ========================================================================== */

/* This is only needed as this test case is running back to back Sciclient
 * Init and de-inits.
 */
extern Sciclient_ServiceHandle_t gSciclientHandle;

int32_t App_getRevisionTestPol(void)
{
    int32_t status = CSL_EFAIL;
    Sciclient_ConfigPrms_t        config =
    {
        SCICLIENT_SERVICE_OPERATION_MODE_POLLED,
        NULL,
        0 /* isSecure = 0 un secured for all cores */
    };
    struct tisci_msg_version_req request;
    const Sciclient_ReqPrm_t      reqPrm =
    {
        TISCI_MSG_VERSION,
        TISCI_MSG_FLAG_AOP,
        (uint8_t *) &request,
        sizeof(request),
        SCICLIENT_SERVICE_WAIT_FOREVER
    };

    struct tisci_msg_version_resp response;
    Sciclient_RespPrm_t           respPrm =
    {
        0,
        (uint8_t *) &response,
        sizeof (response)
    };
    /* This is only needed as this test case is running back to back Sciclient
     * Init and de-inits.
     */
    while (gSciclientHandle.initCount != 0)
    {
        status = Sciclient_deinit();
    }
    
    status = Sciclient_init(&config);

    if (status == CSL_PASS)
    {
        status = Sciclient_service(&reqPrm, &respPrm);

        if (CSL_PASS == status)
        {
            if (respPrm.flags == TISCI_MSG_FLAG_ACK)
            {
                status = CSL_PASS;
                App_sciclientPrintf(
                                  " DMSC Firmware Version %s\n",
                                  (char *) response.str);
                App_sciclientPrintf(
                                  " Firmware revision 0x%x\n", response.version);
                App_sciclientPrintf(
                                  " ABI revision %d.%d\n", response.abi_major,
                                  response.abi_minor);
            }
            else
            {
                App_sciclientPrintf(
                                  " DMSC Firmware Get Version failed \n");
            }
        }
        else
        {
            App_sciclientPrintf(
                              " DMSC Firmware Get Version failed \n");
        }
    }
    if (status == CSL_PASS)
    {
        status = Sciclient_deinit();
    }
    return status;
}

int32_t App_getRevisionTestIntr(void)
{
    int32_t status = CSL_EFAIL;
    Sciclient_ConfigPrms_t        config =
    {
        SCICLIENT_SERVICE_OPERATION_MODE_INTERRUPT,
        NULL,
        0 /* isSecure = 0 un secured for all cores */
    };

    struct tisci_msg_version_req request;
    const Sciclient_ReqPrm_t      reqPrm =
    {
        TISCI_MSG_VERSION,
        TISCI_MSG_FLAG_AOP,
        (uint8_t *) &request,
        sizeof(request),
        SCICLIENT_SERVICE_WAIT_FOREVER
    };
    struct tisci_msg_version_resp response;
    Sciclient_RespPrm_t           respPrm =
    {
        0,
        (uint8_t *) &response,
        sizeof (response)
    };
    /* This is only needed as this test case is running back to back Sciclient
     * Init and de-inits.
     */
    while (gSciclientHandle.initCount != 0)
    {
        status = Sciclient_deinit();
    }
    
    status = Sciclient_init(&config);
    if (status == CSL_PASS)
    {
        status = Sciclient_service(&reqPrm, &respPrm);
        if (CSL_PASS == status)
        {
            if (respPrm.flags == TISCI_MSG_FLAG_ACK)
            {
                status = CSL_PASS;
                App_sciclientPrintf(
                                  " DMSC Firmware Version %s\n",
                                  (char *) response.str);
                App_sciclientPrintf(
                                  " Firmware revision 0x%x\n", response.version);
                App_sciclientPrintf(
                                  " ABI revision %d.%d\n", response.abi_major,
                                  response.abi_minor);
            }
            else
            {
                App_sciclientPrintf(
                                  " DMSC Firmware Get Version failed \n");
            }
        }
        else
        {
            App_sciclientPrintf(
                              " DMSC Firmware Get Version failed \n");
        }
    }
    if (status == CSL_PASS)
    {
        status = Sciclient_deinit();
    }
    return status;
}

static int32_t App_invalidReqPrmTest(void)
{
    int32_t status = CSL_EFAIL;

    Sciclient_ConfigPrms_t        config =
    {
        SCICLIENT_SERVICE_OPERATION_MODE_POLLED,
        NULL,
        0 /* isSecure = 0 un secured for all cores */
    };
    struct tisci_msg_version_req request;
    const Sciclient_ReqPrm_t      reqPrm_badTxSize =
    {
        TISCI_MSG_VERSION,
        TISCI_MSG_FLAG_AOP,
        (uint8_t*) &request,
        100,
        SCICLIENT_SERVICE_WAIT_FOREVER
    };

    const Sciclient_ReqPrm_t      reqPrm_good =
    {
        TISCI_MSG_VERSION,
        TISCI_MSG_FLAG_AOP,
        (uint8_t*)&request,
        0,
        SCICLIENT_SERVICE_WAIT_FOREVER
    };

    struct tisci_msg_version_resp response;
    Sciclient_RespPrm_t           respPrm_badRxsize =
    {
        0,
        (uint8_t *) &response,
        100
    };

    Sciclient_RespPrm_t           respPrm_good =
    {
        0,
        (uint8_t *) &response,
        sizeof (response)
    };
    /* This is only needed as this test case is running back to back Sciclient
     * Init and de-inits.
     */
    while (gSciclientHandle.initCount != 0)
    {
        status = Sciclient_deinit();
    }
    status = Sciclient_init(&config);
    if (status == CSL_PASS)
    {
        status = Sciclient_service(NULL, &respPrm_good);
        if (CSL_EBADARGS == status)
        {
            status = CSL_PASS;
            App_sciclientPrintf(" NULL Arg Test PASSED \n");
        }
        else
        {
            App_sciclientPrintf(" NULL Arg Test FAILED \n");
        }
        status = Sciclient_service(&reqPrm_badTxSize, &respPrm_good);
        if (CSL_EBADARGS == status)
        {
            status = CSL_PASS;
            App_sciclientPrintf(" Tx Payload Check PASSED \n");
        }
        else
        {
            App_sciclientPrintf(" Tx Payload Check FAILED \n");
        }
        status = Sciclient_service(&reqPrm_good, &respPrm_badRxsize);
        if (CSL_EBADARGS == status)
        {
            status = CSL_PASS;
            App_sciclientPrintf(" Rx Payload Check PASSED \n");
        }
        else
        {
            App_sciclientPrintf(" Rx Payload Check FAILED \n");
        }
    }
    if (status == CSL_PASS)
    {
        status = Sciclient_deinit();
    }
    return status;
}

static int32_t App_timeoutTest(void)
{
    int32_t status = CSL_EFAIL;
    Sciclient_ConfigPrms_t        config =
    {
        SCICLIENT_SERVICE_OPERATION_MODE_POLLED,
        NULL,
        0 /* isSecure = 0 un secured for all cores */
    };
    struct tisci_msg_version_req request;
    const Sciclient_ReqPrm_t      reqPrm =
    {
        TISCI_MSG_VERSION,
        TISCI_MSG_FLAG_AOP,
        (uint8_t *)&request,
        sizeof(request),
        (uint32_t)0x0A
    };
    struct tisci_msg_version_resp response;
    Sciclient_RespPrm_t           respPrm =
    {
        0,
        (uint8_t *) &response,
        sizeof (response)
    };
    /* This is only needed as this test case is running back to back Sciclient
     * Init and de-inits.
     */
    while (gSciclientHandle.initCount != 0)
    {
        status = Sciclient_deinit();
    }
    status = Sciclient_init(&config);
    if (status == CSL_PASS)
    {
        status = Sciclient_service(&reqPrm, &respPrm);
        if (CSL_ETIMEOUT == status)
        {
            status = CSL_PASS;
            App_sciclientPrintf(
                             " Timeout test PASSED \n");
        }
        else
        {
            App_sciclientPrintf(
                             " Timeout Test FAILED \n");
        }
    }
    if (status == CSL_PASS)
    {
        status = Sciclient_deinit();
    }
    return status;
}

#if defined (SOC_AM65XX) || defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2)
static int32_t App_msmcQueryTest(void)
{
    int32_t status = CSL_EFAIL;
    Sciclient_ConfigPrms_t        config =
    {
        SCICLIENT_SERVICE_OPERATION_MODE_POLLED,
        NULL,
        0 /* isSecure = 0 un secured for all cores */
    };

    const struct tisci_query_msmc_req req = {};
    struct tisci_query_msmc_resp resp;
    /* This is only needed as this test case is running back to back Sciclient
     * Init and de-inits.
     */
    while (gSciclientHandle.initCount != 0)
    {
        status = Sciclient_deinit();
    }
    status = Sciclient_init(&config);
    if (status == CSL_PASS)
    {
        status = Sciclient_msmcQuery(&req, &resp, SCICLIENT_SERVICE_WAIT_FOREVER);
    }
    if (status == CSL_PASS)
    {
        App_sciclientPrintf("MSMC Start Address = 0x%X%X\n", resp.msmc_start_high, resp.msmc_start_low);
        App_sciclientPrintf("MSMC End Address = 0x%X%X\n", resp.msmc_end_high, resp.msmc_end_low);
    }
    else
    {
        App_sciclientPrintf("MSMC Query FAILED !!");
    }
    if (status == CSL_PASS)
    {
        status = Sciclient_deinit();
    }
    return status;
}
#endif

#if defined (SOC_AM65XX)
struct appSciclientRmGetRangeTest
{
    uint16_t           type;
    uint8_t            subtype;
    int32_t            expectedResult;
};

struct appSciclientRmGetRangeTest gAppSciclientTstParams [] =
{
    {TISCI_DEV_NAVSS0_MODSS_INTA0, TISCI_RESASG_SUBTYPE_GLOBAL_EVENT_SEVT, CSL_PASS},
    {TISCI_DEV_NAVSS0_MODSS_INTA0, TISCI_RESASG_SUBTYPE_IA_VINT, CSL_PASS},
    {TISCI_DEV_NAVSS0_MODSS_INTA1, TISCI_RESASG_SUBTYPE_GLOBAL_EVENT_SEVT, CSL_PASS},
    {TISCI_DEV_NAVSS0_MODSS_INTA1, TISCI_RESASG_SUBTYPE_IA_VINT, CSL_PASS},
    {TISCI_DEV_NAVSS0_UDMASS_INTA0, TISCI_RESASG_SUBTYPE_GLOBAL_EVENT_SEVT, CSL_PASS},
    {TISCI_DEV_NAVSS0_UDMASS_INTA0, TISCI_RESASG_SUBTYPE_IA_VINT, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_INTR_AGGR_0, TISCI_RESASG_SUBTYPE_GLOBAL_EVENT_SEVT, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_INTR_AGGR_0, TISCI_RESASG_SUBTYPE_IA_VINT, CSL_PASS},
    {TISCI_DEV_NAVSS0_PROXY0, TISCI_RESASG_SUBTYPE_PROXY_PROXIES, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_PROXY0, TISCI_RESASG_SUBTYPE_PROXY_PROXIES, CSL_PASS},
    {TISCI_DEV_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_UDMAP_TX_H, CSL_PASS},
    {TISCI_DEV_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_UDMAP_TX, CSL_PASS},
    {TISCI_DEV_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_UDMAP_TX_EXT, CSL_PASS},
    {TISCI_DEV_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_UDMAP_RX_H, CSL_PASS},
    {TISCI_DEV_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_UDMAP_RX, CSL_PASS},
    {TISCI_DEV_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_GP, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_UDMAP_TX_H, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_UDMAP_TX, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_UDMAP_RX_H, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_UDMAP_RX, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_GP, CSL_PASS},
    {TISCI_DEV_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_ERROR_OES, CSL_PASS},
    {TISCI_DEV_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_VIRTID, CSL_PASS},
    {TISCI_DEV_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_MONITORS, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_ERROR_OES, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_VIRTID, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_RINGACC0, TISCI_RESASG_SUBTYPE_RA_MONITORS, CSL_PASS},
    {TISCI_DEV_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_UDMAP_TX_HCHAN, CSL_PASS},
    {TISCI_DEV_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_UDMAP_TX_CHAN, CSL_PASS},
    {TISCI_DEV_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_UDMAP_TX_ECHAN, CSL_PASS},
    {TISCI_DEV_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_UDMAP_RX_HCHAN, CSL_PASS},
    {TISCI_DEV_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_UDMAP_RX_CHAN, CSL_PASS},
    {TISCI_DEV_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_GLOBAL_EVENT_TRIGGER, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_UDMAP_TX_HCHAN, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_UDMAP_TX_CHAN, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_UDMAP_RX_HCHAN, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_UDMAP_RX_CHAN, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_GLOBAL_EVENT_TRIGGER, CSL_PASS},
    {TISCI_DEV_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_UDMAP_RX_FLOW_COMMON, CSL_PASS},
    {TISCI_DEV_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_UDMAP_INVALID_FLOW_OES, CSL_PASS},
    {TISCI_DEV_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_UDMAP_GLOBAL_CONFIG, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_UDMAP_RX_FLOW_COMMON, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_UDMAP_INVALID_FLOW_OES, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_UDMAP0, TISCI_RESASG_SUBTYPE_UDMAP_GLOBAL_CONFIG, CSL_PASS},
    {TISCI_DEV_CMPEVENT_INTRTR0, TISCI_RESASG_SUBTYPE_IR_OUTPUT, CSL_PASS},
    {TISCI_DEV_MAIN2MCU_LVL_INTRTR0, TISCI_RESASG_SUBTYPE_IR_OUTPUT, CSL_PASS},
    {TISCI_DEV_MAIN2MCU_LVL_INTRTR0, TISCI_RESASG_SUBTYPE_IR_OUTPUT, CSL_PASS},
    {TISCI_DEV_MAIN2MCU_PLS_INTRTR0, TISCI_RESASG_SUBTYPE_IR_OUTPUT, CSL_PASS},
    {TISCI_DEV_GPIOMUX_INTRTR0, TISCI_RESASG_SUBTYPE_IR_OUTPUT, CSL_PASS},
    {TISCI_DEV_TIMESYNC_INTRTR0, TISCI_RESASG_SUBTYPE_IR_OUTPUT, CSL_PASS},
    {TISCI_DEV_WKUP_GPIOMUX_INTRTR0, TISCI_RESASG_SUBTYPE_IR_OUTPUT, CSL_PASS},
    {TISCI_DEV_NAVSS0_INTR_ROUTER_0, TISCI_RESASG_SUBTYPE_IR_OUTPUT, CSL_PASS},
    {TISCI_DEV_NAVSS0_INTR_ROUTER_0, TISCI_RESASG_SUBTYPE_IR_OUTPUT, CSL_PASS},
    {TISCI_DEV_NAVSS0_INTR_ROUTER_0, TISCI_RESASG_SUBTYPE_IR_OUTPUT, CSL_PASS},
    {TISCI_DEV_NAVSS0_INTR_ROUTER_0, TISCI_RESASG_SUBTYPE_IR_OUTPUT, CSL_PASS},
    {TISCI_DEV_NAVSS0_INTR_ROUTER_0, TISCI_RESASG_SUBTYPE_IR_OUTPUT, CSL_PASS},
    {TISCI_DEV_NAVSS0_INTR_ROUTER_0, TISCI_RESASG_SUBTYPE_IR_OUTPUT, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_INTR_ROUTER_0, TISCI_RESASG_SUBTYPE_IR_OUTPUT, CSL_PASS},
    {TISCI_DEV_MCU_NAVSS0_INTR_ROUTER_0, TISCI_RESASG_SUBTYPE_IR_OUTPUT, CSL_PASS}
};

static int32_t App_rmGetResourceRange(void)
{
    int32_t status = CSL_EFAIL;
    uint32_t i = 0U;
    Sciclient_ConfigPrms_t        config =
    {
        SCICLIENT_SERVICE_OPERATION_MODE_POLLED,
        NULL,
        0 /* isSecure = 0 un secured for all cores */
    };
    /* This is only needed as this test case is running back to back Sciclient
     * Init and de-inits.
     */
    while (gSciclientHandle.initCount != 0)
    {
        status = Sciclient_deinit();
    }
    status = Sciclient_init(&config);
    
    for (i = 0U; i < sizeof (gAppSciclientTstParams)/sizeof (struct appSciclientRmGetRangeTest); i++)
    {
        struct tisci_msg_rm_get_resource_range_resp res;
        struct tisci_msg_rm_get_resource_range_req  req;
        if (status == CSL_PASS)
        {
            req.type           = gAppSciclientTstParams[i].type;
            req.subtype        = gAppSciclientTstParams[i].subtype;
            req.secondary_host = TISCI_MSG_VALUE_RM_UNUSED_SECONDARY_HOST;

            /* Get interrupt number range */
            status =  Sciclient_rmGetResourceRange(
                        &req,
                        &res,
                        0xFFFFFFFFU);
        }
        if (status == CSL_PASS)
        {
            App_sciclientPrintf("\n\rType: %d, Subtype : %d, Start: %d, Num: %d",
                                gAppSciclientTstParams[i].type,
                                gAppSciclientTstParams[i].subtype,
                                res.range_start,
                                res.range_num);
        }
        else
        {
            if (status == gAppSciclientTstParams[i].expectedResult)
            {
                App_sciclientPrintf("\n\rType: %d, Subtype : %d, Expected Failure",
                                gAppSciclientTstParams[i].type,
                                gAppSciclientTstParams[i].subtype);
                status = CSL_PASS;
            }
            else
            {
                App_sciclientPrintf("\n\rType: %d, Subtype : %d, Unexpected Failure!!!!!!!",
                                gAppSciclientTstParams[i].type,
                                gAppSciclientTstParams[i].subtype);
            }
        }
    }
    
    if (status == CSL_PASS)
    {
        status = Sciclient_deinit();
    }
    return status;
}
#endif

#if defined(BUILD_MPU) || defined (__C7100__)
extern void Osal_initMmuDefault(void);
void InitMmu(void)
{
    Osal_initMmuDefault();
}
#endif



