#ifndef PS_LANGUAGE_H
#define PS_LANGUAGE_H

#include <stdint.h>
#include "pty_language.h"

extern const char* const PS_ENGLISH[];
extern const char* const PS_PORTUGUESE[];
extern const char* const PS_SPANISH[];

const char* getPSByLanguage(int index, uint8_t language);

#endif
