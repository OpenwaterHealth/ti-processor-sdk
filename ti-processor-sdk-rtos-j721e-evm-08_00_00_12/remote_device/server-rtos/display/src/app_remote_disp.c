/*
 *
 * Copyright (c) 2017 Texas Instruments Incorporated
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
#include <string.h>

#include <ti/osal/TaskP.h>
#include <ti/drv/dss/dss.h>

#include <protocol/rpmsg-kdrv-transport-display.h>
#include <server-rtos/include/app_log.h>
#include <server-rtos/include/app_queue.h>
#include <server-rtos/include/app_remote_device.h>
#include <server-rtos/include/app_remote_device_display.h>


#define APP_REMOTE_DISPLAY_MAX_BUFFERS          (32)
#define APP_REMOTE_DISPLAY_MAX_COMMITS          (8)
#define APP_REMOTE_DISPLAY_MAX_MESSAGES         (32)

#define APP_REMOTE_DISPLAY_MAX_PACKET_SIZE      (512)

#define APP_REMOTE_DISPLAY_MAX_DEQUEUES         (4)

#define APP_REMOTE_DISPLAY_DISP_PREFIX          (0x10)
#define APP_REMOTE_DISPLAY_PIPE_PREFIX          (0x20)

#define g_sender_tsk_stack_size                 (0x2000)

#define APP_REMOTE_DISPLAY_ASSERT_SUCCESS(x)  { if((x)!=0) while(1); }
#define DEVHDR_2_MSG(x) ((void *)(((struct rpmsg_kdrv_device_header *)(x)) + 1))

struct __app_remote_disp_pipe_data_t;
typedef struct __app_remote_disp_pipe_data_t app_remote_disp_pipe_data_t;
struct __app_remote_disp_disp_data_t;
typedef struct __app_remote_disp_disp_data_t app_remote_disp_disp_data_t;
struct __app_remote_disp_inst_data_t;
typedef struct __app_remote_disp_inst_data_t app_remote_disp_inst_data_t;
struct __app_remote_disp_data_t;
typedef struct __app_remote_disp_data_t app_remote_disp_data_t;

struct __app_remote_disp_rtparams_t;
typedef struct __app_remote_disp_rtparams_t app_remote_disp_rtparams_t;
struct __app_remote_disp_commit_t;
typedef struct __app_remote_disp_commit_t app_remote_disp_commit_t;
struct __app_remote_disp_buffer_t;
typedef struct __app_remote_disp_buffer_t app_remote_disp_buffer_t;

struct __app_remote_disp_message_t;
typedef struct __app_remote_disp_message_t app_remote_disp_message_t;

struct __app_remote_disp_disp_data_t {
    app_remote_disp_disp_prm_t disp_prm;
    uint32_t pipe_count;
    app_remote_disp_pipe_data_t *pipes[APP_REMOTE_DISPLAY_MAX_PIPES];
    app_remote_disp_inst_data_t *inst;
    uint8_t tp_id;
    uint32_t serial;
};

struct __app_remote_disp_pipe_data_t {
    app_remote_disp_pipe_prm_t pipe_prm;
    app_remote_disp_disp_data_t *disp;
    app_remote_disp_inst_data_t *inst;
    uint32_t serial;
    uint8_t tp_id;
    Fvid2_Handle fvid2_hndl;
    uint32_t is_pipe_running;
    uint32_t is_pipe_stopping;
    app_remote_disp_commit_t *stop_commit;
};

struct __app_remote_disp_inst_data_t {
    app_remote_disp_inst_prm_t inst_prm;
    app_remote_disp_disp_data_t disp_data[CSL_DSS_VP_ID_MAX];
    uint32_t disp_count;
    app_remote_disp_pipe_data_t pipe_data[CSL_DSS_VID_PIPE_ID_MAX];
    uint32_t pipe_count;
    uint32_t device_id;
    uint32_t serial;
    app_remote_device_channel_t *channel;
};

struct __app_remote_disp_data_t {
    app_remote_disp_init_prm_t prm;
    uint32_t inst_count;
    app_remote_disp_inst_data_t inst_data[APP_REMOTE_DISPLAY_MAX_INSTANCES];
    app_queue_t send_queue;
    app_queue_t commit_pool;
    app_queue_t buffer_pool;
    app_queue_t message_pool;
    SemaphoreP_Handle send_sem;
    SemaphoreP_Handle lock_sem;
    TaskP_Handle sender_task;
};

struct __app_remote_disp_rtparams_t {
    Fvid2_PosConfig pos_prm;
    Dss_FrameRtParams in_frm_prm;
    Dss_FrameRtParams out_frm_prm;
    Dss_ScRtParams sc_prm;
    Dss_DispRtParams rt_prm;
};

struct __app_remote_disp_commit_t {
    uint32_t commit_id;
    uint32_t num_upds;
    uint32_t upds_done;
    void *msg;
};

struct __app_remote_disp_buffer_t {
    app_remote_disp_commit_t *commit;
    app_remote_disp_rtparams_t rt_prm;
    Fvid2_Frame frame;
    Dss_DispParams disp_prm;
    uint32_t buffer_id;
    void *msg;
};

struct __app_remote_disp_message_t {
    uint32_t request_id;
    uint32_t message_size;
    uint32_t device_id;
    uint32_t is_response;
    app_remote_device_channel_t *channel;
    uint8_t data[0];

};

/* stack for sender task */
static uint8_t g_sender_tsk_stack[g_sender_tsk_stack_size] __attribute__ ((section(".bss:taskStackSection"))) __attribute__ ((aligned(8192)));

/* Storage areas for pools */
static uint8_t g_commit_pool_storage[(sizeof(app_remote_disp_commit_t) + APP_QUEUE_ELEM_META_SIZE) * APP_REMOTE_DISPLAY_MAX_COMMITS];
static uint8_t g_buffer_pool_storage[(sizeof(app_remote_disp_buffer_t) + APP_QUEUE_ELEM_META_SIZE) * APP_REMOTE_DISPLAY_MAX_BUFFERS];
static uint8_t g_message_pool_storage[(APP_REMOTE_DISPLAY_MAX_PACKET_SIZE + sizeof(app_remote_disp_message_t)+ APP_QUEUE_ELEM_META_SIZE) * APP_REMOTE_DISPLAY_MAX_MESSAGES];

static app_remote_disp_data_t g_rdisp_data;

static int32_t appRemoteDisplayPipeFrmDoneCb(Fvid2_Handle handle, void *app_data);

/*
 * Helper Function to compare two instance
 * params and return the number of pipe_ids
 * common between them
 */
static uint32_t appRemoteDisplayInstNumCommonPipes(app_remote_disp_inst_prm_t *src,
        app_remote_disp_inst_prm_t* dst)
{
    uint32_t cnt, icnt;
    uint32_t count_match = 0;
    for(cnt = 0; cnt < src->num_pipes; cnt++) {
        for(icnt = 0; icnt < dst->num_pipes; icnt++) {
            if(src->pipe_prm[cnt].pipe_id == dst->pipe_prm[icnt].pipe_id)
                count_match++;
        }
    }

    return count_match;
}

/*
 * Helper Function to check if pipe supports
 * a format and return TRUE if supported, and
 * return FALSE otherwise.
 */
static uint32_t appRemoteDisplayPipeSupportsFormat(app_remote_disp_pipe_data_t *pipe, uint32_t format)
{
    uint32_t cnt;
    uint32_t ret = FALSE;

    for(cnt = 0; cnt < pipe->pipe_prm.num_formats; cnt++) {
        if(pipe->pipe_prm.formats[cnt] == format) {
            ret = TRUE;
            break;
        }
    }

    return ret;
}

/*
 * Helper Function to check if disp supports
 * a pipe and return TRUE if supported, and
 * return FALSE otherwise.
 */
static uint32_t appRemoteDisplayDispContainsPipeData(app_remote_disp_disp_data_t *disp, app_remote_disp_pipe_data_t *pipe)
{
    uint32_t cnt;
    uint32_t ret = FALSE;

    for(cnt = 0; cnt < disp->pipe_count; cnt++) {
        if(disp->pipes[cnt] == pipe) {
            ret = TRUE;
            break;
        }
    }

    return ret;
}

/*
 * Helper Function to compare all disp_ids
 * within an instance param and return TRUE
 * if duplicates are found, and return FALSE
 * otherwise.
 */
static uint32_t appRemoteDisplayInstHasDuplicateDisps(app_remote_disp_inst_prm_t *prm)
{
    uint32_t cnt, icnt;
    uint32_t ret = FALSE;

    for(cnt = 0; cnt < prm->num_disps; cnt++) {
        for(icnt = cnt + 1; icnt < prm->num_disps; icnt++) {
            if(prm->disp_prm[cnt].disp_id == prm->disp_prm[icnt].disp_id) {
                ret = TRUE;
                break;
            }
        }
        if(ret == TRUE)
            break;
    }

    return ret;
}

/*
 * Helper Function to compare all vid ids
 * within a commit request and return TRUE
 * if duplicates are found, and return FALSE
 * otherwise.
 */
static uint32_t appRemoteDisplayCommitHasDuplicatePipes(struct rpmsg_kdrv_display_commit_request *c)
{
    uint32_t cnt, icnt;
    uint32_t ret = FALSE;

    for(cnt = 0; cnt < c->num_vid_updates; cnt++) {
        for(icnt = cnt + 1; icnt < c->num_vid_updates; icnt++) {
            if(c->vid[cnt].id == c->vid[icnt].id) {
                ret = TRUE;
                break;
            }
        }
        if(ret == TRUE)
            break;
    }

    return ret;
}

/*
 * Helper Function to compare all pipe_ids
 * within an instance param and return TRUE
 * if duplicates are found, and return FALSE
 * otherwise.
 */
static uint32_t appRemoteDisplayInstHasDuplicatePipes(app_remote_disp_inst_prm_t *prm)
{
    uint32_t cnt, icnt;
    uint32_t ret = FALSE;

    for(cnt = 0; cnt < prm->num_pipes; cnt++) {
        for(icnt = cnt + 1; icnt < prm->num_pipes; icnt++) {
            if(prm->pipe_prm[cnt].pipe_id == prm->pipe_prm[icnt].pipe_id) {
                ret = TRUE;
                break;
            }
        }
        if(ret == TRUE)
            break;
    }

    return ret;
}

/*
 * Helper Function to validate if the pipe
 * is terminated to a display within this
 * instance. Return 0 if the termination is
 * correct, -1 otherwise
 */
static int32_t appRemoteDisplayInstValidatePipeTerm(app_remote_disp_inst_prm_t *prm)
{
    uint32_t cnt, icnt;
    uint32_t valid;
    int32_t ret = 0;

    for(cnt = 0; cnt < prm->num_pipes; cnt++) {
        valid = FALSE;
        for(icnt = 0; icnt < prm->num_disps; icnt++) {
            if(prm->pipe_prm[cnt].disp_id == prm->disp_prm[icnt].disp_id) {
                valid = TRUE;
                break;
            }
        }
        if(valid == FALSE) {
            ret = -1;
            break;
        }
    }

    return ret;
}

/*
 * Helper Function to compare all zorders
 * in a pipe init param and check for duplicates.
 * Return TRUE is duplicates are found, and return
 * FALSE otherwise
 */
static uint32_t appRemoteDisplayPipeHasDuplicateZOrders(app_remote_disp_pipe_prm_t *prm)
{
    uint32_t cnt, icnt;
    uint32_t ret = FALSE;
    for(cnt = 0; cnt < prm->num_zorders; cnt++) {
        for(icnt = cnt + 1; icnt < prm->num_zorders; icnt++) {
            if(prm->zorders[cnt] == prm->zorders[icnt]) {
                ret = TRUE;
                break;
            }
        }
        if(ret == TRUE)
            break;
    }

    return ret;
}

/*
 * Helper Function to validate the current zorder
 * and check whether it is one of the allowed zorders
 * Return 0 if the current zorder is valid, -1
 * otherwise
 */
static int32_t appRemoteDisplayPipeValidateCurrentZOrder(app_remote_disp_pipe_prm_t *prm)
{
    uint32_t cnt;
    int32_t ret = -1;

    for(cnt = 0; cnt < prm->num_zorders; cnt++) {
        if(prm->current_zorder == prm->zorders[cnt]) {
            ret = 0;
            break;
        }
    }

    return ret;

}

/*
 * Helper Function to compare all formats in a
 * pipe init param and check for duplicates
 * return TRUE if duplicates are found, and return
 * FALSE otherwise
 */
static uint32_t appRemoteDisplayPipeHasDuplicateFormats(app_remote_disp_pipe_prm_t *prm)
{
    uint32_t cnt, icnt;
    uint32_t ret = FALSE;

    for(cnt = 0; cnt < prm->num_formats; cnt++) {
        for(icnt = cnt + 1; icnt < prm->num_formats; icnt++) {
            if(prm->formats[cnt] == prm->formats[icnt]) {
                ret = TRUE;
                break;
            }
        }
    }

    return ret;
}

/*
 * Helper Function to check for any invalid formats
 * in the pipe init param
 * Return TRUE if found, FALSE otherwise
 */
static uint32_t appRemoteDisplayPipeHasInvalidFormats(app_remote_disp_pipe_prm_t *prm)
{
    uint32_t cnt;
    uint32_t ret = FALSE;

    for(cnt = 0; cnt < prm->num_formats; cnt++) {
        if(prm->formats[cnt] == FVID2_DF_INVALID)
            ret = TRUE;
    }

    return ret;
}

static app_remote_disp_disp_data_t *appRemoteDisplayInstFindDispId(app_remote_disp_inst_data_t *inst,
        uint32_t disp_id)
{
    uint32_t cnt;
    app_remote_disp_disp_data_t *disp = NULL;

    for(cnt = 0; cnt < inst->disp_count; cnt++) {
        if(inst->disp_data[cnt].disp_prm.disp_id == disp_id) {
            disp = &inst->disp_data[cnt];
            break;
        }
    }

    return disp;

}

static app_remote_disp_inst_data_t *appRemoteDisplayDataFindDeviceId(uint32_t device_id)
{
    uint32_t cnt;
    app_remote_disp_inst_data_t *inst = NULL;

    for(cnt = 0; cnt < g_rdisp_data.inst_count; cnt++) {
        if(g_rdisp_data.inst_data[cnt].device_id == device_id) {
            inst = &g_rdisp_data.inst_data[cnt];
            break;
        }
    }

    return inst;
}

static app_remote_disp_disp_data_t *appRemoteDisplayInstFindDispTpId(app_remote_disp_inst_data_t *inst,
        uint32_t disp_tp_id)
{
    uint32_t cnt;
    app_remote_disp_disp_data_t *disp = NULL;

    for(cnt = 0; cnt < inst->disp_count; cnt++) {
        if(inst->disp_data[cnt].tp_id == disp_tp_id) {
            disp = &inst->disp_data[cnt];
            break;
        }
    }

    return disp;

}

static app_remote_disp_pipe_data_t *appRemoteDisplayInstFindPipeTpId(app_remote_disp_inst_data_t *inst,
        uint32_t pipe_tp_id)
{
    uint32_t cnt;
    app_remote_disp_pipe_data_t *pipe = NULL;

    for(cnt = 0; cnt < inst->pipe_count; cnt++) {
        if(inst->pipe_data[cnt].tp_id == pipe_tp_id) {
            pipe = &inst->pipe_data[cnt];
            break;
        }
    }

    return pipe;

}

static uint32_t appRemoteDisplayToSystemDf(uint32_t tp_fmt)
{
    uint32_t fmt;

    switch(tp_fmt) {
        case RPMSG_KDRV_TP_DISPLAY_FORMAT_ARGB8888:
            fmt = FVID2_DF_BGRA32_8888;
            break;
        case RPMSG_KDRV_TP_DISPLAY_FORMAT_XRGB8888:
            fmt = FVID2_DF_BGRX32_8888;
            break;
        default:
            fmt = FVID2_DF_INVALID;
    }

    return fmt;
}

static uint32_t appRemoteDisplayToRemoteFmt(uint32_t df)
{
    uint32_t fmt;

    switch(df) {
        case FVID2_DF_BGRA32_8888:
            fmt = RPMSG_KDRV_TP_DISPLAY_FORMAT_ARGB8888;
            break;
        case FVID2_DF_BGRX32_8888:
            fmt = RPMSG_KDRV_TP_DISPLAY_FORMAT_XRGB8888;
            break;
        default:
            fmt = RPMSG_KDRV_TP_DISPLAY_FORMAT_MAX;
    }

    return fmt;
}

/*
 * Validation Function: validates display param
 * 1. Checks for NULL
 * TODO: check for further hardware caps / DCTRL configs
 */
static int32_t appRemoteDisplayValidateDispPrm(app_remote_disp_disp_prm_t *disp_prm, char *err_str, uint32_t err_len)
{
    int32_t ret = 0;

    if(disp_prm == NULL) {
        snprintf(err_str, err_len, "disp_prm = NULL not allowed");
        ret = -1;
    }

    /*
     * TODO:
     * check if this is a valid display CSL ID.
     * check whether DCTRL has configured the width / height / fps as per this prm
     */

    return ret;
}

/*
 * Validation Function: validates pipe param
 * 1. Checks for NULL
 * 2. Checks for limits
 * 3. Checks for duplicates
 * 4. Checks for current zorder against supplied zorders
 * 5. Checks for valid formats
 * 6. Checks for hardware caps
 * TODO: check for further hardware caps / DCTRL configs
 */
static int32_t appRemoteDisplayValidatePipePrm(app_remote_disp_pipe_prm_t *pipe_prm, char *err_str, uint32_t err_len)
{
    int32_t ret = 0;

    if(pipe_prm == NULL) {
        snprintf(err_str, err_len, "pipe_prm = NULL not allowed");
        ret = -1;
    }

    if(ret == 0) {
        if(pipe_prm->num_zorders > APP_REMOTE_DISPLAY_MAX_ZORDERS) {
            snprintf(err_str, err_len, "num zorders > %u not allowed", APP_REMOTE_DISPLAY_MAX_ZORDERS);
            ret = -1;
        }
    }

    if(ret == 0) {
        if(pipe_prm->num_formats > APP_REMOTE_DISPLAY_MAX_FORMATS) {
            snprintf(err_str, err_len, "num formats > %u not allowed", APP_REMOTE_DISPLAY_MAX_FORMATS);
            ret = -1;
        }
    }

    if(ret == 0) {
        if(appRemoteDisplayPipeHasDuplicateZOrders(pipe_prm) == TRUE) {
            snprintf(err_str, err_len, "duplicate zorders within a pipe not allowed");
            ret = -1;
        }
    }

    if(ret == 0) {
        if(appRemoteDisplayPipeHasDuplicateFormats(pipe_prm) == TRUE) {
            snprintf(err_str, err_len, "duplicate formats within a pipe not allowed");
            ret = -1;
        }
    }

    if(ret == 0) {
        ret = appRemoteDisplayPipeValidateCurrentZOrder(pipe_prm);
        if(ret != 0)
            snprintf(err_str, err_len, "duplicate zorders within a pipe not allowed");
    }

    if(ret == 0) {
        ret = appRemoteDisplayPipeHasInvalidFormats(pipe_prm);
        if(ret != 0)
            snprintf(err_str, err_len, "invalid formats in param not allowed");
    }

    if(ret == 0) {
        if(pipe_prm->scale_cap == TRUE && Dss_dispIsVidLInst(pipe_prm->pipe_id)) {
            snprintf(err_str, err_len, "Could not init vidl pipe as scale capable");
            ret = -1;
        }
    }

    /*
     * TODO:
     * check if this is a valid display DSS_DRV ID.
     * check whether DCTRL has configured this pipe to the disp as per this prm
     * check whether DCTRL has configured this pipe to the layer as per this prm
     * check whether DCTRL has configured this pipe window to pos / dim as per this prm
     * if win_mod_cap == TRUE
     * check if the supplied zorders are valid for SOC.
     * Check whether the supplied zorders can be applied with current DCTRL config
     * check whether the pipe supports all the formats
     */

    return ret;
}

/*
 * Validation Function: validates instance param
 * 1. Checks for NULL
 * 2. Checks for limits
 * 3. Checks whether all pipes / disps are unique
 * 4. Checks whether all pipe / disp combos are valid
 */
static int32_t appRemoteDisplayValidateInstPrm(app_remote_disp_inst_prm_t *inst_prm, char *err_str, uint32_t err_len)
{
    int32_t ret = 0;

    if(inst_prm == NULL) {
        snprintf(err_str, err_len, "inst_prm = NULL not allowed");
        ret = -1;
    }

    if(ret == 0) {
        if(inst_prm->num_disps >= APP_REMOTE_DISPLAY_MAX_DISPS) {
            snprintf(err_str, err_len, "num displays > %u not allowed", APP_REMOTE_DISPLAY_MAX_DISPS);
            ret = -1;
        }
    }

    if(ret == 0) {
        if(inst_prm->num_pipes >= APP_REMOTE_DISPLAY_MAX_PIPES) {
            snprintf(err_str, err_len, "num pipes > %u not allowed", APP_REMOTE_DISPLAY_MAX_PIPES);
            ret = -1;
        }
    }

    if(ret == 0) {
        if(appRemoteDisplayInstHasDuplicatePipes(inst_prm) == TRUE) {
            snprintf(err_str, err_len, "duplicate pipes within an instance not allowed");
            ret = -1;
        }
    }

    if(ret == 0) {
        if(appRemoteDisplayInstHasDuplicateDisps(inst_prm) == TRUE) {
            snprintf(err_str, err_len, "duplicate disps within an instance not allowed");
            ret = -1;
        }
    }

    if(ret == 0) {
        ret = appRemoteDisplayInstValidatePipeTerm(inst_prm);
        if(ret != 0)
            snprintf(err_str, err_len, "incorrect pipe -> disp connection in param");
    }

    return ret;

}

/*
 * Validation Function: validates init param
 * 1. Checks for NULL
 * 2. Checks for limits
 * 3. Checks whether any pipe resources are
 *    duplicated across instances
 */
static int32_t appRemoteDisplayValidateInitPrm(app_remote_disp_init_prm_t *prm, char *err_str, uint32_t err_len)
{
    app_remote_disp_inst_prm_t *sinst, *dinst;
    uint32_t cnt, icnt;
    int32_t ret = 0;

    if(prm == NULL) {
        snprintf(err_str, err_len, "prm = NULL not allowed");
        ret = -1;
    }

    if(ret == 0) {
        if(prm->rpmsg_buf_size > APP_REMOTE_DISPLAY_MAX_PACKET_SIZE) {
            snprintf(err_str, err_len, "rpmsg_buf_size > %u not allowed", APP_REMOTE_DISPLAY_MAX_PACKET_SIZE);
            ret = -1;
        }
    }

    if(ret == 0) {
        if(prm->num_instances > APP_REMOTE_DISPLAY_MAX_INSTANCES) {
            snprintf(err_str, err_len, "num instances > %u not allowed", APP_REMOTE_DISPLAY_MAX_INSTANCES);
            ret = -1;
        }
    }

    if(ret == 0) {
        for(cnt = 0; cnt < prm->num_instances; cnt++) {
            sinst = &prm->inst_prm[cnt];
            for(icnt = cnt + 1; icnt < prm->num_instances; icnt++) {
                dinst = &prm->inst_prm[icnt];
                ret = appRemoteDisplayInstNumCommonPipes(sinst, dinst);
                if(ret != 0) {
                    snprintf(err_str, err_len, "common pipes in multiple instances not allowed");
                    break;
                }
            }
            if(ret != 0)
                break;
        }
    }

    return ret;
}

static int32_t appRemoteDisplayDisconnect(uint32_t device_id)
{
    app_remote_disp_inst_data_t *inst;
    int32_t ret = 0;

    SemaphoreP_pend(g_rdisp_data.lock_sem, SemaphoreP_WAIT_FOREVER);

    inst = appRemoteDisplayDataFindDeviceId(device_id);
    if(inst == NULL) {
        appLogPrintf("%s: Could not find a instance\n", __func__);
        ret = -1;
    }

    if(ret == 0) {
        /*
         * Disconnect the display from the existing channel.
         */
        inst->channel = NULL;
    }

    SemaphoreP_post(g_rdisp_data.lock_sem);
    return ret;
}

static int32_t appRemoteDisplayConnect(uint32_t device_id, app_remote_device_channel_t *channel)
{
    app_remote_disp_inst_data_t *inst;
    int32_t ret = 0;

    SemaphoreP_pend(g_rdisp_data.lock_sem, SemaphoreP_WAIT_FOREVER);

    inst = appRemoteDisplayDataFindDeviceId(device_id);
    if(inst == NULL) {
        appLogPrintf("%s: Could not find a instance\n", __func__);
        ret = -1;
    }

    if(ret == 0) {
        /*
         * Connect the display to one remote-procId + remote-endpt.
         * All future messages from this channel will be entertained
         */
        inst->channel = channel;
    }

    SemaphoreP_post(g_rdisp_data.lock_sem);
    return ret;
}

static uint32_t appRemoteDisplayValidateBuffer(struct rpmsg_kdrv_display_buffer_info *buffer,
        char *err_str, uint32_t err_len)
{
    /* TODO Lots */
    return 0;
}

static int32_t appRemoteDisplayValidatePipeUpdate(app_remote_disp_inst_data_t *inst,
        struct rpmsg_kdrv_display_commit_request *c, uint32_t index,
        app_remote_disp_pipe_data_t *pipe, char *err_str, uint32_t err_len)
{
    int32_t ret = 0;
    struct rpmsg_kdrv_display_vid_update_info *vid;
    app_remote_disp_disp_data_t *disp;
    uint32_t format;

    if(pipe == NULL) {
        snprintf(err_str, err_len, "pipe is NULL");
        ret = -1;
    }

    if(ret == 0) {
        if(c == NULL) {
            snprintf(err_str, err_len, "commit is NULL");
            ret = -1;
        }
    }

    if(ret == 0) {
        if(index >= c->num_vid_updates) {
            snprintf(err_str, err_len, "pipe index exceeps number of requested updates");
            ret = -1;
        }
    }

    if(ret == 0) {
        vid = &c->vid[index];

        if(pipe->pipe_prm.win_mod_cap == 0) {
            /* Is pipe configured as fixed window? Then params in request should match init params */
            if(pipe->pipe_prm.pos_x != vid->dst_x ||
                    pipe->pipe_prm.pos_x != vid->dst_y ||
                    pipe->pipe_prm.width != vid->dst_w ||
                    pipe->pipe_prm.height != vid->dst_h) {
                snprintf(err_str, err_len, "pipe has fixed window but window params mismatch");
                ret = -1;
            }
        } else {
            disp = pipe->disp;

            /* window should be within display resolution bounds */
            if((int32_t)vid->dst_x < 0 ||
                    (int32_t)vid->dst_y < 0 ||
                    vid->dst_x + vid->dst_w > disp->disp_prm.width ||
                    vid->dst_y + vid->dst_h > disp->disp_prm.height) {
                snprintf(err_str, err_len, "requested window (%u,%u-%ux%u) exceeds display dimensions (%ux%u)",
                        vid->dst_x, vid->dst_y, vid->dst_w, vid->dst_h, disp->disp_prm.width, disp->disp_prm.height);
                ret = -1;
            }
        }
    }

    if(ret == 0) {
        if(pipe->pipe_prm.scale_cap == 0) {
            /* Is pipe configured as no-scale? Then params should not indicate scaling */
            if(vid->buffer.width != vid->dst_w ||
                    vid->buffer.height != vid->dst_h) {
                snprintf(err_str, err_len, "pipe cannot scale but scaling indicated");
                ret = -1;
            }
        }
    }


    if(ret == 0) {
        format = appRemoteDisplayToSystemDf(vid->buffer.format);
        if(FVID2_DF_INVALID == format) {
            snprintf(err_str, err_len, "invalid format %u", vid->buffer.format);
            ret = -1;
        }
    }

    if(ret == 0) {
        /* Is pipe configured to support this format? */
        if(FALSE == appRemoteDisplayPipeSupportsFormat(pipe, format)) {
            snprintf(err_str, err_len, "pipe %u does not support format %u", pipe->tp_id, vid->buffer.format);
            ret = -1;
        }
    }

    if(ret == 0) {
        /* validate the different buffer parameters */
        ret = appRemoteDisplayValidateBuffer(&vid->buffer, err_str, err_len);
    }

    return ret;
}

static int32_t appRemoteDisplayValidatePipeDisable(app_remote_disp_inst_data_t *inst,
        app_remote_disp_pipe_data_t *pipe, char *err_str, uint32_t err_len)
{
    int32_t ret = 0;

    if(pipe == NULL) {
        snprintf(err_str, err_len, "pipe is NULL");
        ret = -1;
    }

    /*TODO: HwiP_disable ??? */
    if(ret == 0) {
        /* Cant stop a pipe if it is not started already */
        if(pipe->is_pipe_running == 0) {
            snprintf(err_str, err_len, "pipe stop requested but pipe not running");
            ret = -1;
        }
    }

    return ret;
}

static int32_t appRemoteDisplayValidateCommit(app_remote_disp_inst_data_t *inst,
        struct rpmsg_kdrv_display_commit_request *c, char *err_str, uint32_t err_len)
{
    int32_t ret = 0;
    uint32_t cnt;
    app_remote_disp_disp_data_t *disp_data;
    app_remote_disp_pipe_data_t *pipe_data;

    /* is valid display? */
    disp_data = appRemoteDisplayInstFindDispTpId(inst, c->id);
    if(disp_data == NULL) {
        snprintf(err_str, err_len, "disp_id = %u not found", c->id);
        ret = -1;
    }

    if(ret == 0) {
        /* Does the display support the supplied number of pipes? */
        if(c->num_vid_updates > disp_data->pipe_count) {
            snprintf(err_str, err_len, "number of pipe update requests %u exceeds display pipe count %u",
                    c->num_vid_updates, disp_data->pipe_count);
            ret = -1;
        }
    }

    if(ret == 0) {
        /* Are any two vid updates on teh same pipe? */
        if(TRUE == appRemoteDisplayCommitHasDuplicatePipes(c)) {
            snprintf(err_str, err_len, "commit has duplicate pipe update requests");
            ret = -1;
        }
    }

    for(cnt = 0; ret == 0 && cnt < c->num_vid_updates; cnt++) {
        if(ret == 0) {
            /* Is valid pipe? */
            pipe_data = appRemoteDisplayInstFindPipeTpId(inst, c->vid[cnt].id);
            if(pipe_data == NULL) {
                snprintf(err_str, err_len, "invalid pipe id %u", c->vid[cnt].id);
                ret = -1;
            }
        }

        if(ret == 0) {
            /* Is pipe configured to display? */
            if(FALSE == appRemoteDisplayDispContainsPipeData(disp_data, pipe_data)) {
                snprintf(err_str, err_len, "disp %u does not support pipe %u", disp_data->tp_id, pipe_data->tp_id);
                ret = -1;
            }
        }

        if(ret == 0) {
            if(c->vid[cnt].enabled == 0)
                ret = appRemoteDisplayValidatePipeDisable(inst, pipe_data, err_str, err_len);
            else
                ret = appRemoteDisplayValidatePipeUpdate(inst, c, cnt, pipe_data, err_str, err_len);
        }
    }

    return ret;
}

static int32_t appRemoteDisplayGetPipeData(app_remote_disp_inst_data_t *inst,
        struct rpmsg_kdrv_display_commit_request *c, app_remote_disp_pipe_data_t **pipes)
{
    int32_t ret = 0;
    uint32_t i;

    for(i = 0; i < c->num_vid_updates; i++) {
        pipes[i] = appRemoteDisplayInstFindPipeTpId(inst, c->vid[i].id);
        if(pipes[i] == NULL) {
            appLogPrintf("%s: Could not get pipe Data\n", __func__);
            ret = -1;
            break;
        }
    }

    return ret;
}

static app_remote_disp_commit_t *appRemoteDisplayAllocateCommit()
{
    int32_t ret = 0;
    void *value;
    app_remote_disp_commit_t *commit = NULL;

    ret = appQueueGet(&g_rdisp_data.commit_pool, &value);
    if(ret != 0) {
        appLogPrintf("%s: Failed to alloc commit for reference\n", __func__);
        ret = -1;
    }

    if(ret == 0) {
        commit = value;
        memset(commit, 0, sizeof(*commit));

        ret = appQueueGet(&g_rdisp_data.message_pool, &commit->msg);
        if(ret != 0) {
            appLogPrintf("%s: Failed to alloc message for commit ref\n", __func__);
            ret = -1;
        }
    }

    if(ret == 0) {
        memset(commit->msg, 0,
                sizeof(struct rpmsg_kdrv_display_commit_done_message) +
                sizeof(struct rpmsg_kdrv_device_header) +
                sizeof(app_remote_disp_message_t));
    }

    return commit;
}

static app_remote_disp_buffer_t *appRemoteDisplayAllocateBuffer()
{
    int32_t ret;
    void *value;
    app_remote_disp_buffer_t *buffer = NULL;

    ret = appQueueGet(&g_rdisp_data.buffer_pool, &value);
    if(ret != 0) {
        appLogPrintf("%s: Failed to alloc buffer for reference\n", __func__);
        ret = -1;
    }

    if(ret == 0) {
        buffer = value;
        memset(buffer, 0, sizeof(*buffer));

        ret = appQueueGet(&g_rdisp_data.message_pool, &buffer->msg);
        if(ret != 0) {
            appLogPrintf("%s: Failed to alloc message for buffer ref\n", __func__);
            ret = -1;
        }
    }

    if(ret == 0) {
        memset(buffer->msg, 0,
                sizeof(struct rpmsg_kdrv_display_buffer_done_message) +
                sizeof(struct rpmsg_kdrv_device_header) +
                sizeof(app_remote_disp_message_t));
    }

    return buffer;
}

static int32_t appRemoteDisplayAllocateRefs(app_remote_disp_inst_data_t *inst,
        struct rpmsg_kdrv_display_commit_request *c, app_remote_disp_commit_t **commit,
        app_remote_disp_buffer_t **buffers)
{
    int32_t ret = 0;
    uint32_t i;

    *commit = appRemoteDisplayAllocateCommit();
    if(*commit == NULL) {
        appLogPrintf("%s: Failed to allocate commit\n", __func__);
        ret = -1;
    }

    if(ret == 0) {
        for(i = 0; i < c->num_vid_updates; i++) {
            if(c->vid[i].enabled) {
                buffers[i] = appRemoteDisplayAllocateBuffer();
                if(buffers[i] == NULL) {
                    appLogPrintf("%s Failed to allocate buffer\n", __func__);
                    ret = -1;
                    break;
                }
            }
        }
    }

    return ret;
}

static int32_t appRemoteDisplayConvertFormats(app_remote_disp_inst_data_t *inst,
        struct rpmsg_kdrv_display_commit_request *c, uint32_t *formats)
{
    int32_t ret = 0;
    uint32_t i;

    for(i = 0; i < c->num_vid_updates; i++) {
        if(c->vid[i].enabled) {
            formats[i] = appRemoteDisplayToSystemDf(c->vid[i].buffer.format);
            if(formats[i] == FVID2_DF_INVALID) {
                appLogPrintf("%s: Could not convert format\n", __func__);
                ret = -1;
                break;
            }
        } else {
            formats[i] = FVID2_DF_INVALID;
        }
    }

    return ret;
}

static void appRemoteDisplaySetupCommit(app_remote_disp_inst_data_t *inst,
        struct rpmsg_kdrv_display_commit_request *c, app_remote_disp_pipe_data_t **pipes,
        uint32_t *formats, app_remote_disp_commit_t *commit, app_remote_disp_buffer_t **buffers)
{

    app_remote_disp_pipe_data_t *pipe_data;
    struct rpmsg_kdrv_display_vid_update_info *vid;
    app_remote_disp_buffer_t *f;
    uint32_t num_planes;
    uint32_t fmt;
    uint32_t i, j;

    commit->commit_id = c->commit_id;
    commit->num_upds = c->num_vid_updates;
    commit->upds_done = 0;

    for(i = 0; i < c->num_vid_updates; i++) {
        if(c->vid[i].enabled) {
            pipe_data = pipes[i];
            vid = &c->vid[i];
            f = buffers[i];
            fmt = formats[i];
            num_planes = vid->buffer.num_planes;

            f->commit = commit;
            f->buffer_id = vid->buffer.buffer_id;
            Fvid2Frame_init(&f->frame);

            for(j = 0; j < num_planes; j++) {
                /* TODO: temporary hack since addr field is u32 */
                f->frame.addr[j] = vid->buffer.plane[j];
            }
            f->frame.fid = FVID2_FID_FRAME;
            f->frame.appData = f;

            Dss_dispRtParamsInit(&f->rt_prm.rt_prm);
            Dss_scRtParamsInit(&f->rt_prm.sc_prm);
            Dss_frameRtParamsInit(&f->rt_prm.in_frm_prm);
            Dss_frameRtParamsInit(&f->rt_prm.out_frm_prm);
            Fvid2PosConfig_init(&f->rt_prm.pos_prm);

            /* Always fill RTParams */
            f->rt_prm.in_frm_prm.width = vid->buffer.width;
            f->rt_prm.out_frm_prm.width = vid->dst_w;
            f->rt_prm.in_frm_prm.height = vid->buffer.height;
            f->rt_prm.out_frm_prm.height = vid->dst_h;
            f->rt_prm.in_frm_prm.dataFormat = fmt;
            f->rt_prm.out_frm_prm.dataFormat = fmt;
            for(j = 0; j < num_planes; j++) {
                f->rt_prm.in_frm_prm.pitch[j] = vid->buffer.pitch[j];
                f->rt_prm.out_frm_prm.pitch[j] = vid->buffer.pitch[j];
            }
            f->rt_prm.pos_prm.startX = vid->dst_x;
            f->rt_prm.pos_prm.startY = vid->dst_y;

            f->rt_prm.rt_prm.posCfg = &f->rt_prm.pos_prm;
            f->rt_prm.rt_prm.inFrmParams = &f->rt_prm.in_frm_prm;
            f->rt_prm.rt_prm.outFrmParams = &f->rt_prm.out_frm_prm;
            f->rt_prm.rt_prm.scParams = &f->rt_prm.sc_prm;

            f->frame.perFrameCfg = &f->rt_prm.rt_prm;

            /* pipe will be started now, so have params for IOCTL_DSS_DISP_SET_DSS_PARAMS ready */
            if(pipe_data->is_pipe_running == 0) {
                Dss_dispParamsInit(&f->disp_prm);

                f->disp_prm.pipeCfg.pipeType = (Dss_dispIsVidLInst(pipe_data->pipe_prm.pipe_id) ?
                        CSL_DSS_VID_PIPE_TYPE_VIDL : CSL_DSS_VID_PIPE_TYPE_VID);
                f->disp_prm.pipeCfg.inFmt.width = vid->buffer.width;
                f->disp_prm.pipeCfg.inFmt.height = vid->buffer.height;
                for(j = 0; j < num_planes; j++)
                    f->disp_prm.pipeCfg.inFmt.pitch[j] = vid->buffer.pitch[j];
                f->disp_prm.pipeCfg.inFmt.dataFormat = fmt;
                f->disp_prm.pipeCfg.inFmt.scanFormat = FVID2_SF_PROGRESSIVE;
                f->disp_prm.pipeCfg.outWidth = vid->dst_w;
                f->disp_prm.pipeCfg.outHeight = vid->dst_h;
                f->disp_prm.pipeCfg.scEnable = (Dss_dispIsVidLInst(pipe_data->pipe_prm.pipe_id) ? FALSE : TRUE);
                f->disp_prm.alphaCfg.globalAlpha = 0xff;
                f->disp_prm.alphaCfg.preMultiplyAlpha = FALSE;
                f->disp_prm.layerPos.startX = vid->dst_x;
                f->disp_prm.layerPos.startY = vid->dst_y;
            }
        }
    }
}

static int32_t appRemoteDisplaySendOneCommit(app_remote_disp_inst_data_t *inst,
        struct rpmsg_kdrv_display_commit_request *c)
{
    app_remote_disp_commit_t *commit;
    int32_t ret;
    int32_t fvid2_ret;
    app_remote_disp_buffer_t *buffers[APP_REMOTE_DISPLAY_MAX_PIPES] = {NULL};
    app_remote_disp_pipe_data_t *pipes[APP_REMOTE_DISPLAY_MAX_PIPES] = {NULL};
    uint32_t formats[APP_REMOTE_DISPLAY_MAX_PIPES];
    Fvid2_FrameList frmList;
    app_remote_disp_buffer_t *f;
    app_remote_disp_pipe_data_t *pipe_data;
    uint32_t i;

    /* Get all pipe structs for this commit */
    ret = appRemoteDisplayGetPipeData(inst, c, &pipes[0]);
    if(ret != 0)
        appLogPrintf("%s: Could not get all pipe data\n", __func__);

    /* Convert all formats */
    if(ret == 0) {
        ret = appRemoteDisplayConvertFormats(inst, c, &formats[0]);
        if(ret != 0)
            appLogPrintf("%s: Could not convert all formats\n", __func__);
    }

    /* Get commit (with msg) and buffers (with msg) from pools */
    if(ret == 0) {
        ret = appRemoteDisplayAllocateRefs(inst, c, &commit, &buffers[0]);
        if(ret != 0)
            appLogPrintf("%s: Failed to allocate ref structures\n", __func__);

    }

    if(ret == 0) {
        /* Setup the commit structures */
        appRemoteDisplaySetupCommit(inst, c, &pipes[0], &formats[0], commit, &buffers[0]);

        /* TODO: do we need a HwiP_disable here? these data are accessed from ISR */
        for(i = 0; i < c->num_vid_updates; i++) {
            pipe_data = pipes[i];

            if(c->vid[i].enabled == 0) {
                /* Stop requested. Stop the pipe */
                pipe_data->is_pipe_stopping = 1;
                pipe_data->is_pipe_running = 0;
                pipe_data->stop_commit = commit;

                fvid2_ret = Fvid2_stop(pipe_data->fvid2_hndl, NULL);
                if(fvid2_ret != FVID2_SOK) {
                    appLogPrintf("%s: Failed to stop pipe on request\n", __func__);
                    ret = -1;
                }
            } else {
                /* Pipe not running. Queue this frame and start the pipe */
                f = buffers[i];

                Fvid2FrameList_init(&frmList);
                frmList.numFrames  = 1U;
                frmList.frames[0U] = &f->frame;

                if(pipe_data->is_pipe_running == 0) {
                    pipe_data->is_pipe_running = 1;

                    fvid2_ret = Fvid2_control(pipe_data->fvid2_hndl,
                            IOCTL_DSS_DISP_SET_DSS_PARAMS,
                            &f->disp_prm, NULL);
                    if(fvid2_ret != FVID2_SOK) {
                        appLogPrintf("%s: Failed to set disp params for pipe update\n", __func__);
                        ret = -1;
                    }

                    if(ret == 0) {
                        fvid2_ret = Fvid2_queue(pipe_data->fvid2_hndl, &frmList, 0U);
                        if(FVID2_SOK != fvid2_ret) {
                            appLogPrintf("%s: Failed to queue pre-start frame\n", __func__);
                            ret = -1;
                        }
                    }

                    if(ret == 0) {
                        fvid2_ret = Fvid2_start(pipe_data->fvid2_hndl, NULL);
                        if(fvid2_ret != FVID2_SOK)
                        {
                            appLogPrintf("%s: Failed to start pipe\n", __func__);
                            ret = -1;
                        }
                    }

                } else {
                    /*Pipe running. Just queue the frame */
                    fvid2_ret = Fvid2_queue(pipe_data->fvid2_hndl, &frmList, 0U);
                    if(FVID2_SOK != fvid2_ret) {
                        appLogPrintf("%s: Failed to queue frame\n", __func__);
                        ret = -1;
                    }
                }
            }

            if(ret != 0) {
                appLogPrintf("%s: Pipe Update failed\n", __func__);
                break;
            }
        }
    }

    return ret;

}

static int32_t appRemoteDisplayHandleCommitRequest(app_remote_disp_inst_data_t *inst,
        app_remote_device_channel_t *channel, uint32_t request_id,
        struct rpmsg_kdrv_display_commit_request *req)
{
    void *value;
    int32_t ret;
    app_remote_disp_message_t *msg;
    struct rpmsg_kdrv_device_header *dev_hdr;
    struct rpmsg_kdrv_display_commit_response *resp;
    char err_str[128];
    uint32_t should_commit = TRUE;

    /* validate first. This should catch errors early and prevent H/W setup with invalid params */
    ret = appRemoteDisplayValidateCommit(inst, req, err_str, 128);
    if(ret != 0) {
        appLogPrintf("%s: invalid commit: %s\n", __func__, err_str);
        should_commit = FALSE;
        ret = 0;
    }

    /* We might not be doing H/W setup, but should send the status message */
    ret = appQueueGet(&g_rdisp_data.message_pool, &value);
    if(ret != 0) {
        appLogPrintf("%s: Could not get an empty message\n", __func__);
    }

    if(ret == 0) {
        msg = (app_remote_disp_message_t *)value;
        memset(msg, 0, sizeof(*msg) + sizeof(*dev_hdr) + sizeof(*resp));

        msg->request_id = request_id;
        msg->message_size = sizeof(*resp);
        msg->device_id = inst->device_id;
        msg->is_response = TRUE;
        msg->channel = inst->channel;

        dev_hdr = (struct rpmsg_kdrv_device_header *)(&msg->data[0]);
        resp = (struct rpmsg_kdrv_display_commit_response *)(DEVHDR_2_MSG(dev_hdr));

        resp->header.message_type = RPMSG_KDRV_TP_DISPLAY_COMMIT_RESPONSE;
        resp->commit_id = req->commit_id;
        resp->status = ((should_commit == TRUE) ? 0 : 1);

        ret = appQueuePut(&g_rdisp_data.send_queue, msg);
        if(ret != 0) {
            /* Can't really do anything if this is goofed up. keep going */
            appLogPrintf("%s: Could not queue message for transmission\n", __func__);
        }
    }

    if(ret == 0) {
        SemaphoreP_post(g_rdisp_data.send_sem);
        if(should_commit == TRUE) {
            ret = appRemoteDisplaySendOneCommit(inst, req);
            if(ret != 0)
                appLogPrintf("%s: Actual Commit failed\n", __func__);
        }
    }

    return ret;
}

static int32_t appRemoteDisplayHandleQueryRequest(app_remote_disp_inst_data_t *inst,
        app_remote_device_channel_t *channel, uint32_t request_id,
        struct rpmsg_kdrv_display_ready_query_request *req)
{
    void *value;
    int32_t ret;
    app_remote_disp_message_t *msg;
    struct rpmsg_kdrv_device_header *dev_hdr;
    struct rpmsg_kdrv_display_ready_query_response *resp;

    ret = appQueueGet(&g_rdisp_data.message_pool, &value);
    if(ret != 0)
        appLogPrintf("%s: Could not get an empty message\n", __func__);

    if(ret == 0) {
        msg = (app_remote_disp_message_t *)value;
        memset(msg, 0, sizeof(*msg) + sizeof(*dev_hdr) + sizeof(*resp));

        msg->request_id = request_id;
        msg->message_size = sizeof(*resp);
        msg->device_id = inst->device_id;
        msg->is_response = TRUE;
        msg->channel = inst->channel;

        dev_hdr = (struct rpmsg_kdrv_device_header *)(&msg->data[0]);
        resp = (struct rpmsg_kdrv_display_ready_query_response *)(DEVHDR_2_MSG(dev_hdr));

        resp->header.message_type = RPMSG_KDRV_TP_DISPLAY_READY_QUERY_RESPONSE;
        /* We are always ready if we have reached this point */
        resp->ready = 1;

        ret = appQueuePut(&g_rdisp_data.send_queue, msg);
        if(ret != 0)
            appLogPrintf("%s: Could not queue message for transmission\n", __func__);
    }

    if(ret == 0)
        SemaphoreP_post(g_rdisp_data.send_sem);

    return ret;
}

static int32_t appRemoteDisplayHandleResInfoRequest(app_remote_disp_inst_data_t *inst,
        app_remote_device_channel_t *channel, uint32_t request_id,
        struct rpmsg_kdrv_display_res_info_request *req)
{
    uint32_t cnt, icnt, z, fmt;
    void *value;
    int32_t ret = 0;
    app_remote_disp_message_t *msg;
    struct rpmsg_kdrv_device_header *dev_hdr;
    struct rpmsg_kdrv_display_res_info_response *resp;
    struct rpmsg_kdrv_display_vp_info *vp_dst;
    struct rpmsg_kdrv_display_vid_info *vid_dst;
    app_remote_disp_pipe_data_t *vid_src;
    app_remote_disp_disp_data_t *vp_src;

    if(inst->disp_count > RPMSG_KDRV_TP_DISPLAY_MAX_VPS) {
        appLogPrintf("%s: instance has more disps than transport allows\n", __func__);
        ret = -1;;
    }

    if(ret == 0) {
        for(cnt = 0; cnt < inst->disp_count; cnt++) {
            if(inst->disp_data[cnt].pipe_count > RPMSG_KDRV_TP_DISPLAY_MAX_VIDS) {
                appLogPrintf("%s: instance has more pipes than transport allows\n", __func__);
                ret = -1;
            }

            for(icnt = 0; icnt < inst->disp_data[cnt].pipe_count; icnt++) {
                if(ret == 0) {
                    if(inst->disp_data[cnt].pipes[icnt]->pipe_prm.num_zorders > RPMSG_KDRV_TP_DISPLAY_MAX_ZORDERS) {
                        appLogPrintf("%s: instance has more zorders than transport allows\n", __func__);
                        ret = -1;
                    }
                }
                if(ret == 0) {
                    if(inst->disp_data[cnt].pipes[icnt]->pipe_prm.num_formats > RPMSG_KDRV_TP_DISPLAY_MAX_FORMATS) {
                        appLogPrintf("%s: instance has more formats than transport allows\n", __func__);
                        ret = -1;
                    }
                }
            }
        }
    }

    if(ret == 0) {
        /* Get a messgage from message pool, fill it up, and put in send queue */
        ret = appQueueGet(&g_rdisp_data.message_pool, &value);
        if(ret != 0) {
            appLogPrintf("%s: Could not get an empty message\n", __func__);
        }
    }

    if(ret == 0) {
        msg = (app_remote_disp_message_t *)value;
        memset(msg, 0, sizeof(*msg) + sizeof(*dev_hdr) + sizeof(*resp));

        msg->request_id = request_id;
        msg->message_size = sizeof(*resp);
        msg->device_id = inst->device_id;
        msg->is_response = TRUE;
        msg->channel = inst->channel;

        dev_hdr = (struct rpmsg_kdrv_device_header *)(&msg->data[0]);
        resp = (struct rpmsg_kdrv_display_res_info_response *)(DEVHDR_2_MSG(dev_hdr));

        resp->header.message_type = RPMSG_KDRV_TP_DISPLAY_RES_INFO_RESPONSE;

        /* Fill the data from all VPs */
        resp->num_vps = inst->disp_count;
        for(cnt = 0; cnt < resp->num_vps; cnt++) {
            vp_dst = &resp->vp[cnt];
            vp_src = &inst->disp_data[cnt];

            vp_dst->id = vp_src->tp_id;
            vp_dst->width = vp_src->disp_prm.width;
            vp_dst->height = vp_src->disp_prm.height;
            vp_dst->refresh = vp_src->disp_prm.refresh;
            vp_dst->num_vids = vp_src->pipe_count;

            /* Fill the data from all Vids (of this VP) */
            for(icnt = 0; icnt < vp_dst->num_vids; icnt++) {
                vid_dst = &vp_dst->vid[icnt];
                vid_src = vp_src->pipes[icnt];

                vid_dst->id = vid_src->tp_id;
                vid_dst->mutable_window = vid_src->pipe_prm.win_mod_cap;
                vid_dst->can_scale = vid_src->pipe_prm.scale_cap;

                if(vid_dst->mutable_window != 0) {
                    vid_dst->fixed_window_x = 0;
                    vid_dst->fixed_window_y = 0;
                    vid_dst->fixed_window_w = 0;
                    vid_dst->fixed_window_h = 0;
                } else {
                    vid_dst->fixed_window_x = vid_src->pipe_prm.pos_x;
                    vid_dst->fixed_window_y = vid_src->pipe_prm.pos_y;
                    vid_dst->fixed_window_w = vid_src->pipe_prm.width;
                    vid_dst->fixed_window_h = vid_src->pipe_prm.height;
                }

                vid_dst->num_formats = vid_src->pipe_prm.num_formats;
                vid_dst->num_zorders = vid_src->pipe_prm.num_zorders;
                vid_dst->init_zorder = vid_src->pipe_prm.current_zorder;

                /* TODO : Do we need to check the FMT convertions? already checksd in Init */
                for(fmt = 0; fmt < vid_dst->num_formats; fmt++)
                    vid_dst->format[fmt] = appRemoteDisplayToRemoteFmt(vid_src->pipe_prm.formats[fmt]);

                for(z = 0; z < vid_dst->num_zorders; z++)
                    vid_dst->zorder[z] = vid_src->pipe_prm.zorders[z];

            }
        }

        ret = appQueuePut(&g_rdisp_data.send_queue, msg);
        if(ret != 0)
            appLogPrintf("%s: Could not queue message for transmission\n", __func__);
    }

    if(ret == 0)
        SemaphoreP_post(g_rdisp_data.send_sem);

    return ret;

}

static int32_t appRemoteDisplayRequest(uint32_t device_id, app_remote_device_channel_t *channel,
        uint32_t request_id, void *data, uint32_t len)
{
    app_remote_disp_inst_data_t *inst;
    struct rpmsg_kdrv_display_message_header *hdr = data;
    int32_t ret = 0;

    SemaphoreP_pend(g_rdisp_data.lock_sem, SemaphoreP_WAIT_FOREVER);

    inst = appRemoteDisplayDataFindDeviceId(device_id);
    if(inst == NULL) {
        appLogPrintf("%s: Could not find a instance\n", __func__);
        ret = -1;
    }

    if(ret == 0) {
        if(channel != inst->channel) {
            appLogPrintf("%s: mismatch channel\n", __func__);
            ret = -1;
        }
    }

    if(ret == 0) {
        switch(hdr->message_type) {
            case RPMSG_KDRV_TP_DISPLAY_READY_QUERY_REQUEST:
                ret = appRemoteDisplayHandleQueryRequest(inst, channel, request_id,
                        (struct rpmsg_kdrv_display_ready_query_request *)data);
                break;
            case RPMSG_KDRV_TP_DISPLAY_RES_INFO_REQUEST:
                ret = appRemoteDisplayHandleResInfoRequest(inst, channel, request_id,
                        (struct rpmsg_kdrv_display_res_info_request *)data);
                break;
            case RPMSG_KDRV_TP_DISPLAY_COMMIT_REQUEST:
                ret = appRemoteDisplayHandleCommitRequest(inst, channel, request_id,
                        (struct rpmsg_kdrv_display_commit_request *)data);
                break;
            default:
                appLogPrintf("%s: unidentified request\n", __func__);
                ret = -1;
        }
    }

    SemaphoreP_post(g_rdisp_data.lock_sem);
    return ret;
}

static int32_t appRemoteDisplayMessage(uint32_t device_id, app_remote_device_channel_t *channel,
        void *data, uint32_t len)
{
    app_remote_disp_inst_data_t *inst;
    int32_t ret = 0;

    SemaphoreP_pend(g_rdisp_data.lock_sem, SemaphoreP_WAIT_FOREVER);

    inst = appRemoteDisplayDataFindDeviceId(device_id);
    if(inst == NULL) {
        appLogPrintf("%s: Could not find a instance\n", __func__);
        ret = -1;
    }

    if(ret == 0) {
        if(channel != inst->channel) {
            appLogPrintf("%s: mismatch channel\n", __func__);
            ret = -1;
        }
    }

    if(ret == 0) {
        /* Messages are not yet supported */
        appLogPrintf("%s: unidentified message\n", __func__);
        ret = -1;
    }

    SemaphoreP_post(g_rdisp_data.lock_sem);
    return ret;
}

static int32_t appRemoteDisplayMessageDoneFn(void *meta, void *msg, uint32_t len)
{
    int32_t ret;
    app_remote_disp_message_t *message = (app_remote_disp_message_t *)meta;

    /* Put the empty message back in message pool */
    ret = appQueuePut(&g_rdisp_data.message_pool, message);
    if(ret != 0)
        appLogPrintf("%s: Could not put empty message to pool\n", __func__);

    return ret;
}

static void appRemoteDisplaySenderTaskFn(void *arg0, void *arg1)
{
    void *value;
    app_remote_disp_message_t *msg;
    int32_t ret;

    while(1) {
        ret = 0;

        /* Wait for a signal = a new message to be sent */
        SemaphoreP_pend(g_rdisp_data.send_sem, SemaphoreP_WAIT_FOREVER);
        ret = appQueueGet(&g_rdisp_data.send_queue, &value);
        if(ret != 0)
            appLogPrintf("%s: Could not dequeue message to send\n", __func__);

        if(ret == 0) {
            msg = (app_remote_disp_message_t *)value;
            /* Use remote device framework to send the message */
            ret = appRemoteDeviceSendMessage(msg->channel, &msg->data[0], msg->message_size, msg->is_response, msg->request_id,
                    msg->device_id, appRemoteDisplayMessageDoneFn, msg);
            if(ret != 0)
                appLogPrintf("%s: Could not send message\n", __func__);

        }

        APP_REMOTE_DISPLAY_ASSERT_SUCCESS(ret);
    }
}

static Fvid2_Frame * appRemoteDisplayBufPrgmCb(Fvid2_Handle handle,
        Fvid2_Frame *cur_frm,
        uint32_t is_repeat,
        uint32_t repeat_cnt)
{
    return cur_frm;
}

/*TODO: Should we call the following two functions with HwiP_disable? They access data set by a Task */
static int32_t appRemoteDisplayGetStopCommit(app_remote_disp_pipe_data_t *pipe, app_remote_disp_commit_t **commit)
{
    int32_t ret = 0;
    app_remote_disp_commit_t *lcommit;

    if(pipe->is_pipe_stopping != 1) {
        appLogPrintf("%s: pipe %u:%u not stopping\n", __func__, pipe->inst->serial, pipe->serial);
        ret = -1;
    }

    if(ret == 0) {
        lcommit = pipe->stop_commit;
        if(lcommit == NULL) {
            appLogPrintf("%s: pipe %u:%u stop-commit = NULL\n", __func__, pipe->inst->serial, pipe->serial);
            ret = -1;
        }
    }

    if(ret == 0) {
        if(lcommit->msg == NULL) {
            appLogPrintf("%s: pipe %u:%u commit = %p commit->msg is NULL\n", __func__, pipe->inst->serial, pipe->serial,
                    commit);
            ret = -1;
        }
    }

    if(ret == 0) {
        pipe->stop_commit = NULL;
        pipe->is_pipe_stopping = 0;
        *commit = lcommit;
    }

    return ret;
}

static int32_t appRemoteDisplayGetFrameCommit(Fvid2_Frame *frm, app_remote_disp_commit_t **commit)
{
    int32_t ret = 0;
    app_remote_disp_commit_t *lcommit;
    app_remote_disp_buffer_t *ref;

    ref = frm->appData;
    if(ref == NULL) {
        appLogPrintf("%s: frame %p frame->appData is NULL\n", __func__, frm);
        ret = -1;
    }

    if(ret == 0) {
        lcommit = ref->commit;

        if(lcommit != NULL && lcommit->msg == NULL) {
            appLogPrintf("%s: frame = %p frame->msg is NULL\n", __func__, frm);
            ret = -1;
        }
    }

    if(ret == 0) {
        if(lcommit != NULL)
            ref->commit = NULL;

        *commit = lcommit;
    }

    return ret;

}

static int32_t appRemoteDisplayPipePrgmCb(Fvid2_Frame *prog_frm,
        void *cb_data)
{
    app_remote_disp_pipe_data_t *pipe = cb_data;
    app_remote_disp_commit_t *commit = NULL;
    app_remote_disp_message_t *msg;
    struct rpmsg_kdrv_device_header *dev_hdr;
    struct rpmsg_kdrv_display_commit_done_message *resp;
    int32_t ret = 0;

    if(prog_frm == NULL) {
        /* Either the pipe is stopping => prog_frm == NULL, so get the stop-commit from pipe_data*/
        ret = appRemoteDisplayGetStopCommit(pipe, &commit);
    } else {
        /* Or, get the commit from the frame that has just been programmed */
        ret = appRemoteDisplayGetFrameCommit(prog_frm, &commit);
    }

    if(ret == 0) {
        /* We may have a NULL commit for a repeated frame */
        if(commit != NULL) {

            /* TODO: decide if we need to lock this. It is always done in interrupt*/
            commit->upds_done++;

            /* all pipe-updates for this commit ref complete */
            if(commit->upds_done == commit->num_upds) {
                msg = commit->msg;
                dev_hdr = (struct rpmsg_kdrv_device_header *)(&msg->data[0]);
                resp = (struct rpmsg_kdrv_display_commit_done_message *)(DEVHDR_2_MSG(dev_hdr));

                msg->request_id = 0;
                msg->message_size = sizeof(*resp);
                msg->device_id = pipe->inst->device_id;
                msg->is_response = FALSE;
                msg->channel = pipe->inst->channel;

                resp->header.message_type = RPMSG_KDRV_TP_DISPLAY_COMMIT_DONE_MESSAGE;
                resp->commit_id = commit->commit_id;

                /*queue the mesage for transmission */
                ret = appQueuePut(&g_rdisp_data.send_queue, msg);
                if(ret != 0)
                    appLogPrintf("%s: Could not queue message for transmission\n", __func__);

                if(ret == 0) {
                    SemaphoreP_post(g_rdisp_data.send_sem);

                    /* Dont need the commit any more, dealloc it */
                    ret = appQueuePut(&g_rdisp_data.commit_pool, commit);
                    if(ret != 0)
                        appLogPrintf("%s: Could not put commit to pool\n", __func__);
                }
            }
        }
    }

    if(ret == 0 && prog_frm == NULL) {
        ret = appRemoteDisplayPipeFrmDoneCb(pipe->fvid2_hndl, pipe);
        if(ret != 0)
            appLogPrintf("%s: Could not dequeue frame at pipe stop\n", __func__);
    }

    APP_REMOTE_DISPLAY_ASSERT_SUCCESS(ret);

    return FVID2_SOK;
}

static int32_t appRemoteDisplayReturnFrame(app_remote_disp_pipe_data_t *pipe, Fvid2_Frame *frm)
{
    app_remote_disp_buffer_t *f;
    app_remote_disp_message_t *msg;
    struct rpmsg_kdrv_device_header *dev_hdr;
    struct rpmsg_kdrv_display_buffer_done_message *resp;
    int32_t ret = 0;

    f = frm->appData;
    msg = f->msg;
    if(msg == NULL) {
        appLogPrintf("%s: buffer = %p buffer->msg is NULL\n", __func__, frm);
        ret = -1;
    }

    if(ret == 0) {
        dev_hdr = (struct rpmsg_kdrv_device_header *)(&msg->data[0]);
        resp = (struct rpmsg_kdrv_display_buffer_done_message *)(DEVHDR_2_MSG(dev_hdr));

        msg->request_id = 0;
        msg->message_size = sizeof(*resp);
        msg->device_id = pipe->inst->device_id;
        msg->is_response = FALSE;
        msg->channel = pipe->inst->channel;

        resp->header.message_type = RPMSG_KDRV_TP_DISPLAY_BUFFER_DONE_MESSAGE;
        resp->buffer_id = f->buffer_id;

        /*queue the mesage for transmission */
        ret = appQueuePut(&g_rdisp_data.send_queue, msg);
        if(ret != 0)
            appLogPrintf("%s: Could not queue message for transmission\n", __func__);
    }

    if(ret == 0) {
        SemaphoreP_post(g_rdisp_data.send_sem);

        if(f->commit)
            appLogPrintf("DEBUG: pending commit %p in frame %p\n", f->commit, f);

        /* Dont need the frame any more, dealloc it */
        ret = appQueuePut(&g_rdisp_data.buffer_pool, f);
        if(ret != 0)
            appLogPrintf("%s: Could not put buffer in pool\n", __func__);

    }

    return ret;
}

static int32_t appRemoteDisplayPipeFrmDoneCb(Fvid2_Handle handle, void *app_data)
{
    app_remote_disp_pipe_data_t *pipe = app_data;
    Fvid2_FrameList frmList;
    Fvid2_Frame *frames[APP_REMOTE_DISPLAY_MAX_DEQUEUES];
    Fvid2_Frame *frm;
    uint32_t num_frames = 0;
    uint32_t dqcount = 0;
    int32_t ret = 0;
    int32_t fvid2_ret = 0;
    uint32_t i;

    /* Get all frames that can be dequeued. May be = 0 */
    do {
        fvid2_ret = Fvid2_dequeue(pipe->fvid2_hndl, &frmList, 0U, FVID2_TIMEOUT_NONE);
        if(fvid2_ret != FVID2_SOK) {
            ret = 0;
            break;
        }

        dqcount++;
        if(dqcount > 1)
            appLogPrintf("DEBUG: Dequeue 1+ frames in frame done callback for pipe %u\n",
                    pipe->pipe_prm.pipe_id);

        if(frmList.numFrames > 1)
            appLogPrintf("DEBUG: Dequeue 1+ frames (in frmList) in frame done callback for pipe %u\n",
                    pipe->pipe_prm.pipe_id);

        for(i = 0; i < frmList.numFrames; i++) {
            if(num_frames == APP_REMOTE_DISPLAY_MAX_DEQUEUES) {
                appLogPrintf("%s: Could not find slot to store dequeued frame, num_frames = %u\n", __func__, num_frames);
                ret = -1;
            }

            if(ret == 0) {
                frames[num_frames] = frmList.frames[i];
                num_frames++;
            }
        }

    } while(TRUE && ret == 0);

    if(ret == 0) {
        for(i = 0; i < num_frames; i++) {
            frm = frames[i];

            ret = appRemoteDisplayReturnFrame(pipe, frm);
            if(ret != 0) {
                appLogPrintf("%s: Could not return frame %u\n", i);
                break;
            }
        }
    }

    APP_REMOTE_DISPLAY_ASSERT_SUCCESS(ret);

    return FVID2_SOK;
}

static int32_t appRemoteDisplaySenderTaskInit()
{
    SemaphoreP_Params sem_params;
    TaskP_Params tsk_prm;
    int32_t ret;

    /* The send queue. Task picks up from this queue and sends */
    ret = appQueueInit(&g_rdisp_data.send_queue, FALSE, 0, 0, NULL, 0);
    if(ret != 0)
        appLogPrintf("%s: Could not initialize send queue\n", __func__);

    if(ret == 0) {
        SemaphoreP_Params_init(&sem_params);
        sem_params.mode = SemaphoreP_Mode_COUNTING;

        g_rdisp_data.send_sem = SemaphoreP_create(0, &sem_params);
        if(g_rdisp_data.send_sem == NULL) {
            appLogPrintf("%s: Could not initialize send semaphore\n", __func__);
            ret = -1;
        }
    }

    if(ret == 0) {

        TaskP_Params_init(&tsk_prm);
        tsk_prm.priority = 10;
        tsk_prm.stack = &g_sender_tsk_stack;
        tsk_prm.stacksize = g_sender_tsk_stack_size;

        g_rdisp_data.sender_task = TaskP_create(appRemoteDisplaySenderTaskFn, &tsk_prm);
        if(g_rdisp_data.sender_task == NULL) {
            appLogPrintf("%s: Could not initialize sender task\n", __func__);
            ret = -1;
        }
    }

    return ret;
}

static int32_t appRemoteDisplayInitDisp(app_remote_disp_inst_data_t *inst, app_remote_disp_disp_prm_t *disp_prm)
{
    int32_t ret = 0;
    app_remote_disp_disp_data_t *disp_data = &inst->disp_data[inst->disp_count];
    char err_str[128];

    /* Validate display params */
    ret = appRemoteDisplayValidateDispPrm(disp_prm, err_str, 128);
    if(ret != 0)
        appLogPrintf("%s: [disp %u:%u] Could not validate Disp params: %s\n", __func__, inst->serial, inst->disp_count,
                err_str);

    if(ret == 0) {
        if(inst->disp_count >= APP_REMOTE_DISPLAY_MAX_DISPS) {
            appLogPrintf("%s: [disp %u:%u] Could not find slot for display\n", __func__, inst->serial, inst->disp_count);
            ret = -1;
        }
    }

    /* Copy params into inst data */
    if(ret == 0) {
        memset(disp_data, 0, sizeof(*disp_data));
        memcpy(&disp_data->disp_prm, disp_prm, sizeof(*disp_prm));
        disp_data->serial = inst->disp_count;

        disp_data->pipe_count = 0;
        disp_data->inst = inst;

        /* unique resource id for remote driver */
        disp_data->tp_id = APP_REMOTE_DISPLAY_DISP_PREFIX | inst->disp_count + 1;

        inst->disp_count++;
    }

    return ret;
}

static int32_t appRemoteDisplayInitPipe(app_remote_disp_inst_data_t *inst, app_remote_disp_pipe_prm_t *pipe_prm)
{
    int32_t ret;
    int32_t fvid2_ret;
    char err_str[128];
    app_remote_disp_disp_data_t *disp;
    Dss_DispCreateParams dss_create_prm;
    Dss_DispCreateStatus dss_create_stat;
    Fvid2_CbParams fvid2_cb_prm;
    Dss_DispPipePrgmCbParams dss_pipe_prgm_cb_prm;
    Dss_DispBufPrgmCbParams dss_buf_prgm_cb_prm;
    app_remote_disp_pipe_data_t *pipe_data = &inst->pipe_data[inst->pipe_count];

    /* Validate pipe params */
    ret = appRemoteDisplayValidatePipePrm(pipe_prm, err_str, 128);
    if(ret != 0)
        appLogPrintf("%s: [pipe %u:%u] Could not validate pipe params: %s\n", __func__, inst->serial, inst->pipe_count,
                err_str);

    if(ret == 0) {
        if(inst->pipe_count >= APP_REMOTE_DISPLAY_MAX_PIPES) {
            appLogPrintf("%s: [pipe %u:%u] Could not find slot for pipe\n", __func__, inst->serial, inst->pipe_count);
            ret = -1;
        }
    }

    /* Copy params into inst data */
    if(ret == 0) {
        memset(pipe_data, 0, sizeof(*pipe_data));
        memcpy(&pipe_data->pipe_prm, pipe_prm, sizeof(*pipe_prm));
        pipe_data->serial = inst->pipe_count;

        disp = appRemoteDisplayInstFindDispId(inst, pipe_prm->disp_id);
        if(disp == NULL) {
            appLogPrintf("%s: [pipe %u:%u] Could not find disp data\n", __func__, inst->serial, inst->pipe_count);
            ret = -1;
        }
    }

    if(ret == 0) {
        if(disp->pipe_count >= APP_REMOTE_DISPLAY_MAX_PIPES) {
            appLogPrintf("%s: [pipe %u:%u] Could not find slot for pipe in disp\n", __func__, inst->serial, inst->pipe_count);
            ret = -1;
        }
    }

    /* Link pipe and display data and create Fvid2 handle for pipe */
    if(ret == 0) {
        disp->pipes[disp->pipe_count] = pipe_data;
        disp->pipe_count++;
        pipe_data->disp = disp;

        /* unique resource id for remote driver */
        pipe_data->tp_id = APP_REMOTE_DISPLAY_PIPE_PREFIX | (inst->pipe_count + 1);
        pipe_data->inst = inst;

        Dss_dispCreateParamsInit(&dss_create_prm);
        Fvid2CbParams_init(&fvid2_cb_prm);

        dss_create_prm.periodicCbEnable = TRUE;
        dss_create_prm.progPipeVsyncEnable = TRUE;

        fvid2_cb_prm.cbFxn = appRemoteDisplayPipeFrmDoneCb;
        fvid2_cb_prm.appData = pipe_data;
        fvid2_cb_prm.errCbFxn = NULL;
        fvid2_cb_prm.errList = NULL;

        pipe_data->fvid2_hndl = Fvid2_create(DSS_DISP_DRV_ID, pipe_data->pipe_prm.pipe_id,
                &dss_create_prm, &dss_create_stat,
                &fvid2_cb_prm);
        if((NULL == pipe_data->fvid2_hndl) || (dss_create_stat.retVal != FVID2_SOK))
        {
            appLogPrintf("%s: [pipe %u:%u] Could not create FVID2 handle\n", __func__, inst->serial, inst->pipe_count);
            ret = -1;
        }
    }

    /* Register the Buffer-program callback */
    if(ret == 0) {
        dss_buf_prgm_cb_prm.bufPrgmCbFxn = appRemoteDisplayBufPrgmCb;
        fvid2_ret = Fvid2_control(pipe_data->fvid2_hndl,
                IOCTL_DSS_DISP_REGISTER_BUF_PRGM_CB,
                &dss_buf_prgm_cb_prm, NULL);
        if(fvid2_ret != FVID2_SOK)
        {
            appLogPrintf("%s: [pipe %u:%u] Could not register BufPrgmCb\n", __func__, inst->serial, inst->pipe_count);
            ret = -1;
        }
    }

    /* Register the pipe program callback */
    if(ret == 0) {
        dss_pipe_prgm_cb_prm.pipePrgmCbFxn = appRemoteDisplayPipePrgmCb;
        dss_pipe_prgm_cb_prm.appData = pipe_data;
        fvid2_ret = Fvid2_control(pipe_data->fvid2_hndl,
                IOCTL_DSS_DISP_REGISTER_PIPE_PRGM_CB,
                &dss_pipe_prgm_cb_prm, NULL);
        if(fvid2_ret != FVID2_SOK)
        {
            appLogPrintf("%s: [pipe %u:%u] Could not register PipePrgmCb\n", __func__, inst->serial, inst->pipe_count);
            ret = -1;
        }
    }

    if(ret == 0)
        inst->pipe_count++;

    return ret;
}

static uint32_t appRemoteDisplayFillPrivData(uint32_t device_id, void *priv_data, uint32_t avail_len)
{
    int32_t ret = -1;
    struct rpmsg_kdrv_display_device_data *display_data = (struct rpmsg_kdrv_display_device_data *)priv_data;

    if(avail_len >= sizeof(*display_data)) {
        display_data->periodic_vsync = 0;
        display_data->deferred_buffer_usage = 1;
        ret = sizeof(*display_data);
    }

    return ret;
}

static int32_t appRemoteDisplayInitInst(app_remote_disp_inst_prm_t *inst_prm)
{
    int32_t ret;
    uint32_t cnt;
    app_remote_disp_inst_data_t *inst_data = &g_rdisp_data.inst_data[g_rdisp_data.inst_count];
    app_remote_device_register_prm_t disp_register_prm;
    char err_str[128];

    /* validate instance params */
    ret = appRemoteDisplayValidateInstPrm(inst_prm, err_str, 128);
    if(ret != 0)
        appLogPrintf("%s: [inst %u] Could not validate inst params: %s\n", __func__, g_rdisp_data.inst_count, err_str);

    if(ret == 0) {
        /* find a slot for this instance in global data */
        if(g_rdisp_data.inst_count > APP_REMOTE_DISPLAY_MAX_INSTANCES) {
            appLogPrintf("%s: [inst %u] Could not find slot for instance\n", g_rdisp_data.inst_count);
            ret = -1;
        }
    }

    if(ret == 0) {
        /* copy params into inst data */
        memset(inst_data, 0, sizeof(*inst_data));
        memcpy(&inst_data->inst_prm, inst_prm, sizeof(*inst_prm));
        inst_data->serial = g_rdisp_data.inst_count;

        /*initialise displays in this instance */
        for(cnt = 0; cnt < inst_prm->num_disps; cnt++) {
            ret = appRemoteDisplayInitDisp(inst_data, &inst_prm->disp_prm[cnt]);
            if(ret != 0) {
                appLogPrintf("%s: [inst %u] Could not initialize disp %u\n", g_rdisp_data.inst_count, cnt);
                break;
            }
        }
    }

    if(ret == 0) {
        /* initialise pipes in this instance */
        for(cnt = 0; cnt < inst_prm->num_pipes; cnt++) {
            ret = appRemoteDisplayInitPipe(inst_data, &inst_prm->pipe_prm[cnt]);
            if(ret != 0) {
                appLogPrintf("%s: [inst %u] Could not initialize pipe %u\n", __func__, g_rdisp_data.inst_count, cnt);
                break;
            }
        }
    }

    if(ret == 0) {
        appRemoteDeviceRegisterParamsInit(&disp_register_prm);

        disp_register_prm.num_host_ids = 1;
        disp_register_prm.host_ids[0] = inst_data->inst_prm.host_id;
        disp_register_prm.device_type = APP_REMOTE_DEVICE_DEVICE_TYPE_DISPLAY;
        disp_register_prm.cb.fill_priv_data = appRemoteDisplayFillPrivData;
        disp_register_prm.cb.connect = appRemoteDisplayConnect;
        disp_register_prm.cb.disconnect = appRemoteDisplayDisconnect;
        disp_register_prm.cb.request = appRemoteDisplayRequest;
        disp_register_prm.cb.message = appRemoteDisplayMessage;
        snprintf(disp_register_prm.name, APP_REMOTE_DEVICE_MAX_NAME, "%s", inst_data->inst_prm.name);

        /* Register a virtual display device */
        ret = appRemoteDeviceRegisterDevice(&disp_register_prm, &inst_data->device_id);
        if(ret != 0)
            appLogPrintf("%s: [inst %u] Could not register remote device\n", __func__, g_rdisp_data.inst_count);
    }

    if(ret == 0)
        g_rdisp_data.inst_count++;

    return ret;
}

static int32_t appRemoteDisplayPoolsInit(app_remote_disp_init_prm_t *prm)
{
    int32_t ret;

    /* The pool for commit references */
    ret = appQueueInit(&g_rdisp_data.commit_pool, TRUE, APP_REMOTE_DISPLAY_MAX_COMMITS,
            sizeof(app_remote_disp_commit_t), g_commit_pool_storage, sizeof(g_commit_pool_storage));
    if(ret != 0)
        appLogPrintf("%s: Could not initialize commit pool\n", __func__);

    if(ret == 0) {
        /* The pool for buffer references */
        ret = appQueueInit(&g_rdisp_data.buffer_pool, TRUE, APP_REMOTE_DISPLAY_MAX_BUFFERS,
                sizeof(app_remote_disp_buffer_t), g_buffer_pool_storage, sizeof(g_buffer_pool_storage));
        if(ret != 0) {
            appLogPrintf("%s: Could not initialize buffer pool\n", __func__);
        }
    }

    if(ret == 0) {
        /* The pool for transport messages */
        ret = appQueueInit(&g_rdisp_data.message_pool, TRUE, APP_REMOTE_DISPLAY_MAX_MESSAGES,
                prm->rpmsg_buf_size + sizeof(app_remote_disp_message_t),
                g_message_pool_storage, sizeof(g_message_pool_storage));
        if(ret != 0) {
            appLogPrintf("%s: Could not initialize message pool\n", __func__);
        }
    }

    return ret;
}

int32_t appRemoteDisplayInit(app_remote_disp_init_prm_t *prm)
{
    int32_t ret;
    uint32_t cnt;
    SemaphoreP_Params sem_params;
    char err_str[128];

    /* validate params. */
    ret = appRemoteDisplayValidateInitPrm(prm, err_str, 128);
    if(ret != 0)
        appLogPrintf("%s: Could not validate init params: %s\n", __func__, err_str);

    if(ret == 0) {
        /* copy params into inst data */
        memset(&g_rdisp_data, 0, sizeof(g_rdisp_data));
        memcpy(&g_rdisp_data.prm, prm, sizeof(*prm));

        SemaphoreP_Params_init(&sem_params);
        sem_params.mode = SemaphoreP_Mode_BINARY;

        g_rdisp_data.lock_sem = SemaphoreP_create(1, &sem_params);
        if(g_rdisp_data.lock_sem == NULL) {
            appLogPrintf("%s: Could not initialize lock semaphore (mutex)\n", __func__);
            ret = -1;
        }
    }

    if(ret == 0) {
        /* allocate pools */
        ret = appRemoteDisplayPoolsInit(prm);
        if(ret != 0) {
            appLogPrintf("%s: Could not initialize mandatory pools\n", __func__);
        }
    }

    if(ret == 0) {
        /* initialise instances */
        for(cnt = 0; cnt < prm->num_instances; cnt++) {
            ret = appRemoteDisplayInitInst(&prm->inst_prm[cnt]);
            if(ret != 0) {
                appLogPrintf("%s: Could not initialize instance %u\n", __func__, cnt);
                break;
            }
        }
    }

    if(ret == 0) {
        /* start message sender task (common for all instances) */
        ret = appRemoteDisplaySenderTaskInit();
        if(ret != 0)
            appLogPrintf("Could not start sender task\n");
    }

    return ret;
}
