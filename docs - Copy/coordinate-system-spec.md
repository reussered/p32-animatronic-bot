# Coordinate System Specification

## Overview
The P32 Animatronic Bot supports two distinct coordinate systems to accommodate different physical arrangements and use cases.

## 2D Planar Coordinate System

### Definition
- **Coordinate System ID**: `"planar_2d"`
- **Use Case**: Flat-panel arrangements, wall-mounted displays, simple layouts
- **Reference Point**: `"nose_center"` at (0, 0, 0)
- **Axes**: X (horizontal), Y (vertical), Z (depth - typically 0)

### Example Usage
```json
{
  "coordinate_system": "planar_2d",
  "reference_point": "nose_center",
  "position": {
    "x": "-2.0 INCH",
    "y": "1.5 INCH", 
    "z": "0.0 INCH"
  }
}
```

## 3D Skull-Based Coordinate System

### Definition
- **Coordinate System ID**: `"skull_3d"`
- **Use Case**: Anatomical humanoid faces, realistic animatronics
- **Reference Point**: `"nose_center"` at (0, 0, 0)
- **Baseline**: 3.0" eye spacing as standard human proportion
- **Scaling**: All positions scale proportionally with eye spacing changes

### Anatomical Reference Model
```
        (-1.5", +1.0", -0.5")  (+1.5", +1.0", -0.5")
              LEFT_EYE ●————————————————● RIGHT_EYE
                      |                |
                      |    3.0" EYE    |
                      |    SPACING     |
                      |                |
              (-0.75", 0, +0.25")      | (+0.75", 0, +0.25")
            LEFT_NOSTRIL ●             ● RIGHT_NOSTRIL
                         |  (0,0,0)    |
                         |     ●       |
                         |  NOSE_CTR   |
                         |             |
                   (-1.0", -1.5", 0)   | (+1.0", -1.5", 0)
              MOUTH_LEFT_CORNER ●——————● MOUTH_RIGHT_CORNER
                                |      |
                                | 2.0" |
                                |      |
```

### Goblin Full Implementation
The Goblin Full bot uses a compressed skull variant:
- **Vertical Compression**: 30% (multiply all Y coordinates by 0.7)
- **Eye Spacing**: 3.0" baseline maintained
- **Compression Formula**: `final_y = base_y * 0.7`

### Scaling Algorithm
When eye spacing changes from 3.0" baseline:
```
scale_factor = new_eye_spacing / 3.0
final_x = base_x * scale_factor
final_y = base_y * scale_factor * compression_factor
final_z = base_z * scale_factor
```

## JSON Schema Patterns

### Position Object
```json
{
  "position": {
    "x": "value UNIT",
    "y": "value UNIT", 
    "z": "value UNIT"
  }
}
```

### Supported Units
- `INCH` - Inches (primary unit)
- `MM` - Millimeters  
- `CM` - Centimeters

### Complete Coordinate Definition
```json
{
  "coordinate_system": "skull_3d",
  "reference_point": "nose_center",
  "eye_spacing": "3.0 INCH",
  "compression_factor": 0.7,
  "position": {
    "x": "-1.5 INCH",
    "y": "1.0 INCH",
    "z": "-0.5 INCH"
  }
}
```

## Implementation Notes
- All coordinate values must include unit strings
- Reference point is always `"nose_center"` at origin (0,0,0)
- 3D skull system enables realistic facial component positioning
- Scaling allows same bot definition to work with different physical sizes
- Compression factors enable species-specific facial proportions