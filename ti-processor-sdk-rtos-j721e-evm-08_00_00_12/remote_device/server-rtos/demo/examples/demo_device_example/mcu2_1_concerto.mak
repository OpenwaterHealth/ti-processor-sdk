ifeq ($(BUILD_CPU_MCU2_1),yes)
ifeq ($(TARGET_CPU),R5F)

CPU_ID=mcu2_1

TARGET      := app_demo_device_example
TARGETTYPE  := exe

IDIRS       :=
IDIRS       += $(REMOTE_DEVICE_PATH)

CSOURCES    := main_rtos.c

ifeq ($(RTOS),SYSBIOS)
	XDC_BLD_FILE = $($(_MODULE)_SDIR)/mcu2_1_sysbios.bld
	ifeq ($(SOC),j721e)
		XDC_CFG_FILE = $($(_MODULE)_SDIR)/j721e_mcu2_1_sysbios.cfg
		XDC_PLATFORM = "ti.platforms.cortexR:J7ES_MAIN"
		LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/j721e_mcu2_1_sysbios.lds
	endif
	ifeq ($(SOC),j7200)
		XDC_CFG_FILE = $($(_MODULE)_SDIR)/j7200_mcu2_1_sysbios.cfg
		XDC_PLATFORM = "ti.platforms.cortexR:J7200_MAIN"
		LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/j7200_mcu2_1_sysbios.lds
	endif
endif
ifeq ($(RTOS),FREERTOS)
	CSOURCES += ipc_trace.c
	ifeq ($(SOC),j721e)
		CSOURCES += j721e_mcu2_1_mpu_cfg.c
		LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/j721e_mcu2_1_freertos.lds
	endif
	ifeq ($(SOC),j7200)
		CSOURCES += j7200_mcu2_1_mpu_cfg.c
		LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/j7200_mcu2_1_freertos.lds
	endif
endif

LDIRS += $(PDK_PATH)/packages/ti/csl/lib/$(SOC)/r5f/$(TARGET_BUILD)/
LDIRS += $(PDK_PATH)/packages/ti/drv/ipc/lib/$(SOC)/$(CPU_ID)/$(TARGET_BUILD)/
LDIRS += $(PDK_PATH)/packages/ti/drv/sciclient/lib/$(SOC)/$(CPU_ID)/$(TARGET_BUILD)/
ifeq ($(RTOS),SYSBIOS)
	LDIRS += $(PDK_PATH)/packages/ti/osal/lib/tirtos/$(SOC)/r5f/$(TARGET_BUILD)/
endif
ifeq ($(RTOS),FREERTOS)
	LDIRS += $(PDK_PATH)/packages/ti/kernel/lib/$(SOC)/$(CPU_ID)/$(TARGET_BUILD)/
	LDIRS += $(PDK_PATH)/packages/ti/osal/lib/freertos/$(SOC)/r5f/$(TARGET_BUILD)/
endif

SYS_STATIC_LIBS += rtsv7R4_T_le_v3D16_eabi

STATIC_LIBS += lib_remote_device
STATIC_LIBS += lib_remote_device_demo

ifeq ($(RTOS),FREERTOS)
	ADDITIONAL_STATIC_LIBS += ti.kernel.freertos.aer5f
	ADDITIONAL_STATIC_LIBS += ti.csl.init.aer5f
endif
ADDITIONAL_STATIC_LIBS += ti.osal.aer5f
ADDITIONAL_STATIC_LIBS += ti.csl.aer5f
ADDITIONAL_STATIC_LIBS += ipc.aer5f
ADDITIONAL_STATIC_LIBS += sciclient.aer5f

DEFS        :=
ifeq ($(SOC),j721e)
DEFS        += SOC_J721E
endif
ifeq ($(SOC),j7200)
DEFS        += SOC_J7200
endif
DEFS        += BUILD_MCU2_1
DEFS        += A72_LINUX_OS
DEFS        += $(RTOS)

endif
endif

