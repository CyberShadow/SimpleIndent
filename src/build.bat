@echo off
call :config nmake.exe
if "%FAR2DEV%"=="" set FAR2DEV=../../../FAR2
if "%FAR3DEV%"=="" set FAR3DEV=../../../FAR

%MAKER% FARDEV=%FAR2DEV%/fardev                        %DIRSUFF%=.Far1
%MAKER% FARDEV=%FAR2DEV%/fardev WIDE=1                 %DIRSUFF%=.Far2
%MAKER% FARDEV=%FAR3DEV%        DEF_NAME=SimpleIndent3 %DIRSUFF%=.Far3 FAR3=1
goto :eof

:config
if "%~$PATH:1"=="" goto config_gnu
goto config_vc

:config_gnu
set MAKER=mingw32-make -f makefile_gcc
set DIRSUFF=DIRSURF
goto :eof

:config_vc
set MAKER=nmake -f makefile_vc
set DIRSUFF=DIRSUFF
goto :eof
