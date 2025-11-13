# 03. Component System Rules

**Read Time**: 4 minutes | **Criticality**: ⚠️ BUILD-BREAKING if violated

---

## Standard Includes (Automatic)

⚠️ **DO NOT manually `#include` these in your `.src` files** - they're auto-injected:

**Always available in every component:**
```cpp
// Core infrastructure (auto-included)
#include "core/memory/SharedMemory.hpp"        // GSM access
#include "with.hpp"                            // Utility macros
#include "esp_random.h"                        // ESP32 random functions

// Shared state classes (auto-included)
#include "shared/BalanceCompensation.hpp"
#include "shared/BehaviorControl.hpp"
#include "shared/CollisionAvoidance.hpp"
#include "shared/EmergencyCoordination.hpp"
#include "shared/Environment.hpp"
#include "shared/FrameProcessor.hpp"
#include "shared/ManipulationControl.hpp"
#include "shared/MicrophoneData.hpp"
#include "shared/Mood.hpp"
#include "shared/Personality.hpp"
#include "shared/SensorFusion.hpp"
#include "shared/SysTest.hpp"

// Shared type definitions (auto-included)
#include "config/shared_headers/color_schema.hpp"
#include "config/shared_headers/PixelType.hpp"
#include "config/shared_headers/represents.hpp"
```

**You can directly use:**
- `GSM.read<Mood>()` / `GSM.write<Mood>()` - Global Shared Memory
- `Mood`, `Personality`, `Environment`, etc. - State classes
- `PixelType`, `color_schema` - Display types
- ESP-IDF functions - Standard ESP32 APIs

**Only `#include` in your `.src` when you need:**
- Hardware-specific drivers: `#include "driver/spi_master.h"`
- ESP-IDF features not auto-included: `#include "esp_log.h"`
- Component-specific types from other components (rare, use static variables instead)

---

## Component Lifecycle

Every component has exactly 2 functions:

```cpp
esp_err_t component_name_init(void);  // Called once at startup
void component_name_act(void);         // Called every N loops based on hitCount
```

**NO exceptions**. No arguments, no return from `act()`.

---

## Function Signatures (Immutable)

❌ **NEVER** add parameters or change return types:

```cpp
// ❌ WRONG - parameters not allowed
void component_act(int param) { }

// ❌ WRONG - return type on act()
int component_act(void) { return 0; }

// ❌ WRONG - missing void
esp_err_t component_init() { }

// ✅ CORRECT - exact signatures
esp_err_t component_init(void) { return ESP_OK; }
void component_act(void) { }
```

**Why**: Dispatch tables expect exact function pointer types. Wrong signature = compilation failure.

---

## File Structure Rules

Every component has 3 files (optional `.hdr`):

```
config/components/hardware/servo_driver.json  # Component definition
config/components/hardware/servo_driver.src   # Implementation
config/components/hardware/servo_driver.hdr   # Declarations (optional)
```

### .json File
```json
{
  "name": "servo_driver",
  "description": "Servo motor controller",
  "timing": {
    "hitCount": 10
  }
}
```

### .hdr File (declarations only)
```cpp
// Function prototypes
esp_err_t servo_driver_init(void);
void servo_driver_act(void);

// Type definitions
typedef struct {
    uint8_t pin;
    uint16_t angle;
} servo_config_t;

// Constants
#define SERVO_MAX_ANGLE 180
```

### .src File (implementation)
```cpp
// Static variables at file scope (shared with other components in same subsystem)
static servo_config_t servo_state;

esp_err_t servo_driver_init(void) {
    servo_state.pin = 5;
    servo_state.angle = 90;
    // Hardware init here
    return ESP_OK;
}

void servo_driver_act(void) {
    if (g_loopCount % 10 == 0) {
        // Update servo position
    }
}
```

---

## The Golden Rules

### Rule 1: NO #include of .hdr in .src

❌ **NEVER DO THIS:**
```cpp
// In servo_driver.src
#include "components/hardware/servo_driver.hdr"  // WRONG!
```

**Why**: `generate_tables.py` aggregates ALL `.hdr` content into the subsystem `.hpp` file, which is already `#include`'d at the top of the generated `.cpp`. Including it again causes multiple definition errors.

**What happens**: Build script automatically strips these (converts to comment), but don't rely on it.

### Rule 2: NO extern in .src files

❌ **NEVER DO THIS:**
```cpp
// In component_a.src
extern int shared_value;  // WRONG!

// In component_b.src
extern int shared_value;  // WRONG!
```

✅ **DO THIS:**
```cpp
// At top of FIRST .src file that needs it
static int shared_value = 0;

// component_a.src
void component_a_act(void) {
    shared_value = 10;  // Write
}

// component_b.src (same subsystem)
void component_b_act(void) {
    int val = shared_value;  // Read
}
```

**Why**: All `.src` files in a subsystem compile into ONE `.cpp` file. They share file scope. Using `extern` creates linker errors.

### Rule 3: NO direct component calls

❌ **NEVER DO THIS:**
```cpp
void component_a_act(void) {
    component_b_act();  // WRONG!
}
```

✅ **DO THIS:**
```cpp
// Use static variables for coordination
static bool trigger_b = false;

void component_a_act(void) {
    trigger_b = true;  // Signal component B
}

void component_b_act(void) {
    if (trigger_b) {
        // Do something
        trigger_b = false;
    }
}
```

**Why**: Components are isolated. Main loop calls them, not each other.

### Rule 4: Static variables for same-subsystem communication

✅ **ALWAYS declare at file scope (top of file):**
```cpp
// At top of .src file, outside all functions
static uint16_t* display_buffer = NULL;
static uint32_t buffer_size = 0;
static bool buffer_ready = false;

esp_err_t component_init(void) {
    display_buffer = (uint16_t*)malloc(57600);
    buffer_size = 57600;
    return ESP_OK;
}

void component_act(void) {
    if (display_buffer != NULL) {
        // Use buffer
    }
}
```

**Why**: File-scoped `static` variables are shared across all components in the same subsystem (same compilation unit).

### Rule 5: GSM for cross-subsystem communication

When components are on **different chips**, use Global Shared Memory:

```cpp
// Component on HEAD chip
void head_component_act(void) {
    Mood* mood = GSM.read<Mood>();
    if (mood->curiosity > 50) {
        // Adjust eye behavior
    }
}

// Component on TORSO chip
void torso_component_act(void) {
    Mood* mood = GSM.read<Mood>();
    mood->curiosity += 10;
    GSM.write<Mood>();  // Sync to all chips via ESP-NOW
}
```

**Why**: ESP-NOW automatically synchronizes GSM writes across all ESP32 chips.

---

## Component Timing

### hitCount Mechanism

Components execute when `g_loopCount % hitCount == 0`:

```cpp
// In JSON
{
  "timing": {
    "hitCount": 10
  }
}

// Executes every 10 loops
void component_act(void) {
    // This runs when g_loopCount is 0, 10, 20, 30, 40...
}
```

### Timing Guidelines

| Component Type | hitCount Range | Frequency | Use Case |
|----------------|----------------|-----------|----------|
| Displays | 1-5 | Every loop or 5 loops | Smooth animation |
| Sensors | 10-30 | Every 1-3 seconds | Responsive input |
| Mood/Behavior | 10-20 | Every 1-2 seconds | State updates |
| Personality | 20-50 | Every 2-5 seconds | Slow decisions |
| Network | 50-100 | Every 5-10 seconds | Telemetry |
| System | 100-500 | Every 10-50 seconds | Health checks |

**Lower hitCount = runs more often = higher CPU load**

---

## Init Function Best Practices

```cpp
esp_err_t component_init(void) {
    // 1. Validate prerequisites
    if (some_dependency == NULL) {
        return ESP_FAIL;
    }
    
    // 2. Allocate resources
    display_buffer = (uint16_t*)malloc(buffer_size);
    if (display_buffer == NULL) {
        return ESP_FAIL;
    }
    
    // 3. Initialize hardware
    esp_err_t err = configure_spi_bus();
    if (err != ESP_OK) {
        free(display_buffer);
        return err;
    }
    
    // 4. Set initial state
    component_state = READY;
    
    return ESP_OK;
}
```

**Init failures halt the boot sequence**. Always validate and report errors.

---

## Act Function Best Practices

```cpp
void component_act(void) {
    // 1. Early exit if not ready
    if (!component_ready) {
        return;
    }
    
    // 2. Timing checks (if needed beyond hitCount)
    if (g_loopCount % sub_timing == 0) {
        // Sub-frequency logic
    }
    
    // 3. Read inputs (sensors, GSM, static variables)
    uint16_t sensor_value = read_sensor();
    Mood* mood = GSM.read<Mood>();
    
    // 4. Process logic
    if (sensor_value > threshold && mood->alertness > 50) {
        update_display();
    }
    
    // 5. Write outputs (hardware, GSM, static variables)
    GSM.write<Mood>();
}
```

**Keep act() fast**. No blocking calls, no long loops.

---

## Variable Scope Rules

| Scope | Declaration | Lifetime | Visibility | Use Case |
|-------|-------------|----------|------------|----------|
| **File scope (static)** | `static int var;` (outside functions) | Program lifetime | All components in subsystem | Shared state |
| **Function scope (static)** | `static int var;` (inside function) | Program lifetime | Single function | Persistent counters |
| **Local** | `int var;` (inside function) | Function call | Single function | Temporary values |
| **Global (GSM)** | `GSM.read<Type>()` | Program lifetime | All subsystems | Cross-chip state |

---

## Common Mistakes

| Mistake | Why It's Wrong | How to Fix |
|---------|----------------|------------|
| `#include "component.hdr"` in `.src` | Already aggregated | Remove include |
| `extern int var;` in `.src` | File scope already shared | Use `static` at file scope |
| Calling other component functions | Breaks isolation | Use static variables or GSM |
| Parameters on `init()` or `act()` | Breaks dispatch table | Remove parameters, use globals |
| Returning values from `act()` | Dispatch expects void | Use static variables to report state |
| Forgetting `return ESP_OK;` in `init()` | Boot fails silently | Always return status |
| Blocking in `act()` (delays, loops) | Freezes other components | Use timing checks, state machines |

---

## Component Registry

**Location**: `config/components/` (shared) or `config/bots/bot_families/{family}/` (creature-specific)

### Directory Structure

```
config/components/
├── hardware/          # Sensors, actuators (ultrasonic, servo, motor)
├── drivers/           # Device drivers (GC9A01, ADXL345)
├── interfaces/        # Communication buses (SPI, I2C, UART)
├── behaviors/         # AI, decision-making, state machines
└── functional/        # Utilities, math, filters

config/bots/bot_families/goblins/
└── components/        # Goblin-specific (goblin_eye, goblin_claw)
```

### Naming Convention

Component name in JSON **must match** function names:

```json
{
  "component": "goblin_eye_renderer"
}
```

Requires:
```cpp
esp_err_t goblin_eye_renderer_init(void);  // {component}_init
void goblin_eye_renderer_act(void);        // {component}_act
```

### Component Discovery

`generate_tables.py` searches in order:
1. Explicit `path` in JSON → use that file
2. Creature components: `config/bots/bot_families/{family}/components/{name}.json`
3. Shared components: `config/components/**/{name}.json`

**Duplicate names** (same component in shared and creature-specific): Creature-specific wins.

---

## System Tools Reference

| Tool | Purpose | When to Use |
|------|---------|-------------|
| `tools/generate_tables.py` | Aggregates .src/.hdr, generates dispatch tables | After any JSON or code change (always before build) |
| `tools/generate_file_structure.ps1` | Updates project documentation tree | After adding/moving files in config/ |
| `.\ascii_validator.ps1` | Scans workspace for non-ASCII characters | Before committing, when build errors mention encoding |
| `.\generate_file_structure.ps1` | Regenerates config tree documentation | After restructuring component directories |

**Standard workflow**:
```powershell
python tools/generate_tables.py goblin_head src
.\generate_file_structure.ps1
pio run -e goblin_head
```

---

## Quick Reference

**Need to share data?**
```
Same subsystem? → static variable at file scope
Different chips? → GSM.read<T>() / GSM.write<T>()
```

**Component not executing?**
```
Check hitCount in JSON
Check init() returned ESP_OK
Check if component in subsystem JSON
Check component name matches {name}_init/{name}_act
```

**Build errors?**
```
Multiple definition → Remove extern, use static
Undefined reference → Check function signatures
Missing symbol → Regenerate tables
Component not found → Check registry paths, naming
```

---

**Previous**: [02. Build Workflow](02_BUILD_WORKFLOW.md)  
**Next**: [04. Coding Standards](04_CODING_STANDARDS.md)
