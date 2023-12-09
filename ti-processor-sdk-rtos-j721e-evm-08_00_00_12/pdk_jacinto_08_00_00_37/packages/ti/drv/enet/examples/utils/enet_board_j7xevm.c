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
 * \file  enet_board_j7xevm.c
 *
 * \brief This file contains the implementation of J721E/J7200 EVM support.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <ti/drv/enet/enet.h>
#include <ti/drv/enet/include/phy/enetphy.h>
#include <ti/drv/enet/include/phy/dp83867.h>
#include <ti/drv/enet/examples/utils/include/enet_board_j7xevm.h>
#include <ti/drv/enet/examples/utils/include/enet_apputils.h>

#include <ti/board/board.h>
#if defined (SOC_J721E)
#include <ti/board/src/j721e_evm/include/board_pinmux.h>
#include <ti/board/src/j721e_evm/include/board_utils.h>
#include <ti/board/src/j721e_evm/include/board_control.h>
#include <ti/board/src/j721e_evm/include/board_ethernet_config.h>
#include <ti/board/src/j721e_evm/include/board_serdes_cfg.h>
#elif defined (SOC_J7200)
#include <ti/board/src/j7200_evm/include/board_pinmux.h>
#include <ti/board/src/j7200_evm/include/board_utils.h>
#include <ti/board/src/j7200_evm/include/board_control.h>
#include <ti/board/src/j7200_evm/include/board_ethernet_config.h>
#include <ti/board/src/j7200_evm/include/board_serdes_cfg.h>
#else
#error "Invalid SOC"
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* GESI PHYs connected to CPSW9G MDIO (MDIO_MDC_SEL0 = MDIO_MDC_SEL1 = Low) */
#define ENET_BOARD_J7XEVM_ICSSG_MDIO_MUX             ENET_BIT(0U)

/* GESI PHYs connected to CPSW9G MDIO (MDIO_MDC_SEL0 = MDIO_MDC_SEL1 = High) */
#define ENET_BOARD_J7XEVM_CPSW9G_MDIO_MUX            ENET_BIT(1U)

/* Initialize QSGMII board.
 *
 * Set GPIOs to release QSGMII PHY out of reset and release COMA_MODE pin. */
#define ENET_BOARD_J7XEVM_QPENET_INIT                ENET_BIT(2U)

/* Enable SERDES Sierra0 clocks (valid for J721E only) */
#define ENET_BOARD_J7XEVM_SERDES_SIERRA0_CLKS        ENET_BIT(3U)

/* Enable SERDES Sierra1 clocks (valid for J721E only) */
#define ENET_BOARD_J7XEVM_SERDES_SIERRA1_CLKS        ENET_BIT(4U)

/* Enable SERDES Torrent clocks */
#define ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS        ENET_BIT(5U)

/* Enable RMII route.
 *
 * I2C GPIO expander 2 (addr 0x22) on CPB board:
 *  - P11 = MCASP/TRACE_MUX_S0 = High
 *  - P12 = MCASP/TRACE_MUX_S1 = Low  */
#define ENET_BOARD_J7XEVM_GESI_RMII_MUX              ENET_BIT(6U)

/* MCU2_0 UART instances:
 *  - J721E EVM: UART2
 *  - J7200 EVM: UART3 */
#if defined(BUILD_MCU2_0)
#if defined(SOC_J7200)
#define ENET_BOARD_J7XEVM_MCU2_0_UART_INST           (3U)
#else
#define ENET_BOARD_J7XEVM_MCU2_0_UART_INST           (2U)
#endif
#endif

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static const EnetBoard_PortCfg *EnetBoard_getPortCfg(const EnetBoard_EthPort *ethPort);

static const EnetBoard_PortCfg *EnetBoard_findPortCfg(const EnetBoard_EthPort *ethPort,
                                                      const EnetBoard_PortCfg *ethPortCfgs,
                                                      uint32_t numEthPorts);

#if defined(BUILD_MCU2_0)
#if defined (SOC_J721E)
static void EnetBoard_configSierra0Clks(void);

static void EnetBoard_disableSierra0Clks(void);

static void EnetBoard_configSierra1Clks(void);

static void EnetBoard_disableSierra1Clks(void);
#endif

static void EnetBoard_configTorrentClks(void);

static void EnetBoard_disableTorrentClks(void);
#endif

static void EnetBoard_setEnetControl(Enet_Type enetType,
                                     Enet_MacPort macPort,
                                     EnetMacPort_Interface *mii);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/*!
 * \brief Common Processor Board (CPB) board's DP83867 PHY configuration.
 */
static const Dp83867_Cfg gEnetCpbBoard_dp83867PhyCfg =
{
    .txClkShiftEn         = true,
    .rxClkShiftEn         = true,
    .txDelayInPs          = 2000U,  /* 2.00 ns */
    .rxDelayInPs          = 2000U,  /* 2.00 ns */
    .txFifoDepth          = 4U,
    .idleCntThresh        = 4U,     /* Improves short cable performance */
    .impedanceInMilliOhms = 35000,  /* 35 ohms */
    .gpio0Mode            = DP83867_GPIO0_LED3,
    .gpio1Mode            = DP83867_GPIO1_COL, /* Unused */
    .ledMode              =
    {
        DP83867_LED_LINKED,         /* Unused */
        DP83867_LED_LINKED_100BTX,
        DP83867_LED_RXTXACT,
        DP83867_LED_LINKED_1000BT,
    },
};

/*
 * GESI board's DP83867 PHY configuration.
 */
static const Dp83867_Cfg gEnetGesiBoard_dp83867PhyCfg =
{
    .txClkShiftEn         = true,
    .rxClkShiftEn         = true,
    .txDelayInPs          = 2750U,  /* 2.75 ns */
    .rxDelayInPs          = 2500U,  /* 2.50 ns */
    .txFifoDepth          = 4U,
    .idleCntThresh        = 4U,     /* Improves short cable performance */
    .impedanceInMilliOhms = 35000,  /* 35 ohms */
    .gpio0Mode            = DP83867_GPIO0_LED3,
    .gpio1Mode            = DP83867_GPIO1_COL, /* Unused */
    .ledMode              =
    {
        DP83867_LED_LINKED,         /* Unused */
        DP83867_LED_LINKED_100BTX,
        DP83867_LED_RXTXACT,
        DP83867_LED_LINKED_1000BT,
    },
};

/*
 * J721E/J7200 GESI board configuration.
 *
 * 1 x RGMII PHY connected to J721E/J7200 CPSW_2G MAC port.
 */
static const EnetBoard_PortCfg gEnetCpbBoard_j7xEthPort[] =
{
    {    /* "MCU_ENET" */
        .enetType = ENET_CPSW_2G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr         = 0U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = &gEnetCpbBoard_dp83867PhyCfg,
            .extendedCfgSize = sizeof(gEnetCpbBoard_dp83867PhyCfg),
        },
        .flags    = 0U,
    },
};

#if defined(SOC_J721E)
/*
 * J721E GESI board configuration.
 *
 * 4 x RGMII PHYs and 1 x RMII are connected to J721E CPSW_9G MAC ports.
 */
static const EnetBoard_PortCfg gEnetGesiBoard_j721eEthPort[] =
{
    {    /* "PRG1_RGMII1_B" */
        .enetType = ENET_CPSW_9G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr         = 12U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = &gEnetGesiBoard_dp83867PhyCfg,
            .extendedCfgSize = sizeof(gEnetGesiBoard_dp83867PhyCfg),
        },
        .flags    = ENET_BOARD_J7XEVM_CPSW9G_MDIO_MUX,
    },
    {    /* "PRG1_RGMII2_T" */
        .enetType = ENET_CPSW_9G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_8,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr         = 15U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = &gEnetGesiBoard_dp83867PhyCfg,
            .extendedCfgSize = sizeof(gEnetGesiBoard_dp83867PhyCfg),
        },
        .flags    = ENET_BOARD_J7XEVM_CPSW9G_MDIO_MUX,
    },
    {   /* "PRG0_RGMII1_B" */
        .enetType = ENET_CPSW_9G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_3,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr         = 0U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = &gEnetGesiBoard_dp83867PhyCfg,
            .extendedCfgSize = sizeof(gEnetGesiBoard_dp83867PhyCfg),
        },
        .flags    = ENET_BOARD_J7XEVM_CPSW9G_MDIO_MUX,
    },
    {   /* "PRG0_RGMII02_T" */
        .enetType = ENET_CPSW_9G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_4,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr         = 3U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = &gEnetGesiBoard_dp83867PhyCfg,
            .extendedCfgSize = sizeof(gEnetGesiBoard_dp83867PhyCfg),
        },
        .flags    = ENET_BOARD_J7XEVM_CPSW9G_MDIO_MUX,
    },
    {    /* "RMII8" */
        .enetType = ENET_CPSW_9G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_8,
        .mii      = { ENET_MAC_LAYER_MII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr         = 23U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = NULL,
            .extendedCfgSize = 0U,
        },
        .flags    = (ENET_BOARD_J7XEVM_CPSW9G_MDIO_MUX |
                     ENET_BOARD_J7XEVM_GESI_RMII_MUX),
    },
};

/*
 * J721E GESI board configuration for ICSSG.
 *
 * 4 x RGMII PHYs and 1 x RMII are connected to J721E ICCSG MAC ports.
 */
static const EnetBoard_PortCfg gEnetGesiBoard_j721eIcssgEthPort[] =
{
    {    /* "PRG1_RGMII1_B" */
        .enetType = ENET_ICSSG_DUALMAC,
        .instId   = 1U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr         = 12U,
            .isStrapped      = true,
            .skipExtendedCfg = false,
            .extendedCfg     = &gEnetGesiBoard_dp83867PhyCfg,
            .extendedCfgSize = sizeof(gEnetGesiBoard_dp83867PhyCfg),
        },
        .flags    = ENET_BOARD_J7XEVM_ICSSG_MDIO_MUX,
    },
    {    /* "PRG1_RGMII2_T" */
        .enetType = ENET_ICSSG_DUALMAC,
        .instId   = 1U,
        .macPort  = ENET_MAC_PORT_2,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr         = 15U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = &gEnetGesiBoard_dp83867PhyCfg,
            .extendedCfgSize = sizeof(gEnetGesiBoard_dp83867PhyCfg),
        },
        .flags    = ENET_BOARD_J7XEVM_ICSSG_MDIO_MUX,
    },
    {   /* "PRG0_RGMII1_B" */
        .enetType = ENET_ICSSG_DUALMAC,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr         = 0U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = &gEnetGesiBoard_dp83867PhyCfg,
            .extendedCfgSize = sizeof(gEnetGesiBoard_dp83867PhyCfg),
        },
        .flags    = ENET_BOARD_J7XEVM_ICSSG_MDIO_MUX,
    },
    {   /* "PRG0_RGMII02_T" */
        .enetType = ENET_ICSSG_DUALMAC,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_2,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr         = 3U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = &gEnetGesiBoard_dp83867PhyCfg,
            .extendedCfgSize = sizeof(gEnetGesiBoard_dp83867PhyCfg),
        },
        .flags    = ENET_BOARD_J7XEVM_ICSSG_MDIO_MUX,
    },
};

/*
 * J721E QSGMII board (QpENet) configuration.
 *
 * All 4 x QSGMII ports are connected to J721E CPSW_9G MAC ports.
 */
static const EnetBoard_PortCfg gEnetQpENetBoard_j721eEthPort[] =
{
    {    /* "P0" */
        .enetType = ENET_CPSW_9G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_2,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_QUAD_SERIAL_MAIN },
        .phyCfg   =
        {
            .phyAddr         = 16U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = NULL,
            .extendedCfgSize = 0U,
        },
        .flags    = (ENET_BOARD_J7XEVM_QPENET_INIT |
                     ENET_BOARD_J7XEVM_SERDES_SIERRA0_CLKS |
                     ENET_BOARD_J7XEVM_SERDES_SIERRA1_CLKS |
                     ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS),
    },
    {    /* "P1" */
        .enetType = ENET_CPSW_9G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_5,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_QUAD_SERIAL_SUB },
        .phyCfg   =
        {
            .phyAddr         = 17U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = NULL,
            .extendedCfgSize = 0U,
        },
        .flags    = (ENET_BOARD_J7XEVM_QPENET_INIT |
                     ENET_BOARD_J7XEVM_SERDES_SIERRA0_CLKS |
                     ENET_BOARD_J7XEVM_SERDES_SIERRA1_CLKS |
                     ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS),
    },
    {    /* "P2" */
        .enetType = ENET_CPSW_9G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_6,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_QUAD_SERIAL_SUB },
        .phyCfg   =
        {
            .phyAddr         = 18U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = NULL,
            .extendedCfgSize = 0U,
        },
        .flags    = (ENET_BOARD_J7XEVM_QPENET_INIT |
                     ENET_BOARD_J7XEVM_SERDES_SIERRA0_CLKS |
                     ENET_BOARD_J7XEVM_SERDES_SIERRA1_CLKS |
                     ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS),
    },
    {    /* "P3" */
        .enetType = ENET_CPSW_9G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_7,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_QUAD_SERIAL_SUB },
        .phyCfg   =
        {
            .phyAddr         = 19U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = NULL,
            .extendedCfgSize = 0U,
        },
        .flags    = (ENET_BOARD_J7XEVM_QPENET_INIT |
                     ENET_BOARD_J7XEVM_SERDES_SIERRA0_CLKS |
                     ENET_BOARD_J7XEVM_SERDES_SIERRA1_CLKS |
                     ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS),
    },
};

/*
 * J721E SGMII board configuration.
 *
 * 1 x SGMII port connected to J721E CPSW_9G MAC port.
 */
static const EnetBoard_PortCfg gEnetSgmiiBoard_j721eEthPort[] =
{
    {
        .enetType = ENET_CPSW_9G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_2,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_SERIAL },
        .phyCfg   =
        {
            .phyAddr         = 10U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = NULL,
            .extendedCfgSize = 0U,
        },
        .flags    = (ENET_BOARD_J7XEVM_QPENET_INIT |
                     ENET_BOARD_J7XEVM_SERDES_SIERRA0_CLKS |
                     ENET_BOARD_J7XEVM_SERDES_SIERRA1_CLKS |
                     ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS),
    },
};

/*
 * J721E dummy board used for MAC loopback setup.
 */
static const EnetBoard_PortCfg gEnetLpbkBoard_j721eEthPort[] =
{
    {    /* CPSW_2G RGMII MAC loopback */
        .enetType = ENET_CPSW_2G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr = ENETPHY_INVALID_PHYADDR,
        },
        .flags    = 0U,
    },
    {    /* CPSW_2G RMII MAC loopback */
        .enetType = ENET_CPSW_2G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_MII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr = ENETPHY_INVALID_PHYADDR,
        },
        .flags    = 0U,
    },
    {    /* CPSW_9G RGMII MAC loopback */
        .enetType = ENET_CPSW_9G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr = ENETPHY_INVALID_PHYADDR,
        },
        .flags    = 0U,
    },
    {    /* CPSW_9G RGMII MAC loopback */
        .enetType = ENET_CPSW_9G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_SERIAL },
        .phyCfg   =
        {
            .phyAddr = ENETPHY_INVALID_PHYADDR,
        },
        .flags    = (ENET_BOARD_J7XEVM_SERDES_SIERRA0_CLKS |
                     ENET_BOARD_J7XEVM_SERDES_SIERRA1_CLKS |
                     ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS),
    },
};
#endif /* defined(SOC_J721E) */

#if defined(SOC_J7200)
/*
 * J7200 GESI board configuration.
 *
 * Only one RGMII PHY is connected to J7200 CPSW_5G MAC port.
 */
static const EnetBoard_PortCfg gEnetGesiBoard_j7200EthPort[] =
{
    {    /* "PRG1_RGMII2_T" */
        .enetType = ENET_CPSW_5G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_2,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr         = 0U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = &gEnetGesiBoard_dp83867PhyCfg,
            .extendedCfgSize = sizeof(gEnetGesiBoard_dp83867PhyCfg),
        },
        .flags    = ENET_BOARD_J7XEVM_CPSW9G_MDIO_MUX,
    },
};

/*
 * J7200 QSGMII board (QpENet) configuration.
 *
 * All 4 x QSGMII ports are connected to J7200 CPSW_5G MAC ports.
 */
static const EnetBoard_PortCfg gEnetQpENetBoard_j7200EthPort[] =
{
    {    /* "P0" */
        .enetType = ENET_CPSW_5G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_QUAD_SERIAL_MAIN },
        .phyCfg   =
        {
            .phyAddr         = 16U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = NULL,
            .extendedCfgSize = 0U,
        },
        .flags    = (ENET_BOARD_J7XEVM_QPENET_INIT |
                     ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS),
    },
    {    /* "P1" */
        .enetType = ENET_CPSW_5G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_2,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_QUAD_SERIAL_SUB },
        .phyCfg   =
        {
            .phyAddr         = 17U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = NULL,
            .extendedCfgSize = 0U,
        },
        .flags    = (ENET_BOARD_J7XEVM_QPENET_INIT |
                     ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS),
    },
    {    /* "P2" */
        .enetType = ENET_CPSW_5G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_3,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_QUAD_SERIAL_SUB },
        .phyCfg   =
        {
            .phyAddr         = 18U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = NULL,
            .extendedCfgSize = 0U,
        },
        .flags    = (ENET_BOARD_J7XEVM_QPENET_INIT |
                     ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS),
    },
    {    /* "P3" */
        .enetType = ENET_CPSW_5G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_4,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_QUAD_SERIAL_SUB },
        .phyCfg   =
        {
            .phyAddr         = 19U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = NULL,
            .extendedCfgSize = 0U,
        },
        .flags    = (ENET_BOARD_J7XEVM_QPENET_INIT |
                     ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS),
    },
};

/*
 * J7200 SGMII board configuration.
 *
 * 1 x SGMII port connected to J7200 CPSW_5G MAC port.
 */
static const EnetBoard_PortCfg gEnetSgmiiBoard_j7200EthPort[] =
{
    {
        .enetType = ENET_CPSW_5G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_SERIAL },
        .phyCfg   =
        {
            .phyAddr         = 10U,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = NULL,
            .extendedCfgSize = 0U,
        },
        .flags    = (ENET_BOARD_J7XEVM_QPENET_INIT |
                     ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS),
    },
};

/*
 * J7200 virtual board used for MAC loopback setup.
 */
static const EnetBoard_PortCfg gEnetLpbkBoard_j7200EthPort[] =
{
    {    /* CPSW_2G RGMII MAC loopback */
        .enetType = ENET_CPSW_2G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr = ENETPHY_INVALID_PHYADDR,
        },
        .flags    = 0U,
    },
    {    /* CPSW_2G RMII MAC loopback */
        .enetType = ENET_CPSW_2G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_MII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr = ENETPHY_INVALID_PHYADDR,
        },
        .flags    = 0U,
    },
    {    /* CPSW_5G RGMII MAC loopback */
        .enetType = ENET_CPSW_5G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr = ENETPHY_INVALID_PHYADDR,
        },
        .flags    = 0U,
    },
    {    /* CPSW_5G SGMII MAC loopback */
        .enetType = ENET_CPSW_5G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_SERIAL },
        .phyCfg   =
        {
            .phyAddr = ENETPHY_INVALID_PHYADDR,
        },
        .flags    = (ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS),
    },
};
#endif /* defined(SOC_J7200) */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static void EnetBoard_enetExpCfg(void)
{
    Board_STATUS boardStatus;

    /* Release PHY reset. Note this is needed for both SGMII and QSGMII boards */
    boardStatus = Board_cpswEnetExpPhyReset(0U);
    EnetAppUtils_assert(BOARD_SOK == boardStatus);

    if (Board_detectBoard(BOARD_ID_ENET))
    {
        /* Release the COMA_MODE pin */
        boardStatus = Board_cpswEnetExpComaModeCfg(0U);
        EnetAppUtils_assert(BOARD_SOK == boardStatus);
    }
}

const EnetBoard_PhyCfg *EnetBoard_getPhyCfg(const EnetBoard_EthPort *ethPort)
{
    const EnetBoard_PortCfg *portCfg;

    portCfg = EnetBoard_getPortCfg(ethPort);

    return (portCfg != NULL) ? &portCfg->phyCfg : NULL;
}

static const EnetBoard_PortCfg *EnetBoard_getPortCfg(const EnetBoard_EthPort *ethPort)
{
    const EnetBoard_PortCfg *portCfg = NULL;

    if (ENET_NOT_ZERO(ethPort->boardId & ENETBOARD_CPB_ID))
    {
        portCfg = EnetBoard_findPortCfg(ethPort,
                                        gEnetCpbBoard_j7xEthPort,
                                        ENETPHY_ARRAYSIZE(gEnetCpbBoard_j7xEthPort));
    }

    if ((portCfg == NULL) &&
        ENET_NOT_ZERO(ethPort->boardId & ENETBOARD_GESI_ID))
    {
#if defined(SOC_J721E)
        if (ethPort->enetType == ENET_CPSW_9G)
        {
            portCfg = EnetBoard_findPortCfg(ethPort,
                                            gEnetGesiBoard_j721eEthPort,
                                            ENETPHY_ARRAYSIZE(gEnetGesiBoard_j721eEthPort));
        }
        else if (ethPort->enetType == ENET_ICSSG_DUALMAC)
        {
            portCfg = EnetBoard_findPortCfg(ethPort,
                                            gEnetGesiBoard_j721eIcssgEthPort,
                                            ENETPHY_ARRAYSIZE(gEnetGesiBoard_j721eIcssgEthPort));
        }
#elif defined(SOC_J7200)
        portCfg = EnetBoard_findPortCfg(ethPort,
                                        gEnetGesiBoard_j7200EthPort,
                                        ENETPHY_ARRAYSIZE(gEnetGesiBoard_j7200EthPort));
#endif
    }

    if ((portCfg == NULL) &&
        ENET_NOT_ZERO(ethPort->boardId & ENETBOARD_QPENET_ID))
    {
#if defined(SOC_J721E)
        portCfg = EnetBoard_findPortCfg(ethPort,
                                        gEnetQpENetBoard_j721eEthPort,
                                        ENETPHY_ARRAYSIZE(gEnetQpENetBoard_j721eEthPort));
#elif defined(SOC_J7200)
        portCfg = EnetBoard_findPortCfg(ethPort,
                                        gEnetQpENetBoard_j7200EthPort,
                                        ENETPHY_ARRAYSIZE(gEnetQpENetBoard_j7200EthPort));
#endif
    }

    if ((portCfg == NULL) &&
        ENET_NOT_ZERO(ethPort->boardId & ENETBOARD_SGMII_ID))
    {
#if defined(SOC_J721E)
        portCfg = EnetBoard_findPortCfg(ethPort,
                                        gEnetSgmiiBoard_j721eEthPort,
                                        ENETPHY_ARRAYSIZE(gEnetSgmiiBoard_j721eEthPort));
#elif defined(SOC_J7200)
        portCfg = EnetBoard_findPortCfg(ethPort,
                                        gEnetSgmiiBoard_j7200EthPort,
                                        ENETPHY_ARRAYSIZE(gEnetSgmiiBoard_j7200EthPort));
#endif
    }

    if ((portCfg == NULL) &&
        ENET_NOT_ZERO(ethPort->boardId & ENETBOARD_LOOPBACK_ID))
    {
#if defined(SOC_J721E)
        portCfg = EnetBoard_findPortCfg(ethPort,
                                        gEnetLpbkBoard_j721eEthPort,
                                        ENETPHY_ARRAYSIZE(gEnetLpbkBoard_j721eEthPort));
#elif defined(SOC_J7200)
        portCfg = EnetBoard_findPortCfg(ethPort,
                                        gEnetLpbkBoard_j7200EthPort,
                                        ENETPHY_ARRAYSIZE(gEnetLpbkBoard_j7200EthPort));
#endif
    }


    return portCfg;
}

static const EnetBoard_PortCfg *EnetBoard_findPortCfg(const EnetBoard_EthPort *ethPort,
                                                      const EnetBoard_PortCfg *ethPortCfgs,
                                                      uint32_t numEthPorts)
{
    const EnetBoard_PortCfg *ethPortCfg = NULL;
    bool found = false;
    uint32_t i;

    for (i = 0U; i < numEthPorts; i++)
    {
        ethPortCfg = &ethPortCfgs[i];

        if ((ethPortCfg->enetType == ethPort->enetType) &&
            (ethPortCfg->instId == ethPort->instId) &&
            (ethPortCfg->macPort == ethPort->macPort) &&
            (ethPortCfg->mii.layerType == ethPort->mii.layerType) &&
            (ethPortCfg->mii.sublayerType == ethPort->mii.sublayerType))
        {
            found = true;
            break;
        }
    }

    return found ? ethPortCfg : NULL;
}

int32_t EnetBoard_init(void)
{
#if defined(BUILD_MCU2_0)
    Board_initParams_t initParams;
#endif
    Board_initCfg boardCfg;
    Board_STATUS boardStatus;

    EnetAppUtils_setupSciServer();

    /* Set UART in main domain for MCU2_0 */
#if defined(BUILD_MCU2_0)
    Board_getInitParams(&initParams);
    initParams.uartInst      = ENET_BOARD_J7XEVM_MCU2_0_UART_INST;
    initParams.uartSocDomain = BOARD_SOC_DOMAIN_MAIN;
    /* Use non-exclusive mode in board lib when accessing the module for clock enable
     * to let Linux also access the module */
    initParams.pscMode = BOARD_PSC_DEVICE_MODE_NONEXCLUSIVE;
    Board_setInitParams(&initParams);
#endif

    /* Board configuration parameters. Further configuration done explicitly (i.e. ENETCTRL) */
    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_MODULE_CLOCK |
               BOARD_INIT_UART_STDIO;
    boardStatus = Board_init(boardCfg);
    EnetAppUtils_assert(boardStatus == BOARD_SOK);

#if defined (SOC_J7200) && defined(BUILD_MCU2_0)
    /* Set SOM's MUX2 for UART3 route */
    boardStatus = Board_control(BOARD_CTRL_CMD_SET_SOM_UART_MUX, NULL);
    EnetAppUtils_assert(boardStatus == BOARD_SOK);
#endif

    return (boardStatus == BOARD_SOK) ? ENET_SOK : ENET_EFAIL;
}

void EnetBoard_deinit(void)
{
    Board_initCfg boardCfg;
    Board_STATUS boardStatus;
#if defined(BUILD_MCU2_0)
    int32_t enetCard = Board_detectEnetCard();
#endif

#if defined(BUILD_MCU2_0)
    if ( (enetCard != BOARD_ENET_QSGMII) &&
         (enetCard != BOARD_ENET_SGMII))
    {
#if defined (SOC_J721E)
        EnetBoard_disableSierra0Clks();
        EnetBoard_disableSierra1Clks();
#endif
        EnetBoard_disableTorrentClks();
    }
#endif
    //TODO should Enet control be reseted as part of deinit
    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_MODULE_CLOCK |
               BOARD_INIT_UART_STDIO;
    boardStatus = Board_deinit(boardCfg);
    EnetAppUtils_assert(boardStatus == BOARD_SOK);
}

int32_t EnetBoard_setupPorts(EnetBoard_EthPort *ethPorts,
                             uint32_t numEthPorts)
{
    Board_STATUS boardStatus;
    Board_PinmuxConfig_t pinmuxCfg;
    bool gesiDetected = false;
#if defined(BUILD_MCU2_0)
    int32_t enetCard = Board_detectEnetCard();
#endif
    uint32_t hits = 0U;
    uint32_t flags = 0U;
    uint32_t i;
    int32_t status = ENET_SOK;

    bool qenetDetected = false;
    gesiDetected = Board_detectBoard(BOARD_ID_GESI);
    for (i = 0U; i < numEthPorts; i++)
    {
        EnetBoard_EthPort *ethPort = &ethPorts[i];
        const EnetBoard_PortCfg *portCfg;

        portCfg = EnetBoard_getPortCfg(ethPort);
        if (portCfg != NULL)
        {
            EnetBoard_setEnetControl(ethPort->enetType, ethPort->macPort, &ethPort->mii);
            flags |= portCfg->flags;
            hits++;
        }
    }

    EnetAppUtils_print("EnetBoard_setupPorts: %u of %u ports configurations found\n", hits, numEthPorts);

    /* Set pinmux for either CPSW9G or ICSSG, they are mutually exclusive */
    Board_pinmuxGetCfg(&pinmuxCfg);

#if defined(SOC_J721E)
    if (ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_ICSSG_MDIO_MUX))
    {
        pinmuxCfg.gesiExp = BOARD_PINMUX_GESI_ICSSG;
    }
#endif
    if (ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_CPSW9G_MDIO_MUX))
    {
        pinmuxCfg.gesiExp = BOARD_PINMUX_GESI_CPSW;
    }

    Board_pinmuxSetCfg(&pinmuxCfg);

    boardStatus = Board_init(BOARD_INIT_PINMUX_CONFIG);
    EnetAppUtils_assert(boardStatus == BOARD_SOK);

    /* ICSSG and CPSW9G use the same PHYs (hence same MDIO), they are mutually exclusive */
    if ((ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_ICSSG_MDIO_MUX)) &&
        (ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_CPSW9G_MDIO_MUX)))
    {
        EnetAppUtils_print("EnetBoard_setupPorts: ICSS and CPSW MDIOs on GESI board are mutually exclusive\n");
        EnetAppUtils_assert(false);
    }

    /* Check if GESI board is detected if any GESI related configuration is required */
    if (ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_GESI_RMII_MUX) ||
        ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_ICSSG_MDIO_MUX) ||
        ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_CPSW9G_MDIO_MUX))
    {
        if (!gesiDetected)
        {
            EnetAppUtils_print("EnetBoard_setupPorts: GESI board not detected\n");
            status = ENET_EFAIL;
        }
    }

    /* Check if QpENet board is detected if any QSMII related configuration is required */
    if (ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_QPENET_INIT) ||
        ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_SERDES_SIERRA0_CLKS) ||
        ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_SERDES_SIERRA1_CLKS) ||
        ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS))
    {
#if defined(BUILD_MCU2_0)
        if ( (enetCard != BOARD_ENET_QSGMII) &&
             (enetCard != BOARD_ENET_SGMII))
        {
            EnetAppUtils_print("EnetBoard_setupPorts: QpENet (QSGMII) or SGMII board not detected\n");
            status = ENET_EFAIL;
        }
        else
        {
            qenetDetected = true;
        }
#else
        EnetAppUtils_print("EnetBoard_setupPorts: QpENet/SGMII not supported on current core\n");
        status = ENET_EFAIL;
#endif
    }

    /* RMII route in GESI board */
#if defined(SOC_J721E)
    if ((ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_GESI_RMII_MUX)) && (gesiDetected))
    {
        boardStatus = Board_control(BOARD_CTRL_CMD_SET_RMII_DATA_MUX, NULL);
        EnetAppUtils_assert(boardStatus == BOARD_SOK);
    }
    /* GESI PHYs connected to ICSSG MDIO */
    if ((ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_ICSSG_MDIO_MUX)) && (gesiDetected))
    {
        boardStatus = Board_control(BOARD_CTRL_CMD_SET_ICSSG_MDIO_MUX, NULL);
        EnetAppUtils_assert(boardStatus == BOARD_SOK);
    }
#endif

    /* GESI PHYs connected to CPSW9G MDIO */
    if ((ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_CPSW9G_MDIO_MUX)) && (gesiDetected))
    {
        boardStatus = Board_control(BOARD_CTRL_CMD_SET_GESI_CPSW_MDIO_MUX, NULL);
        EnetAppUtils_assert(boardStatus == BOARD_SOK);
    }

    /* QpENet QSGMII PHY init */
    if ((ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_QPENET_INIT)) && (qenetDetected))
    {
        EnetBoard_enetExpCfg();
    }

    /* SERDES Sierra 0 clocks setup */
    if (ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_SERDES_SIERRA0_CLKS))
    {
#if defined (SOC_J721E) && defined(BUILD_MCU2_0)
        EnetBoard_configSierra0Clks();
#else
        EnetAppUtils_print("EnetBoard_setupPorts: SERDES Sierra0 is not present in this SoC\n");
        status = ENET_EFAIL;
#endif
    }

    /* SERDES Sierra 1 clocks setup */
    if (ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_SERDES_SIERRA1_CLKS))
    {
#if defined (SOC_J721E) && defined(BUILD_MCU2_0)
        EnetBoard_configSierra1Clks();
#else
        EnetAppUtils_print("EnetBoard_setupPorts: SERDES Sierra1 is not present in this SoC\n");
        status = ENET_EFAIL;
#endif
    }

    /* SERDES Torrent clocks setup */
    if (ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS))
    {
#if defined(BUILD_MCU2_0)
        EnetBoard_configTorrentClks();
#endif
    }

    /* SERDES configuration */
    if (ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_SERDES_SIERRA0_CLKS) ||
        ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_SERDES_SIERRA1_CLKS) ||
        ENET_NOT_ZERO(flags & ENET_BOARD_J7XEVM_SERDES_TORRENT_CLKS))
    {
#if defined(BUILD_MCU2_0)
        if (enetCard == BOARD_ENET_QSGMII)
        {
            status = Board_serdesCfgQsgmii();
        }
        else if (enetCard == BOARD_ENET_SGMII)
        {
            status = Board_serdesCfgSgmii();
        }
        else
        {
            status = ENET_EFAIL;
        }
#else
        EnetAppUtils_print("EnetBoard_setupPorts: QpENet/SGMII not supported on current core\n");
        status = ENET_EFAIL;
#endif
    }

    return status;
}

#if defined(BUILD_MCU2_0)
#if defined (SOC_J721E)
static void EnetBoard_configSierra0Clks(void)
{
    uint32_t moduleId, clkId, clkRateHz;

    /* Configure the required PLLs for SERDES0 */
    moduleId  = TISCI_DEV_SERDES_16G0;
    clkId     = TISCI_DEV_SERDES_16G0_CORE_REF1_CLK;
    clkRateHz = 100000000U;
    EnetAppUtils_clkRateSet(moduleId, clkId, clkRateHz);

    clkId     = TISCI_DEV_SERDES_16G0_CORE_REF_CLK;
    clkRateHz = 100000000U;
    EnetAppUtils_clkRateSet(moduleId, clkId, clkRateHz);
    EnetAppUtils_setDeviceState(moduleId, TISCI_MSG_VALUE_DEVICE_SW_STATE_ON, 0U);
}

static void EnetBoard_disableSierra0Clks(void)
{
    uint32_t moduleId;

    /* Set module set to HW AUTO */
    moduleId = TISCI_DEV_SERDES_16G0;
    EnetAppUtils_setDeviceState(moduleId, TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF, 0U);
}

static void EnetBoard_configSierra1Clks(void)
{
    uint32_t moduleId, clkId, clkRateHz;

    /* Configure the required PLLs for SERDES1 */
    moduleId  = TISCI_DEV_SERDES_16G1;
    clkId     = TISCI_DEV_SERDES_16G1_CORE_REF1_CLK;
    clkRateHz = 100000000U;
    EnetAppUtils_clkRateSet(moduleId, clkId, clkRateHz);

    clkId     = TISCI_DEV_SERDES_16G1_CORE_REF_CLK;
    clkRateHz = 100000000U;
    EnetAppUtils_clkRateSet(moduleId, clkId, clkRateHz);
    EnetAppUtils_setDeviceState(moduleId, TISCI_MSG_VALUE_DEVICE_SW_STATE_ON, 0U);
}

static void EnetBoard_disableSierra1Clks(void)
{
    uint32_t moduleId;

    /* Set module set to HW AUTO */
    moduleId  = TISCI_DEV_SERDES_16G1;
    EnetAppUtils_setDeviceState(moduleId, TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF, 0U);
}
#endif

static void EnetBoard_configTorrentClks(void)
{
    uint32_t moduleId, clkId, clkRateHz;

     /* Configure the required PLLs for SERDES1 */
#if defined (SOC_J721E)
    moduleId  = TISCI_DEV_SERDES_10G0;
    clkId     = TISCI_DEV_SERDES_10G0_CORE_REF_CLK;
#elif defined (SOC_J7200)
    moduleId  = TISCI_DEV_SERDES_10G1;
    clkId     = TISCI_DEV_SERDES_10G1_CORE_REF_CLK;
#endif
    clkRateHz = 100000000U;
    EnetAppUtils_clkRateSet(moduleId, clkId, clkRateHz);
    EnetAppUtils_setDeviceState(moduleId, TISCI_MSG_VALUE_DEVICE_SW_STATE_ON, 0U);
}

static void EnetBoard_disableTorrentClks(void)
{
    uint32_t moduleId;

    /* Set module set to HW AUTO */
#if defined (SOC_J721E)
    moduleId  = TISCI_DEV_SERDES_10G0;
#elif defined (SOC_J7200)
    moduleId  = TISCI_DEV_SERDES_10G1;
#endif
    EnetAppUtils_setDeviceState(moduleId, TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF, 0U);
}
#endif

static void EnetBoard_setEnetControl(Enet_Type enetType,
                                     Enet_MacPort macPort,
                                     EnetMacPort_Interface *mii)
{
    uint32_t portNum = ENET_MACPORT_NORM(macPort);
    uint32_t modeSel = 0U;
    int32_t status = BOARD_FAIL;

    if (EnetMacPort_isRmii(mii))
    {
        modeSel = RMII;
    }
    else if (EnetMacPort_isRgmii(mii))
    {
        modeSel = RGMII;
    }
    else if (EnetMacPort_isSgmii(mii))
    {
        modeSel = SGMII;
    }
    else if (EnetMacPort_isQsgmii(mii))
    {
        modeSel = (mii->sublayerType == ENET_MAC_SUBLAYER_QUAD_SERIAL_MAIN) ? QSGMII : QSGMII_SUB;
    }
    else
    {
        EnetAppUtils_print("Invalid MII type: layer %u suyblayer %u\n", mii->layerType, mii->sublayerType);
        EnetAppUtils_assert(false);
    }

    switch (enetType)
    {
        case ENET_CPSW_2G:
            Board_unlockMMR();
            status = Board_cpsw2gMacModeConfig(modeSel);
            break;
#if defined (SOC_J7200)
        case ENET_CPSW_5G:
            status = Board_cpsw5gEthConfig(portNum, modeSel);
            break;
#endif
#if defined (SOC_J721E)
        case ENET_CPSW_9G:
            status = Board_cpsw9gEthConfig(portNum, modeSel);
            break;
        case ENET_ICSSG_DUALMAC:
        case ENET_ICSSG_SWITCH:
            /* TODO - Use port specific API (when available) for more granularity */
            status = Board_ethConfigIcss();
            status = Board_icssEthPhyConfig();
            break;
#endif
        default:
            break;
    }

    EnetAppUtils_assert(status == BOARD_SOK);
}

void EnetBoard_getMacAddrList(uint8_t macAddr[][ENET_MAC_ADDR_LEN],
                                      uint32_t maxMacEntries,
                                      uint32_t *pAvailMacEntries)
{
#if !defined(ENETAPPUTILS_BYPASS_I2C)
    uint8_t macAddrBuf[ENET_RM_NUM_MACADDR_MAX * BOARD_MAC_ADDR_BYTES];
    Board_STATUS boardStatus;
    uint32_t macAddrCnt, tempCnt;
    uint32_t allocMacEntries = 0;
    uint32_t i, j;

    EnetAppUtils_assert(pAvailMacEntries != NULL);

    if (Board_detectBoard(BOARD_ID_GESI))
    {
        /* Read number of MAC addresses in GESI board */
        boardStatus = Board_readMacAddrCount(BOARD_ID_GESI, &macAddrCnt);
        EnetAppUtils_assert(boardStatus == BOARD_SOK);
        EnetAppUtils_assert(macAddrCnt <= ENET_RM_NUM_MACADDR_MAX);

        /* Read MAC addresses */
        boardStatus = Board_readMacAddr(BOARD_ID_GESI,
                                        macAddrBuf,
                                        sizeof(macAddrBuf),
                                        &tempCnt);
        EnetAppUtils_assert(boardStatus == BOARD_SOK);
        EnetAppUtils_assert(tempCnt == macAddrCnt);

        /* Save only those required to meet the max number of MAC entries */
        macAddrCnt = EnetUtils_min(macAddrCnt, maxMacEntries);
        for (i = 0U; i < macAddrCnt; i++)
        {
            ENET_UTILS_COMPILETIME_ASSERT(ENET_MAC_ADDR_LEN == BOARD_MAC_ADDR_BYTES);
            memcpy(macAddr[i], &macAddrBuf[i * BOARD_MAC_ADDR_BYTES], ENET_MAC_ADDR_LEN);
        }

        allocMacEntries = macAddrCnt;
    }

    if (Board_detectBoard(BOARD_ID_ENET))
    {
        /* Read number of MAC addresses in QUAD Eth board */
        boardStatus = Board_readMacAddrCount(BOARD_ID_ENET, &macAddrCnt);
        EnetAppUtils_assert(boardStatus == BOARD_SOK);
        EnetAppUtils_assert(macAddrCnt <= ENET_RM_NUM_MACADDR_MAX);

        /* Read MAC addresses */
        boardStatus = Board_readMacAddr(BOARD_ID_ENET,
                                        macAddrBuf,
                                        sizeof(macAddrBuf),
                                        &tempCnt);
        EnetAppUtils_assert(boardStatus == BOARD_SOK);
        EnetAppUtils_assert(tempCnt == macAddrCnt);

        /* Save only those required to meet the max number of MAC entries */
        macAddrCnt = EnetUtils_min(macAddrCnt, maxMacEntries - allocMacEntries);
        for (i = 0U, j = allocMacEntries; i < macAddrCnt; i++, j++)
        {
            ENET_UTILS_COMPILETIME_ASSERT(ENET_MAC_ADDR_LEN == BOARD_MAC_ADDR_BYTES);
            memcpy(macAddr[j], &macAddrBuf[i * BOARD_MAC_ADDR_BYTES], ENET_MAC_ADDR_LEN);
        }

        allocMacEntries += macAddrCnt;
    }

    *pAvailMacEntries = allocMacEntries;

    if (allocMacEntries == 0U)
    {
        EnetAppUtils_print("EnetBoard_getMacAddrList Failed - GESI/ENET board not present\n");
        EnetAppUtils_assert(false);
    }
#else
    /*
     * Workaround for EthFw/u-boot I2C conflicts:
     * EthFw reads MAC addresses from GESI and QUAD_ETH boards during EthFw
     * initialization which are stored in EEPROM memories and are read via
     * I2C.  These I2C accesses tend to occur around the same u-boot is also
     * performing I2C accesses, causing transactions to timeout or other
     * similar symptoms.
     *
     * I2C sharing is a known limitation but no current solution exists at
     * this time.  As a temporary workaround, EthFw will use fixed MAC
     * addresses in Linux builds. For RTOS build, MAC addresses will still
     * be read from EEPROM as such I2C contention isn't an problem.
     */
    uint8_t macAddrBuf[][ENET_MAC_ADDR_LEN] =
    {
        { 0x70U, 0xFFU, 0x76U, 0x1DU, 0x92U, 0xC1U },
        { 0x70U, 0xFFU, 0x76U, 0x1DU, 0x92U, 0xC2U },
        { 0x70U, 0xFFU, 0x76U, 0x1DU, 0x92U, 0xC3U },
        { 0x70U, 0xFFU, 0x76U, 0x1DU, 0x92U, 0xC4U },
        { 0x70U, 0xFFU, 0x76U, 0x1DU, 0x92U, 0xC5U },
        { 0x70U, 0xFFU, 0x76U, 0x1DU, 0X8BU, 0xC4U },
        { 0x70U, 0xFFU, 0x76U, 0x1DU, 0X8BU, 0xC5U },
        { 0x70U, 0xFFU, 0x76U, 0x1DU, 0X8BU, 0xC6U },
        { 0x70U, 0xFFU, 0x76U, 0x1DU, 0X8BU, 0xC7U },
    };
    uint32_t macAddrCnt = ENET_ARRAYSIZE(macAddrBuf);

    /* Save only those required to meet the max number of MAC entries */
    *pAvailMacEntries = EnetUtils_min(macAddrCnt, maxMacEntries);
    memcpy(&macAddr[0U][0U], &macAddrBuf[0U][0U], *pAvailMacEntries * ENET_MAC_ADDR_LEN);
#endif
}

