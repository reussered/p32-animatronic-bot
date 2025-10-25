# P32 Animatronic Bot - AI Coding Agent Instructions

## � PROJECT LOCATION (MANDATORY SETUP - FIRST)

**Project Root Directory**: `f:\GitHub\p32-animatronic-bot`

**All file paths are relative to this root directory.** Use absolute paths when running commands or referencing files. This location is fixed and should not change.

## �🚨 CRITICAL: READ AI-AGENT-RULES.md SECOND 🚨

**Before doing ANYTHING, read and follow: `.github/AI-AGENT-RULES.md`**

These are ironclad rules that must NEVER be broken. They exist because of repeated violations.

## 🚨 CRITICAL: READ NAMING_RULES.md THIRD 🚨

**Before doing ANYTHING, read and follow: `NAMING_RULES.md`**

These comprehensive naming rules must be understood before any file creation, modification, or code generation. Naming violations cause build failures and system confusion.

## Project Overview
ESP32-S3 based animatronic system with MOOD-driven behaviors using ESP-IDF framework. Uses JSON-driven configuration for hardware components, bot definitions, and interface specifications.

## CRITICAL: Pure Component-Driven Architecture

**NOTHING executes unless it's a component with init() and act() functions.**

- **System Level**: Core platform (WiFi, Serial, Watchdog) - always present
- **Family Level**: Behavior/personality shared across bot family (Goblin, Cat, Bear)
- **Bot-Specific Level**: Positioned hardware components (eyes, nose, mouth, sensors)

**Components can contain other components**:
- **Creature Assembly** (`goblin_full.json`) contains **Subsystem Assemblies** (`goblin_head.json`, `goblin_torso.json`)
- **Subsystem Assembly** (`goblin_head.json`) contains **Hardware Components** (`left_eye.json`, `nose.json`)
- **A single JSON file defines the entire creature** via recursive composition

**Every creature requires**:
1. Component JSON file with `component_name` and `hitCount` timing
2. Component CPP code with `{name}_init(void)` and `{name}_act(void)` functions **WITH NO ARGUMENTS**. If both init_function and act_function are set to "STUB" in the software section, this indicates a hardware-only component and these functions are not created.
3. Include `p32_shared_state.h` to access `g_loopCount` and all global shared state
4. Attachment at appropriate level (System/Family/Subsystem/Hardware)
5. Registration in generated component tables

The core loop in `app_main()` iterates once through all of the attached components init functions, then repeatedly through all of the components act functions ONLY - it contains no application logic.

**CRITICAL**: All component functions use **NO ARGUMENTS** - they access `g_loopCount` and all shared state from `p32_shared_state.h` directly.

**ESP-NOW Shared State Protocol (CRITICAL FOR MULTI-CHIP SYSTEMS)**:
- **Local Read**: `SharedMemory::read()` gets current local copy of shared state
- **Local Write**: `SharedMemory::write()` updates local copy + broadcasts via ESP-NOW to all other chips
- **No Direct Communication**: Components NEVER call functions on other chips - ONLY through shared state broadcasts
- **State Synchronization**: All chips maintain identical copies of shared state via ESP-NOW mesh
- **Thread Safety**: Shared state reads/writes are atomic operations

**ASCII-Only Encoding**: ESP-IDF toolchain requires ASCII encoding. NO UTF-8, NO Unicode characters in source files.

**Shapes are defined using scad and stl files. If the shape parameter is missing, this component only affects software. The component must have a shape parameter if both init_function and act_function are set to "STUB" (hardware-only component).**

- for example, The goblin_head component has a shape key that specifies `goblin_skull.scad` which defines the mounting framework for eyes/nose/mouth components. The corresponding stl files are also saved into git to reduce creature generation time.

**System components distributed strategically**:
- **Torso subsystem** hosts system-level components (WiFi, ESP-NOW mesh, telemetry)
- **Head subsystem** focuses exclusively on real-time rendering (displays, audio, sensors)
- **Load balanced**: Torso 50% CPU (coordination), Head 75% CPU (rendering)

## Architecture Patterns

### Configuration-Driven Design
- **Bot Definitions**: bots are defined as being part of a specific family.  for example /config/bots/bot_families/goblins contains all of the bots that define different goblins.  each JSON file in the ../goblins folder defines  
		complete animatronic characters with spatial coordinates, mood defaults, and behavior parameters
- **Component Library**: Three-tier system:
  - `config/components/hardware/` - Physical device specs (displays, sensors)  
  - `config/components/interfaces/` - Connection definitions (SPI buses, I2S, GPIO mappings)
  - `config/components/positioned/` - Spatially-aware component instances
- **Reference System**: Use `"author": "config/author.json"` for consistent metadata across all configs

### Coordinate System Convention
Two coordinate systems supported:
- **2D Planar**: `"coordinate_system": "planar_2d"` for flat-panel arrangements
- **3D Skull-Based**: `"coordinate_system": "skull_3d"` for anatomical humanoid faces
- **Reference Origin**: Always `"reference_point": "nose_center"` (0,0,0)
- **Units**: Explicitly specified: `"x": "-1.5 INCH"` (always include unit strings)
- **Scaling**: 3D skull system scales all positions proportionally with eye spacing changes
- **Goblin Full**: Uses compressed skull (30% vertical compression) with 3.0" eye spacing baseline

### Hardware Interface Patterns
- **SPI Devices**: Two-component design - shared bus (`SPI_BUS_VSPI`) + device-specific CS pin (`SPI_DEVICE_N`)
- **I2S Audio**: Same pattern - shared bus (`I2S_BUS_0`) + device-specific data pin (`I2S_DEVICE_N`)
- **GPIO Mapping**: Always specify exact pin numbers, no abstraction layers
- **Pin Efficiency**: SPI uses 3 shared pins + 1 unique CS pin per device (4 pins for 1 display, 5 pins for 2 displays)
- **Bus + Device Architecture**: Bus component defines shared pins, device components reference bus + add unique pins

### Universal Head Architecture (CRITICAL DESIGN PATTERN)
**Skull Framework + Component Modules = Complete Head**
- **Skull Responsibility**: Provides mounting framework at P32 coordinates, creature-specific silhouette
- **Component Responsibility**: Contains electronics + detailed creature features (eye shells, nose shapes, etc.) as ell as the software required to drive the electronics.
- **Standard Mounting**: 26mm eye/mouth rings, 22x17mm nose bracket, creature-specific ear points
- **Modularity**: Same electronics work across all creatures (goblin, cat, bear) via component shell changes
- **File Pattern**: `{creature}_skull.scad` + `{creature}_eye_shells.scad` + `{creature}_nose_shell.scad`

### Two-Tier Component Architecture (NEVER CONFUSE AGAIN)
**TIER 1: Basic Hardware Mounts** - Universal across ALL creatures
- `sensor_basic_mount.scad` → `sensor_basic_mount.stl` (HC-SR04 bracket - IDENTICAL for goblin, cat, bear)
- `display_basic_mount.scad` → `display_basic_mount.stl` (GC9A01 bracket - IDENTICAL for goblin, cat, bear) 
- `speaker_basic_mount.scad` → `speaker_basic_mount.stl` (40mm speaker bracket - IDENTICAL for goblin, cat, bear)

**TIER 2: Decorative Shells** - Unique per creature, incorporates Tier 1 mounts
- `goblin_nose_shell.scad` → Contains warty goblin nose + uses `sensor_basic_mount`
- `cat_nose_shell.scad` → Contains pink triangle nose + uses `sensor_basic_mount`
- `bear_nose_shell.scad` → Contains black button nose + uses `sensor_basic_mount`

**CRITICAL: The "goblin_nose_sensor.stl" file is actually the UNIVERSAL sensor_basic_mount, NOT goblin-specific!**

## Development Workflows

### Build System
- **Primary**: PlatformIO with ESP-IDF framework (`platformio.ini` → `esp32-s3-devkitc-1`)
- **Secondary**: CMake via ESP-IDF (`CMakeLists.txt` includes IDF project setup)
- **Commands**:
  - Build: `pio run`
  - Build & Upload: `pio run -t upload`
  - Build, Upload & Monitor: `pio run -t upload -t monitor`
  - Clean Build: `pio run --clean-first`
  - Specific Environment: `pio run -e test_bot_minimal`

### Component Generation Workflow (CRITICAL)
**NEVER manually create component dispatch tables - ALWAYS use the generator:**

1. **Generate Components**: `python tools/generate_tables.py goblin_full src`
   - Reads `config/bots/bot_families/goblins/goblin_full.json`
   - Recursively loads all contained components
   - Creates `component_tables.cpp` with init/act function pointers
   - Updates `CMakeLists.txt` with exact component file list

2. **Validate Configuration**: `.\generate_file_structure.ps1`
   - Scans all JSON configs for completeness
   - Identifies empty/malformed files
   - Displays component hierarchies

3. **ESP-IDF Alternative**:
   ```bash
   idf.py set-target esp32s3
   idf.py build
   idf.py -p COM3 flash monitor
   ```

### 3D Shape Generation Workflow
- **Generate Mounting System**: `.\tools\generate-mounting-system.ps1 -BotType goblin`
- **Create STL Files**: `.\tools\generate-stl-files.ps1 -BasicMounts -BotShells`
- **View STLs**: `.\tools\launch-stl-viewer.ps1 assets/shapes/stl/file.stl`
- **Two-Tier Architecture**: Basic hardware mounts + character-specific decorative shells
- **Integration**: Uses `use <../basic_mounts/mount.scad>` pattern for shell composition

### Multi-Chip Development
- **Head Subsystem**: `pio run -e goblin_head`
- **Torso Subsystem**: `pio run -e goblin_torso`
- **ESP-NOW Testing**: Use `test/distance_eye_test/` for mesh validation
- **MAC Address Coordination**: Upload head first to get ESP-NOW peer address

### Asset Organization
- **Character-based**: `assets/{animations,sounds}/{bear,cat,cyclops,goblin}/`
- **3D Shapes**: Two-tier mounting system in `assets/shapes/scad/`
  - **Tier 1**: `basic_mounts/` - Generic hardware brackets (display, sensor, speaker)
  - **Tier 2**: `bot_shells/` - Character-specific decorative shells incorporating basic mounts
- **Shape Generation**: Automated OpenSCAD → STL pipeline via PowerShell tools
- **STL Organization**: Generated files in `assets/shapes/stl/{basic_mounts,bot_shells}/`
- **Personality Integration**: Character shells provide unique aesthetics while reusing hardware mounts

## Code Conventions

### ASCII-Only Encoding (MANDATORY)
- **ESP-IDF Toolchain Limitation**: NO UTF-8, NO Unicode characters in ANY source files
- **JSON Files**: Must be ASCII-encoded (toolchain requirement)
- **Source Code**: All .cpp/.hpp files must be ASCII-only
- **Scripts**: No emojis (🚨, 📁, etc.) in PowerShell or Python scripts
- **Violation**: Build failures with cryptic encoding errors

### Filename Case Conventions
- **Default**: All filenames must be entirely lowercase
- **Exception**: Files containing class definitions
  - Filename MUST exactly match the class name (case-sensitive)
  - Example: `SharedMemory.hpp` contains `class SharedMemory`
  - Example: `ComponentRegistry.cpp` contains `class ComponentRegistry`
- **Scripts**: Always lowercase (`.ps1`, `.py`, `.sh`)
- **Documentation**: Always lowercase (`.md`, `.txt`)

### Component Function Architecture (IRONCLAD RULES)
**These rules exist because missing one `return ESP_OK;` stops everything:**

```cpp
// In component_name.hpp:
esp_err_t component_name_init(void);  // MUST return esp_err_t
void component_name_act(void);        // MUST return void

// In component_name.cpp:
esp_err_t component_name_init(void) {
    // ... initialization code ...
    return ESP_OK;  // MANDATORY return statement
}

void component_name_act(void) {
    // ... action code ...
    // No return statement needed
}
```

### Component Name Requirements
- `component_name` serves as both human identifier and technical identifier
- **Global component name uniqueness** across entire system (MANDATORY)
- Component names MUST be globally unique - no duplicates allowed
- Used in function names: `{component_name}_init(void)`, `{component_name}_act(void)`

### Bilateral Components Pattern
- `{creature}_{side}_{component}` (e.g., `goblin_left_eye`)
- Examples: `robot_left_eye`, `goblin_right_ear`, `cyclops_center_eye`

### Component File Structure
- **Source files**: `{component_name}.cpp` in `src/components/`
- **Header files**: `{component_name}.hpp` in `include/components/`
- **Examples**: `goblin_left_eye.cpp`, `goblin_right_eye.cpp`

### Component Isolation - NO Function Calls Between Components
**CRITICAL ARCHITECTURAL PRINCIPLE: Components never ever reference code in other components.**

- Components are **completely isolated** from each other
- **NO component function ever calls another component's function**
- Components communicate ONLY through global shared state (`p32_shared_state.h`)
- Each component reads/writes `g_loopCount`, `g_shared_state`, etc. directly
- The "contains" relationship in JSON is for **build inclusion**, NOT function calls
- Component dispatch system calls `init()` and `act()` functions independently

### JSON Configuration Standards
- Always include: `"relative_filename"`, `"version"`, `"author"` fields
- Use UPPERCASE for types/IDs: `"bot_type": "GOBLINFULL"`, `"interface_id": "SPI_BUS_VSPI"`
- Coordinate objects require `x`, `y`, `z` with unit strings: `"x": "-1.5 INCH"`
- Reference other configs via relative paths: `"author": "config/author.json"`
- ASCII-only encoding (NO UTF-8, NO Unicode)

### File Organization Patterns

#### Configuration Structure
- **Creatures/Bots**: `config/bots/bot_families/{family}/{bot_name}.json`
- **Hardware specs**: `config/hardware/{component_type}.json`
- **Interface definitions**: `config/interfaces/{interface_type}.json`
- **Internal Components**: `config/internal/{component_name}.json`
- **Instrumentation Components**: `config/instrumentation/{Component_Name}.json`
- **Test Components**: `config/test/{component_name}.json`
- **Subsystem Components**: `config/subsystems/{subsystem_name}.json`

#### Generated Code Structure
- **Component implementations**: `src/components/{component_name}.cpp`
- **Component headers**: `include/components/{component_name}.hpp`
- **Shared classes**: `shared/{ClassName}.hpp`

#### Asset Organization
- **3D Models**: `assets/shapes/scad/{category}/{model}.scad`
- **STL Files**: `assets/shapes/stl/{category}/{model}.stl`
- **Animations**: `assets/animations/{creature_family}/{animation_name}.json`
- **Sounds**: `assets/sounds/{creature_family}/{sound_file}.wav`

### Two-Tier Mounting Architecture

#### Tier 1: Universal Hardware Mounts (Identical Across ALL Creatures)
- `sensor_basic_mount.scad/.stl` - HC-SR04 bracket
- `display_basic_mount.scad/.stl` - GC9A01 bracket
- `speaker_basic_mount.scad/.stl` - 40mm speaker bracket

#### Tier 2: Creature-Specific Decorative Shells
- `goblin_nose_shell.scad/.stl` - Goblin aesthetics + sensor_basic_mount
- `cat_nose_shell.scad/.stl` - Cat aesthetics + sensor_basic_mount
- `bear_nose_shell.scad/.stl` - Bear aesthetics + sensor_basic_mount

**CRITICAL: The "goblin_nose_sensor.stl" file is actually the UNIVERSAL sensor_basic_mount, NOT goblin-specific!**

### Shared State Naming
- **Case-insensitive**: `g_Mood`, `G_MOOD`, `g_mood` all equivalent
- **Recommended convention**: lowercase with underscores
- **Examples**: `"g_Mood"`, `"g_Envir"`, `"g_Personality"`
- **Defined Classes**: Mood (9 emotions), Environment (sensor data), Personality (future)

### Prohibited Naming Patterns

#### Incorrect Hardware Naming (Causes Confusion)
- `goblin_nose_sensor.stl` ❌ - Implies goblin-specific sensor hardware
- `cat_display_mount.stl` ❌ - Implies cat-specific display hardware
- `bear_speaker_bracket.stl` ❌ - Implies bear-specific speaker hardware

#### Incorrect Subsystem Naming
- `test_head` ❌ - Use `head` for test configurations
- `goblin_head` ❌ - Use `head` regardless of creature type

#### Incorrect Filename Patterns
- `Emergency_Backup_System.ps1` ❌ - Scripts with capitals
- `WORK_PROTECTION.ps1` ❌ - Scripts all caps
- `Directory-Monitor.ps1` ❌ - Scripts with hyphens and caps
- `sharedmemory.hpp` ❌ - Class file not matching case
- `componentregistry.cpp` ❌ - Class file not matching case

## Key Files Reference

### Core Configuration Files
- `config/author.json` - Project metadata template
- `config/bots/bot_families/goblins/goblin_full.json` - Complete bot example with 3D skull coordinate system
- `config/components/interfaces/spi_bus_vspi.json` - Standard ESP32 SPI bus setup
- `config/components/interfaces/spi_device_*.json` - Individual device GPIO assignments
- `config/components/hardware/gc9a01.json` - Display hardware specifications

### Component Architecture Files
- `P32-COMPONENT-ARCHITECTURE-RULES.md` - Ironclad component function rules
- `docs/THREE-LEVEL-COMPONENT-ATTACHMENT-SPEC.md` - Component attachment specifications
- `AI-AGENT-RULES.md` - Critical AI agent behavioral rules

### Build System Files
- `platformio.ini` - PlatformIO configuration with multiple environments
- `CMakeLists.txt` - ESP-IDF build configuration
- `component_tables.cpp` - Auto-generated dispatch tables (NEVER EDIT MANUALLY)
- `p32_shared_state.h` - Global shared state definitions

### Tooling Scripts (PowerShell)
- `tools/generate_tables.py` - Component table generator (CRITICAL)
- `generate_file_structure.ps1` - Configuration validation
- `tools/generate-mounting-system.ps1` - 3D mounting system generator
- `tools/generate-stl-files.ps1` - OpenSCAD to STL converter

### Universal Architecture Specifications (READ ALL)
**Read all architecture documents automatically using these patterns:**
- `docs/*ARCHITECTURE*.md` - All architecture specification documents
- `docs/*UNIVERSAL*.md` - Universal design patterns for all subsystems
- `docs/*-spec.md` - Technical specifications for all systems
- `docs/*-SPEC.md` - Architecture specifications (capitalized)
- `docs/*diagram*.md` - Wiring and system interconnection diagrams

### Build Instructions
- `COMPLETE-BUILD-GUIDE.md` - Manufacturing & assembly documentation
- `QUICK-BUILD-SUMMARY.md` - Print-ready build summary
- `GOBLIN-HEAD-BUILD-MANUAL.md` - Goblin head assembly manual

## Critical Constraints (Not Obvious From Code)

### ASCII-Only Encoding
- **ESP-IDF Toolchain Limitation**: NO UTF-8, NO Unicode characters in ANY source files
- **JSON Files**: Must be ASCII-encoded (toolchain requirement)
- **Source Code**: All .cpp/.hpp files must be ASCII-only
- **Violation**: Build failures with cryptic encoding errors

### Memory Constraints
- **ESP32-S3**: 512KB RAM, 8MB Flash maximum
- **No Dynamic Allocation**: Avoid `new`/`malloc` in hot paths
- **Stack Size**: Functions limited to ~4KB stack
- **Global Variables**: Preferred over heap allocation

### Real-Time Requirements
- **No Blocking Operations**: Components must return quickly (< 10ms typical)
- **Interrupt Context**: Some functions called from ISRs
- **Timing Critical**: Display updates, sensor polling have strict timing requirements

### PowerShell Tooling
- **Windows Environment**: All tooling scripts are PowerShell (.ps1)
- **Execution Policy**: May need `Set-ExecutionPolicy Bypass` for local scripts
- **Path Handling**: Use absolute paths, avoid relative path assumptions

## Development Notes
- Project uses Windows PowerShell for tooling scripts
- Configuration system designed for rapid prototyping of different animatronic characters
- Mood-driven behavior system suggests AI/ML integration planned
- Hardware targets ESP32-S3 for advanced audio/display capabilities

## Future Architecture Vision
- **Multi-Tiered AI**: ESP32 (reflexive) → Raspberry Pi (behavioral) → Cloud (conversational)
- **Distributed Control**: Pi master controller + multiple ESP32 subsystem controllers
- **Modular Subsystems**: Head/face, arms/hands, mobility base, tentacles - mix and match
- **Full Mobility**: NEMA 17 motors for platform locomotion, 28BYJ-48 for head articulation
- **Expandable Design**: Current focus on goblin head, architecture supports full robot expansion