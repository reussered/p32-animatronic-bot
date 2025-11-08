# Personality-Driven Mood Intensity Pattern

## Overview

The personality system modulates how strongly mood affects the visual display. While **Mood** defines the *current emotional state* (anger, fear, happiness, etc.), **Personality** defines the *character traits* that determine *how intensely* mood changes are expressed visually.

**Pattern**: Same rendering code works for ALL eyes because personality intensity is read dynamically from SharedMemory.

## Architecture

```
┌─ Personality Module (torso) ─────────────────────┐
│  Reads: base_aggression, base_curiosity from     │
│          goblin_personality.json                 │
│  Writes: Personality struct to GSM               │
└────────────────────────────────────────────────┬─┘
                                                  │
                                    ESP-NOW Mesh (broadcast)
                                                  │
┌─────────────────────────────────────────────────▼─────┐
│  Head Subsystem (ESP32-S3)                             │
│  ┌─────────────────────────────────────────────────┐  │
│  │ goblin_left_eye_act():                          │  │
│  │   1. Read Mood from GSM                         │  │
│  │   2. Read Personality from GSM                  │  │
│  │   3. Calculate intensity_multiplier             │  │
│  │   4. Apply to color buffer with multiplier      │  │
│  │   5. Send to display                            │  │
│  │                                                  │  │
│  │ goblin_right_eye_act():  [IDENTICAL CODE]      │  │
│  │ goblin_mouth_act():       [IDENTICAL CODE]      │  │
│  └─────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
```

## Step 1: Define Personality Structure

Create `shared/Personality.hpp`:

```cpp
#ifndef PERSONALITY_HPP
#define PERSONALITY_HPP

#include <cstdint>

class Personality {
public:
    // Family-level traits (set by torso personality module)
    int8_t base_aggression;      // 0-127: how aggressive responses are (-128 to +127)
    int8_t base_curiosity;       // 0-127: how curious/exploratory
    int8_t base_fear;            // 0-127: baseline fear/caution
    int8_t base_affection;       // 0-127: how friendly/affectionate
    
    Personality() 
        : base_aggression(60), base_curiosity(70), base_fear(20), base_affection(40)
    {
    }
    
    // Get intensity multiplier (0.5 to 2.0) based on trait
    float getIntensityMultiplier(int8_t trait_value) const {
        // Neutral at 0, ranges from 0.5x (timid) to 2.0x (intense)
        if (trait_value < 0) {
            // Negative: dampen effects (0.5x to 1.0x)
            return 0.5f + (trait_value / -128.0f) * 0.5f;
        } else {
            // Positive: amplify effects (1.0x to 2.0x)
            return 1.0f + (trait_value / 127.0f) * 1.0f;
        }
    }
};

#endif // PERSONALITY_HPP
```

## Step 2: Register Personality with SharedMemory

Add to `include/core/memory/SharedMemory.hpp`:

```cpp
#include "../shared/Personality.hpp"  // Add include

// Add type specialization
template<>
inline shared_type_id_t getTypeId<Personality>() {
    return 4;  // New type ID
}
```

## Step 3: Personality Module (Torso)

Create `config/components/functional/goblin_personality.src`:

```cpp
// goblin_personality.src - Family-level personality trait management
#include "esp_log.h"
#include "Personality.hpp"
#include "core/memory/SharedMemory.hpp"

static const char* TAG = "goblin_personality";

esp_err_t goblin_personality_init(void) {
    ESP_LOGI(TAG, "Initializing goblin personality traits");
    
    // Get or create personality instance in SharedMemory
    Personality* pers = GSM.read<Personality>();
    if (!pers) {
        ESP_LOGE(TAG, "Failed to initialize personality");
        return ESP_FAIL;
    }
    
    // Load from goblin_personality.json configuration (would be injected by use_fields)
    // For now, use defaults from Personality constructor
    pers->base_aggression = 60;   // From JSON: "base_aggression": 60
    pers->base_curiosity = 70;    // From JSON: "base_curiosity": 70
    pers->base_fear = 20;         // From JSON: "base_fear": 20
    pers->base_affection = 40;    // Reasonable default
    
    // Broadcast to all other chips (head, etc)
    GSM.write<Personality>();
    
    ESP_LOGI(TAG, "Personality loaded: aggression=%d, curiosity=%d",
             pers->base_aggression, pers->base_curiosity);
    
    return ESP_OK;
}

void goblin_personality_act(void) {
    // In a more sophisticated implementation, would react to sensors:
    // - Camera detects threat → increase aggression
    // - Idle for long time → increase curiosity
    // - etc.
    
    // For now, personality is static (set once at init)
    // In future, uncomment to update dynamically:
    // Personality* pers = GSM.read<Personality>();
    // if (pers && threat_detected) {
    //     pers->base_aggression = MIN(127, pers->base_aggression + 10);
    //     GSM.write<Personality>();
    // }
}
```

## Step 4: Eye Display with Personality Intensity

The **same code works for ALL eyes** because personality is read from SharedMemory.

### Option A: Simple Multiplicative Intensity (RGB value multiplication)

```cpp
// goblin_left_eye.src - Eye display with personality-based mood intensity
#include "esp_log.h"
#include "Mood.hpp"
#include "Personality.hpp"
#include "core/memory/SharedMemory.hpp"

static const char* TAG = "goblin_left_eye";
static GoblinEyeMoodDisplay* eye_display = nullptr;

esp_err_t goblin_left_eye_init(void) {
    ESP_LOGI(TAG, "Initializing left eye display");
    
    // Initialize display driver (GC9A01 round LCD, 240x240, RGB565)
    eye_display = new GoblinEyeMoodDisplay();
    if (!eye_display->init(240, 240, RGB565_FORMAT, send_frame_to_gc9a01_left)) {
        ESP_LOGE(TAG, "Failed to initialize eye display");
        return ESP_FAIL;
    }
    
    return ESP_OK;
}

void goblin_left_eye_act(void) {
    if (!eye_display) return;
    
    // Read current mood
    Mood* mood = GSM.read<Mood>();
    if (!mood) return;
    
    // Read personality traits
    Personality* pers = GSM.read<Personality>();
    if (!pers) return;
    
    // Calculate how intensely to show mood based on personality
    float intensity_mult = pers->getIntensityMultiplier(pers->base_aggression);
    
    // APPROACH 1: Apply intensity to mood components before rendering
    Mood mood_scaled = *mood;
    mood_scaled.anger() = (int8_t)(mood_scaled.anger() * intensity_mult);
    mood_scaled.curiosity() = (int8_t)(mood_scaled.curiosity() * intensity_mult);
    mood_scaled.fear() = (int8_t)(mood_scaled.fear() * intensity_mult);
    mood_scaled.happiness() = (int8_t)(mood_scaled.happiness() * intensity_mult);
    
    // Render frame with scaled mood
    uint32_t base_color = 0x00FF00;  // Green iris
    eye_display->renderFrame(mood_scaled, base_color);
    eye_display->sendToDisplay();
}

// ... (identical for goblin_right_eye.src and goblin_mouth.src)
```

### Option B: Advanced - Apply Intensity to Color Tint Directly

```cpp
// More sophisticated: intensity modulates the color tint delta, not mood components

void goblin_left_eye_act(void) {
    if (!eye_display) return;
    
    Mood* mood = GSM.read<Mood>();
    Personality* pers = GSM.read<Personality>();
    if (!mood || !pers) return;
    
    // Get personality-driven intensity multiplier
    float aggression_intensity = pers->getIntensityMultiplier(pers->base_aggression);
    
    // Calculate mood tint (same as goblin_eye_mood_processor.src)
    Pixel tint = {0, 0, 0};
    
    // Apply intensity multiplier to each mood component's contribution
    tint.r = (uint8_t)((mood->anger * 255.0f * 1.5f) * aggression_intensity);
    tint.g = (uint8_t)((mood->curiosity * 255.0f * 1.2f) * pers->getIntensityMultiplier(pers->base_curiosity));
    tint.b = (uint8_t)((mood->fear * 255.0f * 1.3f) * pers->getIntensityMultiplier(pers->base_fear));
    
    // Yellow for happiness
    uint8_t happiness_tint = (uint8_t)((mood->happiness * 180.0f * 1.4f) * pers->getIntensityMultiplier(pers->base_affection));
    tint.r = (uint8_t)MIN(255, tint.r + happiness_tint);
    tint.g = (uint8_t)MIN(255, tint.g + happiness_tint);
    
    // Apply tint to frame buffer
    for (int i = 0; i < TOTAL_PIXELS; i++) {
        frame_buffer[i].r = (uint8_t)MIN(255, frame_buffer[i].r + tint.r);
        frame_buffer[i].g = (uint8_t)MIN(255, frame_buffer[i].g + tint.g);
        frame_buffer[i].b = (uint8_t)MIN(255, frame_buffer[i].b + tint.b);
    }
    
    eye_display->sendToDisplay();
}
```

## Step 5: Use use_fields to Inject Personality Configuration

Update `config/bots/bot_families/goblins/head/goblin_left_eye.json`:

```json
{
    "name": "goblin_left_eye",
    "use_fields": {
        "color_schema": "RGB565",
        "personality_intensity_source": "base_aggression"
    },
    "components": []
}
```

The `personality_intensity_source` field tells the eye which personality trait to use for intensity scaling.

## How It Works: Reusable Code Pattern

**Torso (master)**:
```
goblin_personality.src reads config → sets Personality in GSM → broadcasts via ESP-NOW
```

**Head (all eyes, identical code)**:
```
Loop:
  mood = GSM.read<Mood>()           // From torso's mood module
  pers = GSM.read<Personality>()    // From torso's personality module (same object)
  intensity = pers->getIntensityMultiplier(pers->base_aggression)
  Apply intensity to color buffer
  Render & send to display
```

**Same code for ALL eyes**: The intensity multiplier is looked up dynamically, so no code duplication needed.

## Example Personality Traits from JSON

From `config/components/functional/goblin_personality.json`:

```json
{
    "name": "goblin_personality",
    "personality_traits": {
        "base_aggression": 60,      // High (goblins are aggressive)
        "base_curiosity": 70,       // Very high (goblins are mischievous)
        "base_fear": 20,            // Low (goblins are brave)
        "threat_sensitivity": "HIGH"
    }
}
```

## Expected Visual Behavior

- **Angry goblin** (high `base_aggression`): Red/orange tint 2x stronger in eyes
- **Curious goblin** (high `base_curiosity`): Green tint 2x stronger
- **Calm goblin** (low `base_aggression`): Color effects muted to 0.5x
- **Scared goblin** (high `base_fear`): Blue tint 2x stronger

## Integration Checklist

- [ ] Create `shared/Personality.hpp` with struct and `getIntensityMultiplier()`
- [ ] Add Personality to SharedMemory type IDs
- [ ] Create `config/components/functional/goblin_personality.src` (torso module)
- [ ] Update `config/bots/bot_families/goblins/head/goblin_left_eye.src` with code above
- [ ] Copy same code to `goblin_right_eye.src` and `goblin_mouth.src`
- [ ] Verify personality broadcasts via ESP-NOW from torso to head
- [ ] Test: Change personality traits → observe eye colors intensify/dampen

## Notes

- **No hardware dependency**: Personality is purely software/configuration
- **Reusable**: Identical code works for left eye, right eye, mouth, any other display
- **Dynamic**: Personality can be modified at runtime by torso module based on sensor input
- **Modular**: Each component reads only what it needs from SharedMemory
