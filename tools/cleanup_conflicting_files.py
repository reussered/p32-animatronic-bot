#!/usr/bin/env python3
"""
P32 Component File Cleanup
Removes conflicting .c files when .cpp files exist
Ensures we follow RULE 5: use .cpp files for components
"""

import os
from pathlib import Path

def cleanup_conflicting_files():
    """Remove .c files that have corresponding .cpp files"""
    src_components = Path("src/components")
    
    if not src_components.exists():
        print("ERROR: src/components directory not found")
        return
    
    c_files = list(src_components.glob("*.c"))
    cpp_files = list(src_components.glob("*.cpp"))
    
    # Get base names for comparison
    cpp_basenames = {f.stem for f in cpp_files}
    
    conflicts_found = []
    
    for c_file in c_files:
        if c_file.stem in cpp_basenames:
            conflicts_found.append((c_file, src_components / f"{c_file.stem}.cpp"))
    
    if not conflicts_found:
        print("No conflicting files found.")
        return
    
    print(f"Found {len(conflicts_found)} conflicting files:")
    for c_file, cpp_file in conflicts_found:
        print(f"  CONFLICT: {c_file.name} vs {cpp_file.name}")
        
        # Check if the .c file is auto-generated placeholder
        try:
            with open(c_file, 'r') as f:
                c_content = f.read()
            
            with open(cpp_file, 'r') as f:
                cpp_content = f.read()
            
            # Auto-generated .c files usually have "Auto-generated" in first few lines
            c_is_autogen = "Auto-generated" in c_content[:200]
            cpp_is_autogen = "Auto-generated" in cpp_content[:200]
            
            if c_is_autogen and not cpp_is_autogen:
                print(f"    -> {c_file.name} is auto-generated placeholder, {cpp_file.name} has real implementation")
                print(f"    -> REMOVING {c_file.name} (preserving real implementation)")
                c_file.unlink()
            elif cpp_is_autogen and not c_is_autogen:
                print(f"    -> {cpp_file.name} is auto-generated placeholder, {c_file.name} has real implementation")
                print(f"    -> REMOVING {cpp_file.name} (preserving real implementation)")
                cpp_file.unlink()
            elif c_is_autogen and cpp_is_autogen:
                print(f"    -> Both are auto-generated. Keeping .cpp file (RULE 5)")
                c_file.unlink()
            else:
                print(f"    -> Both have real implementations! Manual review needed.")
                print(f"       .c file size: {len(c_content)} chars")
                print(f"       .cpp file size: {len(cpp_content)} chars")
        
        except Exception as e:
            print(f"    -> ERROR analyzing files: {e}")
    
    print("\\nCleanup complete!")

if __name__ == "__main__":
    cleanup_conflicting_files()