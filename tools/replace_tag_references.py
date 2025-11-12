#!/usr/bin/env python3
"""Replace TAG references with string literals in component .src files."""

import json
import re
from pathlib import Path

def main():
    registry_path = Path("config/component_registry.json")
    
    with open(registry_path, 'r', encoding='utf-8') as f:
        registry_data = json.load(f)
    
    components = registry_data.get('components', [])
    
    # Pattern to match ESP_LOG calls with TAG variables
    tag_pattern = re.compile(r'ESP_LOG([IEDWV])\((TAG[_\w]*),')
    
    processed = 0
    modified = 0
    
    for entry in components:
        name = entry.get('name', '')
        json_path = entry.get('path', '')
        if not json_path or not name:
            continue
            
        # Get .src path from .json path
        src_path = Path(json_path.replace('.json', '.src'))
        
        if not src_path.exists():
            continue
            
        processed += 1
        
        # Read file
        try:
            with open(src_path, 'r', encoding='ascii', newline='') as f:
                content = f.read()
        except Exception as e:
            print(f"Error reading {src_path}: {e}")
            continue
        
        # Check if TAG references exist
        matches = tag_pattern.findall(content)
        if not matches:
            continue
        
        # Replace TAG references with component name string literal
        new_content = tag_pattern.sub(rf'ESP_LOG\1("{name}",', content)
        
        # Write back
        try:
            with open(src_path, 'w', encoding='ascii', newline='') as f:
                f.write(new_content)
            modified += 1
            print(f"Replaced TAG refs in: {src_path.name}")
        except Exception as e:
            print(f"Error writing {src_path}: {e}")
    
    print(f"\nProcessed {processed} component .src files")
    print(f"Modified {modified} files")

if __name__ == '__main__':
    main()
