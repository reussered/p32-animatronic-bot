#!/usr/bin/env python3
"""
Complete Component Validation & Correction Suite
Fixes all common component definition issues:
1. Missing required fields (name, type, description)
2. Invalid components arrays (non-strings, duplicates)
3. Missing software field (init_function, act_function)
4. Missing timing field
5. Inconsistent formatting
"""

import json
import sys
from pathlib import Path
from typing import Dict, List, Optional, Tuple

def validate_component(data: Dict) -> Tuple[bool, List[str]]:
    """Validate component definition. Returns (is_valid, list_of_issues)."""
    issues = []
    
    # Required fields
    if 'name' not in data:
        issues.append("missing: 'name'")
    
    if 'type' not in data:
        issues.append("missing: 'type'")
    
    if 'description' not in data:
        issues.append("missing: 'description'")
    
    # Check components array
    components = data.get('components', [])
    if not isinstance(components, list):
        issues.append("invalid: 'components' is not a list")
    else:
        # Check each component is a string
        for i, comp in enumerate(components):
            if not isinstance(comp, str):
                issues.append(f"invalid: components[{i}] = {type(comp).__name__} (should be str)")
        
        # Check for duplicates
        if len(components) != len(set(components)):
            dups = [c for c in set(components) if components.count(c) > 1]
            issues.append(f"duplicates in components: {dups}")
    
    # Check software field for leaf components
    if data.get('type') in ['HARDWARE', 'DRIVER', 'SENSOR', 'INTERFACE']:
        if 'software' not in data:
            issues.append("missing: 'software' (required for leaf components)")
        else:
            software = data['software']
            if 'init_function' not in software:
                issues.append("missing: 'software.init_function'")
            if 'act_function' not in software:
                issues.append("missing: 'software.act_function'")
    
    # Check timing field
    if 'timing' not in data and data.get('type') not in ['SUBSYSTEM_COMPOSITE', 'ASSEMBLY']:
        issues.append("missing: 'timing' (recommended for all components)")
    
    # Check relative_filename for bot_family components
    if 'bot_families' in data.get('relative_filename', ''):
        if 'relative_filename' not in data:
            issues.append("missing: 'relative_filename' (required for bot_families)")
    
    return len(issues) == 0, issues

def correct_component(filepath: str, data: Dict) -> Tuple[bool, str]:
    """Apply automatic corrections to component. Returns (was_modified, message)."""
    modified = False
    
    # Auto-fill name from filename if missing
    if 'name' not in data:
        data['name'] = Path(filepath).stem
        modified = True
    
    # Auto-fill type if missing (guess from context)
    if 'type' not in data:
        if 'components' in data and isinstance(data['components'], list):
            data['type'] = 'ASSEMBLY'  # Has child components
        else:
            data['type'] = 'COMPONENT'
        modified = True
    
    # Auto-fill description if missing
    if 'description' not in data:
        data['description'] = f"Auto-generated component: {data.get('name', 'unknown')}"
        modified = True
    
    # Fix components array - ensure all are strings
    components = data.get('components', [])
    if isinstance(components, list):
        cleaned = []
        had_issues = False
        for comp in components:
            if isinstance(comp, str):
                cleaned.append(comp)
            elif isinstance(comp, dict) and 'name' in comp:
                cleaned.append(comp['name'])
                had_issues = True
            else:
                had_issues = True
        
        if had_issues:
            data['components'] = cleaned
            modified = True
        
        # Remove duplicates
        if len(cleaned) != len(set(cleaned)):
            data['components'] = list(dict.fromkeys(cleaned))  # Remove dups, preserve order
            modified = True
    
    # Add software field if missing for leaf components
    if data.get('type') in ['HARDWARE', 'DRIVER', 'SENSOR', 'INTERFACE']:
        if 'software' not in data:
            data['software'] = {
                'init_function': f"{data['name']}_init",
                'act_function': f"{data['name']}_act"
            }
            modified = True
    
    # Add timing if missing
    if 'timing' not in data and data.get('type') not in ['SUBSYSTEM_COMPOSITE', 'ASSEMBLY']:
        data['timing'] = {'hitCount': 1}
        modified = True
    
    return modified, "Auto-corrected" if modified else "Already correct"

def process_file(filepath: str) -> Tuple[str, str, int]:
    """Process single JSON file. Returns (status, message, modifications)."""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        # Validate
        is_valid, issues = validate_component(data)
        
        if is_valid:
            return ("OK", "Valid", 0)
        
        # Attempt correction
        modified, msg = correct_component(filepath, data)
        
        if modified:
            # Revalidate after correction
            is_valid_after, remaining_issues = validate_component(data)
            
            if is_valid_after:
                # Write corrected file
                with open(filepath, 'w', encoding='utf-8') as f:
                    json.dump(data, f, indent=4)
                return ("FIXED", f"Corrected: {', '.join(issues[:2])}", len(issues))
            else:
                return ("PARTIAL", f"Partial fix: {', '.join(remaining_issues[:2])}", len(remaining_issues))
        else:
            return ("UNFIXABLE", ", ".join(issues[:2]), len(issues))
    
    except json.JSONDecodeError as e:
        return ("ERROR", f"Invalid JSON: {str(e)[:50]}", 1)
    except Exception as e:
        return ("ERROR", f"Exception: {str(e)[:50]}", 1)

def main():
    print("\n" + "="*80)
    print("COMPONENT VALIDATION & CORRECTION SUITE")
    print("="*80)
    print("Validating and correcting all component definitions...")
    print("="*80 + "\n")
    
    stats = {"OK": 0, "FIXED": 0, "PARTIAL": 0, "UNFIXABLE": 0, "ERROR": 0}
    detailed_issues = {"FIXED": [], "PARTIAL": [], "UNFIXABLE": [], "ERROR": []}
    
    # Find all JSON files
    json_files = []
    for base_dir in ["config/bots", "config/components"]:
        base_path = Path(base_dir)
        if base_path.exists():
            json_files.extend(sorted(base_path.rglob("*.json")))
    
    print(f"Found {len(json_files)} JSON files\n")
    
    for i, json_file in enumerate(json_files, 1):
        filepath = str(json_file)
        status, message, issue_count = process_file(filepath)
        stats[status] += 1
        
        if status != "OK":
            detailed_issues[status].append((json_file.name, message))
            print(f"[{i:3d}] {status:10s} {json_file.name:40s} {message}")
    
    # Summary
    print("\n" + "="*80)
    print("SUMMARY")
    print("="*80)
    print(f"OK:         {stats['OK']:4d} files (valid, no changes needed)")
    print(f"FIXED:      {stats['FIXED']:4d} files (corrected and verified)")
    print(f"PARTIAL:    {stats['PARTIAL']:4d} files (partially corrected)")
    print(f"UNFIXABLE:  {stats['UNFIXABLE']:4d} files (manual review needed)")
    print(f"ERROR:      {stats['ERROR']:4d} files (parse errors)")
    print(f"Total:      {sum(stats.values()):4d} files")
    print("="*80)
    
    if stats['FIXED'] > 0:
        print(f"\n✓ {stats['FIXED']} components auto-corrected")
    
    if stats['PARTIAL'] > 0:
        print(f"\n⚠ {stats['PARTIAL']} components partially corrected (manual review):")
        for name, msg in detailed_issues['PARTIAL'][:5]:
            print(f"   - {name}: {msg}")
        if stats['PARTIAL'] > 5:
            print(f"   ... and {stats['PARTIAL'] - 5} more")
    
    if stats['UNFIXABLE'] > 0:
        print(f"\n✗ {stats['UNFIXABLE']} components cannot auto-fix (manual review):")
        for name, msg in detailed_issues['UNFIXABLE'][:5]:
            print(f"   - {name}: {msg}")
        if stats['UNFIXABLE'] > 5:
            print(f"   ... and {stats['UNFIXABLE'] - 5} more")
    
    if stats['ERROR'] > 0:
        print(f"\n✗ {stats['ERROR']} files have errors:")
        for name, msg in detailed_issues['ERROR'][:5]:
            print(f"   - {name}: {msg}")
        if stats['ERROR'] > 5:
            print(f"   ... and {stats['ERROR'] - 5} more")
    
    print("\n" + "="*80)
    
    # Return success if no unfixable or errors
    return 0 if (stats['UNFIXABLE'] == 0 and stats['ERROR'] == 0) else 1

if __name__ == "__main__":
    sys.exit(main())
