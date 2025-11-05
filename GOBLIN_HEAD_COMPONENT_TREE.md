# GOBLIN_HEAD Controller Component Tree

**Subsystem Name**: `goblin_head`  
**Controller Board**: `ESP32_S3_R8N16`  
**Subsystem Entry Point**: `config/bots/bot_families/goblins/head/goblin_head.json`

---

## Component Hierarchy Tree

```
goblin_head (CONTROLLER - ESP32_S3_R8N16)
│
├─ goblin_left_eye (POSITIONED_COMPONENT)
│  └─ goblin_eye (DISPLAY_DRIVER)
│     └─ gc9a01 (DISPLAY_DRIVER - Hardware Spec)
│        └─ spi_display_bus (BUS_INTERFACE)
│           └─ generic_spi_display (DRIVER)
│              └─ spi_bus (BUS_INTERFACE) 
│                 └─ [LEAF - No children]
│
├─ goblin_right_eye (POSITIONED_COMPONENT)
│  └─ goblin_eye (DISPLAY_DRIVER)
│     └─ gc9a01 (DISPLAY_DRIVER - Hardware Spec)
│        └─ spi_display_bus (BUS_INTERFACE)
│           └─ generic_spi_display (DRIVER)
│              └─ spi_bus (BUS_INTERFACE)
│                 └─ [LEAF - No children]
│
├─ goblin_nose (POSITIONED_COMPONENT)
│  └─ hc_sr04_sensor (HARDWARE - Ultrasonic Sensor)
│     └─ gpio_bus (BUS_INTERFACE)
│        ├─ gpio_pair_driver (DRIVER)
│        └─ gpio_single_driver (DRIVER)
│
├─ goblin_mouth (POSITIONED_COMPONENT)
│  └─ goblin_mouth (POSITIONED_COMPONENT) [CIRCULAR - points to itself]
│
├─ goblin_speaker (POSITIONED_COMPONENT)
│  └─ speaker (HARDWARE - I2S Speaker)
│     └─ [No direct components listed]
│
├─ goblin_left_ear (POSITIONED_COMPONENT)
│  ├─ servo_sg90_micro (HARDWARE - Servo Motor)
│  │  └─ pwm_bus (BUS_INTERFACE)
│  │     └─ pwm_driver (DRIVER)
│  │
│  └─ hw496_microphone (MEMS_MICROPHONE - Hardware)
│     ├─ adc_bus (BUS_INTERFACE) [DEPENDENCY]
│     └─ generic_mic_driver (DRIVER) [DEPENDENCY]
│
├─ goblin_right_ear (POSITIONED_COMPONENT)
│  ├─ servo_sg90_micro (HARDWARE - Servo Motor)
│  │  └─ pwm_bus (BUS_INTERFACE)
│  │     └─ pwm_driver (DRIVER)
│  │
│  └─ hw496_microphone (MEMS_MICROPHONE - Hardware)
│     ├─ adc_bus (BUS_INTERFACE) [DEPENDENCY]
│     └─ generic_mic_driver (DRIVER) [DEPENDENCY]
│
└─ [REFERENCE SUBSYSTEMS - NOT LOCAL]
   ├─ config/subsystems/goblin_head.json (SUBSYSTEM COPY)
   └─ config/subsystems/goblin_torso.json (EXTERNAL SUBSYSTEM)
```

---

## Component Details by Layer

### LAYER 0: CONTROLLER (Subsystem Root)
| Name | File | Type | Role |
|------|------|------|------|
| `goblin_head` | config/bots/bot_families/goblins/head/goblin_head.json | CONTROLLER | Subsystem entry point; defines ESP32-S3-R8N16 board |

### LAYER 1: POSITIONED COMPONENTS (Creature-Specific Placements)
| Name | File | Timing (hitCount) | Hardware Containers | Function |
|------|------|-------------------|-------------------|----------|
| `goblin_left_eye` | config/components/creature_specific/goblin_left_eye.json | 200 | goblin_eye | Left eye display animation |
| `goblin_right_eye` | config/components/creature_specific/goblin_right_eye.json | 200 | goblin_eye | Right eye display with bird animation |
| `goblin_nose` | config/components/creature_specific/goblin_nose.json | 180000 | hc_sr04_sensor | Proximity sensor monitoring |
| `goblin_mouth` | config/components/creature_specific/goblin_mouth.json | 36000 | goblin_mouth | Mouth display animation (CIRCULAR?) |
| `goblin_speaker` | config/components/creature_specific/goblin_speaker.json | 84000 | speaker | Audio output processing |
| `goblin_left_ear` | config/components/creature_specific/goblin_left_ear.json | 10 | servo_sg90_micro, hw496_microphone | Ear movement + sound input |
| `goblin_right_ear` | config/components/creature_specific/goblin_right_ear.json | 10 | servo_sg90_micro, hw496_microphone | Ear movement + sound input |

### LAYER 2: DISPLAY LOGIC (Shared Processing)
| Name | File | Type | Purpose |
|------|------|------|---------|
| `goblin_eye` | config/components/creature_specific/goblin_eye.json | DISPLAY_DRIVER | Shared mood-based eye color processing (1.5x intensity) |

### LAYER 3: HARDWARE SPECIFICATIONS
| Name | File | Type | Physical Device | Key Specs |
|------|------|------|-----------------|-----------|
| `gc9a01` | config/components/hardware/gc9a01.json | DISPLAY_DRIVER | 1.28" round LCD | 240x240, RGB565 (16-bit color) |
| `hc_sr04_sensor` | config/components/hardware/hc_sr04_sensor.json | SENSOR | Ultrasonic distance | Range: 2cm-157cm, 15mA @ 5V |
| `speaker` | config/components/hardware/speaker.json | I2S_SPEAKER | 40mm audio driver | 3W, 100Hz-20kHz, 3.3V |
| `servo_sg90_micro` | config/components/hardware/servo_sg90_micro.json | SERVO_MOTOR | Micro servo | 180°, 1.8kg-cm torque, 50Hz PWM |
| `hw496_microphone` | config/components/hardware/hw496_microphone.json | MEMS_MICROPHONE | Audio input module | 20Hz-20kHz, adjustable gain (25-125x) |

### LAYER 4: BUS INTERFACES (Communication Layers)
| Name | File | Type | Purpose | Contained Drivers |
|------|------|------|---------|------------------|
| `spi_display_bus` | config/components/drivers/spi_display_bus.json | BUS_INTERFACE | Write-only SPI for display controllers | generic_spi_display |
| `spi_bus` | config/components/interfaces/spi_bus.json | BUS_INTERFACE | General SPI communication | (no children listed) |
| `gpio_bus` | config/components/drivers/gpio_bus.json | BUS_INTERFACE | GPIO digital I/O | gpio_pair_driver, gpio_single_driver |
| `pwm_bus` | config/components/drivers/pwm_bus.json | BUS_INTERFACE | PWM signal generation | pwm_driver |
| `adc_bus` | (DEPENDENCY) | BUS_INTERFACE | Analog-to-digital conversion | (not fully traced) |

### LAYER 5: DRIVERS (Hardware Abstraction)
| Name | File | Type | Purpose |
|------|------|------|---------|
| `generic_spi_display` | config/components/drivers/generic_spi_display.json | DRIVER | Generic SPI display control (GC9A01, ILI9341, etc.) |
| `gpio_pair_driver` | (Not fully read) | DRIVER | Pair of GPIO pins (trigger + echo for HC-SR04) |
| `gpio_single_driver` | (Not fully read) | DRIVER | Single GPIO pin operations |
| `pwm_driver` | (Not fully read) | DRIVER | PWM signal generation for servo |
| `generic_mic_driver` | (DEPENDENCY - not traced) | DRIVER | Generic microphone driver for HW-496 |

---

## Key Observations

### 1. **Dual Eye Architecture**
- Both `goblin_left_eye` and `goblin_right_eye` contain `goblin_eye`
- `goblin_eye` processes mood-based color rendering with **1.5x emotion intensity** (goblins show emotions strongly)
- Both eyes share the same GC9A01 display hardware spec
- Each eye has separate SPI bus allocation (CS pins differ: CS=3 for left, CS=6 for right)

### 2. **Timing Hierarchy**
| Component | hitCount | Execution Rate | Purpose |
|-----------|----------|-----------------|---------|
| goblin_left_eye, goblin_right_eye | 200 | Every 200 loops | Fast eye animation refresh |
| goblin_eye | 1 | Every loop | Real-time mood color processing |
| gc9a01 | 1 | Every loop | Display hardware updates |
| goblin_mouth | 36000 | Every 36k loops | Slow mouth updates (rarely changes) |
| goblin_nose | 180000 | Every 180k loops | Infrequent proximity checks |
| goblin_speaker | 84000 | Every 84k loops | Audio output (only when needed) |
| goblin_left_ear, goblin_right_ear | 10 | Every 10 loops | Regular ear/microphone processing |

### 3. **Hardware Interface Requirements**
- **SPI Displays** (eyes): 2 displays on single SPI bus with separate CS pins (CS=3, CS=6)
- **GPIO Pair** (nose): Trigger + Echo pins for HC-SR04 ultrasonic sensor
- **PWM** (ears): 50Hz control signal for SG90 micro servos
- **ADC** (ears): Analog audio input from HW-496 microphones

### 4. **ISSUE: Circular Component Reference**
`goblin_mouth.json` contains in its components array:
```json
"components": [
    "config/components/creature_specific/goblin_mouth.json"
]
```
This is a **self-reference**. Should likely reference hardware (GC9A01) instead.

### 5. **Shared Memory Integration**
- `goblin_eye` reads global `Mood` state via `GSM.read<Mood>()`
- Colors applied via mood-based palette system:
  - ANGER: Red tint (1.5x intensity)
  - CURIOSITY: Green tint
  - FEAR: Blue tint
  - HAPPINESS: Yellow tint
  - And 5 more emotion states

### 6. **Two-Tier Mounting Architecture**
All positioned components use two-tier mounts:
1. **Universal Hardware Mount** (e.g., `display_basic_mount`, `sensor_basic_mount`)
2. **Creature-Specific Decorative Shell** (e.g., `goblin_left_eye_shell`, `goblin_nose_shell`)

---

## File Organization Pattern

```
config/
├─ bots/bot_families/goblins/head/
│  ├─ goblin_head.json (SUBSYSTEM ROOT)
│  ├─ goblin_left_eye.json
│  ├─ goblin_right_eye.json
│  ├─ goblin_nose.json
│  ├─ goblin_mouth.json
│  ├─ goblin_speaker.json
│  ├─ goblin_left_ear.json
│  └─ goblin_right_ear.json
│
├─ components/
│  ├─ creature_specific/
│  │  ├─ goblin_eye.json (+ .src, .hdr)
│  │  ├─ goblin_left_eye.json (+ .src, .hdr)
│  │  ├─ goblin_right_eye.json
│  │  ├─ goblin_nose.json
│  │  ├─ goblin_mouth.json
│  │  ├─ goblin_speaker.json
│  │  ├─ goblin_left_ear.json
│  │  └─ goblin_right_ear.json
│  │
│  ├─ hardware/
│  │  ├─ gc9a01.json
│  │  ├─ hc_sr04_sensor.json
│  │  ├─ speaker.json
│  │  ├─ servo_sg90_micro.json
│  │  └─ hw496_microphone.json
│  │
│  ├─ drivers/
│  │  ├─ spi_display_bus.json
│  │  ├─ generic_spi_display.json
│  │  ├─ gpio_bus.json
│  │  ├─ pwm_bus.json
│  │  ├─ pwm_driver.json
│  │  └─ [gpio_pair_driver, gpio_single_driver, etc.]
│  │
│  └─ interfaces/
│     └─ spi_bus.json
│
└─ subsystems/
   ├─ goblin_head.json (SUBSYSTEM COPY)
   └─ goblin_torso.json
```

---

## Code Fragment Files (.src/.hdr)

### Source Code Fragments
| Component | File | Purpose |
|-----------|------|---------|
| `goblin_eye` | goblin_eye.src | Mood-based color palette rendering |
| `goblin_eye` | goblin_eye.hdr | Public API declarations |
| `goblin_left_eye` | goblin_left_eye.src | Left eye buffer allocation (template-based) |
| `goblin_left_eye` | goblin_left_eye.hdr | Public API: `goblin_left_eye_get_buffer()` |

---

## Next Steps for Implementation

1. **Fix `goblin_mouth` circular reference** - should contain `gc9a01.json` instead
2. **Trace GPIO drivers** - read `gpio_pair_driver.json`, `gpio_single_driver.json`
3. **Trace ADC bus** - read `adc_bus.json` and `generic_mic_driver.json`
4. **Trace PWM driver** - read `pwm_driver.json`
5. **Check SPI interface** - read `spi_bus.json` and confirm pin allocation
6. **Verify shared memory integration** - all creature-specific components should use `GSM.read<Mood>()`

---

*Generated: 2025-11-05*
