#
# This file is the makefile for building SA2UL test application.
#
include $(PDK_INSTALL_PATH)/ti/build/Rules.make

ifeq ($(IS_BAREMETAL),yes)
BUILD_OS_TYPE = baremetal
# Build Specific flags
CFLAGS_LOCAL_DEFINES = -DBAREMETAL
# -DAM64X_USE_DEFAULT_FLOW
# Linker flags
LNKFLAGS_LOCAL_mpu1_0  += --entry Entry
COMP_LIST_COMMON = $(PDK_COMMON_BAREMETAL_COMP)
SRCDIR = $(PDK_SA_COMP_PATH)/test/SaUnitTest/src/utils/baremetal
INCDIR = $(PDK_SA_COMP_PATH)/test/SaUnitTest/src/utils/baremetal
# below are empty for baremetal builds
EXTERNAL_INTERFACES =
XDC_CFG_FILE_mpu1_0 =
XDC_CFG_FILE_mcu1_0 =
else
BUILD_OS_TYPE = tirtos
# Build Specific flags
CFLAGS_LOCAL_DEFINES = -DUSE_BIOS
EXTERNAL_INTERFACES = bios xdc
ifeq ($(SOC), am65xx)
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_mpu1_0 = $(PDK_INSTALL_PATH)/ti/build/am65xx/sysbios_a53.cfg
XDC_CFG_FILE_mcu1_0 = $(PDK_INSTALL_PATH)/ti/build/am65xx/sysbios_r5f.cfg
endif

ifeq ($(SOC), am64x)
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_mpu1_0 = $(PDK_INSTALL_PATH)/ti/build/am64x/sysbios_a53.cfg
XDC_CFG_FILE_mcu1_0 = $(PDK_INSTALL_PATH)/ti/build/am64x/sysbios_r5f.cfg
endif

ifeq ($(SOC), j721e)
# Enable XDC build for application by providing XDC CFG File per core
XDC_CFG_FILE_mpu1_0 = $(PDK_SA_COMP_PATH)/test/SaUnitTest/j721e/sysbios_a72.cfg
XDC_CFG_FILE_mcu1_0 = $(PDK_SA_COMP_PATH)/test/SaUnitTest/j721e/sysbios_r5f.cfg
endif

# Start with empty list
COMP_LIST_COMMON    = $(PDK_COMMON_TIRTOS_COMP)
SRCDIR              =
INCDIR              =
endif


ifeq ($(BUILD_OS_TYPE), tirtos)
SA_OS_TYPE=
else
SA_OS_TYPE=_Baremetal
endif

#Name of the directory created under packages/ti/binary/
APP_NAME = SA$(SA_OS_TYPE)_TestApp
# Name of the binary if different from the default (APP_NAME)_$(BOARD_$(CORE)_<build_profile>
LOCAL_APP_NAME =  SA$(SA_OS_TYPE)_$(BOARD)_$(CORE)TestApp

SRCDIR += $(PDK_SA_COMP_PATH)/test/SaUnitTest/src
SRCDIR += $(PDK_SA_COMP_PATH)/test/SaUnitTest/src/common
SRCDIR += $(PDK_SA_COMP_PATH)/test/SaUnitTest/src/salldsim
SRCDIR += $(PDK_SA_COMP_PATH)/test/SaUnitTest/src/tests
SRCDIR += $(PDK_SA_COMP_PATH)/test/SaUnitTest/$(SOC)
INCDIR += $(PDK_SA_COMP_PATH)/test/SaUnitTest/src
INCDIR += $(PDK_SA_COMP_PATH)/test/SaUnitTest/src/common
INCDIR += $(PDK_SA_COMP_PATH)/test/SaUnitTest/src/salldsim
INCDIR += $(PDK_SA_COMP_PATH)/test/SaUnitTest/src/tests
INCDIR += $(PDK_SA_COMP_PATH)/../../..

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk $(EXTERNAL_INTERFACES)

# List all the components required by the application
ifeq ($(SOC),$(filter $(SOC), am65xx j721e am64x))
COMP_LIST_COMMON += salite2
else
COMP_LIST_COMMON += cppi qmss
endif

ifeq ($(CORE),$(filter $(CORE), mpu1_0))
    ifeq ($(SOC),$(filter $(SOC), am65xx am64x))
        EXTERNAL_LNKCMD_FILE_LOCAL = $(PDK_SA_COMP_PATH)/test/SaUnitTest/am65xx/linker_sa2ul_a53.lds
    endif
    ifeq ($(SOC), j721e)
        EXTERNAL_LNKCMD_FILE_LOCAL = $(PDK_SA_COMP_PATH)/test/SaUnitTest/$(SOC)/linker_sa2ul_a72.lds
    endif
endif

ifeq ($(CORE),$(filter $(CORE), mcu1_0))
    ifeq ($(SOC),$(filter $(SOC), am64x j721e))
        ifeq ($(IS_BAREMETAL),yes)
            EXTERNAL_LNKCMD_FILE_LOCAL = $(PDK_SA_COMP_PATH)/test/SaUnitTest/$(SOC)/linker_r5.lds
        else
            EXTERNAL_LNKCMD_FILE_LOCAL = $(PDK_SA_COMP_PATH)/test/SaUnitTest/$(SOC)/linker_r5_sysbios.lds
        endif
    endif
endif

INCLUDE_INTERNAL_INTERFACES =
CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS) $(CFLAGS_LOCAL_DEFINES) -DNSS_LITE2 -DNSS_PKA_GEN2 -DSIMULATOR_SUPPORT -DDISABLE_CYCLE_DELAY

# Cache Coherent settings
ifeq ($(SOC),$(filter $(SOC), am65xx j721e))
    ifeq ($(CORE), mpu1_0)
        CFLAGS_LOCAL_COMMON += -DTEST_CORE_CACHE_COHERENT
    endif
endif

# AM64X specific DMA, Cache Coherent settings
ifeq ($(SOC),$(filter $(SOC), am64x))
     CFLAGS_LOCAL_COMMON += -DDMA_TYPE_LCDMA
    ifeq ($(CORE), mpu1_0)
        CFLAGS_LOCAL_COMMON += -DACP_COHERENCY
    endif
endif

PACKAGE_SRCS_COMMON = src makefile $(SOC) ../Module.xs

# Common source files and CFLAGS across all platforms and cores
#SRCS_SA_UT_UTILS  = mmr.c padconfig.c pll.c psc.c
SRCS_SA_UT_UTILS  =
SRCS_SA_UT_SOC    = sa_rescfg.c
SRCS_SA_UT_TOP    = framework.c testconn2.c  testMain.c  testmem.c  testutil.c
SRCS_SA_UT_COMMON = common2.c
SRCS_SA_UT_TESTS  = test5_1.c test5_2.c test5_3.c test1_7.c
SRCS_SA_UT_SIMLLD = salldcfg.c  salld_osal.c  salldsim.c  salldutil.c

SRCS_COMMON       = $(SRCS_SA_UT_TOP) $(SRCS_SA_UT_SOC) $(SRCS_SA_UT_COMMON) $(SRCS_SA_UT_TESTS) $(SRCS_SA_UT_UTILS) $(SRCS_SA_UT_SIMLLD)
SRCS_ASM_COMMON   = $(SRCS_SA_UT_UTILS_ASM)

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

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
