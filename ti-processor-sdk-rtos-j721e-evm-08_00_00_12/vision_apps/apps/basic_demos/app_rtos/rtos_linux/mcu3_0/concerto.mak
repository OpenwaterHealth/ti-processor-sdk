ifeq ($(BUILD_APP_RTOS_LINUX),yes)
ifeq ($(BUILD_CPU_MCU3_0),yes)
ifeq ($(TARGET_CPU),R5F)

include $(PRELUDE)

DEFS+=CPU_mcu3_0 SOC_J721E

TARGET      := vx_app_rtos_linux_mcu3_0
TARGETTYPE  := exe
CSOURCES    := main.c gpio_utils.c

ifeq ($(RTOS),SYSBIOS)
	XDC_BLD_FILE = $($(_MODULE)_SDIR)/../../bios_cfg/config_r5f.bld
	XDC_IDIRS    = $($(_MODULE)_SDIR)/../../bios_cfg/
	XDC_CFG_FILE = $($(_MODULE)_SDIR)/mcu3_0.cfg
	XDC_PLATFORM = "ti.platforms.cortexR:J7ES_MAIN"
	LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/linker_mem_map.cmd
	LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/linker.cmd
endif
ifeq ($(RTOS),FREERTOS)
	CSOURCES += ../../common/mpu_cfg/$(SOC)_mpu_cfg.c
	LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/$(SOC)_linker_mem_map_freertos.cmd
	LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/$(SOC)_linker_freertos.cmd
endif

IDIRS+=$(VISION_APPS_PATH)/apps/basic_demos/app_rtos/rtos_linux

LDIRS += $(PDK_PATH)/packages/ti/drv/ipc/lib/$(SOC)/mcu3_0/$(TARGET_BUILD)/
LDIRS += $(PDK_PATH)/packages/ti/drv/udma/lib/$(SOC)/mcu3_0/$(TARGET_BUILD)/
LDIRS += $(PDK_PATH)/packages/ti/drv/sciclient/lib/$(SOC)/mcu3_0/$(TARGET_BUILD)/
LDIRS += $(PDK_PATH)/packages/ti/drv/gpio/lib/$(SOC)/r5f/$(TARGET_BUILD)/

ifeq ($(RTOS),FREERTOS)
	LDIRS += $(PDK_PATH)/packages/ti/kernel/lib/$(SOC)/mcu3_0/$(TARGET_BUILD)/
endif

include $($(_MODULE)_SDIR)/../../concerto_r5f_inc.mak

# CPU instance specific libraries
STATIC_LIBS += app_rtos_common_mcu3_0
STATIC_LIBS += app_rtos_linux
STATIC_LIBS += app_utils_sciclient
//STATIC_LIBS += app_utils_d3board

ADDITIONAL_STATIC_LIBS += sciclient.aer5f
ADDITIONAL_STATIC_LIBS += ti.drv.gpio.aer5f

DEFS        += $(RTOS)

include $(FINALE)

endif
endif
endif
