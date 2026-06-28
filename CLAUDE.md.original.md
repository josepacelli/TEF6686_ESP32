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
   - **ESP32Time**
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

`Estacao` struct: `freq_khz`, `ps`, `rt`, `pty_code`, `pi_code`, `musica`, `anoMusica`, `posScroll`, `hora/minuto/segundo`, `dia/mes/ano`, `tempo`, `temperatura`, `rds_ativo`.

`carregarEstacoes()` fills a `std::vector<Estacao> estacoes` (static, file-local) with **40 Fortaleza/CE Brazilian radio stations** (indices 0–39). Uses the `STATION(freq, pty, idx)` macro to populate each entry. Stations are loaded in near-ascending frequency order then sorted by `freq_khz` at the end. Called once in `setup()`.

`findEstacao()` (file-local) matches exact `freq_khz` first, then falls back to ±100 kHz tolerance.

`conteudo.cpp` contains genre arrays and random generation helpers used as fallback when station has no song data.

**PS/RT Priority:** Live RDS data (`radio.rds.stationName`, `radio.rds.rtRadio`) always takes precedence over the static database. Database entries are displayed only when no live RDS is received.

**PS/RT/Music Flow:**
- `buscarPS(freq_khz)` → live RDS name or formatted `ps_text | NTP datetime or static date | weather`
- `buscarRT(freq_khz)` → live RDS RT or `rt_text - song (year) | NTP datetime or static date | weather`
- `buscarPSPlain(freq_khz)` → raw `e->ps` with no formatting (for editor)
- `buscarRTPlain(freq_khz)` → raw `e->rt` with no formatting (for editor)
- `buscarPI(freq_khz)` → PI code as 4-char hex string
- `buscarGenero(freq_khz)` → genre label for PTY
- `rotateStationMusic(freq_khz)` — refresh music/year to new random value
- `rotateStationWeather(freq_khz)` — refresh time, date, weather to new random values
- `setAllRDS(bool)` — bulk enable/disable RDS for all stations
- `avancarScroll(freq_khz)`, `totalEstacoes()`, `getEstacao(i)`, `isRDSAtivo(freq_khz)`

### Multi-Language PS/RT System — Critical

There are **6 parallel language array files** for station PS and RT strings, all using the **same integer index** (currently 0–39):

| File | Content |
|------|---------|
| `src/ps_portugues.cpp` | Full Portuguese station name + tagline |
| `src/ps_ingles.cpp` | Short English station name + frequency |
| `src/ps_espanhol.cpp` | Short Spanish station name |
| `src/rt_portugues.cpp` | Full Portuguese RT string (max 64 chars) |
| `src/rt_ingles.cpp` | Full English RT string |
| `src/rt_espanhol.cpp` | Full Spanish RT string |

**When adding or renaming a station, all 6 language files must be updated at the same index.**

Dispatch lives in `src/ps_language.{h,cpp}` (`getPSByLanguage(index, language)`) and `src/rt_language.{h,cpp}` (`getRTByLanguage(index, language)`) via the `PTYLanguage` enum (stored in EEPROM byte 56 as `languageSet`):
- Values 2, 5, 8 → Portuguese arrays
- Values 3, 6, 9 → Spanish arrays
- All others / default → English arrays

The `Estacao.ps` and `Estacao.rt` fields are populated at `carregarEstacoes()` time. These base strings are then formatted by `montarPS()` and `buscarRT()` to include metadata (music, weather, NTP time, etc.).

### RDS Editor Presets (`src/ps_opcoes.{h,cpp}`, `src/rt_opcoes.{h,cpp}`)

Fixed arrays of generic station name / RT text options shown as dropdown choices in the RDS editor screen (`ui3.cpp`). Not linked to the station database.

### UI Rendering System (`src/ui_language.h`, `src/ui*.cpp`)

`src/ui_language.h` defines `UIStringID` enum (0 to `UI_STRING_COUNT`). Menu labels and prompts are looked up via `getUIString(UIStringID, language)` which dispatches across `UI_ENGLISH[]`, `UI_PORTUGUESE[]`, `UI_SPANISH[]`.

UI screens are split across three files:
- `src/ui1.cpp` — Main tuner display, frequency, signal meter, menu navigation
- `src/ui2.cpp` — Settings menus (volume, contrast, language, bandwidth, etc.)
- `src/ui3.cpp` — RDS editor, custom PS/RT/PTY edit screens, station details

Each file renders to a TFT_eSPI display. Menu system uses button interrupts (MODE/BW/POWER) and rotary encoder for input.

`src/arrows_ui.{h,cpp}` — helper functions for drawing directional arrows on the display (frequency, menu, scroll, preset navigation).

### PTY Language System (`src/pty_language.{h,cpp}`)

Nine PTY category string arrays covering RBDS and Brazilian variants, split across `src/pty_portugues.cpp`, `src/pty_ingles.cpp`, `src/pty_espanhol.cpp`. Language selected via `languageSet` EEPROM byte (56), applied via `setPTYLanguage()` in `setup()`. `getPTYName(code)` (in `conteudo.cpp`) returns current-language PTY label.

### Music Data (`src/musicas.{h,cpp}`)

Global arrays for random content generation: `brazilianArtists[]`, `musicGenres[]`, `brazilianYears[]`, `brazilianSongs[]`. Used by `conteudo.cpp` helpers (`generateRandomSong`, `getRandomGenreByPTY`, etc.) to populate filler entries.

### WiFi & NTP (`src/WiFiConnect.{h,cpp}`, `src/WiFiConnectParam.{h,cpp}`, `src/NTPupdate.{h,cpp}`)

WiFi runs on a dedicated FreeRTOS task (`wifiTask`) pinned to core 0 to avoid blocking the UI on core 1. On connect, it immediately calls `NTPupdate()` which syncs `ESP32Time rtc` from `0.pool.ntp.org`. Once synced, `NTPupdated = true` and `estacoes.cpp` inserts the live NTP datetime into PS/RT strings (overriding static date/time fields).

`WiFiConnect` (adapted from WiFiManager) provides a captive portal AP (`src/WC_AP_HTML.h` contains embedded HTML) for entering SSID/password without reflashing. Credentials are persisted in EEPROM bytes 59–157.

`src/logbook.h` — thin `log_info()` wrapper around `Serial.println()`.

### Frequency Mapping (`src/frequencia.{h,cpp}`)

`allFrequencies[]` — sorted array of all tunable FM frequencies (79.7–107.9 MHz). `allPTYCodes[]` — corresponding PTY code per frequency. `getPTYCodeForFreq(freq_khz)` lookup. Used for preset scanning and jump-to-frequency logic.

## EEPROM Layout (157 bytes total)

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
| 57 | byte | `wifi` (0=disabled) |
| 58 | byte | `NTPoffset` (int8_t, e.g. 253 = -3 for BRT) |
| 59 | 33 bytes | `wifiSSID` (null-terminated string) |
| 92 | 65 bytes | `wifiPass` (null-terminated string) |

## Key Constants (Hardware Pins)

```
ROTARY_PIN_A=34  ROTARY_PIN_B=36  ROTARY_BUTTON=39
PIN_POT=35  PWRBUTTON=4  BWBUTTON=25  MODEBUTTON=26
CONTRASTPIN=2  STANDBYLED=19  SMETERPIN=27
```

## Adding a New Station — Checklist

1. Pick next available index (currently max is 39; next would be 40).
2. Add entry to **all 6 language files** (`ps_portugues.cpp`, `ps_ingles.cpp`, `ps_espanhol.cpp`, `rt_portugues.cpp`, `rt_ingles.cpp`, `rt_espanhol.cpp`) at that index.
3. Add a `STATION(freq_khz, pty_code, index)` call in `carregarEstacoes()`. Order in source doesn't matter — the sort at the end ensures ascending `freq_khz`. Frequency in kHz (e.g. 107100 for 107.1 MHz).
4. If the station has a known PI code, set `e.pi_code` before the `estacoes.push_back(e)` inside the macro expansion (requires macro adjustment or direct code).
5. Common `pty_code` values: 1=News, 2=Affairs, 4=Education, 6=Culture, 8=Talk, 10=Pop, 12=Easy Listening, 13=Classical, 20=Religious, 25=Folk/Forró, 26=Country.
