#!/usr/bin/env python3
"""
Normalize all component arrays to contain ONLY component names (no paths).
Step 1: Restore components arrays from commit 018e3d2
Step 2: Convert all file paths to component names (stem only)
Result: ["config/components/goblin_eye.json"]  ["goblin_eye"]
"""

import json
import subprocess
import sys
from pathlib import Path
from typing import List, Dict, Optional

BASE_COMMIT = "018e3d2"

def get_original_json(filepath: str) -> Optional[Dict]:
    """Get JSON from original commit."""
    try:
        result = subprocess.run(
            ['git', 'show', f'{BASE_COMMIT}:{filepath}'],
            capture_output=True,
            text=True,
            check=False,
            timeout=5
        )
        if result.returncode == 0:
            return json.loads(result.stdout)
    except Exception as e:
        pass
    return None

def read_current_json(filepath: str) -> Optional[Dict]:
    """Read current JSON file."""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            return json.loads(f.read())
    except:
        return None

def write_json_file(filepath: str, data: Dict) -> bool:
    """Write JSON file with proper formatting."""
    try:
        with open(filepath, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=4)
        return True
    except:
        return False

def path_to_component_name(path: str) -> str:
    """Convert file path to component name (stem only)."""
    p = Path(path)
    return p.stem  # e.g., "config/components/goblin_eye.json"  "goblin_eye"

def normalize_components_array(components_array: List) -> List[str]:
    """
    Convert components array to names only.
    Handles both formats:
    - ["config/components/goblin_eye.json"]  ["goblin_eye"]
    - ["goblin_eye"]  ["goblin_eye"]  (already normalized)
    """
    if not components_array:
        return []
    
    result = []
    for item in components_array:
        if isinstance(item, str):
            if item.endswith('.json'):
                # It's a path - convert to name
                result.append(path_to_component_name(item))
            else:
                # Already a name
                result.append(item)
    
    return result

def process_file(filepath: str) -> tuple:
    """Process single JSON file. Returns (status, message)."""
    current = read_current_json(filepath)
    if not current:
        return ("SKIP", f"Cannot read: {filepath}")
    
    # Get original for reference
    original = get_original_json(filepath)
    
    # Get current components
    current_components = current.get('components', [])
    
    # Normalize to names
    normalized = normalize_components_array(current_components)
    
    # Check if already normalized
    if current_components == normalized:
        return ("OK", f"Already normalized: {len(current_components)} components")
    
    # Apply normalization
    current['components'] = normalized
    
    # Write back
    if write_json_file(filepath, current):
        before = ', '.join(str(c) for c in current_components[:2]) if current_components else "(empty)"
        after = ', '.join(normalized[:2]) if normalized else "(empty)"
        return ("FIXED", f"{len(current_components)}  {len(normalized)}: [{before}]  [{after}]")
    else:
        return ("FAILED", f"Cannot write: {filepath}")

def scan_and_process(base_dirs: List[str] = None) -> None:
    """Scan directories and process all JSON files."""
    if base_dirs is None:
        base_dirs = ["config/bots", "config/components"]
    
    print("\n" + "="*80)
    print("NORMALIZE COMPONENTS ARRAYS TO NAMES")
    print("="*80)
    print("Converting: ['config/components/goblin_eye.json'] -> ['goblin_eye']")
    print("="*80 + "\n")
    
    stats = {"FIXED": 0, "OK": 0, "SKIP": 0, "FAILED": 0}
    fixed_files = []
    
    # Find all JSON files
    all_json_files = []
    for base_dir in base_dirs:
        base_path = Path(base_dir)
        if base_path.exists():
            all_json_files.extend(sorted(base_path.rglob("*.json")))
    
    print(f"Found {len(all_json_files)} JSON files to process\n")
    
    for i, json_file in enumerate(all_json_files, 1):
        filepath = str(json_file)
        status, message = process_file(filepath)
        stats[status] += 1
        
        if status == "FIXED":
            print(f"[{i:3d}]  FIXED: {json_file.name:40s} {message}")
            fixed_files.append(filepath)
        elif status == "FAILED":
            print(f"[{i:3d}]  FAILED: {json_file.name:40s} {message}")
        elif status == "SKIP":
            pass  # Don't print skipped files, too many
    
    # Summary
    print("\n" + "="*80)
    print("SUMMARY")
    print("="*80)
    print(f"Fixed:     {stats['FIXED']:4d} files")
    print(f"OK:        {stats['OK']:4d} files (already normalized)")
    print(f"Skipped:   {stats['SKIP']:4d} files")
    print(f"Failed:    {stats['FAILED']:4d} files")
    print(f"Total:     {sum(stats.values()):4d} files")
    print("="*80)
    
    if fixed_files:
        print(f"\nFiles modified:")
        for f in fixed_files[:10]:
            print(f"  - {f}")
        if len(fixed_files) > 10:
            print(f"  ... and {len(fixed_files) - 10} more")
    
    if stats['FAILED'] > 0:
        print(f"\n WARNING: {stats['FAILED']} files failed to process")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(scan_and_process())
