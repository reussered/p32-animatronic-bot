# OpenSCAD Shape Generation Specification

## Overview

The P32 Animatronic Bot includes an automated STL generation system that converts JSON hardware configurations into 3D printable components using OpenSCAD templates.

## Architecture

### Template System
- **Base Templates**: `assets/shapes/scad/templates/` contain parametric OpenSCAD models
- **Parameter Injection**: JSON configs provide dimensions, mounting holes, and features
- **Automated Generation**: Scripts convert JSON + template → customized STL files
- **Quality Analysis**: Validation of mesh integrity, printability, and tolerances

### File Organization
```
assets/shapes/
├── scad/
│   ├── templates/
│   │   ├── display_mount.scad
│   │   ├── sensor_bracket.scad  
│   │   └── speaker_enclosure.scad
│   ├── generated/
│   │   ├── goblin_eye_left.scad
│   │   └── goblin_mouth.scad
├── stl/
│   ├── goblin_eye_left.stl
│   └── goblin_mouth.stl
└── descriptions/
    ├── goblin_eye_left.md
    └── goblin_mouth.md
```

## Shape Generation Process

### Step 1: Hardware Configuration Analysis
Extract physical parameters from hardware JSON:

```json
{
  "hardware_type": "GC9A01_DISPLAY",
  "physical_specs": {
    "diameter": "1.28 INCH",
    "thickness": "0.08 INCH", 
    "mounting_holes": {
      "count": 4,
      "diameter": "0.1 INCH",
      "spacing": "1.1 INCH"
    }
  }
}
```

### Step 2: Position Integration  
Incorporate spatial positioning from positioned component JSON:

```json
{
  "position": {
    "x": "-1.05 INCH",
    "y": "+0.7 INCH", 
    "z": "-0.35 INCH"
  },
  "orientation": {
    "tilt": "15 DEGREES",
    "rotation": "0 DEGREES"
  }
}
```

### Step 3: Template Parameter Injection
Generate OpenSCAD file with injected parameters:

```scad
// Generated from goblin_eye_left.json
module goblin_eye_left() {
    display_mount(
        diameter = 32.512,      // 1.28 INCH converted to mm
        thickness = 2.032,      // 0.08 INCH converted to mm
        mount_holes = 4,
        hole_diameter = 2.54,   // 0.1 INCH converted to mm
        hole_spacing = 27.94,   // 1.1 INCH converted to mm
        tilt_angle = 15,
        position = [-26.67, 17.78, -8.89]  // Converted coordinates
    );
}
```

### Step 4: STL Generation
```bash
openscad -o assets/shapes/stl/goblin_eye_left.stl assets/shapes/scad/generated/goblin_eye_left.scad
```

## Template Libraries

### Display Mount Template
File: `assets/shapes/scad/templates/display_mount.scad`

```scad
module display_mount(diameter, thickness, mount_holes, hole_diameter, hole_spacing, tilt_angle, position) {
    translate(position) {
        rotate([tilt_angle, 0, 0]) {
            difference() {
                // Main mounting ring
                cylinder(h=thickness*2, d=diameter+5);
                
                // Display cutout  
                translate([0,0,-1]) 
                    cylinder(h=thickness*2+2, d=diameter);
                
                // Mounting holes
                for(i=[0:mount_holes-1]) {
                    rotate([0, 0, i*(360/mount_holes)]) {
                        translate([hole_spacing/2, 0, -1])
                            cylinder(h=thickness*2+2, d=hole_diameter);
                    }
                }
            }
        }
    }
}
```

### Sensor Bracket Template
File: `assets/shapes/scad/templates/sensor_bracket.scad`

```scad
module sensor_bracket(width, height, depth, sensor_holes, mounting_holes, position) {
    translate(position) {
        difference() {
            // Main bracket body
            cube([width, height, depth]);
            
            // Sensor mounting holes
            for(hole = sensor_holes) {
                translate([hole.x, hole.y, -1])
                    cylinder(h=depth+2, d=hole.diameter);
            }
            
            // Case mounting holes  
            for(hole = mounting_holes) {
                translate([hole.x, hole.y, -1])
                    cylinder(h=depth+2, d=hole.diameter);
            }
        }
    }
}
```

## Personality Integration

### Goblin Characteristics
- **Broken Horn**: Left horn 70% normal height with jagged edges
- **Weathered Surfaces**: Add surface texture and wear patterns
- **Asymmetric Features**: Slight position variations for character

### Shape Modification Parameters
```json
{
  "personality_traits": {
    "horn_left_damage": 0.7,
    "surface_weathering": true,
    "asymmetry_factor": 0.05,
    "battle_scars": [
      {"type": "notch", "position": "ear_left", "size": 0.2},
      {"type": "crack", "position": "forehead", "length": 0.8}
    ]
  }
}
```

## Quality Analysis Workflow

### Mesh Validation
1. **Watertight Check**: Ensure closed manifold geometry
2. **Normal Consistency**: Verify proper face orientation  
3. **Self-Intersection**: Detect and resolve overlapping geometry
4. **Minimum Feature Size**: Validate printability at 0.2mm layer height

### Tolerance Analysis
- **Mounting Holes**: +0.1mm clearance for M3 screws
- **Display Fitment**: +0.2mm clearance for component insertion
- **Assembly Clearances**: 0.5mm minimum between moving parts

### Print Optimization
- **Support Structure**: Minimize overhangs beyond 45°
- **Layer Orientation**: Optimize strength for mounting stresses
- **Infill Requirements**: 20% minimum for structural components

## Generation Scripts

### PowerShell Generation Script
File: `generate_shapes.ps1`

```powershell
# Generate 3D shapes from JSON configurations
param(
    [string]$BotConfig = "config/bots/goblin_full.json"
)

$bot = Get-Content $BotConfig | ConvertFrom-Json

foreach($component in $bot.components) {
    $positioned = Get-Content $component | ConvertFrom-Json
    $hardware = Get-Content $positioned.hardware_reference | ConvertFrom-Json
    
    # Generate OpenSCAD file
    $scadPath = "assets/shapes/scad/generated/$($positioned.component_id).scad"
    Generate-ScadFromJson -Hardware $hardware -Position $positioned -Output $scadPath
    
    # Generate STL
    $stlPath = "assets/shapes/stl/$($positioned.component_id).stl"  
    & openscad -o $stlPath $scadPath
    
    # Generate description
    $mdPath = "assets/shapes/descriptions/$($positioned.component_id).md"
    Generate-ShapeDescription -Component $positioned -Output $mdPath
}
```

## Integration with Bot System

### Component Shape References
Each positioned component can reference generated shapes:

```json  
{
  "component_id": "goblin_eye_left",
  "hardware_reference": "config/components/hardware/gc9a01_display.json",
  "interface_assignment": "SPI_DEVICE_1", 
  "position": { ... },
  "generated_shapes": [
    "assets/shapes/stl/goblin_eye_left.stl",
    "assets/shapes/stl/goblin_eye_left_bracket.stl"
  ]
}
```

### Iterative Refinement
1. **Generate Initial Shapes**: From JSON specifications
2. **Test Fit**: Print prototypes and verify fitment
3. **Update Parameters**: Adjust JSON configs based on test results  
4. **Regenerate**: Automated regeneration with updated parameters
5. **Version Control**: Track shape evolution through Git