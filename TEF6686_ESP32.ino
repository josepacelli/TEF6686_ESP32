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
byte themeSet = 0;
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
int rdsStationIndex = 0;
int rdsScrollTop = 0;
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
  EEPROM.begin(300);
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
  themeSet = EEPROM.readByte(241);
  if (themeSet > 76) themeSet = 0;
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
    EEPROM.writeByte(241, themeSet);
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
        }
      } else {
        if (menuoption == 30) {
          getEstacao(ptyStationIndex).pty_code = (int8_t)ptyEditCode;
        } else if (menuoption == 50) {
          if (psEditCount > 0) getEstacao(ptyStationIndex).ps = getEstacao(psEditIndex).ps;
        } else if (menuoption == 70) {
          if (rtEditCount > 0) getEstacao(ptyStationIndex).rt = getEstacao(rtEditIndex).rt;
        } else if (menuoption == 90) {
          getEstacao(ptyStationIndex).pi_code = (uint16_t)piEditCode;
          saveCustomPICodes();
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
        if (menuoption > 150) menuoption = 30;
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
          lastCustomFreq = 0;
          EEPROM.writeByte(56, languageSet);
          EEPROM.commit();
          break;

        case 250:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(getThemeName(themeSet), 165, 110, 4);
          themeSet = (themeSet + 1) % 77;
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
        if (menuoption < 30) menuoption = 150;
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
          lastCustomFreq = 0;
          EEPROM.writeByte(56, languageSet);
          EEPROM.commit();
          break;

        case 250:
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(getThemeName(themeSet), 165, 110, 4);
          themeSet = (themeSet + 76) % 77;
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
    tft.drawString(">>", 275, 90, 2);
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
    tft.setTextColor(TFT_WHITE);
    tft.drawString("PS:", 15, 50, 2);
    tft.setTextColor(TFT_YELLOW);
    String ps6 = getEstacao(ptyStationIndex).ps;
    if (ps6.length() > 20) ps6 = ps6.substring(0, 20);
    tft.drawString(ps6, 60, 50, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("RT:", 15, 70, 2);
    tft.setTextColor(TFT_YELLOW);
    String rt6 = getEstacao(ptyStationIndex).rt;
    if (rt6.length() > 20) rt6 = rt6.substring(0, 20);
    tft.drawString(rt6, 60, 70, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("PI:", 15, 90, 2);
    tft.setTextColor(TFT_YELLOW);
    String piStr6 = String(piCode6, HEX); piStr6.toUpperCase();
    while (piStr6.length() < 4) piStr6 = "0" + piStr6;
    tft.drawString(piStr6, 60, 90, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("RDS:", 15, 110, 2);
    bool rdsEn6 = getEstacao(ptyStationIndex).rds_ativo;
    tft.setTextColor(rdsEn6 ? TFT_GREEN : TFT_RED);
    tft.drawString(rdsEn6 ? "ON" : "OFF", 60, 110, 2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Todos RDS:", 15, 150, 2);
    tft.setTextColor(TFT_CYAN);
    tft.drawString("LIGAR", 110, 150, 2);
    tft.setTextColor(TFT_SKYBLUE);
    tft.drawString(getUIString(UI_BACK, languageSet), 15, 130, 2);
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

const char* getThemeName(uint8_t t) {
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
    default: return "Original";
  }
}

void applyTheme(uint8_t theme) {
  themeSet = theme % 77;
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
