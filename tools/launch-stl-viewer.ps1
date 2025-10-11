# STL Viewer Launcher Script
# Opens the web-based STL viewer for P32 Animatronic Bot shapes

param(
    [string]$STLFile = "",
    [switch]$Help
)

if ($Help) {
    Write-Host "P32 Animatronic Bot - STL Viewer Launcher" -ForegroundColor Green
    Write-Host ""
    Write-Host "Usage: .\launch-stl-viewer.ps1 [STLFile]"
    Write-Host ""
    Write-Host "The viewer supports drag to rotate, right-click to pan, wheel to zoom"
    return
}

# Get the directory where this script is located
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ViewerPath = Join-Path $ScriptDir "stl-viewer.html"

# Check if viewer exists
if (-not (Test-Path $ViewerPath)) {
    Write-Host "Error: STL viewer not found at $ViewerPath" -ForegroundColor Red
    return
}

# Convert to absolute path for browser
$ViewerPath = (Resolve-Path $ViewerPath).Path
$ViewerURL = "file:///$($ViewerPath.Replace('\', '/'))"

Write-Host "P32 Animatronic Bot - STL Viewer" -ForegroundColor Green
Write-Host "Opening STL viewer in default browser..." -ForegroundColor Yellow

# Launch in default browser
Start-Process $ViewerURL
Write-Host " STL viewer opened successfully" -ForegroundColor Green

if ($STLFile -and (Test-Path $STLFile)) {
    Write-Host ""
    Write-Host "STL File: $STLFile" -ForegroundColor Cyan
    Write-Host "Use the Choose File button in the viewer to load this STL file" -ForegroundColor Yellow
}
