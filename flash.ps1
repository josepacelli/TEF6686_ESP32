#!/usr/bin/env pwsh
# ======================================
# TEF6686_ESP32 - Simple Flash Tool
# Based on batch flash script
# Default port: COM16
# ======================================

# Color helpers
function Write-Green($msg)  { Write-Host $msg -ForegroundColor Green }
function Write-Yellow($msg) { Write-Host $msg -ForegroundColor Yellow }
function Write-Red($msg)    { Write-Host $msg -ForegroundColor Red }

# Header
Write-Host "-----------------------------"
Write-Host "- Update tool ESP32 TEF6686 -"
Write-Host "-       Version 1.16        -"
Write-Host "-----------------------------"
Write-Host ""

# List available COM ports
Write-Host "Available COM-ports:"
$ports = Get-WmiObject Win32_PnPEntity -Filter "Name like '%(COM%'" -ErrorAction SilentlyContinue |
    Select-Object -ExpandProperty Name | ForEach-Object { $_.Split('(')[1].TrimEnd(')') } | Sort-Object
if ($ports) {
    $ports | ForEach-Object { Write-Host "  $_" }
} else {
    Write-Host "  (none detected)"
}
Write-Host ""

# Get COM port (default COM16)
$input = Read-Host "Enter COM port number, e.g. '16' for COM16 [default: 16]"
if ([string]::IsNullOrWhiteSpace($input)) {
    $comPort = "COM16"
} else {
    if ($input -match "^COM\d+") {
        $comPort = $input
    } else {
        $comPort = "COM$input"
    }
}

Write-Host ""
Write-Green "Using port: $comPort"
Write-Host ""

# Compile?
$compile = Read-Host "Compile firmware first? (y/n) [default: y]"
if ([string]::IsNullOrWhiteSpace($compile) -or $compile -match "^[Yy]$") {
    if (-not (Get-Command arduino-cli -ErrorAction SilentlyContinue)) {
        Write-Red "ERROR: arduino-cli not found"
        exit 1
    }

    Write-Host "Compiling..."
    arduino-cli compile `
        --fqbn esp32:esp32:esp32:PartitionScheme=huge_app,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600 `
        --export-binaries `
        TEF6686_ESP32.ino

    if ($LASTEXITCODE -ne 0) {
        Write-Red "ERROR: Compilation failed"
        exit 1
    }
    Write-Green "Compilation successful"
    Write-Host ""
}

# Define binary files
$buildDir = "build\esp32.esp32.esp32"
$OUTPUT_DIR = "build"

# Ensure boot_app0.bin exists
$BOOT_APP = "$OUTPUT_DIR\boot_app0.bin"
if (-not (Test-Path $BOOT_APP)) {
    Write-Host "Locating boot_app0.bin..."
    $bootSrc = Get-ChildItem `
        "$env:LOCALAPPDATA\Arduino15\packages\esp32\hardware\esp32" `
        -Recurse -Filter boot_app0.bin -ErrorAction SilentlyContinue |
        Select-Object -First 1

    if ($bootSrc) {
        Copy-Item $bootSrc.FullName $BOOT_APP
        Write-Green "[OK] boot_app0.bin copied"
    } else {
        Write-Yellow "[WARN] boot_app0.bin not found in Arduino packages"
    }
} else {
    Write-Green "[OK] boot_app0.bin (cached)"
}
Write-Host ""

$binaries = @(
    @{ offset = "0x1000"; file = "$buildDir\TEF6686_ESP32.ino.bootloader.bin"; name = "Bootloader" },
    @{ offset = "0x8000"; file = "$buildDir\TEF6686_ESP32.ino.partitions.bin"; name = "Partitions" },
    @{ offset = "0xe000"; file = "build\boot_app0.bin"; name = "Boot App0" },
    @{ offset = "0x10000"; file = "$buildDir\TEF6686_ESP32.ino.bin"; name = "Firmware" }
)

# Verify all files exist
Write-Host "Checking binary files..."
$allOk = $true
foreach ($bin in $binaries) {
    if (Test-Path $bin.file) {
        Write-Green "  [OK] $($bin.name): $($bin.file)"
    } else {
        Write-Red "  [ERROR] Missing: $($bin.file)"
        $allOk = $false
    }
}

if (-not $allOk) {
    Write-Red "ERROR: Missing binary files. Compile first with arduino-cli."
    exit 1
}

Write-Host ""
Write-Host "Starting flash..."
Write-Host ""

# Build esptool command
$args = @(
    "--chip", "esp32",
    "--port", $comPort,
    "--baud", "921600",
    "--before", "default_reset",
    "--after", "hard_reset",
    "write_flash", "-z",
    "--flash_mode", "dio",
    "--flash_freq", "80m",
    "--flash_size", "4MB"
)

# Add all binaries
foreach ($bin in $binaries) {
    $args += $bin.offset
    $args += $bin.file
}

# Flash
Write-Host "Running: esptool.exe @args"
Write-Host ""
& esptool.exe @args
$exitCode = $LASTEXITCODE

Write-Host ""
if ($exitCode -eq 0) {
    Write-Green "======================================"
    Write-Green "SUCCESS: Firmware flashed!"
    Write-Green "======================================"
} else {
    Write-Red "======================================"
    Write-Red "ERROR: Flash failed (exit code $exitCode)"
    Write-Red "======================================"
    exit 1
}

