#include "arrows.h"

// Display frequency tuning arrows (up/down for manual tuning)
// Note: Uses 'direction' global from main sketch - call from TEF6686_ESP32.ino only
/*
void ShowFrequencyArrows(uint32_t freq, bool seeking) {
  if (seeking) {
    // Show directional arrow during seek
    if (direction == 1) {
      drawArrowUp(310, 80, 10, TFT_YELLOW);  // Seek up
    } else {
      drawArrowDown(310, 80, 10, TFT_YELLOW); // Seek down
    }
  }
}
*/

// Display menu navigation arrows
void ShowMenuArrows(int currentMenuItem, int totalItems) {
  // Up arrow if not at top
  if (currentMenuItem > 0) {
    drawTriangleUp(310, 50, 8, TFT_CYAN);
  }

  // Down arrow if not at bottom
  if (currentMenuItem < totalItems - 1) {
    drawTriangleDown(310, 150, 8, TFT_CYAN);
  }
}

// Display rotary encoder direction hint
void ShowEncoderArrows(bool rotateCW) {
  if (rotateCW) {
    drawArrowRight(290, 100, 10, TFT_GREEN);
  } else {
    drawArrowLeft(290, 100, 10, TFT_GREEN);
  }
}

// Display scroll indicators for long text
void ShowScrollArrows(bool canScrollUp, bool canScrollDown, int x, int y) {
  if (canScrollUp) {
    drawArrowUp(x, y - 5, 6, TFT_LIGHTGREY);
  }

  if (canScrollDown) {
    drawArrowDown(x, y + 100, 6, TFT_LIGHTGREY);
  }
}

// Display frequency increase/decrease arrows on sides of frequency display
void ShowFrequencyAdjustArrows(int freqX, int freqY) {
  drawArrowLeft(freqX - 30, freqY, 8, TFT_ORANGE);  // Decrease frequency
  drawArrowRight(freqX + 30, freqY, 8, TFT_ORANGE); // Increase frequency
}

// Display preset navigation arrows
void ShowPresetArrows(bool hasPrevious, bool hasNext, int x, int y) {
  if (hasPrevious) {
    drawArrowLeft(x - 20, y, 10, TFT_PURPLE);
  }

  if (hasNext) {
    drawArrowRight(x + 20, y, 10, TFT_PURPLE);
  }
}

// Display double arrows for fast tune
void ShowFastTuneArrows(bool tuning) {
  if (tuning) {
    drawDoubleArrowUp(160, 50, 12, TFT_RED);
  }
}

// Display clear status indicator with arrows
void ClearArrows(int x, int y, int width, int height) {
  tft.fillRect(x, y, width, height, TFT_BLACK);
}
