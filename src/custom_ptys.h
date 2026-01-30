#pragma once
#include <Arduino.h>

struct PTYEntry {
  uint32_t freq_khz; // frequency in kHz (e.g. 102700 = 102.7 MHz)
  String pty;
};

void loadCustomPTYS();
void saveCustomPTYS();
size_t getCustomPTYSCount();
PTYEntry getCustomPTYEntry(size_t idx);
String findCustomPTYForFreq(uint32_t freq_khz);
void addCustomPTY(uint32_t freq_khz, const String &pty);
void removeCustomPTY(size_t idx);
