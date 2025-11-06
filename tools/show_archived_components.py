#!/usr/bin/env python3
"""Display the list of all component files that would be archived"""

import json
from pathlib import Path
from typing import List

def has_bot_type(file_path: Path) -> bool:
    """Check if file has bot_type field (indicates it's a real bot)."""
    try:
        with open(file_path, 'r') as f:
            data = json.load(f)
        return "bot_type" in data
    except:
        return False

def identify_components(bots_dir: Path) -> List[Path]:
    """Find files that are components (no bot_type) in the bots directory."""
    bot_files = list(bots_dir.glob("**/[!.]*.json"))
    
    # Filter to only bot-like files (exclude intended components)
    bot_files = [f for f in bot_files if "components" not in str(f) and "subsystems" not in str(f)]
    
    component_files = []
    for bot_file in bot_files:
        # Quick heuristic: component names include "eye", "ear", "nose", etc.
        if any(pattern in bot_file.name.lower() for pattern in 
               ["eye", "ears", "mouth", "nose", "snout", "speaker", "sensor", "actuator"]):
            # Verify it's truly a component (no bot_type)
            if not has_bot_type(bot_file):
                component_files.append(bot_file)
    
    return sorted(component_files)

if __name__ == "__main__":
    bots_dir = Path("config/bots")
    if not bots_dir.exists():
        print(f"Error: Bots directory not found: {bots_dir}")
        exit(1)
    
    component_files = identify_components(bots_dir)
    
    print("\n" + "="*80)
    print("COMPLETE LIST OF ARCHIVED COMPONENTS (49 FILES)")
    print("="*80)
    
    # Group by creature type
    groups = {}
    for f in component_files:
        # Extract creature type from path or filename
        creature = "other"
        for part in ["android", "bear", "cat", "dragon", "elf", "goblin", "pixie", "wolf", "cyclops", "ghost"]:
            if part in f.name.lower():
                creature = part
                break
        
        if creature not in groups:
            groups[creature] = []
        groups[creature].append(f.name)
    
    # Print grouped
    total = 0
    for creature in sorted(groups.keys()):
        print(f"\n {creature.upper()} ({len(groups[creature])} files):")
        for fname in sorted(groups[creature]):
            print(f"    {fname}")
            total += 1
    
    print("\n" + "="*80)
    print(f"TOTAL: {total} component files")
    print("="*80)
    print("\n These files will be moved to: config/bots/.archived_components/")
    print(" They will NOT be deleted - just archived for cleanup")
    print("\n")
