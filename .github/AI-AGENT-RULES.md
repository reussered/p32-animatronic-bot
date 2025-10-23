
# At the end of any rule evaluation or task, agents must read and apply the rules in .github/consistant_project_rules.md. However, these rules do not supersede the AI-AGENT-RULES.md; the ironclad agent rules in this file always take precedence.
# BASE LEVEL RULE: When the user asks why you are not doing something they requested, you must explicitly explain any system-level constraints, memory or message limits, or default design rules that are causing the issue. Do not give generic or evasive answers—be specific about the technical or policy reason for the behavior.

# AI AGENT IRONCLAD RULES - NEVER BREAK THESE
- always read and make sure you understand any file before changing it.
- if the filename or contents of a file are not consistant with what is expected because of appying any rule, either one from the system or the user, never change the ciontents of a 
	file or its filename unless that is within the exoplicit purpose of the current task.  Unless its obvious the file contents or filename should be changed as part of the current tyask,
	ask for permission from the human before changing or deleting the file.



## RULE: IMMEDIATE REPORTING OF BLOCKERS

If any project rule, technical limitation, or system constraint prevents the agent from proceeding with the current task, the agent must immediately report the exact reason to the user. The agent must not pause, stop, or silently fail without providing a clear explanation of the blocker, so the user can address or override it.
	the current task being worked on.


<!-- ## RULE 1 - these rules are consistant

- If a contradiction between one of these rules and another one of the rules in this list is detected, these rules must be modified using feedback from the user to make them so before anything else is allowed to proceed.
- These rules are not complete, in the sense additional consistant rules will be required as development proceeds.
- Anytime the user requests an action or task not allowed by one or more of these rules, he must be warned exactly what rule conflicts with his directions. At that point the human will be allowed to do 1 of these options:
	- a. Change the agent-ai rule (this ruleset) either temporarily or permanently to allow the task to proceed
	- b. Change the requested task
	- c. Abort the task completely and do something else
		

## RULE 2 TRANSPARENT DEVELOPMENT

- Agent will clearly communicate what files will be modified before making changes
- Agent has permission to modify any files necessary to complete the task specified by the human
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

## RULE 5 System uses components defined in json files for all functionality.

- all component names are unique.
- every component has the following files.  if any component is worked on that is missing one of these files, the user is immiedietly notified and all processing must stop.  
	these files are {ComponentName}.json, {ComponentName}.cpp, and {ComponentName}.hpp
- each component has 2 functions, an init function named {name}_init( void ), and {name}_act(void ) that are defined in {ComponentName}.hpp and implemented in {ComponentName}.cpp
- {ComponentName}.hpp is located in and {ComponentName}.cpp is located in the root/src/components and root/include/components folders.
- for example, if the component name is GC9A01, the functions are gc9a01_init()  and gc9a01_act() located in root/src/components/gc9a01.cpp and root/include/components/gc9a01.hpp
- all components that have a shape defined have a file called {ComponentName}.scad and {ComponentName}.stl.  these files are locatedn in the root/assets/shapes/scad and root//assets/shapes/stl directories.
- every creature is defined by a single json file and all of the json files defined by recursive composition using the components contained within that file.  the rules used to compose the creature are defined inside markup files in the root/docs folder.  
	never use the contents of the in the /docs/obsolete folder



	
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

## RULE: WINDOWS DEVELOPMENT ENVIRONMENT REQUIREMENTS

**Environment Specifications:**
- Windows PowerShell environment (powershell.exe v5.1)
- ASCII encoding ONLY - NO UTF-8 ENCODING - NO UNICODE ANYWHERE
- Windows NTFS file system with backslash paths
- CRLF line endings (not LF)
- Current directory: F:\GitHub\p32-animatronic-bot

**Mandatory Development Practices:**

1. Use Windows PowerShell syntax only
2. Use backslash paths: F:\GitHub\p32-animatronic-bot\config\file.json
3. NO Linux/Unix commands ever
4. Create files that work natively on Windows
5. Use simple PowerShell commands, not complex scripts
6. ASCII encoding only - NO UTF-8 - never Unicode
7. Test before scaling up
8. One file at a time if complex operations fail
9. Never validate VS Code config files (.vscode/*) as project JSON - exclude from consistency checks
10. Respect .gitignore files - files and folders listed in .gitignore are NOT part of the project and must be excluded from all validation and processing

**Token Cost Awareness:**
- Failed commands cost tokens from user budget
- Repeated mistakes are expensive
- Test approach before implementing at scale
- Use proven patterns that work except when they contradict one of the rules established for this project

## RULE: PROJECT NAVIGATION AND KEY LOCATIONS

**Script Locations:**
- Component table generation: `tools\generate_tables.py`
- Individual component creation: `tools\generate_individual_components.py`
- File structure analysis: `generate_file_structure.ps1` (root directory)
- STL generation: `tools\generate-stl-files.ps1`
- System verification: `tools\verify_full_system.ps1`

**Configuration Structure:**
- **Creatures/Bots**: `config\bots\bot_families\{family}\{bot_name}.json` (goblins, cats, bears, robots, etc.)
- **Hardware specs**: `config\hardware\{component_type}.json` (servo_9g_micro.json, gc9a01_display.json, etc.)
- **Interface definitions**: `config\interfaces\{interface_type}.json` (spi_device_1.json, pwm_channel_3.json, etc.)
- **Positioned components**: `config\components\positioned\{subsystem}\{component}.json`
- **Subsystem assemblies**: `config\subsystems\{subsystem_name}.json` (goblin_head.json, goblin_torso.json)
- **Component templates**: `config\components\templates\{type}\{template}.json`
- **Behaviors**: `config\behaviors\{behavior_name}.json`
- **Moods**: `config\moods\{mood_config}.json`

**Generated Code Structure:**
- **Component implementations**: `src\components\{component_name}.cpp`
- **Component headers**: `include\components\{component_name}.hpp`
- **Dispatch tables**: `src\subsystems\{subsystem}\{subsystem}_dispatch_tables.hpp/.cpp`
- **Shared classes**: `shared\{ClassName}.hpp` (Mood, Environment, SysTest, etc.)
- **Core system**: `src\core\memory\SharedMemory.hpp/.cpp`
- **Main application**: `src\main.cpp`

**Asset Structure:**
- **3D Models**: `assets\shapes\scad\{category}\{model}.scad` and `assets\shapes\stl\{category}\{model}.stl`
- **Animations**: `assets\animations\{creature_family}\{animation_name}.json`
- **Sounds**: `assets\sounds\{creature_family}\{sound_file}.wav`
- **Manufacturing**: `config\manufacturing\{process}\{specification}.json`

**Documentation Structure:**
- **Build guides**: `docs\build_guides\{guide_name}.md`
- **Architecture docs**: `docs\{system}_ARCHITECTURE.md`
- **Specifications**: `docs\{component}-spec.md`
- **Wiring diagrams**: `docs\wiring\{system}_wiring.md`

**Family Categories Available:**
- `config\bots\bot_families\goblins\` - Goblin creatures
- `config\bots\bot_families\cats\` - Cat creatures  
- `config\bots\bot_families\bears\` - Bear creatures
- `config\bots\bot_families\robots\` - Test/mechanical bots
- `config\bots\bot_families\tests\` - Hardware validation and system test bots
- `config\bots\bot_families\dragons\` - Dragon creatures
- `config\bots\bot_families\horror\` - Horror creatures
- `config\bots\bot_families\fantasy\` - Fantasy creatures

**Key Generation Commands:**
- Generate dispatch tables: `python tools\generate_tables.py config\bots\bot_families\{family}\{bot}.json src`
- Create missing components: `python tools\generate_individual_components.py`
- Validate JSON consistency: `.\generate_file_structure.ps1`
- Example: `python tools\generate_tables.py config\bots\bot_families\tests\test_bot.json src`

**File Organization Rules:**
- Hardware specifications → `config\hardware\`
- Interface definitions → `config\interfaces\`  
- Positioned components reference hardware via `hardware_reference` field
- Interface assignments via `interface_assignment` field
- All paths use Windows backslash format
- **UPDATE RULE**: When creating new directories, families, or major structural changes, update this project navigation section immediately to maintain accurate documentation
- **COMMIT RULE**: After updating AI-AGENT-RULES.md, always commit that file alone to git repository with descriptive message

**Current Project Status (Updated: 2025-10-23):**
- **Active Development**: Test bot with dual GC9A01 displays for hardware validation
- **Test Components Created**: test_left_eye, test_right_eye in `src\components\test\`
- **SysTest Class**: New shared class for system testing in `shared\SysTest.hpp`
- **Test Bot Family**: `config\bots\bot_families\tests\test_bot.json` for validation testing
- **Power System**: Stable 3.16V delivery via ATX PSU + breakout board
- **Hardware Status**: 2x GC9A01 displays connected and cool-running

