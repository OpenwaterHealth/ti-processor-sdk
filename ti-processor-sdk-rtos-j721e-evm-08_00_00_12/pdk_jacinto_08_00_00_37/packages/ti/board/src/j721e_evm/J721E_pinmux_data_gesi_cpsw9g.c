/**
* Note: This file was auto-generated by TI PinMux on 5/15/2019 at 8:07:17 AM.
*
* \file  J721E_pinmux_data.c
*
* \brief  This file contains the pin mux configurations for the boards.
*         These are prepared based on how the peripherals are extended on
*         the boards.
*
* \copyright Copyright (CU) 2019 Texas Instruments Incorporated -
*             http://www.ti.com/
*/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "J721E_pinmux.h"

/** Peripheral Pin Configurations */


static pinmuxPerCfg_t gCpsw9g0PinCfg[] =
{
    /* MyCPSW9G3 -> CLKOUT -> AA25 */
    {
        PIN_PRG0_PRU1_GPO10, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    {PINMUX_END}
};

static pinmuxModuleCfg_t gCpsw9gPinCfg[] =
{
    {0, TRUE, gCpsw9g0PinCfg},
    {PINMUX_END}
};


static pinmuxPerCfg_t gGpio0PinCfg[] =
{
    /* MyGPIO0 -> GPIO0_96 -> T23 */
    {
        PIN_RGMII5_RD0, PIN_MODE(7) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyGPIO0 -> GPIO0_104 -> W26 */
    {
        PIN_RGMII6_RXC, PIN_MODE(7) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    {PINMUX_END}
};

static pinmuxModuleCfg_t gGpioPinCfg[] =
{
    {0, TRUE, gGpio0PinCfg},
    {PINMUX_END}
};


static pinmuxPerCfg_t gMdio0PinCfg[] =
{
    /* MyMDIO1 -> MDIO0_MDC -> V24 */
    {
        PIN_MDIO0_MDC, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyMDIO1 -> MDIO0_MDIO -> V26 */
    {
        PIN_MDIO0_MDIO, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    {PINMUX_END}
};

static pinmuxModuleCfg_t gMdioPinCfg[] =
{
    {0, TRUE, gMdio0PinCfg},
    {PINMUX_END}
};


static pinmuxPerCfg_t gRgmii3PinCfg[] =
{
    /* MyRGMII3 -> RGMII3_RD0 -> AF28 */
    {
        PIN_PRG0_PRU0_GPO0, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII3 -> RGMII3_RD1 -> AE28 */
    {
        PIN_PRG0_PRU0_GPO1, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII3 -> RGMII3_RD2 -> AE27 */
    {
        PIN_PRG0_PRU0_GPO2, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII3 -> RGMII3_RD3 -> AD26 */
    {
        PIN_PRG0_PRU0_GPO3, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII3 -> RGMII3_RXC -> AE26 */
    {
        PIN_PRG0_PRU0_GPO6, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII3 -> RGMII3_RX_CTL -> AD25 */
    {
        PIN_PRG0_PRU0_GPO4, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII3 -> RGMII3_TD0 -> AJ28 */
    {
        PIN_PRG0_PRU0_GPO11, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII3 -> RGMII3_TD1 -> AH27 */
    {
        PIN_PRG0_PRU0_GPO12, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII3 -> RGMII3_TD2 -> AH29 */
    {
        PIN_PRG0_PRU0_GPO13, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII3 -> RGMII3_TD3 -> AG28 */
    {
        PIN_PRG0_PRU0_GPO14, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII3 -> RGMII3_TXC -> AH28 */
    {
        PIN_PRG0_PRU0_GPO16, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII3 -> RGMII3_TX_CTL -> AG27 */
    {
        PIN_PRG0_PRU0_GPO15, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    {PINMUX_END}
};

static pinmuxPerCfg_t gRgmii4PinCfg[] =
{
    /* MyRGMII4 -> RGMII4_RD0 -> AE29 */
    {
        PIN_PRG0_PRU1_GPO0, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII4 -> RGMII4_RD1 -> AD28 */
    {
        PIN_PRG0_PRU1_GPO1, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII4 -> RGMII4_RD2 -> AD27 */
    {
        PIN_PRG0_PRU1_GPO2, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII4 -> RGMII4_RD3 -> AC25 */
    {
        PIN_PRG0_PRU1_GPO3, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII4 -> RGMII4_RXC -> AC26 */
    {
        PIN_PRG0_PRU1_GPO6, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII4 -> RGMII4_RX_CTL -> AD29 */
    {
        PIN_PRG0_PRU1_GPO4, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII4 -> RGMII4_TD0 -> AG26 */
    {
        PIN_PRG0_PRU1_GPO11, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII4 -> RGMII4_TD1 -> AF27 */
    {
        PIN_PRG0_PRU1_GPO12, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII4 -> RGMII4_TD2 -> AF26 */
    {
        PIN_PRG0_PRU1_GPO13, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII4 -> RGMII4_TD3 -> AE25 */
    {
        PIN_PRG0_PRU1_GPO14, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII4 -> RGMII4_TXC -> AG29 */
    {
        PIN_PRG0_PRU1_GPO16, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII4 -> RGMII4_TX_CTL -> AF29 */
    {
        PIN_PRG0_PRU1_GPO15, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    {PINMUX_END}
};

static pinmuxPerCfg_t gRgmii1PinCfg[] =
{
    /* MyRGMII1 -> RGMII1_RD0 -> AC23 */
    {
        PIN_PRG1_PRU0_GPO0, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII1 -> RGMII1_RD1 -> AG22 */
    {
        PIN_PRG1_PRU0_GPO1, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII1 -> RGMII1_RD2 -> AF22 */
    {
        PIN_PRG1_PRU0_GPO2, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII1 -> RGMII1_RD3 -> AJ23 */
    {
        PIN_PRG1_PRU0_GPO3, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII1 -> RGMII1_RXC -> AD22 */
    {
        PIN_PRG1_PRU0_GPO6, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII1 -> RGMII1_RX_CTL -> AH23 */
    {
        PIN_PRG1_PRU0_GPO4, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII1 -> RGMII1_TD0 -> AF24 */
    {
        PIN_PRG1_PRU0_GPO11, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII1 -> RGMII1_TD1 -> AJ24 */
    {
        PIN_PRG1_PRU0_GPO12, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII1 -> RGMII1_TD2 -> AG24 */
    {
        PIN_PRG1_PRU0_GPO13, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII1 -> RGMII1_TD3 -> AD24 */
    {
        PIN_PRG1_PRU0_GPO14, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII1 -> RGMII1_TXC -> AE24 */
    {
        PIN_PRG1_PRU0_GPO16, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII1 -> RGMII1_TX_CTL -> AC24 */
    {
        PIN_PRG1_PRU0_GPO15, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    {PINMUX_END}
};

static pinmuxPerCfg_t gRgmii2PinCfg[] =
{
    /* MyRGMII2 -> RGMII2_RD0 -> AE22 */
    {
        PIN_PRG1_PRU1_GPO0, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII2 -> RGMII2_RD1 -> AG23 */
    {
        PIN_PRG1_PRU1_GPO1, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII2 -> RGMII2_RD2 -> AF23 */
    {
        PIN_PRG1_PRU1_GPO2, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII2 -> RGMII2_RD3 -> AD23 */
    {
        PIN_PRG1_PRU1_GPO3, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII2 -> RGMII2_RXC -> AE23 */
    {
        PIN_PRG1_PRU1_GPO6, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII2 -> RGMII2_RX_CTL -> AH24 */
    {
        PIN_PRG1_PRU1_GPO4, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII2 -> RGMII2_TD0 -> AJ25 */
    {
        PIN_PRG1_PRU1_GPO11, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII2 -> RGMII2_TD1 -> AH25 */
    {
        PIN_PRG1_PRU1_GPO12, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII2 -> RGMII2_TD2 -> AG25 */
    {
        PIN_PRG1_PRU1_GPO13, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII2 -> RGMII2_TD3 -> AH26 */
    {
        PIN_PRG1_PRU1_GPO14, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII2 -> RGMII2_TXC -> AJ26 */
    {
        PIN_PRG1_PRU1_GPO16, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII2 -> RGMII2_TX_CTL -> AJ27 */
    {
        PIN_PRG1_PRU1_GPO15, PIN_MODE(4) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    {PINMUX_END}
};

static pinmuxPerCfg_t gRgmii7PinCfg[] =
{
    /* MyRGMII7 -> RGMII7_RD0 -> AC23 */
    {
        PIN_PRG1_PRU0_GPO0, PIN_MODE(9) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII7 -> RGMII7_RD1 -> AG22 */
    {
        PIN_PRG1_PRU0_GPO1, PIN_MODE(9) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII7 -> RGMII7_RD2 -> AF22 */
    {
        PIN_PRG1_PRU0_GPO2, PIN_MODE(9) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII7 -> RGMII7_RD3 -> AJ23 */
    {
        PIN_PRG1_PRU0_GPO3, PIN_MODE(9) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII7 -> RGMII7_RXC -> AD22 */
    {
        PIN_PRG1_PRU0_GPO6, PIN_MODE(9) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII7 -> RGMII7_RX_CTL -> AH23 */
    {
        PIN_PRG1_PRU0_GPO4, PIN_MODE(9) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII7 -> RGMII7_TD0 -> AF24 */
    {
        PIN_PRG1_PRU0_GPO11, PIN_MODE(9) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII7 -> RGMII7_TD1 -> AJ24 */
    {
        PIN_PRG1_PRU0_GPO12, PIN_MODE(9) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII7 -> RGMII7_TD2 -> AG24 */
    {
        PIN_PRG1_PRU0_GPO13, PIN_MODE(9) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII7 -> RGMII7_TD3 -> AD24 */
    {
        PIN_PRG1_PRU0_GPO14, PIN_MODE(9) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII7 -> RGMII7_TXC -> AE24 */
    {
        PIN_PRG1_PRU0_GPO16, PIN_MODE(9) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII7 -> RGMII7_TX_CTL -> AC24 */
    {
        PIN_PRG1_PRU0_GPO15, PIN_MODE(9) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    {PINMUX_END}
};

static pinmuxPerCfg_t gRgmii8PinCfg[] =
{
    /* MyRGMII8 -> RGMII8_RD0 -> AE22 */
    {
        PIN_PRG1_PRU1_GPO0, PIN_MODE(8) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII8 -> RGMII8_RD1 -> AG23 */
    {
        PIN_PRG1_PRU1_GPO1, PIN_MODE(8) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII8 -> RGMII8_RD2 -> AF23 */
    {
        PIN_PRG1_PRU1_GPO2, PIN_MODE(8) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII8 -> RGMII8_RD3 -> AD23 */
    {
        PIN_PRG1_PRU1_GPO3, PIN_MODE(8) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII8 -> RGMII8_RXC -> AE23 */
    {
        PIN_PRG1_PRU1_GPO6, PIN_MODE(8) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII8 -> RGMII8_RX_CTL -> AH24 */
    {
        PIN_PRG1_PRU1_GPO4, PIN_MODE(8) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRGMII8 -> RGMII8_TD0 -> AJ25 */
    {
        PIN_PRG1_PRU1_GPO11, PIN_MODE(8) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII8 -> RGMII8_TD1 -> AH25 */
    {
        PIN_PRG1_PRU1_GPO12, PIN_MODE(8) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII8 -> RGMII8_TD2 -> AG25 */
    {
        PIN_PRG1_PRU1_GPO13, PIN_MODE(8) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII8 -> RGMII8_TD3 -> AH26 */
    {
        PIN_PRG1_PRU1_GPO14, PIN_MODE(8) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII8 -> RGMII8_TXC -> AJ26 */
    {
        PIN_PRG1_PRU1_GPO16, PIN_MODE(8) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRGMII8 -> RGMII8_TX_CTL -> AJ27 */
    {
        PIN_PRG1_PRU1_GPO15, PIN_MODE(8) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    {PINMUX_END}
};

static pinmuxModuleCfg_t gRgmiiPinCfg[] =
{
    {3, TRUE, gRgmii3PinCfg},
    {4, TRUE, gRgmii4PinCfg},
    {1, TRUE, gRgmii1PinCfg},
    {2, TRUE, gRgmii2PinCfg},
    {PINMUX_END}
};


static pinmuxModuleCfg_t gSgmiiPinCfg[] =
{
    {3, TRUE, gRgmii3PinCfg},
    {4, TRUE, gRgmii4PinCfg},
    {7, TRUE, gRgmii7PinCfg},
    {8, TRUE, gRgmii8PinCfg},
    {PINMUX_END}
};

static pinmuxModuleCfg_t gQsgmiiPinCfg[] =
{
    {1, TRUE, gRgmii1PinCfg},
    {3, TRUE, gRgmii3PinCfg},
    {4, TRUE, gRgmii4PinCfg},
    {8, TRUE, gRgmii8PinCfg},
    {PINMUX_END}
};


static pinmuxPerCfg_t gRmii8PinCfg[] =
{
    /* MyRMII8 -> RMII8_CRS_DV -> Y28 */
    {
        PIN_RGMII6_TX_CTL, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRMII8 -> RMII8_RXD0 -> W25 */
    {
        PIN_RGMII6_RD0, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRMII8 -> RMII8_RXD1 -> W24 */
    {
        PIN_RGMII6_RD1, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRMII8 -> RMII8_RX_ER -> V23 */
    {
        PIN_RGMII6_RX_CTL, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyRMII8 -> RMII8_TXD0 -> W27 */
    {
        PIN_RGMII6_TD0, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRMII8 -> RMII8_TXD1 -> V25 */
    {
        PIN_RGMII6_TD1, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyRMII8 -> RMII8_TX_EN -> W29 */
    {
        PIN_RGMII6_TXC, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    {PINMUX_END}
};

static pinmuxPerCfg_t gRmii0PinCfg[] =
{
    /* MyRMII0 -> RMII_REF_CLK -> AD18 */
    {
        PIN_PRG1_MDIO0_MDC, PIN_MODE(5) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    {PINMUX_END}
};

static pinmuxModuleCfg_t gRmiiPinCfg[] =
{
    {8, TRUE, gRmii8PinCfg},
    {0, TRUE, gRmii0PinCfg},
    {PINMUX_END}
};


pinmuxBoardCfg_t gJ721E_MainPinmuxDataGesiCpsw9g[] =
{
    {0, gCpsw9gPinCfg},
    {1, gGpioPinCfg},
    {2, gMdioPinCfg},
    {3, gRgmiiPinCfg},
    {4, gRmiiPinCfg},
    {PINMUX_END}
};

pinmuxBoardCfg_t gJ721E_MainPinmuxDataGesiCpsw9gSgmii[] =
{
    {0, gCpsw9gPinCfg},
    {1, gGpioPinCfg},
    {2, gMdioPinCfg},
    {3, gSgmiiPinCfg},
    {4, gRmiiPinCfg},
    {PINMUX_END}
};

pinmuxBoardCfg_t gJ721E_MainPinmuxDataGesiCpsw9gQsgmii[] =
{
    {0, gCpsw9gPinCfg},
    {1, gGpioPinCfg},
    {2, gMdioPinCfg},
    {3, gQsgmiiPinCfg},
    {PINMUX_END}
};
pinmuxBoardCfg_t gJ721E_WkupPinmuxDataGesiCpsw9g[] =
{
    {PINMUX_END}
};
