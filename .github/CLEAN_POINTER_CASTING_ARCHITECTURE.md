# Clean Pointer Casting Architecture

## Overview

The goblin eye/mouth rendering system uses a **generic-to-specific pointer casting pattern** for type-safe display buffer management across different pixel formats.

## Pattern: Transform void*  TypedPointer

```cpp
// ALLOCATE: Generic bytes (positioned components)
void* generic_buffer = malloc(size);  // unsigned char* in our case

// TRANSFORM TO SPECIFIC: At point of use (processing components)
PixelType* typed_buffer = PixelType::fromBytes(generic_buffer);

// WORK WITH SPECIFIC: Full type safety
for (uint32_t i = 0; i < pixel_count; i++) {
    typed_buffer[i] = somePixelValue;  // Type-safe operations
}

// TRANSFORM BACK TO GENERIC: Cast back for generic handling
void* generic_again = typed_buffer[0].toBytes();

// FREE GENERIC: Raw byte cleanup
free(generic_again);
```

## Clean Interface: Static Helper Methods

### In `color_schema.hpp`

```cpp
struct Pixel_RGB565 {
    // ...pixel fields...
    
    // Transform FROM raw bytes TO typed pixels
    static Pixel_RGB565* fromBytes(uint8_t* byte_ptr) {
        return reinterpret_cast<Pixel_RGB565*>(byte_ptr);
    }
    
    static const Pixel_RGB565* fromBytes(const uint8_t* byte_ptr) {
        return reinterpret_cast<const Pixel_RGB565*>(byte_ptr);
    }
    
    // Transform FROM typed pixel TO raw bytes
    uint8_t* toBytes() {
        return reinterpret_cast<uint8_t*>(this);
    }
    
    const uint8_t* toBytes() const {
        return reinterpret_cast<const uint8_t*>(this);
    }
};
```

## Architecture in Practice

### 1. Positioned Components Allocate (e.g., `goblin_left_eye.src`)

```cpp
// Allocate as unsigned char* (generic bytes)
static unsigned char* display_buffer = nullptr;

esp_err_t goblin_left_eye_init(void) {
    display_buffer = (unsigned char*)malloc(DISPLAY_BYTES);
    
    // Initialize with neutral colors using uint16_t
    uint16_t neutral = 0x0400;
    uint16_t* buffer_u16 = (uint16_t*)display_buffer;
    for (uint32_t i = 0; i < PIXELS; i++) {
        buffer_u16[i] = neutral;
    }
    return ESP_OK;
}

void goblin_left_eye_act(void) {
    // Empty - buffer managed by component chain
}
```

### 2. Processing Component Uses Template (e.g., `goblin_eye.src`)

```cpp
// In mood_effects.hpp:
template<class PixelType>
void adjustMood(
    unsigned char* buffer_ptr,      // Generic bytes from positioned component
    uint32_t pixel_count,
    const Mood& mood,
    const MoodColorEffect* mood_effects)
{
    // TRANSFORM: Use clean static method
    PixelType* buffer = PixelType::fromBytes(buffer_ptr);
    
    // ... calculate mood deltas ...
    
    // APPLY: Type-safe pixel operations
    for (uint32_t i = 0; i < pixel_count; ++i) {
        PixelType& pixel = buffer[i];
        pixel.r = pixel.r + r_delta;
        pixel.g = pixel.g + g_delta;
        pixel.b = pixel.b + b_delta;
    }
}

// In goblin_eye.src:
void goblin_eye_act(void) {
    Mood* mood_ptr = GSM.read<Mood>();
    if (mood_ptr == nullptr) return;
    
    // Call template - it internally uses fromBytes()
    adjustMood<Pixel_RGB565>(
        display_buffer,        // unsigned char* from positioned component
        240 * 240,
        *mood_ptr,
        goblin_mood_effects
    );
}
```

### 3. Display Component Frees (e.g., `generic_spi_display.src`)

```cpp
void generic_spi_display_act(void) {
    // Send buffer to hardware
    send_to_spi(display_buffer, buffer_size);
    
    // Free as generic bytes
    free(display_buffer);
    display_buffer = nullptr;
}
```

## Benefits

| Benefit | Why |
|---------|-----|
| **One Allocation Point** | Left/right/mouth components each allocate once |
| **Multiple Processors** | `goblin_eye` works with any positioned component's buffer |
| **Type Safety at Use** | `adjustMood<PixelType>` ensures correct pixel arithmetic |
| **Reusability** | Same `adjustMood` template works for all creatures |
| **Clean Semantics** | `fromBytes()` and `toBytes()` make intent explicit |
| **No Redundant Casts** | Casts hidden inside helper methods |

## Component Chain

```
goblin_left_eye        goblin_right_eye        goblin_mouth
                                                    
  allocate            allocate                  allocate
unsigned char*      unsigned char*          unsigned char*
display_buffer      display_buffer          display_buffer
                                                    
    
                          
                    goblin_eye
                        
                adjustMood<Pixel_RGB565>()
              (uses Pixel_RGB565::fromBytes())
                        
                generic_spi_display
                        
                  free(display_buffer)
```

## Key Design Rules

1. **Allocators (positioned components)**: Use `unsigned char*` storage
2. **Processors (goblin_eye)**: Use `adjustMood<PixelType>()` template
3. **Template internals**: Call `PixelType::fromBytes()` to cast
4. **Deallocators (generic_spi_display)**: Free as raw `unsigned char*`

## Extension: Adding New Pixel Formats

To support a new pixel format (e.g., `Pixel_RGB666`):

1. Define the struct in `color_schema.hpp`
2. Add `fromBytes()` and `toBytes()` methods
3. Template instantiation happens automatically
4. Creature components (goblin_eye) already work with it

```cpp
struct Pixel_RGB666 {
    // ... 6-6-6 bit encoding ...
    
    static Pixel_RGB666* fromBytes(uint8_t* byte_ptr) {
        return reinterpret_cast<Pixel_RGB666*>(byte_ptr);
    }
    // ... rest of helpers ...
};

// No changes needed in goblin_eye.src!
// adjustMood<Pixel_RGB666>(...) just works
```

## Related Files

- `config/shared_headers/color_schema.hpp` - Pixel format definitions + `fromBytes()`/`toBytes()`
- `config/shared_headers/mood_effects.hpp` - `adjustMood<PixelType>()` template
- `config/bots/bot_families/goblins/head/goblin_left_eye.src` - Left eye allocator
- `config/bots/bot_families/goblins/head/goblin_right_eye.src` - Right eye allocator
- `config/bots/bot_families/goblins/head/goblin_mouth.src` - Mouth allocator
- `config/bots/bot_families/goblins/head/goblin_eye.src` - Generic processor using `adjustMood<>`
- `config/bots/bot_families/goblins/head/generic_spi_display.src` - Display + deallocator

