# Display Buffer Architecture - CRITICAL DESIGN PATTERN

## Problem
Eye components (gle/gre) were hardcoding display buffer sizes (240x240), creating tight coupling that would break when reusing code with different sized displays.

## Solution
Display component provides simple interface for buffer allocation and size queries. Eye components request what they need without knowing specific display dimensions.

## Display Component Interface

Every display component (gc9a01, oled128x64, etc.) must provide these three functions:

```cpp
uint8_t* getBuffer() {
    return malloc(57600);  // Or whatever this display's frame size is
}

uint32_t getFrameSize() {
    return 240 * 240;  // Total pixels for this display
}

uint32_t getFrameRowSize() {
    return 240;  // Pixels per row for this display
}
```

## Eye Component Usage

Eye components use the interface without knowing display specifics:

```cpp
void goblin_left_eye_init() {
    uint8_t* my_buffer = getBuffer();           // Get our own buffer
    uint32_t total_pixels = getFrameSize();     // For loop bounds
    uint32_t row_width = getFrameRowSize();     // For coordinate math
    
    // Now we can work with any display size
    uint32_t center_x = row_width / 2;
    for (uint32_t i = 0; i < total_pixels; i++) {
        uint32_t x = i % row_width;
        uint32_t y = i / row_width;
        // ... eyeball rendering math
    }
}
```

## Key Benefits

1. **Zero Coupling**: Eye components don't know display dimensions
2. **Automatic Scaling**: Same eye code works with 128x64, 240x240, 320x240, etc.
3. **Multiple Buffers**: Each component gets its own malloc'd buffer
4. **Simple Interface**: Just three functions, no complexity
5. **Reusable Components**: Eye logic works across all display types

## Example Scaling

### GC9A01 (240x240)
```cpp
uint32_t getFrameSize() { return 57600; }
uint32_t getFrameRowSize() { return 240; }
```

### OLED (128x64)  
```cpp
uint32_t getFrameSize() { return 8192; }
uint32_t getFrameRowSize() { return 128; }
```

### TFT (320x240)
```cpp
uint32_t getFrameSize() { return 76800; }
uint32_t getFrameRowSize() { return 320; }
```

**Same eye component code works with all three!**

## Code Generation Integration

The creature generation script concatenates component files into creature_functions.cpp/hpp. Since all components are in the same file, eye components can directly call display functions without additional includes or linking.

## Critical Rules

1. **Display components MUST provide all three functions**
2. **Eye components MUST use the interface, never hardcode sizes**
3. **Each component calls getBuffer() to get its own unique buffer**
4. **Display component doesn't know or care what the buffers are used for**

---
**This architecture prevents "major uglies" when reusing code with different display hardware.**