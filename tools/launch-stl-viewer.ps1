?# P32 Animatronic Bot - Enhanced Shape Viewer Launcher
# Supports both single-color STL and multi-color 3MF/AMF files

param(
    [string]$File = "",
    [switch]$UseSystemViewer,
    [string]$PreferredViewer = "PrusaSlicer",
    [switch]$Help
)

if ($Help) {
    Write-Host "P32 Animatronic Bot - Enhanced Shape Viewer Launcher" -ForegroundColor Green
    Write-Host ""
    Write-Host "Usage: .\launch-stl-viewer.ps1 [File] [Options]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -File <path>              Path to shape file (STL, 3MF, AMF)"
    Write-Host "  -UseSystemViewer          Use Windows default viewer for multi-color files"
    Write-Host "  -PreferredViewer <name>   Preferred multi-color viewer (PrusaSlicer, BambuStudio, 3DViewer)"
    Write-Host "  -Help                     Show this help message"
    Write-Host ""
    Write-Host "Supported Formats:"
    Write-Host "  STL - Single color (opens web viewer)"
    Write-Host "  3MF - Multi-color with embedded regions"
    Write-Host "  AMF - Multi-material with color definitions"
    Write-Host ""
    Write-Host "Multi-Color Viewers:"
    Write-Host "  PrusaSlicer - Best overall multi-color support"
    Write-Host "  Bambu Studio - Advanced color region handling"
    Write-Host "  Microsoft 3D Viewer - Built-in Windows viewer"
    return
}

function Launch-MultiColorViewer($FilePath, $ViewerType) {
    $Extension = [System.IO.Path]::GetExtension($FilePath).ToLower()
    
    Write-Host "Attempting to launch $ViewerType for $Extension file..." -ForegroundColor Yellow
    
    switch ($ViewerType) {
        "PrusaSlicer" {
            $PrusaPaths = @(
                "${env:ProgramFiles}\Prusa3D\PrusaSlicer\prusa-slicer.exe",
                "${env:ProgramFiles(x86)}\Prusa3D\PrusaSlicer\prusa-slicer.exe",
                "${env:LOCALAPPDATA}\Programs\PrusaSlicer\prusa-slicer.exe"
            )
            foreach ($path in $PrusaPaths) {
                if (Test-Path $path) {
                    Start-Process $path -ArgumentList "`"$FilePath`""
                    Write-Host " Opened in PrusaSlicer" -ForegroundColor Green
                    return $true
                }
            }
        }
        "BambuStudio" {
            $BambuPaths = @(
                "${env:ProgramFiles}\Bambu Studio\BambuStudio.exe",
                "${env:ProgramFiles(x86)}\Bambu Studio\BambuStudio.exe",
                "${env:LOCALAPPDATA}\Programs\BambuStudio\BambuStudio.exe"
            )
            foreach ($path in $BambuPaths) {
                if (Test-Path $path) {
                    Start-Process $path -ArgumentList "`"$FilePath`""
                    Write-Host " Opened in Bambu Studio" -ForegroundColor Green
                    return $true
                }
            }
        }
        "3DViewer" {
            if ($Extension -eq ".3mf") {
                try {
                    Start-Process "ms-3dviewer:" -ArgumentList $FilePath
                    Write-Host " Opened in Microsoft 3D Viewer" -ForegroundColor Green
                    return $true
                } catch {
                    # 3D Viewer not available
                }
            }
        }
    }
    return $false
}

function Launch-WebViewer($STLFile) {
    # Get the directory where this script is located
    $ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
    $ViewerPath = Join-Path $ScriptDir "stl-viewer.html"

    # Check if viewer exists
    if (-not (Test-Path $ViewerPath)) {
        Write-Host "Error: STL viewer not found at $ViewerPath" -ForegroundColor Red
        return $false
    }

    # Convert to absolute path for browser
    $ViewerPath = (Resolve-Path $ViewerPath).Path
    $ViewerURL = "file:///$($ViewerPath.Replace('\', '/'))"

    Write-Host "Opening web-based STL viewer..." -ForegroundColor Yellow
    Start-Process $ViewerURL
    
    if ($STLFile -and (Test-Path $STLFile)) {
        Write-Host " Web viewer opened - use Choose File button to load: $STLFile" -ForegroundColor Green
    } else {
        Write-Host " Web viewer opened successfully" -ForegroundColor Green
    }
    return $true
}

# Main execution logic
Write-Host "P32 Animatronic Bot - Shape Viewer Launcher" -ForegroundColor Green

if (-not $File) {
    Write-Host "No file specified, opening web viewer..." -ForegroundColor Yellow
    Launch-WebViewer
    return
}

if (-not (Test-Path $File)) {
    Write-Host "Error: File not found: $File" -ForegroundColor Red
    return
}

$Extension = [System.IO.Path]::GetExtension($File).ToLower()
$FileName = Split-Path -Leaf $File

Write-Host "File: $FileName ($Extension)" -ForegroundColor Cyan

# Handle multi-color files
if ($Extension -eq ".3mf" -or $Extension -eq ".amf") {
    Write-Host "Multi-color file detected!" -ForegroundColor Green
    Write-Host "Attempting to use preferred viewer: $PreferredViewer" -ForegroundColor Yellow
    
    $Success = $false
    
    # Try preferred viewer first
    if (-not $Success) {
        $Success = Launch-MultiColorViewer $File $PreferredViewer
    }
    
    # Try alternative viewers
    if (-not $Success) {
        Write-Host "Preferred viewer not found, trying alternatives..." -ForegroundColor Yellow
        
        $Viewers = @("PrusaSlicer", "BambuStudio", "3DViewer")
        foreach ($viewer in $Viewers) {
            if ($viewer -ne $PreferredViewer) {
                $Success = Launch-MultiColorViewer $File $viewer
                if ($Success) { break }
            }
        }
    }
    
    if (-not $Success) {
        Write-Host ""
        Write-Host "  No suitable multi-color viewer found!" -ForegroundColor Red
        Write-Host ""
        Write-Host "Recommended installations:" -ForegroundColor Yellow
        Write-Host "   PrusaSlicer: https://www.prusa3d.com/prusaslicer/" -ForegroundColor Cyan
        Write-Host "   Bambu Studio: https://bambulab.com/en/download/studio" -ForegroundColor Cyan
        Write-Host "   Windows 3D Viewer: Available via Microsoft Store" -ForegroundColor Cyan
        Write-Host ""
        Write-Host "These viewers provide full multi-color support with material assignment." -ForegroundColor White
    }
    
} elseif ($Extension -eq ".stl") {
    # Handle STL files with web viewer
    Write-Host "Single-color STL file - using web viewer" -ForegroundColor Yellow
    Launch-WebViewer $File
    
} else {
    Write-Host "Unsupported file format: $Extension" -ForegroundColor Red
    Write-Host "Supported formats: STL (web viewer), 3MF/AMF (multi-color viewers)" -ForegroundColor Yellow
}
