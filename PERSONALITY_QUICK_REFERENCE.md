# Personality-Mood Integration: Quick Reference

## One-Page Cheat Sheet

### The Pattern

```
┌─ Torso (ESP32-S3) ─────────────────────┐
│ goblin_personality.src                 │
│ - Load traits from JSON config          │
│ - Write to SharedMemory (GSM)           │
│ - Broadcast via ESP-NOW to head         │
└─────────────────────┬───────────────────┘
                      │ ESP-NOW Mesh
                      ▼
┌─ Head (ESP32-S3) ───────────────────────┐
│ goblin_left_eye.src   [IDENTICAL CODE]  │
│ goblin_right_eye.src  [IDENTICAL CODE]  │
│ goblin_mouth.src      [IDENTICAL CODE]  │
│                                          │
│ void act():                              │
│   mood = GSM.read<Mood>()               │
│   pers = GSM.read<Personality>()        │
│   intensity = pers->getIntensityMultiplier()  │
│   tint *= intensity                     │
│   render_buffer_with_tint()             │
│   send_to_display()                     │
└──────────────────────────────────────────┘
```

### Personality Class

```cpp
class Personality {
  int8_t base_aggression;    // -128 to +127
  int8_t base_curiosity;     // -128 to +127
  int8_t base_fear;          // -128 to +127
  int8_t base_affection;     // -128 to +127
  
  float getIntensityMultiplier(int8_t trait) {
    // Trait < 0:  returns 0.5 to 1.0 (dampen)
    // Trait = 0:  returns 1.0 (neutral)
    // Trait > 0:  returns 1.0 to 2.0 (amplify)
  }
};
```

### Eye Rendering Code Pattern

```cpp
void goblin_left_eye_act() {
  // 1. Read state from SharedMemory
  Mood* mood = GSM.read<Mood>();
  Personality* pers = GSM.read<Personality>();
  
  // 2. Calculate intensity multiplier
  float intensity = pers->getIntensityMultiplier(pers->base_aggression);
  
  // 3. Apply to tint calculation
  red_tint = mood->anger * 255.0f * 1.5f * intensity;  // <-- multiplier applied
  
  // 4. Clamp and apply to buffer
  for (i = 0; i < total_pixels; i++) {
    buffer[i].r = MIN(255, buffer[i].r + red_tint);
  }
  
  // 5. Send to display
  send_to_display(buffer);
}
```

### Why Same Code Works Everywhere

| Component | Code Location | What's Different? |
|-----------|---------------|-------------------|
| Left Eye | `goblin_left_eye.src` | **NOTHING** - Uses `left_eye_buffer` variable |
| Right Eye | `goblin_right_eye.src` | **NOTHING** - Uses `right_eye_buffer` variable |
| Mouth | `goblin_mouth.src` | **NOTHING** - Uses `mouth_buffer` variable |

Everything else—mood scaling, personality lookup, intensity multiplier—is **IDENTICAL**.

### Intensity Multiplier Examples

```
Personality trait range:  -128        0         +127
                           |          |          |
Intensity multiplier:     0.5x  →  1.0x  →  2.0x

trait = -80  →  getIntensityMultiplier() = 0.5 + (80/128)*0.5 = 0.8125x
trait = 0    →  getIntensityMultiplier() = 1.0x
trait = +80  →  getIntensityMultiplier() = 1.0 + (80/127)*1.0 = 1.63x
trait = +127 →  getIntensityMultiplier() = 2.0x
```

### Example: Aggressive vs Calm Goblin

**Aggressive Goblin** (base_aggression = 100)
```
intensity = getIntensityMultiplier(100) = 1.0 + (100/127) = 1.79x
anger = 80 (very angry)
red_tint = 80 * 255 * 1.5f * 1.79 = massive red overlay
visual → Eyes bright RED, INTENSE
```

**Calm Goblin** (base_aggression = -80)
```
intensity = getIntensityMultiplier(-80) = 0.5 + (80/128)*0.5 = 0.81x
anger = 80 (same mood, but...)
red_tint = 80 * 255 * 1.5f * 0.81 = muted red overlay
visual → Eyes dimly RED, SOFT
```

### SharedMemory Integration

```cpp
// In include/core/memory/SharedMemory.hpp

#include "Personality.hpp"  // Add this

template<>
inline shared_type_id_t getTypeId<Personality>() {
    return 4;  // Personality = type ID 4
}
```

Then use anywhere:
```cpp
Personality* pers = GSM.read<Personality>();  // Get
pers->base_aggression = 60;                    // Modify
GSM.write<Personality>();                     // Broadcast
```

### Files to Check

```
✅ shared/Personality.hpp              - Class definition
✅ include/Personality.hpp             - Class definition (duplicate for includes)
✅ include/core/memory/SharedMemory.hpp - Type registration
✅ config/bots/bot_families/goblins/torso/goblin_personality.src - Broadcaster
✅ config/bots/bot_families/goblins/head/goblin_left_eye.src     - Consumer
```

### Regenerate Command

After modifying `.src` or JSON files:

```powershell
python tools/generate_tables.py config/bots/bot_families/goblins/torso/goblin_torso.json src
python tools/generate_tables.py config/bots/bot_families/goblins/head/goblin_head.json src
```

### Testing Checklist

- [ ] Compile without errors
- [ ] Flash to ESP32-S3 (torso + head)
- [ ] Check serial: "Personality loaded: aggression=60, curiosity=70..."
- [ ] Check serial: "Left eye buffer allocated..."
- [ ] Visual: Eyes respond to mood (default colors appear)
- [ ] Visual: Change personality in code → eye colors intensify/dim
- [ ] Head receives ESP-NOW personality broadcast from torso

### Common Mistakes

❌ **WRONG**: Copy just the mood rendering, forget to multiply by intensity
```cpp
red_tint = mood->anger * 255.0f * 1.5f;  // Missing intensity!
```

✅ **RIGHT**: Always include the personality multiplier
```cpp
red_tint = mood->anger * 255.0f * 1.5f * intensity;  // Include multiplier
```

---

❌ **WRONG**: Different code for each eye
```cpp
// Left eye: multiply by 1.5
// Right eye: multiply by 1.8
// Mouth: multiply by 2.0
```

✅ **RIGHT**: Single code, use personality from GSM
```cpp
intensity = pers->getIntensityMultiplier(pers->base_aggression);
// All eyes use same intensity
```

---

❌ **WRONG**: Hardcode personality values in eye component
```cpp
float intensity = 1.5f;  // Hardcoded - can't change!
```

✅ **RIGHT**: Read personality from SharedMemory
```cpp
Personality* pers = GSM.read<Personality>();
float intensity = pers->getIntensityMultiplier(pers->base_aggression);
```
