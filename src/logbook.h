#ifndef LOGBOOK_H
#define LOGBOOK_H

#include <Arduino.h>

inline void log_info(const String& message) {
  Serial.println(message);
}

#endif
