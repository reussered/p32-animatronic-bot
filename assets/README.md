# Assets JSON Validation

This document describes the JSON file structure and validation system for the P32 Animatronic Bot.

## Overview

The `assets` folder contains JSON configuration files organized by type:
- `moods/` - Mood state configurations
- `behaviors/` - Behavior pattern definitions
- `animations/` - LED animation sequences

All JSON files are validated for syntax, schema compliance, and consistency.

## Running Validation

To validate all JSON files in the assets folder:

```bash
python3 validate_json.py
```

Or specify a custom path:

```bash
python3 validate_json.py /path/to/assets
```

## JSON File Schemas

### Mood Files (`moods/*.json`)

Mood files define emotional states and their associated behaviors.

**Required Fields:**
- `name` (string): Unique identifier for the mood
- `type` (string): Must be "mood"
- `description` (string): Human-readable description
- `priority` (integer): Priority level (1-10, higher = more important)
- `triggers` (array of strings): Events that activate this mood
- `behaviors` (array of strings): Behaviors associated with this mood
- `led_color` (string): Hex color code for LED display
- `servo_settings` (object):
  - `speed` (integer): Servo speed (0-100)
  - `range` (integer): Servo range in degrees (0-180)

**Example:**
```json
{
  "name": "happy",
  "type": "mood",
  "description": "Happy and cheerful mood",
  "priority": 5,
  "triggers": ["positive_interaction", "music_detected"],
  "behaviors": ["wave", "smile", "dance"],
  "led_color": "#FFD700",
  "servo_settings": {
    "speed": 80,
    "range": 180
  }
}
```

### Behavior Files (`behaviors/*.json`)

Behavior files define specific actions and servo movements.

**Required Fields:**
- `name` (string): Unique identifier for the behavior
- `type` (string): Must be "behavior"
- `description` (string): Human-readable description
- `duration_ms` (integer): Total duration in milliseconds (0-10000)
- `servos` (array of objects): Servo movement sequence
  - Each servo object contains:
    - `id` (integer): Servo identifier
    - `position` (integer): Target position in degrees
    - `time_ms` (integer): Time to reach position in milliseconds

**Optional Fields:**
- `sound_effect` (string or null): Audio file to play during behavior

**Example:**
```json
{
  "name": "wave",
  "type": "behavior",
  "description": "Wave hand/arm gesture",
  "duration_ms": 2000,
  "servos": [
    {"id": 1, "position": 90, "time_ms": 500},
    {"id": 1, "position": 180, "time_ms": 1000},
    {"id": 1, "position": 90, "time_ms": 1500}
  ],
  "sound_effect": "hello.wav"
}
```

### Animation Files (`animations/*.json`)

Animation files define LED light patterns and sequences.

**Required Fields:**
- `name` (string): Unique identifier for the animation
- `type` (string): Must be "animation"
- `description` (string): Human-readable description
- `duration_ms` (integer): Total duration in milliseconds
- `loop` (boolean): Whether animation should loop
- `frames` (array of objects): Animation keyframes
  - Each frame object contains:
    - `time_ms` (integer): Time offset for this frame
    - `leds` (array of objects): LED states for this frame
      - Each LED object contains:
        - `id` (integer): LED identifier
        - `color` (string): Hex color code
        - `brightness` (integer): Brightness level (0-100)

**Example:**
```json
{
  "name": "blink_pattern",
  "type": "animation",
  "description": "LED blink animation",
  "duration_ms": 1000,
  "loop": true,
  "frames": [
    {
      "time_ms": 0,
      "leds": [
        {"id": 0, "color": "#FF0000", "brightness": 100}
      ]
    },
    {
      "time_ms": 500,
      "leds": [
        {"id": 0, "color": "#000000", "brightness": 0}
      ]
    }
  ]
}
```

## Validation Rules

The validator checks for:

1. **Syntax Validation**: All files must be valid JSON
2. **Schema Compliance**: All required fields must be present
3. **Type Checking**: Fields must have the correct data types
4. **Range Validation**: Numeric values must be within specified ranges
5. **Naming Consistency**: Filename should match the `name` field (warning if not)
6. **Nested Structure**: Complex objects are validated recursively

## Adding New JSON Files

When adding new configuration files:

1. Place them in the appropriate subfolder (`moods/`, `behaviors/`, or `animations/`)
2. Ensure the filename matches the `name` field (e.g., `happy.json` for name "happy")
3. Include all required fields for that type
4. Run `python3 validate_json.py` to verify the file is valid
5. Check that there are no errors or warnings

## Extending the Validator

To add validation for a new file type:

1. Open `validate_json.py`
2. Add a new schema definition to the `SCHEMAS` dictionary
3. Define required fields, field types, and any value constraints
4. Run validation to test your new schema
