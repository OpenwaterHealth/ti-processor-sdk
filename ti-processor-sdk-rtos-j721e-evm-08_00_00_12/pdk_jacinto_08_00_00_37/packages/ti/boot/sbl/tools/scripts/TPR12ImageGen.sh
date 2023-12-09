#!/bin/bash

# K3ImageGen <CoreID> <.out> [CoreID2] [.out2] ....
# Core ID definition:
#       MCU1_CPU0_ID : 0
#       MCU1_CPU1_ID : 1
#       DSP1_C66X_ID : 2
#       MCU1_SMP_ID  : 3
#       ONLY_LOAD_ID : 4
#
# Device Id for TPR12 is 55
# Device ID & CPU ID should be in sync with SBL. Refer to SBL user guide for values
#  
# CoreID is provided by the user of the script
#
#

export Dev_ID=55


if [ -z $TOOLS_PATH ]; then
    export TOOLS_PATH=$PDK_INSTALL_PATH/ti/boot/sbl/tools
fi



# validating number of parametters passed in
if [ "$#" -lt 2 ]; then
    echo "Invalid number of parameters."
    echo "Syntax:"
    echo "$0 <CoreID> <.out> [<CoreID2> <.out2>] .... "
    echo ""
    exit
else
    export COREID=$1
    export APP_OUT_FILE=$2
    export APP_OUT_NAME=$(basename ${2%.*})
    export APP_OUT_DIR=$(dirname $2)
    echo "Converting $APP_OUT_FILE to SBL loadable format for core #$COREID ..."
fi

if [ ! -z $BIN_PATH ]
then
    echo "BIN_PATH defined"
else
    echo "BIN_PATH is not defined"
    export BIN_PATH=$(cd $APP_OUT_DIR; pwd)
fi

mkdir -p $BIN_PATH
echo "Using $BIN_PATH directory"

if [ -f $APP_OUT_FILE  ]
then
    export image_gen=1
    export APP_RPRC="$BIN_PATH/$APP_OUT_NAME.rprc"
    mono "$TOOLS_PATH/out2rprc/bin/out2rprc.exe" $APP_OUT_FILE $APP_RPRC
    echo "Generated $APP_RPRC"
else
    echo "$APP_OUT_FILE does not exist"
fi


# ImageGen
if [ ! -z $image_gen  ]
then
    # Generating MulticoreImage Gen
    # doing single core binary now. 
    # TODO: extend to multi-core image gen by adding more COREID and core's RPRC image.
    rm -f $BIN_PATH/$APP_OUT_NAME.appimage $BIN_PATH/$APP_OUT_NAME.appimage.signed
    "$TOOLS_PATH/multicoreImageGen/bin/MulticoreImageGen" LE $Dev_ID $BIN_PATH/$APP_OUT_NAME.appimage $COREID $APP_RPRC
fi

#Undefine the image_gen variable to avoid re generation of the app Image.
unset image_gen
