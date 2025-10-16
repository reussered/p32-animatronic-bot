# Eye Animation Implementation for GC9A01 Displays

## Overview

This document describes the complete eye animation system for the P32 animatronic goblin, rendering pixel-perfect animated eyes to GC9A01 round displays.

## Architecture

### Three-Layer System

```
┌─────────────────────────────────────────────────────────┐
│ Layer 3: Animation Control (goblin_eye_left.cpp)       │
│ - Proximity-based animation switching                   │
│ - Mood-driven expression changes                        │
│ - Animation state machine                               │
└─────────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────┐
│ Layer 2: Animation Data (p32_eye_display.cpp)          │
│ - Keyframe definitions (timing, pupil, openness)       │
│ - Keyframe interpolation                                │
│ - Animation playback engine                             │
└─────────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────┐
│ Layer 1: Graphics Rendering (p32_eye_graphics.cpp)     │
│ - Pixel-level eye rendering (sclera, iris, pupil)      │
│ - Eyelid overlay based on openness                      │
│ - Expression overlays (angry, happy, surprised)         │
│ - RGB565 framebuffer generation                         │
└─────────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────┐
│ Layer 0: SPI Hardware (GC9A01 driver)                  │
│ - SPI bus initialization (shared)                       │
│ - Per-display CS pin control                            │
│ - Framebuffer transmission                              │
└─────────────────────────────────────────────────────────┘
```

## Animation Keyframes

### Keyframe Structure

```c
typedef struct {
    uint32_t time_ms;           // Time in animation
    float pupil_size;           // 0.0-1.0 (small to dilated)
    float eye_openness;         // 0.0-1.0 (closed to wide open)
    uint32_t color;             // RGB color 0xRRGGBB
    eye_expression_t expression; // Eye shape/expression
} eye_keyframe_t;
```

### Built-in Animations

#### 1. Goblin Blink (800ms, non-looping)

```c
goblin_blink_animation = {
    .duration_ms = 800,
    .loop = false,
    .keyframes = {
        {0,   0.7f, 0.8f, 0x4444ff, EYE_NORMAL},      // Open
        {200, 0.7f, 0.4f, 0x4444ff, EYE_NORMAL},      // Half closed
        {400, 0.7f, 0.0f, 0x4444ff, EYE_NORMAL},      // Closed
        {800, 0.7f, 0.8f, 0x4444ff, EYE_NORMAL}       // Open
    }
};
```

**Usage**: Idle state, periodic blinking

#### 2. Angry Stare (2000ms, looping)

```c
goblin_angry_stare_animation = {
    .duration_ms = 2000,
    .loop = true,
    .keyframes = {
        {0,    0.6f, 0.4f, 0xff4444, EYE_ANGRY_NARROW}, // Narrow angry
        {1000, 0.8f, 0.3f, 0xff2222, EYE_ANGRY_NARROW}, // Intense
        {2000, 0.6f, 0.4f, 0xff4444, EYE_ANGRY_NARROW}  // Back
    }
};
```

**Usage**: Threat response, aggressive mood

#### 3. Curious Look (1500ms, non-looping)

```c
goblin_curious_look_animation = {
    .duration_ms = 1500,
    .loop = false,
    .keyframes = {
        {0,    0.5f, 0.9f, 0x44ff44, EYE_SURPRISED_WIDE}, // Wide open
        {750,  0.7f, 0.7f, 0x44ff44, EYE_NORMAL},         // Normal
        {1500, 0.5f, 0.9f, 0x44ff44, EYE_SURPRISED_WIDE}  // Wide again
    }
};
```

**Usage**: Face detected within 30cm, investigating objects

## Graphics Rendering

### Eye Components

```
         ╔════════════════════════════════╗
         ║      Eyelid (Skin Tone)        ║
         ║  ┌──────────────────────────┐  ║
         ║  │  Sclera (White)          │  ║
         ║  │    ┌──────────────┐      │  ║
         ║  │    │ Iris (Green) │      │  ║
         ║  │    │   ┌──────┐   │      │  ║
         ║  │    │   │Pupil │   │      │  ║
         ║  │    │   │(Black│   │      │  ║
         ║  │    │   └──────┘   │      │  ║
         ║  │    └──────────────┘      │  ║
         ║  └──────────────────────────┘  ║
         ║      Eyelid (Skin Tone)        ║
         ╚════════════════════════════════╝
```

### Rendering Pipeline

1. **Clear framebuffer** to eyelid color (background)
2. **Draw sclera** (white circle, 80% of display)
3. **Draw iris** (colored circle, 40% of eyeball)
4. **Draw pupil** (black circle, size from keyframe)
5. **Apply eyelid overlay** (based on eye_openness)
6. **Apply expression overlay** (angry squint, etc.)

### RGB565 Pixel Format

- 16 bits per pixel
- 5 bits red, 6 bits green, 5 bits blue
- Total framebuffer: 240×240×2 = 115,200 bytes (112.5 KB)

```c
#define RGB565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))
```

### Color Palette

```c
COLOR_SCLERA_WHITE   = RGB565(255, 255, 255)  // Eye white
COLOR_IRIS_GOBLIN    = RGB565(80, 255, 80)    // Green goblin eye
COLOR_PUPIL_BLACK    = RGB565(0, 0, 0)        // Black pupil
COLOR_IRIS_ANGRY     = RGB565(255, 80, 80)    // Red angry eye
COLOR_IRIS_CURIOUS   = RGB565(80, 200, 255)   // Blue curious eye
COLOR_EYELID_SKIN    = RGB565(120, 150, 100)  // Goblin skin
```

## Animation State Machine

### Proximity-Based Switching

```c
// Read distance from shared state (HC-SR04 sensor)
uint8_t distance = g_shared_state.distance_cm;

if (distance < 30) {
    // Face CLOSE → CURIOUS
    eye_display_start_animation(&display, &goblin_curious_look_animation);
} 
else if (distance > 50) {
    // Face FAR → IDLE BLINK
    eye_display_start_animation(&display, &goblin_blink_animation);
}
```

### Animation Timing

- **Component hitCount**: 50 loops (5 seconds @ 10 Hz)
- **Blink animation**: 800ms (plays 6× per component execution)
- **Curious animation**: 1500ms (plays 3× per component execution)
- **Frame interpolation**: Smooth transitions between keyframes

## SPI Display Communication

### GC9A01 Initialization Sequence

```c
gc9a01_send_cmd(0xEF);  // Inter register enable
gc9a01_send_cmd(0xEB);
gc9a01_send_data(0x14);

gc9a01_send_cmd(0xFE);  // Inter register enable
gc9a01_send_cmd(0xEF);

gc9a01_send_cmd(0x36);  // Memory access control
gc9a01_send_data(0x48); // Row/column exchange, BGR

gc9a01_send_cmd(0x3A);  // Pixel format
gc9a01_send_data(0x05); // 16-bit RGB565

gc9a01_send_cmd(0x11);  // Sleep out
vTaskDelay(120ms);

gc9a01_send_cmd(0x29);  // Display on
```

### Framebuffer Transmission

```c
// Set window to full screen (240×240)
gc9a01_send_cmd(0x2A);  // Column address
gc9a01_send_data(0x00); gc9a01_send_data(0x00);
gc9a01_send_data(0x00); gc9a01_send_data(0xEF);

gc9a01_send_cmd(0x2B);  // Row address  
gc9a01_send_data(0x00); gc9a01_send_data(0x00);
gc9a01_send_data(0x00); gc9a01_send_data(0xEF);

gc9a01_send_cmd(0x2C);  // Memory write

// Send framebuffer in chunks
const int CHUNK_SIZE = 4096;  // pixels per chunk
for (int offset = 0; offset < 57600; offset += CHUNK_SIZE) {
    spi_transaction_t trans = {
        .length = pixels_to_send * 16,  // bits
        .tx_buffer = &framebuffer[offset]
    };
    spi_device_polling_transmit(spi_display, &trans);
}
```

### Performance

- **SPI Clock**: 10 MHz
- **Frame size**: 115,200 bytes
- **Transfer time**: ~92ms per frame
- **Effective FPS**: ~10.8 FPS (limited by SPI bandwidth)
- **Chunk size**: 4096 pixels (8192 bytes) for smooth transfers

## Memory Usage

### Per Eye

- **Framebuffer**: 115,200 bytes (112.5 KB)
- **Display state**: 256 bytes
- **Graphics config**: 64 bytes
- **Animation data**: Static (shared across eyes)
- **Total per eye**: ~113 KB

### Both Eyes

- **Total RAM**: ~226 KB
- **ESP32-S3 has**: 512 KB SRAM + 2 MB PSRAM
- **Headroom**: Plenty (PSRAM can be used for framebuffers if needed)

## Adding New Animations

### 1. Define Keyframes

```c
eye_animation_t my_new_animation = {
    .name = "my_animation",
    .duration_ms = 1000,
    .loop = true,
    .keyframe_count = 4,
    .keyframes = {
        {0,    pupil_size, openness, color, expression},
        {250,  pupil_size, openness, color, expression},
        {500,  pupil_size, openness, color, expression},
        {1000, pupil_size, openness, color, expression}
    }
};
```

### 2. Add to Animation Library

```c
// In p32_eye_display.cpp
extern eye_animation_t my_new_animation;
```

### 3. Trigger from Component

```c
// In goblin_eye_left_act()
eye_display_start_animation(&left_eye_display, &my_new_animation);
```

## Expression Types

### EYE_NORMAL
- Standard relaxed eye
- No eyelid overlay

### EYE_ANGRY_NARROW
- Diagonal eyelid overlay (furrowed brow)
- Constricted pupil
- Red iris tint

### EYE_HAPPY_SQUINT
- Top and bottom eyelid partial close
- Normal pupil
- Warm color tint

### EYE_SURPRISED_WIDE
- No eyelid overlay
- Dilated pupil
- Bright color

### EYE_SLEEPY_DROOPY
- Heavy top eyelid
- Small pupil
- Muted colors

## Testing

### SIMPLE_TEST Mode

When compiled with `-DSIMPLE_TEST`:
- ASCII art rendering to serial console
- Animation data sent to PC via HTTP
- No SPI hardware required

### Hardware Mode

When compiled without SIMPLE_TEST:
- Full pixel rendering to GC9A01 displays
- Real-time animation playback
- Proximity-based animation switching

### Validation

1. **Static Test**: Render single frame, verify colors
2. **Blink Test**: Play blink animation, check smooth closing/opening
3. **Proximity Test**: Move hand near sensor, verify animation change
4. **Performance Test**: Monitor frame timing, check for dropped frames

## Troubleshooting

### Black Screen
- Check SPI wiring (MOSI=13, CLK=12, CS=15/16)
- Verify GC9A01 initialization sequence executed
- Check framebuffer allocation successful

### Flickering
- Reduce SPI clock speed (currently 10 MHz)
- Increase chunk size (currently 4096 pixels)
- Check for memory allocation issues

### Slow Animation
- Verify hitCount is 50 (not 500)
- Check animation duration_ms is reasonable
- Monitor ESP32 task scheduling

### Wrong Colors
- Verify RGB565 conversion correct
- Check byte order (MSB first for GC9A01)
- Test with solid color first

## Future Enhancements

1. **More Animations**
   - Sleepy (slow blink with droopy lids)
   - Surprised (rapid pupil dilation)
   - Tracking (pupil follows object)

2. **Performance**
   - DMA-based SPI transfers
   - Double-buffered rendering
   - Partial screen updates

3. **Effects**
   - Glow effect around iris
   - Bloodshot veins when tired
   - Sparkle reflection in pupil

4. **Interactivity**
   - Eye tracking with camera
   - Mood-based color shifts
   - Blink rate tied to arousal level

## Related Documentation

- `docs/COMPONENT-FUNCTION-SIGNATURES.md` - NO ARGUMENTS pattern
- `docs/GLOBAL-SHARED-MEMORY-API.md` - Shared state access
- `docs/FACE-TRACKING-INTEGRATION.md` - Proximity detection
- `VERIFIED-WIRING.md` - Hardware pin assignments
