# P32 Component Code Requirements

## Overview
This document defines the mandatory structure and requirements for all P32 animatronic component source files to ensure proper compilation, initialization, and execution within the component system.

📘 **[Three-Level Component Attachment Architecture](THREE-LEVEL-COMPONENT-ATTACHMENT-SPEC.md)** - READ FIRST

**CRITICAL PRINCIPLE**: Every feature must be a component with `init()` and `act(loopCount)` functions. The core loop in `app_main()` contains NO application logic - it ONLY iterates through registered components.

## Architecture: C/C++ Hybrid System

The P32 system uses a **hybrid C/C++ architecture** which is supported and recommended by ESP-IDF:

### C++ Core Systems (High-Level Logic)
- **Mood System** (`Mood.cpp/.hpp`) - Advanced mood calculations with floating-point precision
- **Frame Processor** (`FrameProcessor.cpp/.hpp`) - Efficient mood-based rendering engine
- **Frame Mood Processor** (`FrameMoodProcessor.hpp`) - Integration layer for mood-driven animations

### C Component Layer (Hardware Interface)
- **Component Files** (`src/components/*.c`) - Individual component implementations
- **Hardware Drivers** - GPIO, SPI, I2S, sensor interfaces
- **ESP-IDF Integration** - FreeRTOS tasks, system calls
- **Component Tables** - Init/action function registration

### Integration Pattern
C component files call C++ systems through proper C linkage:
```c
// C component calls C++ mood system
extern "C" {
    void update_mood_from_c(float mood_values[8]);
    void process_frame_from_c(uint8_t* pixel_data, int width, int height);
}
```

## Component File Structure Requirements

### 1. File Naming Convention
- **Pattern**: `{component_name}.c`
- **Location**: `src/components/`
- **Examples**: 
  - `goblin_eye_left.c`
  - `goblin_eye_right.c` 
  - `goblin_nose.c`
  - `system_core.c`
  - `network_monitor.c`

### 2. Language Selection: C vs C++

#### When to Use C (.c files)
- Hardware interface components (GPIO, SPI, I2S)
- ESP-IDF API integration
- Simple sensor reading/actuator control
- Component system registration (init/act functions)

#### When to Use C++ (.cpp files)  
- Complex algorithms (mood calculations, pathfinding)
- Object-oriented designs (animation systems, AI behaviors)
- Mathematical computations with floating-point precision
- STL containers and advanced language features

#### C/C++ Interoperability
For C components that need C++ functionality:
```c
// In C component file - call C++ functions
#ifdef __cplusplus
extern "C" {
#endif

// C-compatible function declarations
void update_mood_from_component(float mood_values[8]);
void process_animation_frame(uint8_t* pixels, int width, int height);

#ifdef __cplusplus
}
#endif
```

### 3. Mandatory Header Structure

Every component file MUST include the following header structure:

```c
// P32 Component: {component_name}
// Auto-generated individual component file
// Memory footprint can be measured independently

#include "p32_component_config.h"

#ifdef ENABLE_{FAMILY}_COMPONENTS

#include "esp_log.h"
#include "esp_err.h"
// ... other required includes
```

### 3. Required Preprocessor Guards

#### Top-Level Guard
- **REQUIRED**: Every component file MUST be wrapped in an `#ifdef` guard
- **Pattern**: `#ifdef ENABLE_{FAMILY}_COMPONENTS`
- **Examples**:
  - `#ifdef ENABLE_GOBLIN_COMPONENTS`
  - `#ifdef ENABLE_SYSTEM_COMPONENTS`
  - `#ifdef ENABLE_ROBOT_COMPONENTS`

#### Guard Placement
- **Opening**: After includes, before any code
- **Closing**: At end of file with comment: `#endif // ENABLE_{FAMILY}_COMPONENTS`

### 4. Mandatory Function Definitions

Each component file MUST define exactly two functions:

#### A. Initialization Function
```c
esp_err_t p32_comp_{component_name}_init(void) {
    // Initialization code here
    return ESP_OK; // or appropriate error code
}
```

**Requirements**:
- Return type: `esp_err_t`
- Naming pattern: `p32_comp_{component_name}_init`
- Parameter: `void`
- Must return `ESP_OK` on success or appropriate ESP error code

#### B. Action Function  
```c
void p32_comp_{component_name}_act(uint32_t loopCount) {
    // Action/loop code here
}
```

**Requirements**:
- Return type: `void`
- Naming pattern: `p32_comp_{component_name}_act`
- Parameter: `uint32_t loopCount`
- Called repeatedly in main loop

### 5. Function Name Mapping

The function names MUST match the entries in the component tables:

#### Init Table (`src/initTable.c`)
```c
init_func_t initTable[INIT_TABLE_SIZE] = {
    p32_comp_{component_name}_init,
    // ... other init functions
};
```

#### Action Table (`src/actTable.c`)
```c
act_table_entry_t actTable[ACT_TABLE_SIZE] = {
    { p32_comp_{component_name}_act, hitCount },
    // ... other action functions
};
```

### 6. Header File Declarations

Component functions MUST be declared in the appropriate header files:

#### Init Table Header (`include/initTable.h`)
```c
// Forward declarations for component init functions
esp_err_t p32_comp_{component_name}_init(void);
```

#### Action Table Header (`include/actTable.h`) 
```c
// Forward declarations for component action functions
void p32_comp_{component_name}_act(uint32_t loopCount);
```

## Component Configuration Requirements

### 1. Enable Flags
Each component family MUST have an enable flag defined in `src/p32_component_config.h`:

```c
// Component enable flags for conditional compilation
#define ENABLE_GOBLIN_COMPONENTS
#define ENABLE_SYSTEM_COMPONENTS  
#define ENABLE_ROBOT_COMPONENTS
// etc.
```

### 2. Component Registration
Components MUST be registered in the appropriate tables:

1. **Init Table**: Add init function to `src/initTable.c`
2. **Action Table**: Add action function with timing to `src/actTable.c`  
3. **Header Declarations**: Add function declarations to headers

## Validation Requirements

### Automated Checks
The following checks MUST be performed during build validation:

#### 1. File Structure Validation
- ✅ All `.c` files in `src/components/` have proper header structure
- ✅ All component files have top-level `#ifdef` guards
- ✅ All component files have closing `#endif` with proper comment

#### 2. Function Naming Validation  
- ✅ Init functions follow `p32_comp_{name}_init` pattern
- ✅ Action functions follow `p32_comp_{name}_act` pattern
- ✅ Function signatures match requirements exactly

#### 3. Table Consistency Validation
- ✅ All functions in `initTable.c` have corresponding source files
- ✅ All functions in `actTable.c` have corresponding source files  
- ✅ All functions declared in headers are implemented
- ✅ Table sizes match actual entries

#### 4. Configuration Validation
- ✅ All component families have enable flags in `p32_component_config.h`
- ✅ Enable flags match `#ifdef` guards in source files
- ✅ No orphaned component files without enable flags

### Manual Review Checklist

Before accepting component code changes:

- [ ] Component follows proper file naming convention
- [ ] Header structure includes all required elements  
- [ ] Top-level `#ifdef` guard uses correct family name
- [ ] Init function has correct signature and naming
- [ ] Action function has correct signature and naming
- [ ] Functions are added to appropriate tables
- [ ] Header declarations are updated
- [ ] Enable flag exists in configuration
- [ ] Closing `#endif` has proper comment

## Error Patterns to Avoid

### ❌ Common Mistakes

1. **Missing Top-Level Guard**
   ```c
   // WRONG - no #ifdef guard
   #include "esp_log.h"
   esp_err_t goblin_eye_left_init(void) { ... }
   ```

2. **Incorrect Function Naming**
   ```c
   // WRONG - doesn't match table expectations
   esp_err_t goblin_eye_left_init(void) { ... }  // Missing p32_comp_ prefix
   ```

3. **Mismatched Table Entries**
   ```c
   // initTable.c references p32_comp_left_eye_init
   // but source file defines goblin_eye_left_init
   ```

4. **Missing Configuration Flag**
   ```c
   // WRONG - no ENABLE_GOBLIN_COMPONENTS in p32_component_config.h
   #ifdef ENABLE_GOBLIN_COMPONENTS  // Flag doesn't exist
   ```

### ✅ Correct Implementation

```c
// P32 Component: goblin_eye_left
#include "p32_component_config.h"

#ifdef ENABLE_GOBLIN_COMPONENTS

#include "esp_log.h"
#include "esp_err.h"

esp_err_t p32_comp_left_eye_init(void) {
    // Initialization code
    return ESP_OK;
}

void p32_comp_left_eye_act(uint32_t loopCount) {
    // Action code
}

#endif // ENABLE_GOBLIN_COMPONENTS
```

## Future Enhancements

### Automated Validation Script
Consider implementing a validation script (`tools/validate_components.py`) that:

- Scans `src/components/` for all `.c` files
- Validates file structure and function patterns  
- Checks table consistency
- Generates reports of missing/incorrect components
- Integrates with CI/CD pipeline

### Code Generation
Future improvements could include:

- Automated component template generation
- Bot JSON → component code generation  
- Table auto-generation from component scan
- Header file auto-update from source files

## Build System Configuration

### C++ Support in ESP-IDF
The build system is properly configured for C/C++ mixing:

#### CMakeLists.txt Configuration
```cmake
# Both C and C++ sources supported
set(P32_COMPONENT_SOURCES
    Mood.cpp                        # C++ Mood system
    FrameProcessor.cpp              # C++ Frame processing  
    components/goblin_eye_left.c    # C hardware component
    components/goblin_eye_right.c   # C hardware component
)
```

#### PlatformIO Configuration
```ini
[env:esp32-s3-devkitc-1]
platform = espressif32
framework = espidf  # ESP-IDF supports C++ out of the box
```

#### Compiler Behavior
- `.c` files compiled with C compiler
- `.cpp` files compiled with C++ compiler  
- Automatic linking of both object files
- C++ standard library available (`<cstdio>`, `<cstring>`, etc.)

### Integration Best Practices
1. **C++ Core Systems**: Use for complex algorithms and data structures
2. **C Hardware Layer**: Use for ESP-IDF APIs and hardware control
3. **Extern C Linkage**: For C components calling C++ functions
4. **Header Guards**: Use `#ifdef __cplusplus` when needed

## Related Documentation

- `docs/component-system-architecture.md` - Overall component system design
- `docs/bot-json-specification.md` - Bot configuration format  
- `src/p32_component_config.h` - Component enable flags
- `include/initTable.h` - Init function declarations
- `include/actTable.h` - Action function declarations
- `include/Mood.hpp` - C++ Mood system interface
- `include/FrameProcessor.hpp` - C++ Frame processing interface

---

**Version**: 1.0.0  
**Last Updated**: October 13, 2025  
**Maintainer**: P32 Development Team