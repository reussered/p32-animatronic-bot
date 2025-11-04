# Goblin Animatronic - Implementation Summary

**Status**: Approved compromise architecture  
**Servo Count**: 22 (Config B - Standard, recommended)  
**Cost**: ~$167 per creature  
**Power**: 8.76W running, 17.5W peak @ 6V  

---

## Quick Reference: 22 Servos Needed

### Head (7 servos)
- 4 × Eye pan/tilt (2 per eye, horizontal+vertical) - **LP $4 each**
- 2 × Eyebrow raise (left/right independent) - **LP $4 each**
- 1 × Mouth open/close (jaw hinge) - **MP $5**

### Arms (8 servos - 4 per arm)
- 2 × Shoulder pitch (reach forward/back) - **HP $6 each**
- 2 × Shoulder yaw (raise/lower) - **HP $6 each**
- 2 × Elbow flex (bend arm) - **HP $6 each**
- 2 × Wrist rotate (twist forearm) - **MP $5 each**

### Hands (4 servos - 2 per hand)
- 2 × Hand curl (ALL 5 fingers together via linkage) - **HP $6 each**
- 2 × Thumb opposition (in/out of palm) - **LP $4 each**

### Torso (3 servos)
- 1 × Lean pitch (forward/back tilt) - **HP $6**
- 1 × Twist yaw (rotate at waist) - **MP $5**
- 1 × Breath heave (chest expansion) - **MP $5**

### Neck (Optional, 2 servos for v2)
- 1 × Pitch (nod head) - **MP $5**
- 1 × Yaw (turn head) - **MP $5**

---

## The Hand Compromise (CRITICAL)

### Reality: Cannot do individual fingers
- Would need 1 servo per joint = ~14 servos per hand
- Total for both hands: 28 servos (impossible power budget)
- Cost would be prohibitive

### Solution: Coupled finger control
```
Single servo controls ALL 5 fingers via central linkage bar
- Opens: All fingers extend together (flat hand)
- Closes: All fingers curl together (fist)
- Plus: Thumb on independent servo for opposition (thumbs up gesture)

Result: Believable hand gestures without individual articulation
```

---

## Component Files to Create (22 total)

### Head subsystem: 7 files
```
goblin_eye_left_horizontal.json/.src/.hdr
goblin_eye_left_vertical.json/.src/.hdr
goblin_eye_right_horizontal.json/.src/.hdr
goblin_eye_right_vertical.json/.src/.hdr
goblin_eyebrow_left_raise.json/.src/.hdr
goblin_eyebrow_right_raise.json/.src/.hdr
goblin_mouth_open_close.json/.src/.hdr
```

### Arms subsystem: 8 files
```
goblin_arm_left_shoulder_pitch.json/.src/.hdr
goblin_arm_left_shoulder_yaw.json/.src/.hdr
goblin_arm_left_elbow_flex.json/.src/.hdr
goblin_arm_left_wrist_rotate.json/.src/.hdr
goblin_arm_right_shoulder_pitch.json/.src/.hdr
goblin_arm_right_shoulder_yaw.json/.src/.hdr
goblin_arm_right_elbow_flex.json/.src/.hdr
goblin_arm_right_wrist_rotate.json/.src/.hdr
```

### Hands subsystem: 4 files
```
goblin_hand_left_curl.json/.src/.hdr
goblin_hand_left_thumb_opposition.json/.src/.hdr
goblin_hand_right_curl.json/.src/.hdr
goblin_hand_right_thumb_opposition.json/.src/.hdr
```

### Torso subsystem: 3 files
```
goblin_torso_lean_pitch.json/.src/.hdr
goblin_torso_twist_yaw.json/.src/.hdr
goblin_torso_breath_heave.json/.src/.hdr
```

**Total: 22 × 3 (JSON + SRC + HDR) = 66 files**

---

## Servo Type Distribution

| Type | Purpose | Count | Cost/Each | Total |
|------|---------|-------|-----------|-------|
| **LP** | Eyes, eyebrows, thumbs (low power) | 6 | $4 | $24 |
| **MP** | Mouth, wrists, torso, neck | 8 | $5 | $40 |
| **HP** | Arms, hand curl (load-bearing) | 7 | $6 | $42 |
| | | **TOTAL** | | **$106** |

Plus: Controller ($16) + Power supply ($20) + Wiring ($25) = **$167 total**

---

## Power Budget (Config B - Standard, 22 servos)

| Metric | Idle | Running | Peak Stall |
|--------|------|---------|-----------|
| **Current** | 0.95A | 1.46A | 2.9A |
| **Power** | 5.7W | 8.76W | 17.5W |
| **Supply** | 2A OK | 2A OK | 3A recommended |

**Conclusion**: Single 6V 3A external power supply is sufficient.

---

## Control Architecture

### I2C Servo Control (Recommended)
```
ESP32 → I2C Bus (SDA/SCL) → PCA9685 Servo Controller (16-ch) 
                          → PCA9685 Servo Controller (16-ch)
                          
Total: 32 channels available (22 used for goblin, 10 margin)
```

**Why I2C?**
- Only 2 wires needed (SDA/SCL)
- Can control 32 servos per ESP32
- Standard library support
- Cascadable for multiple creatures

---

## Animation Capabilities

### ✓ CAN DO:
- Full head control (eyes pan/tilt, eyebrows, mouth)
- Arm gestures (reach, wave, scratch)
- Hand gestures (fist, open, thumbs up)
- Body language (lean, twist, breathe)
- Complex expressions (angry, happy, confused)
- Realistic laugh (mouth + breathing + arm wiggle)

### ✗ CANNOT DO:
- Individual finger articulation
- Complex manipulation (picking up objects)
- Leg movement (static in v1)
- Fine facial details (nose twitch, lip sync)

---

## What NOT to Create

**Avoid these temptations:**
- ❌ Individual finger servos (1-2 per finger)
- ❌ Per-toe control
- ❌ Micro-expression servos
- ❌ Separate head servos for ears (can add later)
- ❌ More than 1 servo per arm joint

**These would break the power budget and cost model.**

---

## Scaling to All 13 Families

If goblin uses 22 servos:
- **13 families × 22 servos = 286 servos**
- **286 × $6 average = $1,716** (servos only)
- **Controllers, power supplies, wiring = +$500**
- **Total: ~$2,200 for full animatronic fleet**

**This is achievable and reasonable for a complete project.**

---

## File Organization

```
config/bots/bot_families/goblins/
├── goblin_family.json (base creature template)
│
├── head/
│   ├── goblin_eye_left_horizontal.json
│   ├── goblin_eye_left_horizontal.src
│   ├── goblin_eye_left_horizontal.hdr
│   ├── [... 6 more eye/brow/mouth files ...]
│
├── arms/
│   ├── goblin_arm_left_shoulder_pitch.json
│   ├── goblin_arm_left_shoulder_pitch.src
│   ├── [... 7 more arm files ...]
│
├── hands/
│   ├── goblin_hand_left_curl.json
│   ├── goblin_hand_left_curl.src
│   ├── [... 3 more hand files ...]
│
└── torso/
    ├── goblin_torso_lean_pitch.json
    ├── goblin_torso_lean_pitch.src
    ├── [... 2 more torso files ...]
```

---

## Next Action: Generate Component Templates

**Ready to create the 22 component definition files** (`.json` + `.src` + `.hdr`) for goblin.

Each file will include:
- **JSON**: Servo specs, PWM ranges, timing, pin assignment
- **SRC**: C code for init() and act() functions
- **HDR**: Function declarations and structs

**Recommend**: Start with head subsystem (7 components), then arms (8), then hands (4), then torso (3).

---

## Summary

✓ **Compromise accepted**: Coupled finger control via single servo  
✓ **22 servos**: Balanced between capability and feasibility  
✓ **$167 cost**: Per-creature budget  
✓ **8.76W running**: Power-efficient design  
✓ **66 component files**: To create and maintain  
✓ **Repeatable**: Same architecture for all 13 families  

**Ready to proceed with component generation.**

