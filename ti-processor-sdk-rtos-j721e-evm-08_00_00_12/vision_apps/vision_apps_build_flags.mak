
ifndef $(VISION_APPS_BUILD_FLAGS_MAK)
VISION_APPS_BUILD_FLAGS_MAK = 1

# Used to version control the tivision_apps.so and ipk files
PSDK_VERSION?=8.0.0

# Edit below file in tiovx/ to change additional build options
include $(TIOVX_PATH)/build_flags.mak

# Build specific CPUs
BUILD_CPU_MPU1?=yes
BUILD_CPU_MCU1_0?=no
BUILD_CPU_MCU2_0?=yes
BUILD_CPU_MCU2_1?=yes
BUILD_CPU_MCU3_0?=yes
BUILD_CPU_MCU3_1?=no
BUILD_CPU_C6x_1?=yes
BUILD_CPU_C6x_2?=yes
BUILD_CPU_C7x_1?=yes

BUILD_ENABLE_ETHFW?=yes

# If set to no, then MCU core firmware will be built with NO board dependencies
# (such as I2C, board specific PINMUX, DSS, HDMI, I2C, ETHFW, CSIRX, CSITX).  Most of
# the packaged vision_apps require these interfaces on the MCU for the EVM, but
# when porting to a board other than an EVM, or using applications which control
# these interfaces from the HLOS on A72 (such as EdgeAI kit), then this should be set
# to 'no'.
BUILD_MCU_BOARD_DEPENDENCIES?=yes

ifeq ($(BUILD_MCU_BOARD_DEPENDENCIES),no)
BUILD_ENABLE_ETHFW=no
endif

# Need to export this variable so that the following xdc .cfg file can pick this up from the env:
# ${PSDK_PATH}/vision_apps/apps/basic_demos/app_rtos/rtos_linux/mcu2_0/mcu2_0.cfg
export BUILD_ENABLE_ETHFW

# A72 OS specific Build flag
BUILD_LINUX_A72?=yes
BUILD_QNX_A72?=no

# Set to 'yes' to link all .out files against libtivision_apps.so instead of static libs
# (Only supported on A72, ignored on x86_64)
LINK_SHARED_OBJ?=no

# Since MCU R5F runs in locked step mode in vision apps, dont set these to 'yes'
BUILD_CPU_MCU1_1?=no

# Build TI-RTOS fileio binaries
BUILD_APP_RTOS_FILEIO?=no

# Build RTOS + Linux binaries
BUILD_APP_RTOS_LINUX?=$(BUILD_LINUX_A72)

# Build RTOS + QNX binaries
BUILD_APP_RTOS_QNX?=$(BUILD_QNX_A72)

# PDK board to build for, valid values: j721e_sim j721e_evm d3_rvp
BUILD_PDK_BOARD=d3_rvp

# Flag to select silicon revision: 1_1 for ES 1.1, 1_0 for ES 1.0
J7ES_SR?=1_1

# when mcu2-1 build is enabled, mcu2-0 must also be built
ifeq ($(BUILD_CPU_MCU2_1),yes)
BUILD_CPU_MCU2_0=yes
endif

# Build a specific CPU type's based on CPU flags status defined above
ifneq (,$(filter yes,$(BUILD_CPU_MCU1_0) $(BUILD_CPU_MCU1_1) $(BUILD_CPU_MCU2_0) $(BUILD_CPU_MCU2_1) $(BUILD_CPU_MCU3_0) $(BUILD_CPU_MCU3_1)))
BUILD_ISA_R5F=yes
else
BUILD_ISA_R5F=no
endif
ifneq (,$(filter yes,$(BUILD_CPU_C6x_1) $(BUILD_CPU_C6x_2)))
BUILD_ISA_C6x=yes
else
BUILD_ISA_C6x=no
endif
ifneq (,$(filter yes,$(BUILD_CPU_C7x_1)))
BUILD_ISA_C7x=yes
else
BUILD_ISA_C7x=no
endif
ifneq (,$(filter yes,$(BUILD_CPU_MPU1)))
BUILD_ISA_A72=yes
else
BUILD_ISA_A72=no
endif

endif # ifndef $(VISION_APPS_BUILD_FLAGS_MAK)
