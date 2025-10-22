
# At the end of any rule evaluation or task, agents must read and apply the rules in .github/consistant_project_rules.md. However, these rules do not supersede the AI-AGENT-RULES.md; the ironclad agent rules in this file always take precedence.
# BASE LEVEL RULE: When the user asks why you are not doing something they requested, you must explicitly explain any system-level constraints, memory or message limits, or default design rules that are causing the issue. Do not give generic or evasive answers—be specific about the technical or policy reason for the behavior.

# AI AGENT IRONCLAD RULES - NEVER BREAK THESE


## RULE: IMMEDIATE REPORTING OF BLOCKERS

If any project rule, technical limitation, or system constraint prevents the agent from executing a user instruction, the agent must immediately report the exact reason to the user. The agent must not pause, stop, or silently fail without providing a clear explanation of the blocker, so the user can address or override it.



<!-- ## RULE 1 - these rules are consistant

- If a contradiction between one of these rules and another one of the rules in this list is detected, these rules must be modified using feedback from the user to make them so before anything else is allowed to proceed.
- These rules are not complete, in the sense additional consistant rules will be required as development proceeds.
- Anytime the user requests an action or task not allowed by one or more of these rules, he must be warned exactly what rule conflicts with his directions. At that point the human will be allowed to do 1 of these options:
	- a. Change the agent-ai rule (this ruleset) either temporarily or permanently to allow the task to proceed
	- b. Change the requested task
	- c. Abort the task completely and do something else
		

## RULE 2 TRANSPARENT DEVELOPMENT

- Agent will clearly communicate what files will be modified before making changes
- Agent has permission to modify any files necessary to complete the requested task
- Agent will report all changes made at the end of each task
- Human can stop or redirect at any time during development
- All changes will be committed to git when the task is completed -->
..
## RULE: FRAMEWORK IS ESP-IDF, NOT ARDUINO

**NEVER reference:**
- Arduino.h
- Adafruit libraries  
- Arduino IDE
- Arduino functions (delay(), pinMode(), etc.)

**ALWAYS use:**
- ESP-IDF native APIs
- FreeRTOS

## RULE 5 System uses components for all functionality.

- the ComponentName is called name here to simplify sermantics.
- all component names are unique
- each component has 2 functions, an init function named {name}_init( void ), and {name}_act(void ) 
- the files these two functions are defined in are named like name.hpp and name.cpp.  they are located in the /src/components and /include/components folders.
- for example, if the component name is GC9A01, the functions are gc9a01_init()  and gc9a01_act() located in src/components/gc9a01.cpp and /include/components/gc9a01.hpp



	
## RULE 6: ALWAYS USE BRACES (ALLMAN STYLE)

**ALL `for`, `while`, `if`, `else` statements MUST use braces, even for single-line bodies.**

**Braces MUST be on their own line at the same indentation level (Allman style).**

**CORRECT (Allman Style):**

```cpp
for (int i = 0; i < SIZE; i++)
{
    doSomething;
}

if (condition)
{
    doAction;
}

while (running)
{
    update;
}

if (errorCode != ESP_OK )
{
    handleError( errorCode );
}
else
{
    continueWork();
}

void myFunction(int bepoop)
{
    // Function body
}

class twindles
{
    // class definition
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
class boogywoogy {                                  // ❌ NO - K&R style (brace on same line)
}

void* function babydoll( int arg1, float arg2) {    // ❌ NO - K&R style (brace on same line)

}


```

**Rationale:**

- Prevents bugs when adding debug statements
- Makes code structure visually clear
- Braces at same indent level show scope hierarchy
- Enforces consistent style
- Required for safety-critical embedded code

## RULE 7: ASK BEFORE ASSUMING

**If unclear:**

- ASK the human
- Don't guess
- Don't assume
- Don't improvise

**Better to ask than mess up code.**

## RULE 8: WHEN HUMAN SAYS "DON'T, make sure this doesn't contridict any known rules.  If so, report the inconsistancy immedietly.  once the human and ai have discussed what the final rule should be, encode it  and ap[end it to the end of this set of rules, ensuring rules are consistant.

## RULE 9: all FILES MUST BE ASCII ENCODED (NO BOM)


**Reference:** See `docs/component-json-requirements.md` for complete JSON parsing rules and validation requirements.

## RULE 10: DEVELOPMENT ENVIRONMENT REQUIREMENTS

**Operating System:** Windows  
**Default Shell:** PowerShell (Windows PowerShell v5.1)

**CRITICAL PowerShell Syntax Rules:**

- Use semicolon (`;`) to join commands on single line - **NOT** double ampersand (`&&`)
- Example: `command1; command2` ✅ **CORRECT**
- Example: `command1 && command2` ❌ **WRONG** (this is bash syntax)

**When generating terminal commands:**

- Generate commands correctly for PowerShell syntax
- Never use bash operators (`&&`, `||`) in PowerShell commands
- Agent must learn from syntax mistakes and not repeat them

**Build System:**

- Primary: PlatformIO with ESP-IDF framework
- Target: ESP32-S3-DevKitC-1
- Commands: `pio run` for builds, `pio run -t upload` for flashing

## RULE 11: COMPONENT GENERATION IS MANDATORY

**ALL component code MUST be generated via scripts - NEVER manually created**

**CRITICAL ARCHITECTURE REQUIREMENT:**

```
IF creating_component_code THEN
    → Use generate_tables.py or generate_individual_components.py
    → NEVER manually create .cpp/.hpp files in src/components/
    → NEVER manually edit generated dispatch tables
ELSE
    → Build will fail with missing headers
    → Inconsistent naming conventions  
    → Architecture violations
```

**Component Generation Commands:**

- Generate for specific bot: `python tools\generate_tables.py config\bots\bot_families\goblins\goblin_full.json src`
- Individual components: `python tools\generate_individual_components.py`

**Manual component creation ALWAYS fails** - the generation system is required for:

- Consistent naming conventions
- Proper header file generation
- Dispatch table integration
- Multi-ESP32 variant support

## RULE 12: MULTI-ESP32 ENVIRONMENT SELECTION  

**Each subsystem targets optimal ESP32 variant for cost/performance**

```
IF gpio_needed <= 22 AND memory_needed <= 350KB THEN
    → Use ESP32-C3 environment (cost-optimized $2-3)
ELSIF gpio_needed <= 34 AND memory_needed <= 500KB THEN  
    → Use ESP32 environment (balanced $4-5)
ELSE
    → Use ESP32-S3 environment (high-performance $6-8)
```

**Build Commands:**

- Specific environment: `pio run -e goblin_head`
- Chip type group: `pio run -e left_arm -e right_arm` (all ESP32-C3)
- All environments: `pio run`

read all of the rules files in docs/rules. make sure none of the rules contridict any othe rules.

