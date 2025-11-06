#!/usr/bin/env python3
"""
fix_component_structure.py

Walks the entire config tree and validates/fixes component references in the
components:[] arrays of JSON files. 

For each referenced file:
1. Check if it exists at the specified path
2. If not, search for the basename in the project
3. If found elsewhere, update the path
4. If not found anywhere, add to bad_components list

This script creates a master index of all JSON files in the project for lookup.
"""

import json
import os
import sys
from pathlib import Path
from typing import Dict, List, Set, Tuple

class ComponentStructureFixer:
    def __init__(self, project_root: str = "."):
        self.project_root = Path(project_root)
        self.config_root = self.project_root / "config"
        
        # Master file index: maps basename -> list of full paths
        self.file_index: Dict[str, List[Path]] = {}
        
        # Track changes and issues
        self.files_processed = 0
        self.references_fixed = 0
        self.bad_components: List[Dict] = []
        self.fixed_references: List[Dict] = []
        
        print("[*] Initializing ComponentStructureFixer")
        self._build_file_index()
    
    def _build_file_index(self):
        """Build master index of all JSON files in config/"""
        print("[*] Building master file index...")
        
        for json_file in self.config_root.rglob("*.json"):
            basename = json_file.name
            rel_path = json_file.relative_to(self.config_root)
            
            if basename not in self.file_index:
                self.file_index[basename] = []
            self.file_index[basename].append(rel_path)
        
        print(f"[+] Indexed {len(self.file_index)} unique JSON basenames")
        
        # Show duplicates (multiple files with same name in different locations)
        duplicates = {k: v for k, v in self.file_index.items() if len(v) > 1}
        if duplicates:
            print(f"[!] Found {len(duplicates)} basenames with multiple locations:")
            for basename, paths in sorted(duplicates.items()):
                print(f"    {basename}:")
                for path in paths:
                    print(f"      - {path}")
    
    def _resolve_reference(self, reference: str, current_file: Path) -> Tuple[bool, str, str]:
        """
        Try to resolve a component reference.
        
        Returns: (was_fixed, original_path, resolved_path)
        If already valid: (False, reference, reference)
        If fixed: (True, reference, new_path)
        If not found: (False, reference, "NOT_FOUND")
        """
        # Normalize the reference path
        ref_path = Path(reference)
        
        # Check if reference exists as-is
        full_path = self.config_root / ref_path
        if full_path.exists():
            return (False, reference, reference)
        
        # Extract basename and search for it
        basename = ref_path.name
        
        if basename not in self.file_index:
            return (False, reference, "NOT_FOUND")
        
        # Get list of locations where this file exists
        locations = self.file_index[basename]
        
        if len(locations) == 1:
            # Found exactly one location - use it
            new_path = str(locations[0]).replace("\\", "/")
            return (True, reference, new_path)
        else:
            # Multiple locations - need to disambiguate
            # Prefer the one that's closest in path structure to current file
            # For now, return first match but mark as ambiguous
            new_path = str(locations[0]).replace("\\", "/")
            print(f"    [!] AMBIGUOUS: {basename} found in {len(locations)} locations, using first: {new_path}")
            return (True, reference, new_path)
    
    def process_json_file(self, json_path: Path):
        """Process a single JSON file to fix component references."""
        self.files_processed += 1
        
        try:
            with open(json_path, 'r', encoding='ascii') as f:
                content = f.read()
            
            data = json.loads(content)
        except Exception as e:
            print(f"[!] ERROR reading {json_path}: {e}")
            return
        
        if "components" not in data or not isinstance(data["components"], list):
            return
        
        original_components = data["components"].copy()
        modified = False
        
        for i, component_ref in enumerate(data["components"]):
            if not isinstance(component_ref, str):
                continue
            
            was_fixed, original, resolved = self._resolve_reference(
                component_ref,
                json_path
            )
            
            if was_fixed:
                data["components"][i] = resolved
                modified = True
                self.references_fixed += 1
                self.fixed_references.append({
                    "file": str(json_path.relative_to(self.project_root)).replace("\\", "/"),
                    "original": original,
                    "resolved": resolved
                })
                print(f"  [*] Fixed: {original} -> {resolved}")
            
            elif resolved == "NOT_FOUND":
                self.bad_components.append({
                    "file": str(json_path.relative_to(self.project_root)).replace("\\", "/"),
                    "component": component_ref,
                    "reason": "File not found anywhere in project"
                })
                print(f"  [!] BAD: {component_ref} (not found anywhere)")
        
        # Write back if modified
        if modified:
            try:
                with open(json_path, 'w', encoding='ascii') as f:
                    json.dump(data, f, indent=4)
                print(f"[+] UPDATED: {json_path.relative_to(self.project_root)}")
            except Exception as e:
                print(f"[!] ERROR writing {json_path}: {e}")
    
    def run(self):
        """Process all JSON files in config/"""
        print("\n[*] Processing JSON files...")
        
        json_files = list(self.config_root.rglob("*.json"))
        print(f"[*] Found {len(json_files)} JSON files to process")
        
        for json_file in sorted(json_files):
            rel_path = json_file.relative_to(self.project_root)
            print(f"\n[*] {rel_path}")
            self.process_json_file(json_file)
        
        self.print_report()
    
    def print_report(self):
        """Print summary report."""
        print("\n" + "="*80)
        print("COMPONENT STRUCTURE FIX REPORT")
        print("="*80)
        
        print(f"\nFiles Processed: {self.files_processed}")
        print(f"References Fixed: {self.references_fixed}")
        print(f"Bad Components Found: {len(self.bad_components)}")
        
        if self.fixed_references:
            print("\n" + "-"*80)
            print("FIXED REFERENCES:")
            print("-"*80)
            for fix in self.fixed_references:
                print(f"\nFile: {fix['file']}")
                print(f"  {fix['original']}")
                print(f"  -> (fixed to)")
                print(f"  {fix['resolved']}")
        
        if self.bad_components:
            print("\n" + "-"*80)
            print("BAD COMPONENTS (NOT FOUND):")
            print("-"*80)
            for bad in self.bad_components:
                print(f"\nFile: {bad['file']}")
                print(f"  Component: {bad['component']}")
                print(f"  Reason: {bad['reason']}")
        
        print("\n" + "="*80)
        
        # Save report to file
        report_file = self.project_root / "component_fix_report.json"
        report_data = {
            "summary": {
                "files_processed": self.files_processed,
                "references_fixed": self.references_fixed,
                "bad_components_count": len(self.bad_components)
            },
            "fixed_references": self.fixed_references,
            "bad_components": self.bad_components
        }
        
        try:
            with open(report_file, 'w', encoding='ascii') as f:
                json.dump(report_data, f, indent=2)
            print(f"\n[+] Report saved to: {report_file}")
        except Exception as e:
            print(f"\n[!] ERROR saving report: {e}")


def main():
    project_root = "."
    
    # Allow custom project root as argument
    if len(sys.argv) > 1:
        project_root = sys.argv[1]
    
    fixer = ComponentStructureFixer(project_root)
    fixer.run()


if __name__ == "__main__":
    main()
