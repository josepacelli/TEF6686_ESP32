#ifndef FREQUENCIA_H
#define FREQUENCIA_H

#include <cstdint>
#include <stdint.h>

extern const uint32_t allFrequencies[];
extern const int8_t   allPTYCodes[];
extern const int      FREQ_COUNT;

int8_t getPTYCodeForFreq(uint32_t freq_khz);

#endif // FREQUENCIA_H
