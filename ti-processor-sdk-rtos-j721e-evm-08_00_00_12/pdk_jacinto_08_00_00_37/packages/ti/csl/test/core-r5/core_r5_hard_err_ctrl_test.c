/* Copyright (c) Texas Instruments Incorporated 2019
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
 *  \file     core_r5_hard_err_ctrl.c
 *
 *  \brief    This file contains hard error control test code for R5 core.
 *
 *  \details  Hard error related register read/write and configuration tests
 **/

/*===========================================================================*/
/*                         Include files                                     */
/*===========================================================================*/
#include <core_r5_test.h>

/*===========================================================================*/
/*                         Declarations                                      */
/*===========================================================================*/


/*===========================================================================*/
/*                         Macros                                            */
/*===========================================================================*/

/*===========================================================================*/
/*                         Internal function declarations                    */
/*===========================================================================*/

/*===========================================================================*/
/*                         Global Variables                                  */
/*===========================================================================*/

/*===========================================================================*/
/*                   Local Function definitions                              */
/*===========================================================================*/


/*===========================================================================*/
/*                        Test Function definitions                          */
/*===========================================================================*/

/* PDK-6037: This test demonstrates the APIs to enable hard error cache
 */
int32_t cslcore_r5_hardErrTest(void)
{
    /* Declarations of variables */
    int32_t    testResult = CSL_APP_TEST_PASS;
    uint32_t   atcmRegionReg, btcmRegionReg;

    /* Read the ATCM and BTCM register values and write it back */
    atcmRegionReg = CSL_armR5ReadATCMRegionRegister();
    btcmRegionReg = CSL_armR5ReadBTCMRegionRegister();
    CSL_armR5WriteATCMRegionRegister(atcmRegionReg);
    CSL_armR5WriteBTCMRegionRegister(btcmRegionReg);

    /* disable the hard error Cache */
    CSL_armR5CacheEnableHardErrCache(FALSE);

    /* Enable the hard error cahce for TCM */
    CSL_armR5CacheEnableHardErrCache(TRUE);

   return (testResult);
}


/* Nothing past this point */
