#pragma once

/**
 * @file color_schema.hpp
 * @brief Color schema definitions for display pixel formats
 * 
 * This file defines pixel format classes for different display color depths.
 * Each class represents a specific color encoding format (RGB565, RGB666, RGB888, etc.)
 * 
 * IMPORTANT: Different displays require different formats based on their hardware design.
 * You must select the format that matches your specific display controller/driver.
 * These formats are NOT interchangeable between different display types.
 * 
 * Format selection:
 *   - RGB565 (5-6-5 bit encoding): GC9A01, ST7789, ILI9341, and most ESP32 displays
 *   - RGB666 (6-6-6 bit encoding): Some OLED displays (check datasheet)
 *   - RGB888 (8-8-8 bit encoding): High-end displays, universal intermediate format
 *   - RGB444 (4-4-4 bit encoding): Ultra-low-power or legacy systems
 *   - RGB555 (5-5-5 bit encoding): Legacy systems, alternative to RGB565
 *   - Grayscale (8-bit): E-ink displays, monochrome OLEDs
 * 
 * @author P32 Animatronic Bot Project
 */

#include <cstdint>

// Forward declaration
struct Pixel_RGB565;

// Global InitialPixel - must be set before allocating buffers containing Pixel_RGB565s
extern Pixel_RGB565 RGB565_InitialPixel;


/**
 * @class Pixel_RGB565
 * @brief 5-6-5 bit RGB color format (16-bit per pixel)
 * 
 * Memory layout: RRRRRGGG GGGBBBBB (using bit-fields for efficient packing)
 * - Red: 5 bits (0-31, maps to 0-255)
 * - Green: 6 bits (0-63, maps to 0-255)
 * - Blue: 5 bits (0-31, maps to 0-255)
 * 
 * Total: 2 bytes per pixel
 * Example: 240x240 display = 115,200 bytes (112.5 KB)
 * 
 * Most common format for:
 * - GC9A01 (240x240 circular displays)
 * - ST7789 (240x240, 135x240 displays)
 * - ILI9341 (320x240 displays)
 * 
 * Uses bit-fields for memory efficiency (same as used in gc9a01.hdr)
 */
struct Pixel_RGB565
{
    unsigned int red : 5;
    unsigned int green : 6;
    unsigned int blue : 5;
    
    // Default constructor using InitialPixel
    Pixel_RGB565() 
    {
        *this = RGB565_InitialPixel;
    }
    
    // Constexpr constructor for static colors - scales 8-bit (0-255) to bit-field ranges
    constexpr Pixel_RGB565(uint8_t r, uint8_t g, uint8_t b)
        : red(r >> 3), green(g >> 2), blue(b >> 3)  // Scale: 8-bit to 5-bit, 6-bit, 5-bit
    {}
    
    // Copy constructor
    Pixel_RGB565(const Pixel_RGB565& other) 
        : red(other.red), green(other.green), blue(other.blue)
    {}
    
    // Assignment operator
    Pixel_RGB565& operator=(const Pixel_RGB565& other)
    {
        red = other.red;
        green = other.green;
        blue = other.blue;
        return *this;
    }
    
    // Static color constants (using 8-bit values 0-255, auto-scaled to bit-fields)
    static constexpr Pixel_RGB565 Red()   { return Pixel_RGB565(255, 0,   0); }
    static constexpr Pixel_RGB565 Green() { return Pixel_RGB565(0,   255, 0); }
    static constexpr Pixel_RGB565 Blue()  { return Pixel_RGB565(0,   0,   255); }
    static constexpr Pixel_RGB565 Black() { return Pixel_RGB565(0,   0,   0); }
    static constexpr Pixel_RGB565 White() { return Pixel_RGB565(255, 255, 255); }
    static constexpr Pixel_RGB565 Yellow(){ return Pixel_RGB565(255, 255, 0); }
    static constexpr Pixel_RGB565 Cyan()  { return Pixel_RGB565(0,   255, 255); }
    static constexpr Pixel_RGB565 Magenta(){ return Pixel_RGB565(255, 0,   255); }
    
    // Conversion operator to/from RGB888 (universal format)
    explicit operator Pixel_RGB888() const
    {
        return Pixel_RGB888(
            (red << 3) | (red >> 2),    // Scale 5-bit to 8-bit with dithering
            (green << 2) | (green >> 4),  // Scale 6-bit to 8-bit with dithering
            (blue << 3) | (blue >> 2)   // Scale 5-bit to 8-bit with dithering
        );
    }
    
    Pixel_RGB565& operator=(const Pixel_RGB888& other)
    {
        red = other.r >> 3;    // Scale 8-bit to 5-bit
        green = other.g >> 2;  // Scale 8-bit to 6-bit
        blue = other.b >> 3;   // Scale 8-bit to 5-bit
        return *this;
    }
    
    // Conversion operator to uint8_t* (returns pointer to the bit-field storage)
    explicit operator uint8_t*() 
    { 
        return reinterpret_cast<uint8_t*>(this); 
    }
    
    explicit operator const uint8_t*() const 
    { 
        return reinterpret_cast<const uint8_t*>(this); 
    }
};


/**
 * @class Pixel_RGB444
 * @brief 4-4-4 bit RGB color format (12-bit per pixel)
 * 
 * Memory layout: RRRRGGG GGGBBBB (using bit-fields for efficient packing)
 * - Red: 4 bits (0-15, maps to 0-255)
 * - Green: 4 bits (0-15, maps to 0-255)
 * - Blue: 4 bits (0-15, maps to 0-255)
 * 
 * Total: 1.5 bytes per pixel (12-bit)
 * Example: 240x240 display = 86,400 bytes (84.375 KB)
 * 
 * Used by:
 * - Ultra-low-power displays
 * - Legacy display systems
 * - Memory-constrained embedded systems
 */
struct Pixel_RGB444
{
    unsigned int red : 4;
    unsigned int green : 4;
    unsigned int blue : 4;
    
    // Default constructor
    Pixel_RGB444() : red(0), green(0), blue(0) {}
    
    // Constexpr constructor for static colors - scales 8-bit (0-255) to bit-field ranges
    constexpr Pixel_RGB444(uint8_t r, uint8_t g, uint8_t b)
        : red(r >> 4), green(g >> 4), blue(b >> 4)  // Scale: 8-bit to 4-bit
    {}
    
    // Copy constructor
    Pixel_RGB444(const Pixel_RGB444& other) 
        : red(other.red), green(other.green), blue(other.blue)
    {}
    
    // Assignment operator
    Pixel_RGB444& operator=(const Pixel_RGB444& other)
    {
        red = other.red;
        green = other.green;
        blue = other.blue;
        return *this;
    }
    
    // Static color constants (using 8-bit values 0-255, auto-scaled to bit-fields)
    static constexpr Pixel_RGB444 Red()   { return Pixel_RGB444(255, 0,   0); }
    static constexpr Pixel_RGB444 Green() { return Pixel_RGB444(0,   255, 0); }
    static constexpr Pixel_RGB444 Blue()  { return Pixel_RGB444(0,   0,   255); }
    static constexpr Pixel_RGB444 Black() { return Pixel_RGB444(0,   0,   0); }
    static constexpr Pixel_RGB444 White() { return Pixel_RGB444(255, 255, 255); }
    static constexpr Pixel_RGB444 Yellow(){ return Pixel_RGB444(255, 255, 0); }
    static constexpr Pixel_RGB444 Cyan()  { return Pixel_RGB444(0,   255, 255); }
    static constexpr Pixel_RGB444 Magenta(){ return Pixel_RGB444(255, 0,   255); }
    
    // Comparison operators
    bool operator==(const Pixel_RGB444& other) const 
    { 
        return (red == other.red) && (green == other.green) && (blue == other.blue); 
    }
    
    bool operator!=(const Pixel_RGB444& other) const 
    { 
        return !(*this == other); 
    }
    
    // Arithmetic operators for color blending/accumulation
    Pixel_RGB444 operator+(const Pixel_RGB444& other) const
    {
        return Pixel_RGB444(
            ((red + other.red) > 15) ? 15 : (red + other.red),
            ((green + other.green) > 15) ? 15 : (green + other.green),
            ((blue + other.blue) > 15) ? 15 : (blue + other.blue)
        );
    }
    
    Pixel_RGB444& operator+=(const Pixel_RGB444& other)
    {
        red = ((red + other.red) > 15) ? 15 : (red + other.red);
        green = ((green + other.green) > 15) ? 15 : (green + other.green);
        blue = ((blue + other.blue) > 15) ? 15 : (blue + other.blue);
        return *this;
    }
    
    // Conversion operator to/from RGB888 (universal format)
    explicit operator Pixel_RGB888() const
    {
        return Pixel_RGB888(
            (red << 4) | red,      // Scale 4-bit to 8-bit with dithering
            (green << 4) | green,
            (blue << 4) | blue
        );
    }
    
    Pixel_RGB444& operator=(const Pixel_RGB888& other)
    {
        red = other.r >> 4;    // Scale 8-bit to 4-bit
        green = other.g >> 4;
        blue = other.b >> 4;
        return *this;
    }
    
    // Conversion operator to uint8_t* (returns pointer to the bit-field storage)
    explicit operator uint8_t*() 
    { 
        return reinterpret_cast<uint8_t*>(this); 
    }
    
    explicit operator const uint8_t*() const 
    { 
        return reinterpret_cast<const uint8_t*>(this); 
    }
};


/**
 * @class Pixel_RGB555
 * @brief 5-5-5 bit RGB color format (15-bit per pixel, padded to 16-bit)
 * 
 * Memory layout: XRRRRRG GGGBBBBB (using bit-fields, X is unused/padding bit)
 * - Red: 5 bits (0-31, maps to 0-255)
 * - Green: 5 bits (0-31, maps to 0-255)
 * - Blue: 5 bits (0-31, maps to 0-255)
 * - Unused: 1 bit (padding)
 * 
 * Total: 2 bytes per pixel
 * Example: 240x240 display = 115,200 bytes (112.5 KB)
 * 
 * Used by:
 * - Legacy color display systems
 * - Alternative to RGB565 when uniform bit allocation is preferred
 */
struct Pixel_RGB555
{
    unsigned int red : 5;
    unsigned int green : 5;
    unsigned int blue : 5;
    unsigned int unused : 1;  // Padding bit
    
    // Default constructor
    Pixel_RGB555() : red(0), green(0), blue(0), unused(0) {}
    
    // Constexpr constructor for static colors - scales 8-bit (0-255) to bit-field ranges
    constexpr Pixel_RGB555(uint8_t r, uint8_t g, uint8_t b)
        : red(r >> 3), green(g >> 3), blue(b >> 3), unused(0)  // Scale: 8-bit to 5-bit
    {}
    
    // Copy constructor
    Pixel_RGB555(const Pixel_RGB555& other) 
        : red(other.red), green(other.green), blue(other.blue), unused(0)
    {}
    
    // Assignment operator
    Pixel_RGB555& operator=(const Pixel_RGB555& other)
    {
        red = other.red;
        green = other.green;
        blue = other.blue;
        unused = 0;
        return *this;
    }
    
    // Static color constants (using 8-bit values 0-255, auto-scaled to bit-fields)
    static constexpr Pixel_RGB555 Red()   { return Pixel_RGB555(255, 0,   0); }
    static constexpr Pixel_RGB555 Green() { return Pixel_RGB555(0,   255, 0); }
    static constexpr Pixel_RGB555 Blue()  { return Pixel_RGB555(0,   0,   255); }
    static constexpr Pixel_RGB555 Black() { return Pixel_RGB555(0,   0,   0); }
    static constexpr Pixel_RGB555 White() { return Pixel_RGB555(255, 255, 255); }
    static constexpr Pixel_RGB555 Yellow(){ return Pixel_RGB555(255, 255, 0); }
    static constexpr Pixel_RGB555 Cyan()  { return Pixel_RGB555(0,   255, 255); }
    static constexpr Pixel_RGB555 Magenta(){ return Pixel_RGB555(255, 0,   255); }
    
    // Comparison operators
    bool operator==(const Pixel_RGB555& other) const 
    { 
        return (red == other.red) && (green == other.green) && (blue == other.blue); 
    }
    
    bool operator!=(const Pixel_RGB555& other) const 
    { 
        return !(*this == other); 
    }
    
    // Arithmetic operators for color blending/accumulation
    Pixel_RGB555 operator+(const Pixel_RGB555& other) const
    {
        return Pixel_RGB555(
            ((red + other.red) > 31) ? 31 : (red + other.red),
            ((green + other.green) > 31) ? 31 : (green + other.green),
            ((blue + other.blue) > 31) ? 31 : (blue + other.blue)
        );
    }
    
    Pixel_RGB555& operator+=(const Pixel_RGB555& other)
    {
        red = ((red + other.red) > 31) ? 31 : (red + other.red);
        green = ((green + other.green) > 31) ? 31 : (green + other.green);
        blue = ((blue + other.blue) > 31) ? 31 : (blue + other.blue);
        return *this;
    }
    
    // Conversion operator to/from RGB888 (universal format)
    explicit operator Pixel_RGB888() const
    {
        return Pixel_RGB888(
            (red << 3) | (red >> 2),    // Scale 5-bit to 8-bit with dithering
            (green << 3) | (green >> 2),
            (blue << 3) | (blue >> 2)
        );
    }
    
    Pixel_RGB555& operator=(const Pixel_RGB888& other)
    {
        red = other.r >> 3;    // Scale 8-bit to 5-bit
        green = other.g >> 3;
        blue = other.b >> 3;
        unused = 0;
        return *this;
    }
    
    // Conversion operator to uint8_t* (returns pointer to the bit-field storage)
    explicit operator uint8_t*() 
    { 
        return reinterpret_cast<uint8_t*>(this); 
    }
    
    explicit operator const uint8_t*() const 
    { 
        return reinterpret_cast<const uint8_t*>(this); 
    }
};


/**
 * @class Pixel_RGB666
 * @brief 6-6-6 bit RGB color format (18-bit per pixel, padded to 32-bit)
 * 
 * Memory layout: 00RRRRRR 00GGGGGG 00BBBBBB (stored as 3 bytes or padded to 4)
 * - Red: 6 bits (0-63, maps to 0-255)
 * - Green: 6 bits (0-63, maps to 0-255)
 * - Blue: 6 bits (0-63, maps to 0-255)
 * 
 * Total: 3 bytes per pixel (or 4 bytes with padding)
 * Example: 240x240 display = 172,800 bytes (168.75 KB) or 230,400 bytes (225 KB) with padding
 * 
 * Used by:
 * - Some OLED displays (mid-range color depth)
 * - Displays requiring better color accuracy than RGB565
 */
struct Pixel_RGB666
{
    uint8_t r, g, b;  // 6-bit values stored in 8-bit bytes (upper 6 bits used)
    
    // Default constructor
    Pixel_RGB666() : r(0), g(0), b(0) {}
    
    // Constructor from 8-bit RGB components
    Pixel_RGB666(uint8_t r_val, uint8_t g_val, uint8_t b_val)
    {
        // Convert from 8-bit (0-255) to 6-bit format (0-63)
        r = (r_val >> 2) & 0x3F;
        g = (g_val >> 2) & 0x3F;
        b = (b_val >> 2) & 0x3F;
    }
    
    // Extract RGB components back to 8-bit range
    uint8_t red() const   { return (r & 0x3F) << 2; }
    uint8_t green() const { return (g & 0x3F) << 2; }
    uint8_t blue() const  { return (b & 0x3F) << 2; }
    
    // Static color constants (using 8-bit values 0-255, auto-scaled to 6-bit)
    static constexpr Pixel_RGB666 Red()   { return Pixel_RGB666(255, 0,   0); }
    static constexpr Pixel_RGB666 Green() { return Pixel_RGB666(0,   255, 0); }
    static constexpr Pixel_RGB666 Blue()  { return Pixel_RGB666(0,   0,   255); }
    static constexpr Pixel_RGB666 White() { return Pixel_RGB666(255, 255, 255); }
    static constexpr Pixel_RGB666 Black() { return Pixel_RGB666(0,   0,   0); }
    static constexpr Pixel_RGB666 Yellow(){ return Pixel_RGB666(255, 255, 0); }
    static constexpr Pixel_RGB666 Cyan()  { return Pixel_RGB666(0,   255, 255); }
    static constexpr Pixel_RGB666 Magenta(){ return Pixel_RGB666(255, 0,   255); }
    
    // Assignment operator
    Pixel_RGB666& operator=(const Pixel_RGB666& other)
    {
        r = other.r;
        g = other.g;
        b = other.b;
        return *this;
    }
    
    // Comparison operators
    bool operator==(const Pixel_RGB666& other) const 
    { 
        return (r == other.r) && (g == other.g) && (b == other.b); 
    }
    
    bool operator!=(const Pixel_RGB666& other) const 
    { 
        return !(*this == other); 
    }
    
    // Arithmetic operators for color blending/accumulation
    Pixel_RGB666 operator+(const Pixel_RGB666& other) const
    {
        return Pixel_RGB666(
            ((r + other.r) > 63) ? 63 : (r + other.r),
            ((g + other.g) > 63) ? 63 : (g + other.g),
            ((b + other.b) > 63) ? 63 : (b + other.b)
        );
    }
    
    Pixel_RGB666& operator+=(const Pixel_RGB666& other)
    {
        r = ((r + other.r) > 63) ? 63 : (r + other.r);
        g = ((g + other.g) > 63) ? 63 : (g + other.g);
        b = ((b + other.b) > 63) ? 63 : (b + other.b);
        return *this;
    }
    
    // Conversion operator to/from RGB888 (universal format)
    explicit operator Pixel_RGB888() const
    {
        return Pixel_RGB888(
            (r << 2) | (r >> 4),  // Scale 6-bit to 8-bit with dithering
            (g << 2) | (g >> 4),
            (b << 2) | (b >> 4)
        );
    }
    
    Pixel_RGB666& operator=(const Pixel_RGB888& other)
    {
        r = (other.r >> 2) & 0x3F;
        g = (other.g >> 2) & 0x3F;
        b = (other.b >> 2) & 0x3F;
        return *this;
    }
    
    // Conversion operator to uint8_t* (returns pointer to the byte storage)
    explicit operator uint8_t*() 
    { 
        return reinterpret_cast<uint8_t*>(this); 
    }
    
    explicit operator const uint8_t*() const 
    { 
        return reinterpret_cast<const uint8_t*>(this); 
    }
};


/**
 * @class Pixel_RGB888
 * @brief 8-8-8 bit RGB color format (24-bit per pixel)
 * 
 * Memory layout: RRRRRRRR GGGGGGGG BBBBBBBB
 * - Red: 8 bits (0-255)
 * - Green: 8 bits (0-255)
 * - Blue: 8 bits (0-255)
 * 
 * Total: 3 bytes per pixel
 * Example: 240x240 display = 172,800 bytes (168.75 KB)
 * 
 * Highest color fidelity, used by:
 * - Large OLED displays (AMOLED, etc.)
 * - High-end LCD displays
 * - Displays where color accuracy is critical
 */
struct Pixel_RGB888
{
    uint8_t r, g, b;
    
    // Default constructor
    Pixel_RGB888() : r(0), g(0), b(0) {}
    
    // Constructor from 8-bit RGB components
    Pixel_RGB888(uint8_t r_val, uint8_t g_val, uint8_t b_val)
        : r(r_val), g(g_val), b(b_val)
    {}
    
    // Extract RGB components (no conversion needed - already 8-bit)
    uint8_t red() const   { return r; }
    uint8_t green() const { return g; }
    uint8_t blue() const  { return b; }
    
    // Static color constants
    static constexpr Pixel_RGB888 Red()   { return Pixel_RGB888(255, 0,   0); }
    static constexpr Pixel_RGB888 Green() { return Pixel_RGB888(0,   255, 0); }
    static constexpr Pixel_RGB888 Blue()  { return Pixel_RGB888(0,   0,   255); }
    static constexpr Pixel_RGB888 White() { return Pixel_RGB888(255, 255, 255); }
    static constexpr Pixel_RGB888 Black() { return Pixel_RGB888(0,   0,   0); }
    static constexpr Pixel_RGB888 Yellow(){ return Pixel_RGB888(255, 255, 0); }
    static constexpr Pixel_RGB888 Cyan()  { return Pixel_RGB888(0,   255, 255); }
    static constexpr Pixel_RGB888 Magenta(){ return Pixel_RGB888(255, 0,   255); }
    
    // CSS Standard Colors
    static constexpr Pixel_RGB888 aliceblue()          { return Pixel_RGB888(240, 248, 255); }
    static constexpr Pixel_RGB888 antiquewhite()       { return Pixel_RGB888(250, 235, 215); }
    static constexpr Pixel_RGB888 aqua()               { return Pixel_RGB888(0,   255, 255); }
    static constexpr Pixel_RGB888 aquamarine()         { return Pixel_RGB888(127, 255, 212); }
    static constexpr Pixel_RGB888 azure()              { return Pixel_RGB888(240, 255, 255); }
    static constexpr Pixel_RGB888 beige()              { return Pixel_RGB888(245, 245, 220); }
    static constexpr Pixel_RGB888 bisque()             { return Pixel_RGB888(255, 228, 196); }
    static constexpr Pixel_RGB888 blanchedalmond()     { return Pixel_RGB888(255, 235, 205); }
    static constexpr Pixel_RGB888 blueviolet()         { return Pixel_RGB888(138, 43,  226); }
    static constexpr Pixel_RGB888 brown()              { return Pixel_RGB888(165, 42,  42); }
    static constexpr Pixel_RGB888 burlywood()          { return Pixel_RGB888(222, 184, 135); }
    static constexpr Pixel_RGB888 cadetblue()          { return Pixel_RGB888(95,  158, 160); }
    static constexpr Pixel_RGB888 chartreuse()         { return Pixel_RGB888(127, 255, 0); }
    static constexpr Pixel_RGB888 chocolate()          { return Pixel_RGB888(210, 105, 30); }
    static constexpr Pixel_RGB888 coral()              { return Pixel_RGB888(255, 127, 80); }
    static constexpr Pixel_RGB888 cornflowerblue()     { return Pixel_RGB888(100, 149, 237); }
    static constexpr Pixel_RGB888 cornsilk()           { return Pixel_RGB888(255, 248, 220); }
    static constexpr Pixel_RGB888 crimson()            { return Pixel_RGB888(220, 20,  60); }
    static constexpr Pixel_RGB888 darkblue()           { return Pixel_RGB888(0,   0,   139); }
    static constexpr Pixel_RGB888 darkcyan()           { return Pixel_RGB888(0,   139, 139); }
    static constexpr Pixel_RGB888 darkgoldenrod()      { return Pixel_RGB888(184, 134, 11); }
    static constexpr Pixel_RGB888 darkgray()           { return Pixel_RGB888(169, 169, 169); }
    static constexpr Pixel_RGB888 darkgreen()          { return Pixel_RGB888(0,   100, 0); }
    static constexpr Pixel_RGB888 darkkhaki()          { return Pixel_RGB888(189, 183, 107); }
    static constexpr Pixel_RGB888 darkmagenta()        { return Pixel_RGB888(139, 0,   139); }
    static constexpr Pixel_RGB888 darkolivegreen()     { return Pixel_RGB888(85,  107, 47); }
    static constexpr Pixel_RGB888 darkorange()         { return Pixel_RGB888(255, 140, 0); }
    static constexpr Pixel_RGB888 darkorchid()         { return Pixel_RGB888(153, 50,  204); }
    static constexpr Pixel_RGB888 darkred()            { return Pixel_RGB888(139, 0,   0); }
    static constexpr Pixel_RGB888 darksalmon()         { return Pixel_RGB888(233, 150, 122); }
    static constexpr Pixel_RGB888 darkseagreen()       { return Pixel_RGB888(143, 188, 143); }
    static constexpr Pixel_RGB888 darkslateblue()      { return Pixel_RGB888(72,  61,  139); }
    static constexpr Pixel_RGB888 darkslategray()      { return Pixel_RGB888(47,  79,  79); }
    static constexpr Pixel_RGB888 darkturquoise()      { return Pixel_RGB888(0,   206, 209); }
    static constexpr Pixel_RGB888 darkviolet()         { return Pixel_RGB888(148, 0,   211); }
    static constexpr Pixel_RGB888 deeppink()           { return Pixel_RGB888(255, 20,  147); }
    static constexpr Pixel_RGB888 deepskyblue()        { return Pixel_RGB888(0,   191, 255); }
    static constexpr Pixel_RGB888 dimgray()            { return Pixel_RGB888(105, 105, 105); }
    static constexpr Pixel_RGB888 dodgerblue()         { return Pixel_RGB888(30,  144, 255); }
    static constexpr Pixel_RGB888 firebrick()          { return Pixel_RGB888(178, 34,  34); }
    static constexpr Pixel_RGB888 floralwhite()        { return Pixel_RGB888(255, 250, 240); }
    static constexpr Pixel_RGB888 forestgreen()        { return Pixel_RGB888(34,  139, 34); }
    static constexpr Pixel_RGB888 fuchsia()            { return Pixel_RGB888(255, 0,   255); }
    static constexpr Pixel_RGB888 gainsboro()          { return Pixel_RGB888(220, 220, 220); }
    static constexpr Pixel_RGB888 ghostwhite()         { return Pixel_RGB888(248, 248, 255); }
    static constexpr Pixel_RGB888 gold()               { return Pixel_RGB888(255, 215, 0); }
    static constexpr Pixel_RGB888 goldenrod()          { return Pixel_RGB888(218, 165, 32); }
    static constexpr Pixel_RGB888 gray()               { return Pixel_RGB888(128, 128, 128); }
    static constexpr Pixel_RGB888 grey()               { return Pixel_RGB888(128, 128, 128); }
    static constexpr Pixel_RGB888 greenyellow()        { return Pixel_RGB888(173, 255, 47); }
    static constexpr Pixel_RGB888 honeydew()           { return Pixel_RGB888(240, 255, 240); }
    static constexpr Pixel_RGB888 hotpink()            { return Pixel_RGB888(255, 105, 180); }
    static constexpr Pixel_RGB888 indianred()          { return Pixel_RGB888(205, 92,  92); }
    static constexpr Pixel_RGB888 indigo()             { return Pixel_RGB888(75,  0,   130); }
    static constexpr Pixel_RGB888 ivory()              { return Pixel_RGB888(255, 255, 240); }
    static constexpr Pixel_RGB888 khaki()              { return Pixel_RGB888(240, 230, 140); }
    static constexpr Pixel_RGB888 lavender()           { return Pixel_RGB888(230, 230, 250); }
    static constexpr Pixel_RGB888 lavenderblush()      { return Pixel_RGB888(255, 240, 245); }
    static constexpr Pixel_RGB888 lawngreen()          { return Pixel_RGB888(124, 252, 0); }
    static constexpr Pixel_RGB888 lemonchiffon()       { return Pixel_RGB888(255, 250, 205); }
    static constexpr Pixel_RGB888 lightblue()          { return Pixel_RGB888(173, 216, 230); }
    static constexpr Pixel_RGB888 lightcoral()         { return Pixel_RGB888(240, 128, 128); }
    static constexpr Pixel_RGB888 lightcyan()          { return Pixel_RGB888(224, 255, 255); }
    static constexpr Pixel_RGB888 lightgoldenrodyellow(){ return Pixel_RGB888(250, 250, 210); }
    static constexpr Pixel_RGB888 lightgray()          { return Pixel_RGB888(211, 211, 211); }
    static constexpr Pixel_RGB888 lightgrey()          { return Pixel_RGB888(211, 211, 211); }
    static constexpr Pixel_RGB888 lightgreen()         { return Pixel_RGB888(144, 238, 144); }
    static constexpr Pixel_RGB888 lightpink()          { return Pixel_RGB888(255, 182, 193); }
    static constexpr Pixel_RGB888 lightsalmon()        { return Pixel_RGB888(255, 160, 122); }
    static constexpr Pixel_RGB888 lightseagreen()      { return Pixel_RGB888(32,  178, 170); }
    static constexpr Pixel_RGB888 lightskyblue()       { return Pixel_RGB888(135, 206, 250); }
    static constexpr Pixel_RGB888 lightslategray()     { return Pixel_RGB888(119, 136, 153); }
    static constexpr Pixel_RGB888 lightslategrey()     { return Pixel_RGB888(119, 136, 153); }
    static constexpr Pixel_RGB888 lightsteelblue()     { return Pixel_RGB888(176, 196, 222); }
    static constexpr Pixel_RGB888 lightyellow()        { return Pixel_RGB888(255, 255, 224); }
    static constexpr Pixel_RGB888 lime()               { return Pixel_RGB888(0,   255, 0); }
    static constexpr Pixel_RGB888 limegreen()          { return Pixel_RGB888(50,  205, 50); }
    static constexpr Pixel_RGB888 linen()              { return Pixel_RGB888(250, 240, 230); }
    static constexpr Pixel_RGB888 maroon()             { return Pixel_RGB888(128, 0,   0); }
    static constexpr Pixel_RGB888 mediumaquamarine()   { return Pixel_RGB888(102, 205, 170); }
    static constexpr Pixel_RGB888 mediumblue()         { return Pixel_RGB888(0,   0,   205); }
    static constexpr Pixel_RGB888 mediumorchid()       { return Pixel_RGB888(186, 85,  211); }
    static constexpr Pixel_RGB888 mediumpurple()       { return Pixel_RGB888(147, 112, 219); }
    static constexpr Pixel_RGB888 mediumseagreen()     { return Pixel_RGB888(60,  179, 113); }
    static constexpr Pixel_RGB888 mediumslateblue()    { return Pixel_RGB888(123, 104, 238); }
    static constexpr Pixel_RGB888 mediumspringgreen()  { return Pixel_RGB888(0,   250, 154); }
    static constexpr Pixel_RGB888 mediumturquoise()    { return Pixel_RGB888(72,  209, 204); }
    static constexpr Pixel_RGB888 mediumvioletred()    { return Pixel_RGB888(199, 21,  133); }
    static constexpr Pixel_RGB888 midnightblue()       { return Pixel_RGB888(25,  25,  112); }
    static constexpr Pixel_RGB888 mintcream()          { return Pixel_RGB888(245, 255, 250); }
    static constexpr Pixel_RGB888 mistyrose()          { return Pixel_RGB888(255, 228, 225); }
    static constexpr Pixel_RGB888 moccasin()           { return Pixel_RGB888(255, 228, 181); }
    static constexpr Pixel_RGB888 navajowhite()        { return Pixel_RGB888(255, 222, 173); }
    static constexpr Pixel_RGB888 navy()               { return Pixel_RGB888(0,   0,   128); }
    static constexpr Pixel_RGB888 oldlace()            { return Pixel_RGB888(253, 245, 230); }
    static constexpr Pixel_RGB888 olive()              { return Pixel_RGB888(128, 128, 0); }
    static constexpr Pixel_RGB888 olivedrab()          { return Pixel_RGB888(107, 142, 35); }
    static constexpr Pixel_RGB888 orange()             { return Pixel_RGB888(255, 165, 0); }
    static constexpr Pixel_RGB888 orangered()          { return Pixel_RGB888(255, 69,  0); }
    static constexpr Pixel_RGB888 orchid()             { return Pixel_RGB888(218, 112, 214); }
    static constexpr Pixel_RGB888 palegoldenrod()      { return Pixel_RGB888(238, 232, 170); }
    static constexpr Pixel_RGB888 palegreen()          { return Pixel_RGB888(152, 251, 152); }
    static constexpr Pixel_RGB888 paleturquoise()      { return Pixel_RGB888(175, 238, 238); }
    static constexpr Pixel_RGB888 palevioletred()      { return Pixel_RGB888(219, 112, 147); }
    static constexpr Pixel_RGB888 papayawhip()         { return Pixel_RGB888(255, 239, 213); }
    static constexpr Pixel_RGB888 peachpuff()          { return Pixel_RGB888(255, 218, 185); }
    static constexpr Pixel_RGB888 peru()               { return Pixel_RGB888(205, 133, 63); }
    static constexpr Pixel_RGB888 pink()               { return Pixel_RGB888(255, 192, 203); }
    static constexpr Pixel_RGB888 plum()               { return Pixel_RGB888(221, 160, 221); }
    static constexpr Pixel_RGB888 powderblue()         { return Pixel_RGB888(176, 224, 230); }
    static constexpr Pixel_RGB888 purple()             { return Pixel_RGB888(128, 0,   128); }
    static constexpr Pixel_RGB888 rebeccapurple()      { return Pixel_RGB888(102, 51,  153); }
    static constexpr Pixel_RGB888 rosybrown()          { return Pixel_RGB888(188, 143, 143); }
    static constexpr Pixel_RGB888 royalblue()          { return Pixel_RGB888(65,  105, 225); }
    static constexpr Pixel_RGB888 saddlebrown()        { return Pixel_RGB888(139, 69,  19); }
    static constexpr Pixel_RGB888 salmon()             { return Pixel_RGB888(250, 128, 114); }
    static constexpr Pixel_RGB888 sandybrown()         { return Pixel_RGB888(244, 164, 96); }
    static constexpr Pixel_RGB888 seagreen()           { return Pixel_RGB888(46,  139, 87); }
    static constexpr Pixel_RGB888 seashell()           { return Pixel_RGB888(255, 245, 238); }
    static constexpr Pixel_RGB888 sienna()             { return Pixel_RGB888(160, 82,  45); }
    static constexpr Pixel_RGB888 silver()             { return Pixel_RGB888(192, 192, 192); }
    static constexpr Pixel_RGB888 skyblue()            { return Pixel_RGB888(135, 206, 235); }
    static constexpr Pixel_RGB888 slateblue()          { return Pixel_RGB888(106, 90,  205); }
    static constexpr Pixel_RGB888 slategray()          { return Pixel_RGB888(112, 128, 144); }
    static constexpr Pixel_RGB888 slategrey()          { return Pixel_RGB888(112, 128, 144); }
    static constexpr Pixel_RGB888 snow()               { return Pixel_RGB888(255, 250, 250); }
    static constexpr Pixel_RGB888 springgreen()        { return Pixel_RGB888(0,   255, 127); }
    static constexpr Pixel_RGB888 steelblue()          { return Pixel_RGB888(70,  130, 180); }
    static constexpr Pixel_RGB888 tan()                { return Pixel_RGB888(210, 180, 140); }
    static constexpr Pixel_RGB888 teal()               { return Pixel_RGB888(0,   128, 128); }
    static constexpr Pixel_RGB888 thistle()            { return Pixel_RGB888(216, 191, 216); }
    static constexpr Pixel_RGB888 tomato()             { return Pixel_RGB888(255, 99,  71); }
    static constexpr Pixel_RGB888 turquoise()          { return Pixel_RGB888(64,  224, 208); }
    static constexpr Pixel_RGB888 violet()             { return Pixel_RGB888(238, 130, 238); }
    static constexpr Pixel_RGB888 wheat()              { return Pixel_RGB888(245, 245, 220); }
    static constexpr Pixel_RGB888 whitesmoke()         { return Pixel_RGB888(245, 245, 245); }
    static constexpr Pixel_RGB888 yellowgreen()        { return Pixel_RGB888(154, 205, 50); }
    
    // Assignment operator
    Pixel_RGB888& operator=(const Pixel_RGB888& other)
    {
        r = other.r;
        g = other.g;
        b = other.b;
        return *this;
    }
    
    // Comparison operators
    bool operator==(const Pixel_RGB888& other) const 
    { 
        return (r == other.r) && (g == other.g) && (b == other.b); 
    }
    
    bool operator!=(const Pixel_RGB888& other) const 
    { 
        return !(*this == other); 
    }
    
    // Arithmetic operators for color blending/accumulation
    Pixel_RGB888 operator+(const Pixel_RGB888& other) const
    {
        return Pixel_RGB888(
            ((r + other.r) > 255) ? 255 : (r + other.r),
            ((g + other.g) > 255) ? 255 : (g + other.g),
            ((b + other.b) > 255) ? 255 : (b + other.b)
        );
    }
    
    Pixel_RGB888& operator+=(const Pixel_RGB888& other)
    {
        r = ((r + other.r) > 255) ? 255 : (r + other.r);
        g = ((g + other.g) > 255) ? 255 : (g + other.g);
        b = ((b + other.b) > 255) ? 255 : (b + other.b);
        return *this;
    }
    
    // Conversion operator to uint8_t* (returns pointer to the byte storage)
    explicit operator uint8_t*() 
    { 
        return reinterpret_cast<uint8_t*>(this); 
    }
    
    explicit operator const uint8_t*() const 
    { 
        return reinterpret_cast<const uint8_t*>(this); 
    }
};


/**
 * @class Pixel_Grayscale
 * @brief 8-bit Grayscale format (monochrome with 256 levels)
 * 
 * Memory layout: GGGGGGGG
 * - Grayscale: 8 bits (0=black, 255=white)
 * 
 * Total: 1 byte per pixel
 * Example: 240x240 display = 57,600 bytes (56.25 KB)
 * 
 * Used by:
 * - E-ink displays (Kindle, e-readers)
 * - Monochrome OLED displays (with 256 levels)
 * - Low-power display applications
 */
struct Pixel_Grayscale
{
    uint8_t value;
    
    // Default constructor
    Pixel_Grayscale() : value(0) {}
    
    // Constructor from grayscale value
    explicit Pixel_Grayscale(uint8_t v) : value(v) {}
    
    // Constructor from RGB (converts to grayscale using standard formula)
    Pixel_Grayscale(uint8_t r, uint8_t g, uint8_t b)
    {
        // Standard luminosity formula for RGB to grayscale conversion
        // Y = 0.299*R + 0.587*G + 0.114*B
        value = (uint8_t)(0.299f * r + 0.587f * g + 0.114f * b);
    }
    
    // Extract grayscale value
    uint8_t gray() const { return value; }
    
    // For RGB compatibility, return same value for all channels
    uint8_t red() const   { return value; }
    uint8_t green() const { return value; }
    uint8_t blue() const  { return value; }
    
    // Static color constants
    static constexpr Pixel_Grayscale Black() { return Pixel_Grayscale(0); }
    static constexpr Pixel_Grayscale White() { return Pixel_Grayscale(255); }
    static constexpr Pixel_Grayscale Gray25() { return Pixel_Grayscale(64); }
    static constexpr Pixel_Grayscale Gray50() { return Pixel_Grayscale(128); }
    static constexpr Pixel_Grayscale Gray75() { return Pixel_Grayscale(192); }
    
    // Assignment operator
    Pixel_Grayscale& operator=(const Pixel_Grayscale& other)
    {
        value = other.value;
        return *this;
    }
    
    // Comparison operators
    bool operator==(const Pixel_Grayscale& other) const { return value == other.value; }
    bool operator!=(const Pixel_Grayscale& other) const { return value != other.value; }
    
    // Arithmetic operators for color blending/accumulation
    Pixel_Grayscale operator+(const Pixel_Grayscale& other) const
    {
        return Pixel_Grayscale(((value + other.value) > 255) ? 255 : (value + other.value));
    }
    
    Pixel_Grayscale& operator+=(const Pixel_Grayscale& other)
    {
        value = ((value + other.value) > 255) ? 255 : (value + other.value);
        return *this;
    }
    
    // Conversion operator to/from RGB888
    explicit operator Pixel_RGB888() const
    {
        return Pixel_RGB888(value, value, value);
    }
    
    Pixel_Grayscale& operator=(const Pixel_RGB888& other)
    {
        // Standard luminosity formula: Y = 0.299*R + 0.587*G + 0.114*B
        value = (uint8_t)(0.299f * other.red() + 0.587f * other.green() + 0.114f * other.blue());
        return *this;
    }
    
    // Conversion operator to uint8_t* (returns pointer to the byte value)
    explicit operator uint8_t*() 
    { 
        return reinterpret_cast<uint8_t*>(this); 
    }
    
    explicit operator const uint8_t*() const 
    { 
        return reinterpret_cast<const uint8_t*>(this); 
    }
};
