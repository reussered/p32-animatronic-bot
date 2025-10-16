# P32 Animatronic Bot - AI Coding Agent Instructions

## 🚨 CRITICAL: READ AI-AGENT-RULES.md FIRST 🚨

**Before doing ANYTHING, read and follow: `.github/AI-AGENT-RULES.md`**

These are ironclad rules that must NEVER be broken. They exist because of repeated violations.

## Project Overview
ESP32-S3 based animatronic system with MOOD-driven behaviors using ESP-IDF framework. Uses JSON-driven configuration for hardware components, bot definitions, and interface specifications.

## CRITICAL: Pure Component-Driven Architecture

**READ FIRST**: 
- `docs/THREE-LEVEL-COMPONENT-ATTACHMENT-SPEC.md`
- `docs/HIERARCHICAL-COMPONENT-COMPOSITION-SPEC.md`
- `docs/COMPONENT-FUNCTION-SIGNATURES.md` (NO ARGUMENTS pattern)
- `docs/MESH-STATE-SYNCHRONIZATION-SPEC.md` (Distributed global state)
- `docs/CPP-CLASS-SERIALIZATION-PATTERN.md` (C++ classes + POD serialization)
- `docs/RAW-MEMORY-BLOCK-PATTERN.md` (Simplest memcmp/memcpy pattern)
- `docs/FAST-CHANGE-DETECTION-EXAMPLE.md` (Complete working code)

**NOTHING executes unless it's a component with init() and act() functions.**

- **System Level**: Core platform (WiFi, Serial, Watchdog) - always present
- **Family Level**: Behavior/personality shared across bot family (Goblin, Cat, Bear)
- **Bot-Specific Level**: Positioned hardware components (eyes, nose, mouth, sensors)

**Components can contain other components**:
- **Creature Assembly** (`goblin_full.json`) contains **Subsystem Assemblies** (`goblin_head.json`, `goblin_torso.json`)
- **Subsystem Assembly** (`goblin_head.json`) contains **Hardware Components** (`left_eye.json`, `nose.json`)
- **A single JSON file defines the entire creature** via recursive composition

**Every feature requires**:
1. Component JSON file with `component_name` and `hitCount` timing
2. Component C code with `{name}_init(void)` and `{name}_act(void)` functions **WITH NO ARGUMENTS**
3. Include `p32_shared_state.h` to access `g_loopCount` and all global shared state
4. Attachment at appropriate level (System/Family/Subsystem/Hardware)
5. Registration in generated component tables

The core loop in `app_main()` ONLY iterates through components - it contains no application logic.

**CRITICAL**: All component functions use **NO ARGUMENTS** - they access `g_loopCount` and all shared state from `p32_shared_state.h` directly.

**Shape assemblies are referenced by components**: The head component references `goblin_skull.scad`, which defines the mounting framework for eyes/nose/mouth components.

**System components distributed strategically**:
- **Torso subsystem** hosts system-level components (WiFi, ESP-NOW mesh, telemetry)
- **Head subsystem** focuses exclusively on real-time rendering (displays, audio, sensors)
- **Load balanced**: Torso 50% CPU (coordination), Head 75% CPU (rendering)

📘 **[Distributed Processing Architecture](../docs/DISTRIBUTED-PROCESSING-ARCHITECTURE.md)**
📘 **[Component Function Signatures](../docs/COMPONENT-FUNCTION-SIGNATURES.md)** - NO ARGUMENTS pattern

## Architecture Patterns

### Configuration-Driven Design
- **Bot Definitions**: `config/bots/*.json` define complete animatronic characters with spatial coordinates, mood defaults, and behavior configs
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
- **Component Responsibility**: Contains electronics + detailed creature features (eye shells, nose shapes, etc.)
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
- **Commands**: Use `pio run` for builds, `pio run -t upload` for flashing

### Configuration Analysis
- Run `.\generate_file_structure.ps1` to scan and validate JSON configs
- Script auto-displays JSON contents and identifies empty/malformed files
- Ignores `.pio` build artifacts by default

### 3D Shape Generation Workflow
- **Generate Mounting System**: `.\tools\generate-mounting-system.ps1 -BotType goblin`
- **Create STL Files**: `.\tools\generate-stl-files.ps1 -BasicMounts -BotShells`
- **View STLs**: `.\tools\launch-stl-viewer.ps1 assets/shapes/stl/file.stl`
- **Two-Tier Architecture**: Basic hardware mounts + character-specific decorative shells
- **Integration**: Uses `use <../basic_mounts/mount.scad>` pattern for shell composition

### Asset Organization
- **Character-based**: `assets/{animations,sounds}/{bear,cat,cyclops,goblin}/`
- **3D Shapes**: Two-tier mounting system in `assets/shapes/scad/`
  - **Tier 1**: `basic_mounts/` - Generic hardware brackets (display, sensor, speaker)
  - **Tier 2**: `bot_shells/` - Character-specific decorative shells incorporating basic mounts
- **Shape Generation**: Automated OpenSCAD → STL pipeline via PowerShell tools
- **STL Organization**: Generated files in `assets/shapes/stl/{basic_mounts,bot_shells}/`
- **Personality Integration**: Character shells provide unique aesthetics while reusing hardware mounts

### Testing Guidelines
- **Test Framework**: PlatformIO Unit Testing (native ESP-IDF compatible)
- **Test Location**: `test/` directory for unit tests
- **Test Structure**: Each test should verify component init() and act() functions
- **Hardware Testing**: Use `test/distance_eye_test/` as reference for component integration tests
- **Running Tests**: Execute with `pio test` command
- **Test Coverage**: Focus on component lifecycle (init, act cycle count behavior)
- **Mock Strategy**: Components should be testable independently via shared state access
- **Documentation**: See `test/README` and PlatformIO docs for advanced testing patterns

## Code Conventions

### JSON Configuration Standards
- Always include: `"relative_filename"`, `"version"`, `"author"` fields
- Use UPPERCASE for types/IDs: `"bot_type": "GOBLINFULL"`, `"interface_id": "SPI_BUS_VSPI"`
- Coordinate objects require `x`, `y`, `z` with unit strings
- Reference other configs via relative paths: `"author": "config/author.json"`

### Component Composition
- Bot definitions compose multiple positioned components
- Each component instance maps to hardware definitions via interface assignments
- SPI displays use sequential device assignments: SPI_DEVICE_1 (left eye), SPI_DEVICE_2 (right eye), SPI_DEVICE_3 (mouth)
- Mood system uses 8 emotional states: FEAR, ANGER, IRRITATION, HAPPINESS, CONTENTMENT, HUNGER, CURIOSITY, AFFECTION

### Hardware Abstraction
- Interface definitions separate bus config from device config
- Devices reference buses by ID, enabling bus sharing
- Pin assignments centralized in interface configs

### Error Handling Conventions
- **Return Type**: All init functions return `esp_err_t` for error propagation
- **Success Check**: Use `ESP_ERROR_CHECK()` macro for critical operations that must succeed
- **Error Codes**: Return `ESP_OK` on success, specific error codes on failure
- **Logging**: Use ESP-IDF logging macros: `ESP_LOGI()`, `ESP_LOGW()`, `ESP_LOGE()`
- **Pattern**:
```c
esp_err_t component_init(void) {
    esp_err_t ret = hardware_setup();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Hardware setup failed: %s", esp_err_to_name(ret));
        return ret;
    }
    return ESP_OK;
}
```

## Key Files Reference

### Core Configuration Files
- `config/author.json` - Project metadata template
- `config/bots/goblin_full.json` - Complete bot example with 3D skull coordinate system
- `config/components/interfaces/spi_bus_vspi.json` - Standard ESP32 SPI bus setup
- `config/components/interfaces/spi_device_*.json` - Individual device GPIO assignments

### Universal Architecture Specifications (READ ALL)
**Read all architecture documents automatically using these patterns:**
- `docs/*ARCHITECTURE*.md` - All architecture specification documents
- `docs/*UNIVERSAL*.md` - Universal design patterns for all subsystems  
- `docs/*-spec.md` - Technical specifications for all systems
- `docs/*-SPEC.md` - Architecture specifications (capitalized)
- `docs/*diagram*.md` - Wiring and system interconnection diagrams
- `generate_file_structure.ps1` - Configuration validation tool
- `tools/generate-mounting-system.ps1` - Creates basic mounts + character shells
- `tools/generate-stl-files.ps1` - Batch converts .scad files to .stl for 3D printing
- `tools/launch-stl-viewer.ps1` - Web-based STL viewer for design validation
- `src/main.c` - Currently minimal ESP-IDF entry point

## Performance Considerations
- **CPU Load Balancing**: Torso subsystem 50% CPU, Head subsystem 75% CPU
- **Component Timing**: Use `hitCount` modulo scheduling - avoid real-time delays in act() functions
- **Memory Management**: ESP32-S3 has 512KB SRAM + 2MB PSRAM - monitor allocation patterns
- **SPI Bus Sharing**: Multiple displays share single SPI bus (3 shared pins + unique CS pins)
- **Power Budget**: Document in `docs/POWER-BUDGET-SPEC.md` - track current draw per component
- **Optimization Strategy**: See `docs/ESP32_OPTIMIZATION_STRATEGY.md` for performance tuning
- **Performance Testing**: Reference `docs/P32-PERFORMANCE-TESTING-STRATEGY.md` for measurement methodology
- **Critical**: Never use `vTaskDelay()` or blocking calls in main loop or act() functions

## Dependencies and Tools
- **Platform**: ESP-IDF (Espressif IoT Development Framework) - NOT Arduino
- **Build System**: PlatformIO (primary) or ESP-IDF native CMake (secondary)
- **Target Hardware**: ESP32-S3-DevKitC-1 (dual-core, PSRAM support)
- **JSON Library**: ArduinoJson ^7.0.4 for configuration parsing
- **3D Modeling**: OpenSCAD for parametric shape generation
- **Scripts**: PowerShell for Windows-based development tooling
- **Version Control**: Git with `.gitignore` excluding build artifacts (`.pio/`, `build/`)
- **Hardware Drivers**: Native ESP-IDF APIs (driver/spi_master.h, driver/gpio.h, driver/i2s.h)
- **Firmware Upload**: USB serial @ 921600 baud via PlatformIO or esptool
- **Monitoring**: ESP32 exception decoder filter for crash analysis

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