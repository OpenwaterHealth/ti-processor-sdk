
TIOVX_PATH ?= $(abspath .)
PSDK_PATH ?= $(abspath ..)

CUSTOM_KERNEL_PATH ?= $(TIOVX_PATH)/kernels_j7


VXLIB_PATH ?= $(PSDK_PATH)/vxlib_c66x_1_1_5_0
J7_C_MODELS_PATH ?= $(PSDK_PATH)/j7_c_models
TIDL_PATH ?= $(PSDK_PATH)/tidl_j7_08_00_00_10/ti_dl
IVISION_PATH ?= $(PSDK_PATH)/ivision
IMAGING_PATH ?= $(PSDK_PATH)/imaging
MMALIB_PATH ?= $(PSDK_PATH)/mmalib_02_01_00_07

TIARMCGT_ROOT ?= $(PSDK_PATH)/ti-cgt-arm_20.2.0.LTS
CGT7X_ROOT ?= $(PSDK_PATH)/ti-cgt-c7000_1.4.2.LTS
CGT6X_ROOT ?= $(PSDK_PATH)/ti-cgt-c6000_8.3.7
GCC_SYSBIOS_ARM_ROOT ?= $(PSDK_PATH)/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf
GCC_LINUX_ARM_ROOT ?= $(PSDK_PATH)/gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu
BIOS_PATH ?= $(PSDK_PATH)/bios_6_83_02_07
XDCTOOLS_PATH ?= $(PSDK_PATH)/xdctools_3_61_04_40_core
PDK_PATH ?= $(PSDK_PATH)/pdk_jacinto_08_00_00_37
PDK_QNX_PATH ?= $(PSDK_PATH)/psdkqa/pdk
VISION_APPS_PATH ?= $(PSDK_PATH)/vision_apps
MATHLIB_PATH     ?= $(PSDK_PATH)/mathlib_c66x_3_1_2_1

BUILD_OS ?= Linux

ifneq ($(CUSTOM_KERNEL_PATH),)
include $(CUSTOM_KERNEL_PATH)/custom_tools_path.mak
endif
ifneq ($(CUSTOM_APPLICATION_PATH),)
include $(CUSTOM_APPLICATION_PATH)/custom_tools_path.mak
endif

ifeq ($(BUILD_OS),Linux)
GCC_LINUX_ROOT ?= /usr/
endif
