#!/usr/bin/env python3
"""
STL Visualization Script
Generates actual images of STL geometry using matplotlib and numpy-stl
"""

import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.patches import Polygon
import os
import sys

def read_stl_ascii(file_path):
    """Read ASCII STL file and extract triangles"""
    triangles = []
    
    try:
        with open(file_path, 'r') as f:
            lines = f.readlines()
        
        current_triangle = []
        reading_triangle = False
        
        for line in lines:
            line = line.strip().lower()
            
            if line.startswith('facet normal'):
                reading_triangle = True
                current_triangle = []
            elif line.startswith('vertex') and reading_triangle:
                coords = line.split()[1:4]
                vertex = [float(coord) for coord in coords]
                current_triangle.append(vertex)
            elif line.startswith('endfacet'):
                if len(current_triangle) == 3:
                    triangles.append(current_triangle)
                reading_triangle = False
                
    except Exception as e:
        print(f"Error reading STL file: {e}")
        return []
    
    return np.array(triangles)

def read_stl_binary(file_path):
    """Read binary STL file"""
    try:
        with open(file_path, 'rb') as f:
            # Skip header
            f.read(80)
            
            # Read number of triangles
            triangle_count = int.from_bytes(f.read(4), byteorder='little')
            print(f"Triangle count: {triangle_count}")
            
            triangles = []
            
            for i in range(triangle_count):
                # Skip normal vector (12 bytes)
                f.read(12)
                
                # Read 3 vertices (9 floats = 36 bytes)
                triangle = []
                for j in range(3):
                    vertex = []
                    for k in range(3):
                        coord_bytes = f.read(4)
                        coord = np.frombuffer(coord_bytes, dtype=np.float32)[0]
                        vertex.append(coord)
                    triangle.append(vertex)
                triangles.append(triangle)
                
                # Skip attribute byte count
                f.read(2)
                
            return np.array(triangles)
            
    except Exception as e:
        print(f"Error reading binary STL: {e}")
        return []

def determine_stl_format(file_path):
    """Determine if STL is ASCII or binary"""
    try:
        with open(file_path, 'rb') as f:
            header = f.read(80)
            
        # Try to decode as ASCII
        try:
            header_text = header.decode('ascii').lower()
            if 'solid' in header_text:
                return 'ascii'
        except:
            pass
            
        return 'binary'
        
    except Exception as e:
        print(f"Error determining STL format: {e}")
        return 'binary'

def visualize_stl(stl_file, output_image):
    """Create 3D visualization of STL file"""
    
    print(f"Processing: {stl_file}")
    
    # Determine file format and read triangles
    format_type = determine_stl_format(stl_file)
    print(f"STL format detected: {format_type}")
    
    if format_type == 'ascii':
        triangles = read_stl_ascii(stl_file)
    else:
        triangles = read_stl_binary(stl_file)
    
    if len(triangles) == 0:
        print("No triangles found in STL file!")
        return False
    
    print(f"Loaded {len(triangles)} triangles")
    
    # Create figure with subplots
    fig = plt.figure(figsize=(15, 10))
    fig.suptitle(f'STL Visualization: {os.path.basename(stl_file)}', fontsize=16)
    
    # Multiple viewing angles
    angles = [
        (30, 45, "3/4 View"),
        (0, 0, "Front View"), 
        (0, 90, "Side View"),
        (90, 0, "Top View")
    ]
    
    for idx, (elev, azim, title) in enumerate(angles):
        ax = fig.add_subplot(2, 2, idx+1, projection='3d')
        
        # Plot triangles
        for triangle in triangles[:min(len(triangles), 1000)]:  # Limit for performance
            # Create triangle patch
            tri_array = np.array(triangle)
            
            # Plot triangle edges
            ax.plot([tri_array[0,0], tri_array[1,0]], 
                   [tri_array[0,1], tri_array[1,1]], 
                   [tri_array[0,2], tri_array[1,2]], 'b-', alpha=0.6, linewidth=0.5)
            ax.plot([tri_array[1,0], tri_array[2,0]], 
                   [tri_array[1,1], tri_array[2,1]], 
                   [tri_array[1,2], tri_array[2,2]], 'b-', alpha=0.6, linewidth=0.5)
            ax.plot([tri_array[2,0], tri_array[0,0]], 
                   [tri_array[2,1], tri_array[0,1]], 
                   [tri_array[2,2], tri_array[0,2]], 'b-', alpha=0.6, linewidth=0.5)
        
        # Set viewing angle
        ax.view_init(elev=elev, azim=azim)
        
        # Set equal aspect ratio
        all_points = triangles.reshape(-1, 3)
        max_range = np.ptp(all_points, axis=0).max() / 2.0
        mid_x = np.mean(all_points[:, 0])
        mid_y = np.mean(all_points[:, 1]) 
        mid_z = np.mean(all_points[:, 2])
        
        ax.set_xlim(mid_x - max_range, mid_x + max_range)
        ax.set_ylim(mid_y - max_range, mid_y + max_range)
        ax.set_zlim(mid_z - max_range, mid_z + max_range)
        
        ax.set_title(title)
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.set_zlabel('Z')
    
    plt.tight_layout()
    plt.savefig(output_image, dpi=150, bbox_inches='tight')
    plt.close()
    
    print(f"Saved visualization: {output_image}")
    return True

def main():
    """Main function to process STL files"""
    
    # STL files to visualize
    stl_files = [
        "assets/shapes/stl/green_goblin_skull.stl",
        "assets/shapes/stl/display_basic_mount.stl", 
        "assets/shapes/stl/goblin_nose_sensor.stl",
        "assets/shapes/stl/speaker_basic_mount.stl"
    ]
    
    # Create output directory
    output_dir = "assets/shapes/images"
    os.makedirs(output_dir, exist_ok=True)
    
    success_count = 0
    
    for stl_file in stl_files:
        if os.path.exists(stl_file):
            base_name = os.path.splitext(os.path.basename(stl_file))[0]
            output_image = os.path.join(output_dir, f"{base_name}_visualization.png")
            
            if visualize_stl(stl_file, output_image):
                success_count += 1
        else:
            print(f"STL file not found: {stl_file}")
    
    print(f"\nVisualization complete: {success_count}/{len(stl_files)} files processed")

if __name__ == "__main__":
    main()