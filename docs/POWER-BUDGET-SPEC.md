# Power Budget Specification - P32 Animatronic Bot

## Critical Design Constraint

**Every component MUST have documented power requirements** - without accurate power budgeting, the system will experience brownouts, resets, or component damage.

## Power Budget Philosophy

1. **Measure twice, budget once** - Always use worst-case maximums, not typical values
2. **Voltage rail isolation** - Different subsystems may need different voltages
3. **Peak vs. continuous** - Servos draw massive peak current during motion
4. **Battery capacity planning** - Runtime = (Battery mAh) / (Average current draw in mA)
5. **Thermal management** - Power dissipated as heat must be managed

## Current Power Inventory (Goblin Full Bot)

### ESP32-S3 Subsystems

#### Head Subsystem (ESP32-S3 DevKitC-1)

| Component | Voltage | Current (mA) | Power (W) | Duty Cycle | Avg Current (mA) |
|-----------|---------|--------------|-----------|------------|------------------|
| **ESP32-S3 Module** | 3.3V | 500 | 1.65 | 100% | 500 |
| **Left Eye (GC9A01)** | 3.3V | 50 | 0.165 | 100% | 50 |
| **Right Eye (GC9A01)** | 3.3V | 50 | 0.165 | 100% | 50 |
| **Mouth Display (GC9A01)** | 3.3V | 50 | 0.165 | 100% | 50 |
| **Nose Sensor (HC-SR04)** | 5V | 15 | 0.075 | 10% | 1.5 |
| **Speaker (I2S 3W)** | 3.3V | 500 | 1.65 | 20% | 100 |
| **Neck Pan Servo (SG90)** | 5V | 500 | 2.5 | 10% | 50 |
| **Neck Tilt Servo (SG90)** | 5V | 500 | 2.5 | 10% | 50 |
| **Ear LEDs (WS2812 x2)** | 5V | 60 | 0.3 | 50% | 30 |
| **TOTAL HEAD** | - | - | **9.22W** | - | **881.5 mA** |

**Head Power Rails**:
- **3.3V Rail**: 1,200 mA (ESP32 + 3x displays + speaker)
- **5V Rail**: 131.5 mA (sensor + servos + LEDs)

#### Torso Subsystem (ESP32-S3 DevKitC-1)

| Component | Voltage | Current (mA) | Power (W) | Duty Cycle | Avg Current (mA) |
|-----------|---------|--------------|-----------|------------|------------------|
| **ESP32-S3 Module** | 3.3V | 500 | 1.65 | 100% | 500 |
| **WiFi Active** | 3.3V | 250 | 0.825 | 30% | 75 |
| **ESP-NOW Mesh** | 3.3V | 100 | 0.33 | 100% | 100 |
| **Bluetooth LE** | 3.3V | 50 | 0.165 | 50% | 25 |
| **Spine Flexion Servo** | 5V | 500 | 2.5 | 20% | 100 |
| **Spine Extension Servo** | 5V | 500 | 2.5 | 20% | 100 |
| **Waist Rotation Servo** | 5V | 500 | 2.5 | 20% | 100 |
| **Torso Status LED** | 3.3V | 20 | 0.066 | 50% | 10 |
| **Torso Speaker** | 3.3V | 500 | 1.65 | 20% | 100 |
| **TOTAL TORSO** | - | - | **12.18W** | - | **1,110 mA** |

**Torso Power Rails**:
- **3.3V Rail**: 810 mA (ESP32 + WiFi + Mesh + BLE + LED + speaker)
- **5V Rail**: 300 mA (3x servos)

### System-Wide Power Summary

| Subsystem | 3.3V (mA) | 5V (mA) | Peak Power (W) | Avg Power (W) |
|-----------|-----------|---------|----------------|---------------|
| **Head** | 1,200 | 131.5 | 9.22 | 6.08 |
| **Torso** | 810 | 300 | 12.18 | 7.33 |
| **TOTAL** | **2,010** | **431.5** | **21.4W** | **13.41W** |

## Power Supply Architecture

### Option 1: Single 5V Buck Converter + 3.3V LDOs

```
┌─────────────────────────────────────────────────────────────┐
│  7.4V 2S LiPo Battery (3000 mAh)                           │
│  Voltage Range: 8.4V (full) → 6.0V (empty)                 │
└─────────────────────────────────────────────────────────────┘
                        │
                        ▼
        ┌───────────────────────────────┐
        │  5V Buck Converter (5A rated) │
        │  Efficiency: ~85%              │
        └───────────────────────────────┘
                        │
            ┌───────────┴───────────┐
            ▼                       ▼
    ┌──────────────┐        ┌──────────────┐
    │  5V Devices  │        │  3.3V LDO    │
    │  (Servos,    │        │  (3A rated)  │
    │   Sensors,   │        │  Efficiency: │
    │   LEDs)      │        │  ~75%        │
    │  432 mA      │        └──────────────┘
    └──────────────┘                │
                                    ▼
                            ┌──────────────┐
                            │ 3.3V Devices │
                            │ (ESP32s,     │
                            │  Displays,   │
                            │  Speakers)   │
                            │ 2,010 mA     │
                            └──────────────┘
```

**Runtime Calculation**:
- Battery: 7.4V × 3000 mAh = 22.2 Wh
- Average Power: 13.41W
- **Runtime: 22.2 Wh / 13.41W = 1.66 hours (100 minutes)**

### Option 2: Dual Buck Converters (More Efficient)

```
┌─────────────────────────────────────────────────────────────┐
│  7.4V 2S LiPo Battery (3000 mAh)                           │
└─────────────────────────────────────────────────────────────┘
                        │
            ┌───────────┴───────────┐
            ▼                       ▼
    ┌──────────────┐        ┌──────────────┐
    │  5V Buck     │        │  3.3V Buck   │
    │  (1A rated)  │        │  (3A rated)  │
    │  Eff: 90%    │        │  Eff: 90%    │
    └──────────────┘        └──────────────┘
            │                       │
            ▼                       ▼
    ┌──────────────┐        ┌──────────────┐
    │  5V Devices  │        │ 3.3V Devices │
    │  432 mA      │        │ 2,010 mA     │
    └──────────────┘        └──────────────┘
```

**Improved Runtime**:
- 5V rail: (5V × 0.432A) / 0.90 = 2.4W from battery
- 3.3V rail: (3.3V × 2.01A) / 0.90 = 7.37W from battery
- Total: 9.77W from battery (vs 13.41W with LDO losses)
- **Runtime: 22.2 Wh / 9.77W = 2.27 hours (136 minutes)**

## Critical Power Gotchas

### 1. Servo Peak Current Surges

**DANGER**: SG90 servos can draw **800mA peak** during stall or rapid motion.

- **6 servos moving simultaneously** = 4.8A peak current!
- **Solution**: Stagger servo movements in code (never move all at once)
- **Hardware**: Large bulk capacitors (1000-2200µF) on 5V rail

### 2. Display Backlight Current

**GC9A01 displays**:
- Specified: 50mA typical
- **Reality**: Up to 80mA at full brightness
- **Solution**: PWM dim backlights to 70% (saves power, extends battery)

### 3. WiFi Peak Current

**ESP32-S3 WiFi transmit**:
- Specified: 250mA average
- **Peak**: Can spike to 500mA during TX burst
- **Solution**: Ensure 3.3V rail can supply 1A peaks (ESP32 + WiFi spike)

### 4. Speaker Distortion Under Load

**I2S 3W speakers**:
- Specified: 500mA at 3.3V
- **Reality**: Peaks to 900mA on bass transients
- **Solution**: Dedicated 3.3V rail for audio (isolate from ESP32)

## Power Monitoring Strategy

### Hardware Power Monitor (Already Implemented!)

Your `power_monitor.c` component reads ADC for battery voltage:

```c
// src/components/system/power_monitor.c
// Monitors battery voltage and calculates percentage
// hitCount: 200 (every 20 seconds)
```

### Recommended Additions

1. **INA219 Current Sensor** (I2C):
   - Measures actual current on each power rail
   - Detects brownout conditions before they crash system
   - Cost: $2-3 per sensor

2. **Battery Voltage Divider** (Already mentioned in code):
   - ADC Channel 0 (GPIO 36)
   - Divider ratio: 2:1 or 3:1 depending on battery voltage
   - Update battery percentage in `g_shared_state.battery_percent`

3. **Per-Rail Monitoring**:
   - 3.3V rail: Measure with ADC (no divider needed)
   - 5V rail: Measure with ADC + small divider (5V → 3.3V max)

## Battery Selection Guide

### Current Goblin Full Bot (Head + Torso Only)

| Battery | Voltage | Capacity | Runtime | Weight | Cost |
|---------|---------|----------|---------|--------|------|
| **2S 3000mAh LiPo** | 7.4V | 3000mAh | 1.5 hrs | 150g | $25 |
| **2S 5000mAh LiPo** | 7.4V | 5000mAh | 2.5 hrs | 250g | $35 |
| **3S 2200mAh LiPo** | 11.1V | 2200mAh | 1.8 hrs | 180g | $22 |

### Future Full Robot (4x Arms + 2x Legs Added)

| Battery | Voltage | Capacity | Runtime | Weight | Cost |
|---------|---------|----------|---------|--------|------|
| **3S 5000mAh LiPo** | 11.1V | 5000mAh | 1.5 hrs | 400g | $45 |
| **4S 5000mAh LiPo** | 14.8V | 5000mAh | 1.8 hrs | 500g | $55 |
| **3S 10000mAh LiPo** | 11.1V | 10000mAh | 3.0 hrs | 800g | $80 |

**Recommendation**: Start with **2S 5000mAh** for testing (2.5 hour runtime, affordable).

## Power Failure Modes & Protection

### Brown-Out Detection (BOD)

ESP32-S3 has built-in BOD:
```c
// In system_core_init()
esp_brownout_init();
esp_brownout_set_threshold(ESP_BROWNOUT_DET_LVL_SEL_2);  // 2.43V threshold
```

### Low Battery Warning

```c
// In power_monitor_act()
if (battery_percent < 20) {
    ESP_LOGW(TAG, "Low battery: %d%%", battery_percent);
    // Reduce display brightness
    // Disable non-critical servos
    // Flash warning LED
}

if (battery_percent < 10) {
    ESP_LOGE(TAG, "Critical battery: %d%%", battery_percent);
    // Trigger graceful shutdown
    // Save state to NVS
    // Turn off all peripherals except WiFi (send alert)
}
```

### Servo Current Limiting

```c
// In servo control components
#define MAX_SIMULTANEOUS_SERVOS 2

static uint8_t servos_moving = 0;

void servo_act(void) {
    if (servos_moving >= MAX_SIMULTANEOUS_SERVOS) {
        return;  // Defer movement until others finish
    }
    
    servos_moving++;
    move_servo();
    servos_moving--;
}
```

## Power Budget Validation Checklist

Before adding any new component:

- [ ] **Voltage rail identified** - Which rail (3.3V, 5V, 12V)?
- [ ] **Current measured** - Actual measurement with multimeter, not just datasheet
- [ ] **Peak current documented** - Worst-case scenario (startup, stall, full load)
- [ ] **Duty cycle estimated** - What % of time is it active?
- [ ] **Average current calculated** - Peak × Duty Cycle
- [ ] **Power supply headroom verified** - Rail can supply peak + 20% margin
- [ ] **Thermal dissipation considered** - Does it need heatsink?
- [ ] **Battery runtime recalculated** - Update total system power budget
- [ ] **Hardware JSON updated** - Add electrical_specs to component file
- [ ] **Power monitor code updated** - Add to monitoring if critical

## Hardware JSON Power Spec Template

Every hardware component MUST include this section:

```json
{
  "electrical_specs": {
    "supply_voltage": "5V",
    "logic_voltage": "3.3V",
    "current_consumption_typical": "50mA",
    "current_consumption_max": "100mA",
    "peak_inrush_current": "200mA",
    "power_consumption_typical": "0.25W",
    "power_consumption_max": "0.5W",
    "startup_delay": "100ms",
    "power_sequencing": "MUST_POWER_BEFORE_LOGIC"
  },
  "thermal_specs": {
    "operating_temp_range": "-20C to +85C",
    "max_case_temp": "85C",
    "thermal_resistance": "50C/W",
    "requires_heatsink": false
  }
}
```

## Power Supply Component Files Needed

Create these hardware definitions:

1. **config/components/hardware/buck_5v_5a.json** - 5V buck converter spec
2. **config/components/hardware/buck_3v3_3a.json** - 3.3V buck converter spec
3. **config/components/hardware/lipo_2s_5000mah.json** - Battery spec
4. **config/components/hardware/ina219_current_sensor.json** - Current monitor
5. **config/components/hardware/bulk_capacitor_2200uf.json** - Power smoothing

## Future: Multi-Subsystem Power Distribution

When adding arms/legs (each with ESP32-C3):

```
Main Battery (3S 11.1V 5000mAh)
    │
    ├─► Head Subsystem Buck (3.3V 2A) → Head ESP32-S3 + Displays
    │
    ├─► Torso Subsystem Buck (3.3V 2A) → Torso ESP32-S3 + System
    │
    ├─► Arm Left Buck (3.3V 1A) → Arm ESP32-C3 + 7 Servos
    │
    ├─► Arm Right Buck (3.3V 1A) → Arm ESP32-C3 + 7 Servos
    │
    ├─► Leg Left Buck (5V 3A) → Leg ESP32-C3 + NEMA 17 Motors
    │
    └─► Leg Right Buck (5V 3A) → Leg ESP32-C3 + NEMA 17 Motors
```

**Total Power**: ~40-50W (need 10,000+ mAh battery for 2 hour runtime)

## Summary

✅ **Current inventory complete** - Head + Torso power documented  
✅ **Power rails defined** - 3.3V (2A) and 5V (0.5A)  
✅ **Battery sizing calculated** - 2S 5000mAh LiPo = 2.5 hours  
✅ **Critical gotchas identified** - Servo surges, WiFi peaks, speaker transients  
✅ **Monitoring strategy** - ADC voltage sensing + future INA219 current  
⏳ **Hardware JSON audit needed** - Verify all components have complete electrical_specs  
⏳ **Power supply components** - Create JSON definitions for buck converters, batteries  
⏳ **Protection code** - Implement low battery warnings and graceful shutdown  

📘 **Next Steps**: Audit all hardware JSON files and add missing power specs
