#
# This file is the makefile for building NDK2ENET NIMU library.
#
ifeq ($(RULES_MAKE), )
include $(PDK_INSTALL_PATH)/ti/build/Rules.make
else
include $(RULES_MAKE)
endif

MODULE_NAME = nimuenet

SRCDIR = nimuenet
INCDIR = . include

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk xdc bios ndk
INCLUDE_INTERNAL_INTERFACES = enet
CFLAGS_LOCAL_COMMON = -DNDK_NOUSERAPIS
CFLAGS_LOCAL_COMMON += $(PDK_CFLAGS) $(ENET_CFLAGS)
CFLAGS_LOCAL_COMMON += -DNIMU_INSTRUMENTATION_ENABLED

#NDK checksum offload is not available in AM65xx PG1
ifneq ($(SOC),$(filter $(SOC), am65xx))
ENET_CFLAGS += -DNDK_CHECKSUM_SUPPORT
endif
# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += ndk2enet.c nimu_ndk.c

PACKAGE_SRCS_COMMON = .

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Temporary override for "_SYS_SELECT_H" redefined warning
ifeq ($(CORE), mpu1_0)
CFLAGS_LOCAL_COMMON += -std=c99
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
