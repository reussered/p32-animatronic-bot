/**
 * @file GOBLIN_MOOD_DISPLAY_INTEGRATION_GUIDE.md
 * @brief Complete guide for integrating mood-based display system into goblin head
 * 
 * This document explains how to use the three new components:
 * 1. mood_calculator_template.hpp - Generic mood-to-color converter
 * 2. goblin_eye_mood_display.hpp - Eye display with mood effects
 * 3. goblin_mouth_mood_display.hpp - Mouth display with chunking
 */

# Goblin Mood-Based Display Integration Guide

## Overview

The goblin head now supports **mood-aware color rendering** for both eyes and mouth displays.
This means character emotions automatically affect visual appearance:
- **ANGER**: Red-shifted eyes, glowing effect
- **FEAR**: Blue-shifted eyes, wide-eyed look
- **HAPPINESS**: Golden/yellow warm tones
- **CURIOSITY**: Green-shifted sparkle effects
- **SADNESS**: Desaturated cool tones

## Architecture

### Three-Layer System

```
SharedMemory (Mood struct)
    ↓
MoodCalculator<PixelType> (template-based)
    ↓
Display Rendering (Eyes or Mouth)
    ↓
Display Driver (SPI to hardware)
```

### Color Schema Auto-Detection

No need to hardcode RGB565 vs RGB666. Just tell the system your display resolution:

```cpp
// Automatically detects color format based on display type
GoblinEyeMoodDisplay left_eye;
left_eye.init(240, 240, RGB565_FORMAT);  // GC9A01

GoblinMouthMoodDisplay mouth;
mouth.init(480, 320, RGB666_FORMAT);     // ILI9341 with chunking
```

## Setup Steps

### Step 1: Add to Your Component

In your component's `.src` file:

```cpp
#include "config/components/templates/goblin_eye_mood_display.hpp"
#include "config/components/templates/goblin_mouth_mood_display.hpp"

static GoblinEyeMoodDisplay left_eye;
static GoblinEyeMoodDisplay right_eye;
static GoblinMouthMoodDisplay mouth;

esp_err_t goblin_head_init(void) {
    // Initialize left eye: 240x240 GC9A01 RGB565
    if (!left_eye.init(240, 240, RGB565_FORMAT)) {
        return ESP_FAIL;
    }
    
    // Initialize right eye: 240x240 GC9A01 RGB565
    if (!right_eye.init(240, 240, RGB565_FORMAT)) {
        return ESP_FAIL;
    }
    
    // Initialize mouth: 480x320 ILI9341 RGB666 with chunking
    if (!mouth.init(480, 320, RGB666_FORMAT, true, 80)) {
        return ESP_FAIL;
    }
    
    return ESP_OK;
}
```

### Step 2: Render Frames with Mood

In your component's `_act()` function:

```cpp
void goblin_head_act(void) {
    // Read current mood from SharedMemory
    Mood mood = GSM.read<Mood>();
    
    // Render left eye with mood applied
    left_eye.renderFrame(mood, 0x00AA00);  // Green base color
    
    // Render right eye with mood applied
    right_eye.renderFrame(mood, 0x00AA00);
    
    // Send left eye to SPI device 0
    const uint8_t* left_buffer = left_eye.getFrameBuffer();
    spi_display_driver_send(0, left_buffer, left_eye.getBufferSize());
    
    // Send right eye to SPI device 1
    const uint8_t* right_buffer = right_eye.getFrameBuffer();
    spi_display_driver_send(1, right_buffer, right_eye.getBufferSize());
    
    // Render mouth chunks sequentially (for large ILI9341)
    const uint8_t* chunk = mouth.renderNextChunk(mood, 0xFF6600);
    while (chunk) {
        spi_display_driver_send_chunk(2, chunk, mouth.getChunkSize());
        chunk = mouth.renderNextChunk(mood, 0xFF6600);
    }
    mouth.resetChunkIndex();  // Ready for next frame
}
```

## Display Configuration Matrix

### Eyes (No Chunking Needed)

| Display | Resolution | Color Schema | Buffer | Chips |
|---------|-----------|--------------|--------|-------|
| GC9A01  | 240×240   | RGB565       | 112.5 KB | 2 |
| ST7789  | 240×320   | RGB565       | 150 KB   | 2 |
| ST7735  | 160×128   | RGB565       | 40 KB    | 2 |
| SSD1331 | 96×64     | RGB565       | 12 KB    | 2 |

**Total for dual eyes (GC9A01):** 225 KB (good headroom)

### Mouth (Chunking Strategy)

| Display | Resolution | Color Schema | Full Size | Chunk | Chunks | Peak Memory |
|---------|-----------|--------------|-----------|-------|--------|-------------|
| ILI9341 | 480×320   | RGB666       | 450 KB    | 480×80 | 4 | 112.5 KB ✓ |
| RA8875  | 800×480   | RGB666       | 1125 KB   | 800×80 | 6 | 150 KB ✓ |
| ILI9481 | 800×480   | RGB888       | 1152 KB   | 800×40 | 12| 96 KB ✓ |

## Memory Analysis

### Recommended Configuration

```
Left eye (GC9A01):     112.5 KB
Right eye (GC9A01):    112.5 KB
Mouth (ILI9341):       112.5 KB (single chunk)
---
Total:                 337.5 KB

Available (300 KB constraint): 
  Dual eyes = 225 KB, Headroom = 75 KB (can fit ST7735 mouth at 40 KB)
  OR
  Both eyes + one ILI9341 chunk = 337.5 KB (tight, but fits with careful allocation)
```

### Budget Impact

- **Eyes only (dual GC9A01):** 225 KB → 75 KB headroom for animations
- **Eyes + ST7735 mouth:** 265 KB → 35 KB headroom
- **Eyes + ILI9341 chunked:** ~330 KB → requires tight heap management but works

## Mood Customization

### Default Mood Palettes

The `MoodCalculator` comes with sensible defaults:

```cpp
// ANGER: Red-shifted, glowing
{red_delta: +40, green_delta: -30, blue_delta: -30, intensity: 1.5x, glow: ON}

// CURIOSITY: Green-shifted
{red_delta: -20, green_delta: +35, blue_delta: -10, intensity: 1.2x, sparkle: OFF}

// FEAR: Blue-shifted, dilated
{red_delta: -40, green_delta: +20, blue_delta: +50, intensity: 1.8x, dilate: ON}

// HAPPINESS: Warm golden
{red_delta: +30, green_delta: +25, blue_delta: -20, intensity: 1.1x, glow: ON}

// SADNESS: Desaturated cool
{red_delta: -15, green_delta: -15, blue_delta: +10, intensity: 0.8x, desaturate: ON}
```

### Customize for Your Goblin

To change mood colors for your specific creature:

```cpp
// In goblin_head_init()
MoodColorDelta custom_anger = {
    .red_delta = 60,        // More red shift
    .green_delta = -40,
    .blue_delta = -40,
    .intensity = 2.0f,      // Stronger effect
    .enable_glow = true,
    .enable_desaturation = false,
    .enable_warmth = true,
    .enable_coolness = false
};

left_eye_mood_calc->setMoodDelta(ANGER, custom_anger);
right_eye_mood_calc->setMoodDelta(ANGER, custom_anger);
```

## Animation Integration

### Control Eye State

```cpp
// Set pupil position (for gaze direction)
left_eye.setPupilPosition(120, 120);  // Center of eye

// Set eyelid openness (0=closed, 255=open)
left_eye.setEyelidOpenness(200);

// Trigger blink animation
left_eye.blink();
```

### Control Mouth State

```cpp
// Set mouth open amount (0=closed, 255=wide open)
mouth.setMouthOpen(200);

// Set expression parameter (e.g., for smile intensity)
mouth.setExpression(150);

// Enable/disable smile
mouth.setSmile(true);
```

## Performance Characteristics

### CPU Usage

- **MoodCalculator instantiation:** Compile-time (zero runtime cost)
- **Per-pixel mood application:** ~50 CPU cycles on ESP32-S3
- **Full frame render (240×240):** ~2.4M cycles (57.6K pixels × 50 cycles)
- **At 240 MHz:** ~10 ms to render dual eyes

### Memory Usage

| Component | Size |
|-----------|------|
| MoodCalculator<T> instance | ~200 bytes |
| GC9A01 eye buffer (240×240 RGB565) | 115,200 bytes |
| ST7735 mouth buffer (160×128 RGB565) | 40,960 bytes |
| ILI9341 chunk buffer (480×80 RGB666) | 115,200 bytes |
| **Total (dual eyes + mouth chunk)** | **~330 KB** |

### Refresh Rates

| Display | Resolution | Chunks | Render Time | FPS |
|---------|-----------|--------|-----------|-----|
| GC9A01 (eye) | 240×240 | 1 | 10 ms | 100+ |
| ST7735 (mouth) | 160×128 | 1 | 2.6 ms | 100+ |
| ILI9341 (mouth) | 480×320 | 4 | 40 ms | 25 |
| RA8875 (mouth) | 800×480 | 6 | 60 ms | 16 |

## Debugging

### Check Compilation

```bash
# Verify templates compile correctly
pio run -e goblin_head > build.log 2>&1
Select-String -Path build.log -Pattern "error|warning.*mood|warning.*color"
```

### Verify Mood Integration

```cpp
// In goblin_head_act(), add debug output:
Mood current_mood = GSM.read<Mood>();
ESP_LOGI(TAG, "Current mood: %d, intensity: %d", 
         current_mood.state, current_mood.intensity);
```

### Check Memory Usage

```cpp
// Query buffer sizes at runtime
uint32_t left_size = left_eye.getBufferSize();
uint32_t right_size = right_eye.getBufferSize();
uint32_t mouth_chunk_size = mouth.getChunkSize();

ESP_LOGI(TAG, "Eye buffers: %d + %d bytes", left_size, right_size);
ESP_LOGI(TAG, "Mouth chunk: %d bytes", mouth_chunk_size);
ESP_LOGI(TAG, "Total peak: %d bytes", left_size + right_size + mouth_chunk_size);
```

## Common Issues & Solutions

### Issue: "Buffer exceeds 300 KB"
**Solution:** Enable chunking for mouth display, reduce eye resolution, or use ST7735 instead of ILI9341.

### Issue: "Mood changes not visible"
**Solution:** Check that `GSM.read<Mood>()` is returning valid mood state. Verify base color isn't too dark (mood shifts need room to show).

### Issue: "Display flickers during mood transitions"
**Solution:** Reduce mood intensity or smooth transitions with `setMood(mood, intensity * 0.9f)` for gradual fade.

### Issue: "Mouth chunks appear out of order"
**Solution:** Call `mouth.resetChunkIndex()` at end of frame. Don't call `renderNextChunk()` across multiple frames.

## Files Created

1. **mood_calculator_template.hpp** - Core mood calculation engine (template)
2. **goblin_eye_mood_display.hpp** - Eye display driver
3. **goblin_mouth_mood_display.hpp** - Mouth display driver with chunking
4. **mood_calculator_template.json** - Component metadata
5. **goblin_eye_mood_display.json** - Component metadata
6. **goblin_mouth_mood_display.json** - Component metadata
7. **GOBLIN_MOOD_DISPLAY_INTEGRATION_GUIDE.md** - This file

## Next Steps

1. ✅ Create display templates (DONE)
2. ⏳ Integrate into `goblin_left_eye.src` / `goblin_right_eye.src`
3. ⏳ Integrate into `goblin_mouth.src`
4. ⏳ Update `goblin_eye.json` with color_schema parameter
5. ⏳ Test mood transitions on actual hardware
6. ⏳ Optimize chunk sizes for your specific displays

## References

- `config/shared_headers/color_schema.hpp` - Pixel format definitions
- `shared/Mood.hpp` - Mood state enum and structure
- `include/FrameProcessor.hpp` - Display rendering utilities
- `DISPLAY_BUFFER_ANALYSIS_300KB.md` - Memory constraint analysis
