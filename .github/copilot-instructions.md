# Copilot Instructions for TEF6686_ESP32

## Project Overview
- **Purpose:** Advanced tuner software for NXP TEF668x tuners using ESP32, with a touchscreen GUI, WiFi, RDS, NTP, and logging features.
- **Main Entry:** `TEF6686_ESP32.ino` orchestrates initialization and high-level logic.
- **Core Logic:** Most functionality is implemented in `src/` C++ files, organized by feature (e.g., tuner, GUI, comms, RDS, WiFi, logging).

## Architecture & Key Components
- **Tuner Control:**
  - `TEF6686.cpp/h` manages tuner logic and hardware interface, using `Tuner_Drv_Lithio.*` and `Tuner_Interface.*` for low-level I2C communication.
  - Band, frequency, and patch data are handled via constants and patch header files.
- **GUI:**
  - `gui.cpp/h` implements the touchscreen interface, color themes, and display logic using TFT_eSPI.
  - Touch events are handled in `touch.cpp/h`.
- **Networking:**
  - WiFi setup and captive portal: `WiFiConnect.*`, `WiFiConnectParam.*`, and `WC_AP_HTML.h` (customized from WiFiManager).
  - NTP time sync: `NTPupdate.*`.
  - UDP/TCP comms: `comms.*` (handles remote commands, station list, etc.).
- **RDS:**
  - `rds.cpp/h` and `RdsPiBuffer.hpp` handle Radio Data System decoding and display.
- **Logging:**
  - `logbook.cpp/h` manages station logs, stored as CSV in SPIFFS (`/logbook.csv`).
- **Data:**
  - `data/` contains CSV files with frequency/channel data for different regions.

## Developer Workflows
- **Build/Upload:**
  - Use the Arduino IDE or PlatformIO for ESP32. Required libraries: TFT_eSPI, TimeLib, WiFi, etc. (see includes in `.ino` and `src/`).
  - SPIFFS must be uploaded separately for web/logbook/data files.
- **Debugging:**
  - Serial output is used for debugging; check for `Serial.print` in code.
  - Many features are toggled by global flags (see `constants.h`, `gui.h`, etc.).
- **Testing:**
  - No formal test suite; manual testing via device and web interface.

## Project-Specific Patterns & Conventions
- **Global State:**
  - Many features use global variables declared as `extern` in headers (e.g., `menu`, `wifi`, `scandxmode`).
- **UI Themes:**
  - Color and theme logic is centralized in `gui.cpp` (`doTheme()` function).
- **Web Interface:**
  - Web server endpoints are defined in `logbook.cpp` and related files.
- **CSV Data:**
  - Frequency/channel data is parsed from `data/*.csv` at runtime.
- **Customization:**
  - WiFi captive portal and parameters are heavily customized from upstream WiFiManager.

## Integration Points & External Dependencies
- **Libraries:** TFT_eSPI, TimeLib, WiFi, SPIFFS, WebServer, EEPROM, ArduinoJson, etc.
- **Hardware:** Designed for ESP32 with NXP TEF668x tuner and touchscreen display.
- **External Data:** Uses CSV files for frequency/channel info; logbook stored in SPIFFS.

## Key Files/Directories
- `TEF6686_ESP32.ino` — main entry point
- `src/TEF6686.*`, `src/Tuner_Drv_Lithio.*`, `src/Tuner_Interface.*` — tuner logic
- `src/gui.*`, `src/touch.*` — GUI and touch handling
- `src/comms.*` — network communication
- `src/rds.*`, `src/RdsPiBuffer.hpp` — RDS
- `src/logbook.*` — logging and web interface
- `src/WiFiConnect*`, `src/WC_AP_HTML.h` — WiFi/captive portal
- `data/` — frequency/channel CSVs

## Useful Links
- [Project Wiki](https://github.com/PE5PVB/TEF6686_ESP32/wiki)
- [Upstream WiFiConnect](https://github.com/smurf0969/WiFiConnect)

---
For questions about unclear patterns or missing documentation, check the project Wiki or open a discussion.
