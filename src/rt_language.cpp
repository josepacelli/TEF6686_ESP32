#include "rt_language.h"

const char* getRTByLanguage(int index, uint8_t language) {
  switch (language) {
    case PTY_LANG_PORTUGUESE:
    case PTY_LANG_RBDS_PORTUGUESE:
    case PTY_LANG_BR_PORTUGUESE:  return RT_PORTUGUESE[index];
    case PTY_LANG_SPANISH:
    case PTY_LANG_RBDS_SPANISH:
    case PTY_LANG_BR_SPANISH:     return RT_SPANISH[index];
    case PTY_LANG_ENGLISH:
    case PTY_LANG_RBDS:
    case PTY_LANG_BR_ENGLISH:
    default:                      return RT_ENGLISH[index];
  }
}
