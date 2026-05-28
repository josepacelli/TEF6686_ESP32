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
};

void loadIsaacPTYs();
String findCustomPSForFreq(uint32_t freq_khz);
String findCustomRTForFreq(uint32_t freq_khz);
int8_t findCustomPTYCodeForFreq(uint32_t freq_khz);
void advanceSongScrollPos(uint32_t freq_khz);
String findCustomSongForFreq(uint32_t freq_khz);
String findCustomGenreForFreq(uint32_t freq_khz);

#endif // CUSTOM_PTYS_H
