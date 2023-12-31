#
# This file is the makefile for building Sciclient Unit Test application.
#
ifeq ($(RULES_MAKE), )
include $(PDK_INSTALL_PATH)/ti/build/Rules.make
else
include $(RULES_MAKE)
endif

APP_NAME = sciserver_testapp_$(BUILD_OS_TYPE)

SRCDIR = . ../common
INCDIR = . ../common

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = sciserver_testapp_main.c \
              sciclient_appCommon.c

CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS)
PACKAGE_SRCS_COMMON = . ../common

# Enable XDC build for application by providing XDC CFG File per core
ifeq ($(BUILD_OS_TYPE),tirtos)
# List all the components required by the application
COMP_LIST_COMMON  = $(PDK_COMMON_TIRTOS_COMP) bios xdc
EXTERNAL_INTERFACES = bios xdc
XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/$(SOC)/sysbios_$(ISA).cfg
CONFIG_BLD_LNK_r5f = $(PDK_INSTALL_PATH)/ti/drv/sciclient/examples/sciserver_testapp/linker_r5_sysbios.lds
endif

ifeq ($(BUILD_OS_TYPE),freertos)
CFLAGS_OS_DEFINES = -DFREERTOS
# List all the components required by the application
COMP_LIST_COMMON  = $(PDK_COMMON_FREERTOS_COMP)
EXTERNAL_INTERFACES = freertos
CONFIG_BLD_LNK_r5f = $(PDK_INSTALL_PATH)/ti/drv/sciclient/examples/sciserver_testapp/linker_r5_freertos.lds
endif

COMP_LIST_COMMON += sciserver_tirtos

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk sciclient board uart $(EXTERNAL_INTERFACES)

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

