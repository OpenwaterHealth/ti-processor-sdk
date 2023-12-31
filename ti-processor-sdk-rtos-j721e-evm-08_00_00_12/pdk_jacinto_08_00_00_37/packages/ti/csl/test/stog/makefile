#
# This file is the makefile for building CSL STOG Test App.
#
#   Copyright (c) Texas Instruments Incorporated 2020
#
include $(PDK_INSTALL_PATH)/ti/build/Rules.make

APP_NAME = csl_stog_test_app
BUILD_OS_TYPE=baremetal
SRCDIR = .
INCDIR = .

SRCDIR += $(SOC)/
INCDIR += $(SOC)/

SRCDIR += $(PDK_INSTALL_PATH)/ti/build/unit-test/Unity/src $(PDK_INSTALL_PATH)/ti/build/unit-test/config
INCDIR += $(PDK_INSTALL_PATH)/ti/build/unit-test/Unity/src $(PDK_INSTALL_PATH)/ti/build/unit-test/config

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk

# List all the components required by the application
ifeq ($(CSL_BUILD),$(filter $(CSL_BUILD), CSL))
  COMP_LIST_COMMON = $(PDK_COMMON_BAREMETAL_COMP)
else
  COMP_LIST_COMMON = $(PDK_COMMON_BAREMETAL_COMP)
  COMP_LIST_COMMON := $(filter-out csl,$(COMP_LIST_COMMON))
  COMP_LIST_COMMON += csl2 csl_trim
endif

# Common source files and CFLAGS across all platforms and cores
PACKAGE_SRCS_COMMON = .
CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS) -DUNITY_INCLUDE_CONFIG_H

SRCS_COMMON += stog_test_main.c stog_test_func.c stog_test_api.c stog_test_err.c
SRCS_COMMON += stog_test_cfg.c
SRCS_ASM_COMMON += stog_test_utils.asm
SRCS_COMMON += esm_app.c
SRCS_COMMON += unity.c unity_config.c

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =
ifeq ($(SOC),$(filter $(SOC), j721e))
  EXTERNAL_LNKCMD_FILE_LOCAL = $(PDK_INSTALL_PATH)/ti/csl/test/stog/$(SOC)/linker_r5.lds
endif

# Include common make files
ifeq ($(MAKERULEDIR), )
#Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(ROOTDIR)/ti/build/makerules
  export MAKERULEDIR
endif
include $(MAKERULEDIR)/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
