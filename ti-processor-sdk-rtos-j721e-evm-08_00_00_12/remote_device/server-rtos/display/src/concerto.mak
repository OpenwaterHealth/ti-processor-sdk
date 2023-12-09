include $(PRELUDE)
ifeq ($(SOC),j721e)
TARGET      := lib_remote_device_display
TARGETTYPE  := library

IDIRS       :=
IDIRS       += $(REMOTE_DEVICE_PATH)

CSOURCES    := app_remote_disp.c

DEFS        :=
DEFS        += SOC_J721E
endif

include $(FINALE)
