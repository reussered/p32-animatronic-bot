# Two-Tier Mounting System Architecture

## Overview

The P32 Animatronic Bot uses a **two-tier mounting system** that separates generic hardware mounting from character-specific aesthetics. This modular approach enables hardware reuse across different bot characters while maintaining unique visual personalities.

## Architecture Layers

### Tier 1: Basic Hardware Mounts
**Purpose**: Generic, reusable mounting brackets for standard hardware components
**Location**: `assets/shapes/scad/basic_mounts/`

#### Components:
- **`display_basic_mount.scad`** - Generic GC9A01 1.28" circular display mounting
- **`sensor_basic_mount.scad`** - Generic HC-SR04 ultrasonic sensor mounting  
- **`speaker_basic_mount.scad`** - Generic 40mm round speaker mounting

#### Characteristics:
- **Hardware-Specific**: Precise fit for component dimensions
- **Utilitarian Design**: Focus on function over form
- **Standard Mounting**: M3 screws, standard spacing
- **Cable Management**: Built-in wire routing channels
- **Reinforcement**: Structural tabs for case mounting

### Tier 2: Bot-Specific Decorative Shells
**Purpose**: Character-themed outer shells that incorporate and conceal basic mounts
**Location**: `assets/shapes/scad/bot_shells/`

#### Goblin Character Components:
- **`goblin_eye_shells.scad`** - Craggy eye sockets with brow ridges
- **`goblin_nose_shell.scad`** - Warty protrusion with nostril details
- **`goblin_mouth_shell.scad`** - Fanged maw with lip ridges

#### Characteristics:
- **Character-Specific**: Unique aesthetic for each bot personality
- **Integrated Design**: Shells incorporate basic mounts as `use <../basic_mounts/mount.scad>`
- **Surface Details**: Warts, crags, fangs, and texture elements
- **Anatomical Features**: Character-appropriate features (nostrils, brows, etc.)

## Design Philosophy

### Separation of Concerns
- **Hardware Interface**  **Basic Mounts** (Tier 1)
- **Character Aesthetics**  **Decorative Shells** (Tier 2)
- **Spatial Positioning**  **Bot Configuration JSON**

### Modularity Benefits
- **Hardware Reuse**: Same basic mounts work across all bot types
- **Character Swapping**: Change shells to transform robot personality
- **Maintenance**: Replace hardware mounts without affecting character aesthetics
- **Development**: Design character shells independently from hardware integration

### Manufacturing Workflow
1. **Print Basic Mounts** (Tier 1) - Higher precision, durable material
2. **Print Decorative Shells** (Tier 2) - Character material/color
3. **Hardware Assembly** - Mount components to basic brackets
4. **Aesthetic Assembly** - Install decorated hardware into shells
5. **System Integration** - Mount completed assemblies to robot frame

## File Structure

```
assets/shapes/scad/
 basic_mounts/           # Tier 1: Generic hardware mounts
    display_basic_mount.scad
    sensor_basic_mount.scad
    speaker_basic_mount.scad
 bot_shells/             # Tier 2: Character-specific shells
    goblin_eye_shells.scad
    goblin_nose_shell.scad
    goblin_mouth_shell.scad
    bear_eye_shells.scad      # Future characters
    cat_nose_shell.scad
    cyclops_eye_shell.scad
 templates/              # Original template system (legacy)
     display_mount.scad
     sensor_bracket.scad
     speaker_enclosure.scad
```

## Hardware Specifications

### GC9A01 Display Mount
- **Display Diameter**: 32.512mm (1.28")
- **Mount Thickness**: 2.032mm (0.08") 
- **Mounting Holes**: 4x 2.54mm (0.1") diameter
- **Hole Spacing**: 27.94mm (1.1")
- **Case Mounting**: 4x M3 screws in reinforcement tabs

### HC-SR04 Sensor Mount  
- **Sensor Dimensions**: 45mm x 20mm x 15mm
- **Transducer Diameter**: 16mm
- **Transducer Spacing**: 24mm
- **PCB Mounting**: 4x 2.5mm holes
- **Case Mounting**: 2x M3 screws through flanges

### 40mm Speaker Mount
- **Speaker Diameter**: 40mm
- **Mount Depth**: 20mm
- **Mounting Holes**: 4x 3mm diameter  
- **Hole Spacing**: 35mm
- **Case Mounting**: 6x M3 screws (60 spacing)

## Character Variations

### Goblin Configuration
- **Eye Style**: Craggy sockets, asymmetric brow ridges
- **Nose Style**: Warty protrusion, prominent nostrils
- **Mouth Style**: Fanged maw, irregular lip ridges  
- **Surface Texture**: Organic warts and crags
- **Color Scheme**: Dark green tones

### Future Character Designs
- **Bear**: Rounded sockets, gentle curves, fur texture
- **Cat**: Almond eyes, triangular nose, whisker frames
- **Cyclops**: Massive central eye, minimal features, scarred texture

## Generation Tools

### `tools/generate-mounting-system.ps1`
- Generates both tier systems
- Configurable bot types
- Automated file organization
- OpenSCAD preview integration

### `tools/generate-stl-files.ps1`  
- Converts .scad files to .stl files
- Batch processing for all components
- File size reporting
- Error handling and validation

## Usage Examples

```powershell
# Generate complete goblin mounting system
.\tools\generate-mounting-system.ps1 -BotType goblin

# Generate STL files for 3D printing
.\tools\generate-stl-files.ps1 -BasicMounts -BotShells

# Generate only basic mounts (reusable across bots)
.\tools\generate-mounting-system.ps1 -GenerateBasicMounts -GenerateBotShells:$false
```

## Integration with ESP32 System

The two-tier mounting system integrates with the existing P32 configuration architecture:

- **Hardware Components**: `config/components/hardware/` define component specs
- **Interface Definitions**: `config/components/interfaces/` define GPIO assignments  
- **Positioned Components**: `config/components/positioned/` define spatial coordinates
- **Bot Definitions**: `config/bots/` reference positioned components
- **Generated Mounts**: Use positioning data for automatic mount generation

This creates a complete pipeline from JSON configuration  3D printable mounts  assembled hardware  functional animatronic system.