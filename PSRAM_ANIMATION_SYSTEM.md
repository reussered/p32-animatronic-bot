# PSRAM Animation System for Goblin Eyes

## Overview
Complete rewrite of the eye animation system to use ESP32-S3's 8MB PSRAM for storing multiple full-frame animation sequences. This replaces the old 4-frame static SRAM blink system with a rich expression library.

## Memory Architecture

### Old System (commit f6831ae)
- **Location**: Static SRAM allocation
- **Size**: 57,600 bytes per eye (115,200 bytes total)
- **Frames**: 4 frames (blink cycle only)
- **Format**: Palette-indexed (1 byte/pixel) converted to RGB565 after mood processing
- **Limitation**: Only one animation type, consumed significant SRAM

### New System (current)
- **Location**: PSRAM (MALLOC_CAP_SPIRAM)
- **Size**: 3.3 MB per eye (6.6 MB total for both eyes)
- **Frames**: 29 frames per eye (7 animation types)
- **Format**: RGB565 (2 bytes/pixel) - direct display format
- **Benefit**: Frees SRAM for other subsystems, enables rich expression library

## Animation Library

### Left Eye (`goblin_left_eye.src`)
| Animation | Frames | Delay | Description |
|-----------|--------|-------|-------------|
| BLINK | 4 | 30 loops | Normal blinking (open → partial → nearly closed → closed) |
| STARTLE | 5 | 15 loops | Surprised expression with dilating pupil (gradual eye widening) |
| CONTENT | 4 | 50 loops | Relaxed half-closed eye with slight variation |
| DART | 5 | 20 loops | Side-to-side eye movement (center → left → far left → right → far right) |
| SLEEP | 3 | 60 loops | Closed eye with REM flutter in frame 1 |
| CURIOUS | 4 | 25 loops | Investigative look with constricting pupil (focused) |
| SUSPICIOUS | 4 | 35 loops | Narrowed eye looking sideways (+20 pixels right) |

### Right Eye (`goblin_right_eye.src`)
- **Identical frame counts and timing** to left eye
- **Mirrored dart animation**: moves right first, then left
- **Mirrored suspicious animation**: looks left (-20 pixels) vs right eye looking right

### Total Memory Usage
```
Left eye:   29 frames × 115,200 bytes = 3,340,800 bytes (~3.3 MB)
Right eye:  29 frames × 115,200 bytes = 3,340,800 bytes (~3.3 MB)
Total eyes: 6,681,600 bytes (~6.4 MB) of 8 MB PSRAM available
Remaining:  ~1.6 MB for mouth animations and future expansion
```

## Data Structures

### Animation Structure
```cpp
typedef struct {
    uint16_t* frames;        // Pointer to animation frames in PSRAM (RGB565)
    uint8_t frame_count;     // Number of frames in animation
    uint8_t current_frame;   // Current frame index
    uint32_t frame_delay;    // Loops between frame changes
    uint32_t last_update;    // Last loop count when frame changed
    const char* name;        // Animation name for logging
} Animation;
```

### Animation Types
```cpp
enum AnimationType {
    ANIM_BLINK = 0,
    ANIM_STARTLE,
    ANIM_CONTENT,
    ANIM_DART,
    ANIM_SLEEP,
    ANIM_CURIOUS,
    ANIM_SUSPICIOUS,
    ANIM_COUNT
};
```

## Component Flow

### Initialization Sequence (at boot)
1. **goblin_left_eye_init()**:
   - Allocate 3.3 MB in PSRAM for 29 frames
   - Call procedural generation for all 7 animation types
   - Set `current_display_buffer` to first frame of BLINK
   
2. **goblin_right_eye_init()**:
   - Allocate another 3.3 MB in PSRAM for 29 frames
   - Call mirrored procedural generation for all 7 animation types
   - Set `current_right_display_buffer` to first frame of BLINK

3. **goblin_eye_init()**:
   - Initialize mood tracking
   - Set up `goblin_mood_effects[]` array (9 mood components)

### Runtime Sequence (every loop)
1. **goblin_left_eye_act()** (hitCount = 1):
   - Check if `frame_delay` loops elapsed
   - Advance to next frame: `current_frame = (current_frame + 1) % frame_count`
   - Update `current_display_buffer` pointer to new frame in PSRAM

2. **goblin_eye_act()** (hitCount = 1):
   - Read `Mood` from GSM (Global Shared Memory)
   - Call `adjustMood<Pixel_RGB565>()` on `current_display_buffer`
   - Applies color shifts in-place (modifies PSRAM data)

3. **generic_spi_display_act()** (hitCount = TBD):
   - TODO: Implement dual-DMA pipeline
   - Copy `current_display_buffer` from PSRAM → SRAM DMA buffer
   - Transfer SRAM buffer → GC9A01 display via SPI

4. **goblin_right_eye_act()** (hitCount = 1):
   - Same as left eye but uses `right_animations[]` and `current_right_display_buffer`

## Animation Generation

### Procedural Rendering
All animations are procedurally generated at init time using geometric primitives:

```cpp
void draw_filled_circle(uint16_t* frame, int cx, int cy, int radius, uint16_t color)
void draw_circle(uint16_t* frame, int cx, int cy, int radius, uint16_t color)
void draw_ellipse(uint16_t* frame, int cx, int cy, int rx, int ry, uint16_t color)
```

### Color Palette
- **White (sclera)**: `0xFFFF`
- **Black (eyelid/pupil)**: `0x0000`
- **Brown (iris)**: `0x4A20`
- **Dark gray (REM)**: `0x2104`

### Example: Blink Animation
```cpp
Frame 0: Fully open eye
  - White background (sclera)
  - Brown filled circle radius 70 (iris)
  - Black filled circle radius 30 (pupil)

Frame 1: Partially closed
  - Black rectangle top 80 rows (eyelid)
  - Iris shifted down +10 pixels, radius 65
  - Pupil shifted down +10 pixels, radius 28

Frame 2: Nearly closed
  - Black rectangle top 110 rows (eyelid)
  - Iris shifted down +20 pixels, radius 40
  - No pupil visible

Frame 3: Fully closed
  - All black (closed eye)
```

## Mood Color System

### Integration with Existing adjustMood
The new system **preserves** the existing mood color adjustment logic:
- Animation frames stored in PSRAM are **base patterns** (neutral browns/whites/blacks)
- `goblin_eye_act()` applies mood color shifts **every frame**
- Mood effects defined in `goblin_mood_effects[]` array (9 components × RGB multipliers)
- `adjustMood<Pixel_RGB565>()` modifies buffer in-place with saturation

### Separation of Concerns
| System | Responsibility | Domain |
|--------|---------------|--------|
| Animation | Eye **shape** changes (open/closed/darting) | Time-based |
| Mood | Eye **color** changes (red tint, blue tint, desaturate) | State-based |

This separation allows animations and mood to work **orthogonally**:
- Startle animation can be red-tinged (fear) or yellow-tinged (excitement)
- Blink animation can be any mood color
- No coupling between shape and color

## Animation Selection (TODO)

### Current Behavior
- Both eyes continuously play BLINK animation
- Animation type is hardcoded: `current_animation = ANIM_BLINK`

### Planned Behavior
Read mood/behavior state from GSM and select appropriate animation:
```cpp
void goblin_left_eye_act(void)
{
    // Read mood from GSM
    Mood* mood_ptr = GSM.read<Mood>();
    if (mood_ptr) {
        // Select animation based on dominant mood component
        if (mood_ptr->fear > 50) {
            current_animation = ANIM_STARTLE;
        }
        else if (mood_ptr->contentment > 60) {
            current_animation = ANIM_CONTENT;
        }
        else if (mood_ptr->curiosity > 40) {
            current_animation = ANIM_CURIOUS;
        }
        // ... other mappings
        else {
            current_animation = ANIM_BLINK;  // Default
        }
    }
    
    // Continue with frame advancement...
}
```

### Behavior Triggers
Consider adding triggers for specific animations:
- **DART**: When `attention_target` changes rapidly in Environment
- **SLEEP**: When `energy_level` in SysTest drops below threshold
- **SUSPICIOUS**: When `unknown_entity` detected in Environment

## File Structure

### Modified Files
- `config/bots/bot_families/goblins/head/goblin_left_eye.src`
  - Complete rewrite: PSRAM allocation + 7 animation generators + helpers
  
- `config/bots/bot_families/goblins/head/goblin_right_eye.src`
  - Complete rewrite: Mirrored PSRAM system for right eye
  
- `config/bots/bot_families/goblins/head/goblin_eye.src`
  - Updated to use `current_display_buffer` pointer instead of `front_buffer`
  - Removed buffer size checks (PSRAM buffers are always full-frame)

### Unchanged Files
- `config/shared_headers/PixelType.hpp` - adjustMood template works as-is
- `include/with.hpp` - Mood class unchanged
- `shared/mood.hpp` - Mood processing unchanged

## Build Integration

### Regenerate Tables
After modifying `.src` files, must regenerate dispatch tables:
```powershell
python tools/generate_tables.py goblin_full src
.\generate_file_structure.ps1
```

### Build & Flash
```powershell
pio run -e goblin_head > build.log 2>&1
Select-String -Path build.log -Pattern "error|failed|success"
pio run -e goblin_head -t upload > upload.log 2>&1
```

### Expected Serial Output
```
[goblin_left_eye] Initializing left eye animation system in PSRAM
[goblin_left_eye] Allocating 3340800 bytes (29 frames) in PSRAM
[goblin_left_eye] Generating animation sequences...
[goblin_left_eye] Generated blink animation
[goblin_left_eye] Generated startle animation
[goblin_left_eye] Generated contentment animation
[goblin_left_eye] Generated dart animation
[goblin_left_eye] Generated sleep animation
[goblin_left_eye] Generated curious animation
[goblin_left_eye] Generated suspicious animation
[goblin_left_eye] Left eye animation system initialized (position: -50,30,-35 mm)

[goblin_right_eye] Initializing right eye animation system in PSRAM
[goblin_right_eye] Allocating 3340800 bytes (29 frames) in PSRAM
[goblin_right_eye] Generating animation sequences...
... (same as left eye)
[goblin_right_eye] Right eye animation system initialized (position: 50,30,-35 mm)

[goblin_eye] Initializing goblin eye mood processing
[goblin_eye] Goblin eye initialized (emotion intensity: 1.5x)
```

## Performance Characteristics

### Memory Access Patterns
- **PSRAM Read**: ~40-80 ns per access (slower than SRAM)
- **SRAM Read**: ~5 ns per access
- **Frame Pointer Update**: 1 instruction (just pointer arithmetic)
- **Mood Processing**: Processes 57,600 pixels every frame (when mood changes)

### CPU Load
- **Frame Advancement**: Minimal (comparison + modulo + pointer update)
- **Animation Selection**: Minimal (read GSM + comparison)
- **Mood Color Adjustment**: Medium (57,600 pixel iterations with float math)
- **Generation at Init**: One-time cost (~500ms total for all animations)

### Optimization Opportunities
1. **Double Buffering**: Keep mood-adjusted copy in SRAM to avoid PSRAM→SRAM copies
2. **Partial Updates**: Only adjust mood on changed regions
3. **Frame Skipping**: Lower frame_delay values for smoother animation
4. **Animation Blending**: Crossfade between animations for smooth transitions

## Future Expansion

### Mouth Animations
- **Size**: 320×240 ILI9341 = 153,600 bytes per frame
- **Available**: ~1.6 MB remaining PSRAM
- **Capacity**: ~10 frames (enough for 4 mouth animations)

### Suggestions
1. **Mouth Open** (3 frames): closed → partial → full open
2. **Mouth Smile** (3 frames): neutral → slight smile → wide smile
3. **Mouth Frown** (2 frames): neutral → frown
4. **Mouth Talk** (4 frames): phoneme approximations for lip-sync

### Animation Variants
Consider per-personality variants:
- Aggressive goblin: Faster blink (delay = 20), more startle
- Lazy goblin: Slower blink (delay = 40), more sleep
- Curious goblin: More dart/curious animations

## Technical Notes

### Single Compilation Unit
- All `.src` files aggregate into one `goblin_head.cpp`
- `draw_filled_circle()` defined in `goblin_left_eye.src` is visible to `goblin_right_eye.src`
- No need to `#include` headers between `.src` files
- File-scoped `static` variables share within subsystem

### PSRAM Allocation
```cpp
heap_caps_malloc(size, MALLOC_CAP_SPIRAM)
```
- Allocates from external PSRAM, not internal SRAM
- Slower access but much larger capacity
- Survives deep sleep (if configured)
- No DMA capability (must copy to SRAM first)

### RGB565 Format
```
Bits: 15-11 (R), 10-5 (G), 4-0 (B)
0x4A20 = 0100 1010 0010 0000
       = R:01001 (9), G:010000 (16), B:00000 (0)
       = RGB888(72, 64, 0) ≈ Brown
```

## Comparison Table

| Feature | Old System (f6831ae) | New System (current) |
|---------|---------------------|----------------------|
| Memory Type | Static SRAM | PSRAM |
| Memory Per Eye | 57,600 bytes | 3,340,800 bytes |
| Total Memory | 115,200 bytes | 6,681,600 bytes |
| Animation Types | 1 (blink only) | 7 (multiple expressions) |
| Total Frames | 4 | 29 per eye |
| Pixel Format | Palette (1 byte) | RGB565 (2 bytes) |
| Generation | Procedural per-frame | Procedural at init |
| Mood System | Separate layer | Same (preserved) |
| Frame Selection | Time-based modulo | Time-based with delay |
| SRAM Pressure | High (115KB) | Low (pointers only) |
| Expression Range | Limited | Rich |

## Conclusion

The new PSRAM animation system provides:
✅ **7× more animation variety** (7 types vs 1)
✅ **7× more frames** (29 vs 4 per eye)
✅ **Frees 115 KB SRAM** for other subsystems
✅ **Preserves mood color system** exactly as-is
✅ **Room for expansion** (1.6 MB PSRAM remaining)

The procedural generation approach keeps code size small while enabling rich expression without storing pre-rendered assets.
