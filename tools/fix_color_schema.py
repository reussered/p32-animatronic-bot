#!/usr/bin/env python3
"""Remove duplicate Pixel_RGB888 definition from color_schema.hpp"""

from pathlib import Path

def main():
    file_path = Path("config/shared_headers/color_schema.hpp")
    
    with open(file_path, 'r', encoding='ascii') as f:
        lines = f.readlines()
    
    # Find the two struct Pixel_RGB888 definitions
    rgb888_starts = []
    for i, line in enumerate(lines):
        if line.strip() == 'struct Pixel_RGB888':
            rgb888_starts.append(i)
    
    if len(rgb888_starts) != 2:
        print(f"Error: Found {len(rgb888_starts)} Pixel_RGB888 definitions, expected 2")
        return
    
    print(f"Found Pixel_RGB888 at lines: {rgb888_starts[0]+1}, {rgb888_starts[1]+1}")
    
    # Find the end of the second definition (look for }; followed by empty line and comment)
    second_start = rgb888_starts[1]
    end_idx = None
    
    for i in range(second_start, len(lines)):
        if lines[i].strip() == '};':
            # Check if next non-empty line starts with /**
            for j in range(i+1, min(i+5, len(lines))):
                if lines[j].strip():
                    if lines[j].strip().startswith('/**'):
                        end_idx = i + 1  # Include the }; line
                        break
                    break
            if end_idx:
                break
    
    if not end_idx:
        print("Error: Could not find end of second Pixel_RGB888 definition")
        return
    
    print(f"Second definition ends at line {end_idx}")
    print(f"Removing lines {second_start-7} to {end_idx} (includes comment block)")
    
    # Remove from the comment block before struct to the closing brace
    # Find the comment block start (/** before struct)
    comment_start = second_start
    for i in range(second_start - 1, max(0, second_start - 20), -1):
        if lines[i].strip().startswith('/**'):
            comment_start = i
            break
    
    # Remove the duplicate definition including its comment block
    new_lines = lines[:comment_start] + lines[end_idx:]
    
    # Write back
    with open(file_path, 'w', encoding='ascii', newline='') as f:
        f.writelines(new_lines)
    
    print(f"Removed duplicate Pixel_RGB888 definition")
    print(f"Removed {end_idx - comment_start} lines")

if __name__ == '__main__':
    main()
