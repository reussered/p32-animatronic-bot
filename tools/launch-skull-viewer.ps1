# Skull Assembly Viewer Launcher
# Opens the interactive 3D skull assembly viewer

param(
    [string]$CreatureType = "goblin",
    [switch]$AutoGenerate,
    [switch]$Help
)

if ($Help) {
    Write-Host "P32 Skull Assembly Viewer Launcher" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "USAGE:" -ForegroundColor Yellow
    Write-Host "  .\launch-skull-viewer.ps1 [-CreatureType <type>] [-AutoGenerate] [-Help]"
    Write-Host ""
    Write-Host "PARAMETERS:" -ForegroundColor Yellow
    Write-Host "  -CreatureType    Specify creature to view (default: goblin)"
    Write-Host "                   Options: goblin, zombie, android, cat, lion, dragon_fire, dragon_ice, bear"
    Write-Host "  -AutoGenerate    Automatically generate STL files before viewing"
    Write-Host "  -Help           Show this help message"
    Write-Host ""
    Write-Host "EXAMPLES:" -ForegroundColor Green
    Write-Host "  .\launch-skull-viewer.ps1                          # View goblin assembly"
    Write-Host "  .\launch-skull-viewer.ps1 -CreatureType dragon_fire # View fire dragon"
    Write-Host "  .\launch-skull-viewer.ps1 -AutoGenerate            # Generate then view"
    Write-Host ""
    return
}

# Configuration
$ViewerFile = "tools\skull-assembly-viewer.html"
$ScadFile = "assets\shapes\scad\integrated_skull_assembly_system.scad"
$StlOutputDir = "assets\shapes\stl"

Write-Host "Starting P32 Skull Assembly Viewer..." -ForegroundColor Cyan
Write-Host "Creature Type: $CreatureType" -ForegroundColor Yellow

# Check if viewer file exists
if (-not (Test-Path $ViewerFile)) {
    Write-Host " Error: Viewer file not found: $ViewerFile" -ForegroundColor Red
    Write-Host "Make sure you're running this from the project root directory." -ForegroundColor Yellow
    return
}

# Auto-generate STL files if requested
if ($AutoGenerate) {
    Write-Host "Auto-generating STL files..." -ForegroundColor Yellow
    
    if (-not (Test-Path $ScadFile)) {
        Write-Host " Error: OpenSCAD file not found: $ScadFile" -ForegroundColor Red
        return
    }
    
    # Create output directory if it doesn't exist
    if (-not (Test-Path $StlOutputDir)) {
        New-Item -ItemType Directory -Path $StlOutputDir -Force | Out-Null
        Write-Host "Created STL output directory: $StlOutputDir" -ForegroundColor Green
    }
    
    # Generate STL for the specified creature type
    $StlFile = "$StlOutputDir\${CreatureType}_skull_assembly.stl"
    $OpenScadCommand = "openscad -o `"$StlFile`" -D `"creature_type=`"$CreatureType`"`" `"$ScadFile`""
    
    Write-Host "Generating STL: $StlFile" -ForegroundColor Yellow
    Write-Host "Command: $OpenScadCommand" -ForegroundColor Gray
    
    try {
        Invoke-Expression $OpenScadCommand
        if (Test-Path $StlFile) {
            Write-Host "STL generated successfully: $StlFile" -ForegroundColor Green
        } else {
            Write-Host "STL generation may have failed - file not found" -ForegroundColor Yellow
        }
    } catch {
        Write-Host "Error generating STL: $($_.Exception.Message)" -ForegroundColor Red
        Write-Host "Make sure OpenSCAD is installed and in your PATH" -ForegroundColor Yellow
    }
}

# Get the absolute path for the viewer
$AbsoluteViewerPath = (Resolve-Path $ViewerFile).Path
$ViewerUrl = "file:///$($AbsoluteViewerPath.Replace('\', '/'))"

# Add creature type as URL parameter
if ($CreatureType -ne "goblin") {
    $ViewerUrl += "?creature=$CreatureType"
}

Write-Host "Opening viewer in default browser..." -ForegroundColor Green
Write-Host "URL: $ViewerUrl" -ForegroundColor Gray

# Launch the viewer in the default browser
try {
    Start-Process $ViewerUrl
    Write-Host "Viewer launched successfully!" -ForegroundColor Green
    Write-Host ""
    Write-Host "VIEWER CONTROLS:" -ForegroundColor Yellow
    Write-Host " Mouse drag: Rotate view"
    Write-Host " Mouse wheel: Zoom in/out"
    Write-Host " Dropdown: Switch creature types"
    Write-Host " Buttons: Toggle components, labels, measurements"
    Write-Host " Generate STL: Export current assembly to STL file"
    Write-Host ""
} catch {
    Write-Host "Error launching viewer: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host "Try opening the file manually: $AbsoluteViewerPath" -ForegroundColor Yellow
}

# Show available STL files
Write-Host "Available STL Files:" -ForegroundColor Cyan
if (Test-Path $StlOutputDir) {
    $StlFiles = Get-ChildItem "$StlOutputDir\*.stl" -ErrorAction SilentlyContinue
    if ($StlFiles) {
        foreach ($file in $StlFiles) {
            $size = [math]::Round($file.Length / 1MB, 2)
            Write-Host "  * $($file.Name) ($size MB)" -ForegroundColor Gray
        }
    } else {
        Write-Host "  No STL files found. Use -AutoGenerate to create them." -ForegroundColor Gray
    }
} else {
    Write-Host "  STL directory not found. Use -AutoGenerate to create files." -ForegroundColor Gray
}

Write-Host ""
Write-Host "Advanced Usage:" -ForegroundColor Cyan
Write-Host "To generate all creature STL files:" -ForegroundColor Gray
Write-Host "  .\tools\generate-stl-files.ps1 -SkullAssemblies" -ForegroundColor Yellow
Write-Host ""
Write-Host "To view STL files in dedicated STL viewer:" -ForegroundColor Gray
Write-Host "  .\tools\launch-stl-viewer.ps1 assets\shapes\stl\goblin_skull_assembly.stl" -ForegroundColor Yellow