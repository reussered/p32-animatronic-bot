# p32-animatronic-bot
ESP32-based animatronic BOT system with MOOD-driven behaviors

## Overview

This project implements an animatronic bot system controlled by an ESP32 microcontroller. The bot features mood-driven behaviors with configurable JSON files for moods, behaviors, and LED animations.

## Project Structure

```
p32-animatronic-bot/
├── assets/                     # JSON configuration files
│   ├── moods/                 # Mood state definitions
│   ├── behaviors/             # Behavior pattern definitions
│   ├── animations/            # LED animation sequences
│   └── README.md              # Asset documentation
├── validate_json.py           # JSON validation script
├── test_validation.py         # Validation test suite
└── README.md                  # This file
```

## JSON Asset Validation

All JSON configuration files in the `assets/` folder are validated for syntax, schema compliance, and consistency.

### Running Validation

To validate all JSON files:

```bash
python3 validate_json.py
```

The validator recursively checks all JSON files in the assets folder and subdirectories, ensuring:
- Valid JSON syntax
- Required fields are present
- Correct data types
- Values are within valid ranges
- Consistent naming conventions

### Running Tests

To run the validation test suite:

```bash
python3 test_validation.py
```

This runs comprehensive tests including:
- Valid file validation
- Detection of missing fields
- Type checking
- Range validation
- Nested object validation
- Recursive directory validation

## Asset Files

The `assets/` folder contains three types of JSON configuration files:

- **Moods** (`moods/*.json`) - Define emotional states with associated behaviors, LED colors, and servo settings
- **Behaviors** (`behaviors/*.json`) - Define specific actions with servo movement sequences
- **Animations** (`animations/*.json`) - Define LED light patterns and sequences

See [assets/README.md](assets/README.md) for detailed schema documentation and examples.

## Contributing

When adding new configuration files:

1. Place files in the appropriate `assets/` subfolder
2. Follow the schema documented in `assets/README.md`
3. Run `python3 validate_json.py` to ensure validity
4. Run `python3 test_validation.py` to verify the validation system still works

## License

This project is open source.