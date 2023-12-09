#!/bin/bash
# ******************************************************************************
# * FILE PURPOSE: Creates the bootable application binary
# ******************************************************************************
# * FILE NAME: pdkAppImageCreate.sh
# *
# * DESCRIPTION:
# *  Creates bootable application binary from the .out file generated by the
# *  CCS projects.
# *
# * USAGE:
# *  pdkAppImageCreate.sh [PDK_PATH] [CG_TOOL_PATH] [APP_PATH] [APP_NAME] [SOC] [PROCESSOR] [SECUREMODE=yes/no]
# *
# *  SECUREMODE parameter is optional. Will fall back to GP build if not specified
# *
# * Copyright (C) 2016 - 2018, Texas Instruments, Inc.
# *****************************************************************************

export PDK_PATH=$1
export PDK_INSTALL_PATH=$PDK_PATH
export CG_TOOL_PATH=$2
export APP_PATH=$3
export APP_NAME=$4
export SOC=$5
export PROCESSOR=$6
export SECUREMODE=$7

if [ $SOC == AM437x ] || [ $SOC == AM335x ]; then
        $CG_TOOL_PATH/bin/arm-none-eabi-objcopy -O binary $APP_NAME.out $APP_NAME.bin
        $PDK_PATH/ti/starterware/tools/ti_image/tiimage_bin/tiimage 0x80000000 NONE $APP_NAME.bin app

        if [ "$SECUREMODE" == "SECUREMODE=yes" ]; then

            # setting the TI_SECURE_SEC_DEV_PKG variable
            source $PDK_PATH/pdksetupenv.sh

            if [ ! -z $TI_SECURE_DEV_PKG ]; then
                echo TI_SECURE_DEV_PKG set. Secure the app ...
                $TI_SECURE_DEV_PKG/scripts/secure-binary-image.sh $APP_NAME.bin "$APP_NAME"_signed.bin
                $PDK_PATH/ti/starterware/tools/ti_image/tiimage_bin/tiimage 0x80000000 NONE "$APP_NAME"_signed.bin "$APP_NAME"_signed_ti.bin
                echo "$APP_NAME"_signed.bin can be loaded by SBL UART boot
                echo "$APP_NAME"_signed_ti.bin can be loaded by SBL MMCSD boot
            else
                echo "SECURE mode requested but TI_SECURE_DEV_PKG is not defined..."
            fi
        fi
fi

if [ $SOC == AM572x ] || [ $SOC == AM571x ] || [ $SOC == AM574x ]; then
    if [ "$PROCESSOR" == "arm" ]; then
        export BIN_PATH=$APP_PATH
        export APP_MPU_CPU0=$APP_PATH/$APP_NAME.out
        export TOOLS_PATH=$PDK_PATH/ti/boot/sbl/tools
        . $PDK_PATH/ti/boot/sbl/tools/scripts/AM57xImageGen.sh
    elif [ "$PROCESSOR" == "dsp" ]; then
        export BIN_PATH=$APP_PATH
        export APP_DSP1=$APP_PATH/$APP_NAME.out
        export TOOLS_PATH=$PDK_PATH/ti/boot/sbl/tools
        . $PDK_PATH/ti/boot/sbl/tools/scripts/AM57xImageGen.sh
    elif [ "$PROCESSOR" == "m4" ]; then
        export BIN_PATH=$APP_PATH
        export APP_IPU1_CPU0=$APP_PATH/$APP_NAME.out
        export TOOLS_PATH=$PDK_PATH/ti/boot/sbl/tools
        . $PDK_PATH/ti/boot/sbl/tools/scripts/AM57xImageGen.sh
    else
        echo Invalid Processor core
    fi
fi

if [ $SOC == K2G ]; then
    if [ "$PROCESSOR" == "arm" ]; then
        export BIN_PATH=$APP_PATH
        export APP_MPU_CPU0=$APP_PATH/$APP_NAME.out
        export TOOLS_PATH=$PDK_PATH/ti/boot/sbl/tools
        . $PDK_PATH/ti/boot/sbl/tools/scripts/K2GImageGen.sh
    elif [ "$PROCESSOR" == "dsp" ]; then
        export BIN_PATH=$APP_PATH
        export APP_DSP0=$APP_PATH/$APP_NAME.out
        export TOOLS_PATH=$PDK_PATH/ti/boot/sbl/tools
        . $PDK_PATH/ti/boot/sbl/tools/scripts/K2GImageGen.sh
    else
        echo Invalid Processor core
    fi
fi

if [ $SOC == OMAPL137 ] || [ $SOC == OMAPL138 ]; then
    if [ "$PROCESSOR" == "arm" ]; then
        export BIN_PATH=$APP_PATH
        export APP_MPU_CPU0=$APP_PATH/$APP_NAME.out
        export TOOLS_PATH=$PDK_PATH/ti/boot/sbl/tools
        . $PDK_PATH/ti/boot/sbl/tools/scripts/OMAPL13xImageGen.sh
    elif [ "$PROCESSOR" == "dsp" ]; then
        export BIN_PATH=$APP_PATH
        export APP_DSP0=$APP_PATH/$APP_NAME.out
        export TOOLS_PATH=$PDK_PATH/ti/boot/sbl/tools
        . $PDK_PATH/ti/boot/sbl/tools/scripts/OMAPL13xImageGen.sh
    else
        echo Invalid Processor core
    fi
fi


if [ $SOC == am65xx ] || [ $SOC == j721e ] || [ $SOC == am64x ]; then
    export CORE_ID=""
    #Refer to SBL document for core ID value (0, or 4 bellow).
    if [ "$PROCESSOR" == "mpu" ]; then
        export CORE_ID=4
    elif [ "$PROCESSOR" == "mcu" ]; then
        export CORE_ID=0
    elif [ "$PROCESSOR" == "mpu1_0" ]; then
        export CORE_ID=0
    elif [ "$PROCESSOR" == "mpu1_1" ]; then
        export CORE_ID=1
    elif [ "$PROCESSOR" == "mpu2_0" ]; then
        export CORE_ID=2
    elif [ "$PROCESSOR" == "mpu2_1" ]; then
        export CORE_ID=3
    elif [ "$PROCESSOR" == "mcu1_0" ]; then
        export CORE_ID=4
    elif [ "$PROCESSOR" == "mcu1_1" ]; then
        export CORE_ID=5
    elif [ "$PROCESSOR" == "mcu2_0" ]; then
        export CORE_ID=6
    elif [ "$PROCESSOR" == "mcu2_1" ]; then
        export CORE_ID=7
    elif [ "$PROCESSOR" == "mcu3_0" ]; then
        export CORE_ID=8
    elif [ "$PROCESSOR" == "mcu3_1" ]; then
        export CORE_ID=9
    elif [ "$PROCESSOR" == "c66xdsp_1" ]; then
        export CORE_ID=10
    elif [ "$PROCESSOR" == "c66xdsp_2" ]; then
        export CORE_ID=11
    elif [ "$PROCESSOR" == "c7x_1" ]; then
        export CORE_ID=12
    elif [ "$PROCESSOR" == "c7x_1" ]; then
        export CORE_ID=13
    elif [ "$PROCESSOR" == "m4f_0" ]; then
        export CORE_ID=14
    fi

    if [ "$CORE_ID" == "" ]; then
        echo Invalid Processor core
    else
        export BIN_PATH=$APP_PATH
        export TOOLS_PATH=$PDK_PATH/ti/boot/sbl/tools
        export PDK_INSTALL_PATH=$PDK_PATH
        . $PDK_PATH/ti/boot/sbl/tools/scripts/K3ImageGen.sh $CORE_ID $APP_PATH/$APP_NAME
    fi
fi