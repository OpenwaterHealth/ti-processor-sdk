ifeq ($(TARGET_CPU),$(filter $(TARGET_CPU), x86_64 A72))

include $(PRELUDE)

TARGET      := vx_app_dual_graph

CSOURCES    := main.c 
CSOURCES    += app_histo_module.c
CSOURCES    += app_cust_cap_module.c

ifeq ($(TARGET_CPU),x86_64)

TARGETTYPE  := exe

CSOURCES    += main_x86.c

include $(VISION_APPS_PATH)/apps/concerto_x86_64_inc.mak

endif

ifeq ($(TARGET_CPU),A72)
ifeq ($(TARGET_OS),$(filter $(TARGET_OS), LINUX QNX))
TARGETTYPE  := exe

CSOURCES    += main_linux_arm.c

include $(VISION_APPS_PATH)/apps/concerto_a72_inc.mak
IDIRS += $(IMAGING_IDIRS)
IDIRS += $(VISION_APPS_KERNELS_IDIRS)
IDIRS += $(VISION_APPS_MODULES_IDIRS)
IDIRS += $(VISION_APPS_PATH)/apps/basic_demos/app_openwater/include

STATIC_LIBS += $(IMAGING_LIBS)
STATIC_LIBS += $(VISION_APPS_KERNELS_LIBS)
STATIC_LIBS += $(VISION_APPS_MODULES_LIBS)

endif
endif

include $(FINALE)

endif
