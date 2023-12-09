# Makefile for UART unit test app
include $(PDK_INSTALL_PATH)/ti/build/Rules.make
export DISABLE_RECURSE_DEPS

#Name of the directory created under packages/ti/binary/
APP_NAME = OSAL_TestApp_$(BUILD_OS_TYPE)

PACKAGE_SRCS_COMMON = . ../src

ifeq ($(SOC),$(filter $(SOC), am64x am65xx j721e j7200 tpr12 awr294x j721s2))
SRCDIR = . ../src ../
INCDIR = . ../src ../
# Common source files across all platforms and cores
SRCS_COMMON += main_osal_test.c
endif

ifeq ($(BUILD_OS_TYPE),tirtos)
CFLAGS_OS_DEFINES = -DUSE_BIOS
EXTERNAL_INTERFACES = bios xdc

# List all the components required by the application
COMP_LIST_COMMON = $(PDK_COMMON_TIRTOS_COMP)

ifeq ($(SOC),$(filter $(SOC), am64x am65xx))
ifeq ($(CORE),$(filter $(CORE), mpu1_0))
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/$(SOC)/sysbios_a53.cfg
endif
endif


ifeq ($(SOC),$(filter $(SOC), am65xx))
ifeq ($(CORE),$(filter $(CORE), mcu1_0))
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/am65xx/sysbios_r5f.cfg
EXTERNAL_LNKCMD_FILE = $(PDK_INSTALL_PATH)/ti/build/am65xx/SIMMAXWELL.cmd
endif
endif

ifeq ($(SOC),$(filter $(SOC), j721e))
ifeq ($(CORE),$(filter $(CORE), mpu1_0))
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/j721e/sysbios_a72.cfg
endif
endif

ifeq ($(SOC),$(filter $(SOC), am64x j721e j721s2))
ifeq ($(CORE),$(filter $(CORE), mcu1_0 mcu2_0 mcu2_1 mcu3_0 mcu1_1 mcu3_1 ))
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/$(SOC)/sysbios_r5f.cfg
EXTERNAL_LNKCMD_FILE = $(PDK_INSTALL_PATH)/ti/build/$(SOC)/linker_r5.lds
endif
endif

ifeq ($(SOC),$(filter $(SOC), j721e))
ifeq ($(CORE),$(filter $(CORE), c66xdsp_1))
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/$(SOC)/sysbios_c66.cfg
EXTERNAL_LNKCMD_FILE = $(PDK_INSTALL_PATH)/ti/build/$(SOC)/linker_c66.lds
endif

ifeq ($(CORE),$(filter $(CORE), c66xdsp_2))
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/j721e/sysbios_c66.cfg
EXTERNAL_LNKCMD_FILE = $(PDK_INSTALL_PATH)/ti/build/j721e/linker_c66.lds
endif

ifeq ($(CORE),$(filter $(CORE), c7x_1))
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/j721e/sysbios_c7x.cfg
EXTERNAL_LNKCMD_FILE = $(PDK_INSTALL_PATH)/ti/build/j721e/linker_c7x.lds
endif
endif

ifeq ($(SOC),$(filter $(SOC), j7200))

ifeq ($(CORE),$(filter $(CORE), mpu1_0))
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/j7200/sysbios_a72.cfg
endif

ifeq ($(CORE),$(filter $(CORE), mcu1_0 mcu2_0 mcu2_1 mcu1_1))
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/j7200/sysbios_r5f.cfg
EXTERNAL_LNKCMD_FILE = $(PDK_INSTALL_PATH)/ti/build/j7200/linker_r5.lds
endif

endif

ifeq ($(SOC),$(filter $(SOC), j721s2))
ifeq ($(CORE),$(filter $(CORE), mpu1_0))
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/j721s2/sysbios_a72.cfg
endif

ifeq ($(CORE),$(filter $(CORE), c7x_1))
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/j721s2/sysbios_c7x.cfg
EXTERNAL_LNKCMD_FILE = $(PDK_INSTALL_PATH)/ti/build/j721s2/linker_c7x.lds
endif
endif

ifeq ($(SOC),$(filter $(SOC), tpr12))
  XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/tpr12/sysbios_$(ISA).cfg
  XDC_CFG_UPDATE_$(CORE) = ../$(SOC)/osal_log_config.xs
  PACKAGE_SRCS_COMMON += ../$(SOC)
endif

ifeq ($(SOC),$(filter $(SOC), awr294x))
  XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/awr294x/sysbios_$(ISA).cfg
  XDC_CFG_UPDATE_$(CORE) = ../$(SOC)/osal_log_config.xs
  PACKAGE_SRCS_COMMON += ../$(SOC)
endif

endif

ifeq ($(BUILD_OS_TYPE),freertos)
CFLAGS_OS_DEFINES = -DFREERTOS
EXTERNAL_INTERFACES = freertos
COMP_LIST_COMMON    = $(PDK_COMMON_FREERTOS_COMP)
endif

ifeq ($(BUILD_OS_TYPE),safertos)
CFLAGS_OS_DEFINES = -DSAFERTOS
EXTERNAL_INTERFACES = safertos
COMP_LIST_COMMON    = $(PDK_COMMON_SAFERTOS_COMP)
endif

ifeq ($(SOC),$(filter $(SOC), am65xx j721e j7200 am64x tpr12 awr294x j721s2))
  CFLAGS_OSAL_UT += -DENABLE_TIMER_TEST
endif

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk $(EXTERNAL_INTERFACES)

CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS) $(CFLAGS_OSAL_UT) $(CFLAGS_OS_DEFINES)

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
