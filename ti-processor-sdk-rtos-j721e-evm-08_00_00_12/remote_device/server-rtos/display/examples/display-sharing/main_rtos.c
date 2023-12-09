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

#include <stdio.h>
#include <stdint.h>

/* OSAL */
#include <ti/osal/osal.h>
#include <ti/osal/SemaphoreP.h>
#include <ti/osal/TaskP.h>

#include <ti/drv/ipc/ipc.h>
#include <ti/drv/dss/dss.h>
#include <ti/board/board.h>
#include <ti/board/src/j721e_evm/include/board_cfg.h>
#include <ti/board/src/j721e_evm/include/board_pinmux.h>
#include <ti/csl/soc/cslr_soc_ctrl_mmr.h>

#include <ti/drv/sciclient/sciclient.h>

#include <server-rtos/remote-device.h>
#include <server-rtos/include/app_remote_device_display.h>

#include "ipc_rsctable.h"
#include "app_dss.h"

#define IPC_RPMESSAGE_OBJ_SIZE  256
#define VQ_BUF_SIZE             2048
#define REMOTE_DEVICE_ENDPT     26
#define RPMSG_DATA_SIZE         (256*512 + IPC_RPMESSAGE_OBJ_SIZE)
#define VRING_BASE_ADDRESS      0xAA000000

#ifdef SYSBIOS
  /* To print to SysMin Buffer(view from CCS ROV) */
  #include <xdc/runtime/System.h>
  #define App_printf System_printf
#else
  #include "ipc_trace.h"
  #define App_printf  Ipc_Trace_printf
#endif

static uint8_t g_dssStackBuf[IPC_TASK_STACKSIZE] __attribute__ ((section(".bss:taskStackSection"))) __attribute__ ((aligned(8192)));
static uint8_t g_monitorStackBuf[IPC_TASK_STACKSIZE] __attribute__ ((section(".bss:taskStackSection"))) __attribute__ ((aligned(8192)));
static uint8_t g_rdevStackBuf[IPC_TASK_STACKSIZE] __attribute__ ((section(".bss:taskStackSection"))) __attribute__ ((aligned(8192)));
static uint8_t g_ipcStackBuf[IPC_TASK_STACKSIZE] __attribute__ ((section(".bss:taskStackSection"))) __attribute__ ((aligned(8192)));
static uint8_t g_vdevMonStackBuf[IPC_TASK_STACKSIZE] __attribute__ ((section(".bss:taskStackSection"))) __attribute__ ((aligned(8192)));
static uint8_t g_mainStackBuf[IPC_TASK_STACKSIZE] __attribute__ ((section(".bss:taskStackSection"))) __attribute__ ((aligned(8192)));
static uint8_t ctrlTaskBuf[IPC_TASK_STACKSIZE] __attribute__ ((section(".bss:taskStackSection"))) __attribute__ ((aligned(8192)));

static uint8_t  sysVqBuf[VQ_BUF_SIZE]  __attribute__ ((section ("ipc_data_buffer"), aligned (8)));
static uint8_t  gCntrlBuf[RPMSG_DATA_SIZE] __attribute__ ((section("ipc_data_buffer"), aligned (8)));

static SemaphoreP_Handle g_dss_wait_sem;
static SemaphoreP_Handle g_dss_ready_sem;
static SemaphoreP_Handle g_rdev_init_wait_sem;
static SemaphoreP_Handle g_ipc_init_wait_sem;
static SemaphoreP_Handle g_rdev_start_sem;


static uint32_t selfProcId = IPC_MCU2_1;
static uint32_t gRemoteProc[] =
{
    IPC_MPU1_0, IPC_MCU1_0, IPC_MCU1_1, IPC_MCU2_0, IPC_MCU3_0, IPC_MCU3_1, IPC_C66X_1, IPC_C66X_2, IPC_C7X_1
};
static uint32_t gNumRemoteProc = sizeof(gRemoteProc)/sizeof(uint32_t);

/* App Log Print max line length */
#define APP_LOG_PRINT_MAX_LINE_LENGTH  ((uint32_t) 512U)

void appLogPrintf(const char *format, ...)
{
    char buffer[APP_LOG_PRINT_MAX_LINE_LENGTH];
    va_list args;

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    App_printf("%s", buffer);
}

static void rpmsg_vdevMonitorFxn(void* arg0, void* arg1)
{
    int32_t status;

    /* Wait for Linux VDev ready... */
    while(!Ipc_isRemoteReady(IPC_MPU1_0))
    {
        TaskP_sleep(1);
    }

    /* Create the VRing now ... */
    status = Ipc_lateVirtioCreate(IPC_MPU1_0);
    if(status != IPC_SOK)
    {
        App_printf("%s: Ipc_lateVirtioCreate failed\n", __func__);
        return;
    }

    status = RPMessage_lateInit(IPC_MPU1_0);
    if(status != IPC_SOK)
    {
        App_printf("%s: RPMessage_lateInit failed\n", __func__);
        return;
    }

    status = appRemoteDeviceLateAnnounce(IPC_MPU1_0);
    if(status != IPC_SOK)
    {
        App_printf("%s: RPMessage_announce() failed\n", __func__);
    }
}

static void monitorAndUnlockRdev(void* a0, void* a1)
{
    SemaphoreP_pend(g_ipc_init_wait_sem, SemaphoreP_WAIT_FOREVER);
    SemaphoreP_pend(g_rdev_init_wait_sem, SemaphoreP_WAIT_FOREVER);
    SemaphoreP_post(g_rdev_start_sem);

}

static void dss_init(void* a0, void* a1)
{
    Fvid2_InitPrms initPrmsFvid2;

    appConfigureDisplayPrcm(APP_DISPLAY_TYPE_HDMI);
    appConfigureHdmiLcd();

    Fvid2InitPrms_init(&initPrmsFvid2);
    initPrmsFvid2.printFxn = appLogPrintf;

    Fvid2_init(&initPrmsFvid2);

    appDssInit();

    appDctrlInit();

    SemaphoreP_post(g_dss_wait_sem);

    SemaphoreP_pend(g_dss_ready_sem, SemaphoreP_WAIT_FOREVER);

    appDssRun();
}

static void ipcPrintFxn(const char *str)
{
    App_printf("%s", str);

    return;
}

static void ipc_init(void* a0, void* a1)
{
    TaskP_Params      params;
    uint32_t          numProc = gNumRemoteProc;
    Ipc_VirtIoParams  vqParam;
    Ipc_InitPrms      initPrms;

    /* Initialize params with defaults */
    IpcInitPrms_init(0U, &initPrms);
    
    initPrms.printFxn = &ipcPrintFxn;

    Ipc_init(&initPrms);

    /* Step1 : Initialize the multiproc */
    Ipc_mpSetConfig(selfProcId, numProc, &gRemoteProc[0]);

    App_printf("IPC_echo_test (core : %s) .....\r\n",
            Ipc_mpGetSelfName());


    Ipc_loadResourceTable((void*)&ti_ipc_remoteproc_ResourceTable);

    /* Step2 : Initialize Virtio */
    vqParam.vqObjBaseAddr = (void*)&sysVqBuf[0];
    vqParam.vqBufSize     = numProc * Ipc_getVqObjMemoryRequiredPerCore();
    vqParam.vringBaseAddr = (void*)VRING_BASE_ADDRESS;
    vqParam.vringBufSize  = IPC_VRING_BUFFER_SIZE;
    vqParam.timeoutCnt    = 100;  /* Wait for counts */
    Ipc_initVirtIO(&vqParam);

    /* Step 3: Initialize RPMessage */
    RPMessage_Params cntrlParam;

    /* Initialize the param */
    RPMessageParams_init(&cntrlParam);

    /* Set memory for HeapMemory for control task */
    cntrlParam.buf         = &gCntrlBuf[0];
    cntrlParam.bufSize     = RPMSG_DATA_SIZE;
    cntrlParam.stackBuffer = &ctrlTaskBuf[0];
    cntrlParam.stackSize   = IPC_TASK_STACKSIZE;
    RPMessage_init(&cntrlParam);

    SemaphoreP_post(g_ipc_init_wait_sem);

    TaskP_Params_init(&params);
    params.priority = 1; //Default priority
    params.stacksize = IPC_TASK_STACKSIZE;
    params.stack = &g_vdevMonStackBuf[0];
    TaskP_create(rpmsg_vdevMonitorFxn, &params);
}

static void remotedev_init(void* a0, void* a1)
{
    app_remote_device_init_prm_t remote_dev_init_prm;
    app_remote_disp_init_prm_t remote_disp_init_prm;
    app_remote_disp_inst_prm_t *inst;
    app_remote_disp_disp_prm_t *disp;
    app_remote_disp_pipe_prm_t *pipe;


    appRemoteDeviceInitParamsInit(&remote_dev_init_prm);

    remote_dev_init_prm.rpmsg_buf_size = 256;
    remote_dev_init_prm.remote_device_endpt = REMOTE_DEVICE_ENDPT;
    remote_dev_init_prm.wait_sem = g_rdev_start_sem;

    SemaphoreP_pend(g_dss_wait_sem, SemaphoreP_WAIT_FOREVER);

    appRemoteDeviceInit(&remote_dev_init_prm);
    App_printf("Remote device (core : mcu2_1) .....\r\n");

    appRemoteDisplayInitPrmSetDefault(&remote_disp_init_prm);

    remote_disp_init_prm.rpmsg_buf_size = 256;
    remote_disp_init_prm.num_instances = 1;

    inst = &remote_disp_init_prm.inst_prm[0];

    inst->host_id = IPC_MPU1_0;
    inst->num_disps = 1;
    inst->num_pipes = 2;
    snprintf((char *)&inst->name[0], APP_REMOTE_DISPLAY_MAX_NAME_LEN, "r5f-tidss");

    /* Values for VP2. Must match values configured in DCTRL */
    disp = &inst->disp_prm[0];

    disp->disp_id = CSL_DSS_VP_ID_2;
    disp->width = 1920;
    disp->height = 1080;
    disp->refresh = 60;

    /* Values for VIDL2 */
    pipe = &inst->pipe_prm[0];

    pipe->disp_id = CSL_DSS_VP_ID_2;
    pipe->pipe_id = DSS_DISP_INST_VIDL2;
    pipe->scale_cap = 0;
    pipe->win_mod_cap = 1;
    pipe->pos_x = 0;
    pipe->pos_y = 0;
    pipe->width = 0;
    pipe->height = 0;
    pipe->current_zorder = 0;
    pipe->num_zorders = 2;
    pipe->zorders[0] = 0;
    pipe->zorders[1] = 1;
    pipe->num_formats = 2;
    pipe->formats[0] = FVID2_DF_BGRA32_8888;
    pipe->formats[1] = FVID2_DF_BGRX32_8888;

    /* Values for VID1 */
    pipe = &inst->pipe_prm[1];

    pipe->disp_id = CSL_DSS_VP_ID_2;
    pipe->pipe_id = DSS_DISP_INST_VID1;
    pipe->scale_cap = 1;
    pipe->win_mod_cap = 1;
    pipe->pos_x = 0;
    pipe->pos_y = 0;
    pipe->width = 0;
    pipe->height = 0;
    pipe->current_zorder = 1;
    pipe->num_zorders = 2;
    pipe->zorders[0] = 0;
    pipe->zorders[1] = 1;
    pipe->num_formats = 2;
    pipe->formats[0] = FVID2_DF_BGRA32_8888;
    pipe->formats[1] = FVID2_DF_BGRX32_8888;

    appRemoteDisplayInit(&remote_disp_init_prm);

    App_printf("Remote display device (core : mcu2_1) .....\r\n");

    SemaphoreP_post(g_rdev_init_wait_sem);
    SemaphoreP_post(g_dss_ready_sem);
}

void appConfigureSoC(void)
{
    /* Set drive strength */
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
            CSL_WKUP_CTRL_MMR_CFG0_H_IO_DRVSTRNGTH0_PROXY, 0xFU);

    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
            CSL_WKUP_CTRL_MMR_CFG0_V_IO_DRVSTRNGTH0_PROXY, 0xFU);
}

static pinmuxPerCfg_t gVout0PinCfg[] =
{
    /* MyVOUT1 -> VOUT0_DATA0 -> AE22 */
    {
        PIN_PRG1_PRU1_GPO0, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA1 -> AG23 */
    {
        PIN_PRG1_PRU1_GPO1, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA2 -> AF23 */
    {
        PIN_PRG1_PRU1_GPO2, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA3 -> AD23 */
    {
        PIN_PRG1_PRU1_GPO3, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA4 -> AH24 */
    {
        PIN_PRG1_PRU1_GPO4, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA5 -> AG21 */
    {
        PIN_PRG1_PRU1_GPO5, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA6 -> AE23 */
    {
        PIN_PRG1_PRU1_GPO6, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA7 -> AC21 */
    {
        PIN_PRG1_PRU1_GPO7, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA8 -> Y23 */
    {
        PIN_PRG1_PRU1_GPO8, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA9 -> AF21 */
    {
        PIN_PRG1_PRU1_GPO9, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA10 -> AB23 */
    {
        PIN_PRG1_PRU1_GPO10, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA11 -> AJ25 */
    {
        PIN_PRG1_PRU1_GPO11, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA12 -> AH25 */
    {
        PIN_PRG1_PRU1_GPO12, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA13 -> AG25 */
    {
        PIN_PRG1_PRU1_GPO13, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA14 -> AH26 */
    {
        PIN_PRG1_PRU1_GPO14, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA15 -> AJ27 */
    {
        PIN_PRG1_PRU1_GPO15, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA16 -> AF24 */
    {
        PIN_PRG1_PRU0_GPO11, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA17 -> AJ24 */
    {
        PIN_PRG1_PRU0_GPO12, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA18 -> AG24 */
    {
        PIN_PRG1_PRU0_GPO13, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA19 -> AD24 */
    {
        PIN_PRG1_PRU0_GPO14, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA20 -> AC24 */
    {
        PIN_PRG1_PRU0_GPO15, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA21 -> AE24 */
    {
        PIN_PRG1_PRU0_GPO16, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA22 -> AJ20 */
    {
        PIN_PRG1_PRU0_GPO8, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DATA23 -> AG20 */
    {
        PIN_PRG1_PRU0_GPO9, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_DE -> AC22 */
    {
        PIN_PRG1_PRU1_GPO17, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_EXTPCLKIN -> AH21 */
    {
        PIN_PRG1_PRU0_GPO19, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    /* MyVOUT1 -> VOUT0_HSYNC -> AJ26 */
    {
        PIN_PRG1_PRU1_GPO16, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_PCLK -> AH22 */
    {
        PIN_PRG1_PRU1_GPO19, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* MyVOUT1 -> VOUT0_VSYNC -> AJ22 */
    {
        PIN_PRG1_PRU1_GPO18, PIN_MODE(10) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    {PINMUX_END}
};

static pinmuxPerCfg_t gI2c1PinCfg[] =
{
    /* MyI2C1 -> I2C1_SCL -> Y6 */
    {
        PIN_I2C1_SCL, PIN_MODE(0) | \
        ((PIN_PULL_DIRECTION | PIN_INPUT_ENABLE) & (~PIN_PULL_DISABLE))
    },
    /* MyI2C1 -> I2C1_SDA -> AA6 */
    {
        PIN_I2C1_SDA, PIN_MODE(0) | \
        ((PIN_PULL_DIRECTION | PIN_INPUT_ENABLE) & (~PIN_PULL_DISABLE))
    },
    {PINMUX_END}
};

static pinmuxModuleCfg_t gHdmiPinCfg[] =
{
    {0, TRUE, gVout0PinCfg},
    {1, TRUE, gI2c1PinCfg},
    {PINMUX_END}
};

static pinmuxBoardCfg_t gDisplaySharingPinmuxDataInfo[] =
{
    {0, gHdmiPinCfg},
    {PINMUX_END}
};

static void taskFxn(void* a0, void* a1)
{
    TaskP_Params dss_taskParams;
    TaskP_Params ipc_taskParams;
    TaskP_Params rdev_taskParams;
    TaskP_Params monitor_taskParams;
    SemaphoreP_Params sem_params;

    Sciclient_init(NULL);

    Board_pinmuxUpdate(gDisplaySharingPinmuxDataInfo,
                       BOARD_SOC_DOMAIN_MAIN);

    appConfigureSoC();

    SemaphoreP_Params_init(&sem_params);
    sem_params.mode = SemaphoreP_Mode_BINARY;
    g_dss_ready_sem = SemaphoreP_create(0, &sem_params);

    SemaphoreP_Params_init(&sem_params);
    sem_params.mode = SemaphoreP_Mode_BINARY;
    g_dss_wait_sem = SemaphoreP_create(0, &sem_params);

    SemaphoreP_Params_init(&sem_params);
    sem_params.mode = SemaphoreP_Mode_BINARY;
    g_ipc_init_wait_sem = SemaphoreP_create(0, &sem_params);

    SemaphoreP_Params_init(&sem_params);
    sem_params.mode = SemaphoreP_Mode_BINARY;
    g_rdev_init_wait_sem = SemaphoreP_create(0, &sem_params);

    SemaphoreP_Params_init(&sem_params);
    sem_params.mode = SemaphoreP_Mode_BINARY;
    g_rdev_start_sem = SemaphoreP_create(0, &sem_params);

    TaskP_Params_init(&ipc_taskParams);
    ipc_taskParams.priority = 2; // Higher than default priority
    ipc_taskParams.stack = &g_ipcStackBuf[0];
    ipc_taskParams.stacksize = IPC_TASK_STACKSIZE;
    TaskP_create(ipc_init, &ipc_taskParams);

    TaskP_Params_init(&rdev_taskParams);
    rdev_taskParams.priority = 2; // higher than default priority
    rdev_taskParams.stack = &g_rdevStackBuf[0];
    rdev_taskParams.stacksize = IPC_TASK_STACKSIZE;
    TaskP_create(remotedev_init, &rdev_taskParams);

    TaskP_Params_init(&monitor_taskParams);
    monitor_taskParams.priority = 2; // higher than default priority
    monitor_taskParams.stack = &g_monitorStackBuf[0];
    monitor_taskParams.stacksize = IPC_TASK_STACKSIZE;
    TaskP_create(monitorAndUnlockRdev, &monitor_taskParams);

    TaskP_Params_init(&dss_taskParams);
    dss_taskParams.priority = 2; //higher than default priority
    dss_taskParams.stack = &g_dssStackBuf[0];
    dss_taskParams.stacksize = IPC_TASK_STACKSIZE;
    TaskP_create(dss_init, &dss_taskParams);
}

int main(void)
{
    TaskP_Handle task;
    TaskP_Params taskParams;

    OS_init();

    /* Initialize the task params */
    TaskP_Params_init(&taskParams);
    /* Set the task priority higher than the default priority (1) */
    taskParams.priority = 2;
    taskParams.stack = &g_mainStackBuf[0];
    taskParams.stacksize = IPC_TASK_STACKSIZE;

    task = TaskP_create(taskFxn, &taskParams);
    if(NULL == task)
    {
        OS_stop();
    }
    OS_start();    /* does not return */

    return(0);
}

