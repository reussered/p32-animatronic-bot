# Three-Tier Inheritance Architecture Specification

## Problem Statement
P32 Animatronic Bot project faces combinatorial explosion:
- 15+ creature types × 11 subsystems × 5+ components = **1000+ configuration files**
- 90%+ identical content with minor creature-specific variations
- Maintenance nightmare: changes require updates across hundreds of files

## Solution: Three-Tier Template Inheritance

### Tier 1: Universal Base Templates
**Location**: `config/subsystems/base/`
**Purpose**: Hardware specifications common to ALL creatures

```
base/
├── head_base.json          # ESP32-S3 controller, power, communication
├── torso_base.json         # ESP32-C3 controller, power distribution
├── arm_base.json           # ESP32-C3 controller, servo specifications  
├── leg_base.json           # ESP32-C3 controller, locomotion servos
├── hand_base.json          # ESP32-C3 controller, finger articulation
└── base_mobility.json      # ESP32-C3 controller, platform movement
```

### Tier 2: Creature Family Templates  
**Location**: `config/subsystems/families/`
**Purpose**: Biological/mechanical characteristics shared within creature types

```
families/
├── mammal_family.json      # Fur, warm-blood, organic movement patterns
├── reptile_family.json     # Scales, cold-blood, predator behaviors  
├── humanoid_family.json    # Bipedal, tool-use, social behaviors
├── mechanical_family.json  # Precision, tech aesthetics, robotic movement
├── undead_family.json      # Decay, erratic movement, horror behaviors
└── insect_family.json      # Compound eyes, multi-limb, hive behaviors
```

### Tier 3: Individual Creature Overrides
**Location**: `config/bots/creatures/`
**Purpose**: Specific creature variations (scale, colors, unique features)

```
creatures/
├── bear_grizzly.json       # Scale 1.6x, brown fur, territorial
├── dragon_ancient.json     # Scale 2.0x, fire effects, wing servos
├── android_medical.json    # White shell, precision sensors, healing
└── zombie_crawler.json     # Decay effects, erratic servos, horror
```

## Inheritance Resolution Chain

```json
{
  "creature_id": "bear_grizzly",
  "inherits": [
    "config/subsystems/base/head_base.json",
    "config/subsystems/families/mammal_family.json"
  ],
  "overrides": {
    "scale_factor": 1.6,
    "fur_color": "grizzly_brown",
    "behavioral_profile": "territorial_protective",
    "eye_spacing": "4.2 INCH"
  }
}
```

**Resolution Process**:
1. Load base template (ESP32 specs, power, communication)
2. Apply family template (mammal characteristics)  
3. Apply creature overrides (bear-specific scale/color)
4. Generate final configuration with all inherited + overridden values

## Component Template System

### Hardware Component Templates
**Location**: `config/templates/hardware/`
```
hardware/
├── servo_mg996r_template.json     # High-torque servo specifications
├── servo_sg90_template.json       # Micro servo specifications  
├── display_gc9a01_template.json   # Circular LCD specifications
├── sensor_hcsr04_template.json    # Ultrasonic sensor specifications
└── controller_esp32s3_template.json
```

### Positioned Component Templates  
**Location**: `config/templates/positioned/`
```
positioned/
├── shoulder_joint_template.json   # Standard shoulder servo positioning
├── eye_pair_template.json         # Bilateral eye positioning logic
├── finger_set_template.json       # 5-finger hand servo layout
└── leg_assembly_template.json     # Hip/knee/ankle servo chain
```

## File Reduction Calculation

### Before (Current Path):
- 15 creatures × 11 subsystems × 5 components = **825 component files**
- Plus controllers, interfaces, behaviors = **~1000 total files**

### After (Inheritance System):
- **6 base templates** (universal hardware)
- **6 family templates** (biological/mechanical types)  
- **15 creature overrides** (specific variations)
- **20 hardware templates** (reusable components)
- **10 positioned templates** (standard assemblies)
- **Total: ~57 files** instead of 1000+

## Implementation Priority

1. **Phase 1**: Create base subsystem templates
2. **Phase 2**: Create family characteristic templates  
3. **Phase 3**: Build JSON inheritance processor/validator
4. **Phase 4**: Convert existing creatures to new system
5. **Phase 5**: Create component template library

## Validation System

```python
def resolve_creature_config(creature_file):
    config = load_json(creature_file)
    
    # Apply inheritance chain
    for inherit_file in config['inherits']:
        base_config = load_json(inherit_file)
        config = deep_merge(base_config, config)
    
    # Apply overrides
    config = apply_overrides(config, config['overrides'])
    
    # Validate final configuration
    validate_esp32_pins(config)
    validate_power_budget(config) 
    validate_servo_limits(config)
    
    return config
```

This architecture eliminates the combinatorial explosion while preserving creature uniqueness and maintainability.