/********************************************************************
 * Copyright (C) 2018-2019 Texas Instruments Incorporated.
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
 *  Name        : csl_mcu_armss_ccmr5.h
*/
#ifndef CSL_MCU_ARMSS_CCMR5_H
#define CSL_MCU_ARMSS_CCMR5_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <ti/csl/cslr.h>
#include <ti/csl/csl_types.h>
#include <ti/csl/hw_types.h>
#include <ti/csl/arch/r5/cslr_mcu_armss_ccmr5.h>

/**     
 *
 * @defgroup CSL_MCU_ARMSS_CCMR5_API MCU ARMSS CCMR5 API
 *
CCM for R5F provides APIs to select the operating modes and read the status for

 * -# CPU compare block
 * -# Inactivity monitor
 * -# VIM compare block
 * The APIs for programming the polarity of the CPU signals are also provided.
 * The errors issued by the RTL are routed to the ESM (error signaling module) external to the subsystem.
 *
 */

/**
@defgroup CSL_MCU_ARMSS_CCMR5_FUNCTION  MCU CCMR5 Functions
@ingroup CSL_MCU_ARMSS_CCMR5_API
*/

/**
@defgroup CSL_MCU_ARMSS_CCMR5_ENUM MCU CCMR5 Enumerated Data Types
@ingroup CSL_MCU_ARMSS_CCMR5_API
*/

/**     
 *  @addtogroup CSL_MCU_ARMSS_CCMR5_ENUM
    @{
 *    
 */

/**    
 * @brief This typedef defines the MCU ARMSS CCM R5 register ID type
 *        used under the context of CSL CCMR5 APIs
 *    
 */
typedef enum CSL_McuArmssCcmR5RegId_e
{
    CSL_MCU_ARMSS_CCMR5_CCMSR1_REGID = (0u),
    /** register id for Status Register 1 */
    CSL_MCU_ARMSS_CCMR5_CCMKEYR1_REGID = (1u),
    /** register id for KEY register 1 */
    CSL_MCU_ARMSS_CCMR5_CCMSR2_REGID = (2u),
    /** register id for Status Register 2 */
    CSL_MCU_ARMSS_CCMR5_CCMKEYR2_REGID = (3u),
    /** register id for KEY register 2 */
    CSL_MCU_ARMSS_CCMR5_CCMSR3_REGID = (4u),
    /** register id for Status Register 3 */
    CSL_MCU_ARMSS_CCMR5_CCMKEYR3_REGID = (5u),
    /** register id for KEY register 3 */
    CSL_MCU_ARMSS_CCMR5_POLCNTRL_REGID = (6u)
    /** register id for Polatiry Control Reg */
} CSL_McuArmssCcmR5RegId;

/**    
 * @brief This enum defines the MCU ARMSS CCM R5 module ID type
 *        used under the context of CSL CCMR5 APIs
 *      
 */
typedef enum CSL_McuArmssCcmR5ModuleId_e
{
    CSL_MCU_ARMSS_CCMR5_CPU_MODULE_ID = (0u),
    /** module id for Cpu */
    CSL_MCU_ARMSS_CCMR5_VIM_MODULE_ID = (1u),
    /** module id for VIM */
    CSL_MCU_ARMSS_CCMR5_INACTIVITY_MONITOR_MODULE_ID = (2u),
    /** module id for Inactivity monitor */
    CSL_MCU_ARMSS_CCMR5_INVALID_MODULE_ID = (255u)
    /** invalid module id for Cpu */
} CSL_McuArmssCcmR5ModuleId;


/**    
 * @brief This enum defines the MCU ARMSS CCM R5 Operation Mode Key values
 *        used under the context of CSL CCMR5 APIs
 *      
 */

typedef enum CSL_McuArmssCcmR5OpModeKey_e
{
    CSL_MCU_ARMSS_CCMR5_MKEY_CMP_MODE_ACTIVE = (0u),
    /** compare mode active operation Mode key */
    CSL_MCU_ARMSS_CCMR5_MKEY_SELF_TEST_MODE = (6u),
    /** Self Test active operation Mode key */
    CSL_MCU_ARMSS_CCMR5_MKEY_ERR_FORCE_MODE = (9u),
    /** Error Force mode operation Mode key */
    CSL_MCU_ARMSS_CCMR5_MKEY_SELF_TEST_ERR_FORCE_MODE = (15u)
    /** Self test and Error Force Mode key */
} CSL_McuArmssCcmR5OpModeKey;

/* @} */

/**
 *  \addtogroup CSL_MCU_ARMSS_CCMR5_FUNCTION
 *  @{
 */

/**
 *  \brief Read CCM Regiser
 *
 *  This function reads the contents of a given CCM register
 *  and returns the status of the operation for API caller to indicate
 *  success or failure on the operation.
 *
 *  Consult the CCM module documentation for a description of the
 *  registers under MCU_ARMSS_CCMR5 registers section of the
 *  techinical reference manual http://www.ti.com/lit/pdf/spruid7
 *
 *  \param baseAddress       [IN]  Base address of MCU_ARMSS_CCMR5 registers
 *  \param regId             [IN]  CCMR5 register ID to be read
 *  \param pValToBeRead      [OUT] pointer to address of Register value read
 *  \param pMetaInfo         [OUT] pointer to meta info (Optional)
 *                                 for CSL_EFAIL: reason for API failure as
 *                                 defined under CSL_ErrType_t
 *                                 for CSL_PASS: Not applicable
 * 
 *  \return The CSL error code for the API.
 *                                 Success: CSL_PASS
 *                                 Fail   : CSL_EFAIL
 */
int32_t CSL_armR5ReadCCMRegister (
                   uintptr_t                baseAddress,
                   CSL_McuArmssCcmR5RegId   regId,
                   uint32_t                 *pValToBeRead,
                   int32_t                  *pMetaInfo
);

/**
 *  \brief Configure CCM Regiser
 *
 *  This function configures a given CCM register and returns the status of the 
 *  operation for API caller to indicate success or failure on the operation.
 *
 *  Consult the CCM module documentation for a description of the
 *  registers under MCU_ARMSS_CCMR5 registers section of the 
 *  techinical reference manual http://www.ti.com/lit/pdf/spruid7
 *
 *  \param baseAddress       [IN]  Base address of MCU_ARMSS_CCMR5 registers
 *  \param regId             [IN]  CCMR5 register ID to be configured
 *  \param valToBeWritten    [OUT] Register value to be written to
 *  \param pMetaInfo         [OUT] pointer to meta info (Optional)
 *                                 for CSL_EFAIL: reason for API failure as
 *                                 defined under CSL_ErrType_t
 *                                 for CSL_PASS: Not applicable
 *  \return The CSL error code for the API. 
 *                                 Success: CSL_PASS
 *                                 Fail   : CSL_EFAIL
 */
int32_t CSL_armR5ConfigureCCMRegister (
                   uintptr_t                baseAddress,
                   CSL_McuArmssCcmR5RegId   regId,
                   uint32_t                 valToBeWritten,
                   int32_t                  *pMetaInfo
);

/**
 *  \brief Configure Operating mode for CPU, VIM, CCM Inactivity monitor
 *
 *  This function configures a Operation mode key and returns the status of the 
 *  operation for API caller to indicate success or failure on the operation.
 *
 *  Consult the CCM module documentation for a description of the
 *  registers under MCU_ARMSS_CCMR5 registers section of the 
 *  techinical reference manual http://www.ti.com/lit/pdf/spruid7
 *
 *  \param baseAddress       [IN]  Base address of MCU_ARMSS_CCMR5 registers
 *  \param moduleId          [IN]  CCMR5 module ID to be configured
 *  \param opModeKey         [OUT] Operating mode to be set in KEY register
 *  \param pMetaInfo         [OUT] pointer to meta info (Optional)
 *                                 for CSL_EFAIL: reason for API failure as
 *                                 defined under CSL_ErrType_t
 *                                 for CSL_PASS: Not applicable
 * 
 *  \return The CSL error code for the API. 
 *                                 Success: CSL_PASS
 *                                 Fail   : CSL_EFAIL
 */
int32_t CSL_armR5CCMSetOperationModeKey (
                   uintptr_t                    baseAddress,
                   CSL_McuArmssCcmR5ModuleId    moduleId,
                   CSL_McuArmssCcmR5OpModeKey   opModeKey,
                   int32_t                     *pMetaInfo
);

/**
 *  \brief Read Compare Error status for CPU, VIM, CCM Inactivity monitor
 *
 *  This function reads the a Compare Error status and returns the status of the
 *  operation for API caller to indicate success or failure on the operation.
 *
 *  Consult the CCM module documentation for a description of the
 *  registers under MCU_ARMSS_CCMR5 registers section of the 
 *  techinical reference manual http://www.ti.com/lit/pdf/spruid7
 *
 *  \param baseAddress       [IN]  Base address of MCU_ARMSS_CCMR5 registers
 *  \param moduleId          [IN]  CCMR5 module ID to be configured
 *  \param pCmpError         [OUT] Pointer to Compare Error for the module
 *  \param pMetaInfo         [OUT] pointer to meta info (Optional)
 *                                 for CSL_EFAIL: reason for API failure as
 *                                 defined under CSL_ErrType_t
 *                                 for CSL_PASS: Not applicable
 * 
 *  \return The CSL error code for the API. 
 *                                 Success: CSL_PASS
 *                                 Fail   : CSL_EFAIL
 */
int32_t CSL_armR5CCMGetCompareError (
                   uintptr_t                      baseAddress,
                   CSL_McuArmssCcmR5ModuleId      moduleId,
                   uint32_t                      *pCmpError,
                   int32_t                       *pMetaInfo
);

/**
 *  \brief Get Operation mode key value for CPU, VIM, CCM Inactivity monitor
 *
 *  This function reads the a Operation mode key and returns the status of the 
 *  operation for API caller to indicate success or failure on the operation.
 *
 *  Consult the CCM module documentation for a description of the
 *  registers under MCU_ARMSS_CCMR5 registers section of the 
 *  techinical reference manual http://www.ti.com/lit/pdf/spruid7
 *
 *  \param baseAddress       [IN]  Base address of MCU_ARMSS_CCMR5 registers
 *  \param moduleId          [IN]  CCMR5 module ID to be configured
 *  \param pOpModeKey        [OUT] Pointer to Operation Mode for the module
 *  \param pMetaInfo         [OUT] pointer to meta info (Optional)
 *                                 for CSL_EFAIL: reason for API failure as
 *                                 defined under CSL_ErrType_t
 *                                 for CSL_PASS: Not applicable
 * 
 *  \return The CSL error code for the API. 
 *                                 Success: CSL_PASS
 *                                 Fail   : CSL_EFAIL
 * Note: Please note that we cannot read the value that is written 
 *       to the MKEY register unless the programmed key value is lockstep mode
 * Additional information:
 * Only 4 valid key values are expected to be programmed
 * 1. LOCK_STEP_MODE
 * 2. SELF_TEST_MODE
 * 3. ERR_FORCE_MODE
 * 4. ST_ERR_FORCE_MODE
 * For any other value that is programmed the CCMKEYR register would reflect
 * LOCK_STEP MODE. Also the error forcing/self test error forcing mode operates
 * for 1 cpuclk cycle and after that the key register would reflect the 
 * LOCK STEP mode, even though the ERR FORCE & SELF TEST ERR FORCE
 * mode would not complete in 1 cycle
 */
int32_t CSL_armR5CCMGetOperationModeKey (
                   uintptr_t                      baseAddress,
                   CSL_McuArmssCcmR5ModuleId      moduleId,
                   CSL_McuArmssCcmR5OpModeKey    *pOpModeKey,
                   int32_t                       *pMetaInfo
);

/**
 *  \brief Clear Compare Error value for CPU, VIM, CCM Inactivity monitor
 *
 *  This function clears the a Compare Error Statusand returns the status of the 
 *  operation for API caller to indicate success or failure on the operation.
 *
 *  Consult the CCM module documentation for a description of the
 *  registers under MCU_ARMSS_CCMR5 registers section of the 
 *  techinical reference manual http://www.ti.com/lit/pdf/spruid7
 *
 *  \param baseAddress       [IN]  Base address of MCU_ARMSS_CCMR5 registers
 *  \param moduleId          [IN]  CCMR5 module ID to be configured
 *  \param pMetaInfo         [OUT] pointer to meta info (Optional)
 *                                 for CSL_EFAIL: reason for API failure as
 *                                 defined under CSL_ErrType_t
 *                                 for CSL_PASS: Not applicable
 * 
 *  \return The CSL error code for the API. 
 *                                 Success: CSL_PASS
 *                                 Fail   : CSL_EFAIL
 */
int32_t CSL_armR5CCMClearCompareError (
                   uintptr_t                      baseAddress,
                   CSL_McuArmssCcmR5ModuleId      moduleId,
                   int32_t                       *pMetaInfo
);

/* @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* CSL_MCU_ARMSS_CCMR5_H */
