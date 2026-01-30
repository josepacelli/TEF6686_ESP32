@echo off
setlocal enabledelayedexpansion
cls
color f0
echo ======================================
echo TEF6686_ESP32 - Build and Flash Tool
echo ======================================
echo.

REM Definir paths
set "BUILD_DIR=build\esp32.esp32.esp32"
set "OUTPUT_DIR=build"
set "MKSPIFFS_PATH=C:\Users\josep\AppData\Local\Arduino15\packages\esp32\tools\mkspiffs\0.2.3\mkspiffs.exe"

echo.
set /p COMPILE="Do you want to compile again? (y/n): "
if /i "!COMPILE!"=="y" (
    echo.
    echo Opening Arduino IDE...
    echo Please compile: Sketch ^> Export Compiled Binary
    echo.
    start "" "TEF6686_ESP32.ino"
    set /p READY="Press Enter when compilation is done: "
)

echo.
echo Step 0: Check and export binaries...
echo.

REM Verificar se o firmware ja existe
if not exist "%BUILD_DIR%\TEF6686_ESP32.ino.bin" (
    echo [WARNING] Firmware binary not found in %BUILD_DIR%
    echo.
    echo You need to export the binary from Arduino IDE:
    echo 1. In Arduino IDE: Sketch ^> Export Compiled Binary
    echo 2. Or enable "Show verbose output during compilation"
    echo    in File ^> Preferences
    echo.
    echo The binary will be generated in: %BUILD_DIR%
    echo.
    set /p CONTINUE="Press Enter to continue when ready, or type 'abort' to cancel: "
    if /i "!CONTINUE!"=="abort" (
        echo Aborted.
        pause >NUL
        exit /b 1
    )
    if not exist "%BUILD_DIR%\TEF6686_ESP32.ino.bin" (
        echo.
        echo [ERROR] Firmware binary still not found!
        pause >NUL
        exit /b 1
    )
)

echo [OK] All required binaries found
echo.

REM Verificar se mkspiffs existe
if not exist "%MKSPIFFS_PATH%" (
    echo [ERROR] mkspiffs.exe not found at: %MKSPIFFS_PATH%
    echo Please verify the mkspiffs path for your ESP32 version.
    pause >NUL
    exit /b 1
)

echo Step 1: Generate SPIFFS binary...
echo.

REM Gerar SPIFFS com tamanho correto (800KB)
"%MKSPIFFS_PATH%" -c data -b 4096 -p 256 -s 819200 "%OUTPUT_DIR%\TEF6686_ESP32.spiffs.bin" >NUL 2>&1

if %ERRORLEVEL% neq 0 (
    echo [FAIL] Error generating SPIFFS binary
    pause >NUL
    exit /b 1
)
echo [OK] SPIFFS binary generated: %OUTPUT_DIR%\TEF6686_ESP32.spiffs.bin
echo.

echo Step 2: Copy binary files from build folder...
echo.

REM Copiar bootloader
if exist "%BUILD_DIR%\TEF6686_ESP32.ino.bootloader.bin" (
    copy /Y "%BUILD_DIR%\TEF6686_ESP32.ino.bootloader.bin" "%OUTPUT_DIR%\bootloader.bin" >NUL
    echo [OK] bootloader.bin copied
) else (
    echo [FAIL] bootloader.bin not found
    pause >NUL
    exit /b 1
)

REM Copiar partitions
if exist "%BUILD_DIR%\TEF6686_ESP32.ino.partitions.bin" (
    copy /Y "%BUILD_DIR%\TEF6686_ESP32.ino.partitions.bin" "%OUTPUT_DIR%\partitions.bin" >NUL
    echo [OK] partitions.bin copied
) else (
    echo [FAIL] partitions.bin not found
    pause >NUL
    exit /b 1
)

REM Copiar boot_app0
if exist "%BUILD_DIR%\boot_app0.bin" (
    copy /Y "%BUILD_DIR%\boot_app0.bin" "%OUTPUT_DIR%\boot_app0.bin" >NUL
    echo [OK] boot_app0.bin copied
) else (
    echo [FAIL] boot_app0.bin not found
    pause >NUL
    exit /b 1
)

REM Copiar firmware
if exist "%BUILD_DIR%\TEF6686_ESP32.ino.bin" (
    copy /Y "%BUILD_DIR%\TEF6686_ESP32.ino.bin" "%OUTPUT_DIR%\TEF6686_ESP32.ino.bin" >NUL
    echo [OK] TEF6686_ESP32.ino.bin copied
) else (
    echo [FAIL] TEF6686_ESP32.ino.bin not found
    pause >NUL
    exit /b 1
)

echo.
echo ======================================
echo All binaries ready for flashing!
echo ======================================
echo.

REM Calcular tamanho do firmware e endereco correto do SPIFFS
echo Step 3: Calculate SPIFFS address...
echo.

for /f %%A in ('powershell -NoProfile -Command "((Get-Item '%OUTPUT_DIR%\TEF6686_ESP32.ino.bin').Length + 0x10000 + 0xFFFF) -band 0xFFFF0000 | ForEach-Object { [string]::Format('0x{0:X}', $_) }"') do set "SPIFFS_ADDR=%%A"

echo Firmware: %OUTPUT_DIR%\TEF6686_ESP32.ino.bin
echo SPIFFS binary: %OUTPUT_DIR%\TEF6686_ESP32.spiffs.bin
echo SPIFFS address: %SPIFFS_ADDR% (auto-calculated, alinhado a 64KB)
echo.

echo ======================================
echo Flash command (change COM16 if needed):
echo ======================================
echo.
echo esptool.exe --chip esp32 --port COM16 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 build\bootloader.bin 0x8000 build\partitions.bin 0xe000 build\boot_app0.bin 0x10000 build\TEF6686_ESP32.ino.bin %SPIFFS_ADDR% build\TEF6686_ESP32.spiffs.bin
echo.
pause >NUL
