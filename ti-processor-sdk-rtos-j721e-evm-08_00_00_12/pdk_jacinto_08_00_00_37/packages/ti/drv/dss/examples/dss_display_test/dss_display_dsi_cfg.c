/*
 *  Copyright (c) Texas Instruments Incorporated 2018
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 *  \file dss_display_test.c
 *
 *  \brief DSS sample application that displays two ARGB32 buffers.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <string.h>
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>

#include <ti/drv/i2c/I2C.h>
#include <ti/drv/i2c/soc/I2C_soc.h>
#include <ti/board/src/devices/board_devices.h>
#include <ti/board/board.h>
#include <ti/board/board_cfg.h>
#include <ti/board/src/j721e_evm/include/board_i2c_io_exp.h>

#include <dss_display_test.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

extern void App_wait(uint32_t wait_in_ms);
static int32_t DispApp_SetBoardMux();
static int32_t DispApp_InitI2c(DispApp_Obj *appObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

I2C_Handle gI2cHandle = NULL;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


uint8_t serdesConfig[][4] = {
{0x16, 0x01, 0x0A, 0x5},
{0x16, 0x03, 0x9A, 0x5},
{0x16, 0x17, 0x9E, 0x5},
{0x16, 0x07, 0x58, 0x5},
{0x16, 0x08, 0x22, 0x5},
{0x16, 0x70, 0x80, 0x5},
{0x16, 0x77, 0x24, 0x5},


{0x16, 0x01, 0x08, 0x5},
{0x16, 0x1E, 0x01, 0x5},
{0x16, 0x03, 0x9A, 0x5},
{0x16, 0x03, 0x9A, 0x5},
{0x16, 0x03, 0x9A, 0x5},
{0x16, 0x40, 0x04, 0x5},
{0x16, 0x40, 0x05, 0x5},
{0x16, 0x41, 0x21, 0x5},
{0x16, 0x42, 0x60, 0x5},
{0x16, 0x40, 0x09, 0x5},
{0x16, 0x40, 0x09, 0x5},
{0x16, 0x41, 0x21, 0x5},
{0x16, 0x42, 0x60, 0x5},
{0x16, 0x5b, 0x85, 0x5},
{0x16, 0x4f, 0x8c, 0x5},
{0x16, 0x4f, 0x84, 0x5},
{0x16, 0x40, 0x05, 0x5},
{0x16, 0x40, 0x04, 0x5},
{0x16, 0x41, 0x05, 0x5},
{0x16, 0x42, 0x16, 0x5},
{0x16, 0x40, 0x08, 0x5},
{0x16, 0x40, 0x08, 0x5},
{0x16, 0x41, 0x05, 0x5},
{0x16, 0x42, 0x0c, 0x5},
{0x16, 0x01, 0x00, 0x5},
{0x16, 0x66, 0x03, 0x5},
{0x16, 0x67, 0x03, 0x5},
{0x16, 0x65, 0x01, 0x5},
{0x16, 0x64, 0x00, 0x5},
{0x16, 0x64, 0x04, 0x5},


{0x27, 0x00, 0xFE, 0x5},

{0x11, 0x1D, 0x28, 0x5},
{0x11, 0x1D, 0x29, 0x5},

{0x11, 0x01, 0x06, 0x5},
{0x11, 0x01, 0x04, 0x5},
{0x11, 0x03, 0xf0, 0x5},
{0x11, 0x03, 0xf0, 0x5},
{0x11, 0x03, 0xf8, 0x5},
{0x11, 0x29, 0x00, 0x5},
{0x11, 0x29, 0x00, 0x5},
{0x11, 0x65, 0x00, 0x5},
{0x11, 0x65, 0x00, 0x5},

{0x12, 0x0C, 0x20, 0x5},
{0x12, 0x00, 0x01, 0x5},
{0x12, 0x04, 0xE6, 0x5},

};

static int32_t DispApp_SetBoardMux()
{
    Board_I2cInitCfg_t i2cCfg;

    /*setting power mux for dsi lcd*/
    i2cCfg.i2cInst   = BOARD_I2C_IOEXP_DEVICE4_INSTANCE;
    i2cCfg.socDomain = BOARD_SOC_DOMAIN_MAIN;
    i2cCfg.enableIntr = false;
    Board_setI2cInitConfig(&i2cCfg);

    /* Enable DSI in IO Expander */
    Board_i2cIoExpInit();

    /* Enable Reset for external Power */
    Board_i2cIoExpSetPinDirection(BOARD_I2C_IOEXP_DEVICE4_ADDR,
                                  ONE_PORT_IOEXP,
                                  PORTNUM_0,
                                  PIN_NUM_7,
                                  PIN_DIRECTION_OUTPUT);
    Board_i2cIoExpPinLevelSet(BOARD_I2C_IOEXP_DEVICE4_ADDR,
                              ONE_PORT_IOEXP,
                              PORTNUM_0,
                              PIN_NUM_7,
                              GPIO_SIGNAL_LEVEL_HIGH);

    /* Enable Power Switch control */
    Board_i2cIoExpSetPinDirection(BOARD_I2C_IOEXP_DEVICE4_ADDR,
                                  ONE_PORT_IOEXP,
                                  PORTNUM_0,
                                  PIN_NUM_2,
                                  PIN_DIRECTION_OUTPUT);

    Board_i2cIoExpPinLevelSet(BOARD_I2C_IOEXP_DEVICE4_ADDR,
                              ONE_PORT_IOEXP,
                              PORTNUM_0,
                              PIN_NUM_2,
                              GPIO_SIGNAL_LEVEL_HIGH);
    Board_i2cIoExpDeInit();

    return (FVID2_SOK);
}

static int32_t DispApp_InitI2c(DispApp_Obj *appObj)
{
    int32_t status = FVID2_SOK;
    uint32_t loopCnt;
    uint8_t domain, i2cInst, slaveAddr;
    I2C_Params i2cParams;
    I2C_HwAttrs i2cConfig;

    /* Initialize I2C Driver */
    for(loopCnt = 0; loopCnt < I2C_HWIP_MAX_CNT; loopCnt++)
    {
        I2C_socGetInitCfg(loopCnt, &i2cConfig);
        i2cConfig.enableIntr = false;
        I2C_socSetInitCfg(loopCnt, &i2cConfig);
    }

    /* Initializes the I2C */
    I2C_init();

    /* Initializes the I2C Parameters */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz; /* 400KHz */

    Board_fpdUb941GetI2CAddr(&domain, &i2cInst, &slaveAddr);

    /* Configures the I2C instance with the passed parameters*/
    gI2cHandle = I2C_open(i2cInst, &i2cParams);
    if(gI2cHandle == NULL)
    {
        App_print("\nI2C Open failed!\n");
        status = FVID2_EFAIL;
    }

    return (status);
}

int32_t DispApp_SetDsiSerdesCfg(DispApp_Obj *appObj)
{
    int32_t status;
    uint32_t cnt, clientAddr;

    status = DispApp_SetBoardMux();

    if (FVID2_SOK == status)
    {
        status = DispApp_InitI2c(appObj);
    }

    if (FVID2_SOK == status)
    {
        for (cnt = 0; cnt < sizeof(serdesConfig)/4; cnt ++)
        {
            clientAddr = serdesConfig[cnt][0];
            status = Board_i2c8BitRegWr(
                gI2cHandle, clientAddr, serdesConfig[cnt][1],
                &serdesConfig[cnt][2], 1U, BOARD_I2C_TRANSACTION_TIMEOUT);

            App_wait((uint32_t)serdesConfig[cnt][3]);

            if (0 != status)
            {
                App_print("\n Write Failed for 0x%x 0x%x 0x%x !\n",
                    clientAddr, serdesConfig[cnt][1], serdesConfig[cnt][2]);
                break;
            }
        }
    }

    I2C_close(gI2cHandle);

    return (status);
}
