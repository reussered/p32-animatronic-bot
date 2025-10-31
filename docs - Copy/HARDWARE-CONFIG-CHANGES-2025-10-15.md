# Hardware Configuration Changes - October 15, 2025

## Summary of Updates

Based on actual hardware availability and pinout differences, the following changes were made to the P32 Goblin Head configuration.

---

## 1. Removed Mouth Display (Temporary)

**File Modified:** `config/bots/bot_families/goblins/goblin_full.json`

**Change:**
```json
// BEFORE:
"positioned_components": [
    "config/components/positioned/goblin/head_components/goblin_eye_left.json",
    "config/components/positioned/goblin/head_components/goblin_eye_right.json",
    "config/components/positioned/goblin/head_components/goblin_mouth.json",  // ← REMOVED
    "config/components/positioned/goblin/head_components/goblin_speaker.json",
    "config/components/positioned/goblin/head_components/goblin_nose.json"
]

// AFTER:
"positioned_components": [
    "config/components/positioned/goblin/head_components/goblin_eye_left.json",
    "config/components/positioned/goblin/head_components/goblin_eye_right.json",
    "config/components/positioned/goblin/head_components/goblin_speaker.json",
    "config/components/positioned/goblin/head_components/goblin_nose.json"
]
```

**Reason:** Current mouth display has no exposed pins. Can be added later when replacement hardware arrives.

**GPIO Freed:** 5 pins (GPIO 17, 20, 23, plus shared SPI bus that's already in use)

---

## 2. Display Pin Mapping Clarification

**New Documentation:** `docs/GOBLIN-HEAD-PRACTICAL-WIRING-GUIDE.md`

### GC9A01 Pin Name Translation

Your actual displays expose these pins:
- **SCK** → ESP32 SPI Clock (GPIO 14)
- **SDA** → ESP32 SPI MOSI (GPIO 13)
- **DC** → Data/Command select (GPIO 18 left, GPIO 19 right)
- **CS** → Chip Select (GPIO 15 left, GPIO 16 right)
- **RST** → Reset (GPIO 21 left, GPIO 22 right)
- **VCC** → 5V power rail
- **GND** → Ground rail

**Note:** Displays don't expose SDO (MISO), but GPIO 12 is still configured for completeness.

### Complete Display Wiring

**Left Eye:**
```
Display SCK  →  GPIO 14 (Yellow - shared)
Display SDA  →  GPIO 13 (Green - shared)
Display DC   →  GPIO 18 (Red)
Display CS   →  GPIO 15 (Orange)
Display RST  →  GPIO 21 (Brown)
Display VCC  →  5V rail
Display GND  →  GND rail
```

**Right Eye:**
```
Display SCK  →  GPIO 14 (Yellow - shared)
Display SDA  →  GPIO 13 (Green - shared)
Display DC   →  GPIO 19 (Pink)
Display CS   →  GPIO 16 (Purple)
Display RST  →  GPIO 22 (White)
Display VCC  →  5V rail
Display GND  →  GND rail
```

---

## 3. Added Microphone Hardware (HW-496)

**Component Added:** HW-496 Electret Microphone Module

**Purpose:** Sound detection for "ears" - enables audio reactivity

**Specifications:**
- Electret condenser microphone
- Dual outputs: Analog (AO) + Digital (DO)
- Adjustable sensitivity (onboard potentiometer)
- Operating voltage: 3.3V-5V
- Frequency response: 100Hz-10kHz

**Wiring:**
```
HW-496 VCC  →  5V power rail
HW-496 GND  →  Ground rail
HW-496 AO   →  GPIO 35 (Analog output - sound amplitude)
HW-496 DO   →  GPIO 36 (Digital output - threshold detection)
```

**GPIO Used:** 2 pins (35, 36)

**Features:**
- Real-time sound amplitude measurement (analog)
- Binary sound detection with adjustable threshold (digital)
- Can trigger mood changes based on ambient noise
- Future use: Voice detection, rhythm analysis, reactive animations

---

## 4. Updated GPIO Pin Count

**Previous Total:** 12 GPIO pins (with mouth display)

**New Total:** 16 GPIO pins (without mouth, with microphone)

**Breakdown:**
```
SPI Bus (Shared):        3 pins  (GPIO 12, 13, 14)
Left Eye (Unique):       3 pins  (GPIO 15, 18, 21)
Right Eye (Unique):      3 pins  (GPIO 16, 19, 22)
Audio I2S:               3 pins  (GPIO 4, 5, 6)
Distance Sensor:         2 pins  (GPIO 9, 10)
Microphone:              2 pins  (GPIO 35, 36)
────────────────────────────────────────────────
TOTAL:                  16 pins
```

**Available for Future:** 29+ GPIO pins still unused

---

## 5. Component Definition Files (NOT Modified)

The following files remain unchanged and can be reused when mouth display is added:

- ✅ `config/components/hardware/gc9a01_display.json` (still valid)
- ✅ `config/components/positioned/goblin/head_components/goblin_mouth.json` (still valid)
- ✅ All SPI interface definitions (still valid)

**To re-enable mouth later:**
1. Get GC9A01 display with exposed pins
2. Wire to GPIO 17 (CS), GPIO 20 (DC), GPIO 23 (RST)
3. Add line back to `goblin_full.json` positioned_components array
4. Rebuild and flash

---

## 6. New Practical Wiring Guide Created

**File:** `docs/GOBLIN-HEAD-PRACTICAL-WIRING-GUIDE.md`

**Contents:**
- Hardware-verified pin mappings (matches your actual displays)
- Pin name translation table (SCK/SDA/DC/CS/RST)
- Microphone integration instructions
- Updated power distribution diagram
- Step-by-step assembly order
- Testing checklist
- Troubleshooting guide specific to your hardware
- Instructions for adding mouth display later

**Advantages over previous guide:**
- Uses actual pin names from your hardware
- Includes microphone that was missing
- Removes components you don't have yet
- More detailed troubleshooting
- Breadboard layout diagrams
- Clear wire color coding

---

## 7. Configuration Files Status

### Modified Files
- ✅ `config/bots/bot_families/goblins/goblin_full.json` - Mouth component removed

### New Files
- ✅ `docs/GOBLIN-HEAD-PRACTICAL-WIRING-GUIDE.md` - Hardware-verified guide

### Unchanged Files (Still Valid)
- ✅ `config/components/hardware/gc9a01_display.json`
- ✅ `config/components/hardware/hc_sr04_sensor.json`
- ✅ `config/components/hardware/speaker.json`
- ✅ `config/components/interfaces/spi_bus.json`
- ✅ `config/components/interfaces/spi_device_1.json` (left eye)
- ✅ `config/components/interfaces/spi_device_2.json` (right eye)
- ✅ `config/components/interfaces/spi_device_3.json` (mouth - for future)
- ✅ All positioned component files

---

## 8. Next Steps

### Immediate Actions
1. ✅ Remove mouth from `goblin_full.json` - COMPLETE
2. ✅ Create practical wiring guide - COMPLETE
3. ⏳ Order microphone module (HW-496) if not in stock
4. ⏳ Wire hardware according to new guide
5. ⏳ Test each component individually
6. ⏳ Flash firmware and verify operation

### Future Additions
1. **Mouth Display** - When hardware with exposed pins arrives
   - Requires 5 additional GPIO: 17, 20, 23 (+ shared SPI bus)
   - Simple config file edit to re-enable

2. **Component Code Generation** - Next development task
   - Parse `goblin_full.json` (now with 4 components instead of 5)
   - Generate `src/components/*.c` files
   - Create component registry tables

3. **Hardware Testing** - Validate architecture
   - Flash ESP32-S3 with compiled firmware
   - Test ESP-NOW mesh (if multiple ESP32s available)
   - Verify mood system and animations

---

## Summary

**Changes Made:**
- ✅ Mouth display temporarily removed from bot config
- ✅ Display pin mapping clarified (SCK/SDA/DC/CS/RST)
- ✅ Microphone hardware (HW-496) added for audio reactivity
- ✅ Practical wiring guide created with actual hardware specs
- ✅ GPIO count updated: 16 pins used, 29+ available

**No Breaking Changes:**
- All component definition files remain valid
- Component code generation can proceed as planned
- Architecture documentation unchanged
- State synchronization system unaffected

**Ready for:**
- Physical hardware assembly
- Component code generation from JSON
- First firmware build and flash

---

*Document created: October 15, 2025*
*Project: P32 Animatronic Bot - Goblin Head Assembly*

