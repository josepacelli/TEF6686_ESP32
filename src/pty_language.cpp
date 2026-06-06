#include "pty_language.h"

// Global language variable - default to English
uint8_t currentPTYLanguage = PTY_LANG_ENGLISH;

// Set current language
void setPTYLanguage(uint8_t language) {
  if (language >= PTY_LANG_ENGLISH && language <= PTY_LANG_SPANISH) {
    currentPTYLanguage = language;
  }
}

// Get PTY text by language
const char* getPTYTextByLanguage(uint8_t ptyCode, uint8_t language) {
  if (ptyCode >= 0 && ptyCode < 32) {
    switch(language) {
      case PTY_LANG_PORTUGUESE:
        return PTY_PORTUGUESE[ptyCode];
      case PTY_LANG_SPANISH:
        return PTY_SPANISH[ptyCode];
      case PTY_LANG_ENGLISH:
      default:
        return PTY_ENGLISH[ptyCode];
    }
  }
  return "";
}
