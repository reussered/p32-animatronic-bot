# Enhanced Goblin Head Coordinate System Specification

## Overview
This document defines the comprehensive coordinate system for the goblin head subsystem, expanding on the original skull_3d system to include anatomical landmarks, component mounting points, and integration specifications for the broader P32 animatronic architecture.

## Reference Frame Definition

### Primary Origin
- **Reference Point:** `nose_center` (0, 0, 0)
- **Coordinate System:** `skull_3d` with anatomical enhancements
- **Units:** Inches for JSON configurations, millimeters for OpenSCAD
- **Conversion Factor:** 25.4 mm/inch

### Axis Orientation
- **+X Axis:** Right side of goblin (goblin's right, observer's left when facing)
- **+Y Axis:** Forward direction (toward goblin's face)  
- **+Z Axis:** Upward direction (toward top of skull)

## Component Positions (P32 Standard)

### Primary Components
```json
{
  "left_eye": {
    "position": {"x": "-1.05 INCH", "y": "+0.7 INCH", "z": "-0.35 INCH"},
    "mm_equivalent": {"x": -26.67, "y": 17.78, "z": -8.89},
    "hardware": "GC9A01 240x240 circular display",
    "interface": "SPI_DEVICE_1"
  },
  "right_eye": {
    "position": {"x": "+1.05 INCH", "y": "+0.7 INCH", "z": "-0.35 INCH"},
    "mm_equivalent": {"x": 26.67, "y": 17.78, "z": -8.89},
    "hardware": "GC9A01 240x240 circular display", 
    "interface": "SPI_DEVICE_2"
  },
  "nose_sensor": {
    "position": {"x": "0 INCH", "y": "0 INCH", "z": "+0.25 INCH"},
    "mm_equivalent": {"x": 0, "y": 0, "z": 6.35},
    "hardware": "HC-SR04 ultrasonic distance sensor",
    "interface": "GPIO_PAIR_1"
  },
  "mouth_display": {
    "position": {"x": "0 INCH", "y": "-1.05 INCH", "z": "0 INCH"},
    "mm_equivalent": {"x": 0, "y": -26.67, "z": 0},
    "hardware": "GC9A01 240x240 circular display",
    "interface": "SPI_DEVICE_3"
  }
}
```

## Anatomical Landmarks (Enhanced System)

### Skull Geometry Reference Points
```json
{
  "cranium_peak": {
    "position": {"x": "0 INCH", "y": "+0.4 INCH", "z": "+1.77 INCH"},
    "mm_equivalent": {"x": 0, "y": 10, "z": 45},
    "description": "Highest point of goblin skull, sagittal crest attachment"
  },
  "brow_ridge_center": {
    "position": {"x": "0 INCH", "y": "+1.0 INCH", "z": "+0.35 INCH"}, 
    "mm_equivalent": {"x": 0, "y": 25.4, "z": 8.89},
    "description": "Prominent goblin brow ridge, intimidation feature"
  },
  "left_temple": {
    "position": {"x": "-1.38 INCH", "y": "+0.4 INCH", "z": "+0.2 INCH"},
    "mm_equivalent": {"x": -35, "y": 10, "z": 5},
    "description": "Temporal bone prominence, muscle attachment"
  },
  "right_temple": {
    "position": {"x": "+1.38 INCH", "y": "+0.4 INCH", "z": "+0.2 INCH"},
    "mm_equivalent": {"x": 35, "y": 10, "z": 5},
    "description": "Temporal bone prominence, muscle attachment"
  },
  "jaw_hinge_left": {
    "position": {"x": "-1.77 INCH", "y": "+0.6 INCH", "z": "-0.2 INCH"},
    "mm_equivalent": {"x": -45, "y": 15, "z": -5},
    "description": "TMJ location, jaw articulation point"
  },
  "jaw_hinge_right": {
    "position": {"x": "+1.77 INCH", "y": "+0.6 INCH", "z": "-0.2 INCH"},
    "mm_equivalent": {"x": 45, "y": 15, "z": -5},
    "description": "TMJ location, jaw articulation point"
  },
  "jaw_tip": {
    "position": {"x": "0 INCH", "y": "-0.6 INCH", "z": "-1.38 INCH"},
    "mm_equivalent": {"x": 0, "y": -15, "z": -35},
    "description": "Lowest point of mandible, chin equivalent"
  }
}
```

### Dental Reference Points (Goblin-Specific)
```json
{
  "left_upper_fang": {
    "position": {"x": "-0.31 INCH", "y": "-0.8 INCH", "z": "+0.12 INCH"},
    "mm_equivalent": {"x": -8, "y": -20, "z": 3},
    "description": "Left upper canine, prominent goblin fang"
  },
  "right_upper_fang": {
    "position": {"x": "+0.31 INCH", "y": "-0.8 INCH", "z": "+0.12 INCH"},
    "mm_equivalent": {"x": 8, "y": -20, "z": 3},
    "description": "Right upper canine, prominent goblin fang"
  }
}
```

## Dimensional Specifications

### Overall Skull Dimensions
- **Length (Y-axis):** 140mm (5.51 inches) - 85% of human proportions
- **Width (X-axis):** 120mm (4.72 inches) - broader than human for eye spacing
- **Height (Z-axis):** 90mm (3.54 inches) - 30% compressed vertically
- **Eye Spacing:** 76.2mm (3.0 inches) - matches P32 specification

### Component Clearances
- **Eye Mount Diameter:** 30mm (accommodates GC9A01 + shell)
- **Nose Sensor Volume:** 20mm x 15mm x 8mm (HC-SR04 dimensions)
- **Mouth Mount Diameter:** 30mm (accommodates GC9A01 + shell)
- **Internal Cable Routing:** 10mm diameter channels minimum

## Integration Points

### Head-Neck Joint Interface
```json
{
  "neck_attachment_point": {
    "position": {"x": "0 INCH", "y": "-0.8 INCH", "z": "-2.36 INCH"},
    "mm_equivalent": {"x": 0, "y": -20, "z": -60},
    "description": "Standard attachment point for goblin_full_torso",
    "interface_type": "servo_mount_standard",
    "rotation_range": {"yaw": "45", "pitch": "30", "roll": "15"}
  }
}
```

### Electronics Housing
```json
{
  "main_controller_bay": {
    "position": {"x": "0 INCH", "y": "-0.8 INCH", "z": "-1.18 INCH"},
    "mm_equivalent": {"x": 0, "y": -20, "z": -30},
    "dimensions": {"x": "2.17 INCH", "y": "1.1 INCH", "z": "0.31 INCH"},
    "mm_dimensions": {"x": 55, "y": 28, "z": 8},
    "description": "ESP32-S3-DevKitC-1 mounting bay in skull base"
  }
}
```

## Coordinate System Extensions

### Future Expansion Points
```json
{
  "ear_attachment_left": {
    "position": {"x": "-2.17 INCH", "y": "+0.8 INCH", "z": "+0.4 INCH"},
    "mm_equivalent": {"x": -55, "y": 20, "z": 10},
    "description": "Future goblin ear component mounting"
  },
  "ear_attachment_right": {
    "position": {"x": "+2.17 INCH", "y": "+0.8 INCH", "z": "+0.4 INCH"},
    "mm_equivalent": {"x": 55, "y": 20, "z": 10},
    "description": "Future goblin ear component mounting"
  },
  "horn_mount_left": {
    "position": {"x": "-0.79 INCH", "y": "+0.98 INCH", "z": "+1.57 INCH"},
    "mm_equivalent": {"x": -20, "y": 25, "z": 40},
    "description": "Optional horn attachment for variant goblins"
  },
  "horn_mount_right": {
    "position": {"x": "+0.79 INCH", "y": "+0.98 INCH", "z": "+1.57 INCH"},
    "mm_equivalent": {"x": 20, "y": 25, "z": 40},
    "description": "Optional horn attachment for variant goblins"
  }
}
```

### Cable Routing Channels
```json
{
  "main_cable_spine": {
    "start": {"x": "0 INCH", "y": "-0.8 INCH", "z": "-1.18 INCH"},
    "end": {"x": "0 INCH", "y": "-0.8 INCH", "z": "-2.36 INCH"},
    "diameter": "0.39 INCH",
    "mm_diameter": 10,
    "description": "Primary cable route from controller to neck joint"
  },
  "eye_cable_left": {
    "start": {"x": "-1.05 INCH", "y": "+0.7 INCH", "z": "-0.35 INCH"},
    "end": {"x": "0 INCH", "y": "-0.8 INCH", "z": "-1.18 INCH"},
    "description": "SPI + power routing to left eye display"
  },
  "eye_cable_right": {
    "start": {"x": "+1.05 INCH", "y": "+0.7 INCH", "z": "-0.35 INCH"},
    "end": {"x": "0 INCH", "y": "-0.8 INCH", "z": "-1.18 INCH"},
    "description": "SPI + power routing to right eye display"
  }
}
```

## Manufacturing Tolerances

### 3D Printing Specifications
- **Layer Height:** 0.2mm (0.008 inches) for structural components
- **Layer Height:** 0.1mm (0.004 inches) for detail features (fangs, etc.)
- **Wall Thickness:** 2mm (0.079 inches) minimum for structural integrity
- **Support Material:** Required for overhangs > 45
- **Infill Density:** 20% for skull shell, 100% for mounting points

### Assembly Tolerances
- **Component Fits:** +0.2mm (+0.008 inches) clearance for slip fits
- **Press Fits:** -0.1mm (-0.004 inches) interference for permanent assembly
- **Threading:** M3 hardware standard for all mechanical fasteners
- **Electrical Clearance:** 2mm (0.079 inches) minimum between conductors

## Coordinate System Validation

### Test Points for Verification
```json
{
  "validation_measurements": [
    {
      "name": "eye_spacing",
      "measurement": "distance between left_eye and right_eye centers",
      "expected_value": "3.0 INCH  0.02 INCH",
      "tolerance": "0.02 INCH"
    },
    {
      "name": "eye_to_nose_distance", 
      "measurement": "distance from eye_center to nose_center",
      "expected_value": "0.90 INCH  0.02 INCH",
      "tolerance": "0.02 INCH"
    },
    {
      "name": "nose_to_mouth_distance",
      "measurement": "distance from nose_center to mouth_center", 
      "expected_value": "1.05 INCH  0.02 INCH",
      "tolerance": "0.02 INCH"
    }
  ]
}
```

## Related Documentation
- `docs/coordinate-system-spec.md` - Original P32 coordinate system
- `docs/goblin-full-interconnection-diagram.md` - Electrical integration
- `docs/two-tier-mounting-system.md` - Component mounting architecture
- `assets/shapes/scad/goblin_skull_complete.scad` - OpenSCAD implementation
- `assets/shapes/scad/goblin_head_assembly.scad` - Complete assembly view

---

**Document Version:** 2.0  
**Last Updated:** October 14, 2025  
**Scope:** Goblin head subsystem (part of full-body P32 architecture)  
**Status:** Implementation ready, STL generation pending