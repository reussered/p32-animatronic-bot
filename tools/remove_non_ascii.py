#!/usr/bin/env python3
"""
Remove all non-ASCII characters from a file, replacing with ASCII equivalents.
Usage: python remove_non_ascii.py <file_path>
"""
import sys
import re
from pathlib import Path

# Mapping of common non-ASCII characters to ASCII replacements
REPLACEMENTS = {
    '\u274c': '[ERROR]',  # X mark
    '\u2705': '[OK]',     # Checkmark
    '\U0001f4dd': '[NOTE]',  # Memo/notepad
    '\U0001f50d': '[SEARCH]',  # Magnifying glass
    '\U0001f527': '[FIX]',  # Wrench
    '\U0001f40d': '[FIX]',  # Snake
    '\U0001f6a7': '[MAINTENANCE]',  # Construction
}

def remove_non_ascii(file_path):
    """Remove all non-ASCII characters from a file."""
    file_path = Path(file_path)
    
    if not file_path.exists():
        print(f"ERROR: File not found: {file_path}")
        return False
    
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f"ERROR: Could not read file: {e}")
        return False
    
    original_length = len(content)
    
    # First, replace known emoji/special characters with ASCII equivalents
    for char, replacement in REPLACEMENTS.items():
        if char in content:
            print(f"  Replacing '{char}' with '{replacement}'")
            content = content.replace(char, replacement)
    
    # Then, remove any remaining non-ASCII characters
    # Keep only ASCII printable characters (32-126) plus whitespace (9, 10, 13)
    cleaned_content = ''.join(char for char in content if ord(char) in range(32, 127) or char in '\t\n\r')
    
    if cleaned_content != content:
        print(f"  Removed additional non-ASCII characters")
        content = cleaned_content
    
    if len(content) != original_length:
        print(f"[CHANGE] File size changed: {original_length} -> {len(content)} bytes")
    else:
        print(f"[OK] File is already ASCII-compliant")
        return False
    
    try:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"[SAVE] Successfully cleaned {file_path}")
        return True
    except Exception as e:
        print(f"ERROR: Could not write file: {e}")
        return False

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python remove_non_ascii.py <file_path> [file_path2] ...")
        sys.exit(1)
    
    for file_path in sys.argv[1:]:
        print(f"Processing {file_path}...")
        remove_non_ascii(file_path)
        print()
