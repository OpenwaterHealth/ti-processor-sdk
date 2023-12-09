#ifndef APP_D3BOARD_H_
#define APP_D3BOARD_H_

/*!
 * @file        app_d3board.h
 * @brief       Header file for the d3board control server
 * @details     d3board implements the remote service server
 *              that controls the D3 personality card
 *
 * @copyright   Copyright (C) 2021 D3 Engineering, LLC. All Rights Reserved.
 */

/**
 * \defgroup group_vision_apps_utils_d3board Personality card API (TI-RTOS only)
 *
 * \brief This section contains APIs for the Personality card
 *
 * \ingroup group_vision_apps_utils
 *
 * @{
 */
#include <stdint.h>

#define MILLISEC 1000u
#define MICROSEC 1u

#define APP_REMOTE_SERVICE_D3BOARD_NAME  "com.ti.remote_service_d3board"

enum AppRemoteServiceD3boardCmdId {APP_REMOTE_SERVICE_D3BOARD_CMD_INIT=0, APP_REMOTE_SERVICE_D3BOARD_CMD_START_FSYNC, APP_REMOTE_SERVICE_D3BOARD_CMD_STOP_FSYNC, APP_REMOTE_SERVICE_D3BOARD_CMD_QUERY, APP_REMOTE_SERVICE_D3BOARD_CMD_ENABLE_LASER, APP_REMOTE_SERVICE_D3BOARD_CMD_DISABLE_LASER, APP_REMOTE_SERVICE_D3BOARD_CMD_QUERY_LASER, APP_REMOTE_SERVICE_D3BOARD_CMD_DEINIT};
enum AppRemoteServiceD3boardModuleId {APP_REMOTE_SERVICE_D3BOARD_FRAMESYNC=1<<0, APP_REMOTE_SERVICE_D3BOARD_NUM_MODULE=1};
enum AppRemoteServiceD3boardState {APP_REMOTE_SERVICE_D3BOARD_ENABLED=0, APP_REMOTE_SERVICE_D3BOARD_ON, APP_REMOTE_SERVICE_D3BOARD_OFF, APP_REMOTE_SERVICE_D3BOARD_DISABLED}; 
enum AppRemoteServiceD3boardLaserState {APP_REMOTE_SERVICE_D3BOARD_LASER_DISABLED=0, APP_REMOTE_SERVICE_D3BOARD_LASER_ENABLED}; 

#define MODULE_IDX(a) (a)

typedef struct
{
    /*Inputs */
    uint32_t moduleId;
    uint32_t fSyncPeriod;
    uint32_t strobeLightDelayStart;
    uint32_t strobeLightDuration;
    uint32_t fSyncDuration;
    uint32_t timerResolution;
} AppD3boardInitCmdParams;

typedef struct
{
    /*moduleId Input */
    uint32_t moduleId;
    /* state Output */
    uint32_t state;
} AppD3boardQueryCmdParams;

typedef struct
{
    /*moduleId Input */
    uint32_t moduleId;
} AppD3boardCmdParams;

typedef struct
{
    /*moduleId Input */
    uint32_t moduleId;
} AppD3boardDeInitCmdParams;

typedef struct
{
    uint32_t fSyncPeriod;
    uint32_t fSyncDuration;
    uint32_t strobeLightOffset;
    uint32_t strobeLightDuration;
    uint32_t timerResolution;
} AppD3board_HwTimerObj;

typedef struct
{
    uint32_t isGPIO_initialized;
    uint32_t frameSyncState;
    uint32_t laserOutputState;
    AppD3board_HwTimerObj hwTimerObj;

} AppD3boardObj;

typedef struct
{
    uint32_t fSyncPeriod;
    uint32_t strobeLightOffset;
    uint32_t strobeLightDuration;
    uint32_t fSyncDuration;
    uint32_t timerResolution;
} AppD3board_HwTimerParams;

int32_t appRemoteServiceD3boardInit(void);

int32_t appRemoteServiceD3boardDeInit(void);

int32_t appRemoteServiceD3boardHandler(char *service_name, uint32_t cmd,
    void *prm, uint32_t prm_size, uint32_t flags);

int32_t appD3board_initHwTimer(AppD3board_HwTimerObj *obj, AppD3board_HwTimerParams *params);
int32_t appD3board_deInitHwTimer(AppD3board_HwTimerObj *obj);
int32_t appD3board_enableLaserOutput(AppD3board_HwTimerObj *obj);
int32_t appD3board_disableLaserOutput(AppD3board_HwTimerObj *obj);

/* @} */

#endif
