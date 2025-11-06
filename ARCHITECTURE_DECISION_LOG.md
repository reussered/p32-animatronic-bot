# ARCHITECTURE DECISION LOG

## Purpose

This log captures key architectural decisions, problem-solving processes, and implementation choices made during P32 Animatronic Bot development. **Future agents should read this first** to understand project context and avoid re-teaching solved problems.

## Decision: Multi-Agent Development Strategy (2025-06-XX)

### Problem
- Complex multi-subsystem project with interdependent components
- Single developer bottleneck in development velocity
- Risk of architectural inconsistencies across subsystems
- Need for parallel development while maintaining system coherence

### Solution Chosen
- **Parallel AI agent development** with 7 specialized agents
- **Subsystem-focused agents**: Head, Torso, Arms, Legs, Hands/Feet, Audio/Communication, Testing
- **Coordination protocols** for architectural consistency
- **Shared standards** enforced across all agents

### Reasoning
- Parallel development maximizes velocity on complex systems
- Specialization allows deep expertise in each subsystem
- Coordination protocols prevent architectural drift
- Agent handoffs allow continuous development cycles

### Implementation Details
- Each agent focuses on specific subsystems (head, torso, limbs, etc.)
- Shared architecture standards enforced via coordination protocols
- Regular integration testing to catch inconsistencies
- Documentation-driven development with clear interfaces

### Files Affected
- `MULTI_AGENT_DEVELOPMENT_STRATEGY.md` - Strategy documentation
- Agent assignment protocols and coordination rules

## Decision: Multi-ESP32 Distributed Architecture (2025-07-XX)

### Problem
- Single ESP32 insufficient for full animatronic bot complexity
- Need distributed processing across multiple chips
- Component hierarchy requires intelligent chip assignment
- Development workflow needs subsystem isolation for testing

### Solution Chosen
- **JSON-driven chip assignment** with hierarchical inheritance
- **Single codebase** running on all ESP32 chips
- **Subsystem isolation** via dispatch table generation
- **Duplicate component handling** in dispatch tables (allowed)
- **No inter-component function calls** - only main.cpp calls components

### Reasoning
- JSON configuration allows flexible subsystem boundaries
- Single codebase simplifies maintenance and updates
- Dispatch table duplication reflects physical component reuse
- Component isolation prevents tight coupling and race conditions
- Hierarchical inheritance reduces configuration verbosity

### Implementation Details
- Components inherit chip assignment from parent if not specified
- `generate_tables.py` creates subsystem-specific dispatch tables
- Duplicate entries in dispatch tables for reused components
- Function definitions unique per subsystem (no duplicates within subsystem)
- Cross-subsystem function duplication allowed when needed

### Files Affected
- `MULTI-ESP32-DESIGN.md` - Complete architecture specification
- `tools/generate_tables.py` - Dispatch table generation
- `config/bots/bot_families/**/*.json` - Chip assignment configurations
- `src/subsystems/**/dispatch_tables.cpp` - Generated dispatch tables

## Decision: Component-Based Architecture (2025-08-XX)

### Problem
- Traditional monolithic embedded code becomes unmaintainable at scale
- Need modular, reusable components for complex animatronic systems
- Component integration and testing challenges
- Hardware abstraction requirements for different ESP32 variants

### Solution Chosen
- **Pure component system**: Everything is a component with `init()`/`act()` functions
- **JSON-driven composition**: Recursive component inclusion via JSON files
- **Three-level hierarchy**: System, Family, Bot-Specific components
- **SharedMemory coordination**: Components communicate via GSM.read()/write()
- **No direct function calls**: Components never call each other

### Reasoning
- Component modularity enables reuse across different bot configurations
- JSON composition allows runtime configuration without recompilation
- Three-level hierarchy organizes components by scope and purpose
- SharedMemory prevents tight coupling while enabling coordination
- Function call isolation ensures predictable execution and testing

### Implementation Details
- Each component: `{name}.json`, `{name}.src`, `{name}.hdr`
- Recursive JSON composition: `goblin_head.json` contains `goblin_left_eye.json`
- SharedMemory API: `GSM.write<Mood>()` broadcasts to all chips
- Main loop: Only iterates components, no application logic in `app_main()`
- Auto-generated dispatch tables from JSON analysis

### Files Affected
- `config/components/**/*.json` - Component definitions
- `config/components/**/*.src` - Component implementations
- `config/components/**/*.hdr` - Component interfaces
- `tools/generate_tables.py` - Component aggregation and dispatch generation

## Decision: SharedMemory Usage Patterns (2025-10-XX)

### Problem
- Need clear guidelines for when to use SharedMemory vs local subsystem memory
- Risk of overusing SharedMemory for data that doesn't need cross-subsystem visibility
- Performance implications of unnecessary inter-chip communication
- Confusion about data ownership and update responsibilities

### Solution Chosen
- **SharedMemory ONLY for cross-subsystem data** - data that needs to be visible across different ESP32 chips/subsystems
- **Local subsystem memory for intra-subsystem coordination** - data shared only within components on the same chip
- **Data ownership by subsystem** - each data type has a primary "owner" subsystem that writes it
- **Read-everywhere access** - any subsystem can read SharedMemory data, but only owner writes

### Reasoning
- Performance optimization: Avoid unnecessary inter-chip communication overhead
- Data consistency: Clear ownership prevents conflicting updates
- Scalability: Local coordination is faster and doesn't burden the mesh network
- Architecture clarity: Explicit distinction between local and global data scopes

### Implementation Details
- **Environment state** (power system, temperature, etc.) owned by torso subsystem, read by all subsystems
- **Mood state** owned by torso subsystem (power-based modulation) AND head subsystem (sensory-based updates), read by all subsystems
- **Position data** owned by respective limb subsystems, read by coordination systems
- **Local coordination** (eye synchronization, arm gesture coordination) uses direct memory within subsystem
- **Cross-subsystem mood modulation**: Torso reads Environment.power_level to modulate mood intensity
- **Power-aware behavior cascade**: Low power  subdued mood  sad expressions  shivering  collapse sequence

### Power-Aware Mood Behavior Examples
```cpp
// Torso subsystem - Power-aware mood modulation component
void goblin_power_mood_act(void) {
    Environment *env = GSM.read<Environment>();
    Mood *mood = GSM.read<Mood>();
    
    static Mood last_mood = *mood;  // Track previous mood state
    
    // Power level affects mood intensity and emotional state
    if (env->power_level < 0.2f) {           // Critical power
        mood->intensity *= 0.3f;             // Much more subdued
        mood->sadness = 1.0f;                // Extremely sad
        mood->energy = 0.1f;                 // Very low energy
        // Triggers: shivering, whimpering, collapse behaviors
    } else if (env->power_level < 0.5f) {    // Low power  
        mood->intensity *= 0.7f;             // Moderately subdued
        mood->sadness = 0.8f;                // Quite sad
        mood->energy = 0.4f;                 // Low energy
    }
    
    // Only write to SharedMemory if mood actually changed
    if (memcmp(mood, &last_mood, sizeof(Mood)) != 0) {
        GSM.write<Mood>();  // Broadcast updated mood to all subsystems
        last_mood = *mood;  // Update tracking
    }
}

// SharedMemory - Cross-subsystem (power state known everywhere)
Environment *env = GSM.read<Environment>();  // Read power state in any subsystem
// Only power component (torso) does: GSM.write<Environment>();

// SharedMemory - Cross-subsystem (mood known everywhere)  
Mood *mood = GSM.read<Mood>();  // Read mood in any subsystem
// Only mood-updating components (head) do: GSM.write<Mood>();

// Local memory - Intra-subsystem (eye coordination on head chip)
static eye_position_t left_eye_pos, right_eye_pos;  // Shared within head subsystem only
```

### Behavioral Cascade Under Low Power
1. **Power monitoring** (torso)  Environment.power_level drops
2. **Mood modulation** (head)  Reads power, adjusts sadness/energy  
3. **Facial expressions** (head)  Extremely sad face based on mood
4. **Audio responses** (head)  Pitiful whimpering sounds
5. **Physical behavior** (torso/limbs)  Shivering, eventual collapse
6. **Recovery behavior** (all)  Power restoration triggers mood recovery

### Files Affected
- `include/components/SharedMemory.hpp` - SharedMemory class implementation
- `src/components/SharedMemory.cpp` - ESP-NOW broadcast implementation
- Component implementations using appropriate memory scopes

## Future SharedMemory Enhancements (2025-11-XX)

### Proposed Features
- **Global timestamp**: Track last time any SharedMemory variable was modified
- **Change history stack**: Maintain last 4-8 changed variables with timestamps
- **Rate limiting**: Prevent rapid mood changes during power fluctuations

### Use Cases
- **Power-aware stability**: When power drops rapidly, mood changes are dampened by checking recent change history
- **Debugging**: Track what variables changed and when for system analysis
- **Behavioral smoothing**: Prevent erratic behavior during environmental fluctuations

### Implementation Sketch
```cpp
// Memory-efficient SharedMemory with minimal static allocation
enum class SharedMemoryType : uint8_t {
    Mood = 0,
    Environment = 1, 
    Position = 2,
    AudioBuffer = 3,
    // ... expandable as needed
};

class SharedMemory {
    // Current functionality...
    
    // Future enhancements - minimal static footprint
    uint32_t last_change_timestamp;
    
    // Option A: Minimal static buffer (4 entries) with overwrite
    static constexpr size_t MAX_HISTORY = 4;  // Conservative sizing
    struct ChangeEntry {
        SharedMemoryType type_id;
        uint32_t timestamp;
    };
    ChangeEntry change_history[MAX_HISTORY];  // Fixed static array
    size_t history_index = 0;  // Ring buffer index
    
    // Option B: Lazy dynamic allocation (only when needed)
    // std::unique_ptr<ChangeEntry[]> change_history;
    // size_t allocated_size = 0;
    
    template<typename T>
    void write(const T& data) {
        // Update global timestamp
        last_change_timestamp = esp_timer_get_time() / 1000;  // milliseconds
        
        // Ring buffer update (static allocation)
        change_history[history_index].type_id = getTypeId<T>();
        change_history[history_index].timestamp = last_change_timestamp;
        history_index = (history_index + 1) % MAX_HISTORY;
        
        // Broadcast to other chips...
    }
    
private:
    template<typename T>
    static SharedMemoryType getTypeId() {
        if constexpr (std::is_same_v<T, Mood>) return SharedMemoryType::Mood;
        if constexpr (std::is_same_v<T, Environment>) return SharedMemoryType::Environment;
        // ... compile-time mapping
    }
};
```

### Benefits
- **Behavioral stability**: Mood won't fluctuate wildly during power transitions
- **Performance optimization**: Reduce unnecessary network traffic
- **Debug capabilities**: Historical change tracking for troubleshooting
- **Communication resilience**: Robust handling of ESP-NOW failures

### Communication Failure Handling
**CRITICAL**: ESP-NOW communication can fail due to interference, distance, or hardware issues.

**Failure Scenarios:**
- **Write timeouts**: SharedMemory broadcast doesn't reach all chips
- **Partial reception**: Some chips receive update, others don't
- **Network congestion**: High traffic causes dropped packets
- **Hardware failures**: Individual chips become unreachable

**Required Safeguards:**
- **Acknowledgment system**: Confirm receipt from all expected chips
- **Timeout handling**: Retry logic with exponential backoff
- **Fallback modes**: Local operation when network unavailable
- **Consistency checks**: Detect and resolve data divergence
- **Health monitoring**: Track communication reliability per chip

**Implementation Pattern:**
```cpp
template<typename T>
esp_err_t SharedMemory::write(const T& data) {
    // Attempt broadcast with timeout
    esp_err_t result = esp_now_send_broadcast(&data, sizeof(T), BROADCAST_TIMEOUT_MS);
    
    if (result != ESP_OK) {
        // Log failure, attempt retry, enter degraded mode
        return handle_communication_failure(result);
    }
    
    // Wait for acknowledgments from all chips
    if (!wait_for_acknowledgments(ALL_CHIPS_TIMEOUT_MS)) {
        // Some chips didn't acknowledge - handle partial failure
        return handle_partial_failure();
    }
    
    return ESP_OK;
}
```

## Decision: Dynamic Pin Assignment System (2025-10-30)

### Problem

- Multiple components needed to share GPIO pins without conflicts
- Static pin assignments at build time were inflexible
- No mechanism to prevent runtime pin conflicts between components

### Solution Chosen

- **Dynamic runtime pin assignment** during component `init()` functions
- **Global `assigned_pins[]` array** tracks all assigned pins across all bus types
- **Round-robin assignment** from dedicated assignable arrays per bus type
- **Permanent assignments** - pins are never released (reflects physical hardware reality)

### Reasoning

- Hardware-first design: Pin assignments represent physical connections that don't change at runtime
- Conflict prevention: Single source of truth for all pin assignments
- Flexibility: Components can be added/removed without rebuild
- Bus isolation: Each bus type has its own assignable array but shares global conflict tracking

### Implementation Details

- `assign_pin(assignable_array, count)` function assigns first available pin
- Bus controllers (SPI, I2C, ADC) provide current pin sets to generic drivers
- Multiply-capable pins assigned LAST (preserved for maximum flexibility)

### Files Affected

- `include/esp32_s3_r8n16_pin_assignments.h` - Pin arrays and global tracking
- `src/components/*/pin_assignment.c` - Assignment logic
- `PIN_ASSIGNMENT_RULES.md` - Detailed rules documentation

## Decision: ADC Bus Architecture (2025-10-30)

### Problem
- Microphone driver used direct ADC access instead of standardized bus pattern
- Inconsistent with SPI/I2C bus implementations
- No pin cycling or conflict management for ADC resources

### Solution Chosen
- **ADC bus controller** following exact SPI bus pattern
- **Pin cycling in `act()` function** to provide different ADC pins to different components
- **Generic microphone driver** using `adc_bus_get_current_pins()` API
- **ADC assignable array** with multiply-capable pins at end

### Reasoning
- Consistency: All bus types use identical patterns (init assigns pins, act cycles through sets)
- Resource management: ADC pins managed like other shared resources
- Driver abstraction: Generic drivers work with any bus implementation
- Hardware optimization: Pin cycling allows multiple ADC devices to share bus

### Implementation Details
- `adc_bus_init()` assigns pins from `adc_assignable[]` array
- `adc_bus_act()` cycles through assigned pin sets, resets index when reaching end
- `adc_bus_get_current_pins()` provides current pin set to generic drivers
- HW496 microphone uses generic ADC driver instead of direct ADC access

### Files Affected
- `config/components/interfaces/adc_bus.src` - ADC bus implementation
- `config/components/interfaces/adc_bus.hdr` - ADC bus API
- `config/components/drivers/generic_mic_driver.src` - Generic microphone driver
- `src/components/hw496_microphone.cpp` - Updated to use generic driver

## Decision: Multiply-Capable Pins Assigned Last (2025-10-30)

### Problem
- Precious multiply-capable pins (GPIO_NUM_2,4,5,6,7,8,9,10,38,39) were being assigned first
- These pins can serve SPI, I2C, PWM, ADC, GPIO - maximum flexibility
- Once assigned to one bus, unavailable for other uses

### Solution Chosen
- **Reorder all assignable arrays** to put multiply-capable pins at the END
- **Specialized pins first**: GPIO_NUM_11-18, 21, 40-44 (limited alternative functions)
- **Multiply-capable pins last**: GPIO_NUM_2,4,5,6,7,8,9,10,38,39 (maximum flexibility)

### Reasoning
- Resource preservation: Save flexible pins for buses that need them most
- Maximum utilization: Specialized pins used when available, flexible pins as fallback
- Future-proofing: Preserves options for adding new bus types or components

### Implementation Details
- All bus arrays reordered: SPI, I2C, I2S, PWM, ADC
- ADC array: ADC-specific pins (1,3,32-37) first, multiply-capable pins last
- Other arrays: GPIO-specialized pins first, multiply-capable pins last

### Files Affected
- `include/esp32_s3_r8n16_pin_assignments.h` - All assignable arrays reordered
- `PIN_ASSIGNMENT_RULES.md` - Formalized rules documentation

## Decision: Component Architecture Pattern (2025-XX-XX)

### Problem
- Need consistent component lifecycle and communication
- Components should be independent but coordinate when needed
- Real-time constraints require predictable execution patterns

### Solution Chosen
- **Pure component system**: Everything is a component with `init()`/`act()` functions
- **SharedMemory API**: Components communicate via `GSM.read<T>()`/`GSM.write<T>()`
- **No direct calls**: Components never call each other directly
- **Main loop iteration**: `app_main()` only iterates components, no application logic

### Reasoning
- Modularity: Components can be added/removed independently
- Testability: Each component can be tested in isolation
- Multi-chip support: SharedMemory works across ESP32 chips
- Real-time safety: Predictable execution without blocking calls

### Implementation Details
- Each component: `esp_err_t component_init(void)` and `void component_act(void)`
- SharedMemory: `GSM.write<Mood>()` broadcasts to all chips
- Dispatch tables: Auto-generated from JSON configurations
- JSON composition: Recursive inclusion (goblin_head.json contains goblin_eye.json)

### Files Affected
- `config/components/**/*.src` - Component source files
- `config/components/**/*.hdr` - Component headers
- `tools/generate_tables.py` - Auto-generates dispatch tables
- `src/subsystems/**/dispatch_tables.cpp` - Generated dispatch logic

## LESSONS LEARNED

### Documentation is Critical
- **Always document decisions** with problem/solution/reasoning
- **Include implementation details** for future maintenance
- **Create searchable documentation** that agents can read independently

### Hardware-First Design
- **Physical constraints drive architecture** - don't fight the hardware
- **Permanent assignments** reflect physical reality
- **Global conflict tracking** prevents hardware conflicts

### Consistency Matters
- **All bus types use identical patterns** - reduces cognitive load
- **Standardized APIs** make components interchangeable
- **Auto-generated code** ensures consistency

### Future Agent Guidelines
1. **Read this log first** before making changes
2. **Check existing documentation** before asking for explanations
3. **Follow established patterns** unless there's a compelling reason to change
4. **Document new decisions** by adding to this log
5. **Test thoroughly** - hardware issues are expensive to fix







