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
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * \file     enet_board_pinmux.c
 *
 * \brief    This file contains the CPSW5G/CPSW9G specific pinmux functions for J721E EVM and GESI board.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/csl/soc.h>
#include <ti/board/board.h>
#if defined (SOC_J721E)
#include <ti/board/src/j721e_evm/include/board_pinmux.h>
#elif defined (SOC_J7200)
#include <ti/board/src/j7200_evm/include/board_pinmux.h>
#else
#error "Invalid SOC"
#endif

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

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

extern pinmuxBoardCfg_t gEthFwPinmuxData[];

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t EnetBoard_configEthFwPinmux (void)
{
    int32_t status = BOARD_SOK;

#if defined(BUILD_MCU2_0) && defined(BOARD_D3_BB)
    Board_pinmuxUpdate(gJ721E_MainPinmuxDataD3BBCpsw9g, BOARD_SOC_DOMAIN_MAIN);
    Board_pinmuxUpdate(gJ721E_WkupPinmuxDataD3BBCpsw9g, BOARD_SOC_DOMAIN_WKUP);
#elif defined(BUILD_MCU2_0) && defined(SOC_J721E)
    Board_pinmuxUpdate(gEthFwPinmuxData,
                       BOARD_SOC_DOMAIN_MAIN);
    /* Overwrite the ICSSG RGMII muc configurations with CPSW */
    /* Below is CPSW9G QSGMII pinmux configuration,
       RGMII Ports - 1,3,4,8. QSGMII ports - 2,5,6,7 */
    Board_pinmuxUpdate(gJ721E_MainPinmuxDataGesiCpsw9gQsgmii,
                       BOARD_SOC_DOMAIN_MAIN);
#elif defined(BUILD_MCU2_0) && defined(SOC_J7200)
    /* Below is CPSW5G QSGMII pinmux configuration,
       RGMII Ports - 1. QSGMII ports - 2,3,4 */
    Board_pinmuxUpdate(gJ7200_MainPinmuxDataCpsw,
                       BOARD_SOC_DOMAIN_MAIN);
    Board_pinmuxUpdate(gJ7200_WkupPinmuxDataCpsw ,
                       BOARD_SOC_DOMAIN_MAIN);
#endif

    return status;
}
