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
#include <iss_sensors.h>
#include <iss_sensor_if.h>
#include <iss_sensor_priv.h>
#include <app_remote_service.h>
#include <app_ipc.h>
#include <app_mem.h>
#include <max9296.h>
#include <max9295.h>
#include <d3_gmsl_board.h>
#include <ow_common.h>


/*******************************************************************************
 *  Globals
 *******************************************************************************
 */
IssSensors_Handle *gIssSensorTable[ISS_SENSORS_MAX_SUPPORTED_SENSOR];

/*******************************************************************************
 *  Local Functions Declarations
 *******************************************************************************
 */
static int32_t checkForHandle(void* handle);

static uint32_t sensor_updateCameraPairTemp(CameraTempReadings * pCamTemp);
// static uint32_t xCount = 0;

/*******************************************************************************
 *  Function Definition
 *******************************************************************************
 */
static I2C_Handle gISS_Sensor_I2cHandle = NULL;
static uint8_t gISS_Sensor_ByteOrder = BOARD_I2C_REG_ADDR_MSB_FIRST;
static uint8_t num_sensors_open = 0;
static IssSensors_Handle * g_pSenHndl[ISS_SENSORS_MAX_SUPPORTED_SENSOR];

#define COMMON_DES_CFG_SIZE    (69U)
static I2cParams ub960DesCfg_Common[COMMON_DES_CFG_SIZE] = {
    {0x01, 0x02, 0x20},
    {0x1f, 0x00, 0x00},

    {0x0D, 0x90, 0x1}, /*I/O to 3V3 - Options not valid with datashee*/
    {0x0C, 0x0F, 0x1}, /*Enable All ports*/

    /*Select Channel 0*/                                               
    {0x4C, 0x01, 0x10},
    {0x58, 0x5E, 0x1}, /*Enable Back channel, set to 50Mbs*/

    /*Select Channel 1*/
    {0x4C, 0x12, 0x10},
    {0x58, 0x5E, 0x1},/*Enable Back channel, set to 50Mbs*/

    /*Select Channel 2*/
    {0x4C, 0x24, 0x10},
    {0x58, 0x5E, 0x1},/*Enable Back channel, set to 50Mbs*/

    /*Select Channel 3*/
    {0x4C, 0x38, 0x10},
    {0x58, 0x5E, 0x1},/*Enable Back channel, set to 50Mbs*/

    {0x20, 0x00, 0x1}, /*Forwarding on and using CSIport 0 */

    /*Sets GPIOS*/     
    {0x10, 0x83, 0x1},  /* PORT0 Signals */
    {0x11, 0xA3, 0x1},
    {0x12, 0xC3, 0x1},
    {0x13, 0xE3, 0x1},

    {0x4C, 0x01, 0x10}, /* 0x01 RX0 */
    {0x32, 0x01, 0x1}, /*Enable TX port 0*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0xBC, 0x00, 0x1}, /*Unknown*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x7C, 0x0}, /*CSI Mode*/
    {0x72, 0x00, 0x0}, /*VC Map - All to 0 */

    {0x4C, 0x12, 0x10}, /* 0x12 RX1 */
    {0x32, 0x01, 0x1}, /*Enable TX port 0*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0xBC, 0x00, 0x1}, /*Unknown*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x7C, 0x0}, /*CSI Mode*/
    {0x72, 0x00, 0x0}, /*VC Map - All to 1 */

    {0x4C, 0x24, 0x10}, /* 0x24 RX2 */
    {0x32, 0x01, 0x1}, /*Enable TX port 0*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0xBC, 0x00, 0x1}, /*Unknown*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x7C, 0x0}, /*CSI Mode*/
    {0x72, 0x00, 0x0}, /*VC Map - All to 2 */

    {0x4C, 0x38, 0x10}, /* 0x38 RX3 */
    {0x32, 0x01, 0x1}, /*Enable TX port 0*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0xBC, 0x00, 0x1}, /*Unknown*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x7C, 0x0}, /*CSI Mode*/
    {0x72, 0x00, 0x0}, /*VC Map - All to 3 */

    /* Enable external frame sync */
    {0x4C, 0x01, 0x10}, // Chan 0
    {0x6E, 0xA0, 0x10}, // BC GPIO1 FS
    {0x4C, 0x12, 0x10}, // Chan 1
    {0x6E, 0xA0, 0x10}, // BC GPIO1 FS
    {0x4C, 0x24, 0x10}, // Chan 2
    {0x6E, 0xA0, 0x10}, // BC GPIO1 FS
    {0x4C, 0x38, 0x10}, // Chan 3
    {0x6E, 0xA0, 0x10}, // BC GPIO1 FS

    {0x0F, 0x20, 0x10}, // GPIO5 Input Enable.

    {0x18, 0xD0, 0x10}, // Enable FrameSync External GPIO5
    {0x20, 0xF0, 0x1}, /*Forwarding off and using CSIport 0 */

    {0xFFFF, 0x00, 0x0} //End of script
};

// Addresses for all 8 sensors
static uint32_t devAddresses[8] = {
    SENSOR_0_I2C_ALIAS, SENSOR_1_I2C_ALIAS, SENSOR_2_I2C_ALIAS, SENSOR_3_I2C_ALIAS, 
    SENSOR_4_I2C_ALIAS, SENSOR_5_I2C_ALIAS, SENSOR_6_I2C_ALIAS, SENSOR_7_I2C_ALIAS
};

int32_t initFusion2_UB97x()
{
    int32_t status = -1;
    uint32_t tca6408I2CSlaveAddr = 0x20;
    uint8_t regAddr = 0x3;
    uint8_t regVal = 0xFE;

    status = Board_i2c8BitRegWr(gISS_Sensor_I2cHandle, tca6408I2CSlaveAddr, regAddr, &regVal, 1U, SENSOR_I2C_TIMEOUT);
    if(0 == status)
    {
        issLogPrintf("write 0x%x to TCA6408 register 0x%x \n", regVal, regAddr);
    }else
    {
        printf("Error writing to TCA6408 register 0x%x \n", regAddr);
    }

    appLogWaitMsecs(100);

    return status;
}

int32_t deInitFusion2_UB97x()
{
    int32_t status = 0;
    return status;
}

void getIssSensorI2cInfo(uint8_t * byteOrder, I2C_Handle * i2cHndl)
{
    *byteOrder = gISS_Sensor_ByteOrder;
    *i2cHndl = gISS_Sensor_I2cHandle;
}

static int32_t setupI2CInst(uint8_t i2cInst)
{
    int32_t status = -1;
    I2C_Params i2cParams;

    /* Initializes the I2C Parameters */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    /* Configures the I2C instance with the passed parameters*/
    if(NULL == gISS_Sensor_I2cHandle)
    {
        gISS_Sensor_I2cHandle = I2C_open(i2cInst, &i2cParams);
    }

    if(gISS_Sensor_I2cHandle == NULL)
    {
        printf(" I2C: ERROR: I2C Open Failed for instance %d !!!\n", i2cInst);
        status = -1;
    }
    else
    {
        status = 0;
    }

    return status;
}

// enable_bank is the 960 config bits pos is the 1 first camera in group 2 second camera in group
static uint8_t getCameraPortConfig(uint8_t enable_bank, uint8_t pos)   
{
    uint8_t found = 0;
    int cam_port = 0;
    for (cam_port = 0; cam_port < 4; cam_port++) {
        if ((enable_bank & (uint8_t)(1 << cam_port)) != 0) {
            found++;
            if(found == pos)
                return (cam_port << 4) | (1 << cam_port);
        }
    }
    return 0;
}


int32_t IssSensor_Init()
{
    int32_t status = -1;
    uint32_t cnt;
    uint8_t  ub960I2cInstId;
    uint8_t  ub960I2cAddr;


    for(cnt=0;cnt<ISS_SENSORS_MAX_SUPPORTED_SENSOR;cnt++)
    {
        g_pSenHndl[cnt] = NULL;
    }
    
    Board_fpdU960GetI2CAddr(&ub960I2cInstId, &ub960I2cAddr, BOARD_CSI_INST_0);
    status = setupI2CInst(ub960I2cInstId);
    if(status!=0)
    {
        printf("++++++++++++++++++++++>>>>> FAILED TO INITIALIZE I2C\n");
        return status;
    }

    status = appRemoteServiceRegister(
            IMAGE_SENSOR_REMOTE_SERVICE_NAME,
            ImageSensor_RemoteServiceHandler
            );

    if(status!=0)
    {
        printf(" REMOTE_SERVICE_SENSOR: ERROR: Unable to register remote service sensor handler\n");
        return status;
    }

    for (cnt = 0U; cnt < ISS_SENSORS_MAX_SUPPORTED_SENSOR; cnt ++)
    {
        gIssSensorTable[cnt] = NULL;
    }

    /*
       Call init function of all supported sensors.
       This will register the sensors with the framework
       */
    status |= IssSensor_IMX390_Init();
    status |= IssSensor_IMX390CM_Init();
    status |= IssSensor_AR0233_Init();
    status |= IssSensor_AR0820_Init();
    status |= IssSensor_rawtestpat_Init();
    status |= IssSensor_testpat_Init();
    status |= IssSensor_gw_ar0233_Init();
    status |= IssSensor_HIMAX5530_Init();

    return status;
}

int32_t IssSensor_Register(IssSensors_Handle *pSensorHandle)
{
    int32_t status = 0;
    uint32_t cnt = 0U;

    if (NULL == pSensorHandle)
    {
        status = -2;
    }
    else
    {
        /* Find a free entry in the sensor table */
        for (cnt = 0U; cnt < ISS_SENSORS_MAX_SUPPORTED_SENSOR; cnt ++)
        {
            if (NULL == gIssSensorTable[cnt])
            {
                gIssSensorTable[cnt] = pSensorHandle;
                issLogPrintf("Found sensor %s at location %d \n", pSensorHandle->createPrms->name, cnt);
                break;
            }
        }

        if (cnt == ISS_SENSORS_MAX_SUPPORTED_SENSOR)
        {
            printf(" ISS_SENSOR: Could not register sensor \n");
            status = -2;
        }
    }

    return (status);
}

IssSensors_Handle * getSensorHandleFromName(char *name)
{
    uint32_t cnt;
    IssSensors_Handle *pSensorHandle = NULL;
    /* Check For Errors */
    if (NULL == name)
    {
        return NULL;
    }
    else
    {
        for (cnt = 0U; cnt < ISS_SENSORS_MAX_SUPPORTED_SENSOR; cnt ++)
        {
            pSensorHandle = gIssSensorTable[cnt];
            if(NULL == pSensorHandle)
            {
                issLogPrintf("pSensorHandle is NULL \n");
                return NULL;
            }
            if(NULL == pSensorHandle->createPrms)
            {
                issLogPrintf("createPrms is NULL \n");
                return NULL;
            }
            if (0 == strncmp(pSensorHandle->createPrms->name, name, ISS_SENSORS_MAX_NAME))
            {
                break;
            }
        }
    }
    return pSensorHandle;
}

int32_t IssSensor_GetSensorInfo(char *name, IssSensor_CreateParams *sensor_prms)
{
    int32_t status = -1;
    uint32_t cnt;
    IssSensors_Handle *pSensorHandle = NULL;

    /* Check For Errors */
    if ((NULL == sensor_prms) || (NULL == name))
    {
        status = -2;
    }
    else
    {
        for (cnt = 0U; cnt < ISS_SENSORS_MAX_SUPPORTED_SENSOR; cnt ++)
        {
            pSensorHandle = gIssSensorTable[cnt];
            if(NULL == pSensorHandle)
            {
                issLogPrintf("pSensorHandle is NULL \n");
                return -1;
            }
            if(NULL == pSensorHandle->createPrms)
            {
                issLogPrintf("createPrms is NULL \n");
                return -1;
            }
            if (0 == strncmp(pSensorHandle->createPrms->name, name, ISS_SENSORS_MAX_NAME))
            {
                memcpy(sensor_prms, pSensorHandle->createPrms, sizeof(IssSensor_CreateParams));
                status = 0;
                break;
            }
        }
    }
    return (status);
}

#if 0
int32_t IssSensor_GetSensorInfoFromDccId(uint32_t dccId, IssSensor_Info *pInfo)
{
    int32_t status = -1;
    uint32_t cnt;
    IssSensors_Handle *pSensorHandle = NULL;
    IssSensor_CreateParams *pCreatePrms = NULL;

    appLogPrintf("Entering IssSensor_GetSensorInfoFromDccId \n");

    /* Check For Errors */
    if (NULL == pInfo)
    {
        status = -2;
    }
    else
    {
        for (cnt = 0U; cnt < ISS_SENSORS_MAX_SUPPORTED_SENSOR; cnt ++)
        {
            pSensorHandle = gIssSensorTable[cnt];
            pCreatePrms = pSensorHandle->createPrms;
            if (pCreatePrms->dccId == dccId)
            {
                memcpy(pInfo, pCreatePrms->sensorInfo, sizeof(IssSensor_Info));
                status = 0;
                break;
            }
        }
    }
    return (status);
}
#endif

IssSensors_Handle * IssSensor_GetSensorHandle(char * name)
{
    uint32_t cnt;
    IssSensors_Handle *pSensorHandle = NULL;
    IssSensor_CreateParams *pCreatePrms = NULL;

    /* Check For Errors */
    if (NULL != name)
    {
        for (cnt = 0U; cnt < ISS_SENSORS_MAX_SUPPORTED_SENSOR; cnt ++)
        {
            pSensorHandle = gIssSensorTable[cnt];
            if(NULL == pSensorHandle)
            {
                return NULL;
            }

            pCreatePrms = pSensorHandle->createPrms;

            if (vx_true_e == pSensorHandle->isUsed)
            {
                if(0 == strncmp(pCreatePrms->name, name, ISS_SENSORS_MAX_NAME))
                {
                    break;
                }
            }
        }
    }
    return (void*)(pSensorHandle);
}

int32_t IssSensor_Delete(void* handle)
{
    int32_t status = -1;
    IssSensors_Handle *pSensorHandle;
    printf("Entering IssSensor_Delete \n");

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        pSensorHandle = (IssSensors_Handle *)handle;

        memset(pSensorHandle->createPrms, 0, sizeof(IssSensor_CreateParams));
    }
    else
    {
        issLogPrintf("Warning : channel handle is invalid \n");
    }

    return (status);
}

int32_t IssSensor_Config(void* handle, uint32_t chId, uint32_t feat)
{
    int32_t status = -1;
    IssSensors_Handle *pSensorHandle;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        pSensorHandle = (IssSensors_Handle *)handle;
        //if(chId < pSensorHandle->createPrms->num_channels)
        if(1)
        {
            if (NULL != pSensorHandle->sensorFxns->config)
            {
                status = pSensorHandle->sensorFxns->config(chId, handle, feat);
            }
        }
        else
        {
            printf("IssSensor_Config Error : Incorrect channel ID %d \n", chId);
            status = -1;
        }
    }
    else
    {
        issLogPrintf("Warning : channel ID %d handle is invalid \n", chId);
    }

    return (status);
}


static int32_t UB953_WriteReg(uint8_t   i2cInstId,
        uint8_t   slaveI2cAddr,
        uint16_t  regAddr,
        uint8_t   regVal)
{
    int32_t status = -1;
    printf("UB953 write: 0x%2x[0x%2x] = 0x%2x\n", slaveI2cAddr, regAddr, regVal);
    status = Board_i2c8BitRegWr(gISS_Sensor_I2cHandle, slaveI2cAddr, regAddr, &regVal, 1U, SENSOR_I2C_TIMEOUT);
    if(0 != status)
    {
        printf("Error writing 0x%x to UB953 register 0x%x \n", regVal, regAddr);
    }
    return status;
}

static int32_t UB960_ReadReg(uint8_t   i2cInstId,
        uint8_t   slaveI2cAddr,
        uint16_t  regAddr,
        uint8_t   *regVal)
{
    int32_t status = -1;
    status = Board_i2c8BitRegRd(gISS_Sensor_I2cHandle, slaveI2cAddr, regAddr, regVal, 1U, SENSOR_I2C_TIMEOUT);
    if(0 != status)
    {
        printf("Error reading from UB960 register 0x%x \n", regAddr);
    }
    return status;
}

static int32_t UB960_WriteReg(uint8_t   i2cInstId,
        uint8_t   slaveI2cAddr,
        uint16_t  regAddr,
        uint8_t   regVal)
{
    int32_t status = -1;
//    printf("UB960 write: 0x%2x[0x%2x] = 0x%2x\n", slaveI2cAddr, regAddr, regVal);
    status = Board_i2c8BitRegWr(gISS_Sensor_I2cHandle, slaveI2cAddr, regAddr, &regVal, 1U, SENSOR_I2C_TIMEOUT);
    if(0 != status)
    {
        printf("Error writing 0x%x to UB960 register 0x%x \n", regVal, regAddr);
    }
    return status;
}

/*Checks which cameras are connected to the specified Deserializer*/
/*Returns a 4-bit mask - 0 = Ser Not Detected, 1 = Ser Detected*/
static int32_t IssSensor_detect_serializer(int8_t ub960InstanceId, uint8_t * cameras_detected)
{
    uint8_t mask = 0x0;
    uint8_t pageSelectOrig = 0x0;
    uint8_t regVal = 0x0;
    uint8_t found = 0x0;
    int32_t status = -1;
    uint8_t  ub960I2cInstId;
    uint8_t  ub960I2cAddr;

    Board_fpdU960GetI2CAddr(&ub960I2cInstId, &ub960I2cAddr, ub960InstanceId);

    status = UB960_ReadReg(ub960I2cInstId, ub960I2cAddr, 0x4C, &pageSelectOrig);
    if(status != 0)
    {
        return status;
    }

    status = UB960_WriteReg(ub960I2cInstId, ub960I2cAddr, 0x4C, 0x01);
    if(status != 0)
    {
        return status;
    }

    status = UB960_ReadReg(ub960I2cInstId, ub960I2cAddr, 0x4D, &regVal);
    if(status != 0)
    {
        return status;
    }

    found = regVal & 0x3;//LOCK_STS and PORT_PASS
    if(found == 0x3)
    {
        mask |=  0x1;/*Set bit#0*/
    }

    status = UB960_WriteReg(ub960I2cInstId, ub960I2cAddr, 0x4C, 0x12);
    if(status != 0)
    {
        return status;
    }

    status = UB960_ReadReg(ub960I2cInstId, ub960I2cAddr, 0x4D, &regVal);
    if(status != 0)
    {
        return status;
    }

    found = regVal & 0x3;//LOCK_STS and PORT_PASS
    if(found == 0x3)
    {
        mask |=  0x2;/*Set bit#1*/
    }

    status = UB960_WriteReg(ub960I2cInstId, ub960I2cAddr, 0x4C, 0x24);
    if(status != 0)
    {
        return status;
    }

    status = UB960_ReadReg(ub960I2cInstId, ub960I2cAddr, 0x4D, &regVal);
    if(status != 0)
    {
        return status;
    }

    found = regVal & 0x3;//LOCK_STS and PORT_PASS
    if(found == 0x3)
    {
        mask |=  0x4;/*Set bit#2*/
    }

    status = UB960_WriteReg(ub960I2cInstId, ub960I2cAddr, 0x4C, 0x38);
    if(status != 0)
    {
        return status;
    }

    status = UB960_ReadReg(ub960I2cInstId, ub960I2cAddr, 0x4D, &regVal);
    if(status != 0)
    {
        return status;
    }

    found = regVal & 0x3;//LOCK_STS and PORT_PASS
    if(found == 0x3)
    {
        mask |=  0x8;/*Set bit#3*/
    }

    status = UB960_WriteReg(ub960I2cInstId, ub960I2cAddr, 0x4C, pageSelectOrig);

    if(ub960InstanceId == BOARD_CSI_INST_0)
    {
        *cameras_detected |= mask;
    }else if(ub960InstanceId == BOARD_CSI_INST_1)
    {
        *cameras_detected |= (mask << 4U);
    }

    return status;
}

/*Probes all the sensors in gIssSensorTable*/
/*If a sensor is detected, the index of the sensor in gIssSensorTable is copied to sensor_id_list*/
static int32_t IssSensor_detect_sensor(uint8_t chId)
{
    int32_t probeStatus = -1;
    int32_t i = 0;
    IssSensors_Handle * pSenHndl;
    int32_t sensor_id_found = -1;

    while ((sensor_id_found < 0) && (i<ISS_SENSORS_MAX_SUPPORTED_SENSOR))
    {
        pSenHndl = gIssSensorTable[i];
        if(NULL !=pSenHndl)
        {
            probeStatus = pSenHndl->sensorFxns->probe(chId, pSenHndl);

            if(probeStatus == 0 )
            {
                /*Sensor found at index i*/
                issLogPrintf("Found sensor %s at port # %d\n", pSenHndl->createPrms->name, chId);
                sensor_id_found = i;
            }
        }
        i++;
    }

    return sensor_id_found;
}

/*Checks which cameras are connected to the specified Deserializer*/
/*Returns a 4-bit mask - 0 = Camera Not Detected, 1 = Camera Detected*/
int32_t IssSensor_DeserializerInit(/*uint8_t *mask*/)
{
    int32_t status = -1;

    issLogPrintf("Deserializer Init.\n");

#ifdef BUILD_TDA4_RVP
    /* Power cycle cameras... */
    ImageSensor_RVPFPDSetPower(0);
    appLogWaitMsecs(100);
    ImageSensor_RVPFPDSetPower(0x00);
    appLogWaitMsecs(500);
    printf("TDA4RVP: Performing FPD-Link power cycle.\n");
#endif

    /* UB960 Global configuration is done here which should be shared by all sensors*/
    /* Sensor driver is not supposed to overwrite global configuration*/
    /* Sensor driver can only modify the page corresponding to the channel ID selected*/
    status = ub960_cfgScript(ub960DesCfg_Common, 0U);        
    if(status!=0)
    {
        printf("Error : ub960_cfgScript returned %d while configuring DES 0 \n", status);
        return status;
    }

    status = ub960_cfgScript(ub960DesCfg_Common, 1U);
    if(status!=0)
    {
        printf("Error : ub960_cfgScript returned %d while configuring DES 1 \n", status);
        return status;
    }
    return status;
}

int32_t IssSensor_PowerOn(void* handle, uint32_t chMask)
{
    int32_t status = -1;
    IssSensors_Handle *pSensorHandle;


    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        uint32_t chId = 0;
        pSensorHandle = (IssSensors_Handle *)handle;
        pSensorHandle->sensorIntfPrms->numCamerasStreaming = 0;
        // while ((chMask > 0) && (chId < ISS_SENSORS_MAX_SUPPORTED_SENSOR))
        while (chId < ISS_SENSORS_MAX_SUPPORTED_SENSOR)
        {
            if(chMask & 0x1)
            {
                if (NULL != pSensorHandle->sensorFxns->powerOn)
                {                    
                    printf("PowerOn for channel: %i mask: 0x%x\n", chId, chMask); 
                    status = pSensorHandle->sensorFxns->powerOn(chId, handle);
                    num_sensors_open++;
                }
            }
            chMask = chMask >> 1;
            chId++;
        }
    }
    else
    {
        issLogPrintf("Warning : channel handle is invalid \n");
    }

    issLogPrintf("IssSensor_PowerOn : num_sensors_open %d \n ", num_sensors_open);  
    return (status);

}

int32_t IssSensor_PowerOff(void* handle, uint32_t chId)
{
    int32_t status = -1;
    IssSensors_Handle *pSensorHandle;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        pSensorHandle = (IssSensors_Handle *)handle;

        if(pSensorHandle->sensorIntfPrms->numCamerasStreaming != 0)
        {
            issLogPrintf("IssSensor_PowerOff : Warning %d cameras are still streaming \n ", pSensorHandle->sensorIntfPrms->numCamerasStreaming);        
        }

        if (NULL != pSensorHandle->sensorFxns->powerOff)
        {
            issLogPrintf("IssSensor_PowerOff : chID: %d \n ", chId);    
            status = pSensorHandle->sensorFxns->powerOff(chId, handle);
        }
    }else{
        issLogPrintf("IssSensor_PowerOff : Invalid handle \n ");    
    }

    num_sensors_open--;
    issLogPrintf("IssSensor_PowerOff : num_sensors_open %d \n ", num_sensors_open);  

    return (status);

}

int32_t IssSensor_Start(void* handle, uint32_t chId)
{
    int32_t status = -1;
    uint32_t numCamerasEnabled = 0;
    IssSensors_Handle *pSensorHandle = (IssSensors_Handle *)NULL;
    printf("Entering IssSensor_Start %i \n", chId);

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        pSensorHandle = (IssSensors_Handle *)handle;
        if(1U == pSensorHandle->sensorIntfPrms->sensorBroadcast)
        {
            issLogPrintf("Turning on broadcast mode \n");
            if(0x0 == chId)
            {
                /*Broadcast mode - UB960_0*/
                uint8_t i;
                for(i=0;i<4U;i++)
                {
                    g_pSenHndl[i] = pSensorHandle;
                }
                numCamerasEnabled = 4;
            }
            else if(0x04 == chId)
            {
                /*Broadcast mode - UB960_1*/
                uint8_t i;
                for(i=4;i<ISS_SENSORS_MAX_SUPPORTED_SENSOR;i++)
                {
                    g_pSenHndl[i] = pSensorHandle;
                }
                numCamerasEnabled = 4;
            }
        }
        else
        {
            numCamerasEnabled = 1;
        }

        if(NULL != g_pSenHndl[chId])
        {
            issLogPrintf("Warning : channel ID %d already has a registered sensor handle \n", chId);
        }
        issLogPrintf("Adding HANDLE: channel ID %d \n", chId);
        g_pSenHndl[chId] = pSensorHandle;

        if(NULL != pSensorHandle->sensorFxns->streamOn)
        {
            status = pSensorHandle->sensorFxns->streamOn(chId, handle);
        }

        if(status == 0)
        {
            pSensorHandle->sensorIntfPrms->numCamerasStreaming += numCamerasEnabled;
        }
    }
    else
    {
        issLogPrintf("Warning : channel ID %d handle is invalid \n", chId);
    }

    return (status);
}

int32_t IssSensor_Stop(void* handle, uint32_t chId)
{
    int32_t status = -1;
    IssSensors_Handle *pSensorHandle;
    printf("Entering IssSensor_Stop \n");

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        pSensorHandle = (IssSensors_Handle *)handle;

        if (1) //(chId < pSensorHandle->createPrms->num_channels)
        {
            if(NULL != pSensorHandle->sensorFxns->streamOff)
            {
                status |= pSensorHandle->sensorFxns->streamOff(chId, handle);
            }
            pSensorHandle->sensorIntfPrms->numCamerasStreaming -= 1;
        }
        else
        {
            printf("IssSensor_Stop Error: Incorrect channel ID %d \n", chId);
            status = -1;
        }
    }

    g_pSenHndl[chId] = NULL;

    return (status);
}

int32_t IssSensor_SetAeParams(void *handle, uint32_t chId, IssSensor_ExposureParams *pExpPrms)
{
    int32_t status = -1;
    IssSensors_Handle * pSensorHandle = NULL;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        pSensorHandle = (IssSensors_Handle *)handle;
        if(pSensorHandle != NULL)
        {
            if(NULL != pSensorHandle->sensorFxns->setAeParams)
            {
                status |= pSensorHandle->sensorFxns->setAeParams(handle, chId, pExpPrms);
            }
        }
        else
        {
            printf("IssSensor_SetAeParams Error: sensor handle is NULL for channel ID %d \n", chId);
            status = -1;
        }
    }

    return (status);
}

int32_t IssSensor_SetAwbParams(void *handle, uint32_t chId, IssSensor_WhiteBalanceParams *pWbPrms)
{
    int32_t status = -1;
    IssSensors_Handle * pSensorHandle = NULL;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        pSensorHandle = (IssSensors_Handle *)handle;
        //if(chId < pSensorHandle->createPrms->num_channels)
        if(pSensorHandle != NULL)
        {
            if(NULL != pSensorHandle->sensorFxns->setAwbParams)
            {
                status |= pSensorHandle->sensorFxns->setAwbParams(handle, chId, pWbPrms);
            }
            else
            {
                printf("IssSensor_SetAwbParams Error : Sensor setAwbParams callback is NULL \n");
                memset(pWbPrms, 0x0, sizeof(IssSensor_WhiteBalanceParams));
            }
        }
        else
        {
            printf("IssSensor_SetAwbParams Error : sensor handle is NULL for  channel ID %d \n", chId);
            status = -1;
        }
    }

    return (status);
}

int32_t IssSensor_Control(void* handle, uint32_t cmd, void* cmdArgs, void* cmdRetArgs)
{
    int32_t status = VX_FAILURE;
    uint32_t chId = 0xFF;
    IMAGE_SENSOR_CTRLCMD ctrlCmd;
    uint8_t * cmd_ptr = (uint8_t *)cmdArgs;

    if(NULL == cmd_ptr)
    {
        printf("Error : cmd_ptr is NULL \n");
        return VX_FAILURE;
    }

    cmd_ptr += ISS_SENSORS_MAX_NAME;

    memcpy(&chId, cmd_ptr, sizeof(uint32_t));
    cmd_ptr += sizeof(uint32_t);

    memcpy(&ctrlCmd, cmd_ptr, sizeof(IMAGE_SENSOR_CTRLCMD));
    cmd_ptr += sizeof(IMAGE_SENSOR_CTRLCMD);

    switch(ctrlCmd)
    {
        case IMAGE_SENSOR_CTRLCMD_GETEXPPRG:
            {
                IssSensors_Handle * pSenHndl = g_pSenHndl[chId];
                if(NULL != pSenHndl)
                {
                    status = pSenHndl->sensorFxns->getExpPrg(chId, (void*)pSenHndl, (IssAeDynamicParams *)cmd_ptr);
                }
                else
                {
                    printf("Error : sensor handle is NULL for channel %d \n", chId);
                    status = VX_FAILURE;
                }
            }
            break;
        case IMAGE_SENSOR_CTRLCMD_GETWBCFG:
            {
                IssSensors_Handle * pSenHndl = g_pSenHndl[chId];
                if(NULL != pSenHndl)
                {
                    if(NULL != pSenHndl->sensorFxns)
                    {
                        if(NULL != pSenHndl->sensorFxns->getWbCfg)
                        {
                            status = pSenHndl->sensorFxns->getWbCfg(chId, (void*)pSenHndl, (IssAwbDynamicParams *)cmd_ptr);
                        }else
                        {
                            /*Sensor driver does not support WB update API*/
                            memset(cmd_ptr, 0, sizeof(IssAwbDynamicParams));
                        }
                    }
                }
                else
                {
                    memset(cmd_ptr, -1, sizeof(IssAwbDynamicParams));
                    status = VX_FAILURE;
                }
            }
            break;  
        case IMAGE_SENSOR_CTRLCMD_SETEXPGAIN:
            {
                IssSensor_ExposureParams aePrms;
                IssSensors_Handle * pSenHndl = g_pSenHndl[chId];
                if(NULL != pSenHndl)
                {
                    memcpy(&aePrms, cmd_ptr, sizeof(IssSensor_ExposureParams));
                    status = IssSensor_SetAeParams(pSenHndl, chId, &aePrms);
                }
                else
                {
                    printf("Error : sensor handle is NULL for channel %d \n", chId);
                    status = VX_FAILURE;
                }                
            }
            break;
        case IMAGE_SENSOR_CTRLCMD_GETEXPGAIN:
            /*Reserved for future use*/
            status = 0;
            break;
        case IMAGE_SENSOR_CTRLCMD_SETWBGAIN:
            {
                IssSensor_WhiteBalanceParams awbPrms;
                IssSensors_Handle * pSenHndl = g_pSenHndl[chId];
                if(NULL != pSenHndl)
                {
                    memcpy(&awbPrms, cmd_ptr, sizeof(IssSensor_WhiteBalanceParams));
                    status = IssSensor_SetAwbParams(pSenHndl, chId, &awbPrms);
                }
                else
                {
                    printf("Error : sensor handle is NULL for channel %d \n", chId);
                    status = VX_FAILURE;
                }         
            }
            break;
        case IMAGE_SENSOR_CTRLCMD_GETWBGAIN:
            /*Reserved for future use*/
            status = 0;
            break;
        case IMAGE_SENSOR_CTRLCMD_DEBUG:
            /*Reserved for future use*/
            status = 0;
            issLogPrintf("IMAGE_SENSOR_CTRLCMD_DEBUG \n");
            {
                uint32_t * ptr32 = (uint32_t * )cmd_ptr;
                IssSensors_Handle * pSenHndl = g_pSenHndl[chId];
                uint32_t rw_flag = *ptr32++; /*ReadWrite Flag = 1 for Write*/
                uint32_t devType = *ptr32++; /*Device Type*/
                uint32_t regAddr32 = *ptr32++; /*Register Address*/
                uint32_t regVal32 = *ptr32; /*Register Value*/
                uint32_t slaveI2cAddr = 0x0;
                uint8_t regAddr = 0xBC;
                uint8_t regVal = 0xDE;

                if(0U == devType)
                {
                    slaveI2cAddr = 0x3D ;//pSenHndl->createPrms->i2cAddrDes[chId];
                }
                else if(1U == devType)
                {
                    slaveI2cAddr = pSenHndl->createPrms->i2cAddrSer[chId];
                }
                else if(2U == devType)
                {
                    slaveI2cAddr = pSenHndl->createPrms->i2cAddrSensor[chId];
                }
                else
                {
                    printf("IMAGE_SENSOR_CTRLCMD_DEBUG Error : Unsupported devType %d \n", devType);
                    return -1;
                }

                if(2U == devType)
                {
                    I2cParams sensorI2cParams;
                    sensorI2cParams.nDelay = 1U;
                    sensorI2cParams.nRegAddr = (uint16_t)(regAddr32);
                    sensorI2cParams.nRegValue = (uint16_t)(regVal32);
                    status = pSenHndl->sensorFxns->readWriteReg(chId, pSenHndl, rw_flag, &sensorI2cParams);
                    if(0 != status)
                    {
                        printf("IMAGE_SENSOR_CTRLCMD_DEBUG : Error reading from register 0x%x on the image sensor %s \n", regAddr, pSenHndl->createPrms->name);
                    }
                    else
                    {
                        ptr32 = (uint32_t * )cmdArgs;
                        *ptr32 = (uint32_t)sensorI2cParams.nRegValue;
                    }
                }
                else
                {
                    if(0U == rw_flag)
                    {
                        regAddr = (uint8_t)(regAddr32);
                        status = Board_i2c8BitRegRd(gISS_Sensor_I2cHandle, slaveI2cAddr, regAddr, &regVal, 1U, SENSOR_I2C_TIMEOUT);
                        if(0 != status)
                        {
                            printf("IMAGE_SENSOR_CTRLCMD_DEBUG : Error reading from register 0x%x on slave device 0x%x \n", regAddr, slaveI2cAddr);
                        }
                        issLogPrintf("IMAGE_SENSOR_CTRLCMD_DEBUG : Read 0x%x from register 0x%x on slave device 0x%x \n", regVal, regAddr, slaveI2cAddr);
                    }
                    else if (1U == rw_flag)
                    {
                        regAddr = (uint8_t)(regAddr32);
                        regVal =  (uint8_t)(regVal32);
                        status = Board_i2c8BitRegWr(gISS_Sensor_I2cHandle, slaveI2cAddr, regAddr, &regVal, 1U, SENSOR_I2C_TIMEOUT);
                        if(0 != status)
                        {
                            printf("IMAGE_SENSOR_CTRLCMD_DEBUG : Error writing 0x%x to register 0x%x on slave device 0x%x \n", regVal, regAddr, slaveI2cAddr);
                        }
                    }
                    else
                    {
                        status = -1;
                        printf("IMAGE_SENSOR_CTRLCMD_DEBUG Error : Invalid rwflag = %d \n", rw_flag);
                    }

                    if(0 == status)
                    {
                        ptr32 = (uint32_t * )cmdArgs;
                        *ptr32 = (uint32_t)regVal;
                    }
                }
            }
            break;
        case IMAGE_SENSOR_CTRLCMD_READ_SENSOR_REG:
            {
                uint32_t * ptr32 = (uint32_t * )cmd_ptr;
                IssSensors_Handle * pSenHndl = g_pSenHndl[chId];
                I2cParams reg_read;
                reg_read.nDelay = 0;
                reg_read.nRegAddr = (uint16_t)(*ptr32);
                ptr32++;
                reg_read.nRegValue = 0xFF;
                status = pSenHndl->sensorFxns->readWriteReg(chId, pSenHndl, 0, &reg_read);
                *ptr32 = reg_read.nRegValue;
            }
            break;
        case IMAGE_SENSOR_CTRLCMD_WRITE_SENSOR_REG:
            {
                uint32_t * ptr32 = (uint32_t * )cmd_ptr;
                IssSensors_Handle * pSenHndl = g_pSenHndl[chId];
                I2cParams reg_write;
                reg_write.nDelay = 0;
                reg_write.nRegAddr = (uint16_t)(*ptr32);
                ptr32++;
                reg_write.nRegValue = (uint16_t)(*ptr32);
                status = pSenHndl->sensorFxns->readWriteReg(chId, pSenHndl, 1, &reg_write);
                *ptr32 = reg_write.nRegValue;
            }
            break;
        default:
            status = -1;
            printf("IMAGE_SENSOR_CTRLCMD_DEBUG Error : Unknown control command %d \n", ctrlCmd);
            break;
    }

    return (status);
}

static uint32_t sensor_updateCameraPairTemp(CameraTempReadings * pCamTemp)
{
    int32_t status = 0;
    I2C_Handle sensorI2cHandle = NULL;
    uint8_t sensorI2cByteOrder = 255U;
    int i = 0;
    uint32_t mask = pCamTemp->mask;
    uint32_t regData[8] = { 0 };
    uint32_t iAddress = 0;

    if(mask == 0x00) // no cameras active
        return 0;

    getIssSensorI2cInfo(&sensorI2cByteOrder, &sensorI2cHandle);
    
    for (i = 0; i < 8; i++) {
        // printf("Get Camera: %d in Camera-set: 0x%02x\n", i+1, mask);
        if (mask & (1 << i))  // check if camera is enabled
        {
            iAddress = devAddresses[i];
            // printf("Address: 0x%02x\n", (uint8_t)iAddress);
            status = Board_i2c16BitRegRd(sensorI2cHandle, iAddress, 0x2236,(uint8_t *) &(regData[i]), 1,sensorI2cByteOrder, 10);
            if(regData[i] > 0)
                status |= Board_i2c16BitRegRd(sensorI2cHandle, iAddress, 0x2237,(uint8_t *) &(regData[i]), 1,sensorI2cByteOrder, 10);

            if(status != 0)
            {
                issLogPrintf("Error reading camera temp sensors for Camera: %d in Camera-set: 0x%02x\n", i, mask);
                regData[i] = 0;
            }
        }
        else
        {
            regData[i] = 0;
        }
        
    }

    pCamTemp->cam_temp_lower = ((uint8_t)regData[3] << 24 | (uint8_t)regData[2] << 16 | (uint8_t)regData[1] << 8 | (uint8_t)regData[0]);
    pCamTemp->cam_temp_upper = ((uint8_t)regData[7] << 24 | (uint8_t)regData[6] << 16 | (uint8_t)regData[5] << 8 | (uint8_t)regData[4]);
    // issLogPrintf("MASK: 0x%02X TEMPs: %08X  %08X\n",(uint8_t)pCamTemp->mask, pCamTemp->cam_temp_upper, pCamTemp->cam_temp_lower);
    return status;
}

/*******************************************************************************
 *  Local Functions Definition
 *******************************************************************************
 */

static int32_t checkForHandle(void* handle)
{
    int32_t found = -1;
    uint32_t cnt;

    /* Find a free entry in the sensor table */
    for (cnt = 0U; cnt < ISS_SENSORS_MAX_SUPPORTED_SENSOR; cnt ++)
    {
        if (handle == (void*)gIssSensorTable[cnt])
        {
            found = 0;
            break;
        }
    }

    return (found);
}

static int32_t i2c_a16d8_WriteReg(uint8_t i2cInstId,
        uint8_t slaveI2cAddr,
        uint16_t regAddr,
        uint8_t regVal,
        const char *chip_name)
{
    int32_t status = -1;
    status = Board_i2c16BitRegWr(gISS_Sensor_I2cHandle, slaveI2cAddr, regAddr, &regVal,
            1U, BOARD_I2C_REG_ADDR_MSB_FIRST, I2C_WAIT_FOREVER);
    if (status != 0) {
        appLogPrintf("Error writing 0x%x to %s register 0x%x\n", regVal, chip_name, regAddr);
    }
    return status;
}

int32_t ub960_cfgScript(I2cParams *script, int8_t ub960InstanceId)
{
    uint16_t regAddr;
    uint8_t  regValue;
    uint16_t delayMilliSec;
    uint32_t cnt;
    int32_t status = 0;
    uint8_t  ub960I2cInstId;
    uint8_t  ub960I2cAddr;

    /*Assumption for Fusion1 board - maximum two UB960s*/
    if((ub960InstanceId > BOARD_CSI_INST_1) || (ub960InstanceId < BOARD_CSI_INST_0))
    {
        printf("Error : Invalid ub960InstanceId %d \n", ub960InstanceId);
        return -1;
    }
    Board_fpdU960GetI2CAddr(&ub960I2cInstId, &ub960I2cAddr, ub960InstanceId);
    // issLogPrintf("ub960_cfgScript : ub960I2cAddr = 0x%x \n", ub960I2cAddr);

    // issLogPrintf("UB960 config start \n");
    if(NULL != script)
    {
        cnt = 0;
        regAddr  = script[0].nRegAddr;

        while(regAddr != 0xFFFF)
        {
            /* Convert Registers address and value into 8bit array */
            regAddr  = script[cnt].nRegAddr;
            regValue = script[cnt].nRegValue;
            delayMilliSec = script[cnt].nDelay;
            status |= UB960_WriteReg(ub960I2cInstId, ub960I2cAddr, regAddr, regValue);

            if (0 != status)
            {
                printf(" UB960 Error: Reg Write Failed for regAddr %x\n", regAddr);
                break;
            }

            if(delayMilliSec > 0)
            {
                appLogWaitMsecs(delayMilliSec);
            }

            cnt++;
        }
    }
    // issLogPrintf("End of UB960 config \n");
    return (status);
}

int32_t ub953_cfgScript(uint8_t  i2cInstId, uint8_t  slaveAddr, I2cParams *script)
{
    uint16_t regAddr;
    uint8_t  regValue;
    uint16_t delayMilliSec;
    uint32_t cnt;
    int32_t status = 0;

    issLogPrintf("ub953 config start : slaveAddr = 0x%x \n", slaveAddr);
    if(NULL != script)
    {
        cnt = 0;
        regAddr  = script[0].nRegAddr;

        while(regAddr != 0xFFFF)
        {
            regAddr  = script[cnt].nRegAddr;
            regValue = script[cnt].nRegValue;
            delayMilliSec = script[cnt].nDelay;
            /* Convert Registers address and value into 8bit array */
            status |= UB953_WriteReg(i2cInstId, slaveAddr, regAddr, regValue);

            if (0 != status)
            {
                printf(" UB953 Error: Reg Write Failed for regAddr %x\n", regAddr);
                break;
            }
            cnt++;
            if(delayMilliSec > 0)
            {
                appLogWaitMsecs(delayMilliSec);
            }
        }
    }
    issLogPrintf(" End of UB953 config \n");
    return (status);
}

int32_t a16d8_cfgScript(uint8_t  i2cInstId, uint8_t  slaveAddr, I2cParams *script, const char *chip_name)
{
    uint16_t regAddr;
    uint8_t  regValue;
    uint16_t delayMilliSec;
    uint32_t cnt = 0;
    int32_t status = 0;

    appLogPrintf(" %s config start : slaveAddr = 0x%x \n", chip_name, slaveAddr);
    if(NULL != script) {
        do {
            regAddr  = script[cnt].nRegAddr;
            regValue = script[cnt].nRegValue;
            delayMilliSec = script[cnt].nDelay;

            status |= i2c_a16d8_WriteReg(i2cInstId, slaveAddr, regAddr, regValue, chip_name);
            if ( status != 0) {
                appLogPrintf(" %s: Reg Write Failed for regAddr %x\n", chip_name, regAddr);
                break;
            }
            appLogWaitMsecs(delayMilliSec);
            cnt++;
        } while (script[cnt].nRegAddr != 0xFFFF);
    }
    appLogPrintf(" End of %s config %d entries.\n", chip_name, cnt);
    return (status);
}

int32_t max9296_cfgScript(uint8_t i2cInstId, uint8_t slaveAddr, I2cParams *script)
{
    return a16d8_cfgScript(i2cInstId, slaveAddr, script, "MAX9296");
}

int32_t max9295_cfgScript(uint8_t i2cInstId, uint8_t slaveAddr, I2cParams *script)
{
    return a16d8_cfgScript(i2cInstId, slaveAddr, script, "MAX9295");
}

int32_t enableUB960Broadcast(int8_t ub960InstanceId)
{
    int32_t status = 0;
    I2cParams enableUB960BroadcastScript[5] =
    {
        {0x4C, 0x0F, 0x10},
        {0x65, (SER_0_I2C_ALIAS<< 1U), 0x10},
        {0x66, (SENSOR_0_I2C_ALIAS << 1U), 0x10},
        {0x72, 0xE4,0x10},
        {0xFFFF,0x00, 0x00}
    };

    if(0U == ub960InstanceId)
    {
        enableUB960BroadcastScript[1].nRegValue = (SER_0_I2C_ALIAS<<1U);
        enableUB960BroadcastScript[2].nRegValue = (SENSOR_0_I2C_ALIAS<<1U);
    } else if(1U == ub960InstanceId)
    {
        enableUB960BroadcastScript[1].nRegValue = (SER_4_I2C_ALIAS<<1U);
        enableUB960BroadcastScript[2].nRegValue = (SENSOR_4_I2C_ALIAS<<1U);
    }
    else
    {
        printf("enableUB960Broadcast: Error: Invalid instance ID \n");
        return -1;
    }

    status = ub960_cfgScript(enableUB960BroadcastScript, ub960InstanceId);

    return status;
}

int32_t disableUB960Broadcast(int8_t ub960InstanceId)
{
    int32_t status = 0;
    I2cParams disableUB960BroadcastScript[17] =
    {
        {0x4C, 0x01, 0x10},
        {0x65, (SER_0_I2C_ALIAS<< 1U), 0x10},
        {0x66, (SENSOR_0_I2C_ALIAS << 1U), 0x10},
        {0x72, 0x00,0x10},

        {0x4C, 0x12, 0x10},
        {0x65, (SER_1_I2C_ALIAS<< 1U), 0x10},
        {0x66, (SENSOR_1_I2C_ALIAS << 1U), 0x10},
        {0x72, 0x55,0x10},

        {0x4C, 0x24, 0x10},
        {0x65, (SER_2_I2C_ALIAS<< 1U), 0x10},
        {0x66, (SENSOR_2_I2C_ALIAS << 1U), 0x10},
        {0x72, 0xAA,0x10},

        {0x4C, 0x38, 0x10},
        {0x65, (SER_3_I2C_ALIAS<< 1U), 0x10},
        {0x66, (SENSOR_3_I2C_ALIAS << 1U), 0x10},
        {0x72, 0xFF,0x10},

        {0xFFFF,0x00, 0x00}
    };

    if(0U == ub960InstanceId)
    {
        disableUB960BroadcastScript[1].nRegValue = (SER_0_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[2].nRegValue = (SENSOR_0_I2C_ALIAS<<1U);

        disableUB960BroadcastScript[5].nRegValue = (SER_1_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[6].nRegValue = (SENSOR_1_I2C_ALIAS<<1U);

        disableUB960BroadcastScript[9].nRegValue = (SER_2_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[10].nRegValue = (SENSOR_2_I2C_ALIAS<<1U);

        disableUB960BroadcastScript[13].nRegValue = (SER_3_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[14].nRegValue = (SENSOR_3_I2C_ALIAS<<1U);
    } else if(1U == ub960InstanceId)
    {
        disableUB960BroadcastScript[1].nRegValue = (SER_4_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[2].nRegValue = (SENSOR_4_I2C_ALIAS<<1U);

        disableUB960BroadcastScript[5].nRegValue = (SER_5_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[6].nRegValue = (SENSOR_5_I2C_ALIAS<<1U);

        disableUB960BroadcastScript[9].nRegValue = (SER_6_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[10].nRegValue = (SENSOR_6_I2C_ALIAS<<1U);

        disableUB960BroadcastScript[13].nRegValue = (SER_7_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[14].nRegValue = (SENSOR_7_I2C_ALIAS<<1U);
    }
    else
    {
        printf("enableUB960Broadcast: Error: Invalid instance ID \n");
        return -1;
    }
    status = ub960_cfgScript(disableUB960BroadcastScript, ub960InstanceId);

    return status;
}

int8_t getUB960InstIdFromChId(uint32_t chId)
{
    int8_t ub960InstanceId = -1;
    /*Assumptions : 
      max 2 UB960 instances
      upto 4 cameras per instance of UB960
      */
    if(chId < 4U)
    {
        ub960InstanceId = BOARD_CSI_INST_0;
    }else if(chId < 8U)
    {
        ub960InstanceId = BOARD_CSI_INST_1;
    }else
    {
        printf("Error : Invalid chId 0x%x \n", chId);
        ub960InstanceId = -1;
    }

    return ub960InstanceId;
}

int32_t enableUB960Streaming(uint32_t chId)
{
    int32_t status = -1;
    int8_t ub960InstanceId = getUB960InstIdFromChId(chId);

    I2cParams ub960DesCSI2Enable[6u] = {
        {0x20, 0xF0, 0x1}, /*Forwarding off and using CSIport 0 */
        {0x4C, 0x00, 0x10},
        {0x72, 0xE4, 0x10}, // vc
        {0x33, 0x03, 0x10},
        {0x20, 0x00, 0x1}, /*Forwarding on and using CSIport 0 */
        {0xFFFF, 0x00, 0x0} //End of script
    };

    if(ub960InstanceId < 0)
    {
        printf("Error : Invalid ub960InstanceId \n");
    }
    else
    {
        uint8_t   ub960I2cInstId;
        uint8_t   ub960I2cAddr;
        uint16_t  regAddr = 0x4C;
        uint8_t   regVal = 0xAB;
        Board_fpdU960GetI2CAddr(&ub960I2cInstId, &ub960I2cAddr, ub960InstanceId);
        status = Board_i2c8BitRegRd(gISS_Sensor_I2cHandle, ub960I2cAddr, regAddr, &regVal, 1U, SENSOR_I2C_TIMEOUT);

        if(0xFF != regVal)
        {
            /*broadcast mode is not set. Need to page select for the channel ID*/
            switch(chId)
            {
                case 0:
                case 4:
                    ub960DesCSI2Enable[1].nRegValue = 0x01;
                    ub960DesCSI2Enable[4].nRegValue = 0xE0;
                    break;
                case 1:
                case 5:
                    ub960DesCSI2Enable[1].nRegValue = 0x12;
                    ub960DesCSI2Enable[4].nRegValue = 0xD0;
                    break;
                case 2:
                case 6:
                    ub960DesCSI2Enable[1].nRegValue = 0x24;
                    ub960DesCSI2Enable[4].nRegValue = 0xB0;
                    break;
                case 3:
                case 7:
                    ub960DesCSI2Enable[1].nRegValue = 0x38;
                    ub960DesCSI2Enable[4].nRegValue = 0x70;
                    break;
                case 0xFF:
                    ub960DesCSI2Enable[1].nRegValue = 0xF;
                    ub960DesCSI2Enable[3].nRegValue = 0xE4;
                    ub960DesCSI2Enable[4].nRegValue = 0x00;
                    break;
                default:
                    printf("Error : Invalid channel ID 0x%x \n", chId);
                    status = -1;
                    break;
            }
        }
        status |= ub960_cfgScript(ub960DesCSI2Enable, ub960InstanceId);
    }

#if 0  /*Define for Register dump*/
    appLogWaitMsecs(100);
    uint8_t regData, sensorI2cByteOrder;
    I2C_Handle sensorI2cHandle = NULL;
    int i;
    getIssSensorI2cInfo(&sensorI2cByteOrder, &sensorI2cHandle);
    appLogPrintf("960 Inst %d Ch %d \n",ub960InstanceId,chId);
    for(i=0;i<0x81;i++)
    {
        if(ub960InstanceId == 1)
            Board_i2c8BitRegRd(sensorI2cHandle, 0x3d, i, &regData, 1, SENSOR_I2C_TIMEOUT);
        else
            Board_i2c8BitRegRd(sensorI2cHandle, 0x30, i, &regData, 1, SENSOR_I2C_TIMEOUT);

        appLogWaitMsecs(10);
        appLogPrintf("Reg: 0x%X, Data: 0x%X \n",i,regData);
    }
#endif

    return status;
}

int32_t disableUB960Streaming(uint32_t chId)
{
    int32_t status = -1;
    int8_t ub960InstanceId = getUB960InstIdFromChId(chId);

    I2cParams ub960DesCSI2Disable[5u] = {
        {0x20, 0xF0, 0x1}, /*Forwarding off and using CSIport 0 */
        {0x4C, 0x00, 0x10},
        {0x33, 0x02, 0x10},
        {0xFFFF, 0x00, 0x0} //End of script
    };

    if(ub960InstanceId < 0)
    {
        printf("Error : Invalid ub960InstanceId \n");
    }
    else
    {
        switch(chId)
        {
            case 0:
            case 4:
                ub960DesCSI2Disable[1].nRegValue = 0x01;
                break;
            case 1:
            case 5:
                ub960DesCSI2Disable[1].nRegValue = 0x12;
                break;
            case 2:
            case 6:
                ub960DesCSI2Disable[1].nRegValue = 0x24;
                break;
            case 3:
            case 7:
                ub960DesCSI2Disable[1].nRegValue = 0x38;
                break;
            case 0xFF:
                ub960DesCSI2Disable[1].nRegValue = 0xF;
                break;
            default:
                printf("Error : Invalid channel ID 0x%x \n", chId);
                status = -1;
                break;
        }
        status = ub960_cfgScript(ub960DesCSI2Disable, ub960InstanceId);
    }

    return status;
}

int32_t UB960_SetSensorAlias(uint32_t chId, uint8_t sensor_phy_i2c_addr_7bit, uint8_t ser_alias_i2c_addr_7bit)
{
    int32_t status = -1;
    int8_t ub960InstanceId = getUB960InstIdFromChId(chId);
    uint8_t pageSelReg = 0x4C;
    uint8_t pageSelVal = 0xFF;
    uint8_t ub960I2cAddr;
    uint8_t  ub960I2cInstId;
    uint8_t  sensor_alias[ISS_SENSORS_MAX_SUPPORTED_SENSOR] =
    {SENSOR_0_I2C_ALIAS, SENSOR_1_I2C_ALIAS, SENSOR_2_I2C_ALIAS, SENSOR_3_I2C_ALIAS,
        SENSOR_4_I2C_ALIAS, SENSOR_5_I2C_ALIAS, SENSOR_6_I2C_ALIAS, SENSOR_7_I2C_ALIAS};

    I2cParams ub960setSensorAlias[5u] = {
        {0x4C, 0x00, 0x1},
        {0x65, 0x00, 0x1},
        {0x5E, 0x00, 0x1},
        {0x66, 0x00, 0x1},
        {0xFFFF, 0x00, 0x0} //End of script
    };

    if(ub960InstanceId < 0)
    {
        printf("Error : Invalid ub960InstanceId %d\n", ub960InstanceId);
    }
    else
    {
        Board_fpdU960GetI2CAddr(&ub960I2cInstId, &ub960I2cAddr, ub960InstanceId);
        UB960_ReadReg(ub960I2cInstId, ub960I2cAddr, pageSelReg, &pageSelVal);
        if(0x0F == pageSelVal)
        {
            /*Broadcast Enabled. No need to set page select register*/
            ub960setSensorAlias[0].nRegValue = 0x0F;
        }
        else
        {
            /*Broadcast not enabled. Must set page select as per channel ID*/
            switch(chId)
            {
                case 0:
                case 4:
                    ub960setSensorAlias[0].nRegValue = 0x01;
                    break;
                case 1:
                case 5:
                    ub960setSensorAlias[0].nRegValue = 0x12;
                    break;
                case 2:
                case 6:
                    ub960setSensorAlias[0].nRegValue = 0x24;
                    break;
                case 3:
                case 7:
                    ub960setSensorAlias[0].nRegValue = 0x38;
                    break;
                case 0xFF:
                    ub960setSensorAlias[0].nRegValue = 0x0F;
                    break;
                default:
                    printf("Error : Invalid channel ID 0x%x \n", chId);
                    status = -1;
                    break;
            }
        }

        ub960setSensorAlias[1].nRegValue = ser_alias_i2c_addr_7bit<<1U;
        ub960setSensorAlias[2].nRegValue = sensor_phy_i2c_addr_7bit << 1U;
        ub960setSensorAlias[3].nRegValue = sensor_alias[chId]<<1U;
        status = ub960_cfgScript(ub960setSensorAlias, ub960InstanceId);
    }

    return status;
}

int32_t ImageSensor_RemoteServiceHandler(char *service_name, uint32_t cmd,
        void *prm, uint32_t prm_size, uint32_t flags)
{
    int32_t status = -1;
    uint16_t cmdMemoryNeeded = 0;
    uint8_t * cmd_param = (uint8_t * )prm;
    uint8_t chId;
    uint32_t sensor_features_requested = 0;
    uint32_t channel_mask = 0;
    //uint32_t channel_mask_supported = 0;
    char * sensor_name = NULL;
    IssSensors_Handle * pSenHndl = NULL;
    IssSensor_CreateParams * pSenParams = (IssSensor_CreateParams * )NULL;

    switch(cmd)
    {
        case IM_SENSOR_CMD_ENUMERATE:
            issLogPrintf("ImageSensor_RemoteServiceHandler: IM_SENSOR_CMD_CREATE \n");
            cmdMemoryNeeded = ISS_SENSORS_MAX_NAME*ISS_SENSORS_MAX_SUPPORTED_SENSOR;
            if(prm_size < cmdMemoryNeeded)
            {
                printf("Error : Insufficient prm size %d, need at least %d \n", prm_size, cmdMemoryNeeded);
                return -1;
            }

            status = IssSensor_DeserializerInit();
            if(0 == status)
            {
                uint8_t count;
                for(count=0;count<ISS_SENSORS_MAX_SUPPORTED_SENSOR;count++)
                {
                    pSenHndl = gIssSensorTable[count];
                    if(NULL !=pSenHndl)
                    {
                        memcpy(cmd_param + (count*ISS_SENSORS_MAX_NAME), pSenHndl->createPrms->name, ISS_SENSORS_MAX_NAME);
                    }
                }
            }

            break;
        case IM_SENSOR_CMD_QUERY:
            sensor_name = (char*)(cmd_param);
            issLogPrintf("ImageSensor_RemoteServiceHandler: IM_SENSOR_CMD_QUERY Query for %s \n", sensor_name);

            /*Copy sensor properties at prm, after sensor name*/
            pSenParams = (IssSensor_CreateParams * )(cmd_param+ISS_SENSORS_MAX_NAME);
            status = IssSensor_GetSensorInfo(sensor_name, pSenParams);
            break;
        case IM_SENSOR_CMD_PWRON:
            sensor_name = (char*)(cmd_param);
            memcpy(&channel_mask, (cmd_param+ISS_SENSORS_MAX_NAME), sizeof(uint32_t));
            issLogPrintf("ImageSensor_RemoteServiceHandler: IM_SENSOR_CMD_PWRON chMask 0x%x\n", channel_mask);
            pSenHndl = getSensorHandleFromName(sensor_name);
            if(NULL == pSenHndl)
            {
                status = -1;
                printf("ERROR : NULL handle returned for sensor %s \n", sensor_name);
            }
            else
            {
                //TODO: we have a handle let check the requested cameras if they are streamin
                status = IssSensor_PowerOn((void*)pSenHndl, channel_mask);
                status |= enableUB960Streaming(0);
                if(channel_mask > 0x0F)
                {
                    status |= enableUB960Streaming(4);
                }

            }
            break;
        case IM_SENSOR_CMD_CONFIG:
            issLogPrintf("ImageSensor_RemoteServiceHandler: IM_SENSOR_CMD_CONFIG \n");
            sensor_name = (char*)(cmd_param);
            memcpy(&sensor_features_requested, (cmd_param+ISS_SENSORS_MAX_NAME), sizeof(uint32_t));
            memcpy(&channel_mask, (cmd_param+ISS_SENSORS_MAX_NAME+sizeof(uint32_t)), sizeof(uint32_t));
            issLogPrintf("Application requested features = 0x%x chMask = 0x%x \n ", sensor_features_requested, channel_mask);
            pSenHndl = getSensorHandleFromName(sensor_name);
            if(NULL == pSenHndl)
            {
                status = -1;
                printf("ERROR : NULL handle returned for sensor %s \n", sensor_name);
            }
            else
            {
                status = 0;
                //channel_mask_supported = (1<<pSenHndl->createPrms->num_channels) - 1;
                //channel_mask &= channel_mask_supported;
                
                if(1U == pSenHndl->sensorIntfPrms->sensorBroadcast)
                {
                    if((channel_mask & 0x0F) == 0x0F)
                    {
                        issLogPrintf("Configuring all cameras on UB960_0 in broadcast mode \n");
                        status |= enableUB960Broadcast(0);
                        status |= IssSensor_Config((void*)pSenHndl, 0, sensor_features_requested);
                        channel_mask &= 0xF0;
                    }

                    if((channel_mask & 0xF0) == 0xF0)
                    {
                        issLogPrintf("Configuring all cameras on UB960_1 in broadcast mode \n");
                        status |= enableUB960Broadcast(1);
                        status |= IssSensor_Config((void*)pSenHndl, 4, sensor_features_requested);
                        channel_mask &= 0x0F;
                    }
                }else
                {
                    issLogPrintf("Disabling Broadcast on all channels \n");
                    status |= disableUB960Broadcast(0);
                    status |= disableUB960Broadcast(1);
                }

                if(0 != channel_mask)
                {
                    chId = 0;
                    //while( (channel_mask > 0) && (chId < ISS_SENSORS_MAX_CHANNEL) )                    
                    //TODO: check if camera is enabled
                    while( chId < ISS_SENSORS_MAX_CHANNEL )
                    {
                        if((channel_mask & 0x1) == 0x1)
                        {
                            issLogPrintf("Configuring camera # %d \n", chId);
                            status |= IssSensor_Config((void*)pSenHndl, chId, sensor_features_requested);
                        }
                        chId++;
                        channel_mask = channel_mask >> 1;
                    }
                }
            }
            issLogPrintf("IM_SENSOR_CMD_CONFIG returning status = %d \n", status);

            break;
        case IM_SENSOR_CMD_STREAM_ON:
            issLogPrintf("ImageSensor_RemoteServiceHandler: IM_SENSOR_CMD_STREAM_ON \n");
            sensor_name = (char*)(cmd_param);
            memcpy(&channel_mask, (cmd_param+ISS_SENSORS_MAX_NAME), sizeof(uint32_t));
            pSenHndl = getSensorHandleFromName(sensor_name);
            if(NULL == pSenHndl)
            {
                status = -1;
                printf("ERROR : NULL handle returned for sensor %s \n", sensor_name);
            }
            else
            {
                status = 0;

                //channel_mask_supported = (1<<pSenHndl->createPrms->num_channels) - 1;
                //channel_mask &= channel_mask_supported;
                if(1U == pSenHndl->sensorIntfPrms->sensorBroadcast)
                {
                    if((channel_mask & 0x0F) == 0x0F)
                    {
                        issLogPrintf("Starting all cameras on UB960_0 in broadcast mode \n");
                        status |= enableUB960Broadcast(0);
                        status |= IssSensor_Start((void*)pSenHndl, 0);
                        channel_mask &= 0xF0;
                    }

                    if((channel_mask & 0xF0) == 0xF0)
                    {
                        issLogPrintf("Starting all cameras on UB960_1 in broadcast mode \n");
                        status |= enableUB960Broadcast(1);
                        status |= IssSensor_Start((void*)pSenHndl, 4);
                        channel_mask &= 0x0F;
                    }
                }

                /*Disable broadcast after enabling streaming 
                  so that every camera can have independent 2A control */
                status |= disableUB960Broadcast(0);
                status |= disableUB960Broadcast(1);
                issLogPrintf("Disabled Broadcast\n");
                chId = 0;

                //while( (channel_mask > 0) && (chId < ISS_SENSORS_MAX_CHANNEL) )
                while( chId < ISS_SENSORS_MAX_CHANNEL )
                {
                    if(channel_mask & 0x1)
                    {
                        issLogPrintf("Starting chId %d \n", chId);
                        status = IssSensor_Start((void*)pSenHndl, chId);
                        if(status < 0)
                        {
                            issLogPrintf("Error : Failed to start sensor at channel Id %d \n", chId);
                        }
                        if( NULL ==g_pSenHndl[chId])
                        {
                            issLogPrintf("Error : sensor handle at channel Id %d = NULL \n", chId);
                        }
                    }

                    chId++;
                    channel_mask = channel_mask >> 1U;
                }
            }

            break;
        case IM_SENSOR_CMD_STREAM_OFF:
            issLogPrintf("ImageSensor_RemoteServiceHandler: IM_SENSOR_CMD_STREAM_OFF \n");

            sensor_name = (char*)(cmd_param);
            memcpy(&channel_mask, (cmd_param+ISS_SENSORS_MAX_NAME), sizeof(uint32_t));
            issLogPrintf("Stream OFF chMask: 0x%x\n", channel_mask);
            pSenHndl = getSensorHandleFromName(sensor_name);
            if(NULL == pSenHndl)
            {
                status = -1;
                printf("ERROR : NULL handle returned for sensor %s \n", sensor_name);
            }
            else
            {
                status = 0;
                chId = 0;

                // while( (channel_mask > 0) && (chId < ISS_SENSORS_MAX_CHANNEL) )
                while( chId < ISS_SENSORS_MAX_CHANNEL )
                {
                    if(channel_mask & 0x1)
                    {
                        issLogPrintf("Stopping chId %d \n", chId);
                        status = IssSensor_Stop((void*)pSenHndl, chId);
                        if(status < 0)
                        {
                            printf("Warning : Failed to stop sensor at channel Id %d \n", chId);
                        }
                        if( NULL !=g_pSenHndl[chId])
                        {
                            printf("Warning : sensor handle at channel Id %d is not NULL \n", chId);
                        }
                    }

                    chId++;
                    channel_mask = channel_mask >> 1U;
                }
            }
            break;
        case IM_SENSOR_CMD_ACTIVATEPAIR:
            Iss_activateCameraPair(*(int32_t *)cmd_param);
            status = 0;
            break;
        case IM_SENSOR_CMD_ACITIVATE_MASK:
            Iss_activateCameraMask(*(uint16_t *)cmd_param);
            status = 0;
            break;
        case IM_SENSOR_CMD_PWROFF:
            issLogPrintf("ImageSensor_RemoteServiceHandler: IM_SENSOR_CMD_PWROFF \n");
            sensor_name = (char*)(cmd_param);
            memcpy(&channel_mask, (cmd_param+ISS_SENSORS_MAX_NAME), sizeof(uint32_t));
            issLogPrintf("Power OFF chMask: 0x%x\n", channel_mask);
            pSenHndl = getSensorHandleFromName(sensor_name);
            if(NULL == pSenHndl)
            {
                status = -1;
                printf("ERROR : IM_SENSOR_CMD_PWROFF NULL handle returned for sensor %s \n", sensor_name);
            }
            else
            {
                status = 0;
                chId = 0;

                // while( (channel_mask > 0) && (chId < ISS_SENSORS_MAX_CHANNEL) )
                while( chId < ISS_SENSORS_MAX_CHANNEL )
                {
                    if(channel_mask & 0x1)
                    {
                        status = IssSensor_PowerOff((void*)pSenHndl, chId);
                        if(status < 0)
                        {
                            printf("Warning : Failed to power off sensor at channel Id %d \n", chId);
                        }
                        if( NULL !=g_pSenHndl[chId])
                        {
                            printf("Warning : sensor handle at channel Id %d is not NULL \n", chId);
                        }
                    }

                    chId++;
                    channel_mask = channel_mask >> 1U;
                }

                status |= disableUB960Streaming(0);
                if(channel_mask > 0x0F)
                {
                    status |= disableUB960Streaming(4);
                }

            }
            break;
        case IM_SENSOR_CMD_CTL:
            status = IssSensor_Control(NULL, 0, (void*)cmd_param, NULL);
            break;
        case IM_SENSOR_CMD_DETECT:
            {
                uint8_t chId = 0;
                int32_t probeStatus;
                uint8_t serializers_detected = 0;
                memcpy(&channel_mask, cmd_param, sizeof(uint32_t));

                probeStatus = IssSensor_detect_serializer(BOARD_CSI_INST_0, &serializers_detected);
                if(probeStatus != 0)
                {
                    issLogPrintf("IssSensor_detect_serializer returned 0x%x \n", probeStatus);
                    return -1;
                }

                probeStatus = IssSensor_detect_serializer(BOARD_CSI_INST_1, &serializers_detected);
                if(probeStatus != 0)
                {
                    issLogPrintf("IssSensor_detect_serializer returned 0x%x \n", probeStatus);
                    return -1;
                }

                while(channel_mask > 0)
                {
                    if(channel_mask & 0x1)
                    {
                        int32_t detectedSensor;
                        int32_t tmp = (1<<chId);
                        if(tmp&serializers_detected)
                        {
                            detectedSensor = IssSensor_detect_sensor(chId);
                            if(detectedSensor < 0)
                            {
                                issLogPrintf("IM_SENSOR_CMD_DETECT found serializer but no sensor at chId %d \n", chId);
                                cmd_param[chId] = 0xFF;
                            }
                            else
                            {
                                issLogPrintf("IM_SENSOR_CMD_DETECT cmd_param[%d] = %d \n", chId, cmd_param[chId]);
                                cmd_param[chId] = (uint8_t)detectedSensor;
                            }
                        }else
                        {
                            issLogPrintf("IM_SENSOR_CMD_DETECT No serializer found at port %d \n", chId);
                            cmd_param[chId] = 0xFF;
                        }

                    }
                    else
                    {
                        cmd_param[chId] = 0xFF;
                    }
                    channel_mask = channel_mask >> 1;
                    chId++;
                }
                status = 0;
            }
            break;
        case IM_SENSOR_CMD_D3_USER_I2C:
            D3Utils_I2CCommandExecute((D3UtilsI2C_Command *)cmd_param);
            break;
        case IM_SENSOR_CMD_TEMPPAIR:
            {
                uintptr_t addr = (uintptr_t)(*(volatile uint32_t*)cmd_param);
                // printf("%s IM_SENSOR Temp PHYS_PTR: 0x%08x\n", appIpcGetCpuName(appIpcGetSelfCpuId()), (uint32_t)addr);
                uint32_t ret_val = 0x0;
                appMemCacheInv((void*)addr, sizeof(CameraTempReadings));
                // issLogPrintf("IM_SENSOR_CMD_TEMPPAIR CALLED: MASK: 0x%02X TEMPs: 0x%08X 0x%08X\n", (uint8_t)((CameraTempReadings *)addr)->mask, ((CameraTempReadings *)addr)->cam_temp_lower, ((CameraTempReadings *)addr)->cam_temp_upper);
                                
                ret_val = sensor_updateCameraPairTemp((CameraTempReadings *)addr);
                if(ret_val != 0){
                    issLogPrintf("ImageSensor_RemoteServiceHandler : IM_SENSOR_CMD_TEMPPAIR error retrieving temperature\n");
                    status = -1;
                }else{
                    status = 0;
                }

                appMemCacheWbInv((void*)addr, sizeof(CameraTempReadings));
                
                //if(xCount % 30 == 0)
                //{
                //    printf("%s IM_SENSOR Temp PHYS_PTR: 0x%08x\n", appIpcGetCpuName(appIpcGetSelfCpuId()), (uint32_t)addr);
                //    printf("IM_SENSOR Temp PHYS_VAL: 0x%08x\n", *(volatile uint32_t*)addr);
                //}
                // xCount++;
                #ifdef R5F
                asm("  dsb");
                #endif
            }
            break;
        case IM_SENSOR_CMD_BOARD_TEMP:
            {
                uintptr_t addr = (uintptr_t)(*(volatile uint32_t*)cmd_param);
                // uint32_t pair = 0x00;
                uint32_t ret_val = 0x0;
                appMemCacheInv((void*)addr, 256);
                // pair = *(volatile uint32_t*)addr;

                ret_val = D3Utils_I2CReadPersTemp();
                if(ret_val == 0){
                    issLogPrintf("ImageSensor_RemoteServiceHandler : IM_SENSOR_CMD_BOARD_TEMP error retrieving temperature\n");
                    status = -1;
                }else{
                    status = 0;
                }
                *(volatile uint32_t*)addr = ret_val;
            
                appMemCacheWbInv((void*)addr, 256);
                
                #ifdef R5F
                asm("  dsb");
                #endif
            }
            break;
        default:
            issLogPrintf("ImageSensor_RemoteServiceHandler : Unsupported command : %d\n", cmd);
            status = -1;
    }

    return status;
}

void Iss_activateCameraMask(uint16_t ch_mask) 
{
    
    uint16_t right_bank = ch_mask & 0xF0;  // cameras 4-7
    uint16_t left_bank = (ch_mask & 0x0F) << 4;  // cameras 0-3
    uint16_t right_fwd = (right_bank & 0x0F) | (~right_bank & 0xF0);
    uint16_t left_fwd = (left_bank & 0x0F) | (~left_bank & 0xF0);

    issLogPrintf("Iss_activateCameraMask\nch_mask: 0x%02x\n", ch_mask);
    issLogPrintf("right_bank: 0x%02x\n", right_bank);
    issLogPrintf("left_bank: 0x%02x\n", left_bank);
    issLogPrintf("right_fwd: 0x%02x\n", right_fwd);
    issLogPrintf("left_fwd: 0x%02x\n", left_fwd);

    I2cParams ub960activateCfg[] = {
        {0x20, 0xE0, 0x10}, // Enable forwarding for port #0 and #4, disabling for 1,2,3,4,5,6
        {0x4C, 0x00, 0x10},
        {0x72, 0x00, 0x10}, /*VC Map - All to 0 */
        {0x4C, 0x00, 0x10},
        {0x72, 0x55, 0x10}, /*VC Map - All to 1 */
        {0x4C, 0x00, 0x10},
        {0x72, 0xAA, 0x10}, /*VC Map - All to 2 */
        {0xFFFF, 0x0, 0x0}, // end
    };

    I2cParams ub960deactivateCfg[] = {
        {0x20, 0xF0, 0x10}, // Disable all ports
        {0xFFFF, 0x0, 0x0}, // end
    };

    ub960_cfgScript(ub960deactivateCfg, 0);
    ub960_cfgScript(ub960deactivateCfg, 1);

    // configure UB960 0
    ub960activateCfg[0].nRegValue = left_fwd; 
    if(left_bank != 0xF0){
        // setup first camera of bank
        ub960activateCfg[1].nRegValue = getCameraPortConfig(left_bank >> 4, 1);
        // setup second camera of bank
        ub960activateCfg[3].nRegValue = getCameraPortConfig(left_bank >> 4, 2);
        // setup third camera of bank
        ub960activateCfg[5].nRegValue = getCameraPortConfig(left_bank >> 4, 3);
    }
    ub960_cfgScript(ub960activateCfg, 0);

    // configure UB960 1
    ub960activateCfg[0].nRegValue = right_fwd; 
    if(right_bank != 0xF0){
        // setup first camera of bank
        ub960activateCfg[1].nRegValue = getCameraPortConfig(right_bank >> 4, 1);
        // setup second camera of bank
        ub960activateCfg[3].nRegValue = getCameraPortConfig(right_bank >> 4, 2);
        // setup third camera of bank
        ub960activateCfg[5].nRegValue = getCameraPortConfig(right_bank >> 4, 3);
    }
    ub960_cfgScript(ub960activateCfg, 1);

    return;
}

void Iss_activateCameraPair(int32_t pair) {

    uint32_t config_mask = (uint32_t)pair;
    uint16_t upper_bank = (uint16_t)config_mask & 0xF0;  // cameras 4-7
    uint16_t lower_bank = ((uint16_t)config_mask & 0x0F) << 4;  // cameras 0-3
    uint16_t upper_fwd = (upper_bank & 0x0F) | (~upper_bank & 0xF0);
    uint16_t lower_fwd = (lower_bank & 0x0F) | (~lower_bank & 0xF0);

    I2cParams ub960activateCfg[] = {
        {0x20, 0xE0, 0x10}, // Enable forwarding for port #0 and #4, disabling for 1,2,3,4,5,6
        {0x4C, 0x00, 0x10},
        {0x72, 0x00, 0x10}, /*VC Map - All to 0 */
        {0x4C, 0x00, 0x10},
        {0x72, 0x55, 0x10}, /*VC Map - All to 1 */
        {0xFFFF, 0x0, 0x0}, // end
    };

    I2cParams ub960deactivateCfg[] = {
        {0x20, 0xF0, 0x10}, // Disable all ports
        {0xFFFF, 0x0, 0x0}, // end
    };

    ub960_cfgScript(ub960deactivateCfg, 0);
    ub960_cfgScript(ub960deactivateCfg, 1);

#if 0
    issLogPrintf("config_mask: 0x%02x\n", config_mask);
    issLogPrintf("upper_bank: 0x%02x\n", upper_bank);
    issLogPrintf("lower_bank: 0x%02x\n", lower_bank);
    issLogPrintf("upper_fwd: 0x%02x\n", upper_fwd);
    issLogPrintf("lower_fwd: 0x%02x\n", lower_fwd);
#endif

    // configure UB960 0
    ub960activateCfg[0].nRegValue = lower_fwd; 
    if(lower_bank != 0xF0){
        // setup first camera of bank
        ub960activateCfg[1].nRegValue = getCameraPortConfig(lower_bank >> 4, 1);
        // setup second camera of bank
        ub960activateCfg[3].nRegValue = getCameraPortConfig(lower_bank >> 4, 2);
    }
    ub960_cfgScript(ub960activateCfg, 0);

    // configure UB960 1
    ub960activateCfg[0].nRegValue = upper_fwd; 
    if(upper_bank != 0xF0){
        // setup first camera of bank
        ub960activateCfg[1].nRegValue = getCameraPortConfig(upper_bank >> 4, 1);
        // setup second camera of bank
        ub960activateCfg[3].nRegValue = getCameraPortConfig(upper_bank >> 4, 2);
    }
    ub960_cfgScript(ub960activateCfg, 1);

    return;
}

uint32_t D3Utils_I2CReadPersTemp(){
    I2C_Handle sensorI2cHandle = NULL;
    uint8_t sensorI2cByteOrder = 255U;
    
    getIssSensorI2cInfo(&sensorI2cByteOrder, &sensorI2cHandle);

    return Board_i2c1ReadTemp(sensorI2cHandle,0x49) * (float) 1000;  // 0x49 is the location of the TMP102 on the personailty board
}

float Board_i2c1ReadTemp(void *handle, uint32_t slaveAddr)
{
    uint8_t txBuffer[1];
    uint8_t rxBuffer[2];
    int16_t temperature;
    I2C_Transaction i2cTransaction;
    I2C_Handle i2cHandle = (I2C_Handle)handle;
    // Configure TMP102 sensor for temperature readings
    txBuffer[0] = 0x00;  // Select temperature register
    I2C_transactionInit(&i2cTransaction);
    i2cTransaction.slaveAddress = slaveAddr;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;
    i2cTransaction.timeout = I2C_WAIT_FOREVER;
    if (I2C_transfer(i2cHandle, &i2cTransaction) == false)
    {
        printf("Error writing to TMP102 sensor\n");
        return -1;
    }
    // Read temperature data from the sensor
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 2;
    // Read temperature data from the sensor
    if (I2C_transfer(i2cHandle, &i2cTransaction) == false)
    {
        printf("Error reading from TMP102 sensor\n");
        return -1;
    }
    // Calculate temperature from the received data
    temperature = (rxBuffer[0] << 8) | rxBuffer[1];
    temperature >>= 4;  // Right-align the 12-bit temperature value
    if (temperature & 0x800)
    {
        // Negative temperature, perform sign extension
        temperature |= 0xF000;
    }
    // Convert temperature to degrees Celsius
    float celsius = temperature * 0.0625;
    // Print the temperature value
    printf("Temperature: %f degrees Celsius\n", celsius);
    return celsius;
}


void D3Utils_I2CCommandExecute(D3UtilsI2C_Command *cmd)
{

    I2C_Handle sensorI2cHandle = NULL;
    uint8_t sensorI2cByteOrder = 255U;

    uint32_t devAddr = cmd->devAddr;
    uint32_t regAddr = cmd->regAddr;
    uint32_t regData = cmd->regData;
    
    getIssSensorI2cInfo(&sensorI2cByteOrder, &sensorI2cHandle);

    switch(cmd->type) {
        case TRANSACTION_TYPE_8A8D:
            switch(cmd->direction) {
                case D3_I2C_WRITE:
                    Board_i2c8BitRegWr(sensorI2cHandle, devAddr, regAddr,(uint8_t *) &regData, 1, I2C_WAIT_FOREVER);
                    break;
                case D3_I2C_READ:
                    Board_i2c8BitRegRd(sensorI2cHandle, devAddr, regAddr,(uint8_t *) &cmd->regData, 1, I2C_WAIT_FOREVER);
                    break;
                default:
                    printf("Invalid read direction.\n");
                    break;
            }
            break;
        case TRANSACTION_TYPE_8A16D:
            switch(cmd->direction) {
                case D3_I2C_WRITE:
                    Board_i2c8BitRegWr(sensorI2cHandle, devAddr, regAddr,(uint8_t *) &regData, 2, I2C_WAIT_FOREVER); 
                    break;
                case D3_I2C_READ:
                    Board_i2c8BitRegRd(sensorI2cHandle, devAddr, regAddr,(uint8_t *) &cmd->regData, 2, I2C_WAIT_FOREVER);
                    break;
                default:
                    printf("Invalid read direction.\n");
                    break;
            }
            break;
        case TRANSACTION_TYPE_16A8D:
            switch(cmd->direction) {
                case D3_I2C_WRITE:
                    Board_i2c16BitRegWr(sensorI2cHandle, devAddr, regAddr,(uint8_t *) &regData, 1,sensorI2cByteOrder, I2C_WAIT_FOREVER);
                    break;
                case D3_I2C_READ:
                    Board_i2c16BitRegRd(sensorI2cHandle, devAddr, regAddr,(uint8_t *) &cmd->regData, 1,sensorI2cByteOrder, I2C_WAIT_FOREVER);
                    break;
                default:
                    printf("Invalid read direction.\n");
                    break;
            }
            break;
        case TRANSACTION_TYPE_16A16D:
            switch(cmd->direction) {
                case D3_I2C_WRITE:
                    Board_i2c16BitRegWr(sensorI2cHandle, devAddr, regAddr,(uint8_t *) &regData, 2,sensorI2cByteOrder, I2C_WAIT_FOREVER);
                    break;
                case D3_I2C_READ:
                    Board_i2c16BitRegRd(sensorI2cHandle, devAddr, regAddr,(uint8_t *) &cmd->regData, 2,sensorI2cByteOrder, I2C_WAIT_FOREVER);
                    break;
                default:
                    printf("Invalid read direction.\n");
                    break;
            }
            break;
        default:
            printf("Unknown transaction type.\n");
            break;
    }
    printf("\nD3_I2C_UTILS: Dev: 0x%02X, Addr: 0x%04X, Data: 0x%04X\n", devAddr, regAddr, cmd->regData);
    return;
}

/*enableMask - 8 bit mask that corresponds to 8 capture channels - bit0 -> channel 0,
 *  setting a bit to '1' enables power for the corresponding channel*/
int32_t ImageSensor_RVPFPDSetPower(uint8_t enableMask)
{
    int32_t status = 0;
    I2C_Handle sensorI2cHandle = NULL;
    uint8_t sensorI2cByteOrder = 255U;
    uint8_t val;

    getIssSensorI2cInfo(&sensorI2cByteOrder, &sensorI2cHandle);

    /*Enable General FPD Link Power*/
    if(enableMask !=0)
    {
        val = 0x10;
        status |= Board_i2c8BitRegWr(sensorI2cHandle,PCAL9539A_GPIO_EXP_MISC_IO_I2C_ADDR,
                PCAL9539A_GPIO_EXP_VAL_REG_H,
                &val,
                1,
                I2C_WAIT_FOREVER);
        val = 0xEF;
        status |= Board_i2c8BitRegWr(sensorI2cHandle,PCAL9539A_GPIO_EXP_MISC_IO_I2C_ADDR,
                PCAL9539A_GPIO_EXP_DIR_REG_H,
                &val,
                1,
                I2C_WAIT_FOREVER);

    }

    /*Enable PDB 960_0*/
   // if((enableMask & 0x0F) != 0)
    {
        val = 0x80;
        status |= Board_i2c8BitRegWr(sensorI2cHandle,PCAL9539A_GPIO_EXP_UB960_IO_I2C_ADDR,
                PCAL9539A_GPIO_EXP_VAL_REG_L,
                &val,
                1,
                I2C_WAIT_FOREVER);
        val = 0x7F;
        status |= Board_i2c8BitRegWr(sensorI2cHandle,PCAL9539A_GPIO_EXP_UB960_IO_I2C_ADDR,
                PCAL9539A_GPIO_EXP_DIR_REG_L,
                &val,
                1,
                I2C_WAIT_FOREVER);
    }

    /*Enable PDB 960_1*/
   // if((enableMask & 0xF0) != 0)
    {
        val = 0x80;
        status |=  Board_i2c8BitRegWr(sensorI2cHandle,PCAL9539A_GPIO_EXP_UB960_IO_I2C_ADDR,
                PCAL9539A_GPIO_EXP_VAL_REG_H,
                &val,
                1,
                I2C_WAIT_FOREVER);
        val = 0x7F;
        status |= Board_i2c8BitRegWr(sensorI2cHandle,PCAL9539A_GPIO_EXP_UB960_IO_I2C_ADDR,
                PCAL9539A_GPIO_EXP_DIR_REG_H,
                &val,
                1,
                I2C_WAIT_FOREVER);
    }


    /*Enable Channel Power For Lower Bank*/
    val = enableMask & 0x0F;
    status |=  Board_i2c8BitRegWr(sensorI2cHandle,PCAL9539A_GPIO_EXP_FPD_POW_I2C_ADDR,
            PCAL9539A_GPIO_EXP_VAL_REG_L,
            &val,
            1,
            I2C_WAIT_FOREVER);
    val = ~(enableMask & 0x0F);
    status |=  Board_i2c8BitRegWr(sensorI2cHandle,PCAL9539A_GPIO_EXP_FPD_POW_I2C_ADDR,
            PCAL9539A_GPIO_EXP_DIR_REG_L,
            &val,
            1,
            I2C_WAIT_FOREVER);

    /*Enable Channel Power For Upper Bank*/
    val = (enableMask & 0xF0) >> 4;
    status |= Board_i2c8BitRegWr(sensorI2cHandle,PCAL9539A_GPIO_EXP_FPD_POW_I2C_ADDR,
            PCAL9539A_GPIO_EXP_VAL_REG_H,
            &val,
            1,
            I2C_WAIT_FOREVER);
    val = ~((enableMask & 0xF0) >> 4);
    status |= Board_i2c8BitRegWr(sensorI2cHandle,PCAL9539A_GPIO_EXP_FPD_POW_I2C_ADDR,
            PCAL9539A_GPIO_EXP_DIR_REG_H,
            &val,
            1,
            I2C_WAIT_FOREVER);


    if(status != 0)
    {
        printf("ImageSensor_FPDSetPower: Failed to Set FPD Power \n");

    }

    return status;

}


int32_t IssSensor_DeInit()
{
    int32_t status = -1;
    
    status = appRemoteServiceUnRegister(IMAGE_SENSOR_REMOTE_SERVICE_NAME);
    if(status!=0)
    {
        printf(" REMOTE_SERVICE_SENSOR: ERROR: Unable to unregister remote service sensor handler\n");
    }
    
    if(gISS_Sensor_I2cHandle != NULL)
    {
        issLogPrintf("IssSensor_PowerOff : closing i2c handle \n ");  
        I2C_close(gISS_Sensor_I2cHandle);
        gISS_Sensor_I2cHandle = NULL;
    }
    return status;
}
