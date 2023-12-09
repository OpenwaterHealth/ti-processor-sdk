ifeq ($(TARGET_PLATFORM),J7)
ifeq ($(TARGET_OS),$(filter $(TARGET_OS),SYSBIOS FREERTOS))

include $(PRELUDE)

TARGET      := app_utils_misc
TARGETTYPE  := library

ifeq ($(SOC),j721e)
DEFS+=SOC_J721E
endif

ifeq ($(TARGET_CPU),C71)
ASSEMBLY += app_c7x_init_asm.asm
CSOURCES += app_c7x_init.c
endif

ifeq ($(TARGET_OS),SYSBIOS)
CSOURCES += app_cpu_hz_tirtos.c
endif

ifeq ($(TARGET_OS),FREERTOS)
IDIRS    += $(PDK_PATH)/packages/ti/kernel/freertos/FreeRTOS-LTS/FreeRTOS-Kernel/include/
CSOURCES += app_cpu_hz_freertos.c
endif

ifeq ($(TARGET_CPU),R5F)

IDIRS    += $(PDK_PATH)/packages/ti/kernel/freertos/portable/TI_CGT/r5f
IDIRS    += $(PDK_PATH)/packages/ti/kernel/freertos/config/$(SOC)/r5f

CSOURCES += app_r5f_init.c

ifeq ($(BUILD_PDK_BOARD), j721e_evm)
CSOURCES += app_pinmux.c
DEFS+=j721e_evm
endif

ifeq ($(BUILD_PDK_BOARD), d3_rvp)
CSOURCES += app_pinmux.c
DEFS+=d3_rvp

endif

endif

include $(FINALE)

endif
endif
