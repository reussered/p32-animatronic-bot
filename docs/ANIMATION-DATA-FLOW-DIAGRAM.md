# Animation Data Flow Diagram

## Complete System Architecture

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           USER INTERACTION                              │
│                                                                         │
│     [Hand approaches sensor] → [HC-SR04 measures distance]             │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                    COMPONENT: goblin_nose.cpp                           │
│                    hitCount: 200 (every 20 seconds)                     │
│                                                                         │
│  void goblin_nose_act(void) {                                          │
│      uint8_t distance_cm = measure_hc_sr04();                          │
│      g_shared_state.distance_cm = distance_cm;  // WRITE to shared mem │
│  }                                                                      │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                    SHARED MEMORY (C++ Classes)                          │
│                                                                         │
│  p32_shared_state_t g_shared_state = {                                 │
│      .distance_cm = 25,          // ← Updated by nose sensor           │
│      .mood = CURIOUS,                                                   │
│      .temperature_c = 230,                                              │
│      // ... other state                                                 │
│  };                                                                     │
│                                                                         │
│  Synchronized across ESP32 subsystems via ESP-NOW mesh                  │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                    COMPONENT: goblin_eye_left.cpp                       │
│                    hitCount: 50 (every 5 seconds)                       │
│                                                                         │
│  void goblin_eye_left_act(void) {                                      │
│      // READ from shared memory                                         │
│      uint8_t distance = g_shared_state.distance_cm;                    │
│                                                                         │
│      // DECISION: Switch animation based on proximity                   │
│      if (distance < 30 && current != curious) {                        │
│          eye_display_start_animation(&display, &goblin_curious);       │
│          current_animation = &goblin_curious_look_animation;           │
│      }                                                                  │
│      else if (distance > 50 && current != blink) {                     │
│          eye_display_start_animation(&display, &goblin_blink);         │
│          current_animation = &goblin_blink_animation;                  │
│      }                                                                  │
│                                                                         │
│      // UPDATE: Calculate current frame                                 │
│      eye_display_update(&display, current_time_ms);                    │
│                                                                         │
│      // RENDER: Generate pixel graphics                                 │
│      eye_graphics_render_frame(framebuffer, &config, &current_frame);  │
│                                                                         │
│      // TRANSMIT: Send to display                                       │
│      spi_transmit_framebuffer_to_gc9a01(framebuffer);                  │
│  }                                                                      │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                    ANIMATION ENGINE (eye_display_update)                │
│                                                                         │
│  Input: current_animation = &goblin_curious_look_animation             │
│         elapsed_time = 750ms                                            │
│                                                                         │
│  Animation Definition:                                                  │
│  ┌─────────────────────────────────────────────────────────────────┐  │
│  │ goblin_curious_look_animation {                                  │  │
│  │   duration_ms: 1500                                              │  │
│  │   loop: false                                                    │  │
│  │   keyframes: [                                                   │  │
│  │     {0ms,    pupil:0.5, openness:0.9, color:0x44ff44, WIDE}    │  │
│  │     {750ms,  pupil:0.7, openness:0.7, color:0x44ff44, NORMAL}  │  │
│  │     {1500ms, pupil:0.5, openness:0.9, color:0x44ff44, WIDE}    │  │
│  │   ]                                                              │  │
│  │ }                                                                │  │
│  └─────────────────────────────────────────────────────────────────┘  │
│                                                                         │
│  Interpolation Logic:                                                   │
│  ┌─────────────────────────────────────────────────────────────────┐  │
│  │ Keyframe 1 (750ms): pupil=0.7, openness=0.7                     │  │
│  │ Keyframe 2 (1500ms): pupil=0.5, openness=0.9                    │  │
│  │                                                                  │  │
│  │ Current time: 750ms (exactly at keyframe 1)                     │  │
│  │ Interpolation factor: t = 0.0                                   │  │
│  │                                                                  │  │
│  │ Result frame:                                                    │  │
│  │   current_frame.pupil_size = 0.7                                │  │
│  │   current_frame.eye_openness = 0.7                              │  │
│  │   current_frame.color = 0x44ff44 (green-blue)                   │  │
│  │   current_frame.expression = EYE_NORMAL                         │  │
│  └─────────────────────────────────────────────────────────────────┘  │
│                                                                         │
│  Output: current_frame with interpolated values                        │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                    GRAPHICS RENDERER (eye_graphics_render_frame)        │
│                                                                         │
│  Input: current_frame {                                                │
│           pupil_size: 0.7                                              │
│           eye_openness: 0.7                                            │
│           color: 0x44ff44                                              │
│           expression: EYE_NORMAL                                       │
│         }                                                               │
│                                                                         │
│  Rendering Pipeline:                                                    │
│  ┌─────────────────────────────────────────────────────────────────┐  │
│  │ STEP 1: Clear framebuffer to eyelid color (goblin skin)         │  │
│  │   memset(framebuffer, COLOR_EYELID_SKIN, 115200 bytes)          │  │
│  │                                                                  │  │
│  │ STEP 2: Draw sclera (white circle)                              │  │
│  │   radius = 0.8 * display_width / 2 = 96 pixels                  │  │
│  │   fill_circle(120, 120, 96, COLOR_SCLERA_WHITE)                 │  │
│  │                                                                  │  │
│  │ STEP 3: Draw iris (green-blue circle from keyframe color)       │  │
│  │   radius = 0.4 * eyeball_radius = 38 pixels                     │  │
│  │   color = RGB565(0x44, 0xff, 0x44) = green-blue                 │  │
│  │   fill_circle(120, 120, 38, iris_color)                         │  │
│  │                                                                  │  │
│  │ STEP 4: Draw pupil (black circle, size from frame)              │  │
│  │   pupil_ratio = 0.2 + (0.6 - 0.2) * 0.7 = 0.48                  │  │
│  │   radius = 0.48 * iris_radius = 18 pixels                       │  │
│  │   fill_circle(120, 120, 18, COLOR_PUPIL_BLACK)                  │  │
│  │                                                                  │  │
│  │ STEP 5: Draw eyelids (based on openness = 0.7)                  │  │
│  │   vertical_range = 192 pixels (eyeball diameter)                │  │
│  │   coverage = (1.0 - 0.7) * 192 / 2 = 28 pixels                  │  │
│  │   top_eyelid_y = 24 + 28 = 52                                   │  │
│  │   bottom_eyelid_y = 216 - 28 = 188                              │  │
│  │   fill_rect(0, 0, 240, 52, COLOR_EYELID_SKIN)      // Top       │  │
│  │   fill_rect(0, 188, 240, 52, COLOR_EYELID_SKIN)    // Bottom    │  │
│  │                                                                  │  │
│  │ STEP 6: Apply expression overlay (EYE_NORMAL = none)            │  │
│  │   // No additional overlay for normal expression                │  │
│  └─────────────────────────────────────────────────────────────────┘  │
│                                                                         │
│  Output: 240×240 RGB565 framebuffer (115,200 bytes)                    │
│                                                                         │
│  Visual Result:                                                         │
│  ┌────────────────────────────────────┐                                │
│  │ ██████████████████████████████████ │ ← Top eyelid (skin)            │
│  │ ██████████████████████████████████ │                                │
│  │ ██████████████████████████████████ │                                │
│  │     ○○○○○○○○○○○○○○○○○○○○○○         │ ← Sclera (white)               │
│  │   ○○○○○○○○○○○○○○○○○○○○○○○○○       │                                │
│  │  ○○○○○     ●●●●●     ○○○○○○       │ ← Iris (green-blue)            │
│  │ ○○○○○○     ●●●●●     ○○○○○○○      │ ← Pupil (black)                │
│  │  ○○○○○     ●●●●●     ○○○○○○       │                                │
│  │   ○○○○○○○○○○○○○○○○○○○○○○○○○       │                                │
│  │     ○○○○○○○○○○○○○○○○○○○○○○         │                                │
│  │ ██████████████████████████████████ │ ← Bottom eyelid (skin)         │
│  │ ██████████████████████████████████ │                                │
│  └────────────────────────────────────┘                                │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                    SPI TRANSMISSION (chunked)                           │
│                                                                         │
│  Protocol: GC9A01 via SPI2_HOST                                        │
│  Clock: 10 MHz                                                          │
│  Mode: Mode 0 (CPOL=0, CPHA=0)                                         │
│  Data: RGB565 (16 bits per pixel)                                      │
│                                                                         │
│  Transmission Sequence:                                                 │
│  ┌─────────────────────────────────────────────────────────────────┐  │
│  │ 1. Set column address (0-239)                                    │  │
│  │    CMD: 0x2A                                                     │  │
│  │    DATA: 0x00, 0x00, 0x00, 0xEF                                 │  │
│  │                                                                  │  │
│  │ 2. Set row address (0-239)                                       │  │
│  │    CMD: 0x2B                                                     │  │
│  │    DATA: 0x00, 0x00, 0x00, 0xEF                                 │  │
│  │                                                                  │  │
│  │ 3. Write to memory                                               │  │
│  │    CMD: 0x2C                                                     │  │
│  │    DC=1 (data mode)                                              │  │
│  │                                                                  │  │
│  │ 4. Send framebuffer in chunks                                    │  │
│  │    For offset = 0 to 57600 step 4096:                           │  │
│  │      chunk_size = min(4096, remaining_pixels)                   │  │
│  │      spi_transaction_t trans = {                                │  │
│  │        .length = chunk_size * 16,  // bits                      │  │
│  │        .tx_buffer = &framebuffer[offset]                        │  │
│  │      }                                                           │  │
│  │      spi_device_polling_transmit(spi_display, &trans)           │  │
│  │                                                                  │  │
│  │    Total chunks: 15 (57600 ÷ 4096 ≈ 14.06)                      │  │
│  │    Transfer time: ~92ms @ 10 MHz                                │  │
│  └─────────────────────────────────────────────────────────────────┘  │
│                                                                         │
│  Performance:                                                           │
│    - Bytes per frame: 115,200                                          │
│    - SPI clock: 10 MHz                                                 │
│    - Throughput: ~1.25 MB/s                                            │
│    - Frame time: 92ms                                                  │
│    - Effective FPS: 10.8                                               │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                    GC9A01 ROUND LCD DISPLAY                             │
│                                                                         │
│                  ╔══════════════════════╗                               │
│                ╔═╝                      ╚═╗                             │
│              ╔═╝                          ╚═╗                           │
│            ╔═╝    ░░░░░░░░░░░░░░░░░░░░    ╚═╗                         │
│           ╔╝    ░░                  ░░    ╚╗                           │
│          ╔╝    ░░    ◆◆◆◆◆◆◆◆◆◆    ░░    ╚╗                          │
│         ╔╝    ░░    ◆◆         ◆◆    ░░    ╚╗                         │
│         ║    ░░    ◆◆   ████   ◆◆    ░░    ║                          │
│        ║    ░░    ◆◆    ████    ◆◆    ░░    ║                         │
│        ║    ░░    ◆◆   ████   ◆◆    ░░    ║                          │
│         ║    ░░    ◆◆         ◆◆    ░░    ║                          │
│         ╚╗    ░░    ◆◆◆◆◆◆◆◆◆◆    ░░    ╔╝                         │
│          ╚╗    ░░                  ░░    ╔╝                           │
│           ╚╗    ░░░░░░░░░░░░░░░░░░░░    ╔╝                           │
│            ╚═╗                          ╔═╝                             │
│              ╚═╗                      ╔═╝                               │
│                ╚═╗                  ╔═╝                                 │
│                  ╚══════════════════╝                                   │
│                                                                         │
│  Legend:                                                                │
│    ░░ = Sclera (white)                                                 │
│    ◆◆ = Iris (green-blue, varies by mood)                             │
│    ██ = Pupil (black, size varies 0.2-0.6)                            │
│    Top/bottom areas = Eyelids (goblin skin tone)                       │
│                                                                         │
│  Physical specs:                                                        │
│    - Size: 1.28" diagonal                                              │
│    - Resolution: 240×240 pixels                                        │
│    - Interface: SPI (4-wire)                                           │
│    - Power: 3.3V @ 30mA typical                                        │
│    - Viewing angle: 170°                                               │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

## Data Types Throughout Pipeline

### 1. Sensor Reading (uint8_t)
```c
uint8_t distance_cm = 25;  // 0-255 cm
```

### 2. Shared State (struct)
```c
p32_shared_state_t {
    uint8_t distance_cm;   // Sensor value
    Mood mood;             // Emotional state
    // ... other fields
}
```

### 3. Animation Keyframe (struct)
```c
eye_keyframe_t {
    uint32_t time_ms;         // 0-65535 ms
    float pupil_size;         // 0.0-1.0
    float eye_openness;       // 0.0-1.0
    uint32_t color;           // 0xRRGGBB
    eye_expression_t expr;    // enum
}
```

### 4. RGB565 Framebuffer (uint16_t array)
```c
uint16_t framebuffer[57600];  // 240×240 pixels
// Format: RRRRR GGGGGG BBBBB
//         5-bit  6-bit  5-bit
```

### 5. SPI Transaction (struct)
```c
spi_transaction_t {
    size_t length;          // bits to send
    void* tx_buffer;        // pointer to data
    // ... flags, callbacks
}
```

## Timing Analysis

```
Time (ms)    0      100    200    300    400    500    600    700    800
             │      │      │      │      │      │      │      │      │
             ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼

Main Loop    ●      ●      ●      ●      ●      ●      ●      ●      ●
(10 Hz)      └──────100ms────────────────────────────────────────────

Eye Left     ●                                                       ●
Act          │                                                       │
(hitCount    └───────────────────────5000ms─────────────────────────
= 50)        │                                                       │
             └─[measure]─[decide]─[update]─[render]─[transmit]─────
               <1ms      <1ms     <5ms     <10ms    <92ms

Nose Act     ●                                                       ●
(hitCount    │                                                       │
= 200)       └──────────────────────20000ms──────────────────────────
             │
             └─[measure HC-SR04]─[update shared state]──────────────
               <50ms (echo wait)  <1ms

Animation    ┌─────────────────800ms blink cycle──────────────────┐
Timeline     │                                                     │
             0ms      200ms      400ms      600ms      800ms
             Open     Closing    Closed     Opening    Open
             (0.8)    (0.4)      (0.0)      (0.4)      (0.8)
```

## Memory Map

```
ESP32-S3 Memory Layout:

┌─────────────────────────────────────┐
│ SRAM (512 KB total)                 │
├─────────────────────────────────────┤
│                                     │
│ System/FreeRTOS: ~100 KB            │
│                                     │
│ Left Eye Framebuffer: 113 KB       │
│                                     │
│ Right Eye Framebuffer: 113 KB      │
│                                     │
│ Shared State: <1 KB                 │
│                                     │
│ Component Code: ~50 KB              │
│                                     │
│ Available: ~135 KB                  │
│                                     │
└─────────────────────────────────────┘

┌─────────────────────────────────────┐
│ PSRAM (2 MB - optional)             │
├─────────────────────────────────────┤
│                                     │
│ (Can move framebuffers here if      │
│  SRAM pressure increases)           │
│                                     │
└─────────────────────────────────────┘
```

## Summary

This diagram shows the complete data flow from:
1. **Physical sensor** (HC-SR04 ultrasonic)
2. **Sensor component** (reads and stores distance)
3. **Shared memory** (distributes state across system)
4. **Eye component** (decides animation, updates timing)
5. **Animation engine** (interpolates keyframes)
6. **Graphics renderer** (generates pixel-perfect eyes)
7. **SPI hardware** (transmits framebuffer)
8. **Physical display** (shows animated eye to user)

The system runs continuously at 10 Hz, with eyes updating every 5 seconds, rendering smooth animations with proper eyelid movement, pupil dilation, and expression changes based on proximity and mood.
