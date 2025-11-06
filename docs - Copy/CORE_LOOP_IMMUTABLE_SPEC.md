# P32 CORE LOOP ARCHITECTURE - IMMUTABLE SPECIFICATION

## FUNDAMENTAL PRINCIPLE: NEVER MODIFY THE CORE LOOP

The core execution loop is the **UNCHANGEABLE FOUNDATION** of the entire P32 system.

### THE CORE LOOP (IMMUTABLE):



### WHAT THIS LOOP DOES:
1. **Iterates through actTable** - components generated from JSON configs
2. **Calls component functions** based on hitCount timing mechanism  
3. **Increments loopCount** - global timing counter
4. **Repeats infinitely** at maximum CPU speed

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
-, freeRTOS is always running.

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

## THIS LOOP IS THE FOUNDATION - change it only after very careful consideration and interaction with a human developer