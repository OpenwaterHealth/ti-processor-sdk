ifeq ($(TARGET_PLATFORM),J7)

include $(PRELUDE)
TARGET      := app_utils_mem
TARGETTYPE  := library

ifeq ($(TARGET_OS),SYSBIOS)
CSOURCES    := app_mem_tirtos.c
endif

ifeq ($(TARGET_OS),FREERTOS)
CSOURCES    := app_mem_free_rtos.c
endif

ifeq ($(TARGET_OS),LINUX)
CSOURCES     := app_mem_linux_dma_heap.c
endif

ifeq ($(TARGET_OS),QNX)
IDIRS += $(PSDK_QNX_PATH)/qnx/sharedmemallocator/usr/public
IDIRS += $(PSDK_QNX_PATH)/qnx/sharedmemallocator/resmgr/public
CSOURCES    := app_mem_qnx.c
endif

include $(FINALE)

endif

ifeq ($(TARGET_CPU),x86_64)

include $(PRELUDE)
TARGET      := app_utils_mem
TARGETTYPE  := library

CSOURCES    := app_mem_pc.c

IDIRS       += $(VISION_APPS_PATH)

include $(FINALE)

endif
