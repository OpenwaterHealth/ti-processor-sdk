/**
 *   Copyright (c) Texas Instruments Incorporated 2018
 *   All rights reserved.
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
 */

/**
 *  \file vhwa_soc.c
 *
 *  \brief File containing the SOC related configuration functions for VHWA.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/csl/soc.h>
#include <ti/drv/vhwa/soc/vhwa_soc.h>
#include <ti/drv/vhwa/include/vhwa_common.h>
#include <vhwa_soc_priv.h>


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
/* Global containing SoC information. */
static Msc_SocInfo gMsc_SocInfo = {
    (CSL_vpac_cntlRegs *)(0x50000000U),
    (CSL_vpac_intd_cfgRegs *)(0x50000000U + 0x4000U),
    (CSL_mscRegs *)(0x50000000U + 0x1C0000U),
    (CSL_lseRegs *)(0x50000000U + 0x1C0800U),
    (CSL_htsRegs *)(0x50000000U + 0x8000U)
};

static Ldc_SocInfo gLdc_SocInfo = {
    (CSL_vpac_cntlRegs *)(0x50000000U),
    (CSL_vpac_intd_cfgRegs *)(0x50000000U + 0x4000U),
    (CSL_ldc_coreRegs *)(0x50000000U + 0x20000U),
    (CSL_lseRegs *)(0x50000000U + 0x20400U),
    (CSL_htsRegs *)(0x50008000U),
    (uint32_t *) (0x50000000U + 0x20000U + 0x800U),
    (uint32_t *) (0x50000000U + 0x20000U + 0x1000U),
};

static Dof_SocInfo gDof_SocInfo = {
    (CSL_dmpacRegs *)(0x40000000u),
    (CSL_dmpac_intd_cfgRegs *)(0x40000000u + 0x1000u),
    (CSL_dmpac_foco_coreRegs *)(0x40000000u + 0x24000U),
    (CSL_dmpac_dofRegs *)(0x40000000u + 0x80000U),
    (CSL_lseRegs *)(0x40000000u + 0x24200U),
    (CSL_htsRegs *)(0x40000000u + 0x8000u)
};

static Viss_SocInfo gViss_SocInfo = {
    (CSL_vpac_cntlRegs *)(0x50000000U),
    (CSL_vpac_intd_cfgRegs *)(0x50000000U + 0x4000U),
    (uint32_t)(0x50000000U + 0x80000U),
    (CSL_htsRegs *)(0x50008000U)
};

static Nf_SocInfo gNf_SocInfo = {
    (CSL_vpac_cntlRegs *)(0x50000000U),
    (CSL_vpac_intd_cfgRegs *)(0x50000000U + 0x4000U),
    (CSL_vpac_nfRegs *)(0x50000000U + 0x1C2000u),
    (CSL_lseRegs *)(0x50000000U + 0x1C3000U),
    (CSL_htsRegs *)(0x50000000U + 0x8000U)
};

static Sde_SocInfo gSde_SocInfo = {
    (CSL_dmpacRegs *)(0x40000000u),
    (CSL_dmpac_intd_cfgRegs *)(0x40000000u + 0x1000u),
    (CSL_dmpac_foco_coreRegs *)(0x40000000u + 0x28000U),
    (CSL_dmpac_sdeRegs *)(0x40000000u + 0x100000U),
    (CSL_lseRegs *)(0x40000000u + 0x28200U),
    (CSL_htsRegs *)(0x40000000u + 0x8000u)
};
#else
/* Global containing SoC information. */
static Msc_SocInfo gMsc_SocInfo = {
    (CSL_vpac_cntlRegs *)CSL_VPAC0_VPAC_REGS_VPAC_REGS_CFG_IP_MMRS_BASE,
    (CSL_vpac_intd_cfgRegs *)CSL_VPAC0_CP_INTD_CFG_INTD_CFG_BASE,
    (CSL_mscRegs *)CSL_VPAC0_PAR_VPAC_MSC_CFG_VP_CFG_VP_BASE,
    (CSL_lseRegs *)CSL_VPAC0_PAR_VPAC_MSC_CFG_VP_LSE_CFG_VP_BASE,
    (CSL_htsRegs *)CSL_VPAC0_HTS_S_VBUSP_BASE
};

static Ldc_SocInfo gLdc_SocInfo = {
    (CSL_vpac_cntlRegs *)CSL_VPAC0_VPAC_REGS_VPAC_REGS_CFG_IP_MMRS_BASE,
    (CSL_vpac_intd_cfgRegs *)CSL_VPAC0_CP_INTD_CFG_INTD_CFG_BASE,
    (CSL_ldc_coreRegs *)CSL_VPAC0_PAR_VPAC_LDC0_S_VBUSP_MMR_VBUSP_BASE,
    (CSL_lseRegs *)CSL_VPAC0_PAR_VPAC_LDC0_S_VBUSP_VPAC_LDC_LSE_CFG_VP_BASE,
    (CSL_htsRegs *)CSL_VPAC0_HTS_S_VBUSP_BASE,
    (uint32_t *)CSL_VPAC0_PAR_VPAC_LDC0_S_VBUSP_PIXWRINTF_DUALY_LUTCFG_DUALY_LUT_BASE,
    (uint32_t *)CSL_VPAC0_PAR_VPAC_LDC0_S_VBUSP_PIXWRINTF_DUALC_LUTCFG_DUALC_LUT_BASE
};

static Dof_SocInfo gDof_SocInfo = {
    (CSL_dmpacRegs *)CSL_DMPAC0_DMPAC_REGS_DMPAC_REGS_CFG_IP_MMRS_BASE,
    (CSL_dmpac_intd_cfgRegs *)CSL_DMPAC0_CP_INTD_CFG_INTD_CFG_BASE,
    (CSL_dmpac_foco_coreRegs *)CSL_DMPAC0_DMPAC_FOCO_0_CFG_SLV_DMPAC_FOCO_CORE_FOCO_REGS_CFG_IP_MMRS_BASE,
    (CSL_dmpac_dofRegs *)CSL_DMPAC0_PAR_DOF_CFG_VP_MMR_VBUSP_DOFCORE_BASE,
    (CSL_lseRegs *)CSL_DMPAC0_DMPAC_FOCO_0_CFG_SLV_VPAC_FOCO_LSE_CFG_VP_BASE,
    (CSL_htsRegs *)CSL_DMPAC0_HTS_S_VBUSP_BASE
};

static Viss_SocInfo gViss_SocInfo = {
    (CSL_vpac_cntlRegs *)CSL_VPAC0_VPAC_REGS_VPAC_REGS_CFG_IP_MMRS_BASE,
    (CSL_vpac_intd_cfgRegs *)CSL_VPAC0_CP_INTD_CFG_INTD_CFG_BASE,
    (uint32_t)CSL_VPAC0_PAR_VPAC_VISS0_S_VBUSP_MMR_CFG_VISS_TOP_BASE,
    (CSL_htsRegs *)CSL_VPAC0_HTS_S_VBUSP_BASE
};

static Nf_SocInfo gNf_SocInfo = {
    (CSL_vpac_cntlRegs *)CSL_VPAC0_VPAC_REGS_VPAC_REGS_CFG_IP_MMRS_BASE,
    (CSL_vpac_intd_cfgRegs *)CSL_VPAC0_CP_INTD_CFG_INTD_CFG_BASE,
    (CSL_vpac_nfRegs *)CSL_VPAC0_PAR_VPAC_NF_S_VBUSP_MMR_VBUSP_NF_CFG_BASE,
    (CSL_lseRegs *)CSL_VPAC0_PAR_VPAC_NF_S_VBUSP_VPAC_NF_LSE_CFG_VP_BASE,
    (CSL_htsRegs *)CSL_VPAC0_HTS_S_VBUSP_BASE
};

static Sde_SocInfo gSde_SocInfo = {
    (CSL_dmpacRegs *)CSL_DMPAC0_DMPAC_REGS_DMPAC_REGS_CFG_IP_MMRS_BASE,
    (CSL_dmpac_intd_cfgRegs *)CSL_DMPAC0_CP_INTD_CFG_INTD_CFG_BASE,
    (CSL_dmpac_foco_coreRegs *)CSL_DMPAC0_DMPAC_FOCO_1_CFG_SLV_DMPAC_FOCO_CORE_FOCO_REGS_CFG_IP_MMRS_BASE,
    (CSL_dmpac_sdeRegs *)CSL_DMPAC0_PAR_PAR_SDE_S_VBUSP_MMR_VBUSP_MMR_BASE,
    (CSL_lseRegs *)CSL_DMPAC0_DMPAC_FOCO_1_CFG_SLV_VPAC_FOCO_LSE_CFG_VP_BASE,
    (CSL_htsRegs *)CSL_DMPAC0_HTS_S_VBUSP_BASE
};
#endif


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void Msc_getSocInfo(Msc_SocInfo *socInfo)
{
    if (NULL != socInfo)
    {
        socInfo->vpacCntlRegs = gMsc_SocInfo.vpacCntlRegs;
        socInfo->vpacIntdRegs = gMsc_SocInfo.vpacIntdRegs;
        socInfo->mscRegs      = gMsc_SocInfo.mscRegs;
        socInfo->lseRegs      = gMsc_SocInfo.lseRegs;
        socInfo->htsRegs      = gMsc_SocInfo.htsRegs;
    }
}

void Msc_getIrqInfo(Vhwa_IrqInfo irqInfo[])
{
    if (NULL != irqInfo)
    {
        irqInfo[0U].irqNum     = VHWA_M2M_MSC0_IRQ_NUM;
        irqInfo[0U].vhwaIrqNum = VHWA_M2M_IRQ_NUM_1;

        irqInfo[1U].irqNum     = VHWA_M2M_MSC1_IRQ_NUM;
        irqInfo[1U].vhwaIrqNum = VHWA_M2M_IRQ_NUM_2;
    }
}

void Ldc_getSocInfo(Ldc_SocInfo *socInfo)
{
    if (NULL != socInfo)
    {
        socInfo->vpacCntlRegs = gLdc_SocInfo.vpacCntlRegs;
        socInfo->vpacIntdRegs = gLdc_SocInfo.vpacIntdRegs;
        socInfo->ldcRegs      = gLdc_SocInfo.ldcRegs;
        socInfo->lseRegs      = gLdc_SocInfo.lseRegs;
        socInfo->htsRegs      = gLdc_SocInfo.htsRegs;
        socInfo->lumaLutBaseAddr = gLdc_SocInfo.lumaLutBaseAddr;
        socInfo->chromaLutBaseAddr = gLdc_SocInfo.chromaLutBaseAddr;
    }
}

void Ldc_getIrqInfo(Vhwa_IrqInfo *irqInfo)
{
    if (NULL != irqInfo)
    {
        irqInfo->irqNum     = VHWA_M2M_LDC_IRQ_NUM;
        irqInfo->vhwaIrqNum = VHWA_M2M_IRQ_NUM_3;
    }
}

void Dof_getSocInfo(Dof_SocInfo *socInfo)
{
    if (NULL != socInfo)
    {
        socInfo->dmpacCntlRegs = gDof_SocInfo.dmpacCntlRegs;
        socInfo->dmpacIntdRegs = gDof_SocInfo.dmpacIntdRegs;
        socInfo->dmpacFocoRegs = gDof_SocInfo.dmpacFocoRegs;
        socInfo->dofRegs      = gDof_SocInfo.dofRegs;
        socInfo->lseRegs      = gDof_SocInfo.lseRegs;
        socInfo->htsRegs      = gDof_SocInfo.htsRegs;
    }
}

void Dof_getIrqInfo(Vhwa_IrqInfo *irqInfo)
{
    if (NULL != irqInfo)
    {
        irqInfo->irqNum     = VHWA_M2M_DOF_IRQ_NUM;
        irqInfo->vhwaIrqNum = VHWA_M2M_DMPAC_IRQ_NUM_1;
    }
}

void Viss_getSocInfo(Viss_SocInfo *socInfo)
{
    if (NULL != socInfo)
    {
        socInfo->vpacCntlRegs = gViss_SocInfo.vpacCntlRegs;
        socInfo->vpacIntdRegs = gViss_SocInfo.vpacIntdRegs;
        socInfo->vissBaseAddr = gViss_SocInfo.vissBaseAddr;
        socInfo->htsRegs      = gViss_SocInfo.htsRegs;
    }
}

void Viss_getIrqInfo(Vhwa_IrqInfo *irqInfo)
{
    if (NULL != irqInfo)
    {
        irqInfo->irqNum     = VHWA_M2M_VISS_IRQ_NUM;
        irqInfo->vhwaIrqNum = VHWA_M2M_IRQ_NUM_0;
    }
}

void Nf_getSocInfo(Nf_SocInfo *socInfo)
{
    if (NULL != socInfo)
    {
        socInfo->vpacCntlRegs = gNf_SocInfo.vpacCntlRegs;
        socInfo->vpacIntdRegs = gNf_SocInfo.vpacIntdRegs;
        socInfo->nfRegs       = gNf_SocInfo.nfRegs;
        socInfo->lseRegs      = gNf_SocInfo.lseRegs;
        socInfo->htsRegs      = gNf_SocInfo.htsRegs;
    }
}

void Nf_getIrqInfo(Vhwa_IrqInfo *irqInfo)
{
    if (NULL != irqInfo)
    {
        irqInfo->irqNum     = VHWA_M2M_NF_IRQ_NUM;
        irqInfo->vhwaIrqNum = VHWA_M2M_IRQ_NUM_4;
    }
}

void Sde_getSocInfo(Sde_SocInfo *socInfo)
{
    if (NULL != socInfo)
    {
        socInfo->dmpacCntlRegs = gSde_SocInfo.dmpacCntlRegs;
        socInfo->dmpacIntdRegs = gSde_SocInfo.dmpacIntdRegs;
        socInfo->dmpacFocoRegs = gSde_SocInfo.dmpacFocoRegs;
        socInfo->sdeRegs      = gSde_SocInfo.sdeRegs;
        socInfo->lseRegs      = gSde_SocInfo.lseRegs;
        socInfo->htsRegs      = gSde_SocInfo.htsRegs;
    }
}

void Sde_getIrqInfo(Vhwa_IrqInfo *irqInfo)
{
    if (NULL != irqInfo)
    {
        irqInfo->irqNum     = VHWA_M2M_SDE_IRQ_NUM;
        irqInfo->vhwaIrqNum = VHWA_M2M_DMPAC_IRQ_NUM_0;
    }
}

void Fc_getSocHtsInfo(CSL_htsRegs **htsRegs)
{
    if (NULL != htsRegs)
    {
        *htsRegs      = (CSL_htsRegs *)CSL_VPAC0_HTS_S_VBUSP_BASE;
    }
}

