@echo off
set MAKER=nmake -f makefile_vc
set FARROOT=..\..\..
%MAKER% FARDEV=%FARROOT%\FAR2\fardev                       DIRSUFF=.Far1
%MAKER% FARDEV=%FARROOT%\FAR2\fardev WIDE=1                DIRSUFF=.Far2
%MAKER% FARDEV=%FARROOT%\FAR\fardev DEF_NAME=SimpleIndent3 DIRSUFF=.Far3
