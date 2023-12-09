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
* Created     :	09/21/2021
* Revision    :	1.0
* Description :	C++ file for the c7x kernel that implements histogram function
* 
*		
***************************************************************************
* Revision History
* Ver	Date			Author		Description 
* 1.0	09/22/2021		VC		Initial version	
***************************************************************************/

/**
 * \file app_c7x_kernel_histo_kernel.c User Kernel implementation for C7x histogram function
 *
 *  This file shows a sample implementation of a user kernel function.
 *
 *  To implement a user kernel the below top level interface functions are implemented
 *  - app_c7x_kernel_histo_register() : Registers user kernel to OpenVX context
 *       - The implementation of this function has slight variation depending on the kernel implemented as
 *         as OpenVX user kernel or TIOVX target kernel
 *  - app_c7x_kernel_histo_unregister() : Un-Registers user kernel from OpenVX context
 *       - The implementation of this function is same for both OpenVX user kernel and TIOVX target kernel
 *  - app_c7x_kernel_histo_kernel_node(): Using the user kernel name, creates a OpenVX node within a graph
 *       - The implementation of this function is same for both OpenVX user kernel and TIOVX target kernel
 *
 *  When working with user kernel or target kernel,
 *  - app_c7x_kernel_histo_register() MUST be called after vxCreateContext() and
 *     before app_c7x_kernel_histo_kernel_node().
 *  - app_c7x_kernel_histo_unregister() MUST be called before vxReleaseContext() and
 *  - app_c7x_kernel_histo_kernel_node() is called to insert the user kernel node into a OpenVX graph
 *
 *  When working with target kernel, additional the target side implementation is done in file
 *  \ref app_c7x_target_kernel_histo.cpp
 *
 *  Follow the comments for the different functions in the file to understand how a user/target kernel is implemented.
 */

#include <stdio.h>
#include <VX/vx.h>
#include <TI/tivx.h>
#include <TI/j7.h>
#include "app_c7x_kernel.h"

static vx_status VX_CALLBACK app_c7x_kernel_histo_kernel_validate(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num,
            vx_meta_format metas[]);

/** \brief Handle to the registered user kernel [static global] */
static vx_kernel app_c7x_kernel_histo_kernel = NULL;

/** \brief Kernel ID of the registered user kernel. Used to create a node for the kernel function [static global] */
static vx_enum app_c7x_kernel_histo_kernel_id = 0;


/**
 * \brief Add user/target kernel to OpenVX context
 *
 * \param context [in] OpenVX context into with the user kernel is added
 */
vx_status app_c7x_kernel_histo_register(vx_context context)
{
    vx_kernel kernel = NULL;
    vx_status status;
    uint32_t index;

    /**
     * - Dynamically allocate a kernel ID and store it in  the global 'app_c7x_kernel_histo_kernel_id'
     *
     * This is used later to create the node with this kernel function
     * \code
     */
    status = vxAllocateUserKernelId(context, &app_c7x_kernel_histo_kernel_id);
    /** \endcode */
    if(status!=VX_SUCCESS)
    {
        printf(" ERROR: vxAllocateUserKernelId failed (%d)!!!\n", status);
    }
    if(status==VX_SUCCESS)
    {
        /**
         * - Register kernel to OpenVX context
         *
         * A kernel can be identified with its kernel ID (allocated above). \n
         * A kernel can also be identified with its unique kernel name string.
         * APP_C7X_KERNEL_HISTO_NAME defined in
         * file app_c7x_kernel.h in this case.
         *
         * When calling vxAddUserKernel(), additional callbacks are registered.
         * For target kernel, the 'run' callback is set to NULL.
         * Typically 'init' and 'deinit' callbacks are also set to NULL.
         * 'validate' callback is typically set
         *
         * \code
         */
        kernel = vxAddUserKernel(
                    context,
                    APP_C7X_KERNEL_HISTO_NAME,
                    app_c7x_kernel_histo_kernel_id,
                    NULL,
                    APP_C7X_HISTO_MAX_PARAMS, /* number of parameters objects for this user function */
                    app_c7x_kernel_histo_kernel_validate,
                    NULL,
                    NULL);

        status = vxGetStatus((vx_reference)kernel);
        /** \endcode */

        if ( status == VX_SUCCESS)
        {
            /**
             * - Add supported target's on which this target kernel can be run
             *
             * \code
             */
            tivxAddKernelTarget(kernel, TIVX_TARGET_DSP_C7_1);
            /** \endcode */
        }
    }
    /**
     * - Checking is kernel was added successfully to OpenVX context
     * \code
     */
    status = vxGetStatus((vx_reference)kernel);
    /** \endcode */
    if ( status == VX_SUCCESS)
    {
        /**
         * - Now define parameters for the kernel
         *
         *   When specifying the parameters, the below attributes of each parameter are specified,
         *   - parameter index in the function parameter list
         *   - the parameter direction: VX_INPUT or VX_OUTPUT
         *   - parameter data object type
         *   - paramater state: VX_PARAMETER_STATE_REQUIRED or VX_PARAMETER_STATE_OPTIONAL
         * \code
         */
        index = 0;

        status = vxAddParameterToKernel(kernel,
            index,
            VX_INPUT,
            VX_TYPE_USER_DATA_OBJECT,
            VX_PARAMETER_STATE_REQUIRED
            );
        index++;
        
        if ( status == VX_SUCCESS)
        {
        status = vxAddParameterToKernel(kernel,
            index,
            VX_INPUT,
            TIVX_TYPE_RAW_IMAGE,
            VX_PARAMETER_STATE_REQUIRED
            );
        index++;
        }

        if ( status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                index,
                VX_OUTPUT,
                VX_TYPE_DISTRIBUTION,
                VX_PARAMETER_STATE_REQUIRED
                );
            index++;
        }
        if ( status == VX_SUCCESS)
        {
        status = vxAddParameterToKernel(kernel,
            index,
            VX_OUTPUT,
            VX_TYPE_SCALAR,
            VX_PARAMETER_STATE_REQUIRED
            );
        index++;
        }
        if ( status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                index,
                VX_OUTPUT,
                VX_TYPE_SCALAR,
                VX_PARAMETER_STATE_REQUIRED
                );
            index++;
        }
        /** \endcode */
        /**
         * - After all parameters are defined, now the kernel is finalized, i.e it is ready for use.
         * \code
         */
        if ( status == VX_SUCCESS)
        {
            status = vxFinalizeKernel(kernel);
        }
        /** \endcode */
        if( status != VX_SUCCESS)
        {
            printf(" ERROR: vxAddParameterToKernel, vxFinalizeKernel failed (%d)!!!\n", status);
            vxReleaseKernel(&kernel);
            kernel = NULL;
        }
    }
    else
    {
        kernel = NULL;
        printf(" ERROR: vxAddUserKernel failed (%d)!!!\n", status);
    }

    if(status==VX_SUCCESS)
    {
        /**
         * - Set kernel handle to the global user kernel handle
         *
         * This global handle is used later to release the kernel when done with it
         * \code
         */
        app_c7x_kernel_histo_kernel = kernel;
        /** \endcode */
    }

    return status;
}


/**
 * \brief Remove user/target kernel from context
 *
 * \param context [in] OpenVX context from which the kernel will be removed
 */
vx_status app_c7x_kernel_histo_unregister(vx_context context)
{
    vx_status status;

    /**
     * - Remove user kernel from context and set the global 'app_c7x_kernel_histo_kernel' to NULL
     *
     * \code
     */
    status = vxRemoveKernel(app_c7x_kernel_histo_kernel);
    app_c7x_kernel_histo_kernel = NULL;
    /** \endcode */

    if(status!=VX_SUCCESS)
    {
        printf(" Unable to remove kernel (%d)!!!\n", status);
    }

    return status;
}

/**
 *  \brief User/target kernel validate function
 *
 *  This function gets called during vxGraphVerify.
 *  The node which will runs the kernel, parameter references
 *  are passed as input to this function.
 *  This function checks the parameters to make sure all attributes of the parameter are as expected.
 *  ex, data format checks, image width, height relations between input and output.
 *
 *  \param node [in] OpenVX node which will execute the kernel
 *  \param parameters [in] Parameters references for this kernel function
 *  \param num [in] Number of parameter references
 *  \param metas [in/out] Meta references update with attribute values
 *
 *  \return VX_SUCCESS if validate is successful, else appropiate error code
 */
static vx_status VX_CALLBACK app_c7x_kernel_histo_kernel_validate(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num,
            vx_meta_format metas[])
{
    vx_status status = VX_SUCCESS;
    vx_enum ref_type;
    vx_uint32 i;
    vx_size distribution_bins;
    vx_uint32 distribution_range;
    vx_int32 distribution_offset;

    if (num != APP_C7X_HISTO_MAX_PARAMS)
    {
        printf(" ERROR: Number of parameters dont match !!!\n");
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    for (i = 0U; i < APP_C7X_HISTO_MAX_PARAMS; i ++)
    {
        /* Check for NULL */
        if (NULL == parameters[i])
        {
            printf(" ERROR: Parameter %d is NULL !!!\n", i);
            status = VX_ERROR_NO_MEMORY;
            break;
        }
    }
    if (VX_SUCCESS == status)
    {
        /* Get the image width/heigh and format */
        status = vxQueryReference(parameters[APP_C7X_HISTO_IN_RAW_IMG_IDX],
             (vx_enum)VX_REFERENCE_TYPE, &ref_type, sizeof(ref_type));
        if(status!=VX_SUCCESS)
        {
            printf(" ERROR: Unable to query input image !!!\n");
        }
    }
    if (VX_SUCCESS == status)
    {
        /* Get the image width/heigh and format */
        status = vxQueryDistribution((vx_distribution)parameters[APP_C7X_HISTO_OUT_DISTRIBUTION_IDX],
            (vx_enum)VX_DISTRIBUTION_BINS, &distribution_bins, sizeof(distribution_bins));
        status |= vxQueryDistribution((vx_distribution)parameters[APP_C7X_HISTO_OUT_DISTRIBUTION_IDX],
            (vx_enum)VX_DISTRIBUTION_RANGE, &distribution_range, sizeof(distribution_range));
        status |= vxQueryDistribution((vx_distribution)parameters[APP_C7X_HISTO_OUT_DISTRIBUTION_IDX],
            (vx_enum)VX_DISTRIBUTION_OFFSET, &distribution_offset, sizeof(distribution_offset));

        if(status!=VX_SUCCESS)
        {
            printf(" ERROR: Unable to query output histogram !!!\n");
        }
    }

    if (VX_SUCCESS == status)
    {
        /* Check for validity of data format */
        if (TIVX_TYPE_RAW_IMAGE != ref_type)
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            printf(" C7x Histogram kernel ERROR: Input image format not correct !!!\n");
        }
        if (distribution_bins > distribution_range) 
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            printf(" C7x Histogram kernel ERROR: Histogram number of bins %u greater than range %d !!!\n", (uint32_t)distribution_bins, distribution_range);
        }
        if (distribution_offset != 0) 
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            printf(" C7x Histogram kernel ERROR: Histogram's distribution offset not equal to 0 !!!\n");
        }

    }

    if (VX_SUCCESS == status)
    {
        vx_enum scalar_type;
        vxQueryScalar((vx_scalar)parameters[APP_C7X_HISTO_OUT_MEAN_IDX], (vx_enum)VX_SCALAR_TYPE, &scalar_type, sizeof(scalar_type));
        if ((vx_enum)VX_TYPE_UINT32 != scalar_type)
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            printf(" C7x Histogram kernel ERROR: mean scalar type not uint32 !!!\n");
        }
        vxQueryScalar((vx_scalar)parameters[APP_C7X_HISTO_OUT_SD_IDX], (vx_enum)VX_SCALAR_TYPE, &scalar_type, sizeof(scalar_type));
        if ((vx_enum)VX_TYPE_UINT32 != scalar_type)
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            printf(" C7x Histogram kernel ERROR: mean scalar type not uint32 !!!\n");
        }
    }

    return status;
}

/**
 *  \brief User/target kernel node create function
 *
 *  Given a graph reference, this function creates a OpenVX node and inserts it into the graph.
 *  The list of parameter references is also provided as input.
 *  Exact data type are used instead of base class references to allow some level
 *  of compile time type checking.
 *  In this example, there is two input image and one output image that are passed as parameters.
 *
 *  \param graph [in] OpenVX graph
 *  \param in1 [in] Input image 1 reference
 *  \param in2 [in] Input image 2 reference
 *  \param out [in] Output image reference
 *
 *  \return OpenVX node that is created and inserted into the graph
 */
vx_node app_c7x_kernel_histo_kernel_node(vx_graph graph, vx_user_data_object config, tivx_raw_image in, vx_distribution out, vx_scalar mean, vx_scalar sd)
{
    vx_node node;
    /**
     * - Put parameters into a array of references
     * \code
     */
    vx_reference refs[] = {(vx_reference)config, (vx_reference)in, (vx_reference)out, (vx_reference)mean, (vx_reference)sd};
    /** \endcode */

    /**
     * - Use TIOVX API to make a node using the graph, kernel ID, and parameter reference array as input
     * \code
     */
    node = tivxCreateNodeByKernelName(graph,
                APP_C7X_KERNEL_HISTO_NAME,
                refs, sizeof(refs)/sizeof(refs[0])
                );
    vxSetReferenceName((vx_reference)node, "APP_C7X_HISTO");
    /** \endcode */

    return node;
}


