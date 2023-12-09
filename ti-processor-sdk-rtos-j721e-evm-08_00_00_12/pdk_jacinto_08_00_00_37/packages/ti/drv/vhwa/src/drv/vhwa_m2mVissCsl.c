/**
 *   Copyright (c) Texas Instruments Incorporated 2019
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
 */

/**
 *  \file vhwa_m2mVissApi.c
 *
 *  \brief API Implementation
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/vhwa/src/drv/vhwa_m2mVissPriv.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


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


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t Vhwa_m2mVissSetRfeConfig(Vhwa_M2mVissInstObj *instObj,
		Vhwa_M2mVissHandleObj *hObj, const Rfe_Control *rfeCtrl)
{
    int32_t status = FVID2_SOK;
    Vhwa_M2mVissRegAddr *regAddrs = NULL;
    /* enable buffer object if used */
    bool enableBufObj = false;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != rfeCtrl))
    {
        if (true == instObj->initPrms.configThroughUdmaFlag)
        {
            if (true == hObj->appBuffInitDone)
            {
                /* we will be writing configuration to buffer object */
                regAddrs = &hObj->buffRegAddrs;
                enableBufObj = true;
            }
            else
            {
                status = FVID2_EFAIL;
            }

        }
        else
        {
            regAddrs = &instObj->regAddrs;
        }

        if (FVID2_SOK == status)
        {
            switch (rfeCtrl->module)
            {
                case RFE_MODULE_DPC_LUT:
                {
					status = CSL_rfeSetDpcLutConfig(
						regAddrs->rfeRegs, regAddrs->dpcLutAddr,
						rfeCtrl->dpcLutCfg);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified =
                                true;
                        hObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].isModified
                                = true;
                    }
                    break;
                }

                case RFE_MODULE_DPC_OTF:
                {
					status = CSL_rfeSetDpcOtfConfig(
						regAddrs->rfeRegs, rfeCtrl->dpcOtfCfg);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified
                                = true;
                    }
                    break;
                }

                case RFE_MODULE_GAIN_OFST:
                {
					status = CSL_rfeSetWbConfig(
						regAddrs->rfeRegs, rfeCtrl->wbConfig);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified
                                = true;
                    }
                    break;
                }

                case RFE_MODULE_LSC:
                {
					status = CSL_rfeSetLscConfig(
						regAddrs->rfeRegs, regAddrs->lscLutAddr,
						rfeCtrl->lscConfig);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified =
                                true;
                        hObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].isModified
                                = true;
                    }
                    break;
                }

                case RFE_MODULE_WDR_MERGE_MA1:
                {
					status = CSL_rfeSetWdrConfig(
						regAddrs->rfeRegs, RFE_MODULE_WDR_MERGE_MA1,
						rfeCtrl->wdrMergeMa1);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified
                                = true;
                    }
                    break;
                }

                case RFE_MODULE_WDR_MERGE_MA2:
                {
					status = CSL_rfeSetWdrConfig(
						regAddrs->rfeRegs, RFE_MODULE_WDR_MERGE_MA2,
						rfeCtrl->wdrMergeMa2);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified
                                = true;
                    }
                    break;
                }

                case RFE_MODULE_PWL1:
                {
					status = CSL_rfeSetPwlConfig(
						regAddrs->rfeRegs, RFE_MODULE_PWL1,
						rfeCtrl->pwl1Cfg);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified
                                = true;
                    }
                    break;
                }

                case RFE_MODULE_PWL2:
                {
					status = CSL_rfeSetPwlConfig(
						regAddrs->rfeRegs, RFE_MODULE_PWL2,
						rfeCtrl->pwl2Cfg);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified
                                = true;
                    }
                    break;
                }

                case RFE_MODULE_PWL3:
                {
					status = CSL_rfeSetPwlConfig(
						regAddrs->rfeRegs, RFE_MODULE_PWL3,
						rfeCtrl->pwl3Cfg);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified
                                = true;
                    }
                    break;
                }

                case RFE_MODULE_DECOMP_LUT1:
                {
					status = CSL_rfeSetLutConfig(
						regAddrs->rfeRegs, regAddrs->decompLut1Addr,
						RFE_MODULE_DECOMP_LUT1, rfeCtrl->decomp1Cfg);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified =
                                true;
                        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].isModified
                                = true;
                    }
                    break;
                }

                case RFE_MODULE_DECOMP_LUT2:
                {
					status = CSL_rfeSetLutConfig(
						regAddrs->rfeRegs, regAddrs->decompLut2Addr,
                    RFE_MODULE_DECOMP_LUT2, rfeCtrl->decomp2Cfg);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified =
                                true;
                        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].isModified
                                 = true;
                    }
                    break;
                }

                case RFE_MODULE_DECOMP_LUT3:
                {
					status = CSL_rfeSetLutConfig(
						regAddrs->rfeRegs, regAddrs->decompLut3Addr,
						RFE_MODULE_DECOMP_LUT3, rfeCtrl->decomp3Cfg);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified =
                                true;
                        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].isModified
                                = true;
                    }
                    break;
                }

                case RFE_MODULE_COMP_LUT:
                {
					status = CSL_rfeSetLutConfig(
						regAddrs->rfeRegs, regAddrs->compLutAddr,
                        RFE_MODULE_COMP_LUT, rfeCtrl->compCfg);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified =
                                true;
                        hObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].isModified
                                = true;
                    }
                    break;
                }

                case RFE_MODULE_H3A:
                {
					status = CSL_rfeSetH3aConfig(
						regAddrs->rfeRegs, rfeCtrl->h3aInCfg);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified =
                                true;
                    }
                    break;
                }

                case RFE_MODULE_H3A_LUT:
                {
					status = CSL_rfeSetH3aLutConfig(
						regAddrs->rfeRegs, regAddrs->h3aLutAddr,
							rfeCtrl->h3aLutCfg);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified =
                                true;
                        hObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].isModified
                                = true;
                    }
                    break;
                }
                default:
                {
                    /* Nothing to do here */
                    break;
                }
            }
        }
    }
    else
    {
        status = FVID2_EBADARGS;
    }

    return (status);
}

int32_t Vhwa_m2mVissGetGlbceConfig(Vhwa_M2mVissInstObj *instObj,
    const Vhwa_M2mVissHandleObj *hObj, Glbce_Control *ctrl)
{
    int32_t                 status = FVID2_EBADARGS;
    Vhwa_M2mVissRegAddr    *regAddrs;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != ctrl))
    {
        regAddrs = &instObj->regAddrs;

        switch (ctrl->module)
        {
            case GLBCE_MODULE_GET_STATS_INFO:
            {
                status = CSL_glbceGetStatsInfo(regAddrs->glbceStatsMem,
                    ctrl->statsInfo);
                break;
            }
            default:
            {
                /* Nothing to do here */
                break;
            }
        }
    }

    return (status);
}

int32_t Vhwa_m2mVissSetGlbceConfig(Vhwa_M2mVissInstObj *instObj,
		Vhwa_M2mVissHandleObj *hObj, const Glbce_Control *ctrl)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mVissRegAddr    *regAddrs;
    /* enable buffer object if used */
    bool enableBufObj = false;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != ctrl))
    {
        if (true == instObj->initPrms.configThroughUdmaFlag)
        {
            if (true == hObj->appBuffInitDone)
            {
                /* we will be writing configuration to buffer object */
                regAddrs = &hObj->buffRegAddrs;
                enableBufObj = true;
            }
            else
            {
                status = FVID2_EFAIL;
            }

        }
        else
        {
            regAddrs = &instObj->regAddrs;
        }

        if (FVID2_SOK == status)
        {
            switch (ctrl->module)
            {
                case GLBCE_MODULE_GLBCE:
                {
					status = CSL_glbceSetToneMapConfig(regAddrs->glbceRegs,
						ctrl->glbceCfg);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].isModified =
                                true;
                    }
                    break;
                }

                case GLBCE_MODULE_FWD_PERCEPT:
                {
					status = CSL_glbceSetPerceptConfig(regAddrs->glbceRegs,
						ctrl->fwdPrcptCfg, GLBCE_MODULE_FWD_PERCEPT);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].isModified =
                                true;
                    }
                    break;
                }

                case GLBCE_MODULE_REV_PERCEPT:
                {
					status = CSL_glbceSetPerceptConfig(regAddrs->glbceRegs,
						ctrl->revPrcptCfg, GLBCE_MODULE_REV_PERCEPT);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].isModified =
                                true;
                    }
                    break;
                }

                case GLBCE_MODULE_WDR:
                {
					status = CSL_glbceSetWdrConfig(regAddrs->glbceRegs,
						ctrl->wdrCfg);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].isModified =
                                true;
                    }
                    break;
                }
                default:
                {
                    /* Nothing to do here */
                    break;
                }
            }
        }
    }
    else
    {
        status = FVID2_EBADARGS;
    }

    return (status);
}

int32_t Vhwa_m2mVissSetNsf4Config(Vhwa_M2mVissInstObj *instObj,
    	Vhwa_M2mVissHandleObj *hObj, const Nsf4v_Config *nsf4Cfg)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mVissRegAddr    *regAddrs;
    /* enable buffer object if used */
    bool enableBufObj = false;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != nsf4Cfg))
    {
        if (true == instObj->initPrms.configThroughUdmaFlag)
        {
            if (true == hObj->appBuffInitDone)
            {
                /* we will be writing configuration to buffer object */
                regAddrs = &hObj->buffRegAddrs;
                enableBufObj = true;
            }
            else
            {
                status = FVID2_EFAIL;
            }

        }
        else
        {
            regAddrs = &instObj->regAddrs;
        }

        if (FVID2_SOK == status)
        {
            status = CSL_nsf4vSetConfig(regAddrs->nsf4Regs, nsf4Cfg);
            /* Enable used buffer object */
            if ((FVID2_SOK == status) && (true == enableBufObj))
            {
                hObj->bufferObjHolder[BUFF_ID_NSF4_REGS].isModified =
                        true;
            }
        }
    }
    else
    {
        status = FVID2_EBADARGS;
    }

    return (status);
}

int32_t Vhwa_m2mVissSetH3aConfig(Vhwa_M2mVissInstObj *instObj,
    	Vhwa_M2mVissHandleObj *hObj, const H3a_Config *h3aCfg)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mVissRegAddr    *regAddrs;
    /* enable buffer object if used */
    bool enableBufObj = false;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != h3aCfg))
    {
        if (true == instObj->initPrms.configThroughUdmaFlag)
        {
            if (true == hObj->appBuffInitDone)
            {
                /* we will be writing configuration to buffer object */
                regAddrs = &hObj->buffRegAddrs;
                enableBufObj = true;
            }
            else
            {
                status = FVID2_EFAIL;
            }

        }
        else
        {
            regAddrs = &instObj->regAddrs;
        }

        if (FVID2_SOK == status)
        {
            status = CSL_h3aSetConfig(regAddrs->h3aRegs, h3aCfg);
            if ((FVID2_SOK == status) && (true == enableBufObj))
            {
                hObj->bufferObjHolder[BUFF_ID_H3A_REGS].isModified = true;
            }
        }
    }
    else
    {
        status = FVID2_EBADARGS;
    }

    return (status);
}

int32_t Vhwa_m2mVissSetFcpConfig(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj, const Fcp_Control *ctrl)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mVissRegAddr    *regAddrs;
    /* enable buffer object if used */
    bool enableBufObj = false;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != ctrl))
    {
        if (true == instObj->initPrms.configThroughUdmaFlag)
        {
            if (true == hObj->appBuffInitDone)
            {
                /* we will be writing configuration to buffer object */
                regAddrs = &hObj->buffRegAddrs;
                enableBufObj = true;
            }
            else
            {
                status = FVID2_EFAIL;
            }

        }
        else
        {
            regAddrs = &instObj->regAddrs;
        }

        if (FVID2_SOK == status)
        {
            switch (ctrl->module)
            {
                case FCP_MODULE_COMPANDING:
                {
                    status = CSL_fcpSetCompConfig(regAddrs->cfaRegs,
                            ctrl->inComp);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_CFA_REGS].isModified =
                                true;
                    }
                    break;
                }

                case FCP_MODULE_CFA:
                {
					status = CSL_fcpSetCfaConfig(regAddrs->cfaRegs,
						ctrl->cfa);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_CFA_REGS].isModified =
                                true;
                    }
                    break;
                }

                case FCP_MODULE_CCM:
                {
					status = CSL_fcpSetCcmConfig(regAddrs->fcpRegs,
						ctrl->ccm);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_FCP_REGS].isModified =
                                true;
                    }
                    break;
                }

                case FCP_MODULE_GAMMA:
                {
					status = CLS_fcpSetGammaConfig(regAddrs->fcpRegs,
						regAddrs->gammaLut1Addr, regAddrs->gammaLut2Addr,
						regAddrs->gammaLut3Addr, ctrl->gamma);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_FCP_REGS].isModified =
                                true;
                        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].isModified
                                = true;
                        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].isModified
                                = true;
                        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].isModified
                                = true;
                    }
                    break;
                }

                case FCP_MODULE_RGB2HSV:
                {
					status = CSL_fcpSetRgb2HsvConfig(regAddrs->fcpRegs,
						ctrl->rgb2Hsv);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_FCP_REGS].isModified =
                                true;
                    }
                    break;
                }

                case FCP_MODULE_RGB2YUV:
                {
					status = CLS_fcpSetRgb2YuvConfig(regAddrs->fcpRegs,
						ctrl->rgb2Yuv);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_FCP_REGS].isModified =
                                true;
                    }
                    break;
                }

                #if 0
                case FCP_MODULE_OUT_SELECT:
                {
                    status = CSL_fcpSetOutputMux(regAddrs->fcpRegs,
                        ctrl->outSelect);
                    if (FVID2_SOK == status)
                    {
                        memcpy(&hObj->fcpOutCfg,
                            ctrl->outSelect, sizeof(Fcp_OutputSelect));
                    }
                    break;
                }
                #endif

                case FCP_MODULE_HISTOGRAM:
                {
					status = CSL_fcpSetHistogramConfig(regAddrs->fcpRegs,
						ctrl->hist);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_FCP_REGS].isModified =
                                true;
                    }
                    break;
                }

                case FCP_MODULE_RGB_LUT:
                {
					status = CSL_fcpSetRgbLutConfig(regAddrs->fcpRegs,
						regAddrs->y8C8LutAddr, regAddrs->c8G8LutAddr,
						regAddrs->s8B8LutAddr, ctrl->rgbLut);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_FCP_REGS].isModified =
                                true;
                        hObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].isModified
                                = true;
                        hObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].isModified
                                = true;
                        hObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].isModified
                                = true;
                    }
                    break;
                }

                case FCP_MODULE_YUV_SAT_LUT:
                {
					status = CSL_fcpSetYuvSatLutConfig(regAddrs->fcpRegs,
						regAddrs->y8C8LutAddr, regAddrs->c8G8LutAddr,
						regAddrs->s8B8LutAddr, ctrl->yuvSatLut);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_FCP_REGS].isModified =
                                true;
                        hObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].isModified
                                = true;
                        hObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].isModified
                                = true;
                        hObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].isModified
                                = true;
                    }
                    break;
                }

                case FCP_MODULE_EE:
                {
					if ((VHWA_M2M_VISS_EE_DISABLE <
							hObj->vsPrms.edgeEnhancerMode) &&
						(VHWA_M2M_VISS_EE_MAX >
							hObj->vsPrms.edgeEnhancerMode))
                    {
                        status = CSL_eeSetConfig(regAddrs->eeRegs, ctrl->eeCfg);
                        /* Enable used buffer object */
                        if ((FVID2_SOK == status) && (true == enableBufObj))
                        {
                            hObj->bufferObjHolder[BUFF_ID_EE_REGS].isModified =
                                    true;
                        }

                        if (FVID2_SOK == status)
                        {
                            hObj->eeMux = CSL_eeGetMuxValue(regAddrs->eeRegs);
                        }
                    }
                    else
                    {
                        status = FVID2_EINVALID_PARAMS;
                        GT_0trace(VhwaVissTrace, GT_ERR,
                                "Edge Enhancer is disabled in SET_PARAMS!!\n");
                    }
                    break;
                }
                default:
                {
                    /* Nothing to do here */
                    break;
                }
            }
        }
    }
    else
    {
        status = FVID2_EBADARGS;
    }

    return (status);
}

int32_t Vhwa_m2mVissReadHistogram(Vhwa_M2mVissInstObj *instObj,
    const Vhwa_M2mVissHandleObj *hObj, Fcp_HistData *histData)
{

    int32_t                 status = FVID2_EBADARGS;
    Vhwa_M2mVissRegAddr    *regAddrs;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != histData))
    {
        regAddrs = &instObj->regAddrs;

        status = CSL_fcpReadHistogram(regAddrs->histRegs, histData->hist);
    }

    return (status);
}

void Vhwa_m2mVissSetHtsCfg(const Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj, const Vhwa_M2mVissParams *vsPrms)
{
    uint32_t                 cnt;
    CSL_HtsSchConfig        *htsCfg = NULL;
    Vhwa_M2mVissChParams    *chPrms = NULL;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != vsPrms))
    {
        htsCfg = &hObj->htsCfg;

        CSL_htsSchConfigInit(htsCfg);

        htsCfg->schId = CSL_HTS_HWA_SCH_VISS0;

        htsCfg->pipeline      = instObj->pipeline;
        htsCfg->enableStream  = (uint32_t)FALSE;
        htsCfg->enableHop     = (uint32_t)FALSE;
        htsCfg->enableWdTimer = (uint32_t)FALSE;

        htsCfg->enableBwLimit = FALSE;
        htsCfg->cycleCnt      = 0U;
        htsCfg->tokenCnt      = 0U;

        for (cnt = 0u; cnt < VHWA_M2M_VISS_MAX_INPUTS; cnt ++)
        {
            chPrms = &hObj->inChPrms[cnt];

            if ((uint32_t)TRUE == chPrms->isEnabled)
            {
                htsCfg->consCfg[cnt].enable         = (uint32_t)TRUE;
                /* Input Producer is fixed to UDMA */
                htsCfg->consCfg[cnt].prodId         = CSL_HTS_PROD_IDX_UDMA;

                htsCfg->dmaProdCfg[cnt].enable      = (uint32_t)TRUE;
                htsCfg->dmaProdCfg[cnt].dmaChNum    = instObj->inUtcCh[cnt];
                htsCfg->dmaProdCfg[cnt].pipeline    = instObj->pipeline;
                htsCfg->dmaProdCfg[cnt].consId      = CSL_HTS_CONS_IDX_UDMA;
                /* Default mapping for DMA */

#ifndef VHWA_VISS_M2M_ENABLE_LINE_MODE
                htsCfg->dmaProdCfg[cnt].threshold   = hObj->thr + 1u;
#else
                htsCfg->dmaProdCfg[cnt].threshold   = 1U;
#endif

#ifdef VHWA_VISS_M2M_LSE_LPBK_ENABLE
                htsCfg->dmaProdCfg[cnt].threshold   = 1U;
#endif

                htsCfg->dmaProdCfg[cnt].countDec    = 1U;
                htsCfg->dmaProdCfg[cnt].depth       = chPrms->sl2Depth;
                htsCfg->dmaProdCfg[cnt].cntPreLoad  = 0U;

#ifndef VHWA_VISS_M2M_ENABLE_LINE_MODE
                htsCfg->dmaProdCfg[cnt].cntPostLoad =
                    hObj->maxLseInitSkip + hObj->thr;
#else
                htsCfg->dmaProdCfg[cnt].cntPostLoad =
                    hObj->maxLseInitSkip;
#endif

#ifdef VHWA_VISS_M2M_LSE_LPBK_ENABLE
                htsCfg->dmaProdCfg[cnt].cntPostLoad = 0U;
#endif
                htsCfg->dmaProdCfg[cnt].enableHop = (uint32_t)TRUE;
                htsCfg->dmaProdCfg[cnt].numHop = chPrms->height;
            }
            else
            {
                htsCfg->consCfg[cnt].enable = (uint32_t)FALSE;
                htsCfg->dmaProdCfg[cnt].enable = (uint32_t)FALSE;
            }
        }

        for (cnt = 0u; cnt < VHWA_M2M_VISS_MAX_OUTPUTS; cnt ++)
        {
            chPrms = &hObj->outChPrms[cnt];

            if ((uint32_t)TRUE == chPrms->isEnabled)
            {
                htsCfg->prodCfg[cnt].enable         = (uint32_t)TRUE;
                htsCfg->prodCfg[cnt].consId         = CSL_HTS_CONS_IDX_UDMA;

                /* TODO: Change this based on number of filters */
                htsCfg->prodCfg[cnt].threshold      = 1u;
                htsCfg->prodCfg[cnt].countDec       = 1U;
                htsCfg->prodCfg[cnt].depth          = chPrms->sl2Depth;
                htsCfg->prodCfg[cnt].cntPreLoad     = 0u;
                htsCfg->prodCfg[cnt].cntPostLoad    = 0u;

                htsCfg->dmaConsCfg[cnt].enable      = (uint32_t)TRUE;
                htsCfg->dmaConsCfg[cnt].dmaChNum    = instObj->outUtcCh[cnt];
                htsCfg->dmaConsCfg[cnt].pipeline    = instObj->pipeline;
                htsCfg->dmaConsCfg[cnt].enableStream = (uint32_t)FALSE;
                htsCfg->dmaConsCfg[cnt].prodId      = CSL_HTS_PROD_IDX_UDMA;
                    /* Default mapping for DMA */
            }
            else
            {
                htsCfg->prodCfg[cnt].enable         = (uint32_t)FALSE;
                htsCfg->dmaConsCfg[cnt].enable      = (uint32_t)FALSE;
            }
        }

        htsCfg->dmaConsCfg[6].bypass      = (uint32_t)TRUE;
    }
}

void Vhwa_m2mVissSetLseCfg(const Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj, const Vhwa_M2mVissParams *vsPrms)
{
    uint32_t                cnt;
    CSL_LseConfig          *lseCfg = NULL;
    Vhwa_M2mVissChParams   *chPrms = NULL;
    Vhwa_M2mVissCreateArgs *createArgs = NULL;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != vsPrms))
    {
        lseCfg = &hObj->lseCfg;
        createArgs = &hObj->createArgs;

        /* Initialize LSE configuration with the default values */
        CSL_lseConfigInit(lseCfg);

        /* LSE number of input channels is 1 for VISS, only number of
         * buffers are changing based on the merge mode */
        chPrms = &hObj->inChPrms[0U];
        lseCfg->numInCh = 1U;
        lseCfg->numOutCh = VHWA_M2M_VISS_MAX_OUT_DMA_CH - 1u;

        lseCfg->inChCfg[0U].enable            = (uint32_t)TRUE;
        lseCfg->inChCfg[0U].frameWidth        = chPrms->width;
        lseCfg->inChCfg[0U].frameHeight       = chPrms->height;
        /* No support for Skipping */
        lseCfg->inChCfg[0U].enableAddrIncrBy2 = (uint32_t)FALSE;
        lseCfg->enablePsa                     = createArgs->enablePsa;

        /* Frame Mode for VISS */
#ifndef VHWA_VISS_M2M_ENABLE_LINE_MODE
        lseCfg->enableInChSyncOnFrame         = (uint32_t)TRUE;
#else
        lseCfg->enableInChSyncOnFrame         = (uint32_t)FALSE;
#endif

        if (TRUE == lseCfg->enableInChSyncOnFrame)
        {
            lseCfg->inChCfg[0U].vpHBlankCnt   = hObj->maxLseHorzBlanking;
        }
        else
        {
            lseCfg->inChCfg[0U].vpHBlankCnt   = 220u;
        }

        lseCfg->inChCfg[0U].ccsf              = chPrms->ccsf;
        lseCfg->inChCfg[0U].startOffset       = 0U;
        lseCfg->inChCfg[0U].lineOffset        = chPrms->sl2Pitch;
        lseCfg->inChCfg[0U].circBufSize       = chPrms->sl2Depth;

#ifdef VHWA_VISS_M2M_LSE_LPBK_ENABLE
        lseCfg->inChCfg[0U].vpHBlankCnt = 0U;
        lseCfg->inChCfg[0U].ccsf              = FVID2_CCSF_BITS16_PACKED;
#endif
        /* Assumes No padding required for VISS */
        lseCfg->inChCfg[0U].knTopPadding      = 0U;
        lseCfg->inChCfg[0U].knBottomPadding   = 0U;
        lseCfg->inChCfg[0U].knLineOffset      = 0U;
        lseCfg->inChCfg[0U].knHeight          = 1U;

        lseCfg->inChCfg[0U].numInBuff = 0u;
        for (cnt = 0u; cnt < VHWA_M2M_VISS_MAX_INPUTS; cnt ++)
        {
            chPrms = &hObj->inChPrms[cnt];
            if ((uint32_t)TRUE == chPrms->isEnabled)
            {
                lseCfg->inChCfg[0U].bufAddr[cnt] = (uint32_t)chPrms->sl2Addr;
                lseCfg->inChCfg[0U].numInBuff ++;
            }
        }

        hObj->maxLseInitSkip = 0u;
        hObj->minLseInitSkip = 0xFFFFFFFFu;
        for (cnt = 0u; cnt < (VHWA_M2M_VISS_MAX_OUT_DMA_CH - 1u); cnt ++)
        {
            chPrms = &hObj->outChPrms[cnt];
            if ((uint32_t)TRUE == chPrms->isEnabled)
            {
                lseCfg->outChCfg[cnt].enable = (uint32_t)TRUE;
                lseCfg->outChCfg[cnt].ccsf =
                    (Fvid2_ColorCompStorageFmt)chPrms->ccsf;
                lseCfg->outChCfg[cnt].lineOffset = chPrms->sl2Pitch;
                lseCfg->outChCfg[cnt].circBufSize = chPrms->sl2Depth;
                lseCfg->outChCfg[cnt].bufAddr = (uint32_t)chPrms->sl2Addr;
                lseCfg->outChCfg[cnt].numInitOutSkip =
                        hObj->maxLseVertBlanking + chPrms->extraBlanking;

                if ((FVID2_DF_YUV422I_UYVY ==
                        (Fvid2_DataFormat)chPrms->dataFmt) ||
                    (FVID2_DF_YUV422I_YUYV ==
                    (Fvid2_DataFormat)chPrms->dataFmt))
                {
                    lseCfg->outChCfg[cnt].enableYuv422Out = (uint32_t)TRUE;
                    lseCfg->outChCfg[cnt].yuv422DataFmt =
                        (Fvid2_DataFormat)chPrms->dataFmt;
                }
                else
                {
                    lseCfg->outChCfg[cnt].enableYuv422Out = (uint32_t)FALSE;
                }

#ifdef VHWA_VISS_M2M_LSE_LPBK_ENABLE
                lseCfg->outChCfg[cnt].numInitOutSkip = 0U;
                lseCfg->outChCfg[cnt].ccsf           = FVID2_CCSF_BITS16_PACKED;
#endif
                if (TRUE == chPrms->isLumaCh)
                {
                    if (lseCfg->outChCfg[cnt].numInitOutSkip >
                            hObj->maxLseInitSkip)
                    {
                        hObj->maxLseInitSkip =
                            lseCfg->outChCfg[cnt].numInitOutSkip;
                    }
                    if (lseCfg->outChCfg[cnt].numInitOutSkip <
                            hObj->minLseInitSkip)
                    {
                        hObj->minLseInitSkip =
                            lseCfg->outChCfg[cnt].numInitOutSkip;
                    }
                }

            }
            else
            {
                lseCfg->outChCfg[cnt].enable = (uint32_t)FALSE;
            }
        }

        /* Enable H3A Channel */
        chPrms = &hObj->outChPrms[VHWA_M2M_VISS_OUT_H3A_IDX];
        if ((uint32_t)TRUE == chPrms->isEnabled)
        {
            lseCfg->h3aCfg.enable       = (uint32_t)TRUE;
            lseCfg->h3aCfg.circBufSize  = chPrms->sl2Depth;
            lseCfg->h3aCfg.lineOffset   = chPrms->sl2Pitch;
            lseCfg->h3aCfg.bufAddr      = (uint32_t)chPrms->sl2Addr;
        }
        else
        {
            lseCfg->h3aCfg.enable       = (uint32_t)FALSE;
        }

#ifdef VHWA_VISS_M2M_LSE_LPBK_ENABLE
        lseCfg->lpbkCfg.enable = TRUE;
        lseCfg->lpbkCfg.coreEnable = FALSE;
        lseCfg->lpbkCfg.inCh = 0u;
#endif
    }
}

int32_t Vhwa_memVissSetVissSizeAndPath(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mVissRegAddr    *regAddrs = NULL;
    CSL_GlbceConfig         glbceCfg;
    Vhwa_M2mVissParams     *vsPrms = NULL;
    /* enable buffer object if used */
    bool enableBufObj = false;

    if ((NULL != instObj) && (NULL != hObj))
    {
        if (true == instObj->initPrms.configThroughUdmaFlag)
        {
            if (true == hObj->appBuffInitDone)
            {
                /* we will be writing configuration to buffer object */
                regAddrs = &hObj->buffRegAddrs;
                enableBufObj = true;
            }
            else
            {
                status = FVID2_EFAIL;
            }

        }
        else
        {
            regAddrs = &instObj->regAddrs;
        }

        vsPrms = &hObj->vsPrms;

        if (FVID2_SOK == status)
        {
            /* Set the input frame size in FCP */
			status = CSL_fcpSetFrameConfig(regAddrs->cfaRegs, regAddrs->fcpRegs,
				&hObj->fcpCfg);
            if (FVID2_SOK != status)
            {
                GT_0trace(VhwaVissTrace, GT_ERR,
                        "CSL_fcpSetConfig Failed!!\n");
            }
            /* Enable used buffer object */
            if ((FVID2_SOK == status) && (true == enableBufObj))
            {
                hObj->bufferObjHolder[BUFF_ID_CFA_REGS].isModified = true;
                hObj->bufferObjHolder[BUFF_ID_FCP_REGS].isModified = true;
            }
        }

        if (FVID2_SOK == status)
        {
            status = CSL_fcpSetOutputMux(regAddrs->fcpRegs,
                &hObj->fcpOutCfg);
            if (FVID2_SOK != status)
            {
                GT_0trace(VhwaVissTrace, GT_ERR,
                        "CSL_fcpSetOutputMux Failed!!\n");
            }
            /* Enable used buffer object */
            if ((FVID2_SOK == status) && (true == enableBufObj))
            {
                hObj->bufferObjHolder[BUFF_ID_FCP_REGS].isModified = true;
            }
        }
        if (FVID2_SOK == status)
        {
            /* Set the input frame size in RFE */
            status = CSL_rfeSetFrameConfig(regAddrs->rfeRegs, &hObj->rfeCfg);
            if (FVID2_SOK != status)
            {
                GT_0trace(VhwaVissTrace, GT_ERR,
                        "CSL_rfeSetConfig Failed!!\n");
            }
            /* Enable used buffer object */
            if ((FVID2_SOK == status) && (true == enableBufObj))
            {
                hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified = true;
            }
        }
        if (FVID2_SOK == status)
        {
            /* Set the input frame size in RFE */
            status = CSL_eeSetFrameConfig(regAddrs->eeRegs, &hObj->eeCfg);
            if (FVID2_SOK != status)
            {
                GT_0trace(VhwaVissTrace, GT_ERR,
                        "CSL_eeSetFrameConfig Failed!!\n");
            }
            /* Enable used buffer object */
            if ((FVID2_SOK == status) && (true == enableBufObj))
            {
                hObj->bufferObjHolder[BUFF_ID_EE_REGS].isModified = true;
            }
        }
        if (FVID2_SOK == status)
        {
            if ((uint32_t)TRUE == vsPrms->enableGlbce)
            {
                /* First enable GLBCE */
                CSL_vissTopGlbceEnable(instObj->regAddrs.topRegs, TRUE);

                glbceCfg.width = hObj->fcpCfg.width;
                glbceCfg.height = hObj->fcpCfg.height;
                glbceCfg.isOneShotMode = TRUE;
                status = CSL_glbceSetConfig(regAddrs->glbceRegs, &glbceCfg);
                if (FVID2_SOK == status)
                {
                    status = CSL_glbceStart(regAddrs->glbceRegs);
                    /* Enable used buffer object */
                    if ((FVID2_SOK == status) && (true == enableBufObj))
                    {
                        hObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].isModified = true;
                    }
                    if (FVID2_SOK != status)
                    {
                        GT_0trace(VhwaVissTrace, GT_ERR,
                                "CSL_glbceStart Failed!!\n");
                    }
                }
                else
                {
                    GT_0trace(VhwaVissTrace, GT_ERR,
                            "CSL_glbceSetConfig Failed!!\n");
                }
            }
            else
            {
                #if 0
                glbceCfg.width = hObj->fcpCfg.width;
                glbceCfg.height = hObj->fcpCfg.height;
                glbceCfg.isOneShotMode = TRUE;
                status = CSL_glbceSetConfig(regAddrs->glbceRegs, &glbceCfg);
                if (FVID2_SOK == status)
                {
                    status = CSL_glbceStop(regAddrs->glbceRegs);
                }
                else
                {
                    status = FVID2_EFAIL;
                }
                #else
                CSL_vissTopGlbceEnable(instObj->regAddrs.topRegs, FALSE);
                #endif
            }
        }

        if (FVID2_SOK == status)
        {
            if ((uint32_t)TRUE == vsPrms->enableNsf4)
            {
                CSL_vissTopNsf4Enable(instObj->regAddrs.topRegs, TRUE);

                status = CSL_nsf4vSetFrameConfig(regAddrs->nsf4Regs,
                    &hObj->nsf4vCfg);
                /* Enable used buffer object */
                if ((FVID2_SOK == status) && (true == enableBufObj))
                {
                    hObj->bufferObjHolder[BUFF_ID_NSF4_REGS].isModified = true;
                }
            }
            else
            {
                CSL_vissTopNsf4Enable(instObj->regAddrs.topRegs, FALSE);
            }
        }

        if (FVID2_SOK == status)
        {
            /* Enable AF/AEWB only if H3A output is enabled */
            if ((uint32_t)TRUE ==
                    vsPrms->outPrms[VHWA_M2M_VISS_OUT_H3A_IDX].enable)
            {
                /* Parameters must be set to start H3A */
                if (H3A_MODULE_MIN == hObj->h3aModule)
                {
                    status = FVID2_EINVALID_PARAMS
                    GT_0trace(VhwaVissTrace, GT_ERR,
                            "H3a Params not set!!\n");
                }

                if (H3A_MODULE_AF == hObj->h3aModule)
                {
                    CSL_h3aStart(regAddrs->h3aRegs, H3A_MODULE_AF);
                    /* Enable used buffer object */
                    if (true == enableBufObj)
                    {
                        hObj->bufferObjHolder[BUFF_ID_H3A_REGS].isModified =
                                true;
                    }
                }
                else
                {
                    CSL_h3aStop(regAddrs->h3aRegs, H3A_MODULE_AF);
                    /* Enable used buffer object */
                    if (true == enableBufObj)
                    {
                        hObj->bufferObjHolder[BUFF_ID_H3A_REGS].isModified =
                                true;
                    }
                }
                if (H3A_MODULE_AEWB == hObj->h3aModule)
                {
                    CSL_h3aStart(regAddrs->h3aRegs, H3A_MODULE_AEWB);
                    /* Enable used buffer object */
                    if (true == enableBufObj)
                    {
                        hObj->bufferObjHolder[BUFF_ID_H3A_REGS].isModified =
                                true;
                    }
                }
                else
                {
                    CSL_h3aStop(regAddrs->h3aRegs, H3A_MODULE_AEWB);
                    /* Enable used buffer object */
                    if (true == enableBufObj)
                    {
                        hObj->bufferObjHolder[BUFF_ID_H3A_REGS].isModified =
                                true;
                    }
                }
            }
            else
            {
                CSL_h3aStop(regAddrs->h3aRegs, H3A_MODULE_AF);
                CSL_h3aStop(regAddrs->h3aRegs, H3A_MODULE_AEWB);
                /* Enable used buffer object */
                if (true == enableBufObj)
                {
                    hObj->bufferObjHolder[BUFF_ID_H3A_REGS].isModified =
                            true;
                }
            }
        }
        if (FVID2_SOK == status)
        {
            if ((VHWA_M2M_VISS_EE_DISABLE < vsPrms->edgeEnhancerMode) &&
                (VHWA_M2M_VISS_EE_MAX > vsPrms->edgeEnhancerMode))
            {
                CSL_eeStart(regAddrs->eeRegs, hObj->eeMux);
                /* Enable used buffer object */
                if (true == enableBufObj)
                {
                    hObj->bufferObjHolder[BUFF_ID_EE_REGS].isModified =
                            true;
                }
            }
            else
            {
                CSL_eeStop(regAddrs->eeRegs, hObj->eeMux);
                /* Enable used buffer object */
                if (true == enableBufObj)
                {
                    hObj->bufferObjHolder[BUFF_ID_EE_REGS].isModified =
                            true;
                }
            }
        }
    }
    else
    {
        status = FVID2_EBADARGS;
    }

    return (status);
}

int32_t Vhwa_memVissSetVissStartModules(Vhwa_M2mVissInstObj *instObj,
    		Vhwa_M2mVissHandleObj *hObj)
{
    int32_t                 status = FVID2_EBADARGS;
    Vhwa_M2mVissRegAddr    *regAddrs = NULL;

    if ((NULL != instObj) && (NULL != hObj))
    {
        status = FVID2_SOK;
        /* enable buffer object if used */
        bool enableBufObj = false;

        if (true == instObj->initPrms.configThroughUdmaFlag)
        {
            if (true == hObj->appBuffInitDone)
            {
                /* we will be writing configuration to buffer object */
                regAddrs = &hObj->buffRegAddrs;
                enableBufObj = true;
            }
            else
            {
                status = FVID2_EFAIL;
            }

        }
        else
        {
            regAddrs = &instObj->regAddrs;
        }

        if (FVID2_SOK == status)
        {
            if ((uint32_t)TRUE == hObj->vsPrms.enableGlbce)
            {
                status = CSL_glbceStart(regAddrs->glbceRegs);
                /* Enable used buffer object */
                if ((FVID2_SOK == status) && (true == enableBufObj))
                {
                    hObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].isModified =
                            true;
                }
            }

            if (FVID2_SOK == status)
            {
                switch (hObj->h3aModule)
                {
                    default:
                    case H3A_MODULE_MAX:
                        /* Stop both the modules */
                        CSL_h3aStop(regAddrs->h3aRegs, H3A_MODULE_AF);
                        CSL_h3aStop(regAddrs->h3aRegs, H3A_MODULE_AEWB);
                        break;
                    case H3A_MODULE_AF:
                        /* Enable AF, but disable AEWB */
                        CSL_h3aStart(regAddrs->h3aRegs, H3A_MODULE_AF);
                        CSL_h3aStop(regAddrs->h3aRegs, H3A_MODULE_AEWB);
                        break;
                    case H3A_MODULE_AEWB:
                        /* Enable AEWB, but disable AF */
                        CSL_h3aStart(regAddrs->h3aRegs, H3A_MODULE_AEWB);
                        CSL_h3aStop(regAddrs->h3aRegs, H3A_MODULE_AF);
                        break;
                }
                /* Enable used buffer object */
                if (true == enableBufObj)
                {
                    hObj->bufferObjHolder[BUFF_ID_H3A_REGS].isModified = true;
                }
            }
        }
    }

    return (status);
}

void Vhwa_m2mVissStopModules(Vhwa_M2mVissInstObj *instObj,
    const Vhwa_M2mVissHandleObj *hObj)
{
    Vhwa_M2mVissRegAddr    *regAddrs = NULL;

    if ((NULL != instObj) && (NULL != hObj))
    {
        regAddrs = &instObj->regAddrs;
        if (H3A_MODULE_AEWB == hObj->h3aModule)
        {
            CSL_h3aStop(regAddrs->h3aRegs, H3A_MODULE_AEWB);
        }
        if (H3A_MODULE_AF == hObj->h3aModule)
        {
            CSL_h3aStop(regAddrs->h3aRegs, H3A_MODULE_AF);
        }
    }
}

void Vhwa_m2mVissGlbceInit(Vhwa_M2mVissInstObj *instObj,
    	Vhwa_M2mVissHandleObj *hObj)
{
    Vhwa_M2mVissRegAddr    *regAddrs = NULL;
    volatile uint32_t       timeOutIter = VHWA_M2M_VISS_GLBCE_TIMEOUT_ITER;

    if ((NULL != instObj) && (NULL != hObj))
    {
        regAddrs = &instObj->regAddrs;

        if ((uint32_t)TRUE == hObj->vsPrms.enableGlbce)
        {
            /* Enable GLBCE at the VISS Top */
            CSL_vissTopGlbceEnable(regAddrs->topRegs, TRUE);

            /* If GLBCE FILT_DONE is not set, perform GLBCE startup sequence */
            if ((uint32_t)FALSE ==
                    CSL_vissTopIsGlbceFiltDone(regAddrs->topRegs))
            {
                /* Set the clock to free running */
                CSL_vissTopGlbceFreeRunning(regAddrs->topRegs, TRUE);

                /* Wait for Filtering done interrupt, status bit set */
                do
                {
                    if ((uint32_t)TRUE ==
                            CSL_vissTopIsGlbceFiltDone(regAddrs->topRegs))
                    {
                        break;
                    }
                    timeOutIter = timeOutIter - 1u;
                } while (timeOutIter > 0u);

                /* Disable Free running clock */
                CSL_vissTopGlbceFreeRunning(regAddrs->topRegs, FALSE);
            }
        }
    }
}

void Vhwa_m2mVissNsf4Init(Vhwa_M2mVissInstObj *instObj,
    	Vhwa_M2mVissHandleObj *hObj)
{
    Vhwa_M2mVissRegAddr    *regAddrs = NULL;

    if ((NULL != instObj) && (NULL != hObj))
    {
        regAddrs = &instObj->regAddrs;

        if ((uint32_t)TRUE == hObj->vsPrms.enableNsf4)
        {
            CSL_vissTopNsf4Enable(regAddrs->topRegs, TRUE);
        }
        else
        {
            CSL_vissTopNsf4Enable(regAddrs->topRegs, FALSE);
        }
    }
}


int32_t Vhwa_m2mVissSetH3aParams(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj, H3a_Config *h3aCfg)
{
    int32_t                 status = FVID2_EBADARGS;
    uint32_t                sizeInBytes = 0u;
    uint32_t                sl2LineSize;
    Vhwa_M2mVissChParams   *chPrms = NULL;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != h3aCfg))
    {
        chPrms  = &hObj->outChPrms[VHWA_M2M_VISS_OUT_H3A_IDX];

        if ((uint32_t)TRUE == chPrms->isEnabled)
        {
            /* First set the H3A Config */
            status = Vhwa_m2mVissSetH3aConfig(instObj, hObj, h3aCfg);
        }
        else /* H3A Must be enable in SET_PARAMS */
        {
            status = FVID2_EINVALID_PARAMS;
        }

        if (FVID2_SOK == status)
        {
            /* Calculate size in bytes*/
            if (H3A_MODULE_AEWB == h3aCfg->module)
            {
                sizeInBytes = CSL_h3aCalcAewbSize(h3aCfg);
            }
            else /* Get the size for AF */
            {
                sizeInBytes = CSL_h3aCalcAfSize(h3aCfg);
            }

            /* Return the output size in the H3A Config */
            h3aCfg->outputSize = sizeInBytes;

            /* Updated the width and height as per the output size */
            sl2LineSize    = chPrms->widthInBytes;
            chPrms->width  = sl2LineSize;
            chPrms->height = sizeInBytes / sl2LineSize;

            if (0u != (sizeInBytes % sl2LineSize))
            {
                chPrms->height ++;
            }

            /* Store h3a module here, so that it can be enabled/disabled
             * at start time. */
            hObj->h3aModule = h3aCfg->module;
        }

        Vhwa_m2mVissSetH3aTrDesc(instObj, hObj);
    }

    return (status);
}

void Vhwa_m2mVissInitAddresses(Vhwa_M2mVissInstObj *instObj)
{
    uint32_t vissBaseAddr;

    if (NULL != instObj)
    {
        vissBaseAddr = instObj->socInfo.vissBaseAddr;

        instObj->regAddrs.topRegs =
            (CSL_viss_topRegs *)(vissBaseAddr +
                CSL_VPAC_VISS_VISS_TOP_REGS_BASE);
        instObj->regAddrs.lseRegs =
            (CSL_lseRegs *)(vissBaseAddr +
                CSL_VPAC_VISS_LSE_REGS_BASE);
        instObj->regAddrs.htsRegs =
            instObj->socInfo.htsRegs;

        instObj->regAddrs.rfeRegs =
            (CSL_rawfe_cfgRegs *)(vissBaseAddr +
                CSL_VPAC_VISS_RAWFE_CFG_REGS_BASE);
        instObj->regAddrs.glbceRegs =
            (CSL_glbceRegs *)(vissBaseAddr +
                CSL_VPAC_VISS_GLBCE_CFG_REGS_BASE);
        /* Assuming System Address is same as local R5F
         * address for GLBCE */
        instObj->regAddrs.glbceStatsMem =
            (CSL_glbce_statmemRegs *)(vissBaseAddr +
                 CSL_VPAC_VISS_GLBCE_STATMEM_REGS_BASE);
        instObj->regAddrs.fcpRegs =
            (CSL_flexcc_cfgRegs *)(vissBaseAddr +
                CSL_VPAC_VISS_FLEXCC_CFG_REGS_BASE);
        instObj->regAddrs.cfaRegs =
            (CSL_flexcfaRegs *)(vissBaseAddr +
                CSL_VPAC_VISS_FLEXCFA_REGS_BASE);
        instObj->regAddrs.nsf4Regs =
            (CSL_nsf4vRegs *)(vissBaseAddr +
                CSL_VPAC_VISS_NSF4V_REGS_BASE);
        instObj->regAddrs.h3aRegs =
            (CSL_rawfe_h3a_cfgRegs *)(vissBaseAddr +
                CSL_VPAC_VISS_RAWFE_H3A_CFG_REGS_BASE);

        instObj->regAddrs.dpcLutAddr =
            (uint32_t *)(vissBaseAddr +
                CSL_VPAC_VISS_RAWFE_DPC_LUT_RAM_REGS_BASE);
        instObj->regAddrs.lscLutAddr =
            (uint32_t *)(vissBaseAddr +
                CSL_VPAC_VISS_RAWFE_LSC_LUT_RAM_REGS_BASE);
        instObj->regAddrs.decompLut3Addr =
            (uint32_t *)(vissBaseAddr +
                CSL_VPAC_VISS_RAWFE_PWL_LUT3_RAM_REGS_BASE);
        instObj->regAddrs.decompLut2Addr =
            (uint32_t *)(vissBaseAddr +
                CSL_VPAC_VISS_RAWFE_PWL_LUT2_RAM_REGS_BASE);
        instObj->regAddrs.decompLut1Addr =
            (uint32_t *)(vissBaseAddr +
                CSL_VPAC_VISS_RAWFE_PWL_LUT1_RAM_REGS_BASE);
        instObj->regAddrs.compLutAddr =
            (uint32_t *)(vissBaseAddr +
                CSL_VPAC_VISS_RAWFE_WDR_LUT_RAM_REGS_BASE);
        instObj->regAddrs.h3aLutAddr =
            (uint32_t *)(vissBaseAddr +
                CSL_VPAC_VISS_RAWFE_H3A_LUT_RAM_REGS_BASE);

        instObj->regAddrs.gammaLut1Addr =
            (uint32_t *)(vissBaseAddr +
                CSL_VPAC_VISS_FLEXCC_CONTRASTC1_REGS_BASE);
        instObj->regAddrs.gammaLut2Addr =
            (uint32_t *)(vissBaseAddr +
                CSL_VPAC_VISS_FLEXCC_CONTRASTC2_REGS_BASE);
        instObj->regAddrs.gammaLut3Addr =
            (uint32_t *)(vissBaseAddr +
                CSL_VPAC_VISS_FLEXCC_CONTRASTC3_REGS_BASE);

        instObj->regAddrs.y8C8LutAddr =
            (uint32_t *)(vissBaseAddr +
                CSL_VPAC_VISS_FLEXCC_Y8R8_REGS_BASE);
        instObj->regAddrs.c8G8LutAddr =
            (uint32_t *)(vissBaseAddr +
                CSL_VPAC_VISS_FLEXCC_C8G8_REGS_BASE);
        instObj->regAddrs.s8B8LutAddr =
            (uint32_t *)(vissBaseAddr +
                CSL_VPAC_VISS_FLEXCC_S8B8_REGS_BASE);

        instObj->regAddrs.eeRegs = (CSL_flexeeRegs *)
            (vissBaseAddr +
                CSL_VPAC_VISS_FLEXEE_REGS_BASE);

        instObj->regAddrs.histRegs = (CSL_flexcc_histRegs *)
            (vissBaseAddr +
                CSL_VPAC_VISS_FLEXCC_HIST_REGS_BASE);
    }
}

void Vhwa_m2mVissInitBuffAddresses(Vhwa_M2mVissHandleObj *hObj,
        uint32_t *bufferPtr)
{
    if ((NULL != hObj) && (NULL != bufferPtr))
    {
        hObj->buffRegAddrs.topRegs = (CSL_viss_topRegs*) bufferPtr;
        /* size alloted for topRegs */
        bufferPtr = bufferPtr
                + ((sizeof(CSL_viss_topRegs)) / (sizeof(uint32_t)));

        hObj->buffRegAddrs.lseRegs = (CSL_lseRegs*) bufferPtr;
        /* size alloted for lseRegs */
        bufferPtr = bufferPtr + ((sizeof(CSL_lseRegs)) / (sizeof(uint32_t)));

        hObj->buffRegAddrs.htsRegs = (CSL_htsRegs*) bufferPtr;
        /* size alloted for hts hwa0, DMA 0-4 and DMA 240-245 registers */
        bufferPtr = bufferPtr + ((sizeof(CSL_htsRegs)) / (sizeof(uint32_t)));

        hObj->buffRegAddrs.rfeRegs = (CSL_rawfe_cfgRegs*) bufferPtr;
        /* size alloted for rfeRegs */
        bufferPtr = bufferPtr
                + ((sizeof(CSL_rawfe_cfgRegs)) / (sizeof(uint32_t)));

        hObj->buffRegAddrs.dpcLutAddr = bufferPtr;
        /* size alloted for dpcLutAddr */
        bufferPtr = bufferPtr + RFE_DPC_LUT_SIZE;

        hObj->buffRegAddrs.lscLutAddr = bufferPtr;
        /* size alloted for lscLutAddr */
        bufferPtr = bufferPtr + RFE_LSC_TBL_SIZE;

        /*
         * LUT has 639 entries, Two lut entries are stored in one 32bit memory
         * so Last Lut entry will be stored only on Low position
         * Size should be (639/2) + 1;
         */
        hObj->buffRegAddrs.decompLut1Addr = bufferPtr;
        /* size alloted for decompLut1Addr */
        bufferPtr = bufferPtr + ((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1);

        /*
         * LUT has 639 entries, Two lut entries are stored in one 32bit memory
         * so Last Lut entry will be stored only on Low position
         * Size should be (639/2) + 1;
         */
        hObj->buffRegAddrs.decompLut2Addr = bufferPtr;
        /* size alloted for decompLut2Addr */
        bufferPtr = bufferPtr + ((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1);

        /*
         * LUT has 639 entries, Two lut entries are stored in one 32bit memory
         * so Last Lut entry will be stored only on Low position
         * Size should be (639/2) + 1;
         */
        hObj->buffRegAddrs.decompLut3Addr = bufferPtr;
        /* size alloted for decompLut3Addr */
        bufferPtr = bufferPtr + ((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1);

        /*
         * LUT has 639 entries, Two lut entries are stored in one 32bit memory
         * so Last Lut entry will be stored only on Low position
         * Size should be (639/2) + 1;
         */
        hObj->buffRegAddrs.compLutAddr = bufferPtr;
        /* size alloted for compLutAddr */
        bufferPtr = bufferPtr + ((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1);

        hObj->buffRegAddrs.h3aLutAddr = bufferPtr;
        /* size alloted for h3aLutAddr */
        bufferPtr = bufferPtr + ((RFE_H3A_COMP_LUT_SIZE / 2) + 1);

        hObj->buffRegAddrs.h3aRegs = (CSL_rawfe_h3a_cfgRegs*) bufferPtr;
        /* size alloted for h3aRegs */
        bufferPtr = bufferPtr
                + ((sizeof(CSL_rawfe_h3a_cfgRegs)) / (sizeof(uint32_t)));

        hObj->buffRegAddrs.glbceRegs = (CSL_glbceRegs*) bufferPtr;
        /* size alloted for glbceRegs */
        bufferPtr = bufferPtr + ((sizeof(CSL_glbceRegs)) / (sizeof(uint32_t)));

        hObj->buffRegAddrs.glbceStatsMem = (CSL_glbce_statmemRegs*) bufferPtr;
        /* size alloted for glbceStatsMem */
        bufferPtr = bufferPtr
                + ((sizeof(CSL_glbce_statmemRegs)) / (sizeof(uint32_t)));

        hObj->buffRegAddrs.nsf4Regs = (CSL_nsf4vRegs*) bufferPtr;
        /* size alloted for nsf4Regs */
        bufferPtr = bufferPtr + ((sizeof(CSL_nsf4vRegs)) / (sizeof(uint32_t)));

        hObj->buffRegAddrs.cfaRegs = (CSL_flexcfaRegs*) bufferPtr;
        /* size alloted for cfaRegs */
        bufferPtr = bufferPtr
                + ((sizeof(CSL_flexcfaRegs)) / (sizeof(uint32_t)));

        hObj->buffRegAddrs.fcpRegs = (CSL_flexcc_cfgRegs*) bufferPtr;
        /* size alloted for fcpRegs */
        bufferPtr = bufferPtr
                + ((sizeof(CSL_flexcc_cfgRegs)) / (sizeof(uint32_t)));

        /*
         * LUT has 513 entries, Two lut entries are stored in one 32bit memory
         * so Last Lut entry will be stored only on Low position
         * Size should be (513/2) + 1;
         */
        hObj->buffRegAddrs.gammaLut1Addr = bufferPtr;
        /* size alloted for gammaLut1Addr */
        bufferPtr = bufferPtr + ((FCP_GAMMA_LUT_SIZE / 2) + 1);

        /*
         * LUT has 513 entries, Two lut entries are stored in one 32bit memory
         * so Last Lut entry will be stored only on Low position
         * Size should be (513/2) + 1;
         */
        hObj->buffRegAddrs.gammaLut2Addr = bufferPtr;
        /* size alloted for gammaLut2Addr */
        bufferPtr = bufferPtr + ((FCP_GAMMA_LUT_SIZE / 2) + 1);

        /*
         * LUT has 513 entries, Two lut entries are stored in one 32bit memory
         * so Last Lut entry will be stored only on Low position
         * Size should be (513/2) + 1;
         */
        hObj->buffRegAddrs.gammaLut3Addr = bufferPtr;
        /* size alloted for gammaLut2Addr */
        bufferPtr = bufferPtr + ((FCP_GAMMA_LUT_SIZE / 2) + 1);

        /*
         * LUT has 513 entries, Two lut entries are stored in one 32bit memory
         * so Last Lut entry will be stored only on Low position
         * Size should be (513/2) + 1;
         */
        hObj->buffRegAddrs.y8C8LutAddr = bufferPtr;
        /* size alloted for y8C8LutAddr */
        bufferPtr = bufferPtr + ((FCP_RGB_YUVSAT_LUT_SIZE / 2) + 1);

        /*
         * LUT has 513 entries, Two lut entries are stored in one 32bit memory
         * so Last Lut entry will be stored only on Low position
         * Size should be (513/2) + 1;
         */
        hObj->buffRegAddrs.c8G8LutAddr = bufferPtr;
        /* size alloted for c8G8LutAddr */
        bufferPtr = bufferPtr + ((FCP_RGB_YUVSAT_LUT_SIZE / 2) + 1);

        /*
         * LUT has 513 entries, Two lut entries are stored in one 32bit memory
         * so Last Lut entry will be stored only on Low position
         * Size should be (513/2) + 1;
         */
        hObj->buffRegAddrs.s8B8LutAddr = bufferPtr;
        /* size alloted for s8B8LutAddr */
        bufferPtr = bufferPtr + ((FCP_RGB_YUVSAT_LUT_SIZE / 2) + 1);

        hObj->buffRegAddrs.eeRegs = (CSL_flexeeRegs*) bufferPtr;
        /* size alloted for eeRegs */
        bufferPtr = bufferPtr + ((sizeof(CSL_flexeeRegs)) / (sizeof(uint32_t)));

        hObj->buffRegAddrs.histRegs = (CSL_flexcc_histRegs*) bufferPtr;
        /* size alloted for histRegs */
        bufferPtr = bufferPtr
                + ((sizeof(CSL_flexcc_histRegs)) / (sizeof(uint32_t)));
    }
}

void Vhwa_m2mVissInitBuffObject(Vhwa_M2mVissInstObj *instObj,
        Vhwa_M2mVissHandleObj *hObj)
{
    if ((NULL != instObj) && (NULL != hObj))
    {
        /* BUFF_ID_TOP_REGS */
        /* initialize buffer object for BUFF_ID_TOP_REGS */
        hObj->bufferObjHolder[BUFF_ID_TOP_REGS].bufferID = BUFF_ID_TOP_REGS;
        hObj->bufferObjHolder[BUFF_ID_TOP_REGS].length =
                sizeof(CSL_viss_topRegs);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_TOP_REGS].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_TOP_REGS].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.topRegs;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_TOP_REGS].dstPtr =
                (uint32_t*) instObj->regAddrs.topRegs;

        /* BUFF_ID_LSE_REGS */
        /* initialize buffer object for BUFF_ID_LSE_REGS */
        hObj->bufferObjHolder[BUFF_ID_LSE_REGS].bufferID = BUFF_ID_LSE_REGS;
        hObj->bufferObjHolder[BUFF_ID_LSE_REGS].length = sizeof(CSL_lseRegs);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_LSE_REGS].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_LSE_REGS].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.lseRegs;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_LSE_REGS].dstPtr =
                (uint32_t*) instObj->regAddrs.lseRegs;

        /* BUFF_ID_HTS_HWA0_REGS */
        /* initialize buffer object for BUFF_ID_HTS_HWA0_REGS */
        hObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].bufferID =
                BUFF_ID_HTS_HWA0_REGS;
        hObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].length =
                (BUFF_OBJ_HTS_HWA0_REG_SIZE * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].srcPtr =
                (uint32_t*) ((uint32_t*) hObj->buffRegAddrs.htsRegs
                        + (CSL_HTS_HWA0_SCHEDULER_CONTROL / 4));
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].dstPtr =
                (uint32_t*) ((uint32_t*) instObj->regAddrs.htsRegs
                        + (CSL_HTS_HWA0_SCHEDULER_CONTROL / 4));

        /* BUFF_ID_HTS_DMA_0_4_REGS */
        /* initialize buffer object for BUFF_ID_HTS_DMA_0_4_REGS */
        hObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].bufferID =
                BUFF_ID_HTS_DMA_0_4_REGS;
        hObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].length =
                (BUFF_OBJ_HTS_DMA_0_4_REG_SIZE * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].srcPtr =
                (uint32_t*) ((uint32_t*) hObj->buffRegAddrs.htsRegs
                        + (CSL_HTS_DMA0_SCHEDULER_CONTROL / 4));
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].dstPtr =
                (uint32_t*) ((uint32_t*) instObj->regAddrs.htsRegs
                        + (CSL_HTS_DMA0_SCHEDULER_CONTROL / 4));

        /* BUFF_ID_HTS_DMA_240_245_REGS */
        /* initialize buffer object for BUFF_ID_HTS_DMA_240_245_REGS */
        hObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].bufferID =
                BUFF_ID_HTS_DMA_240_245_REGS;
        hObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].length =
                (BUFF_OBJ_HTS_DMA240_245_REG_SIZE * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].srcPtr =
                (uint32_t*) ((uint32_t*) hObj->buffRegAddrs.htsRegs
                        + (CSL_HTS_DMA240_SCHEDULER_CONTROL / 4));
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].dstPtr =
                (uint32_t*) ((uint32_t*) instObj->regAddrs.htsRegs
                        + (CSL_HTS_DMA240_SCHEDULER_CONTROL / 4));

        /* BUFF_ID_RFE_REGS */
        /* initialize buffer object for BUFF_ID_RFE_REGS */
        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].bufferID = BUFF_ID_RFE_REGS;
        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].length =
                sizeof(CSL_rawfe_cfgRegs);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.rfeRegs;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_RFE_REGS].dstPtr =
                (uint32_t*) instObj->regAddrs.rfeRegs;

        /* BUFF_ID_DPC_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_DPC_LUT_ADDR */
        hObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].bufferID =
                BUFF_ID_DPC_LUT_ADDR;
        hObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].length = (RFE_DPC_LUT_SIZE
                * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.dpcLutAddr;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].dstPtr =
                (uint32_t*) instObj->regAddrs.dpcLutAddr;

        /* BUFF_ID_LSC_SUT_ADDR */
        /* initialize buffer object for BUFF_ID_LSC_SUT_ADDR */
        hObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].bufferID =
                BUFF_ID_LSC_SUT_ADDR;
        hObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].length = (RFE_LSC_TBL_SIZE
                * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.lscLutAddr;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].dstPtr =
                (uint32_t*) instObj->regAddrs.lscLutAddr;

        /* BUFF_ID_DECOMP_LUT1_ADDR */
        /* initialize buffer object for BUFF_ID_DECOMP_LUT1_ADDR */
        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].bufferID =
                BUFF_ID_DECOMP_LUT1_ADDR;
        /* two LUT's will be written to one 32 bit register */
        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].length =
                (((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.decompLut1Addr;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].dstPtr =
                (uint32_t*) instObj->regAddrs.decompLut1Addr;

        /* BUFF_ID_DECOMP_LUT2_ADDR */
        /* initialize buffer object for BUFF_ID_DECOMP_LUT2_ADDR */
        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].bufferID =
                BUFF_ID_DECOMP_LUT2_ADDR;
        /* two LUT's will be written to one 32 bit register */
        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].length =
                (((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.decompLut2Addr;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].dstPtr =
                (uint32_t*) instObj->regAddrs.decompLut2Addr;

        /* BUFF_ID_DECOMP_LUT3_ADDR */
        /* initialize buffer object for BUFF_ID_DECOMP_LUT3_ADDR */
        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].bufferID =
                BUFF_ID_DECOMP_LUT3_ADDR;
        /* two LUT's will be written to one 32 bit register */
        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].length =
                (((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.decompLut3Addr;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].dstPtr =
                (uint32_t*) instObj->regAddrs.decompLut3Addr;

        /* BUFF_ID_COMP_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_COMP_LUT_ADDR */
        hObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].bufferID =
                BUFF_ID_COMP_LUT_ADDR;
        /* two LUT's will be written to one 32 bit register */
        hObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].length =
                (((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.compLutAddr;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].dstPtr =
                (uint32_t*) instObj->regAddrs.compLutAddr;

        /* BUFF_ID_H3A_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_H3A_LUT_ADDR */
        hObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].bufferID =
                BUFF_ID_H3A_LUT_ADDR;
        hObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].length =
                (((RFE_H3A_COMP_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.h3aLutAddr;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].dstPtr =
                (uint32_t*) instObj->regAddrs.h3aLutAddr;

        /* BUFF_ID_H3A_REGS */
        /* initialize buffer object for BUFF_ID_H3A_REGS */
        hObj->bufferObjHolder[BUFF_ID_H3A_REGS].bufferID = BUFF_ID_H3A_REGS;
        hObj->bufferObjHolder[BUFF_ID_H3A_REGS].length =
                sizeof(CSL_rawfe_h3a_cfgRegs);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_H3A_REGS].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_H3A_REGS].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.h3aRegs;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_H3A_REGS].dstPtr =
                (uint32_t*) instObj->regAddrs.h3aRegs;

        /* BUFF_ID_GLBCE_REGS */
        /* initialize buffer object for BUFF_ID_GLBCE_REGS */
        hObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].bufferID = BUFF_ID_GLBCE_REGS;
        hObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].length =
                sizeof(CSL_glbceRegs);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.glbceRegs;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].dstPtr =
                (uint32_t*) instObj->regAddrs.glbceRegs;

        /* BUFF_ID_GLBCE_STATS_MEM */
        /* initialize buffer object for BUFF_ID_GLBCE_STATS_MEM */
        hObj->bufferObjHolder[BUFF_ID_GLBCE_STATS_MEM].bufferID =
                BUFF_ID_GLBCE_STATS_MEM;
        hObj->bufferObjHolder[BUFF_ID_GLBCE_STATS_MEM].length =
                sizeof(CSL_glbce_statmemRegs);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_GLBCE_STATS_MEM].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_GLBCE_STATS_MEM].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.glbceStatsMem;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_GLBCE_STATS_MEM].dstPtr =
                (uint32_t*) instObj->regAddrs.glbceStatsMem;

        /* BUFF_ID_NSF4_REGS */
        /* initialize buffer object for BUFF_ID_NSF4_REGS */
        hObj->bufferObjHolder[BUFF_ID_NSF4_REGS].bufferID = BUFF_ID_NSF4_REGS;
        hObj->bufferObjHolder[BUFF_ID_NSF4_REGS].length = sizeof(CSL_nsf4vRegs);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_NSF4_REGS].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_NSF4_REGS].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.nsf4Regs;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_NSF4_REGS].dstPtr =
                (uint32_t*) instObj->regAddrs.nsf4Regs;

        /* BUFF_ID_CFA_REGS */
        /* initialize buffer object for BUFF_ID_CFA_REGS */
        hObj->bufferObjHolder[BUFF_ID_CFA_REGS].bufferID = BUFF_ID_CFA_REGS;
        hObj->bufferObjHolder[BUFF_ID_CFA_REGS].length =
                sizeof(CSL_flexcfaRegs);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_CFA_REGS].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_CFA_REGS].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.cfaRegs;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_CFA_REGS].dstPtr =
                (uint32_t*) instObj->regAddrs.cfaRegs;

        /* BUFF_ID_FCP_REGS */
        /* initialize buffer object for BUFF_ID_FCP_REGS */
        hObj->bufferObjHolder[BUFF_ID_FCP_REGS].bufferID = BUFF_ID_FCP_REGS;
        hObj->bufferObjHolder[BUFF_ID_FCP_REGS].length =
                sizeof(CSL_flexcc_cfgRegs);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_FCP_REGS].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_FCP_REGS].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.fcpRegs;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_FCP_REGS].dstPtr =
                (uint32_t*) instObj->regAddrs.fcpRegs;

        /* BUFF_ID_GAMMA_LUT1_ADDR */
        /* initialize buffer object for BUFF_ID_GAMMA_LUT1_ADDR */
        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].bufferID =
                BUFF_ID_GAMMA_LUT1_ADDR;
        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].length =
                (((FCP_GAMMA_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.gammaLut1Addr;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].dstPtr =
                (uint32_t*) instObj->regAddrs.gammaLut1Addr;

        /* BUFF_ID_GAMMA_LUT2_ADDR */
        /* initialize buffer object for BUFF_ID_GAMMA_LUT2_ADDR */
        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].bufferID =
                BUFF_ID_GAMMA_LUT2_ADDR;
        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].length =
                (((FCP_GAMMA_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.gammaLut2Addr;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].dstPtr =
                (uint32_t*) instObj->regAddrs.gammaLut2Addr;

        /* BUFF_ID_GAMMA_LUT3_ADDR */
        /* initialize buffer object for BUFF_ID_GAMMA_LUT3_ADDR */
        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].bufferID =
                BUFF_ID_GAMMA_LUT3_ADDR;
        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].length =
                (((FCP_GAMMA_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.gammaLut3Addr;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].dstPtr =
                (uint32_t*) instObj->regAddrs.gammaLut3Addr;

        /* BUFF_ID_Y8C8_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_Y8C8_LUT_ADDR */
        hObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].bufferID =
                BUFF_ID_Y8C8_LUT_ADDR;
        hObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].length =
                (((FCP_RGB_YUVSAT_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.y8C8LutAddr;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].dstPtr =
                (uint32_t*) instObj->regAddrs.y8C8LutAddr;

        /* BUFF_ID_C8G8_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_C8G8_LUT_ADDR */
        hObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].bufferID =
                BUFF_ID_C8G8_LUT_ADDR;
        hObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].length =
                (((FCP_RGB_YUVSAT_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.c8G8LutAddr;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].dstPtr =
                (uint32_t*) instObj->regAddrs.c8G8LutAddr;

        /* BUFF_ID_S8B8_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_S8B8_LUT_ADDR */
        hObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].bufferID =
                BUFF_ID_S8B8_LUT_ADDR;
        hObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].length =
                (((FCP_RGB_YUVSAT_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.s8B8LutAddr;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].dstPtr =
                (uint32_t*) instObj->regAddrs.s8B8LutAddr;

        /* BUFF_ID_EE_REGS */
        /* initialize buffer object for BUFF_ID_EE_REGS */
        hObj->bufferObjHolder[BUFF_ID_EE_REGS].bufferID = BUFF_ID_EE_REGS;
        hObj->bufferObjHolder[BUFF_ID_EE_REGS].length = sizeof(CSL_flexeeRegs);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_EE_REGS].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_EE_REGS].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.eeRegs;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_EE_REGS].dstPtr =
                (uint32_t*) instObj->regAddrs.eeRegs;

        /* BUFF_ID_HIST_REGS */
        /* initialize buffer object for BUFF_ID_HIST_REGS */
        hObj->bufferObjHolder[BUFF_ID_HIST_REGS].bufferID = BUFF_ID_HIST_REGS;
        hObj->bufferObjHolder[BUFF_ID_HIST_REGS].length =
                sizeof(CSL_flexcc_histRegs);
        /* enable when modified, disable after creating the TR */
        hObj->bufferObjHolder[BUFF_ID_HIST_REGS].isModified = false;
        /* This will be the buffer pointer */
        hObj->bufferObjHolder[BUFF_ID_HIST_REGS].srcPtr =
                (uint32_t*) hObj->buffRegAddrs.histRegs;
        /* This will be register address */
        hObj->bufferObjHolder[BUFF_ID_HIST_REGS].dstPtr =
                (uint32_t*) instObj->regAddrs.histRegs;
    }
}

void Vhwa_m2mVissInitSaveBuffObject(Vhwa_M2mVissInstObj *instObj)
{
    if (NULL != instObj)
    {
        /* BUFF_ID_TOP_REGS */
        /* initialize buffer object for BUFF_ID_TOP_REGS */
        instObj->bufferObjHolder[BUFF_ID_TOP_REGS].bufferID = BUFF_ID_TOP_REGS;
        instObj->bufferObjHolder[BUFF_ID_TOP_REGS].length =
                sizeof(CSL_viss_topRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_TOP_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_TOP_REGS].srcPtr =
                (uint32_t*) instObj->regAddrs.topRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_TOP_REGS].dstPtr =
                (uint32_t*) &instObj->regData.topRegs;

        /* BUFF_ID_LSE_REGS */
        /* initialize buffer object for BUFF_ID_LSE_REGS */
        instObj->bufferObjHolder[BUFF_ID_LSE_REGS].bufferID = BUFF_ID_LSE_REGS;
        instObj->bufferObjHolder[BUFF_ID_LSE_REGS].length = sizeof(CSL_lseRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_LSE_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_LSE_REGS].srcPtr =
                (uint32_t*) instObj->regAddrs.lseRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_LSE_REGS].dstPtr =
                (uint32_t*) &instObj->regData.lseRegs;

        /* BUFF_ID_HTS_HWA0_REGS */
        /* initialize buffer object for BUFF_ID_HTS_HWA0_REGS */
        instObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].bufferID =
                BUFF_ID_HTS_HWA0_REGS;
        instObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].length =
                (BUFF_OBJ_HTS_HWA0_REG_SIZE * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].isModified = false;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].srcPtr =
                (uint32_t*) ((uint32_t*) instObj->regAddrs.htsRegs
                        + (CSL_HTS_HWA0_SCHEDULER_CONTROL / 4));
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].dstPtr =
                (uint32_t*) ((uint32_t*) &instObj->regData.htsRegs
                        + (CSL_HTS_HWA0_SCHEDULER_CONTROL / 4));

        /* BUFF_ID_HTS_DMA_0_4_REGS */
        /* initialize buffer object for BUFF_ID_HTS_DMA_0_4_REGS */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].bufferID =
                BUFF_ID_HTS_DMA_0_4_REGS;
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].length =
                (BUFF_OBJ_HTS_DMA_0_4_REG_SIZE * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].isModified = false;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].srcPtr =
                (uint32_t*) ((uint32_t*) instObj->regAddrs.htsRegs
                        + (CSL_HTS_DMA0_SCHEDULER_CONTROL / 4));
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].dstPtr =
                (uint32_t*) ((uint32_t*) &instObj->regData.htsRegs
                        + (CSL_HTS_DMA0_SCHEDULER_CONTROL / 4));

        /* BUFF_ID_HTS_DMA_240_245_REGS */
        /* initialize buffer object for BUFF_ID_HTS_DMA_240_245_REGS */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].bufferID =
                BUFF_ID_HTS_DMA_240_245_REGS;
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].length =
                (BUFF_OBJ_HTS_DMA240_245_REG_SIZE * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].isModified =
                false;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].srcPtr =
                (uint32_t*) ((uint32_t*) instObj->regAddrs.htsRegs
                        + (CSL_HTS_DMA240_SCHEDULER_CONTROL / 4));
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].dstPtr =
                (uint32_t*) ((uint32_t*) &instObj->regData.htsRegs
                        + (CSL_HTS_DMA240_SCHEDULER_CONTROL / 4));

        /* BUFF_ID_RFE_REGS */
        /* initialize buffer object for BUFF_ID_RFE_REGS */
        instObj->bufferObjHolder[BUFF_ID_RFE_REGS].bufferID = BUFF_ID_RFE_REGS;
        instObj->bufferObjHolder[BUFF_ID_RFE_REGS].length =
                sizeof(CSL_rawfe_cfgRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_RFE_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_RFE_REGS].srcPtr =
                (uint32_t*) instObj->regAddrs.rfeRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_RFE_REGS].dstPtr =
                (uint32_t*) &instObj->regData.rfeRegs;

        /* BUFF_ID_DPC_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_DPC_LUT_ADDR */
        instObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].bufferID =
                BUFF_ID_DPC_LUT_ADDR;
        instObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].length =
                (RFE_DPC_LUT_SIZE * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].srcPtr =
                (uint32_t*) instObj->regAddrs.dpcLutAddr;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].dstPtr =
                (uint32_t*) &instObj->regData.dpcLutData;

        /* BUFF_ID_LSC_SUT_ADDR */
        /* initialize buffer object for BUFF_ID_LSC_SUT_ADDR */
        instObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].bufferID =
                BUFF_ID_LSC_SUT_ADDR;
        instObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].length =
                (RFE_LSC_TBL_SIZE * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].srcPtr =
                (uint32_t*) instObj->regAddrs.lscLutAddr;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].dstPtr =
                (uint32_t*) &instObj->regData.lscLutData;

        /* BUFF_ID_DECOMP_LUT1_ADDR */
        /* initialize buffer object for BUFF_ID_DECOMP_LUT1_ADDR */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].bufferID =
                BUFF_ID_DECOMP_LUT1_ADDR;
        /* two LUT's will be written to one 32 bit register */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].length =
                (((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].srcPtr =
                (uint32_t*) instObj->regAddrs.decompLut1Addr;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].dstPtr =
                (uint32_t*) &instObj->regData.decompLut1Data;

        /* BUFF_ID_DECOMP_LUT2_ADDR */
        /* initialize buffer object for BUFF_ID_DECOMP_LUT2_ADDR */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].bufferID =
                BUFF_ID_DECOMP_LUT2_ADDR;
        /* two LUT's will be written to one 32 bit register */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].length =
                (((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].srcPtr =
                (uint32_t*) instObj->regAddrs.decompLut2Addr;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].dstPtr =
                (uint32_t*) &instObj->regData.decompLut2Data;

        /* BUFF_ID_DECOMP_LUT3_ADDR */
        /* initialize buffer object for BUFF_ID_DECOMP_LUT3_ADDR */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].bufferID =
                BUFF_ID_DECOMP_LUT3_ADDR;
        /* two LUT's will be written to one 32 bit register */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].length =
                (((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].srcPtr =
                (uint32_t*) instObj->regAddrs.decompLut3Addr;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].dstPtr =
                (uint32_t*) &instObj->regData.decompLut3Data;

        /* BUFF_ID_COMP_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_COMP_LUT_ADDR */
        instObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].bufferID =
                BUFF_ID_COMP_LUT_ADDR;
        /* two LUT's will be written to one 32 bit register */
        instObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].length =
                (((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].srcPtr =
                (uint32_t*) instObj->regAddrs.compLutAddr;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].dstPtr =
                (uint32_t*) &instObj->regData.compLutData;

        /* BUFF_ID_H3A_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_H3A_LUT_ADDR */
        instObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].bufferID =
                BUFF_ID_H3A_LUT_ADDR;
        instObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].length =
                (((RFE_H3A_COMP_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].srcPtr =
                (uint32_t*) instObj->regAddrs.h3aLutAddr;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].dstPtr =
                (uint32_t*) &instObj->regData.h3aLutData;

        /* BUFF_ID_H3A_REGS */
        /* initialize buffer object for BUFF_ID_H3A_REGS */
        instObj->bufferObjHolder[BUFF_ID_H3A_REGS].bufferID = BUFF_ID_H3A_REGS;
        instObj->bufferObjHolder[BUFF_ID_H3A_REGS].length =
                sizeof(CSL_rawfe_h3a_cfgRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_H3A_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_H3A_REGS].srcPtr =
                (uint32_t*) instObj->regAddrs.h3aRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_H3A_REGS].dstPtr =
                (uint32_t*) &instObj->regData.h3aRegs;

        /* BUFF_ID_GLBCE_REGS */
        /* initialize buffer object for BUFF_ID_GLBCE_REGS */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].bufferID =
                BUFF_ID_GLBCE_REGS;
        instObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].length =
                sizeof(CSL_glbceRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].srcPtr =
                (uint32_t*) instObj->regAddrs.glbceRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].dstPtr =
                (uint32_t*) &instObj->regData.glbceRegs;

        /* BUFF_ID_GLBCE_STATS_MEM */
        /* initialize buffer object for BUFF_ID_GLBCE_STATS_MEM */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_STATS_MEM].bufferID =
                BUFF_ID_GLBCE_STATS_MEM;
        instObj->bufferObjHolder[BUFF_ID_GLBCE_STATS_MEM].length =
                sizeof(CSL_glbce_statmemRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_STATS_MEM].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_STATS_MEM].srcPtr =
                (uint32_t*) instObj->regAddrs.glbceStatsMem;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_STATS_MEM].dstPtr =
                (uint32_t*) &instObj->regData.glbceStatsMem;

        /* BUFF_ID_NSF4_REGS */
        /* initialize buffer object for BUFF_ID_NSF4_REGS */
        instObj->bufferObjHolder[BUFF_ID_NSF4_REGS].bufferID =
                BUFF_ID_NSF4_REGS;
        instObj->bufferObjHolder[BUFF_ID_NSF4_REGS].length =
                sizeof(CSL_nsf4vRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_NSF4_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_NSF4_REGS].srcPtr =
                (uint32_t*) instObj->regAddrs.nsf4Regs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_NSF4_REGS].dstPtr =
                (uint32_t*) &instObj->regData.nsf4Regs;

        /* BUFF_ID_CFA_REGS */
        /* initialize buffer object for BUFF_ID_CFA_REGS */
        instObj->bufferObjHolder[BUFF_ID_CFA_REGS].bufferID = BUFF_ID_CFA_REGS;
        instObj->bufferObjHolder[BUFF_ID_CFA_REGS].length =
                sizeof(CSL_flexcfaRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_CFA_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_CFA_REGS].srcPtr =
                (uint32_t*) instObj->regAddrs.cfaRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_CFA_REGS].dstPtr =
                (uint32_t*) &instObj->regData.cfaRegs;

        /* BUFF_ID_FCP_REGS */
        /* initialize buffer object for BUFF_ID_FCP_REGS */
        instObj->bufferObjHolder[BUFF_ID_FCP_REGS].bufferID = BUFF_ID_FCP_REGS;
        instObj->bufferObjHolder[BUFF_ID_FCP_REGS].length =
                sizeof(CSL_flexcc_cfgRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_FCP_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_FCP_REGS].srcPtr =
                (uint32_t*) instObj->regAddrs.fcpRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_FCP_REGS].dstPtr =
                (uint32_t*) &instObj->regData.fcpRegs;

        /* BUFF_ID_GAMMA_LUT1_ADDR */
        /* initialize buffer object for BUFF_ID_GAMMA_LUT1_ADDR */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].bufferID =
                BUFF_ID_GAMMA_LUT1_ADDR;
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].length =
                (((FCP_GAMMA_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].srcPtr =
                (uint32_t*) instObj->regAddrs.gammaLut1Addr;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].dstPtr =
                (uint32_t*) &instObj->regData.gammaLut1Data;

        /* BUFF_ID_GAMMA_LUT2_ADDR */
        /* initialize buffer object for BUFF_ID_GAMMA_LUT2_ADDR */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].bufferID =
                BUFF_ID_GAMMA_LUT2_ADDR;
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].length =
                (((FCP_GAMMA_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].srcPtr =
                (uint32_t*) instObj->regAddrs.gammaLut2Addr;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].dstPtr =
                (uint32_t*) &instObj->regData.gammaLut2Data;

        /* BUFF_ID_GAMMA_LUT3_ADDR */
        /* initialize buffer object for BUFF_ID_GAMMA_LUT3_ADDR */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].bufferID =
                BUFF_ID_GAMMA_LUT3_ADDR;
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].length =
                (((FCP_GAMMA_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].srcPtr =
                (uint32_t*) instObj->regAddrs.gammaLut3Addr;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].dstPtr =
                (uint32_t*) &instObj->regData.gammaLut3Data;

        /* BUFF_ID_Y8C8_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_Y8C8_LUT_ADDR */
        instObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].bufferID =
                BUFF_ID_Y8C8_LUT_ADDR;
        instObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].length =
                (((FCP_RGB_YUVSAT_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].srcPtr =
                (uint32_t*) instObj->regAddrs.y8C8LutAddr;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].dstPtr =
                (uint32_t*) &instObj->regData.y8C8LutData;

        /* BUFF_ID_C8G8_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_C8G8_LUT_ADDR */
        instObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].bufferID =
                BUFF_ID_C8G8_LUT_ADDR;
        instObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].length =
                (((FCP_RGB_YUVSAT_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].srcPtr =
                (uint32_t*) instObj->regAddrs.c8G8LutAddr;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].dstPtr =
                (uint32_t*) &instObj->regData.c8G8LutData;

        /* BUFF_ID_S8B8_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_S8B8_LUT_ADDR */
        instObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].bufferID =
                BUFF_ID_S8B8_LUT_ADDR;
        instObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].length =
                (((FCP_RGB_YUVSAT_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].srcPtr =
                (uint32_t*) instObj->regAddrs.s8B8LutAddr;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].dstPtr =
                (uint32_t*) &instObj->regData.s8B8LutData;

        /* BUFF_ID_EE_REGS */
        /* initialize buffer object for BUFF_ID_EE_REGS */
        instObj->bufferObjHolder[BUFF_ID_EE_REGS].bufferID = BUFF_ID_EE_REGS;
        instObj->bufferObjHolder[BUFF_ID_EE_REGS].length =
                sizeof(CSL_flexeeRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_EE_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_EE_REGS].srcPtr =
                (uint32_t*) instObj->regAddrs.eeRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_EE_REGS].dstPtr =
                (uint32_t*) &instObj->regData.eeRegs;

        /* BUFF_ID_HIST_REGS */
        /* initialize buffer object for BUFF_ID_HIST_REGS */
        instObj->bufferObjHolder[BUFF_ID_HIST_REGS].bufferID =
                BUFF_ID_HIST_REGS;
        instObj->bufferObjHolder[BUFF_ID_HIST_REGS].length =
                sizeof(CSL_flexcc_histRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_HIST_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_HIST_REGS].srcPtr =
                (uint32_t*) instObj->regAddrs.histRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_HIST_REGS].dstPtr =
                (uint32_t*) &instObj->regData.histRegs;
    }
}

void Vhwa_m2mVissInitRestoreBuffObject(Vhwa_M2mVissInstObj *instObj,
        Vhwa_M2mVissHandleObj *hObj)
{
    if ((NULL != instObj) && (NULL != hObj))
    {
        /* BUFF_ID_TOP_REGS */
        /* initialize buffer object for BUFF_ID_TOP_REGS */
        instObj->bufferObjHolder[BUFF_ID_TOP_REGS].bufferID = BUFF_ID_TOP_REGS;
        instObj->bufferObjHolder[BUFF_ID_TOP_REGS].length =
                sizeof(CSL_viss_topRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_TOP_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_TOP_REGS].srcPtr =
                (uint32_t*) &instObj->regData.topRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_TOP_REGS].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.topRegs;

        /* BUFF_ID_LSE_REGS */
        /* initialize buffer object for BUFF_ID_LSE_REGS */
        instObj->bufferObjHolder[BUFF_ID_LSE_REGS].bufferID = BUFF_ID_LSE_REGS;
        instObj->bufferObjHolder[BUFF_ID_LSE_REGS].length = sizeof(CSL_lseRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_LSE_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_LSE_REGS].srcPtr =
                (uint32_t*) &instObj->regData.lseRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_LSE_REGS].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.lseRegs;

        /* BUFF_ID_HTS_HWA0_REGS */
        /* initialize buffer object for BUFF_ID_HTS_HWA0_REGS */
        instObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].bufferID =
                BUFF_ID_HTS_HWA0_REGS;
        instObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].length =
                (BUFF_OBJ_HTS_HWA0_REG_SIZE * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].isModified = false;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].srcPtr =
                (uint32_t*) ((uint32_t*) &instObj->regData.htsRegs
                        + (CSL_HTS_HWA0_SCHEDULER_CONTROL / 4));
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].dstPtr =
                (uint32_t*) ((uint32_t*) hObj->buffRegAddrs.htsRegs
                        + (CSL_HTS_HWA0_SCHEDULER_CONTROL / 4));

        /* BUFF_ID_HTS_DMA_0_4_REGS */
        /* initialize buffer object for BUFF_ID_HTS_DMA_0_4_REGS */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].bufferID =
                BUFF_ID_HTS_DMA_0_4_REGS;
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].length =
                (BUFF_OBJ_HTS_DMA_0_4_REG_SIZE * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].isModified = false;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].srcPtr =
                (uint32_t*) ((uint32_t*) &instObj->regData.htsRegs
                        + (CSL_HTS_DMA0_SCHEDULER_CONTROL / 4));
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].dstPtr =
                (uint32_t*) ((uint32_t*) hObj->buffRegAddrs.htsRegs
                        + (CSL_HTS_DMA0_SCHEDULER_CONTROL / 4));

        /* BUFF_ID_HTS_DMA_240_245_REGS */
        /* initialize buffer object for BUFF_ID_HTS_DMA_240_245_REGS */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].bufferID =
                BUFF_ID_HTS_DMA_240_245_REGS;
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].length =
                (BUFF_OBJ_HTS_DMA240_245_REG_SIZE * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].isModified =
                false;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].srcPtr =
                (uint32_t*) ((uint32_t*) &instObj->regData.htsRegs
                        + (CSL_HTS_DMA240_SCHEDULER_CONTROL / 4));
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].dstPtr =
                (uint32_t*) ((uint32_t*) hObj->buffRegAddrs.htsRegs
                        + (CSL_HTS_DMA240_SCHEDULER_CONTROL / 4));

        /* BUFF_ID_RFE_REGS */
        /* initialize buffer object for BUFF_ID_RFE_REGS */
        instObj->bufferObjHolder[BUFF_ID_RFE_REGS].bufferID = BUFF_ID_RFE_REGS;
        instObj->bufferObjHolder[BUFF_ID_RFE_REGS].length =
                sizeof(CSL_rawfe_cfgRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_RFE_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_RFE_REGS].srcPtr =
                (uint32_t*) &instObj->regData.rfeRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_RFE_REGS].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.rfeRegs;

        /* BUFF_ID_DPC_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_DPC_LUT_ADDR */
        instObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].bufferID =
                BUFF_ID_DPC_LUT_ADDR;
        instObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].length =
                (RFE_DPC_LUT_SIZE * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].srcPtr =
                (uint32_t*) &instObj->regData.dpcLutData;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_DPC_LUT_ADDR].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.dpcLutAddr;

        /* BUFF_ID_LSC_SUT_ADDR */
        /* initialize buffer object for BUFF_ID_LSC_SUT_ADDR */
        instObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].bufferID =
                BUFF_ID_LSC_SUT_ADDR;
        instObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].length =
                (RFE_LSC_TBL_SIZE * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].srcPtr =
                (uint32_t*) &instObj->regData.lscLutData;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_LSC_SUT_ADDR].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.lscLutAddr;

        /* BUFF_ID_DECOMP_LUT1_ADDR */
        /* initialize buffer object for BUFF_ID_DECOMP_LUT1_ADDR */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].bufferID =
                BUFF_ID_DECOMP_LUT1_ADDR;
        /* two LUT's will be written to one 32 bit register */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].length =
                (((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].srcPtr =
                (uint32_t*) &instObj->regData.decompLut1Data;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT1_ADDR].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.decompLut1Addr;

        /* BUFF_ID_DECOMP_LUT2_ADDR */
        /* initialize buffer object for BUFF_ID_DECOMP_LUT2_ADDR */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].bufferID =
                BUFF_ID_DECOMP_LUT2_ADDR;
        /* two LUT's will be written to one 32 bit register */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].length =
                (((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].srcPtr =
                (uint32_t*) &instObj->regData.decompLut2Data;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT2_ADDR].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.decompLut2Addr;

        /* BUFF_ID_DECOMP_LUT3_ADDR */
        /* initialize buffer object for BUFF_ID_DECOMP_LUT3_ADDR */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].bufferID =
                BUFF_ID_DECOMP_LUT3_ADDR;
        /* two LUT's will be written to one 32 bit register */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].length =
                (((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].srcPtr =
                (uint32_t*) &instObj->regData.decompLut3Data;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_DECOMP_LUT3_ADDR].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.decompLut3Addr;

        /* BUFF_ID_COMP_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_COMP_LUT_ADDR */
        instObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].bufferID =
                BUFF_ID_COMP_LUT_ADDR;
        /* two LUT's will be written to one 32 bit register */
        instObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].length =
                (((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].srcPtr =
                (uint32_t*) &instObj->regData.compLutData;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_COMP_LUT_ADDR].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.compLutAddr;

        /* BUFF_ID_H3A_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_H3A_LUT_ADDR */
        instObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].bufferID =
                BUFF_ID_H3A_LUT_ADDR;
        instObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].length =
                (((RFE_H3A_COMP_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].srcPtr =
                (uint32_t*) &instObj->regData.h3aLutData;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_H3A_LUT_ADDR].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.h3aLutAddr;

        /* BUFF_ID_H3A_REGS */
        /* initialize buffer object for BUFF_ID_H3A_REGS */
        instObj->bufferObjHolder[BUFF_ID_H3A_REGS].bufferID = BUFF_ID_H3A_REGS;
        instObj->bufferObjHolder[BUFF_ID_H3A_REGS].length =
                sizeof(CSL_rawfe_h3a_cfgRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_H3A_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_H3A_REGS].srcPtr =
                (uint32_t*) &instObj->regData.h3aRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_H3A_REGS].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.h3aRegs;

        /* BUFF_ID_GLBCE_REGS */
        /* initialize buffer object for BUFF_ID_GLBCE_REGS */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].bufferID =
                BUFF_ID_GLBCE_REGS;
        instObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].length =
                sizeof(CSL_glbceRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].srcPtr =
                (uint32_t*) &instObj->regData.glbceRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_REGS].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.glbceRegs;

        /* BUFF_ID_GLBCE_STATS_MEM */
        /* initialize buffer object for BUFF_ID_GLBCE_STATS_MEM */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_STATS_MEM].bufferID =
                BUFF_ID_GLBCE_STATS_MEM;
        instObj->bufferObjHolder[BUFF_ID_GLBCE_STATS_MEM].length =
                sizeof(CSL_glbce_statmemRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_STATS_MEM].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_STATS_MEM].srcPtr =
                (uint32_t*) &instObj->regData.glbceStatsMem;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_GLBCE_STATS_MEM].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.glbceStatsMem;

        /* BUFF_ID_NSF4_REGS */
        /* initialize buffer object for BUFF_ID_NSF4_REGS */
        instObj->bufferObjHolder[BUFF_ID_NSF4_REGS].bufferID =
                BUFF_ID_NSF4_REGS;
        instObj->bufferObjHolder[BUFF_ID_NSF4_REGS].length =
                sizeof(CSL_nsf4vRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_NSF4_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_NSF4_REGS].srcPtr =
                (uint32_t*) &instObj->regData.nsf4Regs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_NSF4_REGS].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.nsf4Regs;

        /* BUFF_ID_CFA_REGS */
        /* initialize buffer object for BUFF_ID_CFA_REGS */
        instObj->bufferObjHolder[BUFF_ID_CFA_REGS].bufferID = BUFF_ID_CFA_REGS;
        instObj->bufferObjHolder[BUFF_ID_CFA_REGS].length =
                sizeof(CSL_flexcfaRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_CFA_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_CFA_REGS].srcPtr =
                (uint32_t*) &instObj->regData.cfaRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_CFA_REGS].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.cfaRegs;

        /* BUFF_ID_FCP_REGS */
        /* initialize buffer object for BUFF_ID_FCP_REGS */
        instObj->bufferObjHolder[BUFF_ID_FCP_REGS].bufferID = BUFF_ID_FCP_REGS;
        instObj->bufferObjHolder[BUFF_ID_FCP_REGS].length =
                sizeof(CSL_flexcc_cfgRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_FCP_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_FCP_REGS].srcPtr =
                (uint32_t*) &instObj->regData.fcpRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_FCP_REGS].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.fcpRegs;

        /* BUFF_ID_GAMMA_LUT1_ADDR */
        /* initialize buffer object for BUFF_ID_GAMMA_LUT1_ADDR */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].bufferID =
                BUFF_ID_GAMMA_LUT1_ADDR;
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].length =
                (((FCP_GAMMA_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].srcPtr =
                (uint32_t*) &instObj->regData.gammaLut1Data;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT1_ADDR].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.gammaLut1Addr;

        /* BUFF_ID_GAMMA_LUT2_ADDR */
        /* initialize buffer object for BUFF_ID_GAMMA_LUT2_ADDR */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].bufferID =
                BUFF_ID_GAMMA_LUT2_ADDR;
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].length =
                (((FCP_GAMMA_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].srcPtr =
                (uint32_t*) &instObj->regData.gammaLut2Data;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT2_ADDR].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.gammaLut2Addr;

        /* BUFF_ID_GAMMA_LUT3_ADDR */
        /* initialize buffer object for BUFF_ID_GAMMA_LUT3_ADDR */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].bufferID =
                BUFF_ID_GAMMA_LUT3_ADDR;
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].length =
                (((FCP_GAMMA_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].srcPtr =
                (uint32_t*) &instObj->regData.gammaLut3Data;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_GAMMA_LUT3_ADDR].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.gammaLut3Addr;

        /* BUFF_ID_Y8C8_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_Y8C8_LUT_ADDR */
        instObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].bufferID =
                BUFF_ID_Y8C8_LUT_ADDR;
        instObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].length =
                (((FCP_RGB_YUVSAT_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].srcPtr =
                (uint32_t*) &instObj->regData.y8C8LutData;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_Y8C8_LUT_ADDR].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.y8C8LutAddr;

        /* BUFF_ID_C8G8_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_C8G8_LUT_ADDR */
        instObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].bufferID =
                BUFF_ID_C8G8_LUT_ADDR;
        instObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].length =
                (((FCP_RGB_YUVSAT_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].srcPtr =
                (uint32_t*) &instObj->regData.c8G8LutData;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_C8G8_LUT_ADDR].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.c8G8LutAddr;

        /* BUFF_ID_S8B8_LUT_ADDR */
        /* initialize buffer object for BUFF_ID_S8B8_LUT_ADDR */
        instObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].bufferID =
                BUFF_ID_S8B8_LUT_ADDR;
        instObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].length =
                (((FCP_RGB_YUVSAT_LUT_SIZE / 2) + 1) * 4);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].srcPtr =
                (uint32_t*) &instObj->regData.s8B8LutData;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_S8B8_LUT_ADDR].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.s8B8LutAddr;

        /* BUFF_ID_EE_REGS */
        /* initialize buffer object for BUFF_ID_EE_REGS */
        instObj->bufferObjHolder[BUFF_ID_EE_REGS].bufferID = BUFF_ID_EE_REGS;
        instObj->bufferObjHolder[BUFF_ID_EE_REGS].length =
                sizeof(CSL_flexeeRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_EE_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_EE_REGS].srcPtr =
                (uint32_t*) &instObj->regData.eeRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_EE_REGS].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.eeRegs;

        /* BUFF_ID_HIST_REGS */
        /* initialize buffer object for BUFF_ID_HIST_REGS */
        instObj->bufferObjHolder[BUFF_ID_HIST_REGS].bufferID =
                BUFF_ID_HIST_REGS;
        instObj->bufferObjHolder[BUFF_ID_HIST_REGS].length =
                sizeof(CSL_flexcc_histRegs);
        /* enable when modified, disable after creating the TR */
        instObj->bufferObjHolder[BUFF_ID_HIST_REGS].isUsed = true;
        /* This will be the buffer pointer */
        instObj->bufferObjHolder[BUFF_ID_HIST_REGS].srcPtr =
                (uint32_t*) &instObj->regData.histRegs;
        /* This will be register address */
        instObj->bufferObjHolder[BUFF_ID_HIST_REGS].dstPtr =
                (uint32_t*) hObj->buffRegAddrs.histRegs;
    }
}

void Vhwa_m2mVissResetModifiedFlag(Vhwa_M2mVissHandleObj *hObj)
{
    int32_t buffObjectCnt = 0;
    for (buffObjectCnt = 0; buffObjectCnt < BUFF_ID_MAXBUFID; buffObjectCnt++)
    {
        /*
         * Disable all the buffer object after creating the TR.
         * This would be helpful to enable only used buffer object
         * and create TR accordingly.
         */
        hObj->bufferObjHolder[buffObjectCnt].isModified = false;
    }
}

int32_t Vhwa_m2mVissPopConfigRings(Vhwa_M2mVissInstObj *instObj)
{
    int32_t status = FVID2_SOK;
    uint64_t ringPopVal = 0;

    /* Check for Null pointer */
    GT_assert(VhwaVissTrace, (NULL != instObj));

    status = Udma_ringDequeueRaw(instObj->configCqRingHndl, &ringPopVal);
    if (UDMA_SOK != status)
    {
        GT_1trace(VhwaVissTrace, GT_ERR,
                "UDMA Failed to pop Ring for in ch%d!!\n", chId);
    }
    /* TODO check for the DMA completion flag */

    return (status);
}

void Vhwa_m2mVissUpdateModifiedBufObj(Vhwa_M2mVissHandleObj *hObj)
{
    uint32_t buffObjectCnt = 0;
    /* if buffer object is Modified, isUsed also set to true */
    for (buffObjectCnt = 0; buffObjectCnt < BUFF_ID_MAXBUFID; buffObjectCnt++)
    {
        if (true == hObj->bufferObjHolder[buffObjectCnt].isModified)
        {
            if (false == hObj->bufferObjHolder[buffObjectCnt].isUsed)
            {
                hObj->bufferObjHolder[buffObjectCnt].isUsed = true;
            }
        }
    }
}

uint32_t Vhwa_m2mVissCalcNumOfTr(Vhwa_M2mVissHandleObj *hObj)
{
    uint32_t numOfTr = 0;
    uint32_t buffObjectCnt = 0;

    /* Vhwa_m2mVissUpdateUsedBufObj should be executed before this function */
    if (true == hObj->sameAsPrevHandle)
    {
        /* Same handle so update only modified buffer object */
        for (buffObjectCnt = 0; buffObjectCnt < BUFF_ID_MAXBUFID;
                buffObjectCnt++)
        {
            if (true == hObj->bufferObjHolder[buffObjectCnt].isModified)
            {
                numOfTr++;
            }
        }
    }
    else
    {
        /* different handle so update all used buffer objects */
        for (buffObjectCnt = 0; buffObjectCnt < BUFF_ID_MAXBUFID;
                buffObjectCnt++)
        {
            if (true == hObj->bufferObjHolder[buffObjectCnt].isUsed)
            {
                numOfTr++;
            }
        }
    }

    return numOfTr;
}

int32_t Vhwa_m2mVissSubmitConfUDMABuf(Vhwa_M2mVissInstObj *instObj,
        Vhwa_M2mVissHandleObj *hObj)
{
    int32_t status = FVID2_SOK;

    /* Update used buffer object from dirty buffer objects */
    Vhwa_m2mVissUpdateModifiedBufObj(hObj);

    hObj->numOfConfigTrSubmitted = Vhwa_m2mVissCalcNumOfTr(hObj);

    /**
     * Check if any TR needs to be submitted.
     */
    if (hObj->numOfConfigTrSubmitted > 0)
    {
        /* cache write back application config buffer, after modification */
        CacheP_wb(hObj->appBufInfo.bufferPtr, hObj->appBufInfo.length);

        Vhwa_m2mVissSetConfigTRDesc(instObj, &hObj->bufferObjHolder[0],
                hObj->sameAsPrevHandle, hObj->numOfConfigTrSubmitted);

        /* submit linked list of UDMA TR's to ring queue */
        status = Vhwa_m2mVissSubmitConfigUDMARing(instObj,
                instObj->configTxTrMem);
        if (FVID2_SOK != status)
        {
            status = FVID2_EFAIL;
            GT_0trace(VhwaVissTrace, GT_ERR,
                    "Vhwa_m2mVissSubmitConfigUDMARing failed!!\n");
        }
    }

    return status;
}

int32_t Vhwa_m2mVissCheckConfUDMAComp(Vhwa_M2mVissInstObj *instObj,
        Vhwa_M2mVissHandleObj *hObj)
{
    int32_t status = FVID2_SOK;
    SemaphoreP_Status semStatus;

    if (hObj->numOfConfigTrSubmitted > 0)
    {
        if (FVID2_SOK == status)
        {
            semStatus = SemaphoreP_pend(instObj->configCqEventSemaphore,
            SemaphoreP_WAIT_FOREVER);
            if (SemaphoreP_OK == semStatus)
            {
                status = FVID2_SOK;
            }
            else
            {
                status = FVID2_EFAIL;
            }
        }

        if (FVID2_SOK == status)
        {
            /*
             * semaphore post informs transfer complete,
             * check the completion queue
             */
            status = Vhwa_m2mVissPopConfigRings(instObj);
            if (FVID2_SOK != status)
            {
                status = FVID2_EFAIL;
            }
        }

        /* Cache invalidate, before modifying for next iteration */
        CacheP_Inv(hObj->appBufInfo.bufferPtr,
                (int32_t) hObj->appBufInfo.length);
        /*
         * After submitting all the TR's disable all the buffer object,
         * isModified Flag.
         */
        Vhwa_m2mVissResetModifiedFlag(hObj);
        /* set numOfConfigTrSubmitted to zero */
        hObj->numOfConfigTrSubmitted = 0;
    }

    return status;
}

int32_t Vhwa_m2mVissSubmitSaveRestoreConfigUDMA(Vhwa_M2mVissInstObj *instObj,
        Vhwa_M2mVissHandleObj *hObj, bool saveConfig)
{
    int32_t status = FVID2_SOK;
    /* number of TR to be prepared */
    uint32_t numOfTr = 0;
    SemaphoreP_Status semStatus;
    /* should be always false in Vhwa_m2mVissSubmitSaveRestoreConfigUDMA */
    bool sameAsPrevHandle = false;

    /* we need to save or restore all the buffer object */
    numOfTr = BUFF_ID_MAXBUFID;

    if (true == saveConfig)
    {
        /* cache write back reg data */
        CacheP_wb(&instObj->regData, sizeof(Vhwa_M2mVissSaveRegData));
    }
    else
    {
        if (NULL != hObj)
        {
            /* cache write back application config buffer, after modification */
            CacheP_wb(hObj->appBufInfo.bufferPtr, hObj->appBufInfo.length);
            /* cache write back reg data */
            CacheP_wb(&instObj->regData, sizeof(Vhwa_M2mVissSaveRegData));
        }
        else
        {
            status = FVID2_EFAIL;
        }
    }

    Vhwa_m2mVissSetConfigTRDesc(instObj, &instObj->bufferObjHolder[0],
            sameAsPrevHandle, numOfTr);

    /* submit linked list of UDMA TR's to ring queue */
    if (FVID2_SOK == status)
    {
        status = Vhwa_m2mVissSubmitConfigUDMARing(instObj,
                instObj->configTxTrMem);
        if (FVID2_SOK != status)
        {
            status = FVID2_EFAIL;
            GT_0trace(VhwaVissTrace, GT_ERR,
                    "Vhwa_m2mVissSubmitConfigUDMARing failed!!\n");
        }
    }

    if (FVID2_SOK == status)
    {
        semStatus = SemaphoreP_pend(instObj->configCqEventSemaphore,
        SemaphoreP_WAIT_FOREVER);
        if (SemaphoreP_OK == semStatus)
        {
            status = FVID2_SOK;
        }
        else
        {
            status = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == status)
    {
        /*
         * semaphore post informs transfer complete,
         * check the completion queue
         */
        status = Vhwa_m2mVissPopConfigRings(instObj);
        if (FVID2_SOK != status)
        {
            status = FVID2_EFAIL;
        }
    }

    if (true == saveConfig)
    {
        /* Cache invalidate so that CPU reads new data */
        CacheP_Inv(&instObj->regData,
                (int32_t) sizeof(Vhwa_M2mVissSaveRegData));
    }
    else
    {
        if (NULL != hObj)
        {
            /* Cache invalidate so that CPU reads new data */
            CacheP_Inv(&instObj->regData,
                    (int32_t) sizeof(Vhwa_M2mVissSaveRegData));
            /* Cache invalidate, before modifying for next iteration */
            CacheP_Inv(hObj->appBufInfo.bufferPtr,
                    (int32_t) hObj->appBufInfo.length);
        }
        else
        {
            status = FVID2_EFAIL;
        }
    }

    return status;
}

uint32_t vhwa_m2mCalAppBufSize(void)
{
    /* buffer size to return */
    uint32_t bufferSize = 0;

    /* BUFF_ID_TOP_REGS */
    bufferSize = (((sizeof(CSL_viss_topRegs)) / (sizeof(uint32_t)))
                /* BUFF_ID_LSE_REGS */
                + ((sizeof(CSL_lseRegs)) / (sizeof(uint32_t)))
                /* BUFF_ID_HTS_HWA0_REGS, BUFF_ID_HTS_DMA_0_4_REGS, BUFF_ID_HTS_DMA_240_245_REGS */
                + ((sizeof(CSL_htsRegs)) / (sizeof(uint32_t)))
                /* BUFF_ID_RFE_REGS */
                + ((sizeof(CSL_rawfe_cfgRegs)) / (sizeof(uint32_t)))
                /* BUFF_ID_DPC_LUT_ADDR */
                + RFE_DPC_LUT_SIZE
                /* BUFF_ID_LSC_SUT_ADDR */
                + RFE_LSC_TBL_SIZE
                /* BUFF_ID_DECOMP_LUT1_ADDR */
                + ((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1)
                /* BUFF_ID_DECOMP_LUT2_ADDR */
                + ((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1)
                /* BUFF_ID_DECOMP_LUT3_ADDR */
                + ((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1)
                /* BUFF_ID_COMP_LUT_ADDR */
                + ((RFE_COMP_DECOMP_LUT_SIZE / 2) + 1)
                /* BUFF_ID_H3A_LUT_ADDR */
                + ((RFE_H3A_COMP_LUT_SIZE / 2) + 1)
                /* BUFF_ID_H3A_REGS */
                + ((sizeof(CSL_rawfe_h3a_cfgRegs)) / (sizeof(uint32_t)))
                /* BUFF_ID_GLBCE_REGS */
                + ((sizeof(CSL_glbceRegs)) / (sizeof(uint32_t)))
                /* BUFF_ID_GLBCE_STATS_MEM */
                + ((sizeof(CSL_glbce_statmemRegs)) / (sizeof(uint32_t)))
                /* BUFF_ID_NSF4_REGS */
                + ((sizeof(CSL_nsf4vRegs)) / (sizeof(uint32_t)))
                /* BUFF_ID_CFA_REGS */
                + ((sizeof(CSL_flexcfaRegs)) / (sizeof(uint32_t)))
                /* BUFF_ID_FCP_REGS */
                + ((sizeof(CSL_flexcc_cfgRegs)) / (sizeof(uint32_t)))
                /* BUFF_ID_GAMMA_LUT1_ADDR */
                + ((FCP_GAMMA_LUT_SIZE / 2) + 1)
                /* BUFF_ID_GAMMA_LUT2_ADDR */
                + ((FCP_GAMMA_LUT_SIZE / 2) + 1)
                /* BUFF_ID_GAMMA_LUT3_ADDR */
                + ((FCP_GAMMA_LUT_SIZE / 2) + 1)
                /* BUFF_ID_Y8C8_LUT_ADDR */
                + ((FCP_RGB_YUVSAT_LUT_SIZE / 2) + 1)
                /* BUFF_ID_C8G8_LUT_ADDR */
                + ((FCP_RGB_YUVSAT_LUT_SIZE / 2) + 1)
                /* BUFF_ID_S8B8_LUT_ADDR */
                + ((FCP_RGB_YUVSAT_LUT_SIZE / 2) + 1)
                /* BUFF_ID_EE_REGS */
                + ((sizeof(CSL_flexeeRegs)) / (sizeof(uint32_t)))
                /* BUFF_ID_HIST_REGS */
                + ((sizeof(CSL_flexcc_histRegs)) / (sizeof(uint32_t))));
    /*
     * size calculated will by number of registers to write,
     * each register size is 4 bytes.
     */
    bufferSize = bufferSize * sizeof(uint32_t);
    /* return the buffer size in number of bytes */
    return bufferSize;
}

/*
 * This is the main function which handles initialization for enabling heterogeneous support.
 */
int32_t Vhwa_m2mVissProcessAppBuf(Vhwa_M2mVissInstObj *instObj,
        Vhwa_M2mVissHandleObj *hObj)
{
    int32_t status = FVID2_SOK;
    /* False since we are restoring */
    bool saveConfig = false;

    /* perform operation only if appBuffInitDone is false */
    if (false == hObj->appBuffInitDone)
    {
        /* memset the buffer pointer to zero */
        Fvid2Utils_memset(hObj->appBufInfo.bufferPtr, 0x0,
                hObj->appBufInfo.length);

        /* Partition the buffer address for the modules */
        Vhwa_m2mVissInitBuffAddresses(hObj, hObj->appBufInfo.bufferPtr);

        /* Initialize buffer object for the handle */
        Vhwa_m2mVissInitBuffObject(instObj, hObj);

        /* Initialize instance buffer object to restore initial register data */
        Vhwa_m2mVissInitRestoreBuffObject(instObj, hObj);

        /* perform UDMA transfer to restore initial register data on handle buffer */
        status = Vhwa_m2mVissSubmitSaveRestoreConfigUDMA(instObj, hObj,
                saveConfig);
        if (FVID2_SOK != status)
        {
            GT_0trace(VhwaVissTrace, GT_ERR,
                    "UDMA config mem alloc failed !!\n");
        }
        /* after initialization of buffer object, set valid appBuffInitDone to true */
        hObj->appBuffInitDone = true;
    }

    return status;
}
#if 0
int32_t Vhwa_memVissSetVissStartModules(Vhwa_M2mVissInstObj *instObj,
    const Vhwa_M2mVissHandleObj *hObj)
{
    int32_t status = FVID2_EBADARGS;

    if ((NULL != instObj) && (NULL != hObj))
    {
        status = FVID2_SOK;
        if ((uint32_t)TRUE == hObj->vsPrms.enableGlbce)
        {
            /* Since GLBCE is in one-shot mode, it requires to be started
             * for every frame */
            status = CSL_glbceStart(instObj->regAddrs.glbceRegs);
        }
    }

    return (status);
}
#endif

/* ========================================================================== */
/*                           Local Functions                                  */
/* ========================================================================== */

