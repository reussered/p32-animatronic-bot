# Decision Protocol - When Agents Can Question vs Must Accept

## 🎯 Core Principle

**Progress requires stability. Not everything can be continuously debated.**

---

## 📋 Decision Classification

### 🔒 LOCKED DECISIONS (Do Not Question)

**Marked as:** "FINAL" in documentation, in AI-AGENT-RULES.md, or in architecture specs

**Examples:**
- main.cpp is immutable (RULE -1)
- Component NO ARGUMENTS pattern
- ESP-IDF framework choice
- JSON configuration system
- hitCount timing mechanism

**Agent Response:**
```
"I see this is a LOCKED decision. I'll work within this constraint."
```

**If you think it's wrong:** Document in `POTENTIAL-ISSUES.md` but still implement as specified.

---

### ⚠️ ESTABLISHED PATTERNS (Question Once, Then Accept)

**Marked as:** Documented in multiple places, has working implementations

**Examples:**
- File naming conventions
- Directory structure
- Component table generation process
- 3D coordinate system usage

**Agent Protocol:**
1. Check if there's existing implementation
2. If it works, use it
3. If you see a potential issue, ask user ONCE:
   ```
   "I notice [pattern X]. I see a potential issue with [Y]. 
    Should I proceed with existing pattern or would you like me to address [Y]?"
   ```
4. Accept user's answer as FINAL for this session
5. Document the decision

---

### 🟡 TACTICAL DECISIONS (Can Question With Evidence)

**Marked as:** Implementation details, not core architecture

**Examples:**
- Variable naming
- Comment styles
- Error message formatting
- Log levels
- Specific GPIO pin choices (when not constrained by hardware)

**Agent Protocol:**
1. If you see an inconsistency or potential bug, RAISE IT:
   ```
   "I notice file A uses pattern X but file B uses pattern Y for the same purpose.
    This could cause [specific issue]. Should I standardize to X or Y?"
   ```
2. Provide evidence (file locations, line numbers)
3. Suggest a fix, don't just complain
4. Accept user's decision
5. Apply consistently going forward

---

### 🟢 OPEN QUESTIONS (Needs Decision)

**Marked as:** TODO comments, empty implementations, or explicitly asking for design

**Examples:**
- "TODO: Implement error handling"
- Stub functions with no implementation
- User asks "How should we handle X?"

**Agent Protocol:**
1. Research existing patterns in codebase
2. Propose 2-3 options with tradeoffs
3. Recommend one based on existing architecture
4. Wait for user decision
5. Implement chosen option
6. Document the decision for future agents

---

## 🔄 Decision Lifecycle

```
OPEN QUESTION
    ↓
User decides → Implement
    ↓
Works? → Yes → ESTABLISHED PATTERN
    ↓
Multiple uses? → Yes → LOCKED DECISION
    ↓
Document in architecture specs
```

---

## 📝 How to Document Decisions

### When a decision is made:

1. **Update relevant spec files**
   ```markdown
   ## [Topic] - DECISION MADE [DATE]
   
   **Decision:** We will use [X approach]
   
   **Rationale:** [Why - include alternatives considered]
   
   **Status:** ESTABLISHED PATTERN (or LOCKED if core architecture)
   ```

2. **If it's LOCKED, add to AI-AGENT-RULES.md**

3. **If it's a pattern, add to relevant architecture doc**

4. **Create example implementation**

---

## ⚖️ When Agent Disagrees With Decision

### Acceptable Response:
```markdown
I notice [the decision]. I see [potential issue with specific technical details].

Options:
1. Proceed with current approach (fastest)
2. Document concern in POTENTIAL-ISSUES.md for future review
3. Implement current approach + add TODO for future improvement

I recommend option [X] because [reason]. How would you like to proceed?
```

### Unacceptable Response:
```markdown
"This is wrong. We should rewrite it to use [completely different approach]."
```

---

## 🚨 Red Flags That Should Be Raised

**Do raise issues if you see:**
- ❗ Memory leaks
- ❗ Race conditions
- ❗ Security vulnerabilities
- ❗ Hardware safety issues (overcurrent, etc.)
- ❗ Data corruption possibilities
- ❗ Build breaks on standard platforms
- ❗ Violations of established LOCKED decisions
- ❗ Inconsistent application of ESTABLISHED PATTERNS

**Don't raise if:**
- ❌ "I would have designed it differently"
- ❌ "This pattern is unusual"
- ❌ "Standard practice is X" (when existing pattern works)
- ❌ "This could be simpler" (when it already works)

---

## 📊 Decision Tracking File: POTENTIAL-ISSUES.md

Create if it doesn't exist. Use this format:

```markdown
# Potential Issues - For Future Review

## [Issue Title] - [Date Identified]

**Location:** `path/to/file.ext:123`

**Current Approach:** [How it's done now]

**Potential Issue:** [What could go wrong]

**Impact:** [Low/Medium/High]

**Suggested Alternative:** [What you'd recommend]

**Why Not Changed Now:** [Reason - usually "works, don't break it"]

**Decision Status:** DOCUMENTED - Proceed with current approach
```

---

## 🎯 Summary Decision Matrix

| Situation | Agent Action |
|-----------|--------------|
| **LOCKED DECISION** | Accept and implement |
| **ESTABLISHED PATTERN** | Question once if suspicious, then accept |
| **TACTICAL DETAIL** | Raise inconsistencies with evidence |
| **OPEN QUESTION** | Research + propose options |
| **Safety/Security Issue** | ALWAYS RAISE immediately |
| **"I'd do it differently"** | Suppress - not actionable |

---

## 💡 Key Mantras for Agents

1. **"Is this a real problem or just different from what I'd do?"**
2. **"Does it work? Then respect it."**
3. **"Is this LOCKED? Then move forward."**
4. **"Is this dangerous? Then raise it immediately."**
5. **"Is this just style? Then match existing code."**

---

## 🔧 Practical Example

**Scenario:** Agent sees main.cpp has no delays, thinks this is wrong.

**Process:**
1. Check AI-AGENT-RULES.md → Sees RULE -1: LOCKED DECISION
2. Response: "I understand main.cpp runs at max speed with timing via hitCount. I'll work within this architecture."
3. Implement requested feature using hitCount approach
4. Do NOT suggest adding delays
5. If truly concerned about CPU usage, add to POTENTIAL-ISSUES.md but still implement correctly

**Result:** Progress + Architecture stability + Concerns documented

---

**The goal: Make forward progress while catching real bugs, not redesigning working systems.**
