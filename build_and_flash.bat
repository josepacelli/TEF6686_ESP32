@echo off
setlocal EnableDelayedExpansion
cls

REM =====================================================
REM CONFIGURACOES
REM =====================================================
set "BUILD_DIR=build\esp32.esp32.esp32"
set "OUTPUT_DIR=build"

echo ======================================
echo TEF6686_ESP32 - Build and Flash Tool
echo ======================================
echo.

REM =====================================================
REM LIMPEZA
REM =====================================================
echo Cleaning previous build outputs...
if exist "%OUTPUT_DIR%" del /Q "%OUTPUT_DIR%\*" >nul 2>&1

REM =====================================================
REM DETECTAR ESPTOOL
REM =====================================================
where esptool.py >nul 2>&1
if not errorlevel 1 (
    set "ESPTOOL=esptool.py"
) else (
    where esptool >nul 2>&1
    if not errorlevel 1 (
        set "ESPTOOL=esptool"
    ) else (
        python -m esptool version >nul 2>&1
        if not errorlevel 1 (
            set "ESPTOOL=python -m esptool"
        ) else (
            echo [ERROR] esptool not found
            echo Install with: pip install --user esptool
            pause
            exit /b 1
        )
    )
)

echo [OK] Using: %ESPTOOL%
echo.

REM =====================================================
REM COMPILAR?
REM =====================================================
set /p COMPILE="Do you want to compile again? (y/n): "
if /i "%COMPILE%"=="y" (
    where arduino-cli >nul 2>&1 || (
        echo [ERROR] arduino-cli not found
        pause
        exit /b 1
    )

    echo Cleaning previous build outputs...
    if exist "%BUILD_DIR%" del /Q "%BUILD_DIR%\*" >nul 2>&1

    echo Compiling...

    arduino-cli compile ^
        --fqbn "esp32:esp32:esp32:PartitionScheme=huge_app,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none,EraseFlash=none" ^
        --export-binaries ^
        TEF6686_ESP32.ino || (
            echo [ERROR] Compilation failed
            pause
            exit /b 1
        )

    echo [OK] Compilation successful
)

REM =====================================================
REM VALIDAR BINARIOS
REM =====================================================
if not exist "%BUILD_DIR%\TEF6686_ESP32.ino.bin" (
    echo [ERROR] Firmware bin not found
    pause
    exit /b 1
)

if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

copy /y "%BUILD_DIR%\*.bin" "%OUTPUT_DIR%\" >nul 2>&1

ren "%OUTPUT_DIR%\TEF6686_ESP32.ino.bootloader.bin" bootloader.bin >nul 2>&1
ren "%OUTPUT_DIR%\TEF6686_ESP32.ino.partitions.bin" partitions.bin >nul 2>&1

REM =====================================================
REM CALCULAR ENDERECO SPIFFS
REM =====================================================
for /f %%A in ('powershell -NoProfile ^
    "$f=Get-Item '%OUTPUT_DIR%\TEF6686_ESP32.ino.bin'; ^
     '{0:X}' -f ((($f.Length+0x10000+0xFFFF)-band 0xFFFF0000))"') do (
    set "SPIFFS_ADDR=0x%%A"
)

if not defined SPIFFS_ADDR set "SPIFFS_ADDR=0x290000"

echo SPIFFS address: %SPIFFS_ADDR%
echo.

REM =====================================================
REM PORTA SERIAL
REM =====================================================
set /p SERIAL_PORT="Enter serial port (ex: COM3): "
if "%SERIAL_PORT%"=="" exit /b 1

REM =====================================================
REM DOWNLOAD MODE
REM =====================================================
:ask_ready
set /p READY="Is ESP32 in download mode? (Y/n): "
if "%READY%"=="" set READY=Y
if /i "%READY%"=="Y" goto flash
if /i "%READY%"=="N" (
    echo Hold BOOT and press RESET
    goto ask_ready
)
goto ask_ready

REM =====================================================
REM FLASH
REM =====================================================
:flash
set "FLASH_SUCCESS=0"
set "BAUDS=921600 460800 115200"

for %%B in (%BAUDS%) do (
    if !FLASH_SUCCESS! EQU 0 (
        echo Flashing at %%B baud...

        call %ESPTOOL% --chip esp32 --port %SERIAL_PORT% --baud %%B ^
            write_flash -z ^
            0x1000 "%OUTPUT_DIR%\bootloader.bin" ^
            0x8000 "%OUTPUT_DIR%\partitions.bin" ^
            0xe000 "%OUTPUT_DIR%\boot_app0.bin" ^
            0x10000 "%OUTPUT_DIR%\TEF6686_ESP32.ino.bin" ^
            %SPIFFS_ADDR% "%OUTPUT_DIR%\TEF6686_ESP32.spiffs.bin"

        if errorlevel 1 (
            echo Failed at %%B
        ) else (
            set FLASH_SUCCESS=1
        )
    )
)

if %FLASH_SUCCESS% EQU 0 (
    echo [ERROR] Flash failed
    pause
    exit /b 1
)

echo ======================================
echo FLASH COMPLETED SUCCESSFULLY
echo ======================================
pause
