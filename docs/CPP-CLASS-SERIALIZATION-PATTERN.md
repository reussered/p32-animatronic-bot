# C++ Class Serialization for ESP-NOW Mesh Synchronization

## Design Challenge

**Problem**: Components use C++ class instances (Mood, Personality, etc.) as global state, but ESP-NOW mesh requires Plain Old Data (POD) structures for wireless transmission.

**Solution**: Maintain C++ class instances as "working" globals, serialize to POD struct for mesh transmission, deserialize on receiving nodes back to C++ classes.

**Optimization**: Use `memcmp` on entire POD struct buffer to detect changes (much faster than per-class operator!= checking), and `memcpy` for fast serialization/deserialization.

## Architecture Pattern

### Two-Layer Global State

```

  LAYER 1: C++ Class Instances (Working State)               
     
  extern Mood g_mood;                                         
  extern Personality g_personality;                           
  extern BehaviorState g_behavior;                            
                                                              
  Components work with these objects directly using OOP       
  methods like g_mood.happiness() = 80, g_mood.addAnger(10)  

                           
                  serialize()    deserialize()
                           

  LAYER 2: POD Struct (Mesh Transmission State)              
     
  p32_shared_state_t g_shared_state {                        
      int8_t mood_serialized[9];                             
      uint8_t personality_serialized[32];                    
      uint8_t behavior_serialized[16];                       
      // ... other POD fields                                
  }                                                           
                                                              
  ESP-NOW transmits this POD struct over wireless            

```

## Implementation Details

### Global State Header (p32_shared_state.h)

```cpp
#ifndef P32_SHARED_STATE_H
#define P32_SHARED_STATE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Global loop counter
extern uint32_t g_loopCount;

// POD struct for ESP-NOW mesh transmission (Layer 2)
typedef struct {
    uint32_t version;
    uint32_t timestamp_ms;
    uint8_t source_node_id;
    
    // Serialized C++ class instances
    int8_t mood_serialized[9];           // Mood class: 9 int8_t components
    uint8_t personality_serialized[32];  // Personality class: TBD structure
    uint8_t behavior_serialized[16];     // BehaviorState class: TBD structure
    
    // Sensor data (POD fields)
    uint8_t distance_cm;
    bool touch_detected;
    // ... more POD fields
    
    uint32_t checksum;
} p32_shared_state_t;

extern p32_shared_state_t g_shared_state;

#ifdef __cplusplus
}
#endif

// ===== C++ Layer 1: Working Class Instances =====
#ifdef __cplusplus
#include "Mood.hpp"
#include "Personality.hpp"
#include "BehaviorState.hpp"

// Global C++ class instances (Layer 1 - components work with these)
extern Mood g_mood;
extern Personality g_personality;
extern BehaviorState g_behavior;

// ===== Serialization: C++ Class  POD Struct =====

inline void mood_serialize() {
    for (int i = 0; i < Mood::componentCount; i++) {
        g_shared_state.mood_serialized[i] = g_mood.components[i];
    }
}

inline void personality_serialize() {
    // Serialize Personality class to personality_serialized[]
    // Implementation depends on Personality class structure
}

inline void behavior_serialize() {
    // Serialize BehaviorState class to behavior_serialized[]
}

// Master function: Serialize ALL classes before ESP-NOW broadcast
inline void sync_all_classes_to_shared_state() {
    mood_serialize();
    personality_serialize();
    behavior_serialize();
}

// ===== Deserialization: POD Struct  C++ Class =====

inline void mood_deserialize() {
    for (int i = 0; i < Mood::componentCount; i++) {
        g_mood.components[i] = g_shared_state.mood_serialized[i];
    }
}

inline void personality_deserialize() {
    // Deserialize personality_serialized[] to Personality class
}

inline void behavior_deserialize() {
    // Deserialize behavior_serialized[] to BehaviorState class
}

// Slave function: Deserialize ALL classes after ESP-NOW receive
inline void sync_all_classes_from_shared_state() {
    mood_deserialize();
    personality_deserialize();
    behavior_deserialize();
}

#endif // __cplusplus

#endif // P32_SHARED_STATE_H
```

### Component Usage Pattern

#### C++ Component Modifying Mood

```cpp
// src/components/goblin_mood.cpp

#include "p32_shared_state.h"
#include "Mood.hpp"

esp_err_t goblin_mood_init(void) {
    // Initialize C++ Mood class
    g_mood.happiness() = 50;
    g_mood.curiosity() = 30;
    
    // Serialize to shared state
    mood_serialize();
    
    return ESP_OK;
}

void goblin_mood_act(void) {
    // NO ARGUMENTS - access globals
    
    // Read sensor data
    uint8_t distance = g_shared_state.distance_cm;
    
    // WORK WITH C++ CLASS DIRECTLY (Layer 1)
    if (distance < 30) {
        g_mood.addCuriosity(20);   // Use C++ methods
        g_mood.addExcitement(15);
    }
    
    // Apply natural decay
    if (g_loopCount % 50 == 0) {
        if (g_mood.excitement() > 0) {
            g_mood.addExcitement(-5);
        }
    }
    
    // SERIALIZE to Layer 2 before mesh broadcast
    sync_all_classes_to_shared_state();
    
    // Mark dirty to trigger ESP-NOW broadcast
    mesh_mark_state_dirty();
    
    ESP_LOGD(TAG, "Mood: HAPPINESS=%d, CURIOSITY=%d", 
        g_mood.happiness(), g_mood.curiosity());
}
```

#### C++ Component Reading Mood

```cpp
// src/components/goblin_eye_left.cpp

#include "p32_shared_state.h"
#include "Mood.hpp"

void goblin_eye_left_act(void) {
    // NO ARGUMENTS - access globals
    
    // READ FROM C++ CLASS DIRECTLY (Layer 1)
    // (Already deserialized by ESP-NOW receive handler)
    int8_t happiness = g_mood.happiness();
    int8_t anger = g_mood.anger();
    int8_t fear = g_mood.fear();
    
    // Use C++ methods
    if (g_mood.hasAnyMood()) {
        render_eye_expression(happiness, anger, fear);
    }
    
    ESP_LOGD(TAG, "Eye rendered with mood at loop %u", g_loopCount);
}
```

### ESP-NOW Mesh Component Integration with Dirty Checking

```cpp
// src/components/esp_now_mesh.cpp

#include "p32_shared_state.h"

void esp_now_mesh_act(void) {
    // NO ARGUMENTS - executes every loop (hitCount: 1)
    
    // METHOD 1: Per-Class Change Detection (operator!= checking)
    // 
    // bool dirty = have_classes_changed();  // Checks each class individually
    
    // METHOD 2: Whole-Struct Change Detection (memcmp - RECOMMENDED)
    // 
    // Serialize C++ classes to POD struct first
    sync_all_classes_to_shared_state();
    
    // FAST: Single memcmp on entire buffer (much faster than per-class checking)
    bool dirty = has_shared_state_changed();
    
    if (dirty) {
        // Update metadata
        g_shared_state.version++;
        g_shared_state.timestamp_ms = esp_timer_get_time() / 1000;
        g_shared_state.source_node_id = get_local_node_id();
        
        // Calculate checksum
        g_shared_state.checksum = calculate_crc32(&g_shared_state,
            sizeof(p32_shared_state_t) - sizeof(uint32_t));
        
        // Broadcast POD struct via ESP-NOW
        esp_now_send(NULL, (uint8_t *)&g_shared_state, 
            sizeof(p32_shared_state_t));
        
        // FAST: Single memcpy to save entire shadow copy
        save_shared_state_as_previous();
        
        ESP_LOGD(TAG, "State broadcast: v%u (state changed)", 
            g_shared_state.version);
    }
    // No broadcast if nothing changed - saves wireless bandwidth!
}

// ESP-NOW receive callback
void on_esp_now_recv(const uint8_t *mac_addr, const uint8_t *data, int len) {
    p32_shared_state_t *received = (p32_shared_state_t *)data;
    
    // Validate and version check
    if (validate_checksum(received) && received->version > g_shared_state.version) {
        // Copy POD struct to local
        memcpy(&g_shared_state, received, sizeof(p32_shared_state_t));
        
        // DESERIALIZE to C++ class instances (Layer 1)
        sync_all_classes_from_shared_state();
        
        ESP_LOGI(TAG, "State synced: v%u, Mood.happiness=%d",
            g_shared_state.version, g_mood.happiness());
    }
}

// Public API for components
void mesh_mark_state_dirty(void) {
    s_state_dirty = true;
}
```

## Why This Pattern?

### Benefits

1. **OOP in Components**: Components use clean C++ class interfaces
   - `g_mood.happiness() = 80` instead of `g_shared_state.mood_serialized[2] = 80`
   - Encapsulation, methods, operator overloading all work

2. **ESP-NOW Compatibility**: Mesh transmission uses POD struct
   - No vtables, no pointers, no C++ runtime overhead
   - Direct memory copy for wireless transmission

3. **Transparent Synchronization**: Components don't worry about mesh
   - Just work with C++ classes normally
   - Mesh component handles serialize/deserialize automatically

4. **Type Safety**: Compiler enforces correct usage
   - Can't accidentally transmit non-POD data over ESP-NOW
   - Can't forget to serialize before broadcast

### Trade-offs

1. **Memory Overhead**: Two copies of state (C++ class + serialized POD)
   - Acceptable: Mood is 9 bytes, Personality ~32 bytes, negligible on ESP32
   
2. **Serialization Cost**: CPU cycles for serialize/deserialize
   - Optimized: Simple memcpy or array iteration
   - Only happens when state changes (not every loop)

3. **Manual Sync**: Must remember to call serialize/deserialize
   - Mitigated: Wrapper functions `sync_all_classes_*()` centralize logic
   - Mesh component calls automatically on send/receive

## Complete Example: Full Cycle

### Torso Master Updates Mood

```
1. Component modifies C++ class:
   g_mood.happiness() = 80;
   
2. Component serializes to POD:
   sync_all_classes_to_shared_state();
    mood_serialize() copies g_mood.components[] to g_shared_state.mood_serialized[]
   
3. Component marks dirty:
   mesh_mark_state_dirty();
   
4. Mesh component broadcasts POD struct:
   esp_now_send(NULL, &g_shared_state, sizeof(p32_shared_state_t));
```

### Head Slave Receives Update

```
5. ESP-NOW receives POD struct:
   on_esp_now_recv(mac, data, len)
   
6. Mesh component validates and copies:
   memcpy(&g_shared_state, received, sizeof(p32_shared_state_t))
   
7. Mesh component deserializes to C++ class:
   sync_all_classes_from_shared_state();
    mood_deserialize() copies g_shared_state.mood_serialized[] to g_mood.components[]
   
8. Display component reads C++ class:
   int8_t happiness = g_mood.happiness();  // = 80 (synchronized!)
```

## Adding New C++ Class Components

### Step 1: Define C++ Class

```cpp
// include/Personality.hpp
class Personality {
public:
    // Personality traits
    int8_t aggressiveness = 0;
    int8_t playfulness = 0;
    int8_t sociability = 0;
    // ... more traits
};
```

### Step 2: Add Serialized Field to POD Struct

```cpp
// include/p32_shared_state.h
typedef struct {
    // ... existing fields
    uint8_t personality_serialized[32];  // Size matches class
    // ...
} p32_shared_state_t;
```

### Step 3: Add Global Instance

```cpp
// include/p32_shared_state.h (C++ section)
extern Personality g_personality;
```

### Step 4: Implement Serialization

```cpp
// include/p32_shared_state.h (inline functions)
inline void personality_serialize() {
    g_shared_state.personality_serialized[0] = g_personality.aggressiveness;
    g_shared_state.personality_serialized[1] = g_personality.playfulness;
    // ... copy all fields
}

inline void personality_deserialize() {
    g_personality.aggressiveness = g_shared_state.personality_serialized[0];
    g_personality.playfulness = g_shared_state.personality_serialized[1];
    // ... restore all fields
}
```

### Step 5: Update Master Sync Functions

```cpp
inline void sync_all_classes_to_shared_state() {
    mood_serialize();
    personality_serialize();  // ADD THIS
}

inline void sync_all_classes_from_shared_state() {
    mood_deserialize();
    personality_deserialize();  // ADD THIS
}
```

### Step 6: Define Global in Implementation

```cpp
// src/p32_shared_state.cpp
Personality g_personality;
```

Done! Now all components can use `g_personality` directly and mesh synchronization happens automatically.

## Performance Comparison: Per-Class vs. Whole-Struct Change Detection

### Method 1: Per-Class Checking (Less Efficient)

```cpp
// Check each class individually using operator!=
bool have_classes_changed() {
    if (g_mood != g_mood_previous) return true;
    if (g_personality != g_personality_previous) return true;
    // More classes = more comparisons
    return false;
}

// Multiple memcpy operations
void save_classes_as_previous() {
    g_mood_previous = g_mood;  // Mood::operator= uses memcpy
    g_personality_previous = g_personality;  // Personality::operator= uses memcpy
    // Each class has its own shadow copy
}
```

**Cost**: N comparisons + N memcpy operations (where N = number of classes)

### Method 2: Whole-Struct Checking (RECOMMENDED)

```cpp
// Single memcmp on entire POD buffer
bool has_shared_state_changed() {
    const size_t metadata_size = offsetof(p32_shared_state_t, mood_serialized);
    const size_t data_size = sizeof(p32_shared_state_t) - metadata_size - sizeof(uint32_t);
    
    const uint8_t* current_data = (const uint8_t*)&g_shared_state + metadata_size;
    const uint8_t* previous_data = (const uint8_t*)&g_shared_state_previous + metadata_size;
    
    return (memcmp(current_data, previous_data, data_size) != 0);
}

// Single memcpy for entire struct
void save_shared_state_as_previous() {
    memcpy(&g_shared_state_previous, &g_shared_state, sizeof(p32_shared_state_t));
}
```

**Cost**: 1 comparison + 1 memcpy operation (constant time regardless of class count)

### Performance Benefits

| Aspect | Per-Class Method | Whole-Struct Method | Winner |
|--------|------------------|---------------------|--------|
| **Comparisons** | N (one per class) | 1 (entire buffer) |  Whole-Struct |
| **Memory Copies** | N (one per class) | 1 (entire buffer) |  Whole-Struct |
| **Code Complexity** | Grows with classes | Constant |  Whole-Struct |
| **CPU Cycles** | O(N) | O(1) |  Whole-Struct |
| **Maintenance** | Update per class | No changes needed |  Whole-Struct |

**Example with 5 classes**:
- Per-Class: 5 operator!= calls + 5 operator= calls
- Whole-Struct: 1 memcmp + 1 memcpy

**Speedup**: ~5-10x faster with memcmp method!

### When to Use Each Method

**Use Per-Class (Method 1)** when:
- Debugging specific class changes
- Need detailed logging per class
- Learning the system

**Use Whole-Struct (Method 2)** when:
- Production code (performance critical)
- Many C++ class globals (5+)
- Mesh broadcasts every loop cycle
- Minimizing CPU overhead

## Summary

**Design Pattern**: C++ classes for component usage + POD struct for mesh transmission

**Key Insight**: Separate "working state" (C++) from "transmission state" (POD)

**Performance Optimization**: Use `memcmp` on entire buffer instead of per-class checking

**Trade-off**: Small memory overhead for massive developer experience improvement

**Result**: Components use clean OOP interfaces, mesh gets efficient POD transmission with minimal CPU overhead

 **[Mesh State Synchronization](MESH-STATE-SYNCHRONIZATION-SPEC.md)**
 **[Component Function Signatures](COMPONENT-FUNCTION-SIGNATURES.md)**
 **[Three-Level Component Attachment](THREE-LEVEL-COMPONENT-ATTACHMENT-SPEC.md)**
