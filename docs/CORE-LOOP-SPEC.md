# Core Loop Specification - THE ENTIRE SYSTEM

## THE COMPLETE CODE (5 LINES)

```c
void app_main(void) {
    for (int i = 0; i < INIT_TABLE_SIZE; i++) initTable[i]();
    
    while (true) {
        for (int i = 0; i < ACT_TABLE_SIZE; i++) 
            if (g_loopCount % actTable[i].hitCount == 0) actTable[i].act_func();
        g_loopCount++;
    }
}
```

## THAT'S IT. NOTHING ELSE.

**NO vTaskDelay. NO delays. NO waits. RUNS FULL SPEED.**

## What This Code Does:

1. **Line 1**: Initialize all components once
2. **Line 2**: Loop forever
3. **Line 3-4**: Execute components when `g_loopCount % hitCount == 0`
4. **Line 5**: Increment the global loop counter

## Performance:

- Loop executes ~120,000 times per second (120 iterations/ms)
- Each iteration: scan component table, execute matching components
- Component table scan takes ~8 microseconds
- Only components with matching hitCount execute

## Timing Examples:

```
g_loopCount = 0:  Execute components with hitCount=1
g_loopCount = 1:  Execute components with hitCount=1
g_loopCount = 2:  Execute components with hitCount=1,2
g_loopCount = 3:  Execute components with hitCount=1,3
g_loopCount = 5:  Execute components with hitCount=1,5
g_loopCount = 10: Execute components with hitCount=1,2,5,10
```

## What Controls Timing:

1. **Natural loop speed** (~8s per iteration)
2. **Component hitCount values** (from JSON config)
3. **Component execution time** (how long each act() takes)

## Component Requirements:

- **init()** - Called once at startup, NO ARGUMENTS
- **act()** - Called based on timing, NO ARGUMENTS, MUST BE FAST
- Components access globals via `p32_shared_state.h`
- Components must NOT block or delay

## What This Is NOT:

-  NOT a task scheduler with delays
-  NOT cooperative multitasking with yields
-  NOT FreeRTOS tasks with vTaskDelay
-  NOT blocking I/O with waits

## What This IS:

-  Pure component execution engine
-  Full-speed loop with modulo-based timing
-  Declarative timing via JSON hitCount values
-  Zero OS overhead - just table scanning and function calls

## Everything Else Is Components:

All functionality lives in components:
- WiFi  `wifi_station` component
- Mesh  `esp_now_mesh` component  
- Displays  `goblin_eye_left`, `goblin_eye_right` components
- Sensors  `goblin_nose` component
- Mood  `goblin_mood` component
- Behavior  `goblin_behavior` component

**The core loop just executes them. That's all it does.**
