# 🚨 CRITICAL: EMBEDDED SYSTEMS DEVELOPMENT CONTEXT 🚨

**THIS IS NOT WEB DEVELOPMENT - THIS IS EMBEDDED MICROCONTROLLER PROGRAMMING**

**Target Hardware**: ESP32-S3 microcontroller
**Framework**: ESP-IDF native APIs (NOT Arduino, NOT web frameworks)
**Resource Constraints**: Memory and performance constrained embedded environment
**Encoding**: ASCII-only for toolchain compatibility (NO UTF-8, NO Unicode)
**Dependencies**: Hardware drivers, FreeRTOS, SPI/I2S protocols
**Toolchain**: PlatformIO + ESP-IDF, GCC cross-compiler
**Debugging**: Serial console, hardware debugger (NOT browser dev tools)
**Performance**: Real-time constraints, interrupt-driven (NOT async/await patterns)

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

# AI AGENT IRONCLAD RULES - NEVER BREAK THESE

## 📋 MANDATORY PREREQUISITE: READ ALL RULE DOCUMENTS

**BEFORE implementing ANY changes, the AI agent MUST read and internalize:**

1. **`.github/AI-AGENT-RULES.md`** - This file (comprehensive architecture rules)
2. **`ARCHITECTURE_DECISION_LOG.md`** - Historical decisions, problem-solving approaches, and implementation rationale
3. **`NAMING_RULES.md`** - Component naming conventions and file organization
4. **`docs/component-json-requirements.md`** - Consolidated JSON, generation, and dynamic pin rules

**FAILURE TO READ THESE DOCUMENTS WILL RESULT IN ARCHITECTURE VIOLATIONS**

---

## RULE 1: IMMEDIATE REPORTING OF BLOCKERS
If any project rule, technical limitation, or system constraint prevents the agent from proceeding with the current task, the agent must immediately report the exact reason to the user. The agent must not pause, stop, or silently fail without providing a clear explanation of the blocker.

## RULE 2: COMPONENT-BASED ARCHITECTURE
**ALL functionality is implemented as components defined in JSON files.**

- Component names are globally unique
- Each component has: `{ComponentName}.json`, `{ComponentName}.src`, `{ComponentName}.hdr`
- Each component has two functions: `{name}_init(void)` and `{name}_act(void)`
- Files located in: `src/components/` and `include/components/`
- Components with shapes have: `{ComponentName}.scad` and `{ComponentName}.stl` in `assets/shapes/`

## RULE 3: COMPONENT ISOLATION - NO DIRECT FUNCTION CALLS
**Components NEVER call each other's functions directly.**

- Components communicate ONLY through SharedMemory
- The "contains" relationship in JSON is for build inclusion, NOT function calls
- Component dispatch system calls `init()` and `act()` functions independently
- Example: `goblin_left_eye_act()` NEVER calls `goblin_eye_act()` or `gc9a01_act()`

## RULE 4: SHARED STATE COORDINATION
**Components coordinate through shared state variables without breaking isolation.**

**Two Levels of State Sharing:**
1. **Local Subsystem Memory** - Direct memory variables for coordination between components within same subsystem
2. **SharedMemory Class** - Used for inter-system communications. Each subsystem has all of the init() and act() functions in a single compilable unit. They cannot call each other directly because the same component code fragments may be part of different subsystems.

**Testing Principle**: Component-level testing is inappropriate because component software is designed as fragments. Testing occurs at the subsystem level where components integrate and coordinate through shared state.

## RULE 5: PROVEN IMPLEMENTATION PATTERNS
**USE THESE WORKING PATTERNS - DO NOT REDESIGN THEM**

### PATTERN 1: MOOD-BASED DISPLAY ANIMATION (GOBLIN EYE SYSTEM)
**Location**: `goblin_left_eye` + `goblin_right_eye` + `goblin_eye` + `gc9a01_display`

**Architecture**:
- **Positioned Components** (`goblin_left_eye`, `goblin_right_eye`): Handle buffer allocation, positioning, SPI binding
- **Creature Logic Component** (`goblin_eye`): Shared mood-based color processing, palette management
- **Hardware Component** (`gc9a01_display`): Display driver interface (getBuffer, getFrameSize, getFrameRowSize)

**Code Structure**:
```cpp
// goblin_left_eye.src - Positioned component
void goblin_left_eye_init(void) {
    left_eye_animation_buffer = getBuffer();  // From gc9a01_display
    // Initialize buffer management
}

void goblin_left_eye_act(void) {
    // Set currentFrame pointer for goblin_eye to use
    currentFrame = left_eye_animation_buffer;
    // Handle left eye specific logic
}

// goblin_eye.src - Shared creature logic
void goblin_eye_init(void) {
    init_goblin_eye_palette();  // 256-color organized palette
}

void goblin_eye_act(void) {
    if (currentFrame) {
        process_frame_with_mood();  // Apply mood-based color adjustments
    }
}

// gc9a01_display.src - Hardware interface
uint8_t* getBuffer(void);      // Return display buffer
uint32_t getFrameSize(void);   // Total pixels
uint32_t getFrameRowSize(void); // Pixels per row
```

**Reusable Pattern**: This exact structure can be copied for any creature's eyes, ears, mouth, etc. Just change the palette colors and mood mappings.

### PATTERN 2: SHARED STATE COMMUNICATION
**Location**: `SharedMemory` class with `GSM` global instance

**Usage Pattern**:
```cpp
// Reading shared state
Mood *mood = GSM.read<Mood>();
Environment *env = GSM.read<Environment>();

// Modifying shared state in place
mood->anger = 75;
env->temperature = 25.5;

// Writing shared state (triggers inter-subsystem sync)
GSM.write<Mood>();      // Broadcasts current mood state
GSM.write<Environment>(); // Broadcasts current environment state
```

**Implementation Rules**:
- `read<T>()` is local memory access (fast, no network)
- `write<T>()` triggers inter-subsystem synchronization
- All shared classes in `/shared` directory
- Defined classes: Mood, Environment, Personality, SysTest

### PATTERN 3: JSON-DRIVEN COMPONENT COMPOSITION
**Location**: Recursive composition from `goblin_full.json` → subsystems → positioned components

**Structure**:
```json
{
    "bot_type": "GOBLINFULL",
    "subsystem_assemblies": ["config/subsystems/goblin_head.json"],
    "family_level_components": ["config/components/functional/goblin_mood.json"],
    "mood_defaults": {"ANGER": 20, "HAPPINESS": 0}
}
```

**Generation Process**:
1. `python tools/generate_tables.py goblin_full src` creates dispatch tables
2. Auto-generates: `p32_dispatch_tables.cpp/hpp`, `p32_component_functions.cpp/hpp`
3. Links all component `init()` and `act()` functions into main loop

## RULE 6: FILE ORGANIZATION STANDARDS

**Code Structure**:
- **Component implementations**: `src/components/{component_name}.src`
- **Component headers**: `include/components/{component_name}.hdr`
- **Dispatch tables**: `src/subsystems/{subsystem}/{subsystem}_dispatch_tables.hpp/.cpp`
- **Shared classes**: `shared/{ClassName}.hpp`
- **Core system**: `src/core/memory/SharedMemory.hpp/.cpp`

**Asset Structure**:
- **3D Models**: `assets/shapes/scad/{category}/{model}.scad` and `.stl`
- **Animations**: `assets/animations/{creature}/{animation}.json`
- **Sounds**: `assets/sounds/{creature}/{sound}.wav`

**Configuration Structure**:
- **Bot families**: `config/bots/bot_families/{family}/{bot}.json`
- **Subsystems**: `config/subsystems/{subsystem_name}.json`
- **Components**: `config/components/{type}/{component}.json`

## RULE 7: BUILD AND DEPLOYMENT PROCESS

**Development Workflow**:
1. **Modify JSON**: Update component configurations
2. **Run Generation**: `python tools/generate_tables.py {bot_name} src`
3. **Build**: `pio run -e {environment}`
4. **Flash and Test**: Deploy to hardware and validate inter-subsystem communication
5. **Iterate**: Refine configurations based on testing results

**Critical Rule**: Never manually edit generated C++ files. All changes must be made through JSON configurations and regenerated.

## RULE 8: SHARED MEMORY API - SOLE COMMUNICATION MECHANISM

**SharedMemory is the ONLY inter-subsystem communication mechanism.**

**Core Usage Pattern**:
```cpp
// CORRECT: Fast local reads (NO network overhead)
Mood *mood = GSM.read<Mood>();
Environment *envir = GSM.read<Environment>();

// CORRECT: Direct modifications (local only)
mood->ANGER = 75;
envir->distance_cm = 25.5;

// CORRECT: Trigger synchronization (WITH network overhead)
GSM.write<Mood>();      // Broadcasts current mood state
GSM.write<Environment>(); // Broadcasts current environment state
```

**Initialization**:
```cpp
GSM.init();  // Initialize shared memory system
```

**Prohibited Methods**:
- Direct communication protocol API calls
- Direct Bluetooth/WiFi APIs for inter-subsystem coordination
- Custom inter-ESP32 communication protocols
- File transfer or serial communication between subsystems

## RULE 9: DISPATCH TABLE GENERATION ARCHITECTURE

**CRITICAL: Four-File Auto-Generation Architecture**

**Generated Files**:
1. **`src/p32_dispatch_tables.cpp`** - Implementation with tables
2. **`include/p32_dispatch_tables.hpp`** - Header declarations
3. **`src/p32_component_functions.cpp`** - Component function aggregator
4. **`include/p32_component_functions.hpp`** - Function declarations

**Generation Process**:
- Process depth-first to create flat execution order
- Generated files MUST be included in build system
- NEVER manually edit generated files

## RULE 10: MULTI-ESP32 DISTRIBUTED ARCHITECTURE

- **Each subsystem** runs on a dedicated ESP32 chip
- **Controllers** are defined by hardware type (ESP32-S3-DevKitC-1, etc.)
- **SharedMemory** handles inter-ESP32 communication transparently
- **SharedMemory** coordinates between ESP32 controllers

## RULE 11: PYTHON TOOLS FOR CODE GENERATION AND AUTOMATION

**Purpose**: Creates the controlling code for each ESP32 subsystem from JSON component configurations

**Tool Integration with Development Workflow**:
1. **Modify JSON Configurations**: Update bot definitions, component specifications, or hardware assignments
2. **Run Code Generation Tools**: Execute Python scripts to generate/update C++ code
3. **Build Subsystems**: Use PlatformIO to compile individual ESP32 targets
4. **Flash and Test**: Deploy to hardware and validate inter-subsystem communication
5. **Iterate**: Refine configurations based on testing results

**Critical Rule**: Never manually edit generated C++ files - all changes must be made through JSON configurations and regenerated via Python tools.

## RULE 12: DEVELOPMENT PRACTICES

**Windows Development Environment**:
1. Use Windows PowerShell syntax only
2. Use backslash paths: `F:\GitHub\p32-animatronic-bot\config\file.json`
3. NO Linux/Unix commands ever
4. Create files that work natively on Windows
5. Use simple PowerShell commands, not complex scripts
6. ASCII encoding only - NO UTF-8 - never Unicode
7. Test before scaling up
8. One file at a time if complex operations fail
9. Never validate VS Code config files (.vscode/*) as project JSON - exclude from consistency checks
10. Respect .gitignore files - files and folders listed in .gitignore are NOT part of the project and must be excluded from all validation and processing

**Token Cost Awareness**:
- Failed commands cost tokens from user budget
- Repeated mistakes are expensive
- Test approach before implementing at scale
- Use proven patterns that work except when they contradict one of the rules established for this project

## RULE 13: PROJECT NAVIGATION AND KEY LOCATIONS

**Script Locations**:
- Build scripts: `build.ps1`, `build_multi_esp32.ps1`
- Generation tools: `python tools/generate_tables.py`
- Validation: `validate_json_improved.py`, `ascii_validator.ps1`

**Configuration Structure**:
- **Creatures/Bots**: `config\bots\bot_families\{family}\{bot_name}.json` (goblins, cats, bears, robots, etc.)
- **Hardware specs**: `config\hardware\{component_type}.json` (servo_9g_micro.json, gc9a01_display.json, etc.)
- **Interface definitions**: `config\interfaces\{interface_type}.json` (spi_bus.json, pwm_channel_3.json, etc.)
- **Positioned components**: `config\components\positioned\{subsystem}\{component}.json`
- **Subsystem assemblies**: `config\subsystems\{subsystem_name}.json` (goblin_head.json, goblin_torso.json)
- **Component templates**: `config\components\templates\{type}\{template}.json`
- **Behaviors**: `config\behaviors\{behavior_name}.json`
- **Moods**: `config\moods\{mood_config}.json`

**Generated Code Structure**:
- **Component implementations**: `src\components\{component_name}.src`
- **Component headers**: `include\components\{component_name}.hdr`
- **Dispatch tables**: `src\subsystems\{subsystem}\{subsystem}_dispatch_tables.hpp/.cpp`
- **Shared classes**: `shared\{ClassName}.hpp` (Mood, Environment, SysTest, etc.)
- **Core system**: `src\core\memory/SharedMemory.hpp/.cpp`

**Asset Structure**:
- **3D Models**: `assets\shapes\scad\{category}\{model}.scad` and `assets\shapes\stl\{category}\{model}.stl`
- **Animations**: `assets\animations\{creature_family}\{animation_name}.json`
- **Sounds**: `assets\sounds\{creature_family}\{sound_file}.wav`
- **Manufacturing**: `config\manufacturing\{process}\{specification}.json`

**Documentation Structure**:
- **Build guides**: `docs\build_guides\{guide_name}.md`
- **Architecture docs**: `docs\{system}_ARCHITECTURE.md`
- **Specifications**: `docs\{component}-spec.md`
- **Wiring diagrams**: `docs\wiring\{system}_wiring.md`

**Family Categories Available**:
- `config\bots\bot_families\goblins\` - Goblin creatures
- `config\bots\bot_families\cats\` - Cat creatures
- `config\bots\bot_families\bears\` - Bear creatures
- `config\bots\bot_families\robots\` - Test/mechanical bots
- `config\bots\bot_families\tests\` - Hardware validation and system test bots
- `config\bots\bot_families\dragons\` - Dragon creatures
- `config\bots\bot_families\horror\` - Horror creatures
- `config\bots\bot_families\fantasy\` - Fantasy creatures

**Key Generation Commands**:
- Generate dispatch tables: `python tools\generate_tables.py config\bots\bot_families\{family}\{bot}.json src`
- Create missing components: `python tools\generate_individual_components.py`
- Validate JSON consistency: `.\generate_file_structure.ps1`
- Example: `python tools\generate_tables.py config\bots\bot_families\tests\test_bot.json src`

**File Organization Rules**:
- Hardware specifications → `config\hardware\`
- Interface definitions → `config\interfaces\`
- Positioned components reference hardware via `hardware_reference` field
- All paths use Windows backslash format

## RULE 14: ENCAPSULATED INTERFACE ARCHITECTURE

**OBSOLETE: All automatic interface assignment systems are obsolete.**

**New Encapsulated Architecture:**

- **Positioned Components**: Only declare `hardware_reference`
- **Hardware Components**: Encapsulate ALL interface details internally
- **No Automatic Assignment**: No template interfaces, numbered instances, or generation scripts
- **Pure Encapsulation**: Components are "private classes" - interface details completely hidden

**DISPLAY SYSTEM ARCHITECTURE:**

**Positioned Component Layer (goblin_left_eye):**

- References goblin_eye hardware, positioned on left side
- Does NOT care about driver details or specific pins
- Only knows positioning coordinates and hardware_reference

**Hardware Component Layer (goblin_eye):**

- Knows it uses gc9a01 display type
- Gets all display parameters (frame size, pixel size, etc.) from display component
- Uses "display:" keyword to specify display type, no other details

**Implementation Functions:**

- getBuffer() - Returns pointer to frame memory
- getFrameWidth() - Returns frame width in pixels
- getFrameHeight() - Returns frame height in pixels

**Display Driver Layer (gc9a01):**

- Transfers data from frame_ptr one pixel at a time
- Contains generic_spi_display for SPI protocol handling
- Frame buffer points to memory start for transfer
- frame_row_size and frame_row_count describe pixels per frame
- bytes_per_pixel set in specific display code

**Generic SPI Display Layer:**

- Used for all displays using SPI bus protocol
- Contains spi_bus component for GPIO pin determination
- GPIO pin assignment is DONE IN CODE by spi_bus component DYNAMICALLY IN THE COMPONENT LOOP
- Can be used for any display device
- Generic_spi_driver doesn't care about specific pins, just how to use them
- Can be used for any device using SPI protocol

**Critical Rules:**

- NO automatic interface assignment scripts or systems
- NO template interfaces or numbered instances
- ALL interface details encapsulated within hardware_reference chain
- Components are treated as private classes with hidden implementation details

## RULE 15: COMPONENT COORDINATION THROUGH SHARED STATE

**Core Principle**: Components only act on information they alone have access to, coordinating through shared state variables without breaking isolation.

**Two Levels of State Sharing:**
1. **Local Subsystem Memory** - Direct memory variables for coordination between components within same subsystem (e.g., head subsystem)
2. **SharedMemory Class** - Used for inter-system communications. Each subsystem has all of the init() and act() functions in a single compilable unit. They cannot call each other directly because the same component code fragments may be part of different subsystems.

**Component Buffer Management Pattern:**

```cpp
// In display component (generated once even if referenced multiple times)
uint8_t *frame_ptr;  // used for all eye operations
const uint32_t FRAME_ROW = 240;
const uint32_t ROW_COUNT = 240;
const uint32_t FRAME_SIZE = FRAME_ROW * ROW_COUNT;
uint32_t current_spi_channel;

// In goblin_left_eye component
const uint8_t left_eye_buffer[FRAME_SIZE];

// In goblin_right_eye component
const uint8_t right_eye_buffer[FRAME_SIZE];
```

**Initialization Order (reverse dependency):**

1. **display_init()** - establishes frame specifications and shared variables
2. **goblin_eye_init()** - does nothing (shared logic component)
3. **goblin_left_eye_init()** / **goblin_right_eye_init()** - sets frame_ptr to respective buffer

**Execution Flow Pattern:**

- **Eye components** (left/right): Set frame_ptr to their buffer, set current_spi_channel, bounds check only
- **Shared processor** (goblin_eye): Modifies frame based on Mood/Environment/Personality
- **Display component**: Transfers frame via current_spi_channel, advances frame_ptr

**Critical Rules:**

- Each component only accesses data it owns or global shared state
- No component calls functions on other components
- Display component doesn't care about frame contents or buffer boundaries
- Eye components reset frame_ptr when bounds exceeded
- Code generation must handle duplicate component detection (display referenced by multiple eyes)

## RULE 16: DISPLAY DRIVER INTERFACE CONTRACT

**All components with `component_type: "DISPLAY_DRIVER"` MUST export these three interface functions:**

```cpp
uint8_t* getBuffer(void);       // Allocate display buffer (malloc)
uint32_t getFrameSize(void);    // Total pixels in frame
uint32_t getFrameRowSize(void); // Pixels per row
```

**These functions provide hardware abstraction allowing creature components to work with any display:**

- **GC9A01 (240x240 circular)**: getFrameSize() returns 57600, getFrameRowSize() returns 240
- **SSD1306 (128x64 OLED)**: getFrameSize() returns 8192, getFrameRowSize() returns 128
- **ILI9341 (320x240 TFT)**: getFrameSize() returns 76800, getFrameRowSize() returns 320
- **ST7796 (320x480 TFT)**: getFrameSize() returns 153600, getFrameRowSize() returns 320

**Component Type Constraints:**

- `creature_specific_display` components (like `goblin_eye`) MUST contain a `display_driver` component
- `positioned_component` display components MUST reference `display_driver` type in their hierarchy
- Eye/mouth/display components can call display interface functions when display_driver is in their containment chain

**JSON Configuration Requirements:**

```json
{
    "component_type": "DISPLAY_DRIVER",
    "required_interface_functions": [
        "getBuffer", "getFrameSize", "getFrameRowSize"
    ]
}
```

This enables plug-and-play display substitution while maintaining interface compatibility.

## RULE 17: GLOBAL SHARED MEMORY API - SOLE COMMUNICATION MECHANISM

**SharedMemory is the ONLY inter-subsystem communication mechanism:**

- **Single Communication Protocol**: SharedMemory class is the exclusive gateway for all inter-subsystem communication
- **Internal Protocol Encapsulation**: SharedMemory contains complete internal implementation of inter-subsystem communication protocol
- **No External Protocol Access**: Components NEVER directly access communication APIs
- **Component Isolation**: All components use only SharedMemory API for inter-subsystem coordination

**CRITICAL Architecture Understanding:**

**`read()` is Local Memory Access (NO Network Traffic):**
- `read(string name)` returns pointer to class variable in local memory
- No memcpy or network synchronization unless variable doesn't exist
- If variable doesn't exist, it's created automatically using default constructor
- Components can call `read()` **as often as needed** (even every loop!) with ZERO network overhead

**`write()` Triggers Inter-Subsystem Synchronization:**
- `write(string name)` ensures shared memory in every subsystem has the same value
- `write()` internally uses communication protocol to broadcast changes to all other controllers
- Only `write()` triggers network traffic between subsystems
- Binary transmission maintains efficiency

**Required Usage Pattern:**
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

**Change Detection Pattern (Recommended):**
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

**SharedMemory Implementation Rules:**

**Case-Insensitive Names:**
- All SharedMemory variable names are case-insensitive
- `SharedMemory.read("g_Mood")` same as `SharedMemory.read("G_MOOD")` or `SharedMemory.read("g_mood")`
- Consistent naming convention recommended: use lowercase with underscores

**Fixed-Size Data Blocks:**
- SharedMemory data blocks NEVER change in size after creation
- If a data structure needs size changes, create NEW class with DIFFERENT name
- Prevents memory fragmentation and ensures reliable inter-subsystem synchronization
- Version control through name changes (e.g., `g_Envir_v2` if Environment class structure changes)

**Defined SharedMemory Classes:**

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

**Usage Examples:**
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

**Prohibited Communication Methods:**
- Direct communication protocol API calls
- Direct Bluetooth classic or BLE APIs
- Direct WiFi communication for inter-subsystem coordination
- Custom inter-ESP32 communication protocols
- File transfer or serial communication between subsystems

## RULE 18: JSON-DRIVEN PERSONALITY SYSTEM

**Data-Driven Personality Architecture:**

**Personality module is entirely data-driven** - no hardcoded behavior logic!

- **JSON defines personality traits** - how strongly creature reacts to stimuli
- **C++ code is generic** - reads JSON config, applies formulas
- **Same code works for all creatures** - only JSON changes
- **Artists can tweak personalities** - no programmer required

**Personality JSON Structure Requirements:**
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

**Generic Personality Component Pattern:**
- Load personality JSON from filesystem during init()
- Apply mood reactions based on environment triggers
- Use SharedMemory for reading Environment and writing Mood
- Support mood decay over time
- Enable different creatures to have distinct personalities using same code

**Required Mood Components:**
ANGER, FEAR, HAPPINESS, SADNESS, CURIOSITY, AFFECTION, IRRITATION, CONTENTMENT, EXCITEMENT

**SharedMemory Integration:**
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

## RULE 19: THREE-LEVEL COMPONENT ATTACHMENT ARCHITECTURE

**CRITICAL DESIGN PRINCIPLE:**

**NOTHING executes unless it's a component with init() and act() functions.**

**Pure Component-Driven Architecture:**
- ESP32 `app_main()` contains ONLY the component execution loop
- All functionality is implemented as components with `init()` and `act()` functions
- Components are attached at three hierarchical levels
- Core loop iterates through registered components calling their `act()` functions based on `hitCount` timing

**Three Component Levels:**

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

**Component Registration Requirements:**
- JSON configuration defines component hierarchy
- Code generation creates dispatch tables
- Compilation links all component implementations
- Runtime execution based on hitCount timing

## RULE 20: TWO-TIER MOUNTING ARCHITECTURE

**NEVER CONFUSE UNIVERSAL MOUNTS WITH CREATURE SHELLS**

**TIER 1: Universal Hardware Mounts (IDENTICAL across ALL creatures)**
- `sensor_basic_mount.scad/.stl` - HC-SR04 ultrasonic (ALL creatures)
- `display_basic_mount.scad/.stl` - GC9A01 round LCD (ALL creatures)
- `speaker_basic_mount.scad/.stl` - 40mm I2S speaker (ALL creatures)
- `servo_basic_mount.scad/.stl` - SG90, MG996R servos (ALL creatures)

**Universal Mount Characteristics:**
- Precise component fit with exact hardware dimensions
- Standard M3 screw mounting, consistent hole patterns
- Built-in cable management and wire routing
- Utilitarian function over form, minimal aesthetics
- ZERO creature-specific features (no warts, whiskers, scales)

**TIER 2: Creature-Specific Decorative Shells (UNIQUE per creature)**
- `goblin_nose_shell.scad/.stl` - Incorporates `sensor_basic_mount` + warty texture
- `cat_nose_shell.scad/.stl` - Incorporates `sensor_basic_mount` + pink triangle nose
- `bear_nose_shell.scad/.stl` - Incorporates `sensor_basic_mount` + black button nose

**Shell Integration Pattern:**
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

**Critical Design Rule:**
- Universal mounts provide hardware compatibility
- Decorative shells provide creature aesthetics
- Same electronics work across all creatures via shell substitution

## RULE 21: MULTI-ESP32 ENVIRONMENT SELECTION

**Each subsystem targets optimal ESP32 variant for cost/performance**

```
IF gpio_needed <= 22 AND memory_needed <= 350KB THEN
    → Use ESP32-C3 environment (cost-optimized $2-3)
ELSIF gpio_needed <= 34 AND memory_needed <= 500KB THEN
    → Use ESP32 environment (balanced $4-5)
ELSE
    → Use ESP32-S3 environment (high-performance $6-8)
```

**Build Commands:**

- Specific environment: `pio run -e goblin_head`
- Chip type group: `pio run -e left_arm -e right_arm` (all ESP32-C3)
- All environments: `pio run`

## RULE 22: JSON MOUNTING ARCHITECTURE SPECIFICATION

**Every positioned component JSON MUST include a `mounting_architecture` field:**

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

**Three Mounting Architecture Types:**

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

**Shape Generation Options:**
- **Auto**: System generates OpenSCAD from hardware specs and templates
- **Manual**: Designer creates custom OpenSCAD files manually
- **Template**: Uses existing template files with parameter substitution

**Physical Integration Requirements:**
- All mounting specs must define precise hardware fit
- Cable management and strain relief required
- Standard M3 screw mounting patterns preferred
- Maintenance access and tool clearance consideration

## RULE 23: BRACES STYLE - ALLMAN FORMATTING

**ALL `for`, `while`, `if`, `else` statements MUST use braces, even for single-line bodies.**

**Braces MUST be on their own line at the same indentation level (Allman style).**

**CORRECT (Allman Style):**

```cpp
for (int i = 0; i < SIZE; i++)
{
    doSomething;
}

if (condition)
{
    doAction;
}

while (running)
{
    update;
}

if (errorCode != ESP_OK )
{
    handleError( errorCode );
}
else
{
    continueWork();
}

void myFunction(int bepoop)
{
    // Function body
}

class twindles
{
    // class definition
}
```

**WRONG:**

```c
for (int i = 0; i < SIZE; i++) doSomething();      // ❌ NO - missing braces
if (condition) doAction();                          // ❌ NO - missing braces
while (running) update();                           // ❌ NO - missing braces

for (int i = 0; i < SIZE; i++) {                   // ❌ NO - K&R style (brace on same line)
    doSomething();
}

if (condition) {                                    // ❌ NO - K&R style (brace on same line)
    doAction();
}
class boogywoogy {                                  // ❌ NO - K&R style (brace on same line)
}

void* function babydoll( int arg1, float arg2) {    // ❌ NO - K&R style (brace on same line)

}


```

**Rationale:**

- Prevents bugs when adding debug statements
- Makes code structure visually clear
- Braces at same indent level show scope hierarchy
- Enforces consistent style
- Required for safety-critical embedded code

## RULE 24: ASK BEFORE ASSUMING

**If unclear:**

- ASK the human
- Don't guess
- Don't assume
- Don't improvise

**Better to ask than mess up code.**

## RULE 25: ASCII ENCODING ONLY

**ALL files MUST BE ASCII encoded (NO BOM)**

**Reference:** See `docs/component-json-requirements.md` for complete JSON parsing rules and validation requirements.

## RULE 26: COMPONENT GENERATION IS MANDATORY

**ALL component code MUST be generated via scripts - NEVER manually created**

**CRITICAL ARCHITECTURE REQUIREMENT:**

**NEVER edit the GENERATED files in `src/components/` (.cpp/.hpp extensions)**

```
IF creating_component_code THEN
    → Use generate_tables.py or generate_individual_components.py
    → It is permitted when necessary to edit .src/.hdr files in config/components/ (these are permanent source files)
    → NEVER manually create .cpp/.hpp files in src/components/
    → NEVER manually edit generated dispatch tables
ELSE
    → Build will fail with missing headers
    → Inconsistent naming conventions
    → Architecture violations
```

**Component Generation Commands:**

- Generate for specific bot: `python tools\generate_tables.py config\bots\bot_families\{family}\{bot}.json src`
- Individual components: `python tools\generate_individual_components.py`

**Manual component creation ALWAYS fails** - the generation system is required for:

- Consistent naming conventions
- Proper header file generation
- Dispatch table integration
- Multi-ESP32 variant support

---

## CONSOLIDATION COMPLETE

All critical architecture rules from the following specification documents have been consolidated into this AI-AGENT-RULES.md file:

- GLOBAL-SHARED-MEMORY-API.md
- JSON-DRIVEN-PERSONALITY-SPEC.md
- DISPATCH-TABLE-GENERATION-RULES-SPEC.md
- THREE-LEVEL-COMPONENT-ATTACHMENT-SPEC.md
- TWO-TIER-ARCHITECTURE-DEFINITIVE-SPEC.md
- MULTI-ESP32-COMPONENT-ARCHITECTURE-SPEC.md
- JSON-MOUNTING-ARCHITECTURE-SPEC.md

**Original specification documents can now be moved to docs/obsolete/ as they have been fully consolidated.**

**Note**: Communications protocol specifications were obsoleted as protocols are now fully encapsulated within SharedMemory class.
 
 
