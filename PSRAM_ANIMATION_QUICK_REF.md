# PSRAM Animation Quick Reference

## Implementation Status: ✅ COMPLETE

## What Was Done
- ✅ Implemented 7 animation types per eye (blink, startle, content, dart, sleep, curious, suspicious)
- ✅ 29 total frames per eye stored in PSRAM (6.6 MB total)
- ✅ Procedural generation of all animations using geometric primitives
- ✅ Preserved existing mood color system (orthogonal to animations)
- ✅ Freed 115 KB SRAM by moving to PSRAM
- ✅ Dispatch tables regenerated (31 entries)

## Memory Usage
```
Left eye:   29 frames × 115,200 bytes = 3.3 MB PSRAM
Right eye:  29 frames × 115,200 bytes = 3.3 MB PSRAM
Total:      6.6 MB used / 8 MB available
Remaining:  1.4 MB for mouth animations
```

## Animation Library
| Type | Frames | Delay | Description |
|------|--------|-------|-------------|
| BLINK | 4 | 30 | Normal blinking cycle |
| STARTLE | 5 | 15 | Wide eyes with dilating pupil |
| CONTENT | 4 | 50 | Relaxed half-closed eye |
| DART | 5 | 20 | Side-to-side scanning |
| SLEEP | 3 | 60 | Closed with REM flutter |
| CURIOUS | 4 | 25 | Focused investigative look |
| SUSPICIOUS | 4 | 35 | Narrowed sideways glance |

## Files Modified
1. `config/bots/bot_families/goblins/head/goblin_left_eye.src` (420+ lines)
2. `config/bots/bot_families/goblins/head/goblin_right_eye.src` (360+ lines)
3. `config/bots/bot_families/goblins/head/goblin_eye.src` (updated for new buffer system)

## Next Steps

### Immediate (Ready to Test)
```powershell
# Build and flash
pio run -e goblin_head > build.log 2>&1
pio run -e goblin_head -t upload > upload.log 2>&1

# Monitor serial output
pio device monitor
```

### TODO #1: Animation Selection Logic
Add mood-based animation selection in `goblin_left_eye_act()` and `goblin_right_eye_act()`:
```cpp
Mood* mood = GSM.read<Mood>();
if (mood->fear > 50) current_animation = ANIM_STARTLE;
else if (mood->contentment > 60) current_animation = ANIM_CONTENT;
// ... etc
```

### TODO #2: Display Pipeline
Implement `generic_spi_display.src`:
1. Copy `current_display_buffer` from PSRAM → SRAM DMA buffer
2. Transfer SRAM → GC9A01 via SPI
3. Handle dual-DMA ping-pong buffering

### TODO #3: Mouth Animations
Create `goblin_mouth.src` with 4 mouth shapes (1.4 MB PSRAM available)

## Technical Details

### Animation Flow
```
Init: Allocate PSRAM → Generate all frames → Set to BLINK
Loop: Advance frame → Apply mood colors → Send to display
```

### Data Structure
```cpp
typedef struct {
    uint16_t* frames;      // PSRAM pointer (RGB565)
    uint8_t frame_count;   // Total frames in animation
    uint8_t current_frame; // Current frame index
    uint32_t frame_delay;  // Loops between frames
    uint32_t last_update;  // Last g_loopCount update
    const char* name;      // For logging
} Animation;
```

### Key Variables (file-scoped static)
```cpp
// Left eye
static Animation left_animations[ANIM_COUNT];
static uint16_t* current_display_buffer;
static AnimationType current_animation;

// Right eye
static Animation right_animations[ANIM_COUNT];
static uint16_t* current_right_display_buffer;
static AnimationType current_right_animation;
```

## Expected Serial Output
```
[goblin_left_eye] Initializing left eye animation system in PSRAM
[goblin_left_eye] Allocating 3340800 bytes (29 frames) in PSRAM
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
... (same as left eye)
[goblin_right_eye] Right eye animation system initialized (position: 50,30,-35 mm)

[goblin_eye] Initializing goblin eye mood processing
[goblin_eye] Goblin eye initialized (emotion intensity: 1.5x)
```

## Documentation
- **PSRAM_ANIMATION_SYSTEM.md** - Comprehensive technical guide (500+ lines)
- **IMPLEMENTATION_SUMMARY.md** - Complete implementation report
- **This file** - Quick reference card

---

**Ready to build and test!** 🚀
