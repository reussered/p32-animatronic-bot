# START HERE - New Agent Instructions

## ⚠️ BEFORE YOU DO ANYTHING ⚠️

**Read `.github/AI-AGENT-RULES.md` FIRST** (5 min read)

**Key mindset:**
- ✅ Architecture is FINAL - implement, don't redesign
- ✅ main.cpp is IMMUTABLE - never add delays
- ✅ Timing via hitCount ONLY - not FreeRTOS delays
- ✅ 40+ hours of decisions made - respect them

**If you find yourself thinking "why don't we just..."**
→ STOP. Read RULE 0.5 in AI-AGENT-RULES.md.

---

## 🎯 Current Task (2 minutes to resume)

**Issue:** LED blinks 10x/sec, should be 1x/sec  
**Solution Ready:** Diagnostic code added to measure actual loop speed  
**Next Step:** Upload and read the delta value from serial monitor

```powershell
pio run -t upload -t monitor
```

Look for: `🔆 ON - Loop: X (delta: YYYYY)`  
The delta value = actual hitCount needed.

---

## 📚 Knowledge Base (Read as Needed)

### Critical Rules (2 min read)
→ `.github/AI-AGENT-RULES.md` - Ironclad rules NEVER break

### Architecture Understanding (10 min read)
→ `docs/COMPONENT-FUNCTION-SIGNATURES.md` - NO ARGUMENTS pattern  
→ `docs/CORE-LOOP-SPEC.md` - main.cpp is IMMUTABLE  
→ `docs/DISTRIBUTED-PROCESSING-ARCHITECTURE.md` - System design

### Common Mistakes & Solutions (5 min read)
→ `docs/LESSONS-LEARNED-BUILD-ERRORS.md` - Every mistake documented with fixes

### Previous Session Details (when needed)
→ `SESSION-END-HARDWARE-SMOKE-INCIDENT.md` - Display testing smoke incident  
→ `SESSION-END-OCT16-LOOP-SPEED-ISSUE.md` - Current loop speed calibration  
→ `QUICK-RESUME-OCT16.md` - Tonight's work summary

---

## 💡 How to Use This System

**Strategy:** Read documents ONLY when you need the specific knowledge.

- Starting session? Read this file only (3 min)
- Need to understand components? Read COMPONENT-FUNCTION-SIGNATURES.md
- Hit a build error? Read LESSONS-LEARNED-BUILD-ERRORS.md
- Confused about architecture? Read relevant spec docs

**Don't load everything at once!** Use semantic search or grep to find what you need.

---

## 🔧 Project Status Dashboard

| Area | Status | Next Action |
|------|--------|-------------|
| **Current Task** | ⚠️ Loop speed calibration | Measure delta, update hitCount |
| **Build System** | ✅ Compiling | Working |
| **Component Tables** | ✅ Generated | Working |
| **ESP32-S3 Hardware** | ✅ Functional | Passed smoke test |
| **Display Testing** | ⏳ Waiting | New GC9A01s ordered |
| **Architecture** | ✅ Complete | 3,576 lines documented |

---

## 🚀 Quick Commands Reference

```powershell
# Build and upload
pio run -t upload -t monitor

# Regenerate component tables
python tools/generate_tables.py simple_test_bot src

# Check for errors
pio run

# View git changes
git status
git diff
```

---

## 📞 Emergency Context Recovery

If you're confused or need full context:

1. Read `QUICK-RESUME-OCT16.md` (50 lines)
2. Search for specific topics: use semantic_search or grep_search tools
3. Read specific docs only when needed
4. **Don't try to load everything at once!**

---

**Total read time for basic context:** 3 minutes  
**Total read time for full understanding:** 30-60 minutes (read docs as needed)

**The knowledge is preserved. Access it when you need it, not all at once!**
