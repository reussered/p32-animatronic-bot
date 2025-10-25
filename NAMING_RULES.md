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

- **NEVER use Unicode characters** in any file content
- **NEVER use emojis** (🚨, 📁, etc.) in scripts or code
- **ASCII ONLY**: Use standard ASCII characters (32-126)
- **Reason**: Unicode causes PowerShell parsing errors and cross-platform issues

### Filename Case Conventions

- **Default**: All filenames must be entirely lowercase
- **Exception**: Files containing class definitions
  - Filename MUST exactly match the class name (case-sensitive)
  - Example: `SharedMemory.hpp` contains `class SharedMemory`
  - Example: `ComponentRegistry.cpp` contains `class ComponentRegistry`
- **Scripts**: Always lowercase (`.ps1`, `.py`, `.sh`)
- **Documentation**: Always lowercase (`.md`, `.txt`)

### Examples

**CORRECT Filenames:**

```bash
emergency_backup_system.ps1     # Script - lowercase
work_protection.ps1             # Script - lowercase
directory_monitor.ps1           # Script - lowercase
project_rules.md                # Documentation - lowercase
SharedMemory.hpp                # Class file - matches class name
ComponentRegistry.cpp           # Class file - matches class name
GoblinPersonality.hpp           # Class file - matches class name
```

**INCORRECT Filenames:**

```bash
Emergency_Backup_System.ps1     # Script with capitals
WORK_PROTECTION.ps1             # Script all caps
Directory-Monitor.ps1           # Script with hyphens and caps
sharedmemory.hpp                # Class file not matching case
componentregistry.cpp           # Class file not matching case
```

## 🧩 COMPONENT NAMING CONVENTIONS

### Component Name Requirements

- `component_name` serves as both human identifier and technical identifier
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

- **Source files**: `{component_name}.cpp` in `src/components/`
- **Header files**: `{component_name}.hpp` in `include/components/`
- **Examples**: `goblin_left_eye.cpp`, `goblin_right_eye.cpp`

## 🏗️ SUBSYSTEM NAMING CONVENTIONS

### JSON-Controlled Subsystem Identification

**Subsystem names are determined by the 'controller' key during recursive JSON composition:**

- Subsystems are identified when JSON components contain the `'controller'` field
- The subsystem name is derived from the component path where the controller assignment occurs
- Generation script discovers subsystems by traversing JSON tree and finding controller assignments

### Subsystem Name Derivation

**Subsystem names are derived directly from the component name that contains the 'controller' field:**

- The subsystem name equals the component name where the `'controller'` field is found
- No predefined standard names - subsystem names are determined by JSON configuration
- Component names become subsystem names when they contain controller assignments

### Generated Subsystem Files

For subsystem named `{subsystem}`:

- `{subsystem}_component_functions.cpp`
- `{subsystem}_component_functions.hpp`
- `{subsystem}_dispatch_tables.cpp`
- `{subsystem}_dispatch_tables.hpp`

### Subsystem File Generation Process

1. **Discovery Phase**: JSON tree traversal finds components with the `'controller'` field
2. **Assignment Phase**: Components are assigned to subsystems through normal JSON containment relationships
3. **File Generation**: Separate files generated for each discovered subsystem
4. **CMakeLists.txt**: Each subsystem gets `{subsystem}_CMakeLists.txt` in `src/`

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

### Configuration Structure

- **Creatures/Bots**: `config\bots\bot_families\{family}\{bot_name}.json`
- **Hardware specs**: `config\hardware\{component_type}.json`
- **Interface definitions**: `config\interfaces\{interface_type}.json`
- **Internal Components**: `config\internal\{component_name}.json`
- **Instrumentation Components**: `config\instrumentation\{Component_Name}.json` - Components that measure and potentially report conditions as they occur in the system
- **Test Components**:
  - Persisted: `config\test\persisted\{ComponentName}.json`
  - Temporary: `config\test\{component_name}.json`
- **Subsystem Components**: `config\subsystems\{subsystem_name}.json` - Components that have the `controller` keyword

### Generated Code Structure

- **Component implementations**: `src\components\{component_name}.cpp`
- **Component headers**: `include\components\{component_name}.hpp`
- **Generated Software**: Created by python scripts in `/tools` go into regular `src/` and `include/` folders. This includes dispatch tables and component functions `{subsystem}_dispatch_tables.hpp/.cpp` and `{subsystem}_component_functions.hpp/.cpp`. Only the last copy generated is kept. Because the folder and files are not in `.gitignore`, this means that the latest will be stored whenever we commit, which is a good thing
- **Shared classes**: `shared\{ClassName}.hpp`

### Component Software Templates

- **Location**: Outside `src/` and `include/` folder hierarchies (e.g., `templates/` or `component_templates/`)
- **Purpose**: Template .cpp/.hpp files designated by the `software` keyword section in component JSON
- **Usage**: These files have .cpp/.hpp extensions but are **NEVER compiled or used directly**
- **Processing**: Become part of `{subsystem}_component_functions.hpp/.cpp` files during generation
- **Naming**: `{component_name}.cpp` and `{component_name}.hpp` (same as regular components)

### Asset Organization

- **3D Models**: `assets\shapes\scad\{category}\{model}.scad`
- **STL Files**: `assets\shapes\stl\{category}\{model}.stl`
- **Animations**: `assets\animations\{creature_family}\{animation_name}.json`
- **Sounds**: `assets\sounds\{creature_family}\{sound_file}.wav`

## 🔗 SHARED STATE NAMING

### SharedMemory Variable Names

- **Case-insensitive**: `g_Mood`, `G_MOOD`, `g_mood` all equivalent
- **Recommended convention**: lowercase with underscores
- **Examples**: `"g_Mood"`, `"g_Envir"`, `"g_Personality"`

### Defined Shared Classes

- **Mood Class**: `"g_Mood"` - 9 emotions (ANGER, FEAR, HAPPINESS, etc.)
- **Environment Class**: `"g_Envir"` - sensor data (distance, temperature, etc.)
- **Personality Class**: `"g_Personality"` - personality configuration (future)

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

- `docs\build_guides\{guide_name}.md`
- Examples: `goblin_head_build_guide.md`

### Architecture Documents

- `docs\{system}_ARCHITECTURE.md`
- `docs\{component}-spec.md`

## 🚫 PROHIBITED NAMING PATTERNS

### Incorrect Hardware Naming (Causes Confusion)

- `goblin_nose_sensor.stl` ❌ - Implies goblin-specific sensor hardware
- `cat_display_mount.stl` ❌ - Implies cat-specific display hardware
- `bear_speaker_bracket.stl` ❌ - Implies bear-specific speaker hardware

### Incorrect Subsystem Naming

- `test_head` ❌ - Use `head` for test configurations
- `goblin_head` ❌ - Use `head` regardless of creature type

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

### Auto-Generated Files (Must be in .gitignore)

- `src/p32_dispatch_tables.cpp`
- `include/p32_dispatch_tables.hpp`
- `src/p32_component_functions.cpp`
- `include/p32_component_functions.hpp`
- `{subsystem}_component_functions.cpp`
- `{subsystem}_dispatch_tables.cpp`

### Generation Command Patterns

- `python tools\generate_tables.py config\bots\bot_families\{family}\{bot}.json src`
- `python tools\generate_individual_components.py`

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

*Last Updated: October 25, 2025*
*Extracted from: ai rules (.github/AI-AGENT-RULES.md), project_coding_standards.md, SUBSYSTEM-GENERATION-RULES.md, component-code-requirements.md (docs/), two-tier-mounting-system.md (docs/), .github/consistant_project_rules.md*

