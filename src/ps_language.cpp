#include "ps_language.h"

const char* getPSByLanguage(int index, uint8_t language) {
  switch (language) {
    case PTY_LANG_PORTUGUESE:
    case PTY_LANG_RBDS_PORTUGUESE:
    case PTY_LANG_BR_PORTUGUESE:  return PS_PORTUGUESE[index];
    case PTY_LANG_SPANISH:
    case PTY_LANG_RBDS_SPANISH:
    case PTY_LANG_BR_SPANISH:     return PS_SPANISH[index];
    case PTY_LANG_ENGLISH:
    case PTY_LANG_RBDS:
    case PTY_LANG_BR_ENGLISH:
    default:                      return PS_ENGLISH[index];
  }
}
