ifndef $(REMOTE_DEVICE_BUILD_FLAGS_MAK)
REMOTE_DEVICE_BUILD_FLAGS_MAK = 1

# BOARD and SOC selection
SOC?=j721e

ifeq ($(SOC),j721e)
BOARD=j721e_evm
endif
ifeq ($(SOC),j7200)
BOARD=j7200_evm
endif

# Build specific CPUs
BUILD_CPU_MCU1_0?=yes
BUILD_CPU_MCU2_0?=yes
BUILD_CPU_MCU3_0?=yes
BUILD_CPU_MCU1_1?=yes
BUILD_CPU_MCU2_1?=yes
BUILD_CPU_MCU3_1?=yes

# Build a specific CPU type's based on CPU flags status defined above
ifneq (,$(filter yes,$(BUILD_CPU_MCU1_0) $(BUILD_CPU_MCU1_1) $(BUILD_CPU_MCU2_0) $(BUILD_CPU_MCU2_1) $(BUILD_CPU_MCU3_0) $(BUILD_CPU_MCU3_1)))
BUILD_ISA_R5F=yes
else
BUILD_ISA_R5F=no
endif

# RTOS selection - SYSBIOS or FREERTOS
RTOS?=SYSBIOS

endif # ifndef $(REMOTE_DEVICE_BUILD_FLAGS_MAK)
