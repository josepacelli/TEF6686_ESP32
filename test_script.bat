@echo off
setlocal enabledelayedexpansion

set "BUILD_DIR=build\esp32.esp32.esp32"
set "OUTPUT_DIR=build"

echo Testing copy command...
echo.

REM Clean old .bin files from output directory
echo Cleaning old bin files...
for /f %%A in ('dir /b "!OUTPUT_DIR!\*.bin" 2^>nul') do del /Q "!OUTPUT_DIR!\%%A"

echo.
echo Copying new bin files...

REM Copy all .bin files from build directory
for %%A in ("!BUILD_DIR!\*.bin") do copy /Y "%%A" "!OUTPUT_DIR!\" >nul 2>&1

REM Verify copy was successful
if exist "!OUTPUT_DIR!\TEF6686_ESP32.ino.bin" (
    echo [OK] Binary files copied from !BUILD_DIR!
) else (
    echo [WARNING] Error copying binary files
)

echo.
echo Done!
pause
