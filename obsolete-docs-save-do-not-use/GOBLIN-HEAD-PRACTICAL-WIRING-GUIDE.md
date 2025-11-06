#  P32 Goblin Head - Practical Wiring Guide (Hardware-Verified)

##  What You're Building

A mood-driven animatronic goblin head with:
- **2 Animated Displays** (left and right eyes)
- **Distance Sensing** (HC-SR04 proximity detection)
- **Audio Output** (MAX98357A amplifier + speaker)
- **Microphone Input** (HW-496 for ears/sound detection)
- **9 Emotional States** (automatic mood changes)

**Note:** Mouth display temporarily removed (can add later with 5 additional GPIO pins)

---

##  Complete Pin Mapping

###  GPIO Pin Assignments

| GPIO | Component | Function | Display Pin | Wire Color |
|------|-----------|----------|-------------|------------|
| **SPI Bus (Shared)** | | | | |
| 12 | Both Eyes | SPI MISO | SDO | Blue |
| 13 | Both Eyes | SPI MOSI | **SDA** | Green |
| 14 | Both Eyes | SPI CLK | **SCK** | Yellow |
| **Left Eye GC9A01** | | | | |
| 15 | Left Eye | Chip Select | **CS** | Orange |
| 18 | Left Eye | Data/Command | **DC** | Red |
| 21 | Left Eye | Reset | **RST** | Brown |
| **Right Eye GC9A01** | | | | |
| 16 | Right Eye | Chip Select | **CS** | Purple |
| 19 | Right Eye | Data/Command | **DC** | Pink |
| 22 | Right Eye | Reset | **RST** | White |
| **Audio I2S** | | | | |
| 4 | MAX98357A | I2S BCLK | BCLK | Gray |
| 5 | MAX98357A | I2S WS (LRCK) | LRC | Cyan |
| 6 | MAX98357A | I2S DATA | DIN | Magenta |
| **Distance Sensor** | | | | |
| 9 | HC-SR04 | TRIG | TRIG | Yellow/Black |
| 10 | HC-SR04 | ECHO | ECHO | Yellow/White |
| **Microphone** | | | | |
| 35 | HW-496 | Analog Out | AO | Red/White |
| 36 | HW-496 | Digital Out | DO | Red/Black |

**Total GPIO Used:** 16 pins  
**Available for Expansion:** 29+ pins (including future mouth display)

---

##  GC9A01 Display Pin Mapping (CRITICAL!)

Your displays expose these pins:
```
Display Pins:     ESP32 Function:     GPIO:

 SCK    SPI CLK         GPIO 14  (Yellow - Shared)
 SDA    SPI MOSI        GPIO 13  (Green - Shared)
 DC     Data/Command    GPIO 18/19  (Eye-specific)
 CS     Chip Select     GPIO 15/16  (Eye-specific)
 RST    Reset           GPIO 21/22  (Eye-specific)
 VCC    5V Power Rail           
 GND    Ground Rail             


NOTE: Your displays don't expose SDO (MISO), but we still
      connect GPIO 12 to complete the SPI bus.
```

**Pin Name Translation:**
- **SCK** = SPI Clock (same as CLK)
- **SDA** = SPI MOSI (Master Out, Slave In - same as DATA)
- **DC** = Data/Command select (same as A0 or RS on some displays)
- **CS** = Chip Select (same as SS)
- **RST** = Reset

---

##  Power Distribution System

```
Power Rail Architecture:

5V Supply  ESP32 VIN
            Left Eye VCC
            Right Eye VCC
            Audio Amp VCC
            HC-SR04 VCC
            HW-496 VCC

Ground  ESP32 GND
            Left Eye GND
            Right Eye GND
            Audio Amp GND
            HC-SR04 GND
            HW-496 GND
```

**Critical:** All components share common 5V and Ground rails

---

##  Assembly Order (Follow This Sequence!)

### 1 **Power Setup First**
```
Breadboard Layout:

 Red Rail (+5V)    [From USB/PSU]   
                                             
 Main Area         [ESP32 + Components]      
                   [Place components here]   
                                             
 Black Rail (GND)  [Common ground]  

```

**Test with multimeter:**
- Red rail = 5.0V  0.1V
- Continuity between all black rail points

---

### 2 **ESP32-S3 Connection**
```
ESP32 Power:
    VIN pin  Red power rail (+5V)
    GND pin  Black power rail (GND)
```
**Verify:** ESP32 LED lights up when powered

---

### 3 **Left Eye Display (GC9A01)**
```
Left Eye Wiring:
    Display Pin    ESP32 GPIO    Wire Color
    
    VCC            5V rail       Red
    GND            GND rail      Black
    SCK            GPIO 14       Yellow (shared)
    SDA            GPIO 13       Green (shared)
    DC             GPIO 18       Red
    CS             GPIO 15       Orange
    RST            GPIO 21       Brown
```

---

### 4 **Right Eye Display (GC9A01)**
```
Right Eye Wiring:
    Display Pin    ESP32 GPIO    Wire Color
    
    VCC            5V rail       Red
    GND            GND rail      Black
    SCK            GPIO 14       Yellow (shared)
    SDA            GPIO 13       Green (shared)
    DC             GPIO 19       Pink
    CS             GPIO 16       Purple
    RST            GPIO 22       White
```

**Note:** SCK and SDA are shared between both eyes (SPI bus)

---

### 5 **Audio System (MAX98357A + Speaker)**
```
I2S Audio Amplifier:
    MAX98357A Pin    ESP32 GPIO    Wire Color
    
    VIN              5V rail       Red
    GND              GND rail      Black
    BCLK             GPIO 4        Gray
    LRC (LRCK/WS)    GPIO 5        Cyan
    DIN              GPIO 6        Magenta
    
Speaker Connections:
    MAX98357A +      Speaker +     Red
    MAX98357A -      Speaker -     Black
```

---

### 6 **Distance Sensor (HC-SR04)**
```
Ultrasonic Sensor:
    HC-SR04 Pin    ESP32 GPIO    Wire Color
    
    VCC            5V rail       Red
    GND            GND rail      Black
    TRIG           GPIO 9        Yellow/Black
    ECHO           GPIO 10       Yellow/White
```

**IMPORTANT:** HC-SR04 requires full 5V (not 3.3V)

---

### 7 **Microphone Module (HW-496)**
```
Microphone Input (for ears):
    HW-496 Pin    ESP32 GPIO    Wire Color
    
    VCC           5V rail       Red
    GND           GND rail      Black
    AO            GPIO 35       Red/White (analog)
    DO            GPIO 36       Red/Black (digital)
```

**Features:**
- Analog output (AO): Sound amplitude level
- Digital output (DO): Binary sound detection (adjustable threshold via potentiometer)
- Onboard sensitivity adjustment

---

##  Testing Checklist

###  Power Verification (Use Multimeter)
- [ ] 5.0V present on ESP32 VIN
- [ ] 5.0V on left eye VCC
- [ ] 5.0V on right eye VCC
- [ ] 5.0V on audio amp VCC
- [ ] 5.0V on HC-SR04 VCC
- [ ] 5.0V on HW-496 VCC
- [ ] Continuity on all GND connections
- [ ] NO continuity between 5V and GND (check for shorts!)

###  Component Continuity Tests
- [ ] GPIO 14 connected to both display SCK pins
- [ ] GPIO 13 connected to both display SDA pins
- [ ] All individual CS/DC/RST pins connected correctly
- [ ] I2S pins (4,5,6) connected to audio amp
- [ ] Sensor pins (9,10) connected to HC-SR04
- [ ] Microphone pins (35,36) connected to HW-496

###  Software Configuration
- [ ] `goblin_full.json` has mouth component removed
- [ ] `ENABLE_GOBLIN_COMPONENTS` defined in config
- [ ] PlatformIO project builds without errors
- [ ] Upload successful to ESP32
- [ ] Serial monitor shows boot messages

###  Functional Tests
- [ ] Left eye display shows content
- [ ] Right eye display shows content
- [ ] Both eyes animate (blink cycle, mood colors)
- [ ] Speaker produces clear audio
- [ ] Distance sensor readings change with proximity
- [ ] Microphone reacts to sound (LED or serial output)
- [ ] Mood system cycles through emotions

---

##  Component Specifications

### ESP32-S3-DevKitC-1
- **MCU:** Dual-core Xtensa LX7 @ 240MHz
- **RAM:** 512KB SRAM
- **Flash:** 8MB
- **ADC:** 2x 12-bit SAR ADC (for microphone)
- **GPIO:** 45 pins (16 used, 29+ available)

### GC9A01 Round Display (2)
- **Size:** 1.28" diameter
- **Resolution:** 240240 pixels
- **Interface:** SPI 4-wire + DC/RST
- **Exposed Pins:** SCK, SDA, DC, CS, RST, VCC, GND
- **Colors:** 65,536 (RGB565)
- **Voltage:** 3.3V logic, 5V power

### MAX98357A I2S Audio Amplifier
- **Interface:** I2S digital input
- **Output:** 3W @ 4, 1.8W @ 8
- **SNR:** 92dB
- **Sample Rate:** Up to 96kHz
- **Voltage:** 2.5V-5.5V (use 5V)

### HC-SR04 Ultrasonic Distance Sensor
- **Range:** 2cm to 400cm
- **Accuracy:** 3mm
- **Beam Angle:** 15 degrees
- **Interface:** Digital trigger/echo
- **Voltage:** 5V required (won't work at 3.3V)

### HW-496 Electret Microphone Module
- **Type:** Electret condenser microphone
- **Outputs:** Analog (AO) + Digital (DO)
- **Sensitivity:** Adjustable via onboard potentiometer
- **Voltage:** 3.3V-5V compatible
- **Frequency Response:** 100Hz-10kHz

---

##  Troubleshooting Guide

### Display Shows Nothing (Black Screen)
**Check:**
1. 5V power on display VCC pins (measure with multimeter)
2. SCK (GPIO 14) and SDA (GPIO 13) connected to BOTH displays
3. CS pin unique to each display (GPIO 15 left, GPIO 16 right)
4. DC pin connected (GPIO 18 left, GPIO 19 right)
5. RST pin connected (GPIO 21 left, GPIO 22 right)

### Display Shows Garbled/Wrong Colors
**Check:**
1. SDA connected to display SDA (not SDO - common mistake!)
2. SCK and SDA not swapped
3. DC pin connected (controls data vs command mode)

### No Audio Output
**Check:**
1. 5V power to MAX98357A VIN
2. I2S pins correct: GPIO 4 (BCLK), GPIO 5 (LRC), GPIO 6 (DIN)
3. Speaker polarity (though usually doesn't matter for testing)
4. Audio amplifier GAIN pin (leave floating for 9dB gain)

### Sensor Always Shows Same Distance
**Check:**
1. HC-SR04 getting full 5V (measure VCC pin)
2. TRIG on GPIO 9, ECHO on GPIO 10
3. No obstructions in front of sensor (needs clear line of sight)
4. Sensor pointing forward (has directional beam)

### Microphone Not Responding
**Check:**
1. Power connected (VCC/GND)
2. AO connected to GPIO 35 (analog input)
3. DO connected to GPIO 36 (digital input)
4. Try adjusting sensitivity potentiometer on HW-496 module
5. Test with loud sound (clap near microphone)

### Build/Upload Errors
**Check:**
1. USB-C cable connected to ESP32 (data cable, not charge-only)
2. Correct COM port selected in PlatformIO
3. `goblin_full.json` has mouth component removed (temporary)
4. ESP32-S3 drivers installed (CP210x or CH340)

---

##  Adding Mouth Display Later

When you get a display with exposed pins, add these connections:

```
Future Mouth Display (GC9A01):
    Display Pin    ESP32 GPIO    Notes
    
    VCC            5V rail
    GND            GND rail
    SCK            GPIO 14       Shared with eyes
    SDA            GPIO 13       Shared with eyes
    DC             GPIO 20       New
    CS             GPIO 17       New
    RST            GPIO 23       New
```

Then add back to `goblin_full.json`:
```json
"config/components/positioned/goblin/head_components/goblin_mouth.json"
```

---

##  Expansion Ideas (29+ GPIO Available!)

### Immediate Additions
- **Neck Servos:** GPIO 7, 8 (PWM for pan/tilt)
- **Status LEDs:** GPIO 11, 47, 48 (RGB indicators)
- **Touch Sensors:** GPIO 1, 2, 3 (capacitive touch)

### Advanced Expansions
- **WS2812B LED Strips:** GPIO 40-45 (accent lighting)
- **Additional Sensors:** GPIO 37-39 (temperature, humidity, light)
- **Motor Controllers:** GPIO 26-27 (H-bridge for mobility)
- **Camera Module:** Use separate ESP32-CAM and mesh networking

---

##  Support Resources

- **GitHub Repo:** [p32-animatronic-bot](https://github.com/reussered/p32-animatronic-bot)
- **ESP32-S3 Pinout:** [Espressif Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/)
- **Component Datasheets:** Manufacturer websites
- **Session Docs:** `SESSION-STARTUP.md` for architecture overview

---

** Happy Building! Your goblin awaits...**

*This guide reflects the actual hardware you have. Print for workbench reference.*
