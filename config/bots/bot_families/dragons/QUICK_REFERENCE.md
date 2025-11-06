# Flying Dragon - Quick Reference Card

**Date**: November 5, 2025 | **Status**:  Organization Complete

---

##  File Locations

### Main Files
- **Bot Definition**: `config/bots/bot_families/dragons/flying_dragons/flying_dragon_full.json`
- **Integration Guide**: `config/bots/bot_families/dragons/flying_dragons/README.md`
- **Reference Index**: `config/bots/bot_families/dragons/flying_dragons/INDEX.md`

### Components
- **Motor Control**: `config/components/functional/flying_dragon_motor_controller.json`
- **Wing Servos**: `config/components/functional/flying_dragon_wing_servo_controller.json`
- **Sensor Fusion**: `config/components/functional/flying_dragon_sensor_fusion.json`
- **Flight Safety**: `config/components/functional/flying_dragon_flight_safety.json`
- **Behavior Logic**: `config/components/functional/flying_dragon_behavior_sequencer.json`
- **Subsystem**: `config/components/subsystem/flying_dragon_flight_system.json`

### Hardware Specs
```
config/bots/bot_families/dragons/flying_dragons/hardware_templates/
 T_MOTOR_U3_580KV.json        (Motor: 580 KV, 950 gf, 55g)
 HOBBYWING_40A_ESC.json       (ESC: 40A, BLHeli-32)
 MG996R_SERVO.json            (Servo: 11 kgcm, 55g)
 ICM20689_BMP390.json         (IMU: 1kHz + baro: 50Hz)
```

---

##  Quick Specs

| Spec | Value |
|------|-------|
| **Total Mass** | 2560 g |
| **Thrust-to-Weight** | 1.48:1 |
| **Hover Current** | 43 A @ 14.8 V |
| **Hover Power** | 614 W |
| **Flight Time** | 5.8 min hover, 15 min max |
| **Max Speed** | 15 m/s |
| **Service Ceiling** | 50 m |
| **Cruise Speed** | 5 m/s |

---

##  Pin Assignments

| Component | Pins | Type | Count |
|-----------|------|------|-------|
| Motors | 12-15 | PWM output | 4 |
| Servos | 16-21 | PWM output | 6 |
| IMU (I2C) | 21-22 | I2C | 2 |
| Barometer (I2C) | 21-22 | I2C | 2 |
| Battery ADC | 36 | Analog input | 1 |

---

##  Assembly Phases

**Phase 1**: Airframe + Motors + Servos (Week 1-2)  
**Phase 2**: Electrical Integration + Calibration (Week 3-4)  
**Phase 3**: Firmware + Flight Control Tuning (Week 5-7)  
**Phase 4**: Autonomous Behavior + Integration Testing (Week 8-10)  
**Phase 5**: Documentation & Optimization (Week 11-15)

---

##  Power Budget

| Component | Current | Voltage | Power |
|-----------|---------|---------|-------|
| Motors (4) | 40 A | 14.8 V | 592 W |
| ESCs (4) | 0.8 A | 14.8 V | 12 W |
| Servos (6) | 2 A | 5.0 V | 10 W |
| Sensors | 0.05 A | 3.3 V | 0.2 W |
| **Total** | **43 A** |  | **614 W** |

---

##  Motor Mixing (Quad X)

```cpp
m1 = throttle + pitch - roll + yaw    // Front-left
m2 = throttle + pitch + roll - yaw    // Front-right
m3 = throttle - pitch - roll - yaw    // Rear-left
m4 = throttle - pitch + roll + yaw    // Rear-right
```

PWM Range: 1000 s (zero) to 2000 s (max), 490 Hz ESC frequency

---

##  Safety Configuration

| Setting | Value |
|---------|-------|
| Watchdog Timeout | 500 ms |
| Min Battery Voltage | 11.0 V |
| Low Battery Alert | 11.5 V |
| Altitude Floor | 0.5 m |
| Gyro Rate Limit | 500 /s |
| Emergency Descent | 0.5 m/s |

---

##  Component States

**Flight State Machine** (8 states):
1. IDLE  Powered down
2. ARMING  Pre-flight checks
3. HOVER  Altitude hold
4. CLIMB  Ascending
5. CRUISE_FORWARD  Horizontal flight
6. AGGRESSIVE_MANEUVER  High G maneuvers
7. LANDING  Controlled descent
8. EMERGENCY_DESCENT  Failsafe descent

**Mood Influences**:
- **Calm**: 20 roll, 20 pitch, 0.5 m/s climb, 2.5 Hz wing flap
- **Playful**: 45 roll, 35 pitch, 1.0 m/s climb, 3.5 Hz wing flap
- **Aggressive**: 60 roll, 50 pitch, 1.5 m/s climb, 4.0 Hz wing flap

---

##  Component Dependencies

```
Execution Order (priority):
1. flying_dragon_sensor_fusion      (IMU @ 1 kHz + baro @ 50 Hz)
2. flying_dragon_flight_safety      (Watchdog + battery monitor)
3. flying_dragon_motor_controller   (Thrust distribution)
4. flying_dragon_wing_servo_controller (Flapping sync)
5. flying_dragon_behavior_sequencer (State machine + moods)
```

---

##  Mass Budget

| Component | Count | Unit (g) | Total (g) |
|-----------|-------|----------|-----------|
| Airframe | 1 | 500 | 500 |
| Motors | 4 | 55 | 220 |
| ESCs | 4 | 28 | 112 |
| Servos | 6 | 55 | 330 |
| Flight Controller | 1 | 100 | 100 |
| Sensors | 1 | 20 | 20 |
| Battery | 1 | 550 | 550 |
| Wiring | 1 | 120 | 120 |
| Payload | 1 | 100 | 100 |
| **TOTAL** |  |  | **2560** |

---

##  Hardware Templates

**T_MOTOR_U3_580KV**
- KV: 580 | Thrust: 950 gf | Weight: 55g | Max current: 40A

**HOBBYWING_40A_ESC**
- Continuous: 40A | Burst: 50A | Firmware: BLHeli-32 | Weight: 28g

**MG996R_SERVO**
- Torque: 11 kgcm | Speed: 0.16 sec/60 | Weight: 55g | Control: 0-180

**ICM20689_BMP390**
- IMU: 1 kHz @ 16g / 2000/s | Baro: 50 Hz @ 2m | I2C: 400 kHz

---

##  Documentation

| File | Size | Purpose |
|------|------|---------|
| README.md | 25 KB | Assembly & integration guide |
| INDEX.md | 30 KB | Component specs & reference |
| FLYING_DRAGONS_ORGANIZATION.md | 15 KB | Folder structure & rationale |

---

##  Readiness Checklist

- [x] All 12 JSON files created and organized
- [x] Hardware templates documented
- [x] Power & mass budgets calculated
- [x] Motor mixing equations provided
- [x] Assembly procedures defined
- [x] Component dependencies mapped
- [x] Pin assignments documented
- [x] Safety parameters configured
- [x] Integration guide completed
- [x] Ready for Phase 1 prototyping

---

##  Next Actions

1. **Review README.md**  Understand bot architecture
2. **Validate specs**  Cross-check with supplier datasheets
3. **Create BOM**  List all parts with part numbers
4. **Order components**  Procure all hardware
5. **Assemble airframe**  Follow Phase 1 procedures
6. **Integrate electronics**  Wire motors, ESCs, servos, sensors
7. **Calibrate sensors**  IMU & barometer offsets
8. **Test firmware**  Bench test before flight
9. **Commission hardware**  First test flight checklist
10. **Iterate tuning**  PID gains & behavior refinement

---

**Last Updated**: November 5, 2025  
**Status**:  Ready for Phase 1 Assembly  
**Estimate**: 15 weeks to first flight
