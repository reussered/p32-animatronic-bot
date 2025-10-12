# Component Mounting Specifications
## Hardware Integration Requirements for Skull Assemblies

### Component Placement Analysis
Based on the positioned component JSON configurations, each skull assembly must accommodate:

#### **Display Components (GC9A01 - 1.28" Circular)**

**Left Eye Position:**
- Coordinates: x=-1.05", y=+0.7", z=-0.35" (from nose center)
- Millimeter conversion: x=-26.67mm, y=+17.78mm, z=-8.89mm
- Mounting requirements: 
  - 32.5mm diameter cutout for display
  - 4x M2.5 mounting holes on 27.94mm pitch circle
  - 6mm depth clearance for PCB and connectors
  - Cable routing channel to main harness

**Right Eye Position:**
- Coordinates: x=+1.05", y=+0.7", z=-0.35" (from nose center)
- Millimeter conversion: x=+26.67mm, y=+17.78mm, z=-8.89mm
- Same mounting requirements as left eye

**Mouth Position:**
- Coordinates: x=0", y=-1.05", z=0" (from nose center)
- Millimeter conversion: x=0mm, y=-26.67mm, z=0mm
- Same mounting requirements as eyes

#### **Sensor Components (HC-SR04 Ultrasonic)**

**Nose Sensor Position:**
- Coordinates: x=0", y=0", z=+0.25" (from nose center)
- Millimeter conversion: x=0mm, y=0mm, z=+6.35mm
- Mounting requirements:
  - 45mm x 20mm rectangular bracket mounting area
  - 2x 16mm diameter holes for transducers (24mm spacing)
  - Forward-facing orientation for proximity detection
  - Cable routing to GPIO pins

#### **Audio Components (I2S Speakers)**

**Left Ear Position:**
- Coordinates: x=-1.5", y=+0.75", z=+0.5" (from nose center)
- Millimeter conversion: x=-38.1mm, y=+19.05mm, z=+12.7mm
- Mounting requirements:
  - Side-facing orientation (90° rotation)
  - Speaker driver mounting (typically 20-40mm diameter)
  - Acoustic chamber design for sound projection

**Right Ear Position:**
- Coordinates: x=+1.5", y=+0.75", z=+0.5" (from nose center)
- Millimeter conversion: x=+38.1mm, y=+19.05mm, z=+12.7mm
- Same requirements as left ear (mirrored orientation)

### Skull Assembly Requirements

#### **Structural Integrity**
- **Wall Thickness**: Minimum 2.5mm for 3D printing strength
- **Reinforcement Zones**: Extra material around all mounting points
- **Mounting Rings**: 4mm thick reinforcement rings around display cutouts
- **Support Structures**: Internal ribs connecting high-stress areas

#### **Cable Management**
- **Main Harness Channel**: 8mm diameter channel from nose to skull base
- **Branch Channels**: 6mm diameter channels to each component
- **Exit Point**: Single cable exit at skull base/neck connection
- **Service Access**: Removable rear skull panel for maintenance

#### **3D Printing Considerations**
- **Layer Orientation**: Print skull upright for optimal surface finish
- **Support Material**: Minimal supports needed with proper cavity design
- **Print Bed Adhesion**: Wide skull base provides stable printing platform
- **Post-Processing**: Smooth finish required for eye socket areas

### Mounting Hardware Specifications

#### **Display Mounting**
- **Screws**: M2.5 x 8mm socket head cap screws (16 total - 4 per display)
- **Washers**: M2.5 flat washers to distribute load
- **Spacers**: 2mm nylon spacers to prevent over-tightening
- **Gaskets**: Silicone O-rings for weather sealing (if outdoor use)

#### **Sensor Mounting**
- **Bracket**: Custom 3D printed HC-SR04 bracket
- **Attachment**: M3 x 12mm screws into threaded inserts
- **Alignment**: Precise forward-facing orientation for accuracy
- **Weather Protection**: Acoustic-transparent cover for outdoor use

#### **Speaker Mounting**
- **Drivers**: 28mm or 36mm diameter speakers (depending on skull size)
- **Enclosure**: Sealed back volume for proper bass response
- **Mounting**: M3 screws into reinforced ear cavity walls
- **Acoustic Design**: Ported or sealed depending on frequency response needs

### Creature-Specific Adaptations

#### **Goblin Skull (Base Design)**
- **Eye Spacing**: 53.34mm (2.1" center-to-center)
- **Skull Depth**: 140mm from face to back
- **Overall Width**: 130mm at widest point
- **Height**: 150mm from jaw to crown

#### **Cat/Feline Skulls**
- **Eye Spacing**: Reduced to 45mm for feline proportions
- **Snout Extension**: Additional 30mm forward projection
- **Ear Position**: Higher and more pointed ear cavities
- **Size Scaling**: 0.8x for domestic cat, 1.4x for large cats

#### **Dragon Skulls**
- **Eye Spacing**: Increased to 65mm for intimidating presence
- **Snout Extension**: Extended 50mm forward with tapered design
- **Crest Features**: Additional mounting points for decorative elements
- **Scale Texture**: Surface texturing for realistic dragon appearance

#### **Bear Skulls**
- **Eye Spacing**: 60mm (between goblin and dragon)
- **Massive Build**: 1.3x scale factor for imposing presence
- **Sagittal Crest**: Prominent ridge requiring reinforcement
- **Robust Construction**: Thicker walls (3.5mm) for durability

### Assembly Process

#### **Pre-Assembly Preparation**
1. **3D Print Validation**: Test fit all components before final assembly
2. **Thread Cutting**: Tap all mounting holes with appropriate thread pitch
3. **Surface Preparation**: Sand and smooth all mating surfaces
4. **Cable Preparation**: Pre-route cables through channels

#### **Component Installation Order**
1. **Displays First**: Mount all three displays with proper cable routing
2. **Sensor Second**: Install nose sensor with forward alignment verification
3. **Speakers Last**: Mount ear speakers with acoustic chamber sealing
4. **Cable Management**: Secure all cables and test continuity
5. **Final Assembly**: Close rear access panel and perform system test

#### **Quality Control Checklist**
- [ ] All displays properly seated and secured
- [ ] Sensor alignment verified with test measurements
- [ ] Speakers properly sealed with no air leaks
- [ ] All cables routed without stress or pinching
- [ ] No interference between components
- [ ] Skull aesthetics maintained with hidden mounting hardware

### Future Expansion Considerations

#### **Modular Upgrades**
- **Camera Integration**: Space reserved for ESP32-CAM module
- **Additional Sensors**: Provisions for IMU, temperature, humidity sensors
- **Servo Mounts**: Attachment points for jaw, eye, or ear movement
- **LED Integration**: Channels for accent lighting or status indicators

#### **Scaling System**
- **Parametric Design**: All dimensions scale proportionally with eye spacing
- **Component Variants**: Different sized components for different skull scales
- **Family Consistency**: Common mounting interface across all creature types
- **Manufacturing Efficiency**: Shared tooling and hardware across variants

This comprehensive mounting specification ensures that every skull assembly properly integrates all required hardware components while maintaining the anatomical accuracy and aesthetic appeal of each creature type.