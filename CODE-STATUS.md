# Code Status Tracking

## Status Definitions

-  **DONE** - Code is final, complete, locked. DO NOT MODIFY.
-  **IN PROGRESS** - Code is being actively developed
-  **BROKEN** - Code has errors and needs fixing
-  **TODO** - Code doesn't exist yet, needs to be written

## Current Status

###  DONE - DO NOT TOUCH

**docs/CORE-LOOP-SPEC.md**
- Status: LOCKED FINAL
- Last modified: 2025-10-15
- The 5-line core loop is IMMUTABLE
- NO vTaskDelay, NO arguments, runs full speed

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

###  BROKEN - NEEDS FIXING

**src/main.c**
- Current state: Has vTaskDelay, passes arguments, wrong implementation
- Needs to match: CORE-LOOP-SPEC.md exactly
- Fix required: Replace with the 5-line version

###  TODO - NOT YET WRITTEN

**include/p32_shared_state.h** - Global state structure
**src/p32_shared_state.cpp** - Global state implementation  
**src/components/esp_now_mesh.cpp** - Mesh synchronization
**Component implementations** - Hardware drivers

---

## Rules for AI Agent

1. **NEVER modify  DONE code** - Even if you think it needs improvement
2. **Only fix  BROKEN code** - And only to match the spec
3. **Write  TODO code** - Following existing patterns
4. **Update this file** - When status changes

5. **If you think DONE code needs changing:**
   - STOP
   - Ask the human first
   - Explain why
   - Wait for permission

**The human decides when code is DONE, not you.**
