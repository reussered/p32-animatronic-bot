# Goblin Servo Architecture & Implementation Plan

**Date**: November 4, 2025  
**Scope**: Complete servo motor specification for a single goblin animatronic  
**Target**: Theme-park quality animation with practical power budget

---

## PART 1: GOBLIN ANATOMY & MOVEMENT GOALS

### Physical Characteristics
- **Head**: Medium size, expressive face (wide mouth, beady eyes)
- **Body**: Squat, muscular build
- **Arms**: Long relative to body, good for gesturing
- **Hands**: Small, crude (don't need fine manipulation)
- **Legs**: Short, can be static or articulated
- **Height**: ~3-4 feet (scaled down from human)

### Desired Animation Capabilities
- **Head**: Look around, emote (eyebrows, mouth corners)
- **Arms**: Reach, gesture, wave, scratch
- **Hands**: Fist curl, open flat, thumb gesture
- **Body**: Lean forward/back, shoulder shrug, laugh (chest heave)
- **Legs**: Optional (assume static for v1)

---

## PART 2: COMPLETE SERVO INVENTORY FOR GOBLIN

### HEAD SUBSYSTEM (7 servos)

#### Eyes (4 servos)
```
goblin_eye_left_horizontal:
  - Joint: Eyeball pan (left-right gaze)
  - Servo: SG90 or LP (low power)
  - Cost: $4
  - Current: 60 mA running, 150 mA stall
  - Range: ±30°
  - Purpose: Look left/right
  - Mounting: Behind eye socket, linkage to eyeball
  
goblin_eye_left_vertical:
  - Joint: Eyeball tilt (up-down gaze)
  - Servo: SG90 or LP
  - Cost: $4
  - Current: 60 mA running, 150 mA stall
  - Range: ±20°
  - Purpose: Look up/down
  - Mounting: Behind eye socket, vertical linkage
  
goblin_eye_right_horizontal:
  - Joint: Eyeball pan (right eyeball)
  - Servo: SG90 or LP
  - Cost: $4
  - Current: 60 mA running, 150 mA stall
  - Range: ±30°
  - Purpose: Right eye gaze
  
goblin_eye_right_vertical:
  - Joint: Eyeball tilt (right eyeball)
  - Servo: SG90 or LP
  - Cost: $4
  - Current: 60 mA running, 150 mA stall
  - Range: ±20°
  - Purpose: Right eye vertical movement
```

#### Eyebrows (2 servos)
```
goblin_eyebrow_left_raise:
  - Joint: Eyebrow raise/lower (expression)
  - Servo: SG90 or LP
  - Cost: $4
  - Current: 60 mA running
  - Range: 0-20° (small movement)
  - Purpose: Angry, surprised, concerned expressions
  - Mechanism: Single linkage to eyebrow platform
  - Note: Coupled movement with right eyebrow possible
  
goblin_eyebrow_right_raise:
  - Joint: Right eyebrow (independent for asymmetric expressions)
  - Servo: SG90 or LP
  - Cost: $4
  - Current: 60 mA running
  - Range: 0-20°
  - Purpose: Express skepticism, confusion
```

#### Mouth (1 servo)
```
goblin_mouth_open_close:
  - Joint: Jaw hinge (open/close mouth)
  - Servo: MP (medium power, needs more torque for jaw)
  - Cost: $5
  - Current: 80 mA running, 300 mA stall
  - Range: 0-60° (closed to wide grin)
  - Purpose: Speech sync, laughing, snarling
  - Mechanism: Direct linkage from servo horn to jaw
  - Note: This is THE key expression servo
```

**Head Subtotal**: 7 servos, ~$32, ~360 mA running current

---

### ARM SUBSYSTEM (8 servos - 4 per arm)

#### Left Arm (4 servos)
```
goblin_arm_left_shoulder_pitch:
  - Joint: Shoulder pitch (forward/backward)
  - Servo: HP (high power, needs torque for arm weight)
  - Cost: $6
  - Current: 100 mA running, 600 mA stall
  - Torque: ~5-8 kg⋅cm (support arm weight + gesture load)
  - Range: ±60° (arm down to arm up)
  - Purpose: Reach toward/away from body
  - Mounting: Shoulder joint, linkage to upper arm
  - Note: CRITICAL load-bearing servo
  
goblin_arm_left_shoulder_yaw:
  - Joint: Shoulder raise/lower (up-down movement)
  - Servo: HP
  - Cost: $6
  - Current: 100 mA running, 600 mA stall
  - Range: ±40° (arm at side to arm raised)
  - Purpose: Wave, shrug, reach up/down
  - Mounting: Shoulder joint, vertical linkage to upper arm
  
goblin_arm_left_elbow_flex:
  - Joint: Elbow bend (upper-lower arm angle)
  - Servo: HP
  - Cost: $6
  - Current: 100 mA running, 600 mA stall
  - Range: 0-120° (arm straight to full curl)
  - Purpose: Bend arm, grab, reach
  - Mounting: Elbow joint, direct linkage to forearm
  
goblin_arm_left_wrist_rotate:
  - Joint: Wrist rotation (twist forearm)
  - Servo: MP (lower torque needed, minimal load)
  - Cost: $5
  - Current: 80 mA running, 300 mA stall
  - Range: ±90° (wrist twist)
  - Purpose: Palm rotation, stylistic movements
  - Mounting: Wrist joint, central shaft linkage
```

#### Right Arm (4 servos)
```
goblin_arm_right_shoulder_pitch:
  - Identical to left arm
  - Cost: $6
  
goblin_arm_right_shoulder_yaw:
  - Identical to left arm
  - Cost: $6
  
goblin_arm_right_elbow_flex:
  - Identical to left arm
  - Cost: $6
  
goblin_arm_right_wrist_rotate:
  - Identical to left arm
  - Cost: $5
```

**Arm Subtotal**: 8 servos, $46, ~720 mA running current

---

### HAND SUBSYSTEM (4 servos - 2 per hand)

#### Left Hand (2 servos)
```
goblin_hand_left_curl:
  - Joint: Finger curl (all 5 fingers together)
  - Servo: HP (needs significant torque for linkage load)
  - Cost: $6
  - Current: 100 mA running, 600 mA stall
  - Torque: ~6-8 kg⋅cm (move linked finger bases)
  - Range: 0-90° (open palm to closed fist)
  - Purpose: Make fist, open hand, grab gesture
  - Mechanism: Central linkage bar connected to all 5 finger bases
  - Note: This is a MECHANICAL compromise (not individual joints)
  
goblin_hand_left_thumb_opposition:
  - Joint: Thumb in/out of palm (opposition movement)
  - Servo: LP (minimal torque)
  - Cost: $4
  - Current: 50 mA running, 150 mA stall
  - Range: 0-45° (thumb pressed to thumb extended)
  - Purpose: Thumbs up, pointing, thumb gesture
  - Mechanism: Cam or simple pivot linkage to thumb joint
```

#### Right Hand (2 servos)
```
goblin_hand_right_curl:
  - Identical to left hand
  - Cost: $6
  
goblin_hand_right_thumb_opposition:
  - Identical to left hand
  - Cost: $4
```

**Hand Subtotal**: 4 servos, $20, ~300 mA running current

---

### TORSO SUBSYSTEM (3 servos)

```
goblin_torso_lean_pitch:
  - Joint: Lean forward/backward (torso pitch)
  - Servo: HP
  - Cost: $6
  - Current: 100 mA running
  - Range: ±30° (lean forward to arch back)
  - Purpose: Emphatic gestures, stumbling, surprise
  - Mounting: Waist joint, linkage to central torso
  
goblin_torso_twist_yaw:
  - Joint: Torso rotation (twist at waist)
  - Servo: MP
  - Cost: $5
  - Current: 80 mA running
  - Range: ±45° (look over shoulder, dodge)
  - Purpose: Lateral movement, looking around
  - Mounting: Waist pivot, central shaft
  
goblin_torso_breath_heave:
  - Joint: Chest expansion/compression (breathing)
  - Servo: MP
  - Cost: $5
  - Current: 80 mA running
  - Range: 0-20° (subtle breathing motion)
  - Purpose: Realistic breathing, laughing heaves
  - Mechanism: Expandable frame or sliding chest plate
  - Note: Subtle but very expressive
```

**Torso Subtotal**: 3 servos, $16, ~260 mA running current

---

### NECK SUBSYSTEM (Optional - 2 servos)

```
goblin_neck_tilt_pitch:
  - Joint: Neck tilt (nod head up/down)
  - Servo: MP
  - Cost: $5
  - Current: 80 mA running
  - Range: ±30°
  - Purpose: Nodding, head movement independent of gaze
  
goblin_neck_tilt_yaw:
  - Joint: Neck turn (look side to side with head)
  - Servo: MP
  - Cost: $5
  - Current: 80 mA running
  - Range: ±40°
  - Purpose: Head look, following gaze
  - Note: Can partially decouple from eye pan for realism
```

**Neck Subtotal (Optional)**: 2 servos, $10, ~160 mA running current

---

## PART 3: COMPLETE GOBLIN SERVO MANIFEST

### Configuration A: MINIMAL (No neck, no torso breathing)

| Subsystem | Component | Servo Type | Cost | Qty |
|-----------|-----------|-----------|------|-----|
| **Head** | Eye horizontal (both) | LP | $4 | 2 |
| | Eye vertical (both) | LP | $4 | 2 |
| | Eyebrows (both) | LP | $4 | 2 |
| | Mouth open/close | MP | $5 | 1 |
| **Arms** | Shoulder pitch (both) | HP | $6 | 2 |
| | Shoulder yaw (both) | HP | $6 | 2 |
| | Elbow flex (both) | HP | $6 | 2 |
| | Wrist rotate (both) | MP | $5 | 2 |
| **Hands** | Hand curl (both) | HP | $6 | 2 |
| | Thumb opposition (both) | LP | $4 | 2 |
| | | | **Total** | **17 servos** |

**Minimal Config Totals**:
- **Servo count**: 17
- **Total cost**: $88
- **Running current**: 1,200 mA (~1.2 A)
- **Peak current**: 2,400 mA (~2.4 A, assuming ~50% stall)
- **Power consumption**: 7.2W running, 14.4W peak @ 6V

---

### Configuration B: STANDARD (Add neck, breathing)

| Subsystem | Component | Servo Type | Cost | Qty |
|-----------|-----------|-----------|------|-----|
| **All from Config A** | | | | 17 |
| **Neck** | Neck pitch (nod) | MP | $5 | 1 |
| | Neck yaw (turn) | MP | $5 | 1 |
| **Torso** | Lean pitch | HP | $6 | 1 |
| | Twist yaw | MP | $5 | 1 |
| | Breathing heave | MP | $5 | 1 |
| | | | **Total** | **22 servos** |

**Standard Config Totals**:
- **Servo count**: 22
- **Total cost**: $121
- **Running current**: 1,460 mA (~1.46 A)
- **Peak current**: 2,920 mA (~2.9 A)
- **Power consumption**: 8.76W running, 17.5W peak @ 6V

---

### Configuration C: DELUXE (Add ear movement, jaw articulation)

Possible additions (not implemented by default):
```
goblin_ear_left_flex: LP servo, $4
goblin_ear_right_flex: LP servo, $4
goblin_jaw_separate_hinge: MP servo, $5 (separate from mouth, allows more expressions)
```

Would bring total to **~28 servos**, **$161 cost**, **~1.8A running current**

---

## PART 4: GOBLIN SERVO COMPONENT FILES TO CREATE

### Essential Components (Config A - Minimal)

**Head Subsystem** (`config/bots/bot_families/goblins/head/`)
```
goblin_eye_left_horizontal.json        (LP servo)
goblin_eye_left_vertical.json          (LP servo)
goblin_eye_right_horizontal.json       (LP servo)
goblin_eye_right_vertical.json         (LP servo)
goblin_eyebrow_left_raise.json         (LP servo)
goblin_eyebrow_right_raise.json        (LP servo)
goblin_mouth_open_close.json           (MP servo)
```

**Arm Subsystem** (`config/bots/bot_families/goblins/arms/`)
```
goblin_arm_left_shoulder_pitch.json    (HP servo)
goblin_arm_left_shoulder_yaw.json      (HP servo)
goblin_arm_left_elbow_flex.json        (HP servo)
goblin_arm_left_wrist_rotate.json      (MP servo)
goblin_arm_right_shoulder_pitch.json   (HP servo)
goblin_arm_right_shoulder_yaw.json     (HP servo)
goblin_arm_right_elbow_flex.json       (HP servo)
goblin_arm_right_wrist_rotate.json     (MP servo)
```

**Hand Subsystem** (`config/bots/bot_families/goblins/hands/`)
```
goblin_hand_left_curl.json             (HP servo)
goblin_hand_left_thumb_opposition.json (LP servo)
goblin_hand_right_curl.json            (HP servo)
goblin_hand_right_thumb_opposition.json (LP servo)
```

**Minimal Total**: 19 component files

### Additional Components (Config B - Standard)

**Neck Subsystem** (`config/bots/bot_families/goblins/head/`)
```
goblin_neck_pitch.json                 (MP servo)
goblin_neck_yaw.json                   (MP servo)
```

**Torso Subsystem** (`config/bots/bot_families/goblins/torso/`)
```
goblin_torso_lean_pitch.json           (HP servo)
goblin_torso_twist_yaw.json            (MP servo)
goblin_torso_breath_heave.json         (MP servo)
```

**Standard Total**: 24 component files

---

## PART 5: POWER AND CONTROL ARCHITECTURE

### Power Distribution

```
POWER ARCHITECTURE (CORRECTED):

Single 6V external power supply (25-30A capable):
├─ Primary Motor Power Bus (HIGH CURRENT, carries actual servo current)
│  └─ Servo Multiplexer / Controller Board
│     ├─ Relay/Motor driver 1 (servos 1-8)
│     ├─ Relay/Motor driver 2 (servos 9-16)
│     ├─ Relay/Motor driver 3 (servos 17-22)
│     └─ Power distribution (6V @ 25-30A to all servos)
│
└─ ESP32 Control Board (LOW CURRENT, logic only)
   ├─ GPIO signal pins (3.3V, ~12mA per pin)
   │  └─ Connected to relay/motor driver control inputs
   └─ Power: 5V @ 100-200mA (separate regulator, not driving motors)

Current Budget (Running):
- 22 servos @ 65mA average = 1,430 mA total
- External supply handles this (25-30A available, ~1.43A needed)
- ESP32 handles: ~22 GPIO pins × 12mA = ~264mA signal current
- ESP32 regulator: Separate 5V supply sufficient (~500mA capacity)

Peak Budget (Stall):
- 22 servos @ 300mA average stall = 6,600 mA = ~6.6A peak
- 25-30A external supply easily handles this
- Sequential control: Move one arm at a time (avoid simultaneous stalls)
- ESP32 completely isolated from motor current (no impact)
```

### Servo Control Strategy

**Option 1: Individual PWM Pins (Requires multiplexer)**
```
Each servo gets dedicated PWM channel
Requires: PWM multiplexer IC (74HC595) or servo controller board
Cost: ~$20-50 per goblin
Benefit: Full independent control of each servo
```

**Option 2: I2C Servo Controller**
```
Single I2C bus controls all 22 servos
Requires: PCA9685 (16-channel) or similar + cascaded boards
Cost: ~$15-30 per goblin
Benefit: Only 2 I2C lines needed (SDA/SCL)
```

**Option 3: Shared SPI Bus + Shift Registers**
```
Serial protocol daisy-chains servo commands
Benefit: Minimal wiring
```

**Recommendation for P32**: Use **PCA9685 I2C servo controller**
- Two 16-channel boards = 32 channels (enough for 22 servos + margin)
- Cost: ~$25 per goblin
- Wiring: Just SDA/SCL + power
- Scalable: Can use same boards for all creatures

---

## PART 6: BEHAVIORAL SEQUENCES FOR GOBLIN

### Predefined Animation Loops

#### "Idle Breathing"
```
Duration: 2 seconds
Sequence:
  t=0.0s:    torso_breath_heave → 0° (exhale)
  t=0.5s:    eyes blink (quick close/open)
  t=1.0s:    torso_breath_heave → 15° (inhale - subtle)
  t=1.5s:    random eyebrow twitch
  t=2.0s:    repeat
```

#### "Aggressive Snarl"
```
Duration: 1 second
Sequence:
  t=0.0s:    mouth_open_close → 45°
             eyebrow_left_raise → 15°
             eyebrow_right_raise → 5°
             arm_left_shoulder_pitch → 30° (lean forward)
  t=0.5s:    torso_lean_pitch → 20°
  t=1.0s:    return to neutral
```

#### "Laugh / Cheer"
```
Duration: 2 seconds
Sequence:
  t=0.0s:    mouth_open_close → 60° (wide)
             torso_breath_heave → 20° (inhale)
  t=0.3s:    torso_lean_pitch → 15° (bounce forward)
  t=0.6s:    hands_curl → 50% (fists half-clenched in glee)
  t=0.9s:    repeat breath pattern
  t=1.2s:    arm oscillate shoulder_pitch ±20° (wiggle arms in excitement)
  t=2.0s:    return to neutral
```

#### "Reaching Gesture"
```
Duration: 1.5 seconds
Sequence:
  t=0.0s:    arm_left_shoulder_pitch → 60° (reach out)
             arm_left_elbow_flex → 30° (arm mostly straight)
             hand_left_curl → 0° (open hand)
             hand_left_thumb_opposition → 0° (thumb out)
  t=0.75s:   hold position
  t=1.5s:    return to neutral
```

---

## PART 7: INTEGRATION WITH PROJECT STRUCTURE

### File Organization

```
config/bots/bot_families/goblins/
├── goblin_family.json
├── goblin_full.json (complete creature definition)
│
├── head/
│   ├── goblin_eye_left_horizontal.json
│   ├── goblin_eye_left_horizontal.src
│   ├── goblin_eye_left_horizontal.hdr
│   ├── goblin_eye_left_vertical.json
│   ├── goblin_eye_left_vertical.src
│   ├── goblin_eye_left_vertical.hdr
│   ├── goblin_eye_right_horizontal.json
│   ├── [... repeat for all eye/eyebrow/mouth components ...]
│   └── goblin_mouth_open_close.hdr
│
├── arms/
│   ├── goblin_arm_left_shoulder_pitch.json
│   ├── goblin_arm_left_shoulder_pitch.src
│   ├── [... all 8 arm components ...]
│
├── hands/
│   ├── goblin_hand_left_curl.json
│   ├── goblin_hand_left_curl.src
│   ├── [... all 4 hand components ...]
│
├── torso/
│   ├── goblin_torso_lean_pitch.json
│   ├── goblin_torso_lean_pitch.src
│   ├── [... 3 torso components ...]
│
└── [Optional: legs/ for future leg animation]
```

### JSON Component Template

```json
{
  "name": "goblin_eye_left_horizontal",
  "creature_family": "goblins",
  "subsystem": "head",
  "joint_type": "servo",
  "servo_type": "lp",
  "servo_model": "SG90 equivalent",
  "hardware_interface": "pwm",
  "pwm_channel": 0,
  "i2c_address": "0x40",
  "i2c_channel": 0,
  "pin_assignment": "dynamic",
  "timing": {
    "hitCount": 3,
    "update_frequency_hz": 50,
    "pulse_width_min_us": 1000,
    "pulse_width_max_us": 2000,
    "center_us": 1500
  },
  "movement": {
    "range_degrees": 60,
    "neutral_position_degrees": 0,
    "min_degrees": -30,
    "max_degrees": 30,
    "speed_degrees_per_second": 90
  },
  "mechanical": {
    "linkage_type": "simple",
    "load_estimate_kg": 0.1,
    "friction_estimate_percent": 15,
    "backlash_degrees": 3
  },
  "power": {
    "voltage": 6,
    "current_idle_ma": 20,
    "current_running_ma": 60,
    "current_stall_ma": 150
  },
  "animation": {
    "default_behavior": "idle_blink",
    "response_to": ["gaze_target", "emotion"]
  }
}
```

---

## PART 8: COST & POWER SUMMARY FOR SINGLE GOBLIN

### Configuration A: Minimal (17 servos)

| Category | Count | Unit Cost | Total Cost |
|----------|-------|-----------|-----------|
| LP Servos (eyes, eyebrows, thumb) | 6 | $4 | $24 |
| MP Servos (mouth, wrist) | 3 | $5 | $15 |
| HP Servos (arms, hand curl) | 6 | $6 | $36 |
| I2C Servo Controller (PCA9685) | 2 | $8 | $16 |
| Power supply (6V 2A) | 1 | $15 | $15 |
| Connectors, wiring, brackets | - | - | $20 |
| **Total** | | | **$126** |

**Power Consumption**:
- Idle: 0.72W
- Running: 7.2W
- Peak: 14.4W

---

### Configuration B: Standard (22 servos)

| Category | Count | Unit Cost | Total Cost |
|----------|-------|-----------|-----------|
| LP Servos | 6 | $4 | $24 |
| MP Servos | 8 | $5 | $40 |
| HP Servos | 7 | $6 | $42 |
| I2C Servo Controller (PCA9685 x2) | 2 | $8 | $16 |
| Power supply (6V 3A) | 1 | $20 | $20 |
| Connectors, wiring, brackets | - | - | $25 |
| **Total** | | | **$167** |

**Power Consumption**:
- Idle: 0.95W
- Running: 8.76W
- Peak: 17.5W

---

## PART 9: ANIMATION CAPABILITY MATRIX

### What This Goblin Can DO

| Animation | Servos Used | Realism | Notes |
|-----------|-----------|---------|-------|
| Look around | Eye H/V (4) | High | Smooth gaze following |
| Express emotion | Eyebrows (2) + Mouth (1) | High | Angry, happy, confused |
| Gesture with arms | Arms (8) | High | Reach, wave, point |
| Make fist/open hand | Hand curl (2) | Medium | All fingers move together |
| Nod head | Neck (2, optional) | High | Independent of eyes |
| Laugh | Mouth (1) + Torso breath (1) | High | Realistic jiggling |
| Point/Thumbs up | Thumb opposition (2) + Elbow (2) | High | Combined gesture |
| Lean forward (aggressive) | Torso lean (1) + Arms | Medium | Threatening posture |

---

### What This Goblin CANNOT DO

| Animation | Why | Notes |
|-----------|-----|-------|
| Pick up objects | No individual finger control | Hands are coupled |
| Write/draw | No precise hand control | Designed for gestures |
| Complex sign language | Limited hand articulation | Needs independent fingers |
| Leg kicking | No leg servos | Static legs (v1) |
| Facial morphing | Limited mid-face control | Only mouth, eyebrows, eyes |
| Independent ear movements | No ear servos (optional) | Can be added later |

---

## PART 10: NEXT STEPS

### Phase 1: Component Template Creation
1. Create 19 base component files (Config A)
2. Generate `.json`, `.src`, `.hdr` for each servo
3. Test PWM control for each servo individually
4. Validate power budget

### Phase 2: Integration Testing
1. Assemble full goblin servo hierarchy
2. Test animation sequences (breathing, look around, snarl)
3. Validate current draw matches predictions
4. Tune servo speeds and ranges

### Phase 3: Mechanical Assembly
1. 3D print goblin head with linkages
2. Install servos in body
3. Calibrate servo centers and ranges
4. Test combined movements

### Phase 4: Animation Programming
1. Create behavior state machine
2. Implement idle loops
3. Add gesture recognition (if using sensors)
4. Test with full animation sequences

---

## CONCLUSION

**A fully functional, expressive goblin animatronic requires:**
- **22 servo motors** (Config B - Standard)
- **$167 in components**
- **~9W running power**, **17.5W peak**
- **24 component definition files**

This is a **realistic, achievable design** that balances:
- ✓ Expression capability (7 independent head features)
- ✓ Gesture capability (8 arm degrees of freedom)
- ✓ Hand interaction (2-servo hand compromise)
- ✓ Power budget (under 20W peak)
- ✓ Cost effectiveness (~$167 per creature)

**For all 13 creature families**, scaling this design = **~2,200 servos**, **$2,171 cost**, repeatable architecture.

