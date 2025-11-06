# Configuration Files

This directory contains JSON configuration files for the ESP32-based animatronic bot system.

## Structure

The configuration is organized into three main categories:

### 1. Moods (`config/moods/`)

Defines different emotional states and their visual/behavioral characteristics.

**Files:**
- `happy.json` - Cheerful and energetic mood
- `sad.json` - Melancholic and low-energy mood
- `angry.json` - Agitated and intense mood
- `neutral.json` - Default calm and balanced mood

**Schema:**
```json
{
  "name": string,           // Unique identifier
  "displayName": string,    // Human-readable name
  "description": string,    // Description of the mood
  "led": {                  // LED color and pattern
    "color": string,        // Hex color code
    "brightness": number,   // 0-255
    "pattern": string       // Pattern name
  },
  "servo": {                // Servo positions
    "headTilt": number,     // Degrees
    "headPan": number,      // Degrees
    "eyeBrows": number,     // Degrees
    "mouth": number         // Percentage
  },
  "animations": {           // Animation settings
    "idle": string,         // Idle animation name
    "transition": string,   // Transition style
    "speed": number         // Speed multiplier
  },
  "sounds": {               // Audio settings
    "enabled": boolean,
    "volume": number,       // 0-100
    "effects": array        // Sound effect names
  },
  "priority": number,       // Mood priority (1-10)
  "duration": number,       // Duration in ms (0 = indefinite)
  "triggers": array         // Trigger event names
}
```

### 2. Behaviors (`config/behaviors/`)

Defines action sequences and behavioral patterns.

**Files:**
- `greeting.json` - Welcome behavior when detecting new person
- `idle_scan.json` - Periodic scanning behavior when idle
- `celebration.json` - Excited celebration behavior
- `sleep.json` - Low-power sleep behavior

**Schema:**
```json
{
  "name": string,           // Unique identifier
  "displayName": string,    // Human-readable name
  "description": string,    // Description of the behavior
  "type": string,           // "interactive" or "autonomous"
  "sequence": [             // Array of actions
    {
      "action": string,     // Action name
      "duration": number,   // Duration in ms
      "params": object      // Action-specific parameters
    }
  ],
  "moodModifier": object,   // Mood changes (mood: delta)
  "cooldown": number,       // Cooldown time in ms
  "enabled": boolean,
  "triggers": array         // Trigger event names
}
```

### 3. Hardware (`config/hardware/`)

Defines hardware pin configurations and settings.

**Files:**
- `servos.json` - Servo motor configurations
- `leds.json` - LED strip and indicator light configurations
- `sensors.json` - Sensor input configurations
- `audio.json` - Audio output configurations

**Schema:**
```json
{
  "name": string,           // Unique identifier
  "displayName": string,    // Human-readable name
  "description": string,    // Description of the hardware
  "type": string,           // "input", "output", or "actuator"
  "devices": [              // Array of device configurations
    {
      "id": string,         // Device identifier
      "pin": number,        // GPIO pin number
      // ... device-specific properties
    }
  ],
  "enabled": boolean,
  "config": object          // Hardware-specific configuration
}
```

## Consistency Rules

All JSON files within each subfolder must have the same top-level keys to ensure consistency:

- **Moods**: All mood files share the same schema
- **Behaviors**: All behavior files share the same schema
- **Hardware**: All hardware files share the same schema

## Validation

To validate the JSON files:

```bash
# Check JSON syntax
for file in config/*/*.json; do
  python3 -m json.tool "$file" > /dev/null && echo " $file" || echo " $file"
done
```

## Adding New Configurations

When adding new configuration files:

1. Place them in the appropriate subfolder
2. Ensure they follow the schema for that category
3. Include all required top-level keys
4. Validate the JSON syntax
5. Test with the bot system
