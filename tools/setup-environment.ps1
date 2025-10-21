?# P32 Animatronic Bot - Environment Setup Script
# Sets up all required PATH variables and verifies installation

Write-Host "=== P32 Animatronic Bot Environment Setup ===" -ForegroundColor Cyan
Write-Host ""

# Get current User PATH
$CurrentPath = [Environment]::GetEnvironmentVariable("Path", "User")
$PathEntries = $CurrentPath -split ';' | Where-Object { $_ -ne "" }
$Modified = $false

# =============================================================================
# 1. OpenSCAD Path
# =============================================================================
Write-Host "Checking OpenSCAD installation..." -ForegroundColor Yellow

$OpenSCADPath = "C:\Program Files\OpenSCAD"
$OpenSCADExe = Join-Path $OpenSCADPath "openscad.exe"

if (Test-Path $OpenSCADExe) {
    Write-Host "   OpenSCAD found: $OpenSCADExe" -ForegroundColor Green
    
    if ($PathEntries -notcontains $OpenSCADPath) {
        Write-Host "  + Adding to PATH: $OpenSCADPath" -ForegroundColor Cyan
        $PathEntries += $OpenSCADPath
        $Modified = $true
    } else {
        Write-Host "   Already in PATH" -ForegroundColor Green
    }
} else {
    Write-Host "   OpenSCAD NOT FOUND at $OpenSCADPath" -ForegroundColor Red
    Write-Host "    Please install from: https://openscad.org/" -ForegroundColor Yellow
    Write-Host "    Or update the path in this script if installed elsewhere" -ForegroundColor Yellow
}

# =============================================================================
# 2. Project Tools Directory
# =============================================================================
Write-Host ""
Write-Host "Checking Project Tools directory..." -ForegroundColor Yellow

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$ToolsPath = Join-Path $ProjectRoot "tools"

if (Test-Path $ToolsPath) {
    Write-Host "   Tools directory found: $ToolsPath" -ForegroundColor Green
    
    if ($PathEntries -notcontains $ToolsPath) {
        Write-Host "  + Adding to PATH: $ToolsPath" -ForegroundColor Cyan
        $PathEntries += $ToolsPath
        $Modified = $true
    } else {
        Write-Host "   Already in PATH" -ForegroundColor Green
    }
} else {
    Write-Host "   Tools directory NOT FOUND: $ToolsPath" -ForegroundColor Red
}

# =============================================================================
# 3. Python (Optional - for STL visualization)
# =============================================================================
Write-Host ""
Write-Host "Checking Python installation (optional)..." -ForegroundColor Yellow

$PythonCmd = Get-Command python -ErrorAction SilentlyContinue
if ($PythonCmd) {
    $PythonVersion = & python --version 2>&1
    Write-Host "   Python found: $PythonVersion" -ForegroundColor Green
    Write-Host "    Location: $($PythonCmd.Source)" -ForegroundColor Gray
} else {
    Write-Host "   Python not found in PATH" -ForegroundColor Yellow
    Write-Host "    Required for STL visualization tools" -ForegroundColor Gray
    Write-Host "    Install from: https://www.python.org/" -ForegroundColor Gray
}

# =============================================================================
# 4. Git LFS (Required for STL files)
# =============================================================================
Write-Host ""
Write-Host "Checking Git LFS installation..." -ForegroundColor Yellow

$GitLFSCmd = Get-Command git-lfs -ErrorAction SilentlyContinue
if ($GitLFSCmd) {
    $GitLFSVersion = & git-lfs version 2>&1
    Write-Host "   Git LFS found: $GitLFSVersion" -ForegroundColor Green
} else {
    Write-Host "   Git LFS not found" -ForegroundColor Yellow
    Write-Host "    Required for managing large STL files" -ForegroundColor Gray
    Write-Host "    Run: git lfs install" -ForegroundColor Gray
}

# =============================================================================
# Update PATH if modified
# =============================================================================
if ($Modified) {
    Write-Host ""
    Write-Host "=== Updating User PATH ===" -ForegroundColor Cyan
    
    # Remove any empty entries
    $CleanPath = $PathEntries | Where-Object { $_ -ne "" }
    
    # Join and set
    $NewPath = $CleanPath -join ';'
    [Environment]::SetEnvironmentVariable("Path", $NewPath, "User")
    
    Write-Host "   PATH updated successfully!" -ForegroundColor Green
    Write-Host ""
    Write-Host "IMPORTANT: You must restart your terminal/IDE for changes to take effect!" -ForegroundColor Yellow
    Write-Host "  - Close all PowerShell windows" -ForegroundColor Cyan
    Write-Host "  - Close VS Code (if using integrated terminal)" -ForegroundColor Cyan
    Write-Host "  - Reopen and test with: Get-Command openscad" -ForegroundColor Cyan
} else {
    Write-Host ""
    Write-Host "=== No PATH changes needed ===" -ForegroundColor Green
}

# =============================================================================
# Display Current PATH
# =============================================================================
Write-Host ""
Write-Host "=== Current User PATH Entries ===" -ForegroundColor Cyan
$CurrentPath = [Environment]::GetEnvironmentVariable("Path", "User")
$CurrentPath -split ';' | ForEach-Object {
    if ($_ -match "OpenSCAD|p32-animatronic-bot") {
        Write-Host "   $_" -ForegroundColor Green
    } else {
        Write-Host "    $_" -ForegroundColor Gray
    }
}

# =============================================================================
# Verification Commands
# =============================================================================
Write-Host ""
Write-Host "=== Verification Commands (run after restart) ===" -ForegroundColor Cyan
Write-Host ""
Write-Host "Test OpenSCAD:" -ForegroundColor Yellow
Write-Host "  Get-Command openscad" -ForegroundColor White
Write-Host "  openscad --version" -ForegroundColor White
Write-Host ""
Write-Host "Test project tools:" -ForegroundColor Yellow
Write-Host "  generate-mounting-system.ps1 -?" -ForegroundColor White
Write-Host "  generate-stl-files.ps1 -?" -ForegroundColor White
Write-Host ""
Write-Host "Generate test STL:" -ForegroundColor Yellow
Write-Host "  openscad -o test.stl -D `"`$fn=30`" -c `"cube([10,10,10]);`"" -ForegroundColor White
Write-Host ""

Write-Host "=== Setup Complete ===" -ForegroundColor Green
Write-Host ""
