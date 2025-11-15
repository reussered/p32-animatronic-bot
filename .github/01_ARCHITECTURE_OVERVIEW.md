# 01. Architecture Overview

**Read Time**: 5 minutes | **Criticality**: ⚠️ BUILD-BREAKING if violated

---

## Core Concept: Single Compilation Unit

⚠️ **CRITICAL**: All `.src` files for a subsystem compile into ONE `.cpp` file.

```
config/components/eye_driver.src  ──┐
config/components/display_mgr.src ──┼──> src/goblin_head_component_functions.cpp
config/components/mood_tracker.src ─┘
```

**Implications**:
- All `.src` files share **same file scope**
- Components communicate via `static` variables (no `extern` needed)
- `.hdr` content aggregated into ONE `.hpp` (included once at top of `.cpp`)
- `generate_tables.py` auto-strips any `#include "*.hdr"` from `.src` files

---

## Component Isolation

❌ **NEVER**: Call component functions directly
✅ **ALWAYS**: Components only called by main loop

```cpp
// ❌ WRONG - Direct component call
void component_a_act(void) {
    component_b_act();  // NEVER DO THIS
}

// ✅ CORRECT - Static variable communication (same subsystem)
static uint16_t shared_display_buffer[240*240];

void component_a_act(void) {
    shared_display_buffer[0] = 0xFF;  // Write
}

void component_b_act(void) {
    uint16_t pixel = shared_display_buffer[0];  // Read
}
```

---

## Multi-ESP32 Distributed System

Each subsystem runs on separate ESP32 chip:

```
HEAD (ESP32-S3)          TORSO (ESP32-S3)         ARM_LEFT (ESP32-C3)
├─ eyes                  ├─ system_core           ├─ shoulder
├─ mouth                 ├─ shared_memory         ├─ elbow
└─ ears                  └─ personality           └─ gripper
```

**Inter-Subsystem Communication**: Global Shared Memory (GSM)

---

## Communication Rules

| Scenario | Method | Example |
|----------|--------|---------|
| Same subsystem (same chip) | `static` variables | `static uint16_t* buffer;` |
| Different subsystems (different chips) | GSM | `GSM.read<Mood>()` / `GSM.write<Mood>()` |
| Component → Main Loop | Return via `esp_err_t` (init) or globals | `return ESP_OK;` |

---

## Component Pattern

Every component follows this exact pattern:

```cpp
// In .hdr file - declarations only
esp_err_t component_name_init(void);
void component_name_act(void);

// In .src file - implementation
esp_err_t component_name_init(void) {
    // Setup hardware, assign pins, allocate memory
    return ESP_OK;  // or ESP_FAIL
}

void component_name_act(void) {
    // Execute component logic (called every N loops)
    // Access g_loopCount for timing
    // Use static variables or GSM for communication
}
```

---

## The Core Loop (Immutable)

⚠️ **NEVER MODIFY**: This loop is auto-generated and must not be touched.

```cpp
void app_main(void) {
    // Init all components once
    for (int i = 0; i < INIT_TABLE_SIZE; i++) {
        initTable[i]();
    }
    
    // Main loop - runs forever at max speed
    while (true) {
        for (int i = 0; i < ACT_TABLE_SIZE; i++) {
            if (g_loopCount % actTable[i].hitCount == 0) {
                actTable[i].act_func();
            }
        }
        g_loopCount++;
    }
}
```

**Execution**: Component runs when `g_loopCount % hitCount == 0`

---

## JSON-Driven Composition

Everything defined in JSON:

```json
{
  "bot_name": "goblin_full",
  "subsystems": {
    "head": {
      "chip": "ESP32_S3",
      "components": ["goblin_left_eye", "goblin_right_eye", "goblin_mouth"]
    }
  }
}
```

`generate_tables.py` reads JSON → generates dispatch tables → builds firmware

---

## Key Architecture Rules

| Rule | Why | Consequence if Violated |
|------|-----|-------------------------|
| No direct component calls | Components isolated | Runtime crashes, stack overflow |
| No `#include` of `.hdr` in `.src` | Single compilation unit | Multiple definition errors |
| No `extern` in `.src` files | File scope sharing | Linker errors, duplicate symbols |
| Static variables for same-subsystem | Shared compilation unit | Works correctly |
| GSM for cross-subsystem | ESP-NOW transport | ESP-NOW handles sync |
| Never edit generated files | Source truth in config/ | Changes lost on regeneration |

---

## Quick Decision Tree

**Need to share data between components?**
```
Are they in the same subsystem (same chip)?
├─ YES → Use static variable at file scope
└─ NO  → Use GSM.read<T>() / GSM.write<T>()

Are you modifying code?
├─ In config/**/*.src → ✅ CORRECT
├─ In config/**/*.hdr → ✅ CORRECT
├─ In src/components/*.cpp → ❌ NEVER (auto-generated)
└─ In include/components/*.hpp → ❌ NEVER (auto-generated)
```

---

**Next**: [02. Build Workflow](02_BUILD_WORKFLOW.md) - How to regenerate tables and build firmware
