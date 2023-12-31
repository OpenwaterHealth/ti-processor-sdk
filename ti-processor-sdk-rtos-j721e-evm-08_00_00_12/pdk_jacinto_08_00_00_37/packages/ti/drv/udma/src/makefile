#
# This file is the makefile for building UDMA driver library.
#
ifeq ($(RULES_MAKE), )
include $(PDK_INSTALL_PATH)/ti/build/Rules.make
else
include $(RULES_MAKE)
endif

MODULE_NAME = udma

SRCDIR = src
SRCDIR += soc/$(SOC)
INCDIR =

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk

PACKAGE_SRCS_COMMON = udma.h udma_component.mk makefile .gitignore docs $(SRCDIR)
PACKAGE_SRCS_COMMON += include/udma_cfg.h
PACKAGE_SRCS_COMMON += include/udma_ch.h
PACKAGE_SRCS_COMMON += include/udma_event.h
PACKAGE_SRCS_COMMON += include/udma_flow.h
PACKAGE_SRCS_COMMON += include/udma_osal.h
PACKAGE_SRCS_COMMON += include/udma_proxy.h
PACKAGE_SRCS_COMMON += include/udma_ring.h
PACKAGE_SRCS_COMMON += include/udma_rm.h
PACKAGE_SRCS_COMMON += include/udma_types.h
PACKAGE_SRCS_COMMON += include/udma_utils.h
PACKAGE_SRCS_COMMON += soc/udma_soc.h
PACKAGE_SRCS_COMMON += config_mk.bld package.bld package.xdc package.xs
PACKAGE_SRCS_COMMON += Settings.xdc.xdt udmaver.h udmaver.h.xdt

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += udma.c
SRCS_COMMON += udma_rm.c udma_rmcfg.c
SRCS_COMMON += udma_ch.c
SRCS_COMMON += udma_ring_common.c udma_proxy.c udma_flow.c udma_event.c
SRCS_COMMON += udma_osal.c udma_utils.c
SRCS_COMMON += udma_soc.c
#DRU is not present in all SOC
ifeq ($(SOC),$(filter $(SOC), am65xx j721e))
SRCS_COMMON += udma_dru.c
PACKAGE_SRCS_COMMON += include/udma_dru.h
endif
#Normal RA is not present in all SOC
ifeq ($(SOC),$(filter $(SOC), am65xx j721e j7200))
SRCS_COMMON += udma_ring_normal.c
endif
#LCDMA RA is not present in all SOC
ifeq ($(SOC),$(filter $(SOC), am64x))
SRCS_COMMON += udma_ring_lcdma.c
endif

CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS) $(UDMA_CFLAGS)

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
