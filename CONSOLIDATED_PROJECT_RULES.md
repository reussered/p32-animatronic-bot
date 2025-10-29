# P32 ANIMATRONIC BOT - CONSOLIDATED PROJECT RULES

## 🚨 CRITICAL SETUP (READ FIRST)

**Project Root**: `f:\GitHub\p32-animatronic-bot` (fixed location, use absolute paths)

**MANDATORY**: Read this document before any code changes. This consolidates all architectural rules, naming conventions, and coding standards from multiple source documents.

**Target Hardware**: Multiple ESP32 chips in several subsystems, each using one of the ESP32 family. Default to ESP32-S3-R8N16. Actual chips used start with S3 but less capable chips may be specified after analysis and testing.  
**Framework**: ESP-IDF native APIs (NOT Arduino, NOT web frameworks)  
**Encoding**: ASCII-only for toolchain compatibility (NO UTF-8, NO Unicode, NO emojis)  
**Performance**: Real-time constraints, interrupt-driven (NOT async/await patterns)  
**Dependencies**: Hardware drivers, FreeRTOS, SPI/I2S protocols  
**Toolchain**: PlatformIO + ESP-IDF, GCC cross-compiler  
**Debugging**: Serial console, hardware debugger (NOT browser dev tools)

---

## 🤖 AI AGENT BEHAVIOR GUIDELINES

- **Defer to user expertise** as domain expert
- **Never delete, modify, or refactor** existing code without explicit permission
- **Explain all proposed changes** clearly and get confirmation for significant edits
- **Preserve existing code context and intent** - no changes "out of context" or based on external assumptions
- **Immediate reporting of blockers**: If any rule prevents proceeding, report the exact reason immediately

---

## 🏗️ ARCHITECTURE OVERVIEW

### Pure Component System

**ALL functionality is implemented as components defined in JSON files.**

- Everything is a component with `init()`/`act()` functions
- Main loop iterates components only - no application logic in `app_main()`
- Component names are globally unique across entire system
- Each component has: `{ComponentName}.json`, `{ComponentName}.src`, `{ComponentName}.hdr`
- Components with shapes have: `{ComponentName}.scad` and `{ComponentName}.stl` in `assets/shapes/`

### Component Pattern

```cpp
esp_err_t component_name_init(void);  // Returns ESP_OK on success
void component_name_act(void);        // No args, accesses g_loopCount/g_shared_state
```

### Component Isolation - NO Direct Function Calls

**Components NEVER call each other's functions directly.**

- Components communicate with other subsystems and the global system state ONLY through SharedMemory
- The "contains" relationship in JSON is for build inclusion, NOT function calls
- Component dispatch system calls `init()` and `act()` functions independently
-
All of the component source code fragments (in component)name.src go into a single compileable source file called: {subsystem)name}_component_functions.cpp and the hdr files are aggregated into the corresponding .hpp file.  This means that variables defined outside the init() and act() functions can be used by components added later when the JSON dependency is walked.  For a fully implemented example see goblin_head_component_functions.cpp in src/ and  goblin_head_component_functions.hpp in include/

**Note**: The dispatch tables contain the components in the order the tree is walked without deduplication. The component_function.cpp and .hpp only include each component once - duplicates are deleted.

- Example: `goblin_left_eye_act()` NEVER calls `goblin_eye_act()` or `gc9a01_act()`

### Shared State Coordination

**Components coordinate through shared state variables without breaking isolation.**

**Two Levels of State Sharing:**

1. **Local Subsystem Memory** - Direct memory variables for coordination between components within same subsystem
2. **SharedMemory Class** - Used for inter-system communications across chips (broadcasts to all chips in mesh network)

**Testing Principle**: Component-level testing is inappropriate because component software is designed as fragments. Testing occurs at the subsystem level where components integrate and coordinate through shared state.

---

## 🔧 ESSENTIAL WORKFLOWS

- **Generate Components**: `python tools/generate_tables.py goblin_full src` (reads JSON, creates dispatch tables and aggregates component files)
- **Build & Flash**: `pio run -t upload -t monitor`
- **Validate Config**: `.\generate_file_structure.ps1`
- **Multi-Chip Testing**: Use `test/distance_eye_test/` for SharedMemory mesh validation

---

## 📋 NAMING CONVENTIONS

### Filename Case Conventions

- **Default**: All filenames must be entirely lowercase
- **Exception**: Files containing class definitions
  - Filename MUST exactly match the class name (case-sensitive)
  - Example: `SharedMemory.hpp` contains `class SharedMemory`
- **Scripts**: Always lowercase (`.ps1`, `.py`, `.sh`)
- **Documentation**: Always lowercase (`.md`, `.txt`)

### Component Naming

- `component_name` serves as both human identifier and technical identifier
- Used in function names: `{component_name}_init(void)`, `{component_name}_act(void)`
- Bilateral Pattern: `{creature}_{side}_{component}` (e.g., `goblin_left_eye`)

### Subsystem Naming

- Subsystems are identified when JSON components contain the `'controller'` field
- The subsystem name equals the component name where the `'controller'` field is found
- Generated files: `{subsystem}_component_functions.cpp/.hpp`, `{subsystem}_dispatch_tables.cpp/.hpp`

### Hardware & Interface Naming

**Two-Tier Mounting Architecture:**

- **Tier 1**: Universal Hardware Mounts (identical across ALL creatures)
- **Tier 2**: Creature-Specific Decorative Shells

### Shared Class Definitions

- All classes shared through SharedMemory are defined in `/shared`

---

## 🗂️ FILE ORGANIZATION PATTERNS

### Configuration Structure

- **Creatures/Bots**: `config\bots\bot_families\{family}\{bot}.json`
- **Hardware specs**: `config\hardware\{component_type}.json`
- **Interface definitions**: `config\interfaces\{interface_type}.json`
- **Internal Components**: `config\internal\{component_name}.json`
- **Instrumentation Components**: `config\instrumentation\{Component_Name}.json`
- **Test Components**: `config\test\{component_name}.json`
- **Subsystem Components**: `config\subsystems\{subsystem_name}.json`

### Generated Code Structure

- **Component implementations**: `{component_name}.src`
- **Component headers**: `{component_name}.hdr`
- **Generated Software**: `src/p32_dispatch_tables.cpp`, `include/p32_dispatch_tables.hpp`
- **Subsystem files**: `{subsystem}_component_functions.cpp/.hpp`, `{subsystem}_dispatch_tables.cpp/.hpp`

### Asset Organization

- **3D Models**: `assets\shapes\scad\{category}\{model}.scad`
- **STL Files**: `assets\shapes\stl\{category}\{model}.stl`
- **Animations**: `assets\animations\{creature_family}\{animation_name}.json`
- **Sounds**: `assets\sounds\{creature_family}\{sound_file}.wav`

---

## 🏛️ COMPONENT RELATIONSHIPS & JSON INTERPRETATION

### Three-Level Hierarchy

- **System Level**: Core platform (WiFi, Serial, Watchdog) - always present
- **Family Level**: Behavior/personality shared across bot family (Goblin, Cat, Bear)
- **Bot-Specific Level**: Positioned hardware components (eyes, nose, mouth, sensors)

### Recursive Composition

- `goblin_full.json` → `goblin_head.json` → `left_eye.json`

### JSON Config System

- **Hardware**: `config/components/hardware/` - Physical device specs
- **Interfaces**: `config/components/interfaces/` - Connection definitions (SPI buses, GPIO)
- **Positioned**: `config/components/positioned/` - Spatially-aware component instances

### JSON Interpretation Rules

- Reference other configs: `"author": "config/author.json"`
- Coordinates: `"x": "-1.5 INCH"` (always include units)
- Coordinate systems: `"coordinate_system": "planar_2d"` or `"skull_3d"`
- Required fields: `"relative_filename"`, `"version"`, `"author"`
- Shape parameter: Present for hardware-only components (`"init_function": "STUB"`)

### JSON Parsing Requirements (CRITICAL)

**ASCII Encoding Mandatory (NO UTF-8 BOM):**

- ALL JSON files MUST be saved as pure ASCII without UTF-8 BOM
- UTF-8 BOM breaks Python JSON parser with "Expecting value: line 1 column 1 (char 0)"
- Always use ASCII encoding, never UTF-8 with BOM

**Mandatory JSON Structure:**

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

**Function Name Generation:**

- Init: `p32_{component_name}_init()`
- Act: `p32_{component_name}_act(uint32_t loopCount)`
- Or "STUB" if not needed

**C++ Parsing Implementation:**

- Uses cJSON library for parsing
- Error handling required - check for NULL before accessing values
- Type validation - ensure correct data types

**Component Composition:**

- Wildcard pattern: Any field ending in `*_components` triggers recursive inclusion
- Example: `"left_eye_components": "config/components/positioned/goblin_left_eye.json"`
- Processing: Generator parses hierarchy, generates dispatch tables, handles duplicates

**Bus Interface Requirements:**

- SPI: Shared (SCLK, MISO) + Unique per device (CS, MOSI)
- I2S: Shared (BCLK, WS) + Unique (DATA)
- I2C: Shared (SCL, SDA) - address-based multiplexing
- ADC/PWM: Unique pins per channel
- GPIO_PAIR: 2 unique pins per pair

**Validation:**

- Automated tools: `config/validate.py` for syntax/structure
- BOM detection: Automated UTF-8 BOM corruption checking
- Reference validation: Verify all file paths exist

### Component Containment Types

1. **contained_components**: Direct child components loaded when parent initializes
2. **includes_components**: Dependency components loaded as requirements

### Component Discovery and Inclusion

As each component is found using a depth first walk, the keywords "interface" along with any key of the form xxx_components are considered to be included components. The value of the key is the filename of the component.

---

## 🔗 INTEGRATION POINTS

- **SharedMemory Mesh**: `SharedMemory::write()` broadcasts to all chips
- **Hardware Interfaces**: SPI bus + device pattern, I2S shared bus + unique pins
- **Coordinate Systems**: 2D planar or 3D skull-based with `"reference_point": "nose_center"`
- **PowerShell Tooling**: All scripts lowercase, absolute paths required

---

## 🛠️ PROVEN IMPLEMENTATION PATTERNS (FROM CODE ANALYSIS)

### PATTERN 1: Component Dispatch Tables

**Location**: `component_tables.cpp`

**Architecture**:

- Auto-generated from JSON bot configuration
- Separate tables for init, act, and timing control
- Components called independently through function pointers

**Code Structure**:

```cpp
esp_err_t (*initTable[TABLE_SIZE])(void) = {
    heartbeat_init,
    network_monitor_init
};

void (*actTable[TABLE_SIZE])(void) = {
    heartbeat_act,
    network_monitor_act
};

uint32_t hitCountTable[TABLE_SIZE] = {
    1,  // every 1 loops
    1   // every 1 loops
};
```

### PATTERN 2: Mood-Based Pixel Processing

**Location**: `PIXEL_PROCESSING_IMPLEMENTATION.cpp`

**Architecture**:

- Shared mood state from SharedMemory
- Delta-based color adjustments (optimization)
- RGB565 pixel manipulation with clamping

**Code Structure**:

```cpp
void processPixelsWithMood(uint8_t* currentFrame, uint32_t frame_size) {
    Mood currentGlobalMood = gsm.read<Mood>();
    static Mood oldMood;
    
    if (oldMood != currentGlobalMood) {
        MoodColorDelta totalDelta = calculateTotalMoodDelta(oldMood, currentGlobalMood);
        
        for (uint32_t pixel = 0; pixel < frame_size; pixel++) {
            RGB565Pixel pixelValue = goblin_eye_palette[currentFrame[pixel]];
            pixelValue.applyColorDelta(totalDelta);
            ((uint16_t*)currentFrame)[pixel] = pixelValue.value;
        }
        
        oldMood = currentGlobalMood;
    }
}
```

### PATTERN 3: Bilateral Component Architecture

**Example**: `goblin_left_eye` + `goblin_right_eye` + `goblin_eye` + `gc9a01_display`

- **Positioned Components**: Handle buffer allocation, positioning, SPI binding
- **Creature Logic Component**: Shared mood-based processing
- **Hardware Component**: Display driver interface

---

## ✅ VALIDATION REQUIREMENTS

### Automated Checks

- ASCII-only character validation in all files
- Filename case checking for class files
- Component name global uniqueness verification
- Cross-platform compatibility testing

### Pre-commit Validation

- Scan all files for Unicode characters
- Verify class filename matching
- Validate component naming patterns
- Check subsystem naming conventions

---

## 🚫 PROHIBITED PATTERNS

### Incorrect Hardware Naming

- `goblin_nose_sensor.stl` ❌ (implies goblin-specific sensor hardware)
- `cat_display_mount.stl` ❌ (implies cat-specific display hardware)

### Incorrect Subsystem Naming

- `test_head` ❌ (use `head` for test configurations)
- `goblin_head` ❌ (use `head` regardless of creature type)

---

## 📝 SUMMARY

**Key Principles:**

1. **ASCII Only** - No Unicode, no emojis, no special characters
2. **Case Sensitivity** - Class files match class names exactly
3. **Global Uniqueness** - Component names must be unique across entire system
4. **Component Isolation** - No direct function calls between components
5. **SharedMemory Coordination** - All inter-component communication through shared state
6. **JSON-Driven Configuration** - All components defined in JSON with recursive composition
7. **Two-Tier Architecture** - Universal mounts + creature-specific shells
8. **Real-Time Constraints** - Components must return quickly, no blocking operations

**Enforcement:**

- Automated validation scripts
- Pre-commit checks
- Manual code reviews
- Build system rejection of violations

---

*Last Updated: October 28, 2025*  
*Consolidated from: NAMING_RULES.md, .github/copilot-instructions.md, .github/AI-AGENT-RULES.md, project_coding_standards.md, P32-ARCHITECTURAL-RULES-REVIEW.md, README.md, and code analysis of component_tables.cpp, PIXEL_PROCESSING_IMPLEMENTATION.cpp*
 
 
 
 
