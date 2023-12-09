ifeq ($(TARGET_CPU),$(filter $(TARGET_CPU), x86_64 C71))

include $(PRELUDE)
TARGET      := vx_app_multi_cam_histo_c7x_target_kernel
TARGETTYPE  := library
CSOURCES    := $(call all-c-files)
CPPSOURCES  := $(call all-cpp-files)

IDIRS       += $(VISION_APPS_PATH)/utils/ipc/include
IDIRS       += $(VISION_APPS_PATH)/utils/remote_service/include
IDIRS       += $(VISION_APPS_PATH)/apps/basic_demos/app_openwater/include
IDIRS       += $(VISION_APPS_PATH)/utils/perf_stats/include

ifeq ($(TARGET_CPU), x86_64)
IDIRS       += $(CGT7X_ROOT)/host_emulation/include/C7100
CFLAGS += --std=c++14 -D_HOST_EMULATION -pedantic -fPIC -w -c -g
CFLAGS += -Wno-sign-compare
endif


include $(FINALE)

endif
