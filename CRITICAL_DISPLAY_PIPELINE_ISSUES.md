# CRITICAL: Display Pipeline Architecture Violations

**Status**: 🚨 **BUILD-BREAKING VIOLATIONS DETECTED**  
**Date**: November 14, 2025  
**Scope**: `generic_spi_display.src` and related display components

---

## Executive Summary

The `generic_spi_display` component contains **multiple critical architectural violations** that contradict the fundamental rules established in `.github/`:

1. **Incorrect use of file-scoped static variables** as if they were `use_fields`
2. **Misunderstanding of the single compilation unit model**
3. **Violation of the intra-subsystem communication pattern**
4. **Invalid assumptions about variable scope and initialization**

These issues will cause **undefined behavior**, **linker errors**, and **incorrect data flow** when multiple display components are used in the same subsystem.

---

## Issue 1: Misuse of Hardcoded Static Variables

### The Violation

In `generate_tables.py` line 709-720, the generator creates these hardcoded static variables at the top of every generated subsystem `.cpp` file:

```python
lines.extend([
    "// Subsystem-scoped static variables (shared across all components in this file)",
    "static int display_width = 240;",
    "static int display_height = 240;",
    "static int bytes_per_pixel = 2;  // RGB565",
    "static uint8_t* front_buffer = NULL;",
    "static uint8_t* back_buffer = NULL;",
    "static int display_size = 0;",
    "static int current_row_count = 10;",
    "static int max_display_height = INT_MAX;  // Min height across all displays",
    "static char* color_schema = nullptr;",
    "",
])
```

### The Problem

These variables are **HARDCODED with single-display values** (240x240), but `generic_spi_display.src` line 189-207 **READS** them as if they were **per-component configuration**:

```cpp
esp_err_t generic_spi_display_init(void) {
    // Line 189: Assumes front_buffer is THIS component's buffer
    if (next_slot_index < MAX_DISPLAY_SLOTS && front_buffer != NULL)
    {
        display_buffers[next_slot_index].front_buffer = front_buffer;  // ❌ WRONG
        display_buffers[next_slot_index].back_buffer = back_buffer;    // ❌ WRONG
        display_buffers[next_slot_index].buffer_size = display_width * display_height * bytes_per_pixel; // ❌ WRONG
        display_buffers[next_slot_index].width = display_width;        // ❌ WRONG
        display_buffers[next_slot_index].height = display_height;      // ❌ WRONG
```

### Why This Fails

**Scenario**: Subsystem has 3 displays (left eye 240x240, right eye 240x240, mouth 128x64)

1. Generator creates **ONE set** of static variables: `display_width=240`, `display_height=240`, `front_buffer=NULL`
2. **All three components** see the **SAME variables**
3. When `goblin_left_eye_init()` runs, it might allocate buffer and **does NOT update the static variables**
4. When `generic_spi_display_init()` runs, it reads `front_buffer` (still NULL) and `display_width` (240), which are **NOT this component's values**
5. **Data corruption ensues** - wrong buffers registered, wrong dimensions assumed

### Architectural Mismatch

Per `.github/copilot-instructions.md`:

> **Intra-Subsystem Communication**: Because of the single compilation unit, components within the same subsystem **must** communicate using file-scoped `static` variables (e.g., `static uint16_t* display_buffer = NULL;`).

✅ **Correct usage**: One component allocates and writes `display_buffer`, another reads and displays it  
❌ **Incorrect usage**: Multiple components trying to use the same variable name for **different per-instance data**

---

## Issue 2: Missing `use_fields` Implementation

### What Should Happen

Per `generate_tables.py` lines 406-436, components should use `use_fields` in their JSON to receive per-component configuration:

```python
def collect_use_fields(component_name: str, data: Dict[str, Any], context: SubsystemContext) -> None:
    """Collect use_fields from a component into the subsystem's use_field_vars tracking.
    
    This builds up the complete picture of which components use which variables.
    """
    use_fields = data.get("use_fields")
    if not isinstance(use_fields, dict):
        return
```

**Example of correct use_fields pattern**:

```json
{
  "name": "goblin_left_eye",
  "type": "CREATURE_PART",
  "use_fields": {
    "display_width": 240,
    "display_height": 240,
    "bytes_per_pixel": 2
  },
  "components": [
    "generic_spi_display"
  ]
}
```

Then `generate_tables.py` would:
1. Generate static variable declarations initialized from parent values
2. **NOT inject assignments** into component functions (per line 782-784 comment)
3. Each component instance would see values **initialized at declaration time**

### Current State

`generic_spi_display` **does not use `use_fields`** - it assumes the hardcoded static variables magically contain the right values for each instance. This violates the architecture.

---

## Issue 3: Invalid Buffer Pointer Management

### The Code Pattern

```cpp
// Line 189: generic_spi_display_init()
if (next_slot_index < MAX_DISPLAY_SLOTS && front_buffer != NULL)
{
    display_buffers[next_slot_index].front_buffer = front_buffer;
```

### The Architectural Question

**Where does `front_buffer` get its value?**

Looking at the hardcoded declaration (generator line 713):
```python
"static uint8_t* front_buffer = NULL;",
```

It's initialized to `NULL`. For `generic_spi_display_init()` to work, some **other component** must have:
1. Allocated the buffer
2. Written to the `front_buffer` static variable
3. **Before** `generic_spi_display_init()` runs

### The Init Order Problem

Per `.github/06_TIMING_AND_EXECUTION.md`, component init order is based on **JSON traversal order**, not explicit dependencies. There's **NO GUARANTEE** that buffer-allocating component runs before display component.

**Result**: Race condition - `generic_spi_display_init()` may run before buffer exists.

---

## Issue 4: Shared State Across Multiple Instances

### The "Multiple Display" Scenario

From `generic_spi_display.src` lines 50-57:

```cpp
#define MAX_DISPLAY_SLOTS 3  // Left eye, right eye, mouth
static display_buffer_slot_t display_buffers[MAX_DISPLAY_SLOTS] = {0};
static int next_slot_index = 0;
static int current_send_slot = 0;
```

This code **assumes** that:
1. `generic_spi_display` component appears **multiple times** in the dispatch table (once per display)
2. Each call to `generic_spi_display_init()` increments `next_slot_index`
3. All instances share the `display_buffers[]` array

### The Architectural Violation

Per `.github/03_COMPONENT_SYSTEM_RULES.md`:

> ### Rule 1: NO #include of .hdr in .src
> 
> **Why**: `generate_tables.py` aggregates ALL `.hdr` content into the subsystem `.hpp` file, which is already `#include`'d at the top of the generated `.cpp`. Including it again causes multiple definition errors.

This rule exists because **the generator deduplicates components**. Looking at `generate_tables.py` lines 180-205:

```python
# If we've already aggregated this component, only attach missing artifacts.
existing = self.unique_components.get(component_name)
if existing is not None:
    if existing.src_path is None and src_path is not None:
        existing.src_path = src_path
        existing.src_content = read_ascii_file(src_path)
    if existing.hdr_path is None and hdr_path is not None:
        existing.hdr_path = hdr_path
        existing.hdr_content = read_ascii_file(hdr_path)
    return  # ❌ EXITS EARLY - DOES NOT ADD TO .cpp AGAIN
```

**CRITICAL FINDING**: If `goblin_left_eye.json` includes `generic_spi_display` and `goblin_right_eye.json` ALSO includes `generic_spi_display`, the generator will:

1. Add `generic_spi_display` source to `.cpp` file **ONCE**
2. Add entries to dispatch table for **BOTH** eyes
3. When `goblin_left_eye_init()` runs, it calls `generic_spi_display_init()` → sets `next_slot_index = 1`
4. When `goblin_right_eye_init()` runs, it calls `generic_spi_display_init()` → sets `next_slot_index = 2`

**But wait** - there's only **ONE copy** of `generic_spi_display_init()` in the `.cpp` file!

### The Dispatch Table Reality

Looking at `generate_tables.py` lines 812-827:

```python
for visit in context.visits:
    if visit.template_type:
        init_entries.append(f"&{visit.init_func.replace('<T>', f'<{visit.template_type}>')}")
        act_entries.append(f"&{visit.act_func.replace('<T>', f'<{visit.template_type}>')}")
    else:
        init_entries.append(f"&{visit.init_func}")
        act_entries.append(f"&{visit.act_func}")
```

The dispatch table contains **function pointers**. If the same function is added twice, both entries point to **the same function**.

**Result**: `generic_spi_display_init()` is called **multiple times** during init phase, but all calls execute the **same code** with the **same static variables**.

---

## Issue 5: Interface File Confusion

### The Situation

- `config/components/interfaces/spi_display_bus.hdr` defines `extern spi_display_pinset_t cur_spi_display_pin;`
- `config/components/interfaces/spi_display_bus.src` defines the actual variable
- `generate_tables.py` line 598 specifically includes interface headers:

```python
def collect_interface_includes(context: SubsystemContext) -> Set[str]:
    """Determine which interface headers need to be included based on component usage."""
    includes: Set[str] = set()

    # Check for SPI display bus usage
    spi_display_components = {
        "gc9a01", "generic_spi_display", "spi_display_bus",
        "goblin_left_eye", "goblin_right_eye", "goblin_eye"
    }

    for definition in context.unique_components.values():
        if definition.name in spi_display_components:
            includes.add("components/interfaces/spi_display_bus.hdr")
            break
```

### The Architectural Intent

**Interface pattern**: Bus component allocates slots and rotates `cur_spi_display_pin` in its `act()` function. Consumer components read `cur_spi_display_pin` to get their pin assignments.

**This is correct intra-subsystem communication via static variables!**

### The Problem

`generic_spi_display` **doesn't use** `cur_spi_display_pin`. Instead, it:
1. Tries to read `front_buffer` / `display_width` / etc. (which are NOT interface variables)
2. Maintains its own `display_buffers[]` slot array
3. Implements its own DMA state machine with `spi_state`

**Question**: Should display buffer management be in the **interface** (shared across all displays) or **per-component** (each display manages its own)?

---

## Recommended Architecture

### Option A: Display Buffer as Interface

**Concept**: Display buffer allocation happens in a dedicated component (e.g., `display_buffer_manager`), exposes pointers via static variables.

```cpp
// display_buffer_manager.src (runs FIRST via low hitCount)
static uint8_t* eye_left_buffer = NULL;
static uint8_t* eye_right_buffer = NULL;
static uint8_t* mouth_buffer = NULL;

esp_err_t display_buffer_manager_init(void) {
    eye_left_buffer = (uint8_t*)malloc(240 * 240 * 2);
    eye_right_buffer = (uint8_t*)malloc(240 * 240 * 2);
    mouth_buffer = (uint8_t*)malloc(128 * 64 * 2);
    return ESP_OK;
}
```

```cpp
// goblin_left_eye.src
void goblin_left_eye_act(void) {
    render_eye_to_buffer(eye_left_buffer, 240, 240);
}
```

```cpp
// generic_spi_display.src
void generic_spi_display_act(void) {
    // Read cur_spi_display_pin to determine which display
    // Read appropriate buffer based on pin assignment
    send_to_display(current_buffer, width, height);
}
```

**Pros**:
- Clear separation of concerns
- Explicit buffer ownership
- Follows intra-subsystem communication rules

**Cons**:
- Requires defining all buffers upfront
- Less flexible for dynamic configurations

---

### Option B: use_fields for Per-Component Config

**Concept**: Each display component declares its requirements via `use_fields`, generator handles initialization.

```json
// goblin_left_eye.json
{
  "name": "goblin_left_eye",
  "use_fields": {
    "my_display_width": 240,
    "my_display_height": 240,
    "my_bytes_per_pixel": 2
  },
  "components": ["generic_spi_display"]
}
```

Generator would create:
```cpp
static int my_display_width = 240;
static int my_display_height = 240;
static int my_bytes_per_pixel = 2;
```

**But this STILL doesn't solve the buffer pointer problem** - who allocates?

---

### Option C: Renderer + Driver Split (Recommended)

**Concept**: Separate rendering (pixel generation) from display (DMA transfer).

**Pipeline**:
1. **Buffer Manager Component**: Allocates all buffers at init
2. **Renderer Components** (goblin_left_eye, goblin_right_eye): Write pixels to assigned buffers
3. **Display Bus Component** (spi_display_bus): Rotates through pin assignments
4. **Display Driver Component** (generic_spi_display): Reads current buffer/pins, executes DMA

**Implementation**:
```cpp
// display_buffer_interface.hdr (NEW INTERFACE)
struct display_buffer_descriptor_t {
    uint8_t* buffer;
    uint16_t width;
    uint16_t height;
    uint8_t bpp;
    bool ready;
};

extern display_buffer_descriptor_t display_buffers[3];
extern int active_display_index;  // Rotated by spi_display_bus
```

```cpp
// goblin_left_eye.src
void goblin_left_eye_act(void) {
    display_buffer_descriptor_t* desc = &display_buffers[0];  // Left eye = slot 0
    if (desc->buffer != NULL) {
        render_eye_animation(desc->buffer, desc->width, desc->height);
        desc->ready = true;
    }
}
```

```cpp
// generic_spi_display.src
void generic_spi_display_act(void) {
    display_buffer_descriptor_t* desc = &display_buffers[active_display_index];
    if (desc->ready && desc->buffer != NULL) {
        dma_transfer_to_display(desc->buffer, desc->width * desc->height * desc->bpp);
        desc->ready = false;
    }
}
```

**Pros**:
- Clean separation: renderers don't know about DMA, driver doesn't know about rendering
- Interface pattern properly used
- Scales to any number of displays
- Follows `.github/05_COMMUNICATION_PATTERNS.md` exactly

**Cons**:
- Requires refactoring existing code
- More components to manage

---

## Required Changes Summary

### Immediate Fixes (Prevent Build Failures)

1. **Remove hardcoded static variables** from `generate_tables.py` line 709-720
2. **Implement proper `use_fields` support** for display dimensions
3. **Create display buffer interface** header with proper `extern` declarations
4. **Refactor `generic_spi_display.src`** to use interface variables instead of hardcoded names

### Architectural Decisions Needed

1. **Buffer allocation strategy**: Who allocates display buffers?
   - Option: Dedicated buffer manager component
   - Option: Each renderer allocates its own
   - Option: Display driver allocates based on config

2. **Display identification**: How does driver know which display it's serving?
   - Current approach: Slot index (fragile)
   - Better: Read from `spi_display_bus` rotation
   - Best: Explicit assignment via use_fields

3. **DMA state management**: Should it be per-display or shared?
   - Current: Shared (incorrect for multiple displays)
   - Proposed: Per-slot in interface struct

---

## Questions for Discussion

1. **Do we keep the "multiple calls to same init function" pattern**, or should each display have a **separate component**?
   - Current: `generic_spi_display` reused 3 times
   - Alternative: `left_eye_display`, `right_eye_display`, `mouth_display` as separate components

2. **How should `use_fields` interact with buffer allocation**?
   - Should `use_fields` contain buffer SIZE, and driver allocates?
   - Or should `use_fields` contain buffer POINTER (allocated elsewhere)?

3. **Is the debug mode (network streaming) orthogonal to production mode**, or should they be **separate components**?
   - Current: One component with `if (debug)` branches
   - Cleaner: `generic_spi_display` (production) vs `network_display_stream` (debug)

4. **Should the generator support per-instance static variables**, or is the current "one static variable set per subsystem" the correct model?

---

## Next Steps

**DO NOT make changes until we agree on:**

1. ✅ Architectural pattern: Interface-based communication
2. ✅ Buffer ownership model
3. ✅ Display identification mechanism
4. ✅ Generator modifications needed

**After decisions**:

1. Update `.github/` documentation to clarify display pipeline rules
2. Create new interface definitions
3. Refactor components in sequence
4. Regenerate tables
5. Test with multi-display subsystem

---

## References

- `.github/copilot-instructions.md` - Single compilation unit model
- `.github/03_COMPONENT_SYSTEM_RULES.md` - Static variable communication rules
- `.github/05_COMMUNICATION_PATTERNS.md` - Intra-subsystem patterns
- `tools/generate_tables.py` - Component aggregation logic
- `config/components/drivers/generic_spi_display.src` - Current implementation
- `config/components/interfaces/spi_display_bus.hdr` - Interface pattern example
