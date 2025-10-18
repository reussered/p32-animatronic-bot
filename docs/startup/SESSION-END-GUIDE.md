# Session End Documentation Guide

## 🎯 Purpose

Create **layered documentation** that preserves all knowledge but loads efficiently for new agents.

---

## 📊 Three-Tier Documentation System

### Tier 1: Quick Resume (REQUIRED - Max 100 lines)

**File naming:** `QUICK-RESUME-[DATE].md`

**Contents:**
- Current issue/task (3 sentences max)
- Immediate next steps (numbered list, max 5 items)
- Critical files modified
- Quick command to resume work

**Example:**
```markdown
# Quick Resume - Oct 16

## Issue: LED blinks too fast
Loop speed faster than expected. Added diagnostic.

## Next Steps:
1. Run: pio run -t upload -t monitor
2. Read delta value from serial
3. Update hitCount in JSON
4. Regenerate tables

## Modified: src/components/simple_core_test.cpp
```

---

### Tier 2: Full Session Report (REQUIRED - Detailed but organized)

**File naming:** `SESSION-END-[DATE]-[TOPIC].md`

**Contents (use these exact sections):**

#### 1. Executive Summary (5-10 lines)
```markdown
## Session Summary
**Date:** [DATE]
**Duration:** [TIME]
**Status:** [Complete/In Progress/Blocked]
**Issue:** [One sentence]
**Resolution:** [One sentence or "In progress"]
```

#### 2. Problem Analysis (As needed)
```markdown
## Problem Analysis

### What Went Wrong
[Detailed description]

### Root Cause
[Technical explanation]

### Why It Matters
[Impact on project]
```

#### 3. Solution Applied (As needed)
```markdown
## Solution

### Changes Made
[List with file paths]

### Code Changes
[Key snippets with context]

### Verification Steps
[How to confirm it works]
```

#### 4. Knowledge Gained (CRITICAL - Never skip!)
```markdown
## Lessons Learned

### New Understanding
[What we discovered about the system]

### Mistakes Made
[What NOT to do - future agents need this!]

### Patterns Established
[Reusable solutions]
```

#### 5. Next Agent Instructions (REQUIRED)
```markdown
## Resume Instructions

### Prerequisites
[What to read first - links to other docs]

### Step-by-Step
1. [Concrete action]
2. [Expected result]
3. [Next action]

### Success Criteria
[How to know it's working]
```

#### 6. File Inventory
```markdown
## Modified Files
- `path/to/file.ext` - [Brief description of changes]
- `path/to/file2.ext` - [Brief description of changes]

## Created Files
- `path/to/new/file.ext` - [Purpose]

## Files Needing Review
- `path/to/file.ext` - [Why it needs attention]
```

---

### Tier 3: Reference Documentation (Update as needed)

**Permanent knowledge bases** - update these when you learn something fundamental:

- `docs/LESSONS-LEARNED-BUILD-ERRORS.md` - Add new build errors
- `.github/AI-AGENT-RULES.md` - Add new ironclad rules
- Architecture specs - Update when design changes

---

## 🚀 Session End Checklist

When ending a session, create:

1. ✅ **Quick Resume** - 50-100 lines, immediate context
2. ✅ **Full Session End** - Comprehensive, well-organized
3. ✅ **Update START-HERE.md** - Point to your new docs
4. ✅ **Update relevant permanent docs** - If you learned something fundamental

---

## 📝 Template: Quick Resume

```markdown
# Quick Resume - [DATE]

## Current Task
[One sentence - what are we doing?]

## Status
[Complete/In Progress/Blocked]

## Next Steps
1. [Action]
2. [Action]
3. [Action]

## Key Files
- `path/file` - [why it matters]

## Commands
```bash
[command to resume work]
```

## Context (if needed)
→ `SESSION-END-[DATE]-[TOPIC].md` - Full details
```

---

## 📝 Template: Full Session End

```markdown
# Session End - [DATE] - [TOPIC]

## Session Summary
**Date:** [DATE]
**Duration:** [HOURS]
**Status:** [STATUS]
**Branch:** [GIT BRANCH]

**Issue:** [One sentence]
**Resolution:** [One sentence]

---

## Problem Analysis

### What Happened
[Detailed description]

### Root Cause
[Technical explanation]

### Impact
[Why it matters]

---

## Solution Applied

### Changes Made
- `file1` - [description]
- `file2` - [description]

### Key Code Changes
```language
[code snippet with context]
```

### How to Verify
1. [Step]
2. [Expected result]

---

## Lessons Learned

### New Understanding
- [Discovery 1]
- [Discovery 2]

### Mistakes Made
- [What NOT to do]
- [Why it was wrong]

### Patterns Established
- [Reusable solution]

---

## Resume Instructions

### Read First
- `doc1.md` - [Why]
- `doc2.md` - [Why]

### Next Steps
1. [Action]
2. [Expected result]
3. [Next action]

### Success Criteria
- [ ] [Checkpoint 1]
- [ ] [Checkpoint 2]

---

## File Inventory

### Modified
- `path/file` - [changes]

### Created
- `path/file` - [purpose]

### Needs Attention
- `path/file` - [issue]

---

## Related Documentation
- `doc1.md` - [relevant topic]
- `doc2.md` - [relevant topic]
```

---

## 🎓 Best Practices

### DO:
✅ Keep Quick Resume under 100 lines
✅ Use clear section headers in Full Session End
✅ Include commands in code blocks
✅ Link to related docs
✅ Document mistakes (future agents need this!)
✅ Include file paths with changes
✅ Write concrete next steps

### DON'T:
❌ Put everything in one giant file
❌ Skip the Quick Resume (next agent needs fast context!)
❌ Forget to document mistakes
❌ Use vague language ("might need to...", "probably...")
❌ Include entire file contents (just the relevant snippets)
❌ Skip the "why" (explain reasoning, not just what)

---

## 💡 The Philosophy

**Knowledge Preservation ≠ Loading Everything At Once**

- Preserve ALL hard-won knowledge (in Full Session End)
- Make it accessible on-demand (via Quick Resume + START-HERE)
- Layer information by urgency (Quick → Full → Reference)
- Let agents load context as needed (not all upfront)

**Result:** Fast startup + Complete knowledge + No token overflow

---

## 📞 Questions to Ask Yourself

Before ending session:

1. ✅ Can next agent resume in 5 minutes with Quick Resume?
2. ✅ Is all knowledge preserved in Full Session End?
3. ✅ Did I document mistakes for future agents?
4. ✅ Are next steps concrete and testable?
5. ✅ Did I update START-HERE.md?
6. ✅ Did I update permanent knowledge bases if needed?

---

**Remember:** You're not just documenting for yourself - you're teaching the next agent!
