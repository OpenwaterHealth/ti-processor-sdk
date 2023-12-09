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

#ifndef VXLIB_HISTOGRAM_I16U_O32U_CN_H_
#define VXLIB_HISTOGRAM_I16U_O32U_CN_H_ 1

#include "VXLIB_types.h"

VXLIB_STATUS C7xVXLIB_histogram_i16u_o32u_cn(uint16_t restrictsrc0[],
                                             VXLIB_bufParams2D_t * restrict src0_addr,
                                             uint32_t restrict dist[],
                                             uint16_t offset,
                                             uint16_t range,
                                             uint16_t numBins,
                                             uint16_t numHist);

VXLIB_STATUS C7xVXLIB_histogram_i16u_o32u_checkParams_cn(uint16_t src0[],
                                                         VXLIB_bufParams2D_t *src0_addr,
                                                         uint32_t dist[],
                                                         uint16_t offset,
                                                         uint16_t range,
                                                         uint16_t numBins);

VXLIB_STATUS C7xVXLIB_SkipMeanSdVec_i16u_o32u_cn(uint16_t restrict src[],
                                             VXLIB_bufParams2D_t *restrict src_addr,
                                             uint32_t *restrict pSumSkip,
                                             uint32_t *restrict pSquareSum,
                                             uint32_t *restrict pMaskBits);

#endif
