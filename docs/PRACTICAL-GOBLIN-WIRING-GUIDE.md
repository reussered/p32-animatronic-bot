# P32 Goblin Head - Practical Wiring Guide
**Updated for Actual Hardware: GC9A01 Displays + HW-496 Microphones**

---

## Your Actual Hardware

### GC9A01 Round Displays (2x for Eyes)
**Exposed Pins on Your Displays:**
- **SCK** (Serial Clock) = SPI CLK
- **SDA** (Serial Data) = SPI MOSI  
- **DC** (Data/Command) = Also called RS or A0
- **CS** (Chip Select) = Device select
- **RST** (Reset) = Hardware reset
- **VCC** (Power) = 3.3V-5V
- **GND** (Ground)

### HW-496 Electret Microphone Modules (2x for Ears)
**Pins:**
- **VCC** = 3.3V-5V power
- **GND** = Ground
- **AO** (Analog Out) = Audio signal (connect to ESP32 ADC)
- **DO** (Digital Out) = Not used for audio capture

### HC-SR04 Ultrasonic Sensor (1x for Nose)
**Pins:**
- **VCC** = 5V power
- **GND** = Ground
- **TRIG** = Trigger input
- **ECHO** = Echo output

**⚠️ MOUTH DISPLAY REMOVED** - Your display has no exposed pins, so we'll use just 2 eyes.

---

## Complete Pin Assignment

### ESP32-S3-DevKitC-1 GPIO Usage

```
┌────────────────────────────────────────────────────────┐
│              ESP32-S3-DevKitC-1                        │
├────────────────────────────────────────────────────────┤
│                                                        │
│  SPI BUS (Shared by both eye displays):               │
│  ├── GPIO 12: MISO (not used by GC9A01)               │
│  ├── GPIO 13: MOSI → SDA on displays                  │
│  └── GPIO 14: CLK  → SCK on displays                  │
│                                                        │
│  LEFT EYE DISPLAY (GC9A01):                           │
│  ├── GPIO 15: CS  (Chip Select)                       │
│  ├── GPIO 18: DC  (Data/Command)                      │
│  └── GPIO 19: RST (Reset)                             │
│                                                        │
│  RIGHT EYE DISPLAY (GC9A01):                          │
│  ├── GPIO 16: CS  (Chip Select)                       │
│  ├── GPIO 20: DC  (Data/Command)                      │
│  └── GPIO 21: RST (Reset)                             │
│                                                        │
│  NOSE SENSOR (HC-SR04):                               │
│  ├── GPIO 9:  TRIG (Trigger pulse)                    │
│  └── GPIO 10: ECHO (Echo response)                    │
│                                                        │
│  LEFT EAR MICROPHONE (HW-496):                        │
│  └── GPIO 1 (ADC1_CH0): AO (Analog audio)             │
│                                                        │
│  RIGHT EAR MICROPHONE (HW-496):                       │
│  └── GPIO 2 (ADC1_CH1): AO (Analog audio)             │
│                                                        │
└────────────────────────────────────────────────────────┘

TOTAL GPIO USAGE: 13 pins (plenty of headroom)
```

---

## Pin Function Mapping Table

| ESP32 GPIO | Display Pin | Function | Component |
|------------|-------------|----------|-----------|
| **13** | **SDA** | SPI MOSI (shared) | Both Eyes |
| **14** | **SCK** | SPI CLK (shared) | Both Eyes |
| **15** | **CS** | Chip Select | Left Eye |
| **18** | **DC** | Data/Command | Left Eye |
| **19** | **RST** | Reset | Left Eye |
| **16** | **CS** | Chip Select | Right Eye |
| **20** | **DC** | Data/Command | Right Eye |
| **21** | **RST** | Reset | Right Eye |

**KEY TRANSLATION:**
- **Your "SDA"** = ESP32 "MOSI" (GPIO 13)
- **Your "SCK"** = ESP32 "CLK" (GPIO 14)
- **Your "DC"** = ESP32 "DC" (GPIO 18/20)
- **Your "CS"** = ESP32 "CS" (GPIO 15/16)
- **Your "RST"** = ESP32 "RST" (GPIO 19/21)

---

## Visual Wiring Diagram

### Left Eye Display (GC9A01)

```
ESP32-S3          GC9A01 Left Eye
GPIO 13 (MOSI) ──────► SDA
GPIO 14 (CLK)  ──────► SCK
GPIO 15 (CS)   ──────► CS
GPIO 18 (DC)   ──────► DC
GPIO 19 (RST)  ──────► RST
3.3V           ──────► VCC
GND            ──────► GND
```

### Right Eye Display (GC9A01)

```
ESP32-S3          GC9A01 Right Eye
GPIO 13 (MOSI) ──────► SDA  (shared with left eye)
GPIO 14 (CLK)  ──────► SCK  (shared with left eye)
GPIO 16 (CS)   ──────► CS   (unique to right eye)
GPIO 20 (DC)   ──────► DC   (unique to right eye)
GPIO 21 (RST)  ──────► RST  (unique to right eye)
3.3V           ──────► VCC
GND            ──────► GND
```

### Nose Sensor (HC-SR04)

```
ESP32-S3          HC-SR04
GPIO 9  (TRIG) ──────► TRIG
GPIO 10 (ECHO) ──────► ECHO
5V             ──────► VCC
GND            ──────► GND
```

### Left Ear Microphone (HW-496)

```
ESP32-S3          HW-496 Left
GPIO 1 (ADC)   ──────► AO (Analog Out)
3.3V           ──────► VCC
GND            ──────► GND
(DO not connected)
```

### Right Ear Microphone (HW-496)

```
ESP32-S3          HW-496 Right
GPIO 2 (ADC)   ──────► AO (Analog Out)
3.3V           ──────► VCC
GND            ──────► GND
(DO not connected)
```

---

## Power Distribution

### Power Rails Needed

```
5V Supply (HC-SR04 requires 5V):
├── HC-SR04 VCC
└── (Optional: Power ESP32 via USB)

3.3V Supply (ESP32 provides):
├── Left Eye Display VCC
├── Right Eye Display VCC
├── Left Ear Microphone VCC
└── Right Ear Microphone VCC

Ground (Common):
├── All GND pins connected together
└── Must be shared across all components
```

**⚠️ IMPORTANT**: 
- GC9A01 displays can run on 3.3V or 5V - use 3.3V from ESP32
- HW-496 microphones prefer 3.3V (cleaner signal)
- HC-SR04 requires 5V for proper operation

---

## Step-by-Step Wiring Instructions

### Step 1: Prepare Components
1. Label each display: "LEFT EYE" and "RIGHT EYE"
2. Label each microphone: "LEFT EAR" and "RIGHT EAR"
3. Cut wires to appropriate lengths (6-8 inches recommended)
4. Use color coding:
   - **Red** = Power (VCC, 3.3V, 5V)
   - **Black** = Ground (GND)
   - **Yellow** = SPI Clock (SCK/CLK)
   - **Blue** = SPI Data (SDA/MOSI)
   - **Green** = Control signals (CS, DC, RST)

### Step 2: Wire Shared SPI Bus First
**Connect to BOTH displays:**

```
ESP32 GPIO 13 → LEFT Eye SDA + RIGHT Eye SDA (parallel connection)
ESP32 GPIO 14 → LEFT Eye SCK + RIGHT Eye SCK (parallel connection)
```

Use a breadboard or solder junction to split these signals.

### Step 3: Wire Left Eye Unique Pins

```
ESP32 GPIO 15 → LEFT Eye CS
ESP32 GPIO 18 → LEFT Eye DC
ESP32 GPIO 19 → LEFT Eye RST
ESP32 3.3V    → LEFT Eye VCC
ESP32 GND     → LEFT Eye GND
```

### Step 4: Wire Right Eye Unique Pins

```
ESP32 GPIO 16 → RIGHT Eye CS
ESP32 GPIO 20 → RIGHT Eye DC
ESP32 GPIO 21 → RIGHT Eye RST
ESP32 3.3V    → RIGHT Eye VCC
ESP32 GND     → RIGHT Eye GND
```

### Step 5: Wire Nose Sensor

```
ESP32 GPIO 9  → HC-SR04 TRIG
ESP32 GPIO 10 → HC-SR04 ECHO
5V Supply     → HC-SR04 VCC
ESP32 GND     → HC-SR04 GND
```

### Step 6: Wire Ear Microphones

**Left Ear:**
```
ESP32 GPIO 1 → HW-496 AO (Analog Out)
ESP32 3.3V   → HW-496 VCC
ESP32 GND    → HW-496 GND
```

**Right Ear:**
```
ESP32 GPIO 2 → HW-496 AO (Analog Out)
ESP32 3.3V   → HW-496 VCC
ESP32 GND    → HW-496 GND
```

---

## Testing Procedure

### Test 1: Power-Up Check (No Code Yet)
1. Connect ESP32 to USB power
2. **Verify voltages**:
   - Measure 3.3V on ESP32 3.3V pin
   - Measure 5V on USB VBUS pin
3. **Check for shorts**:
   - Measure resistance between 3.3V and GND (should be >1kΩ)
   - Measure resistance between 5V and GND (should be >1kΩ)

### Test 2: Display Communication Test
Upload minimal SPI test code to:
1. Initialize SPI bus (GPIO 13, 14)
2. Send reset sequence to left eye (CS=15, DC=18, RST=19)
3. Verify display lights up
4. Repeat for right eye (CS=16, DC=20, RST=21)

### Test 3: Sensor Test
1. HC-SR04: Send 10µs pulse on TRIG (GPIO 9)
2. Measure ECHO pulse width on GPIO 10
3. Verify distance readings make sense

### Test 4: Microphone Test
1. Read ADC values from GPIO 1 (left ear)
2. Read ADC values from GPIO 2 (right ear)
3. Make noise near microphones
4. Verify ADC values change (should see ~1000-3000 range)

---

## Bill of Materials (BOM)

### Electronics

| Component | Quantity | Cost (USD) | Link/Part# |
|-----------|----------|------------|------------|
| ESP32-S3-DevKitC-1 | 1 | $15-20 | ESP32-S3-DevKitC-1 |
| GC9A01 Round Display 240x240 | 2 | $12-18 ea | GC9A01 1.28" |
| HW-496 Electret Microphone | 2 | $2-4 ea | HW-496 |
| HC-SR04 Ultrasonic Sensor | 1 | $3-5 | HC-SR04 |
| Breadboard (for testing) | 1 | $5-10 | 830-point |
| Jumper Wires (M-M, M-F) | 40+ | $5-10 | Assorted kit |
| **TOTAL** | | **$60-95** | |

### Power Supply
- USB-C cable (for ESP32) - $5-10
- 5V 2A power adapter (if not using USB) - $8-12

---

## Configuration Files to Update

Based on this wiring, you'll need to update these JSON config files:

### 1. Update SPI Device Configs

**`config/components/interfaces/spi_device_1.json`** (Left Eye):
```json
{
  "interface_id": "SPI_DEVICE_1",
  "interface_type": "SPI_DEVICE",
  "gpio_assignments": {
    "cs_pin": 15,
    "dc_pin": 18,
    "rst_pin": 19
  }
}
```

**`config/components/interfaces/spi_device_2.json`** (Right Eye):
```json
{
  "interface_id": "SPI_DEVICE_2",
  "interface_type": "SPI_DEVICE",
  "gpio_assignments": {
    "cs_pin": 16,
    "dc_pin": 20,
    "rst_pin": 21
  }
}
```

### 2. Create ADC Interface Configs

**`config/components/interfaces/adc_left_ear.json`**:
```json
{
  "interface_id": "ADC_LEFT_EAR",
  "interface_type": "ADC",
  "gpio_assignments": {
    "adc_pin": 1,
    "adc_channel": "ADC1_CH0"
  },
  "config": {
    "resolution": "12_BIT",
    "attenuation": "11dB",
    "sample_rate_hz": 8000
  }
}
```

**`config/components/interfaces/adc_right_ear.json`**:
```json
{
  "interface_id": "ADC_RIGHT_EAR",
  "interface_type": "ADC",
  "gpio_assignments": {
    "adc_pin": 2,
    "adc_channel": "ADC1_CH1"
  },
  "config": {
    "resolution": "12_BIT",
    "attenuation": "11dB",
    "sample_rate_hz": 8000
  }
}
```

### 3. Remove Mouth Display References

Delete or comment out in `config/bots/bot_families/goblins/goblin_full.json`:
- ~~"config/components/positioned/goblin_mouth.json"~~

---

## Quick Reference Card

**Print this and keep it near your workbench:**

```
═══════════════════════════════════════════════════════════
               P32 GOBLIN HEAD QUICK REFERENCE
═══════════════════════════════════════════════════════════

LEFT EYE:  CS=15, DC=18, RST=19, SDA=13, SCK=14
RIGHT EYE: CS=16, DC=20, RST=21, SDA=13, SCK=14

NOSE SENSOR: TRIG=9, ECHO=10

LEFT EAR MIC:  ADC=GPIO1
RIGHT EAR MIC: ADC=GPIO2

POWER:
  - Displays: 3.3V
  - Mics: 3.3V
  - Sensor: 5V
  - All GNDs connected together

SPI TRANSLATION:
  Your "SDA" = ESP32 "MOSI" (GPIO 13)
  Your "SCK" = ESP32 "CLK"  (GPIO 14)

═══════════════════════════════════════════════════════════
```

---

## Common Issues & Solutions

### Issue: Displays Show Garbage/Static
**Cause**: SPI timing too fast or DC pin wrong
**Fix**: 
1. Reduce SPI clock to 10 MHz (from 40 MHz)
2. Verify DC pins are correct (GPIO 18 and 20)
3. Check all ground connections

### Issue: One Display Works, Other Doesn't
**Cause**: CS pin conflict or not connected
**Fix**:
1. Verify CS pins are unique (15 and 16)
2. Check continuity with multimeter
3. Ensure shared MOSI/CLK connected to both

### Issue: Microphones Always Read Same Value
**Cause**: ADC not initialized or wrong attenuation
**Fix**:
1. Initialize ADC with 11dB attenuation
2. Verify 3.3V power to microphone
3. Check AO pin connected (not DO pin)

### Issue: Sensor Reads 0 or Maximum
**Cause**: 5V power missing or TRIG/ECHO swapped
**Fix**:
1. Verify 5V on HC-SR04 VCC pin
2. Double-check TRIG=GPIO9, ECHO=GPIO10
3. Try different sensor (common failure mode)

---

## Next Steps

1. ✅ **Print this guide** for reference during wiring
2. ✅ **Gather all components** and verify part numbers
3. ✅ **Wire shared SPI bus first** (easiest to debug)
4. ✅ **Test one display** before wiring second
5. ✅ **Add microphones last** (least critical for initial testing)

**Ready to start wiring!** 🔧⚡

---

**Document Status:** Updated for Real Hardware  
**Last Updated:** October 16, 2025  
**Hardware:** GC9A01 displays + HW-496 microphones  
**Configuration:** 2 eyes, 1 nose sensor, 2 ear microphones (NO MOUTH)
