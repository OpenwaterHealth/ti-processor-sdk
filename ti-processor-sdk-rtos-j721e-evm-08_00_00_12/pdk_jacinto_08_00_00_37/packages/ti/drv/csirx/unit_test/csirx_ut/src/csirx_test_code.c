/*
 *  Copyright (c) Texas Instruments Incorporated 2019
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
 *  \file csirx_test_code.c
 *
 *  \brief Contains test for various test-cases for CSIRX FVID3 DRV.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <csirx_test.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
static int32_t App_init(CsirxTestTaskObj *taskObj);
static int32_t App_create(CsirxTestTaskObj *taskObj);
static int32_t App_csiTest(CsirxTestTaskObj *taskObj);
static int32_t App_delete(CsirxTestTaskObj *taskObj);
static int32_t App_frameCompletionCb(Fvid2_Handle handle,
                                     Ptr appData,
                                     Ptr reserved);
static int32_t App_allocAndQFrames(CsirxTestTaskObj *taskObj);
static int32_t App_captFreeFrames(CsirxTestTaskObj *taskObj);
#if defined (SIMULATOR)
static void App_sendFrame(CsirxTestTaskObj *taskObj);
#endif
static Bool App_continueCapture(CsirxTestTaskObj *taskObj);
extern int32_t App_sensorConfig(uint32_t chNum);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
extern struct Udma_DrvObj gUdmaDrvObj;

#if defined (SIMULATOR)
#ifdef __cplusplus
#pragma DATA_SECTION(".tb_argData")
#pragma DATA_ALIGN(128)
#else
#pragma DATA_SECTION(gSimTbArgs, ".tb_argData")
#pragma DATA_ALIGN(gSimTbArgs, 128)
#endif  /* #ifdef __cplusplus */
/* For VLAB:
 * Word0: is for start sending frame flag. '0' means hold or send otherwise
 * Word1: Image/frame format as per CSI protocol
 * Word2: Number of frames to send per channel
 */
uint32_t gSimTbArgs[10U];
#endif

/** \brief Log enable for CSIRX Unit Test  application */
extern uint32_t gAppTrace;
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
int32_t Csirx_captureTest(CsirxTestTaskObj *taskObj)
{
    int retVal = FVID2_SOK;
    uint32_t chIdx;
    CsirxInstObj *instObj;

    /* App Init */
    retVal += App_init(taskObj);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(gAppTrace,
                  GT_ERR,
                  APP_NAME ": [ERROR]App_init() FAILED!!!\r\n");
    }
    /* App create */
    if (FVID2_SOK == retVal)
    {
        retVal += App_create(taskObj);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(gAppTrace,
                      GT_ERR,
                      APP_NAME ": [ERROR]App_create() FAILED!!!\r\n");
        }
    }
    /* App CSI test function */
    if (FVID2_SOK == retVal)
    {
        retVal += App_csiTest(taskObj);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(gAppTrace,
                      GT_ERR,
                      APP_NAME ": [ERROR]App_csiTest() FAILED!!!\r\n");
        }
    }

    /* App CSI delete function */
    if (FVID2_SOK == retVal)
    {
        retVal += App_delete(taskObj);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(gAppTrace,
                      GT_ERR,
                      APP_NAME ": [ERROR]App_delete() FAILED!!!\r\n");
        }
    }
    /* print channel stats here */
    GT_0trace(gAppTrace, GT_INFO,
                  "================================================\r\n");
    GT_0trace(gAppTrace, GT_INFO,
                  " ::Frames per seconds information for channels::\r\n");
    GT_0trace(gAppTrace, GT_INFO,
                  " |Inst ID   |Channel ID|Avg FPS|Min FPS|Max FPS|\r\n");
    instObj = &taskObj->instObj;
    for (chIdx = 0U ; chIdx < instObj->instCfgInfo->numCh ; chIdx++)
    {
        GT_5trace(gAppTrace, GT_INFO,
                  " |%d         |%d         |%.2f   |%.2f   |%.2f   |\r\n",
          instObj->instCfgInfo->csiDrvInst,
          chIdx,
          instObj->chObj[chIdx].fps,
          instObj->chObj[chIdx].minFps,
          instObj->chObj[chIdx].maxFps);
    }
    GT_0trace(gAppTrace, GT_INFO,
                  "================================================\r\n");

    taskObj->testResult = retVal;

    return retVal;
}

static int32_t App_init(CsirxTestTaskObj *taskObj)
{
    int32_t retVal = FVID2_SOK;

    return retVal;
}

static int32_t App_create(CsirxTestTaskObj *taskObj)
{
    int32_t retVal = FVID2_SOK;
    SemaphoreP_Params semParams;
    Csirx_CreateParams createParams;
    uint32_t loopCnt;
    Csirx_DPhyCfg dphyCfg;
    Fvid2_TimeStampParams tsParams;

    /* prepare create paramters */
    createParams.numCh           = taskObj->instObj.instCfgInfo->numCh;
    createParams.frameDropBuf    = taskObj->instObj.instCfgInfo->frameDropBuf;
    createParams.frameDropBufLen = taskObj->instObj.instCfgInfo->frameDropBufLen;
    /* Copy init params to instance object */
    Fvid2Utils_memcpy(&createParams.instCfg,
                      taskObj->instObj.instCfgInfo->instCfg,
                      sizeof (Csirx_InstCfg));
    /* Copy channel data */
    for (loopCnt = 0U ; loopCnt < createParams.numCh ; loopCnt++)
    {
        Fvid2Utils_memcpy(&createParams.chCfg[loopCnt],
                          taskObj->instObj.chObj[loopCnt].chCfgInfo->chCfg,
                          sizeof (Csirx_ChCfg));
    }

    /* Register call-back */
    Fvid2CbParams_init(&taskObj->instObj.cbPrms);
    taskObj->instObj.cbPrms.cbFxn   = (Fvid2_CbFxn) &App_frameCompletionCb;
    taskObj->instObj.cbPrms.appData = taskObj;

    /* Fvid2_create() */
    taskObj->drvHandle = Fvid2_create(
                                    CSIRX_CAPT_DRV_ID,
                                    taskObj->instObj.instCfgInfo->csiDrvInst,
                                    &createParams,
                                    &taskObj->instObj.createStatus,
                                    &taskObj->instObj.cbPrms);

    if ((NULL == taskObj->drvHandle) ||
        (taskObj->instObj.createStatus.retVal != FVID2_SOK))
    {
        GT_0trace(gAppTrace, GT_ERR,
                  APP_NAME ": Capture Create Failed!!!\r\n");
        retVal = taskObj->instObj.createStatus.retVal;
    }
    if (retVal == FVID2_SOK)
    {
        /* Allocate instance semaphore */
        SemaphoreP_Params_init(&semParams);
        semParams.mode = SemaphoreP_Mode_BINARY;
        taskObj->instObj.lockSem = SemaphoreP_create(1U, &semParams);
        if (taskObj->instObj.lockSem  == NULL)
        {
            GT_0trace(
                gAppTrace, GT_ERR,
                "Instance semaphore create failed!!\r\n");
            retVal = FVID2_EALLOC;
        }
    }
    if (retVal == FVID2_SOK)
    {
        /* Set CSIRX D-PHY configuration parameters */
        Csirx_initDPhyCfg(&dphyCfg);
        dphyCfg.inst = CSIRX_INSTANCE_ID_0;
        retVal = Fvid2_control(taskObj->drvHandle,
                               IOCTL_CSIRX_SET_DPHY_CONFIG,
                               &dphyCfg,
                               NULL);
        if(FVID2_SOK != retVal)
        {
            GT_0trace(gAppTrace,
                      GT_ERR,
                      APP_NAME
                      ":Set D-PHY Configuration FAILED!!!\r\n");
        }
        else
        {
            GT_0trace(gAppTrace,
                      GT_INFO,
                      APP_NAME
                      ":Set D-PHY Configuration Successful!!!\r\n");
        }
    }
    if (retVal == FVID2_SOK)
    {
        tsParams.timeStampFxn = (Fvid2_TimeStampFxn)&TimerP_getTimeInUsecs;
        /* register time stamping function */
        retVal = Fvid2_control(taskObj->drvHandle,
                               FVID2_REGISTER_TIMESTAMP_FXN,
                               &tsParams,
                               NULL);
    }

    GT_0trace(gAppTrace, GT_INFO, APP_NAME ": CSIRX Capture created\r\n");
    return retVal;
}

static int32_t App_csiTest(CsirxTestTaskObj *taskObj)
{
    int32_t retVal = FVID2_SOK;
    volatile uint32_t lastFrameNo = 0U;
    uint32_t startTime, elapsedTime;

    /* Allocate and queue all available frames */
    retVal = App_allocAndQFrames(taskObj);

    startTime = App_getCurTimeInMsec();
    /* Sensor Config here */
    if (FVID2_SOK != App_sensorConfig(taskObj->instObj.instCfgInfo->numCh))
    {
        retVal = FVID2_EFAIL;
    }
    elapsedTime = App_getElapsedTimeInMsec(startTime);
    GT_1trace(gAppTrace,
              GT_INFO,
              APP_NAME ": Time taken to configure Sensor:%dms\r\n",
              elapsedTime);

    if (retVal == FVID2_SOK)
    {
        retVal    = Fvid2_start(taskObj->drvHandle, NULL);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(gAppTrace, GT_ERR,
                      APP_NAME ": Capture Start Failed!!!\r\n");
        }
    }

    GT_0trace(gAppTrace, GT_INFO, APP_NAME ": Send frame now...\r\n");
#if defined (SIMULATOR)
    /* Send first Frame here from TB*/
    App_sendFrame(taskObj);
#endif

    /* Wait for reception completion */
    while (App_continueCapture(taskObj))
    {
        /* Pend on semaphore until last frame is received */
        SemaphoreP_pend(taskObj->instObj.lockSem,
                        SemaphoreP_WAIT_FOREVER);
        if ((lastFrameNo == 0U) &&
            (taskObj->instObj.chObj[0U].captFrames))
        {
            lastFrameNo = taskObj->instObj.chObj[0U].captFrames;
            GT_0trace(gAppTrace, GT_INFO, APP_NAME ": Stream Detected!!!\r\n");
        }
        GT_1trace(gAppTrace, GT_INFO,
                  APP_NAME ": Frames Received: %d\r\n",
                  taskObj->instObj.chObj[0U].captFrames);
    }

    GT_1trace(gAppTrace, GT_INFO,
              APP_NAME ": Frames Received: %d\r\n",
              taskObj->instObj.chObj[0U].captFrames);
    GT_1trace(gAppTrace, GT_INFO,
              APP_NAME ": Frames Received with errors: %d\r\n",
              taskObj->instObj.chObj[0U].captErrFrames);
    retVal = Fvid2_stop(taskObj->drvHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(gAppTrace, GT_ERR,
                  APP_NAME ": Capture Stop Failed!!!\r\n");
        return retVal;
    }
    retVal = App_captFreeFrames(taskObj);
    if (retVal != FVID2_SOK)
    {
        GT_0trace(gAppTrace, GT_ERR,
                  APP_NAME ": Capture Stop Failed!!!\r\n");
    }
    if (retVal == FVID2_ENO_MORE_BUFFERS)
    {
        /* All buffer might be de-queued during stop,
           in this case no error shall be returned */
        retVal = FVID2_SOK;
    }

    return retVal;
}

static int32_t App_allocAndQFrames(CsirxTestTaskObj *taskObj)
{
    int32_t retVal = FVID2_SOK;
    uint32_t chIdx = 0U, frmIdx = 0U;
    static Fvid2_FrameList frmList;
    Fvid2_Frame  *pFrm;
    CsirxInstObj *instObj;
    CsirxChObj   *chObj;

    /* for every channel in a capture handle,
       allocate memory for and queue frames */
    Fvid2FrameList_init(&frmList);
    frmList.numFrames = 0U;
    instObj = &taskObj->instObj;
    for (chIdx = 0U; chIdx < instObj->instCfgInfo->numCh ; chIdx++)
    {
        chObj = &instObj->chObj[chIdx];
        for (frmIdx = 0U; frmIdx < chObj->chCfgInfo->numFrames ; frmIdx++)
        {
            /* assign frames memory */
            /* Only following fields are used in CSIRX DRV */
            pFrm = (Fvid2_Frame *)
                    &chObj->frames[frmIdx];
            chObj->frmAddr[frmIdx] = (uint64_t *)Utils_memAlloc(
                       chObj->chCfgInfo->heapId,
                       (chObj->chCfgInfo->chCfg->outFmt.pitch[0U] * \
                        chObj->chCfgInfo->chCfg->outFmt.height),
                       UDMA_CACHELINE_ALIGNMENT);
            pFrm->addr[0U] = (uint64_t)chObj->frmAddr[frmIdx];
            pFrm->chNum = chObj->chCfgInfo->chCfg->chId;
            pFrm->appData = taskObj;
            frmList.frames[frmList.numFrames] = pFrm;
            frmList.numFrames++;
        }
    }
    /* queue the frames in frmList
     * All allocated frames are queued here as an example.
     * In general at least 2 frames per stream/channel need to queued
     * before capture can be started.
     * Failing which, frame could be dropped.
     */
    /* last parameter, i.e. streamId is unused in CSIRX DRV */
    retVal = Fvid2_queue(taskObj->drvHandle, &frmList, 0U);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(gAppTrace,
                  GT_ERR,
                  APP_NAME ": Capture Queue Failed!!!\r\n");
    }

    return retVal;
}

#if defined (SIMULATOR)
static void App_sendFrame(CsirxTestTaskObj *taskObj)
{
    /* Write the image format here */
    /* Assuming all channels will have same data type for now */
    CSL_REG32_WR(&gSimTbArgs[1U],
                 taskObj->instObj.chObj[0U].chCfgInfo->chCfg->inCsiDataType);
    /* Write number of image to send here */
    CSL_REG32_WR(&gSimTbArgs[2U],
                 taskObj->instObj.chObj[0U].chCfgInfo->numCaptFrames);
    /* Ask TB to send the data */
    CSL_REG32_WR(&gSimTbArgs[0U], 1U);
}
#endif

static int32_t App_frameCompletionCb(Fvid2_Handle handle,
                                     Ptr appData,
                                     Ptr reserved)
{
    int32_t  retVal = FVID2_SOK;
    uint32_t frmIdx = 0U, curTs, diffTs;
    static Fvid2_FrameList frmList;
    Fvid2_Frame *pFrm;
    CsirxTestTaskObj *taskObj = (CsirxTestTaskObj *) appData;
    volatile uint32_t tempVar;
    CsirxChObj *chObj;

    GT_assert(gAppTrace, (appData != NULL));

    curTs = AppUtils_getCurTimeInMsec();
    Fvid2FrameList_init(&frmList);
    retVal = Fvid2_dequeue(
        taskObj->drvHandle,
        &frmList,
        0U,
        FVID2_TIMEOUT_NONE);
    if (FVID2_SOK == retVal)
    {
        for (frmIdx = 0; frmIdx < frmList.numFrames; frmIdx++)
        {
            pFrm = frmList.frames[frmIdx];
            chObj = &taskObj->instObj.chObj[pFrm->chNum];
            /* Update frame stats */
            if (chObj->firstTs == 0U)
            {
                chObj->firstTs = curTs;
            }
            else
            {
                if (chObj->captFrames > 2U)
                {
                    diffTs = curTs - chObj->firstTs;
                    chObj->fps = ((float)diffTs / chObj->captFrames - 1U);
                    if (chObj->minFps > chObj->fps)
                    {
                        chObj->minFps = chObj->fps;
                    }
                    if (chObj->maxFps < chObj->fps)
                    {
                        chObj->maxFps = chObj->fps;
                    }
                }
            }
            if (FVID2_FRAME_STATUS_COMPLETED != pFrm->status)
            {
                chObj->captErrFrames++;
            }
            else
            {
                chObj->captFrames++;
            }
        }

        /* Queue back de-queued frames,
           last param i.e. streamId is unused in DRV */
        retVal = Fvid2_queue(taskObj->drvHandle, &frmList, 0U);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(gAppTrace, GT_ERR,
                      APP_NAME ": Capture Queue Failed!!!\r\n");
        }
    }

    /* always return 'FVID2_SOK' */
    /* Post semaphore here for sending next Image */
    SemaphoreP_post(taskObj->instObj.lockSem);

    return FVID2_SOK;
}

static int32_t App_captFreeFrames(CsirxTestTaskObj *taskObj)
{
    int32_t retVal = FVID2_SOK;
    static Fvid2_FrameList frmList;
    uint32_t chIdx = 0U, frmIdx = 0U;
    CsirxInstObj *instObj;
    CsirxChObj *chObj;

    /* for every stream and channel in a capture handle */
    Fvid2FrameList_init(&frmList);

    /* Deq-queue any frames queued more than needed */
    retVal = Fvid2_dequeue(
                    taskObj->drvHandle,
                    &frmList,
                    0U,
                    FVID2_TIMEOUT_NONE);
    if (retVal == FVID2_ENO_MORE_BUFFERS)
    {
        retVal = FVID2_SOK;
    }
    if (retVal != FVID2_SOK)
    {
        GT_0trace(gAppTrace,
                  GT_ERR,
                  APP_NAME ": De-queue in shut down failed!!!\r\n");
    }
    /* Free up frame allocated memories here */
    /* for every channel in a capture handle, de-allocate frame memory */
    instObj = &taskObj->instObj;
    for (chIdx = 0U; chIdx < instObj->instCfgInfo->numCh ; chIdx++)
    {
        chObj = &instObj->chObj[chIdx];
        for (frmIdx = 0U; frmIdx < chObj->chCfgInfo->numFrames ; frmIdx++)
        {
            /* free frames memory */
            retVal += Utils_memFree(
               chObj->chCfgInfo->heapId,
               (chObj->frmAddr[frmIdx]),
               (chObj->chCfgInfo->chCfg->outFmt.pitch[0U] * \
                chObj->chCfgInfo->chCfg->outFmt.height));
        }
    }
    if (retVal != FVID2_SOK)
    {
        GT_0trace(gAppTrace,
                  GT_ERR,
                  APP_NAME ": Frame memory freeing up failed!!!\r\n");
    }

    return retVal;
}

static int32_t App_delete(CsirxTestTaskObj *taskObj)
{
    int32_t retVal = FVID2_SOK;
    static Fvid2_FrameList frmList;

    Fvid2FrameList_init(&frmList);
    /* Dequeue all the request from the driver */
    retVal = Fvid2_dequeue(
                    taskObj->drvHandle,
                    &frmList,
                    0U,
                    FVID2_TIMEOUT_NONE);

    if ((FVID2_SOK != retVal) && (FVID2_ENO_MORE_BUFFERS != retVal))
    {
        GT_0trace(gAppTrace, GT_ERR,
                  APP_NAME ": Capture De-queue Failed!!!\r\n");
    }
    retVal = Fvid2_delete(taskObj->drvHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(gAppTrace, GT_ERR,
                  APP_NAME ": FVID2 Delete Failed!!!\r\n");
    }
    else
    {
        taskObj->drvHandle = NULL;
    }

    if (FVID2_SOK == retVal)
    {
        /* Delete semaphore */
        SemaphoreP_delete(taskObj->instObj.lockSem);
        GT_0trace(gAppTrace, GT_INFO, APP_NAME ": Capture Driver deleted\r\n");
    }

    return retVal;
}

static Bool App_continueCapture(CsirxTestTaskObj *taskObj)
{
    Bool retVal = TRUE;
    uint32_t chIdx;
    CsirxInstObj *instObj;

    instObj = &taskObj->instObj;
    for (chIdx = 0U ;chIdx <  instObj->instCfgInfo->numCh ; chIdx++)
    {
        if (instObj->chObj[chIdx].captFrames <
            instObj->chObj[chIdx].chCfgInfo->numCaptFrames)
        {
            break;
        }
    }
    if (chIdx >= instObj->instCfgInfo->numCh)
    {
        retVal = FALSE;
    }

    return retVal;
}
