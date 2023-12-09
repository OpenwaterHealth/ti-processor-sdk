@REM ******************************************************************************
@REM * FILE NAME: create_sd.bat
@REM *
@REM * DESCRIPTION:
@REM *  This script file parses the .out executables generated by the diagnostic
@REM *  makefile into SD card loadable files.
@REM *
@REM *  The script expects the board as a parameter. Out2rprc and MulitcoreImageGen
@REM *  can be set manually by environment variables %OUT2RPRC% and 
@REM *  %MULTICOREIMAGEGEN%, respectively. If not set, script will assume default
@REM *  location in %PDK_INSTALL_PATH%/ti/boot/sbl/tools
@REM *
@REM *  Syntax:
@REM *      create_sd.bat [BOARD]
@REM *
@REM *  Example:
@REM *      create_sd.bat idkAM572x
@REM
@REM * Copyright (C) 2015, Texas Instruments, Inc.
@REM *****************************************************************************

@echo off
set board=%1
set target=%2
set cpu_id=%3
set k3_platform=no
set file_ext=*.out

IF "%board%" == "evmAM335x" (
echo Done! create_sd script not needed for %board%
goto :end
)

IF "%board%" == "evmAM437x" (
echo Done! create_sd script not needed for %board%
goto :end
)

IF "%board%" == "idkAM437x" (
echo Done! create_sd script not needed for %board%
goto :end
)

IF "%board%" == "skAM437x" (
echo Done! create_sd script not needed for %board%
goto :end
)

IF "%board%" == "bbbAM335x" (
echo Done! create_sd script not needed for %board%
goto :end
)

IF "%board%" == "icev2AM335x" (
echo Done! create_sd script not needed for %board%
goto :end
)

IF "%board%" == "skAM335x" (
echo Done! create_sd script not needed for %board%
goto :end
) 

set boarddir=%PDK_INSTALL_PATH%/ti/board

IF EXIST %boarddir%/bin/%board%/%target% (
echo Creating SD files for %board%...
) ELSE (
echo Missing folder/files under %boarddir%/bin/%board%/%target%
echo Usage: create_sd.bat [BOARD]
echo     Eg. create_sd.bat idkAM572x
goto :err
)

set out2rprc=%OUT2RPRC%
IF [%out2rprc%] == [] (
	set out2rprc=%PDK_INSTALL_PATH%/ti/boot/sbl/tools/out2rprc/bin/out2rprc.exe
)
IF NOT EXIST %out2rprc% (
	echo Missing out2rprc!
	echo     Not found in default location: %%PDK_INSTALL_PATH
%%/ti/boot/sbl/tools/out2rprc/bin/out2rprc.exe
	echo     Override default location by setting OUT2RPRC, eg:
	echo         set OUT2RPRC=%PDK_INSTALL_PATH%/ti/boot/sbl/tools/out2rprc/bin/out2rprc.exe
	goto :err
) ELSE (
	echo Using out2prc: %out2rprc%
)

set multigen=%MULTICOREIMAGEGEN%
IF [%multigen%] == [] (
	set multigen=%PDK_INSTALL_PATH%/ti/boot/sbl/tools/multicoreImageGen/bin/MulticoreImageGen.exe
)
IF NOT EXIST %multigen% (
	echo Missing multigen!
	echo     Not found in default location: %%PDK_INSTALL_PATH%%/ti/boot/sbl/tools/multicoreImageGen/bin/MulticoreImageGen.exe
	echo     Override default location by setting MULTICOREIMAGEGEN, eg:
	echo         set MULTICOREIMAGEGEN=%PDK_INSTALL_PATH%/ti/boot/sbl/tools/multicoreImageGen/bin/MulticoreImageGen
	goto :err
) ELSE (
	echo Using multigen: %multigen%
)

set sd_dir=%boarddir%/bin/%board%/sd

IF "%board%" == "am65xx_evm" (
set sd_dir=%boarddir%/bin/%board%/sd/%target%
set k3_platform=yes
) 

IF "%board%" == "am65xx_idk" (
set sd_dir=%boarddir%/bin/%board%/sd/%target%
set k3_platform=yes
)

IF "%board%" == "j721e_evm" (
set sd_dir=%boarddir%/bin/%board%/sd/%target%
set k3_platform=yes
IF "%target%" == "armv8" (
	set file_ext=*mpu1_0_release.xa72fg
) ELSE (
	set file_ext=*mcu1_0_release.xer5f
)
)

IF "%k3_platform%" == "yes" (
set x509certgen=powershell -executionpolicy unrestricted -command %PDK_INSTALL_PATH%/ti/build/makerules/x509CertificateGen.ps1
set k3devkey=%PDK_INSTALL_PATH%/ti/build/makerules/k3_dev_mpk.pem
)

md "%sd_dir:/=\%"
PUSHD  %CD%
cd %boarddir%/bin/%board%/%target%
for %%f in (%file_ext%) do call :Parse %%f
goto :loopend
:Parse
set out=%1
echo Parsing %out%
for /f "tokens=1 delims=_" %%a in ("%out%") do set name=%%a
set rprc=%sd_dir%/%name%_rprc
%out2rprc% %out% %rprc%
%multigen% LE 55 %sd_dir%/%name%_TEST %cpu_id% %rprc%
goto :end
:loopend
cd %sd_dir%
IF EXIST framework_TEST (
	IF EXIST frameworkLoader_TEST (
		move framework_TEST framework
		move frameworkLoader_TEST app
	) ELSE (
		move framework_TEST app
	)
) ELSE (
	echo Warning! Diagnostic framework not found!
)
del *_rprc
IF "%k3_platform%" == "yes" (
echo Signing app for K3 platform ...
ren app %sd_dir%/framework_app.us
%x509certgen% -b %sd_dir%/framework_app.us -o %sd_dir%/app -c R5 -l 0x0 -k %k3devkey%
del %sd_dir%/framework_app.us
)
POPD
goto :success

:err
echo Error encountered while running!
goto :end

:success
echo Done! Check %sd_dir% for images to put onto SD card.

:end
