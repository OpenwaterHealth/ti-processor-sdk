/*
 * DMSC firmware
 *
 * Cortex-M3 (CM3) firmware for power management
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

#include <types/array_size.h>
#include <lib/ioremap.h>
#include <lib/mmr_lock.h>
#include <lib/trace.h>
#include <osal/osal_hwi.h>

#define MAIN_CTRL_BASE                          (0x43000000UL)
#define MCU_CTRL_BASE                           (0x4500000UL)

#define CTRL_MMR0_PARTITION_SIZE                (0x4000U)

#define CTRLMMR_LOCK_KICK0                      (0x01008U)
#define CTRLMMR_LOCK_KICK0_UNLOCK_VAL           (0x68ef3490U)
#define CTRLMMR_LOCK_KICK0_LOCK_VAL             (0x0U)
#define CTRLMMR_LOCK_KICK0_UNLOCKED_MASK        BIT(0)
#define CTRLMMR_LOCK_KICK0_UNLOCKED_SHIFT       (0U)
#define CTRLMMR_LOCK_KICK1                      (0x0100cU)
#define CTRLMMR_LOCK_KICK1_UNLOCK_VAL           (0xd172bc5aU)
#define CTRLMMR_LOCK_KICK1_LOCK_VAL             (0x0U)

static volatile s32 mmr_lock_count = 0;
static void mmr_writel(u32 v, u32 a)
{
	writel(v, 0x60000000 + a);
}

static void mmr_unlock(u32 base, u32 partition)
{
	/* Translate the base address */
	u32 part_base = base + (partition * CTRL_MMR0_PARTITION_SIZE);

	/*
	 * Unlock the requested partition if locked using two-step sequence.
	 * This register does not read back the value written.
	 */
	mmr_writel(CTRLMMR_LOCK_KICK0_UNLOCK_VAL, part_base + CTRLMMR_LOCK_KICK0);
	mmr_writel(CTRLMMR_LOCK_KICK1_UNLOCK_VAL, part_base + CTRLMMR_LOCK_KICK1);
}

static void mmr_lock(u32 base, u32 partition)
{
	/* Translate the base address */
	u32 part_base = base + (partition * CTRL_MMR0_PARTITION_SIZE);

	/*
	 * Unlock the requested partition if locked using two-step sequence.
	 * This register does not read back the value written.
	 */
	mmr_writel(CTRLMMR_LOCK_KICK0_LOCK_VAL, part_base + CTRLMMR_LOCK_KICK0);
	mmr_writel(CTRLMMR_LOCK_KICK1_LOCK_VAL, part_base + CTRLMMR_LOCK_KICK1);
}

void mmr_unlock_all()
{
	u32 key = osal_hwip_disable();

	if (mmr_lock_count == 0) {
		/* Unlock CLKSEL MMR partitions */
		mmr_unlock(MAIN_CTRL_BASE, 2);
		/* Unlock RST_CTRL MMR partitions*/
		mmr_unlock(MAIN_CTRL_BASE, 6);
		/*
		 * We may not even have access to MCU island(warm reset) to lock the CTRL_BASE..
		 * However, we expect MCU island to self manage.
		 * TBD....
		 */
		mmr_unlock(MCU_CTRL_BASE, 2);
		mmr_unlock(MCU_CTRL_BASE, 6);
	}
	mmr_lock_count++;
	osal_hwip_restore(key);
}

void mmr_lock_all()
{
	u32 key = osal_hwip_disable();

	mmr_lock_count--;
	if (mmr_lock_count == 0) {
		/* Lock CLKSEL MMR partitions */
		mmr_lock(MAIN_CTRL_BASE, 2);
		/* lock RST_CTRL MMR partitions*/
		mmr_lock(MAIN_CTRL_BASE, 6);
		/*
		 * We may not even have access to MCU island(warm reset) to lock the CTRL_BASE..
		 * However, we expect MCU island to self manage. if we have to control
		 * MCU island is "unsafe".
		 * TBD....
		 */
		mmr_lock(MCU_CTRL_BASE, 2);
		mmr_lock(MCU_CTRL_BASE, 6);
	}
	osal_hwip_restore(key);
}
