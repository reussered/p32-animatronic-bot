# Generate STL files from OpenSCAD mounting system
# Requires OpenSCAD to be installed and in PATH

param(
    [switch]$BasicMounts,
    [switch]$BotShells,
    [string]$BotType = "goblin"
)

# Default to true if no specific switches provided
if (-not $BasicMounts -and -not $BotShells) {
    $BasicMounts = $true
    $BotShells = $true
}

Write-Host "=== STL Generation for P32 Mounting System ===" -ForegroundColor Cyan

# Check if OpenSCAD is available
$OpenSCADPath = Get-Command "openscad" -ErrorAction SilentlyContinue
if (-not $OpenSCADPath) {
    Write-Host "ERROR: OpenSCAD not found in PATH" -ForegroundColor Red
    Write-Host "Please install OpenSCAD from: https://openscad.org/" -ForegroundColor Yellow
    Write-Host "Or add OpenSCAD to your system PATH" -ForegroundColor Yellow
    return
}

$ScadBasicDir = "assets\shapes\scad\basic_mounts"
$ScadBotShellsDir = "assets\shapes\scad\bot_shells"
$StlBasicDir = "assets\shapes\stl\basic_mounts"
$StlBotShellsDir = "assets\shapes\stl\bot_shells"

# Generate Basic Mount STLs
if ($BasicMounts) {
    Write-Host "`n=== Generating Basic Mount STLs ===" -ForegroundColor Green
    
    $BasicMountFiles = @(
        "display_basic_mount.scad",
        "sensor_basic_mount.scad", 
        "speaker_basic_mount.scad"
    )
    
    foreach ($ScadFile in $BasicMountFiles) {
        $InputPath = "$ScadBasicDir\$ScadFile"
        $StlName = $ScadFile -replace "\.scad$", ".stl"
        $OutputPath = "$StlBasicDir\$StlName"
        
        if (Test-Path $InputPath) {
            Write-Host "Generating: $StlName" -ForegroundColor Yellow
            $Result = & openscad -o $OutputPath $InputPath 2>&1
            
            if ($LASTEXITCODE -eq 0) {
                Write-Host "  ✓ Success: $OutputPath" -ForegroundColor Green
            } else {  
                Write-Host "  ✗ Failed: $Result" -ForegroundColor Red
            }
        }
    }
}

# Generate Bot Shell STLs
if ($BotShells) {
    Write-Host "`n=== Generating $BotType Shell STLs ===" -ForegroundColor Green
    
    $BotShellFiles = Get-ChildItem "$ScadBotShellsDir\$BotType*.scad" -ErrorAction SilentlyContinue
    
    if (-not $BotShellFiles) {
        Write-Host "No shell files found for bot type: $BotType" -ForegroundColor Yellow
        return
    }
    
    foreach ($ScadFile in $BotShellFiles) {
        $StlName = $ScadFile.Name -replace "\.scad$", ".stl"
        $OutputPath = "$StlBotShellsDir\$StlName"
        
        Write-Host "Generating: $StlName" -ForegroundColor Yellow
        $Result = & openscad -o $OutputPath $ScadFile.FullName 2>&1
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "  ✓ Success: $OutputPath" -ForegroundColor Green
        } else {
            Write-Host "  ✗ Failed: $Result" -ForegroundColor Red
        }
    }
}

Write-Host "`n=== STL Generation Complete ===" -ForegroundColor Cyan
Write-Host "Basic Mount STLs: $StlBasicDir" -ForegroundColor Yellow
Write-Host "Bot Shell STLs: $StlBotShellsDir" -ForegroundColor Yellow

# Show file sizes
Write-Host "`nGenerated Files:" -ForegroundColor Cyan
Get-ChildItem "$StlBasicDir\*.stl", "$StlBotShellsDir\*.stl" -ErrorAction SilentlyContinue | 
    ForEach-Object {
        $SizeKB = [math]::Round($_.Length / 1KB, 1)
        Write-Host "  $($_.Name): $SizeKB KB" -ForegroundColor White
    }