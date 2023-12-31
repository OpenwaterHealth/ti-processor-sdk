/*
 *
 * Copyright (c) 2019 Texas Instruments Incorporated
 *
 * All rights reserved not granted herein.
 *
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
 * license under copyrights and patents it now or hereafter owns or controls to make,
 * have made, use, import, offer to sell and sell ("Utilize") this software subject to the
 * terms herein.  With respect to the foregoing patent license, such license is granted
 * solely to the extent that any such patent is necessary to Utilize the software alone.
 * The patent license shall not apply to any combinations which include this software,
 * other than combinations with devices manufactured by or for TI ("TI Devices").
 * No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this license
 * (including the above copyright notice and the disclaimer and (if applicable) source
 * code license limitations below) in the documentation and/or other materials provided
 * with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted provided
 * that the following conditions are met:
 *
 * *       No reverse engineering, decompilation, or disassembly of this software is
 * permitted with respect to any software provided in binary form.
 *
 * *       any redistribution and use are licensed by TI for use only with TI Devices.
 *
 * *       Nothing shall obligate TI to provide you with source code for the software
 * licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the
 * source code are permitted provided that the following conditions are met:
 *
 * *       any redistribution and use of the source code, including any resulting derivative
 * works, are licensed by TI for use only with TI Devices.
 *
 * *       any redistribution and use of any object code compiled from the source code
 * and any resulting derivative works, are licensed by TI for use only with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers
 *
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef TIVX_LIDAR_KERNELS_H_
#define TIVX_LIDAR_KERNELS_H_

#include <VX/vx.h>
#include <VX/vx_kernels.h>

#include <perception/algos/lidar_ogmap.h>
#include <perception/algos/lidar_gpc.h>
#include <perception/algos/lidar_mdc.h>
#include <perception/algos/fsd_pfsd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup group_vision_apps_kernels_lidar TIVX Kernels for Lidar Processing
 *
 * \brief This section documents the kernels defined for Lidar processing
 * \ingroup group_vision_apps_kernels
 */

/*!
 * \file
 * \brief The list of supported kernels in this kernel extension.
 */

/*! \brief OpenVX module name
 * \ingroup group_vision_apps_kernels_lidar
 */
#define TIVX_MODULE_NAME_LIDAR    "lidar"

/*! \brief Kernel Name: Lidar ground plane classification (GPC)
 * \ingroup group_vision_apps_kernels_lidar
 */
#define TIVX_KERNEL_LIDAR_GPC_NAME     "com.ti.lidar.lidar_gpc"

/*! \brief Kernel Name: Lidar Motion Distortion Correction (MDC)
 * \ingroup group_vision_apps_kernels_lidar
 */
#define TIVX_KERNEL_LIDAR_MDC_NAME     "com.ti.lidar.lidar_mdc"

/*********************************
 *      Functions
 *********************************/

/*!
 * \brief Used for the application to load the lidar kernels into the context.
 * \ingroup group_vision_apps_kernels_lidar
 */
void tivxLidarLoadKernels(vx_context context);

/*!
 * \brief Used for the application to unload the lidar kernels from the context.
 * \ingroup group_vision_apps_kernels_lidar
 */
void tivxLidarUnLoadKernels(vx_context context);


/************************************************
 *      Ground Plane Classification (GPC) Kernel
 ************************************************/

//typedef PTK_Lidar_GpcConfig tivx_lidar_gpc_config_t;

#ifdef __cplusplus
}
#endif

#endif /* TIVX_LIDAR_KERNELS_H_ */


