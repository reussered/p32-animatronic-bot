# Offline Agent Task: Complete Goblin Eye Display Pipeline

## Task Status: READY FOR OFFLINE AGENT
**Created**: 2025-11-06  
**Priority**: High  
**Estimated Effort**: Medium  

---

## Overview

Complete the goblin eye display rendering pipeline. The architecture is designed, templates are written, and component skeletons exist. This task consolidates all components into a working system.

## What's Already Done ✅

### Architecture & Templates
- ✅ `config/shared_headers/color_schema.hpp` - All 6 pixel formats with `fromBytes()`/`toBytes()` + saturating `+=` operators
- ✅ `config/shared_headers/mood_effects.hpp` - Template `adjustMood<PixelType>()` using clean casting interface
- ✅ Documentation: `CLEAN_POINTER_CASTING_ARCHITECTURE.md`, `PIXEL_CASTING_INTERFACE_COMPLETE.md`

### Component Allocation (Positioned Components)
- ✅ `config/bots/bot_families/goblins/head/goblin_left_eye.src` - Allocates `unsigned char* display_buffer`
- ✅ `config/bots/bot_families/goblins/head/goblin_right_eye.src` - Allocates `unsigned char* display_buffer`
- ✅ `config/bots/bot_families/goblins/head/goblin_mouth.src` - Allocates `unsigned char* display_buffer`

### Component Processing
- ✅ `config/bots/bot_families/goblins/head/goblin_eye.src` - Calls `adjustMood<Pixel_RGB565>()` template with `goblin_mood_effects[]`

## What Needs to be Done

### 1. Verify Component Chain Integration

**File**: `config/bots/bot_families/goblins/head/goblin_eye.src`

**Task**: Ensure `display_buffer` symbol is correctly resolved from the positioned component.

**Issue**: The `goblin_eye.src` file references `display_buffer` which is `static` in the positioned component (left_eye, right_eye, or mouth). The build system concatenates `.src` files into a single compilation unit, so file-scoped `static` variables are visible across the component chain.

**Verification Steps**:
1. Check that `goblin_eye.src` is listed **after** the positioned component in the JSON configuration
   - Example order: `goblin_left_eye` → `goblin_eye` → `generic_spi_display`
2. Verify build output for linker errors related to `display_buffer`
3. Test on hardware or simulator to confirm mood effects render

**Reference**: See `.github/CONSOLIDATED_PROJECT_RULES.md` section on "Single Compilation Unit"

### 2. Verify Pixel Format Casting

**File**: `config/shared_headers/color_schema.hpp` (all 6 pixel formats)

**Task**: Confirm `fromBytes()` and `toBytes()` are present and correct.

**Verification Steps**:
```cpp
// For each format (RGB565, RGB444, RGB555, RGB666, RGB888, Grayscale):
// 1. Check static fromBytes() exists
static Pixel_RGB565* fromBytes(uint8_t* byte_ptr);
static const Pixel_RGB565* fromBytes(const uint8_t* byte_ptr);

// 2. Check instance toBytes() exists
uint8_t* toBytes();
const uint8_t* toBytes() const;

// 3. Check saturating operator+=
Pixel_RGB565& operator+=(const Pixel_RGB565& other);
```

### 3. Verify Mood System Integration

**File**: `include/core/memory/SharedMemory.hpp`

**Task**: Ensure `Mood` class is registered with type ID and can be read via `GSM.read<Mood>()`.

**Verification Steps**:
1. Confirm `Mood` has a unique type ID (check existing IDs to avoid collisions)
2. Verify `GSM.read<Mood>()` compiles and returns valid pointer
3. Check `Mood` is broadcasted from torso to head via ESP-NOW

**Reference**: `goblin_eye_act()` calls `Mood* mood_ptr = GSM.read<Mood>();`

### 4. Build System Verification

**File**: Component JSON configuration files

**Task**: Verify component ordering and dependencies in the build chain.

**Checklist**:
- [ ] Positioned component (left_eye/right_eye/mouth) appears **before** `goblin_eye` in component list
- [ ] `goblin_eye` appears **before** `generic_spi_display` in component list
- [ ] `generic_spi_display` is configured to send buffer and free it
- [ ] Build script (`tools/generate_tables.py`) correctly concatenates components

**Command to Test**:
```powershell
python tools/generate_tables.py goblin_full src
# Check for errors in generated src/components/*.cpp
.\generate_file_structure.ps1
```

### 5. Runtime Testing

**Test Scenario**: Display renders mood-based color effects on goblin eyes.

**Setup**:
1. Build and flash firmware to head ESP32-S3
2. Monitor logs for initialization messages
3. Trigger mood changes (via serial command or JSON broadcast)
4. Observe display color changes matching mood intensity

**Expected Behavior**:
- Anger (high) → Red tint with reduced green/blue
- Happiness (high) → Yellow/warm tint
- Fear (high) → Blue tint with reduced saturation
- Colors shift smoothly as mood components change

**Debug Points**:
- Check `goblin_eye_init()` completes without errors
- Verify `goblin_eye_act()` processes each mood update
- Confirm `adjustMood<Pixel_RGB565>()` template instantiates correctly
- Monitor display buffer memory usage (~115 KB per eye)

### 6. Edge Cases & Error Handling

**Verify**:
- [ ] Null buffer pointer handling in `adjustMood<>()`
- [ ] Zero pixel count handling
- [ ] Mood component saturation (clamped to [-128, +127])
- [ ] Color delta saturation (clamped per pixel format)
- [ ] Buffer allocation failure recovery

### 7. Clean Up Obsolete Files (If Present)

**Remove**:
- `shared/Personality.hpp` (if exists) - Personality system was replaced with simpler architecture
- `include/Personality.hpp` (if exists) - Same
- `config/bots/bot_families/goblins/torso/goblin_personality.src` (if exists) - Same
- Old personality-related documentation files

**Why**: These were part of an earlier architectural direction that was corrected to use simpler template-based mood rendering.

---

## Key Concepts to Understand

### Generic-to-Specific Pointer Casting

```cpp
// ALLOCATE as generic bytes (positioned component)
unsigned char* display_buffer = malloc(size);

// TRANSFORM to specific type at point of use (processor)
Pixel_RGB565* pixels = Pixel_RGB565::fromBytes(display_buffer);

// USE with type safety
pixels[i] += color_delta;  // Uses operator+= for saturating addition

// TRANSFORM back to generic (deallocator)
free(display_buffer);
```

### Template Instantiation Through Formats

The `adjustMood<PixelType>()` template works with any pixel format that has:
1. `PixelType::fromBytes(uint8_t*)` - Static factory method
2. `operator+=(const PixelType&)` - Saturating arithmetic
3. `.r`, `.g`, `.b` members (or equivalent for Grayscale)

Example instantiations:
```cpp
adjustMood<Pixel_RGB565>(...);   // For GC9A01 displays
adjustMood<Pixel_RGB888>(...);   // For AMOLED displays
adjustMood<Pixel_Grayscale>(...); // For e-ink displays
```

### Single Compilation Unit Architecture

All `.src` files for a subsystem are concatenated into one `.cpp` file:
```
goblin_left_eye.src
+ goblin_eye.src
+ generic_spi_display.src
= src/components/goblins_head_main.cpp
```

Result: File-scoped `static` variables in `goblin_left_eye.src` are visible to `goblin_eye.src`.

---

## Testing Checklist

- [ ] Compile without errors
- [ ] Compile without warnings (or acceptable warnings documented)
- [ ] Flash to ESP32-S3 head controller
- [ ] Display initializes to neutral color (dark green)
- [ ] Mood updates change display colors
- [ ] Color changes match mood intensity multipliers
- [ ] No memory leaks (buffer freed after each frame)
- [ ] Performance acceptable (60+ FPS if SPI supports it)
- [ ] Can switch between left eye, right eye, mouth without issues

---

## Files Modified/Created This Session

1. `config/shared_headers/color_schema.hpp` - Added conversion methods to all 6 pixel formats
2. `config/shared_headers/mood_effects.hpp` - Created `adjustMood<PixelType>()` template
3. `config/bots/bot_families/goblins/head/goblin_left_eye.src` - Simplified to allocation-only
4. `config/bots/bot_families/goblins/head/goblin_right_eye.src` - Updated to allocation-only
5. `config/bots/bot_families/goblins/head/goblin_mouth.src` - Updated to allocation-only
6. `config/bots/bot_families/goblins/head/goblin_eye.src` - Updated to use `adjustMood<>()` template
7. `.github/CLEAN_POINTER_CASTING_ARCHITECTURE.md` - Architecture documentation
8. `.github/PIXEL_CASTING_INTERFACE_COMPLETE.md` - Pixel format interface documentation

---

## References

- **Project Rules**: `.github/CONSOLIDATED_PROJECT_RULES.md`
- **Component System**: `.github/COMPONENT_ORGANIZATION_PLAN.md` or equivalent
- **Mood System**: `shared/Mood.hpp`
- **SharedMemory**: `include/core/memory/SharedMemory.hpp`
- **Color Schema**: `config/shared_headers/color_schema.hpp`

---

## Success Criteria

✅ **Task Complete When**:
1. All components compile without errors
2. Component chain correctly resolves `display_buffer` symbol
3. Display renders initial neutral color without errors
4. Mood changes trigger visible color changes on display
5. Color effects match defined goblin mood mappings
6. No memory leaks detected
7. All edge cases handled gracefully
8. (Optional) Pixel format tested with multiple formats (RGB565, RGB888, etc.)

---

## Notes for Offline Agent

- This is a **system integration task**, not a new feature
- Most of the heavy lifting (templates, architecture) is complete
- Main work: Verification, testing, and fixing linker/integration issues
- Expected issues: Symbol resolution between components, build system quirks
- Reference the copilot-instructions.md for specific rules about this codebase

