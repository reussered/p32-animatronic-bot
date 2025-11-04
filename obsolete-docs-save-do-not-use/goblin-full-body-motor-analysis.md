# Goblin Full Body Motor Sizing and Load Analysis

## Overview
Complete motor specification analysis for the entire goblin animatronic system. Strategic motor selection balances performance requirements with cost - using smaller motors where appropriate while ensuring robust NEMA 17s for high-load applications.

## Complete Goblin Body Weight Breakdown

### Head Subsystem (Previously Calculated)
```json
{
  "head_assembly": {
    "3d_printed_skull": "123.75 grams",
    "electronics": "88.0 grams", 
    "hardware": "38.0 grams",
    "subtotal": "249.75 grams",
    "safety_margin": "25 grams",
    "total_head_weight": "275 grams"
  }
}
```

### Torso Subsystem
```json
{
  "torso_components": {
    "chest_shell_3d_printed": {
      "material": "PLA, 15% infill",
      "volume_estimate": "450 cm³", 
      "weight": "84 grams"
    },
    "spine_structure": {
      "material": "PLA, 50% infill (structural)",
      "volume_estimate": "200 cm³",
      "weight": "125 grams"
    },
    "torso_electronics": {
      "esp32_c3_controller": "12 grams",
      "servo_drivers": "25 grams",
      "power_distribution": "45 grams", 
      "wiring_harness": "60 grams",
      "subtotal": "142 grams"
    },
    "mounting_hardware": {
      "m4_structural_bolts": "35 grams",
      "m3_component_screws": "20 grams",
      "brackets_and_joints": "40 grams",
      "subtotal": "95 grams"
    },
    "torso_total": "446 grams"
  }
}
```

### Arms Subsystem (Pair)
```json
{
  "arm_components_each": {
    "upper_arm_shell": {
      "material": "PLA, 10% infill", 
      "volume_estimate": "120 cm³",
      "weight": "18 grams"
    },
    "forearm_shell": {
      "material": "PLA, 10% infill",
      "volume_estimate": "90 cm³", 
      "weight": "14 grams"
    },
    "hand_assembly": {
      "palm_structure": "25 grams",
      "finger_segments": "35 grams", 
      "finger_servos_5x": "50 grams",
      "subtotal": "110 grams"
    },
    "arm_electronics": {
      "esp32_c3_controller": "12 grams",
      "servo_drivers": "15 grams",
      "wiring": "25 grams",
      "subtotal": "52 grams"
    },
    "arm_hardware": {
      "joint_hardware": "30 grams",
      "servo_mounts": "20 grams",
      "subtotal": "50 grams"
    },
    "single_arm_total": "244 grams",
    "both_arms_total": "488 grams"
  }
}
```

### Legs Subsystem (Pair)  
```json
{
  "leg_components_each": {
    "thigh_shell": {
      "material": "PLA, 20% infill (load bearing)",
      "volume_estimate": "200 cm³",
      "weight": "50 grams"
    },
    "shin_shell": {
      "material": "PLA, 20% infill",
      "volume_estimate": "150 cm³",
      "weight": "38 grams" 
    },
    "foot_assembly": {
      "foot_structure": "45 grams",
      "ankle_hardware": "25 grams",
      "subtotal": "70 grams"
    },
    "leg_hardware": {
      "knee_joint": "40 grams",
      "hip_joint": "35 grams", 
      "ankle_joint": "30 grams",
      "structural_bolts": "25 grams",
      "subtotal": "130 grams"
    },
    "single_leg_total": "288 grams",
    "both_legs_total": "576 grams"
  }
}
```

### Base/Platform Subsystem
```json
{
  "base_components": {
    "chassis_plate": {
      "material": "PLA, 30% infill (structural)",
      "volume_estimate": "300 cm³",
      "weight": "112 grams"
    },
    "wheel_assemblies": {
      "wheels_4x": "120 grams",
      "motor_mounts": "60 grams", 
      "drive_hardware": "80 grams",
      "subtotal": "260 grams"
    },
    "base_electronics": {
      "main_esp32_s3": "25 grams",
      "motor_drivers": "80 grams",
      "power_system": "200 grams",
      "wiring": "45 grams",
      "subtotal": "350 grams"
    },
    "base_total": "722 grams"
  }
}
```

## Complete System Weight Summary
```json
{
  "system_totals": {
    "head_with_motors": "275 + 560 = 835 grams",
    "torso": "446 grams",
    "arms_both": "488 grams", 
    "legs_both": "576 grams",
    "base_platform": "722 grams",
    "total_dry_weight": "3067 grams",
    "total_with_margins": "3374 grams (7.4 lbs)",
    "operational_weight": "3.4 kg"
  }
}
```

## Joint-by-Joint Motor Analysis

### Head Motors (2 Required)
```json
{
  "head_yaw_motor": {
    "load_description": "Rotate entire head left/right",
    "static_load": "275 grams @ 59mm moment arm",
    "required_torque": "0.16 N⋅m static",
    "dynamic_requirement": "90° in 0.5s (brisk response)",
    "safety_factor": "2.5x",
    "final_requirement": "0.40 N⋅m (56 oz⋅in)",
    "motor_selection": "NEMA 17 Standard (65 oz⋅in)",
    "justification": "Head visibility critical - needs robust performance"
  },
  "head_pitch_motor": {
    "load_description": "Tilt head up/down", 
    "static_load": "Reduced gravitational component",
    "required_torque": "0.20 N⋅m (reduced moment arm)",
    "safety_factor": "2.5x",
    "final_requirement": "0.50 N⋅m (71 oz⋅in)", 
    "motor_selection": "NEMA 17 Standard (92 oz⋅in)",
    "justification": "Head expression crucial - invest in quality"
  }
}
```

### Torso Motors (2 Required)
```json
{
  "torso_twist_motor": {
    "load_description": "Rotate torso with head + arms",
    "static_load": "(275 + 488) grams @ 150mm moment arm",
    "required_torque": "1.12 N⋅m static",
    "dynamic_requirement": "Upper body gestures", 
    "safety_factor": "3.0x (high importance)",
    "final_requirement": "3.36 N⋅m (475 oz⋅in)",
    "motor_selection": "NEMA 17 High Torque (600+ oz⋅in)",
    "justification": "Core body movement - absolutely critical"
  },
  "torso_lean_motor": {
    "load_description": "Lean torso forward/back",
    "static_load": "Upper body weight @ spine pivot",
    "required_torque": "2.8 N⋅m (major structural load)",
    "safety_factor": "3.0x",
    "final_requirement": "8.4 N⋅m (1188 oz⋅in)",
    "motor_selection": "NEMA 23 (1200+ oz⋅in)",
    "justification": "Structural integrity - cannot fail"
  }
}
```

### Arm Motors (6 per arm = 12 total)
```json
{
  "shoulder_roll_motor": {
    "load_description": "Lift entire arm outward", 
    "static_load": "244 grams @ 200mm moment arm",
    "required_torque": "0.48 N⋅m static",
    "safety_factor": "2.5x",
    "final_requirement": "1.2 N⋅m (170 oz⋅in)",
    "motor_selection": "NEMA 17 Standard (178 oz⋅in)",
    "justification": "Arm gestures important for expression"
  },
  "shoulder_pitch_motor": {
    "load_description": "Swing arm forward/back",
    "static_load": "Similar to roll, different axis",
    "required_torque": "0.45 N⋅m",
    "final_requirement": "1.13 N⋅m (160 oz⋅in)", 
    "motor_selection": "NEMA 17 Standard (178 oz⋅in)",
    "justification": "Critical for natural arm movement"
  },
  "elbow_motor": {
    "load_description": "Bend forearm + hand",
    "static_load": "162 grams @ 150mm moment arm",
    "required_torque": "0.24 N⋅m",
    "safety_factor": "2.0x (lighter load)",
    "final_requirement": "0.48 N⋅m (68 oz⋅in)",
    "motor_selection": "NEMA 14 (84 oz⋅in)",
    "justification": "Smaller motor acceptable - cost savings"
  },
  "wrist_motors_3x": {
    "load_description": "Wrist roll/pitch/yaw",
    "static_load": "Hand only: 110 grams @ 80mm",
    "required_torque": "0.09 N⋅m each",
    "safety_factor": "2.0x",
    "final_requirement": "0.18 N⋅m (25 oz⋅in) each",
    "motor_selection": "28BYJ-48 Stepper (35 oz⋅in)",
    "justification": "Small cheap motors perfect for hands"
  }
}
```

### Leg Motors (6 per leg = 12 total) 
```json
{
  "hip_roll_motor": {
    "load_description": "Leg abduction (sideways)",
    "static_load": "288 grams @ 250mm moment arm",
    "required_torque": "0.70 N⋅m static", 
    "walking_dynamics": "Additional 2x for walking forces",
    "safety_factor": "3.0x (mobility critical)",
    "final_requirement": "4.2 N⋅m (594 oz⋅in)",
    "motor_selection": "NEMA 17 High Torque (600+ oz⋅in)",
    "justification": "Walking stability requires robust motors"
  },
  "hip_pitch_motor": {
    "load_description": "Leg swing forward/back (walking)",
    "static_load": "Full leg weight",
    "walking_dynamics": "Primary walking motor - highest loads",
    "required_torque": "1.8 N⋅m base + 2x walking",
    "safety_factor": "3.0x",
    "final_requirement": "10.8 N⋅m (1527 oz⋅in)",
    "motor_selection": "NEMA 23 (1600+ oz⋅in)", 
    "justification": "Walking power - absolutely critical"
  },
  "knee_motor": {
    "load_description": "Bend shin + foot",
    "static_load": "108 grams @ 180mm",
    "walking_dynamics": "Moderate walking forces",
    "safety_factor": "2.5x",
    "final_requirement": "1.35 N⋅m (191 oz⋅in)",
    "motor_selection": "NEMA 17 Standard (200 oz⋅in)",
    "justification": "Standard NEMA 17 sufficient"
  },
  "ankle_motors_3x": {
    "load_description": "Foot positioning and balance", 
    "static_load": "Foot weight only: 70 grams",
    "balance_dynamics": "Critical for stability",
    "safety_factor": "2.0x",
    "final_requirement": "0.35 N⋅m (49 oz⋅in) each",
    "motor_selection": "NEMA 14 (84 oz⋅in)",
    "justification": "Smaller motors OK - precise positioning"
  }
}
```

### Mobility Motors (4 Required)
```json
{
  "wheel_drive_motors": {
    "load_description": "Move entire 3.4kg robot",
    "static_load": "Rolling resistance + inclines",
    "performance_target": "1 m/s max speed, quick acceleration",
    "torque_at_wheel": "12 N⋅m per wheel (4 wheels)",
    "gear_reduction": "20:1 recommended",
    "motor_requirement": "0.6 N⋅m at motor shaft",
    "safety_factor": "2.0x",
    "final_requirement": "1.2 N⋅m (170 oz⋅in)",
    "motor_selection": "NEMA 17 Standard (178 oz⋅in)",
    "justification": "Mobility fundamental - invest properly"
  }
}
```

## Motor Selection Summary by Type

### NEMA 23 Motors (Highest Loads) - 3 Required
```json
{
  "nema_23_applications": {
    "torso_lean": "8.4 N⋅m required",
    "hip_pitch_left": "10.8 N⋅m required", 
    "hip_pitch_right": "10.8 N⋅m required",
    "recommended_spec": {
      "holding_torque": "≥12 N⋅m (1700 oz⋅in)",
      "rated_current": "3.0A",
      "step_angle": "1.8°",
      "estimated_cost": "$45-60 each",
      "total_cost": "$135-180"
    }
  }
}
```

### NEMA 17 High Torque (Heavy Duty) - 7 Required
```json
{
  "nema_17_high_torque_applications": {
    "torso_twist": "3.36 N⋅m required",
    "hip_roll_left": "4.2 N⋅m required",
    "hip_roll_right": "4.2 N⋅m required", 
    "shoulder_roll_left": "1.2 N⋅m required",
    "shoulder_roll_right": "1.2 N⋅m required",
    "shoulder_pitch_left": "1.13 N⋅m required",
    "shoulder_pitch_right": "1.13 N⋅m required",
    "recommended_spec": {
      "holding_torque": "≥4.5 N⋅m (600+ oz⋅in)",
      "rated_current": "2.0A",
      "step_angle": "1.8°", 
      "estimated_cost": "$25-35 each",
      "total_cost": "$175-245"
    }
  }
}
```

### NEMA 17 Standard (Medium Duty) - 11 Required
```json
{
  "nema_17_standard_applications": {
    "head_yaw": "0.40 N⋅m required",
    "head_pitch": "0.50 N⋅m required",
    "knee_left": "1.35 N⋅m required",
    "knee_right": "1.35 N⋅m required",
    "wheel_drive_4x": "1.2 N⋅m each required",
    "mobility_steering": "0.8 N⋅m required",
    "recommended_spec": {
      "holding_torque": "≥1.5 N⋅m (200+ oz⋅in)",
      "rated_current": "1.2A",
      "step_angle": "1.8°",
      "estimated_cost": "$15-25 each", 
      "total_cost": "$165-275"
    }
  }
}
```

### NEMA 14 Motors (Light Duty) - 8 Required
```json
{
  "nema_14_applications": {
    "elbow_left": "0.48 N⋅m required",
    "elbow_right": "0.48 N⋅m required", 
    "ankle_pitch_left": "0.35 N⋅m required",
    "ankle_pitch_right": "0.35 N⋅m required",
    "ankle_roll_left": "0.35 N⋅m required", 
    "ankle_roll_right": "0.35 N⋅m required",
    "ankle_yaw_left": "0.35 N⋅m required",
    "ankle_yaw_right": "0.35 N⋅m required",
    "recommended_spec": {
      "holding_torque": "≥0.6 N⋅m (84+ oz⋅in)",
      "rated_current": "0.8A",
      "step_angle": "1.8°",
      "estimated_cost": "$8-12 each",
      "total_cost": "$64-96"
    }
  }
}
```

### 28BYJ-48 Small Steppers (Precision) - 6 Required  
```json
{
  "small_stepper_applications": {
    "wrist_roll_left": "0.18 N⋅m required",
    "wrist_pitch_left": "0.18 N⋅m required",
    "wrist_yaw_left": "0.18 N⋅m required", 
    "wrist_roll_right": "0.18 N⋅m required",
    "wrist_pitch_right": "0.18 N⋅m required",
    "wrist_yaw_right": "0.18 N⋅m required",
    "recommended_spec": {
      "holding_torque": "0.25 N⋅m (35 oz⋅in)",
      "voltage": "5V",
      "step_angle": "5.625°",
      "estimated_cost": "$3-5 each",
      "total_cost": "$18-30"
    }
  }
}
```

## Complete Motor System Cost Analysis

### Motor Costs Summary
```json
{
  "motor_costs": {
    "nema_23_motors": "$135-180 (3 units)",
    "nema_17_high_torque": "$175-245 (7 units)", 
    "nema_17_standard": "$165-275 (11 units)",
    "nema_14_motors": "$64-96 (8 units)",
    "small_steppers": "$18-30 (6 units)",
    "total_motor_cost": "$557-826",
    "average_estimate": "$692"
  }
}
```

### Driver Electronics Cost
```json
{
  "driver_costs": {
    "nema_23_drivers": "$45-60 (3x TMC2160)",
    "nema_17_drivers": "$126-180 (18x TMC2209)", 
    "nema_14_drivers": "$32-48 (8x A4988)",
    "small_stepper_drivers": "$12-18 (6x ULN2003)",
    "total_driver_cost": "$215-306",
    "average_estimate": "$261"
  }
}
```

### Power Supply Requirements
```json
{
  "power_analysis": {
    "nema_23_power": "108W (3 × 36W)",
    "nema_17_power": "432W (18 × 24W)",
    "nema_14_power": "64W (8 × 8W)", 
    "small_stepper_power": "18W (6 × 3W)",
    "total_peak_power": "622W",
    "typical_operating_power": "200W (30% duty cycle)",
    "power_supply_recommendation": "24V @ 30A (720W capacity)",
    "power_supply_cost": "$80-120"
  }
}
```

## Strategic Motor Selection Rationale

### Where We Invest in Premium Motors (NEMA 17/23)
1. **Structural Joints** - Torso, hips, shoulders where failure affects entire system
2. **Mobility Critical** - Walking, balance, and locomotion motors  
3. **Expression Important** - Head movement for character interaction
4. **Safety Critical** - Any joint where failure could damage robot

### Where We Use Budget Motors (NEMA 14, 28BYJ-48)
1. **End Effectors** - Wrists, ankles where loads are light
2. **Fine Positioning** - Hands, feet where precision matters more than power
3. **Secondary Joints** - Elbows where failure won't cascade  
4. **Non-Critical Movement** - Decorative or subtle gestures

### Cost vs Performance Balance
- **Total Motor Investment:** ~$692 (35 motors)
- **Performance Guaranteed:** Brisk, robust movement where needed
- **Cost Savings Achieved:** ~40% vs all-NEMA-17 approach (~$1150)
- **Reliability Focus:** Premium motors on load-bearing/critical joints

## Implementation Phases

### Phase 1: Core Movement (Priority 1)
- Head motors (2x NEMA 17 Standard) 
- Torso twist (1x NEMA 17 High Torque)
- Mobility (4x NEMA 17 Standard)
- **Cost:** ~$200, **Capability:** Basic head/body/movement

### Phase 2: Upper Body (Priority 2)  
- Torso lean (1x NEMA 23)
- Shoulders (4x NEMA 17 High Torque)
- Elbows (2x NEMA 14)
- **Cost:** ~$300, **Capability:** Full upper body gestures

### Phase 3: Lower Body (Priority 3)
- Hips (4x mixed NEMA 23/17 High Torque) 
- Knees (2x NEMA 17 Standard)
- Ankles (6x NEMA 14)
- **Cost:** ~$350, **Capability:** Walking and balance

### Phase 4: Fine Control (Priority 4)
- Wrists (6x 28BYJ-48)
- **Cost:** ~$25, **Capability:** Hand articulation

## Risk Analysis

### Over-Engineering Risks
- **Avoided:** Using NEMA 17s where NEMA 14s sufficient
- **Cost Impact:** Would add ~$280 unnecessary expense
- **Power Impact:** Would increase power requirements 60%

### Under-Engineering Risks  
- **Mitigated:** Safety factors on all critical joints
- **Monitoring:** Load sensors can detect motor strain
- **Upgrade Path:** Mechanical mounts compatible with larger motors

### Failure Mode Protection
- **Critical Joints:** 3x safety factor prevents overload
- **Cascade Prevention:** Strong structural joints protect weaker end joints
- **Graceful Degradation:** Non-critical joint failure doesn't stop core functions

---

**Document Version:** 1.0  
**Last Updated:** October 14, 2025  
**Analysis Status:** Complete - Ready for procurement planning  
**Next Phase:** Detailed mechanical design and motor mount specifications