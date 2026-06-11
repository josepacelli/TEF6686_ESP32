#include "rt_language.h"

const char* getRTByLanguage(int index, uint8_t language) {
  switch (language) {
    case PTY_LANG_PORTUGUESE: return RT_PORTUGUESE[index];
    case PTY_LANG_SPANISH:    return RT_SPANISH[index];
    case PTY_LANG_ENGLISH:
    default:                  return RT_ENGLISH[index];
  }
}
