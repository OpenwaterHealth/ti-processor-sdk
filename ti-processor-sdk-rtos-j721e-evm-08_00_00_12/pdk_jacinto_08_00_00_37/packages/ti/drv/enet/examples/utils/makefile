#
# Example utils (TI-RTOS) makefile
#

ifeq ($(RULES_MAKE), )
include $(PDK_INSTALL_PATH)/ti/build/Rules.make
else
include $(RULES_MAKE)
endif
include $(PDK_INSTALL_PATH)/ti/drv/enet/examples/build/config.mk


# Two flavors of the enet_example_utils_<OS_TYPE> are built depending on the target app
# to bypass I2C, QSGMII and UART conflicts when integrating EthFw with HLOS
# running on MPU1_0.  So, below table is applicable when building this
# library for MCU2_0 which is the only core that supports EthFw.
# Here the <OS_TYPE> could tirtos, freertos or safertos.
#
#               | Bypass | Bypass |   UART  |
#      App      |  I2C   | QSGMII | allowed | Library
# --------------+--------+--------+---------+----------------
#   EthFw J721E |   Yes  |   Yes  |   Yes   | enet_example_utils_<OS_TYPE>
#   EthFw J7200 |   Yes  |   No   |   No    | enet_example_utils_<OS_TYPE>
# CPSW examples |   No   |   No   |   Yes   | enet_example_utils_full_<OS_TYPE>

ifeq ($(BUILD_OS_TYPE),baremetal)
  MODULE_NAME = enet_example_utils$(ENET_OS_TYPE_POSTFIX)
  ENET_CFLAGS += -DENETAPPUTILS_UART_ALLOWED
endif
ifeq ($(BUILD_OS_TYPE),tirtos)
  ifeq ($(APPUTILS_TYPE),full)
    MODULE_NAME = enet_example_utils_full$(ENET_OS_TYPE_POSTFIX)
    ENET_CFLAGS += -DENETAPPUTILS_UART_ALLOWED
  else
    MODULE_NAME = enet_example_utils$(ENET_OS_TYPE_POSTFIX)
    ifeq ($(CORE),$(filter $(CORE), mcu2_0))
      ENET_CFLAGS += -DENETAPPUTILS_BYPASS_I2C
      ifeq ($(SOC),$(filter $(SOC), j721e))
        ENET_CFLAGS += -DENETAPPUTILS_BYPASS_QSGMII
      endif
      ifneq ($(SOC),$(filter $(SOC), j7200))
        ENET_CFLAGS += -DENETAPPUTILS_UART_ALLOWED
      endif
    else
      ENET_CFLAGS += -DENETAPPUTILS_UART_ALLOWED
    endif
  endif
endif
ifeq ($(BUILD_OS_TYPE),freertos)
  ifeq ($(APPUTILS_TYPE),full)
    MODULE_NAME = enet_example_utils_full$(ENET_OS_TYPE_POSTFIX)
    ENET_CFLAGS += -DENETAPPUTILS_UART_ALLOWED
  else
    MODULE_NAME = enet_example_utils$(ENET_OS_TYPE_POSTFIX)
    ifeq ($(CORE),$(filter $(CORE), mcu2_0))
      ENET_CFLAGS += -DENETAPPUTILS_BYPASS_I2C
      ifeq ($(SOC),$(filter $(SOC), j721e))
        ENET_CFLAGS += -DENETAPPUTILS_BYPASS_QSGMII
      endif
      ifneq ($(SOC),$(filter $(SOC), j7200))
        ENET_CFLAGS += -DENETAPPUTILS_UART_ALLOWED
      endif
    else
      ENET_CFLAGS += -DENETAPPUTILS_UART_ALLOWED
    endif
  endif
endif

SRCDIR += .
INCDIR += include/

# Common source files and CFLAGS across all platforms and cores
PACKAGE_SRCS_COMMON = .

# Source files: J721E, J7200
ifeq ($(SOC),$(filter $(SOC), j721e j7200))
    SRCS_COMMON += enet_board_j7xevm.c
endif
ifeq ($(SOC),$(filter $(SOC), am65xx))
    SRCS_COMMON += enet_board_am65xevm.c enet_board_am65xevm_pinmux_data.c
endif

# From CPSW LLD
ifeq ($(SOC),$(filter $(SOC), am65xx))
  SRCDIR += V0
endif
ifeq ($(SOC),$(filter $(SOC), j721e j7200))
  SRCDIR += V1
endif
ifeq ($(SOC),$(filter $(SOC), tpr12 awr294x))
  SRCDIR += V2
endif

ifeq ($(BUILD_OS_TYPE),baremetal)
  SRCS_COMMON += enet_apputils_baremetal.c
endif
ifeq ($(BUILD_OS_TYPE),tirtos)
  # List all the external components/interfaces, whose interface header files
  # need to be included for this component
  INCLUDE_EXTERNAL_INTERFACES += xdc bios ndk
  CFLAGS_LOCAL_COMMON = -DNDK_NOUSERAPIS
  ifneq ($(SOC),$(filter $(SOC), tpr12 awr294x))
    SRCS_COMMON += enet_networkutils.c
  endif
  SRCS_COMMON += enet_mcm.c
  SRCS_COMMON += enet_apputils_rtos.c
endif
ifeq ($(BUILD_OS_TYPE),freertos)
  ENET_CFLAGS += -DFREERTOS
  # List all the external components/interfaces, whose interface header files
  # need to be included for this component
  INCLUDE_EXTERNAL_INTERFACES +=
  CFLAGS_LOCAL_COMMON =
  ifneq ($(SOC),$(filter $(SOC), tpr12 awr294x))
    INCLUDE_INTERNAL_INTERFACES += lwipif_$(BUILD_OS_TYPE)
    INCLUDE_INTERNAL_INTERFACES += lwipstack_$(BUILD_OS_TYPE)
    INCLUDE_INTERNAL_INTERFACES += lwipcontrib_$(BUILD_OS_TYPE)
    # OS ports
    INCDIR += $(PDK_ENET_COMP_PATH)/lwipif/ports/$(BUILD_OS_TYPE)/include
    INCDIR += $(PDK_ENET_COMP_PATH)/lwipif/ports/$(BUILD_OS_TYPE)/include/arch
    SRCS_COMMON += enet_networkutils.c
  endif
  SRCS_COMMON += enet_mcm.c
  SRCS_COMMON += enet_apputils_rtos.c
endif

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += enet_apputils.c
SRCS_COMMON += enet_appmemutils.c
SRCS_COMMON += enet_appethpatterns.c
SRCS_COMMON += enet_appsoc.c
SRCS_COMMON += enet_apprmcfg.c
ifeq ($(SOC),$(filter $(SOC), j721e j7200 am65xx))
  SRCS_COMMON += enet_udmautils.c
  SRCS_COMMON += enet_ioctlutils.c
  SRCS_COMMON += enet_apputils_k3.c
endif
ifeq ($(SOC),$(filter $(SOC), tpr12 awr294x))
  SRCS_COMMON += enet_cpdmautils.c
  SRCS_COMMON += enet_ioctlutils.c
endif

PACKAGE_SRCS_COMMON = . ../common ../build/$(SOC)/ ../build/config.mk

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =
SRCS_am65xx_idk += enet_appboardutils_am65xx_evm.c
SRCS_am65xx_evm += enet_appboardutils_am65xx_evm.c
SRCS_j721e_evm  += enet_appboardutils_j721e_evm.c
SRCS_j721e_evm  += enet_board_pinmux.c enet_board_pinmux_j721e_data.c
SRCS_j7200_evm  += enet_appboardutils_j721e_evm.c
SRCS_j7200_evm  += enet_board_pinmux.c enet_board_pinmux_j7200_data.c
SRCS_tpr12_evm  += enet_appboardutils_tpr12_evm.c enet_board_tpr12evm.c
SRCS_awr294x_evm  += enet_appboardutils_awr294x_evm.c enet_board_awr294xevm.c

SRCS_d3_rvp  += enet_appboardutils_j721e_evm.c
SRCS_d3_rvp  += enet_board_pinmux.c enet_board_pinmux_j721e_data.c

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES += pdk
CFLAGS_LOCAL_COMMON += $(PDK_CFLAGS) $(ENET_CFLAGS)

# Include common make files
ifeq ($(MAKERULEDIR), )
  #Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(ROOTDIR)/ti/build/makerules
  export MAKERULEDIR
endif
include $(MAKERULEDIR)/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
