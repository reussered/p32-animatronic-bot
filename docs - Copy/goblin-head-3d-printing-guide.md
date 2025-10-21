# Goblin Head 3D Printable Parts Guide

## Overview
Complete 3D printing guide for the Goblin Full animatronic head system. All STL files generated from OpenSCAD designs with integrated weight calculations and motor load analysis.

## Generated STL Files Status

### Available STL Files
```bash
assets/shapes/stl/
├── goblin_skull_complete.stl          # Main skull structure
├── goblin_head_assembly.stl           # Complete assembly with electronics
├── goblin_nose_sensor.stl             # Nose sensor mount
├── example_display_mount.stl          # Display mounting example
├── sensor_basic_mount.stl             # Basic sensor mount
└── test_cube.stl                      # Calibration test cube
```

## Printing Specifications

### Material Requirements
- **Primary Material**: PLA plastic
- **Density**: 1.25 g/cm³
- **Color**: Goblin green (#4A5D23) recommended
- **Alternative Colors**: Dark gray, brown, or natural PLA

### Print Settings
```json
{
  "layer_height": "0.2mm",
  "infill_density": {
    "structural_parts": "20-30%",
    "mounting_rings": "100%",
    "decorative_shells": "10-15%"
  },
  "wall_thickness": "2.0mm minimum",
  "support_material": "Auto-generated for overhangs >45°",
  "print_speed": "50mm/s standard, 25mm/s details",
  "nozzle_temperature": "210°C PLA",
  "bed_temperature": "60°C"
}
```

## Part-by-Part Printing Guide

### 1. Goblin Skull Complete (`goblin_skull_complete.stl`)
```yaml
Part Details:
  Description: Main skull structure with integrated component mounts
  Print Time: ~8-12 hours
  Material Usage: ~58g PLA
  Infill: 20%
  Supports: Yes (around eye sockets and jaw area)
  
Printing Notes:
  - Orient with skull base flat on bed
  - Enable supports for eye socket overhangs
  - Use tree supports to minimize cleanup
  - Print brim for better bed adhesion
  
Post-Processing:
  - Remove supports carefully around display mounts
  - Sand eye socket surfaces smooth
  - Test fit GC9A01 displays before assembly
  
Weight Analysis:
  Printed Weight: 57.5g
  Center of Mass: [0, -5, 10]mm from nose center
  Structural Load: Primary skull bearing component
```

### 2. Goblin Head Assembly (`goblin_head_assembly.stl`)
```yaml
Part Details:
  Description: Complete assembly model with electronics shown
  Print Time: ~15-20 hours
  Material Usage: ~125g PLA
  Infill: 20% skull, 100% mounting points
  Supports: Yes (extensive)
  
Printing Notes:
  - This is a visualization/test model
  - Contains embedded electronics representations
  - Use for fit testing and assembly planning
  - Not intended for actual use - print individual components
  
Assembly Validation:
  - Verify all component clearances
  - Check wire routing paths
  - Confirm mounting hole alignment
  - Test motor attachment points
```

### 3. Goblin Nose Sensor Mount (`goblin_nose_sensor.stl`)
```yaml
Part Details:
  Description: HC-SR04 ultrasonic sensor mounting bracket
  Print Time: ~2-3 hours
  Material Usage: ~15g PLA
  Infill: 100% (structural component)
  Supports: Minimal
  
Printing Notes:
  - Orient with mounting face down
  - High infill for structural integrity
  - Precise sensor pocket dimensions
  - No supports needed for basic geometry
  
Post-Processing:
  - Test fit HC-SR04 sensor
  - Drill M3 mounting holes if needed
  - Smooth sensor pocket surfaces
  
Installation:
  Position: Goblin nose tip
  Coordinates: [0, 15, -5]mm from nose center
  Wiring: GPIO 9 (TRIG), GPIO 10 (ECHO)
```

### 4. Display Basic Mount (`example_display_mount.stl`)
```yaml
Part Details:
  Description: Standard GC9A01 display mounting ring
  Print Time: ~1-2 hours
  Material Usage: ~8g PLA
  Infill: 100% (structural)
  Supports: None required
  
Printing Notes:
  - Print multiple copies (3 needed for full system)
  - Orient flat for best surface finish
  - Tight tolerances for display fit
  - Check dimensions with calipers
  
Usage Locations:
  Left Eye: [-38.1, 0, 0]mm (1.5" spacing)
  Right Eye: [38.1, 0, 0]mm (1.5" spacing)  
  Mouth: [0, -25.4, -38.1]mm (below nose)
  
Wire Management:
  - SPI shared bus: GPIO 12,13,14
  - Individual CS pins: GPIO 15,16,17
```

### 5. Sensor Basic Mount (`sensor_basic_mount.stl`)
```yaml
Part Details:
  Description: Universal sensor mounting bracket
  Print Time: ~1.5-2 hours
  Material Usage: ~12g PLA
  Infill: 50% (medium strength)
  Supports: None required
  
Printing Notes:
  - Designed for HC-SR04 and similar sensors
  - Adjustable mounting holes
  - Wire management channels included
  - Print in sensor-compatible orientation
  
Compatibility:
  - HC-SR04 Ultrasonic (primary)
  - PIR motion sensors
  - Distance sensors
  - Light sensors with similar form factor
```

## Assembly Weight Calculations (Automated from OpenSCAD)

### Component Weight Breakdown
```
=== GOBLIN HEAD ASSEMBLY WEIGHT ANALYSIS ===
Head Assembly Weight: 249.75 grams (8.81 oz)
Complete System Weight: 1089.75 grams (2.4 lbs)
Center of Mass: [0, -8.2, -2.1] mm from nose
Moment Arm Length: 59.1 mm
Required Motor Torque: 0.795 N⋅m (112.5 oz⋅in)
Recommended Motor: NEMA 17 with ≥120 oz⋅in holding torque
============================================
```

### Individual Component Weights
```json
{
  "3d_printed_components": {
    "skull_base": "57.5g",
    "mounting_rings": "31.25g", 
    "decorative_shells": "35.0g",
    "total_printed": "123.75g"
  },
  "electronics": {
    "gc9a01_displays_3x": "24.0g",
    "hc_sr04_sensor": "9.0g", 
    "esp32_s3_devkit": "25.0g",
    "wire_harness": "30.0g",
    "total_electronics": "88.0g"
  },
  "hardware": {
    "m3_screws": "15.0g",
    "m3_nuts": "8.0g",
    "washers": "3.0g", 
    "threaded_inserts": "12.0g",
    "total_hardware": "38.0g"
  }
}
```

## Multi-Color Printing Options

### Goblin Family Color Palette
```json
{
  "primary_color": "#4A5D23 (Forest Green)",
  "secondary_color": "#8B4513 (Saddle Brown)", 
  "accent_color": "#CD853F (Peru Gold)",
  "highlight_color": "#F5DEB3 (Wheat Cream)"
}
```

### Color Assignment Strategy
```yaml
Layer Assignments:
  Base Skull (Layers 1-80%): Forest Green (#4A5D23)
  Detail Features (Layers 80-90%): Saddle Brown (#8B4513)  
  Accent Lines (Layers 90-95%): Peru Gold (#CD853F)
  Highlights (Layers 95-100%): Wheat Cream (#F5DEB3)

Printing Method:
  - Use automatic color change commands
  - Pause for manual filament changes
  - Or print with MMU-compatible printer
```

## Quality Control and Testing

### Dimensional Verification
```bash
Critical Measurements:
  Eye Spacing: 76.2mm (3.0" exact)
  Display Pocket Diameter: 24.5mm (GC9A01 fit)
  Sensor Mount Opening: 20x15mm (HC-SR04 fit)
  M3 Hole Diameter: 3.2mm (tap clearance)
  Wall Thickness: ≥2.0mm minimum
```

### Fit Testing Protocol
1. **Display Test**: Insert GC9A01 displays into eye and mouth sockets
2. **Sensor Test**: Mount HC-SR04 in nose bracket
3. **Wiring Test**: Route cables through internal channels  
4. **Assembly Test**: Connect all components with actual hardware
5. **Weight Test**: Verify total weight matches 249.75g ±10%

### Common Printing Issues and Solutions

#### Layer Adhesion Problems
```yaml
Symptoms: Delamination, weak joints
Causes: Low bed temperature, poor first layer
Solutions:
  - Increase bed temperature to 65°C
  - Use glass bed with adhesion spray
  - Print slower first layer (20mm/s)
```

#### Support Material Issues  
```yaml
Symptoms: Rough surfaces, difficult removal
Causes: Over-aggressive supports
Solutions:
  - Use tree supports instead of linear
  - Reduce support density to 15%
  - Enable support interface layers
```

#### Dimensional Accuracy Issues
```yaml
Symptoms: Tight fits, components don't mount
Causes: Printer calibration, filament shrinkage
Solutions:
  - Calibrate steps/mm for all axes
  - Print test cube and measure
  - Adjust hole sizes +0.1mm if needed
```

## Assembly Instructions Integration

### Connection to Main Build Guide
This 3D printing guide integrates with:
- `goblin-full-interconnection-diagram.md` - Electrical connections
- `goblin-head-weight-and-motor-analysis.md` - Motor requirements  
- `goblin-head-coordinate-system.md` - Positioning specifications

### Next Steps After Printing
1. **Clean and finish** all printed parts
2. **Install threaded inserts** in mounting holes
3. **Test fit all electronics** before final assembly
4. **Follow wiring diagram** for electrical connections
5. **Mount and test** complete head assembly
6. **Calibrate motor systems** with actual weight

## Advanced Printing Techniques

### Hollow Printing for Weight Reduction
```yaml
Technique: Vase mode printing for decorative shells
Benefits: 70% weight reduction, faster printing
Applications: Non-structural shells and covers
Settings: 
  - Single wall thickness
  - 0% infill
  - Spiral printing mode
```

### Threaded Insert Installation
```yaml
Pre-Installation:
  - Heat threaded inserts to 200°C
  - Use soldering iron or heat-set tool
  - Press inserts flush with surface
  
Insert Specifications:
  - M3 x 5mm threaded inserts
  - Brass or stainless steel
  - 12 total inserts required
```

### Professional Finishing
```yaml
Surface Treatment:
  - Sand with 220-400 grit
  - Apply primer if painting
  - Use automotive paint for durability
  
Protective Coatings:
  - Clear coat for UV protection
  - Conformal coating on electronics mounts
  - Anti-static treatment if needed
```

---

**Document Version:** 1.0  
**Last Updated:** October 14, 2025  
**STL Generation Date:** October 14, 2025  
**Print Status:** Ready for production  
**Quality:** Professional-grade STL files with integrated weight analysis