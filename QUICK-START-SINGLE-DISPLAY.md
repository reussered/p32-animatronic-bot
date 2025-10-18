# Quick Start - Single GC9A01 Display

## ✅ Current System Status

**Everything is working perfectly!**
- ESP32-S3 communicating via COM4
- Loop timing validated: ~762k loops/sec
- LED ON/OFF messages scrolling at 1 Hz in serial monitor ✓
- System ready for hardware

---

## 🔌 Connect Your First GC9A01 Display

### What You Need
- 1x GC9A01 round display (240x240 pixels)
- 7 jumper wires (female-to-female recommended)
- ESP32-S3 DevKitC-1 (already connected via USB)

### Pin Connections

```
GC9A01 Display          ESP32-S3 Pin
─────────────────       ────────────
VCC (Power)      →      3.3V
GND (Ground)     →      GND
SCK (SPI Clock)  →      GPIO 14
SDA (SPI MOSI)   →      GPIO 13
RES (Reset)      →      GPIO 26
DC (Data/Cmd)    →      GPIO 27
CS (Chip Select) →      GPIO 15
```

### Visual Wiring Diagram

```
     ESP32-S3 DevKitC-1                    GC9A01 Display
     ┌─────────────────┐                   ┌──────────┐
     │                 │                   │  ╭────╮  │
     │     [USB-C]     │                   │ │    │  │
     │                 │                   │ │ ●● │  │  ← Round LCD
     │                 │                   │ │    │  │
     │                 │                   │  ╰────╯  │
     │                 │                   │          │
 3.3V├─── (Red) ───────┼───────────────────┤ VCC     │
  GND├─── (Black) ─────┼───────────────────┤ GND     │
     │                 │                   │          │
  G14├─── (Orange) ────┼───────────────────┤ SCK     │
  G13├─── (Yellow) ────┼───────────────────┤ SDA     │
  G26├─── (White) ─────┼───────────────────┤ RES     │
  G27├─── (Green) ─────┼───────────────────┤ DC      │
  G15├─── (Blue) ──────┼───────────────────┤ CS      │
     │                 │                   │          │
     └─────────────────┘                   └──────────┘
```

### Wire Color Guide (Suggested)
- **Red**: 3.3V Power
- **Black**: Ground
- **Orange**: SCK (Clock)
- **Yellow**: SDA (Data)
- **White**: RES (Reset)
- **Green**: DC (Data/Command)
- **Blue**: CS (Chip Select)

---

## 📋 Step-by-Step Connection

### 1. **Power OFF** (disconnect USB first!)

### 2. Connect Power Rails
```
ESP32-S3 3.3V  →  GC9A01 VCC (red wire)
ESP32-S3 GND   →  GC9A01 GND (black wire)
```

### 3. Connect SPI Bus (Shared Signals)
```
ESP32-S3 GPIO 14  →  GC9A01 SCK (orange wire)
ESP32-S3 GPIO 13  →  GC9A01 SDA (yellow wire)
```

### 4. Connect Control Signals
```
ESP32-S3 GPIO 26  →  GC9A01 RES (white wire)
ESP32-S3 GPIO 27  →  GC9A01 DC  (green wire)
ESP32-S3 GPIO 15  →  GC9A01 CS  (blue wire)
```

### 5. **Power ON** (reconnect USB)

---

## 🎯 Pin Function Explanation

| Pin | Function | Why We Need It |
|-----|----------|----------------|
| **VCC** | 3.3V Power | Powers the display |
| **GND** | Ground | Common ground reference |
| **SCK** | SPI Clock | Synchronizes data transmission |
| **SDA** | SPI Data | Sends pixel data to display |
| **RES** | Reset | Resets display on startup |
| **DC** | Data/Command | Tells display if we're sending commands or pixel data |
| **CS** | Chip Select | Enables this specific display (important when multiple displays) |

---

## ⚠️ Important Notes

### Power
- **ALWAYS use 3.3V, NEVER 5V!** 
- GC9A01 is NOT 5V tolerant
- ESP32-S3 3.3V pin can provide ~600mA (enough for one display)

### Pin Availability
- GPIO 14, 13 are SPI bus (can be shared with more displays)
- GPIO 15 is unique CS pin (each display needs its own CS)
- GPIO 26 is reset (can be shared across displays)
- GPIO 27 is DC (unique for this display)

### What This Gives You
- **Left Eye Display** position (per goblin_full.json)
- Display will show test patterns once firmware is updated
- Can add second display later (shares most pins, needs different CS and DC)

---

## 🚀 Next Steps (After Hardware Connected)

### 1. Update Configuration
Edit `config/bots/simple_test_bot.json` to include display component

### 2. Generate Tables
```powershell
python tools/generate_tables.py simple_test_bot src
```

### 3. Rebuild & Upload
```powershell
pio run -t upload
```

### 4. Watch Serial Monitor
You should see:
- Display initialization messages
- Test pattern rendering
- FPS performance metrics

---

## 📊 Expected Performance

With one GC9A01 display:
- **Resolution**: 240x240 pixels = 57,600 pixels
- **Update Rate**: 30-60 FPS possible
- **CPU Usage**: ~20-30% for rendering
- **Remaining CPU**: ~70% for sensors, audio, behaviors

---

## 🔍 Troubleshooting

### Display Stays Blank
- Check 3.3V power connection (measure with multimeter if available)
- Verify GND connection
- Check all 7 wires are firmly connected

### Display Shows Garbage/Static
- Check SCK and SDA connections (clock and data)
- Verify CS (chip select) connection
- Reset ESP32-S3 (press RESET button or reconnect USB)

### ESP32-S3 Won't Boot
- Disconnect display power (VCC)
- If ESP32-S3 boots, display might be faulty
- Check for shorts between wires

### Nothing in Serial Monitor
- Verify COM4 connection
- Press RESET button on ESP32-S3
- Reconnect USB cable

---

## 📁 Configuration Files Reference

### Display Component JSON (Future)
`config/components/positioned/goblin_left_eye.json`
- Defines display at left eye position
- References GPIO pins
- Sets update frequency (hitCount)

### SPI Bus Configuration
`config/components/interfaces/spi_bus_vspi.json`
- Already configured for GPIO 14 (SCK) and GPIO 13 (MOSI)

### SPI Device Configuration
`config/components/interfaces/spi_device_1.json`
- Already configured for GPIO 15 (CS)
- Already configured for GPIO 27 (DC)

---

## ✅ Safety Checklist

Before connecting power:
- [ ] All wires firmly seated
- [ ] No exposed wire touching other pins
- [ ] VCC goes to 3.3V (NOT 5V!)
- [ ] GND connected properly
- [ ] Display sitting on non-conductive surface
- [ ] USB cable ready to connect

---

## 🎨 What You'll See Eventually

Once firmware is updated:
1. **Initialization**: Display backlight turns on
2. **Test Pattern**: Solid color fill (red/green/blue)
3. **Animation**: Blinking eye or mood-driven expression
4. **Real-time**: Display updates based on sensor input

---

**Rest up! When ready, connect the display and we'll make it show something cool!** 😊

Hardware is the next exciting step! 🎉
