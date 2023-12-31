/********************************************************************
 * Copyright (C) 2020 Texas Instruments Incorporated.
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
 *
 *  Name        : cslr_mst_tog.h
*/
#ifndef CSLR_MST_TOG_H_
#define CSLR_MST_TOG_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>

/**************************************************************************
* Hardware Region  : Master Timeout Gasket Registers
**************************************************************************/


/**************************************************************************
* Register Overlay Structure
**************************************************************************/
typedef struct {
    volatile uint32_t CONTROL;                /* Control Register */
} CSL_MstTog_Regs;

/**************************************************************************
* Register Macros
**************************************************************************/
#define CSL_MST_TOG_CONTROL                                   (0x00000000U)


/**************************************************************************
* Field Definition Macros
**************************************************************************/
/* VBUSM Master Timeout control MMR fields */
#define CSL_MST_TOG_VAL_MASK        (0x00000007U)
#define CSL_MST_TOG_VAL_SHIFT       (0x00000000U)
#define CSL_MST_TOG_EN_MASK         (0x00008000U)
#define CSL_MST_TOG_EN_SHIFT        (0x0000000FU)
#define CSL_MST_TOG_FORCE_MASK      (0x00FF0000U)
#define CSL_MST_TOG_FORCE_SHIFT     (0x00000010U)


#ifdef __cplusplus
}
#endif
#endif /* CSLR_MST_TOG_H_ */
