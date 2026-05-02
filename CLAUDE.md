# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

ESP32 firmware for a TEF668x FM/AM radio tuner with touchscreen GUI, WiFi remote control, and RDS decoding. Written as an Arduino sketch.

## Build & Flash

**Compile** (via `arduino-cli`):
```bash
arduino-cli compile \
  --fqbn esp32:esp32:esp32:PartitionScheme=huge_app,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600 \
  --export-binaries \
  TEF6686_ESP32.ino
```

**Generate SPIFFS image** (from `/data` directory):
```bash
mkspiffs -c data -b 4096 -p 256 -s 819200 TEF6686_ESP32.spiffs.bin
```

**Flash full firmware**:
```bash
esptool --chip esp32 --port COM16 --baud 460800 write_flash \
  0x1000 bootloader.bin \
  0x8000 partitions.bin \
  0xe000 boot_app0.bin \
  0x10000 TEF6686_ESP32.ino.bin \
  0x200000 TEF6686_ESP32.spiffs.bin
```

**Interactive script** (handles compile + port selection + flash):
```bash
./gravar.sh
```
PowerShell equivalents: `flash.ps1`, `gravar.ps1`. SPIFFS-only: `gravar_spiff.sh`.

Default port is `COM16` (`.vscode/arduino.json`).

## Architecture

```
TEF6686_ESP32.ino       — main sketch: init, event loop, GPIO, mode switching (~5100 lines)
src/
  TEF6686.cpp/h         — high-level tuner logic: tuning, AF testing, RDS buffering, quality metrics
  Tuner_Drv_Lithio.cpp/h — TEF668x command API: frequency, bandwidth, volume, RDS commands
  Tuner_Interface.cpp/h  — I2C protocol layer (address 0x64), patch loading, reset
  gui.cpp/h             — TFT rendering: themes, frequency display, RDS info, menus
  touch.cpp/h           — touchscreen input handling
  rds.cpp/h             — full RDS decoder: groups 0A–15B, AF, RT, CT, TMC, PTY
  RdsPiBuffer.cpp/h     — circular buffer for PI codes
  comms.cpp/h           — XDRGTK protocol, RDS Spy USB/TCP, station list exchange
  WiFiConnect.cpp/h     — captive portal WiFi manager
  NTPupdate.cpp/h       — NTP time sync
  logbook.cpp/h         — SPIFFS CSV logbook + web server (port 80)
  constants.h           — version, frequency bands, thresholds, EEPROM offsets
  language.h            — multi-language string database
  Tuner_Patch_*.h       — binary firmware patches for TEF102/TEF205
data/
  *.csv                 — regional station frequency databases (uploaded to SPIFFS)
  logbook.csv           — runtime station log
  logo.png              — branding for web logbook
```

## Key Design Points

**I2C command structure** (`Tuner_Drv_Lithio.cpp`):
- All TEF668x commands go through `devTEF_Set_Cmd(module, cmd, len, args...)`
- Byte 0: Module ID — FM=32, AM=33, AUDIO=48, APPL=64
- Bytes 3+: 16-bit big-endian parameters
- 2ms delay after each write (unless `Data_Accelerator` is true)

**Chip patching** (`Tuner_Interface.cpp`):
- TEF102 uses 224-byte patch; TEF205 uses 512-byte patch
- Patch loaded on every boot via `Tuner_Patch(byte TEF)` before any commands

**EEPROM layout** (`constants.h`):
- 100+ settings at fixed offsets; 20 preset channels (frequency + band + BW + name)
- Never shift EEPROM addresses — breaks user saved settings

**SPIFFS**:
- Partition scheme `huge_app` leaves ~819 KB for SPIFFS
- `/data` directory contents uploaded separately from firmware
- Logbook CSV created at runtime; survives firmware OTA but not SPIFFS reflash

**Display** (`gui.cpp`):
- TFT_eSPI library (PE5PVB fork with dynamic SPI speed)
- 320×240 landscape; color themes stored as RGB565 arrays
- 4 built-in themes; theme index in EEPROM

**WiFi remote control** (`comms.cpp`):
- UDP commands: `*T<freq>` tune, `*U`/`*D` step, `*>`/`*<` seek
- XDRGTK protocol over TCP for RTL-SDR/HDSDR integration
- RDS Spy mode streams raw RDS blocks over USB serial

## GPIO Map

| Pin | Function |
|-----|----------|
| 34/36 | Rotary encoder A/B |
| 39 | Rotary button |
| 35 | Volume potentiometer (ADC) |
| 13 | Battery voltage (ADC) |
| 4 | Band button |
| 25 | Bandwidth button |
| 26 | Mode/Menu button |
| 2 | PWM contrast |
| 19 | Standby LED |
| 27 | S-meter PWM output |
| 33 | Touch IRQ |
| 14 | External IRQ |

## Required Libraries

`TFT_eSPI` (PE5PVB fork), `TimeLib`, `ESP32Time`, `ArduinoJson`, `Hash` — plus ESP32 core libraries (`Wire`, `WiFi`, `SPIFFS`, `WebServer`, `EEPROM`).
