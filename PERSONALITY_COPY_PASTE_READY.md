# Personality System: Copy-Paste Ready Code

## Ready-to-Use Code Snippets

### 1. goblin_right_eye.src (Copy from left_eye with buffer name change)

Simply replace all occurrences of `left_eye_buffer` with `right_eye_buffer` in the code from `goblin_left_eye.src`. Everything else is identical.

**Key changes only:**
```cpp
// CHANGE THIS:
static uint16_t* left_eye_buffer = nullptr;

// TO THIS:
static uint16_t* right_eye_buffer = nullptr;

// CHANGE THIS:
if (!left_eye_buffer) {
    return;
}

// TO THIS:
if (!right_eye_buffer) {
    return;
}

// CHANGE THIS:
left_eye_buffer = (uint16_t*)malloc(BUFFER_BYTES);
if (!left_eye_buffer) {

// TO THIS:
right_eye_buffer = (uint16_t*)malloc(BUFFER_BYTES);
if (!right_eye_buffer) {

// CHANGE ALL OCCURRENCES:
left_eye_buffer[i] = ...
// TO:
right_eye_buffer[i] = ...
```

### 2. goblin_mouth.src (Same pattern)

Replace `left_eye_buffer` with `mouth_buffer` throughout.

The rendering logic is **completely identical** - personality intensity is read from SharedMemory the same way.

### 3. Quick Copy-Paste for Any New Display

For any new display component (e.g., `goblin_eyes_combined.src`):

```cpp
// goblin_eyes_combined.src - Example: render both eyes in one component
#include "esp_log.h"
#include "Mood.hpp"
#include "Personality.hpp"
#include "core/memory/SharedMemory.hpp"

static const char* TAG = "goblin_eyes_combined";
static uint16_t* combined_buffer = nullptr;
static const uint32_t BUFFER_SIZE = 240 * 240 * 2;  // Both eyes
static uint32_t last_mood_checksum = 0;

struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} cached_mood_tint = {0, 0, 0};

esp_err_t goblin_eyes_combined_init(void) {
    ESP_LOGI(TAG, "Initializing combined eye display");
    
    combined_buffer = (uint16_t*)malloc(BUFFER_SIZE * 2);
    if (!combined_buffer) {
        ESP_LOGE(TAG, "Failed to allocate buffer");
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize with green iris
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        combined_buffer[i] = 0x0400;
    }
    
    return ESP_OK;
}

void goblin_eyes_combined_act(void) {
    if (!combined_buffer) return;
    
    Mood* mood = GSM.read<Mood>();
    Personality* pers = GSM.read<Personality>();
    if (!mood || !pers) return;
    
    uint32_t current_checksum = (uint32_t)(mood->anger * 1000) + 
                                (uint32_t)(mood->curiosity * 100) + 
                                (uint32_t)(mood->fear * 10) + 
                                (uint32_t)(mood->happiness);
    
    if (current_checksum != last_mood_checksum) {
        last_mood_checksum = current_checksum;
        
        // <-- PERSONALITY INTENSITY MULTIPLIERS <--
        float aggression_intensity = pers->getIntensityMultiplier(pers->base_aggression);
        float curiosity_intensity = pers->getIntensityMultiplier(pers->base_curiosity);
        float fear_intensity = pers->getIntensityMultiplier(pers->base_fear);
        float affection_intensity = pers->getIntensityMultiplier(pers->base_affection);
        
        uint16_t anger_component = (uint16_t)(mood->anger * 255.0f * 1.5f * aggression_intensity);
        cached_mood_tint.r = (anger_component > 255) ? 255 : (uint8_t)anger_component;
        
        uint16_t curiosity_component = (uint16_t)(mood->curiosity * 255.0f * 1.2f * curiosity_intensity);
        cached_mood_tint.g = (curiosity_component > 255) ? 255 : (uint8_t)curiosity_component;
        
        uint16_t fear_component = (uint16_t)(mood->fear * 255.0f * 1.3f * fear_intensity);
        cached_mood_tint.b = (fear_component > 255) ? 255 : (uint8_t)fear_component;
        
        uint8_t happiness_tint = (uint8_t)(mood->happiness * 180.0f * 1.4f * affection_intensity);
        uint16_t new_r = (uint16_t)cached_mood_tint.r + happiness_tint;
        uint16_t new_g = (uint16_t)cached_mood_tint.g + happiness_tint;
        cached_mood_tint.r = (new_r > 255) ? 255 : (uint8_t)new_r;
        cached_mood_tint.g = (new_g > 255) ? 255 : (uint8_t)new_g;
    }
    
    // Apply tint to both eye buffers
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        uint16_t pixel = combined_buffer[i];
        
        uint8_t r5 = (pixel >> 11) & 0x1F;
        uint8_t g6 = (pixel >> 5) & 0x3F;
        uint8_t b5 = pixel & 0x1F;
        
        uint8_t r8 = (r5 << 3) | (r5 >> 2);
        uint8_t g8 = (g6 << 2) | (g6 >> 4);
        uint8_t b8 = (b5 << 3) | (b5 >> 2);
        
        uint16_t new_r = (uint16_t)r8 + cached_mood_tint.r;
        uint16_t new_g = (uint16_t)g8 + cached_mood_tint.g;
        uint16_t new_b = (uint16_t)b8 + cached_mood_tint.b;
        
        r8 = (new_r > 255) ? 255 : (uint8_t)new_r;
        g8 = (new_g > 255) ? 255 : (uint8_t)new_g;
        b8 = (new_b > 255) ? 255 : (uint8_t)new_b;
        
        uint16_t r5_new = (r8 >> 3) & 0x1F;
        uint16_t g6_new = (g8 >> 2) & 0x3F;
        uint16_t b5_new = (b8 >> 3) & 0x1F;
        
        combined_buffer[i] = (r5_new << 11) | (g6_new << 5) | b5_new;
    }
}
```

## Implementation Checklist

### Phase 1: Foundation (DONE ✅)
- [x] Create `Personality.hpp` in `shared/`
- [x] Create `Personality.hpp` in `include/`
- [x] Add Personality to SharedMemory type IDs
- [x] Create goblin_personality.src (torso broadcaster)
- [x] Implement goblin_left_eye.src with personality scaling

### Phase 2: Replication (YOUR TURN)
- [ ] Copy goblin_left_eye.src to goblin_right_eye.src (change buffer name)
- [ ] Copy goblin_left_eye.src to goblin_mouth.src (change buffer name)
- [ ] Verify all three files have personality integration

### Phase 3: Build & Test (YOUR TURN)
- [ ] Regenerate: `python tools/generate_tables.py config/bots/bot_families/goblins/torso/goblin_torso.json src`
- [ ] Regenerate: `python tools/generate_tables.py config/bots/bot_families/goblins/head/goblin_head.json src`
- [ ] Build: `pio run -e goblin_head`
- [ ] Flash torso + head
- [ ] Check serial logs for "Personality loaded"
- [ ] Verify eyes display with baseline mood colors
- [ ] Change personality values → verify intensity changes

### Phase 4: Optimization (OPTIONAL)
- [ ] Add dynamic personality changes based on sensor input
- [ ] Implement personality persistence (NVS storage)
- [ ] Create personality adjustment API for future UI/control

## Testing Verification

```cpp
// Test 1: Verify personality broadcasts
// Serial output should show:
// "Personality loaded: aggression=60, curiosity=70, fear=20, affection=40"

// Test 2: Verify intensity calculation
Personality p;
p.base_aggression = 0;
assert(p.getIntensityMultiplier(0) == 1.0f);      // Neutral

p.base_aggression = 127;
float intense = p.getIntensityMultiplier(127);
assert(intense == 2.0f);                          // Maximum amplification

p.base_aggression = -128;
float dampen = p.getIntensityMultiplier(-128);
assert(dampen == 0.5f);                           // Maximum dampening

// Test 3: Verify color rendering
// With mood->anger = 80, personality aggression = 100
// Expected: intensity = 1.79x
// Expected red_tint = 80 * 255 * 1.5 * 1.79 ≈ 54,927 (clamped to 255)
// Expected visual: bright red overlay
```

## Success Criteria

- ✅ All three eye displays use identical code (only buffer names differ)
- ✅ Personality is read from SharedMemory in all displays
- ✅ Intensity multiplier scales mood effects consistently
- ✅ All eyes respond together to personality changes
- ✅ Code is modular and works for any future display component
- ✅ No hardcoded personality values in display components

## Common Issues & Solutions

| Issue | Cause | Solution |
|-------|-------|----------|
| Eyes don't show color | Personality not broadcast | Check torso init runs |
| Different colors per eye | Using different code | Copy exact code |
| Intensity always 1.0x | Not reading personality | Verify `GSM.read<Personality>()` |
| Compilation errors | Missing #include | Check all includes present |
| ESP-NOW not working | Controller not configured | Check torso has WiFi/ESP-NOW |

## Next Advanced Features

After basic integration works, consider:

1. **Personality Lerp**: Smoothly transition personality over time
   ```cpp
   target_aggression = 100;
   current_aggression += (target_aggression - current_aggression) * 0.1f;
   ```

2. **Sensor-Driven Personality**: React to environment
   ```cpp
   if (threat_detected) base_aggression = 100;
   if (safe_zone) base_fear = 0;
   if (food_detected) base_curiosity = 127;
   ```

3. **Personality Memory**: Save/restore between power cycles
   ```cpp
   nvs_write<Personality>();  // Save to NVS
   nvs_read<Personality>();   // Restore on boot
   ```

4. **Multi-Creature Negotiation**: Average personalities in mesh
   ```cpp
   // If multiple goblins, blend personalities
   average_aggression = (my_aggression + neighbor_aggression) / 2;
   ```
