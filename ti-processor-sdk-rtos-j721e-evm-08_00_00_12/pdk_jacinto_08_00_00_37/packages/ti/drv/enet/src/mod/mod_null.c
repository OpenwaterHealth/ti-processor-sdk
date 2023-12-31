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
 * \file  mod_null.c
 *
 * \brief This file contains the implementation of the ModXyz.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <ti/drv/enet/include/core/enet_base.h>
#include <ti/drv/enet/include/core/enet_trace.h>
#include <ti/drv/enet/include/mod/mod_null.h>
#include <ti/drv/enet/priv/core/enet_trace_priv.h>

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

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void NullMod_initCfg(NullMod_Cfg *nullModCfg)
{
    ENETTRACE_INFO("Initialize null module config\n");

    /* Initialize other module-level params */
    nullModCfg->dummy = 0xBAADBEEF;
}

int32_t NullMod_open(EnetMod_Handle hMod,
                     Enet_Type enetType,
                     uint32_t instId,
                     const void *cfg)
{
    int32_t status = ENET_SOK;

    ENETTRACE_INFO("%s: Opening null module\n", hMod->name);

    /* Check if dummy feature 1 is enabled */
    if (ENET_FEAT_IS_EN(hMod->features, ENET_NULLMOD_FEAT1))
    {
        ENETTRACE_INFO("%s: Feature 1 is enabled\n", hMod->name);
    }

    /* Check if dummy errata 1 is applicable */
    if (ENET_ERRATA_IS_EN(hMod->errata, ENET_NULLMOD_ERRATA1))
    {
        ENETTRACE_INFO("%s: Errata 1 is applicable\n", hMod->name);
    }

    /* Check if dummy errata 2 is applicable */
    if (ENET_ERRATA_IS_EN(hMod->errata, ENET_NULLMOD_ERRATA2))
    {
        ENETTRACE_INFO("%s: Errata 2 is applicable\n", hMod->name);
    }

    return status;
}

int32_t NullMod_rejoin(EnetMod_Handle hMod,
                       Enet_Type enetType,
                       uint32_t instId)
{
    int32_t status = ENET_SOK;

    ENETTRACE_INFO("%s: Rejoining null module\n", hMod->name);

    return status;
}

int32_t NullMod_ioctl(EnetMod_Handle hMod,
                      uint32_t cmd,
                      Enet_IoctlPrms *prms)
{
    int32_t status = ENET_SOK;

    ENETTRACE_INFO("%s: IOCTL %u on null module\n", hMod->name, cmd);

    return status;
}

void NullMod_close(EnetMod_Handle hMod)
{
    ENETTRACE_INFO("%s: Closing null module\n", hMod->name);
}
