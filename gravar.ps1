# ======================================
# TEF6686_ESP32 - Build and Flash Tool
# Windows PowerShell
# ======================================

$ErrorActionPreference = "Stop"

# =====================================================
# CONFIGURAÇÕES
# =====================================================
$BUILD_DIR  = "build\esp32.esp32.esp32"
$OUTPUT_DIR = "build"
$DATA_DIR   = "data"

$DEFAULT_PORT = "COM16"
$SERIAL_PORT  = $DEFAULT_PORT

# =====================================================
# FUNÇÕES DE LOG
# =====================================================
function Write-Green($msg)  { Write-Host $msg -ForegroundColor Green }
function Write-Yellow($msg) { Write-Host $msg -ForegroundColor Yellow }
function Write-Red($msg)    { Write-Host $msg -ForegroundColor Red }
function Write-Cyan($msg)   { Write-Host $msg -ForegroundColor Cyan }
function Write-Gray($msg)   { Write-Host $msg -ForegroundColor Gray }

# =====================================================
# DETECTAR PORTAS COM
# =====================================================
function Find-ComPorts {
    $ports = @()
    Get-WmiObject Win32_SerialPort | ForEach-Object { $ports += $_.DeviceID }

    if ($ports.Count -eq 0) {
        # Fallback: check registry
        $ports = @(Get-ChildItem "HKLM:\HARDWARE\DEVICEMAP\SERIALCOMM" -ErrorAction SilentlyContinue |
                   ForEach-Object { $_.GetValue($_) })
    }

    return $ports | Where-Object { $_ } | Sort-Object
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

# =====================================================
# VERIFICAR DEPENDÊNCIAS
# =====================================================
function Verify-Dependencies {
    Write-Cyan "`n[CHECK] Verificando dependências..."

    $missing = @()

    if (-not (Get-Command arduino-cli -ErrorAction SilentlyContinue)) {
        $missing += "arduino-cli"
    }

    if (-not (Find-Esptool)) {
        $missing += "esptool/python"
    }

    if ($missing.Count -gt 0) {
        Write-Red "[ERROR] Faltam ferramentas:"
        foreach ($tool in $missing) {
            Write-Host "  - $tool"
        }
        Write-Host ""
        Write-Host "Instale com:"
        Write-Host "  - arduino-cli: https://arduino.github.io/arduino-cli/"
        Write-Host "  - esptool: pip install --user esptool"
        exit 1
    }

    Write-Green "[OK] Todas as dependências encontradas"
}

# =====================================================
# HEADER
# =====================================================
Write-Host "======================================"
Write-Host "TEF6686_ESP32 - Build and Flash Tool"
Write-Host "======================================"

# Verificar dependências
Verify-Dependencies

# =====================================================
# DETECTAR ESPTOOL
# =====================================================
$ESP = Find-Esptool
Write-Cyan "`n[INFO] esptool: $($ESP.Cmd) $($ESP.Args -join ' ')"

# =====================================================
# CRIAR DATA/ SE NÃO EXISTIR
# =====================================================
if (-not (Test-Path $DATA_DIR)) {
    Write-Yellow "[WARN] $DATA_DIR não existe, criando..."
    New-Item -ItemType Directory -Path $DATA_DIR | Out-Null
    Write-Green "[OK] $DATA_DIR criado"
}

# =====================================================
# MENU
# =====================================================
Write-Host ""
Write-Cyan "[OPÇÕES]"
Write-Host "1 - Compilar + Flash (com SPIFFS)"
Write-Host "2 - Flash apenas (firmware existente)"
Write-Host "3 - Erase flash + Flash (limpeza completa)"
Write-Host "4 - Apenas compilar"
Write-Host ""
$choice = Read-Host "Escolha uma opção [1-4]"

$DO_COMPILE = $choice -in @("1", "4")
$DO_FLASH = $choice -in @("1", "2", "3")
$ERASE_FIRST = $choice -eq "3"

# =====================================================
# COMPILAR
# =====================================================
if ($DO_COMPILE) {
    Write-Cyan "`n[COMPILE] Compilando com Arduino CLI..."

    arduino-cli compile `
        --fqbn esp32:esp32:esp32:PartitionScheme=huge_app,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none,EraseFlash=none `
        --export-binaries `
        TEF6686_ESP32.ino

    if ($LASTEXITCODE -ne 0) {
        Write-Red "[ERROR] Compilação falhou"
        exit 1
    }
    Write-Green "[OK] Compilação bem-sucedida"
}

if (-not $DO_FLASH) {
    Write-Green "`n[OK] Compilação concluída. Saindo."
    exit 0
}

# =====================================================
# VERIFICAR FIRMWARE
# =====================================================
$FIRMWARE = "$BUILD_DIR\TEF6686_ESP32.ino.bin"
if (-not (Test-Path $FIRMWARE)) {
    Write-Red "[ERROR] Firmware não encontrado: $FIRMWARE"
    Write-Host "Compile primeiro usando a opção 1 ou 4"
    exit 1
}
Write-Cyan "`n[FIRMWARE] $(Get-Item $FIRMWARE | Select-Object -ExpandProperty Length) bytes"

# =====================================================
# SPIFFS
# =====================================================
$SPIFFS_BIN = "$OUTPUT_DIR\TEF6686_ESP32.spiffs.bin"
$SPIFFS_ENABLED = $false

if (Test-Path $DATA_DIR) {
    $dataSize = (Get-ChildItem -Path $DATA_DIR -Recurse -File -ErrorAction SilentlyContinue |
                 Measure-Object -Property Length -Sum).Sum
    if (-not $dataSize) { $dataSize = 0 }

    if ($dataSize -gt 0) {
        Write-Cyan "`n[SPIFFS] Tamanho da pasta data/: $dataSize bytes"

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
            New-Item -ItemType Directory -Force -Path $OUTPUT_DIR | Out-Null

            # Tamanhos preferidos para SPIFFS
            $preferredSizes = @(819200, 1048576, 1310720, 1572864) # 800KB, 1MB, 1.25MB, 1.5MB

            $mkspiffsSucceeded = $false
            foreach ($size in $preferredSizes) {
                if ($size -lt ($dataSize + 1024)) { continue }

                Write-Host "Tentando mkspiffs com $size bytes..."
                & $mkspiffs -c $DATA_DIR -b 4096 -p 256 -s $size $SPIFFS_BIN 2>&1 | Out-Null

                if ($LASTEXITCODE -eq 0 -and (Test-Path $SPIFFS_BIN)) {
                    Write-Green "[OK] SPIFFS gerado ($size bytes)"
                    $SPIFFS_ENABLED = $true
                    $mkspiffsSucceeded = $true
                    break
                } else {
                    if (Test-Path $SPIFFS_BIN) { Remove-Item $SPIFFS_BIN -Force }
                }
            }

            if (-not $mkspiffsSucceeded) {
                Write-Yellow "[WARN] mkspiffs falhou, continuando sem SPIFFS"
            }
        } else {
            Write-Yellow "[WARN] mkspiffs não encontrado, SPIFFS desabilitado"
        }
    } else {
        Write-Gray "[INFO] Pasta data/ vazia, SPIFFS desabilitado"
    }
} else {
    Write-Gray "[INFO] Pasta data/ não existe, SPIFFS desabilitado"
}

# =====================================================
# COPIAR BINÁRIOS
# =====================================================
Write-Cyan "`n[BINS] Copiando binários..."
New-Item -ItemType Directory -Force -Path $OUTPUT_DIR | Out-Null

Copy-Item "$BUILD_DIR\*.bin" $OUTPUT_DIR -Force -ErrorAction SilentlyContinue

Rename-Item "$OUTPUT_DIR\TEF6686_ESP32.ino.bootloader.bin" "bootloader.bin" -Force -ErrorAction SilentlyContinue
Rename-Item "$OUTPUT_DIR\TEF6686_ESP32.ino.partitions.bin" "partitions.bin" -Force -ErrorAction SilentlyContinue

# boot_app0.bin
$BOOT_APP = "$OUTPUT_DIR\boot_app0.bin"
if (-not (Test-Path $BOOT_APP)) {
    $bootSrc = Get-ChildItem `
        "$env:LOCALAPPDATA\Arduino15\packages\esp32\hardware\esp32" `
        -Recurse -Filter boot_app0.bin -ErrorAction SilentlyContinue |
        Select-Object -First 1

    if ($bootSrc) {
        Copy-Item $bootSrc.FullName $BOOT_APP
        Write-Green "[OK] boot_app0.bin"
    } else {
        Write-Yellow "[WARN] boot_app0.bin não encontrado"
    }
} else {
    Write-Green "[OK] boot_app0.bin (cached)"
}

# Verificar binários essenciais
$bootloader = "$OUTPUT_DIR\bootloader.bin"
$partitions = "$OUTPUT_DIR\partitions.bin"

if (-not (Test-Path $bootloader) -or -not (Test-Path $partitions)) {
    Write-Red "[ERROR] Binários necessários não encontrados"
    exit 1
}

# =====================================================
# CALCULAR ENDEREÇO SPIFFS
# =====================================================
$fwSize = (Get-Item $FIRMWARE).Length
$SPIFFS_ADDR = ("0x{0:X}" -f ((($fwSize + 0x10000 + 0xFFFF) -band 0xFFFF0000)))

# =====================================================
# PORTA SERIAL
# =====================================================
Write-Cyan "`n[PORT] Detectando portas COM..."
$availablePorts = @(Find-ComPorts)

if ($availablePorts.Count -eq 0) {
    Write-Yellow "[WARN] Nenhuma porta COM detectada"
    $SERIAL_PORT = Read-Host "Digite a porta COM (ex: COM3)"
} elseif ($availablePorts.Count -eq 1) {
    $SERIAL_PORT = $availablePorts[0]
    Write-Green "[OK] Porta detectada: $SERIAL_PORT"
} else {
    Write-Host "Portas disponíveis:"
    for ($i = 0; $i -lt $availablePorts.Count; $i++) {
        $marker = if ($availablePorts[$i] -eq $DEFAULT_PORT) { " (padrão)" } else { "" }
        Write-Host "  $($i+1) - $($availablePorts[$i])$marker"
    }

    $portChoice = Read-Host "Escolha porta [1-$($availablePorts.Count)]"
    if ($portChoice -match "^\d+$" -and [int]$portChoice -ge 1 -and [int]$portChoice -le $availablePorts.Count) {
        $SERIAL_PORT = $availablePorts[[int]$portChoice - 1]
    } else {
        $SERIAL_PORT = $DEFAULT_PORT
    }
    Write-Green "[OK] Usando porta: $SERIAL_PORT"
}

# =====================================================
# APAGAR FLASH (OPCIONAL)
# =====================================================
if ($ERASE_FIRST) {
    Write-Cyan "`n[ERASE] Apagando flash..."

    & $ESP.Cmd @($ESP.Args) `
        --chip esp32 `
        --port $SERIAL_PORT `
        --baud 460800 `
        erase_flash

    if ($LASTEXITCODE -ne 0) {
        Write-Yellow "[WARN] Erase retornou erro, continuando..."
    } else {
        Write-Green "[OK] Flash apagado"
    }
}

# =====================================================
# PREPARAR FLASH
# =====================================================
Write-Cyan "`n[FLASH] Preparando arquivos..."
$FLASH_FILES = @(
    "0x1000",  "$OUTPUT_DIR\bootloader.bin",
    "0x8000",  "$OUTPUT_DIR\partitions.bin",
    "0xe000",  "$OUTPUT_DIR\boot_app0.bin",
    "0x10000", "$OUTPUT_DIR\TEF6686_ESP32.ino.bin"
)

if ($SPIFFS_ENABLED -and (Test-Path $SPIFFS_BIN)) {
    $FLASH_FILES += $SPIFFS_ADDR
    $FLASH_FILES += $SPIFFS_BIN
    Write-Gray "  SPIFFS em $SPIFFS_ADDR"
}
else {
    Write-Gray "  SPIFFS desabilitado"
}

Write-Gray "  Bootloader: $(Get-Item $OUTPUT_DIR\bootloader.bin | ForEach-Object Length) bytes"
Write-Gray "  Partitions: $(Get-Item $OUTPUT_DIR\partitions.bin | ForEach-Object Length) bytes"
Write-Gray "  Firmware: $(Get-Item $FIRMWARE | ForEach-Object Length) bytes"

# =====================================================
# FLASH (COM FALLBACK DE BAUDRATE)
# =====================================================
$BAUDS = @(921600, 460800, 115200)
$FLASH_SUCCESS = $false

foreach ($BAUD in $BAUDS) {
    Write-Cyan "`n[UPLOAD] Gravando em $BAUD baud..."

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
        @FLASH_FILES 2>&1 | Tee-Object -Variable flashOutput | ForEach-Object { Write-Gray $_ }

    if ($LASTEXITCODE -eq 0) {
        $FLASH_SUCCESS = $true
        break
    } else {
        Write-Yellow "[WARN] Flash falhou em $BAUD baud, tentando próximo..."
    }
}

if (-not $FLASH_SUCCESS) {
    Write-Red "[ERROR] Flash falhou em todos os baudrates"
    exit 1
}

# =====================================================
# SUCESSO
# =====================================================
Write-Host ""
Write-Green "======================================"
Write-Green "Flash concluído com sucesso!"
Write-Green "======================================"
Write-Host ""

# Monitor opcional
$monitor = Read-Host "Abrir monitor serial? (s/n)"
if ($monitor -match "^[SsYy]$") {
    Write-Cyan "`n[MONITOR] Conectando em 115200 baud..."
    & $ESP.Cmd @($ESP.Args) --chip esp32 --port $SERIAL_PORT --baud 115200 monitor
}
