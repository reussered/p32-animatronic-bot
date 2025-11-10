Display Buffer Analysis - 300KB RAM Constraint
================================================

System Constraint: ESP32-S3 R8N16 has ~512KB total RAM
Usable for application: ~300KB (after OS, stack, system overhead)
Maximum single buffer: ~300KB


BUFFER SIZE CALCULATIONS BY DISPLAY TYPE
========================================

Formula: Resolution (W  H)  Bytes-per-pixel = Buffer size in bytes
Then divide by 1024 to convert to KB

Display Type | Resolution | Color Depth | Bytes/Pixel | Full Buffer | Calculation
---|---|---|---|---|---
GC9A01 | 240240 | RGB565 | 2 | 115.2 KB | 240  240  2 = 115,200 bytes  1024 = 112.5 KB
ST7789 | 240320 | RGB565 | 2 | 153.6 KB | 240  320  2 = 153,600 bytes  1024 = 150 KB
ST7735 | 160128 | RGB565 | 2 | 40.96 KB | 160  128  2 = 40,960 bytes  1024 = 40 KB
ILI9341 | 480320 | RGB666 | 3 | 460.8 KB | 480  320  3 = 460,800 bytes  1024 = 450 KB
ILI9486 | 480320 | RGB888 | 3 | 460.8 KB | 480  320  3 = 460,800 bytes  1024 = 450 KB
ILI9481 | 800480 | RGB888 | 3 | 1,152 KB | 800  480  3 = 1,152,000 bytes  1024 = 1,125 KB
RA8875 | 800480 | RGB666 | 3 | 1,152 KB | 800  480  3 = 1,152,000 bytes  1024 = 1,125 KB
SSD1306 | 12864 | Monochrome | 0.125 | 1.024 KB | 128  64  0.125 = 1,024 bytes  1024 = 1 KB
SSD1331 | 9664 | RGB565 | 2 | 12.29 KB | 96  64  2 = 12,288 bytes  1024 = 12 KB
SSD1351 | 128128 | RGB565 | 2 | 32.77 KB | 128  128  2 = 32,768 bytes  1024 = 32 KB


DETAILED FRAME BUFFER SIZE BREAKDOWN
====================================

GC9A01 Display (Goblin Eye):
  Resolution: 240 pixels wide  240 pixels tall
  Pixels total: 240  240 = 57,600 pixels per frame
  Color depth: RGB565 (16-bit, 2 bytes per pixel)
  Bytes per frame: 57,600  2 = 115,200 bytes
  Buffer size: 115,200  1024 = 112.5 KB per eye
  Dual eyes: 112.5 KB  2 = 225 KB for both eyes

  Memory breakdown:
    57,600 pixels  2 bytes = 115,200 bytes
    115,200 bytes  1024 bytes/KB = 112.5 KB
    115,200 bytes  1,024 KB = 0.1123 MB

ST7789 Display (Alternative eye):
  Resolution: 240 pixels wide  320 pixels tall
  Pixels total: 240  320 = 76,800 pixels per frame
  Color depth: RGB565 (16-bit, 2 bytes per pixel)
  Bytes per frame: 76,800  2 = 153,600 bytes
  Buffer size: 153,600  1024 = 150 KB per display

ST7735 Display (Budget eye/mouth):
  Resolution: 160 pixels wide  128 pixels tall
  Pixels total: 160  128 = 20,480 pixels per frame
  Color depth: RGB565 (16-bit, 2 bytes per pixel)
  Bytes per frame: 20,480  2 = 40,960 bytes
  Buffer size: 40,960  1024 = 40 KB per display

ILI9341 Display (High-res mouth):
  Resolution: 480 pixels wide  320 pixels tall
  Pixels total: 480  320 = 153,600 pixels per frame
  Color depth: RGB666 (18-bit, but stored as 3 bytes per pixel)
  Bytes per frame: 153,600  3 = 460,800 bytes
  Buffer size: 460,800  1024 = 450 KB per display
  Status: EXCEEDS 300KB limit if used with dual eyes

ILI9341 Chunked (4 horizontal strips):
  Strip dimensions: 480 pixels wide  80 pixels tall
  Pixels per strip: 480  80 = 38,400 pixels
  Color depth: RGB666 (3 bytes per pixel)
  Bytes per chunk: 38,400  3 = 115,200 bytes
  Buffer size per chunk: 115,200  1024 = 112.5 KB
  Number of chunks: 4 (rows 0-79, 80-159, 160-239, 240-319)
  Total chunks to render: 4 sequential renders
  Peak memory: 112.5 KB (only one chunk in memory at a time)

SSD1306 Display (Monochrome status):
  Resolution: 128 pixels wide  64 pixels tall
  Pixels total: 128  64 = 8,192 pixels per frame
  Color depth: Monochrome (1-bit, but typically stored as 1 bit per pixel)
  Bytes per frame: (8,192 bits)  8 = 1,024 bytes
  Buffer size: 1,024  1024 = 1 KB per display
  Status: Negligible, can have multiple on same bus


RECOMMENDED GOBLIN CONFIGURATION MEMORY MATH
===========================================

Option A: Dual GC9A01 Eyes + ST7735 Mouth

Left Eye (GC9A01):
  240  240  2 = 115,200 bytes = 112.5 KB

Right Eye (GC9A01):
  240  240  2 = 115,200 bytes = 112.5 KB

Mouth (ST7735):
  160  128  2 = 40,960 bytes = 40 KB

Total frame buffers: 112.5 + 112.5 + 40 = 265 KB
Headroom in 300KB: 300 - 265 = 35 KB
Status: FITS comfortably


Option B: Dual GC9A01 Eyes + Chunked ILI9341 Mouth

Left Eye (GC9A01):
  240  240  2 = 115,200 bytes = 112.5 KB

Right Eye (GC9A01):
  240  240  2 = 115,200 bytes = 112.5 KB

Mouth chunk (ILI9341, 48080 chunk):
  480  80  3 = 115,200 bytes = 112.5 KB
  (4 chunks total, but only one in memory at a time)

Total peak memory: 112.5 + 112.5 + 112.5 = 337.5 KB
Status: EXCEEDS by 37.5 KB (would need to reduce eye buffers or use alternative strategy)

Alternative strategy (sequential rendering):
  - Pre-allocate: 112.5 KB (eyes)
  - Reuse buffer for mouth chunks: 112.5 KB
  - Total required: 112.5 KB (share buffer, render sequentially)
  - This allows both displays but requires swap logic


Option C: Single GC9A01 Eye + Dual ST7735 Secondary displays

Primary Eye (GC9A01):
  240  240  2 = 115,200 bytes = 112.5 KB

Secondary 1 (ST7735):
  160  128  2 = 40,960 bytes = 40 KB

Secondary 2 (ST7735):
  160  128  2 = 40,960 bytes = 40 KB

Total: 112.5 + 40 + 40 = 192.5 KB
Headroom: 300 - 192.5 = 107.5 KB
Status: FITS with good headroom (not suitable for dual-eye goblin though)


GOBLIN HEAD CONFIGURATION ANALYSIS
===================================

Scenario A: Dual GC9A01 Eyes + ILI9341 Mouth (EXCEEDS BUDGET)

Component | Buffer Size | Running Total | Status
---|---|---|---
Left Eye (GC9A01) | 115.2 KB | 115.2 KB | OK
Right Eye (GC9A01) | 115.2 KB | 230.4 KB | OK
Mouth (ILI9341) | 460.8 KB | 691.2 KB | EXCEEDS by 391.2 KB!

Result: Cannot fit all three displays simultaneously in single 300KB buffer


SOLUTION 1: CHUNKED RENDERING (RECOMMENDED FOR LARGE DISPLAYS)
==============================================================

Concept: Render display in horizontal or vertical strips instead of full frame

### ILI9341 Mouth Display - Chunked to 4 rows

Display: 480320 pixels, RGB666 (3 bytes/pixel)
Full buffer: 460.8 KB (exceeds limit)
Chunking strategy: Render 4 horizontal strips of 80 pixels height each

Chunk configuration:
  Resolution per chunk: 48080 pixels
  Bytes per chunk: 480  80  3 = 115.2 KB (FITS!)
  Number of chunks: 4
  Rendering time per chunk: ~16ms (assuming 80 MHz SPI)
  Total time to update mouth: 4  16ms = 64ms (15 FPS update rate)

Timing flow:
  1. Render chunk 0 (rows 0-79) to buffer [115.2 KB]
  2. Transfer to display
  3. Clear buffer
  4. Render chunk 1 (rows 80-159)
  5. Transfer to display
  ... (repeat for chunks 2, 3)

Result: Full mouth display updates at ~15 FPS instead of 60 FPS, but FITS in 300KB


### Alternative: Vertical strips (4804 = 120px wide)

Chunk configuration:
  Resolution per chunk: 120320 pixels
  Bytes per chunk: 120  320  3 = 115.2 KB (FITS!)
  Number of chunks: 4
  Update pattern: Left-to-right or right-to-left sweep effect

Advantage: Creates visual effect of display painting from edge to edge
Disadvantage: Slower visual update than horizontal chunks


SOLUTION 2: DUAL-BUFFER SYSTEM (BETTER PERFORMANCE, HIGHER RAM USE)
==================================================================

Concept: Use two smaller buffers, alternate between them

Scenario: Dual GC9A01 Eyes + Compact ILI9341

Primary buffer (used by both eyes):
  - Size: 115.2 KB (fits single GC9A01)
  - Used by: Left eye animation engine
  - Then: Right eye animation engine (sequential, not simultaneous)

Secondary buffer (fixed size, reserved):
  - Size: 115.2 KB
  - Used by: ILI9341 mouth (chunked into smaller allocations as needed)

Total: 230.4 KB (still within 300KB)

Rendering pipeline:
  1. Render left eye animation  primary buffer [115.2 KB]
  2. DMA transfer to left eye display
  3. Simultaneously: Render mouth chunk  secondary buffer [57.6 KB]
  4. DMA transfer mouth chunk to display
  5. Repeat for right eye + next mouth chunk


SOLUTION 3: SMART MEMORY ALLOCATION (RUNTIME POOLING)
======================================================

Concept: Allocate buffers on-demand, reuse freed memory

Memory pool: 300 KB heap

Display initialization:
  - Left eye: allocate 115.2 KB, retain in memory
  - Right eye: REUSE primary buffer (render, transfer, clear) + 115.2 KB primary
  - Mouth: allocate 115.2 KB chunk
  - Total allocated simultaneously: 230.4 KB

Rendering cycle (50ms per full frame):
  Loop:
    1. Render left eye to buffer[0] at t=0ms
    2. Start DMA transfer left eye at t=2ms
    3. Render right eye to buffer[0] (reuse) at t=4ms (while DMA still running)
    4. Start DMA transfer right eye at t=6ms
    5. Render mouth chunk[0] to buffer[1] at t=8ms
    6. Start DMA transfer mouth at t=10ms
    7. Switch to mouth chunk[1] while DMA running
    8. Complete all DMA by t=50ms
    9. Loop

Result: Full 60 FPS animation for eyes + 15 FPS for mouth with only 230.4 KB peak allocation


RECOMMENDED APPROACH FOR GOBLIN HEAD
====================================

Configuration: Dual GC9A01 Eyes + ILI9341 Mouth + Optional SSD1306 Status

Memory Layout:
  Buffer 0 (Eye/Mouth staging): 115.2 KB
  Buffer 1 (Secondary staging): 115.2 KB
  Remaining for OS/stack: 69.6 KB
  Total: 300 KB

Rendering Strategy:
  1. Eyes: Full 60 FPS simultaneous rendering (alternate frames)
     - Frame A: Render + transfer both eyes from single buffer
     - Frame B: Render + transfer both eyes from single buffer
     - Use DMA to overlap rendering + transfer

  2. Mouth: Chunked 48080 pixel strips, ~15 FPS effective update
     - Chunk 0 (rows 0-79): 115.2 KB, renders at t=0ms
     - Chunk 1 (rows 80-159): renders at t=66ms
     - Chunk 2 (rows 160-239): renders at t=132ms
     - Chunk 3 (rows 240-319): renders at t=198ms
     - Full mouth updates every 264ms (3.8 FPS full refresh)

  3. Status (SSD1306): 1.024 KB, negligible
     - Can update every frame without concern

GPIO/Power Impact:
  - Eyes: 9 GPIO pins, 0.6W
  - Mouth: 3 GPIO pins, 0.8W
  - Status: 0 GPIO pins (I2C shared), 0.06W
  - Total: 12 GPIO, 1.46W


CHUNKING IMPLEMENTATION PSEUDOCODE
===================================

```cpp
// Buffer definitions
uint8_t display_buffer[115200];  // 115.2 KB for one chunk

// Mouth display parameters
#define MOUTH_WIDTH 480
#define MOUTH_HEIGHT 320
#define CHUNK_HEIGHT 80  // 48080 fits in 115.2 KB with RGB666

void render_mouth_chunked()
{
  for (int chunk = 0; chunk < 4; chunk++)
  {
    int y_start = chunk * CHUNK_HEIGHT;
    int y_end = y_start + CHUNK_HEIGHT;
    
    // Render this chunk to buffer
    for (int y = y_start; y < y_end; y++)
    {
      for (int x = 0; x < MOUTH_WIDTH; x++)
      {
        uint32_t color = mouth_animation[y][x];
        // Convert to RGB666 and write to buffer
        buffer[index++] = color_to_bytes(color);
      }
    }
    
    // Transfer chunk to display at position y_start
    display_write_region(0, y_start, MOUTH_WIDTH, y_end, buffer);
    
    // Optionally: clear buffer (not needed if overwriting)
  }
}

// Alternative: Vertical chunks (left-to-right sweep)
#define CHUNK_WIDTH 120  // 4804
void render_mouth_vertical_chunks()
{
  for (int chunk = 0; chunk < 4; chunk++)
  {
    int x_start = chunk * CHUNK_WIDTH;
    int x_end = x_start + CHUNK_WIDTH;
    
    // Render vertical strip
    for (int y = 0; y < MOUTH_HEIGHT; y++)
    {
      for (int x = x_start; x < x_end; x++)
      {
        uint32_t color = mouth_animation[y][x];
        buffer[index++] = color_to_bytes(color);
      }
    }
    
    // Transfer vertical chunk
    display_write_region(x_start, 0, x_end, MOUTH_HEIGHT, buffer);
  }
}
```


ALTERNATIVE: DOWNSIZE ILI9341 MOUTH DISPLAY
===========================================

If chunking adds too much complexity, consider using ST7789 instead:

Display | Resolution | Bytes | Buffer Size | Fits in 300KB?
---|---|---|---|---
ILI9341 | 480320 RGB666 | 3 | 460.8 KB | NO (needs chunking)
ST7789 | 240320 RGB565 | 2 | 153.6 KB | YES (direct)
ST7735 | 160128 RGB565 | 2 | 40.96 KB | YES (direct)

Configuration B: Dual GC9A01 Eyes + ST7789 Mouth (NO CHUNKING NEEDED)

Component | Buffer | Running Total
---|---|---
Left Eye | 115.2 KB | 115.2 KB
Right Eye | 115.2 KB | 230.4 KB
Mouth (ST7789) | 153.6 KB | 384 KB (EXCEEDS by 84 KB)

Result: Still exceeds, but closer


Configuration C: Dual GC9A01 Eyes + ST7735 Mouth (FITS CLEANLY)

Component | Buffer | Running Total
---|---|---
Left Eye | 115.2 KB | 115.2 KB
Right Eye | 115.2 KB | 230.4 KB
Mouth (ST7735) | 40.96 KB | 271.36 KB
Remaining | - | 28.64 KB
**Total** | - | **271.36 KB within 300KB limit!**

Advantage: Full 60 FPS rendering on all displays, no chunking needed
Disadvantage: Mouth display is smaller (160128, suitable for simple mouth shapes, not detailed teeth)


MEMORY CONSTRAINT SUMMARY
========================

Hard Constraint: 300 KB maximum usable heap

Option 1: CHUNKING (Recommended for high-res mouth)
  - Use: Dual GC9A01 Eyes (230.4 KB) + ILI9341 Mouth chunked (115.2 KB per chunk)
  - Eyes: 60 FPS
  - Mouth: 15 FPS effective (full refresh every ~260ms)
  - Complexity: Medium (requires chunk rendering logic)
  - Flexibility: Can switch between horizontal/vertical chunks

Option 2: DOWNSIZE MOUTH (Recommended for simplicity)
  - Use: Dual GC9A01 Eyes (230.4 KB) + ST7735 Mouth (40.96 KB)
  - Eyes: 60 FPS
  - Mouth: 60 FPS
  - Total: 271.36 KB (fits cleanly)
  - Complexity: Low (no chunking)
  - Limitation: Smaller mouth display, less detail

Option 3: SINGLE LARGE DISPLAY (Compromise)
  - Use: One GC9A01 Eye (115.2 KB) + ILI9341 Mouth chunked (115.2 KB)
  - Eyes: Only left eye, or single "mono" eye display
  - Not suitable for dual-eye goblin

Option 4: I2C STATUS + SPI PRIMARY (Hybrid)
  - Use: Dual GC9A01 Eyes (230.4 KB) + SSD1306 Status (1 KB) on I2C
  - Eyes: 60 FPS
  - Status: Full monochrome display for health, battery, mode info
  - Total: 231.4 KB (excellent headroom)
  - No mouth display (use status screen instead)


RECOMMENDATION FOR GOBLIN HEAD (FINAL)
======================================

Best Practice: Dual GC9A01 Eyes (240240) + ST7735 Mouth (160128)

Memory layout:
  - Eye buffer: 115.2 KB
  - Mouth buffer: 40.96 KB
  - Status/miscellaneous: ~50 KB available
  - Total: 271.36 KB (27.64 KB headroom for safety)

Performance:
  - Eyes: 60 FPS smooth animation
  - Mouth: 60 FPS smooth animation
  - All displays responsive and real-time
  - No chunking complexity

GPIO: 12 pins
Power: 1.04W
Configuration: Clean, no memory juggling


If you MUST have higher-resolution mouth (ILI9341):
  - Implement chunking (4 horizontal strips, 115.2 KB each)
  - Mouth updates at ~15 FPS (acceptable for expression display)
  - Use DMA transfers to overlap rendering + IO


DOCUMENT STATUS: Memory analysis and chunking strategy reference
NOT PART OF BUILD - Standalone technical reference
ASCII encoding, no special characters
