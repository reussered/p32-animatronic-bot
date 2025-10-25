# P32 Component Architecture Rules

## CRITICAL: Component Function Pair Architecture

These are IRONCLAD rules that must NEVER be broken. Violations will cause build failures.

### Rule 1: Component File Structure
Every component MUST have exactly two files:
- **`component_name.cpp`** - Implementation file
- **`component_name.hpp`** - Header declaration file

### Rule 2: Component Function Signatures
Every component MUST implement exactly two functions with EXACT signatures:

```cpp
// In component_name.hpp:
esp_err_t component_name_init(void);  // MUST return esp_err_t
void component_name_act(void);        // MUST return void

// In component_name.cpp:
esp_err_t component_name_init(void) {
    // ... initialization code ...
    return ESP_OK;  // MANDATORY return statement
}

void component_name_act(void) {
    // ... action code ...
    // No return statement needed
}
```

### Rule 3: Aggregation Deduplication Pattern
**Function Names vs File Inclusion:**

- **✅ Function Names**: ALWAYS added to dispatch tables in execution order
- **✅ Header Files**: Only included ONCE in aggregation files to prevent compilation errors

**Example - Dual Eye System:**
```cpp
// Dispatch Tables (component_tables.cpp) - FUNCTIONS REPEAT
esp_err_t (*initTable[])(void) = {
    goblin_left_eye_init,    // [0] Left eye instance
    goblin_eye_init,         // [1] Eye logic for left
    gc9a01_init,             // [2] Display driver for left
    goblin_right_eye_init,   // [3] Right eye instance  
    goblin_eye_init,         // [4] Eye logic for right (REPEATED)
    gc9a01_init              // [5] Display driver for right (REPEATED)
};

// Aggregation Files (p32_component_functions.hpp) - HEADERS DEDUPLICATED
#include "components/goblin_left_eye.hpp"
#include "components/goblin_right_eye.hpp" 
#include "components/goblin_eye.hpp"        // ONLY ONCE
#include "components/gc9a01.hpp"            // ONLY ONCE
```

### Rule 4: Component Registration Chain
The execution pattern: `left_eye → eye → gc9a01, right_eye → eye → gc9a01`

- Each component instance gets its complete initialization/action chain
- Functions repeat in dispatch tables for proper execution order
- Header files deduplicated to prevent multiple definition errors

### Rule 5: Central Aggregation Requirements
These files MUST exist and be maintained:

1. **`p32_component_registry.hpp`** - Dispatch table declarations
2. **`p32_component_functions.hpp`** - Component header aggregation (deduplicated)
3. **`p32_component_functions.cpp`** - Includes aggregation header
4. **`component_tables.cpp`** - Generated dispatch tables with function repeats

### Rule 6: CMakeLists.txt Component Filtering
Build system MUST only compile components that are in the dispatch tables:

```cmake
set(P32_COMPONENT_SOURCES
    # Only components referenced in component_tables.cpp
    components/heartbeat.cpp
    components/network_monitor.cpp
    # ... etc - EXACTLY matching dispatch table
)
```

### Rule 7: Generate Tables Integration
When `generate_tables.py` creates dispatch tables:
- It expects ALL components to follow Rules 1-2
- It creates function repetition patterns per Rule 3
- It requires manual CMakeLists.txt updates per Rule 6

## Enforcement
These rules exist because:
1. **Dispatch Table Compilation** - Tables need exact function pointers
2. **Build System Integration** - CMakeLists.txt includes by pattern
3. **Automatic Generation** - Scripts rely on predictable structure
4. **40 Years of Experience** - Missing one `return ESP_OK;` stops everything

**VIOLATION = BUILD FAILURE**