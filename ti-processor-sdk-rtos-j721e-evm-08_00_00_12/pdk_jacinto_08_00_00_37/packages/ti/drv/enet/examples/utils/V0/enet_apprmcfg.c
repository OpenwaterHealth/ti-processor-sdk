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
 * \file     enet_apprmcfg.c
 *
 * \brief    This file contains the CPSW driver resource management
 *           configuration used to initialize the RM init parameters passed
 *           during driver init.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/enet/enet.h>
#include <ti/drv/enet/include/per/cpsw.h>
#include <ti/drv/enet/examples/utils/include/enet_appsoc.h>
#include <ti/drv/ipc/ipc.h>

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

/*!
 *  \brief CPSW2G default configuration
 *
 *   Note: If user wishes to change the Resource Partition the following
 *   things must be considered:
 *   1. Sum of numTxCh allocated to each core should not exceed 8.
 *   2. Sum of numRxFlows allocated to each core should not exceed 7 (not 8),
 *      as one Rx flow is reserved to the master core.
 *
 */
static EnetRm_ResPrms gEnetAppRmDefCfg_2G =
{
    .coreDmaResInfo =
    {
        [0] =
            /* MPU1_0 */
        {
            .coreId        = IPC_MPU1_0,
            .numTxCh       = 2U,    /* numTxCh */
            .numRxFlows    = 1U,    /* numRxFlows */
            .numMacAddress = 0U,    /* numMacAddress */
        },
        [1] =
            /* MCU1_0 */
        {
            .coreId        = IPC_MCU1_0,
            .numTxCh       = 4U,    /* numTxCh */
            .numRxFlows    = 5U,    /* numRxFlows */
            .numMacAddress = 1U,    /* numMacAddress */
        },
        [2] =
            /* MCU1_1 */
        {
            .coreId        = IPC_MCU1_1,
            .numTxCh       = 2U,    /* numTxCh */
            .numRxFlows    = 2U,    /* numRxFlows */
            .numMacAddress = 0U,    /* numMacAddress */
        },
    },
    .numCores = 3U,
};

/*!
 *  \brief ICSSG default configuration
 *         Note: If user wishes to change the Resource Partition the following
 *         things must be considered:
 *         1. Sum of numTxCh allocated to each core should not exceed 8.
 *         2. Sum of numRxFlows allocated to each core should not exceed 10 (not 64),
 *            as one Rx flow is reserved to the master core.
 *
 */
static EnetRm_ResPrms gEnetAppRmDefCfg_iccsgDMac =
{
    .coreDmaResInfo =
    {
        [0] =
        {
            .coreId        = IPC_MCU1_0,
            .numTxCh       = 4U,
            .numRxFlows    = 8U,
            .numMacAddress = 1U,
        },
    },
    .numCores = 1U,
};

static EnetRm_ResPrms gEnetAppRmDefCfg_iccsgSwt =
{
    .coreDmaResInfo =
    {
        [0] =
        {
            .coreId        = IPC_MCU1_0,
            .numTxCh       = 8U,
            .numRxFlows    = 8U,
            .numMacAddress = 1U,
        },
    },
    .numCores = 1U,
};

/* Cores IOCTL Privileges */
static const EnetRm_IoctlPermissionTable gEnetAppIoctlPermission_2G =
{
    .defaultPermittedCoreMask = (ENET_BIT(IPC_MPU1_0) |
                                 ENET_BIT(IPC_MCU1_0) |
                                 ENET_BIT(IPC_MCU1_1)),
    .numEntries = 0,
};

static const EnetRm_IoctlPermissionTable gEnetAppIoctlPermission_iccsg =
{
    .defaultPermittedCoreMask = (ENET_BIT(IPC_MPU1_0) |
                                 ENET_BIT(IPC_MCU1_0) |
                                 ENET_BIT(IPC_MCU1_1)),
    .numEntries = 0,
};

/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

const EnetRm_ResPrms *EnetAppRm_getResPartInfo(Enet_Type enetType)
{
    const EnetRm_ResPrms *rmInitPrms = NULL;

    switch (enetType)
    {
        case ENET_CPSW_2G:
        {
            rmInitPrms = &gEnetAppRmDefCfg_2G;
            break;
        }

        case ENET_ICSSG_DUALMAC:
        {
            rmInitPrms = &gEnetAppRmDefCfg_iccsgDMac;
            break;
        }

        case ENET_ICSSG_SWITCH:
        default:
        {
            rmInitPrms = &gEnetAppRmDefCfg_iccsgSwt;
            break;
        }
    }

    return(rmInitPrms);
}

const EnetRm_IoctlPermissionTable *EnetAppRm_getIoctlPermissionInfo(Enet_Type enetType)
{
    const EnetRm_IoctlPermissionTable *ioctlPerm = NULL;

    switch (enetType)
    {
        case ENET_CPSW_2G:
        {
            ioctlPerm = &gEnetAppIoctlPermission_2G;
            break;
        }

        case ENET_ICSSG_DUALMAC:
        case ENET_ICSSG_SWITCH:
        {
            ioctlPerm = &gEnetAppIoctlPermission_iccsg;
            break;
        }
        default:
        {
            ioctlPerm = NULL;
            break;
        }
    }

    return(ioctlPerm);
}
