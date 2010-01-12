@echo off
if not %POCKETBOOKSDK%.==. goto C1
echo Environment variable POCKETBOOKSDK is not set
pause
:C1
set PATH=%POCKETBOOKSDK%\bin;%PATH%

set INCLUDE=
set LIBS=-linkview -lfreetype -ljpeg -lz -lgdi32
set OUTPUT=PBTanks.exe

rm -f %OUTPUT%

set IMAGES=
if not exist images\*.bmp goto NOIMG
set IMAGES=%TEMP%\images.temp.c
pbres -c %IMAGES% images/*.bmp
if errorlevel 1 goto L_ER
:NOIMG

@echo on
gcc -static -Wall -O2 -fomit-frame-pointer %INCLUDE% -o %OUTPUT% src/*.c %IMAGES% %LIBS%
@echo off

if errorlevel 1 goto L_ER

goto END

rem exit 0

:L_ER
echo Build error
rem exit 1

:END
