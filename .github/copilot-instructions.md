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

## 🔧 Essential Workflows
**Generate Components**: `python tools/generate_tables.py goblin_full src` (reads JSON, creates dispatch tables)

**Build & Flash**: `pio run -t upload -t monitor`

**Validate Config**: `.\generate_file_structure.ps1`

**Multi-Chip Testing**: Use `test/distance_eye_test/` for SharedMemory mesh validation

## � Key Conventions
**ASCII-Only**: ESP-IDF toolchain requires ASCII encoding (NO UTF-8/Unicode)

**Component Names**: Globally unique, used in function names `{name}_init`/`{name}_act`

**Bilateral Pattern**: `goblin_left_eye`, `goblin_right_eye`, `cyclops_center_eye`

**Filename Case**: Lowercase except class files (must match class name: `SharedMemory.hpp`)

**JSON Config**: Include `"relative_filename"`, `"version"`, `"author"` fields

## 🗂️ File Organization
- **Components**: `src/components/{name}.cpp`, `include/components/{name}.hpp`
- **Configs**: `config/bots/bot_families/{family}/{bot}.json`
- **Assets**: `assets/{animations,sounds}/{creature}/`, `assets/shapes/scad/`
- **3D Models**: Tier 1 (universal hardware) + Tier 2 (creature shells)

## ⚡ Critical Constraints
**ESP32-S3**: 512KB RAM, 8MB Flash - no dynamic allocation in hot paths

**Real-Time**: Components must return quickly (<10ms), no blocking operations

**Multi-Chip**: SharedMemory broadcasts via internal mesh protocol, never direct function calls

**Build System**: PlatformIO + ESP-IDF, multiple environments for subsystems

## 🔗 Integration Points
**SharedMemory Mesh**: `SharedMemory::write()` broadcasts to all chips

**Hardware Interfaces**: SPI bus + device pattern, I2S shared bus + unique pins

**Coordinate Systems**: 2D planar or 3D skull-based with `"reference_point": "nose_center"`

**PowerShell Tooling**: All scripts lowercase, absolute paths required

## 🏛️ Component Relationships & JSON Interpretation

**Three-Level Hierarchy**:
- **System Level**: Core platform (WiFi, Serial, Watchdog) - always present
- **Family Level**: Behavior/personality shared across bot family (Goblin, Cat, Bear)  
- **Bot-Specific Level**: Positioned hardware components (eyes, nose, mouth, sensors)

**Recursive Composition**: `goblin_full.json` → `goblin_head.json` → `left_eye.json`

**JSON Config System**:
- **Hardware**: `config/components/hardware/` - Physical device specs
- **Interfaces**: `config/components/interfaces/` - Connection definitions (SPI buses, GPIO)
- **Positioned**: `config/components/positioned/` - Spatially-aware component instances

**JSON Interpretation Rules**:
- Reference other configs: `"author": "config/author.json"`
- Coordinates: `"x": "-1.5 INCH"` (always include units)
- Coordinate systems: `"coordinate_system": "planar_2d"` or `"skull_3d"`
- Required fields: `"relative_filename"`, `"version"`, `"author"`
- Shape parameter: Present for hardware-only components (`"init_function": "STUB"`)