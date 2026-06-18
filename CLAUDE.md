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

`carregarEstacoes()` fills a `std::vector<Estacao> estacoes` (static, file-local) with 84 Fortaleza/CE Brazilian radio stations. Stations are added in ascending frequency order (79.7–107.9 MHz) for readability. Called once in `setup()`. Vector is sorted by `freq_khz` at the end of the function.

`findEstacao()` (file-local) matches exact `freq_khz` first, then falls back to ±100 kHz tolerance.

`conteudo.cpp` contains genre arrays and random generation helpers used as fallback when station has no song data.

**PS/RT/Music Flow:**
- `buscarPS(freq_khz)` formats: `[pty_code] PtyName | ps_text | song_name (year) | date time | weather`
- `buscarRT(freq_khz)` formats: `[pty_code] PtyName | rt_text - song_name (year) | date time | weather`
- Music is fetched via `buscarMusica()` and rotated (scrolled) via `avancarScroll()` for long song names.

Lookup API (used in main sketch when tuning):
- `buscarPS(freq_khz)` → PS string (full formatted with music)
- `buscarRT(freq_khz)` → RT string (full formatted with music)
- `buscarPTY(freq_khz)` → PTY code
- `buscarMusica(freq_khz)` → song name (auto-generates random if empty)
- `avancarScroll(freq_khz)`, `totalEstacoes()`, `getEstacao(i)`, `isRDSAtivo(freq_khz)`

### Multi-Language PS/RT System — Critical

There are **6 parallel language array files** for station PS and RT strings, all using the **same integer index** (currently 0–83):

| File | Content |
|------|---------|
| `src/ps_portugues.cpp` | Full Portuguese station name + tagline |
| `src/ps_ingles.cpp` | Short English station name + frequency |
| `src/ps_espanhol.cpp` | Short Spanish station name |
| `src/rt_portugues.cpp` | Full Portuguese RT string (max 64 chars) |
| `src/rt_ingles.cpp` | Full English RT string |
| `src/rt_espanhol.cpp` | Full Spanish RT string |

**When adding or renaming a station, all 6 language files must be updated at the same index.**

`getPSByLanguage(index, language)` and `getRTByLanguage(index, language)` dispatch via the `PTYLanguage` enum (stored in EEPROM byte 56 as `languageSet`):
- Values 2, 5, 8 → Portuguese arrays
- Values 3, 6, 9 → Spanish arrays
- All others / default → English arrays

The `Estacao.ps` and `Estacao.rt` fields are populated at `carregarEstacoes()` time by calling `getPSByLanguage(index, currentPTYLanguage)`. These base strings are then formatted by `montarPS()` and `buscarRT()` to include metadata (PTY, music, weather, etc.).

### UI Rendering System (`src/ui_language.h`, `src/ui*.cpp`)

`src/ui_language.h` defines `UIStringID` enum (0 to `UI_STRING_COUNT`). Menu labels and prompts are looked up via `getUIString(UIStringID, language)` which dispatches across `UI_ENGLISH[]`, `UI_PORTUGUESE[]`, `UI_SPANISH[]`.

UI screens are split across three files:
- `src/ui1.cpp` — Main tuner display, frequency, signal meter, menu navigation
- `src/ui2.cpp` — Settings menus (volume, contrast, language, bandwidth, etc.)
- `src/ui3.cpp` — RDS editor, custom PS/RT/PTY edit screens, station details

Each file renders to a TFT_eSPI display. Menu system uses button interrupts (MODE/BW/POWER) and rotary encoder for input.

### PTY Language System (`src/pty_language.{h,cpp}`)

Nine PTY category string arrays covering RBDS and Brazilian variants. Language selected via `languageSet` EEPROM byte (56), applied via `setPTYLanguage()` in `setup()`. `getPTYName(code)` (in `conteudo.cpp`) returns current-language PTY label.

### Music Data (`src/musicas.{h,cpp}`)

Global arrays for random content generation: `brazilianArtists[]`, `musicGenres[]`, `brazilianYears[]`, `brazilianSongs[]`. Used by `conteudo.cpp` helpers (`generateRandomSong`, `getRandomGenreByPTY`, etc.) to populate filler entries.

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
| 56 | byte | `languageSet` (PTYLanguage enum value) |

## Key Constants (Hardware Pins)

```
ROTARY_PIN_A=34  ROTARY_PIN_B=36  ROTARY_BUTTON=39
PIN_POT=35  PWRBUTTON=4  BWBUTTON=25  MODEBUTTON=26
CONTRASTPIN=2  STANDBYLED=19  SMETERPIN=27
```

## Adding a New Station — Checklist

1. Pick next available index (currently max is 83; next would be 84).
2. Add entry to **all 6 language files** (`ps_portugues.cpp`, `ps_ingles.cpp`, `ps_espanhol.cpp`, `rt_portugues.cpp`, `rt_ingles.cpp`, `rt_espanhol.cpp`) at that index.
3. Add `Estacao` block in `carregarEstacoes()` in **frequency order** (ascending, 79.7–107.9 MHz). Format: `freq_khz` in kHz (e.g. 107100 for 107.1 MHz). Include all metadata: `pty_code`, PS/RT language indices, `musica`, times, weather. The sort at the end ensures consistency.
4. Common `pty_code` values: 1=News, 2=Affairs, 4=Education, 6=Culture, 8=Talk, 10=Pop, 12=Easy Listening, 13=Classical, 20=Religious, 25=Folk/Forró, 26=Country.

## Frequency Mapping

`src/frequencia.cpp` declares `allFrequencies[]` — sorted array of all tunable FM frequencies (79.7–107.9 MHz) with their corresponding `Estacao` language indices. Used for preset scanning and jump-to-frequency logic.

## Empty/Stub Files

- `src/rds3.cpp` — currently empty, reserved for future RDS features
- `src/rds1.cpp`, `src/rds2.cpp` — referenced in git history but don't exist on disk
