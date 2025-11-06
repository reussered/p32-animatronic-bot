# Component Standardization Script Usage

## Overview

This PowerShell script automates the standardization of component reference keywords in JSON configuration files for the P32 Animatronic Bot project.

## What it does

- Changes any keyword matching `*component*` to exactly `"components"`
- Converts single component references to arrays: `"component": "file.json"`  `"components": ["file.json"]`
- Preserves `"family_components"` as an exception
- Maintains all component file references and connections

## Usage

### Basic usage (process current directory)

```powershell
.\standardize_components.ps1
```

### Process specific directory

```powershell
.\standardize_components.ps1 -Path "f:\GitHub\p32-animatronic-bot\config"
```

### Dry run (see what would be changed)

```powershell
.\standardize_components.ps1 -WhatIf
```

### Verbose output

```powershell
.\standardize_components.ps1 -Verbose
```

### Combined options

```powershell
.\standardize_components.ps1 -Path ".\config" -WhatIf -Verbose
```

## Parameters

- `-Path`: Directory to scan for JSON files (default: current directory)
- `-WhatIf`: Show what would be changed without making actual modifications
- `-Verbose`: Display detailed processing information

## Important Notes

- **Always backup your files before running** (especially without `-WhatIf`)
- Files with multiple component sections may need manual consolidation after running the script
- The script preserves all component connections and file references
- Only JSON files are processed

## Example Output

```text
P32 Component Keyword Standardization Script
==============================================
Found 866 JSON files to process
Processing: config\bots\test_bot.json
  No changes needed in: config\bots\test_bot.json
Processing: config\components\hardware\gc9a01_display.json
  Converted single component to array: config/components/drivers/pwm_driver.json
  Made 1 changes in: config\components\hardware\gc9a01_display.json
...
==============================================
Standardization Complete!
Files processed: 866
Total changes made: 47
```