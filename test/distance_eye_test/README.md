# Distance → Eye Expression Test

## Quick Start - Upload Instructions

### Step 1: Upload HEAD code first (to get MAC address)

```powershell
cd test\distance_eye_test
pio run -e head -t upload -t monitor
```

**Expected output:**
```
=== HEAD (Slave) - Eye Expression Test ===
✓ Display initialized (240×240 circular)
✓ WiFi initialized - Head MAC: 24:0A:C4:XX:XX:XX
  ^^^ COPY THIS MAC ADDRESS TO torso_test.cpp! ^^^
✓ ESP-NOW initialized
🚀 System ready, waiting for mesh data from torso...
```

**IMPORTANT**: Copy the MAC address (e.g., `24:0A:C4:12:34:56`)

### Step 2: Update torso code with HEAD MAC address

Edit `src/torso_test.cpp` line 14:

```cpp
// BEFORE:
uint8_t head_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// AFTER (example - use YOUR head's MAC):
uint8_t head_mac[] = {0x24, 0x0A, 0xC4, 0x12, 0x34, 0x56};
```

### Step 3: Upload TORSO code

```powershell
pio run -e torso -t upload -t monitor
```

**Expected output:**
```
=== TORSO (Master) - Distance → Mood Test ===
✓ HC-SR04 sensor initialized
✓ WiFi initialized - Torso MAC: 24:0A:C4:YY:YY:YY
✓ ESP-NOW initialized
✓ Head peer registered
🚀 System ready! Move your face near the sensor...

📏 Distance: 255 cm
📏 Distance:  45 cm 🎯 (TARGET DETECTED)
😊 Mood updated: HAPPY=+30, CURIOUS=+50, FEAR=+10
  ✓ Broadcast 'g_Envir' (2 bytes)
  ✓ Broadcast 'g_MOOD' (3 bytes)
```

### Step 4: Monitor HEAD display

The HEAD ESP32 should show:
```
📡 Received 'g_Envir' (2 bytes)
   📏 Distance: 45 cm 🎯
📡 Received 'g_MOOD' (3 bytes)
   😊 Mood: HAPPY=+30, CURIOUS=+50, FEAR=+10
👁️  Eye rendered in 42 ms:
   Color: YELLOW, Pupil: DILATED (35px), Eye: NORMAL (100px)
```

**Display shows**: Yellow eye with dilated pupils! 😳

## Wiring

### Torso ESP32-S3 + HC-SR04 Sensor

```
HC-SR04 Pins → ESP32-S3 Pins
────────────────────────────
VCC  → 5V (VBUS)
GND  → GND
TRIG → GPIO 18
ECHO → GPIO 19
```

### Head ESP32-S3 + GC9A01 Display

```
GC9A01 Pins → ESP32-S3 Pins
────────────────────────────
VCC → 3.3V
GND → GND
SCL → GPIO 12 (SPI CLK)
SDA → GPIO 11 (SPI MOSI)
RES → GPIO 10 (Reset)
DC  → GPIO 9  (Data/Command)
CS  → GPIO 8  (Chip Select)
BL  → 3.3V (Backlight - always on)
```

## Test Sequence

1. **Power on both ESP32s** (USB-C cables)
2. **Stand far away** (2+ meters) → Display shows neutral blue eye
3. **Move to 80cm** → Eye stays blue (slightly aware)
4. **Move to 50cm** → Eye turns YELLOW, pupils dilate! 👁️
5. **Move to 20cm** (right in its face) → Pupils VERY DILATED! 😳
6. **Back away** → Pupils contract, returns to blue

## Expected Behavior by Distance

| Distance | Mood | Eye Color | Pupil | Visual |
|----------|------|-----------|-------|--------|
| >100cm   | Neutral | 🔵 BLUE | Normal | Bored |
| 60-100cm | Slightly curious | 🔵 BLUE | Normal | Aware |
| 30-60cm  | Curious | 🟡 YELLOW | Dilated | Interested |
| <30cm    | Very curious | 🟡 YELLOW | Very dilated | Excited! |

## Troubleshooting

### "ESP-NOW init failed"
- Both devices must use `WiFi.mode(WIFI_STA)`
- Restart both ESP32s

### "Failed to add head peer"
- Did you update `head_mac[]` in torso code?
- Check MAC address matches HEAD device exactly

### "No mesh packets received"
- Verify HEAD MAC copied correctly (colons vs commas!)
- Both ESP32s must be powered on
- Try moving devices closer together

### Display blank/white
- Check SPI wiring (CLK, MOSI, CS, DC, RST)
- Verify 3.3V power (NOT 5V!)
- Test with Adafruit GC9A01A library examples first

### Distance always 255
- Check HC-SR04 wiring
- Verify 5V power to sensor
- Ensure nothing blocking sensor

## Files

```
test/distance_eye_test/
├── platformio.ini           ← PlatformIO config (2 environments)
├── include/
│   └── test_shared_types.h  ← Shared data structures
└── src/
    ├── torso_test.cpp       ← Master (sensor + mood + ESP-NOW TX)
    └── head_test.cpp        ← Slave (ESP-NOW RX + display)
```

## Hardware Cost

- 2× ESP32-S3 DevKitC-1: $20
- 1× HC-SR04 sensor: $2
- 1× GC9A01 display: $10
- Breadboards + wires: $8

**Total: $40**

## Success Criteria

✅ Torso measures distance via HC-SR04
✅ Torso broadcasts `g_Envir` via ESP-NOW
✅ Torso updates `g_MOOD` based on distance
✅ Torso broadcasts `g_MOOD` via ESP-NOW
✅ Head receives mesh packets
✅ Head renders eye expression
✅ Display changes when you move closer/farther
✅ Latency < 100ms

**If all pass → ESP-NOW mesh PROVEN! Ready for full P32 system!** 🎉
