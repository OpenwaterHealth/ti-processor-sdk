/**************************************************************************
*  Copyright 2021   Neither this file nor any part of it may be used for 
*     any purpose without the written consent of D3 Engineering, LLC.
*     All rights reserved.
*				
*				D 3   E N G I N E E R I N G
*
* File Name   :	app_c7x_kernel.h
* Release     :	1.0
* Author      :	D3 Engineering Technical Staff
* Created     :	09/23/2021
* Revision    :	1.0
* Description :	C++ file for the c7x kernel that implements histogram function
* 
*		
***************************************************************************
* Revision History
* Ver	Date			Author		Description 
* 1.0	09/23/2021		VC		Initial version	
***************************************************************************/

/**
 * \file app_c7x_target_kernel_img_add.c Target Kernel implementation for Phase to RGB conversion function
 *
 *  This file shows a sample implementation of a target kernel function.
 *
 *  To implement a target kernel the below top level interface functions are implemented
 *  - vxTutorialAddTargetKernelPhaseRgb() : Registers target kernel to TIOVX target framework
 *  - vxTutorialRemoveTargetKernelPhaseRgb() : Un-Registers target kernel from TIOVX target framework
 *
 *  When registering a target kernel, the following callback function are implemented and registered with the TIOVX framework
 *  - app_c7x_target_kernel_img_add() : kernel execute/run function
 *  - app_c7x_target_kernel_img_addCreate() : kernel init function
 *  - app_c7x_target_kernel_img_addDelete() : kernel deinit function
 *  - app_c7x_target_kernel_img_addControl(): kernel control function
 *
 *  When working with target kernel
 *  - vxTutorialAddTargetKernelPhaseRgb() MUST be called during TIOVX target framework system init
 *     - This is done by using function tivxRegisterTutorialTargetKernels() in \ref vx_tutorial_target_kernel.c
 *  - vxTutorialRemoveTargetKernelPhaseRgb() MUST be called during TIOVX target framework system deinit
 *     - This is done by using function tivxUnRegisterTutorialTargetKernels() in \ref vx_tutorial_target_kernel.c
 *
 *  When registering a target kernel a unique name MUST be used to register the
 *  kernel on target side and HOST side.
 *
 *  Follow the comments for the different functions in the file to understand how a user/target kernel is implemented.
 */

#include <TI/tivx.h>
#include <TI/j7.h>
#include <TI/tivx_target_kernel.h>
#include "../app_c7x_kernel.h"
#include "VXLIB_histogram_i16u_o32u_cn.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <c7x.h>
#include <iomanip> 
#include <time.h>

// #include <headSensorService.h>
// #include <app_ipc.h>
// #include <app_remote_service.h>

#include <utils/udma/include/app_udma.h>


#define SHOW_PERF_INFO 0

#if SHOW_PERF_INFO
#include <app_perf_stats.h>
#endif

//#define ENABLE_DMA_KERNEL_PRINT_STATS
//#define ENABLE_HISTO_KERNEL_PRINT_STATS

#define EXEC_PIPELINE_STAGE1(x) ((x) & 0x00000001)
#define EXEC_PIPELINE_STAGE2(x) ((x) & 0x00000002)
#define EXEC_PIPELINE_STAGE3(x) ((x) & 0x00000004)
#define DARK_PIXEL_THR_NO_GAIN 127
#define DARK_PIXEL_THR_16_GAIN 255
#define MIN_BRIGHT_INDEX 200
#define MAX_BRIGHT_INDEX 1015 //Keep this to a multiple of 8(-1) for memory alignment
#define MIN_BIN_VAL 100
#define MEASURE_HISTO_KERNEL_PERFORMANCE 0
#if MEASURE_HISTO_KERNEL_PERFORMANCE
    static clock_t timeSpent = 0;
    static clock_t timeSpentHistKernel = 0;
    static int frameCount = 0;
#endif

/**
 * \brief Target kernel handle [static global]
 */
static tivx_target_kernel app_c7x_target_kernel_histo_handle = NULL;

typedef enum {
    DATA_COPY_CPU = 0,
    DATA_COPY_DMA = 1,
    DATA_COPY_DEFAULT = DATA_COPY_CPU

} DataCopyStyle;

typedef struct {

    app_udma_ch_handle_t udmaChHdl;
    app_udma_copy_nd_prms_t tfrPrms;

    vx_uint32 transfer_type;

    vx_uint32 icnt1_next;
    vx_uint32 icnt2_next;
    vx_uint32 icnt3_next;

}DMAObj;

typedef struct {

    vx_int32 ch_num;
    uint32_t enableChanBitFlag;

    DMAObj dmaObjSrc0;
    DMAObj dmaObjSrc1;
    DMAObj dmaObjDst;

    vx_uint32 blkWidth;
    vx_uint32 blkHeight;
    vx_uint32 remHeight;
    vx_uint32 numSets;
    vx_uint32 l2_req_size;
    vx_uint32 l1_req_size;
    vx_uint32 l2_avail_size;
    vx_uint32 l1_avail_size;

    uint8_t *pL2;
    uint32_t l2_heap_id;
    uint8_t *pL1;
    uint32_t l1_heap_id;

    uint32_t numHIST;
    uint32_t numBins;
    uint64_t ref_ts;
    uint64_t ref_tick;

} tivxC7xKernelParams;

static vx_int32 inst_id = 0;

static vx_status dma_create(DMAObj *dmaObj, vx_size transfer_type, vx_uint32 dma_ch)
{
    vx_status status = VX_SUCCESS;

    dmaObj->transfer_type = transfer_type;

    memset(&dmaObj->tfrPrms, 0, sizeof(app_udma_copy_nd_prms_t));

    dmaObj->icnt1_next = 0;
    dmaObj->icnt2_next = 0;
    dmaObj->icnt3_next = 0;

    if(transfer_type == DATA_COPY_DMA)
    {
#ifndef x86_64
        dmaObj->udmaChHdl = appUdmaCopyNDGetHandle(dma_ch);
#endif
    }
    else
    {
        dmaObj->udmaChHdl = NULL;
    }

    return status;
}

static vx_status dma_delete(DMAObj *dmaObj)
{
    vx_status status = VX_SUCCESS;

#ifndef x86_64
    status = appUdmaCopyDelete(dmaObj->udmaChHdl);
#endif

    return status;
}

static vx_status dma_init(DMAObj *dmaObj)
{
    vx_status status = VX_SUCCESS;

    if(dmaObj->transfer_type == DATA_COPY_DMA)
    {
#ifndef x86_64
        appUdmaCopyNDInit(dmaObj->udmaChHdl, &dmaObj->tfrPrms);
#endif
    }
    else
    {
        dmaObj->icnt1_next = 0;
        dmaObj->icnt2_next = 0;
        dmaObj->icnt3_next = 0;
    }

    return status;
}

static vx_status dma_deinit(DMAObj *dmaObj)
{
    vx_status status = VX_SUCCESS;

    if(dmaObj->transfer_type == DATA_COPY_DMA)
    {
#ifndef x86_64
        appUdmaCopyNDDeinit(dmaObj->udmaChHdl);
#endif
    }
    else
    {
        dmaObj->icnt1_next = 0;
        dmaObj->icnt2_next = 0;
        dmaObj->icnt3_next = 0;
    }
    return status;
}

static vx_status dma_transfer_trigger(DMAObj *dmaObj)
{
    vx_status status = VX_SUCCESS;

    if(dmaObj->transfer_type == DATA_COPY_DMA)
    {
#ifndef x86_64
        appUdmaCopyNDTrigger(dmaObj->udmaChHdl);
#endif
    }
    else
    {
        app_udma_copy_nd_prms_t *tfrPrms;
        vx_uint32 icnt1, icnt2, icnt3;

        tfrPrms = (app_udma_copy_nd_prms_t *)&dmaObj->tfrPrms;

        /* This is for case where for every trigger ICNT0 * ICNT1 bytes get transferred */
        icnt3 = dmaObj->icnt3_next;
        icnt2 = dmaObj->icnt2_next;
        icnt1 = dmaObj->icnt1_next;

        vx_uint8 *pSrcNext = (vx_uint8 *)(tfrPrms->src_addr + (icnt3 * tfrPrms->dim3) + (icnt2 * tfrPrms->dim2));
        vx_uint8 *pDstNext = (vx_uint8 *)(tfrPrms->dest_addr + (icnt3 * tfrPrms->ddim3) + (icnt2 * tfrPrms->ddim2));

        for(icnt1 = 0; icnt1 < tfrPrms->icnt1; icnt1++)
        {
            memcpy(pDstNext, pSrcNext, tfrPrms->icnt0);

            pSrcNext += tfrPrms->dim1;
            pDstNext += tfrPrms->ddim1;
        }

        icnt2++;

        if(icnt2 == tfrPrms->icnt2)
        {
            icnt2 = 0;
            icnt3++;
        }

        dmaObj->icnt3_next = icnt3;
        dmaObj->icnt2_next = icnt2;
    }

    return status;
}

static vx_status dma_transfer_wait(DMAObj *dmaObj)
{
    vx_status status = VX_SUCCESS;

    if(dmaObj->transfer_type == DATA_COPY_DMA)
    {
#ifndef x86_64
        appUdmaCopyNDWait(dmaObj->udmaChHdl);
#endif
    }

    return status;
}



static vx_status histo_dma_setup_input_image(tivxC7xKernelParams *prms,
                             DMAObj *dmaObj,
                             vx_uint16 *pL2,
                             tivx_obj_desc_raw_image_t *image_desc,
                             void *desc_target_ptr)
{
    vx_status status = VX_SUCCESS;
    vx_uint32 rawImageBitDepth, sizeOfPix;
    app_udma_copy_nd_prms_t *tfrPrms = (app_udma_copy_nd_prms_t *)&dmaObj->tfrPrms;
    vx_imagepatch_addressing_t *pImage = (vx_imagepatch_addressing_t *)&image_desc->imagepatch_addr[0];
    vx_uint32 img_width  = pImage->dim_x;
    vx_int32  img_stride = pImage->stride_y;
    vx_uint8 *pDDR = NULL;

    rawImageBitDepth = image_desc->params.format[0].msb + 1;
    if (rawImageBitDepth <= 8)
    {
        sizeOfPix = 1;
    }
    else if (rawImageBitDepth <= 16)
    {
        sizeOfPix = 2;
    }
    else
    {
        printf("Raw image bit depth exceeds 16 bits !\n");
        status = VX_FAILURE;
    }

    pDDR = (vx_uint8 *)desc_target_ptr;

    tfrPrms->copy_mode    = 2;
    tfrPrms->eltype       = sizeof(uint8_t);

    tfrPrms->dest_addr    = (uintptr_t)pL2;
    tfrPrms->src_addr     = (uint64_t)pDDR;

    tfrPrms->icnt0        = img_width*sizeOfPix;
    tfrPrms->icnt1        = prms->blkHeight;
    tfrPrms->icnt2        = 2;
    tfrPrms->icnt3        = prms->numSets / 2;
    tfrPrms->dim1         = img_stride;
    tfrPrms->dim2         = (prms->blkHeight * img_stride);
    tfrPrms->dim3         = (prms->blkHeight * img_stride * 2);

    tfrPrms->dicnt0       = tfrPrms->icnt0;
    tfrPrms->dicnt1       = tfrPrms->icnt1;
    tfrPrms->dicnt2       = 2; /* Ping-pong */
    tfrPrms->dicnt3       = prms->numSets / 2;
    tfrPrms->ddim1        = prms->blkWidth*sizeOfPix;
    tfrPrms->ddim2        = (prms->blkHeight * prms->blkWidth)*sizeOfPix;
    tfrPrms->ddim3        = 0;

    dma_init(dmaObj);

    return status;
}

static vx_status histo_dma_setup_output_image(tivxC7xKernelParams *prms,
                             DMAObj *dmaObj,
                             vx_uint32 *pL1,
                             tivx_obj_desc_distribution_t *dst_desc,
                             void *desc_target_ptr)
{
    vx_status status = VX_SUCCESS;
    vx_uint32 numBins = dst_desc->num_bins;
    vx_uint32 sizeOfBin = 4;
    app_udma_copy_nd_prms_t *tfrPrms = (app_udma_copy_nd_prms_t *)&dmaObj->tfrPrms;
    vx_uint8 *pDDR = NULL;

    pDDR = (vx_uint8 *)desc_target_ptr;
    // D3: We initialize the DMA for one-shot transfer triggered at the end of the frame only
    // to write out the histogram
    tfrPrms->copy_mode    = 2;
    tfrPrms->eltype       = sizeof(uint8_t);

    tfrPrms->dest_addr    = (uint64_t)pDDR;
    tfrPrms->src_addr     = (uintptr_t)pL1;

    tfrPrms->icnt0        = numBins*sizeOfBin;
    tfrPrms->icnt1        = 1;
    tfrPrms->icnt2        = 1;
    tfrPrms->icnt3        = 1;
    tfrPrms->dim1         = numBins*sizeOfBin;
    tfrPrms->dim2         = numBins*sizeOfBin;
    tfrPrms->dim3         = 0;

    tfrPrms->dicnt0       = tfrPrms->icnt0;
    tfrPrms->dicnt1       = tfrPrms->icnt1;
    tfrPrms->dicnt2       = tfrPrms->icnt2; /* No Ping-pong */
    tfrPrms->dicnt3       = tfrPrms->icnt3;
    tfrPrms->ddim1        = numBins*sizeOfBin;
    tfrPrms->ddim2        = numBins*sizeOfBin;
    tfrPrms->ddim3        = 0;

    dma_init(dmaObj);

    return status;
}


static vx_status histo_dma_setup(tivxC7xKernelParams *prms,
                             tivx_obj_desc_raw_image_t *src_desc0,
                             tivx_obj_desc_distribution_t *dst_desc,
                             void *src_desc0_target_ptr,
                             void *dst_desc_target_ptr)
{
    vx_status status = VX_SUCCESS;
    vx_uint16 *pSrc0L2[2] = {NULL};
    vx_uint32 *pDst = NULL;

    vx_uint32 numBins = dst_desc->num_bins;

    /* Setting first image pointer to the base of L2 then the second buffer
       directly following the pointer location.  Repeating this pattern for
       all images */
    pSrc0L2[0] = (vx_uint16 *)prms->pL2;
    pSrc0L2[1] = (vx_uint16 *)pSrc0L2[0] + (prms->blkWidth * prms->blkHeight);

    if (prms->numHIST==1)
    {
        pDst  = (vx_uint32 *)prms->pL1;
    }
    else
    {
        pDst  = (vx_uint32 *)((vx_uint16 *)pSrc0L2[1] + (prms->blkWidth * prms->blkHeight));
    }

    /* Setting up src0 */
    status = histo_dma_setup_input_image(prms, &prms->dmaObjSrc0, pSrc0L2[0], src_desc0, src_desc0_target_ptr);

    if (VX_SUCCESS == status)
    {
        /* Setting up dst */
        status = histo_dma_setup_output_image(prms, &prms->dmaObjDst, pDst, dst_desc, dst_desc_target_ptr);
    }
    else
    {
        printf("dma setup for src0 failed\n");
    }

    return status;
}

static vx_status histo_pipeline_blocks(tivxC7xKernelParams *prms,
                             tivx_obj_desc_raw_image_t *src_desc0,
                             //tivx_obj_desc_image_t *src_desc1,
                             tivx_obj_desc_distribution_t *dst_desc,
                             void *src_desc0_target_ptr,
                             //void *src_desc1_target_ptr,
                             void *dst_desc_target_ptr,
                             vx_uint32 *pMean,
                             vx_uint32 *pSd,
                             vx_uint32 skipRows = 20)
{
    vx_status status = VX_SUCCESS;
    VXLIB_bufParams2D_t vxlib_src0;
    uint16 sumSkip;
    uint16 sqSumSkip;
    uint16 skipMask;

    //uint16_t overflow_policy;

    vx_uint32 pipeup = 2;
    vx_uint32 pipedown = 2;
    vx_uint32 exec_cmd = 1;
    vx_uint32 pipeline = exec_cmd;
    vx_uint32 ping_npong = 0;
    vx_uint32 blk;

    vx_uint16 *pSrc0L2[2] = {NULL};
    vx_uint32 *pDstL1 = NULL;
    vx_uint32 *pDstL2= NULL;

    /* Setting first image pointer to the base of L2 then the second buffer
       directly following the pointer location.  Repeating this pattern for
       all images */
    pSrc0L2[0] = (vx_uint16 *)prms->pL2;
    pSrc0L2[1] = (vx_uint16 *)pSrc0L2[0] + (prms->blkWidth * prms->blkHeight);
    pDstL2= (vx_uint32*)((vx_uint16 *)pSrc0L2[1] + (prms->blkWidth * prms->blkHeight));

    pDstL1  = (vx_uint32 *)prms->pL1;

    vxlib_src0.dim_x = prms->blkWidth;
    vxlib_src0.dim_y = prms->blkHeight;
    vxlib_src0.stride_y = prms->blkWidth;
    vxlib_src0.data_type = VXLIB_UINT16;

    /* Initialize histogram to 0 */
#if 0
    for (int32_t i=0; i < dst_desc->num_bins; i++)
    {
        *((uint32_t*)pDstL1 + i)= 0;
    }
#else
    if (prms->numHIST == 1)
    {
        int16_ptr pArray = (int16_ptr)pDstL1;
        for (int32_t i = 0; i < dst_desc->num_bins / 16; i++)
        {
            *(pArray + i) = (int16)(0);
        }
    }
    else
    {
        /* Use this table for 32-bit HIST elements:
        Size Quanta Num elemnts in Quanta    # ways  Quanta replicated
        64-bit	    2	                     16	    16x
        128-bit	    4                         8	    8x
        256-bit	    8                         4	    4x
        512-bit	    16                        2	    2x
        */
        uint16 vZero= (uint16)(0);
        uint32_t row_offset= 0;
        uint32_t numElmtsInQuanta= 32/prms->numHIST;
        for (int32_t j=0; j < dst_desc->num_bins; j += numElmtsInQuanta)
        {
            // Update the quanta in all N ways
            __lookup_init(__LUT_SET0, vZero, row_offset);

            //Increment the row_offset by 2 as there are 2, 32-bit bins every row in a 64-bit bank
            row_offset += numElmtsInQuanta;
        }
    }
#endif

#ifdef ENABLE_HISTO_KERNEL_PRINT_STATS
    printf("<<<<Histogram kernel stats>>>>>\n");
    printf("blk_width = %d\n", prms->blkWidth);
    printf("blk_height = %d\n", prms->blkHeight);
    printf("num_sets = %d\n", prms->numSets);
    printf("num_hist = %d\n", prms->numHIST);
    printf("ping_address = %l\n", pSrc0L2[0]);
    printf("pong_address = %l\n", pSrc0L2[1]);
#endif

    sumSkip = (uint16)(0);
    sqSumSkip = (uint16)(0);

    if( prms->enableChanBitFlag==2 )
    {
        skipMask = (uint16)(DARK_PIXEL_THR_NO_GAIN);
    }
    else
    {
        skipMask = (uint16)(DARK_PIXEL_THR_16_GAIN);
    }

#if MEASURE_HISTO_KERNEL_PERFORMANCE
    clock_t beginTiming = clock();
#endif

    for(blk = 0; blk < (prms->numSets + pipeup + pipedown); blk++)
    {
        if(EXEC_PIPELINE_STAGE1(pipeline))
        {
            dma_transfer_trigger(&prms->dmaObjSrc0);
            //dma_transfer_trigger(&prms->dmaObjSrc1);
        }

        if(EXEC_PIPELINE_STAGE2(pipeline))
        {
            if(blk>skipRows)
            {
                status = C7xVXLIB_histogram_i16u_o32u_cn(
                    (uint16_t *)pSrc0L2[ping_npong],
                    &vxlib_src0,
                    (uint32_t *)pDstL1,
                    dst_desc->offset, dst_desc->range, dst_desc->num_bins, prms->numHIST);
            }
            else
            {
                C7xVXLIB_SkipMeanSdVec_i16u_o32u_cn((uint16_t *)pSrc0L2[ping_npong],
                                             &vxlib_src0,
                                             (uint32_t*)&sumSkip,
                                             (uint32_t*)&sqSumSkip,
                                             (uint32_t*)&skipMask
                                             );
            }
            ping_npong = !ping_npong;
        }
        if(EXEC_PIPELINE_STAGE1(pipeline))
        {
            dma_transfer_wait(&prms->dmaObjSrc0);
            //dma_transfer_wait(&prms->dmaObjSrc1);
        }

        if(blk == (prms->numSets - 1))
        {
            exec_cmd = 0;
        }

        pipeline = (pipeline << 1) | exec_cmd;
    }

    // Merging histogram output
    if (prms->numHIST > 1)
    {
        uint32_t *pDst=  pDstL2;
        for (int32_t bin = 0; bin < dst_desc->num_bins; bin++)
        {
            //Linearly access the histogram
            uint16 vHist = __lookup_read_uint(__LUT_SET0, (uint16)(bin));

            //Merge all partial outputs of bin(N) to a single scalar value
            uint32_t bin_val = __horizontal_add(vHist);
            *pDst++ = bin_val;
        }
    }
#if MEASURE_HISTO_KERNEL_PERFORMANCE
    clock_t endTiming = clock();
    timeSpentHistKernel += (endTiming - beginTiming);
#endif

    uint64_t hist_timestamp = prms->ref_ts + (__TSC - prms->ref_tick);

#if 0
    uint64_t sumSkipTot =  __horizontal_add(sumSkip);
    vx_float32 meanSkip = (float)sumSkipTot/((float)(skipRows*prms->blkWidth));
    uint64_t sqSumSkipTot =  __horizontal_add(sqSumSkip);
    double sdSkipComp = (double) sqSumSkipTot - 
                        (double) meanSkip * (double) meanSkip * ((double)(skipRows*prms->blkWidth));
    sdSkipComp = sdSkipComp / ((double)(skipRows*prms->blkWidth-1));
    double sdSkip_double= __recip(__recip_sqrt(sdSkipComp));
    float2 sdSkip_temp=__double_to_float(sdSkip_double);
    vx_float32  sdSkip= (vx_float32)sdSkip_temp.s[0];
    memcpy(&pDstL2[14], &hist_timestamp, sizeof(vx_uint64));
    memcpy(&pDstL2[16], &meanSkip, sizeof(vx_float32));
    memcpy(&pDstL2[17], &sdSkip, sizeof(vx_float32));
    //printf( "Dark row mean %f, sd %f and ch %u \n",meanSkip,sdSkip,prms->ch_num );

     dma_transfer_trigger(&prms->dmaObjDst);

    // Calculate mean and SD for pixels in the histogram
    {
        //TODO: Need to vectorize this index finding block
        int32_t lastThrBinInd;
        for ( lastThrBinInd = MAX_BRIGHT_INDEX; lastThrBinInd > MIN_BRIGHT_INDEX; lastThrBinInd--)
        {
            if( pDstL2[lastThrBinInd] >= MIN_BIN_VAL )
                break;
        }

        uint8_ptr pVecHisto=  (uint8_ptr)pDstL2;
        pVecHisto += MIN_BRIGHT_INDEX/8;
        uint64_t binIdxArray[8]= {200, 201, 202, 203, 204, 205, 206, 207};
        ulong8   binIdxVec= *((ulong8_ptr)&binIdxArray[0]);
        ulong8   incVec= (ulong8)(8);
        ulong8   numPixVec= (ulong8)(0);
        ulong8   histSumVec= (ulong8)(0);
        ulong8   histSqSumVec= (ulong8)(0);
        for( int32_t binInds = MIN_BRIGHT_INDEX; binInds <= lastThrBinInd; binInds+=8 )
        {
            ulong8 vecHisto= __vload_unpack_long(pVecHisto);
            numPixVec += vecHisto;
            ulong8 temp = vecHisto*binIdxVec;
            histSumVec += temp;
            temp = binIdxVec*temp;
            histSqSumVec+= temp;
            binIdxVec+= incVec;
            pVecHisto++;
        }
        uint64_t numPix= __horizontal_add(numPixVec);
        uint64_t histSum= __horizontal_add(histSumVec);
        uint64_t histSqSum= __horizontal_add(histSqSumVec);

        vx_float32 mean = (float)histSum/((float)numPix);
        double sdComp = (double)histSqSum -
                            (double)mean * (double)mean * (double)numPix;
        sdComp = sdComp / ((double)(numPix-1));
        double sd_double= __recip(__recip_sqrt(sdComp));
        float2 sd_temp=__double_to_float(sd_double);
        vx_float32  sd= (vx_float32)sd_temp.s[0];

        //Calculate the dark pixel corrected speckle contrast
        sdComp = __recip(__recip_sqrt(sdComp - sdSkipComp)); //Corrected std
        sd_temp=__double_to_float(sdComp);
        sd= (vx_float32)sd_temp.s[0];
        mean -= meanSkip; //Corrected mean
        vx_float32  contrast = sd/mean;

        memcpy(pSd, &contrast, 4);
        memcpy(pMean, &mean, 4);
    }
#else
    uint64_t sumSkipTot =  __horizontal_add(sumSkip);
    vx_float32 meanSkip = (float)sumSkipTot/((float)(skipRows*prms->blkWidth));
    
    memcpy(&pDstL2[14], &hist_timestamp, sizeof(vx_uint64));
    memcpy(&pDstL2[16], &meanSkip, sizeof(vx_float32));

    dma_transfer_trigger(&prms->dmaObjDst);
    memcpy(pSd, &meanSkip, 4);
    memcpy(pMean, &meanSkip, 4);
#endif

#if MEASURE_HISTO_KERNEL_PERFORMANCE
    endTiming = clock();
    timeSpent += (endTiming - beginTiming);
    ++frameCount;
    if( frameCount>=40 )
    {
        double timePerFrame = ((double)timeSpent) / ((double)CLOCKS_PER_SEC);
        double timePerFrameHist = ((double)timeSpentHistKernel) / ((double)CLOCKS_PER_SEC);
        timePerFrame /= ((double)frameCount);
        timePerFrameHist /= ((double)frameCount);
        std::cout << "Histo kernel took " << timePerFrameHist << " and all comps took " << 
                  timePerFrame << " secs per frame for the last " << frameCount << " frames\n";
        frameCount = 0;
        timeSpent = 0;
        timeSpentHistKernel = 0;
    }
#endif

    if(status!=VX_SUCCESS)
    {
        printf("Kernel processing failed !!!\n");
        status = VX_FAILURE;
    }

    dma_transfer_wait(&prms->dmaObjDst);
    return status;
}

static vx_status histo_dma_teardown(tivxC7xKernelParams *prms)
{
    vx_status status = VX_SUCCESS;

    dma_deinit(&prms->dmaObjSrc0);
    //dma_deinit(&prms->dmaObjSrc1);
    dma_deinit(&prms->dmaObjDst);

    return status;
}
#if 1
static vx_status histo_c7x_setup(tivxC7xKernelParams *prms)
{
    vx_status status = VX_SUCCESS;
    __sLTCRFlags_t set0flags;
    //1 way lut for 32-bit data with 4096 entries (12-bit index)

    set0flags.INTERPOLATION = __LUT_INTERP_OFF;  //NA, applicable only in LUT mode
    set0flags.SATURATION = __LUT_SAT_ON;         //Saturate after reaching limit
    set0flags.SIGNUNSIGN = __LUT_UNSIGNED;       //data elements are unsigned
    set0flags.ELETYPE = __LUT_ELETYPE_32BIT;     //This is the bit-depth of each bin
    if ((prms->numHIST==1) && (prms->numBins<= 4096))
    {
        set0flags.NUMTBLS = __LUT_NUM_TBL_1;        //1 lookup every cycle
        set0flags.TBLSIZE = __LUT_TBL_SIZE_16KBYTES; //Total table size
    }
    else if ((prms->numHIST==2)  && (prms->numBins<= 4096))
    {
        set0flags.NUMTBLS = __LUT_NUM_TBL_2;        //2 lookup every cycle
        set0flags.TBLSIZE = __LUT_TBL_SIZE_32KBYTES; //Total table size
    }
    else if ((prms->numHIST==4)  && (prms->numBins<= 2048))
    {
        set0flags.NUMTBLS = __LUT_NUM_TBL_4;        //2 lookup every cycle
        set0flags.TBLSIZE = __LUT_TBL_SIZE_32KBYTES; //Total table size
    }
    else if ((prms->numHIST==8)  && (prms->numBins<= 1024))
    {
        set0flags.NUMTBLS = __LUT_NUM_TBL_8;        //8 lookup every cycle
        set0flags.TBLSIZE = __LUT_TBL_SIZE_32KBYTES; //Total table size
    }
    else
    {
        status= VX_FAILURE;
    }
    set0flags.WEIGHTSIZE = __LUT_WSIZE_8BIT;     //Set weight size as 8-bit
    set0flags.PROMOTION = __LUT_PROMOTE_OFF;     //NA, applicable only in LUT mode

    //Set configuration register for SET0
    __LUT_SET_LTCR(__LUT_SET0, __lut_set_param(&set0flags));

    // Start the table at offset 0 from the beginning of L1D-SRAM
    __LUT_SET_LTBR(__LUT_SET0, 0x0000);

    // Enable set 1
    __LUT_SET_LTER(__LUT_ENABLE_0);

    return status;
}
#endif
static vx_status histo_execute(tivxC7xKernelParams *prms,
                             tivx_obj_desc_raw_image_t *src_desc0,
                             //tivx_obj_desc_image_t *src_desc1,
                             tivx_obj_desc_distribution_t *dst_desc,
                             void *src_desc0_target_ptr,
                             //void *src_desc1_target_ptr,
                             void *dst_desc_target_ptr,
                             vx_uint32 *pMean,
                             vx_uint32 *pSd)
{
    vx_status status = VX_SUCCESS;

    //if (0)
    //{
    status = histo_dma_setup(prms, src_desc0, dst_desc, src_desc0_target_ptr, dst_desc_target_ptr);
    status |= histo_c7x_setup(prms);

    if (VX_SUCCESS == status)
    {
        status = histo_pipeline_blocks(prms, src_desc0, dst_desc, src_desc0_target_ptr, dst_desc_target_ptr, pMean, pSd);

        if (VX_SUCCESS == status)
        {
            status = histo_dma_teardown(prms);
        }
        else
        {
            printf("histo_pipeline_blocks failed\n");
        }
    }
    else
    {
        printf("histo_dma_setup failed\n");
    }
    //}

    return status;
}

/**
 * \brief Target kernel run function
 *
 * \param kernel [in] target kernel handle
 * \param obj_desc [in] Parameter object descriptors
 * \param num_params [in] Number of parameter object descriptors
 * \param priv_arg [in] kernel instance priv argument
 */
vx_status VX_CALLBACK app_c7x_target_kernel_histo_process(
    tivx_target_kernel_instance kernel, tivx_obj_desc_t *obj_desc[],
    uint16_t num_params, void *priv_arg)
{
    vx_status status = VX_SUCCESS;
    tivx_obj_desc_raw_image_t *src_desc0;
    tivx_obj_desc_distribution_t *dst_desc;
    tivxC7xKernelParams *prms = NULL;

    if ((num_params != APP_C7X_HISTO_MAX_PARAMS)
        || (NULL == obj_desc[APP_C7X_HISTO_IN_CONFIG_IDX])
        || (NULL == obj_desc[APP_C7X_HISTO_IN_RAW_IMG_IDX])
        || (NULL == obj_desc[APP_C7X_HISTO_OUT_DISTRIBUTION_IDX])
        )
    {
        status = VX_FAILURE;
    }

    if(status==VX_SUCCESS)
    {
        uint32_t size;

        status = tivxGetTargetKernelInstanceContext(kernel,
            (void **)&prms, &size);
        if ((VX_SUCCESS != status) || (NULL == prms) ||
            (sizeof(tivxC7xKernelParams) != size))
        {
            status = VX_FAILURE;
        }
    }

    if(status==VX_SUCCESS)
    {
        void *src_desc0_target_ptr;
        //void *src_desc1_target_ptr;
        void *dst_desc_target_ptr;
        tivx_obj_desc_scalar_t *mean, *sd;

        /* Only run histogram if it was enabled for this channel */
        if (1)//(prms->enableChanBitFlag & (1 << prms->ch_num)) != 0)
        {
            /* Get the Src and Dst descriptors */
            src_desc0 = (tivx_obj_desc_raw_image_t *)obj_desc[APP_C7X_HISTO_IN_RAW_IMG_IDX];            
            dst_desc = (tivx_obj_desc_distribution_t *)obj_desc[APP_C7X_HISTO_OUT_DISTRIBUTION_IDX];
            mean = (tivx_obj_desc_scalar_t *)obj_desc[APP_C7X_HISTO_OUT_MEAN_IDX];
            sd = (tivx_obj_desc_scalar_t *)obj_desc[APP_C7X_HISTO_OUT_SD_IDX];

            /* Get the target pointer from the shared pointer for all
           buffers */
            src_desc0_target_ptr = tivxMemShared2TargetPtr(&src_desc0->mem_ptr[0]);
            
            dst_desc_target_ptr = tivxMemShared2TargetPtr(&dst_desc->mem_ptr);

            /* Map all buffers, which invalidates the cache */
            tivxMemBufferMap(src_desc0_target_ptr,
                             src_desc0->mem_size[0], VX_MEMORY_TYPE_HOST,
                             VX_READ_ONLY);
                             
            tivxMemBufferMap(dst_desc_target_ptr,
                             dst_desc->mem_size, VX_MEMORY_TYPE_HOST,
                             VX_WRITE_ONLY);
#if SHOW_PERF_INFO            
            app_perf_point_t histo_perf;
            char strPerfName[20] = {0};
            sprintf(strPerfName, "HISTO_C7 CH: %02d", prms->ch_num);            
            appPerfPointSetName(&histo_perf, strPerfName);
            appPerfPointBegin(&histo_perf);
#endif
            status = histo_execute(prms, src_desc0, dst_desc, src_desc0_target_ptr, dst_desc_target_ptr, (vx_uint32 *)&mean->data.u32, (vx_uint32 *)&sd->data.u32);
#if SHOW_PERF_INFO 
            appPerfPointEnd(&histo_perf);
            appPerfPointPrint(&histo_perf);
#endif
            tivxMemBufferUnmap(src_desc0_target_ptr,
                               src_desc0->mem_size[0], VX_MEMORY_TYPE_HOST,
                               VX_READ_ONLY);
                               
            tivxMemBufferUnmap(dst_desc_target_ptr,
                               dst_desc->mem_size, VX_MEMORY_TYPE_HOST,
                               VX_WRITE_ONLY);
        }
    }

    return (status);
}

static vx_status app_c7x_target_kernel_histo_timestamp_sync
(
    tivxC7xKernelParams *prms,
    const tivx_obj_desc_user_data_object_t *usr_data_obj
)
{
    tivxHistoTimeSync time_sync;
    vx_status status = (vx_status)VX_SUCCESS;

    // printf("app_c7x_target_kernel_histo_timestamp_sync enter\n");

    if (NULL != usr_data_obj)
    {
        void  *target_ptr = tivxMemShared2TargetPtr(&usr_data_obj->mem_ptr);

        tivxMemBufferMap(target_ptr, usr_data_obj->mem_size,
            (vx_enum)VX_MEMORY_TYPE_HOST, (vx_enum)VX_READ_ONLY);

        if (sizeof(tivxHistoTimeSync) ==  usr_data_obj->mem_size)
        {
            memcpy(&time_sync, target_ptr, sizeof(tivxHistoTimeSync));
        }

        prms->ref_ts = time_sync.refTimestamp;
        prms->ref_tick = __TSC;
        // printf("app_c7x_target_kernel_histo_timestamp_sync: timestamp=0x%llx\n", prms->ref_ts);

        tivxMemBufferUnmap(target_ptr, usr_data_obj->mem_size,
            (vx_enum)VX_MEMORY_TYPE_HOST, (vx_enum)VX_READ_ONLY);

        // printf("app_c7x_target_kernel_histo_timestamp_sync after unmap\n");

    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR,
            "app_c7x_target_kernel_histo_timestamp_sync: Data Object is NULL\n");
        status = (vx_status)VX_ERROR_INVALID_PARAMETERS;
    }

    // printf("app_c7x_target_kernel_histo_timestamp_sync exit\n");
    return (status);
}

static vx_status app_c7x_target_kernel_histo_change_pair
(
    tivxC7xKernelParams *prms,
    const tivx_obj_desc_user_data_object_t *usr_data_obj
)
{
    tivxHistoCmd cmd;
    vx_status status = (vx_status)VX_SUCCESS;

    if (NULL != usr_data_obj)
    {
        void  *target_ptr = tivxMemShared2TargetPtr(&usr_data_obj->mem_ptr);

        tivxMemBufferMap(target_ptr, usr_data_obj->mem_size,
            (vx_enum)VX_MEMORY_TYPE_HOST, (vx_enum)VX_READ_ONLY);

        if (sizeof(tivxHistoCmd) ==  usr_data_obj->mem_size)
        {
            memcpy(&cmd, target_ptr, sizeof(tivxHistoCmd));
        }

        // = cmd.enableChanBitFlag; // this is the channel pair
        //printf("app_c7x_target_kernel_histo_enable_chan: enableChanBitFlag=%d set\n", prms->enableChanBitFlag);
        tivxMemBufferUnmap(target_ptr, usr_data_obj->mem_size,
            (vx_enum)VX_MEMORY_TYPE_HOST, (vx_enum)VX_READ_ONLY);
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR,
            "app_c7x_target_kernel_histo_enable_chan: Data Object is NULL\n");
        status = (vx_status)VX_ERROR_INVALID_PARAMETERS;
    }

    return (status);
}

static vx_status app_c7x_target_kernel_histo_enable_chan
(
    tivxC7xKernelParams *prms,
    const tivx_obj_desc_user_data_object_t *usr_data_obj
)
{
    tivxHistoCmd cmd;
    vx_status status = (vx_status)VX_SUCCESS;

    if (NULL != usr_data_obj)
    {
        void  *target_ptr = tivxMemShared2TargetPtr(&usr_data_obj->mem_ptr);

        tivxMemBufferMap(target_ptr, usr_data_obj->mem_size,
            (vx_enum)VX_MEMORY_TYPE_HOST, (vx_enum)VX_READ_ONLY);

        if (sizeof(tivxHistoCmd) ==  usr_data_obj->mem_size)
        {
            memcpy(&cmd, target_ptr, sizeof(tivxHistoCmd));
        }

        prms->enableChanBitFlag= cmd.enableChanBitFlag;
        //printf("app_c7x_target_kernel_histo_enable_chan: enableChanBitFlag=%d set\n", prms->enableChanBitFlag);
        tivxMemBufferUnmap(target_ptr, usr_data_obj->mem_size,
            (vx_enum)VX_MEMORY_TYPE_HOST, (vx_enum)VX_READ_ONLY);
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR,
            "app_c7x_target_kernel_histo_enable_chan: Data Object is NULL\n");
        status = (vx_status)VX_ERROR_INVALID_PARAMETERS;
    }

    return (status);
}

static vx_status VX_CALLBACK app_c7x_target_kernel_histo_control
(
    tivx_target_kernel_instance kernel,
    uint32_t node_cmd_id,
    tivx_obj_desc_t *obj_desc[],
    uint16_t num_params,
    void *priv_arg
)
{
    vx_status status = VX_SUCCESS;

    tivxC7xKernelParams *prms = NULL;

    if (NULL == obj_desc[0])
    {
        printf("Command handle is NULL!\n");
        status = VX_FAILURE;
    }

    if(status==VX_SUCCESS)
    {
        uint32_t size;

        status = tivxGetTargetKernelInstanceContext(kernel,
            (void **)&prms, &size);
        if ((VX_SUCCESS != status) || (NULL == prms) ||
            (sizeof(tivxC7xKernelParams) != size))
        {
            status = VX_FAILURE;
        }
    }

    switch (node_cmd_id)
    {
        case TIVX_C7X_HISTO_CMD_ENABLE_CHAN:
        {
            app_c7x_target_kernel_histo_enable_chan(prms,
                (tivx_obj_desc_user_data_object_t *)obj_desc[0U]);
            break;
        }
        case TIVX_C7X_HISTO_CMD_CHANGE_PAIR:
        {
            app_c7x_target_kernel_histo_change_pair(prms,
                (tivx_obj_desc_user_data_object_t *)obj_desc[0U]);
            break;
        }        
        case TIVX_C7X_HISTO_TIMESTAMP_SYNC:
        {
            app_c7x_target_kernel_histo_timestamp_sync(prms,
                (tivx_obj_desc_user_data_object_t *)obj_desc[0U]);
            break;
        }
        default:
        {
            VX_PRINT(VX_ZONE_ERROR,
                "app_c7x_target_kernel_histo_control: Invalid Command Id\n");
            status = (vx_status)VX_FAILURE;
            break;
        }
    }

    return (status);
}

/**
 * \brief Target kernel create function
 *
 * \param kernel [in] target kernel handle
 * \param param_obj_desc [in] Parameter object descriptors
 * \param num_params [in] Number of parameter object descriptors
 * \param priv_arg [in] kernel instance priv argument
 */
vx_status VX_CALLBACK app_c7x_target_kernel_histo_create(tivx_target_kernel_instance kernel, tivx_obj_desc_t *param_obj_desc[], uint16_t num_params, void *priv_arg)
{
    vx_status status = VX_SUCCESS;
    uint32_t i;
    int32_t rawImageBitDepth, sizeOfPix;
    tivx_obj_desc_user_data_object_t *config_desc;
    void                             *config_target_ptr;
    tivxHistoParams *params;
    uint32_t sizeOfBin;
    
    static DMAObj dmaObjSrc0;
    static DMAObj dmaObjDst;
    

    for (i = 0U; i < num_params; i++)
    {
        if (NULL == param_obj_desc[i])
        {
            status = VX_FAILURE;
            break;
        }
    }

    if (VX_SUCCESS == status)
    {
        tivxC7xKernelParams *kernelParams = NULL;

        kernelParams = (tivxC7xKernelParams *)tivxMemAlloc(sizeof(tivxC7xKernelParams), TIVX_MEM_EXTERNAL);

        if (kernelParams == NULL)
        {
            status = VX_FAILURE;
        }

        /* Below is added directly from img preproc, need to review */
        if (status == VX_SUCCESS)
        {
            tivx_mem_stats l2_stats, l1_stats;
            vx_uint32 blk_width, blk_height, rem_height, mblk_height;
            vx_uint32 in_size1, total_size, out_size, l1_req_size, l2_req_size, l1_avail_size, l2_avail_size, num_sets;

            /* Get config structure */
            config_desc = (tivx_obj_desc_user_data_object_t *) param_obj_desc[APP_C7X_HISTO_IN_CONFIG_IDX];
            config_target_ptr = tivxMemShared2TargetPtr(&config_desc->mem_ptr);
            tivxMemBufferMap(
                config_target_ptr,
                config_desc->mem_size,
                VX_MEMORY_TYPE_HOST, VX_READ_ONLY);
            params = (tivxHistoParams *)config_target_ptr;

            kernelParams->enableChanBitFlag= params->enableChanBitFlag;
            kernelParams->ch_num= inst_id;
            inst_id++;

            // printf("app_c7x_target_kernel_histo_process: Histogram created for channel %d", kernelParams->ch_num);

            if ((kernelParams->enableChanBitFlag & (1 << kernelParams->ch_num)) != 0)
            {
                printf(" which is enabled\n");
            }
            else
            {
                printf(" which is disabled\n");
            }

            if (kernelParams->ch_num == 0)
            {
                dma_create(&dmaObjSrc0, DATA_COPY_DMA, 8);
                dma_create(&dmaObjDst, DATA_COPY_DMA, 10);
            }
            else
            {
                kernelParams->dmaObjSrc0= dmaObjSrc0;
                kernelParams->dmaObjDst= dmaObjDst;
            }

            tivx_obj_desc_raw_image_t *in_img_desc = (tivx_obj_desc_raw_image_t *)param_obj_desc[APP_C7X_HISTO_IN_RAW_IMG_IDX];
            rawImageBitDepth = in_img_desc->params.format[0].msb + 1;
            if (rawImageBitDepth <= 8)
            {
                sizeOfPix = 1;
            }
            else if (rawImageBitDepth <= 16)
            {
                sizeOfPix = 2;
            }
            else
            {
                printf("Raw image bit depth exceeds 16 bits !\n");
                status = VX_FAILURE;
            }

            vx_imagepatch_addressing_t *pIn = (vx_imagepatch_addressing_t *)&in_img_desc->imagepatch_addr[0];
            vx_int32 in_width = pIn->dim_x;
            vx_uint32 in_height = pIn->dim_y;
            vx_int32 in_stride = pIn->stride_y;

            blk_width = pIn->dim_x;

            tivx_obj_desc_distribution_t *out_distribution_desc = (tivx_obj_desc_distribution_t *)param_obj_desc[APP_C7X_HISTO_OUT_DISTRIBUTION_IDX];
            kernelParams->numBins = out_distribution_desc->num_bins;
            sizeOfBin = 4;

            /* Resetting L2 heap */
            tivxMemFree(NULL, 0, (vx_enum)TIVX_MEM_INTERNAL_L2);
            tivxMemStats(&l2_stats, (vx_enum)TIVX_MEM_INTERNAL_L2);

            l2_avail_size = l2_stats.free_size;

            l2_req_size = 480 * 1024; /* Out of available 512KB - 32KB is cache and 480KB is SRAM */

            if (l2_req_size > l2_avail_size)
                l2_req_size = l2_avail_size;

            total_size = 0;
            mblk_height = 1;
            blk_height = mblk_height;
            rem_height = 0;

            in_size1 = 0;
            //in_size2 = 0;

            /* Using a block width equal to the width of the image.
               Calculating the block height in this loop by starting
               with a height of 1 then multiplying the height by 2
               through each loop.  The loop terminates when either
               the total size required exceeds the available L2
               or the height can longer be divided evenly into the
               total height of the image (so that we can have the
               same size blocks being used in the DMA) */
            while ((total_size < l2_req_size) && (rem_height == 0))
            {
                kernelParams->blkWidth = blk_width;
                kernelParams->blkHeight = blk_height;
                kernelParams->remHeight = rem_height;

                blk_height = mblk_height;
                mblk_height *= 2;

                in_size1 = (blk_width * mblk_height) * sizeOfPix;
                // D3: For this function, there is only one input image, so comment out below line
                //in_size2  = (blk_width * mblk_height);
                // D3: For this function, out_size is size of the histogram
                //out_size = (blk_width * mblk_height * num_bytes);
                out_size = kernelParams->numBins * sizeOfBin;
                /* Double buffer inputs and outputs*/
                in_size1 = in_size1 * 2;
                // D3: For this function, there is only one input image, so comment out below line
                //in_size2 = in_size2 * 2;
                // D3: For this function, the output is written out once at the end of processing so no ping-pong needed
                //out_size = out_size * 2;
                // D3: For this function, there is only one input image, so comment out below line
                //total_size = in_size1 + in_size2 + out_size;
                total_size = in_size1 + out_size;
                rem_height = in_height % mblk_height;
            }

            num_sets = in_height / kernelParams->blkHeight;
            kernelParams->numSets = num_sets;
            kernelParams->l2_avail_size = l2_avail_size;
            kernelParams->l2_req_size = l2_req_size;

            /* In_size for two U8 images will be 2 x blkWidth x blkHeight */
            in_size1 = (kernelParams->blkWidth * kernelParams->blkHeight) * sizeOfPix;
            // D3: For this function, there is only one input image, so comment out below line
            //in_size2  = (kernelParams->blkWidth * kernelParams->blkHeight);
            // D3: For this function, out_size is size of the histogram
            out_size = kernelParams->numBins * sizeOfBin;

            /* Double buffer inputs and outputs*/
            in_size1 = in_size1 * 2;
            //in_size2 = in_size2 * 2;
            // D3: For this function, the output is written out once at the end of processing so no ping-pong needed
            //out_size = out_size * 2;
            //total_size = in_size1 + in_size2 + out_size;
            total_size = in_size1 + out_size;

#ifdef ENABLE_DMA_KERNEL_PRINT_STATS
            printf("blk_width = %d\n", kernelParams->blkWidth);
            printf("blk_height = %d\n", kernelParams->blkHeight);
            printf("num_sets = %d\n", kernelParams->numSets);
            printf("rem_height = %d\n", kernelParams->remHeight);
            printf("in_size1 = %d\n", in_size1);
            //printf("in_size2 = %d\n", in_size2);
            printf("out_size = %d\n", out_size);
            printf("total_size = %d\n", total_size);
            //printf("req_size = %d\n", req_size);
            //printf("avail_size = %d\n", avail_size);
            printf("\n");
#endif

            /* Resetting L1 heap */
            tivxMemFree(NULL, 0, (vx_enum)TIVX_MEM_INTERNAL_L1);
            tivxMemStats(&l1_stats, (vx_enum)TIVX_MEM_INTERNAL_L1);

            l1_avail_size = l1_stats.free_size;

            l1_req_size = 40 * 1024; /* Out of available 40KB */

            if (l1_req_size > l1_avail_size)
            {
                l1_req_size = l1_avail_size;
            }

            if (out_size > l1_req_size)
            {
                printf("app_c7x_target_kernel_histo_process error: out_size exceeed l1 size!\n");
                status = VX_FAILURE;
            }

            kernelParams->l1_avail_size = l1_avail_size;
            kernelParams->l1_req_size = l1_req_size;
            kernelParams->numHIST = l1_req_size / (kernelParams->numBins * sizeOfBin);
            //printf("l1_req_size= %u, range= %u, kernelParams->numBins= %u, sizeOfBin= %u, numHIST=%u\n", l1_req_size, out_distribution_desc->range, kernelParams->numBins, sizeOfBin, kernelParams->numHIST);
            if ((kernelParams->blkWidth % kernelParams->numHIST) != 0)
            {
                printf("app_c7x_target_kernel_histo_process error: image width must be multiple of %u !\n", kernelParams->numHIST);
                status = VX_FAILURE;
            }

            if (status == VX_SUCCESS)
            {
                kernelParams->l2_heap_id = TIVX_MEM_INTERNAL_L2; /* TIVX_MEM_INTERNAL_L2 or TIVX_MEM_EXTERNAL */

                kernelParams->pL2 = (uint8_t *)tivxMemAlloc(kernelParams->l2_req_size, kernelParams->l2_heap_id);
                if (kernelParams->pL2 == NULL)
                {
                    printf("Unable to allocate L2 scratch!\n");
                    status = VX_FAILURE;
                }

                kernelParams->l1_heap_id = TIVX_MEM_INTERNAL_L1; /* TIVX_MEM_INTERNAL_L1 or TIVX_MEM_EXTERNAL */

                kernelParams->pL1 = (uint8_t *)tivxMemAlloc(kernelParams->l1_req_size, kernelParams->l1_heap_id);
                if (kernelParams->pL1 == NULL)
                {
                    printf("Unable to allocate L1 scratch!\n");
                    status = VX_FAILURE;
                }

            }

            tivxMemBufferUnmap(
                config_target_ptr,
                config_desc->mem_size,
                VX_MEMORY_TYPE_HOST, VX_READ_ONLY);
        }
        tivxSetTargetKernelInstanceContext(kernel, kernelParams, sizeof(tivxC7xKernelParams));
    }

    return status;
}

/**
 * \brief Target kernel delete function
 *
 * \param kernel [in] target kernel handle
 * \param obj_desc [in] Parameter object descriptors
 * \param num_params [in] Number of parameter object descriptors
 * \param priv_arg [in] kernel instance priv argument
 */
vx_status VX_CALLBACK app_c7x_target_kernel_histo_delete(tivx_target_kernel_instance kernel, tivx_obj_desc_t *param_obj_desc[], uint16_t num_params, void *priv_arg)
{
    vx_status status = VX_SUCCESS;
    uint32_t i;

    for (i = 0U; i < num_params; i ++)
    {
        if (NULL == param_obj_desc[i])
        {
            status = VX_FAILURE;
            break;
        }
    }

    if (VX_SUCCESS == status)
    {
        uint32_t size;
        tivxC7xKernelParams *prms = NULL;

        status = tivxGetTargetKernelInstanceContext(kernel,
            (void **)&prms, &size);

        if (prms->ch_num== (inst_id - 1))
        {
            dma_delete(&prms->dmaObjSrc0);
            dma_delete(&prms->dmaObjDst);
        }

        tivxMemFree(prms->pL2, prms->l2_req_size, prms->l2_heap_id);
        tivxMemFree(prms->pL1, prms->l1_req_size, prms->l1_heap_id);
        tivxMemFree(prms, sizeof(tivxC7xKernelParams), TIVX_MEM_EXTERNAL);
    }

    inst_id= 0;
    return status;
}

/**
 * \brief Add target kernel to TIOVX framework
 *
 */
void app_c7x_target_kernel_histo_register(void)
{
    char target_name[TIVX_TARGET_MAX_NAME];
    vx_enum self_cpu;

    /**
     * - Get CPU ID of the running CPU
     *
     * Add kernel to target framework only if it is supported on this target
     * \code
     */
    self_cpu = tivxGetSelfCpuId();
    /** \endcode */

    if ((self_cpu == TIVX_CPU_ID_DSP_C7_1))
    {
        /**
         * - Find target name based on currently running CPU
         *
         * \code
         */
        strncpy(target_name, TIVX_TARGET_DSP_C7_1,
                TIVX_TARGET_MAX_NAME);
        /** \endcode */

        /**
         * - Register target kernel to TIOVX framework
         *
         * "APP_C7X_KERNEL_HISTO_NAME" is the name of the target kernel.
         * See also \ref app_c7x_kernel.h
         *
         * This MUST match the name specified when registering the same kernel
         * on the HOST side.
         *
         * The registered target kernel handle is stored in a global variable.
         * This is used during app_c7x_target_kernel_histo_unregister()
         *
         * \code
         */
        app_c7x_target_kernel_histo_handle = tivxAddTargetKernelByName(
                    (char*)APP_C7X_KERNEL_HISTO_NAME,
                    target_name,
                    app_c7x_target_kernel_histo_process,
                    app_c7x_target_kernel_histo_create,
                    app_c7x_target_kernel_histo_delete,
                    app_c7x_target_kernel_histo_control,
                    NULL);
        /** \endcode */
    }
}

/**
 * \brief Remove target kernel from TIOVX framework
 *
 */
vx_status app_c7x_target_kernel_histo_unregister(void)
{
    vx_status status = VX_SUCCESS;

    /**
     * - UnRegister target kernel from TIOVX framework
     *
     * The target kernel handle used is the one returned during
     * tivxAddTargetKernel()
     *
     * \code
     */
    status = tivxRemoveTargetKernel(app_c7x_target_kernel_histo_handle);
    /** \endcode */

    if (VX_SUCCESS == status)
    {
        app_c7x_target_kernel_histo_handle = NULL;
    }
    return status;
}
