#!/usr/bin/env python3
"""
Update references from category folder files to family folder files.
"""

import json
from pathlib import Path
import sys

root = Path("/home/runner/work/p32-animatronic-bot/p32-animatronic-bot")

# Mapping of old paths (to be removed) to new paths (to keep)
REPLACEMENTS = {
    "config/bots/bot_families/animals/bear_family.json": 
        "config/bots/bot_families/bears/bear_family.json",
    
    "config/bots/bot_families/animals/cat_family.json": 
        "config/bots/bot_families/cats/cat_family.json",
    
    "config/bots/bot_families/fantasy/goblin_family.json": 
        "config/bots/bot_families/goblins/goblin_family.json",
    
    "config/bots/bot_families/goblins/subsystems/goblin_head.json": 
        "config/bots/bot_families/goblins/head/goblin_head.json",
    
    "config/bots/bot_families/goblins/subsystems/goblin_head_left_eye_only.json": 
        "config/bots/bot_families/goblins/head/goblin_head_left_eye_only.json",
    
    "config/bots/bot_families/goblins/subsystems/goblin_pirate_head.json": 
        "config/bots/bot_families/goblins/head/goblin_pirate_head.json",
    
    "config/bots/bot_families/mechanical/robot_family.json": 
        "config/bots/bot_families/robots/robot_family.json",
    
    "config/bots/bot_families/tests/subsystems/test_head.json": 
        "config/bots/bot_families/tests/head/test_head.json",
}

def update_file_references(file_path):
    """Update references in a JSON file."""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        replacements_made = []
        
        for old_path, new_path in REPLACEMENTS.items():
            if old_path in content:
                content = content.replace(old_path, new_path)
                replacements_made.append((old_path, new_path))
        
        if replacements_made:
            # Write updated content
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)
            return replacements_made
        
        return None
    except Exception as e:
        print(f"Error processing {file_path}: {e}")
        return None

def main():
    print("=== Updating File References ===\n")
    
    # Find all JSON files
    json_files = list(root.glob("config/bots/bot_families/**/*.json"))
    
    total_files_updated = 0
    total_replacements = 0
    
    for json_file in sorted(json_files):
        replacements = update_file_references(json_file)
        if replacements:
            total_files_updated += 1
            rel_path = json_file.relative_to(root)
            print(f"\n{rel_path}:")
            for old, new in replacements:
                print(f"  {old}")
                print(f"    -> {new}")
                total_replacements += 1
    
    print(f"\n\n=== Summary ===")
    print(f"Files updated: {total_files_updated}")
    print(f"Total replacements: {total_replacements}")
    
    if total_files_updated == 0:
        print("\nNo files needed updating.")
    else:
        print(f"\n✓ Successfully updated {total_files_updated} files")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
