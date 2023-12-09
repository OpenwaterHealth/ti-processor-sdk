/*
 * System Firmware Trace layer
 *
 * Debug Trace layer APIs
 *
 * Copyright (C) 2018-2020, Texas Instruments Incorporated
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

#ifndef TRACE_H
#define TRACE_H

#include <config.h>
#include <lib/bitops.h>
#include <types/short_types.h>

/*
 * Trace data version of form AA.BBB, where AA is the major version and
 * BBB is the minor version.  The major version must be updated whenever
 * an update to the trace data occurs which breaks backwards compatibility.
 * The minor version must be updated for all other additions.
 *
 * The trace data version is output via the TRACE_BP_ACTION_TRACE_DATA_VERSION
 * action when Baseport initialization completes.  Subsequent
 * TRACE_BP_ACTION_TRACE_DATA_VERSION traces are sent after every N traces,
 * where N is a configurable System Firmware build option.
 *
 * Any System Firmware trace parser utilities should be able to comprehend
 * the trace data based on the trace data version.
 */
#define TRACE_VERSION_MAJOR                     ((u32) 0x03U)
#define TRACE_VERSION_MINOR                     ((u32) 0x003U)

#define TRACE_PRINT_MAX_LENGTH                  255
#define TRACE_NUM_BUFFER_CONVERSION_SIZE        ((sizeof(u32) * 8) + 1)

#define TRACE_LEVEL_ERR                         0U
#define TRACE_LEVEL_WARN                        1U
#define TRACE_LEVEL_INFO                        2U
#define TRACE_LEVEL_DEBUG                       3U

/* Trace Debug Channel Macros */
/**
 * \def TRACE_DEBUG_CHANNEL_BP
 *	Baseport trace debug channel
 * \def TRACE_DEBUG_CHANNEL_SEC
 *	Security trace debug channel
 * \def TRACE_DEBUG_CHANNEL_RM
 *	Resource Management debug channel
 * \def TRACE_DEBUG_CHANNEL_PM
 *      Power Management debug channel
 */
#define TRACE_DEBUG_CHANNEL_BP                  0U
#define TRACE_DEBUG_CHANNEL_SEC                 1U
#define TRACE_DEBUG_CHANNEL_RM                  2U
#define TRACE_DEBUG_CHANNEL_PM                  3U

/** \def TRACE_DEBUG_CHANNEL_COUNT
         Count that matches the number of channels defined above.
*/
#define TRACE_DEBUG_CHANNEL_COUNT               4U

/*
 * Trace debug action codes
 ** A generic debug message.
 */
#define TRACE_DEBUG_ACTION_GENERIC_DEBUG        0x7FU

#define TRACE_DEBUG_DOMAIN_SHIFT                29U
#define TRACE_DEBUG_DOMAIN_MASK                 0xE0000000U

#define TRACE_DEBUG_ACTION_SHIFT                22U
#define TRACE_DEBUG_ACTION_MASK                 0x1FC00000U

#define TRACE_DEBUG_VAL_SHIFT                   0U
#define TRACE_DEBUG_VAL_MASK                    0x003FFFFFU

/* Optional sub-action codes */
#define TRACE_DEBUG_SUB_ACTION_SHIFT            16U
#define TRACE_DEBUG_SUB_ACTION_MASK             0x003F0000U

#define TRACE_DEBUG_SUB_ACTION_VAL_SHIFT        0U
#define TRACE_DEBUG_SUB_ACTION_VAL_MASK         0x0000FFFFU

/* Baseport actions */
#define TRACE_BP_ACTION_INIT_COMPLETE                   0x0U

#define TRACE_BP_ACTION_TISCI_MSG_RECEIVED              0x1U
#define TRACE_BP_ACTION_TISCI_MSG_SENDER_HOST_ID        0x2U
#define TRACE_BP_VAL_TISCI_MSG_RECEIVED_HWI_ID_SHIFT    16U

#define TRACE_BP_ACTION_OSAL_CONTEXT_SWITCH             0xAU
#define TRACE_BP_VAL_OSAL_CONTEXT_SWITCH_ENTER          0x0U
#define TRACE_BP_VAL_OSAL_CONTEXT_SWITCH_RESTORE        0x1U
#define TRACE_BP_VAL_OSAL_CONTEXT_SWITCH_EXIT           0x2U

#define TRACE_BP_ACTION_TRACE_DATA_VERSION              0x10U
#define TRACE_BP_VAL_VERSION_MAJOR_SHIFT                12U

#define TRACE_BP_ACTION_SYSFW_VERSION                   0x11U
#define TRACE_BP_VAL_VERSION_SHIFT                      8U
#define TRACE_BP_VAL_SUBVERSION_SHIFT                   4U

/** RM debug actions */
#define TRACE_RM_ACTION_RM_INIT                         0x00U

#define TRACE_RM_ACTION_RM_CORE_INIT                    0x01U

#define TRACE_RM_ACTION_RA_INIT                         0x03U
#define TRACE_RM_ACTION_RING_CONFIGURE                  0x04U
#define TRACE_RM_ACTION_RING_VALIDATE_INDEX             0x06U
#define TRACE_RM_ACTION_RING_MON_CFG                    0x07U
#define TRACE_RM_ACTION_RING_OES_SET                    0x08U
#define TRACE_RM_ACTION_RING_OES_GET                    0x09U

#define TRACE_RM_ACTION_UDMAP_INIT                      0x0CU
#define TRACE_RM_ACTION_UDMAP_TX_CH_SET_THRD_ID         0x0DU
#define TRACE_RM_ACTION_UDMAP_TX_CH_CFG                 0x0EU
#define TRACE_RM_ACTION_UDMAP_GCFG_CFG                  0x10U
#define TRACE_RM_ACTION_UDMAP_RX_CH_SET_THRD_ID         0x14U
#define TRACE_RM_ACTION_UDMAP_RX_CH_CFG                 0x15U

#define TRACE_RM_ACTION_PROXY_INIT                      0x18U
#define TRACE_RM_ACTION_PROXY_CFG                       0x19U
#define TRACE_RM_ACTION_PROXY_OES_SET                   0x1AU
#define TRACE_RM_ACTION_PROXY_OES_GET                   0x1BU

#define TRACE_RM_ACTION_UDMAP_FLOW_CFG                  0x1DU
#define TRACE_RM_ACTION_UDMAP_FLOW_SZ_CFG               0x1EU
#define TRACE_RM_ACTION_UDMAP_OES_SET                   0x1FU
#define TRACE_RM_ACTION_UDMAP_OES_GET                   0x20U
#define TRACE_RM_ACTION_UDMAP_FLOW_DELEGATE             0x21U

#define TRACE_RM_ACTION_PSIL_INIT                       0x23U
#define TRACE_RM_ACTION_PSIL_PAIR                       0x24U
#define TRACE_RM_ACTION_PSIL_READ                       0x26U
#define TRACE_RM_ACTION_PSIL_WRITE                      0x27U
#define TRACE_RM_ACTION_PSIL_UNPAIR                     0x28U
#define TRACE_RM_ACTION_PSIL_DRU_DST_OFFSET             0x29U

#define TRACE_RM_ACTION_IRQ_INIT                        0x2BU
#define TRACE_RM_ACTION_IRQ_SET                         0x2CU
#define TRACE_RM_ACTION_IRQ_RELEASE                     0x2DU

#define TRACE_RM_ACTION_IRQ_IA_OES_SET                  0x2EU
#define TRACE_RM_ACTION_IRQ_IA_OES_GET                  0x2FU
#define TRACE_RM_ACTION_IRQ_IA_INIT                     0x30U
#define TRACE_RM_ACTION_IRQ_IA_MAP_VINT                 0x31U
#define TRACE_RM_ACTION_IRQ_IA_UNMAP_VINT               0x32U

#define TRACE_RM_ACTION_IRQ_IR_INIT                     0x34U
#define TRACE_RM_ACTION_IRQ_IR_CFG                      0x35U
#define TRACE_RM_ACTION_IRQ_IR_CLR                      0x36U

#define TRACE_RM_ACTION_RESASG_FIREWALL_CFG             0x3BU
#define TRACE_RM_ACTION_RESASG_VALIDATE_RESOURCE        0x3CU

#define TRACE_RM_ACTION_RESOURCE_GET                    0x3DU

#define TRACE_RM_ACTION_DEVGRP_VALIDATE                 0x3EU

#define TRACE_RM_ACTION_FAIL                            0x40U

/** RM debug sub-actions */
#define TRACE_RM_SUB_ACTION_DEVICE_ID                   0x00U
#define TRACE_RM_SUB_ACTION_INDEX                       0x01U
#define TRACE_RM_SUB_ACTION_EVENT                       0x03U
#define TRACE_RM_SUB_ACTION_OES_REG_INDEX               0x04U
#define TRACE_RM_SUB_ACTION_SS_DEVICE_ID                0x05U
#define TRACE_RM_SUB_ACTION_RESASG_UTYPE                0x06U
#define TRACE_RM_SUB_ACTION_VALID_PARAM_HI              0x07U
#define TRACE_RM_SUB_ACTION_VALID_PARAM_LO              0x08U

#define TRACE_RM_SUB_ACTION_RING_VIRTID                 0x0AU
#define TRACE_RM_SUB_ACTION_RING_MODE                   0x0BU
#define TRACE_RM_SUB_ACTION_RING_SIZE                   0x0CU
#define TRACE_RM_SUB_ACTION_RING_BA_LO_HI               0x0DU
#define TRACE_RM_SUB_ACTION_RING_BA_LO_LO               0x0EU
#define TRACE_RM_SUB_ACTION_RING_COUNT_HI               0x0FU
#define TRACE_RM_SUB_ACTION_RING_COUNT_LO               0x10U
#define TRACE_RM_SUB_ACTION_RING_ORDERID                0x11U
#define TRACE_RM_SUB_ACTION_RING_MONITOR_SOURCE         0x12U
#define TRACE_RM_SUB_ACTION_RING_MONITOR_MODE           0x13U
#define TRACE_RM_SUB_ACTION_RING_MONITOR_QUEUE          0x14U

#define TRACE_RM_SUB_ACTION_UDMA_CH_PAUSE_ON_ERR        0x0AU
#define TRACE_RM_SUB_ACTION_UDMA_CH_ATYPE               0x0BU
#define TRACE_RM_SUB_ACTION_UDMA_CH_TYPE                0x0CU
#define TRACE_RM_SUB_ACTION_UDMA_CH_FETCH_SIZE          0x0DU
#define TRACE_RM_SUB_ACTION_UDMA_CH_CQ_QNUM             0x0EU
#define TRACE_RM_SUB_ACTION_UDMA_CH_PRIORITY            0x0FU
#define TRACE_RM_SUB_ACTION_UDMA_CH_QOS                 0x10U
#define TRACE_RM_SUB_ACTION_UDMA_CH_ORDERID             0x11U
#define TRACE_RM_SUB_ACTION_UDMA_CH_SCHED_PRIORITY      0x12U
#define TRACE_RM_SUB_ACTION_UDMA_CH_THREAD_ID           0x1EU
#define TRACE_RM_SUB_ACTION_UDMA_CH_BURST_SIZE          0x1FU

#define TRACE_RM_SUB_ACTION_UDMA_TX_CH_FILT_EINFO       0x20U
#define TRACE_RM_SUB_ACTION_UDMA_TX_CH_FILT_PSWORDS     0x21U
#define TRACE_RM_SUB_ACTION_UDMA_TX_CH_SUPR_TDPKT       0x22U
#define TRACE_RM_SUB_ACTION_UDMA_TX_CH_CREDIT_COUNT     0x23U
#define TRACE_RM_SUB_ACTION_UDMA_TX_CH_FDEPTH           0x24U
#define TRACE_RM_SUB_ACTION_UDMA_TX_CH_TDTYPE           0x25U
#define TRACE_RM_SUB_ACTION_UDMA_TX_CH_EXTENDED_CH_TYPE  0x26U

#define TRACE_RM_SUB_ACTION_UDMA_RX_CH_FLOWID_START     0x20U
#define TRACE_RM_SUB_ACTION_UDMA_RX_CH_FLOWID_COUNT     0x21U
#define TRACE_RM_SUB_ACTION_UDMA_RX_CH_IGNORE_SHORT     0x22U
#define TRACE_RM_SUB_ACTION_UDMA_RX_CH_IGNORE_LONG      0x23U

#define TRACE_RM_SUB_ACTION_UDMA_FLOW_RX_EINFO_PRESENT  0x0AU
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_RX_PSINFO_PRESENT 0x0BU
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_RX_ERROR_HANDLING 0x0CU
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_DESC_TYPE         0x0DU
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_RX_SOP_OFFSET     0x0EU
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_RX_PS_LOCATION    0x0FU
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_SRC_TAG_SEL       0x10U
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_DEST_TAG_SEL      0x11U
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_RX_DEST_QNUM      0x12U
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_RX_FDQ0_SZ0_QNUM  0x13U
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_RX_FDQ1_QNUM      0x14U
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_RX_FDQ2_QNUM      0x15U
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_RX_FDQ3_QNUM      0x16U
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_RX_FDQ0_SZ1_QNUM  0x17U
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_RX_FDQ0_SZ2_QNUM  0x18U
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_RX_FDQ0_SZ3_QNUM  0x19U
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_RX_SIZE_THRESH_EN 0x1AU

#define TRACE_RM_SUB_ACTION_UDMA_FLOW_DELEGATE_SUPPORT  0x0AU
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_DELEGATE_HOST     0x0BU
#define TRACE_RM_SUB_ACTION_UDMA_FLOW_DELEGATE_CLEAR    0x0CU

#define TRACE_RM_SUB_ACTION_PSIL_SRC_THREAD             0x0AU
#define TRACE_RM_SUB_ACTION_PSIL_DST_THREAD             0x0BU
#define TRACE_RM_SUB_ACTION_PSIL_THREAD_ENABLED         0x0CU
#define TRACE_RM_SUB_ACTION_PSIL_THREAD_DISABLED        0x0DU
#define TRACE_RM_SUB_ACTION_PSIL_SRC_THREAD_PEER        0x0EU
#define TRACE_RM_SUB_ACTION_PSIL_THREAD                 0x0FU
#define TRACE_RM_SUB_ACTION_PSIL_THREAD_CFG_REG_ADDR    0x10U
#define TRACE_RM_SUB_ACTION_PSIL_THREAD_CFG_REG_VAL_HI  0x11U
#define TRACE_RM_SUB_ACTION_PSIL_THREAD_CFG_REG_VAL_LO  0x12U

#define TRACE_RM_SUB_ACTION_IRQ_IA_ID                   0x0AU
#define TRACE_RM_SUB_ACTION_IRQ_GLOBAL_EVENT            0x0BU
#define TRACE_RM_SUB_ACTION_IRQ_SECONDARY_HOST          0x0DU
#define TRACE_RM_SUB_ACTION_IRQ_SRC_ID                  0x0EU
#define TRACE_RM_SUB_ACTION_IRQ_SRC_INDEX               0x0FU
#define TRACE_RM_SUB_ACTION_IRQ_DST_ID                  0x10U
#define TRACE_RM_SUB_ACTION_IRQ_VINT                    0x11U
#define TRACE_RM_SUB_ACTION_IRQ_VINT_STATUS_BIT_INDEX   0x12U

#define TRACE_RM_SUB_ACTION_IA_VINT                     0x0AU
#define TRACE_RM_SUB_ACTION_IA_GLOBAL_EVENT             0x0BU
#define TRACE_RM_SUB_ACTION_IA_VINT_STATUS_BIT          0x0CU

#define TRACE_RM_SUB_ACTION_IR_INPUT                    0x0AU
#define TRACE_RM_SUB_ACTION_IR_OUTPUT                   0x0BU

#define TRACE_RM_SUB_ACTION_RESOURCE_GET_TYPE           0x0AU
#define TRACE_RM_SUB_ACTION_RESOURCE_GET_SUBTYPE        0x0BU
#define TRACE_RM_SUB_ACTION_RESOURCE_GET_RANGE_START    0x0CU
#define TRACE_RM_SUB_ACTION_RESOURCE_GET_RANGE_NUM      0x0DU
#define TRACE_RM_SUB_ACTION_RESOURCE_GET_SECONDARY_HOST 0x0EU
#define TRACE_RM_SUB_ACTION_RESOURCE_GET_RANGE_START_SEC 0x0FU
#define TRACE_RM_SUB_ACTION_RESOURCE_GET_RANGE_NUM_SEC  0x10U

#define TRACE_RM_SUB_ACTION_RESASG_VALIDATE_HOST        0x0AU
#define TRACE_RM_SUB_ACTION_RESASG_FWL_ID               0x0DU
#define TRACE_RM_SUB_ACTION_RESASG_FWL_CH               0x0EU

#define TRACE_RM_SUB_ACTION_DEVGRP_BOARDCFG             0x0AU
#define TRACE_RM_SUB_ACTION_DEVGRP_DEVICE               0x0BU

/** PM debug actions */
#define TRACE_PM_ACTION_DEVICE_ON                      0x0U
#define TRACE_PM_ACTION_DEVICE_OFF                     0x1U
#define TRACE_PM_ACTION_CLOCK_ENABLE                   0x2U
#define TRACE_PM_ACTION_CLOCK_DISABLE                  0x3U
#define TRACE_PM_ACTION_CLOCK_SET_RATE                 0x4U
#define TRACE_PM_ACTION_CLOCK_SET_PARENT               0x5U
#define TRACE_PM_ACTION_MSG_RECEIVED                   0x6U
#define TRACE_PM_ACTION_MSG_PARAM_DEV_CLK_ID           0x7U
#define TRACE_PM_ACTION_MSG_PARAM_VAL                  0x8U
#define TRACE_PM_ACTION_WAKE_ARM                       0x9U
#define TRACE_PM_ACTION_WAKE_HANDLER                   0xAU
#define TRACE_PM_ACTION_PD_GET                         0xBU
#define TRACE_PM_ACTION_PD_PUT                         0xCU
#define TRACE_PM_ACTION_SET_LOCAL_RESET                0xDU
#define TRACE_PM_ACTION_MODULE_GET                     0xEU
#define TRACE_PM_ACTION_MODULE_PUT                     0xFU
#define TRACE_PM_ACTION_RETENTION_GET                  0x10U
#define TRACE_PM_ACTION_RETENTION_PUT                  0x11U
#define TRACE_PM_ACTION_PD_INIT                        0x12U
#define TRACE_PM_ACTION_INVALID_PSC_DATA               0x13U
#define TRACE_PM_ACTION_PSC_TRANSITION_TIMEOUT         0x14U
#define TRACE_PM_ACTION_PSC_INVALID_DEP_DATA           0x15U
#define TRACE_PM_ACTION_PSC_RST_DONE_TIMEOUT           0x16U
#define TRACE_PM_ACTION_SET_MODULE_RESET               0x17U
#define TRACE_PM_ACTION_INIT                           0x20U
#define TRACE_PM_ACTION_DEV_INIT                       0x21U
#define TRACE_PM_ACTION_SYSRESET                       0x22U
#define TRACE_PM_ACTION_FAIL                    0x40U
#define TRACE_PM_ACTION_EXCLUSIVE_BUSY                 0x7BU
#define TRACE_PM_ACTION_EXCLUSIVE_DEVICE               0x7CU
#define TRACE_PM_ACTION_INVALID_STATE                  0x7DU
#define TRACE_PM_ACTION_BAD_DEVICE                 0x7EU

#define TRACE_PM_VAL_CLOCK_VAL_SHIFT                10U
#define TRACE_PM_VAL_CLOCK_VAL_MASK                 0x0001FC00U
#define TRACE_PM_VAL_CLOCK_EXP_SHIFT                17U
#define TRACE_PM_VAL_CLOCK_EXP_MASK                 0x003E0000U
#define TRACE_PM_VAL_CLOCK_ID_SHIFT                 0U
#define TRACE_PM_VAL_CLOCK_ID_MASK                  0x000003FFU
#define TRACE_PM_VAL_MAX_FREQ                       (TRACE_PM_VAL_CLOCK_VAL_MASK >> TRACE_PM_VAL_CLOCK_VAL_SHIFT)

#define TRACE_PM_VAL_PSC_SHIFT                      20U
#define TRACE_PM_VAL_PD_SHIFT                       14U
#define TRACE_PM_VAL_LPSC_SHIFT                     14U

#define TRACE_PM_VAL_MAX_PSC_DATA                   0x00003FFFU

#define TRACE_PM_VAL_PD_POS1                        1U
#define TRACE_PM_VAL_PD_POS2                        2U
#define TRACE_PM_VAL_PD_POS3                        3U
#define TRACE_PM_VAL_PD_POS4                        4U
#define TRACE_PM_VAL_PD_POS5                        5U
#define TRACE_PM_VAL_PD_POS6                        6U
#define TRACE_PM_VAL_PD_POS7                        7U

#define TRACE_PM_MSG_CLK_ID_SHIFT                   10U

#define TRACE_PM_VAL_INIT_ERR_MASK                  0xFFFFU
#define TRACE_PM_VAL_INIT_ERR_SHIFT                 0U
#define TRACE_PM_VAL_INIT_IDX_MASK                  0x7U
#define TRACE_PM_VAL_INIT_IDX_SHIFT                 16U
#define TRACE_PM_VAL_INIT_DEFER                     (0x1U << 19)
#define TRACE_PM_VAL_INIT_ERROR                     (0x1U << 20)

#define TRACE_PM_VAL_DEV_INIT_ERR_MASK              0x0FFFU
#define TRACE_PM_VAL_DEV_INIT_ERR_SHIFT             0U
#define TRACE_PM_VAL_DEV_INIT_DEVICE_ID_SHIFT       12U
#define TRACE_PM_VAL_DEV_INIT_DEVICE_ID_MASK        0x3FFU

#define TRACE_PM_ACTION_SYSRESET_DOMAIN_MASK           0x3FC000U
#define TRACE_PM_ACTION_SYSRESET_DOMAIN_SHIFT          14U
#define TRACE_PM_ACTION_SYSRESET_ERR_MASK              0x3FFFU
#define TRACE_PM_ACTION_SYSRESET_ERR_SHIFT             0U
#define TRACE_PM_ACTION_SYSRESET_ERR_VAL_SUCCESS       0x0U
#define TRACE_PM_ACTION_SYSRESET_ERR_VAL_INVALID_ARG   0x1U
#define TRACE_PM_ACTION_SYSRESET_ERR_VAL_WRITE_FAIL    0x2U
#define TRACE_PM_ACTION_SYSRESET_ERR_VAL_TIMEOUT       0x3U
#define TRACE_PM_ACTION_SYSRESET_ERR_VAL_DEINIT_FAIL   0x4U

#define TRACE_PM_VAL_EXCLUSIVE_BUSY_DEVICE_ID_SHIFT 12U
#define TRACE_PM_VAL_EXCLUSIVE_BUSY_DEVICE_ID_MASK  0x003FF000U
#define TRACE_PM_VAL_EXCLUSIVE_BUSY_RHOST_ID_SHIFT  6U
#define TRACE_PM_VAL_EXCLUSIVE_BUSY_RHOST_ID_MASK   0x00000FC0U
#define TRACE_PM_VAL_EXCLUSIVE_BUSY_EHOST_ID_SHIFT  0U
#define TRACE_PM_VAL_EXCLUSIVE_BUSY_EHOST_ID_MASK   0x0000003FU

#define TRACE_PM_VAL_EXCLUSIVE_DEVICE_ID_SHIFT      8U
#define TRACE_PM_VAL_EXCLUSIVE_DEVICE_ID_MASK       0x003FFF00U
#define TRACE_PM_VAL_EXCLUSIVE_HOST_ID_SHIFT        0U
#define TRACE_PM_VAL_EXCLUSIVE_HOST_ID_MASK         0x000000FFU

#define TRACE_SEC_EARLY_INIT                        0x1U
#define TRACE_SEC_INIT                              0x2U
#define TRACE_SEC_VAL_INIT_START                    0x0U
#define TRACE_SEC_VAL_INIT_END                      0x1U

#define TRACE_SEC_ACTION_FAIL                       0x40U

/**
 * \brief Set the level of messages to be output.
 *
 * \param trace_level Level of messages to allow to be printed globally.
 */
void trace_set_print_level(u8 trace_level);

/**
 * \brief Prints a plain string to configured trace destinations.
 *
 * \param trace_level The trace level of this message.
 * \param fmt String to output.
 */
void trace_print(u8 trace_level, const u8 *fmt);

/**
 * \brief Prints a plain string with a u32 in hex format to configured
 *        trace destinations.
 *
 * \param trace_level The trace level of this message.
 * \param fmt String to output.
 * \param arg Value to print in hex format after the string.
 */
void trace_print_arg(u8 trace_level, const u8 *fmt, u32 arg);

/**
 * \brief Prints a single character to configured trace destinations.
 *
 * \param ch Character to output.
 */
#ifdef CONFIG_TRACE
s32 trace_print_output_char(u8 ch);

/**
 * \brief Prints a character to trace destination, use in secure world ONLY.
 *
 * \param ch Character to output.
 */
s32 trace_print_output_char_core(u8 ch);
/**
 * \brief Provide boardcfg values for trace src_enables and dst_enables
 *
 * \param src_enables Value from boardcfg_dbg_cfg data structure.
 * \param dst_enables Value from boardcfg_dbg_cfg data structure.
 */
void trace_reconfigure(u16 src_enables, u16 dst_enables);
#else
static inline s32 trace_print_output_char(u8 ch __attribute__((unused)))
{
	return 0;
}

static inline s32 trace_print_output_char_core(u8 ch __attribute__((unused)))
{
	return 0;
}

static inline void trace_reconfigure(u16 src_enables __attribute__((unused)), u16 dst_enables __attribute__((unused)))
{
}
#endif

/**
 * \brief Outputs a u32 trace debug value to configured trace destinations.
 *
 * \param channel Output channel to use.
 * \param val Trace debug value to output.
 */
void trace_debug(u8 channel, u32 val);

/**
 * \brief Outputs a u32 trace debug value to configured trace destinations.
 *
 * \param channel Output channel to use.
 * \param val Trace debug value to output.
 *
 * This function should be invoked from privileged code.
 */
void trace_debug_core(u8 channel, u32 val);

/**
 * \brief Return the internal trace subsystem error code.
 *
 * If something inside the trace framework fails during a call to a public
 * API, such as a hardware character output, the error is cached inside the
 * trace framework. This call retrieves the internal error code.
 *
 * \return Trace subsystem error code or SUCCESS if no error has occurred.
 */
s32 trace_get_error(void);

/**
 * \brief Initialize the trace subsystem.
 *
 * \return Error code on failure, SUCCESS otherwise.
 */
s32 trace_init(void);

/**
 *   \def TRACE_err(fmt)
 *	  Output a string at trace level TRACE_LEVEL_ERR.
 *   \def TRACE_warn(fmt)
 *	  Output a string at trace level TRACE_LEVEL_WARN.
 *   \def TRACE_info(fmt)
 *	  Output a string at trace level TRACE_LEVEL_INFO.
 *   \def TRACE_debug(fmt)
 *	  Output a string at trace level TRACE_LEVEL_DEBUG.
 *   \def TRACE_err_arg(fmt, arg)
 *	  Output a string at trace level TRACE_LEVEL_ERR with a u32 appended.
 *   \def TRACE_warn_arg(fmt, arg)
 *	  Output a string at trace level TRACE_LEVEL_WARN with a u32 appended.
 *   \def TRACE_info_arg(fmt, arg)
 *	  Output a string at trace level TRACE_LEVEL_INFO with a u32 appended.
 *   \def TRACE_debug_arg(fmt, arg)
 *	  Output a string at trace level TRACE_LEVEL_DEBUG with a u32 appended.
 *   \def bp_trace(action, val)
 *        Output a Baseport Trace Debug Value.
 *   \def sec_trace(action, val)
 *        Output a Security Trace Debug Value.
 *   \def rm_trace(action, val)
 *        Output a Resource Management Trace Debug Value.
 *   \def pm_trace(action, val)
 *        Output a Power Management Trace Debug Value.
 */
#ifdef CONFIG_TRACE
#define TRACE_err(fmt) trace_print(TRACE_LEVEL_ERR, fmt)
#define TRACE_warn(fmt) trace_print(TRACE_LEVEL_WARN, fmt)
#define TRACE_info(fmt) trace_print(TRACE_LEVEL_INFO, fmt)
#define TRACE_debug(fmt) trace_print(TRACE_LEVEL_DEBUG, fmt)
#define TRACE_err_arg(fmt, arg) trace_print_arg(TRACE_LEVEL_ERR, fmt, arg)
#define TRACE_warn_arg(fmt, arg) trace_print_arg(TRACE_LEVEL_WARN, fmt, arg)
#define TRACE_info_arg(fmt, arg) trace_print_arg(TRACE_LEVEL_INFO, fmt, arg)
#define TRACE_debug_arg(fmt, arg) trace_print_arg(TRACE_LEVEL_DEBUG, fmt, arg)

#define bp_trace(action, val) trace_debug(TRACE_DEBUG_CHANNEL_BP, \
					  ((((u32) action) << TRACE_DEBUG_ACTION_SHIFT)	\
					   | (val)))
#define bp_trace_core(action, val) trace_debug_core(TRACE_DEBUG_CHANNEL_BP, \
						    ((((u32) action) << TRACE_DEBUG_ACTION_SHIFT) \
						     | (val)))
#define sec_trace(action, val) trace_debug(TRACE_DEBUG_CHANNEL_SEC, \
					   ((((u32) action) << TRACE_DEBUG_ACTION_SHIFT) \
					    | (val)))
#define rm_trace(action, val) trace_debug(TRACE_DEBUG_CHANNEL_RM, \
					  ((((u32) action) << TRACE_DEBUG_ACTION_SHIFT)	\
					   | (val)))
#define pm_trace(action, val) trace_debug(TRACE_DEBUG_CHANNEL_PM, \
					  ((((u32) action) << TRACE_DEBUG_ACTION_SHIFT)	\
					   | (val)))

#define bp_trace_sub(action, sub_action, val) trace_debug(TRACE_DEBUG_CHANNEL_BP, \
							  ((((u32) action) << TRACE_DEBUG_ACTION_SHIFT)	\
							   | (((u32) sub_action) << TRACE_DEBUG_SUB_ACTION_SHIFT)	\
							   | (val)))
#define bp_trace_core_sub(action, sub_action, val) trace_debug_core(TRACE_DEBUG_CHANNEL_BP, \
								    ((((u32) action) << TRACE_DEBUG_ACTION_SHIFT) \
								     | (((u32) sub_action) << TRACE_DEBUG_SUB_ACTION_SHIFT)	\
								     | (val)))
#define sec_trace_sub(action, sub_action, val) trace_debug(TRACE_DEBUG_CHANNEL_SEC, \
							   ((((u32) action) << TRACE_DEBUG_ACTION_SHIFT) \
							    | (((u32) sub_action) << TRACE_DEBUG_SUB_ACTION_SHIFT)	\
							    | (val)))
#define rm_trace_sub(action, sub_action, val) trace_debug(TRACE_DEBUG_CHANNEL_RM, \
							  ((((u32) action) << TRACE_DEBUG_ACTION_SHIFT)	\
							   | (((u32) sub_action) << TRACE_DEBUG_SUB_ACTION_SHIFT)	\
							   | (val)))
#define pm_trace_sub(action, sub_action, val) trace_debug(TRACE_DEBUG_CHANNEL_PM, \
							  ((((u32) action) << TRACE_DEBUG_ACTION_SHIFT)	\
							   | (((u32) sub_action) << TRACE_DEBUG_SUB_ACTION_SHIFT)	\
							   | (val)))
#else
#define TRACE_err(fmt)
#define TRACE_warn(fmt)
#define TRACE_info(fmt)
#define TRACE_debug(fmt)
#define TRACE_err_arg(fmt, arg)
#define TRACE_warn_arg(fmt, arg)
#define TRACE_info_arg(fmt, arg)
#define TRACE_debug_arg(fmt, arg)

#define bp_trace(action, val)
#define bp_trace_core(action, val)
#define sec_trace(action, val)
#define rm_trace(action, val)
#define pm_trace(action, val)

#define bp_trace_sub(action, sub_action, val)
#define bp_trace_core_sub(action, sub_action, val)
#define sec_trace_sub(action, sub_action, val)
#define rm_trace_sub(action, sub_action, val)
#define pm_trace_sub(action, sub_action, val)
#endif

#endif
