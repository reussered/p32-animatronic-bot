# Multi-ESP32 Distributed System Design

## Design Goals

1. **Single Codebase**: One main.cpp runs on ALL ESP32 chips
2. **JSON-Driven Configuration**: Each ESP32 gets different components via JSON
3. **Dynamic Subsystem Discovery**: Number and type of subsystems determined by JSON tree analysis
4. **Hierarchical Chip Assignment**: Components inherit chip assignment from parent if not specified
5. **Development Workflow**: Easy testing of individual subsystems during development
6. **Architecture Flexibility**: Adding wrist controllers or changing subsystem boundaries only requires JSON changes

## System Architecture

### Component Hierarchy & Chip Assignment

**Example Component Tree:**
```
goblin_full                    // Top level - no chip assignment
 goblin_head               // chip: "esp32-s3-n16r8"
    goblin_left_eye       // inherits: esp32-s3-n16r8
       goblin_eye        // inherits: esp32-s3-n16r8  
       gc9a01            // inherits: esp32-s3-n16r8
    goblin_right_eye      // inherits: esp32-s3-n16r8
       goblin_eye        // inherits: esp32-s3-n16r8 (DUPLICATE)
       gc9a01            // inherits: esp32-s3-n16r8 (DUPLICATE)
    goblin_nose           // inherits: esp32-s3-n16r8
 goblin_torso              // chip: "esp32-s3-n16r8"  
    spine_servo_1         // inherits: esp32-s3-n16r8
    power_management      // inherits: esp32-s3-n16r8
 left_arm                  // chip: "esp32-c3"
     shoulder_servo        // inherits: esp32-c3
     elbow_servo           // inherits: esp32-c3
```

### Critical Architecture Rules

1. **Duplicate Dispatch Entries Allowed**: When `goblin_eye` appears in both left and right eye branches, it gets **duplicate entries** in the head dispatch table
2. **Unique Component Functions**: Each unique component gets **one function definition** per subsystem (no duplicates in `head_component_functions.cpp`)
3. **No Inter-Component Calls**: Components **never call other component functions** - only main.cpp calls component functions
4. **Subsystem Isolation**: Each subsystem runs independently with its own dispatch table and component functions

## Component Hierarchy & Function Call Rules

### **CRITICAL ARCHITECTURE RULE**: 
**Components NEVER call other component functions directly. ALL component functions are ONLY called from main.cpp via dispatch tables.**

### Component Tree Example

```
goblin_full
 goblin_head (chip: esp32-s3-n16r8)
    goblin_left_eye
       goblin_eye
          gc9a01
       gc9a01
    goblin_right_eye  
       goblin_eye     [DUPLICATE]
          gc9a01     [DUPLICATE]
       gc9a01         [DUPLICATE]
    goblin_nose
 goblin_torso (chip: esp32-s3-n16r8)
    power_management
    spine_servos
 left_arm (chip: esp32-c3)
     shoulder_servo
     elbow_servo
```

### Dispatch Table Generation Rules

#### Rule 1: Duplicate Entries in Dispatch Tables
When `goblin_eye` and `gc9a01` appear in multiple branches, they get **multiple entries** in dispatch tables:

```cpp
// head_dispatch_tables.cpp - Multiple entries for duplicated components
p32_component_init_t init_table[] = {
    goblin_left_eye_init,
    goblin_eye_init,        // First instance  
    gc9a01_init,           // First instance
    goblin_right_eye_init,
    goblin_eye_init,        // DUPLICATE ENTRY - Second instance
    gc9a01_init,           // DUPLICATE ENTRY - Second instance  
    goblin_nose_init
};

p32_component_act_t act_table[] = {
    goblin_left_eye_act,
    goblin_eye_act,         // First instance
    gc9a01_act,            // First instance  
    goblin_right_eye_act,
    goblin_eye_act,         // DUPLICATE ENTRY - Second instance
    gc9a01_act,            // DUPLICATE ENTRY - Second instance
    goblin_nose_act
};
```

#### Rule 2: Function Definitions - No Duplication Within Subsystem
Each function is defined only **once per subsystem**, regardless of how many dispatch table entries:

```cpp
// head_component_functions.cpp - Functions defined only once per subsystem
void goblin_eye_init(void) {
    // Defined only once, even though called twice from dispatch table
}

void goblin_eye_act(void) {
    // Defined only once, even though called twice from dispatch table  
}

void gc9a01_init(void) {
    // Defined only once, even though called twice from dispatch table
}

void gc9a01_act(void) {
    // Defined only once, even though called twice from dispatch table
}
```

#### Rule 3: Cross-Subsystem Function Duplication Allowed
If `gc9a01` appears in different subsystems, it gets separate function definitions:

```cpp
// head_component_functions.cpp  
void gc9a01_init(void) { /* Head subsystem implementation */ }
void gc9a01_act(void) { /* Head subsystem implementation */ }

// left_arm_component_functions.cpp (if it has a display)
void gc9a01_init(void) { /* Left arm subsystem implementation */ }  
void gc9a01_act(void) { /* Left arm subsystem implementation */ }
```

### main.cpp Architecture

```cpp
// main.cpp - ONLY place component functions are called
void app_main() {
    // Phase 1: Initialize all components for this subsystem
    for (int i = 0; i < init_table_size; i++) {
        init_table[i]();  // Call each init function
    }
    
    // Phase 2: Main execution loop
    while(1) {
        for (int i = 0; i < act_table_size; i++) {
            act_table[i]();  // Call each act function
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
```

### Component Isolation Rules

####  ALLOWED:
```cpp
void goblin_eye_act(void) {
    // Access global shared state
    auto shared_data = GSM.read<SomeDataType>();
    
    // Modify local variables
    static int frame_counter = 0;
    frame_counter++;
    
    // Update shared state
    GSM.write<SomeDataType>(modified_data);
}
```

####  FORBIDDEN:
```cpp
void goblin_eye_act(void) {
    // NEVER call other component functions directly
    gc9a01_act();           //  FORBIDDEN
    goblin_nose_act();      //  FORBIDDEN
    power_management_init(); //  FORBIDDEN
}
```

### Communication Between Components

**ONLY through shared state via GSM (Global Shared Memory)**:

```cpp
// Component A writes data
void component_a_act(void) {
    EyePosition pos = {x: 10, y: 20};
    GSM.write<EyePosition>(pos);
}

// Component B reads data (in same or different loop iteration)
void component_b_act(void) {
    auto pos = GSM.read<EyePosition>();
    // Use pos.x and pos.y
}
```

## File Generation Strategy

### Input: Single Bot JSON
```
config/bots/goblin_full.json   Contains ALL subsystems
```

### Output: Per-Subsystem Files
```
src/subsystems/
 head/
    head_dispatch_tables.hpp
    head_dispatch_tables.cpp
    head_component_functions.hpp
    head_component_functions.cpp
 torso/
    torso_dispatch_tables.hpp
    torso_dispatch_tables.cpp
    torso_component_functions.hpp
    torso_component_functions.cpp
 left_arm/
    left_arm_dispatch_tables.hpp
    left_arm_dispatch_tables.cpp
    left_arm_component_functions.hpp
    left_arm_component_functions.cpp
 ... (7 more subsystems)
```

## Build System Integration

### PlatformIO Environments
```ini
[env:head_esp32s3]
board = esp32-s3-devkitc-1
build_flags = -DSUBSYSTEM=HEAD
build_src_filter = +<*> +<subsystems/head/>

[env:torso_esp32s3] 
board = esp32-s3-devkitc-1
build_flags = -DSUBSYSTEM=TORSO
build_src_filter = +<*> +<subsystems/torso/>

[env:left_arm_esp32c3]
board = esp32-c3-devkitm-1
build_flags = -DSUBSYSTEM=LEFT_ARM
build_src_filter = +<*> +<subsystems/left_arm/>
```

### Universal main.cpp
```cpp
#include "p32_shared_state.h"

#ifdef SUBSYSTEM_HEAD
#include "subsystems/head/head_dispatch_tables.hpp"
#elif defined(SUBSYSTEM_TORSO)
#include "subsystems/torso/torso_dispatch_tables.hpp"
#elif defined(SUBSYSTEM_LEFT_ARM)
#include "subsystems/left_arm/left_arm_dispatch_tables.hpp"
// ... etc
#endif

void app_main() {
    // Initialize all components for this subsystem
    p32_init_all_components();
    
    // Main loop - execute all components for this subsystem
    while(1) {
        p32_execute_all_components();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
```

## Development Workflow

## Implementation Strategy

### Phase 1: JSON Tree Analysis Engine

```python
class SubsystemDiscovery:
    def discover_subsystems(self, bot_json_path):
        """
        Scan JSON tree to find all nodes with 'chip' assignments
        Returns: {subsystem_name: {chip_type, component_instances[]}}
        """
        subsystems = {}
        self.walk_json_tree(bot_json, subsystems)
        return subsystems
        
    def walk_json_tree(self, node, subsystems, current_chip=None, instance_path=[]):
        """
        Recursively walk JSON tree, tracking chip assignments and component instances
        """
        # Check if this node defines a chip
        if 'chip' in node:
            current_chip = node['chip']
            subsystem_name = self.get_subsystem_name(node)
            subsystems[subsystem_name] = {
                'chip_type': self.normalize_chip_name(current_chip),
                'component_instances': [],  # Track ALL instances
                'unique_components': set()  # Track unique component types
            }
            
        # Process components - they inherit current_chip
        if 'component_name' in node and current_chip:
            subsystem_name = self.find_controlling_subsystem(current_chip, subsystems)
            component_instance = {
                'name': node['component_name'],
                'instance_path': instance_path.copy(),  # Track where this instance appears
                'data': node
            }
            subsystems[subsystem_name]['component_instances'].append(component_instance)
            subsystems[subsystem_name]['unique_components'].add(node['component_name'])
            
        # Recurse into child nodes
        for child in self.get_child_nodes(node):
            child_path = instance_path + [node.get('component_name', 'unknown')]
            self.walk_json_tree(child, subsystems, current_chip, child_path)
```

### Phase 2: Dispatch Table Generation with Duplicates

```python
class DispatchTableGenerator:
    def generate_dispatch_tables(self, subsystem_name, subsystem_data):
        """
        Generate dispatch tables with duplicate entries for component instances
        """
        # Generate dispatch table with ALL instances (including duplicates)
        init_entries = []
        act_entries = []
        
        for instance in subsystem_data['component_instances']:
            component_name = instance['name']
            safe_name = self.sanitize_component_name(component_name)
            
            # Add entry for each instance (creates duplicates)
            init_entries.append(f"{safe_name}_init")
            act_entries.append(f"{safe_name}_act")
            
        return self.create_dispatch_table_code(init_entries, act_entries)
        
    def generate_component_functions(self, subsystem_name, subsystem_data):
        """
        Generate function definitions for unique components only (no duplicates)
        """
        function_definitions = []
        
        # Only generate functions for unique components
        for component_name in subsystem_data['unique_components']:
            safe_name = self.sanitize_component_name(component_name)
            
            # Generate init function (once per unique component)
            init_function = f"""
void {safe_name}_init(void) {{
    // Component initialization logic
    // This function may be called multiple times from dispatch table
    // but is defined only once per subsystem
}}
"""
            
            # Generate act function (once per unique component)  
            act_function = f"""
void {safe_name}_act(void) {{
    // Component execution logic
    // This function may be called multiple times from dispatch table
    // but is defined only once per subsystem
}}
"""
            
            function_definitions.append(init_function)
            function_definitions.append(act_function)
            
        return function_definitions
```

### Phase 3: Component Isolation Enforcement

```python
class ComponentIsolationValidator:
    def validate_component_functions(self, file_path):
        """
        Ensure component functions never call other component functions
        """
        with open(file_path, 'r') as f:
            content = f.read()
            
        # Check for forbidden function calls
        forbidden_calls = self.find_component_function_calls(content)
        
        if forbidden_calls:
            raise ValidationError(f"Component isolation violation in {file_path}: {forbidden_calls}")
            
    def find_component_function_calls(self, content):
        """
        Look for component_name_init() or component_name_act() calls
        """
        pattern = r'(\w+)_(init|act)\s*\('
        matches = re.findall(pattern, content)
        
        # Filter out allowed calls (like GSM.read, etc.)
        forbidden = [match for match in matches if self.is_component_function(match[0])]
        return forbidden
```

## Development Workflow

### 1. Design Phase 
- [x] Create dynamic architecture design
- [ ] Validate JSON tree analysis approach
- [ ] Plan implementation phases

### 2. JSON Tree Analysis Engine
- [ ] Implement recursive JSON tree walker
- [ ] Create chip assignment inheritance logic
- [ ] Build subsystem discovery algorithm
- [ ] Test with different JSON configurations

### 3. Dynamic Code Generation Engine  
- [ ] Generate subsystem-specific dispatch tables
- [ ] Generate subsystem-specific component function declarations
- [ ] Create dynamic file naming system
- [ ] Handle variable number of subsystems

### 4. Dynamic Build System Integration
- [ ] Generate PlatformIO environments dynamically
- [ ] Set up conditional compilation per subsystem
- [ ] Create upload scripts for variable chip count

### 5. Testing Strategy
- [ ] Test with 3-chip minimal configuration
- [ ] Test with 9-chip full body configuration  
- [ ] Test with 11-chip wrist controller configuration
- [ ] Validate JSON changes automatically create new subsystems

## Key Design Questions

### Q1: How to handle shared components?
**Example**: `goblin_personality` used by multiple subsystems

**Options**:
A) Duplicate in each subsystem (simple)
B) Designate "owner" subsystem and use ESP-NOW communication
C) Replicate with synchronization

**Recommendation**: Start with A (duplication) for simplicity

### Q2: How to handle system-level components?
**Example**: `wifi_station`, `esp_now_mesh` should only run on TORSO

**Solution**: System components have explicit `chip: "torso_esp32s3"` assignment

### Q3: How to handle component dependencies?
**Example**: `left_eye` contains `gc9a01` display driver

**Solution**: Both components go to same chip (inheritance), dependency satisfied locally

## Success Metrics

1.  **Single JSON Input**: One `goblin_full.json` generates all subsystem files
2.  **Correct Distribution**: Head gets 36 components, other subsystems get appropriate counts  
3.  **Build Independence**: Each subsystem compiles independently
4.  **Upload Workflow**: Can flash all 9 ESP32s with single command
5.  **Development Testing**: Can test individual subsystems on breadboard

## Next Steps

1. **User Validation**: Get approval on this design approach
2. **Implementation Phasing**: Break into smaller, testable chunks
3. **Component Analysis**: Analyze existing goblin components to validate chip assignments
4. **Prototype**: Start with 2-chip system (head + torso) before scaling to 9

---

**This design should be approved before any implementation begins.**