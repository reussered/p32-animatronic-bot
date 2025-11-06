# P32 Goblin Full - Printable Build Guide
*High-Quality Visual Assembly Instructions*

---

## Print Settings for Best Results

**Recommended Print Settings:**
- **Paper:** 8.5" x 11" (US Letter) or A4  
- **Orientation:** Portrait
- **Color:** Full color recommended for wiring diagrams
- **Pages:** Print all sections, approximately 15 pages
- **Binding:** Three-hole punch for binder assembly

---

## Quick Reference Component List

| Component | Quantity | Key Specs |
|-----------|----------|-----------|
| ESP32-S3-DevKitC-1 | 1 | Main controller |
| GC9A01 Round Display | 3 | 240x240px, SPI |
| MAX98357A Audio Amp | 1 | I2S digital input |
| HC-SR04 Sensor | 1 | Ultrasonic distance |
| 4 Speaker | 1 | 3W, 40mm diameter |
| 5V Power Supply | 1 | 2A minimum |
| Breadboard | 1 | 830 tie points |
| Jumper Wires | 1 set | Assorted colors |

---

## Critical Wiring Summary

### Complete Visual Wiring Diagram

![P32 Goblin Wiring Diagram](goblin_wiring_diagram.svg)

*Full system connection diagram showing all components, GPIO assignments, and power distribution*

### Power Distribution (Most Important!)
```
5V Supply  Breadboard Red Rail  All Component VCC pins
Ground  Breadboard Black Rail  All Component GND pins  
ESP32 VIN  Breadboard Red Rail
ESP32 GND  Breadboard Black Rail
```

### GPIO Pin Chart (Print This Page!)

| GPIO | Component | Function | Wire Color |
|------|-----------|----------|------------|
| 4 | Audio Amp | I2S BCLK | Red |
| 5 | Audio Amp | I2S WS | Black |
| 6 | Audio Amp | I2S DATA | White |
| 9 | HC-SR04 | TRIG | Gray |
| 10 | HC-SR04 | ECHO | Pink |
| 12 | All Displays | SPI MISO | Blue |
| 13 | All Displays | SPI MOSI | Green |
| 14 | All Displays | SPI CLK | Yellow |
| 15 | Left Eye | CS | Orange |
| 16 | Right Eye | CS | Purple |
| 17 | Mouth | CS | Brown |

### ESP32-S3 GPIO Pin Reference

```
         ESP32-S3-DevKitC-1 Pinout (Key Pins Used)
    
      VIN                                 3.3V 
      GND                                 RST  
      IO3                                 IO46 
      IO4   I2S BCLK (Audio)             IO0  
      IO5   I2S WS (Audio)               IO45 
      IO6   I2S DATA (Audio)             IO48 
      IO7                                 IO47 
      IO15  SPI CS1 (Left Eye)           IO21 
      IO16  SPI CS2 (Right Eye)          IO14  SPI CLK (All)
      IO17  SPI CS3 (Mouth)              IO13  SPI MOSI (All)
      IO18                                IO12  SPI MISO (All)
      IO8                                 IO11 
      IO19                                IO10  Sensor ECHO
      IO20                                IO9   Sensor TRIG
    
```

---

## Step-by-Step Assembly

### Step 1: Power Setup
**[INSERT HIGH-RES PHOTO: Breadboard with power rails connected]**

1. Connect 5V supply positive to breadboard red rail
2. Connect 5V supply ground to breadboard black rail  
3. Connect ESP32 VIN to breadboard red rail
4. Connect ESP32 GND to breadboard black rail

**Critical:** Verify 5V on red rail with multimeter before proceeding!

### Step 2: Display Wiring

```
SPI Bus Sharing Architecture:
    
    ESP32 GPIO 12  All Display SDO (MISO)
    ESP32 GPIO 13  All Display SDA (MOSI)  
    ESP32 GPIO 14  All Display SCL (CLK)
                                 
    ESP32 GPIO 15                     Left Eye CS (Orange wire)
    ESP32 GPIO 16            Right Eye CS (Purple wire)
    ESP32 GPIO 17  Mouth CS (Brown wire)
                                 
           Left Eye                     GC9A01 Display
          Right Eye            GC9A01 Display  
             Mouth  GC9A01 Display
```

**Shared SPI Bus (connect to ALL displays):**
- GPIO 12 (blue)  SDO pin on all displays
- GPIO 13 (green)  SDA pin on all displays
- GPIO 14 (yellow)  SCL pin on all displays

**Individual Chip Select:**
- GPIO 15 (orange)  Left eye CS pin
- GPIO 16 (purple)  Right eye CS pin
- GPIO 17 (brown)  Mouth CS pin

**Power & Control:**
- Red rail  VCC on all displays
- Black rail  GND on all displays  
- 3.3V  RES pin on all displays
- 3.3V  DC pin on all displays

### Step 3: Audio System
**[INSERT HIGH-RES PHOTO: Audio amplifier and speaker wiring]**

**I2S Digital Audio:**
- GPIO 4 (red)  BCLK pin
- GPIO 5 (black)  LRCK pin  
- GPIO 6 (white)  DIN pin

**Power & Output:**
- Red rail  VCC pin
- Black rail  GND pin
- + terminal  Speaker positive
- - terminal  Speaker negative

### Step 4: Distance Sensor
**[INSERT HIGH-RES PHOTO: HC-SR04 sensor mounted and wired]**

**Digital Interface:**
- GPIO 9 (gray)  TRIG pin
- GPIO 10 (pink)  ECHO pin

**Power:**
- Red rail  VCC pin (5V required)
- Black rail  GND pin

---

## Physical Mounting Positions

**[INSERT HIGH-RES PHOTO: Assembled goblin head showing component positions]**

### 3D Coordinate System (Nose Center = Origin)

| Component | X Position | Y Position | Z Position |
|-----------|------------|------------|------------|
| Left Eye | -1.05" | +0.7" | -0.35" |
| Right Eye | +1.05" | +0.7" | -0.35" |
| Mouth | 0" | -1.05" | 0" |
| Nose Sensor | 0" | 0" | +0.25" |
| Speaker | -0.5" | +0.5" | -1.0" |

**Key Measurements:**
- Eye spacing: 3.0" center-to-center
- Total face width: ~4.5"  
- Face height: ~3.5"
- Sensor protrusion: 0.25" from face

**[INSERT HIGH-RES PHOTO: Dimensional diagram with ruler for scale]**

---

## Software Installation Checklist

### Prerequisites
 Visual Studio Code installed  
 PlatformIO extension installed  
 Git for Windows installed  
 USB-C cable available

### Project Setup
 Clone repository: `git clone https://github.com/reussered/p32-animatronic-bot.git`  
 Open folder in VS Code  
 Verify PlatformIO detects project  
 Check `src/p32_component_config.h` contains: `#define ENABLE_GOBLIN_COMPONENTS`

### Build & Upload
 Run: `pio run` (should complete successfully)  
 Connect ESP32-S3 via USB-C  
 Run: `pio run -t upload`  
 Start monitor: `pio device monitor`

**Expected Results:**
- Build: ~5.8% RAM, ~51.7% Flash usage
- Upload: Successful to detected COM port  
- Monitor: Loop messages showing all components active

**[INSERT HIGH-RES PHOTO: VS Code showing successful build output]**

---

## Testing & Verification

### Visual Confirmation
 All three displays show content  
 Eyes display blinking animation  
 Mouth displays color changes  
 Colors change automatically over time

**[INSERT HIGH-RES PHOTO: All three displays active with different colors]**

### Audio Confirmation  
 Speaker produces startup sounds  
 Audio quality is clear (not distorted)  
 Volume level appropriate

### Sensor Confirmation
 Distance readings change when objects approach nose  
 Serial monitor shows changing distance values  
 System responds to proximity with mood changes

**[INSERT HIGH-RES PHOTO: Serial monitor showing sensor readings]**

---

## Mood System Reference

The goblin cycles through 9 emotional states:

| Emotion | Color Theme | Animation Style | Trigger |
|---------|-------------|-----------------|---------|
| FEAR | Pale/White | Rapid movement | Sudden proximity |
| ANGER | Red | Intense stare | Sustained proximity |  
| IRRITATION | Orange | Narrowed eyes | Repeated interaction |
| HAPPINESS | Bright/Yellow | Wide eyes | Positive interaction |
| CONTENTMENT | Soft/Blue | Relaxed | Extended calm |
| HUNGER | Green | Searching | Time-based |
| CURIOSITY | Blue | Alert | Motion detection |
| AFFECTION | Pink | Gentle gaze | Gentle interaction |
| EXCITEMENT | Flashing | Rapid changes | High activity |

**[INSERT HIGH-RES PHOTO: Grid showing each emotion's display appearance]**

---

## Common Problems & Solutions

### No Display Output
**Problem:** Black screens on all displays  
**Check:** Power connections, SPI wiring, CS pin assignments  
**Solution:** Verify 5V power and individual CS pins

**[INSERT HIGH-RES PHOTO: Multimeter checking 5V on display VCC pin]**

### Compilation Errors  
**Problem:** Build fails with "undefined reference" errors  
**Check:** `p32_component_config.h` file  
**Solution:** Ensure `#define ENABLE_GOBLIN_COMPONENTS` is present

### No Audio Output
**Problem:** Silent speaker  
**Check:** I2S wiring, amplifier power, speaker connections  
**Solution:** Verify GPIO 4,5,6 connections and 5V power to amplifier

### Sensor Not Responding
**Problem:** Fixed distance readings  
**Check:** 5V power (3.3V won't work), GPIO 9,10 connections  
**Solution:** HC-SR04 requires full 5V, verify with multimeter

**[INSERT HIGH-RES PHOTO: Troubleshooting setup with multimeter and test points]**

---

## Safety & Best Practices

### Electrical Safety
- Always disconnect power when making wiring changes
- Use multimeter to verify voltages before connecting components  
- Install 1A fuse in 5V supply line for overcurrent protection
- Keep work area clean and organized

### Component Handling
- Handle ESP32-S3 with anti-static precautions
- Store unused components in anti-static bags  
- Double-check wiring before applying power
- Use color-coded wires consistently

**[INSERT HIGH-RES PHOTO: Organized workspace with tools and components]**

---

## Expansion Ideas

With 26+ unused GPIO pins, consider adding:
- **Servo-controlled neck movement** (PWM on GPIO 18,19)  
- **Camera module** (I2C or SPI interface)
- **Microphone input** (I2S or ADC)
- **Additional displays** (expand SPI bus)
- **LED strips** (WS2812B on data pins)
- **Wheeled base** (motor controllers)

**[INSERT HIGH-RES PHOTO: Example expanded system with additional components]**

---

## Final Assembly Photos

**[INSERT HIGH-RES PHOTO: Completed goblin head - front view]**
**[INSERT HIGH-RES PHOTO: Completed goblin head - side view showing wiring]**  
**[INSERT HIGH-RES PHOTO: Completed goblin head - back view showing connections]**
**[INSERT HIGH-RES PHOTO: System in operation showing animated displays]**

---

## Contact & Support

- **GitHub Issues:** Report problems at repository
- **Documentation:** Complete specs in `/docs` folder  
- **Community:** Project forums and discussion groups
- **Hardware Support:** Component manufacturer resources

**Build completion checklist:**
 All components powered and functional  
 Serial monitor shows continuous operation  
 All three displays animate correctly  
 Audio output clear and appropriate volume  
 Distance sensor responds to proximity  
 Mood system cycles through emotional states  

**Congratulations! Your P32 Goblin Full animatronic is complete!**

---

*This guide is designed for high-quality printing. For digital viewing and additional resources, see the complete documentation at: https://github.com/reussered/p32-animatronic-bot*