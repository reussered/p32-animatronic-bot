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
4. **`docs/component-json-requirements.md`** - Consolidated JSON structure requirements, encoding rules, generation patterns, and dynamic pin assignment rules (CRITICAL for all JSON editing and component creation)

**FAILURE TO READ THESE DOCUMENTS WILL RESULT IN ARCHITECTURE VIOLATIONS**

** ANYTIME contridictory RULES ARE FOUND, THE AGENT MUST STOP AND GET DIRECTION FROM THE HUMAN WHICH RULE TO do.  ONCE THE CORRECT RULE IS DETERMINED REMOVE THE OLD RULE.
** it IS ABSOLUTELY CRITICAL THAT THE AGENT DOES NOT TRY AND REASON OUT WHAT TO DO IN THIS CASE.  IF WHAT TO DO ISN'T ABSOLUTELY CLEAR, 
** NO MATTER WHAT IS BEING DONE, THE AGENT MUST STOP AND ASK THE HUMAN.  THIS ALSO APPLIES TO THOSE RULES THAT MAY BE PART OF THE AGENTS TRAINING
** IT DOES NOT MATTER WHAT THE SOURCE OF THE RULE IS, EVEN EXTERNAL ONES.  aNY CIRCUMSTANCE WHERE THE AGENT IS UNCLEAR ON EXACTLY WHAT SHOULD BE DONE, THE HUMAN MUST BE
** GIVEN THE OPPURTUNITY TO INTERVENE. 

## RULE 0: READ BEFORE CHANGING - MANDATORY INVESTIGATION PROTOCOL

**BEFORE making ANY code changes, modifications, or "fixes", you MUST:**

1. **READ the existing implementation** - Use `read_file`, `grep_search`, or `semantic_search` to understand what's already there
2. **UNDERSTAND the current pattern** - See how the code actually works, don't assume you know
3. **IDENTIFY the actual problem** - Compile errors? Runtime errors? What's the SPECIFIC failure?
4. **VERIFY the fix is needed** - Is the code actually broken, or just different from your assumptions?
5. **CHECK for existing solutions** - Has this been solved before? Are there working examples?

**NEVER:**
- Rewrite working code because you "think" it should be different
- Move files without understanding their purpose and dependencies
- Create duplicate files in different locations
- "Improve" code that already works
- Change patterns without reading the architecture docs first
- Assume you know better than existing implementations

**IF UNCLEAR:** Ask the human before making changes. Better to ask than break working code.

## RULE 1: IMMEDIATE REPORTING OF BLOCKERS
If any project rule, technical limitation, or system constraint prevents the agent from proceeding with the current task, the agent must immediately report the exact reason to the user. The agent must not pause, stop, or silently fail without providing a clear explanation of the blocker.

## RULE 2: NAMING Rules ARE FOUND IN NAMING_RULES.MD
**ALL functionality is implemented as components defined in JSON files.**

- Component names are globally unique
- Each component has: `{ComponentName}.json`, `{ComponentName}.src`, `{ComponentName}.hdr`
- Each component has two functions: `{name}_init(void)` and `{name}_act(void)`
- Components with shapes have: `{ComponentName}.scad` and `{ComponentName}.stl`

## RULE 3: COMPONENT ISOLATION - NO DIRECT FUNCTION CALLS
**Components NEVER call each other's functions directly.**

- Components communicate ONLY through SharedMemory
- The "contains" relationship in JSON is for build inclusion, NOT function calls
- Component dispatch system calls `init()` and `act()` functions independently
- Example: `goblin_left_eye_act()` NEVER calls `goblin_eye_act()` or `gc9a01_act()`

## RULE 4: INTRA-SUBSYSTEM vs INTER-SUBSYSTEM COMMUNICATION

**There are two distinct communication patterns. Mixing them up is a critical architecture violation.**

### 1. INTRA-Subsystem Communication (Within a Single ESP32)
This pattern is for components that are part of the **same subsystem** and compiled into a single file.

- **Mechanism**: File-scoped global variables (e.g., `static` variables or pointers).
- **How it Works**: The build process concatenates all `.src` files for a subsystem's components into one large `.cpp` file. This shared file scope allows them to communicate "privately" and efficiently.
- **Example**:
    1. A display driver component (`gc9a01`) can define a function like `get_display_buffer()` that returns a pointer to its frame buffer.
    2. A hardware-specific component (`goblin_eye`) can call this function to get the pointer and manipulate the buffer directly.
    3. A bus component (`spi_bus`) can dynamically assign GPIO pins and store them in global variables that a generic driver (`generic_spi_display`) can then use for communication.
- **Rule**: This is a fast, efficient, and acceptable way for tightly-coupled components *within the same subsystem* to coordinate.

### 2. INTER-Subsystem Communication (Between Different ESP32s)
This pattern is for communication between components running on **different controllers**.

- **Mechanism**: The `SharedMemory` class (`GSM` global instance) is the **ONLY** permitted mechanism.
- **How it Works**: `GSM.write<T>()` broadcasts a data structure to all other subsystems over the network. `GSM.read<T>()` provides fast, local access to that data.
- **Rule**: Direct function calls, direct hardware communication, or any other mechanism besides `SharedMemory` is strictly forbidden between subsystems.

**Key Takeaway**: Components that define a subsystem (those with a `controller` field in JSON) do not have `init()` or `act()` functions themselves; they are containers. The components they contain can use file-scoped globals to talk to each other, but must use `SharedMemory` to talk to the outside world.

## RULE 5: PROVEN IMPLEMENTATION PATTERNS
**USE THESE WORKING PATTERNS - DO NOT REDESIGN THEM**

### PATTERN 1: MOOD-BASED DISPLAY ANIMATION (GOBLIN EYE SYSTEM)
**Location**: `goblin_left_eye` -> `goblin_eye` -> `gc9a01` ->`spi_display_bus` -> `generic_spi_display`

### PATTERN 2: SHARED STATE COMMUNICATION
**Location**: `SharedMemory` class with `GSM` global instance  IS DECLARED IN THE SUBSYTEMS MAIN.CPP FILE LIKE:
```cpp
// this goes above the invocation to app_main
SharedMemory GSM();

// this is first line of app_main
GSM.init();
```


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


## RULE 6: FILE ORGANIZATION STANDARDS

**The single source of truth for file organization is `.github/naming_rules.md`.**

Refer to that document for all rules regarding the location of configuration files, source code, assets, and documentation. The validation script `tools/validate_file_structure.py` can be used to check for compliance.

## RULE 7: BUILD AND DEPLOYMENT PROCESS

**Development Workflow**:
1. **Modify JSON**: Update component configurations
2. **Run Generation**: `python tools/generate_tables.py {bot_name} src`
3. **Build**: `pio run -e {environment}`
4. **Flash and Test**: Deploy to hardware and validate inter-subsystem communication
5. **Iterate**: Refine configurations based on testing results

**Critical Rule**: Never manually edit generated C++ files. All changes must be made through JSON configurations and regenerated.


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

**Defined SharedMemory Classes:**

**All SharedMemory classes are located in `/shared` directory.**

**1. Mood Class** - `/shared/Mood.hpp`
- Contains 9 different emotions with strength values for each
- Emotions: ANGER, FEAR, HAPPINESS, SADNESS, CURIOSITY, AFFECTION, IRRITATION, CONTENTMENT, EXCITEMENT
- Used by personality components to modify creature emotional state
- Read by display/animation components to adjust visual expressions

**2. Environment Class** - `/shared/Environment.hpp`
- Contains current surroundings and sensor data
- Variables include: distance_cm, temperature_c, humidity, sound_level_db, face_detected, target_confidence
- Updated by sensor components (nose, ears, cameras)
- Read by behavior and personality components for environmental reactions

**3. Personality Class** - `/shared/Personality.hpp` - FUTURE
- Will contain creature personality configuration data
- Expected to grow as personality system develops
- Loaded from JSON personality files during initialization
- Contains thresholds, reaction strengths, and behavioral parameters


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
 `

## RULE 19: THREE-LEVEL COMPONENT ATTACHMENT ARCHITECTURE

**CRITICAL DESIGN PRINCIPLE:**

**NOTHING executes unless it's a component with init() and act() functions.**

**Pure Component-Driven Architecture:**
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

**ALL files MUST BE ASCII encoded (NO BOM or UTF8)**

**Reference:** See `docs/component-json-requirements.md` for complete JSON parsing rules and validation requirements.

## RULE 26: COMPONENT GENERATION IS MANDATORY

**ALL component code MUST be generated via scripts - NEVER manually created**

**CRITICAL ARCHITECTURE REQUIREMENT:**

**NEVER edit the GENERATED files in `src/components/` (.cpp/.hpp extensions)**

```
IF creating_component_code THEN
    → Use generate_tables.py or generate_individual_components.py

    → NEVER manually create .cpp/.hpp files in the src/ or include/ folders
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

# AI Agent Rules

**Note**: This document has been consolidated into `consolidated-rules.md`. Please refer to that file for the latest rules and guidelines.
