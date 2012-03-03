@echo off
if exist out rmdir /S /Q out
mkdir out
for %%a in (COPYING *.txt) do copy %%a out\
copy src\final.32W.vc\SimpleIndent.dll out\SimpleIndent_x86.dll
copy src\final.64W.vc\SimpleIndent.dll out\SimpleIndent_x64.dll
mkdir out\src
copy src\* out\src\
cd out
7z -mx9 a ..\SimpleIndent.7z *
cd ..
