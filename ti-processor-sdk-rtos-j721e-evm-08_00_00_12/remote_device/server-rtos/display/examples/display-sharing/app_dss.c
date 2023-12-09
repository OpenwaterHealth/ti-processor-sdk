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

#include <ti/drv/dss/dss.h>
#include <ti/csl/soc.h>
#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>

#include <ti/drv/sciclient/sciclient.h>
#include <ti/board/src/j721e_evm/include/board_control.h>
#include <ti/csl/soc/cslr_soc_ctrl_mmr.h>

#include "app_dss.h"
#include "app_sciclient_helper.h"

#ifdef SYSBIOS
  /* To print to SysMin Buffer(view from CCS ROV) */
  #include <xdc/runtime/System.h>
  #define App_printf System_printf
#else
  #include "ipc_trace.h"
  #define App_printf  Ipc_Trace_printf
#endif

#define APP_DSS_TEST_BUFFER_RETURN_TASK_STACK_SIZE           (0x2000)
#define APP_DSS_TEST_QUEUE_TASK_STACK_SIZE                   (0x2000)
#define FREE (0)
#define BUSY (1)

static SemaphoreP_Handle g_pipe_syncSem;
static SemaphoreP_Handle g_lock_sem;
static Fvid2_Handle g_pipe_handle;
static uint8_t appDssTestBufferReturnTaskStack[APP_DSS_TEST_BUFFER_RETURN_TASK_STACK_SIZE];
static uint8_t appDssTestQueueTaskStack[APP_DSS_TEST_QUEUE_TASK_STACK_SIZE];

static Fvid2_Handle g_dctrl_handle;
static Dss_DctrlVpParams g_dctrl_vp_params;
static Dss_DctrlPathInfo g_dctrl_path_info;
static Dss_DctrlOverlayParams g_dctrl_overlay_params;
static Dss_DctrlOverlayLayerParams g_dctrl_layer_params;
static Dss_DctrlAdvVpParams g_dctrl_adv_vp_params;
static Dss_DctrlGlobalDssParams g_dctrl_global_dss_params;

static Fvid2_Frame g_dss_frm[2];
static uint32_t g_dss_frm_status[2];
static uint32_t g_dss_last_frm_idx;

static void appDssTestQueueFn(void *arg0, void *arg1)
{
    Fvid2_FrameList frmList;
    Fvid2_Frame *frm;
    int32_t retVal;

    while(1) {
        retVal = FVID2_SOK;
        frm = NULL;

        TaskP_sleep(100);

        SemaphoreP_pend(g_lock_sem, SemaphoreP_WAIT_FOREVER);
        if(g_dss_frm_status[(g_dss_last_frm_idx + 1) % 2] == FREE) {
            frm = &g_dss_frm[(g_dss_last_frm_idx + 1) % 2];
            g_dss_frm_status[(g_dss_last_frm_idx + 1) % 2] = BUSY;
            g_dss_last_frm_idx++;
            g_dss_last_frm_idx %= 2;
        }
        SemaphoreP_post(g_lock_sem);

        if(frm != NULL) {
            Fvid2FrameList_init(&frmList);
            frmList.numFrames  = 1U;
            frmList.frames[0U] = frm;

            retVal = Fvid2_queue(g_pipe_handle, &frmList, 0U);
            if(retVal != FVID2_SOK)
                App_printf("%s: Display Queue 2 Failed!!!\n", __func__);
        }

        while(retVal != FVID2_SOK) {
        }
    }
}

static void appDssTestBufferReturnFn(void *arg0, void *arg1)
{
    int32_t retVal = FVID2_SOK;
    Fvid2_FrameList frmList;

    while(1) {
        retVal = FVID2_SOK;

        if(retVal == FVID2_SOK) {

            /* Wait for callback from DSS ISR */
            SemaphoreP_pend(g_pipe_syncSem, SemaphoreP_WAIT_FOREVER);

            /* Try to dequeque a used frame */
            retVal = Fvid2_dequeue(g_pipe_handle, &frmList, 0U, FVID2_TIMEOUT_NONE);
            if(FVID2_SOK != retVal) {
                /* NOt all callbacks indicate buffer-return, so should act accordingly */
                if(FVID2_EAGAIN == retVal && frmList.numFrames == 0)
                    retVal = FVID2_SOK;
                else
                    App_printf("%s: Display Queue Failed!!!\n", __func__);
            }

        }

        if(retVal == FVID2_SOK && frmList.numFrames != 0) {
            SemaphoreP_pend(g_lock_sem, SemaphoreP_WAIT_FOREVER);
            if(frmList.frames[0] == &g_dss_frm[0])
                g_dss_frm_status[0] = FREE;
            else
                g_dss_frm_status[1] = FREE;
            SemaphoreP_post(g_lock_sem);
        }

        /* assert success */
        while(retVal != FVID2_SOK) {
        }
    }
}


static int32_t appDssTestCbFn(Fvid2_Handle handle, void *appData)
{
    int32_t retVal  = FVID2_SOK;

    /* Callback from ISR: notify task */
    SemaphoreP_post(g_pipe_syncSem);

    return (retVal);
}

int32_t appDssRun()
{
    int32_t retVal = FVID2_SOK;

    uint32_t cnt;
    Dss_DispCreateParams createParams;
    Dss_DispCreateStatus createStatus;
    Dss_DispParams disp_Params;
    Dss_DispPipeMflagParams mflagParams;
    Dss_DispParams *dispParams = &disp_Params;
    Fvid2_CbParams cbParams;
    Fvid2_FrameList frmList;
    SemaphoreP_Params semParams;
    TaskP_Params tsk_params;

    if(retVal == FVID2_SOK) {
        SemaphoreP_Params_init(&semParams);
        semParams.mode = SemaphoreP_Mode_BINARY;
        g_lock_sem = SemaphoreP_create(1U, &semParams);

        Dss_dispCreateParamsInit(&createParams);
        Fvid2CbParams_init(&cbParams);

        cbParams.cbFxn = &appDssTestCbFn;
        cbParams.appData = NULL;

        /* Start using a pipe from R5: this simulates local pipe usage */
        g_pipe_handle = Fvid2_create(
                DSS_DISP_DRV_ID,
                DSS_DISP_INST_VID2,
                &createParams,
                &createStatus,
                &cbParams);
        if((NULL == g_pipe_handle) ||
                (createStatus.retVal != FVID2_SOK))
        {
            App_printf("%s: Display Create Failed!!!\n", __func__);
            retVal = FVID2_EFAIL;
        }
    }

    if(retVal == FVID2_SOK) {
        Dss_dispParamsInit(dispParams);

        /* Configure the locally usable pipe ...*/
        dispParams->pipeCfg.pipeType = CSL_DSS_VID_PIPE_TYPE_VID;
        dispParams->pipeCfg.inFmt.width = 480;
        dispParams->pipeCfg.inFmt.height = 360;
        dispParams->pipeCfg.inFmt.pitch[0] = 480 * 4;
        for(cnt = 1; cnt < FVID2_MAX_PLANES; cnt++)
        {
            dispParams->pipeCfg.inFmt.pitch[cnt] = 0;
        }
        dispParams->pipeCfg.inFmt.dataFormat =
            FVID2_DF_BGRA32_8888;
        dispParams->pipeCfg.inFmt.scanFormat =
            FVID2_SF_PROGRESSIVE;
        dispParams->pipeCfg.outWidth = 480;
        dispParams->pipeCfg.outHeight = 360;
        dispParams->pipeCfg.scEnable = FALSE;
        dispParams->alphaCfg.globalAlpha =
            0xff;
        dispParams->alphaCfg.preMultiplyAlpha =
            FALSE;
        dispParams->layerPos.startX = 1440;
        dispParams->layerPos.startY = 720;

        retVal = Fvid2_control(
                g_pipe_handle,
                IOCTL_DSS_DISP_SET_DSS_PARAMS,
                dispParams,
                NULL);
        if(retVal != FVID2_SOK)
            App_printf("%s: DSS Set Params IOCTL Failed!!!\n", __func__);
    }

    if(retVal == FVID2_SOK) {
        Dss_dispPipeMflagParamsInit(&mflagParams);

        retVal = Fvid2_control(
                g_pipe_handle,
                IOCTL_DSS_DISP_SET_PIPE_MFLAG_PARAMS,
                &mflagParams,
                NULL);
        if(retVal != FVID2_SOK)
            App_printf("%s: DSS Set Params IOCTL Failed!!!\n", __func__);
    }

    if(retVal == FVID2_SOK) {
        Fvid2Frame_init(&g_dss_frm[0]);
        g_dss_frm[0].addr[0U] = (uint64_t)(&gDispArray1[0]);
        g_dss_frm[0].fid = FVID2_FID_FRAME;
        g_dss_frm[0].appData = NULL;
        g_dss_frm_status[0] = FREE;

        Fvid2Frame_init(&g_dss_frm[1]);
        g_dss_frm[1].addr[0U] = (uint64_t)(&gDispArray2[0]);
        g_dss_frm[1].fid = FVID2_FID_FRAME;
        g_dss_frm[1].appData = NULL;
        g_dss_frm_status[1] = FREE;

        Fvid2FrameList_init(&frmList);
        frmList.numFrames  = 1U;
        frmList.frames[0U] = &g_dss_frm[0];

        /* Queue frames ... */
        retVal = Fvid2_queue(g_pipe_handle, &frmList, 0U);
        if(retVal != FVID2_SOK)
            App_printf("%: Display Queue 1 Failed!!!\n", __func__);
    }

    if(retVal == FVID2_SOK) {
        g_dss_frm_status[0] = BUSY;
        g_dss_last_frm_idx = 0;

        SemaphoreP_Params_init(&semParams);
        semParams.mode = SemaphoreP_Mode_BINARY;
        g_pipe_syncSem = SemaphoreP_create(0U, &semParams);

        TaskP_Params_init(&tsk_params);
        tsk_params.priority = 3;
        tsk_params.stack = appDssTestQueueTaskStack;
        tsk_params.stacksize = APP_DSS_TEST_QUEUE_TASK_STACK_SIZE;
        TaskP_create(appDssTestQueueFn, &tsk_params);

        TaskP_Params_init(&tsk_params);
        tsk_params.priority = 3;
        tsk_params.stack = appDssTestBufferReturnTaskStack;
        tsk_params.stacksize = APP_DSS_TEST_BUFFER_RETURN_TASK_STACK_SIZE;
        TaskP_create(appDssTestBufferReturnFn, &tsk_params);

        /* And start pipe driver */
        retVal = Fvid2_start(g_pipe_handle, NULL);
        if(retVal != FVID2_SOK)
            App_printf("%s: Display Start Failed!!!\n", __func__);
    }

    return retVal;
}

int32_t appDssInit(void)
{
    Dss_InitParams initPrmsDss;
    Dss_RmInfo *rmInfo;

    Dss_initParamsInit(&initPrmsDss);

    rmInfo = &initPrmsDss.socParams.rmInfo;

    /*
     * Assign / restrict resources to / from R5 DSS driver.
     * Currently assigning
     * - COMMON_M
     * - VID1, VIDL2 for remote usage
     * - VID2 for local usage
     * - OVR2 + VP2
     */
    rmInfo->isCommRegAvailable[CSL_DSS_COMM_REG_ID_0] = TRUE;
    rmInfo->isCommRegAvailable[CSL_DSS_COMM_REG_ID_1] = FALSE;
    rmInfo->isCommRegAvailable[CSL_DSS_COMM_REG_ID_2] = FALSE;
    rmInfo->isCommRegAvailable[CSL_DSS_COMM_REG_ID_3] = FALSE;

    rmInfo->isPipeAvailable[CSL_DSS_VID_PIPE_ID_VID1]  = TRUE;
    rmInfo->isPipeAvailable[CSL_DSS_VID_PIPE_ID_VIDL1] = FALSE;
    rmInfo->isPipeAvailable[CSL_DSS_VID_PIPE_ID_VID2]  = TRUE;
    rmInfo->isPipeAvailable[CSL_DSS_VID_PIPE_ID_VIDL2] = TRUE;

    rmInfo->isOverlayAvailable[CSL_DSS_OVERLAY_ID_1] = FALSE;
    rmInfo->isOverlayAvailable[CSL_DSS_OVERLAY_ID_2] = TRUE;
    rmInfo->isOverlayAvailable[CSL_DSS_OVERLAY_ID_3] = FALSE;
    rmInfo->isOverlayAvailable[CSL_DSS_OVERLAY_ID_4] = FALSE;

    rmInfo->isPortAvailable[CSL_DSS_VP_ID_1] = FALSE;
    rmInfo->isPortAvailable[CSL_DSS_VP_ID_2] = TRUE;
    rmInfo->isPortAvailable[CSL_DSS_VP_ID_3] = FALSE;
    rmInfo->isPortAvailable[CSL_DSS_VP_ID_4] = FALSE;

    initPrmsDss.socParams.dpInitParams.isAvailable = FALSE;
    initPrmsDss.socParams.dpInitParams.isHpdSupported = FALSE;

    Dss_init(&initPrmsDss);

    return 0;
}

int32_t appDctrlInit()
{
    int32_t retVal = FVID2_SOK;
    uint32_t cnt;
    Dss_DctrlVpParams *vpPrm = &g_dctrl_vp_params;
    Dss_DctrlPathInfo *pathInfo = &g_dctrl_path_info;
    Dss_DctrlOverlayParams *ovlPrm = &g_dctrl_overlay_params;
    Dss_DctrlOverlayLayerParams *layerPrm = &g_dctrl_layer_params;
    Dss_DctrlAdvVpParams *advVpParams = &g_dctrl_adv_vp_params;
    Dss_DctrlGlobalDssParams *globalDssParams = &g_dctrl_global_dss_params;

    /* Use VP2@1280x800 */
    uint32_t csl_vpId = CSL_DSS_VP_ID_2;
    uint32_t dctrl_vpId = DSS_DCTRL_NODE_VP2;
    uint32_t csl_ovrId = CSL_DSS_OVERLAY_ID_2;
    uint32_t dctrl_ovrId = DSS_DCTRL_NODE_OVERLAY2;
    uint32_t dctrl_eptId = DSS_DCTRL_NODE_DPI_DPI0;
    /* Use one local pipe, VID2, and two remote pipes for linux, VID1 and VIDL2 */
    uint32_t num_vids = 3;
    uint32_t csl_vidId[] =   {CSL_DSS_VID_PIPE_ID_VID1,    CSL_DSS_VID_PIPE_ID_VIDL2,   CSL_DSS_VID_PIPE_ID_VID2};
    uint32_t dctrl_vidId[] = {DSS_DCTRL_NODE_VID1,         DSS_DCTRL_NODE_VIDL2,        DSS_DCTRL_NODE_VID2};
    uint32_t csl_layer[] =   {CSL_DSS_OVERLAY_LAYER_NUM_1, CSL_DSS_OVERLAY_LAYER_NUM_0, CSL_DSS_OVERLAY_LAYER_NUM_2};

    if(retVal == FVID2_SOK) {
        Dss_dctrlVpParamsInit(vpPrm);
        Dss_dctrlOverlayParamsInit(ovlPrm);
        Dss_dctrlOverlayLayerParamsInit(layerPrm);
        Dss_dctrlAdvVpParamsInit(advVpParams);
        Dss_dctrlGlobalDssParamsInit(globalDssParams);
        Dss_dctrlPathInfoInit(pathInfo);

        /* Instantiate DCTRL */
        g_dctrl_handle = Fvid2_create(DSS_DCTRL_DRV_ID, DSS_DCTRL_INST_0, NULL, NULL, NULL);
        if(NULL == g_dctrl_handle)
        {
            App_printf("DSS: ERROR: Dctrl create failed!!!\n");
            retVal = FVID2_EFAIL;
        }
    }

    if(retVal == FVID2_SOK) {
        /* Configure DCTRL : modes / layers / paths etc */
        vpPrm->vpId = csl_vpId;

        vpPrm->lcdOpTimingCfg.mInfo.standard = FVID2_STD_1080P_60;
        vpPrm->lcdOpTimingCfg.dvoFormat = FVID2_DV_GENERIC_DISCSYNC;
        vpPrm->lcdOpTimingCfg.videoIfWidth = FVID2_VIFW_24BIT;

        vpPrm->lcdPolarityCfg.actVidPolarity = FVID2_POL_HIGH;
        vpPrm->lcdPolarityCfg.pixelClkPolarity = FVID2_EDGE_POL_FALLING;
        vpPrm->lcdPolarityCfg.hsPolarity = FVID2_POL_HIGH;
        vpPrm->lcdPolarityCfg.vsPolarity = FVID2_POL_HIGH;

        for(cnt = 0; cnt < num_vids; cnt++) {
            pathInfo->edgeInfo[pathInfo->numEdges].startNode = dctrl_vidId[cnt];
            pathInfo->edgeInfo[pathInfo->numEdges].endNode = dctrl_ovrId;
            pathInfo->numEdges++;
        }
        pathInfo->edgeInfo[pathInfo->numEdges].startNode = dctrl_ovrId;
        pathInfo->edgeInfo[pathInfo->numEdges].endNode = dctrl_vpId;
        pathInfo->numEdges++;
        pathInfo->edgeInfo[pathInfo->numEdges].startNode = dctrl_vpId;
        pathInfo->edgeInfo[pathInfo->numEdges].endNode = dctrl_eptId;
        pathInfo->numEdges++;

        ovlPrm->overlayId = csl_ovrId;
        ovlPrm->overlayCfg.backGroundColor = 0x000000;

        layerPrm->overlayId = csl_ovrId;
        for(cnt = 0; cnt < CSL_DSS_VID_PIPE_ID_MAX; cnt++) {
            layerPrm->pipeLayerNum[cnt] = CSL_DSS_OVERLAY_LAYER_INVALID;
        }
        for(cnt = 0; cnt < num_vids; cnt++) {
            layerPrm->pipeLayerNum[csl_vidId[cnt]] = csl_layer[cnt];
        }

        /* Set up DSS using DCTRL */
        retVal = Fvid2_control(g_dctrl_handle, IOCTL_DSS_DCTRL_SET_PATH, pathInfo, NULL);
        if(retVal != FVID2_SOK)
            App_printf("DSS: ERROR: dctrl set path failed !!!\n");
    }

    if(retVal == FVID2_SOK) {
        retVal = Fvid2_control(g_dctrl_handle, IOCTL_DSS_DCTRL_SET_VP_PARAMS, vpPrm, NULL);
        if(retVal != FVID2_SOK)
            App_printf("DSS: ERROR: Dctrl set VP params failed !!!\n");
    }

    if(retVal == FVID2_SOK) {
        retVal = Fvid2_control(g_dctrl_handle, IOCTL_DSS_DCTRL_SET_OVERLAY_PARAMS, ovlPrm, NULL);
        if(retVal != FVID2_SOK)
            App_printf("DSS: ERROR: Dctrl set overlay params failed !!!\n");
    }

    if(retVal == FVID2_SOK) {
        retVal = Fvid2_control(g_dctrl_handle, IOCTL_DSS_DCTRL_SET_LAYER_PARAMS, layerPrm, NULL);
        if(retVal != FVID2_SOK)
            App_printf("DSS: ERROR: Dctrl set layer params failed !!!\n");
    }

    if(retVal == FVID2_SOK) {
        advVpParams->vpId = CSL_DSS_VP_ID_2;
        retVal = Fvid2_control(g_dctrl_handle, IOCTL_DSS_DCTRL_SET_ADV_VP_PARAMS, advVpParams, NULL);
        if(retVal != FVID2_SOK)
            App_printf("DSS: ERROR: Dctrl set adv vp params failed !!!\n");
    }

    if(retVal == FVID2_SOK) {
        retVal = Fvid2_control(g_dctrl_handle, IOCTL_DSS_DCTRL_SET_GLOBAL_DSS_PARAMS, globalDssParams, NULL);
        if(retVal != FVID2_SOK)
            App_printf("DSS: ERROR: Dctrl set global dss params failed !!!\n");
    }
    return retVal;
}

void appConfigureHdmiLcd(void)
{
    Board_control(BOARD_CTRL_CMD_SET_HDMI_PD_HIGH, (void*) 0U);
}

void appConfigureDisplayPrcm(enum app_display_type display)
{
    int32_t status = CSL_PASS;

    SET_CLOCK_PARENT(TISCI_DEV_DSS0,
            TISCI_DEV_DSS0_DSS_INST0_DPI_1_IN_2X_CLK,
            TISCI_DEV_DSS0_DSS_INST0_DPI_1_IN_2X_CLK_PARENT_DPI0_EXT_CLKSEL_OUT0,
            status);
    SET_DEVICE_STATE(TISCI_DEV_DSS0,
            TISCI_MSG_VALUE_DEVICE_SW_STATE_ON,
            status);
    SET_CLOCK_STATE(TISCI_DEV_DSS0,
            TISCI_DEV_DSS0_DSS_FUNC_CLK,
            0,
            TISCI_MSG_VALUE_CLOCK_SW_STATE_REQ,
            status);

    SET_CLOCK_FREQ(TISCI_DEV_DSS0,
            TISCI_DEV_DSS0_DSS_INST0_DPI_1_IN_2X_CLK,
            148500000ULL,
            status);
    SET_CLOCK_STATE(TISCI_DEV_DSS0,
            TISCI_DEV_DSS0_DSS_INST0_DPI_1_IN_2X_CLK_PARENT_DPI0_EXT_CLKSEL_OUT0,
            0,
            TISCI_MSG_VALUE_CLOCK_SW_STATE_REQ,
            status);
    SET_CLOCK_STATE(TISCI_DEV_DSS0,
            TISCI_DEV_DSS0_DSS_INST0_DPI_1_IN_2X_CLK,
            0,
            TISCI_MSG_VALUE_CLOCK_SW_STATE_REQ,
            status);
}
