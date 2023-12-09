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

#include "VXLIB_histogram_i16u_o32u_cn.h"
#include <c7x.h>

#define UINT8_MAX   0xff
#if 0
VXLIB_STATUS C7xVXLIB_histogram_i16u_o32u_cn(uint16_t restrict src[],
                                             VXLIB_bufParams2D_t * restrict src_addr,
                                             uint32_t restrict dist[],
                                             uint16_t offset,
                                             uint16_t range,
                                             uint16_t numBin,
                                             uint16_t numHIST)
{
    uint32_t x, y;
    size_t index;
    uint32_t shift= 0;
    VXLIB_STATUS status = VXLIB_SUCCESS;

    while (range > numBin)
    {
        range>>=1;
        shift++;
    }

#if 1
#ifdef VXLIB_CHECK_PARAMS
    status = C7xVXLIB_histogram_i16u_o32u_checkParams_cn(src, src_addr, dist, offset, range, numHIST);
    if (status == VXLIB_SUCCESS)
#endif
    {
        for (y = 0; y < src_addr->dim_y; y++)
        {

            for (x = 0; x < src_addr->dim_x; x++)
            {

                uint16_t pixel = src[y * src_addr->stride_y + x];
                if ((offset <= (size_t)pixel) && ((size_t)pixel < (offset + range)))
                {
                    index = (pixel - (uint16_t)offset) >> shift;
                    dist[index]++;
                }
            }
        }
    }
#else
    dist[0]=0;
#endif
    return (status);
}
#else
VXLIB_STATUS C7xVXLIB_histogram_i16u_o32u_cn(uint16_t restrict src[],
                                             VXLIB_bufParams2D_t *restrict src_addr,
                                             uint32_t restrict dist[],
                                             uint16_t offset,
                                             uint16_t range,
                                             uint16_t numBin,
                                             uint16_t numHIST)
{
    uint32_t shift= 0;
    VXLIB_STATUS status = VXLIB_SUCCESS;

#if 1
#ifdef VXLIB_CHECK_PARAMS
    status = C7xVXLIB_histogram_i16u_o32u_checkParams_cn(src, src_addr, dist, offset, range, numHIST);
    if (status == VXLIB_SUCCESS)
#endif

    __SE_TEMPLATE_v1 seTemplate = __gen_SE_TEMPLATE_v1();

    while (range > numBin)
    {
        range>>=1;
        shift++;
    }

    //Specify element type
    seTemplate.ELETYPE = __SE_ELETYPE_16BIT;

    //Promote unsigned 16-bit elements to unsigned 32-bit elements
    seTemplate.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;
    seTemplate.ELEDUP = __SE_ELEDUP_OFF;

    if (numHIST == 1)
    {
        //Fetch one element of 32-bit elements
        seTemplate.VECLEN = __SE_VECLEN_1ELEM;
    }
    else if (numHIST == 2)
    {
        //Fetch two element of 32-bit elements
        seTemplate.VECLEN = __SE_VECLEN_2ELEMS;
    }
    else if (numHIST == 4)
    {
        //Fetch four elements of 32-bit elements
        seTemplate.VECLEN = __SE_VECLEN_4ELEMS;
    }
    else if (numHIST == 8)
    {
        //Fetch eight elements of 32-bit elements
        seTemplate.VECLEN = __SE_VECLEN_8ELEMS;
    }
    else if (numHIST == 16)
    {
        //Fetch sixteen elements of 32-bit elements
        seTemplate.VECLEN = __SE_VECLEN_16ELEMS;
    }
    else
    {
        status= VXLIB_ERR_FAILURE;
    }

    //Specify the type of transfer
    seTemplate.DIMFMT = __SE_DIMFMT_2D;

    //Set the size of sample set as ICNT0
    uint32_t xCount= src_addr->dim_x/numHIST;
    seTemplate.ICNT0 = (xCount)*numHIST;
    seTemplate.ICNT1 = src_addr->dim_y;
    seTemplate.DIM1 = src_addr->stride_y;

    __SE0_OPEN((void *)&src[0], seTemplate);

    for (int32_t k = 0; k < xCount*src_addr->dim_y; k++)
    {
        uint16 vSample = __SE0ADV(uint16);
        vSample= vSample >> shift;
        __hist(__LUT_SET0, vSample);
    }

    __SE0_CLOSE();

#else
    dist[0] = 0;
#endif

    return (status);
}

#endif
VXLIB_STATUS C7xVXLIB_histogram_i16u_o32u_checkParams_cn(uint16_t src[],
                                                         VXLIB_bufParams2D_t *src_addr,
                                                         uint32_t dist[],
                                                         uint16_t offset,
                                                         uint16_t range,
                                                         uint16_t numHIST)
{
    VXLIB_STATUS    status = VXLIB_SUCCESS;

    if((src == NULL) || (dist == NULL)) {
        status = VXLIB_ERR_NULL_POINTER;
    } else if( src_addr->stride_y < src_addr->dim_x ) {
        status = VXLIB_ERR_INVALID_DIMENSION;
    } else if( src_addr->data_type != VXLIB_UINT16 ) {
        status = VXLIB_ERR_INVALID_TYPE;
    }

    return (status);
}

VXLIB_STATUS C7xVXLIB_SkipMeanSdVec_i16u_o32u_cn(uint16_t restrict src[],
                                             VXLIB_bufParams2D_t *restrict src_addr,
                                             uint32_t * restrict pSumSum,
                                             uint32_t * restrict pSquareSum,
                                             uint32_t * restrict pMaskBits)
{
    uint16_ptr pVecSumSum= (uint16_ptr)pSumSum;
    uint16_ptr pVecSquareSum= (uint16_ptr)pSquareSum;
    uint16_ptr pVecMaskBits= (uint16_ptr)pMaskBits;

    uint16 vSumSum= *pVecSumSum;
    uint16 vSquareSum= *pVecSquareSum;
    uint16 vMaskBits= *pVecMaskBits;
    
    VXLIB_STATUS status = VXLIB_SUCCESS;

    __SE_TEMPLATE_v1 seTemplate = __gen_SE_TEMPLATE_v1();

    //Specify element type
    seTemplate.ELETYPE = __SE_ELETYPE_16BIT;

    //Promote unsigned 16-bit elements to unsigned 32-bit elements
    seTemplate.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;
    seTemplate.ELEDUP = __SE_ELEDUP_OFF;

    //Fetch eight elements of 32-bit elements
    seTemplate.VECLEN = __SE_VECLEN_8ELEMS;
    
    //Specify the type of transfer
    seTemplate.DIMFMT = __SE_DIMFMT_2D;

    //Set the size of sample set as ICNT0
    uint32_t xCount= src_addr->dim_x/8;
    seTemplate.ICNT0 = (xCount)*8;
    seTemplate.ICNT1 = src_addr->dim_y;
    seTemplate.DIM1 = src_addr->stride_y;

    __SE0_OPEN((void *)&src[0], seTemplate);

    for (int32_t k = 0; k < xCount; k++)
    {
        uint16 vSample = __SE0ADV(uint16);
        vSample = vSample & vMaskBits;
        uint16 pix = vSample;
        vSumSum += pix;
        pix *= vSample;
        vSquareSum += pix;
    }

    __SE0_CLOSE();

    *pVecSumSum= vSumSum;
    *pVecSquareSum= vSquareSum;

    return (status);
}
