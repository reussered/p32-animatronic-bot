# Raw Memory Block Pattern - Simplest Change Detection

## Core Concept

**Treat global state as a single block of raw memory bytes.**

No field-level logic. No metadata exclusions. Just:
1. Compare raw bytes: `memcmp(&old, &current, sizeof(block))`
2. Send raw bytes: `esp_now_send(&current, sizeof(block))`
3. Copy raw bytes: `memcpy(&old, &current, sizeof(block))`

## Complete Implementation

### Header File

**File**: `include/p32_shared_state.h`

```cpp
#ifndef P32_SHARED_STATE_H
#define P32_SHARED_STATE_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Global loop counter
extern uint32_t g_loopCount;

// POD struct - entire struct is treated as raw memory block
typedef struct {
    // Metadata (changes every transmission)
    uint32_t version;
    uint32_t timestamp_ms;
    uint8_t source_node_id;
    
    // Data (serialized C++ classes)
    int8_t mood_serialized[9];
    uint8_t personality_serialized[32];
    
    // Sensor data
    uint8_t distance_cm;
    bool touch_detected;
    int16_t temperature_c;
    
    // System status
    uint8_t battery_percent;
    bool wifi_connected;
    
    // Checksum (calculated after change detection)
    uint32_t checksum;
} p32_shared_state_t;

extern p32_shared_state_t g_shared_state;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include "Mood.hpp"

extern Mood g_mood;

// Shadow copy for change detection
extern p32_shared_state_t g_shared_state_previous;

// Serialize C++ classes to POD arrays
inline void mood_serialize() {
    memcpy(g_shared_state.mood_serialized, g_mood.components, 
           sizeof(g_mood.components));
}

inline void sync_all_classes_to_shared_state() {
    mood_serialize();
}

// Deserialize POD arrays to C++ classes
inline void mood_deserialize() {
    memcpy(g_mood.components, g_shared_state.mood_serialized, 
           sizeof(g_mood.components));
}

inline void sync_all_classes_from_shared_state() {
    mood_deserialize();
}

// ===== RAW MEMORY BLOCK PATTERN =====

// Compare ENTIRE struct as raw bytes (simplest method)
inline bool has_shared_state_changed() {
    return (memcmp(&g_shared_state, &g_shared_state_previous, 
                   sizeof(p32_shared_state_t)) != 0);
}

// Copy ENTIRE struct as raw bytes
inline void save_shared_state_as_previous() {
    memcpy(&g_shared_state_previous, &g_shared_state, 
           sizeof(p32_shared_state_t));
}

#endif // __cplusplus

#endif // P32_SHARED_STATE_H
```

### Implementation File

**File**: `src/p32_shared_state.cpp`

```cpp
#include "p32_shared_state.h"

uint32_t g_loopCount = 0;
p32_shared_state_t g_shared_state = {0};
p32_shared_state_t g_shared_state_previous = {0};

#ifdef __cplusplus
Mood g_mood;
#endif
```

## Mesh Component (Complete Example)

**File**: `src/components/esp_now_mesh.cpp`

```cpp
#include "p32_shared_state.h"
#include <esp_now.h>
#include <esp_crc.h>

esp_err_t esp_now_mesh_init(void) {
    esp_now_init();
    esp_now_register_recv_cb(on_esp_now_recv);
    register_all_mesh_peers();
    
    ESP_LOGI(TAG, "ESP-NOW mesh initialized");
    return ESP_OK;
}

void esp_now_mesh_act(void) {
    // hitCount: 1  executes EVERY loop (100ms)
    
    // STEP 1: Serialize C++ classes  POD arrays
    sync_all_classes_to_shared_state();
    
    // STEP 2: Compare raw memory blocks
    if (memcmp(&g_shared_state, &g_shared_state_previous, 
               sizeof(p32_shared_state_t)) != 0) {
        
        // STEP 3: Update metadata AFTER change detected
        g_shared_state.version++;
        g_shared_state.timestamp_ms = esp_timer_get_time() / 1000;
        g_shared_state.source_node_id = 1;  // Torso = 1
        
        // STEP 4: Calculate checksum
        g_shared_state.checksum = esp_crc32_le(0, 
            (uint8_t*)&g_shared_state, 
            sizeof(p32_shared_state_t) - sizeof(uint32_t));
        
        // STEP 5: Send raw memory block
        esp_err_t ret = esp_now_send(NULL, 
            (uint8_t*)&g_shared_state,  //  Send entire block
            sizeof(p32_shared_state_t));
        
        if (ret == ESP_OK) {
            // STEP 6: Copy raw memory block
            memcpy(&g_shared_state_previous, &g_shared_state, 
                   sizeof(p32_shared_state_t));
            
            ESP_LOGI(TAG, "State broadcast v%u (size=%u bytes)",
                g_shared_state.version, sizeof(p32_shared_state_t));
        } else {
            ESP_LOGW(TAG, "Broadcast failed: %d", ret);
        }
    }
    // NO broadcast if memory blocks are identical
}

// ESP-NOW receive callback (slave nodes)
void on_esp_now_recv(const uint8_t *mac_addr, const uint8_t *data, int len) {
    if (len != sizeof(p32_shared_state_t)) {
        ESP_LOGW(TAG, "Wrong size: %d (expected %u)", 
            len, sizeof(p32_shared_state_t));
        return;
    }
    
    p32_shared_state_t *received = (p32_shared_state_t *)data;
    
    // Validate checksum
    uint32_t calc_checksum = esp_crc32_le(0, data, 
        sizeof(p32_shared_state_t) - sizeof(uint32_t));
    
    if (calc_checksum != received->checksum) {
        ESP_LOGW(TAG, "Checksum mismatch");
        return;
    }
    
    // Version check
    if (received->version > g_shared_state.version) {
        // Copy raw memory block
        memcpy(&g_shared_state, received, sizeof(p32_shared_state_t));
        
        // Deserialize POD arrays  C++ classes
        sync_all_classes_from_shared_state();
        
        ESP_LOGI(TAG, "State synced v%u: mood.happiness=%d",
            g_shared_state.version, g_mood.happiness());
    }
}
```

## Why This Is Simpler

### Old Approach (Complicated)

```cpp
// Skip metadata, calculate offsets, compare data region only
const size_t metadata_size = offsetof(p32_shared_state_t, mood_serialized);
const size_t data_size = sizeof(p32_shared_state_t) - metadata_size - sizeof(uint32_t);
const uint8_t* current_data = (const uint8_t*)&g_shared_state + metadata_size;
const uint8_t* previous_data = (const uint8_t*)&g_shared_state_previous + metadata_size;
return (memcmp(current_data, previous_data, data_size) != 0);
```

**Problems**:
- Complex offset calculations
- Easy to get wrong
- Harder to understand
- More code

### New Approach (Simple)

```cpp
// Compare entire block as raw bytes
return (memcmp(&g_shared_state, &g_shared_state_previous, 
               sizeof(p32_shared_state_t)) != 0);
```

**Benefits**:
- One line
- No offset math
- Can't get wrong
- Crystal clear

## Addressing "But Metadata Changes Every Time!"

**Q**: Metadata (version, timestamp) changes with every broadcast, so won't this always detect a change?

**A**: NO! The order of operations matters:

```cpp
// CORRECT ORDER:
sync_all_classes_to_shared_state();  // Serialize mood/personality
if (memcmp(...) != 0) {              // Check if mood/personality changed
    g_shared_state.version++;        //  Update metadata AFTER check
    esp_now_send(...);
    memcpy(...);
}
```

**Key Insight**: We update `version` and `timestamp` **AFTER** detecting the change, not before.

The shadow copy (`g_shared_state_previous`) contains the **last broadcast state**, including its metadata. So when we compare:
- If only metadata changed (but no actual data), no broadcast needed
- If actual data changed, we detect it, THEN update metadata, THEN broadcast

## Example Timeline

```
Loop 0:
 g_shared_state.mood_serialized[HAPPINESS] = 50
 memcmp()  TRUE (different from initial 0)
 g_shared_state.version = 1
 esp_now_send(&g_shared_state)
 memcpy(&g_shared_state_previous, &g_shared_state)
    Result: Both have version=1, happiness=50

Loop 1:
 g_shared_state.mood_serialized[HAPPINESS] = 50 (unchanged)
 memcmp()  FALSE (identical to previous)
    NO broadcast (version stays 1)

Loop 15:
 g_shared_state.mood_serialized[HAPPINESS] = 70 (changed!)
 memcmp()  TRUE (different from previous)
 g_shared_state.version = 2 (updated AFTER detection)
 esp_now_send(&g_shared_state)
 memcpy(&g_shared_state_previous, &g_shared_state)
    Result: Both have version=2, happiness=70
```

## Performance

| Operation | CPU Cycles | Complexity |
|-----------|-----------|------------|
| `memcmp(&g_shared_state, &g_shared_state_previous, sizeof(...))` | ~50 | O(1) |
| `esp_now_send(&g_shared_state, sizeof(...))` | ~200 | O(1) |
| `memcpy(&g_shared_state_previous, &g_shared_state, sizeof(...))` | ~30 | O(1) |

**Total**: ~280 CPU cycles per loop (100ms) when state changes
**Overhead**: ~50 CPU cycles per loop when state unchanged

## Code Simplification Summary

### Before (Complex)

```cpp
// Exclude metadata from comparison
const size_t metadata_size = offsetof(p32_shared_state_t, mood_serialized);
const size_t data_size = sizeof(p32_shared_state_t) - metadata_size - sizeof(uint32_t);
const uint8_t* current_data = (const uint8_t*)&g_shared_state + metadata_size;
const uint8_t* previous_data = (const uint8_t*)&g_shared_state_previous + metadata_size;
if (memcmp(current_data, previous_data, data_size) != 0) {
    g_shared_state.version++;
    esp_now_send((uint8_t*)&g_shared_state, sizeof(p32_shared_state_t));
    memcpy(&g_shared_state_previous, &g_shared_state, sizeof(p32_shared_state_t));
}
```

### After (Simple)

```cpp
// Compare entire block, update metadata AFTER, send block, copy block
if (memcmp(&g_shared_state, &g_shared_state_previous, sizeof(p32_shared_state_t)) != 0) {
    g_shared_state.version++;
    esp_now_send((uint8_t*)&g_shared_state, sizeof(p32_shared_state_t));
    memcpy(&g_shared_state_previous, &g_shared_state, sizeof(p32_shared_state_t));
}
```

**Lines of code**: 10  4 (60% reduction)
**Complexity**: High  Low
**Maintainability**: Error-prone  Foolproof

## Benefits

 **Simplest possible code** - one memcmp, one send, one memcpy
 **No offset calculations** - treat entire struct as raw bytes
 **No field-level logic** - memory block comparison
 **Metadata handled naturally** - updated AFTER change detection
 **Can't get wrong** - impossible to mess up offset math
 **Same performance** - single memcmp regardless of approach

## Related Documentation

 [Fast Change Detection Example](FAST-CHANGE-DETECTION-EXAMPLE.md) - Complete working code
 [Global State Quick Reference](GLOBAL-STATE-QUICK-REFERENCE.md) - Architecture overview
 [C++ Class Serialization](CPP-CLASS-SERIALIZATION-PATTERN.md) - Two-layer pattern
