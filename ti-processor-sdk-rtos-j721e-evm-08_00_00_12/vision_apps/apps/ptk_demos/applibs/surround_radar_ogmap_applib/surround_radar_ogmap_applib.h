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
#ifndef _SURROUND_RADAR_OGMAP_APPLIB_H_
#define _SURROUND_RADAR_OGMAP_APPLIB_H_

#include <TI/tivx.h>
#include <TI/tivx_debug.h>
#include <TI/tivx_mutex.h>
#include <TI/j7.h>
#include <TI/tivx_park_assist.h>

/**
 * \defgroup group_applib_surround_radar_ogmap Surround Radar OGMAP APPLIB code.
 * \ingroup group_ptk_applib
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Constant for maximum pipeline depth.
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
#define RADAROGAPPLIB_PIPELINE_DEPTH        (PTK_ALG_RADAR_NUM_SENSORS)

/**
 * \brief Constant for default core mapping for the nodes.
 * \ingroup group_applib_surround_radar_ogmap
 */
#define RADAROGAPPLIB_DEFAULT_CORE_MAPPING  (TIVX_TARGET_A72_0)

/* Forward declaration. */
struct RADAROGAPPLIB_Context;

typedef struct RADAROGAPPLIB_Context * RADAROGAPPLIB_Handle;

/**
 * \brief Surround Radar OGMAP APPLIB create parameter context.
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
typedef struct
{
    /** OpenVX references */
    vx_context                      vxContext;

    /** Flag for representing vehicle potision. */
    uint32_t                        ogFlagEgo;

    /** OGMAP and PFSD configuration parameters. */
    tivx_radar_ogmap_pfsd_params_t  ogPfsdCfg;

    /** OG Node Core mapping. */
    const char                    * ogNodeCore;

    /** Base value to be used for any programmed VX events. */
    uint32_t                        vxEvtAppValBase;

    /** Pipeline depth. */
    uint8_t                         pipelineDepth;

    /** Flag to indicate if the graph should be exported
     * 0 - disable
     * 1 - enable
     */
    uint8_t                         exportGraph;

    /** Real-time logging enable.
     * 0 - disable
     * 1 - enable
     */
    uint8_t                         rtLogEnable;

} RADAROGAPPLIB_createParams;

/**
 * \brief Output buffer descriptor.
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
typedef struct
{
    /** Timestamp corresponding to the input that generated this output. */
    uint64_t                ts;

    /** Output Accumulated map. */
    PTK_Map                *outAccMap;

    /** PFSD output descriptor. */
    vx_user_data_object     vxPfsdOutDesc;

    /** Output instantaneous map object. */
    vx_user_data_object     vxOutInstMap;

} RADAROGAPPLIB_OutputBuff;

/**
 * \brief Function to initialize the APPLIB.
 *
 * \param [in] createParams APPLIB create parameters.
 *
 * \return Valid handle on success. NULL otherwise.
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
RADAROGAPPLIB_Handle RADAROGAPPLIB_create(
        RADAROGAPPLIB_createParams *createParams);

/**
 * \brief Function to set the reference frame for mapping.
 *
 * \param [in,out] handle APPLIB handle.
 *
 * \param [in] ecef_w Transform to map to world frame.
 *
 * \return VX_SUCCESS on success
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
vx_status RADAROGAPPLIB_setWorldReference(
        RADAROGAPPLIB_Handle        handle,
        const PTK_RigidTransform_d *ecef_w);

/**
 * \brief Function to process a given sensor frame.
 *
 * \param [in,out] handle APPLIB handle.
 *
 * \param [in] data Sensor data frame.
 *
 * \param [in] sensorId Id of the sensor the data belongs to.
 *
 * \param [in] timestamp Timestamp of the data frame.
 *
 * \return VX_SUCCESS on success
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
vx_status RADAROGAPPLIB_process(
        RADAROGAPPLIB_Handle    handle,
        uint8_t                *data,
        uint32_t                sensorId,
        uint64_t                timestamp);

/**
 * \brief Function to de-init the APPLIB and release the memory associated with
 *        the handle.
 *
 * \param [in,out] handle Reference to APPLIB handle.
 *
 * \return VX_SUCCESS on success
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
vx_status RADAROGAPPLIB_delete(
        RADAROGAPPLIB_Handle *handle);

/**
 * \brief Function to return a pointer to the accumulated OGMAP.
 *
 * \param [in] handle APPLIB handle.
 *
 * \return A valid map on success. NULL, otherwise.
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
PTK_Map* RADAROGAPPLIB_getOutAccMap(
        RADAROGAPPLIB_Handle    handle);

/**
 * \brief Function to reset the performance and other APPLIB specific counters.
 *
 * \param [in] handle APPLIB handle.
 *
 * \return VX_SUCCESS on success
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
vx_status RADAROGAPPLIB_reset(
        RADAROGAPPLIB_Handle    handle);

/**
 * \brief Function to returns references to the output objects at the head
 *        of the output queue. The data is not popped out of the queue.
 *        If the caller calls this functions repeatedly without calling
 *        RADAROGAPPLIB_releaseOutBuff() in between, then the same output
 *        is returned.
 *
 *        Once the graph execution is complete, the output buffer is stored in 
 *        a queue for the use by the calling application. The caller can use
 *        this API to get a reference to the input/output image pair for post
 *        processing. One the caller is done with the input/output pair, a call
 *        must be made to RADAROGAPPLIB_releaseOutBuff()for releasing the
 *        buffered input/output pair. Failing to do will result in subsequent
 *        graph execution failures due to non-availability of the output
 *        buffers.
 *
 * \param [in] handle APPLIB handle.
 *
 * \param [out] buff Output buffer context.
 *
 * \return VX_SUCCESS on success
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
vx_status RADAROGAPPLIB_getOutBuff(
        RADAROGAPPLIB_Handle        handle,
        RADAROGAPPLIB_OutputBuff   *buff);

/**
 * \brief Function to release the output buffer at the head of the output queue.
 *        Once the graph execution is complete, the output buffer is stored in 
 *        a queue for the use by the calling application. The caller is 
 *        responsible for calling this function to release the output buffers
 *        in a timely manner. Failing to do will result in subsequent graph
 *        execution failures due to non-availability of the output buffers.
 *
 * \param [in] handle APPLIB handle.
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
vx_status RADAROGAPPLIB_releaseOutBuff(
        RADAROGAPPLIB_Handle    handle);

/**
 * \brief Function to return the size of the accumulated OGMAP in bytes.
 *
 * \param [in] handle APPLIB handle.
 *
 * \return Size of the map in bytes, if successful. 0, otherwise.
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
uint32_t RADAROGAPPLIB_getOutAccMapSize(
        RADAROGAPPLIB_Handle    handle);

/**
 * \brief Function to print the performance statistics to stdout.
 *
 * \param [in] handle APPLIB handle.
 *
 * \return VX_SUCCESS on success
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
vx_status RADAROGAPPLIB_printStats(
        RADAROGAPPLIB_Handle    handle);

/**
 * \brief Function to export the performance statistics to a file.
 *
 * \param [in] handle APPLIB handle.
 *
 * \param [in,out] fp File handle.
 *
 * \param [in] exportAll Flag to indicate if the memory usage statistics need
 *                       to be exported as well. If this flag is false, then
 *                       only the graph level performance stats will be
 *                       exported.
 *
 * \return VX_SUCCESS on success
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
vx_status RADAROGAPPLIB_exportStats(
        RADAROGAPPLIB_Handle    handle,
        FILE                   *fp,
        bool                    exportAll);

/**
 * \brief Function to wait for the pending graph execution completions. This is
 *        relavant only if the graph is created and managed by the APPLIB.
 *
 *        If the graph is created and manged by the APPLIB, then this function
 *        could potentially block until the current graph execution is
 *        is complete.
 *
 *        If the graph is created and managed by the caller, then this function
 *        always returns immediately with status VX_SUCCESS.
 *
 * \param [in] handle APPLIB handle.
 *
 * \return VX_SUCCESS on success
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
vx_status RADAROGAPPLIB_waitGraph(
        RADAROGAPPLIB_Handle    handle);

/**
 * \brief Function for processing the events programmed to be handled by the
 *        APPLIB. Currently only VX_EVENT_GRAPH_COMPLETED event is handled.
 *
 *        The caller must invoke this handler when it receives a graph
 *        completion event. The APPLIB needs to deueue the graph parameters
 *        and buffer the output for use by the caller and these tasks are
 *        performed inside this handler.
 *
 * \param [in] handle APPLIB handle.
 *
 * \param [in] event Event context.
 *
 * \return VX_SUCCESS on success
 *
 * \ingroup group_applib_surround_radar_ogmap
 */
vx_status RADAROGAPPLIB_processEvent(
        RADAROGAPPLIB_Handle    handle,
        vx_event_t             *event);

#ifdef __cplusplus
}
#endif

#endif /* _SURROUND_RADAR_OGMAP_APPLIB_H_ */

