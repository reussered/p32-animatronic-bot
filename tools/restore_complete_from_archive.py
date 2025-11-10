#!/usr/bin/env python3
"""
Restore Complete Component Definitions from Archive
Replaces current versions with archived versions where archived is longer/more complete.
"""

import json
import shutil
from pathlib import Path

def restore_from_archive():
    """Copy archived components back to current location."""
    
    print("\n" + "="*80)
    print("RESTORE COMPLETE COMPONENTS FROM ARCHIVE")
    print("="*80 + "\n")
    
    archived_dir = Path("config/bots/.archived_components/bot_families")
    current_dir = Path("config/bots/bot_families")
    
    restored_count = 0
    skipped_count = 0
    
    # Find all archived components
    for archived_file in sorted(archived_dir.rglob("*.json")):
        component_name = archived_file.stem
        
        # Find current version
        current_files = list(current_dir.rglob(f"{component_name}.json"))
        
        if not current_files:
            print(f" {component_name:40s} - Not found in current, skipping")
            skipped_count += 1
            continue
        
        current_file = current_files[0]
        
        # Get file sizes
        archived_size = archived_file.stat().st_size
        current_size = current_file.stat().st_size
        
        # Only restore if archived is larger (more complete)
        if archived_size > current_size:
            try:
                # Read archived
                with open(archived_file, 'r', encoding='utf-8-sig') as f:
                    data = json.load(f)
                
                # Preserve current path/name
                data['relative_filename'] = str(current_file.relative_to(Path(".")))
                data['name'] = current_file.stem
                
                # Write back to current location
                with open(current_file, 'w', encoding='utf-8') as f:
                    json.dump(data, f, indent=4)
                
                diff = archived_size - current_size
                print(f" RESTORED: {component_name:40s} +{diff:4d}B")
                restored_count += 1
                
            except Exception as e:
                print(f" FAILED:   {component_name:40s} - {str(e)[:40]}")
        else:
            skipped_count += 1
    
    print("\n" + "="*80)
    print(f"Restored: {restored_count} components")
    print(f"Skipped:  {skipped_count} components (current was complete or equal)")
    print("="*80 + "\n")
    
    return 0

if __name__ == "__main__":
    import sys
    sys.exit(restore_from_archive())
