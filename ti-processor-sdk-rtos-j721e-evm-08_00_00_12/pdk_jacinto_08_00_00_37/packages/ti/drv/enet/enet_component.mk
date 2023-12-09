# File: enet_component.mk
#       This file is component include make file of XYZ driver library.
# List of variables set in this file and their purpose:
# <mod>_RELPATH        - This is the relative path of the module, typically from
#                        top-level directory of the package
# <mod>_PATH           - This is the absolute path of the module. It derives from
#                        absolute path of the top-level directory (set in env.mk)
#                        and relative path set above
# <mod>_INCLUDE        - This is the path that has interface header files of the
#                        module. This can be multiple directories (space separated)
# <mod>_PKG_LIST       - Names of the modules (and sub-modules) that are a part
#                        part of this module, including itself.
# <mod>_BOARD_DEPENDENCY - "yes": means the code for this module depends on
#                             platform and the compiled obj/lib has to be kept
#                             under <platform> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no platform dependent code and hence
#                             the obj/libs are not kept under <platform> dir.
# <mod>_CORE_DEPENDENCY     - "yes": means the code for this module depends on
#                             core and the compiled obj/lib has to be kept
#                             under <core> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no core dependent code and hence
#                             the obj/libs are not kept under <core> dir.
# <mod>_APP_STAGE_FILES     - List of source files that belongs to the module
#                             <mod>, but that needs to be compiled at application
#                             build stage (in the context of the app). This is
#                             primarily for link time configurations or if the
#                             source file is dependent on options/defines that are
#                             application dependent. This can be left blank or
#                             not defined at all, in which case, it means there
#                             no source files in the module <mod> that are required
#                             to be compiled in the application build stage.
#
ifeq ($(enet_component_make_include), )

# List with various rtos_types(such as tirtos(sysbios),freertos,safertos) to build RTOS apps.
# Use the Default List defined in 'ti/build/makerules/component.mk'
# This list will be used to generate RTOS app make rule for each rtos_type.
drvenet_RTOS_LIST       = tirtos freertos

drvenet_SOCLIST         = am65xx j721e j7200 tpr12 awr294x
drvenet_BOARDLIST       = am65xx_evm am65xx_idk j721e_evm j7200_evm tpr12_evm awr294x_evm
drvenet_am65xx_CORELIST = mpu1_0 mcu1_0
drvenet_j721e_CORELIST  = mpu1_0 mcu1_0 mcu2_0 mcu2_1
drvenet_j7200_CORELIST  = mpu1_0 mcu1_0 mcu2_0 mcu2_1
drvenet_tpr12_CORELIST  = mcu1_0 mcu1_1
drvenet_awr294x_CORELIST  = mcu1_0 mcu1_1

# Temp SoC/board lists which excludes tpr12, awr294x for libraries/apps which
# are not enabled yet
drvenet_K3_SOCLIST      = am65xx j721e j7200
drvenet_K3_BOARDLIST    = am65xx_evm am65xx_idk j721e_evm j7200_evm

# List of cores where LwIP is supported (i.e. exclude A cores)
drvenet_lwip_CORELIST  = $(filter-out mpu1_0, ${drvenet_$(SOC)_CORELIST})


#
# Enet RTOS boardlist rule
#
# Define the rule to generate Enet Drivers BOARDLIST for each rtos_type
# Default BOARDLIST for each rtos_type is defined in 'ti/build/makerules/component.mk'
# The following rule filters out Enet Drivers BOARDLIST for each rtos_type.
# Here $(1) refers to the first argument passed to the rule.
# In this case it is $(curos), each instance in "drvenet_RTOS_LIST" (ie, tirtos/freertos/safertos..)
#
define DRV_ENET_RTOS_BOARDLIST_RULE

drvenet_$(1)_BOARDLIST = $(filter $(DEFAULT_BOARDLIST_$(1)), $(drvenet_BOARDLIST))

endef

# Define the macro list with rules of all rtos_types
DRV_ENET_RTOS_BOARDLIST_MACRO_LIST := $(foreach curos, $(drvenet_RTOS_LIST), $(call DRV_ENET_RTOS_BOARDLIST_RULE,$(curos)))

# Evaluate the macro list to generate BOARDLIST for all rtos_types
$(eval ${DRV_ENET_RTOS_BOARDLIST_MACRO_LIST})


#
# Enet RTOS boardlist rule
# Temp SoC/board lists which excludes tpr12, awr294x for libraries/apps which
# are not enabled yet
#
define DRV_ENET_K3_RTOS_BOARDLIST_RULE

drvenet_K3_$(1)_BOARDLIST = $(filter $(DEFAULT_BOARDLIST_$(1)), $(drvenet_K3_BOARDLIST))

endef

DRV_ENET_K3_RTOS_BOARDLIST_MACRO_LIST := $(foreach curos, $(drvenet_RTOS_LIST), $(call DRV_ENET_K3_RTOS_BOARDLIST_RULE,$(curos)))

$(eval ${DRV_ENET_K3_RTOS_BOARDLIST_MACRO_LIST})


############################
# enet package
# List of components included under enet lib
# The components included here are built and will be part of enet lib
############################
enet_LIB_LIST =

############################
# enet app lib package
# List of components included under enet app lib
# The components included here are built and will be part of enet app lib
############################
enet_APP_LIB_LIST =

############################
# enet examples
# List of examples under enet (+= is used at each example definition)
# All the tests mentioned in list are built when test target is called
# List below all examples for allowed values
############################
enet_EXAMPLE_LIST =

#
# Enet Modules
#

#
# Enet library
#
export enet_COMP_LIST = enet
enet_RELPATH = ti/drv/enet
enet_PATH = $(PDK_ENET_COMP_PATH)
export enet_LIBNAME = enet
export enet_LIBPATH = $(PDK_ENET_COMP_PATH)/lib
export enet_MAKEFILE = -fsrc/makefile
export enet_BOARD_DEPENDENCY = no
export enet_CORE_DEPENDENCY = no
enet_PKG_LIST = enet
enet_INCLUDE = $(enet_PATH)
export enet_SOCLIST = $(drvenet_SOCLIST)
export enet_$(SOC)_CORELIST = $(drvenet_$(SOC)_CORELIST)
enet_LIB_LIST += enet

#
# Enet SoC library
#
export enetsoc_COMP_LIST = enetsoc
enetsoc_RELPATH = ti/drv/enet/soc
enetsoc_PATH = $(PDK_ENET_COMP_PATH)/soc
export enetsoc_LIBNAME = enetsoc
export enetsoc_LIBPATH = $(PDK_ENET_COMP_PATH)/lib
export enetsoc_MAKEFILE = -fmakefile
export enetsoc_BOARD_DEPENDENCY = no
export enetsoc_CORE_DEPENDENCY = yes
enetsoc_PKG_LIST = enetsoc
enetsoc_INCLUDE = $(enetsoc_PATH)
export enetsoc_SOCLIST = $(drvenet_SOCLIST)
export enetsoc_$(SOC)_CORELIST = $(drvenet_$(SOC)_CORELIST)
enet_LIB_LIST += enetsoc

#
# Enet PHY library
#
export enetphy_COMP_LIST = enetphy
enetphy_RELPATH = ti/drv/enet/src/phy
enetphy_PATH = $(PDK_ENET_COMP_PATH)/src/phy
export enetphy_LIBNAME = enetphy
export enetphy_LIBPATH = $(PDK_ENET_COMP_PATH)/lib
export enetphy_MAKEFILE = -fmakefile
export enetphy_BOARD_DEPENDENCY = no
export enetphy_CORE_DEPENDENCY = yes
enetphy_PKG_LIST = enetphy
enetphy_INCLUDE = $(enetphy_PATH)
export enetphy_SOCLIST = $(drvenet_SOCLIST)
export enetphy_$(SOC)_CORELIST = $(drvenet_$(SOC)_CORELIST)
enet_LIB_LIST += enetphy

#
# NIMUENET library
#
export nimuenet_COMP_LIST = nimuenet
nimuenet_RELPATH = ti/drv/enet/nimuenet
nimuenet_PATH = $(PDK_ENET_COMP_PATH)/nimuenet
export nimuenet_LIBNAME = nimuenet
export nimuenet_LIBPATH = $(PDK_ENET_COMP_PATH)/lib
export nimuenet_MAKEFILE = -fmakefile
export nimuenet_BOARD_DEPENDENCY = no
export nimuenet_CORE_DEPENDENCY = no
nimuenet_PKG_LIST = nimuenet
nimuenet_INCLUDE = $(nimuenet_PATH)
export nimuenet_SOCLIST = $(drvenet_K3_SOCLIST)
export nimuenet_$(SOC)_CORELIST = $(drvenet_$(SOC)_CORELIST)
ifneq ($(SOC),$(filter $(SOC), tpr12 awr294x))
enet_LIB_LIST += nimuenet
endif

#
# lwipif interface library
#
define lwipif_RULE

export lwipif_$(1)_COMP_LIST = lwipif_$(1)
lwipif_$(1)_RELPATH = ti/drv/enet/lwipif
lwipif_$(1)_PATH = $(PDK_ENET_COMP_PATH)/lwipif
export lwipif_$(1)_LIBNAME = lwipif_$(1)
export lwipif_$(1)_LIBPATH = $(PDK_ENET_COMP_PATH)/lib/$(1)
export lwipif_$(1)_OBJPATH = ti/drv/enet/lwipif/$(1)
export lwipif_$(1)_MAKEFILE = -f makefile BUILD_OS_TYPE=$(1)
export lwipif_$(1)_BOARD_DEPENDENCY = no
export lwipif_$(1)_CORE_DEPENDENCY = no
lwipif_$(1)_PKG_LIST = lwipif_$(1)
lwipif_$(1)_INCLUDE = $(PDK_ENET_COMP_PATH)/lwipif/inc
export lwipif_$(1)_SOCLIST = $(drvenet_SOCLIST)
export lwipif_$(1)_$(SOC)_CORELIST = $(filter $(DEFAULT_$(SOC)_CORELIST_$(1)), $(drvenet_lwip_CORELIST))
ifneq ($(1),$(filter $(1), safertos))
enet_LIB_LIST += lwipif_$(1)
else
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
enet_LIB_LIST += lwipif_$(1)
endif
endif

endef

lwipif_MACRO_LIST := $(foreach curos,$(drvenet_RTOS_LIST),$(call lwipif_RULE,$(curos)))
$(eval ${lwipif_MACRO_LIST})

#
# Enet config server library
#
enet_cfgserver_COMP_LIST = enet_cfgserver
enet_cfgserver_RELPATH = ti/drv/enet/enet_cfgserver
enet_cfgserver_PATH = $(PDK_ENET_COMP_PATH)/enet_cfgserver
enet_cfgserver_LIBNAME = enet_cfgserver
enet_cfgserver_LIBPATH = $(PDK_ENET_COMP_PATH)/lib
enet_cfgserver_MAKEFILE = -fmakefile
export enet_cfgserver_MAKEFILE
export enet_cfgserver_LIBNAME
export enet_cfgserver_LIBPATH
enet_cfgserver_BOARD_DEPENDENCY = no
enet_cfgserver_CORE_DEPENDENCY = no
export enet_cfgserver_COMP_LIST
export enet_cfgserver_BOARD_DEPENDENCY
export enet_cfgserver_CORE_DEPENDENCY
enet_cfgserver_PKG_LIST = enet_cfgserver
enet_cfgserver_INCLUDE = $(enet_cfgserver_PATH)
enet_cfgserver_SOCLIST = $(drvenet_SOCLIST)
export enet_cfgserver_SOCLIST
enet_cfgserver_$(SOC)_CORELIST = $(drvenet_$(SOC)_CORELIST)
export enet_cfgserver_$(SOC)_CORELIST
ifneq ($(SOC),$(filter $(SOC), tpr12 awr294x))
enet_LIB_LIST += enet_cfgserver
endif


#
# Enet example utils library
#
# Define the rule to generate the 'Enet example utils library' make rule for each rtos_type
# - Here $(1) refers to the first argument passed to the rule.
#   - In this case it is $(curos), each instance in "drvenet_RTOS_LIST" (ie, tirtos/freertos/safertos)
# - The target name will be <app_name>_<rtos_type> (ie, enet_example_utils_tirtos/enet_example_utils_freertos/...)
# - Here BOARDLIST passed for the target is drivers BOARDLIST for the particular rtos_type(ie, $(drvenet_$(rtos_type)_BOARDLIST). )
#   - If passing a custom or common BOARDLIST, one **should** always filter with Default BOARDLIST for each rtos_type(ie, $(DEFAULT_BOARDLIST_$(rtos_type)). )
#   - For example "export enet_example_utils_$(1)_BOARDLIST =  $(filter $(DEFAULT_BOARDLIST_$(1)), am64x_evm j721e_evm )
#   - This is because, in case of passing custom or common BOARDLIST, some boards may not be supporting all rtos_types
# - CORELIST passed for the target **should** always be filtered with the Default CORELIST of the SOC for each rtos_type (ie, $(DEFAULT_$(SOC)_CORELIST_$(1)). )
#   - The default CORELIST of an SOC for each rtos_type is defined in 'ti/build/makerules/component.mk'
#   - This is because some rtos_type won't be supported on specific cores. (For example, FreeRTOS is not supported on mpu1_0 core)
# - SafeRTOS example should be added to the example list only if SafeRTOS Kernel is present in the path.
#
define ENET_UTILS_RULE

export enet_example_utils_$(1)_COMP_LIST = enet_example_utils_$(1)
enet_example_utils_$(1)_RELPATH = ti/drv/enet/examples/utils
enet_example_utils_$(1)_PATH = $(PDK_ENET_COMP_PATH)/examples/utils
export enet_example_utils_$(1)_LIBNAME = enet_example_utils_$(1)
export enet_example_utils_$(1)_LIBPATH = $(PDK_ENET_COMP_PATH)/lib
export enet_example_utils_$(1)_OBJPATH = ti/drv/enet/examples/utils/enet_example_utils_$(1)
export enet_example_utils_$(1)_MAKEFILE = -fmakefile BUILD_OS_TYPE=$(1)
export enet_example_utils_$(1)_BOARD_DEPENDENCY = yes
export enet_example_utils_$(1)_CORE_DEPENDENCY = yes
enet_example_utils_$(1)_PKG_LIST = enet_example_utils_$(1)
enet_example_utils_$(1)_INCLUDE = $(enet_example_utils_$(1)_PATH)
export enet_example_utils_$(1)_$(SOC)_CORELIST = $(filter $(DEFAULT_$(SOC)_CORELIST_$(1)), $(drvenet_$(SOC)_CORELIST))
export enet_example_utils_$(1)_BOARDLIST = $(drvenet_$(1)_BOARDLIST)
export enet_example_utils_$(1)_SOCLIST = $(drvenet_SOCLIST)
ifneq ($(1),$(filter $(1), safertos))
enet_LIB_LIST += enet_example_utils_$(1)
else
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
enet_LIB_LIST += enet_example_utils_$(1)
endif
endif

endef

ENET_UTILS_MACRO_LIST := $(foreach curos,$(drvenet_RTOS_LIST),$(call ENET_UTILS_RULE,$(curos)))

$(eval ${ENET_UTILS_MACRO_LIST})

#
# Enet example utils library (full version)
#
define ENET_UTILS_FULL_RULE

export enet_example_utils_full_$(1)_COMP_LIST = enet_example_utils_full_$(1)
enet_example_utils_full_$(1)_RELPATH = ti/drv/enet/examples/utils
enet_example_utils_full_$(1)_PATH = $(PDK_ENET_COMP_PATH)/examples/utils
export enet_example_utils_full_$(1)_LIBNAME = enet_example_utils_full_$(1)
export enet_example_utils_full_$(1)_LIBPATH = $(PDK_ENET_COMP_PATH)/lib
export enet_example_utils_full_$(1)_OBJPATH = ti/drv/enet/examples/utils/enet_example_utils_full_$(1)
export enet_example_utils_full_$(1)_MAKEFILE = -fmakefile BUILD_OS_TYPE=$(1) APPUTILS_TYPE=full
export enet_example_utils_full_$(1)_BOARD_DEPENDENCY = yes
export enet_example_utils_full_$(1)_CORE_DEPENDENCY = yes
enet_example_utils_full_$(1)_PKG_LIST = enet_example_utils_full_$(1)
enet_example_utils_full_$(1)_INCLUDE = $(enet_example_utils_full_$(1)_PATH)
export enet_example_utils_full_$(1)_$(SOC)_CORELIST = $(filter $(DEFAULT_$(SOC)_CORELIST_$(1)), $(drvenet_$(SOC)_CORELIST))
export enet_example_utils_full_$(1)_BOARDLIST = $(drvenet_$(1)_BOARDLIST)
export enet_example_utils_full_$(1)_SOCLIST = $(drvenet_SOCLIST)
ifneq ($(1),$(filter $(1), safertos))
enet_LIB_LIST += enet_example_utils_full_$(1)
else
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
enet_LIB_LIST += enet_example_utils_full_$(1)
endif
endif

endef

ENET_UTILS_FULL_MACRO_LIST := $(foreach curos,$(drvenet_RTOS_LIST),$(call ENET_UTILS_FULL_RULE,$(curos)))

$(eval ${ENET_UTILS_FULL_MACRO_LIST})

#
# Enet example utils (baremetal) library
#
export enet_example_utils_baremetal_COMP_LIST = enet_example_utils_baremetal
enet_example_utils_baremetal_RELPATH = ti/drv/enet/examples/utils
enet_example_utils_baremetal_PATH = $(PDK_ENET_COMP_PATH)/examples/utils
export enet_example_utils_baremetal_LIBNAME = enet_example_utils_baremetal
export enet_example_utils_baremetal_LIBPATH = $(PDK_ENET_COMP_PATH)/lib
export enet_example_utils_baremetal_OBJPATH = $(enet_example_utils_baremetal_RELPATH)/baremetal
export enet_example_utils_baremetal_MAKEFILE = -fmakefile BUILD_OS_TYPE=baremetal
export enet_example_utils_baremetal_BOARD_DEPENDENCY = yes
export enet_example_utils_baremetal_CORE_DEPENDENCY = yes
enet_example_utils_baremetal_PKG_LIST = enet_example_utils_baremetal
enet_example_utils_baremetal_INCLUDE = $(enet_example_utils_baremetal_PATH)
export enet_example_utils_baremetal_SOCLIST = $(drvenet_SOCLIST)
export enet_example_utils_baremetal_$(SOC)_CORELIST = $(drvenet_$(SOC)_CORELIST)
enet_LIB_LIST += enet_example_utils_baremetal

#
# ENET NIMU example (TI-RTOS)
#
export enet_nimu_example_COMP_LIST = enet_nimu_example
enet_nimu_example_RELPATH = ti/drv/enet/examples/enet_nimu_example
enet_nimu_example_PATH = $(PDK_ENET_COMP_PATH)/examples/enet_nimu_example
export enet_nimu_example_BOARD_DEPENDENCY = yes
export enet_nimu_example_CORE_DEPENDENCY = yes
export enet_nimu_example_XDC_CONFIGURO = yes
enet_nimu_example_PKG_LIST = enet_nimu_example
enet_nimu_example_INCLUDE = $(enet_nimu_example_PATH)
export enet_nimu_example_BOARDLIST = $(drvenet_K3_BOARDLIST)
export enet_nimu_example_$(SOC)_CORELIST = $(drvenet_$(SOC)_CORELIST)
export enet_nimu_example_SBL_APPIMAGEGEN = yes
ifneq ($(SOC),$(filter $(SOC), tpr12 awr294x))
enet_EXAMPLE_LIST += enet_nimu_example
endif

#
# ENET lwIP example app
#
define enet_lwip_example_RULE

export enet_lwip_example_$(1)_COMP_LIST = enet_lwip_example_$(1)
enet_lwip_example_$(1)_RELPATH = ti/drv/enet/examples/enet_lwip_example
enet_lwip_example_$(1)_PATH = $(PDK_ENET_COMP_PATH)/examples/enet_lwip_example
export enet_lwip_example_$(1)_BOARD_DEPENDENCY = yes
export enet_lwip_example_$(1)_CORE_DEPENDENCY = yes
export enet_lwip_example_$(1)_XDC_CONFIGURO = $(if $(findstring tirtos,$(1)),yes,no)
export enet_lwip_example_$(1)_MAKEFILE = -f makefile BUILD_OS_TYPE=$(1)
enet_lwip_example_$(1)_PKG_LIST = enet_lwip_example_$(1)
enet_lwip_example_$(1)_INCLUDE = $(enet_lwip_example_$(1)_PATH)
export enet_lwip_example_$(1)_$(SOC)_CORELIST = $(filter $(DEFAULT_$(SOC)_CORELIST_$(1)), $(drvenet_lwip_CORELIST))
export enet_lwip_example_$(1)_BOARDLIST = $(drvenet_$(1)_BOARDLIST)
export enet_lwip_example_$(1)_SOCLIST = $(drvenet_SOCLIST)
export enet_lwip_example_$(1)_SBL_APPIMAGEGEN = yes
ifneq ($(1),$(filter $(1), safertos))
enet_EXAMPLE_LIST += enet_lwip_example_$(1)
else
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
enet_EXAMPLE_LIST += enet_lwip_example_$(1)
endif
endif

endef

enet_lwip_example_MACRO_LIST := $(foreach curos,$(drvenet_RTOS_LIST),$(call enet_lwip_example_RULE,$(curos)))
$(eval ${enet_lwip_example_MACRO_LIST})

#
# Loopback test
#
# Define the rule to generate the 'Enet loopback test' make rule for each rtos_type
# - Here $(1) refers to the first argument passed to the rule.
#   - In this case it is $(curos), each instance in "drvenet_RTOS_LIST" (ie, tirtos/freertos/safertos)
# - The target name will be <app_name>_<rtos_type> (ie, enet_loopback_test_tirtos/enet_loopback_test_freertos/...)
# - Set XDC_CONFIGURO only for 'rtos_type = tirtos'
# - Pass the arg BUILD_OS_TYPE=<rtos_type> to the common makefile for all rtos_types
#   - In the common makefile, make use the param $(BUILD_OS_TYPE)
#   - For example:
#       APP_NAME = enet_loopback_test_$(BUILD_OS_TYPE)
#       ifeq ($(BUILD_OS_TYPE), tirtos)
#           INCLUDE_EXTERNAL_INTERFACES += xdc bios
#       endif
#       ifeq ($(BUILD_OS_TYPE), freertos)
#           INCLUDE_EXTERNAL_INTERFACES += freertos
#       endif
# - Here BOARDLIST passed for the target is drivers BOARDLIST for the particular rtos_type(ie, $(drvenet_$(rtos_type)_BOARDLIST). )
#   - If passing a custom or common BOARDLIST, one **should** always filter with Default BOARDLIST for each rtos_type(ie, $(DEFAULT_BOARDLIST_$(rtos_type)). )
#   - For example "export enet_example_utils_$(1)_BOARDLIST =  $(filter $(DEFAULT_BOARDLIST_$(1)), am64x_evm j721e_evm )
#   - This is because, in case of passing custom or common BOARDLIST, some boards may not be supporting all rtos_types
# - CORELIST passed for the target **should** always be filtered with the Default CORELIST of the SOC for each rtos_type (ie, $(DEFAULT_$(SOC)_CORELIST_$(1)). )
#   - The default CORELIST of an SOC for each rtos_type is defined in 'ti/build/makerules/component.mk'
#   - This is because some rtos_type won't be supported on specific cores. (For example, FreeRTOS is not supported on mpu1_0 core)
# - SafeRTOS example should be added to the example list only if SafeRTOS Kernel is present in the path.
#
define ENET_LOOPBACK_TEST_RULE

export enet_loopback_test_$(1)_COMP_LIST = enet_loopback_test_$(1)
enet_loopback_test_$(1)_RELPATH = ti/drv/enet/examples/enet_loopback_test
enet_loopback_test_$(1)_PATH = $(PDK_ENET_COMP_PATH)/examples/enet_loopback_test
export enet_loopback_test_$(1)_BOARD_DEPENDENCY = yes
export enet_loopback_test_$(1)_CORE_DEPENDENCY = yes
export enet_loopback_test_$(1)_XDC_CONFIGURO = $(if $(findstring tirtos,$(1)),yes,no)
export enet_loopback_test_$(1)_MAKEFILE = -f makefile BUILD_OS_TYPE=$(1)
enet_loopback_test_$(1)_PKG_LIST = enet_loopback_test_$(1)
enet_loopback_test_$(1)_INCLUDE = $(enet_loopback_test_$(1)_PATH)
export enet_loopback_test_$(1)_$(SOC)_CORELIST = $(filter $(DEFAULT_$(SOC)_CORELIST_$(1)), $(drvenet_$(SOC)_CORELIST))
export enet_loopback_test_$(1)_BOARDLIST = $(drvenet_$(1)_BOARDLIST)
export enet_loopback_test_$(1)_SOCLIST = $(drvenet_SOCLIST)
ifeq ($(SOC),$(filter $(SOC), am65xx j721e j7200 am64x tpr12 awr294x))
export enet_loopback_test_$(1)_SBL_APPIMAGEGEN = yes
endif
ifneq ($(1),$(filter $(1), safertos))
enet_EXAMPLE_LIST += enet_loopback_test_$(1)
else
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
enet_EXAMPLE_LIST += enet_loopback_test_$(1)
endif
endif

endef

ENET_LOOPBACK_TEST_MACRO_LIST := $(foreach curos,$(drvenet_RTOS_LIST),$(call ENET_LOOPBACK_TEST_RULE,$(curos)))

$(eval ${ENET_LOOPBACK_TEST_MACRO_LIST})

#
# Multi-port test (TI-RTOS)
#
#
# Defines the rule to generate the 'Enet multiport test' make rule for each rtos_type
# - Here $(1) refers to the first argument passed to the rule. In this case it
#   is $(curos), each instance in "drvenet_RTOS_LIST" (ie, tirtos/freertos/safertos)
#
define ENET_MULTIPORT_TEST_RULE

export enet_multiport_test_$(1)_COMP_LIST = enet_multiport_test_$(1)
enet_multiport_test_$(1)_RELPATH = ti/drv/enet/examples/enet_multiport_test
enet_multiport_test_$(1)_PATH = $(PDK_ENET_COMP_PATH)/examples/enet_multiport_test
export enet_multiport_test_$(1)_BOARD_DEPENDENCY = yes
export enet_multiport_test_$(1)_CORE_DEPENDENCY = yes
export enet_multiport_test_$(1)_XDC_CONFIGURO = $(if $(findstring tirtos,$(1)),yes,no)
export enet_multiport_test_$(1)_MAKEFILE = -f makefile BUILD_OS_TYPE=$(1)
enet_multiport_test_$(1)_PKG_LIST = enet_multiport_test_$(1)
enet_multiport_test_$(1)_INCLUDE = $(enet_multiport_test_$(1)_PATH)
export enet_multiport_test_$(1)_$(SOC)_CORELIST = $(filter $(DEFAULT_$(SOC)_CORELIST_$(1)), $(drvenet_$(SOC)_CORELIST))
export enet_multiport_test_$(1)_BOARDLIST = $(filter $(DEFAULT_BOARDLIST_$(1)), am65xx_idk am65xx_evm)
export enet_multiport_test_$(1)_SOCLIST = $(filter $(DEFAULT_SOCLIST_$(1)), am65xx)
ifeq ($(SOC),$(filter $(SOC), am65xx))
export enet_multiport_test_$(1)_SBL_APPIMAGEGEN = yes
endif
ifneq ($(1),$(filter $(1), safertos))
enet_EXAMPLE_LIST += enet_multiport_test_$(1)
else
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
enet_EXAMPLE_LIST += enet_multiport_test_$(1)
endif
endif

endef

ENET_MULTIPORT_TEST_MACRO_LIST := $(foreach curos,$(drvenet_RTOS_LIST),$(call ENET_MULTIPORT_TEST_RULE,$(curos)))

$(eval ${ENET_MULTIPORT_TEST_MACRO_LIST})

#
# Unit test (TI-RTOS)
#
-include $(PDK_ENET_COMP_PATH)/unit_test/enet_ut_component.mk
ifneq ($(enet_ut_LIB_LIST),)
  enet_LIB_LIST += $(enet_ut_LIB_LIST)
endif
ifneq ($(enet_ut_EXAMPLE_LIST),)
  enet_EXAMPLE_LIST += $(enet_ut_EXAMPLE_LIST)
endif

export enet_LIB_LIST
export enet_EXAMPLE_LIST
export drvenet_LIB_LIST = $(enet_LIB_LIST)
export drvenet_EXAMPLE_LIST = $(enet_EXAMPLE_LIST)

# Enable asserts and prints
ENET_CFLAGS =
ENET_CFLAGS += -DENET_CFG_ASSERT=1
#ENET_CFLAGS += -DENET_CFG_USE_STD_ASSERT
ENET_CFLAGS += -DENET_CFG_PRINT_ENABLE
ENET_CFLAGS += -DUART_ENABLED

# Trace level per build profile:
# 0 - None
# 1 - Error
# 2 - Warning
# 3 - Info
# 4 - Debug
# 5 - Verbose
ifeq ($(BUILD_PROFILE),debug)
    ENET_CFLAGS += -DENET_CFG_TRACE_LEVEL=4
    ENET_CFLAGS += -DENET_CFG_DEV_ERROR=1
    ENET_CFLAGS += -DENETDMA_INSTRUMENTATION_ENABLED
    ENET_CFLAGS += -DENETCPTS_INSTRUMENTATION_ENABLED
else
    ENET_CFLAGS += -DENET_CFG_TRACE_LEVEL=3
endif

ifeq ($(SOC),$(filter $(SOC), am65xx))
ifeq ($(CORE),$(filter $(CORE), mpu1_0 mcu1_0))
    ENET_CFLAGS += -DENET_ENABLE_ICSSG
    export ENET_ENABLE_ICSSG=yes
endif
endif

# lwIP interface layer allows application to perform packet handling
# only on j721e/j7200 mcu2_0 core
ifeq ($(CORE),$(filter $(CORE), mcu2_0))
    ENET_CFLAGS += -DLWIPIF_APP_RX_PKT_HANDLING
endif

export ENET_CFLAGS

enet_component_make_include := 1
endif
