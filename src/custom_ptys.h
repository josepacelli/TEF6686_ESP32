#pragma once
#include <Arduino.h>

struct PTYEntry {
  uint32_t freq_khz; // frequency in kHz (e.g. 102700 = 102.7 MHz)
  uint8_t pty_code; // PTY code (0-31)
  String ps;
  String rt;
  String song;
  String hour;    // HH format
  String minute;  // MM format
  String day;     // DD format
  String month;   // MM format
  String year;    // YYYY format
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
