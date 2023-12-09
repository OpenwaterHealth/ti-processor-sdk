#
# Copyright (c) 2016-2021, Texas Instruments Incorporated
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# *  Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# *  Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# *  Neither the name of Texas Instruments Incorporated nor the names of
#    its contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

# File: osal_component.mk
#       This file is component include make file of OSAL library.
# List of variables set in this file and their purpose:
# <mod>_RELPATH        - This is the relative path of the module, typically from
#                        top-level directory of the package
# <mod>_PATH           - This is the absolute path of the module. It derives from
#                        absolute path of the top-level directory (set in env.mk)
#                        and relative path set above
# <mod>_INCLUDE        - This is the path that has interface header files of the
#                        module. This can be multiple directories (space separated)
# <mod>_PKG_LIST       - Names of the modules (and sub-modules) that are a part
#                        part of this module, including itself.
# <mod>_BOARD_DEPENDENCY - "yes": means the code for this module depends on
#                             board and the compiled obj/lib has to be kept
#                             under <board> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no baord dependent code and hence
#                             the obj/libs are not kept under <board> dir.
# <mod>_CORE_DEPENDENCY     - "yes": means the code for this module depends on
#                             core and the compiled obj/lib has to be kept
#                             under <core> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no core dependent code and hence
#                             the obj/libs are not kept under <core> dir.
# <mod>_APP_STAGE_FILES     - List of source files that belongs to the module
#                             <mod>, but that needs to be compiled at application
#                             build stage (in the context of the app). This is
#                             primarily for link time configurations or if the
#                             source file is dependent on options/defines that are
#                             application dependent. This can be left blank or
#                             not defined at all, in which case, it means there
#                             no source files in the module <mod> that are required
#                             to be compiled in the application build stage.
#
ifeq ($(osal_component_make_include), )

libosal_RTOS_LIST = $(DEFAULT_RTOS_LIST)
libosal_BOARDLIST       = evmAM572x evmAM335x evmAM437x iceK2G idkAM574x idkAM572x idkAM571x idkAM437x am65xx_evm am65xx_idk evmOMAPL137 lcdkOMAPL138 evmK2E evmK2H evmK2K evmK2L j721e_evm j7200_evm am64x_evm tpr12_evm tpr12_qt awr294x_evm j721s2_evm
libosal_SOCLIST         = tda2xx tda2px tda2ex tda3xx dra78x dra72x dra75x am574x am572x am571x k2h k2k k2l k2e k2g c6678 c6657 am437x am335x omapl137 omapl138 am65xx j721e j7200 am64x tpr12 awr294x j721s2
libosal_tirtos_BOARDLIST    = $(libosal_BOARDLIST)
libosal_freertos_BOARDLIST  = am65xx_evm j721e_evm j7200_evm tpr12_evm awr294x_evm
libosal_freertos_SOCLIST    = am65xx j721e j7200 tpr12 awr294x
libosal_safertos_BOARDLIST  = tpr12_evm awr294x_evm
libosal_safertos_SOCLIST    = tpr12 awr294x
libosal_tda2xx_CORELIST = a15_0 ipu1_0
libosal_tda2px_CORELIST = a15_0 ipu1_0
libosal_tda2ex_CORELIST = a15_0 ipu1_0
libosal_tda3xx_CORELIST = ipu1_0
libosal_dra72x_CORELIST = c66x a15_0 ipu1_0
libosal_dra75x_CORELIST = c66x a15_0 ipu1_0
libosal_dra78x_CORELIST = c66x ipu1_0
libosal_am574x_CORELIST = c66x a15_0 ipu1_0
libosal_am572x_CORELIST = c66x a15_0 ipu1_0
libosal_am571x_CORELIST = c66x a15_0 ipu1_0
libosal_k2h_CORELIST    = c66x a15_0
libosal_k2k_CORELIST    = c66x a15_0
libosal_k2l_CORELIST    = c66x a15_0
libosal_k2e_CORELIST    = c66x a15_0
libosal_k2g_CORELIST    = c66x a15_0
libosal_omapl137_CORELIST    = c674x arm9_0
libosal_omapl138_CORELIST    = c674x arm9_0
libosal_c6678_CORELIST  = c66x
libosal_c6657_CORELIST  = c66x
libosal_am437x_CORELIST = a9host
libosal_am335x_CORELIST = a8host
libosal_am65xx_CORELIST = $(DEFAULT_am65xx_CORELIST)
libosal_j721e_CORELIST = $(DEFAULT_j721e_CORELIST)
libosal_j7200_CORELIST = $(DEFAULT_j7200_CORELIST)
libosal_am64x_CORELIST = $(DEFAULT_am64x_CORELIST)
libosal_tpr12_CORELIST = $(DEFAULT_tpr12_CORELIST)
libosal_awr294x_CORELIST = $(DEFAULT_awr294x_CORELIST)
libosal_j721s2_CORELIST = $(DEFAULT_j721s2_CORELIST)
libosal_freertos_am65xx_CORELIST = mcu1_0 mcu1_1
libosal_freertos_j721e_CORELIST = mcu1_0 mcu1_1 mcu2_0 mcu2_1 mcu3_0 mcu3_1 c66xdsp_1 c66xdsp_2
libosal_freertos_j7200_CORELIST = mcu1_0 mcu1_1 mcu2_0 mcu2_1
libosal_freertos_tpr12_CORELIST = $(DEFAULT_tpr12_CORELIST)
libosal_freertos_awr294x_CORELIST = $(DEFAULT_awr294x_CORELIST)
libosal_safertos_tpr12_CORELIST = c66xdsp_1
libosal_safertos_awr294x_CORELIST = c66xdsp_1
libosal_freertos_j721s2_CORELIST = mcu1_0 mcu1_1 mcu2_0 mcu2_1 mcu3_0 mcu3_1

############################
# osal package
# List of components included under osal lib
# The components included here are built and will be part of osal lib
############################
osal_LIB_LIST = osal_nonos osal_nonos_indp
ifeq ($(BUILD_OS_TYPE),tirtos)
osal_LIB_LIST += osal_tirtos osal_tirtos_indp
endif
osal_LIB_LIST += osal_freertos
ifeq ($(SOC),$(filter $(SOC), $(libosal_safertos_SOCLIST)))
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
osal_LIB_LIST += osal_safertos
endif
endif

libosal_LIB_LIST = $(osal_LIB_LIST)

############################
# osal examples
# List of examples under osal
# All the tests mentioned in list are built when test target is called
# List below all examples for allowed values
############################
osal_EXAMPLE_LIST =

#
# OSAL Modules
#

# OSAL NONOS LIB
osal_nonos_COMP_LIST = osal_nonos
osal_nonos_RELPATH = ti/osal
osal_nonos_PATH = $(PDK_OSAL_COMP_PATH)
osal_nonos_LIBNAME = ti.osal
export osal_nonos_LIBNAME
osal_nonos_LIBPATH = $(osal_nonos_PATH)/lib/nonos
export osal_nonos_LIBPATH
osal_nonos_OBJPATH = $(osal_nonos_RELPATH)/osal_nonos
export osal_nonos_OBJPATH
osal_nonos_MAKEFILE = -f build/makefile_nonos.mk
export osal_nonos_MAKEFILE
osal_nonos_PLATFORM_DEPENDENCY = no
osal_nonos_CORE_DEPENDENCY = no
osal_nonos_SOC_DEPENDENCY = yes
export osal_nonos_COMP_LIST
export osal_nonos_PLATFORM_DEPENDENCY
export osal_nonos_CORE_DEPENDENCY
export osal_nonos_SOC_DEPENDENCY
osal_nonos_PKG_LIST = osal_nonos
export osal_nonos_PKG_LIST
osal_nonos_INCLUDE = $(osal_nonos_PATH)
osal_nonos_SOCLIST = $(libosal_SOCLIST)
export osal_nonos_SOCLIST
osal_nonos_$(SOC)_CORELIST = $(libosal_$(SOC)_CORELIST)
ifeq ($(SOC),$(filter $(SOC), j721e))
osal_nonos_$(SOC)_CORELIST += c7x-hostemu
endif
export osal_nonos_$(SOC)_CORELIST

# OSAL NONOS LIB DEVICE INDEPENDENT
osal_nonos_indp_COMP_LIST = osal_nonos_indp
osal_nonos_indp_RELPATH = ti/osal
osal_nonos_indp_PATH = $(PDK_OSAL_COMP_PATH)
osal_nonos_indp_LIBNAME = ti.osal
export osal_nonos_indp_LIBNAME
osal_nonos_indp_LIBPATH = $(osal_nonos_indp_PATH)/lib/nonos
export osal_nonos_indp_LIBPATH
osal_nonos_indp_OBJPATH = $(osal_nonos_indp_RELPATH)/osal_nonos_indp
export osal_nonos_indp_OBJPATH
osal_nonos_indp_MAKEFILE = -f build/makefile_nonos_indp.mk
export osal_nonos_indp_MAKEFILE
osal_nonos_indp_PLATFORM_DEPENDENCY = no
osal_nonos_indp_CORE_DEPENDENCY = no
osal_nonos_indp_SOC_DEPENDENCY = no
export osal_nonos_indp_COMP_LIST
export osal_nonos_indp_PLATFORM_DEPENDENCY
export osal_nonos_indp_CORE_DEPENDENCY
export osal_nonos_indp_SOC_DEPENDENCY
osal_nonos_indp_PKG_LIST = osal_nonos_indp
export osal_nonos_indp_PKG_LIST
osal_nonos_indp_INCLUDE = $(osal_nonos_indp_PATH)
osal_nonos_indp_SOCLIST = 
export osal_nonos_indp_SOCLIST
osal_nonos_indp_$(SOC)_CORELIST = $(libosal_$(SOC)_CORELIST)
export osal_nonos_indp_$(SOC)_CORELIST

# OSAL TIRTOS LIB
osal_tirtos_COMP_LIST = osal_tirtos
osal_tirtos_RELPATH = ti/osal
osal_tirtos_PATH = $(PDK_OSAL_COMP_PATH)
osal_tirtos_LIBNAME = ti.osal
export osal_tirtos_LIBNAME
osal_tirtos_LIBPATH = $(osal_tirtos_PATH)/lib/tirtos
export osal_tirtos_LIBPATH
osal_tirtos_OBJPATH = $(osal_tirtos_RELPATH)/osal_tirtos
export osal_tirtos_OBJPATH
osal_tirtos_MAKEFILE = -f build/makefile_tirtos.mk
export osal_tirtos_MAKEFILE
osal_tirtos_PLATFORM_DEPENDENCY = no
osal_tirtos_CORE_DEPENDENCY = no
osal_tirtos_SOC_DEPENDENCY = yes
export osal_tirtos_COMP_LIST
export osal_tirtos_PLATFORM_DEPENDENCY
export osal_tirtos_CORE_DEPENDENCY
export osal_tirtos_SOC_DEPENDENCY
osal_tirtos_PKG_LIST = osal_tirtos
export osal_tirtos_PKG_LIST
osal_tirtos_INCLUDE = $(osal_tirtos_PATH)
osal_tirtos_SOCLIST = $(libosal_SOCLIST)
export osal_tirtos_SOCLIST
osal_tirtos_$(SOC)_CORELIST = $(libosal_$(SOC)_CORELIST)
export osal_tirtos_$(SOC)_CORELIST

# OSAL TIRTOS DEVICE INDEPENDENT
osal_tirtos_indp_COMP_LIST = osal_tirtos_indp
osal_tirtos_indp_RELPATH = ti/osal
osal_tirtos_indp_PATH = $(PDK_OSAL_COMP_PATH)
osal_tirtos_indp_LIBNAME = ti.osal
export osal_tirtos_indp_LIBNAME
osal_tirtos_indp_LIBPATH = $(osal_tirtos_indp_PATH)/lib/tirtos
export osal_tirtos_indp_LIBPATH
osal_tirtos_indp_OBJPATH = $(osal_tirtos_indp_RELPATH)/osal_tirtos_indp
export osal_tirtos_indp_OBJPATH
osal_tirtos_indp_MAKEFILE = -f build/makefile_tirtos_indp.mk
export osal_tirtos_indp_MAKEFILE
osal_tirtos_indp_PLATFORM_DEPENDENCY = no
osal_tirtos_indp_CORE_DEPENDENCY = no
osal_tirtos_indp_SOC_DEPENDENCY = no
export osal_tirtos_indp_COMP_LIST
export osal_tirtos_indp_PLATFORM_DEPENDENCY
export osal_tirtos_indp_CORE_DEPENDENCY
export osal_tirtos_indp_SOC_DEPENDENCY
osal_tirtos_indp_PKG_LIST = osal_tirtos_indp
export osal_tirtos_indp_PKG_LIST
osal_tirtos_indp_INCLUDE = $(osal_tirtos_indp_PATH)
osal_tirtos_indp_SOCLIST = 
export osal_tirtos_indp_SOCLIST
osal_tirtos_indp_$(SOC)_CORELIST = $(libosal_$(SOC)_CORELIST)
export osal_tirtos_indp_$(SOC)_CORELIST

# OSAL FREE RTOS LIB
osal_freertos_COMP_LIST = osal_freertos
osal_freertos_RELPATH = ti/osal
osal_freertos_PATH = $(PDK_OSAL_COMP_PATH)
osal_freertos_LIBNAME = ti.osal
export osal_freertos_LIBNAME
osal_freertos_LIBPATH = $(osal_freertos_PATH)/lib/freertos
export osal_freertos_LIBPATH
osal_freertos_OBJPATH = $(osal_freertos_RELPATH)/osal_freertos
export osal_freertos_OBJPATH
osal_freertos_MAKEFILE = -f build/makefile_freertos.mk
export osal_freertos_MAKEFILE
osal_freertos_PLATFORM_DEPENDENCY = no
osal_freertos_CORE_DEPENDENCY = no
osal_freertos_SOC_DEPENDENCY = yes
export osal_freertos_COMP_LIST
export osal_freertos_PLATFORM_DEPENDENCY
export osal_freertos_CORE_DEPENDENCY
export osal_freertos_SOC_DEPENDENCY
osal_freertos_PKG_LIST = osal_freertos
export osal_freertos_PKG_LIST
osal_freertos_INCLUDE = $(osal_freertos_PATH)
osal_freertos_SOCLIST = $(libosal_freertos_SOCLIST)
export osal_freertos_SOCLIST
osal_freertos_$(SOC)_CORELIST = $(libosal_freertos_$(SOC)_CORELIST)
export osal_freertos_$(SOC)_CORELIST

# OSAL SAFE RTOS LIB
osal_safertos_COMP_LIST = osal_safertos
osal_safertos_RELPATH = ti/osal
osal_safertos_PATH = $(PDK_OSAL_COMP_PATH)
osal_safertos_LIBNAME = ti.osal
export osal_safertos_LIBNAME
osal_safertos_LIBPATH = $(osal_safertos_PATH)/lib/safertos
export osal_safertos_LIBPATH
osal_safertos_OBJPATH = $(osal_safertos_RELPATH)/osal_safertos
export osal_safertos_OBJPATH
osal_safertos_MAKEFILE = -f build/makefile_safertos.mk
export osal_safertos_MAKEFILE
osal_safertos_PLATFORM_DEPENDENCY = no
osal_safertos_CORE_DEPENDENCY = no
osal_safertos_SOC_DEPENDENCY = yes
export osal_safertos_COMP_LIST
export osal_safertos_PLATFORM_DEPENDENCY
export osal_safertos_CORE_DEPENDENCY
export osal_safertos_SOC_DEPENDENCY
osal_safertos_PKG_LIST = osal_safertos
export osal_safertos_PKG_LIST
osal_safertos_INCLUDE = $(osal_safertos_PATH)
osal_safertos_SOCLIST = $(libosal_safertos_SOCLIST)
export osal_safertos_SOCLIST
osal_safertos_$(SOC)_CORELIST = $(libosal_safertos_$(SOC)_CORELIST)
export osal_safertos_$(SOC)_CORELIST

#
# OSAL Examples
#
OSAL_Baremetal_TestApp_COMP_LIST = OSAL_Baremetal_TestApp
OSAL_Baremetal_TestApp_RELPATH = ti/osal/test/baremetal
OSAL_Baremetal_TestApp_PATH = $(PDK_OSAL_COMP_PATH)/test/baremetal
OSAL_Baremetal_TestApp_BOARD_DEPENDENCY = yes
OSAL_Baremetal_TestApp_CORE_DEPENDENCY = no
export OSAL_Baremetal_TestApp_COMP_LIST
export OSAL_Baremetal_TestApp_BOARD_DEPENDENCY
export OSAL_Baremetal_TestApp_CORE_DEPENDENCY
OSAL_Baremetal_TestApp_PKG_LIST = OSAL_Baremetal_TestApp
OSAL_Baremetal_TestApp_INCLUDE = $(OSAL_Baremetal_TestApp_PATH)
OSAL_Baremetal_TestApp_BOARDLIST = $(libosal_BOARDLIST)
export OSAL_Baremetal_TestApp_BOARDLIST
ifeq ($(SOC),$(filter $(SOC), am65xx j721e j7200 am64x))
OSAL_Baremetal_TestApp_SBL_APPIMAGEGEN = yes
else
OSAL_Baremetal_TestApp_SBL_APPIMAGEGEN = no
endif
export OSAL_Baremetal_TestApp_SBL_APPIMAGEGEN

define OSAL_TestApp_RULE

export OSAL_TestApp_$(1)_COMP_LIST = OSAL_TestApp_$(1)
export OSAL_TestApp_$(1)_RELPATH = ti/osal/test/os_unit_test
export OSAL_TestApp_$(1)_PATH = $(PDK_OSAL_COMP_PATH)/test/os_unit_test
export OSAL_TestApp_$(1)_BOARD_DEPENDENCY = yes
export OSAL_TestApp_$(1)_CORE_DEPENDENCY = no
export OSAL_TestApp_$(1)_XDC_CONFIGURO =  $(if $(findstring tirtos,$(1)),yes,no)
export OSAL_TestApp_$(1)_MAKEFILE = -f makefile BUILD_OS_TYPE=$(1)
export OSAL_TestApp_$(1)_PKG_LIST = OSAL_TestApp_$(1)
export OSAL_TestApp_$(1)_INCLUDE = $(OSAL_TestApp_$(1)_PATH)
export OSAL_TestApp_$(1)_BOARDLIST = $(filter $(DEFAULT_BOARDLIST_$(1)), $(libosal_$(1)_BOARDLIST))
export OSAL_TestApp_$(1)_$(SOC)_CORELIST = $(filter $(DEFAULT_$(SOC)_CORELIST_$(1)), $(libosal_$(SOC)_CORELIST))
ifneq ($(1),$(filter $(1), safertos))
osal_EXAMPLE_LIST += OSAL_TestApp_$(1)
else
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
osal_EXAMPLE_LIST += OSAL_TestApp_$(1)
endif
endif
ifeq ($(SOC),$(filter $(SOC), am65xx j721e j7200 am64x tpr12 awr294x))
export OSAL_TestApp_$(1)_SBL_APPIMAGEGEN = yes
else
export OSAL_TestApp_$(1)_SBL_APPIMAGEGEN = no
endif

endef

OSAL_TestApp_MACRO_LIST := $(foreach curos,$(libosal_RTOS_LIST),$(call OSAL_TestApp_RULE,$(curos)))

$(eval ${OSAL_TestApp_MACRO_LIST})

#Default Core List
OSAL_Baremetal_TestApp_$(SOC)_CORELIST = a15_0 c66x

# Overwrite default core list for below SoCs
ifeq ($(BOARD),$(filter $(BOARD), idkAM574x evmAM572x idkAM572x idkAM571x))
OSAL_Baremetal_TestApp_$(SOC)_CORELIST = a15_0 ipu1_0 c66x
endif

ifeq ($(BOARD),$(filter $(BOARD), evmAM335x))
OSAL_Baremetal_TestApp_$(SOC)_CORELIST = a8host
endif


ifeq ($(BOARD),$(filter $(BOARD), evmAM437x idkAM437x))
OSAL_Baremetal_TestApp_$(SOC)_CORELIST = a9host
endif

ifeq ($(BOARD),$(filter $(BOARD), evmOMAPL137))
osal_baremetal_test_$(SOC)_CORELIST = c674x arm9_0
endif

ifeq ($(BOARD),$(filter $(BOARD), lcdkOMAPL138))
osal_baremetal_test_$(SOC)_CORELIST = c674x arm9_0
endif

ifeq ($(BOARD),$(filter $(BOARD), evmK2E))
osal_baremetal_test_$(SOC)_CORELIST = c66x a15_0
endif

ifeq ($(BOARD),$(filter $(BOARD), evmK2H evmK2K))
osal_baremetal_test_$(SOC)_CORELIST = c66x a15_0
endif

ifeq ($(BOARD),$(filter $(BOARD), evmK2L))
osal_baremetal_test_$(SOC)_CORELIST = c66x a15_0
endif

ifeq ($(BOARD),$(filter $(BOARD), evmOMAPL137))
osal_baremetal_test_$(SOC)_CORELIST = c674x arm9_0
endif

ifeq ($(BOARD),$(filter $(BOARD), lcdkOMAPL138))
osal_baremetal_test_$(SOC)_CORELIST = c674x arm9_0
endif

ifeq ($(BOARD),$(filter $(BOARD), evmK2E))
osal_baremetal_test_$(SOC)_CORELIST = c66x a15_0
endif

ifeq ($(BOARD),$(filter $(BOARD), evmK2H evmK2K))
osal_baremetal_test_$(SOC)_CORELIST = c66x a15_0
endif

ifeq ($(BOARD),$(filter $(BOARD), evmK2L))
osal_baremetal_test_$(SOC)_CORELIST = c66x a15_0
endif

ifeq ($(SOC),$(filter $(SOC), dra72x dra75x))
OSAL_Baremetal_TestApp_$(SOC)_CORELIST = a15_0 ipu1_0 c66x
endif

ifeq ($(SOC),$(filter $(SOC), dra78x))
OSAL_Baremetal_TestApp_$(SOC)_CORELIST = ipu1_0 c66x
endif

ifeq ($(SOC),$(filter $(SOC), am64x))
 OSAL_TestApp_tirtos_$(SOC)_CORELIST = mcu1_0 mpu1_0 mcu2_0
 OSAL_Baremetal_TestApp_$(SOC)_CORELIST = mcu1_0 mpu1_0 m4f_0 mcu2_0
endif

ifeq ($(SOC),$(filter $(SOC), am65xx))
 OSAL_TestApp_tirtos_$(SOC)_CORELIST = mpu1_0 mcu1_0
 OSAL_Baremetal_TestApp_$(SOC)_CORELIST = mpu1_0 mcu1_0
 OSAL_TestApp_freertos_$(SOC)_CORELIST = mcu1_0 mcu1_1
endif

ifeq ($(SOC),$(filter $(SOC), j721e))
 OSAL_TestApp_tirtos_$(SOC)_CORELIST = mpu1_0 mcu1_0 mcu2_0 mcu3_0 c66xdsp_1 c7x_1
 OSAL_Baremetal_TestApp_$(SOC)_CORELIST = mpu1_0 mcu1_0 mcu2_0 mcu3_0
 OSAL_TestApp_freertos_$(SOC)_CORELIST = mcu1_0 mcu1_1 mcu2_0 mcu2_1 mcu3_0 mcu3_1
endif

ifeq ($(SOC),$(filter $(SOC), j7200))
 OSAL_TestApp_tirtos_$(SOC)_CORELIST = mpu1_0 mcu1_0 mcu2_0
 OSAL_Baremetal_TestApp_$(SOC)_CORELIST = mpu1_0 mcu1_0 mcu2_0
 OSAL_TestApp_freertos_$(SOC)_CORELIST = mcu1_0 mcu1_1 mcu2_0 mcu2_1
endif

ifeq ($(SOC),$(filter $(SOC), tpr12 awr294x))
 OSAL_Baremetal_TestApp_$(SOC)_CORELIST = mcu1_0 mcu1_1 c66xdsp_1
endif

export OSAL_Baremetal_TestApp_$(SOC)_CORELIST
export OSAL_TestApp_$(SOC)_CORELIST
export OSAL_freertos_TestApp_$(SOC)_CORELIST

osal_EXAMPLE_LIST += OSAL_Baremetal_TestApp

ifeq ($(CPLUSPLUS_BUILD), yes)
#cpptest
cpptest_COMP_LIST = cpptest
cpptest_RELPATH = ti/osal/test/cpptest
cpptest_PATH = $(PDK_OSAL_COMP_PATH)/test/cpptest
cpptest_BOARD_DEPENDENCY = yes
cpptest_CORE_DEPENDENCY = yes
export cpptest_COMP_LIST
export cpptest_BOARD_DEPENDENCY
export cpptest_CORE_DEPENDENCY
cpptest_PKG_LIST = cpptest
cpptest_INCLUDE = $(cpptest_PATH)
cpptest_BOARDLIST = $(libosal_BOARDLIST)
export cpptest_BOARDLIST

#Default Core List
ifeq ($(BOARD),$(filter $(BOARD), evmAM335x))
cpptest_$(SOC)_CORELIST = a8host
endif

ifeq ($(BOARD),$(filter $(BOARD), evmAM437x idkAM437x))
cpptest_$(SOC)_CORELIST = a9host
endif

ifeq ($(BOARD),$(filter $(BOARD), idkAM574x evmAM572x idkAM572x idkAM571x))
cpptest_$(SOC)_CORELIST = a15_0 ipu1_0 c66x
endif

ifeq ($(BOARD),$(filter $(BOARD), evmK2G iceK2G))
cpptest_$(SOC)_CORELIST = c66x a15_0
endif

export cpptest_$(SOC)_CORELIST
osal_EXAMPLE_LIST += cpptest
endif

export osal_LIB_LIST
export libosal_LIB_LIST
export osal_EXAMPLE_LIST

libosal_EXAMPLE_LIST = $(osal_EXAMPLE_LIST)
export libosal_EXAMPLE_LIST

osal_component_make_include := 1
endif
