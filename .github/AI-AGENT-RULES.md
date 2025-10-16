# AI AGENT IRONCLAD RULES - NEVER BREAK THESE

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
