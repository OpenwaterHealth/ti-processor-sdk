#
# Utility makefile to build linux components and load updated file system
#
# Edit this file to suit your specific build needs
#

ifeq ($(PROFILE), $(filter $(PROFILE),debug all))
LINUX_APP_PROFILE=debug
endif
ifeq ($(PROFILE), $(filter $(PROFILE),release all))
LINUX_APP_PROFILE=release
endif

FIRMWARE_SUBFOLDER?=vision_apps_evm
LINUX_FS_STAGE_PATH = $(VISION_APPS_PATH)/tmp/tivision_apps_targetfs_stage

linux_fs_stage:
	@rm -rf $(LINUX_FS_STAGE_PATH)
	@mkdir -p $(LINUX_FS_STAGE_PATH)
	@mkdir -p $(LINUX_FS_STAGE_PATH)/usr/lib
	@mkdir -p $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)

ifeq ($(BUILD_CPU_MPU1),yes)
	# copy application binaries and scripts
	mkdir -p $(LINUX_FS_STAGE_PATH)/opt/vision_apps
	mkdir -p $(LINUX_FS_STAGE_PATH)/opt/app_python_modules
	cp $(VISION_APPS_PATH)/out/J7/A72/LINUX/$(LINUX_APP_PROFILE)/*.out $(LINUX_FS_STAGE_PATH)/opt/vision_apps
	cp $(VISION_APPS_PATH)/out/J7/A72/LINUX/$(LINUX_APP_PROFILE)/libtivision_apps.so.$(PSDK_VERSION) $(LINUX_FS_STAGE_PATH)/usr/lib
	cp -P $(VISION_APPS_PATH)/out/J7/A72/LINUX/$(LINUX_APP_PROFILE)/libtivision_apps.so $(LINUX_FS_STAGE_PATH)/usr/lib
	cp -r $(VISION_APPS_PATH)/apps/basic_demos/app_linux_fs_files/* $(LINUX_FS_STAGE_PATH)/opt/vision_apps
	cp -r $(VISION_APPS_PATH)/apps/basic_demos/app_python_modules/* $(LINUX_FS_STAGE_PATH)/opt/app_python_modules
	chmod +x $(LINUX_FS_STAGE_PATH)/opt/vision_apps/*.sh

	# copy ptk cfg files
	$(eval PTK_DEMO_DIR = $(VISION_APPS_PATH)/apps/ptk_demos)
	$(eval PTK_DEMO_CFG_DIR = $(LINUX_FS_STAGE_PATH)/opt/vision_apps/ptk_app_cfg)
	$(eval PTK_APP_LIST = app_surround_radar_ogmap app_dof_sfm_fisheye app_lidar_ogmap app_valet_parking app_sde app_sde_obstacle_detection )
	mkdir -p $(PTK_DEMO_CFG_DIR)
	for var in $(PTK_APP_LIST); do \
		mkdir -p $(PTK_DEMO_CFG_DIR)/$$var; \
		cp -R $(PTK_DEMO_DIR)/$$var/config $(PTK_DEMO_CFG_DIR)/$$var; \
	done

	# Copy header files (variables used in this section are defined in makefile_ipk.mak)
	@# copy all the .h files under folders in IPK_INCLUDE_FOLDERS
	@# https://stackoverflow.com/questions/10176849/how-can-i-copy-only-header-files-in-an-entire-nested-directory-to-another-direct
	for folder in $(IPK_INCLUDE_FOLDERS); do \
		mkdir -p $(LINUX_FS_STAGE_PATH)/$(IPK_TARGET_INC_PATH)/$$folder; \
		(cd $(PSDK_PATH)/$$folder && find . -name '*.h' -print | tar --create --files-from -) | (cd $(LINUX_FS_STAGE_PATH)/$(IPK_TARGET_INC_PATH)/$$folder && tar xfp -); \
	done
	ln -sr $(LINUX_FS_STAGE_PATH)/$(IPK_TARGET_INC_PATH)/$(tidl_dir) $(LINUX_FS_STAGE_PATH)/$(IPK_TARGET_INC_PATH)/tidl_j7
endif

ifeq ($(BUILD_CPU_MCU1_0),yes)
	# copy remote firmware files for mcu1_0
	$(eval IMAGE_NAME := vx_app_rtos_linux_mcu1_0.out)
	cp $(VISION_APPS_PATH)/out/J7/R5F/$(RTOS)/$(LINUX_APP_PROFILE)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/.
	$(TIARMCGT_ROOT)/bin/armstrip -p $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME)
	ln -sr $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/j7-mcu-r5f0_0-fw
else
	# Copy MCU1_0 firmware which is used in the default uboot
	ln -sr $(LINUX_FS_STAGE_PATH)/lib/firmware/pdk-ipc/ipc_echo_testb_mcu1_0_release_strip.xer5f $(LINUX_FS_STAGE_PATH)/lib/firmware/j7-mcu-r5f0_0-fw
endif
ifeq ($(BUILD_CPU_MCU1_1),yes)
	# copy remote firmware files for mcu1_1
	$(eval IMAGE_NAME := vx_app_rtos_linux_mcu1_1.out)
	cp $(VISION_APPS_PATH)/out/J7/R5F/$(RTOS)/$(LINUX_APP_PROFILE)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/.
	$(TIARMCGT_ROOT)/bin/armstrip -p $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME)
	ln -sr $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/j7-mcu-r5f0_1-fw
endif
ifeq ($(BUILD_CPU_MCU2_0),yes)
	# copy remote firmware files for mcu2_0
	$(eval IMAGE_NAME := vx_app_rtos_linux_mcu2_0.out)
	cp $(VISION_APPS_PATH)/out/J7/R5F/$(RTOS)/$(LINUX_APP_PROFILE)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/.
	$(TIARMCGT_ROOT)/bin/armstrip -p $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME)
	ln -sr $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/j7-main-r5f0_0-fw
endif
ifeq ($(BUILD_CPU_MCU2_1),yes)
	# copy remote firmware files for mcu2_1
	$(eval IMAGE_NAME := vx_app_rtos_linux_mcu2_1.out)
	cp $(VISION_APPS_PATH)/out/J7/R5F/$(RTOS)/$(LINUX_APP_PROFILE)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/.
	$(TIARMCGT_ROOT)/bin/armstrip -p $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME)
	ln -sr $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/j7-main-r5f0_1-fw
endif
ifeq ($(BUILD_CPU_MCU3_0),yes)
	# copy remote firmware files for mcu3_0
	$(eval IMAGE_NAME := vx_app_rtos_linux_mcu3_0.out)
	cp $(VISION_APPS_PATH)/out/J7/R5F/$(RTOS)/$(LINUX_APP_PROFILE)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/.
	$(TIARMCGT_ROOT)/bin/armstrip -p $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME)
	ln -sr $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/j7-main-r5f1_0-fw
endif
ifeq ($(BUILD_CPU_MCU3_1),yes)
	# copy remote firmware files for mcu3_1
	$(eval IMAGE_NAME := vx_app_rtos_linux_mcu3_1.out)
	cp $(VISION_APPS_PATH)/out/J7/R5F/$(RTOS)/$(LINUX_APP_PROFILE)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/.
	$(TIARMCGT_ROOT)/bin/armstrip -p $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME)
	ln -sr $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/j7-main-r5f1_1-fw
endif
ifeq ($(BUILD_CPU_C6x_1),yes)
	# copy remote firmware files for c6x_1
	$(eval IMAGE_NAME := vx_app_rtos_linux_c6x_1.out)
	cp $(VISION_APPS_PATH)/out/J7/C66/SYSBIOS/$(LINUX_APP_PROFILE)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/.
	$(CGT6X_ROOT)/bin/strip6x -p $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME)
	ln -sr $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/j7-c66_0-fw
endif
ifeq ($(BUILD_CPU_C6x_2),yes)
	# copy remote firmware files for c6x_2
	$(eval IMAGE_NAME := vx_app_rtos_linux_c6x_2.out)
	cp $(VISION_APPS_PATH)/out/J7/C66/SYSBIOS/$(LINUX_APP_PROFILE)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/.
	$(CGT6X_ROOT)/bin/strip6x -p $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME)
	ln -sr $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/j7-c66_1-fw
endif
ifeq ($(BUILD_CPU_C7x_1),yes)
	# copy remote firmware files for c7x_1
	$(eval IMAGE_NAME := vx_app_rtos_linux_c7x_1.out)
	cp $(VISION_APPS_PATH)/out/J7/C71/SYSBIOS/$(LINUX_APP_PROFILE)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/.
	$(CGT7X_ROOT)/bin/strip7x -p $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME)
	ln -sr $(LINUX_FS_STAGE_PATH)/lib/firmware/$(FIRMWARE_SUBFOLDER)/$(IMAGE_NAME) $(LINUX_FS_STAGE_PATH)/lib/firmware/j7-c71_0-fw

	#Build TIDL test case and copy binaries
	#$(MAKE) -C $(TIDL_PATH)/../ run
	mkdir -p $(LINUX_FS_STAGE_PATH)/opt/tidl_test
	cp -P $(TIDL_PATH)/tfl_delegate/out/J7/A72/LINUX/$(LINUX_APP_PROFILE)/*.so*  $(LINUX_FS_STAGE_PATH)/usr/lib
	cp -P $(TIDL_PATH)/rt/out/J7/A72/LINUX/$(LINUX_APP_PROFILE)/*.so*  $(LINUX_FS_STAGE_PATH)/usr/lib
	cp -P $(TIDL_PATH)/onnxrt_EP/out/J7/A72/LINUX/$(LINUX_APP_PROFILE)/*.so*  $(LINUX_FS_STAGE_PATH)/usr/lib
	cp $(TIDL_PATH)/rt/out/J7/A72/LINUX/$(LINUX_APP_PROFILE)/*.out  $(LINUX_FS_STAGE_PATH)/opt/tidl_test/
	cp -r $(TIDL_PATH)/test/testvecs/ $(LINUX_FS_STAGE_PATH)/opt/tidl_test/
	cp -r $(TIDL_PATH)/test/notebooks/ $(LINUX_FS_STAGE_PATH)/opt/notebooks/
endif
	sync


# MODIFY_FS macro for making PSDK RTOS modifications to the PSDK Linux to file system
# $1 : rootfs path
# $2 : bootfs path
define MODIFY_FS =
	# copy uEnv.txt and sysfw.itb for PSDK RTOS
	cp $(VISION_APPS_PATH)/apps/basic_demos/app_linux_fs_files/uEnv.txt $(2)/
	#cp $(2)/sysfw-psdkra.itb $(2)/sysfw.itb
	# update any additional files specific to PSDK RTOS in the filesystem
	-cp $(VISION_APPS_PATH)/apps/basic_demos/app_linux_fs_files/limits.conf $(1)/etc/security/limits.conf 2> /dev/null
	sync
endef

# CLEAN_COPY_FROM_STAGE macro for updating a file system from the stage fs
# $1 : destination rootfs path
define CLEAN_COPY_FROM_STAGE =
	# remove old remote files from filesystem
	-rm -f $(1)/lib/firmware/j7-*-fw
	-rm -rf $(1)/lib/firmware/$(FIRMWARE_SUBFOLDER)
	-rm -rf $(1)/opt/tidl_test/*
	-rm -rf $(1)/opt/notebooks/*
	-rm -rf $(1)/$(IPK_TARGET_INC_PATH)/*

	# create new directories
	-mkdir -p $(1)/$(IPK_TARGET_INC_PATH)

	# copy full vision apps linux fs stage directory into linux fs
	cp -r $(LINUX_FS_STAGE_PATH)/* $(1)/.
	sync
endef

linux_fs_install: linux_fs_stage
	$(call CLEAN_COPY_FROM_STAGE,$(LINUX_FS_PATH))

linux_fs_install_sd: linux_fs_install
	$(call CLEAN_COPY_FROM_STAGE,$(LINUX_SD_FS_ROOT_PATH))

	$(call MODIFY_FS,$(LINUX_SD_FS_ROOT_PATH),$(LINUX_SD_FS_BOOT_PATH))
ifeq ($(BUILD_CPU_MCU1_0),yes)
	$(MAKE) uboot_linux_install_sd
endif

linux_fs_install_nfs: linux_fs_install
	$(call MODIFY_FS,$(LINUX_FS_PATH),$(LINUX_FS_BOOT_PATH))

linux_fs_install_sd_test_data:
	$(call INSTALL_TEST_DATA,$(LINUX_SD_FS_ROOT_PATH),opt/vision_apps)

linux_fs_install_nfs_test_data:
	$(call INSTALL_TEST_DATA,$(LINUX_FS_PATH),opt/vision_apps)

linux_fs_install_tar: linux_fs_install_nfs linux_fs_install_nfs_test_data
	# Creating bootfs tar
	cd $(LINUX_FS_BOOT_PATH) && tar czf $(VISION_APPS_PATH)/bootfs.tar.gz .
	# Creating rootfs tar
	cd $(LINUX_FS_PATH) && sudo tar cpzf $(VISION_APPS_PATH)/rootfs.tar.xz .
