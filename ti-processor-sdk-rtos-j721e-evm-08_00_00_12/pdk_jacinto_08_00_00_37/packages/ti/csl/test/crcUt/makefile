#
# This file is the makefile for building CRC unit test application.
#
include $(PDK_INSTALL_PATH)/ti/build/Rules.make

APP_NAME = csl_crc_unit_testapp
BUILD_OS_TYPE = baremetal

SRCDIR = testApp testLib
INCDIR = testInput testLib

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk
INCLUDE_INTERNAL_INTERFACES =

# List all the components required by the application
COMP_LIST_COMMON = $(PDK_COMMON_BAREMETAL_COMP) udma_apputils

ifneq ($(CSL_BUILD),$(filter $(CSL_BUILD), CSL))
  COMP_LIST_COMMON := $(filter-out csl,$(COMP_LIST_COMMON))
  COMP_LIST_COMMON += csl2 csl_trim
endif

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = st_crcApp.c st_crcParser.c st_crcCommon.c st_crcFullCPU.c st_crcSemiCPU.c st_crcData.c st_crcCodeCover.c

CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS)
PACKAGE_SRCS_COMMON = .

ifeq ($(SOC),$(filter $(SOC), j7200 j721e j7200 am65xx))
 SRCDIR += $(PDK_INSTALL_PATH)/ti/build/unit-test/Unity/src
 SRCDIR += $(PDK_INSTALL_PATH)/ti/build/unit-test/config
 INCDIR += $(PDK_INSTALL_PATH)/ti/build/unit-test/Unity/src
 INCDIR += $(PDK_INSTALL_PATH)/ti/build/unit-test/config
 SRCS_COMMON += unity_config.c unity.c
 CFLAGS_LOCAL_COMMON += -DUNITY_INCLUDE_CONFIG_H
endif

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
