# Verified Hardware Wiring - October 15, 2025

## HC-SR04 Ultrasonic Sensor (Nose)
| Signal | GPIO | Verified |
|--------|------|----------|
| VCC    | 5V   | ✅ User confirmed |
| GND    | GND  | ✅ User confirmed |
| TRIG   | GPIO4 | ✅ User confirmed |
| ECHO   | GPIO5 | ✅ User confirmed |

**Code Location:** `src/components/goblin_nose.c`
- Line: `gpio_set_level(4, 1);` (trigger)
- Line: `while (gpio_get_level(5) == 0)` (echo)

## GC9A01 Display - Shared Pins
| Signal | GPIO | Verified |
|--------|------|----------|
| SDA (MOSI) | GPIO13 | ✅ User confirmed |
| SCL (CLK)  | GPIO12 | ✅ User confirmed (NOT GPIO14) |
| DC         | GPIO2  | ✅ User confirmed |
| RST        | GPIO21 | ✅ User confirmed |
| VCC        | **5V** | ✅ User confirmed (connected to main 5V power bus) |
| GND        | GND    | ✅ |

**Note:** Displays powered by 5V, but logic signals from ESP32-S3 are 3.3V (this is safe - GC9A01 accepts both)

## GC9A01 Display - Left Eye (Unique)
| Signal | GPIO | Verified |
|--------|------|----------|
| CS     | GPIO15 | ✅ User confirmed |

**Code Location:** `src/components/goblin_eye_left.c`
- Line: `#define PIN_CS 15`

## GC9A01 Display - Right Eye (Unique)
| Signal | GPIO | Verified |
|--------|------|----------|
| CS     | GPIO16 | ✅ User confirmed |

**Code Location:** `src/components/goblin_eye_right.c`
- Line: `#define PIN_CS 16`

## Updated Configuration Files
✅ `src/components/goblin_eye_left.c` - PIN_CLK = 12
✅ `src/components/goblin_eye_right.c` - PIN_CLK = 12
✅ `config/components/interfaces/spi_bus_vspi.json` - "clk": 12

## Pin Usage Summary
```
GPIO2  = DC (shared by both displays)
GPIO4  = HC-SR04 Trigger
GPIO5  = HC-SR04 Echo
GPIO12 = SPI Clock (shared by both displays)
GPIO13 = SPI MOSI (shared by both displays)
GPIO15 = Left Eye CS
GPIO16 = Right Eye CS
GPIO21 = RST (shared by both displays)
```

## Physical Board Layout (ESP32-S3-DevKitC-1)
```
USB Port (top)
     |
    3V3
    [obscured pin - NOT USED]
    GPIO13 ← SDA wire here
    GPIO12 ← SCL wire here (below GPIO13)
    ...
    GPIO2  ← DC wire
    ...
    GPIO4  ← HC-SR04 TRIG
    GPIO5  ← HC-SR04 ECHO
    ...
    GPIO15 ← Left Eye CS
    GPIO16 ← Right Eye CS
    ...
    GPIO21 ← RST wire
```

## CRITICAL: GPIO Restrictions on ESP32-S3
❌ **GPIO26-32 are FORBIDDEN** (reserved for flash/PSRAM)
✅ All pins used (2, 4, 5, 12, 13, 15, 16, 21) are SAFE

## Next Steps
1. Build firmware: `pio run`
2. Upload: `pio run -t upload`
3. Monitor: `pio device monitor`
4. Expected behavior:
   - Displays initialize (should see log messages)
   - Displays show color patterns based on eye openness
   - HC-SR04 measures distance
   - Eyes change animation when face approaches (<30cm)
