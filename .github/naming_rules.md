# Naming Rules

**Note**: This document has been consolidated into `consolidated-rules.md`. Please refer to that file for the latest rules and guidelines.

---

# P32 ANIMATRONIC BOT - COMPREHENSIVE NAMING RULES

## 📋 OVERVIEW

This document contains the **ACTUAL SPECIFIC NAMING RULES** extracted from source documents across the P32 Animatronic Bot project. These are not descriptions - these are the exact rules that must be followed.

## 📖 QUICK NAVIGATION

**Quick section reference:**

- 🔧 General File Naming
- 🧩 Component Naming  
- 🏗️ Subsystem Naming
- 🏭 Hardware & Interface
- 📁 File Organization
- 🔗 Shared State
- 📋 Class Files
- 🛠️ Scripts & Tools
- 📚 Documentation
- 🚫 Prohibited Patterns
- ✅ Validation
- 🔄 Code Generation
- 📝 Summary

---

## �🔧 GENERAL FILE NAMING CONVENTIONS

### ASCII Characters Only (MANDATORY)

- **NEVER use Unicode characters** in any file NAME OR content
- **NEVER use emojis** (🚨, 📁, etc.) in scripts or code
- **ASCII ONLY**: Use standard ASCII characters (32-126)
- **Reason**: Unicode causes PowerShell parsing errors and cross-platform issues

### Filename Case Conventions

- all components and the src/hdr files associated with them are located in the config folder (or subfolder)
- all files that are used by the system are in assets - sound files, animation files, etc.
- **Default**: All filenames must be entirely lowercase
- **Exception**: .hpp Files containing class definitions
  - Filename MUST exactly match the class name (case-sensitive)
  - Example: `SharedMemory.hpp` contains `class SharedMemory`
  - Example: `ComponentRegistry.hpp` contains `class ComponentRegistry`
- **Scripts**: Always lowercase (`.ps1`, `.py`, `.sh`)
- **Documentation**: Always lowercase (`.md`, `.txt`, *.pdf, *.doc)


## 🧩 COMPONENT NAMING CONVENTIONS

### Component Name Requirements

- component names serves as both human identifier and technical identifier
- **Global component name uniqueness** across entire system (MANDATORY)
- Component names MUST be globally unique - no duplicates allowed
- Used in function names: `{component_name}_init(void)`, `{component_name}_act(void)`

### Component Function Signatures

```cpp
// Pattern: {component_name}_{function_type}
esp_err_t component_name_init(void);  // Return esp_err_t
void component_name_act(void);        // Return void, NO ARGUMENTS
```

### Bilateral Components Pattern

- `{creature}_{side}_{component}` (e.g., `goblin_left_eye`)
- Examples: `robot_left_eye`, `goblin_right_ear`, `cyclops_center_eye`

### Component File Structure

- all components have a json file and two source code fragment files that reside in the same folder.  they are names:
{name}.json, {name}.src (for source code fragments) and {name}.hdr (for header code fragments) 
- code fragmentss must have proper C++ syntax but are parts of compilable programs designed to be built into compilable .cpp and .hpp programs

## 🏗️ SUBSYSTEM NAMING CONVENTIONS

### JSON-Controlled Subsystem Identification

**Subsystem names are determined by the 'controller' key during recursive JSON composition:**

- Subsystems are components that contain the `'controller'` key
- Generation script discovers subsystems by traversing JSON tree and finding controller keys

### Subsystem Name Derivation

**Subsystem names are derived directly from the component name that contains the 'controller' field:**

- The subsystem name equals the component name where the `'controller'` field is found
- No predefined standard names - subsystem names are determined by JSON configuration
- Component names become subsystem names when they contain controller keys

### Generated Subsystem Files

subsystems with {name} have the following GENERATED files:

- `src/subsystem/{name}_component_functions.cpp`
- `include/subsystem/{name}_component_functions.hpp`
- `src/subsystem/{name}_dispatch_tables.cpp`
- `include/subsystem/{name}_dispatch_tables.hpp`
- `src/subsystem/{name}_main.cpp`
- `include/subsystem/{name}_main.hpp`
- `src/subsystem/{name}_CMakeLists.txt` 

### Subsystem File Generation Process

1. **Discovery Phase**: JSON tree traversal finds components with the `'controller'` field
2. **Assignment Phase**: Components are assigned to subsystems through normal component containment relationships
3. **File Generation**: Separate files generated for each discovered subsystem
 

## 🏭 HARDWARE & INTERFACE NAMING

### Two-Tier Mounting Architecture

#### Tier 1: Universal Hardware Mounts (Identical Across ALL Creatures)

- `sensor_basic_mount.scad/.stl` - HC-SR04 bracket
- `display_basic_mount.scad/.stl` - GC9A01 bracket
- `speaker_basic_mount.scad/.stl` - 40mm speaker bracket

#### Tier 2: Creature-Specific Decorative Shells

- `goblin_nose_shell.scad/.stl` - Goblin aesthetics + sensor_basic_mount
- `cat_nose_shell.scad/.stl` - Cat aesthetics + sensor_basic_mount
- `bear_nose_shell.scad/.stl` - Bear aesthetics + sensor_basic_mount

### Interface Assignment System

- **Interface Keywords**: Components specify interfaces using the `interface:` keyword
- **Bus Types**: `spi_bus`, `gpio_pair`, `i2c_bus`
- **Example**: `interface: spi_bus` (as used in goblin_eye component)
- **Assignment**: Interface type determines hardware connectivity and communication protocol

## 📁 FILE ORGANIZATION PATTERNS

## 📁 FILE ORGANIZATION PATTERNS

### Component Organization (Reorganized November 2025)

**All components now organized by type and family:**

- **Generic Hardware**: `config/components/hardware/{name}.json|src|hdr`
  - Examples: `gc9a01`, `ssd1306`, `ili9341`, `hc_sr04_sensor`, `mg996r_servo`

- **Driver Components**: `config/components/drivers/{name}.json|src|hdr`
  - Examples: `generic_spi_display`, `spi_display_bus`, `i2c_driver`, `pwm_driver`

- **Interface Definitions**: `config/components/interfaces/{name}.json|src|hdr`
  - Examples: `spi_data_bus`, `gpio_pair_hw506_temperature`

- **Behaviors**: `config/components/behaviors/{name}.json|src|hdr`
  - Examples: `walking_gait_controllers`, `celebration`, `idle_scan`

- **Moods**: `config/components/behaviors/moods/{name}.json`
  - Examples: `angry`, `happy`, `neutral`, `sad`

- **Multi-Family Components** (shared across multiple creature families):
  - Location: `config/bots/multi_family/{subsystem_type}/{name}.json|src|hdr`
  - Types: `humanoid/`, `quadruped/`, `insectoid/`
  - Examples: `humanoid_left_knee`, `humanoid_spine_vertebrae`

- **Creature Family Components** (specific to each creature family):
  - Location: `config/bots/bot_families/{family}/`
  - Family template: `{family}_family.json` (e.g., `goblin_family.json`)
  - Creature variants: `{family}_{variant}.json` (e.g., `goblin_warrior.json`, `bear_arctic.json`)
  - Subsystems: `head/`, `torso/`, `arms/`, `legs/`, `hands/`, `feet/`
  - Components: `{subsystem}/{name}.json|src|hdr`
  - Families: `goblins/`, `bears/`, `cats/`, `dragons/`, `elves/`, `wolves/`, `vampires/`, `zombies/`, `orcs/`, `horror/`, `steampunk/`, `androids/`, `fantasy/`, `undead/`, `humanoids/`, `robots/`, `tests/`
  - Examples: `goblins/head/goblin_left_eye.json`, `bears/torso/bear_speaker.src`

### Generated Code Structure

- **Component implementations**: `src/components/{component_name}.cpp`
- **Component headers**: `include/components/{component_name}.hpp`
- **Subsystem dispatch**: `src/subsystems/{subsystem_name}/{subsystem_name}_dispatch_tables.cpp`
- **Subsystem functions**: `src/subsystems/{subsystem_name}/{subsystem_name}_component_functions.cpp`
- Only the latest generated files are kept (not in `.gitignore`, so latest appears in commits)

### Asset Organization

- **3D Models**: `assets/shapes/scad/{category}/{model}.scad`
- **STL Files**: `assets/shapes/stl/{category}/{model}.stl`
- **Animations**: `assets/animations/{creature_family}/{animation_name}.json`
- **Sounds**: `assets/sounds/{creature_family}/{sound_file}.wav`
- **Manufacturing**: `docs/manufacturing/{document}.json`


### Defined Shared Classes

- **Mood Class**:   9 emotions (ANGER, FEAR, HAPPINESS, etc.)
- **Environment Class**: ` - sensor data (distance, temperature, etc.)
- **Personality Class**:   - personality configuration (future)

## 📋 CLASS FILE NAMING

### C++ Header Files (.hpp)

- Filename MUST match class name exactly (case-sensitive)
- No underscores unless in class name
- Examples:

```cpp
// File: SharedMemory.hpp
class SharedMemory { ... };

// File: ComponentRegistry.hpp
class ComponentRegistry { ... };

// File: P32_Core.hpp
class P32_Core { ... };
```
### master_rule.json
contains information about the system and is located in the root directory.  this file contains the names of every component in the 
### C++ Source Files (.cpp)

- Follow same rules as headers
- Must match corresponding .hpp file case

## 🛠️ SCRIPT AND TOOL NAMING

### PowerShell Scripts

- Always lowercase: `generate_file_structure.ps1`
- Use underscores for multi-word: `work_protection.ps1`
- No hyphens: `directory-monitor.ps1` ❌ → `directory_monitor.ps1` ✅

### Python Scripts

- Always lowercase: `validate_json_improved.py`
- Use underscores: `generate_tables.py`

## 📚 DOCUMENTATION NAMING

### Markdown Files

- Always lowercase: `readme.md`, `project_rules.md`
- Use underscores for multi-word: `component_architecture.md`

### Build Guides

- `docs/build_guides/{guide_name}.md`
- Examples: `goblin_head_build_guide.md`

### Ideas
- not current in use, but reflect ideas that are not being worked on  

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

## 🔄 CODE GENERATION NAMING


### Generation Command Patterns

- in /tools go python and powershell scripts  --> example /tools/generate_tables.py 
 

## 📝 SUMMARY

**Key Principles:**

1. **ASCII Only** - No Unicode, no emojis, no special characters
2. **Case Sensitivity** - Class files match class names exactly
3. **Global Uniqueness** - Component names must be unique across entire system
4. **JSON-Controlled Subsystems** - Subsystem names determined by 'controller' key during recursive composition
5. **Two-Tier Architecture** - Universal mounts + creature-specific shells

**Enforcement:**

- Automated validation scripts
- Pre-commit checks
- Manual code reviews
- Build system rejection of violations

---

*Last Updated: November 3 2025*


