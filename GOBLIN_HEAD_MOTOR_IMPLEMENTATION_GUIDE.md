# Goblin Head Motor Control System - Complete Implementation Guide

## Overview

This guide documents the complete stepper motor system for the P32 Goblin head, including:
- **30 stepper motor configurations** (NEMA8 through NEMA42)
- **3 head subsystems** with full motor control (Eyes, Mouth, Neck)
- **Mood-integrated animation** (expressions, gaze, emotions)
- **Optimized memory architecture** (single buffers, stream rendering)

## Architecture Summary

### Motor Categories

| Category | Sizes | Use Cases | Motors |
|----------|-------|-----------|--------|
| **Micro** | NEMA8 | Finger articulation, precision | 2 |
| **Light** | NEMA11-14 | General articulation | 3 |
| **Standard** | NEMA17 | Workhorse (eyes, jaw, etc) | 13 |
| **Heavy** | NEMA23-34 | Neck, torso, heavy load | 10 |
| **Industrial** | NEMA42 | Maximum torque | 1 |
| **Specialized** | Pancake, Linear, Servo | Compact/special apps | 6 |
| **TOTAL** | - | - | **30** |

### Head Subsystems

#### 1. Eye Motor System (`goblin_head_eye_motor.hpp`)

**Articulation (3-DOF per eye):**
- Pan (L/R): NEMA17_STANDARD  1 (shared for both eyes)
- Tilt (U/D): NEMA14_STANDARD  1 (shared for both eyes)
- Blink: NEMA8_GEARED_5  1 (eyelid linear actuator)

**Display:**
- Driver: GC9A01 (240240 RGB565)
- Buffer: 115.2 KB (allocated once, reused every frame)
- Update: 30 Hz

**Key Features:**
```cpp
// Initialize with display callback
left_eye.init(240, 240, RGB565_FORMAT, display_send_callback,
              GPIO_PAN_STEP, GPIO_PAN_DIR,
              GPIO_TILT_STEP, GPIO_TILT_DIR,
              GPIO_BLINK_STEP, GPIO_BLINK_DIR);

// Every frame: render with mood, send to display
left_eye.act(current_mood, now_ms);

// Control methods:
left_eye.lookAt(pan_degrees, tilt_degrees);
left_eye.blink();
left_eye.setGazeStyle(NEUTRAL);  // or FOCUSED, WANDERING
```

**Motor Timing:**
- Pan: 50 s min interval, 250 s recommended
- Tilt: 60 s min, 250 s recommended
- Blink: 30 s min, 150 s recommended

---

#### 2. Mouth Motor System (`goblin_head_mouth_motor.hpp`)

**Articulation (5-DOF):**
- Jaw: NEMA17_HIGHTORQUE (vertical rotation)
- Corner L: NEMA14_STANDARD (left corner up/down)
- Corner R: NEMA14_STANDARD (right corner up/down)
- Cheek L: NEMA8_GEARED_5 (left puff linear)
- Cheek R: NEMA8_GEARED_5 (right puff linear)

**Display:**
- Driver: ILI9341 (480320 RGB666)
- Buffer: 115.2 KB chunk (48080 pixels  3 bytes)
- Chunks: 4 horizontal strips
- Update: 30 Hz (chunks sent sequentially)

**Expression Presets:**
- NEUTRAL, SMILE, FROWN, OPEN_LAUGH, SHOCK, ANGRY, CONFUSED, SKEPTICAL

**Phoneme Library:**
- 5 base phonemes (AH, EH, OH, OO, M/B/P) with jaw/corner shapes
- Each phoneme: 50 ms duration
- Speech sync support

**Key Features:**
```cpp
// Initialize with chunked display callback
mouth.init(480, 320, RGB666_FORMAT,
           true, 80,  // chunking enabled, 80-pixel chunks
           display_send_chunk_callback,
           GPIO_JAW_STEP, GPIO_JAW_DIR, ...);

// Every frame: update expression from mood, render chunks
mouth.act(current_mood, now_ms);

// Control methods:
mouth.setExpression(SMILE, intensity);  // 0-255 intensity
mouth.speakPhoneme(phoneme_id);         // 0-19 phonemes
```

**Motor Timing:**
- Jaw: 40 s min, 200 s recommended
- Corners L/R: 60 s min, 250 s recommended
- Cheeks L/R: 30 s min, 150 s recommended

---

#### 3. Neck Motor System (`goblin_head_neck_motor.hpp`)

**Articulation (3-DOF):**
- Pan (L/R): NEMA23_STANDARD (heavy duty)
- Tilt (U/D): NEMA17_HIGHTORQUE
- Roll: NEMA14_STANDARD (shoulder tilt)

**Preset Poses:**
- CENTER, LEFT_TURN, RIGHT_TURN, DOWN_LOOK, UP_LOOK
- SHY_TILT, CONFUSED_TILT, AGGRESSIVE

**Motion Sequences:**
- NODDING_YES: tilt pattern
- SHAKING_NO: pan pattern
- CONFUSED_WOBBLE: roll pattern

**Smooth Motion:**
- Easing: Cubic ease-in-out
- Typical duration: 300-500 ms
- Interpolation: every 30 Hz update

**Key Features:**
```cpp
// Initialize motors
neck.init(GPIO_PAN_STEP, GPIO_PAN_DIR,
          GPIO_TILT_STEP, GPIO_TILT_DIR,
          GPIO_ROLL_STEP, GPIO_ROLL_DIR);

// Every frame: update easing, move motors
neck.act(now_ms);

// Control methods:
neck.turnTo(pan_deg, tilt_deg, roll_deg, duration_ms);
neck.lookAtPose(SHY_TILT, 500);
neck.performMotion(NODDING_YES, amplitude, frequency);
```

**Motor Timing:**
- Pan: 50 s min, 200 s recommended
- Tilt: 40 s min, 200 s recommended
- Roll: 60 s min, 250 s recommended

---

## File Structure

### Headers (C++ implementations)

```
config/components/hardware/
 stepper_motor_library.hpp          Core library (30 motors)

config/components/templates/
 goblin_head_eye_motor.hpp          Eye control (3-DOF + display)
 goblin_head_mouth_motor.hpp        Mouth control (5-DOF + chunked display)
 goblin_head_neck_motor.hpp         Neck control (3-DOF smooth easing)
 goblin_eye_mood_display_v2.hpp     Eye display rendering (single buffer)
 goblin_mouth_mood_display_v2.hpp   Mouth display chunked rendering
```

### JSON Configurations

```
config/components/hardware/
 stepper_motor_catalog.json                  All 30 motor specs + guide
 stepper_nema8_standard.json                 NEMA8 variant configs
 stepper_nema14_standard.json                NEMA14 variant configs
 stepper_nema17_standard.json                NEMA17 variant configs (13 variants)
 stepper_nema23_standard.json                NEMA23 variant configs (5 variants)

config/components/creature_specific/
 goblin_head_eye_motor.json                  Eye motor configuration
 goblin_head_mouth_motor.json                Mouth motor + expressions
 goblin_head_neck_motor.json                 Neck motor + presets
```

---

## Memory Budget

### Display Buffers
- **Eye Display (GC9A01):** 2402402 = 115.2 KB
- **Mouth Display (ILI9341 chunked):** 480803 = 115.2 KB
- **Total Display:** 230.4 KB

### Motor State
- **Eye Motor State:** 48 bytes
- **Mouth Motor State:** 192 bytes (5 motors)
- **Neck Motor State:** 96 bytes
- **Total Motor State:** 336 bytes

### Callbacks & Overhead
- **Display callbacks:** 64 bytes
- **Motor callbacks:** 32 bytes
- **Misc overhead:** 104 bytes

### Grand Total
**~231 KB used (within 300 KB budget)**

---

## Integration with Component System

### Component Init Flow

```cpp
esp_err_t goblin_head_eye_motor_init(void) {
    // Get GPIO pin assignments from shared config
    // Initialize SPI display driver (GC9A01)
    // Initialize stepper controllers with GPIO pins
    // Allocate display buffer (115.2 KB)
    
    GoblinHeadEyeMotor* eye_left = new GoblinHeadEyeMotor();
    eye_left->init(240, 240, RGB565_FORMAT,
                   gc9a01_send_frame,  // Callback to display driver
                   PIN_EYE_PAN_STEP, PIN_EYE_PAN_DIR,
                   PIN_EYE_TILT_STEP, PIN_EYE_TILT_DIR,
                   PIN_EYE_BLINK_STEP, PIN_EYE_BLINK_DIR);
    
    return ESP_OK;
}
```

### Component Act Flow

```cpp
void goblin_head_eye_motor_act(void) {
    static GoblinHeadEyeMotor* eye_left = nullptr;
    
    // Get current mood from SharedMemory
    Mood current_mood = GSM.read<Mood>();
    
    // Update eye motor + display
    eye_left->act(current_mood, g_loopCount * LOOP_INTERVAL_MS);
    
    // Return buffer to GSM if needed
    // (Display callback already sent to hardware)
}
```

---

## Stepper Motor Selection Guide

### For Eye Movement
**Recommended:** `NEMA17_STANDARD` (pan) + `NEMA14_STANDARD` (tilt)
- Good balance of speed/torque
- 50-60 s min intervals allow smooth 30 Hz updates
- 400-100 ncm torque handles typical eye load

### For Mouth/Jaw
**Recommended:** `NEMA17_HIGHTORQUE` (jaw) + `NEMA14_STANDARD` (corners)
- Jaw needs 1200+ ncm for smooth opening
- Corners: lighter 100 ncm sufficient
- Geared variants (3:1, 5:1) for extra torque

### For Neck Rotation
**Recommended:** `NEMA23_STANDARD` (pan) + `NEMA17_HIGHTORQUE` (tilt)
- Pan needs 1500+ ncm for smooth head turn
- Tilt: 1200 ncm for solid up/down
- Smooth 300ms motion via easing

### For Precision Articulation
**Use:** `NEMA17_PRECISION` with SIXTEENTH_STEP
- 20 s min interval
- Smooth servo-like motion
- Useful for finger control

### For Compact Spaces
**Use:** `NEMA17_PANCAKE` (15mm depth) or `NEMA8_STANDARD`
- PANCAKE: full NEMA17 torque in 20mm length
- NEMA8: ultra-compact micro positioning

### For Heavy/Slow Motion
**Use:** `NEMA17_LSHT_100` or `NEMA23_GEARED_10`
- 100:1 reduction for ultra-smooth slow motion
- Extreme holding torque (40K+ ncm)
- ~1500 s step interval (very slow, very smooth)

---

## Performance Characteristics

### Timing Budget (30 Hz update)
- **Frame time:** 33 ms
- **Display rendering:** ~5 ms (display driver dependent)
- **Motor stepping:** ~1 ms (20-100 s  multiple motors)
- **Mood calculations:** ~2 ms
- **Easing/interpolation:** <1 ms
- **Remaining:** ~24 ms (plenty of headroom)

### Step Intervals (For Reference)

| Motor Type | Min (s) | Recommended (s) | Max RPM |
|-----------|----------|-----------------|---------|
| NEMA8 | 100 | 500 | 2000 |
| NEMA14 | 60 | 300 | 3000-3500 |
| NEMA17 | 50 | 250 | 3000-4000 |
| NEMA23 | 40 | 200 | 4500+ |

---

## Mood Integration

### Mood  Expression Mapping

```
Mood Component     Expression      Motor Effect

HIGH happiness     SMILE           corners up
HIGH sadness       FROWN           corners down
HIGH anger         ANGRY           jaw tight, corners down
HIGH excitement    OPEN_LAUGH      jaw wide, cheeks puff
HIGH curiosity     CONFUSED_TILT   head tilt
HIGH fear         SHOCK           jaw dropped, eyes wide
```

### Intensity Modulation

```cpp
float intensity = current_mood.component / 127.0f;  // 0.0 - 1.0
int16_t corner_position = (int16_t)(20 * intensity);  // Scale to range
```

---

## Example: Complete Gaze & Expression Sequence

```cpp
// Update mouth expression from mood
mouth.setExpression(SMILE, 200);  // 78% intensity

// Coordinate neck + eyes for attention
neck.lookAtPose(CURIOUS, 300);    // Tilt head, smooth 300ms
eyes.lookAt(-30, 15);             // Look left-up
eyes.blink();                      // Natural blink

// Speech animation
for (int i = 0; i < 5; ++i) {
    mouth.speakPhoneme(i);         // AH, EH, OH, OO, M
    vTaskDelay(50 / portTICK_PERIOD_MS);
}

// Return to neutral
neck.turnTo(0, 0, 0, 250);
mouth.setExpression(NEUTRAL, 100);
```

---

## GPIO Pin Assignments (Reference)

### Eye Motors (Goblin Head)
- Pan STEP: GPIO 14, DIR: GPIO 15, EN: GPIO 16
- Tilt STEP: GPIO 17, DIR: GPIO 18, EN: GPIO 19
- Blink STEP: GPIO 20, DIR: GPIO 21, EN: GPIO 22

### Mouth Motors
- Jaw STEP: GPIO 23, DIR: GPIO 24, EN: GPIO 25
- Corner L STEP: GPIO 26, DIR: GPIO 27, EN: GPIO 28
- Corner R STEP: GPIO 29, DIR: GPIO 30, EN: GPIO 31
- Cheek L STEP: GPIO 32, DIR: GPIO 33, EN: GPIO 34
- Cheek R STEP: GPIO 35, DIR: GPIO 36, EN: GPIO 37

### Neck Motors
- Pan STEP: GPIO 38, DIR: GPIO 39, EN: GPIO 40
- Tilt STEP: GPIO 41, DIR: GPIO 42, EN: GPIO 43
- Roll STEP: GPIO 44, DIR: GPIO 45, EN: GPIO 46

*Note: See `include/esp32_s3_r8n16_pin_assignments.h` for authoritative GPIO map*

---

## Next Steps

1. **Create `.src` implementations** for each motor component (`.hpp` already created)
2. **Wire stepper drivers** (DRV8825, A4988, or equivalent)
3. **Test individual motors** before full coordination
4. **Calibrate ranges** (adjust PAN_MIN/MAX, etc based on mechanical limits)
5. **Tune easing** (cubic, quad, etc based on feel preference)
6. **Speech sync** (integrate with audio subsystem)
7. **Full integration** into goblin_head.src component

---

## Reference Files

- **Motor Library:** `config/components/hardware/stepper_motor_library.hpp`
- **Eye Motor:** `config/components/templates/goblin_head_eye_motor.hpp`
- **Mouth Motor:** `config/components/templates/goblin_head_mouth_motor.hpp`
- **Neck Motor:** `config/components/templates/goblin_head_neck_motor.hpp`
- **Mood Display (Eye):** `config/components/templates/goblin_eye_mood_display_v2.hpp`
- **Mood Display (Mouth):** `config/components/templates/goblin_mouth_mood_display_v2.hpp`
- **Stepper Catalog:** `config/components/hardware/stepper_motor_catalog.json`
- **Component Configs:** `config/components/creature_specific/goblin_head_*.json`

---

**Last Updated:** November 6, 2025
**Status:** Implementation files created, ready for `.src` generation and hardware integration
