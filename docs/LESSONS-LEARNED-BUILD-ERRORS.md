# Lessons Learned - Build Errors and Solutions

## Purpose
This document captures mistakes made during development and their solutions to prevent repeating them.

---

## ✅ LESSON 1: All Code Must Be C++ (Except main.cpp)

**Date**: October 16, 2025  
**Issue**: `generate_tables.py` was generating `.c` and `.h` files instead of `.cpp` and `.hpp`

### The Problem
- Generated `p32_component_tables.c` → Should be `.cpp`
- Generated `p32_component_tables.h` → Should be `.hpp`
- Generated `system_components.c` → Should be `.cpp`
- Generated `bot_components.c` → Should be `.cpp`
- Build failed with linking errors

### Root Cause
The project migrated to C++ but the code generation script wasn't updated to reflect this.

### The Fix
Updated `tools/generate_tables.py` lines 330-337:

**BEFORE (WRONG)**:
```python
# Source files
source_files = [
    ("p32_component_tables.c", self.generate_unified_implementation()),
    ("system_components.c", self.generate_component_stub("system")),
    ("bot_components.c", self.generate_component_stub("positioned"))
]
```

**AFTER (CORRECT)**:
```python
# Source files - ALL C++ now (.cpp extensions)
source_files = [
    ("p32_component_tables.cpp", self.generate_unified_implementation()),
    ("system_components.cpp", self.generate_component_stub("system")),
    ("bot_components.cpp", self.generate_component_stub("positioned"))
]
```

Also updated header generation to use `.hpp` and add proper `extern "C"` blocks.

### Key Rule
**ALL generated code must be C++ with proper `extern "C"` linkage for component function pointers.**

- ✅ Component implementations: C++ with `extern "C"` functions
- ✅ Headers: `.hpp` with `extern "C"` blocks for function declarations
- ✅ Tables: `.cpp` files
- ❌ NEVER generate `.c` or `.h` files (except main.cpp which is actually C++)

### Verification Steps
After running `python tools/generate_tables.py <bot_name> src`, verify:
```powershell
# Should exist:
Test-Path include/p32_component_tables.hpp  # TRUE
Test-Path src/p32_component_tables.cpp       # TRUE
Test-Path src/system_components.cpp          # TRUE
Test-Path src/bot_components.cpp             # TRUE

# Should NOT exist:
Test-Path include/p32_component_tables.h    # FALSE
Test-Path src/p32_component_tables.c         # FALSE
```

---

## ✅ LESSON 2: main.cpp is IMMUTABLE

**Date**: October 16, 2025  
**Rule**: NEVER modify `src/main.cpp` under ANY circumstances

### The Sacred Code
```cpp
extern "C" void app_main(void)
{
    for (int i = 0; i < COMPONENT_TABLE_SIZE; i++)
    {
        initTable[i]();
    }
    
    while (true)
    {
        for (int i = 0; i < COMPONENT_TABLE_SIZE; i++)
        {
            if (g_loopCount % hitCountTable[i] == 0)
            {
                actTable[i]();
            }
        }
        g_loopCount++;
    }
}
```

### Why It's Immutable
- This is the **core loop specification** - documented in `docs/CORE-LOOP-SPEC.md`
- ALL functionality is added via components, NOT by modifying main.cpp
- ANY change to main.cpp breaks the architectural contract
- The loop is designed to run at maximum speed without delays

### How to Add Functionality
✅ Create new component JSON files
✅ Create component C++ implementation
✅ Regenerate component tables
❌ NEVER touch main.cpp

---

## ✅ LESSON 3: Component Function Names Must Match Generated Pattern

**Date**: October 16, 2025  
**Issue**: Created `simple_core_test_init()` but generator expected `p32_comp_simple_core_test_init()`

### The Pattern
The `generate_tables.py` script uses this function naming convention:
```python
def generate_function_name(self, component_name: str, func_type: str) -> str:
    name = component_name.lower().replace(' ', '_').replace('-', '_')
    return f"p32_comp_{name}_{func_type}"
```

### Example
If your component JSON has:
```json
{
  "component_name": "simple_core_test"
}
```

Then your C++ implementation MUST use:
```cpp
extern "C" esp_err_t p32_comp_simple_core_test_init(void) { ... }
extern "C" void p32_comp_simple_core_test_act(void) { ... }
```

### The Fix
Always check the generated `p32_component_tables.hpp` to see what function names are expected, then match them exactly in your implementation.

---

## ✅ LESSON 4: Use uint64_t for g_loopCount

**Date**: October 16, 2025  
**Issue**: Component used `extern uint32_t g_loopCount` but system uses `uint64_t`

### The Standard
```cpp
// In p32_shared_state.cpp and main.cpp
extern uint64_t g_loopCount;
```

### Why uint64_t?
- Loop runs at ~120,000 iterations/second
- uint32_t wraps after ~10 hours
- uint64_t runs for millions of years
- Some docs show uint32_t (outdated)

### The Fix
Always use `extern uint64_t g_loopCount;` in component implementations.

---

## ✅ LESSON 5: Generated Tables Must Define g_loopCount

**Date**: October 16, 2025  
**Issue**: Linker error: `undefined reference to 'g_loopCount'`

### The Problem
After generating component tables, build failed with:
```
undefined reference to `g_loopCount'
```

### Root Cause
The `generate_tables.py` script generates `p32_component_tables.cpp` but wasn't defining the global `g_loopCount` variable.

### The Fix
Updated `tools/generate_tables.py` in `generate_unified_implementation()`:

```python
content = [
    '#include "p32_component_tables.hpp"',
    "",
    "// ============================================================================",
    "// P32 Component Dispatch Tables - Implementation",
    "// Auto-generated from JSON bot configuration",
    "// ============================================================================",
    "",
    "// Global loop counter - incremented by main.cpp core loop",
    "uint64_t g_loopCount = 0;",
    ""
]
```

### Key Rule
**The generated `p32_component_tables.cpp` must define `g_loopCount` because it's the single source of truth for this global.**

- `main.cpp` uses `extern uint64_t g_loopCount;` (declares it)
- `p32_component_tables.cpp` defines it: `uint64_t g_loopCount = 0;`
- All components access it via `extern uint64_t g_loopCount;`

### Verification Steps
After running `python tools/generate_tables.py <bot_name> src`, verify:
```powershell
# Check that g_loopCount is defined
Select-String -Path src/p32_component_tables.cpp -Pattern "uint64_t g_loopCount"
# Should show: uint64_t g_loopCount = 0;
```

---

## 📝 How to Use This Document

### When Creating New Components
1. Check this document FIRST
2. Follow the patterns shown here
3. If you encounter a NEW error, add it here

### When Updating Scripts
1. Test the changes
2. Document what was fixed
3. Add verification steps

### When Reviewing Code
1. Verify lessons learned are followed
2. Check that main.cpp wasn't touched
3. Confirm C++ conventions are used

---

## 🔄 Template for New Lessons

```markdown
## ✅ LESSON N: [Short Description]

**Date**: YYYY-MM-DD  
**Issue**: [What went wrong]

### The Problem
[Detailed description of the error]

### Root Cause
[Why it happened]

### The Fix
[Code changes or solution]

### Key Rule
[The principle to follow]

### Verification Steps
[How to check it's fixed]
```

---

**Last Updated**: October 16, 2025  
**Next Review**: Before next build after changes to generation scripts
