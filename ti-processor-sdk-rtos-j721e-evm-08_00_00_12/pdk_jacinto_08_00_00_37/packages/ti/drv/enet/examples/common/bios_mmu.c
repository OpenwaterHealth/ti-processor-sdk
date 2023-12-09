/*
 *  Copyright (c) Texas Instruments Incorporated 2020
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

/*!
 * \file     bios_mmu.c
 *
 * \brief    This file has the common MMU setting function for A53/A72.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdbool.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#if defined(BUILD_MPU)
#include <ti/sysbios/family/arm/v8a/Mmu.h>
#endif

#include <ti/csl/soc.h>
#include <ti/drv/uart/UART_stdio.h>

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

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void EnetAppUtils_waitForEmulator(bool enable)
{
    volatile bool enableDebug = enable;

    while (enableDebug)
    {
        /* Do nothing */
    }
}

void EnetAppUtils_initMmu(void)
{
    Mmu_MapAttrs attrs;
    uint32_t mapIdx = 0U;
    bool retVal;

    /* This is for debug purpose */
    EnetAppUtils_waitForEmulator(false);

    Mmu_initMapAttrs(&attrs);

    attrs.attrIndx = Mmu_AttrIndx_MAIR0;
    retVal         = Mmu_map(0x00000000,
                             0x00000000,
                             0x20000000,
                             &attrs);

    /* Main MMR0 cfg */
    if (retVal == TRUE)
    {
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_CTRL_MMR0_CFG0_BASE, /* 0x10_0000 */
                                 CSL_CTRL_MMR0_CFG0_BASE,
                                 0x00900000,
                                 &attrs);
    }

    /* PSC0 */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_PSC0_BASE, /* 0x40_0000u */
                                 CSL_PSC0_BASE,
                                 CSL_PSC0_SIZE,
                                 &attrs);
    }

    /* gicv3 */
    if (retVal == TRUE)
    {
        uint32_t addr = 0U;
        uint32_t size = 0U;

#if defined (SOC_J721E) || defined (SOC_J7200)
        addr = CSL_COMPUTE_CLUSTER0_GIC_DISTRIBUTOR_BASE;     /* 0x180_0000 */
        size = CSL_COMPUTE_CLUSTER0_GIC_DISTRIBUTOR_SIZE;
#elif SOC_AM65XX
        addr = CSL_GIC0_DISTRIBUTOR_BASE;     /* 0x180_0000u */
        size = CSL_GIC0_DISTRIBUTOR_SIZE;
#else
        EnetAppUtils_assert(FALSE);
#endif

        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(addr,
                                 addr,
                                 size,
                                 &attrs);
    }

    /* Timers */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_TIMER0_CFG_BASE, /* 0x240_0000 */
                                 CSL_TIMER0_CFG_BASE,
                                 0x000c0000,
                                 &attrs);
    }

    /* UART */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_UART0_BASE, /* 0x280_0000 */
                                 CSL_UART0_BASE,
                                 0x00040000,
                                 &attrs);
    }

    /* I2C */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_I2C0_CFG_BASE, /* 0x20_00000 */
                                 CSL_I2C0_CFG_BASE,
                                 0x00100000,
                                 &attrs);
    }

    /* I2C  WKUP domain */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_WKUP_I2C0_CFG_BASE, /* 0x20_00000 */
                                 CSL_WKUP_I2C0_CFG_BASE,
                                 0x00100000,
                                 &attrs);
    }

    /* McSPI */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_MCSPI0_CFG_BASE, /* 0x210_0000 */
                                 CSL_MCSPI0_CFG_BASE,
                                 0x00080000,
                                 &attrs);
    }

    /* MCU MMR0 CFG */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_MCU_CTRL_MMR0_CFG0_BASE, /* 0x40f0_0000 */
                                 CSL_MCU_CTRL_MMR0_CFG0_BASE,
                                 CSL_MCU_CTRL_MMR0_CFG0_SIZE,
                                 &attrs);
    }

    /* PLL0 CFG */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_MCU_PLL0_CFG_BASE, /* 0x40d0_0000 */
                                 CSL_MCU_PLL0_CFG_BASE,
                                 CSL_MCU_PLL0_CFG_SIZE,
                                 &attrs);
    }

    /* WKUP MMR0 cfg */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_WKUP_CTRL_MMR0_CFG0_BASE, /* 0x430_00000 */
                                 CSL_WKUP_CTRL_MMR0_CFG0_BASE,
                                 CSL_WKUP_CTRL_MMR0_CFG0_SIZE,
                                 &attrs);
    }

    /* pinmux ctrl */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(0x02C40000,
                                 0x02C40000,
                                 0x00100000,
                                 &attrs);
    }

    /* pinmux ctrl */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(0x02A430000,
                                 0x02A430000,
                                 0x00001000,
                                 &attrs);
    }

    /* Main NAVSS */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(0x30800000,
                                 0x30800000,
                                 0x0C000000,
                                 &attrs);
    }

    /* MCU NAVSS */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(0x28380000,
                                 0x28380000,
                                 0x03880000,
                                 &attrs);
    }

    /* DRU */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(0x6D000000,
                                 0x6D000000,
                                 0x1000000,
                                 &attrs);
    }

    /* ctcontro10 */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(0x30000000,
                                 0x30000000,
                                 0x0F000000,
                                 &attrs);
    }

    /* PSC WKUP */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_WKUP_PSC0_BASE, /* 0x420_00000 */
                                 CSL_WKUP_PSC0_BASE,
                                 CSL_WKUP_PSC0_SIZE,
                                 &attrs);
    }

    /* CPSW2G */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_MCU_CPSW0_NUSS_BASE, /* 0x460_00000 */
                                 CSL_MCU_CPSW0_NUSS_BASE,
                                 CSL_MCU_CPSW0_NUSS_SIZE,
                                 &attrs);
    }

#if defined (SOC_J721E) || defined (SOC_J7200)
    /* CPSW9G & CPSW5G */
    if (retVal == TRUE)
    {
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_CPSW0_NUSS_BASE, /* 0xc0_00000 */
                                 CSL_CPSW0_NUSS_BASE,
                                 CSL_CPSW0_NUSS_SIZE,
                                 &attrs);
    }
#endif

    /* ICSS-G 0 */
    if (retVal == TRUE)
    {
#if !defined (SOC_J7200)
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_PRU_ICSSG0_DRAM0_SLV_RAM_BASE, /* 0xb0_00000 */
                                 CSL_PRU_ICSSG0_DRAM0_SLV_RAM_BASE,
                                 0x0100000,
                                 &attrs);
#endif
    }

    /* ICSS-G 1 */
    if (retVal == TRUE)
    {
#if !defined (SOC_J7200)
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_PRU_ICSSG1_DRAM0_SLV_RAM_BASE, /* 0xb10_0000 */
                                 CSL_PRU_ICSSG1_DRAM0_SLV_RAM_BASE,
                                 0x0100000,
                                 &attrs);
#endif
    }

    /* ICSS-G 2 */
    if (retVal == TRUE)
    {
#ifdef SOC_AM65XX
        mapIdx++;
        attrs.attrIndx = Mmu_AttrIndx_MAIR0;
        retVal         = Mmu_map(CSL_PRU_ICSSG2_DRAM0_SLV_RAM_BASE, /* 0xb20_0000u */
                                 CSL_PRU_ICSSG2_DRAM0_SLV_RAM_BASE,
                                 0x0100000,
                                 &attrs);
#endif
    }

#define DDR_TEST
#ifdef DDR_TEST
    /* ddr_0 */
    if (retVal == TRUE)
    {
        attrs.attrIndx = Mmu_AttrIndx_MAIR7;
        mapIdx++;
        retVal = Mmu_map(0x80000000, 0x80000000, 0x20000000, &attrs);
    }

    if (retVal == TRUE)
    {
        attrs.attrIndx = Mmu_AttrIndx_MAIR7;
        mapIdx++;
        retVal = Mmu_map(0xA0000000, 0xA0000000, 0x10000000, &attrs);
    }
#endif

    /* MSMC */
    if (retVal == TRUE)
    {
        attrs.attrIndx = Mmu_AttrIndx_MAIR7;
        mapIdx++;
        retVal = Mmu_map(CSL_COMPUTE_CLUSTER0_MSMC_SRAM_BASE /* 0x70_000000u */,
                         CSL_COMPUTE_CLUSTER0_MSMC_SRAM_BASE,
                         CSL_COMPUTE_CLUSTER0_MSMC_SRAM_SIZE,
                         &attrs);
    }

    /* OCMC */
    if (retVal == TRUE)
    {
        uint32_t ramBaseAddr = 0U;
        uint32_t ramSize     = 0U;

#if defined (SOC_J721E) || defined (SOC_J7200)
        ramBaseAddr = CSL_MCU_MSRAM_1MB0_RAM_BASE; /* 0x41c0_0000 */
        ramSize     = CSL_MCU_MSRAM_1MB0_RAM_SIZE;
#elif SOC_AM65XX
        ramBaseAddr = CSL_MCU_MSRAM0_RAM_BASE;     /* 0x41c0_0000u */
        ramSize     = CSL_MCU_MSRAM0_RAM_SIZE;
#else
        EnetAppUtils_assert(FALSE);
#endif

        attrs.attrIndx = Mmu_AttrIndx_MAIR7;
        mapIdx++;
        retVal = Mmu_map(ramBaseAddr,
                         ramBaseAddr,
                         ramSize,
                         &attrs);
    }

    if (retVal == FALSE)
    {
        UART_printf("Mmu_map idx %d returned error %d", mapIdx, retVal);
        EnetAppUtils_waitForEmulator(true);
    }
}
