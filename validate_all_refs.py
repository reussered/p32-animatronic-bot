#!/usr/bin/env python3
"""Comprehensive Component Reference Validator
Checks ALL JSON files in the entire project for missing component references
"""

import json
import os
from pathlib import Path
from collections import defaultdict

PROJECT_ROOT = Path("f:/GitHub/p32-animatronic-bot")
CONFIG_DIR = PROJECT_ROOT / "config"

def main():
    print("\n" + "=" * 50)
    print("COMPREHENSIVE COMPONENT VALIDATION")
    print("=" * 50 + "\n")
    
    all_json_files = list(CONFIG_DIR.rglob("*.json"))
    total_references = 0
    missing_references = []
    checked_files = 0
    
    for json_file in all_json_files:
        checked_files += 1
        try:
            with open(json_file, 'r', encoding='utf-8-sig') as f:
                content = json.load(f)
            
            # Check 'components' array
            if 'components' in content and isinstance(content['components'], list):
                for comp_ref in content['components']:
                    total_references += 1
                    ref_path = Path(comp_ref)
                    if not ref_path.is_absolute():
                        ref_path = PROJECT_ROOT / comp_ref
                    
                    if not ref_path.exists():
                        missing_references.append({
                            'source': str(json_file.relative_to(PROJECT_ROOT)),
                            'missing': comp_ref,
                            'type': 'components[]'
                        })
            
            # Check 'component' field (singular)
            if 'component' in content:
                total_references += 1
                ref_path = Path(content['component'])
                if not ref_path.is_absolute():
                    ref_path = PROJECT_ROOT / content['component']
                
                if not ref_path.exists():
                    missing_references.append({
                        'source': str(json_file.relative_to(PROJECT_ROOT)),
                        'missing': content['component'],
                        'type': 'component'
                    })
            
            # Check body_sections
            if 'body_sections' in content and isinstance(content['body_sections'], list):
                for section in content['body_sections']:
                    # Check section.component
                    if 'component' in section:
                        total_references += 1
                        ref_path = Path(section['component'])
                        if not ref_path.is_absolute():
                            ref_path = PROJECT_ROOT / section['component']
                        
                        if not ref_path.exists():
                            missing_references.append({
                                'source': str(json_file.relative_to(PROJECT_ROOT)),
                                'missing': section['component'],
                                'type': 'body_sections.component'
                            })
                    
                    # Check section.components[]
                    if 'components' in section and isinstance(section['components'], list):
                        for comp_ref in section['components']:
                            total_references += 1
                            ref_path = Path(comp_ref)
                            if not ref_path.is_absolute():
                                ref_path = PROJECT_ROOT / comp_ref
                            
                            if not ref_path.exists():
                                missing_references.append({
                                    'source': str(json_file.relative_to(PROJECT_ROOT)),
                                    'missing': comp_ref,
                                    'type': 'body_sections.components[]'
                                })
        
        except Exception as e:
            print(f"Warning: Could not parse {json_file.name}: {e}")
    
    print(f"\n{'='*50}")
    print("VALIDATION RESULTS")
    print(f"{'='*50}\n")
    
    print(f"Files Checked:        {checked_files}")
    print(f"Total References:     {total_references}")
    print(f"Missing References:   {len(missing_references)}")
    
    if missing_references:
        print(f"\n{'='*50}")
        print("MISSING COMPONENTS")
        print(f"{'='*50}\n")
        
        # Group by missing component
        by_component = defaultdict(list)
        for ref in missing_references:
            by_component[ref['missing']].append(ref['source'])
        
        for missing_comp, sources in sorted(by_component.items()):
            print(f"\n{missing_comp}")
            print(f"  Referenced in {len(sources)} file(s):")
            for source in sources[:5]:  # Show first 5
                print(f"    - {source}")
            if len(sources) > 5:
                print(f"    ... and {len(sources) - 5} more")
        
        # Export to CSV
        csv_file = PROJECT_ROOT / "missing_components_full.csv"
        with open(csv_file, 'w', encoding='utf-8') as f:
            f.write("SourceFile,MissingComponent,Type\n")
            for ref in sorted(missing_references, key=lambda x: x['missing']):
                f.write(f'"{ref["source"]}","{ref["missing"]}","{ref["type"]}"\n')
        
        print(f"\nExported details to: missing_components_full.csv")
    else:
        print("\n All component references are valid!")
    
    print(f"\n{'='*50}\n")

if __name__ == "__main__":
    main()
