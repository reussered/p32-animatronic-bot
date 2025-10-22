# P32 Animatronic Bot - AI Coding Agent Instructions

## 🚨 CRITICAL: READ AI-AGENT-RULES.md FIRST 🚨

**Before doing ANYTHING, read and follow: `.github/AI-AGENT-RULES.md`**

These are ironclad rules that must NEVER be broken. They exist because of repeated violations.

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

The core loop in `app_main()` interates once through all of the attached components init functions, then repeatedly through all of the components act functions ONLY- it contains no application logic.

**CRITICAL**: All component functions use **NO ARGUMENTS** - they access `g_loopCount` and all shared state from `p32_shared_state.h` directly.  Those values that are common to all subsystems are read and updated though the SharedMemory class.  This class contains an internal version of the ESP NOW bluetooth system and is used to communicvate between all of the various subsystems controled by a separate esp32 chip.  the protocol for using this code is to declate a local copy of the class variable, thern read its current state.  this read is purely local code and doesn't affect the state of any other subsystem.  when the local code has made any changes to the shared state, it then calls the write function of the class, which updates the local copy and also sends out an ESP NOW message to all other subsystems to update their copies of the shared state.

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
- as an example of how component definitions nest take the example provided by the goblinm head.  the head contains 2 eyes (left/right) 2 ears (left/right), a nose, and a mouth.
goblin_left_eye contains goblin_eye contains gd9a01 while goblin_right_eye also contains goblin_eye contains gc9a01.
the ultimate act table created is
	goblin_left_eye_act
	goblin_right_eye_act
	goblin_nose_act
	goblin_mouth_act
	
	goblin_eye_act
	gcda01_act
	
	the fact that the goblin_eye and gc9a01 components are included twice in the hiearchy is detected and only one act function for each is included.
	the goblin_left_eye loads the aniomation for the left eye into a buffer, the goblin_right_eye creates a second buffer.  as these buffers are alocated, they are pushed ionto a stack.
	the goblin_eye component specifies the goblins eye shape, while its act component acts on every buffer in the stack to modify the frame, by using the Mood class contents.
	the gc9a01_act function looks at each buffer and displays the contents to the interface deined in the specific eye.  SPI_DEVICE_1 for the left eye and SPI_DEVICE_2 for the right.  all of these things are passed down as partb of the task stack created by the higher level components
	
the shape part is defined in goblin_eye.  this is a goblin specific shape.  the gc9a01 shape defines the miounting bracket used for all displays of that type.


this architecture allows creatures with different number of eyes that just 2

### Hardware Abstraction
- Interface definitions separate bus config from device config
- Devices reference buses by ID, enabling bus sharing
- Pin assignments centralized in interface configs

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