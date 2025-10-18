# P32 Layered Component Architecture Guide

**CRITICAL: Read this before modifying any components!**  
*For AI Agents working on P32 Animatronic Bot*  
*Created: 2025-10-18*

## 🏗️ **Multi-Level Component System Overview**

The P32 system uses a **hierarchical component architecture** where components are layered with different responsibilities. **Never modify the wrong layer!**

### 📋 **The Layer Principle:**
Each component layer has **specific responsibilities** and **specific missing elements**:

- **Higher layers** = More specific (position, animation, creature traits)
- **Lower layers** = More generic (hardware drivers, basic functions)

---

## 🔍 **Example: Display Component Hierarchy**

### **Level 1: goblin_left_eye** (Position + Animation Layer)
```json
File: config/components/positioned/goblin/head_components/goblin_left_eye.json
```
- **✅ HAS**: Specific position coordinates, animation sequences, timing
- **❌ MISSING**: Shape definition, hardware driver functions
- **🎯 ROLE**: "Where does it go and how does it move?"

### **Level 2: goblin_eye** (Shape + Appearance Layer)  
```json
File: config/components/shapes/goblin_eye.json (hypothetical)
```
- **✅ HAS**: Shape definition, creature-specific appearance, visual design
- **❌ MISSING**: Position coordinates, hardware driver functions  
- **🎯 ROLE**: "What does it look like?"

### **Level 3: gc9a01** (Hardware Driver Layer)
```json
File: config/components/hardware/gc9a01.json (hypothetical)
```
- **✅ HAS**: Hardware driver functions (gc9a01_init, gc9a01_act), test execution
- **❌ MISSING**: Shape definition, position coordinates
- **🎯 ROLE**: "How does the hardware actually work?"

---

## 🔄 **Component Execution Chain**

When the system runs, components call down the hierarchy:

```cpp
// Execution Flow:
goblin_left_eye_act()     // Sets up position and animation context
  └── goblin_eye_act()    // Handles shape and appearance rendering  
      └── gc9a01_act()    // Performs actual hardware operations + TESTS
```

### 🎯 **KEY INSIGHT: Tests Go in Hardware Driver Layer**

**SystemTest checking happens in `gc9a01_act()`** - the lowest hardware layer:

```cpp
// gc9a01_act() function:
SystemTest test;
gsm.read("g_Test", &test);
if(test.hasTest("DisplayTest_1")) {
    run_display_test_1();
    return;  // Exit early, skip normal operations
}
if(test.hasTest("GC9A01_Clear_Screen")) {
    clear_screen_test();
    return;
}
// Normal GC9A01 hardware operations if no tests
```

---

## 🚨 **CRITICAL RULES FOR AI AGENTS**

### ❌ **DON'T DO THIS:**
- **Don't add test code to positioning layers** (goblin_left_eye)
- **Don't add position code to hardware layers** (gc9a01)  
- **Don't create separate test driver components**
- **Don't modify component JSON files to add test functionality**

### ✅ **DO THIS:**
- **Add tests to the hardware driver layer** (gc9a01_act function)
- **Keep layer responsibilities separate**
- **Use SystemTest variable to coordinate between layers**
- **Let testpicker populate SystemTest via GSM**

### 🎯 **The SystemTest Design Philosophy:**

**"Components check SystemTest naturally without JSON modifications"**

The genius of SystemTest is that:
1. **testpicker_act()** populates tests in GSM
2. **Hardware drivers** naturally check SystemTest during normal operation  
3. **No component JSON files need modification**
4. **Testing logic stays separate from component architecture**

---

## 🔧 **How to Add New Tests**

### **Step 1: Add test to testpicker component**
```json
// In testpicker.json:
"test_cases_to_add": [
  "DisplayTest_1",
  "GC9A01_Clear_Screen",
  "NEW_TEST_HERE"  ← Add here
]
```

### **Step 2: Hardware driver checks SystemTest**
```cpp
// In gc9a01_act() function (auto-generated):
if(test.hasTest("NEW_TEST_HERE")) {
    run_new_test();
    return;
}
```

### **Step 3: That's it!**
- No JSON modifications to existing components
- No new components needed
- System automatically coordinates via GSM

---

## 📚 **Component Layer Examples**

### **Multi-Level Nesting (3-5+ levels possible):**

```
creature_variant_component         ← Level 1: Creature-specific traits
  └── body_part_component          ← Level 2: Body part positioning  
      └── mechanical_component     ← Level 3: Mechanical design
          └── hardware_driver      ← Level 4: Hardware control + TESTS
              └── subsystem        ← Level 5: Low-level subsystems
```

### **Different Separation Patterns:**
- **Position vs Shape vs Function**
- **Creature vs Generic vs Hardware**  
- **Animation vs Appearance vs Driver**
- **High-level vs Mid-level vs Low-level**

---

## 🎯 **For Next Agent: Key Takeaways**

1. **Understand the layer you're working on** - Position? Shape? Hardware?
2. **Tests go in hardware driver layers** - Not positioning or shape layers
3. **SystemTest coordinates between layers** - No JSON modifications needed
4. **Each layer has specific responsibilities** - Don't mix them
5. **Components can be 3, 4, 5+ levels deep** - Follow the chain down to hardware

### **Quick Check:**
- Adding position code? → Find positioning layer component
- Adding shape/appearance? → Find shape layer component  
- Adding hardware/tests? → Find hardware driver layer component
- Adding tests? → Use SystemTest + hardware driver, don't modify JSON

---

**Remember: The P32 system's power comes from clean layer separation. Respect the architecture!**