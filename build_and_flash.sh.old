#!/bin/bash

# TEF6686_ESP32 - Build and Flash Tool (Linux/Mac)

clear
echo "======================================"
echo "TEF6686_ESP32 - Build and Flash Tool"
echo "======================================"
echo ""

# Define paths
BUILD_DIR="build/esp32.esp32.esp32"
OUTPUT_DIR="build"

# Detect mkspiffs path based on OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux paths
    MKSPIFFS_POSSIBLE_PATHS=(
        "$HOME/.arduino15/packages/esp32/tools/mkspiffs/0.2.3/mkspiffs"
        "$HOME/.local/share/Arduino/packages/esp32/tools/mkspiffs/0.2.3/mkspiffs"
        "/opt/esp32/tools/mkspiffs/0.2.3/mkspiffs"
    )
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS paths
    MKSPIFFS_POSSIBLE_PATHS=(
        "$HOME/Library/Arduino15/packages/esp32/tools/mkspiffs/0.2.3/mkspiffs"
        "$HOME/Library/Arduino/packages/esp32/tools/mkspiffs/0.2.3/mkspiffs"
        "/Applications/Arduino.app/Contents/Java/tools/mkspiffs"
    )
else
    echo "[ERROR] Unsupported OS"
    exit 1
fi

# Find mkspiffs
MKSPIFFS_PATH=""
for path in "${MKSPIFFS_POSSIBLE_PATHS[@]}"; do
    if [ -f "$path" ]; then
        MKSPIFFS_PATH="$path"
        break
    fi
done

# Ask to compile
echo ""
read -p "Do you want to compile? (y/n): " COMPILE
if [[ "$COMPILE" == "y" || "$COMPILE" == "Y" ]]; then
    echo ""
    echo "Compiling with Arduino CLI..."
    echo ""
    
    # Check if arduino-cli is installed
    if ! command -v arduino-cli &> /dev/null; then
        echo "[ERROR] arduino-cli not found"
        echo "Please install it from: https://arduino.github.io/arduino-cli/"
        exit 1
    fi
    
    # Compile with Arduino CLI with ESP32 specific parameters
    arduino-cli compile \
        --fqbn esp32:esp32:esp32:PartitionScheme=huge_app,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none,EraseFlash=none \
        --export-binaries \
        TEF6686_ESP32.ino
    
    if [ $? -ne 0 ]; then
        echo ""
        echo "[ERROR] Compilation failed"
        exit 1
    fi
    
    echo ""
    echo "[OK] Compilation successful"
fi

echo ""
echo "Step 0: Check binaries..."
echo ""

# Check if firmware exists
if [ ! -f "$BUILD_DIR/TEF6686_ESP32.ino.bin" ]; then
    echo "[WARNING] Firmware binary not found in $BUILD_DIR"
    echo ""
    echo "You need to compile the project first using Arduino CLI:"
    echo "  arduino-cli compile --fqbn esp32:esp32:esp32 --export-binaries TEF6686_ESP32.ino"
    echo ""
    echo "Or run this script again and choose 'y' when asked to compile."
    echo ""
    read -p "Press Enter when ready, or type 'abort' to cancel: " CONTINUE
    if [[ "$CONTINUE" == "abort" ]]; then
        echo "Aborted."
        exit 1
    fi
    if [ ! -f "$BUILD_DIR/TEF6686_ESP32.ino.bin" ]; then
        echo ""
        echo "[ERROR] Firmware binary still not found!"
        exit 1
    fi
fi

echo "[OK] All required binaries found"
echo ""

# Check if mkspiffs exists
if [ -z "$MKSPIFFS_PATH" ]; then
    echo "[ERROR] mkspiffs not found"
    echo ""
    echo "Searched in:"
    for path in "${MKSPIFFS_POSSIBLE_PATHS[@]}"; do
        echo "  - $path"
    done
    echo ""
    echo "Please install ESP32 boards in Arduino IDE"
    exit 1
fi

echo "Step 1: Generate SPIFFS binary..."
echo ""

# Generate SPIFFS with correct size (800KB)
"$MKSPIFFS_PATH" -c data -b 4096 -p 256 -s 819200 "$OUTPUT_DIR/TEF6686_ESP32.spiffs.bin" 2>/dev/null

if [ $? -ne 0 ]; then
    echo "[FAIL] Error generating SPIFFS binary"
    exit 1
fi

echo "[OK] SPIFFS binary generated: $OUTPUT_DIR/TEF6686_ESP32.spiffs.bin"
echo ""

echo "Step 2: Copy all binary files from build folder..."
echo ""

# Copy all .bin files from build directory
cp "$BUILD_DIR"/*.bin "$OUTPUT_DIR/" 2>/dev/null

if [ $? -eq 0 ]; then
    echo "[OK] Binary files copied from $BUILD_DIR"
else
    echo "[FAIL] Error copying binary files"
    exit 1
fi

# Find and copy boot_app0.bin if not present
if [ ! -f "$OUTPUT_DIR/boot_app0.bin" ]; then
    echo "[INFO] boot_app0.bin not found in build, searching in ESP32 package..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        BOOT_APP0_SEARCH="$HOME/.arduino15/packages/esp32/hardware/esp32/*/tools/partitions/boot_app0.bin"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        BOOT_APP0_SEARCH="$HOME/Library/Arduino15/packages/esp32/hardware/esp32/*/tools/partitions/boot_app0.bin"
    fi
    
    BOOT_APP0_PATH=$(ls $BOOT_APP0_SEARCH 2>/dev/null | head -n 1)
    
    if [ -f "$BOOT_APP0_PATH" ]; then
        cp "$BOOT_APP0_PATH" "$OUTPUT_DIR/boot_app0.bin"
        echo "[OK] boot_app0.bin copied from ESP32 package"
    else
        echo "[WARNING] boot_app0.bin not found, flash may fail"
    fi
fi

echo ""
echo "Binary files ready:"
ls -lh "$OUTPUT_DIR"/*.bin | awk '{print "  - " $9 " (" $5 ")"}'

echo ""
echo "======================================"
echo "All binaries ready for flashing!"
echo "======================================"
echo ""

# Calculate SPIFFS address based on firmware size
echo "Step 3: Calculate SPIFFS address..."
echo ""

FW_SIZE=$(stat -f%z "$OUTPUT_DIR/TEF6686_ESP32.ino.bin" 2>/dev/null || stat -c%s "$OUTPUT_DIR/TEF6686_ESP32.ino.bin")
SPIFFS_ADDR_INT=$(( (FW_SIZE + 0x10000 + 0xFFFF) & 0xFFFF0000 ))
SPIFFS_ADDR=$(printf "0x%X" $SPIFFS_ADDR_INT)

echo "Firmware: $OUTPUT_DIR/TEF6686_ESP32.ino.bin"
echo "SPIFFS binary: $OUTPUT_DIR/TEF6686_ESP32.spiffs.bin"
echo "SPIFFS address: $SPIFFS_ADDR (auto-calculated, aligned to 64KB)"
echo ""

echo "======================================"
echo "Flash command (change /dev/ttyUSB0 if needed):"
echo "======================================"
echo ""

# Detect available ports
echo "Detecting available serial ports..."
if [[ "$OSTYPE" == "darwin"* ]]; then
    PORTS=$(ls /dev/cu.* 2>/dev/null | grep -E "(usbserial|SLAB|wchusbserial)" || echo "")
else
    PORTS=$(ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null || echo "")
fi

if [ -z "$PORTS" ]; then
    echo "[WARNING] No serial ports detected"
    SERIAL_PORT="/dev/ttyUSB0"
else
    echo "Available ports:"
    echo "$PORTS"
    SERIAL_PORT=$(echo "$PORTS" | head -n 1)
fi

echo ""
echo "Default port: $SERIAL_PORT"
echo ""

# Ask to flash
read -p "Do you want to flash the firmware now? (y/n): " FLASH
if [[ "$FLASH" == "y" || "$FLASH" == "Y" ]]; then
    echo ""
    read -p "Enter serial port [$SERIAL_PORT]: " CUSTOM_PORT
    if [ -n "$CUSTOM_PORT" ]; then
        SERIAL_PORT="$CUSTOM_PORT"
    fi
    
    echo ""
    echo "Step 4: Flashing firmware and SPIFFS..."
    echo ""
    
    # Check if esptool is available
    if ! command -v esptool.py &> /dev/null; then
        echo "[ERROR] esptool.py not found"
        echo "Install with: pip install esptool"
        exit 1
    fi
    
    # Flash command
    esptool.py --chip esp32 \
        --port "$SERIAL_PORT" \
        --baud 921600 \
        --before default_reset \
        --after hard_reset \
        write_flash -z \
        --flash_mode dio \
        --flash_freq 80m \
        --flash_size 4MB \
        0x1000 "$OUTPUT_DIR/bootloader.bin" \
        0x8000 "$OUTPUT_DIR/partitions.bin" \
        0xe000 "$OUTPUT_DIR/boot_app0.bin" \
        0x10000 "$OUTPUT_DIR/TEF6686_ESP32.ino.bin" \
        $SPIFFS_ADDR "$OUTPUT_DIR/TEF6686_ESP32.spiffs.bin"
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "[OK] Flash completed successfully!"
    else
        echo ""
        echo "[FAIL] Flash failed"
        exit 1
    fi
else
    echo ""
    echo "Manual flash command:"
    echo ""
    echo "esptool.py --chip esp32 --port $SERIAL_PORT --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 build/bootloader.bin 0x8000 build/partitions.bin 0xe000 build/boot_app0.bin 0x10000 build/TEF6686_ESP32.ino.bin $SPIFFS_ADDR build/TEF6686_ESP32.spiffs.bin"
fi

echo ""
