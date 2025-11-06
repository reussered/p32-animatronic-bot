# Naming Rules

**Note**: This document has been consolidated into `consolidated-rules.md`. Please refer to that file for the latest rules and guidelines.

---

# P32 ANIMATRONIC BOT - COMPREHENSIVE NAMING RULES

## ≡ƒôï OVERVIEW

This document contains the **ACTUAL SPECIFIC NAMING RULES** extracted from source documents across the P32 Animatronic Bot project. These are not descriptions - these are the exact rules that must be followed.

## ≡ƒôû QUICK NAVIGATION

**Quick section reference:**

- ≡ƒöº General File Naming
- ≡ƒº⌐ Component Naming  
- ≡ƒÅù∩╕Å Subsystem Naming
- ≡ƒÅ¡ Hardware & Interface
- ≡ƒôü File Organization
- ≡ƒöù Shared State
- ≡ƒôï Class Files
- ≡ƒ¢á∩╕Å Scripts & Tools
- ≡ƒôÜ Documentation
- ≡ƒÜ½ Prohibited Patterns
- Γ£à Validation
- ≡ƒöä Code Generation
- ≡ƒô¥ Summary

---

## ∩┐╜≡ƒöº GENERAL FILE NAMING CONVENTIONS

### ASCII Characters Only (MANDATORY)

- **NEVER use Unicode characters** in any file NAME OR content
- **NEVER use emojis** (≡ƒÜ¿, ≡ƒôü, etc.) in scripts or code
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


## ≡ƒº⌐ COMPONENT NAMING CONVENTIONS

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

## ≡ƒÅù∩╕Å SUBSYSTEM NAMING CONVENTIONS

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
 

## ≡ƒÅ¡ HARDWARE & INTERFACE NAMING

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

## ≡ƒôü FILE ORGANIZATION PATTERNS

### Configuration Structure

 - when picking component names for generating json file, use the shortest poissible name the clearly and uniquely identifies it.
	for example a gc9a01 display would be `gc9a01`
	
- **Creatures/Bots**: each specific creature is identified by a jason file in `config/bots/bot_families/{family}/{name}.json`.  
	- inside each family folder there are folders corresponding to the various subsytems of the {family}  for example in the goblins folder goes folders head/torso/arms/legs/feet/hands.
	- inside each of the folders go all of the creature specific components that belong to that subsyetm, so under goblin/head folder would go goblin_head as well as goblin_left_eye, goblin_eye, goblin_nose, etc.
	- this is a signifigant change from having all creature components in the creature_specific folder.  that placement is obsolete and should never be used
		  
- **Hardware specs**: `config/hardware/{name}.json` - here go components that correspond to the sensors, displays, servos, stepper motors, etc.  example `gc9a01`, `sd1306`
- **Interface definitions**: `config/interfaces/{name}.json`- here go all of the bus components... examples ->`spi_data_bus`, `spi_display_bus`, `adc_bus`, etc.
- **Driver Components**: `config/driver/{name}.json` examples ->  `generic_spi_display`, `generic_spi_data``
 - scripts are in /Tools
 - documentation is in /docs
 - build guides go in /docs/build_guides
 - ideas go into /docs/ideas -> these are things that are thought of from time to time that will form the core of future work.  
 - ideas are entirely off line and are not to be acted on until that phase of the project is enacted.
 
### Generated Code Structure

- **Component implementations**: `src/components/{component_name}.cpp`
- **Component headers**: `include/components/{component_name}.hpp`
- **Generated Software**: Created by python scripts in `/tools` go into regular `src/` and `include/` folders. This includes dispatch tables and component functions `{subsystem}_dispatch_tables.hpp/.cpp` and `{subsystem}_component_functions.hpp/.cpp`. Only the last copy generated is kept. Because the folder and files are not in `.gitignore`, this means that the latest will be stored whenever we commit, which is a good thing
- **Shared classes**: `shared/{ClassName}.hpp`
 
### Asset Organization

- **3D Models**: `assets/shapes/scad/{category}/{model}.scad`
- **STL Files**: `assets/shapes/stl/{category}/{model}.stl`
- **Animations**: `assets/animations/{creature_family}/{animation_name}.json`
- **Sounds**: `assets/sounds/{creature_family}/{sound_file}.wav`


### Defined Shared Classes

- **Mood Class**:   9 emotions (ANGER, FEAR, HAPPINESS, etc.)
- **Environment Class**: ` - sensor data (distance, temperature, etc.)
- **Personality Class**:   - personality configuration (future)

## ≡ƒôï CLASS FILE NAMING

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

## ≡ƒ¢á∩╕Å SCRIPT AND TOOL NAMING

### PowerShell Scripts

- Always lowercase: `generate_file_structure.ps1`
- Use underscores for multi-word: `work_protection.ps1`
- No hyphens: `directory-monitor.ps1` Γ¥î ΓåÆ `directory_monitor.ps1` Γ£à

### Python Scripts

- Always lowercase: `validate_json_improved.py`
- Use underscores: `generate_tables.py`

## ≡ƒôÜ DOCUMENTATION NAMING

### Markdown Files

- Always lowercase: `readme.md`, `project_rules.md`
- Use underscores for multi-word: `component_architecture.md`

### Build Guides

- `docs/build_guides/{guide_name}.md`
- Examples: `goblin_head_build_guide.md`

### Ideas
- not current in use, but reflect ideas that are not being worked on  

## Γ£à VALIDATION REQUIREMENTS

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

## ≡ƒöä CODE GENERATION NAMING


### Generation Command Patterns

- in /tools go python and powershell scripts  --> example /tools/generate_tables.py 
 

## ≡ƒô¥ SUMMARY

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


