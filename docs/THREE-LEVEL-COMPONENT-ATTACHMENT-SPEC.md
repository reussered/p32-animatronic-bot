# Three-Level Component Attachment Architecture

## CRITICAL DESIGN PRINCIPLE

**NOTHING executes unless it's a component with init() and act() functions.**

The P32 animatronic system uses a **pure component-driven architecture** where:
- ESP32 `app_main()` contains ONLY the component execution loop
- All functionality is implemented as components with `init()` and `act()` functions
- Components are attached at three hierarchical levels
- The core loop iterates through registered components calling their `act()` functions based on `hitCount` timing

## Related Architecture Documents

📘 **[Hierarchical Component Composition](HIERARCHICAL-COMPONENT-COMPOSITION-SPEC.md)** - Components can contain other components (assemblies, subsystems, hardware)

**CRITICAL**: A single JSON file (`goblin_full.json`) defines the entire creature via recursive component composition.

## Core Loop (Immutable)

```c
// Global loop counter (part of synchronized shared state)
uint32_t g_loopCount = 0;

void app_main(void)
{
    // Initialize all components from JSON-generated initTable
    ESP_LOGI(TAG, "Initializing %d components...", INIT_TABLE_SIZE);
    for (int i = 0; i < INIT_TABLE_SIZE; i++) {
        if (initTable[i] != NULL) {
            esp_err_t ret = initTable[i]();  // NO ARGUMENTS
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to initialize component %d", i);
            }
        }
    }
    
    // Main action loop - components execute based on g_loopCount % hitCount
    while (true) {
        for (int i = 0; i < ACT_TABLE_SIZE; i++) {
            if (actTable[i].act_func != NULL && actTable[i].hitCount > 0) {
                if (g_loopCount % actTable[i].hitCount == 0) {
                    actTable[i].act_func();  // NO ARGUMENTS
                }
            }
        }
        g_loopCount++;
        vTaskDelay(pdMS_TO_TICKS(100)); // 100ms loop period
    }
}
```

**This loop NEVER changes.** All functionality is added via components.

## Three-Level Component Hierarchy

### Level 1: System Components (Always Attached)

**Purpose**: Core platform functionality required by all bots regardless of type.

**Characteristics**:
- Always included in every build
- Lowest execution frequency (high hitCount values)
- No bot-specific logic
- Compiled into every firmware image

**Examples**:
- `system_core` - Core system management (hitCount: 100)
- `network_monitor` - WiFi/Bluetooth connectivity (hitCount: 50)
- `serial_console` - Debug/telemetry interface (hitCount: 25)
- `power_monitor` - Battery/voltage monitoring (hitCount: 200)
- `watchdog_heartbeat` - System health checks (hitCount: 500)

**JSON Configuration**: None required - hardcoded in system

**Code Pattern**:
```cpp
// src/components/SystemCore.cpp
#include "SystemCore.hpp"
#include "p32_shared_state.h"  // Access to g_loopCount and all globals

extern "C" esp_err_t SystemCore_init(void) {
    // NO ARGUMENTS - access globals directly
    ESP_LOGI(TAG, "SystemCore initializing...");
    // Initialize core subsystems
    return ESP_OK;
}

extern "C" void SystemCore_act(void) {
    // NO ARGUMENTS - access g_loopCount from global shared state
    if (g_loopCount % 100 == 0) {
        // Check system health every 10 seconds
        ESP_LOGI(TAG, "System health check at loop %u", g_loopCount);
    }
}
```

**Header Pattern**:
```cpp
// include/components/SystemCore.hpp
#ifndef SYSTEM_CORE_HPP
#define SYSTEM_CORE_HPP

#include "esp_err.h"

extern "C" {
    esp_err_t SystemCore_init(void);
    void SystemCore_act(void);
}

#endif // SYSTEM_CORE_HPP
```

### Level 2: Family Components (Shared Across Bot Family)

**Purpose**: Behavior/personality traits shared by all bots in a family (goblin, cat, bear, etc.)

**Characteristics**:
- Defined in `config/bots/bot_families/{family_type}_family.json`
- Mid-range execution frequency (moderate hitCount values)
- Family-specific behavior but no positioned hardware
- Multiple bots can share the same family components

**Examples**:
- `goblin_personality` - Goblin family behavior traits (hitCount: 25)
- `goblin_mood` - 8-mood emotional state machine (hitCount: 15)
- `goblin_behavior_engine` - Action selection logic (hitCount: 10)
- `cat_personality` - Cat family behavior traits (hitCount: 20)
- `cat_curiosity` - Exploration/hunting behaviors (hitCount: 12)

**JSON Configuration**:
```json
{
  "relative_filename": "config/bots/bot_families/fantasy/goblin_family.json",
  "family_type": "GOBLIN",
  "default_components": [
    "config/components/functional/goblin_personality.json",
    "config/components/functional/goblin_mood.json",
    "config/components/functional/goblin_behavior_engine.json"
  ]
}
```

**Code Pattern**:
```c
// src/components/goblin_personality.c
#include "p32_component_config.h"
#include "p32_shared_state.h"  // Access to g_loopCount and all globals

#ifdef ENABLE_GOBLIN_COMPONENTS

esp_err_t goblin_personality_init(void) {
    // NO ARGUMENTS - access globals directly
    ESP_LOGI(TAG, "Goblin personality initializing...");
    // Load goblin behavioral parameters
    return ESP_OK;
}

void goblin_personality_act(void) {
    // NO ARGUMENTS - access g_loopCount from global shared state
    if (g_loopCount % 25 == 0) {
        // Process personality-driven behaviors every 2.5 seconds
        ESP_LOGD(TAG, "Personality update at loop %u", g_loopCount);
    }
}

#endif // ENABLE_GOBLIN_COMPONENTS
```

### Level 3: Bot-Specific Components (Positioned Hardware)

**Purpose**: Individual component instances with spatial positions (eyes, nose, mouth, sensors)

**Characteristics**:
- Defined in `config/bots/{bot_name}.json` positioned_components array
- Highest execution frequency (low hitCount values)
- Each instance has 3D coordinates and hardware interface references
- Directly controls physical hardware (displays, sensors, motors)

**Examples**:
- `goblin_eye_left` - Left eye GC9A01 display at [-26.67, 17.78, -8.89] (hitCount: 5)
- `goblin_eye_right` - Right eye GC9A01 display at [26.67, 17.78, -8.89] (hitCount: 5)
- `goblin_nose` - HC-SR04 sensor at [0, 0, 6.35] (hitCount: 15)
- `goblin_mouth` - Mouth display at [0, -26.67, 0] (hitCount: 3)
- `goblin_speaker` - 40mm speaker audio (hitCount: 7)

**JSON Configuration**:
```json
{
  "relative_filename": "config/components/positioned/goblineye_left.json",
  "component_name": "left_eye",
  "description": "Left eye GC9A01 display",
  "timing": {
    "hitCount": 5
  },
  "hardware": "config/components/hardware/gc9a01_display.json",
  "interface": "config/components/interfaces/spi_device_1.json",
  "position": {
    "coordinate_system": "skull_3d",
    "reference_point": "nose_center",
    "x": "-26.67 mm",
    "y": "17.78 mm",
    "z": "-8.89 mm"
  }
}
```

**Code Pattern**:
```c
// src/components/goblin_eye_left.c
#include "p32_component_config.h"
#include "p32_shared_state.h"  // Access to g_loopCount and all globals

#ifdef ENABLE_GOBLIN_COMPONENTS

esp_err_t goblin_eye_left_init(void) {
    // NO ARGUMENTS - access globals directly
    ESP_LOGI(TAG, "Left eye initializing at [-26.67, 17.78, -8.89]...");
    // Initialize GC9A01 display via SPI_DEVICE_1
    // Load position: [-26.67, 17.78, -8.89] mm from nose_center
    return ESP_OK;
}

void goblin_eye_left_act(void) {
    // NO ARGUMENTS - access g_loopCount and mood from global shared state
    // Update display every 500ms (hitCount: 5 means called every 5 loops)
    
    // Read synchronized mood state
    float happiness = g_shared_state.mood_values[MOOD_HAPPINESS];
    
    // Render current animation frame
    render_eye_with_mood(happiness);
    
    // Send pixel data to GC9A01 via SPI
    ESP_LOGD(TAG, "Eye frame rendered at loop %u", g_loopCount);
}

#endif // ENABLE_GOBLIN_COMPONENTS
```

## Component Registration Flow

### 1. JSON Configuration → Code Generation

**Input**: Bot configuration JSON files define three levels
**Process**: Python script generates component tables
**Output**: `include/p32_component_registry.h` and `src/component_tables.c`

```python
# tools/generate_component_tables.py
def generate_component_tables(bot_config_path):
    bot_config = load_json(bot_config_path)
    
    # Level 1: System components (hardcoded)
    system_components = [
        {"name": "system_core", "hitCount": 100},
        {"name": "network_monitor", "hitCount": 50}
    ]
    
    # Level 2: Family components (from family template)
    family_template = load_json(bot_config["family_template"])
    family_components = []
    for component_path in family_template["default_components"]:
        component = load_json(component_path)
        family_components.append({
            "name": component["component_name"],
            "hitCount": component["timing"]["hitCount"]
        })
    
    # Level 3: Bot-specific positioned components
    bot_components = []
    for component_path in bot_config["positioned_components"]:
        component = load_json(component_path)
        bot_components.append({
            "name": component["component_name"],
            "hitCount": component["timing"]["hitCount"],
            "position": component["position"]
        })
    
    # Generate initTable and actTable
    generate_init_table(system_components + family_components + bot_components)
    generate_act_table(system_components + family_components + bot_components)
```

### 2. Generated Component Tables

**initTable**: Array of initialization functions
```c
// include/p32_component_registry.h (generated)
typedef esp_err_t (*init_func_t)(void);

extern init_func_t initTable[INIT_TABLE_SIZE];

// src/component_tables.c (generated)
init_func_t initTable[INIT_TABLE_SIZE] = {
    system_core_init,           // Level 1: System
    network_monitor_init,       // Level 1: System
    goblin_personality_init,    // Level 2: Family
    goblin_mood_init,           // Level 2: Family
    goblin_eye_left_init,       // Level 3: Bot-specific
    goblin_eye_right_init,      // Level 3: Bot-specific
    goblin_nose_init            // Level 3: Bot-specific
};
```

**actTable**: Array of action functions with timing
```c
// include/p32_component_registry.h (generated)
typedef void (*act_func_t)(void);  // NO ARGUMENTS - uses globals

typedef struct {
    act_func_t act_func;
    uint32_t hitCount;
    const char* name;
} act_table_entry_t;

extern act_table_entry_t actTable[ACT_TABLE_SIZE];

// src/component_tables.c (generated)
act_table_entry_t actTable[ACT_TABLE_SIZE] = {
    {system_core_act,        100, "system_core"},
    {network_monitor_act,     50, "network_monitor"},
    {goblin_personality_act,  25, "goblin_personality"},
    {goblin_mood_act,         15, "goblin_mood"},
    {goblin_eye_left_act,      5, "left_eye"},
    {goblin_eye_right_act,     5, "right_eye"},
    {goblin_nose_act,         15, "nose_sensor"}
};
```

### 3. Compilation and Linking

**Build System**: PlatformIO with ESP-IDF framework
**Preprocessor Guards**: Enable/disable component families
```ini
# platformio.ini
[env:goblin_full]
build_flags = 
    -DENABLE_SYSTEM_COMPONENTS
    -DENABLE_GOBLIN_COMPONENTS
    -DP32_MAX_COMPONENTS=50
```

**Conditional Compilation**:
```c
// All component files wrapped in family guards
#ifdef ENABLE_GOBLIN_COMPONENTS
esp_err_t goblin_eye_left_init(void) { ... }  // NO ARGUMENTS
void goblin_eye_left_act(void) { ... }         // NO ARGUMENTS
#endif
```

## Timing and Execution Frequency

### hitCount Guidelines

| Component Type | hitCount Range | Execution Period | Example |
|---------------|----------------|------------------|---------|
| System Core | 100-500 | 10-50 seconds | Watchdog, health checks |
| Network | 50-100 | 5-10 seconds | WiFi status, telemetry |
| Personality | 20-50 | 2-5 seconds | Behavior decisions |
| Mood | 10-20 | 1-2 seconds | Emotional state updates |
| Sensors | 10-30 | 1-3 seconds | Distance, touch, light |
| Displays | 3-10 | 300ms-1 second | Animation frames |
| Audio | 5-15 | 500ms-1.5 seconds | Sound playback |

**Loop Period**: 100ms (10 Hz)  
**Execution Frequency**: `loopCount % hitCount == 0`

**Examples**:
- `hitCount: 1` → Every loop (100ms) - Use sparingly!
- `hitCount: 5` → Every 500ms (2 Hz) - Good for displays
- `hitCount: 10` → Every 1 second (1 Hz) - Good for sensors
- `hitCount: 50` → Every 5 seconds (0.2 Hz) - Good for network
- `hitCount: 100` → Every 10 seconds (0.1 Hz) - Good for housekeeping

### Performance Considerations

**Total Execution Time Per Loop**:
```
Loop Period (100ms) >= Sum of all component execution times triggered this loop
```

**Example Analysis** (loopCount = 100):
- `system_core_act()` executes (100 % 100 == 0) → ~2ms
- `network_monitor_act()` executes (100 % 50 == 0) → ~5ms
- `goblin_personality_act()` executes (100 % 25 == 0) → ~3ms
- `goblin_mood_act()` executes (100 % 10 == 0) → ~1ms
- `goblin_eye_left_act()` executes (100 % 5 == 0) → ~8ms
- `goblin_eye_right_act()` executes (100 % 5 == 0) → ~8ms
- `goblin_nose_act()` executes (100 % 15 == 0) → ~2ms

**Total**: ~29ms (fits within 100ms budget with 71ms margin)

## Adding New Functionality

### RULE: Everything Must Be a Component

To add ANY new functionality to the system:

1. **Create Component JSON File**
```json
{
  "relative_filename": "config/components/positioned/new_feature.json",
  "component_name": "new_feature",
  "description": "What this component does",
  "timing": {
    "hitCount": 10
  }
}
```

2. **Write Component C Code**
```c
// src/components/new_feature.c
#include "p32_component_config.h"
#include "p32_shared_state.h"  // Access to g_loopCount and all globals

#ifdef ENABLE_{FAMILY}_COMPONENTS

esp_err_t new_feature_init(void) {
    // NO ARGUMENTS - access globals directly
    // Initialize component
    return ESP_OK;
}

void new_feature_act(void) {
    // NO ARGUMENTS - access g_loopCount from global shared state
    // Component logic executes here
    ESP_LOGD(TAG, "Feature executing at loop %u", g_loopCount);
}

#endif
```

3. **Attach Component at Appropriate Level**
- **System Level**: Add to hardcoded system components (rare)
- **Family Level**: Add to `{family}_family.json` default_components
- **Bot Level**: Add to `{bot_name}.json` positioned_components

4. **Regenerate Component Tables**
```bash
python tools/generate_component_tables.py config/bots/my_bot.json
```

5. **Build and Flash**
```bash
pio run -t upload
```

## Examples

### Example 1: Adding LED Strip Component

**Bot-Specific Level** (each bot instance has different LED strip configuration)

```json
// config/components/positioned/goblin_led_strip.json
{
  "relative_filename": "config/components/positioned/goblin_led_strip.json",
  "component_name": "led_strip",
  "description": "WS2812B LED strip for mood lighting",
  "timing": {
    "hitCount": 8
  },
  "hardware": "config/components/hardware/ws2812b_strip.json",
  "interface": "config/components/interfaces/gpio_pair_1.json"
}
```

```c
// src/components/goblin_led_strip.c
#include "p32_shared_state.h"

#ifdef ENABLE_GOBLIN_COMPONENTS

esp_err_t goblin_led_strip_init(void) {
    // NO ARGUMENTS - access globals directly
    // Initialize WS2812B strip via GPIO
    return ESP_OK;
}

void goblin_led_strip_act(void) {
    // NO ARGUMENTS - access g_loopCount and mood from globals
    // Update LED colors based on current mood
    float anger = g_shared_state.mood_values[MOOD_ANGER];
    set_led_color_from_mood(anger);
    // hitCount: 8 → executes every 8 loops = 800ms updates
}

#endif
```

### Example 2: Adding Behavior Planner Component

**Family Level** (shared across all goblins)

```json
// config/components/functional/goblin_behavior_planner.json
{
  "relative_filename": "config/components/functional/goblin_behavior_planner.json",
  "component_name": "behavior_planner",
  "description": "High-level behavior decision making",
  "timing": {
    "hitCount": 30
  }
}
```

Add to family template:
```json
// config/bots/bot_families/fantasy/goblin_family.json
{
  "default_components": [
    "config/components/functional/goblin_personality.json",
    "config/components/functional/goblin_mood.json",
    "config/components/functional/goblin_behavior_planner.json"  // NEW
  ]
}
```

```c
// src/components/goblin_behavior_planner.c
#include "p32_shared_state.h"

#ifdef ENABLE_GOBLIN_COMPONENTS

void goblin_behavior_planner_act(void) {
    // NO ARGUMENTS - access g_loopCount from global shared state
    // Make high-level behavior decisions
    // hitCount: 30 → executes every 30 loops = 3 seconds
    
    // Read sensor data from shared state
    uint8_t distance = g_shared_state.distance_cm;
    
    // Update mood based on sensor input
    if (distance < 30) {
        g_shared_state.mood_values[MOOD_CURIOSITY] = 0.9f;
        mesh_mark_state_dirty();  // Trigger ESP-NOW broadcast
    }
}

#endif
```

### Example 3: Adding Telemetry Component

**System Level** (all bots need telemetry)

Add to system components hardcoded list:
```c
// tools/generate_component_tables.py
system_components = [
    {"name": "system_core", "hitCount": 100},
    {"name": "network_monitor", "hitCount": 50},
    {"name": "telemetry", "hitCount": 75}  // NEW
]
```

```c
// src/components/telemetry.c
#include "p32_shared_state.h"

#ifdef ENABLE_SYSTEM_COMPONENTS

void telemetry_act(void) {
    // NO ARGUMENTS - access g_loopCount and all shared state
    // hitCount: 75 → executes every 75 loops = 7.5 seconds
    
    // Read entire shared state and send via MQTT
    mqtt_publish_json("goblin/status", &g_shared_state);
    
    ESP_LOGI(TAG, "Telemetry sent at loop %u", g_loopCount);
}

#endif
```

## Memory and Performance Optimization

### Component Isolation

Each component can be:
- **Measured independently** for memory footprint
- **Compiled conditionally** via preprocessor guards
- **Excluded from builds** by removing from JSON configuration
- **Profiled separately** for execution time

### Build Size Optimization

```ini
# platformio.ini - Minimal build
[env:goblin_minimal]
build_flags = 
    -DENABLE_SYSTEM_COMPONENTS    # Only system components
    -DP32_MAX_COMPONENTS=10

# platformio.ini - Full build
[env:goblin_full]
build_flags = 
    -DENABLE_SYSTEM_COMPONENTS
    -DENABLE_GOBLIN_COMPONENTS
    -DP32_MAX_COMPONENTS=50
```

### Execution Time Profiling

```c
void goblin_eye_left_act(void) {
    // NO ARGUMENTS - access globals directly
    uint64_t start = esp_timer_get_time();
    
    // Component logic here
    
    uint64_t duration = esp_timer_get_time() - start;
    if (duration > 10000) {  // 10ms threshold
        ESP_LOGW(TAG, "left_eye act() took %lluus at loop %u", 
            duration, g_loopCount);
    }
}
```

## Summary

### Pure Component Architecture Principles

1. **Nothing executes outside components** - `app_main()` contains only the component loop
2. **Three-level hierarchy** - System → Family → Bot-specific attachment
3. **JSON-driven configuration** - All components defined in JSON files
4. **Timing via hitCount** - Components execute when `g_loopCount % hitCount == 0`
5. **Conditional compilation** - Components enabled/disabled via preprocessor guards
6. **Init + Act pattern** - Every component has `init()` and `act()` functions **with NO arguments**
7. **Global state access** - All components access `g_loopCount` and shared state from `p32_shared_state.h`
8. **Distributed synchronization** - ESP-NOW mesh keeps global state synchronized across all subsystems

### Component Development Checklist

- [ ] Create component JSON file with `component_name` and `hitCount`
- [ ] Write component C/C++ code with `init(void)` and `act(void)` functions (NO ARGUMENTS)
- [ ] Include `p32_shared_state.h` to access `g_loopCount` and global shared state
- [ ] Wrap code in appropriate `#ifdef ENABLE_{FAMILY}_COMPONENTS` guard
- [ ] Attach component at correct level (System/Family/Bot-specific)
- [ ] Regenerate component tables from JSON configuration
- [ ] Build and verify component executes at expected frequency
- [ ] Profile component execution time to ensure it fits within loop budget

**All functionality flows through this three-level component system. No exceptions.**
