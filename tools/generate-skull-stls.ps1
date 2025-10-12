# Simple STL Generation for Skull Assemblies
# Generates basic STL files for viewing and 3D printing

param(
    [string[]]$CreatureTypes = @("goblin", "zombie", "android", "cat", "lion", "dragon_fire", "dragon_ice", "bear"),
    [string]$OutputDir = "assets\shapes\stl\skull_assemblies",
    [switch]$Help
)

if ($Help) {
    Write-Host "Simple STL Generation for P32 Skull Assemblies" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "USAGE:" -ForegroundColor Yellow
    Write-Host "  .\generate-skull-stls.ps1 [-CreatureTypes <types>] [-OutputDir <path>] [-Help]"
    Write-Host ""
    Write-Host "PARAMETERS:" -ForegroundColor Yellow
    Write-Host "  -CreatureTypes   Array of creature types to generate (default: all)"
    Write-Host "  -OutputDir       Output directory for STL files"
    Write-Host "  -Help           Show this help message"
    Write-Host ""
    Write-Host "EXAMPLES:" -ForegroundColor Green
    Write-Host "  .\generate-skull-stls.ps1                                    # Generate all"
    Write-Host "  .\generate-skull-stls.ps1 -CreatureTypes goblin,zombie       # Generate specific"
    Write-Host ""
    return
}

Write-Host "Generating STL Files for Skull Assemblies" -ForegroundColor Cyan
Write-Host "Output Directory: $OutputDir" -ForegroundColor Yellow

# Check if OpenSCAD is available
$OpenSCADPath = Get-Command "openscad" -ErrorAction SilentlyContinue
if (-not $OpenSCADPath) {
    Write-Host "ERROR: OpenSCAD not found in PATH" -ForegroundColor Red
    Write-Host "Please install OpenSCAD from: https://openscad.org/" -ForegroundColor Yellow
    Write-Host "Or add OpenSCAD to your system PATH" -ForegroundColor Yellow
    return
}

# Assembly SCAD file
$AssemblyScadFile = "assets\shapes\scad\integrated_skull_assembly_system.scad"

if (-not (Test-Path $AssemblyScadFile)) {
    Write-Host "ERROR: Assembly SCAD file not found: $AssemblyScadFile" -ForegroundColor Red
    return
}

# Create output directory
if (-not (Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
    Write-Host "Created output directory: $OutputDir" -ForegroundColor Green
}

Write-Host ""
Write-Host "Generating STL files..." -ForegroundColor Green

$SuccessCount = 0
$FailCount = 0

foreach ($CreatureType in $CreatureTypes) {
    Write-Host "Processing: $CreatureType" -ForegroundColor Yellow
    
    $OutputFile = Join-Path $OutputDir "${CreatureType}_skull_assembly.stl"
    
    # Build OpenSCAD command
    $Arguments = @(
        "-o", "`"$OutputFile`"",
        "-D", "creature_type=`"$CreatureType`"",
        "-D", "show_components=true",
        "-D", "show_skull=true", 
        "`"$AssemblyScadFile`""
    )
    
    Write-Host "  Command: openscad $($Arguments -join ' ')" -ForegroundColor Gray
    
    try {
        $StartTime = Get-Date
        $Result = & openscad @Arguments 2>&1
        $EndTime = Get-Date
        $Duration = ($EndTime - $StartTime).TotalSeconds
        
        if ($LASTEXITCODE -eq 0 -and (Test-Path $OutputFile)) {
            $FileInfo = Get-Item $OutputFile
            $SizeMB = [math]::Round($FileInfo.Length / 1MB, 2)
            Write-Host "  SUCCESS - Generated $OutputFile" -ForegroundColor Green
            Write-Host "    Size: $SizeMB MB, Duration: $([math]::Round($Duration, 1))s" -ForegroundColor Gray
            $SuccessCount++
        } else {
            Write-Host "  FAILED - Generation failed for $CreatureType" -ForegroundColor Red
            if ($Result) {
                Write-Host "    Error: $Result" -ForegroundColor Red
            }
            $FailCount++
        }
    } catch {
        Write-Host "  ERROR - Exception during generation: $($_.Exception.Message)" -ForegroundColor Red
        $FailCount++
    }
    
    Write-Host ""
}

# Summary
Write-Host "Generation Complete" -ForegroundColor Cyan
Write-Host "Success: $SuccessCount" -ForegroundColor Green
Write-Host "Failed: $FailCount" -ForegroundColor Red

if ($SuccessCount -gt 0) {
    Write-Host ""
    Write-Host "Generated Files:" -ForegroundColor Yellow
    Get-ChildItem "$OutputDir\*.stl" | ForEach-Object {
        $SizeMB = [math]::Round($_.Length / 1MB, 2)
        Write-Host "  $($_.Name) - $SizeMB MB" -ForegroundColor White
    }
    
    Write-Host ""
    Write-Host "To view generated files:" -ForegroundColor Cyan
    Write-Host "  .\tools\launch-stl-viewer.ps1 `"$OutputDir\goblin_skull_assembly.stl`"" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "To launch interactive viewer:" -ForegroundColor Cyan
    Write-Host "  .\tools\launch-skull-viewer.ps1" -ForegroundColor Yellow
}