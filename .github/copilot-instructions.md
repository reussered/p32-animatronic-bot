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
- `docs/two-tier-mounting-system.md` - Architecture for separating hardware mounts from character aesthetics
- `generate_file_structure.ps1` - Configuration validation tool
- `tools/generate-mounting-system.ps1` - Creates basic mounts + character shells
- `tools/generate-stl-files.ps1` - Batch converts .scad files to .stl for 3D printing
- `tools/launch-stl-viewer.ps1` - Web-based STL viewer for design validation
- `src/main.c` - Currently minimal ESP-IDF entry point

## Error Handling and Debugging

### Common Issues and Solutions
- **JSON Validation Errors**: Run `python3 config/validate.py` to identify malformed configs
- **Build Failures**: Check `platformio.ini` matches your board, verify ESP-IDF installation
- **GPIO Conflicts**: Review `docs/goblin-full-interconnection-diagram.md` for pin assignments
- **OpenSCAD Errors**: Verify OpenSCAD is installed and in PATH for shape generation

### Debugging Approaches
- **Configuration Issues**: Use `.\generate_file_structure.ps1` to inspect all JSON files
- **Hardware Testing**: Flash individual components before full bot assembly
- **3D Printing**: Generate and preview STL files before printing with `.\tools\launch-stl-viewer.ps1`
- **Serial Debugging**: Use `pio device monitor` for ESP32 serial output

### Safety and Validation
- **Always validate JSON** before committing to prevent broken configs
- **Test shapes virtually** before 3D printing to save material
- **Check GPIO assignments** to prevent hardware damage from pin conflicts
- **Incremental testing** - build and test one component at a time

## Development Notes
- Project uses Windows PowerShell for tooling scripts
- Configuration system designed for rapid prototyping of different animatronic characters
- Mood-driven behavior system suggests AI/ML integration planned
- Hardware targets ESP32-S3 for advanced audio/display capabilities
- Development primarily on Windows, but configs are cross-platform
- 3D printing workflow integrated into development process

## Testing and Quality Standards

### Configuration Validation
- **Python Validator**: Run `python3 config/validate.py` to validate all JSON files
- **Validation Checks**: JSON syntax validation and schema consistency within folders
- **Pre-commit**: Always validate configs before committing changes
- **Schema Consistency**: All JSON files in same folder must have identical top-level keys

### Code Quality
- **PlatformIO Tests**: Test directory structure follows PlatformIO Unit Testing framework
- **Documentation**: Update relevant docs when changing architecture or adding features
- **PowerShell Scripts**: Use `Write-Host` with color coding for user feedback
- **Error Handling**: Scripts should validate inputs and provide clear error messages

### File Naming Conventions
- **JSON Configs**: Use snake_case: `goblin_full.json`, `spi_bus_vspi.json`
- **PowerShell Scripts**: Use kebab-case with verb-noun pattern: `generate-mounting-system.ps1`
- **Markdown Docs**: Use kebab-case: `coordinate-system-spec.md`
- **OpenSCAD Files**: Match bot/component names: `goblin_eye_shells.scad`

## Contributing Guidelines

### Making Changes
- **Minimal Changes**: Make surgical, focused changes that don't break existing functionality
- **Documentation First**: Update architecture docs before implementing major features
- **Test After Changes**: Validate JSON configs and test builds after modifications
- **Commit Messages**: Use clear, descriptive commit messages explaining the "why"

### Adding New Bots
1. Create JSON config in `config/bots/` following existing schema
2. Define positioned components in `config/components/positioned/`
3. Generate 3D shapes with `.\tools\generate-mounting-system.ps1 -BotType <name>`
4. Create asset directories: `assets/{animations,sounds}/<bot_name>/`
5. Validate with `python3 config/validate.py`

### Adding New Hardware Components
1. Define hardware specs in `config/components/hardware/`
2. Create interface definition in `config/components/interfaces/`
3. Update GPIO assignment documentation in `docs/`
4. Test with existing bot configurations

### Code Style
- **C/C++ (ESP-IDF)**: Follow ESP-IDF coding standards
- **Python**: Follow PEP 8, use type hints
- **PowerShell**: Use approved verbs, parameter validation, meaningful variable names
- **JSON**: 2-space indentation, always include required metadata fields

## Future Architecture Vision
- **Multi-Tiered AI**: ESP32 (reflexive) → Raspberry Pi (behavioral) → Cloud (conversational)
- **Distributed Control**: Pi master controller + multiple ESP32 subsystem controllers
- **Modular Subsystems**: Head/face, arms/hands, mobility base, tentacles - mix and match
- **Full Mobility**: NEMA 17 motors for platform locomotion, 28BYJ-48 for head articulation
- **Expandable Design**: Current focus on goblin head, architecture supports full robot expansion