#
# This file is the makefile for building Enet driver library.
#
ifeq ($(RULES_MAKE), )
include $(PDK_INSTALL_PATH)/ti/build/Rules.make
else
include $(RULES_MAKE)
endif

MODULE_NAME = enet

SRCDIR = src/core src/mod src/per src/phy src/common
INCDIR = . include src/modules/priv src/phy/priv

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk
INCLUDE_INTERNAL_INTERFACES = csl enetphy

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += enet.c
SRCS_COMMON += enet_trace.c enet_utils.c enet_osal.c enet_queue.c enet_rm.c

# TI-RTOS and PDK based implementations
SRCS_COMMON += enet_osal_dflt.c enet_utils_dflt.c enet_phymdio_dflt.c

# Enet Module source files
SRCS_COMMON += mod_null.c

# CPSW modules
SRCS_COMMON += mdio.c
SRCS_COMMON += cpsw_macport.c cpsw_macport_intervlan.c
SRCS_COMMON += cpsw_hostport.c
SRCS_COMMON += cpsw_ale.c
SRCS_COMMON += cpsw_stats.c
SRCS_COMMON += cpsw_cpts.c

# ICSSG modules
ifeq ($(ENET_ENABLE_ICSSG),yes)
SRCS_COMMON += icssg.c icssg_utils.c
SRCS_COMMON += icssg_timesync.c
SRCS_COMMON += icssg_stats.c
INCLUDE_INTERNAL_INTERFACES += pruss
endif

# Enet Peripheral source files
SRCS_COMMON += per_null.c
SRCS_COMMON += cpsw_intervlan.c
ifeq ($(SOC),$(filter $(SOC), tpr12 awr294x))
SRCS_COMMON += cpsw_tpr12.c
else
SRCS_COMMON += cpsw.c
endif


# DMA modules
ifeq ($(SOC),$(filter $(SOC), j721e j7200 am65xx))
SRCDIR += src/dma/udma
INCDIR += include/dma/udma
SRCS_COMMON += enet_hostport_udma.c
SRCS_COMMON += enet_udma.c enet_udma_priv.c
endif
ifeq ($(SOC),$(filter $(SOC), tpr12 awr294x))
SRCDIR += src/dma/cpdma
INCDIR += include/dma/cpdma
SRCS_COMMON += enet_hostport_cpdma.c
SRCS_COMMON += enet_cpdma.c enet_cpdma_priv.c
endif

PACKAGE_SRCS_COMMON = enet.h enet_cfg.h enet_component.mk makefile src/makefile .gitignore
PACKAGE_SRCS_COMMON += include priv tools docs $(SRCDIR) firmware/
PACKAGE_SRCS_COMMON += config_mk.bld package.bld package.xdc package.xs
PACKAGE_SRCS_COMMON += Settings.xdc.xdt enetver.h enetver.h.xdt
CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS) $(ENET_CFLAGS)

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
