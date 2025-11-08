#!/usr/bin/env python3
"""
Systematic restoration of component JSON files from commit 018e3d2.
Restores missing 'components' arrays and 'use_fields' sections.
"""

import json
import subprocess
import os
import sys
from pathlib import Path

BASE_COMMIT = "018e3d2"
REPO_ROOT = Path.cwd()

class ComponentRestorer:
    def __init__(self):
        self.restored_count = 0
        self.failed_count = 0
        self.unchanged_count = 0
        self.report = []
        
    def get_original_json(self, filepath):
        """Get JSON content from original commit."""
        try:
            result = subprocess.run(
                ['git', 'show', f'{BASE_COMMIT}:{filepath}'],
                capture_output=True,
                text=True,
                check=False
            )
            if result.returncode == 0:
                return json.loads(result.stdout)
            else:
                return None
        except Exception as e:
            print(f"Error reading {filepath} from git: {e}")
            return None
    
    def read_current_json(self, filepath):
        """Read current JSON file."""
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                return json.loads(f.read())
        except Exception as e:
            print(f"Error reading {filepath}: {e}")
            return None
    
    def write_json_file(self, filepath, data):
        """Write JSON file with proper formatting."""
        try:
            with open(filepath, 'w', encoding='utf-8') as f:
                json.dump(data, f, indent=4)
            return True
        except Exception as e:
            print(f"Error writing {filepath}: {e}")
            return False
    
    def restore_file(self, filepath):
        """Restore components and use_fields for a single file."""
        filepath_str = str(filepath)
        
        # Skip non-component files
        if not filepath_str.endswith('.json'):
            return
        
        original = self.get_original_json(filepath_str)
        current = self.read_current_json(filepath_str)
        
        if not current:
            self.report.append(f"SKIP: Cannot parse current JSON: {filepath_str}")
            return
        
        if not original:
            self.report.append(f"SKIP: File not in original commit: {filepath_str}")
            return
        
        original_components = original.get('components', [])
        original_use_fields = original.get('use_fields', {})
        
        current_components = current.get('components', [])
        current_use_fields = current.get('use_fields', {})
        
        # Check if restoration is needed
        components_match = original_components == current_components
        use_fields_match = original_use_fields == current_use_fields
        
        if components_match and use_fields_match:
            self.unchanged_count += 1
            self.report.append(f"OK: {filepath_str}")
            return
        
        # Apply restoration
        changes = []
        
        if not components_match:
            changes.append(f"components: {current_components} -> {original_components}")
            current['components'] = original_components
        
        if not use_fields_match and original_use_fields:
            changes.append(f"use_fields: {len(current_use_fields)} -> {len(original_use_fields)} fields")
            current['use_fields'] = original_use_fields
        
        # Write restored file
        if self.write_json_file(filepath_str, current):
            self.restored_count += 1
            self.report.append(f"RESTORED: {filepath_str}")
            for change in changes:
                self.report.append(f"  - {change}")
        else:
            self.failed_count += 1
            self.report.append(f"FAILED: {filepath_str}")
    
    def restore_directory(self, directory):
        """Recursively restore all JSON files in directory."""
        dir_path = Path(directory)
        
        if not dir_path.exists():
            print(f"ERROR: Directory does not exist: {directory}")
            return
        
        json_files = sorted(dir_path.glob('**/*.json'))
        print(f"\nFound {len(json_files)} JSON files to process...")
        
        for i, filepath in enumerate(json_files, 1):
            print(f"[{i}/{len(json_files)}] Processing: {filepath.name}...", end='', flush=True)
            self.restore_file(filepath)
            print(" Done")
        
        self.print_report()
    
    def print_report(self):
        """Print restoration report."""
        print("\n" + "="*70)
        print("RESTORATION REPORT")
        print("="*70)
        print(f"\nRestored:  {self.restored_count}")
        print(f"Unchanged: {self.unchanged_count}")
        print(f"Failed:    {self.failed_count}")
        print(f"Total:     {self.restored_count + self.unchanged_count + self.failed_count}")
        
        print("\n" + "="*70)
        print("DETAILED LOG:")
        print("="*70)
        for line in self.report:
            print(line)
        
        print("\n" + "="*70)
        print("SUMMARY")
        print("="*70)
        
        if self.failed_count > 0:
            print(f"[!] {self.failed_count} files failed to restore")
        else:
            print("[OK] All files processed successfully")
        
        if self.restored_count > 0:
            print(f"[OK] {self.restored_count} files were restored")
        else:
            print("[INFO] No files needed restoration")

def main():
    # Define directories to restore
    restore_paths = [
        'config/bots/bot_families/goblins',
        'config/components',
    ]
    
    restorer = ComponentRestorer()
    
    print("\n" + "="*70)
    print("COMPONENT JSON RESTORATION TOOL")
    print("="*70)
    print(f"Base commit: {BASE_COMMIT}")
    print(f"Restoring from: {restore_paths}")
    print("="*70)
    
    for path in restore_paths:
        if os.path.isdir(path):
            print(f"\nProcessing directory: {path}")
            restorer.restore_directory(path)
    
    return 0 if restorer.failed_count == 0 else 1

if __name__ == '__main__':
    sys.exit(main())
