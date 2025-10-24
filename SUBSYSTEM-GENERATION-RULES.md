# P32 Subsystem Generation Architecture - IMMUTABLE RULES

## 🚨 CRITICAL: UNIVERSAL SUBSYSTEM PATTERN 🚨

### RULE 1: Universal 3-File Architecture
**EVERY subsystem compiles EXACTLY 3 files:**
1. `main.cpp` - ALWAYS IDENTICAL across ALL subsystems (universal entry point)
2. `{subsystem}_component_functions.cpp` - Contains subsystem-specific components
3. `{subsystem}_dispatch_tables.cpp` - Contains subsystem-specific dispatch tables

**NO EXCEPTIONS. NO VARIATIONS. UNIVERSAL PATTERN.**

### RULE 2: Generation Script Responsibilities
**The generation script MUST:**
- Process JSON recursive composition for bot definitions
- Discover ALL subsystems during JSON traversal
- For EACH subsystem found, auto-generate:
  - `{subsystem}_component_functions.cpp`
  - `{subsystem}_component_functions.hpp`
  - `{subsystem}_dispatch_tables.cpp`
  - `{subsystem}_dispatch_tables.hpp`
  - `src/subsystems/{subsystem}/CMakeLists.txt`

### RULE 3: Subsystem Naming Convention
**Subsystem names MUST follow pattern:**
- `head` (not test_head, goblin_head, etc.)
- `torso`
- `left_arm`
- `right_arm`
- `left_leg`
- `right_leg`
- Additional subsystems: `{descriptive_name}` (snake_case)

**Generated files use exact subsystem name:**
- `head_component_functions.cpp`
- `torso_dispatch_tables.cpp`
- etc.

### RULE 4: CMakeLists.txt Structure
**Each subsystem CMakeLists.txt MUST specify EXACTLY:**
```cmake
# Subsystem: {subsystem_name}
# Universal 3-file compilation pattern
set(COMPONENT_SRCS
    "main.cpp"
    "{subsystem}_component_functions.cpp"
    "{subsystem}_dispatch_tables.cpp"
)
```

### RULE 5: PlatformIO Environment Pattern
**Each environment MUST point to correct subsystem CMakeLists.txt:**
```ini
[env:{environment_name}]
# Points to specific subsystem build configuration
# CMakeLists.txt handles which 3 files to compile
```

### RULE 6: main.cpp Universality
**main.cpp MUST:**
- Be IDENTICAL across all subsystems
- Never contain subsystem-specific logic
- Use dispatch tables to load different component sets
- Never change regardless of subsystem type

### RULE 7: Component Function Aggregation
**{subsystem}_component_functions.cpp MUST:**
- Include headers for ALL components in that subsystem
- Aggregate component implementations via includes
- Never contain actual function implementations
- Serve as central include point for dispatch tables

### RULE 8: Dispatch Table Generation
**{subsystem}_dispatch_tables.cpp MUST:**
- Contain forward declarations for ALL subsystem components
- Define initTable, actTable, hitCountTable arrays
- Set COMPONENT_TABLE_SIZE to match component count
- Include global g_loopCount declaration

### RULE 9: Subsystem Discovery Process
**Generation script MUST:**
1. Parse bot JSON configuration recursively
2. Identify unique subsystem types during composition
3. Group components by subsystem affiliation
4. Generate separate files for each discovered subsystem
5. Create appropriate CMakeLists.txt for each subsystem

### RULE 10: File Location Standards
**Generated files MUST be placed:**
- `src/{subsystem}_component_functions.cpp/hpp` (root src/)
- `src/{subsystem}_dispatch_tables.cpp/hpp` (root src/)
- `src/subsystems/{subsystem}/CMakeLists.txt` (subsystem folder)
- `src/main.cpp` (universal, root src/)

## ARCHITECTURE GUARANTEES

### ✅ What This Architecture Ensures:
- **Universal Compatibility**: Same main.cpp works for any subsystem
- **Distributed Control**: Different ESP32s run different software via component selection
- **Scalable Growth**: New subsystems added by JSON configuration changes
- **Predictable Build**: Always 3 files, always same pattern
- **Automated Generation**: No manual file creation or maintenance

### ❌ What This Architecture Prevents:
- **Subsystem-Specific main.cpp**: Never allowed, breaks universality
- **Manual Component Management**: Generation script handles everything
- **Inconsistent File Patterns**: Enforced naming and structure
- **Build Complexity**: Simple 3-file pattern always

## IMPLEMENTATION SEQUENCE

### Phase 1: Generation Script Enhancement
1. Add subsystem discovery logic to JSON parser
2. Implement component grouping by subsystem type
3. Add file generation for component functions and dispatch tables
4. Create CMakeLists.txt generation for each subsystem

### Phase 2: PlatformIO Configuration
1. Update environments to point to correct subsystem CMakeLists.txt
2. Verify universal main.cpp compatibility
3. Test build process for each generated subsystem

### Phase 3: Validation
1. Verify identical main.cpp across all subsystems
2. Confirm component isolation between subsystems
3. Test distributed ESP32 deployment scenario

## EXAMPLES

### Head Subsystem Files:
```
src/main.cpp (universal)
src/head_component_functions.cpp (head components)
src/head_dispatch_tables.cpp (head dispatch tables)
src/subsystems/head/CMakeLists.txt (build config)
```

### Torso Subsystem Files:
```
src/main.cpp (same universal file)
src/torso_component_functions.cpp (torso components)
src/torso_dispatch_tables.cpp (torso dispatch tables)
src/subsystems/torso/CMakeLists.txt (build config)
```

## ENFORCEMENT

**These rules are IMMUTABLE and must be followed by:**
- All code generation scripts
- All build system configurations
- All subsystem implementations
- All future architecture decisions

**Any deviation from this pattern requires updating this document FIRST.**

---
**Document Status:** ACTIVE ARCHITECTURE SPECIFICATION
**Last Updated:** 2025-10-24
**Enforcement Level:** CRITICAL - NO EXCEPTIONS