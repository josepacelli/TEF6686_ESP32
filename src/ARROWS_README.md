# Arrow Graphics Functions

## Overview
Arrow graphics for UI navigation and status indicators on TFT display.

## Files
- `arrows.h` - Low-level arrow drawing primitives
- `arrows_ui.h/cpp` - High-level UI arrow functions

## Basic Usage

### Include in main sketch
```cpp
#include "src/arrows.h"
#include "src/arrows_ui.h"
```

## Arrow Primitives (arrows.h)

### Line-based arrows (8 directions)
```cpp
drawArrowUp(x, y, size, color);
drawArrowDown(x, y, size, color);
drawArrowLeft(x, y, size, color);
drawArrowRight(x, y, size, color);
drawDoubleArrowUp(x, y, size, color);
drawDoubleArrowDown(x, y, size, color);
```

### Filled triangles
```cpp
drawTriangleUp(x, y, size, color);
drawTriangleDown(x, y, size, color);
drawTriangleLeft(x, y, size, color);
drawTriangleRight(x, y, size, color);
```

### Parameters
- `x, y` - screen coordinates
- `size` - arrow size in pixels (10-15 typical)
- `color` - TFT color constant (TFT_RED, TFT_GREEN, etc.)

## High-Level UI Functions (arrows_ui.h)

### Frequency control
```cpp
ShowFrequencyArrows(freq, seeking);      // Show up/down during seek
ShowFrequencyAdjustArrows(freqX, freqY); // Left/right to adjust freq
ShowFastTuneArrows(tuning);              // Double arrows for fast tune
```

### Menu navigation
```cpp
ShowMenuArrows(currentItem, totalItems); // Up/down arrows for menu
```

### Text scrolling
```cpp
ShowScrollArrows(canUp, canDown, x, y); // Scroll indicators
```

### Preset selection
```cpp
ShowPresetArrows(hasPrev, hasNext, x, y); // Previous/next presets
```

### Encoder feedback
```cpp
ShowEncoderArrows(rotateCW); // Visual rotation direction
```

### Cleanup
```cpp
ClearArrows(x, y, width, height); // Clear arrow area
```

## Example Integration

In `ShowSignalLevel()` or similar display functions:
```cpp
void ShowSignalLevel() {
  // ... existing code ...
  
  // Show tuning arrows
  if (menu == false) {
    ShowFrequencyAdjustArrows(160, 80);  // At frequency display
  }
}
```

In menu code:
```cpp
void ShowMenu() {
  // ... render menu items ...
  
  // Show navigation hints
  ShowMenuArrows(selectedItem, menuItemCount);
}
```

## Color Constants
- `TFT_RED` - Red arrows
- `TFT_GREEN` - Green arrows
- `TFT_BLUE` - Blue arrows
- `TFT_YELLOW` - Yellow arrows
- `TFT_CYAN` - Cyan arrows
- `TFT_MAGENTA` - Magenta arrows
- `TFT_ORANGE` - Orange arrows
- `TFT_PURPLE` - Purple arrows
- `TFT_WHITE` - White arrows
- `TFT_LIGHTGREY` - Light grey (subtle)
- `TFT_DARKGREY` - Dark grey (very subtle)

## Size Recommendations
- Small indicators: 6-8 pixels
- Menu arrows: 8-10 pixels
- Large indicators: 12-15 pixels
- Double arrows: 12-15 pixels
