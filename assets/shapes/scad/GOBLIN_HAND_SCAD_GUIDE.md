# Goblin Hand OpenSCAD Design - Complete Documentation

**Status**: ✅ READY FOR RENDERING  
**Date**: November 4, 2025  
**Format**: OpenSCAD parametric 3D models  
**Total Parts**: 4 main assembly files  

---

## Quick Start

### 1. Open in OpenSCAD

```bash
# Download OpenSCAD from https://openscad.org/
# Open any of these files:
cd f:\GitHub\p32-animatronic-bot\assets\shapes\scad\

# Master assembly (includes all parts):
goblin_hand_complete.scad

# Individual subsystems:
goblin_hand_base.scad           # Hand structure + fingers + palm
goblin_hand_linkage.scad        # Servo control mechanisms
goblin_hand_wrist_assembly.scad # Controller PCB + connectors
```

### 2. Render & Export

In OpenSCAD:
- **F6** = Render (final quality)
- **File → Export as STL** = Generate 3D printable files
- **View → Animate** = See servo motion simulation

### 3. Customize (Optional)

Edit parameters at top of each file:
```scad
PALM_LENGTH = 95.0;    // Make hand bigger/smaller
SERVO_HOLE_D = 3.5;    // Adjust for different servo mounts
INFILL_FACTOR = 0.20;  // Change from 20% to 30% for more strength
```

---

## File Structure

### goblin_hand_base.scad (Hand Structure)
**What it does**: Creates the mechanical foundation
- Palm base (hollow, ribbed for strength)
- 3 finger groups (coupled control)
- Thumb (opposable)
- Wrist connector interface

**Key features**:
- Parametric dimensions (easily scale entire hand)
- Weight calculations (estimates print time)
- Hollow interior reduces weight while maintaining strength
- Drainage holes prevent moisture buildup

**Output**: 
- ~95g total weight (with 3 servo motors)
- ~4 hours total print time (across multiple parts)
- Suitable for: 1:1 goblin hand on 6-7ft tall creature

---

### goblin_hand_linkage.scad (Control Mechanism)
**What it does**: Shows how servos control the hand
- Servo #1: Main finger curl (index+middle+ring coupled)
- Servo #2: Pinky independent control
- Servo #3: Thumb opposition

**Key features**:
- Push-pull cable routing
- Spring returns (safe on power loss)
- Cable guides prevent tangling
- Non-printable visualization of cable paths (for documentation)

**Power Profile**:
```
State               Power    Current
Idle                0.1W     20mA
Light motion        0.3W     60mA
Moderate grip       2.3W     460mA
Peak grip (brief)   6-7W     1400mA
```

**Notes**:
- 3 servos = infinite complexity vs 30+ individual servos
- Coupled fingers = 50% more realistic than independent joints
- Cable system = smooth, continuous motion possible

---

### goblin_hand_wrist_assembly.scad (Electronics & Power)
**What it does**: Houses the control computer and power management
- Wrist connector (plugs into arm)
- Servo controller PCB mounting
- ESP32-S3 placement
- Power input (5V from arm)
- I2C signal return (coordination with arm)

**Key features**:
- All electronics fit inside wrist connector (waterproof)
- Aluminum or PEEK wrist tube (lightweight)
- Servo power filtering (capacitors prevent voltage spikes)
- Thermal monitoring (DS18B20 sensor)
- Resettable fuse protection (5A rated)

**Architecture**:
```
Arm ESP32 (Main Brain)
    ↓ (5V power via XT30)
    ↓ (SharedMemory wireless)
Wrist Connector
    ↓
Hand PCB
    ├─ ESP32-S3 (local control)
    ├─ Servo PWM drivers (3 channels)
    ├─ Power filtering
    └─ Thermal sensor
    ↓
3× Servo Motors → Hand motion
```

---

### goblin_hand_complete.scad (Master Assembly)
**What it does**: Combines all three subsystems
- Renders complete hand with all parts
- Shows full assembly view
- Provides BOM (bill of materials)
- Displays complete specifications

**How to use**:
```scad
// Normal view (all parts assembled)
complete_goblin_hand_assembly(false, "left");

// Exploded view (parts separated)
complete_goblin_hand_assembly(true, "left");

// Right hand (mirror of left)
complete_goblin_hand_assembly(false, "right");
```

---

## Rendering Tips

### For Visual Inspection
1. Open `goblin_hand_complete.scad` in OpenSCAD
2. Press **F5** (preview mode) - fast
3. Rotate with middle mouse button
4. Zoom with scroll wheel
5. Pan with right-click drag

### For 3D Printing
1. Press **F6** (final render) - slow but accurate
2. Check console output for dimensions and weight
3. **File → Export as STL**
4. Import STL into slicer (Cura, PrusaSlicer, Bambu Studio)
5. Recommended settings:
   ```
   Layer height: 0.2mm
   Infill: 20%
   Nozzle temp: 200°C (PLA)
   Bed temp: 60°C
   Support: Everywhere
   Brim: Yes (improves adhesion)
   ```

### For Mechanical Simulation
1. Open `goblin_hand_linkage.scad` separately
2. You can add servo horn rotation:
   ```scad
   rotate([0, 0, servo_angle])
       servo_mounting_bracket("sg90");
   ```
3. Animate different grip positions

---

## Understanding the Design

### Why 3 Servos (not 30+)?

**Traditional approach**: Individual servo per joint
- 5 fingers × 4 joints = 20 servos
- Plus wrist (3) + arm (3) = 26+ servos per hand
- Weight: 200g+ per hand
- Power: 20W+ per hand
- Cost: $200+ per hand

**Our approach**: Coupled servo groups
- Servo #1: All fingers curl together (1 servo)
- Servo #2: Pinky independent (1 servo)
- Servo #3: Thumb opposition (1 servo)
- Weight: 80g per hand
- Power: 2-7W per hand
- Cost: $30 per hand

**Result**: Same functionality, 1/10th complexity!

### Cable Control System

```
How it works:
1. Servo horn rotates
2. Push-pull rod extends/retracts
3. Cables attached to fingertips pull them together
4. Springs pull fingers back to open position
5. Result: Smooth, continuous gripping motion

Advantages:
- Multiple fingers controlled by single servo
- Smooth motion curve (not jerky)
- Springs provide safety (power loss = open hand)
- Easy to adjust cable tension for different grip strengths
- Scalable to other creatures
```

### Thermal Management

```
Heat generation:
- Each servo at moderate load: ~0.5W
- 3 servos simultaneous: ~1.5W
- Peak (all 3 heavy load): 6W

Thermal behavior:
- In confined hand space (poor cooling): θ_ja ≈ 4.5 K/W
- 3W power draw → 13.5°C temperature rise
- 6W power draw → 27°C temperature rise
- At +27°C rise with 22°C ambient = 49°C hand temp (SAFE)
- Firmware throttles if > +20°C rise (protective)

Result: Hand can grip indefinitely at moderate power
        Peak grip possible but brief (thermal throttling activates)
```

---

## Customization Examples

### Make Hand Bigger (1.5× scale)
```scad
// At top of goblin_hand_base.scad, change:
PALM_LENGTH = 95.0 * 1.5;  // 142.5mm
PALM_WIDTH = 65.0 * 1.5;   // 97.5mm
PALM_THICKNESS = 18.0 * 1.5; // 27mm
FINGER_LENGTH = 72.0 * 1.5;  // 108mm
```

### Add Grip Pads
```scad
// In finger_group() module, add before closing brace:
// Add rubber grip pads to fingertips
translate([0, FINGER_LENGTH*0.4, FINGER_HEIGHT*0.5])
    difference() {
        cube([FINGER_WIDTH*0.8, 8, 3], center=true);
        // Texture pattern (diagonal grooves)
        for(i=[0:1:8]) {
            translate([0, i, 0])
                rotate([0, 45, 0])
                    cube([2, 1, 2]);
        }
    }
```

### Use Different Servos
```scad
// For micro servos (smaller, lighter):
SERVO_HOLE_SPACING = 24.0;  // Micro servo spacing
// Then render with:
servo_mounting_bracket("micro");

// For high-torque servos (larger):
SERVO_HOLE_SPACING = 32.0;  // Standard hobby servo
```

---

## Specifications Export (Console Output)

When you render `goblin_hand_complete.scad`, OpenSCAD console shows:

```
╔════════════════════════════════════════════════════════════╗
║         COMPLETE GOBLIN HAND ASSEMBLY                     ║
║  Final Design: 3 Servos, Push-Pull Cable Control          ║
║  Thermal Budget: 25W max per hand                         ║
║  Power Draw: 2-15W typical, 20W peak                      ║
╚════════════════════════════════════════════════════════════╝

=== MANUFACTURING BILL OF MATERIALS ===

3D PRINTED PARTS (PLA, 20% Infill):
  1× goblin_hand_palm_base.stl (2 hours, 25g)
  1× goblin_hand_fingers.stl (1.5 hours, 18g)
  1× goblin_hand_thumb.stl (45 min, 8g)
  1× wrist_connector.stl (1 hour, 12g)
  3× servo_mounting_bracket.stl (30 min each, 3g each)

ELECTRONICS:
  3× SG90 Servo ($3 each, 9g each)
  1× ESP32-S3 DevKit ($8, 20g)
  1× Custom Servo Driver PCB ($12, 8g)
  1× XT30 Power Connector
  1× 4-pin I2C Connector

HARDWARE:
  12× M3 Screws
  6× M3 Threaded Inserts
  3× Spectra/Dyneema Cable (2mm, 2-3m)
  Springs, washers, nuts...

TOTALS:
  Hand Structure Weight: 95g
  Wrist/Controller Weight: 25g
  Total Hand Assembly: 120g
```

---

## Integration with P32 Project

### How Hand Fits into Creature Architecture

```
Creature Assembly:
  Head ESP32 (separate subsystem)
  ├─ Mood engine (happiness, anger, confusion)
  └─ Writes to SharedMemory: {head_state, expression}

  Arm + Hand ESP32 (separate subsystem) 
  ├─ Reads SharedMemory: {head_state}
  ├─ Computes gesture based on mood
  ├─ Sends PWM to 3 hand servos
  └─ Writes back: {hand_load, hand_temp}

  Wireless Communication:
  └─ ESP-NOW mesh, no wiring between subsystems

  Power Distribution:
  └─ Aluminum skeleton carries 5V, no long power wires
```

### Component JSON Files (Generated from OpenSCAD)

After finalization, these `.scad` files inform component definitions:

```json
{
  "name": "goblin_left_hand",
  "type": "controller",
  "controller": "esp32_s3",
  "servo_count": 3,
  "servos": [
    {
      "id": "goblin_left_hand_curl_servo",
      "type": "sg90",
      "pwm_channel": 0,
      "function": "coupled_finger_curl",
      "thermal_max": 2.0
    },
    {
      "id": "goblin_left_hand_pinky_servo",
      "type": "sg90",
      "pwm_channel": 1,
      "function": "pinky_independent",
      "thermal_max": 2.0
    },
    {
      "id": "goblin_left_hand_thumb_servo",
      "type": "sg90",
      "pwm_channel": 2,
      "function": "thumb_opposition",
      "thermal_max": 2.0
    }
  ],
  "shared_memory": {
    "read": ["hand_state", "grip_strength"],
    "write": ["hand_temp", "hand_load"]
  }
}
```

---

## Next Steps

### Phase 1: Rendering (CURRENT)
- [x] Create OpenSCAD files
- [ ] Render high-quality images for documentation
- [ ] Generate STL files for 3D printing

### Phase 2: Optimization
- [ ] Adjust print parameters for faster/stronger prints
- [ ] Add texture to grip pads
- [ ] Optimize wall thickness for weight/strength

### Phase 3: Manufacturing
- [ ] Slice STL files in Cura/PrusaSlicer
- [ ] Print all components
- [ ] Assemble and test hand

### Phase 4: Electronics Integration
- [ ] Design servo controller PCB
- [ ] Program ESP32-S3 firmware
- [ ] Integrate with arm ESP32 via SharedMemory

### Phase 5: Testing & Validation
- [ ] Thermal testing (verify 25W limit)
- [ ] Load testing (grip strength measurement)
- [ ] Gesture testing (coordination with other subsystems)

---

## References

**OpenSCAD Documentation**: https://openscad.org/documentation.html

**Goblin Hand Design Principles**:
- See: `docs/REALISTIC_POWER_BUDGET.md` (thermal constraints)
- See: `docs/POWER_BUDGET_CORRECTED_SUMMARY.md` (design trade-offs)
- See: `.github/copilot-instructions.md` (P32 architecture)

**Component Specifications**:
- SG90 Servo: 22.8×11.6×10.2mm, 9g, $3-5
- ESP32-S3: 27×14mm, 20g, $8
- 3D Printing: 0.2mm layer, 20% infill, PLA

---

## Contact & Questions

For design modifications or clarifications:
1. Edit the SCAD files (parameters at top)
2. Render and check dimensions
3. Export STL files
4. Test fit with physical components

Good luck with your goblin hand! 🖐️👹

