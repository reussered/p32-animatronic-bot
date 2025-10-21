# P32 Dispatch Table Generation Rules Specification

## CRITICAL: Four-File Auto-Generation Architecture

**RULE**: The validation script (`validate_json_improved.py`) MUST auto-generate exactly 4 files from JSON component configurations:

### Required Generated Files:

1. **`src/p32_dispatch_tables.cpp`** - Implementation with tables
2. **`include/p32_dispatch_tables.hpp`** - Header with declarations  
3. **`src/p32_component_functions.cpp`** - Component implementation aggregator
4. **`include/p32_component_functions.hpp`** - Component header aggregator

### Version Control Rules:

**RULE**: ALL four generated files MUST be in `.gitignore` since they are script-generated:
```gitignore
# Auto-generated dispatch tables - created by validate_json_improved.py
src/p32_dispatch_tables.cpp
include/p32_dispatch_tables.hpp
src/p32_component_functions.cpp
include/p32_component_functions.hpp
```

## Component Function Signature Rules

### NO ARGUMENTS Pattern (IMMUTABLE):

**RULE**: ALL component functions MUST follow the NO ARGUMENTS pattern:
```cpp
// CORRECT - Component init functions
esp_err_t component_name_init(void);

// CORRECT - Component act functions  
void component_name_act(void);
```

**RULE**: Components access shared state via `#include "p32_shared_state.h"`:
- `g_loopCount` - Global loop counter (read-only for components)
- SharedMemory class - ESP-NOW mesh state synchronization
- All other global state variables

### Function Naming Convention:

**RULE**: Function names MUST follow this exact pattern:
```cpp
// Pattern: p32_comp_{component_name}_{function_type}
esp_err_t p32_comp_heartbeat_init(void);
void p32_comp_heartbeat_act(void);

esp_err_t p32_comp_goblin_left_eye_init(void);
void p32_comp_goblin_left_eye_act(void);
```

## Dispatch Table Structure Rules

### Three Table Architecture:

**RULE**: Generate exactly 3 parallel dispatch tables with SAME indexing:

```cpp
// Table 1: Initialization functions (called once at startup)
init_func_t initTable[COMPONENT_TABLE_SIZE] = {
    p32_comp_heartbeat_init,
    p32_comp_goblin_left_eye_init,
    // ... parallel to other tables
};

// Table 2: Action functions (called based on timing)
act_func_t actTable[COMPONENT_TABLE_SIZE] = {
    p32_comp_heartbeat_act,
    p32_comp_goblin_left_eye_act,
    // ... parallel to other tables
};

// Table 3: Timing control (execution frequency)
uint32_t hitCountTable[COMPONENT_TABLE_SIZE] = {
    1,    // heartbeat executes every loop
    30,   // left_eye executes every 30 loops
    // ... parallel to other tables
};
```

### Parallel Indexing Rule:

**RULE**: `initTable[i]`, `actTable[i]`, and `hitCountTable[i]` MUST refer to the SAME component
- Component execution: `if (g_loopCount % hitCountTable[i] == 0) actTable[i]()`
- Component at index `i` has init, act, and hitCount all at same array position

## Component Hierarchy Processing Rules

### Depth-First Component Resolution:

**RULE**: Process component hierarchy to create flat execution order:

```json
// INPUT: goblin_full.json contains goblin_head.json
// goblin_head.json contains left_eye.json + right_eye.json  
// left_eye.json contains goblin_eye.json + gc9a01.json
// right_eye.json contains goblin_eye.json + gc9a01.json

// OUTPUT: Flattened execution order (depth-first)
[
  "goblin_left_eye",     // Level 3: positioned component
  "goblin_eye",          // Level 4: shared component (first instance)
  "gc9a01",              // Level 5: hardware driver (first instance)
  "goblin_right_eye",    // Level 3: positioned component
  // goblin_eye SKIPPED  // Level 4: already processed (deduplication)
  // gc9a01 SKIPPED     // Level 5: already processed (deduplication)
]
```

### Component Deduplication Rule:

**RULE**: Shared components appear only ONCE in dispatch tables:
- `goblin_eye` component shared by left_eye + right_eye → included once
- `gc9a01` driver shared by left_eye + right_eye → included once
- Execution order: positioning components first, then shared components

### Component Type Processing Priority:

**RULE**: Process components in this exact order:
1. **System Components** (always present): heartbeat, network_monitor
2. **Family Components** (from family_template): goblin_behavior, goblin_mood  
3. **Positioned Components** (spatial instances): goblin_left_eye, goblin_right_eye
4. **Shared Components** (deduplicated): goblin_eye, gc9a01
5. **Hardware Drivers** (device-specific): i2s_audio, spi_display

## Hardware-Only Component Rules

### Shape-Only Components:

**RULE**: Components with `"hardware_only": true` do NOT generate init/act functions:
```json
{
  "component_name": "goblin_skull",
  "hardware_only": true,
  "shape": "goblin_skull.scad",
  // NO init/act functions generated - only provides mounting framework
}
```

**RULE**: Shape parameter MUST be present for hardware-only components:
- Required: `"shape": "component_shape.scad"`
- Purpose: Defines physical mounting framework for child components
- No software functionality - pure mechanical structure

## File Generation Content Rules

### Dispatch Tables Header (p32_dispatch_tables.hpp):

**RULE**: Header MUST contain:
```cpp
#ifndef P32_DISPATCH_TABLES_HPP
#define P32_DISPATCH_TABLES_HPP

#include "esp_err.h"
#include <stdint.h>

// Function type definitions
typedef esp_err_t (*init_func_t)(void);
typedef void (*act_func_t)(void);

// Table size constant
#define COMPONENT_TABLE_SIZE N

// Forward declarations for ALL component functions
esp_err_t p32_comp_component_name_init(void);
void p32_comp_component_name_act(void);

// External table declarations
extern init_func_t initTable[COMPONENT_TABLE_SIZE];
extern act_func_t actTable[COMPONENT_TABLE_SIZE];
extern uint32_t hitCountTable[COMPONENT_TABLE_SIZE];

// Global loop counter
extern uint64_t g_loopCount;

#endif
```

### Dispatch Tables Implementation (p32_dispatch_tables.cpp):

**RULE**: Implementation MUST contain:
```cpp
#include "p32_dispatch_tables.hpp"

// Global loop counter definition
uint64_t g_loopCount = 0;

// Table implementations with exact parallel indexing
init_func_t initTable[COMPONENT_TABLE_SIZE] = {
    // All init functions in component order
};

act_func_t actTable[COMPONENT_TABLE_SIZE] = {
    // All act functions in SAME ORDER as initTable
};

uint32_t hitCountTable[COMPONENT_TABLE_SIZE] = {
    // All hitCount values in SAME ORDER as initTable
};
```

### Component Functions Aggregator Rules:

**RULE**: p32_component_functions.hpp MUST include ALL unique component headers:
```cpp
// Auto-generated component function aggregator
#ifndef P32_COMPONENT_FUNCTIONS_HPP
#define P32_COMPONENT_FUNCTIONS_HPP

// Include all unique component implementation headers
#include "components/heartbeat.hpp"
#include "components/goblin_eye.hpp"
#include "components/gc9a01.hpp"
// ... deduplicated list of all component headers

#endif
```

**RULE**: p32_component_functions.cpp provides empty implementation stub:
```cpp
#include "p32_component_functions.hpp"
// Implementation provided by individual component files
```

## Integration Rules

### Build System Integration:

**RULE**: Generated files MUST be included in build system:
```cmake
# CMakeLists.txt MUST include auto-generated files
set(SOURCES
    src/main.cpp
    src/p32_dispatch_tables.cpp    # Auto-generated
    src/p32_component_functions.cpp # Auto-generated
    # ... other source files
)
```

### Main Loop Integration:

**RULE**: `main.cpp` MUST include dispatch tables header:
```cpp
#include "p32_dispatch_tables.hpp"

void app_main(void) {
    // Initialize all components once
    for (int i = 0; i < COMPONENT_TABLE_SIZE; i++) {
        esp_err_t result = initTable[i]();
        if (result != ESP_OK) {
            ESP_LOGE("MAIN", "Component %d init failed: %d", i, result);
        }
    }
    
    // Execute components based on timing
    while (true) {
        for (int i = 0; i < COMPONENT_TABLE_SIZE; i++) {
            if (g_loopCount % hitCountTable[i] == 0) {
                actTable[i]();
            }
        }
        g_loopCount++;
    }
}
```

## Validation Script Requirements

### Script Execution Rules:

**RULE**: `validate_json_improved.py` MUST:
1. Scan all JSON component configurations
2. Build hierarchical component tree
3. Flatten to execution order with deduplication
4. Generate all 4 files atomically (success/failure together)
5. Report component counts and changes made

### Error Handling Rules:

**RULE**: Script MUST validate:
- JSON syntax and structure
- Component reference resolution
- Circular dependency detection  
- Missing hitCount or component_name fields
- hardware_only components have shape parameter

### Execution Output Rules:

**RULE**: Script MUST report:
```bash
Components processed: N
Unique component names: N  
Total changes: 4 (if files generated)
Generated: src/p32_dispatch_tables.cpp
Generated: include/p32_dispatch_tables.hpp
Generated: src/p32_component_functions.cpp
Generated: include/p32_component_functions.hpp
```

## Architecture Compliance Rules

### Component Execution Model:

**RULE**: Dispatch tables enable the IMMUTABLE core loop:
```cpp
// THE ENTIRE SYSTEM (5 lines - NEVER changes)
void app_main(void) {
    for (int i = 0; i < COMPONENT_TABLE_SIZE; i++) initTable[i]();
    
    while (true) {
        for (int i = 0; i < COMPONENT_TABLE_SIZE; i++) 
            if (g_loopCount % hitCountTable[i] == 0) actTable[i]();
        g_loopCount++;
    }
}
```

### NO ARGUMENTS Architecture:

**RULE**: Components communicate via shared state, not function parameters:
- All functions take `void` - no arguments passed
- Components read `g_loopCount` and shared state directly
- ESP-NOW mesh communication via SharedMemory class
- This enables the simple dispatch table architecture

## Usage Pattern Rules

### Development Workflow:

**RULE**: After JSON configuration changes:
1. Run `python validate_json_improved.py`
2. Verify 4 files generated successfully
3. Build with `pio run` to test compilation
4. Commit only JSON changes (generated files ignored)

### Debugging Rules:

**RULE**: For dispatch table debugging:
- Check COMPONENT_TABLE_SIZE matches actual component count
- Verify parallel indexing: initTable[i] ↔ actTable[i] ↔ hitCountTable[i]
- Validate all function declarations have implementations
- Confirm no duplicate component names in final tables

This specification provides the complete ruleset for dispatch table generation independent of any specific implementation.