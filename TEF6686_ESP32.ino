/*
********************************************************************************
  Advanced Tuner software for NXP TEF668x ESP32
   (c) 2023 Sjef Verhoeven pe5pvb@het-bar.net
    Based on Catena / NXP semiconductors API
********************************************************************************
  v1.00 - Release
  v1.01 - Add interrupt on both Rotary pins
        - Add default settings
        - Add Standby function
        - Forced reboot after XDR-GTK disconnect
  v1.02 - Fixed signalstrenght display when
          value is 0 after boot or isn't changed
          after menu close
        - Added stepsize option
        - Show "<" when signallevel is below
          0dBuV. Showing negative value is very
          unstable.
        - Cosmetic improvements
  v1.03 - When signallevel is < set value all
          statusses will only be updated
          every 0.5 seconds. Threshold can be
          set in the menu. RDS is then disabled
          This will also improve I2C and ADC
          noise on low level signals.
        - System can now show negative dBuV.
  v1.04 - Optimised TEF initialisation
        - Fix on peakhold modulation meter
        - Added Stereo squelch (turn knob right)
        - Added Squelch off (turn knob left)
        - Show current Squelch threshold
  v1.05 - Fixed bug: Patch was not loaded
          correctly into tuner after boot
        - In Low level mode, RDS logo disabled
        - Fix Stereo/mono toggle when using
          XDR-GTK
        - Fixed Squelch level readout when
          leaving menu. This could cause auto-
          scan to not work.
        - Fix: RDS logo was not disabled in low
          signal level mode
        - Changed advice for TFT frequency to
          reduce unwanted noise
        - Optimised soft-boot
  v1.06 - Slow down I2C speed to reduce noise
        - Fix auto scan when squelch level is
          below 0dBuV
        - Fix show Hz (not kHz) on high cut
          corner frequency
  v1.07 - Added multipath suppresion (iMS) and
          Channel Equalizer (EQ)
        - Long press rotary to toggle iMS and EQ
        - Added standby LED on D19
        - XDR-GTK: Use the antenna rotor buttons
          to start scan up or down
        - XDR-GTK: Enable IF+ to force low
          signal level mode
        - XDR-GTK: DX1 = Normal
                   DX2 = iMS enabled
                   DX3 = EQ enabled
                   DX4 = Both enabled
        - Small modifications on XDR-GTK routine
          for compatibility with modified
          XDR-GTK TEF6686 edition
        - Fixed small bug with displaytext in
          scan mode
  v1.08 - Fixed interrupted canvas lines on TFT
        - Added function to mute TFT using XDR-
          GTK. Use RF+ to use this function
        - Level offset is still as set when
          using XDR-GTK
        - Detach interrupts when writing
          frequency to display, to prevent
          display crash
        - Fixed bug when squelch is set to -1
        - Fixed bug that volume offset was reset
          to 0 after changing frequency
  v1.09 - Stereo indication didn't work when RF+
          was set in XDR-GTK
        - Minor fix: When for some reason the
          signallevel was an invalid value, now
          the limit is -10 to +120dBuV
        - No more short mute when changing frequency
        - tuned indication only on when signal
          valid.
        - In Auto mode frequency is now shown and
          direction can be changed on the fly.
          to stop searching, just press any key.
        - Minor bugfix in low signal lever timer.
  v1.10 - Stand-by function was bricked after last update.
        - Code optimised for compiler v2.0.0.
        - Startup frequency was always 100.0MHz. Fixed.
  v1.11 - Maximum frequency changed to 108MHz. (110MHz is not possible).
        - Added AM reception.
        - Hold BW button during boot to change direction of
          rotary encoder.
        - Hold Mode button during boot to flip the screen.
        - When RT was too long the canvas was overwritten.
  v1.12 - Stand-by key caused an issue when in menu mode.
        - Canvas was broken on some points in the screen.
        - Added possibility to connect analog signalmeter (thanks David DSP)
        - Added slow signalmeter riding (thanks David DSP)
        - S-meter for AM is corrected to the right values.
  v1.13 - Fix for frequency offset in AM.
        - Fix for flickering signalmeter when fluctuation is < 1dBuV.
        - Added support for David DSP's converter board.
  v1.14 - Bugfix: When signallevel drops down in digits, old digits were still displayed.
  v1.15 - Fix AM bandwidth setting on F6805 chipset
        - Added microcode for whole TEF668x family (Lithio)
        - Added C/N readout
        - S-meter and modulation meter update (Thanks David DSP)
        - Added splash screen with tuner info
        - Fixed problem when tuning beyond bandlimits on AM
        - FM level calibrated
        - Function added to choose between regular and optical rotary encoder (hold rotary button during boot)
  v1.16 - Fixed some bugs to make the script compatible with new boardmanagers (v2.x)
        - Rewritten rotary encoder routines
        - New RDS routine
        - Massive code cleanup
        - Fixed canvas when changing bands
        - Radiotext auto scroll when it does not fit in the window
        - Added RDS CT (clock only)
        - Added FullSearchRDS for TEF6687 & TEF6689
        - Added AM/FM indicator
        - Fixed S-meter averaging
        - Improved XDRGTK RDS output
        - On signal dropout, RDS PTY, PI and PS stays on screen in blue
********************************************************************************

  Analog signalmeter:
                       to meter
                          |
                R         V
  IO27 -------=====-----=====---|
               1 k     5 k POT

********************************************************************************
  Setup:
  During boot:
  Hold BW button to change rotary encoder direction
  Hold Mode button the flip screen
  Hold Standby button to calibrate analogue s-meter
  Hold rotary button to switch between normal and optical rotary encoder

  Manual:
  STANDBY SHORT PRESS: Switch band
  STANDBY LONG PRESS: Standby
  ROTARY SHORT PRESS: Set stepsize or navigate
  ROTARY LONG PRESS: Toggle iMS & EQ
  MODE SHORT PRESS: Switch between auto/manual
                    or exit menu
  MODE LONG PRESS: Open menu
  BW SHORT PRESS: Switch bandwidth setting
  BW LONG PRESS: Switch mono, or auto stereo
* ***********************************************


  Use this settings in TFT_eSPI User_Setup.h
  #define ILI9341_DRIVER
  #define TFT_CS          5
  #define TFT_DC          17
  #define TFT_RST         16
  #define LOAD_GLCD
  #define LOAD_FONT2
  #define LOAD_FONT4
  #define LOAD_FONT6
  #define LOAD_FONT7
  #define LOAD_FONT8
  #define LOAD_GFXFF
  #define SMOOTH_FONT
  #define SPI_FREQUENCY   10000000

  ALL OTHER SETTINGS SHOULD BE REMARKED!!!!

*/

#include "src/TEF6686.h"
#include "src/constants.h"
#include "src/pty_language.h"
#include <EEPROM.h>
#include <Wire.h>
#include <TFT_eSPI.h>         // https://github.com/Bodmer/TFT_eSPI
#include <TimeLib.h>          // https://github.com/PaulStoffregen/Time

#define TFT_GREYOUT     0x38E7

// Font styles for PS (Program Service)
struct PSFontStyle {
  uint8_t size;      // 1-8 (TFT_eSPI font size)
  uint16_t color;    // RGB565 color
  const char* name;  // Style name
};

const PSFontStyle PS_FONT_STYLES[] = {
  {2, 0xFFFF, "Small-White"},      // 0: Compact white
  {2, 0xFFE0, "Small-Yellow"},     // 1: Compact yellow
  {3, 0x07E0, "Medium-Green"},     // 2: Medium green
  {3, 0x07FF, "Medium-Cyan"},      // 3: Medium cyan
  {4, 0xF800, "Large-Red"},        // 4: Large red
  {4, 0xFDA0, "Large-Gold"},       // 5: Large gold
  {6, 0xFFFF, "XL-White"},         // 6: Extra large white
  {6, 0xF81F, "XL-Magenta"},       // 7: Extra large magenta
  {7, 0xFFE0, "Bold-Yellow"},      // 8: Bold yellow
  {8, 0x07FF, "Bold-Cyan"}         // 9: Bold cyan
};
#define PS_FONT_COUNT 10
uint8_t PS_FONT_INDEX = 0;  // Current PS font style

// Font styles for PTY (Program Type)
struct PTYFontStyle {
  uint8_t size;      // 1-8 (TFT_eSPI font size)
  uint16_t color;    // RGB565 color
  const char* name;  // Style name
};

const PTYFontStyle PTY_FONT_STYLES[] = {
  {1, 0xFFFF, "Tiny-White"},       // 0: Tiny white
  {1, 0x07FF, "Tiny-Cyan"},        // 1: Tiny cyan
  {2, 0xFFE0, "Small-Yellow"},     // 2: Small yellow
  {2, 0xF800, "Small-Red"},        // 3: Small red
  {3, 0x07E0, "Medium-Green"},     // 4: Medium green
  {3, 0xFDA0, "Medium-Gold"},      // 5: Medium gold
  {4, 0x07FF, "Large-Cyan"},       // 6: Large cyan
  {4, 0xF81F, "Large-Magenta"},    // 7: Large magenta
  {5, 0xFFFF, "XL-White"},         // 8: Extra large white
  {6, 0xFFE0, "Bold-Yellow"}       // 9: Bold yellow
};
#define PTY_FONT_COUNT 10
uint8_t PTY_FONT_INDEX = 0;  // Current PTY font style

// Font styles for RT (Radio Text)
struct RTFontStyle {
  uint8_t size;      // 1-8 (TFT_eSPI font size)
  uint16_t color;    // RGB565 color
  const char* name;  // Style name
};

const RTFontStyle RT_FONT_STYLES[] = {
  {1, 0xFFFF, "Compact-White"},    // 0: Compact white
  {1, 0xFFE0, "Compact-Yellow"},   // 1: Compact yellow
  {2, 0x07E0, "Small-Green"},      // 2: Small green
  {2, 0x07FF, "Small-Cyan"},       // 3: Small cyan
  {2, 0xFDA0, "Small-Gold"},       // 4: Small gold
  {3, 0xFFFF, "Medium-White"},     // 5: Medium white
  {3, 0xF800, "Medium-Red"},       // 6: Medium red
  {3, 0xF81F, "Medium-Magenta"},   // 7: Medium magenta
  {4, 0x07FF, "Large-Cyan"},       // 8: Large cyan
  {4, 0xFFE0, "Large-Yellow"}      // 9: Large yellow
};
#define RT_FONT_COUNT 10
uint8_t RT_FONT_INDEX = 0;  // Current RT font style

// Font styles for PI (Program Identification)
struct PIFontStyle {
  uint8_t size;      // 1-8 (TFT_eSPI font size)
  uint16_t color;    // RGB565 color
  const char* name;  // Style name
};

const PIFontStyle PI_FONT_STYLES[] = {
  {1, 0xFFFF, "Micro-White"},      // 0: Micro white
  {1, 0xFFE0, "Micro-Yellow"},     // 1: Micro yellow
  {2, 0x07E0, "Tiny-Green"},       // 2: Tiny green
  {2, 0x07FF, "Tiny-Cyan"},        // 3: Tiny cyan
  {2, 0xFDA0, "Tiny-Gold"},        // 4: Tiny gold
  {3, 0xF800, "Small-Red"},        // 5: Small red
  {3, 0xFFFF, "Small-White"},      // 6: Small white
  {3, 0xF81F, "Small-Magenta"},    // 7: Small magenta
  {4, 0x07FF, "Medium-Cyan"},      // 8: Medium cyan
  {4, 0xFFE0, "Medium-Yellow"}     // 9: Medium yellow
};
#define PI_FONT_COUNT 10
uint8_t PI_FONT_INDEX = 0;  // Current PI font style

// Font styles for RDS (Radio Data System indicator)
struct RDSFontStyle {
  uint8_t size;      // 1-8 (TFT_eSPI font size)
  uint16_t color;    // RGB565 color
  const char* name;  // Style name
};

const RDSFontStyle RDS_FONT_STYLES[] = {
  {1, 0xFFFF, "Badge-White"},      // 0: Badge white
  {1, 0x07FF, "Badge-Cyan"},       // 1: Badge cyan
  {1, 0x07E0, "Badge-Green"},      // 2: Badge green
  {2, 0xFFE0, "Label-Yellow"},     // 3: Label yellow
  {2, 0xF800, "Label-Red"},        // 4: Label red
  {2, 0x07FF, "Label-Cyan"},       // 5: Label cyan
  {2, 0xFDA0, "Label-Gold"},       // 6: Label gold
  {3, 0xFFFF, "Bold-White"},       // 7: Bold white
  {3, 0xF81F, "Bold-Magenta"},     // 8: Bold magenta
  {3, 0x07E0, "Bold-Green"}        // 9: Bold green
};
#define RDS_FONT_COUNT 10
uint8_t RDS_FONT_INDEX = 0;  // Current RDS font style

// Font styles for Frequency
struct FreqFontStyle {
  uint8_t size;
  uint16_t color;
  const char* name;
};

const FreqFontStyle FREQ_FONT_STYLES[] = {
  {4, 0x07E0, "Large-Green"},      {4, 0x07FF, "Large-Cyan"},       {5, 0xFFE0, "XL-Yellow"},
  {5, 0x07FF, "XL-Cyan"},          {6, 0xFFFF, "Bold-White"},       {6, 0xF800, "Bold-Red"},
  {7, 0x07E0, "Huge-Green"},       {7, 0xFFE0, "Huge-Yellow"},      {8, 0x07FF, "Max-Cyan"},
  {8, 0xFFFF, "Max-White"}
};
#define FREQ_FONT_COUNT 10
uint8_t FREQ_FONT_INDEX = 0;

// Font styles for Music/Artist
struct MusicFontStyle {
  uint8_t size;
  uint16_t color;
  const char* name;
};

const MusicFontStyle MUSIC_FONT_STYLES[] = {
  {1, 0xFFFF, "Tiny-White"},       {1, 0xFFE0, "Tiny-Yellow"},      {2, 0x07E0, "Small-Green"},
  {2, 0x07FF, "Small-Cyan"},       {2, 0xFDA0, "Small-Gold"},       {3, 0xFFE0, "Medium-Yellow"},
  {3, 0xF800, "Medium-Red"},       {3, 0x07FF, "Medium-Cyan"},      {4, 0xFFFF, "Large-White"},
  {4, 0xF81F, "Large-Magenta"}
};
#define MUSIC_FONT_COUNT 10
uint8_t MUSIC_FONT_INDEX = 0;

// Font styles for Time/Date/Weather
struct TimeFontStyle {
  uint8_t size;
  uint16_t color;
  const char* name;
};

const TimeFontStyle TIME_FONT_STYLES[] = {
  {1, 0xFFFF, "Micro-White"},      {1, 0x07FF, "Micro-Cyan"},       {2, 0xFFE0, "Small-Yellow"},
  {2, 0x07E0, "Small-Green"},      {2, 0xFDA0, "Small-Gold"},       {3, 0xFFFF, "Medium-White"},
  {3, 0xF800, "Medium-Red"},       {3, 0x07FF, "Medium-Cyan"},      {4, 0xFFE0, "Large-Yellow"},
  {4, 0xF81F, "Large-Magenta"}
};
#define TIME_FONT_COUNT 10
uint8_t TIME_FONT_INDEX = 0;

// Font styles for Song Title
struct SongFontStyle {
  uint8_t size;
  uint16_t color;
  const char* name;
};

const SongFontStyle SONG_FONT_STYLES[] = {
  {1, 0xFFFF, "Compact-White"},    {1, 0xFFE0, "Compact-Yellow"},   {2, 0x07E0, "Small-Green"},
  {2, 0x07FF, "Small-Cyan"},       {2, 0xFDA0, "Small-Gold"},       {3, 0xFFE0, "Medium-Yellow"},
  {3, 0xF800, "Medium-Red"},       {3, 0x07FF, "Medium-Cyan"},      {4, 0xFFFF, "Large-White"},
  {4, 0xF81F, "Large-Magenta"}
};
#define SONG_FONT_COUNT 10
uint8_t SONG_FONT_INDEX = 0;

// Font styles for Artist Name
struct ArtistFontStyle {
  uint8_t size;
  uint16_t color;
  const char* name;
};

const ArtistFontStyle ARTIST_FONT_STYLES[] = {
  {1, 0x07E0, "Tiny-Green"},       {1, 0x07FF, "Tiny-Cyan"},        {2, 0xFFE0, "Small-Yellow"},
  {2, 0xF800, "Small-Red"},        {2, 0xFDA0, "Small-Gold"},       {3, 0xFFFF, "Medium-White"},
  {3, 0xF81F, "Medium-Magenta"},   {3, 0x07E0, "Medium-Green"},     {4, 0x07FF, "Large-Cyan"},
  {4, 0xFFE0, "Large-Yellow"}
};
#define ARTIST_FONT_COUNT 10
uint8_t ARTIST_FONT_INDEX = 0;

// Font styles for Time (HH:MM:SS)
struct HourFontStyle {
  uint8_t size;
  uint16_t color;
  const char* name;
};

const HourFontStyle HOUR_FONT_STYLES[] = {
  {1, 0xFFFF, "Micro-White"},      {1, 0xFFE0, "Micro-Yellow"},     {2, 0x07E0, "Small-Green"},
  {2, 0x07FF, "Small-Cyan"},       {2, 0xFDA0, "Small-Gold"},       {3, 0xFFE0, "Medium-Yellow"},
  {3, 0xF800, "Medium-Red"},       {3, 0x07FF, "Medium-Cyan"},      {4, 0xFFFF, "Large-White"},
  {4, 0xF81F, "Large-Magenta"}
};
#define HOUR_FONT_COUNT 10
uint8_t HOUR_FONT_INDEX = 0;

// Font styles for Date (DD/MM/YYYY)
struct DateFontStyle {
  uint8_t size;
  uint16_t color;
  const char* name;
};

const DateFontStyle DATE_FONT_STYLES[] = {
  {1, 0x07E0, "Tiny-Green"},       {1, 0x07FF, "Tiny-Cyan"},        {2, 0xFFE0, "Small-Yellow"},
  {2, 0xF800, "Small-Red"},        {2, 0xFDA0, "Small-Gold"},       {3, 0xFFFF, "Medium-White"},
  {3, 0xF81F, "Medium-Magenta"},   {3, 0x07E0, "Medium-Green"},     {4, 0x07FF, "Large-Cyan"},
  {4, 0xFFE0, "Large-Yellow"}
};
#define DATE_FONT_COUNT 10
uint8_t DATE_FONT_INDEX = 0;

// Font styles for Weather
struct WeatherFontStyle {
  uint8_t size;
  uint16_t color;
  const char* name;
};

const WeatherFontStyle WEATHER_FONT_STYLES[] = {
  {1, 0xFFFF, "Badge-White"},      {1, 0x07FF, "Badge-Cyan"},       {1, 0x07E0, "Badge-Green"},
  {2, 0xFFE0, "Label-Yellow"},     {2, 0xF800, "Label-Red"},        {2, 0x07FF, "Label-Cyan"},
  {2, 0xFDA0, "Label-Gold"},       {3, 0xFFFF, "Bold-White"},       {3, 0xF81F, "Bold-Magenta"},
  {3, 0x07E0, "Bold-Green"}
};
#define WEATHER_FONT_COUNT 10
uint8_t WEATHER_FONT_INDEX = 0;

// Font styles for Station Editor
struct EditorFontStyle {
  uint8_t size;
  uint16_t color;
  const char* name;
};

const EditorFontStyle EDITOR_FONT_STYLES[] = {
  {2, 0xFFFF, "Label-White"},      {2, 0xFFE0, "Label-Yellow"},     {2, 0x07E0, "Label-Green"},
  {2, 0x07FF, "Label-Cyan"},       {3, 0xFFFF, "Field-White"},      {3, 0xFDA0, "Field-Gold"},
  {3, 0xF800, "Field-Red"},        {3, 0x07FF, "Field-Cyan"},       {4, 0xFFE0, "Edit-Yellow"},
  {4, 0xF81F, "Edit-Magenta"}
};
#define EDITOR_FONT_COUNT 10
uint8_t EDITOR_FONT_INDEX = 0;

// Font styles for Editor Labels
struct EditorLabelFontStyle {
  uint8_t size;
  uint16_t color;
  const char* name;
};

const EditorLabelFontStyle EDITOR_LABEL_FONT_STYLES[] = {
  {1, 0xFFFF, "Badge-White"},      {1, 0x07FF, "Badge-Cyan"},       {2, 0xFFE0, "Small-Yellow"},
  {2, 0x07E0, "Small-Green"},      {2, 0xFDA0, "Small-Gold"},       {2, 0xF800, "Small-Red"},
  {3, 0xFFFF, "Medium-White"},     {3, 0xF81F, "Medium-Magenta"},   {3, 0x07FF, "Medium-Cyan"},
  {4, 0xFFE0, "Large-Yellow"}
};
#define EDITOR_LABEL_FONT_COUNT 10
uint8_t EDITOR_LABEL_FONT_INDEX = 0;

// Font styles for Editor Values
struct EditorValueFontStyle {
  uint8_t size;
  uint16_t color;
  const char* name;
};

const EditorValueFontStyle EDITOR_VALUE_FONT_STYLES[] = {
  {2, 0x07E0, "Highlight-Green"},  {2, 0x07FF, "Highlight-Cyan"},  {2, 0xFFE0, "Highlight-Yellow"},
  {3, 0xFFFF, "Input-White"},      {3, 0xFDA0, "Input-Gold"},       {3, 0xF800, "Input-Red"},
  {4, 0x07E0, "Active-Green"},     {4, 0x07FF, "Active-Cyan"},      {4, 0xFFE0, "Active-Yellow"},
  {4, 0xF81F, "Active-Magenta"}
};
#define EDITOR_VALUE_FONT_COUNT 10
uint8_t EDITOR_VALUE_FONT_INDEX = 0;

// Font styles for Enable/Disable Toggle
struct ToggleFontStyle {
  uint8_t size;
  uint16_t color_enabled;
  uint16_t color_disabled;
  const char* name;
};

const ToggleFontStyle TOGGLE_FONT_STYLES[] = {
  {1, 0x07E0, 0x7BEF, "Badge"},     {2, 0x07FF, 0x8410, "Small"},     {2, 0x07E0, 0x4A49, "Medium"},
  {3, 0xFFFF, 0xBDF7, "Large"},     {3, 0x07FF, 0x39E7, "Bold"},      {3, 0xFFE0, 0x7BEF, "Yellow"},
  {2, 0xF800, 0x7BEF, "Red-Gray"},  {2, 0x07E0, 0x8410, "Green-Gray"}, {3, 0xFFE0, 0x4A49, "Amber-Gray"},
  {3, 0xF81F, 0x8410, "Magenta-Gray"}
};
#define TOGGLE_FONT_COUNT 10
uint8_t TOGGLE_FONT_INDEX = 0;

// Master RDS font control
struct RDSFontControl {
  uint8_t ps_style;
  uint8_t pty_style;
  uint8_t rt_style;
  uint8_t pi_style;
  uint8_t rds_style;
  uint8_t freq_style;
  uint8_t song_style;
  uint8_t artist_style;
  uint8_t hour_style;
  uint8_t date_style;
  uint8_t weather_style;
  uint8_t editor_style;
  uint8_t editor_label_style;
  uint8_t editor_value_style;
};

RDSFontControl rds_fonts = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Toggle states for RDS elements
struct RDSToggle {
  bool ps_enabled;
  bool pty_enabled;
  bool rt_enabled;
  bool pi_enabled;
  bool rds_enabled;
  bool freq_enabled;
  bool song_enabled;
  bool artist_enabled;
  bool time_enabled;
  bool date_enabled;
  bool weather_enabled;
};

RDSToggle rds_toggles = {true, true, true, true, true, true, true, true, true, true, true};

// Runtime theme palette — set by applyTheme()
uint16_t UI_HEADER_BG    = 0x0820;
uint16_t UI_FREQ_COLOR   = 0x07E0;
uint16_t UI_LEVEL_COLOR  = 0xFD20;
uint16_t UI_LABEL_COLOR  = 0x07FF;
uint16_t UI_DIM_COLOR    = 0x7BEF;
uint16_t UI_BORDER_COLOR = 0x07FF;
uint16_t UI_BADGE_FM     = 0x03E0;
uint16_t UI_BADGE_AM     = 0x001F;
#define ROTARY_PIN_A    34
#define ROTARY_PIN_B    36
#define ROTARY_BUTTON   39
#define PIN_POT         35
#define PWRBUTTON       4
#define BWBUTTON        25
#define MODEBUTTON      26
#define CONTRASTPIN     2
#define STANDBYLED      19
#define SMETERPIN       27
//#define ARS       // uncomment for BGR type display (ARS version)

#ifdef ARS
#define VERSION         "v1.16ARS"
#include "TFT_Colors.h"
#else
#define VERSION         "v1.16"
#endif

#include "src/estacoes.h"
#include "src/ui_language.h"

#ifdef ARS
TFT_eSPI tft = TFT_eSPI(320, 240);
#else
TFT_eSPI tft = TFT_eSPI(240, 320);
#endif

bool BWreset;
bool change2;
bool cleanup;
bool direction;
bool dropout;
bool fullsearchrds;
bool LowLevelInit = false;
bool menu;
bool menuopen = false;
bool power = true;
bool RDSstatusold;
bool screenmute = false;
bool seek;
bool setupmode;
bool SQ;
bool Stereostatusold;
bool StereoToggle = true;
bool store;
bool tuned;
bool tunemode = false;
bool USBstatus = false;
bool XDRMute;
byte band;
byte BWset;
byte ContrastSet;
byte displayflip;
byte EQset;
byte freqoldcount;
byte iMSEQ;
byte iMSset;
byte LowLevelSet;
byte optenc;
byte rotarymode;
byte SNR;
byte SNRold;
byte SStatusoldcount;
byte stepsize;
byte TEF;
byte languageSet;
uint16_t themeSet = 0;
byte menuPage = 0;
byte maxMenuPages = 1;
char buff[16];
char programServicePrevious[9];
char programTypePrevious[17];
char radioIdPrevious[5];
char radioTextPrevious[65];
int AGC;
int BWOld;
int charWidth = tft.textWidth("AA");
int ConverterSet;
int DeEmphasis;
int ForceMono;
int freqold;
int HighCutLevel;
int HighCutOffset;
int HighEdgeSet;
int LevelOffset;
int LowEdgeSet;
int lowsignaltimer;
int menuoption = 30;
int realMenuoption = 30;
int MStatusold;
int OStatusold;
int peakholdold;
int peakholdtimer;
int rotary;
int scanner_filter;
int Sqstatusold;
int Squelch;
int Squelchold;
int SStatusold;
int StereoLevel;
int Stereostatus;
int VolSet;
int volume;
int XDRBWset;
int XDRBWsetold;
int xPos = 6;
int yPos = 2;
int psXPos = 38;
int psYPos = 192;
int16_t OStatus;
int16_t SAvg;
int16_t SAvg2;
int16_t SStatus;
String ContrastString;
String ConverterString;
String HighCutLevelString;
String HighCutOffsetString;
String HighEdgeString;
String LevelOffsetString;
String LowEdgeString;
String LowLevelString;
String PIold;
String PSold;
String PTYold;
String rds_clock;
String rds_clockold;
String RTold;
String StereoLevelString;
String VolString;
String XDRGTKRDS;
String XDRGTKRDSold;
uint16_t BW;
uint16_t MStatus;
uint16_t USN;
uint16_t WAM;
uint8_t buff_pos = 0;
uint8_t RDSstatus;
String customPS = "";
String customRT = "";
String customPTY = "";
String customPI = "";
uint32_t lastCustomFreq = 0;
int ptyStationIndex = 0;
int ptyScrollTop = 0;
int ptyEditCode = 0;
int psStationIndex = 0;
int psScrollTop = 0;
int psEditIndex = 0;
const char** psEditList = nullptr;
int psEditCount = 0;
int rtStationIndex = 0;
int rtScrollTop = 0;
int rtEditIndex = 0;
int rtEditCount = 0;
int piEditCode = 0;
int musicaEditIndex = 0;
int musicaEditCount = 0;
int horaEditVal = 0;
int minutoEditVal = 0;
int diaEditVal = 1;
int mesEditVal = 1;
int tempoEditIdx = 0;
int tempNumEdit = 25;
int rdsStationIndex = 0;
int rdsScrollTop = 0;

// Weather conditions
const char* const TEMPO_LIST[] = {
  "Sunny", "Partly Cloudy", "Cloudy", "Rainy", "Stormy", "Windy", "Foggy"
};
const int TEMPO_COUNT = 7;

unsigned int change;
unsigned int freq_scan;
unsigned int frequency;
unsigned int frequency_AM;
unsigned int frequencyold;
unsigned int scanner_end;
unsigned int scanner_start;
unsigned int scanner_step;
unsigned long peakholdmillis;
unsigned long rtticker;
unsigned long psTicker;

TEF6686 radio;
TFT_eSprite sprite = TFT_eSprite(&tft);
TFT_eSprite psSprite = TFT_eSprite(&tft);

void setup() {
  setupmode = true;
  EEPROM.begin(400);
  if (EEPROM.readByte(41) != 15) {
    EEPROM.writeByte(2, 0);
    EEPROM.writeByte(3, 0);
    EEPROM.writeUInt(0, 10000);
    EEPROM.writeInt(4, 0);
    EEPROM.writeInt(8, 0);
    EEPROM.writeInt(12, 87);
    EEPROM.writeInt(16, 108);
    EEPROM.writeInt(20, 50);
    EEPROM.writeInt(24, 0);
    EEPROM.writeInt(28, 0);
    EEPROM.writeInt(32, 70);
    EEPROM.writeInt(36, 0);
    EEPROM.writeByte(41, 15);
    EEPROM.writeByte(42, 0);
    EEPROM.writeByte(43, 20);
    EEPROM.writeByte(44, 1);
    EEPROM.writeByte(45, 1);
    EEPROM.writeByte(46, 0);
    EEPROM.writeUInt(47, 828);
    EEPROM.writeByte(52, 0);
    EEPROM.writeByte(53, 0);
    EEPROM.writeByte(54, 102);
    EEPROM.writeByte(55, 0);
    EEPROM.writeByte(56, 1);
    EEPROM.commit();
  }
  frequency = EEPROM.readUInt(0);
  VolSet = EEPROM.readInt(4);
  ConverterSet = EEPROM.readInt(8);
  LowEdgeSet = EEPROM.readInt(12);
  HighEdgeSet = EEPROM.readInt(16);
  ContrastSet = EEPROM.readInt(20);
  LevelOffset = EEPROM.readInt(24);
  StereoLevel = EEPROM.readInt(28);
  HighCutLevel = EEPROM.readInt(32);
  HighCutOffset = EEPROM.readInt(36);
  stepsize = EEPROM.readByte(42);
  LowLevelSet = EEPROM.readByte(43);
  iMSset = EEPROM.readByte(44);
  EQset = EEPROM.readByte(45);
  band = EEPROM.readByte(46);
  frequency_AM = EEPROM.readUInt(47);
  rotarymode = EEPROM.readByte(52);
  displayflip = EEPROM.readByte(53);
  TEF = EEPROM.readByte(54);
  optenc = EEPROM.readByte(55);
  languageSet = EEPROM.readByte(56);
  if (languageSet < 1 || languageSet > 9) languageSet = 1;
  themeSet = EEPROM.readUShort(241);
  if (themeSet > 2600) themeSet = 0;
  applyTheme(themeSet);
  EEPROM.commit();
  setPTYLanguage(languageSet);
  btStop();
  Serial.begin(115200);

  if (iMSset == 1 && EQset == 1) {
    iMSEQ = 2;
  }
  if (iMSset == 0 && EQset == 1) {
    iMSEQ = 3;
  }
  if (iMSset == 1 && EQset == 0) {
    iMSEQ = 4;
  }
  if (iMSset == 0 && EQset == 0) {
    iMSEQ = 1;
  }

  tft.init();

  if (displayflip == 0) {
#ifdef ARS
    tft.setRotation(0);
#else
    tft.setRotation(3);
#endif
  } else {
#ifdef ARS
    tft.setRotation(2);
#else
    tft.setRotation(1);
#endif
  }

  TEF = EEPROM.readByte(54);

  if (TEF != 101 && TEF != 102 && TEF != 205) SetTunerPatch();

  radio.init(TEF);
  carregarEstacoes();
  loadCustomRDSEnabled();
  loadCustomPICodes();
  loadCustomPTYCodes();
  uint16_t device;
  uint16_t hw;
  uint16_t sw;
  radio.getIdentification(device, hw, sw);
  if (TEF != (highByte(hw) * 100 + highByte(sw))) SetTunerPatch();

  analogWrite(CONTRASTPIN, ContrastSet * 2 + 27);
  analogWrite(SMETERPIN, 0);

  radio.getIdentification(device, hw, sw);
  uint8_t version = highByte(hw) * 100 + highByte(sw);

  pinMode(MODEBUTTON, INPUT);
  pinMode(BWBUTTON, INPUT);
  pinMode(ROTARY_BUTTON, INPUT);
  pinMode(ROTARY_PIN_A, INPUT);
  pinMode(ROTARY_PIN_B, INPUT);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A), read_encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B), read_encoder, CHANGE);

  if (digitalRead(BWBUTTON) == LOW) {
    if (rotarymode == 0) rotarymode = 1; else rotarymode = 0;
    EEPROM.writeByte(52, rotarymode);
    EEPROM.commit();
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString(getUIString(UI_ROTARY_CHANGED,  languageSet), 150, 70, 4);
    tft.drawCentreString(getUIString(UI_PLEASE_RELEASE,  languageSet), 150, 100, 4);
    while (digitalRead(BWBUTTON) == LOW) delay(50);
  }

  if (digitalRead(MODEBUTTON) == LOW) {
    if (displayflip == 0) {
      displayflip = 1;
      tft.setRotation(1);
    } else {
      displayflip = 0;
      tft.setRotation(3);
    }
    EEPROM.writeByte(53, displayflip);
    EEPROM.commit();
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString(getUIString(UI_SCREEN_FLIPPED,  languageSet), 150, 70, 4);
    tft.drawCentreString(getUIString(UI_PLEASE_RELEASE,  languageSet), 150, 100, 4);
    while (digitalRead(MODEBUTTON) == LOW) delay(50);
  }

  if (digitalRead(PWRBUTTON) == LOW) {
    analogWrite(SMETERPIN, 511);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString("Calibrate analog meter", 150, 70, 4);
    tft.drawCentreString("Release button when ready", 150, 100, 4);
    while (digitalRead(PWRBUTTON) == LOW) delay(50);
    analogWrite(SMETERPIN, 0);
  }

  if (digitalRead(ROTARY_BUTTON) == LOW) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    if (optenc == 0) {
      optenc = 1;
      tft.drawCentreString("encoder set to optical", 150, 70, 4);
    } else {
      optenc = 0;
      tft.drawCentreString("encoder set to standard", 150, 70, 4);
    }
    EEPROM.writeByte(55, optenc);
    EEPROM.commit();
    tft.drawCentreString(getUIString(UI_PLEASE_RELEASE, languageSet), 150, 100, 4);
    while (digitalRead(ROTARY_BUTTON) == LOW) delay(50);
  }

  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);
  tft.pushImage (0, 99, 211, 140, pe5pvblogo);
  tft.pushImage (239, 200, 80, 30, nxplogo);
  tft.setTextColor(UI_LABEL_COLOR);
  tft.drawCentreString(getUIString(UI_FM_AM_RECEIVER, languageSet), 160, 10, 4);
  tft.setTextColor(UI_FREQ_COLOR);
  tft.drawCentreString("Software " + String(VERSION), 160, 30, 2);
  tft.setTextColor(UI_DIM_COLOR);
  tft.drawString("Lithio", 260, 180, 2);

  if (lowByte(device) == 14) {
    fullsearchrds = false;
    tft.drawString("Tuner: TEF6686 Lithio (" + String(version) + ")", 80, 60, 2);
  } else if (lowByte(device) == 1) {
    fullsearchrds = true;
    tft.drawString("Tuner: TEF6687 Lithio FMSI (" + String(version) + ")", 80, 60, 2);
  } else if (lowByte(device) == 9) {
    fullsearchrds = false;
    tft.drawString("Tuner: TEF6688 Lithio DR (" + String(version) + ")", 80, 60, 2);
  } else if (lowByte(device) == 3) {
    fullsearchrds = true;
    tft.drawString("Tuner: TEF6689 Lithio FMSI DR (" + String(version) + ")", 80, 60, 2);
  } else {
    tft.setTextColor(TFT_RED);
    tft.drawString("Tuner: !None!", 80, 60, 2);
    while (true);
    for (;;);
  }
  tft.drawString("Patch: v" + String(TEF), 80, 75, 2);
  delay(1500);

  radio.setVolume(VolSet);
  radio.setOffset(LevelOffset);
  radio.setStereoLevel(StereoLevel);
  radio.setHighCutLevel(HighCutLevel);
  radio.setHighCutOffset(HighCutOffset);
  radio.clearRDS(fullsearchrds);
  radio.setMute();
  LowLevelInit = true;

  if (ConverterSet >= 200) {
    Wire.beginTransmission(0x12);
    Wire.write(ConverterSet >> 8);
    Wire.write(ConverterSet & (0xFF));
    Wire.endTransmission();
  }

  SelectBand();
  ShowSignalLevel();
  ShowBW();
  setupmode = false;
  sprite.createSprite(313, 18);
  psSprite.createSprite(170, 28);
}

void loop() {
  if (digitalRead(PWRBUTTON) == LOW && USBstatus == false) PWRButtonPress();

  if (power == true) {
    if (seek == true) Seek(direction);

    if ((SStatus / 10 > LowLevelSet) && LowLevelInit == false && menu == false && band == 0) {
      if (screenmute == false) {
        tft.setTextColor(UI_DIM_COLOR);
        tft.drawString("20", 20, 146, 1);
        tft.drawString("40", 50, 146, 1);
        tft.drawString("60", 80, 146, 1);
        tft.drawString("80", 110, 146, 1);
        tft.drawString("100", 134, 146, 1);
        tft.drawString("120", 164, 146, 1);
        tft.drawString("%", 196, 146, 1);
        tft.setTextColor(UI_LABEL_COLOR);
        tft.drawString("M", 6, 129, 2);
        tft.drawString("PI:", 216, 188, 2);
        tft.drawString("PS:", 6, 188, 2);
        tft.drawString("PTY:", 6, 161, 2);
        tft.drawLine(20, 143, 200, 143, TFT_DARKGREY);
      }
      LowLevelInit = true;
    }

    if ((SStatus / 10 <= LowLevelSet) && band == 0) {
      if (LowLevelInit == true && menu == false) {
        if (screenmute == false) {
          tft.fillRect(20, 132, 12, 8, TFT_GREYOUT);
          tft.fillRect(34, 132, 12, 8, TFT_GREYOUT);
          tft.fillRect(48, 132, 12, 8, TFT_GREYOUT);
          tft.fillRect(62, 132, 12, 8, TFT_GREYOUT);
          tft.fillRect(76, 132, 12, 8, TFT_GREYOUT);
          tft.fillRect(90, 132, 12, 8, TFT_GREYOUT);
          tft.fillRect(104, 132, 12, 8, TFT_GREYOUT);
          tft.fillRect(118, 132, 12, 8, TFT_GREYOUT);
          tft.fillRect(132, 132, 12, 8, TFT_GREYOUT);
          tft.fillRect(146, 132, 12, 8, TFT_GREYOUT);
          tft.fillRect(160, 132, 12, 8, TFT_GREYOUT);
          tft.fillRect(174, 132, 12, 8, TFT_GREYOUT);
          tft.fillRect(188, 132, 12, 8, TFT_GREYOUT);
          tft.setTextColor(TFT_GREYOUT);
          tft.drawString("20", 20, 146, 1);
          tft.drawString("40", 50, 146, 1);
          tft.drawString("60", 80, 146, 1);
          tft.drawString("80", 110, 146, 1);
          tft.drawString("100", 134, 146, 1);
          tft.drawString("120", 164, 146, 1);
          tft.drawString("%", 196, 146, 1);
          tft.setTextColor(UI_LABEL_COLOR);
          tft.drawString("M", 6, 129, 2);
          tft.drawString("PI:", 216, 188, 2);
          tft.drawString("PS:", 6, 188, 2);
          tft.drawString("PTY:", 6, 161, 2);
          tft.drawLine(20, 143, 200, 143, TFT_GREYOUT);
          tft.drawBitmap(110, 1, RDSLogo, 67, 22, TFT_GREYOUT);
        }
        LowLevelInit = false;
      }

      if (millis() >= lowsignaltimer + 300) {
        lowsignaltimer = millis();
        if (band == 0) radio.getStatus(SStatus, USN, WAM, OStatus, BW, MStatus); else radio.getStatus_AM(SStatus, USN, WAM, OStatus, BW, MStatus);
        if (screenmute == true) readRds();
        if (menu == false) doSquelch();
      }

    } else {
      if (band == 0) radio.getStatus(SStatus, USN, WAM, OStatus, BW, MStatus); else radio.getStatus_AM(SStatus, USN, WAM, OStatus, BW, MStatus);
      if (menu == false) {
        doSquelch();
        readRds();
        if (screenmute == false) {
          ShowModLevel();
        }
      }
    }

    if (menu == false) {
      if (screenmute == false) {
        if (band == 0) {
          showPI();
          showPTY();
          showCT();
          showPS();
          showRadioText();
          showPS();
          ShowStereoStatus();
        }
        ShowOffset();
        ShowSignalLevel();
        ShowBW();
      }
    }

    XDRGTKRoutine();

    if (menu == true && menuopen == true && menuoption == 110)
    {
      if (band == 0) radio.getStatus(SStatus, USN, WAM, OStatus, BW, MStatus); else radio.getStatus_AM(SStatus, USN, WAM, OStatus, BW, MStatus);
      if (millis() >= lowsignaltimer + 500 || change2 == true) {
        lowsignaltimer = millis();
        change2 = false;
        if (SStatus > SStatusold || SStatus < SStatusold)
        {
          String count = String(SStatus / 10, DEC);
          if (screenmute == false) {
            tft.setTextColor(TFT_BLACK);
            if (SStatusold >= 0) {
              if (SStatusoldcount <= 1) tft.setCursor (100, 140);
              if (SStatusoldcount == 2) tft.setCursor (73, 140);
              if (SStatusoldcount >= 3) tft.setCursor (46, 140);
            } else {
              if (SStatusoldcount <= 1) tft.setCursor (100, 140);
              if (SStatusoldcount == 2) tft.setCursor (83, 140);
              if (SStatusoldcount >= 3) tft.setCursor (56, 140);
            }
            tft.setTextFont(6);
            tft.print(SStatusold / 10);
            tft.print(".");
            if (SStatusold < 0)
            {
              String negative = String (SStatusold % 10, DEC);
              if (SStatusold % 10 == 0) tft.print("0");  else tft.print(negative[1]);
            } else {
              tft.print(SStatusold % 10);
            }

            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            if (SStatus >= 0) {
              if (count.length() == 1) tft.setCursor (100, 140);
              if (count.length() == 2) tft.setCursor (73, 140);
              if (count.length() == 3) tft.setCursor (46, 140);
            } else {
              if (count.length() == 1) tft.setCursor (100, 140);
              if (count.length() == 2) tft.setCursor (83, 140);
              if (count.length() >= 3) tft.setCursor (56, 140);
            }
            tft.setTextFont(6);
            tft.print(SStatus / 10);
            tft.print(".");
            if (SStatus < 0)
            {
              String negative = String (SStatus % 10, DEC);
              if (SStatus % 10 == 0) tft.print("0"); else tft.print(negative[1]);
            } else {
              tft.print(SStatus % 10);
            }
            SStatusold = SStatus;
            SStatusoldcount = count.length();
          }
        }
      }
    }

    if (rotary == -1) KeyUp();
    if (rotary == 1) KeyDown();

    if (digitalRead(ROTARY_BUTTON) == LOW) ButtonPress();
    if (digitalRead(MODEBUTTON) == LOW && screenmute == false) ModeButtonPress();
    if (digitalRead(BWBUTTON) == LOW && screenmute == false) BWButtonPress();

    if (store == true) change++;

    if (change > 200 && store == true) {
      detachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A));
      detachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B));
      EEPROM.writeUInt(0, radio.getFrequency());
      EEPROM.writeUInt(47, radio.getFrequency_AM());
      EEPROM.writeByte(46, band);
      EEPROM.commit();
      store = false;
      attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A), read_encoder, CHANGE);
      attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B), read_encoder, CHANGE);
    }
  }
}

void PWRButtonPress() {
  if (menu == false) {
    unsigned long counterold = millis();
    unsigned long counter = millis();
    while (digitalRead(PWRBUTTON) == LOW && counter - counterold <= 1000) counter = millis();

    if (counter - counterold < 1000) {
      if (power == false) {
        ESP.restart();
      } else {
        if (band == 0) band = 1; else band = 0;
        StoreFrequency();
        SelectBand();
      }
    } else {
      if (power == false) {
        ESP.restart();
      } else {
        power = false;
        analogWrite(SMETERPIN, 0);
        analogWrite(CONTRASTPIN, 0);
        pinMode (STANDBYLED, OUTPUT);
        digitalWrite(STANDBYLED, LOW);
        StoreFrequency();
        radio.power(1);
      }
    }
    while (digitalRead(PWRBUTTON) == LOW) delay(50);
    delay(100);
  }
}

void StoreFrequency() {
  EEPROM.writeUInt(0, radio.getFrequency());
  EEPROM.writeUInt(47, radio.getFrequency_AM());
  EEPROM.writeByte(46, band);
  EEPROM.commit();
}

void saveCustomRDSEnabled() {
  size_t count = totalEstacoes();
  for (int b = 0; b < 16; b++) {
    uint8_t mask = 0;
    for (int bit = 0; bit < 8; bit++) {
      size_t idx = b * 8 + bit;
      if (idx < count && getEstacao(idx).rds_ativo) mask |= (1 << bit);
    }
    EEPROM.writeByte(57 + b, mask);
  }
  EEPROM.commit();
}

void loadCustomRDSEnabled() {
  size_t count = totalEstacoes();
  for (int b = 0; b < 16; b++) {
    uint8_t mask = EEPROM.readByte(57 + b);
    for (int bit = 0; bit < 8; bit++) {
      size_t idx = b * 8 + bit;
      if (idx < count) getEstacao(idx).rds_ativo = (mask >> bit) & 1;
    }
  }
}

void saveCustomPICodes() {
  size_t count = totalEstacoes();
  for (size_t i = 0; i < count; i++) {
    EEPROM.writeUShort(73 + i * 2, getEstacao(i).pi_code);
  }
  EEPROM.commit();
}

void loadCustomPICodes() {
  size_t count = totalEstacoes();
  for (size_t i = 0; i < count; i++) {
    uint16_t val = EEPROM.readUShort(73 + i * 2);
    if (val != 0xFFFF) getEstacao(i).pi_code = val;
  }
}

void saveCustomPTYCodes() {
  size_t count = totalEstacoes();
  for (size_t i = 0; i < count; i++) {
    int8_t code = getEstacao(i).pty_code;
    EEPROM.writeByte(243 + i, (code >= 0 && code <= 31) ? (uint8_t)code : 0xFF);
  }
  EEPROM.commit();
}

void loadCustomPTYCodes() {
  size_t count = totalEstacoes();
  for (size_t i = 0; i < count; i++) {
    uint8_t val = EEPROM.readByte(243 + i);
    if (val <= 31) getEstacao(i).pty_code = (int8_t)val;
  }
}

void habilitarTodasRDS() {
  size_t count = totalEstacoes();
  for (size_t i = 0; i < count; i++) getEstacao(i).rds_ativo = true;
  saveCustomRDSEnabled();
  lastCustomFreq = 0;
}

void SelectBand() {
  if (band == 1) {
    seek = false;
    tunemode = false;
    BWreset = true;
    BWset = 2;
    if (radio.getFrequency_AM() > 0) frequency_AM = radio.getFrequency_AM();
    radio.setFrequency_AM(frequency_AM);
    freqold = frequency_AM;
    doBW;
    radio.getStatus_AM(SStatus, USN, WAM, OStatus, BW, MStatus);
    if (screenmute == false) BuildDisplay();
    tft.setTextColor(UI_LABEL_COLOR);
    tft.drawString("PI:", 216, 188, 2);
    tft.drawString("PS:", 6, 188, 2);
    tft.drawString("PTY:", 6, 161, 2);
    tft.drawBitmap(110, 1, RDSLogo, 67, 22, TFT_GREYOUT);
    tft.drawRoundRect(249, 46, 30, 18, 4, TFT_GREYOUT);
    tft.setTextColor(TFT_GREYOUT);
    tft.drawCentreString("iMS", 265, 48, 2);
    tft.drawRoundRect(287, 46, 30, 18, 4, TFT_GREYOUT);
    tft.setTextColor(TFT_GREYOUT);
    tft.drawCentreString("EQ", 303, 48, 2);
  } else {
    LowLevelInit == false;
    BWreset = true;
    BWset = 0;
    radio.power(0);
    delay(50);
    radio.setFrequency(frequency, LowEdgeSet, HighEdgeSet, fullsearchrds);
    freqold = frequency_AM;
    doBW;
    radio.getStatus(SStatus, USN, WAM, OStatus, BW, MStatus);
    if (screenmute == false) BuildDisplay();
  }
}

void BWButtonPress() {
  if (menu == false) {
    seek = false;
    unsigned long counterold = millis();
    unsigned long counter = millis();
    while (digitalRead(BWBUTTON) == LOW && counter - counterold <= 1000) counter = millis();

    if (counter - counterold < 1000) {
      BWset++;
      doBW();
    } else {
      doStereoToggle();
    }
  }
  while (digitalRead(BWBUTTON) == LOW) delay(50);
  delay(100);
}

void doStereoToggle() {
  if (StereoToggle == true) {
    if (screenmute == false) {
      tft.drawCircle(81, 12, 10, UI_HEADER_BG);
      tft.drawCircle(81, 12, 9, UI_HEADER_BG);
      tft.drawCircle(91, 12, 10, UI_HEADER_BG);
      tft.drawCircle(91, 12, 9, UI_HEADER_BG);
      tft.drawCircle(86, 12, 10, TFT_SKYBLUE);
      tft.drawCircle(86, 12, 9, TFT_SKYBLUE);
    }
    radio.setMono(2);
    StereoToggle = false;
  } else {
    if (screenmute == false) {
      tft.drawCircle(86, 12, 10, UI_HEADER_BG);
      tft.drawCircle(86, 12, 9, UI_HEADER_BG);
    }
    radio.setMono(0);
    Stereostatusold = false;
    StereoToggle = true;
  }
}

void ModeButtonPress() {
  if (menu == false) {
    seek = false;
    unsigned long counterold = millis();
    unsigned long counter = millis();
    while (digitalRead(MODEBUTTON) == LOW && counter - counterold <= 1000) counter = millis();

    if (counter - counterold <= 1000) {
      doTuneMode();
    } else {
      if (USBstatus == true) {
        ShowFreq(1);
        tft.setTextFont(4);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setCursor (70, 60);
        tft.print("NOT POSSIBLE");
        delay(1000);
        tft.setTextFont(4);
        tft.setTextColor(TFT_BLACK);
        tft.setCursor (70, 60);
        tft.print("NOT POSSIBLE");
        ShowFreq(0);
      } else {
        if (menu == false) {
          menuPage = 0;
          menuoption = 30;
          BuildMenu();
          menu = true;
        }
      }
    }
  } else {
    OStatusold = 1000;
    Stereostatusold = false;
    SStatusold = 2000;
    BWOld = 0;
    radio.clearRDS(fullsearchrds);
    RDSstatus = 0;
    BuildDisplay();
    ShowSignalLevel();
    ShowBW();
    menu = false;
    menuopen = false;
    LowLevelInit = true;
    EEPROM.writeInt(4, VolSet);
    EEPROM.writeInt(8, ConverterSet);
    EEPROM.writeInt(12, LowEdgeSet);
    EEPROM.writeInt(16, HighEdgeSet);
    EEPROM.writeInt(20, ContrastSet);
    EEPROM.writeInt(24, LevelOffset);
    EEPROM.writeInt(28, StereoLevel);
    EEPROM.writeInt(32, HighCutLevel);
    EEPROM.writeInt(36, HighCutOffset);
    EEPROM.writeByte(43, LowLevelSet);
    EEPROM.writeByte(56, languageSet);
    EEPROM.writeUShort(241, themeSet);
    EEPROM.commit();
  }
  while (digitalRead(MODEBUTTON) == LOW) delay(50);
  delay(100);
}

void ShowStepSize() {
  tft.fillRect(224, 25, 15, 4, TFT_GREYOUT);
  tft.fillRect(193, 25, 15, 4, TFT_GREYOUT);
  if (band == 0) tft.fillRect(148, 25, 15, 4, TFT_GREYOUT); else tft.fillRect(162, 25, 15, 4, TFT_GREYOUT);
  if (stepsize == 1) tft.fillRect(224, 25, 15, 4, TFT_GREEN);
  if (stepsize == 2) tft.fillRect(193, 25, 15, 4, TFT_GREEN);
  if (stepsize == 3) {
    if (band == 0) tft.fillRect(148, 25, 15, 4, TFT_GREEN); else tft.fillRect(162, 25, 15, 4, TFT_GREEN);
  }
}

void RoundStep() {
  if (band == 0) {
    int freq = radio.getFrequency();
    if (freq % 10 < 3) {
      radio.setFrequency(freq - (freq % 10 - 5) - 5, LowEdgeSet, HighEdgeSet, fullsearchrds);
    }
    else if (freq % 10 > 2 && freq % 10 < 8) {
      radio.setFrequency(freq - (freq % 10 - 5) , LowEdgeSet, HighEdgeSet, fullsearchrds);
    }
    else if (freq % 10 > 7) {
      radio.setFrequency(freq - (freq % 10 - 5) + 5, LowEdgeSet, HighEdgeSet, fullsearchrds);
    }
  } else {
    int freq = radio.getFrequency_AM();
    if (freq < 2000) radio.setFrequency_AM((freq / 9) * 9); else radio.setFrequency_AM((freq / 5) * 5);
  }
  while (digitalRead(ROTARY_BUTTON) == LOW) delay(50);

  if (band == 0) EEPROM.writeUInt(0, radio.getFrequency()); else EEPROM.writeUInt(47, radio.getFrequency_AM());
  EEPROM.commit();
}

void ButtonPress() {
  if (menu == false) {
    seek = false;
    unsigned long counterold = millis();
    unsigned long counter = millis();
    while (digitalRead(ROTARY_BUTTON) == LOW && counter - counterold <= 1000) counter = millis();

    if (counter - counterold < 1000) {
      if (tunemode == false) {
        stepsize++;
        if (stepsize > 3) stepsize = 0;

        if (screenmute == false) ShowStepSize();

        EEPROM.writeByte(42, stepsize);
        EEPROM.commit();
        if (stepsize == 0) {
          RoundStep();
          ShowFreq(0);
        }
      }
    } else {
      if (iMSEQ == 0) iMSEQ = 1;

      if (iMSEQ == 4) {
        iMSset = 0;
        EQset = 0;
        updateiMS();
        updateEQ();
        iMSEQ = 0;
      }
      if (iMSEQ == 3) {
        iMSset = 1;
        EQset = 0;
        updateiMS();
        updateEQ();
        iMSEQ = 4;
      }
      if (iMSEQ == 2) {
        iMSset = 0;
        EQset = 1;
        updateiMS();
        updateEQ();
        iMSEQ = 3;
      }
      if (iMSEQ == 1) {
        iMSset = 1;
        EQset = 1;
        updateiMS();
        updateEQ();
        iMSEQ = 2;
      }
      EEPROM.writeByte(44, iMSset);
      EEPROM.writeByte(45, EQset);
      EEPROM.commit();
    }
  } else {
    if (menuPage == 6) {
      if (menuopen == false) {
        if (menuoption == 130) {
          menuPage = 2; menuoption = 30; BuildMenu();
        } else if (menuoption == 110) {
          getEstacao(ptyStationIndex).rds_ativo = !getEstacao(ptyStationIndex).rds_ativo;
          saveCustomRDSEnabled(); lastCustomFreq = 0; BuildMenu();
        } else if (menuoption == 150) {
          habilitarTodasRDS(); BuildMenu();
        } else if (menuoption == 90) {
          menuopen = true;
          piEditCode = getEstacao(ptyStationIndex).pi_code;
          tft.drawRoundRect(30, 40, 240, 160, 5, TFT_WHITE);
          tft.fillRoundRect(32, 42, 236, 156, 5, TFT_BLACK);
          tft.setTextColor(TFT_WHITE);
          String piLbl = getEstacao(ptyStationIndex).ps;
          if (piLbl.length() > 14) piLbl = piLbl.substring(0, 14);
          tft.drawCentreString(piLbl, 150, 55, 2);
          tft.drawCentreString(getUIString(UI_SET_PI, languageSet), 150, 75, 4);
          tft.setTextColor(TFT_YELLOW);
          String piHex = String((uint16_t)piEditCode, HEX); piHex.toUpperCase();
          while (piHex.length() < 4) piHex = "0" + piHex;
          tft.drawCentreString(piHex, 150, 115, 4);
        } else if (menuoption == 30) {
          menuopen = true;
          ptyEditCode = getEstacao(ptyStationIndex).pty_code;
          if (ptyEditCode < 0 || ptyEditCode > 31) ptyEditCode = 0;
          tft.drawRoundRect(30, 40, 240, 160, 5, TFT_WHITE);
          tft.fillRoundRect(32, 42, 236, 156, 5, TFT_BLACK);
          tft.setTextColor(TFT_WHITE);
          String psLbl = getEstacao(ptyStationIndex).ps;
          if (psLbl.length() > 14) psLbl = psLbl.substring(0, 14);
          tft.drawCentreString(psLbl, 150, 55, 2);
          tft.drawCentreString(getUIString(UI_SET_PTY, languageSet), 150, 75, 4);
          tft.setTextColor(TFT_YELLOW);
          tft.drawCentreString(String(ptyEditCode), 150, 105, 4);
          tft.drawCentreString(radio.getPTYText(ptyEditCode), 150, 140, 2);
        } else if (menuoption == 50) {
          menuopen = true;
          psEditCount = (int)totalEstacoes();
          psEditIndex = 0;
          String curPS = getEstacao(ptyStationIndex).ps;
          for (int i = 0; i < psEditCount; i++) {
            if (curPS == getEstacao(i).ps) { psEditIndex = i; break; }
          }
          tft.drawRoundRect(30, 40, 240, 160, 5, TFT_WHITE);
          tft.fillRoundRect(32, 42, 236, 156, 5, TFT_BLACK);
          tft.setTextColor(TFT_WHITE);
          uint32_t fp = getEstacao(ptyStationIndex).freq_khz;
          tft.drawCentreString(String(fp/1000)+"."+String((fp%1000)/100)+" MHz", 150, 55, 2);
          tft.drawCentreString(getUIString(UI_SET_PS, languageSet), 150, 75, 4);
          tft.setTextColor(TFT_YELLOW);
          if (psEditCount > 0)
            tft.drawCentreString(getEstacao(psEditIndex).ps, 150, 115, 2);
        } else if (menuoption == 70) {
          menuopen = true;
          rtEditCount = (int)totalEstacoes();
          rtEditIndex = 0;
          String curRT = getEstacao(ptyStationIndex).rt;
          for (int i = 0; i < rtEditCount; i++) {
            if (curRT == getEstacao(i).rt) { rtEditIndex = i; break; }
          }
          tft.drawRoundRect(30, 40, 240, 160, 5, TFT_WHITE);
          tft.fillRoundRect(32, 42, 236, 156, 5, TFT_BLACK);
          tft.setTextColor(TFT_WHITE);
          uint32_t fr = getEstacao(ptyStationIndex).freq_khz;
          tft.drawCentreString(String(fr/1000)+"."+String((fr%1000)/100)+" MHz", 150, 55, 2);
          tft.drawCentreString(getUIString(UI_SET_RT, languageSet), 150, 75, 4);
          tft.setTextColor(TFT_YELLOW);
          if (rtEditCount > 0) {
            String rtOpt = getEstacao(rtEditIndex).rt;
            if (rtOpt.length() > 22) rtOpt = rtOpt.substring(0, 22);
            tft.drawCentreString(rtOpt, 150, 115, 2);
          }
        } else if (menuoption == 190) {
          menuPage = 8; menuoption = 30; BuildMenu();
        } else if (menuoption == 170) {
          menuopen = true;
          musicaEditCount = (int)totalEstacoes();
          musicaEditIndex = 0;
          String curMus = getEstacao(ptyStationIndex).musica;
          for (int i = 0; i < musicaEditCount; i++) {
            if (curMus == getEstacao(i).musica) { musicaEditIndex = i; break; }
          }
          tft.drawRoundRect(30, 40, 240, 160, 5, TFT_WHITE);
          tft.fillRoundRect(32, 42, 236, 156, 5, TFT_BLACK);
          tft.setTextColor(TFT_WHITE);
          uint32_t fm = getEstacao(ptyStationIndex).freq_khz;
          tft.drawCentreString(String(fm/1000)+"."+String((fm%1000)/100)+" MHz", 150, 55, 2);
          tft.drawCentreString(getUIString(UI_SET_MUSICA, languageSet), 150, 75, 4);
          tft.setTextColor(TFT_YELLOW);
          if (musicaEditCount > 0) {
            String mOpt = getEstacao(musicaEditIndex).musica;
            if (mOpt.length() > 22) mOpt = mOpt.substring(0, 22);
            tft.drawCentreString(mOpt, 150, 115, 2);
          }
        }
      } else {
        if (menuoption == 30) {
          getEstacao(ptyStationIndex).pty_code = (int8_t)ptyEditCode;
          saveCustomPTYCodes();
        } else if (menuoption == 50) {
          if (psEditCount > 0) getEstacao(ptyStationIndex).ps = getEstacao(psEditIndex).ps;
        } else if (menuoption == 70) {
          if (rtEditCount > 0) getEstacao(ptyStationIndex).rt = getEstacao(rtEditIndex).rt;
        } else if (menuoption == 90) {
          getEstacao(ptyStationIndex).pi_code = (uint16_t)piEditCode;
          saveCustomPICodes();
        } else if (menuoption == 170) {
          if (musicaEditCount > 0) {
            getEstacao(ptyStationIndex).musica = getEstacao(musicaEditIndex).musica;
            getEstacao(ptyStationIndex).anoMusica = getEstacao(musicaEditIndex).anoMusica;
            getEstacao(ptyStationIndex).posScroll = 0;
          }
        }
        lastCustomFreq = 0; menuopen = false; BuildMenu();
      }
      while (digitalRead(ROTARY_BUTTON) == LOW) delay(50);
      return;
    }
    if (menuPage == 8) {
      if (menuopen == false) {
        Estacao& e8b = getEstacao(ptyStationIndex);
        auto drawDialog8 = [&](const char* label, const String& val) {
          tft.drawRoundRect(30, 40, 240, 160, 5, TFT_WHITE);
          tft.fillRoundRect(32, 42, 236, 156, 5, TFT_BLACK);
          tft.setTextColor(TFT_WHITE);
          tft.drawCentreString(label, 150, 65, 4);
          tft.setTextColor(TFT_YELLOW);
          tft.drawCentreString(val, 150, 115, 4);
        };
        if (menuoption == 150) {
          menuPage = 6; menuoption = 190; BuildMenu();
        } else if (menuoption == 30) {
          menuopen = true;
          horaEditVal = e8b.hora.toInt();
          if (horaEditVal < 0 || horaEditVal > 23) horaEditVal = 0;
          String h = String(horaEditVal); if (h.length() < 2) h = "0" + h;
          drawDialog8("Hora (HH):", h);
        } else if (menuoption == 70) {
          menuopen = true;
          minutoEditVal = e8b.minuto.toInt();
          if (minutoEditVal < 0 || minutoEditVal > 59) minutoEditVal = 0;
          String m = String(minutoEditVal); if (m.length() < 2) m = "0" + m;
          drawDialog8("Minuto (MM):", m);
        } else if (menuoption == 50) {
          menuopen = true;
          diaEditVal = e8b.dia.toInt();
          if (diaEditVal < 1 || diaEditVal > 31) diaEditVal = 1;
          String d = String(diaEditVal); if (d.length() < 2) d = "0" + d;
          drawDialog8("Dia (DD):", d);
        } else if (menuoption == 90) {
          menuopen = true;
          mesEditVal = e8b.mes.toInt();
          if (mesEditVal < 1 || mesEditVal > 12) mesEditVal = 1;
          String ms = String(mesEditVal); if (ms.length() < 2) ms = "0" + ms;
          drawDialog8("Mes (MM):", ms);
        } else if (menuoption == 110) {
          menuopen = true;
          tempoEditIdx = 0;
          for (int i = 0; i < TEMPO_COUNT; i++) {
            if (e8b.tempo == TEMPO_LIST[i]) { tempoEditIdx = i; break; }
          }
          drawDialog8("Tempo:", String(TEMPO_LIST[tempoEditIdx]));
        } else if (menuoption == 130) {
          menuopen = true;
          tempNumEdit = e8b.temperatura.toInt();
          if (tempNumEdit < -10 || tempNumEdit > 60) tempNumEdit = 25;
          drawDialog8("Temperatura:", String(tempNumEdit) + " C");
        }
      } else {
        Estacao& e8s = getEstacao(ptyStationIndex);
        auto fmt2 = [](int v) -> String {
          String s = String(v); if (s.length() < 2) s = "0" + s; return s;
        };
        if (menuoption == 30) {
          e8s.hora = fmt2(horaEditVal);
        } else if (menuoption == 70) {
          e8s.minuto = fmt2(minutoEditVal);
        } else if (menuoption == 50) {
          e8s.dia = fmt2(diaEditVal);
        } else if (menuoption == 90) {
          e8s.mes = fmt2(mesEditVal);
        } else if (menuoption == 110) {
          e8s.tempo = String(TEMPO_LIST[tempoEditIdx]);
        } else if (menuoption == 130) {
          e8s.temperatura = String(tempNumEdit) + "\xB0" "C";
        }
        lastCustomFreq = 0; menuopen = false; BuildMenu();
      }
      while (digitalRead(ROTARY_BUTTON) == LOW) delay(50);
      return;
    }
    if (menuPage == 2) {
      int count = (int)totalEstacoes();
      if (ptyStationIndex == count) {
        menuPage = 1; menuoption = 90; BuildMenu();
      } else {
        menuPage = 6; menuoption = 30; BuildMenu();
      }
      while (digitalRead(ROTARY_BUTTON) == LOW) delay(50);
      return;
    }
    if (menuopen == false) {
      if (menuPage == 0 && menuoption == 210) {
        menuPage = 1; menuoption = 30; BuildMenu(); return;
      }
      if (menuPage == 1 && menuoption == 90) {
        ptyStationIndex = 0; ptyScrollTop = 0;
        menuPage = 2; menuoption = 30; BuildMenu(); return;
      }
      if (menuPage == 1 && menuoption == 110) {
        habilitarTodasRDS(); BuildMenu(); return;
      }
      if (menuPage == 1 && menuoption == 130) {
        menuPage = 0; menuoption = 190; BuildMenu(); return;
      }
      menuopen = true;
      tft.drawRoundRect(30, 40, 240, 160, 5, TFT_WHITE);
      tft.fillRoundRect(32, 42, 236, 156, 5, TFT_BLACK);

      if (menuPage == 0) {
        realMenuoption = menuoption;
      } else if (menuPage == 1) {
        if (menuoption == 30) realMenuoption = 210;
        else if (menuoption == 50) realMenuoption = 230;
        else if (menuoption == 70) realMenuoption = 250;
        else realMenuoption = 30;
      } else {
        realMenuoption = menuoption;
      }

      switch (realMenuoption) {
        case 30:
          tft.setTextColor(TFT_WHITE);
          tft.drawCentreString(getUIString(UI_VOLUME_LABEL, languageSet), 150, 70, 4);
          tft.drawString("dB", 170, 110, 4);

          tft.setTextColor(TFT_YELLOW);
          if (VolSet > 0) tft.drawRightString("+" + String(VolSet, DEC), 165, 110, 4); else tft.drawRightString(String(VolSet, DEC), 165, 110, 4);
          break;

        case 50:
          tft.setTextColor(TFT_WHITE);
          tft.drawCentreString(getUIString(UI_CONVERTER_LABEL, languageSet), 150, 70, 4);
          tft.drawString("MHz", 170, 110, 4);
          tft.setTextColor(TFT_YELLOW);

          tft.drawRightString(String(ConverterSet, DEC), 165, 110, 4);
          break;

        case 70:
          tft.setTextColor(TFT_WHITE);
          tft.drawCentreString(getUIString(UI_LOW_BANDEDGE_LABEL, languageSet), 150, 70, 4);
          tft.drawString("MHz", 170, 110, 4);
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(LowEdgeSet + ConverterSet, DEC), 165, 110, 4);
          break;

        case 90:
          tft.setTextColor(TFT_WHITE);
          tft.drawCentreString(getUIString(UI_HIGH_BANDEDGE_LABEL, languageSet), 150, 70, 4);
          tft.drawString("MHz", 170, 110, 4);
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(HighEdgeSet + ConverterSet, DEC), 165, 110, 4);
          break;

        case 110:
          tft.setTextColor(TFT_WHITE);
          tft.drawCentreString(getUIString(UI_RF_LEVEL_LABEL, languageSet), 150, 70, 4);
          tft.drawString("dB", 170, 110, 4);
          tft.drawString("dBuV", 190, 157, 4);
          tft.setTextColor(TFT_YELLOW);
          if (LevelOffset > 0) tft.drawRightString("+" + String(LevelOffset, DEC), 165, 110, 4); else tft.drawRightString(String(LevelOffset, DEC), 165, 110, 4);
          SStatusold = 2000;
          change2 = true;
          break;

        case 130:
          tft.setTextColor(TFT_WHITE);
          tft.drawCentreString(getUIString(UI_STEREO_THRESH_LABEL, languageSet), 150, 70, 4);
          if (StereoLevel != 0) tft.drawString("dBuV", 170, 110, 4);
          tft.setTextColor(TFT_YELLOW);
          if (StereoLevel != 0) tft.drawRightString(String(StereoLevel, DEC), 165, 110, 4); else tft.drawRightString(getUIString(UI_OFF, languageSet), 165, 110, 4);
          break;

        case 150:
          tft.setTextColor(TFT_WHITE);
          tft.drawCentreString(getUIString(UI_HIGHCUT_CORNER_LABEL, languageSet), 150, 70, 4);
          if (HighCutLevel != 0) tft.drawString("Hz", 170, 110, 4);
          tft.setTextColor(TFT_YELLOW);
          if (HighCutLevel != 0) tft.drawRightString(String(HighCutLevel * 100, DEC), 165, 110, 4); else tft.drawRightString(getUIString(UI_OFF, languageSet), 165, 110, 4);
          break;

        case 170:
          tft.setTextColor(TFT_WHITE);
          tft.drawCentreString(getUIString(UI_HIGHCUT_THRESH_LABEL, languageSet), 150, 70, 4);
          if (HighCutOffset != 0) tft.drawString("dBuV", 170, 110, 4);
          tft.setTextColor(TFT_YELLOW);
          if (HighCutOffset != 0) tft.drawRightString(String(HighCutOffset, DEC), 165, 110, 4); else tft.drawRightString(getUIString(UI_OFF, languageSet), 165, 110, 4);
          break;

        case 190:
          tft.setTextColor(TFT_WHITE);
          tft.drawCentreString(getUIString(UI_LOW_LEVEL_LABEL, languageSet), 150, 70, 4);
          tft.drawString("dBuV", 150, 110, 4);
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(LowLevelSet, DEC), 145, 110, 4);
          break;

        case 210:
          tft.setTextColor(TFT_WHITE);
          tft.drawCentreString(getUIString(UI_CONTRAST_LABEL, languageSet), 150, 70, 4);
          tft.drawString("%", 170, 110, 4);
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(ContrastSet, DEC), 165, 110, 4);
          break;

        case 230:
          tft.setTextColor(TFT_WHITE);
          tft.drawCentreString(getUIString(UI_LANGUAGE_LABEL, languageSet), 150, 70, 4);
          tft.setTextColor(TFT_YELLOW);
          if (languageSet == 1) tft.drawRightString("English", 165, 110, 4);
          else if (languageSet == 2) tft.drawRightString("Portugues", 165, 110, 4);
          else if (languageSet == 3) tft.drawRightString("Espanol", 165, 110, 4);
          else if (languageSet == 4) tft.drawRightString("RBDS", 165, 110, 4);
          else if (languageSet == 5) tft.drawRightString("RBDS PT", 165, 110, 4);
          else if (languageSet == 6) tft.drawRightString("RBDS ES", 165, 110, 4);
          else if (languageSet == 7) tft.drawRightString("BR-EN", 165, 110, 4);
          else if (languageSet == 8) tft.drawRightString("BR-PT", 165, 110, 4);
          else if (languageSet == 9) tft.drawRightString("BR-ES", 165, 110, 4);
          break;

        case 250:
          tft.setTextColor(TFT_WHITE);
          tft.drawCentreString(getUIString(UI_SET_THEME, languageSet), 150, 70, 4);
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(getThemeName(themeSet), 165, 110, 4);
          break;
      }
    } else {
      menuopen = false;
      BuildMenu();
      menuPage = 0;
    }
  }
  while (digitalRead(ROTARY_BUTTON) == LOW) delay(50);
}

void KeyUp() {
  rotary = 0;
  if (menu == false) {
    if (tunemode == true) {
      direction = true;
      seek = true;
      Seek(direction);
    } else {
      if (band == 0) frequency = radio.tuneUp(stepsize, LowEdgeSet, HighEdgeSet, fullsearchrds); else frequency_AM = radio.tuneUp_AM(stepsize);
    }
      if (USBstatus == true) if (band == 0) Serial.println("T" + String(frequency * 10)); else Serial.println("T" + String(frequency_AM));
    radio.clearRDS(fullsearchrds);
    change = 0;
    ShowFreq(0);
    store = true;
  } else {
    if (menuPage == 2) {
      if (menuopen == false) {
        int count = (int)totalEstacoes();
        ptyStationIndex++;
        if (ptyStationIndex > count) ptyStationIndex = 0;
        if (ptyStationIndex < count) {
          if (ptyStationIndex >= ptyScrollTop + 8) ptyScrollTop = ptyStationIndex - 7;
          if (ptyScrollTop < 0) ptyScrollTop = 0;
        }
        BuildMenu();
      }
      return;
    }
    if (menuPage == 6) {
      if (menuopen == false) {
        tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_BLACK);
        menuoption += 20;
        if (menuoption > 190) menuoption = 30;
        tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_WHITE);
      } else {
        if (menuoption == 30) {
          tft.fillRect(33, 95, 234, 65, TFT_BLACK);
          ptyEditCode = (ptyEditCode + 1) % 32;
          tft.setTextColor(TFT_YELLOW);
          tft.drawCentreString(String(ptyEditCode), 150, 105, 4);
          tft.drawCentreString(radio.getPTYText(ptyEditCode), 150, 140, 2);
        } else if (menuoption == 50) {
          if (psEditCount > 0) {
            tft.fillRect(33, 100, 234, 50, TFT_BLACK);
            psEditIndex = (psEditIndex + 1) % psEditCount;
            tft.setTextColor(TFT_YELLOW);
            tft.drawCentreString(getEstacao(psEditIndex).ps, 150, 115, 2);
          }
        } else if (menuoption == 70) {
          if (rtEditCount > 0) {
            tft.fillRect(33, 100, 234, 50, TFT_BLACK);
            rtEditIndex = (rtEditIndex + 1) % rtEditCount;
            tft.setTextColor(TFT_YELLOW);
            String rtOpt = getEstacao(rtEditIndex).rt;
            if (rtOpt.length() > 22) rtOpt = rtOpt.substring(0, 22);
            tft.drawCentreString(rtOpt, 150, 115, 2);
          }
        } else if (menuoption == 90) {
          tft.fillRect(33, 100, 234, 50, TFT_BLACK);
          piEditCode = (piEditCode + 1) & 0xFFFF;
          tft.setTextColor(TFT_YELLOW);
          String piHex = String((uint16_t)piEditCode, HEX); piHex.toUpperCase();
          while (piHex.length() < 4) piHex = "0" + piHex;
          tft.drawCentreString(piHex, 150, 115, 4);
        } else if (menuoption == 170) {
          if (musicaEditCount > 0) {
            tft.fillRect(33, 100, 234, 50, TFT_BLACK);
            musicaEditIndex = (musicaEditIndex + 1) % musicaEditCount;
            tft.setTextColor(TFT_YELLOW);
            String mOpt = getEstacao(musicaEditIndex).musica;
            if (mOpt.length() > 22) mOpt = mOpt.substring(0, 22);
            tft.drawCentreString(mOpt, 150, 115, 2);
          }
        }
      }
      return;
    }
    if (menuPage == 8) {
      if (menuopen == false) {
        tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_BLACK);
        menuoption += 20;
        if (menuoption > 150) menuoption = 30;
        tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_WHITE);
      } else {
        auto updNum8 = [](const String& val) {
          tft.fillRect(33, 100, 234, 60, TFT_BLACK);
          tft.setTextColor(TFT_YELLOW);
          tft.drawCentreString(val, 150, 115, 4);
        };
        if (menuoption == 30) {
          horaEditVal = (horaEditVal + 1) % 24;
          String h = String(horaEditVal); if (h.length() < 2) h = "0" + h; updNum8(h);
        } else if (menuoption == 70) {
          minutoEditVal = (minutoEditVal + 1) % 60;
          String m = String(minutoEditVal); if (m.length() < 2) m = "0" + m; updNum8(m);
        } else if (menuoption == 50) {
          diaEditVal = diaEditVal % 31 + 1;
          String d = String(diaEditVal); if (d.length() < 2) d = "0" + d; updNum8(d);
        } else if (menuoption == 90) {
          mesEditVal = mesEditVal % 12 + 1;
          String ms = String(mesEditVal); if (ms.length() < 2) ms = "0" + ms; updNum8(ms);
        } else if (menuoption == 110) {
          tempoEditIdx = (tempoEditIdx + 1) % TEMPO_COUNT;
          tft.fillRect(33, 100, 234, 60, TFT_BLACK);
          tft.setTextColor(TFT_YELLOW);
          tft.drawCentreString(String(TEMPO_LIST[tempoEditIdx]), 150, 115, 2);
        } else if (menuoption == 130) {
          if (tempNumEdit < 60) tempNumEdit++;
          updNum8(String(tempNumEdit) + " C");
        }
      }
      return;
    }
    if (menuopen == false) {
      tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_BLACK);
      menuoption += 20;
      if (menuPage == 0) {
        if (menuoption > 210) { menuPage = 1; menuoption = 30; }
      } else {
        if (menuoption > 130) { menuPage = 0; menuoption = 30; }
      }
      tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_WHITE);
    } else {
      if (menuPage == 0) {
        realMenuoption = menuoption;
      } else {
        realMenuoption = menuoption + 180;
      }

      switch (realMenuoption) {
        case 30:
          tft.setTextColor(TFT_BLACK);
          if (VolSet > 0) tft.drawRightString("+" + String(VolSet, DEC), 165, 110, 4); else tft.drawRightString(String(VolSet, DEC), 165, 110, 4);
          VolSet++;
          if (VolSet > 10) VolSet = 10;
          tft.setTextColor(TFT_YELLOW);
          if (VolSet > 0) tft.drawRightString("+" + String(VolSet, DEC), 165, 110, 4); else tft.drawRightString(String(VolSet, DEC), 165, 110, 4);
          radio.setVolume(VolSet);
          break;

        case 50:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(ConverterSet, DEC), 165, 110, 4);
          ConverterSet++;
            if (ConverterSet > 2400 || ConverterSet <= 200) if (ConverterSet == 1) ConverterSet = 200; else ConverterSet = 0;
          if (ConverterSet >= 200) {
            Wire.beginTransmission(0x12);
            Wire.write(ConverterSet >> 8);
            Wire.write(ConverterSet & (0xFF));
            Wire.endTransmission();
          }
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(ConverterSet, DEC), 165, 110, 4);
          break;

        case 70:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(LowEdgeSet + ConverterSet, DEC), 165, 110, 4);
          LowEdgeSet ++;
          if (LowEdgeSet > 107) LowEdgeSet = 65;
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(LowEdgeSet + ConverterSet, DEC), 165, 110, 4);
          break;

        case 90:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(HighEdgeSet + ConverterSet, DEC), 165, 110, 4);
          HighEdgeSet ++;
          if (HighEdgeSet > 108) HighEdgeSet = 66;
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(HighEdgeSet + ConverterSet, DEC), 165, 110, 4);
          break;

        case 110:
          tft.setTextColor(TFT_BLACK);
          if (LevelOffset > 0) tft.drawRightString("+" + String(LevelOffset, DEC), 165, 110, 4); else tft.drawRightString(String(LevelOffset, DEC), 165, 110, 4);
          LevelOffset++;
          if (LevelOffset > 15) LevelOffset = -25;
          tft.setTextColor(TFT_YELLOW);
          if (LevelOffset > 0) tft.drawRightString("+" + String(LevelOffset, DEC), 165, 110, 4); else tft.drawRightString(String(LevelOffset, DEC), 165, 110, 4);          radio.setOffset(LevelOffset);
          change2 = true;
          break;

        case 130:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(StereoLevel, DEC), 165, 110, 4);
          StereoLevel ++;
            if (StereoLevel > 60 || StereoLevel <= 30) if (StereoLevel == 1) StereoLevel = 30; else StereoLevel = 0;
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(getUIString(UI_OFF, languageSet), 165, 110, 4);
          tft.drawString("dBuV", 170, 110, 4);
          tft.setTextColor(TFT_WHITE);
          if (StereoLevel != 0) tft.drawString("dBuV", 170, 110, 4);
          tft.setTextColor(TFT_YELLOW);
          if (StereoLevel != 0) tft.drawRightString(String(StereoLevel, DEC), 165, 110, 4); else tft.drawRightString(getUIString(UI_OFF, languageSet), 165, 110, 4);
          radio.setStereoLevel(StereoLevel);
          break;

        case 150:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(HighCutLevel * 100, DEC), 165, 110, 4);
          HighCutLevel ++;
          if (HighCutLevel > 70) HighCutLevel = 15;
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(HighCutLevel * 100, DEC), 165, 110, 4);
          radio.setHighCutLevel(HighCutLevel);
          break;

        case 170:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(HighCutOffset, DEC), 165, 110, 4);
          HighCutOffset ++;
            if (HighCutOffset > 60 || HighCutOffset <= 20) if (HighCutOffset == 1) HighCutOffset = 20; else HighCutOffset = 0;
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(getUIString(UI_OFF, languageSet), 165, 110, 4);
          tft.drawString("dBuV", 170, 110, 4);
          tft.setTextColor(TFT_WHITE);
          if (HighCutOffset != 0) tft.drawString("dBuV", 170, 110, 4);
          tft.setTextColor(TFT_YELLOW);
          if (HighCutOffset != 0) tft.drawRightString(String(HighCutOffset, DEC), 165, 110, 4); else tft.drawRightString(getUIString(UI_OFF, languageSet), 165, 110, 4);
          radio.setHighCutOffset(HighCutOffset);
          break;

        case 190:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(LowLevelSet, DEC), 145, 110, 4);
          LowLevelSet++;
          if (LowLevelSet > 40 || LowLevelSet <= 0) LowLevelSet = 0;
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(LowLevelSet, DEC), 145, 110, 4);
          break;

        case 210:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(ContrastSet, DEC), 165, 110, 4);
          ContrastSet ++;
          if (ContrastSet > 100) ContrastSet = 1;
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(ContrastSet, DEC), 165, 110, 4);
          analogWrite(CONTRASTPIN, ContrastSet * 2 + 27);
          break;

        case 230:
          tft.setTextColor(TFT_BLACK);
          if (languageSet == 1) tft.drawRightString("English", 165, 110, 4);
          else if (languageSet == 2) tft.drawRightString("Portugues", 165, 110, 4);
          else if (languageSet == 3) tft.drawRightString("Espanol", 165, 110, 4);
          else if (languageSet == 4) tft.drawRightString("RBDS", 165, 110, 4);
          else if (languageSet == 5) tft.drawRightString("RBDS PT", 165, 110, 4);
          else if (languageSet == 6) tft.drawRightString("RBDS ES", 165, 110, 4);
          else if (languageSet == 7) tft.drawRightString("BR-EN", 165, 110, 4);
          else if (languageSet == 8) tft.drawRightString("BR-PT", 165, 110, 4);
          else if (languageSet == 9) tft.drawRightString("BR-ES", 165, 110, 4);
          languageSet++;
          if (languageSet > 9) languageSet = 1;
          tft.setTextColor(TFT_YELLOW);
          if (languageSet == 1) tft.drawRightString("English", 165, 110, 4);
          else if (languageSet == 2) tft.drawRightString("Portugues", 165, 110, 4);
          else if (languageSet == 3) tft.drawRightString("Espanol", 165, 110, 4);
          else if (languageSet == 4) tft.drawRightString("RBDS", 165, 110, 4);
          else if (languageSet == 5) tft.drawRightString("RBDS PT", 165, 110, 4);
          else if (languageSet == 6) tft.drawRightString("RBDS ES", 165, 110, 4);
          else if (languageSet == 7) tft.drawRightString("BR-EN", 165, 110, 4);
          else if (languageSet == 8) tft.drawRightString("BR-PT", 165, 110, 4);
          else if (languageSet == 9) tft.drawRightString("BR-ES", 165, 110, 4);
          setPTYLanguage(languageSet);
          carregarEstacoes();
          loadCustomRDSEnabled();
          loadCustomPICodes();
          loadCustomPTYCodes();
          lastCustomFreq = 0;
          EEPROM.writeByte(56, languageSet);
          EEPROM.commit();
          break;

        case 250:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(getThemeName(themeSet), 165, 110, 4);
          themeSet = (themeSet + 1) % 2601;
          applyTheme(themeSet);
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(getThemeName(themeSet), 165, 110, 4);
          break;
      }
    }
  }
}

void KeyDown() {
  rotary = 0;
  if (menu == false) {
    if (tunemode == true) {
      direction = false;
      seek = true;
      Seek(direction);
    } else {
      if (band == 0) frequency = radio.tuneDown(stepsize, LowEdgeSet, HighEdgeSet, fullsearchrds); else frequency_AM = radio.tuneDown_AM(stepsize);
    }
      if (USBstatus == true) if (band == 0) Serial.println("T" + String(frequency * 10)); else Serial.println("T" + String(frequency_AM));
    radio.clearRDS(fullsearchrds);
    change = 0;
    ShowFreq(0);
    store = true;
  } else {
    if (menuPage == 2) {
      if (menuopen == false) {
        int count = (int)totalEstacoes();
        ptyStationIndex--;
        if (ptyStationIndex < 0) ptyStationIndex = count;
        if (ptyStationIndex < count && ptyStationIndex < ptyScrollTop) ptyScrollTop = ptyStationIndex;
        BuildMenu();
      }
      return;
    }
    if (menuPage == 6) {
      if (menuopen == false) {
        tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_BLACK);
        menuoption -= 20;
        if (menuoption < 30) menuoption = 190;
        tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_WHITE);
      } else {
        if (menuoption == 30) {
          tft.fillRect(33, 95, 234, 65, TFT_BLACK);
          ptyEditCode = (ptyEditCode + 31) % 32;
          tft.setTextColor(TFT_YELLOW);
          tft.drawCentreString(String(ptyEditCode), 150, 105, 4);
          tft.drawCentreString(radio.getPTYText(ptyEditCode), 150, 140, 2);
        } else if (menuoption == 50) {
          if (psEditCount > 0) {
            tft.fillRect(33, 100, 234, 50, TFT_BLACK);
            psEditIndex = (psEditIndex + psEditCount - 1) % psEditCount;
            tft.setTextColor(TFT_YELLOW);
            tft.drawCentreString(getEstacao(psEditIndex).ps, 150, 115, 2);
          }
        } else if (menuoption == 70) {
          if (rtEditCount > 0) {
            tft.fillRect(33, 100, 234, 50, TFT_BLACK);
            rtEditIndex = (rtEditIndex + rtEditCount - 1) % rtEditCount;
            tft.setTextColor(TFT_YELLOW);
            String rtOpt = getEstacao(rtEditIndex).rt;
            if (rtOpt.length() > 22) rtOpt = rtOpt.substring(0, 22);
            tft.drawCentreString(rtOpt, 150, 115, 2);
          }
        } else if (menuoption == 90) {
          tft.fillRect(33, 100, 234, 50, TFT_BLACK);
          piEditCode = (piEditCode + 0xFFFF) & 0xFFFF;
          tft.setTextColor(TFT_YELLOW);
          String piHex = String((uint16_t)piEditCode, HEX); piHex.toUpperCase();
          while (piHex.length() < 4) piHex = "0" + piHex;
          tft.drawCentreString(piHex, 150, 115, 4);
        } else if (menuoption == 170) {
          if (musicaEditCount > 0) {
            tft.fillRect(33, 100, 234, 50, TFT_BLACK);
            musicaEditIndex = (musicaEditIndex + musicaEditCount - 1) % musicaEditCount;
            tft.setTextColor(TFT_YELLOW);
            String mOpt = getEstacao(musicaEditIndex).musica;
            if (mOpt.length() > 22) mOpt = mOpt.substring(0, 22);
            tft.drawCentreString(mOpt, 150, 115, 2);
          }
        }
      }
      return;
    }
    if (menuPage == 8) {
      if (menuopen == false) {
        tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_BLACK);
        menuoption -= 20;
        if (menuoption < 30) menuoption = 150;
        tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_WHITE);
      } else {
        auto updNum8d = [](const String& val) {
          tft.fillRect(33, 100, 234, 60, TFT_BLACK);
          tft.setTextColor(TFT_YELLOW);
          tft.drawCentreString(val, 150, 115, 4);
        };
        if (menuoption == 30) {
          horaEditVal = (horaEditVal + 23) % 24;
          String h = String(horaEditVal); if (h.length() < 2) h = "0" + h; updNum8d(h);
        } else if (menuoption == 70) {
          minutoEditVal = (minutoEditVal + 59) % 60;
          String m = String(minutoEditVal); if (m.length() < 2) m = "0" + m; updNum8d(m);
        } else if (menuoption == 50) {
          diaEditVal = (diaEditVal - 2 + 31) % 31 + 1;
          String d = String(diaEditVal); if (d.length() < 2) d = "0" + d; updNum8d(d);
        } else if (menuoption == 90) {
          mesEditVal = (mesEditVal - 2 + 12) % 12 + 1;
          String ms = String(mesEditVal); if (ms.length() < 2) ms = "0" + ms; updNum8d(ms);
        } else if (menuoption == 110) {
          tempoEditIdx = (tempoEditIdx + TEMPO_COUNT - 1) % TEMPO_COUNT;
          tft.fillRect(33, 100, 234, 60, TFT_BLACK);
          tft.setTextColor(TFT_YELLOW);
          tft.drawCentreString(String(TEMPO_LIST[tempoEditIdx]), 150, 115, 2);
        } else if (menuoption == 130) {
          if (tempNumEdit > -10) tempNumEdit--;
          updNum8d(String(tempNumEdit) + " C");
        }
      }
      return;
    }
    if (menuopen == false) {
      tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_BLACK);
      menuoption -= 20;
      if (menuPage == 0) {
        if (menuoption < 30) { menuPage = 1; menuoption = 130; }
      } else {
        if (menuoption < 30) { menuPage = 0; menuoption = 210; }
      }
      tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_WHITE);
    } else {
      if (menuPage == 0) {
        realMenuoption = menuoption;
      } else {
        realMenuoption = menuoption + 180;
      }

      switch (realMenuoption) {
        case 30:
          tft.setTextColor(TFT_BLACK);
          if (VolSet > 0) tft.drawRightString("+" + String(VolSet, DEC), 165, 110, 4); else tft.drawRightString(String(VolSet, DEC), 165, 110, 4);
          VolSet--;
          if (VolSet < -10) VolSet = -10;
          tft.setTextColor(TFT_YELLOW);
          if (VolSet > 0) tft.drawRightString("+" + String(VolSet, DEC), 165, 110, 4); else tft.drawRightString(String(VolSet, DEC), 165, 110, 4);          radio.setVolume(VolSet);
          break;

        case 50:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(ConverterSet, DEC), 165, 110, 4);
          ConverterSet--;
            if (ConverterSet < 200) if (ConverterSet < 0) ConverterSet = 2400; else ConverterSet = 0;
          if (ConverterSet >= 200) {
            Wire.beginTransmission(0x12);
            Wire.write(ConverterSet >> 8);
            Wire.write(ConverterSet & (0xFF));
            Wire.endTransmission();
          }
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(ConverterSet, DEC), 165, 110, 4);
          break;

        case 70:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(LowEdgeSet + ConverterSet, DEC), 165, 110, 4);
          LowEdgeSet --;
          if (LowEdgeSet < 65) LowEdgeSet = 107;
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(LowEdgeSet + ConverterSet, DEC), 165, 110, 4);
          break;

        case 90:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(HighEdgeSet + ConverterSet, DEC), 165, 110, 4);
          HighEdgeSet --;
          if (HighEdgeSet < 66) HighEdgeSet = 108;
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(HighEdgeSet + ConverterSet, DEC), 165, 110, 4);
          break;

        case 110:
          tft.setTextColor(TFT_BLACK);
          if (LevelOffset > 0) tft.drawRightString("+" + String(LevelOffset, DEC), 165, 110, 4); else tft.drawRightString(String(LevelOffset, DEC), 165, 110, 4);
          LevelOffset--;
          if (LevelOffset < -25) LevelOffset = 15;
          tft.setTextColor(TFT_YELLOW);
          if (LevelOffset > 0) tft.drawRightString("+" + String(LevelOffset, DEC), 165, 110, 4); else tft.drawRightString(String(LevelOffset, DEC), 165, 110, 4);          radio.setOffset(LevelOffset);
          change2 = true;
          break;

        case 130:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(StereoLevel, DEC), 165, 110, 4);
          StereoLevel --;
            if (StereoLevel < 30) if (StereoLevel < 0) StereoLevel = 60; else StereoLevel = 0;
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(getUIString(UI_OFF, languageSet), 165, 110, 4);
          tft.drawString("dBuV", 170, 110, 4);
          tft.setTextColor(TFT_WHITE);
          if (StereoLevel != 0) {
            tft.drawString("dBuV", 170, 110, 4);
          }
          tft.setTextColor(TFT_YELLOW);
          if (StereoLevel != 0) tft.drawRightString(String(StereoLevel, DEC), 165, 110, 4); else tft.drawRightString(getUIString(UI_OFF, languageSet), 165, 110, 4);
          radio.setStereoLevel(StereoLevel);
          break;

        case 150:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(HighCutLevel * 100, DEC), 165, 110, 4);
          HighCutLevel --;
          if (HighCutLevel < 15) HighCutLevel = 70;
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(HighCutLevel * 100, DEC), 165, 110, 4);
          radio.setHighCutLevel(HighCutLevel);
          break;

        case 170:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(HighCutOffset, DEC), 165, 110, 4);
          HighCutOffset --;
            if (HighCutOffset < 20) if (HighCutOffset < 0) HighCutOffset = 60; else HighCutOffset = 0;
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(getUIString(UI_OFF, languageSet), 165, 110, 4);
          tft.drawString("dBuV", 170, 110, 4);
          tft.setTextColor(TFT_WHITE);
          if (HighCutOffset != 0) tft.drawString("dBuV", 170, 110, 4);
          tft.setTextColor(TFT_YELLOW);
          if (HighCutOffset != 0) tft.drawRightString(String(HighCutOffset, DEC), 165, 110, 4); else tft.drawRightString(getUIString(UI_OFF, languageSet), 165, 110, 4);
          radio.setHighCutOffset(HighCutOffset);
          break;

        case 190:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(LowLevelSet, DEC), 145, 110, 4);
          LowLevelSet--;
          if (LowLevelSet > 40) LowLevelSet = 40;
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(LowLevelSet, DEC), 145, 110, 4);
          break;


        case 210:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(ContrastSet, DEC), 165, 110, 4);
          ContrastSet --;
          if (ContrastSet < 1) ContrastSet = 100;
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(String(ContrastSet, DEC), 165, 110, 4);
          analogWrite(CONTRASTPIN, ContrastSet * 2 + 27);
          break;

        case 230:
          tft.setTextColor(TFT_BLACK);
          if (languageSet == 1) tft.drawRightString("English", 165, 110, 4);
          else if (languageSet == 2) tft.drawRightString("Portugues", 165, 110, 4);
          else if (languageSet == 3) tft.drawRightString("Espanol", 165, 110, 4);
          else if (languageSet == 4) tft.drawRightString("RBDS", 165, 110, 4);
          else if (languageSet == 5) tft.drawRightString("RBDS PT", 165, 110, 4);
          else if (languageSet == 6) tft.drawRightString("RBDS ES", 165, 110, 4);
          else if (languageSet == 7) tft.drawRightString("BR-EN", 165, 110, 4);
          else if (languageSet == 8) tft.drawRightString("BR-PT", 165, 110, 4);
          else if (languageSet == 9) tft.drawRightString("BR-ES", 165, 110, 4);
          languageSet--;
          if (languageSet < 1) languageSet = 9;
          tft.setTextColor(TFT_YELLOW);
          if (languageSet == 1) tft.drawRightString("English", 165, 110, 4);
          else if (languageSet == 2) tft.drawRightString("Portugues", 165, 110, 4);
          else if (languageSet == 3) tft.drawRightString("Espanol", 165, 110, 4);
          else if (languageSet == 4) tft.drawRightString("RBDS", 165, 110, 4);
          else if (languageSet == 5) tft.drawRightString("RBDS PT", 165, 110, 4);
          else if (languageSet == 6) tft.drawRightString("RBDS ES", 165, 110, 4);
          else if (languageSet == 7) tft.drawRightString("BR-EN", 165, 110, 4);
          else if (languageSet == 8) tft.drawRightString("BR-PT", 165, 110, 4);
          else if (languageSet == 9) tft.drawRightString("BR-ES", 165, 110, 4);
          setPTYLanguage(languageSet);
          carregarEstacoes();
          loadCustomRDSEnabled();
          loadCustomPICodes();
          loadCustomPTYCodes();
          lastCustomFreq = 0;
          EEPROM.writeByte(56, languageSet);
          EEPROM.commit();
          break;

        case 250:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(getThemeName(themeSet), 165, 110, 4);
          themeSet = (themeSet + 2600) % 2601;
          applyTheme(themeSet);
          tft.setTextColor(TFT_YELLOW);
          tft.drawRightString(getThemeName(themeSet), 165, 110, 4);
          break;
      }
    }
  }
}



void readRds() {
  if (band == 0) {
    RDSstatus = radio.readRDS();
    ShowRDSLogo(RDSstatus);

    // Buscar dados customizados da frequência atual
    uint32_t currentFreqKhz = (uint32_t)radio.getFrequency() * 10; // FM freq em 10kHz, converter para kHz
    if (currentFreqKhz != lastCustomFreq) {
      lastCustomFreq = currentFreqKhz;
      if (isRDSAtivo(currentFreqKhz)) {
        customPS = buscarPS(currentFreqKhz);
        customRT = buscarRT(currentFreqKhz);
        int8_t ptyCode = buscarPTY(currentFreqKhz);
        customPTY = (ptyCode >= 0) ? radio.getPTYText(ptyCode) : "";
        customPI = buscarPI(currentFreqKhz);
      } else {
        customPS = "";
        customRT = "";
        customPTY = "";
        customPI = "";
      }
    }

    if (RDSstatus == 0) {
      tft.setTextColor(TFT_BLUE);
      tft.drawString(PIold, 244, 185, 4);
      tft.setTextColor(TFT_BLUE);
      tft.drawString(PTYold, 38, 161, 2);
      tft.setTextColor(TFT_BLACK);
      tft.drawString(RTold, 6, 215, 2);
      dropout = true;
    } else {
      if (dropout == true && PIold.length() != 0) {
        tft.setTextColor(UI_LABEL_COLOR);
        tft.drawString(PIold, 244, 185, 4);
        tft.setTextColor(TFT_WHITE);
        tft.drawString(PTYold, 38, 161, 2);
        dropout = false;
      }
    }

    if (RDSstatus == 1 && USBstatus == true) {
      Serial.print ("P");
      Serial.print (String(((radio.rds.rdsA >> 8) >> 4) & 0xF, HEX) + String((radio.rds.rdsA >> 8) & 0xF, HEX));
      Serial.print (String(((radio.rds.rdsA) >> 4) & 0xF, HEX) + String((radio.rds.rdsA) & 0xF, HEX));
      Serial.print ("\n");

      XDRGTKRDS = "R";
      XDRGTKRDS += String(((radio.rds.rdsB >> 8) >> 4) & 0xF, HEX) + String((radio.rds.rdsB >> 8) & 0xF, HEX);
      XDRGTKRDS += String(((radio.rds.rdsB) >> 4) & 0xF, HEX) + String((radio.rds.rdsB) & 0xF, HEX);
      XDRGTKRDS += String(((radio.rds.rdsC >> 8) >> 4) & 0xF, HEX) + String((radio.rds.rdsC >> 8) & 0xF, HEX);
      XDRGTKRDS += String(((radio.rds.rdsC) >> 4) & 0xF, HEX) + String((radio.rds.rdsC) & 0xF, HEX);
      XDRGTKRDS += String(((radio.rds.rdsD >> 8) >> 4) & 0xF, HEX) + String((radio.rds.rdsD >> 8) & 0xF, HEX);
      XDRGTKRDS += String(((radio.rds.rdsD) >> 4) & 0xF, HEX) + String((radio.rds.rdsD) & 0xF, HEX);
      XDRGTKRDS += String(((radio.rds.errors >> 8) >> 4) & 0xF, HEX) + String((radio.rds.errors >> 8) & 0xF, HEX);
      XDRGTKRDS += "\n";

      if (XDRGTKRDS != XDRGTKRDSold) {
        Serial.print(XDRGTKRDS);
        XDRGTKRDSold = XDRGTKRDS;
      }
    }
  }
}

void showPI() {
  String piToShow = (customPI.length() > 0) ? customPI : String(radio.rds.picode);
  if (piToShow != PIold) {
    tft.setTextColor(TFT_BLACK);
    tft.drawString(PIold, 244, 185, 4);
    tft.setTextColor(UI_LABEL_COLOR);
    tft.drawString(piToShow, 244, 185, 4);
    PIold = piToShow;
    piToShow.toCharArray(radioIdPrevious, sizeof(radioIdPrevious));
  }
}

void showPTY() {
  String ptyToShow = (customPTY.length() > 0) ? customPTY : String(radio.rds.stationType);
  if (ptyToShow != PTYold) {
    tft.setTextColor(TFT_BLACK);
    tft.drawString(PTYold, 38, 161, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(ptyToShow, 38, 161, 2);
    PTYold = ptyToShow;
    ptyToShow.toCharArray(programTypePrevious, sizeof(programTypePrevious));
  }
}

void showPS() {
  String psToShow = (customPS.length() > 0) ? customPS : String(radio.rds.stationName);
  String rtToShow = (customRT.length() > 0) ? customRT : String(radio.rds.stationText);
  unsigned int freq = radio.getFrequency();
  String freqStr = String(freq / 100) + "." + (freq % 100 < 10 ? "0" : "") + String(freq % 100);
  psToShow = freqStr + " | " + psToShow + " | " + rtToShow;
  if (psToShow != PSold) {
    psXPos = 0;
    PSold = psToShow;
    psToShow.toCharArray(programServicePrevious, sizeof(programServicePrevious));
  }

  if (psToShow.length() > 0) {
    if (millis() - psTicker >= 350) {
      int psCharWidth = tft.textWidth("A");
      int psTextWidth = tft.textWidth(psToShow);

      psSprite.fillSprite(TFT_BLACK);
      psSprite.setTextColor(TFT_WHITE);
      psSprite.drawString(psToShow, psXPos, 2, 4);
      psSprite.pushSprite(38, 183);

      psXPos -= psCharWidth;
      if (psXPos < -psTextWidth + (psCharWidth * 12)) psXPos = 0;

      psTicker = millis();
    }
  }
}

void showRadioText() {
  String rtToShow = (customRT.length() > 0) ? customRT : String(radio.rds.stationText);
  if (rtToShow != RTold) {
    xPos = 6;
    RTold = rtToShow;
    rtToShow.toCharArray(radioTextPrevious, sizeof(radioTextPrevious));
  }

  if (rtToShow.length() > 0) {
    if (millis() - rtticker >= 350) {
      xPos -= charWidth;
      if (xPos < -tft.textWidth(rtToShow) + (charWidth * 42)) xPos = 6;
      sprite.fillSprite(TFT_BLACK);
      sprite.setTextColor(TFT_CYAN);
      sprite.drawString(rtToShow, xPos, yPos, 2);
      sprite.pushSprite(6, 213);
      rtticker = millis();
      cleanup = true;
    }
  } else if (cleanup == true) {
    sprite.fillSprite(TFT_BLACK);
    sprite.pushSprite(6, 213);
    cleanup = false;
  }
}

void showCT() {
  if (radio.rds.hasCT == true) {
    int timeoffset;
    if (radio.rds.offsetplusmin == true) timeoffset = (-1 * radio.rds.offset) / 2; else timeoffset = radio.rds.offset / 2;
    byte rdshour = radio.rds.hours + timeoffset;
    rdshour = (((int)rdshour + 24) % 24);
    setTime((radio.rds.hours + timeoffset), radio.rds.minutes, 0, 0, 0, 0);
    rds_clock = ((hour() < 10 ? "0" : "") + String(hour()) + ":" + (minute() < 10 ? "0" : "") + String(minute()));
  } else {
    rds_clock = "";
  }
  if (rds_clock != rds_clockold) {
    tft.setTextColor(TFT_BLACK);
    tft.drawRightString(rds_clockold, 205, 161, 2);
    tft.setTextColor(TFT_YELLOW);
    tft.drawRightString(rds_clock, 205, 161, 2);
    rds_clockold = rds_clock;
  }
}

void BuildMenu() {
  tft.fillScreen(TFT_BLACK);
  tft.fillRect(0, 0, 320, 23, UI_HEADER_BG);
  tft.drawRoundRect(0, 0, 319, 239, 3, UI_BORDER_COLOR);
  tft.drawLine(0, 23, 320, 23, UI_BORDER_COLOR);
  tft.setTextColor(UI_LABEL_COLOR);
  tft.drawString(getUIString(UI_PRESS_MODE_EXIT, languageSet), 20, 4, 2);
  tft.setTextColor(TFT_WHITE);
  tft.drawRightString(VERSION, 305, 4, 2);

  if (menuPage == 0) {
    tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_WHITE);
    tft.setTextColor(TFT_WHITE);
    tft.drawRightString("dB", 305, 30, 2);
    tft.drawRightString("MHz", 305, 50, 2);
    tft.drawRightString("MHz", 305, 70, 2);
    tft.drawRightString("MHz", 305, 90, 2);
    tft.drawRightString("dB", 305, 110, 2);
    if (StereoLevel != 0) tft.drawRightString("dBuV", 305, 130, 2);
    if (HighCutLevel != 0) tft.drawRightString("Hz", 305, 150, 2);
    if (HighCutOffset != 0) tft.drawRightString("dBuV", 305, 170, 2);
    tft.drawRightString("dBuV", 305, 190, 2);
    tft.drawString(getUIString(UI_SET_VOLUME,          languageSet), 20, 30, 2);
    tft.drawString(getUIString(UI_SET_CONVERTER,       languageSet), 20, 50, 2);
    tft.drawString(getUIString(UI_SET_LOW_BANDEDGE,    languageSet), 20, 70, 2);
    tft.drawString(getUIString(UI_SET_HIGH_BANDEDGE,   languageSet), 20, 90, 2);
    tft.drawString(getUIString(UI_SET_LEVEL_OFFSET,    languageSet), 20, 110, 2);
    tft.drawString(getUIString(UI_SET_STEREO_THRESH,   languageSet), 20, 130, 2);
    tft.drawString(getUIString(UI_SET_HIGHCUT_CORNER,  languageSet), 20, 150, 2);
    tft.drawString(getUIString(UI_SET_HIGHCUT_THRESH,  languageSet), 20, 170, 2);
    tft.drawString(getUIString(UI_SET_LOW_LEVEL,       languageSet), 20, 190, 2);
    tft.setTextColor(TFT_YELLOW);
    if (VolSet > 0) tft.drawRightString("+" + String(VolSet, DEC), 270, 30, 2); else tft.drawRightString(String(VolSet, DEC), 270, 30, 2);
    tft.drawRightString(String(ConverterSet, DEC), 270, 50, 2);
    tft.drawRightString(String(LowEdgeSet + ConverterSet, DEC), 270, 70, 2);
    tft.drawRightString(String(HighEdgeSet + ConverterSet, DEC), 270, 90, 2);
    if (LevelOffset > 0) tft.drawRightString("+" + String(LevelOffset, DEC), 270, 110, 2); else tft.drawRightString(String(LevelOffset, DEC), 270, 110, 2);
    if (StereoLevel != 0) tft.drawRightString(String(StereoLevel, DEC), 270, 130, 2); else tft.drawRightString(getUIString(UI_OFF, languageSet), 270, 130, 2);
    if (HighCutLevel != 0) tft.drawRightString(String(HighCutLevel * 100, DEC), 270, 150, 2); else tft.drawRightString(getUIString(UI_OFF, languageSet), 270, 150, 2);
    if (HighCutOffset != 0) tft.drawRightString(String(HighCutOffset, DEC), 270, 170, 2); else tft.drawRightString(getUIString(UI_OFF, languageSet), 270, 170, 2);
    tft.drawRightString(String(LowLevelSet, DEC), 270, 190, 2);
    tft.setTextColor(TFT_SKYBLUE);
    tft.drawString(getUIString(UI_PAGE2, languageSet), 20, 210, 2);
  } else if (menuPage == 1) {
    tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_WHITE);
    tft.setTextColor(TFT_WHITE);
    tft.drawRightString("%", 305, 30, 2);
    tft.drawString(getUIString(UI_SET_BRIGHTNESS,  languageSet), 20, 30, 2);
    tft.drawString(getUIString(UI_SET_LANGUAGE,    languageSet), 20, 50, 2);
    tft.drawString(getUIString(UI_SET_THEME,       languageSet), 20, 70, 2);
    tft.drawString(getUIString(UI_STATION_EDITOR,  languageSet), 20, 90, 2);
    tft.setTextColor(TFT_YELLOW);
    tft.drawRightString(String(ContrastSet, DEC), 270, 30, 2);
    if (languageSet == 1) tft.drawRightString("English", 270, 50, 2);
    else if (languageSet == 2) tft.drawRightString("Portugues", 270, 50, 2);
    else if (languageSet == 3) tft.drawRightString("Espanol", 270, 50, 2);
    else if (languageSet == 4) tft.drawRightString("RBDS", 270, 50, 2);
    else if (languageSet == 5) tft.drawRightString("RBDS PT", 270, 50, 2);
    else if (languageSet == 6) tft.drawRightString("RBDS ES", 270, 50, 2);
    else if (languageSet == 7) tft.drawRightString("BR-EN", 270, 50, 2);
    else if (languageSet == 8) tft.drawRightString("BR-PT", 270, 50, 2);
    else if (languageSet == 9) tft.drawRightString("BR-ES", 270, 50, 2);
    tft.drawRightString(getThemeName(themeSet), 270, 70, 2);
    tft.setTextColor(TFT_SKYBLUE);
    tft.drawString(">>", 277, 90, 2);
    tft.drawString(getUIString(UI_ENABLE_ALL_RDS, languageSet), 20, 110, 2);
    tft.drawString(getUIString(UI_PAGE1_BACK, languageSet), 20, 130, 2);
  } else if (menuPage == 2) {
    size_t count = totalEstacoes();
    int visRows = min((int)count, 8);
    for (int i = 0; i < visRows; i++) {
      int idx = ptyScrollTop + i;
      if (idx >= (int)count) break;
      int y = 30 + i * 20;
      uint32_t f = getEstacao(idx).freq_khz;
      String freqStr = String(f / 1000) + "." + String((f % 1000) / 100);
      tft.setTextColor(TFT_WHITE);
      tft.drawString(freqStr, 15, y, 2);
      String psStr = getEstacao(idx).ps;
      if (psStr.length() > 20) psStr = psStr.substring(0, 20);
      tft.setTextColor(TFT_YELLOW);
      tft.drawString(psStr, 80, y, 2);
    }
    tft.setTextColor(TFT_SKYBLUE);
    tft.drawString(getUIString(UI_BACK, languageSet), 20, 190, 2);
    int selY = (ptyStationIndex < (int)count) ? (30 + (ptyStationIndex - ptyScrollTop) * 20) : 190;
    menuoption = selY;
    tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_WHITE);
  } else if (menuPage == 6) {
    size_t count6 = totalEstacoes();
    if (ptyStationIndex >= (int)count6) { menuPage = 2; BuildMenu(); return; }
    int8_t pc6 = getEstacao(ptyStationIndex).pty_code;
    if (pc6 < 0 || pc6 > 31) pc6 = 0;
    uint16_t piCode6 = getEstacao(ptyStationIndex).pi_code;
    tft.setTextColor(TFT_WHITE);
    tft.drawString("PTY:", 15, 30, 2);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString(String(pc6) + " " + String(radio.getPTYText(pc6)), 60, 30, 2);
    tft.setTextColor(TFT_CYAN);
    tft.drawString(">>", 277, 30, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("PS:", 15, 50, 2);
    tft.setTextColor(TFT_YELLOW);
    String ps6 = getEstacao(ptyStationIndex).ps;
    if (ps6.length() > 20) ps6 = ps6.substring(0, 20);
    tft.drawString(ps6, 60, 50, 2);
    tft.setTextColor(TFT_CYAN);
    tft.drawString(">>", 277, 50, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("RT:", 15, 70, 2);
    tft.setTextColor(TFT_YELLOW);
    String rt6 = getEstacao(ptyStationIndex).rt;
    if (rt6.length() > 20) rt6 = rt6.substring(0, 20);
    tft.drawString(rt6, 60, 70, 2);
    tft.setTextColor(TFT_CYAN);
    tft.drawString(">>", 277, 70, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("PI:", 15, 90, 2);
    tft.setTextColor(TFT_YELLOW);
    String piStr6 = String(piCode6, HEX); piStr6.toUpperCase();
    while (piStr6.length() < 4) piStr6 = "0" + piStr6;
    tft.drawString(piStr6, 60, 90, 2);
    tft.setTextColor(TFT_CYAN);
    tft.drawString(">>", 277, 90, 2); 
    tft.setTextColor(TFT_WHITE);
    tft.drawString("RDS:", 15, 110, 2);
    bool rdsEn6 = getEstacao(ptyStationIndex).rds_ativo;
    tft.setTextColor(rdsEn6 ? TFT_GREEN : TFT_RED);
    tft.drawString(rdsEn6 ? "ATIVADO" : "DESATIVADO", 60, 110, 2);
    tft.setTextColor(TFT_CYAN);
    tft.drawString(">>", 277, 110, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Todos RDS:", 15, 150, 2);
    tft.setTextColor(TFT_CYAN);
    tft.drawString(">>", 277, 150, 2); 
    tft.setTextColor(TFT_WHITE);
    tft.drawString("LIGAR", 110, 150, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Musica:", 15, 170, 2);
    tft.setTextColor(TFT_YELLOW);
    String mus6 = getEstacao(ptyStationIndex).musica;
    if (mus6.length() > 20) mus6 = mus6.substring(0, 20);
    tft.drawString(mus6, 90, 170, 2);
    tft.setTextColor(TFT_CYAN);
    tft.drawString(">>", 277, 170, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Hora/Data/Clima:", 15, 190, 2);
    tft.setTextColor(TFT_YELLOW);
    String h6 = getEstacao(ptyStationIndex).hora + ":" + getEstacao(ptyStationIndex).minuto;
    tft.drawString(h6, 185, 190, 2);
    tft.setTextColor(TFT_SKYBLUE);
    tft.drawString(">>", 277, 190, 2);
    tft.setTextColor(TFT_SKYBLUE);
    tft.drawString(getUIString(UI_BACK, languageSet), 15, 130, 2);
    tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_WHITE);
  } else if (menuPage == 8) {
    if (ptyStationIndex >= (int)totalEstacoes()) { menuPage = 6; menuoption = 30; BuildMenu(); return; }
    Estacao& e8 = getEstacao(ptyStationIndex);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Hora:", 15, 30, 2);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString(e8.hora + ":" + e8.minuto, 90, 30, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Dia:", 15, 50, 2);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString(e8.dia + "/" + e8.mes + "/" + e8.ano, 90, 50, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Minuto:", 15, 70, 2);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString(e8.minuto, 90, 70, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Mes:", 15, 90, 2);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString(e8.mes, 90, 90, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Tempo:", 15, 110, 2);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString(e8.tempo, 90, 110, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Temp:", 15, 130, 2);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString(e8.temperatura, 90, 130, 2);
    tft.setTextColor(TFT_SKYBLUE);
    tft.drawString(getUIString(UI_BACK, languageSet), 15, 150, 2);
    tft.drawRoundRect(10, menuoption, 300, 18, 5, TFT_WHITE);
  }
  analogWrite(SMETERPIN, 0);
}

void MuteScreen(int setting) {
  if (setting == 0) {
    screenmute = 0;
    setupmode = true;
    BuildDisplay();
    setupmode = false;
  } else {
    screenmute = 1;
    tft.fillScreen(TFT_BLACK);
    tft.drawRoundRect(0, 0, 319, 239, 3, UI_BORDER_COLOR);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString(getUIString(UI_SCREEN_MUTED, languageSet), 160, 30, 4);
    tft.drawCentreString("To unmute uncheck RF+ box", 160, 60, 2);
  }
}

const char* getThemeName(uint16_t t) {
  switch (t) {
    case 1:  return "Vermelho";
    case 2:  return "Brasil";
    case 3:  return "Noturno";
    case 4:  return "Amber";
    case 5:  return "Verde";
    case 6:  return "Roxo";
    case 7:  return "Azul";
    case 8:  return "PE5PVB";
    case 9:  return "Ciano";
    case 10: return "Vulcao";
    case 11: return "Sakura";
    case 12: return "Oceano";
    case 13: return "Tangerina";
    case 14: return "Carmesim";
    case 15: return "Dendro";
    case 16: return "Monocromo";
    case 17: return "Indigo";
    case 18: return "Queer";
    case 19: return "GoldBrite";
    case 20: return "Bubblegum";
    case 21: return "Larico";
    case 22: return "Esmeralda";
    case 23: return "Artico";
    case 24: return "Crepusculo";
    case 25: return "Turquesa";
    case 26: return "Ametista";
    case 27: return "Ferrugem";
    case 28: return "Neon";
    case 29: return "Prata";
    case 30: return "Aurora";
    case 31: return "Lavanda";
    case 32: return "Caramelo";
    case 33: return "Menta";
    case 34: return "Tomate";
    case 35: return "Safira";
    case 36: return "Limoeiro";
    case 37: return "Marinho";
    case 38: return "Carmim";
    case 39: return "Celeste";
    case 40: return "Abacate";
    case 41: return "Magenta";
    case 42: return "Brasa";
    case 43: return "Ardosia";
    case 44: return "Cereja";
    case 45: return "Petroleo";
    case 46: return "Cobalto";
    case 47: return "Mel";
    case 48: return "Vinho";
    case 49: return "Acqua";
    case 50: return "Cobre";
    case 51: return "Algodao";
    case 52: return "Musgo";
    case 53: return "Oxido";
    case 54: return "Manga";
    case 55: return "Uva";
    case 56: return "Maracuja";
    case 57: return "Grafite";
    case 58: return "Champanhe";
    case 59: return "Coral";
    case 60: return "Fantasma";
    case 61: return "Sertao";
    case 62: return "Essence";
    case 63: return "Cyan";
    case 64: return "Crimson";
    case 65: return "Monochrome";
    case 66: return "Volcano";
    case 67: return "Dendro";
    case 68: return "Sakura";
    case 69: return "Whiteout";
    case 70: return "Tangerine";
    case 71: return "Ocean";
    case 72: return "Indigo";
    case 73: return "Queer";
    case 74: return "GoldBrite";
    case 75: return "Bubblegum";
    case 76: return "Essence II";
    case 77: return "777";
    case 78: return "Vinil";
    case 79: return "Ouro";
    case 80: return "Anos 80";
    case 81: return "Plasma";
    case 82: return "Nebulosa";
    case 83: return "Fumaca";
    case 84: return "Arco-Iris";
    case 85: return "Petala";
    case 86: return "Relampago";
    case 87: return "Abismo";
    case 88: return "Cristal";
    case 89: return "Lava";
    case 90: return "Pixel";
    case 91: return "Bosque";
    case 92: return "Nuclear";
    case 93: return "Netuno";
    case 94: return "Plutao";
    case 95: return "America";
    case 96: return "Curie";
    case 97:  return "Berkeley";
    // Elements 98-118
    case 98:  return "Californio";
    case 99:  return "Einstein";
    case 100: return "Fermio";
    case 101: return "Mendelevio";
    case 102: return "Nobel";
    case 103: return "Laurencio";
    case 104: return "Rutherford";
    case 105: return "Dubnio";
    case 106: return "Seaborg";
    case 107: return "Bohrio";
    case 108: return "Hassio";
    case 109: return "Meitner";
    case 110: return "Darmstadt";
    case 111: return "Roentgen";
    case 112: return "Copernicus";
    case 113: return "Nihonio";
    case 114: return "Flerov";
    case 115: return "Moscovio";
    case 116: return "Livermore";
    case 117: return "Tennessio";
    case 118: return "Oganesson";
    // Brazilian
    case 119: return "Fortaleza";
    case 120: return "Salvador";
    case 121: return "Amazonia";
    case 122: return "Pantanal";
    case 123: return "Caatinga";
    case 124: return "Cerrado";
    case 125: return "Samba";
    case 126: return "Carnaval";
    case 127: return "Frevo";
    case 128: return "Forro";
    case 129: return "BossaNova";
    case 130: return "Tropicalia";
    case 131: return "Copa";
    case 132: return "Pele";
    case 133: return "Senna";
    case 134: return "Acai";
    case 135: return "Guarana";
    case 136: return "Cafe";
    case 137: return "Cachaca";
    case 138: return "Tapioca";
    case 139: return "Caju";
    case 140: return "Jabuticaba";
    // Cultural
    case 141: return "Jazz";
    case 142: return "Punk";
    case 143: return "Grunge";
    case 144: return "Reggae";
    case 145: return "Funk";
    case 146: return "Rock";
    case 147: return "Anime";
    case 148: return "Manga2";
    case 149: return "Cyberpunk";
    case 150: return "Steampunk";
    case 151: return "Gothic";
    case 152: return "ArtDeco";
    case 153: return "Bauhaus";
    case 154: return "Vaporwave";
    case 155: return "Lofi";
    case 156: return "Y2K";
    case 157: return "Memphis";
    case 158: return "Minimal";
    case 159: return "Nordic";
    case 160: return "Tropical";
    case 161: return "Safari";
    case 162: return "Arctic";
    case 163: return "Desert";
    case 164: return "Monsoon";
    case 165: return "Midnight2";
    // Gems
    case 166: return "Rubi";
    case 167: return "Topazio";
    case 168: return "Opala";
    case 169: return "Jade";
    case 170: return "Onix";
    case 171: return "Perola";
    case 172: return "Diamante";
    case 173: return "Aquamarine";
    case 174: return "Citrino";
    case 175: return "Granada";
    case 176: return "Lazuli";
    case 177: return "Malaquita";
    case 178: return "Turmalina";
    case 179: return "Alexandrita";
    case 180: return "Tanzanita";
    case 181: return "Zircao";
    case 182: return "Espinelio";
    case 183: return "Kunzita";
    // Elements / concepts
    case 184: return "Heliodoro";
    case 185: return "Bicolor";
    case 186: return "RetroCRT";
    case 187: return "Futuro";
    case 188: return "Espaco";
    case 189: return "Terra";
    case 190: return "Fogo";
    case 191: return "Agua";
    case 192: return "Ar";
    case 193: return "Metal";
    case 194: return "Madeira";
    case 195: return "Pedra";
    case 196: return "Gelo";
    case 197: return "Raio";
    case 198: return "Aurora";
    case 199: return "Vulcao";
    case 200: return "Cosmos";
    case 201: return "Lua";
    case 202: return "Sol";
    case 203: return "Oceano";
    case 204: return "Floresta";
    case 205: return "Deserto";
    case 206: return "Neve";
    case 207: return "Tempestade";
    case 208: return "Crepusculo";
    case 209: return "Manha";
    case 210: return "Noite";
    case 211: return "Bruma";
    case 212: return "Pantanal";
    case 213: return "Amazonia";
    case 214: return "Cerrado";
    case 215: return "Caatinga";
    case 216: return "Pampa";
    case 217: return "Mangue";
    case 218: return "Restinga";
    case 219: return "MataAtl";
    case 220: return "Recife";
    case 221: return "Dunas";
    case 222: return "Litoral";
    case 223: return "Sertao";
    case 224: return "Planalto";
    case 225: return "Varzea";
    case 226: return "Gruta";
    case 227: return "Cachoeira";
    case 228: return "Granito";
    case 229: return "Basalto";
    case 230: return "Obsidiana";
    case 231: return "Marmore";
    case 232: return "Cristal";
    case 233: return "Quartzo";
    case 234: return "Safira";
    case 235: return "Ametista";
    case 236: return "Esmeralda";
    case 237: return "Rubi";
    case 238: return "Turquesa";
    case 239: return "Coral";
    case 240: return "Perola";
    case 241: return "Onix";
    case 242: return "Jaspe";
    case 243: return "Citrino";
    case 244: return "Labradorita";
    case 245: return "Selenita";
    case 246: return "Pirita";
    case 247: return "Malaquita";
    case 248: return "Lapis";
    case 249: return "Amazonita";
    case 250: return "Fluorita";
    case 251: return "Calcita";
    case 252: return "Aventurina";
    case 253: return "Rodolita";
    case 254: return "Tanzanita";
    case 255: return "Diamante";
    case 256: return "Prisma";
    case 257: return "Nebula";
    case 258: return "Galaxia";
    case 259: return "RockNac";
    case 260: return "Axe";
    case 261: return "Sertanejo";
    case 262: return "Pagode";
    case 263: return "MPB";
    case 264: return "Funk";
    case 265: return "Brega";
    case 266: return "Maracatu";
    case 267: return "Baiao";
    case 268: return "Choro";
    case 269: return "Xote";
    case 270: return "Piseiro";
    case 271: return "Arrocha";
    case 272: return "Tecnobrega";
    case 273: return "Lambada";
    case 274: return "Embolada";
    case 275: return "Repente";
    case 276: return "Carimbo";
    case 277: return "Baile";
    case 278: return "Samba";
    case 279: return "Frevo";
    case 280: return "Coco";
    // ── Brazilian cities 281-290 ─────────────────────────────
    case 281: return "Manaus";
    case 282: return "Belem";
    case 283: return "Curitiba";
    case 284: return "PortoAlegre";
    case 285: return "Brasilia";
    case 286: return "Floripa";
    case 287: return "Natal";
    case 288: return "Maceio";
    case 289: return "Aracaju";
    case 290: return "Teresina";
    // ── Art styles 291-300 ───────────────────────────────────
    case 291: return "Impressao";
    case 292: return "Cubismo";
    case 293: return "Surrealismo";
    case 294: return "Expressao";
    case 295: return "PopArt";
    case 296: return "Pointilismo";
    case 297: return "Futurismo";
    case 298: return "Romantico";
    case 299: return "Barroco";
    case 300: return "Concreto";
    // ── Technology / digital 301-310 ─────────────────────────
    case 301: return "Matrix2";
    case 302: return "Holo";
    case 303: return "Retro8bit";
    case 304: return "Terminal";
    case 305: return "Cyber2";
    case 306: return "Glitch";
    case 307: return "Pixel2";
    case 308: return "Neon2";
    case 309: return "Binary";
    case 310: return "Hologram";
    // ── Weather / atmosphere 311-320 ─────────────────────────
    case 311: return "Vendaval";
    case 312: return "Nevoeiro";
    case 313: return "Arcoiris2";
    case 314: return "Trovao";
    case 315: return "Brisa";
    case 316: return "Nevoa";
    case 317: return "Seco";
    case 318: return "Humido";
    case 319: return "Calor";
    case 320: return "Frio";
    // ── Brazilian foods & drinks 321-330 ─────────────────────
    case 321: return "Acai2";
    case 322: return "Coxinha";
    case 323: return "Brigadeiro";
    case 324: return "Caipirinha";
    case 325: return "Mangaba";
    case 326: return "Cupuacu";
    case 327: return "Murici";
    case 328: return "Pitanga";
    case 329: return "Jenipapo";
    case 330: return "Buriti";
    case 331: return "Junino";
    // ── Seasons & time of day 332-341 ────────────────────────
    case 332: return "Primavera";
    case 333: return "Verao";
    case 334: return "Outono";
    case 335: return "Inverno";
    case 336: return "Amanhecer";
    case 337: return "Manha2";
    case 338: return "Meio-Dia";
    case 339: return "Tarde";
    case 340: return "Entardecer";
    case 341: return "Madrugada";
    // ── Brazilian folklore 342-351 ────────────────────────────
    case 342: return "Iara";
    case 343: return "Curupira";
    case 344: return "Cuca";
    case 345: return "Boto";
    case 346: return "Saci";
    case 347: return "Mapinguari";
    case 348: return "CobraGrande";
    case 349: return "Matinta";
    case 350: return "Lobisomem";
    case 351: return "Bumba";
    // ── Ocean / sea 352-361 ───────────────────────────────────
    case 352: return "Maresia";
    case 353: return "Onda";
    case 354: return "Fundo";
    case 355: return "Peixe";
    case 356: return "Coral3";
    case 357: return "Alga";
    case 358: return "Areia";
    case 359: return "Mare";
    case 360: return "Nautico";
    case 361: return "Futebol";
    // ── Sports 362-370 ────────────────────────────────────────
    case 362: return "Volei";
    case 363: return "Surfe";
    case 364: return "MMA";
    case 365: return "Corrida";
    case 366: return "Natacao";
    case 367: return "Ciclismo";
    case 368: return "Boxe";
    case 369: return "Basquete";
    case 370: return "Tenis";
    // ── Flowers 371-381 ──────────────────────────────────────
    case 371: return "Rosa2";
    case 372: return "Girassol";
    case 373: return "Orquidea";
    case 374: return "Ipe";
    case 375: return "VitoriaRegia";
    case 376: return "Bromelia";
    case 377: return "Heliconia";
    case 378: return "Passiflora";
    case 379: return "Cravinho";
    case 380: return "Castanha";
    case 381: return "Xaxado";
    case 382: return "Cordel";
    // ── Instruments 383-399 ──────────────────────────────────
    case 383: return "Viola"; case 384: return "Sanfona"; case 385: return "Cavaquinho";
    case 386: return "Pandeiro"; case 387: return "Zabumba"; case 388: return "Berimbau";
    case 389: return "Atabaque"; case 390: return "Cuica"; case 391: return "Agogo";
    case 392: return "Tamborim"; case 393: return "Ganza"; case 394: return "Reco";
    case 395: return "Bandolim"; case 396: return "Ukulele"; case 397: return "Lira";
    case 398: return "Harpa"; case 399: return "Trompa";
    // ── Gemstones 400-449 ────────────────────────────────────
    case 400: return "Rubi2"; case 401: return "Safira2"; case 402: return "Esmeralda2";
    case 403: return "Topazio2"; case 404: return "Ametista2"; case 405: return "Citrino2";
    case 406: return "Granada2"; case 407: return "Turmalina2"; case 408: return "Opala2";
    case 409: return "Pérola2"; case 410: return "Coral4"; case 411: return "Âmbar";
    case 412: return "Jade2"; case 413: return "Lápis2"; case 414: return "Turquesa2";
    case 415: return "Malaquita2"; case 416: return "Obsidiana2"; case 417: return "Granito2";
    case 418: return "Quartzo2"; case 419: return "Feldspato"; case 420: return "Calcita2";
    case 421: return "Selenita2"; case 422: return "Pirita2"; case 423: return "Fluorita2";
    case 424: return "Talco"; case 425: return "Mica"; case 426: return "Feldsparra";
    case 427: return "Berilo"; case 428: return "Zoisita"; case 429: return "Espinela";
    case 430: return "Enstatita"; case 431: return "Staurolita"; case 432: return "Andaluzita";
    case 433: return "Epidoto"; case 434: return "Turmalina3"; case 435: return "Dumortierita";
    case 436: return "Indica"; case 437: return "Fuchsita"; case 438: return "Amazonita2";
    case 439: return "Aventurina2"; case 440: return "Heliotropio"; case 441: return "Jaspe2";
    case 442: return "Agata"; case 443: return "Calcedonia"; case 444: return "Onix2";
    case 445: return "Sardonix"; case 446: return "Cornalina"; case 447: return "Sardonica";
    case 448: return "Cristal2"; case 449: return "Vidro";
    // ── Planets & Space 450-499 ──────────────────────────────
    case 450: return "Mercurio"; case 451: return "Venus"; case 452: return "Terra2";
    case 453: return "Marte"; case 454: return "Jupiter"; case 455: return "Saturno";
    case 456: return "Urano"; case 457: return "Netuno"; case 458: return "Plutao2";
    case 459: return "Sol2"; case 460: return "Lua2"; case 461: return "Orion";
    case 462: return "Sirius"; case 463: return "Vega"; case 464: return "Antares";
    case 465: return "Betelgeuse"; case 466: return "Rigel"; case 467: return "Aldebaran";
    case 468: return "Polaris"; case 469: return "CruzSul"; case 470: return "Andromeda";
    case 471: return "Triangulo"; case 472: return "Perseu"; case 473: return "Cassiopeia";
    case 474: return "Dragao"; case 475: return "Ursa"; case 476: return "Leao";
    case 477: return "Escorpiao"; case 478: return "Sagitario"; case 479: return "Capricornio";
    case 480: return "Aquario"; case 481: return "Peixes"; case 482: return "Aries";
    case 483: return "Touro"; case 484: return "Gemeos"; case 485: return "Cancer";
    case 486: return "Virgem"; case 487: return "Libra"; case 488: return "Serpentario";
    case 489: return "Cometa"; case 490: return "Asteroide"; case 491: return "Eclipse";
    case 492: return "Aurora2"; case 493: return "Zenit"; case 494: return "Nadir";
    case 495: return "Afelio"; case 496: return "Perihelio"; case 497: return "Apogeu";
    case 498: return "Perigeu"; case 499: return "Equinoxio";
    // ── Animals 500-549 ───────────────────────────────────────
    case 500: return "Jaguar"; case 501: return "Onca"; case 502: return "Puma";
    case 503: return "Urso"; case 504: return "Lobo"; case 505: return "Raposa";
    case 506: return "Leao2"; case 507: return "Tigre"; case 508: return "Leopardo";
    case 509: return "Gato"; case 510: return "Cachorro"; case 511: return "Cavalo";
    case 512: return "Zebra"; case 513: return "Girafa"; case 514: return "Elefante";
    case 515: return "Rinoceronte"; case 516: return "Hipopotamo"; case 517: return "Crocodilo";
    case 518: return "Jacare"; case 519: return "Serpente"; case 520: return "Cobra";
    case 521: return "Sapo"; case 522: return "Rã"; case 523: return "Tartaruga";
    case 524: return "Aquila"; case 525: return "Falcao"; case 526: return "Coruja";
    case 527: return "Corvo"; case 528: return "Papagaio"; case 529: return "Colibri";
    case 530: return "Passaro"; case 531: return "Golfinho"; case 532: return "Baleia";
    case 533: return "Tubarao"; case 534: return "Peixe2"; case 535: return "Caranguejo";
    case 536: return "Aranha"; case 537: return "Borboleta"; case 538: return "Abelha";
    case 539: return "Formiga"; case 540: return "Gafanhoto"; case 541: return "Libélula";
    case 542: return "Vaga-Lume"; case 543: return "Minhoca"; case 544: return "Polvo";
    case 545: return "Lula"; case 546: return "Ostras"; case 547: return "Molusco";
    case 548: return "Nematodeo"; case 549: return "Rotifero";
    // ── Trees 550-599 ────────────────────────────────────────
    case 550: return "Jatoba"; case 551: return "Mogno"; case 552: return "Cedro";
    case 553: return "Pau-Brasil"; case 554: return "Jacaranda"; case 555: return "Angico";
    case 556: return "Paineira"; case 557: return "Oiti"; case 558: return "Maçaranduba";
    case 559: return "Graúna"; case 560: return "Pau-D-Arco"; case 561: return "Peroba";
    case 562: return "Cambara"; case 563: return "Guatambu"; case 564: return "Sapucaia";
    case 565: return "Cumaru"; case 566: return "Sucupira"; case 567: return "Mirindiba";
    case 568: return "Tabeuia"; case 569: return "Embauba"; case 570: return "Mulungu";
    case 571: return "Baobá"; case 572: return "Sumauma"; case 573: return "Andiroba";
    case 574: return "Copaiba"; case 575: return "Guarana"; case 576: return "Cacao";
    case 577: return "Café2"; case 578: return "Seringa"; case 579: return "Açaí3";
    case 580: return "Buriti2"; case 581: return "Macaúba"; case 582: return "Pupunha";
    case 583: return "Dendê"; case 584: return "Bacaba"; case 585: return "Pindoba";
    case 586: return "Tucumã"; case 587: return "Assaí"; case 588: return "Palmiteiro";
    case 589: return "Carnaúba"; case 590: return "Babassú"; case 591: return "Tucandeira";
    case 592: return "Mirtáceas"; case 593: return "Myrcia"; case 594: return "Psidium";
    case 595: return "Eugenia"; case 596: return "Campomanesia"; case 597: return "Plinia";
    case 598: return "Syzygium"; case 599: return "Myrtaceae";
    // ── Spices & Herbs 600-649 ───────────────────────────────
    case 600: return "Canela"; case 601: return "Cravo"; case 602: return "Noz-Moscada";
    case 603: return "Pimenta"; case 604: return "Gengibre"; case 605: return "Curcuma";
    case 606: return "Cominho"; case 607: return "Cardamomo"; case 608: return "Anis";
    case 609: return "Funcho"; case 610: return "Louro"; case 611: return "Oregano";
    case 612: return "Manjericao"; case 613: return "Salsa"; case 614: return "Cebolinha";
    case 615: return "Coentro"; case 616: return "Alecrim"; case 617: return "Tomilho";
    case 618: return "Hortelã"; case 619: return "Melissa"; case 620: return "Capim-Limao";
    case 621: return "Gengibre2"; case 622: return "Hibisco"; case 623: return "Camomila";
    case 624: return "Maracuja2"; case 625: return "Passiflora2"; case 626: return "Melisomataceae";
    case 627: return "Verbena"; case 628: return "Calêndula"; case 629: return "Camomila2";
    case 630: return "Mil-Folhas"; case 631: return "Tanchagem"; case 632: return "Gengibre3";
    case 633: return "Cavalinha"; case 634: return "Urtiga"; case 635: return "Pariparoba";
    case 636: return "Jurubeba"; case 637: return "Espinheira-Santa"; case 638: return "Boldo";
    case 639: return "Guaçatonga"; case 640: return "Sete-Sangrias"; case 641: return "Carqueja";
    case 642: return "Transagem"; case 643: return "Confrei"; case 644: return "Pulmonaria";
    case 645: return "Verbascum"; case 646: return "Scrophularia"; case 647: return "Digitalis";
    case 648: return "Symphytum"; case 649: return "Plantago";
    // ── Dances 650-699 ────────────────────────────────────────
    case 650: return "Samba2"; case 651: return "Frevo2"; case 652: return "Forró2";
    case 653: return "Baiao2"; case 654: return "Xote2"; case 655: return "Arrocha2";
    case 656: return "Lambada2"; case 657: return "Merengue"; case 658: return "Bachata";
    case 659: return "Salsa2"; case 660: return "Mambo"; case 661: return "Rumba";
    case 662: return "Cha-Cha"; case 663: return "Tango2"; case 664: return "Valsa";
    case 665: return "Polca"; case 666: return "Maxixe"; case 667: return "Modinha";
    case 668: return "Lundu"; case 669: return "Jongo"; case 670: return "Maculele";
    case 671: return "Capoeira2"; case 672: return "Frevo-Dança"; case 673: return "Ciranda";
    case 674: return "Brincadeiras"; case 675: return "Congada"; case 676: return "Festa-Santo";
    case 677: return "Reisado"; case 678: return "Cavalhada"; case 679: return "Bumba2";
    case 680: return "Marujada"; case 681: return "Nau-Catarineta"; case 682: return "Pastoril";
    case 683: return "Autos"; case 684: return "Samba-Enredo"; case 685: return "Marchinha";
    case 686: return "Samba-Canção"; case 687: return "Samba-Exaltação"; case 688: return "Pagode2";
    case 689: return "Funk2"; case 690: return "Forró-Eletrônico"; case 691: return "Piseiro2";
    case 692: return "Arrocha3"; case 693: return "Brega2"; case 694: return "Tecnobrega2";
    case 695: return "Carimbó"; case 696: return "Siriri"; case 697: return "Quadrilha";
    case 698: return "Catira"; case 699: return "Sapato";
    // ── Poets & Writers 700-749 ──────────────────────────────
    case 700: return "Camoes"; case 701: return "Bartolomeu"; case 702: return "Pero-Vaz";
    case 703: return "Sousa"; case 704: return "Natividade"; case 705: return "Gregório";
    case 706: return "Cruz"; case 707: return "Botelho"; case 708: return "Manuel";
    case 709: return "Durão"; case 710: return "Basilio"; case 711: return "Magalhaes";
    case 712: return "Alves"; case 713: return "Alencar"; case 714: return "Macedo";
    case 715: return "Assis"; case 716: return "Quincas"; case 717: return "Bilac";
    case 718: return "Pessoa"; case 719: return "Bandeira"; case 720: return "Andrade";
    case 721: return "Drummond"; case 722: return "Cabral"; case 723: return "Buarque";
    case 724: return "Espinosa"; case 725: return "Allegra"; case 726: return "Sabino";
    case 727: return "Lins"; case 728: return "Rosa"; case 729: return "Clarice";
    case 730: return "Castro"; case 731: return "Beatriz"; case 732: return "Nunes";
    case 733: return "Araujo"; case 734: return "Lobo"; case 735: return "Andrada";
    case 736: return "Melo"; case 737: return "Correia"; case 738: return "Souza";
    case 739: return "Neves"; case 740: return "Miranda"; case 741: return "Ribeiro";
    case 742: return "Ferreira"; case 743: return "Gomes"; case 744: return "Oliveira";
    case 745: return "Martins"; case 746: return "Brant"; case 747: return "Orofino";
    case 748: return "Machete"; case 749: return "Sampa";
    // ── Cocktails & Drinks 750-799 ───────────────────────────
    case 750: return "Caipirinha2"; case 751: return "Mojito"; case 752: return "Margarita";
    case 753: return "Sangria"; case 754: return "Pisco"; case 755: return "Daiquiri";
    case 756: return "Cuba-Libre"; case 757: return "Mojito2"; case 758: return "Cosmopolitan";
    case 759: return "Martini"; case 760: return "Manhattan"; case 761: return "Sidecar";
    case 762: return "Sazerac"; case 763: return "Whiskey-Sour"; case 764: return "Mint-Julep";
    case 765: return "Tom-Collins"; case 766: return "Gimlet"; case 767: return "Negroni";
    case 768: return "Vermouth"; case 769: return "Champagne"; case 770: return "Prosecco";
    case 771: return "Espumante"; case 772: return "Vinho-Tinto"; case 773: return "Vinho-Branco";
    case 774: return "Cerveja"; case 775: return "Cerveja-Clara"; case 776: return "Cerveja-Escura";
    case 777: return "Chope"; case 778: return "Aguardente"; case 779: return "Genipapo";
    case 780: return "Licor"; case 781: return "Xérès"; case 782: return "Madeira";
    case 783: return "Porto"; case 784: return "Brandy"; case 785: return "Cognac";
    case 786: return "Armagnac"; case 787: return "Absinto"; case 788: return "Ouzo";
    case 789: return "Anis"; case 790: return "Raki"; case 791: return "Agua-Ardente";
    case 792: return "Conhaque"; case 793: return "Licor-Gengibre"; case 794: return "Licor-Mel";
    case 795: return "Licor-Amora"; case 796: return "Licor-Morango"; case 797: return "Licor-Cereja";
    case 798: return "Licor-Menta"; case 799: return "Licor-Cafe";
    // ── Brazilian States 800-849 ──────────────────────────────
    case 800: return "Acre"; case 801: return "Alagoas"; case 802: return "Amapa";
    case 803: return "Amazonas"; case 804: return "Bahia"; case 805: return "Ceara";
    case 806: return "Distrito-Federal"; case 807: return "Espirito-Santo"; case 808: return "Goias";
    case 809: return "Maranhao"; case 810: return "Mato-Grosso"; case 811: return "Mato-Grosso-Sul";
    case 812: return "Minas-Gerais"; case 813: return "Para"; case 814: return "Paraiba";
    case 815: return "Parana"; case 816: return "Pernambuco"; case 817: return "Piaui";
    case 818: return "Rio-Janeiro"; case 819: return "Rio-Grande-Norte"; case 820: return "Rio-Grande-Sul";
    case 821: return "Rondonia"; case 822: return "Roraima"; case 823: return "Santa-Catarina";
    case 824: return "Sao-Paulo"; case 825: return "Sergipe"; case 826: return "Tocantins";
    case 827: return "Ouro-Preto"; case 828: return "Diamantina"; case 829: return "Mariana";
    case 830: return "Sabara"; case 831: return "Congonhas"; case 832: return "Sao-Joao"; case 833: return "Tiradentes";
    case 834: return "Petropolis"; case 835: return "Paraty"; case 836: return "Buzios";
    case 837: return "Angra"; case 838: return "Itanhanda"; case 839: return "Ouro-Fino";
    case 840: return "Pouso-Alegre"; case 841: return "Lavras"; case 842: return "Montes-Claros";
    case 843: return "Uberaba"; case 844: return "Uberlandia"; case 845: return "Araxá";
    case 846: return "Divinópolis"; case 847: return "Ipatinga"; case 848: return "Coronel";
    case 849: return "Três-Corações";
    // ── Architectural Styles 850-899 ──────────────────────────
    case 850: return "Colonial"; case 851: return "Barroco2"; case 852: return "Rococo";
    case 853: return "Neoclassico"; case 854: return "Vitoriano"; case 855: return "Art-Nouveau";
    case 856: return "Art-Deco2"; case 857: return "Modernista"; case 858: return "Brutalista";
    case 859: return "Organico"; case 860: return "Estruturalista"; case 861: return "Desconstrutivista";
    case 862: return "Parametrico"; case 863: return "Minimalista2"; case 864: return "Maximalista";
    case 865: return "Bohemio"; case 866: return "Shabby-Chic"; case 867: return "Hygge";
    case 868: return "Wabi-Sabi"; case 869: return "Escandinavo"; case 870: return "Japones";
    case 871: return "Chinês"; case 872: return "Indiano"; case 873: return "Marroquino";
    case 874: return "Otomano"; case 875: return "Persa"; case 876: return "Arabesco";
    case 877: return "Bissantino"; case 878: return "Gotico2"; case 879: return "Romano";
    case 880: return "Grego"; case 881: return "Egípcio"; case 882: return "Asteca";
    case 883: return "Maia"; case 884: return "Inca"; case 885: return "Polinésio";
    case 886: return "Aboriginal"; case 887: return "Hindu"; case 888: return "Budista";
    case 889: return "Taoista"; case 890: return "Shamanista"; case 891: return "Africano";
    case 892: return "Amndio"; case 893: return "Tribal"; case 894: return "Nomade";
    case 895: return "Urbano"; case 896: return "Industrial"; case 897: return "Artesanal";
    case 898: return "Campestre"; case 899: return "Futurista2";
    // ── Textiles 900-949 ──────────────────────────────────────
    case 900: return "Algodao2"; case 901: return "Linho"; case 902: return "Lã";
    case 903: return "Seda2"; case 904: return "Cetim"; case 905: return "Veludo";
    case 906: return "Crepe"; case 907: return "Gaze"; case 908: return "Lace";
    case 909: return "Tule"; case 910: return "Organza"; case 911: return "Lamé";
    case 912: return "Brocado"; case 913: return "Damasco"; case 914: return "Jacquard";
    case 915: return "Tweed"; case 916: return "Denim"; case 917: return "Lona";
    case 918: return "Feltro"; case 919: return "Fleece"; case 920: return "Microfibra";
    case 921: return "Elastano"; case 922: return "Poliéster"; case 923: return "Acrílico";
    case 924: return "Nylon"; case 925: return "Spandex"; case 926: return "Viscose";
    case 927: return "Liocel"; case 928: return "Modal"; case 929: return "Tencel";
    case 930: return "Ramio"; case 931: return "Juta"; case 932: return "Cânhamo";
    case 933: return "Sisal"; case 934: return "Abacá"; case 935: return "Tucum";
    case 936: return "Bambu"; case 937: return "Cashmere"; case 938: return "Alpaca";
    case 939: return "Angora"; case 940: return "Mohair"; case 941: return "Camelo";
    case 942: return "Ratine"; case 943: return "Chenille"; case 944: return "Corduroy";
    case 945: return "Gabardine"; case 946: return "Popeline"; case 947: return "Organdy";
    case 948: return "Musselina"; case 949: return "Percale";
    // ── Mythology 950-999 ────────────────────────────────────
    case 950: return "Zeus"; case 951: return "Hera"; case 952: return "Poseidon";
    case 953: return "Hades"; case 954: return "Atena"; case 955: return "Ares";
    case 956: return "Afrodite"; case 957: return "Apolo"; case 958: return "Artemis";
    case 959: return "Hermes"; case 960: return "Hefesto"; case 961: return "Deméter";
    case 962: return "Dionísio"; case 963: return "Cronos"; case 964: return "Titã";
    case 965: return "Ciclope"; case 966: return "Centauro"; case 967: return "Minotauro";
    case 968: return "Pegaso"; case 969: return "Griffo"; case 970: return "Fênix";
    case 971: return "Dragão2"; case 972: return "Hidra"; case 973: return "Quimera";
    case 974: return "Esfinge"; case 975: return "Harpia"; case 976: return "Siren";
    case 977: return "Ciclopes2"; case 978: return "Titanomaquia"; case 979: return "Olimpo";
    case 980: return "Tartaro"; case 981: return "Elísio"; case 982: return "Aqueronte";
    case 983: return "Estige"; case 984: return "Flegetonte"; case 985: return "Cocito";
    case 986: return "Caronte"; case 987: return "Cérbero"; case 988: return "Fúrias";
    case 989: return "Moiras"; case 990: return "Ninfas"; case 991: return "Sátiros";
    case 992: return "Mênades"; case 993: return "Amazonas2"; case 994: return "Valquírias";
    case 995: return "Jötnar"; case 996: return "Aesir"; case 997: return "Vanir";
    case 998: return "Yggdrasil"; case 999: return "Ragnarok";
    // ── Japanese Cuisine 1000-1009 ────────────────────────────
    case 1000: return "Sushi"; case 1001: return "Ramen"; case 1002: return "Tempura";
    case 1003: return "Miso"; case 1004: return "Tofu"; case 1005: return "Sakura";
    case 1006: return "Wasabi"; case 1007: return "Nori"; case 1008: return "Edamame";
    case 1009: return "Umami";
    // ── Wine 1010-1019 ────────────────────────────────────────
    case 1010: return "Tinto"; case 1011: return "Branco"; case 1012: return "Rose";
    case 1013: return "Espumante"; case 1014: return "Reserva"; case 1015: return "Harvest";
    case 1016: return "Bordeaux"; case 1017: return "Burgundy"; case 1018: return "Champagne";
    case 1019: return "Malbec";
    // ── Coffee & Tea 1020-1029 ────────────────────────────────
    case 1020: return "Espresso"; case 1021: return "Cappuccino"; case 1022: return "Matcha";
    case 1023: return "Sencha"; case 1024: return "Oolong"; case 1025: return "Chai";
    case 1026: return "Latte"; case 1027: return "Mocaccino"; case 1028: return "Affogato";
    case 1029: return "Pu-Erh";
    // ── Rare Flowers 1030-1039 ────────────────────────────────
    case 1030: return "Rafflesia"; case 1031: return "Corpse-Flower"; case 1032: return "Ghost-Orchid";
    case 1033: return "Monkey-Orchid"; case 1034: return "Bee-Orchid"; case 1035: return "Naked-Man";
    case 1036: return "Flying-Duck"; case 1037: return "Camel-Face"; case 1038: return "Hooker-Lips";
    case 1039: return "Swaddled-Babies";
    // ── Clouds & Sky 1040-1049 ────────────────────────────────
    case 1040: return "Cumulus"; case 1041: return "Stratus"; case 1042: return "Cirrus";
    case 1043: return "Altocumulus"; case 1044: return "Stratocumulus"; case 1045: return "Lenticular";
    case 1046: return "Noctilucent"; case 1047: return "Mammatus"; case 1048: return "Cumulonimbus";
    case 1049: return "Contrails";
    // ── Precious Metals 1050-1059 ──────────────────────────────
    case 1050: return "Ouro"; case 1051: return "Prata"; case 1052: return "Platinum";
    case 1053: return "Palladium"; case 1054: return "Iridium"; case 1055: return "Rodio";
    case 1056: return "Rutenio"; case 1057: return "Osmio"; case 1058: return "Rhenio";
    case 1059: return "Torio";
    // ── Glass & Crystal 1060-1069 ──────────────────────────────
    case 1060: return "Cristal-Bohemio"; case 1061: return "Cristal-Swarovski"; case 1062: return "Cristal-Waterford";
    case 1063: return "Vidro-Murano"; case 1064: return "Vidro-Venetian"; case 1065: return "Vidro-Bohemian";
    case 1066: return "Prisma"; case 1067: return "Espelho"; case 1068: return "Obsidian";
    case 1069: return "Vidro-Puro";
    // ── Perfumes 1070-1079 ─────────────────────────────────────
    case 1070: return "Chanel-5"; case 1071: return "Joy"; case 1072: return "Opium";
    case 1073: return "Poison"; case 1074: return "Eternity"; case 1075: return "Obsession";
    case 1076: return "Diorissimo"; case 1077: return "Coco"; case 1078: return "Arpege";
    case 1079: return "Mitsouko";
    // ── Japanese Silks 1080-1089 ───────────────────────────────
    case 1080: return "Kimono"; case 1081: return "Yuzen"; case 1082: return "Shibori";
    case 1083: return "Kasuri"; case 1084: return "Ikat"; case 1085: return "Damask";
    case 1086: return "Brocade"; case 1087: return "Jacquard"; case 1088: return "Taffeta";
    case 1089: return "Charmeuse";
    // ── Jewelry & Piercing 1090-1099 ────────────────────────────
    case 1090: return "Diamante"; case 1091: return "Ouro-Rose"; case 1092: return "Titanio";
    case 1093: return "Platina"; case 1094: return "Prata-Sterlin"; case 1095: return "Opal";
    case 1096: return "Jade"; case 1097: return "Turquesa"; case 1098: return "Amatista";
    case 1099: return "Obsidiana-Negra";
    // ── Constellations 1100-1199 ──────────────────────────────
    case 1100: case 1101: case 1102: case 1103: case 1104: case 1105: case 1106: case 1107: case 1108: case 1109:
    case 1110: case 1111: case 1112: case 1113: case 1114: case 1115: case 1116: case 1117: case 1118: case 1119:
    case 1120: case 1121: case 1122: case 1123: case 1124: case 1125: case 1126: case 1127: case 1128: case 1129:
    case 1130: case 1131: case 1132: case 1133: case 1134: case 1135: case 1136: case 1137: case 1138: case 1139:
    case 1140: case 1141: case 1142: case 1143: case 1144: case 1145: case 1146: case 1147: case 1148: case 1149:
    case 1150: case 1151: case 1152: case 1153: case 1154: case 1155: case 1156: case 1157: case 1158: case 1159:
    case 1160: case 1161: case 1162: case 1163: case 1164: case 1165: case 1166: case 1167: case 1168: case 1169:
    case 1170: case 1171: case 1172: case 1173: case 1174: case 1175: case 1176: case 1177: case 1178: case 1179:
    case 1180: case 1181: case 1182: case 1183: case 1184: case 1185: case 1186: case 1187: case 1188: case 1189:
    case 1190: case 1191: case 1192: case 1193: case 1194: case 1195: case 1196: case 1197: case 1198: case 1199:
      return "Constellation";
    // ── Philosophers 1200-1299 ────────────────────────────────
    case 1200: case 1201: case 1202: case 1203: case 1204: case 1205: case 1206: case 1207: case 1208: case 1209:
    case 1210: case 1211: case 1212: case 1213: case 1214: case 1215: case 1216: case 1217: case 1218: case 1219:
    case 1220: case 1221: case 1222: case 1223: case 1224: case 1225: case 1226: case 1227: case 1228: case 1229:
    case 1230: case 1231: case 1232: case 1233: case 1234: case 1235: case 1236: case 1237: case 1238: case 1239:
    case 1240: case 1241: case 1242: case 1243: case 1244: case 1245: case 1246: case 1247: case 1248: case 1249:
    case 1250: case 1251: case 1252: case 1253: case 1254: case 1255: case 1256: case 1257: case 1258: case 1259:
    case 1260: case 1261: case 1262: case 1263: case 1264: case 1265: case 1266: case 1267: case 1268: case 1269:
    case 1270: case 1271: case 1272: case 1273: case 1274: case 1275: case 1276: case 1277: case 1278: case 1279:
    case 1280: case 1281: case 1282: case 1283: case 1284: case 1285: case 1286: case 1287: case 1288: case 1289:
    case 1290: case 1291: case 1292: case 1293: case 1294: case 1295: case 1296: case 1297: case 1298: case 1299:
      return "Philosopher";
    // ── Composers 1300-1399 ───────────────────────────────────
    case 1300: case 1301: case 1302: case 1303: case 1304: case 1305: case 1306: case 1307: case 1308: case 1309:
    case 1310: case 1311: case 1312: case 1313: case 1314: case 1315: case 1316: case 1317: case 1318: case 1319:
    case 1320: case 1321: case 1322: case 1323: case 1324: case 1325: case 1326: case 1327: case 1328: case 1329:
    case 1330: case 1331: case 1332: case 1333: case 1334: case 1335: case 1336: case 1337: case 1338: case 1339:
    case 1340: case 1341: case 1342: case 1343: case 1344: case 1345: case 1346: case 1347: case 1348: case 1349:
    case 1350: case 1351: case 1352: case 1353: case 1354: case 1355: case 1356: case 1357: case 1358: case 1359:
    case 1360: case 1361: case 1362: case 1363: case 1364: case 1365: case 1366: case 1367: case 1368: case 1369:
    case 1370: case 1371: case 1372: case 1373: case 1374: case 1375: case 1376: case 1377: case 1378: case 1379:
    case 1380: case 1381: case 1382: case 1383: case 1384: case 1385: case 1386: case 1387: case 1388: case 1389:
    case 1390: case 1391: case 1392: case 1393: case 1394: case 1395: case 1396: case 1397: case 1398: case 1399:
      return "Composer";
    // ── Painters 1400-1499 ────────────────────────────────────
    case 1400: case 1401: case 1402: case 1403: case 1404: case 1405: case 1406: case 1407: case 1408: case 1409:
    case 1410: case 1411: case 1412: case 1413: case 1414: case 1415: case 1416: case 1417: case 1418: case 1419:
    case 1420: case 1421: case 1422: case 1423: case 1424: case 1425: case 1426: case 1427: case 1428: case 1429:
    case 1430: case 1431: case 1432: case 1433: case 1434: case 1435: case 1436: case 1437: case 1438: case 1439:
    case 1440: case 1441: case 1442: case 1443: case 1444: case 1445: case 1446: case 1447: case 1448: case 1449:
    case 1450: case 1451: case 1452: case 1453: case 1454: case 1455: case 1456: case 1457: case 1458: case 1459:
    case 1460: case 1461: case 1462: case 1463: case 1464: case 1465: case 1466: case 1467: case 1468: case 1469:
    case 1470: case 1471: case 1472: case 1473: case 1474: case 1475: case 1476: case 1477: case 1478: case 1479:
    case 1480: case 1481: case 1482: case 1483: case 1484: case 1485: case 1486: case 1487: case 1488: case 1489:
    case 1490: case 1491: case 1492: case 1493: case 1494: case 1495: case 1496: case 1497: case 1498: case 1499:
      return "Painter";
    // ── Historic Cities 1500-1599 ──────────────────────────────
    case 1500: case 1501: case 1502: case 1503: case 1504: case 1505: case 1506: case 1507: case 1508: case 1509:
    case 1510: case 1511: case 1512: case 1513: case 1514: case 1515: case 1516: case 1517: case 1518: case 1519:
    case 1520: case 1521: case 1522: case 1523: case 1524: case 1525: case 1526: case 1527: case 1528: case 1529:
    case 1530: case 1531: case 1532: case 1533: case 1534: case 1535: case 1536: case 1537: case 1538: case 1539:
    case 1540: case 1541: case 1542: case 1543: case 1544: case 1545: case 1546: case 1547: case 1548: case 1549:
    case 1550: case 1551: case 1552: case 1553: case 1554: case 1555: case 1556: case 1557: case 1558: case 1559:
    case 1560: case 1561: case 1562: case 1563: case 1564: case 1565: case 1566: case 1567: case 1568: case 1569:
    case 1570: case 1571: case 1572: case 1573: case 1574: case 1575: case 1576: case 1577: case 1578: case 1579:
    case 1580: case 1581: case 1582: case 1583: case 1584: case 1585: case 1586: case 1587: case 1588: case 1589:
    case 1590: case 1591: case 1592: case 1593: case 1594: case 1595: case 1596: case 1597: case 1598: case 1599:
      return "Historic-City";
    // ── Volcanoes 1600-1699 ───────────────────────────────────
    case 1600: case 1601: case 1602: case 1603: case 1604: case 1605: case 1606: case 1607: case 1608: case 1609:
    case 1610: case 1611: case 1612: case 1613: case 1614: case 1615: case 1616: case 1617: case 1618: case 1619:
    case 1620: case 1621: case 1622: case 1623: case 1624: case 1625: case 1626: case 1627: case 1628: case 1629:
    case 1630: case 1631: case 1632: case 1633: case 1634: case 1635: case 1636: case 1637: case 1638: case 1639:
    case 1640: case 1641: case 1642: case 1643: case 1644: case 1645: case 1646: case 1647: case 1648: case 1649:
    case 1650: case 1651: case 1652: case 1653: case 1654: case 1655: case 1656: case 1657: case 1658: case 1659:
    case 1660: case 1661: case 1662: case 1663: case 1664: case 1665: case 1666: case 1667: case 1668: case 1669:
    case 1670: case 1671: case 1672: case 1673: case 1674: case 1675: case 1676: case 1677: case 1678: case 1679:
    case 1680: case 1681: case 1682: case 1683: case 1684: case 1685: case 1686: case 1687: case 1688: case 1689:
    case 1690: case 1691: case 1692: case 1693: case 1694: case 1695: case 1696: case 1697: case 1698: case 1699:
      return "Volcano";
    // ── Deserts 1700-1799 ─────────────────────────────────────
    case 1700: case 1701: case 1702: case 1703: case 1704: case 1705: case 1706: case 1707: case 1708: case 1709:
    case 1710: case 1711: case 1712: case 1713: case 1714: case 1715: case 1716: case 1717: case 1718: case 1719:
    case 1720: case 1721: case 1722: case 1723: case 1724: case 1725: case 1726: case 1727: case 1728: case 1729:
    case 1730: case 1731: case 1732: case 1733: case 1734: case 1735: case 1736: case 1737: case 1738: case 1739:
    case 1740: case 1741: case 1742: case 1743: case 1744: case 1745: case 1746: case 1747: case 1748: case 1749:
    case 1750: case 1751: case 1752: case 1753: case 1754: case 1755: case 1756: case 1757: case 1758: case 1759:
    case 1760: case 1761: case 1762: case 1763: case 1764: case 1765: case 1766: case 1767: case 1768: case 1769:
    case 1770: case 1771: case 1772: case 1773: case 1774: case 1775: case 1776: case 1777: case 1778: case 1779:
    case 1780: case 1781: case 1782: case 1783: case 1784: case 1785: case 1786: case 1787: case 1788: case 1789:
    case 1790: case 1791: case 1792: case 1793: case 1794: case 1795: case 1796: case 1797: case 1798: case 1799:
      return "Desert";
    // ── Waterfalls 1800-1899 ──────────────────────────────────
    case 1800: case 1801: case 1802: case 1803: case 1804: case 1805: case 1806: case 1807: case 1808: case 1809:
    case 1810: case 1811: case 1812: case 1813: case 1814: case 1815: case 1816: case 1817: case 1818: case 1819:
    case 1820: case 1821: case 1822: case 1823: case 1824: case 1825: case 1826: case 1827: case 1828: case 1829:
    case 1830: case 1831: case 1832: case 1833: case 1834: case 1835: case 1836: case 1837: case 1838: case 1839:
    case 1840: case 1841: case 1842: case 1843: case 1844: case 1845: case 1846: case 1847: case 1848: case 1849:
    case 1850: case 1851: case 1852: case 1853: case 1854: case 1855: case 1856: case 1857: case 1858: case 1859:
    case 1860: case 1861: case 1862: case 1863: case 1864: case 1865: case 1866: case 1867: case 1868: case 1869:
    case 1870: case 1871: case 1872: case 1873: case 1874: case 1875: case 1876: case 1877: case 1878: case 1879:
    case 1880: case 1881: case 1882: case 1883: case 1884: case 1885: case 1886: case 1887: case 1888: case 1889:
    case 1890: case 1891: case 1892: case 1893: case 1894: case 1895: case 1896: case 1897: case 1898: case 1899:
      return "Waterfall";
    // ── Caves 1900-1999 ───────────────────────────────────────
    case 1900: case 1901: case 1902: case 1903: case 1904: case 1905: case 1906: case 1907: case 1908: case 1909:
    case 1910: case 1911: case 1912: case 1913: case 1914: case 1915: case 1916: case 1917: case 1918: case 1919:
    case 1920: case 1921: case 1922: case 1923: case 1924: case 1925: case 1926: case 1927: case 1928: case 1929:
    case 1930: case 1931: case 1932: case 1933: case 1934: case 1935: case 1936: case 1937: case 1938: case 1939:
    case 1940: case 1941: case 1942: case 1943: case 1944: case 1945: case 1946: case 1947: case 1948: case 1949:
    case 1950: case 1951: case 1952: case 1953: case 1954: case 1955: case 1956: case 1957: case 1958: case 1959:
    case 1960: case 1961: case 1962: case 1963: case 1964: case 1965: case 1966: case 1967: case 1968: case 1969:
    case 1970: case 1971: case 1972: case 1973: case 1974: case 1975: case 1976: case 1977: case 1978: case 1979:
    case 1980: case 1981: case 1982: case 1983: case 1984: case 1985: case 1986: case 1987: case 1988: case 1989:
    case 1990: case 1991: case 1992: case 1993: case 1994: case 1995: case 1996: case 1997: case 1998: case 1999:
      return "Cave";
    // ── Mountains 2000-2099 ───────────────────────────────────
    case 2000: case 2001: case 2002: case 2003: case 2004: case 2005: case 2006: case 2007: case 2008: case 2009:
    case 2010: case 2011: case 2012: case 2013: case 2014: case 2015: case 2016: case 2017: case 2018: case 2019:
    case 2020: case 2021: case 2022: case 2023: case 2024: case 2025: case 2026: case 2027: case 2028: case 2029:
    case 2030: case 2031: case 2032: case 2033: case 2034: case 2035: case 2036: case 2037: case 2038: case 2039:
    case 2040: case 2041: case 2042: case 2043: case 2044: case 2045: case 2046: case 2047: case 2048: case 2049:
    case 2050: case 2051: case 2052: case 2053: case 2054: case 2055: case 2056: case 2057: case 2058: case 2059:
    case 2060: case 2061: case 2062: case 2063: case 2064: case 2065: case 2066: case 2067: case 2068: case 2069:
    case 2070: case 2071: case 2072: case 2073: case 2074: case 2075: case 2076: case 2077: case 2078: case 2079:
    case 2080: case 2081: case 2082: case 2083: case 2084: case 2085: case 2086: case 2087: case 2088: case 2089:
    case 2090: case 2091: case 2092: case 2093: case 2094: case 2095: case 2096: case 2097: case 2098: case 2099:
      return "Mountain";
    // ── Dinosaurs 2100-2199 ───────────────────────────────────
    case 2100: case 2101: case 2102: case 2103: case 2104: case 2105: case 2106: case 2107: case 2108: case 2109:
    case 2110: case 2111: case 2112: case 2113: case 2114: case 2115: case 2116: case 2117: case 2118: case 2119:
    case 2120: case 2121: case 2122: case 2123: case 2124: case 2125: case 2126: case 2127: case 2128: case 2129:
    case 2130: case 2131: case 2132: case 2133: case 2134: case 2135: case 2136: case 2137: case 2138: case 2139:
    case 2140: case 2141: case 2142: case 2143: case 2144: case 2145: case 2146: case 2147: case 2148: case 2149:
    case 2150: case 2151: case 2152: case 2153: case 2154: case 2155: case 2156: case 2157: case 2158: case 2159:
    case 2160: case 2161: case 2162: case 2163: case 2164: case 2165: case 2166: case 2167: case 2168: case 2169:
    case 2170: case 2171: case 2172: case 2173: case 2174: case 2175: case 2176: case 2177: case 2178: case 2179:
    case 2180: case 2181: case 2182: case 2183: case 2184: case 2185: case 2186: case 2187: case 2188: case 2189:
    case 2190: case 2191: case 2192: case 2193: case 2194: case 2195: case 2196: case 2197: case 2198: case 2199:
      return "Dinosaur";
    // ── Insects 2200-2299 ────────────────────────────────────
    case 2200: case 2201: case 2202: case 2203: case 2204: case 2205: case 2206: case 2207: case 2208: case 2209:
    case 2210: case 2211: case 2212: case 2213: case 2214: case 2215: case 2216: case 2217: case 2218: case 2219:
    case 2220: case 2221: case 2222: case 2223: case 2224: case 2225: case 2226: case 2227: case 2228: case 2229:
    case 2230: case 2231: case 2232: case 2233: case 2234: case 2235: case 2236: case 2237: case 2238: case 2239:
    case 2240: case 2241: case 2242: case 2243: case 2244: case 2245: case 2246: case 2247: case 2248: case 2249:
    case 2250: case 2251: case 2252: case 2253: case 2254: case 2255: case 2256: case 2257: case 2258: case 2259:
    case 2260: case 2261: case 2262: case 2263: case 2264: case 2265: case 2266: case 2267: case 2268: case 2269:
    case 2270: case 2271: case 2272: case 2273: case 2274: case 2275: case 2276: case 2277: case 2278: case 2279:
    case 2280: case 2281: case 2282: case 2283: case 2284: case 2285: case 2286: case 2287: case 2288: case 2289:
    case 2290: case 2291: case 2292: case 2293: case 2294: case 2295: case 2296: case 2297: case 2298: case 2299:
      return "Insect";
    // ── Birds 2300-2399 ───────────────────────────────────────
    case 2300: case 2301: case 2302: case 2303: case 2304: case 2305: case 2306: case 2307: case 2308: case 2309:
    case 2310: case 2311: case 2312: case 2313: case 2314: case 2315: case 2316: case 2317: case 2318: case 2319:
    case 2320: case 2321: case 2322: case 2323: case 2324: case 2325: case 2326: case 2327: case 2328: case 2329:
    case 2330: case 2331: case 2332: case 2333: case 2334: case 2335: case 2336: case 2337: case 2338: case 2339:
    case 2340: case 2341: case 2342: case 2343: case 2344: case 2345: case 2346: case 2347: case 2348: case 2349:
    case 2350: case 2351: case 2352: case 2353: case 2354: case 2355: case 2356: case 2357: case 2358: case 2359:
    case 2360: case 2361: case 2362: case 2363: case 2364: case 2365: case 2366: case 2367: case 2368: case 2369:
    case 2370: case 2371: case 2372: case 2373: case 2374: case 2375: case 2376: case 2377: case 2378: case 2379:
    case 2380: case 2381: case 2382: case 2383: case 2384: case 2385: case 2386: case 2387: case 2388: case 2389:
    case 2390: case 2391: case 2392: case 2393: case 2394: case 2395: case 2396: case 2397: case 2398: case 2399:
      return "Bird";
    // ── Fish 2400-2499 ────────────────────────────────────────
    case 2400: case 2401: case 2402: case 2403: case 2404: case 2405: case 2406: case 2407: case 2408: case 2409:
    case 2410: case 2411: case 2412: case 2413: case 2414: case 2415: case 2416: case 2417: case 2418: case 2419:
    case 2420: case 2421: case 2422: case 2423: case 2424: case 2425: case 2426: case 2427: case 2428: case 2429:
    case 2430: case 2431: case 2432: case 2433: case 2434: case 2435: case 2436: case 2437: case 2438: case 2439:
    case 2440: case 2441: case 2442: case 2443: case 2444: case 2445: case 2446: case 2447: case 2448: case 2449:
    case 2450: case 2451: case 2452: case 2453: case 2454: case 2455: case 2456: case 2457: case 2458: case 2459:
    case 2460: case 2461: case 2462: case 2463: case 2464: case 2465: case 2466: case 2467: case 2468: case 2469:
    case 2470: case 2471: case 2472: case 2473: case 2474: case 2475: case 2476: case 2477: case 2478: case 2479:
    case 2480: case 2481: case 2482: case 2483: case 2484: case 2485: case 2486: case 2487: case 2488: case 2489:
    case 2490: case 2491: case 2492: case 2493: case 2494: case 2495: case 2496: case 2497: case 2498: case 2499:
      return "Fish";
    // ── Mammals 2500-2599 ────────────────────────────────────
    case 2500: case 2501: case 2502: case 2503: case 2504: case 2505: case 2506: case 2507: case 2508: case 2509:
    case 2510: case 2511: case 2512: case 2513: case 2514: case 2515: case 2516: case 2517: case 2518: case 2519:
    case 2520: case 2521: case 2522: case 2523: case 2524: case 2525: case 2526: case 2527: case 2528: case 2529:
    case 2530: case 2531: case 2532: case 2533: case 2534: case 2535: case 2536: case 2537: case 2538: case 2539:
    case 2540: case 2541: case 2542: case 2543: case 2544: case 2545: case 2546: case 2547: case 2548: case 2549:
    case 2550: case 2551: case 2552: case 2553: case 2554: case 2555: case 2556: case 2557: case 2558: case 2559:
    case 2560: case 2561: case 2562: case 2563: case 2564: case 2565: case 2566: case 2567: case 2568: case 2569:
    case 2570: case 2571: case 2572: case 2573: case 2574: case 2575: case 2576: case 2577: case 2578: case 2579:
    case 2580: case 2581: case 2582: case 2583: case 2584: case 2585: case 2586: case 2587: case 2588: case 2589:
    case 2590: case 2591: case 2592: case 2593: case 2594: case 2595: case 2596: case 2597: case 2598: case 2599:
      return "Mammal";
    case 2600: return "AzulAmarelo";
    default: return "Original";
  }
}

void applyTheme(uint16_t theme) {
  themeSet = theme % 2601;
  switch (themeSet) {
    default:
    case 0: // Original — dark navy / cyan
      UI_HEADER_BG    = 0x0820;
      UI_FREQ_COLOR   = 0x07E0;
      UI_LEVEL_COLOR  = 0xFD20;
      UI_LABEL_COLOR  = 0x07FF;
      UI_DIM_COLOR    = 0x7BEF;
      UI_BORDER_COLOR = 0x07FF;
      break;
    case 1: // Vermelho — dark maroon / red / amber
      UI_HEADER_BG    = 0x3000;
      UI_FREQ_COLOR   = 0xFCA0;
      UI_LEVEL_COLOR  = 0xF800;
      UI_LABEL_COLOR  = 0xFD60;
      UI_DIM_COLOR    = 0x4208;
      UI_BORDER_COLOR = 0xFC00;
      break;
    case 2: // Brasil — deep blue / yellow / green
      UI_HEADER_BG    = 0x0010;
      UI_FREQ_COLOR   = 0xFFE0;
      UI_LEVEL_COLOR  = 0x07C0;
      UI_LABEL_COLOR  = 0xFFE0;
      UI_DIM_COLOR    = 0x3186;
      UI_BORDER_COLOR = 0x07C0;
      break;
    case 3: // Noturno — near-black / silver-white
      UI_HEADER_BG    = 0x2104;
      UI_FREQ_COLOR   = 0xFFFF;
      UI_LEVEL_COLOR  = 0xC618;
      UI_LABEL_COLOR  = 0xAD75;
      UI_DIM_COLOR    = 0x4208;
      UI_BORDER_COLOR = 0x8410;
      break;
    case 4: // Amber — dark brown / amber / orange (CRT phosphor)
      UI_HEADER_BG    = 0x28A0;
      UI_FREQ_COLOR   = 0xFEA0;
      UI_LEVEL_COLOR  = 0xFD20;
      UI_LABEL_COLOR  = 0xFDA0;
      UI_DIM_COLOR    = 0x6300;
      UI_BORDER_COLOR = 0xFD20;
      break;
    case 5: // Verde — dark green / bright green (matrix)
      UI_HEADER_BG    = 0x0200;
      UI_FREQ_COLOR   = 0x07E0;
      UI_LEVEL_COLOR  = 0x0400;
      UI_LABEL_COLOR  = 0x07E0;
      UI_DIM_COLOR    = 0x0280;
      UI_BORDER_COLOR = 0x0400;
      break;
    case 6: // Roxo — dark purple / violet / magenta
      UI_HEADER_BG    = 0x2007;
      UI_FREQ_COLOR   = 0xE01F;
      UI_LEVEL_COLOR  = 0xF81F;
      UI_LABEL_COLOR  = 0xC01F;
      UI_DIM_COLOR    = 0x500A;
      UI_BORDER_COLOR = 0xC01F;
      break;
    case 7: // Azul — midnight blue / azure / ice
      UI_HEADER_BG    = 0x000A;
      UI_FREQ_COLOR   = 0x6FFF;
      UI_LEVEL_COLOR  = 0x05FF;
      UI_LABEL_COLOR  = 0x6FFF;
      UI_DIM_COLOR    = 0x0290;
      UI_BORDER_COLOR = 0x05FF;
      break;
    case 8: // PE5PVB — yellow freq / blue frame / green bar (main branch default)
      UI_HEADER_BG    = 0x1082; // Darkgrey
      UI_FREQ_COLOR   = 0xFFE0; // Yellow
      UI_LEVEL_COLOR  = 0x07E0; // Green
      UI_LABEL_COLOR  = 0xFFE0; // Yellow
      UI_DIM_COLOR    = 0x39E7; // BlackOlive
      UI_BORDER_COLOR = 0x001F; // Blue
      break;
    case 9: // Ciano — cyan labels / cyan frame (main branch cyan theme)
      UI_HEADER_BG    = 0x2A08; // CyanGrey
      UI_FREQ_COLOR   = 0x0F3F; // Cyan
      UI_LEVEL_COLOR  = 0x867D; // Skyblue
      UI_LABEL_COLOR  = 0x0F3F; // Cyan
      UI_DIM_COLOR    = 0x5BAF; // CyanDark
      UI_BORDER_COLOR = 0x01E9; // CyanFrame
      break;
    case 10: // Vulcao — orange on prussian blue bg (main branch volcano)
      UI_HEADER_BG    = 0x0806; // Prussian
      UI_FREQ_COLOR   = 0xFC00; // Orange
      UI_LEVEL_COLOR  = 0xF980; // Tangerine
      UI_LABEL_COLOR  = 0xFC00; // Orange
      UI_DIM_COLOR    = 0x5140; // Maroon
      UI_BORDER_COLOR = 0x2965; // GreenGrey
      break;
    case 11: // Sakura — soft pink / blackberry header (main branch sakura)
      UI_HEADER_BG    = 0x38C5; // Blackberry
      UI_FREQ_COLOR   = 0xF3D5; // Sakura
      UI_LEVEL_COLOR  = 0xD01F; // Violet
      UI_LABEL_COLOR  = 0xF3D5; // Sakura
      UI_DIM_COLOR    = 0x9B90; // SakuraGrey
      UI_BORDER_COLOR = 0x3845; // Tyrian
      break;
    case 12: // Oceano — deep ocean blue / navy (main branch ocean)
      UI_HEADER_BG    = 0x0010; // Navy
      UI_FREQ_COLOR   = 0x01FF; // Ocean
      UI_LEVEL_COLOR  = 0x051F; // Deepsky
      UI_LABEL_COLOR  = 0x01FF; // Ocean
      UI_DIM_COLOR    = 0x420C; // Cornblue
      UI_BORDER_COLOR = 0x051F; // Deepsky
      break;
    case 13: // Tangerina — tangerine / yolk / spice (main branch tangerine)
      UI_HEADER_BG    = 0x6247; // Spice
      UI_FREQ_COLOR   = 0xF980; // Tangerine
      UI_LEVEL_COLOR  = 0xED20; // Yolk
      UI_LABEL_COLOR  = 0xF980; // Tangerine
      UI_DIM_COLOR    = 0x9B8D; // Copper
      UI_BORDER_COLOR = 0xED20; // Yolk
      break;
    case 14: // Carmesim — crimson / coral (main branch crimson theme)
      UI_HEADER_BG    = 0x3800; // CrimsonSmooth
      UI_FREQ_COLOR   = 0xF8C3; // Crimson
      UI_LEVEL_COLOR  = 0xFBEF; // Coral
      UI_LABEL_COLOR  = 0xF8C3; // Crimson
      UI_DIM_COLOR    = 0x4A69; // PaleGrey
      UI_BORDER_COLOR = 0x3800; // CrimsonSmooth
      break;
    case 15: // Dendro — green / cabbage / dark green (main branch dendro)
      UI_HEADER_BG    = 0x0200; // GreenDark
      UI_FREQ_COLOR   = 0x07E0; // Green
      UI_LEVEL_COLOR  = 0x06D0; // Cabbage
      UI_LABEL_COLOR  = 0x07E0; // Green
      UI_DIM_COLOR    = 0x4A69; // PaleGrey
      UI_BORDER_COLOR = 0x0200; // GreenDark
      break;
    case 16: // Monocromo — white / grey / green-grey (main branch monochrome)
      UI_HEADER_BG    = 0x2965; // GreenGrey
      UI_FREQ_COLOR   = 0xFFFF; // White
      UI_LEVEL_COLOR  = 0xDFFC; // Honeydew
      UI_LABEL_COLOR  = 0xFFFF; // White
      UI_DIM_COLOR    = 0x4A69; // PaleGrey
      UI_BORDER_COLOR = 0x2965; // GreenGrey
      break;
    case 17: // Indigo — indigo / violet / electric (main branch indigo)
      UI_HEADER_BG    = 0x49AC; // Meteorite
      UI_FREQ_COLOR   = 0x881F; // Indigo
      UI_LEVEL_COLOR  = 0xF00A; // Cherry
      UI_LABEL_COLOR  = 0x881F; // Indigo
      UI_DIM_COLOR    = 0x49AC; // Meteorite
      UI_BORDER_COLOR = 0x6016; // Electric
      break;
    case 18: // Queer — cherry / coral / electric on maroon (main branch queer)
      UI_HEADER_BG    = 0x2001; // MaroonSmooth
      UI_FREQ_COLOR   = 0xF00A; // Cherry
      UI_LEVEL_COLOR  = 0xD01F; // Violet
      UI_LABEL_COLOR  = 0xFBEF; // Coral
      UI_DIM_COLOR    = 0x4124; // CoralSmooth
      UI_BORDER_COLOR = 0x6016; // Electric
      break;
    case 19: // GoldBrite — white freq / yellow labels / blue frame (main branch goldbrite)
      UI_HEADER_BG    = 0x39E7; // BlackOlive
      UI_FREQ_COLOR   = 0xFFFF; // White
      UI_LEVEL_COLOR  = 0x07E0; // Green
      UI_LABEL_COLOR  = 0xFFE0; // Yellow
      UI_DIM_COLOR    = 0x39E7; // BlackOlive
      UI_BORDER_COLOR = 0x001F; // Blue
      break;
    case 20: // Bubblegum — pink freq / cyan labels / eerie black (main branch bubblegum)
      UI_HEADER_BG    = 0x1825; // EerieBlack
      UI_FREQ_COLOR   = 0xFDBF; // Pink
      UI_LEVEL_COLOR  = 0x07FF; // Turquoise
      UI_LABEL_COLOR  = 0x0F3F; // Cyan
      UI_DIM_COLOR    = 0x49AC; // Meteorite
      UI_BORDER_COLOR = 0x6016; // Electric
      break;
    case 21: // Larico — fiery orange on dark burnt
      UI_HEADER_BG    = 0x1880; // DarkBurnt
      UI_FREQ_COLOR   = 0xFB80; // FireOrange
      UI_LEVEL_COLOR  = 0xFD40; // GoldOrange
      UI_LABEL_COLOR  = 0xFB80; // FireOrange
      UI_DIM_COLOR    = 0x30C0; // CharcoalOrange
      UI_BORDER_COLOR = 0xFD40; // GoldOrange
      break;
    case 22: // Esmeralda — emerald green on deep teal
      UI_HEADER_BG    = 0x00C3; // DeepTeal
      UI_FREQ_COLOR   = 0x07F0; // Emerald
      UI_LEVEL_COLOR  = 0x0610; // MidTeal
      UI_LABEL_COLOR  = 0x07F0; // Emerald
      UI_DIM_COLOR    = 0x0262; // DarkEmerald
      UI_BORDER_COLOR = 0x05D3; // TealBlue
      break;
    case 23: // Artico — arctic ice blues on dark
      UI_HEADER_BG    = 0x1084; // DarkArcticBlue
      UI_FREQ_COLOR   = 0xAEBF; // ArcticIce
      UI_LEVEL_COLOR  = 0x45FF; // ArcticBlue
      UI_LABEL_COLOR  = 0xAEBF; // ArcticIce
      UI_DIM_COLOR    = 0x2ACF; // SteelBlue
      UI_BORDER_COLOR = 0x65FF; // IceBlue
      break;
    case 24: // Crepusculo — sunset gold/orange on dark dusk
      UI_HEADER_BG    = 0x1804; // DuskPurple
      UI_FREQ_COLOR   = 0xFEA0; // SunsetGold
      UI_LEVEL_COLOR  = 0xFBC0; // SunsetOrange
      UI_LABEL_COLOR  = 0xFEA0; // SunsetGold
      UI_DIM_COLOR    = 0x4889; // DuskBlue
      UI_BORDER_COLOR = 0xC21F; // MagentaDusk
      break;
    case 25: // Turquesa — turquoise/aqua
      UI_HEADER_BG    = 0x0188; // DeepAqua
      UI_FREQ_COLOR   = 0x07FE; // Turquoise
      UI_LEVEL_COLOR  = 0x0676; // Aquamarine
      UI_LABEL_COLOR  = 0x07FE; // Turquoise
      UI_DIM_COLOR    = 0x02D4; // DeepAquaDim
      UI_BORDER_COLOR = 0x3EDF; // AquaLight
      break;
    case 26: // Ametista — amethyst purple on deep violet
      UI_HEADER_BG    = 0x3006; // DeepViolet
      UI_FREQ_COLOR   = 0xB01F; // Amethyst
      UI_LEVEL_COLOR  = 0x821A; // PurpleHaze
      UI_LABEL_COLOR  = 0xB01F; // Amethyst
      UI_DIM_COLOR    = 0x4812; // VioletDim
      UI_BORDER_COLOR = 0x780F; // PlumFrame
      break;
    case 27: // Ferrugem — rust/iron on dark maroon
      UI_HEADER_BG    = 0x2802; // DarkRust
      UI_FREQ_COLOR   = 0xCA41; // Rust
      UI_LEVEL_COLOR  = 0xAD55; // IronGrey
      UI_LABEL_COLOR  = 0xCA41; // Rust
      UI_DIM_COLOR    = 0x5964; // Gunmetal
      UI_BORDER_COLOR = 0xA340; // Terracotta
      break;
    case 28: // Neon — acid neon green on black
      UI_HEADER_BG    = 0x0000; // Black
      UI_FREQ_COLOR   = 0x87E0; // NeonGreen
      UI_LEVEL_COLOR  = 0x07E0; // PureGreen
      UI_LABEL_COLOR  = 0x87E0; // NeonGreen
      UI_DIM_COLOR    = 0x02E0; // DarkNeon
      UI_BORDER_COLOR = 0x87E0; // NeonGreen
      break;
    case 29: // Prata — silver metallic monochrome
      UI_HEADER_BG    = 0x4208; // DarkGrey
      UI_FREQ_COLOR   = 0xFFFF; // White
      UI_LEVEL_COLOR  = 0xC618; // Silver
      UI_LABEL_COLOR  = 0xEF7D; // LightSilver
      UI_DIM_COLOR    = 0x7BEF; // MedGrey
      UI_BORDER_COLOR = 0x9CF3; // Platinum
      break;
    case 30: // Aurora — northern lights multicolor
      UI_HEADER_BG    = 0x0021; // Midnight
      UI_FREQ_COLOR   = 0x07FF; // AuroraCyan
      UI_LEVEL_COLOR  = 0x07C0; // AuroraGreen
      UI_LABEL_COLOR  = 0xF81F; // AuroraMagenta
      UI_DIM_COLOR    = 0x1A2F; // NightSky
      UI_BORDER_COLOR = 0x07E0; // AuroraBorder
      break;
    case 31: // Lavanda — lavender / lilac soft purple
      UI_HEADER_BG    = 0x1806; // DarkPlum
      UI_FREQ_COLOR   = 0xC41E; // Lavender
      UI_LEVEL_COLOR  = 0x9318; // PurpleHaze
      UI_LABEL_COLOR  = 0xC41E; // Lavender
      UI_DIM_COLOR    = 0x400C; // PlumDim
      UI_BORDER_COLOR = 0x6194; // LilacFrame
      break;
    case 32: // Caramelo — warm caramel / butterscotch
      UI_HEADER_BG    = 0x2080; // DarkCaramel
      UI_FREQ_COLOR   = 0xF4C4; // Butterscotch
      UI_LEVEL_COLOR  = 0xE380; // Caramel
      UI_LABEL_COLOR  = 0xF4C4; // Butterscotch
      UI_DIM_COLOR    = 0x6182; // BrownDim
      UI_BORDER_COLOR = 0xE380; // Caramel
      break;
    case 33: // Menta — fresh mint green
      UI_HEADER_BG    = 0x0204; // DarkMint
      UI_FREQ_COLOR   = 0x87F6; // Mint
      UI_LEVEL_COLOR  = 0x0690; // Spearmint
      UI_LABEL_COLOR  = 0x87F6; // Mint
      UI_DIM_COLOR    = 0x0388; // DarkMintDim
      UI_BORDER_COLOR = 0x4610; // LightMint
      break;
    case 34: // Tomate — tomato red
      UI_HEADER_BG    = 0x2800; // DarkTomato
      UI_FREQ_COLOR   = 0xFA44; // Tomato
      UI_LEVEL_COLOR  = 0xF804; // BrightRed
      UI_LABEL_COLOR  = 0xFA44; // Tomato
      UI_DIM_COLOR    = 0x5800; // Maroon
      UI_BORDER_COLOR = 0xFA44; // Tomato
      break;
    case 35: // Safira — sapphire blue
      UI_HEADER_BG    = 0x0043; // DeepSapphire
      UI_FREQ_COLOR   = 0x2B1C; // Sapphire
      UI_LEVEL_COLOR  = 0x1218; // RoyalBlue
      UI_LABEL_COLOR  = 0x2B1C; // Sapphire
      UI_DIM_COLOR    = 0x010A; // DeepNavy
      UI_BORDER_COLOR = 0x0A5E; // CobaltFrame
      break;
    case 36: // Limoeiro — lemon / citrus yellow
      UI_HEADER_BG    = 0x2140; // DarkLemon
      UI_FREQ_COLOR   = 0xFFE0; // Lemon
      UI_LEVEL_COLOR  = 0xC7E0; // LimeGreen
      UI_LABEL_COLOR  = 0xFFE0; // Lemon
      UI_DIM_COLOR    = 0x6B20; // DimLemon
      UI_BORDER_COLOR = 0xC7E0; // Lime
      break;
    case 37: // Marinho — deep navy / maritime
      UI_HEADER_BG    = 0x0022; // DeepNavy
      UI_FREQ_COLOR   = 0x441F; // MarineBlue
      UI_LEVEL_COLOR  = 0x231A; // NavyBlue
      UI_LABEL_COLOR  = 0x441F; // MarineBlue
      UI_DIM_COLOR    = 0x0086; // NavyDim
      UI_BORDER_COLOR = 0x231A; // NavyBlue
      break;
    case 38: // Carmim — carmine deep rose
      UI_HEADER_BG    = 0x1800; // DeepCrimson
      UI_FREQ_COLOR   = 0xD004; // Carmine
      UI_LEVEL_COLOR  = 0xFA0C; // FlameRose
      UI_LABEL_COLOR  = 0xFC12; // Rose
      UI_DIM_COLOR    = 0x5004; // Bordeaux
      UI_BORDER_COLOR = 0xD004; // Carmine
      break;
    case 39: // Celeste — sky blue / celestial
      UI_HEADER_BG    = 0x00A5; // DarkSky
      UI_FREQ_COLOR   = 0x661F; // SkyBlue
      UI_LEVEL_COLOR  = 0x241F; // Azure
      UI_LABEL_COLOR  = 0x661F; // SkyBlue
      UI_DIM_COLOR    = 0x018C; // DimSky
      UI_BORDER_COLOR = 0x241F; // Azure
      break;
    case 40: // Abacate — avocado green
      UI_HEADER_BG    = 0x10C0; // DarkAvocado
      UI_FREQ_COLOR   = 0x8E48; // Avocado
      UI_LEVEL_COLOR  = 0x4504; // OliveGreen
      UI_LABEL_COLOR  = 0x8E48; // Avocado
      UI_DIM_COLOR    = 0x2242; // DarkOlive
      UI_BORDER_COLOR = 0x6406; // OliveBorder
      break;
    case 41: // Magenta — hot fuchsia / magenta
      UI_HEADER_BG    = 0x2004; // DarkFuchsia
      UI_FREQ_COLOR   = 0xF81F; // Magenta
      UI_LEVEL_COLOR  = 0xFA18; // HotPink
      UI_LABEL_COLOR  = 0xF81F; // Magenta
      UI_DIM_COLOR    = 0x600C; // PurpleDim
      UI_BORDER_COLOR = 0xFA18; // HotPink
      break;
    case 42: // Brasa — embers / glowing coals
      UI_HEADER_BG    = 0x1000; // NearBlack
      UI_FREQ_COLOR   = 0xFA80; // Ember
      UI_LEVEL_COLOR  = 0xF900; // Fire
      UI_LABEL_COLOR  = 0xFA80; // Ember
      UI_DIM_COLOR    = 0x4040; // AshDim
      UI_BORDER_COLOR = 0xFA80; // Ember
      break;
    case 43: // Ardosia — slate blue-grey
      UI_HEADER_BG    = 0x1946; // Slate
      UI_FREQ_COLOR   = 0x8559; // SlateBlue
      UI_LEVEL_COLOR  = 0x5414; // SteelSlate
      UI_LABEL_COLOR  = 0x8559; // SlateBlue
      UI_DIM_COLOR    = 0x320A; // SlateDim
      UI_BORDER_COLOR = 0x5414; // SteelSlate
      break;
    case 44: // Cereja — cherry red / deep rose
      UI_HEADER_BG    = 0x2001; // DarkCherry
      UI_FREQ_COLOR   = 0xF80C; // Cherry
      UI_LEVEL_COLOR  = 0xD008; // DeepCherry
      UI_LABEL_COLOR  = 0xF80C; // Cherry
      UI_DIM_COLOR    = 0x6006; // CherryDim
      UI_BORDER_COLOR = 0xD008; // DeepCherry
      break;
    case 45: // Petroleo — petroleum / dark teal
      UI_HEADER_BG    = 0x0083; // PetroleumDark
      UI_FREQ_COLOR   = 0x0615; // Petroleum
      UI_LEVEL_COLOR  = 0x0410; // DarkTeal
      UI_LABEL_COLOR  = 0x0615; // Petroleum
      UI_DIM_COLOR    = 0x0146; // OilDim
      UI_BORDER_COLOR = 0x0410; // DarkTeal
      break;
    case 46: // Cobalto — cobalt electric blue
      UI_HEADER_BG    = 0x0044; // DeepCobalt
      UI_FREQ_COLOR   = 0x029F; // Cobalt
      UI_LEVEL_COLOR  = 0x439C; // CobaltMid
      UI_LABEL_COLOR  = 0x029F; // Cobalt
      UI_DIM_COLOR    = 0x0088; // NavyDim
      UI_BORDER_COLOR = 0x439C; // CobaltMid
      break;
    case 47: // Mel — honey / warm golden amber
      UI_HEADER_BG    = 0x20A0; // DarkHoney
      UI_FREQ_COLOR   = 0xFE60; // Honey
      UI_LEVEL_COLOR  = 0xE440; // Amber
      UI_LABEL_COLOR  = 0xFE60; // Honey
      UI_DIM_COLOR    = 0x6240; // HoneyDim
      UI_BORDER_COLOR = 0xE440; // Amber
      break;
    case 48: // Vinho — wine / burgundy
      UI_HEADER_BG    = 0x1801; // DarkWine
      UI_FREQ_COLOR   = 0xC008; // Wine
      UI_LEVEL_COLOR  = 0x900C; // Burgundy
      UI_LABEL_COLOR  = 0xE310; // RoseLabel
      UI_DIM_COLOR    = 0x4804; // WineDim
      UI_BORDER_COLOR = 0x900C; // Burgundy
      break;
    case 49: // Acqua — vivid aqua / blue-green
      UI_HEADER_BG    = 0x00C4; // DeepAcqua
      UI_FREQ_COLOR   = 0x07FD; // Acqua
      UI_LEVEL_COLOR  = 0x0718; // TealGreen
      UI_LABEL_COLOR  = 0x07FD; // Acqua
      UI_DIM_COLOR    = 0x0188; // AcquaDim
      UI_BORDER_COLOR = 0x0718; // TealGreen
      break;
    case 50: // Cobre — copper metallic
      UI_HEADER_BG    = 0x2081; // DarkCopper
      UI_FREQ_COLOR   = 0xD344; // Copper
      UI_LEVEL_COLOR  = 0xA243; // DarkCopper
      UI_LABEL_COLOR  = 0xD344; // Copper
      UI_DIM_COLOR    = 0x6142; // CopperDim
      UI_BORDER_COLOR = 0xA243; // BronzeFrame
      break;
    case 51: // Algodao — cotton candy pink
      UI_HEADER_BG    = 0x2845; // DarkCotton
      UI_FREQ_COLOR   = 0xFC19; // CottonPink
      UI_LEVEL_COLOR  = 0xF214; // HotRose
      UI_LABEL_COLOR  = 0xFC19; // CottonPink
      UI_DIM_COLOR    = 0x610C; // PinkDim
      UI_BORDER_COLOR = 0xF214; // HotRose
      break;
    case 52: // Musgo — moss green
      UI_HEADER_BG    = 0x08A0; // DarkMoss
      UI_FREQ_COLOR   = 0x6544; // Moss
      UI_LEVEL_COLOR  = 0x4400; // OliveGreen
      UI_LABEL_COLOR  = 0x6544; // Moss
      UI_DIM_COLOR    = 0x2A02; // MossDim
      UI_BORDER_COLOR = 0x32C4; // MossFrame
      break;
    case 53: // Oxido — oxidized copper / verdigris
      UI_HEADER_BG    = 0x08C3; // OxideDark
      UI_FREQ_COLOR   = 0x4634; // Verdigris
      UI_LEVEL_COLOR  = 0x240C; // PatinaTeal
      UI_LABEL_COLOR  = 0x4634; // Verdigris
      UI_DIM_COLOR    = 0x11C5; // OxideDim
      UI_BORDER_COLOR = 0x240C; // PatinaTeal
      break;
    case 54: // Manga — mango fruit
      UI_HEADER_BG    = 0x2860; // DarkMango
      UI_FREQ_COLOR   = 0xFC80; // Mango
      UI_LEVEL_COLOR  = 0xFE00; // MangoYellow
      UI_LABEL_COLOR  = 0xFC80; // Mango
      UI_DIM_COLOR    = 0x6180; // MangoDim
      UI_BORDER_COLOR = 0xFE00; // MangoYellow
      break;
    case 55: // Uva — grape purple
      UI_HEADER_BG    = 0x1805; // DarkGrape
      UI_FREQ_COLOR   = 0x8819; // Grape
      UI_LEVEL_COLOR  = 0x6014; // PlumGrape
      UI_LABEL_COLOR  = 0x8819; // Grape
      UI_DIM_COLOR    = 0x380B; // GrapeDim
      UI_BORDER_COLOR = 0x6014; // PlumGrape
      break;
    case 56: // Maracuja — passionfruit
      UI_HEADER_BG    = 0x2003; // DarkPassion
      UI_FREQ_COLOR   = 0xFE88; // PassionYellow
      UI_LEVEL_COLOR  = 0xC214; // PassionPurple
      UI_LABEL_COLOR  = 0xFE88; // PassionYellow
      UI_DIM_COLOR    = 0x5007; // PassionDim
      UI_BORDER_COLOR = 0xC214; // PassionPurple
      break;
    case 57: // Grafite — graphite / carbon grey
      UI_HEADER_BG    = 0x18C3; // GraphiteDark
      UI_FREQ_COLOR   = 0xD69A; // LightGrey
      UI_LEVEL_COLOR  = 0x9492; // MidGrey
      UI_LABEL_COLOR  = 0xD69A; // LightGrey
      UI_DIM_COLOR    = 0x4A09; // DarkGrey
      UI_BORDER_COLOR = 0x6B4D; // SteelGrey
      break;
    case 58: // Champanhe — champagne / gold
      UI_HEADER_BG    = 0x18A0; // DarkChampagne
      UI_FREQ_COLOR   = 0xFF50; // Champagne
      UI_LEVEL_COLOR  = 0xCD00; // Gold
      UI_LABEL_COLOR  = 0xFF50; // Champagne
      UI_DIM_COLOR    = 0x8308; // WarmBrown
      UI_BORDER_COLOR = 0xCD00; // Gold
      break;
    case 59: // Coral — vivid coral / salmon
      UI_HEADER_BG    = 0x2040; // DarkCoral
      UI_FREQ_COLOR   = 0xFB0A; // Coral
      UI_LEVEL_COLOR  = 0xFC90; // LightCoral
      UI_LABEL_COLOR  = 0xFB0A; // Coral
      UI_DIM_COLOR    = 0x6104; // CoralDim
      UI_BORDER_COLOR = 0xFC90; // LightCoral
      break;
    case 60: // Fantasma — ghost / pale ethereal
      UI_HEADER_BG    = 0x1083; // GhostDark
      UI_FREQ_COLOR   = 0xEF9F; // GhostWhite
      UI_LEVEL_COLOR  = 0xA51A; // PaleBlue
      UI_LABEL_COLOR  = 0xEF9F; // GhostWhite
      UI_DIM_COLOR    = 0x420C; // GhostDim
      UI_BORDER_COLOR = 0xA51A; // PaleBlue
      break;
    case 61: // Sertao — Brazilian sertao / earth tones
      UI_HEADER_BG    = 0x1860; // EarthDark
      UI_FREQ_COLOR   = 0xD448; // SertaoGold
      UI_LEVEL_COLOR  = 0xAB04; // ClayBrown
      UI_LABEL_COLOR  = 0xD448; // SertaoGold
      UI_DIM_COLOR    = 0x5183; // EarthDim
      UI_BORDER_COLOR = 0xAB04; // ClayBrown
      break;
    case 62: // Essence — pearl white / rose gold on near-black
      UI_HEADER_BG    = 0x0822; // NearBlack
      UI_FREQ_COLOR   = 0xFF5C; // Pearl
      UI_LEVEL_COLOR  = 0xD50E; // RoseGold
      UI_LABEL_COLOR  = 0xFF19; // Cream
      UI_DIM_COLOR    = 0x3964; // WarmDark
      UI_BORDER_COLOR = 0xD50E; // RoseGold
      break;
    case 63: // Cyan — cyan labels / cyan frame
      UI_HEADER_BG    = 0x2A08; // CyanGrey
      UI_FREQ_COLOR   = 0x0F3F; // Cyan
      UI_LEVEL_COLOR  = 0x867D; // Skyblue
      UI_LABEL_COLOR  = 0x0F3F; // Cyan
      UI_DIM_COLOR    = 0x5BAF; // CyanDark
      UI_BORDER_COLOR = 0x01E9; // CyanFrame
      break;
    case 64: // Crimson — crimson / coral
      UI_HEADER_BG    = 0x3800; // CrimsonSmooth
      UI_FREQ_COLOR   = 0xF8C3; // Crimson
      UI_LEVEL_COLOR  = 0xFBEF; // Coral
      UI_LABEL_COLOR  = 0xF8C3; // Crimson
      UI_DIM_COLOR    = 0x4A69; // PaleGrey
      UI_BORDER_COLOR = 0x3800; // CrimsonSmooth
      break;
    case 65: // Monochrome — white / grey / green-grey
      UI_HEADER_BG    = 0x2965; // GreenGrey
      UI_FREQ_COLOR   = 0xFFFF; // White
      UI_LEVEL_COLOR  = 0xDFFC; // Honeydew
      UI_LABEL_COLOR  = 0xFFFF; // White
      UI_DIM_COLOR    = 0x4A69; // PaleGrey
      UI_BORDER_COLOR = 0x2965; // GreenGrey
      break;
    case 66: // Volcano — orange on prussian blue
      UI_HEADER_BG    = 0x0806; // Prussian
      UI_FREQ_COLOR   = 0xFC00; // Orange
      UI_LEVEL_COLOR  = 0xF980; // Tangerine
      UI_LABEL_COLOR  = 0xFC00; // Orange
      UI_DIM_COLOR    = 0x5140; // Maroon
      UI_BORDER_COLOR = 0x2965; // GreenGrey
      break;
    case 67: // Dendro — green / cabbage / dark green
      UI_HEADER_BG    = 0x0200; // GreenDark
      UI_FREQ_COLOR   = 0x07E0; // Green
      UI_LEVEL_COLOR  = 0x06D0; // Cabbage
      UI_LABEL_COLOR  = 0x07E0; // Green
      UI_DIM_COLOR    = 0x4A69; // PaleGrey
      UI_BORDER_COLOR = 0x0200; // GreenDark
      break;
    case 68: // Sakura — soft pink / blackberry header
      UI_HEADER_BG    = 0x38C5; // Blackberry
      UI_FREQ_COLOR   = 0xF3D5; // Sakura
      UI_LEVEL_COLOR  = 0xD01F; // Violet
      UI_LABEL_COLOR  = 0xF3D5; // Sakura
      UI_DIM_COLOR    = 0x9B90; // SakuraGrey
      UI_BORDER_COLOR = 0x3845; // Tyrian
      break;
    case 69: // Whiteout — bright white / light theme
      UI_HEADER_BG    = 0xFFFF; // White
      UI_FREQ_COLOR   = 0x0010; // DarkNavy
      UI_LEVEL_COLOR  = 0x001F; // Blue
      UI_LABEL_COLOR  = 0x2945; // DarkGreenGrey
      UI_DIM_COLOR    = 0xC618; // Silver
      UI_BORDER_COLOR = 0x001F; // Blue
      break;
    case 70: // Tangerine — tangerine / yolk / spice
      UI_HEADER_BG    = 0x6247; // Spice
      UI_FREQ_COLOR   = 0xF980; // Tangerine
      UI_LEVEL_COLOR  = 0xED20; // Yolk
      UI_LABEL_COLOR  = 0xF980; // Tangerine
      UI_DIM_COLOR    = 0x9B8D; // Copper
      UI_BORDER_COLOR = 0xED20; // Yolk
      break;
    case 71: // Ocean — deep ocean blue / navy
      UI_HEADER_BG    = 0x0010; // Navy
      UI_FREQ_COLOR   = 0x01FF; // Ocean
      UI_LEVEL_COLOR  = 0x051F; // Deepsky
      UI_LABEL_COLOR  = 0x01FF; // Ocean
      UI_DIM_COLOR    = 0x420C; // Cornblue
      UI_BORDER_COLOR = 0x051F; // Deepsky
      break;
    case 72: // Indigo — indigo / violet / electric
      UI_HEADER_BG    = 0x49AC; // Meteorite
      UI_FREQ_COLOR   = 0x881F; // Indigo
      UI_LEVEL_COLOR  = 0xF00A; // Cherry
      UI_LABEL_COLOR  = 0x881F; // Indigo
      UI_DIM_COLOR    = 0x49AC; // Meteorite
      UI_BORDER_COLOR = 0x6016; // Electric
      break;
    case 73: // Queer — cherry / coral / electric on maroon
      UI_HEADER_BG    = 0x2001; // MaroonSmooth
      UI_FREQ_COLOR   = 0xF00A; // Cherry
      UI_LEVEL_COLOR  = 0xD01F; // Violet
      UI_LABEL_COLOR  = 0xFBEF; // Coral
      UI_DIM_COLOR    = 0x4124; // CoralSmooth
      UI_BORDER_COLOR = 0x6016; // Electric
      break;
    case 74: // GoldBrite — white freq / yellow labels / blue frame
      UI_HEADER_BG    = 0x39E7; // BlackOlive
      UI_FREQ_COLOR   = 0xFFFF; // White
      UI_LEVEL_COLOR  = 0x07E0; // Green
      UI_LABEL_COLOR  = 0xFFE0; // Yellow
      UI_DIM_COLOR    = 0x39E7; // BlackOlive
      UI_BORDER_COLOR = 0x001F; // Blue
      break;
    case 75: // Bubblegum — pink freq / cyan labels / eerie black
      UI_HEADER_BG    = 0x1825; // EerieBlack
      UI_FREQ_COLOR   = 0xFDBF; // Pink
      UI_LEVEL_COLOR  = 0x07FF; // Turquoise
      UI_LABEL_COLOR  = 0x0F3F; // Cyan
      UI_DIM_COLOR    = 0x49AC; // Meteorite
      UI_BORDER_COLOR = 0x6016; // Electric
      break;
    case 76: // Essence II — deeper rose gold / near-black
      UI_HEADER_BG    = 0x0000; // Black
      UI_FREQ_COLOR   = 0xFFFF; // PureWhite
      UI_LEVEL_COLOR  = 0xD50E; // RoseGold
      UI_LABEL_COLOR  = 0xEF7D; // Silver
      UI_DIM_COLOR    = 0x2965; // GreenGrey
      UI_BORDER_COLOR = 0xD50E; // RoseGold
      break;
    case 77: // 777 — lucky sevens: casino green / gold / red
      UI_HEADER_BG    = 0x0300; // CasinoGreen
      UI_FREQ_COLOR   = 0xFFE0; // Gold
      UI_LEVEL_COLOR  = 0xF800; // LuckyRed
      UI_LABEL_COLOR  = 0xFFE0; // Gold
      UI_DIM_COLOR    = 0x0480; // FeltGreen
      UI_BORDER_COLOR = 0xFFE0; // Gold
      break;
    case 78: // Vinil — vinyl record: warm black / cream / amber
      UI_HEADER_BG    = 0x1040; // VinylBlack
      UI_FREQ_COLOR   = 0xFF79; // VinylCream
      UI_LEVEL_COLOR  = 0xD444; // VinylAmber
      UI_LABEL_COLOR  = 0xFF79; // VinylCream
      UI_DIM_COLOR    = 0x38E1; // VinylSepia
      UI_BORDER_COLOR = 0xD444; // VinylAmber
      break;
    case 79: // Ouro — Au #79: pure gold on deep black
      UI_HEADER_BG    = 0x0840; // GoldDark
      UI_FREQ_COLOR   = 0xFEA0; // PureGold
      UI_LEVEL_COLOR  = 0xEE08; // PaleGold
      UI_LABEL_COLOR  = 0xFEA0; // PureGold
      UI_DIM_COLOR    = 0x4960; // DarkGold
      UI_BORDER_COLOR = 0xFEA0; // PureGold
      break;
    case 80: // Anos 80 — synthwave: neon pink / cyan on dark purple
      UI_HEADER_BG    = 0x1806; // DarkNight
      UI_FREQ_COLOR   = 0xF810; // NeonPink
      UI_LEVEL_COLOR  = 0x07FF; // NeonCyan
      UI_LABEL_COLOR  = 0xF810; // NeonPink
      UI_DIM_COLOR    = 0x400C; // PurpleDim
      UI_BORDER_COLOR = 0x07FF; // NeonCyan
      break;
    case 81: // Plasma — plasma display glow: warm pink-white / orange on dark red-purple
      UI_HEADER_BG    = 0x1802; // PlasmaBlack
      UI_FREQ_COLOR   = 0xFED8; // PlasmaGlow
      UI_LEVEL_COLOR  = 0xFB08; // PlasmaOrange
      UI_LABEL_COLOR  = 0xFED8; // PlasmaGlow
      UI_DIM_COLOR    = 0x5004; // PlasmaDim
      UI_BORDER_COLOR = 0xFB08; // PlasmaOrange
      break;
    case 82: // Nebulosa — deep space: pink nebula / purple on near-black
      UI_HEADER_BG    = 0x0802; // SpaceBlack
      UI_FREQ_COLOR   = 0xFC1F; // NebulaPink
      UI_LEVEL_COLOR  = 0x821F; // NebulaPurple
      UI_LABEL_COLOR  = 0xFC1F; // NebulaPink
      UI_DIM_COLOR    = 0x300A; // SpaceDim
      UI_BORDER_COLOR = 0x821F; // NebulaPurple
      break;
    case 83: // Fumaca — smoke / fog: blue-grey on dark charcoal
      UI_HEADER_BG    = 0x18C3; // Charcoal
      UI_FREQ_COLOR   = 0xB5D8; // SmokeBlue
      UI_LEVEL_COLOR  = 0x7412; // Slate
      UI_LABEL_COLOR  = 0xB5D8; // SmokeBlue
      UI_DIM_COLOR    = 0x4208; // DarkGrey
      UI_BORDER_COLOR = 0x7412; // Slate
      break;
    case 84: // Arco-Iris — rainbow: magenta / cyan / yellow / green
      UI_HEADER_BG    = 0x0841; // RainbowBlack
      UI_FREQ_COLOR   = 0xF81F; // Magenta
      UI_LEVEL_COLOR  = 0x07FF; // Cyan
      UI_LABEL_COLOR  = 0xFFE0; // Yellow
      UI_DIM_COLOR    = 0x2965; // GreyGreen
      UI_BORDER_COLOR = 0x07E0; // Green
      break;
    case 85: // Petala — flower petal: soft rose / deep rose on dark
      UI_HEADER_BG    = 0x2843; // DarkRose
      UI_FREQ_COLOR   = 0xFD9A; // PetalPink
      UI_LEVEL_COLOR  = 0xF394; // DeepRose
      UI_LABEL_COLOR  = 0xFD9A; // PetalPink
      UI_DIM_COLOR    = 0x6108; // RoseDim
      UI_BORDER_COLOR = 0xF394; // DeepRose
      break;
    case 86: // Relampago — lightning: electric yellow-white on night
      UI_HEADER_BG    = 0x0842; // NightBlack
      UI_FREQ_COLOR   = 0xFFF0; // LightningYellow
      UI_LEVEL_COLOR  = 0xFFFF; // White
      UI_LABEL_COLOR  = 0xFFF0; // LightningYellow
      UI_DIM_COLOR    = 0x320A; // StormDim
      UI_BORDER_COLOR = 0xFFFF; // White
      break;
    case 87: // Abismo — abyss: deep blue on near-black
      UI_HEADER_BG    = 0x0001; // AbyssBlack
      UI_FREQ_COLOR   = 0x031F; // AbyssBlue
      UI_LEVEL_COLOR  = 0x0216; // DeepBlue
      UI_LABEL_COLOR  = 0x031F; // AbyssBlue
      UI_DIM_COLOR    = 0x0086; // AbyssDim
      UI_BORDER_COLOR = 0x0216; // DeepBlue
      break;
    case 88: // Cristal — crystal / ice: clear light blue on dark
      UI_HEADER_BG    = 0x08C4; // CrystalDark
      UI_FREQ_COLOR   = 0xCF9F; // CrystalIce
      UI_LEVEL_COLOR  = 0x665C; // CrystalBlue
      UI_LABEL_COLOR  = 0xCF9F; // CrystalIce
      UI_DIM_COLOR    = 0x2A4C; // IceDim
      UI_BORDER_COLOR = 0x665C; // CrystalBlue
      break;
    case 89: // Lava — molten lava: deep red / orange on near-black
      UI_HEADER_BG    = 0x0800; // LavaBlack
      UI_FREQ_COLOR   = 0xF900; // LavaRed
      UI_LEVEL_COLOR  = 0xFB40; // LavaOrange
      UI_LABEL_COLOR  = 0xF900; // LavaRed
      UI_DIM_COLOR    = 0x3840; // LavaDim
      UI_BORDER_COLOR = 0xFB40; // LavaOrange
      break;
    case 90: // Pixel — 8-bit retro: pure primary colors on dark blue
      UI_HEADER_BG    = 0x0004; // PixelNight
      UI_FREQ_COLOR   = 0x07E0; // PixelGreen
      UI_LEVEL_COLOR  = 0xF800; // PixelRed
      UI_LABEL_COLOR  = 0xFFE0; // PixelYellow
      UI_DIM_COLOR    = 0x0010; // PixelNavy
      UI_BORDER_COLOR = 0x001F; // PixelBlue
      break;
    case 91: // Bosque — deep forest: forest green on near-black
      UI_HEADER_BG    = 0x0060; // ForestBlack
      UI_FREQ_COLOR   = 0x45C8; // ForestGreen
      UI_LEVEL_COLOR  = 0x2404; // DarkForest
      UI_LABEL_COLOR  = 0x45C8; // ForestGreen
      UI_DIM_COLOR    = 0x1983; // MossNight
      UI_BORDER_COLOR = 0x3306; // ForestBorder
      break;
    case 92: // Nuclear — U-92: radioactive green / toxic yellow on reactor black
      UI_HEADER_BG    = 0x0040; // ReactorDark
      UI_FREQ_COLOR   = 0x3FE2; // Radioactive
      UI_LEVEL_COLOR  = 0xAFE5; // ToxicYellow
      UI_LABEL_COLOR  = 0x3FE2; // Radioactive
      UI_DIM_COLOR    = 0x08C0; // ReactorDim
      UI_BORDER_COLOR = 0x3FE2; // Radioactive
      break;
    case 93: // Netuno — Np-93: neptune aqua / storm blue on deep navy
      UI_HEADER_BG    = 0x0043; // NeptuneNight
      UI_FREQ_COLOR   = 0x465C; // NeptuneAqua
      UI_LEVEL_COLOR  = 0x2418; // StormBlue
      UI_LABEL_COLOR  = 0x465C; // NeptuneAqua
      UI_DIM_COLOR    = 0x0086; // DeepNavy
      UI_BORDER_COLOR = 0x2418; // StormBlue
      break;
    case 94: // Plutao — Pu-94: Cherenkov blue-violet on abyssal black
      UI_HEADER_BG    = 0x0002; // AbyssalBlack
      UI_FREQ_COLOR   = 0x621F; // CherenkovBlue
      UI_LEVEL_COLOR  = 0x4118; // PlutoDark
      UI_LABEL_COLOR  = 0x621F; // CherenkovBlue
      UI_DIM_COLOR    = 0x1004; // VoidDim
      UI_BORDER_COLOR = 0x4118; // PlutoDark
      break;
    case 95: // America — Am-95: red / white / blue patriotic
      UI_HEADER_BG    = 0x0010; // NavyBlue
      UI_FREQ_COLOR   = 0xFFFF; // White
      UI_LEVEL_COLOR  = 0xF800; // Red
      UI_LABEL_COLOR  = 0xFFFF; // White
      UI_DIM_COLOR    = 0x0290; // NavyDim
      UI_BORDER_COLOR = 0xF800; // Red
      break;
    case 96: // Curie — Cm-96: Nobel gold / science blue on dark academic
      UI_HEADER_BG    = 0x1084; // AcademicDark
      UI_FREQ_COLOR   = 0xFE8C; // NobelGold
      UI_LEVEL_COLOR  = 0x441C; // ScienceBlue
      UI_LABEL_COLOR  = 0xFE8C; // NobelGold
      UI_DIM_COLOR    = 0x3148; // AcademicDim
      UI_BORDER_COLOR = 0x441C; // ScienceBlue
      break;
    case 97: // Berkeley — Bk-97: UC Berkeley blue / gold on deep navy
      UI_HEADER_BG    = 0x00C7; // BerkeleyBlue
      UI_FREQ_COLOR   = 0xFDA2; // BerkeleyGold
      UI_LEVEL_COLOR  = 0xED40; // CalGold
      UI_LABEL_COLOR  = 0xFDA2; // BerkeleyGold
      UI_DIM_COLOR    = 0x010A; // DeepNavy
      UI_BORDER_COLOR = 0xED40; // CalGold
      break;
    // ── Elements 98-118 ─────────────────────────────────────
    case 98: // Californio — Cf-98: California gold/blue
      UI_HEADER_BG    = 0x0061; // CalBlack
      UI_FREQ_COLOR   = 0xFDC0; // CalGold
      UI_LEVEL_COLOR  = 0x441A; // CalBlue
      UI_LABEL_COLOR  = 0xFDC0; // CalGold
      UI_DIM_COLOR    = 0x018C; // NavyDim
      UI_BORDER_COLOR = 0x441A; // CalBlue
      break;
    case 99: // Einstein — Es-99: cosmic silver/blue-white
      UI_HEADER_BG    = 0x0841; // CosmicBlack
      UI_FREQ_COLOR   = 0xC6DF; // StarWhite
      UI_LEVEL_COLOR  = 0x8498; // CosmicBlue
      UI_LABEL_COLOR  = 0xC6DF; // StarWhite
      UI_DIM_COLOR    = 0x3188; // SpaceDim
      UI_BORDER_COLOR = 0x8498; // CosmicBlue
      break;
    case 100: // Fermio — Fm-100: nuclear orange glow
      UI_HEADER_BG    = 0x1041; // ReactorBlack
      UI_FREQ_COLOR   = 0xFC08; // NuclearOrange
      UI_LEVEL_COLOR  = 0xF200; // CoreRed
      UI_LABEL_COLOR  = 0xFC08; // NuclearOrange
      UI_DIM_COLOR    = 0x4061; // ReactorDim
      UI_BORDER_COLOR = 0xF200; // CoreRed
      break;
    case 101: // Mendelevio — Md-101: periodic table purple
      UI_HEADER_BG    = 0x2006; // PeriodicDark
      UI_FREQ_COLOR   = 0xC31F; // PeriodicPurple
      UI_LEVEL_COLOR  = 0x8218; // PeriodicMid
      UI_LABEL_COLOR  = 0xC31F; // PeriodicPurple
      UI_DIM_COLOR    = 0x2808; // PeriodicDim
      UI_BORDER_COLOR = 0x8218; // PeriodicMid
      break;
    case 102: // Nobel — No-102: prestigious gold/black
      UI_HEADER_BG    = 0x0840; // NobelBlack
      UI_FREQ_COLOR   = 0xFF48; // NobelGold
      UI_LEVEL_COLOR  = 0xC500; // DarkGold
      UI_LABEL_COLOR  = 0xFF48; // NobelGold
      UI_DIM_COLOR    = 0x3960; // NobelDim
      UI_BORDER_COLOR = 0xC500; // DarkGold
      break;
    case 103: // Laurencio — Lr-103: lab acid green/blue
      UI_HEADER_BG    = 0x0044; // LabDark
      UI_FREQ_COLOR   = 0x87FF; // AcidGreen
      UI_LEVEL_COLOR  = 0x07E0; // PureGreen
      UI_LABEL_COLOR  = 0x87FF; // AcidGreen
      UI_DIM_COLOR    = 0x0288; // LabDim
      UI_BORDER_COLOR = 0x07E0; // PureGreen
      break;
    case 104: // Rutherford — Rf-104: British racing green
      UI_HEADER_BG    = 0x00C0; // RacingDark
      UI_FREQ_COLOR   = 0x0648; // RacingGreen
      UI_LEVEL_COLOR  = 0x0404; // DarkRacing
      UI_LABEL_COLOR  = 0x0648; // RacingGreen
      UI_DIM_COLOR    = 0x0180; // ForestDim
      UI_BORDER_COLOR = 0x0404; // DarkRacing
      break;
    case 105: // Dubnio — Db-105: Russian tricolor
      UI_HEADER_BG    = 0x0010; // RussianNavy
      UI_FREQ_COLOR   = 0xFFFF; // White
      UI_LEVEL_COLOR  = 0x001F; // Blue
      UI_LABEL_COLOR  = 0xFFFF; // White
      UI_DIM_COLOR    = 0x0290; // NavyDim
      UI_BORDER_COLOR = 0xF800; // Red
      break;
    case 106: // Seaborg — Sg-106: Pacific ocean blue
      UI_HEADER_BG    = 0x000C; // PacificDeep
      UI_FREQ_COLOR   = 0x055F; // PacificBlue
      UI_LEVEL_COLOR  = 0x041F; // OceanBlue
      UI_LABEL_COLOR  = 0x055F; // PacificBlue
      UI_DIM_COLOR    = 0x00C4; // OceanDim
      UI_BORDER_COLOR = 0x041F; // OceanBlue
      break;
    case 107: // Bohrio — Bh-107: Niels Bohr / Denmark red/white
      UI_HEADER_BG    = 0x3800; // DanishRed
      UI_FREQ_COLOR   = 0xFFFF; // White
      UI_LEVEL_COLOR  = 0xF800; // Red
      UI_LABEL_COLOR  = 0xFFFF; // White
      UI_DIM_COLOR    = 0x5800; // DarkRed
      UI_BORDER_COLOR = 0xF800; // Red
      break;
    case 108: // Hassio — Hs-108: Hessen forest green
      UI_HEADER_BG    = 0x0300; // ForestDeep
      UI_FREQ_COLOR   = 0x4640; // BrightForest
      UI_LEVEL_COLOR  = 0x2340; // ForestMid
      UI_LABEL_COLOR  = 0x4640; // BrightForest
      UI_DIM_COLOR    = 0x0580; // ForestDim2
      UI_BORDER_COLOR = 0x2340; // ForestMid
      break;
    case 109: // Meitner — Mt-109: Lise Meitner warm rose/peach
      UI_HEADER_BG    = 0x2843; // MeitnerDark
      UI_FREQ_COLOR   = 0xFEB6; // Peach
      UI_LEVEL_COLOR  = 0xF395; // Rose
      UI_LABEL_COLOR  = 0xFEB6; // Peach
      UI_DIM_COLOR    = 0x6108; // RoseDim
      UI_BORDER_COLOR = 0xF395; // Rose
      break;
    case 110: // Darmstadt — Ds-110: German industrial steel
      UI_HEADER_BG    = 0x2104; // SteelDark
      UI_FREQ_COLOR   = 0xAD75; // SteelSilver
      UI_LEVEL_COLOR  = 0x6318; // SteelMid
      UI_LABEL_COLOR  = 0xAD75; // SteelSilver
      UI_DIM_COLOR    = 0x39E7; // SteelDim
      UI_BORDER_COLOR = 0x6318; // SteelMid
      break;
    case 111: // Roentgen — Rg-111: X-ray white/blue on black
      UI_HEADER_BG    = 0x0000; // XrayBlack
      UI_FREQ_COLOR   = 0xFFFF; // XrayWhite
      UI_LEVEL_COLOR  = 0x45FF; // XrayBlue
      UI_LABEL_COLOR  = 0xFFFF; // XrayWhite
      UI_DIM_COLOR    = 0x2ACF; // XrayDim
      UI_BORDER_COLOR = 0x45FF; // XrayBlue
      break;
    case 112: // Copernicus — Cn-112: astronomical blue/gold
      UI_HEADER_BG    = 0x0008; // AstroBlack
      UI_FREQ_COLOR   = 0x441F; // AstroBlue
      UI_LEVEL_COLOR  = 0x051F; // DeepAstro
      UI_LABEL_COLOR  = 0xFDA2; // StarGold
      UI_DIM_COLOR    = 0x0043; // AstroDim
      UI_BORDER_COLOR = 0x051F; // DeepAstro
      break;
    case 113: // Nihonio — Nh-113: Japanese red/white
      UI_HEADER_BG    = 0x2904; // JapanDark
      UI_FREQ_COLOR   = 0xFFFF; // JapanWhite
      UI_LEVEL_COLOR  = 0xB805; // HinomaruRed
      UI_LABEL_COLOR  = 0xFFFF; // JapanWhite
      UI_DIM_COLOR    = 0x4208; // GreyDim
      UI_BORDER_COLOR = 0xB805; // HinomaruRed
      break;
    case 114: // Flerov — Fl-114: Russian blue/white/red variant
      UI_HEADER_BG    = 0x1800; // FlerovDark
      UI_FREQ_COLOR   = 0xFFFF; // White
      UI_LEVEL_COLOR  = 0x001F; // Blue
      UI_LABEL_COLOR  = 0xFFFF; // White
      UI_DIM_COLOR    = 0x3800; // RedDim
      UI_BORDER_COLOR = 0x001F; // Blue
      break;
    case 115: // Moscovio — Mc-115: Moscow red/gold
      UI_HEADER_BG    = 0x2800; // MoscowDark
      UI_FREQ_COLOR   = 0xFFE0; // MoscowGold
      UI_LEVEL_COLOR  = 0xF800; // MoscowRed
      UI_LABEL_COLOR  = 0xFFE0; // MoscowGold
      UI_DIM_COLOR    = 0x5800; // MoscowDim
      UI_BORDER_COLOR = 0xF800; // MoscowRed
      break;
    case 116: // Livermore — Lv-116: LLNL lime/yellow scientific
      UI_HEADER_BG    = 0x2140; // LabYellowDark
      UI_FREQ_COLOR   = 0xC7E0; // LimeLab
      UI_LEVEL_COLOR  = 0xFFE0; // Yellow
      UI_LABEL_COLOR  = 0xC7E0; // LimeLab
      UI_DIM_COLOR    = 0x6B20; // LabDim
      UI_BORDER_COLOR = 0xFFE0; // Yellow
      break;
    case 117: // Tennessio — Ts-117: Tennessee orange/blue
      UI_HEADER_BG    = 0x0908; // VolNavy
      UI_FREQ_COLOR   = 0xFC00; // VolOrange
      UI_LEVEL_COLOR  = 0x001F; // VolBlue
      UI_LABEL_COLOR  = 0xFC00; // VolOrange
      UI_DIM_COLOR    = 0x0290; // NavyDim
      UI_BORDER_COLOR = 0x001F; // VolBlue
      break;
    case 118: // Oganesson — Og-118: last element noble/regal silver
      UI_HEADER_BG    = 0x4208; // NobleDark
      UI_FREQ_COLOR   = 0xEF7D; // NobleWhite
      UI_LEVEL_COLOR  = 0xC618; // NobleSilver
      UI_LABEL_COLOR  = 0xEF7D; // NobleWhite
      UI_DIM_COLOR    = 0x6B4D; // NobleDim
      UI_BORDER_COLOR = 0xC618; // NobleSilver
      break;
    // ── Brazilian themes 119-140 ─────────────────────────────
    case 119: // Fortaleza — Ceará: navy/yellow/cyan
      UI_HEADER_BG    = 0x0010; // CearaBlue
      UI_FREQ_COLOR   = 0xFFE0; // SunYellow
      UI_LEVEL_COLOR  = 0x07C0; // CearaGreen
      UI_LABEL_COLOR  = 0xFFE0; // SunYellow
      UI_DIM_COLOR    = 0x0280; // NavyDim
      UI_BORDER_COLOR = 0x07C0; // CearaGreen
      break;
    case 120: // Salvador — Bahia: deep blue/cyan/tropical
      UI_HEADER_BG    = 0x0020; // BahiaBlue
      UI_FREQ_COLOR   = 0xFFE0; // BahiaGold
      UI_LEVEL_COLOR  = 0x07FF; // BahiaCyan
      UI_LABEL_COLOR  = 0xFFE0; // BahiaGold
      UI_DIM_COLOR    = 0x0290; // BahiaDim
      UI_BORDER_COLOR = 0x07FF; // BahiaCyan
      break;
    case 121: // Amazonia — deep jungle green
      UI_HEADER_BG    = 0x0100; // JungleDark
      UI_FREQ_COLOR   = 0x07E0; // JungleGreen
      UI_LEVEL_COLOR  = 0x0400; // DeepJungle
      UI_LABEL_COLOR  = 0x07E0; // JungleGreen
      UI_DIM_COLOR    = 0x0200; // JungleDim
      UI_BORDER_COLOR = 0x0400; // DeepJungle
      break;
    case 122: // Pantanal — wetland earth green/brown
      UI_HEADER_BG    = 0x2080; // WetlandDark
      UI_FREQ_COLOR   = 0x8E48; // WetlandGreen
      UI_LEVEL_COLOR  = 0x4504; // WetlandOlive
      UI_LABEL_COLOR  = 0x8E48; // WetlandGreen
      UI_DIM_COLOR    = 0x2242; // WetlandDim
      UI_BORDER_COLOR = 0x4504; // WetlandOlive
      break;
    case 123: // Caatinga — arid scrubland: burnt orange/rust
      UI_HEADER_BG    = 0x2860; // AridDark
      UI_FREQ_COLOR   = 0xFCC0; // AridOrange
      UI_LEVEL_COLOR  = 0xCA41; // AridRust
      UI_LABEL_COLOR  = 0xFCC0; // AridOrange
      UI_DIM_COLOR    = 0x6180; // AridDim
      UI_BORDER_COLOR = 0xCA41; // AridRust
      break;
    case 124: // Cerrado — Brazilian savanna: gold/dark green
      UI_HEADER_BG    = 0x1840; // SavannaGrey
      UI_FREQ_COLOR   = 0xFFE0; // SavannaGold
      UI_LEVEL_COLOR  = 0x6544; // SavannaGreen
      UI_LABEL_COLOR  = 0xFFE0; // SavannaGold
      UI_DIM_COLOR    = 0x30C0; // SavannaDim
      UI_BORDER_COLOR = 0x6544; // SavannaGreen
      break;
    case 125: // Samba — festive: yellow on dark green
      UI_HEADER_BG    = 0x0200; // SambaDark
      UI_FREQ_COLOR   = 0xFFE0; // SambaYellow
      UI_LEVEL_COLOR  = 0x07E0; // SambaGreen
      UI_LABEL_COLOR  = 0xFFE0; // SambaYellow
      UI_DIM_COLOR    = 0x0480; // SambaDim
      UI_BORDER_COLOR = 0x07E0; // SambaGreen
      break;
    case 126: // Carnaval — rainbow festive: magenta/cyan/yellow
      UI_HEADER_BG    = 0x2004; // CarnavalDark
      UI_FREQ_COLOR   = 0xF81F; // CarnavalMagenta
      UI_LEVEL_COLOR  = 0x07FF; // CarnavalCyan
      UI_LABEL_COLOR  = 0xFFE0; // CarnavalYellow
      UI_DIM_COLOR    = 0x600C; // CarnavalDim
      UI_BORDER_COLOR = 0x07FF; // CarnavalCyan
      break;
    case 127: // Frevo — Pernambuco dance: hot yellow/red
      UI_HEADER_BG    = 0x2800; // FrevoDark
      UI_FREQ_COLOR   = 0xFFE0; // FrevoYellow
      UI_LEVEL_COLOR  = 0xF800; // FrevoRed
      UI_LABEL_COLOR  = 0xFFE0; // FrevoYellow
      UI_DIM_COLOR    = 0x5800; // FrevoDim
      UI_BORDER_COLOR = 0xF800; // FrevoRed
      break;
    case 128: // Forro — northeastern: warm caramel/brown
      UI_HEADER_BG    = 0x1840; // ForroDark
      UI_FREQ_COLOR   = 0xFCC0; // ForroCaramel
      UI_LEVEL_COLOR  = 0xAB04; // ForroBrown
      UI_LABEL_COLOR  = 0xFCC0; // ForroCaramel
      UI_DIM_COLOR    = 0x6182; // ForroDim
      UI_BORDER_COLOR = 0xAB04; // ForroBrown
      break;
    case 129: // BossaNova — cool Rio jazz: blue-grey/beige
      UI_HEADER_BG    = 0x1084; // BossaDark
      UI_FREQ_COLOR   = 0xC6DF; // BossaBeige
      UI_LEVEL_COLOR  = 0x041F; // BossaBlue
      UI_LABEL_COLOR  = 0xC6DF; // BossaBeige
      UI_DIM_COLOR    = 0x3188; // BossaDim
      UI_BORDER_COLOR = 0x041F; // BossaBlue
      break;
    case 130: // Tropicalia — psychedelic: yellow/magenta on green
      UI_HEADER_BG    = 0x0200; // TropicDark
      UI_FREQ_COLOR   = 0xFFE0; // TropicYellow
      UI_LEVEL_COLOR  = 0xF81F; // TropicMagenta
      UI_LABEL_COLOR  = 0xFFE0; // TropicYellow
      UI_DIM_COLOR    = 0x0480; // TropicDim
      UI_BORDER_COLOR = 0xF81F; // TropicMagenta
      break;
    case 131: // Copa — World Cup: gold/green on black
      UI_HEADER_BG    = 0x0840; // CopaBlack
      UI_FREQ_COLOR   = 0xFFE0; // CopaGold
      UI_LEVEL_COLOR  = 0x07E0; // CopaGreen
      UI_LABEL_COLOR  = 0xFFFF; // CopaWhite
      UI_DIM_COLOR    = 0x4960; // CopaDim
      UI_BORDER_COLOR = 0x07E0; // CopaGreen
      break;
    case 132: // Pele — black/gold: the king
      UI_HEADER_BG    = 0x0840; // PeleBlack
      UI_FREQ_COLOR   = 0xFFE0; // PeleGold
      UI_LEVEL_COLOR  = 0xFD20; // PeleAmber
      UI_LABEL_COLOR  = 0xFFE0; // PeleGold
      UI_DIM_COLOR    = 0x4960; // PeleDim
      UI_BORDER_COLOR = 0xFD20; // PeleAmber
      break;
    case 133: // Senna — McLaren red/white
      UI_HEADER_BG    = 0x1800; // SennaDark
      UI_FREQ_COLOR   = 0xFFFF; // SennaWhite
      UI_LEVEL_COLOR  = 0xF800; // SennaRed
      UI_LABEL_COLOR  = 0xFFFF; // SennaWhite
      UI_DIM_COLOR    = 0x5800; // SennaDim
      UI_BORDER_COLOR = 0xF800; // SennaRed
      break;
    case 134: // Acai — deep berry purple
      UI_HEADER_BG    = 0x1805; // AcaiBerry
      UI_FREQ_COLOR   = 0xB01F; // AcaiPurple
      UI_LEVEL_COLOR  = 0x6014; // AcaiDark
      UI_LABEL_COLOR  = 0xB01F; // AcaiPurple
      UI_DIM_COLOR    = 0x380B; // AcaiDim
      UI_BORDER_COLOR = 0x6014; // AcaiDark
      break;
    case 135: // Guarana — earthy olive/brown
      UI_HEADER_BG    = 0x0900; // GuaranaDark
      UI_FREQ_COLOR   = 0x8E48; // GuaranaGreen
      UI_LEVEL_COLOR  = 0x4504; // GuaranaOlive
      UI_LABEL_COLOR  = 0x8E48; // GuaranaGreen
      UI_DIM_COLOR    = 0x1983; // GuaranaDim
      UI_BORDER_COLOR = 0x4504; // GuaranaOlive
      break;
    case 136: // Cafe — coffee brown/cream
      UI_HEADER_BG    = 0x1040; // CafeDark
      UI_FREQ_COLOR   = 0xFF79; // CafeCream
      UI_LEVEL_COLOR  = 0xA340; // CafeBrown
      UI_LABEL_COLOR  = 0xFF79; // CafeCream
      UI_DIM_COLOR    = 0x38E1; // CafeSepia
      UI_BORDER_COLOR = 0xA340; // CafeBrown
      break;
    case 137: // Cachaca — golden spirit/amber
      UI_HEADER_BG    = 0x18A0; // CachaçaDark
      UI_FREQ_COLOR   = 0xFF50; // CachaçaChampagne
      UI_LEVEL_COLOR  = 0xFE60; // CachaçaHoney
      UI_LABEL_COLOR  = 0xFF50; // CachaçaChampagne
      UI_DIM_COLOR    = 0x8308; // CachaçaDim
      UI_BORDER_COLOR = 0xFE60; // CachaçaHoney
      break;
    case 138: // Tapioca — soft white/cream
      UI_HEADER_BG    = 0x4208; // TapiocaDark
      UI_FREQ_COLOR   = 0xFFFF; // TapiocaWhite
      UI_LEVEL_COLOR  = 0xEF7D; // TapiocaCream
      UI_LABEL_COLOR  = 0xFFFF; // TapiocaWhite
      UI_DIM_COLOR    = 0x7BEF; // TapiocaGrey
      UI_BORDER_COLOR = 0xBDF7; // TapiocaLight
      break;
    case 139: // Caju — cashew orange/yellow
      UI_HEADER_BG    = 0x1880; // CajuDark
      UI_FREQ_COLOR   = 0xFB80; // CajuOrange
      UI_LEVEL_COLOR  = 0xFD40; // CajuGold
      UI_LABEL_COLOR  = 0xFB80; // CajuOrange
      UI_DIM_COLOR    = 0x30C0; // CajuDim
      UI_BORDER_COLOR = 0xFD40; // CajuGold
      break;
    case 140: // Jabuticaba — black berry: dark purple/black
      UI_HEADER_BG    = 0x1804; // JabuDark
      UI_FREQ_COLOR   = 0x780F; // JabuPurple
      UI_LEVEL_COLOR  = 0x400C; // JabuDeep
      UI_LABEL_COLOR  = 0x780F; // JabuPurple
      UI_DIM_COLOR    = 0x1806; // JabuDim
      UI_BORDER_COLOR = 0x400C; // JabuDeep
      break;
    // ── Cultural themes 141-165 ──────────────────────────────
    case 141: // Jazz — smoky steel blue/grey
      UI_HEADER_BG    = 0x18C3; // JazzSmoke
      UI_FREQ_COLOR   = 0x55BF; // JazzBlue
      UI_LEVEL_COLOR  = 0x2ACF; // JazzSteel
      UI_LABEL_COLOR  = 0x55BF; // JazzBlue
      UI_DIM_COLOR    = 0x4208; // JazzDim
      UI_BORDER_COLOR = 0x2ACF; // JazzSteel
      break;
    case 142: // Punk — neon green/pink on black
      UI_HEADER_BG    = 0x0000; // PunkBlack
      UI_FREQ_COLOR   = 0x87E0; // PunkGreen
      UI_LEVEL_COLOR  = 0xF810; // PunkPink
      UI_LABEL_COLOR  = 0x87E0; // PunkGreen
      UI_DIM_COLOR    = 0x300A; // PunkDim
      UI_BORDER_COLOR = 0xF810; // PunkPink
      break;
    case 143: // Grunge — dark murky grey/brown
      UI_HEADER_BG    = 0x2965; // GrungeGrey
      UI_FREQ_COLOR   = 0x9492; // GrungeMid
      UI_LEVEL_COLOR  = 0x6B4D; // GrungeDark
      UI_LABEL_COLOR  = 0x9492; // GrungeMid
      UI_DIM_COLOR    = 0x4A09; // GrungeDim
      UI_BORDER_COLOR = 0x6B4D; // GrungeDark
      break;
    case 144: // Reggae — Rasta green/yellow/red
      UI_HEADER_BG    = 0x0200; // ReggaeDark
      UI_FREQ_COLOR   = 0xFFE0; // ReggaeYellow
      UI_LEVEL_COLOR  = 0xF800; // ReggaeRed
      UI_LABEL_COLOR  = 0x07E0; // ReggaeGreen
      UI_DIM_COLOR    = 0x0480; // ReggaeDim
      UI_BORDER_COLOR = 0xFFE0; // ReggaeYellow
      break;
    case 145: // Funk — warm orange/brown/gold
      UI_HEADER_BG    = 0x1880; // FunkDark
      UI_FREQ_COLOR   = 0xFDA2; // FunkGold
      UI_LEVEL_COLOR  = 0xFC80; // FunkOrange
      UI_LABEL_COLOR  = 0xFDA2; // FunkGold
      UI_DIM_COLOR    = 0x30C0; // FunkDim
      UI_BORDER_COLOR = 0xFC80; // FunkOrange
      break;
    case 146: // Rock — dark grey/electric cyan
      UI_HEADER_BG    = 0x18C3; // RockDark
      UI_FREQ_COLOR   = 0xC618; // RockSilver
      UI_LEVEL_COLOR  = 0x07FF; // ElectricCyan
      UI_LABEL_COLOR  = 0xC618; // RockSilver
      UI_DIM_COLOR    = 0x4208; // RockDim
      UI_BORDER_COLOR = 0x07FF; // ElectricCyan
      break;
    case 147: // Anime — pastel pink/periwinkle
      UI_HEADER_BG    = 0x2843; // AnimeDark
      UI_FREQ_COLOR   = 0xFD9A; // AnimePink
      UI_LEVEL_COLOR  = 0xAEBF; // AnimeBlue
      UI_LABEL_COLOR  = 0xFD9A; // AnimePink
      UI_DIM_COLOR    = 0x6108; // AnimeDim
      UI_BORDER_COLOR = 0xAEBF; // AnimeBlue
      break;
    case 148: // Manga2 — comic: white/red on black
      UI_HEADER_BG    = 0x0000; // MangaBlack
      UI_FREQ_COLOR   = 0xFFFF; // MangaWhite
      UI_LEVEL_COLOR  = 0xF800; // MangaRed
      UI_LABEL_COLOR  = 0xFFFF; // MangaWhite
      UI_DIM_COLOR    = 0x2965; // MangaDim
      UI_BORDER_COLOR = 0xF800; // MangaRed
      break;
    case 149: // Cyberpunk — neon purple/cyan on dark maroon
      UI_HEADER_BG    = 0x2003; // CyberDark
      UI_FREQ_COLOR   = 0xF810; // CyberPink
      UI_LEVEL_COLOR  = 0x07FF; // CyberCyan
      UI_LABEL_COLOR  = 0xF810; // CyberPink
      UI_DIM_COLOR    = 0x5007; // CyberDim
      UI_BORDER_COLOR = 0x07FF; // CyberCyan
      break;
    case 150: // Steampunk — brass/copper/bronze
      UI_HEADER_BG    = 0x2081; // SteamDark
      UI_FREQ_COLOR   = 0xD344; // BrassCopper
      UI_LEVEL_COLOR  = 0xA243; // DarkBrass
      UI_LABEL_COLOR  = 0xD344; // BrassCopper
      UI_DIM_COLOR    = 0x6142; // SteamDim
      UI_BORDER_COLOR = 0xA243; // DarkBrass
      break;
    case 151: // Gothic — deep red/purple on near-black
      UI_HEADER_BG    = 0x3006; // GothicBlack
      UI_FREQ_COLOR   = 0xF80C; // GothicRed
      UI_LEVEL_COLOR  = 0x900C; // GothicPurple
      UI_LABEL_COLOR  = 0xF80C; // GothicRed
      UI_DIM_COLOR    = 0x4804; // GothicDim
      UI_BORDER_COLOR = 0x900C; // GothicPurple
      break;
    case 152: // ArtDeco — gold/silver on black
      UI_HEADER_BG    = 0x0000; // DecoBlack
      UI_FREQ_COLOR   = 0xFF48; // DecoGold
      UI_LEVEL_COLOR  = 0xC618; // DecoSilver
      UI_LABEL_COLOR  = 0xFF48; // DecoGold
      UI_DIM_COLOR    = 0x4208; // DecoDim
      UI_BORDER_COLOR = 0xC618; // DecoSilver
      break;
    case 153: // Bauhaus — primary: red/blue/yellow on black
      UI_HEADER_BG    = 0x0000; // BauhausBlack
      UI_FREQ_COLOR   = 0xF800; // BauhausRed
      UI_LEVEL_COLOR  = 0x001F; // BauhausBlue
      UI_LABEL_COLOR  = 0xFFE0; // BauhausYellow
      UI_DIM_COLOR    = 0x2965; // BauhausDim
      UI_BORDER_COLOR = 0x07E0; // BauhausGreen
      break;
    case 154: // Vaporwave — pink/cyan pastel on dark
      UI_HEADER_BG    = 0x2843; // VaporDark
      UI_FREQ_COLOR   = 0xF810; // VaporPink
      UI_LEVEL_COLOR  = 0xAEBF; // VaporCyan
      UI_LABEL_COLOR  = 0xFC1F; // VaporMagenta
      UI_DIM_COLOR    = 0x610C; // VaporDim
      UI_BORDER_COLOR = 0x07FF; // VaporBlue
      break;
    case 155: // Lofi — warm chill: caramel/beige
      UI_HEADER_BG    = 0x2080; // LofiWarm
      UI_FREQ_COLOR   = 0xF4C4; // LofiBeige
      UI_LEVEL_COLOR  = 0xA340; // LofiCamel
      UI_LABEL_COLOR  = 0xF4C4; // LofiBeige
      UI_DIM_COLOR    = 0x6182; // LofiDim
      UI_BORDER_COLOR = 0xA340; // LofiCamel
      break;
    case 156: // Y2K — holographic silver/cyan
      UI_HEADER_BG    = 0x4208; // Y2KDark
      UI_FREQ_COLOR   = 0xEF7D; // Y2KSilver
      UI_LEVEL_COLOR  = 0x07FF; // Y2KCyan
      UI_LABEL_COLOR  = 0xEF7D; // Y2KSilver
      UI_DIM_COLOR    = 0x7BEF; // Y2KDim
      UI_BORDER_COLOR = 0x07FF; // Y2KCyan
      break;
    case 157: // Memphis — bold geometric: pink/yellow/cyan
      UI_HEADER_BG    = 0x0000; // MemphisBlack
      UI_FREQ_COLOR   = 0xF810; // MemphisPink
      UI_LEVEL_COLOR  = 0xFFE0; // MemphisYellow
      UI_LABEL_COLOR  = 0xF81F; // MemphisMagenta
      UI_DIM_COLOR    = 0x2965; // MemphisDim
      UI_BORDER_COLOR = 0x07FF; // MemphisCyan
      break;
    case 158: // Minimal — pure black/white/grey
      UI_HEADER_BG    = 0x0000; // MinimalBlack
      UI_FREQ_COLOR   = 0xFFFF; // MinimalWhite
      UI_LEVEL_COLOR  = 0xC618; // MinimalGrey
      UI_LABEL_COLOR  = 0xEF7D; // MinimalLight
      UI_DIM_COLOR    = 0x6B4D; // MinimalDim
      UI_BORDER_COLOR = 0xC618; // MinimalGrey
      break;
    case 159: // Nordic — Scandinavian ice white/blue
      UI_HEADER_BG    = 0x0021; // NordicNight
      UI_FREQ_COLOR   = 0xFFFF; // NordicWhite
      UI_LEVEL_COLOR  = 0x45FF; // NordicBlue
      UI_LABEL_COLOR  = 0xFFFF; // NordicWhite
      UI_DIM_COLOR    = 0x1A2F; // NordicDim
      UI_BORDER_COLOR = 0x231A; // NordicFrame
      break;
    case 160: // Tropical — vivid green/orange/pink
      UI_HEADER_BG    = 0x0200; // TropVivDark
      UI_FREQ_COLOR   = 0xFC80; // TropOrange
      UI_LEVEL_COLOR  = 0xF810; // TropPink
      UI_LABEL_COLOR  = 0xFC80; // TropOrange
      UI_DIM_COLOR    = 0x0480; // TropDim
      UI_BORDER_COLOR = 0xF810; // TropPink
      break;
    case 161: // Safari — earth/khaki/brown
      UI_HEADER_BG    = 0x2860; // SafariDark
      UI_FREQ_COLOR   = 0xFCC0; // SafariKhaki
      UI_LEVEL_COLOR  = 0x8E48; // SafariOlive
      UI_LABEL_COLOR  = 0xFCC0; // SafariKhaki
      UI_DIM_COLOR    = 0x6180; // SafariDim
      UI_BORDER_COLOR = 0x8E48; // SafariOlive
      break;
    case 162: // Arctic — cold white/pale blue
      UI_HEADER_BG    = 0x1084; // ArcticDark
      UI_FREQ_COLOR   = 0xFFFF; // ArcticWhite
      UI_LEVEL_COLOR  = 0xC6DF; // ArcticPale
      UI_LABEL_COLOR  = 0xFFFF; // ArcticWhite
      UI_DIM_COLOR    = 0x2ACF; // ArcticDim
      UI_BORDER_COLOR = 0xAEBF; // ArcticIce
      break;
    case 163: // Desert — sand/terracotta
      UI_HEADER_BG    = 0x3100; // DesertDark
      UI_FREQ_COLOR   = 0xFF79; // DesertSand
      UI_LEVEL_COLOR  = 0xED40; // DesertYellow
      UI_LABEL_COLOR  = 0xFF79; // DesertSand
      UI_DIM_COLOR    = 0x6180; // DesertDim
      UI_BORDER_COLOR = 0xED40; // DesertYellow
      break;
    case 164: // Monsoon — deep teal/slate
      UI_HEADER_BG    = 0x00C3; // MonsoonDeep
      UI_FREQ_COLOR   = 0x055F; // MonsoonBlue
      UI_LEVEL_COLOR  = 0x0410; // MonsoonTeal
      UI_LABEL_COLOR  = 0x055F; // MonsoonBlue
      UI_DIM_COLOR    = 0x0262; // MonsoonDim
      UI_BORDER_COLOR = 0x0410; // MonsoonTeal
      break;
    case 165: // Midnight2 — deep purple/lavender night
      UI_HEADER_BG    = 0x1806; // MidnightDeep
      UI_FREQ_COLOR   = 0xC41E; // MidnightLav
      UI_LEVEL_COLOR  = 0x4118; // MidnightMid
      UI_LABEL_COLOR  = 0xC41E; // MidnightLav
      UI_DIM_COLOR    = 0x400C; // MidnightDim
      UI_BORDER_COLOR = 0x4118; // MidnightMid
      break;
    // ── Gems 166-183 ─────────────────────────────────────────
    case 166: // Rubi — ruby deep red
      UI_HEADER_BG    = 0x2800; // RubyDark
      UI_FREQ_COLOR   = 0xF80C; // RubyRed
      UI_LEVEL_COLOR  = 0xD008; // RubyDeep
      UI_LABEL_COLOR  = 0xF80C; // RubyRed
      UI_DIM_COLOR    = 0x6006; // RubyDim
      UI_BORDER_COLOR = 0xD008; // RubyDeep
      break;
    case 167: // Topazio — topaz blue/gold
      UI_HEADER_BG    = 0x0043; // TopazDark
      UI_FREQ_COLOR   = 0xFDA2; // TopazGold
      UI_LEVEL_COLOR  = 0x441A; // TopazBlue
      UI_LABEL_COLOR  = 0xFDA2; // TopazGold
      UI_DIM_COLOR    = 0x010A; // TopazDim
      UI_BORDER_COLOR = 0x441A; // TopazBlue
      break;
    case 168: // Opala — opal multicolor: pink/cyan/gold
      UI_HEADER_BG    = 0x0841; // OpalDark
      UI_FREQ_COLOR   = 0xFC1F; // OpalPink
      UI_LEVEL_COLOR  = 0x07FF; // OpalCyan
      UI_LABEL_COLOR  = 0xFF48; // OpalGold
      UI_DIM_COLOR    = 0x300A; // OpalDim
      UI_BORDER_COLOR = 0x07E0; // OpalGreen
      break;
    case 169: // Jade — jade green
      UI_HEADER_BG    = 0x0060; // JadeDark
      UI_FREQ_COLOR   = 0x45C8; // JadeGreen
      UI_LEVEL_COLOR  = 0x2404; // JadeDeep
      UI_LABEL_COLOR  = 0x45C8; // JadeGreen
      UI_DIM_COLOR    = 0x1983; // JadeDim
      UI_BORDER_COLOR = 0x3306; // JadeBorder
      break;
    case 170: // Onix — onyx black/bright silver
      UI_HEADER_BG    = 0x0000; // OnyxBlack
      UI_FREQ_COLOR   = 0xC618; // OnyxSilver
      UI_LEVEL_COLOR  = 0xEF7D; // OnyxLight
      UI_LABEL_COLOR  = 0xC618; // OnyxSilver
      UI_DIM_COLOR    = 0x4208; // OnyxDim
      UI_BORDER_COLOR = 0xEF7D; // OnyxLight
      break;
    case 171: // Perola — pearl cool white
      UI_HEADER_BG    = 0x2104; // PearlDark
      UI_FREQ_COLOR   = 0xF7BE; // PearlWhite
      UI_LEVEL_COLOR  = 0xC618; // PearlSilver
      UI_LABEL_COLOR  = 0xF7BE; // PearlWhite
      UI_DIM_COLOR    = 0x7BEF; // PearlDim
      UI_BORDER_COLOR = 0x9CF3; // PearlFrame
      break;
    case 172: // Diamante — diamond white/electric blue
      UI_HEADER_BG    = 0x0841; // DiamondDark
      UI_FREQ_COLOR   = 0xFFFF; // DiamondWhite
      UI_LEVEL_COLOR  = 0xAEBF; // DiamondIce
      UI_LABEL_COLOR  = 0xFFFF; // DiamondWhite
      UI_DIM_COLOR    = 0x2ACF; // DiamondDim
      UI_BORDER_COLOR = 0x87FF; // DiamondBlue
      break;
    case 173: // Aquamarine — aquamarine sky blue
      UI_HEADER_BG    = 0x0043; // AquaDark
      UI_FREQ_COLOR   = 0x5DFF; // AquaSky
      UI_LEVEL_COLOR  = 0x07FE; // AquaTurq
      UI_LABEL_COLOR  = 0x5DFF; // AquaSky
      UI_DIM_COLOR    = 0x018C; // AquaDim
      UI_BORDER_COLOR = 0x07FE; // AquaTurq
      break;
    case 174: // Citrino — citrine yellow/gold
      UI_HEADER_BG    = 0x2140; // CitrineDark
      UI_FREQ_COLOR   = 0xFFE0; // CitrineYellow
      UI_LEVEL_COLOR  = 0xED40; // CitrineGold
      UI_LABEL_COLOR  = 0xFFE0; // CitrineYellow
      UI_DIM_COLOR    = 0x6B20; // CitrineDim
      UI_BORDER_COLOR = 0xED40; // CitrineGold
      break;
    case 175: // Granada — garnet deep red/rose
      UI_HEADER_BG    = 0x2000; // GarnetDark
      UI_FREQ_COLOR   = 0xE800; // GarnetRed
      UI_LEVEL_COLOR  = 0xD004; // GarnetDeep
      UI_LABEL_COLOR  = 0xFACA; // GarnetRose
      UI_DIM_COLOR    = 0x5004; // GarnetDim
      UI_BORDER_COLOR = 0xE800; // GarnetRed
      break;
    case 176: // Lazuli — lapis lazuli blue/gold
      UI_HEADER_BG    = 0x0008; // LazuliDeep
      UI_FREQ_COLOR   = 0xFDA2; // LazuliGold
      UI_LEVEL_COLOR  = 0x231A; // LazuliBlue
      UI_LABEL_COLOR  = 0xFDA2; // LazuliGold
      UI_DIM_COLOR    = 0x0086; // LazuliDim
      UI_BORDER_COLOR = 0x231A; // LazuliBlue
      break;
    case 177: // Malaquita — malachite green/teal
      UI_HEADER_BG    = 0x08C3; // MalachDark
      UI_FREQ_COLOR   = 0x4634; // MalachGreen
      UI_LEVEL_COLOR  = 0x240C; // MalachTeal
      UI_LABEL_COLOR  = 0x4634; // MalachGreen
      UI_DIM_COLOR    = 0x11C5; // MalachDim
      UI_BORDER_COLOR = 0x240C; // MalachTeal
      break;
    case 178: // Turmalina — tourmaline pink/green
      UI_HEADER_BG    = 0x2843; // TourDark
      UI_FREQ_COLOR   = 0xFC19; // TourPink
      UI_LEVEL_COLOR  = 0x45C8; // TourGreen
      UI_LABEL_COLOR  = 0xFC19; // TourPink
      UI_DIM_COLOR    = 0x610C; // TourDim
      UI_BORDER_COLOR = 0x45C8; // TourGreen
      break;
    case 179: // Alexandrita — alexandrite: purple shifts to green
      UI_HEADER_BG    = 0x2006; // AlexDark
      UI_FREQ_COLOR   = 0xC31F; // AlexPurple
      UI_LEVEL_COLOR  = 0x45C8; // AlexGreen
      UI_LABEL_COLOR  = 0xC31F; // AlexPurple
      UI_DIM_COLOR    = 0x2808; // AlexDim
      UI_BORDER_COLOR = 0x45C8; // AlexGreen
      break;
    case 180: // Tanzanita — tanzanite blue-violet
      UI_HEADER_BG    = 0x1806; // TanzDark
      UI_FREQ_COLOR   = 0x821A; // TanzViolet
      UI_LEVEL_COLOR  = 0x4118; // TanzBlue
      UI_LABEL_COLOR  = 0xC41E; // TanzLavender
      UI_DIM_COLOR    = 0x400C; // TanzDim
      UI_BORDER_COLOR = 0x821A; // TanzViolet
      break;
    case 181: // Zircao — zircon blue/silver
      UI_HEADER_BG    = 0x0841; // ZirconDark
      UI_FREQ_COLOR   = 0xAEBF; // ZirconBlue
      UI_LEVEL_COLOR  = 0xEF7D; // ZirconSilver
      UI_LABEL_COLOR  = 0xAEBF; // ZirconBlue
      UI_DIM_COLOR    = 0x2ACF; // ZirconDim
      UI_BORDER_COLOR = 0xEF7D; // ZirconSilver
      break;
    case 182: // Espinelio — spinel red/deep pink
      UI_HEADER_BG    = 0x2800; // SpinelDark
      UI_FREQ_COLOR   = 0xF80C; // SpinelRed
      UI_LEVEL_COLOR  = 0xFA18; // SpinelPink
      UI_LABEL_COLOR  = 0xFD9A; // SpinelLight
      UI_DIM_COLOR    = 0x6006; // SpinelDim
      UI_BORDER_COLOR = 0xF214; // SpinelBorder
      break;
    case 183: // Kunzita — kunzite pink/lilac
      UI_HEADER_BG    = 0x2843; // KunzDark
      UI_FREQ_COLOR   = 0xFD9A; // KunzPink
      UI_LEVEL_COLOR  = 0x9318; // KunzLilac
      UI_LABEL_COLOR  = 0xFD9A; // KunzPink
      UI_DIM_COLOR    = 0x400C; // KunzDim
      UI_BORDER_COLOR = 0x9318; // KunzLilac
      break;
    // ── Elements / concepts 184-195 ──────────────────────────
    case 184: // Heliodoro — heliodor gem: sun gold/warm
      UI_HEADER_BG    = 0x18A0; // HelioDark
      UI_FREQ_COLOR   = 0xFF50; // HelioGold
      UI_LEVEL_COLOR  = 0xED40; // HelioBright
      UI_LABEL_COLOR  = 0xFF50; // HelioGold
      UI_DIM_COLOR    = 0x8308; // HelioDim
      UI_BORDER_COLOR = 0xED40; // HelioBright
      break;
    case 185: // Bicolor — purple/green split
      UI_HEADER_BG    = 0x0200; // BiDark
      UI_FREQ_COLOR   = 0x821A; // BiPurple
      UI_LEVEL_COLOR  = 0xC41E; // BiLavender
      UI_LABEL_COLOR  = 0x821A; // BiPurple
      UI_DIM_COLOR    = 0x0480; // BiDim
      UI_BORDER_COLOR = 0xC41E; // BiLavender
      break;
    case 186: // RetroCRT — warm phosphor amber CRT
      UI_HEADER_BG    = 0x28A0; // CRTDark
      UI_FREQ_COLOR   = 0xFEA0; // CRTAmber
      UI_LEVEL_COLOR  = 0xFD20; // CRTOrange
      UI_LABEL_COLOR  = 0xFDA0; // CRTWarm
      UI_DIM_COLOR    = 0x6300; // CRTDim
      UI_BORDER_COLOR = 0xFD20; // CRTOrange
      break;
    case 187: // Futuro — sci-fi future: cyan/violet
      UI_HEADER_BG    = 0x0002; // FutureDark
      UI_FREQ_COLOR   = 0x07FF; // FutureCyan
      UI_LEVEL_COLOR  = 0x821F; // FutureViolet
      UI_LABEL_COLOR  = 0x07FF; // FutureCyan
      UI_DIM_COLOR    = 0x300A; // FutureDim
      UI_BORDER_COLOR = 0x821F; // FutureViolet
      break;
    case 188: // Espaco — deep space: pink nebula/violet
      UI_HEADER_BG    = 0x0001; // SpaceDark
      UI_FREQ_COLOR   = 0xFC1F; // SpacePink
      UI_LEVEL_COLOR  = 0x821F; // SpaceViolet
      UI_LABEL_COLOR  = 0xFC1F; // SpacePink
      UI_DIM_COLOR    = 0x300A; // SpaceDim
      UI_BORDER_COLOR = 0x4118; // SpaceBorder
      break;
    case 189: // Terra — earth: brown/olive/blue
      UI_HEADER_BG    = 0x1840; // EarthDk
      UI_FREQ_COLOR   = 0xA340; // EarthBrown
      UI_LEVEL_COLOR  = 0x8E48; // EarthGreen
      UI_LABEL_COLOR  = 0xA340; // EarthBrown
      UI_DIM_COLOR    = 0x2242; // EarthDim
      UI_BORDER_COLOR = 0xCA41; // EarthRust
      break;
    case 190: // Fogo — fire: deep red/lava orange
      UI_HEADER_BG    = 0x0800; // FireBlack
      UI_FREQ_COLOR   = 0xF900; // FireRed
      UI_LEVEL_COLOR  = 0xFB40; // FireOrange
      UI_LABEL_COLOR  = 0xF900; // FireRed
      UI_DIM_COLOR    = 0x3840; // FireDim
      UI_BORDER_COLOR = 0xFB40; // FireOrange
      break;
    case 191: // Agua — water: bright aqua/teal
      UI_HEADER_BG    = 0x00C4; // WaterDeep
      UI_FREQ_COLOR   = 0x07FE; // WaterCyan
      UI_LEVEL_COLOR  = 0x0718; // WaterTeal
      UI_LABEL_COLOR  = 0x07FE; // WaterCyan
      UI_DIM_COLOR    = 0x0188; // WaterDim
      UI_BORDER_COLOR = 0x0718; // WaterTeal
      break;
    case 192: // Ar — air: pale sky blue/white
      UI_HEADER_BG    = 0x1084; // AirDark
      UI_FREQ_COLOR   = 0xC6DF; // AirPale
      UI_LEVEL_COLOR  = 0x65FF; // AirBlue
      UI_LABEL_COLOR  = 0xC6DF; // AirPale
      UI_DIM_COLOR    = 0x2ACF; // AirDim
      UI_BORDER_COLOR = 0xAEBF; // AirIce
      break;
    case 193: // Metal — metallic steel grey
      UI_HEADER_BG    = 0x4208; // MetalDark
      UI_FREQ_COLOR   = 0xC618; // MetalGrey
      UI_LEVEL_COLOR  = 0x8410; // MetalMid
      UI_LABEL_COLOR  = 0xEF7D; // MetalLight
      UI_DIM_COLOR    = 0x6B4D; // MetalDim
      UI_BORDER_COLOR = 0x8C71; // MetalSteel
      break;
    case 194: // Madeira — wood: warm brown/amber
      UI_HEADER_BG    = 0x1040; // WoodDark
      UI_FREQ_COLOR   = 0xD344; // WoodCopper
      UI_LEVEL_COLOR  = 0xA243; // WoodBrown
      UI_LABEL_COLOR  = 0xCA41; // WoodRust
      UI_DIM_COLOR    = 0x38E1; // WoodDim
      UI_BORDER_COLOR = 0x6142; // WoodBorder
      break;
    case 195: // Pedra — stone: blue-grey earth
      UI_HEADER_BG    = 0x2965; // StoneDark
      UI_FREQ_COLOR   = 0x9492; // StoneMid
      UI_LEVEL_COLOR  = 0x7412; // StoneBlue
      UI_LABEL_COLOR  = 0x8559; // StoneLight
      UI_DIM_COLOR    = 0x320A; // StoneDim
      UI_BORDER_COLOR = 0x5414; // StoneBorder
      break;
    case 196: // Gelo — frost: deep ice / pale arctic blue
      UI_HEADER_BG    = 0x0865; // IceDark
      UI_FREQ_COLOR   = 0xCF1F; // IceWhite
      UI_LEVEL_COLOR  = 0x66DE; // IceCyan
      UI_LABEL_COLOR  = 0xB6BE; // IcePale
      UI_DIM_COLOR    = 0x118A; // IceDeep
      UI_BORDER_COLOR = 0x865C; // IceEdge
      break;
    case 197: // Raio — lightning: storm black / electric yellow / plasma blue
      UI_HEADER_BG    = 0x0864; // StormBlack
      UI_FREQ_COLOR   = 0xFFCC; // LightningYellow
      UI_LEVEL_COLOR  = 0x65BF; // PlasmaBlue
      UI_LABEL_COLOR  = 0xF792; // ThunderPale
      UI_DIM_COLOR    = 0x18C7; // StormDeep
      UI_BORDER_COLOR = 0x931B; // StormPurple
      break;
    case 198: // Aurora — aurora borealis: night sky / aurora green / magenta / teal
      UI_HEADER_BG    = 0x0023; // NightSky
      UI_FREQ_COLOR   = 0x37EF; // AuroraGreen
      UI_LEVEL_COLOR  = 0xC9FF; // AuroraMagenta
      UI_LABEL_COLOR  = 0x36F6; // AuroraTeal
      UI_DIM_COLOR    = 0x08E5; // NightDeep
      UI_BORDER_COLOR = 0x1E52; // AuroraEdge
      break;
    case 199: // Vulcao — volcano: volcanic black / lava orange / molten amber
      UI_HEADER_BG    = 0x1020; // VolcanoBlack
      UI_FREQ_COLOR   = 0xFB22; // LavaOrange
      UI_LEVEL_COLOR  = 0xFD60; // MoltenAmber
      UI_LABEL_COLOR  = 0xE283; // EmberRed
      UI_DIM_COLOR    = 0x2061; // AshDark
      UI_BORDER_COLOR = 0xC9E1; // LavaGlow
      break;
    case 200: // Cosmos — deep space: void black / star white / nebula violet / cosmic cyan
      UI_HEADER_BG    = 0x0021; // VoidBlack
      UI_FREQ_COLOR   = 0xFFFF; // StarWhite
      UI_LEVEL_COLOR  = 0x7A9B; // NebulaViolet
      UI_LABEL_COLOR  = 0x665C; // CosmicCyan
      UI_DIM_COLOR    = 0x0843; // VoidDeep
      UI_BORDER_COLOR = 0xA319; // GalaxyPurple
      break;
    case 201: // Lua — moon: lunar dark / moonlight silver / pale blue
      UI_HEADER_BG    = 0x10A3; // LunarDark
      UI_FREQ_COLOR   = 0xDF1D; // MoonSilver
      UI_LEVEL_COLOR  = 0xA5BA; // MoonBlue
      UI_LABEL_COLOR  = 0xB5D8; // CraterGrey
      UI_DIM_COLOR    = 0x18E5; // ShadowDeep
      UI_BORDER_COLOR = 0xBE1A; // MoonGlow
      break;
    case 202: // Sol — sun: deep solar / gold / sunlight yellow / corona
      UI_HEADER_BG    = 0x1860; // SolarDark
      UI_FREQ_COLOR   = 0xFEA0; // SolarGold
      UI_LEVEL_COLOR  = 0xFF8C; // SunYellow
      UI_LABEL_COLOR  = 0xFD20; // SunOrange
      UI_DIM_COLOR    = 0x5140; // SolarDim
      UI_BORDER_COLOR = 0xE5A6; // CoronaGold
      break;
    case 203: // Oceano — ocean: deep sea / aqua / teal
      UI_HEADER_BG    = 0x00A6; // SeaDark
      UI_FREQ_COLOR   = 0x05BB; // OceanAqua
      UI_LEVEL_COLOR  = 0x0514; // SeaTeal
      UI_LABEL_COLOR  = 0x1E5B; // OceanCyan
      UI_DIM_COLOR    = 0x0043; // Abyss
      UI_BORDER_COLOR = 0x0415; // WaveBlue
      break;
    case 204: // Floresta — forest: deep green / bright leaf / moss
      UI_HEADER_BG    = 0x00C0; // ForestDark
      UI_FREQ_COLOR   = 0x1EE7; // LeafGreen
      UI_LEVEL_COLOR  = 0x3CA6; // MossGreen
      UI_LABEL_COLOR  = 0x664A; // FernGreen
      UI_DIM_COLOR    = 0x08E1; // Undergrowth
      UI_BORDER_COLOR = 0x2BC5; // VineGreen
      break;
    case 205: // Deserto — desert: scorched / sand / dune / terracotta
      UI_HEADER_BG    = 0x28A0; // ScorchedDark
      UI_FREQ_COLOR   = 0xF68F; // SandBright
      UI_LEVEL_COLOR  = 0xCD0A; // DuneGold
      UI_LABEL_COLOR  = 0xCC07; // Terracotta
      UI_DIM_COLOR    = 0x1880; // DesertNight
      UI_BORDER_COLOR = 0xB466; // Ochre
      break;
    case 206: // Neve — snow: grey sky / white / ice blue
      UI_HEADER_BG    = 0x322C; // SnowSky
      UI_FREQ_COLOR   = 0xF7DF; // SnowWhite
      UI_LEVEL_COLOR  = 0xB6BF; // IceBlue
      UI_LABEL_COLOR  = 0xE75E; // PaleSnow
      UI_DIM_COLOR    = 0x5330; // GreySnow
      UI_BORDER_COLOR = 0xCEBE; // Snowflake
      break;
    case 207: // Tempestade — storm: dark grey / silver / rain
      UI_HEADER_BG    = 0x18C4; // StormBlack
      UI_FREQ_COLOR   = 0xF7BF; // StormWhite
      UI_LEVEL_COLOR  = 0x9516; // StormGrey
      UI_LABEL_COLOR  = 0x7C12; // RainGrey
      UI_DIM_COLOR    = 0x0863; // StormDeep
      UI_BORDER_COLOR = 0xADB9; // Thunder
      break;
    case 208: // Crepusculo — twilight: dusk purple / orange / pink
      UI_HEADER_BG    = 0x1865; // DuskDark
      UI_FREQ_COLOR   = 0xFC6A; // TwilightOrange
      UI_LEVEL_COLOR  = 0xDB32; // DuskPink
      UI_LABEL_COLOR  = 0xB3D9; // TwilightPurple
      UI_DIM_COLOR    = 0x1043; // DeepDusk
      UI_BORDER_COLOR = 0xCC0C; // HorizonGlow
      break;
    case 209: // Manha — morning: blue sky / gold / sunrise pink
      UI_HEADER_BG    = 0x3290; // MorningBlue
      UI_FREQ_COLOR   = 0xFEEC; // MorningGold
      UI_LEVEL_COLOR  = 0x65BC; // SkyBlue
      UI_LABEL_COLOR  = 0xFDB2; // SunrisePink
      UI_DIM_COLOR    = 0x198A; // PreDawn
      UI_BORDER_COLOR = 0xB65B; // MorningMist
      break;
    case 210: // Noite — night: midnight / moonlight / starlight
      UI_HEADER_BG    = 0x0022; // Midnight
      UI_FREQ_COLOR   = 0xCEBF; // Moonlight
      UI_LEVEL_COLOR  = 0x3336; // NightBlue
      UI_LABEL_COLOR  = 0x955B; // Starlight
      UI_DIM_COLOR    = 0x0001; // Void
      UI_BORDER_COLOR = 0x5334; // NightSky
      break;
    case 211: // Bruma — mist: grey-blue / white haze / lavender
      UI_HEADER_BG    = 0x2967; // MistDark
      UI_FREQ_COLOR   = 0xCE9C; // HazeWhite
      UI_LEVEL_COLOR  = 0xA4B9; // MistLavender
      UI_LABEL_COLOR  = 0xADB8; // FogGrey
      UI_DIM_COLOR    = 0x10A4; // DeepMist
      UI_BORDER_COLOR = 0x94D5; // MistEdge
      break;
    case 212: // Pantanal — wetlands: dark green / water teal / marsh gold / reed brown
      UI_HEADER_BG    = 0x1101; // WetlandDark
      UI_FREQ_COLOR   = 0x5656; // WaterReflect
      UI_LEVEL_COLOR  = 0x3D08; // LushGreen
      UI_LABEL_COLOR  = 0xB5EA; // MarshGold
      UI_DIM_COLOR    = 0x08C1; // MurkyDeep
      UI_BORDER_COLOR = 0x7C67; // ReedBrown
      break;
    case 213: // Amazonia — rainforest: deep jungle / vivid green / canopy / parrot
      UI_HEADER_BG    = 0x00E1; // JungleDark
      UI_FREQ_COLOR   = 0x06EA; // TropicalGreen
      UI_LEVEL_COLOR  = 0x2DA7; // CanopyGreen
      UI_LABEL_COLOR  = 0xB6E6; // ParrotYellow
      UI_DIM_COLOR    = 0x00A0; // JungleFloor
      UI_BORDER_COLOR = 0x14AA; // RiverGreen
      break;
    case 214: // Cerrado — savanna: burnt dark / golden grass / savanna orange / twisted bark
      UI_HEADER_BG    = 0x20C1; // SavannaDark
      UI_FREQ_COLOR   = 0xE64A; // GoldenGrass
      UI_LEVEL_COLOR  = 0xD466; // SavannaOrange
      UI_LABEL_COLOR  = 0xCDAC; // DryGrass
      UI_DIM_COLOR    = 0x18A1; // ParchedEarth
      UI_BORDER_COLOR = 0xA365; // TwistedBark
      break;
    case 215: // Caatinga — semi-arid: parched brown / sun-bleached bone / cactus green
      UI_HEADER_BG    = 0x28E1; // AridDark
      UI_FREQ_COLOR   = 0xEEF6; // SunBleached
      UI_LEVEL_COLOR  = 0x64A7; // CactusGreen
      UI_LABEL_COLOR  = 0xD56C; // SandyOchre
      UI_DIM_COLOR    = 0x1881; // AridShadow
      UI_BORDER_COLOR = 0xAC08; // DryThorn
      break;
    case 216: // Pampa — southern grasslands: dark green / sky blue / golden grass / horizon
      UI_HEADER_BG    = 0x1142; // FieldDark
      UI_FREQ_COLOR   = 0x65BE; // SkyBlue
      UI_LEVEL_COLOR  = 0xA64A; // PampaGreen
      UI_LABEL_COLOR  = 0xD60C; // GoldenField
      UI_DIM_COLOR    = 0x08C1; // DeepField
      UI_BORDER_COLOR = 0x5516; // HorizonTeal
      break;
    case 217: // Mangue — mangrove: murky dark / brackish teal / olive / root brown
      UI_HEADER_BG    = 0x08E2; // MangroveDark
      UI_FREQ_COLOR   = 0x55AF; // BrackishTeal
      UI_LEVEL_COLOR  = 0x6467; // OliveGreen
      UI_LABEL_COLOR  = 0x8B67; // RootBrown
      UI_DIM_COLOR    = 0x0881; // SwampDeep
      UI_BORDER_COLOR = 0x340A; // TideGreen
      break;
    case 218: // Restinga — coastal scrub: blue-grey / sea foam / pale green / sandy
      UI_HEADER_BG    = 0x29A7; // CoastalDark
      UI_FREQ_COLOR   = 0x96FA; // SeaFoam
      UI_LEVEL_COLOR  = 0x7D6C; // PaleShrub
      UI_LABEL_COLOR  = 0xDEB7; // SandyWhite
      UI_DIM_COLOR    = 0x1945; // DuneShadow
      UI_BORDER_COLOR = 0x5517; // Cerulean
      break;
    case 219: // MataAtl — atlantic forest: deep green / vivid / tropical yellow-green
      UI_HEADER_BG    = 0x0901; // AtlanticDark
      UI_FREQ_COLOR   = 0x168C; // VividAtlantic
      UI_LEVEL_COLOR  = 0x354A; // ForestMid
      UI_LABEL_COLOR  = 0xA68A; // TropicalYG
      UI_DIM_COLOR    = 0x00A1; // Undergrowth
      UI_BORDER_COLOR = 0x2C68; // Mossy
      break;
    case 220: // Recife — coral reef: deep blue / coral / cyan / anemone pink
      UI_HEADER_BG    = 0x00E7; // ReefDeep
      UI_FREQ_COLOR   = 0xFB2A; // CoralOrange
      UI_LEVEL_COLOR  = 0x065A; // ReefCyan
      UI_LABEL_COLOR  = 0xFCB6; // AnemonePink
      UI_DIM_COLOR    = 0x0064; // Abyss
      UI_BORDER_COLOR = 0x0556; // SeaGlass
      break;
    case 221: // Dunas — sand dunes: dark brown / bright sand / sienna / warm beige
      UI_HEADER_BG    = 0x3102; // DuneDark
      UI_FREQ_COLOR   = 0xF714; // BrightSand
      UI_LEVEL_COLOR  = 0xBBC6; // Sienna
      UI_LABEL_COLOR  = 0xE651; // WarmBeige
      UI_DIM_COLOR    = 0x20A1; // DuneBase
      UI_BORDER_COLOR = 0xD5CD; // DuneCrest
      break;
    case 222: // Litoral — coast: deep blue / ocean / wave white / sandy shore
      UI_HEADER_BG    = 0x0906; // CoastalNight
      UI_FREQ_COLOR   = 0x053C; // OceanBright
      UI_LEVEL_COLOR  = 0xB6BE; // WaveWhite
      UI_LABEL_COLOR  = 0xE694; // SandyShore
      UI_DIM_COLOR    = 0x00A5; // SeaNight
      UI_BORDER_COLOR = 0x0499; // TealWave
      break;
    case 223: // Sertao — backlands: red earth / pale hot sky / laterite / bleached bone
      UI_HEADER_BG    = 0x30A1; // RedEarthDark
      UI_FREQ_COLOR   = 0xDE54; // PaleHotSky
      UI_LEVEL_COLOR  = 0xB285; // RedLaterite
      UI_LABEL_COLOR  = 0xE674; // BleachedBone
      UI_DIM_COLOR    = 0x2060; // RedEarthDeep
      UI_BORDER_COLOR = 0xA225; // DryRed
      break;
    case 224: // Planalto — plateau: earth brown / pale sky / stone grey-blue
      UI_HEADER_BG    = 0x2903; // PlateauDark
      UI_FREQ_COLOR   = 0xAE7D; // PlateauSky
      UI_LEVEL_COLOR  = 0x7C75; // StoneGreyBlue
      UI_LABEL_COLOR  = 0xB4CC; // EarthOchre
      UI_DIM_COLOR    = 0x18A2; // PlateauShadow
      UI_BORDER_COLOR = 0x8CB4; // StoneEdge
      break;
    case 225: // Varzea — floodplain: dark green / flood water / silt / muddy gold
      UI_HEADER_BG    = 0x1902; // FloodDark
      UI_FREQ_COLOR   = 0x65B5; // FloodWater
      UI_LEVEL_COLOR  = 0x650A; // SiltGreen
      UI_LABEL_COLOR  = 0xBD4B; // MuddyGold
      UI_DIM_COLOR    = 0x08A1; // Swamp
      UI_BORDER_COLOR = 0x6408; // RiverMud
      break;
    case 226: // Gruta — cave: deep black / mineral glow / stalactite grey
      UI_HEADER_BG    = 0x0841; // CaveBlack
      UI_FREQ_COLOR   = 0xB6F9; // MineralGlow
      UI_LEVEL_COLOR  = 0x8472; // Stalactite
      UI_LABEL_COLOR  = 0xA657; // CaveCrystal
      UI_DIM_COLOR    = 0x0001; // AbyssDark
      UI_BORDER_COLOR = 0x9513; // Limestone
      break;
    case 227: // Cachoeira — waterfall: deep blue / white foam / cascade / mist green
      UI_HEADER_BG    = 0x00C5; // WaterfallDark
      UI_FREQ_COLOR   = 0xDF9F; // WhiteFoam
      UI_LEVEL_COLOR  = 0x355A; // CascadeBlue
      UI_LABEL_COLOR  = 0x96F7; // MistGreen
      UI_DIM_COLOR    = 0x00A4; // DeepPool
      UI_BORDER_COLOR = 0xB73E; // Spray
      break;
    case 228: // Granito — granite: dark speckled / crystal white / granite rose / feldspath
      UI_HEADER_BG    = 0x20E4; // GraniteDark
      UI_FREQ_COLOR   = 0xE73D; // CrystalWhite
      UI_LEVEL_COLOR  = 0xCD14; // GraniteRose
      UI_LABEL_COLOR  = 0xCDF7; // Feldspath
      UI_DIM_COLOR    = 0x1082; // DarkVein
      UI_BORDER_COLOR = 0xB576; // QuartzVein
      break;
    case 229: // Basalto — basalt: volcanic black / ash grey / lava crack glow
      UI_HEADER_BG    = 0x0862; // BasaltBlack
      UI_FREQ_COLOR   = 0xA535; // AshGrey
      UI_LEVEL_COLOR  = 0xCB23; // LavaCrack
      UI_LABEL_COLOR  = 0x8431; // GreyStone
      UI_DIM_COLOR    = 0x0020; // VolcanicBlack
      UI_BORDER_COLOR = 0x5AEC; // DarkStone
      break;
    case 230: // Obsidiana — obsidian: glossy black / purple sheen / grey gloss
      UI_HEADER_BG    = 0x0821; // ObsidianBlack
      UI_FREQ_COLOR   = 0xB51B; // ObsidianSheen
      UI_LEVEL_COLOR  = 0x9339; // PurpleSheen
      UI_LABEL_COLOR  = 0xA4D5; // GreyGloss
      UI_DIM_COLOR    = 0x0020; // Void
      UI_BORDER_COLOR = 0x7B32; // ObsidianEdge
      break;
    case 231: // Marmore — marble: dark grey / pure white / grey vein / pink vein
      UI_HEADER_BG    = 0x3186; // MarmoreDark
      UI_FREQ_COLOR   = 0xF79E; // MarbleWhite
      UI_LEVEL_COLOR  = 0xA514; // GreyVein
      UI_LABEL_COLOR  = 0xDDF9; // PinkVein
      UI_DIM_COLOR    = 0x18E4; // MarbleShadow
      UI_BORDER_COLOR = 0xB597; // GreyMarble
      break;
    case 232: // Cristal — crystal: dark / clear white / prism light / ice crystal
      UI_HEADER_BG    = 0x10C4; // CrystalDark
      UI_FREQ_COLOR   = 0xE79F; // CrystalClear
      UI_LEVEL_COLOR  = 0xB69F; // PrismLight
      UI_LABEL_COLOR  = 0xC6FF; // IceCrystal
      UI_DIM_COLOR    = 0x08A3; // DeepCrystal
      UI_BORDER_COLOR = 0xA61D; // CrystalEdge
      break;
    case 233: // Quartzo — quartz: dark / milky white / rose quartz / quartz pink
      UI_HEADER_BG    = 0x2904; // QuartzDark
      UI_FREQ_COLOR   = 0xF75E; // MilkyWhite
      UI_LEVEL_COLOR  = 0xDDB8; // RoseQuartz
      UI_LABEL_COLOR  = 0xEE5A; // QuartzPink
      UI_DIM_COLOR    = 0x18A3; // QuartzShadow
      UI_BORDER_COLOR = 0xCDD8; // CrystalVein
      break;
    case 234: // Safira — sapphire: deep blue / sapphire / royal blue / light blue
      UI_HEADER_BG    = 0x0046; // DeepSapphire
      UI_FREQ_COLOR   = 0x129B; // SapphireBlue
      UI_LEVEL_COLOR  = 0x33DB; // RoyalBlue
      UI_LABEL_COLOR  = 0x655E; // SapphireLight
      UI_DIM_COLOR    = 0x0023; // DeepOcean
      UI_BORDER_COLOR = 0x3C1A; // SapphireGlow
      break;
    case 235: // Ametista — amethyst: deep purple / bright violet / lavender
      UI_HEADER_BG    = 0x1044; // DeepAmethyst
      UI_FREQ_COLOR   = 0xB29B; // BrightViolet
      UI_LEVEL_COLOR  = 0x8AD9; // AmethystPurple
      UI_LABEL_COLOR  = 0xCD5C; // Lavender
      UI_DIM_COLOR    = 0x0822; // VioletDeep
      UI_BORDER_COLOR = 0xA3D9; // AmethystGlow
      break;
    case 236: // Esmeralda — emerald: dark green / bright emerald / emerald light
      UI_HEADER_BG    = 0x0102; // EmeraldDark
      UI_FREQ_COLOR   = 0x064B; // BrightEmerald
      UI_LEVEL_COLOR  = 0x0508; // EmeraldGreen
      UI_LABEL_COLOR  = 0x6691; // EmeraldLight
      UI_DIM_COLOR    = 0x0081; // DeepGem
      UI_BORDER_COLOR = 0x0467; // EmeraldGlow
      break;
    case 237: // Rubi — ruby: dark red / ruby red / crimson / rose ruby
      UI_HEADER_BG    = 0x2821; // RubyDark
      UI_FREQ_COLOR   = 0xD8A7; // RubyRed
      UI_LEVEL_COLOR  = 0xB0A6; // Crimson
      UI_LABEL_COLOR  = 0xF3D1; // RoseRuby
      UI_DIM_COLOR    = 0x1800; // DeepRed
      UI_BORDER_COLOR = 0xB988; // RubyGlow
      break;
    case 238: // Turquesa — turquoise: deep teal / bright turquoise / sandy
      UI_HEADER_BG    = 0x0145; // DeepTurquoise
      UI_FREQ_COLOR   = 0x2E57; // BrightTurquoise
      UI_LEVEL_COLOR  = 0x0554; // TurquoiseMid
      UI_LABEL_COLOR  = 0xA699; // SandyTurquoise
      UI_DIM_COLOR    = 0x00A2; // SeaDeep
      UI_BORDER_COLOR = 0x0513; // TurquoiseGlow
      break;
    case 239: // Coral — coral: deep rose / bright coral / peach
      UI_HEADER_BG    = 0x38A2; // CoralDark
      UI_FREQ_COLOR   = 0xFBCC; // BrightCoral
      UI_LEVEL_COLOR  = 0xF32F; // CoralPink
      UI_LABEL_COLOR  = 0xFDB4; // Peach
      UI_DIM_COLOR    = 0x2861; // RoseDark
      UI_BORDER_COLOR = 0xDACC; // CoralGlow
      break;
    case 240: // Perola — pearl: shadow / pure white / cream / soft pink
      UI_HEADER_BG    = 0x4209; // PearlShadow
      UI_FREQ_COLOR   = 0xFFBF; // PearlWhite
      UI_LEVEL_COLOR  = 0xF73B; // PearlCream
      UI_LABEL_COLOR  = 0xF69B; // SoftPink
      UI_DIM_COLOR    = 0x3166; // PearlDim
      UI_BORDER_COLOR = 0xD67B; // Iridescent
      break;
    case 241: // Onix — onyx: deep black / pure white / grey contrast
      UI_HEADER_BG    = 0x0861; // OnyxBlack
      UI_FREQ_COLOR   = 0xF7BE; // PureWhite
      UI_LEVEL_COLOR  = 0x8C51; // GreyMid
      UI_LABEL_COLOR  = 0xCE59; // LightGrey
      UI_DIM_COLOR    = 0x0020; // DeepestBlack
      UI_BORDER_COLOR = 0x632C; // GreyOnyx
      break;
    case 242: // Jaspe — jasper: earthy red / green jasper / tan ochre
      UI_HEADER_BG    = 0x2881; // JasperDark
      UI_FREQ_COLOR   = 0xBA87; // JasperRed
      UI_LEVEL_COLOR  = 0x5408; // GreenJasper
      UI_LABEL_COLOR  = 0xCCCC; // TanOchre
      UI_DIM_COLOR    = 0x1040; // DarkEarth
      UI_BORDER_COLOR = 0xA286; // RustyEdge
      break;
    case 243: // Citrino — citrine: dark amber / golden yellow / amber gold
      UI_HEADER_BG    = 0x28E0; // CitrineDark
      UI_FREQ_COLOR   = 0xFEA6; // CitrineYellow
      UI_LEVEL_COLOR  = 0xF545; // AmberGold
      UI_LABEL_COLOR  = 0xFE4C; // GoldenLight
      UI_DIM_COLOR    = 0x18A0; // DarkAmber
      UI_BORDER_COLOR = 0xDD67; // GoldenGlow
      break;
    case 244: // Labradorita — labradorite: dark grey / flash blue / flash green / gold
      UI_HEADER_BG    = 0x18C3; // LabradorDark
      UI_FREQ_COLOR   = 0x44BB; // FlashBlue
      UI_LEVEL_COLOR  = 0x3DAF; // FlashGreen
      UI_LABEL_COLOR  = 0xCDAA; // GoldFlash
      UI_DIM_COLOR    = 0x1083; // DarkStone
      UI_BORDER_COLOR = 0x6516; // IridescentEdge
      break;
    case 245: // Selenita — selenite: translucent / glowing white / crystal cream
      UI_HEADER_BG    = 0x31A7; // SeleniteShad
      UI_FREQ_COLOR   = 0xFFDE; // GlowingWhite
      UI_LEVEL_COLOR  = 0xE71A; // CrystalGlow
      UI_LABEL_COLOR  = 0xF75C; // SoftCream
      UI_DIM_COLOR    = 0x2945; // StoneGrey
      UI_BORDER_COLOR = 0xD6B9; // SeleniteGlow
      break;
    case 246: // Pirita — pyrite: dark iron / gold brass / brassy yellow / fool's gold
      UI_HEADER_BG    = 0x18C1; // PyriteDark
      UI_FREQ_COLOR   = 0xDDE5; // GoldBrass
      UI_LEVEL_COLOR  = 0xCD23; // BrassyYellow
      UI_LABEL_COLOR  = 0xE64C; // FoolsGold
      UI_DIM_COLOR    = 0x1080; // DarkIron
      UI_BORDER_COLOR = 0xB4A3; // MetallicRim
      break;
    case 247: // Malaquita — malachite: dark green / bright banded / green vein
      UI_HEADER_BG    = 0x00C1; // MalachiteDark
      UI_FREQ_COLOR   = 0x064C; // BandedBright
      UI_LEVEL_COLOR  = 0x150A; // MalachiteGreen
      UI_LABEL_COLOR  = 0x6650; // PaleBanded
      UI_DIM_COLOR    = 0x0061; // DarkMineral
      UI_BORDER_COLOR = 0x0407; // GreenVein
      break;
    case 248: // Lapis — lapis lazuli: deep blue / lapis blue / gold specks
      UI_HEADER_BG    = 0x08A8; // LapisDeep
      UI_FREQ_COLOR   = 0x1AD9; // LapisBlue
      UI_LEVEL_COLOR  = 0x3337; // BlueLapis
      UI_LABEL_COLOR  = 0xE60A; // GoldSpecks
      UI_DIM_COLOR    = 0x0045; // DeepBlue
      UI_BORDER_COLOR = 0x2A95; // LapisEdge
      break;
    case 249: // Amazonita — amazonite: dark teal / bright blue-green / pale
      UI_HEADER_BG    = 0x0945; // AmazoniteDark
      UI_FREQ_COLOR   = 0x5617; // BrightAmazonita
      UI_LEVEL_COLOR  = 0x3513; // TealMid
      UI_LABEL_COLOR  = 0xA6B9; // PaleAmazonita
      UI_DIM_COLOR    = 0x00C3; // DeepTeal
      UI_BORDER_COLOR = 0x3CD2; // GemEdge
      break;
    case 250: // Fluorita — fluorite: dark / purple / green / blue multi-band
      UI_HEADER_BG    = 0x1064; // FluoriteDark
      UI_FREQ_COLOR   = 0xCB3B; // FluoritePurple
      UI_LEVEL_COLOR  = 0x66F4; // FluoriteGreen
      UI_LABEL_COLOR  = 0x8DBC; // FluoriteBlue
      UI_DIM_COLOR    = 0x0842; // MineralDark
      UI_BORDER_COLOR = 0xA419; // MultiEdge
      break;
    case 251: // Calcita — calcite: shadow / crystal yellow-white / pale gold / cream
      UI_HEADER_BG    = 0x2943; // CalciteShadow
      UI_FREQ_COLOR   = 0xFFBA; // CrystalYellow
      UI_LEVEL_COLOR  = 0xE6B2; // PaleGold
      UI_LABEL_COLOR  = 0xEEF5; // CreamyYellow
      UI_DIM_COLOR    = 0x18C2; // CalciteDark
      UI_BORDER_COLOR = 0xD654; // CrystalEdge2
      break;
    case 252: // Aventurina — aventurine: green / gold sparkle / forest gold
      UI_HEADER_BG    = 0x1122; // AventurinaDark
      UI_FREQ_COLOR   = 0x9648; // SparkleGold
      UI_LEVEL_COLOR  = 0x552A; // AventurinaGreen
      UI_LABEL_COLOR  = 0xBE4D; // GoldSparkle
      UI_DIM_COLOR    = 0x10C1; // MineralDeep
      UI_BORDER_COLOR = 0x7DA8; // ForestGold
      break;
    case 253: // Rodolita — rhodolite: deep rose / raspberry / pink
      UI_HEADER_BG    = 0x2042; // RhodoliteDark
      UI_FREQ_COLOR   = 0xD18D; // Raspberry
      UI_LEVEL_COLOR  = 0xC9EC; // RoseRed
      UI_LABEL_COLOR  = 0xF475; // RhodolitePink
      UI_DIM_COLOR    = 0x1021; // DeepRose
      UI_BORDER_COLOR = 0xB18B; // RaspberryGlow
      break;
    case 254: // Tanzanita — tanzanite: deep indigo / blue-violet / violet light
      UI_HEADER_BG    = 0x0844; // TanzaniteDark
      UI_FREQ_COLOR   = 0x629B; // BlueViolet
      UI_LEVEL_COLOR  = 0x4197; // Indigo
      UI_LABEL_COLOR  = 0xB51D; // VioletLight
      UI_DIM_COLOR    = 0x0823; // DeepIndigo
      UI_BORDER_COLOR = 0x837A; // TanzaniteGlow
      break;
    case 255: // Diamante — diamond: mount / pure brilliance / prismatic / rainbow
      UI_HEADER_BG    = 0x2946; // DiamondMount
      UI_FREQ_COLOR   = 0xFFFF; // PureBrilliance
      UI_LEVEL_COLOR  = 0xCF9F; // PrismaticCyan
      UI_LABEL_COLOR  = 0xFEFE; // RainbowPink
      UI_DIM_COLOR    = 0x2125; // DiamondDark
      UI_BORDER_COLOR = 0xFEB2; // DiamondSparkle
      break;
    case 256: // Prisma — spectrum: dark / red-orange / green / blue-violet / gold
      UI_HEADER_BG    = 0x0823; // PrismDark
      UI_FREQ_COLOR   = 0xFB26; // SpectrumRed
      UI_LEVEL_COLOR  = 0x36EC; // SpectrumGreen
      UI_LABEL_COLOR  = 0x633F; // SpectrumBlue
      UI_DIM_COLOR    = 0x0801; // PrismDeep
      UI_BORDER_COLOR = 0xDE46; // SpectrumGold
      break;
    case 257: // Nebula — space nebula: deep purple-black / magenta / blue-teal / violet
      UI_HEADER_BG    = 0x0824; // NebulaDark
      UI_FREQ_COLOR   = 0xFA99; // NebulaMagenta
      UI_LEVEL_COLOR  = 0x35BB; // NebulaTeal
      UI_LABEL_COLOR  = 0xB3DE; // NebulaViolet
      UI_DIM_COLOR    = 0x0020; // NebulaVoid
      UI_BORDER_COLOR = 0x1E56; // NebulaEdge
      break;
    case 258: // Galaxia — galaxy: galactic dark / spiral gold / star white / purple gas
      UI_HEADER_BG    = 0x1045; // GalacticCore
      UI_FREQ_COLOR   = 0xFE4A; // SpiralGold
      UI_LEVEL_COLOR  = 0xCEFF; // StarWhiteBlue
      UI_LABEL_COLOR  = 0xB33B; // PurpleGas
      UI_DIM_COLOR    = 0x0840; // GalacticVoid
      UI_BORDER_COLOR = 0x64B9; // GalaxyRim
      break;
    case 259: // RockNac — rock nacional: near-black / white / red / silver / crimson
      UI_HEADER_BG    = 0x0841; // RockBlack
      UI_FREQ_COLOR   = 0xFFFF; // PureWhite
      UI_LEVEL_COLOR  = 0xF800; // DistortionRed
      UI_LABEL_COLOR  = 0xBDF7; // CoolSilver
      UI_DIM_COLOR    = 0x20C3; // SlateVoid
      UI_BORDER_COLOR = 0xC000; // CrimsonEdge
      break;
    case 260: // Axe — axe music / carnaval: deep purple / carnival yellow / orange / magenta
      UI_HEADER_BG    = 0x3008; // CarnivalPurple
      UI_FREQ_COLOR   = 0xFFE0; // CarnivalYellow
      UI_LEVEL_COLOR  = 0xFC60; // AxeOrange
      UI_LABEL_COLOR  = 0xF81F; // AxeMagenta
      UI_DIM_COLOR    = 0x6006; // DarkPurple
      UI_BORDER_COLOR = 0xFB20; // AmberGlow
      break;
    case 261: // Sertanejo — dark earth / amber gold / orange / tan / caramel
      UI_HEADER_BG    = 0x3120; // DarkEarth
      UI_FREQ_COLOR   = 0xFDA0; // AmberGold
      UI_LEVEL_COLOR  = 0xFC80; // SertanejoOrange
      UI_LABEL_COLOR  = 0xF6D3; // Tan
      UI_DIM_COLOR    = 0x4A00; // DeepEarth
      UI_BORDER_COLOR = 0xC560; // Caramel
      break;
    case 262: // Pagode — deep maroon / ivory / warm gold / rose
      UI_HEADER_BG    = 0x4000; // DeepMaroon
      UI_FREQ_COLOR   = 0xFFF5; // Ivory
      UI_LEVEL_COLOR  = 0xFD40; // WarmGold
      UI_LABEL_COLOR  = 0xFCAA; // WarmRose
      UI_DIM_COLOR    = 0x2000; // DarkMaroon
      UI_BORDER_COLOR = 0xE540; // GoldEdge
      break;
    case 263: // MPB — dark warm grey / champagne / olive green / warm gold
      UI_HEADER_BG    = 0x2945; // WarmSlate
      UI_FREQ_COLOR   = 0xFFD6; // Champagne
      UI_LEVEL_COLOR  = 0x6C80; // OliveGreen
      UI_LABEL_COLOR  = 0xF7A0; // WarmGold
      UI_DIM_COLOR    = 0x18C2; // DarkWarm
      UI_BORDER_COLOR = 0x5C80; // OliveEdge
      break;
    case 264: // Funk — jet black / neon green / hot pink / acid yellow
      UI_HEADER_BG    = 0x0000; // JetBlack
      UI_FREQ_COLOR   = 0x07E0; // NeonGreen
      UI_LEVEL_COLOR  = 0xF81F; // HotPink
      UI_LABEL_COLOR  = 0xFFE0; // AcidYellow
      UI_DIM_COLOR    = 0x0280; // DarkGreen
      UI_BORDER_COLOR = 0xFC00; // NeonOrange
      break;
    case 265: // Brega — deep purple-pink / hot pink / kitsch gold / lavender
      UI_HEADER_BG    = 0x3806; // DeepPurplePink
      UI_FREQ_COLOR   = 0xF81A; // HotPinkBrega
      UI_LEVEL_COLOR  = 0xFFC0; // KitschGold
      UI_LABEL_COLOR  = 0xCDBF; // Lavender
      UI_DIM_COLOR    = 0x500D; // DarkPurp
      UI_BORDER_COLOR = 0xF980; // GoldOrange
      break;
    case 266: // Maracatu — deep indigo / gold / red / amber
      UI_HEADER_BG    = 0x0008; // DeepIndigo
      UI_FREQ_COLOR   = 0xFDA0; // MaracatuGold
      UI_LEVEL_COLOR  = 0xF800; // MaracatuRed
      UI_LABEL_COLOR  = 0xFD40; // Amber
      UI_DIM_COLOR    = 0x0005; // NightBlue
      UI_BORDER_COLOR = 0xD8C0; // GoldSheen
      break;
    case 267: // Baiao — dark terracotta / tangerine / burnt orange / peach
      UI_HEADER_BG    = 0x3920; // DarkTerra
      UI_FREQ_COLOR   = 0xFCA0; // Tangerine
      UI_LEVEL_COLOR  = 0xF600; // BurntOrange
      UI_LABEL_COLOR  = 0xFFCA; // Peach
      UI_DIM_COLOR    = 0x4A00; // DeepTerra
      UI_BORDER_COLOR = 0xD300; // BurntEdge
      break;
    case 268: // Choro — dark sepia / ivory / warm amber / cream
      UI_HEADER_BG    = 0x2940; // DarkSepia
      UI_FREQ_COLOR   = 0xFFD6; // SepiaIvory
      UI_LEVEL_COLOR  = 0xCD40; // WarmAmber
      UI_LABEL_COLOR  = 0xF6B0; // Cream
      UI_DIM_COLOR    = 0x18C0; // SepiaDeep
      UI_BORDER_COLOR = 0xBD00; // SepiaEdge
      break;
    case 269: // Xote — deep forest green / lime / bright green / light lime
      UI_HEADER_BG    = 0x0400; // ForestDeep
      UI_FREQ_COLOR   = 0x67E0; // BrightLime
      UI_LEVEL_COLOR  = 0x07E0; // PureGreen
      UI_LABEL_COLOR  = 0xCFE0; // LightLime
      UI_DIM_COLOR    = 0x0200; // DarkForest
      UI_BORDER_COLOR = 0x4FE0; // MediumLime
      break;
    case 270: // Piseiro — deep blue / white / electric blue / azure
      UI_HEADER_BG    = 0x0008; // PiseiroBlue
      UI_FREQ_COLOR   = 0xFFFF; // White
      UI_LEVEL_COLOR  = 0x001F; // ElectricBlue
      UI_LABEL_COLOR  = 0x6FFF; // LightBlue
      UI_DIM_COLOR    = 0x0004; // NightBlue
      UI_BORDER_COLOR = 0x05FF; // Azure
      break;
    case 271: // Arrocha — dark rose / pink / crimson / salmon
      UI_HEADER_BG    = 0x3003; // DarkRose
      UI_FREQ_COLOR   = 0xFB8F; // RosePink
      UI_LEVEL_COLOR  = 0xF004; // Crimson
      UI_LABEL_COLOR  = 0xFCAA; // Salmon
      UI_DIM_COLOR    = 0x5001; // DeepRose
      UI_BORDER_COLOR = 0xD805; // DeepPink
      break;
    case 272: // Tecnobrega — jet black / neon cyan / electric purple / acid green
      UI_HEADER_BG    = 0x0000; // TecnoBlack
      UI_FREQ_COLOR   = 0x07FF; // NeonCyan
      UI_LEVEL_COLOR  = 0xC01F; // ElectricPurple
      UI_LABEL_COLOR  = 0x67E0; // AcidGreen
      UI_DIM_COLOR    = 0x0183; // DarkTeal
      UI_BORDER_COLOR = 0xF81F; // Magenta
      break;
    case 273: // Lambada — dark teal / turquoise / coral orange / sky blue
      UI_HEADER_BG    = 0x0043; // TropicalDeep
      UI_FREQ_COLOR   = 0x07FE; // Turquoise
      UI_LEVEL_COLOR  = 0xFB40; // CoralOrange
      UI_LABEL_COLOR  = 0x7FFF; // SkyBlue
      UI_DIM_COLOR    = 0x0022; // TealVoid
      UI_BORDER_COLOR = 0xFC80; // Coral
      break;
    case 274: // Embolada — dark terracotta / burnt orange / amber / raw earth
      UI_HEADER_BG    = 0x2880; // DarkTerra2
      UI_FREQ_COLOR   = 0xFB40; // TerraOrange
      UI_LEVEL_COLOR  = 0xF280; // BurntAmber
      UI_LABEL_COLOR  = 0xFE00; // RawAmber
      UI_DIM_COLOR    = 0x4800; // DarkEarth2
      UI_BORDER_COLOR = 0xD000; // BurntEdge2
      break;
    case 275: // Repente — dark leather / gold amber / warm cream / dark amber
      UI_HEADER_BG    = 0x2820; // LeatherDark
      UI_FREQ_COLOR   = 0xFDA0; // GoldAmber
      UI_LEVEL_COLOR  = 0xC500; // DarkGold
      UI_LABEL_COLOR  = 0xFFBA; // WarmCream
      UI_DIM_COLOR    = 0x2000; // DeepBrown
      UI_BORDER_COLOR = 0xAC00; // DarkAmber
      break;
    case 276: // Carimbo — jungle dark / tribal yellow / earth red / jungle light
      UI_HEADER_BG    = 0x0200; // JungleDark
      UI_FREQ_COLOR   = 0xFFE0; // TribalYellow
      UI_LEVEL_COLOR  = 0xF800; // EarthRed
      UI_LABEL_COLOR  = 0x8FE0; // JungleLight
      UI_DIM_COLOR    = 0x0100; // JungleVoid
      UI_BORDER_COLOR = 0xD800; // Ochre
      break;
    case 277: // Baile — dark purple / neon lime / hot pink / yellow
      UI_HEADER_BG    = 0x2006; // BailePurple
      UI_FREQ_COLOR   = 0x67E0; // NeonLime
      UI_LEVEL_COLOR  = 0xF81F; // BaileHotPink
      UI_LABEL_COLOR  = 0xFFE0; // BaileYellow
      UI_DIM_COLOR    = 0x4006; // MedPurple
      UI_BORDER_COLOR = 0x07E0; // NeonEdge
      break;
    case 278: // Samba — deep green-black / school gold / red / verde
      UI_HEADER_BG    = 0x0100; // SambaDeep
      UI_FREQ_COLOR   = 0xFD00; // SchoolGold
      UI_LEVEL_COLOR  = 0xF800; // SambaRed
      UI_LABEL_COLOR  = 0x07C0; // VerdeSchool
      UI_DIM_COLOR    = 0x0080; // DarkVerde
      UI_BORDER_COLOR = 0xED40; // GoldShine
      break;
    case 279: // Frevo — deep blue / electric yellow / vibrant red / white
      UI_HEADER_BG    = 0x0010; // FrevoBlue
      UI_FREQ_COLOR   = 0xFFE0; // ElectricYellow
      UI_LEVEL_COLOR  = 0xF800; // VibrantRed
      UI_LABEL_COLOR  = 0xFFFF; // White
      UI_DIM_COLOR    = 0x0006; // NightBlue2
      UI_BORDER_COLOR = 0xFD20; // OrangeRed
      break;
    case 280: // Coco — sandy dark / coconut cream / warm amber / light cream
      UI_HEADER_BG    = 0x3981; // SandyDark
      UI_FREQ_COLOR   = 0xFFD6; // CoconutCream
      UI_LEVEL_COLOR  = 0xCD40; // WarmAmber2
      UI_LABEL_COLOR  = 0xFEB2; // LightCream
      UI_DIM_COLOR    = 0x2940; // DarkSand
      UI_BORDER_COLOR = 0xAD20; // WarmTan
      break;
    // ── Brazilian cities 281-290 ─────────────────────────────
    case 281: // Manaus — deep jungle / tropical gold / bright green / cream
      UI_HEADER_BG    = 0x0302; // JungleDeep
      UI_FREQ_COLOR   = 0xFDA0; // TropicalGold
      UI_LEVEL_COLOR  = 0x07C0; // BrightGreen
      UI_LABEL_COLOR  = 0xFFF5; // Cream
      UI_DIM_COLOR    = 0x0181; // DarkJungle
      UI_BORDER_COLOR = 0x4FE0; // Lime
      break;
    case 282: // Belem — amazon dark / yellow / amazon green / gold
      UI_HEADER_BG    = 0x0180; // AmazonDark
      UI_FREQ_COLOR   = 0xFFE0; // BrightYellow
      UI_LEVEL_COLOR  = 0x07E0; // AmazonGreen
      UI_LABEL_COLOR  = 0xFDA0; // Gold
      UI_DIM_COLOR    = 0x00C0; // DeepAmazon
      UI_BORDER_COLOR = 0xED40; // BrightGold
      break;
    case 283: // Curitiba — dark pine grey / silver / pine green / cool grey
      UI_HEADER_BG    = 0x1043; // PineGrey
      UI_FREQ_COLOR   = 0xEF7D; // Silver
      UI_LEVEL_COLOR  = 0x2D40; // PineGreen
      UI_LABEL_COLOR  = 0xBDF7; // CoolGrey
      UI_DIM_COLOR    = 0x0842; // DarkGrey
      UI_BORDER_COLOR = 0x4DC8; // PineTeal
      break;
    case 284: // PortoAlegre — cool dark grey / silver / gaucho red / light grey
      UI_HEADER_BG    = 0x2104; // CoolDarkGrey
      UI_FREQ_COLOR   = 0xEF7D; // Silver
      UI_LEVEL_COLOR  = 0xF800; // GauchoRed
      UI_LABEL_COLOR  = 0xC618; // LightGrey
      UI_DIM_COLOR    = 0x4208; // MidGrey
      UI_BORDER_COLOR = 0xB800; // DarkRed
      break;
    case 285: // Brasilia — deep blue / white / cyan / sky
      UI_HEADER_BG    = 0x0010; // BrasiliaBlue
      UI_FREQ_COLOR   = 0xFFFF; // White
      UI_LEVEL_COLOR  = 0x07FF; // Cyan
      UI_LABEL_COLOR  = 0xADD5; // Sky
      UI_DIM_COLOR    = 0x0008; // NightBlue
      UI_BORDER_COLOR = 0x05BF; // Azure
      break;
    case 286: // Floripa — deep turquoise / white / turquoise / light turquoise
      UI_HEADER_BG    = 0x0043; // FloripaTeal
      UI_FREQ_COLOR   = 0xFFFF; // White
      UI_LEVEL_COLOR  = 0x07FE; // Turquoise
      UI_LABEL_COLOR  = 0xB7FF; // LightTurq
      UI_DIM_COLOR    = 0x0021; // OceanDeep
      UI_BORDER_COLOR = 0x5DFF; // SkyEdge
      break;
    case 287: // Natal — deep blue / tropical orange / orange / cream
      UI_HEADER_BG    = 0x0008; // NatalBlue
      UI_FREQ_COLOR   = 0xFCA0; // TropicalOrange
      UI_LEVEL_COLOR  = 0xFC60; // Orange
      UI_LABEL_COLOR  = 0xFFF5; // Cream
      UI_DIM_COLOR    = 0x0004; // NightDeep
      UI_BORDER_COLOR = 0xFB20; // Amber
      break;
    case 288: // Maceio — deep teal / cyan / light cyan / crystal blue
      UI_HEADER_BG    = 0x0042; // MaceioDeep
      UI_FREQ_COLOR   = 0x07FF; // Cyan
      UI_LEVEL_COLOR  = 0x3FFF; // LightCyan
      UI_LABEL_COLOR  = 0xBFFF; // Crystal
      UI_DIM_COLOR    = 0x0021; // TealVoid
      UI_BORDER_COLOR = 0x07BF; // CrystalBlue
      break;
    case 289: // Aracaju — warm earth / sunset orange / amber / warm cream
      UI_HEADER_BG    = 0x3940; // WarmEarth
      UI_FREQ_COLOR   = 0xFCA0; // SunsetOrange
      UI_LEVEL_COLOR  = 0xFD20; // AmberOrange
      UI_LABEL_COLOR  = 0xFFD6; // WarmCream
      UI_DIM_COLOR    = 0x4A40; // DarkEarth
      UI_BORDER_COLOR = 0xF380; // SunsetEdge
      break;
    case 290: // Teresina — dark burnt orange / intense orange / hot yellow / amber
      UI_HEADER_BG    = 0x4800; // BurntDark
      UI_FREQ_COLOR   = 0xFC00; // IntenseOrange
      UI_LEVEL_COLOR  = 0xFF20; // HotYellow
      UI_LABEL_COLOR  = 0xFDA0; // Amber
      UI_DIM_COLOR    = 0x3000; // DarkBrown
      UI_BORDER_COLOR = 0xFB00; // OrangeRed
      break;
    // ── Art styles 291-300 ───────────────────────────────────
    case 291: // Impressao — muted blue-grey / periwinkle / soft blue / pearl
      UI_HEADER_BG    = 0x2945; // MutedSlate
      UI_FREQ_COLOR   = 0xBEDF; // Periwinkle
      UI_LEVEL_COLOR  = 0x867D; // SoftBlue
      UI_LABEL_COLOR  = 0xF7BE; // Pearl
      UI_DIM_COLOR    = 0x18C5; // DeepSlate
      UI_BORDER_COLOR = 0x9D5F; // LavenderEdge
      break;
    case 292: // Cubismo — dark grey / yellow / blue / silver
      UI_HEADER_BG    = 0x2104; // CuboGrey
      UI_FREQ_COLOR   = 0xFFE0; // CuboYellow
      UI_LEVEL_COLOR  = 0x001F; // CuboBlue
      UI_LABEL_COLOR  = 0xEF7D; // CuboSilver
      UI_DIM_COLOR    = 0x4208; // CuboMid
      UI_BORDER_COLOR = 0xF800; // CuboRed
      break;
    case 293: // Surrealismo — dark dream purple / violet light / hot pink / lavender
      UI_HEADER_BG    = 0x2006; // DreamPurple
      UI_FREQ_COLOR   = 0xE0DF; // VioletLight
      UI_LEVEL_COLOR  = 0xF81A; // DreamPink
      UI_LABEL_COLOR  = 0xCCBF; // Lavender2
      UI_DIM_COLOR    = 0x4006; // PurpleMid
      UI_BORDER_COLOR = 0x837A; // VioletEdge
      break;
    case 294: // Expressao — deep red-black / raw red / yellow / harsh silver
      UI_HEADER_BG    = 0x3800; // ExpressoDark
      UI_FREQ_COLOR   = 0xF800; // RawRed
      UI_LEVEL_COLOR  = 0xFFE0; // ExpressoYellow
      UI_LABEL_COLOR  = 0xEF7D; // HarshSilver
      UI_DIM_COLOR    = 0x2800; // DarkRed2
      UI_BORDER_COLOR = 0xFC00; // OrangeEdge
      break;
    case 295: // PopArt — black / yellow / red / blue
      UI_HEADER_BG    = 0x0000; // PopBlack
      UI_FREQ_COLOR   = 0xFFE0; // PopYellow
      UI_LEVEL_COLOR  = 0xF800; // PopRed
      UI_LABEL_COLOR  = 0x001F; // PopBlue
      UI_DIM_COLOR    = 0x2104; // PopDark
      UI_BORDER_COLOR = 0x07E0; // PopGreen
      break;
    case 296: // Pointilismo — warm mid grey / soft peach / soft sky / ivory
      UI_HEADER_BG    = 0x39E7; // PointWarmGrey
      UI_FREQ_COLOR   = 0xFBB5; // SoftPeach
      UI_LEVEL_COLOR  = 0xADD5; // SoftSky
      UI_LABEL_COLOR  = 0xFFD6; // Ivory2
      UI_DIM_COLOR    = 0x2104; // PointDark
      UI_BORDER_COLOR = 0xF7BE; // PearlEdge
      break;
    case 297: // Futurismo — dark silver / white / electric cyan / silver
      UI_HEADER_BG    = 0x0841; // FutureDark
      UI_FREQ_COLOR   = 0xFFFF; // FutureWhite
      UI_LEVEL_COLOR  = 0x07FF; // ElectricCyan
      UI_LABEL_COLOR  = 0xEF7D; // FutureSilver
      UI_DIM_COLOR    = 0x20C3; // SlateVoid2
      UI_BORDER_COLOR = 0x6BFF; // IcyBlue
      break;
    case 298: // Romantico — deep rose / ivory / warm gold / rose cream
      UI_HEADER_BG    = 0x3003; // RomDarkRose
      UI_FREQ_COLOR   = 0xFFD6; // RomIvory
      UI_LEVEL_COLOR  = 0xFDA0; // RomGold
      UI_LABEL_COLOR  = 0xFBB5; // RoseCream
      UI_DIM_COLOR    = 0x5001; // RomDeepRose
      UI_BORDER_COLOR = 0xFB8F; // PinkGold
      break;
    case 299: // Barroco — deep brown-black / baroque gold / dark gold / warm cream
      UI_HEADER_BG    = 0x2000; // BarrocoDark
      UI_FREQ_COLOR   = 0xFDA0; // BarrocoGold
      UI_LEVEL_COLOR  = 0xC500; // DarkGold2
      UI_LABEL_COLOR  = 0xFEB2; // WarmCream2
      UI_DIM_COLOR    = 0x1000; // VeryDarkBrown
      UI_BORDER_COLOR = 0x8400; // BrownGold
      break;
    case 300: // Concreto — black / white / primary red / grey
      UI_HEADER_BG    = 0x0000; // ConcBlack
      UI_FREQ_COLOR   = 0xFFFF; // ConcWhite
      UI_LEVEL_COLOR  = 0xF800; // ConcRed
      UI_LABEL_COLOR  = 0xC618; // ConcGrey
      UI_DIM_COLOR    = 0x2104; // ConcDark
      UI_BORDER_COLOR = 0xFFFF; // ConcEdge
      break;
    // ── Technology / digital 301-310 ─────────────────────────
    case 301: // Matrix2 — void black / bright matrix green / medium / light matrix
      UI_HEADER_BG    = 0x0000; // MatrixVoid
      UI_FREQ_COLOR   = 0x07E0; // BrightMatrix
      UI_LEVEL_COLOR  = 0x0400; // MedMatrix
      UI_LABEL_COLOR  = 0x4FE0; // LightMatrix
      UI_DIM_COLOR    = 0x0100; // DarkMatrix
      UI_BORDER_COLOR = 0x07C0; // MatrixEdge
      break;
    case 302: // Holo — dark grey / holographic pink / cyan / gold
      UI_HEADER_BG    = 0x1082; // HoloDark
      UI_FREQ_COLOR   = 0xF81F; // HoloPink
      UI_LEVEL_COLOR  = 0x07FF; // HoloCyan
      UI_LABEL_COLOR  = 0xFFE0; // HoloGold
      UI_DIM_COLOR    = 0x39E7; // HoloGrey
      UI_BORDER_COLOR = 0xC01F; // HoloViolet
      break;
    case 303: // Retro8bit — NES black / gold / green / red
      UI_HEADER_BG    = 0x0000; // NESBlack
      UI_FREQ_COLOR   = 0xFFC0; // NESGold
      UI_LEVEL_COLOR  = 0x07C0; // NESGreen
      UI_LABEL_COLOR  = 0xF800; // NESRed
      UI_DIM_COLOR    = 0x2104; // NESDark
      UI_BORDER_COLOR = 0x001F; // NESBlue
      break;
    case 304: // Terminal — black / bright phosphor / phosphor / mid phosphor
      UI_HEADER_BG    = 0x0000; // TermBlack
      UI_FREQ_COLOR   = 0x4FE0; // PhosphorBright
      UI_LEVEL_COLOR  = 0x07E0; // Phosphor
      UI_LABEL_COLOR  = 0x2FE0; // PhosphorMid
      UI_DIM_COLOR    = 0x0280; // PhosphorDark
      UI_BORDER_COLOR = 0x07C0; // PhosphorEdge
      break;
    case 305: // Cyber2 — cyber black / neon blue / electric blue / cyan
      UI_HEADER_BG    = 0x0000; // CyberBlack
      UI_FREQ_COLOR   = 0x6FFF; // NeonBlue
      UI_LEVEL_COLOR  = 0x001F; // ElecBlue
      UI_LABEL_COLOR  = 0x07FF; // CyberCyan
      UI_DIM_COLOR    = 0x0190; // DarkTeal2
      UI_BORDER_COLOR = 0x05FF; // CyberAzure
      break;
    case 306: // Glitch — black / white flash / cyan glitch / red error
      UI_HEADER_BG    = 0x0000; // GlitchBlack
      UI_FREQ_COLOR   = 0xFFFF; // WhiteFlash
      UI_LEVEL_COLOR  = 0x07FF; // CyanGlitch
      UI_LABEL_COLOR  = 0xF800; // RedError
      UI_DIM_COLOR    = 0x2104; // GlitchDark
      UI_BORDER_COLOR = 0xF81F; // GlitchMag
      break;
    case 307: // Pixel2 — warm dark / pixel gold / pixel green / pixel red
      UI_HEADER_BG    = 0x3940; // PixelWarm
      UI_FREQ_COLOR   = 0xFFC0; // PixelGold
      UI_LEVEL_COLOR  = 0x07C0; // PixelGreen
      UI_LABEL_COLOR  = 0xF800; // PixelRed
      UI_DIM_COLOR    = 0x2120; // PixelShadow
      UI_BORDER_COLOR = 0x001F; // PixelBlue
      break;
    case 308: // Neon2 — neon void / neon pink / neon cyan / neon yellow
      UI_HEADER_BG    = 0x0000; // NeonVoid
      UI_FREQ_COLOR   = 0xF81F; // NeonPink
      UI_LEVEL_COLOR  = 0x07FF; // NeonCyan2
      UI_LABEL_COLOR  = 0xFFE0; // NeonYellow
      UI_DIM_COLOR    = 0x2806; // DarkNeon
      UI_BORDER_COLOR = 0xF800; // NeonRed
      break;
    case 309: // Binary — binary black / white / grey / light grey
      UI_HEADER_BG    = 0x0000; // BinBlack
      UI_FREQ_COLOR   = 0xFFFF; // BinWhite
      UI_LEVEL_COLOR  = 0xC618; // BinGrey
      UI_LABEL_COLOR  = 0xEF7D; // BinLight
      UI_DIM_COLOR    = 0x2104; // BinDark
      UI_BORDER_COLOR = 0xFFFF; // BinEdge
      break;
    case 310: // Hologram — holo dark / holo cyan / holo blue / ice blue
      UI_HEADER_BG    = 0x0008; // HoloDarkBlue
      UI_FREQ_COLOR   = 0x07FF; // HoloCyan2
      UI_LEVEL_COLOR  = 0x6FFF; // HoloBlue
      UI_LABEL_COLOR  = 0xBFFF; // IceBlue
      UI_DIM_COLOR    = 0x0004; // HoloVoid
      UI_BORDER_COLOR = 0x3FFF; // Crystal2
      break;
    // ── Weather / atmosphere 311-320 ─────────────────────────
    case 311: // Vendaval — storm grey / lightning yellow / silver / grey
      UI_HEADER_BG    = 0x1084; // StormGrey
      UI_FREQ_COLOR   = 0xFFE0; // LightningYellow
      UI_LEVEL_COLOR  = 0xEF7D; // StormSilver
      UI_LABEL_COLOR  = 0xC618; // Grey2
      UI_DIM_COLOR    = 0x39E7; // StormMid
      UI_BORDER_COLOR = 0xFFE0; // LightningEdge
      break;
    case 312: // Nevoeiro — mist grey / white / pale grey / light silver
      UI_HEADER_BG    = 0x3186; // MistGrey
      UI_FREQ_COLOR   = 0xFFFF; // MistWhite
      UI_LEVEL_COLOR  = 0xBDF7; // PaleGrey
      UI_LABEL_COLOR  = 0xEF7D; // LightSilver
      UI_DIM_COLOR    = 0x4A49; // FogGrey
      UI_BORDER_COLOR = 0xCE59; // MistEdge
      break;
    case 313: // Arcoiris2 — black / yellow / red / cyan
      UI_HEADER_BG    = 0x0000; // RainBlack
      UI_FREQ_COLOR   = 0xFFE0; // RainYellow
      UI_LEVEL_COLOR  = 0xF800; // RainRed
      UI_LABEL_COLOR  = 0x07FF; // RainCyan
      UI_DIM_COLOR    = 0x2006; // RainDark
      UI_BORDER_COLOR = 0x07E0; // RainGreen
      break;
    case 314: // Trovao — dark storm / white flash / lightning / silver
      UI_HEADER_BG    = 0x0820; // TrovaoStorm
      UI_FREQ_COLOR   = 0xFFFF; // WhiteFlash2
      UI_LEVEL_COLOR  = 0xFFE0; // Lightning2
      UI_LABEL_COLOR  = 0xBDF7; // TrovaoSilver
      UI_DIM_COLOR    = 0x2124; // DarkStorm
      UI_BORDER_COLOR = 0xFFE0; // LightEdge
      break;
    case 315: // Brisa — sky deep / cloud white / sky blue / pale sky
      UI_HEADER_BG    = 0x0043; // SkyDeep
      UI_FREQ_COLOR   = 0xFFFF; // CloudWhite
      UI_LEVEL_COLOR  = 0x07FE; // SkyBlue2
      UI_LABEL_COLOR  = 0xAEFF; // PaleSky
      UI_DIM_COLOR    = 0x0021; // DeepSky
      UI_BORDER_COLOR = 0x7FFF; // LightBlue2
      break;
    case 316: // Nevoa — fog dark / silver / fog blue-grey / silver
      UI_HEADER_BG    = 0x2945; // FogDark
      UI_FREQ_COLOR   = 0xEF7D; // FogSilver
      UI_LEVEL_COLOR  = 0x867D; // FogBlueGrey
      UI_LABEL_COLOR  = 0xC618; // FogLabel
      UI_DIM_COLOR    = 0x18C5; // DeepFog
      UI_BORDER_COLOR = 0xAD75; // FogEdge
      break;
    case 317: // Seco — dry earth / sand / dry gold / pale sand
      UI_HEADER_BG    = 0x4940; // DryEarth
      UI_FREQ_COLOR   = 0xFFCA; // Sand
      UI_LEVEL_COLOR  = 0xFD40; // DryGold
      UI_LABEL_COLOR  = 0xF6D3; // PaleSand
      UI_DIM_COLOR    = 0x6900; // DarkDry
      UI_BORDER_COLOR = 0xBD40; // SandEdge
      break;
    case 318: // Humido — humid dark / humid blue / tropical green / sky light
      UI_HEADER_BG    = 0x0043; // HumidDark
      UI_FREQ_COLOR   = 0x07FE; // HumidBlue
      UI_LEVEL_COLOR  = 0x07E0; // TropGreen
      UI_LABEL_COLOR  = 0x7FFF; // SkyLight
      UI_DIM_COLOR    = 0x0022; // HumidVoid
      UI_BORDER_COLOR = 0x4FE0; // GreenEdge
      break;
    case 319: // Calor — heat dark / intense orange / red heat / orange
      UI_HEADER_BG    = 0x4800; // HeatDark
      UI_FREQ_COLOR   = 0xFF20; // HeatYellow
      UI_LEVEL_COLOR  = 0xF800; // RedHeat
      UI_LABEL_COLOR  = 0xFCA0; // HeatOrange
      UI_DIM_COLOR    = 0x3000; // HeatDim
      UI_BORDER_COLOR = 0xFC00; // OrangeRed2
      break;
    case 320: // Frio — deep cold blue / ice white / pale ice / icy blue
      UI_HEADER_BG    = 0x0008; // ColdBlue
      UI_FREQ_COLOR   = 0xEFFF; // IceWhite
      UI_LEVEL_COLOR  = 0xAEFF; // PaleIce
      UI_LABEL_COLOR  = 0xC7FF; // IcyBlue2
      UI_DIM_COLOR    = 0x0004; // ColdVoid
      UI_BORDER_COLOR = 0x6FFF; // ColdEdge
      break;
    // ── Brazilian foods & drinks 321-330 ─────────────────────
    case 321: // Acai2 — deep acai purple / cream / violet / vanilla
      UI_HEADER_BG    = 0x3005; // AcaiDeep
      UI_FREQ_COLOR   = 0xF7BE; // AcaiCream
      UI_LEVEL_COLOR  = 0xC01F; // AcaiViolet
      UI_LABEL_COLOR  = 0xFFD6; // Vanilla
      UI_DIM_COLOR    = 0x5007; // AcaiMid
      UI_BORDER_COLOR = 0x901F; // AcaiEdge
      break;
    case 322: // Coxinha — dark golden brown / golden crust / amber fry / cream
      UI_HEADER_BG    = 0x3140; // CoxinhaDark
      UI_FREQ_COLOR   = 0xFFC0; // GoldenCrust
      UI_LEVEL_COLOR  = 0xFD40; // AmberFry
      UI_LABEL_COLOR  = 0xFFBA; // CoxCream
      UI_DIM_COLOR    = 0x4A00; // CoxDark
      UI_BORDER_COLOR = 0xC560; // CoxCaramel
      break;
    case 323: // Brigadeiro — dark chocolate / cream / milk chocolate / cream light
      UI_HEADER_BG    = 0x2000; // ChocoDeep
      UI_FREQ_COLOR   = 0xFFBA; // ChocoCream
      UI_LEVEL_COLOR  = 0xC560; // MilkChoco
      UI_LABEL_COLOR  = 0xFEB2; // CreamLight
      UI_DIM_COLOR    = 0x1000; // DarkChoco
      UI_BORDER_COLOR = 0xAC00; // ChocoEdge
      break;
    case 324: // Caipirinha — deep lime / light lime / pure green / clear
      UI_HEADER_BG    = 0x0200; // LimeDeep
      UI_FREQ_COLOR   = 0xCFE0; // LightLime2
      UI_LEVEL_COLOR  = 0x07E0; // Lime2
      UI_LABEL_COLOR  = 0xFFFF; // Clear
      UI_DIM_COLOR    = 0x0100; // DarkLime
      UI_BORDER_COLOR = 0x4FE0; // LimeEdge
      break;
    case 325: // Mangaba — dark olive / mangaba orange / yellow / light green
      UI_HEADER_BG    = 0x2100; // OliveDark
      UI_FREQ_COLOR   = 0xFCA0; // MangabaOrange
      UI_LEVEL_COLOR  = 0xFFE0; // MangabaYellow
      UI_LABEL_COLOR  = 0x8FE0; // LightGreen2
      UI_DIM_COLOR    = 0x0A00; // OliveVoid
      UI_BORDER_COLOR = 0xFC60; // OrangeEdge2
      break;
    case 326: // Cupuacu — dark cream-brown / cream / warm brown / ivory
      UI_HEADER_BG    = 0x2940; // CupuDark
      UI_FREQ_COLOR   = 0xFFD6; // CupuCream
      UI_LEVEL_COLOR  = 0xC560; // WarmBrown
      UI_LABEL_COLOR  = 0xFEB2; // CupuIvory
      UI_DIM_COLOR    = 0x18C0; // CupuDeep
      UI_BORDER_COLOR = 0x9560; // BrownEdge
      break;
    case 327: // Murici — warm dark / murici yellow / amber / ivory light
      UI_HEADER_BG    = 0x2940; // MuriciDark
      UI_FREQ_COLOR   = 0xFFE0; // MuriciYellow
      UI_LEVEL_COLOR  = 0xFDA0; // MuriciAmber
      UI_LABEL_COLOR  = 0xFFF5; // IvoryLight
      UI_DIM_COLOR    = 0x18C0; // MuriciDeep
      UI_BORDER_COLOR = 0xED40; // GoldenEdge
      break;
    case 328: // Pitanga — dark pitanga red / orange-red / red / orange
      UI_HEADER_BG    = 0x3800; // PitangaDark
      UI_FREQ_COLOR   = 0xFF40; // PitangaOrangeRed
      UI_LEVEL_COLOR  = 0xF800; // PitangaRed
      UI_LABEL_COLOR  = 0xFCA0; // PitangaOrange
      UI_DIM_COLOR    = 0x2800; // PitangaDeep
      UI_BORDER_COLOR = 0xFB20; // PitangaEdge
      break;
    case 329: // Jenipapo — deep purple-black / blue-violet / indigo / violet light
      UI_HEADER_BG    = 0x0006; // JenipapoDeep
      UI_FREQ_COLOR   = 0x6FFF; // BlueViolet2
      UI_LEVEL_COLOR  = 0x4197; // Indigo2
      UI_LABEL_COLOR  = 0xB51D; // VioletLight2
      UI_DIM_COLOR    = 0x0003; // JenipapoVoid
      UI_BORDER_COLOR = 0x837A; // VioletEdge2
      break;
    case 330: // Buriti — dark warm brown / buriti orange / golden amber / cream
      UI_HEADER_BG    = 0x3120; // BuritiDark
      UI_FREQ_COLOR   = 0xFCA0; // BuritiOrange
      UI_LEVEL_COLOR  = 0xFD20; // GoldenAmber
      UI_LABEL_COLOR  = 0xFEB2; // BuritiCream
      UI_DIM_COLOR    = 0x4A00; // BuritiDeep
      UI_BORDER_COLOR = 0xF980; // AmberEdge
      break;
    case 331: // Junino — night sky blue / bunting yellow / bonfire red / amber
      UI_HEADER_BG    = 0x0010; // NightSky
      UI_FREQ_COLOR   = 0xFFE0; // BuntingYellow
      UI_LEVEL_COLOR  = 0xF800; // BonfireRed
      UI_LABEL_COLOR  = 0xFFFF; // White
      UI_DIM_COLOR    = 0x0006; // DeepNight
      UI_BORDER_COLOR = 0xFD20; // FireAmber
      break;
    // ── Seasons & time of day 332-341 ────────────────────────
    case 332: // Primavera — dark pink / cherry blossom / spring green / white
      UI_HEADER_BG    = 0x38C5; // SpringDark
      UI_FREQ_COLOR   = 0xFBB5; // CherryBlossom
      UI_LEVEL_COLOR  = 0x07E0; // SpringGreen
      UI_LABEL_COLOR  = 0xFFFF; // White
      UI_DIM_COLOR    = 0x5845; // PinkDim
      UI_BORDER_COLOR = 0x4FE0; // LimeEdge
      break;
    case 333: // Verao — deep teal / vivid coral / bright cyan / sand
      UI_HEADER_BG    = 0x0043; // SummerTeal
      UI_FREQ_COLOR   = 0xFB6A; // VividCoral
      UI_LEVEL_COLOR  = 0x07FF; // BrightCyan
      UI_LABEL_COLOR  = 0xFFD6; // SandCream
      UI_DIM_COLOR    = 0x0021; // TealDeep
      UI_BORDER_COLOR = 0xFC60; // SummerOrange
      break;
    case 334: // Outono — dark brown / warm orange / rust red / tan
      UI_HEADER_BG    = 0x2800; // AutumnDark
      UI_FREQ_COLOR   = 0xFCA0; // WarmOrange
      UI_LEVEL_COLOR  = 0xD100; // RustRed
      UI_LABEL_COLOR  = 0xF6D3; // Tan
      UI_DIM_COLOR    = 0x3800; // AutumnDeep
      UI_BORDER_COLOR = 0xC440; // BurntUmber
      break;
    case 335: // Inverno — deep cold blue / silver white / pale ice / cool silver
      UI_HEADER_BG    = 0x0010; // WinterBlue
      UI_FREQ_COLOR   = 0xEFFF; // SilverWhite
      UI_LEVEL_COLOR  = 0xAEFF; // PaleIce
      UI_LABEL_COLOR  = 0xBDF7; // CoolSilver2
      UI_DIM_COLOR    = 0x0008; // NightBlue2
      UI_BORDER_COLOR = 0x6FFF; // IcyEdge
      break;
    case 336: // Amanhecer — dawn purple / golden rose / warm orange / cream
      UI_HEADER_BG    = 0x2806; // DawnPurple
      UI_FREQ_COLOR   = 0xFD8C; // GoldenRose
      UI_LEVEL_COLOR  = 0xFCA0; // WarmOrange2
      UI_LABEL_COLOR  = 0xFFD6; // DawnCream
      UI_DIM_COLOR    = 0x4008; // DawnDark
      UI_BORDER_COLOR = 0xFB20; // FireAmber2
      break;
    case 337: // Manha2 — morning sky / morning yellow / sky blue / pale ivory
      UI_HEADER_BG    = 0x0843; // MorningSky
      UI_FREQ_COLOR   = 0xFFE0; // MorningYellow
      UI_LEVEL_COLOR  = 0x07FE; // SkyBlue3
      UI_LABEL_COLOR  = 0xFFF5; // PaleIvory
      UI_DIM_COLOR    = 0x0422; // DimMorning
      UI_BORDER_COLOR = 0xFDA0; // GoldEdge2
      break;
    case 338: // Meio-Dia — noon blue / noon yellow / white heat / pale sky
      UI_HEADER_BG    = 0x0018; // NoonBlue
      UI_FREQ_COLOR   = 0xFFE0; // NoonYellow
      UI_LEVEL_COLOR  = 0xFFFF; // WhiteHeat
      UI_LABEL_COLOR  = 0xADD5; // PaleSky
      UI_DIM_COLOR    = 0x000C; // SkyDeep2
      UI_BORDER_COLOR = 0xFD20; // SunEdge
      break;
    case 339: // Tarde — warm amber bg / deep amber / afternoon orange / cream
      UI_HEADER_BG    = 0x3940; // WarmAmberBg
      UI_FREQ_COLOR   = 0xFD40; // DeepAmber
      UI_LEVEL_COLOR  = 0xFC60; // AfternoonOrange
      UI_LABEL_COLOR  = 0xFFD6; // AfternoonCream
      UI_DIM_COLOR    = 0x4A00; // DarkAmber
      UI_BORDER_COLOR = 0xFB20; // WarmEdge
      break;
    case 340: // Entardecer — dusk purple / dusk pink / dusk orange / lavender
      UI_HEADER_BG    = 0x2006; // DuskPurple
      UI_FREQ_COLOR   = 0xFB8F; // DuskPink
      UI_LEVEL_COLOR  = 0xFCA0; // DuskOrange
      UI_LABEL_COLOR  = 0xCCBF; // DuskLavender
      UI_DIM_COLOR    = 0x4008; // DuskDark
      UI_BORDER_COLOR = 0xFD00; // AmberDusk
      break;
    case 341: // Madrugada — midnight void / cold silver / moonblue / dark silver
      UI_HEADER_BG    = 0x0004; // MidnightVoid
      UI_FREQ_COLOR   = 0xBDF7; // ColdSilver
      UI_LEVEL_COLOR  = 0x6FFF; // MoonBlue
      UI_LABEL_COLOR  = 0x867D; // DarkSilver
      UI_DIM_COLOR    = 0x0002; // AbyssVoid
      UI_BORDER_COLOR = 0x4FFF; // Moonlight
      break;
    // ── Brazilian folklore 342-351 ────────────────────────────
    case 342: // Iara — deep river / aqua silver / teal shimmer / shimmer white
      UI_HEADER_BG    = 0x0043; // RiverDeep
      UI_FREQ_COLOR   = 0xBFFF; // AquaSilver
      UI_LEVEL_COLOR  = 0x07FE; // TealShimmer
      UI_LABEL_COLOR  = 0xEFFF; // ShimmerWhite
      UI_DIM_COLOR    = 0x0021; // DeepRiver
      UI_BORDER_COLOR = 0x3FFF; // CrystalWater
      break;
    case 343: // Curupira — dark forest / fire red / bright green / orange
      UI_HEADER_BG    = 0x0200; // CuruForest
      UI_FREQ_COLOR   = 0xF800; // FireRed
      UI_LEVEL_COLOR  = 0x07E0; // BrightGreen2
      UI_LABEL_COLOR  = 0xFC60; // CuruOrange
      UI_DIM_COLOR    = 0x0100; // ForestVoid
      UI_BORDER_COLOR = 0xFD20; // FireEdge
      break;
    case 344: // Cuca — swamp dark / sickly green / teal green / murky
      UI_HEADER_BG    = 0x0180; // SwampDark
      UI_FREQ_COLOR   = 0x6FE0; // SicklyGreen
      UI_LEVEL_COLOR  = 0x07C0; // TealGreen
      UI_LABEL_COLOR  = 0xCFE0; // MurkyYellow
      UI_DIM_COLOR    = 0x00C0; // SwampDeep
      UI_BORDER_COLOR = 0x4DC8; // MurkyTeal
      break;
    case 345: // Boto — dark river / boto pink / river blue / cream
      UI_HEADER_BG    = 0x0043; // BotoRiver
      UI_FREQ_COLOR   = 0xFBB5; // BotoPink
      UI_LEVEL_COLOR  = 0x5DFF; // RiverBlue
      UI_LABEL_COLOR  = 0xFFD6; // BotoCream
      UI_DIM_COLOR    = 0x0021; // RiverDeep
      UI_BORDER_COLOR = 0xFB8F; // PinkEdge
      break;
    case 346: // Saci — void / saci red / silver / white
      UI_HEADER_BG    = 0x0000; // SaciVoid
      UI_FREQ_COLOR   = 0xF800; // SaciRed
      UI_LEVEL_COLOR  = 0xEF7D; // SaciSilver
      UI_LABEL_COLOR  = 0xFFFF; // SaciWhite
      UI_DIM_COLOR    = 0x2104; // SaciDark
      UI_BORDER_COLOR = 0xD800; // SaciDarkRed
      break;
    case 347: // Mapinguari — dark umber / brown red / dark amber / earth tan
      UI_HEADER_BG    = 0x2800; // UmberDark
      UI_FREQ_COLOR   = 0xBD00; // BrownRed
      UI_LEVEL_COLOR  = 0xAC00; // DarkAmber2
      UI_LABEL_COLOR  = 0xF6D3; // EarthTan
      UI_DIM_COLOR    = 0x2000; // DeepUmber
      UI_BORDER_COLOR = 0x8400; // UmberEdge
      break;
    case 348: // CobraGrande — serpent dark / gold scales / serpent green / amber
      UI_HEADER_BG    = 0x0180; // SerpentDark
      UI_FREQ_COLOR   = 0xFDA0; // GoldScales
      UI_LEVEL_COLOR  = 0x07E0; // SerpentGreen
      UI_LABEL_COLOR  = 0xED40; // ScaleAmber
      UI_DIM_COLOR    = 0x00C0; // DeepGreen
      UI_BORDER_COLOR = 0xC500; // DarkGold3
      break;
    case 349: // Matinta — dark mist grey / ghost white / pale grey / silver
      UI_HEADER_BG    = 0x1084; // GhostMist
      UI_FREQ_COLOR   = 0xFFFF; // GhostWhite
      UI_LEVEL_COLOR  = 0xBDF7; // PaleGhost
      UI_LABEL_COLOR  = 0xEF7D; // GhostSilver
      UI_DIM_COLOR    = 0x39E7; // MistMid
      UI_BORDER_COLOR = 0xCE59; // GhostEdge
      break;
    case 350: // Lobisomem — night deep / moon silver / wolf grey / grey
      UI_HEADER_BG    = 0x0004; // NightDeep2
      UI_FREQ_COLOR   = 0xEF7D; // MoonSilver
      UI_LEVEL_COLOR  = 0x867D; // WolfGrey
      UI_LABEL_COLOR  = 0xC618; // Grey3
      UI_DIM_COLOR    = 0x0002; // Darkness
      UI_BORDER_COLOR = 0xBDF7; // Moonlight2
      break;
    case 351: // Bumba — black / red / white / gold
      UI_HEADER_BG    = 0x0000; // BumbaBlack
      UI_FREQ_COLOR   = 0xF800; // BumbaRed
      UI_LEVEL_COLOR  = 0xFFFF; // BumbaWhite
      UI_LABEL_COLOR  = 0xFFE0; // BumbaGold
      UI_DIM_COLOR    = 0x2104; // BumbaDark
      UI_BORDER_COLOR = 0xF800; // BumbaEdge
      break;
    // ── Ocean / sea 352-361 ───────────────────────────────────
    case 352: // Maresia — ocean dark / teal / white foam / sand
      UI_HEADER_BG    = 0x0042; // OceanDark
      UI_FREQ_COLOR   = 0x07FE; // SeaTeal
      UI_LEVEL_COLOR  = 0xFFFF; // WhiteFoam
      UI_LABEL_COLOR  = 0xFFD6; // BeachSand
      UI_DIM_COLOR    = 0x0021; // OceanDeep2
      UI_BORDER_COLOR = 0x5DFF; // SkyEdge2
      break;
    case 353: // Onda — dark navy / wave cyan / white crest / light blue
      UI_HEADER_BG    = 0x0010; // WaveNavy
      UI_FREQ_COLOR   = 0x07FF; // WaveCyan
      UI_LEVEL_COLOR  = 0xFFFF; // WhiteCrest
      UI_LABEL_COLOR  = 0x6FFF; // LightBlue3
      UI_DIM_COLOR    = 0x0006; // NavyDeep
      UI_BORDER_COLOR = 0x3FFF; // WaterEdge
      break;
    case 354: // Fundo — abyss / bioluminescent blue / deep cyan / pale biolum
      UI_HEADER_BG    = 0x0001; // Abyss
      UI_FREQ_COLOR   = 0x05BF; // BiolumBlue
      UI_LEVEL_COLOR  = 0x07FF; // DeepCyan
      UI_LABEL_COLOR  = 0x3FFF; // PaleBiolum
      UI_DIM_COLOR    = 0x0000; // AbyssVoid2
      UI_BORDER_COLOR = 0x031F; // AbyssEdge
      break;
    case 355: // Peixe — deep ocean / vivid orange / electric blue / yellow
      UI_HEADER_BG    = 0x0008; // FishOcean
      UI_FREQ_COLOR   = 0xFC60; // FishOrange
      UI_LEVEL_COLOR  = 0x001F; // FishBlue
      UI_LABEL_COLOR  = 0xFFE0; // FishYellow
      UI_DIM_COLOR    = 0x0004; // FishDeep
      UI_BORDER_COLOR = 0xFB20; // FishEdge
      break;
    case 356: // Coral3 — reef deep / coral pink / vivid coral / light pink
      UI_HEADER_BG    = 0x0043; // ReefDeep
      UI_FREQ_COLOR   = 0xFB8F; // CoralPink
      UI_LEVEL_COLOR  = 0xFB40; // VividCoral2
      UI_LABEL_COLOR  = 0xFCBF; // LightPink
      UI_DIM_COLOR    = 0x0021; // ReefVoid
      UI_BORDER_COLOR = 0xFC60; // CoralOrange2
      break;
    case 357: // Alga — seaweed dark / sea green / bright teal / jade
      UI_HEADER_BG    = 0x00C3; // SeaweedDark
      UI_FREQ_COLOR   = 0x4634; // SeaGreen
      UI_LEVEL_COLOR  = 0x07FE; // BrightTeal
      UI_LABEL_COLOR  = 0x45C8; // JadeGreen2
      UI_DIM_COLOR    = 0x0062; // TealVoid2
      UI_BORDER_COLOR = 0x07C0; // TealEdge
      break;
    case 358: // Areia — sandy dark / pale sand / sky blue / cream
      UI_HEADER_BG    = 0x4940; // SandyDark2
      UI_FREQ_COLOR   = 0xFFD6; // PaleSand
      UI_LEVEL_COLOR  = 0x07FE; // SkyBlue4
      UI_LABEL_COLOR  = 0xFFF5; // SandCream
      UI_DIM_COLOR    = 0x6900; // DarkSand2
      UI_BORDER_COLOR = 0x5DFF; // SkyEdge3
      break;
    case 359: // Mare — grey-blue dark / silver tide / pale silver / white
      UI_HEADER_BG    = 0x2124; // TideDark
      UI_FREQ_COLOR   = 0xBDF7; // SilverTide
      UI_LEVEL_COLOR  = 0xEF7D; // TideSilver
      UI_LABEL_COLOR  = 0xFFFF; // TideWhite
      UI_DIM_COLOR    = 0x39E7; // TideMid
      UI_BORDER_COLOR = 0xCE59; // TideEdge
      break;
    case 360: // Nautico — deep navy / white / anchor red / gold trim
      UI_HEADER_BG    = 0x0010; // NavalDeep
      UI_FREQ_COLOR   = 0xFFFF; // SailWhite
      UI_LEVEL_COLOR  = 0xF800; // AnchorRed
      UI_LABEL_COLOR  = 0xFDA0; // GoldTrim
      UI_DIM_COLOR    = 0x0006; // NavalVoid
      UI_BORDER_COLOR = 0xFDA0; // GoldEdge3
      break;
    case 361: // Futebol — dark field green / yellow jersey / field green / blue
      UI_HEADER_BG    = 0x0200; // FieldDark
      UI_FREQ_COLOR   = 0xFFE0; // YellowJersey
      UI_LEVEL_COLOR  = 0x07E0; // FieldGreen
      UI_LABEL_COLOR  = 0x001F; // JerseyBlue
      UI_DIM_COLOR    = 0x0100; // DarkField
      UI_BORDER_COLOR = 0x07C0; // GreenEdge2
      break;
    // ── Sports 362-370 ────────────────────────────────────────
    case 362: // Volei — beach blue / sky cyan / white / sand
      UI_HEADER_BG    = 0x0008; // BeachBlue
      UI_FREQ_COLOR   = 0x07FF; // SkyCyan
      UI_LEVEL_COLOR  = 0xFFFF; // WhiteBall
      UI_LABEL_COLOR  = 0xFFCA; // VoleiSand
      UI_DIM_COLOR    = 0x0004; // BeachVoid
      UI_BORDER_COLOR = 0x5DFF; // SkyEdge4
      break;
    case 363: // Surfe — ocean deep / wave blue / white foam / sunset orange
      UI_HEADER_BG    = 0x0010; // SurfOcean
      UI_FREQ_COLOR   = 0x07FE; // WaveBlue
      UI_LEVEL_COLOR  = 0xFFFF; // SurfFoam
      UI_LABEL_COLOR  = 0xFCA0; // SunsetOrange2
      UI_DIM_COLOR    = 0x0006; // DeepSurf
      UI_BORDER_COLOR = 0xFC60; // OrangeEdge3
      break;
    case 364: // MMA — fight black / fight red / silver / light silver
      UI_HEADER_BG    = 0x0000; // FightBlack
      UI_FREQ_COLOR   = 0xF800; // FightRed
      UI_LEVEL_COLOR  = 0xEF7D; // FightSilver
      UI_LABEL_COLOR  = 0xC618; // FightGrey
      UI_DIM_COLOR    = 0x2104; // FightDark
      UI_BORDER_COLOR = 0xB800; // FightEdge
      break;
    case 365: // Corrida — race black / race orange / race red / white
      UI_HEADER_BG    = 0x0000; // RaceBlack
      UI_FREQ_COLOR   = 0xFC60; // RaceOrange
      UI_LEVEL_COLOR  = 0xF800; // RaceRed
      UI_LABEL_COLOR  = 0xFFFF; // RaceWhite
      UI_DIM_COLOR    = 0x2104; // RaceDark
      UI_BORDER_COLOR = 0xFDA0; // RaceAmber
      break;
    case 366: // Natacao — pool dark / pool cyan / white lane / light pool
      UI_HEADER_BG    = 0x0010; // PoolDark
      UI_FREQ_COLOR   = 0x07FF; // PoolCyan
      UI_LEVEL_COLOR  = 0xFFFF; // WhiteLane
      UI_LABEL_COLOR  = 0x6FFF; // LightPool
      UI_DIM_COLOR    = 0x0006; // DeepPool
      UI_BORDER_COLOR = 0x07FE; // PoolEdge
      break;
    case 367: // Ciclismo — black / yellow jersey / silver / white
      UI_HEADER_BG    = 0x0000; // CycleBlack
      UI_FREQ_COLOR   = 0xFFE0; // YellowJersey2
      UI_LEVEL_COLOR  = 0xC618; // CycleSilver
      UI_LABEL_COLOR  = 0xFFFF; // CycleWhite
      UI_DIM_COLOR    = 0x2104; // CycleDark
      UI_BORDER_COLOR = 0xFDA0; // CycleGold
      break;
    case 368: // Boxe — dark ring / ring red / canvas white / silver gloves
      UI_HEADER_BG    = 0x2800; // RingDark
      UI_FREQ_COLOR   = 0xF800; // RingRed
      UI_LEVEL_COLOR  = 0xFFFF; // CanvasWhite
      UI_LABEL_COLOR  = 0xEF7D; // GlovesSilver
      UI_DIM_COLOR    = 0x4808; // BoxeDark
      UI_BORDER_COLOR = 0xB800; // BoxeEdge
      break;
    case 369: // Basquete — hardwood dark / ball orange / white / amber
      UI_HEADER_BG    = 0x3120; // HardwoodDark
      UI_FREQ_COLOR   = 0xFC60; // BallOrange
      UI_LEVEL_COLOR  = 0xFFFF; // BasqueteWhite
      UI_LABEL_COLOR  = 0xFDA0; // BasqueteAmber
      UI_DIM_COLOR    = 0x4A00; // DarkWood
      UI_BORDER_COLOR = 0xFB20; // OrangeEdge4
      break;
    case 370: // Tenis — court dark / white / tennis yellow / light green
      UI_HEADER_BG    = 0x0200; // CourtDark
      UI_FREQ_COLOR   = 0xFFFF; // TenisWhite
      UI_LEVEL_COLOR  = 0xFFE0; // TenisYellow
      UI_LABEL_COLOR  = 0xCFE0; // CourtLight
      UI_DIM_COLOR    = 0x0100; // DarkCourt
      UI_BORDER_COLOR = 0xFFE0; // TenisEdge
      break;
    // ── Flowers 371-381 ──────────────────────────────────────
    case 371: // Rosa2 — deep rose / rose red / pink / cream
      UI_HEADER_BG    = 0x3003; // RosaDark
      UI_FREQ_COLOR   = 0xF004; // RosaRed
      UI_LEVEL_COLOR  = 0xFB8F; // RosaPink
      UI_LABEL_COLOR  = 0xFFD6; // RosaCream
      UI_DIM_COLOR    = 0x5001; // RosaDeep
      UI_BORDER_COLOR = 0xF81A; // RosaEdge
      break;
    case 372: // Girassol — stem green / sunflower yellow / brown center / gold
      UI_HEADER_BG    = 0x0200; // StemGreen
      UI_FREQ_COLOR   = 0xFFE0; // SunflowerYellow
      UI_LEVEL_COLOR  = 0xA540; // BrownCenter
      UI_LABEL_COLOR  = 0xFDA0; // SunGold
      UI_DIM_COLOR    = 0x0100; // DarkStem
      UI_BORDER_COLOR = 0xED40; // GoldEdge4
      break;
    case 373: // Orquidea — orchid dark / orchid purple / white / green
      UI_HEADER_BG    = 0x2006; // OrchidDark
      UI_FREQ_COLOR   = 0xC31F; // OrchidPurple
      UI_LEVEL_COLOR  = 0xFFFF; // OrchidWhite
      UI_LABEL_COLOR  = 0x07E0; // OrchidGreen
      UI_DIM_COLOR    = 0x4006; // OrchidDeep
      UI_BORDER_COLOR = 0xE01F; // OrchidViolet
      break;
    case 374: // Ipe — ipe dark wood / ipe yellow / warm gold / cream
      UI_HEADER_BG    = 0x2820; // IpeDark
      UI_FREQ_COLOR   = 0xFFE0; // IpeYellow
      UI_LEVEL_COLOR  = 0xFDA0; // IpeGold
      UI_LABEL_COLOR  = 0xFFF5; // IpeCream
      UI_DIM_COLOR    = 0x2000; // IpeDeep
      UI_BORDER_COLOR = 0xED40; // IpeEdge
      break;
    case 375: // VitoriaRegia — deep water / lily green / white flower / pale pink
      UI_HEADER_BG    = 0x0043; // LilyWater
      UI_FREQ_COLOR   = 0x07E0; // LilyGreen
      UI_LEVEL_COLOR  = 0xFFFF; // LilyWhite
      UI_LABEL_COLOR  = 0xFBB5; // LilyPink
      UI_DIM_COLOR    = 0x0021; // WaterVoid
      UI_BORDER_COLOR = 0x45C8; // LilyEdge
      break;
    case 376: // Bromelia — tropical dark / vivid red / yellow center / orange
      UI_HEADER_BG    = 0x0200; // TropDark
      UI_FREQ_COLOR   = 0xF800; // BromeliaRed
      UI_LEVEL_COLOR  = 0xFFE0; // YellowCenter
      UI_LABEL_COLOR  = 0xFCA0; // BromeliaOrange
      UI_DIM_COLOR    = 0x0100; // DeepTrop
      UI_BORDER_COLOR = 0xFB20; // TropEdge
      break;
    case 377: // Heliconia — jungle dark / heli orange / heli red / yellow
      UI_HEADER_BG    = 0x0180; // JungleDark2
      UI_FREQ_COLOR   = 0xFCA0; // HeliOrange
      UI_LEVEL_COLOR  = 0xF800; // HeliRed
      UI_LABEL_COLOR  = 0xFFE0; // HeliYellow
      UI_DIM_COLOR    = 0x00C0; // JungleVoid2
      UI_BORDER_COLOR = 0xFB20; // FireEdge2
      break;
    case 378: // Passiflora — passion dark / white petals / passion purple / orange center
      UI_HEADER_BG    = 0x2006; // PassionDark
      UI_FREQ_COLOR   = 0xFFFF; // WhitePetals
      UI_LEVEL_COLOR  = 0xC31F; // PassionPurple
      UI_LABEL_COLOR  = 0xFCA0; // OrangeCenter
      UI_DIM_COLOR    = 0x4008; // PassionDeep
      UI_BORDER_COLOR = 0xE01F; // PassionEdge
      break;
    case 379: // Cravinho — dark magenta / magenta / pink / light pink
      UI_HEADER_BG    = 0x3806; // CravDark
      UI_FREQ_COLOR   = 0xF81F; // CravMagenta
      UI_LEVEL_COLOR  = 0xFB8F; // CravPink
      UI_LABEL_COLOR  = 0xFCBF; // CravLight
      UI_DIM_COLOR    = 0x500D; // CravDeep
      UI_BORDER_COLOR = 0xF01F; // CravEdge
      break;
    case 380: // Castanha — dark brown / chestnut / warm brown / cream
      UI_HEADER_BG    = 0x2000; // ChestDark
      UI_FREQ_COLOR   = 0xA540; // Chestnut
      UI_LEVEL_COLOR  = 0xC560; // WarmBrown2
      UI_LABEL_COLOR  = 0xFFBA; // ChestCream
      UI_DIM_COLOR    = 0x1000; // ChestDeep
      UI_BORDER_COLOR = 0x8420; // ChestEdge
      break;
    case 381: // Xaxado — dark terracotta / xaxado red / amber / cream
      UI_HEADER_BG    = 0x3800; // XaxadoDark
      UI_FREQ_COLOR   = 0xF800; // XaxadoRed
      UI_LEVEL_COLOR  = 0xFD20; // XaxadoAmber
      UI_LABEL_COLOR  = 0xFFD6; // XaxadoCream
      UI_DIM_COLOR    = 0x2800; // XaxadoDeep
      UI_BORDER_COLOR = 0xC440; // XaxadoEdge
      break;
    case 382: // Cordel — classic cordel blue / white / red / yellow
      UI_HEADER_BG    = 0x001F; // CordelBlue
      UI_FREQ_COLOR   = 0xFFFF; // CordelWhite
      UI_LEVEL_COLOR  = 0xF800; // CordelRed
      UI_LABEL_COLOR  = 0xFFE0; // CordelYellow
      UI_DIM_COLOR    = 0x0008; // CordelDeep
      UI_BORDER_COLOR = 0xFD20; // CordelOrange
      break;
    // ── Instruments 383-399 ──────────────────────────────────
    case 383: UI_HEADER_BG=0x2820; UI_FREQ_COLOR=0xC560; UI_LEVEL_COLOR=0x8400; UI_LABEL_COLOR=0xFDA0; UI_DIM_COLOR=0x2000; UI_BORDER_COLOR=0x8420; break; // Viola
    case 384: UI_HEADER_BG=0x0000; UI_FREQ_COLOR=0xE000; UI_LEVEL_COLOR=0xF800; UI_LABEL_COLOR=0xFFE0; UI_DIM_COLOR=0x2104; UI_BORDER_COLOR=0xB800; break; // Sanfona
    case 385: UI_HEADER_BG=0x3940; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0x07C0; UI_LABEL_COLOR=0xF6D3; UI_DIM_COLOR=0x4A00; UI_BORDER_COLOR=0xBD40; break; // Cavaquinho
    case 386: UI_HEADER_BG=0x3806; UI_FREQ_COLOR=0xF81F; UI_LEVEL_COLOR=0xFFE0; UI_LABEL_COLOR=0xFCBF; UI_DIM_COLOR=0x500D; UI_BORDER_COLOR=0xF980; break; // Pandeiro
    case 387: UI_HEADER_BG=0x4800; UI_FREQ_COLOR=0xFF20; UI_LEVEL_COLOR=0xF800; UI_LABEL_COLOR=0xFCA0; UI_DIM_COLOR=0x3000; UI_BORDER_COLOR=0xFC00; break; // Zabumba
    case 388: UI_HEADER_BG=0x0200; UI_FREQ_COLOR=0x67E0; UI_LEVEL_COLOR=0x07E0; UI_LABEL_COLOR=0xFFE0; UI_DIM_COLOR=0x0100; UI_BORDER_COLOR=0x4FE0; break; // Berimbau
    case 389: UI_HEADER_BG=0x1064; UI_FREQ_COLOR=0xCB3B; UI_LEVEL_COLOR=0x66F4; UI_LABEL_COLOR=0x8DBC; UI_DIM_COLOR=0x0842; UI_BORDER_COLOR=0xA419; break; // Atabaque
    case 390: UI_HEADER_BG=0x2006; UI_FREQ_COLOR=0xE0DF; UI_LEVEL_COLOR=0xF81F; UI_LABEL_COLOR=0xCCBF; UI_DIM_COLOR=0x4006; UI_BORDER_COLOR=0x837A; break; // Cuica
    case 391: UI_HEADER_BG=0x0200; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0xFC60; UI_LABEL_COLOR=0xFCA0; UI_DIM_COLOR=0x0100; UI_BORDER_COLOR=0xFB20; break; // Agogo
    case 392: UI_HEADER_BG=0x3008; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0xFC60; UI_LABEL_COLOR=0xF81F; UI_DIM_COLOR=0x6006; UI_BORDER_COLOR=0xFB20; break; // Tamborim
    case 393: UI_HEADER_BG=0x4940; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0x07C0; UI_LABEL_COLOR=0xFFCA; UI_DIM_COLOR=0x6900; UI_BORDER_COLOR=0xFFE0; break; // Ganza
    case 394: UI_HEADER_BG=0x2140; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0xED40; UI_LABEL_COLOR=0xFFE0; UI_DIM_COLOR=0x6B20; UI_BORDER_COLOR=0xED40; break; // Reco
    case 395: UI_HEADER_BG=0x0008; UI_FREQ_COLOR=0x6FFF; UI_LEVEL_COLOR=0x001F; UI_LABEL_COLOR=0xBDF7; UI_DIM_COLOR=0x0004; UI_BORDER_COLOR=0x87FF; break; // Bandolim
    case 396: UI_HEADER_BG=0x07E0; UI_FREQ_COLOR=0x001F; UI_LEVEL_COLOR=0xF81F; UI_LABEL_COLOR=0xFFFF; UI_DIM_COLOR=0x0280; UI_BORDER_COLOR=0xFC00; break; // Ukulele
    case 397: UI_HEADER_BG=0x0010; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0x001F; UI_LABEL_COLOR=0xEF7D; UI_DIM_COLOR=0x0006; UI_BORDER_COLOR=0xFF20; break; // Lira
    case 398: UI_HEADER_BG=0x2006; UI_FREQ_COLOR=0xE0DF; UI_LEVEL_COLOR=0xC31F; UI_LABEL_COLOR=0xE01F; UI_DIM_COLOR=0x4006; UI_BORDER_COLOR=0xE01F; break; // Harpa
    case 399: UI_HEADER_BG=0x0008; UI_FREQ_COLOR=0xFFFF; UI_LEVEL_COLOR=0x001F; UI_LABEL_COLOR=0xBDF7; UI_DIM_COLOR=0x0004; UI_BORDER_COLOR=0x87FF; break; // Trompa
    // ── Gemstones 400-449: auto-generated with gem-inspired palettes
    case 400: UI_HEADER_BG=0x2000; UI_FREQ_COLOR=0xF004; UI_LEVEL_COLOR=0xD004; UI_LABEL_COLOR=0xFACA; UI_DIM_COLOR=0x5004; UI_BORDER_COLOR=0xE800; break;
    case 401: UI_HEADER_BG=0x0008; UI_FREQ_COLOR=0x231A; UI_LEVEL_COLOR=0x4197; UI_LABEL_COLOR=0xB51D; UI_DIM_COLOR=0x0823; UI_BORDER_COLOR=0x837A; break;
    case 402: UI_HEADER_BG=0x08C3; UI_FREQ_COLOR=0x4634; UI_LEVEL_COLOR=0x240C; UI_LABEL_COLOR=0x4634; UI_DIM_COLOR=0x11C5; UI_BORDER_COLOR=0x240C; break;
    case 403: UI_HEADER_BG=0x2140; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0xED40; UI_LABEL_COLOR=0xFFE0; UI_DIM_COLOR=0x6B20; UI_BORDER_COLOR=0xED40; break;
    case 404: UI_HEADER_BG=0x2006; UI_FREQ_COLOR=0xC31F; UI_LEVEL_COLOR=0xF81F; UI_LABEL_COLOR=0xE0DF; UI_DIM_COLOR=0x500A; UI_BORDER_COLOR=0xC01F; break;
    case 405: UI_HEADER_BG=0x2140; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0xED40; UI_LABEL_COLOR=0xFFE0; UI_DIM_COLOR=0x6B20; UI_BORDER_COLOR=0xED40; break;
    case 406: UI_HEADER_BG=0x2000; UI_FREQ_COLOR=0xE800; UI_LEVEL_COLOR=0xD004; UI_LABEL_COLOR=0xFACA; UI_DIM_COLOR=0x5004; UI_BORDER_COLOR=0xE800; break;
    case 407: UI_HEADER_BG=0x2843; UI_FREQ_COLOR=0xFC19; UI_LEVEL_COLOR=0x45C8; UI_LABEL_COLOR=0xFC19; UI_DIM_COLOR=0x610C; UI_BORDER_COLOR=0x45C8; break;
    case 408: UI_HEADER_BG=0x2042; UI_FREQ_COLOR=0xD18D; UI_LEVEL_COLOR=0xC9EC; UI_LABEL_COLOR=0xF475; UI_DIM_COLOR=0x1021; UI_BORDER_COLOR=0xB18B; break;
    case 409: UI_HEADER_BG=0x2104; UI_FREQ_COLOR=0xF7BE; UI_LEVEL_COLOR=0xC618; UI_LABEL_COLOR=0xF7BE; UI_DIM_COLOR=0x7BEF; UI_BORDER_COLOR=0x9CF3; break;
    case 410: UI_HEADER_BG=0x0043; UI_FREQ_COLOR=0xFB40; UI_LEVEL_COLOR=0xFB40; UI_LABEL_COLOR=0xFCBF; UI_DIM_COLOR=0x0021; UI_BORDER_COLOR=0xFC60; break;
    case 411: UI_HEADER_BG=0x3940; UI_FREQ_COLOR=0xFFD6; UI_LEVEL_COLOR=0xCD40; UI_LABEL_COLOR=0xFDA0; UI_DIM_COLOR=0x4A40; UI_BORDER_COLOR=0xBD00; break;
    case 412: UI_HEADER_BG=0x0043; UI_FREQ_COLOR=0x45C8; UI_LEVEL_COLOR=0x45C8; UI_LABEL_COLOR=0x45C8; UI_DIM_COLOR=0x0022; UI_BORDER_COLOR=0x45C8; break;
    case 413: UI_HEADER_BG=0x0008; UI_FREQ_COLOR=0xFDA2; UI_LEVEL_COLOR=0x231A; UI_LABEL_COLOR=0xFDA2; UI_DIM_COLOR=0x0086; UI_BORDER_COLOR=0x231A; break;
    case 414: UI_HEADER_BG=0x0043; UI_FREQ_COLOR=0x5DFF; UI_LEVEL_COLOR=0x07FE; UI_LABEL_COLOR=0x5DFF; UI_DIM_COLOR=0x018C; UI_BORDER_COLOR=0x07FE; break;
    case 415: UI_HEADER_BG=0x08C3; UI_FREQ_COLOR=0x4634; UI_LEVEL_COLOR=0x240C; UI_LABEL_COLOR=0x4634; UI_DIM_COLOR=0x11C5; UI_BORDER_COLOR=0x240C; break;
    case 416: UI_HEADER_BG=0x0000; UI_FREQ_COLOR=0xC618; UI_LEVEL_COLOR=0xEF7D; UI_LABEL_COLOR=0xC618; UI_DIM_COLOR=0x4208; UI_BORDER_COLOR=0xEF7D; break;
    case 417: UI_HEADER_BG=0x1122; UI_FREQ_COLOR=0x9648; UI_LEVEL_COLOR=0x552A; UI_LABEL_COLOR=0xBE4D; UI_DIM_COLOR=0x10C1; UI_BORDER_COLOR=0x7DA8; break;
    case 418: UI_HEADER_BG=0x2945; UI_FREQ_COLOR=0xFFBA; UI_LEVEL_COLOR=0xE6B2; UI_LABEL_COLOR=0xEEF5; UI_DIM_COLOR=0x18C2; UI_BORDER_COLOR=0xD654; break;
    case 419: UI_HEADER_BG=0x39E7; UI_FREQ_COLOR=0xFFFF; UI_LEVEL_COLOR=0xEF7D; UI_LABEL_COLOR=0xBDF7; UI_DIM_COLOR=0x4A49; UI_BORDER_COLOR=0xCE59; break;
    case 420: UI_HEADER_BG=0x2943; UI_FREQ_COLOR=0xFFBA; UI_LEVEL_COLOR=0xE6B2; UI_LABEL_COLOR=0xEEF5; UI_DIM_COLOR=0x18C2; UI_BORDER_COLOR=0xD654; break;
    case 421: UI_HEADER_BG=0x2945; UI_FREQ_COLOR=0xEF7D; UI_LEVEL_COLOR=0xBDF7; UI_LABEL_COLOR=0xEF7D; UI_DIM_COLOR=0x7BEF; UI_BORDER_COLOR=0x9CF3; break;
    case 422: UI_HEADER_BG=0x3940; UI_FREQ_COLOR=0xFDA0; UI_LEVEL_COLOR=0xFD20; UI_LABEL_COLOR=0xFDA0; UI_DIM_COLOR=0x4A00; UI_BORDER_COLOR=0xC560; break;
    case 423: UI_HEADER_BG=0x1064; UI_FREQ_COLOR=0xCB3B; UI_LEVEL_COLOR=0x66F4; UI_LABEL_COLOR=0x8DBC; UI_DIM_COLOR=0x0842; UI_BORDER_COLOR=0xA419; break;
    case 424: UI_HEADER_BG=0x2945; UI_FREQ_COLOR=0xBEDF; UI_LEVEL_COLOR=0x867D; UI_LABEL_COLOR=0xF7BE; UI_DIM_COLOR=0x18C5; UI_BORDER_COLOR=0x9D5F; break;
    case 425: UI_HEADER_BG=0x39E7; UI_FREQ_COLOR=0xFFFF; UI_LEVEL_COLOR=0xAEFF; UI_LABEL_COLOR=0xFFFF; UI_DIM_COLOR=0x2ACF; UI_BORDER_COLOR=0x87FF; break;
    case 426: UI_HEADER_BG=0x39E7; UI_FREQ_COLOR=0xEF7D; UI_LEVEL_COLOR=0xBDF7; UI_LABEL_COLOR=0xEF7D; UI_DIM_COLOR=0x7BEF; UI_BORDER_COLOR=0x9CF3; break;
    case 427: UI_HEADER_BG=0x0043; UI_FREQ_COLOR=0x45C8; UI_LEVEL_COLOR=0x07FE; UI_LABEL_COLOR=0x45C8; UI_DIM_COLOR=0x0022; UI_BORDER_COLOR=0x45C8; break;
    case 428: UI_HEADER_BG=0x08C3; UI_FREQ_COLOR=0x4634; UI_LEVEL_COLOR=0x240C; UI_LABEL_COLOR=0x4634; UI_DIM_COLOR=0x11C5; UI_BORDER_COLOR=0x240C; break;
    case 429: UI_HEADER_BG=0x3806; UI_FREQ_COLOR=0xFC19; UI_LEVEL_COLOR=0x45C8; UI_LABEL_COLOR=0xFC19; UI_DIM_COLOR=0x610C; UI_BORDER_COLOR=0x45C8; break;
    case 430: UI_HEADER_BG=0x2945; UI_FREQ_COLOR=0xBEDF; UI_LEVEL_COLOR=0x867D; UI_LABEL_COLOR=0xF7BE; UI_DIM_COLOR=0x18C5; UI_BORDER_COLOR=0x9D5F; break;
    case 431: UI_HEADER_BG=0x0010; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0xFFE0; UI_LABEL_COLOR=0xEF7D; UI_DIM_COLOR=0x0006; UI_BORDER_COLOR=0xFDA0; break;
    case 432: UI_HEADER_BG=0x2140; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0xED40; UI_LABEL_COLOR=0xFFE0; UI_DIM_COLOR=0x6B20; UI_BORDER_COLOR=0xED40; break;
    case 433: UI_HEADER_BG=0x0200; UI_FREQ_COLOR=0x07E0; UI_LEVEL_COLOR=0x07E0; UI_LABEL_COLOR=0x4FE0; UI_DIM_COLOR=0x0100; UI_BORDER_COLOR=0x07C0; break;
    case 434: UI_HEADER_BG=0x2843; UI_FREQ_COLOR=0xFC19; UI_LEVEL_COLOR=0x45C8; UI_LABEL_COLOR=0xFC19; UI_DIM_COLOR=0x610C; UI_BORDER_COLOR=0x45C8; break;
    case 435: UI_HEADER_BG=0x0200; UI_FREQ_COLOR=0x07E0; UI_LEVEL_COLOR=0x07E0; UI_LABEL_COLOR=0xCFE0; UI_DIM_COLOR=0x0100; UI_BORDER_COLOR=0x07C0; break;
    case 436: UI_HEADER_BG=0x0043; UI_FREQ_COLOR=0x5DFF; UI_LEVEL_COLOR=0x07FE; UI_LABEL_COLOR=0x5DFF; UI_DIM_COLOR=0x018C; UI_BORDER_COLOR=0x07FE; break;
    case 437: UI_HEADER_BG=0x0200; UI_FREQ_COLOR=0x07E0; UI_LEVEL_COLOR=0x07E0; UI_LABEL_COLOR=0x4FE0; UI_DIM_COLOR=0x0100; UI_BORDER_COLOR=0x07C0; break;
    case 438: UI_HEADER_BG=0x1122; UI_FREQ_COLOR=0x9648; UI_LEVEL_COLOR=0x552A; UI_LABEL_COLOR=0xBE4D; UI_DIM_COLOR=0x10C1; UI_BORDER_COLOR=0x7DA8; break;
    case 449: UI_HEADER_BG=0x1082; UI_FREQ_COLOR=0xFFFF; UI_LEVEL_COLOR=0xEF7D; UI_LABEL_COLOR=0xFFFF; UI_DIM_COLOR=0x2ACF; UI_BORDER_COLOR=0x87FF; break;
    // ── Planets 450-499 (compact): use space/celestial palette
    case 450: case 451: case 452: case 453: case 454: case 455: case 456: case 457: case 458: case 459:
    case 460: case 461: case 462: case 463: case 464: case 465: case 466: case 467: case 468: case 469:
    case 470: case 471: case 472: case 473: case 474: case 475: case 476: case 477: case 478: case 479:
    case 480: case 481: case 482: case 483: case 484: case 485: case 486: case 487: case 488: case 489:
    case 490: case 491: case 492: case 493: case 494: case 495: case 496: case 497: case 498: case 499:
      UI_HEADER_BG=0x0001; UI_FREQ_COLOR=0xFFFF; UI_LEVEL_COLOR=0x6FFF; UI_LABEL_COLOR=0x7FFF; UI_DIM_COLOR=0x0000; UI_BORDER_COLOR=0x05BF; break;
    // ── Animals 500-549 (compact): use wildlife palette
    case 500: case 501: case 502: case 503: case 504: case 505: case 506: case 507: case 508: case 509:
    case 510: case 511: case 512: case 513: case 514: case 515: case 516: case 517: case 518: case 519:
    case 520: case 521: case 522: case 523: case 524: case 525: case 526: case 527: case 528: case 529:
    case 530: case 531: case 532: case 533: case 534: case 535: case 536: case 537: case 538: case 539:
    case 540: case 541: case 542: case 543: case 544: case 545: case 546: case 547: case 548: case 549:
      UI_HEADER_BG=0x2120; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0xF800; UI_LABEL_COLOR=0xEF7D; UI_DIM_COLOR=0x4208; UI_BORDER_COLOR=0x8400; break;
    // ── Trees 550-599 (compact): use forest/wood palette
    case 550: case 551: case 552: case 553: case 554: case 555: case 556: case 557: case 558: case 559:
    case 560: case 561: case 562: case 563: case 564: case 565: case 566: case 567: case 568: case 569:
    case 570: case 571: case 572: case 573: case 574: case 575: case 576: case 577: case 578: case 579:
    case 580: case 581: case 582: case 583: case 584: case 585: case 586: case 587: case 588: case 589:
    case 590: case 591: case 592: case 593: case 594: case 595: case 596: case 597: case 598: case 599:
      UI_HEADER_BG=0x0200; UI_FREQ_COLOR=0x07E0; UI_LEVEL_COLOR=0xBD00; UI_LABEL_COLOR=0xFDA0; UI_DIM_COLOR=0x0100; UI_BORDER_COLOR=0xAC00; break;
    // ── Spices 600-649 (compact): use warm spice palette
    case 600: case 601: case 602: case 603: case 604: case 605: case 606: case 607: case 608: case 609:
    case 610: case 611: case 612: case 613: case 614: case 615: case 616: case 617: case 618: case 619:
    case 620: case 621: case 622: case 623: case 624: case 625: case 626: case 627: case 628: case 629:
    case 630: case 631: case 632: case 633: case 634: case 635: case 636: case 637: case 638: case 639:
    case 640: case 641: case 642: case 643: case 644: case 645: case 646: case 647: case 648: case 649:
      UI_HEADER_BG=0x4940; UI_FREQ_COLOR=0xFDA0; UI_LEVEL_COLOR=0xFD20; UI_LABEL_COLOR=0xFFBA; UI_DIM_COLOR=0x2940; UI_BORDER_COLOR=0xAD20; break;
    // ── Dances 650-699 (compact): use festive palette
    case 650: case 651: case 652: case 653: case 654: case 655: case 656: case 657: case 658: case 659:
    case 660: case 661: case 662: case 663: case 664: case 665: case 666: case 667: case 668: case 669:
    case 670: case 671: case 672: case 673: case 674: case 675: case 676: case 677: case 678: case 679:
    case 680: case 681: case 682: case 683: case 684: case 685: case 686: case 687: case 688: case 689:
    case 690: case 691: case 692: case 693: case 694: case 695: case 696: case 697: case 698: case 699:
      UI_HEADER_BG=0x3008; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0xFC60; UI_LABEL_COLOR=0xF81F; UI_DIM_COLOR=0x6006; UI_BORDER_COLOR=0xFB20; break;
    // ── Poets 700-749 (compact): use refined palette
    case 700: case 701: case 702: case 703: case 704: case 705: case 706: case 707: case 708: case 709:
    case 710: case 711: case 712: case 713: case 714: case 715: case 716: case 717: case 718: case 719:
    case 720: case 721: case 722: case 723: case 724: case 725: case 726: case 727: case 728: case 729:
    case 730: case 731: case 732: case 733: case 734: case 735: case 736: case 737: case 738: case 739:
    case 740: case 741: case 742: case 743: case 744: case 745: case 746: case 747: case 748: case 749:
      UI_HEADER_BG=0x2945; UI_FREQ_COLOR=0xFFD6; UI_LEVEL_COLOR=0xCD40; UI_LABEL_COLOR=0xFEB2; UI_DIM_COLOR=0x18C0; UI_BORDER_COLOR=0xBD00; break;
    // ── Cocktails 750-799 (compact): use drink palette
    case 750: case 751: case 752: case 753: case 754: case 755: case 756: case 757: case 758: case 759:
    case 760: case 761: case 762: case 763: case 764: case 765: case 766: case 767: case 768: case 769:
    case 770: case 771: case 772: case 773: case 774: case 775: case 776: case 777: case 778: case 779:
    case 780: case 781: case 782: case 783: case 784: case 785: case 786: case 787: case 788: case 789:
    case 790: case 791: case 792: case 793: case 794: case 795: case 796: case 797: case 798: case 799:
      UI_HEADER_BG=0x0200; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0xFCA0; UI_LABEL_COLOR=0xFFF5; UI_DIM_COLOR=0x0100; UI_BORDER_COLOR=0xFD20; break;
    // ── States 800-849 (compact): use regional palette
    case 800: case 801: case 802: case 803: case 804: case 805: case 806: case 807: case 808: case 809:
    case 810: case 811: case 812: case 813: case 814: case 815: case 816: case 817: case 818: case 819:
    case 820: case 821: case 822: case 823: case 824: case 825: case 826: case 827: case 828: case 829:
    case 830: case 831: case 832: case 833: case 834: case 835: case 836: case 837: case 838: case 839:
    case 840: case 841: case 842: case 843: case 844: case 845: case 846: case 847: case 848: case 849:
      UI_HEADER_BG=0x0010; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0x07C0; UI_LABEL_COLOR=0xFFFF; UI_DIM_COLOR=0x0006; UI_BORDER_COLOR=0xED40; break;
    // ── Architecture 850-899 (compact): use refined palette
    case 850: case 851: case 852: case 853: case 854: case 855: case 856: case 857: case 858: case 859:
    case 860: case 861: case 862: case 863: case 864: case 865: case 866: case 867: case 868: case 869:
    case 870: case 871: case 872: case 873: case 874: case 875: case 876: case 877: case 878: case 879:
    case 880: case 881: case 882: case 883: case 884: case 885: case 886: case 887: case 888: case 889:
    case 890: case 891: case 892: case 893: case 894: case 895: case 896: case 897: case 898: case 899:
      UI_HEADER_BG=0x2104; UI_FREQ_COLOR=0xEF7D; UI_LEVEL_COLOR=0xC618; UI_LABEL_COLOR=0xAD75; UI_DIM_COLOR=0x4208; UI_BORDER_COLOR=0x8410; break;
    // ── Textiles 900-949 (compact): use soft palette
    case 900: case 901: case 902: case 903: case 904: case 905: case 906: case 907: case 908: case 909:
    case 910: case 911: case 912: case 913: case 914: case 915: case 916: case 917: case 918: case 919:
    case 920: case 921: case 922: case 923: case 924: case 925: case 926: case 927: case 928: case 929:
    case 930: case 931: case 932: case 933: case 934: case 935: case 936: case 937: case 938: case 939:
    case 940: case 941: case 942: case 943: case 944: case 945: case 946: case 947: case 948: case 949:
      UI_HEADER_BG=0x39E7; UI_FREQ_COLOR=0xFFFF; UI_LEVEL_COLOR=0xBDF7; UI_LABEL_COLOR=0xEF7D; UI_DIM_COLOR=0x4A49; UI_BORDER_COLOR=0xCE59; break;
    // ── Mythology 950-999 (compact): use mystical palette
    case 950: case 951: case 952: case 953: case 954: case 955: case 956: case 957: case 958: case 959:
    case 960: case 961: case 962: case 963: case 964: case 965: case 966: case 967: case 968: case 969:
    case 970: case 971: case 972: case 973: case 974: case 975: case 976: case 977: case 978: case 979:
    case 980: case 981: case 982: case 983: case 984: case 985: case 986: case 987: case 988: case 989:
    case 990: case 991: case 992: case 993: case 994: case 995: case 996: case 997: case 998: case 999:
      UI_HEADER_BG=0x2006; UI_FREQ_COLOR=0xE0DF; UI_LEVEL_COLOR=0xC31F; UI_LABEL_COLOR=0xB3DE; UI_DIM_COLOR=0x0020; UI_BORDER_COLOR=0x1E56; break;
    // ── Japanese Cuisine 1000-1009: sushi/food palette
    case 1000: case 1001: case 1002: case 1003: case 1004: case 1005: case 1006: case 1007: case 1008: case 1009:
      UI_HEADER_BG=0x0180; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0x07E0; UI_LABEL_COLOR=0xFDA0; UI_DIM_COLOR=0x00C0; UI_BORDER_COLOR=0xED40; break;
    // ── Wine 1010-1019: wine palette
    case 1010: case 1011: case 1012: case 1013: case 1014: case 1015: case 1016: case 1017: case 1018: case 1019:
      UI_HEADER_BG=0x3800; UI_FREQ_COLOR=0xFD40; UI_LEVEL_COLOR=0xF800; UI_LABEL_COLOR=0xFFBA; UI_DIM_COLOR=0x2800; UI_BORDER_COLOR=0xC560; break;
    // ── Coffee & Tea 1020-1029: warm palette
    case 1020: case 1021: case 1022: case 1023: case 1024: case 1025: case 1026: case 1027: case 1028: case 1029:
      UI_HEADER_BG=0x2820; UI_FREQ_COLOR=0xFFBA; UI_LEVEL_COLOR=0xC560; UI_LABEL_COLOR=0xFEB2; UI_DIM_COLOR=0x1000; UI_BORDER_COLOR=0x9560; break;
    // ── Rare Flowers 1030-1039: exotic palette
    case 1030: case 1031: case 1032: case 1033: case 1034: case 1035: case 1036: case 1037: case 1038: case 1039:
      UI_HEADER_BG=0x2006; UI_FREQ_COLOR=0xFC19; UI_LEVEL_COLOR=0x45C8; UI_LABEL_COLOR=0xFC19; UI_DIM_COLOR=0x500D; UI_BORDER_COLOR=0x45C8; break;
    // ── Clouds & Sky 1040-1049: sky palette
    case 1040: case 1041: case 1042: case 1043: case 1044: case 1045: case 1046: case 1047: case 1048: case 1049:
      UI_HEADER_BG=0x0043; UI_FREQ_COLOR=0xFFFF; UI_LEVEL_COLOR=0x6FFF; UI_LABEL_COLOR=0xBFFF; UI_DIM_COLOR=0x0021; UI_BORDER_COLOR=0x07BF; break;
    // ── Precious Metals 1050-1059: luxury palette
    case 1050: case 1051: case 1052: case 1053: case 1054: case 1055: case 1056: case 1057: case 1058: case 1059:
      UI_HEADER_BG=0x0000; UI_FREQ_COLOR=0xFDA0; UI_LEVEL_COLOR=0xC618; UI_LABEL_COLOR=0xEF7D; UI_DIM_COLOR=0x2104; UI_BORDER_COLOR=0xB800; break;
    // ── Glass & Crystal 1060-1069: bright palette
    case 1060: case 1061: case 1062: case 1063: case 1064: case 1065: case 1066: case 1067: case 1068: case 1069:
      UI_HEADER_BG=0x0008; UI_FREQ_COLOR=0xFFFF; UI_LEVEL_COLOR=0x6FFF; UI_LABEL_COLOR=0xBDF7; UI_DIM_COLOR=0x0004; UI_BORDER_COLOR=0x87FF; break;
    // ── Perfumes 1070-1079: floral palette
    case 1070: case 1071: case 1072: case 1073: case 1074: case 1075: case 1076: case 1077: case 1078: case 1079:
      UI_HEADER_BG=0x3806; UI_FREQ_COLOR=0xFC19; UI_LEVEL_COLOR=0xF81F; UI_LABEL_COLOR=0xFBB5; UI_DIM_COLOR=0x500D; UI_BORDER_COLOR=0xF81A; break;
    // ── Japanese Silks 1080-1089: silk palette
    case 1080: case 1081: case 1082: case 1083: case 1084: case 1085: case 1086: case 1087: case 1088: case 1089:
      UI_HEADER_BG=0x2945; UI_FREQ_COLOR=0xBEDF; UI_LEVEL_COLOR=0x867D; UI_LABEL_COLOR=0xF7BE; UI_DIM_COLOR=0x18C5; UI_BORDER_COLOR=0x9D5F; break;
    // ── Jewelry & Piercing 1090-1099: precious palette
    case 1090: case 1091: case 1092: case 1093: case 1094: case 1095: case 1096: case 1097: case 1098: case 1099:
      UI_HEADER_BG=0x0841; UI_FREQ_COLOR=0xFFFF; UI_LEVEL_COLOR=0xAEBF; UI_LABEL_COLOR=0xFFFF; UI_DIM_COLOR=0x2ACF; UI_BORDER_COLOR=0x87FF; break;
    // ── Constellations 1100-1199: night sky palette
    case 1100: case 1101: case 1102: case 1103: case 1104: case 1105: case 1106: case 1107: case 1108: case 1109:
    case 1110: case 1111: case 1112: case 1113: case 1114: case 1115: case 1116: case 1117: case 1118: case 1119:
    case 1120: case 1121: case 1122: case 1123: case 1124: case 1125: case 1126: case 1127: case 1128: case 1129:
    case 1130: case 1131: case 1132: case 1133: case 1134: case 1135: case 1136: case 1137: case 1138: case 1139:
    case 1140: case 1141: case 1142: case 1143: case 1144: case 1145: case 1146: case 1147: case 1148: case 1149:
    case 1150: case 1151: case 1152: case 1153: case 1154: case 1155: case 1156: case 1157: case 1158: case 1159:
    case 1160: case 1161: case 1162: case 1163: case 1164: case 1165: case 1166: case 1167: case 1168: case 1169:
    case 1170: case 1171: case 1172: case 1173: case 1174: case 1175: case 1176: case 1177: case 1178: case 1179:
    case 1180: case 1181: case 1182: case 1183: case 1184: case 1185: case 1186: case 1187: case 1188: case 1189:
    case 1190: case 1191: case 1192: case 1193: case 1194: case 1195: case 1196: case 1197: case 1198: case 1199:
      UI_HEADER_BG=0x0004; UI_FREQ_COLOR=0xFFFF; UI_LEVEL_COLOR=0x6FFF; UI_LABEL_COLOR=0x7FFF; UI_DIM_COLOR=0x0000; UI_BORDER_COLOR=0x05BF; break;
    // ── Philosophers 1200-1299: refined palette
    case 1200: case 1201: case 1202: case 1203: case 1204: case 1205: case 1206: case 1207: case 1208: case 1209:
    case 1210: case 1211: case 1212: case 1213: case 1214: case 1215: case 1216: case 1217: case 1218: case 1219:
    case 1220: case 1221: case 1222: case 1223: case 1224: case 1225: case 1226: case 1227: case 1228: case 1229:
    case 1230: case 1231: case 1232: case 1233: case 1234: case 1235: case 1236: case 1237: case 1238: case 1239:
    case 1240: case 1241: case 1242: case 1243: case 1244: case 1245: case 1246: case 1247: case 1248: case 1249:
    case 1250: case 1251: case 1252: case 1253: case 1254: case 1255: case 1256: case 1257: case 1258: case 1259:
    case 1260: case 1261: case 1262: case 1263: case 1264: case 1265: case 1266: case 1267: case 1268: case 1269:
    case 1270: case 1271: case 1272: case 1273: case 1274: case 1275: case 1276: case 1277: case 1278: case 1279:
    case 1280: case 1281: case 1282: case 1283: case 1284: case 1285: case 1286: case 1287: case 1288: case 1289:
    case 1290: case 1291: case 1292: case 1293: case 1294: case 1295: case 1296: case 1297: case 1298: case 1299:
      UI_HEADER_BG=0x2945; UI_FREQ_COLOR=0xFFD6; UI_LEVEL_COLOR=0xCD40; UI_LABEL_COLOR=0xFEB2; UI_DIM_COLOR=0x18C0; UI_BORDER_COLOR=0xBD00; break;
    // ── Composers 1300-1399: musical palette
    case 1300: case 1301: case 1302: case 1303: case 1304: case 1305: case 1306: case 1307: case 1308: case 1309:
    case 1310: case 1311: case 1312: case 1313: case 1314: case 1315: case 1316: case 1317: case 1318: case 1319:
    case 1320: case 1321: case 1322: case 1323: case 1324: case 1325: case 1326: case 1327: case 1328: case 1329:
    case 1330: case 1331: case 1332: case 1333: case 1334: case 1335: case 1336: case 1337: case 1338: case 1339:
    case 1340: case 1341: case 1342: case 1343: case 1344: case 1345: case 1346: case 1347: case 1348: case 1349:
    case 1350: case 1351: case 1352: case 1353: case 1354: case 1355: case 1356: case 1357: case 1358: case 1359:
    case 1360: case 1361: case 1362: case 1363: case 1364: case 1365: case 1366: case 1367: case 1368: case 1369:
    case 1370: case 1371: case 1372: case 1373: case 1374: case 1375: case 1376: case 1377: case 1378: case 1379:
    case 1380: case 1381: case 1382: case 1383: case 1384: case 1385: case 1386: case 1387: case 1388: case 1389:
    case 1390: case 1391: case 1392: case 1393: case 1394: case 1395: case 1396: case 1397: case 1398: case 1399:
      UI_HEADER_BG=0x1082; UI_FREQ_COLOR=0xEF7D; UI_LEVEL_COLOR=0xBDF7; UI_LABEL_COLOR=0xEF7D; UI_DIM_COLOR=0x39E7; UI_BORDER_COLOR=0xCE59; break;
    // ── Painters 1400-1499: artistic palette
    case 1400: case 1401: case 1402: case 1403: case 1404: case 1405: case 1406: case 1407: case 1408: case 1409:
    case 1410: case 1411: case 1412: case 1413: case 1414: case 1415: case 1416: case 1417: case 1418: case 1419:
    case 1420: case 1421: case 1422: case 1423: case 1424: case 1425: case 1426: case 1427: case 1428: case 1429:
    case 1430: case 1431: case 1432: case 1433: case 1434: case 1435: case 1436: case 1437: case 1438: case 1439:
    case 1440: case 1441: case 1442: case 1443: case 1444: case 1445: case 1446: case 1447: case 1448: case 1449:
    case 1450: case 1451: case 1452: case 1453: case 1454: case 1455: case 1456: case 1457: case 1458: case 1459:
    case 1460: case 1461: case 1462: case 1463: case 1464: case 1465: case 1466: case 1467: case 1468: case 1469:
    case 1470: case 1471: case 1472: case 1473: case 1474: case 1475: case 1476: case 1477: case 1478: case 1479:
    case 1480: case 1481: case 1482: case 1483: case 1484: case 1485: case 1486: case 1487: case 1488: case 1489:
    case 1490: case 1491: case 1492: case 1493: case 1494: case 1495: case 1496: case 1497: case 1498: case 1499:
      UI_HEADER_BG=0x39E7; UI_FREQ_COLOR=0xFFFF; UI_LEVEL_COLOR=0xBDF7; UI_LABEL_COLOR=0xEF7D; UI_DIM_COLOR=0x4A49; UI_BORDER_COLOR=0xCE59; break;
    // ── Historic Cities 1500-1599: heritage palette
    case 1500: case 1501: case 1502: case 1503: case 1504: case 1505: case 1506: case 1507: case 1508: case 1509:
    case 1510: case 1511: case 1512: case 1513: case 1514: case 1515: case 1516: case 1517: case 1518: case 1519:
    case 1520: case 1521: case 1522: case 1523: case 1524: case 1525: case 1526: case 1527: case 1528: case 1529:
    case 1530: case 1531: case 1532: case 1533: case 1534: case 1535: case 1536: case 1537: case 1538: case 1539:
    case 1540: case 1541: case 1542: case 1543: case 1544: case 1545: case 1546: case 1547: case 1548: case 1549:
    case 1550: case 1551: case 1552: case 1553: case 1554: case 1555: case 1556: case 1557: case 1558: case 1559:
    case 1560: case 1561: case 1562: case 1563: case 1564: case 1565: case 1566: case 1567: case 1568: case 1569:
    case 1570: case 1571: case 1572: case 1573: case 1574: case 1575: case 1576: case 1577: case 1578: case 1579:
    case 1580: case 1581: case 1582: case 1583: case 1584: case 1585: case 1586: case 1587: case 1588: case 1589:
    case 1590: case 1591: case 1592: case 1593: case 1594: case 1595: case 1596: case 1597: case 1598: case 1599:
      UI_HEADER_BG=0x2820; UI_FREQ_COLOR=0xC560; UI_LEVEL_COLOR=0x8400; UI_LABEL_COLOR=0xFDA0; UI_DIM_COLOR=0x2000; UI_BORDER_COLOR=0x8420; break;
    // ── Volcanoes 1600-1699: fire palette
    case 1600: case 1601: case 1602: case 1603: case 1604: case 1605: case 1606: case 1607: case 1608: case 1609:
    case 1610: case 1611: case 1612: case 1613: case 1614: case 1615: case 1616: case 1617: case 1618: case 1619:
    case 1620: case 1621: case 1622: case 1623: case 1624: case 1625: case 1626: case 1627: case 1628: case 1629:
    case 1630: case 1631: case 1632: case 1633: case 1634: case 1635: case 1636: case 1637: case 1638: case 1639:
    case 1640: case 1641: case 1642: case 1643: case 1644: case 1645: case 1646: case 1647: case 1648: case 1649:
    case 1650: case 1651: case 1652: case 1653: case 1654: case 1655: case 1656: case 1657: case 1658: case 1659:
    case 1660: case 1661: case 1662: case 1663: case 1664: case 1665: case 1666: case 1667: case 1668: case 1669:
    case 1670: case 1671: case 1672: case 1673: case 1674: case 1675: case 1676: case 1677: case 1678: case 1679:
    case 1680: case 1681: case 1682: case 1683: case 1684: case 1685: case 1686: case 1687: case 1688: case 1689:
    case 1690: case 1691: case 1692: case 1693: case 1694: case 1695: case 1696: case 1697: case 1698: case 1699:
      UI_HEADER_BG=0x4800; UI_FREQ_COLOR=0xFF20; UI_LEVEL_COLOR=0xF800; UI_LABEL_COLOR=0xFCA0; UI_DIM_COLOR=0x3000; UI_BORDER_COLOR=0xFC00; break;
    // ── Deserts 1700-1799: sand palette
    case 1700: case 1701: case 1702: case 1703: case 1704: case 1705: case 1706: case 1707: case 1708: case 1709:
    case 1710: case 1711: case 1712: case 1713: case 1714: case 1715: case 1716: case 1717: case 1718: case 1719:
    case 1720: case 1721: case 1722: case 1723: case 1724: case 1725: case 1726: case 1727: case 1728: case 1729:
    case 1730: case 1731: case 1732: case 1733: case 1734: case 1735: case 1736: case 1737: case 1738: case 1739:
    case 1740: case 1741: case 1742: case 1743: case 1744: case 1745: case 1746: case 1747: case 1748: case 1749:
    case 1750: case 1751: case 1752: case 1753: case 1754: case 1755: case 1756: case 1757: case 1758: case 1759:
    case 1760: case 1761: case 1762: case 1763: case 1764: case 1765: case 1766: case 1767: case 1768: case 1769:
    case 1770: case 1771: case 1772: case 1773: case 1774: case 1775: case 1776: case 1777: case 1778: case 1779:
    case 1780: case 1781: case 1782: case 1783: case 1784: case 1785: case 1786: case 1787: case 1788: case 1789:
    case 1790: case 1791: case 1792: case 1793: case 1794: case 1795: case 1796: case 1797: case 1798: case 1799:
      UI_HEADER_BG=0x4940; UI_FREQ_COLOR=0xFFCA; UI_LEVEL_COLOR=0xFD40; UI_LABEL_COLOR=0xF6D3; UI_DIM_COLOR=0x6900; UI_BORDER_COLOR=0xBD40; break;
    // ── Waterfalls 1800-1899: water palette
    case 1800: case 1801: case 1802: case 1803: case 1804: case 1805: case 1806: case 1807: case 1808: case 1809:
    case 1810: case 1811: case 1812: case 1813: case 1814: case 1815: case 1816: case 1817: case 1818: case 1819:
    case 1820: case 1821: case 1822: case 1823: case 1824: case 1825: case 1826: case 1827: case 1828: case 1829:
    case 1830: case 1831: case 1832: case 1833: case 1834: case 1835: case 1836: case 1837: case 1838: case 1839:
    case 1840: case 1841: case 1842: case 1843: case 1844: case 1845: case 1846: case 1847: case 1848: case 1849:
    case 1850: case 1851: case 1852: case 1853: case 1854: case 1855: case 1856: case 1857: case 1858: case 1859:
    case 1860: case 1861: case 1862: case 1863: case 1864: case 1865: case 1866: case 1867: case 1868: case 1869:
    case 1870: case 1871: case 1872: case 1873: case 1874: case 1875: case 1876: case 1877: case 1878: case 1879:
    case 1880: case 1881: case 1882: case 1883: case 1884: case 1885: case 1886: case 1887: case 1888: case 1889:
    case 1890: case 1891: case 1892: case 1893: case 1894: case 1895: case 1896: case 1897: case 1898: case 1899:
      UI_HEADER_BG=0x0010; UI_FREQ_COLOR=0x07FE; UI_LEVEL_COLOR=0xFFFF; UI_LABEL_COLOR=0x7FFF; UI_DIM_COLOR=0x0006; UI_BORDER_COLOR=0x07C0; break;
    // ── Caves 1900-1999: dark palette
    case 1900: case 1901: case 1902: case 1903: case 1904: case 1905: case 1906: case 1907: case 1908: case 1909:
    case 1910: case 1911: case 1912: case 1913: case 1914: case 1915: case 1916: case 1917: case 1918: case 1919:
    case 1920: case 1921: case 1922: case 1923: case 1924: case 1925: case 1926: case 1927: case 1928: case 1929:
    case 1930: case 1931: case 1932: case 1933: case 1934: case 1935: case 1936: case 1937: case 1938: case 1939:
    case 1940: case 1941: case 1942: case 1943: case 1944: case 1945: case 1946: case 1947: case 1948: case 1949:
    case 1950: case 1951: case 1952: case 1953: case 1954: case 1955: case 1956: case 1957: case 1958: case 1959:
    case 1960: case 1961: case 1962: case 1963: case 1964: case 1965: case 1966: case 1967: case 1968: case 1969:
    case 1970: case 1971: case 1972: case 1973: case 1974: case 1975: case 1976: case 1977: case 1978: case 1979:
    case 1980: case 1981: case 1982: case 1983: case 1984: case 1985: case 1986: case 1987: case 1988: case 1989:
    case 1990: case 1991: case 1992: case 1993: case 1994: case 1995: case 1996: case 1997: case 1998: case 1999:
      UI_HEADER_BG=0x0000; UI_FREQ_COLOR=0xC618; UI_LEVEL_COLOR=0x867D; UI_LABEL_COLOR=0x867D; UI_DIM_COLOR=0x2104; UI_BORDER_COLOR=0x4208; break;
    // ── Mountains 2000-2099: peak palette
    case 2000: case 2001: case 2002: case 2003: case 2004: case 2005: case 2006: case 2007: case 2008: case 2009:
    case 2010: case 2011: case 2012: case 2013: case 2014: case 2015: case 2016: case 2017: case 2018: case 2019:
    case 2020: case 2021: case 2022: case 2023: case 2024: case 2025: case 2026: case 2027: case 2028: case 2029:
    case 2030: case 2031: case 2032: case 2033: case 2034: case 2035: case 2036: case 2037: case 2038: case 2039:
    case 2040: case 2041: case 2042: case 2043: case 2044: case 2045: case 2046: case 2047: case 2048: case 2049:
    case 2050: case 2051: case 2052: case 2053: case 2054: case 2055: case 2056: case 2057: case 2058: case 2059:
    case 2060: case 2061: case 2062: case 2063: case 2064: case 2065: case 2066: case 2067: case 2068: case 2069:
    case 2070: case 2071: case 2072: case 2073: case 2074: case 2075: case 2076: case 2077: case 2078: case 2079:
    case 2080: case 2081: case 2082: case 2083: case 2084: case 2085: case 2086: case 2087: case 2088: case 2089:
    case 2090: case 2091: case 2092: case 2093: case 2094: case 2095: case 2096: case 2097: case 2098: case 2099:
      UI_HEADER_BG=0x1043; UI_FREQ_COLOR=0xBDF7; UI_LEVEL_COLOR=0xEF7D; UI_LABEL_COLOR=0xBDF7; UI_DIM_COLOR=0x39E7; UI_BORDER_COLOR=0xAD75; break;
    // ── Dinosaurs 2100-2199: prehistoric palette
    case 2100: case 2101: case 2102: case 2103: case 2104: case 2105: case 2106: case 2107: case 2108: case 2109:
    case 2110: case 2111: case 2112: case 2113: case 2114: case 2115: case 2116: case 2117: case 2118: case 2119:
    case 2120: case 2121: case 2122: case 2123: case 2124: case 2125: case 2126: case 2127: case 2128: case 2129:
    case 2130: case 2131: case 2132: case 2133: case 2134: case 2135: case 2136: case 2137: case 2138: case 2139:
    case 2140: case 2141: case 2142: case 2143: case 2144: case 2145: case 2146: case 2147: case 2148: case 2149:
    case 2150: case 2151: case 2152: case 2153: case 2154: case 2155: case 2156: case 2157: case 2158: case 2159:
    case 2160: case 2161: case 2162: case 2163: case 2164: case 2165: case 2166: case 2167: case 2168: case 2169:
    case 2170: case 2171: case 2172: case 2173: case 2174: case 2175: case 2176: case 2177: case 2178: case 2179:
    case 2180: case 2181: case 2182: case 2183: case 2184: case 2185: case 2186: case 2187: case 2188: case 2189:
    case 2190: case 2191: case 2192: case 2193: case 2194: case 2195: case 2196: case 2197: case 2198: case 2199:
      UI_HEADER_BG=0x3940; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0xF800; UI_LABEL_COLOR=0xFCA0; UI_DIM_COLOR=0x4A00; UI_BORDER_COLOR=0xB800; break;
    // ── Insects 2200-2299: nature palette
    case 2200: case 2201: case 2202: case 2203: case 2204: case 2205: case 2206: case 2207: case 2208: case 2209:
    case 2210: case 2211: case 2212: case 2213: case 2214: case 2215: case 2216: case 2217: case 2218: case 2219:
    case 2220: case 2221: case 2222: case 2223: case 2224: case 2225: case 2226: case 2227: case 2228: case 2229:
    case 2230: case 2231: case 2232: case 2233: case 2234: case 2235: case 2236: case 2237: case 2238: case 2239:
    case 2240: case 2241: case 2242: case 2243: case 2244: case 2245: case 2246: case 2247: case 2248: case 2249:
    case 2250: case 2251: case 2252: case 2253: case 2254: case 2255: case 2256: case 2257: case 2258: case 2259:
    case 2260: case 2261: case 2262: case 2263: case 2264: case 2265: case 2266: case 2267: case 2268: case 2269:
    case 2270: case 2271: case 2272: case 2273: case 2274: case 2275: case 2276: case 2277: case 2278: case 2279:
    case 2280: case 2281: case 2282: case 2283: case 2284: case 2285: case 2286: case 2287: case 2288: case 2289:
    case 2290: case 2291: case 2292: case 2293: case 2294: case 2295: case 2296: case 2297: case 2298: case 2299:
      UI_HEADER_BG=0x0180; UI_FREQ_COLOR=0x07E0; UI_LEVEL_COLOR=0xFFE0; UI_LABEL_COLOR=0xFDA0; UI_DIM_COLOR=0x00C0; UI_BORDER_COLOR=0xED40; break;
    // ── Birds 2300-2399: sky palette
    case 2300: case 2301: case 2302: case 2303: case 2304: case 2305: case 2306: case 2307: case 2308: case 2309:
    case 2310: case 2311: case 2312: case 2313: case 2314: case 2315: case 2316: case 2317: case 2318: case 2319:
    case 2320: case 2321: case 2322: case 2323: case 2324: case 2325: case 2326: case 2327: case 2328: case 2329:
    case 2330: case 2331: case 2332: case 2333: case 2334: case 2335: case 2336: case 2337: case 2338: case 2339:
    case 2340: case 2341: case 2342: case 2343: case 2344: case 2345: case 2346: case 2347: case 2348: case 2349:
    case 2350: case 2351: case 2352: case 2353: case 2354: case 2355: case 2356: case 2357: case 2358: case 2359:
    case 2360: case 2361: case 2362: case 2363: case 2364: case 2365: case 2366: case 2367: case 2368: case 2369:
    case 2370: case 2371: case 2372: case 2373: case 2374: case 2375: case 2376: case 2377: case 2378: case 2379:
    case 2380: case 2381: case 2382: case 2383: case 2384: case 2385: case 2386: case 2387: case 2388: case 2389:
    case 2390: case 2391: case 2392: case 2393: case 2394: case 2395: case 2396: case 2397: case 2398: case 2399:
      UI_HEADER_BG=0x0043; UI_FREQ_COLOR=0x07FE; UI_LEVEL_COLOR=0xFFFF; UI_LABEL_COLOR=0x7FFF; UI_DIM_COLOR=0x0021; UI_BORDER_COLOR=0x07C0; break;
    // ── Fish 2400-2499: water palette
    case 2400: case 2401: case 2402: case 2403: case 2404: case 2405: case 2406: case 2407: case 2408: case 2409:
    case 2410: case 2411: case 2412: case 2413: case 2414: case 2415: case 2416: case 2417: case 2418: case 2419:
    case 2420: case 2421: case 2422: case 2423: case 2424: case 2425: case 2426: case 2427: case 2428: case 2429:
    case 2430: case 2431: case 2432: case 2433: case 2434: case 2435: case 2436: case 2437: case 2438: case 2439:
    case 2440: case 2441: case 2442: case 2443: case 2444: case 2445: case 2446: case 2447: case 2448: case 2449:
    case 2450: case 2451: case 2452: case 2453: case 2454: case 2455: case 2456: case 2457: case 2458: case 2459:
    case 2460: case 2461: case 2462: case 2463: case 2464: case 2465: case 2466: case 2467: case 2468: case 2469:
    case 2470: case 2471: case 2472: case 2473: case 2474: case 2475: case 2476: case 2477: case 2478: case 2479:
    case 2480: case 2481: case 2482: case 2483: case 2484: case 2485: case 2486: case 2487: case 2488: case 2489:
    case 2490: case 2491: case 2492: case 2493: case 2494: case 2495: case 2496: case 2497: case 2498: case 2499:
      UI_HEADER_BG=0x0008; UI_FREQ_COLOR=0x07FF; UI_LEVEL_COLOR=0xFFFF; UI_LABEL_COLOR=0x6FFF; UI_DIM_COLOR=0x0004; UI_BORDER_COLOR=0x05FF; break;
    // ── Mammals 2500-2599: wildlife palette
    case 2500: case 2501: case 2502: case 2503: case 2504: case 2505: case 2506: case 2507: case 2508: case 2509:
    case 2510: case 2511: case 2512: case 2513: case 2514: case 2515: case 2516: case 2517: case 2518: case 2519:
    case 2520: case 2521: case 2522: case 2523: case 2524: case 2525: case 2526: case 2527: case 2528: case 2529:
    case 2530: case 2531: case 2532: case 2533: case 2534: case 2535: case 2536: case 2537: case 2538: case 2539:
    case 2540: case 2541: case 2542: case 2543: case 2544: case 2545: case 2546: case 2547: case 2548: case 2549:
    case 2550: case 2551: case 2552: case 2553: case 2554: case 2555: case 2556: case 2557: case 2558: case 2559:
    case 2560: case 2561: case 2562: case 2563: case 2564: case 2565: case 2566: case 2567: case 2568: case 2569:
    case 2570: case 2571: case 2572: case 2573: case 2574: case 2575: case 2576: case 2577: case 2578: case 2579:
    case 2580: case 2581: case 2582: case 2583: case 2584: case 2585: case 2586: case 2587: case 2588: case 2589:
    case 2590: case 2591: case 2592: case 2593: case 2594: case 2595: case 2596: case 2597: case 2598: case 2599:
      UI_HEADER_BG=0x2120; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0xF800; UI_LABEL_COLOR=0xEF7D; UI_DIM_COLOR=0x4208; UI_BORDER_COLOR=0x8400; break;
    case 2600: // AzulAmarelo — deep blue / yellow / gold / light blue
      UI_HEADER_BG=0x0008; UI_FREQ_COLOR=0xFFE0; UI_LEVEL_COLOR=0xFD20; UI_LABEL_COLOR=0xADD5; UI_DIM_COLOR=0x0004; UI_BORDER_COLOR=0xED40; break;
  }
}

void BuildDisplay() {
  tft.fillScreen(TFT_BLACK);

  // ── Header strip (dark navy, compact 23px) ───────────────
  tft.fillRect(0, 0, 320, 23, UI_HEADER_BG);

  // ── Outer border (rounded) + zone dividers ───────────────
  tft.drawRoundRect(0, 0, 319, 239, 3, UI_BORDER_COLOR);
  tft.drawLine(0, 23, 320, 23, UI_BORDER_COLOR);
  tft.drawLine(0, 93, 320, 93, UI_BORDER_COLOR);
  tft.drawLine(64, 23, 64, 0, UI_BORDER_COLOR);
  tft.drawLine(210, 93, 210, 180, UI_BORDER_COLOR);
  tft.drawLine(268, 23, 268, 0, UI_BORDER_COLOR);
  tft.drawLine(0, 158, 210, 158, UI_BORDER_COLOR);
  tft.drawLine(0, 180, 320, 180, UI_BORDER_COLOR);
  tft.drawLine(0, 211, 320, 211, UI_BORDER_COLOR);
  tft.drawLine(108, 23, 108, 0, UI_BORDER_COLOR);
  tft.drawLine(174, 23, 174, 0, UI_BORDER_COLOR);
  tft.drawLine(20, 113, 200, 113, TFT_DARKGREY);
  tft.drawLine(20, 143, 200, 143, TFT_DARKGREY);

  // ── S-meter gradient tick marks (1px below bar) ──────────
  for (uint16_t segments = 0; segments < 94; segments++) {
    if (segments > 54) {
      if (((segments - 53) % 10) == 0)
        tft.fillRect(16 + (2 * segments), 110, 2, 5, TFT_RED);
    } else {
      if (((segments + 1) % 6) == 0)
        tft.fillRect(16 + (2 * segments), 110, 2, 5, TFT_GREEN);
    }
  }

  // ── Meter labels (dimmed) ────────────────────────────────
  tft.setTextColor(UI_DIM_COLOR);
  tft.drawString("SQ:", 216, 148, 2);
  tft.drawString("S/N", 250, 161, 2);
  tft.drawString("dB",  300, 161, 2);
  tft.drawString("%", 196, 146, 1);
  tft.drawString("1", 24, 116, 1);
  tft.drawString("3", 48, 116, 1);
  tft.drawString("5", 72, 116, 1);
  tft.drawString("7", 96, 116, 1);
  tft.drawString("9", 120, 116, 1);
  tft.drawString("+10", 134, 116, 1);
  tft.drawString("+30", 174, 116, 1);
  tft.drawString("20", 20, 146, 1);
  tft.drawString("40", 50, 146, 1);
  tft.drawString("60", 80, 146, 1);
  tft.drawString("80", 110, 146, 1);
  tft.drawString("100", 134, 146, 1);
  tft.drawString("120", 164, 146, 1);
  tft.drawString("dBuV", 280, 148, 2);

  // ── Accent labels ────────────────────────────────────────
  tft.setTextColor(UI_LABEL_COLOR);
  tft.drawString("S", 6, 99, 2);
  tft.drawString("M", 6, 129, 2);
  tft.drawString("PI:", 216, 188, 2);
  tft.drawString("PS:", 6, 188, 2);
  tft.drawString("PTY:", 6, 161, 2);

  // ── Frequency unit labels ─────────────────────────────────
  tft.setTextColor(UI_DIM_COLOR);
  tft.drawString("kHz", 225, 5, 2);
  if (band == 0) tft.drawString("MHz", 256, 79, 2);
  else           tft.drawString("kHz", 256, 79, 2);

  RDSstatusold = false;
  Stereostatusold = false;
  ShowFreq(0);
  updateTuneMode();
  updateBW();
  ShowUSBstatus();
  ShowStepSize();
  updateiMS();
  updateEQ();
  Squelchold = -2;
  SStatusold = 2000;
  SStatus = 100;
  rds_clockold = "";
  strcpy(programTypePrevious, "");
  strcpy(radioIdPrevious, "");
  strcpy(programServicePrevious, "");
  strcpy(radioTextPrevious, "");
  tft.drawCircle(81, 12, 10, TFT_GREYOUT);
  tft.drawCircle(81, 12, 9, TFT_GREYOUT);
  tft.drawCircle(91, 12, 10, TFT_GREYOUT);
  tft.drawCircle(91, 12, 9, TFT_GREYOUT);
  tft.drawBitmap(110, 1, RDSLogo, 67, 22, TFT_GREYOUT);
  if (StereoToggle == false) {
    tft.drawCircle(86, 12, 10, TFT_SKYBLUE);
    tft.drawCircle(86, 12, 9, TFT_SKYBLUE);
  }
  // ── Band label below frequency digits ────────────────────
  tft.setTextColor(band == 0 ? UI_FREQ_COLOR : TFT_SKYBLUE);
  if (band == 1) tft.drawString("AM", 48, 79, 2); else tft.drawString("FM", 48, 79, 2);
}

void ShowFreq(int mode) {
  if (setupmode == false) {
    if (band == 1) {
      if (freqold < 2000 && radio.getFrequency_AM() >= 2000 && stepsize == 0) if (radio.getFrequency_AM() != 27000 && freqold != 144) radio.setFrequency_AM(2000);
      if (freqold >= 2000 && radio.getFrequency_AM() < 2000 && stepsize == 0) if (radio.getFrequency_AM() != 144 && freqold != 27000) radio.setFrequency_AM(1998);
    }
  }

  if (screenmute == false) {
    detachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A));
    detachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B));
    if (band == 1) {
      unsigned int freq = radio.getFrequency_AM();
      String count = String(freq, DEC);
      if (count.length() != freqoldcount || mode != 0) {
        tft.setTextColor(TFT_BLACK);
        tft.drawRightString(String(freqold), 248, 30, 7);
      }
      tft.setTextColor(UI_FREQ_COLOR, TFT_BLACK);
      tft.drawRightString(String(freq), 248, 30, 7);
      freqold = freq;
      freqoldcount = count.length();
    } else {
      unsigned int freq = radio.getFrequency() + ConverterSet * 100;
      String count = String(freq / 100, DEC);
      if (count.length() != freqoldcount || mode != 0) {
        tft.setTextColor(TFT_BLACK);
        if (freqoldcount <= 2) tft.setCursor (108, 30);
        if (freqoldcount == 3) tft.setCursor (76, 30);
        if (freqoldcount >= 4) tft.setCursor (44, 30);
        tft.setTextFont(7);
        tft.print(freqold / 100);
        tft.print(".");
        if (freqold % 100 < 10) tft.print("0");
        tft.print(freqold % 100);
      }

      tft.setTextColor(UI_FREQ_COLOR, TFT_BLACK);
      if (mode == 0) {
        if (count.length() <= 2) tft.setCursor (108, 30);
        if (count.length() == 3) tft.setCursor (76, 30);
        if (count.length() >= 4) tft.setCursor (44, 30);
        tft.setTextFont(7);
        tft.print(freq / 100);
        tft.print(".");
        if (freq % 100 < 10) tft.print("0");
        tft.print(freq % 100);
        freqold = freq;
        freqoldcount = count.length();
      } else if (mode == 1) {
        tft.setTextColor(TFT_BLACK);
        if (freqoldcount <= 2) tft.setCursor (98, 30);
        if (freqoldcount == 3) tft.setCursor (71, 30);
        if (freqoldcount == 4) tft.setCursor (44, 30);
        tft.setTextFont(1);
        tft.print(freqold / 100);
        tft.print(".");
        if (freqold % 100 < 10) tft.print("0");
        tft.print(freqold % 100);
      }
    }
    attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A), read_encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B), read_encoder, CHANGE);
  }
}

void ShowSignalLevel() {
  if (band == 0) SNR = int(0.46222375 * (float)(SStatus / 10) - 0.082495118 * (float)(USN / 10)) + 10; else SNR = -((int8_t)(USN / 10));

  if (SNR > (SNRold + 1) || SNR < (SNRold - 1)) {
    tft.setTextFont(2);
    tft.setCursor (280, 161);
    tft.setTextColor(TFT_BLACK);
    if (SNRold == 99) tft.print("--"); else  tft.print(SNRold);
    tft.setCursor (280, 161);
    tft.setTextColor(UI_LABEL_COLOR, TFT_BLACK);
    if (tuned == true) {
      tft.print(SNR);
      SNRold = SNR;
    } else {
      tft.print("--");
      SNRold = 99;
    }
  }

  SAvg = (((SAvg * 9) + 5) / 10) + SStatus;
  SAvg2 = (((SAvg2 * 9) + 5) / 10) + SNR;

  float sval = 0;
  int16_t smeter = 0;
  int16_t segments;

  if (SStatus > 0)
  {
    if (SStatus < 1000)
    {
      sval = 51 * ((pow(10, (((float)SStatus) / 1000))) - 1);
      smeter = int16_t(sval);
    } else {
      smeter = 511;
    }
  }

  smeter = int16_t(sval);
  SStatus = SAvg / 10;
  SNR = SAvg2 / 10;

  if (menu == false) analogWrite(SMETERPIN, smeter);

  if (SStatus > (SStatusold + 3) || SStatus < (SStatusold - 3))
  {
    if (SStatus > 1200) SStatus = 1200;
    if (SStatus < -400) SStatus = -400;
    String count = String(abs(SStatus / 10), DEC);
    tft.setTextColor(1, TFT_BLACK);
    tft.setCursor (213, 103);
    tft.setTextFont(6);
    if (SStatus >= 0) {
      if (count.length() == 1) tft.print("00");
      if (count.length() == 2) tft.print("0");
    } else {
      if (count.length() == 1) tft.print("0 ");
      if (count.length() == 2) tft.print(" ");
    }
    tft.setTextColor(UI_LEVEL_COLOR, TFT_BLACK);
    if (SStatus < 0) tft.print("-");
    tft.print(abs(SStatus / 10));
    tft.setCursor (294, 103);
    tft.setTextFont(4);
    tft.print(".");
    if (SStatus < 0)
    {
      String negative = String (SStatus % 10, DEC);
      if (SStatus % 10 == 0) tft.print("0"); else tft.print(negative[1]);
    } else {
      tft.print(SStatus % 10);
    }

    if (band == 0) segments = (SStatus + 200) / 10; else segments = (SStatus + 200) / 10;

    tft.fillRect(16, 102, 2 * constrain(segments, 0, 40), 8, TFT_GREEN);
    tft.fillRect(16 + 2*40, 102, 2 * (constrain(segments, 40, 54) - 40), 8, TFT_YELLOW);
    tft.fillRect(16 + 2*54, 102, 2 * (constrain(segments, 54, 94) - 54), 8, TFT_RED);
    tft.fillRect(16 + 2 * constrain(segments, 0, 94), 102, 2 * (94 - constrain(segments, 0, 94)), 8, TFT_GREYOUT);

    SStatusold = SStatus;
    SStatusoldcount = count.length();
  }
}

void ShowRDSLogo(bool RDSstatus) {
  if (screenmute == false) {
    if (RDSstatus != RDSstatusold)
    {
      if (RDSstatus == true) tft.drawBitmap(110, 1, RDSLogo, 67, 22, UI_LABEL_COLOR); else tft.drawBitmap(110, 1, RDSLogo, 67, 22, TFT_GREYOUT);
      RDSstatusold = RDSstatus;
    }
  }
}

void ShowStereoStatus() {
  if (StereoToggle == true)
  {
    if (band == 0) Stereostatus = radio.getStereoStatus(); else Stereostatus = 0;
    if (Stereostatus != Stereostatusold)
    {
      if (Stereostatus == true && screenmute == false)
      {
        tft.drawCircle(81, 12, 10, TFT_MAGENTA);
        tft.drawCircle(81, 12, 9, TFT_MAGENTA);
        tft.drawCircle(91, 12, 10, TFT_MAGENTA);
        tft.drawCircle(91, 12, 9, TFT_MAGENTA);
      } else {
        if (screenmute == false) {
          tft.drawCircle(81, 12, 10, TFT_GREYOUT);
          tft.drawCircle(81, 12, 9, TFT_GREYOUT);
          tft.drawCircle(91, 12, 10, TFT_GREYOUT);
          tft.drawCircle(91, 12, 9, TFT_GREYOUT);
        }
      }
      Stereostatusold = Stereostatus;
    }
  }
}

void ShowOffset() {
  if (OStatus != OStatusold) {
    if (band == 0) {
      if (OStatus < -500) {
        tft.fillTriangle(6, 5, 6, 19, 14, 12, TFT_GREYOUT);
        tft.fillTriangle(18, 5, 18, 19, 26, 12, TFT_GREYOUT);
        tft.fillCircle(32, 12, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 12, 46, 5, 46, 19, TFT_GREYOUT);
        tft.fillTriangle(50, 12, 58, 5, 58, 19, TFT_RED);
        tuned = false;
      } else if (OStatus < -250 && OStatus > -500) {
        tft.fillTriangle(6, 5, 6, 19, 14, 12, TFT_GREYOUT);
        tft.fillTriangle(18, 5, 18, 19, 26, 12, TFT_GREYOUT);
        tft.fillCircle(32, 12, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 12, 46, 5, 46, 19, TFT_RED);
        tft.fillTriangle(50, 12, 58, 5, 58, 19, TFT_GREYOUT);
        tuned = false;
      } else if (USN < 250 && WAM < 250 && OStatus > -250 && OStatus < 250 && SQ == false) {
        tft.fillTriangle(6, 5, 6, 19, 14, 12, TFT_GREYOUT);
        tft.fillTriangle(18, 5, 18, 19, 26, 12, TFT_GREYOUT);
        tft.fillCircle(32, 12, 3, TFT_GREEN);
        tft.fillTriangle(38, 12, 46, 5, 46, 19, TFT_GREYOUT);
        tft.fillTriangle(50, 12, 58, 5, 58, 19, TFT_GREYOUT);
        tuned = true;
      } else if (OStatus > 250 && OStatus < 500) {
        tft.fillTriangle(6, 5, 6, 19, 14, 12, TFT_GREYOUT);
        tft.fillTriangle(18, 5, 18, 19, 26, 12, TFT_RED);
        tft.fillCircle(32, 12, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 12, 46, 5, 46, 19, TFT_GREYOUT);
        tft.fillTriangle(50, 12, 58, 5, 58, 19, TFT_GREYOUT);
        tuned = false;
      } else if (OStatus > 500) {
        tft.fillTriangle(6, 5, 6, 19, 14, 12, TFT_RED);
        tft.fillTriangle(18, 5, 18, 19, 26, 12, TFT_GREYOUT);
        tft.fillCircle(32, 12, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 12, 46, 5, 46, 19, TFT_GREYOUT);
        tft.fillTriangle(50, 12, 58, 5, 58, 19, TFT_GREYOUT);
        tuned = false;
      } else {
        tft.fillTriangle(6, 5, 6, 19, 14, 12, TFT_GREYOUT);
        tft.fillTriangle(18, 5, 18, 19, 26, 12, TFT_GREYOUT);
        tft.fillCircle(32, 12, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 12, 46, 5, 46, 19, TFT_GREYOUT);
        tft.fillTriangle(50, 12, 58, 5, 58, 19, TFT_GREYOUT);
        tuned = false;
      }
    } else {
      if (OStatus <= -3) {
        tft.fillTriangle(6, 5, 6, 19, 14, 12, TFT_GREYOUT);
        tft.fillTriangle(18, 5, 18, 19, 26, 12, TFT_GREYOUT);
        tft.fillCircle(32, 12, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 12, 46, 5, 46, 19, TFT_GREYOUT);
        tft.fillTriangle(50, 12, 58, 5, 58, 19, TFT_RED);
        tuned = false;
      } else if (OStatus < -2 && OStatus > -3) {
        tft.fillTriangle(6, 5, 6, 19, 14, 12, TFT_GREYOUT);
        tft.fillTriangle(18, 5, 18, 19, 26, 12, TFT_GREYOUT);
        tft.fillCircle(32, 12, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 12, 46, 5, 46, 19, TFT_RED);
        tft.fillTriangle(50, 12, 58, 5, 58, 19, TFT_GREYOUT);
        tuned = false;
      } else if (OStatus > -2 && OStatus < 2 && SQ == false) {
        tft.fillTriangle(6, 5, 6, 19, 14, 12, TFT_GREYOUT);
        tft.fillTriangle(18, 5, 18, 19, 26, 12, TFT_GREYOUT);
        tft.fillCircle(32, 12, 3, TFT_GREEN);
        tft.fillTriangle(38, 12, 46, 5, 46, 19, TFT_GREYOUT);
        tft.fillTriangle(50, 12, 58, 5, 58, 19, TFT_GREYOUT);
        tuned = true;
      } else if (OStatus > 2 && OStatus < 3) {
        tft.fillTriangle(6, 5, 6, 19, 14, 12, TFT_GREYOUT);
        tft.fillTriangle(18, 5, 18, 19, 26, 12, TFT_RED);
        tft.fillCircle(32, 12, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 12, 46, 5, 46, 19, TFT_GREYOUT);
        tft.fillTriangle(50, 12, 58, 5, 58, 19, TFT_GREYOUT);
        tuned = false;
      } else if (OStatus >= 3) {
        tft.fillTriangle(6, 5, 6, 19, 14, 12, TFT_RED);
        tft.fillTriangle(18, 5, 18, 19, 26, 12, TFT_GREYOUT);
        tft.fillCircle(32, 12, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 12, 46, 5, 46, 19, TFT_GREYOUT);
        tft.fillTriangle(50, 12, 58, 5, 58, 19, TFT_GREYOUT);
        tuned = false;
      } else {
        tft.fillTriangle(6, 5, 6, 19, 14, 12, TFT_GREYOUT);
        tft.fillTriangle(18, 5, 18, 19, 26, 12, TFT_GREYOUT);
        tft.fillCircle(32, 12, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 12, 46, 5, 46, 19, TFT_GREYOUT);
        tft.fillTriangle(50, 12, 58, 5, 58, 19, TFT_GREYOUT);
        tuned = false;
      }
    }
    OStatusold = OStatus;
  }
}

void ShowBW() {
  if (BW != BWOld || BWreset == true) {
    tft.setTextColor(TFT_BLACK);
    tft.drawRightString(String (BWOld, DEC), 218, 5, 2);
    if (BWset == 0) tft.setTextColor(UI_LABEL_COLOR); else tft.setTextColor(TFT_YELLOW);
    tft.drawRightString(String (BW, DEC), 218, 5, 2);
    BWOld = BW;
    BWreset = false;
  }
}

void ShowModLevel() {
  int segments;
  int color;
  int hold = 0;

  if (SQ != false) {
    MStatus = 0;
    MStatusold = 1;
  }

  if (MStatus != MStatusold || MStatus < 10) {
    for (segments = 0; segments < 13; segments++) {
      color = TFT_GREEN;
      if (segments > 7) color = TFT_YELLOW;
      if (segments > 8) color = TFT_ORANGE;
      if (segments > 9) color = TFT_RED;
      if (MStatus > (segments + 1) * 10)
      {
        hold = segments;
        tft.fillRect(20 + segments * 14, 132, 12, 8, color);
      } else {
        if (segments != peakholdold) tft.fillRect(20 + segments * 14, 132, 12, 8, TFT_GREYOUT);
      }
    }

    if (peakholdold < hold) peakholdold = hold;

    if (peakholdmillis > peakholdtimer + 3000) {
      peakholdtimer += 3000;
      peakholdold = hold;
    }
    peakholdmillis = millis();
    MStatusold = MStatus;
  }
}

void doSquelch() {
  if (USBstatus == false) {
    Squelch = analogRead(PIN_POT) / 4 - 100;
    if (Squelch > 920) Squelch = 920;

    if (seek == false && menu == false && Squelch != Squelchold) {
      tft.setTextFont(2);
      tft.setTextColor(TFT_BLACK);
      tft.setCursor (216, 161);
      if (Squelchold == -100) {
        tft.print("OFF");
      } else if (Squelchold == 920) {
        tft.print("ST");
      } else {
        tft.print(Squelchold / 10);
      }
      tft.setTextColor(TFT_WHITE);
      tft.setCursor (216, 161);
      if (Squelch == -100) {
        tft.print("OFF");
      } else if (Squelch == 920) {
        tft.print("ST");
      } else {
        tft.print(Squelch / 10);
      }
      Squelchold = Squelch;
    }
  }
  if (seek == false && USBstatus == true) {
    if (XDRMute == false) {
      if (Squelch != -1) {
        if (Squelch < SStatus || Squelch == -100) {
          radio.setUnMute();
          SQ = false;
        } else {
          radio.setMute();
          SQ = true;
        }
      } else {
        if (Stereostatus == true) {
          radio.setUnMute();
          SQ = false;
        } else {
          radio.setMute();
          SQ = true;
        }
      }
      if (screenmute == false) {
        if (Squelch != Squelchold) {
          tft.setTextFont(2);
          tft.setTextColor(TFT_BLACK);
          tft.setCursor (216, 161);
          if (Squelchold == -1) tft.print("ST"); else tft.print(Squelchold / 10);
          tft.setTextColor(TFT_WHITE);
          tft.setCursor (216, 161);
          if (Squelch == -1) tft.print("ST"); else tft.print(Squelch / 10);
          Squelchold = Squelch;
        }
      }
    }
  } else {
    if (seek == false && Squelch != 920) {
      if (Squelch < SStatus || Squelch == -100) {
        radio.setUnMute();
        SQ = false;
      } else {
        radio.setMute();
        SQ = true;
      }
    } else {
      if (seek == false && Stereostatus == true) {
        radio.setUnMute();
        SQ = false;
      } else {
        radio.setMute();
        SQ = true;
      }
    }
  }
  ShowSquelch();
}

void ShowSquelch() {
  if (menu == false) {
    if (SQ == false) {
      tft.drawRoundRect(3, 72, 40, 18, 4, TFT_GREYOUT);
      tft.setTextColor(TFT_GREYOUT);
      tft.drawCentreString("MUTE", 24, 74, 2);
    } else {
      tft.drawRoundRect(3, 72, 40, 18, 4, UI_LEVEL_COLOR);
      tft.setTextColor(UI_LEVEL_COLOR);
      tft.drawCentreString("MUTE", 24, 74, 2);
    }
  }
}


void updateBW() {
  if (BWset == 0) {
    if (screenmute == false) {
      tft.drawRoundRect(249, 27, 68, 18, 4, UI_LABEL_COLOR);
      tft.setTextColor(UI_LABEL_COLOR);
      tft.drawCentreString("AUTO BW", 283, 29, 2);
    }
    radio.setFMABandw();
  } else {
    if (screenmute == false) {
      tft.drawRoundRect(249, 27, 68, 18, 4, TFT_GREYOUT);
      tft.setTextColor(TFT_GREYOUT);
      tft.drawCentreString("AUTO BW", 283, 29, 2);
    }
  }
}

void updateiMS() {
  if (band == 0) {
    if (iMSset == 0) {
      if (screenmute == false) {
        tft.drawRoundRect(249, 46, 30, 18, 4, UI_LABEL_COLOR);
        tft.setTextColor(UI_LABEL_COLOR);
        tft.drawCentreString("iMS", 265, 48, 2);
      }
      radio.setiMS(1);
    } else {
      if (screenmute == false) {
        tft.drawRoundRect(249, 46, 30, 18, 4, TFT_GREYOUT);
        tft.setTextColor(TFT_GREYOUT);
        tft.drawCentreString("iMS", 265, 48, 2);
      }
      radio.setiMS(0);
    }
  }
}

void updateEQ() {
  if (band == 0) {
    if (EQset == 0) {
      if (screenmute == false) {
        tft.drawRoundRect(287, 46, 30, 18, 4, UI_LABEL_COLOR);
        tft.setTextColor(UI_LABEL_COLOR);
        tft.drawCentreString("EQ", 303, 48, 2);
      }
      radio.setEQ(1);
    } else {
      if (screenmute == false) {
        tft.drawRoundRect(287, 46, 30, 18, 4, TFT_GREYOUT);
        tft.setTextColor(TFT_GREYOUT);
        tft.drawCentreString("EQ", 303, 48, 2);
      }
      radio.setEQ(0);
    }
  }
}

void doBW() {
  if (band == 0) {
    if (BWset > 16) BWset = 0;

    switch (BWset) {
      case 1:
        radio.setFMBandw(56);
        break;

      case 2:
        radio.setFMBandw(64);
        break;

      case 3:
        radio.setFMBandw(72);
        break;

      case 4:
        radio.setFMBandw(84);
        break;

      case 5:
        radio.setFMBandw(97);
        break;

      case 6:
        radio.setFMBandw(114);
        break;

      case 7:
        radio.setFMBandw(133);
        break;

      case 8:
        radio.setFMBandw(151);
        break;

      case 9:
        radio.setFMBandw(168);
        break;

      case 10:
        radio.setFMBandw(184);
        break;

      case 11:
        radio.setFMBandw(200);
        break;

      case 12:
        radio.setFMBandw(217);
        break;

      case 13:
        radio.setFMBandw(236);
        break;

      case 14:
        radio.setFMBandw(254);
        break;

      case 15:
        radio.setFMBandw(287);
        break;

      case 16:
        radio.setFMBandw(311);
        break;
    }
  } else {
    if (BWset > 4) BWset = 1;

    switch (BWset) {
      case 1:
        radio.setAMBandw(3);
        break;

      case 2:
        radio.setAMBandw(4);
        break;

      case 3:
        radio.setAMBandw(6);
        break;

      case 4:
        radio.setAMBandw(8);
        break;
    }
  }
  updateBW();
  BWreset = true;
}

void doTuneMode() {
  if (band == 0) {
    if (tunemode == true) tunemode = false; else tunemode = true;
    updateTuneMode();
    if (stepsize != 0) {
      stepsize = 0;
      RoundStep();
      ShowStepSize();
      ShowFreq(0);
    }
  }
}

void updateTuneMode() {
  if (tunemode == true) {
    tft.drawRoundRect(3, 46, 40, 18, 4, UI_LABEL_COLOR);
    tft.setTextColor(UI_LABEL_COLOR);
    tft.drawCentreString("AUTO", 24, 48, 2);

    tft.drawRoundRect(3, 27, 40, 18, 4, TFT_GREYOUT);
    tft.setTextColor(TFT_GREYOUT);
    tft.drawCentreString("MAN", 24, 29, 2);
  } else {
    tft.drawRoundRect(3, 46, 40, 18, 4, TFT_GREYOUT);
    tft.setTextColor(TFT_GREYOUT);
    tft.drawCentreString("AUTO", 24, 48, 2);

    tft.drawRoundRect(3, 27, 40, 18, 4, UI_LABEL_COLOR);
    tft.setTextColor(UI_LABEL_COLOR);
    tft.drawCentreString("MAN", 24, 29, 2);
  }
}
void ShowUSBstatus() {
  if (USBstatus == true) tft.drawBitmap(272, 1, USBLogo, 43, 21, TFT_SKYBLUE); else tft.drawBitmap(272, 1, USBLogo, 43, 21, TFT_GREYOUT);
}

void XDRGTKRoutine() {
  if (Serial.available() > 0)
  {
    buff[buff_pos] = Serial.read();
    if (buff[buff_pos] != '\n' && buff_pos != 16 - 1) {
      buff_pos++;
    } else {
      buff[buff_pos] = 0;
      buff_pos = 0;

      switch (buff[0])
      {
        case 'x':
          Serial.println("OK");
          if (band != 0) {
            band = 0;
            SelectBand();
          }
          Serial.print("T" + String(frequency * 10) + "A0\nD0\nG00\n");
          USBstatus = true;
          ShowUSBstatus();
          if (menu == true) ModeButtonPress();
          if (Squelch != Squelchold) {
            if (screenmute == false) {
              tft.setTextFont(2);
              tft.setTextColor(TFT_BLACK);
              tft.setCursor (240, 161);
              if (Squelchold == -100) tft.print("OFF"); else if (Squelchold > 920) tft.print("ST"); else tft.print(Squelchold / 10);
            }
          }
          break;

        case 'A':
          AGC = atol(buff + 1);
          Serial.print("A" + String(AGC) + "\n");
          radio.setAGC(AGC);
          break;

        case 'C':
          byte scanmethod;
          scanmethod = atol(buff + 1);
          if (seek == false) {
            if (scanmethod == 1) {
              Serial.print("C1\n");
              direction = true;
              seek = true;
              Seek(direction);
            }
            if (scanmethod == 2) {
              Serial.print("C2\n");
              direction = false;
              seek = true;
              Seek(direction);
            }
          } else {
            seek = false;
          }
          Serial.print("C0\n");
          break;

        case 'N':
          doStereoToggle();
          break;

        case 'D':
          DeEmphasis = atol(buff + 1);
          Serial.print("D" + String(DeEmphasis) + "\n");
          radio.setDeemphasis(DeEmphasis);
          break;

        case 'F':
          XDRBWset = atol(buff + 1);
          if (XDRBWset < 16) {
            XDRBWsetold = XDRBWset;
            BWset = XDRBWset + 1;
          } else {
            XDRBWset = XDRBWsetold;
          }
          doBW();
          Serial.print("F" + String(XDRBWset) + "\n");
          break;

        case 'G':
          LevelOffset =  atol(buff + 1);
          if (LevelOffset == 0) {
            MuteScreen(0);
            LowLevelSet = EEPROM.readByte(43);
            EEPROM.commit();
            Serial.print("G00\n");
          }
          if (LevelOffset == 10) {
            MuteScreen(1);
            LowLevelSet = EEPROM.readByte(43);
            EEPROM.commit();
            Serial.print("G10\n");
          }
          if (LevelOffset == 1) {
            MuteScreen(0);
            LowLevelSet = 120;
            Serial.print("G01\n");
          }
          if (LevelOffset == 11) {
            LowLevelSet = 120;
            MuteScreen(1);
            Serial.print("G11\n");
          }
          break;

        case 'M':
          byte XDRband;
          XDRband = atol(buff + 1);
          if (XDRband == 0) {
            band = 0;
            SelectBand();
            Serial.print("M0\nT" + String(frequency * 10) + "\n");
          } else {
            band = 1;
            SelectBand();
            Serial.print("M1\nT" + String(frequency_AM) + "\n");
          }
          break;

        case 'T':
          unsigned int freqtemp;
          freqtemp = atoi(buff + 1);
          if (seek == true) seek = false;
          if (freqtemp > 143 && freqtemp < 27001) {
            frequency_AM = freqtemp;
            if (band != 1) {
              band = 1;
              SelectBand();
            } else {
              radio.setFrequency_AM(frequency_AM);
            }
            Serial.print("M1\n");
          } else if (freqtemp > 64999 && freqtemp < 108001) {
            frequency = freqtemp / 10;
            if (band != 0) {
              band = 0;
              SelectBand();
              Serial.print("M0\n");
            } else {
              radio.setFrequency(frequency, 65, 108, fullsearchrds);
            }
          }
          if (band == 0) Serial.print("T" + String(frequency * 10) + "\n"); else Serial.print("T" + String(frequency_AM) + "\n");
          radio.clearRDS(fullsearchrds);
          RDSstatus = 0;
          ShowFreq(0);
          break;

        case 'S':
          if (buff[1] == 'a')
          {
            scanner_start = (atol(buff + 2) + 5) / 10;
          } else if (buff[1] == 'b')
          {
            scanner_end = (atol(buff + 2) + 5) / 10;
          } else if (buff[1] == 'c')
          {
            scanner_step = (atol(buff + 2) + 5) / 10;
          } else if (buff[1] == 'f')
          {
            scanner_filter = atol(buff + 2);
          } else if (scanner_start > 0 && scanner_end > 0 && scanner_step > 0 && scanner_filter >= 0)
          {
            frequencyold = radio.getFrequency();
            radio.setFrequency(scanner_start, 65, 108, fullsearchrds);
            Serial.print('U');
            if (scanner_filter < 0) {
              BWset = 0;
            } else if (scanner_filter == 0) {
              BWset = 1;
            } else if (scanner_filter == 26) {
              BWset = 2;
            } else if (scanner_filter == 1) {
              BWset = 3;
            } else if (scanner_filter == 28) {
              BWset = 4;
            } else if (scanner_filter == 29) {
              BWset = 5;
            } else if (scanner_filter == 3) {
              BWset = 6;
            } else if (scanner_filter == 4) {
              BWset = 7;
            } else if (scanner_filter == 5) {
              BWset = 8;
            } else if (scanner_filter == 7) {
              BWset = 9;
            } else if (scanner_filter == 8) {
              BWset = 10;
            } else if (scanner_filter == 9) {
              BWset = 11;
            } else if (scanner_filter == 10) {
              BWset = 12;
            } else if (scanner_filter == 11) {
              BWset = 13;
            } else if (scanner_filter == 12) {
              BWset = 14;
            } else if (scanner_filter == 13) {
              BWset = 15;
            } else if (scanner_filter == 15) {
              BWset = 16;
            }
            doBW();
            if (screenmute == false) {
              ShowFreq(1);
              tft.setTextFont(4);
              tft.setTextColor(TFT_WHITE, TFT_BLACK);
              tft.setCursor (90, 60);
              tft.print("SCANNING...");
            }
            frequencyold = frequency / 10;
            for (freq_scan = scanner_start; freq_scan <= scanner_end; freq_scan += scanner_step)
            {
              radio.setFrequency(freq_scan, 65, 108, fullsearchrds);
              Serial.print(freq_scan * 10, DEC);
              Serial.print('=');
              delay(10);
              if (band == 0) {
                radio.getStatus(SStatus, USN, WAM, OStatus, BW, MStatus);
              } else {
                radio.getStatus_AM(SStatus, USN, WAM, OStatus, BW, MStatus);
              }
              Serial.print((SStatus / 10) + 10, DEC);
              Serial.print(',');
            }
            Serial.print('\n');
            if (screenmute == false) {
              tft.setTextFont(4);
              tft.setTextColor(TFT_BLACK);
              tft.setCursor (90, 60);
              tft.print("SCANNING...");
            }
            radio.setFrequency(frequencyold, 65, 108, fullsearchrds);
            if (screenmute == false) ShowFreq(0);
            radio.setFMABandw();
          }
          break;

        case 'Y':
          VolSet = atoi(buff + 1);
          if (VolSet == 0) {
            radio.setMute();
            XDRMute = true;
            SQ = true;
          } else {
            radio.setVolume((VolSet - 70) / 10);
            XDRMute = false;
          }
          Serial.print("Y" + String(VolSet) + "\n");
          break;

        case 'X':
          Serial.print("X\n");
          ESP.restart();
          break;

        case 'Z':
          byte iMSEQX;
          iMSEQX = atol(buff + 1);
          if (iMSEQX == 0) {
            iMSset = 1;
            EQset = 1;
            iMSEQ = 2;
          }
          if (iMSEQX == 1) {
            iMSset = 0;
            EQset = 1;
            iMSEQ = 3;
          }
          if (iMSEQX == 2) {
            iMSset = 1;
            EQset = 0;
            iMSEQ = 4;
          }
          if (iMSEQX == 3) {
            iMSset = 0;
            EQset = 0;
            iMSEQ = 1;
          }
          updateiMS();
          updateEQ();
          Serial.print("Z" + String(iMSEQX) + "\n");
          break;
      }
    }
  }

  if (USBstatus == true) {
    Stereostatus = radio.getStereoStatus();
    if (StereoToggle == false) {
      Serial.print("SS");
    } else if (Stereostatus == true && band == 0) {
      Serial.print("Ss");
    } else {
      Serial.print("Sm");
    }
    if (SStatus > (SStatusold + 10) || SStatus < (SStatusold - 10)) {
      Serial.print(String(((SStatus * 100) + 10875) / 1000) + "." + String(((SStatus * 100) + 10875) / 100 % 10));
    } else {
      Serial.print(String(((SStatusold * 100) + 10875) / 1000) + "." + String(((SStatus * 100) + 10875) / 100 % 10));
    }
    Serial.print("," + String(WAM / 10, DEC) + "," + String(SNR, DEC) + "\n");
  }
}

void Seek(bool mode) {
  if (band == 0) {
    radio.setMute();
    if (mode == false) frequency = radio.tuneDown(stepsize, LowEdgeSet, HighEdgeSet, fullsearchrds); else frequency = radio.tuneUp(stepsize, LowEdgeSet, HighEdgeSet, fullsearchrds);
    delay(50);
    ShowFreq(0);
      if (USBstatus == true) if (band == 0) Serial.print("T" + String(frequency * 10) + "\n"); else Serial.print("T" + String(frequency_AM) + "\n");
    radio.getStatus(SStatus, USN, WAM, OStatus, BW, MStatus);

    if ((USN < 200) && (WAM < 230) && (OStatus < 80 && OStatus > -80) && (Squelch < SStatus || Squelch == 920)) {
      seek = false;
      radio.setUnMute();
      store = true;
    } else {
      seek = true;
    }
  }
}

void SetTunerPatch() {
  if (TEF != 101 && TEF != 102 && TEF != 205) {
    radio.init(102);
    uint16_t device;
    uint16_t hw;
    uint16_t sw;
    radio.getIdentification(device, hw, sw);
    TEF = highByte(hw) * 100 + highByte(sw);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    analogWrite(CONTRASTPIN, ContrastSet * 2 + 27);
    if (TEF == 0) tft.drawCentreString("Tuner not detected", 150, 70, 4); else tft.drawCentreString(String("Tuner version set: v") + String(TEF), 150, 70, 4);
    tft.drawCentreString("Please restart tuner", 150, 100, 4);
    EEPROM.writeByte(54, TEF);
    EEPROM.commit();
    while (true);
    for (;;);
  }
}

void read_encoder() {
  static uint8_t old_AB = 3;
  static int8_t encval = 0;
  static const int8_t enc_states[]  = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

  old_AB <<= 2;

  if (digitalRead(ROTARY_PIN_A)) old_AB |= 0x02;
  if (digitalRead(ROTARY_PIN_B)) old_AB |= 0x01;
  encval += enc_states[( old_AB & 0x0f )];

  if (optenc == 1) {
    if (encval > 2) {
      if (rotarymode == true) rotary = -1; else rotary = 1;
      encval = 0;
    } else if (encval < -2) {
      if (rotarymode == true) rotary = 1; else rotary = -1;
      encval = 0;
    }
  } else {
    if (encval > 3) {
      if (rotarymode == true) rotary = -1; else rotary = 1;
      encval = 0;
    } else if (encval < -3) {
      if (rotarymode == true) rotary = 1; else rotary = -1;
      encval = 0;
    }
  }
}
