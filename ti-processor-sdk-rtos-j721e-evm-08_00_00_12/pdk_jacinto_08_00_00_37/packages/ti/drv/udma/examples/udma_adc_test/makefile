#
# This file is the makefile for building UDMA ADC test app.
#
ifeq ($(RULES_MAKE), )
include $(PDK_INSTALL_PATH)/ti/build/Rules.make
else
include $(RULES_MAKE)
endif

APP_NAME = udma_adc_testapp_$(BUILD_OS_TYPE)

SRCDIR = .
INCDIR =

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk

ifeq ($(BUILD_OS_TYPE), tirtos)
  # List all the components required by the application
  COMP_LIST_COMMON = $(PDK_COMMON_TIRTOS_COMP) 
  INCLUDE_EXTERNAL_INTERFACES += xdc bios
  XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/$(SOC)/sysbios_$(ISA).cfg
endif
ifeq ($(BUILD_OS_TYPE), freertos)
  COMP_LIST_COMMON = $(PDK_COMMON_FREERTOS_COMP)
  INCLUDE_EXTERNAL_INTERFACES += freertos
endif

COMP_LIST_COMMON += udma_apputils

# AM65xx specific : required to demo runtime copy of .text to different area
ifeq ($(SOC),$(filter $(SOC), am65xx))
  ifeq ($(CORE),$(filter $(CORE), mcu1_0))
    # Slave uses local linker command file to avoid code/data memory conflict with master application
    ifeq ($(BUILD_OS_TYPE), tirtos)
      EXTERNAL_LNKCMD_FILE_LOCAL =  $(PDK_INSTALL_PATH)/ti/drv/udma/examples/udma_adc_test/build/am65xx/linker_r5_sysbios.lds
      CFLAGS_LOCAL_COMMON = -DRUN_TIME_RELOCATION_DEMO
    endif
  endif
endif

# Common source files and CFLAGS across all platforms and cores
PACKAGE_SRCS_COMMON = .
SRCS_COMMON = main_rtos.c udma_adc_test.c
CFLAGS_LOCAL_COMMON += $(PDK_CFLAGS)

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
