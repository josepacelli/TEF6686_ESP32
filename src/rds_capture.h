#ifndef RDS_CAPTURE_H
#define RDS_CAPTURE_H

#include <Arduino.h>

#define RDS_CAPTURE_MAX 10

void addCapturedPS(const String& s);
void addCapturedRT(const String& s);
String getCapturedPS(int i);
String getCapturedRT(int i);
int getCapturedPSCount();
int getCapturedRTCount();

#endif
