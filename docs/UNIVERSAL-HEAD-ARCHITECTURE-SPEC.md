# P32 Animatronic Head Architecture Specification
**Universal Design Requirements for All Creature Heads**

## Component-Based Facial Feature System

### Core Design Principle
**The skull provides the mounting framework - detailed features come from attached components.**

### Architecture Overview
```
SKULL (Base Structure) + COMPONENT MODULES = COMPLETE HEAD
    │                        │
    │                        ├── Eyes (GC9A01 + custom shells)
    │                        ├── Nose (HC-SR04 + shaped bracket)
    │                        ├── Mouth (GC9A01 + expression shell)
    │                        └── Ears (Audio + creature-specific shells)
    │
    └── Standardized mounting points per P32 coordinate system
```

### Universal Requirements for All Creatures

#### 1. Skull Responsibilities
- **Structural Framework:** Basic head shape defining creature silhouette
- **Mounting Points:** Standardized brackets at P32 coordinate positions
- **Interior Space:** Hollow cavity for electronics and wiring
- **Material Compatibility:** Designed for PLA 3D printing with 15% infill

#### 2. Component Module Responsibilities  
- **Functional Hardware:** Contains actual electronics (displays, sensors, speakers)
- **Aesthetic Details:** Creature-specific shapes (goblin nose vs cat nose vs bear nose)
- **Mounting Interface:** Compatible with skull mounting brackets
- **Interchangeability:** Same skull can accept different creature component sets

#### 3. Standardized Mounting System
All creature heads must implement:

**Eye Mounting Points:**
- Position: P32 coordinates (-1.05", +0.7", -0.35") and (+1.05", +0.7", -0.35")
- Interface: 26mm outer diameter mounting ring, 22mm inner clearance
- Depth: 4mm mounting thickness
- Hardware: GC9A01 240x240 circular display + creature-specific eye shell

**Nose Mounting Point:**
- Position: P32 coordinates (0", 0", +0.25") 
- Interface: 22x17mm rectangular bracket with 20x15mm sensor clearance
- Depth: 10mm mounting thickness  
- Hardware: HC-SR04 ultrasonic sensor + creature-specific nose shell

**Mouth Mounting Point:**
- Position: P32 coordinates (0", -1.05", 0")
- Interface: 26mm outer diameter mounting ring, 22mm inner clearance
- Depth: 4mm mounting thickness
- Hardware: GC9A01 240x240 circular display + creature-specific mouth shell

**Ear Mounting Points (Optional):**
- Position: Creature-dependent, typically ±3" from skull centerline
- Interface: Variable based on creature ear size/shape
- Hardware: Speaker + creature-specific ear shell

### Implementation Examples

#### Goblin Head Configuration
- **Skull:** Angular, elongated cranium with prominent brow ridge
- **Eye Components:** Large, menacing eye shells with glowing red capability
- **Nose Component:** Flared nostril shell housing distance sensor  
- **Mouth Component:** Wide, snarling mouth shell with fang details
- **Ear Components:** Long, pointed ear shells with integrated speakers

#### Cat Head Configuration (Future)
- **Skull:** Rounded feline cranium with refined cheekbone structure
- **Eye Components:** Almond-shaped cat eye shells with pupil dilation
- **Nose Component:** Small triangular nose shell with pink coloring
- **Mouth Component:** Compact mouth shell with whisker attachment points
- **Ear Components:** Triangular upright ear shells with fur texture

#### Bear Head Configuration (Future)  
- **Skull:** Broad, powerful cranium with heavy jaw structure
- **Eye Components:** Round bear eye shells with brown iris patterns
- **Nose Component:** Large black nose shell with realistic texture
- **Mouth Component:** Wide mouth shell capable of showing teeth/tongue
- **Ear Components:** Small rounded ear shells positioned high on skull

### Design Benefits

1. **Modularity:** Same electronics work across all creatures
2. **Customization:** Easy to create new creatures by changing component shells
3. **Maintenance:** Individual components can be reprinted/replaced without full head rebuild  
4. **Scalability:** System works for any creature size by scaling mounting dimensions
5. **Cost Efficiency:** Electronics investment protected across creature variations

### File Organization Standard
```
config/components/positioned/
├── {creature}_eye_left.json      # Left eye position + shell reference
├── {creature}_eye_right.json     # Right eye position + shell reference  
├── {creature}_nose.json          # Nose sensor position + shell reference
├── {creature}_mouth.json         # Mouth display position + shell reference
└── {creature}_ear_{left|right}.json  # Ear speaker positions + shell references

assets/shapes/scad/bot_shells/
├── {creature}_eye_shells.scad    # Eye appearance variations
├── {creature}_nose_shell.scad    # Nose appearance design
├── {creature}_mouth_shell.scad   # Mouth expression capabilities
└── {creature}_ear_shells.scad    # Ear shape and speaker housing
```

### Manufacturing Workflow
1. **Print skull framework** from creature-specific skull generator
2. **Print component shells** from creature-specific shell generators  
3. **Assemble electronics** into component shells per P32 wiring diagram
4. **Mount components** to skull using standardized mounting interface
5. **Test functionality** before final creature assembly

---

**This architecture ensures consistent manufacturing processes while enabling unlimited creature variety through component shell customization.**