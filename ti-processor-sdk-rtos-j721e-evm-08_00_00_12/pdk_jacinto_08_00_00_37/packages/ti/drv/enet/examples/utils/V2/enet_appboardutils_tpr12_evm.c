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
 * \file     enet_appboardutils_tpr12_evm.c
 *
 * \brief    This file contains the board specific utilities for TPR12 EVM.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <ti/csl/soc.h>
#include <ti/osal/osal.h>
#include <ti/drv/enet/include/phy/dp83867.h>
#include <ti/board/board.h>
#include <ti/drv/enet/enet.h>

#include <ti/drv/enet/examples/utils/include/enet_apputils.h>
#include <ti/drv/enet/examples/utils/include/enet_appboardutils.h>
#include <ti/board/src/tpr12_evm/include/board_ethernet_config.h>

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

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

uint32_t EnetBoard_getPhyAddr(Enet_Type enetType,
                                      Enet_MacPort portNum)
{
    uint32_t phyAddr = ENETPHY_INVALID_PHYADDR;

    switch (enetType)
    {
        case ENET_CPSW_2G:
            phyAddr = 0x00U;
            break;

        default:
            EnetAppUtils_assert(false);
            break;
    }

    return phyAddr;
}

void EnetBoard_setPhyConfigSgmii(Enet_MacPort portNum,
                                         CpswMacPort_Cfg *macConfig,
                                         EnetMacPort_Interface *interface,
                                         EnetPhy_Cfg *phyConfig)
{
    EnetAppUtils_assert(false);
}

void EnetBoard_setPhyConfigQsgmii(Enet_Type enetType,
                                          Enet_MacPort portNum,
                                          CpswMacPort_Cfg *macConfig,
                                          EnetMacPort_Interface *interface,
                                          EnetPhy_Cfg *phyConfig)
{
    EnetAppUtils_assert(false);
}

void EnetBoard_setPhyConfigRmii(Enet_MacPort portNum,
                                        CpswMacPort_Cfg *macConfig,
                                        EnetMacPort_Interface *interface,
                                        EnetPhy_Cfg *phyConfig)
{
    EnetAppUtils_assert(false);
}

void EnetBoard_setPhyConfigRgmii(Enet_Type enetType,
                                         Enet_MacPort portNum,
                                         CpswMacPort_Cfg *macConfig,
                                         EnetMacPort_Interface *interface,
                                         EnetPhy_Cfg *phyConfig)
{
    Dp83867_Cfg extendedCfg;

    Dp83867_initCfg(&extendedCfg);
    EnetPhy_initCfg(phyConfig);
    phyConfig->phyAddr = EnetBoard_getPhyAddr(enetType, portNum);

    switch (enetType)
    {
        case ENET_CPSW_2G:
            EnetAppUtils_assert(portNum == ENET_MAC_PORT_1);

            /* DP83867 specific configuration */
            extendedCfg.txClkShiftEn  = true;
            extendedCfg.rxClkShiftEn  = true;
            extendedCfg.txFifoDepth   = 4U;
            extendedCfg.idleCntThresh = 4U; /* Improves short cable performance */
            extendedCfg.impedanceInMilliOhms = 35000; /* 35 ohms */
            extendedCfg.gpio0Mode     = DP83867_GPIO0_LED3;
            extendedCfg.ledMode[1]    = DP83867_LED_LINKED_100BTX;
            extendedCfg.ledMode[2]    = DP83867_LED_RXTXACT;
            extendedCfg.ledMode[3]    = DP83867_LED_LINKED_1000BT;

            /* RGMII interface type */
            interface->layerType    = ENET_MAC_LAYER_GMII;
            interface->sublayerType = ENET_MAC_SUBLAYER_REDUCED;
            interface->variantType  = ENET_MAC_VARIANT_NONE;
            break;

        case ENET_CPSW_9G:
        case ENET_CPSW_5G:
            EnetAppUtils_assert(false);
            break;
    }

    EnetPhy_setExtendedCfg(phyConfig,
                              &extendedCfg,
                              sizeof(extendedCfg));
}

void EnetBoard_setPhyConfig(Enet_Type enetType,
                                    Enet_MacPort portNum,
                                    CpswMacPort_Cfg *macConfig,
                                    EnetMacPort_Interface *interface,
                                    EnetPhy_Cfg *phyConfig)
{
    return EnetBoard_setPhyConfigRgmii(enetType,
                                               portNum,
                                               macConfig,
                                               interface,
                                               phyConfig);
}

void EnetBoard_setNoPhyConfig(EnetMacPort_Interface *interface,
                                      EnetPhy_Cfg *phyConfig)
{
    phyConfig->phyAddr      = ENETPHY_INVALID_PHYADDR;
    interface->layerType    = ENET_MAC_LAYER_GMII;
    interface->sublayerType = ENET_MAC_SUBLAYER_REDUCED;
    interface->variantType  = ENET_MAC_VARIANT_FORCED;
}

void EnetBoard_setLpbkCfg(bool phyLpbk,
                                  Enet_MacPort portNum,
                                  const EnetMacPort_Interface *interface,
                                  CpswMacPort_Cfg *macConfig,
                                  EnetPhy_Cfg *phyConfig,
                                  EnetMacPort_LinkCfg *linkConfig)
{
    /* Set speed according to the interface type */
    if (interface->layerType == ENET_MAC_LAYER_MII)
    {
        linkConfig->speed = ENET_SPEED_100MBIT;
    }
    else
    {
        linkConfig->speed = ENET_SPEED_1GBIT;
    }

    linkConfig->duplexity = ENET_DUPLEX_FULL;

    /* MAC and PHY loopbacks are mutually exclusive */
    phyConfig->loopbackEn = phyLpbk;
    macConfig->loopbackEn = !phyLpbk;
}

void EnetBoard_getMacAddrList(uint8_t macAddr[][ENET_MAC_ADDR_LEN],
                                      uint32_t maxMacEntries,
                                      uint32_t *pAvailMacEntries)
{
    /* No MAC address programmed in EEPROM of AM65xx EVM */
    *pAvailMacEntries = 0;
}

static void EnetBoard_setMcuEnetControl(uint32_t modeSel)
{
#if 0 /* TODO */
    CSL_mcu_ctrl_mmr_cfg0Regs *mcuRegs;

    mcuRegs = (CSL_mcu_ctrl_mmr_cfg0Regs *)(uintptr_t)CSL_MCU_CTRL_MMR0_CFG0_BASE;
    EnetAppUtils_mcuMmrCtrl(ENETAPPUTILS_MMR_LOCK1, ENETAPPUTILS_UNLOCK_MMR);
    CSL_REG32_FINS(&mcuRegs->MCU_ENET_CTRL,
                   MCU_CTRL_MMR_CFG0_MCU_ENET_CTRL_MODE_SEL,
                   modeSel);
    EnetAppUtils_mcuMmrCtrl(ENETAPPUTILS_MMR_LOCK1, ENETAPPUTILS_LOCK_MMR);
#endif
}

void EnetBoard_setEnetControl(Enet_Type enetType,
                                      uint32_t instId,
                                      Enet_MacPort portNum,
                                      uint32_t modeSel)
{
    EnetAppUtils_assert(portNum == ENET_MAC_PORT_1);
    EnetAppUtils_assert(enetType == ENET_CPSW_2G);
    EnetAppUtils_assert(((modeSel == RMII) || (modeSel == RGMII)));
    EnetBoard_setMcuEnetControl(modeSel);
}
