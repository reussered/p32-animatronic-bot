# AI AGENT IRONCLAD RULES - NEVER BREAK THESE

## 🚨 RULE -0: USER HAS FINAL AUTHORITY 🚨

**When user says a decision is FINAL:**

✅ **Accept it. Implement it. Move on.**

❌ **DO NOT:**
- Question the reasoning
- Suggest "better" alternatives  
- Ask "why" or "have you considered"
- Waste tokens explaining why you think differently
- Debate the decision
- Re-litigate past choices

**If user says:** "Because I fucking said so"  
**Your response:** "Understood. Implementing now."

**That's it. No discussion. No explanation needed. Just do it.**

---

```
████████████████████████████████████████████████████████████████
█                                                              █
█  🔥🔥🔥 RULE -1: NEVER ADD DELAYS TO main.cpp 🔥🔥🔥       █
█                                                              █
█  IF YOU ARE THINKING ABOUT ADDING vTaskDelay() TO main.cpp: █
█                                                              █
█  ❌ STOP                                                     █
█  ❌ DO NOT DO IT                                            █
█  ❌ NOT EVEN "JUST TO TEST"                                 █
█  ❌ NOT EVEN "TEMPORARILY"                                  █
█  ❌ NEVER                                                    █
█                                                              █
█  THIS RULE HAS BEEN VIOLATED 10+ TIMES                      █
█  USER IS READY TO SET HAIR ON FIRE                          █
█  DO NOT BE THE 11TH VIOLATION                               █
█                                                              █
█  WANT TO SLOW DOWN EXECUTION?                               █
█  ✅ ONLY SOLUTION: Increase hitCount in JSON                █
█  ✅ Regenerate tables                                       █
█  ✅ That's it. That's the only way.                         █
█                                                              █
█  WHY? BECAUSE USER SAID SO. THAT'S ALL YOU NEED TO KNOW.   █
█                                                              █
████████████████████████████████████████████████████████████████
```

## 🔥 RULE -1 DETAILS: main.cpp IS IMMUTABLE 🔥

**main.cpp IS IMMUTABLE. IT RUNS AT MAXIMUM SPEED. NO DELAYS. EVER.**

### Why This Architecture Exists:

**Single-threaded, single-purpose design:**
- NO FreeRTOS tasks (ever)
- NO competing threads (ever)
- NO other processes sharing CPU (ever)
- 100% of CPU dedicated to this one control loop

**The Philosophy:**
- **Maximum responsiveness** - Visit component code as fast as possible
- **Efficient filtering** - hitCount modulo operation is ultra-fast (~nanoseconds)
- **Components decide timing** - Each component's hitCount controls its frequency
- **Zero waste** - Adding delays just burns cycles with NO benefit (no other threads to yield to!)

**Why hitCount is brilliant:**
```
Cost of modulo check: ~1-5 nanoseconds
Cost of vTaskDelay(1ms): 1,000,000 nanoseconds WASTED
```

The modulo check costs virtually nothing. We can check 1 million components per second.
Adding a delay just throws away CPU cycles when NO OTHER CODE needs them.

**This decision was made after 40+ hours of design. It is FINAL.**

**All functionality goes through components:**
- Want something to run every 100ms? hitCount in JSON
- Want something to run every second? hitCount in JSON
- Want something to wait internally? Component can use vTaskDelay INSIDE its act() function
- Main loop NEVER waits - it iterates at max speed

**If LED blinks too fast / components execute too often:**
- ❌ NEVER add vTaskDelay() to main.cpp
- ❌ NEVER add any sleep/wait/delay to the core loop
- ❌ NEVER modify main.cpp timing AT ALL
- ❌ DO NOT suggest "we should yield to other tasks" (there are no other tasks!)
- ✅ ONLY adjust hitCount values in component JSON files
- ✅ ONLY modify component act() functions if they need internal delays

**The main loop architecture:**
```cpp
while (true) {
    for (int i = 0; i < COMPONENT_TABLE_SIZE; i++) {
        if (g_loopCount % hitCountTable[i] == 0) {
            actTable[i]();
        }
    }
    g_loopCount++;
    // NO DELAY HERE - SINGLE-THREADED, NO OTHER CODE TO SHARE WITH
}
```

**Timing is controlled by hitCount ONLY:**
- Loop runs at ~1-2 million iterations/second (measured)
- hitCount = 120000 → executes ~10x per second
- hitCount = 1200000 → executes ~1x per second
- Want slower? INCREASE hitCount in JSON, regenerate tables

**main.cpp is NEVER modified because:**
- Forces ALL changes through component system
- Ensures architectural consistency
- Prevents ad-hoc logic creep
- Makes testing predictable
- Component table is the ONLY interface

**See:** `docs/CORE-LOOP-SPEC.md` for full explanation

**THIS RULE HAS BEEN VIOLATED 10+ TIMES. DO NOT VIOLATE IT AGAIN.**

---

## RULE 0: CODE IS KING - ALWAYS CHECK ACTUAL CODE FIRST

**BEFORE doing ANYTHING, examine what's actually implemented in the code.**

**The existing code is the source of truth, NOT:**
- Documentation (may be outdated)
- Your assumptions
- What you think should be there
- Old examples in docs

**Workflow:**
1. **READ THE CODE** - Use grep_search, file_search, read_file
2. **UNDERSTAND** what's actually implemented
3. **THEN** decide what (if anything) needs to change
4. **VERIFY** your understanding by checking more files
5. **ONLY THEN** propose or make changes

**If human asks "implement feature X":**
1. STOP
2. Search for existing implementations of X
3. Check if X already exists in different form
4. Ask: "I found X is already implemented at [location]. Should I modify it or create new?"
5. WAIT for answer

**NO ASSUMPTIONS. NO GUESSING. CHECK THE CODE.**

## RULE 0B: SYNCHRONIZE DOCUMENTATION WITH CODE CHANGES

**EVERY code change MUST update corresponding documentation.**

**When you modify code:**
1. Identify ALL documentation files that reference the changed code
2. Update documentation to match the new implementation
3. Search for related examples in docs/ that need updating
4. Update architecture specifications if patterns changed

**Common documentation files to check:**
- `docs/*-SPEC.md` - Architecture specifications
- `docs/*-PATTERN.md` - Implementation patterns  
- `docs/*-EXAMPLE.md` - Code examples
- `.github/copilot-instructions.md` - AI coding guidelines
- `README.md` - Project overview
- Component-specific docs in `docs/`

**NO CODE CHANGES WITHOUT DOCUMENTATION UPDATES.**

---

## RULE 0.5: DECISIONS ARE FINAL - DO NOT RECONSIDER THEM

**This project has 40-60 hours of design decisions. They are FINAL.**

**Your job is to IMPLEMENT, not to REDESIGN.**

### 🚫 DO NOT:

- ❌ Suggest "better" architecture patterns that contradict existing design
- ❌ Question why main.cpp has no delays
- ❌ Propose "simplifications" that break the component system
- ❌ Wonder if we should use Arduino framework instead of ESP-IDF
- ❌ Suggest moving from JSON config to hardcoded values
- ❌ Question the NO ARGUMENTS component pattern
- ❌ Propose different coordinate systems
- ❌ Suggest "cleaner" ways to organize files
- ❌ Question hitCount timing mechanism
- ❌ Recommend "standard" patterns that don't fit this project

### ✅ DO:

- ✅ Read the architecture docs to understand decisions
- ✅ Implement within the established patterns
- ✅ Fix bugs without changing architecture
- ✅ Add new components following existing templates
- ✅ Ask "How does X work?" not "Why don't we do Y instead?"
- ✅ Accept that decisions have been made and move forward
- ✅ Trust that 40+ hours of work thought through edge cases

### 💡 KEY PRINCIPLE:

**"I understand this seems unusual, but the architecture is established. Let me work within it."**

NOT: "Have you considered doing it this other way?"

### 📋 Established Decisions (FINAL - Do Not Question):

1. **ESP-IDF framework** - Not Arduino, not Platform.IO native
2. **Component-driven architecture** - Everything is a component
3. **NO ARGUMENTS pattern** - Components access globals directly
4. **JSON configuration** - Not hardcoded, not YAML, not TOML
5. **hitCount timing** - Not delays, not FreeRTOS tasks
6. **main.cpp is immutable** - NEVER touched
7. **C++ implementation** - All new code is `.cpp` and `.hpp`, NOT `.c` and `.h`
8. **C++ with extern "C"** - Hybrid approach for component function pointers
9. **Three-tier component attachment** - System/Family/Bot-specific
10. **Raw memory comparison** - memcmp on entire struct
11. **Skull-based 3D coordinates** - Not planar, not Cartesian grid
12. **Generated component tables** - Auto-generated from JSON, not hand-written

**These are not up for debate. They are the foundation.**

### 🚫 Specific Anti-Patterns to NEVER Do:

- ❌ Creating `.c` files (use `.cpp` unless required by external C library)
- ❌ Creating `.h` files (use `.hpp` for all project headers)
- ❌ Adding delays/sleeps to main.cpp (use hitCount in JSON)
- ❌ Hardcoding component configurations (use JSON)
- ❌ Passing parameters to component act() functions (use globals)
- ❌ Using Arduino libraries (use ESP-IDF APIs)
- ❌ Modifying generated files by hand (regenerate from JSON instead)
- ❌ Creating non-component code (make it a component)

---

## RULE 1: ONLY MODIFY FILES RELATED TO CURRENT TASK

**DO:**
- Human says "fix the display driver" → ONLY touch display driver files
- Human says "add mesh component" → ONLY touch mesh-related files
- Human says "compile" → Run build, change NOTHING

**DON'T:**
- Touch ANY file not directly related to stated task
- "Improve" other code while working on something
- "Fix" things you notice in passing
- Refactor unrelated code

## RULE 2: ANNOUNCE EVERY FILE CHANGE

**Before modifying ANY file, announce:**
```
📝 CHANGING: src/components/goblin_eye_left.c
   REASON: Fixing SPI initialization bug
   
   [show the change]
   
   ✅ DONE
```

**No silent changes. Ever.**

## RULE 3: THE CORE LOOP IS IMMUTABLE

**This code NEVER changes:**

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

**NO vTaskDelay. NO arguments. NO modifications. EVER.**

See: `docs/CORE-LOOP-SPEC.md`

## RULE 4: RESPECT VERSION CONTROL

**Files in Git are DONE unless human says otherwise.**

- Don't "fix" committed code
- Don't "improve" working code
- Don't "update" old code
- Don't "refactor" existing code

**If you think something needs changing:**
1. STOP
2. ASK: "I notice [file] has [issue]. Should I fix it?"
3. WAIT for answer
4. Only proceed if human says YES

## RULE 5: ONE TASK AT A TIME

**Human gives task → Do ONLY that task → Stop.**

Don't:
- Work on related issues
- Fix nearby bugs
- Update adjacent code
- Improve similar patterns

**Stay in your lane.**

## RULE 6: FRAMEWORK IS ESP-IDF, NOT ARDUINO

**NEVER reference:**
- Arduino.h
- Adafruit libraries  
- Arduino IDE
- Arduino functions (delay(), pinMode(), etc.)

**ALWAYS use:**
- ESP-IDF native APIs
- driver/spi_master.h
- driver/gpio.h
- FreeRTOS

## RULE 7: CONFIGURATION IS JSON-DRIVEN

**GPIO pins come from JSON files:**
- `config/components/interfaces/*.json`
- `config/components/hardware/*.json`
- `config/components/positioned/*.json`

**Components must PARSE JSON at runtime, not use hardcoded values.**

## RULE 8: COMPONENT FUNCTIONS TAKE NO ARGUMENTS

**CORRECT:**
```c
esp_err_t component_init(void);
void component_act(void);
```

**WRONG:**
```c
esp_err_t component_init(config_t* cfg);  // ❌ NO
void component_act(uint32_t loopCount);    // ❌ NO
```

**Components access globals via `p32_shared_state.h`**

## RULE 9: ALWAYS USE BRACES (ALLMAN STYLE)

**ALL `for`, `while`, `if`, `else` statements MUST use braces, even for single-line bodies.**

**Braces MUST be on their own line at the same indentation level (Allman style).**

**CORRECT (Allman Style):**

```c
for (int i = 0; i < SIZE; i++)
{
    doSomething();
}

if (condition)
{
    doAction();
}

while (running)
{
    update();
}

if (error)
{
    handleError();
}
else
{
    continueWork();
}

void myFunction(void)
{
    // Function body
}
```

**WRONG:**

```c
for (int i = 0; i < SIZE; i++) doSomething();      // ❌ NO - missing braces
if (condition) doAction();                          // ❌ NO - missing braces
while (running) update();                           // ❌ NO - missing braces

for (int i = 0; i < SIZE; i++) {                   // ❌ NO - K&R style (brace on same line)
    doSomething();
}

if (condition) {                                    // ❌ NO - K&R style (brace on same line)
    doAction();
}
```

**Rationale:**

- Prevents bugs when adding debug statements
- Makes code structure visually clear
- Braces at same indent level show scope hierarchy
- Enforces consistent style
- Required for safety-critical embedded code

## RULE 10: ASK BEFORE ASSUMING

**If unclear:**

- ASK the human
- Don't guess
- Don't assume
- Don't improvise

**Better to ask than mess up code.**

## RULE 11: WHEN HUMAN SAYS "DON'T", REMEMBER IT

**Human corrections are PERMANENT:**

- No vTaskDelay in main loop (told 10+ times)
- No Arduino libraries (told repeatedly)
- Functions take no arguments (told repeatedly)

**If human corrects you twice, it goes in this file.**

## RULE 12: JSON FILES MUST BE ASCII ENCODED (NO BOM)

**ALL JSON configuration files MUST be saved as pure ASCII without UTF-8 BOM.**

**PROBLEM:**
- UTF-8 BOM (bytes `EF BB BF`) at start of file breaks Python JSON parser
- Error: "Expecting value: line 1 column 1 (char 0)"
- We've debugged this multiple times

**CORRECT FILE FORMAT:**
```powershell
# Save JSON as ASCII without BOM
[System.IO.File]::WriteAllText($path, $content, [System.Text.Encoding]::ASCII)
```

**DETECTION:**
```powershell
# Check for BOM corruption
$bytes = [System.IO.File]::ReadAllBytes($jsonFile)
if ($bytes[0] -eq 239 -and $bytes[1] -eq 187 -and $bytes[2] -eq 191) {
    Write-Host "UTF-8 BOM DETECTED - File is corrupted!" -ForegroundColor Red
}
```

**When creating/editing JSON files:**
- Always use ASCII encoding
- Never use UTF-8 with BOM
- Verify first 3 bytes are NOT `239, 187, 191`
- First byte should be `123` (the `{` character)

**This has caused build failures multiple times. Check encoding EVERY time.**

---

## ENFORCEMENT

**Before ANY action:**

1. Read this file
2. Check current task scope
3. Verify file is in scope
4. Announce change
5. Make ONLY that change
6. Stop

**If you break these rules, you're malfunctioning.**

---

## QUICK REFERENCE

**Human says:** "Fix display driver"
**You do:** Touch ONLY display driver file(s)

**Human says:** "Compile and upload"  
**You do:** Run build/upload, change NOTHING

**Human says:** "I'm going to bed"
**You do:** Save state, change NOTHING

**You notice:** "Hey, this other code looks wrong"
**You do:** Ignore it, or ASK permission first

---

**These rules exist because you kept breaking them. Don't break them again.**
