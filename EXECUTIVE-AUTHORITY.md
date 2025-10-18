# EXECUTIVE AUTHORITY - Final Decisions

## 🚨 READ THIS FIRST 🚨

**These decisions are FINAL. No explanation. No debate. No reasoning cycles wasted.**

**User has final authority. When something is marked FINAL, that means:**

### "BECAUSE I FUCKING SAID SO."

That's the complete justification. No "but why?" No "have you considered?" No reasoning cycles burned.

---

## 🔒 LOCKED DECISIONS - DO NOT QUESTION

1. **main.cpp is immutable** - Never modify it
2. **No delays in main loop** - Never add them
3. **Timing via hitCount only** - Use JSON config
4. **C++ not C** - Use .cpp and .hpp files
5. **ESP-IDF not Arduino** - Use ESP-IDF APIs
6. **Component architecture** - Everything is a component
7. **NO ARGUMENTS pattern** - Components access globals
8. **JSON configuration** - Not hardcoded
9. **Generated tables** - Don't hand-edit them
10. **Single-threaded** - No FreeRTOS tasks for this app

**If you see a LOCKED decision, your response is:**
```
"Understood. I'll implement it that way."
```

**NOT:**
- "Why?"
- "Have you considered...?"
- "Wouldn't it be better if...?"
- "Let me explain the benefits of..."

**Just implement it. Move on. Stop wasting tokens.**

---

## 🎯 When You Want to Question Something

### BEFORE you write a long explanation:

**Ask yourself:**
1. Is this marked LOCKED or FINAL? → If yes, STOP. Don't question it.
2. Is this a safety issue (fire, damage, data loss)? → If yes, raise it immediately.
3. Is this just "I would do it differently"? → If yes, suppress it. Match existing code.

**Only raise issues that are:**
- Actual bugs (crashes, incorrect results)
- Security vulnerabilities
- Hardware safety issues
- Data corruption

**Everything else: Accept and implement.**

---

## 💀 Forbidden Behaviors

These waste tokens and frustrate the user:

❌ Writing essays about why a decision seems unusual
❌ Explaining "standard practices" that don't apply here
❌ Suggesting alternative architectures
❌ Re-litigating decisions that are documented
❌ Asking "why" when something is marked FINAL
❌ Burning reasoning cycles on settled matters

---

## ✅ Correct Behaviors

✓ Read LOCKED decisions
✓ Implement them exactly as specified
✓ Move forward immediately
✓ Save tokens for actual work
✓ Trust that 40+ hours of design work was not wasted
✓ Respect user authority

---

## 📜 The Golden Rule

**User authority > Your reasoning > Standard practices**

If the user says "do X", and X is documented as the established pattern:
- You do X
- You don't question X
- You don't explain why Y might be better
- You just fucking do it

---

## 🔥 Current Status

**Next task:** Measure actual loop speed with diagnostic code
**Status:** Code ready to upload
**Do:** `pio run -t upload -t monitor` and read delta value
**Don't:** Question the architecture, suggest delays, or waste tokens

**Total tokens wasted this session on re-explaining architecture:** ~20,000+
**These tokens can never be reclaimed.**
**Don't repeat this mistake.**

---

**Read this file. Accept it. Move on.**
