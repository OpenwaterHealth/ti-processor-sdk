/**
*
* \file  J721E_pinmux_data_d3_rvp.c
*
* \brief  This file contains the pin mux configurations for the D3 RVP board.
*
* \copyright Copyright (CU) 2019 Texas Instruments Incorporated -
*             http://www.ti.com/
* \copyright Copyright (CU) 2020 D3 Engineering, LLC.
*/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "J721E_pinmux.h"

/** Peripheral Pin Configurations */

static pinmuxPerCfg_t i2c0_PinCfg[] = {
    /* MyI2C0 -> I2C0_SCL -> AC5 */
    {
        PIN_I2C0_SCL, PIN_MODE(0) | \
        ((PIN_PULL_DIRECTION | PIN_INPUT_ENABLE) & (~PIN_PULL_DISABLE))
    },
    /* MyI2C0 -> I2C0_SDA -> AA5 */
    {
        PIN_I2C0_SDA, PIN_MODE(0) | \
        ((PIN_PULL_DIRECTION | PIN_INPUT_ENABLE) & (~PIN_PULL_DISABLE))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t i2c1_PinCfg[] = {
    /* MyI2C1 -> I2C1_SCL -> Y6 */
    {
        PIN_I2C1_SCL, PIN_MODE(0) | \
        ((PIN_PULL_DIRECTION | PIN_INPUT_ENABLE) & (~PIN_PULL_DISABLE))
    },
    /* MyI2C1 -> I2C1_SDA -> AA6 */
    {
        PIN_I2C1_SDA, PIN_MODE(0) | \
        ((PIN_PULL_DIRECTION | PIN_INPUT_ENABLE) & (~PIN_PULL_DISABLE))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t i2c2_PinCfg[] =
{
    /* MyI2C2 -> I2C2_SCL -> AA1 */
    {
        PIN_SPI0_CLK, PIN_MODE(2) | \
        ((PIN_PULL_DIRECTION | PIN_INPUT_ENABLE) & (~PIN_PULL_DISABLE))
    },
    /* MyI2C2 -> I2C2_SDA -> AB5 */
    {
        PIN_SPI0_D0, PIN_MODE(2) | \
        ((PIN_PULL_DIRECTION | PIN_INPUT_ENABLE) & (~PIN_PULL_DISABLE))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t i2c6_PinCfg[] = {
    /* MyI2C6 -> I2C6_SCL -> AA3 */
    {
        PIN_SPI0_D1, PIN_MODE(2) | \
        ((PIN_PULL_DIRECTION | PIN_INPUT_ENABLE) & (~PIN_PULL_DISABLE))
    },
    /* MyI2C6 -> I2C6_SDA -> Y2 */
    {
        PIN_SPI1_D1, PIN_MODE(2) | \
        ((PIN_PULL_DIRECTION | PIN_INPUT_ENABLE) & (~PIN_PULL_DISABLE))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t mcu_i2c0_PinCfg[] = {
    /* MyMCU_I2C0 -> MCU_I2C0_SCL -> J26 */
    {
        PIN_MCU_I2C0_SCL, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyMCU_I2C0 -> MCU_I2C0_SDA -> H25 */
    {
        PIN_MCU_I2C0_SDA, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t wkup_i2c0_PinCfg[] = {
    /* MyWKUP_I2C0 -> WKUP_I2C0_SCL -> J25 */
    {
        PIN_WKUP_I2C0_SCL, PIN_MODE(0) | \
        ((PIN_PULL_DIRECTION | PIN_INPUT_ENABLE) & (~PIN_PULL_DISABLE))
    },
    /* MyWKUP_I2C0 -> WKUP_I2C0_SDA -> H24 */
    {
        PIN_WKUP_I2C0_SDA, PIN_MODE(0) | \
        ((PIN_PULL_DIRECTION | PIN_INPUT_ENABLE) & (~PIN_PULL_DISABLE))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t mcu_mcan0_PinCfg[] = {
    /* (C29) MCU_MCAN0_RX */
    {
        PIN_MCU_MCAN0_RX, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* (D29) MCU_MCAN0_TX */
    {
        PIN_MCU_MCAN0_TX, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t mcu_mcan1_PinCfg[] = {
    /* MyMCU_MCAN1 -> MCU_MCAN1_RX -> G24 */
    {
        PIN_WKUP_GPIO0_5, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyMCU_MCAN1 -> MCU_MCAN1_TX -> G25 */
    {
        PIN_WKUP_GPIO0_4, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t mcan0_PinCfg[] = {
    /* MyMCAN0 -> MCAN0_RX -> W5 */
    {
        PIN_MCAN0_RX, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyMCAN0 -> MCAN0_TX -> W6 */
    {
        PIN_MCAN0_TX, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t mcan4_PinCfg[] = {
    /* (AJ20) MCAN4_RX */
    {
        PIN_PRG1_PRU0_GPO8, PIN_MODE(6) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* (AE20) MCAN4_TX */
    {
        PIN_PRG1_PRU0_GPO7, PIN_MODE(6) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t mcan5_PinCfg[] = {
    /* (AE21) MCAN5_RX */
    {
        PIN_PRG1_PRU0_GPO18, PIN_MODE(6) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* (AJ21) MCAN5_TX */
    {
        PIN_PRG1_PRU0_GPO17, PIN_MODE(6) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t mcan6_PinCfg[] = {
    /* (AG21) MCAN6_RX */
    {
        PIN_PRG1_PRU1_GPO5, PIN_MODE(6) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* (AH21) MCAN6_TX */
    {
        PIN_PRG1_PRU0_GPO19, PIN_MODE(6) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t mcan7_PinCfg[] = {
    /* (AH25) MCAN7_RX */
    {
        PIN_PRG1_PRU1_GPO13, PIN_MODE(6) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* (AJ25) MCAN7_TX */
    {
        PIN_PRG1_PRU1_GPO12, PIN_MODE(6) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t mdio0_PinCfg[] =
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
    { PINMUX_END }
};

static pinmuxPerCfg_t rgmii3_PinCfg[] = {
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
    { PINMUX_END }
};

static pinmuxPerCfg_t uart0_PinCfg[] = {
    /* MyUART0 -> UART0_CTSn -> Y3 */
    {
        PIN_SPI1_CS0, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyUART0 -> UART0_RTSn -> AA2 */
    {
        PIN_SPI0_CS0, PIN_MODE(1) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyUART0 -> UART0_RXD -> AB2 */
    {
        PIN_UART0_RXD, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyUART0 -> UART0_TXD -> AB3 */
    {
        PIN_UART0_TXD, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t uart1_PinCfg[] = {
    /* MyUART1 -> UART1_RXD -> AA4 */
    {
        PIN_UART1_RXD, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyUART1 -> UART1_TXD -> AB4 */
    {
        PIN_UART1_TXD, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t uart2_PinCfg[] = {
    /* MyUART2 -> UART2_RXD -> Y1 */
    {
        PIN_SPI1_CLK, PIN_MODE(3) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyUART2 -> UART2_TXD -> Y5 */
    {
        PIN_SPI1_D0, PIN_MODE(3) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    { PINMUX_END }
};

static pinmuxPerCfg_t uart8_PinCfg[] = {
    /* MyUART1 -> UART8_RXD -> Y24 */
    {
        PIN_PRG0_PRU1_GPO9, PIN_MODE(14) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyUART1 -> UART8_TXD -> AA25 */
    {
        PIN_PRG0_PRU1_GPO10, PIN_MODE(14) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    { PINMUX_END }
};

static pinmuxModuleCfg_t i2c_ModuleCfg[] = {
    { 0, TRUE, i2c0_PinCfg },
    { 1, TRUE, i2c1_PinCfg },
    { 2, TRUE, i2c2_PinCfg },
    { 6, TRUE, i2c6_PinCfg },
    { PINMUX_END }
};

static pinmuxModuleCfg_t mcu_i2c_ModuleCfg[] = {
    { 0, TRUE, mcu_i2c0_PinCfg },
    { PINMUX_END }
};

static pinmuxModuleCfg_t wkup_i2c_ModuleCfg[] = {
    { 0, TRUE, wkup_i2c0_PinCfg },
    { PINMUX_END }
};

static pinmuxModuleCfg_t uart_ModuleCfg[] = {
    { 0, TRUE, uart0_PinCfg },
    { 1, TRUE, uart1_PinCfg },
    { 2, TRUE, uart2_PinCfg },
    { 8, TRUE, uart8_PinCfg },
    { PINMUX_END }
};

static pinmuxModuleCfg_t wkup_gpio_ModuleCfg[] = {
    { PINMUX_END }
};

static pinmuxModuleCfg_t mdio_ModuleCfg[] = {
    { 0, TRUE, mdio0_PinCfg },
    { PINMUX_END }
};

static pinmuxModuleCfg_t mcu_mcan_ModuleCfg[] = {
    { 0, TRUE, mcu_mcan0_PinCfg },
    { 1, TRUE, mcu_mcan1_PinCfg },
    { PINMUX_END }
};

static pinmuxModuleCfg_t mcan_ModuleCfg[] = {
    { 0, TRUE, mcan0_PinCfg },
    { 4, TRUE, mcan4_PinCfg },
    { 5, TRUE, mcan5_PinCfg },
    { 6, TRUE, mcan6_PinCfg },
    { 7, TRUE, mcan7_PinCfg },
    { PINMUX_END }
};

static pinmuxModuleCfg_t rgmii3_ModuleCfg[] = {
    { 0, TRUE, rgmii3_PinCfg },
    { PINMUX_END }
};


pinmuxBoardCfg_t gJ721E_MainPinmuxDataD3RVP[] =
{
    { 0, mcu_i2c_ModuleCfg },
    { 1, i2c_ModuleCfg },
    { 2, mcu_mcan_ModuleCfg },
    { 3, mcan_ModuleCfg },
    { 4, mdio_ModuleCfg },
    { 5, rgmii3_ModuleCfg },
    { 6, uart_ModuleCfg },
    { PINMUX_END }
};

pinmuxBoardCfg_t gJ721E_WkupPinmuxDataD3RVP[] = {
    { 1, wkup_i2c_ModuleCfg },
    { 2, wkup_gpio_ModuleCfg },
    { 3, mcan_ModuleCfg },
    { PINMUX_END }
};

