#!/usr/bin/env python3
"""
Unicode to ASCII Converter for p32-animatronic-bot
Replaces all Unicode characters with ASCII equivalents across the entire codebase.
"""

import os
import re
import glob
import json
from pathlib import Path

# Unicode to ASCII mapping for common characters
UNICODE_TO_ASCII = {
    # Arrows
    '->': '->',
    '<-': '<-',
    '^': '^',
    'v': 'v',
    
    # Checkmarks and crosses
    'OK': 'OK',
    'OK': 'OK',
    'ERROR': 'ERROR',
    'ERROR': 'ERROR',
    
    # Degree symbols  
    ' deg': ' deg',
    
    # Mathematical symbols
    'PI': 'PI',
    'x': 'x',
    '+/-': '+/-',
    
    # Emojis
    '[ROBOT]': '[ROBOT]',
    '[CHECKLIST]': '[CHECKLIST]',
    '[TOOL]': '[TOOL]',
    '[SIGNAL]': '[SIGNAL]',
    '[RULER]': '[RULER]',
    '[TARGET]': '[TARGET]',
    '[HAPPY]': '[HAPPY]',
    '[EYE]': '[EYE]',
    '[ROCKET]': '[ROCKET]',
    
    # Box drawing characters
    '+': '+',
    '+': '+',
    '+': '+',
    '+': '+',
    '|': '|',
    '=': '=',
    '+': '+',
    '+': '+',
    '+': '+',
    '+': '+',
    '+': '+',
    '+': '+',
    '-': '-',
    '|': '|',
    
    # Special characters
    'Ohm': 'Ohm',
    'micro': 'micro',
    '/': '/',
    '*': '*',
    '*': '*',
    '*': '*',
    '#': '#',
    '#': '#',
    
    # BOM and other special Unicode
    '\ufeff': '',  # BOM
    '\u200d': '',  # Zero-width joiner
    '': '',        # Variation selector
    
    # More emojis
    '[WIZARD]': '[WIZARD]',
    '[EYES]': '[EYES]',
    '[MALE]': '[MALE]',
    '[SPEAKER]': '[SPEAKER]',
    '[PAUSE]': '[PAUSE]',
    
    # Miscellaneous
    'WARNING': 'WARNING',
}

def replace_unicode_in_text(text):
    """Replace Unicode characters with ASCII equivalents."""
    result = text
    for unicode_char, ascii_equiv in UNICODE_TO_ASCII.items():
        result = result.replace(unicode_char, ascii_equiv)
    
    # Check for any remaining non-ASCII characters
    remaining_unicode = re.findall(r'[^\x00-\x7F]', result)
    if remaining_unicode:
        print(f"Warning: Remaining Unicode characters: {set(remaining_unicode)}")
        # Replace any remaining Unicode with placeholder
        result = re.sub(r'[^\x00-\x7F]', '?', result)
    
    return result

def process_file(file_path):
    """Process a single file to replace Unicode characters."""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            original_content = f.read()
        
        # Skip if no Unicode characters
        if not re.search(r'[^\x00-\x7F]', original_content):
            return False
        
        new_content = replace_unicode_in_text(original_content)
        
        if new_content != original_content:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(new_content)
            return True
        
        return False
    except Exception as e:
        print(f"Error processing {file_path}: {e}")
        return False

def main():
    """Main function to process all files."""
    # Define file extensions to process
    extensions = ['*.json', '*.py', '*.cpp', '*.hpp', '*.h', '*.c', '*.js', '*.ts', '*.ps1', '*.bat', '*.sh']
    
    root_dir = Path('f:/GitHub/p32-animatronic-bot')
    os.chdir(root_dir)
    
    modified_files = []
    
    print("Starting Unicode to ASCII conversion...")
    
    for ext in extensions:
        pattern = f"**/{ext}"
        files = glob.glob(pattern, recursive=True)
        
        for file_path in files:
            # Skip certain directories
            if any(skip_dir in file_path for skip_dir in ['.git', '.venv', '__pycache__', '.pio']):
                continue
                
            if process_file(file_path):
                modified_files.append(file_path)
                print(f"Modified: {file_path}")
    
    print(f"\nConversion complete! Modified {len(modified_files)} files.")
    
    if modified_files:
        print("\nModified files:")
        for file_path in modified_files:
            print(f"  - {file_path}")

if __name__ == "__main__":
    main()