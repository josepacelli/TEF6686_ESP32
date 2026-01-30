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
read -p "Do you want to compile again? (y/n): " COMPILE
if [[ "$COMPILE" == "y" || "$COMPILE" == "Y" ]]; then
    echo ""
    echo "Opening Arduino IDE..."
    echo "Please compile: Sketch > Export Compiled Binary"
    echo ""
    
    if [[ "$OSTYPE" == "darwin"* ]]; then
        open "TEF6686_ESP32.ino"
    else
        xdg-open "TEF6686_ESP32.ino" &
    fi
    
    read -p "Press Enter when compilation is done: "
fi

echo ""
echo "Step 0: Check and export binaries..."
echo ""

# Check if firmware exists
if [ ! -f "$BUILD_DIR/TEF6686_ESP32.ino.bin" ]; then
    echo "[WARNING] Firmware binary not found in $BUILD_DIR"
    echo ""
    echo "You need to export the binary from Arduino IDE:"
    echo "1. In Arduino IDE: Sketch > Export Compiled Binary"
    echo "2. Or enable 'Show verbose output during compilation'"
    echo "   in File > Preferences"
    echo ""
    echo "The binary will be generated in: $BUILD_DIR"
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

echo "Step 2: Copy binary files from build folder..."
echo ""

# Copy bootloader
if [ -f "$BUILD_DIR/TEF6686_ESP32.ino.bootloader.bin" ]; then
    cp "$BUILD_DIR/TEF6686_ESP32.ino.bootloader.bin" "$OUTPUT_DIR/bootloader.bin"
    echo "[OK] bootloader.bin copied"
else
    echo "[FAIL] bootloader.bin not found"
    exit 1
fi

# Copy partitions
if [ -f "$BUILD_DIR/TEF6686_ESP32.ino.partitions.bin" ]; then
    cp "$BUILD_DIR/TEF6686_ESP32.ino.partitions.bin" "$OUTPUT_DIR/partitions.bin"
    echo "[OK] partitions.bin copied"
else
    echo "[FAIL] partitions.bin not found"
    exit 1
fi

# Copy boot_app0
if [ -f "$BUILD_DIR/boot_app0.bin" ]; then
    cp "$BUILD_DIR/boot_app0.bin" "$OUTPUT_DIR/boot_app0.bin"
    echo "[OK] boot_app0.bin copied"
else
    echo "[FAIL] boot_app0.bin not found"
    exit 1
fi

# Copy firmware
if [ -f "$BUILD_DIR/TEF6686_ESP32.ino.bin" ]; then
    cp "$BUILD_DIR/TEF6686_ESP32.ino.bin" "$OUTPUT_DIR/TEF6686_ESP32.ino.bin"
    echo "[OK] TEF6686_ESP32.ino.bin copied"
else
    echo "[FAIL] TEF6686_ESP32.ino.bin not found"
    exit 1
fi

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
echo "esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 build/bootloader.bin 0x8000 build/partitions.bin 0xe000 build/boot_app0.bin 0x10000 build/TEF6686_ESP32.ino.bin $SPIFFS_ADDR build/TEF6686_ESP32.spiffs.bin"
echo ""
echo "Or with esptool.exe (Windows style):"
echo "esptool.exe --chip esp32 --port COM16 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 build/bootloader.bin 0x8000 build/partitions.bin 0xe000 build/boot_app0.bin 0x10000 build/TEF6686_ESP32.ino.bin $SPIFFS_ADDR build/TEF6686_ESP32.spiffs.bin"
echo ""
