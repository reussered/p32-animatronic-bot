# Generate STL/3MF files from OpenSCAD mounting system with multi-color support
# Requires OpenSCAD to be installed and in PATH

param(
    [switch]$BasicMounts,
    [switch]$BotShells,
    [switch]$Generated,
    [switch]$SkullAssemblies,
    [switch]$All,
    [switch]$MultiColor,
    [string]$OutputDir = "assets\shapes\stl",
    [string]$ConstructionType = "hard_shell",
    [string]$BotType = "goblin",
    [string]$Family = ""
)

# Default to true if no specific switches provided
if (-not $BasicMounts -and -not $BotShells -and -not $Generated -and -not $SkullAssemblies -and -not $All) {
    $BasicMounts = $true
    $BotShells = $true
}

if ($All) {
    $BasicMounts = $true
    $BotShells = $true
    $Generated = $true
    $SkullAssemblies = $true
}

$OutputFormat = if ($MultiColor) { "3MF" } else { "STL" }
Write-Host "=== $OutputFormat Generation for P32 Mounting System ===" -ForegroundColor Cyan
Write-Host "Construction Type: $ConstructionType" -ForegroundColor Yellow
if ($Family) {
    Write-Host "Bot Family: $Family" -ForegroundColor Yellow
}

# Check if OpenSCAD is available
$OpenSCADPath = Get-Command "openscad" -ErrorAction SilentlyContinue
if (-not $OpenSCADPath) {
    # Try common installation path as fallback
    $OpenSCADExe = "C:\Program Files\OpenSCAD\openscad.exe"
    if (Test-Path $OpenSCADExe) {
        Write-Host "OpenSCAD not in PATH, using: $OpenSCADExe" -ForegroundColor Yellow
        Write-Host "Note: Restart terminal to use 'openscad' command" -ForegroundColor Cyan
        $OpenSCADPath = Get-Command $OpenSCADExe
    } else {
        Write-Host "ERROR: OpenSCAD not found" -ForegroundColor Red
        Write-Host "Please install OpenSCAD from: https://openscad.org/" -ForegroundColor Yellow
        Write-Host "Or add OpenSCAD to your system PATH" -ForegroundColor Yellow
        return
    }
}

$ScadBasicDir = "assets\shapes\scad\basic_mounts"
$ScadBotShellsDir = "assets\shapes\scad\bot_shells"
$ScadGeneratedDir = "assets\shapes\scad\generated"
$StlBasicDir = "$OutputDir\basic_mounts"
$StlBotShellsDir = "$OutputDir\bot_shells"
$StlGeneratedDir = "$OutputDir\generated"

# Function to load family color palette
function Get-FamilyColorPalette($FamilyName, $ConstructionType) {
    if (-not $FamilyName) { return @{} }
    
    $FamilyPath = "config\bots\bot_families\$FamilyName.json"
    if (-not (Test-Path $FamilyPath)) {
        Write-Host "Warning: Family file not found - $FamilyPath" -ForegroundColor Yellow
        return @{}
    }
    
    try {
        $FamilyConfig = Get-Content $FamilyPath | ConvertFrom-Json
        if ($FamilyConfig.construction_types.$ConstructionType.color_palette) {
            return $FamilyConfig.construction_types.$ConstructionType.color_palette
        }
    } catch {
        Write-Host "Warning: Could not load color palette from $FamilyPath" -ForegroundColor Yellow
    }
    
    return @{}
}

# Function to generate single file with color support
function Generate-ShapeFile($ScadFile, $OutputPath, $ConstructionType, $ColorPalette) {
    $ScadPath = $ScadFile.FullName
    $FileName = $ScadFile.BaseName
    
    # Build OpenSCAD arguments
    $Arguments = @()
    $Arguments += "-o"
    $Arguments += $OutputPath
    
    # Add construction type parameter
    $Arguments += "-D"
    $Arguments += "CONSTRUCTION_TYPE=`"$ConstructionType`""
    
    # Add color palette parameters if available
    if ($ColorPalette.primary) {
        $Arguments += "-D"
        $Arguments += "PRIMARY_COLOR=`"$($ColorPalette.primary)`""
    }
    if ($ColorPalette.secondary) {
        $Arguments += "-D"
        $Arguments += "SECONDARY_COLOR=`"$($ColorPalette.secondary)`""
    }
    if ($ColorPalette.accent) {
        $Arguments += "-D"
        $Arguments += "ACCENT_COLOR=`"$($ColorPalette.accent)`""
    }
    if ($ColorPalette.highlight) {
        $Arguments += "-D"
        $Arguments += "HIGHLIGHT_COLOR=`"$($ColorPalette.highlight)`""
    }
    
    # Add source file
    $Arguments += $ScadPath
    
    Write-Host "Generating: $FileName.$($OutputFormat.ToLower())" -ForegroundColor Yellow
    
    try {
        $Result = & openscad @Arguments 2>&1
        
        if ($LASTEXITCODE -eq 0 -and (Test-Path $OutputPath)) {
            Write-Host "   Generated successfully" -ForegroundColor Green
            return $true
        } else {
            Write-Host "   Generation failed" -ForegroundColor Red
            if ($Result) {
                Write-Host "    Error: $Result" -ForegroundColor Red
            }
            return $false
        }
    } catch {
        Write-Host "   Exception during generation: $($_.Exception.Message)" -ForegroundColor Red
        return $false
    }
}

# Load color palette if family specified
$ColorPalette = Get-FamilyColorPalette $Family $ConstructionType
if ($ColorPalette.Count -gt 0) {
    Write-Host "Using color palette: Primary=$($ColorPalette.primary), Secondary=$($ColorPalette.secondary), Accent=$($ColorPalette.accent), Highlight=$($ColorPalette.highlight)" -ForegroundColor Cyan
}

# Generate Basic Mount Files
if ($BasicMounts) {
    Write-Host "`n=== Generating Basic Mount ${OutputFormat}s ===" -ForegroundColor Green
    
    # Ensure output directory exists
    if (-not (Test-Path $StlBasicDir)) {
        New-Item -ItemType Directory -Path $StlBasicDir -Force | Out-Null
    }
    
    $BasicMountFiles = Get-ChildItem "$ScadBasicDir\*.scad" -ErrorAction SilentlyContinue
    
    if ($BasicMountFiles) {
        foreach ($ScadFile in $BasicMountFiles) {
            $OutputName = $ScadFile.BaseName + "." + $OutputFormat.ToLower()
            $OutputPath = Join-Path $StlBasicDir $OutputName
            
            Generate-ShapeFile $ScadFile $OutputPath $ConstructionType $ColorPalette | Out-Null
        }
    } else {
        Write-Host "No basic mount files found in $ScadBasicDir" -ForegroundColor Yellow
    }
}

# Generate Bot Shell Files
if ($BotShells) {
    Write-Host "`n=== Generating $BotType Shell ${OutputFormat}s ===" -ForegroundColor Green
    
    # Ensure output directory exists
    if (-not (Test-Path $StlBotShellsDir)) {
        New-Item -ItemType Directory -Path $StlBotShellsDir -Force | Out-Null
    }
    
    $BotShellFiles = Get-ChildItem "$ScadBotShellsDir\$BotType*.scad" -ErrorAction SilentlyContinue
    
    if ($BotShellFiles) {
        foreach ($ScadFile in $BotShellFiles) {
            $OutputName = $ScadFile.BaseName + "." + $OutputFormat.ToLower()
            $OutputPath = Join-Path $StlBotShellsDir $OutputName
            
            Generate-ShapeFile $ScadFile $OutputPath $ConstructionType $ColorPalette | Out-Null
        }
    } else {
        Write-Host "No shell files found for bot type: $BotType in $ScadBotShellsDir" -ForegroundColor Yellow
    }
}

# Generate from Generated Directory
if ($Generated) {
    Write-Host "`n=== Generating from Generated Directory ===" -ForegroundColor Green
    
    $GeneratedDir = "$OutputDir\generated"
    if (-not (Test-Path $GeneratedDir)) {
        New-Item -ItemType Directory -Path $GeneratedDir -Force | Out-Null
    }
    
    $GeneratedFiles = Get-ChildItem "assets\shapes\scad\generated\*.scad" -ErrorAction SilentlyContinue
    
    if ($GeneratedFiles) {
        foreach ($ScadFile in $GeneratedFiles) {
            $OutputName = $ScadFile.BaseName + "." + $OutputFormat.ToLower()
            $OutputPath = Join-Path $GeneratedDir $OutputName
            
            Generate-ShapeFile $ScadFile $OutputPath $ConstructionType $ColorPalette | Out-Null
        }
    } else {
        Write-Host "No generated files found in assets\shapes\scad\generated" -ForegroundColor Yellow
    }
}

# Generate Skull Assemblies
if ($SkullAssemblies) {
    Write-Host "`n=== Generating Complete Skull Assemblies ===" -ForegroundColor Green
    
    $SkullAssemblyDir = "$OutputDir\skull_assemblies"
    if (-not (Test-Path $SkullAssemblyDir)) {
        New-Item -ItemType Directory -Path $SkullAssemblyDir -Force | Out-Null
    }
    
    $AssemblyScadFile = "assets\shapes\scad\integrated_skull_assembly_system.scad"
    
    if (Test-Path $AssemblyScadFile) {
        # Define creature types to generate
        $CreatureTypes = @("goblin", "zombie", "android", "cat", "lion", "dragon_fire", "dragon_ice", "bear")
        
        foreach ($CreatureType in $CreatureTypes) {
            Write-Host "  Generating $CreatureType skull assembly..." -ForegroundColor Yellow
            
            $OutputName = "${CreatureType}_skull_assembly." + $OutputFormat.ToLower()
            $OutputPath = Join-Path $SkullAssemblyDir $OutputName
            
            # Build OpenSCAD command with creature type parameter
            $Arguments = @()
            $Arguments += "-o"
            $Arguments += "`"$OutputPath`""
            
            # Set creature type parameter
            $Arguments += "-D"
            $Arguments += "creature_type=`"$CreatureType`""
            
            # Use printable assembly function
            $Arguments += "-D"
            $Arguments += "render_mode=`"printable`""
            
            # Add wall thickness parameter
            $Arguments += "-D"
            $Arguments += "wall_thickness=2.5"
            
            # Add construction type if specified
            if ($ConstructionType) {
                $Arguments += "-D"
                $Arguments += "construction_type=`"$ConstructionType`""
            }
            
            # Add color palette if specified
            if ($ColorPalette.primary) {
                $Arguments += "-D"
                $Arguments += "PRIMARY_COLOR=`"$($ColorPalette.primary)`""
            }
            
            # Add source file
            $Arguments += "`"$AssemblyScadFile`""
            
            try {
                $Result = & openscad @Arguments 2>&1
                
                if ($LASTEXITCODE -eq 0 -and (Test-Path $OutputPath)) {
                    $FileSize = Get-Item $OutputPath
                    $SizeKB = [math]::Round($FileSize.Length / 1KB, 1)
                    Write-Host "    Generated successfully - Size: $SizeKB KB" -ForegroundColor Green
                } else {
                    Write-Host "    Generation failed" -ForegroundColor Red
                    if ($Result) {
                        Write-Host "      Error: $Result" -ForegroundColor Red
                    }
                }
            } catch {
                Write-Host "    Exception occurred: $($_.Exception.Message)" -ForegroundColor Red
            }
        }
    } else {
        Write-Host "  Assembly SCAD file not found: $AssemblyScadFile" -ForegroundColor Red
    }
}

Write-Host "`n=== $OutputFormat Generation Complete ===" -ForegroundColor Cyan

# Show output directories
if ($BasicMounts) {
    Write-Host "Basic Mount ${OutputFormat}s: $StlBasicDir" -ForegroundColor Yellow
}
if ($BotShells) {
    Write-Host "Bot Shell ${OutputFormat}s: $StlBotShellsDir" -ForegroundColor Yellow
}
if ($Generated) {
    Write-Host "Generated ${OutputFormat}s: $OutputDir\generated" -ForegroundColor Yellow
}
if ($SkullAssemblies) {
    Write-Host "Skull Assembly ${OutputFormat}s: $OutputDir\skull_assemblies" -ForegroundColor Yellow
}

# Show file sizes and types
Write-Host "`nGenerated Files:" -ForegroundColor Cyan
$SearchPattern = if ($MultiColor) { "*.3mf" } else { "*.stl" }
$SearchPaths = @()
if ($BasicMounts) { $SearchPaths += "$StlBasicDir\$SearchPattern" }
if ($BotShells) { $SearchPaths += "$StlBotShellsDir\$SearchPattern" }
if ($Generated) { $SearchPaths += "$OutputDir\generated\$SearchPattern" }
if ($SkullAssemblies) { $SearchPaths += "$OutputDir\skull_assemblies\$SearchPattern" }

if ($SearchPaths.Count -gt 0) {
    Get-ChildItem $SearchPaths -ErrorAction SilentlyContinue | 
        ForEach-Object {
            $SizeKB = [math]::Round($_.Length / 1KB, 1)
            $Type = if ($MultiColor) { "Multi-Color" } else { "Standard" }
            Write-Host "  $($_.Name): $SizeKB KB ($Type)" -ForegroundColor White
        }
} else {
    Write-Host "  No files generated" -ForegroundColor Yellow
}

# Multi-color usage instructions
if ($MultiColor) {
    Write-Host "`nMulti-Color 3MF Usage:" -ForegroundColor Green
    Write-Host "   Open 3MF files in PrusaSlicer or Bambu Studio" -ForegroundColor Cyan
    Write-Host "   Assign filament colors to match bot family palette" -ForegroundColor Cyan
    Write-Host "   Use automatic color change commands for layer transitions" -ForegroundColor Cyan
    Write-Host "   Preview colors before printing to verify assignments" -ForegroundColor Cyan
}