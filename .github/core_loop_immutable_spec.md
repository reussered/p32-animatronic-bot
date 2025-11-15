# P32 CORE LOOP ARCHITECTURE - UPDATED SPECIFICATION

## FUNDAMENTAL PRINCIPLE: MINIMUM DELAY FOR WATCHDOG COMPLIANCE

The core execution loop delay has been updated to the absolute minimum (1ms) while maintaining FreeRTOS watchdog functionality. This provides maximum responsiveness while ensuring task yielding.

### THE CORE LOOP (UPDATED):

```c
while (true) {
    for (int i = 0; i < ACT_TABLE_SIZE; i++) {
        if (actTable[i].act_func != NULL && actTable[i].hitCount > 0) {
            if (loopCount % actTable[i].hitCount == 0) {
                actTable[i].act_func(loopCount);
            }
        }
    }
    loopCount++;
    vTaskDelay(1);  // 1ms delay - minimum for watchdog compliance
}
```



### WHAT THIS LOOP DOES:

1. **Iterates through actTable** - components generated from JSON configs

2. **Calls component functions** based on hitCount timing mechanism  

3. **Increments loopCount** - global timing counter

4. **Repeats infinitely** at maximum CPU speed



### WHAT IS FORBIDDEN IN THE CORE LOOP:

-  **NO DELAYS** (vTaskDelay, delay(), sleep, etc.)

-  **NO I/O** (Serial.print, ESP_LOGI, file operations)

-  **NO BLOCKING CALLS** (WiFi, HTTP, long calculations)

-  **NO ADDITIONAL LOGIC** (if statements, switches, extra code)

-  **NO TIMING MODIFICATIONS** (loop speed changes)



### WHY THIS LOOP IS IMMUTABLE:

1. **Universal Execution Engine** - Same code runs on ALL ESP32 controllers

2. **Maximum Performance** - No artificial delays or overhead

3. **Component Control** - Timing managed via hitCount, not loop delays

4. **Predictable Behavior** - Deterministic execution across all creatures

5. **Parallel Development** - Teams can work independently knowing loop is fixed



### TIMING IS CONTROLLED BY COMPONENTS:

```c

// Component controls its own timing via hitCount

actTable[0] = {servo_update, 10, "shoulder_servo"};  // Every 10 loops

actTable[1] = {display_update, 50, "left_eye"};      // Every 50 loops  

actTable[2] = {sensor_read, 5, "proximity"};         // Every 5 loops

```



### IF YOU NEED DELAYS OR TIMING:

- **Put them INSIDE component functions** - NOT in the core loop

- **Use hitCount mechanism** for regular intervals

- **Use FreeRTOS tasks** for independent timing requirements



### DEVELOPMENT/DEBUG MODIFICATIONS:

Any debugging, logging, or development aids must be:

- **Added OUTSIDE the core loop** (before/after)

- **Conditional compilation** (#ifdef DEBUG)

- **Removable** without affecting core execution



### VIOLATION CONSEQUENCES:

Modifying the core loop breaks:

- Universal firmware compatibility

- Cross-creature portability  

- Parallel development workflows

- System performance guarantees

- Distributed timing coordination



## THIS LOOP IS THE FOUNDATION - NEVER CHANGE IT