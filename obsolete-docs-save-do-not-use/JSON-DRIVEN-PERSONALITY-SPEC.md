# JSON-Driven Personality System

## Design Philosophy

**Personality module is entirely data-driven** - no hardcoded behavior logic!

- **JSON defines personality traits** - how strongly creature reacts to stimuli
- **C++ code is generic** - reads JSON config, applies formulas
- **Same code works for all creatures** - only JSON changes
- **Artists can tweak personalities** - no programmer required

## Architecture

### Personality JSON Configuration

```json
// config/personalities/goblin_mischievous.json
{
    "relative_filename": "config/personalities/goblin_mischievous.json",
    "version": "1.0.0",
    "author": "config/author.json",
    
    "personality_name": "Mischievous Goblin",
    "personality_id": "GOBLIN_MISCHIEVOUS",
    
    "description": "Curious, excitable, easily irritated. Loves attention and close interaction.",
    
    "mood_reactions": {
        "close_object": {
            "distance_threshold_cm": 30,
            "triggers": [
                {"mood": "CURIOSITY", "delta": 10},
                {"mood": "EXCITEMENT", "delta": 15},
                {"mood": "HAPPINESS", "delta": 5}
            ]
        },
        
        "face_detected": {
            "confidence_threshold": 0.7,
            "triggers": [
                {"mood": "HAPPINESS", "delta": 20},
                {"mood": "AFFECTION", "delta": 10},
                {"mood": "EXCITEMENT", "delta": 15}
            ]
        },
        
        "hot_temperature": {
            "temperature_threshold_c": 28,
            "triggers": [
                {"mood": "IRRITATION", "delta": 5},
                {"mood": "CONTENTMENT", "delta": -5}
            ]
        },
        
        "cold_temperature": {
            "temperature_threshold_c": 18,
            "triggers": [
                {"mood": "IRRITATION", "delta": 3},
                {"mood": "CONTENTMENT", "delta": -3}
            ]
        },
        
        "loud_sound": {
            "sound_threshold_db": 70,
            "triggers": [
                {"mood": "FEAR", "delta": 10},
                {"mood": "IRRITATION", "delta": 5}
            ]
        },
        
        "no_interaction": {
            "timeout_seconds": 60,
            "triggers": [
                {"mood": "SADNESS", "delta": 5},
                {"mood": "EXCITEMENT", "delta": -10}
            ]
        },
        
        "battery_low": {
            "battery_threshold_percent": 20,
            "triggers": [
                {"mood": "SADNESS", "delta": 10},
                {"mood": "IRRITATION", "delta": 5},
                {"mood": "EXCITEMENT", "delta": -10}
            ]
        },
        
        "battery_critical": {
            "battery_threshold_percent": 10,
            "triggers": [
                {"mood": "FEAR", "delta": 15},
                {"mood": "SADNESS", "delta": 15},
                {"mood": "IRRITATION", "delta": 10}
            ]
        }
    },
    
    "mood_decay": {
        "decay_interval_seconds": 5,
        "decay_rates": [
            {"mood": "EXCITEMENT", "delta": -5},
            {"mood": "CURIOSITY", "delta": -3},
            {"mood": "FEAR", "delta": -7},
            {"mood": "ANGER", "delta": -4},
            {"mood": "IRRITATION", "delta": -2}
        ]
    },
    
    "default_mood": {
        "ANGER": 0,
        "FEAR": 0,
        "HAPPINESS": 50,
        "SADNESS": 0,
        "CURIOSITY": 30,
        "AFFECTION": 20,
        "IRRITATION": 0,
        "CONTENTMENT": 40,
        "EXCITEMENT": 10
    }
}
```

### Contrasting Personalities

**Goblin** (Mischievous):
```json
"close_object": {
    "triggers": [
        {"mood": "CURIOSITY", "delta": 10},    // Very curious
        {"mood": "EXCITEMENT", "delta": 15}    // Gets excited
    ]
}
```

**Bear** (Gentle):
```json
"close_object": {
    "triggers": [
        {"mood": "CURIOSITY", "delta": 3},     // Mildly curious
        {"mood": "CONTENTMENT", "delta": 5}    // Calm
    ]
}
```

**Cat** (Aloof):
```json
"close_object": {
    "triggers": [
        {"mood": "IRRITATION", "delta": 8},    // Annoyed by intrusion
        {"mood": "CURIOSITY", "delta": 2}      // Barely curious
    ]
}
```

**Cyclops** (Aggressive):
```json
"close_object": {
    "triggers": [
        {"mood": "ANGER", "delta": 15},        // Gets angry
        {"mood": "FEAR", "delta": -5}          // Less fearful
    ]
}
```

## Generic Personality Component (C++)

```cpp
// src/components/personality_generic.cpp

#include "p32_shared_memory.h"
#include "Mood.hpp"
#include "Environment.hpp"
#include <cJSON.h>

// Personality configuration (loaded from JSON)
static cJSON *s_personality_config = NULL;
static uint32_t s_last_interaction_time = 0;

esp_err_t personality_generic_init(void) {
    // Load personality JSON from filesystem
    const char *personality_file = get_bot_personality_path();  // From bot config
    
    char *json_string = load_file_to_string(personality_file);
    if (json_string == NULL) {
        ESP_LOGE(TAG, "Failed to load personality: %s", personality_file);
        return ESP_ERR_NOT_FOUND;
    }
    
    s_personality_config = cJSON_Parse(json_string);
    free(json_string);
    
    if (s_personality_config == NULL) {
        ESP_LOGE(TAG, "Failed to parse personality JSON");
        return ESP_ERR_INVALID_ARG;
    }
    
    // Initialize mood with default values from JSON
    Mood initialMood;
    cJSON *default_mood = cJSON_GetObjectItem(s_personality_config, "default_mood");
    if (default_mood) {
        initialMood.happiness() = cJSON_GetObjectItem(default_mood, "HAPPINESS")->valueint;
        initialMood.curiosity() = cJSON_GetObjectItem(default_mood, "CURIOSITY")->valueint;
        initialMood.contentment() = cJSON_GetObjectItem(default_mood, "CONTENTMENT")->valueint;
        initialMood.excitement() = cJSON_GetObjectItem(default_mood, "EXCITEMENT")->valueint;
        initialMood.affection() = cJSON_GetObjectItem(default_mood, "AFFECTION")->valueint;
        // ... other moods
    }
    
    write("g_MOOD", &initialMood, sizeof(Mood));
    
    ESP_LOGI(TAG, "Personality loaded: %s", 
        cJSON_GetObjectItem(s_personality_config, "personality_name")->valuestring);
    
    return ESP_OK;
}

void personality_generic_act(void) {
    // Read environment from global memory (NO MESH - just memcpy)
    Environment currentEnvir;
    read("g_Envir", &currentEnvir, sizeof(Environment));
    
    // Read current mood from global memory (NO MESH - just memcpy)
    Mood currentMood;
    read("g_MOOD", &currentMood, sizeof(Mood));
    
    // Work on local copy
    Mood newMood = currentMood;
    
    // Get mood reactions from JSON
    cJSON *reactions = cJSON_GetObjectItem(s_personality_config, "mood_reactions");
    
    // === Close Object Reaction ===
    cJSON *close_obj = cJSON_GetObjectItem(reactions, "close_object");
    if (close_obj) {
        int threshold = cJSON_GetObjectItem(close_obj, "distance_threshold_cm")->valueint;
        
        if (currentEnvir.target_detected && currentEnvir.distance_cm < threshold) {
            cJSON *triggers = cJSON_GetObjectItem(close_obj, "triggers");
            cJSON *trigger = NULL;
            cJSON_ArrayForEach(trigger, triggers) {
                const char *mood_name = cJSON_GetObjectItem(trigger, "mood")->valuestring;
                int delta = cJSON_GetObjectItem(trigger, "delta")->valueint;
                
                // Apply delta to appropriate mood component
                if (strcmp(mood_name, "CURIOSITY") == 0) {
                    newMood.addCuriosity(delta);
                } else if (strcmp(mood_name, "EXCITEMENT") == 0) {
                    newMood.addExcitement(delta);
                } else if (strcmp(mood_name, "HAPPINESS") == 0) {
                    newMood.addHappiness(delta);
                }
                // ... other moods
            }
            
            s_last_interaction_time = g_loopCount;
            ESP_LOGD(TAG, "Close object detected - mood updated");
        }
    }
    
    // === Face Detection Reaction ===
    cJSON *face_detect = cJSON_GetObjectItem(reactions, "face_detected");
    if (face_detect && currentEnvir.face_detected) {
        float threshold = cJSON_GetObjectItem(face_detect, "confidence_threshold")->valuedouble;
        
        if (currentEnvir.target_confidence >= threshold) {
            cJSON *triggers = cJSON_GetObjectItem(face_detect, "triggers");
            cJSON *trigger = NULL;
            cJSON_ArrayForEach(trigger, triggers) {
                const char *mood_name = cJSON_GetObjectItem(trigger, "mood")->valuestring;
                int delta = cJSON_GetObjectItem(trigger, "delta")->valueint;
                
                apply_mood_delta(&newMood, mood_name, delta);
            }
            
            s_last_interaction_time = g_loopCount;
            ESP_LOGD(TAG, "Face detected - mood updated");
        }
    }
    
    // === Temperature Reaction ===
    cJSON *hot_temp = cJSON_GetObjectItem(reactions, "hot_temperature");
    if (hot_temp) {
        int threshold = cJSON_GetObjectItem(hot_temp, "temperature_threshold_c")->valueint;
        
        if (currentEnvir.temperature_c > threshold * 10) {  // temp_c is  10
            cJSON *triggers = cJSON_GetObjectItem(hot_temp, "triggers");
            cJSON *trigger = NULL;
            cJSON_ArrayForEach(trigger, triggers) {
                const char *mood_name = cJSON_GetObjectItem(trigger, "mood")->valuestring;
                int delta = cJSON_GetObjectItem(trigger, "delta")->valueint;
                
                apply_mood_delta(&newMood, mood_name, delta);
            }
        }
    }
    
    // === No Interaction Timeout ===
    cJSON *no_interaction = cJSON_GetObjectItem(reactions, "no_interaction");
    if (no_interaction) {
        int timeout_sec = cJSON_GetObjectItem(no_interaction, "timeout_seconds")->valueint;
        uint32_t timeout_loops = timeout_sec * 10;  // 10 Hz loop rate
        
        if (g_loopCount - s_last_interaction_time > timeout_loops) {
            cJSON *triggers = cJSON_GetObjectItem(no_interaction, "triggers");
            cJSON *trigger = NULL;
            cJSON_ArrayForEach(trigger, triggers) {
                const char *mood_name = cJSON_GetObjectItem(trigger, "mood")->valuestring;
                int delta = cJSON_GetObjectItem(trigger, "delta")->valueint;
                
                apply_mood_delta(&newMood, mood_name, delta);
            }
            
            ESP_LOGD(TAG, "No interaction timeout - mood adjusted");
        }
    }
    
    // === Mood Decay ===
    cJSON *decay = cJSON_GetObjectItem(s_personality_config, "mood_decay");
    if (decay) {
        int interval_sec = cJSON_GetObjectItem(decay, "decay_interval_seconds")->valueint;
        uint32_t interval_loops = interval_sec * 10;
        
        if (g_loopCount % interval_loops == 0) {
            cJSON *decay_rates = cJSON_GetObjectItem(decay, "decay_rates");
            cJSON *rate = NULL;
            cJSON_ArrayForEach(rate, decay_rates) {
                const char *mood_name = cJSON_GetObjectItem(rate, "mood")->valuestring;
                int delta = cJSON_GetObjectItem(rate, "delta")->valueint;
                
                apply_mood_delta(&newMood, mood_name, delta);
            }
            
            ESP_LOGD(TAG, "Mood decay applied");
        }
    }
    
    // === Write Updated Mood (Triggers Mesh Broadcast) ===
    if (newMood != currentMood) {
        write("g_MOOD", &newMood, sizeof(Mood));
        
        ESP_LOGD(TAG, "Mood written: HAPPINESS=%d, CURIOSITY=%d, EXCITEMENT=%d",
            newMood.happiness(), newMood.curiosity(), newMood.excitement());
    }
}

// Helper function to apply mood delta by name
static void apply_mood_delta(Mood *mood, const char *mood_name, int delta) {
    if (strcmp(mood_name, "ANGER") == 0) {
        mood->addAnger(delta);
    } else if (strcmp(mood_name, "FEAR") == 0) {
        mood->addFear(delta);
    } else if (strcmp(mood_name, "HAPPINESS") == 0) {
        mood->addHappiness(delta);
    } else if (strcmp(mood_name, "SADNESS") == 0) {
        mood->addSadness(delta);
    } else if (strcmp(mood_name, "CURIOSITY") == 0) {
        mood->addCuriosity(delta);
    } else if (strcmp(mood_name, "AFFECTION") == 0) {
        mood->addAffection(delta);
    } else if (strcmp(mood_name, "IRRITATION") == 0) {
        mood->addIrritation(delta);
    } else if (strcmp(mood_name, "CONTENTMENT") == 0) {
        mood->addContentment(delta);
    } else if (strcmp(mood_name, "EXCITEMENT") == 0) {
        mood->addExcitement(delta);
    }
}
```

## Bot Configuration References Personality

```json
// config/bots/goblin_full.json
{
    "relative_filename": "config/bots/goblin_full.json",
    "bot_name": "Goblin (Full Body)",
    "bot_type": "GOBLINFULL",
    
    "personality": "config/personalities/goblin_mischievous.json",
    
    "subsystems": [
        {
            "subsystem_type": "HEAD",
            "subsystem_file": "config/bots/goblin_full_head.json"
        },
        {
            "subsystem_type": "TORSO",
            "subsystem_file": "config/bots/goblin_full_torso.json",
            "components": [
                {
                    "component_type": "PERSONALITY",
                    "component_file": "config/components/functional/personality_generic.json",
                    "hitCount": 15
                }
            ]
        }
    ]
}
```

## write() Function Behavior

```cpp
esp_err_t write(const char *name, void *src, size_t size) {
    // 1. Copy local variable  global memory pool
    memcpy(global_pool[name], src, size);
    
    // 2. Mark block as dirty
    mark_dirty(name);
    
    // 3. Mesh component will broadcast on next loop
    // (ESP-NOW packets sent to all peers with this exact memory block)
    
    return ESP_OK;
}
```

**Mesh broadcast happens in `esp_now_mesh_act()`**:

```cpp
void esp_now_mesh_act(void) {
    // hitCount: 1  runs every loop (100ms)
    
    // Get dirty blocks
    const char *dirty_blocks[64];
    int count = get_dirty_blocks(dirty_blocks, 64);
    
    for (int i = 0; i < count; i++) {
        const char *name = dirty_blocks[i];
        void *data = read(name);          // Get pointer to global memory
        size_t size = get_shared_size(name);
        
        // Create ESP-NOW packet
        mesh_packet_t packet;
        strncpy(packet.name, name, 31);
        memcpy(packet.data, data, size);  // Copy EXACT memory block
        packet.size = size;
        
        // Broadcast to all mesh peers
        esp_now_send(NULL, (uint8_t*)&packet, sizeof(packet));
        
        mark_shared_clean(name);
    }
}
```

**On receiving node**:

```cpp
void on_mesh_packet_received(const uint8_t *mac, const uint8_t *data, int len) {
    mesh_packet_t *packet = (mesh_packet_t*)data;
    
    // Copy received memory directly into local global pool
    void *dest = read(packet->name);
    memcpy(dest, packet->data, packet->size);
    
    ESP_LOGD(TAG, "Synced '%s': %d bytes", packet->name, packet->size);
}
```

**Result**: When torso calls `write("g_MOOD", &newMood, sizeof(Mood))`:
1. Local `newMood` variable  torso's global memory
2. Marked dirty
3. Next loop (100ms): ESP-NOW broadcasts exact 9 bytes to head
4. Head receives packet  copies 9 bytes into head's global memory
5. Head eye component calls `read("g_MOOD")`  instant memcpy from head's global pool

## Benefits

###  **Artists Can Design Personalities**
No C++ programming required - just edit JSON:
```json
"close_object": {
    "triggers": [
        {"mood": "CURIOSITY", "delta": 20}  // Change 10  20 (more curious!)
    ]
}
```

###  **Rapid Prototyping**
Test different personalities by swapping JSON files:
```bash
# Calm bear
"personality": "config/personalities/bear_gentle.json"

# Aggressive orc
"personality": "config/personalities/orc_warrior.json"

# Playful cat
"personality": "config/personalities/cat_playful.json"
```

###  **Same Code, Different Creatures**
Generic personality component works for ALL creatures:
- Goblin: Mischievous, curious, excitable
- Bear: Gentle, calm, affectionate
- Cat: Aloof, irritable, independent
- Cyclops: Aggressive, fearful, territorial

###  **Easy Balancing**
Tune mood reactions without recompiling:
```json
// Too aggressive? Reduce anger delta
"loud_sound": {
    "triggers": [
        {"mood": "ANGER", "delta": 5}  // Was 15, now 5
    ]
}
```

###  **Version Control**
Track personality changes in Git:
```bash
git diff config/personalities/goblin_mischievous.json

- {"mood": "CURIOSITY", "delta": 10}
+ {"mood": "CURIOSITY", "delta": 15}
```

## File Organization

```
config/
 personalities/
    bear_gentle.json
    cat_aloof.json
    cat_playful.json
    cyclops_aggressive.json
    goblin_mischievous.json
    goblin_shy.json
    orc_warrior.json

 bots/
    goblin_full.json           References personality
    bear_full.json              References personality
    cat_full.json               References personality

 components/
     functional/
         personality_generic.json   Component definition
```

## Battery Monitoring Component

Battery subsystem updates `g_Envir` with power status:

```cpp
// src/components/battery_monitor.c

#include "p32_shared_memory.h"
#include "Environment.hpp"
#include <driver/adc.h>

// ADC channels for voltage/current sensing
#define BATTERY_VOLTAGE_ADC ADC1_CHANNEL_0  // GPIO1
#define BATTERY_CURRENT_ADC ADC1_CHANNEL_1  // GPIO2

// Voltage divider: 10k + 10k (divide by 2)
#define VOLTAGE_DIVIDER_RATIO 2.0f
#define ADC_VREF_MV 3300
#define ADC_MAX 4095

// Current sensor: ACS712-05A (185 mV/A, 2.5V at 0A)
#define CURRENT_SENSITIVITY_MV_PER_A 185
#define CURRENT_ZERO_OFFSET_MV 2500

// Battery capacity (from POWER-BUDGET-SPEC.md)
#define BATTERY_CAPACITY_MAH 5000
#define BATTERY_CELL_COUNT 2  // 2S LiPo

esp_err_t battery_monitor_init(void) {
    // Configure ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(BATTERY_VOLTAGE_ADC, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(BATTERY_CURRENT_ADC, ADC_ATTEN_DB_11);
    
    ESP_LOGI(TAG, "Battery monitor initialized (2S %dmAh)", BATTERY_CAPACITY_MAH);
    
    return ESP_OK;
}

void battery_monitor_act(void) {
    // hitCount: 20  every 2 seconds
    
    // Read current environment
    Environment currentEnvir;
    read("g_Envir", &currentEnvir, sizeof(Environment));
    
    // === Measure Battery Voltage ===
    int adc_voltage = adc1_get_raw(BATTERY_VOLTAGE_ADC);
    float voltage_v = (adc_voltage * ADC_VREF_MV / ADC_MAX) * VOLTAGE_DIVIDER_RATIO / 1000.0f;
    uint16_t voltage_mv = (uint16_t)(voltage_v * 1000);
    
    // === Measure Battery Current ===
    int adc_current = adc1_get_raw(BATTERY_CURRENT_ADC);
    float current_sensor_mv = (adc_current * ADC_VREF_MV / ADC_MAX);
    float current_a = (current_sensor_mv - CURRENT_ZERO_OFFSET_MV) / CURRENT_SENSITIVITY_MV_PER_A;
    int16_t current_ma = (int16_t)(current_a * 1000);
    
    // === Calculate Power ===
    uint16_t power_mw = (uint16_t)((voltage_v * current_a) * 1000);
    
    // === Calculate State of Charge (SOC) ===
    // 2S LiPo: 8.4V (full)  6.4V (empty)
    float voltage_per_cell = voltage_v / BATTERY_CELL_COUNT;
    uint8_t soc_percent;
    
    if (voltage_per_cell >= 4.2f) {
        soc_percent = 100;
    } else if (voltage_per_cell <= 3.2f) {
        soc_percent = 0;
    } else {
        // Linear approximation (good enough for LiPo)
        soc_percent = (uint8_t)(((voltage_per_cell - 3.2f) / 1.0f) * 100);
    }
    
    // === Calculate Time Remaining ===
    uint16_t time_remaining_min = 0;
    if (current_ma > 100) {  // Only calculate if drawing significant current
        float remaining_mah = (BATTERY_CAPACITY_MAH * soc_percent) / 100.0f;
        time_remaining_min = (uint16_t)((remaining_mah / (current_ma / 1000.0f)) * 60);
    }
    
    // === Update Environment ===
    Environment newEnvir = currentEnvir;
    newEnvir.battery_percent = soc_percent;
    newEnvir.battery_voltage_mv = voltage_mv;
    newEnvir.battery_current_ma = current_ma;
    newEnvir.battery_power_mw = power_mw;
    newEnvir.battery_charging = (current_ma < -100);  // Negative = charging
    newEnvir.battery_low = (soc_percent < 20);
    newEnvir.battery_critical = (soc_percent < 10);
    newEnvir.battery_time_remaining_min = time_remaining_min;
    
    // Only write if changed
    if (newEnvir != currentEnvir) {
        write("g_Envir", &newEnvir, sizeof(Environment));
        
        ESP_LOGI(TAG, "Battery: %d%% (%dmV, %dmA, %dmW) - %dmin remaining",
            soc_percent, voltage_mv, current_ma, power_mw, time_remaining_min);
        
        if (newEnvir.battery_critical) {
            ESP_LOGW(TAG, "CRITICAL BATTERY! Shutdown imminent!");
        } else if (newEnvir.battery_low) {
            ESP_LOGW(TAG, "Low battery warning");
        }
    }
}
```

**Personality Reacts to Battery**:
- **Low battery (20%)**  Sadness +10, Irritation +5, Excitement -10
- **Critical battery (10%)**  Fear +15, Sadness +15, Irritation +10
- **Result**: Creature becomes lethargic and fearful when battery is low!

## Battery State Affects All Behaviors

**Key Insight**: Battery is monitored by ONE subsystem (torso), but affects ALL subsystems!

### Example: Dance Behavior Adapts to Battery

```cpp
// src/components/dance_behavior.cpp

#include "p32_shared_memory.h"
#include "Environment.hpp"

void dance_behavior_act(void) {
    // Read environment (NO MESH - instant memcpy!)
    Environment currentEnvir;
    read("g_Envir", &currentEnvir, sizeof(Environment));
    
    // Adapt dance speed based on battery
    uint8_t dance_speed;
    uint8_t servo_range;
    
    if (currentEnvir.battery_critical) {
        // <10% battery: Very slow, minimal movement
        dance_speed = 10;      // 10% of normal speed
        servo_range = 20;      // 20 range (minimal)
        ESP_LOGI(TAG, "Critical battery - slow dance");
        
    } else if (currentEnvir.battery_low) {
        // <20% battery: Slow, reduced range
        dance_speed = 40;      // 40% of normal speed
        servo_range = 45;      // 45 range (half)
        ESP_LOGI(TAG, "Low battery - reduced dance");
        
    } else if (currentEnvir.battery_percent < 50) {
        // <50% battery: Normal but slightly reduced
        dance_speed = 70;      // 70% of normal speed
        servo_range = 70;      // 70 range
        ESP_LOGI(TAG, "Medium battery - moderate dance");
        
    } else {
        // >50% battery: Full energy!
        dance_speed = 100;     // 100% of normal speed
        servo_range = 90;      // 90 range (full)
        ESP_LOGI(TAG, "Full battery - energetic dance");
    }
    
    // Apply to all servos
    set_arm_dance_pattern(dance_speed, servo_range);
    set_leg_dance_pattern(dance_speed, servo_range);
    set_head_dance_pattern(dance_speed, servo_range);
    
    ESP_LOGD(TAG, "Dance: speed=%d%%, range=%d, battery=%d%%",
        dance_speed, servo_range, currentEnvir.battery_percent);
}
```

### Example: Arm Movement Speed Scaling

```cpp
// src/components/arm_left_controller.cpp

void arm_left_controller_act(void) {
    // Read environment
    Environment currentEnvir;
    read("g_Envir", &currentEnvir, sizeof(Environment));
    
    // Scale servo speed based on battery
    float speed_multiplier;
    
    if (currentEnvir.battery_critical) {
        speed_multiplier = 0.2f;  // 20% speed (preserve power)
    } else if (currentEnvir.battery_low) {
        speed_multiplier = 0.5f;  // 50% speed
    } else {
        speed_multiplier = 1.0f;  // 100% speed
    }
    
    // Apply speed scaling to all servo commands
    set_shoulder_speed(BASE_SPEED * speed_multiplier);
    set_elbow_speed(BASE_SPEED * speed_multiplier);
    set_wrist_speed(BASE_SPEED * speed_multiplier);
}
```

### Example: Display Brightness Reduction

```cpp
// src/components/goblin_eye_left.cpp

void goblin_eye_left_act(void) {
    // Read environment
    Environment currentEnvir;
    read("g_Envir", &currentEnvir, sizeof(Environment));
    
    // Reduce display brightness to save power
    uint8_t brightness;
    
    if (currentEnvir.battery_critical) {
        brightness = 30;   // 30% brightness (5.1W  1.5W per display)
    } else if (currentEnvir.battery_low) {
        brightness = 60;   // 60% brightness
    } else {
        brightness = 100;  // 100% brightness
    }
    
    set_display_brightness(brightness);
    
    // Render mood-based expression at reduced brightness
    render_eye_frame();
}
```

### Benefits of g_Envir Architecture

**Single Source of Truth**:
-  Battery monitor (torso) updates `g_Envir` every 2 seconds
-  Mesh broadcasts to ALL subsystems (head, arms, legs)
-  Each subsystem reads locally (NO network latency!)
-  All behaviors adapt automatically

**Doesn't Change Often**:
- Battery updates: 2-second intervals (0.5 Hz)
- Bandwidth: ~30 bytes  0.5 Hz = **15 bytes/sec**
- Only broadcasts when values change (dirty flag)
- Mesh has plenty of headroom (50 KB/s capacity)

**Known By Everyone**:
- Dance behavior: "Am I energetic or lethargic?"
- Servo controllers: "Should I move fast or slow?"
- Display renderers: "Can I use full brightness?"
- Personality: "Should I be happy or fearful?"
- All components: Instant read via memcpy!

**Architecture Flow**:

```
Torso ESP32-S3:

 Battery Monitor (every 2 sec):       
   measure_voltage()                  
   measure_current()                  
   calculate_soc()                    
   write("g_Envir", &envir)   Triggers broadcast

               ESP-NOW mesh (1-5 ms, 30 bytes)

 Head          Arm Left     Arm Right     Leg Left     
                                                     
 read("g_Envir") read()     read()         read()         NO MESH!
                                                  
 brightness    speed        speed         speed       
 = 60%         = 50%        = 50%         = 50%       


Result: When user says "dance!", creature dances at speed appropriate
        for current battery level. Slow graceful dance when tired!
```

**Real-World Example**:

```
Battery 100%  User: "Dance!"
     Dance speed: 100%, servo range: 90, brightness: 100%
     Result: ENERGETIC hip-hop moves, bright eyes! 

Battery 40%  User: "Dance!"
     Dance speed: 70%, servo range: 70, brightness: 100%
     Result: Moderate groove, full brightness

Battery 15%  User: "Dance!"  (LOW battery)
     Dance speed: 40%, servo range: 45, brightness: 60%
     Result: Slow waltz, dimmed eyes 
     Mood: Sadness +10, Irritation +5

Battery 8%  User: "Dance!"  (CRITICAL battery)
     Dance speed: 10%, servo range: 20, brightness: 30%
     Result: Barely swaying, very dim eyes 
     Mood: Fear +15, Sadness +15, Irritation +10
     System warning: "Shutdown imminent!"
```

## Summary

**Data-Driven Design**:
-  JSON defines ALL personality behavior
-  C++ code is generic (loads JSON, applies rules)
-  Same code works for all creatures
-  Artists/designers can tweak without programming

**Memory Architecture**:
-  `read()` = memcpy from local global pool (NO mesh, instant)
-  `write()` = memcpy to local global pool + mark dirty
-  Mesh broadcasts exact memory block (9 bytes for Mood)
-  Receiving node memcpys into its global pool

**Environment Class**:
-  Sensors (distance, temperature, humidity, sound, light)
-  Face tracking (position, confidence)
-  Battery subsystem (voltage, current, SOC, time remaining)
-  All in single `g_Envir` shared memory block

**Result**: Non-programmers can design creature personalities by editing JSON! 

 **Related Documents**:
- [Global Shared Memory API](GLOBAL-SHARED-MEMORY-API.md)
- [Mesh State Synchronization](MESH-STATE-SYNCHRONIZATION-SPEC.md)
- [Component Function Signatures](COMPONENT-FUNCTION-SIGNATURES.md)
