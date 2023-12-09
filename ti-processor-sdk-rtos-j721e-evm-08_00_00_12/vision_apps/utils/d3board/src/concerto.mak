ifeq ($(TARGET_PLATFORM),J7)
ifeq ($(TARGET_OS),$(filter $(TARGET_OS),SYSBIOS FREERTOS))
ifeq ($(TARGET_CPU),R5F)

include $(PRELUDE)
TARGET      := app_utils_d3board
TARGETTYPE  := library

IDIRS       +=$(PDK_PATH)/packages
IDIRS    	+= $(PDK_PATH)/packages/ti/kernel/freertos/FreeRTOS-LTS/FreeRTOS-Kernel/include
IDIRS    	+= $(PDK_PATH)/packages/ti/kernel/freertos/config/j721e/r5f
IDIRS    	+= $(PDK_PATH)/packages/ti/kernel/freertos/portable/TI_CGT/r5f
CSOURCES    := app_d3board.c app_d3board_gpio.c app_d3board_fsyncTimer.c

ifeq ($(SOC),j721e)
DEFS+=SOC_J721E
endif

include $(FINALE)

endif
endif
endif

