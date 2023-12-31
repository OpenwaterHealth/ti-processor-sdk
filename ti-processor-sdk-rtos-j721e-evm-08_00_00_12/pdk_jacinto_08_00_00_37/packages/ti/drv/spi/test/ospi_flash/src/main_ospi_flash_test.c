/*
 * Copyright (C) 2017 - 2021 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 *  \file   main_ospi_flash_test.c
 *
 *  \brief  Test application main file. This application will write and read
 *          the data to/from OSPI NOR flash through Board flash interface.
 *
 */
#include <stdint.h>
#include <stdio.h>

#if defined(OSPI_TESTAPP_RTOS)
#include "ti/osal/osal.h"
#include "ti/osal/TaskP.h"
#if defined(OSPI_TESTAPP_TIRTOS)
#include <ti/sysbios/utils/Load.h>
#endif
#if defined(SOC_AM65XX) || defined(SOC_J721E) || defined(SOC_J7200) || defined(SOC_AM64X)
#if defined (__aarch64__)
#include <ti/sysbios/family/arm/v8a/Mmu.h>
#endif
#endif
#endif /* #if defined(OSPI_TESTAPP_RTOS) */

/* TI-RTOS Header files */
#include <ti/drv/spi/SPI.h>
#include <ti/drv/spi/src/SPI_osal.h>
#include <ti/drv/spi/soc/SPI_soc.h>
#include <ti/drv/spi/test/src/SPI_log.h>

#include <ti/board/board.h>
#include <ti/board/board_cfg.h>
#include <ti/board/src/flash/include/board_flash.h>
#include <ti/board/src/flash/nor/ospi/nor_spi_patterns.h>
#if defined(SOC_J7200) || defined(SOC_AM64X)
#include <ti/board/src/flash/nor/ospi/nor_xspi.h>
#else
#include <ti/board/src/flash/nor/ospi/nor_ospi.h>
#endif

#ifdef SPI_DMA_ENABLE
#include <ti/osal/CacheP.h>
#endif

#if defined(SOC_AM65XX) || defined(SOC_J721E) || defined(SOC_J7200) || defined(SOC_AM64X)
#include <ti/csl/cslr_fss.h>
#include <ti/csl/csl_rat.h>
#include <ti/csl/arch/csl_arch.h>
#include <ti/drv/sciclient/sciclient.h>
#ifdef SPI_DMA_ENABLE
#include <ti/drv/udma/udma.h>
#endif
#if defined(SOC_AM64X) || defined(SOC_J7200)
#include <ti/board/src/flash/nor/device/s28hs512t.h>
#else
#include <ti/board/src/flash/nor/device/m35xu512.h>
#endif

#if defined(SOC_AM65XX)
#include <ti/csl/soc/am65xx/src/cslr_soc_baseaddress.h>
#include <ti/csl/soc/am65xx/src/cslr_mcu_ctrl_mmr.h>
#include <ti/csl/soc/am65xx/src/cslr_mcu_pll_mmr.h>
#include <ti/csl/soc/am65xx/src/cslr_wkup_ctrl_mmr.h>
#endif

#if defined(SOC_J721E)
#include <ti/csl/soc/j721e/src/cslr_soc_baseaddress.h>
#include <ti/csl/soc/j721e/src/cslr_mcu_ctrl_mmr.h>
#include <ti/csl/soc/j721e/src/cslr_mcu_pll_mmr.h>
#include <ti/csl/soc/j721e/src/cslr_wkup_ctrl_mmr.h>
#endif

#if defined(SOC_J7200)
#include <ti/csl/soc/j7200/src/cslr_soc_baseaddress.h>
#include <ti/csl/soc/j7200/src/cslr_mcu_ctrl_mmr.h>
#include <ti/csl/soc/j7200/src/cslr_mcu_pll_mmr.h>
#include <ti/csl/soc/j7200/src/cslr_wkup_ctrl_mmr.h>
#endif

#endif


/* Define the OSPI test interface */
typedef struct OSPI_Tests_s
{
    bool     (*testFunc)(void *);
    int32_t  testId;
    bool     dacMode;
    bool     dmaMode;
    uint32_t clk;
    char     testDesc[80];

} OSPI_Tests;

/**********************************************************************
 ************************** Macros ************************************
 **********************************************************************/
#define OSPI_PROFILE        /* Enable profiling */
#define OSPI_WRITE          /* Enable write */
#define OSPI_WRITE_TUNING   /* Enable write tuning data pattern to the falsh */
#undef  OSPI_QSPI_FLASH     /* Enable QSPI flash test using OSPI controller */

/* OSPI test ID definitions */
#define OSPI_TEST_ID_DAC_133M     0   /* OSPI flash test in Direct Acess Controller mode at 133MHz RCLK */
#define OSPI_TEST_ID_INDAC_133M   1   /* OSPI flash test in Indirect Acess Controller mode at 133MHz RCLK */
#define OSPI_TEST_ID_DAC_DMA_133M 2   /* OSPI flash test in Direct Acess Controller DMA mode at 133MHz RCLK */
#define OSPI_TEST_ID_DAC_166M     3   /* OSPI flash test in Direct Acess Controller mode at 166MHz RCLK */
#define OSPI_TEST_ID_INDAC_166M   4   /* OSPI flash test in Indirect Acess Controller mode at 166MHz RCLK */
#define OSPI_TEST_ID_DAC_DMA_166M 5   /* OSPI flash test in Direct Acess Controller DMA mode at 166MHz RCLK */
#define OSPI_TEST_ID_DAC_133M_SPI 6   /* OSPI flash test in Direct Acess Controller legacy SPI mode at 133MHz RCLK */
#define OSPI_TEST_ID_WR_TUNING    7   /* OSPI flash test in Direct Acess Controller legacy SPI mode to write tuning data */

/* OSPI NOR flash offset address for read/write test */
#define TEST_ADDR_OFFSET   (0U)

/* Test read/write buffer length in bytes */
#define TEST_BUF_LEN       (0x100000U)

/* Total read/write length in bytes */
#define TEST_DATA_LEN      (TEST_BUF_LEN)

/* Read/write sengment length in bytes */
#define TEST_XFER_SEG_LEN  (TEST_BUF_LEN)

/* Flash offset where the PHY tuning data pattern stored */
#define TEST_TUNE_PATTERN_OFFSET  (NOR_TUNING_DATA_OFFSET)

/**********************************************************************
 ************************** Internal functions ************************
 **********************************************************************/

/* Function to generate known data */
static void GeneratePattern(uint8_t *txBuf, uint8_t *rxBuf, uint32_t length);

/* Data compare function */
bool VerifyData(uint8_t *expData,
                uint8_t *rxData,
                uint32_t length);

/**********************************************************************
 ************************** Global Variables **************************
 **********************************************************************/
#if defined(OSPI_TESTAPP_RTOS)
/* Test application stack */
#define APP_TSK_STACK_MAIN              (16U * 1024U)
static uint8_t  gAppTskStackMain[APP_TSK_STACK_MAIN] __attribute__((aligned(32)));;
#endif

/* Buffer containing the known data that needs to be written to flash */
#if defined(SOC_AM65XX) || defined(SOC_AM64X)
#ifdef SPI_DMA_ENABLE
uint8_t txBuf[TEST_BUF_LEN]  __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT))) __attribute__((section(".benchmark_buffer")));
#else
uint8_t txBuf[TEST_BUF_LEN]  __attribute__((aligned(128))) __attribute__((section(".benchmark_buffer")));
#endif
#endif

#if defined(SOC_J721E) || defined(SOC_J7200)
#ifdef SPI_DMA_ENABLE
uint8_t txBuf[TEST_BUF_LEN]  __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT))) __attribute__((section(".benchmark_buffer")));
#else
uint8_t txBuf[TEST_BUF_LEN]  __attribute__((aligned(128))) __attribute__((section(".benchmark_buffer")));
#endif
#endif

/* Buffer containing the received data */
#if defined(SOC_AM65XX) || defined(SOC_AM64X)
#ifdef SPI_DMA_ENABLE
uint8_t rxBuf[TEST_BUF_LEN]  __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT))) __attribute__((section(".benchmark_buffer")));
#else
uint8_t rxBuf[TEST_BUF_LEN]  __attribute__((aligned(128))) __attribute__((section(".benchmark_buffer")));
#endif
#endif

#if defined(SOC_J721E) || defined(SOC_J7200)
#ifdef SPI_DMA_ENABLE
uint8_t rxBuf[TEST_BUF_LEN]  __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT))) __attribute__((section(".benchmark_buffer")));
#else
uint8_t rxBuf[TEST_BUF_LEN]  __attribute__((aligned(128))) __attribute__((section(".benchmark_buffer")));
#endif
#endif

#if defined(SOC_AM65XX) || defined(SOC_J721E) || defined(SOC_J7200) || defined(SOC_AM64X)

#ifdef SPI_DMA_ENABLE
/*
 * Ring parameters
 */
/** \brief Number of ring entries - we can prime this much memcpy operations */
#define UDMA_TEST_APP_RING_ENTRIES      (1U)
/** \brief Size (in bytes) of each ring entry (Size of pointer - 64-bit) */
#define UDMA_TEST_APP_RING_ENTRY_SIZE   (sizeof(uint64_t))
/** \brief Total ring memory */
#define UDMA_TEST_APP_RING_MEM_SIZE     (UDMA_TEST_APP_RING_ENTRIES * \
                                         UDMA_TEST_APP_RING_ENTRY_SIZE)
/** \brief This ensures every channel memory is aligned */
#define UDMA_TEST_APP_RING_MEM_SIZE_ALIGN ((UDMA_TEST_APP_RING_MEM_SIZE + UDMA_CACHELINE_ALIGNMENT) & ~(UDMA_CACHELINE_ALIGNMENT - 1U))
/**
 *  \brief UDMA TR packet descriptor memory.
 *  This contains the CSL_UdmapCppi5TRPD + Padding to sizeof(CSL_UdmapTR15) +
 *  one Type_15 TR (CSL_UdmapTR15) + one TR response of 4 bytes.
 *  Since CSL_UdmapCppi5TRPD is less than CSL_UdmapTR15, size is just two times
 *  CSL_UdmapTR15 for alignment.
 */
#define UDMA_TEST_APP_TRPD_SIZE         ((sizeof(CSL_UdmapTR15) * 2U) + 4U)

/** \brief This ensures every channel memory is aligned */
#define UDMA_TEST_APP_TRPD_SIZE_ALIGN   ((UDMA_TEST_APP_TRPD_SIZE + UDMA_CACHELINE_ALIGNMENT) & ~(UDMA_CACHELINE_ALIGNMENT - 1U))

/*
 * UDMA driver objects
 */
struct Udma_DrvObj      gUdmaDrvObj;
struct Udma_ChObj       gUdmaChObj;
struct Udma_EventObj    gUdmaCqEventObj;

Udma_DrvHandle          gDrvHandle = NULL;
/*
 * UDMA Memories
 */
static uint8_t gTxRingMem[UDMA_TEST_APP_RING_MEM_SIZE_ALIGN] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gTxCompRingMem[UDMA_TEST_APP_RING_MEM_SIZE_ALIGN] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gTxTdCompRingMem[UDMA_TEST_APP_RING_MEM_SIZE_ALIGN] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gUdmaTprdMem[UDMA_TEST_APP_TRPD_SIZE_ALIGN] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static OSPI_dmaInfo gUdmaInfo;

static void App_print(const char *str)
{
    UART_printf("%s", str);
    return;
}

int32_t Ospi_udma_init(OSPI_v0_HwAttrs *cfg)
{
    int32_t         retVal = UDMA_SOK;
    Udma_InitPrms   initPrms;
    uint32_t        instId;

    if (gDrvHandle == NULL)
    {
        /* UDMA driver init */
#if defined (SOC_AM64X)
        /* Use Block Copy DMA instance for AM64x */
        instId = UDMA_INST_ID_BCDMA_0;
#else
        /* Use MCU NAVSS for MCU domain cores. Rest cores all uses Main NAVSS */
#if defined (BUILD_MCU1_0) || defined (BUILD_MCU1_1)
        instId = UDMA_INST_ID_MCU_0;
#else
        instId = UDMA_INST_ID_MAIN_0;
#endif
#endif

        UdmaInitPrms_init(instId, &initPrms);
        initPrms.printFxn = &App_print;
        retVal = Udma_init(&gUdmaDrvObj, &initPrms);
        if(UDMA_SOK == retVal)
        {
            gDrvHandle = &gUdmaDrvObj;
        }
    }

    if(gDrvHandle != NULL)
    {
        gUdmaInfo.drvHandle      = (void *)gDrvHandle;
        gUdmaInfo.chHandle       = (void *)&gUdmaChObj;
        gUdmaInfo.ringMem        = (void *)&gTxRingMem[0];
        gUdmaInfo.cqRingMem      = (void *)&gTxCompRingMem[0];
        gUdmaInfo.tdCqRingMem    = (void *)&gTxTdCompRingMem[0];
        gUdmaInfo.tprdMem        = (void *)&gUdmaTprdMem[0];
        gUdmaInfo.eventHandle    = (void *)&gUdmaCqEventObj;
        cfg->dmaInfo             = &gUdmaInfo;
    }

    return (retVal);
}

int32_t Ospi_udma_deinit(void)
{
    int32_t         retVal = UDMA_SOK;

    if (gDrvHandle != NULL)
    {
        retVal = Udma_deinit(gDrvHandle);
        if(UDMA_SOK == retVal)
        {
            gDrvHandle = NULL;
        }
    }

    return (retVal);
}
#endif

#if defined(OSPI_TESTAPP_RTOS)

#if defined(BUILD_MPU) || defined (__C7100__)
extern void Osal_initMmuDefault(void);
void InitMmu(void)
{
    Osal_initMmuDefault();
}
#endif

#endif
#endif

#if defined(SOC_AM65XX)
/* define the unlock and lock values */
#define KICK0_UNLOCK_VAL 0x68EF3490
#define KICK1_UNLOCK_VAL 0xD172BC5A
#define KICK_LOCK_VAL    0x00000000

//
#define MAIN_MMR_BASE_ADDRESS   CSL_CTRL_MMR0_CFG0_BASE
#define MCU_MMR_BASE_ADDRESS    CSL_MCU_CTRL_MMR0_CFG0_BASE
#define WKUP_MMR_BASE_ADDRESS   CSL_WKUP_CTRL_MMR0_CFG0_BASE

#define CSL2PTR (uint32_t *)(uintptr_t)

uint32_t MMR_unlock_one(uint32_t * kick0, uint32_t * kick1)
{
    // initialize the status variable
    uint32_t status = 1;

    // if either of the kick lock registers are locked
    if (!(*kick0 & 0x1) | !(*kick1 & 0x1))
    {
        // unlock the partition by writing the unlock values to the kick lock registers
        *kick0 = KICK0_UNLOCK_VAL;
        *kick1 = KICK1_UNLOCK_VAL;
    }

    // check to see if either of the kick registers are unlocked.
    if (!(*kick0 & 0x1))
    {
        status = 0;
    }

    // return the status to the calling program
    return status;

}

uint32_t MMR_lock_one(uint32_t * kick0, uint32_t * kick1)
{
    // create status return variable
    uint32_t status = 1;

    // check to see if either of the kick registers are unlocked.
    if ((*kick0 & 0x1))
    {
        // write the kick lock value to the kick lock registers to lock the partition
        *kick0 = KICK_LOCK_VAL;
        *kick1 = KICK_LOCK_VAL;
    }

    // check to see if either of the kick registers are still unlocked.
    if ((*kick0 & 0x1))
    {
        status = 0;
    }
    // return success or failure
    return status;
}

uint32_t MCU_CTRL_MMR_unlock_all()
{

    // initialize the status variable
    uint32_t status = 1;

    // Unlock the 0th partition
    status &= MMR_unlock_one(
            CSL2PTR ( MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK0_KICK0),
            CSL2PTR ( MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK0_KICK1));

    // Unlock the 1st partition
    status &= MMR_unlock_one(
            CSL2PTR ( MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK1_KICK0),
            CSL2PTR ( MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK1_KICK1));

    // Unlock the 2nd partition
    status &= MMR_unlock_one(
            CSL2PTR ( MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK2_KICK0),
            CSL2PTR ( MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK2_KICK1));

    // Unlock the 3rd partition
    status &= MMR_unlock_one(
            CSL2PTR ( MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK3_KICK0),
            CSL2PTR ( MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK3_KICK1));

    // Unlock the 4th partition
    status &= MMR_unlock_one(
            CSL2PTR ( MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK4_KICK0),
            CSL2PTR ( MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK4_KICK1));
    return status;
}

uint32_t MCU_CTRL_MMR_lock_all()
{

    // initialize the status variable
    uint32_t status = 1;

    // lock the 0th partition
    status &= MMR_lock_one(
            CSL2PTR (MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK0_KICK0),
            CSL2PTR (MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK0_KICK1));

    // lock the 1st partition
    status &= MMR_lock_one(
            CSL2PTR (MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK1_KICK0),
            CSL2PTR (MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK1_KICK1));
    // lock the 2nd partition
    status &= MMR_lock_one(
            CSL2PTR (MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK2_KICK0),
            CSL2PTR (MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK2_KICK1));
    // lock the 3rd partition
    status &= MMR_lock_one(
            CSL2PTR (MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK3_KICK0),
            CSL2PTR (MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK3_KICK1));
    // lock the 4th partition
    status &= MMR_lock_one(
            CSL2PTR (MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK4_KICK0),
            CSL2PTR (MCU_MMR_BASE_ADDRESS
                    + CSL_MCU_CTRL_MMR_CFG0_LOCK4_KICK1));
    return status;
}

void OSPI_configClk(uint32_t freq, bool usePHY)
{
	uint32_t divider = 0x12;

    MCU_CTRL_MMR_unlock_all();


    if(freq == OSPI_MODULE_CLK_166M)
    {
        /* Select CPSWHSDIV4 */
        *(uint32_t *)(CSL_MCU_CTRL_MMR0_CFG0_BASE + CSL_MCU_CTRL_MMR_CFG0_MCU_OSPI0_CLKSEL) = 1;

        if (usePHY)
            divider = 0xC;   /* 2000/12 = 166 */
        else
            divider = 0x6;

        *(uint32_t*)(CSL_MCU_PLL0_CFG_BASE+CSL_MCU_PLL_MMR_CFG_PLL1_HSDIV_CLKDIV) = \
                (*(uint32_t*)(CSL_MCU_PLL0_CFG_BASE+CSL_MCU_PLL_MMR_CFG_PLL1_HSDIV_CLKDIV) & 0x00FFFFFF) | ((divider-1) << 24);

        /* Load new values on 0->1 transition of bit 31 of tenabldiv */
        *(uint32_t*)(CSL_MCU_PLL0_CFG_BASE+CSL_MCU_PLL_MMR_CFG_PLL1_HSDIV_CTRL) &= 0x7FFFFFFF;
        Osal_delay(1);
        *(uint32_t*)(CSL_MCU_PLL0_CFG_BASE+CSL_MCU_PLL_MMR_CFG_PLL1_HSDIV_CTRL) |= 0x80000000;
    }
    else
    {
        /* Select MCUPLL0HSDIV4 */
        *(uint32_t *)(CSL_MCU_CTRL_MMR0_CFG0_BASE + CSL_MCU_CTRL_MMR_CFG0_MCU_OSPI0_CLKSEL) = 0;

        if(freq == OSPI_MODULE_CLK_133M)
            divider = 18;    /* 2400/18 = 133 */
        if(freq == OSPI_MODULE_CLK_160M)
            divider = 15;    /* 2400/15 = 160 */
        if(freq == OSPI_MODULE_CLK_200M)
            divider = 12;    /* 2400/12 = 200 */

        *(uint32_t*)(CSL_MCU_PLL0_CFG_BASE+CSL_MCU_PLL_MMR_CFG_PLL0_HSDIV_CLKDIV) = \
                (*(uint32_t*)(CSL_MCU_PLL0_CFG_BASE+CSL_MCU_PLL_MMR_CFG_PLL0_HSDIV_CLKDIV) & 0x00FFFFFF) | ((divider-1) << 24);

        /* Load new values on 0->1 transition of bit 31 of tenabldiv */
        *(uint32_t*)(CSL_MCU_PLL0_CFG_BASE+CSL_MCU_PLL_MMR_CFG_PLL0_HSDIV_CTRL) &= 0x7FFFFFFF;
        Osal_delay(1);
        *(uint32_t*)(CSL_MCU_PLL0_CFG_BASE+CSL_MCU_PLL_MMR_CFG_PLL0_HSDIV_CTRL) |= 0x80000000;
    }
    MCU_CTRL_MMR_lock_all();

    SPI_log("\n OSPI RCLK running at %d MHz. \n", freq);
    return;

}
#endif

#if defined(SOC_J721E) || defined(SOC_J7200) || defined(SOC_AM64X)
void OSPI_configClk(uint32_t freq, bool usePHY)
{
    OSPI_v0_HwAttrs ospi_cfg;
	int32_t retVal;
    uint64_t ospi_rclk_freq;
    uint32_t parClk;
#if defined (SOC_AM64X)
    uint32_t clkID[] = {
                           TISCI_DEV_FSS0_OSPI_0_OSPI_RCLK_CLK,
    };
    uint32_t devID[] = {
                           TISCI_DEV_FSS0_OSPI_0,
    };
#else
    uint32_t clkID[] = {
                           TISCI_DEV_MCU_FSS0_OSPI_0_OSPI_RCLK_CLK,
                           TISCI_DEV_MCU_FSS0_OSPI_1_OSPI_RCLK_CLK
    };
    uint32_t devID[] = {
                           TISCI_DEV_MCU_FSS0_OSPI_0,
	                       TISCI_DEV_MCU_FSS0_OSPI_1
    };
#endif

    /* Get the default SPI init configurations */
    OSPI_socGetInitCfg(BOARD_OSPI_NOR_INSTANCE, &ospi_cfg);    

    retVal = Sciclient_pmModuleClkRequest(devID[BOARD_OSPI_NOR_INSTANCE],
                                          clkID[BOARD_OSPI_NOR_INSTANCE],
                                          TISCI_MSG_VALUE_CLOCK_SW_STATE_REQ,
                                          TISCI_MSG_FLAG_AOP,
                                          SCICLIENT_SERVICE_WAIT_FOREVER);
    if (retVal != CSL_PASS)
    {
        SPI_log("\n Sciclient_pmModuleClkRequest failed");
	    goto clk_cfg_exit;
    }

    /* Max clocks */
    if (freq == OSPI_MODULE_CLK_166M)
    {
#if defined (SOC_AM64X)
        parClk = TISCI_DEV_FSS0_OSPI_0_OSPI_RCLK_CLK_PARENT_HSDIV4_16FFT_MAIN_0_HSDIVOUT1_CLK;
#else
        parClk = TISCI_DEV_MCU_FSS0_OSPI_0_OSPI_RCLK_CLK_PARENT_HSDIV4_16FFT_MCU_2_HSDIVOUT4_CLK;
#endif

        retVal = Sciclient_pmSetModuleClkParent(devID[BOARD_OSPI_NOR_INSTANCE],
                                                clkID[BOARD_OSPI_NOR_INSTANCE],
                                                parClk,
                                                SCICLIENT_SERVICE_WAIT_FOREVER);
    }
    else
    {
#if defined (SOC_AM64X)
        parClk = TISCI_DEV_FSS0_OSPI_0_OSPI_RCLK_CLK_PARENT_HSDIV4_16FFT_MAIN_0_HSDIVOUT1_CLK;
#else
        parClk = TISCI_DEV_MCU_FSS0_OSPI_0_OSPI_RCLK_CLK_PARENT_HSDIV4_16FFT_MCU_1_HSDIVOUT4_CLK;
#endif
        retVal = Sciclient_pmSetModuleClkParent(devID[BOARD_OSPI_NOR_INSTANCE],
                                                clkID[BOARD_OSPI_NOR_INSTANCE],
                                                parClk,
                                                SCICLIENT_SERVICE_WAIT_FOREVER);
    }
	 
    if (retVal != CSL_PASS)
    {
        SPI_log("\n Sciclient_pmSetModuleClkParent failed");
        goto clk_cfg_exit;
    }
	 
	ospi_cfg.funcClk = freq;
    OSPI_socSetInitCfg(BOARD_OSPI_NOR_INSTANCE, &ospi_cfg);    
	
    ospi_rclk_freq = (uint64_t)freq;
    retVal = Sciclient_pmSetModuleClkFreq(devID[BOARD_OSPI_NOR_INSTANCE],
                                          clkID[BOARD_OSPI_NOR_INSTANCE],
                                          ospi_rclk_freq,
                                          TISCI_MSG_FLAG_AOP,
                                          SCICLIENT_SERVICE_WAIT_FOREVER);

    if (retVal != CSL_PASS)
    {
        SPI_log("\n Sciclient_pmSetModuleClkFreq failed");
	    goto clk_cfg_exit;
    }

	ospi_rclk_freq = 0;
    retVal = Sciclient_pmGetModuleClkFreq(devID[BOARD_OSPI_NOR_INSTANCE],
                                          clkID[BOARD_OSPI_NOR_INSTANCE],
                                          &ospi_rclk_freq,
                                          SCICLIENT_SERVICE_WAIT_FOREVER);
    if (retVal != CSL_PASS)
    {
        SPI_log("\n Sciclient_pmGetModuleClkFreq failed");
	    goto clk_cfg_exit;
    }

    SPI_log("\n OSPI RCLK running at %d MHz. \n", (uint32_t)ospi_rclk_freq);

clk_cfg_exit:
      return;
}	
#endif
	
void OSPI_initConfig(OSPI_Tests *test)
{
    OSPI_v0_HwAttrs ospi_cfg;

#if defined (BUILD_MCU)
    /* Change interrupt number based on core */
    OSPI_socInit();
#endif

    /* Get the default OSPI init configurations */
    OSPI_socGetInitCfg(BOARD_OSPI_NOR_INSTANCE, &ospi_cfg);

#ifdef OSPI_QSPI_FLASH
    ospi_cfg.xferLines = OSPI_XFER_LINES_QUAD;
#endif

#ifdef SPI_CACHE_ENABLE
    ospi_cfg.cacheEnable = true;
#endif

    /* Modify the default OSPI configurations */
    ospi_cfg.dacEnable = test->dacMode;
    if (test->dacMode)
    {
        ospi_cfg.dmaEnable = test->dmaMode;
        /* Enable PHY in DAC mode */
        ospi_cfg.phyEnable = true;
        ospi_cfg.intrEnable = false;
#ifdef SPI_DMA_ENABLE
        if (ospi_cfg.dmaEnable)
        {
            Ospi_udma_init(&ospi_cfg);
        }
#endif
    }
    else
    {
        /* Enable interrupt in INDAC mode */
        ospi_cfg.intrEnable = true;
        /* Disable PHY in INDAC mode */
        ospi_cfg.phyEnable = false;
        ospi_cfg.dmaEnable = false;
    }

    if (test->testId == OSPI_TEST_ID_DAC_133M_SPI)
    {
        /* Disable PHY in legacy SPI mode (1-1-1) */
        ospi_cfg.phyEnable = false;
        ospi_cfg.dtrEnable = false;
        ospi_cfg.xferLines = OSPI_XFER_LINES_SINGLE;
    }
    else
    {
        ospi_cfg.dtrEnable = true;
        ospi_cfg.xferLines = OSPI_XFER_LINES_OCTAL;
    }

    if (test->testId == OSPI_TEST_ID_WR_TUNING)
    {
        ospi_cfg.phyEnable = false;
    }

    ospi_cfg.funcClk = test->clk;

    if (ospi_cfg.funcClk == OSPI_MODULE_CLK_133M)
    {
        ospi_cfg.devDelays[3] = OSPI_DEV_DELAY_CSDA;
    }
    else
    {
        ospi_cfg.devDelays[3] = OSPI_DEV_DELAY_CSDA_3;
    }

    if(ospi_cfg.intrEnable)
    {
        /* Set interrupt path */
        if(OSPI_configSocIntrPath(&ospi_cfg, TRUE) != CSL_PASS)
        {
            SPI_log("\n Set interrupt path failed!\n");
        }
        else
        {
            SPI_log("\n The interrupt path has been set with interrupt number %d", ospi_cfg.intrNum);
        }
    }

    /* Set the default OSPI init configurations */
    OSPI_socSetInitCfg(BOARD_OSPI_NOR_INSTANCE, &ospi_cfg);
}

void OSPI_deInitConfig(OSPI_Tests *test)
{
    OSPI_v0_HwAttrs ospi_cfg;

    /* Get the default OSPI init configurations */
    OSPI_socGetInitCfg(BOARD_OSPI_NOR_INSTANCE, &ospi_cfg);

    /* Release interrupt path */
    OSPI_configSocIntrPath(&ospi_cfg, FALSE);
}

/*
 *  ======== OSPI unit test function ========
 */
static bool OSPI_flash_test(void *arg)
{
    Board_flashHandle boardHandle;
    Board_FlashInfo  *flashInfo;
#ifdef OSPI_WRITE
    uint32_t          blockNum;     /* flash block number */
    uint32_t          pageNum;      /* flash page number */
#endif
    bool              testPassed = true;  /* return value */
    uint32_t          writeMode = OSPI_FLASH_OCTAL_PAGE_PROG;
    uint32_t          readMode = OSPI_FLASH_OCTAL_READ;
    uint32_t          deviceId;     /* flash device ID */
    OSPI_Tests       *test = (OSPI_Tests *)arg;
    uint32_t          i;
    uint32_t          offset;
    uint32_t          xferLen;
    uint32_t          testLen = TEST_DATA_LEN;
    uint32_t          testSegLen = TEST_XFER_SEG_LEN;
#ifdef OSPI_PROFILE
    uint64_t          startTime; /* start time stamp in usec */
    uint64_t          elapsedTime; /* elapsed time in usec */
    float             xferRate;
    uint32_t          xferRateInt;
#if defined(OSPI_TESTAPP_TIRTOS)
    uint32_t          cpuLoad;
#endif
#endif
    uint32_t          startOffset;
    uint8_t          *pBuf;
    uint32_t          tuneEnable;

    if (test->testId == OSPI_TEST_ID_WR_TUNING)
    {
        testLen = NOR_ATTACK_VECTOR_SIZE;
        startOffset = TEST_TUNE_PATTERN_OFFSET;
    }
    else
    {
        testLen = TEST_DATA_LEN;
        startOffset = TEST_ADDR_OFFSET;
    }

    if(test->dacMode)
    {
        tuneEnable = TRUE;
    }
    else
    {
        tuneEnable = FALSE;
    }

    OSPI_initConfig(test);

    /* Default Device, SoC's specifics overrides shall follow */
    deviceId = BOARD_FLASH_ID_MT35XU512ABA1G12;

#if defined(SOC_J7200) || defined(SOC_AM64X)
    deviceId = BOARD_FLASH_ID_S28HS512T;
#endif

#ifdef OSPI_PROFILE
    /* Get start time stamp for the Board_flashOpen measurement */
    startTime = TimerP_getTimeInUsecs();
#endif
    /* Open the Board OSPI NOR device with OSPI port 0
       and use default OSPI configurations */
    boardHandle = Board_flashOpen(deviceId,
                                  BOARD_OSPI_NOR_INSTANCE, (void *)(&tuneEnable));

#ifdef OSPI_PROFILE
    /* Get elapsed time for the Board_flashOpen measurement */
    elapsedTime = TimerP_getTimeInUsecs() - startTime;
    SPI_log("\n Board_flashOpen elpased time %d us \n", (uint32_t)elapsedTime);
#endif
    if (!boardHandle)
    {
        SPI_log("\n Board_flashOpen failed. \n");
        testPassed = false;
        goto err;
    }
    else
    {
        flashInfo = (Board_FlashInfo *)boardHandle;
        SPI_log("\n OSPI NOR device ID: 0x%x, manufacturer ID: 0x%x \n",
                flashInfo->device_id, flashInfo->manufacturer_id);
    }

    /* Generate the data */
    GeneratePattern(txBuf, rxBuf, testLen);
    if (test->testId == OSPI_TEST_ID_WR_TUNING)
    {
        pBuf = nor_attack_vector;
    }
    else
    {
        pBuf = txBuf;
    }
#ifdef SPI_DMA_ENABLE
    if (test->dmaMode)
    {
        CacheP_wbInv((void *)pBuf, (int32_t)testLen);
        CacheP_wbInv((void *)rxBuf, (int32_t)testLen);
    }
#endif

#ifdef OSPI_WRITE
#if defined(SOC_J7200) || defined(SOC_AM64X)
    if (test->dacMode)                          /* DAC writes are not supported on Cypress xSPI Flash - Switch to INDAC mode for write as WA to PDK-7115 */
    {
        SPI_log("\n The Cypress xSPI Flash does not support DAC writes, switching to INDAC mode. \n");
        OSPI_v0_HwAttrs ospi_cfg;
        Board_flashClose(boardHandle);
    #if SPI_DMA_ENABLE
        if (test->dmaMode)
        {
            Ospi_udma_deinit();
        }
    #endif
        OSPI_socGetInitCfg(BOARD_OSPI_NOR_INSTANCE, &ospi_cfg);
        ospi_cfg.dacEnable = false;
        ospi_cfg.phyEnable = false;
        ospi_cfg.dmaEnable = false;
        ospi_cfg.intrEnable = true;
        OSPI_socSetInitCfg(BOARD_OSPI_NOR_INSTANCE, &ospi_cfg);
        boardHandle = Board_flashOpen(deviceId, BOARD_OSPI_NOR_INSTANCE, (void *)(&tuneEnable));
    }
#endif
    for (i = 0; i < testLen; i += NOR_BLOCK_SIZE)
    {
        offset = startOffset + i;
        if (Board_flashOffsetToBlkPage(boardHandle, offset,
                                       &blockNum, &pageNum))
        {
            SPI_log("\n Board_flashOffsetToBlkPage failed. \n");
            testPassed = false;
            goto err;
        }

        /* Erase block, to which data has to be written */
        if (Board_flashEraseBlk(boardHandle, blockNum))
        {
            SPI_log("\n Board_flashEraseBlk failed. \n");
            testPassed = false;
            goto err;
        }
    }
#ifdef OSPI_PROFILE
#if defined(OSPI_TESTAPP_TIRTOS)
    Load_reset( );
#endif
    /* Get start time stamp for the write performance measurement */
    startTime = TimerP_getTimeInUsecs();
#endif
    for (i = 0; i < testLen; i += testSegLen)
    {
        offset = startOffset + i;
        if ((testLen - i) < testSegLen)
        {
            xferLen = testLen - i;
        }
        else
        {
            xferLen = testSegLen;
        }
        /* Write buffer to flash */
        if (Board_flashWrite(boardHandle, offset, &pBuf[i],
                             xferLen, (void *)(&writeMode)))
        {
            SPI_log("\n Board_flashWrite failed. \n");
            testPassed = false;
            goto err;
        }
    }

#ifdef OSPI_PROFILE
    elapsedTime = TimerP_getTimeInUsecs() - startTime;
#if defined(OSPI_TESTAPP_TIRTOS)
    Load_update( );
    cpuLoad = Load_getCPULoad();
#endif
    /* calculate the write transfer rate in Kbps */
    xferRate = (float) (((float) (testLen * 8)) / elapsedTime) * 1000U;
    xferRateInt = (uint32_t)xferRate;
    SPI_log("\n Board_flashWrite %d bytes at transfer rate %d Kbps \n", testLen, xferRateInt);
#if defined(OSPI_TESTAPP_TIRTOS)
    SPI_log("\n Board_flashWrite CPU load %d%% \n", cpuLoad);
#endif
#endif
#endif /* OSPI_WRITE */

#if defined(SOC_J7200) || defined(SOC_AM64X)
    if (test->dacMode)                          /* DAC reads are supported - switching back to DAC mode */
    {
        SPI_log("\n The Cypress xSPI Flash supports DAC reads, switching back to DAC mode. \n");
        OSPI_v0_HwAttrs ospi_cfg;
        Board_flashClose(boardHandle);
        OSPI_socGetInitCfg(BOARD_OSPI_NOR_INSTANCE, &ospi_cfg);
        if (test->testId == OSPI_TEST_ID_DAC_133M_SPI)
        {
            /* Disable PHY in legacy SPI mode (1-1-1) */
            ospi_cfg.phyEnable = false;
            ospi_cfg.dtrEnable = false;
            ospi_cfg.xferLines = OSPI_XFER_LINES_SINGLE;
        }
        else
        {
            /* Enable PHY in octal SPI mode (8-8-8) */
            ospi_cfg.phyEnable = true;
            ospi_cfg.dtrEnable = true;
            ospi_cfg.xferLines = OSPI_XFER_LINES_OCTAL;
        }

        ospi_cfg.dacEnable = true;
        ospi_cfg.dmaEnable = test->dmaMode;
        ospi_cfg.intrEnable = false;
    #ifdef SPI_DMA_ENABLE
        if (ospi_cfg.dmaEnable)
        {
            Ospi_udma_init(&ospi_cfg);
        }
    #endif
        OSPI_socSetInitCfg(BOARD_OSPI_NOR_INSTANCE, &ospi_cfg);
        boardHandle = Board_flashOpen(deviceId, BOARD_OSPI_NOR_INSTANCE, (void *)(&tuneEnable));
    }
#endif
#ifdef OSPI_PROFILE
    if( (test->dacMode) && (test->testId!=OSPI_TEST_ID_DAC_133M_SPI) && (test->testId!=OSPI_TEST_ID_WR_TUNING) )
    {
        uint64_t    startPhyTuningTime;
        uint64_t    elapsedPhyTuningTime;
        uint8_t     tmpRxBuf[4U];

        /* Dummy flash read to get just the PHY tuning time */
        startPhyTuningTime = TimerP_getTimeInUsecs();
        if (Board_flashRead(boardHandle, 0U, &tmpRxBuf[0], 4U, (void *)(&readMode)))
        {
            SPI_log("\n Dummy Board_flashRead for PHY tuning measurement failed. \n");
            testPassed = false;
            goto err;
        }
        elapsedPhyTuningTime = TimerP_getTimeInUsecs() - startPhyTuningTime;
        SPI_log("\n PHY tuning + 4 byte read took %d us \n", (uint32_t)elapsedPhyTuningTime);
    }
#if defined(OSPI_TESTAPP_TIRTOS)
    Load_reset( );
#endif
    /* Get start time stamp for the read performance measurement */
    startTime = TimerP_getTimeInUsecs();
#endif
    for (i = 0; i < testLen; i += testSegLen)
    {
        offset = startOffset + i;
        if ((testLen - i) < testSegLen)
        {
            xferLen = testLen - i;
        }
        else
        {
            xferLen = testSegLen;
        }
        if (Board_flashRead(boardHandle, offset, &rxBuf[i],
                            xferLen, (void *)(&readMode)))
        {
            SPI_log("\n Board_flashRead failed. \n");
            testPassed = false;
            goto err;
        }
    }

#ifdef OSPI_PROFILE
    elapsedTime = TimerP_getTimeInUsecs() - startTime;
#if defined(OSPI_TESTAPP_TIRTOS)
    Load_update( );
    cpuLoad = Load_getCPULoad();
#endif
    /* calculate the write transfer rate in Kbps */
    xferRate = (float) (((float) (testLen * 8)) / elapsedTime);
    xferRateInt = (uint32_t)xferRate;
    SPI_log("\n Board_flashRead %d bytes at transfer rate %d Mbps \n", testLen, xferRateInt);
#if defined(OSPI_TESTAPP_TIRTOS)
    SPI_log("\n Board_flashRead CPU load %d%% \n", cpuLoad);
#endif
#endif

#ifdef OSPI_WRITE
    /* Verify Data */
    if (VerifyData(pBuf, rxBuf, testLen) == false)
    {
        SPI_log("\n Data mismatch. \n");
        testPassed = false;
        goto err;
    }
#endif

err:
    OSPI_deInitConfig(test);
    
    if (boardHandle != 0U)
    {
        Board_flashClose(boardHandle);
    }

#if SPI_DMA_ENABLE
    if (test->dmaMode)
    {
        Ospi_udma_deinit();
    }
#endif

    return (testPassed);
}

void OSPI_test_print_test_desc(OSPI_Tests *test)
{
    char        testId[16] = {0, };

    /* Print unit test ID */
    sprintf(testId, "%d", test->testId);
    SPI_log("\r\n OSPI UT %s\r\n", testId);

    /* Print test description */
    SPI_log("\r\n %s\r\n", test->testDesc);
}

OSPI_Tests Ospi_tests[] =
{
#ifdef OSPI_WRITE_TUNING
#if defined(SOC_J7200) || defined(SOC_AM64X)
    /* testFunc       testID                     dacMode dmaMode clk                   testDesc */
    {OSPI_flash_test, OSPI_TEST_ID_WR_TUNING,    false,  false,  OSPI_MODULE_CLK_133M, "\r\n OSPI flash test slave to write tuning data to flash"},
#else
    {OSPI_flash_test, OSPI_TEST_ID_WR_TUNING,    true,   false,  OSPI_MODULE_CLK_133M, "\r\n OSPI flash test slave to write tuning data to flash"},
#endif
#endif
    {OSPI_flash_test, OSPI_TEST_ID_DAC_133M,     true,   false,  OSPI_MODULE_CLK_133M, "\r\n OSPI flash test slave in DAC mode at 133MHz RCLK"},
    {OSPI_flash_test, OSPI_TEST_ID_INDAC_133M,   false,  false,  OSPI_MODULE_CLK_133M, "\r\n OSPI flash test slave in INDAC mode at 133MHz RCLK"},
#ifdef SPI_DMA_ENABLE
    {OSPI_flash_test, OSPI_TEST_ID_DAC_DMA_133M, true,   true,   OSPI_MODULE_CLK_133M, "\r\n OSPI flash test slave in DAC DMA mode at 133MHz RCLK"},
#endif
    {OSPI_flash_test, OSPI_TEST_ID_DAC_166M,     true,   false,  OSPI_MODULE_CLK_166M, "\r\n OSPI flash test slave in DAC mode at 166MHz RCLK"},
    {OSPI_flash_test, OSPI_TEST_ID_INDAC_166M,   false,  false,  OSPI_MODULE_CLK_166M, "\r\n OSPI flash test slave in INDAC mode at 166MHz RCLK"},
#ifdef SPI_DMA_ENABLE
    {OSPI_flash_test, OSPI_TEST_ID_DAC_DMA_166M, true,   true,   OSPI_MODULE_CLK_166M, "\r\n OSPI flash test slave in DAC DMA mode at 166MHz RCLK"},
#endif
    {OSPI_flash_test, OSPI_TEST_ID_DAC_133M_SPI, true,   false,  OSPI_MODULE_CLK_133M, "\r\n OSPI flash test slave in DAC Legacy SPI mode at 133MHz RCLK"},
    {NULL, }
};

/*
 *  ======== test function ========
 */
#if defined(OSPI_TESTAPP_RTOS)
void spi_test(void* arg0, void* arg1)
#else
void spi_test()
#endif
{
    uint32_t    i;
    bool        testFail = false;
    OSPI_Tests *test;

    /* Init SPI driver */
    SPI_init();

    for (i = 0; ; i++)
    {
        test = &Ospi_tests[i];
        if (test->testFunc == NULL)
        {
            break;
        }

        OSPI_configClk(test->clk, true);

        OSPI_test_print_test_desc(test);

        if (test->testFunc((void *)test) == true)
        {
            SPI_log("\r\n %s have passed\r\n", test->testDesc);
        }
        else
        {
            SPI_log("\r\n %s have failed\r\n", test->testDesc);
            testFail = true;
            break;
        }
    }

    if(testFail == true)
    {
        SPI_log("\n Some tests have failed. \n");
    }
    else
    {
        SPI_log("\n All tests have passed. \n");
    }

    SPI_log("Done\n");

    while (true)
    {
    }
}

/*
 *  ======== main ========
 */
int main(void)
{
    /* Call board init functions */
    Board_initCfg boardCfg;
#if defined(OSPI_TESTAPP_RTOS)
    TaskP_Handle task;
    TaskP_Params taskParams;
#endif
    boardCfg = BOARD_INIT_PINMUX_CONFIG |
        BOARD_INIT_MODULE_CLOCK |
        BOARD_INIT_UART_STDIO;
#if defined (SOC_J7200)
    /* Need to do PLL config through board init for proper clock input on J7200 */
    boardCfg |= BOARD_INIT_PLL;
#endif

    Board_init(boardCfg);

#if defined(OSPI_TESTAPP_RTOS)

	OS_init();
    /* Initialize the task params */
    TaskP_Params_init(&taskParams);
    /* Set the task priority higher than the default priority (1) */
    taskParams.priority     = 2;
    taskParams.stack        = gAppTskStackMain;
    taskParams.stacksize    = sizeof (gAppTskStackMain);

    /* Start BIOS */
	task = TaskP_create(spi_test, &taskParams);
    if (task == NULL) {
        UART_printf("TaskP_create() failed!\n");
        OS_stop();
    }
    OS_start();
#else
    spi_test();
#endif
    return (0);
}

/*
 *  ======== CompareData ========
 */
bool VerifyData(uint8_t *expData,
                uint8_t *rxData,
                uint32_t length)
{
    uint32_t idx = 0;
    uint32_t match = 1;
    bool retVal = false;

    for(idx = 0; ((idx < length) && (match != 0)); idx++)
    {
        if(*expData != *rxData)
        {
            match = 0;
            SPI_log("Data mismatch at idx %d\n", idx);
        }
        expData++;
        rxData++;
    }

    if(match == 1)
    {
        retVal = true;
    }

    return retVal;
}

/*
 *  ======== GeneratePattern ========
 */
static void GeneratePattern(uint8_t *txBuf, uint8_t *rxBuf, uint32_t length)
{
    volatile uint32_t idx;
    volatile uint8_t *txPtr = txBuf;
    volatile uint8_t *rxPtr = rxBuf;

    for(idx = 0; idx < length; idx++)
    {
        if (idx < (length/2))
        {
            *txPtr++ = (uint8_t)idx;
        }
        else if (idx < (length/4*3))
        {
            *txPtr++ = 0xaa;
        }
        else
        {
            *txPtr++ = 0x55;
        }
    	*rxPtr++ = (uint8_t)0U;
    }
}
