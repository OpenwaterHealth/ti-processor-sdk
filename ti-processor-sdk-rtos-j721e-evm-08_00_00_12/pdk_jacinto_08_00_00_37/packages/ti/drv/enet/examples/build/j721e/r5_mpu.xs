/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
/*
 *  ======== r5_mpu.xs ========
 *  MPU Settings for J721E device's Cortex-R5F
 */

/*
 *  -------------------------------------------------------------------------------------------------------------
 * | Id | Base Address | Size | En | Cacheable                                 | XN | AccPerm             | Mask |
 * |-------------------------------------------------------------------------------------------------------------|
 * | 0  | 0x00000000   | 4GB  | T  | Strongly Ordered, Shareable               | T  | RW at PL 1          | 0x0  |
 * |----|--------------|------|----|-------------------------------------------|----|---------------------|------|
 * | 1  | 0 (local TCM)| 32K  | T  | Write-Back, Write-Allocate, Shareable     | F  | RW at PL 1          | 0x0  |
 * |----|--------------|------|----|-------------------------------------------|----|---------------------|------|
 * | 2  | 0x41000000   | 32K  | T  | Write-Back, Write-Allocate, Non-Shareable | F  | RW at PL 1          | 0x0  |
 * |----|--------------|------|----|-------------------------------------------|----|---------------------|------|
 * | 3  | 0x41010000   | 32K  | T  | Write-Back, Write-Allocate, Non-Shareable | F  | RW at PL 1          | 0x0  |
 * |----|--------------|------|----|-------------------------------------------|----|---------------------|------|
 * | 4  | 0x41C00000   | 1MB  | T  | Write-Back, Write-Allocate, Non-Shareable | F  | RW at PL 1          | 0x0  |
 * |----|--------------|------|----|-------------------------------------------|----|---------------------|------|
 * | 5  | 0x70000000   | 8MB  | T  | MSMC Ram - Cachable                       | F  | RW at PL 1          | 0x0  |
 * |----|--------------|------|----|-------------------------------------------|----|---------------------|------|
 * | 6  | 0x80000000   | 2GB  | T  | Write-Back, Write-Alloc, Non-Shareable    | F  | RW at PL 1          | 0x0  |
 *  -------------------------------------------------------------------------------------------------------------
 */

/*
 * Note: Marking a region as shareable will cause the region to behave as outer shareable with write through
 *       no write-allocate caching policy irrespective of the actual cache policy set. Therefore, only select
 *       regions that are actually shared outside the R5 CPUSS must be marked as shared.
 */

var MPU = xdc.useModule('ti.sysbios.family.arm.MPU');
MPU.enableMPU = true;
MPU.enableBackgroundRegion = true;

var attrs = new MPU.RegionAttrs();
MPU.initRegionAttrsMeta(attrs);

var index = 0;

/* make all 4G as strongly ordered, non-cacheable */
attrs.enable = true;
attrs.bufferable = false;
attrs.cacheable = false;
attrs.shareable = true;
attrs.noExecute = true;
attrs.accPerm = 1;          /* RW at PL1 */
attrs.tex = 0;
attrs.subregionDisableMask = 0;
MPU.setRegionMeta(index++, 0x00000000, MPU.RegionSize_4G, attrs);

/* make ATCM as cacheable */
attrs.enable = true;
attrs.bufferable = true;
attrs.cacheable = true;
attrs.shareable = false;
attrs.noExecute = false;
attrs.accPerm = 1;          /* RW at PL1 */
attrs.tex = 1;
attrs.subregionDisableMask = 0;
MPU.setRegionMeta(index++, 0x00000000, MPU.RegionSize_32K, attrs);

/* make ATCM as cacheable */
attrs.enable = true;
attrs.bufferable = true;
attrs.cacheable = true;
attrs.shareable = false;
attrs.noExecute = false;
attrs.accPerm = 1;          /* RW at PL1 */
attrs.tex = 1;
attrs.subregionDisableMask = 0;
MPU.setRegionMeta(index++, 0x41000000, MPU.RegionSize_32K, attrs);

/* make BTCM as cacheable */
attrs.enable = true;
attrs.bufferable = true;
attrs.cacheable = true;
attrs.shareable = false;
attrs.noExecute = false;
attrs.accPerm = 1;          /* RW at PL1 */
attrs.tex = 1;
attrs.subregionDisableMask = 0x0;
MPU.setRegionMeta(index++, 0x41010000, MPU.RegionSize_32K, attrs);

/* MCU OCSRAM as cacheable */
attrs.enable = true;
attrs.bufferable = true;
attrs.cacheable = true;
attrs.shareable = false;
attrs.noExecute = false;
attrs.accPerm = 1;          /* RW at PL1 */
attrs.tex = 1;
attrs.subregionDisableMask = 0;
MPU.setRegionMeta(index++, 0x41C00000, MPU.RegionSize_1M, attrs);

/* make all MSMC as cacheable */
attrs.enable = true;
attrs.bufferable = true;
attrs.cacheable = true;
attrs.shareable = false;
attrs.noExecute = false;
attrs.accPerm = 1;          /* RW at PL1 */
attrs.tex = 1;
attrs.subregionDisableMask = 0;
MPU.setRegionMeta(index++, 0x70000000, MPU.RegionSize_8M, attrs);

/* make all 2G DDR as cacheable */
attrs.enable = true;
attrs.bufferable = true;
attrs.cacheable = true;
attrs.shareable = false;
attrs.noExecute = false;
attrs.accPerm = 1;          /* RW at PL1 */
attrs.tex = 1;
attrs.subregionDisableMask = 0;
MPU.setRegionMeta(index++, 0x80000000, MPU.RegionSize_2G, attrs);

/* make APP_LOG_MEM_ADDR, TIOVX_OBJ_DESC_MEM_ADDR, IPC_VRING_MEM_ADDR as non-cache */
/* Note: the next 4 MPU regions start address (second argument of MPU.setRegionMeta)
   must cover the address range of APP_LOG_MEM, TIOVX_OBJ_DESC_MEM, IPC_VRING_MEM
   in system_memory_map.html and MUST be 16M aligned
 */
var non_cache_base_addr = 0xA3000000;
var MB = 0x100000;

attrs.enable = true;
attrs.bufferable = false;
attrs.cacheable = false;
attrs.shareable = true;
attrs.noExecute = true;
attrs.accPerm = 1;          /* RW at PL1 */
attrs.tex = 0;
attrs.subregionDisableMask = 0;
MPU.setRegionMeta(index++, non_cache_base_addr + 0*16*MB, MPU.RegionSize_16M, attrs);
MPU.setRegionMeta(index++, non_cache_base_addr + 1*16*MB, MPU.RegionSize_16M, attrs);
MPU.setRegionMeta(index++, non_cache_base_addr + 2*16*MB, MPU.RegionSize_16M, attrs);
MPU.setRegionMeta(index++, non_cache_base_addr + 3*16*MB, MPU.RegionSize_16M, attrs);

xdc.print("# MPU setup for " + index + " entries !!!");
