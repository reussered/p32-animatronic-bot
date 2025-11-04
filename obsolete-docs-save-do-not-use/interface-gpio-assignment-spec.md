# Interface GPIO Assignment Specification

## Overview

The P32 Animatronic Bot uses a two-component interface architecture that maximizes pin efficiency through shared bus patterns.

## Architecture Pattern: Bus + Device

### Concept

Instead of self-contained interface definitions, we use:
- **Bus Components**: Define shared pins (MOSI, MISO, CLK, SDA, SCL, etc.)
- **Device Components**: Reference bus + add unique pins (CS, data pins)

### Pin Efficiency Analysis

#### SPI Interface Pattern
- **Shared Bus Pins**: 3 pins (MOSI, MISO, CLK)
- **Unique Device Pins**: 1 pin per device (CS)
- **Total Pin Usage**: 3 + N pins for N devices
- **Efficiency**: 4 pins for 1 display, 5 pins for 2 displays, 6 pins for 3 displays

#### I2S Interface Pattern  
- **Shared Bus Pins**: 2 pins (BCLK, WS)
- **Unique Device Pins**: 1 pin per device (DATA)
- **Total Pin Usage**: 2 + N pins for N devices

## ESP32-S3 GPIO Pool Management

### Available GPIO Pins
ESP32-S3-DevKitC-1 available GPIO pins:
```
GPIO 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 
GPIO 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48
```

### Pin Assignment Strategy
- **SPI Bus**: GPIO 12 (MISO), GPIO 13 (MOSI), GPIO 14 (CLK)
- **SPI Devices**: GPIO 15 (CS1), GPIO 16 (CS2), GPIO 17 (CS3)
- **I2S Bus**: GPIO 4 (BCLK), GPIO 5 (WS)  
- **I2S Devices**: GPIO 6 (DATA1), GPIO 7 (DATA2), GPIO 8 (DATA3)
- **GPIO Pairs**: GPIO 9/10, GPIO 11/18 for sensors

## Interface Configuration Files

### SPI Bus Definition
File: `config/components/interfaces/spi_bus.json`

```json
{
  "relative_filename": "config/components/interfaces/spi_bus.json", 
  "version": "1.0.0",
  "author": "config/author.json",
  "interface_id": "spi_bus",
  "interface_type": "SPI_BUS",
  "pins": {
    "miso": 12,
    "mosi": 13, 
    "clk": 14
  },
  "bus_config": {
    "frequency": "10000000",
    "mode": 0
  }
}
```

### SPI Device Definition
File: `config/components/interfaces/spi_device_1.json`

```json
{
  "relative_filename": "config/components/interfaces/spi_device_1.json",
  "version": "1.0.0", 
  "author": "config/author.json",
  "interface_id": "SPI_DEVICE_1",
  "interface_type": "SPI_DEVICE",
  "bus_reference": "spi_bus",
  "pins": {
    "cs": 15
  },
  "device_config": {
    "cs_active_low": true
  }
}
```

### I2S Bus Definition
File: `config/components/interfaces/i2s_bus_0.json`

```json
{
  "relative_filename": "config/components/interfaces/i2s_bus_0.json",
  "version": "1.0.0",
  "author": "config/author.json", 
  "interface_id": "I2S_BUS_0",
  "interface_type": "I2S_BUS",
  "pins": {
    "bclk": 4,
    "ws": 5
  },
  "bus_config": {
    "sample_rate": "44100",
    "bits_per_sample": 16
  }
}
```

### I2S Device Definition  
File: `config/components/interfaces/i2s_device_1.json`

```json
{
  "relative_filename": "config/components/interfaces/i2s_device_1.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "interface_id": "I2S_DEVICE_1", 
  "interface_type": "I2S_DEVICE",
  "bus_reference": "I2S_BUS_0",
  "pins": {
    "data": 6
  },
  "device_config": {
    "channel": "left"
  }
}
```

## Hardware Component Integration

### Display Components
Each positioned display component references an SPI device:

```json
{
  "hardware_reference": "config/components/hardware/gc9a01_display.json",
  "position": { ... }
}
```

### Audio Components  
Each positioned speaker component references an I2S device:

```json
{
  "hardware_reference": "config/components/hardware/speaker.json", 
  "position": { ... }
}
```

## Implementation Benefits

1. **Pin Efficiency**: Shared buses minimize GPIO usage
2. **Modularity**: Bus and device configs are independent
3. **Scalability**: Easy to add more devices to existing buses
4. **Clarity**: Clear separation between shared and unique pins
5. **Reusability**: Bus definitions can be reused across different bots
