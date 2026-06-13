#include "ui_language.h"

const char* getUIString(int id, uint8_t language) {
  if (id < 0 || id >= UI_STRING_COUNT) return "";
  switch (language) {
    case 2:  return UI_PORTUGUESE[id];
    case 3:  return UI_SPANISH[id];
    default: return UI_ENGLISH[id];
  }
}
