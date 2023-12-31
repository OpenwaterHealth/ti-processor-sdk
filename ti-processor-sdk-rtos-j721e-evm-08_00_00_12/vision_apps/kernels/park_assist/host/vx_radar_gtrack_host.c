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
#include "tivx_kernel_radar_gtrack.h"
#include "TI/tivx_target_kernel.h"

static vx_kernel vx_radar_gtrack_kernel = NULL;

static vx_status VX_CALLBACK tivxAddKernelRadarGtrackValidate(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num,
            vx_meta_format metas[]);
static vx_status VX_CALLBACK tivxAddKernelRadarGtrackInitialize(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num_params);
vx_status tivxAddKernelRadarGtrack(vx_context context);
vx_status tivxRemoveKernelRadarGtrack(vx_context context);

static vx_status VX_CALLBACK tivxAddKernelRadarGtrackValidate(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num,
            vx_meta_format metas[])
{
    vx_status status = VX_SUCCESS;

    vx_user_data_object configuration = NULL;
    vx_char configuration_name[VX_MAX_REFERENCE_NAME];
    vx_size configuration_size;

    vx_user_data_object sensor_config = NULL;
    vx_char sensor_config_name[VX_MAX_REFERENCE_NAME];
    vx_size sensor_config_size;

    vx_user_data_object radar_obj_data = NULL;
    vx_char radar_obj_data_name[VX_MAX_REFERENCE_NAME];
    vx_size radar_obj_data_size;

    vx_user_data_object reftr_and_pose = NULL;
    vx_char reftr_and_pose_name[VX_MAX_REFERENCE_NAME];
    vx_size reftr_and_pose_size;

    vx_user_data_object track_info = NULL;
    vx_char track_info_name[VX_MAX_REFERENCE_NAME];
    vx_size track_info_size;

    if ( (num != TIVX_KERNEL_RADAR_GTRACK_MAX_PARAMS)
        || (NULL == parameters[TIVX_KERNEL_RADAR_GTRACK_CONFIGURATION_IDX])
        || (NULL == parameters[TIVX_KERNEL_RADAR_GTRACK_SENSOR_CONFIG_IDX])
        || (NULL == parameters[TIVX_KERNEL_RADAR_GTRACK_RADAR_OBJ_DATA_IDX])
        || (NULL == parameters[TIVX_KERNEL_RADAR_GTRACK_REFTR_AND_POSE_IDX])
        || (NULL == parameters[TIVX_KERNEL_RADAR_GTRACK_TRACK_INFO_IDX])
    )
    {
        status = VX_ERROR_INVALID_PARAMETERS;
        VX_PRINT(VX_ZONE_ERROR, "One or more REQUIRED parameters are set to NULL\n");
    }

    if (VX_SUCCESS == status)
    {
        configuration = (vx_user_data_object)parameters[TIVX_KERNEL_RADAR_GTRACK_CONFIGURATION_IDX];
        sensor_config = (vx_user_data_object)parameters[TIVX_KERNEL_RADAR_GTRACK_SENSOR_CONFIG_IDX];
        radar_obj_data = (vx_user_data_object)parameters[TIVX_KERNEL_RADAR_GTRACK_RADAR_OBJ_DATA_IDX];
        reftr_and_pose = (vx_user_data_object)parameters[TIVX_KERNEL_RADAR_GTRACK_REFTR_AND_POSE_IDX];
        track_info = (vx_user_data_object)parameters[TIVX_KERNEL_RADAR_GTRACK_TRACK_INFO_IDX];
    }


    /* PARAMETER ATTRIBUTE FETCH */

    if (VX_SUCCESS == status)
    {
        tivxCheckStatus(&status, vxQueryUserDataObject(configuration, VX_USER_DATA_OBJECT_NAME, &configuration_name, sizeof(configuration_name)));
        tivxCheckStatus(&status, vxQueryUserDataObject(configuration, VX_USER_DATA_OBJECT_SIZE, &configuration_size, sizeof(configuration_size)));

        tivxCheckStatus(&status, vxQueryUserDataObject(sensor_config, VX_USER_DATA_OBJECT_NAME, &sensor_config_name, sizeof(sensor_config_name)));
        tivxCheckStatus(&status, vxQueryUserDataObject(sensor_config, VX_USER_DATA_OBJECT_SIZE, &sensor_config_size, sizeof(sensor_config_size)));

        tivxCheckStatus(&status, vxQueryUserDataObject(radar_obj_data, VX_USER_DATA_OBJECT_NAME, &radar_obj_data_name, sizeof(radar_obj_data_name)));
        tivxCheckStatus(&status, vxQueryUserDataObject(radar_obj_data, VX_USER_DATA_OBJECT_SIZE, &radar_obj_data_size, sizeof(radar_obj_data_size)));

        tivxCheckStatus(&status, vxQueryUserDataObject(reftr_and_pose, VX_USER_DATA_OBJECT_NAME, &reftr_and_pose_name, sizeof(reftr_and_pose_name)));
        tivxCheckStatus(&status, vxQueryUserDataObject(reftr_and_pose, VX_USER_DATA_OBJECT_SIZE, &reftr_and_pose_size, sizeof(reftr_and_pose_size)));

        tivxCheckStatus(&status, vxQueryUserDataObject(track_info, VX_USER_DATA_OBJECT_NAME, &track_info_name, sizeof(track_info_name)));
        tivxCheckStatus(&status, vxQueryUserDataObject(track_info, VX_USER_DATA_OBJECT_SIZE, &track_info_size, sizeof(track_info_size)));
    }

    /* PARAMETER CHECKING */

    if (VX_SUCCESS == status)
    {
        if ((configuration_size != sizeof(PTK_Alg_RadarGTrackParams)) ||
            (strncmp(configuration_name, "PTK_Alg_RadarGTrackParams", sizeof(configuration_name)) != 0))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            VX_PRINT(VX_ZONE_ERROR, "'configuration' should be a user_data_object of type:\n PTK_Alg_RadarGTrackParams \n");
        }

        if ((sensor_config_size != sizeof(PTK_Alg_RadarSensorConfig)) ||
            (strncmp(sensor_config_name, "PTK_Alg_RadarSensorConfig", sizeof(sensor_config_name)) != 0))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            VX_PRINT(VX_ZONE_ERROR, "'sensor_config' should be a user_data_object of type:\n PTK_Alg_RadarSensorConfig \n");
        }

        if ((radar_obj_data_size != sizeof(PTK_Alg_RadarDetOutput)) ||
            (strncmp(radar_obj_data_name, "PTK_Alg_RadarDetOutput", sizeof(radar_obj_data_name)) != 0))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            VX_PRINT(VX_ZONE_ERROR, "'radar_obj_data' should be a user_data_object of type:\n PTK_Alg_RadarDetOutput \n");
        }

        if ((reftr_and_pose_size != sizeof(PTK_InsPoseAndRef)) ||
            (strncmp(reftr_and_pose_name, "PTK_InsPoseAndRef", sizeof(reftr_and_pose_name)) != 0))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            VX_PRINT(VX_ZONE_ERROR, "'reftr_and_pose' should be a user_data_object of type:\n PTK_InsPoseAndRef \n");
        }

        if ((track_info_size != sizeof(PTK_Alg_RadarGTrackTargetInfo)) ||
            (strncmp(track_info_name, "PTK_Alg_RadarGTrackTargetInfo", sizeof(track_info_name)) != 0))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            VX_PRINT(VX_ZONE_ERROR, "'track_info' should be a user_data_object of type:\n PTK_Alg_RadarGTrackTargetInfo \n");
        }
    }


    /* CUSTOM PARAMETER CHECKING */

    /* < DEVELOPER_TODO: (Optional) Add any custom parameter type or range checking not */
    /*                   covered by the code-generation script.) > */

    /* < DEVELOPER_TODO: (Optional) If intending to use a virtual data object, set metas using appropriate TI API. */
    /*                   For a code example, please refer to the validate callback of the follow file: */
    /*                   tiovx/kernels/openvx-core/host/vx_absdiff_host.c. For further information regarding metas, */
    /*                   please refer to the OpenVX 1.1 spec p. 260, or search for vx_kernel_validate_f. > */

    return status;
}

static vx_status VX_CALLBACK tivxAddKernelRadarGtrackInitialize(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num_params)
{
    vx_status status = VX_SUCCESS;

    if ( (num_params != TIVX_KERNEL_RADAR_GTRACK_MAX_PARAMS)
        || (NULL == parameters[TIVX_KERNEL_RADAR_GTRACK_CONFIGURATION_IDX])
        || (NULL == parameters[TIVX_KERNEL_RADAR_GTRACK_SENSOR_CONFIG_IDX])
        || (NULL == parameters[TIVX_KERNEL_RADAR_GTRACK_RADAR_OBJ_DATA_IDX])
        || (NULL == parameters[TIVX_KERNEL_RADAR_GTRACK_REFTR_AND_POSE_IDX])
        || (NULL == parameters[TIVX_KERNEL_RADAR_GTRACK_TRACK_INFO_IDX])
    )
    {
        status = VX_ERROR_INVALID_PARAMETERS;
        VX_PRINT(VX_ZONE_ERROR, "One or more REQUIRED parameters are set to NULL\n");
    }
    return status;
}

vx_status tivxAddKernelRadarGtrack(vx_context context)
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
                    TIVX_KERNEL_RADAR_GTRACK_NAME,
                    kernel_id,
                    NULL,
                    TIVX_KERNEL_RADAR_GTRACK_MAX_PARAMS,
                    tivxAddKernelRadarGtrackValidate,
                    tivxAddKernelRadarGtrackInitialize,
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
    vx_radar_gtrack_kernel = kernel;

    return status;
}

vx_status tivxRemoveKernelRadarGtrack(vx_context context)
{
    vx_status status;
    vx_kernel kernel = vx_radar_gtrack_kernel;

    status = vxRemoveKernel(kernel);
    vx_radar_gtrack_kernel = NULL;

    return status;
}


