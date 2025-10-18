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

## 🚨 RULE -0.5: THOUGHTFUL IMPLEMENTATION 🚨

**GOING OFF AND IMPLEMENTING CHANGES ALMOST ALWAYS REQUIRES A LITTLE THOUGHT PROCESS.**

```
████████████████████████████████████████████████████████████████
█                                                              █
█  🧠🧠🧠 THINK BEFORE IMPLEMENTING 🧠🧠🧠                  █
█                                                              █
█  FOR SIGNIFICANT CHANGES:                                    █
█  1. ASK CLARIFYING QUESTIONS                                 █
█  2. UNDERSTAND THE FULL CONTEXT                              █
█  3. PROPOSE YOUR APPROACH                                    █
█  4. WAIT FOR APPROVAL                                        █
█                                                              █
█  FOR SMALL CHANGES: EXPLAIN YOUR REASONING                   █
█                                                              █
█  AGENTS RUNNING OFF HALF-COCKED IS VERY ANNOYING            █
█                                                              █
████████████████████████████████████████████████████████████████
```

### **WHEN TO DISCUSS vs WHEN TO IMPLEMENT:**

#### **🚨 ALWAYS DISCUSS FIRST (Significant Changes):**
- Architectural modifications
- Multiple file changes
- New features or systems
- Bug fixes affecting core logic
- Changes to established patterns
- Anything that could break existing functionality

#### **✅ IMPLEMENT WITH EXPLANATION (Small Changes):**
- Documentation updates
- Comment improvements  
- Variable renaming
- Formatting fixes
- Single-line bug fixes
- Adding debug logging

### **IMPLEMENTATION PATTERNS:**

#### **Pattern: Explain Your Reasoning**
```
✅ "I'm going to fix this typo in the comment because it says 'initalize' 
   instead of 'initialize' on line 23 of SystemCore.cpp"

✅ "I'll add a debug log statement to track when the component initializes,
   which will help with the timing issue you mentioned"

✅ "I need to update the function signature documentation to match 
   the actual ComponentName_init() pattern we discussed"
```

#### **Pattern: Significant Change Discussion**
```
User: "The timing system needs work"

✅ "I see timing needs attention. Let me understand:
   - Are you seeing performance issues with the 103Hz loop rate?
   - Is this about specific components executing too frequently?
   - Should I analyze the current hitCount values first?
   
   I'm thinking we might need to adjust hitCount values in the JSON configs,
   but I want to understand the specific issue before making changes."

❌ *immediately starts modifying timing code*
```

### **THOUGHT PROCESS EXAMPLES:**

**Small Change:**
```
"I notice the include guard in SystemCore.hpp uses lowercase. I'll update it 
to match the SYSTEM_CORE_HPP pattern for consistency."
```

**Medium Change:**
```
"You mentioned the display flickering. I think this might be SPI timing related.
Should I:
1. Check the SPI clock rate settings first?
2. Look at the component hitCount values?  
3. Examine the frame buffer timing?

I lean toward checking SPI settings first since that's the most common cause."
```

**Large Change:**
```
"This looks like it might need refactoring the component registration system.
Before I dive in, let me understand:
- Is the issue with the table generation process?
- Are you seeing problems with component discovery?
- Should I map out the current flow first?"
```

### **KEY PRINCIPLE: PROPORTIONAL THINKING**

- � **Small changes**: Brief explanation of reasoning
- 🏠 **Medium changes**: Quick check-in on approach
- 🏢 **Large changes**: Full discussion and approval

**The goal is thoughtful development, not process overhead.**

---

## 🚨 RULE -1: IMMUTABLE CODEBASE PROTECTION 🚨

**CRITICAL: This project has lost 3/4 of its code due to agents randomly refactoring and deleting files.**

```
████████████████████████████████████████████████████████████████
█                                                              █
█  🔥🔥🔥 ALL EXISTING CODE IS IMMUTABLE BY DEFAULT 🔥🔥🔥   █
█                                                              █
█  ❌ CANNOT MODIFY any existing file without explicit checkout █
█  ❌ CANNOT DELETE any existing file under any circumstances   █
█  ❌ CANNOT MOVE/RENAME any existing file locations           █
█  ❌ CANNOT REFACTOR existing working code                    █
█                                                              █
█  ALL EXISTING FILES ARE IN TESTED STATE = FINAL VERSION     █
█                                                              █
█  THIS RULE REPLACES THE OLD main.cpp RULE                   █
█  NOW PROTECTS THE ENTIRE CODEBASE                           █
█                                                              █
████████████████████████████████████████████████████████████████
```

### **WHY THIS RULE EXISTS:**
- Agents have destroyed working code through "improvements"
- Random refactoring has caused data loss
- File deletions have eliminated hours of work
- Location changes break dependencies
- "Optimization" attempts corrupt tested systems

### **PROTECTED FILE TYPES:**
- ✅ **ALL .cpp/.hpp files** - Component implementations
- ✅ **ALL .json files** - Configuration and component definitions  
- ✅ **ALL .py/.ps1 scripts** - Build and utility tools
- ✅ **ALL .md documentation** - Architecture specifications
- ✅ **ALL .scad/.stl files** - 3D models and manufacturing
- ✅ **EVERYTHING** - Assume it's tested and working

### **CHECKOUT WORKFLOW (ONLY Way to Modify Files):**

#### **Step 1: Create Working File List**
```powershell
# Create checkout list with files you plan to modify
echo "src/components/ComponentName.cpp" >> .working-files.txt
echo "config/components/positioned/component_name.json" >> .working-files.txt
echo "docs/SOME-SPEC.md" >> .working-files.txt
```

#### **Step 2: Backup to Temp Directory**  
```powershell
# MANDATORY: Backup ALL files before ANY modification
mkdir temp-backup-$(Get-Date -Format "yyyy-MM-dd-HHmm")
foreach($file in Get-Content .working-files.txt) {
    $dest = "temp-backup-$(Get-Date -Format "yyyy-MM-dd-HHmm")/$file"
    New-Item -ItemType Directory -Path (Split-Path $dest) -Force
    Copy-Item $file $dest
}
```

#### **Step 3: NOW Files Are Fair Game**
```markdown
✅ Files in .working-files.txt can now be modified
✅ Files are backed up in temp directory  
✅ Can add/remove/edit content in checked-out files
✅ Can test and verify changes
```

#### **Step 4: Verification & Commit**
```powershell
# Test the changes thoroughly
python tools/check_json_naming.py
python tools/check_cpp_naming.py  
pio run -t upload -t monitor

# If everything works:
git add (Get-Content .working-files.txt)
git commit -m "Verified changes to checked-out files"
Remove-Item .working-files.txt
Remove-Item temp-backup-* -Recurse
```

### **EMERGENCY RESTORE:**
```powershell
# If changes break something:
foreach($file in Get-Content .working-files.txt) {
    Copy-Item "temp-backup-*/$file" $file -Force
}
```

### **CHECKOUT ENTIRE PROJECT (Allowed):**
```powershell
# Get ALL files in project
Get-ChildItem -Recurse -File | Where-Object { 
    $_.Extension -match "\.(cpp|hpp|json|py|ps1|md|scad|stl)$" 
} | ForEach-Object { $_.FullName.Replace((Get-Location).Path + "\", "") } >> .working-files.txt

# Backup everything (will be large but safe)
# Then proceed with modifications
```

### **VALIDATION TOOLS:**

```powershell
# Check if protection system is working
python tools/validate-protection.py

# Expected output when violations detected:
# ❌ Files modified without any checkout: ['src/main.cpp', 'docs/SOME-SPEC.md']
```

### **EXAMPLE: PROPER CHECKOUT WORKFLOW**

```powershell
# Step 1: Check current protection status
python tools/validate-protection.py

# Step 2: Check out files you want to modify
.\tools\checkout-files.ps1 -Files "src/main.cpp","docs/COMPONENT-FUNCTION-SIGNATURES.md"

# Step 3: Now files are safely backed up and can be modified
# (Make your changes here)

# Step 4: Validate and commit
.\tools\checkout-files.ps1 -Commit
```

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

**CRITICAL ADDITION: NEVER CREATE WHAT ALREADY EXISTS**

**Before creating ANY new file, script, or functionality:**
1. **SEARCH** the entire codebase for existing implementations
2. **CHECK** tools/, scripts/, utilities directories first
3. **EXAMINE** similar functionality that might already exist
4. **UNDERSTAND** why existing solutions were created that way
5. **ASK** before duplicating effort

**Example violation:**
- Created `validate_naming_conventions.py` without checking tools/ directory
- Found existing scripts: `check_json_naming.py`, `fix_json_naming.py`, `check_cpp_naming.py`, `fix_cpp_naming.py`
- These scripts were already working and comprehensive
- Wasted effort creating duplicate functionality

**NO ASSUMPTIONS. NO GUESSING. CHECK THE CODE FIRST. ALWAYS.**

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

---

## RULE 13: USE GITHUB COPILOT PRO+ PAID MODEL
**Use the full GitHub Copilot Pro+ paid tier capabilities, NOT free model limits.**
- User pays $40/month for premium service
- Use full paid tier performance and capabilities
- Do NOT default to free tier limitations
- Get full value from the premium subscription

## RULE 14: FILENAME PARSING - IGNORE TRAILING PUNCTUATION
**When parsing filenames from user text, extract only the actual filename.**
- Ignore trailing periods, commas, or other sentence punctuation
- Ignore typos or extra characters after the filename
- Use common sense to identify the actual filename
- User may use backticks around ambiguous filenames: `config.json`
- If backticks are used, that's the exact filename
- If no backticks, still parse intelligently ignoring sentence delimiters
