# Visual Component Guide
*What Each STL File Looks Like and Does*

---

##  **MAIN SKULL STRUCTURES**

### `green_goblin_skull.stl` (1.55 MB)
```
     
                           
                                Large eye sockets for displays
                             
                            Nose cavity for sensor
                           
                Mouth opening for display
                           
         
                           
     
```
**Features:**
- 3.0" eye spacing with deep sockets
- Prominent goblin nose bridge
- Large mouth cavity for display
- Internal cable routing channels
- ESP32-S3 mounting cavity in back
- Screw bosses for component attachment

---

##  **COMPONENT MOUNTS**

### `display_basic_mount.stl` (Generated)
```
    
        GC9A01          Screw holes for display
                     
                  Display opening
         ROUND     
         LCD       
            
                     
                         Mounting tabs for skull
    
```
**Purpose**: Secures round GC9A01 displays in eye and mouth sockets

### `sensor_basic_mount.stl` (Generated)
```
    
     HC-SR04 SENSOR  
       TRIG  ECHO        Ultrasonic transducers
                     
                     PCB mounting posts
               
                     
                  Goblin nose shell mount
    
```
**Purpose**: Mounts HC-SR04 sensor in nose position for proximity detection

### `speaker_basic_mount.stl` (Generated)
```
    
         SPEAKER           I2S speaker cavity
          
                  Acoustic mesh opening
            
          
                     
       MOUNT TAB         Internal mounting points
    
```
**Purpose**: Secures I2S speaker inside skull cavity for audio output

---

##  **COMPONENT DIMENSIONS**

### GC9A01 Round LCD Display
- **Size**: 28mm diameter active area
- **PCB**: 32mm x 32mm square board
- **Thickness**: ~3mm PCB + components
- **Mounting**: 4x M2 screws on 26mm spacing
- **Interface**: SPI (shared bus + individual CS)

### HC-SR04 Ultrasonic Sensor  
- **Size**: 45mm x 20mm x 15mm
- **Transducers**: 16mm diameter ultrasonic elements
- **Range**: 2cm to 4m distance measurement
- **Mounting**: 2x mounting holes on 40mm centers
- **Interface**: Digital TRIG/ECHO pins

### ESP32-S3-DevKitC-1
- **Size**: 55mm x 25mm x 12mm
- **USB**: USB-C connector for programming/power
- **Pins**: 39 GPIO pins exposed on headers (dynamically assigned at runtime)
- **Power**: 5V input, 3.3V logic
- **Mounting**: Internal skull cavity with access holes

---

##  **WIRING VISUALIZATION**

### SPI Display Bus (Shared)
```
ESP32-S3          Display Mounts
         
 GPIO [D]  MISO   MISO   MISO   Left Eye, Right Eye, Mouth
 GPIO [D]  MOSI   MOSI   MOSI   (Shared bus saves pins)
 GPIO [D]  CLK    CLK    CLK  
                  
 GPIO [D] CS1 (Left Eye)
 GPIO [D] CS2 (Right Eye)
 GPIO [D] CS3 (Mouth)

```

### I2S Audio Connection
```
ESP32-S3          Speaker Mount
       
 GPIO [D]  BCLK (Clock) 
 GPIO [D]  WS   (Word)  
 GPIO [D]  DATA (Audio) 
       
```

### Sensor Connection
```
ESP32-S3          Nose Sensor Mount
       
 GPIO [D]  TRIG (Trigger) 
 GPIO [D]  ECHO (Response)
       
```

---

##  **ASSEMBLY PREVIEW**

### Complete Head Stack-Up
```
                      Speaker Mount (Internal)
    Eye Mounts    
                    
    
                               Display mounts in eye sockets
                            
         [SENSOR]            Nose sensor mount
                           
           Mouth display mount
        [ESP32-S3]            Controller in back cavity
      
                           
    
```

### Component Integration
1. **Skull provides framework** - Mounting points and cable routing
2. **Mounts secure electronics** - Precise fit for each component type  
3. **Wiring connects everything** - Shared buses minimize pin usage
4. **Software brings it alive** - Mood-driven animations and behaviors

---

##  **PRINT PREPARATION**

### Recommended Print Settings
- **Layer Height**: 0.2mm (balance of speed vs quality)
- **Infill**: 15% (adequate strength, faster printing)
- **Supports**: Yes (for skull overhangs and mounting tabs)
- **Adhesion**: Brim recommended (large surface area)

### Print Order Priority
1. **Test print**: Small mount first to verify settings
2. **Component mounts**: Quick prints to test fit
3. **Main skull**: Large print requiring good adhesion
4. **Assembly**: Test fit before final connections

---

*Each STL file contains precise 3D geometry optimized for 3D printing. The skull structure includes all mounting features, cable management, and proper spacing for the electronic components listed in the wiring diagram.*