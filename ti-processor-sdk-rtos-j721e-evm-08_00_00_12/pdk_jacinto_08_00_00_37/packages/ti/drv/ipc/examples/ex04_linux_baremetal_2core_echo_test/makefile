#
# This file is the makefile for building IPC 2-core echo test app for Linux & Baremetal
#
ifeq ($(RULES_MAKE), )
include $(PDK_INSTALL_PATH)/ti/build/Rules.make
else
include $(RULES_MAKE)
endif

APP_NAME = ex04_linux_baremetal_2core_echo_test

SRCS_COMMON = ex04_linux_baremetal_2core_echo_test.c
CFLAGS_LOCAL_COMMON = -DA72_LINUX_OS -DIPC_EXCLUDE_CTRL_TASKS -DBAREMETAL
BUILD_OS_TYPE = baremetal

include ../common/makefile.mk
