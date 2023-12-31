#
# This file is the makefile for building CSL uart app.
#
include $(PDK_INSTALL_PATH)/ti/build/Rules.make

APP_NAME = csl_uart_test_app
BUILD_OS_TYPE=baremetal

SRCDIR = .
INCDIR =

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk

# List all the components required by the application
COMP_LIST_COMMON += csl_utils_common csl_uart_console
ifeq ($(BOARD),$(filter $(BOARD), idkAM572x evmAM572x idkAM571x idkAM574x))
  COMP_LIST_COMMON += $(PDK_COMMON_BAREMETAL_COMP)
else
  ifeq ($(SOC),$(filter $(SOC), am65xx j721e j7200 am64x))
    COMP_LIST_COMMON += $(PDK_COMMON_BAREMETAL_COMP)
  else
    COMP_LIST_COMMON += csl
    COMP_LIST_a15_0 = csl_init
  endif
endif

# Common source files and CFLAGS across all platforms and cores
PACKAGE_SRCS_COMMON = .
SRCS_COMMON += main.c

CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS)

ifeq ($(SOC),$(filter $(SOC), am65xx am64x j721e j7200))
 SRCDIR += $(PDK_INSTALL_PATH)/ti/build/unit-test/Unity/src
 SRCDIR += $(PDK_INSTALL_PATH)/ti/build/unit-test/config
 INCDIR += $(PDK_INSTALL_PATH)/ti/build/unit-test/Unity/src
 INCDIR += $(PDK_INSTALL_PATH)/ti/build/unit-test/config
 SRCS_COMMON += unity_config.c unity.c
 CFLAGS_LOCAL_COMMON += -DUNITY_INCLUDE_CONFIG_H
endif

ifeq ($(CORE),a15_0)
 LNKCMD_FILE = $(PDK_CSL_COMP_PATH)/example/lnk_a15.cmd
endif
ifeq ($(CORE),ipu1_0)
 LNKCMD_FILE = $(PDK_CSL_COMP_PATH)/example/lnk_m4.cmd
endif
ifeq ($(CORE),c66x)
 LNKCMD_FILE = $(PDK_CSL_COMP_PATH)/example/lnk_dsp.cmd
endif

LNKFLAGS_LOCAL_a15_0 += --entry Entry
LNKFLAGS_LOCAL_mpu1_0 += --entry Entry

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

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
