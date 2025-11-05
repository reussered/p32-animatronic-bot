# Solar Parasol System - Recovery & Implementation Guide

## Document Status
- **Date**: 2025-10-13
- **Status**: RECOVERED AND DOCUMENTED
- **Hardware ID**: SOLAR_PARASOL_2M_DEPLOYABLE
- **Version**: 1.0.0

## Executive Summary

The **Solar Parasol System** is a complete 2-meter deployable solar power generation platform integrated into the P32 Animatronic Bot ecosystem. The system specifications exist in JSON but the implementation is a stub awaiting development.

## Physical Specifications

| Specification | Value | Unit |
|---|---|---|
| **Deployed Diameter** | 2000 | MM |
| **Retracted Length** | 600 | MM |
| **Deployment Height** | 2200 | MM |
| **Total Weight** | 6500 | G |
| **Solar Panel Area** | 2.54 | SQM |
| **Peak Power Output** | 559 | W |
| **Wind Resistance** | 25 | MPH |
| **Deployment Time** | 45 | SEC |

### Deployment Modes

#### Retracted State
```
Dimensions: 600 × 150 × 150 MM
Power Generation: 0 W
Use Case: Storage/transport
```

#### Partial Deployment (Weather Protection)
```
Dimensions: 1000 × 1000 × 1500 MM
Power Generation: 200 W
Use Case: Shade provision with some power
```

#### Full Deployment (Maximum Power)
```
Dimensions: 2000 × 2000 × 2200 MM
Power Generation: 559 W
Use Case: Autonomous power generation
```

## Electrical Specifications

### Power Output
| Parameter | Value |
|---|---|
| Solar Panel Voltage | 12V |
| Peak Current Output | 46.6 A |
| MPPT Controller | 30A 12V |
| Peak Power | 559 W |

### Control Systems
| Component | Count | Power |
|---|---|---|
| Tracking Servos | 2 | — |
| Deployment Servos | 8 | — |
| Total Servo Power | — | 16 W |
| Control Voltage | — | 5V |

### Integration
- **Primary Interface**: I2C (Secondary Bus)
- **Servo Controller**: PCA9685 (16-channel PWM)
- **Protocol**: ESP32 to servo driver via I2C

## Mechanical Subsystems

### 1. Central Hub Assembly
```
Name: PARASOL_HUB_ASSEMBLY
Material: PLA_PLUS
Mounting: ROBOT_SHOULDER_MOUNT
Contains:
  - MPPT controller
  - Servo drivers (PCA9685)
  - Weather sensors
```

### 2. Deployment Mechanism
```
Name: PARASOL_DEPLOYMENT_SYSTEM
Rib Count: 8
Extension Ratio: 3.33:1
Actuation: Servo-driven telescoping
- 8 servo motors drive rib extension
- Each servo manages one rib pair
```

### 3. Solar Panel Mounting
```
Name: PARASOL_SOLAR_MOUNTS
Panel Segments: 16 (flexible)
Interconnects: Flexible/articulated
Weather Sealing: IP65
- Distributed across deployed surface
- Flexible interconnects allow deployment
```

### 4. Tracking System (Gimbal)
```
Name: PARASOL_TRACKING_GIMBAL
Tilt Range: 0–90°
Rotation Range: 0–360°
Positioning Accuracy: ±1°
- 2 servo motors (tilt + rotation)
- Solar irradiance optimization
```

## Control Integration

### Sensor Inputs
- **Wind Speed** - Emergency retract trigger
- **Solar Irradiance** - Optimal positioning
- **Rain Detector** - Weather protection

### Safety Systems
- **Emergency Retract** - Rapid deployment reversal
- **Wind Lock** - Locking mechanism at high wind speeds
- **Overcurrent Protection** - Battery/controller safety

### CAD Components
Located in `config/components/hardware/`:

```
parasol_central_hub.scad               → Hub assembly
parasol_telescoping_ribs.scad          → 8 extending ribs
parasol_solar_cell_mounts.scad         → 16-segment mounts
parasol_deployment_servos.scad         → Servo mounting
parasol_tracking_gimbal.scad           → 2-axis gimbal
parasol_weather_protection.scad        → IP65 sealing
parasol_cable_management.scad          → Wire routing
parasol_shoulder_interface.scad        → Robot mounting
```

## Current Implementation Status

### File Locations
| File | Status | Path |
|---|---|---|
| **JSON Config** | ✅ COMPLETE | `config/components/hardware/solar_parasol_system.json` |
| **Source Code** | 🔧 STUB ONLY | `config/components/hardware/solar_parasol_system.src` |
| **Header** | ❌ MISSING | `config/components/hardware/solar_parasol_system.hdr` |

### Source Code Status
```cpp
// Current state: Skeleton with TODOs
esp_err_t solar_parasol_system_init(void)
{
    // TODO: Implement initialization
    return ESP_OK;
}

void solar_parasol_system_act(void)
{
    // TODO: Implement component behavior
}
```

## Implementation Roadmap

### Phase 1: Core Integration
- [ ] Create `.hdr` header file with component interface
- [ ] Implement GPIO/I2C initialization for servo controller (PCA9685)
- [ ] Implement servo driver setup (16 channels: 8 deployment + 2 tracking)
- [ ] Add sensor input reading (wind, irradiance, rain)

### Phase 2: Deployment Sequencing
- [ ] Implement servo sequencing for smooth deployment
- [ ] Add deployment state machine (retracted → partial → full)
- [ ] Implement emergency retract logic
- [ ] Add wind speed lockout protection

### Phase 3: Solar Tracking
- [ ] Implement 2-axis gimbal control (azimuth + elevation)
- [ ] Add solar irradiance optimization algorithm
- [ ] Integrate tracking with deployment states
- [ ] Add power monitoring for tracking efficiency

### Phase 4: Safety & Monitoring
- [ ] Implement overcurrent protection logic
- [ ] Add weather-based auto-retract
- [ ] Create diagnostic telemetry reporting
- [ ] Add graceful degradation modes

### Phase 5: Integration with Bot
- [ ] Register with Global Shared Memory (GSM)
- [ ] Publish power generation metrics
- [ ] Subscribe to system-wide emergency signals
- [ ] Integrate into bot JSON composition

## Expected SharedMemory Interface

### Write Operations (Solar Parasol → Bot)
```cpp
GSM.write<uint16_t>("parasol.voltage", solar_voltage);      // 12V rail
GSM.write<uint16_t>("parasol.current", current_output);     // 46.6A max
GSM.write<uint16_t>("parasol.power", power_watts);          // 0-559W
GSM.write<uint8_t>("parasol.deployment_pct", deployment);   // 0-100%
GSM.write<uint8_t>("parasol.tilt_angle", tilt);             // 0-90°
GSM.write<uint16_t>("parasol.rotation_deg", rotation);      // 0-360°
```

### Read Operations (Bot → Solar Parasol)
```cpp
uint8_t emergency_retract = GSM.read<uint8_t>("system.emergency_flag");
uint8_t wind_speed = GSM.read<uint8_t>("environment.wind_mph");
uint16_t irradiance = GSM.read<uint16_t>("environment.solar_irradiance");
```

## Use Cases in Goblin Bot Context

1. **Autonomous Power Generation** - Solar charging during idle periods
2. **Mobile Charging Station** - Vehicle-mounted power source
3. **Event Shade Provider** - Unfolds for outdoor event coverage
4. **Emergency Power Source** - Backup during grid loss
5. **Home Energy Offset** - Stationary installation for household use

## Compatibility Matrix

| Target | Compatibility | Notes |
|---|---|---|
| **Goblin Creature** | UNIVERSAL | Any humanoid form factor |
| **Robot Platforms** | PRIMARY | Designed for robot integration |
| **Android Systems** | EXCELLENT | Plug-and-play integration |
| **All Humanoids** | UNIVERSAL | Shoulder-mount interface |

## Files to Create/Modify

### Required New Files
```powershell
# Create header file
New-Item -Path "config/components/hardware/solar_parasol_system.hdr" -ItemType File

# Create test harness
New-Item -Path "test/solar_parasol_test.cpp" -ItemType File
```

### Files to Update
```
1. config/components/hardware/solar_parasol_system.src    → Full implementation
2. platformio.ini                                           → Add environment if needed
3. config/bots/bot_families/goblins/goblin_full.json      → Add to composition
```

## Build Commands

### Regenerate Tables After Changes
```powershell
python tools/generate_tables.py goblin_full src
.\generate_file_structure.ps1
```

### Build & Test
```powershell
pio run -e goblin_head > build.log 2>&1
Select-String -Path build.log -Pattern "error|success"
```

### Serial Monitor (Solar Parasol Diagnostics)
```powershell
Start-Job { pio device monitor } | Wait-Job -Timeout 60 | Receive-Job
```

## Hardware Pin Assignments

**Note**: Pin assignments are dynamic and managed in:
```
include/esp32_s3_r8n16_pin_assignments.h
```

- I2C Secondary Bus (PCA9685 servo controller): Check dynamic pin map
- Sensor inputs (wind, rain, irradiance): ADC channels TBD
- Power monitoring: ADC/INA219 per existing power system

## Power Budget Impact

| Mode | Draw | Net Impact |
|---|---|---|
| **Retracted (Idle)** | <1 mA | Minimal |
| **Partial Deployment** | ~150 mA | Marginal |
| **Full Deployment (Servos Moving)** | ~2 A peak | Temporary |
| **Tracking Active** | ~300 mA | Continuous small draw |
| **Power Generation** | −559 W max | **NET POSITIVE** |

## References

- **JSON Source**: `config/components/hardware/solar_parasol_system.json`
- **Implementation Stub**: `config/components/hardware/solar_parasol_system.src`
- **Component Architecture**: See `.github/copilot-instructions.md`
- **Servo Controller**: PCA9685 16-channel PWM driver
- **MPPT Controller**: Victron SmartSolar or equivalent 30A 12V model

## Recovery Notes

This document was recovered from:
1. **JSON specification** - Comprehensive hardware & electrical specs
2. **Implementation source** - Skeleton code template
3. **Architecture rules** - Component system constraints (single compilation unit per subsystem)

**Status**: System is **architecturally sound** and **documented**, awaiting full C++ implementation.

---
**Document Generated**: 2025-10-13  
**Recovery Status**: ✅ COMPLETE  
**Next Step**: Implement `solar_parasol_system.src` according to roadmap
