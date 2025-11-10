#!/usr/bin/env python3
"""
Fix use_fields declarations in all .src files
Change from extern to static declarations with values from corresponding .json
"""

import json
from pathlib import Path
import re

def get_use_fields(json_path: str) -> dict:
    """Get use_fields from corresponding JSON file."""
    try:
        with open(json_path, 'r', encoding='utf-8-sig') as f:
            data = json.load(f)
            return data.get('use_fields', {})
    except:
        return {}

def generate_static_declarations(use_fields: dict) -> str:
    """Generate static variable declarations from use_fields."""
    if not use_fields:
        return ""
    
    lines = ["// use_fields - from JSON, declared static (file-scoped)"]
    
    for name, value in use_fields.items():
        if isinstance(value, str):
            lines.append(f'static const char* {name} = "{value}";')
        elif isinstance(value, int):
            lines.append(f'static const uint32_t {name} = {value};')
        elif isinstance(value, float):
            lines.append(f'static const float {name} = {value}f;')
        elif isinstance(value, bool):
            val = "true" if value else "false"
            lines.append(f'static const bool {name} = {val};')
    
    return "\n".join(lines)

def fix_src_file(src_path: str) -> tuple:
    """Fix .src file to use static instead of extern for use_fields."""
    # Find corresponding JSON
    json_path = src_path.replace('.src', '.json')
    
    if not Path(json_path).exists():
        return ("SKIP", f"No corresponding JSON: {json_path}")
    
    # Get use_fields
    use_fields = get_use_fields(json_path)
    if not use_fields:
        return ("OK", "No use_fields to declare")
    
    # Read .src file
    with open(src_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Check if already has static declarations
    if "static const" in content and ("display_width" in content or "color_schema" in content):
        return ("OK", "Already using static declarations")
    
    # Check if has extern declarations
    if not "extern" in content:
        return ("SKIP", "No extern declarations to replace")
    
    # Remove extern declarations
    # Pattern: extern type name;
    pattern = r'\nextern\s+(const\s+)?\w+\s+\w+\s*;'
    new_content = re.sub(pattern, '', content)
    
    if new_content == content:
        return ("SKIP", "No extern declarations matched pattern")
    
    # Add static declarations after includes
    include_pattern = r'(#include.*?\n(?:#include.*?\n)*)'
    static_decls = "\n// Use fields - declared static (file-scoped)\n"
    static_decls += generate_static_declarations(use_fields) + "\n"
    
    new_content = re.sub(include_pattern, r'\1' + static_decls, new_content, count=1)
    
    # Write back
    with open(src_path, 'w', encoding='utf-8') as f:
        f.write(new_content)
    
    return ("FIXED", f"Converted {len(use_fields)} use_fields to static")

def main():
    print("\n" + "="*80)
    print("FIX use_fields DECLARATIONS IN .src FILES")
    print("="*80)
    print("Converting: extern type name; -> static const type name = value;\n")
    
    src_files = list(Path("config").rglob("*.src"))
    print(f"Found {len(src_files)} .src files\n")
    
    stats = {"FIXED": 0, "OK": 0, "SKIP": 0}
    
    for src_file in sorted(src_files):
        status, message = fix_src_file(str(src_file))
        stats[status] += 1
        
        if status == "FIXED":
            print(f" FIXED: {src_file.name:50s} {message}")
        elif status == "OK":
            pass  # Don't print OK files
    
    print("\n" + "="*80)
    print("SUMMARY")
    print("="*80)
    print(f"Fixed:  {stats['FIXED']} files")
    print(f"OK:     {stats['OK']} files (already correct)")
    print(f"Skip:   {stats['SKIP']} files")
    print("="*80 + "\n")
    
    return 0

if __name__ == "__main__":
    import sys
    sys.exit(main())
