# P32 Animatronic Bot Project Rules Understanding Report

## 🚨 **CRITICAL CONTEXT: EMBEDDED SYSTEMS DEVELOPMENT**

This is **NOT** web development - this is embedded microcontroller programming for the ESP32 family of chips with strict constraints:

- **Target**: ESP32 family of chips microcontroller (512KB RAM, 8MB Flash)  
- **Framework**: ESP-IDF native APIs (NO Arduino libraries)
- **Environment**: Windows PowerShell, ASCII encoding only (NO UTF-8/Unicode)
- **Architecture**: Real-time, interrupt-driven embedded system
- **Memory**: Resource-constrained with byte-level optimization requirements

## **IRONCLAD ARCHITECTURAL RULES**

### 1. **Pure Component-Driven Architecture**
- **NOTHING executes unless it's a component** with `init()` and `act()` functions
- The main loop contains **ZERO application logic** - only component dispatch
- All functionality is implemented as components with standardized interfaces

### 2. **Three-Level Component Hierarchy**
- **System Level**: Core platform (WiFi, Serial, Watchdog) - always present
- **Family Level**: Behavior/personality shared across bot family (Goblin, Cat, Bear)  
- **Bot-Specific Level**: Positioned hardware components (eyes, nose, mouth, sensors)

### 3. **Component Isolation & Communication**
- Components **NEVER** call functions in other components directly
- All inter-component communication through global shared state (`p32_shared_state.h`)
- **Legal Information Sharing**: Components can use access functions exported by other components (e.g., `getBuffer()`, `getFrameSize()` from display drivers)
- **Shared Variable Pattern**: Components can define shared variables that other components access via `extern` declarations
- **Example Pattern**: `goblin_left_eye.cpp` defines `currentFrame` pointer, other components access it because all of the components for this subsystem are in the same file
- SharedMemory class is the **ONLY** inter-subsystem communication mechanism
- Uses internal ESP-NOW for multi-ESP32 communication with `read()`/`write()` API

## **MULTI-ESP32 DISTRIBUTED ARCHITECTURE**

### Controller Assignment
- Each subsystem runs on dedicated ESP32 chip
- Components belong to exactly ONE subsystem/controller
- `controller` keyword defines subsystem boundaries in JSON AND specifies the exact chip type
- Examples: `"controller": "ESP32-C3"` or `"controller": "ESP32-S3-R8N8"`
- Code generation creates per-subsystem dispatch tables

### Subsystem Distribution Strategy
- **Torso subsystem**: System-level components (WiFi, ESP-NOW mesh, telemetry) - 50% CPU
- **Head subsystem**: Real-time rendering (displays, audio, sensors) - 75% CPU  
- **Load balanced**: Strategic distribution for optimal performance

## **JSON CONFIGURATION SYSTEM**

### Critical Requirements
- **ASCII encoding MANDATORY** (NO UTF-8 BOM) - UTF-8 BOM breaks Python JSON parser
- All components defined via JSON with standardized structure
- `component_name` serves as both human identifier and technical identifier
- Global component name uniqueness across entire system

### Component Function Pattern
```json
"software": {
    "init_function": "component_name_init",
    "act_function": "component_name_act"  // Or "STUB" for hardware-only
}
```

### Recursive Composition Architecture
- Single JSON file defines entire creature via `contained_components` arrays
- Example: `goblin_full.json` → `goblin_head.json` → `goblin_left_eye.json`
- Hierarchical component inclusion enables modular creature construction

## **CODE GENERATION REQUIREMENTS**

### Mandatory Script-Based Generation
- **ALL component code MUST be generated via Python scripts** - never manually created
- **Component File Aggregation**: Individual component .cpp/.hpp files are NEVER compiled in isolation
- **Python Script Process**: Parses JSON files and aggregates all component files into large composite documents
- **Composite Output**: For subsystem named `goblin_head`, creates `goblin_head_component_functions.hpp` and `goblin_head_component_functions.cpp`
- Generation creates 4 files: dispatch tables (.cpp/.hpp) + aggregated component functions (.cpp/.hpp)
- All 4 generated files MUST be in `.gitignore` (auto-generated content)
- Manual component creation always fails due to naming/integration requirements

### Component Naming Conventions
- Function names: `{component_name}_init(void)`, `{component_name}_act(void)`
- **NO ARGUMENTS** pattern - components access `g_loopCount` and shared state directly
- Bilateral components: `{creature}_{side}_{component}` (e.g., `goblin_left_eye`)

## **HARDWARE ABSTRACTION PATTERNS**

### Two-Tier Mounting Architecture
- **Tier 1**: Universal hardware mounts (identical across ALL creatures)
  - `sensor_basic_mount.scad/.stl`, `display_basic_mount.scad/.stl`
- **Tier 2**: Creature-specific decorative shells (unique per creature)
  - `goblin_nose_shell.scad`, `cat_nose_shell.scad` - incorporates basic mounts

### Interface Assignment System
- Template + numbered instance pattern: `spi_device.json` → `spi_device_1.json`, `spi_device_2.json`
- Hardware components declare generic requirements, generation script assigns specific pins
- Automatic conflict detection and sequential assignment

### Display Driver Interface Contract
All `DISPLAY_DRIVER` components must export:
```cpp
uint8_t* getBuffer(void);       // Allocate display buffer
uint32_t getFrameSize(void);    // Total pixels in frame
uint32_t getFrameRowSize(void); // Pixels per row  
```

## **CODING STANDARDS**

### Allman Style Braces (MANDATORY)
- **ALL** `if`, `for`, `while` statements MUST use braces on separate lines
- Prevents bugs when adding debug statements
- Enforces visual scope hierarchy

### Windows Development Environment
- Windows PowerShell syntax only (`;` for command joining, NOT `&&`)
- Backslash paths: `F:\GitHub\p32-animatronic-bot\config\file.json`
- CRLF line endings, ASCII encoding throughout
- NO Linux/Unix commands ever

## **SHARED STATE MANAGEMENT**

### SharedMemory API Pattern
- **`read()`**: Local memory access (NO network overhead) - can call frequently
- **`write()`**: Triggers ESP-NOW synchronization (WITH network overhead) - only when data changes
- Case-insensitive variable names: `"g_Mood"` = `"G_MOOD"` = `"g_mood"`
- Fixed-size data blocks prevent memory fragmentation

### Defined Shared Classes
- **Mood** (`"g_Mood"`): 9 emotional states with strength values
- **Environment** (`"g_Envir"`): Sensor data (distance, temperature, face detection)
- **Personality** (`"g_Personality"`): Future personality configuration system

## **UNIVERSAL HEAD ARCHITECTURE**

### Standard Mounting System
- **Eye mounting**: 26mm rings at ±1.05" from nose center
- **Nose mounting**: 22x17mm bracket at nose center for HC-SR04 sensor  
- **Mouth mounting**: 26mm ring below nose for expression display
- **Modular design**: Same electronics work across all creatures via shell substitution

## **DEVELOPMENT WORKFLOW REQUIREMENTS**

### File Organization Rules
- **Creatures**: `config\bots\bot_families\{family}\{bot_name}.json`
- **Hardware specs**: `config\hardware\{component_type}.json`
- **Positioned components**: `config\components\positioned\{subsystem}\{component}.json`
- **Generated code**: `src\components\{component}.cpp`, `include\components\{component}.hpp`

### Key Generation Commands
- `python tools\generate_tables.py config\bots\bot_families\goblins\goblin_full.json src`
- `python tools\generate_individual_components.py`  
- `.\generate_file_structure.ps1` (validation)

### Critical Rules for AI Agents
- **Always read and understand files before changing them**
- **Ask before assuming** - don't guess, don't improvise
- **Report blockers immediately** - explain technical constraints preventing progress
- **Never manually edit generated files** - all changes via JSON + regeneration
- **Test before scaling** - validate approach on single files before batch operations

## **PERSONALITY & BEHAVIOR SYSTEM**

### Data-Driven Design
- Personality entirely JSON-configurable (no hardcoded behavior)
- Generic C++ code reads JSON config and applies formulas
- Artists can modify creature personalities without programming
- Mood decay, environmental reactions, and triggers all JSON-defined

This architecture enables rapid creation of diverse animatronic characters while maintaining consistent, reliable embedded software patterns. The system scales from simple test bots to complex multi-subsystem creatures using the same core framework.