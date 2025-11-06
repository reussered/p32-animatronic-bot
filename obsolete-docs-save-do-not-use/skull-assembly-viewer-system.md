# P32 Skull Assembly Viewer System
## Complete Hardware-Aware 3D Visualization Platform

### System Overview

We've created a comprehensive system for visualizing and generating animatronic skull assemblies that properly accounts for component placement and mounting requirements. The system includes:

#### **1. Interactive Web-Based Viewer** 
- **File**: `tools/skull-assembly-viewer.html`
- **Features**: 3D visualization with component overlay, creature switching, measurement display
- **Launcher**: `tools/launch-skull-viewer.ps1`

#### **2. Component-Aware Design System**
- **Integration File**: `assets/shapes/scad/integrated_skull_assembly_system.scad`
- **Mounting Specs**: `assets/shapes/descriptions/component_mounting_specifications.md`
- **Reference Database**: `assets/shapes/descriptions/creature_skull_reference_database.md`

#### **3. STL Generation Pipeline**
- **Simple Generator**: `tools/generate-skull-stls.ps1` (basic STL files)
- **Advanced Generator**: `tools/generate-stl-files.ps1 -SkullAssemblies` (multi-color 3MF)
- **Test Assembly**: `assets/shapes/scad/test_skull_assembly.scad`

### Hardware Component Integration

The system properly accounts for **all hardware components** in the skull assemblies:

#### **Display Components (3x GC9A01 1.28" Circular)**
- **Left Eye**: (-26.67mm, 17.78mm, -8.89mm) from nose center
- **Right Eye**: (+26.67mm, 17.78mm, -8.89mm) from nose center  
- **Mouth**: (0mm, -26.67mm, 0mm) from nose center
- **Mounting**: 32.5mm diameter cutouts, M2.5 screws, reinforcement rings

#### **Sensor Components (1x HC-SR04 Ultrasonic)**
- **Nose Position**: (0mm, 0mm, +6.35mm) from nose center
- **Mounting**: 45x20mm bracket, forward-facing orientation
- **Detection**: Proximity sensing for behavior triggers

#### **Audio Components (2x I2S Speakers)**
- **Left Ear**: (-38.1mm, 19.05mm, 12.7mm) from nose center
- **Right Ear**: (+38.1mm, 19.05mm, 12.7mm) from nose center
- **Mounting**: Side-facing orientation, acoustic chamber design

#### **Cable Management**
- **Main Harness**: 8mm channel from nose to skull base
- **Branch Channels**: 6mm channels to each component
- **Service Access**: Removable rear panel for maintenance

### Creature Type Support

The system supports **8 creature types** with anatomically accurate scaling:

1. **Goblin** - Base design (53.34mm eye spacing, 130150140mm)
2. **Zombie** - Decay effects and larger features  
3. **Android** - Geometric elements and panel lines
4. **Cat** - Feline proportions (45mm eye spacing, elongated snout)
5. **Lion** - Large cat scaling (74.68mm eye spacing) 
6. **Dragon Fire** - Angular features, extended snout
7. **Dragon Ice** - Smooth lines, crystalline elements
8. **Bear** - Massive build (69.34mm eye spacing, robust construction)

### Usage Instructions

#### **Launch Interactive Viewer**
```powershell
# View goblin assembly
.\tools\launch-skull-viewer.ps1

# View specific creature type
.\tools\launch-skull-viewer.ps1 -CreatureType dragon_fire

# Auto-generate STL files then view
.\tools\launch-skull-viewer.ps1 -AutoGenerate
```

#### **Generate STL Files**
```powershell
# Generate all creature STL files (requires OpenSCAD)
.\tools\generate-skull-stls.ps1

# Generate specific creatures only
.\tools\generate-skull-stls.ps1 -CreatureTypes goblin,zombie,cat

# Generate with multi-color 3MF support
.\tools\generate-stl-files.ps1 -SkullAssemblies -MultiColor
```

#### **View Generated Files**
```powershell
# Launch STL viewer for specific file
.\tools\launch-stl-viewer.ps1 "assets\shapes\stl\skull_assemblies\goblin_skull_assembly.stl"

# View all generated assemblies
Get-ChildItem "assets\shapes\stl\skull_assemblies\*.stl"
```

### File Outputs

#### **STL Files** (Standard 3D Printing)
- **Location**: `assets/shapes/stl/skull_assemblies/`
- **Format**: Standard STL meshes for any 3D printer
- **Usage**: Prototyping, single-color printing, viewer compatibility

#### **3MF Files** (Multi-Color Printing)  
- **Location**: `assets/shapes/stl/skull_assemblies/`
- **Format**: Multi-color 3MF with embedded color data
- **Usage**: Advanced printers (Prusa, Bambu), automated color changes

### 3D Printing Specifications

#### **Print Settings**
- **Layer Height**: 0.2-0.3mm for structural parts
- **Infill**: 15-20% for lightweight but strong construction  
- **Wall Thickness**: 2.5mm minimum for component mounting integrity
- **Supports**: Minimal due to optimized cavity design

#### **Hardware Requirements**
- **Screws**: M2.5 x 8mm (16 total for displays), M3 x 12mm (sensors/speakers)
- **Washers**: M2.5 and M3 flat washers for load distribution
- **Spacers**: 2mm nylon spacers to prevent over-tightening
- **Threaded Inserts**: M2.5 and M3 heat-set inserts for removable assembly

### Component Validation

Each skull assembly includes **built-in validation** for:
-  Component clearance and fit
-  Cable routing without interference  
-  Mounting hole alignment
-  Acoustic chamber sealing
-  Service access for maintenance
-  Structural integrity at stress points

### Quality Control Checklist

Before finalizing any skull assembly:
- [ ] All displays properly seated and secured
- [ ] Sensor alignment verified with test measurements  
- [ ] Speakers properly sealed with no air leaks
- [ ] All cables routed without stress or pinching
- [ ] No interference between components
- [ ] Skull aesthetics maintained with hidden mounting hardware

### Future Expansion

The system is designed for **easy expansion**:
- **New Creature Types**: Add to creature database with scaling factors
- **Additional Components**: Camera, IMU, servo mounts, LED strips
- **Advanced Features**: Jaw movement, eye tracking, dynamic expressions
- **Manufacturing**: Injection molding tooling, mass production optimization

### Current Status

 **Interactive viewer system complete and functional**  
 **Component integration specifications complete**  
 **Anatomical reference database complete**  
 **STL/3MF generation pipeline ready**  
 **Requires OpenSCAD installation for actual file generation**  
 **Ready for hardware testing and validation**

The skull assembly viewer system provides a complete workflow from design visualization to 3D printing, ensuring that every generated skull properly accommodates all required hardware components while maintaining anatomical accuracy and aesthetic appeal.