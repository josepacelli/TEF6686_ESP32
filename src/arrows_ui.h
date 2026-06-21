#ifndef ARROWS_UI_H
#define ARROWS_UI_H

#include <Arduino.h>

// Frequency tuning arrows
void ShowFrequencyArrows(uint32_t freq, bool seeking);

// Menu navigation arrows
void ShowMenuArrows(int currentMenuItem, int totalItems);

// Rotary encoder direction hint
void ShowEncoderArrows(bool rotateCW);

// Scroll indicators for long text
void ShowScrollArrows(bool canScrollUp, bool canScrollDown, int x, int y);

// Frequency increase/decrease arrows on sides
void ShowFrequencyAdjustArrows(int freqX, int freqY);

// Preset navigation arrows
void ShowPresetArrows(bool hasPrevious, bool hasNext, int x, int y);

// Double arrows for fast tune
void ShowFastTuneArrows(bool tuning);

// Clear arrow display area
void ClearArrows(int x, int y, int width, int height);

#endif // ARROWS_UI_H
