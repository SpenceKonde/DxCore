@echo off
setlocal
pushd "%~dp0"
REM ============================================================
REM  build_all_du.bat   (cmd-native, ASCII-only auto-detect)
REM  Builds the USB CDC bootloader as FIVE LED/flash-size classes
REM  (optiboot dd/dd14-style) and collects them into  ..\hex\ :
REM
REM    class        LED   covers                compiled with
REM    -----------  ----  --------------------  -------------
REM    16du         PA7   16du20/28/32          avr16du32
REM    16du14       PD6   16du14                avr16du14
REM    32du         PA7   32du20/28/32          avr32du32
REM    32du14       PD6   32du14                avr32du14
REM    64du         PA7   64du28/32             avr64du32
REM
REM  The signature is read from SIGROW at runtime (stk500.c), so one
REM  class hex serves every package that shares its flash size -- the
REM  bootloader always reports the true DEVICEID to avrdude.
REM  main.c defaults the LED to PA7; the 14-pin classes pass
REM  LED_PORT=PORTD LED_PIN=6 to move it to PD6 (no PA7 on 14-pin).
REM  Put this in  DxCore\megaavr\bootloaders\usbcdcboot\  and run it.
REM ============================================================

REM --- find avr-gcc: 5 levels up (Arduino\tools), version auto-detected
set "GCCBIN="
for /d %%d in ("%~dp0..\..\..\..\..\tools\avr-gcc\*") do set "GCCBIN=%%~fd\bin"
if not defined GCCBIN (
  echo ERROR: avr-gcc toolchain folder not found under
  echo   %~dp0..\..\..\..\..\tools\avr-gcc\
  echo Adjust the path in the for /d line to match your install.
  popd & exit /b 1
)
if not exist "%GCCBIN%\avr-gcc.exe" (
  echo ERROR: avr-gcc.exe missing in "%GCCBIN%"
  popd & exit /b 1
)
set "PATH=%GCCBIN%;%PATH%"
if not defined MAKE set MAKE=make

REM            class    mcu         LEDport LEDpin
call :build 16du     avr16du32
call :build 16du14   avr16du14   PORTD   6
call :build 32du     avr32du32
call :build 32du14   avr32du14   PORTD   6
call :build 64du     avr64du32

REM  VREG-off variants (*_novreg): for boards feeding 3.3V into VUSB
REM  externally (power configuration 3s). Selected in boards.txt by the
REM  Tools -> "VUSB Power Source" menu ({bootloader.vregsuffix}=_novreg).
call :buildnv 16du     avr16du32
call :buildnv 16du14   avr16du14   PORTD   6
call :buildnv 32du     avr32du32
call :buildnv 32du14   avr32du14   PORTD   6
call :buildnv 64du     avr64du32

echo.
echo === collecting hex files into ..\hex\ ===
if not exist "..\hex" mkdir "..\hex"
move /y usbcdcboot_*.hex "..\hex\" >nul

echo.
echo === hex files in ..\hex\ ===
dir /b "..\hex\usbcdcboot_*.hex"
popd
endlocal
goto :eof

:build
REM  %1=class tag   %2=mcu   %3=LED port (opt)   %4=LED pin (opt)
echo.
echo ------ building %2  -^> usbcdcboot_%1.hex  (LED %3 %4) ------
del /q src\*.o 2>nul
del /q usbcdcboot_%1.elf usbcdcboot_%1.hex usbcdcboot_%1.lst usbcdcboot_%1.map 2>nul
"%MAKE%" MCU=%2 TARGET=usbcdcboot_%1 LED_PORT=%3 LED_PIN=%4 all
goto :eof

:buildnv
REM  %1=class tag   %2=mcu   %3=LED port (opt)   %4=LED pin (opt)
REM  Same as :build but VREG=0 -> usbcdcboot_%1_novreg.hex (external 3.3V VUSB)
echo.
echo ------ building %2  -^> usbcdcboot_%1_novreg.hex  (LED %3 %4, VREG off) ------
del /q src\*.o 2>nul
del /q usbcdcboot_%1_novreg.elf usbcdcboot_%1_novreg.hex usbcdcboot_%1_novreg.lst usbcdcboot_%1_novreg.map 2>nul
"%MAKE%" MCU=%2 TARGET=usbcdcboot_%1_novreg VREG=0 LED_PORT=%3 LED_PIN=%4 all
goto :eof
