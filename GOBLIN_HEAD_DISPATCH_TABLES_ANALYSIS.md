# Goblin Head Dispatch Tables Analysis

## Overview

The `goblin_head` subsystem dispatch tables define the execution order and timing of all components in the goblin's head subsystem. This document shows the actual generated dispatch tables and explains the component pipeline.

---

## Dispatch Table Structure

Three tables work together to control component execution:

1. **`goblin_head_init_table[]`** - Function pointers for initialization
2. **`goblin_head_act_table[]`** - Function pointers for action/update
3. **`goblin_head_hitcount_table[]`** - Execution frequency (lower = more frequent)

---

## Complete Dispatch Tables

### Initialization Table (`goblin_head_init_table`)

```cpp
const init_function_t goblin_head_init_table[] = {
    &goblin_left_eye_init,                              // Entry 0
    &goblin_eye_init,                                   // Entry 1
    &gc9a01_init,                                       // Entry 2
    &spi_display_bus_init,                              // Entry 3
    &generic_spi_display_init,                          // Entry 4
    &goblin_right_eye_init,                             // Entry 5
    &goblin_eye_init,                                   // Entry 6 (DUPLICATE)
    &gc9a01_init,                                       // Entry 7 (DUPLICATE)
    &spi_display_bus_init,                              // Entry 8 (DUPLICATE)
    &generic_spi_display_init,                          // Entry 9 (DUPLICATE)
    &goblin_nose_init,                                  // Entry 10
    &hc_sr04_ultrasonic_distance_sensor_init,           // Entry 11
    &goblin_mouth_init,                                 // Entry 12
    &ili9341_init,                                      // Entry 13
    &goblin_speaker_init,                               // Entry 14
    &speaker_init,                                      // Entry 15
    &i2s_bus_0_init,                                    // Entry 16
    &i2s_driver_init,                                   // Entry 17
    &goblin_left_ear_init,                              // Entry 18
    &servo_sg90_micro_init,                             // Entry 19
    &hw496_microphone_init,                             // Entry 20
    &goblin_right_ear_init,                             // Entry 21
    &servo_sg90_micro_init,                             // Entry 22 (DUPLICATE)
    &hw496_microphone_init                              // Entry 23 (DUPLICATE)
};
```

**Total init entries: 24**

---

### Action Table (`goblin_head_act_table`)

```cpp
const act_function_t goblin_head_act_table[] = {
    &goblin_left_eye_act,                               // Entry 0
    &goblin_eye_act,                                    // Entry 1
    &gc9a01_act,                                        // Entry 2
    &spi_display_bus_act,                               // Entry 3
    &generic_spi_display_act,                           // Entry 4
    &goblin_right_eye_act,                              // Entry 5
    &goblin_eye_act,                                    // Entry 6 (DUPLICATE)
    &gc9a01_act,                                        // Entry 7 (DUPLICATE)
    &spi_display_bus_act,                               // Entry 8 (DUPLICATE)
    &generic_spi_display_act,                           // Entry 9 (DUPLICATE)
    &goblin_nose_act,                                   // Entry 10
    &hc_sr04_ultrasonic_distance_sensor_act,            // Entry 11
    &goblin_mouth_act,                                  // Entry 12
    &ili9341_act,                                       // Entry 13
    &goblin_speaker_act,                                // Entry 14
    &speaker_act,                                       // Entry 15
    &i2s_bus_0_act,                                     // Entry 16
    &i2s_driver_act,                                    // Entry 17
    &goblin_left_ear_act,                               // Entry 18
    &servo_sg90_micro_act,                              // Entry 19
    &hw496_microphone_act,                              // Entry 20
    &goblin_right_ear_act,                              // Entry 21
    &servo_sg90_micro_act,                              // Entry 22 (DUPLICATE)
    &hw496_microphone_act                               // Entry 23 (DUPLICATE)
};
```

**Total act entries: 24**

---

### Hit Count Table (`goblin_head_hitcount_table`)

```cpp
const uint32_t goblin_head_hitcount_table[] = {
    1,      // Entry 0:  goblin_left_eye        - EVERY LOOP
    1,      // Entry 1:  goblin_eye             - EVERY LOOP (LEFT)
    1,      // Entry 2:  gc9a01                 - EVERY LOOP (LEFT)
    1,      // Entry 3:  spi_display_bus        - EVERY LOOP (LEFT)
    1,      // Entry 4:  generic_spi_display    - EVERY LOOP (LEFT)
    5,      // Entry 5:  goblin_right_eye       - EVERY 5 LOOPS
    1,      // Entry 6:  goblin_eye             - EVERY LOOP (RIGHT)
    1,      // Entry 7:  gc9a01                 - EVERY LOOP (RIGHT)
    1,      // Entry 8:  spi_display_bus        - EVERY LOOP (RIGHT)
    1,      // Entry 9:  generic_spi_display    - EVERY LOOP (RIGHT)
    180000, // Entry 10: goblin_nose            - RARE (distance sensor - ~30 seconds)
    1,      // Entry 11: hc_sr04_ultrasonic     - EVERY LOOP
    36000,  // Entry 12: goblin_mouth           - LESS FREQUENT (~6 seconds)
    1,      // Entry 13: ili9341                - EVERY LOOP
    84000,  // Entry 14: goblin_speaker         - RARE (~14 seconds)
    1,      // Entry 15: speaker                - EVERY LOOP
    1,      // Entry 16: i2s_bus_0              - EVERY LOOP
    1,      // Entry 17: i2s_driver             - EVERY LOOP
    1,      // Entry 18: goblin_left_ear        - EVERY LOOP
    1,      // Entry 19: servo_sg90_micro       - EVERY LOOP (LEFT)
    1,      // Entry 20: hw496_microphone       - EVERY LOOP (LEFT)
    1,      // Entry 21: goblin_right_ear       - EVERY LOOP
    1,      // Entry 22: servo_sg90_micro       - EVERY LOOP (RIGHT)
    1       // Entry 23: hw496_microphone       - EVERY LOOP (RIGHT)
};
```

**Total hitcount entries: 24**

---

## Component Pipeline Analysis

### Depth-First Traversal Order

The dispatch table entries reflect the depth-first tree traversal of the component JSON:

```
goblin_head
 goblin_left_eye (Entry 0)
   goblin_eye (Entry 1)
      gc9a01 (Entry 2)
         spi_display_bus (Entry 3)
            generic_spi_display (Entry 4)
 goblin_right_eye (Entry 5)
   goblin_eye (Entry 6 - DUPLICATE)
      gc9a01 (Entry 7 - DUPLICATE)
         spi_display_bus (Entry 8 - DUPLICATE)
            generic_spi_display (Entry 9 - DUPLICATE)
 goblin_nose (Entry 10)
   hc_sr04_ultrasonic_distance_sensor (Entry 11)
 goblin_mouth (Entry 12)
   ili9341 (Entry 13)
 goblin_speaker (Entry 14)
   speaker (Entry 15)
   i2s_bus_0 (Entry 16)
   i2s_driver (Entry 17)
 goblin_left_ear (Entry 18)
   servo_sg90_micro (Entry 19)
   hw496_microphone (Entry 20)
 goblin_right_ear (Entry 21)
    servo_sg90_micro (Entry 22 - DUPLICATE)
    hw496_microphone (Entry 23 - DUPLICATE)
```

---

## Key Observations

### 1. DUPLICATE COMPONENTS (Expected per RULE 4)

**Intentional Duplicates** - As per RULE 4 (Component Pipeline Execution Model), duplicates in dispatch tables are CORRECT when the same component appears in multiple pipelines:

- **goblin_eye**: Appears twice (Entry 1 for left eye, Entry 6 for right eye)
  - First encounter (Entry 1): Code aggregated to `goblin_eye_component_functions.cpp`
  - Second encounter (Entry 6): Same code reused, different data context
  - Each execution operates on different static buffers (left/right eye data)

- **gc9a01**: Appears twice (left and right display)
- **spi_display_bus**: Appears twice (left and right SPI communication)
- **generic_spi_display**: Appears twice
- **servo_sg90_micro**: Appears twice (left and right ear servos)
- **hw496_microphone**: Appears twice (left and right microphones)

**Rationale**: Same hardware driver code executes multiple times per loop with different data contexts managed by file-scoped static variables.

---

### 2. EXECUTION FREQUENCY ANALYSIS

#### High Frequency (hitCount = 1): EVERY LOOP
- Eye display components (left/right)
- Display drivers (gc9a01, spi_display_bus, generic_spi_display)
- Mouth display (ili9341)
- Speaker system (speaker, i2s drivers)
- Ear servos and microphones

**Total: 17 entries execute every loop** - Visual and audio feedback requires continuous updates

#### Medium Frequency (hitCount = 5): EVERY 5 LOOPS
- **goblin_right_eye** only (Entry 5)
- Right eye executes less frequently than left eye
- Asymmetric execution pattern suggests different animation/update needs

#### Low Frequency (Rare Updates)
- **goblin_nose** (hitCount = 180000): Executes ~every 30 seconds
  - Ultrasonic distance sensor (power conservation)
  - No need for continuous range updates
  
- **goblin_mouth** (hitCount = 36000): Executes ~every 6 seconds
  - Static display update unless animation triggered
  
- **goblin_speaker** (hitCount = 84000): Executes ~every 14 seconds
  - Audio playback managed by callback/interrupt, not continuous polling

---

### 3. COMPONENT ORGANIZATION BY FUNCTION

#### Display Pipeline (Entries 0-9)
- Left eye: goblin_left_eye  goblin_eye  gc9a01  spi_display_bus  generic_spi_display
- Right eye: goblin_right_eye  goblin_eye  gc9a01  spi_display_bus  generic_spi_display
- Status: HIGH FREQUENCY (continuous visual updates)

#### Sensor Pipeline (Entries 10-11)
- Nose: goblin_nose  hc_sr04_ultrasonic_distance_sensor
- Status: LOW FREQUENCY (only when needed)

#### Mouth/Expression Pipeline (Entries 12-13)
- Expression: goblin_mouth  ili9341 display driver
- Status: MEDIUM FREQUENCY (animation updates)

#### Audio Pipeline (Entries 14-17)
- Speaker: goblin_speaker  speaker  i2s_bus_0  i2s_driver
- Status: HIGH FREQUENCY I2S driver for audio quality

#### Ear/Listening Pipeline (Entries 18-23)
- Left ear: goblin_left_ear  servo_sg90_micro  hw496_microphone
- Right ear: goblin_right_ear  servo_sg90_micro  hw496_microphone
- Status: HIGH FREQUENCY (audio input monitoring)

---

## Code Generation Output

### Table Sizes
```cpp
const std::size_t goblin_head_init_table_size = 24;  // sizeof(array) / sizeof(type)
const std::size_t goblin_head_act_table_size = 24;
```

### Runtime Execution Loop (Conceptual)
```cpp
// Main loop in goblin_head subsystem
void goblin_head_main_loop() {
    for (int loop = 0; ; loop++) {
        // Initialize (called once)
        for (int i = 0; i < goblin_head_init_table_size; i++) {
            goblin_head_init_table[i]();  // Call init once
        }
        
        // Main loop - continuous updates
        while (running) {
            for (int i = 0; i < goblin_head_act_table_size; i++) {
                // Only call if hitCount condition met
                if (loop % goblin_head_hitcount_table[i] == 0) {
                    goblin_head_act_table[i]();
                }
            }
            loop++;
        }
    }
}
```

---

## Validation Checklist

- [x] Dispatch table entries match (24 init, 24 act, 24 hitcount)
- [x] Duplicate entries present for components in multiple pipelines
- [x] Execution order follows depth-first tree traversal
- [x] Hit counts follow guidelines (RULE 11C)
- [x] No circular dependencies
- [x] All referenced functions exist in component code

---

## Files Generated

```
src/subsystems/goblin_head/
 goblin_head_dispatch_tables.cpp        (This document shows the content)
 goblin_head_component_functions.cpp    (Aggregated .src files)
 goblin_head_main.cpp                   (FreeRTOS task setup)

include/subsystems/goblin_head/
 goblin_head_dispatch_tables.hpp        (Header declarations)
 goblin_head_component_functions.hpp    (Function prototypes)
 goblin_head_main.hpp                   (Task declarations)
```

---

## Related Files

- **Source**: `config/bots/bot_families/goblins/head/goblin_head.json`
- **Generator**: `tools/generate_tables.py`
- **Rules**: RULE 4 (Pipeline Execution Model), RULE 9 (Dispatch Table Generation)
