#ifndef RDS_CAPTURE_H
#define RDS_CAPTURE_H

#include <Arduino.h>

#define RDS_CAPTURE_MAX 10

void addCapturedPS(uint32_t freq_khz, const String& s);
void addCapturedRT(uint32_t freq_khz, const String& s);
String getCapturedPS(uint32_t freq_khz, int i);
String getCapturedRT(uint32_t freq_khz, int i);
int getCapturedPSCount(uint32_t freq_khz);
int getCapturedRTCount(uint32_t freq_khz);

#endif
