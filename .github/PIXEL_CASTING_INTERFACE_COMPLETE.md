# Pixel Format Casting Interface - All Classes Updated

## Summary

Every pixel format class in `color_schema.hpp` now has **clean, uniform conversion methods** for the generic-to-specific pointer casting pattern.

## Conversion Methods on Every Pixel Format

Each pixel class (`Pixel_RGB565`, `Pixel_RGB444`, `Pixel_RGB555`, `Pixel_RGB666`, `Pixel_RGB888`, `Pixel_Grayscale`) now implements:

```cpp
// Transform FROM raw bytes TO typed pixels (static factory)
static PixelType* fromBytes(uint8_t* byte_ptr) {
    return reinterpret_cast<PixelType*>(byte_ptr);
}

static const PixelType* fromBytes(const uint8_t* byte_ptr) {
    return reinterpret_cast<const PixelType*>(byte_ptr);
}

// Transform FROM typed pixel TO raw bytes (instance method)
uint8_t* toBytes() {
    return reinterpret_cast<uint8_t*>(this);
}

const uint8_t* toBytes() const {
    return reinterpret_cast<const uint8_t*>(this);
}
```

## Why Every Class Needs This

The template system relies on **runtime polymorphism through templates**:

```cpp
// In mood_effects.hpp:
template<class PixelType>
void adjustMood(
    unsigned char* buffer_ptr,  // Generic bytes from allocator
    uint32_t pixel_count,
    const Mood& mood,
    const MoodColorEffect* mood_effects)
{
    // PixelType MUST have fromBytes() - will fail to compile if missing
    PixelType* buffer = PixelType::fromBytes(buffer_ptr);
    
    // Use operator+ and operator+= (defined on every PixelType)
    for (uint32_t i = 0; i < pixel_count; ++i) {
        pixel[i] += color_delta;  // Requires operator+=
    }
}
```

**Template instantiation is determined at compile time:**
- `adjustMood<Pixel_RGB565>()` → calls `Pixel_RGB565::fromBytes()`
- `adjustMood<Pixel_RGB888>()` → calls `Pixel_RGB888::fromBytes()`
- `adjustMood<Pixel_Grayscale>()` → calls `Pixel_Grayscale::fromBytes()`

If a pixel format is missing `fromBytes()`, the template instantiation **fails at compile time** with clear errors.

## Pixel Formats Now Complete

| Format | Bits | Bytes/Px | Memory (240×240) | fromBytes | toBytes | +=op |
|--------|------|----------|-----------------|-----------|---------|------|
| RGB565 | 5-6-5 | 2 | 115.2 KB | ✅ | ✅ | ✅ |
| RGB444 | 4-4-4 | 1.5 | 86.4 KB | ✅ | ✅ | ✅ |
| RGB555 | 5-5-5 | 2 | 115.2 KB | ✅ | ✅ | ✅ |
| RGB666 | 6-6-6 | 3 | 172.8 KB | ✅ | ✅ | ✅ |
| RGB888 | 8-8-8 | 3 | 172.8 KB | ✅ | ✅ | ✅ |
| Grayscale | 8 | 1 | 57.6 KB | ✅ | ✅ | ✅ |

## Why += Operators Are Critical

The `adjustMood<>()` template needs `operator+=` for in-place color accumulation:

```cpp
// In adjustMood template:
for (uint32_t i = 0; i < pixel_count; ++i) {
    PixelType& pixel = buffer[i];
    
    // These require operator+= on the PixelType
    pixel.r += r_delta;  // Saturating addition on red channel
    pixel.g += g_delta;  // Saturating addition on green channel
    pixel.b += b_delta;  // Saturating addition on blue channel
}
```

Each format implements saturating arithmetic:
- **RGB565/555/444** (bit-field): Manual clamp (e.g., `((val + delta) > 31) ? 31 : (val + delta)`)
- **RGB666/888** (uint8_t): Manual clamp (e.g., `((val + delta) > 255) ? 255 : (val + delta)`)
- **Grayscale** (uint8_t): Manual clamp (e.g., `((val + delta) > 255) ? 255 : (val + delta)`)

## Usage Pattern Now Complete

```cpp
// 1. ALLOCATE (goblin_left_eye.src)
unsigned char* display_buffer = (unsigned char*)malloc(240 * 240 * 2);

// 2. PROCESS (goblin_eye.src calls adjustMood<>)
adjustMood<Pixel_RGB565>(
    display_buffer,        // unsigned char*
    240 * 240,
    *mood_ptr,
    goblin_mood_effects
);
// Inside adjustMood<Pixel_RGB565>:
//   Pixel_RGB565* pixels = Pixel_RGB565::fromBytes(display_buffer);
//   for (i = 0; i < count; ++i) {
//       pixels[i] += color_delta;  // Uses operator+=
//   }

// 3. DISPLAY (generic_spi_display.src)
send_to_spi(display_buffer, 115200);
free(display_buffer);
```

## Extension: Adding a New Pixel Format

To support a hypothetical `Pixel_RGB1555` (1-bit alpha + 5-5-5 RGB):

1. Define the struct in `color_schema.hpp`
2. Implement all required methods:
   - Constructors (default, with RGB values)
   - `operator=` (assignment)
   - `operator==`, `operator!=` (comparison)
   - `operator+`, `operator+=` (saturating arithmetic) ← **REQUIRED FOR TEMPLATE**
   - `fromBytes()`, `toBytes()` ← **REQUIRED FOR TEMPLATE**
3. Creature components automatically work:
   ```cpp
   adjustMood<Pixel_RGB1555>(buffer, size, mood, effects);  // Just works!
   ```

## Files Updated

- `config/shared_headers/color_schema.hpp` - Added `fromBytes()`/`toBytes()` to all 6 pixel formats
- `config/shared_headers/mood_effects.hpp` - Updated template to use `PixelType::fromBytes()`
- All creature eye components already compatible (template instantiation automatic)

