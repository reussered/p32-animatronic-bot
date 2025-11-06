# P32 Goblin Head - Breadboard Wiring Guide
## Practical Assembly Instructions - October 15, 2025

---

## Hardware Inventory

###  Components Available Now

| Qty | Component | Model/Type | Purpose |
|-----|-----------|------------|---------|
| 1 | **ESP32-S3 DevKitC-1** | Dual-core 240 MHz | Main controller |
| 2 | **GC9A01 Display** | 1.28" circular LCD | Eyes (left & right) |
| 1 | **HC-SR04 Sensor** | Ultrasonic distance | Nose sensor |
| 1 | **Breadboard** | 830-point | Prototyping |
| - | **Jumper Wires** | M-M, M-F | Connections |
| 1 | **USB Cable** | USB-C or Micro-USB | Power & programming |

###  Components On Order (Not in This Guide)

| Component | Status | Notes |
|-----------|--------|-------|
| Audio Amplifier | Few days | Use stub component for now |
| Speakers | Few days | Will add when arrived |
| Mouth Display | Few days | Separate ESP32 subsystem node |
| HW-496 Microphones | TBD | Will add when confirmed |

---

## Pin Assignments - Head Subsystem

### ESP32-S3 DevKitC-1 GPIO Allocation

| GPIO | Function | Component | Notes |
|------|----------|-----------|-------|
| **SPI Bus (Shared by Both Eyes)** ||||
| 14 | `SPI_CLK` | Both displays | Your pin label: **SCK** |
| 13 | `SPI_MOSI` | Both displays | Your pin label: **SDA** |
| **Left Eye (GC9A01 #1)** ||||
| 27 | `DC` (Data/Command) | Left eye only | Your pin label: **DC** |
| 15 | `CS` (Chip Select) | Left eye only | Your pin label: **CS** |
| **Right Eye (GC9A01 #2)** ||||
| 33 | `DC` (Data/Command) | Right eye only | Your pin label: **DC** |
| 32 | `CS` (Chip Select) | Right eye only | Your pin label: **CS** |
| **Shared Display Control** ||||
| 26 | `RST` (Reset) | Both displays | Your pin label: **RST** |
| **Nose Sensor (HC-SR04)** ||||
| 25 | `TRIG` (Trigger) | Ultrasonic TX | Send ping pulse |
| 34 | `ECHO` (Echo) | Ultrasonic RX | Receive response (input only) |
| **Power Rails** ||||
| 3V3 | Power (+3.3V) | All components | Multiple connections |
| GND | Ground (0V) | All components | Common ground |

**Total GPIO Used**: 8 pins  
**Total Connections**: 22 wires (including power/ground)

---

## Your Display Pin Labels  ESP32 Mapping

Your GC9A01 displays have these pins exposed:

```
Display Pin Label  ESP32 Signal  GPIO Number

SCK   SPI_CLK    GPIO 14 (shared)
SDA   SPI_MOSI   GPIO 13 (shared)
DC    DATA_CMD   GPIO 27 (left) / GPIO 33 (right)
CS    CHIP_SEL   GPIO 15 (left) / GPIO 32 (right)
RST   RESET      GPIO 26 (shared)
VCC   POWER      3.3V rail
GND   GROUND     GND rail
```

**Remember**: 
- **SCK = CLK** (Clock signal)
- **SDA = MOSI** (Master Out Slave In - data from ESP32 to display)

---

## Breadboard Layout Diagram

```
                ESP32-S3 DevKitC-1
                
                                  
        USB-C  Power            
                                  
                  [3V3]  [GND]     Power rails
                                
                                
                
                                  
    GPIO 14  SCK (SPI_CLK)    
                                         Shared SPI Bus
    GPIO 13  MOSI (SPI_MOSI)  
                                        
    GPIO 26  RST (Shared)     
                                        
    GPIO 27  DC (Left Eye)       
    GPIO 15  CS (Left Eye)       
                                       
    GPIO 33  DC (Right Eye)      
    GPIO 32  CS (Right Eye)      
                                       
    GPIO 25  TRIG (Sensor)       
    GPIO 34  ECHO (Sensor)       
                                       
                     
                                         
        
                                        
                                        
    Breadboard Power Rails               
           
     [3V3]     
     [GND]  
    
                       
                       
         
        Left      Right     HC-SR04  
        Eye       Eye       Sensor   
        GC9A01    GC9A01             
         
```

---

## Step-by-Step Wiring Instructions

### Setup 1: Power Rails

**Goal**: Establish 3.3V and GND rails on breadboard

1. **Connect ESP32 3V3 pin  Breadboard + rail** (red wire)
2. **Connect ESP32 GND pin  Breadboard - rail** (black wire)

**Test**: Use multimeter - should read 3.3V between rails

---

### Setup 2: Left Eye Display (GC9A01 #1)

**Position**: Place display on left side of breadboard

**Wire Connections** (7 wires):

| Display Pin | Wire Color | Connect To | ESP32 Pin |
|-------------|------------|------------|-----------|
| **SCK** | Orange | SPI Bus | GPIO 14 |
| **SDA** | Yellow | SPI Bus | GPIO 13 |
| **DC** | Green | Left Eye Only | GPIO 27 |
| **CS** | Blue | Left Eye Only | GPIO 15 |
| **RST** | White | Shared | GPIO 26 |
| **VCC** | Red | Breadboard + rail | 3.3V |
| **GND** | Black | Breadboard - rail | GND |

**Test**: Display should power on (backlight visible when ESP32 powered)

---

### Setup 3: Right Eye Display (GC9A01 #2)

**Position**: Place display on right side of breadboard

**Wire Connections** (7 wires):

| Display Pin | Wire Color | Connect To | ESP32 Pin |
|-------------|------------|------------|-----------|
| **SCK** | Orange | SPI Bus | GPIO 14  **Same as left eye** |
| **SDA** | Yellow | SPI Bus | GPIO 13  **Same as left eye** |
| **DC** | Green | Right Eye Only | GPIO 33 |
| **CS** | Blue | Right Eye Only | GPIO 32 |
| **RST** | White | Shared | GPIO 26  **Same as left eye** |
| **VCC** | Red | Breadboard + rail | 3.3V |
| **GND** | Black | Breadboard - rail | GND |

**Important**: 
- SCK, SDA, RST are **shared** - can use same wire or jumper from left eye
- Only DC and CS are **unique** per eye

**Test**: Both displays should power on together

---

### Setup 4: Nose Sensor (HC-SR04)

**Position**: Place sensor in center of breadboard between eyes

**Wire Connections** (4 wires):

| Sensor Pin | Wire Color | Connect To | ESP32 Pin |
|------------|------------|------------|-----------|
| **TRIG** | Purple | Trigger Output | GPIO 25 |
| **ECHO** | Gray | Echo Input | GPIO 34 |
| **VCC** | Red | Breadboard + rail | 3.3V or 5V* |
| **GND** | Black | Breadboard - rail | GND |

** Voltage Note**: HC-SR04 typically uses 5V, but most work at 3.3V with reduced range. If you have 5V available, use it for VCC. ECHO pin is 5V tolerant on ESP32-S3.

**Test**: Sensor should emit ultrasonic pulses (inaudible, but pets may react!)

---

## Final Breadboard Configuration

### Physical Layout (Top View)

```
     
                                                          
       Left Eye       HC-SR04         Right Eye          
       GC9A01         Sensor          GC9A01             
                                   
         O                     O             
                                   
                                                      
                         
                                                       
              [Breadboard]                               
                 
        + Rail (3.3V)           
                                                      
        [Left Eye]  [Sensor]  [Right Eye]            
         7 wires     4 wires    7 wires              
                                                      
        - Rail (GND)           
                 
                                                        
                          Ribbon Cable                  
                                                        
                                        
                    ESP32-S3                           
                   DevKitC-1                           
                                        
                                                        
                      USB Cable                          
                                                        
                    Computer                             
     
```

### Wire Count Summary

| Connection Type | Wires | Notes |
|----------------|-------|-------|
| **SPI Bus Shared** | 3 | CLK, MOSI, RST (to both eyes) |
| **Left Eye Unique** | 2 | DC, CS |
| **Right Eye Unique** | 2 | DC, CS |
| **Power (Eyes)** | 4 | VCC, GND x2 displays |
| **Nose Sensor** | 4 | TRIG, ECHO, VCC, GND |
| **Power Rails** | 2 | 3.3V, GND from ESP32 |
| **USB Power** | 1 | To ESP32 |
| **Total** | 18 wires | Plus USB cable |

---

## Connection Checklist

### Before Powering On

- [ ] **ESP32 3V3  Breadboard + rail** (red wire)
- [ ] **ESP32 GND  Breadboard - rail** (black wire)

### Left Eye Display

- [ ] **Display SCK  ESP32 GPIO 14** (orange)
- [ ] **Display SDA  ESP32 GPIO 13** (yellow)
- [ ] **Display DC  ESP32 GPIO 27** (green)
- [ ] **Display CS  ESP32 GPIO 15** (blue)
- [ ] **Display RST  ESP32 GPIO 26** (white)
- [ ] **Display VCC  Breadboard +3.3V** (red)
- [ ] **Display GND  Breadboard GND** (black)

### Right Eye Display

- [ ] **Display SCK  ESP32 GPIO 14** (shared with left)
- [ ] **Display SDA  ESP32 GPIO 13** (shared with left)
- [ ] **Display DC  ESP32 GPIO 33** (green)
- [ ] **Display CS  ESP32 GPIO 32** (blue)
- [ ] **Display RST  ESP32 GPIO 26** (shared with left)
- [ ] **Display VCC  Breadboard +3.3V** (red)
- [ ] **Display GND  Breadboard GND** (black)

### Nose Sensor

- [ ] **Sensor TRIG  ESP32 GPIO 25** (purple)
- [ ] **Sensor ECHO  ESP32 GPIO 34** (gray)
- [ ] **Sensor VCC  Breadboard +3.3V or 5V** (red)
- [ ] **Sensor GND  Breadboard GND** (black)

---

## Testing Procedure

### Test 1: Power Check

1. **Connect USB cable** to ESP32-S3
2. **Check power LED** on ESP32 (should illuminate)
3. **Measure voltage** between breadboard rails (should be 3.3V)
4. **Check display backlights** (both eyes should glow)

**Expected**: All components powered, no smoke! 

---

### Test 2: Serial Monitor

1. **Open PlatformIO** or Arduino IDE
2. **Select ESP32-S3 port** (usually COM3-COM20 on Windows)
3. **Open serial monitor** (115200 baud)
4. **Press reset button** on ESP32

**Expected**: Boot messages appear (even without firmware uploaded)

---

### Test 3: Display Initialization (After Flashing Firmware)

1. **Flash test firmware** (we'll create this next session)
2. **Watch serial output** for initialization messages:
   ```
   [INFO] Left eye initializing...
   [INFO] Left eye: 240x240 pixels detected
   [INFO] Right eye initializing...
   [INFO] Right eye: 240x240 pixels detected
   ```
3. **Displays should show test pattern** (color bars or circles)

**Expected**: Both displays render graphics

---

### Test 4: Sensor Reading (After Flashing Firmware)

1. **Wave hand** in front of HC-SR04 sensor
2. **Watch serial output** for distance readings:
   ```
   [INFO] Distance: 15 cm
   [INFO] Distance: 12 cm
   [INFO] Distance: 8 cm
   ```
3. **Eyes should react** to distance changes (mood system)

**Expected**: Sensor detects objects, eyes respond

---

## Troubleshooting Guide

### Problem: Displays Don't Power On

**Check**:
- [ ] VCC connected to 3.3V rail (not 5V - will damage display!)
- [ ] GND connected to ground rail
- [ ] ESP32 USB powered
- [ ] Breadboard power rails continuous (no gaps)

**Solution**: Verify power connections with multimeter

---

### Problem: One Display Works, Other Doesn't

**Check**:
- [ ] Both displays share SCK, SDA, RST (3 wires common)
- [ ] Each display has unique DC and CS pins
- [ ] No loose connections on non-working display
- [ ] Both displays have VCC and GND connected

**Solution**: Check unique pins (DC, CS) for working vs non-working display

---

### Problem: Sensor Doesn't Detect

**Check**:
- [ ] TRIG connected to GPIO 25 (output)
- [ ] ECHO connected to GPIO 34 (input only pin)
- [ ] Sensor VCC at 3.3V or 5V (not floating)
- [ ] Sensor facing forward (not obstructed)

**Solution**: Try 5V power if available, GPIO 34 is input-only (correct for ECHO)

---

### Problem: Garbled Display Output

**Check**:
- [ ] SPI clock speed not too fast (try 10 MHz first)
- [ ] Wires not too long (keep under 6 inches)
- [ ] No crosstalk between wires (separate power from signals)
- [ ] Correct CS pin selected in code

**Solution**: Lower SPI speed, shorten wires, check pin assignments in code

---

### Problem: ESP32 Resets Randomly

**Check**:
- [ ] USB cable good quality (not charging-only cable)
- [ ] Total current < 500mA (2 displays + sensor = ~150mA)
- [ ] No short circuits (check with multimeter)
- [ ] Breadboard connections solid (no intermittent contact)

**Solution**: Use powered USB hub if current insufficient

---

## GPIO Pin Reference Card

Print this and keep near your workbench:

```

         P32 GOBLIN HEAD - GPIO QUICK REFERENCE            

  GPIO 14    SPI_CLK     Orange    SCK (both eyes)     
  GPIO 13    SPI_MOSI    Yellow    SDA (both eyes)     
  GPIO 26    RESET       White     RST (both eyes)     

  GPIO 27    DC          Green     Left eye only       
  GPIO 15    CS          Blue      Left eye only       

  GPIO 33    DC          Green     Right eye only      
  GPIO 32    CS          Blue      Right eye only      

  GPIO 25    TRIG        Purple    Nose sensor TX      
  GPIO 34    ECHO        Gray      Nose sensor RX      

  3V3        Power       Red       All components      
  GND        Ground      Black     All components      


Total GPIO: 8 pins | Total Wires: 18 (+ USB)
```

---

## Breadboard Tips

### Wire Management

1. **Use consistent colors** (follow guide above)
2. **Keep wires short** (6 inches or less for signals)
3. **Route power separately** (avoid crosstalk)
4. **Label connections** (masking tape + marker)

### Component Placement

1. **Eyes on sides** (matching goblin skull positions)
2. **Sensor in center** (between eyes, like nose)
3. **ESP32 below** (easy USB access)
4. **Keep displays stable** (they're heavy - use tape if needed)

### Debugging Aids

1. **Multimeter** - Check voltages (3.3V, GND)
2. **LED test** - Blink LED on GPIO before connecting component
3. **Serial logging** - Every component logs initialization status
4. **Oscilloscope** - If available, check SPI signals

---

## Next Steps After Wiring

### 1. Validate Physical Connections 
- [ ] All power rails connected
- [ ] All signal wires connected
- [ ] No short circuits (multimeter continuity test)

### 2. Flash Test Firmware
- [ ] Compile minimal display test
- [ ] Upload to ESP32-S3
- [ ] Verify serial output

### 3. Test Individual Components
- [ ] Left eye displays test pattern
- [ ] Right eye displays test pattern
- [ ] Sensor reads distance
- [ ] All components respond

### 4. Test Integrated System
- [ ] Both eyes render simultaneously
- [ ] Sensor data affects eye expressions
- [ ] Mood system responds to proximity
- [ ] Component timing works (hitCount)

---

## Photo Documentation Checklist

**Take photos for your build log**:

- [ ] Breadboard top view (all components visible)
- [ ] ESP32 GPIO connections (close-up)
- [ ] Left eye wiring (all 7 connections)
- [ ] Right eye wiring (all 7 connections)
- [ ] Sensor wiring (all 4 connections)
- [ ] Power rails (show 3.3V/GND distribution)
- [ ] Complete setup with USB cable

---

## Safety Notes

 **Important**:

1. **3.3V ONLY** for displays - 5V will damage them permanently
2. **GPIO 34 is INPUT ONLY** - Perfect for HC-SR04 ECHO, don't use for output
3. **Current limit** - Breadboard and ESP32 USB combined = max 500mA
4. **No hot swapping** - Power off before connecting/disconnecting components
5. **Check polarity** - VCC/GND reversed will destroy components

---

## Bill of Materials (BOM)

| Item | Quantity | Purpose | Cost Estimate |
|------|----------|---------|---------------|
| ESP32-S3 DevKitC-1 | 1 | Controller | $10 |
| GC9A01 Display | 2 | Eyes | $6 each |
| HC-SR04 Sensor | 1 | Nose | $2 |
| Breadboard 830-point | 1 | Prototyping | $5 |
| Jumper Wires M-M | 20 | Connections | $3 |
| USB Cable | 1 | Power/Program | $3 |
| **Total** | | | **~$35** |

---

## Related Documentation

 **[Component Pin Aliasing](COMPONENT-PIN-ALIASING-SPEC.md)** - Display pin variations  
 **[Hardware Testing Status](HARDWARE-TESTING-STATUS.md)** - Current build progress  
 **[Mouth Subsystem Architecture](MOUTH-SUBSYSTEM-ARCHITECTURE.md)** - Future expansion

---

## Summary

**Current Configuration**: Head subsystem with 2 eyes + 1 nose sensor  
**GPIO Usage**: 8 pins (26% of ESP32-S3 available GPIO)  
**Power Budget**: ~150mA (30% of USB 2.0 limit)  
**Performance**: 60 FPS capable per display  
**Status**:  Ready for firmware flashing and testing

**This is the foundation for your goblin head!** 

When speakers and microphones arrive, we'll add those to spare GPIO pins. The mouth display (with its onboard ESP32) will join as a separate wireless subsystem node.

---

*Guide Version: 1.0*  
*Date: October 15, 2025*  
*Author: P32 Animatronic Bot Project*  
*Next: Flash test firmware and validate all components*
