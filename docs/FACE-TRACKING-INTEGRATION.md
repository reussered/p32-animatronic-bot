# Face-Tracking Eye Integration - Implementation Plan

## Success Criteria
✅ **"I see eyeballs on my display boards change focus when my face gets closer"**

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                    NOSE SENSOR (HC-SR04)                        │
│                    GPIO32 (trigger)                             │
│                    GPIO33 (echo)                                │
└────────────────────┬────────────────────────────────────────────┘
                     │
                     │ Read distance
                     ▼
         ┌────────────────────────┐
         │ goblin_nose_act()      │
         │ - Call HC-SR04 driver  │
         │ - Update g_shared_state│
         └────────────────────────┘
                     │
                     │ g_shared_state.distance_cm
                     ▼
         ┌─────────────────────────────────────────┐
         │   SHARED STATE (mesh synchronized)      │
         │   distance_cm: uint8_t                  │
         └─────────────────────────────────────────┘
                     │
           ┌─────────┴─────────┐
           ▼                   ▼
┌────────────────────┐  ┌────────────────────┐
│ goblin_eye_left_   │  │ goblin_eye_right_  │
│ act()              │  │ act()              │
│                    │  │                    │
│ Read distance      │  │ Read distance      │
│ If < 30cm:         │  │ If < 30cm:         │
│   Play CURIOUS     │  │   Play CURIOUS     │
│ If > 50cm:         │  │   Play CURIOUS     │
│   Play BLINK       │  │   Play BLINK       │
│                    │  │                    │
│ Render to GC9A01   │  │ Render to GC9A01   │
└────────────────────┘  └────────────────────┘
           │                   │
           │ SPI2_HOST         │ SPI2_HOST
           │ CS: GPIO10        │ CS: GPIO9
           ▼                   ▼
    ┌─────────────┐     ┌─────────────┐
    │  LEFT EYE   │     │  RIGHT EYE  │
    │  GC9A01     │     │  GC9A01     │
    │  240x240    │     │  240x240    │
    └─────────────┘     └─────────────┘
```

## Implementation Steps

### Step 1: Integrate HC-SR04 Sensor Driver

**File:** `src/components/goblin_nose.c`

**Current (Simulated):**
```c
void goblin_nose_act(void)
{
    // Simulated distance
    simulated_distance = 20.0f + 10.0f * sinf(...);
}
```

**Target (Real Hardware):**
```c
void goblin_nose_act(void)
{
    extern uint64_t g_loopCount;
    extern p32_shared_state_t g_shared_state;
    
    // Real HC-SR04 ultrasonic sensor reading
    // GPIO32 = trigger, GPIO33 = echo (per goblin_nose.json)
    
    // Send 10µs trigger pulse
    gpio_set_level(32, 1);
    esp_rom_delay_us(10);
    gpio_set_level(32, 0);
    
    // Measure echo pulse duration
    uint32_t timeout = 30000; // 30ms
    uint32_t start = esp_timer_get_time();
    
    // Wait for echo HIGH
    while (gpio_get_level(33) == 0) {
        if ((esp_timer_get_time() - start) > timeout) return;
    }
    
    uint32_t echo_start = esp_timer_get_time();
    
    // Wait for echo LOW
    while (gpio_get_level(33) == 1) {
        if ((esp_timer_get_time() - start) > timeout) return;
    }
    
    uint32_t echo_end = esp_timer_get_time();
    uint32_t duration_us = echo_end - echo_start;
    
    // Calculate distance: duration * speed_of_sound / 2
    // Speed of sound = 343 m/s = 0.0343 cm/µs
    float distance_cm = (duration_us * 0.0343f) / 2.0f;
    
    // Clamp to uint8_t range (0-255 cm)
    if (distance_cm > 255.0f) distance_cm = 255.0f;
    if (distance_cm < 0.0f) distance_cm = 0.0f;
    
    // Update shared state (ESP-NOW will sync to other subsystems)
    g_shared_state.distance_cm = (uint8_t)distance_cm;
    
    ESP_LOGD(TAG, "Distance: %d cm", g_shared_state.distance_cm);
}
```

**Initialization (add to goblin_nose_init):**
```c
esp_err_t goblin_nose_init(void)
{
    ESP_LOGI(TAG, "Initializing nose sensor (HC-SR04)");
    
    // Configure GPIO32 as output (trigger)
    gpio_config_t trigger_conf = {
        .pin_bit_mask = (1ULL << 32),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&trigger_conf));
    
    // Configure GPIO33 as input (echo)
    gpio_config_t echo_conf = {
        .pin_bit_mask = (1ULL << 33),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&echo_conf));
    
    // Set trigger LOW initially
    gpio_set_level(32, 0);
    
    ESP_LOGI(TAG, "HC-SR04 sensor initialized (GPIO32=trigger, GPIO33=echo)");
    return ESP_OK;
}
```

### Step 2: Add Proximity-Based Animation Logic to Eyes

**File:** `src/components/goblin_eye_left.c`

**Add to `goblin_eye_left_act()`:**
```c
void goblin_eye_left_act(void)
{
    extern uint64_t g_loopCount;
    extern p32_shared_state_t g_shared_state;
    
    uint32_t current_time = (uint32_t)(esp_timer_get_time() / 1000);
    
    // Read distance from shared state (updated by nose sensor)
    uint8_t distance = g_shared_state.distance_cm;
    
    // BEHAVIOR: Change animation based on proximity
    static eye_animation_t* current_anim = NULL;
    
    if (distance < 30 && current_anim != &goblin_curious_look_animation)
    {
        // Face is CLOSE → switch to CURIOUS
        ESP_LOGI(TAG, "Face detected at %d cm - CURIOUS MODE", distance);
        eye_display_start_animation(&left_eye_display, &goblin_curious_look_animation);
        current_anim = &goblin_curious_look_animation;
    }
    else if (distance > 50 && current_anim != &goblin_blink_animation)
    {
        // Face is FAR → switch to IDLE BLINK
        ESP_LOGI(TAG, "Face moved away (%d cm) - IDLE MODE", distance);
        eye_display_start_animation(&left_eye_display, &goblin_blink_animation);
        current_anim = &goblin_blink_animation;
    }
    
    // Update animation state
    eye_display_update(&left_eye_display, current_time);
    
    // Render to actual GC9A01 display (SPI2_HOST, CS=GPIO10)
    // TODO: Implement SPI framebuffer transfer
    if (g_loopCount % 5 == 0)
    {
        ESP_LOGD(TAG, "Rendering left eye - openness: %.2f", 
                 left_eye_display.current_frame.eye_openness);
    }
}
```

**Same logic for `goblin_eye_right.c` (CS=GPIO9)**

### Step 3: Initialize SPI Bus and GC9A01 Displays

**File:** `src/components/goblin_eye_left.c`

**Add to `goblin_eye_left_init()`:**
```c
esp_err_t goblin_eye_left_init(void)
{
    ESP_LOGI(TAG, "Initializing left eye display (GC9A01)");
    
    // Initialize SPI bus (shared between both eyes + mouth)
    spi_bus_config_t bus_config = {
        .mosi_io_num = 11,     // SPI MOSI (per spi_bus_vspi.json)
        .miso_io_num = 13,     // SPI MISO
        .sclk_io_num = 12,     // SPI SCLK
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = GC9A01_BUFFER_SIZE
    };
    
    // Initialize SPI2_HOST (only needs to be done once, check if already init)
    static bool spi_bus_initialized = false;
    if (!spi_bus_initialized)
    {
        ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO));
        spi_bus_initialized = true;
        ESP_LOGI(TAG, "SPI2 bus initialized");
    }
    
    // Configure left eye device (CS=GPIO10)
    spi_device_interface_config_t dev_config = {
        .clock_speed_hz = 40 * 1000 * 1000,  // 40 MHz
        .mode = 0,
        .spics_io_num = 10,  // CS pin (per spi_device_1.json)
        .queue_size = 7,
        .pre_cb = NULL
    };
    
    spi_device_handle_t spi_handle;
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_config, &spi_handle));
    
    // Initialize GC9A01 display chip
    // TODO: Send initialization sequence (reset, power on, display settings)
    
    // Initialize animation system
    esp_err_t ret = eye_display_init(&left_eye_display, "LEFT EYE");
    if (ret == ESP_OK)
    {
        eye_display_start_animation(&left_eye_display, &goblin_blink_animation);
        ESP_LOGI(TAG, "Left eye animation system initialized");
    }
    
    return ret;
}
```

### Step 4: Implement GC9A01 Framebuffer Transfer

**Add to `goblin_eye_left_act()` after `eye_display_update()`:**
```c
    // Render animation to framebuffer
    // (eye_display already generates RGB565 pixel data)
    
    // Transfer framebuffer to GC9A01 via SPI
    if (g_loopCount % 5 == 0)  // 20 FPS (120000 / 5 = 24000 Hz)
    {
        // GC9A01 display update sequence:
        // 1. Set memory write command (0x2C)
        // 2. Send RGB565 framebuffer data
        
        spi_transaction_t trans = {
            .length = GC9A01_BUFFER_SIZE * 8,  // Bits
            .tx_buffer = left_eye_display.framebuffer,
            .flags = 0
        };
        
        spi_device_transmit(spi_handle, &trans);
        
        ESP_LOGV(TAG, "Left eye framebuffer sent to GC9A01");
    }
```

## GPIO Pin Assignments (Per JSON Config)

| Component | GPIO | Function |
|-----------|------|----------|
| Nose Sensor | 32 | HC-SR04 Trigger |
| Nose Sensor | 33 | HC-SR04 Echo |
| SPI Bus | 11 | MOSI (shared) |
| SPI Bus | 12 | SCLK (shared) |
| SPI Bus | 13 | MISO (shared) |
| Left Eye | 10 | CS (SPI_DEVICE_1) |
| Right Eye | 9 | CS (SPI_DEVICE_2) |
| Mouth | 14 | CS (SPI_DEVICE_3) |

## Behavior Logic

```
Distance < 30 cm  →  CURIOUS (wide eyes, pupil dilated)
Distance 30-50 cm →  NEUTRAL (current animation continues)
Distance > 50 cm  →  IDLE BLINK (relaxed, periodic blinks)
```

## Testing Procedure

1. **Upload firmware:** `pio run -t upload`
2. **Monitor serial:** `pio device monitor`
3. **Verify initialization:**
   - "HC-SR04 sensor initialized"
   - "SPI2 bus initialized"
   - "Left eye animation system initialized"
   - "Right eye animation system initialized"

4. **Test proximity detection:**
   - Move face CLOSE (< 30cm)
   - Serial log: "Face detected at XX cm - CURIOUS MODE"
   - Displays show wide curious eyes

5. **Test idle return:**
   - Move face FAR (> 50cm)
   - Serial log: "Face moved away (XX cm) - IDLE MODE"
   - Displays return to periodic blinking

## Shared State Integration

The `g_shared_state` structure (defined in `p32_shared_state.h`) is used for:

1. **Sensor → Eyes Communication:**
   - Nose component writes `g_shared_state.distance_cm`
   - Eye components read `g_shared_state.distance_cm`

2. **Future ESP-NOW Mesh:**
   - Torso ESP32 (nose sensor) broadcasts shared state
   - Head ESP32 (eye displays) receives updates
   - Eyes react to remote sensor data

## Success Metrics

✅ **HC-SR04 reads real distance** (not simulated)  
✅ **Shared state updated** (`g_shared_state.distance_cm`)  
✅ **Eyes switch animations** based on proximity  
✅ **GC9A01 displays render** (pixels visible on hardware)  
✅ **Face tracking works** (eyes respond to your movement)

---

📘 **[Component Function Signatures](COMPONENT-FUNCTION-SIGNATURES.md)**  
📘 **[Mesh State Synchronization](MESH-STATE-SYNCHRONIZATION-SPEC.md)**  
📘 **[Distributed Processing Architecture](DISTRIBUTED-PROCESSING-ARCHITECTURE.md)**
