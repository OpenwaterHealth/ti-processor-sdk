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

#include <stdio.h>
#include <errno.h>
#include <VX/vx.h>
#include <TI/tivx.h>
#include <TI/j7.h>
#include <TI/tivx_fileio.h>
#include <TI/tivx_target_kernel.h>
#include "tivx_kernels_target_utils.h"

#include <tivx_fileio_write_histogram_host.h>

#include <headSensorService.h>
#include <app_ipc.h>
#include <app_mem.h>
#include <app_remote_service.h>
#include <ow_common.h>

#include <time.h>

#define NUM_CHANNELS 6
#define WRITE_FILE_OUTPUT
#define HISTO_BUFFER_SIZE 2476800 // 40 fps x 15 sec x (4096 + 32)
#define STATS_CH_LENGTH 600 // 40 fps x 15 sec x 1 channels ( 4 bytes(float) )

typedef struct {
    tivxFileIOHistoWriteCmd cmd;
    vx_uint32 frame_counter;
    vx_uint32 skip_counter;
    vx_int32 cam_pair;
    vx_int32 scan_type;
    vx_uint32 ch_num;

} tivxWriteHistogramParams;

static tivx_target_kernel vx_write_histogram_target_kernel = NULL;

static vx_int32 inst_id = 0;

static uint8_t ch_buffer[NUM_CHANNELS][HISTO_BUFFER_SIZE];
static float meanArr[NUM_CHANNELS][STATS_CH_LENGTH];
static float contrastArr[NUM_CHANNELS][STATS_CH_LENGTH];

static uint8_t* pBufferCH[NUM_CHANNELS] = { NULL };
static uint32_t bufferCH_size[NUM_CHANNELS] = {0};
static uint32_t CH_framecount[NUM_CHANNELS] = {0};
static int bWriteComplete_CH[NUM_CHANNELS] = {0};
static uint32_t scanMeanCH[NUM_CHANNELS] = {0};

static int count_bits_enabled(uint8_t register_val) {
    int count = 0;
    for (int i = 0; i < 8; i++) {
        if ((register_val & (1 << i)) != 0) {
            count++;
        }
    }
    return count;
}

void findMinMaxForStatArray(float *stat, float *minMax, uint32_t numFrames)
{
    uint32_t ind;
    minMax[0] = minMax[1] = stat[0];
    for( ind=0; ind<numFrames; ++ind )
    {
        if(stat[ind]<minMax[0])
            minMax[0] = stat[ind];
        if(stat[ind]>minMax[1])
            minMax[1] = stat[ind];
    }
    return;
}

static vx_status tivxKernelWriteHistogramCmd
(
    tivxWriteHistogramParams *prms,
    const tivx_obj_desc_user_data_object_t *usr_data_obj
)
{
    vx_status status = (vx_status)VX_SUCCESS;

    if (NULL != usr_data_obj)
    {
        void  *target_ptr = tivxMemShared2TargetPtr(&usr_data_obj->mem_ptr);

        tivxMemBufferMap(target_ptr, usr_data_obj->mem_size,
            (vx_enum)VX_MEMORY_TYPE_HOST, (vx_enum)VX_READ_ONLY);

        if (sizeof(tivxFileIOHistoWriteCmd) ==  usr_data_obj->mem_size)
        {
            memcpy(&prms->cmd, target_ptr, sizeof(tivxFileIOHistoWriteCmd));
        }
        
        prms->skip_counter = 0;
        tivxMemBufferUnmap(target_ptr, usr_data_obj->mem_size,
            (vx_enum)VX_MEMORY_TYPE_HOST, (vx_enum)VX_READ_ONLY);
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR,
            "tivxKernelWriteHistogramCmd: Data Object is NULL\n");
        status = (vx_status)VX_ERROR_INVALID_PARAMETERS;
    }

    return (status);
}

static vx_status VX_CALLBACK tivxKernelWriteHistogramControl
(
    tivx_target_kernel_instance kernel,
    uint32_t node_cmd_id,
    tivx_obj_desc_t *obj_desc[],
    uint16_t num_params,
    void *priv_arg
)
{
    vx_status status = VX_SUCCESS;

    tivxWriteHistogramParams *prms = NULL;

    if (NULL == obj_desc[TIVX_KERNEL_WRITE_HISTOGRAM_INPUT_DISTRIBUTION_IDX])
    {
        printf("Input Image handle is NULL!\n");
        status = VX_FAILURE;
    }

    if(status==VX_SUCCESS)
    {
        uint32_t size;

        status = tivxGetTargetKernelInstanceContext(kernel,
            (void **)&prms, &size);
        if ((VX_SUCCESS != status) || (NULL == prms) ||
            (sizeof(tivxWriteHistogramParams) != size))
        {
            status = VX_FAILURE;
        }
    }

    switch (node_cmd_id)
    {
        case TIVX_FILEIO_CMD_SET_FILE_WRITE:
        {
            tivxKernelWriteHistogramCmd(prms,
                (tivx_obj_desc_user_data_object_t *)obj_desc[0U]);
            break;
        }
        default:
        {
            VX_PRINT(VX_ZONE_ERROR,
                "tivxKernelWriteHistogramControl: Invalid Command Id\n");
            status = (vx_status)VX_FAILURE;
            break;
        }
    }

    return (status);
}

static vx_status VX_CALLBACK tivxKernelWriteHistogramCreate
(
  tivx_target_kernel_instance kernel,
  tivx_obj_desc_t *obj_desc[],
  vx_uint16 num_params,
  void *priv_arg
)
{

  tivxWriteHistogramParams * prms = NULL;

  prms = tivxMemAlloc(sizeof(tivxWriteHistogramParams), TIVX_MEM_EXTERNAL);
  if(prms == NULL)
  {
      printf("Unable to allcate memory for tivxWriteHistogramParams");
      return VX_FAILURE;
  }
  prms->ch_num          = inst_id;
  prms->frame_counter   = 0;
  prms->skip_counter    = 0;
  prms->cmd.start_frame = -1;
  prms->cmd.num_frames  = 0;
  prms->cmd.num_skip    = 0;
  prms->cmd.cam_pair    = 0;
  prms->cmd.scan_type   = 0;
  prms->cmd.enableChanBitFlag= 3;
  prms->cmd.virt_ptr = 0;

  inst_id++;
  tivxSetTargetKernelInstanceContext(kernel, prms,  sizeof(tivxWriteHistogramParams));

  return(VX_SUCCESS);
}

static vx_status VX_CALLBACK tivxKernelWriteHistogramDelete(
    tivx_target_kernel_instance kernel, tivx_obj_desc_t *obj_desc[],
    vx_uint16 num_params, void *priv_arg)
{
    vx_status status = VX_SUCCESS;

    if (NULL == obj_desc[TIVX_KERNEL_WRITE_HISTOGRAM_INPUT_DISTRIBUTION_IDX])
    {
        printf("Input Image handle is NULL!\n");
        status = VX_FAILURE;
    }

    if (VX_SUCCESS == status)
    {
        uint32_t size;
        tivxWriteHistogramParams *prms = NULL;

        status = tivxGetTargetKernelInstanceContext(kernel,
            (void **)&prms, &size);

        tivxMemFree(prms, sizeof(tivxWriteHistogramParams), TIVX_MEM_EXTERNAL);

    }

    return (status);
}

static vx_status VX_CALLBACK tivxKernelWriteHistogramProcess
(
    tivx_target_kernel_instance kernel,
    tivx_obj_desc_t *obj_desc[],
    vx_uint16 num_params,
    void *priv_arg
)
{
    vx_status status = VX_SUCCESS;

    tivxWriteHistogramParams *prms = NULL;

    if (NULL == obj_desc[TIVX_KERNEL_WRITE_HISTOGRAM_INPUT_DISTRIBUTION_IDX])
    {
        printf("Input Image handle is NULL!\n");
        status = VX_FAILURE;
    }

    if(status==VX_SUCCESS)
    {
        uint32_t size;

        status = tivxGetTargetKernelInstanceContext(kernel,
            (void **)&prms, &size);
        if ((VX_SUCCESS != status) || (NULL == prms) ||
            (sizeof(tivxWriteHistogramParams) != size))
        {
            status = VX_FAILURE;
        }
    }

    if (VX_SUCCESS == status)
    {
        uint32_t cam0_temp_val = 0;
        uint32_t cam1_temp_val = 0;
        uint32_t timestamp;
        time_t current_time;

        tivx_obj_desc_scalar_t *mean, *sd;
        tivx_obj_desc_distribution_t *in_histogram_desc;
        void* in_histogram_target_ptr = NULL;

        tivx_obj_desc_array_t* file_path_desc;
        void * file_path_target_ptr = NULL;

        tivx_obj_desc_array_t* file_prefix_desc;
        void * file_prefix_target_ptr = NULL;

        mean = (tivx_obj_desc_scalar_t*)obj_desc[TIVX_KERNEL_WRITE_HISTOGRAM_INPUT_MEAN_IDX];
        sd = (tivx_obj_desc_scalar_t*)obj_desc[TIVX_KERNEL_WRITE_HISTOGRAM_INPUT_SD_IDX];

        in_histogram_desc  = (tivx_obj_desc_distribution_t *)obj_desc[TIVX_KERNEL_WRITE_HISTOGRAM_INPUT_DISTRIBUTION_IDX];
        in_histogram_target_ptr  = tivxMemShared2TargetPtr(&in_histogram_desc->mem_ptr);
        tivxMemBufferMap(in_histogram_target_ptr, in_histogram_desc->mem_size, VX_MEMORY_TYPE_HOST, VX_READ_ONLY);

        file_path_desc = (tivx_obj_desc_array_t *)obj_desc[TIVX_KERNEL_WRITE_HISTOGRAM_FILE_PATH_IDX];
        if(file_path_desc != NULL)
        {
            file_path_target_ptr = tivxMemShared2TargetPtr(&file_path_desc->mem_ptr);
            tivxMemBufferMap(file_path_target_ptr, file_path_desc->mem_size, VX_MEMORY_TYPE_HOST,VX_READ_ONLY);
        }

        file_prefix_desc = (tivx_obj_desc_array_t *)obj_desc[TIVX_KERNEL_WRITE_HISTOGRAM_FILE_PREFIX_IDX];
        if(file_prefix_desc != NULL)
        {
            file_prefix_target_ptr = tivxMemShared2TargetPtr(&file_prefix_desc->mem_ptr);
            tivxMemBufferMap(file_prefix_target_ptr, file_prefix_desc->mem_size, VX_MEMORY_TYPE_HOST,VX_READ_ONLY);
        }

        /* This helps in synchonizing frames across nodes */
        if(prms->frame_counter == prms->cmd.start_frame)
            prms->skip_counter = 0;

        if(((prms->cmd.enableChanBitFlag & (1<<prms->ch_num))!= 0) &&
           (prms->frame_counter >= prms->cmd.start_frame) &&
           (prms->frame_counter < (prms->cmd.start_frame + (prms->cmd.num_frames * (prms->cmd.num_skip + 1)))) &&
           (prms->skip_counter == 0))
        {
            //printf("%s HISTO_WRITE Temp prms->cmd.phys_ptr: %p\n", appIpcGetCpuName(appIpcGetSelfCpuId()), prms->cmd.virt_ptr);
            // appMemCacheInv((void*)prms->cmd.virt_ptr, 256);
            
           
            current_time = time(NULL);  // get current time as a time_t value
            timestamp = (uint32_t) current_time;  // cast time_t to uint32_t

            cam0_temp_val = ((volatile CameraTempReadings*)prms->cmd.virt_ptr)->cam_temp_lower;    
            cam1_temp_val = ((volatile CameraTempReadings*)prms->cmd.virt_ptr)->cam_temp_upper;  
 
            printf("HISTO_WRITE Temp Upper: 0x%08X Lower: 0x%08X\n", cam1_temp_val, cam0_temp_val);
            
            if(prms->frame_counter == prms->cmd.start_frame){
                
                printf("Histo Write CH: %d\n", prms->ch_num);
                // new capture initialize buffers      
                memset((uint32_t *)ch_buffer[prms->ch_num], 0, HISTO_BUFFER_SIZE/4);
                memset((uint32_t *)meanArr[prms->ch_num], 0, STATS_CH_LENGTH/4);
                memset((void *)contrastArr[prms->ch_num], 0, STATS_CH_LENGTH*sizeof(float));

                pBufferCH[prms->ch_num] = ch_buffer[prms->ch_num];   // reset pointers 

                CH_framecount[prms->ch_num] = 0;
                bufferCH_size[prms->ch_num] = 0;
                bWriteComplete_CH[prms->ch_num] = 0;
                scanMeanCH[prms->ch_num] = 0;

                // copy data to buffer
                memcpy((uint8_t *)pBufferCH[prms->ch_num], in_histogram_target_ptr, in_histogram_desc->mem_size); // copy data
                pBufferCH[prms->ch_num] += in_histogram_desc->mem_size; // advance pointer
                bufferCH_size[prms->ch_num] += in_histogram_desc->mem_size; // increase data size

                memcpy((uint8_t *)pBufferCH[prms->ch_num], (void*)&mean->data.u32, 4); // copy mean data
                pBufferCH[prms->ch_num] += 4;
                bufferCH_size[prms->ch_num] += 4;

                memcpy((uint8_t *)pBufferCH[prms->ch_num], (void*)&sd->data.u32, 4); // copy sd data
                pBufferCH[prms->ch_num] += 4;
                bufferCH_size[prms->ch_num] += 4;

                memcpy((uint8_t *)pBufferCH[prms->ch_num], (uint8_t *)&cam0_temp_val, 4);
                pBufferCH[prms->ch_num] += 4;
                bufferCH_size[prms->ch_num] += 4;

                memcpy((uint8_t *)pBufferCH[prms->ch_num], (uint8_t *)&cam1_temp_val, 4);
                pBufferCH[prms->ch_num] += 4;
                bufferCH_size[prms->ch_num] += 4;

                memset((uint8_t *)pBufferCH[prms->ch_num], 0, 12); // reserved
                pBufferCH[prms->ch_num] += 12;
                bufferCH_size[prms->ch_num] += 12;

                memcpy((uint8_t *)pBufferCH[prms->ch_num], (void*)&timestamp, 4); // timestamp
                pBufferCH[prms->ch_num] += 4;
                bufferCH_size[prms->ch_num] += 4;
                                
                CH_framecount[prms->ch_num]++; // addvance the frame count

                printf("Histogram Write Start CH: %d\n",prms->ch_num);
                
                if(prms->cmd.num_frames == 1) // check if we only have one frame to capture
                {
                    // last frame
                    // ouput to file
                    printf("Histogram Capture Single Frame CH: %d\n",prms->ch_num);
                    char file_path[TIVX_FILEIO_FILE_PATH_LENGTH];
                    char file_prefix[TIVX_FILEIO_FILE_PREFIX_LENGTH];
                    char file_name[TIVX_FILEIO_FILE_PATH_LENGTH * 2];

                    strcpy(file_path, file_path_target_ptr);
                    strcpy(file_prefix, file_prefix_target_ptr);

                    time_t current_time;
                    /* Obtain current time. */
                    current_time = time(NULL);
                    if (current_time == ((time_t)-1))
                    {
                        // error
                        printf("[OW_MAIN::set_patient_id] Failure to obtain the current time.\n");
                    }

                    //struct tm tm = *localtime(&current_time);    
                    //char ts_string[80];
                    //sprintf(ts_string, "%04d_%02d_%02d_%02d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);    

                    switch(prms->cmd.scan_type){
                        case 0: // fullscan
                            sprintf(file_name, "%s/%s/%s/%s_fullscan_ch_%d_p%d.bin", file_path, prms->cmd.patient_path, prms->cmd.sub_path, file_prefix, prms->ch_num, prms->cmd.cam_pair);
                            break;
                        case 2:
                            sprintf(file_name, "%s/%s/%s/%s_longscan_ch_%d_p%d.bin", file_path, prms->cmd.patient_path, prms->cmd.sub_path, file_prefix, prms->ch_num, prms->cmd.cam_pair);
                            break;
                        case 3:
                            sprintf(file_name, "%s/%s/%s/%s_darkscan_ch_%d_p%d.bin", file_path, prms->cmd.patient_path, prms->cmd.sub_path, file_prefix, prms->ch_num, prms->cmd.cam_pair);
                            break;
                        default:    // treat as test scan
                            sprintf(file_name, "%s/%s/%s/%s_testscan_ch_%d_p%d.bin", file_path, prms->cmd.patient_path, prms->cmd.sub_path, file_prefix, prms->ch_num, prms->cmd.cam_pair);
                            break;
                    }
                    size_t wr_size = 0;
                    FILE *fp = fopen(file_name, "wb");
                    
                    typeHEAD_SENSOR_MESSAGE hsMessage;
                    hsMessage.cmd = HS_CMD_HISTO_DONE_STATE;
                    hsMessage.state = 0;
                    memset(hsMessage.msg, 0, 256);

                    if(fp == NULL)
                    {
                        printf(">>>>> Unable to write Single Frame file %s <<<<<\n", file_name);
                    }
                    else
                    {
                        
                        wr_size = fwrite(ch_buffer[prms->ch_num], 1, bufferCH_size[prms->ch_num], fp);

                        // fwrite((void*)&mean->data.u32, 1, sizeof(uint32_t), fp);
                        // fwrite((void*)&sd->data.u32, 1, sizeof(uint32_t), fp);
                        if(wr_size < 0)
                        {
                            hsMessage.state = 1;
                            strcpy(hsMessage.msg, strerror(wr_size));
                        }

                        fflush(fp);
                        fclose(fp);
                        
                        int bit_count = count_bits_enabled(prms->cmd.enableChanBitFlag);

                        bWriteComplete_CH[prms->ch_num] = 1;
                        printf("Histogram Write Complete Total channels: %d CH: %d FRAMES: %d\n",bit_count,prms->ch_num, CH_framecount[prms->ch_num]);
                        int all_complete = 1;
                        for (int i = 0; i < bit_count; i++) {
                            if (bWriteComplete_CH[i] != 1) {
                                all_complete = 0;
                                break;
                            }
                        }
                        
                        if(all_complete)
                        {
                            status = appRemoteServiceRun(
                                APP_IPC_CPU_MPU1_0,
                                (char*)HEAD_SENSOR_SERVICE_NAME,
                                hsMessage.cmd,
                                (void*)&hsMessage,
                                sizeof(struct HEAD_SENSOR_MESSAGE),
                                0
                            );
                            
                            if(status!= 0){
                                printf("ARM IPC: Sent msg FAILED \n");
                            }
                        }
                    }
                }



            }
            else if(prms->frame_counter == (prms->cmd.start_frame + prms->cmd.num_frames - 1))
            {
                // end capture
                // copy data to buffer
                memcpy((uint8_t *)pBufferCH[prms->ch_num], in_histogram_target_ptr, in_histogram_desc->mem_size); // copy data
                pBufferCH[prms->ch_num] += in_histogram_desc->mem_size; // advance pointer
                bufferCH_size[prms->ch_num] += in_histogram_desc->mem_size; // increase data size

                memcpy((uint8_t *)pBufferCH[prms->ch_num], (void*)&mean->data.u32, 4); // copy mean data
                pBufferCH[prms->ch_num] += 4;
                bufferCH_size[prms->ch_num] += 4;

                memcpy((uint8_t *)pBufferCH[prms->ch_num], (void*)&sd->data.u32, 4); // copy sd data
                pBufferCH[prms->ch_num] += 4;
                bufferCH_size[prms->ch_num] += 4;

                memcpy((uint8_t *)pBufferCH[prms->ch_num], (uint8_t *)&cam0_temp_val, 4);
                pBufferCH[prms->ch_num] += 4;
                bufferCH_size[prms->ch_num] += 4;

                memcpy((uint8_t *)pBufferCH[prms->ch_num], (uint8_t *)&cam1_temp_val, 4);
                pBufferCH[prms->ch_num] += 4;
                bufferCH_size[prms->ch_num] += 4;

                memset((uint8_t *)pBufferCH[prms->ch_num], 0, 12); // reserved
                pBufferCH[prms->ch_num] += 12;
                bufferCH_size[prms->ch_num] += 12;

                memcpy((uint8_t *)pBufferCH[prms->ch_num], (void*)&timestamp, 4); // timestamp
                pBufferCH[prms->ch_num] += 4;
                bufferCH_size[prms->ch_num] += 4;

                CH_framecount[prms->ch_num]++; // advance the frame counter

#ifdef WRITE_FILE_OUTPUT
                // ouput to file
                char file_path[TIVX_FILEIO_FILE_PATH_LENGTH];
                char file_prefix[TIVX_FILEIO_FILE_PREFIX_LENGTH];
                char file_name[TIVX_FILEIO_FILE_PATH_LENGTH * 2];

                strcpy(file_path, file_path_target_ptr);
                strcpy(file_prefix, file_prefix_target_ptr);

                time_t current_time;
                /* Obtain current time. */
                current_time = time(NULL);
                if (current_time == ((time_t)-1))
                {
                    // error
                    printf("[OW_MAIN::set_patient_id] Failure to obtain the current time.\n");
                }

                // struct tm tm = *localtime(&current_time);    
                // char ts_string[80];
                // sprintf(ts_string, "%04d_%02d_%02d_%02d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);    
         
                switch(prms->cmd.scan_type){
                    case 0: // fullscan
                        sprintf(file_name, "%s/%s/%s/%s_fullscan_ch_%d_p%d.bin", file_path, prms->cmd.patient_path, prms->cmd.sub_path, file_prefix, prms->ch_num, prms->cmd.cam_pair);
                        break;
                    case 2: // fullscan
                        sprintf(file_name, "%s/%s/%s/%s_longscan_ch_%d_p%d.bin", file_path, prms->cmd.patient_path, prms->cmd.sub_path, file_prefix, prms->ch_num, prms->cmd.cam_pair);
                        break;
                    case 3: // darkscan
                        sprintf(file_name, "%s/%s/%s/%s_darkscan_ch_%d_p%d.bin", file_path, prms->cmd.patient_path, prms->cmd.sub_path, file_prefix, prms->ch_num, prms->cmd.cam_pair);
                        break;
                    default:    // treat as test scan
                        sprintf(file_name, "%s/%s/%s/%s_testscan_ch_%d_p%d.bin", file_path, prms->cmd.patient_path, prms->cmd.sub_path, file_prefix, prms->ch_num, prms->cmd.cam_pair);
                        break;
                }
                size_t wr_size = 0;
                FILE *fp = fopen(file_name, "wb");

                typeHEAD_SENSOR_MESSAGE hsMessage;
                hsMessage.cmd = HS_CMD_HISTO_DONE_STATE;
                hsMessage.state = 0;
                hsMessage.imageMean[0] = 0;
                hsMessage.imageMean[1] = 0;
                hsMessage.imageMean[2] = 0;
                memset(hsMessage.msg, 0, 256);

                if(prms->cmd.scan_type==1) //Test scan
                {
                    float scanMean = 0;
                    for(uint32_t ind=0; ind<CH_framecount[prms->ch_num]; ++ind)
                    {
                        scanMean += meanArr[prms->ch_num][ind];
                        ++pBufferCH[prms->ch_num];
                    }
                    scanMean /= CH_framecount[prms->ch_num];
                    scanMeanCH[prms->ch_num] = (uint32_t)scanMean;
                }

                if(fp == NULL)
                {
                    printf(">>>>> Unable to write file %s <<<<<\n", file_name);
                }
                else
                {
                    printf("Histogram Writing CH: %d FRAMES: %d BYTES: %d\n",prms->ch_num, CH_framecount[prms->ch_num], bufferCH_size[prms->ch_num]);
                    wr_size = fwrite(ch_buffer[prms->ch_num], 1, bufferCH_size[prms->ch_num], fp);
                    
                    // fwrite((void*)&mean->data.u32, 1, sizeof(uint32_t), fp);
                    // fwrite((void*)&sd->data.u32, 1, sizeof(uint32_t), fp);
                    if(wr_size < 0)
                    {
                        hsMessage.state = 1;
                        strcpy(hsMessage.msg, strerror(wr_size));
                    }

                    fflush(fp);
                    fclose(fp);

                    int bit_count = count_bits_enabled(prms->cmd.enableChanBitFlag);

                    bWriteComplete_CH[prms->ch_num] = 1;                    
                    printf("Histogram Write Complete Total channels: %d CH: %d FRAMES: %d\n",bit_count,prms->ch_num, CH_framecount[prms->ch_num]);

                    int all_complete = 1;
                    for (int i = 0; i < bit_count; i++) {
                        if (bWriteComplete_CH[i] != 1) {
                            all_complete = 0;
                            break;
                        }
                    }

                    if(all_complete)
                    {
                        if(prms->cmd.scan_type==1) //Test scan
                        {
                            hsMessage.imageMean[0] = scanMeanCH[0];
                            hsMessage.imageMean[1] = scanMeanCH[1];
                            hsMessage.imageMean[2] = scanMeanCH[2];
                        }
                    
                        status = appRemoteServiceRun(
                            APP_IPC_CPU_MPU1_0,
                            (char*)HEAD_SENSOR_SERVICE_NAME,
                            hsMessage.cmd,
                            (void*)&hsMessage,
                            sizeof(struct HEAD_SENSOR_MESSAGE),
                            0
                        );

                        if(status!= 0){
                            printf("ARM IPC: Sent msg FAILED \n");
                        }
                    }
                }
#endif
                // printf(">>>>>>>>>>>>>>>>>>>>>>>>>> END CH %d FRAME CAPTURE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n",  prms->ch_num);

            }
             else
            {
                // copy data to buffer
                memcpy((uint8_t *)pBufferCH[prms->ch_num], in_histogram_target_ptr, in_histogram_desc->mem_size); // copy data
                pBufferCH[prms->ch_num] += in_histogram_desc->mem_size; // advance pointer
                bufferCH_size[prms->ch_num] += in_histogram_desc->mem_size; // increase data size

                memcpy((uint8_t *)pBufferCH[prms->ch_num], (void*)&mean->data.u32, 4); // copy mean data
                pBufferCH[prms->ch_num] += 4;
                bufferCH_size[prms->ch_num] += 4;

                memcpy((uint8_t *)pBufferCH[prms->ch_num], (void*)&sd->data.u32, 4); // copy sd data
                pBufferCH[prms->ch_num] += 4;
                bufferCH_size[prms->ch_num] += 4;

                memcpy((uint8_t *)pBufferCH[prms->ch_num], (uint8_t *)&cam0_temp_val, 4);
                pBufferCH[prms->ch_num] += 4;
                bufferCH_size[prms->ch_num] += 4;

                memcpy((uint8_t *)pBufferCH[prms->ch_num], (uint8_t *)&cam1_temp_val, 4);
                pBufferCH[prms->ch_num] += 4;
                bufferCH_size[prms->ch_num] += 4;

                memset((uint8_t *)pBufferCH[prms->ch_num], 0, 12); // reserved
                pBufferCH[prms->ch_num] += 12;
                bufferCH_size[prms->ch_num] += 12;

                memcpy((uint8_t *)pBufferCH[prms->ch_num], (void*)&timestamp, 4); // timestamp
                pBufferCH[prms->ch_num] += 4;
                bufferCH_size[prms->ch_num] += 4;

                CH_framecount[prms->ch_num]++; // advance frame counter
            }

        }
        prms->skip_counter++;
        prms->skip_counter = prms->skip_counter % (prms->cmd.num_skip + 1);
        prms->frame_counter++;

        tivxMemBufferUnmap(in_histogram_target_ptr, in_histogram_desc->mem_size, VX_MEMORY_TYPE_HOST, VX_READ_ONLY);

        if(file_path_target_ptr != NULL)
        {
            tivxMemBufferUnmap(file_path_target_ptr, file_path_desc->mem_size, VX_MEMORY_TYPE_HOST, VX_READ_ONLY);
        }
        if(file_prefix_target_ptr != NULL)
        {
            tivxMemBufferUnmap(file_prefix_target_ptr, file_prefix_desc->mem_size, VX_MEMORY_TYPE_HOST, VX_READ_ONLY);
        }

    }

    return (status);
}
void tivxAddTargetKernelWriteHistogram()
{
    char target_name[TIVX_TARGET_MAX_NAME];
    vx_enum self_cpu;

    self_cpu = tivxGetSelfCpuId();

    if ((self_cpu == TIVX_CPU_ID_A72_0))
    {
        strncpy(target_name, TIVX_TARGET_A72_0, TIVX_TARGET_MAX_NAME);

        vx_write_histogram_target_kernel = tivxAddTargetKernelByName
                                (
                                  TIVX_KERNEL_WRITE_HISTOGRAM_NAME,
                                  target_name,
                                  tivxKernelWriteHistogramProcess,
                                  tivxKernelWriteHistogramCreate,
                                  tivxKernelWriteHistogramDelete,
                                  tivxKernelWriteHistogramControl,
                                  NULL
                                );
    }
}

void tivxRemoveTargetKernelWriteHistogram()
{
    vx_status status = VX_SUCCESS;

    status = tivxRemoveTargetKernel(vx_write_histogram_target_kernel);
    if (status == VX_SUCCESS)
    {
        vx_write_histogram_target_kernel = NULL;
    }
}
