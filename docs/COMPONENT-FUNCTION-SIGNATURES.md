# Component Function Signatures - NO ARGUMENTS Pattern

## CRITICAL SIMPLIFICATION

**All component functions use NO ARGUMENTS** - they access global state directly.

## Rationale

1. **Global loop counter** (`g_loopCount`) is part of shared state
2. **All shared data** (mood, sensors, system status) is in `g_shared_state`
3. **ESP-NOW mesh** synchronizes global state across all subsystems
4. **Simpler function signatures** - no parameter passing needed
5. **Consistent access pattern** - all components use same global includes

## Standard Pattern

### Component Header

```cpp
// include/p32_shared_state.h

#ifndef P32_SHARED_STATE_H
#define P32_SHARED_STATE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Global loop counter (increments every 100ms)
extern uint32_t g_loopCount;

// Shared state structure (synchronized via ESP-NOW mesh)
typedef struct {
    // Version and sync metadata
    uint32_t version;
    uint32_t timestamp_ms;
    uint8_t source_node_id;
    
    // Mood state (serialized from C++ Mood class - see include/Mood.hpp)
    // Mood class has 9 components (-128 to +127 each):
    // ANGER, FEAR, HAPPINESS, SADNESS, CURIOSITY, AFFECTION, 
    // IRRITATION, CONTENTMENT, EXCITEMENT
    int8_t mood_serialized[9];  // Serialized for ESP-NOW transmission
    
    // Sensor data
    uint8_t distance_cm;
    bool touch_detected;
    int16_t temperature_c;
    
    // Behavioral state
    bool is_speaking;
    bool is_moving;
    uint8_t current_behavior;
    
    // System status
    uint8_t battery_percent;
    uint16_t uptime_seconds;
    bool wifi_connected;
    
    uint32_t checksum;
} p32_shared_state_t;

extern p32_shared_state_t g_shared_state;

#ifdef __cplusplus
}
#endif

// C++ Global Mood Instance (used by all C++ components)
#ifdef __cplusplus
#include "Mood.hpp"
extern Mood g_mood;  // Global C++ Mood class instance

// Synchronization helper: Copy Mood class to serialized array
inline void mood_serialize() {
    for (int i = 0; i < Mood::componentCount; i++) {
        g_shared_state.mood_serialized[i] = g_mood.components[i];
    }
}

// Synchronization helper: Copy serialized array to Mood class
inline void mood_deserialize() {
    for (int i = 0; i < Mood::componentCount; i++) {
        g_mood.components[i] = g_shared_state.mood_serialized[i];
    }
}
#endif

#endif // P32_SHARED_STATE_H
```

### Component Implementation

```cpp
// src/components/example_component.cpp

#include "p32_component_config.h"
#include "p32_shared_state.h"      //  ALWAYS include this

#ifdef ENABLE_{FAMILY}_COMPONENTS

// Init function: NO ARGUMENTS
esp_err_t example_component_init(void) {
    ESP_LOGI(TAG, "Initializing example component...");
    // Access globals directly if needed during init
    ESP_LOGI(TAG, "Starting at loop count: %u", g_loopCount);
    // Initialize hardware, allocate resources, etc.
    return ESP_OK;
}

// Act function: NO ARGUMENTS
void example_component_act(void) {
    // Access g_loopCount from global
    // (component executes when g_loopCount % hitCount == 0)
    // Read shared state
    float happiness = g_shared_state.mood_values[MOOD_HAPPINESS];
    uint8_t distance = g_shared_state.distance_cm;
    // Component logic
    if (distance < 30) {
        do_something();
    }
    // Modify shared state
    g_shared_state.mood_values[MOOD_CURIOSITY] = 0.7f;
    // Mark state dirty to trigger ESP-NOW broadcast
    mesh_mark_state_dirty();
    ESP_LOGD(TAG, "Component executed at loop %u", g_loopCount);
}

#endif // ENABLE_{FAMILY}_COMPONENTS
```

## Component Registration Tables

### Generated Header

```c
// include/p32_component_registry.h (GENERATED)

#ifndef P32_COMPONENT_REGISTRY_H
#define P32_COMPONENT_REGISTRY_H

#include <esp_err.h>

// Init function signature: NO ARGUMENTS
typedef esp_err_t (*init_func_t)(void);

// Act function signature: NO ARGUMENTS
typedef void (*act_func_t)(void);

// Act table entry with timing
typedef struct {
    act_func_t act_func;
    uint32_t hitCount;
    const char* name;
} act_table_entry_t;

// Table sizes
#define INIT_TABLE_SIZE 25
#define ACT_TABLE_SIZE 25

// Tables
extern init_func_t initTable[INIT_TABLE_SIZE];
extern act_table_entry_t actTable[ACT_TABLE_SIZE];

#endif // P32_COMPONENT_REGISTRY_H
```

### Generated Implementation

```cpp
// src/component_tables.cpp (GENERATED)

#include "p32_component_registry.h"

// Forward declarations: ALL use void signature
extern esp_err_t system_core_init(void);
extern void system_core_act(void);

extern esp_err_t esp_now_mesh_init(void);
extern void esp_now_mesh_act(void);

extern esp_err_t goblin_personality_init(void);
extern void goblin_personality_act(void);

extern esp_err_t goblin_eye_left_init(void);
extern void goblin_eye_left_act(void);

// Init table
init_func_t initTable[INIT_TABLE_SIZE] = {
    system_core_init,
    esp_now_mesh_init,
    goblin_personality_init,
    goblin_eye_left_init,
    // ... more components
};

// Act table with timing
act_table_entry_t actTable[ACT_TABLE_SIZE] = {
    {system_core_act,        100, "system_core"},
    {esp_now_mesh_act,         1, "esp_now_mesh"},
    {goblin_personality_act,  25, "goblin_personality"},
    {goblin_eye_left_act,      5, "left_eye"},
    // ... more components
};
```

## Main Loop

```cpp
// src/main.cpp

#include "p32_component_registry.h"
#include "p32_shared_state.h"

// Global loop counter (part of shared state)
uint32_t g_loopCount = 0;

// Global shared state (synchronized via ESP-NOW mesh)
p32_shared_state_t g_shared_state = {0};

void app_main(void)
{
    ESP_LOGI(TAG, "P32 Animatronic System Starting...");
    // Initialize shared state
    g_shared_state.version = 0;
    g_shared_state.timestamp_ms = 0;
    // Initialize all components
    ESP_LOGI(TAG, "Initializing %d components...", INIT_TABLE_SIZE);
    for (int i = 0; i < INIT_TABLE_SIZE; i++) {
        if (initTable[i] != NULL) {
            esp_err_t ret = initTable[i]();  //  NO ARGUMENTS
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to initialize component %d", i);
            }
        }
    }
    ESP_LOGI(TAG, "All components initialized, starting main loop");
    // Main component loop
    while (true) {
        // Execute components based on timing
        for (int i = 0; i < ACT_TABLE_SIZE; i++) {
            if (actTable[i].act_func != NULL && actTable[i].hitCount > 0) {
                if (g_loopCount % actTable[i].hitCount == 0) {
                    actTable[i].act_func();  //  NO ARGUMENTS
                }
            }
        }
        // Increment global loop counter
        g_loopCount++;
        // 100ms loop period (10 Hz base rate)
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

## Examples

### System Component (WiFi)

```c
// src/components/wifi_station.c

#include "p32_shared_state.h"

esp_err_t wifi_station_init(void) {
    // NO ARGUMENTS
    esp_wifi_init();
    esp_wifi_set_mode(WIFI_MODE_STA);
    return ESP_OK;
}

void wifi_station_act(void) {
    // NO ARGUMENTS - access g_loopCount from global
    // hitCount: 50  executes every 5 seconds
    
    // Update WiFi status in shared state
    g_shared_state.wifi_connected = esp_wifi_is_connected();
    
    // Mark state dirty if status changed
    if (g_shared_state.wifi_connected != prev_status) {
        mesh_mark_state_dirty();
    }
}
```

### Family Component (Mood - C++ Class Integration)

```cpp
// src/components/goblin_mood.cpp

#include "p32_shared_state.h"
#include "Mood.hpp"

esp_err_t goblin_mood_init(void) {
    // NO ARGUMENTS
    // Initialize mood values using C++ Mood class
    g_mood.happiness() = 50;
    g_mood.curiosity() = 30;
    g_mood.contentment() = 40;
    
    // Serialize to shared state for mesh synchronization
    mood_serialize();
    
    return ESP_OK;
}

void goblin_mood_act(void) {
    // NO ARGUMENTS - access g_loopCount from global
    // hitCount: 15  executes every 1.5 seconds
    
    // Read sensor data from shared state
    uint8_t distance = g_shared_state.distance_cm;
    
    // Update mood using C++ class methods
    if (distance < 30) {
        g_mood.addCuriosity(20);  // Increase curiosity
        g_mood.addExcitement(15); // Increase excitement
        
        // Serialize updated mood to shared state
        mood_serialize();
        
        // Mark state dirty to trigger ESP-NOW broadcast
        mesh_mark_state_dirty();
    }
    
    // Apply natural mood decay over time
    if (g_loopCount % 50 == 0) {  // Every 5 seconds
        // Decay excitement back toward neutral
        if (g_mood.excitement() > 0) {
            g_mood.addExcitement(-5);
            mood_serialize();
            mesh_mark_state_dirty();
        }
    }
    
    ESP_LOGD(TAG, "Mood: HAPPINESS=%d, CURIOSITY=%d at loop %u", 
        g_mood.happiness(), g_mood.curiosity(), g_loopCount);
}
```

### Bot-Specific Component (Display)

```c
// src/components/goblin_eye_left.c

#include "p32_shared_state.h"

static spi_device_handle_t s_spi_display = NULL;

esp_err_t goblin_eye_left_init(void) {
    // NO ARGUMENTS
    ESP_LOGI(TAG, "Initializing left eye at [-26.67, 17.78, -8.89]");
    
    // Initialize GC9A01 display via SPI
    spi_bus_add_device(SPI2_HOST, &dev_config, &s_spi_display);
    
    return ESP_OK;
}

void goblin_eye_left_act(void) {
    // NO ARGUMENTS - access g_loopCount and mood from globals
    // hitCount: 5  executes every 500ms
    
    // Read synchronized mood state
    float happiness = g_shared_state.mood_values[MOOD_HAPPINESS];
    float anger = g_shared_state.mood_values[MOOD_ANGER];
    
    // Render expression based on mood
    render_eye_expression(happiness, anger);
    
    // Send frame buffer to display via SPI
    spi_device_transmit(s_spi_display, &trans);
    
    ESP_LOGD(TAG, "Eye rendered at loop %u", g_loopCount);
}
```

### Mesh Synchronization Component

```c
// src/components/esp_now_mesh.c

#include "p32_shared_state.h"

static bool s_state_dirty = false;

esp_err_t esp_now_mesh_init(void) {
    // NO ARGUMENTS
    esp_now_init();
    register_mesh_peers();
    esp_now_register_recv_cb(on_state_received);
    return ESP_OK;
}

void esp_now_mesh_act(void) {
    // NO ARGUMENTS - access g_loopCount from global
    // hitCount: 1  executes EVERY loop (100ms)
    
    // Check if state needs broadcasting
    if (s_state_dirty) {
        // Update metadata
        g_shared_state.version++;
        g_shared_state.timestamp_ms = esp_timer_get_time() / 1000;
        
        // Broadcast to all mesh nodes
        esp_now_send(NULL, (uint8_t *)&g_shared_state, 
            sizeof(p32_shared_state_t));
        
        s_state_dirty = false;
        
        ESP_LOGD(TAG, "State broadcast: v%u at loop %u",
            g_shared_state.version, g_loopCount);
    }
}

// Public API for components to mark state dirty
void mesh_mark_state_dirty(void) {
    s_state_dirty = true;
}
```

## Benefits

### 1. **Simplified Function Signatures**
- No need to pass `loopCount` parameter
- All components have identical signature pattern
- Easier to understand and maintain

### 2. **Direct Global Access**
- Components access `g_loopCount` when needed
- All shared state in one place (`g_shared_state`)
- Clear data flow

### 3. **Consistent Pattern**
- Every component includes `p32_shared_state.h`
- Every component uses same access pattern
- No special cases or exceptions

### 4. **ESP-NOW Mesh Integration**
- Shared state automatically synchronized
- Components don't need to know about mesh
- Same code works on all subsystems

### 5. **Code Generation Simplification**
- Function pointers have simple signature: `void (*)(void)`
- No parameter marshalling needed
- Tables are simpler to generate

## Summary

**OLD PATTERN (Rejected)**:
```c
esp_err_t component_init(void);
void component_act(void);  //  NO ARGUMENTS pattern per AI-AGENT-RULES.md

// Main loop
actTable[i].act_func(loopCount);  // Pass parameter
```

**NEW PATTERN (Adopted)**:
```c
esp_err_t component_init(void);
void component_act(void);  //  NO ARGUMENTS

// Main loop
g_loopCount++;  // Update global
actTable[i].act_func();  // NO ARGUMENTS

// Component accesses global directly
void component_act(void) {
    ESP_LOGD(TAG, "Loop %u", g_loopCount);  // Read global
}
```

**Reasoning**:
- `g_loopCount` is part of global shared state
- ESP-NOW mesh synchronizes all globals across subsystems
- Simpler function signatures
- Consistent with other global state access (mood, sensors, etc.)

 **[Three-Level Component Attachment](THREE-LEVEL-COMPONENT-ATTACHMENT-SPEC.md)**
 **[Mesh State Synchronization](MESH-STATE-SYNCHRONIZATION-SPEC.md)**
 **[Hierarchical Component Composition](HIERARCHICAL-COMPONENT-COMPOSITION-SPEC.md)**
