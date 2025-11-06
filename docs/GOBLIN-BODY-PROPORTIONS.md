# Goblin Full Body Proportions - 4 Foot Target Height

## Design Philosophy

**Target**: A menacing but manageable **4-foot tall (48 inches / 122 cm)** goblin animatronic

**Proportions**: Based on "aggressive goblin" fantasy archetype
- Oversized head (20% larger than human proportions)
- Long arms (reaching past knees)
- Short torso (compressed vertebral column)
- Hunched posture (reduces effective height)
- Stocky, muscular build

## Overall Height Breakdown

### Standing Upright (Maximum Height)

```

  HEAD (Top of skull to base of neck)                   
  Height: 10 inches (25.4 cm)                            
   Oversized for goblin character                      
   Large cranium for electronics                       
   Prominent brow, pointed ears add height             
  48" (4 feet)
  NECK (C1-C7 vertebrae equivalent)                     
  Height: 3 inches (7.6 cm)                              
   Short, thick neck (goblin characteristic)           
  38"
  TORSO (Chest + Abdomen)                                
  Height: 15 inches (38 cm)                              
   Upper chest: 8" (spine servos 1-2)                  
   Lower abdomen: 7" (spine servo 3 + pelvis)          
   Battery pack integrated into spine structure        
  23"
  PELVIS (Hip joint mounting)                            
  Height: 3 inches (7.6 cm)                              
   Reference point (0,0,0) for coordinate system       
  20"
  UPPER LEG (Femur equivalent)                           
  Height: 10 inches (25.4 cm)                            
   Hip to knee joint                                    
   Contains thigh servos (flexion, rotation)           
  10"
  LOWER LEG (Tibia/Fibula equivalent)                    
  Height: 8 inches (20.3 cm)                             
   Knee to ankle joint                                  
   Contains calf servos (plantar/dorsiflexion)         
  2"
  FOOT (Ankle to ground)                                 
  Height: 2 inches (5 cm)                                
   Flat platform foot design                           
   Wide stance for stability                           
  0" (Ground)

TOTAL UPRIGHT: 48 inches (4 feet / 122 cm)
```

### Typical Hunched Posture (Characteristic Stance)

```
            Head forward, hunched shoulders
         HEAD 
         
             Neck angled forward 30
       
        TORSO      Spine bent forward 15
        (bent)  
       
             Pelvis tilted back
       
         LEGS      Knees slightly bent
        (bent)  
       
         
         FEET 
         

EFFECTIVE HEIGHT: 42-44 inches (hunched typical posture)
INTIMIDATION HEIGHT: 48 inches (fully upright, aggressive stance)
```

## Detailed Segment Dimensions

### Head Assembly

| Component | Dimension | Notes |
|-----------|-----------|-------|
| **Skull Length** | 8" (front to back) | Nose to occipital bone |
| **Skull Width** | 6" (side to side) | Ear to ear at widest |
| **Skull Height** | 7" (chin to crown) | Including jaw |
| **Ear Height** | +3" above skull | Pointed goblin ears extend up |
| **Total Head Height** | 10" | Including ears in upright position |
| **Head Weight** | ~1.5 lbs | Hollow 3D printed PLA skull |
| **Electronics** | ESP32-S3 in rear cavity | Wire routing channels |

**Component Spacing** (from nose center):
- Eye spacing: 2.1" (53mm) center-to-center
- Nose position: Origin (0, 0, 0)
- Mouth position: 1.05" below nose

### Neck Assembly

| Component | Dimension | Notes |
|-----------|-----------|-------|
| **Neck Length** | 3" | Compressed goblin neck |
| **Neck Diameter** | 3.5" | Thick, muscular appearance |
| **Pan Servo** | SG90 at base | 90 rotation |
| **Tilt Servo** | SG90 mid-neck | 45 tilt |
| **Range of Motion** | Pan: 180, Tilt: 90 | Full articulation |

### Torso Assembly (Power Distribution Core)

| Component | Dimension | Notes |
|-----------|-----------|-------|
| **Torso Length** | 15" | Compressed/hunched spine |
| **Chest Width** | 10" | Shoulder to shoulder |
| **Chest Depth** | 6" | Front to back |
| **Spine Segments** | 3 servos | Lower, middle, upper |
| **Battery Location** | Central spine cavity | 2S 5000mAh LiPo |
| **ESP32-S3 Location** | Rear chest cavity | Torso master controller |
| **80/20 Frame** | 1.5" x 1.5" extrusion | Aluminum skeleton |

**Spine Servo Positions** (from pelvis):
- Lower spine: 3" up (lumbar equivalent)
- Middle spine: 8" up (thoracic equivalent)
- Upper spine: 13" up (cervical equivalent)

### Arm Assembly (Long Reach)

| Component | Dimension | Notes |
|-----------|-----------|-------|
| **Shoulder Width** | 10" | Attachment points on torso |
| **Upper Arm** | 9" | Shoulder to elbow |
| **Forearm** | 8" | Elbow to wrist |
| **Hand** | 4" | Wrist to fingertip |
| **Total Arm Reach** | 21" | From shoulder |
| **Arm Span** | 52" | Fingertip to fingertip (wider than height!) |

**Characteristic**: Goblin arms reach **past the knees** when standing

### Leg Assembly (Stable Platform)

| Component | Dimension | Notes |
|-----------|-----------|-------|
| **Hip Width** | 8" | Pelvis mounting points |
| **Upper Leg** | 10" | Hip to knee |
| **Lower Leg** | 8" | Knee to ankle |
| **Foot Length** | 6" | Heel to toe |
| **Foot Width** | 4" | Wide stance |
| **Stance Width** | 12" | Foot center to foot center |

**Walking Parameters**:
- Step length: 8-12 inches
- Step height: 3 inches max
- Walking speed: 0.5-1.0 mph (slow, deliberate)
- Running speed: 2.0 mph (short bursts)

## Weight Distribution

### Component Weight Budget (4-Foot Goblin)

| Subsystem | Weight | Percentage | Notes |
|-----------|--------|------------|-------|
| **Head** | 1.5 lbs | 6% | Hollow skull + 3 displays + sensor |
| **Neck** | 0.5 lbs | 2% | 2 servos + mounting |
| **Torso** | 5.0 lbs | 20% | Battery (3 lbs) + ESP32 + frame |
| **Arms (2x)** | 4.0 lbs | 16% | 7 servos each + structure |
| **Legs (2x)** | 8.0 lbs | 32% | Heavy-duty servos + NEMA 17 motors |
| **Electronics** | 2.0 lbs | 8% | Wiring, PCBs, connectors |
| **Skin/Shell** | 4.0 lbs | 16% | Latex or foam rubber covering |
| **TOTAL** | **25 lbs** | 100% | Manageable for 4-foot robot |

**Center of Gravity**: 18" above ground (mid-torso), slightly forward of pelvis

## Comparison to Human Proportions

| Measurement | Human (Adult) | Goblin | Ratio |
|-------------|---------------|--------|-------|
| **Total Height** | 5'8" (68") | 4'0" (48") | 0.71 |
| **Head Height** | 9" | 10" | 1.11 (oversized) |
| **Torso Length** | 24" | 15" | 0.63 (compressed) |
| **Arm Length** | 24" | 21" | 0.88 |
| **Leg Length** | 32" | 20" | 0.63 (short legs) |
| **Arm Span** | 68" | 52" | 0.76 |
| **Head to Body** | 1:7.5 | 1:3.8 | Large head ratio |

**Goblin Characteristics**:
-  Oversized head (1.11 human)
-  Compressed torso (0.63 human)
-  Short legs (0.63 human)
-  Long arms relative to height (reach past knees)
-  Wide stance (12" vs typical 6-8")

## Scale Options (Future Variants)

### Compact Goblin (3 Feet)
- **Target**: Indoor/tabletop display, lighter weight
- **Scale**: 0.75 of 4-foot design
- **Height**: 36 inches (3 feet)
- **Weight**: ~15 lbs
- **Use case**: Desktop demo, convention display

### Standard Goblin (4 Feet)  **RECOMMENDED**
- **Target**: Primary prototype, manageable size
- **Scale**: 1.0 (base design)
- **Height**: 48 inches (4 feet)
- **Weight**: ~25 lbs
- **Use case**: Full feature testing, indoor operation

### Tall Goblin (5 Feet)
- **Target**: More imposing, outdoor capable
- **Scale**: 1.25 of 4-foot design
- **Height**: 60 inches (5 feet)
- **Weight**: ~40 lbs
- **Use case**: Outdoor events, haunted attractions

### Giant Goblin (6 Feet)
- **Target**: Maximum intimidation
- **Scale**: 1.5 of 4-foot design
- **Height**: 72 inches (6 feet)
- **Weight**: ~65 lbs
- **Use case**: Theme park, permanent installation

## Current Skull Scale Validation

**Clean Goblin Skull** (in development):

Based on P32 component positions:
- Eye spacing: 53.34mm (2.1")  **Scales to 10" head** 
- Display diameter: 32.5mm (1.28")  **Proportional for 4-foot body** 
- Sensor size: 45mm  20mm  **Fits goblin nose scale** 

**Skull dimensions** (estimated from OpenSCAD model):
- Length: ~200mm (7.9")  **Good for 4-foot body** 
- Width: ~150mm (5.9")  **Proportional** 
- Height: ~180mm (7.1")  **Add 3" ears = 10" total** 

**Current skull is correctly sized for 4-foot goblin!** 

## Design Validation Checklist

- [x] **Head scale**: 10" height (including ears) 
- [x] **Component mounting**: GC9A01 displays fit proportionally 
- [x] **Eye spacing**: 2.1" appropriate for head size 
- [ ] **Neck length**: 3" neck assembly design needed
- [ ] **Torso frame**: 15" 80/20 skeleton layout needed
- [ ] **Arm reach**: 21" arms extend past knees (verify)
- [ ] **Leg stability**: 20" legs + 12" stance width (design)
- [ ] **Total height**: 48" assembled height (verify)
- [ ] **Weight budget**: 25 lbs total (track components)

## CAD Model Recommendations

**For accurate scaling**:

1. **Set skull size**: Current model is ~7.9" (good)  add 3" ears
2. **Neck proportion**: Create 3"  3.5" dia neck structure
3. **Torso frame**: 15" vertical 80/20 extrusion spine
4. **Full assembly**: Stack all components to verify 48" total

**OpenSCAD scale factor**: Current skull units are mm
- 1 inch = 25.4mm
- 48 inches total = 1219.2mm from ground to ear tips

## Summary

 **Target Height**: 4 feet (48 inches) standing upright  
 **Hunched Height**: 42-44 inches (typical posture)  
 **Total Weight**: ~25 lbs (manageable)  
 **Head Size**: 10 inches (current skull design is correct scale!)  
 **Arm Reach**: 21 inches (past knees - goblin characteristic)  
 **Stability**: 12" wide stance, low center of gravity  
 **Proportions**: Oversized head, compressed torso, short legs  

**Current clean_goblin_skull.scad is correctly sized for 4-foot goblin body!** 

 **Next Steps**:
1. Design 3" neck assembly (pan/tilt servos)
2. Layout 15" torso frame (80/20 + battery)
3. Design 10" + 8" leg segments (hip/knee/ankle)
4. Design 9" + 8" + 4" arm segments (shoulder/elbow/wrist/hand)
5. Full-body assembly verification
