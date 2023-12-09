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


#ifndef __RPMSG_KDRV_TRANSPORT_DISPLAY_H__
#define __RPMSG_KDRV_TRANSPORT_DISPLAY_H__

#include "rpmsg-kdrv-transport-common.h"

/*
 * Maximum number of planes per buffer
 */
#define RPMSG_KDRV_TP_DISPLAY_MAX_PLANES	(2)

/*
 * Maximum number of shared displays
 */
#define RPMSG_KDRV_TP_DISPLAY_MAX_VPS		(2)

/*
 * Maximum number of pipes per shared display
 */
#define RPMSG_KDRV_TP_DISPLAY_MAX_VIDS		(4)

/*
 * Maximum number of formats supported per pipe
 */
#define RPMSG_KDRV_TP_DISPLAY_MAX_FORMATS	(2)

/*
 * Maximum number of zorders supported per pipe
 */
#define RPMSG_KDRV_TP_DISPLAY_MAX_ZORDERS	(4)

enum rpmsg_kdrv_display_format {
	RPMSG_KDRV_TP_DISPLAY_FORMAT_ARGB8888,
	RPMSG_KDRV_TP_DISPLAY_FORMAT_XRGB8888,
	RPMSG_KDRV_TP_DISPLAY_FORMAT_MAX,
};

enum rpmsg_kdrv_display_message_type {
	RPMSG_KDRV_TP_DISPLAY_READY_QUERY_REQUEST,
	RPMSG_KDRV_TP_DISPLAY_READY_QUERY_RESPONSE,
	RPMSG_KDRV_TP_DISPLAY_RES_INFO_REQUEST,
	RPMSG_KDRV_TP_DISPLAY_RES_INFO_RESPONSE,
	RPMSG_KDRV_TP_DISPLAY_COMMIT_REQUEST,
	RPMSG_KDRV_TP_DISPLAY_COMMIT_RESPONSE,
	RPMSG_KDRV_TP_DISPLAY_COMMIT_DONE_MESSAGE,
	RPMSG_KDRV_TP_DISPLAY_BUFFER_DONE_MESSAGE,
	RPMSG_KDRV_TP_DISPLAY_MAX,
};

/*
 * per-device data for display device
 */
struct rpmsg_kdrv_display_device_data {
	/* Does the device send all vsyncs? */
	u8 periodic_vsync;
	/*Does the device defer the use of buffers? */
	u8 deferred_buffer_usage;
} __packed;

/*
 * message header for display device
 */
struct rpmsg_kdrv_display_message_header {
	/* enum: rpmsg_kdrv_display_message_type */
	u8 message_type;
} __packed;

/* display device request to provide ready / not-ready info */
struct rpmsg_kdrv_display_ready_query_request {
	/* message header */
	struct rpmsg_kdrv_display_message_header header;
} __packed;

/* display device response indicating ready / not-ready status */
struct rpmsg_kdrv_display_ready_query_response {
	/* message header */
	struct rpmsg_kdrv_display_message_header header;
	/* can be 0 : if not ready 1: if ready */
	u8 ready;
} __packed;

/* display device buffer update info */
struct rpmsg_kdrv_display_buffer_info {
	/* buffer width */
	u16 width;
	/* buffer height */
	u16 height;
	/* enum: rpmsg_kdrv_display_format */
	u8 format;
	/* number of planes */
	u8 num_planes;
	/* per plane start addresses */
	u64 plane[RPMSG_KDRV_TP_DISPLAY_MAX_PLANES];
	/* per plane pitch */
	u16 pitch[RPMSG_KDRV_TP_DISPLAY_MAX_PLANES];
	/* buffer id : to be used in buffer-done message */
	u32 buffer_id;
} __packed;

/* display device pipe update info */
struct rpmsg_kdrv_display_vid_update_info {
	/* pipe ID */
	u8 id;
	/*enable / disable request */
	u8 enabled;
	/* window width */
	u16 dst_w;
	/* window height */
	u16 dst_h;
	/* window position X */
	u16 dst_x;
	/* window position Y */
	u16 dst_y;
	/* buffer */
	struct rpmsg_kdrv_display_buffer_info buffer;
} __packed;

/* display device commit request */
struct rpmsg_kdrv_display_commit_request {
	/* message header */
	struct rpmsg_kdrv_display_message_header header;
	/*ID of shared display */
	u8 id;
	/* number of pipe updates in the commit */
	u8 num_vid_updates;
	/* list of pipe updates */
	struct rpmsg_kdrv_display_vid_update_info vid[RPMSG_KDRV_TP_DISPLAY_MAX_VIDS];
	/*commit id : to be used in commit-done message */
	u32 commit_id;
} __packed;

/* display device commit response */
struct rpmsg_kdrv_display_commit_response {
	/* message header */
	struct rpmsg_kdrv_display_message_header header;
	/*commit id : from commit request */
	u32 commit_id;
	/*status : 0 = accepted, 1 = rejected */
	u8 status;
} __packed;

/* display device commit done message */
struct rpmsg_kdrv_display_commit_done_message {
	/* message header */
	struct rpmsg_kdrv_display_message_header header;
	/* commit id : from commit request */
	u32 commit_id;
} __packed;

/*display device buffer deferred release message */
struct rpmsg_kdrv_display_buffer_done_message {
	/* message header */
	struct rpmsg_kdrv_display_message_header header;
	/* buffer id: from bufer_info */
	u32 buffer_id;
} __packed;

/* display device request to provide list of shared resources */
struct rpmsg_kdrv_display_res_info_request {
	/* message header */
	struct rpmsg_kdrv_display_message_header header;
} __packed;

/* display device shared pipe */
struct rpmsg_kdrv_display_vid_info {
	/* pipe ID */
	u8 id;
	/* is pipe window fixed on display? */
	u8 mutable_window;
	/* fixed window position X, if applicable */
	u16 fixed_window_x;
	/* fixed window position Y, if applicable */
	u16 fixed_window_y;
	/* fixed window width, if applicable */
	u16 fixed_window_w;
	/* fixed window height, if applicable */
	u16 fixed_window_h;
	/* can pipe scale buffers? */
	u8 can_scale;
	/* number of formats supported */
	u8 num_formats;
	/*enum: rpmsg_kdrv_display_format */
	u8 format[RPMSG_KDRV_TP_DISPLAY_MAX_FORMATS];
	/* initial zorder of pipe */
	u8 init_zorder;
	/* number of allowed zorders */
	u8 num_zorders;
	/* list of allowed zorders */
	u8 zorder[RPMSG_KDRV_TP_DISPLAY_MAX_ZORDERS];
} __packed;

/* display device shared display */
struct rpmsg_kdrv_display_vp_info {
	/* ID of shared display */
	u8 id;
	/* raster width */
	u16 width;
	/* raster height */
	u16 height;
	/* refresh rate */
	u8 refresh;
	/* number of pipes for this display */
	u8 num_vids;
	/* list of pipes */
	struct rpmsg_kdrv_display_vid_info vid[RPMSG_KDRV_TP_DISPLAY_MAX_VIDS];
} __packed;

/* display device response providing list of shared resources */
struct rpmsg_kdrv_display_res_info_response {
	/* message header */
	struct rpmsg_kdrv_display_message_header header;
	/* number of shared displays */
	u8 num_vps;
	/* list of shared displays */
	struct rpmsg_kdrv_display_vp_info vp[RPMSG_KDRV_TP_DISPLAY_MAX_VPS];
} __packed;

#endif
