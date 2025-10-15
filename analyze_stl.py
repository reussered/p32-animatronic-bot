#!/usr/bin/env python3
"""
Simple STL Header Reader - Quick analysis of STL files
"""

import os
import struct

def analyze_stl_file(stl_path):
    """Analyze STL file and show basic geometry info"""
    
    if not os.path.exists(stl_path):
        print(f"File not found: {stl_path}")
        return
        
    file_size = os.path.getsize(stl_path)
    print(f"\nAnalyzing: {os.path.basename(stl_path)}")
    print(f"File size: {file_size:,} bytes")
    
    # Try to read as binary STL
    try:
        with open(stl_path, 'rb') as f:
            # Read header (80 bytes)
            header = f.read(80)
            print(f"Header: {header.decode('ascii', errors='ignore')[:50]}...")
            
            # Read triangle count
            triangle_count_bytes = f.read(4)
            triangle_count = struct.unpack('<I', triangle_count_bytes)[0]
            print(f"Triangle count: {triangle_count:,}")
            
            # Calculate expected file size
            expected_size = 80 + 4 + (triangle_count * 50)  # header + count + triangles
            print(f"Expected size: {expected_size:,} bytes")
            
            if file_size == expected_size:
                print("✓ Valid binary STL format")
            else:
                print("⚠ Size mismatch - may be ASCII STL")
                
            # Read first triangle to show actual geometry
            if triangle_count > 0:
                # Normal vector (3 floats)
                normal = struct.unpack('<fff', f.read(12))
                print(f"First triangle normal: ({normal[0]:.3f}, {normal[1]:.3f}, {normal[2]:.3f})")
                
                # Three vertices (9 floats)
                vertices = []
                for i in range(3):
                    vertex = struct.unpack('<fff', f.read(12))
                    vertices.append(vertex)
                    print(f"  Vertex {i+1}: ({vertex[0]:.3f}, {vertex[1]:.3f}, {vertex[2]:.3f})")
                
                # Show geometry bounds
                all_coords = [coord for vertex in vertices for coord in vertex]
                min_coord = min(all_coords)
                max_coord = max(all_coords)
                print(f"Sample coordinate range: {min_coord:.3f} to {max_coord:.3f}")
                
    except Exception as e:
        print(f"Error reading STL: {e}")
        
        # Try ASCII format
        try:
            with open(stl_path, 'r') as f:
                first_line = f.readline().strip()
                print(f"First line (ASCII attempt): {first_line}")
                
                if first_line.lower().startswith('solid'):
                    print("✓ Appears to be ASCII STL format")
                    
                    # Count facets
                    f.seek(0)
                    facet_count = 0
                    for line in f:
                        if 'facet normal' in line.lower():
                            facet_count += 1
                    print(f"Facet count (ASCII): {facet_count:,}")
                    
        except Exception as e2:
            print(f"Error reading as ASCII: {e2}")

def main():
    """Analyze all STL files"""
    stl_files = [
        "assets/shapes/stl/green_goblin_skull.stl",
        "assets/shapes/stl/display_basic_mount.stl", 
        "assets/shapes/stl/goblin_nose_sensor.stl",
        "assets/shapes/stl/speaker_basic_mount.stl"
    ]
    
    print("=== STL File Analysis ===")
    
    for stl_file in stl_files:
        analyze_stl_file(stl_file)
    
    print("\n=== Analysis Complete ===")

if __name__ == "__main__":
    main()