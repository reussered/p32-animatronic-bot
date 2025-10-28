# Goblin Head Dual Eye Display Test Harness

This test harness validates the initialization and operation of dual GC9A01 displays for the goblin head subsystem.

## Test Configuration

The test uses the `test_goblin_head_dual_eyes.json` bot configuration which includes:
- Left eye display (goblin_left_eye.json)
- Right eye display (goblin_right_eye.json)
- Hardware validation for pin assignments and bus interfaces

## Generated Components

The code generation creates dispatch tables for 12 components:
1. heartbeat - System heartbeat
2. network_monitor - Network monitoring
3. goblin_left_eye - Left eye display animation
4. goblin_eye - Shared goblin eye processing logic
5. gc9a01 - GC9A01 display driver (left)
6. generic_spi_display - SPI display interface (left)
7. generic_spi_display - SPI display interface (left, secondary)
8. goblin_right_eye - Right eye display animation
9. goblin_eye - Shared goblin eye processing logic
10. gc9a01 - GC9A01 display driver (right)
11. generic_spi_display - SPI display interface (right)
12. generic_spi_display - SPI display interface (right, secondary)

## Building and Running

### Option 1: Standard Component System
Build normally - this runs all components including the display test:
```bash
idf.py build
idf.py flash
idf.py monitor
```

### Option 2: Dedicated Test Harness
Add `DUAL_EYE_DISPLAY_TEST=1` to your build flags for focused testing:
```bash
idf.py build -D DUAL_EYE_DISPLAY_TEST=1
idf.py flash
idf.py monitor
```

## Test Validation

The test harness validates:
- ✅ Component initialization order
- ✅ Pin assignment conflicts
- ✅ Hardware interface setup (SPI bus)
- ✅ Display driver initialization
- ✅ Component action execution timing
- ✅ Dual display coordination

## Expected Output

```
I (123) DUAL_EYE_TEST: Starting Goblin Head Dual Eye Display Test
I (125) DUAL_EYE_TEST: Testing display component initialization...
I (127) DUAL_EYE_TEST: Component 0 initialized successfully
I (129) DUAL_EYE_TEST: Component 1 initialized successfully
...
I (200) DUAL_EYE_TEST: All display components initialized successfully!
I (202) DUAL_EYE_TEST: Display initialization test PASSED
I (204) DUAL_EYE_TEST: Starting continuous display action testing...
```

## Hardware Requirements

- ESP32-S3 development board
- Two GC9A01 240x240 round LCD displays
- SPI bus connections (MOSI, MISO, SCK, CS pins)
- GPIO pins for display control (DC, RST, BL)

## Troubleshooting

- Check SPI pin assignments in component configurations
- Verify display power connections (3.3V)
- Monitor ESP32 serial output for initialization errors
- Check component header files exist (.hdr files in components/ directory)