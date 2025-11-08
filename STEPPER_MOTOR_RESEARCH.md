[STANDALONE DOCUMENTATION - NOT PART OF PROJECT BUILD - HUMAN REFERENCE ONLY]

# Tiny Stepper Motor Research - Power, Torque, Cost Analysis

For animatronic expression and fine movement requiring dozens to hundreds of motors.

---

## Executive Summary

For goblin head expression animation using dozens to hundreds of tiny stepper motors:

**Best Value Options**:
- Small reduction gearmotors: $3.96-$4.95 each (Adafruit 28BYJ-48, 1/16 geared)
  - Power: 5V, approx 100-150mA per phase
  - Torque: 150 gram-force*cm (15 N*mm, 2 oz-force*in)
  - Steps/rev: 516 (geared 1/16 reduction)
  
- Miniature steppers: $148-$151 each (Pololu Sanyo SH2141, NEMA 0)
  - Power: 6.3V, 0.3 A/phase
  - Torque: 66 g-cm (0.92 oz-in)
  - Size: 14x30mm, 27-28g
  - 200 steps/rev

- Pancake steppers: $74.95-$79.95 (Pololu Sanyo NEMA 17, low profile)
  - Power: 4.5-5.9V, 1A/phase
  - Torque: 1-2.2 kg-cm (14-30 oz-in)
  - Size: 50x11-16mm (ultra flat)
  - 200 steps/rev

---

## DETAILED MOTOR OPTIONS

### Tier 1: Budget Reduction Motors (BEST FOR QUANTITY)

**Adafruit 28BYJ-48** - Small Reduction Stepper Motor

| Parameter | Value |
|-----------|-------|
| Cost | $3.96-4.95 (bulk pricing) |
| Voltage | 5V DC |
| Power per phase | approx 100-150mA |
| Holding torque | 150 gram-force*cm |
| Torque (oz-in) | 2 oz-force*in |
| Steps/rev | 516 (1/16 geared down) |
| Speed | up to 12 RPM (with overdriving), 6 RPM nominal |
| Motor type | Unipolar, 32-step base |
| Dimensions | 28mm dia, 20mm tall |
| Shaft | 5mm dia, flattened |
| Weight | 37g |
| Cable | 9 inch (23cm), 5-pin connector |
| Supply per 100 units | USD 396-495 |

Pros: Extremely cheap for quantity, built-in reduction (high torque output), simple wiring
Cons: Slow speed, not ideal for rapid expression changes, bulky for dozens of motors
Use case: Large-scale expression panels where speed not critical, budget is tight
Cost for 100 motors: approx $400

---

### Tier 2: Miniature Stepper Motors (SMALLEST, HIGHEST DENSITY)

**Pololu Sanyo Miniature (SH2141-5541)** - NEMA 0 equivalent

| Parameter | Value |
|-----------|-------|
| Cost | $148.44 |
| Voltage | 6.3V |
| Current per phase | 0.3A (300mA) |
| Holding torque | 66 g-cm |
| Torque (oz-in) | 0.92 oz-in |
| Steps/rev | 200 (1.8 degrees) |
| Motor size | 14x30mm |
| Weight | 27g (smallest available) |
| Shaft | 4mm diameter, round |
| Power per motor | 6.3V x 0.3A = 1.89W |

Pros: Smallest available, ultra-compact for dense array, proven availability
Cons: Very expensive per unit ($148), limited torque (0.66 kg-cm)
Use case: Microexpression details, fine eye movements, precision control
Cost for 100 motors: approx $14,844

---

### Tier 3: NEMA 8 Stepper Motors (BEST BALANCE)

**Pololu SOYO NEMA 8** - Multiple options

#### Option A: 20x30mm, 3.9V, 0.6A/phase
| Parameter | Value |
|-----------|-------|
| Model | SY20STH30-0604A |
| Cost | $46.32 |
| Voltage | 3.9V |
| Current | 0.6A/phase |
| Torque | 180 g-cm (4 oz-in) |
| Weight | 60g |
| Size | 20x30mm |
| Power | 3.9V x 0.6A = 2.34W |

#### Option B: 20x42mm, 4.3V, 0.8A/phase
| Parameter | Value |
|-----------|-------|
| Model | SY20STH42-0804A |
| Cost | $55.92 |
| Voltage | 4.3V |
| Current | 0.8A/phase |
| Torque | 300 g-cm (4.2 oz-in) |
| Weight | 80g |
| Size | 20x42mm |
| Power | 4.3V x 0.8A = 3.44W |

Cost for 100 motors (Option A): approx $4,632
Cost for 100 motors (Option B): approx $5,592

---

### Tier 4: NEMA 11 Stepper Motors (MEDIUM TORQUE)

**Pololu SOYO NEMA 11**

#### Option A: 28x32mm, 3.8V, 0.67A/phase
| Parameter | Value |
|-----------|-------|
| Model | SY28STH32-0674A |
| Cost | $39.92 |
| Voltage | 3.8V |
| Current | 0.67A/phase |
| Torque | 600 g-cm (8.3 oz-in) |
| Weight | 110g |
| Size | 28x32mm |
| Power | 3.8V x 0.67A = 2.55W |

#### Option B: 28x45mm, 4.5V, 0.67A/phase
| Parameter | Value |
|-----------|-------|
| Model | SY28STH45-0674A |
| Cost | $49.52 |
| Voltage | 4.5V |
| Current | 0.67A/phase |
| Torque | 950 g-cm (13 oz-in) |
| Weight | 140g |
| Size | 28x45mm |
| Power | 4.5V x 0.67A = 3.02W |

Cost for 100 motors (Option A): approx $3,992
Cost for 100 motors (Option B): approx $4,952

---

### Tier 5: NEMA 14 Stepper Motors (HIGH EFFICIENCY)

**Pololu SOYO NEMA 14** - Efficient designs

| Model | Voltage | Current | Torque | Size | Cost | Power |
|-------|---------|---------|--------|------|------|-------|
| SY35ST26-0284A | 7.4V | 0.28A | 650 g-cm | 35x26mm | $30.32 | 2.07W |
| SY35ST28-0504A | 10V | 0.5A | 1 kg-cm | 35x28mm | $30.32 | 5W |
| SY35ST36-1004A | 2.7V | 1A | 1.4 kg-cm | 35x36mm | $35.12 | 2.7W |

Cost for 100 motors (cheapest): approx $3,032

---

### Tier 6: NEMA 17 Stepper Motors (MOST COMMON)

**Adafruit NEMA-17** - Standard production motor

| Parameter | Value |
|-----------|-------|
| Cost | $11.20-14.00 per unit |
| Voltage | 12V |
| Current | 350mA/phase |
| Torque | 2 kg-cm (28 oz-in holding) |
| Steps/rev | 200 (1.8 degrees) |
| Size | 42x42mm (standard mounting) |
| Weight | 280-350g |
| Power | 12V x 0.35A = 4.2W |

Cost for 100 motors (bulk): approx $1,120

Pros: Cheapest in bulk, best availability, proven reliability
Cons: Large for expression (42x42mm), consumes more power
Use case: Major structure movements (jaw, head tilt), large-scale positioning

**Pololu NEMA 17** (2.8V variant)

| Parameter | Value |
|-----------|-------|
| Model | SY42STH38-1684A |
| Cost | $35.12 |
| Voltage | 2.8V |
| Current | 1.7A/phase |
| Torque | 3.7 kg-cm (51 oz-in) |
| Size | 42x38mm |
| Power | 2.8V x 1.7A = 4.76W |

---

### Tier 7: NEMA 17 Pancake Motors (LOW PROFILE)

**Sanyo Pancake Motors** - Flat design for expression plates

#### Option A: 42x11.6mm, 3.5V, 1A
| Parameter | Value |
|-----------|-------|
| Model | SS2421-5041 |
| Cost | $82.44 |
| Voltage | 3.5V |
| Current | 1A/phase |
| Torque | 850 g-cm (12 oz-in) |
| Height | 11.6mm (ultra flat) |
| Power | 3.5V x 1A = 3.5W |

Cost for 100 motors: approx $8,244

#### Option B: 50x11mm, 4.5V, 1A
| Parameter | Value |
|-----------|-------|
| Model | SS2501-8040 |
| Cost | $74.95 |
| Voltage | 4.5V |
| Current | 1A/phase |
| Torque | 1 kg-cm (14 oz-in) |
| Height | 11mm (thinnest) |
| Power | 4.5W |

Cost for 100 motors: approx $7,495

#### Option C: 50x16mm, 5.9V, 1A
| Parameter | Value |
|-----------|-------|
| Model | SS2502-8040 |
| Cost | $79.95 |
| Voltage | 5.9V |
| Current | 1A/phase |
| Torque | 2.2 kg-cm (30 oz-in) |
| Height | 16mm |
| Power | 5.9W |

Cost for 100 motors: approx $7,995

---

### Tier 8: NEMA 23 Stepper Motors (LARGE SCALE)

**Pololu SOYO NEMA 23** - Heavy duty

| Model | Voltage | Current | Torque | Size | Cost | Power |
|-------|---------|---------|--------|------|------|-------|
| SY57STH41-1006A | 5.7V | 1A | 4 kg-cm | 56.4x41mm | $57.52 | 5.7W |
| SY57STH56-1006A | 7.4V | 1A | 9 kg-cm | 56.4x56mm | $76.72 | 7.4W |
| SY57STH76-1006A | 8.6V | 1A | 14 kg-cm | 56.4x76mm | $95.92 | 8.6W |

Use case: Major limb movements, large jaw actuation, primary structure
Cost for 50 motors: approx $2,876 (SY57STH56-1006A)

---

## POWER BUDGET ANALYSIS

### For 100 tiny motors (example: NEMA 8 or smaller)

**Minimum safe power supply:**
- 100 motors x 0.6A per phase = 60A total current needed
- At 3.9V: 100 x 2.34W = 234W total power needed
- 60A x 5V = 300W power supply minimum (with headroom)

**Reality**: 
- Not all motors run simultaneously
- Microstepping reduces current draw by 50-70%
- Realistic active load: 20-30 motors at once
- 30 motors x 1.5A = 45A, 30 x 3W = 90W continuous

**Recommendation for 100 motors:**
- Single 24V 10A supply (240W) - overkill but safe
- OR modular approach: Multiple 5V 20A supplies (100W each)

---

## COST SCALING

### 100 Motors Scenarios

| Scenario | Motor Type | Unit Cost | Total Cost | Power per motor | Total Power |
|----------|-----------|-----------|-----------|-----------------|------------|
| Ultra-budget | 28BYJ-48 | $4 | $400 | 0.5W | 50W |
| Compact | NEMA 8 | $46 | $4,600 | 2.3W | 230W |
| Balanced | NEMA 11 | $40 | $4,000 | 2.5W | 250W |
| Fine detail | Miniature | $148 | $14,800 | 1.9W | 190W |
| Pancake flat | SS2421 | $82 | $8,200 | 3.5W | 350W |
| Large limbs | NEMA 23 | $76 | $7,600 | 7.4W | 740W |

---

## DESIGN RECOMMENDATIONS

### Expression Animation (dozens of small motors)

**Optimal choice: Pololu NEMA 8 (SY20STH30-0604A)**
- Cost: $46.32 per motor, $4,632 for 100
- Power: 2.34W each, 234W for 100 (manageable)
- Torque: 180 g-cm (enough for eyelids, eyebrows, mouth)
- Size: 20x30mm (dense placement possible)
- Availability: In stock, proven supplier

**Alternative for ultra-compact: Pololu Pancake NEMA 17**
- Cost: $74.95-82.44, $7,495-8,244 for 100
- Power: 3.5-5.9W each
- Height: 11-16mm (very flat for mounting plates)
- Torque: 850 g-cm to 2.2 kg-cm (excellent for fine control)
- Use case: Built into thin expression plates, premium option

**Budget alternative: Adafruit 28BYJ-48**
- Cost: $3.96 bulk, $396 for 100 (83% savings!)
- Power: ~0.5W per motor, manageable
- Torque: 150 g-cm (lower but acceptable for light duty)
- Speed: Slower response (6 RPM nominal)
- Use case: Large arrays where response time not critical

---

## CONTROL COMPLEXITY

### Microstepping drivers

All NEMA steppers benefit from microstepping to:
- Reduce audible noise
- Improve smoothness (less jerky motion)
- Reduce current draw by 50-70% in idle phases

**Driver options per motor:**
- A4988 ($3-5): Basic, adequate for single coils
- TMC2209 ($5-8): Smart current control, quiet
- DRV8825 ($3-4): Good general purpose

**For 100 motors:**
- Cost: $100-200 in drivers
- Wiring: Minimize trace runs to drivers (critical for noise)
- Supply voltage: Regulate tightly (steppers sensitive to input noise)

---

## THERMAL CONSIDERATIONS

### Power dissipation for 100 motors

**Active scenario (30 motors moving):**
- NEMA 8: 30 x 2.34W = 70W (minimal thermal load)
- NEMA 17: 30 x 4.2W = 126W (needs airflow)
- NEMA 23: 30 x 7.4W = 222W (requires heat management)

**Motor temperature rise:**
- Most NEMA motors rated for 80-100 deg C continuous
- At 70W dissipated in metal housing: approx +5-10 deg C above ambient (with airflow)
- In enclosed head: approx +15-20 deg C rise (requires ventilation)

**Recommendation:** 
Small circulation fans inside goblin head, vent holes for thermal exchange

---

## SOURCING & AVAILABILITY

### Recommended suppliers

1. **Pololu Robotics** (pololu.com)
   - Best selection of small steppers
   - NEMA 8, 11, 14, 17, 23 in stock
   - Reliable shipping, bulk discounts

2. **Adafruit** (adafruit.com)
   - 28BYJ-48 reduction motors (budget option)
   - NEMA-17 standard motors
   - Excellent documentation

3. **Amazon / AliExpress** (bulk option)
   - 28BYJ-48 motors: $2-4 in 10+ packs
   - NEMA-17: $8-12 in bulk
   - Slower shipping, hit-or-miss quality

### Lead times
- In-stock items: 1-3 days
- Build-to-order: 2-4 weeks
- International: 1-2 weeks

---

## FUTURE OPTIMIZATION

### Integrated solutions to research

1. **Integrated stepper+driver modules** (compact all-in-one)
   - Would eliminate separate wiring, reduce complexity
   - Example: DRV8833 breakout boards with onboard logic

2. **Geared steppers** (like 28BYJ-48 but NEMA size)
   - Higher torque output, lower speed
   - Better for fine positioning without microstepping complexity

3. **Servo motors vs steppers** for some applications
   - Servos more compact per movement axis
   - Could mix stepper + servo for coarse/fine control

4. **Magnetic encoders** integrated with steppers
   - Feedback for position verification
   - Example: Pololu Sanyo pancake with encoder ($288.69)

---

## Summary: Recommendation for Goblin Head

**Phase 1 (Prototype)**: 20 x NEMA 8 steppers
- Cost: approx $930
- Power: 47W total, 10-15W active
- Use for: Eye movement (4), eyebrow (4), mouth (4), ear (2), other (6)

**Phase 2 (Full expression)**: 100 x NEMA 8 steppers
- Cost: approx $4,632
- Power: 234W total, 70-100W active (with 30 motors moving)
- Use for: Dense facial grid (eyes, brows, cheeks, mouth, ears, jaw)

**Phase 3 (Premium option)**: Mix NEMA 8 + Pancake NEMA 17
- 50 x NEMA 8 ($2,316) for gross movements
- 50 x Pancake NEMA 17 ($4,122) for ultra-flat detail layers
- Total: $6,438 for 100 motors, superior flat profile

**Budget fallback**: 200 x Adafruit 28BYJ-48 for $792
- Works for large-scale expression grid
- Slower response but massive cost savings
- Good for research/prototype phase
