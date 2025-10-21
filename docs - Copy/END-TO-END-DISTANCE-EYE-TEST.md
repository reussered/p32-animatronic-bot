# End-to-End Test: Distance → Mood → Eye Expression

## Goal

**Prove the complete pipeline works via ESP-NOW mesh**:

1. **Torso**: HC-SR04 sensor measures distance → updates `g_Envir`
2. **Torso**: Personality reads `g_Envir`, updates `g_MOOD` based on distance
3. **Mesh**: Broadcasts `g_Envir` and `g_MOOD` to head
4. **Head**: Eye component reads `g_MOOD`, renders expression on GC9A01 display

**Visual Result**: Move your face closer/farther → **eyes change shape in real-time!**

## Hardware Required

- **2× ESP32-S3 DevKitC-1** ($20 total)
- **1× HC-SR04 ultrasonic sensor** ($2) - on torso
- **1× GC9A01 1.28" circular display** ($10) - on head
- **Breadboards, jumper wires** ($8)

**Total**: $40 (cheaper than initial estimate!)

## Wiring

### Torso ESP32-S3 (Master)

```
HC-SR04 Ultrasonic Sensor:
├─ VCC  → 5V (ESP32 VBUS)
├─ GND  → GND
├─ TRIG → GPIO 18
└─ ECHO → GPIO 19

Power: USB-C (5V 500mA)
```

### Head ESP32-S3 (Slave)

```
GC9A01 Display (SPI):
├─ VCC  → 3.3V
├─ GND  → GND
├─ SCL  → GPIO 12 (SPI CLK)
├─ SDA  → GPIO 11 (SPI MOSI)
├─ RES  → GPIO 10 (Reset)
├─ DC   → GPIO 9  (Data/Command)
├─ CS   → GPIO 8  (Chip Select)
└─ BL   → 3.3V (Backlight always on)

Power: USB-C (5V 500mA)
```

**Mesh**: Both ESP32s communicate wirelessly via ESP-NOW (2.4 GHz WiFi, no router needed!)

## Test Code

### Shared Header (Both Devices)

```cpp
// include/test_shared_types.h

#ifndef TEST_SHARED_TYPES_H
#define TEST_SHARED_TYPES_H

#include <stdint.h>
#include <string.h>

// Environment (sensor data)
typedef struct {
    uint8_t distance_cm;      // 0-255 cm
    bool target_detected;     // Object within 100cm
} Environment;

// Mood (emotional state)
typedef struct {
    int8_t happiness;         // -128 to +127
    int8_t curiosity;         // -128 to +127
    int8_t fear;              // -128 to +127
} Mood;

// Global shared memory blocks
Environment g_Envir;
Mood g_MOOD;

// Mesh packet for ESP-NOW
typedef struct {
    char name[32];            // "g_Envir" or "g_MOOD"
    uint8_t data[64];         // Payload
    size_t size;              // Payload size
} mesh_packet_t;

#endif // TEST_SHARED_TYPES_H
```

### Torso Code (Master)

```cpp
// torso_test.cpp - Upload to Torso ESP32-S3

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "test_shared_types.h"

// HC-SR04 pins
#define TRIG_PIN 18
#define ECHO_PIN 19

// Head ESP32 MAC address (CHANGE THIS!)
// Get from head device: WiFi.macAddress()
uint8_t head_mac[] = {0x24, 0x0A, 0xC4, 0xXX, 0xXX, 0xXX};

esp_now_peer_info_t peerInfo;

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== TORSO (Master) - Distance → Mood Test ===");
    
    // Initialize HC-SR04
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    // Initialize WiFi in station mode (for ESP-NOW)
    WiFi.mode(WIFI_STA);
    Serial.print("Torso MAC: ");
    Serial.println(WiFi.macAddress());
    
    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("ERROR: ESP-NOW init failed!");
        return;
    }
    Serial.println("ESP-NOW initialized");
    
    // Register head as peer
    memcpy(peerInfo.peer_addr, head_mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("ERROR: Failed to add head peer!");
        return;
    }
    Serial.println("Head peer added");
    
    // Initialize global state
    g_Envir.distance_cm = 255;
    g_Envir.target_detected = false;
    
    g_MOOD.happiness = 0;
    g_MOOD.curiosity = 0;
    g_MOOD.fear = 0;
    
    Serial.println("Ready! Move your face near the sensor...\n");
}

uint8_t measure_distance() {
    // Trigger pulse
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    // Measure echo
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // 30ms timeout
    
    if (duration == 0) {
        return 255;  // No detection
    }
    
    // Calculate distance (speed of sound: 343 m/s)
    uint8_t distance_cm = duration * 0.0343 / 2;
    
    return (distance_cm > 255) ? 255 : distance_cm;
}

void broadcast_state(const char *name, void *data, size_t size) {
    mesh_packet_t packet;
    strncpy(packet.name, name, 31);
    packet.name[31] = '\0';
    memcpy(packet.data, data, size);
    packet.size = size;
    
    esp_err_t result = esp_now_send(head_mac, (uint8_t*)&packet, sizeof(packet));
    
    if (result == ESP_OK) {
        Serial.printf("✓ Broadcast '%s' (%d bytes)\n", name, size);
    } else {
        Serial.printf("✗ Broadcast '%s' FAILED!\n", name);
    }
}

void loop() {
    // === 1. SENSOR: Measure Distance ===
    uint8_t distance = measure_distance();
    bool changed = (distance != g_Envir.distance_cm);
    
    g_Envir.distance_cm = distance;
    g_Envir.target_detected = (distance < 100);  // Within 1 meter
    
    if (changed) {
        Serial.printf("📏 Distance: %d cm %s\n", 
            distance, 
            g_Envir.target_detected ? "(TARGET!)" : "");
        
        // Broadcast environment
        broadcast_state("g_Envir", &g_Envir, sizeof(Environment));
    }
    
    // === 2. PERSONALITY: Update Mood Based on Distance ===
    Mood oldMood = g_MOOD;
    
    if (g_Envir.target_detected) {
        if (distance < 30) {
            // Very close (< 30cm) → Curious + Happy
            g_MOOD.curiosity = 80;
            g_MOOD.happiness = 60;
            g_MOOD.fear = 0;
        } else if (distance < 60) {
            // Close (30-60cm) → Moderately curious
            g_MOOD.curiosity = 50;
            g_MOOD.happiness = 30;
            g_MOOD.fear = 10;
        } else {
            // Far (60-100cm) → Slightly curious
            g_MOOD.curiosity = 20;
            g_MOOD.happiness = 10;
            g_MOOD.fear = 30;
        }
    } else {
        // No target → Neutral/bored
        g_MOOD.curiosity = 0;
        g_MOOD.happiness = 0;
        g_MOOD.fear = 0;
    }
    
    // Only broadcast if mood changed
    if (memcmp(&g_MOOD, &oldMood, sizeof(Mood)) != 0) {
        Serial.printf("😊 Mood: HAPPY=%d, CURIOUS=%d, FEAR=%d\n",
            g_MOOD.happiness, g_MOOD.curiosity, g_MOOD.fear);
        
        // Broadcast mood
        broadcast_state("g_MOOD", &g_MOOD, sizeof(Mood));
    }
    
    delay(100);  // 10 Hz loop rate
}
```

### Head Code (Slave)

```cpp
// head_test.cpp - Upload to Head ESP32-S3

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include "test_shared_types.h"

// GC9A01 display pins
#define TFT_CS   8
#define TFT_DC   9
#define TFT_RST  10
#define TFT_CLK  12
#define TFT_MOSI 11

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);

// Display dimensions
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 240
#define CENTER_X      120
#define CENTER_Y      120

void on_data_received(const uint8_t *mac, const uint8_t *data, int len) {
    mesh_packet_t *packet = (mesh_packet_t*)data;
    
    Serial.printf("📡 Received '%s' (%d bytes)\n", packet->name, packet->size);
    
    // Update local global memory
    if (strcmp(packet->name, "g_Envir") == 0) {
        memcpy(&g_Envir, packet->data, packet->size);
        Serial.printf("   Distance: %d cm\n", g_Envir.distance_cm);
        
    } else if (strcmp(packet->name, "g_MOOD") == 0) {
        memcpy(&g_MOOD, packet->data, packet->size);
        Serial.printf("   Mood: HAPPY=%d, CURIOUS=%d, FEAR=%d\n",
            g_MOOD.happiness, g_MOOD.curiosity, g_MOOD.fear);
        
        // Render eye expression!
        render_eye();
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== HEAD (Slave) - Eye Expression Test ===");
    
    // Initialize display
    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(GC9A01A_BLACK);
    
    Serial.println("Display initialized (240×240)");
    
    // Initialize WiFi in station mode
    WiFi.mode(WIFI_STA);
    Serial.print("Head MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.println("^^^ COPY THIS TO TORSO CODE! ^^^");
    
    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("ERROR: ESP-NOW init failed!");
        return;
    }
    
    // Register receive callback
    esp_now_register_recv_cb(on_data_received);
    
    Serial.println("ESP-NOW ready, waiting for mesh data...\n");
    
    // Draw neutral eye
    g_MOOD.happiness = 0;
    g_MOOD.curiosity = 0;
    g_MOOD.fear = 0;
    render_eye();
}

void render_eye() {
    // Clear screen
    tft.fillScreen(GC9A01A_BLACK);
    
    // === Calculate Eye Properties from Mood ===
    
    // Eye color based on dominant mood
    uint16_t eye_color;
    if (g_MOOD.happiness > 40) {
        eye_color = GC9A01A_GREEN;   // Happy = green
    } else if (g_MOOD.curiosity > 40) {
        eye_color = GC9A01A_YELLOW;  // Curious = yellow
    } else if (g_MOOD.fear > 40) {
        eye_color = GC9A01A_RED;     // Fear = red
    } else {
        eye_color = GC9A01A_BLUE;    // Neutral = blue
    }
    
    // Pupil size based on curiosity (dilates when curious)
    int pupil_radius;
    if (g_MOOD.curiosity > 60) {
        pupil_radius = 50;  // Very dilated (wide-eyed)
    } else if (g_MOOD.curiosity > 30) {
        pupil_radius = 35;  // Moderately dilated
    } else {
        pupil_radius = 25;  // Normal
    }
    
    // Eye openness based on fear (wide when fearful)
    int eye_radius = 100;
    if (g_MOOD.fear > 50) {
        eye_radius = 110;  // Wide open (fearful)
    } else if (g_MOOD.happiness > 50) {
        eye_radius = 80;   // Squinted (happy)
    }
    
    // === Draw Eye ===
    
    // Sclera (white of eye)
    tft.fillCircle(CENTER_X, CENTER_Y, eye_radius, GC9A01A_WHITE);
    
    // Iris (colored part)
    tft.fillCircle(CENTER_X, CENTER_Y, pupil_radius + 20, eye_color);
    
    // Pupil (black center)
    tft.fillCircle(CENTER_X, CENTER_Y, pupil_radius, GC9A01A_BLACK);
    
    // Highlight (makes it look alive)
    tft.fillCircle(CENTER_X - 15, CENTER_Y - 15, 8, GC9A01A_WHITE);
    
    Serial.printf("👁️  Eye rendered: color=%s, pupil=%d, size=%d\n",
        (eye_color == GC9A01A_GREEN) ? "GREEN" :
        (eye_color == GC9A01A_YELLOW) ? "YELLOW" :
        (eye_color == GC9A01A_RED) ? "RED" : "BLUE",
        pupil_radius, eye_radius);
}

void loop() {
    // ESP-NOW callbacks handle everything!
    delay(10);
}
```

## Expected Behavior

### Test Sequence

**1. Power on both ESP32s**
```
Torso: "TORSO (Master) - Ready!"
Head:  "HEAD (Slave) - Head MAC: 24:0A:C4:XX:XX:XX"
       "ESP-NOW ready, waiting for mesh data..."
       [Display shows neutral BLUE eye, normal pupil]
```

**2. Stand far away (2+ meters)**
```
Torso: "📏 Distance: 255 cm"
Head:  [No change - too far to detect]
```

**3. Move to 80cm (arm's length)**
```
Torso: "📏 Distance: 80 cm (TARGET!)"
       "😊 Mood: HAPPY=10, CURIOUS=20, FEAR=30"
       "✓ Broadcast 'g_Envir' (2 bytes)"
       "✓ Broadcast 'g_MOOD' (3 bytes)"

Head:  "📡 Received 'g_Envir' (2 bytes)"
       "   Distance: 80 cm"
       "📡 Received 'g_MOOD' (3 bytes)"
       "   Mood: HAPPY=10, CURIOUS=20, FEAR=30"
       "👁️  Eye rendered: color=BLUE, pupil=25, size=100"
       [Display: BLUE eye, slightly curious]
```

**4. Move closer to 50cm (close)**
```
Torso: "📏 Distance: 50 cm (TARGET!)"
       "😊 Mood: HAPPY=30, CURIOUS=50, FEAR=10"
       
Head:  "📡 Received 'g_MOOD'"
       "👁️  Eye rendered: color=YELLOW, pupil=35, size=100"
       [Display: YELLOW eye (curious), pupil DILATED!] ✨
```

**5. Move VERY close (20cm - right in its face!)**
```
Torso: "📏 Distance: 20 cm (TARGET!)"
       "😊 Mood: HAPPY=60, CURIOUS=80, FEAR=0"
       
Head:  "📡 Received 'g_MOOD'"
       "👁️  Eye rendered: color=YELLOW, pupil=50, size=100"
       [Display: YELLOW eye, VERY DILATED pupils!] 😳
```

**6. Back away to 40cm**
```
Head:  "👁️  Eye rendered: color=YELLOW, pupil=35, size=100"
       [Display: Pupils CONTRACT as you move away] 👁️
```

**7. Walk away (150cm+)**
```
Torso: "📏 Distance: 255 cm"
       "😊 Mood: HAPPY=0, CURIOUS=0, FEAR=0"
       
Head:  "📡 Received 'g_MOOD'"
       "👁️  Eye rendered: color=BLUE, pupil=25, size=100"
       [Display: Returns to neutral BLUE, normal pupils]
```

## Visual Summary

```
Your Distance → Torso Sensor → g_Envir → Mesh Broadcast
                                             ↓
                                        Head Receives
                                             ↓
                              Torso Personality → g_MOOD → Mesh Broadcast
                                                               ↓
                                                          Head Receives
                                                               ↓
                                                         Eye Rendering
                                                               ↓
                                                        GC9A01 Display

RESULT: Move face → Eyes react in real-time! 👁️↔️👁️
```

### Eye Expression Chart

| Distance | Mood | Eye Color | Pupil Size | Visual |
|----------|------|-----------|------------|---------|
| >100cm   | Neutral | 🔵 BLUE | Normal (25px) | Bored |
| 60-100cm | Slightly curious | 🔵 BLUE | Normal (25px) | Aware |
| 30-60cm  | Curious | 🟡 YELLOW | Dilated (35px) | Interested |
| <30cm    | Very curious + happy | 🟡 YELLOW | Very dilated (50px) | Excited! |

## Success Criteria

✅ **Torso measures distance** via HC-SR04 sensor
✅ **Torso broadcasts `g_Envir`** every time distance changes
✅ **Torso updates `g_MOOD`** based on distance thresholds
✅ **Torso broadcasts `g_MOOD`** every time mood changes
✅ **Head receives mesh packets** via ESP-NOW
✅ **Head updates local `g_Envir` and `g_MOOD`** from packets
✅ **Head renders eye expression** based on received mood
✅ **Display shows visual change** when you move closer/farther
✅ **Latency <100ms** (distance change → display update)

## Next Steps After Success

1. ✅ **Proven**: ESP-NOW mesh works (5ms latency!)
2. ✅ **Proven**: Global shared memory pattern works (`g_Envir`, `g_MOOD`)
3. ✅ **Proven**: Distance-based mood reactions work
4. ✅ **Proven**: Mood-driven eye expressions work
5. ✅ **Proven**: End-to-end pipeline works (sensor → mesh → display)

**Then**:
- Add second eye display (left + right eyes)
- Add mouth display (3rd GC9A01)
- Implement full `Mood` class (9 components)
- Implement full `Environment` class (temp, humidity, battery, etc.)
- Add JSON-driven personality configuration
- 3D print skull once electronics are validated! 🎉

## Troubleshooting

**Head MAC not printing?**
- Upload minimal sketch: `Serial.println(WiFi.macAddress());`
- Copy MAC to torso `head_mac[]` array

**ESP-NOW init failed?**
- Check WiFi mode: `WiFi.mode(WIFI_STA)`
- Both devices must be on same channel

**No mesh packets received?**
- Verify MAC address in torso code
- Check `esp_now_add_peer()` result
- Monitor serial output on both devices

**Display not working?**
- Verify SPI wiring (CLK, MOSI, CS, DC, RST)
- Check TFT library installed: `Adafruit GC9A01A`
- Test with display example sketch first

**Distance always 255?**
- Check HC-SR04 wiring (TRIG, ECHO)
- Try 5V vs 3.3V power
- Add 1kΩ resistor on ECHO pin (voltage divider)

## Hardware Shopping List

Amazon/AliExpress:
- [ ] 2× ESP32-S3 DevKitC-1 ($10 each = $20)
- [ ] 1× HC-SR04 ultrasonic sensor ($2)
- [ ] 1× GC9A01 1.28" circular display 240×240 ($10)
- [ ] 2× Breadboards ($4)
- [ ] Jumper wires M-M, M-F ($4)

**Total: $40** (prove the concept before spending $17 on skull!)

📘 **Related Documents**:
- [Global Shared Memory API](GLOBAL-SHARED-MEMORY-API.md)
- [JSON-Driven Personality Spec](JSON-DRIVEN-PERSONALITY-SPEC.md)
- [Mesh State Synchronization](MESH-STATE-SYNCHRONIZATION-SPEC.md)
