ifeq ($(TARGET_CPU), $(filter $(TARGET_CPU), x86_64 A72))

include $(PRELUDE)
TARGET      := vx_applib_srv_bowl_lut_gen
TARGETTYPE  := library
CSOURCES    := $(call all-c-files)

IDIRS       += $(VISION_APPS_PATH)/kernels/srv/include
IDIRS       += $(PTK_PATH)/include

include $(FINALE)

endif
