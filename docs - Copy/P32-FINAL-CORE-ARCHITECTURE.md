# P32 Universal Architecture - Final Core Implementation

## IMMUTABLE CORE DESIGN
**This is the final, decade-stable P32 architecture. The central core loop will remain identical.**

### Core Philosophy
- **Universal Main Loop**: Single main.c handles ALL bot types via JSON-driven component tables
- **LoopCount Timing**: No delays - components self-throttle using `loopCount % hitCount`
- **Component Table Architecture**: JSON generates initTable.h/c and actTable.h/c
- **Full-Speed Execution**: ESP32-S3 runs at maximum speed with component-controlled timing

### The Immutable Core Loop
```c
void app_main(void) {
    // Initialize all components from JSON-generated initTable
    for (int i = 0; i < INIT_TABLE_SIZE; i++) {
        if (initTable[i] != NULL) {
            initTable[i]();
        }
    }
    
    // Main action loop - NEVER CHANGES
    while (true) {
        for (int i = 0; i < ACT_TABLE_SIZE; i++) {
            if (actTable[i].act_func != NULL && actTable[i].hitCount > 0) {
                if (loopCount % actTable[i].hitCount == 0) {
                    actTable[i].act_func(loopCount);
                }
            }
        }
        
        loopCount++;
        
        // Yield to FreeRTOS occasionally to prevent watchdog
        if (loopCount % 10000 == 0) {
            vTaskDelay(1);
        }
    }
}
```

### Verified Components for goblin_simple
```
SYSTEM COMPONENTS:
- heartbeat (hitCount: 1) - System heartbeat every loop
- network_monitor (hitCount: 50) - Network checks every 50 loops

BOT COMPONENTS:
- left_eye (hitCount: 5) - Left eye animation every 5 loops  
- right_eye (hitCount: 5) - Right eye animation every 5 loops
- nose_sensor (hitCount: 15) - Proximity sensor every 15 loops
```

### Function Generation Pattern (PERMANENT)
- Init functions: `p32_comp_{component_name}_init()`
- Action functions: `p32_comp_{component_name}_act(uint32_t loopCount)`
- Timing controlled by JSON `"hitCount"` values

### SIMPLE_TEST Implementation
Each component function includes:
```c
#ifdef SIMPLE_TEST
    printf("INIT: {name} - {description}\n");
    return ESP_OK;
#endif
```

### Build System Integration
- `tools/generate_tables.py` reads JSON and generates C files
- `platformio.ini` includes `-DSIMPLE_TEST` for testing
- Component table headers auto-generated from bot JSON configuration

### Future Expansion (Without Core Changes)
- Add more components to JSON  automatic inclusion in tables
- Change hitCount values  automatic timing adjustment  
- New bot types  same core loop, different component tables
- Audio, sensors, displays  all use same architecture

### Files That Never Change
- `src/main.c` - The universal core loop (FINAL VERSION)
- Component table structure and timing logic (IMMUTABLE)
- JSON  C generation workflow (PERMANENT ARCHITECTURE)

### Files That Change Per Bot
- `config/bots/{bot_name}.json` - Component selection and timing
- Generated `initTable.h/c` and `actTable.h/c` - Bot-specific tables
- Generated component implementation files - Bot-specific behaviors

## Historical Significance
This architecture represents the culmination of P32 design principles:
1. **Universal Compatibility** - One main loop serves all animatronics
2. **JSON-Driven Configuration** - Hardware changes don't require code changes
3. **Performance Optimization** - Full-speed execution with precise component timing
4. **Maintainability** - Clear separation between core logic and bot-specific behavior
5. **Scalability** - Supports simple goblin to complex full-body humanoid robots

**Date Established**: October 12, 2025
**Status**: FINAL - No future modifications to core architecture