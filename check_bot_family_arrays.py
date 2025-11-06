#!/usr/bin/env python3
"""Check bot family JSONs for component references that need array conversion"""

import json
from pathlib import Path

PROJECT_ROOT = Path("f:/GitHub/p32-animatronic-bot")
BOT_FAMILIES = PROJECT_ROOT / "config" / "bots" / "bot_families"

def check_needs_fix(data, path=""):
    """Recursively check if data structure needs component array fixes"""
    needs_fix = []
    
    if isinstance(data, dict):
        # Check for 'component' field (singular) that should be 'components' array
        if 'component' in data and isinstance(data['component'], str):
            if data['component'].endswith('.json'):
                needs_fix.append(('component field', path, data['component']))
        
        # Recurse into nested structures
        for key, value in data.items():
            nested = check_needs_fix(value, f"{path}.{key}" if path else key)
            needs_fix.extend(nested)
    
    elif isinstance(data, list):
        for i, item in enumerate(data):
            nested = check_needs_fix(item, f"{path}[{i}]")
            needs_fix.extend(nested)
    
    return needs_fix

def main():
    all_files = list(BOT_FAMILIES.rglob("*.json"))
    files_with_issues = {}
    total_issues = 0
    
    print("\n" + "="*60)
    print("BOT FAMILY JSON COMPONENT ARRAY CHECK")
    print("="*60 + "\n")
    
    for json_file in all_files:
        try:
            content = json.loads(json_file.read_text(encoding='utf-8'))
            issues = check_needs_fix(content)
            
            if issues:
                rel_path = json_file.relative_to(PROJECT_ROOT)
                files_with_issues[str(rel_path)] = issues
                total_issues += len(issues)
        
        except Exception as e:
            print(f"Warning: Could not parse {json_file.name}: {e}")
    
    print(f"Files Checked: {len(all_files)}")
    print(f"Files with Issues: {len(files_with_issues)}")
    print(f"Total Issues Found: {total_issues}\n")
    
    if files_with_issues:
        print("="*60)
        print("FILES NEEDING COMPONENT ARRAY FIXES")
        print("="*60 + "\n")
        
        for file_path, issues in sorted(files_with_issues.items())[:20]:
            print(f"\n{file_path}")
            for issue_type, location, ref in issues[:3]:
                print(f"  [{issue_type}] {location}: {ref}")
            if len(issues) > 3:
                print(f"  ... and {len(issues) - 3} more")
    else:
        print(" All bot family JSONs use proper component arrays!")
    
    print("\n" + "="*60 + "\n")

if __name__ == "__main__":
    main()
