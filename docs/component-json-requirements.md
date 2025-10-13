# P32 Component JSON Structure Requirements

## Current Status
Building `goblin_simple` bot as initial test case.

## Future JSON Component Requirements
**IMPORTANT**: Eventually every single component JSON file needs to be updated to specify:

1. **Component Name**: `"component_name": "left_eye"` - Used to generate function names
2. **Function Names**: Custom function name overrides if needed
3. **Loop Count Modulus**: `"timing": {"hitCount": 5}` - How often component executes (every N loops)  
4. **Description**: Human-readable description for generated comments
5. **Component Type**: system/positioned/family level categorization

## Standard Function Name Generation Pattern
- Init functions: `p32_{component_name}_init()`
- Action functions: `p32_{component_name}_act(uint32_t loopCount)`

## Component Categories
- **System Level**: Serial, WiFi, Bluetooth - always present, low hitCount values
- **Family Level**: Personality, mood, behavior - mid-range hitCount values  
- **Bot Specific**: Eyes, mouth, sensors - higher frequency, low hitCount values

## Timing Guidelines
- System heartbeat: hitCount = 1 (every loop)
- Network/WiFi: hitCount = 50+ (slow)
- Personality: hitCount = 25 (moderate)
- Mood: hitCount = 10-15 (frequent)
- Animations: hitCount = 3-7 (fast)
- Sensors: hitCount = 10-20 (responsive)

## JSON Structure Template
```json
{
  "relative_filename": "config/components/positioned/component_name.json",
  "component_name": "unique_component_name",
  "description": "Human readable description",
  "timing": {
    "hitCount": 10
  },
  "custom_functions": {
    "init_func": "p32_custom_init", 
    "act_func": "p32_custom_act"
  }
}
```

## Files Updated So Far
- `goblineye_left.json` - left_eye, hitCount: 5
- `goblineye_right.json` - right_eye, hitCount: 5  
- `goblin_mouth.json` - mouth, hitCount: 3
- `goblin_speaker.json` - audio, hitCount: 7
- `goblin_nose.json` - nose_sensor, hitCount: 15

## Next Steps
1. Create `goblin_simple.json` bot configuration
2. Test generator with simple bot
3. Build and verify P32 universal main loop
4. Systematically update all component JSON files