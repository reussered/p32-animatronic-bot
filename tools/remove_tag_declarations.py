#!/usr/bin/env python3
"""Remove all TAG declarations from component .src files in the registry."""

import json
import re
from pathlib import Path

def main():
    registry_path = Path("config/component_registry.json")
    
    with open(registry_path, 'r', encoding='utf-8') as f:
        registry_data = json.load(f)
    
    components = registry_data.get('components', [])
    tag_pattern = re.compile(r'^static const char\*?\s+\*?TAG[^\r\n]*;?\s*\r?\n', re.MULTILINE)
    
    processed = 0
    modified = 0
    
    for entry in components:
        json_path = entry.get('path', '')
        if not json_path:
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
        
        # Check if TAG exists
        if not tag_pattern.search(content):
            continue
        
        # Remove TAG declarations
        new_content = tag_pattern.sub('', content)
        
        # Write back
        try:
            with open(src_path, 'w', encoding='ascii', newline='') as f:
                f.write(new_content)
            modified += 1
            print(f"Removed TAG from: {src_path}")
        except Exception as e:
            print(f"Error writing {src_path}: {e}")
    
    print(f"\nProcessed {processed} component .src files")
    print(f"Modified {modified} files")

if __name__ == '__main__':
    main()
