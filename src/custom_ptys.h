#pragma once
#include <Arduino.h>

struct PTYEntry {
  uint32_t freq_khz; // frequency in kHz (e.g. 102700 = 102.7 MHz)
  uint8_t pty_code; // PTY code (0-31)
  String ps;
  String rt;
  String toString() const {
    return String("PTYEntry(freq_khz=") + String(freq_khz) + ", pty_code=" + String(pty_code) + ", ps=" + ps + ", rt=" + rt + ")";
  }
};

void loadCustomPTYS();
void saveCustomPTYS();
size_t getCustomPTYSCount();
PTYEntry getCustomPTYEntry(size_t idx);
int8_t findCustomPTYCodeForFreq(uint32_t freq_khz);
void addCustomPTY(uint32_t freq_khz, uint8_t pty_code, const String &ps, const String &rt);
void removeCustomPTY(size_t idx);

String findCustomPSForFreq(uint32_t freq_khz);
String findCustomRTForFreq(uint32_t freq_khz);
