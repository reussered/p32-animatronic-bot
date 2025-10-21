# Brain-to-Mouth Communication Example

## Architecture Overview

**Torso (Brain)** decides WHAT to say → **Head (Mouth)** renders speech visually

```
┌─────────────────────────────────────────────────────────────┐
│  TORSO ESP32-S3 (BRAIN)                                     │
│  ────────────────────────────────────────────────────────── │
│                                                              │
│  // Behavior planning                                       │
│  if (child_detected) {                                      │
│      say_greeting();  // "Hello, want to learn calculus?"  │
│  }                                                           │
│                                                              │
│  void say_greeting() {                                      │
│      g_shared_state.is_speaking = true;                     │
│      g_shared_state.speech_phoneme = PHONEME_H;             │
│      g_shared_state.speech_volume = 80;                     │
│      mesh_mark_state_dirty();  // Broadcast to head        │
│  }                                                           │
│                                                              │
└─────────────────────────────────────────────────────────────┘
                          │
                          │ SharedMemory Mesh (100ms sync)

> All state synchronization between brain and mouth subsystems is performed using the SharedMemory class, which abstracts ESP-NOW mesh networking. Application code should use SharedMemory.read() and SharedMemory.write() for all inter-subsystem communication.
                          ▼
┌─────────────────────────────────────────────────────────────┐
│  HEAD ESP32-S3 (MOUTH)                                      │
│  ────────────────────────────────────────────────────────── │
│                                                              │
│  // goblin_mouth.cpp                                       │
│  void goblin_mouth_act(void) {                              │
│      if (g_shared_state.is_speaking) {                      │
│          render_mouth_shape(g_shared_state.speech_phoneme); │
│      } else {                                                │
│          render_mouth_idle();                               │
│      }                                                       │
│  }                                                           │
│                                                              │
│  void render_mouth_shape(phoneme_t p) {                     │
│      switch(p) {                                            │
│          case PHONEME_H: draw_open_mouth(); break;          │
│          case PHONEME_O: draw_rounded_mouth(); break;       │
│          case PHONEME_E: draw_wide_smile(); break;          │
│          // ... more phonemes                               │
│      }                                                       │
│  }                                                           │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

## Shared State Extensions

Add speech fields to `p32_shared_state.h`:

```cpp
// include/p32_shared_state.h

typedef enum {
    PHONEME_SILENT = 0,
    PHONEME_A,      // "ah" - open mouth
    PHONEME_E,      // "ee" - wide smile
    PHONEME_I,      // "ih" - narrow
    PHONEME_O,      // "oh" - rounded
    PHONEME_U,      // "oo" - puckered
    PHONEME_M,      // "mm" - closed
    PHONEME_F,      // "ff" - teeth on lip
    PHONEME_TH,     // "th" - tongue out
    PHONEME_L,      // "ll" - tongue up
    PHONEME_W,      // "ww" - rounded + narrow
    PHONEME_COUNT
} phoneme_t;

typedef struct {
    // Version and sync metadata
    uint32_t version;
    uint32_t timestamp_ms;
    uint8_t source_node_id;
    
    // Mood state (serialized from C++ Mood class)
    int8_t mood_serialized[9];
    
    // Sensor data
    uint8_t distance_cm;
    bool touch_detected;
    int16_t temperature_c;
    
    // Behavioral state
    bool is_speaking;
    bool is_moving;
    uint8_t current_behavior;
    
    // SPEECH CONTROL (new fields)
    phoneme_t speech_phoneme;      // Current mouth shape
    uint8_t speech_volume;         // 0-100 (affects mouth opening size)
    uint16_t speech_duration_ms;   // How long to hold this phoneme
    bool speech_sync_audio;        // True if audio is playing
    
    // System status
    uint8_t battery_percent;
    uint16_t uptime_seconds;
    bool wifi_connected;
    
    uint32_t checksum;
} p32_shared_state_t;
```

## Brain Component (Torso)

```cpp
// src/components/speech_controller.cpp
// Runs on TORSO ESP32-S3, coordinates speech output

#include "p32_shared_state.h"
#include "p32_component_config.h"

#ifdef ENABLE_GOBLIN_FAMILY_COMPONENTS

// Phoneme timing table for "Hello"
typedef struct {
    phoneme_t phoneme;
    uint16_t duration_ms;
} phoneme_sequence_t;

static const phoneme_sequence_t WORD_HELLO[] = {
    {PHONEME_SILENT, 100},  // Brief pause before speaking
    {PHONEME_H, 150},       // "H" - breathy open
    {PHONEME_E, 200},       // "e" - wide smile
    {PHONEME_L, 150},       // "l" - tongue up
    {PHONEME_O, 250},       // "o" - rounded mouth
    {PHONEME_SILENT, 200},  // End pause
};

static uint8_t s_current_phoneme_index = 0;
static uint32_t s_phoneme_start_time = 0;
static bool s_is_playing = false;

esp_err_t speech_controller_init(void) {
    ESP_LOGI(TAG, "Speech controller initialized");
    return ESP_OK;
}

void speech_controller_act(void) {
    // hitCount: 10 → executes every 1 second
    
    if (!s_is_playing) {
        return;  // Nothing to do
    }
    
    // Check if current phoneme duration elapsed
    uint32_t elapsed = g_shared_state.timestamp_ms - s_phoneme_start_time;
    
    if (elapsed >= WORD_HELLO[s_current_phoneme_index].duration_ms) {
        // Move to next phoneme
        s_current_phoneme_index++;
        
        if (s_current_phoneme_index >= sizeof(WORD_HELLO)/sizeof(WORD_HELLO[0])) {
            // Word complete
            s_is_playing = false;
            g_shared_state.is_speaking = false;
            s_current_phoneme_index = 0;
            
            ESP_LOGI(TAG, "Speech complete: 'Hello'");
        } else {
            // Update to next phoneme
            g_shared_state.speech_phoneme = WORD_HELLO[s_current_phoneme_index].phoneme;
            g_shared_state.speech_duration_ms = WORD_HELLO[s_current_phoneme_index].duration_ms;
            s_phoneme_start_time = g_shared_state.timestamp_ms;
            
            ESP_LOGI(TAG, "Phoneme: %d, duration: %d ms",
                g_shared_state.speech_phoneme,
                g_shared_state.speech_duration_ms);
        }
        
        mesh_mark_state_dirty();  // Broadcast updated phoneme to head
    }
}

// Public API: Trigger speech
void say_hello(void) {
    s_is_playing = true;
    s_current_phoneme_index = 0;
    s_phoneme_start_time = g_shared_state.timestamp_ms;
    
    g_shared_state.is_speaking = true;
    g_shared_state.speech_phoneme = WORD_HELLO[0].phoneme;
    g_shared_state.speech_duration_ms = WORD_HELLO[0].duration_ms;
    g_shared_state.speech_volume = 80;  // Medium volume
    
    mesh_mark_state_dirty();
    
    ESP_LOGI(TAG, "Starting speech: 'Hello'");
}

#endif // ENABLE_GOBLIN_FAMILY_COMPONENTS
```

## Mouth Component (Head)

```cpp
// src/components/goblin_mouth.cpp
// Runs on HEAD ESP32-S3, renders mouth shapes on GC9A01 display

#include "p32_shared_state.h"
#include "p32_component_config.h"
#include "driver/spi_master.h"

#ifdef ENABLE_GOBLIN_FAMILY_COMPONENTS

static spi_device_handle_t s_spi_mouth_display = NULL;
static uint16_t s_framebuffer[240*240];  // GC9A01 is 240x240

esp_err_t goblin_mouth_init(void) {
    // Initialize SPI connection to mouth display
    // (SPI bus already initialized by system component)
    
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 40*1000*1000,  // 40 MHz
        .mode = 0,
        .spics_io_num = GPIO_MOUTH_CS,   // From config
        .queue_size = 1,
    };
    
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_cfg, &s_spi_mouth_display));
    
    // Initialize GC9A01 display (send init commands)
    init_gc9a01_display(s_spi_mouth_display);
    
    ESP_LOGI(TAG, "Mouth display initialized");
    return ESP_OK;
}

void goblin_mouth_act(void) {
    // hitCount: 3 → executes every 300ms (3.3 FPS minimum, smooth enough for speech)
    
    // Check if speech is active
    if (g_shared_state.is_speaking) {
        render_mouth_phoneme(g_shared_state.speech_phoneme, g_shared_state.speech_volume);
    } else {
        render_mouth_idle();
    }
    
    // Send framebuffer to display via SPI
    send_framebuffer_to_display(s_spi_mouth_display, s_framebuffer);
}

void render_mouth_idle(void) {
    // Neutral closed mouth (black screen with small line)
    clear_framebuffer(0x0000);  // Black
    
    // Draw horizontal line for closed mouth
    for (int x = 80; x < 160; x++) {
        for (int y = 115; y < 125; y++) {
            s_framebuffer[y*240 + x] = 0xFFFF;  // White line
        }
    }
}

void render_mouth_phoneme(phoneme_t phoneme, uint8_t volume) {
    clear_framebuffer(0x0000);  // Black background
    
    // Scale mouth size based on volume (0-100)
    float scale = volume / 100.0f;
    
    switch(phoneme) {
        case PHONEME_SILENT:
            render_mouth_idle();  // Closed
            break;
            
        case PHONEME_A:
            // "ah" - Wide open mouth (vertical oval)
            draw_ellipse(120, 120, 60*scale, 80*scale, 0xF800);  // Red
            break;
            
        case PHONEME_E:
            // "ee" - Wide smile (horizontal oval)
            draw_ellipse(120, 120, 90*scale, 40*scale, 0xFFE0);  // Yellow
            break;
            
        case PHONEME_O:
            // "oh" - Rounded mouth (circle)
            draw_circle(120, 120, 50*scale, 0x07E0);  // Green
            break;
            
        case PHONEME_U:
            // "oo" - Small puckered (small circle)
            draw_circle(120, 120, 30*scale, 0x001F);  // Blue
            break;
            
        case PHONEME_M:
            // "mm" - Closed (horizontal line)
            draw_horizontal_line(80, 160, 120, 0xFFFF);  // White
            break;
            
        case PHONEME_L:
            // "ll" - Tongue up (open with line at top)
            draw_ellipse(120, 120, 50*scale, 60*scale, 0xF800);  // Red mouth
            draw_horizontal_line(100, 140, 100, 0xF8E0);  // Pink tongue
            break;
            
        default:
            // Unknown phoneme - show neutral
            render_mouth_idle();
            break;
    }
    
    ESP_LOGD(TAG, "Rendered phoneme: %d, volume: %d%%", phoneme, volume);
}

// Helper: Draw filled ellipse
void draw_ellipse(int cx, int cy, int rx, int ry, uint16_t color) {
    for (int y = cy - ry; y <= cy + ry; y++) {
        for (int x = cx - rx; x <= cx + rx; x++) {
            // Ellipse equation: (x-cx)²/rx² + (y-cy)²/ry² <= 1
            int dx = x - cx;
            int dy = y - cy;
            if (dx*dx*ry*ry + dy*dy*rx*rx <= rx*rx*ry*ry) {
                if (x >= 0 && x < 240 && y >= 0 && y < 240) {
                    s_framebuffer[y*240 + x] = color;
                }
            }
        }
    }
}

// Helper: Draw filled circle
void draw_circle(int cx, int cy, int r, uint16_t color) {
    draw_ellipse(cx, cy, r, r, color);  // Circle is ellipse with rx == ry
}

// Helper: Draw horizontal line
void draw_horizontal_line(int x1, int x2, int y, uint16_t color) {
    for (int x = x1; x < x2; x++) {
        if (x >= 0 && x < 240 && y >= 0 && y < 240) {
            s_framebuffer[y*240 + x] = color;
        }
    }
}

void clear_framebuffer(uint16_t color) {
    for (int i = 0; i < 240*240; i++) {
        s_framebuffer[i] = color;
    }
}

void send_framebuffer_to_display(spi_device_handle_t spi, uint16_t *fb) {
    spi_transaction_t trans = {
        .length = 240*240*2*8,  // bits
        .tx_buffer = fb,
    };
    spi_device_transmit(spi, &trans);
}

#endif // ENABLE_GOBLIN_FAMILY_COMPONENTS
```

## Behavior Trigger (Example)

```cpp
// src/components/goblin_behavior_engine.cpp
// Runs on TORSO, decides when to speak

#include "p32_shared_state.h"

void goblin_behavior_engine_act(void) {
    // hitCount: 25 → executes every 2.5 seconds
    
    // Check if child is nearby (from synchronized sensor data)
    if (g_shared_state.distance_cm < 50 && !g_shared_state.is_speaking) {
        // Child detected and not currently speaking
        say_hello();  // Trigger speech (calls speech_controller)
        
        ESP_LOGI(TAG, "Child detected at %d cm, saying hello", 
            g_shared_state.distance_cm);
    }
}
```

## Visual Result

**What you'll see on the mouth display:**

```
Time    Phoneme      Display
────────────────────────────────────
0.0s    SILENT       [━━━━━━━━━]     (closed line)
0.1s    H            [  ◯◯◯  ]       (breathy open - red)
0.25s   E            [━━━◯◯◯━━━]     (wide smile - yellow)
0.45s   L            [━━◯◯◯━━]       (tongue up - red + pink)
0.6s    O            [   ◯◯◯   ]     (rounded - green)
0.85s   SILENT       [━━━━━━━━━]     (closed line)
```

## Advantages of This Approach

✅ **Decoupled**: Brain decides WHAT to say, mouth renders HOW to show it
✅ **Low latency**: Phoneme updates sync within 100ms via mesh
✅ **Scalable**: Add more words by defining phoneme sequences
✅ **Testable**: Can test brain and mouth independently
✅ **Synchronized**: Mouth shapes match audio playback timing
✅ **Expressive**: Volume parameter adds emphasis (louder = bigger mouth)

## Next Evolution: Audio Synchronization

```cpp
// Later: Add audio playback component
void speech_controller_act(void) {
    // Update phoneme in shared state
    g_shared_state.speech_phoneme = current_phoneme;
    
    // Also trigger audio playback
    if (audio_buffer_ready) {
        i2s_write(audio_chunk);  // Play audio
        g_shared_state.speech_sync_audio = true;
    }
    
    mesh_mark_state_dirty();
}

// Mouth component stays unchanged - just reads g_shared_state.speech_phoneme
```

## Test Procedure

1. **Phase 1**: Prove mesh works (from HARDWARE-VALIDATION-TEST-PLAN.md)
2. **Phase 2**: Add speech fields to p32_shared_state.h
3. **Phase 3**: Implement speech_controller.cpp on torso
4. **Phase 4**: Implement goblin_mouth.cpp on head
5. **Phase 5**: Trigger say_hello() manually via serial command
6. **Phase 6**: Watch mouth display cycle through phonemes for "Hello"

**Expected result**: Mouth display shows smooth transitions from closed → H → E → L → O → closed over ~1 second.

---

This demonstrates the **power of shared state + mesh synchronization**: The brain can "talk" to the mouth with minimal code, just by updating a few fields in `g_shared_state`!

📘 **Related Documents**:
- [Hardware Validation Test Plan](HARDWARE-VALIDATION-TEST-PLAN.md) - Prove mesh works first
- [Mesh State Synchronization](MESH-STATE-SYNCHRONIZATION-SPEC.md) - How state broadcasts work
- [Component Function Signatures](COMPONENT-FUNCTION-SIGNATURES.md) - NO ARGUMENTS access pattern
