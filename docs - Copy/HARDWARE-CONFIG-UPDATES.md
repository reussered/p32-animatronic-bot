# Hardware Configuration Updates - October 15, 2025

## Changes Made This Morning

### 1. Removed Mouth Display from Goblin Full Configuration ✅

**File**: `config/bots/bot_families/goblins/goblin_full.json`

**Change**: Removed mouth display component reference (keeping component definition for future use)

**Reason**: Physical hardware has no exposed pins on mouth display module

**Before**:
```json
"positioned_components": [
  {"component": "config/components/positioned/goblineye_left.json"},
  {"component": "config/components/positioned/goblineye_right.json"},
  {"component": "config/components/positioned/goblin_mouth.json"},  // ← REMOVED
  {"component": "config/components/positioned/goblin_nose.json"},
  ...
]
```

**After**:
```json
"positioned_components": [
  {"component": "config/components/positioned/goblineye_left.json"},
  {"component": "config/components/positioned/goblineye_right.json"},
  {"component": "config/components/positioned/goblin_nose.json"},
  ...
]
```

**Status**: ✅ Component definition preserved in `config/components/positioned/goblin_mouth.json` for future use

---

### 2. Added Pin Aliasing to GC9A01 Display Component ✅

**File**: `config/components/hardware/gc9a01_display.json`

**Change**: Added comprehensive `pin_mapping` section with alternate pin names

**Reason**: Different GC9A01 manufacturers use different pin labels for the same electrical signals

**Your Display Pins**: `SCK`, `SDA`, `DC`, `CS`, `RST`, `VCC`, `GND`

**Pin Mapping Added**:

| Your Pin | Standard Signal | ESP32 Connection | Function |
|----------|----------------|------------------|----------|
| **SCK** | `SPI_CLK` | GPIO 14 | SPI Clock |
| **SDA** | `SPI_MOSI` | GPIO 13 | SPI Data (Master Out) |
| **DC** | `DATA_COMMAND` | GPIO 27/33 | Data/Command Select |
| **CS** | `SPI_CS` | GPIO 15/32 | Chip Select |
| **RST** | `RESET` | GPIO 26 | Hardware Reset (shared) |
| **VCC** | `POWER` | 3.3V | Power Supply |
| **GND** | `GROUND` | GND | Ground Reference |

**Benefits**:
- ✅ Automatic translation of user's physical pin names to ESP32 signals
- ✅ Self-documenting component definitions
- ✅ Support for all GC9A01 display variants
- ✅ Code generation can handle any manufacturer's pinout

---

### 3. Created Pin Aliasing Documentation ✅

**File**: `docs/COMPONENT-PIN-ALIASING-SPEC.md`

**Contents**:
- Complete explanation of pin aliasing system
- Pin mapping tables for GC9A01 displays
- Common manufacturer variations
- Code generation guidance
- Future enhancement ideas (automatic detection, interactive mapper)

**Key Sections**:
- Standard SPI Signal Mapping
- Your Display Pinout (SCK/SDA variant)
- Usage in Code Generation
- Common Manufacturer Variations

---

## Remaining Tasks (For Next Session)

### 1. Add HW-496 Microphone Component (Ears) ⏳

**Requirement**: Microphone modules for goblin ears (not currently in spec)

**Files to Create**:
- `config/components/hardware/hw496_microphone.json`
- `config/components/positioned/goblin_ear_left_mic.json`
- `config/components/positioned/goblin_ear_right_mic.json`

**Specs Needed**:
- Pin mapping (VCC, GND, AO - analog out, DO - digital out)
- ESP32 ADC pin assignments
- Audio processing requirements
- Component positioning in 3D space

### 2. Update Wiring Diagram ⏳

**File**: `docs/goblin-full-interconnection-diagram.md`

**Changes Needed**:
- Remove mouth display wiring
- Add HW-496 microphone wiring for both ears
- Update pin count (reduce by ~5 pins from mouth removal)
- Verify all pin mappings match new aliasing system

### 3. Validate Complete Hardware Configuration ⏳

**Tasks**:
- Verify no pin conflicts
- Confirm total pin count < available ESP32-S3 GPIO
- Test component code generation with new pin mappings
- Create physical wiring checklist

---

## Current Hardware Configuration Summary

### Active Components (Goblin Full)

| Component | Type | Pins Used | GPIO Assignments |
|-----------|------|-----------|------------------|
| **Left Eye** | GC9A01 Display | 7 pins | CLK:14, MOSI:13, DC:27, CS:15, RST:26, VCC, GND |
| **Right Eye** | GC9A01 Display | 5 pins* | DC:33, CS:32 (shares CLK/MOSI/RST) |
| **Nose Sensor** | HC-SR04 | 4 pins | TRIG:?, ECHO:?, VCC, GND |
| **Speaker** | I2S Audio | TBD | I2S pins |
| **System** | WiFi/Mesh | 0 pins | Internal (no GPIO) |

*Right eye shares SPI bus (CLK, MOSI) and reset pin with left eye

### Deferred Components

| Component | Status | Reason |
|-----------|--------|--------|
| **Mouth Display** | ⏳ Deferred | No exposed pins on physical hardware |
| **Ear Microphones** | ⏳ To be added | HW-496 modules not yet in spec |

### Pin Budget

**ESP32-S3 DevKitC-1**: 45 total GPIO pins available

**Current Usage**:
- Left Eye: 5 unique pins (CLK, MOSI, DC, CS, RST)
- Right Eye: 2 unique pins (DC, CS)
- Nose Sensor: 2 pins (TRIG, ECHO)
- Speaker: 3 pins (I2S - TBD)
- System: 0 pins
- **Total**: ~12 pins

**Available**: ~33 pins remaining for microphones, servos, LEDs, etc.

---

## Files Modified This Session

1. ✅ `config/bots/bot_families/goblins/goblin_full.json` - Removed mouth component
2. ✅ `config/components/hardware/gc9a01_display.json` - Added pin aliasing
3. ✅ `docs/COMPONENT-PIN-ALIASING-SPEC.md` - NEW documentation file
4. ✅ `docs/HARDWARE-CONFIG-UPDATES.md` - This summary (NEW)

---

## Next Steps

**Immediate** (Say this to continue):
> "Add the HW-496 microphone components for the goblin ears"

**Then**:
1. Create hardware component JSON for HW-496
2. Create positioned components for left/right ear microphones
3. Add microphones to `goblin_full.json`
4. Update wiring diagram with new configuration
5. Verify pin budget and assignments

---

## Quick Reference

**Your Display Wiring** (SCK/SDA variant):
```
Left Eye GC9A01:
  SCK → GPIO 14 (SPI_CLK - shared)
  SDA → GPIO 13 (SPI_MOSI - shared)
  DC  → GPIO 27 (Data/Command)
  CS  → GPIO 15 (Chip Select)
  RST → GPIO 26 (Reset - shared)
  VCC → 3.3V
  GND → GND

Right Eye GC9A01:
  SCK → GPIO 14 (SPI_CLK - shared)
  SDA → GPIO 13 (SPI_MOSI - shared)
  DC  → GPIO 33 (Data/Command)
  CS  → GPIO 32 (Chip Select)
  RST → GPIO 26 (Reset - shared)
  VCC → 3.3V
  GND → GND
```

**Configuration Status**: ✅ Ready for component code generation (pending microphones)

---

*Updated: October 15, 2025, 9:30 AM*  
*Project: P32 Animatronic Bot - Goblin Head Hardware Configuration*
