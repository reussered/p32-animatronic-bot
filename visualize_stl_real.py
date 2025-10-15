#!/usr/bin/env python3
"""
ASCII STL Reader and Visualizer - Generate actual images of STL geometry
"""

import os
import re
import matplotlib
matplotlib.use('Agg')  # Use non-interactive backend
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import numpy as np

def read_ascii_stl(file_path):
    """Read ASCII STL file and extract triangles"""
    print(f"Reading ASCII STL: {os.path.basename(file_path)}")
    
    triangles = []
    
    try:
        with open(file_path, 'r') as f:
            content = f.read()
        
        # Use regex to find all facets
        facet_pattern = r'facet normal\s+([-\d\.e\+\-]+)\s+([-\d\.e\+\-]+)\s+([-\d\.e\+\-]+).*?endfacet'
        vertex_pattern = r'vertex\s+([-\d\.e\+\-]+)\s+([-\d\.e\+\-]+)\s+([-\d\.e\+\-]+)'
        
        facets = re.findall(facet_pattern, content, re.DOTALL)
        
        for facet in content.split('facet normal')[1:]:  # Skip first empty split
            if 'endfacet' not in facet:
                continue
                
            # Extract vertices from this facet
            vertices = re.findall(vertex_pattern, facet)
            
            if len(vertices) == 3:
                triangle = []
                for vertex_coords in vertices:
                    try:
                        x, y, z = map(float, vertex_coords)
                        triangle.append([x, y, z])
                    except ValueError:
                        break
                
                if len(triangle) == 3:
                    triangles.append(triangle)
        
        print(f"Extracted {len(triangles)} triangles")
        return np.array(triangles)
        
    except Exception as e:
        print(f"Error reading ASCII STL: {e}")
        return np.array([])

def create_stl_visualization(stl_file, output_file, max_triangles=2000):
    """Create comprehensive STL visualization"""
    
    triangles = read_ascii_stl(stl_file)
    
    if len(triangles) == 0:
        print(f"No valid triangles found in {stl_file}")
        return False
    
    # Limit triangles for performance
    if len(triangles) > max_triangles:
        print(f"Limiting to {max_triangles} triangles for visualization")
        # Sample triangles evenly
        indices = np.linspace(0, len(triangles)-1, max_triangles, dtype=int)
        triangles = triangles[indices]
    
    # Calculate geometry bounds
    all_points = triangles.reshape(-1, 3)
    min_bounds = np.min(all_points, axis=0)
    max_bounds = np.max(all_points, axis=0)
    center = (min_bounds + max_bounds) / 2
    size = max_bounds - min_bounds
    max_size = np.max(size)
    
    print(f"Geometry bounds: X({min_bounds[0]:.2f} to {max_bounds[0]:.2f}), Y({min_bounds[1]:.2f} to {max_bounds[1]:.2f}), Z({min_bounds[2]:.2f} to {max_bounds[2]:.2f})")
    
    # Create figure with multiple views
    fig = plt.figure(figsize=(16, 12))
    fig.suptitle(f'STL Geometry: {os.path.basename(stl_file)}\n{len(triangles)} triangles displayed', fontsize=14)
    
    # Define viewing angles
    views = [
        (30, 45, "3D Perspective"),
        (0, 0, "Front View (XZ)"),
        (0, 90, "Side View (YZ)"), 
        (90, 0, "Top View (XY)")
    ]
    
    for i, (elev, azim, title) in enumerate(views):
        ax = fig.add_subplot(2, 2, i+1, projection='3d')
        
        # Create mesh collection
        poly3d = []
        for triangle in triangles:
            poly3d.append(triangle)
        
        # Add collection to plot
        collection = Poly3DCollection(poly3d, alpha=0.7, edgecolor='black', linewidths=0.1)
        collection.set_facecolor('lightblue')
        ax.add_collection3d(collection)
        
        # Set axis limits
        margin = max_size * 0.1
        ax.set_xlim(center[0] - max_size/2 - margin, center[0] + max_size/2 + margin)
        ax.set_ylim(center[1] - max_size/2 - margin, center[1] + max_size/2 + margin)
        ax.set_zlim(center[2] - max_size/2 - margin, center[2] + max_size/2 + margin)
        
        # Set viewing angle
        ax.view_init(elev=elev, azim=azim)
        
        # Labels
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.set_zlabel('Z')
        ax.set_title(title)
    
    # Save with high DPI
    plt.tight_layout()
    plt.savefig(output_file, dpi=200, bbox_inches='tight')
    plt.close()
    
    print(f"Saved visualization: {output_file}")
    return True

def main():
    """Generate visualizations for all STL files"""
    
    stl_files = [
        ("assets/shapes/stl/green_goblin_skull.stl", "goblin_skull_REAL.png"),
        ("assets/shapes/stl/display_basic_mount.stl", "display_mount_REAL.png"),
        ("assets/shapes/stl/goblin_nose_sensor.stl", "nose_sensor_REAL.png"),
        ("assets/shapes/stl/speaker_basic_mount.stl", "speaker_mount_REAL.png")
    ]
    
    # Create output directory
    output_dir = "assets/shapes/images"
    os.makedirs(output_dir, exist_ok=True)
    
    print("=== STL Visualization Generation ===")
    
    success_count = 0
    
    for stl_file, output_name in stl_files:
        if os.path.exists(stl_file):
            output_path = os.path.join(output_dir, output_name)
            
            if create_stl_visualization(stl_file, output_path):
                success_count += 1
            print("-" * 50)
        else:
            print(f"STL file not found: {stl_file}")
    
    print(f"=== Complete: {success_count}/{len(stl_files)} visualizations created ===")

if __name__ == "__main__":
    main()