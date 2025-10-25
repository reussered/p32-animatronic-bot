
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

## RULE 5A: COMPONENT ISOLATION - NO FUNCTION CALLS BETWEEN COMPONENTS

**CRITICAL ARCHITECTURAL PRINCIPLE: Components never ever reference code in other components.**

- Components are **completely isolated** from each other
- **NO component function ever calls another component's function**
- Components communicate ONLY through global shared state (`p32_shared_state.h`)
- Each component reads/writes `g_loopCount`, `g_shared_state`, etc. directly
- The "contains" relationship in JSON is for **build inclusion**, NOT function calls
- Component dispatch system calls `init()` and `act()` functions independently
- Example: `goblin_left_eye_act()` NEVER calls `goblin_eye_act()` or `gc9a01_act()`

**WRONG (Function Calls Between Components):**
```cpp
void goblin_left_eye_act(void) {
    goblin_eye_act();      // NEVER DO THIS
    gc9a01_act();          // NEVER DO THIS
}
```

**CORRECT (Isolated Components Using Shared State):**
```cpp
void goblin_left_eye_act(void) {
    // Read from global shared state
    uint32_t frame = g_loopCount / 10;
    
    // Do component-specific work
    process_left_eye_frame(frame);
    
    // Write to global shared state
    g_shared_state.left_eye_ready = true;
}
```

	
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

## RULE 13: INTERFACE ASSIGNMENT ARCHITECTURE

**Interface System Overview:**
The system uses a template + numbered instance pattern for hardware interface assignment, automatically managed by generation scripts.

**Interface Template Structure:**
- **Template Interfaces** define the protocol/connection type (stored in `config\interfaces\`)
  - `spi_device.json` - Defines SPI device protocol template
  - `gpio_pair.json` - Defines 2-pin GPIO interface template  
  - `i2c_device.json` - Defines I2C device protocol template
  - `1wire_device.json` - Defines 1-wire protocol template

**Numbered Instance Generation:**
- **Numbered Instances** provide specific pin assignments (auto-generated)
  - `spi_device_1.json`, `spi_device_2.json`, etc. - Specific SPI chip select pins
  - `gpio_pair_1.json`, `gpio_pair_2.json`, etc. - Specific GPIO pin pairs
  - `i2c_device_1.json`, `i2c_device_2.json`, etc. - Specific I2C device addresses

**Hardware Component Interface Declaration:**
Components declare generic interface requirements in their hardware specifications:

```json
// In config/hardware/gc9a01_display.json
{
  "control_interface": {
    "protocol": "spi_device",
    "required_pins": ["MOSI", "SCK", "CS", "DC", "RST"]
  }
}

// In config/hardware/hw500_vibration_sensor.json  
{
  "control_interface": {
    "protocol": "gpio_pair",
    "required_pins": ["digital_input", "analog_input"]
  }
}
```

**Automatic Interface Assignment Process:**
1. **Bot Scanning**: Generation script scans all positioned components in a bot configuration
2. **Interface Counting**: Counts how many components need each interface type
3. **Sequential Assignment**: Assigns next available numbered interface instance
   - First SPI component → `spi_device_1.json`
   - Second SPI component → `spi_device_2.json`  
   - First GPIO pair component → `gpio_pair_1.json`
   - Second GPIO pair component → `gpio_pair_2.json`
4. **Interface Generation**: Creates numbered interface files if they don't exist
5. **Assignment Injection**: Updates positioned component configs with specific interface references

**Generated Interface Assignment Example:**
```json
// In positioned component after generation
{
  "component_name": "left_eye_display",
  "hardware_reference": "config/hardware/gc9a01_display.json",
  "interface_assignment": {
    "spi_device": "config/interfaces/spi_device_1.json"
  }
}
```

**Critical Rules:**
- Components NEVER reference numbered interfaces directly in their definitions
- Hardware specs declare generic interface requirements only
- Generation script handles all specific pin assignments
- Interface templates define electrical and protocol specifications
- Numbered instances provide actual GPIO pin mappings
- Pin conflict detection occurs during interface generation
- Same interface type shared across multiple ESP32 variants with different pin mappings

**UPDATE RULE**: When creating new directories, families, or major structural changes, update this project navigation section immediately to maintain accurate documentation
- **COMMIT RULE**: After updating AI-AGENT-RULES.md, always commit that file alone to git repository with descriptive message

**Current Project Status (Updated: 2025-10-23):**
- **Active Development**: Test bot with dual GC9A01 displays for hardware validation
- **Test Components Created**: test_left_eye, test_right_eye in `src\components\test\`
- **SysTest Class**: New shared class for system testing in `shared\SysTest.hpp`
- **Test Bot Family**: `config\bots\bot_families\tests\test_bot.json` for validation testing
- **Power System**: Stable 3.16V delivery via ATX PSU + breakout board
- **Hardware Status**: 2x GC9A01 displays connected and cool-running

## RULE 5B: COMPONENT COORDINATION THROUGH SHARED STATE

**Core Principle**: Components only act on information they alone have access to, coordinating through shared state variables without breaking isolation.

**Two Levels of State Sharing:**
1. **Local Subsystem Memory** - Direct memory variables for coordination between components within same subsystem (e.g., head subsystem)
2. **SharedMemory Class** - ESP-NOW communication for coordination between different subsystems (head ↔ torso ↔ other ESP32 controllers)

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

## RULE 14: DISPLAY DRIVER INTERFACE CONTRACT

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

## RULE 15: GLOBAL SHARED MEMORY API - SOLE COMMUNICATION MECHANISM

**SharedMemory is the ONLY inter-subsystem communication mechanism:**

- **Single Communication Protocol**: SharedMemory class is the exclusive gateway for all inter-subsystem communication
- **Internal ESP-NOW Encapsulation**: SharedMemory contains a complete internal implementation of ESP-NOW bluetooth protocol
- **No External Protocol Access**: Components NEVER directly access ESP-NOW, Bluetooth, WiFi, or any other communication APIs
- **Component Isolation**: All components use only SharedMemory API for inter-subsystem coordination

**CRITICAL Architecture Understanding:**

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
- Direct ESP-NOW API calls (esp_now_send, esp_now_register_recv_cb, etc.)
- Direct Bluetooth classic or BLE APIs
- Direct WiFi communication for inter-subsystem coordination
- Custom inter-ESP32 communication protocols
- File transfer or serial communication between subsystems

## RULE 16: DISPATCH TABLE GENERATION ARCHITECTURE

**CRITICAL: Four-File Auto-Generation Architecture**

The validation script (`validate_json_improved.py`) MUST auto-generate exactly 4 files from JSON component configurations:

1. **`src/p32_dispatch_tables.cpp`** - Implementation with tables
2. **`include/p32_dispatch_tables.hpp`** - Header with declarations  
3. **`src/p32_component_functions.cpp`** - Component implementation aggregator
4. **`include/p32_component_functions.hpp`** - Component header aggregator

**Version Control Rules:**
ALL four generated files MUST be in `.gitignore` since they are script-generated:
```gitignore
# Auto-generated dispatch tables - created by validate_json_improved.py
src/p32_dispatch_tables.cpp
include/p32_dispatch_tables.hpp
src/p32_component_functions.cpp
include/p32_component_functions.hpp
```

**Component Function Signature Rules:**
- ALL component functions MUST follow the NO ARGUMENTS pattern:
  ```cpp
  esp_err_t component_name_init(void);
  void component_name_act(void);
  ```
- Components access shared state via `#include "p32_shared_state.h"`
- Function names MUST follow pattern: `{component_name}_{function_type}`
- All component names MUST be globally unique across the entire system

**Three Table Architecture:**
Generate exactly 3 parallel dispatch tables with SAME indexing:
```cpp
init_func_t initTable[COMPONENT_TABLE_SIZE];    // Called once at startup
act_func_t actTable[COMPONENT_TABLE_SIZE];      // Called based on timing  
uint32_t hitCountTable[COMPONENT_TABLE_SIZE];   // Execution frequency
```

**Parallel Indexing Rule:**
`initTable[i]`, `actTable[i]`, and `hitCountTable[i]` MUST refer to the SAME component.

**Component Hierarchy Processing:**
- Process depth-first to create flat execution order
- Dispatch tables: NO deduplication (multiple entries allowed)
- Function definitions: WITH deduplication (single definition only)
- Hardware-only components (`"hardware_only": true`) do NOT generate init/act functions

**Integration Rules:**
- Generated files MUST be included in build system
- `main.cpp` MUST include dispatch tables header
- Core loop: Initialize all components once, then execute based on timing

## RULE 17: JSON-DRIVEN PERSONALITY SYSTEM

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

# Trigger synchronization (with network overhead)
SharedMemory.write("g_Mood");
```

## RULE 18: THREE-LEVEL COMPONENT ATTACHMENT ARCHITECTURE

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

## RULE 19: TWO-TIER MOUNTING ARCHITECTURE

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

## RULE 20: MULTI-ESP32 DISTRIBUTED ARCHITECTURE

**Distributed Controller Model:**

- **Each subsystem** runs on a dedicated ESP32 chip
- **Controllers** are defined by hardware type (ESP32-S3-DevKitC-1, etc.)
- **Components** belong to exactly ONE subsystem/controller
- **SharedMemory** handles inter-ESP32 communication transparently

**Component-to-Controller Assignment Rules:**

1. **Controller Detection**: When recursive descent encounters a `controller` keyword, that component defines a subsystem boundary
2. **Membership Inheritance**: All components contained within a controller belong to that subsystem
3. **Single Membership**: Each component belongs to exactly ONE subsystem (no splitting across controllers)
4. **Boundary Enforcement**: Components cannot span multiple controllers

**Controller Keyword Usage:**
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

**Recursive Component Composition:**
- **Single JSON file** defines entire creature via recursive composition
- **`contained_components`** keyword lists child components
- **Controller boundaries** determine subsystem membership during traversal
- **Hierarchical inheritance** flows down the component tree

**Code Generation per Subsystem:**
- Individual component files (1:1 mapping)
- Per-subsystem dispatch tables (many:1 mapping)
- Build system isolates components by controller assignment

## RULE 21: JSON MOUNTING ARCHITECTURE SPECIFICATION

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

**Note**: ESP-NOW-COMMUNICATIONS-PROTOCOL-SPEC.md was obsoleted entirely as ESP-NOW is now fully encapsulated within SharedMemory class.

## RULE 23: PYTHON TOOLS FOR CODE GENERATION AND AUTOMATION

**Critical Python scripts located in `/tools` directory handle code generation and project automation.**

**Primary Code Generation Tool:**

**Subsystem Code Generation:**
- **Purpose**: Creates the controlling code for each ESP32 subsystem from JSON component configurations
- **Input**: Bot JSON files with component hierarchies and controller assignments
- **Output**: Complete C++ codebase for each subsystem including dispatch tables, component functions, and build configurations
- **Process**: Recursive component traversal, controller boundary detection, code template application

**Key Tool Functions:**
- **Component Processing**: Scans JSON component definitions and resolves hierarchical dependencies
- **Controller Assignment**: Determines which components belong to which ESP32 subsystem
- **Dispatch Table Generation**: Creates init/act function tables with proper timing (hitCount) values
- **Build System Integration**: Generates platform.ini environments and CMakeLists.txt entries for each subsystem
- **Interface Mapping**: Assigns hardware interfaces (SPI, I2S, GPIO) to positioned components
- **Validation**: Ensures JSON syntax, component references, and architectural compliance

**Tool Usage Pattern:**
```bash
# Generate code for all subsystems from bot configuration
python tools/generate_subsystem_code.py config/bots/goblin_full.json

# Validate JSON configurations and update dispatch tables  
python tools/validate_json_improved.py

# Create hardware interface assignments
python tools/assign_interfaces.py config/bots/goblin_full.json
```

**Code Generation Requirements:**
- **JSON Input Validation**: All component references must resolve to valid files
- **Global Uniqueness**: Component names must be globally unique across entire system
- **Controller Boundaries**: Components assigned to correct ESP32 subsystem
- **Build System Updates**: Platform.ini and CMakeLists.txt automatically updated
- **Header Dependencies**: Include paths and dependencies properly resolved

**Additional Tools Functions:**
- **3D Shape Generation**: OpenSCAD file creation from hardware specifications
- **Interface Management**: GPIO pin assignment and conflict detection
- **Configuration Validation**: JSON schema validation and consistency checking
- **Build Automation**: Multi-target compilation for distributed ESP32 system
- **Documentation Generation**: Automatic update of component inventories and pin mappings

**Tool Integration with Development Workflow:**
1. **Modify JSON Configurations**: Update bot definitions, component specifications, or hardware assignments
2. **Run Code Generation Tools**: Execute Python scripts to generate/update C++ code
3. **Build Subsystems**: Use PlatformIO to compile individual ESP32 targets
4. **Flash and Test**: Deploy to hardware and validate inter-subsystem communication
5. **Iterate**: Refine configurations based on testing results

**Critical Rule**: Never manually edit generated C++ files - all changes must be made through JSON configurations and regenerated via Python tools.
