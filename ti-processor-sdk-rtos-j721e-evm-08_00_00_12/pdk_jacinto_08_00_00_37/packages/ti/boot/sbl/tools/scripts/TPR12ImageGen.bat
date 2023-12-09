@echo off
REM Define Device Id for TPR12 - 55
REM Device ID & CPU ID should be in sync with SBL. Refer SBL user guide for values
set Dev_ID=55

REM MCU1_CPU0_ID = 0
REM MCU1_CPU1_ID = 1
REM DSP1_C66X_ID = 2
REM MCU1_SMP_ID  = 3
REM ONLY_LOAD_ID = 4

if not defined TOOLS_PATH (
set TOOLS_PATH=%PDK_INSTALL_PATH%\ti\boot\sbl\tools
)

REM checking arguments
if -%1-==-- goto noArgProvided
if -%2-==-- goto noArgProvided

REM we now have enough 2 parameters
set APP_OUT_FILE=%2
set CORE_ID=%1
set APP_OUT_NAME=%~n2
set APP_OUT_DIR=%~dp0
set APP_OUT_DIR=%APP_OUT_DIR:~0,-1%

REM Define Output file path
if not defined BIN_PATH (
echo BIN_PATH not defined.
set BIN_PATH=%APP_OUT_DIR%
)

IF NOT EXIST %BIN_PATH%\ mkdir %BIN_PATH%

echo BIN_PATH set to %BIN_PATH%

IF EXIST %APP_OUT_FILE% (
set image_gen=1
set APP_RPRC=%BIN_PATH%\%APP_OUT_NAME%.rprc
)


if defined APP_RPRC (
echo converting %APP_OUT_FILE% to RPRC format for core #%CORE_ID%
echo TOOLS_PATH=%TOOLS_PATH%
"%TOOLS_PATH%\out2rprc\bin\out2rprc.exe" %APP_OUT_FILE% %APP_RPRC% )


REM ImageGen
if defined image_gen (
echo Generating MulticoreImage 
"%TOOLS_PATH%\multicoreImageGen\bin\MulticoreImageGen.exe" LE %Dev_ID% %BIN_PATH%\%APP_OUT_NAME%.appimage %CORE_ID% %APP_RPRC%

if not defined image_gen (
echo. Error Application .out missing!! )

REM Clearing the image gen flag
set image_gen=
goto done


:noArgProvided
echo Invalid number of parameters.
echo Syntax:
echo "%~nx0 <CoreID> <.out> [<CoreID2> <.out2>] ...."

:done

