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
 * \file     enet_appsoc.h
 *
 * \brief    This file contains the CPSW low-level driver SOC specific utility
 *           function implementations for J721E/J7200 devices.
 */

#ifndef ENETAPP_SOC_H_
#define ENETAPP_SOC_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/enet/enet.h>
#if defined (SOC_J721E)
#include <ti/board/src/j721e_evm/include/board_control.h>
#elif defined (SOC_J7200)
#include <ti/board/src/j7200_evm/include/board_control.h>
#else
#error "Invalid SOC"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define ENET_APP_SOC_TSYNC_ROUTER_NUM_INPUT                     (56U)
#define ENET_APP_SOC_TSYNC_ROUTER_NUM_OUTPUT                    (48U)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief  CPTS Clock Select Mux enum
 */
typedef enum
{
    ENETAPPUTILS_CPTS_CLKSEL_MAIN_PLL3_HSDIV1_CLKOUT = 0x0U,
    /**< Main PLL3 HSDIV1 Clockout for CPTS*/
    ENETAPPUTILS_CPTS_CLKSEL_MAIN_PLL0_HSDIV6_CLKOUT = 0x1U,
    /**< Main PLL0 HSDIV6 Clockout for CPTS*/
    ENETAPPUTILS_CPTS_CLKSEL_MCU_CPTS_REF_CLK        = 0x2U,
    /**< MCU CPTS Reference Clockout*/
    ENETAPPUTILS_CPTS_CLKSEL_CPTS_RFT_CLK            = 0x3U,
    /**< CPTS RFT Clock */
    ENETAPPUTILS_CPTS_CLKSEL_MCU_EXT_REFCLK0         = 0x4U,
    /**< MCU External Reference Clock0*/
    ENETAPPUTILS_CPTS_CLKSEL_EXT_REFCLK1             = 0x5U,
    /**< External Reference Clock 1 for CPTS*/
    ENETAPPUTILS_CPTS_CLKSEL_SERDES0_IP2_LN0_TXMCLK  = 0x6U,
    /**< SERDES0 IP2 Lane0 Tx MCLK*/
    ENETAPPUTILS_CPTS_CLKSEL_SERDES0_IP2_LN1_TXMCLK  = 0x7U,
    /**< SERDES0 IP2 Lane1 Tx MCLK*/
    ENETAPPUTILS_CPTS_CLKSEL_SERDES1_IP2_LN0_TXMCLK  = 0x8U,
    /**< SERDES1 IP2 Lane0 Tx MCLK*/
    ENETAPPUTILS_CPTS_CLKSEL_SERDES1_IP2_LN1_TXMCLK  = 0x9U,
    /**< SERDES1 IP2 Lane1 Tx MCLK*/
    ENETAPPUTILS_CPTS_CLKSEL_SERDES2_IP2_LN0_TXMCLK  = 0xAU,
    /**< SERDES2 IP2 Lane0 Tx MCLK*/
    ENETAPPUTILS_CPTS_CLKSEL_SERDES2_IP2_LN1_TXMCLK  = 0xBU,
    /**< SERDES2 IP2 Lane1 Tx MCLK*/
    ENETAPPUTILS_CPTS_CLKSEL_SERDES3_IP2_LN0_TXMCLK  = 0xCU,
    /**< SERDES3 IP2 Lane0 Tx MCLK*/
    ENETAPPUTILS_CPTS_CLKSEL_SERDES3_IP2_LN1_TXMCLK  = 0xDU,
    /**< SERDES3 IP2 Lane1 Tx MCLK*/
    ENETAPPUTILS_CPTS_CLKSEL_MCU_PLL2_HSDIV1_CLKOUT  = 0xEU,
    /**< MCU PLL2 HSDIV1 Clockout*/
    ENETAPPUTILS_CPTS_CLKSEL_MAIN_SYSCLK0            = 0xFU,
    /**< Main System Clock*/
} EnetAppUtils_CptsClkSelMux;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* ENETAPP_SOC_H_ */
