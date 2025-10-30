# P32 Animatronic Bot - Complete Architectural Ruleset

## 🚨 CRITICAL SETUP (READ FIRST)

- **Project Root**: `f:\GitHub\p32-animatronic-bot` (fixed location, use absolute paths)
- **MANDATORY**: Read this document before any code changes. This consolidates all architectural rules without duplicates.

## 🤖 AI Agent Behavior Guidelines

- **Defer to user expertise** as domain expert
- **Never delete, modify, or refactor** existing code without explicit permission
- **Explain all proposed changes** clearly and get confirmation for significant edits
- **Preserve existing code context and intent** - no changes "out of context" or based on external assumptions

## 🏗️ Architecture Overview

### Pure Component System

- Everything is a component with `init()`/`act()` functions
- Main loop iterates components only - no application logic in `app_main()`

### Component Pattern

```cpp
esp_err_t component_name_init(void);  // Returns ESP_OK on success
void component_name_act(void);        // No args, reports errors via SharedMemory
```

### Error Reporting

- Components report errors through SharedMemory (GSM) for monitoring and user notification

### Communication

- **Within subsystems**: Components communicate via local memory in aggregated `{subsystem}_component_functions.cpp` files
- **Across subsystems/chips**: Use SharedMemory for all inter-component coordination and global variable communication
- **Never call component functions directly** across subsystem boundaries

## 🔧 Essential Workflows

- **Generate Components**: `python tools/generate_tables.py goblin_full src` (reads JSON, creates dispatch tables and aggregates individual component cpp/hpp files into subsystem component_functions.cpp/hpp)
- **Build & Flash**: `pio run -t upload -t monitor`
- **Validate Config**: `.\generate_file_structure.ps1`
- **Multi-Chip Testing**: Use `test/distance_eye_test/` for SharedMemory mesh validation

## ⚡ Critical Constraints

- **ESP32 Variants**: Determined per subsystem by final code analysis and testing
- **Real-Time**: Components must return quickly (<10ms), no blocking operations
- **Multi-Chip**: SharedMemory broadcasts via internal mesh protocol, never direct function calls
- **Build System**: PlatformIO + ESP-IDF, multiple environments for subsystems

## 🗂️ File Organization

- **Components**: `src/components/{name}.cpp`, `include/components/{name}.hpp`
- **Configs**: `config/bots/bot_families/{family}/{bot}.json`
- **Assets**: `assets/{animations,sounds}/{creature}/` for creature-specific assets, `assets/shapes/scad/` for shared shapes
- **3D Models**: Tier 1 (universal hardware) + Tier 2 (creature shells)

## 🔗 Integration Points

- **SharedMemory Mesh**: `SharedMemory::write()` broadcasts to all chips
- **Hardware Interfaces**: SPI bus + device pattern, I2S shared bus + unique pins
- **Coordinate Systems**: 2D planar or 3D skull-based with `"reference_point": "nose_center"`
- **PowerShell Tooling**: All scripts lowercase, absolute paths required

## 🏛️ Component Relationships & JSON Interpretation

### Three-Level Hierarchy

- **System Level**: Core platform (WiFi, Serial, Watchdog) - always present
- **Family Level**: Behavior/personality shared across bot family (Goblin, Cat, Bear)
- **Bot-Specific Level**: Positioned hardware components (eyes, nose, mouth, sensors)

### Recursive Composition

- `goblin_full.json` → `goblin_head.json` → `left_eye.json`

### Component Containment Hierarchy

**CONTAINMENT TYPES:**

1. **contained_components**: Direct child components loaded when parent initializes
   - **Loading Order**: Components loaded in JSON array order during traversal
   - **Use Case**: Physical sub-components, positioned elements, subsystem assemblies
   - **Example**: `goblin_left_eye.json` contains `goblin_eye.json`

2. **includes_components**: Dependency components loaded as requirements  
   - **Loading Order**: Loaded immediately after parent during JSON traversal
   - **Use Case**: Interfaces, drivers, shared libraries that component depends on
   - **Example**: `gc9a01_display.json` includes `generic_spi_display.json`

**LOADING BEHAVIOR:**

- **MANDATORY**: Components loaded EXACTLY as encountered during JSON parsing
- **MANDATORY**: Depth-first recursive traversal: parent → includes_components → contained_components
- **MANDATORY**: Dispatch tables preserve ALL traversal encounters (including duplicates when same component encountered multiple times)
- **MANDATORY**: Component source files deduplicated (each .src/.hpp file included only once)
- **MANDATORY**: Initialization order follows dispatch table order

**COMPONENT LOADING SEQUENCE (MANDATORY):**

When parsing a JSON component file:

1. Process `includes_components` array in order → load each dependency
2. Process `contained_components` array in order → recursively load each child

**DISPATCH TABLE RULES (MANDATORY):**

- **NO DEDUPLICATION**: Every component encounter during traversal gets an entry
- **PRESERVE ORDER**: Tables reflect exact JSON parsing sequence
- **ALLOW DUPLICATES**: Same component can appear multiple times if encountered multiple times

**SOURCE FILE RULES (MANDATORY):**

- **DEDUPLICATED**: Each component source file included exactly once
- **BUILD UNIQUE**: No duplicate .src files in compilation
- **HEADER UNIQUE**: No duplicate .hpp files in includes

**CONTAINMENT RULES:**

- **NEVER Direct Calls**: Contained components never call each other directly
- **SharedMemory Only**: All inter-component communication via SharedMemory
- **Subsystem Boundaries**: Containment respects subsystem boundaries for multi-chip coordination

### JSON Config System

- Reference other configs: `"author": "config/author.json"`
- Coordinates: `"x": "-1.5 INCH"` (always include units)
- Coordinate systems: `"coordinate_system": "planar_2d"` or `"skull_3d"`
- Required fields: `"relative_filename"`, `"version"`, `"author"`
- Shape parameter: Present for hardware-only components (`"init_function": "STUB"`)

## 🎯 Component Development Rules

### Component Function Signatures

- **MANDATORY**: All components use NO ARGUMENTS pattern:

```cpp
esp_err_t component_name_init(void);  // Returns ESP_OK on success
void component_name_act(void);        // No parameters, accesses global state
```

- **WHY**: Enables dispatch table auto-generation and ensures component isolation.

### Component Isolation

- **NEVER**: Components call each other directly. Use SharedMemory for communication:

```cpp
// ❌ WRONG - Direct component call
goblin_right_eye_act();

// ✅ CORRECT - Use SharedMemory
SharedMemory GSM;
Environment* env = GSM.read<Environment>();
```

### Global State Access

- **REQUIRED**: Components access shared state through global variables:
  - `g_loopCount`: Current loop iteration (uint64_t)
  - `g_shared_state`: Global shared memory instance
- **WHY**: Eliminates parameter passing overhead in real-time system.

### Timing Control

- **REQUIRED**: Use `g_loopCount` for timing, not delays:

```cpp
void component_act(void) {
    if (g_loopCount % 30 == 0) {  // Every 30 loops ≈ 500ms at 60Hz
        // Do periodic work
    }
}
```

### Memory Management

- **FORBIDDEN**: Dynamic allocation in hot paths. Use static buffers:

```cpp
static uint8_t* animation_buffer = nullptr;  // ✅ OK
// std::vector<uint8_t> buffer;  // ❌ FORBIDDEN
```

## 🔄 SharedMemory API Rules

### Reading Shared State

- **REQUIRED**: Use template read method:

```cpp
SharedMemory GSM;
Mood* curMood = GSM.read<Mood>();
if (curMood != nullptr) {
    uint8_t current_mood = curMood->mood_value;
}
```

- **WHY**: Template-based type safety eliminates string parameter errors.

### Writing Shared State

- **REQUIRED**: Use write method to broadcast changes:

```cpp
SharedMemory GSM;
Mood* curMood = GSM.read<Mood>();
if (curMood != nullptr) {
    curMood->mood_value = new_mood;
    GSM.write<Mood>();  // Broadcast to all chips
}
```

### Data Types

- **SUPPORTED**: POD types and simple structs. No complex objects with pointers.
- **WHY**: ESP-NOW requires serializable data structures.

## 📊 Dispatch Table Generation Rules

### Auto-Generation Process

- **REQUIRED**: Use Python tool to generate dispatch tables:

```bash
python tools/generate_tables.py goblin_full src
```

- **WHY**: Maintains consistency between JSON configs and C++ code.

### Table Structure

- **MANDATORY**: Tables contain function pointers only:

```cpp
esp_err_t (*initTable[TABLE_SIZE])(void) = {
    heartbeat_init,
    goblin_left_eye_init,
    // ... auto-generated entries
};

void (*actTable[TABLE_SIZE])(void) = {
    heartbeat_act,
    goblin_left_eye_act,
    // ... auto-generated entries
};
```

### Component Registration

- **REQUIRED**: Components listed in JSON config are auto-registered in tables.
- **WHY**: Eliminates manual table maintenance errors.

## 🌐 Multi-Chip Communication Rules

### ESP-NOW Mesh

- **AUTOMATIC**: SharedMemory handles all network communication:
  - `read<T>()`: Retrieves latest value from mesh
  - `write<T>()`: Broadcasts changes to all chips

### Mesh Roles

- **DEFINED**: JSON specifies chip roles:

```json
"mesh_role": "SLAVE_NODE_HIGH_PRIORITY"
```

- **WHY**: Enables hierarchical communication patterns.

### Synchronization

- **GUARANTEED**: SharedMemory ensures eventual consistency across chips.
- **WHY**: Components can assume shared state is current.

## ⚙️ JSON Configuration Rules

### File Structure

- **MANDATORY**: All JSON files include metadata:

```json
{
    "relative_filename": "config/components/...",
    "version": "1.0.0",
    "author": "config/author.json"
}
```

### Coordinate Systems

- **REQUIRED**: Specify coordinate system and units:

```json
"coordinate_system": "skull_3d",
"reference_point": "nose_center",
"units": "MILLIMETERS"
```

### Component Composition

- **HIERARCHICAL**: Components reference other components:

```json
"contained_components": [
    "config/components/hardware/gc9a01_display.json",
    "config/components/creature_specific/goblin_eye.json"
]
```

## 🔌 Hardware Interface Rules

### SPI Bus Sharing

- **REQUIRED**: Multiple devices on shared SPI bus with unique CS pins.
- **WHY**: Maximizes pin utilization on ESP32-S3.

### I2S Audio

- **SHARED**: I2S bus shared across audio components with unique pins.
- **WHY**: Enables multiple audio sources.

### Pin Aliasing

- **MANDATORY**: Use pin aliases instead of GPIO numbers:

```cpp
#define PIN_SPI_CS_EYE_LEFT GPIO_NUM_5
```

- **WHY**: Enables hardware abstraction and remapping.

## 🎨 Animation & Display Rules

### Buffer Management

- **STATIC**: Use static buffers for animation data:

```cpp
static uint8_t* left_eye_buffer = nullptr;
```

- **WHY**: Prevents memory fragmentation in real-time system.

### Frame Timing

- **LOOP-BASED**: Use loop counter for frame advancement:

```cpp
if (g_loopCount % 30 == 0) {
    current_frame = (current_frame + 1) % 6;
}
```

### Color Palettes

- **MOOD-BASED**: Colors change based on creature mood:

```json
"mood_color_mapping": {
    "ANGER": {"primary_range": "96-127"}
}
```

## 🧪 Testing & Validation Rules

### Component Testing

- **ISOLATED**: Test components independently using mock SharedMemory.
- **WHY**: Ensures component isolation is maintained.

### Multi-Chip Testing

- **REQUIRED**: Use `test/distance_eye_test/` for mesh validation.
- **WHY**: Validates SharedMemory synchronization.

### Build Validation

- **MANDATORY**: Run `pio run` before committing.
- **WHY**: Catches compilation errors early.

## 📝 Code Style Rules

### ASCII Encoding

- **MANDATORY**: All files in ASCII encoding only.
- **WHY**: ESP-IDF toolchain requirement.

### Naming Conventions

- **REQUIRED**:
  - Functions: `snake_case`
  - Files: `lowercase`
  - Classes: `PascalCase`
  - Globals: `g_camelCase`

### Include Guards

- **REQUIRED**: Use `#pragma once` or standard guards.
- **WHY**: Prevents multiple inclusion errors.

## 🔄 Development Workflow Rules

### Feature Development

1. **Design**: Create JSON config first
2. **Implement**: Write component with init/act functions
3. **Generate**: Run `python tools/generate_tables.py`
4. **Build**: `pio run -t upload -t monitor`
5. **Test**: Validate on hardware

### Code Changes

- **NEVER**: Modify existing code without user permission.
- **WHY**: Preserves architectural integrity.

### Documentation

- **REQUIRED**: Update this rules document for any architectural changes.
- **WHY**: Maintains system consistency.## 🚫 Forbidden Patterns

### Direct Component Calls
```cpp
// ❌ NEVER DO THIS
goblin_right_eye_act();
other_component_init();
```

### Dynamic Allocation in Hot Paths
```cpp
// ❌ NEVER DO THIS
std::vector<uint8_t> buffer;
auto* ptr = new uint8_t[size];
```

### Blocking Operations
```cpp
// ❌ NEVER DO THIS
vTaskDelay(100);
while (!condition) { }
```

### Direct ESP-NOW Usage
```cpp
// ❌ NEVER DO THIS
esp_now_send(peer_addr, data, size);
```

## ✅ Recommended Patterns

### Shared State Access

```cpp
SharedMemory GSM;
Environment* env = GSM.read<Environment>();
if (env != nullptr) {
    // Use env->field
    GSM.write<Environment>();  // If modified
}
```

### Recommended Timing Control

```cpp
static uint32_t last_update = 0;
if (g_loopCount - last_update >= 60) {  // Every second at 60Hz
    last_update = g_loopCount;
    // Do periodic work
}
```

### Recommended Buffer Management

```cpp
static uint8_t* buffer = nullptr;
if (buffer == nullptr) {
    buffer = getBuffer();  // One-time allocation
}
// Use buffer...
```

## 📋 Checklist for New Components

- [ ] JSON config created with required metadata
- [ ] Component implements `init()` and `act()` with NO ARGUMENTS
- [ ] Uses SharedMemory for inter-component communication
- [ ] Accesses global state via `g_loopCount`
- [ ] No dynamic allocation in hot paths
- [ ] Uses static buffers for data
- [ ] Tested in isolation
- [ ] Dispatch tables regenerated
- [ ] Builds successfully
- [ ] Validated on hardware

## 🔍 Debugging Rules

### Log Levels
- **REQUIRED**: Use ESP_LOG macros:
```cpp
ESP_LOGI(TAG, "Component initialized");
ESP_LOGE(TAG, "Error: %s", error_msg);
```

### Memory Debugging
- **REQUIRED**: Monitor heap usage:
```cpp
ESP_LOGI(TAG, "Free heap: %d bytes", esp_get_free_heap_size());
```

### Network Debugging
- **REQUIRED**: Monitor ESP-NOW traffic:
```cpp
ESP_LOGD(TAG, "Received data from mesh");
```

---

**REMEMBER**: This document is the single source of truth for P32 architecture. All rules are mandatory unless explicitly overridden by user instruction. When in doubt, ask the user for clarification rather than making assumptions.
