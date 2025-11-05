# Flying Dragons Bothome - Reorganization Summary

**Effective Date**: November 5, 2025  
**Operation**: Flattened `flying_dragons/` subfolder into bothome with part-based organization  
**Status**: ✅ COMPLETE

---

## What Changed

### **Before (Nested Structure)**
```
config/bots/bot_families/dragons/
└── flying_dragons/
    ├── flying_dragon_full.json
    ├── README.md
    ├── INDEX.md
    ├── QUICK_REFERENCE.md
    └── hardware_templates/
        ├── T_MOTOR_U3_580KV.json
        ├── HOBBYWING_40A_ESC.json
        ├── MG996R_SERVO.json
        └── ICM20689_BMP390.json
```

### **After (Flattened + Organized)**
```
config/bots/bot_families/dragons/  ← bothome (canonical location)
├── flying_dragon_full.json         ← Master bot definition
├── README.md                       ← Integration guide
├── INDEX.md                        ← JSON reference
├── QUICK_REFERENCE.md              ← Quick lookup
├── BOTHOME_STRUCTURE.md            ← Organizational guide
├── REORGANIZATION_SUMMARY.md       ← This file
│
├── head/                           ← Head sensory components
│   ├── dragon_nostril_left.json
│   └── dragon_nostril_right.json
│
├── wings/                          ← Wing articulation
│   └── [wing servo configs]
│
├── body/                           ← Main body structure
│   └── body_segment/
│
├── motors/                         ← Propulsion system
│   └── [motor control configs]
│
├── sensors/                        ← Sensor fusion
│   └── [sensor configs]
│
├── power/                          ← Battery & safety
│   └── [power management configs]
│
├── control/                        ← Flight logic
│   └── [behavior/state machine configs]
│
├── hardware_templates/             ← Procurement specs (reference)
│   ├── T_MOTOR_U3_580KV.json
│   ├── HOBBYWING_40A_ESC.json
│   ├── MG996R_SERVO.json
│   └── ICM20689_BMP390.json
│
└── legs/                           ← (Empty - for future variants)
```

---

## File Organization Key

### **Root Level** (Master & Documentation)

| File | Purpose | Status |
|------|---------|--------|
| `flying_dragon_full.json` | Master bot definition | ✅ Moved from flying_dragons/ |
| `README.md` | Integration guide (25 KB) | ✅ Moved from flying_dragons/ |
| `INDEX.md` | JSON reference (30 KB) | ✅ Moved from flying_dragons/ |
| `QUICK_REFERENCE.md` | Quick lookup (10 KB) | ✅ Moved from flying_dragons/ |
| `BOTHOME_STRUCTURE.md` | Org guide (this project) | ✅ Created |
| `REORGANIZATION_SUMMARY.md` | This migration doc | ✅ Created |

### **Part-Based Subfolders**

| Folder | Purpose | Contains | Status |
|--------|---------|----------|--------|
| **head/** | Eye, mouth, speaker, nostrils | 2 component files (nostril_left, nostril_right) | ✅ Created |
| **wings/** | Wing flapping articulation | [pending component files] | ✅ Created |
| **body/** | Airframe structure | body_segment/ subfolder | ✅ Created |
| **motors/** | Propulsion motor control | [motor controller config] | ✅ Created |
| **sensors/** | IMU & barometer fusion | [sensor fusion config] | ✅ Created |
| **power/** | Battery & safety systems | [safety & battery monitoring] | ✅ Created |
| **control/** | Flight state machine | [behavior sequencer config] | ✅ Created |
| **hardware_templates/** | Procurement reference | 4 template files | ✅ Moved from flying_dragons/ |
| **legs/** | Leg articulation (future) | Empty | ✅ Created |
| **feet/** | Foot articulation (future) | Empty | ✅ Placeholder |

---

## Hardware Components Location

### **In config/hardware/** (Persistent Location)

Flying dragon hardware specifications are stored in `config/hardware/`:

```
config/hardware/
├── flying_dragon_motor_controller.json      ← 4× T-Motor U3 580KV + ESCs
├── flying_dragon_wing_servo_controller.json ← 6× MG996R servos
├── flying_dragon_sensor_fusion.json         ← ICM20689 + BMP390
├── flying_dragon_flight_safety.json         ← Watchdog + battery monitor
└── flying_dragon_behavior_sequencer.json    ← Flight state machine
```

**These files are the source-of-truth** for all flying dragon hardware specifications.

### **In bothome/hardware_templates/** (Reference Only)

Hardware procurement templates for reference:

```
config/bots/bot_families/dragons/hardware_templates/
├── T_MOTOR_U3_580KV.json      ← Motor specs (580 KV, 950 gf, 55g)
├── HOBBYWING_40A_ESC.json     ← ESC specs (40A continuous)
├── MG996R_SERVO.json          ← Servo specs (11 kg⋅cm, 55g)
└── ICM20689_BMP390.json       ← Sensor specs (1 kHz IMU, 50 Hz barometer)
```

**These are read-only procurement reference documents.**

---

## Data Flow (Updated)

```
┌─────────────────────────────────────────────────────────┐
│ bothome/flying_dragon_full.json (Master Definition)     │
│ Defines subsystems:                                      │
│ ├─ flying_dragon_head (references head/ components)    │
│ └─ flying_dragon_flight_controller (references other/) │
└─────────────────────────────────────────────────────────┘
                           ↓
        ┌──────────────────────────────────────┐
        │   config/hardware/                   │
        │  (Hardware specifications)           │
        │  ├─ motor_controller                 │
        │  ├─ wing_servo_controller            │
        │  ├─ sensor_fusion                    │
        │  ├─ flight_safety                    │
        │  └─ behavior_sequencer               │
        └──────────────────────────────────────┘
                           ↓
        ┌──────────────────────────────────────┐
        │   config/components/subsystem/       │
        │   flying_dragon_flight_system.json   │
        │  (Integrates all 5 flight components)│
        └──────────────────────────────────────┘
```

---

## Migration Process (Completed)

### **Step 1: Create Part-Based Folders** ✅
```powershell
# Created 8 part-based folders in bothome:
# head/, wings/, body/, motors/, sensors/, power/, control/, legs/, feet/
```

### **Step 2: Move Master Definition** ✅
```powershell
# flying_dragon_full.json: flying_dragons/ → bothome/
Move-Item "config/bots/bot_families/dragons/flying_dragons/flying_dragon_full.json" `
          "config/bots/bot_families/dragons/flying_dragon_full.json"
```

### **Step 3: Move Documentation** ✅
```powershell
# README.md, INDEX.md, QUICK_REFERENCE.md: flying_dragons/ → bothome/
# All documentation now at bothome root for direct access
```

### **Step 4: Move Hardware Templates** ✅
```powershell
# hardware_templates/: flying_dragons/ → bothome/
# T_MOTOR_U3_580KV.json, HOBBYWING_40A_ESC.json, etc.
```

### **Step 5: Remove Empty flying_dragons/ Folder** ✅
```powershell
# Deleted obsolete flying_dragons/ subfolder
# Hierarchy now flattened to bothome level
```

### **Step 6: Create Organizational Guides** ✅
```powershell
# Created BOTHOME_STRUCTURE.md (comprehensive guide)
# Created REORGANIZATION_SUMMARY.md (this file)
```

---

## Component Organization (WIP)

### **Currently in bothome/head/**
- ✅ dragon_nostril_left.json
- ✅ dragon_nostril_right.json

### **To Be Organized** (Next Phase)

**From flying_dragon_full.json subsystem structure:**

- **head/** should contain:
  - flying_dragon_left_eye<GC9A01> config
  - flying_dragon_right_eye<GC9A01> config
  - flying_dragon_mouth<GC9A01> config
  - flying_dragon_speaker<MINISPKR> config
  - dragon_nostril_left.json ✅
  - dragon_nostril_right.json ✅

- **wings/** should contain:
  - flying_dragon_wing_servo_controller<MG996R_SERVO> config
  - Wing articulation parameters
  - Servo synchronization logic

- **motors/** should contain:
  - flying_dragon_motor_controller<T_MOTOR_U3_580KV> references
  - Motor mixing matrix
  - Thrust allocation tables

- **sensors/** should contain:
  - flying_dragon_sensor_fusion<ICM20689_BMP390> references
  - Complementary filter logic
  - Calibration parameters

- **power/** should contain:
  - flying_dragon_flight_safety<WATCHDOG_BATTERY_MONITOR> references
  - Battery monitoring logic
  - Watchdog configuration

- **control/** should contain:
  - flying_dragon_behavior_sequencer<ESP32_S3_FLIGHT_LOGIC> references
  - Flight state machine definition
  - PID controller gains

---

## Key Architectural Principles

### **1. Single Source of Truth**
- **Hardware specs**: `config/hardware/` (primary)
- **Bot assembly**: `bothome/flying_dragon_full.json` (primary)
- **Procurement reference**: `bothome/hardware_templates/` (secondary)

### **2. Part-Based Organization**
- Aligns with flying dragon anatomy (head, wings, body)
- Mirrors subsystem structure in flying_dragon_full.json
- Enables intuitive navigation

### **3. Flattened Root**
- Master files at bothome level for quick access
- No unnecessary nesting
- Faster to locate and modify

### **4. Reference & Implementation Separation**
- **Reference** (hardware_templates/): Procurement specs, detailed specs
- **Implementation** (config/hardware/): Active hardware definitions
- **Component defs** (config/components/): Behavioral logic

---

## File Size Summary

| Location | Files | Total Size |
|----------|-------|-----------|
| bothome/ root | 6 files | ~100 KB (README, INDEX, QUICK_REFERENCE, flying_dragon_full.json, org docs) |
| bothome/hardware_templates/ | 4 files | ~4.4 KB |
| bothome/head/ | 2 files | ~0.5 KB |
| bothome/wings/ | — | — |
| bothome/body/ | 1 subdir | — |
| bothome/motors/ | — | — |
| bothome/sensors/ | — | — |
| bothome/power/ | — | — |
| bothome/control/ | — | — |
| **TOTAL** | **13+** | **~105 KB** |

---

## Navigation Quick Start

### **I want to...**

| Goal | Start Here | Then Review |
|------|-----------|------------|
| Understand the complete flying dragon | `flying_dragon_full.json` | → README.md for assembly |
| See all component specs | `INDEX.md` | → Hardware sections |
| Get pin assignments | `QUICK_REFERENCE.md` | → Pin assignment table |
| Procure hardware | `hardware_templates/` | → Bill of materials |
| Build the flying dragon | `README.md` (4-phase) | → Part folders (head/, wings/, etc.) |
| Understand motor control | `config/hardware/flying_dragon_motor_controller.json` | → motors/ folder |
| Configure flight behavior | `config/hardware/flying_dragon_behavior_sequencer.json` | → control/ folder |
| Add a new dragon variant | Copy `flying_dragon_full.json` | → Create new bot_{name}_{id}.json |

---

## Verification Checklist

- [x] flying_dragons/ subfolder removed (files migrated)
- [x] Master definition at bothome root
- [x] Documentation at bothome root
- [x] Hardware templates accessible
- [x] Part-based folders created (head, wings, body, motors, sensors, power, control, legs)
- [x] Hardware specs verified in config/hardware/
- [x] Subsystem integration file verified in config/components/subsystem/
- [x] Organizational guides created
- [ ] Component definition files organized into part folders (next phase)
- [ ] Subsystem component assignments validated

---

## Next Steps

1. **Organize component files** into appropriate part folders based on flying_dragon_full.json subsystem structure
2. **Validate** that all component references point to correct locations
3. **Test** that flying_dragon_full.json correctly references all subsystems
4. **Document** any variant-specific modifications in part folder READMEs
5. **Scale** to additional dragon families (ancient_fire, frost_guardian, etc.)

---

## Rollback Instructions (If Needed)

To restore the previous nested structure:

```powershell
# Create flying_dragons subfolder
New-Item -ItemType Directory -Path "config/bots/bot_families/dragons/flying_dragons"

# Move files back to flying_dragons/
Move-Item "flying_dragon_full.json" "flying_dragons/"
Move-Item "README.md" "flying_dragons/"
Move-Item "INDEX.md" "flying_dragons/"
Move-Item "QUICK_REFERENCE.md" "flying_dragons/"
Move-Item "hardware_templates/" "flying_dragons/"

# Remove part-based folders if needed
Remove-Item "head/", "wings/", "body/", "motors/", "sensors/", "power/", "control/" -Recurse
```

---

## Questions & Support

For questions about the new bothome structure:
1. Consult `BOTHOME_STRUCTURE.md` for detailed organization
2. Review `README.md` for assembly and integration procedures
3. Check `QUICK_REFERENCE.md` for specific specs and pin assignments
4. Examine `flying_dragon_full.json` to understand subsystem composition

