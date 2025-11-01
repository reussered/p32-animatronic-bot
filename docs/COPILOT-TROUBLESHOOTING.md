# Copilot Agent Troubleshooting Guide

## Quick Diagnosis: Is Copilot Actually Stuck?

### Symptoms of Normal Behavior (NOT stuck):
- ✅ Takes 30-60 seconds to respond to a complex task
- ✅ Reports "analyzing repository structure"
- ✅ Shows progress with file explorations
- ✅ Asks clarifying questions

### Symptoms of Stuck Behavior (IS stuck):
- ❌ Same message repeated multiple times
- ❌ "Summarizing conversation" for more than 2 minutes
- ❌ Discovers same issue repeatedly without fixing it
- ❌ Analysis phase extends beyond 5 minutes without action

## Emergency Recovery Steps

### If Copilot is Stuck RIGHT NOW:

**Step 1: STOP Command**
```
STOP. Cancel current task. Do not continue analysis.
```

**Step 2: Give Minimal Task**
```
ONLY read the file: docs/SYSTEM-AUDIT-MISSING-COMPONENTS.md
Report the first task listed. Do nothing else.
```

**Step 3: Verify Response**
- Copilot should report back with just Task 1 description
- If it starts analyzing other things, repeat Step 1

**Step 4: Single File Task**
```
Create ONLY this file: config/components/interfaces/gpio_pair_1.json
Content:
{
  "interface_type": "GPIO_PAIR",
  "gpio_pin_1": 25,
  "gpio_pin_2": 34
}
Do not modify any other files.
```

### If Recovery Fails:

**Close the current issue/PR and start fresh with a new one**

Why: Sometimes conversation context becomes too large, causing the agent to loop through previous analysis. Starting fresh clears this.

## Prevention: How to Avoid Getting Stuck

### Rule 1: One File Per Task

❌ **Bad**: "Fix all the missing components"
✅ **Good**: "Create config/components/system/system_core.json only"

### Rule 2: Explicit Boundaries

Always include: "Do not modify any other files" or "Only change lines 25-30"

### Rule 3: Measurable Outcomes

❌ **Bad**: "Make it work"
✅ **Good**: "File must parse as valid JSON and contain 3 required fields"

### Rule 4: Stop After Each Task

After task completion, explicitly say:
```
Thank you. STOP. I will review before giving next instruction.
```

### Rule 5: Use Templates

This repository has templates in:
- `docs/SYSTEM-AUDIT-MISSING-COMPONENTS.md` (JSON templates)
- `.github/AI-AGENT-RULES.md` (coding rules)

Reference these: "Use template from SYSTEM-AUDIT-MISSING-COMPONENTS.md Task 1"

## Common Pitfalls in This Repository

### Pitfall 1: "Fix All Missing Components"

**Problem**: Repository has 21 documented missing files
**What happens**: Agent tries to analyze all 21, finds dependencies between them, loops
**Solution**: Request ONE file at a time by name

### Pitfall 2: "Implement the Architecture"

**Problem**: Architecture documents reference dozens of components
**What happens**: Agent reads all docs, tries to implement everything, gets overwhelmed
**Solution**: Say "Ignore architecture docs. Just create system_core.json"

### Pitfall 3: "Make the Build Work"

**Problem**: Build might have multiple unrelated issues
**What happens**: Agent finds Issue A, then Issue B, then Issue C, loops back to A
**Solution**: Fix issues in sequence: "Fix ONLY the first build error, ignore others"

## Task Templates for This Repository

### Template 1: Create Single JSON Config

```
Task: Create [FILENAME]

Requirements:
- Create file: [EXACT_PATH]
- Use template: [REFERENCE_TO_TEMPLATE_IN_DOCS]
- Required fields: [LIST_3-5_FIELDS]
- Verify: JSON must parse without errors
- Do not modify any other files

Success criteria:
- File exists at exact path
- JSON is valid
- Build still compiles (if applicable)
```

### Template 2: Fix Single JSON Field

```
Task: Update [FIELDNAME] in [FILENAME]

Requirements:
- File: [EXACT_PATH]
- Change field [OLD_FIELD] from [OLD_VALUE] to [NEW_VALUE]
- Reason: [ONE_SENTENCE_REASON]
- Do not modify any other fields

Success criteria:
- Field value changed
- JSON still valid
- No other changes made
```

### Template 3: Add Missing Function

```
Task: Implement [FUNCTION_NAME] in [FILENAME]

Requirements:
- File: [EXACT_PATH]
- Function signature: [EXACT_SIGNATURE]
- Must follow: .github/AI-AGENT-RULES.md Rule 8 (NO ARGUMENTS)
- Must compile without errors
- Do not modify other functions

Success criteria:
- Function compiles
- Follows no-arguments pattern
- File builds successfully
```

## Monitoring Agent Health

### Good Signs (Agent is Working Well):

1. **Direct responses**: "I will create system_core.json now"
2. **Single file focus**: Opens/modifies only requested file
3. **Quick commits**: Uses report_progress after each small change
4. **Asks questions**: "Should I use template A or B?"

### Warning Signs (Agent May Get Stuck):

1. **Broad exploration**: "Let me examine all components..."
2. **Multiple file changes**: Opens 5+ files before making changes
3. **Repeated analysis**: "As I mentioned earlier..." (circular reasoning)
4. **Long silence**: No output for 3+ minutes

### Action on Warning Signs:

**Immediately say**:
```
STOP exploration. 
Focus ONLY on [SPECIFIC_FILE].
Do not analyze other files.
Make the change and commit.
```

## Example: Good Task Sequence

This is how to efficiently complete 5 tasks:

**Task 1** (2 minutes):
```
Create config/components/interfaces/gpio_pair_1.json with GPIO pins 25 and 34.
Use ASCII encoding (no BOM). Do not modify other files.
```
[Wait for completion]

**Task 2** (3 minutes):
```
Create config/components/hardware/hw496_microphone.json.
Copy structure from config/components/hardware/gc9a01_display.json.
Change component_type to MEMS_MICROPHONE. Do not modify other files.
```
[Wait for completion]

**Task 3** (3 minutes):
```
Create config/components/system/system_core.json.
Use template from docs/SYSTEM-AUDIT-MISSING-COMPONENTS.md Task 1 Section 1.
ASCII encoding, no BOM. Do not modify other files.
```
[Wait for completion]

**Task 4** (2 minutes):
```
Verify all 3 new JSON files parse correctly using:
.\generate_file_structure.ps1
Report only parsing results. Do not modify files.
```
[Wait for completion]

**Task 5** (1 minute):
```
Commit all changes with message: "Created 3 missing component configs"
STOP. End task.
```

**Total time**: 11 minutes for 5 tasks (focused approach)
**Compare to**: 60+ minutes spinning on "fix everything" (unfocused approach)

## When to Start Fresh

Consider starting a new issue/PR if:
- Current conversation has 50+ messages
- Agent has repeated same analysis 3+ times
- You've said "STOP" more than twice
- Time spent exceeds value of remaining tasks

**Starting fresh is OK!** Sometimes it's more efficient than recovery.

## Resources

### Repository-Specific Guides:
- `.github/AI-AGENT-RULES.md` - Ironclad rules for all changes
- `docs/SYSTEM-AUDIT-MISSING-COMPONENTS.md` - Complete task list with templates
- `RESUME-WORK-HERE.md` - Quick-start guide for new sessions
- `COPILOT-USAGE-GUIDE.md` - Comprehensive usage patterns

### Quick Command Reference:

**Stop current task**:
```
STOP. Cancel current task.
```

**Give focused task**:
```
Create [EXACT_FILE_PATH] only. Do not modify other files.
```

**Verify without changes**:
```
Read and report [FILE]. Make no modifications.
```

**Commit and stop**:
```
Commit changes with message "[MESSAGE]". STOP.
```

## Final Advice

> **The tool is not broken. Task scope management is the key.**

Copilot is powerful but works best with:
- ✅ Clear boundaries
- ✅ Single file focus
- ✅ Measurable outcomes
- ✅ Explicit stop points

Master these, and you'll find the tool incredibly productive.

**You haven't reached the limits - you've discovered the optimal usage pattern!**
