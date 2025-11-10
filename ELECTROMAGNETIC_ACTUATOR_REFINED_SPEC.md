Electromagnetic Actuator Design - REFINED SPEC
===============================================

Context: Tiny plastic pieces for goblin face animation
Target: Eyebrows (ultra-light), Cheeks (2 per side), Jaws, Ears
Power: Minimal, distributed across many small actuators


1. EYEBROW ACTUATOR (ULTRA-LIGHT)
==================================

Purpose: Subtle raise/lower for expression (minimal movement, delicate)

Mechanical Design:
  - Housing: 25mm  10mm  12mm (PETG, very compact)
  - Filament per unit: ~5-8g
  - Operating principle: Tiny solenoid push-pull, spring return
  - Iron core: 2mm diameter  20mm length (thin rod)
  - Stroke: 5-8mm (small movement for small eyebrow)
  - Force requirement: 30g MAX (light lift against minimal resistance)
  - Spring: Very light tension (~0.5N return force)
  - Duty cycle: 5% (occasional movement during animation)

Electrical Design:
  - Wire gauge: 30 AWG (0.25mm, very fine)
  - Coil turns: 200-250 total (3 layers  70-80 turns)
  - Coil resistance: ~8-12 ohms
  - Voltage: 5V DC (low power)
  - Current per pulse: ~0.4-0.5A (minimal)
  - Power per pulse: ~0.2-0.25W
  - Driver: ULN2003 single channel (can stack multiple eyebrows per IC)

BOM per Eyebrow (2 units):
  - 3D housing (2): 2  $0.25 = $0.50
  - 2mm iron rod (2): 2  $0.02 = $0.04
  - 30 AWG copper wire (8m total): $0.05
  - Tiny neodymium magnet 3mm (2): 2  $0.03 = $0.06
  - Lightweight spring (2): 2  $0.02 = $0.04
  - ULN2003 allocation (2 channels): $0.10
  - Diodes, resistors: $0.05
  - Connectors: $0.05
  - SUBTOTAL: $0.89 for PAIR (both eyebrows)
  - Per eyebrow: $0.45


2. CHEEK ACTUATOR (2 PER SIDE = 4 TOTAL)
=========================================

Purpose: Puff/depress cheeks with fine granularity (separate left-upper, left-lower, right-upper, right-lower)

Mechanical Design:
  - Housing per unit: 20mm  12mm  10mm (tiny, localized puff)
  - Filament per unit: ~4-5g
  - Operating principle: Light solenoid push against cheek foam
  - Iron core: 2mm diameter  15mm length
  - Stroke: 3-5mm (gentle puff effect)
  - Force: 20-30g (light touch, foam compresses easily)
  - Spring: Very light return (~0.3N)
  - Duty cycle: 10% (breathing effect + expression)

Electrical Design:
  - Wire gauge: 30 AWG (fine winding)
  - Coil turns: 150-200 total (2-3 layers)
  - Coil resistance: ~6-10 ohms
  - Voltage: 5V DC
  - Current per pulse: ~0.5A
  - Power per pulse: ~0.25W
  - Driver: ULN2003 (1 channel per cheek = 4 channels total)

BOM per Cheek (4 units):
  - 3D housing (4): 4  $0.20 = $0.80
  - 2mm iron rod (4): 4  $0.02 = $0.08
  - 30 AWG copper wire (6m total): $0.04
  - Tiny neodymium magnet 3mm (4): 4  $0.03 = $0.12
  - Lightweight spring (4): 4  $0.02 = $0.08
  - ULN2003 allocation (4 channels): $0.15
  - Diodes, resistors: $0.08
  - Connectors: $0.10
  - SUBTOTAL: $1.45 for ALL CHEEKS (4 units)
  - Per cheek: $0.36


3. JAW ACTUATOR (MODERATE FORCE)
=================================

Purpose: Open/close jaw for speech (requires more force than eyebrows/cheeks)

Mechanical Design:
  - Housing: 35mm  15mm  18mm (medium-sized)
  - Filament: ~12-15g
  - Operating principle: Linear solenoid with cam linkage to jaw hinge
  - Iron core: 3mm diameter  25mm length (thicker rod for force)
  - Stroke: 15-20mm (full jaw range)
  - Force: 100-150g (overcome jaw weight + motion resistance)
  - Spring: Moderate compression (~1.5N return)
  - Duty cycle: 15% (speech requires frequent motion)

Electrical Design:
  - Wire gauge: 26 AWG (0.4mm)
  - Coil turns: 300-400 total (3-4 layers  80-120 turns)
  - Coil resistance: ~12-18 ohms
  - Voltage: 9V DC (more power for heavier load)
  - Current per pulse: ~0.7-0.8A
  - Power per pulse: ~0.7-1.0W per jaw
  - Driver: 2N2222 transistor or MOSFET (can handle higher current)

BOM per Jaw (2 units):
  - 3D housing (2): 2  $0.50 = $1.00
  - 3mm iron rod (2): 2  $0.04 = $0.08
  - 26 AWG copper wire (12m): $0.08
  - Neodymium magnet 5mm (2): 2  $0.05 = $0.10
  - Medium spring (2): 2  $0.04 = $0.08
  - 2N2222 transistor (shared): $0.10
  - Diodes, resistors: $0.10
  - Cam linkage (3D-printed, 2): 2  $0.25 = $0.50
  - Connectors: $0.10
  - SUBTOTAL: $2.14 for PAIR (both jaws)
  - Per jaw: $1.07


4. EAR ACTUATOR (2 UNITS)
=========================

Purpose: Swivel ears for attention/expression (moderate speed, holds position)

Mechanical Design:
  - Housing: 28mm  35mm  12mm (vertical profile)
  - Filament: ~10-12g
  - Operating principle: Rotary solenoid via linear actuator + cam
  - Iron core: 3mm diameter  30mm length
  - Rotation: 45-60 (perked to neutral to drooped)
  - Force: 80-120g (ear weight + air resistance)
  - Spring: Light detent (~0.8N at extremes)
  - Duty cycle: 5-8% (ears animate less frequently)

Electrical Design:
  - Wire gauge: 26 AWG
  - Coil turns: 300-350 total (3-4 layers)
  - Coil resistance: ~12-16 ohms
  - Voltage: 9V DC
  - Current per pulse: ~0.6-0.8A
  - Power per pulse: ~0.6-0.9W per ear
  - Driver: 2N2222 transistor or ULN2003 (if current within limits)

BOM per Ear (2 units):
  - 3D housing (2): 2  $0.50 = $1.00
  - 3mm iron rod (2): 2  $0.04 = $0.08
  - 26 AWG copper wire (12m): $0.08
  - Neodymium magnet 5mm (2): 2  $0.05 = $0.10
  - Light spring (2): 2  $0.03 = $0.06
  - 2N2222 transistor (shared): $0.05
  - Diodes, resistors: $0.08
  - Cam linkage (3D-printed, 2): 2  $0.25 = $0.50
  - Connectors: $0.10
  - SUBTOTAL: $2.05 for PAIR (both ears)
  - Per ear: $1.03


TOTAL SYSTEM BOM (REFINED)
==========================

  Eyebrows (2 units): $0.89
  Cheeks (4 units): $1.45
  Jaws (2 units): $2.14
  Ears (2 units): $2.05
  Shared driver ICs (2 ULN2003, 2 2N2222): $0.60
  Miscellaneous (bulk capacitors, diodes, resistors): $0.40
  Power supply (12V, 1A) + wiring: $2.00
  
  TOTAL SYSTEM COST: $9.53

  Per actuator average: $0.79
  Per facial feature (average): $1.19


POWER BUDGET (REFINED)
======================

  Peak simultaneous (all actuators):
    - Eyebrows: 2  0.25W = 0.5W
    - Cheeks: 4  0.25W = 1.0W
    - Jaws: 2  1.0W = 2.0W
    - Ears: 2  0.9W = 1.8W
    - Total peak: ~5.3W

  Typical animation (50% duty):
    - Average: ~2-3W
    - Safe supply: 12V @ 1A (12W capacity, 4-5 overhead)

  Idle state: ~0W (no holding power, springs maintain position)


COIL WINDING SPECIFICATIONS (QUICK REFERENCE)
==============================================

Eyebrow (30 AWG):
  - Core: 2mm  20mm
  - 3 layers, 70-80 turns/layer = ~250 turns
  - Estimated wire length: ~8m per eyebrow
  - Resistance check: 8-12 ohms (multimeter validation)

Cheek (30 AWG):
  - Core: 2mm  15mm
  - 2-3 layers, 60-80 turns/layer = ~200 turns
  - Estimated wire length: ~5.5m per cheek
  - Resistance check: 6-10 ohms

Jaw (26 AWG):
  - Core: 3mm  25mm
  - 3-4 layers, 80-120 turns/layer = ~400 turns
  - Estimated wire length: ~12m per jaw
  - Resistance check: 12-18 ohms

Ear (26 AWG):
  - Core: 3mm  30mm
  - 3-4 layers, 75-100 turns/layer = ~350 turns
  - Estimated wire length: ~12m per ear
  - Resistance check: 12-16 ohms


ASSEMBLY PRIORITY
=================

Phase 1 (Prototype): Single eyebrow
  - Validate 30 AWG winding technique
  - Confirm spring tension for 30g limit
  - Test 5V drive with ULN2003

Phase 2 (Scale to pair): Both eyebrows
  - Verify timing synchronization
  - Measure actual current draw vs. calculated

Phase 3 (Add cheeks): Single cheek, then all 4
  - Same winding approach as eyebrows (30 AWG)
  - Validate 4-channel ULN2003 driving

Phase 4 (Add jaws): Both jaws
  - Thicker wire (26 AWG), heavier coils
  - Validate 2N2222 transistor drive at 9V

Phase 5 (Add ears): Both ears
  - Same driver as jaws, same voltage tier (9V)

Phase 6 (Integration): Wire harness + control firmware


MATERIAL SOURCING (ESTIMATED COSTS)
===================================

  Bulk spools (sufficient for 20+ actuators each):
    - 30 AWG copper wire (25m spool): $3-5
    - 26 AWG copper wire (25m spool): $4-6
    - 2mm steel rod stock (100mm): $1-2
    - 3mm steel rod stock (100mm): $1-2
    - Neodymium magnet assortment (100): $5-8
    - Compression spring assortment: $5-10
    - PETG filament (500g): $8-12
    - ABS filament (500g): $8-12

  ICs & discrete components (bulk):
    - ULN2003 ICs (10): $3-5
    - 2N2222 transistor (50): $2-3
    - Diode assortment (100): $2-3
    - Resistor assortment: $2-3
    - Capacitor assortment: $2-3

  Total startup material cost: ~$50-80 (covers 20-50 actuators)


GPIO PIN MAPPING (EXAMPLE)
==========================

  ESP32 GPIO assignments (8 outputs needed):
    - GPIO 14: Eyebrow Left
    - GPIO 15: Eyebrow Right
    - GPIO 16: Cheek Left Upper
    - GPIO 17: Cheek Left Lower
    - GPIO 18: Cheek Right Upper
    - GPIO 19: Cheek Right Lower
    - GPIO 20: Jaw Left
    - GPIO 21: Jaw Right
    - GPIO 22: Ear Left
    - GPIO 23: Ear Right

  Alternate (if GPIO scarce): Use PCA9685 I2C expander
    - All 10 actuators on single I2C bus
    - GPIO 21 (SDA), GPIO 22 (SCL)


NEXT IMMEDIATE STEPS
====================

1. Prototype single eyebrow:
   - Wind 30 AWG coil on 2mm rod (8-10m wire, ~250 turns)
   - Measure coil resistance (should be 8-12 ohms)
   - Assemble with light spring, 3mm magnet
   - Mount in 3D-printed housing
   - Test with 5V ULN2003 driver at GPIO pulse rate

2. Measure actual performance:
   - Current draw during pulse
   - Stroke distance achieved
   - Force exerted (scale weight test)
   - Response time (oscilloscope or GPIO logging)

3. Iterate if needed:
   - More turns = more force but slower response
   - Thinner wire = less resistance, less force per turn
   - Spring tension adjustment = stroke control

4. Once eyebrow validated, clone design to cheeks (same coil spec, just 4 copies)

5. Then scale jaw/ear designs separately (heavier coils, 26 AWG, 9V drive)


DOCUMENT STATUS: REFINED SPECIFICATION FOR TINY PLASTIC ANIMATRONIC PIECES
NOT PART OF BUILD SYSTEM - REFERENCE ONLY
ASCII ENCODING - NO SPECIAL CHARACTERS
