# P32 Animatronic Bot - AI Coding Agent Instructions

## Project Overview
ESP32-S3 based animatronic system with MOOD-driven behaviors using ESP-IDF framework. Uses JSON-driven configuration for hardware components, bot definitions, and interface specifications.

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

## Development Workflows

### Build System
- **Primary**: PlatformIO with ESP-IDF framework (`platformio.ini` → `esp32-s3-devkitc-1`)
- **Secondary**: CMake via ESP-IDF (`CMakeLists.txt` includes IDF project setup)
- **Commands**: Use `pio run` for builds, `pio run -t upload` for flashing

### Configuration Analysis
- Run `.\generate_file_structure.ps1` to scan and validate JSON configs
- Script auto-displays JSON contents and identifies empty/malformed files
- Ignores `.pio` build artifacts by default

### Asset Organization
- **Character-based**: `assets/{animations,sounds}/{bear,cat,cyclops,goblin}/`
- **3D Shapes**: `assets/shapes/{scad,stl,descriptions}/` for printable components
- **Shape Generation**: OpenSCAD templates + automated STL generation from hardware/coordinate configs
- **Personality Integration**: Shape modifications (broken horns) directly affect bot personality traits
- Assets organized by bot type for easy character swapping

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

## Key Files Reference
- `config/author.json` - Project metadata template
- `config/bots/goblin_full.json` - Complete bot example with 3D skull coordinate system
- `config/components/interfaces/spi_bus_vspi.json` - Standard ESP32 SPI bus setup
- `config/components/interfaces/spi_device_*.json` - Individual device GPIO assignments
- `docs/coordinate-system-spec.md` - Detailed specification for both 2D and 3D coordinate systems
- `docs/interface-gpio-assignment-spec.md` - GPIO assignment and interface architecture
- `docs/goblin-full-interconnection-diagram.md` - Complete wiring diagram with all GPIO assignments
- `docs/openscad-shape-generation-spec.md` - Automated STL generation system using OpenSCAD
- `generate_file_structure.ps1` - Configuration validation tool
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