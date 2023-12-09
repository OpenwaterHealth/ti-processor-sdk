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
 * \brief    This file contains the Enet driver resource management
 *           configuration used to initialize the RM init parameters passed
 *           during driver init.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/enet/enet.h>
#include <ti/drv/enet/include/per/cpsw.h>
#include <ti/drv/enet/examples/utils/include/enet_appsoc.h>

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

#if 0
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
        [1] =
            /* MCU1_0 */
        {
            .coreId        = 0U,
            .numTxCh       = 1U,    /* numTxCh */
            .numRxFlows    = 1U,    /* numRxFlows */
            .numMacAddress = 1U,    /* numMacAddress */
        },
    },
    .numCores = 1U,
};
#endif

/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

const EnetRm_ResPrms *EnetAppRm_getResourcePartitionInfo(Enet_Type enetType)
{
    const EnetRm_ResPrms *rmInitPrms = NULL;

    return(rmInitPrms);
}

const EnetRm_IoctlPermissionTable *EnetAppRm_getIoctlPermissionInfo(Enet_Type enetType)
{
    const EnetRm_IoctlPermissionTable *ioctlPerm = NULL;

    return(ioctlPerm);
}
