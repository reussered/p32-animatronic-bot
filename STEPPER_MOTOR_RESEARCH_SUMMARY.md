[STANDALONE DOCUMENTATION - NOT PART OF PROJECT BUILD - HUMAN REFERENCE ONLY]

# Stepper Motor Research - Complete Summary

Research compiled on November 6, 2025 for goblin animatronic expression system.

---

## Documents Created

This research package includes 3 comprehensive documents:

### 1. STEPPER_MOTOR_RESEARCH.md (Complete Technical Analysis)
- Detailed specs for 30+ motor models
- Power budget calculations
- Cost scaling for 50-1000 motors
- Thermal considerations
- Sourcing and availability
- 400+ lines of comprehensive data

### 2. STEPPER_MOTOR_QUICK_REFERENCE.md (Decision Guide)
- TL;DR recommendations
- Cost tiers comparison table
- Power budget summaries
- Motor specifications quick lookup
- 4 scaling scenarios
- Perfect for quick decisions

### 3. STEPPER_MOTOR_CONTROL_ARCHITECTURE.md (Implementation Guide)
- Control signal architecture
- GPIO multiplexing solutions (PCA9685, shift registers)
- Power delivery strategies
- Noise management
- Firmware requirements
- Scalability path (prototype -> full system)
- Testing checklist

---

## Key Findings

### Best Value for Expression Animation

**Pololu NEMA 8 (SY20STH30-0604A)**
- Cost: USD $46.32 per motor
- For 100 motors: USD $4,632 total
- Power: 3.9V, 0.6A/phase = 2.34W per motor
- Torque: 180 g-cm (sufficient for eyebrows, eyelids, mouth)
- Size: 20x30mm (compact for dense placement)
- Steps/rev: 200 (smooth 1.8 degree increments)

### Cost Spectrum

| Budget Option | Tier 1: 28BYJ-48 | Recommended: NEMA 8 | Premium: NEMA 17 Pancake |
|---------------|-----------------|-------------------|----------------------|
| Cost/100 motors | $400 | $4,632 | $7,495 |
| Cost/motor | $4 | $46 | $75 |
| Torque | 150 g-cm | 180 g-cm | 1-2.2 kg-cm |
| Size | 28x20mm | 20x30mm | 50x11-16mm |
| Power/motor | 0.5W | 2.34W | 3.5W |
| Best for | Ultra-budget | General expression | Micro-detail, flat mounting |

### Power Requirements

For 100 x NEMA 8 steppers:
- Total max power: 234W (if all running)
- Realistic active: 70W (30 motors moving)
- Recommended supply: Single 24V/10A (240W) OR multiple 5V/5A modular

### Control Strategy

Use I2C GPIO expanders (PCA9685):
- Solve ESP32 GPIO limitation (only 16 PWM timers)
- 4x PCA9685 boards = 64 independent channels
- Control 50-100 motors via single I2C bus
- Total control electronics cost: USD $270-350 for 100 motors

---

## Recommendation for Goblin Head

### Phase 1 (Prototype): 20 motors, USD $1,200 total
- 20x NEMA 8 motors: $926
- Control electronics: $200
- Testing and validation: $100
- Use for: Eyes, eyebrows, mouth basic movements

### Phase 2 (Full Expression): 100 motors, USD $5,200 total
- 100x NEMA 8 motors: $4,632
- Control electronics: $350
- Mechanical mounting: $200
- Full facial expression capability

### Phase 3 (Premium Enhancement): 100-150 motors, USD $6,500-8,500
- 50x NEMA 8 (coarse movements)
- 50-100x Pancake NEMA 17 (micro-detail, ultra-flat)
- Advanced control with position feedback
- Theatrical-quality expression

---

## Next Actions

1. **Immediate**: Order 5-10 test motors (NEMA 8) for physical evaluation
   - Verify torque adequate for eyelid control
   - Test noise levels
   - Confirm microstepping smoothness

2. **Short term** (Week 1-2): 
   - Build prototype control circuit (PCA9685 + A4988 + 1 motor)
   - Develop firmware skeleton for motor control
   - Test I2C bus reliability with drivers

3. **Medium term** (Week 3-4):
   - Design mechanical mounting brackets (3D print)
   - Scale to 20-motor prototype
   - Validate animation sequencing

4. **Long term** (Week 5-8):
   - Finalize motor count and distribution on face
   - Order full 100-motor batch
   - Assemble complete expression system

---

## Data Quality Notes

All research sourced from:
- Pololu Robotics official specs
- Adafruit official product pages
- Manufacturer datasheets (Sanyo, SOYO)
- In-stock supplier verification

Prices current as of November 6, 2025 (subject to availability/exchange rates).

Power calculations conservative (worst-case assumptions).

Torque values from holding torque specs (actual performance may vary with load).

---

## File Compliance

All documents created as:
- ASCII-only (0x00-0x7F)
- Standalone research documentation
- NOT part of project build
- NOT imported by any code
- Explicitly labeled as non-binding human reference

These files are research and analysis only. Implementation will follow architectural guidelines in `.github/AI-AGENT-RULES.md`.
