#include "custom_ptys.h"
#include <SPIFFS.h>
#include <vector>
#include <cmath>
#include "logbook.h"

static std::vector<PTYEntry> customPtys;
static const char * CUSTOM_PTY_PATH = "/custom_ptys.csv";

void loadCustomPTYS() {
  customPtys.clear();
  log_info("Iniciando leitura do CSV de PTYs personalizados...\n");
  if (!SPIFFS.exists(CUSTOM_PTY_PATH)) {
    log_info("Arquivo de PTYs personalizados não existe.\n");
    return;
  };
  fs::File f = SPIFFS.open(CUSTOM_PTY_PATH, "r");
  if (!f) {
    log_info("Erro ao abrir o arquivo de PTYs personalizados.\n");
    return;
  }
  while (f.available()) {
    String line = f.readStringUntil('\n');
    line.trim();
    log_info("Read line: " + line + "\n");
    if (line.length() == 0) continue;
    int comma = line.indexOf(',');
    if (comma == -1) continue;
    String freqs = line.substring(0, comma);
    String pty = line.substring(comma + 1);
    freqs.trim(); pty.trim();
    // accept either kHz integer or MHz with decimal
    uint32_t freq_khz = 0;
    if (freqs.indexOf('.') != -1) {
      // MHz like 102.7 -> kHz *1000 = 102700
      float mhz = freqs.toFloat();
      freq_khz = (uint32_t)round(mhz * 1000.0);
    } else {
      freq_khz = (uint32_t)freqs.toInt();
      // if it looks like MHz without decimal but small, assume MHz
      if (freq_khz > 0 && freq_khz < 2000) freq_khz = freq_khz * 1000;
    }

    PTYEntry e; e.freq_khz = freq_khz; e.pty = pty;
    log_info("PTY: freq_khz=" + String(e.freq_khz) + " pty=" + e.pty + "\n");
    customPtys.push_back(e);
  }
  f.close();
}

void saveCustomPTYS() {
  fs::File f = SPIFFS.open(CUSTOM_PTY_PATH, "w");
  if (!f) return;
  for (auto &e : customPtys) {
    // write frequency as MHz with one decimal
    // freq_khz is in kHz (e.g., 102700 = 102.7 MHz)
    String freqStr = String((float)e.freq_khz / 1000.0, 1);
    f.print(freqStr + "," + e.pty + "\n");
  }
  f.flush();
  f.close();
}

size_t getCustomPTYSCount() {
  return customPtys.size();
}

PTYEntry getCustomPTYEntry(size_t idx) {
  PTYEntry e; e.freq_khz = 0; e.pty = "";
  if (idx < customPtys.size()) return customPtys[idx];
  return e;
}

String findCustomPTYForFreq(uint32_t freq_khz) {
  // exact match first
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) return e.pty;
  }
  // try match within 100 kHz tolerance for rounding differences
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) return e.pty;
  }
  return String("");
}

void addCustomPTY(uint32_t freq_khz, const String &pty) {
  PTYEntry e; e.freq_khz = freq_khz; e.pty = pty;
  customPtys.push_back(e);
  saveCustomPTYS();
}

void removeCustomPTY(size_t idx) {
  if (idx < customPtys.size()) {
    customPtys.erase(customPtys.begin() + idx);
    saveCustomPTYS();
  }
}
