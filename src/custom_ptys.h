#pragma once
#include <Arduino.h>

struct PTYEntry {
  uint32_t freq_khz; // frequency in kHz (e.g. 102700 = 102.7 MHz)
  uint8_t pty_code; // PTY code (0-31)
  String ps;
  String rt;
  String song;
  String songYear; // year of the song (YYYY format)
  String hour;    // HH format
  String minute;  // MM format
  String second;  // SS format
  String day;     // DD format
  String month;   // MM format
  String year;    // YYYY format
  String weather; // weather condition (e.g. Sunny, Rainy, Cloudy)
  String temperature; // temperature (e.g. 28°C)
  int songScrollPos = 0; // scroll position for song rotation
  String toString() const {
    return String("PTYEntry(freq_khz=") + String(freq_khz) + ", pty_code=" + String(pty_code) + ", ps=" + ps + ", rt=" + rt + ", song=" + song + ")";
  }
};

void loadIsaacPTYs();
int8_t findCustomPTYCodeForFreq(uint32_t freq_khz);
String findCustomPSForFreq(uint32_t freq_khz);
String findCustomRTForFreq(uint32_t freq_khz);
String findCustomSongForFreq(uint32_t freq_khz);
String findCustomTimeForFreq(uint32_t freq_khz);
String findCustomDateForFreq(uint32_t freq_khz);
String rotateSongString(const String& song, int scrollPos);
void advanceSongScrollPos(uint32_t freq_khz);
String formatSongWithYear(const String& song, const String& year);
const char* getPTYName(uint8_t pty_code);
String getRandomGospelPS();
String getRandomGospelRT();
String getRandomGenreByPTY(uint8_t pty_code);
String getRandomPSByPTY(uint8_t pty_code);
String getRandomRTByPTY(uint8_t pty_code);
String getGenreByPS(const String& ps);
