include $(PRELUDE)
TARGET      := lib_remote_device_demo
TARGETTYPE  := library

IDIRS       :=
IDIRS       += $(REMOTE_DEVICE_PATH)

CSOURCES    := app_remote_demo.c

DEFS        :=
ifeq ($(SOC),j721e)
DEFS        += SOC_J721E
endif
ifeq ($(SOC),j7200)
DEFS        += SOC_J7200
endif

include $(FINALE)
