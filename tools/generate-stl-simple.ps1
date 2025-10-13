# P32 Animatronic Bot - STL Generation System (Simplified)
param(
    [switch]$AllSubsystems,
    [string]$OutputDir = "assets/shapes/stl"
)

Write-Host "P32 ANIMATRONIC BOT - STL GENERATION SYSTEM" -ForegroundColor Cyan
Write-Host "=============================================" -ForegroundColor Cyan

# Create output directories
$SubDirs = @("torso", "arms", "hands", "legs", "feet", "electronics")
foreach ($dir in $SubDirs) {
    $fullPath = Join-Path $OutputDir $dir
    if (-not (Test-Path $fullPath)) {
        New-Item -ItemType Directory -Path $fullPath -Force | Out-Null
        Write-Host "Created directory: $fullPath" -ForegroundColor Gray
    }
}

# Find OpenSCAD
$OpenSCAD = $null
$OpenSCADPaths = @(
    "C:\Program Files\OpenSCAD\openscad.exe",
    "C:\Program Files (x86)\OpenSCAD\openscad.exe"
)

foreach ($path in $OpenSCADPaths) {
    if (Test-Path $path) {
        $OpenSCAD = $path
        break
    }
}

if (-not $OpenSCAD) {
    Write-Host "ERROR: OpenSCAD not found. Please install OpenSCAD." -ForegroundColor Red
    exit 1
}

Write-Host "Using OpenSCAD: $OpenSCAD" -ForegroundColor Green

# Simple STL generation function
function Generate-STL {
    param([string]$ScadFile, [string]$StlFile, [string]$Description)
    
    Write-Host "Generating: $Description" -ForegroundColor Cyan
    
    try {
        $process = Start-Process -FilePath $OpenSCAD -ArgumentList @("-o", $StlFile, $ScadFile) -Wait -PassThru -WindowStyle Hidden
        if ($process.ExitCode -eq 0) {
            Write-Host "  ✓ Generated successfully" -ForegroundColor Green
        } else {
            Write-Host "  ✗ Generation failed" -ForegroundColor Red
        }
    } catch {
        Write-Host "  ✗ Error: $($_.Exception.Message)" -ForegroundColor Red
    }
}

# Create simple SCAD template
function Create-ComponentSCAD {
    param([string]$Name, [int]$Width, [int]$Height, [int]$Depth, [string]$OutputFile)
    
    $content = @'
// P32 Animatronic Bot - {0}
// Dimensions: {1}x{2}x{3}mm

module component() {{
    difference() {{
        // Main body
        cube([{1}, {2}, {3}]);
        
        // Internal cavity
        translate([2, 2, 2])
            cube([{1}-4, {2}-4, {3}-2]);
        
        // Mounting holes
        for(x = [5, {1}-5]) {{
            for(y = [5, {2}-5]) {{
                translate([x, y, -1])
                    cylinder(h={3}+2, d=3.2);
            }}
        }}
    }}
}}

component();
'@ -f $Name, $Width, $Height, $Depth
    
    $content | Out-File -FilePath $OutputFile -Encoding UTF8
}

if ($AllSubsystems) {
    Write-Host ""
    Write-Host "GENERATING ALL SUBSYSTEM COMPONENTS" -ForegroundColor Yellow
    Write-Host "====================================" -ForegroundColor Yellow
    
    # Torso components
    Write-Host ""
    Write-Host "Torso Components:" -ForegroundColor White
    Create-ComponentSCAD -Name "Pelvis-Base" -Width 200 -Height 150 -Depth 50 -OutputFile "temp_pelvis.scad"
    Generate-STL -ScadFile "temp_pelvis.scad" -StlFile "$OutputDir/torso/pelvis_base.stl" -Description "Pelvis Base"
    
    Create-ComponentSCAD -Name "Spine-Vertebra" -Width 60 -Height 60 -Depth 30 -OutputFile "temp_vertebra.scad"
    Generate-STL -ScadFile "temp_vertebra.scad" -StlFile "$OutputDir/torso/spine_vertebra.stl" -Description "Spine Vertebra (Print 6x)"
    
    Create-ComponentSCAD -Name "ESP32-S3-Housing" -Width 70 -Height 55 -Depth 25 -OutputFile "temp_esp32_s3.scad"
    Generate-STL -ScadFile "temp_esp32_s3.scad" -StlFile "$OutputDir/torso/esp32_s3_housing.stl" -Description "ESP32-S3 Controller Housing"
    
    # Arm components
    Write-Host ""
    Write-Host "Arm Components:" -ForegroundColor White
    Create-ComponentSCAD -Name "Shoulder-Housing" -Width 100 -Height 80 -Depth 60 -OutputFile "temp_shoulder.scad"
    Generate-STL -ScadFile "temp_shoulder.scad" -StlFile "$OutputDir/arms/shoulder_housing.stl" -Description "Shoulder Housing"
    
    Create-ComponentSCAD -Name "Upper-Arm" -Width 250 -Height 50 -Depth 40 -OutputFile "temp_upper_arm.scad"
    Generate-STL -ScadFile "temp_upper_arm.scad" -StlFile "$OutputDir/arms/upper_arm_structure.stl" -Description "Upper Arm Structure"
    
    Create-ComponentSCAD -Name "Elbow-Housing" -Width 80 -Height 80 -Depth 50 -OutputFile "temp_elbow.scad"
    Generate-STL -ScadFile "temp_elbow.scad" -StlFile "$OutputDir/arms/elbow_housing.stl" -Description "Elbow Housing"
    
    # Hand components
    Write-Host ""
    Write-Host "Hand Components:" -ForegroundColor White
    Create-ComponentSCAD -Name "Palm-Base" -Width 90 -Height 130 -Depth 25 -OutputFile "temp_palm.scad"
    Generate-STL -ScadFile "temp_palm.scad" -StlFile "$OutputDir/hands/palm_base.stl" -Description "Palm Base"
    
    Create-ComponentSCAD -Name "Finger-Segment" -Width 18 -Height 35 -Depth 12 -OutputFile "temp_finger.scad"
    Generate-STL -ScadFile "temp_finger.scad" -StlFile "$OutputDir/hands/finger_segment.stl" -Description "Finger Segment (Print 15x)"
    
    Create-ComponentSCAD -Name "ESP32-C3-Hand" -Width 55 -Height 40 -Depth 20 -OutputFile "temp_esp32_c3_hand.scad"
    Generate-STL -ScadFile "temp_esp32_c3_hand.scad" -StlFile "$OutputDir/hands/esp32_c3_housing.stl" -Description "Hand ESP32-C3 Controller"
    
    # Leg components
    Write-Host ""
    Write-Host "Leg Components:" -ForegroundColor White
    Create-ComponentSCAD -Name "Hip-Housing" -Width 120 -Height 100 -Depth 80 -OutputFile "temp_hip.scad"
    Generate-STL -ScadFile "temp_hip.scad" -StlFile "$OutputDir/legs/hip_joint_housing.stl" -Description "Hip Joint Housing"
    
    Create-ComponentSCAD -Name "Upper-Leg" -Width 60 -Height 300 -Depth 50 -OutputFile "temp_upper_leg.scad"
    Generate-STL -ScadFile "temp_upper_leg.scad" -StlFile "$OutputDir/legs/upper_leg_structure.stl" -Description "Upper Leg Structure"
    
    Create-ComponentSCAD -Name "Knee-Housing" -Width 80 -Height 80 -Depth 60 -OutputFile "temp_knee.scad"
    Generate-STL -ScadFile "temp_knee.scad" -StlFile "$OutputDir/legs/knee_joint_housing.stl" -Description "Knee Joint Housing"
    
    # Foot components
    Write-Host ""
    Write-Host "Foot Components:" -ForegroundColor White
    Create-ComponentSCAD -Name "Foot-Base" -Width 120 -Height 200 -Depth 30 -OutputFile "temp_foot_base.scad"
    Generate-STL -ScadFile "temp_foot_base.scad" -StlFile "$OutputDir/feet/foot_base_platform.stl" -Description "Foot Base Platform"
    
    Create-ComponentSCAD -Name "Toe-Segment" -Width 20 -Height 40 -Depth 15 -OutputFile "temp_toe.scad"
    Generate-STL -ScadFile "temp_toe.scad" -StlFile "$OutputDir/feet/toe_segment.stl" -Description "Toe Segment (Print 5x)"
    
    Create-ComponentSCAD -Name "ESP32-C3-Foot" -Width 55 -Height 40 -Depth 20 -OutputFile "temp_esp32_c3_foot.scad"
    Generate-STL -ScadFile "temp_esp32_c3_foot.scad" -StlFile "$OutputDir/feet/esp32_c3_housing.stl" -Description "Foot ESP32-C3 Controller"
    
    # Electronics mounts
    Write-Host ""
    Write-Host "Electronics Mounts:" -ForegroundColor White
    Create-ComponentSCAD -Name "Power-Distribution" -Width 100 -Height 80 -Depth 30 -OutputFile "temp_power_dist.scad"
    Generate-STL -ScadFile "temp_power_dist.scad" -StlFile "$OutputDir/electronics/power_distribution_mount.stl" -Description "Power Distribution Mount"
    
    Create-ComponentSCAD -Name "Buck-Converter" -Width 60 -Height 40 -Depth 20 -OutputFile "temp_buck.scad"
    Generate-STL -ScadFile "temp_buck.scad" -StlFile "$OutputDir/electronics/buck_converter_mount.stl" -Description "Buck Converter Mount"
    
    Create-ComponentSCAD -Name "Servo-Driver" -Width 65 -Height 26 -Depth 15 -OutputFile "temp_servo_driver.scad"
    Generate-STL -ScadFile "temp_servo_driver.scad" -StlFile "$OutputDir/electronics/servo_driver_mount.stl" -Description "Servo Driver Mount"
}

# Clean up temporary files
Write-Host ""
Write-Host "Cleaning up temporary files..." -ForegroundColor Gray
Get-ChildItem -Path "." -Filter "temp_*.scad" | Remove-Item -Force -ErrorAction SilentlyContinue

# Summary
Write-Host ""
Write-Host "STL GENERATION COMPLETE" -ForegroundColor Green
Write-Host "=======================" -ForegroundColor Green

$stlFiles = Get-ChildItem -Path $OutputDir -Filter "*.stl" -Recurse -ErrorAction SilentlyContinue
if ($stlFiles) {
    $totalFiles = $stlFiles.Count
    $totalSize = ($stlFiles | Measure-Object -Property Length -Sum).Sum
    $totalSizeMB = [math]::Round($totalSize / 1MB, 1)
    
    Write-Host ("Generated $totalFiles STL files (" + $totalSizeMB + " MB total)") -ForegroundColor Cyan
    Write-Host "Output directory: $OutputDir" -ForegroundColor Cyan
    
    Write-Host ""
    Write-Host "Generated Files:" -ForegroundColor White
    foreach ($file in $stlFiles) {
        $sizeKB = [math]::Round($file.Length / 1KB, 1)
        $relativePath = $file.FullName.Replace($pwd.Path + "\", "")
        Write-Host "  ✓ $relativePath ($sizeKB KB)" -ForegroundColor Green
    }
} else {
    Write-Host "No STL files were generated." -ForegroundColor Red
}

Write-Host ""
Write-Host "NEXT STEPS:" -ForegroundColor Yellow
Write-Host "1. Review STL files using: .\tools\launch-stl-viewer.ps1" -ForegroundColor White
Write-Host "2. Begin 3D printing with recommended settings" -ForegroundColor White  
Write-Host "3. Follow assembly guides in documentation" -ForegroundColor White