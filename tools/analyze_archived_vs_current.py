#!/usr/bin/env python3
"""
Archive vs Current Component Analysis & Merge
Compares archived_components vs bot_families versions.
Keeps the longer/more complete version (data integrity first).
"""

import json
import os
from pathlib import Path
from typing import Dict, List, Tuple

def get_file_info(filepath: str) -> Tuple[int, float]:
    """Get file size and modification time."""
    try:
        stat = os.stat(filepath)
        return stat.st_size, stat.st_mtime
    except:
        return 0, 0

def compare_component_files(name: str) -> Dict:
    """Compare archived vs current version of a component."""
    archived_path = f"config/bots/.archived_components/bot_families/{name}.json"
    current_path = f"config/bots/bot_families/{name}.json"
    
    # Find actual paths (might be nested)
    archived_files = list(Path("config/bots/.archived_components").rglob(f"{name}.json"))
    current_files = list(Path("config/bots/bot_families").rglob(f"{name}.json"))
    
    result = {
        "name": name,
        "archived_path": None,
        "current_path": None,
        "archived_size": 0,
        "current_size": 0,
        "archived_date": 0,
        "current_date": 0,
        "winner": None,
        "reason": None
    }
    
    if archived_files:
        result["archived_path"] = str(archived_files[0])
        result["archived_size"], result["archived_date"] = get_file_info(result["archived_path"])
    
    if current_files:
        result["current_path"] = str(current_files[0])
        result["current_size"], result["current_date"] = get_file_info(result["current_path"])
    
    # Determine winner (longer = more complete data)
    if result["archived_size"] > result["current_size"]:
        result["winner"] = "archived"
        result["reason"] = f"Archived is {result['archived_size'] - result['current_size']} bytes longer"
    elif result["current_size"] > result["archived_size"]:
        result["winner"] = "current"
        result["reason"] = f"Current is {result['current_size'] - result['archived_size']} bytes longer"
    else:
        result["winner"] = "equal"
        result["reason"] = "Same size"
    
    return result

def merge_components(archived_path: str, current_path: str) -> Dict:
    """Merge archived and current, keeping longer/more complete."""
    try:
        with open(archived_path, 'r', encoding='utf-8-sig') as f:
            archived = json.load(f)
    except:
        archived = {}
    
    try:
        with open(current_path, 'r', encoding='utf-8') as f:
            current = json.load(f)
    except:
        current = {}
    
    # If archived is longer, it has more data - use it but preserve current name/path
    if len(str(archived)) > len(str(current)):
        merged = archived
        merged['name'] = current.get('name', archived.get('name'))
        merged['relative_filename'] = current.get('relative_filename', archived.get('relative_filename'))
        return merged
    else:
        return current

def main():
    print("\n" + "="*80)
    print("ARCHIVE vs CURRENT COMPONENT ANALYSIS")
    print("="*80 + "\n")
    
    # Find all archived components
    archived_dir = Path("config/bots/.archived_components")
    if not archived_dir.exists():
        print("✗ No archived components directory found")
        return 1
    
    archived_files = list(archived_dir.rglob("*.json"))
    current_dir = Path("config/bots/bot_families")
    current_files = list(current_dir.rglob("*.json"))
    
    print(f"Archived components: {len(archived_files)}")
    print(f"Current components:  {len(current_files)}\n")
    
    # Get unique names
    archived_names = {f.stem for f in archived_files}
    current_names = {f.stem for f in current_files}
    
    overlapping = archived_names & current_names
    archived_only = archived_names - current_names
    current_only = current_names - archived_names
    
    print(f"Overlapping components (both versions): {len(overlapping)}")
    print(f"Archived-only: {len(archived_only)}")
    print(f"Current-only:  {len(current_only)}\n")
    
    # Analyze overlapping
    print("="*80)
    print("OVERLAPPING COMPONENTS - Size Comparison")
    print("="*80 + "\n")
    
    archived_longer = 0
    current_longer = 0
    equal = 0
    
    comparisons = []
    for name in sorted(overlapping):
        info = compare_component_files(name)
        comparisons.append(info)
        
        if info["winner"] == "archived":
            archived_longer += 1
        elif info["winner"] == "current":
            current_longer += 1
        else:
            equal += 1
    
    # Show top 15 by size difference
    comparisons_sorted = sorted(
        comparisons,
        key=lambda x: abs(x["archived_size"] - x["current_size"]),
        reverse=True
    )
    
    for i, info in enumerate(comparisons_sorted[:15], 1):
        winner_marker = "A" if info["winner"] == "archived" else "C" if info["winner"] == "current" else "="
        print(f"[{winner_marker}] {info['name']:40s} A:{info['archived_size']:6d}B C:{info['current_size']:6d}B  {info['reason']}")
    
    if len(comparisons_sorted) > 15:
        print(f"... and {len(comparisons_sorted) - 15} more")
    
    print("\n" + "="*80)
    print("SUMMARY")
    print("="*80)
    print(f"Archived version is longer: {archived_longer:3d} components")
    print(f"Current version is longer:  {current_longer:3d} components")
    print(f"Same size:                  {equal:3d} components")
    print(f"Total overlapping:          {len(comparisons):3d} components")
    print("="*80 + "\n")
    
    # Recommendation
    print("RECOMMENDATION:")
    if archived_longer > current_longer:
        print(f"✓ ARCHIVED components are MORE COMPLETE ({archived_longer} vs {current_longer})")
        print("  Action: Keep archived versions as primary source")
    elif current_longer > archived_longer:
        print(f"✓ CURRENT components are MORE COMPLETE ({current_longer} vs {archived_longer})")
        print("  Action: Keep current versions as primary source")
    else:
        print("✓ Both versions are equally complete")
        print("  Action: Keep current versions (more recent)")
    
    return 0

if __name__ == "__main__":
    import sys
    sys.exit(main())
