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
 * \file  enet_hostport_cpdma.c
 *
 * \brief This file contains the implementation of the peripheral specific
 *        APIs for CPDMA interface.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <ti/drv/enet/enet.h>
#include <ti/drv/enet/enet_cfg.h>
#include <ti/drv/enet/include/core/enet_base.h>
#include <ti/drv/enet/include/core/enet_utils.h>
#include <ti/drv/enet/include/core/enet_soc.h>
#include <ti/drv/enet/include/core/enet_per.h>
#include <ti/drv/enet/priv/core/enet_base_priv.h>
#include <ti/drv/enet/priv/core/enet_trace_priv.h>
#include <ti/drv/enet/include/common/enet_osal_dflt.h>
#include <ti/drv/enet/include/common/enet_utils_dflt.h>
#include <ti/drv/enet/priv/mod/cpsw_ale_priv.h>
#include <ti/drv/enet/include/per/cpsw.h>
#include <ti/drv/enet/priv/mod/cpsw_ale_priv.h>
#include <ti/drv/enet/priv/mod/cpsw_cpts_priv.h>
#include <ti/drv/enet/priv/mod/cpsw_hostport_priv.h>
#include <ti/drv/enet/priv/mod/cpsw_macport_priv.h>
#if defined(SOC_TPR12) || defined(SOC_AWR294X)
#include <ti/drv/enet/priv/per/cpsw_tpr12_priv.h>
#endif
#include <ti/drv/enet/priv/per/enet_hostport_cpdma.h>
#include <ti/drv/enet/include/core/enet_dma.h>
#include <ti/drv/enet/src/dma/cpdma/enet_cpdma_priv.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
void EnetHostPortDma_initCfg(Enet_Type enetType, const void *dmaCfg)
{
    EnetCpdma_initParams(enetType, (EnetDma_Cfg *)dmaCfg);
}

EnetDma_Handle EnetHostPortDma_open(EnetPer_Handle hPer,
                                    const void *dmaCfg,
                                    const EnetRm_ResCfg *resCfg)
{
    EnetDma_Handle hDma = NULL;

    hDma = EnetDma_open(hPer->enetType, hPer->instId, dmaCfg);
    ENETTRACE_ERR_IF(NULL == hDma, "Failed to open Hostport DMA\n");

    return hDma;
}

void EnetHostPortDma_close(EnetDma_Handle hDma)
{
    int32_t status;

    if (hDma->initFlag)
    {
        /* Close dma module */
        status = EnetDma_close(hDma);
        Enet_assert(status == ENET_SOK);
    }
}

