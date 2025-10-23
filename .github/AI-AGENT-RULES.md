
# 🚨 CRITICAL: EMBEDDED SYSTEMS DEVELOPMENT CONTEXT 🚨

**THIS IS NOT WEB DEVELOPMENT - THIS IS EMBEDDED MICROCONTROLLER PROGRAMMING**

**Target Hardware**: ESP32-S3 microcontroller (512KB RAM, 8MB Flash)
**Framework**: ESP-IDF native APIs (NOT Arduino, NOT web frameworks)
**Memory Constraints**: Resource-constrained embedded environment
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

# At the end of any rule evaluation or task, agents must read and apply the rules in .github/consistant_project_rules.md. However, these rules do not supersede the AI-AGENT-RULES.md; the ironclad agent rules in this file always take precedence.
# BASE LEVEL RULE: When the user asks why you are not doing something they requested, you must explicitly explain any system-level constraints, memory or message limits, or default design rules that are causing the issue. Do not give generic or evasive answers—be specific about the technical or policy reason for the behavior.

# AI AGENT IRONCLAD RULES - NEVER BREAK THESE
- always read and make sure you understand any file before changing it.
- if the filename or contents of a file are not consistant with what is expected because of appying any rule, either one from the system or the user, never change the ciontents of a 
	file or its filename unless that is within the exoplicit purpose of the current task.  Unless its obvious the file contents or filename should be changed as part of the current tyask,
	ask for permission from the human before changing or deleting the file.



## RULE: IMMEDIATE REPORTING OF BLOCKERS

If any project rule, technical limitation, or system constraint prevents the agent from proceeding with the current task, the agent must immediately report the exact reason to the user. The agent must not pause, stop, or silently fail without providing a clear explanation of the blocker, so the user can address or override it.
	the current task being worked on.


<!-- ## RULE 1 - these rules are consistant

- If a contradiction between one of these rules and another one of the rules in this list is detected, these rules must be modified using feedback from the user to make them so before anything else is allowed to proceed.
- These rules are not complete, in the sense additional consistant rules will be required as development proceeds.
- Anytime the user requests an action or task not allowed by one or more of these rules, he must be warned exactly what rule conflicts with his directions. At that point the human will be allowed to do 1 of these options:
	- a. Change the agent-ai rule (this ruleset) either temporarily or permanently to allow the task to proceed
	- b. Change the requested task
	- c. Abort the task completely and do something else
		

## RULE 2 TRANSPARENT DEVELOPMENT

- Agent will clearly communicate what files will be modified before making changes
- Agent has permission to modify any files necessary to complete the task specified by the human
- Agent will report all changes made at the end of each task
- Human can stop or redirect at any time during development
- All changes will be committed to git when the task is completed -->
..
## RULE: FRAMEWORK IS ESP-IDF, NOT ARDUINO

**NEVER reference:**
- Arduino.h
- Adafruit libraries  
- Arduino IDE
- Arduino functions (delay(), pinMode(), etc.)

**ALWAYS use:**
- ESP-IDF native APIs
- FreeRTOS

## RULE 5 System uses components defined in json files for all functionality.

- all component names are unique.
- every component has the following files.  if any component is worked on that is missing one of these files, the user is immiedietly notified and all processing must stop.  
	these files are {ComponentName}.json, {ComponentName}.cpp, and {ComponentName}.hpp
- each component has 2 functions, an init function named {name}_init( void ), and {name}_act(void ) that are defined in {ComponentName}.hpp and implemented in {ComponentName}.cpp
- {ComponentName}.hpp is located in and {ComponentName}.cpp is located in the root/src/components and root/include/components folders.
- for example, if the component name is GC9A01, the functions are gc9a01_init()  and gc9a01_act() located in root/src/components/gc9a01.cpp and root/include/components/gc9a01.hpp
- all components that have a shape defined have a file called {ComponentName}.scad and {ComponentName}.stl.  these files are locatedn in the root/assets/shapes/scad and root//assets/shapes/stl directories.
- every creature is defined by a single json file and all of the json files defined by recursive composition using the components contained within that file.  the rules used to compose the creature are defined inside markup files in the root/docs folder.  
	never use the contents of the in the /docs/obsolete folder



	
## RULE 6: ALWAYS USE BRACES (ALLMAN STYLE)

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

## RULE 7: ASK BEFORE ASSUMING

**If unclear:**

- ASK the human
- Don't guess
- Don't assume
- Don't improvise

**Better to ask than mess up code.**

## RULE 8: WHEN HUMAN SAYS "DON'T, make sure this doesn't contridict any known rules.  If so, report the inconsistancy immedietly.  once the human and ai have discussed what the final rule should be, encode it  and ap[end it to the end of this set of rules, ensuring rules are consistant.

## RULE 9: all FILES MUST BE ASCII ENCODED (NO BOM)


**Reference:** See `docs/component-json-requirements.md` for complete JSON parsing rules and validation requirements.

## RULE 10: DEVELOPMENT ENVIRONMENT REQUIREMENTS

**Operating System:** Windows  
**Default Shell:** PowerShell (Windows PowerShell v5.1)

**CRITICAL PowerShell Syntax Rules:**

- Use semicolon (`;`) to join commands on single line - **NOT** double ampersand (`&&`)
- Example: `command1; command2` ✅ **CORRECT**
- Example: `command1 && command2` ❌ **WRONG** (this is bash syntax)

**When generating terminal commands:**

- Generate commands correctly for PowerShell syntax
- Never use bash operators (`&&`, `||`) in PowerShell commands
- Agent must learn from syntax mistakes and not repeat them

**Build System:**

- Primary: PlatformIO with ESP-IDF framework
- Target: ESP32-S3-DevKitC-1
- Commands: `pio run` for builds, `pio run -t upload` for flashing

## RULE 11: COMPONENT GENERATION IS MANDATORY

**ALL component code MUST be generated via scripts - NEVER manually created**

**CRITICAL ARCHITECTURE REQUIREMENT:**

```
IF creating_component_code THEN
    → Use generate_tables.py or generate_individual_components.py
    → NEVER manually create .cpp/.hpp files in src/components/
    → NEVER manually edit generated dispatch tables
ELSE
    → Build will fail with missing headers
    → Inconsistent naming conventions  
    → Architecture violations
```

**Component Generation Commands:**

- Generate for specific bot: `python tools\generate_tables.py config\bots\bot_families\goblins\goblin_full.json src`
- Individual components: `python tools\generate_individual_components.py`

**Manual component creation ALWAYS fails** - the generation system is required for:

- Consistent naming conventions
- Proper header file generation
- Dispatch table integration
- Multi-ESP32 variant support

## RULE 12: MULTI-ESP32 ENVIRONMENT SELECTION  

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

read all of the rules files in docs/rules. make sure none of the rules contridict any othe rules.

## RULE: WINDOWS DEVELOPMENT ENVIRONMENT REQUIREMENTS

**Environment Specifications:**
- Windows PowerShell environment (powershell.exe v5.1)
- ASCII encoding ONLY - NO UTF-8 ENCODING - NO UNICODE ANYWHERE
- Windows NTFS file system with backslash paths
- CRLF line endings (not LF)
- Current directory: F:\GitHub\p32-animatronic-bot

**Mandatory Development Practices:**

1. Use Windows PowerShell syntax only
2. Use backslash paths: F:\GitHub\p32-animatronic-bot\config\file.json
3. NO Linux/Unix commands ever
4. Create files that work natively on Windows
5. Use simple PowerShell commands, not complex scripts
6. ASCII encoding only - NO UTF-8 - never Unicode
7. Test before scaling up
8. One file at a time if complex operations fail
9. Never validate VS Code config files (.vscode/*) as project JSON - exclude from consistency checks
10. Respect .gitignore files - files and folders listed in .gitignore are NOT part of the project and must be excluded from all validation and processing

**Token Cost Awareness:**
- Failed commands cost tokens from user budget
- Repeated mistakes are expensive
- Test approach before implementing at scale
- Use proven patterns that work except when they contradict one of the rules established for this project

## RULE: PROJECT NAVIGATION AND KEY LOCATIONS

**Script Locations:**
- Component table generation: `tools\generate_tables.py`
- Individual component creation: `tools\generate_individual_components.py`
- File structure analysis: `generate_file_structure.ps1` (root directory)
- STL generation: `tools\generate-stl-files.ps1`
- System verification: `tools\verify_full_system.ps1`

**Configuration Structure:**
- **Creatures/Bots**: `config\bots\bot_families\{family}\{bot_name}.json` (goblins, cats, bears, robots, etc.)
- **Hardware specs**: `config\hardware\{component_type}.json` (servo_9g_micro.json, gc9a01_display.json, etc.)
- **Interface definitions**: `config\interfaces\{interface_type}.json` (spi_device_1.json, pwm_channel_3.json, etc.)
- **Positioned components**: `config\components\positioned\{subsystem}\{component}.json`
- **Subsystem assemblies**: `config\subsystems\{subsystem_name}.json` (goblin_head.json, goblin_torso.json)
- **Component templates**: `config\components\templates\{type}\{template}.json`
- **Behaviors**: `config\behaviors\{behavior_name}.json`
- **Moods**: `config\moods\{mood_config}.json`

**Generated Code Structure:**
- **Component implementations**: `src\components\{component_name}.cpp`
- **Component headers**: `include\components\{component_name}.hpp`
- **Dispatch tables**: `src\subsystems\{subsystem}\{subsystem}_dispatch_tables.hpp/.cpp`
- **Shared classes**: `shared\{ClassName}.hpp` (Mood, Environment, SysTest, etc.)
- **Core system**: `src\core\memory\SharedMemory.hpp/.cpp`
- **Main application**: `src\main.cpp`

**Asset Structure:**
- **3D Models**: `assets\shapes\scad\{category}\{model}.scad` and `assets\shapes\stl\{category}\{model}.stl`
- **Animations**: `assets\animations\{creature_family}\{animation_name}.json`
- **Sounds**: `assets\sounds\{creature_family}\{sound_file}.wav`
- **Manufacturing**: `config\manufacturing\{process}\{specification}.json`

**Documentation Structure:**
- **Build guides**: `docs\build_guides\{guide_name}.md`
- **Architecture docs**: `docs\{system}_ARCHITECTURE.md`
- **Specifications**: `docs\{component}-spec.md`
- **Wiring diagrams**: `docs\wiring\{system}_wiring.md`

**Family Categories Available:**
- `config\bots\bot_families\goblins\` - Goblin creatures
- `config\bots\bot_families\cats\` - Cat creatures  
- `config\bots\bot_families\bears\` - Bear creatures
- `config\bots\bot_families\robots\` - Test/mechanical bots
- `config\bots\bot_families\tests\` - Hardware validation and system test bots
- `config\bots\bot_families\dragons\` - Dragon creatures
- `config\bots\bot_families\horror\` - Horror creatures
- `config\bots\bot_families\fantasy\` - Fantasy creatures

**Key Generation Commands:**
- Generate dispatch tables: `python tools\generate_tables.py config\bots\bot_families\{family}\{bot}.json src`
- Create missing components: `python tools\generate_individual_components.py`
- Validate JSON consistency: `.\generate_file_structure.ps1`
- Example: `python tools\generate_tables.py config\bots\bot_families\tests\test_bot.json src`

**File Organization Rules:**
- Hardware specifications → `config\hardware\`
- Interface definitions → `config\interfaces\`  
- Positioned components reference hardware via `hardware_reference` field
- Interface assignments via `interface_assignment` field
- All paths use Windows backslash format
- **UPDATE RULE**: When creating new directories, families, or major structural changes, update this project navigation section immediately to maintain accurate documentation
- **COMMIT RULE**: After updating AI-AGENT-RULES.md, always commit that file alone to git repository with descriptive message

**Current Project Status (Updated: 2025-10-23):**
- **Active Development**: Test bot with dual GC9A01 displays for hardware validation
- **Test Components Created**: test_left_eye, test_right_eye in `src\components\test\`
- **SysTest Class**: New shared class for system testing in `shared\SysTest.hpp`
- **Test Bot Family**: `config\bots\bot_families\tests\test_bot.json` for validation testing
- **Power System**: Stable 3.16V delivery via ATX PSU + breakout board
- **Hardware Status**: 2x GC9A01 displays connected and cool-running

## RULE: SHARED COMPONENT ARCHITECTURE - CRITICAL

**IRONCLAD RULE: When a component appears multiple times in the component tree, it is used UNCHANGED for dispatch, but the function definitions are included ONLY ONCE to avoid duplicate function errors in compile.**

**Example Architecture:**

```text
goblin_left_eye contains goblin_eye contains gc9a01
goblin_right_eye contains goblin_eye contains gc9a01
```

**Generated Function Table:**

```cpp
// UNIQUE functions per instance
goblin_left_eye_act()
goblin_right_eye_act()

// SHARED functions - single definition, multiple callers
goblin_eye_act()        // Called by both left and right eye
gc9a01_act()           // Called by both displays
```

**CRITICAL ARCHITECTURE PRINCIPLES:**

1. **NEVER modify shared component code** (goblin_eye.cpp, gc9a01.cpp) when fixing build errors
2. **Component-specific functions** get separate definitions per instance (test_left_eye_act, test_right_eye_act)
3. **Shared processing functions** have single definition, multiple callers (goblin_eye_act, gc9a01_act)
4. **Frame buffer management** happens in component-specific code, shared processing acts on buffers
5. **SPI device assignment** handled in component-specific code (SPI_DEVICE_1 vs SPI_DEVICE_2)

**VIOLATION CONSEQUENCES:**

- Duplicate function definition errors
- Breaking working mood/rendering systems
- Architecture violations

**This rule has been violated 10+ times. It is now IRONCLAD and cannot be ignored.**

## RULE: GLOBAL SHARED MEMORY API - CRITICAL

**The SharedMemory system is the ONLY way to communicate between subsystems.**

### CRITICAL API Understanding

**`read()` Does NOT Use Network:**
- `Mood *mood = GSM.read<Mood>("global_mood");` returns pointer to local memory
- If object doesn't exist, it's created with default constructor and auto-synchronized
- Components can call `read()` as often as needed with ZERO network overhead

**`write()` Triggers Mesh Broadcast:**
- `GSM.write<Mood>("global_mood");` synchronizes current state to all subsystems
- Only `write()` triggers ESP-NOW mesh traffic
- NEVER call ESP-NOW APIs directly - use SharedMemory exclusively

### Correct Usage Pattern

```cpp
void component_act(void) {
    // Get pointer to shared state (local access only)
    Mood *currentMood = GSM.read<Mood>("global_mood");
    Environment *envir = GSM.read<Environment>("global_environment");
    
    // Modify state directly via pointer
    if (envir->temperature_c > 80 && currentMood->contentment() > 20) {
        currentMood->contentment() -= (envir->temperature_c - 80);
        
        // Broadcast changes to all subsystems
        GSM.write<Mood>("global_mood");
    }
}
```

**ENFORCEMENT:** No component may directly call ESP-NOW APIs. All inter-subsystem communication MUST use SharedMemory class.

## RULE: THREE-LEVEL COMPONENT ATTACHMENT - CRITICAL

**NOTHING executes unless it's a component with init() and act() functions.**

### Level 1: System Components (Always Attached)
- Core platform functionality (WiFi, Serial, Watchdog)
- Always included in every build
- Low execution frequency (high hitCount values)

### Level 2: Family Components (Shared Across Bot Family)  
- Behavior/personality shared by family (goblin_personality, cat_behavior)
- Defined in `config/bots/bot_families/{family}_family.json`
- Mid-range execution frequency

### Level 3: Bot-Specific Components (Positioned Hardware)
- Physical hardware with spatial coordinates 
- Defined in individual bot JSON files
- High execution frequency (low hitCount values)

**Component Function Signature:** ALL components must have:
- `esp_err_t {name}_init(void)` - NO ARGUMENTS
- `void {name}_act(void)` - NO ARGUMENTS  
- Access globals via `#include "p32_shared_state.h"`

## RULE: TWO-TIER MOUNTING ARCHITECTURE - CRITICAL

**Universal hardware mounts + creature-specific decorative shells = modular system**

### Tier 1: Universal Hardware Mounts (IDENTICAL across ALL creatures)
- `sensor_basic_mount.scad/.stl` - HC-SR04 ultrasonic (ALL creatures)
- `display_basic_mount.scad/.stl` - GC9A01 round LCD (ALL creatures) 
- `speaker_basic_mount.scad/.stl` - 40mm I2S speaker (ALL creatures)
- Simple geometric brackets, utilitarian design, ZERO creature features

### Tier 2: Creature-Specific Decorative Shells (UNIQUE per creature)
- `goblin_nose_shell.scad/.stl` - Warty texture + incorporates sensor_basic_mount
- `cat_nose_shell.scad/.stl` - Pink triangle + incorporates sensor_basic_mount
- `bear_nose_shell.scad/.stl` - Black button + incorporates sensor_basic_mount

### Integration Pattern
```openscad
// goblin_nose_shell.scad
use <../basic_mounts/sensor_basic_mount.scad>

module goblin_nose_shell() {
    union() {
        sensor_basic_mount();  // Include universal hardware
        goblin_warts();        // Add creature features
        goblin_nostril_shape();
    }
}
```

**CRITICAL:** File naming must be accurate:
- ✅ `sensor_basic_mount.stl` (universal)
- ✅ `goblin_nose_shell.stl` (creature-specific)
- ❌ `goblin_nose_sensor.stl` (WRONG - implies goblin-specific sensor)

## RULE: JSON-DRIVEN PERSONALITY SYSTEM

**Personality is entirely data-driven - no hardcoded behavior logic.**

### Architecture Pattern
- JSON defines personality traits and reaction thresholds
- C++ code is generic - reads JSON, applies formulas  
- Same code works for all creatures - only JSON changes
- Artists can tweak personalities without programmer

### Configuration Structure
```json
{
  "personality_name": "Mischievous Goblin",
  "mood_reactions": {
    "close_object": {
      "distance_threshold_cm": 30,
      "triggers": [
        {"mood": "CURIOSITY", "delta": 10},
        {"mood": "EXCITEMENT", "delta": 15}
      ]
    }
  }
}
```

**Implementation uses SharedMemory exclusively - NO direct ESP-NOW calls.**

## RULE: COMPONENT GENERATION IS MANDATORY

**ALL component code MUST be generated via scripts - NEVER manually created.**

### Generation Commands
- Specific bot: `python tools\generate_tables.py config\bots\bot_families\goblins\goblin_full.json src`
- Individual components: `python tools\generate_individual_components.py`

**Manual component creation ALWAYS fails** - generation system required for:
- Consistent naming conventions
- Proper header file generation  
- Dispatch table integration
- Multi-ESP32 variant support

**VIOLATION CONSEQUENCE:** Build failures with missing headers and architecture violations.

## RULE: COORDINATE SYSTEM SPECIFICATION

**Two coordinate systems supported for different physical arrangements.**

### 2D Planar Coordinate System
- **Coordinate System ID**: `"planar_2d"`
- **Use Case**: Flat-panel arrangements, wall-mounted displays, simple layouts
- **Reference Point**: `"nose_center"` at (0, 0, 0)
- **Axes**: X (horizontal), Y (vertical), Z (depth - typically 0)

### 3D Skull-Based Coordinate System  
- **Coordinate System ID**: `"skull_3d"`
- **Use Case**: Anatomical humanoid faces, realistic animatronics
- **Reference Point**: `"nose_center"` at (0, 0, 0)
- **Baseline**: 3.0" eye spacing as standard human proportion
- **Scaling**: All positions scale proportionally with eye spacing changes

### Goblin Full Implementation
- **Vertical Compression**: 30% (multiply all Y coordinates by 0.7)
- **Eye Spacing**: 3.0" baseline maintained
- **Compression Formula**: `final_y = base_y * 0.7`

### Scaling Algorithm
```
scale_factor = new_eye_spacing / 3.0
final_x = base_x * scale_factor
final_y = base_y * scale_factor * compression_factor
final_z = base_z * scale_factor
```

### JSON Position Format
```json
{
  "coordinate_system": "skull_3d",
  "reference_point": "nose_center", 
  "position": {
    "x": "-1.5 INCH",
    "y": "1.0 INCH",
    "z": "-0.5 INCH"
  }
}
```

## RULE: JSON STRUCTURE REQUIREMENTS

**ALL JSON files must follow strict encoding and structure requirements.**

### ASCII Encoding Mandatory
- **ALL JSON files MUST be saved as pure ASCII without UTF-8 BOM**
- **PROBLEM**: UTF-8 BOM (bytes `EF BB BF`) breaks Python JSON parser
- **DETECTION**: First 3 bytes must NOT be `239, 187, 191`
- **CORRECT**: First byte should be `123` (the `{` character)

### Mandatory Fields (All JSON Files)
```json
{
  "relative_filename": "config/components/positioned/component_name.json",
  "version": "1.0.0", 
  "author": "config/author.json",
  "component_name": "unique_component_name",
  "description": "Human readable description",
  "timing": {
    "hitCount": 10
  },
  "software": {
    "init_function": "component_name_init",
    "act_function": "component_name_act"
  }
}
```

### Reference Pattern
- Use `"author": "config/author.json"` for consistent metadata
- Use relative paths for all file references
- Component names must be unique across entire system

## RULE: COMPONENT CODE REQUIREMENTS

**All component files must follow mandatory structure for compilation.**

### File Structure Requirements
- **Pattern**: `{component_name}.cpp` in `src/components/`
- **Language**: C++ (`.cpp`) - NEVER C files
- **Headers**: `{component_name}.hpp` in `include/components/`

### Mandatory Header Structure
```cpp
// P32 Component: {component_name}
// Auto-generated individual component file

#include "p32_component_config.h"
#include "core/memory/SharedMemory.hpp"  // For GSM access
#include "esp_log.h"
#include "esp_err.h"
// Additional includes for shared classes as needed
```

### Mandatory Function Definitions
```cpp
esp_err_t {component_name}_init(void) {
    // Initialization code
    return ESP_OK;
}

void {component_name}_act(void) {
    // Action/loop code - NO ARGUMENTS
    // Access g_loopCount from p32_shared_state.h
}
```

**Function Requirements:**
- **NO ARGUMENTS** - access globals directly from `p32_shared_state.h`
- Return `ESP_OK` from init functions
- Use void return for act functions

## RULE: INTERFACE GPIO ASSIGNMENT

**Bus + Device architecture maximizes pin efficiency through shared resources.**

### Architecture Pattern
- **Bus Components**: Define shared pins (MOSI, MISO, CLK, SDA, SCL)
- **Device Components**: Reference bus + add unique pins (CS, data pins)
- **Pin Efficiency**: 3 + N pins for N SPI devices, 2 + N pins for N I2S devices

### SPI Interface Pattern
- **Shared Bus Pins**: GPIO 12 (MISO), GPIO 13 (MOSI), GPIO 14 (CLK)
- **Device CS Pins**: GPIO 15 (CS1), GPIO 16 (CS2), GPIO 17 (CS3)
- **Total Usage**: 4 pins for 1 display, 5 pins for 2 displays

### I2S Interface Pattern
- **Shared Bus Pins**: GPIO 4 (BCLK), GPIO 5 (WS)
- **Device Data Pins**: GPIO 6 (DATA1), GPIO 7 (DATA2), GPIO 8 (DATA3)

### Configuration Files
```json
// SPI Bus: config/components/interfaces/spi_bus_vspi.json
{
  "interface_id": "SPI_BUS_VSPI",
  "interface_type": "SPI_BUS", 
  "pins": {
    "miso": 12, "mosi": 13, "clk": 14
  }
}

// SPI Device: config/components/interfaces/spi_device_1.json
{
  "interface_id": "SPI_DEVICE_1",
  "interface_type": "SPI_DEVICE",
  "bus_reference": "SPI_BUS_VSPI",
  "pins": {
    "cs": 15
  }
}
```

## RULE: UNIVERSAL HEAD ARCHITECTURE

**Skull framework + component modules = complete head system.**

### Design Principle
- **Skull Responsibility**: Mounting framework at P32 coordinates, creature silhouette
- **Component Responsibility**: Electronics + detailed creature features + software

### Standardized Mounting Points
- **Eyes**: 26mm outer diameter rings at P32 coordinates (±1.05", +0.7", -0.35")
- **Nose**: 22x17mm bracket at (0", 0", +0.25") for HC-SR04 sensor
- **Mouth**: 26mm outer diameter ring at (0", -1.05", 0") for GC9A01 display
- **Ears**: Variable creature-dependent positions for speakers

### Universal Application
- Same electronics work across all creatures (goblin, cat, bear)
- Component shells provide unique aesthetics while reusing hardware mounts
- Modular system allows creature variations with identical electronics

### File Organization
```
config/components/positioned/
├── {creature}_eye_left.json      # Left eye position + shell reference
├── {creature}_eye_right.json     # Right eye position + shell reference
├── {creature}_nose.json          # Nose sensor position + shell reference
├── {creature}_mouth.json         # Mouth display position + shell reference
└── {creature}_ear_{left|right}.json  # Ear positions + shell references
```

