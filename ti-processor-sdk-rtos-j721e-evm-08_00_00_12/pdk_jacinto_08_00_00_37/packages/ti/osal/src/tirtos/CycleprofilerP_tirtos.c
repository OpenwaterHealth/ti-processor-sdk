/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
 *  ======== CycleprofilerP_tirtos.c ========
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <ti/osal/CycleprofilerP.h>

#if defined(SOC_TPR12) || defined(SOC_AWR294X)

#ifdef _TMS320C6X
#include <c6x.h>
void CycleprofilerP_init(void)
{
    TSCL = 0;
}
uint32_t CycleprofilerP_getTimeStamp(void)
{
    return(TSCL);
}
#endif

#if defined(__ARM_ARCH_7A__) || defined(__TI_ARM_V7R4__)
#include <ti/sysbios/family/arm/v7a/Pmu.h>
void CycleprofilerP_init(void)
{
    Pmu_configureCounter(0, 0x11, FALSE);
    Pmu_startCounter(0);
}
uint32_t CycleprofilerP_getTimeStamp(void) 
{
    return(Pmu_getCount(0));
}
#endif

#else

#include <xdc/runtime/Types.h>
#include <xdc/runtime/Timestamp.h>

void CycleprofilerP_init(void)
{
}
uint32_t CycleprofilerP_getTimeStamp(void)
{
    Types_Timestamp64 bios_timestamp64 = {0U, 0U};
    
    Timestamp_get64(&bios_timestamp64);
    return(bios_timestamp64.lo);
}

#endif


