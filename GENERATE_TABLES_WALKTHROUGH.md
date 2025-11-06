# How generate_tables.py Walks the Component Tree and Generates Dispatch Tables

## Overview

The `tools/generate_tables.py` script performs a **depth-first traversal** of the JSON component tree, collecting components and generating three key files per subsystem:

1. **`{subsystem}_component_functions.cpp`** - Concatenated source code fragments
2. **`{subsystem}_dispatch_tables.cpp`** - Dispatch tables (init, act, hitcount arrays)
3. **`{subsystem}_main.cpp`** - Main loop that uses the dispatch tables

---

## Core Classes and Data Structures

### 1. JsonLoader
```python
class JsonLoader:
    def load(self, path: Path) -> Dict[str, Any]
```
- Caches JSON files
- **ENFORCES ASCII ENCODING** - raises `UnicodeDecodeError` if UTF-8 BOM is detected
- Uses `encoding="ascii"` when opening files

### 2. ComponentSourceIndex
```python
class ComponentSourceIndex:
    def find_source_file(component_name, data, json_path) -> Optional[Path]
    def find_header_file(component_name, data, json_path) -> Optional[Path]
```
- Indexes all `.src` and `.hdr` files by component name
- Searches for source files in three places (in order):
  1. `software.source_file` (explicit reference in JSON)
  2. Same directory as JSON with `.src` extension
  3. Component name index (any `.src` file matching the component name)

### 3. ComponentVisit
```python
@dataclass
class ComponentVisit:
    name: str
    init_func: str
    act_func: str
    hit_count: int
    json_path: Optional[Path]
    template_type: Optional[str]
```
- Represents **one traversal visit** of a component
- Includes duplicates (if a component is referenced twice, there are two visits)
- Used to populate dispatch tables

### 4. ComponentDefinition
```python
@dataclass
class ComponentDefinition:
    name: str
    json_path: Optional[Path]
    data: Dict[str, Any]
    init_func: str
    act_func: str
    hit_count: int
    src_path: Optional[Path]
    hdr_path: Optional[Path]
    src_content: Optional[str] = None
    hdr_content: Optional[str] = None
```
- Represents **unique component** (one per component name, regardless of how many times referenced)
- Stores loaded `.src` and `.hdr` file contents
- Contains the full JSON data

### 5. SubsystemContext
```python
@dataclass
class SubsystemContext:
    name: str
    controller: str
    json_path: Optional[Path]
    visits: List[ComponentVisit] = []              # ALL visits including duplicates
    unique_components: Dict[str, ComponentDefinition] = {}  # Only unique ones
```
- Represents one subsystem (identified by `"controller"` field in JSON)
- **TWO LISTS**:
  - `visits`: All component visits in DFS order (with duplicates)
  - `unique_components`: Only one entry per component name

---

## The DFS Traversal Algorithm

### Main Entry Point: `TableGenerator.run()`

```python
def run(self) -> None:
    self._process_json(self.root_config, [], None)  # Start DFS
    for ctx in self.subsystem_order:
        self._emit_subsystem_outputs(ctx)            # Generate files for each subsystem
```

### DFS Implementation: `_process_json(json_path, stack, template_type)`

```python
def _process_json(self, json_path: Path, stack: List[SubsystemContext], template_type: Optional[str]) -> None:
    
    # Step 1: Load the JSON file
    data = self.loader.load(json_path)
    component_name = data.get("name")
    controller_value = data.get("controller")
    is_controller = bool(controller_value and component_name)
    
    # Step 2: Check if this is a controller (subsystem boundary)
    if is_controller:
        # Create or retrieve subsystem context
        context = self.subsystems.get(component_name)
        if context is None:
            context = SubsystemContext(
                name=component_name,
                controller=controller_value,
                json_path=json_path,
            )
            self.subsystems[component_name] = context
            self.subsystem_order.append(context)  # Track traversal order
        
        # PUSH subsystem onto stack
        stack.append(context)
        new_context = context
    
    # Step 3: Register component with active subsystem
    active_context = stack[-1] if stack else None
    if active_context is not None and component_name and not is_controller:
        active_context.register_component(json_path, data, self.sources, template_type)
        # This adds ONE entry to:
        #   - active_context.visits (ComponentVisit)
        #   - active_context.unique_components (if first time) (ComponentDefinition)
    
    # Step 4: RECURSE into children in components[] array
    components = data.get("components")
    if isinstance(components, list):
        for entry in components:
            if isinstance(entry, str):
                # Resolve the path and recurse
                child_path, child_template_type = resolve_json_reference(json_path, entry)
                self._process_json(child_path, stack, child_template_type)  # RECURSIVE
            elif isinstance(entry, dict):
                # Inline component definition
                self._process_inline(entry, json_path, stack)
    
    # Step 5: POP if this was a controller
    if new_context is not None:
        stack.pop()
```

---

## What Happens at Each Node

### When visiting a component JSON:

1. **Load JSON**  `data = loader.load(json_path)`

2. **Check if subsystem (controller)**:
   ```python
   is_controller = bool(controller_value and component_name)
   ```
   - If YES: Create `SubsystemContext`, PUSH onto stack
   - If NO: Continue

3. **Register with active subsystem**:
   ```python
   if active_context and component_name and not is_controller:
       active_context.register_component(...)
   ```
   - Adds to `visits` list (ALWAYS - even if duplicate)
   - Adds to `unique_components` dict (ONLY if first time)

4. **Resolve function names**:
   ```python
   init_func, act_func = resolve_function_names(component_name, data)
   hit_count = resolve_hit_count(data)
   ```
   - From `software.init_function` (default: `{name}_init`)
   - From `software.act_function` (default: `{name}_act`)
   - From `timing.hitCount` (default: 1)

5. **Load artifacts**:
   ```python
   src_path = sources.find_source_file(...)
   hdr_path = sources.find_header_file(...)
   definition.load_artifacts()  # Read file contents
   ```

6. **RECURSE into `components[]` array** for each child

7. **POP from stack** if this was a controller

---

## Example: goblin_head DFS Traversal

Starting from: `config/bots/bot_families/goblins/head/goblin_head.json`

```
CALL: _process_json(goblin_head.json, [], None)

  IS CONTROLLER: YES (controller: "ESP32_S3_R8N16", name: "goblin_head")
  ACTION: Create SubsystemContext("goblin_head"), PUSH onto stack
  STACK: [goblin_head_context]
  
  RECURSE: components[0] = "config/bots/bot_families/goblins/head/goblin_left_eye.json"
  CALL: _process_json(goblin_left_eye.json, [goblin_head_context], None)
    
    IS CONTROLLER: NO
    ACTION: Register goblin_left_eye with goblin_head_context
    VISITS: [ComponentVisit(goblin_left_eye, ...)]
    
    RECURSE: components[0] = "config/components/creature_specific/goblin_eye.json"
    CALL: _process_json(goblin_eye.json, [goblin_head_context], None)
      
      IS CONTROLLER: NO
      ACTION: Register goblin_eye with goblin_head_context
      VISITS: [goblin_left_eye, goblin_eye]
      
      RECURSE: components[0] = "config/components/hardware/gc9a01.json"
      CALL: _process_json(gc9a01.json, [goblin_head_context], None)
        
        IS CONTROLLER: NO
        ACTION: Register gc9a01 with goblin_head_context
        VISITS: [goblin_left_eye, goblin_eye, gc9a01]
        
        RECURSE: components[0] = "config/components/drivers/spi_display_bus.json"
        CALL: _process_json(spi_display_bus.json, [goblin_head_context], None)
          
          IS CONTROLLER: NO
          ACTION: Register spi_display_bus with goblin_head_context
          VISITS: [goblin_left_eye, goblin_eye, gc9a01, spi_display_bus]
          
          RECURSE: components[0] = "config/components/drivers/generic_spi_display.json"
          CALL: _process_json(generic_spi_display.json, [goblin_head_context], None)
            
            IS CONTROLLER: NO
            ACTION: Register generic_spi_display with goblin_head_context
            VISITS: [..., generic_spi_display]
            
            RECURSE: components[0] = "config/components/interfaces/spi_bus.json"
            CALL: _process_json(spi_bus.json, [goblin_head_context], None)
              
              IS CONTROLLER: NO
              ACTION: Register spi_bus with goblin_head_context
              VISITS: [..., spi_bus]
              
              RECURSE: components[] = EMPTY - RETURN
            RETURN FROM spi_bus
          RETURN FROM generic_spi_display
        RETURN FROM spi_display_bus
      RETURN FROM gc9a01
    RETURN FROM goblin_eye
  RETURN FROM goblin_left_eye
  
  RECURSE: components[1] = "config/bots/bot_families/goblins/head/goblin_right_eye.json"
  CALL: _process_json(goblin_right_eye.json, [goblin_head_context], None)
    
    IS CONTROLLER: NO
    ACTION: Register goblin_right_eye with goblin_head_context
    VISITS: [..., goblin_right_eye]
    
    RECURSE: components[0] = "config/components/creature_specific/goblin_eye.json"
    [ALREADY VISITED - but we register AGAIN]
    CALL: _process_json(goblin_eye.json, [goblin_head_context], None)
      
      IS CONTROLLER: NO
      ACTION: Register goblin_eye AGAIN
      VISITS: [..., goblin_eye]  DUPLICATE ENTRY
      NOTE: unique_components dict SKIPS (already have goblin_eye entry)
      
      [Continue DFS as before...]
```

---

## Key Insights

### 1. VISITS vs UNIQUE_COMPONENTS

| List | Contains | Purpose |
|------|----------|---------|
| `visits` | All DFS visits including duplicates | Populates dispatch tables (allows duplicate entries) |
| `unique_components` | One entry per component name | Source code aggregation (prevents duplication) |

### 2. Dispatch Table Generation

```python
def render_dispatch_source(context: SubsystemContext) -> str:
    init_entries = []
    for visit in context.visits:  #  Uses VISITS (with duplicates)
        init_entries.append(f"&{visit.init_func}")
    
    # Result: If goblin_eye appears twice in visits, init_table has two entries
```

### 3. Component Source Aggregation

```python
def render_component_source(context: SubsystemContext) -> str:
    included_srcs: Set[Path] = set()
    for definition in sorted(context.unique_components.values()):  #  Uses UNIQUE
        if definition.src_path in included_srcs:
            continue  #  Skip duplicates
        included_srcs.add(definition.src_path)
        lines.append(definition.src_content)
```

Result: Each `.src` file included **exactly once**, even if component referenced multiple times.

### 4. Component Registration Logic

```python
def register_component(self, json_path, data, sources, template_type):
    # ALWAYS add to visits
    self.visits.append(ComponentVisit(...))
    
    # ONLY add to unique_components if NOT already there
    if component_name in self.unique_components:
        # Already registered - just update missing artifacts
        return
    self.unique_components[component_name] = ComponentDefinition(...)
```

---

## Generated Dispatch Tables for goblin_head

### After DFS traversal completes:

**`goblin_head_init_table[]`** (from visits):
```cpp
const init_function_t goblin_head_init_table[] = {
    &goblin_left_eye_init,    // [0]
    &goblin_eye_init,         // [1]
    &gc9a01_init,             // [2]
    &spi_display_bus_init,    // [3]
    &generic_spi_display_init,// [4]
    &spi_bus_init,            // [5]
    &goblin_right_eye_init,   // [6]
    &goblin_eye_init,         // [7]  DUPLICATE (second visit of goblin_eye)
    // ... more entries for other components
};
```

**`goblin_head_hitcount_table[]`**:
```cpp
const uint32_t goblin_head_hitcount_table[] = {
    200,  // goblin_left_eye
    1,    // goblin_eye
    1,    // gc9a01
    1,    // spi_display_bus
    // ...
};
```

**Control flow in `app_main()`**:
```cpp
extern "C" void app_main(void) {
    // PHASE 1: Initialize all components
    for (size_t i = 0; i < goblin_head_init_table_size; ++i) {
        if (goblin_head_init_table[i]) {
            goblin_head_init_table[i]();  // Calls each init function
        }
    }
    
    // PHASE 2: Main loop - run act functions based on hitCount
    while (true) {
        for (size_t i = 0; i < goblin_head_act_table_size; ++i) {
            const auto func = goblin_head_act_table[i];
            const auto hit = goblin_head_hitcount_table[i];
            if (func && hit > 0U && (g_loopCount % hit) == 0U) {
                func();  // Execute if (loopCount % hitCount == 0)
            }
        }
        ++g_loopCount;
    }
}
```

---

## Critical Issues Detected

### 1. **Circular Component Reference**: goblin_mouth
```json
{
    "name": "goblin_mouth",
    "components": [
        "config/components/creature_specific/goblin_mouth.json"   SELF!
    ]
}
```
**Problem**: Self-reference creates infinite recursion risk
**Expected**: Should reference `gc9a01.json` (the display hardware)

### 2. **Duplicate Component Entries in Dispatch Tables**
- `goblin_eye` appears twice (both eyes reference it)
- `servo_sg90_micro` appears twice (both ears reference it)
- `hw496_microphone` appears twice (both ears reference it)

This is **intentional** for frequency control via hitCount, but could cause multiple initializations.

### 3. **Cross-Subsystem Reference**
```json
{
    "components": [
        "...",
        "config/subsystems/goblin_torso.json"
    ]
}
```
**Problem**: goblin_head refers to goblin_torso (different subsystem)
**Result**: goblin_torso components added to goblin_head dispatch tables

---

This is the **complete mechanism** by which JSON drives C++ code generation.
