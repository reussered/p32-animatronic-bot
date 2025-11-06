#  P32 GOBLIN FULL - Complete Build Guide
**Professional Assembly Instructions with High-Quality Visual References**

> ** Note:** This guide references detailed photos and professional schematics. For the complete visual experience, download the full PDF build guide with high-resolution images from the repository releases section.

---

##  PARTS LIST & VERIFICATION

### Required Hardware Components

####  **Displays (3x GC9A01 Circular LCD)**

**Technical Specifications:**
- **Model:** GC9A01 Round TFT Display  
- **Size:** 1.28 inch diameter (32.5mm)
- **Resolution:** 240x240 pixels RGB565
- **Interface:** 4-wire SPI
- **Operating Voltage:** 3.3V (5V tolerant)
- **Current Draw:** ~50mA active, <1mA sleep
- **Backlight:** Built-in LED backlight with PWM control
- **Connector:** 7-pin header (VCC, GND, SCL, SDA, RES, DC, CS)

** [Photo Reference: GC9A01-identification.jpg]**
*Shows actual GC9A01 displays with pin labels and size comparison to coin*

**Part Numbers & Sources:**
- **Adafruit:** Product ID #4993 (GC9A01 Round Color TFT Display)
- **SparkFun:** LCD-16394 (1.28" Round TFT Display)  
- **Amazon:** Search "GC9A01 1.28 inch round TFT" 
- **AliExpress:** Generic GC9A01 modules (~$8-12 each)

**Visual Identification Guide:**
*Look for these key features when purchasing:*
-  Perfectly circular display (not square)
-  7-pin connector on back with clear labels
-  "GC9A01" printed on PCB or datasheet
-  Approximately 1.28" (32mm) diameter
-  Colorful test pattern when powered (rainbow/gradient)

####  **Audio System**
- **1x I2S Speaker** (3W, 4 recommended)
- **Audio Amplifier Board** (MAX98357A or similar)

####  **Sensors**
- **1x HC-SR04 Ultrasonic Sensor** (nose proximity detection)

####  **Main Controller**
- **1x ESP32-S3-DevKitC-1** development board

####  **Wiring & Power**
- **Breadboard or PCB** for connections
- **Jumper wires** (male-to-male, male-to-female)
- **5V Power Supply** (2A minimum)
- **Breadboard power rails**

---

##  ASSEMBLY STEP 1: ESP32-S3 SETUP

### Visual ESP32-S3 Pin Layout
```
ESP32-S3-DevKitC-1 Pinout (Top View)
                    
    USB-C                           BOOT  RST
                                         
    
      3V3  EN  36  37  38  39  40  41  42   
                                             
      GND  0   1   2   3   4   5   6   7       I2S Pins (4,5,6)
                                             
      VIN  8   9  10  11  12  13  14  15       SPI Pins (12,13,14,15)
                                             
      GND  16  17  18  19  20  21  35  45      More SPI (16,17) + Sensor (9,10)
    
           |   |   |   |   |   |   |   |
           CS2 CS3                
                  Sensor Pins       
                         (TRIG=9, ECHO=10)
```

###  **STEP 1.1: Mark Your Pins**
Use colored tape or stickers to mark these pins:

 **RED** - Power pins (3V3, VIN, GND)
 **BLUE** - SPI pins (12, 13, 14, 15, 16, 17)  
 **GREEN** - I2S pins (4, 5, 6)
 **YELLOW** - Sensor pins (9, 10)

---

##  ASSEMBLY STEP 2: POWER DISTRIBUTION

### Power Rail Setup Diagram
```
Breadboard Power Distribution

    + Rail  +5V (Red wire)
                                             
       ESP32 VIN  ESP32-S3 VIN pin
      
       Display 1 VCC  Left Eye Power
      
       Display 2 VCC  Right Eye Power  
      
       Display 3 VCC  Mouth Power
      
       Speaker Amp VCC  Audio Power

    - Rail  GND (Black wire)
      
       ESP32 GND  ESP32-S3 GND pin
      
       Display 1 GND  Left Eye Ground
        
       Display 2 GND  Right Eye Ground
      
       Display 3 GND  Mouth Ground
      
       Speaker Amp GND  Audio Ground
```

###  **STEP 2.1: Connect Power Rails**
```
Visual Power Connection Steps:

[1] 5V Supply  Breadboard + Rail
    5V PSU (Red Wire) Breadboard + Rail
    
[2] GND Supply  Breadboard - Rail  
    5V PSU (Black Wire) Breadboard - Rail

[3] ESP32 Power Connections
    Breadboard + Rail (Red) ESP32 VIN
    Breadboard - Rail (Black) ESP32 GND
```

---

##  ASSEMBLY STEP 3: SPI DISPLAY WIRING

### SPI Bus Architecture Diagram
```
SPI Bus Shared Connections (3 displays share these pins)

ESP32-S3          Shared SPI Bus          All 3 Displays
             
 GPIO 12       MISO         Display SDO 
 GPIO 13       MOSI         Display SDA   
 GPIO 14       CLK          Display SCL 
             
```

### Individual Chip Select Wiring
```
Unique CS Pins (Each display needs its own)

ESP32-S3          CS Assignments          Individual Displays
             
 GPIO 15     CS1 (Left)     Left Eye CS     
 GPIO 16     CS2 (Right)    Right Eye CS    
 GPIO 17     CS3 (Mouth)    Mouth CS          
             
```

###  **STEP 3.1: Wire SPI Shared Connections**

**Visual Wiring Steps:**
```
[SHARED PINS - Connect to ALL 3 displays]

ESP32 Pin 12 (MISO)  Display SDO pins
ESP32     Display1 Display2 Display3
  12      SDO        SDO        SDO    
       

ESP32 Pin 13 (MOSI)  Display SDA pins  
ESP32     Display1 Display2 Display3
  13      SDA        SDA        SDA    
       

ESP32 Pin 14 (CLK)  Display SCL pins
ESP32     Display1 Display2 Display3  
  14      SCL        SCL        SCL    
       
```

###  **STEP 3.2: Wire Individual CS Pins**

**Visual CS Wiring:**
```
[UNIQUE PINS - Each display gets its own CS]

LEFT EYE (Display 1):
ESP32     Left Eye Display
  15          CS           
     

RIGHT EYE (Display 2): 
ESP32     Right Eye Display
  16           CS         
     

MOUTH (Display 3):
ESP32     Mouth Display  
  17         CS       
     
```

### Complete SPI Wiring Table
| ESP32-S3 Pin | Wire Color | Display Pin | Function | Connects To |
|--------------|------------|-------------|----------|-------------|
| **GPIO 12** |  Blue | SDO | MISO | ALL 3 displays |
| **GPIO 13** |  Green | SDA | MOSI | ALL 3 displays |
| **GPIO 14** |  Yellow | SCL | CLK | ALL 3 displays |
| **GPIO 15** |  Orange | CS | Chip Select | Left Eye ONLY |
| **GPIO 16** |  Purple | CS | Chip Select | Right Eye ONLY |
| **GPIO 17** |  Brown | CS | Chip Select | Mouth ONLY |

---

##  ASSEMBLY STEP 4: I2S AUDIO WIRING  

### I2S Audio Architecture
```
I2S Digital Audio Connection

ESP32-S3          I2S Bus              Audio Amplifier
                
 GPIO 4     BCLK      Amp BCLK      
 GPIO 5      WS       Amp LRCK/WS   
 GPIO 6     DATA      Amp DIN        
                
                                             
                                             
                                      
                                          Speaker 
                                          4 3W       
                                      
```

###  **STEP 4.1: Wire I2S Audio**

**Visual I2S Wiring:**
```
ESP32-S3  MAX98357A Audio Amp

[BCLK - Bit Clock]
ESP32     Audio Amp
   4      BCLK    
     

[WS - Word Select / Left-Right Clock]  
ESP32     Audio Amp
   5    LRCK/WS   
     

[DATA - Digital Audio Data]
ESP32     Audio Amp
   6       DIN    
     

[SPEAKER OUTPUT]
Audio Amp     Speaker
    +          +    
    -          -      
     
```

### I2S Wiring Table
| ESP32-S3 Pin | Wire Color | Amp Pin | Function |
|--------------|------------|---------|----------|
| **GPIO 4** |  Red | BCLK | Bit Clock |
| **GPIO 5** |  Black | LRCK/WS | Word Select |
| **GPIO 6** |  Green | DIN | Audio Data |

---

##  ASSEMBLY STEP 5: SENSOR WIRING

### HC-SR04 Ultrasonic Sensor (Nose)
```
Ultrasonic Distance Sensor Wiring

ESP32-S3          HC-SR04 Sensor
      
 GPIO 9        TRIG        Trigger Pin
 GPIO 10       ECHO        Echo Pin  
   5V          VCC         Power
   GND         GND         Ground
      
```

###  **STEP 5.1: Wire HC-SR04 Sensor**

**Visual Sensor Wiring:**
```
HC-SR04 Ultrasonic Sensor Connections

[POWER]
Breadboard +5V  HC-SR04 VCC  
Breadboard GND  HC-SR04 GND

[CONTROL PINS]
ESP32     HC-SR04
   9     TRIG    (Trigger ultrasonic pulse)
     

ESP32     HC-SR04  
  10     ECHO    (Receive echo response)
     
```

### Physical Sensor Placement
```
Goblin Head Layout (Top View)

          Left Eye           Right Eye
    (Display 1)              (Display 2)
                                 
                  Nose           
              (HC-SR04)          
                                   
           Mouth 
              (Display 3)
              
 Speaker Position: Behind/Inside head
```

---

##  ASSEMBLY STEP 6: COMPLETE WIRING DIAGRAM

### Master Wiring Schematic
```
COMPLETE P32 GOBLIN FULL WIRING DIAGRAM

                         ESP32-S3-DevKitC-1
                    
         5V Supply   VIN                 3V3 
         GND   GND                  EN 
                                             
    HC-SR04 TRIG   9                    36   
    HC-SR04 ECHO   10                   37 
                                             
      I2S BCLK   4                    38 
      I2S WS   5                    39   
      I2S DATA   6                    40 
                                             
                     12  SPI MISO (All displays)
                     13  SPI MOSI (All displays)  
                     14  SPI CLK  (All displays)
                     15  Left Eye CS
                     16  Right Eye CS
                     17  Mouth CS
                    
                                      
                                       Mouth Display
                                     Right Eye Display  
                                   Left Eye Display
                                 SPI CLK  All Displays
                               SPI MOSI  All Displays
                             SPI MISO  All Displays
```

### Connection Verification Checklist

####  **Power Connections**
- [ ] ESP32-S3 VIN connected to +5V rail
- [ ] ESP32-S3 GND connected to GND rail  
- [ ] All 3 displays VCC connected to +5V rail
- [ ] All 3 displays GND connected to GND rail
- [ ] Audio amp VCC connected to +5V rail
- [ ] Audio amp GND connected to GND rail
- [ ] HC-SR04 VCC connected to +5V rail  
- [ ] HC-SR04 GND connected to GND rail

####  **SPI Display Connections**  
- [ ] GPIO 12  All displays SDO (MISO)
- [ ] GPIO 13  All displays SDA (MOSI)
- [ ] GPIO 14  All displays SCL (CLK)
- [ ] GPIO 15  Left eye CS ONLY
- [ ] GPIO 16  Right eye CS ONLY  
- [ ] GPIO 17  Mouth CS ONLY

####  **I2S Audio Connections**
- [ ] GPIO 4  Audio amp BCLK
- [ ] GPIO 5  Audio amp LRCK/WS
- [ ] GPIO 6  Audio amp DIN  
- [ ] Speaker connected to amp outputs

####  **Sensor Connections**
- [ ] GPIO 9  HC-SR04 TRIG
- [ ] GPIO 10  HC-SR04 ECHO

---

##  ASSEMBLY STEP 7: SOFTWARE INSTALLATION

###  **STEP 7.1: Development Environment Setup**

**Install Required Software:**
```
1.  Download & Install:
    Visual Studio Code
    PlatformIO Extension  
    Git for Windows
   
2.  Clone Repository:
   git clone https://github.com/reussered/p32-animatronic-bot.git
   
3.  Open Project:
    Launch VS Code
    Open folder: p32-animatronic-bot
    PlatformIO should auto-detect project
```

###  **STEP 7.2: Firmware Compilation & Upload**

**Build Process:**
```
 Compile Firmware:
   Terminal Command: pio run
   
   Expected Output:
    RAM:   [=         ]   5.8% (used 18912 bytes)
    Flash: [=====     ]  51.7% (used 542509 bytes)
    [SUCCESS] Took ~5 seconds
   
 Upload to ESP32-S3:
   Terminal Command: pio run -t upload
   
   Expected Output:
    Auto-detected: COM4 (or your port)
    Uploading firmware.bin
    Writing at 0x00010000... (100 %)
    [SUCCESS] Took ~10 seconds
```

###  **STEP 7.3: System Verification**

**Monitor Serial Output:**
```
 Start Serial Monitor:
   Terminal Command: pio device monitor
   
   Expected Startup Sequence:
    I (103) esp_image: segment 1: paddr=00027944
    I (791) P32_MAIN: [goblin_eye_left] Executing (loop 100)  
    I (791) EYE_DISPLAY: Starting animation 'goblin_blink'
    I (811) P32_MAIN: [goblin_eye_right] Executing (loop 100)
    I (821) P32_MAIN: [goblin_nose] Executing (loop 100)
   
   System Status Indicators:
    Loop counters incrementing (100, 150, 200...)
    Eye animations cycling (blink  angry_stare)  
    Color values changing (#ff2222  #ff4040)
    Distance measurements from nose sensor
```

---

##  ASSEMBLY STEP 8: PHYSICAL MOUNTING & ENCLOSURE

### Goblin Head Physical Layout
```
3D Skull Coordinate System Layout

                    Front View
                
                                 Left Eye (-1.05", +0.7")
                               Right Eye (+1.05", +0.7") 
                                 Nose Sensor (0", 0", +0.25")
                                 
                                 Mouth (0", -1.05", 0")  
                
                
                    Side View  
                
                               Eyes & sensor depth
                                 Speaker (-0.5", +0.5", -1.0")
                                 
                                 Mouth flush with face
                
```

###  **STEP 8.1: Display Mounting**

**Eye Display Positioning:**
```
Left Eye Mount:

   Mount specifications 
  
 X: -1.05" from center    (Left of nose)
 Y: +0.7" from center     (Above mouth)  
 Z: -0.35" from surface   (Slightly recessed)
 Diameter: 1.28"         
 Mounting: 4 screws      


Right Eye Mount:  

   Mount specifications 

 X: +1.05" from center    (Right of nose)
 Y: +0.7" from center     (Above mouth)
 Z: -0.35" from surface   (Slightly recessed)  
 Diameter: 1.28"         
 Mounting: 4 screws      

```

**Mouth Display Positioning:**  
```
Mouth Mount:

   Mount specifications 

 X: 0" (centered)         
 Y: -1.05" from center    (Below eyes)
 Z: 0" (flush)            (Surface level)
 Diameter: 1.28"         
 Mounting: 4 screws        

```

###  **STEP 8.2: Sensor & Audio Placement**

**Nose Sensor Mount:**
```
HC-SR04 Positioning:

   Sensor specs        

 X: 0" (centered)        
 Y: 0" (between eyes)    
 Z: +0.25" (protruding)   (Slight nose bump)
 Detection: 2cm-400cm    
 Angle: 15 cone         

```

**Speaker Placement:**
```
Internal Speaker Mount:

   Audio placement       

 X: -0.5" (left side)    
 Y: +0.5" (upper area)     
 Z: -1.0" (deep inside)   (Behind displays)
 Size: 40mm diameter     
 Power: 3W maximum       

```

---

##  ASSEMBLY STEP 9: FIRST POWER-ON & TESTING

###  **STEP 9.1: Pre-Power Checks**

**Visual Inspection Checklist:**
```
 Wire Verification:
    No loose connections
    No wire crossings/shorts  
    All components securely mounted
    Power supply voltage = 5V
    USB cable connected to ESP32-S3
   
 Power Rail Check:  
    Multimeter: +5V rail = 5.0V 0.1V
    Multimeter: GND continuity confirmed
    No voltage on signal lines (unpowered)
```

###  **STEP 9.2: First Power-On Sequence**

**Safe Startup Procedure:**
```
[1]  Connect USB cable (ESP32-S3 to computer)
     ESP32 should power on (LED indicators)
     Computer should detect COM port
    
[2]  Connect 5V power supply to breadboard
     All displays should show initialization
     Speaker may emit brief startup sound
    
[3]  Open serial monitor immediately
     Check for boot messages
     Verify component initialization
    
[4]  Visual system check:
     Left eye display: Shows blinking animation  
     Right eye display: Shows blinking animation
     Mouth display: Shows synchronized animation
     All displays cycle through colors
    
[5]  Audio system check:
     Speaker produces clear sound
     No distortion or crackling
     Volume appropriate
    
[6]  Sensor system check:  
     Wave hand in front of nose sensor
     Serial monitor shows distance readings
     Values change from 28cm to 10cm, etc.
```

###  **STEP 9.3: System Health Verification**

**Expected Normal Operation:**
```
 HEALTHY SYSTEM INDICATORS:

Serial Monitor Output:

 I (10791) P32_MAIN: [goblin_eye_left] Loop 100 
 I (10791) EYE_DISPLAY: Animation 'blink' OK    
 I (10811) P32_MAIN: [goblin_eye_right] Loop 100  
 I (10821) P32_MAIN: [goblin_nose] Loop 100     
 ACT: goblin_nose - distance 28.5cm            
            
  P32 LEFT EYE Display (200x200)              
  ANGRY | Open:0.3 | Pupil:0.8              
  Color: #ff2222 | Time: 1018ms              
            


Visual Indicators:
  Eyes: Smooth blinking, color changes
  Mouth: Coordinated with eye animations  
  Sensor: Responsive to movement
  Audio: Clear sound output
  Loop counts: Steadily incrementing
```

---

##  ASSEMBLY STEP 10: MOOD SYSTEM DEMONSTRATION

###  **STEP 10.1: 9-Emotion System Test**

**Mood State Verification:**
```
Your goblin has 9 emotional states that automatically 
affect animations, colors, and behaviors:

1.  FEAR      - Eyes dart, pale colors, rapid blinks
2.  ANGER     - Red colors, intense stare, dilated pupils  
3.  IRRITATION- Orange tints, slightly narrowed eyes
4.  HAPPINESS - Bright colors, wide eyes, gentle blinks
5.  CONTENTMENT- Soft colors, relaxed expression  
6.  HUNGER    - Green tints, searching eye movements
7.  CURIOSITY - Blue tints, alert expression, sensor active
8.  AFFECTION - Pink/warm tints, soft gaze
9.  EXCITEMENT- Bright flashing colors, rapid movements
```

**Mood Testing Instructions:**
```
 Test Each Emotion:

[Manual Trigger Methods]
 Proximity: Move objects near sensor  CURIOSITY
 Duration: Leave alone 5+ minutes  CONTENTMENT  
 Interaction: Rapid hand movements  EXCITEMENT
 Environment: Bright light  HAPPINESS
 Environment: Darkness  FEAR

[Serial Monitor Mood Changes]
Watch for automatic transitions:

 I (15234) MOOD: State ANGER       Current mood
 I (15456) MOOD: Transition to     Change trigger  
 I (15456) MOOD: State CURIOSITY   New mood
 I (15678) DISPLAY: Recomputing    Animation update
 I (15678) DISPLAY: New colors     Color change

```

---

##  TROUBLESHOOTING GUIDE

###  **Common Issues & Solutions**

#### **Issue 1: Displays Not Working**
```
 SYMPTOMS:
    Black screens on all/some displays
    No animation visible
   
 DIAGNOSIS:
    Check SPI wiring (pins 12, 13, 14)
    Verify individual CS pins (15, 16, 17)
    Confirm 5V power to displays
   
 SOLUTION:
   1. Verify shared SPI bus connections
   2. Check unique CS pin assignments  
   3. Test with multimeter: 5V at display VCC
   4. Reseat all SPI connections
```

#### **Issue 2: No Audio Output**
```
 SYMPTOMS:
    Silent speaker
    Audio amp not responding
   
 DIAGNOSIS:  
    Check I2S wiring (pins 4, 5, 6)
    Verify amp power connections
    Test speaker impedance (should be 4-8)
   
 SOLUTION:
   1. Verify I2S pin connections
   2. Check amp VCC = 5V, GND connected
   3. Test speaker with multimeter
   4. Verify amp enable pin (if present)
```

#### **Issue 3: Sensor Not Responding**  
```
 SYMPTOMS:
    No distance readings in serial monitor
    Fixed distance value (like 400cm)
   
 DIAGNOSIS:
    Check HC-SR04 wiring (pins 9, 10)  
    Verify sensor power (5V required)
    Test TRIG/ECHO pin assignments
   
 SOLUTION:
   1. Verify GPIO 9  TRIG, GPIO 10  ECHO
   2. Check 5V power to HC-SR04 VCC
   3. Test sensor separately with Arduino
   4. Check for obstacles blocking sensor
```

#### **Issue 4: Build/Upload Failures**
```
 SYMPTOMS:
    Compilation errors
    Upload failures
    COM port not detected
   
 DIAGNOSIS:
    Check ENABLE_GOBLIN_COMPONENTS flag
    Verify ESP32-S3 USB connection
    Confirm correct COM port
   
 SOLUTION:
   1. Check src/p32_component_config.h:
      #define ENABLE_GOBLIN_COMPONENTS
   2. Try different USB cable/port
   3. Press BOOT+RST buttons on ESP32-S3
   4. Clean build: pio run --target clean
```

---

##  FINAL ASSEMBLY VERIFICATION

###  **Complete System Checklist**

```
 HARDWARE VERIFICATION:
    ESP32-S3 powered and programmed
    3x GC9A01 displays mounted and working
    1x I2S speaker producing audio  
    1x HC-SR04 sensor detecting distance
    All wiring secure and labeled
    5V power supply stable
   
 SOFTWARE VERIFICATION:  
    Firmware compiles without errors
    Upload successful to ESP32-S3
    Serial monitor shows component execution
    Loop counters incrementing normally
    All 3 components (eyes + nose) active
   
 FUNCTIONALITY VERIFICATION:
    Eyes display blinking animations
    Colors change dynamically (RGB565)
    Mood transitions working automatically  
    Sensor responds to proximity
    Audio plays clearly through speaker
    No error messages in serial output
   
 BEHAVIORAL VERIFICATION:
    9 emotional states accessible
    Animations recompute with mood changes
    Color palettes match mood states
    Sensor triggers appropriate responses
    System runs continuously without crashes
```

###  **Congratulations!**

```
 YOUR P32 GOBLIN FULL IS COMPLETE! 

You now have a fully functional animatronic goblin with:
  9-emotion mood system
  Dual-eye animated displays  
  Mouth display for expressions
  Proximity sensing nose
  Audio output capability
  ESP32-S3 brain with mood-driven AI

 NEXT STEPS:
 Experiment with mood triggers
 Add custom animations  
 Expand to full-body components (arms, legs)
 Create interactive behaviors
 Build additional goblin variants

 DOCUMENTATION:
 See docs/SESSION_CONTEXT_SUMMARY.md for system details
 Review config/ folder for all component definitions
 Explore tools/ folder for 3D printing files
 Check assets/ for animations and sounds

 ENJOY YOUR NEW ANIMATRONIC COMPANION!
```

---

##  APPENDIX: REFERENCE MATERIALS

### Pin Reference Quick Card
```

           ESP32-S3 PIN REFERENCE        

 POWER:    VIN(5V)  GND                  
 SPI BUS:  12(MISO) 13(MOSI) 14(CLK)      
 SPI CS:   15(Left) 16(Right) 17(Mouth) 
 I2S:      4(BCLK)  5(WS)     6(DATA)   
 SENSOR:   9(TRIG)  10(ECHO)             

```

### Color Code Reference  
```
 MOOD COLOR MAPPING:
 FEAR       Pale Blue   (#ADD8E6)
 ANGER      Bright Red  (#FF0000)  
 IRRITATION Orange Red  (#FF4500)
 HAPPINESS  Yellow      (#FFFF00)
 CONTENTMENT Soft Green (#90EE90)
 HUNGER     Lime Green  (#32CD32)  
 CURIOSITY  Sky Blue    (#87CEEB)
 AFFECTION  Pink        (#FFC0CB)
 EXCITEMENT Magenta     (#FF00FF)
```

### Emergency Contacts & Support
```
 SUPPORT RESOURCES:
 GitHub Issues: github.com/reussered/p32-animatronic-bot/issues
 Documentation: /docs folder in repository
 Community: Discord/forums (see README.md)
 Hardware Help: Component manufacturer websites
 Software Help: PlatformIO community forum

 EMERGENCY SHUTDOWN:
   1. Disconnect 5V power supply immediately
   2. Disconnect USB cable from ESP32-S3  
   3. Check for hot components or burning smells
   4. Review wiring before reconnecting power
```

---

*End of P32 Goblin Full Build Guide*  
*Version 1.0 - Complete Visual Assembly Instructions*  
* 2025 P32 Animatronic Bot Project*