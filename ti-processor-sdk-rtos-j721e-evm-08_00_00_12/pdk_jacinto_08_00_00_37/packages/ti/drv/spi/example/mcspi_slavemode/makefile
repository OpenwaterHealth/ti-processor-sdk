# Makefile for SPI master/slave test app
include $(PDK_INSTALL_PATH)/ti/build/Rules.make


#Baremetal Specifics
ifeq ($(BUILD_OS_TYPE), baremetal)

    MCSPI_OS_TESTPREFIX=_Baremetal
    CFLAGS_OS_DEFINES = -DBARE_METAL
    LNKFLAGS_LOCAL_mpu1_0 += --entry Entry
    COMP_LIST_COMMON =  $(PDK_COMMON_BAREMETAL_COMP)
    EXTERNAL_INTERFACES =
    XDC_CFG_FILE_mpu1_0 =
    XDC_CFG_FILE_mcu1_0 =

endif

#TI RTOS Specifics
ifeq ($(BUILD_OS_TYPE), tirtos)

    MCSPI_OS_TESTPREFIX=
    CFLAGS_OS_DEFINES = -DUSE_BIOS -DRTOS_ENV
    EXTERNAL_INTERFACES = xdc bios
    COMP_LIST_COMMON = $(PDK_COMMON_TIRTOS_COMP)

    ifeq ($(SOC),$(filter $(SOC), am65xx))
        XDC_CFG_FILE_mpu1_0 = ./am65xx/mcspiMasterSlaveTest_a53.cfg
        XDC_CFG_FILE_mcu1_0 = ./am65xx/mcspiMasterSlaveTest_r5.cfg
    else
        XDC_CFG_FILE_$(CORE) = $(PDK_INSTALL_PATH)/ti/build/$(SOC)/sysbios_$(ISA).cfg
    endif

endif

#FreeRTOS Specifics
ifeq ($(BUILD_OS_TYPE), freertos)

    MCSPI_OS_TESTPREFIX=
    CFLAGS_OS_DEFINES = -DFREERTOS -DRTOS_ENV
    EXTERNAL_INTERFACES = freertos
    COMP_LIST_COMMON = $(PDK_COMMON_FREERTOS_COMP)

endif

SRCDIR = . src
INCDIR = . src

# Common source files across all platforms and cores
SRCS_COMMON += main_mcspi_slave_mode.c SPI_log.c

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk $(EXTERNAL_INTERFACES)

ifeq ($(SOC),$(filter $(SOC), am65xx j721e j7200))
    ifeq ($(CORE), mpu1_0)
        # Slave running on mpu1_0 core 
        CFLAGS_SPI_MS     = -DMCSPI_SLAVE_TASK
        APP_MASTER_SLAVE  = Slave
        ifeq ($(SOC),$(filter $(SOC), am65xx))
        # Slave uses local linker command file to avoid code/data memory conflict with master application
            EXTERNAL_LNKCMD_FILE_LOCAL =  $(PDK_INSTALL_PATH)/ti/drv/spi/example/mcspi_slavemode/am65xx/linker_mpu.lds
        endif
    else
        # Master running on mcu1_0 core, use the common linker command file in build
        CFLAGS_SPI_MS     = -DMCSPI_MASTER_TASK
        APP_MASTER_SLAVE  = Master
        ifeq ($(SOC),$(filter $(SOC), j721e j7200))
            # Slave uses local linker command file to avoid code/data memory conflict with master application
            ifeq ($(BUILD_OS_TYPE), baremetal)
                EXTERNAL_LNKCMD_FILE_LOCAL =  $(PDK_INSTALL_PATH)/ti/drv/spi/example/mcspi_slavemode/$(SOC)/linker_mcu.lds
            endif
            ifeq ($(BUILD_OS_TYPE), tirtos)
                EXTERNAL_LNKCMD_FILE_LOCAL =  $(PDK_INSTALL_PATH)/ti/drv/spi/example/mcspi_slavemode/$(SOC)/linker_mcu_sysbios.lds
            endif
            ifeq ($(BUILD_OS_TYPE), freertos)
                EXTERNAL_LNKCMD_FILE_LOCAL =  $(PDK_INSTALL_PATH)/ti/drv/spi/example/mcspi_slavemode/$(SOC)/linker_mcu_freertos.lds
            endif
        endif
    endif
endif

# DMA enabled build
ifeq ($(DMA), enable)

    MCSPI_DMA_MODE_INFIX = _Dma
    COMP_LIST_COMMON += spi_dma 
    CFLAGS_SPI_DMA    = -DSPI_DMA_ENABLE
else
    MCSPI_DMA_MODE_INFIX =
    COMP_LIST_COMMON += spi
    CFLAGS_SPI_DMA    =
endif


#Name of the directory created under packages/ti/binary/
APP_NAME = MCSPI$(MCSPI_OS_TESTPREFIX)_$(APP_MASTER_SLAVE)$(MCSPI_DMA_MODE_INFIX)_TestApp_$(BUILD_OS_TYPE)

ifeq ($(SOC),$(filter $(SOC), am64x))
CFLAGS_SPI_MS     = -DMCSPI_MASTER_TASK -Dam64x_evm=am64x_evm
APP_MASTER_SLAVE  = Master
endif

ifeq ($(SOC),$(filter $(SOC), am65xx))
    ifeq ($(BOARD),$(filter $(BOARD), am65xx_evm))
        CFLAGS_SPI_MS    += -Dam65xx_evm=am65xx_evm
    endif
    ifeq ($(BOARD),$(filter $(BOARD), am65xx_idk))
        CFLAGS_SPI_MS    += -Dam65xx_idk=am65xx_idk
    endif
endif

ifeq ($(SOC),$(filter $(SOC), j7200))
CFLAGS_SPI_MS    += -Dj7200_evm=j7200_evm
endif

ifeq ($(SOC),$(filter $(SOC), j721e))
CFLAGS_SPI_MS    += -Dj721e_evm=j721e_evm
endif


ifeq ($(BOARD),$(filter $(BOARD), am65xx_evm am65xx_idk j721e_sim j721e_evm j7200_evm am64x_evm))
CFLAGS_SPI_UT    = -DMCSPI_MULT_CHANNEL
endif

ifeq ($(SOC),$(filter $(SOC), j721e j7200))
  ifeq ($(CORE),mcu1_0)
    COMP_LIST_COMMON += sciserver_tirtos
  endif
endif

PACKAGE_SRCS_COMMON = .
CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS) $(CFLAGS_SPI_UT) $(CFLAGS_SPI_DMA) $(CFLAGS_SPI_MS) $(CFLAGS_OS_DEFINES)

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
