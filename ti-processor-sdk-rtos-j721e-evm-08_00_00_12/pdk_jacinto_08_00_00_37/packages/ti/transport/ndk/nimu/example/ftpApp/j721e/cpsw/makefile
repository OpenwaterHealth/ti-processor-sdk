# Makefile for NIMU FTP sample app
include $(PDK_INSTALL_PATH)/ti/build/Rules.make

#Name of the directory created under packages/ti/binary/
APP_NAME = NIMU_FtpCpsw_ExampleApp

# Name of the binary if different from the default (APP_NAME)_$(BOARD_$(CORE)_<build_profile>
LOCAL_APP_NAME =  NIMU_FtpCpsw_$(BOARD)_$(CORE)ExampleApp

SRCDIR = . ../src ../../example/ftpApp/ftpserver  ../../example/src
INCDIR = . src/
INCDIR += $(bios_PATH)/packages/ti/posix/ccs
# Common source files across all platforms and cores
SRCS_COMMON += main_j721e.c nimu_osal.c ftp_commands.c ftp_filerout.c ftpserver.c

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = bios xdc ndk pdk

# List all the components required by the application
COMP_LIST_COMMON = $(PDK_COMMON_TIRTOS_COMP)
COMP_LIST_COMMON += emac uart_console nimu

CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS)
CFLAGS_LOCAL_COMMON += -DNIMU_FTP_APP -DNIMU_APP_CPSW

ifeq ($(CORE),$(filter $(CORE), mpu1_0))
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_$(CORE) =  $(PDK_INSTALL_PATH)/ti/transport/ndk/nimu/example/j721e/nimuExample_a72.cfg
CFLAGS_LOCAL_COMMON += -std=c99
endif

ifeq ($(CORE),$(filter $(CORE), mcu1_0))
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_$(CORE) =  $(PDK_INSTALL_PATH)/ti/transport/ndk/nimu/example/j721e/nimuExample_r5.cfg
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
