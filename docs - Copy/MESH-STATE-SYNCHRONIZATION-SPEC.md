# Distributed Global State Synchronization via ESP-NOW Mesh

## CRITICAL DESIGN CHALLENGE

**Problem**: Components use global variables for communication, but each subsystem runs on a separate ESP32 chip with **independent memory spaces**.

**Solution**: ESP-NOW mesh network automatically synchronizes shared global state across all subsystems.

## Architecture Overview

### Single ESP32 System (Simple Case)

```cpp
// In a single-chip system, components share global memory:

#include "p32_shared_state.h"
#include "Mood.hpp"

// Global state (shared memory)
extern Mood g_mood;  // C++ Mood class with 9 components (-128 to +127 each)
extern uint8_t g_shared_state.distance_cm;
extern bool g_shared_state.is_speaking;

// Component A writes (C++ component)
void mood_component_act(void) {
    g_mood.happiness() = 80;  // Write to global Mood class
}

// Component B reads (same memory space, can be C or C++)
void display_component_act(void) {
    int8_t happiness = g_mood.happiness();  // Read global Mood class
    render_expression(happiness);
}

// ✓ Works because both components run on same ESP32
```

### Multi-ESP32 Distributed System (Complex Case)

```
┌─────────────────────────────────────────────────────────────────┐
│                 TORSO ESP32-S3 (Master)                         │
│  Memory Space: 0x3FC88000 - 0x3FCFFFFF                          │
├─────────────────────────────────────────────────────────────────┤
│  Global State (Local to Torso):                                 │
│  float current_mood[8] = {0.2, 0.5, ...}  ◄─── SEPARATE        │
│  uint8_t sensor_distance = 30                                   │
│  bool is_speaking = false                                       │
└─────────────────────────────────────────────────────────────────┘
                            │
                    ESP-NOW Mesh Network
                            │
      ┌─────────────────────┼─────────────────────┐
      ▼                     ▼                     ▼
┌──────────────────┐  ┌──────────────────┐  ┌──────────────────┐
│  HEAD ESP32-S3   │  │ ARM_L ESP32-C3   │  │ ARM_R ESP32-C3   │
│  Memory: 0x3FC88 │  │  Memory: 0x3FC00 │  │  Memory: 0x3FC00 │
├──────────────────┤  ├──────────────────┤  ├──────────────────┤
│  Global State:   │  │  Global State:   │  │  Global State:   │
│  float mood[8]   │  │  float mood[8]   │  │  float mood[8]   │
│  = {0.2, 0.5...} │  │  = {0.0, 0.0...} │  │  = {0.0, 0.0...} │
│  ▲               │  │  ▲               │  │  ▲               │
│  │ STALE!        │  │  │ STALE!        │  │  │ STALE!        │
│  └─ NOT SYNCED   │  │  └─ NOT SYNCED   │  │  └─ NOT SYNCED   │
└──────────────────┘  └──────────────────┘  └──────────────────┘

❌ Problem: Each ESP32 has its own copy of global state
❌ Changes on Torso don't automatically propagate to Head/Arms
❌ Head makes decisions based on stale mood values
```

## Solution: Distributed State Synchronization

### Every Subsystem Runs Mesh Network Component

**CRITICAL**: The `esp_now_mesh` component must be attached to **EVERY subsystem**, not just the master:

```json
// config/subsystems/goblin_torso.json
{
  "subsystem_components": [
    "config/components/system/esp_now_mesh.json"  ◄─── Master mode
  ]
}

// config/subsystems/goblin_head.json
{
  "subsystem_components": [
    "config/components/system/esp_now_mesh.json"  ◄─── Slave mode
  ]
}

// config/subsystems/goblin_arm_left.json
{
  "subsystem_components": [
    "config/components/system/esp_now_mesh.json"  ◄─── Slave mode
  ]
}
```

### Mesh Component Adapts Based on Role

```c
// src/components/esp_now_mesh.c

esp_err_t esp_now_mesh_init(void) {
    // Detect role from JSON configuration
    mesh_role_t role = get_mesh_role_from_config();
    
    if (role == MESH_MASTER) {
        ESP_LOGI(TAG, "Initializing as MESH MASTER");
        init_master_coordinator();
    } else {
        ESP_LOGI(TAG, "Initializing as MESH SLAVE");
        init_slave_responder();
    }
    
    // Initialize ESP-NOW protocol
    esp_now_init();
    
    // Register callbacks for state synchronization
    esp_now_register_recv_cb(on_state_update_received);
    esp_now_register_send_cb(on_state_update_sent);
    
    return ESP_OK;
}
```

## Global State Synchronization Protocol

### Shared State Structure

```c
// include/p32_shared_state.h
// This structure is replicated on EVERY ESP32 subsystem

#pragma pack(push, 1)
typedef struct {
    // Version and sync metadata
    uint32_t version;           // Incremented on every update
    uint32_t timestamp_ms;      // When this state was last updated
    uint8_t source_node_id;     // Which subsystem originated the change
    
    // Mood state (8 emotions)
    float mood_values[8];       // FEAR, ANGER, IRRITATION, etc.
    
    // Sensor aggregation
    uint8_t distance_cm;        // Nose sensor distance
    bool touch_detected;        // Touch sensor state
    int16_t temperature_c;      // Temperature reading
    
    // Behavioral flags
    bool is_speaking;           // Audio output active
    bool is_moving;             // Motion in progress
    uint8_t current_behavior;   // Active behavior ID
    
    // System status
    uint8_t battery_percent;    // Battery level
    uint16_t uptime_seconds;    // System uptime
    bool wifi_connected;        // WiFi status
    
    // Checksum for validation
    uint32_t checksum;          // CRC32 of entire structure
} p32_shared_state_t;
#pragma pack(pop)

// Global instance on EVERY ESP32
extern p32_shared_state_t g_shared_state;
```

### State Update Flow

#### Master Publishes State Change (C++ Component)

```cpp
// TORSO: Mood component updates global C++ class
void goblin_mood_act(void) {
    // NO ARGUMENTS - access g_loopCount from global
    
    // Update C++ Mood class instance
    g_mood.happiness() = 80;    // Update class directly
    g_mood.curiosity() = 60;
    
    // Serialize C++ class to POD struct for ESP-NOW transmission
    sync_all_classes_to_shared_state();  // Calls mood_serialize(), etc.
    
    // Update metadata
    g_shared_state.version++;
    g_shared_state.timestamp_ms = esp_timer_get_time() / 1000;
    g_shared_state.source_node_id = NODE_ID_TORSO;
    
    // Calculate checksum
    g_shared_state.checksum = calculate_crc32(&g_shared_state, 
        sizeof(p32_shared_state_t) - sizeof(uint32_t));
    
    // Broadcast POD struct to all slaves via ESP-NOW
    mesh_broadcast_state_update(&g_shared_state);
    
    ESP_LOGI(TAG, "Mood updated: HAPPINESS=%d, broadcasting to mesh", 
        g_mood.happiness());
}
```

#### Slaves Receive and Apply State (C++ Component)

```cpp
// HEAD: Receives state update via ESP-NOW callback
void on_state_update_received(const uint8_t *mac_addr, const uint8_t *data, int len) {
    p32_shared_state_t *received_state = (p32_shared_state_t *)data;
    
    // Validate checksum
    uint32_t calc_checksum = calculate_crc32(received_state, 
        sizeof(p32_shared_state_t) - sizeof(uint32_t));
    
    if (calc_checksum != received_state->checksum) {
        ESP_LOGW(TAG, "Invalid checksum, discarding state update");
        return;
    }
    
    // Check if newer than local state
    if (received_state->version > g_shared_state.version) {
        // Apply update to local POD struct
        memcpy(&g_shared_state, received_state, sizeof(p32_shared_state_t));
        
        // Deserialize POD struct to C++ class instances
        sync_all_classes_from_shared_state();  // Calls mood_deserialize(), etc.
        
        ESP_LOGI(TAG, "State synchronized: version=%u, HAPPINESS=%d",
            g_shared_state.version, g_mood.happiness());
        
        // Notify local components of state change
        notify_components_state_changed();
    }
}

// HEAD: Display component now reads synchronized C++ Mood class
void goblin_eye_left_act(void) {
    // NO ARGUMENTS - access g_loopCount and g_mood from globals
    
    // Read from C++ Mood class (automatically synchronized by mesh!)
    int8_t happiness = g_mood.happiness();
    int8_t anger = g_mood.anger();
    
    // Render expression based on synchronized mood
    render_expression(happiness, anger);
    
    ESP_LOGD(TAG, "Eye rendered at loop %u with HAPPINESS=%d", 
        g_loopCount, happiness);
    
    // ✓ Works because ESP-NOW mesh keeps C++ classes in sync
}
```

## Complete Synchronization Architecture

### State Update Sequence Diagram

```
Time: T0 (Sensor detects object)
┌──────────────────────────────────────────────────────────────────┐
│ HEAD: nose_sensor detects object @ 30cm                          │
│                                                                  │
│  void goblin_nose_act(uint32_t loopCount) {                     │
│      uint8_t distance = read_hc_sr04_sensor();                  │
│      g_shared_state.distance_cm = distance;  // Update local    │
│      g_shared_state.version++;                                  │
│      mesh_broadcast_state_update(&g_shared_state);              │
│  }                                                               │
└──────────────────────────────────────────────────────────────────┘
                            │
                    ESP-NOW Broadcast
                            │
      ┌─────────────────────┼─────────────────────┐
      ▼                     ▼                     ▼
Time: T0 + 5ms (All slaves receive)
┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│   TORSO      │    │   ARM_LEFT   │    │   ARM_RIGHT  │
│              │    │              │    │              │
│ Receives     │    │ Receives     │    │ Receives     │
│ state update │    │ state update │    │ state update │
│              │    │              │    │              │
│ g_shared_    │    │ g_shared_    │    │ g_shared_    │
│ state.       │    │ state.       │    │ state.       │
│ distance_cm  │    │ distance_cm  │    │ distance_cm  │
│ = 30         │    │ = 30         │    │ = 30         │
└──────────────┘    └──────────────┘    └──────────────┘

Time: T0 + 10ms (Torso processes and responds)
┌──────────────────────────────────────────────────────────────────┐
│ TORSO: Behavior planner evaluates sensor data                   │
│                                                                  │
│  void goblin_behavior_planner_act(uint32_t loopCount) {         │
│      if (g_shared_state.distance_cm < 50) {                     │
│          // Object detected, trigger CURIOUS mood               │
│          g_shared_state.mood_values[MOOD_CURIOSITY] = 0.9f;     │
│          g_shared_state.version++;                              │
│          mesh_broadcast_state_update(&g_shared_state);          │
│      }                                                           │
│  }                                                               │
└──────────────────────────────────────────────────────────────────┘
                            │
                    ESP-NOW Broadcast
                            │
      ┌─────────────────────┼─────────────────────┐
      ▼                     ▼                     ▼
Time: T0 + 15ms (All subsystems apply mood change)
┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│   HEAD       │    │   ARM_LEFT   │    │   ARM_RIGHT  │
│              │    │              │    │              │
│ g_shared_    │    │ g_shared_    │    │ g_shared_    │
│ state.mood   │    │ state.mood   │    │ state.mood   │
│ [CURIOSITY]  │    │ [CURIOSITY]  │    │ [CURIOSITY]  │
│ = 0.9        │    │ = 0.9        │    │ = 0.9        │
│              │    │              │    │              │
│ Display      │    │ Arm gesture  │    │ Arm gesture  │
│ shows wide   │    │ reaches fwd  │    │ reaches fwd  │
│ curious eyes │    │              │    │              │
└──────────────┘    └──────────────┘    └──────────────┘

Result: All subsystems act on synchronized mood state!
```

## Implementation Details

### Mesh Component Configuration

```json
// config/components/system/esp_now_mesh.json
{
  "component_name": "esp_now_mesh",
  "component_type": "SYSTEM_LEVEL",
  "timing": {
    "hitCount": 1,
    "description": "Check for state updates every loop (100ms)"
  },
  
  "synchronization_config": {
    "mode": "AUTOMATIC",
    "broadcast_on_change": true,
    "throttle_ms": 50,
    "max_broadcast_rate_hz": 20,
    "state_version_tracking": true,
    "conflict_resolution": "LATEST_TIMESTAMP_WINS"
  },
  
  "state_fields": [
    {
      "name": "mood_values",
      "type": "float[8]",
      "sync_priority": "HIGH",
      "broadcast_on_change": true
    },
    {
      "name": "distance_cm",
      "type": "uint8_t",
      "sync_priority": "MEDIUM",
      "broadcast_on_change": true
    },
    {
      "name": "battery_percent",
      "type": "uint8_t",
      "sync_priority": "LOW",
      "broadcast_on_change": false,
      "periodic_sync_interval_ms": 30000
    }
  ]
}
```

### ESP-NOW Mesh Component (Master Mode)

```c
// src/components/esp_now_mesh.c

#ifdef ENABLE_SYSTEM_COMPONENTS

static mesh_role_t s_mesh_role = MESH_ROLE_UNKNOWN;
static bool s_state_dirty = false;

esp_err_t esp_now_mesh_init(void) {
    // Determine role from subsystem configuration
    s_mesh_role = subsystem_get_mesh_role();
    
    // Initialize ESP-NOW protocol
    ESP_ERROR_CHECK(esp_now_init());
    
    if (s_mesh_role == MESH_MASTER) {
        ESP_LOGI(TAG, "Initializing MESH MASTER");
        register_all_slave_peers();
    } else {
        ESP_LOGI(TAG, "Initializing MESH SLAVE");
        register_master_peer();
    }
    
    // Register state synchronization callbacks
    esp_now_register_recv_cb(on_mesh_state_received);
    esp_now_register_send_cb(on_mesh_state_sent);
    
    // Initialize local shared state
    init_shared_state();
    
    return ESP_OK;
}

void esp_now_mesh_act(uint32_t loopCount) {
    // Every loop iteration (100ms)
    
    if (s_mesh_role == MESH_MASTER) {
        // Master: Broadcast state changes to all slaves
        if (s_state_dirty) {
            broadcast_state_to_slaves();
            s_state_dirty = false;
        }
        
        // Periodically request health from slaves
        if (loopCount % 50 == 0) {
            request_slave_health_status();
        }
    } else {
        // Slave: Check if local state needs to be sent to master
        if (s_state_dirty) {
            send_state_update_to_master();
            s_state_dirty = false;
        }
        
        // Respond to master heartbeat
        if (loopCount % 10 == 0) {
            send_heartbeat_to_master();
        }
    }
}

// Called by ANY component when it modifies shared state
void mesh_mark_state_dirty(void) {
    s_state_dirty = true;
}

// Broadcast state to all mesh nodes
void broadcast_state_to_slaves(void) {
    // Update metadata
    g_shared_state.version++;
    g_shared_state.timestamp_ms = esp_timer_get_time() / 1000;
    g_shared_state.source_node_id = get_local_node_id();
    
    // Calculate checksum
    g_shared_state.checksum = calculate_crc32(&g_shared_state,
        sizeof(p32_shared_state_t) - sizeof(uint32_t));
    
    // Broadcast via ESP-NOW
    esp_now_send(NULL, (uint8_t *)&g_shared_state, sizeof(p32_shared_state_t));
    
    ESP_LOGD(TAG, "State broadcast: version=%u, mood[0]=%.2f, loopCount=%u",
        g_shared_state.version, g_shared_state.mood_values[0], g_loopCount);
}

// Receive state update from mesh
void on_mesh_state_received(const uint8_t *mac_addr, const uint8_t *data, int len) {
    if (len != sizeof(p32_shared_state_t)) {
        ESP_LOGW(TAG, "Invalid state size: %d", len);
        return;
    }
    
    p32_shared_state_t *received_state = (p32_shared_state_t *)data;
    
    // Validate checksum
    uint32_t calc_checksum = calculate_crc32(received_state,
        sizeof(p32_shared_state_t) - sizeof(uint32_t));
    
    if (calc_checksum != received_state->checksum) {
        ESP_LOGW(TAG, "Checksum mismatch");
        return;
    }
    
    // Apply if newer version
    if (received_state->version > g_shared_state.version) {
        memcpy(&g_shared_state, received_state, sizeof(p32_shared_state_t));
        
        ESP_LOGI(TAG, "State synchronized from node %d, version=%u",
            received_state->source_node_id, received_state->version);
        
        // Notify local components
        notify_local_components_state_changed();
    } else if (received_state->version < g_shared_state.version) {
        ESP_LOGD(TAG, "Received stale state (v%u < v%u), ignoring",
            received_state->version, g_shared_state.version);
    }
}

#endif // ENABLE_SYSTEM_COMPONENTS
```

### Component Usage Pattern

```c
// ANY component on ANY subsystem can modify shared state

// Example 1: Mood component (on TORSO)
void goblin_mood_act(uint32_t loopCount) {
    // Update mood based on sensors/behavior
    g_shared_state.mood_values[MOOD_HAPPINESS] = 0.8f;
    
    // Mark state as dirty (triggers mesh broadcast)
    mesh_mark_state_dirty();
}

// Example 2: Display component (on HEAD) reads synchronized state
void goblin_eye_left_act(uint32_t loopCount) {
    // Read from local copy (kept in sync by mesh)
    float happiness = g_shared_state.mood_values[MOOD_HAPPINESS];
    
    // Use synchronized state
    render_expression(happiness);
}

// Example 3: Arm component (on ARM_LEFT ESP32-C3) also reads mood
void arm_left_gesture_act(uint32_t loopCount) {
    float curiosity = g_shared_state.mood_values[MOOD_CURIOSITY];
    
    if (curiosity > 0.7f) {
        // Arm reaches forward when curious
        set_arm_pose(POSE_REACH_FORWARD);
    }
}
```

## Performance Optimization

### Selective Synchronization

```c
// Only broadcast changed fields to minimize bandwidth

typedef struct {
    uint32_t version;
    uint8_t changed_fields_bitmap;  // Bit flags for changed fields
    
    // Only include data for changed fields
    union {
        float mood_values[8];
        uint8_t distance_cm;
        bool is_speaking;
    } data;
    
    uint32_t checksum;
} p32_delta_state_t;

// Broadcast only changes
void broadcast_delta_state(uint8_t changed_fields) {
    p32_delta_state_t delta;
    delta.version = g_shared_state.version;
    delta.changed_fields_bitmap = changed_fields;
    
    if (changed_fields & FIELD_MOOD_VALUES) {
        memcpy(delta.data.mood_values, g_shared_state.mood_values, sizeof(float) * 8);
    }
    
    esp_now_send(NULL, (uint8_t *)&delta, sizeof(delta));
}
```

### Throttling and Rate Limiting

```c
// Prevent mesh flooding
#define MAX_STATE_BROADCAST_RATE_HZ 20
#define MIN_BROADCAST_INTERVAL_MS (1000 / MAX_STATE_BROADCAST_RATE_HZ)

static uint32_t s_last_broadcast_time_ms = 0;

void esp_now_mesh_act(uint32_t loopCount) {
    uint32_t now_ms = esp_timer_get_time() / 1000;
    
    if (s_state_dirty) {
        // Throttle broadcasts
        if ((now_ms - s_last_broadcast_time_ms) >= MIN_BROADCAST_INTERVAL_MS) {
            broadcast_state_to_slaves();
            s_last_broadcast_time_ms = now_ms;
            s_state_dirty = false;
        }
    }
}
```

## Summary

### Key Architectural Decisions

1. ✅ **ESP-NOW mesh on EVERY subsystem** - Not just master, but all slaves too
2. ✅ **Automatic state synchronization** - Components don't need to know about mesh
3. ✅ **Global state structure replicated** - Same memory layout on all ESP32 chips
4. ✅ **Version-based conflict resolution** - Latest version always wins
5. ✅ **Checksum validation** - Ensure data integrity over wireless
6. ✅ **Throttling and delta updates** - Optimize bandwidth usage
7. ✅ **Transparent to components** - Components read/write globals normally, mesh handles sync

### Component Benefits

```
// Component code looks the same on any subsystem:

void some_component_act(uint32_t loopCount) {
    // Read global state (automatically synchronized!)
    float mood = g_shared_state.mood_values[MOOD_HAPPINESS];
    
    // Make decisions based on state
    if (mood > 0.7f) {
        do_happy_action();
    }
    
    // Modify global state
    g_shared_state.sensor_distance = read_sensor();
    
    // Mark dirty (triggers automatic mesh broadcast)
    mesh_mark_state_dirty();
}

// ✓ Works on Torso (ESP32-S3)
// ✓ Works on Head (ESP32-S3)
// ✓ Works on Arms (ESP32-C3)
// ✓ Works on Legs (ESP32-C3)
```

**Result**: Clean component abstraction with transparent distributed state synchronization!

📘 **[Distributed Processing Architecture](DISTRIBUTED-PROCESSING-ARCHITECTURE.md)**
📘 **[Hierarchical Component Composition](HIERARCHICAL-COMPONENT-COMPOSITION-SPEC.md)**
