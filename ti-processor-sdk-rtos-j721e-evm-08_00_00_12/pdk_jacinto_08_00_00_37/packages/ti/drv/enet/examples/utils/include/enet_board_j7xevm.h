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
 * \file  enet_board_j7xevm.h
 *
 * \brief This file contains the definitions used for J721E/J7200 boards.
 */

#ifndef ENET_BOARD_J7XEVM_H_
#define ENET_BOARD_J7XEVM_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/enet/examples/utils/include/enet_board.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/*!
 * \brief J7X Common Processor Board (CPB) id.
 */
#define ENETBOARD_CPB_ID                      (ENETPHY_BIT(0U))

/*!
 * \brief J7X Gateway/Ethernet Switch/Industrial Expansion (GESI) board id.
 */
#define ENETBOARD_GESI_ID                     (ENETPHY_BIT(1U))

/*!
 * \brief J7X Quad Port Eth Expansion (QpENet) board id.
 */
#define ENETBOARD_QPENET_ID                   (ENETPHY_BIT(2U))

/*!
 * \brief J7X SGMII board id.
 */
#define ENETBOARD_SGMII_ID                    (ENETPHY_BIT(3U))

/*!
 * \brief J7X dummy board, used for MAC loopback.
 */
#define ENETBOARD_LOOPBACK_ID                 (ENETPHY_BIT(4U))

/*!
 * \brief J721E EVM base configuration.
 *
 * All available PHYs are available to fully utilize CPSW_2G and CPSW_9G
 * capabilities.
 */
#define ENETBOARD_J721E_EVM                   (ENETBOARD_CPB_ID |   \
                                               ENETBOARD_GESI_ID |  \
                                               ENETBOARD_SGMII_ID | \
                                               ENETBOARD_QPENET_ID)

/*!
 * \brief J7200 EVM base configuration.
 *
 * It excludes GESI board as it would cause MAC port conflict with QSGMII
 * board.
 */
#define ENETBOARD_J7200_EVM                   (ENETBOARD_CPB_ID |   \
                                               ENETBOARD_SGMII_ID | \
                                               ENETBOARD_QPENET_ID)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                        Deprecated Function Declarations                    */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* ENET_BOARD_J7XEVM_H_ */
