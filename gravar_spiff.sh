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
read -rp "Do you want to compile again? (y/N): " COMPILE
# Default to No when empty
COMPILE=${COMPILE:-N}
if [[ "${COMPILE}" =~ ^[Yy]$ ]]; then

    echo -e "Limpando pasta de build..."
    echo

    rm -rvf $OUTPUT_DIR

    echo
    echo "Compiling with Arduino CLI..."
    echo
    
    # Check if arduino-cli is installed
    if ! command -v arduino-cli &>/dev/null; then
        echo -e "${RED}[ERROR] arduino-cli not found${NC}"
        echo "Please install it from: https://arduino.github.io/arduino-cli/"
        exit 1
    fi
    
    # Compile with Arduino CLI with ESP32 specific parameters
    arduino-cli compile \
        --fqbn esp32:esp32:esp32:PartitionScheme=huge_app,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none,EraseFlash=none \
        --export-binaries \
        TEF6686_ESP32.ino
    
    if [ $? -ne 0 ]; then
        echo
        echo -e "${RED}[ERROR] Compilation failed${NC}"
        exit 1
    fi
    
    echo
    echo -e "${GREEN}[OK] Compilation successful${NC}"
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

# Check if SPIFFS binary already exists or try to generate it
echo "Step 1: Check/Generate SPIFFS binary..."
echo

if [ -f "${OUTPUT_DIR}/TEF6686_ESP32.spiffs.bin" ]; then
    echo "[OK] SPIFFS binary already exists: ${OUTPUT_DIR}/TEF6686_ESP32.spiffs.bin"
else
    # Check if mkspiffs exists
    MKSPIFFS=""
    if command -v mkspiffs &>/dev/null; then
        MKSPIFFS="mkspiffs"
    elif command -v mkspiffs.py &>/dev/null; then
        MKSPIFFS="mkspiffs.py"
    else
        # Try to find mkspiffs in Arduino15 folders
        if [ -d "$HOME/.arduino15/packages/esp32/tools/mkspiffs" ]; then
            MKSPIFFS=$(find "$HOME/.arduino15/packages/esp32/tools/mkspiffs/" -name mkspiffs -type f 2>/dev/null | head -n 1)
        elif [ -d "$HOME/Library/Arduino15/packages/esp32/tools/mkspiffs" ]; then
            MKSPIFFS=$(find "$HOME/Library/Arduino15/packages/esp32/tools/mkspiffs/" -name mkspiffs -type f 2>/dev/null | head -n 1)
        fi
    fi

    if [ -n "${MKSPIFFS}" ]; then
        echo "Generating SPIFFS binary with ${MKSPIFFS}..."
        mkdir -p "${OUTPUT_DIR}"
        "${MKSPIFFS}" -c data -b 4096 -p 256 -s 819200 "${OUTPUT_DIR}/TEF6686_ESP32.spiffs.bin" >/dev/null 2>&1
        
        if [ $? -eq 0 ]; then
            echo "[OK] SPIFFS binary generated: ${OUTPUT_DIR}/TEF6686_ESP32.spiffs.bin"
        else
            echo -e "${YELLOW}[WARNING] Could not generate SPIFFS binary${NC}"
            echo "Will attempt to use pre-existing SPIFFS binary"
        fi
    else
        echo -e "${YELLOW}[WARNING] mkspiffs not found, skipping SPIFFS generation${NC}"
        echo "If you need SPIFFS, please ensure TEF6686_ESP32.spiffs.bin exists in ${OUTPUT_DIR}/"
    fi
fi
echo

echo "Step 2: Copy binary files from build folder..."
echo

# Copy all .bin files from build directory
cp "${BUILD_DIR}"/*.bin "${OUTPUT_DIR}/" 2>/dev/null

if [ $? -eq 0 ]; then
    echo "[OK] Binary files copied from ${BUILD_DIR}"
else
    echo -e "${YELLOW}[WARNING] Error copying binary files${NC}"
fi

# Rename files if they have the full names
if [ -f "${OUTPUT_DIR}/TEF6686_ESP32.ino.bootloader.bin" ]; then
    mv "${OUTPUT_DIR}/TEF6686_ESP32.ino.bootloader.bin" "${OUTPUT_DIR}/bootloader.bin" 2>/dev/null
fi

if [ -f "${OUTPUT_DIR}/TEF6686_ESP32.ino.partitions.bin" ]; then
    mv "${OUTPUT_DIR}/TEF6686_ESP32.ino.partitions.bin" "${OUTPUT_DIR}/partitions.bin" 2>/dev/null
fi

# Find and copy boot_app0.bin if not present
if [ ! -f "${OUTPUT_DIR}/boot_app0.bin" ]; then
    echo "[INFO] boot_app0.bin not found in build, searching in ESP32 package..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        BOOT_APP0_SEARCH="$HOME/.arduino15/packages/esp32/hardware/esp32/*/tools/partitions/boot_app0.bin"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        BOOT_APP0_SEARCH="$HOME/Library/Arduino15/packages/esp32/hardware/esp32/*/tools/partitions/boot_app0.bin"
    fi
    
    BOOT_APP0_PATH=$(ls ${BOOT_APP0_SEARCH} 2>/dev/null | head -n 1)
    
    if [ -f "${BOOT_APP0_PATH}" ]; then
        cp "${BOOT_APP0_PATH}" "${OUTPUT_DIR}/boot_app0.bin"
        echo "[OK] boot_app0.bin copied from ESP32 package"
    else
        echo -e "${YELLOW}[WARNING] boot_app0.bin not found, flash may fail${NC}"
    fi
fi

# Verify essential files exist
echo
echo "Binary files ready:"
ls -lh "${OUTPUT_DIR}"/*.bin 2>/dev/null | awk '{print "  - " $9 " (" $5 ")"}'

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
echo "Ready to flash to ${SERIAL_PORT}"
echo "======================================"
echo

# Ask if ESP32 is ready
while true; do
    echo
    read -rp "Is your ESP32 in download mode? (Y/n): " ready
    ready=${ready:-Y} # Default to 'Y' if no input is provided
    case ${ready} in
    [Yy]*)
        break
        ;;
    [Nn]*)
        echo
        echo "Instructions to enter download mode:"
        echo "1. For boards WITH BOOT button: Hold BOOT button and press EN/RESET"
        echo "2. For boards WITHOUT BOOT button: Connect GPIO0 to GND and reset"
        echo "3. Some boards auto-enter download mode when flashing starts"
        ;;
    *) echo "Please answer yes (y) or no (n)." ;;
    esac
done

echo
echo "======================================"
echo "Flashing to ${SERIAL_PORT}..."
echo "======================================"
echo
# Opcional: geração SPIFFS com tamanhos adaptativos (convertido de gravar.ps1)
# Pergunta ao usuário se quer tentar gerar a imagem SPIFFS testando vários tamanhos
read -rp "Deseja (re)gerar SPIFFS tentando tamanhos diferentes? (y/N): " GEN_SPIFFS_ADV
if [[ "${GEN_SPIFFS_ADV}" =~ ^[Yy]$ ]]; then
    # Detecta mkspiffs se não estiver definido
    if [ -z "${MKSPIFFS}" ]; then
        if command -v mkspiffs &>/dev/null; then
            MKSPIFFS="mkspiffs"
        elif command -v mkspiffs.py &>/dev/null; then
            MKSPIFFS="mkspiffs.py"
        else
            if [ -d "$HOME/.arduino15/packages/esp32/tools/mkspiffs" ]; then
                MKSPIFFS=$(find "$HOME/.arduino15/packages/esp32/tools/mkspiffs/" -name mkspiffs -type f 2>/dev/null | head -n 1)
            elif [ -d "$HOME/Library/Arduino15/packages/esp32/tools/mkspiffs" ]; then
                MKSPIFFS=$(find "$HOME/Library/Arduino15/packages/esp32/tools/mkspiffs/" -name mkspiffs -type f 2>/dev/null | head -n 1)
            fi
        fi
    fi

    if [ -n "${MKSPIFFS}" ]; then
        echo "Calculando tamanho total de data/..."
        dataSize=0
        if [ -d "data" ]; then
            while IFS= read -r -d '' file; do
                if stat -f%z "$file" >/dev/null 2>&1; then
                    size=$(stat -f%z "$file")
                else
                    size=$(stat -c%s "$file" 2>/dev/null || echo 0)
                fi
                dataSize=$((dataSize + size))
            done < <(find data -type f -print0 2>/dev/null)
        fi
        echo "Total bytes em data/: ${dataSize}"

        preferredSizes=(819200 1048576 1310720 1572864)

        # Escolhe o primeiro tamanho preferido que seja >= dataSize + 4096, ou o maior
        startSize=""
        for s in "${preferredSizes[@]}"; do
            if [ "$s" -ge $((dataSize + 4096)) ]; then
                startSize=$s
                break
            fi
        done
        if [ -z "${startSize}" ]; then
            startSize=${preferredSizes[-1]}
        fi

        echo "Tentando gerar SPIFFS com tamanhos preferidos..."
        mkdir -p "${OUTPUT_DIR}"
        mk_ok=false
        for size in "${preferredSizes[@]}"; do
            if [ "$size" -lt $((dataSize + 1024)) ]; then
                continue
            fi
            echo "Tentando mkspiffs com size=${size} bytes..."
            "${MKSPIFFS}" -c data -b 4096 -p 256 -s ${size} "${OUTPUT_DIR}/TEF6686_ESP32.spiffs.bin"
            if [ $? -eq 0 ] && [ -f "${OUTPUT_DIR}/TEF6686_ESP32.spiffs.bin" ]; then
                echo -e "${GREEN}[OK] SPIFFS gerada (size ${size})${NC}"
                mk_ok=true
                break
            else
                echo -e "${YELLOW}[WARNING] mkspiffs falhou com size ${size}${NC}"
                [ -f "${OUTPUT_DIR}/TEF6686_ESP32.spiffs.bin" ] && rm -f "${OUTPUT_DIR}/TEF6686_ESP32.spiffs.bin"
            fi
        done

        if ! ${mk_ok}; then
            echo -e "${RED}[ERROR] mkspiffs falhou para todos os tamanhos testados.${NC}"
            echo "Sugestão: verifique arquivos muito grandes em data/ ou aumente preferredSizes no script."
        fi
    else
        echo -e "${YELLOW}[WARNING] mkspiffs não encontrado, pulando geração adaptativa de SPIFFS${NC}"
    fi
fi

# Opcional: gravar formatador SPIFFS (convertido de flash.ps1)
read -rp "Deseja gravar o formatador de SPIFFS (tools/format_Spiffs.ino.bin)? (y/N): " WRITE_FORMATTER
if [[ "${WRITE_FORMATTER}" =~ ^[Yy]$ ]]; then
    FORMAT_BIN_SRC="tools/format_Spiffs.ino.bin"
    FORMAT_BIN_DST="${OUTPUT_DIR}/format_Spiffs.ino.bin"

    if [ ! -f "${FORMAT_BIN_SRC}" ]; then
        echo -e "${RED}[ERROR] ${FORMAT_BIN_SRC} não encontrado${NC}"
        exit 1
    fi

    # Copiar formatador para build
    mkdir -p "${OUTPUT_DIR}"
    cp -f "${FORMAT_BIN_SRC}" "${FORMAT_BIN_DST}"
    echo -e "${GREEN}Formatting filesystem...${NC}"

    # Flash formatter (usa baud alto primeiro)
    if ! ${ESPTOOL} --chip esp32 --port "${SERIAL_PORT}" --baud 460800 \
        --before default_reset --after hard_reset write_flash -z \
        --flash_mode dio --flash_freq 80m --flash_size 4MB \
        0x1000 "${OUTPUT_DIR}/bootloader.bin" \
        0x8000 "${OUTPUT_DIR}/partitions.bin" \
        0xe000 "${OUTPUT_DIR}/boot_app0.bin" \
        0x10000 "${FORMAT_BIN_DST}"; then
        echo -e "${RED}[ERROR] Falha ao gravar o formatador SPIFFS${NC}"
        exit 1
    fi

    echo
    read -rp "O rádio mostrou 'Formatting finished'? (Y/n): " answer
    answer=${answer:-Y}
    if [[ "${answer}" =~ ^[Yy]$ ]]; then
        echo
        echo "Agora desligue e ligue o rádio novamente."
        echo "Quando aparecer 'Formatting finished' no rádio, desligue-o."
        echo "Em seguida ligue o rádio segurando o botão BOOT. Pressione ENTER para continuar..."
        read -r
    else
        echo "Aguardando 14 segundos..."
        sleep 14
    fi
fi

# Flash the complete firmware
echo "Flashing firmware and SPIFFS..."

# Check if SPIFFS binary exists before flashing
FLASH_FILES=(
    0x1000 "${OUTPUT_DIR}/bootloader.bin"
    0x8000 "${OUTPUT_DIR}/partitions.bin"
    0xe000 "${OUTPUT_DIR}/boot_app0.bin"
    0x10000 "${OUTPUT_DIR}/TEF6686_ESP32.ino.bin"
)

# Add SPIFFS only if it exists
if [ -f "${OUTPUT_DIR}/TEF6686_ESP32.spiffs.bin" ]; then
    FLASH_FILES+=("${SPIFFS_ADDR}" "${OUTPUT_DIR}/TEF6686_ESP32.spiffs.bin")
    echo "Including SPIFFS at address ${SPIFFS_ADDR}"
else
    echo -e "${YELLOW}[WARNING] SPIFFS binary not found, flashing without it${NC}"
fi

# Try flashing with different baud rates
BAUD_RATES=(460800 115200)
FLASH_SUCCESS=false

for BAUD in "${BAUD_RATES[@]}"; do
    echo
    echo "Attempting flash at ${BAUD} baud..."
    
    if ${ESPTOOL} --chip esp32 --port "${SERIAL_PORT}" --baud ${BAUD} \
        --before default_reset --after hard_reset write_flash -z \
        --flash_mode dio --flash_freq 80m --flash_size 4MB \
        "${FLASH_FILES[@]}"; then
        FLASH_SUCCESS=true
        break
    else
        echo -e "${YELLOW}[WARNING] Flash failed at ${BAUD} baud, trying slower speed...${NC}"
    fi
done

if ! ${FLASH_SUCCESS}; then
    echo
    echo -e "${RED}Error flashing! Tried multiple baud rates without success.${NC}"
    echo "Please check:"
    echo "  - USB cable quality (try a different cable)"
    echo "  - Serial port connection"
    echo "  - ESP32 is properly powered"
    exit 1
fi

# Completion message
echo
echo -e "${GREEN}======================================"
echo -e "Flash completed successfully!"
echo -e "======================================${NC}"
echo
