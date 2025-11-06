# FLYING DRAGON - COMPONENT SPECIFICATION & JSON TEMPLATES

**Date**: November 4, 2025  
**Purpose**: Define JSON structure for flying dragon bot composition in P32 system

---

## 1. COMPONENT JSON DEFINITIONS

### 1.1 Motor Controller Component Definition

**File**: `config/components/functional/flying_dragon_motor_controller.json`

```json
{
  "name": "flying_dragon_motor_controller",
  "category": "flight_system",
  "description": "Brushless motor control for quadcopter lift system",
  "version": "1.0.0",
  "hardware_templates": [
    {
      "name": "T_MOTOR_U3_580KV",
      "motor_type": "brushless_sensored",
      "kv_rating": 580,
      "max_continuous_thrust_gf": 950,
      "max_continuous_current_a": 40,
      "weight_g": 55,
      "motor_count": 4,
      "esc_type": "HOBBYWING_40A_ESC",
      "esc_current_rating_a": 40,
      "esc_weight_g": 28,
      "esc_count": 4,
      "pwm_frequency_hz": 490,
      "pwm_pins": [12, 13, 14, 15],
      "prop_diameter_inches": 10,
      "prop_pitch_inches": 5
    }
  ],
  "power_requirements": {
    "nominal_voltage_v": 14.8,
    "min_voltage_v": 11.0,
    "max_current_a": 160,
    "average_current_hover_a": 40
  },
  "dependencies": [],
  "function_signature": "flying_dragon_motor_controller_init(void); void flying_dragon_motor_controller_act(void);"
}
```

### 1.2 Wing Servo Controller Component

**File**: `config/components/functional/flying_dragon_wing_servo_controller.json`

```json
{
  "name": "flying_dragon_wing_servo_controller",
  "category": "articulation_system",
  "description": "6-servo wing articulation control (3 per wing)",
  "version": "1.0.0",
  "hardware_templates": [
    {
      "name": "MG996R_SERVO",
      "servo_type": "digital_servo",
      "torque_kg_cm": 11,
      "speed_sec_60deg": 0.16,
      "weight_g": 55,
      "servo_count": 6,
      "pwm_pins": [16, 17, 18, 19, 20, 21],
      "pwm_frequency_hz": 50,
      "control_range_degrees": [0, 180],
      "flap_frequency_hz": 3.0,
      "wing_articulation": {
        "left_wing": {
          "shoulder_servo": 0,
          "twist_servo": 1,
          "flap_servo": 2
        },
        "right_wing": {
          "shoulder_servo": 3,
          "twist_servo": 4,
          "flap_servo": 5
        }
      }
    }
  ],
  "power_requirements": {
    "nominal_voltage_v": 5.0,
    "nominal_current_per_servo_a": 0.8,
    "peak_current_a": 5.0
  },
  "dependencies": [],
  "function_signature": "flying_dragon_wing_servo_controller_init(void); void flying_dragon_wing_servo_controller_act(void);"
}
```

### 1.3 Sensor Fusion Component

**File**: `config/components/functional/flying_dragon_sensor_fusion.json`

```json
{
  "name": "flying_dragon_sensor_fusion",
  "category": "sensor_system",
  "description": "IMU + barometer for attitude & altitude estimation",
  "version": "1.0.0",
  "hardware_templates": [
    {
      "name": "ICM20689_BMP390",
      "imu_type": "ICM20689",
      "imu_i2c_address": "0x68",
      "imu_sample_rate_hz": 1000,
      "imu_accel_range_g": 16,
      "imu_gyro_range_dps": 2000,
      "barometer_type": "BMP390",
      "barometer_i2c_address": "0x77",
      "barometer_sample_rate_hz": 50,
      "barometer_accuracy_m": 2.0,
      "i2c_bus": {
        "sda_pin": 21,
        "scl_pin": 22,
        "frequency_hz": 400000
      }
    }
  ],
  "power_requirements": {
    "nominal_voltage_v": 3.3,
    "nominal_current_a": 0.05
  },
  "dependencies": ["flying_dragon_flight_safety"],
  "function_signature": "flying_dragon_sensor_fusion_init(void); void flying_dragon_sensor_fusion_act(void);"
}
```

### 1.4 Flight Safety Component

**File**: `config/components/functional/flying_dragon_flight_safety.json`

```json
{
  "name": "flying_dragon_flight_safety",
  "category": "safety_system",
  "description": "Failsafe monitoring, battery management, emergency procedures",
  "version": "1.0.0",
  "hardware_templates": [
    {
      "name": "WATCHDOG_BATTERY_MONITOR",
      "watchdog_timeout_ms": 500,
      "battery_adc_pin": 36,
      "battery_min_voltage_v": 11.0,
      "battery_cells": 4,
      "low_battery_alert_v": 11.5,
      "altitude_floor_m": 0.5,
      "gyro_limit_dps": 500,
      "emergency_descent_rate_ms": 0.5
    }
  ],
  "power_requirements": {
    "nominal_voltage_v": 3.3,
    "nominal_current_a": 0.01
  },
  "dependencies": ["flying_dragon_sensor_fusion", "flying_dragon_motor_controller"],
  "function_signature": "flying_dragon_flight_safety_init(void); void flying_dragon_flight_safety_act(void);"
}
```

### 1.5 Behavior Sequencer Component

**File**: `config/components/functional/flying_dragon_behavior_sequencer.json`

```json
{
  "name": "flying_dragon_behavior_sequencer",
  "category": "control_system",
  "description": "Flight state machine, mood-based behavior, autonomous sequences",
  "version": "1.0.0",
  "hardware_templates": [
    {
      "name": "ESP32_S3_FLIGHT_LOGIC",
      "flight_controller": "ESP32_S3",
      "clock_speed_mhz": 240,
      "flight_loop_frequency_hz": 100,
      "states": [
        "IDLE",
        "ARMING",
        "HOVER",
        "CLIMB",
        "CRUISE_FORWARD",
        "AGGRESSIVE_MANEUVER",
        "LANDING",
        "EMERGENCY_DESCENT"
      ],
      "mood_influence": {
        "calm": {
          "max_roll_deg": 20,
          "max_pitch_deg": 20,
          "max_climb_rate_ms": 0.5,
          "wing_beat_freq_hz": 2.5
        },
        "playful": {
          "max_roll_deg": 45,
          "max_pitch_deg": 35,
          "max_climb_rate_ms": 1.0,
          "wing_beat_freq_hz": 3.5
        },
        "aggressive": {
          "max_roll_deg": 60,
          "max_pitch_deg": 50,
          "max_climb_rate_ms": 1.5,
          "wing_beat_freq_hz": 4.0
        }
      }
    }
  ],
  "power_requirements": {
    "nominal_voltage_v": 3.3,
    "nominal_current_a": 0.2
  },
  "dependencies": [
    "flying_dragon_sensor_fusion",
    "flying_dragon_motor_controller",
    "flying_dragon_wing_servo_controller",
    "flying_dragon_flight_safety"
  ],
  "function_signature": "flying_dragon_behavior_sequencer_init(void); void flying_dragon_behavior_sequencer_act(void);"
}
```

---

## 2. BOT FAMILY DEFINITIONS

### 2.1 Flying Dragon Full Bot

**File**: `config/bots/bot_families/flying_dragons/flying_dragon_full.json`

```json
{
  "name": "flying_dragon_full",
  "description": "Complete flying dragon with head (eyes/mouth/speaker) and flight systems",
  "version": "1.0.0",
  "creature_family": "dragon",
  "subsystems": [
    {
      "name": "flying_dragon_head",
      "type": "creature_head",
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
      "type": "flight_system",
      "enabled": true,
      "components": [
        "flying_dragon_sensor_fusion<ICM20689_BMP390>",
        "flying_dragon_flight_safety<WATCHDOG_BATTERY_MONITOR>",
        "flying_dragon_motor_controller<T_MOTOR_U3_580KV>",
        "flying_dragon_wing_servo_controller<MG996R_SERVO>",
        "flying_dragon_behavior_sequencer<ESP32_S3_FLIGHT_LOGIC>"
      ],
      "configuration": {
        "flight_loop_frequency_hz": 100,
        "wing_flap_frequency_hz": 3.0,
        "thrust_to_weight_ratio": 1.5,
        "battery_capacity_mah": 5000
      }
    }
  ],
  "assembly_notes": "Flying dragon with articulated wings (3 DoF per side) and quadcopter propulsion. Sensor fusion provides stability; failsafes ensure safe descent.",
  "mass_budget_g": {
    "airframe": 500,
    "motors": 220,
    "escs": 140,
    "servos": 330,
    "flight_controller": 100,
    "battery": 550,
    "wiring": 120,
    "payload": 100,
    "total": 2560
  },
  "performance": {
    "max_flight_time_min": 15,
    "cruise_speed_ms": 5,
    "max_speed_ms": 15,
    "service_ceiling_m": 50,
    "thrust_to_weight": 1.5
  }
}
```

### 2.2 Flight Controller Subsystem

**File**: `config/components/subsystem/flying_dragon_flight_system.json`

```json
{
  "name": "flying_dragon_flight_system",
  "category": "subsystem",
  "description": "Integrated flight control subsystem (motors + wings + sensors + safety)",
  "type": "container",
  "components": [
    "flying_dragon_sensor_fusion<ICM20689_BMP390>",
    "flying_dragon_flight_safety<WATCHDOG_BATTERY_MONITOR>",
    "flying_dragon_motor_controller<T_MOTOR_U3_580KV>",
    "flying_dragon_wing_servo_controller<MG996R_SERVO>",
    "flying_dragon_behavior_sequencer<ESP32_S3_FLIGHT_LOGIC>"
  ],
  "execution_priority": [
    1,
    2,
    3,
    4,
    5
  ],
  "notes": "Execute in order: sensors  safety  motors  servos  behavior logic"
}
```

---

## 3. HARDWARE TEMPLATE SPECIFICATIONS

### 3.1 Motor Template: T_MOTOR_U3_580KV

```json
{
  "template_name": "T_MOTOR_U3_580KV",
  "manufacturer": "T-Motor",
  "model": "U3 PRO 580KV",
  "part_number": "U3 PRO 580",
  "specifications": {
    "type": "brushless_outrunner",
    "kv": 580,
    "pole_pairs": 14,
    "stator_diameter_mm": 20,
    "stator_length_mm": 15,
    "no_load_current_a": 0.2,
    "max_continuous_current_a": 40,
    "max_burst_current_a": 60,
    "internal_resistance_ohms": 0.12,
    "weight_g": 55,
    "voltage_nominal_v": 14.8,
    "voltage_range_v": [11.0, 16.8],
    "thrust_at_nominal_voltage": {
      "rpm": 10640,
      "static_thrust_gf": 950,
      "static_thrust_kg": 0.95,
      "power_w": 150
    },
    "sensored": true,
    "sensor_type": "hall_effect",
    "connector": "XT30"
  },
  "mechanical_interface": {
    "shaft_diameter_mm": 3,
    "shaft_length_mm": 5,
    "mount_pattern": "M2_holes",
    "mount_diameter_mm": 19
  }
}
```

### 3.2 ESC Template: HOBBYWING_40A_ESC

```json
{
  "template_name": "HOBBYWING_40A_ESC",
  "manufacturer": "Hobbywing",
  "model": "XRotor 40A",
  "specifications": {
    "continuous_current_a": 40,
    "burst_current_a": 50,
    "voltage_nominal_v": 14.8,
    "voltage_range_v": [2.0, 6.0],
    "lipo_cell_range": "2S6S",
    "weight_g": 28,
    "firmware": "BLHeli-32",
    "pwm_frequency_hz": 490,
    "pwm_resolution_bit": 13,
    "input_protocol": ["PWM", "DShot600"],
    "telemetry": ["voltage", "current", "temperature"],
    "connector_input": "JST_XH",
    "connector_motor": "HXT_3.5mm_bullet",
    "programming": "via_USB_adapter"
  },
  "thermal": {
    "temperature_sensor": true,
    "low_voltage_throttle": true,
    "overtemp_cutoff_c": 120
  }
}
```

### 3.3 Servo Template: MG996R_SERVO

```json
{
  "template_name": "MG996R_SERVO",
  "manufacturer": "MG",
  "model": "MG996R",
  "specifications": {
    "type": "digital_servo",
    "torque_kg_cm": 11,
    "speed_unloaded_sec_60deg": 0.16,
    "voltage_nominal_v": 5.0,
    "voltage_range_v": [4.5, 6.0],
    "no_load_current_a": 0.1,
    "loaded_current_a": 0.8,
    "peak_current_a": 1.2,
    "weight_g": 55,
    "dimensions_mm": [40.7, 20, 37.5],
    "control_range_degrees": [0, 180],
    "connector": "3pin_male",
    "pwm_frequency_hz": 50,
    "pwm_pulse_range_us": [500, 2500],
    "dead_band_us": 5,
    "gear_type": "plastic"
  }
}
```

### 3.4 Sensor Template: ICM20689_BMP390

```json
{
  "template_name": "ICM20689_BMP390",
  "components": [
    {
      "sensor": "ICM20689",
      "type": "6_axis_imu",
      "i2c_address": "0x68",
      "i2c_frequency_hz": 400000,
      "accel_range_g": [2, 4, 8, 16],
      "accel_sensitivity_lsb_g": 2048,
      "accel_noise_density_ug_rtHz": 75,
      "gyro_range_dps": [250, 500, 1000, 2000],
      "gyro_sensitivity_lsb_dps": 131,
      "gyro_noise_density": 12.5,
      "sample_rate_hz": 1000,
      "temperature_range_c": [-40, 85],
      "power_consumption_ma": 3.5
    },
    {
      "sensor": "BMP390",
      "type": "barometer",
      "i2c_address": "0x77",
      "pressure_range_pa": [30000, 110000],
      "temperature_range_c": [-40, 85],
      "altitude_accuracy_m": 2.0,
      "altitude_resolution_m": 0.1,
      "sample_rate_hz": 50,
      "power_consumption_ma": 0.2
    }
  ],
  "breakout_board": {
    "dimensions_mm": [25, 25],
    "mounting_holes": "M2",
    "connector": "4pin_i2c",
    "address_selectable": false
  }
}
```

---

## 4. EXAMPLE MOTOR MIXING EQUATIONS (C++)

```cpp
// Motor mixing for quad X-configuration
// Motors arranged: 1=Front-Left, 2=Front-Right, 3=Rear-Left, 4=Rear-Right

void compute_motor_commands(
    float throttle_cmd,       // [0.01.0]
    float roll_cmd,           // [1.01.0] ( left, + right)
    float pitch_cmd,          // [1.01.0] ( forward, + backward)
    float yaw_rate_cmd,       // [1.01.0] ( CCW, + CW)
    float motor_out[4])       // Output PWM [0.01.0]
{
    // Compute motor mixing (simplified, no cross-coupling)
    float m1 = throttle_cmd + pitch_cmd - roll_cmd + yaw_rate_cmd;
    float m2 = throttle_cmd + pitch_cmd + roll_cmd - yaw_rate_cmd;
    float m3 = throttle_cmd - pitch_cmd - roll_cmd - yaw_rate_cmd;
    float m4 = throttle_cmd - pitch_cmd + roll_cmd + yaw_rate_cmd;

    // Clamp to [0.01.0] to prevent saturation
    motor_out[0] = constrain(m1, 0.0f, 1.0f);
    motor_out[1] = constrain(m2, 0.0f, 1.0f);
    motor_out[2] = constrain(m3, 0.0f, 1.0f);
    motor_out[3] = constrain(m4, 0.0f, 1.0f);
}
```

---

## 5. CALIBRATION PROCEDURES

### 5.1 Motor Calibration Checklist

```markdown
1. **ESC Initialization** (per ESC)
   - Connect single motor to ESC
   - Power on without throttle (silence = ready)
   - Gradually increase throttle from 0100%
   - Verify smooth acceleration (no cogging, no buzzing)
   - Record RPM at 50%, 75%, 100% throttle

2. **Static Thrust Measurement**
   - Place motor+prop on scale (isolated setup)
   - Set throttle to 0% (idle ~1000 s PWM)
   - Record baseline weight (should be motor weight only)
   - Increase to 50% throttle (1500 s PWM), wait 5 sec, record scale reading
   - Subtract baseline to get static thrust at 50%
   - Repeat at 75% and 100% throttle
   - Compare to spec data (should be 5%)

3. **Cross-Motor Validation**
   - Repeat static thrust test for all 4 motors
   - Verify all motors produce 95% of rated thrust
   - Flag any motor <90% for replacement

4. **ESC PWM Response**
   - Connect PWM signal generator to ESC
   - Send 1000 s pulse (idle), verify no spin
   - Increase to 1100 s, verify gentle spin
   - Increase to 1500 s, record response time (<100 ms expected)
   - Record RPM vs. PWM curve for control loop tuning
```

### 5.2 Sensor Calibration

```markdown
**IMU (ICM20689)**
   - Place on level surface
   - Read accelerometer: X=0, Y=0, Z=~1g (0.05g tolerance)
   - Read gyroscope: X=0, Y=0, Z=0 (5/s tolerance)
   - If out of tolerance, perform hard-iron/soft-iron calibration via datasheetgyro calibration

**Barometer (BMP390)**
   - Place at known altitude (e.g., sea level = 0 m)
   - Record raw pressure reading
   - Compare to atmospheric table for that date/location
   - Apply altitude offset calibration if >2 m error
```

---

## 6. POWER BUDGET EXAMPLE

```markdown
**Hover Condition** (4 motors at 50% throttle)
   - Motor current per motor: 10 A
   - Total motor current: 40 A
   - ESC overhead: ~2 A (voltage conversion losses)
   - Servo current (at rest): 0.5 A
   - Flight logic + sensors: 0.3 A
   - **Total system current: ~43 A**

**Battery capacity**: 5000 mAh = 5 Ah
**Flight time**: 5 Ah  43 A  7 min hover

**Cruise Condition** (mixed forward flight, motors at 40% throttle)
   - Motor current per motor: 8 A
   - Total motor current: 32 A
   - Servo current (active wing flap @ 3 Hz): 1.0 A
   - Overhead: ~2.5 A
   - **Total system current: ~35.5 A**

**Flight time**: 5 Ah  35.5 A  8.5 min mixed flight

**Peak Condition** (aggressive maneuver, 75% throttle)
   - Motor current: 30 A (total)
   - Servo current: 1.5 A
   - Overhead: 3 A
   - **Total: ~34.5 A (short burst only)**
   - Duration: Limited to ~5 sec before battery voltage drops below LVC threshold
```

---

## 7. INTEGRATION VALIDATION SCRIPT (Python)

**File**: `tools/validate_flying_dragon_config.py`

```python
#!/usr/bin/env python3
"""
Validate flying dragon bot configuration for consistency and feasibility.
"""

import json
import os
from pathlib import Path

def validate_motor_thrust_budget():
    """Verify that total motor thrust exceeds 1.5 dragon weight."""
    dragon_mass_kg = 2.5
    thrust_per_motor_gf = 950
    total_thrust_gf = 4 * thrust_per_motor_gf
    required_thrust_gf = dragon_mass_kg * 1000 * 1.5  # 1.5 T/W ratio
    
    print(f"Motor Thrust Budget:")
    print(f"  Dragon mass: {dragon_mass_kg} kg")
    print(f"  Total available thrust: {total_thrust_gf} gf ({total_thrust_gf/1000:.2f} kgf)")
    print(f"  Required (1.5 T/W): {required_thrust_gf} gf ({required_thrust_gf/1000:.2f} kgf)")
    print(f"   PASS" if total_thrust_gf > required_thrust_gf else "   FAIL")

def validate_power_budget():
    """Verify battery capacity is sufficient for target flight time."""
    battery_capacity_mah = 5000
    avg_current_hover_a = 43
    avg_current_cruise_a = 35.5
    
    hover_time_min = battery_capacity_mah / (avg_current_hover_a * 1000 * 60)
    cruise_time_min = battery_capacity_mah / (avg_current_cruise_a * 1000 * 60)
    
    print(f"\nPower Budget:")
    print(f"  Battery: {battery_capacity_mah} mAh")
    print(f"  Hover time: {hover_time_min:.1f} min @ {avg_current_hover_a} A avg")
    print(f"  Cruise time: {cruise_time_min:.1f} min @ {avg_current_cruise_a} A avg")
    print(f"  Target: 1012 min mixed flight")
    print(f"   PASS" if cruise_time_min > 10 else "   MARGINAL (consider larger battery)")

def validate_servo_torque():
    """Check wing servo torque is sufficient for articulation."""
    servo_torque_kg_cm = 11
    wing_moment_arm_cm = 5  # Approximate moment arm for wing flap
    wing_mass_g = 200
    wing_weight_kg = wing_mass_g / 1000
    
    required_torque_kg_cm = wing_weight_kg * wing_moment_arm_cm
    
    print(f"\nServo Torque Budget:")
    print(f"  Servo torque: {servo_torque_kg_cm} kg-cm")
    print(f"  Wing mass: {wing_mass_g} g")
    print(f"  Required torque (estimated): {required_torque_kg_cm:.1f} kg-cm")
    print(f"   PASS" if servo_torque_kg_cm > required_torque_kg_cm * 2 else "   MARGINAL")

def validate_component_files():
    """Verify all required component files exist."""
    components = [
        "flying_dragon_motor_controller",
        "flying_dragon_wing_servo_controller",
        "flying_dragon_sensor_fusion",
        "flying_dragon_flight_safety",
        "flying_dragon_behavior_sequencer"
    ]
    
    print(f"\nComponent Files Validation:")
    comp_dir = Path("config/components/functional")
    all_exist = True
    
    for comp in components:
        json_exists = (comp_dir / f"{comp}.json").exists()
        src_exists = (comp_dir / f"{comp}.src").exists()
        hdr_exists = (comp_dir / f"{comp}.hdr").exists()
        
        status = "" if (json_exists and src_exists and hdr_exists) else ""
        print(f"  {status} {comp}: .json={json_exists}, .src={src_exists}, .hdr={hdr_exists}")
        all_exist = all_exist and json_exists and src_exists and hdr_exists
    
    return all_exist

if __name__ == "__main__":
    print("=" * 60)
    print("FLYING DRAGON BOT CONFIGURATION VALIDATION")
    print("=" * 60)
    
    validate_motor_thrust_budget()
    validate_power_budget()
    validate_servo_torque()
    validate_component_files()
    
    print("\n" + "=" * 60)
    print("Validation complete. Address any  FAIL items before deployment.")
    print("=" * 60)
```

---

## 8. DEPLOYMENT CHECKLIST

Before proceeding to Phase 1 prototyping:

- [ ] All JSON definitions reviewed for correctness
- [ ] Component templates match physical hardware specifications
- [ ] Motor thrust calculations verified (1.5 T/W ratio confirmed)
- [ ] Power budget validated for 10+ min flight time
- [ ] Sensor accuracies acceptable (altitude 2 m, IMU 0.1 g)
- [ ] Safety limits defined (altitude floor, gyro limit, battery LVC)
- [ ] P32 component architecture aligned with motor/servo/sensor separation
- [ ] SharedMemory integration planned for multi-chip coordination
- [ ] Failsafe procedures documented and testable
- [ ] Tether safety harness design approved
- [ ] All component source files (`*.src`) use Allman braces and ASCII encoding
- [ ] Build system configured to aggregate component files into subsystem `.cpp`
- [ ] Telemetry logging prepared for post-flight analysis

---

**Document Status**: Ready for Implementation  
**Next Action**: Begin Phase 1 prototyping per FLYING_DRAGON_DEEP_DIVE.md
