# Multi-ESP32 Component Architecture Specification

## Overview

The P32 Animatronic Bot uses a **distributed multi-ESP32 architecture** where different subsystems run on separate ESP32 controllers. This document defines how components are organized, assigned to controllers, and how code generation works across the distributed system.

## Core Architecture Principles

### 1. Distributed Controller Model
- **Each subsystem** runs on a dedicated ESP32 chip
- **Controllers** are defined by hardware type (ESP32-S3-DevKitC-1, etc.)
- **Components** belong to exactly ONE subsystem/controller
- **SharedMemory** handles inter-ESP32 communication transparently

### 2. Recursive Component Composition
- **Single JSON file** defines entire creature via recursive composition
- **`contained_components`** keyword lists child components
- **Controller boundaries** determine subsystem membership during traversal
- **Hierarchical inheritance** flows down the component tree

## Component-to-Controller Assignment

### Controller Keyword
```json
{
  "component_id": "goblin_head",
  "controller": "ESP32_S3_DEVKITC_1",
  "contained_components": [
    "config/components/positioned/goblin_left_eye.json",
    "config/components/positioned/goblin_right_eye.json"
  ]
}
```

### Assignment Rules
1. **Controller Detection**: When recursive descent encounters a `controller` keyword, that component defines a subsystem boundary
2. **Membership Inheritance**: All components contained within a controller belong to that subsystem
3. **Single Membership**: Each component belongs to exactly ONE subsystem (no splitting across controllers)
4. **Boundary Enforcement**: Components cannot span multiple controllers

### Example Architecture
```json
goblin_full.json (creature definition - complete system manifest)
{
  "component_id": "goblin_full",
  "component_type": "CREATURE_ASSEMBLY",
  "contained_components": [
    "config/subsystems/goblin_head.json",
    "config/subsystems/goblin_torso.json"
  ],
  
  "subsystem_manifest": {
    "goblin_head": {
      "controller": "ESP32_S3_DEVKITC_1",
      "components": [
        "goblin_left_eye",
        "goblin_right_eye", 
        "goblin_nose",
        "goblin_ear_left",
        "goblin_ear_right"
      ]
    },
    "goblin_torso": {
      "controller": "ESP32_S3_DEVKITC_1",
      "components": [
        "goblin_speaker",
        "goblin_wifi",
        "goblin_power_management"
      ]
    }
  }
}

goblin_head.json (controller: ESP32_S3_HEAD)
 goblin_left_eye.json  BELONGS TO: head subsystem
 goblin_right_eye.json  BELONGS TO: head subsystem
 goblin_nose.json  BELONGS TO: head subsystem
 goblin_ear_left.json  BELONGS TO: head subsystem

goblin_torso.json (controller: ESP32_S3_TORSO)
 goblin_speaker.json  BELONGS TO: torso subsystem
 goblin_wifi.json  BELONGS TO: torso subsystem
 goblin_power.json  BELONGS TO: torso subsystem
```

## Code Generation Architecture

### Per-Component Files (1:1 Mapping)
Each component generates its own individual files:
```
goblin_left_eye.json  
  - src/components/goblin_left_eye.cpp
  - include/components/goblin_left_eye.hpp

goblin_speaker.json 
  - src/components/goblin_speaker.cpp  
  - include/components/goblin_speaker.hpp
```

### Per-Subsystem Files (Many:1 Mapping)
All components in a subsystem contribute to shared subsystem files:
```
HEAD SUBSYSTEM (goblin_head controller):
  - src/subsystems/goblin_head/goblin_head_dispatch_tables.cpp
  - include/subsystems/goblin_head/goblin_head_dispatch_tables.hpp
  - src/subsystems/goblin_head/goblin_head_component_functions.cpp
  - include/subsystems/goblin_head/goblin_head_component_functions.hpp

TORSO SUBSYSTEM (goblin_torso controller):
  - src/subsystems/goblin_torso/goblin_torso_dispatch_tables.cpp
  - include/subsystems/goblin_torso/goblin_torso_dispatch_tables.hpp
  - src/subsystems/goblin_torso/goblin_torso_component_functions.cpp
  - include/subsystems/goblin_torso/goblin_torso_component_functions.hpp
```

### Dispatch Table Contents
Each subsystem's dispatch table contains:
```cpp
// goblin_head_dispatch_tables.cpp
init_function_t head_init_table[] = {
    goblin_left_eye_init,    // from head subsystem
    goblin_right_eye_init,   // from head subsystem  
    goblin_nose_init,        // from head subsystem
    goblin_ear_left_init     // from head subsystem
};

act_function_t head_act_table[] = {
    goblin_left_eye_act,     // from head subsystem
    goblin_right_eye_act,    // from head subsystem
    goblin_nose_act,         // from head subsystem  
    goblin_ear_left_act      // from head subsystem
};

uint32_t head_hitcount_table[] = {
    5,    // goblin_left_eye hitCount
    5,    // goblin_right_eye hitCount
    100,  // goblin_nose hitCount
    10    // goblin_ear_left hitCount
};
```

## Recursive Descent Algorithm

### Traversal Logic
```
1. Start at creature root (goblin_full.json)
2. For each component in contained_components:
   a. Load component JSON
   b. Check for 'controller' keyword
   c. If controller found:
      - Mark as subsystem boundary
      - Set current_controller = controller value
      - Create new subsystem context
   d. Add component to current_controller's subsystem
   e. If component has contained_components:
      - Recursively process children (inherit current_controller)
   f. Continue until all components processed
```

### Controller Inheritance
- Components **inherit** controller from parent until new controller encountered
- **Explicit controller** always overrides inherited controller
- **Root components** must have explicit controller (no default)

## Inter-Subsystem Communication

### SharedMemory Architecture
- **Transparent Communication**: Components use `SharedMemory.read()/write()`
- **ESP-NOW Backend**: SharedMemory encapsulates ESP-NOW mesh internally
- **Multi-ESP32 Sync**: All ESP32s stay synchronized automatically
- **No Explicit Mesh Config**: Components don't configure ESP-NOW directly

### Communication Pattern
```cpp
// Component on HEAD ESP32
auto shared_data = GSM.read<MoodState>();  // Local read
shared_data.curiosity += 10;               // Modify locally  
GSM.write<MoodState>(shared_data);         // Update HEAD + sync to TORSO
```

## File Organization

### Directory Structure
```
src/
 components/                    # Individual component files
    goblin_left_eye.cpp       # Per-component implementation
    goblin_speaker.cpp        # Per-component implementation
    ...
 subsystems/                   # Subsystem coordination files
    goblin_head/              # HEAD ESP32 subsystem
       goblin_head_dispatch_tables.cpp
       goblin_head_component_functions.cpp
    goblin_torso/             # TORSO ESP32 subsystem
        goblin_torso_dispatch_tables.cpp
        goblin_torso_component_functions.cpp
include/
 components/                   # Individual component headers
    goblin_left_eye.hpp      # Per-component interface
    ...
 subsystems/                  # Subsystem coordination headers
     goblin_head/             # HEAD ESP32 subsystem
     goblin_torso/            # TORSO ESP32 subsystem

config/
 templates/
    creature_patterns/       # Creature pattern templates
        humanoid_standard.json    # Standard bipedal humanoid
        quadruped_standard.json   # 4-legged creatures
        winged_biped.json         # Humanoid + wings
        winged_quadruped.json     # Quadruped + wings
        multi_winged_insect.json  # 6 legs + 4 wings
        spider_arachnid.json     # 8 legs + abdomen
        serpentine.json           # Segmented body sections
 bots/
    bot_families/
        goblins/
            goblin_family.json    # References creature_pattern
            goblin_full.json      # Generated from pattern
 subsystems/
     goblin_head.json         # Implementation details (controller, components)
     goblin_torso.json        # Implementation details (controller, components)
     goblin_left_arm.json     # Implementation details (controller, components)
     goblin_left_hand.json    # Implementation details (controller, components)
     goblin_right_arm.json    # Implementation details (controller, components)
     goblin_right_hand.json   # Implementation details (controller, components)
     goblin_left_leg.json     # Implementation details (controller, components)
     goblin_left_foot.json    # Implementation details (controller, components)
     goblin_right_leg.json    # Implementation details (controller, components)
     goblin_right_foot.json   # Implementation details (controller, components)
```

## Build System Integration

### Platform.ini Configuration
Each ESP32 gets its own build environment:
```ini
[env:goblin_head]
platform = espressif32
board = esp32-s3-devkitc-1
build_src_filter = 
    +<components/>
    +<subsystems/goblin_head/>
    -<subsystems/goblin_torso/>

[env:goblin_torso]  
platform = espressif32
board = esp32-s3-devkitc-1
build_src_filter =
    +<components/>
    +<subsystems/goblin_torso/>
    -<subsystems/goblin_head/>
```

### Compilation Targets
- **Individual builds**: `pio run -e goblin_head`, `pio run -e goblin_torso`
- **Component isolation**: Each ESP32 only compiles its assigned components
- **Shared components**: Component .cpp files compiled into multiple subsystems if needed

## Validation Requirements

### JSON Schema Validation
1. **Controller keyword** must be valid ESP32 hardware identifier
2. **Contained components** must exist and be valid JSON
3. **Subsystem boundaries** must not create circular dependencies  
4. **Component membership** must be unambiguous (single controller per component)

### Code Generation Validation  
1. **Dispatch tables** must contain all subsystem components
2. **Function names** must match component_id patterns
3. **HitCount arrays** must parallel dispatch table order
4. **Include paths** must resolve correctly per subsystem

## Migration Guide

### Converting Existing JSON Files
1. **Add controller keywords** to subsystem boundary components
2. **Convert positioned_components**  `contained_components`
3. **Remove ESP-NOW mesh configs** (handled by SharedMemory)
4. **Update validation scripts** to enforce new patterns

### Example Conversion
```json
// OLD FORMAT
{
  "positioned_components": [...],
  "mesh_role": "SLAVE_NODE_HIGH_PRIORITY",
  "mesh_capabilities": ["ESP_NOW_SLAVE"]
}

// NEW FORMAT  
{
  "controller": "ESP32_S3_DEVKITC_1",
  "contained_components": [...]
}
```

## Future Extensibility

### Creature Pattern Template System

**Pattern Templates** define standardized subsystem structures for different creature types:

```json
config/templates/creature_patterns/humanoid_standard.json
{
  "pattern_id": "humanoid_standard",
  "description": "Standard bipedal humanoid with articulated limbs",
  "subsystems": [
    {"subsystem_id": "head", "primary_functions": ["vision", "audio_input", "audio_output", "sensors"]},
    {"subsystem_id": "torso", "primary_functions": ["coordination", "power", "wireless", "core_sensors"]},
    {"subsystem_id": "left_arm", "primary_functions": ["arm_articulation", "shoulder_movement"]},
    {"subsystem_id": "left_hand", "primary_functions": ["finger_articulation", "grip_sensors"]},
    {"subsystem_id": "right_arm", "primary_functions": ["arm_articulation", "shoulder_movement"]},
    {"subsystem_id": "right_hand", "primary_functions": ["finger_articulation", "grip_sensors"]},
    {"subsystem_id": "left_leg", "primary_functions": ["leg_articulation", "hip_movement"]},
    {"subsystem_id": "left_foot", "primary_functions": ["foot_articulation", "ground_sensors"]},
    {"subsystem_id": "right_leg", "primary_functions": ["leg_articulation", "hip_movement"]},
    {"subsystem_id": "right_foot", "primary_functions": ["foot_articulation", "ground_sensors"]}
  ]
}
```

**Family Definition** references pattern template:
```json
config/bots/bot_families/goblins/goblin_family.json
{
  "family_id": "goblin",
  "creature_pattern": "humanoid_standard",
  "family_specific_config": {
    "size_scale": 0.7,
    "aggression_level": "high",
    "ear_style": "pointed"
  }
}
```

**Template Generator** creates stub subsystem files from pattern, preserving information hiding:
- Pattern defines WHAT subsystems exist (structure)
- Individual subsystem JSON files define HOW they're implemented (controller, components)
- Implementation details stay encapsulated in subsystem files

**Available Patterns:**
- `humanoid_standard.json` - Standard bipedal humanoid (10 subsystems)
- `quadruped_standard.json` - 4-legged creatures (head, torso, 4 legs)
- `winged_biped.json` - Humanoid + wings (12 subsystems)
- `winged_quadruped.json` - Quadruped + wings (8 subsystems)
- `multi_winged_insect.json` - 6 legs + 4 wings (11 subsystems)
- `spider_arachnid.json` - 8 legs + abdomen (10 subsystems)
- `serpentine.json` - Segmented body sections (variable subsystems)

### Adding New Subsystems
1. **Define controller** in appropriate JSON component
2. **Update build system** with new environment
3. **Code generation** automatically creates subsystem files
4. **SharedMemory** handles communication without changes

### Multi-Creature Support
- **Same architecture** works for cat, bear, dragon creatures
- **Controller reuse** across different creature types
- **Modular subsystems** can be mixed and matched

## Critical Design Decisions

### Why This Architecture?
1. **Hardware Reality**: Physical ESP32 chips require separate builds
2. **Performance Isolation**: CPU-intensive tasks isolated per subsystem  
3. **Development Simplicity**: Components written without distributed system complexity
4. **Scalability**: Easy to add more subsystems (arms, legs, tail)
5. **Reliability**: Subsystem failure doesn't crash entire creature

### Architecture Constraints
1. **Single Controller**: Components cannot span multiple ESP32s
2. **Explicit Boundaries**: Controller keywords must be explicit
3. **Synchronous Communication**: SharedMemory provides synchronous interface
4. **Code Generation**: Dispatch tables generated per subsystem, not globally

---

**Version**: 1.0  
**Last Updated**: 2025-10-22  
**Status**: Authoritative Architecture Document