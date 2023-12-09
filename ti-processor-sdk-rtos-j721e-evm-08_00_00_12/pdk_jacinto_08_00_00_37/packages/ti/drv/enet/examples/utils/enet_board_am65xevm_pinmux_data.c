/*
 *  Copyright (c) Texas Instruments Incorporated 2021
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
 * \file  enet_board_am65xevm_pinmux_data.c
 *
 * \brief This file contains pinmux data for the AM65xx MII custom board.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/board/src/am65xx_idk/include/pinmux.h>
#include <ti/board/src/am65xx_idk/am65xx_idk_pinmux.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static pinmuxPerCfg_t gPru_icssg0_mii_g_rt0PinCfg[] =
{
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii_mt0_clk -> AC24 */
    {
        PIN_PRG0_PRU1_GPO16, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii0_txen -> AE27 */
    {
        PIN_PRG0_PRU1_GPO15, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii0_txd3 -> AD24 */
    {
        PIN_PRG0_PRU1_GPO14, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii0_txd2 -> AD25 */
    {
        PIN_PRG0_PRU1_GPO13, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii0_txd1 -> AC25 */
    {
        PIN_PRG0_PRU1_GPO12, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii0_txd0 -> AB24 */
    {
        PIN_PRG0_PRU1_GPO11, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii0_rxdv -> Y24 */
    {
        PIN_PRG0_PRU0_GPO4, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii_mr0_clk -> Y25 */
    {
        PIN_PRG0_PRU0_GPO6, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii0_rxd3 -> AA27 */
    {
        PIN_PRG0_PRU0_GPO3, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii0_rxd2 -> W24 */
    {
        PIN_PRG0_PRU0_GPO2, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii0_rxer -> V28 */
    {
        PIN_PRG0_PRU0_GPO5, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii0_rxd1 -> W25 */
    {
        PIN_PRG0_PRU0_GPO1, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii0_rxd0 -> V24 */
    {
        PIN_PRG0_PRU0_GPO0, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii0_rxlink -> V27 */
    {
        PIN_PRG0_PRU0_GPO8, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii_mt1_clk -> AD28 */
    {
        PIN_PRG0_PRU0_GPO16, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii1_txen -> AA24 */
    {
        PIN_PRG0_PRU0_GPO15, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii1_txd3 -> AD26 */
    {
        PIN_PRG0_PRU0_GPO14, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii1_txd2 -> AC26 */
    {
        PIN_PRG0_PRU0_GPO13, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii1_txd1 -> AD27 */
    {
        PIN_PRG0_PRU0_GPO12, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii1_txd0 -> AB25 */
    {
        PIN_PRG0_PRU0_GPO11, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii1_rxdv -> AA25 */
    {
        PIN_PRG0_PRU1_GPO4, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii_mr1_clk -> AB27 */
    {
        PIN_PRG0_PRU1_GPO6, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii1_rxd3 -> AB26 */
    {
        PIN_PRG0_PRU1_GPO3, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii1_rxd2 -> AC27 */
    {
        PIN_PRG0_PRU1_GPO2, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii1_rxer -> U23 */
    {
        PIN_PRG0_PRU1_GPO5, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii1_rxd1 -> AC28 */
    {
        PIN_PRG0_PRU1_GPO1, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii1_rxd0 -> AB28 */
    {
        PIN_PRG0_PRU1_GPO0, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG0_MII_G_RT1 -> pr0_mii1_rxlink -> W27 */
    {
        PIN_PRG0_PRU1_GPO8, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    {PINMUX_END}
};

static pinmuxModuleCfg_t gPru_icssg0_mii_g_rtPinCfg[] =
{
    {0, TRUE, gPru_icssg0_mii_g_rt0PinCfg},
    {PINMUX_END}
};

static pinmuxPerCfg_t gPru_icssg1_mii_g_rt0PinCfg[] =
{
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii_mt0_clk -> AE19 */
    {
        PIN_PRG1_PRU1_GPO16, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii0_txen -> AG19 */
    {
        PIN_PRG1_PRU1_GPO15, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii0_txd3 -> AH19 */
    {
        PIN_PRG1_PRU1_GPO14, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii0_txd2 -> AF19 */
    {
        PIN_PRG1_PRU1_GPO13, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii0_txd1 -> AE20 */
    {
        PIN_PRG1_PRU1_GPO12, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii0_txd0 -> AC20 */
    {
        PIN_PRG1_PRU1_GPO11, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii0_rxdv -> AG23 */
    {
        PIN_PRG1_PRU0_GPO4, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii_mr0_clk -> AF22 */
    {
        PIN_PRG1_PRU0_GPO6, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii0_rxd3 -> AD21 */
    {
        PIN_PRG1_PRU0_GPO3, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii0_rxd2 -> AF23 */
    {
        PIN_PRG1_PRU0_GPO2, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii0_rxer -> AF27 */
    {
        PIN_PRG1_PRU0_GPO5, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii0_rxd1 -> AG24 */
    {
        PIN_PRG1_PRU0_GPO1, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii0_rxd0 -> AE22 */
    {
        PIN_PRG1_PRU0_GPO0, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii0_rxlink -> AF28 */
    {
        PIN_PRG1_PRU0_GPO8, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii_mt1_clk -> AD20 */
    {
        PIN_PRG1_PRU0_GPO16, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii1_txen -> AD19 */
    {
        PIN_PRG1_PRU0_GPO15, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii1_txd3 -> AG20 */
    {
        PIN_PRG1_PRU0_GPO14, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii1_txd2 -> AH21 */
    {
        PIN_PRG1_PRU0_GPO13, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii1_txd1 -> AH20 */
    {
        PIN_PRG1_PRU0_GPO12, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii1_txd0 -> AF21 */
    {
        PIN_PRG1_PRU0_GPO11, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii1_rxdv -> AE21 */
    {
        PIN_PRG1_PRU1_GPO4, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii_mr1_clk -> AG22 */
    {
        PIN_PRG1_PRU1_GPO6, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii1_rxd3 -> AH22 */
    {
        PIN_PRG1_PRU1_GPO3, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii1_rxd2 -> AG21 */
    {
        PIN_PRG1_PRU1_GPO2, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii1_rxer -> AC22 */
    {
        PIN_PRG1_PRU1_GPO5, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii1_rxd1 -> AH23 */
    {
        PIN_PRG1_PRU1_GPO1, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii1_rxd0 -> AH24 */
    {
        PIN_PRG1_PRU1_GPO0, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyPRU_ICSSG1_MII_G_RT1 -> pr1_mii1_rxlink -> AE24 */
    {
        PIN_PRG1_PRU1_GPO8, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    {PINMUX_END}
};

static pinmuxModuleCfg_t gPru_icssg1_mii_g_rtPinCfg[] =
{
    {0, TRUE, gPru_icssg1_mii_g_rt0PinCfg},
    {PINMUX_END}
};

pinmuxBoardCfg_t gAM65xxMIIPinmuxData[] =
{
    {0, gPru_icssg0_mii_g_rtPinCfg},
    {1, gPru_icssg1_mii_g_rtPinCfg},
    {PINMUX_END}
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* None */
