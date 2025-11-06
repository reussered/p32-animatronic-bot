# JSON Construction Rules for P32 Animatronic Bot

## Overview

This document defines the unified JSON construction rules for the P32 Animatronic Bot component architecture. All component containment uses the standardized `"components"` keyword to indicate contained components.

## Core Principles

### 1. Recursive Component Composition

- **Single JSON file** defines entire creature via recursive composition
- **`components`** keyword lists child components (standardized from all variants)
- **Depth-first traversal**: Container component initializes, then recursively loads and initializes all contained components
- **Hierarchical inheritance** flows down the component tree

### 2. Component Loading Behavior

- **MANDATORY**: Components loaded EXACTLY as encountered during JSON parsing
- **MANDATORY**: Depth-first recursive traversal: container  contained components
- **Loading Order**: Components loaded in JSON array order during traversal
- **Initialization Order**: Container component initializes first, then all contained components

### 3. Component Types and Hierarchy

#### Three-Level Component Hierarchy

*Conceptual example showing the hierarchical structure (actual components vary by subsystem):*

```text
Level 0: CREATURE_ASSEMBLY (goblin_full)           hitCount: 100  (10 sec)
   Level 1: SYSTEM (system_core)                hitCount: 100  (10 sec)
   Level 2: FAMILY (goblin_personality)         hitCount: 25   (2.5 sec)
   Level 3: SUBSYSTEM_ASSEMBLIES
        goblin_head                             hitCount: 25   (2.5 sec)
           Level 4: HARDWARE_DEVICES
               left_eye                        hitCount: 5    (500ms)
               right_eye                       hitCount: 5    (500ms)
               nose                            hitCount: 15   (1.5 sec)
               mouth                           hitCount: 3    (300ms)
               speaker                         hitCount: 7    (1.4 sec)
               left_ear                        hitCount: 10   (1 sec)
               right_ear                       hitCount: 10   (1 sec)
               ... (additional components)
       
        goblin_torso                            hitCount: 50   (5 sec)
           Level 4: HARDWARE_DEVICES
               spine_servo                     hitCount: 20   (2 sec)
               waist_servo                     hitCount: 20   (2 sec)
       
        goblin_arm_left                         hitCount: 30   (3 sec)
            Level 4: HARDWARE_DEVICES
                shoulder_servo                  hitCount: 10   (1 sec)
                elbow_servo                     hitCount: 10   (1 sec)
```

#### Component Type Definitions

- **CREATURE_ASSEMBLY**: Contains subsystems (creature root level)
- **SUBSYSTEM_ASSEMBLY**: Contains hardware components + optionally system components
- **HARDWARE_DEVICE**: Controls physical device (leaf level)

## JSON Structure Requirements

### Required Fields for All Components

```json
{
  "relative_filename": "config/components/example.json",
  "version": "3.0.0",
  "author": "config/author.json",
  "name": "example_component",
  "type": "HARDWARE_DEVICE",
  "timing": {
    "hitCount": 10,
    "description": "Update every 1 second"
  }
}
```

### Component Containment

Components are contained within other components via the `"components"` array. A single component can be referenced in multiple containment arrays across different container components.

```json
{
  "name": "container_component",
  "type": "SUBSYSTEM_ASSEMBLY",
  "components": [
    "config/components/child1.json",
    "config/components/child2.json",
    "config/components/child3.json"
  ]
}
```

### Controller Assignment (Subsystem Boundaries)

```json
{
  "name": "goblin_head",
  "type": "SUBSYSTEM_ASSEMBLY",
  "controller": "ESP32_S3_DEVKITC_1",
  "components": [
    "config/components/positioned/goblin_left_eye.json",
    "config/components/positioned/goblin_right_eye.json"
  ]
}
```

#### Controller Assignment Rules

1. **Controller Detection**: When recursive descent encounters a `controller` keyword, that component defines a subsystem boundary
2. **Membership Inheritance**: All components contained within a controller belong to that subsystem
3. **Single Membership**: Each component belongs to exactly ONE subsystem (no splitting across controllers)
4. **Boundary Enforcement**: Components cannot span multiple controllers
5. **Explicit Controllers**: Root components must have explicit controller (no default inheritance)

## Shape Assembly Integration

### Assembly Shapes Reference Components

Each assembly component defines a `shape_assembly` that provides the structural framework for its child components:

**Creature Level (goblin_full)**:

- Shape: `goblin_full_skeleton.scad` - Complete body frame
- Mounts: Subsystem attachment points (head socket, shoulder joints, hip joints)

**Subsystem Level (goblin_head)**:

- Shape: `goblin_skull.scad` - Skull frame with mounting rings
- Mounts: Component attachment points (26mm eye rings, 22x17mm nose bracket)

**Hardware Level (left_eye)**:

- Shape: `display_basic_mount.scad` + `goblin_eye_shells.scad`
- Mounts: Device-specific bracket + decorative shell

### Shape Assembly Structure

```json
{
  "shape_assembly": {
    "type": "SKULL_FRAME",
    "scad_file": "assets/shapes/scad/skull_generators/goblin_skull.scad",
    "description": "Anatomical skull frame with component mounting rings",
    "mounting_points": {
      "eye_left": "26mm_ring_at_[-26.67,17.78,-8.89]",
      "eye_right": "26mm_ring_at_[26.67,17.78,-8.89]",
      "nose": "22x17mm_bracket_at_[0,0,6.35]",
      "mouth": "26mm_ring_at_[0,-26.67,0]"
    }
  }
}
```

## Code Generation Architecture

### Code Generation Architecture: Aggregation and Dynamic Headers

The code generation process combines two strategies: aggregating source files and dynamically generating headers.

1. **Component Source Aggregation (`.cpp` files)**:
    - The `generate_tables.py` script reads all required `.src` files (the permanent source code for components) from the `config/components/` directory.
    - It concatenates their contents into a single, large `p32_component_functions.cpp` file for each subsystem. This reduces the number of compilation units.
    - **Permanent Source**: `config/components/{component_name}.src`
    - **Generated Aggregate**: `src/subsystems/{subsystem}/p32_component_functions.cpp`

2. **Dynamic Header and Struct Generation (`.hpp` file)**:
    - The `p32_component_functions.hpp` file is **not** an aggregation of `.hdr` files.
    - Instead, it is dynamically generated by `generate_tables.py` during the JSON tree walk.
    - This generated header contains:
        - **Function Prototypes**: `init()` and `act()` function declarations for every component in the subsystem.
        - **Dynamic C-Style Structs**: For each component, a C-style `struct` is generated to mirror the keyword hierarchy of its JSON configuration. This provides a compile-time mechanism for components to access their configuration parameters.
    - **Generated Header**: `include/subsystems/{subsystem}/p32_component_functions.hpp`

### Per-Subsystem Dispatch Tables (Many-to-One)

All components assigned to a subsystem contribute to that subsystem's shared dispatch table files:

```text
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

Each subsystem's dispatch table contains function pointers for its components. **Crucially, these tables intentionally include duplicate entries.** For every component reference the parser encounters during its recursive JSON tree walk, it adds a corresponding entry to the `initTable` and `actTable`. This allows a single component's logic to be instantiated and executed multiple times.

```cpp
// goblin_head_dispatch_tables.cpp
init_function_t head_init_table[] = {
    goblin_left_eye_init,    // from head subsystem
    goblin_right_eye_init,   // from head subsystem
    goblin_eye_init,         // Referenced by left_eye
    goblin_eye_init,         // Referenced by right_eye (duplicate is intentional)
    goblin_nose_init,        // from head subsystem
    goblin_ear_left_init     // from head subsystem
};

act_function_t head_act_table[] = {
    goblin_left_eye_act,     // from head subsystem
    goblin_right_eye_act,    // from head subsystem
    goblin_eye_act,          // Referenced by left_eye
    goblin_eye_act,          // Referenced by right_eye (duplicate is intentional)
    goblin_nose_act,         // from head subsystem
    goblin_ear_left_act      // from head subsystem
};

uint32_t head_hitcount_table[] = {
    5,    // goblin_left_eye hitCount
    5,    // goblin_right_eye hitCount
    5,    // goblin_eye hitCount
    5,    // goblin_eye hitCount
    100,  // goblin_nose hitCount
    10    // goblin_ear_left hitCount
};
```

## Component Code Generation Patterns

### Assembly Component Pattern

```c
// src/components/goblin_head.c
#include "p32_component_config.h"

#ifdef ENABLE_GOBLIN_COMPONENTS

// Child component declarations (from JSON components)
extern esp_err_t goblin_eye_left_init(void);
extern esp_err_t goblin_eye_right_init(void);
extern esp_err_t goblin_nose_init(void);
extern esp_err_t goblin_mouth_init(void);

extern void goblin_eye_left_act(uint32_t loopCount);
extern void goblin_eye_right_act(uint32_t loopCount);
extern void goblin_nose_act(uint32_t loopCount);
extern void goblin_mouth_act(uint32_t loopCount);

// Assembly component initialization
esp_err_t goblin_head_init(void) {
    ESP_LOGI(TAG, "Initializing goblin head subsystem...");

    // Initialize all child components
    goblin_eye_left_init();
    goblin_eye_right_init();
    goblin_nose_init();
    goblin_mouth_init();

    // Load shape assembly: goblin_skull.scad defines mounting framework
    ESP_LOGI(TAG, "Shape assembly: goblin_skull.scad");
    ESP_LOGI(TAG, "Mounting points: 2x eyes, 1x nose, 1x mouth");

    return ESP_OK;
}

// Assembly component action (coordinates children)
void goblin_head_act(uint32_t loopCount) {
    // Head-level coordination logic
    if (loopCount % 25 == 0) {
        // Synchronize facial expressions across eyes and mouth
        // Apply mood-driven behavior to all facial components
        // Coordinate sensor data from nose to eye reactions

        ESP_LOGD(TAG, "Head subsystem coordination tick");
    }
}

#endif // ENABLE_GOBLIN_COMPONENTS
```

### Hardware Component Pattern

```c
// src/components/goblin_left_eye.c
#include "p32_component_config.h"

#ifdef ENABLE_GOBLIN_COMPONENTS

// Hardware component initialization
esp_err_t goblin_left_eye_init(void) {
    ESP_LOGI(TAG, "Initializing left eye display...");

    // Initialize GC9A01 display hardware
    // Configure SPI interface
    // Load eye expression assets

    return ESP_OK;
}

// Hardware component action
void goblin_left_eye_act(uint32_t loopCount) {
    // Update display every 5th loop (200 FPS  40 FPS)
    if (loopCount % 5 == 0) {
        // Render current eye expression
        // Update based on mood/emotion state
        // Handle blink animations
    }
}

#endif // ENABLE_GOBLIN_COMPONENTS
```

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
2. **Components** must exist and be valid JSON
3. **Subsystem boundaries** must not create circular dependencies
4. **Component membership** must be unambiguous (single controller per component)

### Code Generation Validation

1. **Dispatch tables** must contain all subsystem components
2. **Function names** must match name patterns
3. **HitCount arrays** must parallel dispatch table order
4. **Include paths** must resolve correctly per subsystem

## Migration from Legacy Keywords

### Keyword Standardization

All legacy containment keywords are converted to `"components"`:

| Legacy Keyword | Status | Replacement |
|----------------|--------|-------------|
| `contained_components` |  Removed | `"components"` |
| `includes_components` |  Removed | `"components"` |
| `subsystem_components` |  Removed | `"components"` |
| `positioned_components` |  Removed | `"components"` |
| `family_level_components` |  Removed | `"components"` |
| `components` |  Standard | `"components"` |

### Conversion Examples

```json
// BEFORE (multiple legacy keywords)
{
  "contained_components": ["child1.json"],
  "includes_components": ["dep1.json"],
  "subsystem_components": ["sub1.json"],
  "positioned_components": ["pos1.json"]
}

// AFTER (unified keyword)
{
  "components": [
    "child1.json",
    "dep1.json",
    "sub1.json",
    "pos1.json"
  ]
}
```

## File Organization

### Directory Structure

```text
config/
 bots/
    goblin_full.json              # Creature definition (root)
 subsystems/
    goblin_head.json             # Subsystem specs
    goblin_torso.json            # Subsystem specs
    goblin_arm_left.json         # Subsystem specs
 components/
     positioned/                  # Hardware components
        goblin_left_eye.json
        goblin_right_eye.json
     hardware/                    # Device definitions
         gc9a01_display.json

src/
 components/                      # Generated per-component files
    goblin_left_eye.cpp
    goblin_right_eye.cpp
 subsystems/                      # Generated per-subsystem files
     goblin_head/
        goblin_head_dispatch_tables.cpp
        goblin_head_component_functions.cpp
     goblin_torso/
         goblin_torso_dispatch_tables.cpp
         goblin_torso_component_functions.cpp

include/
 components/                      # Generated per-component headers
    goblin_left_eye.hpp
    goblin_right_eye.hpp
 subsystems/                      # Generated per-subsystem headers
     goblin_head/
        goblin_head_dispatch_tables.hpp
        goblin_head_component_functions.hpp
     goblin_torso/
         goblin_torso_dispatch_tables.hpp
         goblin_torso_component_functions.hpp
```

---

**Version**: 1.0
**Last Updated**: 2025-01-27
**Status**: Authoritative JSON Construction Specification
