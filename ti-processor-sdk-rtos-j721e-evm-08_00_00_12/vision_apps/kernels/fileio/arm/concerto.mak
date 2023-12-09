
ifeq ($(TARGET_CPU), $(filter $(TARGET_CPU), x86_64 A72 ))

include $(PRELUDE)
TARGET      := vx_target_kernels_fileio
TARGETTYPE  := library

CSOURCES    := vx_kernels_fileio_target.c
CSOURCES    += vx_fileio_write_array_target.c
CSOURCES    += vx_fileio_write_image_target.c
CSOURCES    += vx_fileio_write_raw_image_target.c
CSOURCES    += vx_fileio_write_tensor_target.c
CSOURCES    += vx_fileio_write_user_data_object_target.c
CSOURCES    += vx_fileio_write_histogram_target.c

IDIRS       += $(VISION_APPS_PATH)/kernels/fileio/include
IDIRS       += $(VISION_APPS_PATH)/kernels/fileio/host
IDIRS       += $(VISION_APPS_PATH)/utils/ipc/include
IDIRS       += $(VISION_APPS_PATH)/utils/remote_service/include
IDIRS       += $(VISION_APPS_PATH)/utils/mem/include
IDIRS       += $(VISION_APPS_PATH)/apps/basic_demos/app_openwater/include
IDIRS       += $(VXLIB_PATH)/packages

DEFS        += SOC_J721E

include $(FINALE)

endif
