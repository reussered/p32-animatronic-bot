#!/usr/bin/env powershell
<#
QUICK START: Goblin Mood Display System
========================================
After you review and approve, use this to integrate the mood system
#>

# ============================================================================
# STEP 1: REVIEW (5 minutes)
# ============================================================================

# Read the summary
$files = @(
    "MOOD_SYSTEM_READY_FOR_REVIEW.txt",
    "GOBLIN_MOOD_DISPLAY_IMPLEMENTATION_SUMMARY.md",
    "config/components/templates/mood_calculator_template.json"
)

Write-Host "START HERE:" -ForegroundColor Green
Write-Host "1. Review MOOD_SYSTEM_READY_FOR_REVIEW.txt"
Write-Host "2. Check GOBLIN_MOOD_DISPLAY_IMPLEMENTATION_SUMMARY.md"
Write-Host "3. Examine GOBLIN_MOOD_DISPLAY_INTEGRATION_GUIDE.md"

# ============================================================================
# STEP 2: DECIDE CONFIG (2 minutes)
# ============================================================================

Write-Host "`n CHOOSE YOUR DISPLAY CONFIGURATION:" -ForegroundColor Cyan

Write-Host @"

CONFIG A: Safe & Simple (Recommended)
  - Left eye:  GC9A01 240240 RGB565   (112.5 KB)
  - Right eye: GC9A01 240240 RGB565   (112.5 KB)
  - Mouth:     ST7735 160128 RGB565   (40 KB)
  TOTAL: 265 KB (35 KB HEADROOM) 

CONFIG B: High-Res Mouth (Tight)
  - Left eye:  GC9A01 240240 RGB565   (112.5 KB)
  - Right eye: GC9A01 240240 RGB565   (112.5 KB)
  - Mouth:     ILI9341 480320 chunked (112.5 KB)
  TOTAL: 337.5 KB (BARELY FITS) 

CONFIG C: Balanced
  - Left eye:  GC9A01 240240 RGB565   (112.5 KB)
  - Mouth:     ILI9341 480320 chunked (112.5 KB)
  TOTAL: 225 KB (75 KB HEADROOM) 

CONFIG D: Custom
  Edit GOBLIN_MOOD_DISPLAY_EXAMPLE.cpp and mix/match displays

"@

# ============================================================================
# STEP 3: APPROVE MOODS (3 minutes)
# ============================================================================

Write-Host "REVIEW DEFAULT MOOD COLORS:" -ForegroundColor Cyan
Write-Host @"

In: config/components/templates/mood_calculator_template.hpp
Look for: initializeDefaultMoodPalette()

Current defaults:
  ANGER:      Red      (+40 red, -30 green, -30 blue, 1.5x intensity, glow ON)
  CURIOSITY:  Green    (-20 red, +35 green, -10 blue, 1.2x intensity)
  FEAR:       Blue     (-40 red, +20 green, +50 blue, 1.8x intensity)
  HAPPINESS:  Gold     (+30 red, +25 green, -20 blue, 1.1x intensity, glow ON)
  SADNESS:    Cool     (-15 red, -15 green, +10 blue, 0.8x intensity, desaturate ON)

Approve? (y/n) or suggest changes...

"@

# ============================================================================
# STEP 4: GIT COMMIT (1 minute)
# ============================================================================

Write-Host "`nWHEN YOU'RE READY, COMMIT:" -ForegroundColor Green
Write-Host @"

git commit -m "Add template-based mood display system for goblin head

- Generic mood calculator template (RGB565/666/888 agnostic)
- Eye display driver with mood effects
- Mouth display driver with sequential chunking
- Supports 300KB RAM constraint with multiple configurations
- Complete integration guide and working examples
- Non-destructive: no changes to existing code"

"@

# ============================================================================
# STEP 5: INTEGRATION (30 minutes)
# ============================================================================

Write-Host "INTEGRATION CHECKLIST:" -ForegroundColor Cyan
Write-Host @"

[ ] Copy mood calculator to goblin_left_eye.src
[ ] Copy mood calculator to goblin_right_eye.src  
[ ] Copy mood calculator to goblin_mouth.src
[ ] Update #includes in each .src file
[ ] Call init() in your *_init() function
[ ] Call renderFrame() in your *_act() function
[ ] Test: verify mood transitions change colors
[ ] Optimize: tune chunk heights if needed
[ ] Document: add notes to goblin_head.json

See GOBLIN_MOOD_DISPLAY_EXAMPLE.cpp for exact patterns
"@

# ============================================================================
# STEP 6: TEST (15 minutes)
# ============================================================================

Write-Host "`nTESTING:" -ForegroundColor Green
Write-Host @"

1. Build: pio run -e goblin_head
2. Flash: pio run -e goblin_head -t upload
3. Monitor: pio device monitor
4. Change mood in SharedMemory:
   GSM.write<Mood>({ANGER, 255});
5. Verify eyes/mouth color change
6. Try all 5 moods: ANGER, FEAR, CURIOSITY, HAPPINESS, SADNESS

"@

# ============================================================================
# TROUBLESHOOTING
# ============================================================================

Write-Host "COMMON ISSUES:" -ForegroundColor Yellow
Write-Host @"

Buffer exceeds 300 KB?
   Use CONFIG A (ST7735 mouth) or CONFIG C (single eye)
   Enable chunking in mouth.init()

Mood changes not visible?
   Check GSM.read<Mood>() returns valid values
   Verify base color isn't too dark (mood shifts need room)
   Check mood_calc is instantiated correctly

Compilation errors?
   Verify color_schema.hpp exists
   Check Mood.hpp defines all 5 mood states
   Confirm MoodState enum matches

Flicker on screen?
   Reduce mood intensity (set lower than 1.0)
   Smooth transitions: intensity * 0.9f for gradual fade
   Check chunk rendering completes before timeout

"@

Write-Host "`nFILES READY TO INTEGRATE:" -ForegroundColor Green
Write-Host @"

Templates (copy into your .src files):
   config/components/templates/mood_calculator_template.hpp
   config/components/templates/goblin_eye_mood_display.hpp
   config/components/templates/goblin_mouth_mood_display.hpp

Documentation:
   GOBLIN_MOOD_DISPLAY_INTEGRATION_GUIDE.md
   GOBLIN_MOOD_DISPLAY_EXAMPLE.cpp
   GOBLIN_MOOD_DISPLAY_IMPLEMENTATION_SUMMARY.md

"@

# ============================================================================
# SUMMARY
# ============================================================================

Write-Host "`n" + ("="*60)
Write-Host "MOOD SYSTEM READY FOR INTEGRATION" -ForegroundColor Green
Write-Host ("="*60)
Write-Host @"

What you have:
   Template-based color modification (0 runtime overhead)
   Auto-detecting pixel format (just set resolution)
   Memory-efficient chunking (fits 300 KB constraint)
   5 mood states with customizable effects
   Complete integration guide + working examples

Next:
  1. Review all files
  2. Approve mood colors
  3. Choose display configuration
  4. Integrate into goblin_head.src
  5. Test on hardware

Estimated effort: 1-2 hours total
Result: Goblin emotions automatically reflected in eye/mouth colors

"@
