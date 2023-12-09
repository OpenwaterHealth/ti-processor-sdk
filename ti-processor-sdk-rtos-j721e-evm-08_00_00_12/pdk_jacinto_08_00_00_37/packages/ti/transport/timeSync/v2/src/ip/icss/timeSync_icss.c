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
 * \file timeSync_icss.c
 *
 * \brief This file contains the implementation of ICSS specific
 *        funcions for PTP stack
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <string.h>
#include <assert.h>

/* OSAL Header files */
#include <ti/osal/osal.h>
#include <ti/osal/soc/osal_soc.h>

#include <ti/csl/soc.h>
#if defined(__ARM_ARCH_7A__)
#include <ti/csl/csl_armGicAux.h>
#endif

#include <ti/drv/icss_emac/firmware/icss_dualemac/src/icss_timeSync_memory_map.h>

#include <ti/transport/timeSync/v2/include/timeSync.h>
#include <ti/transport/timeSync/v2/include/timeSync_tools.h>
#include <ti/transport/timeSync/v2/src/ip/icss/timeSync_icss_ecap_edma.h>

#include <ti/drv/icss_emac/icss_emacDrv.h>
#include <ti/drv/icss_emac/icss_emac_ver.h>
#include <ti/drv/icss_emac/src/icss_emacLoc.h>

#include <ti/transport/timeSync/v2/src/ip/icss/timeSync_icss_ip_header.h>

#include <ti/osal/HwiP.h>
#include <ti/osal/TimerP.h>

#include <ti/osal/TaskP.h>
#include <ti/osal/EventP.h>

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */

/**
 * @def TIMESYNC_TX_CALLBACK_HWI_PRIORITY
 *      Priority for Tx callback interrupt. Range 0-31
 */
#define TIMESYNC_TX_CALLBACK_HWI_PRIORITY       (20U)

/**
 * @def TIMESYNC_RX_PKTINFO_COUNT
 *      Rx packet information count
 */
#define TIMESYNC_RX_PKTINFO_COUNT               (64U)

/**
 * @def TIMESYNC_SYNC0_PERIOD_DIVIDER
 *      CMP1 period divided by this number makes the width of the Sync pulse
 */
#define TIMESYNC_SYNC0_PERIOD_DIVIDER           (4U)

/**
 * @def TIMESYNC_PTP_LATCH0_HWI_PRIORITY
 *      Priority for Latch0 interrupt. Range 0-31
 */
#define TIMESYNC_PTP_LATCH0_HWI_PRIORITY        (20U)

/**
 * @def TIMESYNC_PTP_LATCH_INT_NUM
 *      Latch interrupt number.
 */
#if defined(SOC_AM335x)
#define TIMESYNC_PTP_LATCH_INT_NUM              (23U)
#elif defined(SOC_AM437x)
#define TIMESYNC_PTP_LATCH_INT_NUM              (23U + 32U)
#elif defined(SOC_AM572x) || defined(SOC_AM574x) || defined(SOC_AM571x)
#define TIMESYNC_PTP_LATCH_INT_NUM              (122U + 32U)
#endif

/**
 * @def TIMESYNC_LATCH0_ISR_MASK
 *      Interrupt mask for LATCH0 input
 */
#define TIMESYNC_LATCH0_ISR_MASK                (0x1000U)

/**
 * @def TIMSYNC_RX_PHY_LATENCY
 *      Rx PHY Latency in nano seconds. This is the known delay for TLK110
 */
#define TIMSYNC_RX_PHY_LATENCY                  (186U)

/**
 * @def TIMSYNC_TX_PHY_LATENCY
 *      Tx PHY latency in nano seconds
 */

/* Since there is no corresponding SFD timestamp for Tx
 * we add a fixed delay of 640ns to the known SOF Tx timestamp.
 * 640 (1 byte delay to get SFD) + 86 (PHY delay) = 726ns */
#define TIMSYNC_TX_PHY_LATENCY                  (726U)

/**
 * @def TIMESYNC_TX_TS_ISR_MASK_P1
 *      Interrupt mask for PRU event 26
 */
#define TIMESYNC_TX_TS_ISR_MASK_P1              (0x4000000U)

/**
 * @def TIMESYNC_TX_TS_ISR_MASK_P2
 *      Interrupt mask for PRU event 27
 */
#define TIMESYNC_TX_TS_ISR_MASK_P2              (0x8000000U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

typedef struct TimeSync_RxPktInfoEle_s
{
    /** Rx queue number returned by rx interrupt*/
    uint8_t rxQnum;

    uint8_t portNum;
} TimeSync_RxPktInfoEle;

typedef struct TimeSync_RxPktInfo_s
{
    TimeSync_RxPktInfoEle rxPktInfoArr[TIMESYNC_RX_PKTINFO_COUNT];

    uint32_t rdIdx;

    uint32_t wrIdx;
}TimeSync_RxPktInfo;

typedef struct TimeSync_Obj_s
{
    TimeSync_Config timeSyncConfig;

    /** EDMA parameters*/
    EdmaConfig edmaConfig;

    /**icss emac handle*/
    ICSS_EmacHandle emacHandle;

    /** Tx timestamp interrupt number*/
    uint32_t txIntNum;

    TimeSync_RxPktInfo rxPktInfo;
}TimeSync_Obj;

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/**
 * @brief Initialize PTP stored variables in the DRAM
 * @param timeSyncHandle pointer to PTP Handle structure
 * @return none
 */
static void TimeSync_dramInit(TimeSync_Handle timeSyncHandle);

static void TimeSync_drvIEPConfig(TimeSync_Handle timeSyncHandle);

static int32_t TimeSync_registerTxIntr(TimeSync_Handle timeSyncHandle);

static int32_t TimeSync_registerLatchIntr(TimeSync_Handle timeSyncHandle);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static TimeSync_Obj gTimeSyncCpswObj;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void TimeSync_notifyRxFxn(uint32_t *queue_number,
                          void *userArg)
{
    TimeSync_Handle timeSyncHandle = NULL;
    TimeSync_FrameNotifyConfig *frameNotifyCfg = NULL;

    if (userArg != NULL)
    {
        timeSyncHandle = (TimeSync_Handle)userArg;
        frameNotifyCfg = &timeSyncHandle->timeSyncConfig.frameNotifyCfg;

        timeSyncHandle->rxPktInfo.rxPktInfoArr[timeSyncHandle->rxPktInfo.wrIdx++].rxQnum =
            *queue_number;
        timeSyncHandle->rxPktInfo.wrIdx %= TIMESYNC_RX_PKTINFO_COUNT;

        /* Notify PTP Rx callback */
        if (frameNotifyCfg->rxNotifyCb != NULL)
        {
            frameNotifyCfg->rxNotifyCb(frameNotifyCfg->rxNotifyCbArg);
        }
    }
}

void TimeSync_notifyTxFxn(void* arg)
{
    volatile uint32_t *intStatusPtr = NULL;
    volatile uint8_t *bytePtr = NULL;
    uint8_t portNum = 0U;
    uint32_t sharedRAMbaseAddress = 0U;
    uint16_t seqId = 0U;
    TimeSync_Handle timeSyncHandle = NULL;
    TimeSync_FrameNotifyConfig *frameNotifyCfg = NULL;

    timeSyncHandle = (TimeSync_Handle)arg;
    frameNotifyCfg = &timeSyncHandle->timeSyncConfig.frameNotifyCfg;

    sharedRAMbaseAddress = (((ICSS_EmacHwAttrs *)
                             (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr;

    intStatusPtr = (uint32_t *)(uint32_t)((((ICSS_EmacHwAttrs *)
                                            (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->prussIntcRegs \
                                          + CSL_ICSSINTC_SECR0);

    /*Check which port posted the interrupt*/
    if (*intStatusPtr & TIMESYNC_TX_TS_ISR_MASK_P1)
    {
        bytePtr = (uint8_t *)((uint32_t)(sharedRAMbaseAddress +
                                         TX_TS_NOTIFICATION_OFFSET_SYNC_P1));
        *intStatusPtr = TIMESYNC_TX_TS_ISR_MASK_P1;
        portNum = ICSS_EMAC_PORT_1;
    }
    else
    {
        bytePtr = (uint8_t *)((uint32_t)(sharedRAMbaseAddress +
                                         TX_TS_NOTIFICATION_OFFSET_SYNC_P2));
        *intStatusPtr = TIMESYNC_TX_TS_ISR_MASK_P2;
        portNum = ICSS_EMAC_PORT_2;
    }

    if (frameNotifyCfg->txNotifyCb != NULL)
    {
        /*sync frame*/
        if (*(bytePtr))
        {
            frameNotifyCfg->txNotifyCb(frameNotifyCfg->txNotifyCbArg,
                                       portNum,
                                       TIMESYNC_SYNC_FRAME,
                                       seqId);
            *(bytePtr) = 0;
        }

        /*delay request frame*/
        if (*(bytePtr + 1))
        {
            frameNotifyCfg->txNotifyCb(frameNotifyCfg->txNotifyCbArg,
                                       portNum,
                                       TIMESYNC_DELAY_REQ_FRAME,
                                       seqId);
            *(bytePtr + 1) = 0;
        }

        /*Pdelay response frame*/
        if (*(bytePtr + 2))
        {
            frameNotifyCfg->txNotifyCb(frameNotifyCfg->txNotifyCbArg,
                                       portNum,
                                       TIMESYNC_PDELAY_RESP_FRAME,
                                       seqId);
            *(bytePtr + 2) = 0;
        }
    }
}

void TimeSync_latchIsr(void* arg)
{
    volatile uint32_t *intStatusPtr = NULL;
    TimeSync_Handle timeSyncHandle  = NULL;
    TimeSync_PpsConfig *pPpsConfig  = NULL;

    /* void* is of type integer, so compared against 0U */
    if (arg != 0x0U)
    {
        timeSyncHandle = (TimeSync_Handle)arg;

        /* Call PPS callback */
        pPpsConfig = &timeSyncHandle->timeSyncConfig.protoCfg.ppsConfig;
        if (pPpsConfig->ppsNotifyCb != NULL)
        {
            pPpsConfig->ppsNotifyCb(pPpsConfig->ppsNotifyCbArg);
        }

        /*clear the interrupt*/
        intStatusPtr = (uint32_t *)(uint32_t)((((ICSS_EmacHwAttrs *)
                                                (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->prussIntcRegs \
                                              + CSL_ICSSINTC_SECR0);
        *intStatusPtr = TIMESYNC_LATCH0_ISR_MASK;
    }
}

TimeSync_Handle TimeSync_open(TimeSync_Config *timeSyncConfig)
{
    ICSS_EmacBaseAddressHandle_T emacBaseAddr;
    uint8_t i;
    TimeSync_Handle timeSyncHandle = NULL;
    int8_t status = TIMESYNC_OK;
    volatile uint8_t *ptpCtrlPtr = NULL;
    volatile uint8_t *bytePtr = NULL;
    uint8_t byteVal = TRUE;

    if (timeSyncConfig != NULL)
    {
        memset(&gTimeSyncCpswObj, 0U, sizeof(gTimeSyncCpswObj));
        timeSyncHandle = &gTimeSyncCpswObj;

        /*Copy timeSyncConfig to handle */
        timeSyncHandle->timeSyncConfig = *timeSyncConfig;

        if (timeSyncConfig->frameNotifyCfg.nwDrvHandle != NULL)
        {
            timeSyncHandle->emacHandle = (ICSS_EmacHandle)timeSyncConfig->frameNotifyCfg.nwDrvHandle;

            /*Packet processing callback*/
            ((((ICSS_EmacObject *)
               timeSyncHandle->emacHandle->object)->callBackHandle)->rxRTCallBack)->callBack =
                (ICSS_EmacCallBack)TimeSync_notifyRxFxn;
            ((((ICSS_EmacObject *)
               timeSyncHandle->emacHandle->object)->callBackHandle)->rxRTCallBack)->userArg =
                timeSyncHandle;

            TimeSync_dramInit(timeSyncHandle);

            emacBaseAddr = ((ICSS_EmacHwAttrs *)(timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg;

            if (timeSyncConfig->protoCfg.deviceCfg == TIMESYNC_MASTER_CLOCK)
            {
                /*Write into memory*/
                bytePtr = (uint8_t *)(((ICSSEMAC_HwAttrs *)
                                       timeSyncHandle->emacHandle->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr + DUT_IS_MASTER_OFFSET;
                *bytePtr = 1;
            }

            /* Configures domainNumber list. Firmware supports 2 domains*/
            for (i = 0; i < TIMESYNC_NUM_DOMAINS; i++)
            {
                HWREGB(emacBaseAddr->dataRam1BaseAddr + TIMESYNC_DOMAIN_NUMBER_LIST + i) =
                    timeSyncHandle->timeSyncConfig.protoCfg.domainNumber[i];
            }

            TimeSync_drvIEPConfig(timeSyncHandle);

#if defined(SOC_AM572x) || defined(SOC_AM571x) || defined(SOC_AM574x) || defined(SOC_K2G)
#else
            if (TIMESYNC_IP_VER_ICSS_V1 == timeSyncHandle->timeSyncConfig.socConfig.ipVersion)
            {
                status = TimeSync_ecapEdmaConfig(timeSyncHandle->emacHandle,
                                                 &timeSyncHandle->edmaConfig);
            }
#endif
            if (status == TIMESYNC_OK)
            {
                ptpCtrlPtr = (uint8_t *)((((ICSS_EmacHwAttrs *)
                                           (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg->sharedDataRamBaseAddr) +
                                         TIMESYNC_CTRL_VAR_OFFSET);

                /*Encode the forced two-step mode value, this tells firmware
                 * to treat a 1-step sync as 2-step and process accordingly */
                if (timeSyncHandle->timeSyncConfig.protoCfg.stepMode == TIMESYNC_DOUBLE_STEP)
                {
                    byteVal |= (1U << 1U);
                }

                *(ptpCtrlPtr) = byteVal;
            }

            if (status == TIMESYNC_OK)
            {
                status = TimeSync_registerTxIntr(timeSyncHandle);
            }

            if (status == TIMESYNC_OK)
            {
                status = TimeSync_registerLatchIntr(timeSyncHandle);
            }
        }
        else
        {
            status = TIMESYNC_PARAM_INVALID;
        }
    }
    else
    {
        status = TIMESYNC_PARAM_INVALID;
    }

    return (status == TIMESYNC_OK ? (timeSyncHandle) : NULL);
}

int32_t TimeSync_deInit(TimeSync_Handle timeSyncHandle)
{
    volatile uint8_t *ptpCtrlPtr = NULL;

    ptpCtrlPtr = (uint8_t *)((((ICSS_EmacHwAttrs *)(timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg->sharedDataRamBaseAddr) +
                             TIMESYNC_CTRL_VAR_OFFSET);
    (*ptpCtrlPtr) = FALSE;

    return TIMESYNC_OK;
}

int32_t TimeSync_getRxTimestamp(TimeSync_Handle timeSyncHandle,
                                TimeSync_FrameType rxFrameType,
                                uint8_t rxPort,
                                uint16_t seqId,
                                uint32_t *nanoseconds,
                                uint64_t *seconds)
{
    uint32_t timeStampOffsetAddr = 0U;
    uint8_t *bytePtr = NULL;
    uint64_t Nanoseconds64 = 0U;
    ICSS_EmacBaseAddressHandle_T emacBaseAddr;

    if (timeSyncHandle->emacHandle == NULL)
    {
        return TIMESYNC_HANDLE_NOT_INITIALIZED;
    }

    emacBaseAddr = ((ICSS_EmacHwAttrs *)(timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg;

    /* Input error check */
    if (rxFrameType == TIMESYNC_PDELAY_REQ_FRAME)
    {
        rxFrameType = TIMESYNC_DELAY_REQ_FRAME;
    }
    if (rxFrameType == TIMESYNC_PDELAY_RESP_FRAME)
    {
        rxFrameType = TIMESYNC_DELAY_RESP_FRAME;
    }
    if ((rxFrameType != TIMESYNC_SYNC_FRAME) &&
        (rxFrameType != TIMESYNC_DELAY_REQ_FRAME) &&
        (rxFrameType != TIMESYNC_DELAY_RESP_FRAME))
    {
        return TIMESYNC_RX_FRAMETYPE_NOTVALID;
    }

    if ((rxPort != ICSS_EMAC_PORT_1) &&
        (rxPort != ICSS_EMAC_PORT_2))
    {
        return TIMESYNC_RX_PORTNUMBER_NOTVALID;
    }

    if (rxFrameType == TIMESYNC_SYNC_FRAME)
    {
        if (rxPort == ICSS_EMAC_PORT_1)
        {
            timeStampOffsetAddr = RX_SYNC_TIMESTAMP_OFFSET_P1;
        }
        else
        {
            timeStampOffsetAddr = RX_SYNC_TIMESTAMP_OFFSET_P2;
        }
    }
    else if (rxFrameType == TIMESYNC_DELAY_REQ_FRAME)
    {
        if (rxPort == ICSS_EMAC_PORT_1)
        {
            timeStampOffsetAddr = RX_PDELAY_REQ_TIMESTAMP_OFFSET_P1;
        }
        else
        {
            timeStampOffsetAddr = RX_PDELAY_REQ_TIMESTAMP_OFFSET_P2;
        }
    }
    else if (rxFrameType == TIMESYNC_DELAY_RESP_FRAME)
    {
        if (rxPort == ICSS_EMAC_PORT_1)
        {
            timeStampOffsetAddr = RX_PDELAY_RESP_TIMESTAMP_OFFSET_P1;
        }
        else
        {
            timeStampOffsetAddr = RX_PDELAY_RESP_TIMESTAMP_OFFSET_P2;
        }
    }

    bytePtr = (uint8_t *)((uint32_t)(emacBaseAddr->sharedDataRamBaseAddr +
                                     timeStampOffsetAddr));

    if (TIMESYNC_IP_VER_ICSS_V2 == timeSyncHandle->timeSyncConfig.socConfig.ipVersion)
    {
        memcpy(&Nanoseconds64, bytePtr, 8);
        *nanoseconds = (uint32_t)(Nanoseconds64 % (uint64_t)TIMESYNC_SEC_TO_NS);
        *seconds = Nanoseconds64 / (uint64_t)TIMESYNC_SEC_TO_NS;
    }
    else
    {
        memcpy(nanoseconds, bytePtr, 4);
        memcpy(seconds, bytePtr + 4, 6);
    }

    bytePtr  = (uint8_t *)(emacBaseAddr->sharedDataRamBaseAddr + DUT_IS_MASTER_OFFSET);
    *bytePtr = 0;

    return TIMESYNC_OK;
}

int32_t TimeSync_getTxTimestamp(TimeSync_Handle timeSyncHandle,
                                TimeSync_FrameType txFrameType,
                                uint8_t txPort,
                                uint16_t seqId,
                                uint32_t *nanoseconds,
                                uint64_t *seconds)
{
    int32_t status = TIMESYNC_OK;
    uint32_t timeStampOffsetAddr = 0;
    uint8_t *bytePtr;
    uint64_t Nanoseconds64;
    ICSS_EmacBaseAddressHandle_T emacBaseAddr;

    if (timeSyncHandle == NULL)
    {
        status = TIMESYNC_HANDLE_NOT_INITIALIZED;
    }
    else if (timeSyncHandle->emacHandle == NULL)
    {
        status = TIMESYNC_HANDLE_NOT_INITIALIZED;
    }

    if (status == TIMESYNC_OK)
    {
        emacBaseAddr = ((ICSS_EmacHwAttrs *)(timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg;

        /* Input error check */
        if (txFrameType == TIMESYNC_PDELAY_REQ_FRAME)
        {
            txFrameType = TIMESYNC_DELAY_REQ_FRAME;
        }
        if (txFrameType == TIMESYNC_PDELAY_RESP_FRAME)
        {
            txFrameType = TIMESYNC_DELAY_RESP_FRAME;
        }

        if ((txFrameType != TIMESYNC_SYNC_FRAME) &&
            (txFrameType != TIMESYNC_DELAY_REQ_FRAME) &&
            (txFrameType != TIMESYNC_DELAY_RESP_FRAME))
        {
            return TIMESYNC_TX_FRAMETYPE_NOTVALID;    /* not a valid frameType value */
        }

        if ((txPort != ICSS_EMAC_PORT_1) && (txPort != ICSS_EMAC_PORT_2))
        {
            return TIMESYNC_TX_PORTNUMBER_NOTVALID;    /* not a valid port number */
        }

        /* Read timestamp */
        if (txFrameType == TIMESYNC_SYNC_FRAME)
        {
            if (txPort == ICSS_EMAC_PORT_1)
            {
                timeStampOffsetAddr = TX_SYNC_TIMESTAMP_OFFSET_P1;
            }
            else
            {
                timeStampOffsetAddr = TX_SYNC_TIMESTAMP_OFFSET_P2;
            }
        }
        else if (txFrameType == TIMESYNC_DELAY_REQ_FRAME)
        {
            if (txPort == ICSS_EMAC_PORT_1)
            {
                timeStampOffsetAddr = TX_PDELAY_REQ_TIMESTAMP_OFFSET_P1;
            }
            else
            {
                timeStampOffsetAddr = TX_PDELAY_REQ_TIMESTAMP_OFFSET_P2;
            }
        }
        else if (txFrameType == TIMESYNC_DELAY_RESP_FRAME)
        {
            if (txPort == ICSS_EMAC_PORT_1)
            {
                timeStampOffsetAddr = TX_PDELAY_RESP_TIMESTAMP_OFFSET_P1;
            }
            else
            {
                timeStampOffsetAddr = TX_PDELAY_RESP_TIMESTAMP_OFFSET_P2;
            }
        }

        bytePtr = (uint8_t *)((uint32_t)(emacBaseAddr->sharedDataRamBaseAddr +
                                         timeStampOffsetAddr));

        /*Return timestamp */
        if (TIMESYNC_IP_VER_ICSS_V2 == timeSyncHandle->timeSyncConfig.socConfig.ipVersion)
        {
            memcpy(&Nanoseconds64, bytePtr, 8);
            *nanoseconds = (uint32_t)(Nanoseconds64 % (uint64_t)TIMESYNC_SEC_TO_NS);
            *seconds = Nanoseconds64 / (uint64_t)TIMESYNC_SEC_TO_NS;
        }
        else
        {
            memcpy(nanoseconds, bytePtr, 4);
            memcpy(seconds, bytePtr + 4, 6);
        }
    }

    return status;
}

int32_t TimeSync_adjTimeSlowComp(TimeSync_Handle timeSyncHandle,
                                 int32_t adjOffset,
                                 uint32_t syncInterval)
{
    if (timeSyncHandle->emacHandle == NULL)
    {
        return TIMESYNC_HANDLE_NOT_INITIALIZED;
    }

#if defined(SOC_AM335x) || defined(SOC_AM437x)
    /*Do adjustment*/
    if (TIMESYNC_IP_VER_ICSS_V1 == timeSyncHandle->timeSyncConfig.socConfig.ipVersion)
    {
        TimeSync_adjTimeEcapEdma(timeSyncHandle->emacHandle,
                                 &timeSyncHandle->edmaConfig,
                                 adjOffset,
                                 syncInterval);
    }

#else
    uint32_t compensationPeriod = 0U;
    ICSS_EmacBaseAddressHandle_T emacBaseAddr = ((ICSS_EmacHwAttrs *)(timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg;

    /* set compensation interval = sync interval/drift */
    compensationPeriod  = (uint32_t)((double)(syncInterval) /
                                      (double)(abs(adjOffset)));
    /*No compensation required*/
    if (adjOffset == 0)
    {
        /* set compensation increment = 5ns (default val)*/
        HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_GLOBAL_CFG_REG)  = 0x00000551;
        HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_SLOW_COMPEN_REG) = 0;
    }
    else if (adjOffset < 0) /* slave is faster*/
    {
        /* set compensation increment = 10ns (default val)*/
        HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_GLOBAL_CFG_REG)  = 0x00000A51;
        HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_SLOW_COMPEN_REG) = compensationPeriod;
    }
    else    /* master is faster*/
    {
        /* set compensation increment = 0ns*/
        HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_GLOBAL_CFG_REG)  = 0x00000051;
        HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_SLOW_COMPEN_REG) = compensationPeriod;
    }
#endif

    return TIMESYNC_OK;
}

int32_t TimeSync_setClockTime(TimeSync_Handle timeSyncHandle,
                              uint32_t nanoseconds,
                              uint64_t seconds)
{
    uint64_t newCycleCounter;
    uint32_t iepBaseAddress;
    uint32_t sharedRAMbaseAddress;
    uint8_t *bytePtr;

    if (timeSyncHandle->emacHandle == NULL)
    {
        return TIMESYNC_HANDLE_NOT_INITIALIZED;
    }

    /*Write it to IEP register*/
    iepBaseAddress = (((ICSSEMAC_HwAttrs *)
                       (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->prussIepRegs;
    sharedRAMbaseAddress = (((ICSS_EmacHwAttrs *)
                             (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr;

    bytePtr = (uint8_t *)((uint32_t)(iepBaseAddress + CSL_ICSSIEP_COUNT_REG0));

    if (TIMESYNC_IP_VER_ICSS_V1 == timeSyncHandle->timeSyncConfig.socConfig.ipVersion)
    {
        /*Write nanoseconds value into IEP register*/
        memcpy(bytePtr, &nanoseconds, 4);
        /*Write seconds value into seconds counter*/
        bytePtr = (uint8_t *)((uint32_t)(sharedRAMbaseAddress +
                                         TIMESYNC_SECONDS_COUNT_OFFSET));
        memcpy(bytePtr, &seconds, 6);
    }
    else
    {
        /*Calculate the absolute time in nanoseconds*/
        newCycleCounter = seconds *
                          (uint64_t)TIMESYNC_SEC_TO_NS +
                          (uint64_t)nanoseconds;
        memcpy(bytePtr, &newCycleCounter, 8);
    }

    return TIMESYNC_OK;
}

void TimeSync_getCurrentTime(TimeSync_Handle timeSyncHandle,
                             uint32_t *nanoseconds,
                             uint64_t *seconds)
{
    uint32_t iepBaseAddress;
    uint32_t sharedRAMbaseAddress;
    uint8_t *bytePtr;
    uint64_t seconds_and_nanosec;

    iepBaseAddress = (((ICSSEMAC_HwAttrs *)
                       (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->prussIepRegs;
    sharedRAMbaseAddress = (((ICSSEMAC_HwAttrs *)
                             (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr;

    if (TIMESYNC_IP_VER_ICSS_V1 == timeSyncHandle->timeSyncConfig.socConfig.ipVersion)
    {
        bytePtr = (uint8_t *)((uint32_t)(iepBaseAddress + CSL_ICSSIEP_COUNT_REG0));
        memcpy(nanoseconds, bytePtr, 4);
        bytePtr = (uint8_t *)((uint32_t)(sharedRAMbaseAddress +
                                         TIMESYNC_SECONDS_COUNT_OFFSET));
        memcpy(seconds, bytePtr, 6);
    }
    else
    {
        bytePtr = (uint8_t *)((uint32_t)(iepBaseAddress + CSL_ICSSIEP_COUNT_REG0));
        memcpy(&seconds_and_nanosec, bytePtr, 8);
        *nanoseconds = (uint32_t)(seconds_and_nanosec % (uint64_t)TIMESYNC_SEC_TO_NS);
        *seconds = seconds_and_nanosec / (uint64_t)TIMESYNC_SEC_TO_NS;
    }
}

int32_t TimeSync_getPtpFrame(TimeSync_Handle timeSyncHandle,
                             uint8_t *frame,
                             uint32_t *size,
                             uint8_t *rxPort)
{
    int32_t status = TIMESYNC_OK;
    ICSS_EmacRxArgument rxArg;
    ICSSEMAC_Handle icssEmacHandle;
    uint8_t queueNumber;

    if (timeSyncHandle != NULL)
    {
        if (timeSyncHandle->emacHandle != NULL)
        {
            queueNumber = timeSyncHandle->rxPktInfo.rxPktInfoArr[timeSyncHandle->rxPktInfo.rdIdx++].rxQnum;
            timeSyncHandle->rxPktInfo.rdIdx %= TIMESYNC_RX_PKTINFO_COUNT;

            icssEmacHandle = timeSyncHandle->emacHandle;
            rxArg.icssEmacHandle = icssEmacHandle;
            rxArg.destAddress = (uint32_t)(frame);
            rxArg.more = 0;
            rxArg.queueNumber = queueNumber;
            rxArg.port = 0;

            if (((((ICSS_EmacObject *)
                   icssEmacHandle->object)->callBackHandle)->rxCallBack)->callBack != NULL)
            {
                *size = ((((ICSS_EmacObject *)
                           icssEmacHandle->object)->callBackHandle)->rxCallBack)->callBack(&rxArg,
                                                                                           ((((ICSS_EmacObject *)
                                                                                              icssEmacHandle->object)->callBackHandle)->rxCallBack)->userArg);
                *rxPort = rxArg.port;
            }
        }
        else
        {
            status = TIMESYNC_HANDLE_NOT_INITIALIZED;
        }
    }
    else
    {
        status = TIMESYNC_HANDLE_NOT_INITIALIZED;
    }

    return status;
}

int32_t TimeSync_sendPtpFrame(TimeSync_Handle timeSyncHandle,
                              uint8_t *frame,
                              uint32_t size,
                              uint8_t txPort)
{
    int32_t status = TIMESYNC_OK;
    ICSS_EmacTxArgument txArg;
    ICSSEMAC_Handle icssEmacHandle;
    uint32_t quePri = 0U;

    if (timeSyncHandle != NULL)
    {
        if (timeSyncHandle->emacHandle != NULL)
        {
            icssEmacHandle = timeSyncHandle->emacHandle;

            if (txPort == ICSS_EMAC_PORT_1)
            {
                quePri = ICSS_EMAC_QUEUE1;
            }
            else
            {
                quePri = ICSS_EMAC_QUEUE3;
            }

            txArg.icssEmacHandle = icssEmacHandle;
            txArg.lengthOfPacket = size;
            txArg.portNumber = txPort;
            txArg.queuePriority = quePri;
            txArg.srcAddress = frame;

            status = ((((ICSS_EmacObject *)
                        icssEmacHandle->object)->callBackHandle)->txCallBack)->callBack(&txArg,
                                                                                        ((((ICSS_EmacObject *)
                                                                                           icssEmacHandle->object)->callBackHandle)->txCallBack)->userArg);
        }
        else
        {
            status = TIMESYNC_HANDLE_NOT_INITIALIZED;
        }
    }
    else
    {
        status = TIMESYNC_HANDLE_NOT_INITIALIZED;
    }

    return status;
}

void TimeSync_recalibratePps(TimeSync_Handle timeSyncHandle)
{
    uint64_t doubleWord = 0;
    uint64_t remainder  = 0;
    uint32_t word = 0;
    uint8_t *bytePtr;
    ICSS_EmacBaseAddressHandle_T emacBaseAddr;

    emacBaseAddr = ((ICSS_EmacHwAttrs *)(timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg;
    /*First disable CMP1*/
    word = HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_CMP_CFG_REG);
    /*clear the CMP1 enable bit and store*/
    word &= ~((uint32_t)(4));
    HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_CMP_CFG_REG) = word;

    /*Read IEP value and re-program CMP1*/
    bytePtr = (uint8_t *)(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_COUNT_REG0);
    memcpy(&doubleWord, bytePtr, 8);

    /*We need to make sure that IEP aligns with the start of second boundary
     * This is done by finding the distance to second boundary and setting cmp1
     * to that value. An extra second is added as a margin of safety*/
    remainder = doubleWord % (uint64_t)TIMESYNC_SEC_TO_NS;

    if (remainder != 0)
    {
        doubleWord += (TIMESYNC_SEC_TO_NS - remainder) + TIMESYNC_SEC_TO_NS;
    }

    /*now write this value to CMP1 register*/
    bytePtr = (uint8_t *)(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_CMP1_REG0);
    memcpy(bytePtr, &doubleWord, 8);

    bytePtr = (uint8_t *)((((ICSSEMAC_HwAttrs *)
                            (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr
                          + TIMESYNC_CMP1_CMP_OFFSET);
    memcpy(bytePtr, &doubleWord, 8);

    /*Enable CMP1*/
    word = HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_CMP_CFG_REG);
    /*enable CMP1 bit and store back*/
    word |= 0x4;
    HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_CMP_CFG_REG) = word;
}

int32_t TimeSync_getPpsTs(TimeSync_Handle timeSyncHandle,
                          uint32_t *nanoseconds,
                          uint64_t *seconds)
{
    uint32_t iepBaseAddress;
    uint8_t *bytePtr;
    uint64_t seconds_and_nanosec;

    if (timeSyncHandle == NULL)
    {
        return TIMESYNC_HANDLE_NOT_INITIALIZED;
    }

    iepBaseAddress = (((ICSSEMAC_HwAttrs *)
                       (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->prussIepRegs;

    if (TIMESYNC_IP_VER_ICSS_V2 == timeSyncHandle->timeSyncConfig.socConfig.ipVersion)
    {
        bytePtr = (uint8_t *)((uint32_t)(iepBaseAddress + CSL_ICSSIEP_CAPR6_REG0));
        memcpy(&seconds_and_nanosec, bytePtr, 8);
        *nanoseconds = (uint32_t)(seconds_and_nanosec % (uint64_t)TIMESYNC_SEC_TO_NS);
        *seconds = seconds_and_nanosec / (uint64_t)TIMESYNC_SEC_TO_NS;
    }

    /*V1 or AM3/AM4 ICSS is not supported right now since it will require special handling for seconds component*/
    if (TIMESYNC_IP_VER_ICSS_V1 == timeSyncHandle->timeSyncConfig.socConfig.ipVersion)
    {
        return TIMESYNC_FEATURE_NOT_ENABLED;
    }

    return TIMESYNC_OK;
}

int32_t TimeSync_isSingleStepSupported(TimeSync_Handle timeSyncHandle)
{
    int32_t retVal = FALSE;

    if (timeSyncHandle != NULL)
    {
        if (TIMESYNC_IP_VER_ICSS_V2 == timeSyncHandle->timeSyncConfig.socConfig.ipVersion)
        {
            return TRUE;
        }
    }

    return retVal;
}

int8_t TimeSync_isPortLinkUp(TimeSync_Handle timeSyncHandle,
                             uint8_t portNum)
{
    int8_t isLinkUp = FALSE;

    if (timeSyncHandle != NULL)
    {
        isLinkUp = ((ICSS_EmacObject *)(timeSyncHandle->emacHandle)->object)->linkStatus[portNum];
    }

    return isLinkUp;
}

void TimeSync_reset(TimeSync_Handle timeSyncHandle)
{
    uint8_t *sharedRAMbaseAddress = NULL;
    uint8_t *bytePtr = NULL;
    uint32_t word = 0;

    sharedRAMbaseAddress = (uint8_t *)(((ICSSEMAC_HwAttrs *)
                                        timeSyncHandle->emacHandle->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr;

    memset(sharedRAMbaseAddress + SYNC_MASTER_MAC_OFFSET, 0x0, 6);

    /*reset the master port used in redundant clock*/
    HWREGB(sharedRAMbaseAddress + MASTER_PORT_NUM_OFFSET) = 0;

    if (TIMESYNC_IP_VER_ICSS_V1 == timeSyncHandle->timeSyncConfig.socConfig.ipVersion)
    {
        memset(sharedRAMbaseAddress + TIMESYNC_SECONDS_COUNT_OFFSET, 0x0, 8);
    }

    word = 1 * 1024;
    bytePtr = (uint8_t *)(sharedRAMbaseAddress + TIMESYNC_TC_RCF_OFFSET);
    memcpy(bytePtr, &word, 4);
}

void TimeSync_close(TimeSync_Handle timeSyncHandle)
{

}

/* ========================================================================== */
/*                        IP-specific function declarations                  */
/* ========================================================================== */

void TimeSync_updateParentAddress(TimeSync_Handle timeSyncHandle,
                                  uint8_t *parentMac)
{
    /*write master mac id*/
    volatile uint8_t *macId = NULL;
    uint8_t count = 0;

    macId = (uint8_t *)((((ICSS_EmacHwAttrs *)
                          (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr +
                        SYNC_MASTER_MAC_OFFSET);

    for (count = 0; count < 6; count++)
    {
        macId[count] = parentMac[count];
    }
}

void TimeSync_setRcf(TimeSync_Handle timeSyncHandle,
                     uint32_t rcfValue)
{
    uint8_t *ptrScaledRcf = NULL;

    ptrScaledRcf = (uint8_t *)((((ICSS_EmacHwAttrs *)
                                 (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr +
                               TIMESYNC_TC_RCF_OFFSET);
    /* write into ICSS memory for firmware */
    memcpy(ptrScaledRcf, &rcfValue, 4);
}

void TimeSync_setPathDelay(TimeSync_Handle timeSyncHandle,
                           uint32_t pathDelay,
                           uint8_t portNum)
{
    uint8_t *bytePtr = NULL;
    uint32_t sharedRAMbaseAddress = 0U;

    if (timeSyncHandle != NULL)
    {
        sharedRAMbaseAddress = (((ICSS_EmacHwAttrs *)
                                 (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr;
        if (ICSS_EMAC_PORT_1 == (portNum + 1U))
        {
            bytePtr = (uint8_t *)(sharedRAMbaseAddress + P1_SMA_LINE_DELAY_OFFSET);
        }
        else
        {
            bytePtr = (uint8_t *)(sharedRAMbaseAddress + P2_SMA_LINE_DELAY_OFFSET);
        }

        memcpy(bytePtr, &pathDelay, 4);
    }
}

void TimeSync_writeTsSingleStepSync(TimeSync_Handle timeSyncHandle,
                                    uint8_t portNum)
{
    /*The logic of this function works like this
     * Assume three timestamps Y, Y' and Z
     * Y--------------Y'----------------Z
     * Z is the start of the seconds cycle i.e. nanoseconds value of 0
     * Y denotes current time inside this function which is invoked
     * just before transmitting the sync frame. Y' is the actual Tx SOF timestamp
     * and is guaranteed to be within few microsends/miliseconds ahead of Y.
     * Now three possible cases exist
     * Y-----Z-----Y' in which case Y' lies in the next cycle
     * Y-----Y'----Z  in which case Y' is in current cycle
     * corner cases in which Y == Z and Y' == Z also fall under the above 2
     *
     * In firmware the timestamp Y' is compared against Z and it is determined
     * whether it falls in next or previous cycle. The corresponding seconds value
     * of Z is then used to get the seconds value. Nanoseconds value is obtained by
     * subtraction and long division and remainder operation is avoided.
     */

    uint8_t *bytePtr  = NULL;
    uint64_t iepCount = 0;
    uint64_t seconds  = 0;
    uint32_t iepBaseAddress;
    uint32_t sharedRAMbaseAddress;

    iepBaseAddress = (((ICSSEMAC_HwAttrs *)(timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->prussIepRegs;
    sharedRAMbaseAddress = (((ICSSEMAC_HwAttrs *)timeSyncHandle->emacHandle->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr;

    bytePtr = (uint8_t *)((uint32_t)(iepBaseAddress + CSL_ICSSIEP_COUNT_REG0));
    /*Get current time 64 bit counter and time in seconds or Y*/
    memcpy(&iepCount, bytePtr, 8);
    seconds = iepCount / (uint64_t)TIMESYNC_SEC_TO_NS;

    /*Get next start of cycle TS or Z (see comments)*/
    seconds += 1;
    iepCount = seconds * (uint64_t)TIMESYNC_SEC_TO_NS;

    if (ICSS_EMAC_PORT_1 == (portNum + 1))
    {
        memcpy((uint8_t *)(sharedRAMbaseAddress + SINGLE_STEP_IEP_OFFSET_P1), &iepCount, 8);
        memcpy((uint8_t *)(sharedRAMbaseAddress + SINGLE_STEP_SECONDS_OFFSET_P1), &seconds, 8);
    }
    else
    {
        memcpy((uint8_t *)(sharedRAMbaseAddress + SINGLE_STEP_IEP_OFFSET_P2), &iepCount, 8);
        memcpy((uint8_t *)(sharedRAMbaseAddress + SINGLE_STEP_SECONDS_OFFSET_P2), &seconds, 8);
    }
}

/* ========================================================================== */
/*                          Internal Function Definitions                     */
/* ========================================================================== */

static void TimeSync_dramInit(TimeSync_Handle timeSyncHandle)
{
    uint8_t *bytePtr = NULL;
    uint16_t *halfWordPtr = NULL;
    uint32_t word = 0;
    uint64_t doubleWord = 0;

    /*write MII correction*/
    halfWordPtr = (uint16_t *)((((ICSS_EmacHwAttrs *)
                                 (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr
                               + MII_RX_CORRECTION_OFFSET);

    *halfWordPtr = TIMSYNC_RX_PHY_LATENCY;

    /*write MII correction*/
    halfWordPtr = (uint16_t *)((((ICSSEMAC_HwAttrs *)
                                 (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr
                               +
                               MII_TX_CORRECTION_OFFSET);
    *halfWordPtr = TIMSYNC_TX_PHY_LATENCY;

    /*Set seconds value to 0*/
    bytePtr = (uint8_t *)((((ICSS_EmacHwAttrs *)
                            (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr
                          +
                          TIMESYNC_SECONDS_COUNT_OFFSET);
    memset(bytePtr, 0x0, 6);

    /*Initialize rcf to 1 */
    bytePtr = (uint8_t *)((((ICSS_EmacHwAttrs *)
                            (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr
                          +
                          TIMESYNC_TC_RCF_OFFSET);
    word = 1 * 1024;
    memcpy(bytePtr, &word, 4);

    /*This flag will be set and cleared by firmware */
    /*Write Sync0 period for sync signal generation in PTP memory in shared RAM*/
    bytePtr = (uint8_t *)((((ICSSEMAC_HwAttrs *)
                            (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr
                          +
                          TIMESYNC_SYNC0_WIDTH_OFFSET);

    word = timeSyncHandle->timeSyncConfig.protoCfg.ppsConfig.ppsPulseIntervalNs / TIMESYNC_SYNC0_PERIOD_DIVIDER;
    memcpy(bytePtr, &word, 4);

    /*Write CMP1 period for sync signal generation in PTP memory in shared RAM*/
    bytePtr = (uint8_t *)((((ICSSEMAC_HwAttrs *)
                            (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr
                          +
                          TIMESYNC_CMP1_CMP_OFFSET);
    doubleWord = (uint64_t)timeSyncHandle->timeSyncConfig.protoCfg.ppsConfig.ppsPulseIntervalNs;
    memcpy(bytePtr, &doubleWord, 8);

    bytePtr = (uint8_t *)((((ICSSEMAC_HwAttrs *)
                            (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr
                          +
                          TIMESYNC_CMP1_PERIOD_OFFSET);
    word = timeSyncHandle->timeSyncConfig.protoCfg.ppsConfig.ppsPulseIntervalNs;
    memcpy(bytePtr, &word, 4);

    /*Write Sync0 period for sync signal generation in PTP memory in shared RAM*/
    bytePtr = (uint8_t *)((((ICSSEMAC_HwAttrs *)
                            (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr
                          +
                          TIMESYNC_DOMAIN_NUMBER_LIST);
    /* Configures domainNumber list. Firmware supports 2 domains*/
    memcpy(bytePtr, timeSyncHandle->timeSyncConfig.protoCfg.domainNumber,
           TIMESYNC_NUM_DOMAINS);

    /*Configure 1-step/2-step*/
    bytePtr = (uint8_t *)((((ICSSEMAC_HwAttrs *)
                            (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr
                          +
                          DISABLE_SWITCH_SYNC_RELAY_OFFSET);
    if (timeSyncHandle->timeSyncConfig.protoCfg.stepMode == TIMESYNC_DOUBLE_STEP)
    {
        *bytePtr = 1U;
    }
    else
    {
        *bytePtr = 0U;
    }

    /*Initialize the location which contains information about
     * whether Link local frames will have HSR tag or not.
     * This configures the setting to LL frame without HSR tag
     */
    bytePtr = (uint8_t *)((((ICSSEMAC_HwAttrs *)
                            (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr
                          + LINK_LOCAL_FRAME_HAS_HSR_TAG);
    *(bytePtr) = 0;

    /*Populate the source port identity so delay response frames can be compared against this*/
    bytePtr = (uint8_t *)((((ICSSEMAC_HwAttrs *)
                            (timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg)->sharedDataRamBaseAddr
                          + PTP_CLK_IDENTITY_OFFSET);
    memcpy(bytePtr, timeSyncHandle->timeSyncConfig.protoCfg.clockIdentity, 8);
}

static void TimeSync_drvIEPConfig(TimeSync_Handle timeSyncHandle)
{
    uint32_t iep_reg_val = 0;
    uint8_t *bytePtr = NULL;

    ICSS_EmacBaseAddressHandle_T emacBaseAddr = ((ICSS_EmacHwAttrs *)(timeSyncHandle->emacHandle)->hwAttrs)->emacBaseAddrCfg;

    /*Program compare 1 event value*/
    iep_reg_val = timeSyncHandle->timeSyncConfig.protoCfg.ppsConfig.ppsPulseIntervalNs;
    HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_CMP1_REG0) = iep_reg_val;

    /*Enable and program capture config register
     * We have to enable CAP6R in first event mode*/
    iep_reg_val  = HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_CAP_CFG_REG);
    iep_reg_val |= 0xC0;
    HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_CAP_CFG_REG) = iep_reg_val;

    /*Program Sync 0 Width. The value in register is multiplied by 5 by HW*/
    iep_reg_val = timeSyncHandle->timeSyncConfig.protoCfg.ppsConfig.ppsPulseIntervalNs /
                  (TIMESYNC_SYNC0_PERIOD_DIVIDER * 5);
    HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_SYNC_PWIDTH_REG) = iep_reg_val;

    /*Program Sync 0 Start cycles*/
    iep_reg_val = 0;
    HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_SYNC_START_REG) = iep_reg_val;

    /*Enable Sync 0 and Sync 1 in one-shot mode
     * Sync 1 will follow Sync0*/
    iep_reg_val  = 0;
    iep_reg_val |= 0x7;
    HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_SYNC_CTRL_REG) = iep_reg_val;

    /*Set Sync1 delay to 0 so it follows Sync0 exactly*/
    iep_reg_val = 0;
    HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_SYNC1_DELAY_REG) = iep_reg_val;

    /*Enable compare 1*/
    iep_reg_val = HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_CMP_CFG_REG);
    iep_reg_val = iep_reg_val | 0x4;

    /*We enable reset on CMP0 only for older ICSS versions which
     * have a 32 bit counter */
    if (TIMESYNC_IP_VER_ICSS_V1 == timeSyncHandle->timeSyncConfig.socConfig.ipVersion)
    {
        /*Enable reset*/
        iep_reg_val = iep_reg_val | 0x1;
        /*Enable compare 0*/
        iep_reg_val = iep_reg_val | 0x2;
        /*Program compare 0. Also the cycle count*/
        HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_CMP0_REG0) = (uint32_t)TIMESYNC_SEC_TO_NS;
    }
    else
    {
        /*set default increment value and clear slow compensation reg*/
        HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_GLOBAL_CFG_REG)  = 0x00000551;
        HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_SLOW_COMPEN_REG) = 0;
    }

    /*Reset IEP count to 0 before enabling compare config regs
       This ensures that we don't hit CMP1 with a large value in IEP*/
    bytePtr = (uint8_t *)((uint32_t)(emacBaseAddr->prussIepRegs +
                                     CSL_ICSSIEP_COUNT_REG0));
    memset(bytePtr, 0x0, 8);

    HWREG(emacBaseAddr->prussIepRegs + CSL_ICSSIEP_CMP_CFG_REG) = iep_reg_val;
}

static int32_t TimeSync_registerTxIntr(TimeSync_Handle timeSyncHandle)
{
    OsalRegisterIntrParams_t intrInitParams;
    OsalInterruptRetCode_e regIntrRetCode;
    HwiP_Handle hwiHandle;

    /*Configure Time Sync interrupts*/
#ifdef _TMS320C6X
    timeSyncHandle->txIntNum = 78;
    /* For PRU2 Eth0 */
#elif defined(__TI_ARM_V7M4__)
    timeSyncHandle->txIntNum = 29;
#elif defined(__ARM_ARCH_7A__)
    timeSyncHandle->txIntNum = CSL_armGicGetGicIdForIrqInputLine(123);
#elif
    timeSyncHandle->txIntNum = 24;
#endif

    /*Setup Tx callback interrupt*/
    Osal_RegisterInterrupt_initParams(&intrInitParams);
    intrInitParams.corepacConfig.priority = TIMESYNC_TX_CALLBACK_HWI_PRIORITY;
    intrInitParams.corepacConfig.arg = (uintptr_t)timeSyncHandle;
    intrInitParams.corepacConfig.isrRoutine = TimeSync_notifyTxFxn;
    intrInitParams.corepacConfig.corepacEventNum = timeSyncHandle->txIntNum;

#ifdef _TMS320C6X
    intrInitParams.corepacConfig.intVecNum = OSAL_REGINT_INTVEC_EVENT_COMBINER;
#else
    intrInitParams.corepacConfig.intVecNum = intrInitParams.corepacConfig.corepacEventNum;
#endif

#if defined(__ARM_ARCH_7A__)
    intrInitParams.corepacConfig.triggerSensitivity = OSAL_ARM_GIC_TRIG_TYPE_LEVEL;
#endif

    regIntrRetCode = Osal_RegisterInterrupt(&intrInitParams, &hwiHandle);

    if (regIntrRetCode != OSAL_INT_SUCCESS)
    {
        return TIMESYNC_UNABLE_TO_CREATE_INTERRUPT;
    }

    /*Enable interrupt*/
    Osal_EnableInterrupt(intrInitParams.corepacConfig.corepacEventNum, intrInitParams.corepacConfig.intVecNum);

    return TIMESYNC_OK;
}

static int32_t TimeSync_registerLatchIntr(TimeSync_Handle timeSyncHandle)
{
    OsalRegisterIntrParams_t intrInitParams;
    OsalInterruptRetCode_e regIntrRetCode;
    HwiP_Handle hwiHandle;

    /*Setup latch0 interrupt*/
    Osal_RegisterInterrupt_initParams(&intrInitParams);
    intrInitParams.corepacConfig.priority = TIMESYNC_PTP_LATCH0_HWI_PRIORITY;
    intrInitParams.corepacConfig.arg = (uintptr_t)timeSyncHandle;
    intrInitParams.corepacConfig.isrRoutine = TimeSync_latchIsr;
    intrInitParams.corepacConfig.corepacEventNum = TIMESYNC_PTP_LATCH_INT_NUM;

#ifdef _TMS320C6X
    intrInitParams.corepacConfig.intVecNum = OSAL_REGINT_INTVEC_EVENT_COMBINER;
#else
    intrInitParams.corepacConfig.intVecNum = intrInitParams.corepacConfig.corepacEventNum;
#endif

#if defined(__ARM_ARCH_7A__)
    intrInitParams.corepacConfig.triggerSensitivity = OSAL_ARM_GIC_TRIG_TYPE_LEVEL;
#endif

    regIntrRetCode = Osal_RegisterInterrupt(&intrInitParams, &hwiHandle);

    if (regIntrRetCode != OSAL_INT_SUCCESS)
    {
        return TIMESYNC_UNABLE_TO_CREATE_INTERRUPT;
    }

    /*Enable interrupt*/
    Osal_EnableInterrupt(intrInitParams.corepacConfig.corepacEventNum, intrInitParams.corepacConfig.intVecNum);

    return TIMESYNC_OK;
}
