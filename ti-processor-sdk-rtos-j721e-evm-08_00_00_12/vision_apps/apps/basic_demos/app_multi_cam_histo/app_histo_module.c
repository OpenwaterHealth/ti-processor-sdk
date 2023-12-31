/**************************************************************************
*  Copyright 2021   Neither this file nor any part of it may be used for 
*     any purpose without the written consent of D3 Engineering, LLC.
*     All rights reserved.
*				
*				D 3   E N G I N E E R I N G
*
* File Name   :	app_histo_module.c
* Release     :	1.0
* Author      :	D3 Engineering Technical Staff
* Created     :	09/21/2021
* Revision    :	1.0
* Description :	Implementation for the c7x histogram module
* 
*		
***************************************************************************
* Revision History
* Ver	Date			Author		Description 
* 1.0	09/21/2021		VC		Initial version	
***************************************************************************/

#include "app_histo_module.h"
#include "app_c7x_kernel.h"
#include "app_common.h"

#define HISTO_DEBUG

#ifdef HISTO_DEBUG
#define DEBUG_PRINTF(f_, ...) printf((f_), ##__VA_ARGS__)
#else
#define DEBUG_PRINTF(f_, ...)
#endif

vx_status app_init_histo(vx_context context, HistoObj *histoObj, SensorObj *sensorObj, char *objName, vx_int32 bufq_depth, uint32_t num_histo_bins)
{
    vx_int32 q;
    vx_status status = VX_SUCCESS;
    IssSensor_CreateParams *sensorParams = &sensorObj->sensorParams;

    histoObj->config = vxCreateUserDataObject(context, "histoConfig", sizeof(tivxHistoParams), NULL);
    status = vxGetStatus((vx_reference)histoObj->config);

    if (status == VX_SUCCESS)
    {
        status = vxCopyUserDataObject(histoObj->config, 0, sizeof(tivxHistoParams),
                                      &histoObj->params, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
    }

    histoObj->num_channels = sensorObj->num_cameras_enabled;
    histoObj->range = (1 << (sensorParams->sensorInfo.raw_params.format[0].msb + 1));
    if (num_histo_bins > histoObj->range)
    {
        printf("Number of bins %d passed as parameter greater than sensor's dynamic range. Setting number of bins to sensor's range=%d", num_histo_bins, histoObj->range);
        histoObj->numBins = histoObj->range;
    }
    else
    {
        histoObj->numBins= num_histo_bins;
    }
    
    vx_distribution histoOut = vxCreateDistribution(context, histoObj->numBins, 0, histoObj->range);

    status = vxGetStatus((vx_reference)histoOut);

    if (status != VX_SUCCESS)
    {
        printf("<<<<Histo Module Error>>> histoOut creation error\n");
    }
    else
    {
        DEBUG_PRINTF("<<<<Histo Module>>> histoOut creation successful, range= %d, #bins = %d\n", histoObj->range, histoObj->numBins);
        for (q = 0; q < bufq_depth; q++)
        {
            histoObj->outHistoArr[q] = vxCreateObjectArray(context, (vx_reference)histoOut, sensorObj->num_cameras_enabled);
            histoObj->outHistoCh0[q] = (vx_distribution)vxGetObjectArrayItem((vx_object_array)histoObj->outHistoArr[q], 0);
        }
        vxReleaseDistribution(&histoOut);
        vxSetReferenceName((vx_reference)histoObj->outHistoArr[0], "Histogram output");
    }

    vx_uint32 zero = 0;

    vx_scalar scalarOut = vxCreateScalar(context, VX_TYPE_UINT32, &zero);

    status = vxGetStatus((vx_reference)scalarOut);

    if (status != VX_SUCCESS)
    {
        printf("<<<<Histo Module Error>>> mean scalar creation error\n");
    }
    else
    {
        DEBUG_PRINTF("<<<<Histo Module>>> mean scalar creation successful\n");
        for (q = 0; q < bufq_depth; q++)
        {
            histoObj->outMeanArr[q] = vxCreateObjectArray(context, (vx_reference)scalarOut, sensorObj->num_cameras_enabled);
            histoObj->outMeanCh0[q] = (vx_scalar)vxGetObjectArrayItem((vx_object_array)histoObj->outMeanArr[q], 0);
        }
        vxReleaseScalar(&scalarOut);
        vxSetReferenceName((vx_reference)histoObj->outMeanArr[0], "Mean output");
    }

    {
        vx_uint32 zero = 0;

        vx_scalar scalarOut = vxCreateScalar(context, VX_TYPE_UINT32, &zero);
        status = vxGetStatus((vx_reference)scalarOut);

        if (status != VX_SUCCESS)
        {
            printf("<<<<Histo Module Error>>> sd scalar creation error\n");
        }
        else
        {
            DEBUG_PRINTF("<<<<Histo Module>>> sd scalar creation successful\n");
            for (q = 0; q < bufq_depth; q++)
            {
                histoObj->outSdArr[q] = vxCreateObjectArray(context, (vx_reference)scalarOut, sensorObj->num_cameras_enabled);
                histoObj->outSdCh0[q] = (vx_scalar)vxGetObjectArrayItem((vx_object_array)histoObj->outSdArr[q], 0);
            }
            vxReleaseScalar(&scalarOut);
            vxSetReferenceName((vx_reference)histoObj->outSdArr[0], "Sd output");
        }
    }

    {
        char file_path[TIVX_FILEIO_FILE_PATH_LENGTH];
        char file_prefix[TIVX_FILEIO_FILE_PREFIX_LENGTH];

        strcpy(file_path, histoObj->output_file_path);
        histoObj->file_path = vxCreateArray(context, VX_TYPE_UINT8, TIVX_FILEIO_FILE_PATH_LENGTH);
        status = vxGetStatus((vx_reference)histoObj->file_path);
        if (status == VX_SUCCESS)
        {
            vxAddArrayItems(histoObj->file_path, TIVX_FILEIO_FILE_PATH_LENGTH, &file_path[0], 1);
        }
        else
        {
            printf("[HISTO-MODULE] Unable to create file path object for storing HISTO outputs! \n");
        }

        strcpy(file_prefix, "histo_output");
        histoObj->file_prefix = vxCreateArray(context, VX_TYPE_UINT8, TIVX_FILEIO_FILE_PREFIX_LENGTH);
        status = vxGetStatus((vx_reference)histoObj->file_prefix);
        if (status == VX_SUCCESS)
        {
            vxAddArrayItems(histoObj->file_prefix, TIVX_FILEIO_FILE_PREFIX_LENGTH, &file_prefix[0], 1);
        }
        else
        {
            printf("[HISTO-MODULE] Unable to create file prefix object for storing HISTO outputs! \n");
        }

        histoObj->write_cmd = vxCreateUserDataObject(context, "tivxFileIOHistoWriteCmd", sizeof(tivxFileIOHistoWriteCmd), NULL);
        status = vxGetStatus((vx_reference)histoObj->write_cmd);
        if (status != VX_SUCCESS)
        {
            printf("[HISTO-MODULE] Unable to create fileio write cmd object for HISTO node! \n");
        }

        histoObj->cmd = vxCreateUserDataObject(context, "tivxHistoCmd", sizeof(tivxHistoCmd), NULL);
        status = vxGetStatus((vx_reference)histoObj->cmd);
        if (status != VX_SUCCESS)
        {
            printf("[HISTO-MODULE] Unable to create cmd object for HISTO node! \n");
        }

        histoObj->time_sync = vxCreateUserDataObject(context, "tivxHistoTimeSync", sizeof(tivxHistoTimeSync), NULL);
        status = vxGetStatus((vx_reference)histoObj->time_sync);
        if (status != VX_SUCCESS)
        {
            printf("[HISTO-MODULE] Unable to create time sync object for HISTO node! \n");
        }

    }

    return status;
}

void app_deinit_histo(HistoObj *histoObj, int32_t bufq_depth)
{
    int32_t q;

    vxReleaseUserDataObject(&histoObj->config);

    for (q = 0; q < bufq_depth; q++)
    {
        vxReleaseObjectArray(&histoObj->outHistoArr[q]);
        vxReleaseDistribution(&histoObj->outHistoCh0[q]);
        vxReleaseObjectArray(&histoObj->outMeanArr[q]);
        vxReleaseScalar(&histoObj->outMeanCh0[q]);
        vxReleaseObjectArray(&histoObj->outSdArr[q]);
        vxReleaseScalar(&histoObj->outSdCh0[q]);
    }
    vxReleaseArray(&histoObj->file_path);
    vxReleaseArray(&histoObj->file_prefix);
    vxReleaseUserDataObject(&histoObj->write_cmd);
    vxReleaseUserDataObject(&histoObj->cmd);
    vxReleaseUserDataObject(&histoObj->time_sync);
}

void app_delete_histo(HistoObj *histoObj)
{
    if (histoObj->node != NULL)
    {
        vxReleaseNode(&histoObj->node);
    }

    if(histoObj->write_node != NULL)
    {
        vxReleaseNode(&histoObj->write_node);
    }
}

vx_status app_create_graph_histo(vx_graph graph,
                           HistoObj *histoObj,
                           vx_object_array inputRawImageArr)
{
    vx_status status = VX_SUCCESS;
    tivx_raw_image inImage;
    vx_distribution outHisto;
    vx_scalar outMean, outSd;

    inImage  = (tivx_raw_image)vxGetObjectArrayItem((vx_object_array)inputRawImageArr, 0);
    outHisto = (vx_distribution)vxGetObjectArrayItem((vx_object_array)histoObj->outHistoArr[0], 0);
    outMean = (vx_scalar)vxGetObjectArrayItem((vx_object_array)histoObj->outMeanArr[0], 0);
    outSd = (vx_scalar)vxGetObjectArrayItem((vx_object_array)histoObj->outSdArr[0], 0);

    histoObj->node = app_c7x_kernel_histo_kernel_node(graph,
                                                    histoObj->config,
                                                    inImage,
                                                    outHisto,
                                                    outMean,
                                                    outSd);

    APP_ASSERT_VALID_REF(histoObj->node);

    if(status == VX_SUCCESS)
    {
        status = vxSetReferenceName((vx_reference)histoObj->node, "HistogramNode");
    }
    vx_bool replicate[] = {vx_false_e, vx_true_e, vx_true_e, vx_true_e, vx_true_e};
    vxReplicateNode(graph, histoObj->node, replicate, 5);
    vxReleaseReference((vx_reference*)&inImage);
    status = vxReleaseDistribution(&outHisto);
    status |= vxReleaseScalar(&outMean);
    status |= vxReleaseScalar(&outSd);

    if (status == VX_SUCCESS)
    {
        status = app_create_graph_histo_write_output(graph, histoObj);
    }

    return(status);
}

vx_status app_send_cmd_histo_enableChan(HistoObj *histoObj, uint32_t enableChanBitFlag)
{
    vx_status status = VX_SUCCESS;

    tivxHistoCmd cmd;

    cmd.enableChanBitFlag = enableChanBitFlag;

    status = vxCopyUserDataObject(histoObj->cmd, 0, sizeof(tivxHistoCmd),\
                  &cmd, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

    if(status == VX_SUCCESS)
    {
        vx_reference refs[2];

        refs[0] = (vx_reference)histoObj->cmd;

        status = tivxNodeSendCommand(histoObj->node, TIVX_CONTROL_CMD_SEND_TO_ALL_REPLICATED_NODES,
                                 TIVX_C7X_HISTO_CMD_ENABLE_CHAN,
                                 refs, 1u);

        if(VX_SUCCESS != status)
        {
            printf("Histo Node send command failed!\n");
        }

        APP_PRINTF("Histo Node send command success!\n");
    }
    return (status);
}

vx_status app_create_graph_histo_write_output(vx_graph graph, HistoObj *histoObj)
{
    vx_status status = VX_SUCCESS;

    vx_distribution outHisto = (vx_distribution)vxGetObjectArrayItem(histoObj->outHistoArr[0], 0);
    vx_scalar outMean = (vx_scalar)vxGetObjectArrayItem((vx_object_array)histoObj->outMeanArr[0], 0);
    vx_scalar outSd = (vx_scalar)vxGetObjectArrayItem((vx_object_array)histoObj->outSdArr[0], 0);
    histoObj->write_node = tivxWriteHistogramNode(graph, outHisto, outMean, outSd, histoObj->file_path, histoObj->file_prefix);

    status = vxGetStatus((vx_reference)histoObj->write_node);
    if(status == VX_SUCCESS)
    {
        vxSetNodeTarget(histoObj->write_node, VX_TARGET_STRING, TIVX_TARGET_A72_0);
        vxSetReferenceName((vx_reference)histoObj->write_node, "histo_write_node");

        vx_bool replicate[] = { vx_true_e, vx_true_e, vx_true_e, vx_false_e, vx_false_e};
        vxReplicateNode(graph, histoObj->write_node, replicate, 5);
    }
    else
    {
        printf("[HISTO-MODULE] Unable to create node to write HISTO output! \n");
    }

    vxReleaseDistribution(&outHisto);
    vxReleaseScalar(&outMean);
    vxReleaseScalar(&outSd);

    return (status);
}

vx_status app_send_cmd_histo_write_node(HistoObj *histoObj, vx_uint32 enableChanBitFlag, vx_uint32 start_frame, vx_uint32 num_frames, vx_uint32 num_skip, vx_int32 cam_pair, vx_int32 scan_type)
{
    vx_status status = VX_SUCCESS;

    tivxFileIOHistoWriteCmd write_cmd;

    write_cmd.start_frame = start_frame;
    write_cmd.num_frames = num_frames;
    write_cmd.num_skip = num_skip;
    write_cmd.cam_pair = cam_pair;
    write_cmd.scan_type = scan_type;
    write_cmd.enableChanBitFlag= enableChanBitFlag;
    
    status = vxCopyUserDataObject(histoObj->write_cmd, 0, sizeof(tivxFileIOHistoWriteCmd),\
                  &write_cmd, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

    if(status == VX_SUCCESS)
    {
        vx_reference refs[2];

        refs[0] = (vx_reference)histoObj->write_cmd;

        status = tivxNodeSendCommand(histoObj->write_node, TIVX_CONTROL_CMD_SEND_TO_ALL_REPLICATED_NODES,
                                 TIVX_FILEIO_CMD_SET_FILE_WRITE,
                                 refs, 1u);

        if(VX_SUCCESS != status)
        {
            printf("Histo Write Node send command failed!\n");
        }

        APP_PRINTF("Histo Write node send command success!\n");
    }
    return (status);
}