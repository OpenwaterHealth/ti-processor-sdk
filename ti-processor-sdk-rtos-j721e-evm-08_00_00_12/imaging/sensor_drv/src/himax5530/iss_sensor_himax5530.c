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
#include "iss_sensor_himax5530.h"
#include "himax5530_serdes_config.h"
// #include "himax5530_config.h"
#include <math.h>

//Gain for Sensors 0-3
#define HIGH_GAIN 16

//Gain for Sensors 4-7
#define LOW_GAIN 1

// TODO Register values have been redacted due to NDA waiting on legal
#ifdef MCLK_25_MHZ
uint16_t regValData[1][3] = {
    {0x0003, 0x00, 0}, // 
};
#elif MCLK_19_2_MHZ || MCLK_15_MHZ
uint16_t regValData[1][3] = {
    {0x0003, 0x00, 0}, //
};
#elif MCLK_19_2_MHZ_60HZ
uint16_t regValData[1][3] = {     //19MHz60hz0920x2720
    {0x0003, 0x00, 0}, // 
};
#elif MCLK_19_2_MHZ_80HZ
uint16_t regValData[1][3] = { // 19MHz80hz0654x2720
    {0x0003, 0x00, 0}, // 
 };
#endif

static IssSensor_CreateParams  himax5530CreatePrms = {
    SENSOR_HIMAX5530_UB953_D3,       /*sensor name*/
    0x6,                             /*i2cInstId*/
    {
        SENSOR_0_I2C_ALIAS, SENSOR_1_I2C_ALIAS, SENSOR_2_I2C_ALIAS, SENSOR_3_I2C_ALIAS,
        SENSOR_4_I2C_ALIAS, SENSOR_5_I2C_ALIAS, SENSOR_6_I2C_ALIAS, SENSOR_7_I2C_ALIAS
    },/*i2cAddrSensor*/
    {
        SER_0_I2C_ALIAS, SER_1_I2C_ALIAS, SER_2_I2C_ALIAS, SER_3_I2C_ALIAS,
        SER_4_I2C_ALIAS, SER_5_I2C_ALIAS, SER_6_I2C_ALIAS, SER_7_I2C_ALIAS
    },/*i2cAddrSer*/
    /*IssSensor_Info*/
    {
        {
            HIMAX5530_OUT_WIDTH,               /*width*/
            HIMAX5530_OUT_HEIGHT,            /*height*/
            1,                              /*num_exposures*/
            vx_false_e,                     /*line_interleaved*/
            {
                {TIVX_RAW_IMAGE_16_BIT, 9},    /*dataFormat and MSB [0]*/
            },
            0,                              /*meta_height_before*/
            0,      /*meta_height_after*/
        },
        ISS_SENSOR_HIMAX5530_FEATURES,     /*features*/
        ALGORITHMS_ISS_AEWB_MODE_NONE,  /*aewbMode*/
        30,                             /*fps*/
        4,                              /*numDataLanes*/
        {1, 2, 3, 4},                   /*dataLanesMap*/
        {0, 0, 0, 0},                   /*dataLanesPolarity*/
        CSIRX_LANE_BAND_SPEED_720_TO_800_MBPS, /*csi_laneBandSpeed*/
    },
    8,                                  /*numChan*/
    5530,                                /*dccId*/
};


static IssSensorFxns           himax5530SensorFxns = {
    HIMAX5530_Probe,
    HIMAX5530_Config,
    HIMAX5530_StreamOn,
    HIMAX5530_StreamOff,
    HIMAX5530_PowerOn,
    HIMAX5530_PowerOff,
    HIMAX5530_GetExpParams,
    HIMAX5530_SetAeParams,
    HIMAX5530_GetDccParams,
    HIMAX5530_InitAewbConfig,
    HIMAX5530_GetIspConfig,
    HIMAX5530_ReadWriteReg,
    HIMAX5530_GetExpPrgFxn,
    HIMAX5530_deinit,
    NULL,
    NULL
};

static IssSensorIntfParams     himax5530SensorIntfPrms = {
    0,             /*sensorBroadcast*/
    0,             /*enableFsin*/
    0,             /*numCamerasStreaming*/
};

IssSensorConfig     himax5530SensorRegConfig = {
    NULL,     /*desCfgPreScript*/
    ub953SerCfg_HIMAX5530,     /*serCfgPreScript*/
    NULL,        /*sensorCfgPreScript*/
    NULL,        /*desCfgPostScript*/
    NULL,                    /*serCfgPostScript*/
    NULL,                    /*sensorCfgPostScript*/
};

IssSensors_Handle himax5530SensorHandle = {
    1,                                 /*isUsed*/
    &himax5530CreatePrms,                /*CreatePrms*/
    &himax5530SensorFxns,                /*SensorFxns*/
    &himax5530SensorIntfPrms,            /*SensorIntfPrms*/
};

extern IssSensors_Handle * gIssSensorTable[ISS_SENSORS_MAX_SUPPORTED_SENSOR];
//static uint16_t sp1hGainRegValueOld[ISS_SENSORS_MAX_CHANNEL];

int32_t IssSensor_HIMAX5530_Init()
{
    int32_t status;
    int32_t chId;
    status = IssSensor_Register(&himax5530SensorHandle);
    if(0 != status)
    {
        printf("IssSensor_HIMAX5530_Init failed \n");
    }
    for(chId=0;chId<ISS_SENSORS_MAX_CHANNEL;chId++)
        {
// sp1hGainRegValueOld[chId] = 0;
        }

    return status;

}

/*******************************************************************************
*  Local Functions Definition
*******************************************************************************
*/

// This is a duplicate of the IMX390 probe.
// This probably needs to be updated, but I don't know what the Himax ID register is.
static int32_t HIMAX5530_Probe(uint32_t chId, void *pSensorHdl)
{
    int32_t status = -1;
    uint32_t i2cInstId;
    uint8_t sensorI2cAddr;
    uint16_t chipIdRegAddr = HIMAX5530_CHIP_ID_REG_ADDR;
    uint8_t chipIdRegValueRead = HIMAX5530_CHIP_ID_REG_VAL; // HIMAX probe?
    IssSensors_Handle * pSenHandle = (IssSensors_Handle*)pSensorHdl;
    IssSensor_CreateParams * pCreatePrms;
    I2cParams    * serCfg = NULL;
    uint8_t count=0;
    uint8_t max_retries = 1;

    assert(NULL != pSenHandle);
    pCreatePrms = pSenHandle->createPrms;
    assert(NULL != pCreatePrms);

    i2cInstId = pCreatePrms->i2cInstId;
    sensorI2cAddr = pCreatePrms->i2cAddrSensor[chId];

    status = UB960_SetSensorAlias(chId, HIMAX5530_I2C_ADDR, pCreatePrms->i2cAddrSer[chId]);
    if(0 != status)
    {
        printf("HIMAX5530_Probe Error: UB960_SetSensorAlias for chId %d returned %d \n", chId, status);
        return status;
    }

    serCfg = himax5530SensorRegConfig.serCfgPreScript;
    /*The code assumes that I2C instance is the same for sensor and serializer*/
    if(NULL != serCfg)
    {
        status = ub953_cfgScript(i2cInstId, pCreatePrms->i2cAddrSer[chId], serCfg);
    }

    /*Read chip ID to detect if the sensor can be detected*/
    while( (chipIdRegValueRead != HIMAX5530_CHIP_ID_REG_VAL) && (count < max_retries))
        {
            status = HIMAX5530_ReadReg(i2cInstId, sensorI2cAddr, chipIdRegAddr, &chipIdRegValueRead, 1U);
            if(status == 0 )
            {
                if(chipIdRegValueRead == HIMAX5530_CHIP_ID_REG_VAL)
                {
                    status = 0;
                    issLogPrintf("HIMAX5530_Probe SUCCESS : Read expected value 0x%x at chip ID register 0x%x \n", HIMAX5530_CHIP_ID_REG_VAL, chipIdRegAddr);
                }
            else
                {
                    status = -1;
                    issLogPrintf("HIMAX5530_Probe : 0x%x read at chip ID register 0x%x. Expected 0x%x \n", chipIdRegValueRead, chipIdRegAddr, HIMAX5530_CHIP_ID_REG_VAL);
                    issLogPrintf("HIMAX5530 Probe Failed.. Retrying \n");
                    appLogWaitMsecs(100);
                }
            }
        else
            {
                issLogPrintf("HIMAX5530 Probe : Failed to read CHIP_ID register 0x%x \n", chipIdRegAddr);
            }
            count++;
        }
    return (status);
}

static int32_t HIMAX5530_Sensor_RegConfig(uint32_t i2cInstId, uint8_t sensorI2cAddr, I2cParams *sensorCfg, uint16_t sensor_cfg_script_len, uint32_t chId)
{   
    int32_t status = 0;
    // uint16_t regAddr;
    uint8_t regValue;
    uint16_t regInd;
    uint8_t debugMode = 1; // 0 = outputs nothing, 1 = only outputs write errors, 2 = outputs all read/writes and includes read dumps

    // HIMAX REGISTER CONFIGURATION
    if (debugMode >= 2) {
        issLogPrintf("HM5530 register debug mode is set to: 2, all write & read dumps output");
    } else if (debugMode >= 1) {
        issLogPrintf("HM5530 register debug mode is set to: 1, write errors output");
    } else {
        issLogPrintf("HM5530 register debug mode is set to: 0, no read or write dump output");
    }
    

    /* TODO:
        Delete himax5530_config.h (no longer used)
        Better way to configure 25MHz vs 19.2MHz 953 settings
        Include all 635 regisgters and add column for which to write to?
    */

    // Read dump of registers in regValData
    if (debugMode >= 2) {
        issLogPrintf("===========HM5530 Register Read Dump (Pre Write Dump)==========\n");
        for (regInd = 0; regInd < sizeof(regValData) / sizeof(regValData[0]); regInd++) {
            status = HIMAX5530_ReadReg(i2cInstId, sensorI2cAddr, regValData[regInd][0], &regValue, 1U);
            issLogPrintf("\t,0x%04x, 0x%02X \n", regValData[regInd][0], regValue);
        }
    }

    // Reset all registers to factory defaults
    issLogPrintf("===========HM5530 Register Reset==========\n");
    status = HIMAX5530_WriteReg(i2cInstId, sensorI2cAddr, 0x0103, 0x00, 1U);
    appLogWaitMsecs(1);
    if (status == 0) {
        issLogPrintf("\t0x%04x, 0x%02X Sensor reset write succeeded\n", 0x0103, 0x00);
    } else {
        issLogPrintf("\t0x%04x, 0x%02X SENSOR RESET WRITE FAILED\n", 0x0103, 0x00);
    }

    // Write dump of registers in regValData
    issLogPrintf("===========HM5530 Register Write==========\n");
    for (regInd = 0; regInd < sizeof(regValData) / sizeof(regValData[0]); regInd++) {
        if (regValData[regInd][2] == 0) {
            status = HIMAX5530_WriteReg(i2cInstId, sensorI2cAddr, regValData[regInd][0], regValData[regInd][1], 1U);
            if ((status == 0) && (debugMode >= 2)) {
                issLogPrintf("\t0x%04x, %02X write succeeded\n", regValData[regInd][0], regValData[regInd][1]);
            } else if ((status != 0) && (debugMode >= 1)) {
                issLogPrintf("\t0x%04x, %02X WRITE FAILED\n", regValData[regInd][0], regValData[regInd][1]);
            }
        } else if (debugMode >= 2) {
            issLogPrintf("\t0x%04x, %02X RO/WO register\n", regValData[regInd][0], regValData[regInd][1]);
        }
    }

    // CMU update
    status = HIMAX5530_WriteReg(i2cInstId, sensorI2cAddr, 0x0104, 0x01, 1U);
    appLogWaitMsecs(1); // Same 1 ms wait used in Gen1b system
    if (status == 0) {
        issLogPrintf("\t0x%04x, 0x%02X CMU write succeeded\n", 0x0104, 0x01);
    } else {
        issLogPrintf("\t0x%04x, 0x%02X CMU WRITE FAILED\n", 0x0104, 0x01);
    }
    
    // Read dump of registers in regValData
    if (debugMode >= 2) {
        issLogPrintf("===========HM5530 Register Read Dump (Post Write Dump)==========\n");
        for (regInd = 0; regInd < sizeof(regValData) / sizeof(regValData[0]); regInd++) {
            status = HIMAX5530_ReadReg(i2cInstId, sensorI2cAddr, regValData[regInd][0], &regValue, 1U);
            issLogPrintf("\t,0x%04x, 0x%02X \n", regValData[regInd][0], regValue);
        }
    }
    issLogPrintf("===========HM5530 Register Settings End==========\n");

    issLogPrintf("===========HM5530 GAIN TEST==========\n");
    if ( chId == 1 || chId == 5 )
    {
        status = HIMAX5530_Set_Gain(i2cInstId, sensorI2cAddr, LOW_GAIN);
        issLogPrintf("HM5530 Low Gain set %i\n",LOW_GAIN);
    }
    else
    {
        status = HIMAX5530_Set_Gain(i2cInstId, sensorI2cAddr, HIGH_GAIN);
        issLogPrintf("HM5530 High Gain set %i\n",HIGH_GAIN);
    }
    issLogPrintf("===========HM5530 GAIN TEST End==========\n");
    
#ifdef MCLK_25_MHZ
    issLogPrintf("===========HM5530 running with MCLK_25_MHZ settings==========\n");
#elif MCLK_19_2_MHZ || MCLK_15_MHZ
    issLogPrintf("===========HM5530 running with MCLK_19_2_MHZ settings==========\n");
#endif

    return status;
}

static int32_t HIMAX5530_Set_Gain(uint32_t i2cInstId, uint8_t sensorI2cAddr, double gain_dec) {
    int32_t status = 0;

    // Convert decimal gain value to HM5530’s format
    uint16_t gain_himax = (uint16_t)round(log2(gain_dec) * 16.0);
    status = HIMAX5530_WriteReg(i2cInstId, sensorI2cAddr, 0x0205, gain_himax, 1U);
    if (status != 0) {
        issLogPrintf("\t0x%04x, 0x%02X Gain write FAILED\n", 0x0205, gain_himax);
    }
    // CMU update required for gain change
    status = HIMAX5530_WriteReg(i2cInstId, sensorI2cAddr, 0x0104, 0x01, 1U);
    if (status != 0) {
        issLogPrintf("\t0x%04x, 0x%02X Gain CMU write FAILED\n", 0x0205, gain_himax);
    }
    appLogWaitMsecs(1);

    return status;
}

//static uint32_t himax5530FeaturesEnabled;
static int32_t HIMAX5530_Config(uint32_t chId, void *pSensorHdl, uint32_t sensor_features_requested)
{  // LEOH TO DO
    int32_t status = 0;
    uint32_t i2cInstId;
    uint16_t sensor_cfg_script_len = 0;
    I2cParams *sensorCfg = himax5530SensorRegConfig.sensorCfgPreScript;
    I2cParams *serCfg = himax5530SensorRegConfig.serCfgPreScript;
    IssSensors_Handle * pSenHandle = (IssSensors_Handle*)pSensorHdl;
    IssSensor_CreateParams * pCreatePrms;
    uint8_t sensorI2Caddr = HIMAX5530_I2C_ADDR;

    printf("Entered HIMAX5530_Config.\n");

    assert(NULL != pSenHandle);
    pCreatePrms = pSenHandle->createPrms;
    assert(NULL != pCreatePrms);

    if(sensor_features_requested != (sensor_features_requested & ISS_SENSOR_HIMAX5530_FEATURES))
    {
        printf("HIMAX5530_Config : Error. feature set 0x%x is not supported \n", sensor_features_requested);
        return -1;
    }

//    himax5530FeaturesEnabled= sensor_features_requested;

    i2cInstId = pCreatePrms->i2cInstId;
    
    status = HIMAX5530_Sensor_RegConfig(i2cInstId, pCreatePrms->i2cAddrSensor[chId], sensorCfg, sensor_cfg_script_len, chId);
    if(status != 0) {
        printf("HIMAX5530_Config: Error, Initial config attempt failed, power cycling and trying again.\n");
        IssSensor_DeserializerInit();
        status = UB960_SetSensorAlias(chId, sensorI2Caddr, pCreatePrms->i2cAddrSer[chId]);
        status = ub953_cfgScript(i2cInstId, pCreatePrms->i2cAddrSer[chId], serCfg);
        status = HIMAX5530_Sensor_RegConfig(i2cInstId, pCreatePrms->i2cAddrSensor[chId], sensorCfg, sensor_cfg_script_len, chId);
    }

    return (status);
}

static int32_t HIMAX5530_StreamOn(uint32_t chId, void *pSensorHdl)
{
    int32_t status = 0;

    IssSensors_Handle * pSenHandle = (IssSensors_Handle*)pSensorHdl;
    IssSensor_CreateParams * pCreatePrms;
    uint32_t i2cInstId;
    uint8_t sensorI2cAddr;
    int8_t ub960InstanceId = getUB960InstIdFromChId(chId);

    if(ub960InstanceId < 0)
    {
        printf("Invalid ub960InstanceId \n");
        return -1;
    }

    assert(NULL != pSenHandle);
    pCreatePrms = pSenHandle->createPrms;
    assert(NULL != pCreatePrms);

    i2cInstId = pCreatePrms->i2cInstId;
    sensorI2cAddr = pCreatePrms->i2cAddrSensor[chId];

    status |= HIMAX5530_WriteReg(i2cInstId, sensorI2cAddr, 0x0100, 0x1, 1u);/*ACTIVE*/
    
    return (status);
}

static int32_t HIMAX5530_StreamOff(uint32_t chId, void *pSensorHdl)
{
    int32_t status = 0;
    IssSensors_Handle * pSenHandle = (IssSensors_Handle*)pSensorHdl;
    IssSensor_CreateParams * pCreatePrms;
    uint32_t i2cInstId;
    uint8_t sensorI2cAddr;

    assert(NULL != pSenHandle);
    pCreatePrms = pSenHandle->createPrms;
    assert(NULL != pCreatePrms);

    i2cInstId = pCreatePrms->i2cInstId;
    sensorI2cAddr = pCreatePrms->i2cAddrSensor[chId];

    printf("HIMAX5530_StreamOff : chId = %i i2cInstID = %i sensorI2cAddr = 0x%x \n", chId, i2cInstId, sensorI2cAddr);   
    status |= HIMAX5530_WriteReg(i2cInstId, sensorI2cAddr, 0x0100, 0x0, 1u);/*DEACTIVATE*/
    return status;
}

static int32_t HIMAX5530_PowerOn(uint32_t chId, void *pSensorHdl)
{
    int32_t status = 0;
    // should move power on sequence here
    printf("HIMAX5530_PowerOn : chId = %i \n", chId);
    uint32_t i2cInstId;
    I2cParams *serCfg = himax5530SensorRegConfig.serCfgPreScript;
    IssSensors_Handle * pSenHandle = (IssSensors_Handle*)pSensorHdl;
    IssSensor_CreateParams * pCreatePrms;
    uint8_t sensorI2Caddr = HIMAX5530_I2C_ADDR;

    assert(NULL != pSenHandle);
    pCreatePrms = pSenHandle->createPrms;
    assert(NULL != pCreatePrms);

    i2cInstId = pCreatePrms->i2cInstId;

    printf("HIMAX5530_PowerOn : chId = %i i2cInstID = %i serializer = 0x%x configuring deserializer\n", chId, i2cInstId, pCreatePrms->i2cAddrSer[chId]);    
    status = UB960_SetSensorAlias(chId, sensorI2Caddr, pCreatePrms->i2cAddrSer[chId]);
    if(0 != status)
    {
        printf("HIMAX5530_PowerOn Error : UB960_SetSensorAlias for chId %d returned %d \n", chId, status);
    }
    else
    {
        printf("HIMAX5530_PowerOn : chId = %i i2cInstID = %i serializer = 0x%x powering on sensor configuring serializer\n", chId, i2cInstId, pCreatePrms->i2cAddrSer[chId]);    
        status = ub953_cfgScript(i2cInstId, pCreatePrms->i2cAddrSer[chId], serCfg);
        if(0 != status)
        {
            printf("HIMAX5530_PowerOn Error : UB953 config failed for camera # %d \n", chId);
        }
    }

    return status;
}

static int32_t HIMAX5530_PowerOff(uint32_t chId, void *pSensorHdl)
{
    int32_t status = 0;
    uint32_t i2cInstId;
    IssSensors_Handle * pSenHandle = (IssSensors_Handle*)pSensorHdl;
    IssSensor_CreateParams * pCreatePrms;

    assert(NULL != pSenHandle);
    pCreatePrms = pSenHandle->createPrms;
    assert(NULL != pCreatePrms);

    i2cInstId = pCreatePrms->i2cInstId;

    // should move power off sequence here
    printf("HIMAX5530_PowerOff : chId = %i \n", chId);
    

    I2cParams ub953PowerdownHIMAX[2] = {
        {0x0D, 0x20, 0x100},
        {0xFFFF, 0x00, 0x0} //End of script
    };
     
    // need to write 0x20 to 0x0D of the serializer
    printf("HIMAX5530_PowerOff : chId = %i i2cInstID = %i serializer = 0x%x \n", chId, i2cInstId, pCreatePrms->i2cAddrSer[chId]);    
    status = ub953_cfgScript(i2cInstId, pCreatePrms->i2cAddrSer[chId], ub953PowerdownHIMAX);
    

    return status;
}

//static uint16_t sp1hGainRegValueOld[ISS_SENSORS_MAX_CHANNEL];
static int32_t HIMAX5530_SetAeParams(void *pSensorHdl, uint32_t chId, IssSensor_ExposureParams *pExpPrms)
{

    // This is from the IMX390, figure out correct way to set AE for
    // Himax

/*
    uint16_t regAddr;
    uint16_t cnt;
    uint8_t regValue;
    int32_t status = -1;
    IssSensors_Handle * pSenHandle = (IssSensors_Handle*)pSensorHdl;
    IssSensor_CreateParams * pCreatePrms;
    uint32_t i2cInstId;
    uint8_t sensorI2cAddr;
    uint32_t sp1h_again = 0U;

    assert(NULL != pSenHandle);
    pCreatePrms = pSenHandle->createPrms;
    assert(NULL != pCreatePrms);

    i2cInstId = pCreatePrms->i2cInstId;
    sensorI2cAddr = pCreatePrms->i2cAddrSensor[chId];

// Exp time is fixed to 11ms for LFM. Set Analog Gain Only

    for (cnt = 0; cnt < ISS_HIMAX5530_GAIN_TBL_SIZE; cnt ++)
        {
        if (pExpPrms->analogGain[ISS_SENSOR_EXPOSURE_LONG] <= gHIMAX5530GainsTable[cnt][0])
        {
        sp1h_again = gHIMAX5530GainsTable[cnt][1];
        break;
        }
        }

    if(sp1hGainRegValueOld[chId] == sp1h_again)
    {
//Reduce I2C transactions.
//Do not write to the sensor if register value does not change
    return 0;
    }
    sp1hGainRegValueOld[chId] = sp1h_again;

    regAddr = 0x0008;
    regValue = 1;
    status = HIMAX5530_WriteReg(i2cInstId, sensorI2cAddr, regAddr, regValue, 1u);
    if(status != 0)
    {
    printf("Error writing 0x%x to HIMAX5530 register 0x%x \n", regValue, regAddr);
    }

    regAddr = HIMAX5530_SP1H_ANALOG_GAIN_CONTROL_REG_ADDR;
    regValue = sp1h_again & 0xFF;
    status = HIMAX5530_WriteReg(i2cInstId, sensorI2cAddr, regAddr, regValue, 1u);
    if(status != 0)
    {
    printf("Error writing 0x%x to HIMAX5530 register 0x%x \n", regValue, regAddr);
    }

    regAddr = HIMAX5530_SP1H_ANALOG_GAIN_CONTROL_REG_ADDR_HIGH;
    regValue = sp1h_again >> 8;
    status = HIMAX5530_WriteReg(i2cInstId, sensorI2cAddr, regAddr, regValue, 1u);
    if(status != 0)
    {
    printf("Error writing 0x%x to HIMAX5530 register 0x%x \n", regValue, regAddr);
    }

    regAddr = 0x0008;
    regValue = 0;
    status = HIMAX5530_WriteReg(i2cInstId, sensorI2cAddr, regAddr, regValue, 1u);
    if(status != 0)
    {
        printf("Error writing 0x%x to HIMAX5530 register 0x%x \n", regValue, regAddr);
    }
*/

    return 0;
}

static int32_t HIMAX5530_GetDccParams(uint32_t chId, void *pSensorHdl, IssSensor_DccParams *pDccPrms)
{
    int32_t status = 0;
    return (status);
}

static int32_t HIMAX5530_GetExpParams(uint32_t chId, void *pSensorHdl, IssSensor_ExposureParams *pExpPrms)
{
    int32_t status = 0;

//    assert(NULL != pExpPrms);
//    pExpPrms->expRatio = ISS_SENSOR_HIMAX5530_DEFAULT_EXP_RATIO;

    return (status);
}

static void HIMAX5530_InitAewbConfig(uint32_t chId, void *pSensorHdl)
{
    return;
}

static void HIMAX5530_GetIspConfig (uint32_t chId, void *pSensorHdl)
{
    return;
}

static void HIMAX5530_deinit (uint32_t chId, void *pSensorHdl)
{
    //todo
    return;
}

static int32_t HIMAX5530_ReadWriteReg (uint32_t chId, void *pSensorHdl, uint32_t readWriteFlag, I2cParams *pReg)
{
    int32_t status = 0;

    uint8_t regValue = 0;
    IssSensors_Handle * pSenHandle = (IssSensors_Handle*)pSensorHdl;
    IssSensor_CreateParams * pCreatePrms;

    assert(NULL != pSenHandle);
    pCreatePrms = pSenHandle->createPrms;

    assert(NULL != pReg);

    if (1u == readWriteFlag)
    {
        /*write*/
        regValue = pReg->nRegValue;
        status = HIMAX5530_WriteReg(pCreatePrms->i2cInstId,
        pCreatePrms->i2cAddrSensor[chId], pReg->nRegAddr, regValue, 1u);
    }
else
    {
/*read*/
        status = HIMAX5530_ReadReg(pCreatePrms->i2cInstId,
        pCreatePrms->i2cAddrSensor[chId], pReg->nRegAddr, &regValue, 1u);

        if (0 == status)
        {
            pReg->nRegValue = regValue;
        }
    }
    return (status);
}

static int32_t HIMAX5530_ReadReg(uint8_t     i2cInstId,
    uint8_t         i2cAddr,
    uint16_t        regAddr,
    uint8_t         *regVal,
    uint32_t        numRegs)
{
    int32_t  status = -1;
    I2C_Handle sensorI2cHandle = NULL;
    static uint8_t sensorI2cByteOrder = 255U;
    getIssSensorI2cInfo(&sensorI2cByteOrder, &sensorI2cHandle);
    if(NULL == sensorI2cHandle)
    {
        printf("Sensor I2C Handle is NULL \n");
        return -1;
    }
    status = Board_i2c16BitRegRd(sensorI2cHandle, i2cAddr, regAddr, regVal, numRegs, sensorI2cByteOrder, SENSOR_I2C_TIMEOUT);
    if(0 != status)
    {
        issLogPrintf("Error : I2C Timeout while reading from HIMAX5530 register 0x%x \n", regAddr);
    }
    return (status);
}

static int32_t HIMAX5530_WriteReg(uint8_t    i2cInstId,
    uint8_t       i2cAddr,
    uint16_t         regAddr,
    uint8_t          regVal,
    uint32_t      numRegs)
{
    int32_t  status = -1;
    I2C_Handle sensorI2cHandle = NULL;
    static uint8_t sensorI2cByteOrder = 255U;
    getIssSensorI2cInfo(&sensorI2cByteOrder, &sensorI2cHandle);
    if(NULL == sensorI2cHandle)
    {
        printf("Sensor I2C Handle is NULL \n");
        return -1;
    }
    status = Board_i2c16BitRegWr(sensorI2cHandle, i2cAddr, regAddr, &regVal, numRegs, sensorI2cByteOrder, SENSOR_I2C_TIMEOUT);
    if(0 != status)
    {
        printf("Error : I2C Timeout while writing 0x%x to HIMAX5530 register 0x%x \n", regVal, regAddr);
    }

    return (status);
}

static int32_t HIMAX5530_GetExpPrgFxn(uint32_t chId, void *pSensorHdl, IssAeDynamicParams *p_ae_dynPrms)
{
    // From the IMX390, stubbed out until proper exposure calculation is decided on.
/*
int32_t  status = -1;
uint8_t count = 0;

p_ae_dynPrms->targetBrightnessRange.min = 40;
p_ae_dynPrms->targetBrightnessRange.max = 50;
p_ae_dynPrms->targetBrightness = 45;
p_ae_dynPrms->threshold = 1;
p_ae_dynPrms->enableBlc = 1;
p_ae_dynPrms->exposureTimeStepSize = 1;

p_ae_dynPrms->exposureTimeRange[count].min = 11000;
p_ae_dynPrms->exposureTimeRange[count].max = 11000;
p_ae_dynPrms->analogGainRange[count].min = 1024;
p_ae_dynPrms->analogGainRange[count].max = 8192;
p_ae_dynPrms->digitalGainRange[count].min = 256;
p_ae_dynPrms->digitalGainRange[count].max = 256;
count++;

p_ae_dynPrms->numAeDynParams = count;
*/
    return 0;
}
