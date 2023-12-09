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
 * \file  enet_board_tpr12evm.c
 *
 * \brief This file contains the implementation of TPR12 EVM support.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <ti/drv/enet/enet.h>
#include <ti/drv/enet/include/phy/enetphy.h>
#include <ti/drv/enet/include/phy/dp83867.h>
#include <ti/drv/enet/examples/utils/include/enet_board_tpr12evm.h>
#include <ti/drv/enet/examples/utils/include/enet_apputils.h>
#include <ti/board/board.h>
#include <ti/csl/soc.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
   TPR:MSS_CTRL:CPSW_CONTROL

   Address offset    0x0000016C
   Physical address  0x0212016C
   Instance          MSS_CTRL
   CPSW_CONTROL_RGMII1_ID_MODE     16  Writing 1'b1 would disable the internal clock delays. And those delays need to be handled on board.
   CPSW_CONTROL_RMII_REF_CLK_OE_N  8   To select the rmii_ref_clk from PAD or from MSS_RCM. 0: clock will be from mss_rcm through IO internal loopback 1: will be from
   CPSW_CONTROL_PORT1_MODE_SEL     2:0 Port 1 Interface
                                         00 = GMII/MII
                                         01 = RMII
                                         10 = RGMII
                                         11 = Not Supported
*/

#define MSS_CPSW_CONTROL_PORT_MODE_RMII                                   (0x1U)
#define MSS_CPSW_CONTROL_PORT_MODE_RGMII                                  (0x2U)

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
 * AM65xx board configuration.
 *
 * 1 x RGMII PHY connected to AM65xx CPSW_2G MAC port.
 */
static const EnetBoard_PortCfg gEnetCpbBoard_tpr12EthPort[] =
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
            .skipExtendedCfg = true,
            .extendedCfg     = &gEnetCpbBoard_dp83867PhyCfg,
            .extendedCfgSize = sizeof(gEnetCpbBoard_dp83867PhyCfg),
        },
        .flags    = 0U,
    },
};

/*
 * TPR12 dummy board used for MAC loopback setup.
 */
static const EnetBoard_PortCfg gEnetLpbkBoard_tpr12EthPort[] =
{
    {    /* RGMII MAC loopback */
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
    {    /* RMII MAC loopback */
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
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

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
                                        gEnetCpbBoard_tpr12EthPort,
                                        ENETPHY_ARRAYSIZE(gEnetCpbBoard_tpr12EthPort));
    }
    if ((portCfg == NULL) &&
        ENET_NOT_ZERO(ethPort->boardId & ENETBOARD_LOOPBACK_ID))
    {
        portCfg = EnetBoard_findPortCfg(ethPort,
                                        gEnetLpbkBoard_tpr12EthPort,
                                        ENETPHY_ARRAYSIZE(gEnetLpbkBoard_tpr12EthPort));
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
    Board_initCfg boardCfg;
    Board_STATUS boardStatus;

    /* Board configuration parameters. Further configuration done explicitly (i.e. ENETCTRL) */
    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_MODULE_CLOCK |
               BOARD_INIT_ENETCTRL_CPSW2G |
               BOARD_INIT_ETH_PHY |
               BOARD_INIT_UART_STDIO;
    boardStatus = Board_init(boardCfg);
    EnetAppUtils_assert(boardStatus == BOARD_SOK);

    return (boardStatus == BOARD_SOK) ? ENET_SOK : ENET_EFAIL;
}

void EnetBoard_deinit(void)
{
    Board_initCfg boardCfg;
    Board_STATUS boardStatus;

    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_MODULE_CLOCK |
               BOARD_INIT_ENETCTRL_CPSW2G |
               BOARD_INIT_ETH_PHY |
               BOARD_INIT_UART_STDIO;
    boardStatus = Board_deinit(boardCfg);
    EnetAppUtils_assert(boardStatus == BOARD_SOK);
}

int32_t EnetBoard_setupPorts(EnetBoard_EthPort *ethPorts,
                             uint32_t numEthPorts)
{
    CSL_mss_ctrlRegs *mssCtrlRegs = (CSL_mss_ctrlRegs *)CSL_MSS_CTRL_U_BASE;

    DebugP_assert(numEthPorts == 1);
    DebugP_assert(ethPorts[0].macPort == ENET_MAC_PORT_1);
    DebugP_assert(ethPorts[0].mii.sublayerType == ENET_MAC_SUBLAYER_REDUCED);
    switch(ethPorts[0].mii.layerType)
    {
        case ENET_MAC_LAYER_MII:
            CSL_FINS( mssCtrlRegs->CPSW_CONTROL,MSS_CTRL_CPSW_CONTROL_CPSW_CONTROL_PORT1_MODE_SEL, MSS_CPSW_CONTROL_PORT_MODE_RMII);
            break;
        case ENET_MAC_LAYER_GMII:
            CSL_FINS( mssCtrlRegs->CPSW_CONTROL,MSS_CTRL_CPSW_CONTROL_CPSW_CONTROL_PORT1_MODE_SEL, MSS_CPSW_CONTROL_PORT_MODE_RGMII);
            break;
        default:
            DebugP_assert(false);
    }
    /* Nothing else to do */
    return ENET_SOK;
}
