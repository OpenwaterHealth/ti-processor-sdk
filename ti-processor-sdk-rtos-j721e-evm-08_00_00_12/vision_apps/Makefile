
include vision_apps_tools_path.mak

# Edit below file to change default build options
include vision_apps_build_flags.mak

GIT_HASH := $(shell git describe --abbrev=7 --dirty --always --tags)
GIT_VERSION := $(shell git describe --tags)

# Project specific build defs (don't change across different combos):
BUILD_DEFS :=
ifeq ($(BUILD_IVISION_KERNELS),yes)
BUILD_DEFS += BUILD_IVISION_KERNELS
endif
ifneq ($(CUSTOM_KERNEL_PATH),)
BUILD_DEFS += CUSTOM_KERNEL_PATH
endif
ifneq ($(APP_CONFIG_BASE_PATH),)
BUILD_DEFS += APP_CONFIG_BASE_PATH
endif
ifeq ($(BUILD_TUTORIAL),yes)
BUILD_DEFS += BUILD_TUTORIAL
endif
ifeq ($(BUILD_CONFORMANCE_TEST),yes)
BUILD_DEFS += BUILD_CONFORMANCE_TEST
endif

DIRECTORIES :=
DIRECTORIES += utils
DIRECTORIES += kernels
DIRECTORIES += modules
DIRECTORIES += applibs
DIRECTORIES += apps

TARGET_COMBOS :=

ifeq ($(BUILD_TARGET_MODE),yes)
  ifeq ($(PROFILE), $(filter $(PROFILE), debug all))
	ifeq ($(BUILD_ISA_R5F),yes)
	TARGET_COMBOS += J7:$(RTOS):R5F:3:debug:TIARMCGT
	endif
	ifeq ($(BUILD_ISA_C6x),yes)
	TARGET_COMBOS += J7:SYSBIOS:C66:2:debug:CGT6X
	endif
	ifeq ($(BUILD_ISA_C7x),yes)
	TARGET_COMBOS += J7:SYSBIOS:C71:1:debug:CGT7X
	endif
	ifeq ($(BUILD_ISA_A72),yes)
		ifeq ($(BUILD_LINUX_A72),yes)
		TARGET_COMBOS += J7:LINUX:A72:1:debug:GCC_LINUX_ARM
		endif
		ifeq ($(BUILD_QNX_A72),yes)
		TARGET_COMBOS += J7:QNX:A72:1:debug:GCC_QNX_ARM
		endif
	endif
  endif

  ifeq ($(PROFILE), $(filter $(PROFILE), release all))
	ifeq ($(BUILD_ISA_R5F),yes)
	TARGET_COMBOS += J7:$(RTOS):R5F:3:release:TIARMCGT
	endif
	ifeq ($(BUILD_ISA_C6x),yes)
	TARGET_COMBOS += J7:SYSBIOS:C66:2:release:CGT6X
	endif
	ifeq ($(BUILD_ISA_C7x),yes)
	TARGET_COMBOS += J7:SYSBIOS:C71:1:release:CGT7X
	endif
	ifeq ($(BUILD_ISA_A72),yes)
		ifeq ($(BUILD_LINUX_A72),yes)
		TARGET_COMBOS += J7:LINUX:A72:1:release:GCC_LINUX_ARM
		endif
		ifeq ($(BUILD_QNX_A72),yes)
		TARGET_COMBOS += J7:QNX:A72:1:release:GCC_QNX_ARM
		endif
	endif
  endif

endif

ifeq ($(BUILD_EMULATION_MODE),yes)
  ifeq ($(OS),Windows_NT)
    ifeq ($(PROFILE), $(filter $(PROFILE), debug all))
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), x86_64 all))
            TARGET_COMBOS += PC:WINDOWS:x86_64:1:debug:GCC_WINDOWS
        endif
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), X86 all))
            TARGET_COMBOS += PC:WINDOWS:X86:1:debug:GCC_WINDOWS
        endif
    endif

    ifeq ($(PROFILE), $(filter $(PROFILE), release all))
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), x86_64 all))
            TARGET_COMBOS += PC:WINDOWS:x86_64:1:release:GCC_WINDOWS
        endif
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), X86 all))
            TARGET_COMBOS += PC:WINDOWS:X86:1:release:GCC_WINDOWS
        endif
    endif
  else
    ifeq ($(PROFILE), $(filter $(PROFILE), debug all))
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), x86_64 all))
            TARGET_COMBOS += PC:LINUX:x86_64:1:debug:GCC_LINUX
        endif
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), X86 all))
            TARGET_COMBOS += PC:LINUX:X86:1:debug:GCC_LINUX
        endif
    endif

    ifeq ($(PROFILE), $(filter $(PROFILE), release all))
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), x86_64 all))
            TARGET_COMBOS += PC:LINUX:x86_64:1:release:GCC_LINUX
        endif
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), X86 all))
            TARGET_COMBOS += PC:LINUX:X86:1:release:GCC_LINUX
        endif
    endif
  endif
endif

CONCERTO_ROOT ?= concerto
BUILD_MULTI_PROJECT := 1
BUILD_TARGET := target.mak
BUILD_PLATFORM :=

include $(CONCERTO_ROOT)/rules.mak

# Project specific rules

.PHONY: all vision_apps sdk
all:

doxy_docs:
	-$(Q)$(MKDIR) docs/user_guide/ $(QUIET) || true
	-$(Q)$(COPY) tools/3d_calibration_tool/PSDK_RTOS_UserGuide_3D_SurroundView_Manual_CalibTool.pdf docs/user_guide/
	-$(Q)$(COPY) tools/3d_calibration_tool/poster_calib_chart.pdf docs/user_guide/
	$(DOXYGEN) internal_docs/doxy_cfg_user_guide/user_guide_linux.cfg 2> internal_docs/doxy_cfg_user_guide/doxy_warnings.txt
	-$(Q)$(COPY) internal_docs/doxy_cfg_user_guide/theme/*.png docs/user_guide/

doxy_design_docs:
	$(DOXYGEN) internal_docs/doxy_cfg_design/design_guide.cfg 2> internal_docs/doxy_cfg_design/doxy_warnings.txt

doxy_datasheet_docs:
	-$(Q)$(MKDIR) docs/datasheet/ $(QUIET) || true
	$(DOXYGEN) internal_docs/doxy_cfg_datasheet/datasheet.cfg 2> internal_docs/doxy_cfg_datasheet/doxy_warnings.txt

# Additional make targets to build various related components
include makerules/makefile_pdk.mak
include makerules/makefile_tidl_mmalib.mak
include makerules/makefile_tiovx_ptk_imaging_remote_device.mak
include makerules/makefile_test_data.mak
include makerules/makefile_linux_arm.mak
include makerules/makefile_help.mak
include makerules/makefile_config.mak
include makerules/makefile_tiadalg.mak
include makerules/makefile_check_paths.mak
include makerules/makefile_qnx.mak
include makerules/makefile_sbl.mak
include makerules/makefile_ethfw.mak
include makerules/makefile_ipk.mak
include makerules/makefile_uboot.mak

tmp_clean:
	rm -rf tmp/

vision_apps: sdk_check_paths
	touch ${PSDK_PATH}/vision_apps/apps/basic_demos/app_rtos/rtos_linux/mcu2_0/mcu2_0.cfg
	rm -f ${PSDK_PATH}/vision_apps/git_version.h
	@echo "#ifndef GIT_VERSION_H" >> git_version.h
	@echo "#define GIT_VERSION_H" >> git_version.h
	@echo "" >> git_version.h
	@echo "#define GIT_BUILD_DATE " >> git_version.h
	@echo "#define GIT_BUILD_HASH  " >> git_version.h
	@echo "#define GIT_BUILD_VERSION  " >> git_version.h
	@echo "" >> git_version.h
	@echo "#endif /* GIT_VERSION_H */" >> git_version.h
	
	sed -i "s/#define GIT_BUILD_HASH .*/#define GIT_BUILD_HASH \"$(GIT_HASH)\"/" ${PSDK_PATH}/vision_apps/git_version.h
	sed -i "s/#define GIT_BUILD_VERSION .*/#define GIT_BUILD_VERSION \"$(GIT_VERSION)\"/" ${PSDK_PATH}/vision_apps/git_version.h
	sed -i "s/#define GIT_BUILD_DATE.*/#define GIT_BUILD_DATE \"$$(date '+%Y-%m-%d %H:%M:%S %Z')\"/" ${PSDK_PATH}/vision_apps/git_version.h
	$(MAKE) all
vision_apps_clean: sdk_check_paths pdk_emu_clean clean tmp_clean
vision_apps_scrub: sdk_check_paths
	rm -rf out/

vision_apps_docs: sdk_check_paths doxy_docs

sdk: sdk_check_paths pdk ethfw remote_device imaging ptk tiovx tiadalg qnx
	$(MAKE) vision_apps
	$(MAKE) tidl_rt
ifeq ($(BUILD_CPU_MCU1_0),yes)
	$(MAKE) uboot
endif

sdk_clean: sdk_check_paths pdk_clean ethfw_clean remote_device_clean imaging_clean ptk_clean tiovx_clean tidl_clean tiadalg_clean vision_apps_clean qnx_clean sbl_bootimage_clean tidl_rt_clean tmp_clean
ifeq ($(BUILD_CPU_MCU1_0),yes)
	$(MAKE) uboot_clean
endif

sdk_scrub: sdk_check_paths pdk_scrub ethfw_scrub remote_device_scrub imaging_scrub ptk_scrub tiovx_scrub tidl_scrub tiadalg_scrub vision_apps_scrub qnx_scrub sbl_bootimage_scrub tidl_rt_scrub
ifeq ($(BUILD_CPU_MCU1_0),yes)
	$(MAKE) uboot_clean
endif

sdk_docs: sdk_check_paths tiovx_docs vision_apps_docs ptk_docs tiadalg_docs
	$(MAKE) -C $(PSDK_PATH)/psdk_rtos sphinx_docs

#KW build: Split the build into two - components which need not be part of
# SDK KW report as the report for these components are separately delivered
# Build it first and the use "kw_build" target for KW inject
kw_pre_build: pdk

kw_build: sdk
