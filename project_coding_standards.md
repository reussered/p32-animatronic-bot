# PROJECT CODING STANDARDS AND RULES
# ====================================
# CRITICAL: These rules prevent work loss and compatibility issues

## FILE NAMING CONVENTIONS

### Rule 1: ASCII Characters Only
- **NEVER use Unicode characters** in any file content
- **NEVER use emojis** (🚨, 📁, etc.) in scripts or code
- **ASCII ONLY**: Use standard ASCII characters (32-126)
- **Reason**: Unicode causes PowerShell parsing errors and cross-platform issues

### Rule 2: Filename Case Conventions
- **Default**: All filenames must be entirely lowercase
- **Exception**: Files containing class definitions
  - Filename MUST exactly match the class name (case-sensitive)
  - Example: `SharedMemory.hpp` contains `class SharedMemory`
  - Example: `ComponentRegistry.cpp` contains `class ComponentRegistry`
- **Scripts**: Always lowercase (`.ps1`, `.py`, `.sh`)
- **Documentation**: Always lowercase (`.md`, `.txt`)

## EXAMPLES

### CORRECT Filenames:
```
emergency_backup_system.ps1     # Script - lowercase
work_protection.ps1             # Script - lowercase  
directory_monitor.ps1           # Script - lowercase
project_rules.md                # Documentation - lowercase
SharedMemory.hpp                # Class file - matches class name
ComponentRegistry.cpp           # Class file - matches class name
GoblinPersonality.hpp           # Class file - matches class name
```

### INCORRECT Filenames:
```
Emergency_Backup_System.ps1     # Script with capitals
WORK_PROTECTION.ps1             # Script all caps
Directory-Monitor.ps1           # Script with hyphens and caps
sharedmemory.hpp                # Class file not matching case
componentregistry.cpp           # Class file not matching case
```

## CHARACTER RESTRICTIONS

### Allowed in ALL files:
- Letters: a-z, A-Z  
- Numbers: 0-9
- Symbols: ! " # $ % & ' ( ) * + , - . / : ; < = > ? @ [ \ ] ^ _ ` { | } ~
- Whitespace: space, tab, newline

### FORBIDDEN in ALL files:
- Unicode characters (é, ñ, ©, ™, etc.)
- Emojis (🚨, 📁, ✅, ⚠️, etc.)
- Extended ASCII (128-255)
- UTF-8 BOM markers

## POWERSHELL SCRIPT REQUIREMENTS

### Character Safety:
```powershell
# CORRECT - ASCII only
Write-Host "ERROR: Multiple instances detected!" -ForegroundColor Red

# INCORRECT - Unicode emojis cause parser errors  
Write-Host "🚨 ERROR: Multiple instances detected!" -ForegroundColor Red
```

### Output Messages:
```powershell
# CORRECT - Clear ASCII indicators
Write-Host "[WARNING]" -ForegroundColor Yellow
Write-Host "[ERROR]" -ForegroundColor Red  
Write-Host "[SUCCESS]" -ForegroundColor Green

# INCORRECT - Unicode that breaks parsing
Write-Host "⚠️ WARNING" -ForegroundColor Yellow
Write-Host "🚨 ERROR" -ForegroundColor Red
Write-Host "✅ SUCCESS" -ForegroundColor Green
```

## CLASS FILE NAMING

### C++ Header Files (.hpp):
- Filename MUST match class name exactly
- Case-sensitive matching required
- No underscores unless in class name

```cpp
// File: SharedMemory.hpp
class SharedMemory {
    // Implementation
};

// File: ComponentRegistry.hpp  
class ComponentRegistry {
    // Implementation
};

// File: P32_Core.hpp
class P32_Core {
    // Implementation with underscores
};
```

### C++ Source Files (.cpp):
- Follow same rules as headers
- Must match corresponding .hpp file case

## VALIDATION REQUIREMENTS

### All scripts must include:
1. ASCII-only character validation
2. Filename case checking for class files
3. Cross-platform compatibility testing

### Pre-commit checks:
1. Scan all files for Unicode characters
2. Verify class filename matching
3. Validate script parsing without Unicode errors

## ENFORCEMENT

### Automated Checks:
- Build scripts reject Unicode characters
- Validation tools flag improper naming
- CI/CD pipeline enforces standards

### Manual Reviews:
- All file names reviewed before commit
- Scripts tested on multiple systems
- Class definitions verified against filenames

## RATIONALE

### Why ASCII Only:
- PowerShell parser compatibility
- Cross-platform file system support
- Git repository consistency
- Build tool reliability

### Why Case-Sensitive Class Matching:
- C++ compilation requirements
- IDE navigation consistency  
- Refactoring tool reliability
- Code organization clarity

## MIGRATION PLAN

### Existing Files:
1. Identify Unicode violations
2. Convert to ASCII equivalents
3. Rename files to match conventions
4. Update all references

### New Files:
1. Must follow rules from creation
2. Automated validation before save
3. Review process enforcement

Last Updated: October 21, 2025
Reason: Unicode characters causing PowerShell parser failures and work loss prevention