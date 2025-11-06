# JSON Mounting Architecture Specification
**How to specify hardware mounting approaches in positioned component configurations**

---

##  **Mounting Architecture Field**

Every positioned component JSON **MUST** include a `mounting_architecture` field that specifies how the hardware is physically mounted and shaped.

### **Field Structure:**
```json
{
  "mounting_architecture": {
    "type": "two_tier | integrated | hardware_direct",
    "hardware_mount": "basic_mount_name (if two_tier)",
    "decorative_shell": "shell_name (if two_tier)", 
    "integrated_shape": "shape_name (if integrated)",
    "shape_generation": "auto | manual | template"
  }
}
```

---

##  **Mounting Architecture Types**

### **1. Two-Tier Architecture** 
**Type:** `"two_tier"`
**Use When:** Hardware will be reused across multiple creatures

```json
{
  "mounting_architecture": {
    "type": "two_tier",
    "hardware_mount": "sensor_basic_mount",
    "decorative_shell": "goblin_nose_shell",
    "shape_generation": "auto"
  }
}
```

**Result:**
- Generates/uses `sensor_basic_mount.scad/.stl` (universal hardware bracket)
- Generates/uses `goblin_nose_shell.scad/.stl` (creature-specific shell incorporating hardware mount)
- Shell uses `use <../basic_mounts/sensor_basic_mount.scad>` pattern

### **2. Integrated Single-Piece**
**Type:** `"integrated"`
**Use When:** Custom design is simpler than two-tier approach

```json
{
  "mounting_architecture": {
    "type": "integrated", 
    "integrated_shape": "goblin_ear_complete",
    "shape_generation": "manual"
  }
}
```

**Result:**
- Generates/uses `goblin_ear_complete.scad/.stl` (single piece with hardware mount built-in)
- Hardware mount geometry is part of the creature-specific shape
- No separate basic mount file created

### **3. Hardware Direct**
**Type:** `"hardware_direct"`
**Use When:** Using hardware component physical shape directly

```json
{
  "mounting_architecture": {
    "type": "hardware_direct",
    "shape_generation": "none"
  }
}
```

**Result:**
- No custom mounting shape generated
- Uses hardware component's own mounting holes and geometry
- Hardware mounts directly to skull/frame structure
- Relies on standard screws/brackets from hardware vendor

---

##  **Shape Generation Options**

### **Auto Generation** (`"shape_generation": "auto"`)
- System automatically generates OpenSCAD files from hardware specs
- Uses templates and hardware JSON physical_specs
- Appropriate for standard components (displays, sensors, speakers)

### **Manual Design** (`"shape_generation": "manual"`)
- Designer creates custom OpenSCAD files manually
- Used for complex geometry that can't be auto-generated
- Appropriate for artistic or highly integrated designs

### **Template Based** (`"shape_generation": "template"`)
- Uses existing template files with parameter substitution
- Good for variations of proven designs
- Templates located in `assets/shapes/scad/templates/`

### **None** (`"shape_generation": "none"`)
- No shape files generated or required
- Hardware mounts directly using vendor-provided mounting solutions

---

##  **File Naming Conventions**

### **Two-Tier Architecture:**
```
assets/shapes/scad/basic_mounts/{hardware}_basic_mount.scad
assets/shapes/scad/bot_shells/{creature}_{location}_shell.scad

Examples:
- sensor_basic_mount.scad (universal)
- goblin_nose_shell.scad (creature-specific)
- display_basic_mount.scad (universal)
- cat_eye_shell.scad (creature-specific)
```

### **Integrated Architecture:**
```
assets/shapes/scad/integrated/{creature}_{location}_{hardware}.scad

Examples:
- goblin_ear_speaker.scad
- cat_tail_servo.scad
- bear_crown_leds.scad
```

### **Hardware Direct:**
```
No shape files generated.
Hardware mounts using vendor brackets/screws.
```

---

##  **Processing Workflow**

### **1. JSON Analysis**
Shape generation tools read positioned component JSON files and extract:
- `mounting_architecture.type`  Determines generation approach
- `hardware_reference`  Gets physical specs for mounting
- `mounting_architecture.hardware_mount`  Names universal bracket (if two_tier)
- `mounting_architecture.decorative_shell`  Names creature shell (if two_tier)

### **2. Shape Generation**
Based on architecture type:

**Two-Tier:**
1. Generate/verify `{hardware}_basic_mount.scad` exists
2. Generate `{creature}_{location}_shell.scad` incorporating basic mount
3. Both files output to appropriate STL directories

**Integrated:**
1. Generate `{creature}_{location}_{hardware}.scad` as single piece
2. Include hardware mounting geometry directly in creature aesthetics

**Hardware Direct:**
1. No shape generation required
2. Document required vendor mounting hardware

### **3. STL Output**
Generated STL files organized by architecture type:
```
assets/shapes/stl/
 basic_mounts/         Universal hardware brackets
 bot_shells/           Creature-specific shells  
 integrated/           Single-piece designs
 vendor/               Hardware direct (documentation only)
```

---

##  **Complete Examples**

### **Goblin Nose (Two-Tier)**
```json
{
  "component_id": "goblin_nose",
  "hardware_reference": "config/components/hardware/hc_sr04_sensor.json",
  "mounting_architecture": {
    "type": "two_tier",
    "hardware_mount": "sensor_basic_mount", 
    "decorative_shell": "goblin_nose_shell",
    "shape_generation": "auto"
  },
  "position": {"x": "0 INCH", "y": "0 INCH", "z": "+0.25 INCH"}
}
```

**Generated Files:**
- `sensor_basic_mount.scad/.stl` (reusable across all creatures)
- `goblin_nose_shell.scad/.stl` (goblin-specific with warts, incorporates sensor mount)

### **Goblin Ear (Integrated)**
```json
{
  "component_id": "goblin_ear_left", 
  "hardware_reference": "config/components/hardware/speaker.json",
  "mounting_architecture": {
    "type": "integrated",
    "integrated_shape": "goblin_ear_complete",
    "shape_generation": "manual"
  },
  "position": {"x": "-2.1 INCH", "y": "+0.5 INCH", "z": "+0.8 INCH"}
}
```

**Generated Files:**
- `goblin_ear_complete.scad/.stl` (single piece with speaker cavity and goblin ear aesthetics)

### **Power LED Strip (Hardware Direct)**
```json
{
  "component_id": "status_leds",
  "hardware_reference": "config/components/hardware/ws2812b_strip.json", 
  "mounting_architecture": {
    "type": "hardware_direct",
    "shape_generation": "none"
  },
  "position": {"x": "0 INCH", "y": "-1.5 INCH", "z": "+1.2 INCH"}
}
```

**Generated Files:**
- None (LED strip mounts using adhesive backing or vendor clips)

---

##  **Implementation Benefits**

 **Clear Intent**: JSON explicitly states mounting approach for each component  
 **Tool Compatibility**: Shape generators know exactly what to create  
 **Design Flexibility**: Mix approaches within same project as appropriate  
 **Documentation**: Self-documenting configuration files  
 **Automation**: Tools can auto-generate appropriate OpenSCAD files  
 **Validation**: Can verify that required shape files exist for each architecture type  

This specification ensures that every component's mounting approach is explicitly defined and automatically processable by the shape generation tools.