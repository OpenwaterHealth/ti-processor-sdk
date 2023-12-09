#
# This file is the makefile for building Keywriter image
# that is loaded by R5 ROM.
#
include $(PDK_INSTALL_PATH)/ti/build/Rules.make

APP_NAME                     = keywriter_img
LOCAL_APP_NAME               = keywriter_img_$(SOC)
BUILD_OS_TYPE                = baremetal

KEYWRITER_APP_DIR            = $(PDK_KEYWR_COMP_PATH)
SRCDIR                      += $(KEYWRITER_APP_DIR)

INCDIR                      += $(PDK_INSTALL_PATH)/ti/board/src/$(BOARD)/include
INCDIR                      += $(PDK_INSTALL_PATH)/ti/csl
INCDIR                      += $(KEYWRITER_APP_DIR)/boardcfgs/$(SOC)
INCDIR                      += $(PDK_INSTALL_PATH)/ti/drv/pmic
INCDIR                      += $(PDK_INSTALL_PATH)/ti/drv/pmic/include
INCDIR                      += $(PDK_INSTALL_PATH)/ti/drv/pmic/test/common
INCDIR                      += $(PDK_INSTALL_PATH)/ti/drv/pmic/test/power_test
INCDIR                      += $(KEYWRITER_APP_DIR)/soc/$(SOC)

PACKAGE_SRCS_COMMON          = .

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES  = pdk

# List all the components required by the application
COMP_LIST_COMMON             = pmic sciclient_direct board uart osal_nonos csl csl_init i2c gpio rm_pm_hal

CFLAGS_LOCAL_COMMON          = $(PDK_CFLAGS)

SRCS_COMMON                 += main.c
SRCS_COMMON                 += soc/$(SOC)/pmic_example.c
SRCS_COMMON                 += soc/$(SOC)/keywriter_utils.c
SRCS_ASM_COMMON             += init.asm

EXTERNAL_LNKCMD_FILE_LOCAL   = $(PDK_KEYWR_COMP_PATH)/soc/$(SOC)/linker.lds

export R5_STARTUP_MODE      := SPLIT_MODE

SCI_CLIENT_DIR              := $(PDK_INSTALL_PATH)/ti/drv/sciclient
SCI_CLIENT_TOOLS_BIN2C_DIR  := $(SCI_CLIENT_DIR)/tools/bin2c

ifeq ($(OS),Windows_NT)
  BIN2C_EXE                 := $(SCI_CLIENT_TOOLS_BIN2C_DIR)/bin2c.exe
else
  BIN2C_EXE                 := $(SCI_CLIENT_TOOLS_BIN2C_DIR)/bin2c.out
endif

# signed keywr sys bin
SIGNED_SYSFW_BIN            := $(KEYWRITER_APP_DIR)/tifs_bin/$(SOC)/ti-fs-keywriter.bin

# Include common make files
ifeq ($(MAKERULEDIR), )
  MAKERULEDIR               := $(ROOTDIR)/ti/build/makerules
  export MAKERULEDIR
endif
include $(MAKERULEDIR)/common.mk
include $(PDK_KEYWR_COMP_PATH)/build/bin2c.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
