#!/usr/bin/env python3
"""Remove Unicode characters from camera documentation files."""

import sys
import re

def replace_unicode(text):
    """Replace Unicode with ASCII equivalents."""
    # Replace various dashes and hyphens
    text = text.replace('-', '-')  # en-dash
    text = text.replace('-', '-')  # em-dash
    
    # Replace degree symbols
    text = text.replace('', 'deg')
    
    # Replace multiplication signs
    text = text.replace('', 'x')
    
    # Replace plus/minus
    text = text.replace('', '+/-')
    
    # Replace middle dot / center dot
    text = text.replace('', '.')
    
    # Replace superscript/subscript with plain text
    text = text.replace('', '2')
    text = text.replace('', '3')
    
    # Replace fractions if any
    text = text.replace('', '1/2')
    text = text.replace('', '1/4')
    text = text.replace('', '3/4')
    
    # Replace special quotes
    text = text.replace('"', '"')
    text = text.replace('"', '"')
    text = text.replace(''', "'")
    text = text.replace(''', "'")
    
    # Replace other Unicode symbols
    text = text.replace('', 'TM')
    text = text.replace('', '(c)')
    text = text.replace('', '(R)')
    
    # Replace arrows
    text = text.replace('', '->')
    text = text.replace('', '<-')
    text = text.replace('', '=>')
    text = text.replace('', '-')
    
    # Replace checkmarks and X marks and stars
    text = text.replace('', 'OK')
    text = text.replace('', 'OKOK')
    text = text.replace('', 'OK')
    text = text.replace('', 'NO')
    text = text.replace('', 'WARNING')
    text = text.replace('', 'WARNING')
    text = text.replace('', 'STAR')
    
    # Replace micro symbol and infinity
    text = text.replace('', 'u')
    text = text.replace('', 'infinity')
    
    # Replace em-dash alternatives (already done above but be thorough)
    text = text.replace('-', '-')
    
    # Replace Omega/special letters
    text = text.replace('', 'Ohm')
    text = text.replace('', 'ohm')
    
    # Replace other common Unicode
    text = text.replace('', '*')
    text = text.replace('', '[BLOCK]')
    text = text.replace('', '[BOX]')
    
    return text

def process_file(filepath):
    """Process a single file."""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_length = len(content)
        cleaned = replace_unicode(content)
        cleaned_length = len(cleaned)
        
        if original_length != cleaned_length:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(cleaned)
            print(f"Cleaned {filepath}: {original_length} -> {cleaned_length} chars")
            return True
        else:
            print(f"No changes needed for {filepath}")
            return False
    except Exception as e:
        print(f"Error processing {filepath}: {e}", file=sys.stderr)
        return False

if __name__ == '__main__':
    files = [
        'f:\\GitHub\\p32-animatronic-bot\\CAMERA_RESEARCH_FINDINGS.md',
        'f:\\GitHub\\p32-animatronic-bot\\CAMERA_COST_ANALYSIS.md',
        'f:\\GitHub\\p32-animatronic-bot\\CAMERA_SYSTEM_ANALYSIS.md',
        'f:\\GitHub\\p32-animatronic-bot\\CAMERA_DECISION_QUICK_REFERENCE.md',
        'f:\\GitHub\\p32-animatronic-bot\\CAMERA_COST_RELIABILITY_ANALYSIS.md',
    ]
    
    count = 0
    for filepath in files:
        if process_file(filepath):
            count += 1
    
    print(f"\nProcessed {count} files with changes")
    sys.exit(0)
