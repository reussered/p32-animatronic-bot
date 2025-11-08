[STANDALONE DOCUMENTATION - NOT PART OF PROJECT BUILD - HUMAN REFERENCE ONLY]

# Stepper Motor Budget Reality Check - USD 8-10 per Complete Unit

**Constraint**: Total cost (motor + controller + mounting) must be under USD 8-10 per axis
**Scale**: 50-200+ motors for full expression system
**Total budget**: USD 400-2000 maximum for expression subsystem

---

## The Math Problem

Previous research recommended NEMA 8 at $46 per motor - this is COMPLETELY UNREALISTIC for dozens+ units.

Required budget: USD 8-10 per complete motor unit including all parts

---

## Ultra-Budget Options - USD 5-8 Range

### Option 1: Adafruit 28BYJ-48 Reduction Motor (BEST VALUE)

**Motor**: Adafruit 28BYJ-48
- Cost: USD $3.96-4.95 (bulk pricing, Amazon 10+ packs)
- Voltage: 5V DC
- Holding torque: 150 g-cm (adequate for light expression)
- Speed: 6-12 RPM
- Gearing: 1/16 reduction (516 steps/rev)
- Weight: 37g
- Size: 28mm dia, 20mm tall

**Driver**: ULN2803 Darlington array (8 motors per chip)
- Cost: USD $0.50-1.00 per chip (bulk)
- Channels: 8 (one chip drives 8 motors)
- Logic: Simple 4-wire unipolar control
- Interface: Direct GPIO (no I2C complexity)

**Mounting bracket**: 3D printed PLA
- Cost: USD 0.50-1.00 (filament only)
- Design: Servo-style L-bracket
- Assembly: 2 M2 screws + press-fit

**Total per motor unit**: USD 4.00 + 0.10 + 0.50 = **USD 4.60**
**For 100 motors**: USD 460 total (motor + driver + bracket)

---

### Option 2: NEMA 11 Budget Clone (AliExpress/Amazon)

**Motor**: Generic NEMA 11 stepper (AliExpress bulk)
- Cost: USD $5-8 per motor (100+ packs, very cheap)
- Voltage: 4-5V
- Torque: 600 g-cm
- Size: 28x28mm
- Quality: Hit-or-miss, but serviceable

**Driver**: A4988 on cheap breakout board
- Cost: USD $0.80-1.50 (bulk, no branding)
- Interface: Step/direction (SPI-like)
- Supply: 5V logic, motor coil rated to input voltage

**Mounting**: 3D printed bracket
- Cost: USD $0.50

**Total per motor unit**: USD 6.50 + 1.20 + 0.50 = **USD 8.20**
**For 100 motors**: USD 820 total

---

### Option 3: Automotive Stepper (X27.168 Gauge Motor)

**Motor**: Automotive gauge stepper (salvage/bulk)
- Cost: USD $1-3 per motor (salvaged from junk gauge clusters)
- Voltage: 5V
- Torque: Very low (~50 g-cm) but works for tiny movements
- Size: Compact, integrated housing
- Source: eBay automotive parts, bulk lots
- Advantage: Pre-integrated with mechanical stop and detents

**Driver**: Simple 4-wire unipolar driver
- Cost: USD $0.50 (ULN2803 shared among 8 motors)

**Mounting**: Minimal (already has integrated bracket)
- Cost: USD $0

**Total per motor unit**: USD 2.00 + 0.10 = **USD 2.10**
**For 100 motors**: USD 210 total (cheapest!)
**Downside**: Salvage sourcing unreliable, torque very weak

---

## Cost Breakdown for 100-Motor System

### Budget Scenario A: 28BYJ-48 (Recommended for Budget)
```
100x 28BYJ-48 motors @ $4.00 ea      = USD $400
13x ULN2803 chips @ $0.80 ea         = USD $10 (drives 104 motors)
100x 3D printed brackets @ $0.50 ea  = USD $50
Wiring/connectors/misc               = USD $40
TOTAL                                = USD $500
```
Cost per motor unit: **USD 5.00**

### Budget Scenario B: NEMA 11 from AliExpress
```
100x NEMA 11 motors @ $6.50 ea       = USD $650
100x A4988 drivers @ $1.00 ea        = USD $100
100x 3D printed brackets @ $0.50 ea  = USD $50
Wiring/connectors/misc               = USD $50
TOTAL                                = USD $850
```
Cost per motor unit: **USD 8.50**

### Budget Scenario C: Mixed (50 cheap + 50 better quality)
```
50x 28BYJ-48 @ $4.00 ea              = USD $200
50x NEMA 11 @ $6.00 ea               = USD $300
7x ULN2803 @ $0.80 ea                = USD $6
50x A4988 @ $1.00 ea                 = USD $50
100x 3D printed brackets             = USD $50
Wiring/connectors/misc               = USD $50
TOTAL                                = USD $656
```
Cost per motor unit: **USD 6.56**

---

## Power Consumption at USD 8-10 Budget

**28BYJ-48 system (100 motors)**:
- Per motor: ~100-150mA at 5V = 0.5W
- All 100 at full: 50W
- Realistic active (30 motors): 15W
- Power supply: Single 5V/3A USB = USD $5-10

**NEMA 11 system (100 motors)**:
- Per motor: ~600mA at 5V = 3W
- All 100 at full: 300W (unrealistic)
- Realistic active (30 motors): 90W
- Power supply: 5V/20A = USD $30-50

---

## Control Strategy at USD 8-10 Budget

### Option A: Direct GPIO (Simplest, No I2C)

**For 28BYJ-48 unipolar steppers**:
- Each motor needs 4 GPIO lines
- 100 motors = 400 GPIO lines needed
- ESP32 has 34 usable GPIO
- **Solution**: Use shift registers (74HC595)
  - 8x 74HC595 chips = 64 output lines per chip (512 total)
  - Cost: USD $0.50 per chip x 8 = USD $4 total
  - Connected via SPI to ESP32 (3 GPIO pins)

**Total control cost**: USD 4 for shift registers + USD 10-15 for ULN2803 + wiring = **USD 20 total**

### Option B: I2C Multiplexing (Moderate)

**PCA9685 PWM expanders**:
- 4x PCA9685 = 64 channels
- Cost: USD $3-5 per board x 4 = USD $20 total
- But: Overkill for simple on/off control
- Better for smooth PWM, not needed for steppers

---

## Realistic System Architecture USD 8-10

### Component Breakdown

**Per Motor Unit**:
1. Stepper motor: USD 4-6
2. Motor controller (driver): USD 1-2 (shared across 8 motors if unipolar, or per-motor if bipolar)
3. Mounting bracket: USD 0.50-1
4. Wiring/connectors (allocated): USD 0.50

**Total per unit**: USD 6-9.50

### Full 100-Motor System Budget

| Component | Quantity | Unit Cost | Total |
|-----------|----------|-----------|-------|
| 28BYJ-48 motors | 100 | $4.00 | $400 |
| ULN2803 driver chips | 13 | $0.80 | $10 |
| 74HC595 shift registers | 8 | $0.50 | $4 |
| 3D printed brackets | 100 | $0.50 | $50 |
| A4988 modules (backup) | 0 | - | $0 |
| Wiring/connectors/misc | - | - | $50 |
| Power supply (5V/3A) | 1 | $10 | $10 |
| **TOTAL** | | | **$524** |
| **Per motor** | | | **$5.24** |

---

## Reality Check: Torque vs Cost

**At USD 4 per motor (28BYJ-48)**:
- Torque: 150 g-cm
- Good for: Eyelid flutter, eyebrow twitch, mouth corner
- NOT good for: Heavy jaw movement, eye rotation, major expression changes

**Trade-off**:
- Get many small motors for micro-expressions (cheap, requires many units)
- Use fewer larger motors for macro-movements (expensive, fewer units needed)

**Solution**: Hybrid system
- 50x 28BYJ-48 ($200) for fine detail (eyes, eyebrows, mouth)
- 20x NEMA 11 ($120-150) for larger movements (jaw, ear, cheek)
- Total: 70 motors, USD $320-350, still under budget

---

## Sourcing at Budget Prices

### Motor Sources

**Adafruit 28BYJ-48**:
- Direct: USD $4.95 per unit
- Amazon bulk (10+): USD $3.96-4.50
- AliExpress (100+): USD $2-3 (slow shipping)

**NEMA 11 Budget Clones**:
- AliExpress bulk (100+): USD $4-6
- Amazon bulk: USD $6-8
- Sparkfun/Pololu: USD $35-40 (too expensive)

**Driver Chips**:
- ULN2803: USD $0.50-1.00 on AliExpress
- A4988 breakout boards: USD $0.80-1.50
- Local electronics: USD $2-5

**74HC595 Shift Registers**:
- AliExpress: USD $0.20-0.50 per chip
- Local electronics shop: USD $0.50-1.00

---

## Prototype Path at USD 8-10 Budget

### Phase 1: Test & Validate (USD 50)
- 5x 28BYJ-48 motors: USD $20
- 1x ULN2803 + breadboard: USD $5
- 1x ESP32 dev board: USD $10
- Wiring/misc: USD $15

### Phase 2: Proof of Concept (USD 100)
- 20x 28BYJ-48 motors: USD $80
- 3x ULN2803 chips: USD $3
- Mounting brackets (3D print): USD $10
- Wiring/power: USD $7

### Phase 3: Full Expression (USD 500)
- 100x 28BYJ-48 motors: USD $400
- Drivers + brackets + wiring: USD $100

---

## Conclusion: USD 8-10 Budget is Achievable

**Best strategy for USD 8-10 per motor unit**:
1. Use Adafruit 28BYJ-48 reduction motors (USD $4 bulk)
2. Use ULN2803 driver chips (USD $0.10 per motor, shared)
3. Use 3D-printed mounting brackets (USD $0.50-1.00)
4. Use shift registers for GPIO expansion (USD $0.05 per motor)

**Expected system**:
- 100 motors complete system: USD 500-600
- Cost per motor: USD 5-6
- Remaining budget: USD 200-400 for mounting structure and testing

**Performance trade-off**:
- Slower speed (6 RPM vs 50+ RPM)
- Lower torque (150 g-cm vs 600+ g-cm)
- But still adequate for facial expression if well-designed mechanically

**Key insight**: The motors are cheap. The expensive part is the CONTROL ELECTRONICS and MECHANICAL DESIGN. At budget prices, focus on:
1. Efficient driver sharing (multiple motors per driver)
2. Clever mechanical design (leverage vs motorforce)
3. Smart animation sequencing (not all motors at once)
