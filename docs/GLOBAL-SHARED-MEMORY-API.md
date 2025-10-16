# Global Shared Memory API - Simple Read/Write Interface

## Design Philosophy

**Simple API for complex distributed system:**
- Components allocate named memory blocks during `init()`
- Components read/write using string names
- Mesh system automatically synchronizes across all subsystems
- Zero pointers, zero manual serialization

## CRITICAL Architecture Understanding

### read() Does NOT Use Mesh Network!

**`read()` is just `memcpy()` from global memory pool** - NO network latency, NO packets!

```c
void* read(const char *name) {
    // Just memcpy from global pool → local variable
    // NO mesh communication happens here!
    // Ultra-fast: <1 microsecond
    return memory_pool_pointer;
}
```

**`write()` triggers mesh broadcast** - synchronizes to all other subsystems:

```c
void write(const char *name, void *value) {
    // 1. memcpy local variable → global pool
    // 2. Mark block dirty
    // 3. Mesh component broadcasts on next loop
    //    (ESP-NOW packets sent to all peers)
}
```

### Typical Data Flow

```
Torso ESP32-S3 (Master):
┌─────────────────────────────────────────┐
│ Sensor: write("g_Envir", &envir)        │ ← Triggers broadcast
│    ↓ (memcpy to global pool)            │
│ Personality: read("g_Envir", &envir)    │ ← NO MESH (local memcpy)
│    ↓ (process mood changes)             │
│ Personality: write("g_MOOD", &mood)     │ ← Triggers broadcast
└─────────────────────────────────────────┘
              ↓ ESP-NOW mesh (1-5 ms)
Head ESP32-S3 (Slave):
┌─────────────────────────────────────────┐
│ Mesh: receives "g_MOOD" packet          │
│    ↓ (memcpy into local global pool)    │
│ Eye: read("g_MOOD", &mood)              │ ← NO MESH (local memcpy)
│    ↓ (detect changes, render frames)    │
│ Eye: render_eye_frame()                 │
└─────────────────────────────────────────┘
```

**Key Insight**: Components can call `read()` **as often as needed** (even every loop!) with ZERO network overhead. Only `write()` triggers mesh traffic.

### Use Cases

**Fast reads inside personality functions**:
```cpp
void goblin_personality_act(void) {
    Environment envir;
    read("g_Envir", &envir, sizeof(Environment));  // NO MESH - instant!
    
    Mood currentMood;
    read("g_MOOD", &currentMood, sizeof(Mood));    // NO MESH - instant!
    
    // Process mood based on environment...
}
```

**Eye detects mood changes for animation recalculation** (yesterday's implementation):
```cpp
void goblin_eye_left_act(void) {
    Mood currentMood;
    read("g_MOOD", &currentMood, sizeof(Mood));  // NO MESH - instant!
    
    if (currentMood != s_lastMood) {
        // Mood changed! Recalculate animation frames
        generate_blink_animation();
    }
}
```

## Core API

### Allocation (in init() functions only)

```c
esp_err_t alloc_shared(const char *name, size_t size);
```

**Example**:
```c
esp_err_t goblin_mood_init(void) {
    // Allocate space for Mood class (9 bytes)
    alloc_shared("g_MOOD", sizeof(Mood));
    
    // Allocate space for distance sensor
    alloc_shared("g_DISTANCE", sizeof(uint8_t));
    
    // Allocate space for boolean flag
    alloc_shared("g_SPEAKING", sizeof(bool));
    
    return ESP_OK;
}
```

### Read Access (in act() functions)

```c
esp_err_t read(const char *name, void *dest, size_t size);
```

**Parameters**:
- `name`: String identifier (convention: prefix with "g_")
- `dest`: Pointer to local variable to copy into
- `size`: Size of destination (MUST match allocated size)

**Returns**: `ESP_OK` on success, `ESP_ERR_INVALID_SIZE` if size mismatch, `ESP_ERR_NOT_FOUND` if name not found

**Example**:
```c
void goblin_eye_left_act(void) {
    // Define local copy
    Mood currentMood;
    
    // Read from global memory into local variable
    if (read("g_MOOD", &currentMood, sizeof(Mood)) == ESP_OK) {
        int8_t happiness = currentMood.components[MOOD_HAPPINESS];
        int8_t anger = currentMood.components[MOOD_ANGER];
        
        render_eye_expression(happiness, anger);
    }
    
    // Read distance sensor value
    uint8_t distance_cm;
    if (read("g_DISTANCE", &distance_cm, sizeof(uint8_t)) == ESP_OK) {
        if (distance_cm < 30) {
            dilate_pupils();  // React to close object
        }
    }
}
```

### Write Access (in act() functions)

```c
esp_err_t write(const char *name, void *src, size_t size);
```

**Parameters**:
- `name`: String identifier (convention: prefix with "g_")
- `src`: Pointer to local variable to copy from
- `size`: Size of source (MUST match allocated size)

**Returns**: `ESP_OK` on success, automatically triggers mesh broadcast

**Example**:
```c
void nose_sensor_act(void) {
    // Local variable for sensor reading
    uint8_t distance_cm = measure_distance();
    
    // Write to shared memory (triggers mesh broadcast)
    write("g_DISTANCE", &distance_cm, sizeof(uint8_t));
}
```

### Complete Pattern: Read-Modify-Write (with change detection)

```c
void goblin_mood_act(void) {
    // 1. Define local working copy
    Mood currentMood;
    
    // 2. Read from global memory
    read("g_MOOD", &currentMood, sizeof(Mood));
    
    // 3. Calculate new mood based on sensors
    Mood newMood = currentMood;
    
    uint8_t distance_cm;
    read("g_DISTANCE", &distance_cm, sizeof(uint8_t));
    
    if (distance_cm < 30) {
        newMood.addCuriosity(20);
        newMood.addExcitement(15);
    }
    
    // Apply natural decay
    if (g_loopCount % 50 == 0) {
        if (newMood.excitement() > 0) {
            newMood.addExcitement(-5);
        }
    }
    
    // 4. Only write if changed (avoids unnecessary mesh traffic)
    if (newMood != currentMood) {
        currentMood = newMood;
        
        // Write to global memory AND trigger mesh broadcast
        write("g_MOOD", &currentMood, sizeof(Mood));
    }
}
```

**Key Benefits**:
- ✅ Local working copy (fast access, no locking)
- ✅ Explicit sync points (only when calling read/write)
- ✅ Change detection (avoid unnecessary mesh traffic)
- ✅ Size-safe (sizeof() ensures correct byte count)
- ✅ Write triggers sync automatically (no polling)
    Mood currentMood;
    
    // 2. Read from global memory
    if (read("g_MOOD", &currentMood, sizeof(Mood)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read mood");
        return;
    }
    
    // 3. Modify local copy
    Mood newMood = currentMood;
    newMood.addCuriosity(10);
    newMood.addExcitement(5);
    
    // 4. Check if changed
    if (newMood != currentMood) {
        currentMood = newMood;
        
        // 5. Write back (triggers mesh broadcast)
        write("g_MOOD", &currentMood, sizeof(Mood));
    }
}
```

## Implementation

### Header File

```c
// include/p32_shared_memory.h

#ifndef P32_SHARED_MEMORY_H
#define P32_SHARED_MEMORY_H

#include <stdint.h>
#include <stddef.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

// Maximum number of named memory blocks
#define MAX_SHARED_BLOCKS 64

// Maximum name length
#define MAX_NAME_LENGTH 32

// Maximum total shared memory (64 KB)
#define MAX_SHARED_MEMORY_SIZE (64 * 1024)

/**
 * Allocate a named block of shared memory
 * 
 * ONLY call during component init() functions!
 * 
 * @param name String identifier (max 31 chars)
 * @param size Size in bytes
 * @return ESP_OK on success, ESP_ERR_NO_MEM if full
 */
esp_err_t alloc_shared(const char *name, size_t size);

/**
 * Read a named shared memory block into local variable
 * 
 * Copies data from global memory into destination buffer
 * Size MUST match allocated size (enforced at runtime)
 * 
 * Safe to call from any component act() function
 * 
 * @param name String identifier (convention: "g_NAME")
 * @param dest Pointer to destination buffer (local variable)
 * @param size Size of destination (must match allocated size)
 * @return ESP_OK on success
 *         ESP_ERR_NOT_FOUND if name not allocated
 *         ESP_ERR_INVALID_SIZE if size mismatch
 */
esp_err_t read(const char *name, void *dest, size_t size);

/**
 * Write local variable to named shared memory block
 * 
 * Copies data from source buffer into global memory
 * Size MUST match allocated size (enforced at runtime)
 * Automatically marks block as dirty for mesh broadcast
 * 
 * Safe to call from any component act() function
 * 
 * @param name String identifier (convention: "g_NAME")
 * @param src Pointer to source buffer (local variable)
 * @param size Size of source (must match allocated size)
 * @return ESP_OK on success
 *         ESP_ERR_NOT_FOUND if name not allocated
 *         ESP_ERR_INVALID_SIZE if size mismatch
 */
esp_err_t write(const char *name, void *src, size_t size);

/**
 * Get size of a named block
 * 
 * @param name String identifier
 * @return Size in bytes, or 0 if not found
 */
size_t get_shared_size(const char *name);

/**
 * Check if block has been modified since last sync
 * 
 * Used internally by mesh system
 * 
 * @param name String identifier
 * @return true if modified, false otherwise
 */
bool is_shared_dirty(const char *name);

/**
 * Mark block as clean (synced)
 * 
 * Used internally by mesh system after broadcast
 * 
 * @param name String identifier
 */
void mark_shared_clean(const char *name);

/**
 * Get list of all dirty blocks (for mesh broadcast)
 * 
 * Used internally by mesh system
 * 
 * @param dirty_list Output array of names
 * @param max_count Maximum entries in array
 * @return Number of dirty blocks found
 */
int get_dirty_blocks(const char **dirty_list, int max_count);

/**
 * Debug: Print all allocated blocks
 */
void dump_shared_memory(void);

#ifdef __cplusplus
}
#endif

#endif // P32_SHARED_MEMORY_H
```

### Implementation File

```c
// src/p32_shared_memory.c

#include "p32_shared_memory.h"
#include <string.h>
#include <esp_log.h>

static const char *TAG = "SHARED_MEM";

// Memory block descriptor
typedef struct {
    char name[MAX_NAME_LENGTH];
    void *data;
    size_t size;
    bool dirty;      // Modified since last mesh sync
    bool allocated;  // Block is in use
} shared_block_t;

// Global registry of shared blocks
static shared_block_t g_shared_blocks[MAX_SHARED_BLOCKS];
static int g_block_count = 0;

// Global memory pool (allocated from heap once)
static uint8_t *g_memory_pool = NULL;
static size_t g_memory_used = 0;

// Initialize memory pool on first allocation
static esp_err_t init_memory_pool(void) {
    if (g_memory_pool == NULL) {
        g_memory_pool = (uint8_t*)malloc(MAX_SHARED_MEMORY_SIZE);
        if (g_memory_pool == NULL) {
            ESP_LOGE(TAG, "Failed to allocate %d byte memory pool", 
                MAX_SHARED_MEMORY_SIZE);
            return ESP_ERR_NO_MEM;
        }
        ESP_LOGI(TAG, "Allocated %d byte shared memory pool", 
            MAX_SHARED_MEMORY_SIZE);
    }
    return ESP_OK;
}

esp_err_t alloc_shared(const char *name, size_t size) {
    if (name == NULL || size == 0) {
        ESP_LOGE(TAG, "Invalid parameters: name=%p, size=%d", name, size);
        return ESP_ERR_INVALID_ARG;
    }
    
    if (strlen(name) >= MAX_NAME_LENGTH) {
        ESP_LOGE(TAG, "Name too long: '%s' (max %d chars)", 
            name, MAX_NAME_LENGTH - 1);
        return ESP_ERR_INVALID_ARG;
    }
    
    // Check if name already exists
    for (int i = 0; i < g_block_count; i++) {
        if (g_shared_blocks[i].allocated && 
            strcmp(g_shared_blocks[i].name, name) == 0) {
            ESP_LOGW(TAG, "Block '%s' already allocated (%d bytes)", 
                name, g_shared_blocks[i].size);
            return ESP_OK;  // Not an error, just ignore
        }
    }
    
    // Check capacity
    if (g_block_count >= MAX_SHARED_BLOCKS) {
        ESP_LOGE(TAG, "Too many blocks (max %d)", MAX_SHARED_BLOCKS);
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize memory pool if needed
    esp_err_t ret = init_memory_pool();
    if (ret != ESP_OK) {
        return ret;
    }
    
    // Check memory availability
    if (g_memory_used + size > MAX_SHARED_MEMORY_SIZE) {
        ESP_LOGE(TAG, "Out of memory: need %d bytes, have %d remaining",
            size, MAX_SHARED_MEMORY_SIZE - g_memory_used);
        return ESP_ERR_NO_MEM;
    }
    
    // Allocate block
    shared_block_t *block = &g_shared_blocks[g_block_count];
    strncpy(block->name, name, MAX_NAME_LENGTH - 1);
    block->name[MAX_NAME_LENGTH - 1] = '\0';
    block->data = g_memory_pool + g_memory_used;
    block->size = size;
    block->dirty = false;
    block->allocated = true;
    
    // Zero-initialize memory
    memset(block->data, 0, size);
    
    g_memory_used += size;
    g_block_count++;
    
    ESP_LOGI(TAG, "Allocated '%s': %d bytes at offset %d (total: %d/%d bytes)",
        name, size, g_memory_used - size, g_memory_used, MAX_SHARED_MEMORY_SIZE);
    
    return ESP_OK;
}

esp_err_t read(const char *name, void *dest, size_t size) {
    if (name == NULL || dest == NULL || size == 0) {
        ESP_LOGE(TAG, "Invalid parameters: name=%p, dest=%p, size=%d", 
            name, dest, size);
        return ESP_ERR_INVALID_ARG;
    }
    
    for (int i = 0; i < g_block_count; i++) {
        if (g_shared_blocks[i].allocated && 
            strcmp(g_shared_blocks[i].name, name) == 0) {
            
            // Verify size matches
            if (g_shared_blocks[i].size != size) {
                ESP_LOGE(TAG, "Size mismatch for '%s': expected %d, got %d",
                    name, g_shared_blocks[i].size, size);
                return ESP_ERR_INVALID_SIZE;
            }
            
            // Copy from global memory to local variable
            memcpy(dest, g_shared_blocks[i].data, size);
            
            ESP_LOGD(TAG, "Read %d bytes from '%s'", size, name);
            
            return ESP_OK;
        }
    }
    
    ESP_LOGW(TAG, "Block not found: '%s'", name);
    return ESP_ERR_NOT_FOUND;
}

esp_err_t write(const char *name, void *src, size_t size) {
    if (name == NULL || src == NULL || size == 0) {
        ESP_LOGE(TAG, "Invalid parameters: name=%p, src=%p, size=%d", 
            name, src, size);
        return ESP_ERR_INVALID_ARG;
    }
    
    for (int i = 0; i < g_block_count; i++) {
        if (g_shared_blocks[i].allocated && 
            strcmp(g_shared_blocks[i].name, name) == 0) {
            
            // Verify size matches
            if (g_shared_blocks[i].size != size) {
                ESP_LOGE(TAG, "Size mismatch for '%s': expected %d, got %d",
                    name, g_shared_blocks[i].size, size);
                return ESP_ERR_INVALID_SIZE;
            }
            
            // Copy from local variable into global memory
            memcpy(g_shared_blocks[i].data, src, size);
            
            // Mark as dirty for mesh broadcast
            g_shared_blocks[i].dirty = true;
            
            ESP_LOGD(TAG, "Wrote %d bytes to '%s' (marked dirty)", size, name);
            
            return ESP_OK;
        }
    }
    
    ESP_LOGW(TAG, "Block not found for write: '%s'", name);
    return ESP_ERR_NOT_FOUND;
}

size_t get_shared_size(const char *name) {
    if (name == NULL) {
        return 0;
    }
    
    for (int i = 0; i < g_block_count; i++) {
        if (g_shared_blocks[i].allocated && 
            strcmp(g_shared_blocks[i].name, name) == 0) {
            return g_shared_blocks[i].size;
        }
    }
    
    return 0;
}

bool is_shared_dirty(const char *name) {
    if (name == NULL) {
        return false;
    }
    
    for (int i = 0; i < g_block_count; i++) {
        if (g_shared_blocks[i].allocated && 
            strcmp(g_shared_blocks[i].name, name) == 0) {
            return g_shared_blocks[i].dirty;
        }
    }
    
    return false;
}

void mark_shared_clean(const char *name) {
    if (name == NULL) {
        return;
    }
    
    for (int i = 0; i < g_block_count; i++) {
        if (g_shared_blocks[i].allocated && 
            strcmp(g_shared_blocks[i].name, name) == 0) {
            g_shared_blocks[i].dirty = false;
            return;
        }
    }
}

int get_dirty_blocks(const char **dirty_list, int max_count) {
    int count = 0;
    
    for (int i = 0; i < g_block_count && count < max_count; i++) {
        if (g_shared_blocks[i].allocated && g_shared_blocks[i].dirty) {
            dirty_list[count++] = g_shared_blocks[i].name;
        }
    }
    
    return count;
}

void dump_shared_memory(void) {
    ESP_LOGI(TAG, "=== Shared Memory Dump ===");
    ESP_LOGI(TAG, "Blocks: %d / %d", g_block_count, MAX_SHARED_BLOCKS);
    ESP_LOGI(TAG, "Memory: %d / %d bytes (%.1f%%)",
        g_memory_used, MAX_SHARED_MEMORY_SIZE,
        (float)g_memory_used * 100.0f / MAX_SHARED_MEMORY_SIZE);
    
    for (int i = 0; i < g_block_count; i++) {
        if (g_shared_blocks[i].allocated) {
            ESP_LOGI(TAG, "  [%2d] %-20s  %4d bytes  %s",
                i,
                g_shared_blocks[i].name,
                g_shared_blocks[i].size,
                g_shared_blocks[i].dirty ? "DIRTY" : "clean");
        }
    }
    
    ESP_LOGI(TAG, "========================");
}
```

## Usage Examples

## Standard Global Class Variables

### Recommended Naming Convention

Use `g_` prefix for all global shared classes:

```cpp
// Core robot state
alloc_shared("g_MOOD", sizeof(Mood));           // Emotional state
alloc_shared("g_Envir", sizeof(Environment));    // Sensor readings

// Behavioral planning
alloc_shared("g_MOOD_TARGET", sizeof(Mood));    // Target mood for transitions
alloc_shared("g_BEHAVIOR", sizeof(Behavior));   // Current behavior state

// System status
alloc_shared("g_BATTERY", sizeof(BatteryStatus));
alloc_shared("g_SYSTEM", sizeof(SystemStatus));
```

### Environment Class Definition

```cpp
// include/Environment.hpp

#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <stdint.h>

class Environment {
public:
    // Temperature & humidity
    int16_t temperature_c;      // Celsius × 10 (e.g., 235 = 23.5°C)
    uint8_t humidity_percent;   // 0-100%
    
    // Ultrasonic sensor (nose)
    uint8_t distance_cm;        // 0-255 cm (HC-SR04 range)
    bool target_detected;       // Object within detection range
    
    // Target face tracking (future: OpenCV on Pi)
    int16_t target_x;           // Horizontal angle (-90° to +90°)
    int16_t target_y;           // Vertical angle (-45° to +45°)
    float target_confidence;    // 0.0 to 1.0 (face detection confidence)
    bool face_detected;         // Face tracking active
    
    // Light level (future: photoresistor)
    uint16_t ambient_light;     // 0-1023 (ADC reading)
    
    // Sound level (future: microphone)
    uint8_t sound_level_db;     // 0-120 dB
    
    // Battery subsystem
    uint8_t battery_percent;    // 0-100% state of charge
    uint16_t battery_voltage_mv; // Millivolts (e.g., 7400 = 7.4V)
    int16_t battery_current_ma; // Milliamps (positive = discharging, negative = charging)
    uint16_t battery_power_mw;  // Milliwatts (calculated: voltage × current)
    bool battery_charging;      // Charging active
    bool battery_low;           // <20% charge (warning state)
    bool battery_critical;      // <10% charge (critical state)
    uint16_t battery_time_remaining_min; // Minutes until empty (0 = unknown)
    
    // Constructor
    Environment() : 
        temperature_c(200),      // Default 20.0°C
        humidity_percent(50),
        distance_cm(255),        // Max distance (no detection)
        target_detected(false),
        target_x(0),
        target_y(0),
        target_confidence(0.0f),
        face_detected(false),
        ambient_light(512),      // Mid-range
        sound_level_db(40),      // Quiet room
        battery_percent(100),    // Fully charged
        battery_voltage_mv(8400), // 2S LiPo full charge (4.2V × 2)
        battery_current_ma(0),   // No load
        battery_power_mw(0),
        battery_charging(false),
        battery_low(false),
        battery_critical(false),
        battery_time_remaining_min(0)
    {}
    
    // Comparison operator (for change detection)
    bool operator!=(const Environment& other) const {
        return temperature_c != other.temperature_c ||
               humidity_percent != other.humidity_percent ||
               distance_cm != other.distance_cm ||
               target_detected != other.target_detected ||
               target_x != other.target_x ||
               target_y != other.target_y ||
               face_detected != other.face_detected ||
               battery_percent != other.battery_percent ||
               battery_voltage_mv != other.battery_voltage_mv ||
               battery_current_ma != other.battery_current_ma ||
               battery_low != other.battery_low ||
               battery_critical != other.battery_critical;
    }
};

#endif // ENVIRONMENT_HPP
```

### Example 1: Sensor Component (Write-Only Producer)

```c
// src/components/nose_sensor.c

#include "p32_shared_memory.h"
#include "Environment.hpp"

esp_err_t nose_sensor_init(void) {
    // Allocate space for environment data
    alloc_shared("g_Envir", sizeof(Environment));
    
    // Initialize HC-SR04 hardware
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
    
    return ESP_OK;
}

void nose_sensor_act(void) {
    // Read current environment from global memory
    Environment currentEnvir;
    read("g_Envir", &currentEnvir, sizeof(Environment));
    
    // Measure distance with HC-SR04
    uint8_t distance_cm = measure_ultrasonic_distance();
    
    // Update local copy
    Environment newEnvir = currentEnvir;
    newEnvir.distance_cm = distance_cm;
    newEnvir.target_detected = (distance_cm < 100);  // Within 1 meter
    
    // Only write if changed (avoids unnecessary mesh traffic)
    if (newEnvir != currentEnvir) {
        write("g_Envir", &newEnvir, sizeof(Environment));
        ESP_LOGD(TAG, "Distance: %d cm (broadcast)", distance_cm);
    }
}
```

### Example 2: Personality Component (Read Multiple Classes)

```cpp
// src/components/goblin_personality.cpp

#include "p32_shared_memory.h"
#include "Mood.hpp"
#include "Environment.hpp"

esp_err_t goblin_personality_init(void) {
    // Personality reads from g_Envir, writes to g_MOOD
    // (Allocations done by sensor and mood components)
    return ESP_OK;
}

void goblin_personality_act(void) {
    // Read environment (NO MESH NETWORK - just memcpy from global pool)
    Environment currentEnvir;
    read("g_Envir", &currentEnvir, sizeof(Environment));
    
    // Read current mood (NO MESH NETWORK - just memcpy)
    Mood currentMood;
    read("g_MOOD", &currentMood, sizeof(Mood));
    
    // Calculate new mood based on environment
    Mood newMood = currentMood;
    
    // React to close objects (curiosity + excitement)
    if (currentEnvir.target_detected && currentEnvir.distance_cm < 30) {
        newMood.addCuriosity(10);
        newMood.addExcitement(15);
        ESP_LOGI(TAG, "Object close! Curious + Excited");
    }
    
    // React to face detection (happiness + affection)
    if (currentEnvir.face_detected) {
        newMood.addHappiness(20);
        newMood.addAffection(10);
        ESP_LOGI(TAG, "Face detected! Happy + Affectionate");
    }
    
    // React to temperature (contentment or irritation)
    if (currentEnvir.temperature_c > 280) {  // > 28°C = hot
        newMood.addIrritation(5);
        newMood.addContentment(-5);
    } else if (currentEnvir.temperature_c < 180) {  // < 18°C = cold
        newMood.addIrritation(3);
    }
    
    // Apply natural mood decay
    if (g_loopCount % 50 == 0) {  // Every 5 seconds
        if (newMood.excitement() > 0) newMood.addExcitement(-5);
        if (newMood.curiosity() > 0) newMood.addCuriosity(-3);
    }
    
    // Only write if mood changed (triggers mesh broadcast)
    if (newMood != currentMood) {
        write("g_MOOD", &newMood, sizeof(Mood));
        ESP_LOGD(TAG, "Mood updated: HAPPINESS=%d, CURIOSITY=%d",
            newMood.happiness(), newMood.curiosity());
    }
}
```

### Example 3: Eye Component (Read-Only Consumer) (C++ Class)

```cpp
// src/components/goblin_mood.cpp

#include "p32_shared_memory.h"
#include "Mood.hpp"

esp_err_t goblin_mood_init(void) {
    // Allocate space for Mood class
    alloc_shared("g_MOOD", sizeof(Mood));
    
    // Initialize local mood
    Mood initialMood;
    initialMood.happiness() = 50;
    initialMood.curiosity() = 30;
    initialMood.contentment() = 40;
    
    // Write initial mood to global memory
    write("g_MOOD", &initialMood, sizeof(Mood));
    
    return ESP_OK;
}

void goblin_mood_act(void) {
    // 1. Read current mood from global memory
    Mood currentMood;
    if (read("g_MOOD", &currentMood, sizeof(Mood)) != ESP_OK) {
        return;
    }
    
    // 2. Read sensor data
    uint8_t distance_cm;
    if (read("g_DISTANCE", &distance_cm, sizeof(uint8_t)) == ESP_OK) {
        if (distance_cm < 30) {
            currentMood.addCuriosity(10);
        }
    }
    
    // 3. Apply mood decay
    if (g_loopCount % 50 == 0) {
        if (currentMood.excitement() > 0) {
            currentMood.addExcitement(-5);
        }
    }
    
    // 4. Write updated mood (only if changed)
    Mood newMood = currentMood;
    if (newMood != currentMood) {  // Assumes Mood has operator!=
        write("g_MOOD", &newMood, sizeof(Mood));
    }
}
```

### Example 2: Display Rendering (Read-Only Consumer)

```c
// src/components/goblin_eye_left.c

#include "p32_shared_memory.h"
#include "Mood.hpp"

esp_err_t goblin_eye_left_init(void) {
    // No allocation needed - just reads mood from torso
    // Initialize GC9A01 display hardware
    // ...
    return ESP_OK;
}

void goblin_eye_left_act(void) {
    // Read mood from global memory (synchronized via mesh)
    Mood currentMood;
    if (read("g_MOOD", &currentMood, sizeof(Mood)) != ESP_OK) {
        ESP_LOGW(TAG, "Mood not available yet");
        return;
    }
    
    // Extract mood components
    int8_t happiness = currentMood.components[MOOD_HAPPINESS];
    int8_t anger = currentMood.components[MOOD_ANGER];
    int8_t curiosity = currentMood.components[MOOD_CURIOSITY];
    
    // Render eye expression based on mood
    uint16_t eye_color;
    if (happiness > 50) {
        eye_color = 0x07E0;  // Green (happy)
    } else if (anger > 50) {
        eye_color = 0xF800;  // Red (angry)
    } else if (curiosity > 50) {
        eye_color = 0xFFE0;  // Yellow (curious)
    } else {
        eye_color = 0x001F;  // Blue (neutral)
    }
    
    render_eye(eye_color);
}
```

### Example 4: Eye Component with Change Detection (Yesterday's Implementation)

```cpp
// src/components/goblin_eye_left.cpp

#include "p32_shared_memory.h"
#include "Mood.hpp"

// Local state for change detection
static Mood s_lastMood;
static bool s_animation_dirty = true;

esp_err_t goblin_eye_left_init(void) {
    // Initialize GC9A01 display
    spi_bus_add_device(SPI2_HOST, &dev_config, &s_spi_display);
    
    // Force first animation calculation
    s_lastMood.happiness() = -128;  // Impossible value
    
    return ESP_OK;
}

void goblin_eye_left_act(void) {
    // Read mood from shared memory (NO MESH - just memcpy from global pool!)
    Mood currentMood;
    read("g_MOOD", &currentMood, sizeof(Mood));
    
    // Detect if mood has changed since last frame
    if (currentMood != s_lastMood) {
        s_lastMood = currentMood;
        s_animation_dirty = true;
        
        ESP_LOGD(TAG, "Mood changed! Recalculating animation frames");
    }
    
    // Recalculate animation frames ONLY when mood changes
    if (s_animation_dirty) {
        int8_t happiness = currentMood.happiness();
        int8_t anger = currentMood.anger();
        int8_t curiosity = currentMood.curiosity();
        
        // Calculate eye properties based on dominant mood
        uint16_t eye_color;
        uint8_t pupil_size;
        
        if (happiness > 50) {
            eye_color = 0x07E0;      // Green (happy)
            pupil_size = 80;         // Normal pupils
        } else if (anger > 50) {
            eye_color = 0xF800;      // Red (angry)
            pupil_size = 60;         // Constricted (angry stare)
        } else if (curiosity > 50) {
            eye_color = 0xFFE0;      // Yellow (curious)
            pupil_size = 100;        // Dilated (wide-eyed)
        } else {
            eye_color = 0x001F;      // Blue (neutral)
            pupil_size = 70;         // Normal
        }
        
        // Regenerate animation frames (expensive operation!)
        generate_blink_animation(eye_color, pupil_size);
        generate_look_around_animation(eye_color, pupil_size);
        
        s_animation_dirty = false;
    }
    
    // Render current animation frame (fast - just SPI transfer)
    render_eye_frame();
}
```

**Key Insight**: Eye component uses **local static state** to detect mood changes. Only recalculates expensive animation frames when mood actually changes, not every loop!

**Data Flow**:
1. **Sensor** (nose) updates `g_Envir` → triggers mesh broadcast
2. **Personality** reads `g_Envir` (NO MESH - local memcpy), updates `g_MOOD` → triggers mesh broadcast  
3. **Eye** reads `g_MOOD` (NO MESH - local memcpy), detects change, recalculates animation

**Performance**: `read()` is just `memcpy()` - **NO network latency**! Mesh only happens on `write()`.

### Example 5: Sensor Data (Write-Only Producer)

```c
// src/components/nose_sensor.c

#include "p32_shared_memory.h"

esp_err_t nose_sensor_init(void) {
    // Allocate space for distance reading
    alloc_shared("g_DISTANCE", sizeof(uint8_t));
    
    // Initialize HC-SR04 hardware
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
    
    return ESP_OK;
}

void nose_sensor_act(void) {
    // Measure distance with HC-SR04
    uint8_t distance_cm = measure_ultrasonic_distance();
    
    // Write to shared memory (triggers mesh broadcast)
    write("g_DISTANCE", &distance_cm, sizeof(uint8_t));
    
    ESP_LOGD(TAG, "Distance: %d cm (broadcast)", distance_cm);
}
```

## Mesh Broadcast Integration

### ESP-NOW Mesh Component

```c
// src/components/esp_now_mesh.c

#include "p32_shared_memory.h"
#include "esp_now.h"

void esp_now_mesh_act(void) {
    // hitCount: 1 → runs every loop (100ms)
    
    // Get list of dirty blocks
    const char *dirty_blocks[MAX_SHARED_BLOCKS];
    int dirty_count = get_dirty_blocks(dirty_blocks, MAX_SHARED_BLOCKS);
    
    if (dirty_count > 0) {
        ESP_LOGD(TAG, "Broadcasting %d dirty blocks", dirty_count);
        
        // For each dirty block, broadcast via ESP-NOW
        for (int i = 0; i < dirty_count; i++) {
            const char *name = dirty_blocks[i];
            void *data = read(name);
            size_t size = get_shared_size(name);
            
            // Create mesh packet
            typedef struct {
                char name[32];
                uint8_t data[256];  // Max block size
                size_t size;
            } mesh_packet_t;
            
            mesh_packet_t packet;
            strncpy(packet.name, name, 31);
            packet.name[31] = '\0';
            memcpy(packet.data, data, size);
            packet.size = size;
            
            // Broadcast to all mesh nodes
            esp_now_send(NULL, (uint8_t*)&packet, sizeof(mesh_packet_t));
            
            // Mark as clean (synced)
            mark_shared_clean(name);
        }
    }
}

void on_mesh_packet_received(const uint8_t *mac, const uint8_t *data, int len) {
    typedef struct {
        char name[32];
        uint8_t data[256];
        size_t size;
    } mesh_packet_t;
    
    mesh_packet_t *packet = (mesh_packet_t*)data;
    
    ESP_LOGD(TAG, "Received update for '%s' (%d bytes)", 
        packet->name, packet->size);
    
    // Update local shared memory directly (bypass write to avoid re-broadcast)
    // This is internal mesh system code - components never call this
    for (int i = 0; i < g_block_count; i++) {
        if (g_shared_blocks[i].allocated && 
            strcmp(g_shared_blocks[i].name, packet->name) == 0) {
            
            if (g_shared_blocks[i].size == packet->size) {
                memcpy(g_shared_blocks[i].data, packet->data, packet->size);
                // Don't mark dirty - this is an incoming sync
                ESP_LOGD(TAG, "Synced '%s' from mesh", packet->name);
            }
            break;
        }
    }
}
```

## Key Design Benefits

### 1. **Local Working Copies = Fast**
Components work with local class instances (Mood, BehaviorState, etc.) with zero overhead. No locking, no shared memory access during computation.

### 2. **Explicit Sync Points = Predictable**
Only synchronize when calling `read()` or `write()`. No hidden polling, no surprise latency.

### 3. **Change Detection = Efficient**
```cpp
if (newMood != currentMood) {  // Only sync if changed
    write("g_MOOD", &newMood, sizeof(Mood));
}
```
Avoids unnecessary mesh traffic when values haven't changed.

### 4. **Size-Aware = Safe**
```cpp
read("g_MOOD", &currentMood, sizeof(Mood));  // Compile-time size check
```
Using `sizeof()` ensures correct byte counts, catches mismatches.

### 5. **Open-Ended = Flexible**
Can allocate **any size** blocks:
- Small: `alloc_shared("g_DISTANCE", 1);`  // 1 byte
- Medium: `alloc_shared("g_MOOD", sizeof(Mood));`  // ~10 bytes
- Large: `alloc_shared("g_CAMERA_FRAME", 64000);`  // 64 KB

**Could allocate 100 KB buffers** - but ESP-NOW has limits! See Performance section below.

### 6. **Multiple Instances = Powerful**
```cpp
alloc_shared("g_MOOD", sizeof(Mood));
alloc_shared("g_Envir", sizeof(Environment));  // Sensor data
alloc_shared("g_MOOD_TARGET", sizeof(Mood));   // Behavior planning
alloc_shared("g_MOOD_HISTORY", sizeof(Mood) * 10);  // Array of 10 moods
```
Multiple global classes for different purposes, completely open-ended.

## Performance Characteristics

### Memory Usage

- **Overhead per block**: 40 bytes (name + metadata)
- **Maximum blocks**: 64 (configurable)
- **Maximum total memory**: 64 KB default (configurable up to 100 KB+)
- **Typical usage**: 20-30 blocks, ~5-10 KB
- **Large buffer support**: Yes! Can allocate 64 KB camera frames, audio buffers, etc.

### Latency

- **Local read/write**: <1 μs (memcpy)
- **Component computation**: 0 ms (works on local copy)
- **Local write**: <10 μs (memcpy + dirty flag)
- **Mesh broadcast**: 1-5 ms per packet (ESP-NOW transmission)
- **End-to-end sync**: <10 ms typical (write → queue → transmit → receive)

### ESP-NOW Network Limits ⚠️

**CRITICAL**: ESP-NOW is **NOT Bluetooth** - It's Espressif's proprietary protocol on 2.4 GHz WiFi

**Hard Limits**:
- ✅ **Max packet size**: 250 bytes (protocol limit)
- ⚠️ **Max throughput**: ~50-100 KB/s practical (250 KB/s theoretical)
- ✅ **Max peers**: 20 devices (10 encrypted + 10 unencrypted)
- ✅ **Latency**: 1-5 ms (MUCH faster than Bluetooth's 100+ ms)
- ✅ **Range**: 100-200m line-of-sight, 20-50m indoor

**Comparison**:
| Protocol | Throughput | Latency | Range | Setup Time |
|----------|-----------|---------|-------|------------|
| **ESP-NOW** | **50-100 KB/s** | **1-5 ms** | **100-200m** | **<1 ms** |
| Bluetooth Classic | 1-3 MB/s | 100+ ms | 10-30m | 5-10 sec |
| BLE 5.0 | 125 KB/s | 10-50 ms | 50-100m | 1-2 sec |

**Why ESP-NOW for P32**:
- ✅ Ultra-low latency (critical for real-time facial expressions)
- ✅ No pairing required (instant mesh coordination)
- ✅ Long range (entire robot + 50m radius)
- ✅ Low power consumption
- ⚠️ Limited throughput (must manage bandwidth)

### Bandwidth Analysis

**Throughput Calculations**:
```
Best case (1 small block/loop):
- 1 packet × 50 bytes × 10 Hz = 0.5 KB/s ✅ No problem

Typical case (5-10 blocks/loop):
- 5 packets × 100 bytes × 10 Hz = 5 KB/s ✅ Plenty of headroom

Heavy case (20 blocks/loop):
- 20 packets × 100 bytes × 10 Hz = 20 KB/s ✅ Still OK

Extreme case (50 blocks × 200 bytes):
- 50 packets × 200 bytes × 10 Hz = 100 KB/s ⚠️ AT LIMIT

DANGER (100 KB buffer every loop):
- 400 packets × 250 bytes × 10 Hz = 1,000 KB/s ❌ IMPOSSIBLE
```

**Real-World P32 Usage** (Estimated):
| Block | Size | Write Freq | Bandwidth |
|-------|------|------------|-----------|
| `g_MOOD` | 9 bytes | 1 Hz | 9 B/s |
| `g_DISTANCE` | 1 byte | 10 Hz | 10 B/s |
| `g_TEMPERATURE` | 2 bytes | 0.1 Hz | 0.2 B/s |
| `g_BATTERY` | 1 byte | 0.2 Hz | 0.2 B/s |
| `g_SERVO_POS` | 28 bytes | 5 Hz | 140 B/s |
| **TOTAL** | **41 B** | - | **~160 B/s** |

**Headroom**: 160 B/s ÷ 50,000 B/s = **0.3% of capacity** ✅

**100 KB Buffer Scenarios**:
```c
// ❌ BAD: Update entire 100 KB buffer every loop
alloc_shared("g_HUGE_BUFFER", 100 * 1024);
write("g_HUGE_BUFFER", &buf, 100*1024);  // Every 100ms
// = 100 KB × 10 Hz = 1,000 KB/s ❌ 20× over capacity!

// ✅ GOOD: Update rarely (once per 10 seconds)
write("g_HUGE_BUFFER", &buf, 100*1024);  // Every 10 sec
// = 100 KB × 0.1 Hz = 10 KB/s ✅ Acceptable

// ✅ BETTER: Split into chunks, update 1 chunk per loop
alloc_shared("g_CHUNK_0", 10*1024);
alloc_shared("g_CHUNK_1", 10*1024);
// ... 10 chunks total
write("g_CHUNK_N", &chunk, 10*1024);  // 1 chunk every 100ms
// = 10 KB × 10 Hz = 100 KB/s ⚠️ At limit but works

// ✅ BEST: Use different protocol for bulk data
// ESP-NOW: Real-time control (<10 KB/s)
// WiFi TCP: Bulk transfers (1+ MB/s)
```

**Design Guidelines**:
1. ✅ **Keep blocks small** (<200 bytes each)
2. ✅ **Minimize write frequency** (use comparison checks!)
3. ✅ **Batch related data** (single "mood" block vs 9 separate)
4. ✅ **Prioritize critical data** (facial expressions > debug logs)
5. ⚠️ **Test limits experimentally** (every robot is different)

## Workflow Summary

**The key insight**: No polling! Sync only happens when component calls `write()`.

```cpp
// Component workflow:
void component_act(void) {
    // 1. Define local variable
    Mood currentMood;
    
    // 2. Read from global memory → local copy
    read("g_MOOD", &currentMood, sizeof(Mood));
    
    // 3. Work with local copy (fast, no overhead)
    Mood newMood = currentMood;
    newMood.addCuriosity(20);
    
    // 4. Only write if changed (explicit sync point)
    if (newMood != currentMood) {
        currentMood = newMood;
        write("g_MOOD", &currentMood, sizeof(Mood));  // ← Triggers mesh sync
    }
}
```

**No memory polling** - mesh system only broadcasts blocks marked dirty by `write()` calls.

## Best Practices

### DO

✅ Allocate all shared memory in `init()` functions
✅ Use descriptive names with `g_` prefix ("g_MOOD", "g_DISTANCE")
✅ Define local working copies in `act()` functions
✅ Use `sizeof()` for size parameter (compile-time safety)
✅ Compare before writing to avoid unnecessary mesh traffic
✅ Experiment with large buffers (64-100 KB) to find system limits

### DON'T

❌ Allocate during `act()` functions
❌ Use generic names ("data1", "temp")
❌ Work directly on global memory (defeats local copy pattern)
❌ Write every loop if value hasn't changed (mesh traffic!)
❌ Store pointers in shared memory (invalid across nodes!)

## Debugging

### Check Allocations

```c
void system_core_init(void) {
    // ... after all components init ...
    
    dump_shared_memory();
    
    // Output:
    // === Shared Memory Dump ===
    // Blocks: 8 / 64
    // Memory: 127 / 65536 bytes (0.2%)
    //   [ 0] mood                   9 bytes  clean
    //   [ 1] distance_cm            1 bytes  DIRTY
    //   [ 2] temperature_c          2 bytes  clean
    //   [ 3] is_speaking            1 bytes  clean
    //   ...
}
```

### Monitor Dirty Blocks

```c
void esp_now_mesh_act(void) {
    const char *dirty_blocks[MAX_SHARED_BLOCKS];
    int dirty_count = get_dirty_blocks(dirty_blocks, MAX_SHARED_BLOCKS);
    
    if (dirty_count > 5) {
        ESP_LOGW(TAG, "High update rate: %d blocks dirty", dirty_count);
    }
}
```

## Summary

✅ **Simple API**: Just `alloc_shared()`, `read()`, `write()`
✅ **String-based**: No manual pointer management
✅ **Automatic sync**: Mesh broadcasts dirty blocks every loop
✅ **Type-safe**: Cast pointers to appropriate types
✅ **Fast**: Direct memory access, no copies on read
✅ **Efficient**: Only dirty blocks broadcast, not entire state

**Next**: Implement and test with hardware validation tests!

📘 **Related Documents**:

- [Mesh State Synchronization](MESH-STATE-SYNCHRONIZATION-SPEC.md)
- [Component Function Signatures](COMPONENT-FUNCTION-SIGNATURES.md)
- [Hardware Validation Test Plan](HARDWARE-VALIDATION-TEST-PLAN.md)
