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

#include "TI/tivx.h"
#include "TI/tivx_park_assist.h"
#include "tivx_park_assist_kernels_priv.h"
#include "tivx_kernel_sfm_ogmap.h"
#include "TI/tivx_target_kernel.h"

static vx_kernel vx_sfm_ogmap_kernel = NULL;

static vx_status VX_CALLBACK tivxAddKernelSfmOgmapValidate(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num,
            vx_meta_format metas[]);
static vx_status VX_CALLBACK tivxAddKernelSfmOgmapInitialize(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num_params);
vx_status tivxAddKernelSfmOgmap(vx_context context);
vx_status tivxRemoveKernelSfmOgmap(vx_context context);

static vx_status VX_CALLBACK tivxAddKernelSfmOgmapValidate(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num,
            vx_meta_format metas[])
{
    vx_status status = VX_SUCCESS;

    vx_user_data_object configuration = NULL;
    vx_char configuration_name[VX_MAX_REFERENCE_NAME];
    vx_size configuration_size;

    vx_array input_point = NULL;
    vx_enum input_point_item_type;
    vx_size input_point_item_size;

    vx_user_data_object input_point_transform = NULL;
    vx_char input_point_transform_name[VX_MAX_REFERENCE_NAME];
    vx_size input_point_transform_size;

    vx_user_data_object input_pose = NULL;
    vx_char input_pose_name[VX_MAX_REFERENCE_NAME];
    vx_size input_pose_size;

    vx_user_data_object reftr_and_pose = NULL;
    vx_char reftr_and_pose_name[VX_MAX_REFERENCE_NAME];
    vx_size reftr_and_pose_size;

    vx_user_data_object pfsd_out_desc = NULL;
    vx_char pfsd_out_desc_name[VX_MAX_REFERENCE_NAME];
    vx_size pfsd_out_desc_size;

    vx_user_data_object acc_output_map = NULL;
    vx_char acc_output_map_name[VX_MAX_REFERENCE_NAME];
    vx_size acc_output_map_size;

    vx_user_data_object inst_output_map = NULL;
    vx_char inst_output_map_name[VX_MAX_REFERENCE_NAME];
    vx_size inst_output_map_size;

    if ( (num != TIVX_KERNEL_SFM_OGMAP_MAX_PARAMS)
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_CONFIGURATION_IDX])
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_INPUT_POINT_IDX])
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_INPUT_POINT_TRANSFORM_IDX])
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_INPUT_POSE_IDX])
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_REFTR_AND_POSE_IDX])
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_PFSD_OUT_DESC_IDX])
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_ACC_OUTPUT_MAP_IDX])
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_INST_OUTPUT_MAP_IDX])
    )
    {
        status = VX_ERROR_INVALID_PARAMETERS;
        VX_PRINT(VX_ZONE_ERROR, "One or more REQUIRED parameters are set to NULL\n");
    }

    if (VX_SUCCESS == status)
    {
        configuration = (vx_user_data_object)parameters[TIVX_KERNEL_SFM_OGMAP_CONFIGURATION_IDX];
        input_point = (vx_array)parameters[TIVX_KERNEL_SFM_OGMAP_INPUT_POINT_IDX];
        input_point_transform = (vx_user_data_object)parameters[TIVX_KERNEL_SFM_OGMAP_INPUT_POINT_TRANSFORM_IDX];
        input_pose = (vx_user_data_object)parameters[TIVX_KERNEL_SFM_OGMAP_INPUT_POSE_IDX];
        reftr_and_pose = (vx_user_data_object)parameters[TIVX_KERNEL_SFM_OGMAP_REFTR_AND_POSE_IDX];
        pfsd_out_desc = (vx_user_data_object)parameters[TIVX_KERNEL_SFM_OGMAP_PFSD_OUT_DESC_IDX];
        acc_output_map = (vx_user_data_object)parameters[TIVX_KERNEL_SFM_OGMAP_ACC_OUTPUT_MAP_IDX];
        inst_output_map = (vx_user_data_object)parameters[TIVX_KERNEL_SFM_OGMAP_INST_OUTPUT_MAP_IDX];
    }


    /* PARAMETER ATTRIBUTE FETCH */

    if (VX_SUCCESS == status)
    {
        tivxCheckStatus(&status, vxQueryUserDataObject(configuration, VX_USER_DATA_OBJECT_NAME, &configuration_name, sizeof(configuration_name)));
        tivxCheckStatus(&status, vxQueryUserDataObject(configuration, VX_USER_DATA_OBJECT_SIZE, &configuration_size, sizeof(configuration_size)));

        tivxCheckStatus(&status, vxQueryArray(input_point, VX_ARRAY_ITEMTYPE, &input_point_item_type, sizeof(input_point_item_type)));
        tivxCheckStatus(&status, vxQueryArray(input_point, VX_ARRAY_ITEMSIZE, &input_point_item_size, sizeof(input_point_item_size)));

        tivxCheckStatus(&status, vxQueryUserDataObject(input_point_transform, VX_USER_DATA_OBJECT_NAME, &input_point_transform_name, sizeof(input_point_transform_name)));
        tivxCheckStatus(&status, vxQueryUserDataObject(input_point_transform, VX_USER_DATA_OBJECT_SIZE, &input_point_transform_size, sizeof(input_point_transform_size)));

        tivxCheckStatus(&status, vxQueryUserDataObject(input_pose, VX_USER_DATA_OBJECT_NAME, &input_pose_name, sizeof(input_pose_name)));
        tivxCheckStatus(&status, vxQueryUserDataObject(input_pose, VX_USER_DATA_OBJECT_SIZE, &input_pose_size, sizeof(input_pose_size)));

        tivxCheckStatus(&status, vxQueryUserDataObject(reftr_and_pose, VX_USER_DATA_OBJECT_NAME, &reftr_and_pose_name, sizeof(reftr_and_pose_name)));
        tivxCheckStatus(&status, vxQueryUserDataObject(reftr_and_pose, VX_USER_DATA_OBJECT_SIZE, &reftr_and_pose_size, sizeof(reftr_and_pose_size)));

        tivxCheckStatus(&status, vxQueryUserDataObject(pfsd_out_desc, VX_USER_DATA_OBJECT_NAME, &pfsd_out_desc_name, sizeof(pfsd_out_desc_name)));
        tivxCheckStatus(&status, vxQueryUserDataObject(pfsd_out_desc, VX_USER_DATA_OBJECT_SIZE, &pfsd_out_desc_size, sizeof(pfsd_out_desc_size)));

        tivxCheckStatus(&status, vxQueryUserDataObject(acc_output_map, VX_USER_DATA_OBJECT_NAME, &acc_output_map_name, sizeof(acc_output_map_name)));
        tivxCheckStatus(&status, vxQueryUserDataObject(acc_output_map, VX_USER_DATA_OBJECT_SIZE, &acc_output_map_size, sizeof(acc_output_map_size)));

        tivxCheckStatus(&status, vxQueryUserDataObject(inst_output_map, VX_USER_DATA_OBJECT_NAME, &inst_output_map_name, sizeof(inst_output_map_name)));
        tivxCheckStatus(&status, vxQueryUserDataObject(inst_output_map, VX_USER_DATA_OBJECT_SIZE, &inst_output_map_size, sizeof(inst_output_map_size)));
    }

    /* PARAMETER CHECKING */

    if (VX_SUCCESS == status)
    {
        if ((configuration_size != sizeof(tivx_sfm_ogmap_pfsd_params_t)) ||
            (strncmp(configuration_name, "tivx_sfm_ogmap_pfsd_params_t", sizeof(configuration_name)) != 0))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            VX_PRINT(VX_ZONE_ERROR, "'configuration' should be a user_data_object of type:\n tivx_sfm_ogmap_pfsd_params_t \n");
        }

        if ( input_point_item_size != sizeof(PTK_Point))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            VX_PRINT(VX_ZONE_ERROR, "'input_point' should be an array of type:\n PTK_Point \n");
        }

        if ((input_point_transform_size != sizeof(PTK_RigidTransform)) ||
            (strncmp(input_point_transform_name, "PTK_RigidTransform", sizeof(input_point_transform_name)) != 0))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            VX_PRINT(VX_ZONE_ERROR, "'input_point_transform' should be a user_data_object of type:\n PTK_RigidTransform \n");
        }

        if ((input_pose_size != sizeof(PTK_RigidTransform)) ||
            (strncmp(input_pose_name, "PTK_RigidTransform", sizeof(input_pose_name)) != 0))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            VX_PRINT(VX_ZONE_ERROR, "'input_pose' should be a user_data_object of type:\n PTK_RigidTransform \n");
        }

        if ((reftr_and_pose_size != sizeof(PTK_InsPoseAndRef)) ||
            (strncmp(reftr_and_pose_name, "PTK_InsPoseAndRef", sizeof(reftr_and_pose_name)) != 0))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            VX_PRINT(VX_ZONE_ERROR, "'reftr_and_pose' should be a user_data_object of type:\n PTK_InsPoseAndRef \n");
        }

        if ((pfsd_out_desc_size != sizeof(PTK_Alg_FsdPfsdPSDesc)) ||
            (strncmp(pfsd_out_desc_name, "PTK_Alg_FsdPfsdPSDesc", sizeof(pfsd_out_desc_name)) != 0))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            VX_PRINT(VX_ZONE_ERROR, "'pfsd_out_desc' should be a user_data_object of type:\n PTK_Alg_FsdPfsdPSDesc \n");
        }
    }

    return status;
}

static vx_status VX_CALLBACK tivxAddKernelSfmOgmapInitialize(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num_params)
{
    vx_status status = VX_SUCCESS;

    if ( (num_params != TIVX_KERNEL_SFM_OGMAP_MAX_PARAMS)
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_CONFIGURATION_IDX])
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_INPUT_POINT_IDX])
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_INPUT_POINT_TRANSFORM_IDX])
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_INPUT_POSE_IDX])
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_REFTR_AND_POSE_IDX])
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_PFSD_OUT_DESC_IDX])
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_ACC_OUTPUT_MAP_IDX])
        || (NULL == parameters[TIVX_KERNEL_SFM_OGMAP_INST_OUTPUT_MAP_IDX])
    )
    {
        status = VX_ERROR_INVALID_PARAMETERS;
        VX_PRINT(VX_ZONE_ERROR, "One or more REQUIRED parameters are set to NULL\n");
    }
    return status;
}

vx_status tivxAddKernelSfmOgmap(vx_context context)
{
    vx_kernel kernel;
    vx_status status;
    uint32_t index;
    vx_enum kernel_id;

    status = vxAllocateUserKernelId(context, &kernel_id);
    if(status != VX_SUCCESS)
    {
        VX_PRINT(VX_ZONE_ERROR, "Unable to allocate user kernel ID\n");
    }

    if (status == VX_SUCCESS)
    {
        kernel = vxAddUserKernel(
                    context,
                    TIVX_KERNEL_SFM_OGMAP_NAME,
                    kernel_id,
                    NULL,
                    TIVX_KERNEL_SFM_OGMAP_MAX_PARAMS,
                    tivxAddKernelSfmOgmapValidate,
                    tivxAddKernelSfmOgmapInitialize,
                    NULL);

        status = vxGetStatus((vx_reference)kernel);
    }
    if (status == VX_SUCCESS)
    {
        index = 0;

        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_INPUT,
                        VX_TYPE_USER_DATA_OBJECT,
                        VX_PARAMETER_STATE_REQUIRED
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_INPUT,
                        VX_TYPE_ARRAY,
                        VX_PARAMETER_STATE_REQUIRED
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_INPUT,
                        VX_TYPE_USER_DATA_OBJECT,
                        VX_PARAMETER_STATE_REQUIRED
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_INPUT,
                        VX_TYPE_USER_DATA_OBJECT,
                        VX_PARAMETER_STATE_REQUIRED
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_INPUT,
                        VX_TYPE_USER_DATA_OBJECT,
                        VX_PARAMETER_STATE_REQUIRED
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_OUTPUT,
                        VX_TYPE_USER_DATA_OBJECT,
                        VX_PARAMETER_STATE_REQUIRED
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_OUTPUT,
                        VX_TYPE_USER_DATA_OBJECT,
                        VX_PARAMETER_STATE_REQUIRED
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_OUTPUT,
                        VX_TYPE_USER_DATA_OBJECT,
                        VX_PARAMETER_STATE_REQUIRED
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            /* add supported target's */
            tivxAddKernelTarget(kernel, TIVX_TARGET_DSP1);
            tivxAddKernelTarget(kernel, TIVX_TARGET_DSP2);
            tivxAddKernelTarget(kernel, TIVX_TARGET_A72_0);
            tivxAddKernelTarget(kernel, TIVX_TARGET_A72_1);
            tivxAddKernelTarget(kernel, TIVX_TARGET_A72_2);
            tivxAddKernelTarget(kernel, TIVX_TARGET_A72_3);
            tivxAddKernelTarget(kernel, TIVX_TARGET_DSP_C7_1);
        }
        if (status == VX_SUCCESS)
        {
            status = vxFinalizeKernel(kernel);
        }
        if (status != VX_SUCCESS)
        {
            vxReleaseKernel(&kernel);
            kernel = NULL;
        }
    }
    else
    {
        kernel = NULL;
    }
    vx_sfm_ogmap_kernel = kernel;

    return status;
}

vx_status tivxRemoveKernelSfmOgmap(vx_context context)
{
    vx_status status;
    vx_kernel kernel = vx_sfm_ogmap_kernel;

    status = vxRemoveKernel(kernel);
    vx_sfm_ogmap_kernel = NULL;

    return status;
}


