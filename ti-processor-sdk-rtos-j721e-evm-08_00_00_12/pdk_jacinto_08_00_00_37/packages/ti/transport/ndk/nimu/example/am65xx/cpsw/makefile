# Makefile for NIMU sample app
include $(PDK_INSTALL_PATH)/ti/build/Rules.make

ifeq ($(SMP), enable)
    #Name of the directory created under packages/ti/binary/
    APP_NAME = NIMU_Cpsw_SMP_ExampleApp
    # Name of the binary if different from the default (APP_NAME)_$(BOARD_$(CORE)_<build_profile>
    LOCAL_APP_NAME =  NIMU_Cpsw_$(BOARD)_$(CORE)_SMP_ExampleApp
else
    #Name of the directory created under packages/ti/binary/
    APP_NAME = NIMU_Cpsw_ExampleApp
    # Name of the binary if different from the default (APP_NAME)_$(BOARD_$(CORE)_<build_profile>
    LOCAL_APP_NAME =  NIMU_Cpsw_$(BOARD)_$(CORE)ExampleApp
endif

SRCDIR = . src/ ../example/src
INCDIR = . src/
INCDIR += $(bios_PATH)/packages/ti/posix/ccs
SRCS_COMMON += main_am65xx.c nimu_osal.c

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = bios xdc ndk pdk

# List all the components required by the application
COMP_LIST_COMMON = $(PDK_COMMON_TIRTOS_COMP)
COMP_LIST_COMMON += emac uart_console nimu

CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS) -DNDK_NOUSERAPIS -DNIMU_APP_CPSW


ifeq ($(CORE),$(filter $(CORE), mpu1_0))
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_$(CORE) = ./am65xx/nimuExample_a53.cfg
CFLAGS_LOCAL_COMMON += -std=c99 
EXTERNAL_LNKCMD_FILE_LOCAL = $(pdk_PATH)/ti/transport/ndk/nimu/example/am65xx/nimu_linker_a53.lds
endif

ifeq ($(CORE),$(filter $(CORE), mcu1_0))
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_$(CORE) = ./am65xx/nimuExample_r5.cfg
EXTERNAL_LNKCMD_FILE_LOCAL = $(pdk_PATH)/ti/transport/ndk/nimu/example/am65xx/nimu_linker_r5.lds
endif

PACKAGE_SRCS_COMMON = .


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
