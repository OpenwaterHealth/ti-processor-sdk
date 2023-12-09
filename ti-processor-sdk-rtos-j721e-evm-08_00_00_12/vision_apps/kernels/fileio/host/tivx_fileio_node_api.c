/*
 *
 * Copyright (c) 2020 Texas Instruments Incorporated
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

#include <TI/tivx.h>
#include <TI/tivx_fileio.h>

VX_API_ENTRY vx_node VX_API_CALL tivxWriteArrayNode(vx_graph   graph,
                                                    vx_array   in_array,
                                                    vx_array   file_path,
                                                    vx_array   file_prefix)
{
    vx_reference prms[] = {
            (vx_reference)in_array,
            (vx_reference)file_path,
            (vx_reference)file_prefix
    };
    vx_node node = tivxCreateNodeByKernelName(graph,
                                           TIVX_KERNEL_WRITE_ARRAY_NAME,
                                           prms,
                                           dimof(prms));
    return node;
}

VX_API_ENTRY vx_node VX_API_CALL tivxWriteImageNode(vx_graph   graph,
                                                    vx_image   in_image,
                                                    vx_array   file_path,
                                                    vx_array   file_prefix)
{
    vx_reference prms[] = {
            (vx_reference)in_image,
            (vx_reference)file_path,
            (vx_reference)file_prefix
    };
    vx_node node = tivxCreateNodeByKernelName(graph,
                                           TIVX_KERNEL_WRITE_IMAGE_NAME,
                                           prms,
                                           dimof(prms));
    return node;
}

VX_API_ENTRY vx_node VX_API_CALL tivxWriteRawImageNode(vx_graph   graph,
                                                       tivx_raw_image   in_raw_image,
                                                       vx_array   file_path,
                                                       vx_array   file_prefix)
{
    vx_reference prms[] = {
            (vx_reference)in_raw_image,
            (vx_reference)file_path,
            (vx_reference)file_prefix
    };
    vx_node node = tivxCreateNodeByKernelName(graph,
                                           TIVX_KERNEL_WRITE_RAW_IMAGE_NAME,
                                           prms,
                                           dimof(prms));
    return node;
}

VX_API_ENTRY vx_node VX_API_CALL tivxWriteTensorNode(vx_graph   graph,
                                                     vx_tensor  in_tensor,
                                                     vx_array   file_path,
                                                     vx_array   file_prefix)
{
    vx_reference prms[] = {
            (vx_reference)in_tensor,
            (vx_reference)file_path,
            (vx_reference)file_prefix
    };
    vx_node node = tivxCreateNodeByKernelName(graph,
                                          TIVX_KERNEL_WRITE_TENSOR_NAME,
                                          prms,
                                          dimof(prms));
    return node;
}

VX_API_ENTRY vx_node VX_API_CALL tivxWriteUserDataObjectNode(vx_graph             graph,
                                                             vx_user_data_object  in_user_data_object,
                                                             vx_array             file_path,
                                                             vx_array             file_prefix)
{
    vx_reference prms[] = {
            (vx_reference)in_user_data_object,
            (vx_reference)file_path,
            (vx_reference)file_prefix
    };
    vx_node node = tivxCreateNodeByKernelName(graph,
                                          TIVX_KERNEL_WRITE_USER_DATA_OBJECT_NAME,
                                          prms,
                                          dimof(prms));
    return node;
}

VX_API_ENTRY vx_node VX_API_CALL tivxWriteHistogramNode(vx_graph             graph,
                                                             vx_distribution  in_histogram,
                                                             vx_scalar mean,
                                                             vx_scalar sd,
                                                             vx_array             file_path,
                                                             vx_array             file_prefix)
{
    vx_reference prms[] = {
            (vx_reference)in_histogram,
            (vx_reference)mean,
            (vx_reference)sd,
            (vx_reference)file_path,
            (vx_reference)file_prefix
    };
    vx_node node = tivxCreateNodeByKernelName(graph,
                                          TIVX_KERNEL_WRITE_HISTOGRAM_NAME,
                                          prms,
                                          dimof(prms));
    return node;
}
