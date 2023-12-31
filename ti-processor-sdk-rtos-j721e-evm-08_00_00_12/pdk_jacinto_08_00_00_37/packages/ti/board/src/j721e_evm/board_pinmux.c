/******************************************************************************
 * Copyright (c) 2019-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *****************************************************************************/

/** 
 *  \file board_pinmux.c
 *
 *  \brief This file enables pinmux for the board 
 *
 */

#include <ti/csl/soc.h>
#include "board_internal.h"
#include "board_pinmux.h"

static Board_PinmuxConfig_t gBoardPinmuxCfg = {BOARD_PINMUX_CUSTOM,
                                               BOARD_PINMUX_INFO_VOUT,
                                               BOARD_PINMUX_CSI_COMMON,
                                               BOARD_PINMUX_ENET_QSGMII};

/**
 *  \brief  Gets base address of padconfig registers
 *
 *  \param   domain [IN]  SoC domain for pinmux
 *  \n                     BOARD_SOC_DOMAIN_MAIN - Main domain
 *  \n                     BOARD_SOC_DOMAIN_WKUP - Wakeup domain
 *
 *  \return   Valid address in case success or 0 in case of failure
 */
static void Board_pinmuxWriteProxy1Reg(uint8_t domain,
                                       uint32_t baseAddr,
                                       uint32_t regVal)
{
    uint32_t    claimAddr;
    uint32_t    claimOffset;
    uint32_t    claimShift;
    uint32_t    claimRegVal;

    if(domain == BOARD_SOC_DOMAIN_MAIN)
    {
        claimAddr = BOARD_MAIN_MMR_P7_CLAIM_ADDR;
        /* Get the register offset from base of P7 claim offset range */
        claimOffset = (baseAddr - BOARD_MAIN_PMUX_CTRL_ADDR);
    }
    else
    {
        claimAddr = BOARD_WKUP_MMR_P7_CLAIM_ADDR;
        /* Get the register offset from base of P7 claim offset range */
        claimOffset = (baseAddr - BOARD_WKUP_PMUX_CTRL_ADDR);
    }

    /* Each claim register configures 128 byte address range.
       Calculate the register offset for P7 claim register block and
       and shift values within the claim register */
    claimShift  = claimOffset % BOARD_MMR_CLAIM_ADDR_PER_REG;
    claimOffset = claimOffset / BOARD_MMR_CLAIM_ADDR_PER_REG;

    claimRegVal = HW_RD_REG32((claimAddr + 4*claimOffset));
    claimRegVal |= (1 << claimShift);

    /* Claim the register access */
    HW_WR_REG32((claimAddr + 4*claimOffset), claimRegVal);

    /* Write PAD config MMR register */
    HW_WR_REG32(baseAddr, regVal);
}

/**
 *  \brief  Gets base address of padconfig registers
 *
 *  \param   domain [IN]  SoC domain for pinmux
 *  \n                     BOARD_SOC_DOMAIN_MAIN - Main domain
 *  \n                     BOARD_SOC_DOMAIN_WKUP - Wakeup domain
 *
 *  \return   Valid address in case success or 0 in case of failure
 */
static uint32_t Board_pinmuxGetBaseAddr(uint8_t domain)
{
    uint32_t baseAddr;

    switch(domain)
    {
        case BOARD_SOC_DOMAIN_MAIN:
            baseAddr = BOARD_MAIN_PMUX_CTRL_ADDR;
        break;
        case BOARD_SOC_DOMAIN_WKUP:
            baseAddr = BOARD_WKUP_PMUX_CTRL_ADDR;
        break;
        default:
            baseAddr = 0;
        break;
    }

    return baseAddr;
}

/**
 *  \brief  Sets pinmux mode for a pin in main domain
 *
 *  Only pinmux mode is updated by this function. Other values of
 *  padconfig register remains unchanged after this function call.
 *
 *  \param    offset [IN]    Pad config offset of the pin
 *  \param    mode   [IN]    Pad config mux mode.
 *
 *  \return   None
 */
void Board_pinMuxSetMode(uint32_t offset, uint32_t mode)
{
    uint32_t baseAddr;
    uint32_t regVal;

    Board_unlockMMR();

    baseAddr = Board_pinmuxGetBaseAddr(BOARD_SOC_DOMAIN_MAIN);

    regVal = HW_RD_REG32((baseAddr + offset));
    regVal &= ~(BOARD_MODE_PIN_MASK);
    mode &= BOARD_MODE_PIN_MASK;
    regVal |= mode;
    Board_pinmuxWriteProxy1Reg(BOARD_SOC_DOMAIN_MAIN, 
                               (baseAddr + offset), 
                               regVal);
}

/**
 *  \brief  Sets pinmux mode for a pin in wake-up domain
 *
 *  Only pinmux mode is updated by this function. Other values of
 *  padconfig register remains unchanged after this function call.
 *
 *  \param    offset [IN]    Pad config offset of the pin
 *  \param    mode   [IN]    Pad config mux mode.
 *
 *  \return   None
 */
void Board_pinMuxSetModeWkup(uint32_t offset, uint32_t mode)
{
    uint32_t baseAddr;
    uint32_t regVal;

    Board_unlockMMR();

    baseAddr = Board_pinmuxGetBaseAddr(BOARD_SOC_DOMAIN_WKUP);

    regVal = HW_RD_REG32((baseAddr + offset));
    regVal &= ~(BOARD_MODE_PIN_MASK);
    mode &= BOARD_MODE_PIN_MASK;
    regVal |= mode;
    Board_pinmuxWriteProxy1Reg(BOARD_SOC_DOMAIN_WKUP, 
                               (baseAddr + offset), 
                               regVal);
}

/**
 *  \brief Sets padconfig register of a pin at given offset
 *
 *  Configures whole padconfig register of the pin at given offset
 *  with the value in 'muxData'.
 *
 *  \param   domain  [IN]  SoC domain for pinmux
 *  \n                      BOARD_SOC_DOMAIN_MAIN - Main domain
 *  \n                      BOARD_SOC_DOMAIN_WKUP - Wakeup domain
 *
 *  \param   offset  [IN]  Pad config offset of the pin
 *  \param   muxData [IN]  Value to be written to padconfig register
 *
 *  \return   BOARD_SOK in case of success or appropriate error code
 *
 */
Board_STATUS Board_pinmuxSetReg(uint8_t  domain,
                                uint32_t offset,
                                uint32_t muxData)
{
    uint32_t baseAddr;
    Board_STATUS status = BOARD_SOK;

    Board_unlockMMR();

    baseAddr = Board_pinmuxGetBaseAddr(domain);
    if(baseAddr != 0)
    {
        Board_pinmuxWriteProxy1Reg(domain, 
                                   (baseAddr + offset), 
                                   muxData);
    }
    else
    {
        status = BOARD_INVALID_PARAM;
    }

    return status;
}

/**
 *  \brief Gets padconfig register of a pin at given offset
 *
 *  \param   domain  [IN]  SoC domain for pinmux
 *  \n                      BOARD_SOC_DOMAIN_MAIN - Main domain
 *  \n                      BOARD_SOC_DOMAIN_WKUP - Wakeup domain
 *  \param   offset  [IN]  Pad config offset of the pin
 *  \param   muxData [OUT] Value of padconfig register
 *
 *  \return   BOARD_SOK in case of success or appropriate error code
 *
 */
Board_STATUS Board_pinmuxGetReg(uint8_t  domain,
                                uint32_t offset,
                                uint32_t *muxData)
{
    uint32_t baseAddr;
    Board_STATUS status = BOARD_SOK;

    baseAddr = Board_pinmuxGetBaseAddr(domain);
    if(baseAddr != 0)
    {
        *muxData = HW_RD_REG32((baseAddr + offset));
    }
    else
    {
        status = BOARD_INVALID_PARAM;
    }

    return status;
}

/**
 *  \brief Sets the board pinmux configuration.
 *
 *  There are multiple addon cards that can connected to baseboard and
 *  multiple addon cards can be connected to one expansion connector.
 *  Pinmux configured through Board_init will be set to a default
 *  combination of the boards which can be changed using this function.
 *
 *  \n Usage:
 *  \n - Call Board_pinmuxGetCfg to get default pinmux config
 *  \n - Call Board_pinmuxSetCfg to change pinmux config
 *  \n - Call Board_init with pinmux flag to apply the updated pinmux config
 *
 *  \param   pinmuxCfg [IN]  Pinmux configurations
 *
 *  \return  BOARD_SOK in case of success or appropriate error code
 *
 */
Board_STATUS Board_pinmuxSetCfg(Board_PinmuxConfig_t *pinmuxCfg)
{
    gBoardPinmuxCfg = *pinmuxCfg;

    return BOARD_SOK;
}

/**
 *  \brief Gets the board pinmux configuration.
 *
 *  There are multiple addon cards that can connected to baseboard and
 *  multiple addon cards can be connected to one expansion connector.
 *  Pinmux configured through Board_init will be set to a default
 *  combination of the boards which can be read using this function.
 *
 *  \param   pinmuxCfg [IN]  Pinmux configurations
 *
 *  \return  BOARD_SOK in case of success or appropriate error code
 *
 */
Board_STATUS Board_pinmuxGetCfg(Board_PinmuxConfig_t *pinmuxCfg)
{
    *pinmuxCfg = gBoardPinmuxCfg;

    return BOARD_SOK;
}

/**
 * \brief  Board pinmuxing update function
 *
 * Provides the option to configure/update the pinmux.
 * This function can be used to change the pinmux set by
 * Board_init by default.
 *
 * \param   pinmuxData [IN]  Pinmux data structure
 * \param   domain     [IN]  SoC domain for pinmux
 *  \n                        BOARD_SOC_DOMAIN_MAIN - Main domain
 *  \n                        BOARD_SOC_DOMAIN_WKUP - Wakeup domain
 *
 * \return  BOARD_SOK in case of success or appropriate error code
 *
 */
Board_STATUS Board_pinmuxUpdate (pinmuxBoardCfg_t *pinmuxData,
                                 uint32_t domain)
{
    pinmuxModuleCfg_t *pModuleData = NULL;
    pinmuxPerCfg_t *pInstanceData = NULL;
    int32_t i, j, k;
    uint32_t rdRegVal;
    uint32_t baseAddr;
    Board_STATUS status = BOARD_SOK;

    Board_unlockMMR();

    /* MAIN domain pinmux needs RAT configuration for C66x core. */
    if(domain == BOARD_SOC_DOMAIN_MAIN)
    {
        Board_setRATCfg();
    }

    baseAddr = Board_pinmuxGetBaseAddr(domain);
    if(baseAddr != 0)
    {
        for(i = 0; PINMUX_END != pinmuxData[i].moduleId; i++)
        {
            pModuleData = pinmuxData[i].modulePinCfg;
            for(j = 0; (PINMUX_END != pModuleData[j].modInstNum); j++)
            {
                if(pModuleData[j].doPinConfig == TRUE)
                {
                    pInstanceData = pModuleData[j].instPins;
                    for(k = 0; (PINMUX_END != pInstanceData[k].pinOffset); k++)
                    {
                        rdRegVal = HW_RD_REG32((baseAddr + pInstanceData[k].pinOffset));
                        rdRegVal = (rdRegVal & BOARD_PINMUX_BIT_MASK);
                        Board_pinmuxWriteProxy1Reg(domain, 
                                                   (baseAddr + pInstanceData[k].pinOffset), 
                                                   (pInstanceData[k].pinSettings));
                    }
                }
            }
        }
    }
    else
    {
        status = BOARD_INVALID_PARAM;
    }

    if(domain == BOARD_SOC_DOMAIN_MAIN)
    {
        /* Clear the RAT configuration to allow applications to use the region */
        Board_restoreRATCfg();
    }

    return status;
}

/**
 * \brief  Board pinmuxing enable function
 *
 * Enables pinmux for the board interfaces. Pin mux is done based
 * on the default/primary functionality of the board. Any pins shared by
 * multiple interfaces need to be reconfigured to access the secondary
 * functionality.
 *
 * \param   void
 *
 * \return  BOARD_SOK in case of success or appropriate error code
 *
 */
Board_STATUS Board_pinmuxConfig (void)
{
    Board_STATUS status = BOARD_SOK;

    /* Pinmux for baseboard */
    Board_pinmuxUpdate(gJ721E_MainPinmuxData,
                       BOARD_SOC_DOMAIN_MAIN);
    Board_pinmuxUpdate(gJ721E_WkupPinmuxData,
                       BOARD_SOC_DOMAIN_WKUP);

    /* Pinmux for Application cards */
    if(gBoardPinmuxCfg.autoCfg)
    {
        /* Auto detect the application boards connected and configure the pinmux */
        if(Board_detectBoard(BOARD_ID_GESI) == TRUE)
        {
            gBoardPinmuxCfg.gesiExp = BOARD_PINMUX_GESI_CPSW;
        }
        else
        {
            if(Board_detectBoard(BOARD_ID_INFOTAINMENT) == TRUE)
            {
                gBoardPinmuxCfg.gesiExp = BOARD_PINMUX_INFO_VOUT;
            }
        }
    }

    if((gBoardPinmuxCfg.gesiExp == BOARD_PINMUX_GESI_ICSSG) ||
       (gBoardPinmuxCfg.gesiExp == BOARD_PINMUX_GESI_CPSW))
    {
        /* By default ICSSG RGMII is enabled */
        Board_pinmuxUpdate(gJ721E_MainPinmuxDataGesiIcssg,
                           BOARD_SOC_DOMAIN_MAIN);
        Board_pinmuxUpdate(gJ721E_WkupPinmuxDataGesiIcssg,
                           BOARD_SOC_DOMAIN_WKUP);

        if(gBoardPinmuxCfg.gesiExp == BOARD_PINMUX_GESI_CPSW)
        {
            /* Overwrite the ICSSG RGMII muc configurations with CPSW9G RGMII */
            Board_pinmuxUpdate(gJ721E_MainPinmuxDataGesiCpsw9g,
                               BOARD_SOC_DOMAIN_MAIN);
            Board_pinmuxUpdate(gJ721E_WkupPinmuxDataGesiCpsw9g,
                               BOARD_SOC_DOMAIN_WKUP);
        }
    }
    else if(gBoardPinmuxCfg.gesiExp == BOARD_PINMUX_INFO_VOUT)
    {
        Board_pinmuxUpdate(gJ721E_MainPinmuxDataInfo,
                           BOARD_SOC_DOMAIN_MAIN);
        Board_pinmuxUpdate(gJ721E_WkupPinmuxDataInfo,
                           BOARD_SOC_DOMAIN_WKUP);
    }
    else
    {
        return (BOARD_INVALID_PARAM);
    }

    if(gBoardPinmuxCfg.fssCfg == BOARD_PINMUX_FSS_HPB)
    {
        Board_pinmuxUpdate(gJ721E_WkupPinmuxDataHpb,
                           BOARD_SOC_DOMAIN_WKUP);
    }

    return status;
}

/**
 * \brief  Board pinmuxing enable function for main domain
 *
 * Enables pinmux for the board interfaces connected to main domain.
 * Pin mux is done based on the default/primary functionality of the board.
 * Any pins shared by multiple interfaces need to be reconfigured to access
 * the secondary functionality.
 *
 * \param   void
 *
 * \return  BOARD_SOK in case of success or appropriate error code
 *
 */
Board_STATUS Board_pinmuxConfigMain (void)
{
    Board_STATUS status = BOARD_SOK;

    /* Pinmux for baseboard */
    Board_pinmuxUpdate(gJ721E_MainPinmuxData,
                       BOARD_SOC_DOMAIN_MAIN);

    if(gBoardPinmuxCfg.autoCfg)
    {
        /* Auto detect the application boards connected and configure the pinmux */
        if(Board_detectBoard(BOARD_ID_GESI) == TRUE)
        {
            gBoardPinmuxCfg.gesiExp = BOARD_PINMUX_GESI_CPSW;
        }
        else
        {
            if(Board_detectBoard(BOARD_ID_INFOTAINMENT) == TRUE)
            {
                gBoardPinmuxCfg.gesiExp = BOARD_PINMUX_INFO_VOUT;
            }
        }
    }

    if((gBoardPinmuxCfg.gesiExp == BOARD_PINMUX_GESI_ICSSG) ||
       (gBoardPinmuxCfg.gesiExp == BOARD_PINMUX_GESI_CPSW))
    {
        /* By default ICSSG RGMII is enabled */
        Board_pinmuxUpdate(gJ721E_MainPinmuxDataGesiIcssg,
                           BOARD_SOC_DOMAIN_MAIN);

        if(gBoardPinmuxCfg.gesiExp == BOARD_PINMUX_GESI_CPSW)
        {
            /* Overwrite the ICSSG RGMII muc configurations with CPSW9G RGMII */
            Board_pinmuxUpdate(gJ721E_MainPinmuxDataGesiCpsw9g,
                               BOARD_SOC_DOMAIN_MAIN);
        }
    }
    else if(gBoardPinmuxCfg.gesiExp == BOARD_PINMUX_INFO_VOUT)
    {
        Board_pinmuxUpdate(gJ721E_MainPinmuxDataInfo,
                           BOARD_SOC_DOMAIN_MAIN);
    }
    else
    {
        return (BOARD_INVALID_PARAM);
    }

    return status;
}

/**
 * \brief  Board pinmuxing enable function for wakeup/mcu domain
 *
 * Enables pinmux for the board interfaces connected to wakeup/mcu domain.
 * Pin mux is done based on the default/primary functionality of the board.
 * Any pins shared by multiple interfaces need to be reconfigured to access
 * the secondary functionality.
 *
 * \param   void
 *
 * \return  BOARD_SOK in case of success or appropriate error code
 *
 */
Board_STATUS Board_pinmuxConfigWkup (void)
{
    Board_STATUS status = BOARD_SOK;

    Board_pinmuxUpdate(gJ721E_WkupPinmuxData,
                       BOARD_SOC_DOMAIN_WKUP);

    if(gBoardPinmuxCfg.autoCfg)
    {
        /* Auto detect the application boards connected and configure the pinmux */
        if(Board_detectBoard(BOARD_ID_GESI) == TRUE)
        {
            gBoardPinmuxCfg.gesiExp = BOARD_PINMUX_GESI_CPSW;
        }
        else
        {
            if(Board_detectBoard(BOARD_ID_INFOTAINMENT) == TRUE)
            {
                gBoardPinmuxCfg.gesiExp = BOARD_PINMUX_INFO_VOUT;
            }
        }
    }

    if((gBoardPinmuxCfg.gesiExp == BOARD_PINMUX_GESI_ICSSG) ||
       (gBoardPinmuxCfg.gesiExp == BOARD_PINMUX_GESI_CPSW))
    {
        Board_pinmuxUpdate(gJ721E_WkupPinmuxDataGesiIcssg,
                           BOARD_SOC_DOMAIN_WKUP);

        if(gBoardPinmuxCfg.gesiExp == BOARD_PINMUX_GESI_CPSW)
        {
            Board_pinmuxUpdate(gJ721E_WkupPinmuxDataGesiCpsw9g,
                               BOARD_SOC_DOMAIN_WKUP);
        }
    }
    else if(gBoardPinmuxCfg.gesiExp == BOARD_PINMUX_INFO_VOUT)
    {
        Board_pinmuxUpdate(gJ721E_WkupPinmuxDataInfo,
                           BOARD_SOC_DOMAIN_WKUP);
    }
    else
    {
        return (BOARD_INVALID_PARAM);
    }

    if(gBoardPinmuxCfg.fssCfg == BOARD_PINMUX_FSS_HPB)
    {
        Board_pinmuxUpdate(gJ721E_WkupPinmuxDataHpb,
                           BOARD_SOC_DOMAIN_WKUP);
    }

    return status;
}

void Board_uartTxPinmuxConfig(void)
{
    /* Unlock partition lock kick */
    HW_WR_REG32(BOARD_MCU_UART_TX_LOCK_KICK_ADDR, BOARD_KICK0_UNLOCK_VAL);
    HW_WR_REG32(BOARD_MCU_UART_TX_LOCK_KICK_ADDR + 4U, BOARD_KICK1_UNLOCK_VAL);

    /* Configure pinmux for UART Tx pin */
    HW_WR_REG32(BOARD_MCU_UART_TX_PINMUX_ADDR, BOARD_MCU_UART_TX_PINMUX_VAL);
}
