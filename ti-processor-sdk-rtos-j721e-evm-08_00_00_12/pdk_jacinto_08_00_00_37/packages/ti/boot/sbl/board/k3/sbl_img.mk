#
# This file is the makefile for building SBL image
# that is loaded by R5 ROM.
#
include $(PDK_INSTALL_PATH)/ti/build/Rules.make

HLOS_SUFFIX=
ifeq ($(HLOS_BOOT),yes)
HLOS_SUFFIX=_hlos
endif
HS_SUFFIX=
ifeq ($(BUILD_HS),yes)
HS_SUFFIX=_hs
endif
DMA_SUFFIX=
ifeq ($(BOOTMODE), ospi)
  ifeq ($(SBL_USE_DMA),no)
    DMA_SUFFIX=_nondma
  endif
endif

APP_NAME = sbl_$(BOOTMODE)_img$(HLOS_SUFFIX)$(HS_SUFFIX)
LOCAL_APP_NAME=sbl_$(BOOTMODE)_img$(HLOS_SUFFIX)_$(CORE)
BUILD_OS_TYPE = baremetal

ifeq ($(SOC), $(filter $(SOC), tpr12 awr294x))
ifeq ($(SOC), tpr12)
SRCDIR += $(PDK_SBL_COMP_PATH)/board/evmTPR12
endif
ifeq ($(SOC), awr294x)
SRCDIR += $(PDK_SBL_COMP_PATH)/board/evmAWR294x
endif
else
SRCDIR += $(PDK_SBL_COMP_PATH)/board/k3
endif

INCDIR      += $(PDK_SBL_COMP_PATH)
INCDIR      += $(PDK_SBL_COMP_PATH)/soc
ifeq ($(SOC), $(filter $(SOC), tpr12 awr294x))
INCDIR      += $(PDK_SBL_COMP_PATH)/soc/tpr12
INCDIR      += $(PDK_SBL_COMP_PATH)/soc/tpr12/cfg_$(SOC)
else
INCDIR      += $(PDK_SBL_COMP_PATH)/soc/k3
endif

PACKAGE_SRCS_COMMON = .

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk

# List all the components required by the application
COMP_LIST_COMMON = $(PDK_COMMON_BAREMETAL_COMP)
ifneq ($(strip $(HS_SUFFIX)),) #if $(HS_SUFFIX) is non-empty
  SCICLIENT := $(filter sciclient%,$(COMP_LIST_COMMON))
  COMP_LIST_COMMON := $(subst $(SCICLIENT),$(SCICLIENT)_hs,$(COMP_LIST_COMMON))
endif

CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS) $(SBL_CFLAGS)

# Check for custom flags
ifeq ($(BOOTMODE), cust)
  SBL_CFLAGS = $(CUST_SBL_FLAGS)
  COMP_LIST_COMMON += sbl_lib_$(BOOTMODE)$(HS_SUFFIX)
else
  ifeq ($(SBL_USE_DMA),yes)
    SBL_CFLAGS += -DSBL_USE_DMA=1
  else
    SBL_CFLAGS += -DSBL_USE_DMA=0
  endif
  COMP_LIST_COMMON += sbl_lib_$(BOOTMODE)$(DMA_SUFFIX)$(HLOS_SUFFIX)$(HS_SUFFIX)
endif # ifeq ($(BOOTMODE), cust)

# HLOS Boot flags
ifeq ($(HLOS_BOOT),yes)
  SBL_CFLAGS += -DSBL_ENABLE_HLOS_BOOT -DSBL_HLOS_OWNS_FLASH
endif
ifeq ($(BUILD_HS),yes)
  SBL_CFLAGS += -DSBL_BUILD_HS -DBUILD_HS
endif
# BOOTMODE specific CFLAGS
ifeq ($(BOOTMODE), mmcsd)
  SBL_CFLAGS += -DBOOT_MMCSD
endif # ifeq ($(BOOTMODE), mmcsd)

ifeq ($(BOOTMODE), ospi)
  SBL_CFLAGS += -DBOOT_OSPI
endif # ifeq ($(BOOTMODE), ospi)

ifeq ($(BOOTMODE), hyperflash)
  SBL_CFLAGS += -DBOOT_HYPERFLASH
endif # ifeq ($(BOOTMODE), hyperflash)

ifeq ($(BOOTMODE), uart)
  SBL_CFLAGS += -DBOOT_UART
endif # ifeq ($(BOOTMODE), uart)

ifeq ($(BOOTMODE), qspi)
  SBL_CFLAGS += -DBOOT_QSPI
endif # ifeq ($(BOOTMODE), qspi)

ifeq ($(filter $(SBL_CFLAGS), -DBOOT_MMCSD), -DBOOT_MMCSD)
  COMP_LIST_COMMON += mmcsd fatfs_indp
endif # ifeq ($(filter $(SBL_CFLAGS), -DBOOT_MMCSD), -DBOOT_MMCSD)

ifeq ($(filter $(SBL_CFLAGS), -DBOOT_HYPERFLASH), -DBOOT_HYPERFLASH)
  COMP_LIST_COMMON += spi
endif # ifeq ($(filter $(SBL_CFLAGS), -DBOOT_HYPERFLASH), -DBOOT_HYPERFLASH)

ifeq ($(filter $(SBL_CFLAGS), -DBOOT_OSPI), -DBOOT_OSPI)
ifeq ($(filter $(SBL_CFLAGS), -DSBL_USE_DMA=1), -DSBL_USE_DMA=1)
  COMP_LIST_COMMON += spi_dma
else
  COMP_LIST_COMMON += spi
endif # ifeq ($(filter $(SBL_CFLAGS), -DSBL_USE_DMA=1), -DSBL_USE_DMA=1)
endif # ifeq ($(filter $(SBL_CFLAGS), -DBOOT_OSPI), -DBOOT_OSPI)

ifeq ($(filter $(SBL_CFLAGS), -DBOOT_QSPI), -DBOOT_QSPI)
ifeq ($(filter $(SBL_CFLAGS), -DSBL_USE_DMA=1), -DSBL_USE_DMA=1)
  COMP_LIST_COMMON += spi_dma
else
  COMP_LIST_COMMON += spi
endif # ifeq ($(filter $(SBL_CFLAGS), -DSBL_USE_DMA=1), -DSBL_USE_DMA=1)
endif # ifeq ($(filter $(SBL_CFLAGS), -DBOOT_OSPI), -DBOOT_OSPI)


SRCS_COMMON += sbl_main.c

ifeq ($(SOC), $(filter $(SOC), tpr12))
  SRCS_COMMON += sbl_pinmux.c
endif
ifeq ($(SOC), $(filter $(SOC), tpr12 awr294x))
  SRCS_COMMON += sbl_module_clock_config.c
endif


ifeq ($(SOC), $(filter $(SOC), tpr12 awr294x))
EXTERNAL_LNKCMD_FILE_LOCAL = $(PDK_SBL_COMP_PATH)/soc/tpr12/linker.cmd
else
EXTERNAL_LNKCMD_FILE_LOCAL = $(PDK_SBL_COMP_PATH)/soc/k3/$(SOC)/linker.cmd
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

# Select the SBL_OBJ_COPY to use.
#
SBL_OBJ_COPY := $(TOOLCHAIN_PATH_GCC_ARCH64)/bin/$(GCC_ARCH64_BIN_PREFIX)-objcopy
#SoCs like TPR12 do not have GCC tool. So we package it as part of SBL
#TI ARM CGT objcopy does not copy .data sections correctly so cannot be used
ifeq ("$(wildcard ${TOOLCHAIN_PATH_GCC_ARCH64})","")
ifneq ($(OS),Windows_NT)
SBL_OBJ_COPY := ${PDK_SBL_COMP_PATH}/tools/tpr12SBLImageGen/unix/aarch64-none-elf-objcopy
else
SBL_OBJ_COPY := ${PDK_SBL_COMP_PATH}/tools/tpr12SBLImageGen/windows/aarch64-none-elf-objcopy.exe
endif
endif
export SBL_OBJ_COPY

# EFUSE_DEFAULT - R5 ROM will run the SBL in lockstep mode in lockstep
# #                 enabled devices and in split mode, if the devices do
# #                 not support lockstep.
# # SPLIT_MODE -    R5 ROM will awlys run the SBL on MCU1_0 in split mode
# #                 irrespective of whether the EFuse says the device can
# #                 support lockstep mode.
R5_STARTUP_MODE := SPLIT_MODE
export R5_STARTUP_MODE

include $(MAKERULEDIR)/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
