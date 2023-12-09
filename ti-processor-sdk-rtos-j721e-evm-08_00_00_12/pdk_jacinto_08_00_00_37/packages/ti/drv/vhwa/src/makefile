#
# This file is the makefile for building sciclient .
#
include $(PDK_INSTALL_PATH)/ti/build/Rules.make

MODULE_NAME = vhwa

SRCDIR = src src/csl src/csl/src src/drv
INCDIR = . include

ifeq ($(SOC),$(filter $(SOC), j721e))
  SRCDIR += soc/V0
  INCDIR += soc/V0
  SRCDIR += src/drv/V0
  INCDIR += src/drv/V0
endif

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk
INCLUDE_INTERNAL_INTERFACES = csl osal

# Common source files and CFLAGS across all platforms and cores
PACKAGE_SRCS_COMMON = .

include src/csl/src/src_files_csl.mk src/drv/src_files_drv.mk soc/src_files_soc.mk

CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS)
PACKAGE_SRCS_COMMON = vhwa_component.mk vhwa_msc.h .gitignore include src soc docs
PACKAGE_SRCS_COMMON += vhwaver.h vhwaver.h.xdt Settings.xdc.xdt package.xs
PACKAGE_SRCS_COMMON += package.xdc package.bld config_mk.bld makefile

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
