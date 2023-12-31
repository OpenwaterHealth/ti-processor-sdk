#
# This file is the common makefile for building IPC driver library.
#

SRCDIR = src
ifeq ($(SOC),$(filter $(SOC), am65xx))
  SRCDIR += soc/V0
endif
ifeq ($(SOC),$(filter $(SOC), j721e j7200))
  SRCDIR += soc/V1
endif
ifeq ($(SOC),$(filter $(SOC), am64x))
  SRCDIR += soc/V2
endif
INCDIR =

#$(ECHO) $(ISA)

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk
ifeq ($(BUILD_OS_TYPE), qnx)
INCLUDE_EXTERNAL_INTERFACES += osal_qnx csl_init
endif

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += ipc_api.c ipc_mp.c ipc_soc.c ipc_virtio.c ipc_utils.c
ifeq ($(BUILD_OS_TYPE), qnx)
SRCS_COMMON += ipc_osal_qnx.c
else
SRCS_COMMON += ipc_osal.c
endif
ifeq ($(SOC),$(filter $(SOC), am64x))
SRCS_COMMON += ipc_mailbox_lld.c
else
SRCS_COMMON += ipc_mailbox.c
endif

PACKAGE_SRCS_COMMON = ipc.h ipc_component.mk makefile .gitignore include $(SRCDIR)
PACKAGE_SRCS_COMMON += soc/ipc_soc.h
PACKAGE_SRCS_COMMON += config_mk.bld ipcver.h
CFLAGS_LOCAL_COMMON += $(PDK_CFLAGS) $(IPC_CFLAGS) -DIPC_SUPPORT_SCICLIENT -DIPC_EXCLUDE_POLLED_RX -DMAILBOX_INTERRUPT_MODE

#Temporarily adding it until we have official support of C7x in Bios
CFLAGS_LOCAL_c7x = -Dxdc_target_name__=C71 -Dxdc_target_types__=ti/targets/elf/nda/std.h

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
