# Goblin Eye Left Display Mount

## Component Overview
3D printed mounting bracket for the left eye GC9A01 circular display in the Goblin Full animatronic bot.

## Physical Specifications
- **Display Type**: GC9A01 1.28" Circular LCD
- **Mount Diameter**: 37.5mm (display diameter + 5mm clearance) 
- **Thickness**: 4.06mm (2x display thickness for strength)
- **Material**: PLA or PETG recommended
- **Print Time**: ~15 minutes at 0.2mm layer height

## Mounting Features
- **Display Mounting**: 4 holes at 27.94mm spacing for M2.5 screws
- **Case Mounting**: 4 corner holes for M3 screws at 45° offsets
- **Cable Management**: Side channel for display ribbon cable
- **Reinforcement Ring**: Thicker base layer for structural strength

## Coordinate Position
- **Skull Position**: (-1.05", +0.7", -0.35") from nose center
- **Final Position**: (-26.67mm, +17.78mm, -8.89mm) 
- **Tilt**: 0° (facing straight forward)
- **Interface**: Connected to SPI_DEVICE_1 (GPIO 15 CS pin)

## Print Settings
- **Layer Height**: 0.2mm
- **Infill**: 20% 
- **Supports**: None required
- **Print Orientation**: Flat on build plate, mount holes facing up
- **Nozzle Temperature**: 210°C (PLA) / 240°C (PETG)
- **Bed Temperature**: 60°C (PLA) / 80°C (PETG)

## Assembly Instructions
1. Print part flat on build plate
2. Clean support material from holes if needed
3. Test fit GC9A01 display (should slide in snugly)
4. Install display with M2.5 x 8mm screws
5. Connect ribbon cable through side channel
6. Mount to skull frame with M3 x 10mm screws

## Integration Notes  
- Designed for 30% vertically compressed goblin skull geometry
- Compatible with skull_3d coordinate system scaling
- Cable routing planned for SPI bus sharing with right eye and mouth
- Reinforcement ribs handle mounting stress from head movement

## Revision History
- v1.0: Initial design based on GC9A01 hardware specifications
- Compatible with goblin_full.json bot configuration