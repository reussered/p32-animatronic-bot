# Goblin Head Assembly Weight and Motor Load Analysis

## Overview
This document calculates the complete weight of the goblin head assembly and determines the motor requirements for responsive head movement. Analysis includes static loads, dynamic forces, and safety margins for brisk animatronic response.

## Component Weight Analysis

### 3D Printed Skull Components

#### Primary Skull Structure
```json
{
  "goblin_skull_base": {
    "material": "PLA plastic",
    "density": "1.25 g/cm",
    "volume_estimate": "180 cm",
    "infill_density": "20%",
    "effective_volume": "36 cm + 10 cm shell",
    "weight": "57.5 grams",
    "weight_oz": "2.03 oz"
  },
  "component_mounting_rings": {
    "material": "PLA plastic", 
    "density": "1.25 g/cm",
    "volume_estimate": "25 cm",
    "infill_density": "100% (structural)",
    "weight": "31.25 grams",
    "weight_oz": "1.10 oz"
  },
  "decorative_shells_total": {
    "eye_shells_pair": "15 grams",
    "nose_shell": "8 grams", 
    "mouth_shell": "12 grams",
    "total_weight": "35 grams",
    "weight_oz": "1.23 oz"
  }
}
```

**Total 3D Printed Components: 123.75 grams (4.36 oz)**

### Electronic Components

#### Displays and Sensors
```json
{
  "gc9a01_displays": {
    "unit_weight": "8 grams",
    "quantity": 3,
    "total_weight": "24 grams",
    "weight_oz": "0.85 oz",
    "description": "2x eyes + 1x mouth display"
  },
  "hc_sr04_sensor": {
    "unit_weight": "9 grams", 
    "quantity": 1,
    "total_weight": "9 grams",
    "weight_oz": "0.32 oz"
  },
  "esp32_s3_devkitc1": {
    "unit_weight": "25 grams",
    "quantity": 1, 
    "total_weight": "25 grams",
    "weight_oz": "0.88 oz"
  }
}
```

#### Wiring and Connectors
```json
{
  "wire_harness": {
    "spi_bus_wires": "12 grams",
    "power_distribution": "8 grams", 
    "gpio_sensor_wires": "4 grams",
    "connectors_total": "6 grams",
    "total_weight": "30 grams",
    "weight_oz": "1.06 oz"
  }
}
```

**Total Electronics: 88 grams (3.11 oz)**

### Mechanical Hardware

#### Fasteners and Mounting
```json
{
  "mounting_hardware": {
    "m3_screws": "15 grams",
    "m3_nuts": "8 grams", 
    "washers": "3 grams",
    "threaded_inserts": "12 grams",
    "total_weight": "38 grams",
    "weight_oz": "1.34 oz"
  }
}
```

**Total Hardware: 38 grams (1.34 oz)**

## Complete Head Assembly Weight Summary

```json
{
  "weight_breakdown": {
    "3d_printed_components": "123.75 grams",
    "electronics": "88 grams", 
    "mechanical_hardware": "38 grams",
    "assembly_total": "249.75 grams",
    "assembly_total_oz": "8.81 oz",
    "assembly_total_kg": "0.250 kg",
    "safety_margin_10%": "274.7 grams",
    "design_weight": "275 grams (0.275 kg)"
  }
}
```

## Center of Mass Analysis

### Goblin Head Center of Mass (CoM)
Based on component positioning and weights:

```json
{
  "center_of_mass_calculation": {
    "reference_frame": "nose_center origin (0,0,0)",
    "skull_structure_com": {"x": 0, "y": -5, "z": 10},
    "electronics_com": {"x": 0, "y": -15, "z": -20}, 
    "combined_com": {"x": 0, "y": -8.2, "z": -2.1},
    "com_mm": {"x": 0, "y": -8.2, "z": -2.1},
    "com_inches": {"x": 0, "y": -0.32, "z": -0.08},
    "description": "CoM is 8.2mm behind nose center, 2.1mm below"
  }
}
```

### Moment Arm Calculations
Distance from neck pivot point to center of mass:

```json
{
  "moment_arms": {
    "neck_joint_position": {"x": 0, "y": -20, "z": -60},
    "head_com_position": {"x": 0, "y": -8.2, "z": -2.1}, 
    "moment_arm_vector": {"x": 0, "y": 11.8, "z": 57.9},
    "moment_arm_length": "59.1 mm",
    "moment_arm_inches": "2.33 inches"
  }
}
```

## Motor Load Calculations

### Static Load Analysis

#### Gravitational Torque
```json
{
  "static_torque_requirements": {
    "head_weight": "0.275 kg",
    "gravitational_acceleration": "9.81 m/s",
    "gravitational_force": "2.70 N",
    "moment_arm_length": "0.0591 m",
    "static_holding_torque": "0.159 Nm",
    "static_holding_torque_oz_in": "22.5 ozin",
    "safety_factor": 2.0,
    "required_holding_torque": "0.318 Nm",
    "required_holding_torque_oz_in": "45.0 ozin"
  }
}
```

### Dynamic Load Analysis

#### Acceleration Requirements for "Brisk Response"
Target performance: 90 head turn in 0.5 seconds

```json
{
  "dynamic_performance_targets": {
    "angular_displacement": "90 degrees (/2 radians)",
    "movement_time": "0.5 seconds",
    "angular_acceleration": "12.57 rad/s", 
    "peak_angular_velocity": "6.28 rad/s",
    "description": "Smooth S-curve motion profile"
  }
}
```

#### Moment of Inertia Calculation
```json
{
  "moment_of_inertia": {
    "head_mass": "0.275 kg",
    "moment_arm": "0.0591 m",
    "point_mass_approximation": "9.59  10 kgm",
    "shape_correction_factor": 1.3,
    "effective_moment_of_inertia": "1.25  10 kgm",
    "description": "Accounts for distributed mass and irregular shape"
  }
}
```

#### Dynamic Torque Requirements
```json
{
  "dynamic_torque_calculation": {
    "moment_of_inertia": "1.25  10 kgm",
    "angular_acceleration": "12.57 rad/s",
    "acceleration_torque": "0.0157 Nm", 
    "acceleration_torque_oz_in": "2.22 ozin",
    "static_torque": "0.159 Nm",
    "total_dynamic_torque": "0.175 Nm",
    "total_dynamic_torque_oz_in": "24.7 ozin"
  }
}
```

### Complete Motor Requirements

#### Minimum Motor Specifications
```json
{
  "motor_requirements": {
    "holding_torque_minimum": "0.318 Nm (45 ozin)",
    "dynamic_torque_minimum": "0.175 Nm (25 ozin)", 
    "safety_factor": 2.5,
    "recommended_holding_torque": "0.795 Nm (112.5 ozin)",
    "recommended_dynamic_torque": "0.438 Nm (62 ozin)",
    "step_resolution": "1.8 or finer",
    "microstepping": "16x minimum for smooth motion"
  }
}
```

## Recommended Motor Selection

### NEMA 17 Stepper Motor Specifications
```json
{
  "recommended_motors": {
    "primary_recommendation": {
      "model": "NEMA 17 High Torque",
      "holding_torque": "1.26 Nm (178 ozin)",
      "step_angle": "1.8",
      "rated_current": "2.0 A",
      "resistance": "1.4  per phase",
      "inductance": "3.0 mH per phase",
      "weight": "280 grams",
      "safety_margin": "3.96x static, 7.2x dynamic",
      "suitability": "Excellent - high safety margin"
    },
    "alternative_option": {
      "model": "NEMA 17 Standard",
      "holding_torque": "0.65 Nm (92 ozin)", 
      "step_angle": "1.8",
      "rated_current": "1.2 A",
      "resistance": "3.5  per phase",
      "inductance": "2.8 mH per phase", 
      "weight": "230 grams",
      "safety_margin": "2.04x static, 3.7x dynamic",
      "suitability": "Marginal - minimal safety margin"
    }
  }
}
```

### Motor Driver Requirements
```json
{
  "driver_specifications": {
    "recommended_driver": "TMC2209 or A4988",
    "microstepping": "16x (0.1125 per microstep)",
    "current_rating": "2.5 A minimum",
    "voltage_supply": "12V recommended", 
    "step_frequency": "50 kHz maximum",
    "acceleration_control": "Required for smooth S-curves"
  }
}
```

## Multi-Axis Movement Analysis

### Dual Motor Configuration (Yaw + Pitch)
```json
{
  "dual_axis_requirements": {
    "yaw_motor": {
      "axis": "Vertical rotation (left/right)",
      "load_type": "Primary gravitational load",
      "required_torque": "0.795 Nm (112.5 ozin)",
      "motor_recommendation": "NEMA 17 High Torque"
    },
    "pitch_motor": {
      "axis": "Horizontal rotation (up/down)",
      "load_type": "Reduced gravitational component",
      "required_torque": "0.398 Nm (56.3 ozin)",
      "motor_recommendation": "NEMA 17 Standard acceptable"
    }
  }
}
```

### Combined System Weight Impact
```json
{
  "system_weight_with_motors": {
    "head_assembly": "0.275 kg",
    "yaw_motor": "0.280 kg",
    "pitch_motor": "0.280 kg", 
    "motor_mounts": "0.065 kg",
    "total_head_system": "0.900 kg",
    "total_head_system_lbs": "1.98 lbs",
    "impact_on_torso_motors": "Additional load for body controllers"
  }
}
```

## Performance Validation

### Response Time Calculations
```json
{
  "performance_metrics": {
    "90_degree_turn_time": "0.5 seconds",
    "180_degree_turn_time": "1.0 seconds", 
    "settling_time": "0.1 seconds",
    "total_response_time": "0.6 seconds for 90",
    "angular_resolution": "0.1125 (16x microstepping)",
    "positioning_accuracy": "0.2"
  }
}
```

### Power Consumption Analysis
```json
{
  "power_requirements": {
    "single_motor_power": "24 watts (12V  2A)",
    "dual_motor_power": "48 watts maximum",
    "typical_operating_power": "24 watts (one motor active)",
    "standby_power": "6 watts (holding current)",
    "power_supply_requirement": "12V @ 5A minimum"
  }
}
```

## Safety and Reliability Factors

### Mechanical Safety Margins
```json
{
  "safety_analysis": {
    "torque_safety_factor": "2.5x minimum calculated load",
    "acceleration_safety_factor": "2.0x target performance",
    "structural_safety_factor": "3.0x expected loads",
    "fatigue_life_target": "100,000 operation cycles",
    "environmental_rating": "Indoor operation, 10-40C"
  }
}
```

### Failure Mode Analysis
```json
{
  "failure_modes": {
    "motor_overload": {
      "probability": "Low with proper sizing",
      "mitigation": "Current limiting, thermal protection"
    },
    "mechanical_binding": {
      "probability": "Medium", 
      "mitigation": "Proper bearing selection, lubrication"
    },
    "controller_failure": {
      "probability": "Low",
      "mitigation": "Watchdog timers, safe position defaults"
    }
  }
}
```

## Integration with Full Body Architecture

### Torso Motor Load Impact
```json
{
  "body_integration_loads": {
    "additional_torso_load": "0.900 kg (complete head system)",
    "torso_moment_arm": "~0.3 m (estimated neck length)",
    "additional_torso_torque": "2.65 Nm",
    "torso_motor_upgrade": "May require NEMA 23 motors",
    "cascade_effect": "Head weight affects all body joints"
  }
}
```

## Recommendations Summary

### Motor Selection Final Recommendation
1. **Yaw Motor:** NEMA 17 High Torque (178 ozin) - TMC2209 driver
2. **Pitch Motor:** NEMA 17 High Torque (178 ozin) - TMC2209 driver  
3. **Power Supply:** 12V @ 6A (safety margin for dual motors)
4. **Control:** ESP32-S3 with step/direction interface
5. **Microstepping:** 16x for smooth motion (0.1125 resolution)

### Performance Expectations
- **90 head turn:** 0.5 seconds
- **Positioning accuracy:** 0.2
- **Operational life:** >100,000 cycles
- **Power consumption:** 24W typical, 48W peak

---

**Document Version:** 1.0  
**Last Updated:** October 14, 2025  
**Analysis Status:** Complete - Ready for motor procurement  
**Next Phase:** Motor mount design and integration testing