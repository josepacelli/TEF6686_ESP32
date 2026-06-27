#ifndef PTY_LANGUAGE_H
#define PTY_LANGUAGE_H

#include <stdint.h>

// Language selection enum
enum PTYLanguage {
  PTY_LANG_ENGLISH = 1,
  PTY_LANG_PORTUGUESE = 2,
  PTY_LANG_SPANISH = 3,
  PTY_LANG_RBDS = 4,
  PTY_LANG_RBDS_PORTUGUESE = 5,
  PTY_LANG_RBDS_SPANISH = 6,
  PTY_LANG_BR_ENGLISH = 7,
  PTY_LANG_BR_PORTUGUESE = 8,
  PTY_LANG_BR_SPANISH = 9
};

// Declare external PTY arrays
extern const char* const PTY_ENGLISH[];
extern const char* const PTY_PORTUGUESE[];
extern const char* const PTY_SPANISH[];

// Global language selection variable
extern uint8_t currentPTYLanguage;

// Function to get PTY text by language
const char* getPTYTextByLanguage(uint8_t ptyCode, uint8_t language);

// Function to set current language
void setPTYLanguage(uint8_t language);

#endif
