/*
 * PSI-L
 *
 * PSI-L driver instance data definition
 *
 * Data version: 200505_104840
 *
 * Copyright (C) 2019-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <config.h>
#include <types/short_types.h>
#include <types/sbool.h>
#include <types/array_size.h>
#include <soc/rm_mmr.h>
#include <devices.h>
#include <devices_internal.h>
#include <soc/am64x/soc_devgrps.h>

#include "../../include/psil_inst.h"
#include "psil_cfg.h"

static const struct rm_mmr mmr_DMASS0_PSILCFG_0_PSILCFG_PROXY = {
	.fwl_id = RM_MMR_FWL_INVALID,
	.base	= 0x48130000U,
};

static const struct psil_thread_type src_thread_types[] = {
	{
		.start = 0x1000U,
		.end = 0x1029U,
		.dev_id = AM64X_DEV_DMASS0_PKTDMA_0,
	},
	{
		.start = 0x2000U,
		.end = 0x2013U,
		.dev_id = AM64X_DEV_DMASS0_BCDMA_0,
	},
	{
		.start = 0x4000U,
		.end = 0x4003U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x4100U,
		.end = 0x4104U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x4200U,
		.end = 0x4204U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x4300U,
		.end = 0x4303U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x4304U,
		.end = 0x4307U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x4308U,
		.end = 0x430BU,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x430CU,
		.end = 0x430FU,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x4310U,
		.end = 0x4310U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x4311U,
		.end = 0x4311U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x4400U,
		.end = 0x4403U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x4404U,
		.end = 0x4404U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x4405U,
		.end = 0x4405U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x4406U,
		.end = 0x4406U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x4407U,
		.end = 0x4407U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x4408U,
		.end = 0x4408U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x4409U,
		.end = 0x440BU,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x440CU,
		.end = 0x440EU,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x440FU,
		.end = 0x440FU,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0x4500U,
		.end = 0x4500U,
		.dev_id = SOC_DEV_NONE,
	},
};

static const struct psil_thread_type dst_thread_types[] = {
	{
		.start = 0x9000U,
		.end = 0x901CU,
		.dev_id = AM64X_DEV_DMASS0_PKTDMA_0,
	},
	{
		.start = 0xA000U,
		.end = 0xA013U,
		.dev_id = AM64X_DEV_DMASS0_BCDMA_0,
	},
	{
		.start = 0xC000U,
		.end = 0xC001U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC100U,
		.end = 0xC108U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC200U,
		.end = 0xC208U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC300U,
		.end = 0xC303U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC304U,
		.end = 0xC307U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC308U,
		.end = 0xC30BU,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC30CU,
		.end = 0xC30FU,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC310U,
		.end = 0xC310U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC311U,
		.end = 0xC311U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC400U,
		.end = 0xC403U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC404U,
		.end = 0xC404U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC405U,
		.end = 0xC405U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC406U,
		.end = 0xC406U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC407U,
		.end = 0xC407U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC408U,
		.end = 0xC408U,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC409U,
		.end = 0xC40BU,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC40CU,
		.end = 0xC40EU,
		.dev_id = SOC_DEV_NONE,
	},
	{
		.start = 0xC500U,
		.end = 0xC507U,
		.dev_id = SOC_DEV_NONE,
	},
};

struct psil_instance psil_inst[] = {
	{
		.id = AM64X_DEV_DMASS0,
		.devgrp = SOC_DEVGRP_AM64X_MAIN,
		.proxy = &mmr_DMASS0_PSILCFG_0_PSILCFG_PROXY,
		.src_thread_types = &src_thread_types[0],
		.dst_thread_types = &dst_thread_types[0],
		.n_src_thread_type = ARRAY_SIZE(src_thread_types),
		.n_dst_thread_type = ARRAY_SIZE(dst_thread_types),
		.dru_ch0_dst_thread_offset = 0x0U,
	},
};

const u32 psil_inst_count = ARRAY_SIZE(psil_inst);

/*
 * Threads used by ROM which must be disabled prior to reuse
 */
#if !defined(CONFIG_DEVICE_TYPE_HS)
struct psil_used_thread psil_rom_usage[PSIL_ROM_USAGE_CNT] = {
	/*
	 * Threads used on devices other than HS device
	 */
	{
		.thread = 0x1018U,
		.disabled = SFALSE,
	},
	{
		.thread = 0x4000U,
		.disabled = SFALSE,
	},
	{
		.thread = 0x4001U,
		.disabled = SFALSE,
	},
	{
		.thread = 0x9011U,
		.disabled = SFALSE,
	},
	{
		.thread = 0x9012U,
		.disabled = SFALSE,
	},
	{
		.thread = 0xc000U,
		.disabled = SFALSE,
	},
};
#else
struct psil_used_thread psil_rom_usage[PSIL_ROM_USAGE_CNT] = {
};
#endif
