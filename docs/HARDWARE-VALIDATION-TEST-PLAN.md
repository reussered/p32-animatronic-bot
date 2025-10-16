# Hardware Validation Test Plan - Prove Core Technology First

## Philosophy

**Don't 3D print until electronics are validated!**

Before committing to physical fabrication, we must prove:
1. ✅ ESP-NOW mesh network works reliably
2. ✅ Global shared state synchronizes across nodes
3. ✅ Display rendering achieves target frame rates
4. ✅ Power consumption matches budget predictions
5. ✅ Component architecture scales as designed

**Test with minimal hardware first, then scale up.**

---

## Test Hardware Requirements

### Minimum Viable Test Setup

| Component | Quantity | Purpose | Cost |
|-----------|----------|---------|------|
| **ESP32-S3 DevKitC-1** | 2 | Master (torso) + Slave (head) | $20 |
| **GC9A01 1.28" LCD** | 1 | Display rendering test | $10 |
| **Breadboard** | 2 | Prototype wiring | $5 |
| **Jumper wires** | 20 | Connections | $3 |
| **USB cables** | 2 | Power + programming | $5 |
| **Multimeter** | 1 | Power measurement | $15 |
| **TOTAL** | - | Validate core tech | **$58** |

**ROI**: Spend $58 now to avoid wasting $17+ on 3D prints that might not work.

---

## Test Sequence

### Phase 1: ESP-NOW Mesh Communication (2 hours)

**Objective**: Prove two ESP32-S3 chips can exchange packets via ESP-NOW.

**Hardware Setup**:
```
┌─────────────────────┐         WiFi (ESP-NOW)        ┌─────────────────────┐
│  ESP32-S3 #1        │ ◄──────────────────────────► │  ESP32-S3 #2        │
│  (TORSO MASTER)     │         Channel 6            │  (HEAD SLAVE)       │
│                     │         2.4 GHz              │                     │
│  Role: Coordinator  │                              │  Role: Peripheral   │
│  USB → PC (logging) │                              │  USB → PC (logging) │
└─────────────────────┘                              └─────────────────────┘
```

**Test Code** (simplified from full system):

```c
// torso_master_test.c

#include "esp_now.h"
#include "esp_wifi.h"

uint8_t head_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  // Broadcast initially

typedef struct {
    uint32_t version;
    uint32_t timestamp_ms;
    int8_t mood_happiness;
    int8_t mood_anger;
    uint8_t distance_cm;
} test_packet_t;

void app_main(void) {
    esp_wifi_init();
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    
    esp_now_init();
    
    esp_now_peer_info_t peer = {
        .channel = 6,
        .ifidx = WIFI_IF_STA,
    };
    memcpy(peer.peer_addr, head_mac, 6);
    esp_now_add_peer(&peer);
    
    test_packet_t packet;
    uint32_t loop_count = 0;
    
    while(1) {
        packet.version = loop_count;
        packet.timestamp_ms = esp_timer_get_time() / 1000;
        packet.mood_happiness = (loop_count % 200) - 100;  // Oscillate -100 to +100
        packet.mood_anger = 50;
        packet.distance_cm = 42;
        
        esp_now_send(head_mac, (uint8_t*)&packet, sizeof(packet));
        
        ESP_LOGI(TAG, "MASTER sent packet v%lu: happiness=%d", 
            loop_count, packet.mood_happiness);
        
        loop_count++;
        vTaskDelay(pdMS_TO_TICKS(100));  // 10 Hz
    }
}
```

```c
// head_slave_test.c

#include "esp_now.h"
#include "esp_wifi.h"

typedef struct {
    uint32_t version;
    uint32_t timestamp_ms;
    int8_t mood_happiness;
    int8_t mood_anger;
    uint8_t distance_cm;
} test_packet_t;

void on_receive(const uint8_t *mac, const uint8_t *data, int len) {
    test_packet_t *packet = (test_packet_t*)data;
    
    ESP_LOGI(TAG, "SLAVE received v%lu: happiness=%d, anger=%d, distance=%d", 
        packet->version, 
        packet->mood_happiness, 
        packet->mood_anger, 
        packet->distance_cm);
}

void app_main(void) {
    esp_wifi_init();
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    
    esp_now_init();
    esp_now_register_recv_cb(on_receive);
    
    ESP_LOGI(TAG, "SLAVE listening for packets...");
    
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));  // Just receive, no transmit
    }
}
```

**Success Criteria**:
- ✅ Master sends 10 packets/second
- ✅ Slave receives >95% of packets (allow 5% packet loss)
- ✅ Latency <10ms (check timestamp deltas)
- ✅ No crashes after 10 minutes continuous operation

**Expected Serial Output**:

Master:
```
MASTER sent packet v0: happiness=-100
MASTER sent packet v1: happiness=-99
MASTER sent packet v2: happiness=-98
...
```

Slave:
```
SLAVE received v0: happiness=-100, anger=50, distance=42
SLAVE received v1: happiness=-99, anger=50, distance=42
SLAVE received v2: happiness=-98, anger=50, distance=42
...
```

**If Test Fails**: Debug ESP-NOW configuration, check WiFi channel, verify MAC addresses.

---

### Phase 2: Global Shared State Synchronization (3 hours)

**Objective**: Prove `p32_shared_state_t` struct syncs correctly across mesh.

**Hardware Setup**: Same as Phase 1

**Test Code**: Use actual `p32_shared_state.h` from project

```c
// torso_master_sync_test.c

#include "p32_shared_state.h"
#include "esp_now.h"

p32_shared_state_t g_shared_state = {0};
uint32_t g_loopCount = 0;

void app_main(void) {
    // Initialize ESP-NOW (same as Phase 1)
    
    while(1) {
        // Simulate mood changes
        g_shared_state.mood_serialized[MOOD_HAPPINESS] = 
            (g_loopCount % 200) - 100;  // Oscillate
        
        g_shared_state.mood_serialized[MOOD_ANGER] = 
            (g_loopCount / 10) % 100;   // Slow ramp
        
        // Simulate sensor data
        g_shared_state.distance_cm = 30 + (g_loopCount % 50);
        g_shared_state.temperature_c = 22;
        
        // Update metadata
        g_shared_state.version++;
        g_shared_state.timestamp_ms = esp_timer_get_time() / 1000;
        
        // Broadcast state
        esp_now_send(NULL, (uint8_t*)&g_shared_state, 
            sizeof(p32_shared_state_t));
        
        ESP_LOGI(TAG, "MASTER v%lu: happiness=%d, anger=%d, distance=%d",
            g_shared_state.version,
            g_shared_state.mood_serialized[MOOD_HAPPINESS],
            g_shared_state.mood_serialized[MOOD_ANGER],
            g_shared_state.distance_cm);
        
        g_loopCount++;
        vTaskDelay(pdMS_TO_TICKS(100));  // 10 Hz
    }
}
```

```c
// head_slave_sync_test.c

#include "p32_shared_state.h"
#include "esp_now.h"

p32_shared_state_t g_shared_state = {0};
uint32_t g_loopCount = 0;

void on_state_received(const uint8_t *mac, const uint8_t *data, int len) {
    if (len == sizeof(p32_shared_state_t)) {
        memcpy(&g_shared_state, data, sizeof(p32_shared_state_t));
        
        ESP_LOGI(TAG, "SLAVE synced v%lu: happiness=%d, anger=%d, distance=%d",
            g_shared_state.version,
            g_shared_state.mood_serialized[MOOD_HAPPINESS],
            g_shared_state.mood_serialized[MOOD_ANGER],
            g_shared_state.distance_cm);
    }
}

void app_main(void) {
    // Initialize ESP-NOW (same as Phase 1)
    esp_now_register_recv_cb(on_state_received);
    
    while(1) {
        // Slave components can now read g_shared_state
        // Simulate component reading state
        if (g_loopCount % 10 == 0) {
            ESP_LOGI(TAG, "SLAVE component read: happiness=%d",
                g_shared_state.mood_serialized[MOOD_HAPPINESS]);
        }
        
        g_loopCount++;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

**Success Criteria**:
- ✅ Slave's `g_shared_state` matches master within 100ms
- ✅ Mood values sync correctly (oscillating happiness visible)
- ✅ Sensor data syncs correctly (distance changes visible)
- ✅ Version numbers increment monotonically
- ✅ No memory corruption after 1 hour continuous sync

**Expected Serial Output**:

Master:
```
MASTER v1: happiness=-100, anger=0, distance=30
MASTER v2: happiness=-99, anger=0, distance=31
MASTER v3: happiness=-98, anger=0, distance=32
```

Slave:
```
SLAVE synced v1: happiness=-100, anger=0, distance=30
SLAVE component read: happiness=-100
SLAVE synced v2: happiness=-99, anger=0, distance=31
SLAVE synced v3: happiness=-98, anger=0, distance=32
```

**If Test Fails**: Check struct packing, verify size matches, add checksum validation.

---

### Phase 3: Display Rendering Performance (4 hours)

**Objective**: Prove GC9A01 display can render at 60 FPS from synchronized state.

**Hardware Setup**:
```
┌─────────────────────────────────────────────────┐
│  ESP32-S3 #2 (HEAD SLAVE)                      │
│                                                 │
│  GPIO 13 (MOSI)  ─────────────► DIN   ┌──────┐│
│  GPIO 12 (MISO)  ◄────────────  DOUT  │GC9A01││
│  GPIO 11 (SCK)   ─────────────► CLK   │ 1.28"││
│  GPIO 10 (CS)    ─────────────► CS    │ LCD  ││
│  GPIO 9  (DC)    ─────────────► DC    └──────┘│
│  GPIO 8  (RST)   ─────────────► RST           │
│  3.3V            ─────────────► VCC           │
│  GND             ─────────────► GND           │
│                                                 │
│  USB → PC (power + logging)                    │
└─────────────────────────────────────────────────┘
```

**Test Code**:

```c
// head_display_test.c

#include "p32_shared_state.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

p32_shared_state_t g_shared_state = {0};
spi_device_handle_t spi_display;

// Simple 240x240 frame buffer (RGB565 format)
uint16_t framebuffer[240*240];

void init_gc9a01() {
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = 13,
        .miso_io_num = 12,
        .sclk_io_num = 11,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 240*240*2,
    };
    spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 40*1000*1000,  // 40 MHz
        .mode = 0,
        .spics_io_num = 10,
        .queue_size = 1,
    };
    spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_display);
    
    // Initialize GC9A01 (send init commands)
    // ... (full init sequence omitted for brevity)
}

void render_mood_face(int8_t happiness, int8_t anger) {
    // Clear framebuffer
    uint16_t bg_color = 0x0000;  // Black
    for(int i = 0; i < 240*240; i++) {
        framebuffer[i] = bg_color;
    }
    
    // Draw simple colored circle based on mood
    uint16_t color;
    if (happiness > 50) {
        color = 0x07E0;  // Green (happy)
    } else if (anger > 50) {
        color = 0xF800;  // Red (angry)
    } else {
        color = 0x001F;  // Blue (neutral)
    }
    
    // Draw circle (simplified - actual would use proper circle algorithm)
    for(int y = 0; y < 240; y++) {
        for(int x = 0; x < 240; x++) {
            int dx = x - 120;
            int dy = y - 120;
            if (dx*dx + dy*dy < 80*80) {
                framebuffer[y*240 + x] = color;
            }
        }
    }
    
    // Send framebuffer to display
    spi_transaction_t trans = {
        .length = 240*240*2*8,  // bits
        .tx_buffer = framebuffer,
    };
    spi_device_transmit(spi_display, &trans);
}

void on_state_received(const uint8_t *mac, const uint8_t *data, int len) {
    if (len == sizeof(p32_shared_state_t)) {
        memcpy(&g_shared_state, data, sizeof(p32_shared_state_t));
        
        // Immediately render updated mood
        render_mood_face(
            g_shared_state.mood_serialized[MOOD_HAPPINESS],
            g_shared_state.mood_serialized[MOOD_ANGER]
        );
    }
}

void app_main(void) {
    init_gc9a01();
    
    // Initialize ESP-NOW (same as Phase 2)
    esp_now_register_recv_cb(on_state_received);
    
    ESP_LOGI(TAG, "Display test ready. Waiting for mood updates...");
    
    uint32_t frame_count = 0;
    uint32_t last_fps_time = esp_timer_get_time();
    
    while(1) {
        // Render loop runs independently
        render_mood_face(
            g_shared_state.mood_serialized[MOOD_HAPPINESS],
            g_shared_state.mood_serialized[MOOD_ANGER]
        );
        
        frame_count++;
        
        // Calculate FPS every second
        uint32_t now = esp_timer_get_time();
        if (now - last_fps_time > 1000000) {  // 1 second
            float fps = frame_count * 1000000.0f / (now - last_fps_time);
            ESP_LOGI(TAG, "FPS: %.1f", fps);
            frame_count = 0;
            last_fps_time = now;
        }
        
        vTaskDelay(pdMS_TO_TICKS(16));  // Target 60 FPS
    }
}
```

**Success Criteria**:
- ✅ Display shows solid colors (proves SPI communication works)
- ✅ Colors change based on synchronized mood (green happy, red angry, blue neutral)
- ✅ Achieves >30 FPS (acceptable), target 60 FPS
- ✅ No screen flicker or corruption
- ✅ Mood changes from master visible within 100ms on display

**Expected Serial Output**:
```
Display test ready. Waiting for mood updates...
FPS: 58.3
FPS: 59.1
FPS: 60.2
FPS: 59.8
```

**Visual Test**:
- Master cycles happiness from -100 to +100
- Display transitions: Blue → Green → Blue → Red → Blue
- Smooth color transitions visible

**If Test Fails**: 
- Check SPI wiring (use logic analyzer if available)
- Reduce SPI clock speed from 40 MHz to 20 MHz
- Verify display init sequence
- Profile render_mood_face() to find bottlenecks

---

### Phase 4: End-to-End Mood Synchronization (2 hours)

**Objective**: Prove complete pipeline: Master updates mood → Mesh sync → Display shows mood.

**Hardware Setup**: Combined Phase 2 + Phase 3

**Test Procedure**:

1. **Master generates mood sequence**:
   ```
   0-5 sec:   Happiness +100 (very happy)
   5-10 sec:  Anger +100 (very angry)
   10-15 sec: Neutral (all zeros)
   15-20 sec: Curiosity +100 (interested)
   ```

2. **Monitor slave display**:
   - Should show GREEN (0-5s)
   - Should show RED (5-10s)
   - Should show BLUE (10-15s)
   - Should show YELLOW (15-20s)

3. **Measure latency**:
   - Master logs timestamp when mood changes
   - Slave logs timestamp when display updates
   - Calculate delta (target <100ms)

**Success Criteria**:
- ✅ Display colors match mood state
- ✅ Latency <100ms from mood change to display update
- ✅ No dropped state updates during 20 minute test
- ✅ System stable (no crashes, no memory leaks)

**Expected Serial Output**:

Master:
```
[0.000] MASTER: Setting happiness=100
[5.000] MASTER: Setting anger=100
[10.000] MASTER: Setting neutral
[15.000] MASTER: Setting curiosity=100
```

Slave:
```
[0.083] SLAVE: Display updated to GREEN (happiness=100)
[5.091] SLAVE: Display updated to RED (anger=100)
[10.078] SLAVE: Display updated to BLUE (neutral)
[15.095] SLAVE: Display updated to YELLOW (curiosity=100)
```

**Latency Analysis**:
- Avg: 85ms ✅ (target <100ms)
- Max: 95ms ✅
- Min: 78ms ✅

---

### Phase 5: Power Consumption Validation (1 hour)

**Objective**: Verify actual power draw matches POWER-BUDGET-SPEC.md predictions.

**Hardware Setup**:
```
                      ┌──────────┐
[USB 5V Supply] ──► │Multimeter│ ──► [ESP32-S3 + Display]
                      │(Current) │
                      └──────────┘
```

**Test Procedure**:

1. **Measure idle current** (no display updates):
   - Expected: ~120 mA (ESP32 idle + WiFi)
   
2. **Measure active current** (60 FPS display rendering):
   - Expected: ~200 mA (ESP32 active + display updates)
   
3. **Measure peak current** (WiFi TX + display update):
   - Expected: ~350 mA (WiFi transmit surge)

**Success Criteria**:
- ✅ Idle current within 20% of prediction (96-144 mA)
- ✅ Active current within 20% of prediction (160-240 mA)
- ✅ Peak current within 30% of prediction (245-455 mA)

**Expected Results**:

| State | Predicted | Measured | Delta | Status |
|-------|-----------|----------|-------|--------|
| Idle | 120 mA | 135 mA | +12% | ✅ Within tolerance |
| Active | 200 mA | 215 mA | +7% | ✅ Within tolerance |
| Peak | 350 mA | 380 mA | +8% | ✅ Within tolerance |

**If Significantly Higher**: 
- Check for inefficient code (busy-wait loops)
- Verify display backlight PWM (lower if too bright)
- Disable unnecessary WiFi features

**If Significantly Lower**:
- Great! Update POWER-BUDGET-SPEC.md with actual values
- Longer battery runtime than expected

---

## Test Schedule

**Recommended Sequence** (over 2 days):

### Day 1: Communication Layer (6 hours)
- Morning: Phase 1 - ESP-NOW mesh (2 hrs)
- Afternoon: Phase 2 - Shared state sync (3 hrs)
- Evening: Debug any issues, document findings (1 hr)

### Day 2: Application Layer (7 hours)
- Morning: Phase 3 - Display rendering (4 hrs)
- Afternoon: Phase 4 - End-to-end test (2 hrs)
- Evening: Phase 5 - Power validation (1 hr)

**Total Investment**: 13 hours of testing, $58 in hardware

**ROI**: Catch critical issues BEFORE printing $17+ skull!

---

## Go/No-Go Decision Criteria

### ✅ **GO** - Proceed to Skull Fabrication

All of these must be TRUE:
- ✅ ESP-NOW packet delivery >95%
- ✅ Shared state syncs within 100ms
- ✅ Display renders at >30 FPS
- ✅ End-to-end latency <100ms
- ✅ Power consumption within 30% of predictions
- ✅ No crashes during 1 hour stress test

**If GO**: Proceed with clean_goblin_skull.scad → STL → 3D print

### ❌ **NO-GO** - Debug Issues First

Any of these are TRUE:
- ❌ Packet loss >10%
- ❌ Sync latency >500ms
- ❌ Display FPS <15
- ❌ End-to-end latency >500ms
- ❌ Power consumption >50% higher than predicted
- ❌ Crashes or memory corruption

**If NO-GO**: Debug and fix issues BEFORE printing physical parts

---

## Risk Mitigation

### Common Issues and Solutions

| Issue | Symptom | Solution |
|-------|---------|----------|
| **High packet loss** | <80% delivery | Reduce distance, check WiFi channel congestion |
| **Slow sync** | >500ms latency | Increase ESP-NOW broadcast rate, reduce packet size |
| **Low FPS** | <15 FPS | Optimize render code, reduce resolution, use DMA |
| **High power** | >150% of predicted | Disable unused peripherals, lower clock speed |
| **Crashes** | Reboots/watchdog | Increase stack size, fix memory leaks |

### Fallback Plans

If critical component fails:

1. **ESP-NOW unreliable**: 
   - Fallback: Use UART or SPI between ESP32s (wired, not wireless)
   - Implication: Limits mobility, but proves concept
   
2. **Display too slow**:
   - Fallback: Use simpler graphics (lines/shapes vs full bitmaps)
   - Implication: Less detailed expressions, but functional
   
3. **Power too high**:
   - Fallback: Larger battery (3S 10000mAh) or reduce features
   - Implication: Heavier robot or shorter feature set

---

## Documentation Requirements

After each test phase, document:

1. **Test Results**:
   - Pass/Fail status
   - Measured values vs predictions
   - Screenshots of serial output
   - Photos of hardware setup
   
2. **Issues Encountered**:
   - Description of problem
   - Root cause analysis
   - Solution implemented
   
3. **Lessons Learned**:
   - What worked well
   - What needs improvement
   - Design changes needed

**Create**: `docs/TEST-RESULTS-YYYY-MM-DD.md` after each test session

---

## Next Steps After Validation

### If All Tests Pass ✅

1. Commit test code to `src/tests/` directory
2. Update POWER-BUDGET-SPEC.md with actual measurements
3. Generate clean_goblin_skull.scad → STL
4. Send STL to 3D printer
5. While printing (8-12 hrs), start designing component shells
6. Begin writing dispatch table documentation

### If Tests Fail ❌

1. Document failure mode in TEST-RESULTS.md
2. Create GitHub issue with reproduction steps
3. Debug and fix root cause
4. Re-run failed test phase
5. Do NOT proceed to 3D printing until tests pass

---

## Success Metrics

**Test coverage goals**:
- [ ] 100% of critical path tested (mesh + display + power)
- [ ] 80%+ of component functions unit tested
- [ ] 1 hour continuous operation without crashes
- [ ] Power consumption within 30% of predictions
- [ ] End-to-end latency meets <100ms target

**Confidence level before fabrication**:
- Low (<70%): More testing needed
- Medium (70-90%): Acceptable, proceed with caution
- High (>90%): Confident, proceed to fabrication ✅

---

## Conclusion

**Principle**: "Test software extensively, build hardware once."

By investing 13 hours and $58 in validation testing, we:
- ✅ Catch critical issues early
- ✅ Validate power budget assumptions
- ✅ Prove mesh architecture works
- ✅ Build confidence before fabrication
- ✅ Save money on failed 3D prints

**Next Command**: Order 2× ESP32-S3 DevKitC-1 and 1× GC9A01 display from Amazon/Adafruit.

📘 **Related Documents**:
- [Power Budget Specification](POWER-BUDGET-SPEC.md) - Predicted power consumption
- [Component Function Signatures](COMPONENT-FUNCTION-SIGNATURES.md) - NO ARGUMENTS pattern
- [Mesh State Synchronization](MESH-STATE-SYNCHRONIZATION-SPEC.md) - Shared state design
- [Distributed Processing Architecture](DISTRIBUTED-PROCESSING-ARCHITECTURE.md) - Master/slave roles
