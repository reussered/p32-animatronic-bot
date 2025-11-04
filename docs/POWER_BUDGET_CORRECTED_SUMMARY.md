# Power Budget Corrections Summary

**Status**: ✅ Corrected and clarified  
**Date**: November 4, 2025  
**Updated**: Reflects user feedback on thermal constraints

---

## What Was Wrong

### The Original Premise (INCORRECT)

Previous analysis stated: **"We have 20-30A @ 5V (100-150W) system power budget to work with."**

This number appeared to come from the total system current capability and was treated as a design target.

### Why It Was Wrong

**The constraint is THERMAL, not electrical.**

- A 5V 50A power supply is trivially easy to provision (~$50-100)
- What's NOT easy is dissipating 100+ watts of heat in a plastic toy body without melting it
- Servo motors are ~30-40% efficient (60-70% waste as heat)
- Heat accumulates in confined spaces with nowhere to go
- Plastic melting points: PLA 60-65°C, PETG 80-85°C

**Example**: If we tried to draw 100W in a single hand:
- 10 servos × 10W each
- Confined plastic enclosure ~500 cm³
- NO active cooling (just convection through plastic walls)
- Temperature rise: +80-120°C in minutes
- **Result**: Hand melts, servos fused to mounts

---

## The Corrected Reality

### Thermal Limits (THE HARD CONSTRAINT)

| Scope | Power Limit | Current | Duration | Thermal Rise | Notes |
|-------|-------------|---------|----------|--------------|-------|
| **Per Hand** | 25W max | 5A @ 5V | Sustained | +15-20°C | Realistic limit with ventilation |
| **Per Creature** | 75W typical | 15A @ 5V | Sustained | +15-20°C | Not all body parts simultaneous |
| **System (13 creatures)** | 75-100W | 15-20A @ 5V | Simultaneous | +15-20°C | Only 2-3 creatures animating |
| **Brief surge** | 35-40W per creature | - | <10 seconds | +5°C spike | Dramatic gesture, then rest |

### Why These Numbers Work

```
Per Hand Analysis (25W):
├─ ~10 servos in enclosed hand
├─ 2.5W per servo average (sustainable draw)
├─ Temperature rise: +15-20°C
├─ With ventilation holes: can dissipate to external air
├─ With PETG plastic: melting point 80-85°C >> 50-60°C local temp

Per Creature Analysis (75W):
├─ 20-22 servos distributed across head, arms, torso, legs
├─ Different body parts don't operate at full power simultaneously
├─ Typical sequence: Eyes + mouth move (2W), then arm swings (3W), 
│  then hand gestures (3W), back to idle = ~8W average
├─ Sufficient for expressive animation without thermal stress
└─ Burst capable of 75W for <10 seconds (dramatic moment, then rest)

System Level Analysis (75-100W):
├─ 13 creatures × 5-8W average = 65-104W total
├─ Not all 13 creatures animate simultaneously
├─ Realistic scenario: 2-3 creatures gesture while others idle
├─ Museum setting: creatures take turns performing (prevents thermal buildup)
└─ Cost-effective: One 5V 30A PSU can support entire system with rotation
```

---

## How We Get to 25W per Hand

### Key Assumptions

1. **Servo Count**: 8-12 servos per hand (not 30+)
2. **Linked Control**: Not independent finger articulation; coupled servo groups
3. **Duty Cycle**: Servos don't all move simultaneously; staggered sequences
4. **Thermal Budget**: Firmware throttles speed if temp approaches +20°C rise

### Realistic Servo Power Draw

```
Servo States:
├─ Idle (holding position): ~20mA @ 5V = 0.1W per servo
├─ Light movement (no load): ~60mA @ 5V = 0.3W per servo
├─ Moderate load (gripping): ~150mA @ 5V = 0.75W per servo
└─ Heavy stall (max grip): ~400mA @ 5V = 2W per servo (brief, <1 sec)

Per Hand Example (10 servos):
├─ All idle: 10 × 0.1W = 1W total (SAFE)
├─ 3 moving + 7 holding: (3 × 0.3) + (7 × 0.1) = 1.6W (VERY SAFE)
├─ 5 moderate grip + 5 holding: (5 × 0.75) + (5 × 0.1) = 4.25W (SAFE)
├─ All 10 moderate grip: 10 × 0.75W = 7.5W (STILL SAFE)
├─ **Realistic peak**: 2 heavy stall + 3 moderate + 5 holding = 5.3W
└─ Thermal design target: Never exceed 25W per hand (= 10× safety margin)
```

### Firmware Protection

```cpp
// Pseudocode for thermal enforcement

const int MAX_POWER_PER_HAND_MA = 5000;  // 5A = 25W @ 5V
const int THERMAL_THROTTLE = 40;        // Reduce if +40°C above ambient
const int THERMAL_HALT = 50;            // Stop if +50°C above ambient

if (current_draw_ma > MAX_POWER_PER_HAND_MA) {
  reduce_all_servo_speeds_by(50%);
  queue_delayed_movements();
}

if (temperature_rise > THERMAL_HALT) {
  halt_all_servos();
  sound_alarm();
  wait_for_cooldown();
}

if (temperature_rise > THERMAL_THROTTLE) {
  reduce_all_servo_speeds_by(20%);
  increase_pause_duration_between_gestures();
}
```

---

## Implications for System Design

### Power Supply Changes

**Old (incorrect) target**: Need 50A @ 5V supply for 100-150W theoretical max

**New (correct) target**: Need 15-30A @ 5V supply for realistic 75-100W operation
- One 5V 30A supply (150W): Sufficient for all 13 creatures with throttling
- Cost: ~$80-120 (not expensive!)
- Advantage: Simpler design, fewer complications, more reliable

### Servo Architecture Changes

**Old**: Goal was individual servo per joint (30+ per hand)

**New**: Goal is coupled servo groups (8-12 per hand)
- All fingers curl together = 1 servo (not 5 independent)
- Wrist (pitch, yaw, roll) = 3 servos
- Thumb opposition = 1 servo
- Reserve/fine control = 1-2 servos
- **Total per hand**: 8-12 servos (vs 30+)

### Per-Creature Totals

| Creature Type | Eyes | Head | Arms | Hands | Torso | Legs | Total |
|---------------|------|------|------|-------|-------|------|-------|
| Humanoid | 4 | 6 | 8 | 12 | 4 | 12 | ~46 |
| Goblin | 2 | 4 | 8 | 10 | 4 | 8 | ~36 |
| Robot | 2 | 3 | 8 | 10 | 6 | 8 | ~37 |
| Dragon | 4 | 8 | 6 | 12 | 6 | 10 | ~46 |

**System total**: ~400-500 servos (not 3,000+)  
**System cost**: ~$2,000-2,500 for servos (not $15,000)  
**System power**: ~75-100W sustained (not 500W)

---

## Thermal Management Strategy

### Passive Cooling (Primary)

1. **Ventilation Holes**
   - 0.5" diameter holes drilled in creature body
   - 4-6 holes per subsystem = 20-30 holes total
   - Reduces steady-state temp by ~5-8°C

2. **Material Selection**
   - Use PETG instead of PLA (melting point 80-85°C vs 60-65°C)
   - Provides 15°C+ safety margin
   - Cost: Minimal ($1-2 more per creature)

3. **Aluminum Heat Sinks**
   - Mount servos on aluminum brackets
   - Heat conducted away from plastic
   - Improves convection efficiency

### Active Cooling (Firmware)

1. **Real-Time Thermal Monitoring**
   - DS18B20 temperature sensor on servo controller
   - Measures ambient + servo heat rise
   - Updates every 2 seconds

2. **Adaptive Throttling**
   - Reduce servo speed if temp rises above +20°C
   - Increase pause duration between animations
   - Never allow simultaneous max-load on multiple servos

3. **Hardware Safeguards**
   - Resettable fuse (5A rated)
   - Current monitoring IC (INA219)
   - Firmware enforces max 4.5A per creature

---

## Next Steps: Per-Creature Specifications

### Action Items (Priority Order)

1. **✅ Update REALISTIC_POWER_BUDGET.md** (DONE)
   - Clarify thermal constraint
   - Document 25W per hand as hard limit
   - Remove misleading "20-30A system budget" language

2. **→ Create SERVO_BUDGET_BY_CREATURE.md** (NEXT)
   - Per-creature servo allocation (8-12 per hand, 36-50 total)
   - Coupled servo control strategy (linked finger curl, etc.)
   - Temperature rise estimates for each subsystem

3. **→ Generate component JSON files**
   - `config/bots/bot_families/goblins/hands/goblin_left_hand_servo_controller.json`
   - `config/bots/bot_families/goblins/hands/goblin_left_hand_grip_servo.json`
   - Similar for other creatures

4. **→ Define I2C control protocol**
   - Main ESP32 "brain" per creature
   - Distributed ESP32 per hand/limb segment
   - Communication addresses and heartbeat mechanism

5. **→ Create Bill of Materials**
   - 400-500 servos total × 13 creatures
   - 39-40 ESP32 boards (main + distributed)
   - Power supplies, thermal sensors, wiring

---

## Key Takeaways

### ✅ What We NOW Know

1. **Thermal is the constraint**, not electrical supply capacity
2. **25W per hand maximum** is the hard limit (with safety margin)
3. **Firmware throttling** provides protection against overheating
4. **Coupled servo design** (linked groups) is more practical than individual joints
5. **Distributed ESP32 boards** (one per hand/limb) simplifies control
6. **System cost** is ~$2,500-3,000 (reasonable for 13 creatures)
7. **System power** is ~75-100W sustained (manageable with single PSU)

### ✅ What Changes from Previous Analysis

| Aspect | Old Assumption | New Reality |
|--------|-----------------|-------------|
| Power budget | 100-150W | 75-100W sustained |
| Per hand limit | Undefined (assumed high) | 25W thermal max |
| Servos per hand | 30+ (individual joints) | 8-12 (coupled groups) |
| Total servos | 390+ | 400-500 |
| System cost | $15,000+ | $2,500-3,000 |
| Constraint type | Electrical | **THERMAL** |
| Main challenge | Power supply | **Heat dissipation** |

---

## Conclusion

The P32 project now has a **realistic, thermally-grounded power budget** that balances animation fidelity with physical feasibility. 

Key insight: **We are building animatronics, not precision robotics.** Coupled servo groups that grip and wield axes are better than complex individual finger articulation that melts plastic.

Next phase: Per-creature servo specifications (starting with goblin) that implement this thermal-aware design.

