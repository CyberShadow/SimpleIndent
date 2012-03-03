@echo off
if exist out rmdir /S /Q out
mkdir out
for %%a in (COPYING *.txt) do copy %%a out\
for %%b in (32 64) do for %%f in (1 2 3) do (
  mkdir out\Far%%f\%%b-bit
  copy src\final.%%b.Far%%f.vc\SimpleIndent.dll out\Far%%f\%%b-bit\
)
mkdir out\src
copy src\* out\src\
cd out
7z -mx9 a ..\SimpleIndent.7z *
cd ..
