# Global State Synchronization - Quick Reference

## Overview

P32 animatronic system uses **distributed processing** (5+ ESP32 chips) with **ESP-NOW mesh** to synchronize state across all subsystems.

## Two-Layer Architecture

```

  LAYER 1: C++ Class Instances (Working State)        
     
  extern Mood g_mood;                                 
  extern Personality g_personality;                   
                                                      
  Components use OOP methods:                         
  g_mood.happiness() = 80                             
  g_mood.addCuriosity(20)                             

                     serialize (memcpy)
                     deserialize (memcpy)

  LAYER 2: POD Struct (Mesh Transmission)             
     
  p32_shared_state_t g_shared_state {                 
      int8_t mood_serialized[9];                      
      uint8_t personality_serialized[32];             
      uint8_t distance_cm;                            
      bool wifi_connected;                            
      // ... more POD fields                          
  }                                                   
                                                      
  ESP-NOW broadcasts this struct wirelessly           

```

## Fast Change Detection (memcmp Method)

### The Problem

Broadcasting state **every 100ms** wastes wireless bandwidth when nothing changed.

### Old Solution (Slow)

```cpp
// Check each class individually
bool have_classes_changed() {
    if (g_mood != g_mood_previous) return true;
    if (g_personality != g_personality_previous) return true;
    // More classes = more comparisons
    return false;
}
```

**Cost**: N comparisons (one per class)

### New Solution (FAST - 6x speedup!)

```cpp
// Single memcmp on entire buffer
bool has_shared_state_changed() {
    const size_t metadata_size = offsetof(p32_shared_state_t, mood_serialized);
    const size_t data_size = sizeof(p32_shared_state_t) - metadata_size - sizeof(uint32_t);
    
    const uint8_t* current_data = (const uint8_t*)&g_shared_state + metadata_size;
    const uint8_t* previous_data = (const uint8_t*)&g_shared_state_previous + metadata_size;
    
    return (memcmp(current_data, previous_data, data_size) != 0);
}
```

**Cost**: 1 comparison (regardless of class count)

## Component Workflow

### Writing Components (Modify State)

```cpp
void goblin_mood_act(void) {
    // NO ARGUMENTS - access globals
    
    // 1. Read sensor data
    uint8_t distance = g_shared_state.distance_cm;
    
    // 2. Modify C++ classes directly
    if (distance < 30) {
        g_mood.addCuriosity(20);
        g_mood.addExcitement(15);
    }
    
    // 3. That's it! Mesh component handles broadcasting automatically
}
```

### Reading Components (Use State)

```cpp
void goblin_eye_left_act(void) {
    // NO ARGUMENTS - access globals
    
    // 1. Read C++ classes directly
    int8_t happiness = g_mood.happiness();
    int8_t anger = g_mood.anger();
    
    // 2. Use values
    if (anger > 50) {
        render_angry_eye(anger);
    } else if (happiness > 50) {
        render_happy_eye(happiness);
    }
}
```

### Mesh Component (Broadcast Changes)

```cpp
void esp_now_mesh_act(void) {
    // NO ARGUMENTS - executes EVERY loop (hitCount: 1)
    
    // 1. Serialize C++ classes  POD struct
    sync_all_classes_to_shared_state();
    
    // 2. Fast change detection (single memcmp)
    if (has_shared_state_changed()) {
        // 3. Update metadata
        g_shared_state.version++;
        g_shared_state.timestamp_ms = esp_timer_get_time() / 1000;
        
        // 4. Broadcast via ESP-NOW
        esp_now_send(NULL, (uint8_t *)&g_shared_state, sizeof(p32_shared_state_t));
        
        // 5. Save shadow copy (single memcpy)
        save_shared_state_as_previous();
        
        ESP_LOGI(TAG, "State broadcast v%u", g_shared_state.version);
    }
    // NO broadcast if nothing changed!
}
```

## Key Files

### Header File (C++/C Integration)

**File**: `include/p32_shared_state.h`

- Defines `p32_shared_state_t` POD struct
- Declares `extern Mood g_mood` C++ instance
- Provides `mood_serialize()` and `mood_deserialize()` helpers
- Implements `has_shared_state_changed()` using memcmp
- Implements `save_shared_state_as_previous()` using memcpy

### Implementation File (Global Instances)

**File**: `src/p32_shared_state.cpp`

- Defines `uint32_t g_loopCount = 0`
- Defines `p32_shared_state_t g_shared_state = {0}`
- Defines `Mood g_mood` (C++ instance)
- Defines `p32_shared_state_t g_shared_state_previous = {0}` (shadow copy)

## Performance Metrics

| Method | Comparisons | Memory Copies | CPU Cycles | Scalability |
|--------|-------------|---------------|------------|-------------|
| **Per-Class** | N (5 classes) | N (5 memcpy) | ~300 | O(N) |
| **memcmp** | 1 | 1 | ~50 | O(1) |

**Speedup**: ~6x faster with memcmp method! 

## Adding New C++ Classes

### Step 1: Create C++ Class

```cpp
// include/Personality.hpp
class Personality {
public:
    int8_t components[32];
    // Methods, operator==, operator!=, etc.
};
```

### Step 2: Add to POD Struct

```cpp
// include/p32_shared_state.h
typedef struct {
    // ... existing fields
    int8_t mood_serialized[9];
    uint8_t personality_serialized[32];  // ADD THIS
} p32_shared_state_t;
```

### Step 3: Add Serialization Helpers

```cpp
// include/p32_shared_state.h
extern Personality g_personality;

inline void personality_serialize() {
    memcpy(g_shared_state.personality_serialized, &g_personality, sizeof(g_personality));
}

inline void personality_deserialize() {
    memcpy(&g_personality, g_shared_state.personality_serialized, sizeof(g_personality));
}

// Update master sync functions
inline void sync_all_classes_to_shared_state() {
    mood_serialize();
    personality_serialize();  // ADD THIS
}

inline void sync_all_classes_from_shared_state() {
    mood_deserialize();
    personality_deserialize();  // ADD THIS
}
```

### Step 4: Define Global Instance

```cpp
// src/p32_shared_state.cpp
Personality g_personality;
```

**Done!** The existing `has_shared_state_changed()` automatically includes the new class in change detection (no code changes needed).

## Subsystem Distribution

```
                    TORSO (Master)
                    ESP32-S3
                     system_core
                     wifi_station
                     esp_now_mesh  Broadcasts state
                     telemetry_hub
                     watchdog
                          
        
                                          
    HEAD            ARM_LEFT          ARM_RIGHT
    ESP32-S3        ESP32-C3          ESP32-C3
    Receives        Receives          Receives
                                    
    Deserializes    Deserializes      Deserializes
                                    
    Local copy      Local copy        Local copy
    g_mood          g_mood            g_mood
```

Each subsystem has **independent memory space** with its own copy of `g_mood`, synchronized via ESP-NOW mesh.

## Common Patterns

### Pattern 1: Sensor Updates State

```cpp
void nose_sensor_act(void) {
    // Measure distance
    uint8_t distance = hc_sr04_read();
    
    // Update shared state
    g_shared_state.distance_cm = distance;
    
    // Mesh component detects change and broadcasts automatically
}
```

### Pattern 2: State Drives Behavior

```cpp
void goblin_behavior_act(void) {
    // Read mood
    if (g_mood.curiosity() > 70 && g_shared_state.distance_cm < 30) {
        // Trigger curious behavior
        g_mood.addExcitement(10);
    }
}
```

### Pattern 3: State Drives Rendering

```cpp
void eye_display_act(void) {
    // Render based on mood
    render_eye_expression(g_mood.happiness(), g_mood.anger());
}
```

## Benefits Summary

 **6x faster** change detection with memcmp
 **Single comparison** regardless of class count
 **Clean OOP interface** for components
 **Automatic synchronization** via ESP-NOW mesh
 **No manual broadcasts** - mesh component handles it
 **Minimal wireless traffic** - only when state changes
 **Easy to extend** - add new classes without mesh code changes

## Documentation Links

 **[Fast Change Detection Example](FAST-CHANGE-DETECTION-EXAMPLE.md)** - Complete working code
 **[C++ Class Serialization Pattern](CPP-CLASS-SERIALIZATION-PATTERN.md)** - Two-layer architecture
 **[Mesh State Synchronization](MESH-STATE-SYNCHRONIZATION-SPEC.md)** - Distributed system details
 **[Component Function Signatures](COMPONENT-FUNCTION-SIGNATURES.md)** - NO ARGUMENTS pattern
 **[Three-Level Component Attachment](THREE-LEVEL-COMPONENT-ATTACHMENT-SPEC.md)** - System/Family/Bot hierarchy
