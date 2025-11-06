#!/usr/bin/env python3
"""
Reorganize components so each .src/.hdr file lives with its matching .json file
Principle: {component_name}.json, {component_name}.src, {component_name}.hdr all in same directory
"""

import json
import shutil
from pathlib import Path
from typing import Dict, List, Tuple

def find_orphaned_files() -> Tuple[List[Path], List[Path]]:
    """Find all .src and .hdr files that don't have a matching .json in same directory."""
    components_root = Path("config/components")
    
    orphaned_src = []
    orphaned_hdr = []
    
    # Find all .src files
    for src_file in components_root.rglob("*.src"):
        json_file = src_file.with_suffix(".json")
        if not json_file.exists():
            orphaned_src.append(src_file)
    
    # Find all .hdr files
    for hdr_file in components_root.rglob("*.hdr"):
        json_file = hdr_file.with_suffix(".json")
        if not json_file.exists():
            orphaned_hdr.append(hdr_file)
    
    return orphaned_src, orphaned_hdr

def find_json_files() -> Dict[str, Path]:
    """Map component names to their .json file paths."""
    components_root = Path("config/components")
    json_map = {}
    
    for json_file in components_root.rglob("*.json"):
        name = json_file.stem
        if name not in json_map:
            json_map[name] = json_file
        else:
            print(f"  DUPLICATE: {name}.json exists at multiple locations:")
            print(f"   {json_map[name]}")
            print(f"   {json_file}")
    
    return json_map

def colocate_files(dry_run=True) -> Dict[str, int]:
    """Move .src and .hdr files to be colocated with their .json files."""
    
    json_map = find_json_files()
    results = {
        "moved_src": 0,
        "moved_hdr": 0,
        "errors": 0,
        "already_colocated": 0
    }
    
    components_root = Path("config/components")
    
    # Process all .src files
    for src_file in components_root.rglob("*.src"):
        component_name = src_file.stem
        
        if component_name in json_map:
            json_file = json_map[component_name]
            target_src = json_file.parent / src_file.name
            
            if target_src == src_file:
                results["already_colocated"] += 1
            else:
                try:
                    if not dry_run:
                        target_src.parent.mkdir(parents=True, exist_ok=True)
                        shutil.move(str(src_file), str(target_src))
                    print(f" MOVE: {src_file.name}")
                    print(f"        {src_file.parent.relative_to(components_root)}/")
                    print(f"         {json_file.parent.relative_to(components_root)}/")
                    results["moved_src"] += 1
                except Exception as e:
                    print(f" ERROR moving {src_file}: {e}")
                    results["errors"] += 1
    
    # Process all .hdr files
    for hdr_file in components_root.rglob("*.hdr"):
        component_name = hdr_file.stem
        
        if component_name in json_map:
            json_file = json_map[component_name]
            target_hdr = json_file.parent / hdr_file.name
            
            if target_hdr == hdr_file:
                results["already_colocated"] += 1
            else:
                try:
                    if not dry_run:
                        target_hdr.parent.mkdir(parents=True, exist_ok=True)
                        shutil.move(str(hdr_file), str(target_hdr))
                    print(f" MOVE: {hdr_file.name}")
                    print(f"        {hdr_file.parent.relative_to(components_root)}/")
                    print(f"         {json_file.parent.relative_to(components_root)}/")
                    results["moved_hdr"] += 1
                except Exception as e:
                    print(f" ERROR moving {hdr_file}: {e}")
                    results["errors"] += 1
    
    return results

if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(
        description="Colocate .src and .hdr files with their corresponding .json files"
    )
    parser.add_argument("--dry-run", action="store_true", default=True,
                       help="Show what would be moved without actually moving (default: True)")
    parser.add_argument("--execute", action="store_true",
                       help="Actually perform the moves (use --execute to run)")
    
    args = parser.parse_args()
    
    print("================================================================================")
    print("COMPONENT FILE COLOCATION - DRY RUN")
    print("================================================================================\n")
    
    orphaned_src, orphaned_hdr = find_orphaned_files()
    print(f"Found {len(orphaned_src)} orphaned .src files")
    print(f"Found {len(orphaned_hdr)} orphaned .hdr files\n")
    
    dry_run = not args.execute
    results = colocate_files(dry_run=dry_run)
    
    print("\n================================================================================")
    print("RESULTS")
    print("================================================================================")
    print(f"Moved .src files:     {results['moved_src']}")
    print(f"Moved .hdr files:     {results['moved_hdr']}")
    print(f"Already colocated:    {results['already_colocated']}")
    print(f"Errors:               {results['errors']}")
    
    if dry_run:
        print("\n  DRY RUN MODE - No files were actually moved")
        print("Run with --execute to perform the actual moves")
    else:
        print("\n EXECUTION COMPLETE - Files have been reorganized")

