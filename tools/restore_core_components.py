#!/usr/bin/env python3
"""
Targeted restoration of the 11 core goblin head component JSON files from commit 018e3d2.
"""

import json
import subprocess
import sys
from pathlib import Path

BASE_COMMIT = "018e3d2"

# The 11 core files that existed in the original commit
CORE_FILES = [
    "config/bots/bot_families/goblins/head/goblin_authority_markings.json",
    "config/bots/bot_families/goblins/head/goblin_head.json",
    "config/bots/bot_families/goblins/head/goblin_head_left_eye_only.json",
    "config/bots/bot_families/goblins/head/goblin_left_ear.json",
    "config/bots/bot_families/goblins/head/goblin_left_eye.json",
    "config/bots/bot_families/goblins/head/goblin_mouth.json",
    "config/bots/bot_families/goblins/head/goblin_nose.json",
    "config/bots/bot_families/goblins/head/goblin_pirate_head.json",
    "config/bots/bot_families/goblins/head/goblin_right_ear.json",
    "config/bots/bot_families/goblins/head/goblin_right_eye.json",
    "config/bots/bot_families/goblins/head/goblin_speaker.json",
]

def get_original(filepath):
    """Get JSON from original commit."""
    try:
        result = subprocess.run(
            ['git', 'show', f'{BASE_COMMIT}:{filepath}'],
            capture_output=True,
            text=True,
            check=False
        )
        if result.returncode == 0:
            return json.loads(result.stdout)
    except:
        pass
    return None

def read_current(filepath):
    """Read current file."""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            return json.loads(f.read())
    except:
        return None

def write_file(filepath, data):
    """Write file."""
    try:
        with open(filepath, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=4)
        return True
    except:
        return False

def main():
    print("\n" + "="*70)
    print("TARGETED RESTORATION - 11 CORE GOBLIN HEAD FILES")
    print("="*70)
    print(f"Base commit: {BASE_COMMIT}\n")
    
    restored = 0
    failed = 0
    unchanged = 0
    
    for filepath in CORE_FILES:
        filename = Path(filepath).name
        print(f"Processing: {filename}...", end=' ', flush=True)
        
        original = get_original(filepath)
        current = read_current(filepath)
        
        if not current:
            print("SKIP (cannot read current)")
            continue
        
        if not original:
            print("SKIP (not in original)")
            unchanged += 1
            continue
        
        # Get fields
        orig_components = original.get('components', [])
        orig_use_fields = original.get('use_fields', {})
        curr_components = current.get('components', [])
        curr_use_fields = current.get('use_fields', {})
        
        # Check if changes needed
        if orig_components == curr_components and orig_use_fields == curr_use_fields:
            print("OK (unchanged)")
            unchanged += 1
            continue
        
        # Apply restoration
        if orig_components != curr_components:
            print(f"RESTORE components ({len(curr_components)} -> {len(orig_components)})", end='')
            current['components'] = orig_components
        
        if orig_use_fields != curr_use_fields:
            print(f" + use_fields ({len(curr_use_fields)} -> {len(orig_use_fields)} fields)", end='')
            current['use_fields'] = orig_use_fields
        
        if write_file(filepath, current):
            print(" [DONE]")
            restored += 1
        else:
            print(" [FAILED]")
            failed += 1
    
    # Summary
    print("\n" + "="*70)
    print(f"Restored:  {restored}")
    print(f"Unchanged: {unchanged}")
    print(f"Failed:    {failed}")
    print(f"Total:     {restored + unchanged + failed}")
    print("="*70 + "\n")
    
    return 0 if failed == 0 else 1

if __name__ == '__main__':
    sys.exit(main())
