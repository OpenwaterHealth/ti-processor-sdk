#
# Copyright (c) 2013-2019, Texas Instruments Incorporated
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

# File: component.mk
#       This file is component include make file of PDK.
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
# <mod>_BOARD_DEPENDENCY    - "yes": means the code for this module depends on
#                             board and the compiled obj/lib has to be kept
#                             under <board> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no board dependent code and hence
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
# <mod>_FP_DEPENDENCY       - "yes": means the code for this module depends on
#                             target toolchain floating point support.  Enabling
#                             this option will enable floating point print
#                             support for toolchains which link out floating
#                             point print support to save memory.
#
ifeq ($(pdk_component_make_include), )

include $(PDK_INSTALL_PATH)/ti/build/soc_info.mk

# The below defines the DEFAULT_$(SOC)_CORELIST which are used by various components for an SOC
# This is derived from the CORELIST_$(SOC) defined in soc_info.mk which encompasses all the available cores for a SOC.
# The DEFAULT_$(SOC)_CORELIST is a subset of all the cores and is used for building components.

# Filter out PRU cores from default cores list for building components
DEFAULT_CORELIST_EXCLUDE_CORES = $(CORE_LIST_PRU)

# For J7 cores, mpu1_1 is not a part of default core list
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2))
DEFAULT_CORELIST_EXCLUDE_CORES += mpu1_1
endif

DEFAULT_$(SOC)_CORELIST = $(filter-out $(DEFAULT_CORELIST_EXCLUDE_CORES), $(CORE_LIST_$(SOC)))

# The below defines various RTOS types
DEFAULT_RTOS_LIST = tirtos freertos safertos

# The below defines the DEFAULT_SOCLIST_<rtos_type> for various RTOS types(tirtos/freertos/safertos)
DEFAULT_SOCLIST_tirtos   = $(SOC_LIST_CATALOG) $(SOC_LIST_INFOTAINMENT) $(SOC_LIST_J6_TDA)
DEFAULT_SOCLIST_freertos = am65xx j721e j7200 awr294x
DEFAULT_SOCLIST_safertos = tpr12 awr294x

# The below defines the DEFAULT_BOARDLIST_<rtos_type> for various RTOS types(tirtos/freertos/safertos)
DEFAULT_BOARDLIST_tirtos   = evmDRA72x evmDRA75x evmDRA78x evmAM572x idkAM572x idkAM571x idkAM574x $(BOARD_LIST_J6_TDA) $(BOARD_LIST_J7_TDA) $(BOARD_LIST_TPR12) am64x_evm am64x_svb
DEFAULT_BOARDLIST_freertos = am65xx_evm am65xx_idk j721e_evm j7200_evm tpr12_evm awr294x_evm
DEFAULT_BOARDLIST_safertos = tpr12_evm awr294x_evm

# The below defines the DEFAULT_$(SOC)_CORELIST_<rtos_type> for various RTOS types(tirtos/freertos/safertos)
# This is derived from the DEFAULT_$(SOC)_CORELIST defined above.
# DEFAULT_$(SOC)_CORELIST_<rtos_type> is a subset of all the cores and is used for building components for the particular 'rtos_type'.


DEFAULT_$(SOC)_CORELIST_tirtos = $(DEFAULT_$(SOC)_CORELIST)


ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 am65xx tpr12 awr294x))
DEFAULT_CORELIST_EXCLUDE_CORES_freertos =
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 am65xx))
# FreeRTOS is not supported on mpu core
DEFAULT_CORELIST_EXCLUDE_CORES_freertos += mpu1_0
endif
ifeq ($(SOC),$(filter $(SOC), j721e j721s2))
# FreeRTOS is not currently supported on J7 c66x/c7x cores
DEFAULT_CORELIST_EXCLUDE_CORES_freertos += c66xdsp_1 c66xdsp_2 c7x_1 c7x_2 c7x-hostemu
endif
else
#FreeRTOS is not supported on other SOCs
DEFAULT_CORELIST_EXCLUDE_CORES_freertos = $(DEFAULT_$(SOC)_CORELIST)
endif

DEFAULT_$(SOC)_CORELIST_freertos = $(filter-out $(DEFAULT_CORELIST_EXCLUDE_CORES_freertos), $(DEFAULT_$(SOC)_CORELIST))


ifeq ($(SOC),$(filter $(SOC), tpr12 awr294x))
# SafeRTOS is not currently supported on mcu cores
DEFAULT_CORELIST_EXCLUDE_CORES_safertos = mcu1_0 mcu1_1
else
#SafeRTOS is not supported on other SOCs
DEFAULT_CORELIST_EXCLUDE_CORES_safertos = $(DEFAULT_$(SOC)_CORELIST)
endif

DEFAULT_$(SOC)_CORELIST_safertos = $(filter-out $(DEFAULT_CORELIST_EXCLUDE_CORES_safertos), $(DEFAULT_$(SOC)_CORELIST))


# Core types (without the core IDs). This will be used to parse and order the establish the order of cores
# in the case of building libraries for multiple cores

# Component specific CFLAGS
PDK_CFLAGS = -DBOARD_D3_BB
PDK_LNKFLAGS =

############################
# device pdk package
# List of components included under pdk lib
# The components included here are built and will be part of pdk lib
############################
pdk_LIB_LIST =

############################
# device pdk firmware
# List of components included under pdk firmware
# The components included here are built and will be part of pdk firmware
############################
pdk_FIRM_LIST =

############################
# device pdk application utility packages
# List of application utilities under pdk
# The components included here are built and will be part of pdk app_lib
############################
pdk_APP_LIB_LIST =

############################
# device pdk examples
# List of examples under pdk
# All the tests mentioned in list are built when test target is called
# List below all examples for allowed values
############################
pdk_EXAMPLE_LIST =

############################
# Duplicate example list which should not be built with "all" (since this is
# duplicate -j option will build both and result in .obj correuption) but needs
# to be still exposed to top level makefile so that user can individually call
# them. Also these duplicate targets doesn't support package option and hence
# should not be included when packaging
############################
pdk_DUP_EXAMPLE_LIST =

###########################
# PDK Utils list which are not built on every build
# Example includes the sciclient board configruation.
# This is specifically used when some additional tools are required to build
# these utilities like python.
###########################
pdk_UTILS_LIST =

# Components included independent of OS type

#include each module component makefile
-include $(PDK_SDL_COMP_PATH)/sdl_component.mk
ifneq ($(sdl_LIB_LIST),)
  pdk_LIB_LIST += $(sdl_LIB_LIST)
endif

-include $(PDK_CSL_COMP_PATH)/csl_component.mk
ifneq ($(csl_LIB_LIST),)
  pdk_LIB_LIST += $(csl_LIB_LIST)
endif
ifneq ($(csl_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(csl_APP_LIB_LIST)
endif
ifneq ($(csl_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(csl_EXAMPLE_LIST)
endif
ifneq ($(csl_DUP_EXAMPLE_LIST),)
  pdk_DUP_EXAMPLE_LIST += $(csl_DUP_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_BOARD_COMP_PATH)/board_component.mk
ifneq ($(board_LIB_LIST),)
  pdk_LIB_LIST += $(board_LIB_LIST)
endif
ifneq ($(board_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(board_APP_LIB_LIST)
endif
ifneq ($(board_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(board_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_PM_COMP_PATH)/pm_component.mk
ifneq ($(pm_LIB_LIST),)
  pdk_LIB_LIST += $(pm_LIB_LIST)
endif
ifneq ($(pm_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(pm_APP_LIB_LIST)
endif
ifneq ($(pm_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(pm_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_PMIC_COMP_PATH)/pmic_component.mk
ifneq ($(pmic_LIB_LIST),)
  pdk_LIB_LIST += $(pmic_LIB_LIST)
endif
ifneq ($(pmic_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(pmic_APP_LIB_LIST)
endif
ifneq ($(pmic_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(pmic_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_AUD_COMP_PATH)/aud_component.mk
ifneq ($(aud_LIB_LIST),)
  pdk_LIB_LIST += $(aud_LIB_LIST)
endif
ifneq ($(aud_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(aud_APP_LIB_LIST)
endif
ifneq ($(aud_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(aud_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_SBL_COMP_PATH)/sbl_component.mk
ifneq ($(sbl_LIB_LIST),)
  pdk_LIB_LIST += $(sbl_LIB_LIST)
endif
ifneq ($(sbl_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(sbl_APP_LIB_LIST)
endif
ifneq ($(sbl_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(sbl_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_KEYWR_COMP_PATH)/keywriter_component.mk
ifneq ($(keywriter_LIB_LIST),)
  pdk_LIB_LIST += $(keywriter_LIB_LIST)
endif
ifneq ($(keywriter_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(keywriter_APP_LIB_LIST)
endif
ifneq ($(keywriter_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(keywriter_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_SBL_AUTO_COMP_PATH)/sbl_auto_component.mk
ifneq ($(sbl_auto_LIB_LIST),)
  pdk_LIB_LIST += $(sbl_auto_LIB_LIST)
endif
ifneq ($(sbl_auto_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(sbl_auto_APP_LIB_LIST)
endif
ifneq ($(sbl_auto_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(sbl_auto_EXAMPLE_LIST)
endif
ifneq ($(sbl_auto_DUP_EXAMPLE_LIST),)
  pdk_DUP_EXAMPLE_LIST += $(sbl_auto_DUP_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_CMB_COMP_PATH)/cmb_component.mk
ifneq ($(cmb_LIB_LIST),)
  pdk_LIB_LIST += $(cmb_LIB_LIST)
endif
ifneq ($(cmb_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(cmb_APP_LIB_LIST)
endif
ifneq ($(cmb_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(cmb_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_BOARD_IND_COMP_PATH)/board_ind_component.mk
ifneq ($(board_ind_LIB_LIST),)
  pdk_LIB_LIST += $(board_ind_LIB_LIST)
endif
ifneq ($(board_ind_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(board_ind_APP_LIB_LIST)
endif
ifneq ($(board_ind_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(board_ind_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_UDMA_COMP_PATH)/udma_component.mk
ifneq ($(udma_LIB_LIST),)
  pdk_LIB_LIST += $(udma_LIB_LIST)
endif
ifneq ($(udma_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(udma_APP_LIB_LIST)
endif
ifneq ($(udma_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(udma_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_ENET_COMP_PATH)/enet_component.mk
ifneq ($(enet_LIB_LIST),)
  pdk_LIB_LIST += $(enet_LIB_LIST)
endif
ifneq ($(enet_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(enet_APP_LIB_LIST)
endif
ifneq ($(enet_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(enet_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_LWIP_COMP_PATH)/lwip_component.mk
ifneq ($(lwip_LIB_LIST),)
  pdk_LIB_LIST += $(lwip_LIB_LIST)
endif
ifneq ($(lwip_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(lwip_APP_LIB_LIST)
endif
ifneq ($(lwip_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(lwip_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_CSIRX_COMP_PATH)/csirx_component.mk
ifneq ($(csirx_LIB_LIST),)
  pdk_LIB_LIST += $(csirx_LIB_LIST)
endif
ifneq ($(csirx_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(csirx_APP_LIB_LIST)
endif
ifneq ($(csirx_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(csirx_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_CSITX_COMP_PATH)/csitx_component.mk
ifneq ($(csitx_LIB_LIST),)
  pdk_LIB_LIST += $(csitx_LIB_LIST)
endif
ifneq ($(csitx_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(csitx_APP_LIB_LIST)
endif
ifneq ($(csitx_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(csitx_EXAMPLE_LIST)
endif

# fvid2_component.mk should be included before any video components
# - used to ignore include if component not present
-include $(PDK_FVID2_COMP_PATH)/fvid2_component.mk
ifneq ($(fvid2_LIB_LIST),)
  pdk_LIB_LIST += $(fvid2_LIB_LIST)
endif
ifneq ($(fvid2_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(fvid2_APP_LIB_LIST)
endif
ifneq ($(fvid2_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(fvid2_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_DSS_COMP_PATH)/dss_component.mk
ifneq ($(dss_LIB_LIST),)
  pdk_LIB_LIST += $(dss_LIB_LIST)
endif
ifneq ($(dss_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(dss_APP_LIB_LIST)
endif
ifneq ($(dss_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(dss_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_CAL_COMP_PATH)/cal_component.mk
ifneq ($(cal_LIB_LIST),)
  pdk_LIB_LIST += $(cal_LIB_LIST)
endif
ifneq ($(cal_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(cal_APP_LIB_LIST)
endif
ifneq ($(cal_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(cal_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_SCICLIENT_COMP_PATH)/sciclient_component.mk
ifneq ($(sciclient_LIB_LIST),)
  pdk_LIB_LIST += $(sciclient_LIB_LIST)
endif
ifneq ($(sciclient_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(sciclient_APP_LIB_LIST)
endif
ifneq ($(sciclient_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(sciclient_EXAMPLE_LIST)
endif
ifneq ($(sciclient_UTILS_LIST),)
  pdk_UTILS_LIST += $(sciclient_UTILS_LIST)
endif

-include $(PDK_VHWA_COMP_PATH)/vhwa_component.mk
ifneq ($(vhwa_LIB_LIST),)
  pdk_LIB_LIST += $(vhwa_LIB_LIST)
endif
ifneq ($(vhwa_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(vhwa_APP_LIB_LIST)
endif
ifneq ($(vhwa_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(vhwa_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_IOLINK_COMP_PATH)/iolink_component.mk
ifneq ($(iolink_LIB_LIST),)
  pdk_LIB_LIST += $(iolink_LIB_LIST)
endif
ifneq ($(iolink_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(iolink_EXAMPLE_LIST)
endif

#Below applicable only for TDA devices

# - used to ignore include if component not present
-include $(PDK_DIAG_COMP_PATH)/diag_component.mk
ifneq ($(diag_LIB_LIST),)
  pdk_LIB_LIST += $(diag_LIB_LIST)
endif
ifneq ($(diag_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(diag_APP_LIB_LIST)
endif
ifneq ($(diag_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(diag_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_FIREWALL_L3L4_COMP_PATH)/firewall_l3l4_component.mk
ifneq ($(firewall_l3l4_LIB_LIST),)
  pdk_LIB_LIST += $(firewall_l3l4_LIB_LIST)
endif
ifneq ($(firewall_l3l4_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(firewall_l3l4_APP_LIB_LIST)
endif
ifneq ($(firewall_l3l4_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(firewall_l3l4_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_SDR_COMP_PATH)/sdr_component.mk
ifneq ($(sdr_LIB_LIST),)
  pdk_LIB_LIST += $(sdr_LIB_LIST)
endif
ifneq ($(sdr_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(sdr_APP_LIB_LIST)
endif
ifneq ($(sdr_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(sdr_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_DIAG_EX_COMP_PATH)/diag_ex_component.mk
ifneq ($(diag_ex_LIB_LIST),)
  pdk_LIB_LIST += $(diag_ex_LIB_LIST)
endif
ifneq ($(diag_ex_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(diag_ex_APP_LIB_LIST)
endif
ifneq ($(diag_ex_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(diag_ex_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_IPC_COMP_PATH)/ipc_component.mk
ifneq ($(ipc_LIB_LIST),)
  pdk_LIB_LIST += $(ipc_LIB_LIST)
endif
ifneq ($(ipc_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(ipc_APP_LIB_LIST)
endif
ifneq ($(ipc_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(ipc_EXAMPLE_LIST)
endif
ifneq ($(ipc_DUP_EXAMPLE_LIST),)
  pdk_DUP_EXAMPLE_LIST += $(ipc_DUP_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_IPCLITE_COMP_PATH)/ipclite_component.mk
ifneq ($(ipclite_LIB_LIST),)
  pdk_LIB_LIST += $(ipclite_LIB_LIST)
endif
ifneq ($(ipclite_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(ipclite_APP_LIB_LIST)
endif
ifneq ($(ipclite_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(ipclite_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_TDA3X_SECURITY_COMP_PATH)/security_component.mk
ifneq ($(security_LIB_LIST),)
  pdk_LIB_LIST += $(security_LIB_LIST)
endif
ifneq ($(security_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(security_APP_LIB_LIST)
endif
ifneq ($(security_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(security_EXAMPLE_LIST)
endif

#Below applicable only for TDA devices for backward compatibility;not applicable for PRSDK

# - used to ignore include if component not present
-include $(PDK_NORFLASH_COMP_PATH)/norflash_component.mk
ifneq ($(norflash_LIB_LIST),)
  pdk_LIB_LIST += $(norflash_LIB_LIST)
endif
ifneq ($(norflash_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(norflash_APP_LIB_LIST)
endif
ifneq ($(norflash_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(norflash_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_QSPIFLASH_COMP_PATH)/qspiflash_component.mk
ifneq ($(qspiflash_LIB_LIST),)
  pdk_LIB_LIST += $(qspiflash_LIB_LIST)
endif
ifneq ($(qspiflash_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(qspiflash_APP_LIB_LIST)
endif
ifneq ($(qspiflash_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(qspiflash_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_STW_LLD_COMP_PATH)/stw_lld_component.mk
ifneq ($(stw_lld_LIB_LIST),)
  pdk_LIB_LIST += $(stw_lld_LIB_LIST)
endif
ifneq ($(stw_lld_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(stw_lld_APP_LIB_LIST)
endif
ifneq ($(stw_lld_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(stw_lld_EXAMPLE_LIST)
endif
ifneq ($(stw_lld_DUP_EXAMPLE_LIST),)
  pdk_DUP_EXAMPLE_LIST += $(stw_lld_DUP_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_VPS_COMP_PATH)/vps_component.mk
ifneq ($(vps_LIB_LIST),)
  pdk_LIB_LIST += $(vps_LIB_LIST)
endif
ifneq ($(vps_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(vps_APP_LIB_LIST)
endif
ifneq ($(vps_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(vps_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_OSAL_COMP_PATH)/osal_component.mk
ifneq ($(osal_LIB_LIST),)
  pdk_LIB_LIST += $(osal_LIB_LIST)
endif
ifneq ($(osal_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(osal_APP_LIB_LIST)
endif
ifneq ($(osal_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(osal_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_I2C_COMP_PATH)/i2c_component.mk
ifneq ($(i2c_LIB_LIST),)
  pdk_LIB_LIST += $(i2c_LIB_LIST)
endif
ifneq ($(i2c_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(i2c_APP_LIB_LIST)
endif
ifneq ($(i2c_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(i2c_EXAMPLE_LIST)
endif

ifneq ($(i2c_FIRM_LIST),)
  pdk_FIRM_LIST += $(i2c_FIRM_LIST)
endif
# - used to ignore include if component not present
-include $(PDK_UART_COMP_PATH)/uart_component.mk
ifneq ($(uart_LIB_LIST),)
  pdk_LIB_LIST += $(uart_LIB_LIST)
endif
ifneq ($(uart_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(uart_APP_LIB_LIST)
endif
ifneq ($(uart_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(uart_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_USB_COMP_PATH)/usb_component.mk
ifneq ($(usb_LIB_LIST),)
  pdk_LIB_LIST += $(usb_LIB_LIST)
endif
ifneq ($(usb_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(usb_APP_LIB_LIST)
endif
ifneq ($(usb_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(usb_EXAMPLE_LIST)
endif

  # - used to ignore include if component not present
  -include $(PDK_SERDES_DIAG_COMP_PATH)/serdes_diag_component.mk
  ifneq ($(serdes_diag_LIB_LIST),)
    pdk_LIB_LIST += $(serdes_diag_LIB_LIST)
  endif
  ifneq ($(serdes_diag_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(serdes_diag_APP_LIB_LIST)
endif
  ifneq ($(serdes_diag_EXAMPLE_LIST),)
    pdk_EXAMPLE_LIST += $(serdes_diag_EXAMPLE_LIST)
  endif

  # - used to ignore include if component not present
-include $(PDK_GPIO_COMP_PATH)/gpio_component.mk
ifneq ($(gpio_LIB_LIST),)
  pdk_LIB_LIST += $(gpio_LIB_LIST)
endif
ifneq ($(gpio_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(gpio_APP_LIB_LIST)
endif
ifneq ($(gpio_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(gpio_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_PROFILING_COMP_PATH)/profiling_component.mk
ifneq ($(profiling_LIB_LIST),)
  pdk_LIB_LIST += $(profiling_LIB_LIST)
endif
ifneq ($(profiling_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(profiling_APP_LIB_LIST)
endif
ifneq ($(profiling_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(profiling_EXAMPLE_LIST)
endif
# - used to ignore include if component not present
-include $(PDK_COPYVECS_COMP_PATH)/copyvecs_component.mk
ifneq ($(copyvecs_LIB_LIST),)
  pdk_LIB_LIST += $(copyvecs_LIB_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_TRACE_COMP_PATH)/trace_component.mk
ifneq ($(trace_LIB_LIST),)
  pdk_LIB_LIST += $(trace_LIB_LIST)
endif
ifneq ($(trace_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(trace_APP_LIB_LIST)
endif
ifneq ($(trace_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(trace_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_NIMU_ICSS_COMP_PATH)/nimu_icss_component.mk
ifneq ($(nimu_icss_LIB_LIST),)
  pdk_LIB_LIST += $(nimu_icss_LIB_LIST)
endif
ifneq ($(nimu_icss_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(nimu_icss_APP_LIB_LIST)
endif
ifneq ($(nimu_icss_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(nimu_icss_EXAMPLE_LIST)
endif

 # - used to ignore include if component not present
-include $(PDK_TIMESYNC_COMP_PATH)/timeSync_component.mk
ifneq ($(timeSync_LIB_LIST),)
  pdk_LIB_LIST += $(timeSync_LIB_LIST)
endif
ifneq ($(timeSync_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(timeSync_APP_LIB_LIST)
endif
ifneq ($(timeSync_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(timeSync_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_NIMU_COMP_PATH)/nimu_component.mk
ifneq ($(nimu_LIB_LIST),)
  pdk_LIB_LIST += $(nimu_LIB_LIST)
endif
ifneq ($(nimu_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(nimu_APP_LIB_LIST)
endif
ifneq ($(nimu_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(nimu_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_FATFS_COMP_PATH)/fatfs_component.mk
ifneq ($(fatfs_LIB_LIST),)
  pdk_LIB_LIST += $(fatfs_LIB_LIST)
endif
ifneq ($(fatfs_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(fatfs_APP_LIB_LIST)
endif
ifneq ($(fatfs_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(fatfs_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_MMCSD_COMP_PATH)/mmcsd_component.mk
ifneq ($(mmcsd_LIB_LIST),)
  pdk_LIB_LIST += $(mmcsd_LIB_LIST)
endif
ifneq ($(mmcsd_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(mmcsd_APP_LIB_LIST)
endif
ifneq ($(mmcsd_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(mmcsd_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_PCIE_COMP_PATH)/pcie_component.mk
ifneq ($(pcie_LIB_LIST),)
  pdk_LIB_LIST += $(pcie_LIB_LIST)
endif
ifneq ($(pcie_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(pcie_APP_LIB_LIST)
endif
ifneq ($(pcie_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(pcie_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_MCASP_COMP_PATH)/mcasp_component.mk
ifneq ($(mcasp_LIB_LIST),)
  pdk_LIB_LIST += $(mcasp_LIB_LIST)
endif
ifneq ($(mcasp_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(mcasp_APP_LIB_LIST)
endif
ifneq ($(mcasp_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(mcasp_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_MCBSP_COMP_PATH)/mcbsp_component.mk
ifneq ($(mcbsp_LIB_LIST),)
  pdk_LIB_LIST += $(mcbsp_LIB_LIST)
endif
ifneq ($(mcbsp_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(mcbsp_APP_LIB_LIST)
endif
ifneq ($(mcbsp_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(mcbsp_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_ICSS_EMAC_COMP_PATH)/icss_emac_component.mk
ifneq ($(icss_emac_LIB_LIST),)
  pdk_LIB_LIST += $(icss_emac_LIB_LIST)
endif
ifneq ($(icss_emac_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(icss_emac_APP_LIB_LIST)
endif
ifneq ($(icss_emac_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(icss_emac_EXAMPLE_LIST)
endif
ifneq ($(icss_emac_FIRM_LIST),)
  pdk_FIRM_LIST += $(icss_emac_FIRM_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_EMAC_COMP_PATH)/emac_component.mk
ifneq ($(emac_LIB_LIST),)
  pdk_LIB_LIST += $(emac_LIB_LIST)
endif
ifneq ($(emac_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(emac_APP_LIB_LIST)
endif
ifneq ($(emac_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(emac_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_PRUSS_COMP_PATH)/pruss_component.mk
ifneq ($(pruss_LIB_LIST),)
  pdk_LIB_LIST += $(pruss_LIB_LIST)
endif
ifneq ($(pruss_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(pruss_APP_LIB_LIST)
endif
ifneq ($(pruss_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(pruss_EXAMPLE_LIST)
endif
ifneq ($(pruss_FIRM_LIST),)
  pdk_FIRM_LIST += $(pruss_FIRM_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_SPI_COMP_PATH)/spi_component.mk
ifneq ($(spi_LIB_LIST),)
  pdk_LIB_LIST += $(spi_LIB_LIST)
endif
ifneq ($(spi_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(spi_APP_LIB_LIST)
endif
ifneq ($(spi_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(spi_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_GPMC_COMP_PATH)/gpmc_component.mk
ifneq ($(gpmc_LIB_LIST),)
  pdk_LIB_LIST += $(gpmc_LIB_LIST)
endif
ifneq ($(gpmc_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(gpmc_APP_LIB_LIST)
endif
ifneq ($(gpmc_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(gpmc_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_SA_COMP_PATH)/sa_component.mk
ifneq ($(sa_LIB_LIST),)
  pdk_LIB_LIST += $(sa_LIB_LIST)
endif
ifneq ($(sa_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(sa_APP_LIB_LIST)
endif
ifneq ($(sa_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(sa_EXAMPLE_LIST)
endif

#Below applicable only for TPR12 devices
# - used to ignore include if component not present
-include $(PDK_CRC_COMP_PATH)/crc_component.mk
ifneq ($(crc_LIB_LIST),)
  pdk_LIB_LIST += $(crc_LIB_LIST)
endif
ifneq ($(crc_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(crc_APP_LIB_LIST)
endif
ifneq ($(crc_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(crc_EXAMPLE_LIST)
endif

#Below applicable only for TPR12 devices
# - used to ignore include if component not present
-include $(PDK_GPADC_COMP_PATH)/gpadc_component.mk
ifneq ($(gpadc_LIB_LIST),)
  pdk_LIB_LIST += $(gpadc_LIB_LIST)
endif
ifneq ($(gpadc_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(gpadc_APP_LIB_LIST)
endif
ifneq ($(gpadc_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(gpadc_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_HWA_COMP_PATH)/hwa_component.mk
ifneq ($(hwa_LIB_LIST),)
  pdk_LIB_LIST += $(hwa_LIB_LIST)
endif
ifneq ($(hwa_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(hwa_APP_LIB_LIST)
endif
ifneq ($(hwa_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(hwa_EXAMPLE_LIST)
endif
# - used to ignore include if component not present
-include $(PDK_EDMA_COMP_PATH)/edma_component.mk
ifneq ($(edma_LIB_LIST),)
  pdk_LIB_LIST += $(edma_LIB_LIST)
endif
ifneq ($(edma_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(edma_APP_LIB_LIST)
endif
ifneq ($(edma_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(edma_EXAMPLE_LIST)
endif
# - used to ignore include if component not present
-include $(PDK_MAILBOX_COMP_PATH)/mailbox_component.mk
ifneq ($(mailbox_LIB_LIST),)
  pdk_LIB_LIST += $(mailbox_LIB_LIST)
endif
ifneq ($(mailbox_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(mailbox_APP_LIB_LIST)
endif
ifneq ($(mailbox_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(mailbox_EXAMPLE_LIST)
endif
ifneq ($(mailbox_DUP_EXAMPLE_LIST),)
  pdk_DUP_EXAMPLE_LIST += $(mailbox_DUP_EXAMPLE_LIST)
endif

-include $(PDK_MIBSPI_COMP_PATH)/mibspi_component.mk
ifneq ($(mibspi_LIB_LIST),)
  pdk_LIB_LIST += $(mibspi_LIB_LIST)
endif
ifneq ($(mibspi_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(mibspi_APP_LIB_LIST)
endif
ifneq ($(mibspi_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(mibspi_EXAMPLE_LIST)
endif

-include $(PDK_CANFD_COMP_PATH)/canfd_component.mk
ifneq ($(canfd_LIB_LIST),)
  pdk_LIB_LIST += $(canfd_LIB_LIST)
endif
ifneq ($(canfd_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(canfd_APP_LIB_LIST)
endif
ifneq ($(canfd_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(canfd_EXAMPLE_LIST)
endif

-include $(PDK_FREERTOS_COMP_PATH)/freertos_component.mk
ifneq ($(freertos_LIB_LIST),)
  pdk_LIB_LIST += $(freertos_LIB_LIST)
endif
ifneq ($(freertos_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(freertos_APP_LIB_LIST)
endif
ifneq ($(freertos_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(freertos_EXAMPLE_LIST)
endif

-include $(PDK_SAFERTOS_COMP_PATH)/safertos_component.mk
ifneq ($(safertos_LIB_LIST),)
  pdk_LIB_LIST += $(safertos_LIB_LIST)
endif
ifneq ($(safertos_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(safertos_APP_LIB_LIST)
endif
ifneq ($(safertos_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(safertos_EXAMPLE_LIST)
endif

-include $(PDK_ESM_COMP_PATH)/esm_component.mk
ifneq ($(esm_LIB_LIST),)
  pdk_LIB_LIST += $(esm_LIB_LIST)
endif
ifneq ($(esm_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(esm_APP_LIB_LIST)
endif
ifneq ($(esm_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(esm_EXAMPLE_LIST)
endif
# - used to ignore include if component not present
-include $(PDK_CAN_COMP_PATH)/can_component.mk
ifneq ($(can_LIB_LIST),)
  pdk_LIB_LIST += $(can_LIB_LIST)
endif
ifneq ($(can_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(can_APP_LIB_LIST)
endif
ifneq ($(can_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(can_EXAMPLE_LIST)
endif

-include $(PDK_WATCHDOG_COMP_PATH)/watchdog_component.mk
ifneq ($(watchdog_LIB_LIST),)
  pdk_LIB_LIST += $(watchdog_LIB_LIST)
endif
ifneq ($(watchdog_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(watchdog_APP_LIB_LIST)
endif
ifneq ($(watchdog_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(watchdog_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_ADCBUF_COMP_PATH)/adcbuf_component.mk
ifneq ($(adcbuf_LIB_LIST),)
  pdk_LIB_LIST += $(adcbuf_LIB_LIST)
endif
ifneq ($(adcbuf_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(adcbuf_APP_LIB_LIST)
endif
ifneq ($(adcbuf_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(adcbuf_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_CBUFF_COMP_PATH)/cbuff_component.mk
ifneq ($(cbuff_LIB_LIST),)
  pdk_LIB_LIST += $(cbuff_LIB_LIST)
endif
ifneq ($(cbuff_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $(cbuff_APP_LIB_LIST)
endif
ifneq ($(cbuff_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(cbuff_EXAMPLE_LIST)
endif

# Components included for non-baremetal OS
ifneq ($(BUILD_OS_TYPE), baremetal)
  #Below applicable only for TDA devices for backward compatibility;not applicable for PRSDK
  # - used to ignore include if component not present
  -include $(PDK_BSP_LLD_COMP_PATH)/bsp_lld_component.mk
  ifneq ($(bsp_lld_LIB_LIST),)
    pdk_LIB_LIST += $(bsp_lld_LIB_LIST)
  endif
  ifneq ($(bsp_lld_EXAMPLE_LIST),)
    pdk_EXAMPLE_LIST += $(bsp_lld_EXAMPLE_LIST)
  endif
endif

  #Below applicable only for K3 DMSC; not applicable for PRSDK
  # - used to ignore include if component not present
  -include $(DMSC_COMP_PATH)/dmsc_component.mk
  ifneq ($(dmsc_LIB_LIST),)
    pdk_LIB_LIST += $(dmsc_LIB_LIST)
  endif
  ifneq ($(dmsc_EXAMPLE_LIST),)
    pdk_EXAMPLE_LIST += $(dmsc_EXAMPLE_LIST)
  endif

# - used to ignore include if component not present
-include $(PDK_BOARD_UTILS_COMP_PATH)/board_utils_component.mk
ifneq ($(board_utils_LIB_LIST),)
  pdk_LIB_LIST += $(board_utils_LIB_LIST)
endif
ifneq ($(board_utils_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(board_utils_EXAMPLE_LIST)
endif

# - used to ignore include if component not present
-include $(PDK_BOARD_DIAG_COMP_PATH)/board_diag_component.mk
ifneq ($(board_diag_LIB_LIST),)
  pdk_LIB_LIST += $(board_diag_LIB_LIST)
endif
ifneq ($(board_diag_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $(board_diag_EXAMPLE_LIST)
endif

# Adding new component to the packages/ti/build which is not part of the PDK.
# An example would be some demo which is not built from packages/ti/build but
# would still like to use the ti/build/ infrastructure

ifeq ($($(COMP)_USE_PDK_BUILD),yes)
include $($(COMP)_COMPONENT_MK_PATH)/$(COMP)_component.mk

ifneq ($($(COMP)_LIB_LIST),)
  pdk_LIB_LIST += $($(COMP)_LIB_LIST)
endif

ifneq ($($(COMP)_APP_LIB_LIST),)
  pdk_APP_LIB_LIST += $($(COMP)_APP_LIB_LIST)
endif

ifneq ($($(COMP)_FIRM_LIST),)
  pdk_FIRM_LIST += $($(COMP)_FIRM_LIST)
endif

ifneq ( $($(COMP)_EXAMPLE_LIST),)
  pdk_EXAMPLE_LIST += $($(COMP)_EXAMPLE_LIST)
endif

endif

pdk_PKG_LIST_ALL = $(pdk_EXAMPLE_LIST) $(pdk_LIB_LIST) $(pdk_APP_LIB_LIST) $(pdk_FIRM_LIST)

ifneq ($(CORE),$(filter $(CORE), pru_0 pru_1))
# By default it is little endian for non pru cores
  LIB_ENDIAN_LIST = little
endif
#
# PDK Modules
#

#
# PDK Examples
#

#Default to all if not set from rules.make or command line
ifeq ($(PACKAGE_SELECT), )
  PACKAGE_SELECT := all
endif

ifeq ($(CORE),mcu1_0)
  PDK_CFLAGS += -DBUILD_MCU1_0 -DBUILD_MCU
  PDK_LNKFLAGS += --define=BUILD_MCU1_0 --define=BUILD_MCU
endif

ifeq ($(CORE),mcu1_1)
  PDK_CFLAGS += -DBUILD_MCU1_1 -DBUILD_MCU
  PDK_LNKFLAGS += --define=BUILD_MCU1_1 --define=BUILD_MCU
endif

ifeq ($(CORE),mcu2_0)
  PDK_CFLAGS += -DBUILD_MCU2_0 -DBUILD_MCU
  PDK_LNKFLAGS += --define=BUILD_MCU2_0 --define=BUILD_MCU
endif

ifeq ($(CORE),mcu2_1)
  PDK_CFLAGS += -DBUILD_MCU2_1 -DBUILD_MCU
  PDK_LNKFLAGS += --define=BUILD_MCU2_1 --define=BUILD_MCU
endif

ifeq ($(CORE),mcu3_0)
  PDK_CFLAGS += -DBUILD_MCU3_0 -DBUILD_MCU
  PDK_LNKFLAGS += --define=BUILD_MCU3_0 --define=BUILD_MCU
endif

ifeq ($(CORE),mcu3_1)
  PDK_CFLAGS += -DBUILD_MCU3_1 -DBUILD_MCU
  PDK_LNKFLAGS += --define=BUILD_MCU3_1 --define=BUILD_MCU
endif

ifeq ($(CORE),mpu1_0)
  PDK_CFLAGS += -DBUILD_MPU1_0 -DBUILD_MPU
  PDK_LNKFLAGS += --define=BUILD_MPU1_0 --define=BUILD_MPU
endif

ifeq ($(CORE),mpu1_1)
  PDK_CFLAGS += -DBUILD_MPU1_1 -DBUILD_MPU
  PDK_LNKFLAGS += --define=BUILD_MPU1_1 --define=BUILD_MPU
endif

ifeq ($(CORE),mpu2_0)
  PDK_CFLAGS += -DBUILD_MPU2_0 -DBUILD_MPU
  PDK_LNKFLAGS += --define=BUILD_MPU2_0 --define=BUILD_MPU
endif

ifeq ($(CORE),mpu2_1)
  PDK_CFLAGS += -DBUILD_MPU2_1 -DBUILD_MPU
  PDK_LNKFLAGS += --define=BUILD_MPU2_1 --define=BUILD_MPU
endif

ifeq ($(CORE),ipu1_0)
  PDK_CFLAGS += -DBUILD_IPU1_0 -DBUILD_IPU1
  PDK_LNKFLAGS += --define=BUILD_IPU1_0 --define=BUILD_IPU1
endif

ifeq ($(CORE),ipu1_1)
  PDK_CFLAGS += -DBUILD_IPU1_1 -DBUILD_IPU1
  PDK_LNKFLAGS += --define=BUILD_IPU1_1 --define=BUILD_IPU1
endif

ifeq ($(CORE),ipu2_0)
  PDK_CFLAGS += -DBUILD_IPU2_0 -DBUILD_IPU2
  PDK_LNKFLAGS += --define=BUILD_IPU2_0 --define=BUILD_IPU2
endif

ifeq ($(CORE),ipu2_1)
  PDK_CFLAGS += -DBUILD_IPU2_1 -DBUILD_IPU2
  PDK_LNKFLAGS += --define=BUILD_IPU2_1 --define=BUILD_IPU2
endif

ifeq ($(CORE),m3)
  PDK_CFLAGS += -DBUILD_M3
  PDK_LNKFLAGS += --define=BUILD_M3
endif

ifeq ($(CORE),arp32_1)
  PDK_CFLAGS += -DBUILD_ARP32 -DBUILD_ARP32_1
  PDK_LNKFLAGS += --define=BUILD_ARP32 --define=BUILD_ARP32_1
endif

ifeq ($(CORE),arp32_2)
  PDK_CFLAGS += -DBUILD_ARP32 -DBUILD_ARP32_2
  PDK_LNKFLAGS += --define=BUILD_ARP32 --define=BUILD_ARP32_2
endif

ifeq ($(CORE),arp32_3)
  PDK_CFLAGS += -DBUILD_ARP32 -DBUILD_ARP32_3
  PDK_LNKFLAGS += --define=BUILD_ARP32 --define=BUILD_ARP32_3
endif

ifeq ($(CORE),arp32_4)
  PDK_CFLAGS += -DBUILD_ARP32 -DBUILD_ARP32_4
  PDK_LNKFLAGS += --define=BUILD_ARP32 --define=BUILD_ARP32_4
endif

ifeq ($(CORE),a8host)
  PDK_CFLAGS += -DBUILD_A8
  PDK_LNKFLAGS += --define=BUILD_A8
endif

ifeq ($(CORE),pru_0)
  PDK_CFLAGS += -DPRU0 -Dpru0
  PDK_LNKFLAGS += --define=PRU0 --define=pru0
  # List all the libraries to be built
  ifeq ($(SOC),$(filter $(SOC), am335x am437x))
    PRU_VERSION_LIST = REV1
  endif
  ifeq ($(SOC),$(filter $(SOC), am571x k2g am574x))
    PRU_VERSION_LIST = REV2
  endif
  ifeq ($(SOC),$(filter $(SOC), am572x))
    PRU_VERSION_LIST = REV1 REV2
  endif
endif

ifeq ($(CORE),pru_1)
  PDK_CFLAGS += -DPRU1 -Dpru1
  PDK_LNKFLAGS += --define=PRU1 --define=pru1
  # List all the libraries to be built
  ifeq ($(SOC),$(filter $(SOC), am335x am437x))
    PRU_VERSION_LIST = REV1
  endif
  ifeq ($(SOC),$(filter $(SOC), am571x k2g am574x))
    PRU_VERSION_LIST = REV2
  endif
  ifeq ($(SOC),$(filter $(SOC), am572x))
    PRU_VERSION_LIST = REV1 REV2
  endif
endif

ifeq ($(CORE),a15_0)
  PDK_CFLAGS += -DBUILD_A15_0
  PDK_LNKFLAGS += --define=BUILD_A15_0
endif

ifeq ($(CORE),$(filter $(CORE), c66x c66xdsp_1 c66xdsp_2))
  ifeq ($(CORE),$(filter $(CORE), c66xdsp_1))
    PDK_CFLAGS += -DBUILD_DSP_1 -DBUILD_C66X_1
    PDK_LNKFLAGS += --define=BUILD_DSP_1 --define=BUILD_C66X_1
  endif
  ifeq ($(CORE),$(filter $(CORE), c66xdsp_2))
    PDK_CFLAGS += -DBUILD_DSP_2 -DBUILD_C66X_2
    PDK_LNKFLAGS += --define=BUILD_DSP_2 --define=BUILD_C66X_2
  endif
endif

ifeq ($(CORE),$(filter $(CORE), c7x_1 c7x-hostemu))
  PDK_CFLAGS += -DBUILD_C7X_1
  PDK_LNKFLAGS += --define=BUILD_C7X_1
endif

ifeq ($(CORE),$(filter $(CORE), c7x_2))
  PDK_CFLAGS += -DBUILD_C7X_2
  PDK_LNKFLAGS += --define=BUILD_C7X_2
endif

ifeq ($(CORE),$(filter $(CORE), qnx_mpu1_0))
  PDK_CFLAGS += -DQNX_OS -DBUILD_MPU1_0
  PDK_LNKFLAGS += --define=QNX_OS --define=BUILD_MPU1_0
endif

ifeq ($(CORE),$(filter $(CORE), m4f_0))
  PDK_CFLAGS += -DBUILD_M4F_0 -DBUILD_M4F
  PDK_LNKFLAGS += --define=BUILD_M4F_0 --define=BUILD_M4F
endif

export PDK_CFLAGS
export PDK_LNKFLAGS

ifeq ($(SOC),$(filter $(SOC), j721e))
  PDK_COMMON_COMP = csl uart i2c udma gpio pmic pm_lib
  ifneq ($(CORE),$(filter $(CORE), c7x-hostemu))
    PDK_COMMON_COMP += board
  endif
  ifeq ($(CORE),mcu1_0)
    PDK_COMMON_COMP += sciclient_direct rm_pm_hal
  else
    PDK_COMMON_COMP += sciclient
  endif
endif

ifeq ($(SOC),$(filter $(SOC), j7200))
  PDK_COMMON_COMP = csl uart i2c board udma gpio pmic pm_lib
  ifeq ($(CORE),mcu1_0)
    PDK_COMMON_COMP += sciclient_direct rm_pm_hal
  else
    PDK_COMMON_COMP += sciclient
  endif
endif

ifeq ($(SOC),$(filter $(SOC), j721s2))
  PDK_COMMON_COMP = csl
  ifeq ($(CORE),mcu1_0)
    PDK_COMMON_COMP += sciclient_direct rm_pm_hal
  else
    PDK_COMMON_COMP += sciclient
  endif
endif

ifeq ($(SOC),$(filter $(SOC), am65xx))
  PDK_COMMON_COMP = csl uart i2c board gpio pm_lib
  ifeq ($(CORE),$(filter $(CORE), mpu1_0 mcu1_0 mcu1_1))
    PDK_COMMON_COMP += sciclient udma
  endif
endif

ifeq ($(SOC),$(filter $(SOC), am64x))
  PDK_COMMON_COMP = csl uart i2c board gpio sciclient udma
endif

ifeq ($(SOC),$(filter $(SOC), tpr12))
  PDK_COMMON_COMP = csl uart i2c board gpio edma
endif

ifeq ($(SOC),$(filter $(SOC), awr294x))
  PDK_COMMON_COMP = csl uart  board  edma
  ifeq ($(CORE),$(filter $(CORE), mcu1_0 mcu1_1))
    PDK_COMMON_COMP += i2c gpio
  endif
endif

ifeq ($(SOC),$(filter $(SOC), am571x am572x am574x dra72x dra75x dra78x))
  PDK_COMMON_COMP = csl uart i2c board gpio pm_lib pm_hal
endif

ifeq ($(SOC),$(filter $(SOC), tda2xx tda2px tda2ex tda3xx))
  PDK_COMMON_COMP = csl uart i2c board pm_lib pm_hal
endif

ifeq ($(SOC),$(filter $(SOC), am335x am437x c6657 c6678 k2e k2g k2h k2k k2l omapl137 omapl138))
  PDK_COMMON_COMP = csl uart i2c board gpio
endif

ifeq ($(SOC),$(filter $(SOC), c6747))
  PDK_COMMON_COMP = csl
endif

PDK_COMMON_TIRTOS_COMP = $(PDK_COMMON_COMP) osal_tirtos
ifeq ($(SOC),$(filter $(SOC), tpr12 awr294x))
# Add copyvecs for socs other than tpr12 and awr294x
else
# Enable copy of vectors
  ifeq ($(ISA),$(filter $(ISA), r5f))
    PDK_COMMON_TIRTOS_COMP += copyvecs
  endif
endif

PDK_COMMON_BAREMETAL_COMP = $(PDK_COMMON_COMP) osal_nonos
ifeq ($(ARCH),c66x)
  PDK_COMMON_BAREMETAL_COMP += csl_intc
else
  ifneq ($(ARCH),c71)
    PDK_COMMON_BAREMETAL_COMP += csl_init
  endif
endif

ifeq ($(SOC),$(filter $(SOC), am65xx j721e j7200 j721s2 tpr12 awr294x))
  PDK_COMMON_FREERTOS_COMP = $(PDK_COMMON_COMP) osal_freertos
  PDK_COMMON_FREERTOS_COMP += freertos
  PDK_COMMON_SAFERTOS_COMP = $(PDK_COMMON_COMP) osal_safertos
  PDK_COMMON_SAFERTOS_COMP += safertos
ifeq ($(ARCH),c66x)
  PDK_COMMON_FREERTOS_COMP += csl_intc
  PDK_COMMON_SAFERTOS_COMP += csl_intc
else
  ifneq ($(ARCH),c71)
    PDK_COMMON_FREERTOS_COMP += csl_init
    PDK_COMMON_SAFERTOS_COMP += csl_init
  endif
endif
endif

export PDK_COMMON_TIRTOS_COMP
export PDK_COMMON_BAREMETAL_COMP
export PDK_COMMON_FREERTOS_COMP
export PDK_COMMON_SAFERTOS_COMP

pdk_component_make_include := 1
endif
