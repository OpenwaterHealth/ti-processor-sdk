/*
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
 *  \ingroup  DRV_VHWA_MODULE
 *  \defgroup DRV_MSC_MODULE MSC Module
 *            MSC Module
 *
 */
/**
 *  \ingroup  DRV_MSC_MODULE
 *  \defgroup DRV_MSC_MODULE_INTERFACE MSC Interface
 *            Interface file for MSC M2M FVID2 driver
 *
 *  @{
 */
 /**
 *  \file vhwa_m2mMsc.h
 *
 *  \brief  Defines the structures / control operations that could be used to
 *              configure / control MSC M2M driver
 */

#ifndef VHWA_M2M_MSC_H_
#define VHWA_M2M_MSC_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/drv/vhwa/include/msc_cfg.h>
#include <ti/drv/udma/udma.h>
#include <ti/drv/vhwa/soc/vhwa_soc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/** \brief Minimum supported depth value for read buffer in VHWA memory*/
#define VHWA_M2M_MSC_MIN_RD_BUFFER_DEPTH        (6U)
/** \brief Minimum supported depth value for write buffer in VHWA memory */
#define VHWA_M2M_MSC_MIN_WR_BUFFER_DEPTH        (2U)

/** \brief Maximum possible MSC iteration for a simgle request (Luma + Chroma) */
#define VHWA_M2M_MSC_MAX_COMP                   (2u)

/** \brief Maximum Number of Input buffer */
#define VHWA_M2M_MSC_MAX_INPUT_BUFF             (VHWA_M2M_MSC_MAX_INST)

/** \brief Max Number of handles supported by MSC M2M Driver */
#define VHWA_M2M_MSC_MAX_HANDLES                (8u)

/**
 *  \anchor MSC_Ioctl
 *  \name   MSC IOCTL macros
 *  \brief  Input/Output control MACRO's for MSC driver
 *
 *  @{
 */
/** \brief IOCTL for Setting filter coefficients */
#define VHWA_M2M_IOCTL_MSC_SET_COEFF            \
                                        (VHWA_IOCTL_MSC_IOCTL_BASE)
/** \brief IOCTL for Setting MSC configuration */
#define VHWA_M2M_IOCTL_MSC_SET_PARAMS           \
                                        (VHWA_M2M_IOCTL_MSC_SET_COEFF + 1U)
/** \brief IOCTL to set bandwidth limiter. This IOCTL take pointer to structure
           #Vhwa_HtsLimiter as input */
#define VHWA_M2M_IOCTL_MSC_SET_BW_LIMITER       \
                                        (VHWA_M2M_IOCTL_MSC_SET_PARAMS + 1U)
/** \brief IOCTL to get the IP performance */
#define VHWA_M2M_IOCTL_MSC_GET_PERF             \
                                        (VHWA_M2M_IOCTL_MSC_SET_BW_LIMITER + 1U)
/** \brief IOCTL to get PSA signature for all enabeld outputs */
#define VHWA_M2M_IOCTL_MSC_GET_PSA_SIGN         \
                                        (VHWA_M2M_IOCTL_MSC_GET_PERF + 1U)

/**
 * \brief IOCTL for enabling error events and registering
 *         callbacks for the same.
 */
#define VHWA_M2M_IOCTL_MSC_REGISTER_ERR_CB      \
                                        (VHWA_M2M_IOCTL_MSC_GET_PSA_SIGN + 1U)

/**
 * \brief IOCTL to sync start each module
 *        This IOCTL doesn't configure any register and only enable pipeline
 *        to start processing
 */
#define VHWA_M2M_IOCTL_MSC_SYNC_START      \
                                        (VHWA_M2M_IOCTL_MSC_REGISTER_ERR_CB + 1U)
/** @} */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vhwa_M2mMscParams
 *  \brief Connfiguration parameters of MSC.
 */
typedef struct
{
    uint32_t            loopBack;
    /**< Loopback Mode, Supported only for #VPAC_MSC_INST_ID_1,
         Enabling will loopback data to last scalar output i.e. output 9 */

    uint32_t            enableLineSkip;
    /**< Setting this variable for Octave generation, will offset next input
         line by 2 and ouptput is generated for every other line */

    Fvid2_Format        inFmt;
    /**< Input Frame format, describing input frames storage format
         Following parameters are used from this structure
         pitch = line offset for the input data, used based on the data format
         dataFormat = Input DataFormat
         sfcc = Storage format */
    Fvid2_Format        outFmt[MSC_MAX_OUTPUT];
    /**< Input Frame format, describing input frames storage format
         Following parameters are used from this structure
         pitch = line offset for the input data, used based on the data format
         sfcc = Storage format */
    Msc_Config          mscCfg;
    /**< Noise Filter configuration */
} Vhwa_M2mMscParams;

/**
 *  struct Vhwa_m2mMscCreateArgs
 *  \brief Parameters required to create a new handle
 */
typedef struct
{
    Vhwa_GetTimeStamp  getTimeStamp;
    /**< Function pointer to get timestamp */

    uint32_t enablePsa;
    /**< Flag to enable/Disble MSC PSA Module */
} Vhwa_M2mMscCreatePrms;

/**
 *  struct Vhwa_m2mMscSl2AllocPrms
 *  \brief Init Parameters required to allocate MSC drivere
 */
typedef struct
{
    /** These parameters are used by the driver to allocater SL2 memory, used
     *  all driver handles.
     *  Ensure to allocate max of all handles requirement.
     */
    uint32_t        maxInWidth[VHWA_M2M_MSC_MAX_INPUT_BUFF];
    /**< Maximun width for input image for both instance 0 and 1 */
    uint32_t        inCcsf[VHWA_M2M_MSC_MAX_INPUT_BUFF];
    /**< CCSF for input image for both instance 0 and 1 */
    uint32_t        inBuffDepth[VHWA_M2M_MSC_MAX_INPUT_BUFF];
    /**< Input buffer depth in SL2 for for both instance 0 and 1,
          Minumum configurable depth is 6 */
    uint32_t        maxOutWidth[MSC_MAX_OUTPUT];
    /**< Maximun width image for all 10 output channels */
    uint32_t        outBuffDepth[MSC_MAX_OUTPUT];
    /**< Output buffer depth in Sl2 for all 10 output channels,
         Minimum configurable depth is 2 */
    uint32_t        outCcsf[MSC_MAX_OUTPUT];
    /**< CCSF for all 10 output channels */
} Vhwa_M2mMscSl2AllocPrms;

/**
 *  struct Vhwa_m2mMscInitParams
 *  \brief Init Parameters for MSC M2M Driver
 */
typedef struct
{
    Udma_DrvHandle     drvHandle;
    /**< UDMA driver handle */
    Vhwa_IrqInfo       irqInfo[VHWA_M2M_MSC_MAX_INST];
    /**< IRQ Information */
} Vhwa_M2mMscInitParams;

/**
 *  struct Vhwa_m2mMscPerf
 *  \brief Structure for storing the performance numbers
 */
typedef struct
{
    uint32_t perf[VHWA_M2M_MSC_MAX_COMP];
    /**< Time taken to process image */
    uint32_t swOvhd[VHWA_M2M_MSC_MAX_COMP];
    /**< Software overhead - Time take by drvier for configuration before
         initiating scaling operation */
} Vhwa_M2mMscPerf;

/**
 *  struct Vhwa_m2mMscPsaSign
 *  \brief Structure for storing PSA signatures
 */
typedef struct
{
    uint32_t psaSign[VHWA_M2M_MSC_MAX_COMP][MSC_MAX_OUTPUT];
    /**< LSE PSA signature for all output channels */
} Vhwa_M2mMscPsaSign;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Vhwa_M2mMscInitParams structure init function.
 *
 *  \param  mscInitPrms  Pointer to #Vhwa_M2mMscInitParams structure.
 *
 *  \return None
 */
static inline void Vhwa_m2mMscInitParamsInit(
    Vhwa_M2mMscInitParams *mscInitPrms);

/**
 *  \brief Function to initialize the MSC configuration parameters to default
 *
 *  \param mscCfg   Pointer to a #Vhwa_M2mMscParams containing MSC configuration
 *
 *  \return         None
 */
static inline void Vhwa_m2mMscParamsInit(Vhwa_M2mMscParams *mscCfg);

/**
 *  \brief Function to initialize the MSC create params to default
 *
 *  \param createArgs   Pointer to a #Vhwa_M2mMscCreatePrms, MSC Create Args
 *
 *  \return         None
 */
static inline void Vhwa_M2mMscCreatePrmsInit(Vhwa_M2mMscCreatePrms *createArgs);

/**
 *  \brief MSC initialization function.
 *   This function initializes the MSC hardware and drivers.
 *   This function should be called before calling any driver APIs and
 *   only once.
 *
 *  \param initPrms    Pointer to a #Vhwa_M2mMscInitParams structure
 *                     containing the MSC driver initialization parameters
 *
 *  \return FVID2_SOK if successful, else suitable error code
 */
int32_t Vhwa_m2mMscInit(const Vhwa_M2mMscInitParams *initPrms);

/**
 *  \brief MSC de-initialization function.
 *   This function un-initializes the MSC hardware and drivers.
 *   This function should be called during system shutdown if
 *   Vhwa_m2mMscDeInit() was called by the application.
 *
 *  \return FVID2_SOK if successful, else suitable error code
 */
int32_t Vhwa_m2mMscDeInit(void);

/**
 *  \brief Function to allocate Sl2 memory for input and output buffers.
 *
 *  \param sl2allocPrms Pointer to a #Vhwa_M2mMscSl2AllocPrms structure
 *                      containing the SL2 allocation parameters
 *
 *  \return FVID2_SOK if successful, else suitable error code
 */
int32_t Vhwa_m2mMscAllocSl2(const Vhwa_M2mMscSl2AllocPrms *sl2allocPrms);

/**
 *  \brief Function to free allocated SL2.
 *
 *  \return FVID2_SOK if successful, else suitable error code
 */
int32_t Vhwa_m2mMscFreeSl2(void);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static inline void Vhwa_m2mMscInitParamsInit(Vhwa_M2mMscInitParams *mscInitPrms)
{
    if (NULL != mscInitPrms)
    {
        Msc_getIrqInfo(mscInitPrms->irqInfo);
        mscInitPrms->drvHandle = NULL;
    }
}

static inline void Vhwa_m2mMscParamsInit(Vhwa_M2mMscParams *mscPrms)
{
    if (NULL != mscPrms)
    {
        mscPrms->loopBack = FALSE;
        mscPrms->enableLineSkip = FALSE;
        Msc_ConfigInit(&mscPrms->mscCfg);
    }
}

static inline void Vhwa_M2mMscCreatePrmsInit(Vhwa_M2mMscCreatePrms *createArgs)
{
    if (NULL != createArgs)
    {
        createArgs->getTimeStamp = NULL;
        createArgs->enablePsa = (uint32_t)FALSE;
    }
}

static inline void Vhwa_M2mMscSl2AllocPrmsInit(Vhwa_M2mMscSl2AllocPrms *sl2Prms)
{
    uint32_t cnt;

    if (NULL != sl2Prms)
    {
        for(cnt = 0; cnt < VHWA_M2M_MSC_MAX_INPUT_BUFF; cnt++)
        {
            sl2Prms->maxInWidth[cnt]    = 1920U;
            sl2Prms->inCcsf[cnt]        = FVID2_CCSF_BITS12_UNPACKED16;
            sl2Prms->inBuffDepth[cnt]   = 6U;
        }

        for(cnt = 0; cnt < MSC_MAX_OUTPUT; cnt++)
        {
            sl2Prms->maxOutWidth[cnt]   = 1920U;
            sl2Prms->outCcsf[cnt]       = FVID2_CCSF_BITS12_UNPACKED16;
            sl2Prms->outBuffDepth[cnt]  = 2U;
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif /** VHWA_M2M_MSC_H_ */
 /** @} */
