/*
 *  Copyright (C) 2020-2021 Texas Instruments Incorporated
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
 *  \file sciserver_testapp_main.c
 *
 *  \brief Example Application for sciserver
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/board/board.h>
#include <ti/drv/sciclient/sciserver_tirtos.h>
#include <ti/drv/sciclient/examples/common/sciclient_appCommon.h>
#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Aligned address at which the X509 header is placed. */
#define SCISERVER_COMMON_X509_HEADER_ADDR (0x41cffb00)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    int32_t ret = CSL_PASS;
    Sciclient_ConfigPrms_t clientPrms;
    Sciserver_TirtosCfgPrms_t appPrms;

    OS_init();

    /* Sciclient needs to be initialized before Sciserver. Sciserver depends on
     * Sciclient API to execute message forwarding */
    ret = Sciclient_configPrmsInit(&clientPrms);

    if (ret == CSL_PASS)
    {
        ret = Sciclient_boardCfgParseHeader(
            (uint8_t *) SCISERVER_COMMON_X509_HEADER_ADDR,
            &clientPrms.inPmPrms, &clientPrms.inRmPrms);
    }
    
    if (ret == CSL_PASS)
    {
        ret = Sciclient_init(&clientPrms);
    }

    if (ret == CSL_PASS)
    {
        ret = Sciserver_tirtosInitPrms_Init(&appPrms);
    }

    if (ret == CSL_PASS)
    {
        ret = Sciserver_tirtosInit(&appPrms);
    }

    /* Enable UART console print*/
    if (ret == CSL_PASS)
    {
        App_sciclientConsoleInit();
    }

   App_sciclientPrintf("Sciserver Built On: %s %s\n", __DATE__, __TIME__);
    if (ret == CSL_PASS)
    {
       App_sciclientPrintf("Starting Sciserver..... PASSED\n");

        uint32_t freqHz;
        Sciclient_pmGetModuleClkFreq(TISCI_DEV_GTC0, TISCI_DEV_GTC0_GTC_CLK,
            (uint64_t *) &freqHz, SCICLIENT_SERVICE_WAIT_FOREVER);
       App_sciclientPrintf("GTC freq: %d\n", freqHz);

        OS_start();
    }
    else
    {
        App_sciclientPrintf("Starting Sciserver..... FAILED\n");
    }
    return ret;
}

/* ========================================================================== */
/*                 Internal Function Definitions                              */
/* ========================================================================== */

/* None */

