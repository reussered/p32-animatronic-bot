# Topic 7: JSON Configuration Structure

**Purpose**: Understand how JSON files compose bots and configure components.

---

## Configuration Hierarchy

```
config/bots/bot_families/goblins/
├── goblin_full.json              (root: defines entire creature)
├── subsystems/
│   ├── goblin_head.json          (subsystem: ESP32-S3 head)
│   ├── goblin_torso.json         (subsystem: ESP32-S3 torso)
│   └── goblin_arm_left.json      (subsystem: ESP32-C3 arm)
└── components/
    ├── goblin_eye.json           (component config)
    └── goblin_servo.json         (component config)
```

`generate_tables.py` walks from root → subsystems → components, building dispatch tables.

---

## 1. Root Bot JSON

Defines the complete creature and its subsystems:

```json
{
  "name": "goblin_full",
  "family": "goblins",
  "subsystems": [
    {
      "name": "goblin_head",
      "controller": "ESP32_S3_R8N16",
      "path": "config/bots/bot_families/goblins/subsystems/goblin_head.json"
    },
    {
      "name": "goblin_torso",
      "controller": "ESP32_S3_DEVKITC_1",
      "path": "config/bots/bot_families/goblins/subsystems/goblin_torso.json"
    }
  ]
}
```

**Key fields**:
- `name`: Bot identifier (used in build commands)
- `subsystems`: Array of ESP32 chips comprising the bot
- `controller`: Maps to `platformio.ini` board type
- `path`: Relative to project root

---

## 2. Subsystem JSON

Defines components running on a single ESP32:

```json
{
  "name": "goblin_head",
  "controller": "ESP32_S3_R8N16",
  "components": [
    {
      "component": "goblin_eye_renderer",
      "timing": { "hitCount": 1 },
      "path": "config/bots/bot_families/goblins/components/goblin_eye.json"
    },
    {
      "component": "goblin_servo",
      "timing": { "hitCount": 2 },
      "use_fields": {
        "servo_pin": 18,
        "min_angle": 0,
        "max_angle": 180
      }
    },
    {
      "component": "spi_display_bus",
      "timing": { "hitCount": 2 },
      "path": "config/components/interfaces/spi_display_bus.json"
    }
  ]
}
```

**Key fields**:
- `components`: Array executed in order
- `timing.hitCount`: Execution frequency (see Topic 6)
- `path`: Component JSON location (optional if inline)
- `use_fields`: Inject configuration values (see below)

---

## 3. Component JSON

Can be **inline** (in subsystem JSON) or **external file**:

```json
{
  "component": "goblin_eye_renderer",
  "description": "Renders eye animations to circular display",
  "src_path": "config/bots/bot_families/goblins/components/goblin_eye.src",
  "hdr_path": "config/bots/bot_families/goblins/components/goblin_eye.hdr"
}
```

**Key fields**:
- `src_path`: Path to `.src` file (component logic)
- `hdr_path`: Path to `.hdr` file (function declarations)
- Both paths relative to project root

---

## 4. use_fields: Injecting JSON Values into Components

Components declare `use_fields` to receive configuration from JSON:

```json
{
  "component": "goblin_display",
  "timing": { "hitCount": 2 },
  "use_fields": {
    "display_width": 240,
    "display_height": 240,
    "spi_frequency": 40000000
  }
}
```

### Code Generation: Three Locations

`generate_tables.py` injects code at three points in the aggregated `.cpp`:

#### Location 1: Static Declaration (File Scope)

```cpp
// At top of goblin_head_component_functions.cpp
static uint32_t goblin_display_display_width = 0;
static uint32_t goblin_display_display_height = 0;
static uint32_t goblin_display_spi_frequency = 0;
```

**Pattern**: `static uint32_t {component_name}_{field_name} = 0;`

**Scope**: Accessible to **all components** in subsystem (file scope)

#### Location 2: Assignment in init()

```cpp
esp_err_t goblin_display_init(void)
{
    // First lines injected by generator
    goblin_display_display_width = 240;
    goblin_display_display_height = 240;
    goblin_display_spi_frequency = 40000000;
    
    // Your component logic follows
    ESP_LOGI(TAG, "Display: %dx%d @ %u Hz", 
             goblin_display_display_width,
             goblin_display_display_height,
             goblin_display_spi_frequency);
    return ESP_OK;
}
```

#### Location 3: Assignment in act()

```cpp
void goblin_display_act(void)
{
    // First lines injected by generator (same values)
    goblin_display_display_width = 240;
    goblin_display_display_height = 240;
    goblin_display_spi_frequency = 40000000;
    
    // Your component logic follows
    uint32_t buffer_size = goblin_display_display_width * 
                           goblin_display_display_height * 2;
    render_to_display(buffer, buffer_size);
}
```

**Why assignments in both functions?**
- Ensures values available even if `init()` skipped during testing
- Allows JSON changes to propagate (future hot-reload support)

### Using use_fields in Component Code

```cpp
// In goblin_display.src — NO manual declarations needed
esp_err_t goblin_display_init(void)
{
    // Variables already declared and assigned by generator
    spi_device_interface_config_t cfg = {
        .clock_speed_hz = goblin_display_spi_frequency,  // Use injected value
        // ...
    };
    
    ESP_LOGI(TAG, "Display configured: %ux%u",
             goblin_display_display_width,
             goblin_display_display_height);
    
    return spi_bus_add_device(/* ... */);
}

void goblin_display_act(void)
{
    // Values re-assigned before your code runs
    if (goblin_display_display_width > 0)  // Safe to use
    {
        update_display();
    }
}
```

### use_fields Variable Typing

`use_fields` defines variables with automatic type inference:

- `char*` → field value is quoted string
- `int` → field value is unquoted integer  
- `vartype` → field value is quoted with first char `(` for type casting, e.g.:
  - `"(_uint64_t)42"` → `uint64_t` variable
  - `"(float)14.2"` → `float` variable

When a variable is declared in `use_fields`, the generator:
1. Adds an uninitialized declaration at the beginning of the aggregated `.cpp` file
2. Adds assignment statements after the first bracket of both `init()` and `act()` functions of every component that references that variable in a `use_fields`

**DO**:
- Use valid C identifiers for field names
- Access as `{component_name}_{field_name}`
- These are shared static variables accessible by all components in the subsystem

```cpp
// ✗ DON'T (in .src file)
static uint32_t goblin_display_display_width = 0;  // Duplicate declaration

// ✗ DON'T (in act())
goblin_display_display_width = 320;  // Modifying config value

// ✓ DO
uint32_t width = goblin_display_display_width;  // Read and use
```

---

## 5. Component Paths and Discovery

### Shared Components

Reusable across creatures, stored in `config/components/`:

```json
{
  "component": "spi_display_bus",
  "path": "config/components/interfaces/spi_display_bus.json"
}
```

Structure:
```
config/components/
├── hardware/          (sensors, actuators)
├── drivers/           (device drivers)
├── interfaces/        (communication buses)
├── behaviors/         (AI, decision-making)
└── functional/        (utilities)
```

### Creature-Specific Components

Stored in bot family directory:

```json
{
  "component": "goblin_eye_renderer",
  "path": "config/bots/bot_families/goblins/components/goblin_eye.json"
}
```

### Inline Components

Defined directly in subsystem JSON:

```json
{
  "component": "simple_blinker",
  "timing": { "hitCount": 10 },
  "use_fields": { "led_pin": 13 }
  // No path — generator expects goblin_head/simple_blinker.src/hdr
}
```

---

## 6. JSON Validation Rules

### Required Fields

**Root bot**:
- `name` (string)
- `subsystems` (array)

**Subsystem**:
- `name` (string)
- `controller` (must match `platformio.ini`)
- `components` (array)

**Component**:
- `component` (string, must match `{name}_init` and `{name}_act` functions)
- `timing.hitCount` (integer > 0)

### Optional Fields

- `description` (documentation)
- `use_fields` (configuration injection)
- `path` (external component reference)
- `src_path`/`hdr_path` (explicit file locations)

---

## 7. Build Command Mapping

```powershell
# Regenerate from root bot JSON
python tools/generate_tables.py goblin_full src

# What happens:
# 1. Reads config/bots/bot_families/goblins/goblin_full.json
# 2. For each subsystem in "subsystems" array:
#    - Reads subsystem JSON
#    - For each component:
#      - Reads component JSON (if external)
#      - Loads .src and .hdr files
#      - Aggregates into single .cpp per subsystem
# 3. Generates dispatch tables
# 4. Updates platformio.ini with subsystem environments
```

Generated output:
```
src/subsystems/goblin_head/
├── goblin_head_component_functions.cpp  (aggregated .src files)
├── goblin_head_dispatch_tables.cpp      (init/act tables)
└── goblin_head_main.cpp                 (subsystem entry point)

include/subsystems/goblin_head/
├── goblin_head_component_functions.hpp  (aggregated .hdr files)
├── goblin_head_dispatch_tables.hpp      (table declarations)
└── goblin_head_main.hpp                 (main declarations)
```

---

## 8. Common JSON Mistakes

| Mistake | Symptom | Fix |
|---------|---------|-----|
| `hitCount: 0` | Component never runs | Must be ≥ 1 |
| Invalid `controller` | Build fails, unknown board | Match `platformio.ini` board names |
| Wrong component name | Linker error: undefined reference | Must match `{name}_init`/`{name}_act` |
| Missing `path` for external component | Generation fails | Provide full path from project root |
| Non-numeric `use_fields` value | Generation fails | All values must be numbers |
| Duplicate component names | Multiple definition error | Ensure unique names per subsystem |

---

## 9. Example: Complete Configuration Chain

**Root**: `config/bots/bot_families/goblins/goblin_full.json`
```json
{
  "name": "goblin_full",
  "subsystems": [
    {
      "name": "goblin_head",
      "controller": "ESP32_S3_R8N16",
      "path": "config/bots/bot_families/goblins/subsystems/goblin_head.json"
    }
  ]
}
```

**Subsystem**: `config/bots/bot_families/goblins/subsystems/goblin_head.json`
```json
{
  "name": "goblin_head",
  "components": [
    {
      "component": "goblin_eye",
      "timing": { "hitCount": 1 },
      "use_fields": { "display_width": 240 },
      "path": "config/bots/bot_families/goblins/components/goblin_eye.json"
    }
  ]
}
```

**Component**: `config/bots/bot_families/goblins/components/goblin_eye.json`
```json
{
  "component": "goblin_eye",
  "src_path": "config/bots/bot_families/goblins/components/goblin_eye.src",
  "hdr_path": "config/bots/bot_families/goblins/components/goblin_eye.hdr"
}
```

**Source**: `config/bots/bot_families/goblins/components/goblin_eye.src`
```cpp
esp_err_t goblin_eye_init(void)
{
    // goblin_eye_display_width already declared and assigned
    ESP_LOGI(TAG, "Eye width: %u", goblin_eye_display_width);
    return ESP_OK;
}

void goblin_eye_act(void)
{
    // goblin_eye_display_width re-assigned before this code
    render_eye(goblin_eye_display_width);
}
```

**Generated**: `src/subsystems/goblin_head/goblin_head_component_functions.cpp`
```cpp
#include "subsystems/goblin_head/goblin_head_component_functions.hpp"
// ... auto-includes ...

// Generated static declaration
static uint32_t goblin_eye_display_width = 0;

// --- Begin: config/bots/.../goblin_eye.src ---
esp_err_t goblin_eye_init(void)
{
    goblin_eye_display_width = 240;  // Generated assignment
    
    // Original .src content follows
    ESP_LOGI(TAG, "Eye width: %u", goblin_eye_display_width);
    return ESP_OK;
}

void goblin_eye_act(void)
{
    goblin_eye_display_width = 240;  // Generated assignment
    
    // Original .src content follows
    render_eye(goblin_eye_display_width);
}
// --- End: config/bots/.../goblin_eye.src ---
```

---

**Token count**: ~1,900
