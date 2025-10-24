# P32 Generation Script Rules - IMMUTABLE PROCESS

## 🚨 CRITICAL: GENERATION SCRIPT REQUIREMENTS 🚨

### RULE 1: Input Processing
**The generation script MUST:**
- Accept a bot JSON configuration file as input (e.g., `goblin_full.json`)
- Parse JSON recursively to discover all component hierarchies
- Identify all subsystem types during recursive traversal
- Group components by subsystem affiliation automatically

### RULE 2: Subsystem Discovery Algorithm
**During JSON recursive composition, the script MUST:**
1. **Traverse all `contained_components` arrays** in bot and subsystem definitions
2. **Identify subsystem boundaries** by detecting `subsystem_type` fields
3. **Collect component lists** for each discovered subsystem
4. **Create unique subsystem identifiers** based on `subsystem_type` values

**Valid subsystem types:**
- `HEAD`
- `TORSO` 
- `LEFT_ARM`
- `RIGHT_ARM`
- `LEFT_LEG`
- `RIGHT_LEG`
- Custom subsystem types (must be UPPERCASE with underscores)

### RULE 3: File Generation Requirements
**For EACH discovered subsystem, the script MUST generate:**

#### Component Functions Files:
- `src/{subsystem}_component_functions.cpp`
- `src/{subsystem}_component_functions.hpp`

#### Dispatch Tables Files:
- `src/{subsystem}_dispatch_tables.cpp`
- `src/{subsystem}_dispatch_tables.hpp`

#### Build Configuration:
- `src/subsystems/{subsystem}/CMakeLists.txt`

### RULE 4: Component Functions Generation
**{subsystem}_component_functions.cpp MUST contain:**
```cpp
// ============================================================================
// {SUBSYSTEM} SUBSYSTEM - Component Functions
// Contains all component implementations for the {subsystem} subsystem
// ============================================================================

#include "p32_component_functions.hpp"

// This file aggregates all component implementations for the {SUBSYSTEM} subsystem:
//
// Components loaded for {subsystem} subsystem:
// {LIST_ALL_COMPONENTS_WITH_DESCRIPTIONS}
//
// The actual function definitions are in their respective component files:
// {LIST_ALL_COMPONENT_SOURCE_FILES}
//
// This aggregation file serves as the central include point for all {subsystem}
// subsystem component functions referenced in {subsystem}_dispatch_tables.cpp
```

### RULE 5: Dispatch Tables Generation
**{subsystem}_dispatch_tables.cpp MUST contain:**
```cpp
// ============================================================================
// {SUBSYSTEM} SUBSYSTEM - Dispatch Tables
// Component dispatch tables for the {subsystem} subsystem
// ============================================================================

#include "p32_dispatch_tables.hpp"
#include "p32_component_functions.hpp"

// ============================================================================
// Global Loop Counter - Written by main.cpp, read by all components
// ============================================================================

uint64_t g_loopCount = 0;

// ============================================================================
// Forward Declarations - {SUBSYSTEM} SUBSYSTEM Components
// ============================================================================

{FORWARD_DECLARATIONS_FOR_ALL_COMPONENTS}

// ============================================================================
// {SUBSYSTEM} SUBSYSTEM - Dispatch Tables
// ============================================================================

// Component function type signatures - NO ARGUMENTS pattern
typedef esp_err_t (*init_func_t)(void);
typedef void (*act_func_t)(void);

// Table size - all three tables have same size (one entry per component)
#define COMPONENT_TABLE_SIZE {COMPONENT_COUNT}

// ============================================================================
// Initialization Table - Called once at startup in order
// ============================================================================

init_func_t initTable[COMPONENT_TABLE_SIZE] = {
    {INIT_FUNCTION_LIST}
};

// ============================================================================
// Action Table - Function Pointers (NO ARGUMENTS)
// ============================================================================

act_func_t actTable[COMPONENT_TABLE_SIZE] = {
    {ACT_FUNCTION_LIST}
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[COMPONENT_TABLE_SIZE] = {
    {HIT_COUNT_LIST}
};
```

### RULE 6: CMakeLists.txt Generation
**src/subsystems/{subsystem}/CMakeLists.txt MUST contain:**
```cmake
# ============================================================================
# {SUBSYSTEM} Subsystem Build Configuration
# Auto-generated CMakeLists.txt for {subsystem} subsystem
# ============================================================================

# Universal 3-file compilation pattern for {SUBSYSTEM} subsystem
set(COMPONENT_SRCS
    "../../main.cpp"
    "../../{subsystem}_component_functions.cpp"
    "../../{subsystem}_dispatch_tables.cpp"
)

# Component directories for header files
set(COMPONENT_ADD_INCLUDEDIRS 
    "../../include"
    "../../include/components"
)

# ESP-IDF component registration
register_component()
```

### RULE 7: Header File Generation
**{subsystem}_component_functions.hpp MUST contain:**
```hpp
#ifndef P32_COMPONENT_FUNCTIONS_HPP
#define P32_COMPONENT_FUNCTIONS_HPP

// ============================================================================
// {SUBSYSTEM} Subsystem - Component Function Headers
// This file includes all component headers for the {subsystem} subsystem
// ============================================================================

// Individual component headers
{INCLUDE_ALL_COMPONENT_HEADERS}

// Shared state access
#include "p32_shared_state.h"

#endif // P32_COMPONENT_FUNCTIONS_HPP
```

### RULE 8: Component Analysis Requirements
**The script MUST analyze each component to extract:**
- Component name (from `component_name` field)
- Component timing (from `timing.hitCount` field)
- Component description (from `description` field)
- Required header file path
- Init function name: `{component_name}_init`
- Act function name: `{component_name}_act`

### RULE 9: Subsystem Grouping Logic
**Components belong to a subsystem if:**
1. **Direct containment**: Listed in subsystem's `contained_components` array
2. **Inherited containment**: Referenced by component that belongs to subsystem
3. **Recursive containment**: Part of component hierarchy under subsystem

**Component deduplication:**
- If same component appears multiple times in hierarchy, include only once
- Use component_name as unique identifier
- Maintain original hitCount from first occurrence

### RULE 10: File Output Structure
**Generated files MUST be placed in exact locations:**
```
src/
├── main.cpp (universal - never generated)
├── {subsystem}_component_functions.cpp (generated)
├── {subsystem}_component_functions.hpp (generated)  
├── {subsystem}_dispatch_tables.cpp (generated)
├── {subsystem}_dispatch_tables.hpp (generated)
└── subsystems/
    └── {subsystem}/
        └── CMakeLists.txt (generated)
```

### RULE 11: Generation Script Command Interface
**The script MUST accept:**
```bash
# Generate all subsystems from bot configuration
./generate_subsystems.ps1 -BotConfig "config/bots/goblin_full.json"

# Generate specific subsystem only
./generate_subsystems.ps1 -BotConfig "config/bots/goblin_full.json" -Subsystem "head"

# Regenerate all subsystems (clean + generate)
./generate_subsystems.ps1 -BotConfig "config/bots/goblin_full.json" -Clean
```

### RULE 12: Validation Requirements
**After generation, the script MUST:**
1. **Verify file creation**: All required files exist in correct locations
2. **Validate syntax**: Generated C++ files compile without errors  
3. **Check consistency**: Component counts match across all tables
4. **Report results**: List generated subsystems and component counts

### RULE 13: Error Handling
**The script MUST handle:**
- **Missing JSON files**: Report file path and exit gracefully
- **Invalid JSON syntax**: Show parse error location and exit
- **Missing component fields**: Report which component lacks required fields
- **Circular references**: Detect and report component dependency loops
- **Write permission errors**: Report file system access issues

### RULE 14: Incremental Generation
**The script MUST support:**
- **Selective regeneration**: Only update files for changed subsystems
- **Dependency tracking**: Regenerate if component files have changed
- **Timestamp checking**: Skip generation if outputs are newer than inputs
- **Force regeneration**: Override timestamp checking when requested

## GENERATION WORKFLOW

### Phase 1: Discovery
1. Load and parse bot JSON configuration
2. Recursively traverse all component references  
3. Identify unique subsystems and group components
4. Validate component definitions and resolve dependencies

### Phase 2: Analysis
1. Extract component metadata (names, timing, descriptions)
2. Generate forward declarations and function lists
3. Calculate table sizes and validate consistency
4. Create file content templates with substituted values

### Phase 3: Output
1. Create directory structure if needed
2. Write all generated files to correct locations
3. Validate generated file syntax and structure
4. Report generation results and any warnings

### Phase 4: Verification
1. Attempt compilation of generated files
2. Check that all references resolve correctly
3. Verify table sizes match component counts
4. Confirm CMakeLists.txt points to correct files

## ENFORCEMENT

**These generation rules are IMMUTABLE and must be followed by:**
- All generation script implementations
- All build system integrations
- All component definition standards
- All JSON configuration formats

**Any changes to generation logic require updating this document FIRST.**

---
**Document Status:** ACTIVE GENERATION SPECIFICATION
**Last Updated:** 2025-10-24
**Enforcement Level:** CRITICAL - NO EXCEPTIONS