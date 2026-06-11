#include "ps_language.h"

const char* getPSByLanguage(int index, uint8_t language) {
  switch (language) {
    case PTY_LANG_PORTUGUESE: return PS_PORTUGUESE[index];
    case PTY_LANG_SPANISH:    return PS_SPANISH[index];
    case PTY_LANG_ENGLISH:
    default:                  return PS_ENGLISH[index];
  }
}
