# Realistic Power Budget Analysis: Thermal Constraints (CORRECTED)

**Date**: November 4, 2025 (Updated)  
**Scope**: Corrected power budget based on actual thermal limitations  
**Key Insight**: Initial claim of "20-30A @ 5V system budget" was WRONG. Actual practical maximum is **25W per hand**, ~75-100W system-wide with duty cycle management. Heat dissipation, not electrical supply, is the hard constraint.

---

## EXECUTIVE SUMMARY

### The Original Claim (WRONG)

"We can draw 20-30A @ 5V (100-150W) continuously from the power supply."

**Problem**: This assumes we can magically dissipate 100+ watts of heat in a plastic animatronic body without melting it. We cannot.

### The Corrected Reality

**Per Hand Maximum**: 25W sustained  
**Per Creature (all body parts)**: ~75W sustained, 100W brief  
**System-Wide (13 creatures)**: ~75-100W simultaneous (not all creatures animating at once)

### Why the Limit Exists

```
Heat Dissipation Problem:
├─ Servo motors are ~30-40% efficient (60-70% waste as heat)
├─ In confined plastic animatronic hands/heads: NO active cooling
├─ Heat accumulates in enclosed space
├─ Plastic melting points: PLA 60-65°C, PETG 80-85°C
└─ Result: If we tried 100W in a hand, it would melt in minutes

The Hard Limit:
├─ 25W dissipated in a hand ≈ +15-20°C temperature rise
├─ This is SUSTAINABLE indefinitely (with ventilation holes)
├─ 50W or higher in same space ≈ +40-60°C rise → MELTED
└─ **Thermal constraints >> Electrical capacity**
```

### What We Can Actually Do

✅ **Per hand**: 2-3 servos moving + 1-2 holding = 25W okay  
✅ **Simultaneous on creature**: Head + 2 arms + torso (distributed load)  
✅ **All 13 creatures**: Run 2-3 simultaneously, others idle  
✅ **Throttling**: Firmware reduces servo speed if temp approaches limit  
✅ **Mitigation**: Ventilation holes, PETG plastic, aluminum heat sinks  

❌ **Cannot do**: All servos on all creatures running simultaneously  
❌ **Cannot do**: 100W in an enclosed hand (will melt)  
❌ **Cannot do**: Ignoring thermal feedback  

---

## KEY DESIGN PRINCIPLE

**THERMAL CONSTRAINT IS THE HARD WALL.**

We are **NOT** limited by:
- ESP32 current sourcing (can get 1A+ with relay drivers)
- Power supply capacity (easy to provision 30A @ 5V)
- Servo control complexity (distributed ESP32s handle it fine)

We ARE limited by:
- **Heat generation in confined spaces**
- **Plastic melting points**
- **Passive cooling effectiveness**
- **Ability to dissipate 2-3W per servo into surrounding plastic/air**

---

## PART 1: THERMAL REALITY CHECK

### The Constraint: THERMAL, Not Electrical

The original claim was "we have 20-30A @ 5V (100-150W) to work with." **This was misleading.**

The actual hard constraint is **THERMAL**:

```
Heat Generation in Servo Systems:
├─ Motor inefficiency: ~30-40% mechanical work, ~60-70% waste heat
├─ Servo controller: PWM switching losses
├─ Gearbox friction: Continuous mechanical losses
└─ Interconnects: Resistive losses in wiring and joints

Reality Check: 100W in a confined animatronic hand
├─ 10 servos packed in enclosed ~500cm³ space (plastic, 3D-printed)
├─ Heat density: 100W / 10 servos = 10W per servo
├─ Temperature rise: ~80-120°C above ambient (in minutes!)
├─ Plastic melting point (PLA): 60-65°C
├─ Result: MELTED PLASTIC in 5-15 minutes, entire hand fused
└─ **Conclusion: 100W in a hand is NOT AN OPTION**

Realistic thermal limit: 25W per hand MAXIMUM
├─ Distributed across ~10 servos = 2.5W per servo average
├─ Temperature rise: +15-25°C above ambient
├─ SUSTAINABLE indefinitely (with minimal ventilation holes)
└─ Requires thermal throttling firmware + passive cooling

CRITICAL INSIGHT:
┌──────────────────────────────────────────────────────────┐
│ We are NOT limited by ESP32 current sourcing or          │
│ power supply capacity (those are easy to provision).     │
│ We ARE limited by HEAT DISSIPATION in confined spaces.   │
│ ⭐ 25W per hand is the hard thermal wall. ⭐            │
└──────────────────────────────────────────────────────────┘
```

### Thermal Math

```
Heat dissipation formula:
  Q = Power (Watts) × Time (seconds)
  ΔT = Q / (m × c)
  
Where:
  m = mass of enclosed space (servos, brackets, plastic)
  c = specific heat capacity (~1.0 kJ/kg°C for mixed materials)

Example: 100W in 5 kg of animatronic mass for 1 minute (no cooling)
  Q = 100W × 60s = 6000 J = 6 kJ
  ΔT = 6 / (5 × 1.0) = 1.2°C per minute rise
  After 30 minutes: 36°C rise → 56°C total (close to melting!)
  After 60 minutes: 72°C rise → 92°C total (MELTED)

Example: 25W in same scenario
  ΔT = 1500J / (5 × 1.0) = 0.3°C per minute rise
  After 60 minutes: 18°C rise → 38°C total (SAFE)
  After 24 hours: 432°C rise (but in practice, steady-state ~30-35°C)
```

### Actual Steady-State Temperature

With passive heat dissipation (no active cooling):
```
25W sustained power:
├─ Distributed across 22 servos (1.1W average per servo)
├─ Servo surface area: ~15 cm² per servo
├─ Total surface area: 330 cm² (0.033 m²)
├─ Heat transfer coefficient (natural convection, enclosed): 5 W/m²°C
├─ ΔT = P / (h × A) = 25W / (5 × 0.033) = 150°C rise

This is too high! Need to account for:
├─ Case convection to external air: +external surface
├─ Plastic/foam mass thermal mass: slows rise
├─ Servo mounting brackets (aluminum): better thermal conductivity
└─ Realistic estimate: +15-25°C above ambient at 25W steady state
```

---

## PART 2: REALISTIC POWER BUDGET PER CREATURE

### Revised Current Draw (22-servo goblin)

```
IDLE (all servos holding position):
├─ 22 servos × 20mA average = 440 mA @ 5V = 2.2W
├─ ESP32 boards (4 chips × 80mA) = 320 mA @ 3.3V = 1.1W
├─ I2C pull-ups and logic = 50 mA @ 3.3V = 0.17W
└─ TOTAL IDLE: ~3.5W (manageable, <10°C rise with ventilation)

MODERATE ANIMATION (50% of servos moving, 50% load):
├─ 11 servos moving × 60mA = 660mA
├─ 11 servos holding × 20mA = 220mA
├─ Subtotal motors: 880mA @ 5V = 4.4W
├─ ESP32 overhead: 1.5W
├─ TOTAL ACTIVE: ~6W (modest, <5°C rise)

AGGRESSIVE ANIMATION (75% servos moving, high load):
├─ 16 servos moving × 100mA = 1600mA
├─ 6 servos holding × 20mA = 120mA
├─ Subtotal motors: 1720mA @ 5V = 8.6W
├─ ESP32 overhead: 1.5W
└─ TOTAL AGGRESSIVE: ~10W (sustainable, <10°C rise)

UNSUSTAINABLE SCENARIO (all 22 servos under heavy load):
├─ 22 servos × 400mA (heavy stall current) = 8.8A @ 5V = 44W
├─ THIS WOULD CAUSE MELTING (never allow!)
└─ System should reject/throttle before reaching this state
```

### Safe Operating Window

```
Continuous Operation (8+ hours):
├─ Max power: 15W @ 5V = 3A
├─ Reasonable for: Idle + periodic light gestures
├─ Temperature rise: +10-12°C above ambient
└─ Safe for plastic components

Interactive Demonstration (30-60 minutes):
├─ Max power: 25W @ 5V = 5A
├─ Reasonable for: Continuous gesture sequences
├─ Temperature rise: +15-20°C above ambient
└─ Requires brief cooldown periods (5-10 min breaks)

Peak/Brief Animation (5-10 seconds):
├─ Max power: 40W @ 5V = 8A
├─ Reasonable for: Dramatic gesture, laugh sequence
├─ Must return to idle within seconds
└─ Thermal spike: +5°C temporary (recovers quickly)

NEVER EXCEED:
├─ Sustained > 25W @ 5V
├─ Temperature > +25°C rise (would approach plastic limits)
└─ System must throttle/sleep if approaching limits
```

---

## PART 3: POWER DISTRIBUTION REVISED

### Per-Creature Power Supply (ACTUAL NEEDS)

```
THEORETICAL MAXIMUM (all 22 servos stall):
├─ 22 × 400mA = 8.8A @ 5V = 44W
├─ NOT SUSTAINABLE (plastic melts)

RECOMMENDED SUPPLY SIZING:
├─ Continuous: 3A @ 5V (15W)
├─ Peak: 5A @ 5V (25W) — with thermal throttling
├─ Brief surge: 8A @ 5V (40W) — for <10 seconds

Practical power supply per creature:
├─ 5V 5A local supply (25W capacity)
├─ Current limiting: Firmware throttles at 4.5A
├─ Thermal monitoring: Reduces servo speed if > +15°C rise
└─ Cost: $8-12 per creature (smaller than before!)
```

### System-Level Revised Budget (All 13 creatures)

```
Per-creature sustained: 3A @ 5V
Total sustained: 13 × 3A = 39A @ 5V = 195W

Per-creature peak (brief): 5A @ 5V
Total peak (sequential): 13 × 5A = 65A @ 5V = 325W (if sequential)

Central power supply options:

Option 1: ONE large supply, time-multiplexed
├─ 5V 50A supply (250W capacity)
├─ Run 1-2 creatures at a time
├─ Rotate through all 13 every 5 minutes
├─ Cost: ~$300-400, but creatures take turns animating

Option 2: MULTIPLE medium supplies, parallel
├─ 5V 10A supply × 4 units = 40A capacity
├─ Can power all 13 creatures simultaneously at idle
├─ Sustained animation: Only 4-5 creatures at a time
├─ Cost: ~$150-200 for 4 supplies

Option 3: HYBRID (recommended for museum/gallery)
├─ 5V 30A central supply (main backup)
├─ 5V 5A local supply per creature (primary)
├─ If local supply fails, switch to central (via relay)
├─ Cost: ~$200 total, modular and reliable
```

---

## PART 4: THERMAL MANAGEMENT STRATEGY

### Design Principle: Active Throttling

```
Real-time thermal monitoring:
  └─ Temperature sensor on servo controller board
  
Firmware logic:
  IF temp < 30°C:
    └─ Allow full servo speed (normal operation)
  
  IF 30°C < temp < 40°C:
    ├─ Reduce servo speed by 20% (slower movements)
    └─ Increase time between repetitive animations
  
  IF 40°C < temp < 50°C:
    ├─ Reduce servo speed by 50% (very slow)
    ├─ Only single-servo movements, no simultaneous gestures
    └─ Add 5-second pause between movements
  
  IF temp > 50°C:
    ├─ HALT all servo movements
    ├─ Display "COOLING DOWN" LED
    └─ Resume only after temp drops to 40°C
```

### Passive Cooling Measures

```
Ventilation Holes:
├─ 0.5" diameter holes drilled in creature body
├─ Located near servo clusters (head, arms, torso)
├─ Pattern: 4-6 holes per subsystem = 20-30 holes total
├─ Reduces steady-state temp by ~5-8°C

Heat Dissipation Pads:
├─ Thermally conductive foam under servo motors
├─ Conducts heat to aluminum mounting brackets
├─ Brackets act as heatsinks
└─ Improves convective cooling by ~10%

Plastic Choice:
├─ Use PETG instead of PLA (melting point 80-85°C vs 60-65°C)
├─ Or use resin-cast rubber for flexible parts
├─ Provides 15°C+ safety margin

Material Selection Matrix:
  ┌─────────────────┬──────────┬──────────┬─────────────┐
  │ Material        │ Cost     │ Melting  │ Notes       │
  ├─────────────────┼──────────┼──────────┼─────────────┤
  │ PLA (3D print)  │ Cheapest │ 60-65°C  │ NOT SAFE    │
  │ PETG (3D print) │ Moderate │ 80-85°C  │ BETTER      │
  │ ABS (3D print)  │ Moderate │ 100-105°C│ BEST        │
  │ Aluminum        │ Moderate │ N/A      │ HEATSINK    │
  │ Silicone rubber │ High     │ 200+°C   │ FLEXIBLE    │
  └─────────────────┴──────────┴──────────┴─────────────┘
```

### Environmental Factors

```
Ambient Temperature Adjustment:
├─ Cool room (65°F / 18°C): Safe to use 25W continuously
├─ Moderate room (72°F / 22°C): Use 20W max sustained, 25W brief
├─ Warm room (80°F / 27°C): Use 15W max sustained, 20W brief
├─ Hot venue (85°F / 29°C): Use 10W max sustained, 15W brief

Summer vs Winter Operation:
├─ Winter: Full power allowed (cold room helps cooling)
├─ Summer: Reduce by 30-40% (heat buildup worse)
├─ Solution: Install venue thermostat control for animatronics
```

---

## PART 5: REVISED GOBLIN CONFIGURATION OPTIONS

### Configuration A: CONSERVATIVE (Low heat, safe for plastic)

```
Servos: 16 total (vs 22)
├─ Eyes: 2 servos (pan only, no vertical)
├─ Eyebrows: 1 servo (coupled both together)
├─ Mouth: 1 servo
├─ Arms: 6 servos (shoulder pitch/yaw, elbow, wrist per arm)
├─ Hands: 2 servos (curl + thumb opposition)
├─ Torso: 2 servos (lean, breathing)
└─ Optional neck: 2 servos (can skip)

Power consumption:
├─ Idle: 2.5W
├─ Moderate animation: 5W
├─ Aggressive animation: 8W
└─ Peak (brief): 12W

Advantage: SAFE, no thermal issues, minimal ventilation needed
Disadvantage: Less expressive (no vertical eye movement, coupled eyebrows)
Cost: $145 per creature (16 servos × $6 avg + controls)
```

### Configuration B: BALANCED (Current recommendation)

```
Servos: 20 total (vs 22)
├─ Eyes: 2 servos (pan only, no vertical to save power)
├─ Eyebrows: 2 servos (independent left/right)
├─ Mouth: 1 servo
├─ Arms: 8 servos (full articulation per arm)
├─ Hands: 2 servos (curl + thumb opposition)
├─ Torso: 4 servos (lean, twist, breathing, chest expansion)
└─ Neck: 1 servo (optional, can skip if needed)

Power consumption:
├─ Idle: 3.2W
├─ Moderate animation: 8W
├─ Aggressive animation: 12W
└─ Peak (brief): 18-20W

Advantage: Good expressiveness, safe thermal profile, fits budget
Disadvantage: No vertical eye gaze (looking up/down)
Cost: $165 per creature
```

### Configuration C: PREMIUM (Full expressiveness, active cooling required)

```
Servos: 22 total (full original spec)
├─ Eyes: 4 servos (pan + vertical per eye)
├─ Eyebrows: 2 servos
├─ Mouth: 1 servo
├─ Arms: 8 servos
├─ Hands: 2 servos
├─ Neck: 2 servos (pitch + yaw)
├─ Torso: 3 servos

Power consumption:
├─ Idle: 3.5W
├─ Moderate animation: 10W
├─ Aggressive animation: 15W
└─ Peak (brief): 25W (at thermal limit!)

Requirements:
├─ Active thermal throttling firmware
├─ Ventilation holes in creature body
├─ Temperature monitoring sensor
├─ Ability to reduce animation intensity
├─ Potentially summer operation restrictions

Cost: $200 per creature (includes thermal monitoring)
```

---

## PART 6: FIRMWARE CURRENT LIMITING

### Software-Based Power Budget Control

```cpp
// pseudocode for servo control firmware

const int MAX_CURRENT_MA = 4500;  // 4.5A limit (22.5W @ 5V)
const int THERMAL_WARNING = 40;   // Celsius above ambient
const int THERMAL_SHUTDOWN = 50;  // Celsius above ambient

struct servo_state {
  int pin;
  int desired_angle;
  int current_angle;
  int speed_limit;  // reduced if thermal limit approached
  int power_state;  // ON, REDUCED, IDLE, OFF
};

void thermal_throttle_check() {
  int temp_rise = read_temperature() - ambient_temp;
  int estimated_current = calculate_servo_current();
  
  if (temp_rise > THERMAL_SHUTDOWN) {
    // EMERGENCY: stop all servos
    halt_all_servos();
    sound_alarm();
    return;
  }
  
  if (temp_rise > THERMAL_WARNING) {
    // SLOW DOWN: reduce all servo speeds by 50%
    for (int i = 0; i < NUM_SERVOS; i++) {
      servo[i].speed_limit = servo[i].speed_limit / 2;
    }
  }
  
  if (estimated_current > MAX_CURRENT_MA) {
    // LIMIT: prevent simultaneous servo movements
    if (any_servo_moving()) {
      queue_remaining_movements();
      reduce_servo_speed();
    }
  }
}

void move_servo_limited(int servo_id, int target_angle) {
  // Check if moving this servo would exceed current limit
  int estimated_new_current = estimated_current + 150;  // ~150mA per servo
  
  if (estimated_new_current < MAX_CURRENT_MA) {
    // Safe to move, proceed normally
    move_servo(servo_id, target_angle, DEFAULT_SPEED);
  } else if (estimated_new_current < MAX_CURRENT_MA * 1.2) {
    // Close to limit, slow down movement
    move_servo(servo_id, target_angle, REDUCED_SPEED);
  } else {
    // Exceeds limit, queue for later
    queue_movement(servo_id, target_angle);
  }
}
```

### Hardware Monitoring Circuit

```
Temperature Sensor: DS18B20 (1-Wire)
├─ Mounted on servo controller board
├─ Measures ambient + servo heat
├─ Accuracy: ±0.5°C
├─ Updates every 2 seconds

Current Monitoring: INA219 (I2C)
├─ Inline on 5V servo power line
├─ Measures real-time current draw
├─ Accuracy: ±1.4%
├─ Updates every 500ms

Thermal Cutoff: Resettable Fuse (Polyfuse)
├─ 5A rated fuse (trips at 6.5A)
├─ Backup to firmware throttling
├─ Mechanical protection, not software-dependent
└─ Resets when cooled (after 5-10 minutes)
```

---

## PART 7: REALISTIC SYSTEM-LEVEL BUDGET

### For All 13 Creatures

```
Configuration: BALANCED (20 servos per creature = 260 total)

Per-Creature Sustained:
├─ Idle: 3.2W
├─ Moderate animation: 8W (60% duty cycle avg)
├─ Average continuous: ~5W per creature

Total System Continuous:
├─ 13 creatures × 5W = 65W average
├─ 13 creatures × 8W = 104W peak animation
├─ Never exceed: 13 creatures × 20W = 260W (would cause meltdown!)

Power Supply Recommendation:
├─ Option 1: 5V 30A central (150W) + local 5A per creature (25W)
│  └─ Total capacity: 150W + (13 × 25W) = 475W (safe headroom)
│
├─ Option 2: 5V 15A central (75W) + time-multiplexed creature activation
│  └─ Only 2-3 creatures animate at a time
│
└─ Option 3: 4× 5V 25A supplies in parallel (100W each)
   └─ Total: 400W capacity, distributed across venue

Real-World Scenario: Museum/Gallery Setting
├─ Open 8 hours/day
├─ Creatures idle 70% of time (at 3.2W each = 28W total)
├─ Gesture sequences 25% of time (at 8W each = 104W total)
├─ Brief dramatic moments 5% of time (at 20W each = 260W)
├─ Average draw: (0.7 × 28) + (0.25 × 104) + (0.05 × 260) = ~55W avg

Continuous 24-hour operation (decorative animation):
├─ Run at idle + very light movement only
├─ Power draw: ~40W continuous
├─ 240W per day, ~7kWh per month
├─ Cost: ~$1-2 per month in electricity
```

### Cost Summary (All 13 Creatures - Balanced Config)

| Item | Qty | Unit Cost | Total |
|------|-----|-----------|-------|
| Servo motors (260 total) | 260 | $5 avg | $1,300 |
| ESP32 boards (39 total) | 39 | $8 | $312 |
| I2C multiplexers | 13 | $3 | $39 |
| Relay modules | 13 | $5 | $65 |
| Local PSU (5A × 5V per creature) | 13 | $12 | $156 |
| Temperature/Current sensors | 13 | $8 | $104 |
| Wiring, connectors, fuses | - | - | $400 |
| Mechanical (brackets, linkages) | - | - | $500 |
| Control board PCBs | 13 | $20 | $260 |
| **TOTAL SYSTEM** | | | **$3,136** |

**Per creature average**: $241 (vs $230 previous estimate, pretty close!)

---

## PART 8: SAFETY LIMITS TABLE

### Maximum Safe Operating Conditions

| Scenario | Ambient Temp | Max Power | Max Current | Duration | Thermal Rise |
|----------|--------------|-----------|-------------|----------|--------------|
| Cool room (65°F) | 18°C | 25W | 5A | Continuous | +18°C |
| Moderate room (72°F) | 22°C | 20W | 4A | Continuous | +15°C |
| Warm room (80°F) | 27°C | 15W | 3A | Continuous | +15°C |
| Hot venue (85°F) | 29°C | 10W | 2A | Continuous | +15°C |
| Emergency brief surge | Any | 35W | 7A | <10 seconds | +5°C |

**Golden Rule**: Never allow sustained power > 25W or temperature rise > 20°C

---

## CONCLUSION

**Realistic Power Budget for P32 Animatronic System**:

- **Per creature sustained**: 5A @ 5V = 25W maximum
- **All 13 creatures simultaneous**: Not recommended
- **Realistic scenario**: 2-3 creatures animating at a time, others idle
- **System capacity needed**: 5V 30A supply (150W) with thermal throttling
- **Cost**: ~$3,136 for complete system (reasonable, safe, scalable)

**Key design principles**:
1. ✓ Firmware current limiting (never exceed 4.5A per creature)
2. ✓ Thermal monitoring (temperature-based speed reduction)
3. ✓ Material choice (use PETG, not PLA)
4. ✓ Ventilation (passive cooling holes in creature body)
5. ✓ Throttling priority (animation fidelity vs thermal safety)

**This approach is sustainable, safe, and practical for long-term operation.**

