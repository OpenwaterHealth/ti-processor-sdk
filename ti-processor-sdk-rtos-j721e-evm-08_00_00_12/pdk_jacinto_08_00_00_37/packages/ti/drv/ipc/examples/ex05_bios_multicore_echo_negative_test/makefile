#
# This file is the makefile for building IPC multi-core echo test app for TI RTOS
#
ifeq ($(RULES_MAKE), )
include $(PDK_INSTALL_PATH)/ti/build/Rules.make
else
include $(RULES_MAKE)
endif

APP_NAME = ex05_bios_multicore_echo_negative_test_$(BUILD_OS_TYPE)

SRCS_COMMON += ex05_bios_multicore_echo_negative_test.c ipc_neg_testsetup.c

CFLAGS_LOCAL_COMMON += -DIPC_NEGATIVE_TEST

include ../common/makefile.mk
