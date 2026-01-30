#!/usr/bin/env bash

# Define possible esptool commands
ESPTOOL_CANDIDATES=(esptool esptool.py)
ESPTOOL=""

# Define directories
BUILD_DIR="build/esp32.esp32.esp32"
OUTPUT_DIR="build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Display the tool header
echo -e "======================================"
echo -e "TEF6686_ESP32 - Build and Flash Tool"
echo -e "======================================"
echo

# Check if any esptool command is available
# Iterate through possible esptool commands
for candidate in "${ESPTOOL_CANDIDATES[@]}"; do
    if command -v "${candidate}" &>/dev/null; then
        # If a candidate esptool command is found in PATH, use it
        ESPTOOL=${candidate}
        echo
        echo -e "${GREEN}Using detected esptool: $(which "${ESPTOOL}")${NC}"
        echo
        break
    fi
done

# If no esptool command was found, print an error message and exit
if [ -z "${ESPTOOL}" ]; then
    echo -e "${RED}Error: None of the possible esptool commands ${ESPTOOL_CANDIDATES[*]} are installed or not in your PATH.${NC}"
    echo
    echo "Please install esptool using the following command:"
    echo "  pip install --user esptool"
    echo
    echo "You can also install esptool package using your distribution's package manager."
    exit 1
fi

# Check if we need to compile first
echo
read -rp "Do you want to compile again? (y/n): " COMPILE
if [[ "${COMPILE}" =~ ^[Yy]$ ]]; then
    echo
    echo "Opening Arduino IDE..."
    echo "Please compile: Sketch > Export Compiled Binary"
    echo
    if command -v arduino &>/dev/null; then
        arduino TEF6686_ESP32.ino &
    elif command -v open &>/dev/null; then
        open TEF6686_ESP32.ino
    fi
    echo
    read -rp "Press Enter when compilation is done: "
fi

echo
echo "Step 0: Check and export binaries..."
echo

# Check if firmware binary exists
if [ ! -f "${BUILD_DIR}/TEF6686_ESP32.ino.bin" ]; then
    echo -e "${YELLOW}[WARNING] Firmware binary not found in ${BUILD_DIR}${NC}"
    echo
    echo "You need to export the binary from Arduino IDE:"
    echo "1. In Arduino IDE: Sketch > Export Compiled Binary"
    echo "2. Or enable 'Show verbose output during compilation'"
    echo "   in File > Preferences"
    echo
    echo "The binary will be generated in: ${BUILD_DIR}"
    echo
    read -rp "Press Enter to continue when ready, or type 'abort' to cancel: " CONTINUE
    if [[ "${CONTINUE}" =~ ^[Aa][Bb][Oo][Rr][Tt]$ ]]; then
        echo "Aborted."
        exit 1
    fi
    if [ ! -f "${BUILD_DIR}/TEF6686_ESP32.ino.bin" ]; then
        echo
        echo -e "${RED}[ERROR] Firmware binary still not found!${NC}"
        exit 1
    fi
fi

echo "[OK] All required binaries found"
echo

# Check if mkspiffs exists
MKSPIFFS=""
if command -v mkspiffs &>/dev/null; then
    MKSPIFFS="mkspiffs"
elif command -v mkspiffs.py &>/dev/null; then
    MKSPIFFS="mkspiffs.py"
elif [ -f "$HOME/.arduino15/packages/esp32/tools/mkspiffs/*/mkspiffs" ]; then
    MKSPIFFS=$(find "$HOME/.arduino15/packages/esp32/tools/mkspiffs/" -name mkspiffs -type f 2>/dev/null | head -n 1)
elif [ -f "$HOME/Library/Arduino15/packages/esp32/tools/mkspiffs/*/mkspiffs" ]; then
    MKSPIFFS=$(find "$HOME/Library/Arduino15/packages/esp32/tools/mkspiffs/" -name mkspiffs -type f 2>/dev/null | head -n 1)
fi

if [ -z "${MKSPIFFS}" ]; then
    echo -e "${RED}[ERROR] mkspiffs not found${NC}"
    echo "Please install ESP32 board package in Arduino IDE"
    exit 1
fi

echo "Step 1: Generate SPIFFS binary..."
echo

# Generate SPIFFS with correct size (800KB)
mkdir -p "${OUTPUT_DIR}"
"${MKSPIFFS}" -c data -b 4096 -p 256 -s 819200 "${OUTPUT_DIR}/TEF6686_ESP32.spiffs.bin" >/dev/null 2>&1

if [ $? -ne 0 ]; then
    echo -e "${RED}[FAIL] Error generating SPIFFS binary${NC}"
    exit 1
fi
echo "[OK] SPIFFS binary generated: ${OUTPUT_DIR}/TEF6686_ESP32.spiffs.bin"
echo

echo "Step 2: Copy binary files from build folder..."
echo

# Copy bootloader
if [ -f "${BUILD_DIR}/TEF6686_ESP32.ino.bootloader.bin" ]; then
    cp "${BUILD_DIR}/TEF6686_ESP32.ino.bootloader.bin" "${OUTPUT_DIR}/bootloader.bin"
    echo "[OK] bootloader.bin copied"
else
    echo -e "${RED}[FAIL] bootloader.bin not found${NC}"
    exit 1
fi

# Copy partitions
if [ -f "${BUILD_DIR}/TEF6686_ESP32.ino.partitions.bin" ]; then
    cp "${BUILD_DIR}/TEF6686_ESP32.ino.partitions.bin" "${OUTPUT_DIR}/partitions.bin"
    echo "[OK] partitions.bin copied"
else
    echo -e "${RED}[FAIL] partitions.bin not found${NC}"
    exit 1
fi

# Copy boot_app0
if [ -f "${BUILD_DIR}/boot_app0.bin" ]; then
    cp "${BUILD_DIR}/boot_app0.bin" "${OUTPUT_DIR}/boot_app0.bin"
    echo "[OK] boot_app0.bin copied"
else
    echo -e "${RED}[FAIL] boot_app0.bin not found${NC}"
    exit 1
fi

# Copy firmware
if [ -f "${BUILD_DIR}/TEF6686_ESP32.ino.bin" ]; then
    cp "${BUILD_DIR}/TEF6686_ESP32.ino.bin" "${OUTPUT_DIR}/TEF6686_ESP32.ino.bin"
    echo "[OK] TEF6686_ESP32.ino.bin copied"
else
    echo -e "${RED}[FAIL] TEF6686_ESP32.ino.bin not found${NC}"
    exit 1
fi

echo
echo "======================================"
echo "All binaries ready for flashing!"
echo "======================================"
echo

# Calculate firmware size and correct SPIFFS address
echo "Step 3: Calculate SPIFFS address..."
echo

# Get firmware size and calculate aligned SPIFFS address (aligned to 64KB)
FIRMWARE_SIZE=$(stat -f%z "${OUTPUT_DIR}/TEF6686_ESP32.ino.bin" 2>/dev/null || stat -c%s "${OUTPUT_DIR}/TEF6686_ESP32.ino.bin" 2>/dev/null)
SPIFFS_ADDR=$(printf "0x%X" $((($FIRMWARE_SIZE + 0x10000 + 0xFFFF) & 0xFFFF0000)))

echo "Firmware: ${OUTPUT_DIR}/TEF6686_ESP32.ino.bin"
echo "SPIFFS binary: ${OUTPUT_DIR}/TEF6686_ESP32.spiffs.bin"
echo "SPIFFS address: ${SPIFFS_ADDR} (auto-calculated, aligned to 64KB)"
echo

# Detect serial port
SERIAL_PORT=""
if [ "$(uname)" == "Darwin" ]; then
    # macOS
    mapfile -t USB_DEVICES < <(ls /dev/cu.usbserial-* /dev/cu.SLAB_USBtoUART /dev/cu.wchusbserial* 2>/dev/null)
else
    # Linux - Check for proper group membership
    if [ $(getent group dialout) ]; then
        if ! groups "${USER}" | grep -q '\bdialout\b'; then
            echo -e "${RED}Error: User '${USER}' is not a member of the 'dialout' group.${NC}"
            echo "Please add the user to the 'dialout' group to access the serial port."
            echo "You can add the user to the group with the following command (may require sudo):"
            echo "  sudo usermod -aG dialout ${USER}"
            echo "After adding, you need to reboot for the changes to take effect."
            exit 1
        fi
    elif [ $(getent group uucp) ]; then
        if ! groups "${USER}" | grep -q '\buucp\b'; then
            echo -e "${RED}Error: User '${USER}' is not a member of the 'uucp' group.${NC}"
            echo "Please add the user to the 'uucp' group to access the serial port."
            echo "You can add the user to the group with the following command (may require sudo):"
            echo "  sudo usermod -aG uucp ${USER}"
            echo "After adding, you need to reboot for the changes to take effect."
            exit 1
        fi
    fi
    
    # Detect available /dev/ttyUSB* devices
    mapfile -t USB_DEVICES < <(ls /dev/ttyUSB* 2>/dev/null)
fi

# Check if any USB devices are found
if [ ${#USB_DEVICES[@]} -eq 0 ]; then
    echo -e "${RED}Error: No USB serial devices found.${NC}"
    echo "Please connect your device and try again."
    read -rp "Enter serial port manually (or press Enter to cancel): " SERIAL_PORT
    if [ -z "${SERIAL_PORT}" ]; then
        exit 1
    fi
elif [ ${#USB_DEVICES[@]} -eq 1 ]; then
    # If only one USB device is found, use it
    SERIAL_PORT="${USB_DEVICES[0]}"
    echo -e "${GREEN}Using detected serial port: ${SERIAL_PORT}${NC}"
else
    # If multiple USB devices are found, prompt the user to select one
    echo "Multiple USB serial devices found:"
    for i in "${!USB_DEVICES[@]}"; do
        echo "  [$i]: ${USB_DEVICES[$i]}"
    done
    echo
    read -rp "Please select the serial port [0-$((${#USB_DEVICES[@]} - 1))]: " selection
    if [[ ${selection} =~ ^[0-9]+$ ]] && [ "${selection}" -ge 0 ] && [ "${selection}" -lt ${#USB_DEVICES[@]} ]; then
        SERIAL_PORT="${USB_DEVICES[${selection}]}"
    else
        echo -e "${RED}Invalid selection. Exiting.${NC}"
        exit 1
    fi
fi

echo
echo "======================================"
echo "Flashing to ${SERIAL_PORT}..."
echo "======================================"
echo

# Flash the complete firmware
echo "Flashing firmware and SPIFFS..."
if ! ${ESPTOOL} --chip esp32 --port "${SERIAL_PORT}" --baud 921600 --before default_reset \
    --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB \
    0x1000 "${OUTPUT_DIR}/bootloader.bin" \
    0x8000 "${OUTPUT_DIR}/partitions.bin" \
    0xe000 "${OUTPUT_DIR}/boot_app0.bin" \
    0x10000 "${OUTPUT_DIR}/TEF6686_ESP32.ino.bin" \
    ${SPIFFS_ADDR} "${OUTPUT_DIR}/TEF6686_ESP32.spiffs.bin"; then
    echo
    echo -e "${RED}Error flashing! Please check the serial port and ESP32 connection.${NC}"
    exit 1
fi

# Completion message
echo
echo -e "${GREEN}======================================"
echo -e "Flash completed successfully!"
echo -e "======================================${NC}"
echo
