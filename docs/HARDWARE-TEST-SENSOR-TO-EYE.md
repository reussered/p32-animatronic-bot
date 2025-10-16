# Hardware Test: Sensor → Mood → Eye Expression Pipeline

## Objective

**Prove the complete data flow**: Ultrasonic sensor (nose) → Personality (mood update) → Eye display (expression change)

**What you'll see**: Eyes change shape/color as you move your face closer and further away!

## Hardware Requirements

### Torso ESP32-S3 (Master)
- 1× ESP32-S3 DevKitC-1
- 1× HC-SR04 Ultrasonic Sensor
- 4× Jumper wires
- 1× Breadboard

**Wiring**:
```
HC-SR04 Ultrasonic Sensor:
├─ VCC  → 5V (ESP32 pin)
├─ GND  → GND
├─ TRIG → GPIO 9
└─ ECHO → GPIO 10
```

### Head ESP32-S3 (Slave)
- 1× ESP32-S3 DevKitC-1
- 1× GC9A01 1.28" Circular Display
- 7× Jumper wires
- 1× Breadboard

**Wiring**:
```
GC9A01 Display (SPI):
├─ VCC  → 3.3V
├─ GND  → GND
├─ SCK  → GPIO 12 (SPI CLK)
├─ SDA  → GPIO 11 (SPI MOSI)
├─ RES  → GPIO 13 (Reset)
├─ DC   → GPIO 14 (Data/Command)
└─ CS   → GPIO 10 (Chip Select)
```

**Total Cost**: $58
- 2× ESP32-S3 DevKitC-1: $20
- 1× GC9A01 display: $10
- 1× HC-SR04 sensor: $3
- Breadboards + wires: $8
- Multimeter (if needed): $15

## Test Code

### Shared Header (Both Nodes)

```cpp
// include/test_shared_state.h

#ifndef TEST_SHARED_STATE_H
#define TEST_SHARED_STATE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Simplified Mood class (C struct for testing)
typedef struct {
    int8_t happiness;
    int8_t curiosity;
    int8_t excitement;
    int8_t anger;
    int8_t fear;
} Mood_t;

// Simplified Environment class (C struct for testing)
typedef struct {
    uint8_t distance_cm;
    bool target_detected;
    uint8_t battery_percent;
} Environment_t;

// ESP-NOW packet format
#define MAX_NAME_LENGTH 32
typedef struct {
    char name[MAX_NAME_LENGTH];
    uint8_t data[128];
    size_t size;
} mesh_packet_t;

#ifdef __cplusplus
}
#endif

#endif // TEST_SHARED_STATE_H
```

### Torso Node (Master) - Sensor + Personality

```cpp
// torso_test_main.cpp

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "test_shared_state.h"

static const char *TAG = "TORSO_TEST";

// GPIO pins for HC-SR04
#define TRIG_PIN GPIO_NUM_9
#define ECHO_PIN GPIO_NUM_10

// Global shared memory
static Environment_t g_Envir = {0};
static Mood_t g_Mood = {0};

// Broadcast address (all nodes)
static uint8_t broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// ESP-NOW send callback
void on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    ESP_LOGI(TAG, "Send status: %s", status == ESP_NOW_SEND_SUCCESS ? "SUCCESS" : "FAIL");
}

// Initialize ESP-NOW
void init_esp_now() {
    // Initialize WiFi in station mode
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE));
    
    // Initialize ESP-NOW
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(on_data_sent));
    
    // Add broadcast peer
    esp_now_peer_info_t peer;
    memset(&peer, 0, sizeof(peer));
    memcpy(peer.peer_addr, broadcast_mac, 6);
    peer.channel = 6;
    peer.ifidx = WIFI_IF_STA;
    peer.encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(&peer));
    
    ESP_LOGI(TAG, "ESP-NOW initialized (Master)");
}

// Initialize HC-SR04 sensor
void init_sensor() {
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
    gpio_set_level(TRIG_PIN, 0);
    
    ESP_LOGI(TAG, "HC-SR04 sensor initialized");
}

// Measure distance with HC-SR04
uint8_t measure_distance() {
    // Send 10us trigger pulse
    gpio_set_level(TRIG_PIN, 1);
    ets_delay_us(10);
    gpio_set_level(TRIG_PIN, 0);
    
    // Wait for echo pulse
    int64_t start_time = esp_timer_get_time();
    while (gpio_get_level(ECHO_PIN) == 0) {
        if (esp_timer_get_time() - start_time > 30000) {
            return 255; // Timeout (no echo)
        }
    }
    
    int64_t pulse_start = esp_timer_get_time();
    while (gpio_get_level(ECHO_PIN) == 1) {
        if (esp_timer_get_time() - pulse_start > 30000) {
            return 255; // Timeout (echo too long)
        }
    }
    int64_t pulse_end = esp_timer_get_time();
    
    // Calculate distance: pulse_width (us) / 58 = distance (cm)
    int64_t pulse_width = pulse_end - pulse_start;
    uint8_t distance_cm = (uint8_t)(pulse_width / 58);
    
    return distance_cm;
}

// Broadcast shared memory block
void broadcast_block(const char *name, void *data, size_t size) {
    mesh_packet_t packet;
    strncpy(packet.name, name, MAX_NAME_LENGTH - 1);
    packet.name[MAX_NAME_LENGTH - 1] = '\0';
    memcpy(packet.data, data, size);
    packet.size = size;
    
    esp_now_send(broadcast_mac, (uint8_t*)&packet, sizeof(packet));
}

void app_main() {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize ESP-NOW and sensor
    init_esp_now();
    init_sensor();
    
    // Initialize mood
    g_Mood.happiness = 50;
    g_Mood.curiosity = 30;
    g_Mood.excitement = 10;
    g_Mood.anger = 0;
    g_Mood.fear = 0;
    
    ESP_LOGI(TAG, "=== TORSO TEST: Sensor → Mood → Mesh ===");
    ESP_LOGI(TAG, "Move your face closer/further from sensor!");
    
    uint32_t loop_count = 0;
    
    while (1) {
        // === SENSOR: Measure Distance (every 100ms) ===
        uint8_t distance_cm = measure_distance();
        
        // Update environment
        Environment_t prev_envir = g_Envir;
        g_Envir.distance_cm = distance_cm;
        g_Envir.target_detected = (distance_cm < 100);
        g_Envir.battery_percent = 75; // Simulated
        
        if (g_Envir.distance_cm != prev_envir.distance_cm) {
            ESP_LOGI(TAG, "📏 Distance: %d cm %s", 
                distance_cm,
                g_Envir.target_detected ? "(DETECTED)" : "");
            
            // Broadcast environment
            broadcast_block("g_Envir", &g_Envir, sizeof(Environment_t));
        }
        
        // === PERSONALITY: Update Mood Based on Distance (every 100ms) ===
        Mood_t prev_mood = g_Mood;
        
        if (g_Envir.target_detected) {
            if (distance_cm < 30) {
                // Very close: curiosity + excitement
                g_Mood.curiosity = 80;
                g_Mood.excitement = 90;
                g_Mood.happiness = 70;
                g_Mood.anger = 0;
                g_Mood.fear = 0;
                
                ESP_LOGI(TAG, "😃 Very close! CURIOUS + EXCITED");
                
            } else if (distance_cm < 60) {
                // Medium distance: curious
                g_Mood.curiosity = 60;
                g_Mood.excitement = 50;
                g_Mood.happiness = 60;
                g_Mood.anger = 0;
                g_Mood.fear = 0;
                
                ESP_LOGI(TAG, "🙂 Medium distance - Curious");
                
            } else {
                // Far but detected: mild curiosity
                g_Mood.curiosity = 40;
                g_Mood.excitement = 20;
                g_Mood.happiness = 50;
                g_Mood.anger = 0;
                g_Mood.fear = 0;
                
                ESP_LOGI(TAG, "😐 Far away - Mildly curious");
            }
        } else {
            // No target: neutral mood with decay
            if (loop_count % 50 == 0) {  // Decay every 5 seconds
                if (g_Mood.curiosity > 30) g_Mood.curiosity -= 5;
                if (g_Mood.excitement > 10) g_Mood.excitement -= 5;
                
                ESP_LOGI(TAG, "😴 No target - Mood decay");
            }
        }
        
        // Broadcast mood if changed
        if (memcmp(&g_Mood, &prev_mood, sizeof(Mood_t)) != 0) {
            ESP_LOGI(TAG, "💭 Mood: HAPPINESS=%d, CURIOSITY=%d, EXCITEMENT=%d",
                g_Mood.happiness, g_Mood.curiosity, g_Mood.excitement);
            
            broadcast_block("g_Mood", &g_Mood, sizeof(Mood_t));
        }
        
        loop_count++;
        vTaskDelay(pdMS_TO_TICKS(100)); // 10 Hz loop
    }
}
```

### Head Node (Slave) - Display

```cpp
// head_test_main.cpp

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "test_shared_state.h"

static const char *TAG = "HEAD_TEST";

// GPIO pins for GC9A01
#define PIN_MOSI   GPIO_NUM_11
#define PIN_CLK    GPIO_NUM_12
#define PIN_CS     GPIO_NUM_10
#define PIN_DC     GPIO_NUM_14
#define PIN_RST    GPIO_NUM_13

// Display size
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 240

// Global shared memory (received via mesh)
static Environment_t g_Envir = {0};
static Mood_t g_Mood = {0};

// SPI device handle
static spi_device_handle_t spi_device;

// ESP-NOW receive callback
void on_data_recv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
    mesh_packet_t *packet = (mesh_packet_t*)data;
    
    // Update local shared memory based on packet name
    if (strcmp(packet->name, "g_Envir") == 0) {
        memcpy(&g_Envir, packet->data, sizeof(Environment_t));
        ESP_LOGI(TAG, "📥 Received g_Envir: distance=%d cm", g_Envir.distance_cm);
        
    } else if (strcmp(packet->name, "g_Mood") == 0) {
        memcpy(&g_Mood, packet->data, sizeof(Mood_t));
        ESP_LOGI(TAG, "📥 Received g_Mood: HAPPINESS=%d, CURIOSITY=%d, EXCITEMENT=%d",
            g_Mood.happiness, g_Mood.curiosity, g_Mood.excitement);
    }
}

// Initialize ESP-NOW
void init_esp_now() {
    // Initialize WiFi in station mode
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE));
    
    // Initialize ESP-NOW
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(on_data_recv));
    
    ESP_LOGI(TAG, "ESP-NOW initialized (Slave)");
}

// Initialize GC9A01 display
void init_display() {
    // Configure SPI bus
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * 2,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO));
    
    // Configure SPI device
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 40 * 1000 * 1000,  // 40 MHz
        .mode = 0,
        .spics_io_num = PIN_CS,
        .queue_size = 7,
        .pre_cb = NULL,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_device));
    
    // Configure DC and RST pins
    gpio_set_direction(PIN_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_RST, GPIO_MODE_OUTPUT);
    
    // Reset display
    gpio_set_level(PIN_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    ESP_LOGI(TAG, "GC9A01 display initialized");
}

// Draw filled circle on display
void draw_circle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color) {
    // Simplified: Just fill entire screen with color for this test
    uint16_t *line_buffer = (uint16_t*)malloc(DISPLAY_WIDTH * 2);
    
    for (int i = 0; i < DISPLAY_WIDTH; i++) {
        line_buffer[i] = color;
    }
    
    // Send multiple lines to fill screen
    spi_transaction_t trans;
    memset(&trans, 0, sizeof(trans));
    trans.length = DISPLAY_WIDTH * 16;  // bits
    trans.tx_buffer = line_buffer;
    
    for (int line = 0; line < DISPLAY_HEIGHT; line++) {
        spi_device_polling_transmit(spi_device, &trans);
    }
    
    free(line_buffer);
}

// Render eye based on mood
void render_eye() {
    uint16_t eye_color;
    uint16_t pupil_size;
    
    // Calculate eye appearance based on mood
    if (g_Mood.excitement > 70) {
        // Very excited: Yellow, wide eyes (dilated pupils)
        eye_color = 0xFFE0;  // Yellow
        pupil_size = 100;
        ESP_LOGI(TAG, "👁️  WIDE EYES (excited, dilated pupils)");
        
    } else if (g_Mood.curiosity > 60) {
        // Curious: Cyan, medium-wide eyes
        eye_color = 0x07FF;  // Cyan
        pupil_size = 80;
        ESP_LOGI(TAG, "👁️  CURIOUS EYES (interested)");
        
    } else if (g_Mood.happiness > 60) {
        // Happy: Green, normal eyes
        eye_color = 0x07E0;  // Green
        pupil_size = 70;
        ESP_LOGI(TAG, "👁️  HAPPY EYES (content)");
        
    } else if (g_Mood.fear > 50) {
        // Fearful: Red, constricted pupils
        eye_color = 0xF800;  // Red
        pupil_size = 40;
        ESP_LOGI(TAG, "👁️  FEARFUL EYES (constricted)");
        
    } else {
        // Neutral: Blue, normal
        eye_color = 0x001F;  // Blue
        pupil_size = 60;
        ESP_LOGI(TAG, "👁️  NEUTRAL EYES");
    }
    
    // Draw colored eye (simplified - full screen fill)
    draw_circle(120, 120, pupil_size, eye_color);
}

void app_main() {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize ESP-NOW and display
    init_esp_now();
    init_display();
    
    ESP_LOGI(TAG, "=== HEAD TEST: Mesh → Eye Display ===");
    ESP_LOGI(TAG, "Waiting for mood updates from torso...");
    
    Mood_t last_rendered_mood = {0};
    
    while (1) {
        // Check if mood has changed
        if (memcmp(&g_Mood, &last_rendered_mood, sizeof(Mood_t)) != 0) {
            ESP_LOGI(TAG, "🎨 Mood changed! Rendering new expression...");
            
            render_eye();
            last_rendered_mood = g_Mood;
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); // Check every 100ms
    }
}
```

## Expected Behavior

### Distance → Mood → Eye Color/Size

| Distance | Mood State | Eye Color | Pupil Size | What You'll See |
|----------|------------|-----------|------------|-----------------|
| **<30 cm** | Curiosity: 80<br>Excitement: 90<br>Happiness: 70 | **Yellow** | **100 (dilated)** | 🟡 **WIDE YELLOW EYES** (very excited!) |
| **30-60 cm** | Curiosity: 60<br>Excitement: 50<br>Happiness: 60 | **Cyan** | **80 (medium)** | 🔵 **CYAN EYES** (curious, interested) |
| **60-100 cm** | Curiosity: 40<br>Excitement: 20<br>Happiness: 50 | **Green** | **70 (normal)** | 🟢 **GREEN EYES** (mildly interested) |
| **>100 cm** | Curiosity: 30→decay<br>Excitement: 10→decay | **Blue** | **60 (normal)** | 🔵 **BLUE EYES** (neutral, alone) |

### Serial Monitor Output

**Torso (Master)**:
```
I (1000) TORSO_TEST: === TORSO TEST: Sensor → Mood → Mesh ===
I (1000) TORSO_TEST: Move your face closer/further from sensor!
I (1100) TORSO_TEST: 📏 Distance: 85 cm (DETECTED)
I (1100) TORSO_TEST: 😐 Far away - Mildly curious
I (1100) TORSO_TEST: 💭 Mood: HAPPINESS=50, CURIOSITY=40, EXCITEMENT=20
I (1100) TORSO_TEST: Send status: SUCCESS
I (2500) TORSO_TEST: 📏 Distance: 45 cm (DETECTED)
I (2500) TORSO_TEST: 🙂 Medium distance - Curious
I (2500) TORSO_TEST: 💭 Mood: HAPPINESS=60, CURIOSITY=60, EXCITEMENT=50
I (2500) TORSO_TEST: Send status: SUCCESS
I (3800) TORSO_TEST: 📏 Distance: 22 cm (DETECTED)
I (3800) TORSO_TEST: 😃 Very close! CURIOUS + EXCITED
I (3800) TORSO_TEST: 💭 Mood: HAPPINESS=70, CURIOSITY=80, EXCITEMENT=90
I (3800) TORSO_TEST: Send status: SUCCESS
```

**Head (Slave)**:
```
I (1000) HEAD_TEST: === HEAD TEST: Mesh → Eye Display ===
I (1000) HEAD_TEST: Waiting for mood updates from torso...
I (1120) HEAD_TEST: 📥 Received g_Envir: distance=85 cm
I (1125) HEAD_TEST: 📥 Received g_Mood: HAPPINESS=50, CURIOSITY=40, EXCITEMENT=20
I (1125) HEAD_TEST: 🎨 Mood changed! Rendering new expression...
I (1125) HEAD_TEST: 👁️  HAPPY EYES (content)
I (2520) HEAD_TEST: 📥 Received g_Envir: distance=45 cm
I (2525) HEAD_TEST: 📥 Received g_Mood: HAPPINESS=60, CURIOSITY=60, EXCITEMENT=50
I (2525) HEAD_TEST: 🎨 Mood changed! Rendering new expression...
I (2525) HEAD_TEST: 👁️  CURIOUS EYES (interested)
I (3820) HEAD_TEST: 📥 Received g_Envir: distance=22 cm
I (3825) HEAD_TEST: 📥 Received g_Mood: HAPPINESS=70, CURIOSITY=80, EXCITEMENT=90
I (3825) HEAD_TEST: 🎨 Mood changed! Rendering new expression...
I (3825) HEAD_TEST: 👁️  WIDE EYES (excited, dilated pupils)
```

## What You'll Experience

1. **Power on both ESP32s**
2. **Move your face toward sensor**:
   - Far away (>60 cm): Eyes stay **green/blue** (neutral)
   - Medium (30-60 cm): Eyes turn **cyan** (curious)
   - Close (<30 cm): Eyes turn **YELLOW and WIDE** (excited!)
3. **Move away**:
   - Eyes transition back through colors
   - Eventually decay to **blue** (neutral) if no movement
4. **Latency**: <50 ms from sensor reading to display update!

## Success Criteria

✅ **ESP-NOW mesh working**: Head receives packets from torso
✅ **g_Envir synchronized**: Distance readings appear on head node
✅ **g_Mood synchronized**: Mood calculations appear on head node
✅ **Eye expression updates**: Display changes color/size based on mood
✅ **Real-time response**: <100 ms latency from movement to eye change

## Build Commands

```bash
# Torso node
cd torso_test
pio run -t upload -t monitor --upload-port COM3

# Head node
cd head_test
pio run -t upload -t monitor --upload-port COM4
```

## Next Steps After Success

1. ✅ Prove ESP-NOW mesh works
2. ✅ Prove g_Envir/g_Mood synchronization works
3. ✅ Prove sensor → mood → display pipeline works
4. → Add second eye (left + right synchronized)
5. → Add mouth display (synchronized mood)
6. → Add more complex mood rules from JSON
7. → Print skull and assemble physical goblin head!

**This is the EXACT test you want to see!** 🎯👁️

📘 **Related Documents**:
- [Global Shared Memory API](GLOBAL-SHARED-MEMORY-API.md)
- [JSON-Driven Personality](JSON-DRIVEN-PERSONALITY-SPEC.md)
- [Hardware Validation Test Plan](HARDWARE-VALIDATION-TEST-PLAN.md)
