#pragma once
#include <Arduino.h>

struct PTYEntry {
  uint32_t freq_khz; // frequency in kHz (e.g. 102700 = 102.7 MHz)
  String pty;
  String ps;
  String toString() const {
    return String("PTYEntry(freq_khz=") + String(freq_khz) + ", pty=" + pty + ", ps=" + ps + ")";
  }
};

void loadCustomPTYS();
void saveCustomPTYS();
size_t getCustomPTYSCount();
PTYEntry getCustomPTYEntry(size_t idx);
String findCustomPTYForFreq(uint32_t freq_khz);
void addCustomPTY(uint32_t freq_khz, const String &pty, const String &ps);
void removeCustomPTY(size_t idx);

// Declaração da função para buscar o PS customizado
String findCustomPSForFreq(uint32_t freq_khz);
