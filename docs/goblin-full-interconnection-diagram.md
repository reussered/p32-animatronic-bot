# Goblin Full Interconnection Diagram

## Complete Hardware Configuration

The Goblin Full bot uses 12 GPIO pins total for a 3-display, 1-speaker configuration.

## Pin Assignment Summary

| Component | Interface | GPIO Pins | Pin Function |
|-----------|-----------|-----------|--------------|
| SPI Bus | SPI_BUS_VSPI | 12, 13, 14 | MISO, MOSI, CLK |
| Left Eye Display | SPI_DEVICE_1 | 15 | CS |
| Right Eye Display | SPI_DEVICE_2 | 16 | CS |
| Mouth Display | SPI_DEVICE_3 | 17 | CS |
| I2S Bus | I2S_BUS_0 | 4, 5 | BCLK, WS |
| Speaker | I2S_DEVICE_1 | 6 | DATA |
| Nose Sensor | GPIO_PAIR_1 | 9, 10 | TRIG, ECHO |

**Total GPIO Usage: 12 pins**

## Hardware Wiring Diagram

```
ESP32-S3-DevKitC-1
                    ┌─────────────────┐
                    │                 │
           GPIO 4   │ BCLK            │ ── I2S Speaker BCLK
           GPIO 5   │ WS              │ ── I2S Speaker WS  
           GPIO 6   │ DATA            │ ── I2S Speaker DATA
                    │                 │
           GPIO 9   │ TRIG            │ ── HC-SR04 Nose Sensor TRIG
           GPIO 10  │ ECHO            │ ── HC-SR04 Nose Sensor ECHO
                    │                 │
           GPIO 12  │ MISO            │ ── SPI Bus MISO (shared)
           GPIO 13  │ MOSI            │ ── SPI Bus MOSI (shared)  
           GPIO 14  │ CLK             │ ── SPI Bus CLK (shared)
                    │                 │
           GPIO 15  │ CS1             │ ── Left Eye Display CS
           GPIO 16  │ CS2             │ ── Right Eye Display CS
           GPIO 17  │ CS3             │ ── Mouth Display CS
                    │                 │
                    └─────────────────┘
```

## Component Configuration Files

### Interface Assignments

#### SPI Bus Configuration
- **File**: `config/components/interfaces/spi_bus_vspi.json`
- **Pins**: GPIO 12 (MISO), GPIO 13 (MOSI), GPIO 14 (CLK)
- **Frequency**: 10 MHz

#### SPI Device Configurations
1. **Left Eye**: `spi_device_1.json` → GPIO 15 (CS)
2. **Right Eye**: `spi_device_2.json` → GPIO 16 (CS)  
3. **Mouth**: `spi_device_3.json` → GPIO 17 (CS)

#### I2S Bus Configuration
- **File**: `config/components/interfaces/i2s_bus_0.json`
- **Pins**: GPIO 4 (BCLK), GPIO 5 (WS)
- **Sample Rate**: 44.1 kHz, 16-bit

#### I2S Device Configuration
- **Speaker**: `i2s_device_1.json` → GPIO 6 (DATA)

#### GPIO Pair Configuration
- **Nose Sensor**: `gpio_pair_1.json` → GPIO 9 (TRIG), GPIO 10 (ECHO)

### Positioned Component Mappings

#### Goblin Eye Left
- **Hardware**: `config/components/hardware/gc9a01_display.json`
- **Interface**: `SPI_DEVICE_1` 
- **Position**: (-1.05", +0.7", -0.35") in skull_3d coordinates
- **File**: `config/components/positioned/goblineye_left.json`

#### Goblin Eye Right  
- **Hardware**: `config/components/hardware/gc9a01_display.json`
- **Interface**: `SPI_DEVICE_2`
- **Position**: (+1.05", +0.7", -0.35") in skull_3d coordinates
- **File**: `config/components/positioned/goblineye_right.json`

#### Goblin Mouth
- **Hardware**: `config/components/hardware/gc9a01_display.json` 
- **Interface**: `SPI_DEVICE_3`
- **Position**: (0", -1.05", 0") in skull_3d coordinates
- **File**: `config/components/positioned/goblin_mouth.json`

#### Goblin Speaker
- **Hardware**: `config/components/hardware/speaker.json`
- **Interface**: `I2S_DEVICE_1`
- **Position**: (-0.5", +0.5", -1.0") in skull_3d coordinates  
- **File**: `config/components/positioned/goblin_speaker.json`

#### Goblin Nose Sensor
- **Hardware**: `config/components/hardware/hc_sr04_sensor.json`
- **Interface**: `GPIO_PAIR_1`
- **Position**: (0", 0", +0.25") in skull_3d coordinates
- **File**: `config/components/positioned/goblin_nose.json`

## Bot Definition Integration

The complete bot definition in `config/bots/goblin_full.json` references all positioned components:

```json
{
  "bot_type": "GOBLINFULL",
  "coordinate_system": "skull_3d", 
  "eye_spacing": "3.0 INCH",
  "compression_factor": 0.7,
  "components": [
    "config/components/positioned/goblineye_left.json",
    "config/components/positioned/goblineye_right.json", 
    "config/components/positioned/goblin_mouth.json",
    "config/components/positioned/goblin_speaker.json",
    "config/components/positioned/goblin_nose.json"
  ]
}
```

## Power and Ground Connections

All components require:
- **5V Power**: Connect to ESP32-S3 5V pin or external 5V supply
- **Ground**: Connect to ESP32-S3 GND pins
- **3.3V Logic**: ESP32-S3 GPIO pins operate at 3.3V logic levels

## Pin Efficiency Analysis

- **3 SPI Displays**: Uses 6 pins total (3 shared + 3 unique CS pins)
- **1 I2S Speaker**: Uses 3 pins total (2 shared + 1 unique data pin)  
- **1 GPIO Sensor**: Uses 2 pins total (2 unique pins)
- **Total**: 12 pins for complete face with audio and sensing

This leaves 26+ GPIO pins available for expansion (arms, mobility, additional sensors).

## 3D Printable Components

### Generated STL Files (Ready for 3D Printing)
The complete goblin head system includes professionally generated STL files:

```bash
assets/shapes/stl/
├── goblin_skull_complete.stl          # Main skull (57.5g, 8-12hr print)
├── goblin_head_assembly.stl           # Full assembly visualization  
├── goblin_nose_sensor.stl             # HC-SR04 mount (15g, 2-3hr print)
├── example_display_mount.stl          # GC9A01 mount (8g, 1-2hr print)
├── sensor_basic_mount.stl             # Universal sensor bracket
└── test_cube.stl                      # Printer calibration cube
```

### Print Specifications Summary
- **Total Assembly Weight**: 249.75g (calculated from OpenSCAD)
- **Required Motor Torque**: 0.795 N⋅m (112.5 oz⋅in)
- **Recommended Motors**: NEMA 17 High Torque (≥120 oz⋅in)
- **Material**: PLA plastic, 20% infill structural parts
- **Print Time**: ~12-15 hours total for complete head
- **Color**: Goblin green (#4A5D23) or equivalent

### Manufacturing Documentation
Complete 3D printing guide available at:
**`docs/goblin-head-3d-printing-guide.md`** - Comprehensive printing specifications, color schemes, quality control procedures, and assembly integration.

### Motor Load Analysis
Complete motor sizing analysis available at:
**`docs/goblin-head-weight-and-motor-analysis.md`** - Weight calculations, torque requirements, safety factors, and NEMA 17 motor specifications for responsive head movement.