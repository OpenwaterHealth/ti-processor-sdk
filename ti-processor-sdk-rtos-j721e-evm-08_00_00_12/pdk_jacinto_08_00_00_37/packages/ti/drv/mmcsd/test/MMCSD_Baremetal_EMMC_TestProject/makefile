# Makefile for MMCSD Baremetal EMMC  unit test app
ifeq ($(RULES_MAKE), )
include $(PDK_INSTALL_PATH)/ti/build/Rules.make
else
include $(RULES_MAKE)
endif

export DISABLE_RECURSE_DEPS

#Name of the directory created under packages/ti/binary/
APP_NAME = MMCSD_Baremetal_EMMC_TestProject
# Name of the binary if different from the default (APP_NAME)_$(BOARD_$(CORE)_<build_profile>
ifneq ($(SOC),$(filter $(SOC), am65xx j721e j7200))
    LOCAL_APP_NAME = MMCSD_Baremetal_EMMC_$(BOARD)_$(CORE)TestProject
endif

BUILD_OS_TYPE = baremetal

ifeq ($(SOC),$(filter $(SOC), am65xx j721e))
SRCDIR = ../src ../../
INCDIR = ../src ../include ../../
# Common source files across all platforms and cores
SRCS_COMMON += main_emmc.c

endif

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk csl_arch

# List all the components required by the application
COMP_LIST_COMMON += $(PDK_COMMON_BAREMETAL_COMP)
COMP_LIST_COMMON += mmcsd
COMP_LIST_c66x = csl_intc
COMP_LIST_a15_0   = csl_init
COMP_LIST_mpu1_0  = csl_init
COMP_LIST_mcu1_0  = csl_init

# Common source files and CFLAGS across all platforms and cores
PACKAGE_SRCS_COMMON = .
CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS) -DBARE_METAL

LNKFLAGS_LOCAL_a15_0  += --entry Entry
LNKFLAGS_LOCAL_a8host += --entry Entry
LNKFLAGS_LOCAL_a9host += --entry Entry
LNKFLAGS_LOCAL_mpu1_0 += --entry Entry

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =


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
