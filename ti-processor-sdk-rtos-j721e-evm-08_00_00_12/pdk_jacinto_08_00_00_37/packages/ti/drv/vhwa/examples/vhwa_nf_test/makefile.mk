#
# This file is common makefile for building VHWA NF test app for both TI-RTOS/baremetal
#

SRCDIR = .
SRCDIR += ../common
INCDIR =

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk
SRCS_COMMON += vhwa_examples_common.c vhwa_nf_api.c vhwa_common_crc.c

# List all the components required by the application
COMP_LIST_COMMON = fvid2 vhwa

ifeq ($(BUILD_OS_TYPE), baremetal)
  COMP_LIST_COMMON += $(PDK_COMMON_BAREMETAL_COMP)
  SRCS_COMMON += main_baremetal.c
  EXTERNAL_LNKCMD_FILE_LOCAL = ../common/vhwa_r5.cmd
endif

ifeq ($(BUILD_OS_TYPE),tirtos)
  CFLAGS_OS_DEFINES = -DUSE_BIOS
  INCLUDE_EXTERNAL_INTERFACES += xdc bios
  COMP_LIST_COMMON += $(PDK_COMMON_TIRTOS_COMP)
  SRCS_COMMON += main_rtos.c

  # Enable XDC build for application by providing XDC CFG File per core
  XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/$(SOC)/sysbios_$(ISA).cfg
  EXTERNAL_LNKCMD_FILE_LOCAL = ../common/vhwa_sysbios_r5.cmd

endif

ifeq ($(BUILD_OS_TYPE),freertos)
  CFLAGS_OS_DEFINES = -DFREERTOS
  INCLUDE_EXTERNAL_INTERFACES += freertos
  COMP_LIST_COMMON += $(PDK_COMMON_FREERTOS_COMP)
  SRCS_COMMON += main_rtos.c

  EXTERNAL_LNKCMD_FILE_LOCAL = ../common/vhwa_freertos_r5.cmd
endif

# Common source files and CFLAGS across all platforms and cores
PACKAGE_SRCS_COMMON = . ../include ../common

LNKFLAGS_LOCAL_mpu1_0 += --entry Entry
CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS) $(CFLAGS_GLOBAL_$(BOARD)) $(CFLAGS_OS_DEFINES)

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
