# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

Arduino/ESP32 firmware. NXP TEF668x (Lithio) FM/AM tuner + ILI9341 TFT + rotary encoder + RDS + Brazilian station DB. v1.16.

## Build & Flash

Arduino IDE only. No Makefile.

1. Open `TEF6686_ESP32.ino`, Arduino IDE 2.x
2. Board: `ESP32 Dev Module`
3. Libs: **TFT_eSPI** (configure `User_Setup.h` first), **TimeLib**, **ESP32Time**
4. Flash USB 115200 baud

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

ARS display variant (BGR): `#define ARS` in main `.ino`.

## Architecture

### Entry Point
`TEF6686_ESP32.ino` — monolithic sketch. `setup()`/`loop()`, ~80 globals, UI render, button/rotary, XDR-GTK serial, RDS display, scan, menu.

### HAL (`src/`)

| File | Role |
|------|------|
| `TEF6686.{h,cpp}` | Chip class: `radio` obj. RDS decode, freq tuning, stereo, AGC, iMS/EQ. Owns `rds_` struct. |
| `Tuner_Api.{h,cpp}` | Mid API: `Radio_SetFreq()`, `Radio_ChangeFreqOneStep()`, scan |
| `Tuner_Drv_Lithio.{h,cpp}` | Low-level reg writes, Lithio chip |
| `Tuner_Interface.{h,cpp}` | I2C transport |
| `Tuner_Patch_Lithio_V*.h` | FW patch blobs v101/102/205 — selected via `TEF` EEPROM byte |

### Station DB (`src/estacoes.{h,cpp}` + `src/conteudo.{h,cpp}`)

`Estacao` fields: `freq_khz`, `ps`, `rt`, `pty_code`, `pi_code`, `musica`, `anoMusica`, `posScroll`, `hora/minuto/segundo`, `dia/mes/ano`, `tempo`, `temperatura`, `rds_ativo`.

`carregarEstacoes()` → `std::vector<Estacao> estacoes` (static, file-local), 40 Fortaleza/CE stations (idx 0–39). Uses `STATION(freq, pty, idx)` macro. Sorted by `freq_khz` at end. Called once in `setup()`.

`findEstacao()` (file-local): exact `freq_khz` → fallback ±100 kHz.

`conteudo.cpp`: genre arrays + random gen helpers → fallback when station lacks song data.

**Priority:** Live RDS (`radio.rds.stationName`, `radio.rds.rtRadio`) > static DB.

**API:**
- `buscarPS(f)` → live RDS name or `ps_text | NTP/static date | weather`
- `buscarRT(f)` → live RDS RT or `rt_text - song(year) | NTP/static date | weather`
- `buscarPSPlain(f)` → raw `e->ps` (editor use)
- `buscarRTPlain(f)` → raw `e->rt` (editor use)
- `buscarPI(f)` → PI code 4-char hex
- `buscarPTY(f)`, `buscarGenero(f)`
- `rotateStationMusic(f)` — new random music/year
- `rotateStationWeather(f)` — new random time/date/weather
- `setAllRDS(bool)` — bulk RDS on/off
- `avancarScroll(f)`, `totalEstacoes()`, `getEstacao(i)`, `isRDSAtivo(f)`

### Multi-Lang PS/RT — **Critical**

6 parallel arrays, same integer idx (0–39):

| File | Content |
|------|---------|
| `src/ps_portugues.cpp` | PT name+tagline |
| `src/ps_ingles.cpp` | EN name+freq |
| `src/ps_espanhol.cpp` | ES name |
| `src/rt_portugues.cpp` | PT RT (max 64 chars) |
| `src/rt_ingles.cpp` | EN RT |
| `src/rt_espanhol.cpp` | ES RT |

**Adding/renaming station → update all 6 at same idx.**

Dispatch: `getPSByLanguage(idx, lang)` (`src/ps_language.{h,cpp}`), `getRTByLanguage(idx, lang)` (`src/rt_language.{h,cpp}`). `PTYLanguage` enum → EEPROM byte 56 (`languageSet`):
- 2/5/8 → PT · 3/6/9 → ES · else → EN

`Estacao.ps`/`.rt` set at `carregarEstacoes()` → formatted by `montarPS()`/`buscarRT()` (adds music, weather, NTP time).

### RDS Editor Presets (`src/ps_opcoes.{h,cpp}`, `src/rt_opcoes.{h,cpp}`)

Fixed generic name/RT arrays → dropdown in `ui3.cpp`. Not linked to station DB.

### UI (`src/ui_language.h`, `src/ui*.cpp`)

`UIStringID` enum → `getUIString(id, lang)` → `UI_ENGLISH[]`/`UI_PORTUGUESE[]`/`UI_SPANISH[]`.

- `src/ui1.cpp` — main display, freq, signal meter, menu nav
- `src/ui2.cpp` — settings (vol, contrast, lang, BW, etc.)
- `src/ui3.cpp` — RDS editor, PS/RT/PTY edit, station details

Input: MODE/BW/POWER button interrupts + rotary. `src/arrows_ui.{h,cpp}` — TFT arrow draw helpers.

### PTY Lang (`src/pty_language.{h,cpp}`)

9 PTY arrays split: `pty_portugues.cpp`, `pty_ingles.cpp`, `pty_espanhol.cpp`. Set via `setPTYLanguage()` in `setup()`. `getPTYName(code)` in `conteudo.cpp`.

### Music (`src/musicas.{h,cpp}`)

`brazilianArtists[]`, `musicGenres[]`, `brazilianYears[]`, `brazilianSongs[]` → `conteudo.cpp` random helpers (`generateRandomSong`, `getRandomGenreByPTY`, etc.).

### WiFi & NTP (`src/WiFiConnect.{h,cpp}`, `src/WiFiConnectParam.{h,cpp}`, `src/NTPupdate.{h,cpp}`)

`wifiTask` FreeRTOS on core 0 → avoids UI block on core 1. On connect → `NTPupdate()` → syncs `ESP32Time rtc` from `0.pool.ntp.org` → `NTPupdated = true` → `estacoes.cpp` injects live NTP datetime into PS/RT (overrides static date/time fields).

`WiFiConnect` (WiFiManager fork): captive portal AP for SSID/pass entry. HTML in `src/WC_AP_HTML.h`. Creds → EEPROM 59–157.

`src/logbook.h` — `log_info()` = `Serial.println()` thin wrapper.

### Freq Map (`src/frequencia.{h,cpp}`)

`allFrequencies[]` — sorted FM 79.7–107.9 MHz. `allPTYCodes[]` — PTY per freq. `getPTYCodeForFreq(freq_khz)`. Used for preset scan/jump.

## EEPROM (157 bytes)

| Off | Size | Var |
|-----|------|-----|
| 0 | uint32 | `frequency` (FM kHz) |
| 4 | int32 | `VolSet` |
| 8 | int32 | `ConverterSet` |
| 12 | int32 | `LowEdgeSet` |
| 16 | int32 | `HighEdgeSet` |
| 20 | int32 | `ContrastSet` |
| 24 | int32 | `LevelOffset` |
| 28 | int32 | `StereoLevel` |
| 32 | int32 | `HighCutLevel` |
| 36 | int32 | `HighCutOffset` |
| 41 | byte | init flag (15=init'd) |
| 42 | byte | `stepsize` |
| 43 | byte | `LowLevelSet` |
| 44 | byte | `iMSset` |
| 45 | byte | `EQset` |
| 46 | byte | `band` (0=FM, 1=AM) |
| 47 | uint32 | `frequency_AM` (kHz) |
| 52 | byte | `rotarymode` |
| 53 | byte | `displayflip` |
| 54 | byte | `TEF` (101/102/205) |
| 55 | byte | `optenc` |
| 56 | byte | `languageSet` |
| 57 | byte | `wifi` (0=off) |
| 58 | byte | `NTPoffset` int8_t (253=-3 BRT) |
| 59 | 33B | `wifiSSID` |
| 92 | 65B | `wifiPass` |

## Pins

```
ROTARY_PIN_A=34  ROTARY_PIN_B=36  ROTARY_BUTTON=39
PIN_POT=35  PWRBUTTON=4  BWBUTTON=25  MODEBUTTON=26
CONTRASTPIN=2  STANDBYLED=19  SMETERPIN=27
```

## New Station Checklist

1. Next idx = 40.
2. Add at idx 40 in all 6 lang files: `ps_portugues.cpp`, `ps_ingles.cpp`, `ps_espanhol.cpp`, `rt_portugues.cpp`, `rt_ingles.cpp`, `rt_espanhol.cpp`.
3. Add `STATION(freq_khz, pty_code, 40)` in `carregarEstacoes()`. Source order irrelevant — sorted at end. Freq in kHz (107100 = 107.1 MHz).
4. PI code: set `e.pi_code` in macro expansion (needs macro adjust or direct code).
5. PTY: 1=News, 2=Affairs, 4=Education, 6=Culture, 8=Talk, 10=Pop, 12=Easy Listening, 13=Classical, 20=Religious, 25=Folk/Forró, 26=Country.
