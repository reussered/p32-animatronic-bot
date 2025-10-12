# OpenSCAD Shape Generator
param([string]$ScadFile, [string]$OutputSTL = "", [switch]$ViewAfter)

$openscadPath = "C:\Program Files\OpenSCAD\openscad.exe"

if (-not $ScadFile) { Write-Host "Usage: .\generate-shape.ps1 <ScadFile>"; return }
if (-not (Test-Path $ScadFile)) { Write-Host "File not found: $ScadFile"; return }
if (-not (Test-Path $openscadPath)) { Write-Host "OpenSCAD not found"; return }

if (-not $OutputSTL) {
    $baseName = [System.IO.Path]::GetFileNameWithoutExtension($ScadFile)
    $OutputSTL = "assets/shapes/stl/$baseName.stl"
}

$outputDir = Split-Path $OutputSTL -Parent
if (-not (Test-Path $outputDir)) { New-Item -ItemType Directory -Path $outputDir -Force | Out-Null }

Write-Host "Generating: $ScadFile -> $OutputSTL" -ForegroundColor Green

& $openscadPath -o $OutputSTL $ScadFile

if (Test-Path $OutputSTL) {
    $size = (Get-Item $OutputSTL).Length
    Write-Host "Success! Size: $size bytes" -ForegroundColor Green
    
    if ($ViewAfter) {
        .\tools\launch-stl-viewer.ps1 $OutputSTL
    }
} else {
    Write-Host "Generation failed" -ForegroundColor Red
}
