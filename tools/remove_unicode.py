#!/usr/bin/env python3
"""
remove_unicode.py

Remove all Unicode characters from project files.
Replaces them with ASCII-safe equivalents.
"""

import os
import re
from pathlib import Path
from typing import Dict

class UnicodeRemover:
    def __init__(self, project_root: str = "."):
        self.project_root = Path(project_root)
        
        # Simple mapping: remove most Unicode, keep only ASCII
        self.files_processed = 0
        self.files_modified = 0
        self.total_replacements = 0
    
    def should_skip(self, path: Path) -> bool:
        """Check if we should skip this file."""
        skip_dirs = {'.git', '.venv', 'node_modules', '.pio', 'build', '__pycache__', '.pytest_cache'}
        skip_exts = {'.pyc', '.o', '.a', '.so', '.dll', '.exe', '.bin'}
        
        # Check if any parent directory is in skip list
        for part in path.parts:
            if part in skip_dirs:
                return True
        
        # Check file extension
        if path.suffix in skip_exts:
            return True
        
        return False
    
    def remove_non_ascii(self, content: str) -> tuple:
        """Remove non-ASCII characters, return (modified_content, count)."""
        original_len = len(content)
        
        # Remove all non-ASCII characters
        # Keep only: ASCII 32-126 (printable), 9 (tab), 10 (newline), 13 (carriage return)
        ascii_only = ''.join(
            c for c in content 
            if ord(c) in (9, 10, 13) or (32 <= ord(c) <= 126)
        )
        
        replacements = original_len - len(ascii_only)
        return ascii_only, replacements
    
    def process_file(self, file_path: Path):
        """Process a single file."""
        self.files_processed += 1
        
        if self.files_processed % 100 == 0:
            print(f"  Progress: {self.files_processed} files processed")
        
        try:
            # Read file
            with open(file_path, 'rb') as f:
                raw_bytes = f.read()
            
            # Try to decode as UTF-8
            try:
                content = raw_bytes.decode('utf-8')
            except UnicodeDecodeError:
                # Try ASCII
                try:
                    content = raw_bytes.decode('ascii')
                    return  # Already ASCII
                except UnicodeDecodeError:
                    # Skip binary files
                    return
            
            # Remove non-ASCII
            cleaned, replacements = self.remove_non_ascii(content)
            
            if replacements > 0:
                # Write back as ASCII
                with open(file_path, 'w', encoding='ascii') as f:
                    f.write(cleaned)
                
                self.files_modified += 1
                self.total_replacements += replacements
                
                rel_path = file_path.relative_to(self.project_root)
                print(f"  FIXED: {rel_path} ({replacements} chars removed)")
        
        except Exception as e:
            rel_path = file_path.relative_to(self.project_root)
            print(f"  ERROR processing {rel_path}: {e}")
    
    def run(self):
        """Process all files."""
        print("[*] Starting Unicode removal...")
        
        # Get all files
        all_files = list(self.project_root.rglob("*"))
        files_to_process = [f for f in all_files if f.is_file() and not self.should_skip(f)]
        
        print(f"[*] Found {len(files_to_process)} files to process")
        print()
        
        for file_path in sorted(files_to_process):
            self.process_file(file_path)
        
        self.print_report()
    
    def print_report(self):
        """Print summary report."""
        print()
        print("=" * 60)
        print("UNICODE REMOVAL REPORT")
        print("=" * 60)
        print(f"Files processed:     {self.files_processed}")
        print(f"Files modified:      {self.files_modified}")
        print(f"Total chars removed: {self.total_replacements}")
        print("=" * 60)


if __name__ == "__main__":
    import sys
    
    project_root = "."
    if len(sys.argv) > 1:
        project_root = sys.argv[1]
    
    remover = UnicodeRemover(project_root)
    remover.run()
