/**
 *  \file    sbl_qspi_boardflash.c
 *
 *  \brief   This file contains functions for QSPI read/write operations for SBL
 *
 */

/*
 * Copyright (C) 2020 Texas Instruments Incorporated - http://www.ti.com/
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

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include "string.h"

/* SBL Header files. */
#include "sbl_soc.h"
#include "sbl_rprc_parse.h"
#include "sbl_soc_cfg.h"
#include "sbl_err_trap.h"
#include "sbl_rcm.h"

/* TI-RTOS Header files */
#include <ti/drv/spi/SPI.h>
#if SBL_USE_DMA
#include <ti/drv/edma/edma.h>
#endif

#include <ti/osal/DebugP.h>
#include <ti/drv/spi/src/SPI_osal.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/drv/spi/soc/SPI_soc.h>
#include <ti/drv/spi/soc/QSPI_v1.h> 
#include <ti/csl/cslr_device.h>
#include <ti/csl/arch/csl_arch.h>
#include <ti/csl/arch/r5/csl_arm_r5.h>
#include <ti/csl/arch/r5/interrupt.h>
#include <ti/board/board_cfg.h>
#include <ti/board/src/flash/include/board_flash.h>
#include "sbl_qspi_boardflash.h"

#if defined(tpr12_evm)
#define QSPI_FLASH_ID           BOARD_FLASH_ID_GD25B64CW2G
#elif defined(tpr12_qt)
#define QSPI_FLASH_ID           BOARD_FLASH_ID_W25Q16FWSF
#elif defined(awr294x_evm)
#define QSPI_FLASH_ID           BOARD_FLASH_ID_GD25B64CW2G
#endif

#if defined(tpr12_qt)
#undef  QSPI_OFFSET_SI
#define QSPI_OFFSET_SI              (0x80000U)
#endif


/* QSPI Flash Read Sector API. */
static int32_t SBL_QSPI_ReadSectors(void *dstAddr,
                             void *srcOffsetAddr,
                             uint32_t length);

/* Initialize the QSPI driver and the controller. */
static void SBL_QSPI_Initialize(void);

/* Sets the src address to the given offset address. */
static void SBL_QSPI_seek(void *srcAddr, uint32_t location);

void SBL_DCacheClean(void *addr, uint32_t size);

static void *boardHandle = NULL;

static QSPI_HwAttrs qspi_cfg;


#if SBL_USE_DMA
EDMA_Handle  gEdmaHandle = NULL;
static int32_t Qspi_edma_deinit(void);
static int32_t Qspi_edma_init(void);

static int32_t Qspi_edma_init(void)
{
    /* EDMA driver init */
    uint32_t edma3Id = EDMA_DRV_INST_MSS_A;
    EDMA3CCInitParams 	initParam;
    EDMA_instanceInfo_t instanceInfo;
    int32_t errorCode = EDMA_NO_ERROR;

    SBL_ADD_PROFILE_POINT;

    if (gEdmaHandle == (EDMA_Handle)uint32_to_void_ptr(0U))
    {
        gEdmaHandle = EDMA_getHandle(edma3Id, &instanceInfo);
        if(gEdmaHandle == NULL)
        {
            EDMA3CCInitParams_init(&initParam);
            initParam.initParamSet = TRUE;
            errorCode = EDMA_init(edma3Id, &initParam);
            if (errorCode != EDMA_NO_ERROR)
            {
                SBL_log(SBL_LOG_ERR, "EDMA_init failed!\n");
            }
            if (EDMA_NO_ERROR == errorCode)
            {
                /* Open DMA driver instance 0 for SPI test */
                gEdmaHandle = EDMA_open(edma3Id, &errorCode, &instanceInfo);
                if(gEdmaHandle == NULL)
                {
                    SBL_log(SBL_LOG_ERR, "Open DMA driver failed with error=%d\n \n", errorCode);
                }
            }
        }
    }

    SBL_ADD_PROFILE_POINT;

    return (errorCode);
}

static int32_t Qspi_edma_deinit(void)
{
    int32_t         retVal = EDMA_NO_ERROR;

    SBL_ADD_PROFILE_POINT;

    if (gEdmaHandle)
    {
        retVal = EDMA_close(gEdmaHandle);
        if(EDMA_NO_ERROR == retVal)
        {
            gEdmaHandle = NULL;
        }
    }

    SBL_ADD_PROFILE_POINT;

    return (retVal);
}
#endif


int32_t SBL_qspiInit(void *handle)
{
#if !defined(SBL_BYPASS_QSPI_DRIVER)
    Board_flashHandle h = *(Board_flashHandle *) handle;
    int32_t retVal; 
    uint32_t qspiClkFreq;

    SBL_ADD_PROFILE_POINT;

    if (h)
    {
        Board_flashClose(h);
        SBL_ADD_PROFILE_POINT;
    }
    retVal = QSPI_socGetInitCfg(BOARD_QSPI_NOR_INSTANCE, &qspi_cfg);
    DebugP_assert(retVal == 0);
#if (defined(SOC_TPR12) || defined(SOC_AWR294X))
    {
        Rcm_Return rcmRetVal;

        rcmRetVal = SBL_RcmGetPeripheralFreq(Rcm_PeripheralId_MSS_QSPI, &qspiClkFreq);
        DebugP_assert(rcmRetVal == Rcm_Return_SUCCESS);
    }
#else
    #error "Configure QSPI qspiClkFreq for the SOC"
    qspiClkFreq = 0;
#endif
    qspi_cfg.funcClk = qspiClkFreq;
    SBL_log(SBL_LOG_MAX, "qspiFunClk = %d Hz \n", qspi_cfg.funcClk);

#if SBL_USE_DMA
    qspi_cfg.dmaEnable = true;
    retVal = Qspi_edma_init();
    DebugP_assert(retVal == EDMA_NO_ERROR);
    qspi_cfg.edmaHandle =  gEdmaHandle;
#endif

    /* Set the default SPI init configurations */
    QSPI_socSetInitCfg(BOARD_QSPI_NOR_INSTANCE, &qspi_cfg);

    h = Board_flashOpen(QSPI_FLASH_ID,
                        BOARD_QSPI_NOR_INSTANCE, NULL);
    if (h)
    {
        *(Board_flashHandle *) handle = h;
    }
    else
    {
        SBL_log(SBL_LOG_ERR, "Board_flashOpen failed!\n");
        SblErrLoop(__FILE__, __LINE__);
    }

    SBL_ADD_PROFILE_POINT;

    return 0;
#else
    SBL_ADD_PROFILE_POINT;

    SBL_ADD_PROFILE_POINT;
    return 0;
#endif
}

int32_t SBL_qspiFlashRead(void *handle, uint8_t *dst, uint32_t length,
    uint32_t offset)
{
    uint32_t start_time = SBL_ADD_PROFILE_POINT;
    uint32_t end_time = 0;

#if !defined(SBL_BYPASS_QSPI_DRIVER)
#if SBL_USE_DMA
    if (length > 4 * 1024)
    {
        Board_flashHandle h = *(const Board_flashHandle *) handle;
        uint32_t ioMode  = QSPI_FLASH_QUAD_READ;
        /* split transfer if not reading from 16 byte aligned flash offset */
        uint32_t dma_offset  = (offset + 0xF) & (~0xF);
        uint32_t non_aligned_bytes = dma_offset - offset;
        uint8_t *dma_dst = (dst + non_aligned_bytes);
        uint32_t dma_len = length - non_aligned_bytes;

        SBL_DCacheClean((void *)dst, length);

        dst = (uint8_t *)((uintptr_t)(CSL_locToGlobAddr((uintptr_t)dst)));
        if ((non_aligned_bytes) && (Board_flashRead(h, offset, dst, non_aligned_bytes, (void *)(&ioMode))))
        {
            SBL_log(SBL_LOG_ERR, "Board_flashRead failed!\n");
            SblErrLoop(__FILE__, __LINE__);
        }

        dma_dst = (uint8_t *)((uintptr_t)(CSL_locToGlobAddr((uintptr_t)dma_dst)));
        if (Board_flashRead(h, dma_offset, dma_dst, dma_len, (void *)(&ioMode)))
        {
            SBL_log(SBL_LOG_ERR, "Board_flashRead failed!\n");
            SblErrLoop(__FILE__, __LINE__);
        }
    }
    else
    {
        memcpy((void *)dst, (void *)(qspi_cfg.memMappedBaseAddr + offset), length);
    }
#else
    memcpy((void *)dst, (void *)(qspi_cfg.memMappedBaseAddr + offset), length);
#endif /* #if SBL_USE_DMA */

#else
    memcpy((void *)dst, (void *)(qspi_cfg.dataAddr + offset), length);
#endif /* #if !defined(SBL_BYPASS_QSPI_DRIVER) */

    end_time = SBL_ADD_PROFILE_POINT;

    SBL_log(SBL_LOG_MAX, "Qspi Read speed for 0x%x bytes from offset 0x%x = %d Mbytes per sec\n", length, offset, ((400000000 / (end_time-start_time)) * length)/0x100000);

    return 0;
}

int32_t SBL_qspiClose(void *handle)
{
#if !defined(SBL_BYPASS_QSPI_DRIVER)
    Board_flashHandle h = *(const Board_flashHandle *) handle;

    SBL_ADD_PROFILE_POINT;

    SBL_log(SBL_LOG_MAX, "SBL_qspiClose called\n");
    Board_flashClose(h);
#if SBL_USE_DMA
    Qspi_edma_deinit();
#endif
    SBL_ADD_PROFILE_POINT;
#else
    SBL_ADD_PROFILE_POINT;
    SBL_ADD_PROFILE_POINT;
#endif
    return 0;
}

int32_t SBL_QSPIBootImage(sblEntryPoint_t *pEntry)
{
    int32_t retVal;
    uint32_t offset = QSPI_OFFSET_SI;

    /* Initialization of the driver. */
    SBL_QSPI_Initialize();

    retVal =  SBL_MulticoreImageParse((void *) &offset, QSPI_OFFSET_SI, pEntry, SBL_BOOT_AFTER_COPY);

    SBL_qspiClose(&boardHandle);

    return retVal;
}

static void SBL_QSPI_Initialize(void)
{
    SBL_qspiInit(&boardHandle);

    /* Initialize the function pointers to parse through the RPRC format. */

    fp_readData = &SBL_QSPI_ReadSectors;
    fp_seek     = &SBL_QSPI_seek;

}


static int32_t SBL_QSPI_ReadSectors(void *dstAddr,
                             void *srcOffsetAddr,
                             uint32_t length)
{
    int32_t ret;
    ret = SBL_qspiFlashRead(&boardHandle, (uint8_t *) dstAddr, length,
        *((uint32_t *) srcOffsetAddr));
    *((uint32_t *) srcOffsetAddr) += length;
    return ret;
}

static void SBL_QSPI_seek(void *srcAddr, uint32_t location)
{
    *((uint32_t *) srcAddr) = location;
}



