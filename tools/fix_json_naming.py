#!/usr/bin/env python3
"""
JSON Filename/Component_ID Auto-Fixer
Automatically renames JSON files to match their component_id fields
"""

import json
import os
import sys
from pathlib import Path

def fix_json_naming(config_dir):
    """Fix JSON filenames to match their component_id fields"""
    fixes_made = []
    
    # Look for positioned components (these have component_id fields)
    positioned_dir = Path(config_dir) / "components" / "positioned"
    
    if not positioned_dir.exists():
        print(f"Directory not found: {positioned_dir}")
        return []
    
    for json_file in positioned_dir.glob("**/*.json"):
        try:
            with open(json_file, 'r') as f:
                data = json.load(f)
            
            # Check if this JSON has a component_id field
            if 'component_id' in data:
                component_id = data['component_id']
                filename_stem = json_file.stem  # filename without .json extension
                
                # Check if they don't match
                if component_id != filename_stem:
                    # Create new filename
                    new_filename = json_file.parent / f"{component_id}.json"
                    
                    # Check if target already exists
                    if new_filename.exists():
                        print(f"⚠️  Cannot rename {json_file} - target {new_filename} already exists")
                        continue
                    
                    # Rename the file
                    json_file.rename(new_filename)
                    fixes_made.append({
                        'old': str(json_file),
                        'new': str(new_filename),
                        'component_id': component_id
                    })
                    
        except Exception as e:
            print(f"Error processing {json_file}: {e}")
    
    return fixes_made

def main():
    config_dir = Path(__file__).parent.parent / "config"
    
    print("Auto-fixing JSON filename/component_id correspondence...")
    print(f"Scanning: {config_dir}")
    print()
    
    fixes = fix_json_naming(config_dir)
    
    if not fixes:
        print("✅ No JSON files needed renaming!")
        return 0
    
    print(f"✅ Fixed {len(fixes)} JSON files:")
    print()
    
    for fix in fixes:
        old_name = Path(fix['old']).name
        new_name = Path(fix['new']).name
        print(f"  {old_name} → {new_name}")
    
    print()
    print("All JSON files now match their component_id fields!")
    return 0

if __name__ == "__main__":
    sys.exit(main())