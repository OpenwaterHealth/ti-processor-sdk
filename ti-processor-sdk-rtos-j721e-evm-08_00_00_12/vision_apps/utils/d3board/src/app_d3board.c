/*!
 * @file        app_d3board.c
 * @brief       d3board control server
 * @details     This file implements the remote service server
 *              that controls the D3 personality card
 *
 * @copyright   Copyright (C) 2021 D3 Engineering, LLC. All Rights Reserved.
 */
#include <utils/remote_service/include/app_remote_service.h>
#include <utils/console_io/include/app_log.h>
#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>
#include <ti/board/board.h>
#include <ti/board/src/d3_rvp/include/board_cfg.h>
#include <utils/d3board/include/app_d3board.h>
#include <utils/d3board/include/app_d3board_gpio.h>

static int32_t init(AppD3boardInitCmdParams *cmdPrms);
static int32_t deinit(AppD3boardDeInitCmdParams *cmdPrms);
static int32_t query(AppD3boardQueryCmdParams *cmdPrms);

static int32_t enableLaser(AppD3boardQueryCmdParams *cmdPrms);
static int32_t disableLaser(AppD3boardQueryCmdParams *cmdPrms);
static int32_t queryLaser(AppD3boardQueryCmdParams *cmdPrms);

static AppD3boardObj gD3boardObj= {
    .frameSyncState= APP_REMOTE_SERVICE_D3BOARD_DISABLED,
    .isGPIO_initialized= 0,
    .laserOutputState= APP_REMOTE_SERVICE_D3BOARD_LASER_DISABLED,
    .hwTimerObj.fSyncPeriod= 0,
    .hwTimerObj.fSyncPeriod= 0,
    .hwTimerObj.strobeLightDuration= 0,
    .hwTimerObj.fSyncDuration= 0,
    .hwTimerObj.timerResolution= 225,
};

#if 0
AppD3board_HwTimerObj hwTimerObj= {
    .fSyncPeriod= 0,
    .strobeLightDuration= 0,
    .fSyncDuration= 0,
    .ranAtLeastOnce= 0,
    .frameCount= 0,
    .firstFrameTimerStamp= 0,

    .startfSyncTimer= NULL,
    .stopfSyncTimer= NULL,
    .stopStrobeLightTimer= NULL
};
#endif
int32_t appRemoteServiceD3boardInit()
{
    int32_t status = 0;

    status = appRemoteServiceRegister(
        APP_REMOTE_SERVICE_D3BOARD_NAME, appRemoteServiceD3boardHandler);
    if(status==0)
    {
        appLogPrintf(
            "REMOTE_SERVICE_D3BOARD: remote service D3 board registered\n");
    }
    else
    {
        appLogPrintf(
            "REMOTE_SERVICE_D3BOARD: ERROR: Unable to register remote service D3 Board handler\n");
    }
    return status;
}

int32_t appRemoteServiceD3boardDeInit()
{
    int32_t status = 0;

    status = appRemoteServiceUnRegister(APP_REMOTE_SERVICE_D3BOARD_NAME);
    if(status==0)
    {
        appLogPrintf(
            "REMOTE_SERVICE_D3BOARD: remote service D3 board unregistered\n");
    }
    else
    {
        appLogPrintf(
            "REMOTE_SERVICE_D3BOARD: ERROR: Unable to un-register remote service D3 Board handler\n");
    }

    return status;
}

int32_t appRemoteServiceD3boardHandler(char *service_name, uint32_t cmd,
    void *prm, uint32_t prm_size, uint32_t flags)
{
    int32_t                 status = 0;

    if (NULL != prm)
    {
        appLogPrintf(
            "REMOTE_SERVICE_D3BOARD: Received command %d !!!\n", cmd);

        switch(cmd)
        {
            case APP_REMOTE_SERVICE_D3BOARD_CMD_INIT:
                status = init((AppD3boardInitCmdParams *)prm);
                if (0 == status)
                {
                    appLogPrintf(
                        "REMOTE_SERVICE_D3BOARD: Initialization done !!!\n");
                }
                else
                {
                    appLogPrintf(
                        "REMOTE_SERVICE_D3BOARD: ERROR: Initialization failed !!!\n");
                }
                break;

            case APP_REMOTE_SERVICE_D3BOARD_CMD_START_FSYNC:
                if ((gD3boardObj.frameSyncState == APP_REMOTE_SERVICE_D3BOARD_ENABLED) || (gD3boardObj.frameSyncState == APP_REMOTE_SERVICE_D3BOARD_OFF))
                {
                    gD3boardObj.frameSyncState = APP_REMOTE_SERVICE_D3BOARD_ON;
                    appLogPrintf(
                        "REMOTE_SERVICE_D3BOARD: Frame sync ON !!!\n");
                }
                break;

            case APP_REMOTE_SERVICE_D3BOARD_CMD_STOP_FSYNC:
                if (gD3boardObj.frameSyncState == APP_REMOTE_SERVICE_D3BOARD_ON)
                {
                    gD3boardObj.frameSyncState = APP_REMOTE_SERVICE_D3BOARD_OFF;
                    appLogPrintf(
                        "REMOTE_SERVICE_D3BOARD: Frame sync OFF !!!\n");
                }
                break;
            case APP_REMOTE_SERVICE_D3BOARD_CMD_ENABLE_LASER:
                enableLaser((AppD3boardQueryCmdParams *)prm);
                break;
            case APP_REMOTE_SERVICE_D3BOARD_CMD_DISABLE_LASER:
                disableLaser((AppD3boardQueryCmdParams *)prm);
                break;
            case APP_REMOTE_SERVICE_D3BOARD_CMD_QUERY_LASER:
                status = queryLaser((AppD3boardQueryCmdParams *)prm);
                if (0 == status)
                {
                    appLogPrintf(
                        "REMOTE_SERVICE_D3BOARD: Query Laser State Completed !!!\n");
                }
                else
                {
                    appLogPrintf(
                        "REMOTE_SERVICE_D3BOARD: Failed to Query Laser State !!!\n");
                }
                break;
            case APP_REMOTE_SERVICE_D3BOARD_CMD_QUERY:
                status = query((AppD3boardQueryCmdParams *)prm);
                if (0 == status)
                {
                    appLogPrintf(
                        "REMOTE_SERVICE_D3BOARD: Initialization done !!!\n");
                }
                else
                {
                    appLogPrintf(
                        "REMOTE_SERVICE_D3BOARD: ERROR: Initialization failed !!!\n");
                }
                break;

            case APP_REMOTE_SERVICE_D3BOARD_CMD_DEINIT:
                status = deinit((AppD3boardDeInitCmdParams *)prm);
                if (0 == status)
                {
                    appLogPrintf(
                        "REMOTE_SERVICE_D3BOARD: Deinitialization done !!!\n");
                }
                else
                {
                    appLogPrintf(
                        "REMOTE_SERVICE_D3BOARD: ERROR: Deinitialization failed !!!\n");
                }
                break;
        }

    }
    else
    {
        appLogPrintf(
            "REMOTE_SERVICE_D3BOARD: Invalid parameters passed !!!\n");
    }

    return (status);
}

static pinmuxPerCfg_t gfSyncPinCfg[] =
{
    /* PIN_PRG1_PRU1_GPO4 -> TIMER_IO3 -> AH24 */
    {
        PIN_PRG1_PRU1_GPO4, PIN_MODE(14) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* prg1_pru1_gpo3 -> TIMER_IO2 -> AD23 */
    {
        PIN_PRG1_PRU1_GPO3, PIN_MODE(14) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    {PINMUX_END}
};

static pinmuxPerCfg_t gStrobeLightPinCfg[] =
{
    /* MyGPIO0 -> GPIO0_64 -> AD28 */
    {
        PIN_PRG0_PRU1_GPO1, PIN_MODE(7) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    {PINMUX_END}
};


static pinmuxPerCfg_t gStbTestPinCfg[] =
{
    /* prg1_pru1_gpo6 -> timer_io5 -> AE23 (testpoint 6) */
    {
        PIN_PRG1_PRU1_GPO6, PIN_MODE(14) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* ? -> timer_io0 -> v6 (testpoint 224) */
    {
        PIN_TIMER_IO0, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    {PINMUX_END}
};
#define FSYNC_PIN_IDX 0
#define STROBELIGHT_PIN_IDX 1
#define STB_TEST_PIN_IDX 2

static pinmuxModuleCfg_t gPinCfg[] =
{
    {0, FALSE, gfSyncPinCfg},
    {0, FALSE, gStrobeLightPinCfg},
    {0, FALSE, gStbTestPinCfg},
    {PINMUX_END}
};

static pinmuxBoardCfg_t gD3boardPinmuxDataInfo[] =
{
    {0, gPinCfg},
    {PINMUX_END}
};

static int32_t init(AppD3boardInitCmdParams *cmdPrms)
{
    int32_t j;
    int32_t status = 0;

    gD3boardObj.frameSyncState = APP_REMOTE_SERVICE_D3BOARD_DISABLED;

    if ((cmdPrms->moduleId & APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC) == APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC)
    {
        AppD3board_HwTimerParams hwTimerParams;

        /* enable every pinpad's pin's output drive */
        gPinCfg[FSYNC_PIN_IDX].doPinConfig= TRUE;
        gPinCfg[STROBELIGHT_PIN_IDX].doPinConfig= TRUE;
        gPinCfg[STB_TEST_PIN_IDX].doPinConfig= TRUE;
        for(j = 0; (PINMUX_END != gfSyncPinCfg[j].pinOffset); j++)
        {
            gfSyncPinCfg[j].pinSettings= gfSyncPinCfg[j].pinSettings & (~PIN_OUTPUT_DISABLE);
        }
        Board_pinmuxUpdate(gD3boardPinmuxDataInfo,
                           BOARD_SOC_DOMAIN_MAIN);
        gPinCfg[FSYNC_PIN_IDX].doPinConfig= FALSE;
        gPinCfg[STROBELIGHT_PIN_IDX].doPinConfig= FALSE;
        gPinCfg[STB_TEST_PIN_IDX].doPinConfig= FALSE;

        /* Initialize GPIO module */
        if (gD3boardObj.isGPIO_initialized == 0)
        {
            GPIO_init();
            gD3boardObj.isGPIO_initialized= 1;
        }

        //Turn on TP7 on the D3 personality board
        GPIO_write(GPIO_TP7_IDX, 1);
        // Turn off strobe light
        GPIO_write(GPIO_STROBELIGHT_IDX, 0);
        gD3boardObj.laserOutputState= APP_REMOTE_SERVICE_D3BOARD_LASER_DISABLED;
        hwTimerParams.fSyncPeriod= cmdPrms->fSyncPeriod;
        hwTimerParams.strobeLightOffset= cmdPrms->strobeLightDelayStart;
        hwTimerParams.strobeLightDuration= (cmdPrms->strobeLightDuration);
        hwTimerParams.fSyncDuration= cmdPrms->fSyncDuration;
        hwTimerParams.timerResolution = cmdPrms->timerResolution;
        status= appD3board_initHwTimer(&gD3boardObj.hwTimerObj, &hwTimerParams);
        if (status==0)
        {
            gD3boardObj.frameSyncState= APP_REMOTE_SERVICE_D3BOARD_ENABLED;
            appLogPrintf(
            "REMOTE_SERVICE_D3BOARD: Initialized timers with framesync= %d ns, "
            "strobeLightDelay= %d ns, "
            "fSyncDuration= %d ns!\n",
            hwTimerParams.fSyncPeriod * hwTimerParams.timerResolution * 4,
            hwTimerParams.strobeLightDuration * hwTimerParams.timerResolution * 4,
            hwTimerParams.fSyncDuration * hwTimerParams.timerResolution * 4);
        }
        else
        {
            appLogPrintf(
            "REMOTE_SERVICE_D3BOARD: Unable to allocate all the timers !!!\n");
        }
    }
    else
    {
        appLogPrintf(
            "REMOTE_SERVICE_D3BOARD: ERROR: Unrecognized module !!!\n");
        status = -1;
    }

    return status;
}

static int32_t enableLaser(AppD3boardQueryCmdParams *cmdPrms)
{
    int32_t status = 0;
    
    if ((cmdPrms->moduleId & APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC) == APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC)
    {
        appD3board_enableLaserOutput(&gD3boardObj.hwTimerObj);
        
        gD3boardObj.laserOutputState= APP_REMOTE_SERVICE_D3BOARD_LASER_ENABLED;
        cmdPrms->state = APP_REMOTE_SERVICE_D3BOARD_LASER_ENABLED;
    }
    else
    {
        appLogPrintf(
            "REMOTE_SERVICE_D3BOARD: ERROR: Unrecognized module !!!\n");
        status = -1;
    }

    return status;
}

static int32_t disableLaser(AppD3boardQueryCmdParams *cmdPrms)
{
    int32_t status = 0;

    if ((cmdPrms->moduleId & APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC) == APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC)
    {
        appD3board_disableLaserOutput(&gD3boardObj.hwTimerObj);
        
        gD3boardObj.laserOutputState= APP_REMOTE_SERVICE_D3BOARD_LASER_DISABLED;
        cmdPrms->state = APP_REMOTE_SERVICE_D3BOARD_LASER_DISABLED;
        GPIO_write(GPIO_STROBELIGHT_IDX, 0);
    }
    else
    {
        appLogPrintf(
            "REMOTE_SERVICE_D3BOARD: ERROR: Unrecognized module !!!\n");
        status = -1;
    }

    return status;
}

static int32_t queryLaser(AppD3boardQueryCmdParams *cmdPrms)
{
    int32_t status = 0;

    if ((cmdPrms->moduleId & APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC) == APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC)
    {
        cmdPrms->state= gD3boardObj.laserOutputState;
    }
    else
    {
        appLogPrintf(
            "REMOTE_SERVICE_D3BOARD: ERROR: Unrecognized module !!!\n");
        status = -1;
    }

    return status;
}

static int32_t deinit(AppD3boardDeInitCmdParams *cmdPrms)
{
    int32_t j;
    int32_t status = 0;

    if ((cmdPrms->moduleId & APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC) == APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC)
    {
        appD3board_deInitHwTimer(&gD3boardObj.hwTimerObj);

        // Turn off TP7 on the D3 personality board
        GPIO_write(GPIO_TP7_IDX, 0);
        gD3boardObj.frameSyncState = APP_REMOTE_SERVICE_D3BOARD_DISABLED;

        /* Disable every pinpad's pin's output drive */
        gPinCfg[FSYNC_PIN_IDX].doPinConfig= TRUE;
        gPinCfg[STROBELIGHT_PIN_IDX].doPinConfig= TRUE;
        gPinCfg[STB_TEST_PIN_IDX].doPinConfig= TRUE;
        for(j = 0; (PINMUX_END != gfSyncPinCfg[j].pinOffset); j++)
        {
            gfSyncPinCfg[j].pinSettings= gfSyncPinCfg[j].pinSettings | PIN_OUTPUT_DISABLE;
        }
        Board_pinmuxUpdate(gD3boardPinmuxDataInfo,
                           BOARD_SOC_DOMAIN_MAIN);
        gPinCfg[FSYNC_PIN_IDX].doPinConfig= FALSE;
        gPinCfg[STROBELIGHT_PIN_IDX].doPinConfig= FALSE;
        gPinCfg[STB_TEST_PIN_IDX].doPinConfig= FALSE;


        gD3boardObj.isGPIO_initialized= 0;

    }
    else
    {
        appLogPrintf(
            "REMOTE_SERVICE_D3BOARD: ERROR: Unrecognized module !!!\n");
        status = -1;
    }

    return status;
}

static int32_t query(AppD3boardQueryCmdParams *cmdPrms)
{
    int32_t status = 0;

    if ((cmdPrms->moduleId & APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC) == APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC)
    {
        cmdPrms->state= gD3boardObj.frameSyncState;
    }
    else
    {
        appLogPrintf(
            "REMOTE_SERVICE_D3BOARD: ERROR: Unrecognized module !!!\n");
        status = -1;
    }

    return status;
}

