ifeq ($(BUILD_CPU_MCU2_1),yes)
ifeq ($(TARGET_CPU),R5F)

CPU_ID=mcu2_1

TARGET      := app_display_sharing
TARGETTYPE  := exe

IDIRS       :=
IDIRS       += $(REMOTE_DEVICE_PATH)

CSOURCES    := main_tirtos.c app_dss.c app_dss_data.c

ifeq ($(RTOS),SYSBIOS)
	XDC_BLD_FILE = $($(_MODULE)_SDIR)/mcu2_1_sysbios.bld
	XDC_CFG_FILE = $($(_MODULE)_SDIR)/mcu2_1_sysbios.cfg
	XDC_PLATFORM = "ti.platforms.cortexR:J7ES_MAIN"
	LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/mcu2_1_sysbios.lds
endif
ifeq ($(RTOS),FREERTOS)
	CSOURCES += ipc_trace.c
	CSOURCES += mcu2_1_mpu_cfg.c
	LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/mcu2_1_freertos.lds
endif

LDIRS += $(PDK_PATH)/packages/ti/csl/lib/$(SOC)/r5f/$(TARGET_BUILD)/
LDIRS += $(PDK_PATH)/packages/ti/drv/ipc/lib/$(SOC)/$(CPU_ID)/$(TARGET_BUILD)/
LDIRS += $(PDK_PATH)/packages/ti/drv/dss/lib/$(SOC)/$(CPU_ID)/$(TARGET_BUILD)/
LDIRS += $(PDK_PATH)/packages/ti/drv/fvid2/lib/$(SOC)/r5f/$(TARGET_BUILD)/
LDIRS += $(PDK_PATH)/packages/ti/board/lib/$(BOARD)/r5f/$(TARGET_BUILD)
LDIRS += $(PDK_PATH)/packages/ti/drv/i2c/lib/$(SOC)/r5f/$(TARGET_BUILD)
LDIRS += $(PDK_PATH)/packages/ti/drv/uart/lib/$(SOC)/r5f/$(TARGET_BUILD)
LDIRS += $(PDK_PATH)/packages/ti/drv/sciclient/lib/$(SOC)/$(CPU_ID)/$(TARGET_BUILD)
ifeq ($(RTOS),SYSBIOS)
	LDIRS += $(PDK_PATH)/packages/ti/osal/lib/tirtos/$(SOC)/r5f/$(TARGET_BUILD)/
endif
ifeq ($(RTOS),FREERTOS)
	LDIRS += $(PDK_PATH)/packages/ti/kernel/lib/$(SOC)/$(CPU_ID)/$(TARGET_BUILD)/
	LDIRS += $(PDK_PATH)/packages/ti/osal/lib/freertos/$(SOC)/r5f/$(TARGET_BUILD)/
endif

SYS_STATIC_LIBS += rtsv7R4_T_le_v3D16_eabi

STATIC_LIBS += lib_remote_device
STATIC_LIBS += lib_remote_device_display

ifeq ($(RTOS),FREERTOS)
	ADDITIONAL_STATIC_LIBS += ti.kernel.freertos.aer5f
	ADDITIONAL_STATIC_LIBS += ti.csl.init.aer5f
endif
ADDITIONAL_STATIC_LIBS += ti.osal.aer5f
ADDITIONAL_STATIC_LIBS += ti.csl.aer5f
ADDITIONAL_STATIC_LIBS += ipc.aer5f
ADDITIONAL_STATIC_LIBS += dss.aer5f
ADDITIONAL_STATIC_LIBS += fvid2.aer5f
ADDITIONAL_STATIC_LIBS += ti.board.aer5f
ADDITIONAL_STATIC_LIBS += ti.board.aer5f
ADDITIONAL_STATIC_LIBS += ti.drv.i2c.aer5f
ADDITIONAL_STATIC_LIBS += ti.drv.uart.aer5f
ADDITIONAL_STATIC_LIBS += sciclient.aer5f

DEFS        :=
DEFS        += SOC_J721E
DEFS        += BUILD_MCU2_1
DEFS        += A72_LINUX_OS
DEFS        += $(RTOS)

endif
endif

