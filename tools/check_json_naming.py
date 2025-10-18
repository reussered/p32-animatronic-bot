#!/usr/bin/env python3
"""
JSON Filename/Component_ID Correspondence Checker
Ensures JSON filenames match their internal component_id fields
"""

import json
import os
import sys
from pathlib import Path

def check_json_naming(config_dir):
    """Check that JSON filenames match their component_id fields"""
    mismatches = []
    
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
                
                # Check if they match
                if component_id != filename_stem:
                    mismatches.append({
                        'file': json_file,
                        'filename_stem': filename_stem,
                        'component_id': component_id,
                        'suggested_filename': f"{component_id}.json"
                    })
                    
        except Exception as e:
            print(f"Error reading {json_file}: {e}")
    
    return mismatches

def main():
    config_dir = Path(__file__).parent.parent / "config"
    
    print("Checking JSON filename/component_id correspondence...")
    print(f"Scanning: {config_dir}")
    print()
    
    mismatches = check_json_naming(config_dir)
    
    if not mismatches:
        print("✅ All JSON files have matching filenames and component_ids!")
        return 0
    
    print(f"❌ Found {len(mismatches)} mismatches:")
    print()
    
    for mismatch in mismatches:
        print(f"File: {mismatch['file']}")
        print(f"  Filename: {mismatch['filename_stem']}.json")
        print(f"  Component ID: {mismatch['component_id']}")
        print(f"  Should be: {mismatch['suggested_filename']}")
        print()
    
    return len(mismatches)

if __name__ == "__main__":
    sys.exit(main())