# P32 Head Subsystem - Design & Implementation Instructions
**Specific Guidelines for Animatronic Head Development**

## Head Subsystem Overview

The head subsystem is the primary interactive interface for P32 animatronic creatures, providing:
- **Visual Expression**: Eye displays with mood-driven animations
- **Vocal Expression**: Mouth display with speech/expression synchronization  
- **Environmental Sensing**: Proximity detection and spatial awareness
- **Audio Output**: Directional sound with creature-specific vocalizations
- **Creature Identity**: Distinctive silhouette and feature positioning

## Universal Head Architecture Implementation

### 1. Skull Framework Requirements

**Primary Responsibility:** Mounting structure + creature silhouette
- Generate skull using `assets/shapes/scad/skull_generators/{creature}_skull.scad`
- Provide standardized P32 mounting points at exact coordinates
- Maintain creature-specific proportions and anatomical features
- Include interior hollow space for electronics routing
- Design for PLA 3D printing with minimal supports

**Standard Mounting Interface:**
```
Eyes: 26mm OD mounting rings at P32 eye coordinates
Nose: 22x17mm rectangular bracket at P32 nose coordinate  
Mouth: 26mm OD mounting ring at P32 mouth coordinate
Ears: Creature-specific mounting points for audio components
```

### 2. Component Module System

**Eye Components:**
- **Electronics**: 2x GC9A01 240x240 circular displays
- **Shell Files**: `assets/shapes/scad/bot_shells/{creature}_eye_shells.scad`
- **Functionality**: Mood-driven eye animations, pupil dilation, color changes
- **Mounting**: Fits standard 26mm mounting rings in skull

**Nose Component:**  
- **Electronics**: HC-SR04 ultrasonic distance sensor
- **Shell Files**: `assets/shapes/scad/bot_shells/{creature}_nose_shell.scad`
- **Functionality**: Proximity detection, obstacle avoidance, interaction triggering
- **Mounting**: 22x17mm bracket with sensor clearance

**Mouth Component:**
- **Electronics**: 1x GC9A01 240x240 circular display  
- **Shell Files**: `assets/shapes/scad/bot_shells/{creature}_mouth_shell.scad`
- **Functionality**: Speech visualization, expression display, breathing animations
- **Mounting**: Fits standard 26mm mounting ring in skull

**Ear Components (Optional):**
- **Electronics**: Speaker modules for audio output
- **Shell Files**: `assets/shapes/scad/bot_shells/{creature}_ear_shells.scad`  
- **Functionality**: Directional audio, creature vocalizations
- **Mounting**: Creature-specific attachment points

### 3. P32 Coordinate Integration

All head designs must implement the P32 coordinate system:
```
Reference Point: Nose center (0, 0, 0)
Left Eye: (-1.05 INCH, +0.7 INCH, -0.35 INCH)  
Right Eye: (+1.05 INCH, +0.7 INCH, -0.35 INCH)
Nose Sensor: (0 INCH, 0 INCH, +0.25 INCH)
Mouth Display: (0 INCH, -1.05 INCH, 0 INCH)
```

Scale proportionally for different creature sizes while maintaining eye spacing ratios.

### 4. Electronics Integration

**Controller**: ESP32-S3 master controller per head
**Wiring**: Follow `docs/goblin-full-interconnection-diagram.md` pattern
**Communication**: 
- SPI bus shared between all three displays (eyes + mouth)
- Individual chip select pins per display
- I2C for sensor communication
- GPIO for additional creature-specific features

### 5. File Organization Standards

**Configuration Files:**
```
config/bots/{creature}_full.json - Complete head definition
config/components/positioned/{creature}_eye_left.json
config/components/positioned/{creature}_eye_right.json  
config/components/positioned/{creature}_nose.json
config/components/positioned/{creature}_mouth.json
```

**3D Design Files:**
```
assets/shapes/scad/skull_generators/{creature}_skull.scad - Main skull framework
assets/shapes/scad/bot_shells/{creature}_eye_shells.scad - Eye appearance  
assets/shapes/scad/bot_shells/{creature}_nose_shell.scad - Nose housing
assets/shapes/scad/bot_shells/{creature}_mouth_shell.scad - Mouth expressions
assets/shapes/scad/bot_shells/{creature}_ear_shells.scad - Audio housings
```

**Generated STL Files:**
```
assets/shapes/stl/{creature}_skull_complete.stl - Ready to print skull
assets/shapes/stl/{creature}_components/ - All component shells
```

## Creature-Specific Variations

### Goblin Head Characteristics
- Elongated cranium with prominent brow ridge  
- Large eye sockets for menacing expressions
- Flared nostrils and aggressive jaw structure
- Long pointed ears with audio integration
- Forest green coloring (#3D5A2B)

### Cat Head Characteristics (Future)
- Rounded feline cranium with refined features
- Almond-shaped eye sockets for cat-like expressions  
- Small triangular nose with realistic texturing
- Triangular upright ears positioned high on skull
- Natural cat coloring variations

### Bear Head Characteristics (Future)  
- Broad, powerful cranium with heavy jaw
- Round eye sockets for gentle/fierce expressions
- Large black nose with textured surface
- Small rounded ears positioned high and back
- Brown/black coloring with fur texture options

## Development Workflow

### 1. Design Phase
1. Define creature proportions and anatomical features
2. Create skull generator with P32 coordinate compliance
3. Design component shells with creature-specific aesthetics  
4. Validate mounting interface compatibility

### 2. Implementation Phase  
1. Generate STL files using OpenSCAD automation
2. 3D print skull and component shells
3. Assemble electronics per wiring specifications
4. Test component fit and functionality

### 3. Integration Phase
1. Install components in skull mounting points  
2. Route wiring through skull interior
3. Configure software for creature-specific behaviors
4. Calibrate sensors and display positioning

### 4. Validation Phase
1. Verify all P32 coordinates are accurate
2. Test mood-driven animation system  
3. Validate sensor functionality and range
4. Confirm audio output and directionality

## Quality Standards

**Mechanical Requirements:**
- All mounting interfaces must be within 0.5mm tolerance
- Component shells must fit without forcing or gaps
- Skull must support component weight without deformation
- Cable routing must not interfere with component operation

**Electronic Requirements:**  
- All displays must achieve 60+ FPS animation rates
- Sensor must detect objects within 2-400cm range
- Audio output must be clear and directional  
- System must operate on 12V power supply

**Software Requirements:**
- Mood changes must trigger animation updates within 100ms
- Eye tracking must coordinate between left/right displays  
- Sensor data must integrate with behavioral engine
- Component failures must not crash entire system

---

**This specification ensures consistent head subsystem development while enabling unlimited creature variety through component customization.**