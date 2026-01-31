@echo off
setlocal enabledelayedexpansion
cls

REM Define directories
set "BUILD_DIR=build\esp32.esp32.esp32"
set "OUTPUT_DIR=build"

REM Colors (using title for visual feedback on Windows)
echo.
echo ======================================
echo TEF6686_ESP32 - Build and Flash Tool
echo ======================================
echo.

REM Check if esptool is available
where esptool.py >nul 2>&1
if !errorlevel! equ 0 (
    set "ESPTOOL=esptool.py"
    for /f "tokens=*" %%i in ('where esptool.py') do set "ESPTOOL_PATH=%%i"
    echo [OK] Using detected esptool: !ESPTOOL_PATH!
    echo.
) else (
    where esptool >nul 2>&1
    if !errorlevel! equ 0 (
        set "ESPTOOL=esptool"
        for /f "tokens=*" %%i in ('where esptool') do set "ESPTOOL_PATH=%%i"
        echo [OK] Using detected esptool: !ESPTOOL_PATH!
        echo.
    ) else (
        REM Try using python -m esptool as fallback
        python -m esptool version >nul 2>&1
        if !errorlevel! equ 0 (
            set "ESPTOOL=python -m esptool"
            echo [OK] Using python -m esptool
            echo.
        ) else (
            echo [ERROR] esptool not found
            echo.
            echo Please install esptool using:
            echo   pip install --user esptool
            echo.
            pause
            exit /b 1
        )
    )
)

REM Check if we need to compile first
echo.
set /p COMPILE="Do you want to compile again? (y/n): "
if /i "!COMPILE!"=="y" (
    echo.
    echo Compiling with Arduino CLI...
    echo.
    
    REM Check if arduino-cli is installed
    where arduino-cli >nul 2>&1
    if !errorlevel! neq 0 (
        echo [ERROR] arduino-cli not found
        echo Please install it from: https://arduino.github.io/arduino-cli/
        echo.
        pause
        exit /b 1
    )
    
    REM Compile with Arduino CLI
    arduino-cli compile ^
        --fqbn esp32:esp32:esp32:PartitionScheme=huge_app,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none,EraseFlash=none ^
        --export-binaries ^
        TEF6686_ESP32.ino
    
    if !errorlevel! neq 0 (
        echo.
        echo [ERROR] Compilation failed
        echo.
        pause
        exit /b 1
    )
    
    echo.
    echo [OK] Compilation successful
)

echo.
echo Step 0: Check and export binaries...
echo.

REM Check if firmware binary exists
if not exist "!BUILD_DIR!\TEF6686_ESP32.ino.bin" (
    echo [WARNING] Firmware binary not found in !BUILD_DIR!
    echo.
    echo You need to export the binary from Arduino IDE:
    echo 1. In Arduino IDE: Sketch ^> Export Compiled Binary
    echo 2. Or enable "Show verbose output during compilation"
    echo    in File ^> Preferences
    echo.
    echo The binary will be generated in: !BUILD_DIR!
    echo.
    set /p CONTINUE="Press Enter to continue when ready, or type 'abort' to cancel: "
    if /i "!CONTINUE!"=="abort" (
        echo Aborted.
        echo.
        pause
        exit /b 1
    )
    if not exist "!BUILD_DIR!\TEF6686_ESP32.ino.bin" (
        echo.
        echo [ERROR] Firmware binary still not found!
        echo.
        pause
        exit /b 1
    )
)

echo [OK] All required binaries found
echo.

REM Check if SPIFFS binary already exists or try to generate it
echo Step 1: Check/Generate SPIFFS binary...
echo.

if exist "!OUTPUT_DIR!\TEF6686_ESP32.spiffs.bin" (
    echo [OK] SPIFFS binary already exists: !OUTPUT_DIR!\TEF6686_ESP32.spiffs.bin
) else (
    REM Check if mkspiffs exists
    where mkspiffs >nul 2>&1
    if !errorlevel! equ 0 (
        set "MKSPIFFS=mkspiffs"
    ) else (
        where mkspiffs.py >nul 2>&1
        if !errorlevel! equ 0 (
            set "MKSPIFFS=mkspiffs.py"
        ) else (
            REM Try to find mkspiffs in Arduino15
            if exist "%USERPROFILE%\AppData\Local\Arduino15\packages\esp32\tools\mkspiffs" (
                for /f "tokens=*" %%i in ('dir /b /s "%USERPROFILE%\AppData\Local\Arduino15\packages\esp32\tools\mkspiffs\*mkspiffs.exe" 2^>nul ^| findstr /r /c:"mkspiffs\.exe$"') do set "MKSPIFFS=%%i"
            )
        )
    )

    if defined MKSPIFFS (
        echo Generating SPIFFS binary with !MKSPIFFS!...
        if not exist "!OUTPUT_DIR!" mkdir "!OUTPUT_DIR!"
        "!MKSPIFFS!" -c data -b 4096 -p 256 -s 819200 "!OUTPUT_DIR!\TEF6686_ESP32.spiffs.bin" >nul 2>&1
        
        if !errorlevel! equ 0 (
            echo [OK] SPIFFS binary generated: !OUTPUT_DIR!\TEF6686_ESP32.spiffs.bin
        ) else (
            echo [WARNING] Could not generate SPIFFS binary
            echo Will attempt to use pre-existing SPIFFS binary
        )
    ) else (
        echo [WARNING] mkspiffs not found, skipping SPIFFS generation
        echo If you need SPIFFS, please ensure TEF6686_ESP32.spiffs.bin exists in !OUTPUT_DIR!\
    )
)
echo.

echo Step 2: Copy binary files from build folder...
echo.

REM Copy all .bin files from build directory
if exist "!BUILD_DIR!\*.bin" (
    copy /Y "!BUILD_DIR!\*.bin" "!OUTPUT_DIR!\" >nul 2>&1
    if !errorlevel! equ 0 (
        echo [OK] Binary files copied from !BUILD_DIR!
    ) else (
        echo [WARNING] Error copying binary files
    )
)

REM Rename files if they have the full names
if exist "!OUTPUT_DIR!\TEF6686_ESP32.ino.bootloader.bin" (
    ren "!OUTPUT_DIR!\TEF6686_ESP32.ino.bootloader.bin" "bootloader.bin" >nul 2>&1
)

if exist "!OUTPUT_DIR!\TEF6686_ESP32.ino.partitions.bin" (
    ren "!OUTPUT_DIR!\TEF6686_ESP32.ino.partitions.bin" "partitions.bin" >nul 2>&1
)

REM Find and copy boot_app0.bin if not present
if not exist "!OUTPUT_DIR!\boot_app0.bin" (
    echo [INFO] boot_app0.bin not found in build, searching in ESP32 package...
    
    for /f "tokens=*" %%i in ('dir /b /s "%USERPROFILE%\AppData\Local\Arduino15\packages\esp32\hardware\esp32\*\tools\partitions\boot_app0.bin" 2^>nul') do (
        copy /Y "%%i" "!OUTPUT_DIR!\boot_app0.bin" >nul 2>&1
        if !errorlevel! equ 0 (
            echo [OK] boot_app0.bin copied from ESP32 package
            goto :boot_app0_done
        )
    )
    
    echo [WARNING] boot_app0.bin not found, flash may fail
    :boot_app0_done
)

REM Verify essential files exist
echo.
echo Binary files ready:
if exist "!OUTPUT_DIR!\*.bin" (
    dir "!OUTPUT_DIR!\*.bin"
)

echo.
echo ======================================
echo All binaries ready for flashing!
echo ======================================
echo.

REM Calculate firmware size and correct SPIFFS address
echo Step 3: Calculate SPIFFS address...
echo.

REM Get firmware size and calculate aligned SPIFFS address (aligned to 64KB)
for /f %%A in ('powershell -NoProfile -Command "$fw = ([System.IO.FileInfo]'!OUTPUT_DIR!\TEF6686_ESP32.ino.bin').Length; [string]::Format('0x{0:X}', (($fw + 0x10000 + 0xFFFF) -band 0xFFFF0000))"') do set "SPIFFS_ADDR=%%A"

if not defined SPIFFS_ADDR (
    set "SPIFFS_ADDR=0x290000"
    echo [WARNING] Could not calculate SPIFFS address, using default: !SPIFFS_ADDR!
)

echo Firmware: !OUTPUT_DIR!\TEF6686_ESP32.ino.bin
echo SPIFFS binary: !OUTPUT_DIR!\TEF6686_ESP32.spiffs.bin
echo SPIFFS address: !SPIFFS_ADDR! (auto-calculated, aligned to 64KB)
echo.

REM Detect serial port
echo Step 4: Detect serial port...
echo.

setlocal enabledelayedexpansion
set "PORT_COUNT=0"

REM Check available COM ports
for /l %%N in (1,1,256) do (
    mode COM%%N >nul 2>&1
    if !errorlevel! equ 0 (
        set /a PORT_COUNT+=1
        set "PORT_!PORT_COUNT!=COM%%N"
        echo   [!PORT_COUNT!]: COM%%N
    )
)

if !PORT_COUNT! equ 0 (
    echo [ERROR] No USB serial devices found.
    echo.
    echo Please connect your device and try again.
    set /p SERIAL_PORT="Enter serial port manually (or press Enter to cancel): "
    if "!SERIAL_PORT!"=="" (
        echo.
        pause
        exit /b 1
    )
) else if !PORT_COUNT! equ 1 (
    set "SERIAL_PORT=!PORT_1!"
    echo.
    echo [OK] Using detected serial port: !SERIAL_PORT!
) else (
    echo.
    set /p PORT_SELECTION="Please select the serial port [1-!PORT_COUNT!]: "
    
    if defined PORT_!PORT_SELECTION! (
        set "SERIAL_PORT=!PORT_!PORT_SELECTION!!!"
    ) else (
        echo [ERROR] Invalid selection.
        echo.
        pause
        exit /b 1
    )
)

echo.
echo ======================================
echo Ready to flash to !SERIAL_PORT!
echo ======================================
echo.

REM Ask if ESP32 is ready
setlocal enabledelayedexpansion
set "READY=N"
:ask_ready
if "!READY!"=="N" (
    echo.
    set /p READY="Is your ESP32 in download mode? (Y/n): "
    if "!READY!"=="" set "READY=Y"
    
    if /i "!READY!"=="y" (
        goto :continue_flash
    ) else if /i "!READY!"=="n" (
        echo.
        echo Instructions to enter download mode:
        echo 1. For boards WITH BOOT button: Hold BOOT button and press EN/RESET
        echo 2. For boards WITHOUT BOOT button: Connect GPIO0 to GND and reset
        echo 3. Some boards auto-enter download mode when flashing starts
        echo.
        set "READY=N"
        goto :ask_ready
    ) else (
        echo Please answer yes (y) or no (n).
        set "READY=N"
        goto :ask_ready
    )
)

:continue_flash
echo.
echo ======================================
echo Flashing to !SERIAL_PORT!...
echo ======================================
echo.

REM Flash the complete firmware with multiple baud rate attempts
setlocal enabledelayedexpansion
set "FLASH_SUCCESS=0"
set "BAUD_RATES=460800 115200"

for %%B in (!BAUD_RATES!) do (
    if !FLASH_SUCCESS! equ 0 (
        echo.
        echo Attempting flash at %%B baud...
        
        if exist "!OUTPUT_DIR!\TEF6686_ESP32.spiffs.bin" (
            echo Including SPIFFS at address !SPIFFS_ADDR!
            !ESPTOOL! --chip esp32 --port !SERIAL_PORT! --baud %%B ^
                --before default_reset --after hard_reset write_flash -z ^
                --flash_mode dio --flash_freq 80m --flash_size 4MB ^
                0x1000 "!OUTPUT_DIR!\bootloader.bin" ^
                0x8000 "!OUTPUT_DIR!\partitions.bin" ^
                0xe000 "!OUTPUT_DIR!\boot_app0.bin" ^
                0x10000 "!OUTPUT_DIR!\TEF6686_ESP32.ino.bin" ^
                !SPIFFS_ADDR! "!OUTPUT_DIR!\TEF6686_ESP32.spiffs.bin"
        ) else (
            !ESPTOOL! --chip esp32 --port !SERIAL_PORT! --baud %%B ^
                --before default_reset --after hard_reset write_flash -z ^
                --flash_mode dio --flash_freq 80m --flash_size 4MB ^
                0x1000 "!OUTPUT_DIR!\bootloader.bin" ^
                0x8000 "!OUTPUT_DIR!\partitions.bin" ^
                0xe000 "!OUTPUT_DIR!\boot_app0.bin" ^
                0x10000 "!OUTPUT_DIR!\TEF6686_ESP32.ino.bin"
        )
        
        if !errorlevel! equ 0 (
            set "FLASH_SUCCESS=1"
        ) else (
            echo [WARNING] Flash failed at %%B baud, trying slower speed...
        )
    )
)

if !FLASH_SUCCESS! equ 0 (
    echo.
    echo [ERROR] Flash failed! Tried multiple baud rates without success.
    echo.
    echo Please check:
    echo   - USB cable quality (try a different cable)
    echo   - Serial port connection
    echo   - ESP32 is properly powered
    echo.
    pause
    exit /b 1
)

REM Completion message
echo.
echo ======================================
echo Flash completed successfully!
echo ======================================
echo.
pause
