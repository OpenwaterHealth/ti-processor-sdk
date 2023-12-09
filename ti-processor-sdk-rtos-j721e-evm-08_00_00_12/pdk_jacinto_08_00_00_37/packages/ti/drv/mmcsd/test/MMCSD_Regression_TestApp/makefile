# Makefile for MMCSD DMA unit test app
ifeq ($(RULES_MAKE), )
include $(PDK_INSTALL_PATH)/ti/build/Rules.make
else
include $(RULES_MAKE)
endif

#TI RTOS Specifics
ifeq ($(BUILD_OS_TYPE), tirtos)

    APP_NAME = MMCSD_Regression_TestApp_$(BUILD_OS_TYPE)
    CFLAGS_OS_DEFINES = -DUSE_BIOS -DRTOS_ENV

    ifeq ($(SOC),$(filter $(SOC), am65xx))
        XDC_CFG_FILE_mpu1_0 = ../am65xx/a53/mmcsdUnitTest_a53.cfg
        XDC_CFG_FILE_mcu1_0 = ../am65xx/r5/mmcsdUnitTest_r5.cfg
    else
        XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/$(SOC)/sysbios_$(ISA).cfg
    endif

    INCLUDE_EXTERNAL_INTERFACES = bios xdc pdk

    COMP_LIST_COMMON = $(PDK_COMMON_TIRTOS_COMP)

endif

#FreeRTOS Specifics
ifeq ($(BUILD_OS_TYPE), freertos)

    APP_NAME = MMCSD_Regression_TestApp_$(BUILD_OS_TYPE)
    CFLAGS_OS_DEFINES = -DFREERTOS -DRTOS_ENV
    INCLUDE_EXTERNAL_INTERFACES = freertos pdk
    COMP_LIST_COMMON = $(PDK_COMMON_FREERTOS_COMP)

endif

CFLAGS_MMCSD_UT += -DMMCSD_ADMA_ENABLED -DMMCSD_REGRESSION_TEST -DMEASURE_TIME -DMMCSD_TEST_FATFS_BENCHMARK_ENABLED

SRCDIR = ../src ../../
INCDIR = ../src ../include ../../
# Common source files across all platforms and cores
SRCS_COMMON += main.c profiling.c

# List all the components required by the application
COMP_LIST_COMMON += mmcsd fatfs_indp

# CFLAGS_MMCSD_UT += -DIO_CONSOLE
# am65xx build
ifeq ($(BOARD),$(filter $(BOARD), am65xx-sim))
CFLAGS_MMCSD_UT    += -DsimAM6x
endif

# EVM build
ifeq ($(BOARD),$(filter $(BOARD), am65xx-evm))
CFLAGS_MMCSD_UT += -DevmAM65xx
endif


PACKAGE_SRCS_COMMON = ../
CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS) $(CFLAGS_MMCSD_UT) $(CFLAGS_OS_DEFINES)

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
