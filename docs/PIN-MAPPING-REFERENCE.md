# Pin Mapping Reference Guide

## Generic Pin Functions (lowercase)
- `data_input`: SPI MOSI signal (Master Out, Slave In)
- `data_output`: SPI MISO signal (Master In, Slave Out) 
- `clock`: SPI clock signal
- `chip_select`: SPI chip select signal
- `data_command`: Display data/command control
- `reset`: Hardware reset signal
- `power_positive`: Positive power supply
- `power_ground`: Ground connection

## Your Display Wiring (Waveshare GC9A01)

| ESP32-S3 Pin | Generic Name | Your Display Label | Function |
|--------------|--------------|-------------------|----------|
| 13 | data_input | SDA | Data from ESP32 to display |
| 12 | data_output | (not connected) | Data from display to ESP32 |
| 14 | clock | SCL | SPI clock signal |
| 15 | chip_select | CS | Chip select |
| 2 | data_command | DC | Data/Command control |
| 4 | reset | RST | Hardware reset |
| 3.3V | power_positive | VCC | Power supply |
| GND | power_ground | GND | Ground |

## Configuration Files Updated

1. **SPI_BUS_VSPI**: Now uses generic `data_input`, `data_output`, `clock`
2. **SPI_DEVICE_1**: Now uses generic `chip_select`, `data_command`, `reset`
3. **MakerNames.json**: Maps manufacturer labels to generic names

## Code Generation Impact

The component generator will:
1. Read generic pin names from component configs
2. Look up manufacturer variant in MakerNames.json  
3. Generate code using the correct pin mappings
4. Handle different manufacturer naming conventions automatically

This abstraction allows the same component definitions to work with displays from different manufacturers just by changing the `manufacturer_variant` field.