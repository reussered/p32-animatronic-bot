# P32 Animatronic Bot

ESP32-S3 based animatronic system with MOOD-driven behaviors using ESP-IDF framework.

## Overview

This project implements a modular, configuration-driven animatronic robot system featuring:
- **JSON-driven configuration** for hardware components and bot personalities
- **3D skull-based coordinate system** for anatomical accuracy
- **Mood-driven behaviors** with 8 emotional states
- **Two-tier mounting system** for 3D printed components
- **Automated tooling** for configuration validation and shape generation

## Quick Start

### Prerequisites
- PlatformIO with ESP-IDF framework
- Python 3.x (for configuration validation)
- PowerShell (for tooling scripts)
- OpenSCAD (optional, for 3D shape generation)

### Build and Flash
```bash
# Build the project
pio run

# Flash to ESP32-S3
pio run -t upload

# Monitor serial output
pio device monitor
```

### Configuration Validation
```bash
# Validate all JSON configurations
python3 config/validate.py

# Analyze configuration structure
powershell .\generate_file_structure.ps1
```

## Project Structure

```
├── config/                    # JSON configuration files
│   ├── bots/                 # Bot personality definitions
│   ├── components/           # Hardware and interface specs
│   │   ├── hardware/        # Physical device specifications
│   │   ├── interfaces/      # GPIO and bus definitions
│   │   └── positioned/      # Spatially-aware components
├── docs/                      # Architecture documentation
├── src/                       # ESP32 source code
├── tools/                     # PowerShell automation scripts
└── assets/                    # 3D shapes, sounds, animations
    └── shapes/
        ├── scad/             # OpenSCAD source files
        └── stl/              # Generated STL files
```

## Key Features

### Configuration-Driven Design
- Define complete animatronic characters in JSON
- Three-tier component system: hardware → interfaces → positioned
- Automatic validation and consistency checking

### Coordinate System
- 3D skull-based coordinates with nose_center reference (0,0,0)
- Support for both planar 2D and anatomical 3D layouts
- Proportional scaling based on eye spacing

### Hardware Abstraction
- SPI bus sharing for multiple displays (eyes, mouth)
- I2S audio with flexible GPIO mapping
- Centralized pin assignment management

### 3D Shape Generation
```powershell
# Generate mounting system for a bot
.\tools\generate-mounting-system.ps1 -BotType goblin

# Convert to STL for printing
.\tools\generate-stl-files.ps1 -BasicMounts -BotShells

# Preview in browser
.\tools\launch-stl-viewer.ps1 assets/shapes/stl/goblin_eye_shells.stl
```

## Documentation

- [`docs/coordinate-system-spec.md`](docs/coordinate-system-spec.md) - 2D and 3D coordinate systems
- [`docs/two-tier-mounting-system.md`](docs/two-tier-mounting-system.md) - 3D printing architecture
- [`docs/interface-gpio-assignment-spec.md`](docs/interface-gpio-assignment-spec.md) - GPIO pin mappings
- [`docs/openscad-shape-generation-spec.md`](docs/openscad-shape-generation-spec.md) - Shape generation workflow

## GitHub Copilot Support

This repository is configured with comprehensive [GitHub Copilot instructions](/.github/copilot-instructions.md) to assist AI coding agents. The instructions cover:
- Project architecture and design patterns
- Development workflows and tooling
- Code conventions and standards
- Testing and quality guidelines
- Contributing best practices

When using GitHub Copilot or other AI coding assistants, they will automatically reference these instructions for context-aware suggestions.

## Contributing

See the [Copilot Instructions - Contributing Guidelines](/.github/copilot-instructions.md#contributing-guidelines) section for detailed information on:
- Making changes to existing bots
- Adding new bot configurations
- Adding new hardware components
- Code style requirements

Always validate JSON configurations before committing:
```bash
python3 config/validate.py
```

## Current Status

**Hardware Target**: ESP32-S3-DevKitC-1  
**Primary Bot**: Goblin Full (3D skull-based, compressed vertical dimensions)  
**Mood States**: 8 emotional states (FEAR, ANGER, IRRITATION, HAPPINESS, CONTENTMENT, HUNGER, CURIOSITY, AFFECTION)

## Future Roadmap

- Multi-tiered AI: ESP32 (reflexive) → Raspberry Pi (behavioral) → Cloud (conversational)
- Distributed control with multiple ESP32 subsystems
- Modular expansion: arms, hands, mobility base, tentacles
- Full platform mobility with NEMA 17 motors
- Advanced head articulation with stepper motors

## License

[Add license information]

## Contact

[Add contact/author information]
