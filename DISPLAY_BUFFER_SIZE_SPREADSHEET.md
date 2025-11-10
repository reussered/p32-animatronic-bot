Display Buffer Size Reference - Complete Spreadsheet
=====================================================

QUICK LOOKUP TABLE - Buffer Size in Bytes

Display Model | Resolution (WxH) | Pixels | Color Depth | Bytes/Pixel | Total Bytes | KB | MB | Notes
---|---|---|---|---|---|---|---|---
GC9A01 | 240x240 | 57,600 | RGB565 | 2 | 115,200 | 112.5 | 0.1123 | Goblin eye
ST7789 | 240x320 | 76,800 | RGB565 | 2 | 153,600 | 150 | 0.1465 | Rectangular eye
ST7735 | 160x128 | 20,480 | RGB565 | 2 | 40,960 | 40 | 0.0391 | Budget display
ILI9341 | 480x320 | 153,600 | RGB666 | 3 | 460,800 | 450 | 0.4395 | Mouth display
ILI9486 | 480x320 | 153,600 | RGB888 | 3 | 460,800 | 450 | 0.4395 | High-color mouth
ILI9481 | 800x480 | 384,000 | RGB888 | 3 | 1,152,000 | 1,125 | 1.0986 | XL display
RA8875 | 800x480 | 384,000 | RGB666 | 3 | 1,152,000 | 1,125 | 1.0986 | Large with accel
SSD1306 | 128x64 | 8,192 | Monochrome | 0.125 | 1,024 | 1 | 0.001 | Status OLED
SSD1331 | 96x64 | 6,144 | RGB565 | 2 | 12,288 | 12 | 0.0117 | Micro RGB OLED
SSD1351 | 128x128 | 16,384 | RGB565 | 2 | 32,768 | 32 | 0.0313 | Small RGB OLED


CALCULATION BREAKDOWN - Each Display Type

GC9A01 (Goblin Eye Display)
  Width: 240 pixels
  Height: 240 pixels
  Total pixels: 240  240 = 57,600 pixels
  Color format: RGB565 (16-bit = 2 bytes per pixel)
  Bytes needed: 57,600 pixels  2 bytes/pixel = 115,200 bytes
  Kilobytes: 115,200 bytes  1,024 bytes/KB = 112.5 KB
  Megabytes: 115,200 bytes  1,048,576 bytes/MB = 0.1123 MB
  One frame buffer: 115,200 bytes


ST7789 (Rectangular Eye Alternative)
  Width: 240 pixels
  Height: 320 pixels
  Total pixels: 240  320 = 76,800 pixels
  Color format: RGB565 (16-bit = 2 bytes per pixel)
  Bytes needed: 76,800 pixels  2 bytes/pixel = 153,600 bytes
  Kilobytes: 153,600 bytes  1,024 bytes/KB = 150 KB
  Megabytes: 153,600 bytes  1,048,576 bytes/MB = 0.1465 MB
  One frame buffer: 153,600 bytes


ST7735 (Budget Display)
  Width: 160 pixels
  Height: 128 pixels
  Total pixels: 160  128 = 20,480 pixels
  Color format: RGB565 (16-bit = 2 bytes per pixel)
  Bytes needed: 20,480 pixels  2 bytes/pixel = 40,960 bytes
  Kilobytes: 40,960 bytes  1,024 bytes/KB = 40 KB
  Megabytes: 40,960 bytes  1,048,576 bytes/MB = 0.0391 MB
  One frame buffer: 40,960 bytes


ILI9341 (High-Resolution Mouth Display)
  Width: 480 pixels
  Height: 320 pixels
  Total pixels: 480  320 = 153,600 pixels
  Color format: RGB666 (18-bit packed = 3 bytes per pixel)
  Bytes needed: 153,600 pixels  3 bytes/pixel = 460,800 bytes
  Kilobytes: 460,800 bytes  1,024 bytes/KB = 450 KB
  Megabytes: 460,800 bytes  1,048,576 bytes/MB = 0.4395 MB
  One frame buffer: 460,800 bytes


ILI9486 (Large Color Display)
  Width: 480 pixels
  Height: 320 pixels
  Total pixels: 480  320 = 153,600 pixels
  Color format: RGB888 (24-bit = 3 bytes per pixel)
  Bytes needed: 153,600 pixels  3 bytes/pixel = 460,800 bytes
  Kilobytes: 460,800 bytes  1,024 bytes/KB = 450 KB
  Megabytes: 460,800 bytes  1,048,576 bytes/MB = 0.4395 MB
  One frame buffer: 460,800 bytes


ILI9481 (XL Display)
  Width: 800 pixels
  Height: 480 pixels
  Total pixels: 800  480 = 384,000 pixels
  Color format: RGB888 (24-bit = 3 bytes per pixel)
  Bytes needed: 384,000 pixels  3 bytes/pixel = 1,152,000 bytes
  Kilobytes: 1,152,000 bytes  1,024 bytes/KB = 1,125 KB
  Megabytes: 1,152,000 bytes  1,048,576 bytes/MB = 1.0986 MB
  One frame buffer: 1,152,000 bytes


RA8875 (Large Display with Graphics Acceleration)
  Width: 800 pixels
  Height: 480 pixels
  Total pixels: 800  480 = 384,000 pixels
  Color format: RGB666 (18-bit = 3 bytes per pixel)
  Bytes needed: 384,000 pixels  3 bytes/pixel = 1,152,000 bytes
  Kilobytes: 1,152,000 bytes  1,024 bytes/KB = 1,125 KB
  Megabytes: 1,152,000 bytes  1,048,576 bytes/MB = 1.0986 MB
  One frame buffer: 1,152,000 bytes


SSD1306 (Monochrome OLED Status Display)
  Width: 128 pixels
  Height: 64 pixels
  Total pixels: 128  64 = 8,192 pixels
  Color format: Monochrome (1-bit per pixel, packed 8 pixels per byte)
  Bytes needed: (8,192 pixels  8 pixels/byte) = 1,024 bytes
  Kilobytes: 1,024 bytes  1,024 bytes/KB = 1 KB
  Megabytes: 1,024 bytes  1,048,576 bytes/MB = 0.001 MB
  One frame buffer: 1,024 bytes


SSD1331 (Micro RGB OLED)
  Width: 96 pixels
  Height: 64 pixels
  Total pixels: 96  64 = 6,144 pixels
  Color format: RGB565 (16-bit = 2 bytes per pixel)
  Bytes needed: 6,144 pixels  2 bytes/pixel = 12,288 bytes
  Kilobytes: 12,288 bytes  1,024 bytes/KB = 12 KB
  Megabytes: 12,288 bytes  1,048,576 bytes/MB = 0.0117 MB
  One frame buffer: 12,288 bytes


SSD1351 (Small RGB OLED)
  Width: 128 pixels
  Height: 128 pixels
  Total pixels: 128  128 = 16,384 pixels
  Color format: RGB565 (16-bit = 2 bytes per pixel)
  Bytes needed: 16,384 pixels  2 bytes/pixel = 32,768 bytes
  Kilobytes: 32,768 bytes  1,024 bytes/KB = 32 KB
  Megabytes: 32,768 bytes  1,048,576 bytes/MB = 0.0313 MB
  One frame buffer: 32,768 bytes


GOBLIN HEAD CONFIGURATION OPTIONS - Total Memory Required

Option A: Dual GC9A01 Eyes + ST7735 Mouth + SSD1306 Status

Left Eye (GC9A01): 115,200 bytes
Right Eye (GC9A01): 115,200 bytes
Mouth (ST7735): 40,960 bytes
Status (SSD1306): 1,024 bytes

Total: 272,384 bytes = 266 KB = 0.266 MB

Fits in 300KB? YES (with 27.6 KB headroom)
Frame rate: 60 FPS all displays
Recommendation: BEST OPTION - Simple, no chunking


Option B: Dual GC9A01 Eyes + ST7789 Mouth + SSD1306 Status

Left Eye (GC9A01): 115,200 bytes
Right Eye (GC9A01): 115,200 bytes
Mouth (ST7789): 153,600 bytes
Status (SSD1306): 1,024 bytes

Total: 385,024 bytes = 376 KB = 0.376 MB

Fits in 300KB? NO (exceeds by 76 KB)
Workaround: Need chunking or sequential rendering


Option C: Dual GC9A01 Eyes + Chunked ILI9341 Mouth (4 chunks)

Left Eye (GC9A01): 115,200 bytes
Right Eye (GC9A01): 115,200 bytes
Mouth chunk (480x80): 115,200 bytes
Status (SSD1306): 1,024 bytes

Total: 346,624 bytes = 338 KB = 0.339 MB

Peak allocation: 346,624 bytes
Fits in 300KB? NO (exceeds by 38.6 KB, but manageable with buffer sharing)
Frame rate: Eyes 60 FPS, Mouth 15 FPS (chunked)
Recommendation: If you need high-res mouth


Option D: Dual GC9A01 Eyes + SSD1306 Status (No Mouth Display)

Left Eye (GC9A01): 115,200 bytes
Right Eye (GC9A01): 115,200 bytes
Status (SSD1306): 1,024 bytes

Total: 231,424 bytes = 226 KB = 0.226 MB

Fits in 300KB? YES (with 69 KB headroom)
Frame rate: 60 FPS all displays
Extra headroom: 69 KB for other features
Recommendation: Good if mouth not critical


Option E: Single GC9A01 Eye + Dual ST7735 Displays + SSD1306 Status

Primary Eye (GC9A01): 115,200 bytes
Secondary 1 (ST7735): 40,960 bytes
Secondary 2 (ST7735): 40,960 bytes
Status (SSD1306): 1,024 bytes

Total: 198,144 bytes = 193 KB = 0.193 MB

Fits in 300KB? YES (with 107 KB headroom)
Frame rate: 60 FPS all displays
Note: Not suitable for dual-eye goblin (only one eye)


CHUNKING ANALYSIS - ILI9341 Mouth Split into 4 Horizontal Chunks

Full ILI9341 buffer needed: 460,800 bytes (450 KB) - EXCEEDS 300KB

Chunked approach (4 horizontal strips):

Chunk 0 (rows 0-79):
  Width: 480 pixels
  Height: 80 pixels
  Total pixels: 480  80 = 38,400 pixels
  Bytes: 38,400  3 = 115,200 bytes
  
Chunk 1 (rows 80-159):
  Width: 480 pixels
  Height: 80 pixels
  Total pixels: 480  80 = 38,400 pixels
  Bytes: 38,400  3 = 115,200 bytes
  
Chunk 2 (rows 160-239):
  Width: 480 pixels
  Height: 80 pixels
  Total pixels: 480  80 = 38,400 pixels
  Bytes: 38,400  3 = 115,200 bytes
  
Chunk 3 (rows 240-319):
  Width: 480 pixels
  Height: 80 pixels
  Total pixels: 480  80 = 38,400 pixels
  Bytes: 38,400  3 = 115,200 bytes


Sum of all chunks: 460,800 bytes (same as full buffer)
But only ONE chunk in memory at a time: 115,200 bytes
Peak memory usage: 115,200 bytes per chunk
Sequential rendering time: 4 chunks  66ms/chunk = 264ms total


MEMORY CONSTRAINT ANALYSIS

System Total RAM: 512 KB
OS/Stack overhead: ~200 KB
Available for application: ~300 KB

Display buffer sizes relative to 300 KB constraint:

Display | Bytes | KB | % of 300KB | Fits Alone?
---|---|---|---|---
GC9A01 | 115,200 | 112.5 | 37.5% | YES
ST7789 | 153,600 | 150 | 50% | YES
ST7735 | 40,960 | 40 | 13% | YES
ILI9341 | 460,800 | 450 | 150% | NO
ILI9486 | 460,800 | 450 | 150% | NO
ILI9481 | 1,152,000 | 1,125 | 375% | NO
RA8875 | 1,152,000 | 1,125 | 375% | NO
SSD1306 | 1,024 | 1 | 0.3% | YES
SSD1331 | 12,288 | 12 | 4% | YES
SSD1351 | 32,768 | 32 | 11% | YES


RECOMMENDED GOBLIN HEAD CONFIGURATION

FINAL RECOMMENDATION: Dual GC9A01 Eyes + ST7735 Mouth

Left eye (GC9A01): 115,200 bytes = 112.5 KB
Right eye (GC9A01): 115,200 bytes = 112.5 KB
Mouth (ST7735): 40,960 bytes = 40 KB
Status (SSD1306): 1,024 bytes = 1 KB

Total required: 272,384 bytes = 266 KB

Headroom available: 300 KB - 266 KB = 34 KB
Percentage used: 88.8% of 300 KB
Percentage available: 11.2% headroom

VERDICT: FITS COMFORTABLY
- All displays can update simultaneously at 60 FPS
- No chunking complexity
- 34 KB spare for other memory needs
- Mouth display slightly smaller (160128) but still expressive
- Best balance of complexity vs performance


ALTERNATIVE: If You Really Need High-Res Mouth (ILI9341)

With chunking strategy:
Left eye (GC9A01): 115,200 bytes = 112.5 KB
Right eye (GC9A01): 115,200 bytes = 112.5 KB
Mouth chunk (48080): 115,200 bytes = 112.5 KB (reuse buffer)
Status (SSD1306): 1,024 bytes = 1 KB

Peak memory: 346,624 bytes = 338 KB

Problem: Exceeds 300 KB by 38 KB
Solution: Buffer sharing/sequential allocation
  - Allocate eyes: 230,400 bytes
  - Allocate mouth chunk: 115,200 bytes
  - Total: 345,600 bytes - STILL EXCEEDS
  
Actual solution requires:
  - Reusing the same buffer space for sequential operations
  - Eyes render  transfer
  - Clear buffer
  - Mouth chunk render  transfer
  - Only one buffer active per cycle = 115,200 bytes at a time
  - Total RAM with clever allocation: ~240 KB (FITS with 60 KB headroom)


DOCUMENT STATUS: Display buffer size reference spreadsheet
NOT PART OF BUILD - Technical reference for memory planning
ASCII encoding, no special characters
