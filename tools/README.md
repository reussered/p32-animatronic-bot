# P32 Animatronic Bot - Tools

This directory contains development and visualization tools for the P32 Animatronic Bot project.

## Documentation Generation (NEW)

### Assembly PDF Generator
- `generate-assembly-pdf.ps1` - **Creates professional PDF assembly guides from markdown**

**Quick Start:**
```powershell
# First time: Install Pandoc
.\tools\generate-assembly-pdf.ps1 -InstallPandoc

# Generate all PDFs
.\tools\generate-assembly-pdf.ps1
```

**Output**: `docs\assembly_guides\*.pdf`

**Features:**
- Professional styling with table of contents
- Print-optimized layout (letter size, proper margins)
- Syntax highlighting for code blocks
- ASCII diagrams preserved correctly
- Page numbers and document headers/footers

**Complete Documentation**: See `docs\PDF-GENERATION-WORKFLOW.md`

---

## STL Viewer

A web-based 3D viewer for validating generated STL shapes.

### Files
- `stl-viewer.html` - Web-based STL viewer with Three.js
- `launch-stl-viewer.ps1` - PowerShell launcher script

### Features
- **Interactive 3D Viewing**: Rotate, pan, and zoom STL models
- **Shape Analysis**: Displays triangle count, dimensions, file size
- **Wireframe Mode**: Toggle between solid and wireframe rendering  
- **Auto-fit Camera**: Automatically frames the loaded model
- **Grid and Axes**: Visual reference for scale and orientation
- **Modern UI**: Clean interface with animatronic-themed styling

### Usage

#### Quick Start
```powershell
.\tools\launch-stl-viewer.ps1
```

#### With Specific STL File
```powershell  
.\tools\launch-stl-viewer.ps1 assets/shapes/stl/goblin_eye_left.stl
```

### Controls
- **Left Click + Drag**: Rotate the 3D model
- **Right Click + Drag**: Pan the view
- **Mouse Wheel**: Zoom in/out
- **Reset View Button**: Return to auto-fit camera position
- **Wireframe Button**: Toggle rendering mode

### Integration with Shape Generation
1. Generate STL from OpenSCAD: `openscad -o output.stl input.scad`
2. Launch viewer: `.\tools\launch-stl-viewer.ps1 output.stl`
3. Validate shape geometry and dimensions
4. Iterate on design parameters as needed