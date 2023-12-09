#
# This file is the makefile for building CSL XIP app.
#
#   Copyright (c) Texas Instruments Incorporated 2021
#
include $(PDK_INSTALL_PATH)/ti/build/Rules.make

APP_NAME = memory_benchmarking_app

ifndef $(MEM)
	MEM=ocmc
endif

LOCAL_APP_NAME = $(MEM)_perf_$(BOARD)_$(CORE)ExampleApp

BUILD_OS_TYPE=tirtos

SRCDIR = .
INCDIR = .

ifeq ($(CORE), mcu2_0)
SRCS_ASM_COMMON += ratConf.asm
endif

CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS)

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk xdc bios
SRCS_COMMON += main_tirtos.c

XDC_CFG_FILE_$(CORE) = ./sysbios_r5f.cfg

ifeq ($(MEM), xip)
     CFLAGS_LOCAL_COMMON += -DBUILD_XIP
endif
EXTERNAL_LNKCMD_FILE_LOCAL = $(pdk_PATH)/ti/csl/example/ospi/memory_benchmarking/linker_$(CORE)_$(MEM).lds

# List all the components required by the application
COMP_LIST_COMMON = $(PDK_COMMON_TIRTOS_COMP)

ifeq ($(CORE), mcu2_0)
	CFLAGS_LOCAL_COMMON += -DNO_SCISERVER
	CFLAGS_LOCAL_COMMON += -DBUILD_MCU2_0
else
	CFLAGS_LOCAL_COMMON += -DBUILD_MCU1_0
ifeq ($(MEM), xip)
    COMP_LIST_COMMON += sciserver_tirtos
else
    CFLAGS_LOCAL_COMMON += -DNO_SCISERVER
endif

endif

ifeq ($(CCS), 1)
	CFLAGS_LOCAL_COMMON += -DCCS
	# refer FAQ in SDK for the reason for
	# using the utility with CCS boot.
	SRCS_ASM_COMMON += utilsCopyVecs2ATcm.asm
endif

ifeq ($(MULTICORE), 1)
	CFLAGS_LOCAL_COMMON += -DMULTICORE
endif

SRCS_EMAC_UT_NULL =

EXT_LIB_LIST_mcu1_0=


PACKAGE_SRCS_COMMON = .

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
