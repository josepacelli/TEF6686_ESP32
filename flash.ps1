# ======================================
# TEF6686_ESP32 - Build and Flash Tool
# Windows PowerShell
# Porta padrão: COM16
# ======================================

$ErrorActionPreference = "Stop"

# =====================================================
# CONFIGURAÇÕES
# =====================================================
$BUILD_DIR  = "build\esp32.esp32.esp32"
$OUTPUT_DIR = "build"

$DEFAULT_PORT = "COM16"
$SERIAL_PORT  = $DEFAULT_PORT

# =====================================================
# FUNÇÕES DE LOG
# =====================================================
function Write-Green($msg)  { Write-Host $msg -ForegroundColor Green }
function Write-Yellow($msg) { Write-Host $msg -ForegroundColor Yellow }
function Write-Red($msg)    { Write-Host $msg -ForegroundColor Red }

# =====================================================
# DETECTAR ESPTOOL (FORMA CORRETA)
# =====================================================
function Find-Esptool {
    if (Get-Command esptool.py -ErrorAction SilentlyContinue) {
        return @{ Cmd = "esptool.py"; Args = @() }
    }
    if (Get-Command esptool -ErrorAction SilentlyContinue) {
        return @{ Cmd = "esptool"; Args = @() }
    }
    try {
        python -m esptool version | Out-Null
        return @{ Cmd = "python"; Args = @("-m", "esptool") }
    } catch {
        return $null
    }
}



# =====================================================
# HEADER
# =====================================================
Write-Host "======================================"
Write-Host "TEF6686_ESP32 - Build and Flash Tool"
Write-Host "======================================"
Write-Host ""

# =====================================================
# ESPTOOL
# =====================================================
$ESP = Find-Esptool
if (-not $ESP) {
    Write-Red "[ERROR] esptool not found"
    Write-Host "Install with:"
    Write-Host "  pip install --user esptool"
    Read-Host "Press ENTER to exit"
    exit 1
}

Write-Green "Using esptool via: $($ESP.Cmd) $($ESP.Args -join ' ')"

# =====================================================
# COMPILAR?
# =====================================================
$compile = Read-Host "Do you want to compile again? (y/n)"
if ($compile -match "^[Yy]$") {

    if (-not (Get-Command arduino-cli -ErrorAction SilentlyContinue)) {
        Write-Red "[ERROR] arduino-cli not found"
        Write-Host "Download: https://arduino.github.io/arduino-cli/"
        exit 1
    }

    Write-Host "Compiling with Arduino CLI..."

    arduino-cli compile `
        --fqbn esp32:esp32:esp32:PartitionScheme=huge_app,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none,EraseFlash=none `
        --export-binaries `
        TEF6686_ESP32.ino

    Write-Green "[OK] Compilation successful"
}

# =====================================================
# VERIFICAR FIRMWARE
# =====================================================
$FIRMWARE = "$BUILD_DIR\TEF6686_ESP32.ino.bin"
if (-not (Test-Path $FIRMWARE)) {
    Write-Yellow "[WARNING] Firmware not found:"
    Write-Host "  $FIRMWARE"
    Read-Host "Press ENTER to continue after exporting the binary"
    if (-not (Test-Path $FIRMWARE)) {
        Write-Red "[ERROR] Firmware still not found"
        exit 1
    }
}

# =====================================================
# SPIFFS
# =====================================================
$SPIFFS_BIN = "$OUTPUT_DIR\TEF6686_ESP32.spiffs.bin"

if (-not (Test-Path $SPIFFS_BIN)) {

    $mkspiffs = Get-Command mkspiffs -ErrorAction SilentlyContinue

    if (-not $mkspiffs) {
        $mkspiffsExe = Get-ChildItem `
            "$env:LOCALAPPDATA\Arduino15\packages\esp32\tools\mkspiffs" `
            -Recurse -Filter mkspiffs.exe -ErrorAction SilentlyContinue |
            Select-Object -First 1

        if ($mkspiffsExe) {
            $mkspiffs = $mkspiffsExe.FullName
        }
    }

    if ($mkspiffs) {
        Write-Host "Generating SPIFFS..."
        New-Item -ItemType Directory -Force -Path $OUTPUT_DIR | Out-Null

        # compute total size of data folder to choose adequate SPIFFS image size
        $dataSize = (Get-ChildItem -Path data -Recurse -File | Measure-Object -Property Length -Sum).Sum
        if (-not $dataSize) { $dataSize = 0 }
        Write-Host "Total bytes in data/: $dataSize"

        # preferred sizes to try (bytes)
        $preferredSizes = @(819200, 1048576, 1310720, 1572864) # 800KB, 1MB, 1.25MB, 1.5MB

        # pick starting size: first preferred >= dataSize + 4096 overhead
        $startSize = $preferredSizes | Where-Object { $_ -ge ($dataSize + 4096) } | Select-Object -First 1
        if (-not $startSize) { $startSize = $preferredSizes[-1] }

        $mkspiffsSucceeded = $false
        foreach ($size in $preferredSizes) {
            if ($size -lt ($dataSize + 1024)) { continue }
            Write-Host "Trying mkspiffs with size $size bytes..."
            & $mkspiffs -c data -b 4096 -p 256 -s $size $SPIFFS_BIN
            if ($LASTEXITCODE -eq 0 -and (Test-Path $SPIFFS_BIN)) {
                Write-Green "[OK] SPIFFS generated (size $size)"
                $mkspiffsSucceeded = $true
                break
            } else {
                Write-Yellow "[WARNING] mkspiffs failed with size $size (exit $LASTEXITCODE)"
                # remove partial output if present
                if (Test-Path $SPIFFS_BIN) { Remove-Item $SPIFFS_BIN -Force }
            }
        }

        if (-not $mkspiffsSucceeded) {
            Write-Red "[ERROR] mkspiffs failed for all tried sizes."
            Write-Host "Suggestion: check for very large files in data/ or increase preferredSizes in flash.ps1"
        }
    }
    else {
        Write-Yellow "[WARNING] mkspiffs not found, skipping SPIFFS"
    }
}
else {
    Write-Green "[OK] SPIFFS already exists"
}

# =====================================================
# COPIAR BINÁRIOS
# =====================================================
Copy-Item "$BUILD_DIR\*.bin" $OUTPUT_DIR -Force -ErrorAction SilentlyContinue

Rename-Item "$OUTPUT_DIR\TEF6686_ESP32.ino.bootloader.bin" "bootloader.bin" -ErrorAction SilentlyContinue
Rename-Item "$OUTPUT_DIR\TEF6686_ESP32.ino.partitions.bin" "partitions.bin" -ErrorAction SilentlyContinue

# boot_app0.bin
$BOOT_APP = "$OUTPUT_DIR\boot_app0.bin"
if (-not (Test-Path $BOOT_APP)) {
    $bootSrc = Get-ChildItem `
        "$env:LOCALAPPDATA\Arduino15\packages\esp32\hardware\esp32" `
        -Recurse -Filter boot_app0.bin -ErrorAction SilentlyContinue |
        Select-Object -First 1

    if ($bootSrc) {
        Copy-Item $bootSrc.FullName $BOOT_APP
        Write-Green "[OK] boot_app0.bin copied"
    }
    else {
        Write-Yellow "[WARNING] boot_app0.bin not found"
    }
}

# =====================================================
# CALCULAR ENDEREÇO SPIFFS
# =====================================================
$fwSize = (Get-Item $FIRMWARE).Length
$SPIFFS_ADDR = ("0x{0:X}" -f ((($fwSize + 0x10000 + 0xFFFF) -band 0xFFFF0000)))

Write-Host ""
Write-Host "SPIFFS address: $SPIFFS_ADDR"

# =====================================================
# PORTA SERIAL
# =====================================================
Write-Green "Using serial port: $SERIAL_PORT"
$customPort = Read-Host "Press ENTER to keep COM16 or type another COM port"
if ($customPort) {
    $SERIAL_PORT = $customPort
}

# =====================================================
# FORMATAÇÃO DO FILESYSTEM (SPIFFS)
# =====================================================
$FORMAT_BIN_SRC = "tools\format_Spiffs.ino.bin"
$FORMAT_BIN_DST = "$OUTPUT_DIR\format_Spiffs.ino.bin"

if (-not (Test-Path $FORMAT_BIN_SRC)) {
    Write-Red "[ERROR] format_Spiffs.ino.bin not found in tools\"
    exit 1
}

# Copiar formatador para build
Copy-Item $FORMAT_BIN_SRC $FORMAT_BIN_DST -Force
Write-Green "Formatting filesystem......"

& $ESP.Cmd @($ESP.Args) `
    --chip esp32 `
    --port $SERIAL_PORT `
    --baud 921600 `
    --before default_reset `
    --after hard_reset `
    write_flash -z `
    --flash_mode dio `
    --flash_freq 80m `
    --flash_size 4MB `
    0x1000  "$OUTPUT_DIR\bootloader.bin" `
    0x8000  "$OUTPUT_DIR\partitions.bin" `
    0xe000  "$OUTPUT_DIR\boot_app0.bin" `
    0x10000 "$FORMAT_BIN_DST"

if ($LASTEXITCODE -ne 0) {
    Write-Red "[ERROR] Failed to flash SPIFFS formatter"
    exit 1
}

Write-Host ""
$answer = Read-Host "Did the radio show 'Formatting finished'? (Y/n)"
if ($answer -match "^[Yy]$" -or $answer -eq "") {

    Write-Host ""
    Write-Host "Now switch your radio OFF and back ON."
    Write-Host "When you see the message 'Formatting finished' on your radio, switch OFF the radio."
    Write-Host "Next, switch your radio ON while holding the BOOT-button."
    Write-Host "Press ENTER to continue..."
    Read-Host | Out-Null
}
else {
    Write-Host "Waiting 14 seconds..."
    Start-Sleep -Seconds 14
}


# =====================================================
# PREPARAR FLASH
# =====================================================
$FLASH_FILES = @(
    "0x1000",  "$OUTPUT_DIR\bootloader.bin",
    "0x8000",  "$OUTPUT_DIR\partitions.bin",
    "0xe000",  "$OUTPUT_DIR\boot_app0.bin",
    "0x10000", "$OUTPUT_DIR\TEF6686_ESP32.ino.bin"
)

if (Test-Path $SPIFFS_BIN) {
    $FLASH_FILES += $SPIFFS_ADDR
    $FLASH_FILES += $SPIFFS_BIN
    Write-Host "Including SPIFFS at $SPIFFS_ADDR"
}
else {
    Write-Yellow "[WARNING] Flashing without SPIFFS"
}

# =====================================================
# FLASH (COM FALLBACK DE BAUDRATE)
# =====================================================
$BAUDS = @(921600,460800, 115200)
$FLASH_SUCCESS = $false

foreach ($BAUD in $BAUDS) {

    Write-Host ""
    Write-Host "Flashing at $BAUD baud..."

    & $ESP.Cmd @($ESP.Args) `
        --chip esp32 `
        --port $SERIAL_PORT `
        --baud $BAUD `
        --before default_reset `
        --after hard_reset `
        write_flash -z `
        --flash_mode dio `
        --flash_freq 80m `
        --flash_size 4MB `
        @FLASH_FILES

    if ($LASTEXITCODE -eq 0) {
        $FLASH_SUCCESS = $true
        break
    }
    else {
        Write-Yellow "[WARNING] Flash failed at $BAUD, trying next..."
    }
}

if (-not $FLASH_SUCCESS) {
    Write-Red "[ERROR] Flash failed at all baud rates"
    exit 1
}

# =====================================================
# FINAL
# =====================================================
Write-Host ""
Write-Green "======================================"
Write-Green "Flash completed successfully!"
Write-Green "======================================"
Write-Host ""
