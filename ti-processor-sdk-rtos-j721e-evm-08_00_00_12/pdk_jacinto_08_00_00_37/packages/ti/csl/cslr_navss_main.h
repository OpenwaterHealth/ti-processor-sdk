/*
 *  Copyright (C) 2016-2020 Texas Instruments Incorporated.
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
*/
#ifndef CSLR_NAVSS_MAIN_TOP_H_
#define CSLR_NAVSS_MAIN_TOP_H_

#include <ti/csl/soc.h>
#include <ti/csl/csl.h>

#if defined (SOC_AM65XX) || defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J7200)
#if defined (SOC_J721E)
#include <ti/csl/src/ip/navss/V1/cslr_navss_main.h>
#elif defined (SOC_J7200)
#include <ti/csl/src/ip/navss/V2/cslr_navss_main.h>
#elif defined (SOC_J721S2)
#include <ti/csl/src/ip/navss/V3/cslr_navss_main.h>
#endif
#include <ti/csl/cslr_cbass.h>
#include <ti/csl/cslr_cpts.h>
#include <ti/csl/cslr_ecc_aggr.h>
#include <ti/csl/cslr_intaggr.h>
#include <ti/csl/cslr_intr_router.h>
#if defined (SOC_J7200)
#include <ti/csl/cslr_pat.h>
#endif
#include <ti/csl/cslr_proxy.h>
#include <ti/csl/cslr_psilcfg.h>
#include <ti/csl/cslr_pvu.h>
#include <ti/csl/cslr_ringacc.h>
#include <ti/csl/cslr_sec_proxy.h>
#include <ti/csl/cslr_timer_mgr.h>
#include <ti/csl/cslr_udmap.h>
#endif /* defined (SOC_AM65XX) || defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J7200) */

#endif /* CSLR_NAVSS_MAIN_TOP_H_ */
