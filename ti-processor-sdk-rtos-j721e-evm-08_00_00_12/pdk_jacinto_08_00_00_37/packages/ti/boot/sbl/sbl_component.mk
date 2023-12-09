#
# Copyright (c) 2018, Texas Instruments Incorporated
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

# File: sbl_component.mk
#       This file is component include make file of SBL.
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
#                             platform and the compiled obj/lib has to be kept
#                             under <platform> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no platform dependent code and hence
#                             the obj/libs are not kept under <platform> dir.
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
ifeq ($(sbl_component_make_include), )

sbl_BOARDLIST = am65xx_evm am65xx_idk j721e_evm j7200_evm am64x_evm tpr12_evm tpr12_qt awr294x_evm

sbl_SOCLIST = am65xx j721e j7200 am64x tpr12 awr294x

am65xx_smp_CORELIST := mcu1_0 mpu1_0 mpu2_0
sbl_am65xx_CORELIST := mcu1_0 mcu1_1 mpu1_0 mpu1_1 mpu2_0 mpu2_1
am65xx_LASTCORE := $(word $(words $(sbl_am65xx_CORELIST)), $(sbl_am65xx_CORELIST))

j721e_smp_CORELIST := mcu1_0 mcu2_0 mcu3_0 mpu1_0
sbl_j721e_CORELIST := mcu1_0 mcu1_1 mcu2_0 mcu2_1 mcu3_0 mcu3_1 mpu1_0 mpu1_1
j721e_LASTCORE := $(word $(words $(sbl_j721e_CORELIST)), $(sbl_j721e_CORELIST))

j7200_smp_CORELIST := mcu1_0 mcu2_0 mpu1_0
sbl_j7200_CORELIST := mcu1_0 mcu1_1 mcu2_0 mcu2_1 mpu1_0 mpu1_1
j7200_LASTCORE := $(word $(words $(sbl_j7200_CORELIST)), $(sbl_j7200_CORELIST))

am64x_smp_CORELIST := mcu1_0 mcu2_0 mpu1_0
sbl_am64x_CORELIST := mcu1_0 mcu1_1 mcu2_0 mcu2_1 mpu1_0 mpu1_1
am64x_LASTCORE := $(word $(words $(sbl_am64x_CORELIST)), $(sbl_am64x_CORELIST))

sbl_tpr12_CORELIST := mcu1_0 mcu1_1 c66xdsp_1
tpr12_LASTCORE := $(word $(words $(sbl_tpr12_CORELIST)), $(sbl_tpr12_CORELIST))

sbl_awr294x_CORELIST := mcu1_0 c66xdsp_1
awr294x_LASTCORE := $(word $(words $(sbl_awr294x_CORELIST)), $(sbl_awr294x_CORELIST))

sbl_DISABLE_PARALLEL_MAKE = yes
############################
# sbl package
# List of components included under sbl
# The components included here are built and will be part of sbl
############################

ifeq ($(SOC),$(filter $(SOC), am64x))
  sbl_LIB_LIST = sbl_lib_mmcsd sbl_lib_mmcsd_hlos sbl_lib_ospi sbl_lib_ospi_hlos sbl_lib_uart sbl_lib_cust
  sbl_LIB_LIST += sbl_lib_ospi_nondma_hlos
else
  ifeq ($(SOC), $(filter $(SOC), tpr12 awr294x))
    sbl_LIB_LIST = sbl_lib_uart
    sbl_LIB_LIST += sbl_lib_qspi sbl_lib_qspi_nondma
  else
    sbl_LIB_LIST = sbl_lib_mmcsd sbl_lib_ospi sbl_lib_uart sbl_lib_hyperflash sbl_lib_cust
    sbl_LIB_LIST += sbl_lib_mmcsd_hlos sbl_lib_ospi_hlos sbl_lib_hyperflash_hlos
    sbl_LIB_LIST += sbl_lib_ospi_nondma sbl_lib_ospi_nondma_hlos
    sbl_LIB_LIST += sbl_lib_mmcsd_hs sbl_lib_ospi_hs sbl_lib_uart_hs sbl_lib_hyperflash_hs sbl_lib_cust_hs
    sbl_LIB_LIST += sbl_lib_mmcsd_hlos_hs sbl_lib_ospi_hlos_hs sbl_lib_hyperflash_hlos_hs
    sbl_LIB_LIST += sbl_lib_ospi_nondma_hs sbl_lib_ospi_nondma_hlos_hs
  endif
endif

############################
# sbl example
# List of examples under sbl (+= is used at each example definition)
# All the tests mentioned in list are built when test target is called
# List below all examples for allowed values
############################
ifeq ($(SOC),$(filter $(SOC), am64x))
  sbl_EXAMPLE_LIST = sbl_mmcsd_img sbl_mmcsd_img_hlos sbl_ospi_img sbl_ospi_img_hlos sbl_uart_img
else
  ifeq ($(SOC), $(filter $(SOC), tpr12 awr294x))
    sbl_EXAMPLE_LIST = sbl_uart_img sbl_qspi_img
  else
    sbl_EXAMPLE_LIST = sbl_uart_img
    sbl_EXAMPLE_LIST += sbl_mmcsd_img sbl_mmcsd_img_hlos sbl_ospi_img sbl_ospi_img_hlos sbl_hyperflash_img sbl_hyperflash_img_hlos
    sbl_EXAMPLE_LIST += sbl_mmcsd_img_hs sbl_ospi_img_hs sbl_hyperflash_img_hs sbl_uart_img_hs
    sbl_EXAMPLE_LIST += sbl_mmcsd_img_hlos_hs sbl_ospi_img_hlos_hs sbl_hyperflash_img_hlos_hs
  endif
endif


#
# SBL Modules
#

# SBL MMCSD LIB
sbl_lib_mmcsd_COMP_LIST = sbl_lib_mmcsd
sbl_lib_mmcsd_RELPATH = ti/boot/sbl
export sbl_lib_mmcsd_OBJPATH = ti/boot/sbl/mmcsd
sbl_lib_mmcsd_LIBNAME = sbl_lib_mmcsd
sbl_lib_mmcsd_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_mmcsd_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/mmcsd
sbl_lib_mmcsd_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=mmcsd SBL_USE_DMA=yes
export sbl_lib_mmcsd_MAKEFILE
export sbl_lib_mmcsd_LIBNAME
export sbl_lib_mmcsd_LIBPATH
sbl_lib_mmcsd_BOARD_DEPENDENCY = yes
sbl_lib_mmcsd_SOC_DEPENDENCY = yes
sbl_lib_mmcsd_CORE_DEPENDENCY = no
export sbl_lib_mmcsd_COMP_LIST
export sbl_lib_mmcsd_BOARD_DEPENDENCY
export sbl_lib_mmcsd_CORE_DEPENDENCY
sbl_lib_mmcsd_PKG_LIST = sbl_lib_mmcsd
sbl_lib_mmcsd_INCLUDE = $(sbl_lib_mmcsd_PATH)
sbl_lib_mmcsd_SOCLIST = $(sbl_SOCLIST)
sbl_lib_mmcsd_BOARDLIST = $(sbl_BOARDLIST)
export sbl_lib_mmcsd_SOCLIST
export sbl_lib_mmcsd_BOARDLIST
sbl_lib_mmcsd_$(SOC)_CORELIST = mcu1_0
export sbl_lib_mmcsd_$(SOC)_CORELIST

# SBL MMCSD LIB - HS build variant
sbl_lib_mmcsd_hs_COMP_LIST = sbl_lib_mmcsd_hs
sbl_lib_mmcsd_hs_RELPATH = ti/boot/sbl
export sbl_lib_mmcsd_hs_OBJPATH = ti/boot/sbl/mmcsd_hs
sbl_lib_mmcsd_hs_LIBNAME = sbl_lib_mmcsd_hs
sbl_lib_mmcsd_hs_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_mmcsd_hs_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/mmcsd_hs
sbl_lib_mmcsd_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=mmcsd SBL_USE_DMA=yes BUILD_HS=yes
export sbl_lib_mmcsd_hs_MAKEFILE
export sbl_lib_mmcsd_hs_LIBNAME
export sbl_lib_mmcsd_hs_LIBPATH
sbl_lib_mmcsd_hs_BOARD_DEPENDENCY = yes
sbl_lib_mmcsd_hs_SOC_DEPENDENCY = yes
sbl_lib_mmcsd_hs_CORE_DEPENDENCY = no
export sbl_lib_mmcsd_hs_COMP_LIST
export sbl_lib_mmcsd_hs_BOARD_DEPENDENCY
export sbl_lib_mmcsd_hs_CORE_DEPENDENCY
sbl_lib_mmcsd_hs_PKG_LIST = sbl_lib_mmcsd_hs
sbl_lib_mmcsd_hs_INCLUDE = $(sbl_lib_mmcsd_hs_PATH)
sbl_lib_mmcsd_hs_SOCLIST = $(sbl_SOCLIST)
sbl_lib_mmcsd_hs_BOARDLIST = $(sbl_BOARDLIST)
export sbl_lib_mmcsd_hs_SOCLIST
export sbl_lib_mmcsd_hs_BOARDLIST
sbl_lib_mmcsd_hs_$(SOC)_CORELIST = mcu1_0
export sbl_lib_mmcsd_hs_$(SOC)_CORELIST

# SBL MMCSD HLOS LIB
sbl_lib_mmcsd_hlos_COMP_LIST = sbl_lib_mmcsd_hlos
sbl_lib_mmcsd_hlos_RELPATH = ti/boot/sbl
export sbl_lib_mmcsd_hlos_OBJPATH = ti/boot/sbl/mmcsd_hlos
sbl_lib_mmcsd_hlos_LIBNAME = sbl_lib_mmcsd_hlos
sbl_lib_mmcsd_hlos_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_mmcsd_hlos_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/mmcsd_hlos
sbl_lib_mmcsd_hlos_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=mmcsd HLOS_BOOT=yes SBL_USE_DMA=yes
export sbl_lib_mmcsd_hlos_MAKEFILE
export sbl_lib_mmcsd_hlos_LIBNAME
export sbl_lib_mmcsd_hlos_LIBPATH
sbl_lib_mmcsd_hlos_BOARD_DEPENDENCY = yes
sbl_lib_mmcsd_hlos_SOC_DEPENDENCY = yes
sbl_lib_mmcsd_hlos_CORE_DEPENDENCY = no
export sbl_lib_mmcsd_hlos_COMP_LIST
export sbl_lib_mmcsd_hlos_BOARD_DEPENDENCY
export sbl_lib_mmcsd_hlos_CORE_DEPENDENCY
sbl_lib_mmcsd_hlos_PKG_LIST = sbl_lib_mmcsd_hlos
sbl_lib_mmcsd_hlos_INCLUDE = $(sbl_lib_mmcsd_hlos_PATH)
sbl_lib_mmcsd_hlos_SOCLIST = $(sbl_SOCLIST)
sbl_lib_mmcsd_hlos_BOARDLIST = $(sbl_BOARDLIST)
export sbl_lib_mmcsd_hlos_SOCLIST
export sbl_lib_mmcsd_hlos_BOARDLIST
sbl_lib_mmcsd_hlos_$(SOC)_CORELIST = mcu1_0
export sbl_lib_mmcsd_hlos_$(SOC)_CORELIST

# SBL MMCSD HLOS LIB - HS build variant
sbl_lib_mmcsd_hlos_hs_COMP_LIST = sbl_lib_mmcsd_hlos_hs
sbl_lib_mmcsd_hlos_hs_RELPATH = ti/boot/sbl
export sbl_lib_mmcsd_hlos_hs_OBJPATH = ti/boot/sbl/mmcsd_hlos_hs
sbl_lib_mmcsd_hlos_hs_LIBNAME = sbl_lib_mmcsd_hlos_hs
sbl_lib_mmcsd_hlos_hs_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_mmcsd_hlos_hs_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/mmcsd_hlos_hs
sbl_lib_mmcsd_hlos_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=mmcsd HLOS_BOOT=yes SBL_USE_DMA=yes BUILD_HS=yes
export sbl_lib_mmcsd_hlos_hs_MAKEFILE
export sbl_lib_mmcsd_hlos_hs_LIBNAME
export sbl_lib_mmcsd_hlos_hs_LIBPATH
sbl_lib_mmcsd_hlos_hs_BOARD_DEPENDENCY = yes
sbl_lib_mmcsd_hlos_hs_SOC_DEPENDENCY = yes
sbl_lib_mmcsd_hlos_hs_CORE_DEPENDENCY = no
export sbl_lib_mmcsd_hlos_hs_COMP_LIST
export sbl_lib_mmcsd_hlos_hs_BOARD_DEPENDENCY
export sbl_lib_mmcsd_hlos_hs_CORE_DEPENDENCY
sbl_lib_mmcsd_hlos_hs_PKG_LIST = sbl_lib_mmcsd_hlos_hs
sbl_lib_mmcsd_hlos_hs_INCLUDE = $(sbl_lib_mmcsd_hlos_hs_PATH)
sbl_lib_mmcsd_hlos_hs_SOCLIST = $(sbl_SOCLIST)
sbl_lib_mmcsd_hlos_hs_BOARDLIST = $(sbl_BOARDLIST)
export sbl_lib_mmcsd_hlos_hs_SOCLIST
export sbl_lib_mmcsd_hlos_hs_BOARDLIST
sbl_lib_mmcsd_hlos_hs_$(SOC)_CORELIST = mcu1_0
export sbl_lib_mmcsd_hlos_hs_$(SOC)_CORELIST

# SBL OSPI LIB
sbl_lib_ospi_COMP_LIST = sbl_lib_ospi
sbl_lib_ospi_RELPATH = ti/boot/sbl
export sbl_lib_ospi_OBJPATH = ti/boot/sbl/ospi
sbl_lib_ospi_LIBNAME = sbl_lib_ospi
sbl_lib_ospi_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_ospi_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/ospi
sbl_lib_ospi_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=ospi SBL_USE_DMA=yes
export sbl_lib_ospi_MAKEFILE
export sbl_lib_ospi_LIBNAME
export sbl_lib_ospi_LIBPATH
sbl_lib_ospi_BOARD_DEPENDENCY = yes
sbl_lib_ospi_SOC_DEPENDENCY = yes
sbl_lib_ospi_CORE_DEPENDENCY = no
export sbl_lib_ospi_COMP_LIST
export sbl_lib_ospi_BOARD_DEPENDENCY
export sbl_lib_ospi_CORE_DEPENDENCY
sbl_lib_ospi_PKG_LIST = sbl_lib_ospi
sbl_lib_ospi_INCLUDE = $(sbl_lib_ospi_PATH)
sbl_lib_ospi_SOCLIST = $(sbl_SOCLIST)
sbl_lib_ospi_BOARDLIST = $(sbl_BOARDLIST)
export sbl_lib_ospi_SOCLIST
export sbl_lib_ospi_BOARDLIST
sbl_lib_ospi_$(SOC)_CORELIST = mcu1_0
export sbl_lib_ospi_$(SOC)_CORELIST

# SBL OSPI LIB - HS build variant
sbl_lib_ospi_hs_COMP_LIST = sbl_lib_ospi_hs
sbl_lib_ospi_hs_RELPATH = ti/boot/sbl
export sbl_lib_ospi_hs_OBJPATH = ti/boot/sbl/ospi_hs
sbl_lib_ospi_hs_LIBNAME = sbl_lib_ospi_hs
sbl_lib_ospi_hs_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_ospi_hs_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/ospi_hs
sbl_lib_ospi_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=ospi SBL_USE_DMA=yes BUILD_HS=yes
export sbl_lib_ospi_hs_MAKEFILE
export sbl_lib_ospi_hs_LIBNAME
export sbl_lib_ospi_hs_LIBPATH
sbl_lib_ospi_hs_BOARD_DEPENDENCY = yes
sbl_lib_ospi_hs_SOC_DEPENDENCY = yes
sbl_lib_ospi_hs_CORE_DEPENDENCY = no
export sbl_lib_ospi_hs_COMP_LIST
export sbl_lib_ospi_hs_BOARD_DEPENDENCY
export sbl_lib_ospi_hs_CORE_DEPENDENCY
sbl_lib_ospi_hs_PKG_LIST = sbl_lib_ospi_hs
sbl_lib_ospi_hs_INCLUDE = $(sbl_lib_ospi_hs_PATH)
sbl_lib_ospi_hs_SOCLIST = $(sbl_SOCLIST)
sbl_lib_ospi_hs_BOARDLIST = $(sbl_BOARDLIST)
export sbl_lib_ospi_hs_SOCLIST
export sbl_lib_ospi_hs_BOARDLIST
sbl_lib_ospi_hs_$(SOC)_CORELIST = mcu1_0
export sbl_lib_ospi_hs_$(SOC)_CORELIST

# SBL OSPI HLOS LIB
sbl_lib_ospi_hlos_COMP_LIST = sbl_lib_ospi_hlos
sbl_lib_ospi_hlos_RELPATH = ti/boot/sbl
export sbl_lib_ospi_hlos_OBJPATH = ti/boot/sbl/ospi_hlos
sbl_lib_ospi_hlos_LIBNAME = sbl_lib_ospi_hlos
sbl_lib_ospi_hlos_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_ospi_hlos_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/ospi_hlos
sbl_lib_ospi_hlos_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=ospi HLOS_BOOT=yes SBL_USE_DMA=yes
export sbl_lib_ospi_hlos_MAKEFILE
export sbl_lib_ospi_hlos_LIBNAME
export sbl_lib_ospi_hlos_LIBPATH
sbl_lib_ospi_hlos_BOARD_DEPENDENCY = yes
sbl_lib_ospi_hlos_SOC_DEPENDENCY = yes
sbl_lib_ospi_hlos_CORE_DEPENDENCY = no
export sbl_lib_ospi_hlos_COMP_LIST
export sbl_lib_ospi_hlos_BOARD_DEPENDENCY
export sbl_lib_ospi_hlos_CORE_DEPENDENCY
sbl_lib_ospi_hlos_PKG_LIST = sbl_lib_ospi_hlos
sbl_lib_ospi_hlos_INCLUDE = $(sbl_lib_ospi_hlos_PATH)
sbl_lib_ospi_hlos_SOCLIST = $(sbl_SOCLIST)
sbl_lib_ospi_hlos_BOARDLIST = $(sbl_BOARDLIST)
export sbl_lib_ospi_hlos_SOCLIST
export sbl_lib_ospi_hlos_BOARDLIST
sbl_lib_ospi_hlos_$(SOC)_CORELIST = mcu1_0
export sbl_lib_ospi_hlos_$(SOC)_CORELIST

# SBL OSPI HLOS LIB - HS build variant
sbl_lib_ospi_hlos_hs_COMP_LIST = sbl_lib_ospi_hlos_hs
sbl_lib_ospi_hlos_hs_RELPATH = ti/boot/sbl
export sbl_lib_ospi_hlos_hs_OBJPATH = ti/boot/sbl/ospi_hlos_hs
sbl_lib_ospi_hlos_hs_LIBNAME = sbl_lib_ospi_hlos_hs
sbl_lib_ospi_hlos_hs_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_ospi_hlos_hs_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/ospi_hlos_hs
sbl_lib_ospi_hlos_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=ospi HLOS_BOOT=yes SBL_USE_DMA=yes BUILD_HS=yes
export sbl_lib_ospi_hlos_hs_MAKEFILE
export sbl_lib_ospi_hlos_hs_LIBNAME
export sbl_lib_ospi_hlos_hs_LIBPATH
sbl_lib_ospi_hlos_hs_BOARD_DEPENDENCY = yes
sbl_lib_ospi_hlos_hs_SOC_DEPENDENCY = yes
sbl_lib_ospi_hlos_hs_CORE_DEPENDENCY = no
export sbl_lib_ospi_hlos_hs_COMP_LIST
export sbl_lib_ospi_hlos_hs_BOARD_DEPENDENCY
export sbl_lib_ospi_hlos_hs_CORE_DEPENDENCY
sbl_lib_ospi_hlos_hs_PKG_LIST = sbl_lib_ospi_hlos_hs
sbl_lib_ospi_hlos_hs_INCLUDE = $(sbl_lib_ospi_hlos_hs_PATH)
sbl_lib_ospi_hlos_hs_SOCLIST = $(sbl_SOCLIST)
sbl_lib_ospi_hlos_hs_BOARDLIST = $(sbl_BOARDLIST)
export sbl_lib_ospi_hlos_hs_SOCLIST
export sbl_lib_ospi_hlos_hs_BOARDLIST
sbl_lib_ospi_hlos_hs_$(SOC)_CORELIST = mcu1_0
export sbl_lib_ospi_hlos_hs_$(SOC)_CORELIST

# SBL OSPI LIB with NON-DMA
export sbl_lib_ospi_nondma_COMP_LIST = sbl_lib_ospi_nondma
sbl_lib_ospi_nondma_RELPATH = ti/boot/sbl
export sbl_lib_ospi_nondma_OBJPATH = ti/boot/sbl/ospi_nondma
sbl_lib_ospi_nondma_PATH = $(PDK_SBL_COMP_PATH)
export sbl_lib_ospi_nondma_LIBNAME = sbl_lib_ospi_nondma
export sbl_lib_ospi_nondma_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/ospi
export sbl_lib_ospi_nondma_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=ospi SBL_USE_DMA=no
export sbl_lib_ospi_nondma_BOARD_DEPENDENCY = yes
export sbl_lib_ospi_nondma_SOC_DEPENDENCY = yes
export sbl_lib_ospi_nondma_CORE_DEPENDENCY = no
sbl_lib_ospi_nondma_PKG_LIST = sbl_lib_ospi_nondma
sbl_lib_ospi_nondma_INCLUDE = $(sbl_lib_ospi_nondma_PATH)
export sbl_lib_ospi_nondma_SOCLIST = $(sbl_SOCLIST)
export sbl_lib_ospi_nondma_BOARDLIST = $(sbl_BOARDLIST)
export sbl_lib_ospi_nondma_$(SOC)_CORELIST = mcu1_0

# SBL OSPI LIB with NON-DMA - HS build variant
export sbl_lib_ospi_nondma_hs_COMP_LIST = sbl_lib_ospi_nondma_hs
sbl_lib_ospi_nondma_hs_RELPATH = ti/boot/sbl
export sbl_lib_ospi_nondma_hs_OBJPATH = ti/boot/sbl/ospi_nondma_hs
sbl_lib_ospi_nondma_hs_PATH = $(PDK_SBL_COMP_PATH)
export sbl_lib_ospi_nondma_hs_LIBNAME = sbl_lib_ospi_nondma_hs
export sbl_lib_ospi_nondma_hs_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/ospi_hs
export sbl_lib_ospi_nondma_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=ospi SBL_USE_DMA=no BUILD_HS=yes
export sbl_lib_ospi_nondma_hs_BOARD_DEPENDENCY = yes
export sbl_lib_ospi_nondma_hs_SOC_DEPENDENCY = yes
export sbl_lib_ospi_nondma_hs_CORE_DEPENDENCY = no
sbl_lib_ospi_nondma_hs_PKG_LIST = sbl_lib_ospi_nondma_hs
sbl_lib_ospi_nondma_hs_INCLUDE = $(sbl_lib_ospi_nondma_hs_PATH)
export sbl_lib_ospi_nondma_hs_SOCLIST = $(sbl_SOCLIST)
export sbl_lib_ospi_nondma_hs_BOARDLIST = $(sbl_BOARDLIST)
export sbl_lib_ospi_nondma_hs_$(SOC)_CORELIST = mcu1_0

# SBL OSPI HLOS LIB with NON-DMA
export sbl_lib_ospi_nondma_hlos_COMP_LIST = sbl_lib_ospi_nondma_hlos
sbl_lib_ospi_nondma_hlos_RELPATH = ti/boot/sbl
export sbl_lib_ospi_nondma_hlos_OBJPATH = ti/boot/sbl/ospi_nondma_hlos
sbl_lib_ospi_nondma_hlos_PATH = $(PDK_SBL_COMP_PATH)
export sbl_lib_ospi_nondma_hlos_LIBNAME = sbl_lib_ospi_nondma_hlos
export sbl_lib_ospi_nondma_hlos_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/ospi_hlos
export sbl_lib_ospi_nondma_hlos_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=ospi HLOS_BOOT=yes SBL_USE_DMA=no
export sbl_lib_ospi_nondma_hlos_BOARD_DEPENDENCY = yes
export sbl_lib_ospi_nondma_hlos_SOC_DEPENDENCY = yes
export sbl_lib_ospi_nondma_hlos_CORE_DEPENDENCY = no
sbl_lib_ospi_nondma_hlos_PKG_LIST = sbl_lib_ospi_nondma_hlos
sbl_lib_ospi_nondma_hlos_INCLUDE = $(sbl_lib_ospi_nondma_hlos_PATH)
export sbl_lib_ospi_nondma_hlos_SOCLIST = $(sbl_SOCLIST)
export sbl_lib_ospi_nondma_hlos_BOARDLIST = $(sbl_BOARDLIST)
export sbl_lib_ospi_nondma_hlos_$(SOC)_CORELIST = mcu1_0

# SBL OSPI HLOS LIB with NON-DMA - HS build variant
export sbl_lib_ospi_nondma_hlos_hs_COMP_LIST = sbl_lib_ospi_nondma_hlos_hs
sbl_lib_ospi_nondma_hlos_hs_RELPATH = ti/boot/sbl
export sbl_lib_ospi_nondma_hlos_hs_OBJPATH = ti/boot/sbl/ospi_nondma_hlos_hs
sbl_lib_ospi_nondma_hlos_hs_PATH = $(PDK_SBL_COMP_PATH)
export sbl_lib_ospi_nondma_hlos_hs_LIBNAME = sbl_lib_ospi_nondma_hlos_hs
export sbl_lib_ospi_nondma_hlos_hs_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/ospi_hlos_hs
export sbl_lib_ospi_nondma_hlos_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=ospi HLOS_BOOT=yes SBL_USE_DMA=no BUILD_HS=yes
export sbl_lib_ospi_nondma_hlos_hs_BOARD_DEPENDENCY = yes
export sbl_lib_ospi_nondma_hlos_hs_SOC_DEPENDENCY = yes
export sbl_lib_ospi_nondma_hlos_hs_CORE_DEPENDENCY = no
sbl_lib_ospi_nondma_hlos_hs_PKG_LIST = sbl_lib_ospi_nondma_hlos_hs
sbl_lib_ospi_nondma_hlos_hs_INCLUDE = $(sbl_lib_ospi_nondma_hlos_hs_PATH)
export sbl_lib_ospi_nondma_hlos_hs_SOCLIST = $(sbl_SOCLIST)
export sbl_lib_ospi_nondma_hlos_hs_BOARDLIST = $(sbl_BOARDLIST)
export sbl_lib_ospi_nondma_hlos_hs_$(SOC)_CORELIST = mcu1_0

# SBL HYPERFLASH LIB
sbl_lib_hyperflash_COMP_LIST = sbl_lib_hyperflash
sbl_lib_hyperflash_RELPATH = ti/boot/sbl
export sbl_lib_hyperflash_OBJPATH = ti/boot/sbl/hyperflash
sbl_lib_hyperflash_LIBNAME = sbl_lib_hyperflash
sbl_lib_hyperflash_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_hyperflash_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/hyperflash
sbl_lib_hyperflash_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=hyperflash SBL_USE_DMA=yes
export sbl_lib_hyperflash_MAKEFILE
export sbl_lib_hyperflash_LIBNAME
export sbl_lib_hyperflash_LIBPATH
sbl_lib_hyperflash_BOARD_DEPENDENCY = yes
sbl_lib_hyperflash_SOC_DEPENDENCY = yes
sbl_lib_hyperflash_CORE_DEPENDENCY = no
export sbl_lib_hyperflash_COMP_LIST
export sbl_lib_hyperflash_BOARD_DEPENDENCY
export sbl_lib_hyperflash_CORE_DEPENDENCY
sbl_lib_hyperflash_PKG_LIST = sbl_lib_hyperflash
sbl_lib_hyperflash_INCLUDE = $(sbl_lib_hyperflash_PATH)
sbl_lib_hyperflash_SOCLIST = j721e
sbl_lib_hyperflash_BOARDLIST = j721e_evm
export sbl_lib_hyperflash_SOCLIST
export sbl_lib_hyperflash_BOARDLIST
sbl_lib_hyperflash_$(SOC)_CORELIST = mcu1_0
export sbl_lib_hyperflash_$(SOC)_CORELIST

# SBL HYPERFLASH LIB - HS build variant
sbl_lib_hyperflash_hs_COMP_LIST = sbl_lib_hyperflash_hs
sbl_lib_hyperflash_hs_RELPATH = ti/boot/sbl
export sbl_lib_hyperflash_hs_OBJPATH = ti/boot/sbl/hyperflash_hs
sbl_lib_hyperflash_hs_LIBNAME = sbl_lib_hyperflash_hs
sbl_lib_hyperflash_hs_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_hyperflash_hs_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/hyperflash_hs
sbl_lib_hyperflash_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=hyperflash SBL_USE_DMA=yes BUILD_HS=yes
export sbl_lib_hyperflash_hs_MAKEFILE
export sbl_lib_hyperflash_hs_LIBNAME
export sbl_lib_hyperflash_hs_LIBPATH
sbl_lib_hyperflash_hs_BOARD_DEPENDENCY = yes
sbl_lib_hyperflash_hs_SOC_DEPENDENCY = yes
sbl_lib_hyperflash_hs_CORE_DEPENDENCY = no
export sbl_lib_hyperflash_hs_COMP_LIST
export sbl_lib_hyperflash_hs_BOARD_DEPENDENCY
export sbl_lib_hyperflash_hs_CORE_DEPENDENCY
sbl_lib_hyperflash_hs_PKG_LIST = sbl_lib_hyperflash_hs
sbl_lib_hyperflash_hs_INCLUDE = $(sbl_lib_hyperflash_hs_PATH)
sbl_lib_hyperflash_hs_SOCLIST = j721e
sbl_lib_hyperflash_hs_BOARDLIST = j721e_evm
export sbl_lib_hyperflash_hs_SOCLIST
export sbl_lib_hyperflash_hs_BOARDLIST
sbl_lib_hyperflash_hs_$(SOC)_CORELIST = mcu1_0
export sbl_lib_hyperflash_hs_$(SOC)_CORELIST

# SBL HYPERFLASH HLOS LIB
sbl_lib_hyperflash_hlos_COMP_LIST = sbl_lib_hyperflash_hlos
sbl_lib_hyperflash_hlos_RELPATH = ti/boot/sbl
export sbl_lib_hyperflash_hlos_OBJPATH = ti/boot/sbl/hyperflash_hlos
sbl_lib_hyperflash_hlos_LIBNAME = sbl_lib_hyperflash_hlos
sbl_lib_hyperflash_hlos_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_hyperflash_hlos_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/hyperflash_hlos
sbl_lib_hyperflash_hlos_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=hyperflash HLOS_BOOT=yes SBL_USE_DMA=yes
export sbl_lib_hyperflash_hlos_MAKEFILE
export sbl_lib_hyperflash_hlos_LIBNAME
export sbl_lib_hyperflash_hlos_LIBPATH
sbl_lib_hyperflash_hlos_BOARD_DEPENDENCY = yes
sbl_lib_hyperflash_hlos_SOC_DEPENDENCY = yes
sbl_lib_hyperflash_hlos_CORE_DEPENDENCY = no
export sbl_lib_hyperflash_hlos_COMP_LIST
export sbl_lib_hyperflash_hlos_BOARD_DEPENDENCY
export sbl_lib_hyperflash_hlos_CORE_DEPENDENCY
sbl_lib_hyperflash_hlos_PKG_LIST = sbl_lib_hyperflash_hlos
sbl_lib_hyperflash_hlos_INCLUDE = $(sbl_lib_hyperflash_hlos_PATH)
sbl_lib_hyperflash_hlos_SOCLIST = j721e
sbl_lib_hyperflash_hlos_BOARDLIST = j721e_evm
export sbl_lib_hyperflash_hlos_SOCLIST
export sbl_lib_hyperflash_hlos_BOARDLIST
sbl_lib_hyperflash_hlos_$(SOC)_CORELIST = mcu1_0
export sbl_lib_hyperflash_hlos_$(SOC)_CORELIST

# SBL HYPERFLASH HLOS LIB - HS build variant
sbl_lib_hyperflash_hlos_hs_COMP_LIST = sbl_lib_hyperflash_hlos_hs
sbl_lib_hyperflash_hlos_hs_RELPATH = ti/boot/sbl
export sbl_lib_hyperflash_hlos_hs_OBJPATH = ti/boot/sbl/hyperflash_hlos_hs
sbl_lib_hyperflash_hlos_hs_LIBNAME = sbl_lib_hyperflash_hlos_hs
sbl_lib_hyperflash_hlos_hs_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_hyperflash_hlos_hs_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/hyperflash_hlos_hs
sbl_lib_hyperflash_hlos_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=hyperflash HLOS_BOOT=yes SBL_USE_DMA=yes BUILD_HS=yes
export sbl_lib_hyperflash_hlos_hs_MAKEFILE
export sbl_lib_hyperflash_hlos_hs_LIBNAME
export sbl_lib_hyperflash_hlos_hs_LIBPATH
sbl_lib_hyperflash_hlos_hs_BOARD_DEPENDENCY = yes
sbl_lib_hyperflash_hlos_hs_SOC_DEPENDENCY = yes
sbl_lib_hyperflash_hlos_hs_CORE_DEPENDENCY = no
export sbl_lib_hyperflash_hlos_hs_COMP_LIST
export sbl_lib_hyperflash_hlos_hs_BOARD_DEPENDENCY
export sbl_lib_hyperflash_hlos_hs_CORE_DEPENDENCY
sbl_lib_hyperflash_hlos_hs_PKG_LIST = sbl_lib_hyperflash_hlos_hs
sbl_lib_hyperflash_hlos_hs_INCLUDE = $(sbl_lib_hyperflash_hlos_hs_PATH)
sbl_lib_hyperflash_hlos_hs_SOCLIST = j721e
sbl_lib_hyperflash_hlos_hs_BOARDLIST = j721e_evm
export sbl_lib_hyperflash_hlos_hs_SOCLIST
export sbl_lib_hyperflash_hlos_hs_BOARDLIST
sbl_lib_hyperflash_hlos_hs_$(SOC)_CORELIST = mcu1_0
export sbl_lib_hyperflash_hlos_hs_$(SOC)_CORELIST

# SBL UART LIB
sbl_lib_uart_COMP_LIST = sbl_lib_uart
sbl_lib_uart_RELPATH = ti/boot/sbl
export sbl_lib_uart_OBJPATH = ti/boot/sbl/uart
sbl_lib_uart_LIBNAME = sbl_lib_uart
sbl_lib_uart_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_uart_LIBNAME = sbl_lib_uart
sbl_lib_uart_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/uart
sbl_lib_uart_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=uart SBL_USE_DMA=yes
export sbl_lib_uart_MAKEFILE
export sbl_lib_uart_LIBNAME
export sbl_lib_uart_LIBPATH
sbl_lib_uart_BOARD_DEPENDENCY = yes
sbl_lib_uart_SOC_DEPENDENCY = yes
sbl_lib_uart_CORE_DEPENDENCY = no
export sbl_lib_uart_COMP_LIST
export sbl_lib_uart_BOARD_DEPENDENCY
export sbl_lib_uart_CORE_DEPENDENCY
sbl_lib_uart_PKG_LIST = sbl_lib_uart
sbl_lib_uart_INCLUDE = $(sbl_lib_uart_PATH)
sbl_lib_uart_SOCLIST = $(sbl_SOCLIST)
sbl_lib_uart_BOARDLIST = $(sbl_BOARDLIST)
export sbl_lib_uart_SOCLIST
export sbl_lib_uart_BOARDLIST
sbl_lib_uart_$(SOC)_CORELIST = mcu1_0
export sbl_lib_uart_$(SOC)_CORELIST

# SBL UART LIB - HS build variant
sbl_lib_uart_hs_COMP_LIST = sbl_lib_uart_hs
sbl_lib_uart_hs_RELPATH = ti/boot/sbl
export sbl_lib_uart_hs_OBJPATH = ti/boot/sbl/uart_hs
sbl_lib_uart_hs_LIBNAME = sbl_lib_uart_hs
sbl_lib_uart_hs_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_uart_hs_LIBNAME = sbl_lib_uart_hs
sbl_lib_uart_hs_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/uart_hs
sbl_lib_uart_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=uart SBL_USE_DMA=yes BUILD_HS=yes
export sbl_lib_uart_hs_MAKEFILE
export sbl_lib_uart_hs_LIBNAME
export sbl_lib_uart_hs_LIBPATH
sbl_lib_uart_hs_BOARD_DEPENDENCY = yes
sbl_lib_uart_hs_SOC_DEPENDENCY = yes
sbl_lib_uart_hs_CORE_DEPENDENCY = no
export sbl_lib_uart_hs_COMP_LIST
export sbl_lib_uart_hs_BOARD_DEPENDENCY
export sbl_lib_uart_hs_CORE_DEPENDENCY
sbl_lib_uart_hs_PKG_LIST = sbl_lib_uart_hs
sbl_lib_uart_hs_INCLUDE = $(sbl_lib_uart_hs_PATH)
sbl_lib_uart_hs_SOCLIST = $(sbl_SOCLIST)
sbl_lib_uart_hs_BOARDLIST = $(sbl_BOARDLIST)
export sbl_lib_uart_hs_SOCLIST
export sbl_lib_uart_hs_BOARDLIST
sbl_lib_uart_hs_$(SOC)_CORELIST = mcu1_0
export sbl_lib_uart_hs_$(SOC)_CORELIST

# SBL QSPI LIB
sbl_lib_qspi_COMP_LIST = sbl_lib_qspi
sbl_lib_qspi_RELPATH = ti/boot/sbl
export sbl_lib_qspi_OBJPATH = ti/boot/sbl/qspi
sbl_lib_qspi_LIBNAME = sbl_lib_qspi
sbl_lib_qspi_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_qspi_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/qspi
sbl_lib_qspi_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=qspi SBL_USE_DMA=yes
export sbl_lib_qspi_MAKEFILE
export sbl_lib_qspi_LIBNAME
export sbl_lib_qspi_LIBPATH
sbl_lib_qspi_BOARD_DEPENDENCY = yes
sbl_lib_qspi_SOC_DEPENDENCY = yes
sbl_lib_qspi_CORE_DEPENDENCY = no
export sbl_lib_qspi_COMP_LIST
export sbl_lib_qspi_BOARD_DEPENDENCY
export sbl_lib_qspi_CORE_DEPENDENCY
sbl_lib_qspi_PKG_LIST = sbl_lib_qspi
sbl_lib_qspi_INCLUDE = $(sbl_lib_qspi_PATH)
sbl_lib_qspi_SOCLIST = tpr12 awr294x
sbl_lib_qspi_BOARDLIST = tpr12_evm tpr12_qt awr294x_evm
export sbl_lib_qspi_SOCLIST
export sbl_lib_qspi_BOARDLIST
sbl_lib_qspi_$(SOC)_CORELIST = mcu1_0
export sbl_lib_qspi_$(SOC)_CORELIST

# SBL QSPI LIB with NON-DMA
export sbl_lib_qspi_nondma_COMP_LIST = sbl_lib_qspi_nondma
sbl_lib_qspi_nondma_RELPATH = ti/boot/sbl
export sbl_lib_qspi_nondma_OBJPATH = ti/boot/sbl/qspi_nondma
sbl_lib_qspi_nondma_PATH = $(PDK_SBL_COMP_PATH)
export sbl_lib_qspi_nondma_LIBNAME = sbl_lib_qspi_nondma
export sbl_lib_qspi_nondma_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/qspi
export sbl_lib_qspi_nondma_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=qspi SBL_USE_DMA=no
export sbl_lib_qspi_nondma_BOARD_DEPENDENCY = yes
export sbl_lib_qspi_nondma_SOC_DEPENDENCY = yes
export sbl_lib_qspi_nondma_CORE_DEPENDENCY = no
sbl_lib_qspi_nondma_PKG_LIST = sbl_lib_qspi_nondma
sbl_lib_qspi_nondma_INCLUDE = $(sbl_lib_qspi_nondma_PATH)
export sbl_lib_qspi_nondma_SOCLIST = tpr12 awr294x
export sbl_lib_qspi_nondma_BOARDLIST = tpr12_evm tpr12_qt awr294x_evm
export sbl_lib_qspi_nondma_$(SOC)_CORELIST = mcu1_0

#
# SBL Examples
#
# SBL MMCSD Image
sbl_mmcsd_img_COMP_LIST = sbl_mmcsd_img
sbl_mmcsd_img_RELPATH = ti/boot/sbl/board/k3
sbl_mmcsd_img_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)/mmcsd/bin
sbl_mmcsd_img_PATH = $(PDK_SBL_COMP_PATH)/board/k3
sbl_mmcsd_img_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=mmcsd SBL_USE_DMA=yes BUILD_HS=no
export sbl_mmcsd_img_MAKEFILE
export sbl_mmcsd_img_SBL_CERT_KEY=$(SBL_CERT_KEY)
sbl_mmcsd_img_BOARD_DEPENDENCY = yes
sbl_mmcsd_img_SOC_DEPENDENCY = yes
sbl_mmcsd_img_CORE_DEPENDENCY = no
export sbl_mmcsd_img_COMP_LIST
export sbl_mmcsd_img_BOARD_DEPENDENCY
export sbl_mmcsd_img_SOC_DEPENDENCY
export sbl_mmcsd_img_CORE_DEPENDENCY
sbl_mmcsd_img_PKG_LIST = sbl
sbl_mmcsd_img_INCLUDE = $(sbl_mmcsd_img_PATH)
sbl_mmcsd_img_BOARDLIST = $(sbl_BOARDLIST)
export sbl_mmcsd_img_BOARDLIST
sbl_mmcsd_img_$(SOC)_CORELIST = mcu1_0
export sbl_mmcsd_img_$(SOC)_CORELIST
sbl_mmcsd_img_SBL_IMAGEGEN = yes
export sbl_mmcsd_img_SBL_IMAGEGEN

# SBL MMCSD "HLOS Boot" Image
sbl_mmcsd_img_hlos_COMP_LIST = sbl_mmcsd_img_hlos
sbl_mmcsd_img_hlos_RELPATH = ti/boot/sbl/board/k3
sbl_mmcsd_img_hlos_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)/mmcsd/bin
sbl_mmcsd_img_hlos_PATH = $(PDK_SBL_COMP_PATH)/board/k3
sbl_mmcsd_img_hlos_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=mmcsd HLOS_BOOT=yes SBL_USE_DMA=yes BUILD_HS=no
export sbl_mmcsd_img_hlos_MAKEFILE
export sbl_mmcsd_img_hlos_SBL_CERT_KEY=$(SBL_CERT_KEY)
sbl_mmcsd_img_hlos_BOARD_DEPENDENCY = yes
sbl_mmcsd_img_hlos_SOC_DEPENDENCY = yes
sbl_mmcsd_img_hlos_CORE_DEPENDENCY = no
export sbl_mmcsd_img_hlos_COMP_LIST
export sbl_mmcsd_img_hlos_BOARD_DEPENDENCY
export sbl_mmcsd_img_hlos_SOC_DEPENDENCY
export sbl_mmcsd_img_hlos_CORE_DEPENDENCY
sbl_mmcsd_img_hlos_PKG_LIST = sbl
sbl_mmcsd_img_hlos_INCLUDE = $(sbl_mmcsd_img_hlos_PATH)
sbl_mmcsd_img_hlos_BOARDLIST = $(sbl_BOARDLIST)
export sbl_mmcsd_img_hlos_BOARDLIST
sbl_mmcsd_img_hlos_$(SOC)_CORELIST = mcu1_0
export sbl_mmcsd_img_hlos_$(SOC)_CORELIST
sbl_mmcsd_img_hlos_SBL_IMAGEGEN = yes
export sbl_mmcsd_img_hlos_SBL_IMAGEGEN

# SBL MMCSD Image - For HS build
export sbl_mmcsd_img_hs_COMP_LIST = sbl_mmcsd_img_hs
sbl_mmcsd_img_hs_RELPATH = ti/boot/sbl/board/k3
sbl_mmcsd_img_hs_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)_hs/mmcsd/bin
#Note: The below variable is purposefully set to build folder - different than
#non-hs app.
#The APPNAME or app -C directory (_PATH variable) should be unique as the complier
#creates lto_$APPNAME optimization file in the directory in which -C is called
#Because of this multiple lto_ files with same name can be created if app name
#and app path are same
sbl_mmcsd_img_hs_PATH = $(PDK_SBL_COMP_PATH)/build
export sbl_mmcsd_img_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=mmcsd SBL_USE_DMA=yes BUILD_HS=yes
export sbl_mmcsd_img_hs_SBL_CERT_KEY=$(SBL_CERT_KEY_HS)
export sbl_mmcsd_img_hs_BOARD_DEPENDENCY = yes
export sbl_mmcsd_img_hs_SOC_DEPENDENCY = yes
export sbl_mmcsd_img_hs_CORE_DEPENDENCY = no
sbl_mmcsd_img_hs_PKG_LIST = sbl
sbl_mmcsd_img_hs_INCLUDE = $(sbl_mmcsd_img_hs_PATH)
export sbl_mmcsd_img_hs_BOARDLIST = $(sbl_BOARDLIST)
export sbl_mmcsd_img_hs_$(SOC)_CORELIST = mcu1_0
export sbl_mmcsd_img_hs_SBL_IMAGEGEN = yes

# SBL MMCSD "HLOS Boot" Image - For HS build
export sbl_mmcsd_img_hlos_hs_COMP_LIST = sbl_mmcsd_img_hlos_hs
sbl_mmcsd_img_hlos_hs_RELPATH = ti/boot/sbl/board/k3
sbl_mmcsd_img_hlos_hs_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)_hs/mmcsd/bin
#Note: The below variable is purposefully set to build folder - different than
#non-hs app.
#The APPNAME or app -C directory (_PATH variable) should be unique as the complier
#creates lto_$APPNAME optimization file in the directory in which -C is called
#Because of this multiple lto_ files with same name can be created if app name
#and app path are same
sbl_mmcsd_img_hlos_hs_PATH = $(PDK_SBL_COMP_PATH)/build
export sbl_mmcsd_img_hlos_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=mmcsd HLOS_BOOT=yes SBL_USE_DMA=yes BUILD_HS=yes
export sbl_mmcsd_img_hlos_hs_SBL_CERT_KEY=$(SBL_CERT_KEY_HS)
export sbl_mmcsd_img_hlos_hs_BOARD_DEPENDENCY = yes
export sbl_mmcsd_img_hlos_hs_SOC_DEPENDENCY = yes
export sbl_mmcsd_img_hlos_hs_CORE_DEPENDENCY = no
sbl_mmcsd_img_hlos_hs_PKG_LIST = sbl
sbl_mmcsd_img_hlos_hs_INCLUDE = $(sbl_mmcsd_img_hlos_hs_PATH)
export sbl_mmcsd_img_hlos_hs_BOARDLIST = $(sbl_BOARDLIST)
export sbl_mmcsd_img_hlos_hs_$(SOC)_CORELIST = mcu1_0
export sbl_mmcsd_img_hlos_hs_SBL_IMAGEGEN = yes


# SBL OSPI Image
sbl_ospi_img_COMP_LIST = sbl_ospi_img
sbl_ospi_img_RELPATH = ti/boot/sbl/board/k3
sbl_ospi_img_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)/ospi/bin
sbl_ospi_img_PATH = $(PDK_SBL_COMP_PATH)/board/k3
sbl_ospi_img_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=ospi SBL_USE_DMA=yes BUILD_HS=no
export sbl_ospi_img_MAKEFILE
export sbl_ospi_img_SBL_CERT_KEY=$(SBL_CERT_KEY)
sbl_ospi_img_BOARD_DEPENDENCY = yes
sbl_ospi_img_SOC_DEPENDENCY = yes
sbl_ospi_img_CORE_DEPENDENCY = no
export sbl_ospi_img_COMP_LIST
export sbl_ospi_img_BOARD_DEPENDENCY
export sbl_ospi_img_SOC_DEPENDENCY
export sbl_ospi_img_CORE_DEPENDENCY
sbl_ospi_img_PKG_LIST = sbl
sbl_ospi_img_INCLUDE = $(sbl_ospi_img_PATH)
sbl_ospi_img_BOARDLIST = $(sbl_BOARDLIST)
export sbl_ospi_img_BOARDLIST
sbl_ospi_img_$(SOC)_CORELIST = mcu1_0
export sbl_ospi_img_$(SOC)_CORELIST
sbl_ospi_img_SBL_IMAGEGEN = yes
export sbl_ospi_img_SBL_IMAGEGEN

# SBL OSPI "HLOS Boot" Image
sbl_ospi_img_hlos_COMP_LIST = sbl_ospi_img_hlos
sbl_ospi_img_hlos_RELPATH = ti/boot/sbl/board/k3
sbl_ospi_img_hlos_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)/ospi/bin
sbl_ospi_img_hlos_PATH = $(PDK_SBL_COMP_PATH)/board/k3
ifeq ($(SOC),$(filter $(SOC), am64x))
  sbl_ospi_img_hlos_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=ospi HLOS_BOOT=yes SBL_USE_DMA=no BUILD_HS=no
else
  sbl_ospi_img_hlos_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=ospi HLOS_BOOT=yes SBL_USE_DMA=yes BUILD_HS=no
endif
export sbl_ospi_img_hlos_MAKEFILE
export sbl_ospi_img_hlos_SBL_CERT_KEY=$(SBL_CERT_KEY)
sbl_ospi_img_hlos_BOARD_DEPENDENCY = yes
sbl_ospi_img_hlos_SOC_DEPENDENCY = yes
sbl_ospi_img_hlos_CORE_DEPENDENCY = no
export sbl_ospi_img_hlos_COMP_LIST
export sbl_ospi_img_hlos_BOARD_DEPENDENCY
export sbl_ospi_img_hlos_SOC_DEPENDENCY
export sbl_ospi_img_hlos_CORE_DEPENDENCY
sbl_ospi_img_hlos_PKG_LIST = sbl
sbl_ospi_img_hlos_INCLUDE = $(sbl_ospi_img_hlos_PATH)
sbl_ospi_img_hlos_BOARDLIST = $(sbl_BOARDLIST)
export sbl_ospi_img_hlos_BOARDLIST
sbl_ospi_img_hlos_$(SOC)_CORELIST = mcu1_0
export sbl_ospi_img_hlos_$(SOC)_CORELIST
sbl_ospi_img_hlos_SBL_IMAGEGEN = yes
export sbl_ospi_img_hlos_SBL_IMAGEGEN

# SBL OSPI Image - For HS build
export sbl_ospi_img_hs_COMP_LIST = sbl_ospi_img_hs
sbl_ospi_img_hs_RELPATH = ti/boot/sbl/board/k3
sbl_ospi_img_hs_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)_hs/ospi/bin
#Note: The below variable is purposefully set to build folder - different than
#non-hs app.
#The APPNAME or app -C directory (_PATH variable) should be unique as the complier
#creates lto_$APPNAME optimization file in the directory in which -C is called
#Because of this multiple lto_ files with same name can be created if app name
#and app path are same
sbl_ospi_img_hs_PATH = $(PDK_SBL_COMP_PATH)/build
export sbl_ospi_img_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=ospi SBL_USE_DMA=yes BUILD_HS=yes
export sbl_ospi_img_hs_SBL_CERT_KEY=$(SBL_CERT_KEY_HS)
export sbl_ospi_img_hs_BOARD_DEPENDENCY = yes
export sbl_ospi_img_hs_SOC_DEPENDENCY = yes
export sbl_ospi_img_hs_CORE_DEPENDENCY = no
sbl_ospi_img_hs_PKG_LIST = sbl
sbl_ospi_img_hs_INCLUDE = $(sbl_ospi_img_hs_PATH)
export sbl_ospi_img_hs_BOARDLIST = $(sbl_BOARDLIST)
export sbl_ospi_img_hs_$(SOC)_CORELIST = mcu1_0
export sbl_ospi_img_hs_SBL_IMAGEGEN = yes

# SBL OSPI "HLOS Boot" Image - For HS build
export sbl_ospi_img_hlos_hs_COMP_LIST = sbl_ospi_img_hlos_hs
sbl_ospi_img_hlos_hs_RELPATH = ti/boot/sbl/board/k3
sbl_ospi_img_hlos_hs_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)_hs/ospi/bin
#Note: The below variable is purposefully set to build folder - different than
#non-hs app.
#The APPNAME or app -C directory (_PATH variable) should be unique as the complier
#creates lto_$APPNAME optimization file in the directory in which -C is called
#Because of this multiple lto_ files with same name can be created if app name
#and app path are same
sbl_ospi_img_hlos_hs_PATH = $(PDK_SBL_COMP_PATH)/build
export sbl_ospi_img_hlos_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=ospi HLOS_BOOT=yes SBL_USE_DMA=yes BUILD_HS=yes
export sbl_ospi_img_hlos_hs_SBL_CERT_KEY=$(SBL_CERT_KEY_HS)
export sbl_ospi_img_hlos_hs_BOARD_DEPENDENCY = yes
export sbl_ospi_img_hlos_hs_SOC_DEPENDENCY = yes
export sbl_ospi_img_hlos_hs_CORE_DEPENDENCY = no
sbl_ospi_img_hlos_hs_PKG_LIST = sbl
sbl_ospi_img_hlos_hs_INCLUDE = $(sbl_ospi_img_hlos_hs_PATH)
export sbl_ospi_img_hlos_hs_BOARDLIST = $(sbl_BOARDLIST)
export sbl_ospi_img_hlos_hs_$(SOC)_CORELIST = mcu1_0
export sbl_ospi_img_hlos_hs_SBL_IMAGEGEN = yes

# KEYWRITER Image
# SBL HYPERFLASH Image
sbl_hyperflash_img_COMP_LIST = sbl_hyperflash_img
sbl_hyperflash_img_RELPATH = ti/boot/sbl/board/k3
sbl_hyperflash_img_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)/hyperflash/bin
sbl_hyperflash_img_PATH = $(PDK_SBL_COMP_PATH)/board/k3
sbl_hyperflash_img_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=hyperflash SBL_USE_DMA=yes BUILD_HS=no
export sbl_hyperflash_img_SBL_CERT_KEY=$(SBL_CERT_KEY)
export sbl_hyperflash_img_MAKEFILE
sbl_hyperflash_img_BOARD_DEPENDENCY = yes
sbl_hyperflash_img_SOC_DEPENDENCY = yes
sbl_hyperflash_img_CORE_DEPENDENCY = no
export sbl_hyperflash_img_COMP_LIST
export sbl_hyperflash_img_BOARD_DEPENDENCY
export sbl_hyperflash_img_SOC_DEPENDENCY
export sbl_hyperflash_img_CORE_DEPENDENCY
sbl_hyperflash_img_PKG_LIST = sbl
sbl_hyperflash_img_INCLUDE = $(sbl_hyperflash_img_PATH)
sbl_hyperflash_img_BOARDLIST = j721e_evm
export sbl_hyperflash_img_BOARDLIST
sbl_hyperflash_img_$(SOC)_CORELIST = mcu1_0
export sbl_hyperflash_img_$(SOC)_CORELIST
sbl_hyperflash_img_SBL_IMAGEGEN = yes
export sbl_hyperflash_img_SBL_IMAGEGEN

# SBL HYPERFLASH "HLOS Boot" Image
sbl_hyperflash_img_hlos_COMP_LIST = sbl_hyperflash_img_hlos
sbl_hyperflash_img_hlos_RELPATH = ti/boot/sbl/board/k3
sbl_hyperflash_img_hlos_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)/hyperflash/bin
sbl_hyperflash_img_hlos_PATH = $(PDK_SBL_COMP_PATH)/board/k3
sbl_hyperflash_img_hlos_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=hyperflash HLOS_BOOT=yes SBL_USE_DMA=yes BUILD_HS=no
export sbl_hyperflash_img_hlos_SBL_CERT_KEY=$(SBL_CERT_KEY)
export sbl_hyperflash_img_hlos_MAKEFILE
sbl_hyperflash_img_hlos_BOARD_DEPENDENCY = yes
sbl_hyperflash_img_hlos_SOC_DEPENDENCY = yes
sbl_hyperflash_img_hlos_CORE_DEPENDENCY = no
export sbl_hyperflash_img_hlos_COMP_LIST
export sbl_hyperflash_img_hlos_BOARD_DEPENDENCY
export sbl_hyperflash_img_hlos_SOC_DEPENDENCY
export sbl_hyperflash_img_hlos_CORE_DEPENDENCY
sbl_hyperflash_img_hlos_PKG_LIST = sbl
sbl_hyperflash_img_hlos_INCLUDE = $(sbl_hyperflash_img_hlos_PATH)
sbl_hyperflash_img_hlos_BOARDLIST = j721e_evm
export sbl_hyperflash_img_hlos_BOARDLIST
sbl_hyperflash_img_hlos_$(SOC)_CORELIST = mcu1_0
export sbl_hyperflash_img_hlos_$(SOC)_CORELIST
sbl_hyperflash_img_hlos_SBL_IMAGEGEN = yes
export sbl_hyperflash_img_hlos_SBL_IMAGEGEN

# SBL HYPERFLASH Image - For HS build
export sbl_hyperflash_img_hs_COMP_LIST = sbl_hyperflash_img_hs
sbl_hyperflash_img_hs_RELPATH = ti/boot/sbl/board/k3
sbl_hyperflash_img_hs_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)_hs/hyperflash/bin
#Note: The below variable is purposefully set to build folder - different than
#non-hs app.
#The APPNAME or app -C directory (_PATH variable) should be unique as the complier
#creates lto_$APPNAME optimization file in the directory in which -C is called
#Because of this multiple lto_ files with same name can be created if app name
#and app path are same
sbl_hyperflash_img_hs_PATH = $(PDK_SBL_COMP_PATH)/build
export sbl_hyperflash_img_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=hyperflash SBL_USE_DMA=yes BUILD_HS=yes
export sbl_hyperflash_img_hs_SBL_CERT_KEY=$(SBL_CERT_KEY_HS)
export sbl_hyperflash_img_hs_BOARD_DEPENDENCY = yes
export sbl_hyperflash_img_SOC_DEPENDENCY = yes
export sbl_hyperflash_img_hs_CORE_DEPENDENCY = no
sbl_hyperflash_img_hs_PKG_LIST = sbl
sbl_hyperflash_img_hs_INCLUDE = $(sbl_hyperflash_img_hs_PATH)
export sbl_hyperflash_img_hs_BOARDLIST = j721e_evm
export sbl_hyperflash_img_hs_$(SOC)_CORELIST = mcu1_0
export sbl_hyperflash_img_hs_SBL_IMAGEGEN = yes

# SBL HYPERFLASH "HLOS Boot" Image - For HS build
export sbl_hyperflash_img_hlos_hs_COMP_LIST = sbl_hyperflash_img_hlos_hs
sbl_hyperflash_img_hlos_hs_RELPATH = ti/boot/sbl/board/k3
sbl_hyperflash_img_hlos_hs_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)_hs/hyperflash/bin
#Note: The below variable is purposefully set to build folder - different than
#non-hs app.
#The APPNAME or app -C directory (_PATH variable) should be unique as the complier
#creates lto_$APPNAME optimization file in the directory in which -C is called
#Because of this multiple lto_ files with same name can be created if app name
#and app path are same
sbl_hyperflash_img_hlos_hs_PATH = $(PDK_SBL_COMP_PATH)/build
export sbl_hyperflash_img_hlos_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=hyperflash HLOS_BOOT=yes SBL_USE_DMA=yes BUILD_HS=yes
export sbl_hyperflash_img_hlos_hs_SBL_CERT_KEY=$(SBL_CERT_KEY_HS)
export sbl_hyperflash_img_hlos_hs_BOARD_DEPENDENCY = yes
export sbl_hyperflash_img_hlos_SOC_DEPENDENCY = yes
export sbl_hyperflash_img_hlos_hs_CORE_DEPENDENCY = no
sbl_hyperflash_img_hlos_hs_PKG_LIST = sbl
sbl_hyperflash_img_hlos_hs_INCLUDE = $(sbl_hyperflash_img_hlos_hs_PATH)
export sbl_hyperflash_img_hlos_hs_BOARDLIST = j721e_evm
export sbl_hyperflash_img_hlos_hs_$(SOC)_CORELIST = mcu1_0
export sbl_hyperflash_img_hlos_hs_SBL_IMAGEGEN = yes

# SBL UART Image
sbl_uart_img_COMP_LIST = sbl_uart_img
ifeq ($(SOC), $(filter $(SOC), tpr12 awr294x))
ifeq ($(SOC), $(filter $(SOC), tpr12))
sbl_uart_img_RELPATH = ti/boot/sbl/board/evmTPR12
endif
ifeq ($(SOC), $(filter $(SOC), awr294x))
sbl_uart_img_RELPATH = ti/boot/sbl/board/evmAWR294x
endif
else
sbl_uart_img_RELPATH = ti/boot/sbl/board/k3
endif
sbl_uart_img_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)/uart/bin
ifeq ($(SOC), $(filter $(SOC), tpr12 awr294x))
ifeq ($(SOC), $(filter $(SOC), tpr12))
sbl_uart_img_PATH = $(PDK_SBL_COMP_PATH)/board/evmTPR12
endif
ifeq ($(SOC), $(filter $(SOC), awr294x))
sbl_uart_img_PATH = $(PDK_SBL_COMP_PATH)/board/evmAWR294x
endif
else
sbl_uart_img_PATH = $(PDK_SBL_COMP_PATH)/board/k3
endif
sbl_uart_img_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=uart SBL_USE_DMA=yes BUILD_HS=no
export sbl_uart_img_MAKEFILE
export sbl_uart_img_SBL_CERT_KEY=$(SBL_CERT_KEY)
sbl_uart_img_BOARD_DEPENDENCY = yes
sbl_uart_img_SOC_DEPENDENCY = yes
sbl_uart_img_CORE_DEPENDENCY = no
export sbl_uart_img_COMP_LIST
export sbl_uart_img_BOARD_DEPENDENCY
export sbl_uart_img_SOC_DEPENDENCY
export sbl_uart_img_CORE_DEPENDENCY
sbl_uart_img_PKG_LIST = sbl
sbl_uart_img_INCLUDE = $(sbl_uart_img_PATH)
sbl_uart_img_BOARDLIST = $(sbl_BOARDLIST)
export sbl_uart_img_BOARDLIST
sbl_uart_img_$(SOC)_CORELIST = mcu1_0
export sbl_uart_img_$(SOC)_CORELIST
sbl_uart_img_SBL_IMAGEGEN = yes
export sbl_uart_img_SBL_IMAGEGEN

# SBL UART Image - For HS build
export sbl_uart_img_hs_COMP_LIST = sbl_uart_img_hs
sbl_uart_img_hs_RELPATH = ti/boot/sbl/board/k3
sbl_uart_img_hs_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)_hs/uart/bin
#Note: The below variable is purposefully set to build folder - different than
#non-hs app.
#The APPNAME or app -C directory (_PATH variable) should be unique as the complier
#creates lto_$APPNAME optimization file in the directory in which -C is called
#Because of this multiple lto_ files with same name can be created if app name
#and app path are same
sbl_uart_img_hs_PATH = $(PDK_SBL_COMP_PATH)/build
export sbl_uart_img_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=uart SBL_USE_DMA=yes BUILD_HS=yes
export sbl_uart_img_hs_SBL_CERT_KEY=$(SBL_CERT_KEY_HS)
export sbl_uart_img_hs_BOARD_DEPENDENCY = yes
export sbl_uart_img_hs_SOC_DEPENDENCY = yes
export sbl_uart_img_hs_CORE_DEPENDENCY = no
sbl_uart_img_hs_PKG_LIST = sbl
sbl_uart_img_hs_INCLUDE = $(sbl_uart_img_hs_PATH)
export sbl_uart_img_hs_BOARDLIST = $(sbl_BOARDLIST)
export sbl_uart_img_hs_$(SOC)_CORELIST = mcu1_0
export sbl_uart_img_hs_SBL_IMAGEGEN = yes

# SBL QSPI Image
ifeq ($(SOC), $(filter $(SOC), tpr12 awr294x))
sbl_qspi_img_COMP_LIST = sbl_qspi_img
ifeq ($(SOC), $(filter $(SOC), tpr12))
sbl_qspi_img_RELPATH = ti/boot/sbl/board/evmTPR12
endif
ifeq ($(SOC), $(filter $(SOC), awr294x))
sbl_qspi_img_RELPATH = ti/boot/sbl/board/evmAWR294x
endif
sbl_qspi_img_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)/qspi/bin
ifeq ($(SOC), $(filter $(SOC), tpr12))
sbl_qspi_img_PATH = $(PDK_SBL_COMP_PATH)/board/evmTPR12
endif
ifeq ($(SOC), $(filter $(SOC), awr294x))
sbl_qspi_img_PATH = $(PDK_SBL_COMP_PATH)/board/evmAWR294x
endif
sbl_qspi_img_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=qspi SBL_USE_DMA=yes BUILD_HS=no
export sbl_qspi_img_MAKEFILE
export sbl_qspi_img_SBL_CERT_KEY=$(SBL_CERT_KEY)
sbl_qspi_img_BOARD_DEPENDENCY = yes
sbl_qspi_img_SOC_DEPENDENCY = yes
sbl_qspi_img_CORE_DEPENDENCY = no
export sbl_qspi_img_COMP_LIST
export sbl_qspi_img_BOARD_DEPENDENCY
export sbl_qspi_img_SOC_DEPENDENCY
export sbl_qspi_img_CORE_DEPENDENCY
sbl_qspi_img_PKG_LIST = sbl
sbl_qspi_img_INCLUDE = $(sbl_qspi_img_PATH)
sbl_qspi_img_BOARDLIST = tpr12_evm tpr12_qt awr294x_evm
export sbl_qspi_img_BOARDLIST
sbl_qspi_img_$(SOC)_CORELIST = mcu1_0
export sbl_qspi_img_$(SOC)_CORELIST
sbl_qspi_img_SBL_IMAGEGEN = yes
export sbl_qspi_img_SBL_IMAGEGEN
endif

# Individual Core Boot Test (Ordered)
sbl_boot_test_ordered_COMP_LIST = sbl_boot_test_ordered
sbl_boot_test_ordered_RELPATH = ti/boot/sbl/example/k3MulticoreApp
sbl_boot_test_ordered_BINPATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp/binary
sbl_boot_test_ordered_PATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp
sbl_boot_test_ordered_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_boot_test_ordered.mk
export sbl_boot_test_ordered_MAKEFILE
sbl_boot_test_ordered_BOARD_DEPENDENCY = no
sbl_boot_test_ordered_SOC_DEPENDENCY = no
sbl_boot_test_ordered_CORE_DEPENDENCY = no
export sbl_boot_test_ordered_COMP_LIST
export sbl_boot_test_ordered_BOARD_DEPENDENCY
export sbl_boot_test_ordered_SOC_DEPENDENCY
export sbl_boot_test_ordered_CORE_DEPENDENCY
sbl_boot_test_ordered_PKG_LIST = sbl_boot_test_ordered
sbl_boot_test_ordered_INCLUDE = $(sbl_boot_test_ordered_PATH)
sbl_boot_test_ordered_BOARDLIST = $(sbl_BOARDLIST)
export sbl_boot_test_ordered_BOARDLIST
sbl_boot_test_ordered_$(SOC)_CORELIST = $(sbl_$(SOC)_CORELIST)
export sbl_boot_test_ordered_$(SOC)_CORELIST
ifneq ($(SOC), $(filter $(SOC), tpr12 awr294x))
sbl_EXAMPLE_LIST += sbl_boot_test_ordered
endif
sbl_boot_test_ordered_SBL_APPIMAGEGEN = yes
export sbl_boot_test_ordered_SBL_APPIMAGEGEN

# Multicore AMP Boot Test (Ordered)
sbl_multicore_amp_ordered_COMP_LIST = sbl_multicore_amp_ordered
sbl_multicore_amp_ordered_RELPATH = ti/boot/sbl/example/k3MulticoreApp
sbl_multicore_amp_ordered_BINPATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp/binary
sbl_multicore_amp_ordered_PATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp
sbl_multicore_amp_ordered_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_multicore_amp_ordered.mk
export sbl_multicore_amp_ordered_MAKEFILE
# SBL multicore amp depends on sbl_boot_test for all the cores
sbl_multicore_amp_ordered_DEPENDS_ON=sbl_boot_test_ordered
export sbl_multicore_amp_ordered_DEPENDS_ON
sbl_multicore_amp_ordered_BOARD_DEPENDENCY = no
sbl_multicore_amp_ordered_SOC_DEPENDENCY = no
sbl_multicore_amp_ordered_CORE_DEPENDENCY = no
export sbl_multicore_amp_ordered_COMP_LIST
export sbl_multicore_amp_ordered_BOARD_DEPENDENCY
export sbl_multicore_amp_ordered_SOC_DEPENDENCY
export sbl_multicore_amp_ordered_CORE_DEPENDENCY
sbl_multicore_amp_ordered_PKG_LIST = sbl_multicore_amp_ordered
sbl_multicore_amp_ordered_INCLUDE = $(sbl_multicore_amp_ordered_PATH)
sbl_multicore_amp_ordered_BOARDLIST = $(sbl_BOARDLIST)
export sbl_multicore_amp_ordered_BOARDLIST
sbl_multicore_amp_ordered_$(SOC)_CORELIST = $($(SOC)_LASTCORE)
export sbl_multicore_amp_ordered_$(SOC)_CORELIST
ifneq ($(SOC), $(filter $(SOC), tpr12 awr294x))
sbl_EXAMPLE_LIST += sbl_multicore_amp_ordered
endif
sbl_multicore_amp_ordered_SBL_APPIMAGEGEN = no
export sbl_multicore_amp_ordered_SBL_APPIMAGEGEN

# Individual Core Boot Test (Short)
sbl_boot_test_short_COMP_LIST = sbl_boot_test_short
sbl_boot_test_short_RELPATH = ti/boot/sbl/example/k3MulticoreApp
sbl_boot_test_short_BINPATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp/binary
sbl_boot_test_short_PATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp
sbl_boot_test_short_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_boot_test_short.mk
export sbl_boot_test_short_MAKEFILE
sbl_boot_test_short_BOARD_DEPENDENCY = no
sbl_boot_test_short_SOC_DEPENDENCY = no
sbl_boot_test_short_CORE_DEPENDENCY = no
export sbl_boot_test_short_COMP_LIST
export sbl_boot_test_short_BOARD_DEPENDENCY
export sbl_boot_test_short_SOC_DEPENDENCY
export sbl_boot_test_short_CORE_DEPENDENCY
sbl_boot_test_short_PKG_LIST = sbl_boot_test_short
sbl_boot_test_short_INCLUDE = $(sbl_boot_test_short_PATH)
sbl_boot_test_short_BOARDLIST = $(sbl_BOARDLIST)
export sbl_boot_test_short_BOARDLIST
sbl_boot_test_short_$(SOC)_CORELIST = $(sbl_$(SOC)_CORELIST)
export sbl_boot_test_short_$(SOC)_CORELIST
ifneq ($(SOC), $(filter $(SOC), tpr12 awr294x))
sbl_EXAMPLE_LIST += sbl_boot_test_short
endif
sbl_boot_test_short_SBL_APPIMAGEGEN = yes
export sbl_boot_test_short_SBL_APPIMAGEGEN

# Multicore AMP Boot Test (Short)
sbl_multicore_amp_short_COMP_LIST = sbl_multicore_amp_short
sbl_multicore_amp_short_RELPATH = ti/boot/sbl/example/k3MulticoreApp
sbl_multicore_amp_short_BINPATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp/binary
sbl_multicore_amp_short_PATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp
sbl_multicore_amp_short_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_multicore_amp_short.mk
export sbl_multicore_amp_short_MAKEFILE
# SBL multicore amp depends on sbl_boot_test for all the cores
sbl_multicore_amp_short_DEPENDS_ON=sbl_boot_test_short
export sbl_multicore_amp_short_DEPENDS_ON
sbl_multicore_amp_short_BOARD_DEPENDENCY = no
sbl_multicore_amp_short_SOC_DEPENDENCY = no
sbl_multicore_amp_short_CORE_DEPENDENCY = no
export sbl_multicore_amp_short_COMP_LIST
export sbl_multicore_amp_short_BOARD_DEPENDENCY
export sbl_multicore_amp_short_SOC_DEPENDENCY
export sbl_multicore_amp_short_CORE_DEPENDENCY
sbl_multicore_amp_short_PKG_LIST = sbl_multicore_amp_short
sbl_multicore_amp_short_INCLUDE = $(sbl_multicore_amp_short_PATH)
sbl_multicore_amp_short_BOARDLIST = $(sbl_BOARDLIST)
export sbl_multicore_amp_short_BOARDLIST
sbl_multicore_amp_short_$(SOC)_CORELIST = $($(SOC)_LASTCORE)
export sbl_multicore_amp_short_$(SOC)_CORELIST
ifneq ($(SOC), $(filter $(SOC), tpr12 awr294x))
sbl_EXAMPLE_LIST += sbl_multicore_amp_short
endif
sbl_multicore_amp_short_SBL_APPIMAGEGEN = no
export sbl_multicore_amp_short_SBL_APPIMAGEGEN

# Individual Core Boot Test
sbl_boot_test_COMP_LIST = sbl_boot_test
sbl_boot_test_RELPATH = ti/boot/sbl/example/k3MulticoreApp
sbl_boot_test_BINPATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp/binary
sbl_boot_test_PATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp
sbl_boot_test_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_boot_test.mk
export sbl_boot_test_MAKEFILE
sbl_boot_test_BOARD_DEPENDENCY = no
sbl_boot_test_SOC_DEPENDENCY = no
sbl_boot_test_CORE_DEPENDENCY = no
export sbl_boot_test_COMP_LIST
export sbl_boot_test_BOARD_DEPENDENCY
export sbl_boot_test_SOC_DEPENDENCY
export sbl_boot_test_CORE_DEPENDENCY
sbl_boot_test_PKG_LIST = sbl_boot_test
sbl_boot_test_INCLUDE = $(sbl_boot_test_PATH)
sbl_boot_test_BOARDLIST = $(sbl_BOARDLIST)
export sbl_boot_test_BOARDLIST
sbl_boot_test_$(SOC)_CORELIST = $(sbl_$(SOC)_CORELIST)
export sbl_boot_test_$(SOC)_CORELIST
ifneq ($(SOC), $(filter $(SOC), tpr12 awr294x))
sbl_EXAMPLE_LIST += sbl_boot_test
endif
sbl_boot_test_SBL_APPIMAGEGEN = yes
export sbl_boot_test_SBL_APPIMAGEGEN

# Multicore AMP Boot Test
sbl_multicore_amp_COMP_LIST = sbl_multicore_amp
sbl_multicore_amp_RELPATH = ti/boot/sbl/example/k3MulticoreApp
sbl_multicore_amp_BINPATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp/binary
sbl_multicore_amp_PATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp
sbl_multicore_amp_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_multicore_amp.mk
export sbl_multicore_amp_MAKEFILE
# SBL multicore amp depends on sbl_boot_test for all the cores
sbl_multicore_amp_DEPENDS_ON=sbl_boot_test
export sbl_multicore_amp_DEPENDS_ON
sbl_multicore_amp_BOARD_DEPENDENCY = no
sbl_multicore_amp_SOC_DEPENDENCY = no
sbl_multicore_amp_CORE_DEPENDENCY = no
export sbl_multicore_amp_COMP_LIST
export sbl_multicore_amp_BOARD_DEPENDENCY
export sbl_multicore_amp_SOC_DEPENDENCY
export sbl_multicore_amp_CORE_DEPENDENCY
sbl_multicore_amp_PKG_LIST = sbl_multicore_amp
sbl_multicore_amp_INCLUDE = $(sbl_multicore_amp_PATH)
sbl_multicore_amp_BOARDLIST = $(sbl_BOARDLIST)
export sbl_multicore_amp_BOARDLIST
sbl_multicore_amp_$(SOC)_CORELIST = $($(SOC)_LASTCORE)
export sbl_multicore_amp_$(SOC)_CORELIST
ifneq ($(SOC), $(filter $(SOC), tpr12 awr294x))
sbl_EXAMPLE_LIST += sbl_multicore_amp
endif
sbl_multicore_amp_SBL_APPIMAGEGEN = no
export sbl_multicore_amp_SBL_APPIMAGEGEN

# R5 Lockstep and MPU SMP Boot Test
sbl_smp_test_COMP_LIST = sbl_smp_test
sbl_smp_test_RELPATH = ti/boot/sbl/example/k3MulticoreApp
sbl_smp_test_BINPATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp/binary
sbl_smp_test_PATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp
sbl_smp_test_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_smp_test.mk BOOTMODE=mmcsd
export sbl_smp_test_MAKEFILE
sbl_smp_test_BOARD_DEPENDENCY = no
sbl_smp_test_SOC_DEPENDENCY = no
sbl_smp_test_CORE_DEPENDENCY = no
export sbl_smp_test_COMP_LIST
export sbl_smp_test_BOARD_DEPENDENCY
export sbl_smp_test_SOC_DEPENDENCY
export sbl_smp_test_CORE_DEPENDENCY
sbl_smp_test_PKG_LIST = sbl_smp_test
sbl_smp_test_INCLUDE = $(sbl_smp_test_PATH)
sbl_smp_test_BOARDLIST = $(sbl_BOARDLIST)
export sbl_smp_test_BOARDLIST
sbl_smp_test_$(SOC)_CORELIST = $($(SOC)_smp_CORELIST)
export sbl_smp_test_$(SOC)_CORELIST
ifneq ($(SOC),$(filter $(SOC), am64x tpr12 awr294x))
sbl_EXAMPLE_LIST += sbl_smp_test
endif
sbl_smp_test_SBL_APPIMAGEGEN = yes
export sbl_smp_test_SBL_APPIMAGEGEN

# Multicore SMP Boot Test
sbl_multicore_smp_COMP_LIST = sbl_multicore_smp
sbl_multicore_smp_RELPATH = ti/boot/sbl/example/k3MulticoreApp
sbl_multicore_smp_BINPATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp/binary
sbl_multicore_smp_PATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp
sbl_multicore_smp_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_multicore_smp.mk
export sbl_multicore_smp_MAKEFILE
# SBL multicore smp depends on sbl_smp_test for all the cores
sbl_multicore_smp_DEPENDS_ON=sbl_smp_test
export sbl_multicore_smp_DEPENDS_ON
sbl_multicore_smp_BOARD_DEPENDENCY = no
sbl_multicore_smp_SOC_DEPENDENCY = no
sbl_multicore_smp_CORE_DEPENDENCY = no
export sbl_multicore_smp_COMP_LIST
export sbl_multicore_smp_BOARD_DEPENDENCY
export sbl_multicore_smp_SOC_DEPENDENCY
export sbl_multicore_smp_CORE_DEPENDENCY
sbl_multicore_smp_PKG_LIST = sbl_multicore_smp
sbl_multicore_smp_INCLUDE = $(sbl_multicore_smp_PATH)
sbl_multicore_smp_BOARDLIST = $(sbl_BOARDLIST)
export sbl_multicore_smp_BOARDLIST
sbl_multicore_smp_$(SOC)_CORELIST := $($(SOC)_LASTCORE)
export sbl_multicore_smp_$(SOC)_CORELIST
ifneq ($(SOC),$(filter $(SOC), am64x tpr12 awr294x))
sbl_EXAMPLE_LIST += sbl_multicore_smp
endif
sbl_multicore_smp_SBL_APPIMAGEGEN = no
export sbl_multicore_smp_SBL_APPIMAGEGEN

# R5 boot XIP Test
sbl_boot_xip_test_COMP_LIST = sbl_boot_xip_test
sbl_boot_xip_test_RELPATH = ti/boot/sbl/example/k3MulticoreApp
sbl_boot_xip_test_BINPATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp/binary
sbl_boot_xip_test_PATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp
sbl_boot_xip_test_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_mcu0_boot_xip_test.mk
export sbl_boot_xip_test_MAKEFILE
sbl_boot_xip_test_BOARD_DEPENDENCY = no
sbl_boot_xip_test_SOC_DEPENDENCY = no
sbl_boot_xip_test_CORE_DEPENDENCY = no
export sbl_boot_xip_test_COMP_LIST
export sbl_boot_xip_test_BOARD_DEPENDENCY
export sbl_boot_xip_test_SOC_DEPENDENCY
export sbl_boot_xip_test_CORE_DEPENDENCY
sbl_boot_xip_test_PKG_LIST = sbl_boot_xip_test
sbl_boot_xip_test_INCLUDE = $(sbl_boot_xip_test_PATH)
sbl_boot_xip_test_BOARDLIST = $(sbl_BOARDLIST)
export sbl_boot_xip_test_BOARDLIST
sbl_boot_xip_test_$(SOC)_CORELIST = mcu1_0
export sbl_boot_xip_test_$(SOC)_CORELIST
ifneq ($(SOC),$(filter $(SOC), tpr12 awr294x))
sbl_EXAMPLE_LIST += sbl_boot_xip_test
endif
sbl_boot_xip_test_SBL_APPIMAGEGEN = yes
sbl_boot_xip_test_SBL_APP_BINIMAGEGEN = yes
sbl_boot_xip_test_SBL_APP_BIN_SECTIONS = --only-section .rstvectors --only-section .sbl_mcu_1_0_resetvector
export sbl_boot_xip_test_SBL_APPIMAGEGEN
export sbl_boot_xip_test_SBL_APP_BINIMAGEGEN
export sbl_boot_xip_test_SBL_APP_BIN_SECTIONS

# R5 boot XIP entry
sbl_boot_xip_entry_COMP_LIST = sbl_boot_xip_entry
sbl_boot_xip_entry_RELPATH = ti/boot/sbl/example/k3MulticoreApp
sbl_boot_xip_entry_BINPATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp/binary
sbl_boot_xip_entry_PATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp
sbl_boot_xip_entry_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_mcu0_boot_xip_entry.mk
export sbl_boot_xip_entry_MAKEFILE
sbl_boot_xip_entry_BOARD_DEPENDENCY = no
sbl_boot_xip_entry_SOC_DEPENDENCY = no
sbl_boot_xip_entry_CORE_DEPENDENCY = no
export sbl_boot_xip_entry_COMP_LIST
export sbl_boot_xip_entry_BOARD_DEPENDENCY
export sbl_boot_xip_entry_SOC_DEPENDENCY
export sbl_boot_xip_entry_CORE_DEPENDENCY
sbl_boot_xip_entry_PKG_LIST = sbl_boot_xip_entry
sbl_boot_xip_entry_INCLUDE = $(sbl_boot_xip_entry_PATH)
sbl_boot_xip_entry_BOARDLIST = $(sbl_BOARDLIST)
export sbl_boot_xip_entry_BOARDLIST
sbl_boot_xip_entry_$(SOC)_CORELIST = mcu1_0
export sbl_boot_xip_entry_$(SOC)_CORELIST
ifneq ($(SOC),$(filter $(SOC), tpr12 awr294x))
sbl_EXAMPLE_LIST += sbl_boot_xip_entry
endif
sbl_boot_xip_entry_SBL_APPIMAGEGEN = yes
export sbl_boot_xip_entry_SBL_APPIMAGEGEN

# Multicore XIP entry
sbl_boot_multicore_xip_entry_COMP_LIST = sbl_boot_multicore_xip_entry
sbl_boot_multicore_xip_entry_RELPATH = ti/boot/sbl/example/sblMcXipEntryApp
sbl_boot_multicore_xip_entry_BINPATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp/binary
sbl_boot_multicore_xip_entry_PATH = $(PDK_SBL_COMP_PATH)/example/sblMcXipEntryApp
sbl_boot_multicore_xip_entry_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/example/sblMcXipEntryApp/makefile.mk
export sbl_boot_multicore_xip_entry_MAKEFILE
sbl_boot_multicore_xip_entry_BOARD_DEPENDENCY = no
sbl_boot_multicore_xip_entry_SOC_DEPENDENCY = no
sbl_boot_multicore_xip_entry_CORE_DEPENDENCY = no
export sbl_boot_multicore_xip_entry_COMP_LIST
export sbl_boot_multicore_xip_entry_BOARD_DEPENDENCY
export sbl_boot_multicore_xip_entry_SOC_DEPENDENCY
export sbl_boot_multicore_xip_entry_CORE_DEPENDENCY
sbl_boot_multicore_xip_entry_PKG_LIST = sbl_boot_multicore_xip_entry
sbl_boot_multicore_xip_entry_INCLUDE = $(sbl_boot_multicore_xip_entry_PATH)
sbl_boot_multicore_xip_entry_BOARDLIST = j721e_evm j7200_evm
export sbl_boot_multicore_xip_entry_BOARDLIST
sbl_boot_multicore_xip_entry_$(SOC)_CORELIST = mcu1_0 mcu2_0
export sbl_boot_multicore_xip_entry_$(SOC)_CORELIST
ifneq ($(SOC),$(filter $(SOC), tpr12 awr294x))
sbl_EXAMPLE_LIST += sbl_boot_multicore_xip_entry
endif
sbl_boot_multicore_xip_entry_SBL_APPIMAGEGEN = yes
export sbl_boot_multicore_xip_entry_SBL_APPIMAGEGEN

# Display profiling info before MCU1_0 boot
# increases delay time, between end of SBL
# and start of app, but useful for
# debugging and tuning performace knobs
#SBL_CFLAGS += -DSBL_DISPLAY_PROFILE_INFO

###### Use boot_perf_benchmark example#######
###### to fine tune the perf knobs  #########

###########START BOOT PERF KNOBS#############
# SBL log level
# no logs = 0, only errors =1, normal logs = 2, all logs = 3

ifeq ($(SOC), $(filter $(SOC), tpr12 awr294x))
SBL_CFLAGS += -DSBL_LOG_LEVEL=0
else
SBL_CFLAGS += -DSBL_LOG_LEVEL=2
endif

SBL_CFLAGS += -DSBL_ENABLE_PLL
SBL_CFLAGS += -DSBL_ENABLE_CLOCKS
SBL_CFLAGS += -DSBL_ENABLE_DDR

############################################
# DISABLING the options above this caption
# improves boot time at the cost of moving
# PLL, LPSC and DDR init to the app
#
# ENABLING the options below this caption
# improves boot time by skipping stuff SBL
# usually does.
###########################################
# If enabled, the SBL will branch to the start
# of MCU_0 app without resetting the core
# if csl defaults are godd enough this enables
# app to skip redoing mcu initialization
#SBL_CFLAGS += -DSBL_SKIP_MCU_RESET

# If enabled, SBL will skip initializing
# sysfw. The SBL will only load it.
# No SCI Client APIs will work. Saves
# boot time. When this is enabled
# make sure that SBL_SKIP_MCU_RESET is also
# enabled, as resetting a core needs SYSFW
# to be running.
# SBL_CFLAGS += -DSBL_SKIP_SYSFW_INIT

# If enabled, SBL will skip calling
# Sciclient_boardCfg* API.  Enabling
# it saves boot time, can affect
# functionality. The app must call the
# Sciclient_boardCfg* APIs that the SBL
# skips. Like for eg, if SBL skips calling
# Sciclient_boardCfgPm, then Sciclient_boardCfgRm
# and Sciclient_boardCfgSec must also
# be skipped.
#SBL_CFLAGS += -DSBL_SKIP_BRD_CFG_BOARD
#SBL_CFLAGS += -DSBL_SKIP_BRD_CFG_RM
#SBL_CFLAGS += -DSBL_SKIP_BRD_CFG_SEC
#SBL_CFLAGS += -DSBL_SKIP_BRD_CFG_PM

# If enabled, SBL will only enable the Clocks
# and PLLs for the MCU domain. Clocks and PLLs
# for the Main domaincan be initialized by the
# app to save boot time or if the application
# usecase demands it.
#SBL_CFLAGS += -DSBL_ENABLE_DEV_GRP_MCU

# If enabled, SBL will use alternate
# config steps to directly boot an HLOS,
# including an alternate set of Sciclient
# RM board cfg settings.
# NOTE: Currently supported only for J721E, J7200
#SBL_CFLAGS += -DSBL_ENABLE_HLOS_BOOT

# If enabled, SBL will not leave the OSPI
# flash in XIP mode, so that the MAIN
# domain Cortex-A code (HLOS) can reset
# the flash and take control of it, if
# needed.
#SBL_CFLAGS += -DSBL_HLOS_OWNS_FLASH

# If enabled, SBL will skip any PINMUX
# init being used in the bootloader and
# expect that it will be handled by
# another app.
#SBL_CFLAGS += -DSBL_SKIP_PINMUX_ENABLE

# If enabled, SBL will skip handling the
# SoC Late Init functions normally contained
# in SBL_SocLateInit()
#SBL_CFLAGS += -DSBL_SKIP_LATE_INIT

# If enabled, the SBL will configure & use
# resources in the MCU domain ONLY to
# enable MCU-only code to run.
#SBL_CFLAGS += -DSBL_USE_MCU_DOMAIN_ONLY

# If enabled for OSPI boot the SBL will bypass the
# OSPI driver completely and let ROM copy the sysftw/application
# from flash.
#SBL_CFLAGS += -DSBL_BYPASS_OSPI_DRIVER

# If enabled for OSPI boot the SBL will bypass the
# OSPI driver for system firmware download and use the OSPI driver for
# downloading application from the flash
#SBL_CFLAGS += -DSBL_BYPASS_OSPI_DRIVER_FOR_SYSFW_DOWNLOAD

# If enabled the SBL image is built for running on VLAB simulation.
#SBL_CFLAGS += -DVLAB_SIM

###########END BOOT PERF KNOBS#############

# Example - Building Custom SBL Images
# Build and SBl with custom flags to change
# different build configurations
CUST_SBL_TEST_SOCS = am65xx j721e j7200 am64x
CUST_SBL_TEST_BOARDS = am65xx_evm j721e_evm j7200_evm am64x_evm
#CUST_SBL_TEST_FLAGS =" -DSBL_USE_DMA=1 -DSBL_LOG_LEVEL=0 -DSBL_SCRATCH_MEM_START=0x70100000 -DSBL_SCRATCH_MEM_SIZE=0xF0000 -DSBL_SKIP_MCU_RESET  -DBOOT_OSPI "
#CUST_SBL_TEST_FLAGS =" -DSBL_USE_DMA=1 -DSBL_LOG_LEVEL=0 -DSBL_SCRATCH_MEM_START=0x70100000 -DSBL_SKIP_MCU_RESET -DSBL_SKIP_BRD_CFG_PM -DBOOT_OSPI "
#CUST_SBL_TEST_FLAGS =" -DSBL_USE_DMA=0 -DSBL_LOG_LEVEL=0 -DSBL_SCRATCH_MEM_START=0x70100000 -DSBL_SCRATCH_MEM_SIZE=0xF0000 -DSBL_SKIP_SYSFW_INIT -DSBL_SKIP_MCU_RESET -DBOOT_OSPI"
#CUST_SBL_TEST_FLAGS =" -DSBL_USE_DMA=1 -DSBL_LOG_LEVEL=1 -DSBL_SCRATCH_MEM_START=0xB8000000 -DSBL_SCRATCH_MEM_SIZE=0x4000000 -DSBL_ENABLE_PLL -DSBL_ENABLE_CLOCKS -DSBL_ENABLE_DDR -DSBL_SKIP_MCU_RESET -DBOOT_OSPI"
ifeq ($(SOC),$(filter $(SOC), j7200 am64x))
# NOTE: If changing to SBL_USE_DMA=1, below, then also change 'sbl_lib_cust' & 'sbl_cust_img' MAKEFILE lines further below to use 'SBL_USE_DMA=yes'
CUST_SBL_TEST_FLAGS =" -DSBL_USE_DMA=0 -DSBL_LOG_LEVEL=1 -DSBL_SCRATCH_MEM_START=0xB8000000 -DSBL_SCRATCH_MEM_SIZE=0x4000000 -DSBL_ENABLE_PLL -DSBL_ENABLE_CLOCKS -DSBL_ENABLE_DDR -DSBL_SKIP_MCU_RESET -DBOOT_OSPI"
else
  ifeq ($(findstring j7,$(SOC)),j7)
# NOTE: If changing to SBL_USE_DMA=1, below, then also change 'sbl_lib_cust' & 'sbl_cust_img' MAKEFILE lines further below to use 'SBL_USE_DMA=yes'
CUST_SBL_TEST_FLAGS =" -DSBL_USE_DMA=0 -DSBL_LOG_LEVEL=1 -DSBL_SCRATCH_MEM_START=0xB8000000 -DSBL_SCRATCH_MEM_SIZE=0x4000000 -DSBL_ENABLE_PLL -DSBL_ENABLE_CLOCKS -DSBL_ENABLE_DDR -DSBL_SKIP_MCU_RESET -DBOOT_OSPI"
# Custom ex: Early CAN response (NOTE: before using line below, comment out the line above)
#CUST_SBL_TEST_FLAGS =" -DSBL_USE_DMA=0 -DSBL_LOG_LEVEL=1 -DSBL_SCRATCH_MEM_START=0x41cc0000 -DSBL_SCRATCH_MEM_SIZE=0x40000 -DSBL_ENABLE_PLL -DSBL_ENABLE_CLOCKS -DSBL_SKIP_MCU_RESET -DBOOT_OSPI -DSBL_ENABLE_DEV_GRP_MCU -DSBL_HLOS_OWNS_FLASH -DSBL_SKIP_PINMUX_ENABLE -DSBL_SKIP_LATE_INIT -DSBL_USE_MCU_DOMAIN_ONLY"
  else
CUST_SBL_TEST_FLAGS =" -DSBL_USE_DMA=0 -DSBL_LOG_LEVEL=1 -DSBL_SCRATCH_MEM_START=0x70100000 -DSBL_SCRATCH_MEM_SIZE=0xF0000 -DSBL_ENABLE_PLL -DSBL_ENABLE_CLOCKS -DSBL_SKIP_MCU_RESET -DBOOT_OSPI"
  endif
endif

# SBL Custom LIB
sbl_lib_cust_COMP_LIST = sbl_lib_cust
sbl_lib_cust_RELPATH = ti/boot/sbl
export sbl_lib_cust_OBJPATH = ti/boot/sbl/cust
sbl_lib_cust_LIBNAME = sbl_lib_cust
sbl_lib_cust_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_cust_LIBNAME = sbl_lib_cust
sbl_lib_cust_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/cust
sbl_lib_cust_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=cust SBL_USE_DMA=no CUST_SBL_FLAGS=$(CUST_SBL_TEST_FLAGS)
export sbl_lib_cust_MAKEFILE
export sbl_lib_cust_LIBNAME
export sbl_lib_cust_LIBPATH
sbl_lib_cust_BOARD_DEPENDENCY = yes
sbl_lib_cust_SOC_DEPENDENCY = yes
sbl_lib_cust_CORE_DEPENDENCY = no
export sbl_lib_cust_COMP_LIST
export sbl_lib_cust_BOARD_DEPENDENCY
export sbl_lib_cust_CORE_DEPENDENCY
sbl_lib_cust_PKG_LIST = sbl_lib_cust
sbl_lib_cust_INCLUDE = $(sbl_lib_cust_PATH)
sbl_lib_cust_SOCLIST = $(CUST_SBL_TEST_SOCS)
sbl_lib_cust_BOARDLIST = $(CUST_SBL_TEST_BOARDS)
export sbl_lib_cust_SOCLIST
export sbl_lib_cust_BOARDLIST
sbl_lib_cust_$(SOC)_CORELIST = mcu1_0
export sbl_lib_cust_$(SOC)_CORELIST

# SBL Custom LIB - HS build variant
sbl_lib_cust_hs_COMP_LIST = sbl_lib_cust_hs
sbl_lib_cust_hs_RELPATH = ti/boot/sbl
export sbl_lib_cust_hs_OBJPATH = ti/boot/sbl/cust_hs
sbl_lib_cust_hs_LIBNAME = sbl_lib_cust_hs
sbl_lib_cust_hs_PATH = $(PDK_SBL_COMP_PATH)
sbl_lib_cust_hs_LIBNAME = sbl_lib_cust_hs
sbl_lib_cust_hs_LIBPATH = $(PDK_SBL_COMP_PATH)/lib/cust_hs
sbl_lib_cust_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_lib.mk BOOTMODE=cust SBL_USE_DMA=no CUST_SBL_FLAGS=$(CUST_SBL_TEST_FLAGS) BUILD_HS=yes
export sbl_lib_cust_hs_MAKEFILE
export sbl_lib_cust_hs_LIBNAME
export sbl_lib_cust_hs_LIBPATH
sbl_lib_cust_hs_BOARD_DEPENDENCY = yes
sbl_lib_cust_hs_SOC_DEPENDENCY = yes
sbl_lib_cust_hs_CORE_DEPENDENCY = no
export sbl_lib_cust_hs_COMP_LIST
export sbl_lib_cust_hs_BOARD_DEPENDENCY
export sbl_lib_cust_hs_CORE_DEPENDENCY
sbl_lib_cust_hs_PKG_LIST = sbl_lib_cust_hs
sbl_lib_cust_hs_INCLUDE = $(sbl_lib_cust_hs_PATH)
sbl_lib_cust_hs_SOCLIST = $(CUST_SBL_TEST_SOCS)
sbl_lib_cust_hs_BOARDLIST = $(CUST_SBL_TEST_BOARDS)
export sbl_lib_cust_hs_SOCLIST
export sbl_lib_cust_hs_BOARDLIST
sbl_lib_cust_hs_$(SOC)_CORELIST = mcu1_0
export sbl_lib_cust_hs_$(SOC)_CORELIST

# SBL custom image
sbl_cust_img_COMP_LIST = sbl_cust_img
sbl_cust_img_RELPATH = ti/boot/sbl/board/k3
sbl_cust_img_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)/cust/bin
sbl_cust_img_PATH = $(PDK_SBL_COMP_PATH)/board/k3
sbl_cust_img_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk BOOTMODE=cust SBL_USE_DMA=no BUILD_HS=no CUST_SBL_FLAGS=$(CUST_SBL_TEST_FLAGS)
export sbl_cust_img_MAKEFILE
export sbl_cust_img_SBL_CERT_KEY=$(SBL_CERT_KEY)
sbl_cust_img_BOARD_DEPENDENCY = yes
sbl_cust_img_SOC_DEPENDENCY = yes
sbl_cust_img_CORE_DEPENDENCY = no
export sbl_cust_img_COMP_LIST
export sbl_cust_img_BOARD_DEPENDENCY
export sbl_cust_img_SOC_DEPENDENCY
export sbl_cust_img_CORE_DEPENDENCY
sbl_cust_img_PKG_LIST = sbl
sbl_cust_img_INCLUDE = $(sbl_cust_img_PATH)
sbl_cust_img_SOCLIST = $(CUST_SBL_TEST_SOCS)
sbl_cust_img_BOARDLIST = $(CUST_SBL_TEST_BOARDS)
export sbl_cust_img_SOCLIST
export sbl_cust_img_BOARDLIST
sbl_cust_img_$(SOC)_CORELIST = mcu1_0
export sbl_cust_img_$(SOC)_CORELIST
ifneq ($(SOC),$(filter $(SOC), tpr12 awr294x))
ifneq ($(BOARD),$(filter $(BOARD), am64x_svb))
sbl_EXAMPLE_LIST += sbl_cust_img
endif
endif
sbl_cust_img_SBL_IMAGEGEN = yes
export sbl_cust_img_SBL_IMAGEGEN

# SBL custom image - For HS build
export sbl_cust_img_hs_COMP_LIST = sbl_cust_img_hs
sbl_cust_img_hs_RELPATH = ti/boot/sbl/board/k3
sbl_cust_img_hs_CUSTOM_BINPATH = $(PDK_SBL_COMP_PATH)/binary/$(BOARD)_hs/cust/bin
#Note: The below variable is purposefully set to build folder - different than
#non-hs app.
#The APPNAME or app -C directory (_PATH variable) should be unique as the complier
#creates lto_$APPNAME optimization file in the directory in which -C is called
#Because of this multiple lto_ files with same name can be created if app name
#and app path are same
sbl_cust_img_hs_PATH = $(PDK_SBL_COMP_PATH)/build
export sbl_cust_img_hs_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_img.mk CUST_SBL_FLAGS=$(CUST_SBL_TEST_FLAGS) BOOTMODE=cust SBL_USE_DMA=no BUILD_HS=yes
export sbl_cust_img_hs_SBL_CERT_KEY=$(SBL_CERT_KEY_HS)
export sbl_cust_img_hs_BOARD_DEPENDENCY = yes
export sbl_cust_img_hs_SOC_DEPENDENCY = yes
export sbl_cust_img_hs_CORE_DEPENDENCY = no
sbl_cust_img_hs_PKG_LIST = sbl
sbl_cust_img_hs_INCLUDE = $(sbl_cust_img_hs_PATH)
export sbl_cust_img_hs_SOCLIST = $(CUST_SBL_TEST_SOCS)
export sbl_cust_img_hs_BOARDLIST = $(CUST_SBL_TEST_BOARDS)
export sbl_cust_img_hs_$(SOC)_CORELIST = mcu1_0
export sbl_cust_img_hs_SBL_IMAGEGEN = yes
ifneq ($(SOC),$(filter $(SOC), am64x tpr12 awr294x))
sbl_EXAMPLE_LIST += sbl_cust_img_hs
endif

# R5 boot performance Test - works only with custom SBL
sbl_boot_perf_test_COMP_LIST = sbl_boot_perf_test
sbl_boot_perf_test_RELPATH = ti/boot/sbl/example/k3MulticoreApp
sbl_boot_perf_test_BINPATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp/binary
sbl_boot_perf_test_PATH = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp
sbl_boot_perf_test_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_mcu0_boot_perf_test.mk BOOTMODE=cust CUST_SBL_FLAGS=$(CUST_SBL_TEST_FLAGS)
export sbl_boot_perf_test_MAKEFILE
sbl_boot_perf_test_BOARD_DEPENDENCY = no
sbl_boot_perf_test_SOC_DEPENDENCY = no
sbl_boot_perf_test_CORE_DEPENDENCY = no
export sbl_boot_perf_test_COMP_LIST
export sbl_boot_perf_test_BOARD_DEPENDENCY
export sbl_boot_perf_test_SOC_DEPENDENCY
export sbl_boot_perf_test_CORE_DEPENDENCY
sbl_boot_perf_test_PKG_LIST = sbl_boot_perf_test
sbl_boot_perf_test_INCLUDE = $(sbl_boot_perf_test_PATH)
sbl_boot_perf_test_SOCLIST = $(CUST_SBL_TEST_SOCS)
sbl_boot_perf_test_BOARDLIST = $(CUST_SBL_TEST_BOARDS)
export sbl_boot_perf_test_SOCLIST
export sbl_boot_perf_test_BOARDLIST
sbl_boot_perf_test_$(SOC)_CORELIST = mcu1_0
export sbl_boot_perf_test_$(SOC)_CORELIST
ifneq ($(SOC),$(filter $(SOC), am64x tpr12 awr294x))
sbl_EXAMPLE_LIST += sbl_boot_perf_test
endif
sbl_boot_perf_test_SBL_APPIMAGEGEN = yes
export sbl_boot_perf_test_SBL_APPIMAGEGEN

# TPR12 QT Test Bench image
ifeq ($(SOC), $(filter $(SOC), tpr12 awr294x))
sbl_r4tb_COMP_LIST = sbl_r4tb
sbl_r4tb_RELPATH = ti/boot/sbl/example/tpr12MulticoreApp/r4_tb
sbl_r4tb_BINPATH = $(PDK_SBL_COMP_PATH)/example/tpr12MulticoreApp/r4_tb/obj
sbl_r4tb_PATH = $(PDK_SBL_COMP_PATH)/example/tpr12MulticoreApp/r4_tb
sbl_r4tb_MAKEFILE = -f$(sbl_r4tb_PATH)/makefile all R4_TB_PATH:=${sbl_r4tb_PATH} R4_CODEGEN_INSTALL_PATH:=${TOOLCHAIN_PATH_R5}
export sbl_r4tb_MAKEFILE
sbl_r4tb_BOARD_DEPENDENCY = no
sbl_r4tb_SOC_DEPENDENCY = no
sbl_r4tb_CORE_DEPENDENCY = no
export sbl_r4tb_COMP_LIST
export sbl_r4tb_BOARD_DEPENDENCY
export sbl_r4tb_SOC_DEPENDENCY
export sbl_r4tb_CORE_DEPENDENCY
sbl_r4tb_PKG_LIST = sbl_r4tb
sbl_r4tb_INCLUDE = $(sbl_r4tb_PATH)
sbl_r4tb_BOARDLIST = tpr12_evm tpr12_qt awr294x_evm
export sbl_r4tb_BOARDLIST
sbl_r4tb_$(SOC)_CORELIST = $(sbl_$(SOC)_CORELIST)
export sbl_r4tb_$(SOC)_CORELIST
sbl_r4tb_SBL_APPIMAGEGEN = no
export sbl_r4tb_SBL_APPIMAGEGEN
endif

ifeq ($(SOC), $(filter $(SOC), tpr12 awr294x))
# R5 Lockstep and DSP Boot Test
sbl_tpr12_test_COMP_LIST = sbl_tpr12_test
sbl_tpr12_test_RELPATH = ti/boot/sbl/example/tpr12MulticoreApp
sbl_tpr12_test_BINPATH = $(PDK_SBL_COMP_PATH)/example/tpr12MulticoreApp/binary
sbl_tpr12_test_PATH = $(PDK_SBL_COMP_PATH)/example/tpr12MulticoreApp
sbl_tpr12_test_MAKEFILE = -f$(PDK_SBL_COMP_PATH)/build/sbl_tpr12_test.mk BOOTMODE=uart
export sbl_tpr12_test_MAKEFILE
sbl_tpr12_test_BOARD_DEPENDENCY = no
sbl_tpr12_test_SOC_DEPENDENCY = no
sbl_tpr12_test_CORE_DEPENDENCY = no
export sbl_tpr12_test_COMP_LIST
export sbl_tpr12_test_BOARD_DEPENDENCY
export sbl_tpr12_test_SOC_DEPENDENCY
export sbl_tpr12_test_CORE_DEPENDENCY
sbl_tpr12_test_PKG_LIST = sbl_tpr12_test
sbl_tpr12_test_INCLUDE = $(sbl_tpr12_test_PATH)
sbl_tpr12_test_BOARDLIST = $(sbl_BOARDLIST)
export sbl_tpr12_test_BOARDLIST
sbl_tpr12_test_$(SOC)_CORELIST = $(sbl_tpr12_CORELIST)
export sbl_tpr12_test_$(SOC)_CORELIST
sbl_EXAMPLE_LIST += sbl_tpr12_test
sbl_tpr12_test_SBL_APPIMAGEGEN = yes
export sbl_tpr12_test_SBL_APPIMAGEGEN
endif

# SBL not supported for any profile
# other than release
ifneq ($(BUILD_PROFILE), release)
ifneq ($(SOC), $(filter $(SOC), tpr12 awr294x))
sbl_LIB_LIST =
sbl_EXAMPLE_LIST =
SBL_CFLAGS =
endif # TPR12 Debug SBL build support
endif # ifneq ($(BUILD_PROFILE), release)

export sbl_LIB_LIST
export sbl_EXAMPLE_LIST
export SBL_CFLAGS

sbl_component_make_include := 1
endif
