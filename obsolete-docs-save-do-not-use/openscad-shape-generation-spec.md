# OpenSCAD Shape Generation Specification

## Overview

The P32 Animatronic Bot includes an automated multi-color 3D printing system that converts JSON hardware configurations into 3D printable components using OpenSCAD templates. The system supports both hard shell (3D printed) and soft body (fabric) construction types with standardized 4-color palettes.

## Architecture

### Construction Type System
- **Hard Shell**: 3D printed ABS/PLA components with rigid mounting systems
- **Soft Body**: Fabric construction with internal framework and flexible mounting
- **Material Profiles**: Standardized specifications for each construction type
- **Color Palettes**: 4-color HTML-named system for multi-color 3D printing

### Template System
- **Base Templates**: `assets/shapes/scad/templates/` contain parametric OpenSCAD models
- **Parameter Injection**: JSON configs provide dimensions, mounting holes, colors, and features
- **Multi-Color Generation**: Scripts convert JSON + template  3MF/AMF files with color regions
- **Construction Variants**: Same geometry adapted for hard shell vs soft body mounting
- **Quality Analysis**: Validation of mesh integrity, printability, and color region assignments

### File Organization
```
assets/shapes/
 scad/
    templates/
       display_mount.scad
       sensor_bracket.scad  
       speaker_enclosure.scad
    generated/
       goblin_eye_left.scad
       goblin_mouth.scad
 stl/
    goblin_eye_left.stl
    goblin_mouth.stl
 descriptions/
     goblin_eye_left.md
     goblin_mouth.md
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
- **Support Structure**: Minimize overhangs beyond 45
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

## Multi-Color 3D Printing System

### Color Palette Architecture
Each bot family includes a standardized 4-color HTML palette for multi-color 3D printing:

```json
"color_palette": {
  "primary": "DarkOliveGreen",
  "secondary": "SaddleBrown", 
  "accent": "Goldenrod",
  "highlight": "OrangeRed"
}
```

### Construction Type Integration
The system supports two construction methodologies:

#### Hard Shell (3D Printed)
- **Primary Material**: PLA/PETG for structural components
- **Secondary Material**: TPU for flexible joints and seals
- **Color Mapping**: PrimaryShell, SecondaryStructure, AccentDetails, HighlightFeatures
- **Output Format**: 3MF files with embedded color regions

#### Soft Body (Fabric/Foam)
- **Primary Material**: Fleece/felt outer covering
- **Secondary Material**: Foam padding and structure
- **Color Mapping**: PrimaryFabric, SecondaryTrim, AccentEmbroidery, HighlightEyes/Features
- **Output Format**: PDF sewing patterns with color-coded regions

### OpenSCAD Multi-Color Implementation

#### Color Region Definition
```scad
// Define color regions using HTML color names
module multi_color_component(construction_type="hard_shell") {
    if (construction_type == "hard_shell") {
        // Primary structure - DarkOliveGreen
        color("DarkOliveGreen") 
            main_shell_structure();
            
        // Secondary details - SaddleBrown
        color("SaddleBrown")
            mounting_brackets();
            
        // Accent features - Goldenrod  
        color("Goldenrod")
            decorative_elements();
            
        // Highlight details - OrangeRed
        color("OrangeRed")
            led_bezels_and_features();
    }
}
```

#### Construction-Aware Generation
```scad
// Construction type parameter drives geometry
CONSTRUCTION_TYPE = "hard_shell"; // or "soft_body"
COLOR_PALETTE = ["DarkOliveGreen", "SaddleBrown", "Goldenrod", "OrangeRed"];

module generate_component(type, palette) {
    if (type == "hard_shell") {
        generate_hard_shell_geometry(palette);
    } else if (type == "soft_body") {
        generate_soft_body_patterns(palette);
    }
}
```

### Enhanced PowerShell Pipeline

#### Multi-Color STL/3MF Generation
File: `tools/generate-stl-files.ps1` (Enhanced)

```powershell
# Enhanced batch generation with multi-color support
param(
    [switch]$BasicMounts,
    [switch]$BotShells, 
    [switch]$Generated,
    [switch]$All,
    [switch]$MultiColor,
    [string]$OutputDir = "assets/shapes/stl",
    [string]$ConstructionType = "hard_shell",
    [string]$Family = ""
)

function Generate-MultiColorFile($scadFile, $outputPath, $constructionType, $family) {
    # Load family color palette if specified
    $paletteArgs = ""
    if ($family) {
        $familyConfig = Get-Content "config/bots/bot_families/$family.json" | ConvertFrom-Json
        $palette = $familyConfig.construction_types.$constructionType.color_palette
        $paletteArgs = "-D `"PRIMARY_COLOR=`"$($palette.primary)`"`" -D `"SECONDARY_COLOR=`"$($palette.secondary)`"`" -D `"ACCENT_COLOR=`"$($palette.accent)`"`" -D `"HIGHLIGHT_COLOR=`"$($palette.highlight)`"`""
    }
    
    $constructionArgs = "-D `"CONSTRUCTION_TYPE=`"$constructionType`"`""
    
    if ($MultiColor) {
        # Generate 3MF with embedded colors
        & openscad -o $outputPath $constructionArgs $paletteArgs $scadFile.FullName
    } else {
        # Generate standard STL
        & openscad -o $outputPath $constructionArgs $scadFile.FullName
    }
}
```

### File Format Support

#### 3MF (3D Manufacturing Format)
- **Multi-Color Support**: Embedded color regions with material assignments
- **Slicer Compatibility**: PrusaSlicer, Bambu Studio, Ultimaker Cura
- **Benefits**: Single file contains geometry + colors + print settings

#### AMF (Additive Manufacturing Format)  
- **Multi-Material Support**: Multiple material definitions per object
- **Color Specification**: RGB values or material properties
- **Legacy Support**: Older multi-color workflow compatibility

#### STL Fallback
- **Single Color**: Traditional STL for single-material printing
- **Multi-Part**: Separate STL files per color region (legacy workflow)

### Slicer Integration Workflow

1. **Generate 3MF**: OpenSCAD  3MF with embedded color regions
2. **Import to Slicer**: PrusaSlicer/Bambu Studio recognizes color assignments
3. **Material Mapping**: Assign filament colors to match HTML palette
4. **Print Planning**: Automatic color change commands at layer boundaries
5. **Quality Validation**: Preview multi-color regions before printing

## Multi-Color Viewing and Validation

### Recommended Viewers for Multi-Color Files

#### PrusaSlicer (Recommended)
- **File Support**: 3MF, AMF, STL with multi-material projects
- **Color Preview**: Real-time multi-color visualization
- **Integration**: Direct integration with 3MF color regions
- **Installation**: Free from Prusa Research
- **Usage**: `PrusaSlicer.exe path/to/file.3mf`

#### Bambu Studio
- **File Support**: 3MF with excellent color region handling
- **Features**: Advanced multi-color preview and editing tools
- **Compatibility**: Optimized for multi-material printing workflows
- **Installation**: Free from Bambu Lab
- **Benefits**: Superior color assignment and preview capabilities

#### Microsoft 3D Viewer (Windows 10/11)
- **File Support**: 3MF native support with color regions
- **Integration**: Built into Windows, no installation required
- **Usage**: Right-click 3MF file  "Open with"  "3D Viewer"
- **Limitations**: View-only, no editing capabilities

#### FreeCAD with Multi-Color Plugin
- **File Support**: 3MF, AMF with color import capabilities
- **Open Source**: Free alternative for multi-color viewing
- **Customization**: Extensible with Python plugins for color handling
- **Installation**: Download from FreeCAD.org

### Enhanced STL Viewer (Current System)

#### Current Limitations
The existing `tools/stl-viewer.html` supports only single-color STL files:
- **Format**: STL only (no color information)
- **Rendering**: WebGL with uniform material properties
- **Interaction**: Basic rotate/pan/zoom controls

#### Recommended Enhancements
1. **3MF Support**: Upgrade to Three.js 3MF loader for color regions
2. **AMF Support**: Add AMF parsing with material definitions
3. **Color Controls**: UI for toggling color regions on/off
4. **Material Preview**: Show different material properties per region

### PowerShell Viewer Integration

#### Enhanced launch-stl-viewer.ps1
File: `tools/launch-stl-viewer.ps1` (Enhanced)

```powershell
# Enhanced viewer launcher with multi-color support
param(
    [string]$File = "",
    [switch]$UseSystemViewer,
    [string]$PreferredViewer = "PrusaSlicer",
    [switch]$Help
)

function Launch-MultiColorViewer($FilePath, $ViewerType) {
    $Extension = [System.IO.Path]::GetExtension($FilePath).ToLower()
    
    switch ($ViewerType) {
        "PrusaSlicer" {
            if (Get-Command "PrusaSlicer.exe" -ErrorAction SilentlyContinue) {
                Start-Process "PrusaSlicer.exe" -ArgumentList $FilePath
                return $true
            }
        }
        "BambuStudio" {
            if (Get-Command "BambuStudio.exe" -ErrorAction SilentlyContinue) {
                Start-Process "BambuStudio.exe" -ArgumentList $FilePath
                return $true
            }
        }
        "3DViewer" {
            if ($Extension -eq ".3mf") {
                Start-Process "ms-3dviewer:" -ArgumentList $FilePath
                return $true
            }
        }
    }
    return $false
}

# Detect file type and launch appropriate viewer
if ($File) {
    $Extension = [System.IO.Path]::GetExtension($File).ToLower()
    
    if ($Extension -eq ".3mf" -or $Extension -eq ".amf") {
        Write-Host "Multi-color file detected: $Extension" -ForegroundColor Green
        
        if (-not (Launch-MultiColorViewer $File $PreferredViewer)) {
            Write-Host "Preferred viewer not found, trying alternatives..." -ForegroundColor Yellow
            
            # Try alternative viewers
            if (-not (Launch-MultiColorViewer $File "PrusaSlicer") -and
                -not (Launch-MultiColorViewer $File "BambuStudio") -and  
                -not (Launch-MultiColorViewer $File "3DViewer")) {
                Write-Host "No suitable multi-color viewer found." -ForegroundColor Red
                Write-Host "Please install PrusaSlicer or Bambu Studio for best results." -ForegroundColor Yellow
            }
        }
    } else {
        # Fall back to web viewer for STL files
        Write-Host "Single-color file, using web viewer..." -ForegroundColor Yellow
        # ... existing web viewer code ...
    }
}
```

### File Format Validation

#### 3MF Color Region Validation
```powershell
function Test-3MFColorRegions($FilePath) {
    # Extract and validate 3MF color assignments
    $Zip = [System.IO.Compression.ZipFile]::OpenRead($FilePath)
    $ModelEntry = $Zip.Entries | Where-Object { $_.Name -eq "3D/3dmodel.model" }
    
    if ($ModelEntry) {
        $ModelXml = [xml](New-Object System.IO.StreamReader($ModelEntry.Open())).ReadToEnd()
        $ColorGroups = $ModelXml.model.resources.colorgroup
        
        Write-Host "Color regions found: $($ColorGroups.count)" -ForegroundColor Green
        foreach ($group in $ColorGroups) {
            Write-Host "  Color: $($group.color) - Type: $($group.id)" -ForegroundColor Cyan
        }
    }
    $Zip.Dispose()
}
```

## Integration with Bot System

### Component Shape References
Each positioned component can reference generated shapes:

```json  
{
  "component_id": "goblin_eye_left",
  "hardware_reference": "config/components/hardware/gc9a01_display.json",
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