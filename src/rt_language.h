#ifndef RT_LANGUAGE_H
#define RT_LANGUAGE_H

#include <stdint.h>
#include "pty_language.h"

extern const char* const RT_ENGLISH[];
extern const char* const RT_PORTUGUESE[];
extern const char* const RT_SPANISH[];

const char* getRTByLanguage(int index, uint8_t language);

#endif
