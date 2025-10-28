# P32 Animatronic Bot - AI Coding Agent Instructions

## 🚨 CRITICAL SETUP (READ FIRST)
**Project Root**: `f:\GitHub\p32-animatronic-bot` (fixed location, use absolute paths)

**MANDATORY**: Read `.github/AI-AGENT-RULES.md` and `NAMING_RULES.md` before any changes.

## 🏗️ Architecture Overview
**Pure Component System**: Everything is a component with `init()`/`act()` functions. Main loop iterates components only - no application logic in `app_main()`.

**Component Pattern**:
```cpp
esp_err_t component_name_init(void);  // Returns ESP_OK
void component_name_act(void);        // No args, accesses g_loopCount/g_shared_state
```

**Communication**: Components NEVER call each other. Use `SharedMemory::read()`/`SharedMemory::write()` for multi-chip coordination.

**Three-Level Component Hierarchy**:
- **System Level**: Core platform (WiFi, Serial, Watchdog) - always present
- **Family Level**: Behavior/personality shared across bot family (Goblin, Cat, Bear)
- **Bot-Specific Level**: Positioned hardware components (eyes, nose, mouth, sensors)

**Recursive JSON Composition**: `goblin_head.json` contains `goblin_left_eye.json` → `goblin_eye.json` → `gc9a01_display.json` (interfaces assigned via `interface_assignment` field)

## 🔧 Essential Workflows
**Generate Components**: `python tools/generate_tables.py goblin_full src` (reads JSON, creates dispatch tables)

**Build & Flash**: `pio run -t upload -t monitor`

**Validate Config**: `.\generate_file_structure.ps1`

**Multi-Chip Testing**: Use `test/distance_eye_test/` for SharedMemory validation

**Build Environments**:
- `simple_display_test`: Basic display validation
- `test_bot_minimal`: Hardware-only testing
- `goblin_head`: Complete head assembly
- `goblin_torso`: Torso components
- `left_arm`, `right_arm`: Bilateral limb control

## 📋 Workflow Reminders and Checklists
**Critical Workflow Rules**:
- **Never compile .src or .hpp files directly**: These are aggregated using `generate_tables.py`. The original .src and .hpp files live in `config/` locations (these are permanent source files); files in `src/` and `include/` are generated/temporary and should not be edited manually.
- **Always run generate_tables.py first**: Execute `python tools/generate_tables.py goblin_full src` before any build to generate dispatch tables from JSON configs.

**Pre-Build Checklist** (Run before any compilation):
- [ ] Execute `python tools/generate_tables.py goblin_full src` to generate dispatch tables from JSON configs
- [ ] Verify JSON configs are updated and mesh references removed if applicable
- [ ] Check CMakeLists.txt includes correct file paths and include directories
- [ ] Confirm .src files are included in p32_component_functions.cpp
- [ ] Validate component headers exist in include/components/

**Common Oversights to Avoid**:
- Forgetting to run generate_tables.py before builds (generates component_tables.cpp, etc.)
- Using incorrect PlatformIO config (use platformio_multi_variant.ini for goblin_head)
- Assuming .src files are compiled separately (they are included in p32_component_functions.cpp)
- Missing include paths in CMakeLists.txt (add INCLUDE_DIRS for include/ and shared/)

**Validation Steps**:
- Run `.\generate_file_structure.ps1` to check config integrity
- Use `pio run -c platformio_multi_variant.ini -e <env>` for builds
- Check build output for undefined references or missing headers

## � Key Conventions
**ASCII-Only**: ESP-IDF toolchain requires ASCII encoding (NO UTF-8/Unicode)

**Component Names**: Globally unique, used in function names `{name}_init`/`{name}_act`

**Bilateral Pattern**: `goblin_left_eye`, `goblin_right_eye`, `cyclops_center_eye`

**Filename Case**: Lowercase except class files (must match class name: `SharedMemory.hpp`)

**JSON Config**: Include `"relative_filename"`, `"version"`, `"author"` fields

**SharedMemory API**:
```cpp
// Reading shared state (local, fast)
Mood *mood = GSM.read<Mood>();
Environment *env = GSM.read<Environment>();

// Modifying state in place
mood->anger = 75;
env->temperature = 25.5;

// Writing triggers inter-chip sync
GSM.write<Mood>();      // Broadcasts to all chips
GSM.write<Environment>();
```

## 🗂️ File Organization
- **Components**: `src/components/{name}.cpp`, `include/components/{name}.hpp`
- **Configs**: `config/bots/bot_families/{family}/{bot}.json`
- **Assets**: `assets/{animations,sounds}/{creature}/`, `assets/shapes/scad/`
- **3D Models**: Tier 1 (universal hardware) + Tier 2 (creature shells)
- **Generated Files**: `src/subsystems/{subsystem}/{subsystem}_dispatch_tables.cpp`

## ⚡ Critical Constraints
**ESP32-S3**: 512KB RAM, 8MB Flash - no dynamic allocation in hot paths

**Real-Time**: Components must return quickly (<10ms), no blocking operations

**Multi-Chip**: SharedMemory broadcasts to all chips, never direct function calls

**Build System**: PlatformIO + ESP-IDF, multiple environments for subsystems

**Dispatch Tables**: Auto-generated from JSON, never manually edit C++ files

## 🔗 Integration Points
**SharedMemory**: `GSM.write<T>()` broadcasts to all chips

**Hardware Interfaces**: SPI bus + device pattern, I2S shared bus + unique pins

**Coordinate Systems**: 2D planar or 3D skull-based with `"reference_point": "nose_center"`

**PowerShell Tooling**: All scripts lowercase, absolute paths required

**JSON Interpretation Rules**:
- Reference other configs: `"author": "config/author.json"`
- Coordinates: `"x": "-1.5 INCH"` (always include units)
- Coordinate systems: `"coordinate_system": "planar_2d"` or `"skull_3d"`
- Required fields: `"relative_filename"`, `"version"`, `"author"`
- Shape parameter: Present for hardware-only components (`"init_function": "STUB"`)
- Interface assignment: `"interface_assignment": "spi_bus_vspi"` (generic bus component, GPIO pins assigned dynamically at runtime)