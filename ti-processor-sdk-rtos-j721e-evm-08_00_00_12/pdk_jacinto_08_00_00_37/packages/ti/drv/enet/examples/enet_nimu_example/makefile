
# This file is the makefile for building ENET loopback test app for TI RTOS
#

include $(PDK_INSTALL_PATH)/ti/drv/enet/examples/build/config.mk

APP_NAME = enet_nimu_example

SRCDIR += . ../enet_apputils ../common

# Common source files and CFLAGS across all platforms and cores
PACKAGE_SRCS_COMMON = .

INCDIR += $(bios_PATH)/packages/ti/posix/ccs

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES += pdk ndk

# List all the components required by the application
COMP_LIST_COMMON += enet enetsoc enetphy nimuenet enet_example_utils_full_tirtos enet_cfgserver

SRCS_COMMON = main_tirtos.c
ifeq ($(CORE),$(filter $(CORE), mpu1_0))
SRCS_COMMON += bios_mmu.c
endif

# Enable TimeSync on SoCs that have support on PDK TimeSync v2 driver
ifeq ($(SOC),$(filter $(SOC), j721e j7200))
ifeq ($(CORE),$(filter $(CORE), mcu2_0))
COMP_LIST_COMMON += timeSync_hal timeSync_ptp
CFLAGS_LOCAL_COMMON += -DENET_ENABLE_TIMESYNC
endif
endif
ifeq ($(SOC),$(filter $(SOC), am65xx))
ifeq ($(CORE),$(filter $(CORE), mcu1_0))
COMP_LIST_COMMON += timeSync_hal timeSync_ptp
CFLAGS_LOCAL_COMMON += -DENET_ENABLE_TIMESYNC
endif
endif

# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_UPDATE_$(CORE)=$(PDK_ENET_COMP_PATH)/examples/enet_nimu_example/nimu_example.cfg

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON +=

CFLAGS_LOCAL_COMMON += $(PDK_CFLAGS) $(ENET_CFLAGS)

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
