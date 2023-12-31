# File: vhwa_component.mk
#       This file is component include make file of VHWA driver library.
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
ifeq ($(vhwa_component_make_include), )

vhwa_default_SOCLIST        = j721e
vhwa_default_BOARDLIST      = j721e_evm j721e_qt
vhwa_default_j721e_CORELIST = mcu2_0 mcu2_1
vhwa_vpac_j721e_CORELIST    = mcu2_0
vhwa_dmpac_j721e_CORELIST   = mcu2_1
vhwa_RTOS_LIST       = $(DEFAULT_RTOS_LIST)

define DRV_VHWA_RTOS_BOARDLIST_RULE

vhwa_$(1)_BOARDLIST = $(filter $(DEFAULT_BOARDLIST_$(1)), $(vhwa_default_BOARDLIST))

endef

DRV_VHWA_RTOS_BOARDLIST_MACRO_LIST := $(foreach curos, $(vhwa_RTOS_LIST), $(call DRV_VHWA_RTOS_BOARDLIST_RULE,$(curos)))

$(eval ${DRV_VHWA_RTOS_BOARDLIST_MACRO_LIST})

############################
# vhwa package
# List of components included under vhwa lib
# The components included here are built and will be part of vhwa lib
############################
vhwa_LIB_LIST =

############################
# vhwa examples
# List of examples under vhwa (+= is used at each example definition)
# All the tests mentioned in list are built when test target is called
# List below all examples for allowed values
############################
vhwa_EXAMPLE_LIST =

#
# VHWA Modules
#

# VHWA library
vhwa_COMP_LIST = vhwa
vhwa_RELPATH = ti/drv/vhwa
vhwa_PATH = $(PDK_VHWA_COMP_PATH)
vhwa_LIBNAME = vhwa
vhwa_LIBPATH = $(PDK_VHWA_COMP_PATH)/lib
vhwa_MAKEFILE = -fsrc/makefile
export vhwa_MAKEFILE
export vhwa_LIBNAME
export vhwa_LIBPATH
vhwa_BOARD_DEPENDENCY = no
ifeq ($(BOARD),$(filter $(BOARD), j721e_sim j721e_vhwazebu j721e_qt))
  vhwa_BOARD_DEPENDENCY = yes
endif
vhwa_CORE_DEPENDENCY = yes
export vhwa_COMP_LIST
export vhwa_BOARD_DEPENDENCY
export vhwa_CORE_DEPENDENCY
vhwa_PKG_LIST = vhwa
vhwa_INCLUDE = $(vhwa_PATH)
vhwa_SOCLIST = $(vhwa_default_SOCLIST)
export vhwa_SOCLIST
vhwa_$(SOC)_CORELIST = $(vhwa_default_$(SOC)_CORELIST)
export vhwa_$(SOC)_CORELIST
vhwa_LIB_LIST += vhwa

# VHWA MSC baremetal test app
vhwa_msc_baremetal_testapp_COMP_LIST = vhwa_msc_baremetal_testapp
vhwa_msc_baremetal_testapp_RELPATH = ti/drv/vhwa/examples/vhwa_msc_test
vhwa_msc_baremetal_testapp_PATH = $(PDK_VHWA_COMP_PATH)/examples/vhwa_msc_test
vhwa_msc_baremetal_testapp_MAKEFILE = -fmakefile BUILD_OS_TYPE=baremetal
export vhwa_msc_baremetal_testapp_MAKEFILE
vhwa_msc_baremetal_testapp_BOARD_DEPENDENCY = yes
vhwa_msc_baremetal_testapp_CORE_DEPENDENCY = yes
export vhwa_msc_baremetal_testapp_COMP_LIST
export vhwa_msc_baremetal_testapp_BOARD_DEPENDENCY
export vhwa_msc_baremetal_testapp_CORE_DEPENDENCY
vhwa_msc_baremetal_testapp_PKG_LIST = vhwa_msc_baremetal_testapp
vhwa_msc_baremetal_testapp_INCLUDE = $(vhwa_msc_baremetal_testapp_PATH)
vhwa_msc_baremetal_testapp_BOARDLIST = $(vhwa_default_BOARDLIST)
export vhwa_msc_baremetal_testapp_BOARDLIST
vhwa_msc_baremetal_testapp_$(SOC)_CORELIST = $(vhwa_vpac_$(SOC)_CORELIST)
export vhwa_msc_baremetal_testapp_$(SOC)_CORELIST
vhwa_EXAMPLE_LIST += vhwa_msc_baremetal_testapp

# VHWA MSC test app
define VHWA_MSC_TESTAPP_RULE

export vhwa_msc_testapp_$(1)_COMP_LIST = vhwa_msc_testapp_$(1)
vhwa_msc_testapp_$(1)_RELPATH = ti/drv/vhwa/examples/vhwa_msc_test
vhwa_msc_testapp_$(1)_PATH = $(PDK_VHWA_COMP_PATH)/examples/vhwa_msc_test
export vhwa_msc_testapp_$(1)_BOARD_DEPENDENCY = yes
export vhwa_msc_testapp_$(1)_CORE_DEPENDENCY = yes
export vhwa_msc_testapp_$(1)_XDC_CONFIGURO = $(if $(findstring tirtos, $(1)), yes, no)
export vhwa_msc_testapp_$(1)_MAKEFILE = -f makefile BUILD_OS_TYPE=$(1)
vhwa_msc_testapp_$(1)_PKG_LIST = vhwa_msc_testapp_$(1)
vhwa_msc_testapp_$(1)_INCLUDE = $(vhwa_msc_testapp_$(1)_PATH)
export vhwa_msc_testapp_$(1)_BOARDLIST = $(filter $(DEFAULT_BOARDLIST_$(1)), $(vhwa_default_BOARDLIST) )
export vhwa_msc_testapp_$(1)_$(SOC)_CORELIST = $(filter $(DEFAULT_$(SOC)_CORELIST_$(1)), $(vhwa_vpac_$(SOC)_CORELIST))
export vhwa_msc_testapp_$(1)_SBL_APPIMAGEGEN = yes
ifneq ($(1),$(filter $(1), safertos))
vhwa_EXAMPLE_LIST += vhwa_msc_testapp_$(1)
else
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
vhwa_EXAMPLE_LIST += vhwa_msc_testapp_$(1)
endif
endif

endef

VHWA_MSC_TESTAPP_MACRO_LIST := $(foreach curos, $(vhwa_RTOS_LIST), $(call VHWA_MSC_TESTAPP_RULE,$(curos)))

$(eval ${VHWA_MSC_TESTAPP_MACRO_LIST})

# VHWA LDC baremetal test app
vhwa_ldc_baremetal_testapp_COMP_LIST = vhwa_ldc_baremetal_testapp
vhwa_ldc_baremetal_testapp_RELPATH = ti/drv/vhwa/examples/vhwa_ldc_test
vhwa_ldc_baremetal_testapp_PATH = $(PDK_VHWA_COMP_PATH)/examples/vhwa_ldc_test
vhwa_ldc_baremetal_testapp_MAKEFILE = -fmakefile BUILD_OS_TYPE=baremetal
export vhwa_ldc_baremetal_testapp_MAKEFILE
vhwa_ldc_baremetal_testapp_BOARD_DEPENDENCY = yes
vhwa_ldc_baremetal_testapp_CORE_DEPENDENCY = yes
export vhwa_ldc_baremetal_testapp_COMP_LIST
export vhwa_ldc_baremetal_testapp_BOARD_DEPENDENCY
export vhwa_ldc_baremetal_testapp_CORE_DEPENDENCY
vhwa_ldc_baremetal_testapp_PKG_LIST = vhwa_ldc_baremetal_testapp
vhwa_ldc_baremetal_testapp_INCLUDE = $(vhwa_ldc_baremetal_testapp_PATH)
vhwa_ldc_baremetal_testapp_BOARDLIST = $(vhwa_default_BOARDLIST)
export vhwa_ldc_baremetal_testapp_BOARDLIST
vhwa_ldc_baremetal_testapp_$(SOC)_CORELIST = $(vhwa_vpac_$(SOC)_CORELIST)
export vhwa_ldc_baremetal_testapp_$(SOC)_CORELIST
vhwa_EXAMPLE_LIST += vhwa_ldc_baremetal_testapp

# VHWA LDC test app
define VHWA_LDC_TESTAPP_RULE

export vhwa_ldc_testapp_$(1)_COMP_LIST = vhwa_ldc_testapp_$(1)
vhwa_ldc_testapp_$(1)_RELPATH = ti/drv/vhwa/examples/vhwa_ldc_test
vhwa_ldc_testapp_$(1)_PATH = $(PDK_VHWA_COMP_PATH)/examples/vhwa_ldc_test
export vhwa_ldc_testapp_$(1)_BOARD_DEPENDENCY = yes
export vhwa_ldc_testapp_$(1)_CORE_DEPENDENCY = yes
export vhwa_ldc_testapp_$(1)_XDC_CONFIGURO = $(if $(findstring tirtos, $(1)), yes, no)
export vhwa_ldc_testapp_$(1)_MAKEFILE = -f makefile BUILD_OS_TYPE=$(1)
vhwa_ldc_testapp_$(1)_PKG_LIST = vhwa_ldc_testapp_$(1)
vhwa_ldc_testapp_$(1)_INCLUDE = $(vhwa_ldc_testapp_$(1)_PATH)
export vhwa_ldc_testapp_$(1)_BOARDLIST = $(filter $(DEFAULT_BOARDLIST_$(1)), $(vhwa_default_BOARDLIST) )
export vhwa_ldc_testapp_$(1)_$(SOC)_CORELIST = $(filter $(DEFAULT_$(SOC)_CORELIST_$(1)), $(vhwa_vpac_$(SOC)_CORELIST))
export vhwa_ldc_testapp_$(1)_SBL_APPIMAGEGEN = yes
ifneq ($(1),$(filter $(1), safertos))
vhwa_EXAMPLE_LIST += vhwa_ldc_testapp_$(1)
else
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
vhwa_EXAMPLE_LIST += vhwa_ldc_testapp_$(1)
endif
endif

endef

VHWA_LDC_TESTAPP_MACRO_LIST := $(foreach curos, $(vhwa_RTOS_LIST), $(call VHWA_LDC_TESTAPP_RULE,$(curos)))

$(eval ${VHWA_LDC_TESTAPP_MACRO_LIST})

# VHWA DOF baremetal test app
vhwa_dof_baremetal_testapp_COMP_LIST = vhwa_dof_baremetal_testapp
vhwa_dof_baremetal_testapp_RELPATH = ti/drv/vhwa/examples/vhwa_dof_test
vhwa_dof_baremetal_testapp_PATH = $(PDK_VHWA_COMP_PATH)/examples/vhwa_dof_test
vhwa_dof_baremetal_testapp_MAKEFILE = -fmakefile BUILD_OS_TYPE=baremetal
export vhwa_dof_baremetal_testapp_MAKEFILE
vhwa_dof_baremetal_testapp_BOARD_DEPENDENCY = yes
vhwa_dof_baremetal_testapp_CORE_DEPENDENCY = yes
export vhwa_dof_baremetal_testapp_COMP_LIST
export vhwa_dof_baremetal_testapp_BOARD_DEPENDENCY
export vhwa_dof_baremetal_testapp_CORE_DEPENDENCY
vhwa_dof_baremetal_testapp_PKG_LIST = vhwa_dof_baremetal_testapp
vhwa_dof_baremetal_testapp_INCLUDE = $(vhwa_dof_baremetal_testapp_PATH)
vhwa_dof_baremetal_testapp_BOARDLIST = $(vhwa_default_BOARDLIST)
export vhwa_dof_baremetal_testapp_BOARDLIST
vhwa_dof_baremetal_testapp_$(SOC)_CORELIST = $(vhwa_dmpac_$(SOC)_CORELIST)
export vhwa_dof_baremetal_testapp_$(SOC)_CORELIST
vhwa_EXAMPLE_LIST += vhwa_dof_baremetal_testapp

# VHWA DOF test app
define VHWA_DOF_TESTAPP_RULE

export vhwa_dof_testapp_$(1)_COMP_LIST = vhwa_dof_testapp_$(1)
vhwa_dof_testapp_$(1)_RELPATH = ti/drv/vhwa/examples/vhwa_dof_test
vhwa_dof_testapp_$(1)_PATH = $(PDK_VHWA_COMP_PATH)/examples/vhwa_dof_test
export vhwa_dof_testapp_$(1)_BOARD_DEPENDENCY = yes
export vhwa_dof_testapp_$(1)_CORE_DEPENDENCY = yes
export vhwa_dof_testapp_$(1)_XDC_CONFIGURO = $(if $(findstring tirtos, $(1)), yes, no)
export vhwa_dof_testapp_$(1)_MAKEFILE = -f makefile BUILD_OS_TYPE=$(1)
vhwa_dof_testapp_$(1)_PKG_LIST = vhwa_dof_testapp_$(1)
vhwa_dof_testapp_$(1)_INCLUDE = $(vhwa_dof_testapp_$(1)_PATH)
export vhwa_dof_testapp_$(1)_BOARDLIST = $(filter $(DEFAULT_BOARDLIST_$(1)), $(vhwa_default_BOARDLIST) )
export vhwa_dof_testapp_$(1)_$(SOC)_CORELIST = $(filter $(DEFAULT_$(SOC)_CORELIST_$(1)), $(vhwa_dmpac_$(SOC)_CORELIST))
export vhwa_dof_testapp_$(1)_SBL_APPIMAGEGEN = yes
ifneq ($(1),$(filter $(1), safertos))
vhwa_EXAMPLE_LIST += vhwa_dof_testapp_$(1)
else
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
vhwa_EXAMPLE_LIST += vhwa_dof_testapp_$(1)
endif
endif

endef

VHWA_DOF_TESTAPP_MACRO_LIST := $(foreach curos, $(vhwa_RTOS_LIST), $(call VHWA_DOF_TESTAPP_RULE,$(curos)))

$(eval ${VHWA_DOF_TESTAPP_MACRO_LIST})

# VHWA VISS baremetal test app
vhwa_viss_baremetal_testapp_COMP_LIST = vhwa_viss_baremetal_testapp
vhwa_viss_baremetal_testapp_RELPATH = ti/drv/vhwa/examples/vhwa_viss_test
vhwa_viss_baremetal_testapp_PATH = $(PDK_VHWA_COMP_PATH)/examples/vhwa_viss_test
vhwa_viss_baremetal_testapp_MAKEFILE = -fmakefile BUILD_OS_TYPE=baremetal
export vhwa_viss_baremetal_testapp_MAKEFILE
vhwa_viss_baremetal_testapp_BOARD_DEPENDENCY = yes
vhwa_viss_baremetal_testapp_CORE_DEPENDENCY = yes
export vhwa_viss_baremetal_testapp_COMP_LIST
export vhwa_viss_baremetal_testapp_BOARD_DEPENDENCY
export vhwa_viss_baremetal_testapp_CORE_DEPENDENCY
vhwa_viss_baremetal_testapp_PKG_LIST = vhwa_viss_baremetal_testapp
vhwa_viss_baremetal_testapp_INCLUDE = $(vhwa_viss_baremetal_testapp_PATH)
vhwa_viss_baremetal_testapp_BOARDLIST = $(vhwa_default_BOARDLIST)
export vhwa_viss_baremetal_testapp_BOARDLIST
vhwa_viss_baremetal_testapp_$(SOC)_CORELIST = $(vhwa_vpac_$(SOC)_CORELIST)
export vhwa_viss_baremetal_testapp_$(SOC)_CORELIST
vhwa_EXAMPLE_LIST += vhwa_viss_baremetal_testapp

# VHWA VISS test app
define VHWA_VISS_TESTAPP_RULE

export vhwa_viss_testapp_$(1)_COMP_LIST = vhwa_viss_testapp_$(1)
vhwa_viss_testapp_$(1)_RELPATH = ti/drv/vhwa/examples/vhwa_viss_test
vhwa_viss_testapp_$(1)_PATH = $(PDK_VHWA_COMP_PATH)/examples/vhwa_viss_test
export vhwa_viss_testapp_$(1)_BOARD_DEPENDENCY = yes
export vhwa_viss_testapp_$(1)_CORE_DEPENDENCY = yes
export vhwa_viss_testapp_$(1)_XDC_CONFIGURO = $(if $(findstring tirtos, $(1)), yes, no)
export vhwa_viss_testapp_$(1)_MAKEFILE = -f makefile BUILD_OS_TYPE=$(1)
vhwa_viss_testapp_$(1)_PKG_LIST = vhwa_viss_testapp_$(1)
vhwa_viss_testapp_$(1)_INCLUDE = $(vhwa_viss_testapp_$(1)_PATH)
export vhwa_viss_testapp_$(1)_BOARDLIST = $(filter $(DEFAULT_BOARDLIST_$(1)), $(vhwa_default_BOARDLIST) )
export vhwa_viss_testapp_$(1)_$(SOC)_CORELIST = $(filter $(DEFAULT_$(SOC)_CORELIST_$(1)), $(vhwa_vpac_$(SOC)_CORELIST))
export vhwa_viss_testapp_$(1)_SBL_APPIMAGEGEN = yes
ifneq ($(1),$(filter $(1), safertos))
vhwa_EXAMPLE_LIST += vhwa_viss_testapp_$(1)
else
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
vhwa_EXAMPLE_LIST += vhwa_viss_testapp_$(1)
endif
endif

endef

VHWA_VISS_TESTAPP_MACRO_LIST := $(foreach curos, $(vhwa_RTOS_LIST), $(call VHWA_VISS_TESTAPP_RULE,$(curos)))

$(eval ${VHWA_VISS_TESTAPP_MACRO_LIST})

# VHWA NF baremetal test app
vhwa_nf_baremetal_testapp_COMP_LIST = vhwa_nf_baremetal_testapp
vhwa_nf_baremetal_testapp_RELPATH = ti/drv/vhwa/examples/vhwa_nf_test
vhwa_nf_baremetal_testapp_PATH = $(PDK_VHWA_COMP_PATH)/examples/vhwa_nf_test
vhwa_nf_baremetal_testapp_MAKEFILE = -fmakefile BUILD_OS_TYPE=baremetal
export vhwa_nf_baremetal_testapp_MAKEFILE
vhwa_nf_baremetal_testapp_BOARD_DEPENDENCY = yes
vhwa_nf_baremetal_testapp_CORE_DEPENDENCY = yes
export vhwa_nf_baremetal_testapp_COMP_LIST
export vhwa_nf_baremetal_testapp_BOARD_DEPENDENCY
export vhwa_nf_baremetal_testapp_CORE_DEPENDENCY
vhwa_nf_baremetal_testapp_PKG_LIST = vhwa_nf_baremetal_testapp
vhwa_nf_baremetal_testapp_INCLUDE = $(vhwa_nf_baremetal_testapp_PATH)
vhwa_nf_baremetal_testapp_BOARDLIST = $(vhwa_default_BOARDLIST)
export vhwa_nf_baremetal_testapp_BOARDLIST
vhwa_nf_baremetal_testapp_$(SOC)_CORELIST = $(vhwa_vpac_$(SOC)_CORELIST)
export vhwa_nf_baremetal_testapp_$(SOC)_CORELIST
vhwa_EXAMPLE_LIST += vhwa_nf_baremetal_testapp

# VHWA NF test app
define VHWA_NF_TESTAPP_RULE

export vhwa_nf_testapp_$(1)_COMP_LIST = vhwa_nf_testapp_$(1)
vhwa_nf_testapp_$(1)_RELPATH = ti/drv/vhwa/examples/vhwa_nf_test
vhwa_nf_testapp_$(1)_PATH = $(PDK_VHWA_COMP_PATH)/examples/vhwa_nf_test
export vhwa_nf_testapp_$(1)_BOARD_DEPENDENCY = yes
export vhwa_nf_testapp_$(1)_CORE_DEPENDENCY = yes
export vhwa_nf_testapp_$(1)_XDC_CONFIGURO = $(if $(findstring tirtos, $(1)), yes, no)
export vhwa_nf_testapp_$(1)_MAKEFILE = -f makefile BUILD_OS_TYPE=$(1)
vhwa_nf_testapp_$(1)_PKG_LIST = vhwa_nf_testapp_$(1)
vhwa_nf_testapp_$(1)_INCLUDE = $(vhwa_nf_testapp_$(1)_PATH)
export vhwa_nf_testapp_$(1)_BOARDLIST = $(filter $(DEFAULT_BOARDLIST_$(1)), $(vhwa_default_BOARDLIST) )
export vhwa_nf_testapp_$(1)_$(SOC)_CORELIST = $(filter $(DEFAULT_$(SOC)_CORELIST_$(1)), $(vhwa_vpac_$(SOC)_CORELIST))
export vhwa_nf_testapp_$(1)_SBL_APPIMAGEGEN = yes
ifneq ($(1),$(filter $(1), safertos))
vhwa_EXAMPLE_LIST += vhwa_nf_testapp_$(1)
else
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
vhwa_EXAMPLE_LIST += vhwa_nf_testapp_$(1)
endif
endif

endef

VHWA_NF_TESTAPP_MACRO_LIST := $(foreach curos, $(vhwa_RTOS_LIST), $(call VHWA_NF_TESTAPP_RULE,$(curos)))

$(eval ${VHWA_NF_TESTAPP_MACRO_LIST})

# VHWA SDE baremetal test app
vhwa_sde_baremetal_testapp_COMP_LIST = vhwa_sde_baremetal_testapp
vhwa_sde_baremetal_testapp_RELPATH = ti/drv/vhwa/examples/vhwa_sde_test
vhwa_sde_baremetal_testapp_PATH = $(PDK_VHWA_COMP_PATH)/examples/vhwa_sde_test
vhwa_sde_baremetal_testapp_MAKEFILE = -fmakefile BUILD_OS_TYPE=baremetal
export vhwa_sde_baremetal_testapp_MAKEFILE
vhwa_sde_baremetal_testapp_BOARD_DEPENDENCY = yes
vhwa_sde_baremetal_testapp_CORE_DEPENDENCY = yes
export vhwa_sde_baremetal_testapp_COMP_LIST
export vhwa_sde_baremetal_testapp_BOARD_DEPENDENCY
export vhwa_sde_baremetal_testapp_CORE_DEPENDENCY
vhwa_sde_baremetal_testapp_PKG_LIST = vhwa_sde_baremetal_testapp
vhwa_sde_baremetal_testapp_INCLUDE = $(vhwa_sde_baremetal_testapp_PATH)
vhwa_sde_baremetal_testapp_BOARDLIST = $(vhwa_default_BOARDLIST)
export vhwa_sde_baremetal_testapp_BOARDLIST
vhwa_sde_baremetal_testapp_$(SOC)_CORELIST = $(vhwa_dmpac_$(SOC)_CORELIST)
export vhwa_sde_baremetal_testapp_$(SOC)_CORELIST
vhwa_EXAMPLE_LIST += vhwa_sde_baremetal_testapp

# VHWA SDE test app
define VHWA_SDE_TESTAPP_RULE

export vhwa_sde_testapp_$(1)_COMP_LIST = vhwa_sde_testapp_$(1)
vhwa_sde_testapp_$(1)_RELPATH = ti/drv/vhwa/examples/vhwa_sde_test
vhwa_sde_testapp_$(1)_PATH = $(PDK_VHWA_COMP_PATH)/examples/vhwa_sde_test
export vhwa_sde_testapp_$(1)_BOARD_DEPENDENCY = yes
export vhwa_sde_testapp_$(1)_CORE_DEPENDENCY = yes
export vhwa_sde_testapp_$(1)_XDC_CONFIGURO = $(if $(findstring tirtos, $(1)), yes, no)
export vhwa_sde_testapp_$(1)_MAKEFILE = -f makefile BUILD_OS_TYPE=$(1)
vhwa_sde_testapp_$(1)_PKG_LIST = vhwa_sde_testapp_$(1)
vhwa_sde_testapp_$(1)_INCLUDE = $(vhwa_sde_testapp_$(1)_PATH)
export vhwa_sde_testapp_$(1)_BOARDLIST = $(filter $(DEFAULT_BOARDLIST_$(1)), $(vhwa_default_BOARDLIST) )
export vhwa_sde_testapp_$(1)_$(SOC)_CORELIST = $(filter $(DEFAULT_$(SOC)_CORELIST_$(1)), $(vhwa_dmpac_$(SOC)_CORELIST))
export vhwa_sde_testapp_$(1)_SBL_APPIMAGEGEN = yes
ifneq ($(1),$(filter $(1), safertos))
vhwa_EXAMPLE_LIST += vhwa_sde_testapp_$(1)
else
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
vhwa_EXAMPLE_LIST += vhwa_sde_testapp_$(1)
endif
endif

endef

VHWA_SDE_TESTAPP_MACRO_LIST := $(foreach curos, $(vhwa_RTOS_LIST), $(call VHWA_SDE_TESTAPP_RULE,$(curos)))

$(eval ${VHWA_SDE_TESTAPP_MACRO_LIST})

# VHWA INT baremetal test app
vhwa_int_baremetal_testapp_COMP_LIST = vhwa_int_baremetal_testapp
vhwa_int_baremetal_testapp_RELPATH = ti/drv/vhwa/examples/vhwa_int_test
vhwa_int_baremetal_testapp_PATH = $(PDK_VHWA_COMP_PATH)/examples/vhwa_int_test
vhwa_int_baremetal_testapp_MAKEFILE = -fmakefile_baremetal
export vhwa_int_baremetal_testapp_MAKEFILE
vhwa_int_baremetal_testapp_BOARD_DEPENDENCY = yes
vhwa_int_baremetal_testapp_CORE_DEPENDENCY = yes
export vhwa_int_baremetal_testapp_COMP_LIST
export vhwa_int_baremetal_testapp_BOARD_DEPENDENCY
export vhwa_int_baremetal_testapp_CORE_DEPENDENCY
vhwa_int_baremetal_testapp_PKG_LIST = vhwa_int_baremetal_testapp
vhwa_int_baremetal_testapp_INCLUDE = $(vhwa_int_baremetal_testapp_PATH)
vhwa_int_baremetal_testapp_BOARDLIST = $(vhwa_default_BOARDLIST)
export vhwa_int_baremetal_testapp_BOARDLIST
vhwa_int_baremetal_testapp_$(SOC)_CORELIST = $(vhwa_vpac_$(SOC)_CORELIST)
export vhwa_int_baremetal_testapp_$(SOC)_CORELIST
vhwa_EXAMPLE_LIST += vhwa_int_baremetal_testapp

export vhwa_LIB_LIST
export vhwa_EXAMPLE_LIST

VHWA_CFLAGS =

# Enable asserts and prints
VHWA_CFLAGS += -DVHWA_CFG_ASSERT_ENABLE
VHWA_CFLAGS += -DVHWA_CFG_USE_STD_ASSERT
VHWA_CFLAGS += -DVHWA_CFG_PRINT_ENABLE

export VHWA_CFLAGS

vhwa_component_make_include := 1
endif

# VHWA Flex Connect baremetal test app
vhwa_flexconnect_baremetal_testapp_COMP_LIST = vhwa_flexconnect_baremetal_testapp
vhwa_flexconnect_baremetal_testapp_RELPATH = ti/drv/vhwa/examples/vhwa_flexconnect_test
vhwa_flexconnect_baremetal_testapp_PATH = $(PDK_VHWA_COMP_PATH)/examples/vhwa_flexconnect_test
vhwa_flexconnect_baremetal_testapp_MAKEFILE = -fmakefile BUILD_OS_TYPE=baremetal
export vhwa_flexconnect_baremetal_testapp_MAKEFILE
vhwa_flexconnect_baremetal_testapp_BOARD_DEPENDENCY = yes
vhwa_flexconnect_baremetal_testapp_CORE_DEPENDENCY = yes
export vhwa_flexconnect_baremetal_testapp_COMP_LIST
export vhwa_flexconnect_baremetal_testapp_BOARD_DEPENDENCY
export vhwa_flexconnect_baremetal_testapp_CORE_DEPENDENCY
vhwa_flexconnect_baremetal_testapp_PKG_LIST = vhwa_flexconnect_baremetal_testapp
vhwa_flexconnect_baremetal_testapp_INCLUDE = $(vhwa_flexconnect_baremetal_testapp_PATH)
vhwa_flexconnect_baremetal_testapp_BOARDLIST = $(vhwa_default_BOARDLIST)
export vhwa_flexconnect_baremetal_testapp_BOARDLIST
vhwa_flexconnect_baremetal_testapp_$(SOC)_CORELIST = $(vhwa_vpac_$(SOC)_CORELIST)
export vhwa_flexconnect_baremetal_testapp_$(SOC)_CORELIST
vhwa_EXAMPLE_LIST += vhwa_flexconnect_baremetal_testapp
