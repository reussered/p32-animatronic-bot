# Dragons Bothome Structure

**Effective Date**: November 5, 2025  
**Status**: ✅ FLATTENED & REORGANIZED  
**Location**: `config/bots/bot_families/dragons/` (bothome)

---

## Overview

The **flying_dragons/** subfolder has been flattened into **bothome** with anatomy-aligned part-based organization. This structure enables:

- **Direct access** to bot definitions and documentation at the root level
- **Part-based organization** aligned with flying dragon anatomy (head, wings, body, motors, sensors, power, control)
- **Hardware reference templates** accessible from a central location
- **Scalability** for future dragon family variants

---

## Directory Structure

```
config/bots/bot_families/dragons/
├── flying_dragon_full.json           ← Master bot definition (START HERE)
├── README.md                          ← Comprehensive integration guide
├── INDEX.md                           ← Complete JSON reference & assembly checklist
├── QUICK_REFERENCE.md                 ← Quick lookup card with specs & pin assignments
├── BOTHOME_STRUCTURE.md               ← This file
│
├── head/                              ← Head sensory components (eyes, mouth, speaker, nostrils)
│   └── [eye, mouth, speaker configs]
│
├── wings/                             ← Wing articulation & servo control
│   └── [wing servo controller, articulation configs]
│
├── body/                              ← Main body & structural components
│   └── [body-related configurations]
│
├── motors/                            ← Motor control & ESC specifications
│   ├── flying_dragon_motor_controller.json
│   └── [motor mixing, thrust allocation]
│
├── sensors/                           ← IMU, barometer, sensor fusion
│   ├── flying_dragon_sensor_fusion.json
│   └── [sensor calibration, fusion logic]
│
├── power/                             ← Battery, power distribution, monitoring
│   ├── flying_dragon_flight_safety.json  ← Battery monitor & watchdog
│   └── [power budgets, safety thresholds]
│
├── control/                           ← Flight logic, state machine, behavior
│   ├── flying_dragon_behavior_sequencer.json
│   └── [flight states, control loops]
│
├── hardware_templates/                ← Detailed hardware specifications
│   ├── T_MOTOR_U3_580KV.json          ← 4× main motors (950 gf, 55g)
│   ├── HOBBYWING_40A_ESC.json         ← 4× electronic speed controllers
│   ├── MG996R_SERVO.json              ← 6× wing articulation servos
│   └── ICM20689_BMP390.json           ← IMU + barometer sensor
│
└── [Other dragon variants: dragon_ancient_fire_001.json, etc.]

```

---

## File Organization Key

### **Root Level** (Master Definitions)

| File | Purpose | Size | Status |
|------|---------|------|--------|
| `flying_dragon_full.json` | Master bot assembly (subsystems: head, flight_controller) | 2.1 KB | ✅ Master |
| `README.md` | Integration guide, assembly (4 phases), budgets | 25 KB | ✅ Reference |
| `INDEX.md` | JSON reference, component specs, assembly checklist | 30 KB | ✅ Reference |
| `QUICK_REFERENCE.md` | Quick lookup: specs, pins, file locations | 10 KB | ✅ Reference |
| `BOTHOME_STRUCTURE.md` | This organizational document | — | ✅ Current |

### **Anatomy-Based Subfolders**

#### **head/** — Sensory/Display Components
- **Purpose**: Head-mounted sensors and displays
- **Components** (from flying_dragon_full.json subsystem):
  - `flying_dragon_left_eye<GC9A01>` — 240×240 IPS color display
  - `flying_dragon_right_eye<GC9A01>` — 240×240 IPS color display
  - `flying_dragon_mouth<GC9A01>` — 240×135 display
  - `flying_dragon_speaker<MINISPKR>` — Audio output
  - `flying_dragon_nostril_left<RGB_LED>` — Status indicator
  - `flying_dragon_nostril_right<RGB_LED>` — Status indicator

#### **wings/** — Articulation & Wing Control
- **Purpose**: Wing flapping mechanism and servo control
- **Components**:
  - `flying_dragon_wing_servo_controller.json` — 6× servo management (3 per wing, 3 DoF each)
  - Wing synchronization configs (flap frequency, phase locking)
  - Servo calibration parameters

#### **body/** — Structural Components
- **Purpose**: Main airframe and body structure
- **Components**:
  - Body segment definitions
  - Center of gravity calculations
  - Mounting point specifications

#### **motors/** — Propulsion System
- **Purpose**: Motor control and thrust allocation
- **File**: `flying_dragon_motor_controller.json`
- **Contains**:
  - 4× T-Motor U3 580KV brushless motor definitions
  - 4× Hobbywing 40A ESC assignments
  - Motor mixing matrix (quadcopter mixing for 4 motors)
  - Thrust allocation at different throttle levels

**Key Specs**:
- Motor thrust: 950 gf per motor @ 100% throttle
- Total hover thrust: ~3800 gf (1.5× 2560g mass budget)
- ESC capabilities: 40A continuous (60A burst)
- Power: 22.2V (6S LiPo) nominal

#### **sensors/** — Sensor Fusion & Perception
- **Purpose**: Attitude estimation and environmental awareness
- **File**: `flying_dragon_sensor_fusion.json`
- **Contains**:
  - ICM20689 (6-axis IMU: 3-axis accel + 3-axis gyro)
  - BMP390 (barometric sensor for altitude)
  - Complementary filter for attitude fusion (1 kHz rate)

**Key Specs**:
- IMU rate: 1000 Hz (1 ms updates)
- Barometer rate: 50 Hz (~20 ms updates)
- Output: Attitude (roll/pitch/yaw), altitude, vertical velocity

#### **power/** — Battery & Safety Monitoring
- **Purpose**: Power distribution and safety management
- **File**: `flying_dragon_flight_safety.json`
- **Contains**:
  - Battery voltage monitoring (6S LiPo: 25.2V max → 11V minimum)
  - Watchdog timer (500 ms trigger for emergency descent)
  - Power sequencing
  - Low-battery cutoff (11V trigger)

**Key Specs**:
- Nominal: 22.2V (6S LiPo fully charged)
- Safe minimum: 11V (emergency descent triggered)
- Watchdog: 500 ms heartbeat period

#### **control/** — Flight Logic & Behavior
- **Purpose**: State machine and flight control algorithms
- **File**: `flying_dragon_behavior_sequencer.json`
- **Contains**:
  - 8-state flight machine: IDLE, ARMING, HOVER, CLIMB, CRUISE, MANEUVER, LANDING, EMERGENCY_DESCENT
  - PID controllers (cascaded: position/velocity/acceleration loops)
  - Control loop rates: 100 Hz flight loop, 3 Hz wing flap sync
  - Mood modulation of flight behavior

**Key Specs**:
- Hover throttle: ~50% (balanced for 1.5:1 thrust-to-weight)
- Max climb: 2 m/s (conservative for stability)
- Cruise: 5 m/s (efficient flight speed)
- Max speed: 15 m/s (emergency/stunt maneuver)
- Ceiling: 50 m (safe altitude above obstacles)
- Flight time: 5.8 min hover, 15 min mixed profile

### **hardware_templates/** — Procurement Reference

All files are **read-only reference documents** containing detailed hardware specifications:

| File | Device | Specs | Units |
|------|--------|-------|-------|
| `T_MOTOR_U3_580KV.json` | Brushless motor | 580 KV, 950 gf hover thrust, 55g | 4× qty |
| `HOBBYWING_40A_ESC.json` | Speed controller | 40A continuous, BLHeli-32, 32-bit ARM | 4× qty |
| `MG996R_SERVO.json` | Servo actuator | 11 kg⋅cm torque, 50 Hz PWM, 55g | 6× qty |
| `ICM20689_BMP390.json` | IMU + barometer | 1 kHz accel/gyro, 50 Hz pressure | 1× qty |

---

## Data Flow Map

```
┌─────────────────────────────────────────────────────────────┐
│  flying_dragon_full.json (Master Assembly)                  │
│  ├─ Subsystem: flying_dragon_head                           │
│  │  ├─ [Components from head/]                              │
│  │  └─ → Defines head sensory/display pipeline             │
│  └─ Subsystem: flying_dragon_flight_controller              │
│     ├─ [Components from motors/, sensors/, power/, control/]│
│     └─ → Defines flight dynamics & safety                  │
└─────────────────────────────────────────────────────────────┘
                            ↓
            ┌───────────────────────────────────┐
            │   config/components/subsystem/    │
            │  flying_dragon_flight_system.json │
            │ (Integrates all 5 flight parts)   │
            └───────────────────────────────────┘
                            ↓
            ┌───────────────────────────────────┐
            │   config/hardware/                │
            │  flying_dragon_motor_controller   │
            │  flying_dragon_wing_servo_ctrl    │
            │  flying_dragon_sensor_fusion      │
            │  flying_dragon_flight_safety      │
            │  flying_dragon_behavior_seq       │
            └───────────────────────────────────┘
```

---

## Bot Definition Breakdown

### **flying_dragon_full.json**

**Master composition document** that defines the complete flying dragon assembly with all subsystems:

```json
{
  "bot_name": "flying_dragon_full",
  "description": "Complete flying dragon with sensory head and flapping wings",
  "version": "1.0.0",
  "subsystems": [
    {
      "name": "flying_dragon_head",
      "components": [
        "flying_dragon_left_eye<GC9A01>",
        "flying_dragon_right_eye<GC9A01>",
        "flying_dragon_mouth<GC9A01>",
        "flying_dragon_speaker<MINISPKR>",
        "flying_dragon_nostril_left<RGB_LED>",
        "flying_dragon_nostril_right<RGB_LED>"
      ]
    },
    {
      "name": "flying_dragon_flight_controller",
      "components": [
        "flying_dragon_sensor_fusion<ICM20689_BMP390>",
        "flying_dragon_flight_safety<WATCHDOG_BATTERY_MONITOR>",
        "flying_dragon_motor_controller<T_MOTOR_U3_580KV>",
        "flying_dragon_wing_servo_controller<MG996R_SERVO>",
        "flying_dragon_behavior_sequencer<ESP32_S3_FLIGHT_LOGIC>"
      ]
    }
  ],
  "mass_budget": {
    "airframe": "500g",
    "motors": "220g (55g × 4)",
    "escs": "140g (35g × 4)",
    "servos": "330g (55g × 6)",
    "battery": "700g (6S 3700mAh LiPo)",
    "electronics": "200g (controllers, sensors, wiring)",
    "reserves": "70g",
    "total": "2560g"
  },
  "performance": {
    "max_flight_time": "15 min (5% battery reserve)",
    "hover_flight_time": "5.8 min",
    "max_speed": "15 m/s",
    "cruise_speed": "5 m/s",
    "max_climb": "2 m/s",
    "ceiling": "50 m",
    "thrust_to_weight": "1.48:1"
  }
}
```

---

## Hardware Component Specifications

### **motors/** — flying_dragon_motor_controller.json

Defines all 4 propulsion motors and electronic speed controllers:

```json
{
  "component_name": "flying_dragon_motor_controller",
  "description": "Quad-motor thrust system with ESC management",
  "motors": [
    {
      "id": "motor_1_front_left",
      "hardware": "T_MOTOR_U3_580KV",
      "thrust_at_hover": "950 gf",
      "max_thrust": "1000 gf @ 100%",
      "mass": "55g",
      "esc": "HOBBYWING_40A_ESC"
    },
    // ... motor 2, 3, 4 (same specs)
  ],
  "motor_mixing": {
    "type": "quadcopter_x_frame",
    "total_hover_thrust": "3800 gf (1.5× mass budget)",
    "throttle_at_hover": "50%"
  },
  "thrust_allocation": {
    "0_percent": "0 gf total",
    "50_percent": "2000 gf total (hover)",
    "75_percent": "3000 gf total (climb)",
    "100_percent": "4000 gf total (max)",
    "emergency_descent": "1000 gf total (0.5 m/s down)"
  }
}
```

---

### **sensors/** — flying_dragon_sensor_fusion.json

Defines attitude & altitude estimation:

```json
{
  "component_name": "flying_dragon_sensor_fusion",
  "description": "IMU + barometer fusion for 9-DOF attitude + altitude",
  "imu": {
    "hardware": "ICM20689",
    "rate": "1000 Hz",
    "accel_range": "±16g",
    "gyro_range": "±2000°/s",
    "output": ["roll", "pitch", "yaw", "accel_x", "accel_y", "accel_z"]
  },
  "barometer": {
    "hardware": "BMP390",
    "rate": "50 Hz",
    "output": ["altitude", "vertical_velocity", "pressure"]
  },
  "fusion_algorithm": {
    "type": "complementary_filter",
    "loop_rate": "100 Hz",
    "alpha": "0.95",
    "output": ["roll", "pitch", "yaw", "altitude", "climb_rate"]
  }
}
```

---

### **power/** — flying_dragon_flight_safety.json

Defines safety & power monitoring:

```json
{
  "component_name": "flying_dragon_flight_safety",
  "description": "Watchdog + battery monitoring for safe shutdown",
  "battery_monitor": {
    "cell_count": "6S LiPo",
    "max_voltage": "25.2V (6 × 4.2V)",
    "nominal_voltage": "22.2V (6 × 3.7V)",
    "min_safe_voltage": "11V (emergency descent trigger)",
    "sample_rate": "100 Hz"
  },
  "watchdog_timer": {
    "period": "500 ms",
    "action_on_timeout": "emergency_descent"
  },
  "safety_states": {
    "normal": "voltage > 15V, heartbeat active",
    "warning": "voltage 12-15V, reduce climb rate",
    "emergency": "voltage < 11V, descend at 0.5 m/s",
    "failsafe": "watchdog timeout, initiate auto-land"
  }
}
```

---

### **control/** — flying_dragon_behavior_sequencer.json

Defines flight state machine & control loops:

```json
{
  "component_name": "flying_dragon_behavior_sequencer",
  "description": "8-state flight machine with cascaded PID control",
  "flight_states": [
    { "state": "IDLE", "action": "zero_throttle, check_systems" },
    { "state": "ARMING", "action": "run_startup_test, spin_motors_low" },
    { "state": "HOVER", "action": "maintain_altitude, stable_attitude" },
    { "state": "CLIMB", "action": "vertical_ascent, max_2_mps" },
    { "state": "CRUISE", "action": "horizontal_flight, 5_mps_nominal" },
    { "state": "MANEUVER", "action": "agile_flight, up_to_15_mps" },
    { "state": "LANDING", "action": "controlled_descent_0.5_mps" },
    { "state": "EMERGENCY_DESCENT", "action": "failsafe_0.5_mps_down" }
  ],
  "control_loops": {
    "position_loop": "1 Hz (outer)",
    "velocity_loop": "10 Hz (middle)",
    "attitude_loop": "100 Hz (inner, tight)",
    "wing_flap_sync": "3 Hz (independent, asynchronous with motors)"
  },
  "mood_modulation": {
    "mood_happy": "2× wing flap amplitude, 1.2× max speed",
    "mood_alert": "1.5× wing flap, 1× speed",
    "mood_calm": "0.5× flap, 0.5× speed",
    "mood_fearful": "3× flap, emergency descent triggered"
  }
}
```

---

## Integration Workflow

### **Step 1: Start Here**
Read `flying_dragon_full.json` to understand the complete bot composition and subsystems.

### **Step 2: Understand Components**
- Consult `README.md` for assembly procedures (4-phase build)
- Review `INDEX.md` for complete JSON reference
- Check `QUICK_REFERENCE.md` for pin assignments

### **Step 3: Examine Subsystem Details**
- **Head subsystem**: Review component definitions in `head/`
- **Flight controller subsystem**: Review components in `motors/`, `sensors/`, `power/`, `control/`

### **Step 4: Reference Hardware Specs**
- Use `hardware_templates/` for procurement details
- Cross-reference with component definitions in part folders

### **Step 5: Build & Test**
- Follow assembly procedures in `README.md`
- Use pin assignments from `QUICK_REFERENCE.md`
- Validate component definitions against hardware templates

---

## Key Design Decisions

| Decision | Rationale | Impact |
|----------|-----------|--------|
| **Flattened root** | Direct access to master files | Faster iteration, less nesting |
| **Part-based folders** | Align with dragon anatomy & subsystems | Intuitive organization, scalable |
| **Centralized hardware_templates** | Single source of truth for specs | Reduces duplication, easier updates |
| **Cascaded PID loops** | Separate control rates for stability | 100 Hz attitude loop, 1-10 Hz outer loops |
| **Wing flap at 3 Hz** | Independent from motor control | Realistic articulation without motor interference |
| **6S LiPo battery** | 22.2V supports 4× 40A ESCs efficiently | 700g weight, 15 min flight achievable |
| **1.48:1 thrust-to-weight** | Safe margin for maneuvers | 50% throttle at hover, 50% reserve for climbing |

---

## Scaling for Future Dragons

When adding new dragon variants (e.g., dragon_frost_guardian, dragon_ancient_fire):

1. **Create new bot definition** in bothome root: `dragon_{name}_{id}.json`
2. **Reference existing subsystems** from config/components/subsystem/ if compatible
3. **Create new subsystems** (in config/components/subsystem/) if unique behavior needed
4. **Reuse hardware templates** from hardware_templates/ directory
5. **Document** variant-specific differences in a companion README

Example structure for a fire dragon variant:
```json
{
  "bot_name": "dragon_ancient_fire_001",
  "description": "Quad-motor dragon with flame effects",
  "subsystems": [
    "flying_dragon_head",           // Reuse
    "flying_dragon_flight_controller", // Reuse
    "fire_breath_system"            // New subsystem
  ]
}
```

---

## File Maintenance

### **Never Edit (Generated/Reference)**
- `hardware_templates/*.json` — Read-only procurement specs

### **Edit Safely (Configuration)**
- Part folders: `head/`, `wings/`, `body/`, `motors/`, `sensors/`, `power/`, `control/`
- Master definition: `flying_dragon_full.json` (reflect subsystem changes only)

### **Keep Updated (Documentation)**
- `README.md` — Assembly procedures, changes, tips
- `INDEX.md` — Component reference, JSON structure
- `QUICK_REFERENCE.md` — Pins, specs, file locations
- `BOTHOME_STRUCTURE.md` — This organizational guide

---

## Verification Checklist

- [x] flying_dragons/ subfolder flattened
- [x] Master definition (flying_dragon_full.json) at bothome root
- [x] Documentation moved to bothome root (README, INDEX, QUICK_REFERENCE)
- [x] Hardware templates accessible at bothome/hardware_templates/
- [x] Part-based folders created: head, wings, body, motors, sensors, power, control
- [x] Organizational document (BOTHOME_STRUCTURE.md) created
- [ ] Component definitions moved to appropriate part folders
- [ ] Subsystem integration file verified in config/components/subsystem/
- [ ] Hardware specs verified in config/hardware/

---

**Next Steps**: Organize component definition files into their corresponding part folders based on dragon anatomy alignment.

