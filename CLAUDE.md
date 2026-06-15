# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Arduino/ESP32 firmware for the NXP TEF668x (Lithio) FM/AM tuner IC, with ILI9341 TFT display, rotary encoder UI, RDS decode, and a custom Brazilian radio station database. Current release: v1.16.

## Build & Flash

This is an **Arduino IDE** project. No Makefile or CLI build config exists.

1. Open `TEF6686_ESP32.ino` in Arduino IDE (2.x recommended)
2. Board: `ESP32 Dev Module`
3. Required libraries:
   - **TFT_eSPI** (Bodmer) — must configure `User_Setup.h` before first build (see below)
   - **TimeLib** (PaulStoffregen)
4. Flash via USB at 115200 baud

### TFT_eSPI User_Setup.h (mandatory)
```cpp
#define ILI9341_DRIVER
#define TFT_CS   5
#define TFT_DC   17
#define TFT_RST  16
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT
#define SPI_FREQUENCY 10000000
// ALL OTHER SETTINGS MUST BE COMMENTED OUT
```

For the ARS display variant (BGR type), define `#define ARS` in the main `.ino`.

## Architecture

### Entry Point
`TEF6686_ESP32.ino` — monolithic main sketch. Contains `setup()` / `loop()`, all global state (~80 variables), UI rendering, button/rotary handling, XDR-GTK serial protocol, RDS display, scan logic, and menu system.

### Hardware Abstraction Layers (`src/`)

| File | Role |
|------|------|
| `TEF6686.{h,cpp}` | Top-level chip class: `radio` object. RDS decode, frequency tuning, stereo, AGC, iMS/EQ, modulation. Owns the `rds_` struct. |
| `Tuner_Api.{h,cpp}` | Mid-level API: `Radio_SetFreq()`, `Radio_ChangeFreqOneStep()`, scan logic |
| `Tuner_Drv_Lithio.{h,cpp}` | Low-level register writes for the Lithio chip family |
| `Tuner_Interface.{h,cpp}` | I2C transport layer |
| `Tuner_Patch_Lithio_V*.h` | Firmware patch blobs for chip versions 101, 102, 205 — selected at runtime via `TEF` byte from EEPROM |

### Banco de Estações (`src/estacoes.{h,cpp}` + `src/conteudo.{h,cpp}`)

`Estacao` struct: `freq_khz`, `ps`, `rt`, `pty_code`, `musica`, `anoMusica`, `posScroll`, `hora/minuto/segundo`, `dia/mes/ano`, `tempo`, `temperatura`, `rds_ativo`.

`carregarEstacoes()` fills a `std::vector<Estacao> estacoes` (static, file-local) with ~100 Fortaleza/CE Brazilian radio stations. Called once in `setup()`.

`conteudo.cpp` contains genre arrays and random generation helpers used as fallback when station has no song data.

Lookup API (used in main sketch when tuning):
- `buscarPS(freq_khz)` → PS string (full formatted)
- `buscarRT(freq_khz)` → RT string (full formatted)
- `buscarPTY(freq_khz)` → PTY code
- `buscarMusica()`, `avancarScroll()`, `totalEstacoes()`, `getEstacao(i)`
- `isRDSAtivo(freq_khz)` → bool

### PTY Language System (`src/pty_language.{h,cpp}`)

Three PTY category string arrays: `PTY_ENGLISH` (pty1.cpp), `PTY_PORTUGUESE` (pty2.cpp), `PTY_SPANISH` (pty3.cpp). Language selected via `languageSet` EEPROM byte (56), applied via `setPTYLanguage()` in `setup()`.

### Music Data (`src/musicas.{h,cpp}`)

Global arrays for random content generation: `brazilianArtists[]`, `musicGenres[]`, `brazilianYears[]`, `brazilianSongs[]`. Used by `conteudo.cpp` helper functions (`getRandomRTByPTY`, `getSongForPTY`, etc.) to populate filler entries.

## EEPROM Layout (57 bytes)

| Offset | Size | Variable |
|--------|------|----------|
| 0 | uint32 | `frequency` (FM, kHz) |
| 4 | int32 | `VolSet` |
| 8 | int32 | `ConverterSet` |
| 12 | int32 | `LowEdgeSet` |
| 16 | int32 | `HighEdgeSet` |
| 20 | int32 | `ContrastSet` |
| 24 | int32 | `LevelOffset` |
| 28 | int32 | `StereoLevel` |
| 32 | int32 | `HighCutLevel` |
| 36 | int32 | `HighCutOffset` |
| 41 | byte | init flag (15 = initialized) |
| 42 | byte | `stepsize` |
| 43 | byte | `LowLevelSet` |
| 44 | byte | `iMSset` |
| 45 | byte | `EQset` |
| 46 | byte | `band` (0=FM, 1=AM) |
| 47 | uint32 | `frequency_AM` (kHz) |
| 52 | byte | `rotarymode` |
| 53 | byte | `displayflip` |
| 54 | byte | `TEF` (chip version: 101/102/205) |
| 55 | byte | `optenc` (optical encoder flag) |
| 56 | byte | `languageSet` (PTY language) |

## Key Constants (Hardware Pins)

```
ROTARY_PIN_A=34  ROTARY_PIN_B=36  ROTARY_BUTTON=39
PIN_POT=35  PWRBUTTON=4  BWBUTTON=25  MODEBUTTON=26
CONTRASTPIN=2  STANDBYLED=19  SMETERPIN=27
```

## In-Progress Files

- `src/frequencia.cpp` — new, currently empty
- `src/rds3.cpp` — new, currently empty
- `src/rds1.cpp`, `src/rds2.cpp` — listed in git status but don't exist on disk yet
