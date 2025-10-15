# P32 Goblin Head - Build Summary
**Essential Information for Manufacturing**

---

## What You're Building
- **Goblin animatronic head** with 3 displays and sensor
- **Total weight:** 249.75 grams
- **Print time:** 13-21 hours
- **Material:** ~108g PLA filament
- **Total cost:** $150-220

---

## Files Ready to Print

```
assets/shapes/stl/
├── goblin_skull_complete.stl     (371 KB, 8-12 hours, 57.5g)
├── example_display_mount.stl     (78 KB, 1-2 hours, 8g) × 3 copies
├── goblin_nose_sensor.stl        (63 KB, 2-3 hours, 15g)
└── test_cube.stl                 (0.7 KB, 15 min, calibration)
```

**Print Order:**
1. Start with test_cube.stl (15 minutes - verify printer accuracy)
2. Print goblin_skull_complete.stl (main component - 8-12 hours)
3. Print 3 copies of example_display_mount.stl (3-6 hours total)
4. Print goblin_nose_sensor.stl (2-3 hours)

---

## Print Settings
- **Layer height:** 0.2mm
- **Infill:** 20% for skull, 100% for mounts
- **Temperature:** 210°C nozzle, 60°C bed
- **Supports:** Yes for skull (auto-generate)
- **Material:** PLA plastic, goblin green preferred

---

## Electronics Needed

### Main Components
- **ESP32-S3-DevKitC-1** (1x) - $15-20
- **GC9A01 circular displays** (3x) - $24-36 total
- **HC-SR04 ultrasonic sensor** (1x) - $3-5

### Hardware
- **M3 threaded inserts** (12x) - $5-10
- **M3 screws, nuts, washers** - $10-15
- **Wire and connectors** - $10-15

### Motors (Optional)
- **NEMA 17 High Torque** (1x) - $25-35
- **NEMA 17 Standard** (1x) - $15-25
- **TMC2209 drivers** (2x) - $14-20

---

## Wiring Connections

### ESP32-S3 GPIO Pins Used:
```
SPI Bus (All Displays):
├── GPIO 12: MISO
├── GPIO 13: MOSI  
└── GPIO 14: CLK

Individual Display CS:
├── GPIO 15: Left Eye
├── GPIO 16: Right Eye
└── GPIO 17: Mouth

Sensor:
├── GPIO 9: Trigger
└── GPIO 10: Echo

Audio (Future):
├── GPIO 4: BCLK
├── GPIO 5: WS
└── GPIO 6: DATA
```

**Total pins used:** 12 (26+ remaining for expansion)

---

## Assembly Steps

### 1. Print Components
- Print and verify all STL files
- Clean supports, sand surfaces smooth
- Test fit all components before assembly

### 2. Install Hardware  
- Heat threaded inserts to 200°C and press into skull
- Mount displays in rings with secure fit
- Position sensor in nose mount

### 3. Wiring
- Mount ESP32-S3 in rear skull compartment
- Connect SPI bus to all displays (shared pins)
- Connect individual CS pins to each display
- Connect sensor trigger and echo pins
- Verify all connections with multimeter

### 4. Testing
- Apply 5V power and check all displays
- Test sensor distance readings
- Verify SPI communication working
- Check total weight matches 249.75g ±5%

---

## Motor Requirements (If Adding Movement)

**Calculated loads from 249.75g head weight:**
- **Required torque:** 0.795 N⋅m (112.5 oz⋅in)
- **Recommended:** NEMA 17 with ≥120 oz⋅in holding torque
- **Performance target:** 90° turn in 0.5 seconds
- **Power:** 24W per motor maximum

---

## Quick Troubleshooting

### Displays Not Working
- Check 3.3V power supply
- Verify SPI bus connections (GPIO 12,13,14)
- Test CS pin assignments (GPIO 15,16,17)

### Sensor Not Reading
- Check 5V power to HC-SR04
- Verify trigger/echo connections (GPIO 9,10)
- Ensure clear line of sight

### Print Quality Issues
- Calibrate with test cube first
- Check bed leveling and temperature
- Use supports for skull overhangs

---

## File Locations

**Build documentation:**
- `docs/VISUAL-GOBLIN-HEAD-BUILD-GUIDE.md` - Complete guide
- `docs/goblin-full-interconnection-diagram.md` - Wiring details
- `docs/goblin-head-weight-and-motor-analysis.md` - Motor specs

**STL files:**
- `assets/shapes/stl/` - All printable components

**Configuration:**
- `config/bots/goblin_full.json` - Complete bot definition
- `config/components/` - Individual component specs

---

**Ready to start printing!**
**Begin with test_cube.stl to verify printer accuracy**