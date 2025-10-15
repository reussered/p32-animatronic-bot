# Hierarchical Component Composition Architecture

## CRITICAL PRINCIPLE: Assembly Components

**A component can contain other components.**

The P32 system uses **recursive component composition** where:
- A **creature** (goblin_full) is a component that contains **subsystem components**
- A **subsystem** (goblin_head) is a component that contains **hardware components**
- A **hardware component** (left_eye) is a leaf node that controls physical hardware

This means: **A single JSON file (goblin_full.json) defines the entire creature.**

## Component Hierarchy Levels

```
┌─────────────────────────────────────────────────────────────────┐
│                    LEVEL 0: CREATURE                            │
│                 (Complete Robot Assembly)                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  File: config/bots/goblin_full.json                            │
│                                                                 │
│  {                                                              │
│    "bot_type": "GOBLINFULL",                                   │
│    "bot_id": "goblin_full_v1",                                 │
│    "description": "Complete goblin animatronic robot",         │
│                                                                 │
│    "subsystem_components": [                                    │
│      "config/subsystems/goblin_head.json",     ◄─── Level 1    │
│      "config/subsystems/goblin_torso.json",    ◄─── Level 1    │
│      "config/subsystems/goblin_arm_left.json", ◄─── Level 1    │
│      "config/subsystems/goblin_arm_right.json" ◄─── Level 1    │
│    ]                                                            │
│  }                                                              │
│                                                                 │
│  Component Type: ASSEMBLY                                       │
│  Component Code: goblin_full_init(), goblin_full_act()         │
│  Responsibility: Coordinate subsystems, overall behavior        │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
                                │
            ┌───────────────────┼───────────────────┐
            ▼                   ▼                   ▼
┌─────────────────────┐  ┌─────────────────┐  ┌──────────────────┐
│  LEVEL 1: SUBSYSTEM │  │  LEVEL 1:       │  │  LEVEL 1:        │
│  (Head Assembly)    │  │  SUBSYSTEM      │  │  SUBSYSTEM       │
├─────────────────────┤  │  (Arm Assembly) │  │  (Torso)         │
│                     │  ├─────────────────┤  ├──────────────────┤
│ File: config/       │  │ File: config/   │  │ File: config/    │
│ subsystems/         │  │ subsystems/     │  │ subsystems/      │
│ goblin_head.json    │  │ goblin_arm_     │  │ goblin_torso.    │
│                     │  │ left.json       │  │ json             │
│ {                   │  │                 │  │                  │
│   "component_name": │  │ {               │  │ {                │
│     "goblin_head",  │  │   "component_   │  │   "component_    │
│   "description":    │  │     name":      │  │     name":       │
│     "Head subsystem"│  │     "arm_left", │  │     "torso",     │
│                     │  │   "positioned_  │  │   "positioned_   │
│   "positioned_      │  │     components":│  │     components": │
│     components": [  │  │     [           │  │     [            │
│     "goblineye_     │  │     "shoulder_  │  │     "spine_      │
│       left.json",   │  │       servo",   │  │       servo",    │
│     "goblineye_     │  │     "elbow_     │  │     "waist_      │
│       right.json",  │  │       servo"    │  │       servo"     │
│     "goblin_nose    │  │     ]           │  │     ]            │
│       .json",       │  │   "shape_       │  │   "shape_        │
│     "goblin_mouth   │  │     assembly":  │  │     assembly":   │
│       .json"        │  │     "arm_left_  │  │     "torso_      │
│   ],                │  │       frame.    │  │       frame.     │
│   "shape_assembly": │  │       scad"     │  │       scad"      │
│     "goblin_skull.  │  │ }               │  │ }                │
│       scad"         │  │                 │  │                  │
│ }                   │  │ Component Code: │  │ Component Code:  │
│                     │  │ arm_left_init() │  │ torso_init()     │
│ Component Code:     │  │ arm_left_act()  │  │ torso_act()      │
│ goblin_head_init()  │  │                 │  │                  │
│ goblin_head_act()   │  │ Shape: ARM      │  │ Shape: TORSO     │
│                     │  │ structural      │  │ frame + servo    │
│ Shape: SKULL        │  │ frame + servo   │  │ mounts           │
│ structural frame +  │  │ mounts          │  │                  │
│ mounting rings      │  │                 │  │                  │
└─────────────────────┘  └─────────────────┘  └──────────────────┘
         │
         ▼
┌──────────────────────────────────────────────────────────────────┐
│              LEVEL 2: HARDWARE COMPONENTS                        │
│           (Individual Devices with Shapes)                       │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│  File: config/components/positioned/goblineye_left.json         │
│                                                                  │
│  {                                                               │
│    "component_name": "left_eye",                                │
│    "description": "Left eye GC9A01 display",                    │
│    "timing": { "hitCount": 5 },                                 │
│    "hardware": "config/components/hardware/gc9a01_display.json",│
│    "interface": "config/components/interfaces/spi_device_1.json"│
│    "position": {                                                 │
│      "coordinate_system": "skull_3d",                           │
│      "reference_point": "nose_center",                          │
│      "x": "-26.67 mm", "y": "17.78 mm", "z": "-8.89 mm"        │
│    },                                                            │
│    "shape_component": {                                          │
│      "basic_mount": "display_basic_mount.scad",                 │
│      "decorative_shell": "goblin_eye_shells.scad"               │
│    }                                                             │
│  }                                                               │
│                                                                  │
│  Component Code: goblin_eye_left_init(), goblin_eye_left_act()  │
│  Shape: DISPLAY mount (basic) + EYE shell (goblin-specific)     │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
```

## Component Type Classification

### Assembly Components (Contain Other Components)

**Characteristics**:
- Define `subsystem_components` or `positioned_components` arrays
- Coordinate child component execution
- Define structural shape assemblies (skulls, frames)
- Have init() that calls child init() functions
- Have act() that coordinates child act() functions

**Examples**:
- `goblin_full` - Complete robot (Level 0)
- `goblin_head` - Head subsystem (Level 1)
- `goblin_arm_left` - Arm subsystem (Level 1)
- `goblin_torso` - Torso subsystem (Level 1)

### Leaf Components (Control Hardware)

**Characteristics**:
- Define specific hardware device and interface
- Have spatial position coordinates
- Define shapes for mounting and decoration
- Have init() that initializes hardware
- Have act() that directly controls hardware

**Examples**:
- `left_eye` - GC9A01 display
- `nose_sensor` - HC-SR04 ultrasonic sensor
- `shoulder_servo` - MG996R servo motor

## JSON Structure Pattern

### Level 0: Creature (goblin_full.json)

```json
{
  "relative_filename": "config/bots/goblin_full.json",
  "version": "3.0.0",
  "author": "config/author.json",
  
  "bot_type": "GOBLINFULL",
  "bot_id": "goblin_full_v1",
  "description": "Complete goblin animatronic robot - full body",
  
  "family_template": "config/bots/bot_families/fantasy/goblin_family.json",
  
  "coordinate_system": "humanoid_3d",
  "reference_point": "spine_base",
  "units": "INCHES",
  
  "component_name": "goblin_full",
  "component_type": "ASSEMBLY",
  "timing": {
    "hitCount": 100,
    "description": "Creature-level coordination every 10 seconds"
  },
  
  "subsystem_components": [
    "config/subsystems/goblin_head.json",
    "config/subsystems/goblin_torso.json",
    "config/subsystems/goblin_arm_left.json",
    "config/subsystems/goblin_arm_right.json",
    "config/subsystems/goblin_leg_left.json",
    "config/subsystems/goblin_leg_right.json"
  ],
  
  "shape_assembly": {
    "type": "CREATURE_BODY",
    "scad_file": "assets/shapes/scad/creatures/goblin/goblin_full_skeleton.scad",
    "description": "Complete skeletal frame connecting all subsystems"
  },
  
  "mass_properties": {
    "total_weight_kg": "5.2",
    "center_of_gravity": [0, 0, 450],
    "balance_point": "spine_base"
  }
}
```

### Level 1: Subsystem (goblin_head.json)

```json
{
  "relative_filename": "config/subsystems/goblin_head.json",
  "version": "3.0.0",
  "author": "config/author.json",
  
  "subsystem_type": "HEAD",
  "subsystem_id": "goblin_head_v1",
  "description": "Goblin head subsystem with expressions and sensors",
  
  "controller_hardware": "ESP32_S3_DEVKITC_1",
  "mesh_role": "MASTER_NODE",
  
  "coordinate_system": "skull_3d",
  "reference_point": "nose_center",
  "units": "MILLIMETERS",
  
  "component_name": "goblin_head",
  "component_type": "SUBSYSTEM_ASSEMBLY",
  "timing": {
    "hitCount": 25,
    "description": "Head coordination every 2.5 seconds"
  },
  
  "positioned_components": [
    "config/components/positioned/goblineye_left.json",
    "config/components/positioned/goblineye_right.json",
    "config/components/positioned/goblin_nose.json",
    "config/components/positioned/goblin_mouth.json",
    "config/components/positioned/goblin_speaker.json",
    "config/components/positioned/goblin_ear_left.json",
    "config/components/positioned/goblin_ear_right.json"
  ],
  
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
  },
  
  "articulation": {
    "neck_servos": [
      "config/components/positioned/neck_pan_servo.json",
      "config/components/positioned/neck_tilt_servo.json"
    ],
    "degrees_of_freedom": 2
  }
}
```

### Level 2: Hardware Component (goblineye_left.json)

```json
{
  "relative_filename": "config/components/positioned/goblineye_left.json",
  "version": "3.0.0",
  "author": "config/author.json",
  
  "component_name": "left_eye",
  "component_type": "HARDWARE_DEVICE",
  "description": "Left eye GC9A01 round display for expressions",
  
  "timing": {
    "hitCount": 5,
    "description": "Update display every 500ms"
  },
  
  "hardware": "config/components/hardware/gc9a01_display.json",
  "interface": "config/components/interfaces/spi_device_1.json",
  
  "position": {
    "coordinate_system": "skull_3d",
    "reference_point": "nose_center",
    "x": "-26.67 mm",
    "y": "17.78 mm",
    "z": "-8.89 mm",
    "rotation": {"yaw": 0, "pitch": 0, "roll": 0}
  },
  
  "shape_component": {
    "mounting_architecture": "two_tier",
    "basic_mount": {
      "scad_file": "assets/shapes/scad/basic_mounts/display_basic_mount.scad",
      "stl_file": "assets/shapes/stl/basic_mounts/display_basic_mount.stl",
      "description": "Universal GC9A01 display bracket"
    },
    "decorative_shell": {
      "scad_file": "assets/shapes/scad/bot_shells/goblin_eye_shells.scad",
      "stl_file": "assets/shapes/stl/bot_shells/goblin_eye_shells.stl",
      "description": "Craggy goblin eye sockets with warty eyelids",
      "color_palette": "config/bots/bot_families/fantasy/goblin_family.json"
    }
  },
  
  "mounting": {
    "parent_subsystem": "goblin_head",
    "attachment_type": "26mm_ring",
    "attachment_point": "left_eye_socket"
  }
}
```

## Component Code Generation

### Assembly Component Pattern

```c
// src/components/goblin_head.c
#include "p32_component_config.h"

#ifdef ENABLE_GOBLIN_COMPONENTS

// Child component declarations (from JSON positioned_components)
extern esp_err_t goblin_eye_left_init(void);
extern esp_err_t goblin_eye_right_init(void);
extern esp_err_t goblin_nose_init(void);
extern esp_err_t goblin_mouth_init(void);
extern esp_err_t goblin_speaker_init(void);

extern void goblin_eye_left_act(uint32_t loopCount);
extern void goblin_eye_right_act(uint32_t loopCount);
extern void goblin_nose_act(uint32_t loopCount);
extern void goblin_mouth_act(uint32_t loopCount);
extern void goblin_speaker_act(uint32_t loopCount);

// Assembly component initialization
esp_err_t goblin_head_init(void) {
    ESP_LOGI(TAG, "Initializing goblin head subsystem...");
    
    // Initialize all child components
    goblin_eye_left_init();
    goblin_eye_right_init();
    goblin_nose_init();
    goblin_mouth_init();
    goblin_speaker_init();
    
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

### Creature-Level Assembly Pattern

```c
// src/components/goblin_full.c
#include "p32_component_config.h"

#ifdef ENABLE_GOBLIN_COMPONENTS

// Subsystem component declarations (from JSON subsystem_components)
extern esp_err_t goblin_head_init(void);
extern esp_err_t goblin_torso_init(void);
extern esp_err_t goblin_arm_left_init(void);
extern esp_err_t goblin_arm_right_init(void);

extern void goblin_head_act(uint32_t loopCount);
extern void goblin_torso_act(uint32_t loopCount);
extern void goblin_arm_left_act(uint32_t loopCount);
extern void goblin_arm_right_act(uint32_t loopCount);

// Creature-level initialization
esp_err_t goblin_full_init(void) {
    ESP_LOGI(TAG, "Initializing complete goblin creature...");
    
    // Initialize all subsystems
    goblin_head_init();
    goblin_torso_init();
    goblin_arm_left_init();
    goblin_arm_right_init();
    
    // Load shape assembly: goblin_full_skeleton.scad defines body frame
    ESP_LOGI(TAG, "Shape assembly: goblin_full_skeleton.scad");
    ESP_LOGI(TAG, "Subsystems: head, torso, 2x arms, 2x legs");
    
    return ESP_OK;
}

// Creature-level action (coordinates subsystems)
void goblin_full_act(uint32_t loopCount) {
    // Creature-level behavior coordination
    if (loopCount % 100 == 0) {
        // High-level behavior decisions affecting all subsystems
        // Balance and posture control
        // Coordinated movements (walking, gesturing, etc.)
        
        ESP_LOGD(TAG, "Creature-level coordination tick");
    }
}

#endif // ENABLE_GOBLIN_COMPONENTS
```

## Component Table Generation

### Updated Generator Logic

```python
# tools/generate_component_tables.py

def generate_component_tables(creature_json_path):
    """Generate component tables from hierarchical JSON structure"""
    
    creature = load_json(creature_json_path)
    all_components = []
    
    # Level 0: Creature component (e.g., goblin_full)
    creature_component = {
        "name": creature["component_name"],
        "hitCount": creature["timing"]["hitCount"],
        "type": "ASSEMBLY",
        "level": 0
    }
    all_components.append(creature_component)
    
    # Level 1: System components (always present)
    system_components = get_system_components()
    all_components.extend(system_components)
    
    # Level 2: Family components (from family_template)
    if "family_template" in creature:
        family = load_json(creature["family_template"])
        family_components = []
        for component_path in family.get("default_components", []):
            component = load_json(component_path)
            family_components.append({
                "name": component["component_name"],
                "hitCount": component["timing"]["hitCount"],
                "type": "FUNCTIONAL",
                "level": 2
            })
        all_components.extend(family_components)
    
    # Level 3: Subsystem assemblies (recursive)
    for subsystem_path in creature.get("subsystem_components", []):
        subsystem = load_json(subsystem_path)
        
        # Add subsystem assembly component
        subsystem_component = {
            "name": subsystem["component_name"],
            "hitCount": subsystem["timing"]["hitCount"],
            "type": "SUBSYSTEM_ASSEMBLY",
            "level": 3
        }
        all_components.append(subsystem_component)
        
        # Level 4: Hardware components within subsystem
        for hw_component_path in subsystem.get("positioned_components", []):
            hw_component = load_json(hw_component_path)
            hardware_component = {
                "name": hw_component["component_name"],
                "hitCount": hw_component["timing"]["hitCount"],
                "type": "HARDWARE_DEVICE",
                "level": 4,
                "parent": subsystem["component_name"]
            }
            all_components.append(hardware_component)
    
    # Generate initTable and actTable
    generate_init_table(all_components)
    generate_act_table(all_components)
    
    return all_components
```

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

### Shape Generation Command

```powershell
# Generate shapes for entire creature hierarchy
.\tools\generate-creature-shapes.ps1 -CreatureConfig "config/bots/goblin_full.json"

# Process:
# 1. Parse goblin_full.json → find subsystem_components
# 2. Parse each subsystem (goblin_head.json) → find positioned_components  
# 3. Generate assembly shape (goblin_skull.scad)
# 4. Generate component shapes (display_basic_mount.scad, goblin_eye_shells.scad)
# 5. Compile all .scad → .stl files
```

## Migration from Flat to Hierarchical

### Current Structure (Flat)

```
config/bots/goblin_full.json
├─ positioned_components: [
│    left_eye.json,
│    right_eye.json,
│    nose.json,
│    mouth.json,
│    speaker.json
│  ]
```

### New Structure (Hierarchical)

```
config/bots/goblin_full.json                    ◄─── CREATURE
├─ subsystem_components: [
│    config/subsystems/goblin_head.json         ◄─── SUBSYSTEM
│    ├─ positioned_components: [
│    │    left_eye.json,                        ◄─── HARDWARE
│    │    right_eye.json,                       ◄─── HARDWARE
│    │    nose.json,                            ◄─── HARDWARE
│    │    mouth.json                            ◄─── HARDWARE
│    │  ]
│    │
│    config/subsystems/goblin_torso.json        ◄─── SUBSYSTEM
│    ├─ positioned_components: [
│    │    spine_servo.json,                     ◄─── HARDWARE
│    │    waist_servo.json                      ◄─── HARDWARE
│    │  ]
│  ]
```

## Strategic Component Distribution

### System-Level Components Attached to Torso

**CRITICAL DESIGN DECISION**: System-level components (WiFi, ESP-NOW mesh, telemetry) are attached to the **torso subsystem**, NOT the head.

**Rationale**:
- **Head subsystem** has high computational load (2x displays @ 60 FPS, audio processing, sensor filtering)
- **Torso subsystem** has lower real-time demands (3 servos, coordination logic)
- **Torso ESP32-S3** can dedicate processing power to system-level tasks
- **Head ESP32-S3** focuses exclusively on real-time display/audio rendering

### Subsystem Component Allocation

```
goblin_torso.json (ESP32-S3 Master):
├─ system_level_components:
│  ├─ system_core.json          (hitCount: 100)
│  ├─ network_monitor.json      (hitCount: 50)
│  ├─ esp_now_mesh.json         (hitCount: 10)  ◄─── Mesh master
│  ├─ wifi_station.json         (hitCount: 50)  ◄─── WiFi management
│  ├─ bluetooth_central.json    (hitCount: 75)
│  ├─ telemetry_hub.json        (hitCount: 75)  ◄─── MQTT/telemetry
│  ├─ watchdog.json             (hitCount: 500)
│  └─ power_monitor.json        (hitCount: 200)
│
└─ positioned_components:
   ├─ spine_flexion_servo.json  (hitCount: 20)
   ├─ waist_rotation_servo.json (hitCount: 20)
   └─ torso_speaker.json        (hitCount: 15)

goblin_head.json (ESP32-S3 Slave - High Priority):
├─ NO system_level_components   ◄─── Clean separation
│
└─ positioned_components:
   ├─ left_eye.json             (hitCount: 5)   ◄─── 60 FPS capable
   ├─ right_eye.json            (hitCount: 5)   ◄─── 60 FPS capable
   ├─ nose_sensor.json          (hitCount: 15)
   ├─ mouth_display.json        (hitCount: 3)
   ├─ speaker.json              (hitCount: 7)
   └─ neck_servos.json          (hitCount: 10)

goblin_arm_left.json (ESP32-C3 Slave):
├─ NO system_level_components   ◄─── Lightweight slave
│
└─ positioned_components:
   ├─ shoulder_servos (3x)      (hitCount: 10)
   ├─ elbow_servo               (hitCount: 10)
   └─ wrist_servos (2x)         (hitCount: 10)
```

### Processing Load Analysis

**Torso (ESP32-S3 Master)**:
- System tasks: ~25% CPU (WiFi, mesh, telemetry)
- Servo control: ~10% CPU (3 servos @ 50 Hz)
- Coordination: ~15% CPU (behavior planning)
- **Total**: ~50% CPU utilization
- **Available headroom**: 50% for expansion

**Head (ESP32-S3 Slave)**:
- Display rendering: ~40% CPU (2x GC9A01 @ 60 FPS)
- Audio processing: ~20% CPU (I2S mixing)
- Sensor polling: ~10% CPU (HC-SR04 filtering)
- Servo control: ~5% CPU (2 neck servos)
- **Total**: ~75% CPU utilization
- **No room for system tasks** - Correct decision to move to torso!

### Mesh Communication Pattern

```
┌──────────────────────────────────────────────────────┐
│         TORSO (ESP32-S3 Master Coordinator)          │
│  • WiFi Station (internet connectivity)              │
│  • ESP-NOW Mesh Master (slave coordination)          │
│  • Telemetry Hub (MQTT publishing)                   │
│  • System Core (watchdog, power, health)             │
└──────────────────────────────────────────────────────┘
                        │
      ESP-NOW Mesh Network (2.4 GHz, Channel 6)
                        │
      ┌─────────────────┼─────────────────┬───────────┐
      ▼                 ▼                 ▼           ▼
┌──────────┐      ┌──────────┐      ┌─────────┐  ┌─────────┐
│   HEAD   │      │ ARM_LEFT │      │ARM_RIGHT│  │ LEG_L/R │
│ ESP32-S3 │      │ ESP32-C3 │      │ ESP32-C3│  │ ESP32-C3│
│  SLAVE   │      │  SLAVE   │      │  SLAVE  │  │  SLAVE  │
│ Priority │      │ Priority │      │ Priority│  │ Priority│
│   HIGH   │      │  MEDIUM  │      │  MEDIUM │  │  MEDIUM │
└──────────┘      └──────────┘      └─────────┘  └─────────┘
```

## Summary

### Key Principles

1. **Single JSON defines entire creature** - `goblin_full.json` is the root
2. **Components contain components** - Recursive composition via `subsystem_components` and `positioned_components`
3. **Shapes reference assemblies** - Each level defines structural frameworks
4. **Init/Act propagates down** - Parent init() calls child init(), parent act() coordinates children
5. **Strategic component distribution** - System-level components attached to torso (master coordinator)
6. **Processing load balanced** - Head focuses on real-time rendering, torso handles system tasks
7. **Three component types**:
   - **ASSEMBLY** - Contains subsystems (goblin_full)
   - **SUBSYSTEM_ASSEMBLY** - Contains hardware + optionally system components (goblin_head, goblin_torso)
   - **HARDWARE_DEVICE** - Controls physical device (left_eye)

### Component Hierarchy Summary

```
Level 0: CREATURE (goblin_full)           hitCount: 100  (10 sec)
  ├─ Level 1: SYSTEM (system_core)        hitCount: 100  (10 sec)
  ├─ Level 2: FAMILY (goblin_personality) hitCount: 25   (2.5 sec)
  └─ Level 3: SUBSYSTEMS
       ├─ goblin_head                     hitCount: 25   (2.5 sec)
       │   └─ Level 4: HARDWARE
       │       ├─ left_eye                hitCount: 5    (500ms)
       │       ├─ right_eye               hitCount: 5    (500ms)
       │       ├─ nose                    hitCount: 15   (1.5 sec)
       │       └─ mouth                   hitCount: 3    (300ms)
       │
       ├─ goblin_torso                    hitCount: 50   (5 sec)
       │   └─ Level 4: HARDWARE
       │       ├─ spine_servo             hitCount: 20   (2 sec)
       │       └─ waist_servo             hitCount: 20   (2 sec)
       │
       └─ goblin_arm_left                 hitCount: 30   (3 sec)
           └─ Level 4: HARDWARE
               ├─ shoulder_servo          hitCount: 10   (1 sec)
               └─ elbow_servo             hitCount: 10   (1 sec)
```

**Result**: One JSON file → Complete creature with all subsystems and hardware defined.
