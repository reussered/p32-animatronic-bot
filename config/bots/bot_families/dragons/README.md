# Flying Dragons - Bot Family Configuration

**Date**: November 5, 2025  
**Category**: Flying drone + animatronic hybrid robots  
**Parent Directory**: `config/bots/bot_families/dragons/`

---

## Overview

This folder contains complete configurations for the **Flying Dragon** robot family, combining:
- **Articulated wing system** (6 servos for 3 DoF per wing)
- **Quadcopter propulsion** (4× brushless motors + ESCs)
- **Sensor fusion** (IMU + barometer for flight stability)
- **Animatronic head** (eyes, mouth, speaker, light effects)

---

## Folder Structure

```
flying_dragons/
├── README.md (this file)
├── flying_dragon_full.json              # Main bot definition
└── hardware_templates/                  # Reference specifications
    ├── T_MOTOR_U3_580KV.json           # Motor specification
    ├── HOBBYWING_40A_ESC.json          # Electronic speed controller
    ├── MG996R_SERVO.json               # Wing articulation servos
    └── ICM20689_BMP390.json            # Sensor suite (IMU + barometer)
```

---

## Component Files

### Bot Definition
- **`flying_dragon_full.json`**
  - Main bot assembly combining head and flight systems
  - Defines subsystem hierarchy and component composition
  - Performance specs: 1.5× thrust-to-weight, 15-min flight time, 50 m service ceiling

### Component Specifications (in `config/components/functional/`)
1. **`flying_dragon_motor_controller.json`**
   - 4× T-Motor U3 580KV brushless motors
   - 4× Hobbywing 40A ESCs with BLHeli-32 firmware
   - Total hover thrust: 3.8 kg (1.55 kg × 4 motors)
   - Max current: 160A

2. **`flying_dragon_wing_servo_controller.json`**
   - 6× MG996R digital servos (3 per wing)
   - Per-wing articulation: shoulder, twist, flap
   - Wing beat frequency: 3 Hz synchronized with flight controller

3. **`flying_dragon_sensor_fusion.json`**
   - ICM20689: 6-axis IMU @ 1 kHz, hall-effect sensors
   - BMP390: Barometer @ 50 Hz, ±2 m altitude accuracy
   - Complementary filter for attitude estimation

4. **`flying_dragon_flight_safety.json`**
   - Watchdog timeout: 500 ms
   - Battery monitoring (11 V min, 11.5 V alert)
   - Emergency descent rate: 0.5 m/s
   - Gyro rate limit: 500 °/s

5. **`flying_dragon_behavior_sequencer.json`**
   - 8-state machine: IDLE → ARMING → HOVER → CRUISE → LANDING
   - Mood-based behavior modulation
   - 100 Hz flight loop frequency

### Subsystem Definition (in `config/components/subsystem/`)
- **`flying_dragon_flight_system.json`**
  - Integrates all 5 components in execution order
  - Ensures dependency resolution: sensors → safety → motors → servos → behavior

---

## Hardware Templates Reference

Each hardware template file contains detailed specifications for procurement and integration:

### `T_MOTOR_U3_580KV.json`
- **Manufacturer**: T-Motor
- **Static thrust**: 950 gf @ nominal voltage (14.8 V)
- **Weight**: 55 g per motor
- **Sensored**: Hall-effect feedback
- **Power**: 150 W max

### `HOBBYWING_40A_ESC.json`
- **Continuous current**: 40 A (burst 50 A)
- **Firmware**: BLHeli-32 (supports DShot600)
- **Weight**: 28 g
- **Telemetry**: Voltage, current, temperature
- **Programming**: USB adapter required for tuning

### `MG996R_SERVO.json`
- **Torque**: 11 kg⋅cm @ 5 V
- **Speed**: 0.16 sec/60° (unloaded)
- **Weight**: 55 g
- **Control range**: 0–180°
- **PWM**: 50 Hz standard

### `ICM20689_BMP390.json`
- **IMU sample rate**: 1 kHz (low latency for flight control)
- **Accelerometer range**: ±16 g configurable
- **Gyroscope range**: ±2000 °/s
- **Barometer accuracy**: ±2 m altitude
- **I2C bus**: 400 kHz, fixed addresses (0x68 IMU, 0x77 baro)

---

## Power Budget

### Hover Condition (100% thrust from motors, 50% servo load)
| Component | Current (A) | Voltage (V) | Power (W) |
|-----------|------------|-----------|----------|
| Motors (4×) | 40 | 14.8 | 592 |
| ESCs (4×) | 40 | 14.8 | 7 |
| Servos (6×) | 2 | 5.0 | 10 |
| Sensors | 0.05 | 3.3 | 0.17 |
| **Total** | **43 A** | **14.8 V** | **609 W** |

### Flight Time Estimate
- **Battery**: 4S LiPo, 5000 mAh = 59.6 Wh
- **Hover duration**: 59.6 Wh ÷ 609 W ≈ **5.9 minutes** (conservative)
- **With 25% power reserve**: ~**4.5 minutes** safe hover
- **Cruise (35.5 A)**: ~**8.5 minutes** forward flight

---

## Motor Mixing (Quad X-Configuration)

The 4 motors are arranged in X pattern:
```
        Front
          M1 --- M2
          | \ / |
    Left  |  X  |  Right
          | / \ |
          M3 --- M4
         Rear
```

**Motor commands from control inputs:**
```cpp
m1 = throttle + pitch - roll + yaw  // Front-left
m2 = throttle + pitch + roll - yaw  // Front-right
m3 = throttle - pitch - roll - yaw  // Rear-left
m4 = throttle - pitch + roll + yaw  // Rear-right
```

---

## Mass Budget

| Component | Mass (g) | Count | Subtotal (g) |
|-----------|---------|-------|-------------|
| Airframe (carbon fiber) | 500 | 1 | 500 |
| Motors | 55 | 4 | 220 |
| ESCs | 28 | 4 | 112 |
| Servos | 55 | 6 | 330 |
| Flight controller (ESP32-S3) | 100 | 1 | 100 |
| Sensors (ICM20689 + BMP390) | 20 | 1 | 20 |
| Battery (4S LiPo 5000 mAh) | 550 | 1 | 550 |
| Wiring + connectors | 120 | 1 | 120 |
| Payload (head, lights, speaker) | 100 | 1 | 100 |
| **TOTAL** | — | — | **2560 g** |

**Thrust-to-weight ratio**: (950 gf × 4) ÷ 2560 g = **1.48** (target: 1.5)

---

## Deployment Requirements

### Phase 1 - Hardware Assembly
1. Mount 4 motors on motor arms with 10" props
2. Install 4 ESCs (one per motor) with JST connectors
3. Mount 6 servos (3 per wing) with linkage rods
4. Calibrate servo ranges (0–180°) per wing design
5. Verify battery connector (XT30)

### Phase 2 - Electrical Integration
1. Connect motors to ESCs in correct rotation direction
2. Wire ESCs to flight controller (PWM pins 12–15)
3. Wire servos to flight controller (PWM pins 16–21)
4. Connect IMU (I2C pins 21–22) + barometer (shared I2C bus)
5. Connect battery voltage divider to ADC (pin 36)

### Phase 3 - Firmware & Testing
1. Load flight control firmware with `flying_dragon_motor_controller` init
2. Calibrate ESCs to PWM range (1000–2000 µs)
3. Test motor spin direction (reverse ESC firmware if needed)
4. Calibrate IMU accelerometer + gyroscope offsets
5. Run motor thrust test: ramp 1000→1500 µs over 5 seconds

### Phase 4 - Flight Validation
1. Arm system (requires 3-point stick combo)
2. Hover in low-altitude GPS-denied mode (barometer only)
3. Test stabilization (pitch/roll PID tuning)
4. Validate failsafe: cut throttle → smooth descent
5. Log telemetry (voltage, current, attitude) for analysis

---

## File References

### Generated from Specifications
These JSON files were extracted from:
- **Primary source**: `FLYING_DRAGON_COMPONENT_SPEC.md` (Nov 4, 2025)
- **Deep-dive analysis**: `FLYING_DRAGON_DEEP_DIVE.md`
- **Implementation guide**: `FLYING_DRAGON_IMPLEMENTATION_GUIDE.md`

### Integration Points
- **Components**: `config/components/functional/` (referenced by this folder)
- **Subsystems**: `config/components/subsystem/` (container for all components)
- **Bot families**: `config/bots/bot_families/dragons/` (parent directory)

---

## Development Timeline

| Phase | Duration | Key Deliverables |
|-------|----------|-----------------|
| **Phase 1** | Week 1–2 | Component procurement, airframe assembly |
| **Phase 2** | Week 3–4 | Electrical integration, firmware skeleton |
| **Phase 3** | Week 5–7 | Flight control tuning, stability validation |
| **Phase 4** | Week 8–10 | Autonomous behavior, sensor fusion |
| **Phase 5** | Week 11–15 | Integration testing, documentation |

---

## Known Limitations

1. **Servo capacity**: MG996R servos can't sustain continuous high-speed flapping; recommend duty cycle ≤50%
2. **Flight time**: 15 min max with current power budget; to extend: use larger battery or more efficient motors
3. **Wind resistance**: Not recommended for sustained operation in winds >3 m/s
4. **Altitude ceiling**: 50 m absolute max; recommended cruise ceiling 30 m for safety
5. **Component dependencies**: All 5 flight components must initialize in order; missing one breaks entire system

---

## Next Steps

1. **Review** hardware templates and confirm part availability
2. **Order** components per bill of materials in implementation guide
3. **Build** airframe and assemble motor mounts per 3D design
4. **Commission** firmware on test bench before first flight
5. **Iterate** control gains based on flight test results

---

**Last Updated**: November 5, 2025  
**Status**: Configuration complete, ready for Phase 1 prototyping
