#ifndef ARROWS_H
#define ARROWS_H

#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// Draw up arrow
void drawArrowUp(int x, int y, int size, uint32_t color) {
  int half = size / 2;
  tft.drawLine(x - half, y + half, x, y - half, color);
  tft.drawLine(x, y - half, x + half, y + half, color);
  tft.drawLine(x - half, y + half, x + half, y + half, color);
}

// Draw down arrow
void drawArrowDown(int x, int y, int size, uint32_t color) {
  int half = size / 2;
  tft.drawLine(x - half, y - half, x, y + half, color);
  tft.drawLine(x, y + half, x + half, y - half, color);
  tft.drawLine(x - half, y - half, x + half, y - half, color);
}

// Draw left arrow
void drawArrowLeft(int x, int y, int size, uint32_t color) {
  int half = size / 2;
  tft.drawLine(x + half, y - half, x - half, y, color);
  tft.drawLine(x - half, y, x + half, y + half, color);
  tft.drawLine(x + half, y - half, x + half, y + half, color);
}

// Draw right arrow
void drawArrowRight(int x, int y, int size, uint32_t color) {
  int half = size / 2;
  tft.drawLine(x - half, y - half, x + half, y, color);
  tft.drawLine(x + half, y, x - half, y + half, color);
  tft.drawLine(x - half, y - half, x - half, y + half, color);
}

// Draw double arrow up
void drawDoubleArrowUp(int x, int y, int size, uint32_t color) {
  int half = size / 2;
  // Top arrow
  tft.drawLine(x - half, y, x, y - half, color);
  tft.drawLine(x, y - half, x + half, y, color);
  // Bottom arrow
  tft.drawLine(x - half, y + half, x, y, color);
  tft.drawLine(x, y, x + half, y + half, color);
}

// Draw double arrow down
void drawDoubleArrowDown(int x, int y, int size, uint32_t color) {
  int half = size / 2;
  // Top arrow
  tft.drawLine(x - half, y - half, x, y, color);
  tft.drawLine(x, y, x + half, y - half, color);
  // Bottom arrow
  tft.drawLine(x - half, y, x, y + half, color);
  tft.drawLine(x, y + half, x + half, y, color);
}

// Draw triangle pointing up (filled)
void drawTriangleUp(int x, int y, int size, uint32_t color) {
  int half = size / 2;
  tft.fillTriangle(x - half, y + half, x, y - half, x + half, y + half, color);
}

// Draw triangle pointing down (filled)
void drawTriangleDown(int x, int y, int size, uint32_t color) {
  int half = size / 2;
  tft.fillTriangle(x - half, y - half, x, y + half, x + half, y - half, color);
}

// Draw triangle pointing left (filled)
void drawTriangleLeft(int x, int y, int size, uint32_t color) {
  int half = size / 2;
  tft.fillTriangle(x + half, y - half, x - half, y, x + half, y + half, color);
}

// Draw triangle pointing right (filled)
void drawTriangleRight(int x, int y, int size, uint32_t color) {
  int half = size / 2;
  tft.fillTriangle(x - half, y - half, x + half, y, x - half, y + half, color);
}

#endif // ARROWS_H
