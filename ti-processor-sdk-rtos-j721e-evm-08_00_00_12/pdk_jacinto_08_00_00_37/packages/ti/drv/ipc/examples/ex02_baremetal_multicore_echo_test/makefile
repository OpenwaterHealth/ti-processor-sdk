#
# This file is the makefile for building IPC multi-core echo test app for baremetal
#
ifeq ($(RULES_MAKE), )
include $(PDK_INSTALL_PATH)/ti/build/Rules.make
else
include $(RULES_MAKE)
endif

APP_NAME = ex02_baremetal_multicore_echo_test

SRCS_COMMON = ex02_baremetal_multicore_echo_test.c
BUILD_OS_TYPE = baremetal

include ../common/makefile.mk
