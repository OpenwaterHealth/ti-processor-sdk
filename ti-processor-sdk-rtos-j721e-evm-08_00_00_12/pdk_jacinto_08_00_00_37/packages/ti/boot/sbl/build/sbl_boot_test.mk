#
# This file is the makefile for building images used for SBL testing.
#
include $(PDK_INSTALL_PATH)/ti/build/Rules.make

APP_NAME = sbl_boot_test
BUILD_OS_TYPE = baremetal
LOCAL_APP_NAME = sbl_$(BUILD_OS_TYPE)_boot_test_$(BOARD)_$(CORE)TestApp

SBL_SRC_DIR =  $(PDK_INSTALL_PATH)/ti/boot/sbl

SRCDIR      += $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp

INCDIR      += $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp



CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS)
PACKAGE_SRCS_COMMON = .

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES =

# List all the components required by the application
COMP_LIST_COMMON =

SRCS_COMMON += sbl_amp_multicore.c sbl_printf.c

# asm files and linker scripts change due to different tool chains for R5 and A53
ifeq ($(CORE),$(filter $(CORE), mcu1_0 mcu1_1 mcu2_0 mcu2_1 mcu3_0 mcu3_1))
  SRCS_ASM_COMMON = sbl_multicore_r5.asm
  EXTERNAL_LNKCMD_FILE_LOCAL =  $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp/$(SOC)/mcuAmplinker.lds
endif

ifeq ($(CORE),$(filter $(CORE), mpu1_0 mpu1_1 mpu2_0 mpu2_1))
  SRCS_ASM_COMMON = sbl_multicore_a53.asm
  LNKCMD_FILE = $(PDK_SBL_COMP_PATH)/example/k3MulticoreApp/$(SOC)/mpuAmplinker.lds
endif

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

