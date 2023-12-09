/*
 * DMSC firmware
 *
 * Cortex-M3 (CM3) firmware for power management
 *
 * Copyright (C) 2017-2021, Texas Instruments Incorporated
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

#include <sleep.h>
#include <types/array_size.h>
#include <dmsc.h>
#include <soc/am64x/devices.h>
#include <lib/ioremap.h>
#include <sys-reset.h>
#include <device_pm.h>

#define MAIN_CTRL_BASE  0x43000000UL

#define CTRL_MMR0_PARTITION_SIZE                        0x4000

#define CTRLMMR_MAIN_RST_CTRL                            0x18170U
#define CTRLMMR_MAIN_RST_CTRL_SW_MAIN_WARMRST_OFFSET     0U
#define CTRLMMR_MAIN_RST_CTRL_SW_MAIN_WARMRST_MASK       0xFU
#define CTRLMMR_MAIN_RST_CTRL_SW_MAIN_WARMRST_VAL        0x6U

static const struct sleep_mode am64x_sleep_modes[] = {
	{
		.name = (char *) "WFI",
	},
};

static u8 am64x_sleep_block[ARRAY_SIZE(am64x_sleep_modes)];

static s32 am64x_sys_reset_handler(domgrp_t domain __attribute__((unused)))
{
	u32 v;

	/* Issue warm reset */
	/*
	 * We are using MAIN CTRL_MMR here as MCU CTRL_MMR won't
	 * be accessible from DMSC in case MCU domain isolation is
	 * enabled.
	 * Also, in AM64x when not in MCU isolation, resets from MAIN
	 * domain also propagates through MCU domain.
	 */
	v = readl(MAIN_CTRL_BASE + CTRLMMR_MAIN_RST_CTRL);
	v &= ~(CTRLMMR_MAIN_RST_CTRL_SW_MAIN_WARMRST_MASK <<
	       CTRLMMR_MAIN_RST_CTRL_SW_MAIN_WARMRST_OFFSET);
	v |= CTRLMMR_MAIN_RST_CTRL_SW_MAIN_WARMRST_VAL <<
	     CTRLMMR_MAIN_RST_CTRL_SW_MAIN_WARMRST_OFFSET;
	writel(v, MAIN_CTRL_BASE + CTRLMMR_MAIN_RST_CTRL);

	return SUCCESS;
}

s32 dmsc_init(void)
{
	sys_reset_handler_register(am64x_sys_reset_handler);

	return sleep_modes_register(am64x_sleep_modes, am64x_sleep_block,
				    ARRAY_SIZE(am64x_sleep_modes));
}
