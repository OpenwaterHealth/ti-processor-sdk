# Valid values are: psdkra
BUILD_SDK?=psdkra

include $(BUILD_SDK)_tools_path.mak
include $(TIOVX_PATH)/build_flags.mak

# Project specific build defs (don't change across different combos):
BUILD_DEFS :=
ifeq ($(BUILD_IVISION_KERNELS),yes)
BUILD_DEFS += BUILD_IVISION_KERNELS
endif
ifneq ($(CUSTOM_KERNEL_PATH),)
BUILD_DEFS += CUSTOM_KERNEL_PATH
endif
ifneq ($(CUSTOM_APPLICATION_PATH),)
BUILD_DEFS += CUSTOM_APPLICATION_PATH
endif
ifeq ($(BUILD_TUTORIAL),yes)
BUILD_DEFS += BUILD_TUTORIAL
endif
ifeq ($(BUILD_CONFORMANCE_TEST),yes)
BUILD_DEFS += BUILD_CONFORMANCE_TEST
endif

DIRECTORIES :=
DIRECTORIES += source/platform
DIRECTORIES += source/framework

DIRECTORIES += source/vxu
DIRECTORIES += kernels
DIRECTORIES += utils
DIRECTORIES += $(CUSTOM_KERNEL_PATH)
DIRECTORIES += $(CUSTOM_APPLICATION_PATH)

ifeq ($(BUILD_TUTORIAL),yes)
DIRECTORIES += conformance_tests/test_engine
DIRECTORIES += tutorial
endif

DIRECTORIES += tools/sample_use_cases

ifeq ($(BUILD_CONFORMANCE_TEST),yes)
  DIRECTORIES += conformance_tests
endif

TARGET_COMBOS :=

ifeq ($(BUILD_EMULATION_MODE),yes)
    ifeq ($(PROFILE), $(filter $(PROFILE), debug all))
        TARGET_COMBOS += PC:LINUX:x86_64:1:debug:GCC_LINUX
    endif

    ifeq ($(PROFILE), $(filter $(PROFILE), release all))
        TARGET_COMBOS += PC:LINUX:x86_64:1:release:GCC_LINUX
    endif
endif

ifeq ($(BUILD_TARGET_MODE),yes)
    ifeq ($(PROFILE), $(filter $(PROFILE), debug all))
        TARGET_COMBOS += J7:$(RTOS):R5F:1:debug:TIARMCGT
        TARGET_COMBOS += J7:SYSBIOS:A72:1:debug:GCC_SYSBIOS_ARM
        TARGET_COMBOS += J7:SYSBIOS:C66:1:debug:CGT6X
        TARGET_COMBOS += J7:SYSBIOS:C71:1:debug:CGT7X
        ifeq ($(BUILD_LINUX_A72),yes)
            TARGET_COMBOS += J7:LINUX:A72:1:debug:GCC_LINUX_ARM
        endif
        ifeq ($(BUILD_QNX_A72),yes)
            TARGET_COMBOS += J7:QNX:A72:1:debug:GCC_QNX_ARM
        endif
    endif

    ifeq ($(PROFILE), $(filter $(PROFILE), release all))
        TARGET_COMBOS += J7:$(RTOS):R5F:1:release:TIARMCGT
        TARGET_COMBOS += J7:SYSBIOS:A72:1:release:GCC_SYSBIOS_ARM
        TARGET_COMBOS += J7:SYSBIOS:C66:1:release:CGT6X
        TARGET_COMBOS += J7:SYSBIOS:C71:1:release:CGT7X
        ifeq ($(BUILD_LINUX_A72),yes)
            TARGET_COMBOS += J7:LINUX:A72:1:release:GCC_LINUX_ARM
        endif
        ifeq ($(BUILD_QNX_A72),yes)
            TARGET_COMBOS += J7:QNX:A72:1:release:GCC_QNX_ARM
        endif
    endif
endif

CONCERTO_ROOT ?= concerto
BUILD_MULTI_PROJECT := 1
BUILD_TARGET := target.mak
BUILD_PLATFORM :=

include $(CONCERTO_ROOT)/rules.mak

# Project specific rules

.PHONY: all vision_apps_utils doxy_docs doxy_docs_design
all: vision_apps_utils

ifeq ($(BUILD_EMULATION_MODE),yes)
vision_apps_utils:
	BUILD_TARGET_MODE=no $(MAKE) -C $(VISION_APPS_PATH) app_utils_iss
	BUILD_TARGET_MODE=no $(MAKE) -C $(VISION_APPS_PATH) cp_to_lib
else
vision_apps_utils:

endif

doxy_docs:
	$(DOXYGEN) internal_docs/doxy_cfg_user_guide/user_guide_j7_linux.cfg 2> tiovx_dev/internal_docs/doxy_cfg_user_guide/doxy_warnings.txt
	$(COPY) internal_docs/tiovx_release_notes_psdkra.html $(TIOVX_PATH)/tiovx_release_notes.html
	-rm $(TIOVX_PATH)/docs/test_reports/* -f
	$(MKDIR) $(TIOVX_PATH)/docs/test_reports/
	$(COPY) internal_docs/relnotes_archive/test_reports_j7/* $(TIOVX_PATH)/docs/test_reports/.
	$(COPY) internal_docs/doxy_cfg_user_guide/images/*.pdf $(TIOVX_PATH)/docs/user_guide/.

doxy_docs_design:
	$(DOXYGEN) internal_docs/doxy_cfg_design/design_guide.cfg 2> tiovx_dev/internal_docs/doxy_cfg_design/doxy_warnings.txt
