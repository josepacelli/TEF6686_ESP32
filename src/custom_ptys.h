#ifndef CUSTOM_PTYS_H
#define CUSTOM_PTYS_H

#include <Arduino.h>
#include <cstdint>
#include <vector>

struct PTYEntry {
  uint32_t freq_khz;
  String ps;
  String rt;
  int8_t pty_code;
  String song;
  String songYear;
  size_t songScrollPos;
  String hour;
  String minute;
  String second;
  String day;
  String month;
  String year;
  String weather;
  String temperature;
  bool custom_rds_enabled = true;
};

void loadIsaacPTYs();
bool isCustomRDSEnabled(uint32_t freq_khz);
String findCustomPSForFreq(uint32_t freq_khz);
String findCustomRTForFreq(uint32_t freq_khz);
int8_t findCustomPTYCodeForFreq(uint32_t freq_khz);
void advanceSongScrollPos(uint32_t freq_khz);
String findCustomSongForFreq(uint32_t freq_khz);
String findCustomGenreForFreq(uint32_t freq_khz);
size_t getCustomPTYCount();
PTYEntry& getCustomPTYAt(size_t i);
const char** getPSListForPTY(int8_t pty_code, int* count);

#endif // CUSTOM_PTYS_H
