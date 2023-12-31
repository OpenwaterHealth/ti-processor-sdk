#
# This file is the makefile for building timesync icss driver library.
#
include $(PDK_INSTALL_PATH)/ti/build/Rules.make

MODULE_NAME = timeSync

ifneq ($(SOC),$(filter $(SOC), j721e j7200 am65xx))
ifeq ($(SOC),$(filter $(SOC), am574x am572x am571x am335x am437x k2g))
  SRCDIR = src
  INCDIR = . ../include .. $(EDMA3LLD_BIOS6_INSTALLDIR)/packages
endif

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk bios xdc osal_tirtos edma

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON  = icss_timeSync.c icss_timeSync_mem.c
SRCS_COMMON += icss_timeSync_init.c  icss_timeSyncApi.c
SRCS_COMMON += icss_timeSync_tools.c

ifeq ($(SOC),$(filter $(SOC), am335x am437x))
SRCS_COMMON += icss_timeSync_ecap_edma.c
endif

TIMESYNC_ICSS_CFLAGS =
CFLAGS_LOCAL_a15_0  = -mno-unaligned-access
CFLAGS_LOCAL_a8host = -mno-unaligned-access
CFLAGS_LOCAL_a9host = -mno-unaligned-access
CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS) $(TIMESYNC_ICSS_CFLAGS)
endif

PACKAGE_SRCS_COMMON = timeSync_component.mk makefile include docs src build v2
PACKAGE_SRCS_COMMON += config_mk.bld package.bld package.xdc package.xs Settings.xdc.xdt timeSync_ver.h timeSync_ver.h.xdt

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

# Nothing beyond this point:
