#
# This file is the makefile for building IPC r5f-r5f test app for TI RTOS
#
ifeq ($(RULES_MAKE), )
include $(PDK_INSTALL_PATH)/ti/build/Rules.make
else
include $(RULES_MAKE)
endif

APP_NAME = ipc_echo_test_$(BUILD_OS_TYPE)

SRCS_COMMON = ipc_echo_test.c
CFLAGS_LOCAL_COMMON = -DA72_LINUX_OS # -DA72_LINUX_OS_IPC_ATTACH

include ../common/makefile.mk

