#
# This file is the makefile for building PM example utils library.
#
#   Copyright (c) Texas Instruments Incorporated 2015-2016
#
include $(PDK_INSTALL_PATH)/ti/build/Rules.make

MODULE_NAME = pm_example_utils

SRCDIR = .
INCDIR = .

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk csl
INCLUDE_INTERNAL_INTERFACES =

# Common source files and CFLAGS across all platforms and cores
PACKAGE_SRCS_COMMON = .
ifeq ($(SOC),$(filter $(SOC), j721e j7200))
SRCS_COMMON += app_utils_k3.c
else
SRCS_COMMON += app_utils.c uartStdio.c
PACKAGE_SRCS_COMMON += ../lnk_a15.cmd ../lnk_dsp.cmd ../lnk_m4.cmd ../lnk_arp32.cmd
endif

CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS)

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
