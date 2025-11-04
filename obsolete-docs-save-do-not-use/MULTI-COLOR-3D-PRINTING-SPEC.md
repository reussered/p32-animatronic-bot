# Multi-Color 3D Printing Specification
## 4-Color HTML Named Color System for P32 Animatronic Bot

## Overview
The P32 system uses **HTML/CSS Named Colors** for multi-material 3D printing specifications. This allows STL/3MF files to designate color regions using semantic names (`Color 1`, `Color 2`, etc.) while the actual colors are specified using standard HTML color names.

## Color Naming System
All colors use **HTML/CSS Named Colors** as defined in the W3C CSS Color Module specification.

### Common HTML Named Colors
- **Whites**: `White`, `Snow`, `Ivory`, `Linen`, `AntiqueWhite`
- **Grays**: `Silver`, `LightGray`, `Gray`, `DarkGray`, `DimGray`
- **Blacks**: `Black`, `Charcoal`
- **Reds**: `Red`, `Crimson`, `DarkRed`, `FireBrick`, `IndianRed`, `LightCoral`
- **Greens**: `Green`, `LimeGreen`, `ForestGreen`, `DarkOliveGreen`, `SeaGreen`, `DarkGreen`
- **Blues**: `Blue`, `Navy`, `DodgerBlue`, `SteelBlue`, `LightSkyBlue`, `MidnightBlue`
- **Yellows**: `Yellow`, `Gold`, `Goldenrod`, `Khaki`, `LemonChiffon`
- **Oranges**: `Orange`, `OrangeRed`, `DarkOrange`, `Coral`, `Tomato`
- **Browns**: `SaddleBrown`, `Sienna`, `Chocolate`, `Peru`, `BurlyWood`, `Tan`
- **Purples**: `Purple`, `DarkViolet`, `Indigo`, `MediumPurple`, `Orchid`

**Complete Reference**: https://www.w3.org/wiki/CSS/Properties/color/keywords

## 4-Color Palette Structure

### Semantic Color Roles
Each bot family defines a 4-color palette with semantic roles:

```json
"color_palette": {
  "primary": "DarkOliveGreen",    // Color 1: Main body/shell structure
  "secondary": "SaddleBrown",     // Color 2: Secondary structure/details  
  "accent": "Goldenrod",          // Color 3: Decorative features/trim
  "highlight": "OrangeRed"        // Color 4: Eye bezels/special features
}
```

### Construction Type Variations
Different construction types use different palettes:

#### Hard Shell (3D Printed)
```json
"hard_shell": {
  "material": "3d_printed_abs",
  "color_palette": {
    "primary": "DarkOliveGreen",    // Main structural shell
    "secondary": "SaddleBrown",     // Mounting brackets/internals
    "accent": "Goldenrod",          // Surface details/textures
    "highlight": "OrangeRed"        // LED bezels/eye rings
  }
}
```

#### Soft Body (Fabric)
```json
"soft_body": {
  "material": "plush_fabric",
  "color_palette": {
    "primary": "ForestGreen",       // Main fabric covering
    "secondary": "BurlyWood",       // Contrast panels/belly
    "accent": "Khaki",              // Embroidered details
    "highlight": "Crimson"          // Eye highlights/features
  }
}
```

## OpenSCAD Integration

### Color Region Definition
OpenSCAD files use the `color()` module with HTML named colors:

```scad
// Goblin nose shell with 4-color regions
module goblin_nose_shell() {
    // Color 1 (Primary): Main nose structure
    color("DarkOliveGreen") 
        difference() {
            goblin_nose_base();
            sensor_cutout();
        }
    
    // Color 2 (Secondary): Sensor mounting bracket
    color("SaddleBrown")
        sensor_mounting_ring();
    
    // Color 3 (Accent): Warty surface details
    color("Goldenrod")
        for(i = [0:wart_count-1]) {
            translate([wart_positions[i]]) wart_detail();
        }
    
    // Color 4 (Highlight): Nostril edges
    color("OrangeRed")
        for(side = [-1, 1]) {
            translate([side * spacing, 0, 0]) nostril_ring();
        }
}
```

### Parameterized Color Variables
For flexibility, use OpenSCAD parameters:

```scad
// Define at top of file
PRIMARY_COLOR = "DarkOliveGreen";
SECONDARY_COLOR = "SaddleBrown";
ACCENT_COLOR = "Goldenrod";
HIGHLIGHT_COLOR = "OrangeRed";

// Use in modules
module component() {
    color(PRIMARY_COLOR) main_body();
    color(SECONDARY_COLOR) bracket();
    color(ACCENT_COLOR) details();
    color(HIGHLIGHT_COLOR) features();
}
```

### Command Line Color Override
Generate with custom colors via command line:

```powershell
openscad -o goblin_nose.3mf `
  -D "PRIMARY_COLOR=\"DarkSeaGreen\"" `
  -D "SECONDARY_COLOR=\"Chocolate\"" `
  -D "ACCENT_COLOR=\"Gold\"" `
  -D "HIGHLIGHT_COLOR=\"Crimson\"" `
  goblin_nose_shell.scad
```

## File Format Support

### STL (Single Color - Current)
- **Format**: Binary or ASCII STL
- **Color Support**: None (monochrome)
- **Use**: Single-color prints, post-painting required
- **Generation**: `openscad -o output.stl input.scad`

### 3MF (Multi-Color - Future)
- **Format**: 3D Manufacturing Format
- **Color Support**: Per-face color assignment with HTML colors
- **Use**: Multi-material printers (Prusa MMU, Bambu AMS)
- **Generation**: `openscad -o output.3mf input.scad`
- **Slicer Support**: PrusaSlicer, Bambu Studio, Cura

### AMF (Alternative Multi-Color)
- **Format**: Additive Manufacturing Format
- **Color Support**: Per-volume color with RGB/named colors
- **Use**: Legacy multi-material systems
- **Generation**: `openscad -o output.amf input.scad`

## Multi-Material Printer Compatibility

### Prusa i3 MK3S+ with MMU2S/MMU3
- **Max Colors**: 5 (but use 4 for standardization)
- **Slicer**: PrusaSlicer
- **Workflow**: 3MF → Import → Assign filaments → Slice
- **Color Mapping**: Automatic from 3MF embedded colors

### Bambu Lab X1 Carbon with AMS
- **Max Colors**: 4 (perfect match!)
- **Slicer**: Bambu Studio
- **Workflow**: 3MF → Import → Auto-assign colors → Print
- **Color Matching**: Bambu filament color library

### Mosaic Palette 3/3 Pro
- **Max Colors**: 4 or 8
- **Slicer**: Canvas or PrusaSlicer with plugin
- **Workflow**: STL → Canvas assigns colors → Export G-code
- **Manual Mapping**: Map HTML colors to loaded filaments

## Bot Family Color Palettes

### Fantasy Family - Goblin
```json
"hard_shell": {
  "primary": "DarkOliveGreen",      // Goblin skin
  "secondary": "SaddleBrown",       // Leather/dirt
  "accent": "Goldenrod",            // Treasure/trinkets
  "highlight": "OrangeRed"          // Eyes/fire
}
```

### Animals Family - Cat
```json
"hard_shell": {
  "primary": "BurlyWood",           // Tabby fur base
  "secondary": "Sienna",            // Darker stripes
  "accent": "Pink",                 // Nose/paws
  "highlight": "Gold"               // Eye glow
}
```

### Horror Family - Zombie
```json
"hard_shell": {
  "primary": "DarkSeaGreen",        // Decayed skin
  "secondary": "DimGray",           // Exposed bone
  "accent": "DarkRed",              // Blood/gore
  "highlight": "PaleGreen"          // Decay highlights
}
```

### Mechanical Family - Robot
```json
"hard_shell": {
  "primary": "Silver",              // Metal chassis
  "secondary": "Navy",              // Accent panels
  "accent": "Gold",                 // Mechanical details
  "highlight": "Cyan"               // LED accents
}
```

## Generation Workflow

### Step 1: Design with Color Regions
```scad
// goblin_eye_shell.scad
module goblin_eye_shell() {
    color("DarkOliveGreen") eye_socket();
    color("SaddleBrown") display_mount();
    color("Goldenrod") craggy_details();
    color("OrangeRed") led_bezel();
}
```

### Step 2: Generate Multi-Color File
```powershell
# Using generate-stl-files.ps1
.\tools\generate-stl-files.ps1 `
  -BotShells `
  -MultiColor `
  -Family "fantasy\goblin_family" `
  -ConstructionType "hard_shell"
```

### Step 3: Slice with Color Assignment
1. **Open 3MF** in PrusaSlicer/Bambu Studio
2. **View Color Regions** - See 4 distinct colors
3. **Assign Filaments**:
   - Color 1 → Extruder 1: "Prusament Galaxy Black" (DarkOliveGreen substitute)
   - Color 2 → Extruder 2: "Prusament Mud Brown" (SaddleBrown)
   - Color 3 → Extruder 3: "Prusament Golden Yellow" (Goldenrod)
   - Color 4 → Extruder 4: "Prusament Orange Red" (OrangeRed)
4. **Preview & Slice**
5. **Print!**

## Color Substitution Guidelines

### When Exact HTML Color Not Available
Use closest match from filament inventory:

| HTML Color | Hatchbox PLA | Prusament | eSUN PLA+ |
|------------|--------------|-----------|-----------|
| DarkOliveGreen | True Green + Brown | Pesto Blend | Olive Green |
| SaddleBrown | Brown | Chocolate Mud | Coffee |
| Goldenrod | Gold | Prusa Gold | Gold |
| OrangeRed | Orange | Orange Red | Coral Red |
| ForestGreen | Forest Green | Opal Green | Dark Green |
| Silver | Gray | Silver Metallic | Space Gray |

### Artistic License
Bot builders can substitute ANY colors they want:
- Rainbow goblins: `Rainbow`, `HotPink`, `Cyan`, `Yellow`
- Stealth ops: `Black`, `DarkGray`, `Gray`, `Silver`
- Neon party: `LimeGreen`, `Magenta`, `Yellow`, `Cyan`

**The system just needs 4 distinct colors specified using HTML names!**

## PowerShell Script Integration

### Current Implementation
The `generate-stl-files.ps1` script already supports color palettes:

```powershell
# Load family configuration
$FamilyConfig = Get-Content $FamilyPath | ConvertFrom-Json
$ColorPalette = $FamilyConfig.construction_types.$ConstructionType.color_palette

# Pass to OpenSCAD
& openscad -o $OutputPath `
  -D "PRIMARY_COLOR=`"$($ColorPalette.primary)`"" `
  -D "SECONDARY_COLOR=`"$($ColorPalette.secondary)`"" `
  -D "ACCENT_COLOR=`"$($ColorPalette.accent)`"" `
  -D "HIGHLIGHT_COLOR=`"$($ColorPalette.highlight)`"" `
  $ScadPath
```

### Future Enhancement: Color Name Validation
```powershell
# Validate against W3C HTML color names
$ValidColors = @(
    "White", "Silver", "Gray", "Black", "Red", "Maroon", "Yellow", "Olive",
    "Lime", "Green", "Aqua", "Teal", "Blue", "Navy", "Fuchsia", "Purple",
    "Orange", "Crimson", "Gold", "Goldenrod", "DarkOliveGreen", "ForestGreen",
    # ... (147 total HTML named colors)
)

if ($ColorPalette.primary -notin $ValidColors) {
    Write-Warning "Invalid color name: $($ColorPalette.primary)"
}
```

## Testing Strategy

### Visual Verification
1. **OpenSCAD Preview**: View colors in OpenSCAD GUI before export
2. **3MF Import**: Verify color regions in slicer
3. **Test Print**: Single-layer test print to verify color changes
4. **Full Print**: Complete component with all 4 colors

### Color Region Validation
```scad
// Add to SCAD files for testing
module test_color_regions() {
    // Arrange all 4 colors in a grid
    translate([0, 0, 0]) color("DarkOliveGreen") cube([10, 10, 2]);
    translate([12, 0, 0]) color("SaddleBrown") cube([10, 10, 2]);
    translate([0, 12, 0]) color("Goldenrod") cube([10, 10, 2]);
    translate([12, 12, 0]) color("OrangeRed") cube([10, 10, 2]);
}

// Uncomment for testing
// test_color_regions();
```

## Documentation Standards

### Family Configuration Files
Every bot family MUST include both construction types with color palettes:

```json
"construction_types": {
  "hard_shell": {
    "color_palette": {
      "primary": "HTMLColorName1",
      "secondary": "HTMLColorName2", 
      "accent": "HTMLColorName3",
      "highlight": "HTMLColorName4"
    }
  },
  "soft_body": {
    "color_palette": {
      "primary": "HTMLColorName1",
      "secondary": "HTMLColorName2",
      "accent": "HTMLColorName3", 
      "highlight": "HTMLColorName4"
    }
  }
}
```

### OpenSCAD Files
All shape generation SCAD files MUST use parameterized colors:

```scad
// At top of file
PRIMARY_COLOR = "DarkOliveGreen";
SECONDARY_COLOR = "SaddleBrown";
ACCENT_COLOR = "Goldenrod";
HIGHLIGHT_COLOR = "OrangeRed";

// In all color() calls, use variables:
color(PRIMARY_COLOR) main_structure();
color(SECONDARY_COLOR) brackets();
color(ACCENT_COLOR) details();
color(HIGHLIGHT_COLOR) features();
```

## Future Enhancements

### Phase 1 (Current): Single-Color STL
- ✅ HTML named color specifications in JSON
- ✅ Color-aware OpenSCAD generation
- ✅ STL output (no embedded colors)
- ⏳ Manual painting using color guides

### Phase 2 (Next): Multi-Color 3MF
- ⏳ 3MF output format support
- ⏳ Embedded HTML color assignments
- ⏳ Slicer import testing
- ⏳ Multi-material print validation

### Phase 3 (Future): Advanced Features
- ⏳ Gradient transitions between colors
- ⏳ Texture mapping support
- ⏳ Metallic/pearlescent effects
- ⏳ Glow-in-the-dark accent layers

## References

- **HTML Color Names**: https://www.w3.org/wiki/CSS/Properties/color/keywords
- **3MF Specification**: https://3mf.io/specification/
- **OpenSCAD Color Module**: https://en.wikibooks.org/wiki/OpenSCAD_User_Manual/Transformations#color
- **PrusaSlicer Multi-Material**: https://help.prusa3d.com/category/multi-material-upgrade_131
- **Bambu Lab AMS**: https://wiki.bambulab.com/en/x1/manual/ams-guide

---

**Last Updated**: October 15, 2025  
**Status**: Active Specification  
**Version**: 1.0.0
