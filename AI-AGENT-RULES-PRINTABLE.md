# P32 ANIMATRONIC BOT - AI AGENT RULES
## PRINTABLE CONSOLIDATED REFERENCE

**Generated:** October 24, 2025  
**Purpose:** Complete rule consolidation for review and validation  
**Source:** .github/AI-AGENT-RULES.md

---

## 🚨 CRITICAL: EMBEDDED SYSTEMS DEVELOPMENT CONTEXT

**THIS IS NOT WEB DEVELOPMENT - THIS IS EMBEDDED MICROCONTROLLER PROGRAMMING**

- **Target Hardware**: ESP32-S3 microcontroller (512KB RAM, 8MB Flash)
- **Framework**: ESP-IDF native APIs (NOT Arduino, NOT web frameworks)
- **Memory Constraints**: Resource-constrained embedded environment
- **Encoding**: ASCII-only for toolchain compatibility (NO UTF-8, NO Unicode)
- **Dependencies**: Hardware drivers, FreeRTOS, SPI/I2S protocols
- **Toolchain**: PlatformIO + ESP-IDF, GCC cross-compiler
- **Debugging**: Serial console, hardware debugger (NOT browser dev tools)
- **Performance**: Real-time constraints, interrupt-driven (NOT async/await patterns)

**NEVER THINK LIKE A WEB DEVELOPER:**
- NO npm packages, node_modules, or JavaScript patterns
- NO web servers, REST APIs, or HTTP frameworks
- NO browser compatibility concerns or DOM manipulation
- NO database ORMs or cloud service integrations
- NO UTF-8 encoding assumptions or web character sets

**ALWAYS THINK LIKE AN EMBEDDED DEVELOPER:**
- Memory usage in bytes, not megabytes
- CPU cycles matter, power consumption matters
- Hardware registers, GPIO pins, SPI buses
- Interrupt handlers, task priorities, timing constraints
- Component datasheets, electrical specifications
- ASCII encoding, fixed-point math, lookup tables

---

## IRONCLAD AI AGENT RULES

### BASE LEVEL RULE: IMMEDIATE REPORTING OF BLOCKERS
If any project rule, technical limitation, or system constraint prevents the agent from proceeding with the current task, the agent must immediately report the exact reason to the user. The agent must not pause, stop, or silently fail without providing a clear explanation of the blocker.

### CORE DEVELOPMENT RULES
- Always read and understand any file before changing it
- Never change file contents or filename unless explicitly part of current task
- Ask for permission before changing or deleting files unless obviously required
- Follow rules in .github/consistant_project_rules.md (but AI-AGENT-RULES.md takes precedence)

---

## RULE 15: GLOBAL SHARED MEMORY API - SOLE COMMUNICATION MECHANISM

**SharedMemory is the ONLY inter-subsystem communication mechanism:**

- **Single Communication Protocol**: SharedMemory class is the exclusive gateway for all inter-subsystem communication
- **Internal ESP-NOW Encapsulation**: SharedMemory contains a complete internal implementation of ESP-NOW bluetooth protocol
- **No External Protocol Access**: Components NEVER directly access ESP-NOW, Bluetooth, WiFi, or any other communication APIs
- **Component Isolation**: All components use only SharedMemory API for inter-subsystem coordination

### CRITICAL Architecture Understanding:

**`read()` is Local Memory Access (NO Network Traffic):**
- `read(string name)` returns pointer to class variable in local memory
- No memcpy or network synchronization unless variable doesn't exist
- If variable doesn't exist, it's created automatically using default constructor
- Components can call `read()` **as often as needed** (even every loop!) with ZERO network overhead

**`write()` Triggers Inter-Subsystem Synchronization:**
- `write(string name)` ensures shared memory in every subsystem has the same value
- `write()` internally uses ESP-NOW to broadcast changes to all other ESP32 controllers
- Only `write()` triggers network traffic between subsystems
- Binary transmission maintains efficiency

### Required Usage Pattern:
```cpp
// CORRECT: Fast local reads (NO network overhead)
Mood *mood = SharedMemory.read("g_Mood");  // Created automatically if doesn't exist
Environment *envir = SharedMemory.read("g_Envir");

// CORRECT: Direct memory modifications (local only)
mood->ANGER = 0; 
mood->CONTENTMENT = 100;
envir->distance_cm = new_measurement;

// CORRECT: Trigger synchronization (WITH network overhead)
SharedMemory.write("g_Mood");    // Broadcasts to all subsystems
SharedMemory.write("g_Envir");   // Only write when changes made
```

### Change Detection Pattern (Recommended):
```cpp
void component_act(void) {
    Environment *envir = SharedMemory.read("g_Envir");
    uint8_t new_distance = measure_distance();
    
    if (new_distance != envir->distance_cm) {
        envir->distance_cm = new_distance;
        SharedMemory.write("g_Envir");  // Only write if data changed
    }
}
```

### SharedMemory Implementation Rules:

**Case-Insensitive Names:**
- All SharedMemory variable names are case-insensitive
- `SharedMemory.read("g_Mood")` same as `SharedMemory.read("G_MOOD")` or `SharedMemory.read("g_mood")`
- Consistent naming convention recommended: use lowercase with underscores

**Fixed-Size Data Blocks:**
- SharedMemory data blocks NEVER change in size after creation
- If a data structure needs size changes, create NEW class with DIFFERENT name
- Prevents memory fragmentation and ensures reliable inter-subsystem synchronization
- Version control through name changes (e.g., `g_Envir_v2` if Environment class structure changes)

### Defined SharedMemory Classes:

**All SharedMemory classes are located in `/shared` directory.**

**1. Mood Class** (`"g_Mood"`) - `/shared/Mood.hpp`
- Contains 9 different emotions with strength values for each
- Emotions: ANGER, FEAR, HAPPINESS, SADNESS, CURIOSITY, AFFECTION, IRRITATION, CONTENTMENT, EXCITEMENT
- Used by personality components to modify creature emotional state
- Read by display/animation components to adjust visual expressions

**2. Environment Class** (`"g_Envir"`) - `/shared/Environment.hpp`
- Contains current surroundings and sensor data
- Variables include: distance_cm, temperature_c, humidity, sound_level_db, face_detected, target_confidence
- Updated by sensor components (nose, ears, cameras)
- Read by behavior and personality components for environmental reactions

**3. Personality Class** (`"g_Personality"`) - `/shared/Personality.hpp` - FUTURE
- Will contain creature personality configuration data
- Expected to grow as personality system develops
- Loaded from JSON personality files during initialization
- Contains thresholds, reaction strengths, and behavioral parameters

### Usage Examples:
```cpp
// Case-insensitive access (all equivalent)
Mood *mood1 = SharedMemory.read("g_Mood");
Mood *mood2 = SharedMemory.read("G_MOOD");  
Mood *mood3 = SharedMemory.read("g_mood");

// Environment data access
Environment *envir = SharedMemory.read("g_Envir");
if (envir->distance_cm < 30) {
    mood1->CURIOSITY += 10;
    SharedMemory.write("g_Mood");
}

// Future personality access
// Personality *personality = SharedMemory.read("g_Personality");
```

### Prohibited Communication Methods:
- Direct ESP-NOW API calls (esp_now_send, esp_now_register_recv_cb, etc.)
- Direct Bluetooth classic or BLE APIs
- Direct WiFi communication for inter-subsystem coordination
- Custom inter-ESP32 communication protocols
- File transfer or serial communication between subsystems

---

## RULE 16: DISPATCH TABLE GENERATION ARCHITECTURE

### CRITICAL: Four-File Auto-Generation Architecture

The validation script (`validate_json_improved.py`) MUST auto-generate exactly 4 files from JSON component configurations:

1. **`src/p32_dispatch_tables.cpp`** - Implementation with tables
2. **`include/p32_dispatch_tables.hpp`** - Header with declarations  
3. **`src/p32_component_functions.cpp`** - Component implementation aggregator
4. **`include/p32_component_functions.hpp`** - Component header aggregator

### Version Control Rules:
ALL four generated files MUST be in `.gitignore` since they are script-generated:
```gitignore
# Auto-generated dispatch tables - created by validate_json_improved.py
src/p32_dispatch_tables.cpp
include/p32_dispatch_tables.hpp
src/p32_component_functions.cpp
include/p32_component_functions.hpp
```

### Component Function Signature Rules:
- ALL component functions MUST follow the NO ARGUMENTS pattern:
  ```cpp
  esp_err_t component_name_init(void);
  void component_name_act(void);
  ```
- Components access shared state via `#include "p32_shared_state.h"`
- Function names MUST follow pattern: `{component_name}_{function_type}`
- All component names MUST be globally unique across the entire system

### Three Table Architecture:
Generate exactly 3 parallel dispatch tables with SAME indexing:
```cpp
init_func_t initTable[COMPONENT_TABLE_SIZE];    // Called once at startup
act_func_t actTable[COMPONENT_TABLE_SIZE];      // Called based on timing  
uint32_t hitCountTable[COMPONENT_TABLE_SIZE];   // Execution frequency
```

### Parallel Indexing Rule:
`initTable[i]`, `actTable[i]`, and `hitCountTable[i]` MUST refer to the SAME component.

### Component Hierarchy Processing:
- Process depth-first to create flat execution order
- Dispatch tables: NO deduplication (multiple entries allowed)
- Function definitions: WITH deduplication (single definition only)
- Hardware-only components (`"hardware_only": true`) do NOT generate init/act functions

### Integration Rules:
- Generated files MUST be included in build system
- `main.cpp` MUST include dispatch tables header
- Core loop: Initialize all components once, then execute based on timing

---

## RULE 17: JSON-DRIVEN PERSONALITY SYSTEM

### Data-Driven Personality Architecture:

**Personality module is entirely data-driven** - no hardcoded behavior logic!

- **JSON defines personality traits** - how strongly creature reacts to stimuli
- **C++ code is generic** - reads JSON config, applies formulas
- **Same code works for all creatures** - only JSON changes
- **Artists can tweak personalities** - no programmer required

### Personality JSON Structure Requirements:
```json
{
    "personality_name": "Creature Name",
    "personality_id": "CREATURE_ID",
    "mood_reactions": {
        "close_object": {
            "distance_threshold_cm": 30,
            "triggers": [
                {"mood": "CURIOSITY", "delta": 10},
                {"mood": "EXCITEMENT", "delta": 15}
            ]
        }
    },
    "mood_decay": {
        "decay_interval_seconds": 5,
        "decay_rates": [
            {"mood": "EXCITEMENT", "delta": -5}
        ]
    },
    "default_mood": {
        "ANGER": 0, "FEAR": 0, "HAPPINESS": 50,
        "CURIOSITY": 30, "AFFECTION": 20
    }
}
```

### Generic Personality Component Pattern:
- Load personality JSON from filesystem during init()
- Apply mood reactions based on environment triggers
- Use SharedMemory for reading Environment and writing Mood
- Support mood decay over time
- Enable different creatures to have distinct personalities using same code

### Required Mood Components:
ANGER, FEAR, HAPPINESS, SADNESS, CURIOSITY, AFFECTION, IRRITATION, CONTENTMENT, EXCITEMENT

### SharedMemory Integration:
```cpp
// Read environment and mood (no network overhead)
Environment *envir = SharedMemory.read("g_Envir");
Mood *mood = SharedMemory.read("g_Mood");

// Apply personality-driven changes
if (envir->distance_cm < threshold) {
    mood->CURIOSITY += personality_delta;
}

// Trigger synchronization (with network overhead)
SharedMemory.write("g_Mood");
```

---

## RULE 18: THREE-LEVEL COMPONENT ATTACHMENT ARCHITECTURE

### CRITICAL DESIGN PRINCIPLE:

**NOTHING executes unless it's a component with init() and act() functions.**

### Pure Component-Driven Architecture:
- ESP32 `app_main()` contains ONLY the component execution loop
- All functionality is implemented as components with `init()` and `act()` functions
- Components are attached at three hierarchical levels
- Core loop iterates through registered components calling their `act()` functions based on `hitCount` timing

### Three Component Levels:

**Level 1: System Components (Always Attached)**
- Core platform functionality required by all bots
- Always included in every build, lowest execution frequency
- Examples: `system_core`, `network_monitor`, `serial_console`, `power_monitor`, `watchdog_heartbeat`

**Level 2: Family Components (Shared Across Bot Family)**
- Behavior/personality shared across creature family (Goblin, Cat, Bear)
- Examples: `goblin_behavior`, `goblin_mood`, `goblin_personality`

**Level 3: Bot-Specific Components (Positioned Hardware)**
- Physical hardware components with spatial coordinates
- Examples: `goblin_left_eye`, `goblin_right_eye`, `goblin_nose`, `goblin_mouth`

### Component Registration Requirements:
- JSON configuration defines component hierarchy
- Code generation creates dispatch tables
- Compilation links all component implementations
- Runtime execution based on hitCount timing

---

## RULE 19: TWO-TIER MOUNTING ARCHITECTURE

### NEVER CONFUSE UNIVERSAL MOUNTS WITH CREATURE SHELLS

**TIER 1: Universal Hardware Mounts (IDENTICAL across ALL creatures)**
- `sensor_basic_mount.scad/.stl` - HC-SR04 ultrasonic (ALL creatures)
- `display_basic_mount.scad/.stl` - GC9A01 round LCD (ALL creatures)  
- `speaker_basic_mount.scad/.stl` - 40mm I2S speaker (ALL creatures)
- `servo_basic_mount.scad/.stl` - SG90, MG996R servos (ALL creatures)

### Universal Mount Characteristics:
- Precise component fit with exact hardware dimensions
- Standard M3 screw mounting, consistent hole patterns
- Built-in cable management and wire routing
- Utilitarian function over form, minimal aesthetics
- ZERO creature-specific features (no warts, whiskers, scales)

**TIER 2: Creature-Specific Decorative Shells (UNIQUE per creature)**
- `goblin_nose_shell.scad/.stl` - Incorporates `sensor_basic_mount` + warty texture
- `cat_nose_shell.scad/.stl` - Incorporates `sensor_basic_mount` + pink triangle nose
- `bear_nose_shell.scad/.stl` - Incorporates `sensor_basic_mount` + black button nose

### Shell Integration Pattern:
```scad
// goblin_nose_shell.scad
use <../basic_mounts/sensor_basic_mount.scad>

module goblin_nose_shell() {
    // Goblin-specific decorative features
    warty_nose_texture();
    large_nostril_details();
    
    // Universal hardware mount integration
    sensor_basic_mount();
}
```

### Critical Design Rule:
- Universal mounts provide hardware compatibility
- Decorative shells provide creature aesthetics
- Same electronics work across all creatures via shell substitution

---

## RULE 20: MULTI-ESP32 DISTRIBUTED ARCHITECTURE

### Distributed Controller Model:

- **Each subsystem** runs on a dedicated ESP32 chip
- **Controllers** are defined by hardware type (ESP32-S3-DevKitC-1, etc.)
- **Components** belong to exactly ONE subsystem/controller
- **SharedMemory** handles inter-ESP32 communication transparently

### Component-to-Controller Assignment Rules:

1. **Controller Detection**: When recursive descent encounters a `controller` keyword, that component defines a subsystem boundary
2. **Membership Inheritance**: All components contained within a controller belong to that subsystem
3. **Single Membership**: Each component belongs to exactly ONE subsystem (no splitting across controllers)
4. **Boundary Enforcement**: Components cannot span multiple controllers

### Controller Keyword Usage:
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

### Recursive Component Composition:
- **Single JSON file** defines entire creature via recursive composition
- **`contained_components`** keyword lists child components
- **Controller boundaries** determine subsystem membership during traversal
- **Hierarchical inheritance** flows down the component tree

### Code Generation per Subsystem:
- Individual component files (1:1 mapping)
- Per-subsystem dispatch tables (many:1 mapping)
- Build system isolates components by controller assignment

---

## RULE 21: JSON MOUNTING ARCHITECTURE SPECIFICATION

### Every positioned component JSON MUST include a `mounting_architecture` field:

```json
{
  "mounting_architecture": {
    "type": "two_tier | integrated | hardware_direct",
    "hardware_mount": "basic_mount_name (if two_tier)",
    "decorative_shell": "shell_name (if two_tier)", 
    "integrated_shape": "shape_name (if integrated)",
    "shape_generation": "auto | manual | template"
  }
}
```

### Three Mounting Architecture Types:

**1. Two-Tier Architecture** (`"type": "two_tier"`)
- Use when hardware will be reused across multiple creatures
- Generates universal `basic_mount.scad/.stl` + creature-specific `shell.scad/.stl`
- Shell incorporates hardware mount using `use <../basic_mounts/mount.scad>` pattern

**2. Integrated Single-Piece** (`"type": "integrated"`) 
- Use when custom design is simpler than two-tier approach
- Generates single `creature_part_complete.scad/.stl` with built-in hardware mount
- Hardware mount geometry integrated into creature-specific shape

**3. Hardware Direct** (`"type": "hardware_direct"`)
- Use when using hardware component physical shape directly
- No custom mounting shape generated
- Hardware mounts directly to skull/frame using vendor holes/brackets

### Shape Generation Options:
- **Auto**: System generates OpenSCAD from hardware specs and templates
- **Manual**: Designer creates custom OpenSCAD files manually
- **Template**: Uses existing template files with parameter substitution

### Physical Integration Requirements:
- All mounting specs must define precise hardware fit
- Cable management and strain relief required
- Standard M3 screw mounting patterns preferred
- Maintenance access and tool clearance consideration

---

## RULE 22: PYTHON TOOLS FOR CODE GENERATION AND AUTOMATION

**Critical Python scripts located in `/tools` directory handle code generation and project automation.**

### Primary Code Generation Tool:

**Subsystem Code Generation:**
- **Purpose**: Creates the controlling code for each ESP32 subsystem from JSON component configurations
- **Input**: Bot JSON files with component hierarchies and controller assignments
- **Output**: Complete C++ codebase for each subsystem including dispatch tables, component functions, and build configurations
- **Process**: Recursive component traversal, controller boundary detection, code template application

### Key Tool Functions:
- **Component Processing**: Scans JSON component definitions and resolves hierarchical dependencies
- **Controller Assignment**: Determines which components belong to which ESP32 subsystem
- **Dispatch Table Generation**: Creates init/act function tables with proper timing (hitCount) values
- **Build System Integration**: Generates platform.ini environments and CMakeLists.txt entries for each subsystem
- **Interface Mapping**: Assigns hardware interfaces (SPI, I2S, GPIO) to positioned components
- **Validation**: Ensures JSON syntax, component references, and architectural compliance

### Tool Usage Pattern:
```bash
# Generate code for all subsystems from bot configuration
python tools/generate_subsystem_code.py config/bots/goblin_full.json

# Validate JSON configurations and update dispatch tables  
python tools/validate_json_improved.py

# Create hardware interface assignments
python tools/assign_interfaces.py config/bots/goblin_full.json
```

### Code Generation Requirements:
- **JSON Input Validation**: All component references must resolve to valid files
- **Global Uniqueness**: Component names must be globally unique across entire system
- **Controller Boundaries**: Components assigned to correct ESP32 subsystem
- **Build System Updates**: Platform.ini and CMakeLists.txt automatically updated
- **Header Dependencies**: Include paths and dependencies properly resolved

### Additional Tools Functions:
- **3D Shape Generation**: OpenSCAD file creation from hardware specifications
- **Interface Management**: GPIO pin assignment and conflict detection
- **Configuration Validation**: JSON schema validation and consistency checking
- **Build Automation**: Multi-target compilation for distributed ESP32 system
- **Documentation Generation**: Automatic update of component inventories and pin mappings

### Tool Integration with Development Workflow:
1. **Modify JSON Configurations**: Update bot definitions, component specifications, or hardware assignments
2. **Run Code Generation Tools**: Execute Python scripts to generate/update C++ code
3. **Build Subsystems**: Use PlatformIO to compile individual ESP32 targets
4. **Flash and Test**: Deploy to hardware and validate inter-subsystem communication
5. **Iterate**: Refine configurations based on testing results

### Critical Rule: 
**Never manually edit generated C++ files - all changes must be made through JSON configurations and regenerated via Python tools.**

---

## ADDITIONAL RULES (RULES 1-14)

### RULE 5B: COMPONENT COORDINATION THROUGH SHARED STATE
**Core Principle**: Components only act on information they alone have access to, coordinating through shared state variables without breaking isolation.

### RULE 14: DISPLAY DRIVER INTERFACE CONTRACT
**All components with `component_type: "DISPLAY_DRIVER"` MUST export these three interface functions:**
```cpp
uint8_t* getBuffer(void);       // Allocate display buffer (malloc)
uint32_t getFrameSize(void);    // Total pixels in frame  
uint32_t getFrameRowSize(void); // Pixels per row
```

---

## CONSOLIDATION STATUS

### ✅ Consolidated Specification Documents:
- GLOBAL-SHARED-MEMORY-API.md
- JSON-DRIVEN-PERSONALITY-SPEC.md  
- DISPATCH-TABLE-GENERATION-RULES-SPEC.md
- THREE-LEVEL-COMPONENT-ATTACHMENT-SPEC.md
- TWO-TIER-ARCHITECTURE-DEFINITIVE-SPEC.md
- MULTI-ESP32-COMPONENT-ARCHITECTURE-SPEC.md
- JSON-MOUNTING-ARCHITECTURE-SPEC.md

### ❌ Obsoleted Documents:
- ESP-NOW-COMMUNICATIONS-PROTOCOL-SPEC.md (ESP-NOW fully encapsulated within SharedMemory)

### 📁 Original Files Location:
- Active Rules: `.github/AI-AGENT-RULES.md`
- Obsolete Specs: `docs/obsolete/rules/`

---

**END OF CONSOLIDATED RULES DOCUMENT**

**Review Status:** Ready for validation and feedback  
**Next Steps:** Real-world implementation testing to identify any rule gaps or conflicts