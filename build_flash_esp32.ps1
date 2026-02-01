Clear-Host

# =====================================================
# CONFIGURACOES
# =====================================================
$BUILD_DIR  = "build\esp32.esp32.esp32"
$OUTPUT_DIR = "flash"
$BAUDS      = @(921600, 460800, 115200)

Write-Host "======================================"
Write-Host " TEF6686_ESP32 - Build and Flash Tool "
Write-Host "======================================`n"

# =====================================================
# LIMPEZA
# =====================================================
Write-Host "Cleaning previous build outputs..."
if (Test-Path $OUTPUT_DIR) {
    Get-ChildItem $OUTPUT_DIR -Recurse -Force |
        Remove-Item -Force -Recurse -ErrorAction SilentlyContinue
}

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

$ESP = Find-Esptool
if (-not $ESP) {
    Write-Host "[ERROR] esptool not found"
    Write-Host "Install with: pip install --user esptool"
    Read-Host "Press ENTER to exit"
    exit 1
}

Write-Host "[OK] Using: $($ESP.Cmd) $($ESP.Args -join ' ')`n"

# =====================================================
# COMPILAR?
# =====================================================
$compile = Read-Host "Do you want to compile again? (y/n)"
if ($compile -match '^[Yy]$') {

    if (-not (Get-Command arduino-cli -ErrorAction SilentlyContinue)) {
        Write-Host "[ERROR] arduino-cli not found"
        Read-Host "Press ENTER to exit"
        exit 1
    }

    Write-Host "Cleaning build directory..."
    if (Test-Path $BUILD_DIR) {
        Get-ChildItem $BUILD_DIR -Recurse -Force |
            Remove-Item -Force -Recurse -ErrorAction SilentlyContinue
    }

    Write-Host "Compiling firmware...`n"

    arduino-cli compile `
        --fqbn "esp32:esp32:esp32:PartitionScheme=huge_app,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none,EraseFlash=none" `
        --export-binaries `
        TEF6686_ESP32.ino

    if ($LASTEXITCODE -ne 0) {
        Write-Host "`n[ERROR] Compilation failed"
        Read-Host "Press ENTER to exit"
        exit 1
    }

    Write-Host "`n[OK] Compilation successful"
}

# =====================================================
# VALIDAR BINARIOS
# =====================================================
$firmware = Join-Path $BUILD_DIR "TEF6686_ESP32.ino.bin"
if (-not (Test-Path $firmware)) {
    Write-Host "[ERROR] Firmware binary not found"
    Read-Host "Press ENTER to exit"
    exit 1
}

if (-not (Test-Path $OUTPUT_DIR)) {
    New-Item -ItemType Directory -Path $OUTPUT_DIR | Out-Null
}

Copy-Item "$BUILD_DIR\*.bin" $OUTPUT_DIR -Force -ErrorAction SilentlyContinue

Rename-Item "$OUTPUT_DIR\TEF6686_ESP32.ino.bootloader.bin" "bootloader.bin" -ErrorAction SilentlyContinue
Rename-Item "$OUTPUT_DIR\TEF6686_ESP32.ino.partitions.bin" "partitions.bin" -ErrorAction SilentlyContinue

# =====================================================
# CALCULAR ENDERECO SPIFFS (64 KB)
# =====================================================
$fwSize = (Get-Item "$OUTPUT_DIR\TEF6686_ESP32.ino.bin").Length
$SPIFFS_ADDR = [int64](
    [Math]::Floor(($fwSize + 0x10000) / 0x10000) * 0x10000
)
$SPIFFS_ADDR_HEX = "0x{0:X}" -f $SPIFFS_ADDR

Write-Host "SPIFFS address: $SPIFFS_ADDR_HEX`n"

# =====================================================
# PORTA SERIAL
# =====================================================
$SERIAL_PORT = Read-Host "Enter serial port (ex: COM3)"
if ([string]::IsNullOrWhiteSpace($SERIAL_PORT)) {
    exit 1
}

# =====================================================
# DOWNLOAD MODE
# =====================================================
do {
    $ready = Read-Host "Is ESP32 in download mode? (Y/n)"
    if ($ready -eq "") { $ready = "Y" }
    if ($ready -match '^[Nn]$') {
        Write-Host "Hold BOOT and press RESET"
    }
} until ($ready -match '^[Yy]$')

# =====================================================
# FLASH (MULTI-BAUD)
# =====================================================
$flashSuccess = $false

foreach ($baud in $BAUDS) {
    if (-not $flashSuccess) {

        Write-Host "`nFlashing at $baud baud..."

        & $ESP.Cmd @($ESP.Args) `
            --chip esp32 `
            --port $SERIAL_PORT `
            --baud $baud `
            write_flash -z `
            0x1000 "$OUTPUT_DIR\bootloader.bin" `
            0x8000 "$OUTPUT_DIR\partitions.bin" `
            0xe000 "$OUTPUT_DIR\boot_app0.bin" `
            0x10000 "$OUTPUT_DIR\TEF6686_ESP32.ino.bin" `
            $SPIFFS_ADDR_HEX "$OUTPUT_DIR\TEF6686_ESP32.spiffs.bin"

        if ($LASTEXITCODE -eq 0) {
            $flashSuccess = $true
        } else {
            Write-Host "[WARNING] Failed at $baud baud"
        }
    }
}

if (-not $flashSuccess) {
    Write-Host "`n[ERROR] Flash failed at all baud rates"
    Read-Host "Press ENTER to exit"
    exit 1
}

Write-Host "`n======================================"
Write-Host " FLASH COMPLETED SUCCESSFULLY "
Write-Host "======================================"
Read-Host "Press ENTER to exit"
