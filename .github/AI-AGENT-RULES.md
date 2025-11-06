# 🚨 CRITICAL: EMBEDDED SYSTEMS DEVELOPMENT CONTEXT 🚨

**THIS IS NOT WEB DEVELOPMENT - THIS IS EMBEDDED MICROCONTROLLER PROGRAMMING**

**Target Hardware**: ESP32-S3 microcontroller
**Framework**: ESP-IDF native APIs (NOT Arduino, NOT web frameworks)
**Resource Constraints**: Memory and performance constrained embedded environment
**Encoding**: ASCII-only for toolchain compatibility (NO UTF-8, NO Unicode)
**Dependencies**: Hardware drivers, FreeRTOS, SPI/I2S protocols
**Toolchain**: PlatformIO + ESP-IDF, GCC cross-compiler
**Debugging**: Serial console, hardware debugger (NOT browser dev tools)
**Performance**: Real-time constraints, interrupt-driven (NOT async/await patterns)

**NEVER THINK LIKE A WEB DEVELOPER:**
- NO npm packages, node_modules, or JavaScript patterns
- NO web servers, REST APIs, or HTTP frameworks
- NO browser compatibility concerns or DOM manipulation
- NO database ORMs or cloud service integrations
- NO UTF-8 encoding assumptions or web character sets

**ALWAYS THINK LIKE AN EMBEDDED DEVELOPER:**
- Memory usage in bytes, not megabytes
- CPU cycles matter, power consumption matters
- Hardware registers, GPIO pins, SPI buses
- Interrupt handlers, task priorities, timing constraints
- Component datasheets, electrical specifications
- ASCII encoding, fixed-point math, lookup tables

# AI AGENT IRONCLAD RULES - NEVER BREAK THESE

## 📋 MANDATORY PREREQUISITE: READ ALL RULE DOCUMENTS

**BEFORE implementing ANY changes, the AI agent MUST read and internalize:**

1. **`.github/AI-AGENT-RULES.md`** - This file (comprehensive architecture rules - SINGLE SOURCE OF TRUTH)
2. **`ARCHITECTURE_DECISION_LOG.md`** - Historical decisions, problem-solving approaches, and implementation rationale
3. **`.github/copilot-instructions.md`** - Copilot-specific implementation guidance and best practices

**FAILURE TO READ THESE DOCUMENTS WILL RESULT IN ARCHITECTURE VIOLATIONS**

**Note**: All naming rules, component JSON requirements, encoding rules, generation patterns, and pin assignment rules are consolidated in this file (AI-AGENT-RULES.md). This is now the single source of truth.

## 📋 BOUNDED AUTHORITY FRAMEWORK

**The agent has clear decision authority within boundaries. DO NOT ask permission for every decision - act autonomously when these criteria are met. ONLY escalate genuinely ambiguous situations.**

### When Agent Can Decide Autonomously (NO HUMAN APPROVAL NEEDED)

1. **Code matches existing working pattern**: IF the code you're working with follows an existing established pattern in the codebase AND your changes maintain that same pattern → **ACT** (no approval needed)

2. **Fix is directly specified in rules**: IF RULE X explicitly states "do Y in situation Z" AND that situation clearly applies → **ACT** (no approval needed)

3. **Violation is clear-cut**: IF code violates a specific rule (e.g., UTF-8 in ASCII-only system, missing braces, wrong file location) AND the fix is unambiguous → **FIX IT** (no approval needed)

4. **Completing defined work**: IF you're filling empty directories with standard components following RULE 6 structure AND the component pattern matches existing implementations → **GENERATE IT** (no approval needed)

### When Agent MUST Escalate to Human

1. **Contradictory rules**: IF RULE A and RULE B give conflicting guidance for the same situation → **STOP and report which rules conflict and ask which to apply**

2. **Genuinely novel situation**: IF the situation is not covered by any existing rule AND not matching any established pattern in codebase → **STOP and describe the situation with specific details**

3. **Ambiguous requirement**: IF you have multiple valid interpretations of a requirement that would result in different code → **STOP and list the interpretations with pros/cons of each**

4. **Cross-cutting decision**: IF your change affects multiple subsystems or could break existing functionality → **STOP and report potential impacts before proceeding**

5. **Unknown reference**: IF a rule references something you cannot locate (e.g., "apply pattern from component X" but component X doesn't exist) → **STOP and ask where to find the reference**

### Escalation Format (When Required)

When escalating, provide:
- **Specific rule or pattern** that applies (with line number if possible)
- **Situation details** - exact problem and context
- **Decision options** - what choices could be made
- **Recommendation** - which option you think is correct and why
- **Blocker** - what you're blocked on specifically 

## RULE 0: INVESTIGATION BEFORE ACTION - BOUNDED AUTHORITY PROTOCOL

### ⚠️ MANDATORY VERIFICATION GATE - READ BOTH RULE FILES BEFORE PROCEEDING

**This section MUST be completed BEFORE any work begins on this project.**

**Verification Checklist:**
1. ✅ Have you read **`.github/AI-AGENT-RULES.md`** COMPLETELY? (937 lines, ~15 min)
   - If NO: Stop now. Read it completely before proceeding.
   - If YES: What are the main categories of rules? (Name at least 5 RULEs)

2. ✅ Have you read **`.github/copilot-instructions.md`** COMPLETELY? (58 lines, ~2 min)
   - If NO: Stop now. Read it after reading AI-AGENT-RULES.
   - If YES: What is the "Single Compilation Unit" and why does it matter?

3. ✅ Can you articulate the SPECIFIC RULE that applies to your current task?
   - If NO: Escalate to human - ask "Which rule governs this task?"
   - If YES: Quote the rule and explain how your implementation aligns with it

**Consequence of Skipping Verification:**
- Architectural violations guaranteed
- Silent failures likely
- Wasted time debugging incorrect implementations
- Project coherence destroyed

**You have no autonomous authority to proceed without completing this verification.**

---

**BEFORE making ANY code changes, modifications, or "fixes", you MUST:**

1. **READ the existing implementation** - Use `read_file`, `grep_search`, or `semantic_search` to understand what's already there
2. **UNDERSTAND the current pattern** - See how the code actually works, don't assume you know
3. **IDENTIFY the actual problem** - Compile errors? Runtime errors? What's the SPECIFIC failure?
4. **VERIFY the fix is needed** - Is the code actually broken, or just different from your assumptions?
5. **CHECK for existing solutions** - Has this been solved before? Are there working examples?
6. **APPLY BOUNDED AUTHORITY** - Check the framework above. Do you have clear authority to act? If yes, proceed. If no, escalate with specific details.
7. **CITE THE APPLICABLE RULE** - State which RULE from AI-AGENT-RULES.md governs this decision

**DECISION AUTHORITY APPLIED HERE:**
- **Clear pattern match** (code matches working example in codebase) → **ACT** (cite RULE 0)
- **Direct rule violation** (UTF-8 when ASCII required, missing braces, wrong location) → **FIX IT** (cite RULE 25, 23, 6)
- **Contradictory rules or genuinely novel** → **ESCALATE with details** (cite conflicting RULEs)
- **Multiple valid interpretations** → **ESCALATE with options** (cite applicable RULEs)

**NEVER:**
- Rewrite working code because you "think" it should be different
- Move files without understanding their purpose and dependencies
- Create duplicate files in different locations
- "Improve" code that already works
- Change patterns without reading the architecture docs first
- Assume you know better than existing implementations
- Ask permission for decisions that fall under your autonomous authority
- Work on this project without reading BOTH rule files completely

## RULE 1: IMMEDIATE REPORTING OF BLOCKERS
If any project rule, technical limitation, or system constraint prevents the agent from proceeding with the current task, the agent must immediately report the exact reason to the user. The agent must not pause, stop, or silently fail without providing a clear explanation of the blocker.

## RULE 2: NAMING Rules ARE FOUND IN NAMING_RULES.MD
**ALL functionality is implemented as components defined in JSON files.**

- Component names are globally unique
- Each component has: `{ComponentName}.json`, `{ComponentName}.src`, `{ComponentName}.hdr`
- Each component has two functions: `{name}_init(void)` and `{name}_act(void)`
- Components with shapes have: `{ComponentName}.scad` and `{ComponentName}.stl`

## RULE 3: COMPONENT ISOLATION - NO DIRECT FUNCTION CALLS
**Components NEVER call each other's functions directly.**

- The "contains" relationship in JSON is for build inclusion, NOT function calls
- Component dispatch system calls `init()` and `act()` functions independently
- Example: `goblin_left_eye_act()` NEVER calls `goblin_eye_act()` or `gc9a01_act()`

## RULE 4: COMPONENT PIPELINE EXECUTION MODEL

**Components form execution pipelines defined by JSON `components: []` arrays. They execute sequentially in dispatch tables, NOT as function calls.**

### Pipeline Architecture

Each JSON component defines `components: []` array that specifies child components for execution. The `components: []` array defines the execution sequence:

```
goblin_left_eye.json:
  components: [goblin_eye.json]           // Single child in this case

goblin_eye.json:
  components: [gc9a01.json]               // Single child in this case

gc9a01.json:
  components: [spi_display_bus.json]      // Single child in this case

spi_display_bus.json:
  components: [generic_spi_display.json]  // Single child in this case

generic_spi_display.json:
  components: []  (leaf - no further dependencies)
```

**Note:** Components CAN have multiple children in `components: []` array, but in this pipeline each has only one.

### Component Responsibility Chain

Each component in the pipeline has a specific job and knows who to delegate to:

```
goblin_left_eye.json:
  Job: Read mood, generate eye frame data
  Delegates to: goblin_eye.json

goblin_eye.json:
  Job: Apply animation/expression logic to frame
  Delegates to: gc9a01.json

gc9a01.json:
  Job: Format frame data as GC9A01 display commands
  Delegates to: spi_display_bus.json (knows how to send commands to hardware)

spi_display_bus.json:
  Job: Manage SPI protocol, timing, buffer dump
  Delegates to: generic_spi_display.json (hardware-level I/O)

generic_spi_display.json:
  Job: Perform actual bit-banging or hardware SPI to device
  Delegates to: (hardware registers - end of chain)
```

**Key Point**: `gc9a01` COULD contain `spi_display_bus` and `generic_spi_display` logic internally, but the separation exists because:
- `spi_display_bus` is a **generic hardware communication layer** reusable by ANY component needing SPI
- Different display chips (gc9a01, st9943, ili9341) all delegate to the same `spi_display_bus` component
- Other hardware (sensors, SD cards, etc.) can also use `spi_display_bus` for their SPI communication
- This avoids duplicating bus management code across every SPI-based component

### Critical Rules

1. **NO direct function calls between components** - They do not know each other exists
2. **Execution order determined by JSON array order** - The `components: []` array defines pipeline sequence
3. **Data flows through static file-scoped variables** - Each component reads from buffer, processes, writes to buffer
4. **Component names are globally unique (fermionic)** - Each component executes once per cycle (or multiple times if listed multiple times)
5. **Same component can appear in multiple pipelines** - If `gc9a01` appears in both goblin_left_eye and bear_left_eye chains, it's added to dispatch table twice

### Parser Job

When the generator processes a component JSON:

```
1. Traverse components[] array IN ORDER (depth-first)
2. For each component:
   a. Load component.json file
   b. Is this first encounter of this component name?
      - YES: Aggregate .src and .hdr files
      - NO: Skip aggregation (already have it)
   c. Add to dispatch table (always, even if aggregated before)
3. Generate dispatch tables preserving array order
```

Result: Dispatch tables execute in pipeline order, creating data flow.

### INTRA-SUBSYSTEM vs INTER-SUBSYSTEM COMMUNICATION

**There are two distinct communication patterns.**

#### 1. INTRA-Subsystem (Within Same Pipeline/ESP32)

- **Mechanism**: File-scoped `static` variables and pointers
- **How it Works**: All `.src` files for a pipeline are concatenated into one `.cpp` file. Shared file scope allows components to communicate through static globals.
- **Data flow**: 
  - goblin_left_eye writes to `static uint16_t* display_buffer`
  - goblin_eye reads from same buffer, processes, writes back
  - gc9a01 reads from buffer, processes, writes to command buffer
- **Speed**: Fast (zero-copy, direct memory access)
- **Rule**: ONLY for components in same pipeline/subsystem

#### 2. INTER-Subsystem (Between Different ESP32s)

- **Mechanism**: `SharedMemory` class (`GSM` global instance)
- **How it Works**: `GSM.write<T>()` broadcasts data to other ESP32s over network. `GSM.read<T>()` accesses local copy.
- **When to use**: Components on different controllers need to coordinate (e.g., head movement affects torso balance)
- **Rule**: This is the ONLY permitted mechanism for inter-subsystem communication

**Key Takeaway**: 
- Components in same pipeline use file-scoped globals (fast intra-subsystem communication) for transient data like display buffers
- Components across pipelines/controllers ONLY use SharedMemory for shared state classes (Mood, Environment, Personality, SysTest) defined in `/shared`
- **File-scoped globals**: Temporary buffers, intermediate processing data (NEVER persisted via SharedMemory)
- **SharedMemory classes** (`/shared` directory): Persistent state that crosses pipeline/controller boundaries
- **Both exist simultaneously** - different data types use different mechanisms based on scope and persistence needs
- Example: Display frame buffer stays in file-scoped globals (fast), Mood state moves via `GSM.write<Mood>()` to other ESP32s

---

### ANIMATION KEYFRAME PATTERN (Special Case - NOT Component Pipeline)

**Animations are DATA, not executable components. They use a different pattern: `component_id` with `keyframes`.**

Animation files (`assets/animations/{creature}/*.json`) define keyframe sequences with `component_id` references:

```json
{
  "animation_name": "goblin_blink",
  "components": [
    {
      "component_id": "goblin_eye_left",
      "keyframes": [
        {
          "time_ms": 0,
          "display_content": { "eye_openness": 0.8 }
        },
        {
          "time_ms": 100,
          "display_content": { "eye_openness": 0.0 }
        }
      ]
    }
  ]
}
```

**Key Differences from Component Pipeline:**

- Uses `component_id` (not file paths) to reference components by name
- Contains `keyframes` array (animation data, not executables)
- These are **metadata/data files**, not executable components
- Animation system reads these and applies keyframe data to named components at runtime
- **NOT subject to RULE 4 component pipeline requirements** - separate execution model

**CRITICAL**: Inline components in `config/` ARE violations of RULE 4/6. Only animations use `component_id` + `keyframes` pattern.

---

## RULE 5: PROVEN IMPLEMENTATION PATTERNS
**USE THESE WORKING PATTERNS - DO NOT REDESIGN THEM**

### PATTERN 1: MOOD-BASED DISPLAY ANIMATION (GOBLIN EYE SYSTEM)
**Location**: `goblin_left_eye` -> `goblin_eye` -> `gc9a01` ->`spi_display_bus` -> `generic_spi_display`

### PATTERN 2: SHARED STATE COMMUNICATION
**Location**: `SharedMemory` class with `GSM` global instance  IS DECLARED IN THE SUBSYTEMS MAIN.CPP FILE LIKE:
```cpp
// this goes above the invocation to app_main
SharedMemory GSM();

// this is first line of app_main
GSM.init();
```


**Usage Pattern**:
```cpp
// Reading shared state
Mood *mood = GSM.read<Mood>();
Environment *env = GSM.read<Environment>();

// Modifying shared state in place
mood->anger = 75;
env->temperature = 25.5;

// Writing shared state (triggers inter-subsystem sync)
GSM.write<Mood>();      // Broadcasts current mood state
GSM.write<Environment>(); // Broadcasts current environment state
```

**Implementation Rules**:
- `read<T>()` is local memory access (fast, no network)
- `write<T>()` triggers inter-subsystem synchronization
- All shared classes in `/shared` directory
- Defined classes: Mood, Environment, Personality, SysTest


## RULE 6: FILE ORGANIZATION STANDARDS (UPDATED NOVEMBER 2025)

**Components are now organized by type and family, not centrally:**

**Generic/Reusable Components:**
- Hardware: `config/components/hardware/{name}.json|src|hdr`
- Drivers: `config/components/drivers/{name}.json|src|hdr`
- Interfaces: `config/components/interfaces/{name}.json|src|hdr`
- Behaviors: `config/components/behaviors/{name}.json|src|hdr`
- Moods: `config/components/behaviors/moods/{name}.json`

**Shared Multi-Family Components:**
- Location: `config/bots/multi_family/{subsystem_type}/{name}.json|src|hdr`
- Types: `humanoid/`, `quadruped/`, `tentacles/`, `wings/`

**Creature-Specific Components:**
- Location: `config/bots/bot_families/{family}/`
  - Family template: `{family}_family.json`
  - Creature variants: `{family}_{variant}.json`
  - Subsystems: `head/`, `torso/`, `arms/`, `legs/`, `hands/`, `feet/`
  - Components in subsystems: `{subsystem}/{component_name}.json|src|hdr`
- Families: `goblins/`, `bears/`, `cats/`, `dragons/`, etc.

**OBSOLETE (No longer used):**
- `config/components/creature_specific/` - DELETED
- `config/hardware/`, `config/driver/`, `config/interfaces/` - MOVED to `config/components/`
- `config/behaviors/`, `config/moods/` - MOVED to `config/components/behaviors/`

The validation script `tools/validate_file_structure.py` can be used to check for compliance.

## RULE 6B: COMPONENT NAMING - FERMIONIC UNIQUENESS WITH TEMPLATES

**Component names must be globally unique (fermionic - no duplicates).**

However, components can be **template instantiations** with different configurations:

### Template Composition Pattern

```
Generic (reusable) component:
  config/components/hardware/gc9a01.json|src|hdr
  
### Single Creature Using Multiple Displays (Template Pattern)

A creature's eye component references a templatable display component:

```
bear_left_eye.json (concrete component - fermionic unique name):
  components: ["bear_eye.json"]

bear_eye.json (templatable - accepts different displays):
  components: ["gc9a01.json"]            // Configured for gc9a01 display
  
bear_eye_alt.json (same creature logic, different display):
  components: ["st9943.json"]            // Configured for st9943 display

Complete execution chain for bear_left_eye with gc9a01:
  bear_left_eye -> bear_eye -> gc9a01 -> spi_display_bus -> generic_spi_display

Complete execution chain for bear_left_eye with st9943:
  bear_left_eye -> bear_eye_alt -> st9943 -> spi_display_bus -> generic_spi_display
```

### Multiple Creatures Using Same Display Driver

Different creatures can reference the same generic display driver:

```
goblin_left_eye.json:
  components: ["goblin_eye.json"]

goblin_eye.json:
  components: ["gc9a01.json"]

bear_left_eye.json:
  components: ["bear_eye.json"]

bear_eye.json:
  components: ["gc9a01.json"]

Both creatures' pipelines eventually reference the same gc9a01.json file:
  goblin_left_eye -> goblin_eye -> gc9a01 -> spi_display_bus -> generic_spi_display
  bear_left_eye -> bear_eye -> gc9a01 -> spi_display_bus -> generic_spi_display

- gc9a01.src is aggregated once into build
- Appears in dispatch table twice (once per creature's pipeline)
- Same hardware driver code, different data contexts
```

### Fermionic Uniqueness vs. Code Reuse

- **Fermionic Uniqueness**: `goblin_left_eye`, `goblin_eye`, `bear_left_eye`, `bear_eye` are all unique names
- **Code Reuse**: Same `gc9a01.json` can be used by both goblins and bears
- **Variants**: `bear_eye_alt.json` is an alternate version of `bear_eye` using different display, avoids combinatorial explosion

### Path Resolution for Parser

JSON `components: []` arrays specify the next component in the chain:
```json
{
  "name": "bear_left_eye",
  "components": ["bear_eye.json"]
}

{
  "name": "bear_eye",
  "components": ["gc9a01.json"]
}

{
  "name": "bear_eye_alt",
  "components": ["st9943.json"]
}
```

**Parser requires**: Every entry in `components: []` must point to an actual component file that can be found and read.

## RULE 7: BUILD AND DEPLOYMENT PROCESS

**Development Workflow**:
1. **Modify JSON**: Update component configurations
2. **Run Generation**: `python tools/generate_tables.py {bot_name} src`
3. **Build**: `pio run -e {environment}`
4. **Flash and Test**: Deploy to hardware and validate inter-subsystem communication
5. **Iterate**: Refine configurations based on testing results

**Critical Rule**: Never manually edit generated C++ files. All changes must be made through JSON configurations and regenerated.


**Prohibited Methods**:
- Direct communication protocol API calls
- Direct Bluetooth/WiFi APIs for inter-subsystem coordination
- Custom inter-ESP32 communication protocols
- File transfer or serial communication between subsystems

## RULE 9: DISPATCH TABLE GENERATION ARCHITECTURE

**CRITICAL: Six Files Generated Per Subsystem**

**Generated files are organized by bot AND subsystem. File names follow pattern: `{bot_family}_{subsystem_name}`**

**Rationale for Bot/Subsystem-Specific File Names:**
- Full system generation can take considerable time once all subsystems are added
- Each subsystem (head, torso, left_arm, right_arm, etc.) generates independently
- Bot-specific naming allows independent subsystem builds without forcing full regeneration
- Each subsystem can be built, tested, and deployed independently
- Preserves previously-generated working configurations (e.g., `goblin_head_*` files stay stable while generating `goblin_torso_*` files)
- Enables incremental development - new subsystems can be added without regenerating entire system

**Example for `goblin_head` subsystem:**
1. **`src/subsystems/goblin_head/goblin_head_component_functions.cpp`** - Component function aggregator
2. **`include/subsystems/goblin_head/goblin_head_component_functions.hpp`** - Function declarations for components
3. **`src/subsystems/goblin_head/goblin_head_dispatch_tables.cpp`** - Implementation with dispatch tables
4. **`include/subsystems/goblin_head/goblin_head_dispatch_tables.hpp`** - Header declarations for dispatch tables
5. **`src/subsystems/goblin_head/goblin_head_main.cpp`** - Core subsystem main entry point and initialization
6. **`include/subsystems/goblin_head/goblin_head_main.hpp`** - Header declarations for main entry point

**Example for `goblin_torso` subsystem (same creature, different subsystem):**
1. **`src/subsystems/goblin_torso/goblin_torso_component_functions.cpp`** - Component function aggregator
2. **`include/subsystems/goblin_torso/goblin_torso_component_functions.hpp`** - Function declarations
3. **`src/subsystems/goblin_torso/goblin_torso_dispatch_tables.cpp`** - Implementation with dispatch tables
4. **`include/subsystems/goblin_torso/goblin_torso_dispatch_tables.hpp`** - Header declarations
5. **`src/subsystems/goblin_torso/goblin_torso_main.cpp`** - Core subsystem main entry point
6. **`include/subsystems/goblin_torso/goblin_torso_main.hpp`** - Header declarations for main entry point

**Generation Process**:
- Process depth-first to create flat execution order per subsystem
- Generate bot/subsystem-specific filenames from bot definition JSON
- Aggregate component `.src` files into dispatch tables
- Create main.cpp/main.hpp with component registration and FreeRTOS task setup
- Generated files MUST be included in build system
- NEVER manually edit generated files
- Each subsystem's generated files are independent and persist across regenerations of other subsystems

**PlatformIO Configuration (platformio.ini):**
- Create one build environment per subsystem
- Example: `[env:goblin_head]`, `[env:goblin_torso]`, `[env:goblin_left_arm]`, etc.
- Each environment specifies the ESP32 board type based on that subsystem's GPIO/memory requirements (RULE 21)
- Complete creature builds require platformio.ini entries for ALL subsystems
- Multi-subsystem builds use: `pio run` (all environments) or `pio run -e goblin_head -e goblin_torso` (specific subsystems)

## RULE 10: MULTI-ESP32 DISTRIBUTED ARCHITECTURE

- **Each subsystem** runs on a dedicated ESP32 chip
- **Controllers** are defined by hardware type (ESP32-S3-DevKitC-1, etc.)
- **SharedMemory** handles inter-ESP32 communication transparently
- **SharedMemory** coordinates between ESP32 controllers

## RULE 11: PYTHON TOOLS FOR CODE GENERATION AND AUTOMATION

**Purpose**: Creates the controlling code for each ESP32 subsystem from JSON component configurations

**Tool Integration with Development Workflow**:
1. **Modify JSON Configurations**: Update bot definitions, component specifications, or hardware assignments
2. **Run Code Generation Tools**: Execute Python scripts to generate/update C++ code
3. **Build Subsystems**: Use PlatformIO to compile individual ESP32 targets
4. **Flash and Test**: Deploy to hardware and validate inter-subsystem communication
5. **Iterate**: Refine configurations based on testing results

**Critical Rule**: Never manually edit generated C++ files - all changes must be made through JSON component descriptions and the associated .src and .hdr files and regenerated via Python tools.

## RULE 11B: SUBSYSTEM VERSIONING AND UPCONVERSION

**Versioning System:**
- Every subsystem (generated set of files) includes a version identifier in its JSON bot definition
- Version format: `major.minor.patch` (e.g., `1.0.0`)
- Version increments when: JSON structure changes, component additions/removals, or critical bug fixes
- Version is recorded in the generated dispatch tables and main.hpp header

**Upconversion Requirement:**
- **CRITICAL**: When a subsystem is discovered to have an obsolete version, it MUST be immediately upconverted to the current version
- Upconversion means: regenerate ALL files from the subsystem's JSON using the current generation tools
- This ensures: header compatibility, communication protocol alignment, and SharedMemory class compatibility
- Obsolete versions cannot coexist with updated `/shared` classes - regeneration is mandatory

**Detection of Obsolete Versions:**
- Compilation errors related to SharedMemory classes indicate version mismatch
- Missing or changed function signatures in generated dispatch tables indicate version mismatch
- Any modification to `/shared` classes invalidates all subsystem versions - upconvert all subsystems

**Upconversion Process:**
1. Identify the subsystem with obsolete version
2. Locate the subsystem's JSON definition file
3. Run: `python tools/generate_tables.py config/bots/bot_families/{family}/{subsystem_definition}.json src`
4. Rebuild the subsystem: `pio run -e {subsystem_name}`
5. Verify compilation succeeds with no SharedMemory-related errors

**Rationale:**
- Prevents incompatible inter-subsystem communication
- Ensures all subsystems use matching SharedMemory class definitions
- Automatic detection and mandatory upconversion prevents silent failures
- Version tracking enables safe incremental development

## RULE 11C: TIMING GUIDELINES FOR COMPONENT EXECUTION

**Component `hitCount` values control execution frequency. Use these guidelines:**

| Component Type | hitCount Range | Purpose | Notes |
|---|---|---|---|
| **System heartbeat** | 1 | Every loop | Core system timing reference |
| **Animations** | 3-7 | Fast updates | Eyes, mouth, movements |
| **Sensors** | 10-20 | Responsive | Nose, ears, cameras |
| **Mood** | 10-15 | Frequent | Emotional state updates |
| **Personality** | 25 | Moderate | Behavior modulation |
| **Family behaviors** | 10-50 | Mid-range | Shared creature behavior |
| **Network/WiFi** | 50+ | Slow | Infrequent communication |

**Timing Philosophy:**
- Lower hitCount = higher frequency (executes more often)
- Higher hitCount = lower frequency (executes less often)
- System level: hitCount 1-50 (always responsive)
- Family level: hitCount 10-50 (moderate frequency)
- Bot-specific: hitCount 3-30 (high frequency for hardware)

**Critical Rule**: NOTHING executes unless it's a component with `init()` and `act()` functions with a defined `hitCount`.

## RULE 11D: JSON FILE ENCODING AND VALIDATION

**ASCII Encoding is MANDATORY - No UTF-8 BOM:**

All JSON configuration files MUST be saved as pure ASCII without UTF-8 BOM.

**Why This Matters:**
- UTF-8 BOM (bytes `EF BB BF` at file start) breaks Python JSON parser
- Error: "Expecting value: line 1 column 1 (char 0)"
- This is a known recurring issue in the project

**Correct File Format (PowerShell):**
```powershell
# Save JSON as ASCII without BOM
[System.IO.File]::WriteAllText($path, $content, [System.Text.Encoding]::ASCII)
```

**Detection of BOM Corruption (PowerShell):**
```powershell
# Check for BOM corruption
$bytes = [System.IO.File]::ReadAllBytes($jsonFile)
if ($bytes[0] -eq 239 -and $bytes[1] -eq 187 -and $bytes[2] -eq 191) {
    Write-Host "UTF-8 BOM DETECTED - File is corrupted!" -ForegroundColor Red
    # Fix it:
    $content = Get-Content $jsonFile -Raw
    [System.IO.File]::WriteAllText($jsonFile, $content, [System.Text.Encoding]::ASCII)
}
```

**Validation Rules:**
- Always use ASCII encoding
- Never use UTF-8 with BOM
- Verify first 3 bytes are NOT `239, 187, 191`
- First byte should be `123` (the `{` character)
- Run validation: `python config/validate.py` after creating JSON files

## RULE 12: DEVELOPMENT PRACTICES

**Windows Development Environment**:
1. Use Windows PowerShell syntax only
2. Use forward slash paths: `config/bots/bot_families/goblins/goblin_full.json`
3. NO Linux/Unix commands ever
4. Create files that work natively on Windows
5. Use simple PowerShell commands, not complex scripts
6. ASCII encoding only - NO UTF-8 - never Unicode
7. Test before scaling up
8. One file at a time if complex operations fail
9. Never validate VS Code config files (.vscode/*) as project JSON - exclude from consistency checks
10. Respect .gitignore files - files and folders listed in .gitignore are NOT part of the project and must be excluded from all validation and processing

**Token Cost Awareness**:
- Failed commands cost tokens from user budget
- Repeated mistakes are expensive
- Test approach before implementing at scale
- Use proven patterns that work except when they contradict one of the rules established for this project

**Defined SharedMemory Classes:**

21

**⚠️ CRITICAL**: Any changes to the classes in this directory invalidates all of the generated files in `src/` and `include/`. Regenerate immediately after modifying any `/shared` class.

**1. Mood Class** - `/shared/Mood.hpp`
- Contains 9 different emotions with strength values for each
- Emotions: ANGER, FEAR, HAPPINESS, SADNESS, CURIOSITY, AFFECTION, IRRITATION, CONTENTMENT, EXCITEMENT
- Used by personality components to modify creature emotional state
- Read by display/animation components to adjust visual expressions

**2. Environment Class** - `/shared/Environment.hpp`
- Contains current surroundings and sensor data
- Variables include: distance_cm, temperature_c, humidity, sound_level_db, face_detected, target_confidence
- Updated by sensor components (nose, ears, cameras)
- Read by behavior and personality components for environmental reactions

**3. Personality Class** - `/shared/Personality.hpp` - FUTURE
- Will contain creature personality configuration data
- Expected to grow as personality system develops
- Loaded from JSON personality files during initialization
- Contains thresholds, reaction strengths, and behavioral parameters


**Prohibited Communication Methods:**
- Direct communication protocol API calls
- Direct Bluetooth classic or BLE APIs
- Direct WiFi communication for inter-subsystem coordination
- Custom inter-ESP32 communication protocols
- File transfer or serial communication between subsystems

## RULE 18: JSON-DRIVEN PERSONALITY SYSTEM

**Data-Driven Personality Architecture:**

**Personality module is entirely data-driven** - no hardcoded behavior logic!

- **JSON defines personality traits** - how strongly creature reacts to stimuli
- **C++ code is generic** - reads JSON config, applies formulas
- **Same code works for all creatures** - only JSON changes
- **Artists can tweak personalities** - no programmer required

**Personality JSON Structure Requirements:**
```json
{
    "personality_name": "Creature Name",
    "personality_id": "CREATURE_ID",
    "mood_reactions": {
        "close_object": {
            "distance_threshold_cm": 30,
            "triggers": [
                {"mood": "CURIOSITY", "delta": 10},
                {"mood": "EXCITEMENT", "delta": 15}
            ]
        }
    },
    "mood_decay": {
        "decay_interval_seconds": 5,
        "decay_rates": [
            {"mood": "EXCITEMENT", "delta": -5}
        ]
    },
    "default_mood": {
        "ANGER": 0, "FEAR": 0, "HAPPINESS": 50,
        "CURIOSITY": 30, "AFFECTION": 20
    }
}
```

**Generic Personality Component Pattern:**
- Load personality JSON from filesystem during init()
- Apply mood reactions based on environment triggers
- Use SharedMemory for reading Environment and writing Mood
- Support mood decay over time
- Enable different creatures to have distinct personalities using same code
 `

## RULE 19: THREE-LEVEL COMPONENT ATTACHMENT ARCHITECTURE

**CRITICAL DESIGN PRINCIPLE:**

**NOTHING executes unless it's a component with init() and act() functions.**

**Pure Component-Driven Architecture:**
- All functionality is implemented as components with `init()` and `act()` functions
- Components are attached at three hierarchical levels
- Core loop iterates through registered components calling their `act()` functions based on `hitCount` timing

**Three Component Levels:**

**Level 1: System Components (Always Attached)**
- Core platform functionality required by all bots
- Always included in every build, lowest execution frequency
- Examples: `system_core`, `network_monitor`, `serial_console`, `power_monitor`, `watchdog_heartbeat`

**Level 2: Family Components (Shared Across Bot Family)**
- Behavior/personality shared across creature family (Goblin, Cat, Bear)
- Examples: `goblin_behavior`, `goblin_mood`, `goblin_personality`

**Level 3: Bot-Specific Components (Positioned Hardware)**
- Physical hardware components with spatial coordinates
- Examples: `goblin_left_eye`, `goblin_right_eye`, `goblin_nose`, `goblin_mouth`

**Component Registration Requirements:**
- JSON configuration defines component hierarchy
- Code generation creates dispatch tables
- Compilation links all component implementations
- Runtime execution based on hitCount timing

## RULE 20: TWO-TIER MOUNTING ARCHITECTURE

**NEVER CONFUSE UNIVERSAL MOUNTS WITH CREATURE SHELLS**

**TIER 1: Universal Hardware Mounts (IDENTICAL across ALL creatures)**
- `sensor_basic_mount.scad/.stl` - HC-SR04 ultrasonic (ALL creatures)
- `display_basic_mount.scad/.stl` - GC9A01 round LCD (ALL creatures)
- `speaker_basic_mount.scad/.stl` - 40mm I2S speaker (ALL creatures)
- `servo_basic_mount.scad/.stl` - SG90, MG996R servos (ALL creatures)

**Universal Mount Characteristics:**
- Precise component fit with exact hardware dimensions
- Standard M3 screw mounting, consistent hole patterns
- Built-in cable management and wire routing
- Utilitarian function over form, minimal aesthetics
- ZERO creature-specific features (no warts, whiskers, scales)

**TIER 2: Creature-Specific Decorative Shells (UNIQUE per creature)**
- `goblin_nose_shell.scad/.stl` - Incorporates `sensor_basic_mount` + warty texture
- `cat_nose_shell.scad/.stl` - Incorporates `sensor_basic_mount` + pink triangle nose
- `bear_nose_shell.scad/.stl` - Incorporates `sensor_basic_mount` + black button nose

**Shell Integration Pattern:**
```scad
// goblin_nose_shell.scad
use <../basic_mounts/sensor_basic_mount.scad>

module goblin_nose_shell() {
    // Goblin-specific decorative features
    warty_nose_texture();
    large_nostril_details();

    // Universal hardware mount integration
    sensor_basic_mount();
}
```

**Critical Design Rule:**
- Universal mounts provide hardware compatibility
- Decorative shells provide creature aesthetics
- Same electronics work across all creatures via shell substitution

## RULE 21: MULTI-ESP32 ENVIRONMENT SELECTION

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

## RULE 22: JSON MOUNTING ARCHITECTURE SPECIFICATION

**Every positioned component JSON MUST include a `mounting_architecture` field:**

```json
{
  "mounting_architecture": {
    "type": "two_tier | integrated | hardware_direct",
    "hardware_mount": "basic_mount_name (if two_tier)",
    "decorative_shell": "shell_name (if two_tier)",
    "integrated_shape": "shape_name (if integrated)",
    "shape_generation": "auto | manual | template"
  }
}
```

**Three Mounting Architecture Types:**

**1. Two-Tier Architecture** (`"type": "two_tier"`)
- Use when hardware will be reused across multiple creatures
- Generates universal `basic_mount.scad/.stl` + creature-specific `shell.scad/.stl`
- Shell incorporates hardware mount using `use <../basic_mounts/mount.scad>` pattern

**2. Integrated Single-Piece** (`"type": "integrated"`)
- Use when custom design is simpler than two-tier approach
- Generates single `creature_part_complete.scad/.stl` with built-in hardware mount
- Hardware mount geometry integrated into creature-specific shape

**3. Hardware Direct** (`"type": "hardware_direct"`)
- Use when using hardware component physical shape directly
- No custom mounting shape generated
- Hardware mounts directly to skull/frame using vendor holes/brackets

**Shape Generation Options:**
- **Auto**: System generates OpenSCAD from hardware specs and templates
- **Manual**: Designer creates custom OpenSCAD files manually
- **Template**: Uses existing template files with parameter substitution

**Physical Integration Requirements:**
- All mounting specs must define precise hardware fit
- Cable management and strain relief required
- Standard M3 screw mounting patterns preferred
- Maintenance access and tool clearance consideration

## RULE 23: BRACES STYLE - ALLMAN FORMATTING

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

## RULE 24: DECISION AUTHORITY vs. ESCALATION

**This rule clarifies the bounded authority framework from the Bounded Authority Framework section above.**

**Agent has autonomous decision authority FOR:**
- Fixing clear violations (UTF-8 in ASCII-only file, missing braces, wrong file location)
- Following established patterns from existing working code
- Applying rules that directly specify the action for the situation
- Filling empty directories with standard components matching existing patterns
- Generating code that follows established generation patterns

**Agent MUST escalate TO HUMAN FOR:**
- Contradictory rules (report which rules conflict)
- Genuinely novel situations not covered by existing patterns or rules
- Multiple valid technical interpretations of the same requirement
- Changes affecting multiple subsystems or potential breaking changes
- Unknown or missing references mentioned in rules

**Format for escalation:**
When escalating, include: (1) the specific rule/pattern, (2) situation details, (3) decision options, (4) your recommendation with reasoning, (5) what specifically you're blocked on.

## RULE 25: ASCII ENCODING ONLY

**ALL files MUST BE ASCII encoded (NO BOM or UTF8)**

**Reference:** See `docs/component-json-requirements.md` for complete JSON parsing rules and validation requirements.

## RULE 26: COMPONENT GENERATION IS MANDATORY

**ALL component code MUST be generated via scripts - NEVER manually created**

**CRITICAL ARCHITECTURE REQUIREMENT:**

**NEVER edit the GENERATED files in `src/components/` (.cpp/.hpp extensions)**

```
IF creating_component_code THEN
    → Use generate_tables.py or generate_individual_components.py

    → NEVER manually create .cpp/.hpp files in the src/ or include/ folders
    → NEVER manually edit generated dispatch tables
ELSE
    → Build will fail with missing headers
    → Inconsistent naming conventions
    → Architecture violations
```

**Component Generation Commands:**

- Generate for specific bot: `python tools\generate_tables.py config\bots\bot_families\{family}\{bot}.json src`
- Individual components: `python tools\generate_individual_components.py`

**Manual component creation ALWAYS fails** - the generation system is required for:

- Consistent naming conventions
- Proper header file generation
- Dispatch table integration
- Multi-ESP32 variant support

---
