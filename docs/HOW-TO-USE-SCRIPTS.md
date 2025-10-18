# How to Use P32 Animatronic Scripts

**Complete Guide to P32 Build System Scripts**  
*Created: 2025-10-18*  
*Author: reussered*

## Overview

The P32 Animatronic Bot uses a sophisticated build system with JSON-driven component generation, naming convention enforcement, and comprehensive testing tools. This document explains how to use all available scripts.

---

## 🏗️ Core Build System Scripts

### 📋 Component Generation Scripts

#### `generate_tables.py` - Main Component Dispatch Generator
**Purpose**: Generates C++ component tables and execution dispatch from JSON configurations

```powershell
# Generate component tables for a specific bot
python tools\generate_tables.py config\bots\simple_test_bot.json src\

# Generate for full goblin configuration  
python tools\generate_tables.py config\bots\bot_families\goblins\goblin_full.json src\
```

**What it generates**:
- `include\p32_component_tables.hpp` - Component function declarations
- `src\p32_component_tables.cpp` - Init/Act function dispatch tables
- `src\system_components.cpp` - System-level component implementations
- `src\bot_components.cpp` - Bot-specific component implementations

#### `generate_individual_components.py` - Memory Analysis Components
**Purpose**: Creates separate source files for each component to enable precise memory analysis

```powershell
# Generate individual component files (for memory measurement)
python tools\generate_individual_components.py simple_test_bot

# Generate with specific bot configuration
python tools\generate_individual_components.py goblin_full
```

**What it generates**:
- `src\components\{component_name}.c` - Individual component source files
- `include\p32_component_registry.h` - Component registry header
- `src\p32_components.cmake` - CMake component definitions

#### `inheritance_processor.py` - JSON Template Resolution
**Purpose**: Resolves family template inheritance chains into final creature configurations

```powershell
# Process inheritance for all creatures
python tools\inheritance_processor.py

# Process specific creature family
python tools\inheritance_processor.py --family goblins
```

**Inheritance Pattern**:
```
Base Templates → Family Templates → Creature Overrides → Final Configuration
humanoid_base.json → goblin_family.json → goblin_warrior.json → resolved\goblin_warrior.json
```

---

## 🔍 Code Quality & Naming Scripts

### 📝 Naming Convention Enforcement

## 🔤 **P32 Naming Convention Rules**

### **Core Naming Rules:**
1. **All generated function names → lowercase**
   - `component_name_init()`, `component_name_act()`
2. **All JSON component names → lowercase**  
   - `testpicker.json`, `gc9a01_display_driver.json`
3. **All C++ filenames → lowercase**
   - `testpicker.cpp`, `main.cpp`, `component_tables.cpp`
4. **EXCEPTION: Class-containing files → keep uppercase**
   - `SystemTest.hpp`, `Mood.cpp`, `GlobalSharedMemory.hpp`

### **Examples:**
```
✅ CORRECT:
- testpicker.json → testpicker_init(), testpicker_act()
- gc9a01_display_driver.json → gc9a01_display_driver_init()
- SystemTest.hpp (contains SystemTest class)

❌ INCORRECT:  
- TestPicker.json, p32_comp_testpicker_init()
- GC9A01_Display.cpp, TESTPICKER_ACT()
```

#### `check_cpp_naming.py` - Validate C++ Naming
**Purpose**: Ensures CPP filenames match their init/act function prefixes

```powershell
# Check all C++ files for naming convention compliance
python tools\check_cpp_naming.py

# Check specific directory
python tools\check_cpp_naming.py src\components\
```

#### `fix_cpp_naming.py` - Auto-Fix C++ Naming
**Purpose**: Automatically fixes CPP filename/function name mismatches

```powershell
# Fix all C++ naming issues (with backup)
python tools\fix_cpp_naming.py

# Preview fixes without applying
python tools\fix_cpp_naming.py --dry-run
```

#### `check_json_naming.py` - Validate JSON Naming
**Purpose**: Ensures JSON component files follow naming conventions

```powershell
# Check all JSON components for naming compliance
python tools\check_json_naming.py

# Check specific component type
python tools\check_json_naming.py --type system
```

#### `fix_json_naming.py` - Auto-Fix JSON Naming  
**Purpose**: Automatically fixes JSON component naming issues

```powershell
# Fix all JSON naming issues
python tools\fix_json_naming.py

# Fix specific component directory
python tools\fix_json_naming.py config\components\system\
```

---

## 🛡️ Protection & Validation Scripts

### 🔒 File Protection System

#### `checkout-files.ps1` - Safe File Modification
**Purpose**: Safely check out files for modification with automatic backup

```powershell
# Check out specific files for modification
.\tools\checkout-files.ps1 -Files "src\main.cpp","config\system.json"

# Check out entire project
.\tools\checkout-files.ps1 -AllProject

# Show current checkout status  
.\tools\checkout-files.ps1 -ShowStatus

# Restore from backup
.\tools\checkout-files.ps1 -Restore

# Commit changes (remove backups)
.\tools\checkout-files.ps1 -Commit
```

#### `validate-protection.py` - Protection System Validation
**Purpose**: Detect unauthorized file modifications and validate protection system

```powershell
# Validate protection system is working
python tools\validate-protection.py

# Check for unauthorized modifications
python tools\validate-protection.py --strict
```

---

## 🧪 Testing & Verification Scripts

### ✅ Component Verification

#### `verify_components.ps1` - Component Validation
**Purpose**: Validates all required components for P32 Animatronic Bot

```powershell
# Verify all components are present and configured correctly
.\tools\verify_components.ps1

# Quick component check
.\tools\quick_verify.ps1
```

#### `verify_full_system.ps1` - Complete System Check
**Purpose**: Comprehensive system validation including hardware, software, and configuration

```powershell
# Full system verification
.\tools\verify_full_system.ps1

# System check with detailed output
.\tools\system_check.ps1
```

### 🖥️ Display & Testing Scripts

#### `test_display_server.py` - Display Test Server
**Purpose**: Creates web-based display testing interface for GC9A01 displays

```powershell
# Start display test server
python tools\test_display_server.py

# Start with specific port
python tools\test_display_server.py --port 8080
```

#### `esp32_display_server.py` - ESP32 Display Communication
**Purpose**: Direct communication interface with ESP32 display systems

```powershell
# Connect to ESP32 display system
python tools\esp32_display_server.py

# Connect with specific serial port
python tools\esp32_display_server.py --port COM3
```

---

## 🏭 Manufacturing & 3D Generation Scripts

### 🖨️ STL Generation Scripts

#### `generate-stl-files.ps1` - STL File Generation
**Purpose**: Generates 3D printable STL files from OpenSCAD sources

```powershell
# Generate all STL files
.\tools\generate-stl-files.ps1

# Generate specific component STLs
.\tools\generate-stl-files.ps1 -Component "skull"

# Test STL generation
.\tools\test-stl-generation.ps1
```

#### `generate-skull-stls.ps1` - Skull-Specific STL Generation
**Purpose**: Specialized STL generation for skull assembly components

```powershell
# Generate skull assembly STLs
.\tools\generate-skull-stls.ps1

# Generate with custom parameters
.\tools\generate-skull-stls.ps1 -Resolution "0.2mm"
```

### 👀 Visualization Tools

#### `launch-stl-viewer.ps1` - STL Viewer Launch
**Purpose**: Launches web-based STL file viewer for 3D model inspection

```powershell
# Launch STL viewer
.\tools\launch-stl-viewer.ps1

# Launch skull-specific viewer
.\tools\launch-skull-viewer.ps1
```

---

## 🎬 Animation & Media Scripts

### 🗣️ Animation Generation

#### `mouth-animations.ps1` - Mouth Animation Creator  
**Purpose**: Creates grinding maw and mouth animations for GC9A01 circular displays

```powershell
# Generate mouth animations
.\tools\mouth-animations.ps1

# Generate with custom parameters
.\tools\mouth-animations.ps1 -AnimationType "speak"
```

#### `quick-sound.ps1` - Sound System Test
**Purpose**: Quick sound system testing and audio validation

```powershell
# Test sound system
.\tools\quick-sound.ps1

# Test with specific audio file
.\tools\quick-sound.ps1 -AudioFile "test.wav"
```

---

## 🏃‍♂️ Quick Operations Scripts

### ⚡ Rapid Development Tools

#### `quick_goblin_demo.py` - Goblin Demo Launcher
**Purpose**: Quickly demonstrates goblin animatronic capabilities

```powershell
# Run goblin demo
python tools\quick_goblin_demo.py

# Run with specific goblin configuration
python tools\quick_goblin_demo.py --config goblin_warrior
```

#### `setup-environment.ps1` - Environment Setup
**Purpose**: Sets up development environment with all required dependencies

```powershell
# Setup complete development environment
.\tools\setup-environment.ps1

# Setup minimal environment
.\tools\setup-environment.ps1 -Minimal
```

---

## 🔧 Utility Scripts

### 🛠️ System Utilities

#### `OpenSCAD-Utils.ps1` - OpenSCAD Helper Functions
**Purpose**: Utility functions for working with OpenSCAD 3D modeling

```powershell
# Import OpenSCAD utilities (in other scripts)
. .\tools\OpenSCAD-Utils.ps1

# Available functions: Test-OpenSCAD, Convert-ToSTL, etc.
```

#### `printer-keepalive.ps1` - 3D Printer Connection
**Purpose**: Maintains connection to 3D printer during long print jobs

```powershell
# Start printer keepalive
.\tools\printer-keepalive.ps1

# Monitor specific printer
.\tools\printer-keepalive.ps1 -Printer "CR-10"
```

---

## 📋 Script Usage Patterns

### 🔄 Typical Development Workflow

1. **Start Development Session**:
   ```powershell
   .\tools\checkout-files.ps1 -Files "src\components\MyComponent.cpp"
   ```

2. **Validate Code Quality**:
   ```powershell
   python tools\check_cpp_naming.py
   python tools\check_json_naming.py
   ```

3. **Generate Component Tables**:
   ```powershell
   python tools\generate_tables.py config\bots\simple_test_bot.json src\
   ```

4. **Verify System**:
   ```powershell
   .\tools\verify_components.ps1
   ```

5. **Commit Changes**:
   ```powershell
   .\tools\checkout-files.ps1 -Commit
   ```

### 🧪 Testing Workflow

1. **Component Testing**:
   ```powershell
   python tools\generate_individual_components.py simple_test_bot
   python tools\test_display_server.py
   ```

2. **System Validation**:
   ```powershell
   .\tools\verify_full_system.ps1
   python tools\validate-protection.py
   ```

### 🏭 Manufacturing Workflow

1. **Generate 3D Models**:
   ```powershell
   .\tools\generate-stl-files.ps1
   .\tools\generate-skull-stls.ps1
   ```

2. **Visual Inspection**:
   ```powershell
   .\tools\launch-stl-viewer.ps1
   ```

3. **Print Preparation**:
   ```powershell
   .\tools\printer-keepalive.ps1
   ```

---

## ⚠️ Important Notes

### 🚨 Before Using Scripts

1. **Always backup** critical files before running modification scripts
2. **Check protection status** with `validate-protection.py` before major changes
3. **Use checkout system** for safe file modifications
4. **Verify naming conventions** before generating component tables

### 🔒 Protection System Rules

- Use `checkout-files.ps1` for all file modifications
- Never edit protected files directly
- Run `validate-protection.py` to check system integrity
- Follow **DISCUSSION BEFORE ACTION** rule for significant changes

### 📝 JSON-Driven Architecture

- All components are defined in JSON files first
- Python scripts generate C++ implementation from JSON
- Never manually edit generated C++ files
- Always modify JSON sources and regenerate

---

## 🆘 Troubleshooting

### Common Issues

**Script not found**: Ensure you're running from project root directory
**Permission denied**: Run PowerShell as Administrator if needed
**Python errors**: Check Python environment and dependencies
**Component generation fails**: Verify JSON syntax and required fields
**Protection violations**: Use checkout system properly

### Getting Help

1. Check script headers for usage instructions
2. Run scripts with `-h` or `--help` for detailed options
3. Verify file permissions and paths
4. Check prerequisite scripts have been run first

---

*This documentation covers all major scripts in the P32 Animatronic Bot build system. Always refer to individual script headers for the most current usage information.*