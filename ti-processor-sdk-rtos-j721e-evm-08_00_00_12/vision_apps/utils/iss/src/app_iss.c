/*
 *
 * Copyright (c) 2018 Texas Instruments Incorporated
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

#include <utils/ipc/include/app_ipc.h>
#include <utils/remote_service/include/app_remote_service.h>
#include <utils/iss/include/app_iss.h>
#include <utils/mem/include/app_mem.h>
#include <utils/console_io/include/app_log.h>
#include <utils/d3board/include/app_d3board.h>

#include <apps/basic_demos/app_openwater/include/ow_common.h>

#define APP_IPC_D3_BOARD APP_IPC_CPU_MCU2_0

#ifdef A72
static uint8_t  g_cmdPrm[CMD_PARAM_SIZE];

int32_t appEnumerateImageSensor(char *sensor_name_list[], uint8_t  * num_sensors_found)
{
    int32_t status = -1;
    uint8_t  count = 0;
    uint8_t  numRegisteredSensors = 0;
    char* sensor_name = NULL;

    appLogPrintf("ISS: Enumerating sensors ... !!!\n");

    memset(g_cmdPrm, 0, CMD_PARAM_SIZE);
    status = appRemoteServiceRun(
        APP_IPC_CPU_MCU2_0 ,
        IMAGE_SENSOR_REMOTE_SERVICE_NAME,
        IM_SENSOR_CMD_ENUMERATE,
        (void*)g_cmdPrm,
        CMD_PARAM_SIZE,
        0
    );

    if(0 == status)
    {
        for(count=0;count<ISS_SENSORS_MAX_SUPPORTED_SENSOR;count++)
        {
            sensor_name = (char*)(g_cmdPrm + count*ISS_SENSORS_MAX_NAME);
            if(0 !=sensor_name[0])
            {
                appLogPrintf("ISS: Enumerating sensors ... found %d : %s\n", count, sensor_name);
                memcpy(sensor_name_list[count], sensor_name, ISS_SENSORS_MAX_NAME);
                numRegisteredSensors++;
            }
        }
    }
    else
    {
        appLogPrintf("ISS: ERROR: Enumerating sensors failed !!!\n", sensor_name);
    }
    *num_sensors_found = numRegisteredSensors;

    return status;
}
/*
Sends the sensor name in cmd_param
Expects sensor properties at cmd_param[0] + ISS_SENSORS_MAX_NAME
*/

int32_t appQueryImageSensor(char* sensor_name, IssSensor_CreateParams* pSensorCreatePrms)
{
    int32_t status = -1;
    memset(pSensorCreatePrms, 0, sizeof(IssSensor_CreateParams));
    memset(g_cmdPrm, 0, CMD_PARAM_SIZE);
    memcpy(g_cmdPrm, sensor_name, ISS_SENSORS_MAX_NAME);

    appLogPrintf("ISS: Querying sensor [%s]\n", sensor_name);
    status = appRemoteServiceRun(
        APP_IPC_CPU_MCU2_0 ,
        IMAGE_SENSOR_REMOTE_SERVICE_NAME,
        IM_SENSOR_CMD_QUERY,
        (void*)g_cmdPrm,
        CMD_PARAM_SIZE,
        0
    );
    if(0 == status)
    {
        /*Sensor service appends sensor properties to sensor name in cmdprm buffer*/
        memcpy(pSensorCreatePrms, (g_cmdPrm+ISS_SENSORS_MAX_NAME), sizeof(IssSensor_CreateParams));

        appLogPrintf("ISS: Querying sensor [%s] ... Done !!!\n", sensor_name);
    }
    else
    {
        appLogPrintf("ISS: ERROR: Querying sensor [%s] failed !!!\n", sensor_name);
    }
    return status;
}

int32_t appEnableLaser()
{
    int32_t status = -1;
    appLogPrintf("ISS: turning laser on ... !!!\n");

    AppD3boardCmdParams cmdPrms;

    cmdPrms.moduleId= APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC;

    status = appRemoteServiceRun(
        APP_IPC_D3_BOARD,
        APP_REMOTE_SERVICE_D3BOARD_NAME,
        APP_REMOTE_SERVICE_D3BOARD_CMD_ENABLE_LASER,
        (void *)&cmdPrms,
        sizeof(AppD3boardCmdParams),
        0
    );
	
    return status;
}

int32_t appDisableLaser()
{
    int32_t status = -1;
    appLogPrintf("ISS: turning laser off ... !!!\n");

    AppD3boardCmdParams cmdPrms;

    cmdPrms.moduleId= APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC;

    status = appRemoteServiceRun(
        APP_IPC_D3_BOARD,
        APP_REMOTE_SERVICE_D3BOARD_NAME,
        APP_REMOTE_SERVICE_D3BOARD_CMD_DISABLE_LASER,
        (void *)&cmdPrms,
        sizeof(AppD3boardCmdParams),
        0
    );
	
    return status;
}

int32_t appStartFrameSync()
{
    int32_t status = -1;

    AppD3boardCmdParams cmdPrms;

    cmdPrms.moduleId= APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC;

    status = appRemoteServiceRun(
        APP_IPC_D3_BOARD,
        APP_REMOTE_SERVICE_D3BOARD_NAME,
        APP_REMOTE_SERVICE_D3BOARD_CMD_START_FSYNC,
        (void *)&cmdPrms,
        sizeof(AppD3boardCmdParams),
        0
    );

    if(status != 0)
    {
        appLogPrintf("ISS: ERROR: Starting frame sync failed !!!\n");
    }
    else
    {
        appLogPrintf("ISS: Starting frame sync ... Done !!!\n");
    }

    return status;
}

int32_t appStopFrameSync()
{
    int32_t status = -1;

    AppD3boardCmdParams cmdPrms;

    cmdPrms.moduleId = APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC;

    status = appRemoteServiceRun(
        APP_IPC_D3_BOARD,
        APP_REMOTE_SERVICE_D3BOARD_NAME,
        APP_REMOTE_SERVICE_D3BOARD_CMD_STOP_FSYNC,
        (void *)&cmdPrms,
        sizeof(AppD3boardCmdParams),
        0);

    if (status != 0)
    {
        appLogPrintf("ISS: ERROR: Stopping frame sync failed !!!\n");
    }
    else
    {
        appLogPrintf("ISS: Stopping frame sync ... Done !!!\n");
    }

    return status;
}

int32_t appInitD3board(AppD3boardInitCmdParams *cmdPrms)
{
    int32_t status = -1;
    appLogPrintf("ISS: Initializing D3 personality board ... !!!\n");

    status = appRemoteServiceRun(
        APP_IPC_D3_BOARD,
        APP_REMOTE_SERVICE_D3BOARD_NAME,
        APP_REMOTE_SERVICE_D3BOARD_CMD_INIT,
        (void *)cmdPrms,
        sizeof(AppD3boardInitCmdParams),
        0);

    if (status != 0)
    {
        appLogPrintf("ISS: ERROR: Initializing D3 personality board failed !!!\n");
    }
    else
    {
        appLogPrintf("ISS: Initializing D3 personality board ... Done !!!\n");
    }

    return status;
}

int32_t appInitImageSensor(char* sensor_name, uint32_t featuresEnabled, uint32_t channel_mask)
{
    int32_t status = -1;
    // TODO: need to change this for 16ch support
    if((channel_mask >= (1<<ISS_SENSORS_MAX_CHANNEL)) || (channel_mask < 0x1))
    {
        printf("Invalid channel_mask. Valid values are from 0x1 - 0xF");
        return -1;
    }

    appLogPrintf("ISS: Initializing sensor [%s] chMask 0x%x, doing IM_SENSOR_CMD_PWRON ... !!!\n", sensor_name, channel_mask);

    memset(g_cmdPrm, 0, CMD_PARAM_SIZE);
    memcpy(g_cmdPrm, sensor_name, ISS_SENSORS_MAX_NAME);
    memcpy(g_cmdPrm+ISS_SENSORS_MAX_NAME, &channel_mask, sizeof(uint32_t));
    status = appRemoteServiceRun(
        APP_IPC_CPU_MCU2_0 ,
        IMAGE_SENSOR_REMOTE_SERVICE_NAME,
        IM_SENSOR_CMD_PWRON,
        (void*)g_cmdPrm,
        CMD_PARAM_SIZE,
        0
    );

    if(status==0)
    {
        appLogPrintf("ISS: Initializing sensor [%s], doing IM_SENSOR_CMD_CONFIG ... !!!\n", sensor_name);

        memcpy(g_cmdPrm+ISS_SENSORS_MAX_NAME, &featuresEnabled, sizeof(uint32_t));
        memcpy(g_cmdPrm+ISS_SENSORS_MAX_NAME+sizeof(uint32_t), &channel_mask, sizeof(uint32_t));

        status = appRemoteServiceRun(
            APP_IPC_CPU_MCU2_0 ,
            IMAGE_SENSOR_REMOTE_SERVICE_NAME,
            IM_SENSOR_CMD_CONFIG,
            (void*)g_cmdPrm,
            CMD_PARAM_SIZE,
            0
        );
    }
    if(status != 0)
    {
        appLogPrintf("ISS: ERROR: Initializing sensor [%s] failed !!!\n", sensor_name);
    }
    appLogPrintf("ISS: Initializing sensor [%s] ... Done !!!\n", sensor_name);

    return status;
}

int32_t appActivateCameraPairs(int32_t pair)
{
    int32_t status = -1;
    memset(g_cmdPrm, 0xFF, CMD_PARAM_SIZE);
    memcpy(g_cmdPrm, &pair, sizeof(int32_t));

    status = appRemoteServiceRun(
        APP_IPC_CPU_MCU2_0,
        IMAGE_SENSOR_REMOTE_SERVICE_NAME,
        IM_SENSOR_CMD_ACTIVATEPAIR,
        (void*)g_cmdPrm,
        CMD_PARAM_SIZE,
        0
    );

    return status;
}

int32_t appActivateCameraMask(uint16_t mask)
{
    int32_t status = -1;
    memset(g_cmdPrm, 0xFF, CMD_PARAM_SIZE);
    memcpy(g_cmdPrm, &mask, sizeof(uint16_t));

    status = appRemoteServiceRun(
        APP_IPC_CPU_MCU2_0,
        IMAGE_SENSOR_REMOTE_SERVICE_NAME,
        IM_SENSOR_CMD_ACITIVATE_MASK,
        (void*)g_cmdPrm,
        CMD_PARAM_SIZE,
        0
    );

    return status;
}

int32_t appDetectImageSensor(uint8_t *sensor_id_list, uint8_t *num_sensors_found, uint32_t channel_mask)
{
    int32_t status = -1;
    uint8_t sensor_id;
    uint8_t numDetectedSensors = 0;
    uint8_t chId = 0;
    uint8_t * cmd_ptr = g_cmdPrm;

    memset(g_cmdPrm, 0xFF, CMD_PARAM_SIZE);
    memcpy(g_cmdPrm, &channel_mask, sizeof(uint32_t));

    status = appRemoteServiceRun(
        APP_IPC_CPU_MCU2_0 ,
        IMAGE_SENSOR_REMOTE_SERVICE_NAME,
        IM_SENSOR_CMD_DETECT,
        (void*)g_cmdPrm,
        CMD_PARAM_SIZE,
        0
    );

    if(0 == status)
    {
        while(channel_mask > 0)
        {
            if(channel_mask & 0x1)
            {    
                sensor_id = (uint8_t)(*cmd_ptr);
                if(0xFF != sensor_id)
                {
                    numDetectedSensors++;
                    sensor_id_list[chId] = sensor_id;
                }
            }
            channel_mask = channel_mask >> 1;
            chId++;
            cmd_ptr += sizeof(uint8_t);
        }
        *num_sensors_found = numDetectedSensors;
    }
    else
    {
        printf("Error : appDetectImageSensor failed !!!\n");
        *num_sensors_found = 0;
    }

    return status;
}

int32_t appStartImageSensor(char* sensor_name, uint32_t channel_mask)
{
    int32_t status = -1;
    appLogPrintf("ISS: Starting sensor [%s] ... !!!\n", sensor_name);

    memset(g_cmdPrm, 0, CMD_PARAM_SIZE);
    memcpy(g_cmdPrm, sensor_name, ISS_SENSORS_MAX_NAME);
    memcpy(g_cmdPrm+ISS_SENSORS_MAX_NAME, &channel_mask, sizeof(uint32_t));

    status = appRemoteServiceRun(
        APP_IPC_CPU_MCU2_0 ,
        IMAGE_SENSOR_REMOTE_SERVICE_NAME,
        IM_SENSOR_CMD_STREAM_ON,
        (void*)g_cmdPrm,
        CMD_PARAM_SIZE,
        0
    );

    if(status==0)
    {
        appLogPrintf("ISS: Starting sensor [%s] ... !!!\n", sensor_name);
    }
    else
    {
        appLogPrintf("ISS: Starting sensor [%s] failed !!!\n", sensor_name);
    }

    return status;
}

int32_t appStopImageSensor(char* sensor_name, uint32_t channel_mask)
{
    int32_t status = -1;

    appLogPrintf("ISS: Stopping sensor [%s] chMask 0x%x !!!\n", sensor_name, channel_mask);

    memset(g_cmdPrm, 0, CMD_PARAM_SIZE);
    memcpy(g_cmdPrm, sensor_name, ISS_SENSORS_MAX_NAME);
    memcpy(g_cmdPrm+ISS_SENSORS_MAX_NAME, &channel_mask, sizeof(uint32_t));

    status = appRemoteServiceRun(
        APP_IPC_CPU_MCU2_0 ,
        IMAGE_SENSOR_REMOTE_SERVICE_NAME,
        IM_SENSOR_CMD_STREAM_OFF,
        (void*)g_cmdPrm,
        CMD_PARAM_SIZE,
        0
    );

    if(status==0)
    {
        appLogPrintf("ISS: Stopping sensor [%s] ... Done !!!\n", sensor_name);
    }
    else
    {
        appLogPrintf("ISS: Stopping sensor [%s] failed !!!\n", sensor_name);
    }

    return status;
}

// static uint32_t xCount = 0;
uint32_t appGetTempCameraPair(uint32_t pair, uint64_t phys_ptr, void *virt_ptr)
{
    int32_t status = -1;
    // void *virt_ptr;
    // uint64_t phys_ptr;
    uint32_t ptr_value = 0;
    // uint32_t mem_size = 1024;
    //volatile uint32_t ret_val;

    // virt_ptr = appMemAlloc(APP_MEM_HEAP_DDR, mem_size, 32);
    // if(NULL == virt_ptr)
    // {
    //     appLogPrintf("appGetTempCameraPair: failed invalid memory pointer!!!\n");
    //     return (-1);
    // }

    // phys_ptr = appMemGetVirt2PhyBufPtr((uint64_t)(uintptr_t)virt_ptr, APP_MEM_HEAP_DDR);

    /* assume 32b physical pointer */
    ptr_value = (uint32_t)phys_ptr;
    //if(xCount % 30 == 0)
    //{
    //    printf("%s appGetTempCameraPair ptr_value: 0x%08x\n", appIpcGetCpuName(appIpcGetSelfCpuId()), ptr_value);
    //}
    // xCount++;

    // set the pair number as the input
    ((CameraTempReadings*)virt_ptr)->mask = pair;
    ((CameraTempReadings*)virt_ptr)->cam_temp_lower = 0xFFFF;
    ((CameraTempReadings*)virt_ptr)->cam_temp_upper = 0xFFFF;

    appMemCacheWbInv((void*)virt_ptr, sizeof(CameraTempReadings));

    #ifdef A72
    asm("    DSB SY");
    #endif
    // printf("appGetTempCameraPair call mask: 0x%02X TEMPs: %08X  %08X\n", (uint8_t)((CameraTempReadings*)virt_ptr)->mask, ((CameraTempReadings*)virt_ptr)->cam_temp_upper, ((CameraTempReadings*)virt_ptr)->cam_temp_lower);
    status = appRemoteServiceRun(
        APP_IPC_CPU_MCU2_0,
        IMAGE_SENSOR_REMOTE_SERVICE_NAME,
        IM_SENSOR_CMD_TEMPPAIR,
        &ptr_value,
        sizeof(CameraTempReadings),
        0
    );

    appMemCacheInv((void*)virt_ptr, sizeof(CameraTempReadings));
    // printf("appGetTempCameraPair Returned => MASK: 0x%02X TEMPs: %08X  %08X\n",(uint8_t)((CameraTempReadings*)virt_ptr)->mask, ((CameraTempReadings*)virt_ptr)->cam_temp_upper, ((CameraTempReadings*)virt_ptr)->cam_temp_lower);
   
    #ifdef A72
    asm("    DSB SY");
    #endif

    // ret_val = *(volatile uint32_t*)virt_ptr;
    
    // appMemFree(APP_MEM_HEAP_DDR, virt_ptr, mem_size);
    if(status != VX_SUCCESS)
    {
        appLogPrintf("appGetTempCameraPair: failed to call remote procedure\n");        
    }
    return status;
}

uint32_t appGetPersonalityBoardTemp()
{
    void *virt_ptr;
    uint64_t phys_ptr;
    uint32_t mem_size = 1024;

    virt_ptr = appMemAlloc(APP_MEM_HEAP_DDR, mem_size, 32);
    if(NULL == virt_ptr)
    {
        printf("appGetPersonalityBoardTemp: failed invalid memory pointer!!!\n");
        return 0;
    }

    phys_ptr = appMemGetVirt2PhyBufPtr((uint64_t)(uintptr_t)virt_ptr, APP_MEM_HEAP_DDR);
    

    int32_t status = -1;
    uint32_t ptr_value = 0;
    volatile uint32_t ret_val;

    ptr_value = (uint32_t)phys_ptr;

    *(volatile uint32_t*)virt_ptr = 0;

    appMemCacheWbInv((void*)virt_ptr, 256);

    #ifdef A72
    asm("    DSB SY");
    #endif

    status = appRemoteServiceRun(
        APP_IPC_CPU_MCU2_0,
        IMAGE_SENSOR_REMOTE_SERVICE_NAME,
        IM_SENSOR_CMD_BOARD_TEMP,
        &ptr_value,
        sizeof(uint32_t),
        0
    );

    appMemCacheInv((void*)virt_ptr, 256);

    #ifdef A72
    asm("    DSB SY");
    #endif

    ret_val = *(volatile uint32_t*)virt_ptr;
    appMemFree(APP_MEM_HEAP_DDR, virt_ptr, mem_size);

    if(status == 0)
    {
        return ret_val;
    }
    else
    {
        appLogPrintf("appgetPersTemp: failed to call remote procedure\n");
        return 0;
    }
}

int32_t appD3I2CMenu(D3UtilsI2C_Command* cmd)
{
    int32_t status = -1;
    memset(g_cmdPrm, 0xFF, CMD_PARAM_SIZE);
    memcpy(g_cmdPrm, cmd, sizeof(D3UtilsI2C_Command));
    status = appRemoteServiceRun(
        APP_IPC_CPU_MCU2_0,
        IMAGE_SENSOR_REMOTE_SERVICE_NAME,
        IM_SENSOR_CMD_D3_USER_I2C,
        (void *)g_cmdPrm,
        CMD_PARAM_SIZE,
        0
    );

    if((status == 0) && (cmd->direction == D3_I2C_READ)) {
        memcpy(cmd, g_cmdPrm, sizeof(D3UtilsI2C_Command));
    }

    return status;
}

int32_t appDeInitD3board()
{
    int32_t status = -1;
    appLogPrintf("ISS: De-Initializing D3 personality board ... !!!\n");

    AppD3boardDeInitCmdParams cmdPrms;

    cmdPrms.moduleId = APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC;

    status = appRemoteServiceRun(
        APP_IPC_D3_BOARD,
        APP_REMOTE_SERVICE_D3BOARD_NAME,
        APP_REMOTE_SERVICE_D3BOARD_CMD_DEINIT,
        (void *)&cmdPrms,
        sizeof(AppD3boardDeInitCmdParams),
        0);

    if (status != 0)
    {
        appLogPrintf("ISS: ERROR: De-Initializing D3 personality board failed !!!\n");
    }
    else
    {
        appLogPrintf("ISS: De-Initializing D3 personality board ... Done !!!\n");
    }

    return status;
}

int32_t appDeInitImageSensor(char* sensor_name, uint32_t channel_mask)
{
    int32_t status = -1;
    memset(g_cmdPrm, 0, CMD_PARAM_SIZE);
    memcpy(g_cmdPrm, sensor_name, ISS_SENSORS_MAX_NAME);
    memcpy(g_cmdPrm+ISS_SENSORS_MAX_NAME, &channel_mask, sizeof(uint32_t));

    appLogPrintf("ISS: appDeInitImageSensor [%s] chMask 0x%x !!!\n", sensor_name, channel_mask);

    status = appRemoteServiceRun(
        APP_IPC_CPU_MCU2_0 ,
        IMAGE_SENSOR_REMOTE_SERVICE_NAME,
        IM_SENSOR_CMD_PWROFF,
        (void*)g_cmdPrm,
        CMD_PARAM_SIZE,
        0
    );

    if(status==0)
    {
        appLogPrintf("ISS: De-initializing sensor [%s] ... Done !!!\n", sensor_name);
    }
    else
    {
        appLogPrintf("ISS: De-initializing sensor [%s] failed !!!\n", sensor_name);
    }

    return status;
}

#endif /*(A72)*/

#if defined(R5F) && (defined(SYSBIOS) || defined(FREERTOS))
int32_t appIssInit()
{
    int32_t status;
    int32_t itt_status;
    int32_t viss_status;

    appLogPrintf("ISS: Init ... !!!\n");

    status = IssSensor_Init();

    if(status!=0)
    {
        appLogPrintf("ISS: ERROR: Init failed !!!\n");
        return -1;
    }
    else
    {
        appLogPrintf("IssSensor_Init ... Done !!!\n");
    }

    viss_status = VissRemoteServer_Init();
    if(viss_status!=0)
    {
        printf("ISS: Error: Failed to create remote VISS remote server failed. Live tuning will not work !!!\n");
        return -1;
    }
    else
    {
        appLogPrintf("vissRemoteServer_Init ... Done !!!\n");
    }

    itt_status = IttRemoteServer_Init();
    if(itt_status!=0)
    {
        printf("ISS: Warning: Failed to create remote ITT server failed. Live tuning will not work !!!\n");
    }
    else
    {
        appLogPrintf("IttRemoteServer_Init ... Done !!!\n");
    }

    return status;
}

int32_t appIssDeInit()
{
    int32_t status;
    int32_t itt_status;
    int32_t viss_status;
    status = IssSensor_DeInit();
    if(status!=0)
    {
        printf(" appIssDeInit: ERROR: Failed to deinitialize ISS sensor \n");
    }

    viss_status = VissRemoteServer_DeInit();
    if(viss_status!=0)
    {
        printf(" appIssDeInit: ERROR: Failed to deinitialize ISS sensor \n");
    }

    itt_status = IttRemoteServer_DeInit();
    if(itt_status!=0)
    {
        printf(" appIssDeInit: ERROR: Failed to deinitialize remote ITT server \n");
    }

    appLogPrintf("APP ISS: Deinit ... Done !!!\n");
    return (status|viss_status);
}

#endif /*defined(R5F) && (defined(SYSBIOS) || defined(FREERTOS))*/
