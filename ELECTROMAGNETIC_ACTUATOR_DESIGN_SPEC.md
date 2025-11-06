# Electromagnetic Actuator Design Specification
## DIY Motor Assembly for Goblin Face Animation

**Project Context:** Build cost-effective animatronic facial features using 3D-printed housings + copper coil electromagnets + small magnets and iron cores.

**Target Features:** Eyebrows, Jaws, Cheeks, Ears
**Budget Target:** $2-3 per actuator unit (housing + coil + magnets + driver circuit)
**Fabrication:** 3D printing (housings) + hand-wound coils (copper wire) + bulk magnet/iron sourcing

---

## 1. EYEBROW ACTUATOR
### Purpose
Raise/lower eyebrows independently for expression (sad, angry, surprised, neutral)

### Mechanical Design
**Housing Dimensions:**
- Width: 40mm (spans eyebrow breadth)
- Height: 15mm (thin profile fits under skin texture)
- Depth: 20mm (allows space for coil windings)
- Material: PETG or ABS (flexible, durable)

**Operating Principle:**
- **Solenoid push-pull actuator** with spring return
- Iron rod core (4mm diameter, 30mm length) moves linearly inside housing
- Attached to eyebrow linkage via rod end (threaded or glued)
- Spring-loaded neutral position (resting at horizontal)

**Motion Spec:**
- Stroke: 15-20mm (up and down from center)
- Speed: 200-400ms per full cycle (snappy but not mechanical)
- Holding force: ~100-200g (light weight, eyebrow material is foam/silicone)
- Duty cycle: Intermittent (pulses during animation, holds position via magnetic detent)

### Electrical Design
**Coil Specification:**
- Wire gauge: 26-28 AWG (0.4-0.5mm diameter)
- Winding layers: 3-4 layers around iron core
- Turns per layer: ~100-150 turns
- Total turns: 400-600 turns
- Coil resistance: ~15-25 ohms
- Voltage: 5-12V DC (matched to ESP32 logic levels via driver)
- Power per pulse: ~0.3-1.0W (low power, pulsed operation)
- Duty cycle: 5-10% (animated eyebrow moves ~100ms per 2-second cycle)

**Driver Circuit:**
- IC: ULN2003 (7-channel Darlington array) or 2N2222 NPN transistor
- Control: Single GPIO pin per eyebrow (left/right = 2 pins total)
- Logic: Pulse width modulation (PWM) for smooth ramp up/down
- Protection: Diode across coil (1N4007) for back-EMF suppression

**Power Budget:**
- Peak (both eyebrows active): ~2W
- Average (animated): ~0.1-0.2W
- Idle (held): ~0W (unpowered, spring returns to neutral)

### BOM per Eyebrow
| Component | Qty | Unit Cost | Notes |
|-----------|-----|-----------|-------|
| 3D-printed housing | 1 | $0.50 | PETG, 15g filament |
| Iron rod (4mm × 30mm) | 1 | $0.05 | Scrap/bulk steel |
| Copper wire 26 AWG | 15m | $0.10 | Bulk spool ~$8/kg |
| Neodymium magnets 5mm | 2 | $0.10 | Positioning/detent |
| Spring (compression, 3mm) | 1 | $0.05 | Return mechanism |
| ULN2003 driver segment | 0.33 | $0.15 | Shared 7-channel IC |
| Diode 1N4007 | 1 | $0.02 | Back-EMF protection |
| Connectors (JST) | 1 | $0.10 | Control + power leads |
| **Total per eyebrow** | | **$1.05** | Two eyebrows = $2.10 |

---

## 2. JAW ACTUATOR
### Purpose
Open/close jaw for speech, eating animations, expression (smile ↔ frown)

### Mechanical Design
**Housing Dimensions:**
- Width: 50mm (spans jaw width at hinge point)
- Height: 20mm (profile matching jaw thickness)
- Depth: 25mm (room for larger coil, more force needed)
- Material: ABS (rigid, handles higher forces)

**Operating Principle:**
- **Rotary solenoid** or **linear-to-rotary linkage**
- Option A: Linear solenoid pulls jaw down via cable, spring returns up
- Option B: Rotary electromagnet engages permanent magnet for pivot action
- **Recommended: Option A** (simpler, cheaper, easier to wind)

**Motion Spec:**
- Stroke: 20-30mm vertical (full open to closed)
- Rotation: ~30-45° jaw rotation around hinge point
- Speed: 300-600ms full cycle (speech requires moderate speed)
- Holding force: ~300-500g (jaw weight + resistance to closure)
- Duty cycle: Speech animation (frequent motion) or expression holds

### Electrical Design
**Coil Specification:**
- Wire gauge: 24-26 AWG (0.5-0.7mm diameter)
- Winding layers: 4-5 layers (more powerful than eyebrow)
- Turns per layer: 120-150 turns
- Total turns: 600-750 turns
- Coil resistance: ~20-35 ohms
- Voltage: 9-12V DC (higher power for jaw actuation)
- Power per pulse: ~1.5-3.0W (moderate power)
- Duty cycle: 15-25% (jaw animates frequently during speech)

**Driver Circuit:**
- IC: 2N2222 NPN transistor or MOSFET (IRF540N for higher current handling)
- Control: Single GPIO pin per jaw (or combined with spring-return logic)
- Logic: Standard digital output (on/off) or PWM for variable jaw opening
- Protection: Diode 1N4007 across coil, current-limiting resistor at transistor base

**Power Budget:**
- Peak (jaw + eyebrows active): ~4-5W
- Average (speech animation): ~0.5-1.0W
- Idle: ~0W (spring holds)

### BOM per Jaw
| Component | Qty | Unit Cost | Notes |
|-----------|-----|-----------|-------|
| 3D-printed housing | 1 | $0.75 | ABS, 25g filament, more rigid |
| Iron rod (5mm × 40mm) | 1 | $0.08 | Larger core for more force |
| Copper wire 24 AWG | 20m | $0.15 | Thicker wire, ~600-750 turns |
| Neodymium magnets 5-8mm | 3 | $0.20 | Positioning + detent |
| Spring (compression, 4mm) | 2 | $0.10 | Stronger return force |
| 2N2222 NPN transistor | 1 | $0.05 | Higher current than ULN2003 segment |
| Diode 1N4007 | 1 | $0.02 | Back-EMF protection |
| Connectors (JST) | 1 | $0.10 | Control + power |
| Control linkage (laser-cut or 3D-printed) | 1 | $0.30 | Transfer solenoid motion to jaw |
| **Total per jaw** | | **$1.75** | Two jaws = $3.50 |

---

## 3. CHEEK ACTUATOR
### Purpose
Puff/depress cheeks for expression (happy puff, sad hollow), speech enhancement, breathing effect

### Mechanical Design
**Housing Dimensions:**
- Width: 35mm (localized cheek area)
- Height: 25mm (vertical profile for cheek curvature)
- Depth: 20mm (recessed into head)
- Material: PETG (some flexibility for skin compression)

**Operating Principle:**
- **Pneumatic solenoid** control OR **direct mechanical push**
- Option A: Solenoid pushes lever arm against cheek foam (simple)
- Option B: Rotary cam driven by electromagnet (more complex, more expressive)
- **Recommended: Option A** (lowest cost, adequate for puff effect)

**Motion Spec:**
- Stroke: 8-15mm (cheek compression/expansion)
- Force: ~50-150g (soft tissue, no resistance beyond skin elasticity)
- Speed: 200-500ms (medium speed, supports breathing and expression)
- Holding: Passive (cheek skin provides resistance, no active hold needed)
- Duty cycle: 10-20% (breathing effect or expression punctuation)

### Electrical Design
**Coil Specification:**
- Wire gauge: 28 AWG (0.3-0.4mm diameter, fine for low-force application)
- Winding layers: 2-3 layers
- Turns per layer: 80-120 turns
- Total turns: 250-350 turns
- Coil resistance: ~12-18 ohms
- Voltage: 5V DC (low power, minimal heat)
- Power per pulse: ~0.2-0.5W (light touch)
- Duty cycle: 10-20%

**Driver Circuit:**
- IC: ULN2003 driver (single channel per cheek)
- Control: GPIO pin with PWM for variable puff intensity
- Logic: Pulse-train for breathing effect (slow oscillation 0.5-1Hz)
- Protection: Diode 1N4007

**Power Budget:**
- Peak (both cheeks + eyebrows + jaw): ~5-6W
- Average (full animation): ~0.8-1.5W
- Idle: ~0W

### BOM per Cheek
| Component | Qty | Unit Cost | Notes |
|-----------|-----|-----------|-------|
| 3D-printed housing | 1 | $0.50 | PETG, 12g filament |
| Iron rod (3mm × 25mm) | 1 | $0.04 | Smaller core, light force |
| Copper wire 28 AWG | 10m | $0.08 | Fine wire, ~250-350 turns |
| Neodymium magnets 3-5mm | 1 | $0.05 | Light positioning |
| Spring (compression, 2mm) | 1 | $0.03 | Light return |
| ULN2003 driver segment | 0.33 | $0.15 | Shared IC |
| Diode 1N4007 | 1 | $0.02 | Back-EMF |
| Connectors (JST) | 1 | $0.10 | Control + power |
| **Total per cheek** | | **$0.97** | Two cheeks = $1.94 |

---

## 4. EAR ACTUATOR
### Purpose
Swivel ears for attention (perk up), expression (droop), animation (twitch)

### Mechanical Design
**Housing Dimensions:**
- Width: 30mm (ear hinge assembly width)
- Height: 40mm (vertical mount on ear base)
- Depth: 15mm (compact, sits at ear pivot)
- Material: ABS (rigid, bears ear weight)

**Operating Principle:**
- **Rotary solenoid** with permanent magnet latch
- Electromagnet pulls against spring-loaded detent
- Enables ~45-90° ear rotation around pivot axis
- Option A: Linear solenoid with cam linkage converts to rotation
- Option B: Direct rotary electromagnet (requires more complex winding)
- **Recommended: Option A** (proven, lower cost)

**Motion Spec:**
- Rotation: 45-90° (ear swivel from neutral to perked/drooped)
- Torque: ~200-400g·cm (supporting ear weight + positioning against air resistance)
- Speed: 250-500ms per full motion (responsive but not jittery)
- Holding: Spring-loaded detent (passive hold at extremes)
- Duty cycle: 5-10% (ears animate less frequently than jaw/eyebrows)

### Electrical Design
**Coil Specification:**
- Wire gauge: 26 AWG (0.4-0.5mm)
- Winding layers: 3-4 layers
- Turns per layer: 100-150 turns
- Total turns: 400-600 turns
- Coil resistance: ~15-25 ohms
- Voltage: 9V DC (moderate power for torque)
- Power per pulse: ~0.5-1.5W
- Duty cycle: 5-10%

**Driver Circuit:**
- IC: 2N2222 NPN transistor or ULN2003 segment
- Control: GPIO pin per ear (left/right = 2 pins)
- Logic: Digital pulse (on/off) for discrete positions, or PWM for smooth rotation
- Protection: Diode 1N4007, base resistor (10k-100k)

**Power Budget:**
- Peak (all actuators active): ~6-8W
- Average (full animation): ~1.0-2.0W
- Idle: ~0W

### BOM per Ear
| Component | Qty | Unit Cost | Notes |
|-----------|-----|-----------|-------|
| 3D-printed housing | 1 | $0.60 | ABS, 18g filament |
| Iron rod (4mm × 35mm) | 1 | $0.06 | Moderate core for torque |
| Copper wire 26 AWG | 15m | $0.10 | ~400-600 turns |
| Neodymium magnets 5mm | 2 | $0.10 | Magnetic detents at positions |
| Spring (compression, 3mm) | 1 | $0.05 | Return to neutral |
| 2N2222 transistor | 0.5 | $0.03 | Shared across ears |
| Diode 1N4007 | 1 | $0.02 | Back-EMF |
| Connectors (JST) | 1 | $0.10 | Control + power |
| Cam linkage (3D-printed) | 1 | $0.40 | Linear-to-rotary converter |
| **Total per ear** | | **$1.46** | Two ears = $2.92 |

---

## 5. SYSTEM-LEVEL INTEGRATION

### Master Control Architecture
```
ESP32-S3 (primary controller)
    ├─ Eyebrow Left (GPIO X) → ULN2003 ch1
    ├─ Eyebrow Right (GPIO Y) → ULN2003 ch2
    ├─ Jaw Left (GPIO Z) → 2N2222 transistor
    ├─ Jaw Right (GPIO W) → 2N2222 transistor
    ├─ Cheek Left (GPIO V) → ULN2003 ch3
    ├─ Cheek Right (GPIO U) → ULN2003 ch4
    ├─ Ear Left (GPIO T) → 2N2222 transistor
    └─ Ear Right (GPIO S) → 2N2222 transistor
```

### Power Distribution
**Supply:** 12V @ 1-2A (handles all actuators simultaneous peak)
- Option A: USB-C Power Delivery (60W rated, supports 12V 5A)
- Option B: Li-Po battery pack (3S 11.1V + boost to 12V)

**Per-Channel Protection:**
- Diodes on all coils (1N4007)
- 100µF bulk capacitor at power entry
- 10µF bypass capacitor per driver IC
- Current-limiting resistor at transistor base (10k-47k)

### GPIO/Pin Usage
Total pins needed: **8 GPIO** (digital output for 8 actuators)
- Eyebrows: 2 pins
- Jaws: 2 pins
- Cheeks: 2 pins
- Ears: 2 pins

**Alternative (I2C-based GPIO expansion):**
If GPIO scarce, use PCA9685 (16-channel I2C PWM expander) for all 8 channels + future expansion.

### Animation Timing
**Component Definition (pseudocode):**
```json
{
  "actuators": [
    {
      "name": "eyebrow_left",
      "type": "solenoid_linear",
      "gpio_pin": 14,
      "coil_resistance_ohms": 20,
      "voltage_v": 5,
      "max_power_w": 1.0,
      "stroke_mm": 18,
      "duty_cycle_percent": 10,
      "driver_ic": "ULN2003",
      "animation_default": "neutral"
    },
    {
      "name": "jaw",
      "type": "solenoid_linear_rotary",
      "gpio_pin": 15,
      "coil_resistance_ohms": 28,
      "voltage_v": 12,
      "max_power_w": 3.0,
      "stroke_mm": 25,
      "duty_cycle_percent": 20,
      "driver_ic": "2N2222",
      "animation_default": "closed"
    }
  ]
}
```

---

## 6. FULL SYSTEM BOM

| Feature | Left | Right | Unit Cost | Subtotal |
|---------|------|-------|-----------|----------|
| **Eyebrows** | 1 | 1 | $1.05 each | $2.10 |
| **Jaws** | 1 | 1 | $1.75 each | $3.50 |
| **Cheeks** | 1 | 1 | $0.97 each | $1.94 |
| **Ears** | 1 | 1 | $1.46 each | $2.92 |
| **Shared Driver ICs** (ULN2003×2, 2N2222×2) | — | — | — | $1.00 |
| **Diodes, caps, resistors** (miscellaneous) | — | — | — | $0.50 |
| **Power supply (12V, 2A) + connectors** | — | — | — | $3.00 |
| **3D-printed mounting brackets** (integration) | — | — | — | $1.50 |
| **Total Face Animation System** | | | | **$16.46** |
| **Per-actuator average** | | | | **$1.03** |

---

## 7. MANUFACTURING TIMELINE

### Phase 1: Design & Prototyping (Week 1-2)
- [ ] CAD models for each housing (OpenSCAD or Fusion 360)
- [ ] Coil winding jig design (3D-printed or manual)
- [ ] Test coil winding on paper core (validate turns, resistance)
- [ ] Prototype single eyebrow actuator
- [ ] Validate spring return, magnetic detent positioning

### Phase 2: Component Sourcing (Week 1-3, parallel)
- [ ] Order 100x neodymium magnets (bulk from AliExpress, ~$5-10 for 100 pcs)
- [ ] Order iron rod stock (AliExpress, ~$2-5 for assorted lengths)
- [ ] Order copper wire spools 24-28 AWG (25m+ for $5-15/spool)
- [ ] Order springs assortment ($5-10 for mixed pack)
- [ ] Order ULN2003 and 2N2222 transistor bulk ($0.50-1.00 each)
- [ ] Order 3D printer filament (PETG/ABS, 500g = $8-15)

### Phase 3: Assembly & Testing (Week 3-4)
- [ ] Print all housings (eyebrow×2, jaw×2, cheek×2, ear×2)
- [ ] Wind coils for each actuator (estimated 30 minutes per coil)
- [ ] Assemble iron cores, springs, magnets into housings
- [ ] Mount drivers on PCB or breadboard
- [ ] Wire to ESP32 test rig
- [ ] Validate individual actuator response (GPIO pulse → motion)
- [ ] Measure actual power consumption per feature

### Phase 4: Integration (Week 4-5)
- [ ] Mount all 8 actuators into face structure
- [ ] Route control wiring (color-coded, bundled)
- [ ] Load animation sequences (JSON configuration)
- [ ] Calibrate timing and force (adjust PWM values)
- [ ] Test full facial expression animation

---

## 8. RISK MITIGATION

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|-----------|
| Coil windings too loose → insufficient magnetic force | Medium | High | Build winding jig, tension wire during coil wrapping |
| Iron rod diameter mismatch → binding or excessive play | Medium | Medium | Ream housing bore to specific tolerance (±0.1mm) |
| Spring rate too weak → incomplete motion | Medium | Medium | Test with assorted springs; purchase spring variety pack |
| Neodymium magnet orientation wrong → repulsion instead of attraction | Low | High | Mark polarity with marker; test before installation |
| Excessive current draw → ESP32 GPIO damaged | Low | High | Always use transistor driver IC; never direct-drive coil from GPIO |
| Copper wire insulation damaged during winding → short circuit | Low | Medium | Use enamel-coated wire (34 AWG rated); inspect before power-on |
| Housing warping during printing → misalignment | Low | Medium | Print at low temperature (185-195°C for PETG), slow speed |

---

## 9. NEXT STEPS

1. **Validate Design:** Review coil winding specifications with hobbyist electromagnet communities (e.g., Reddit r/electronics, forums)
2. **Prototype Single Unit:** Build one eyebrow actuator end-to-end as proof-of-concept
3. **Create OpenSCAD Models:** Parametric housings for each feature (allow adjustment of core diameter, coil layers, etc.)
4. **Configure JSON Schema:** Define actuator component structure in `config/components/`
5. **Implement Animation Driver:** Write C++ component code for timing, PWM control, and state machine for each feature

---

**Document Status:** STANDALONE DESIGN REFERENCE (not part of project build)  
**Encoding:** ASCII-only (no special characters)  
**Last Updated:** 2025-11-06
