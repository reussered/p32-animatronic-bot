[STANDALONE DOCUMENTATION - NOT PART OF PROJECT BUILD - HUMAN REFERENCE ONLY]

# Stepper Motor Quick Reference - For Expression Animation

---

## TL;DR - Best Option for Goblin Head Expression

**Pololu NEMA 8** (SY20STH30-0604A)
- Cost: $46.32 each, $4,632 for 100 units
- Power: 3.9V, 0.6A/phase = 2.34W per motor
- Torque: 180 g-cm (4 oz-in) - good for eyelids, eyebrows
- Size: 20x30mm - small enough for dense placement
- Steps: 200 steps/rev (1.8 degrees)

---

## Cost Tiers at a Glance

For 100 motors (typical expression grid size):

| Tier | Motor Type | Size | Cost Total | Cost Each | Power/motor | Use |
|------|-----------|------|-----------|-----------|------------|-----|
| 1 | Adafruit 28BYJ-48 | 28x20mm | $400 | $4 | 0.5W | Ultra-budget, slow |
| 2 | NEMA 8 | 20x30mm | $4,632 | $46 | 2.34W | RECOMMENDED |
| 3 | NEMA 11 | 28x32mm | $3,992 | $40 | 2.55W | Compact, low cost |
| 4 | NEMA 17 Pancake | 50x11mm | $7,495 | $75 | 3.5W | Ultra-flat mounting |
| 5 | NEMA 17 Standard | 42x42mm | $1,120 | $11 | 4.2W | General purpose |
| 6 | NEMA 23 | 56x56mm | $7,600 | $76 | 7.4W | Large limbs only |
| 7 | Miniature | 14x30mm | $14,844 | $148 | 1.9W | Micro-detail, very expensive |

---

## Power Budget (100 motors)

**Total if all running**: 230-350W (depends on motor type)

**Realistic active power** (30 motors moving at once):
- NEMA 8: 70W
- NEMA 17: 126W
- NEMA 23: 222W

**Recommended power supply for 100 x NEMA 8:**
- Option A: Single 24V/10A supply = 240W (overkill but safe)
- Option B: Multiple 5V/20A supplies (better modularity)

---

## Torque Comparison

What can each motor type control?

| Motor | Torque | Typical Use |
|-------|--------|------------|
| 28BYJ-48 | 150 g-cm | Light eyelid flutters, small movements |
| NEMA 8 | 180 g-cm | Eyebrows, eyelids, mouth corners |
| NEMA 11 | 600 g-cm | Eye rotation, jaw control |
| NEMA 17 | 2 kg-cm | Large jaw movements, limb positioning |
| NEMA 23 | 9-14 kg-cm | Major limb movements, head tilt |

---

## Control Wiring

### Per motor
- 4 wires (bipolar stepper)
- A4988 driver: cost $3-5
- Total cost per axis: motor + driver + PWM pin on ESP32

### For 100 motors
- 400 wires (manageable via ribbon cable)
- 25 driver boards at $3-5 each = $75-125
- Multiplexing strategy on ESP32 (use SPI or I2C expand)

---

## Stepper Motor Suppliers (In Stock)

1. **Pololu.com** - Best selection, NEMA 8/11/14/17/23
2. **Adafruit.com** - 28BYJ-48, NEMA-17 standard
3. **Amazon** - Bulk 28BYJ-48 ($2-4 each in 10+ packs)

---

## Scaling Scenarios

### Scenario A: Eyes + Mouth (20 motors)
- Cost: $920
- Power: 47W total
- Motors: 4 NEMA 8 for eyes (pan/tilt each) + 4 for eyebrows + 4 for mouth + 8 backup

### Scenario B: Full Goblin Face (100 motors)
- Cost: $4,632
- Power: 234W total (70W active)
- Motors: Distributed across eyes, eyebrows, cheeks, mouth, ears, jaw

### Scenario C: Budget Expression (100 motors, 28BYJ-48)
- Cost: $400
- Power: 50W total
- Motors: Same distribution but slower response time (~6 RPM max)

### Scenario D: Premium Flat (100 motors, mixed)
- Cost: $6,438
- 50 x NEMA 8 (coarse) + 50 x Pancake NEMA 17 (fine detail)
- Power: 280W total
- Best for theatrical expression, ultra-flat mounting plates

---

## Motor Specs Quick Reference

### Adafruit 28BYJ-48
- 5V, 100-150mA
- 150 g-cm torque
- 516 steps/rev (1/16 geared)
- Cost: $4

### Pololu NEMA 8 (Recommended)
- 3.9V, 0.6A/phase
- 180 g-cm torque
- 200 steps/rev
- Cost: $46

### Pololu NEMA 11
- 3.8-4.5V, 0.67A/phase
- 600-950 g-cm torque
- 200 steps/rev
- Cost: $40-50

### Pololu NEMA 17 (Standard)
- 12V, 350mA
- 2 kg-cm torque
- 200 steps/rev
- Cost: $11 (bulk)

### Sanyo Pancake NEMA 17
- 3.5-5.9V, 1A
- 850 g-cm to 2.2 kg-cm torque
- 11-16mm height (ultra-flat)
- 200 steps/rev
- Cost: $75-82

### Pololu NEMA 23
- 5.7-8.6V, 1A
- 4-14 kg-cm torque
- 200 steps/rev
- Cost: $57-96

---

## Critical Design Notes

1. **Microstepping drivers required**
   - Reduces noise by 50%+
   - Smoother animation motion
   - Lower idle current draw

2. **Heat management**
   - 100 motors at full power = 230W
   - Needs ventilation holes in head
   - Small circulation fans recommended

3. **Cable routing**
   - 100 motors = 400 wires minimum
   - Use ribbon cable + connectors
   - Route away from noisy power supplies

4. **Power supply filtering**
   - Steppers create EMI when switching
   - Use 100uF capacitors on driver boards
   - Separate ground plane for logic vs power

5. **GPIO/Timer limitation**
   - ESP32 has limited PWM outputs (16 timers)
   - Use GPIO expanders (PCA9685, etc.)
   - OR multiplex via shift registers

---

## Next Steps

1. Decide on motor tier (recommend NEMA 8)
2. Calculate exact motor count needed for face coverage
3. Design mechanical mounting plates (3D print)
4. Order small test batch (10 motors) to validate
5. Build driver circuit and test control firmware
6. Scale to full 100+ motor configuration
