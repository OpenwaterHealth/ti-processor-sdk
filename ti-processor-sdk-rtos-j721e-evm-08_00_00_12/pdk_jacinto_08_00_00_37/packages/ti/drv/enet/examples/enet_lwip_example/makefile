#
# This file is the makefile for building LWIP Example library.
#
include $(PDK_INSTALL_PATH)/ti/drv/enet/examples/build/config.mk

APP_NAME = enet_lwip_example$(ENET_OS_TYPE_POSTFIX)

# lwip example dir
SRCDIR += .
INCDIR += .

# OS ports
INCDIR += $(PDK_ENET_COMP_PATH)/lwipif/ports/$(BUILD_OS_TYPE)/include
INCDIR += $(PDK_ENET_COMP_PATH)/lwipif/ports/$(BUILD_OS_TYPE)/include/arch

ifeq ($(BUILD_OS_TYPE),freertos)
  ENET_CFLAGS += -DFREERTOS
endif
ifeq ($(BUILD_OS_TYPE),tirtos)
  INCDIR += $(bios_PATH)/packages/ti/posix/ccs
endif

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES += pdk

INCLUDE_INTERNAL_INTERFACES += lwipif_$(BUILD_OS_TYPE)
INCLUDE_INTERNAL_INTERFACES += lwipstack_$(BUILD_OS_TYPE)
INCLUDE_INTERNAL_INTERFACES += lwipcontrib_$(BUILD_OS_TYPE)

# List all the components required by the application
COMP_LIST_COMMON += enet enetsoc enetphy enet_example_utils_full$(ENET_OS_TYPE_POSTFIX)
COMP_LIST_COMMON += lwipif$(ENET_OS_TYPE_POSTFIX)
COMP_LIST_COMMON += lwipstack$(ENET_OS_TYPE_POSTFIX)
COMP_LIST_COMMON += lwipcontrib$(ENET_OS_TYPE_POSTFIX)

ifneq ($(SOC),$(filter $(SOC), tpr12 awr294x))
  COMP_LIST_COMMON += enet_cfgserver
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
XDC_CFG_UPDATE_$(CORE) = $(PDK_ENET_COMP_PATH)/examples/enet_lwip_example/lwip_example.cfg

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += main.c
SRCS_COMMON += test.c
SRCS_COMMON += udp_iperf.c

CFLAGS_LOCAL_COMMON += $(PDK_CFLAGS) $(ENET_CFLAGS)

# Common source files and CFLAGS across all platforms and cores
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
