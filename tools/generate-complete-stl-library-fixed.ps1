?# P32 Animatronic Bot - Complete STL Generation System
# Generates all 3D printable components for humanoid subsystems

param(
    [switch]$TorsoComponents,
    [switch]$ArmComponents,  
    [switch]$HandComponents,
    [switch]$LegComponents,
    [switch]$FootComponents,
    [switch]$AllSubsystems,
    [switch]$ElectronicsMounts,
    [switch]$ValidateModels,
    [switch]$GeneratePhotos,
    [string]$OutputDir = "assets/shapes/stl"
)

# Script header
Write-Host "P32 ANIMATRONIC BOT - STL GENERATION SYSTEM" -ForegroundColor Cyan
Write-Host "=============================================" -ForegroundColor Cyan
Write-Host ""

# Create output directory structure
$SubDirs = @("torso", "arms", "hands", "legs", "feet", "electronics", "tools", "connectors")
foreach ($dir in $SubDirs) {
    $fullPath = Join-Path $OutputDir $dir
    if (-not (Test-Path $fullPath)) {
        New-Item -ItemType Directory -Path $fullPath -Force | Out-Null
        Write-Host "Created directory: $fullPath" -ForegroundColor Gray
    }
}

Write-Host "Output directory structure ready" -ForegroundColor Green
Write-Host ""

# Find OpenSCAD executable
$OpenSCAD = $null
$OpenSCADPaths = @(
    "C:\Program Files\OpenSCAD\openscad.exe",
    "C:\Program Files (x86)\OpenSCAD\openscad.exe",
    "C:\OpenSCAD\openscad.exe",
    "$env:ProgramFiles\OpenSCAD\openscad.exe",
    "${env:ProgramFiles(x86)}\OpenSCAD\openscad.exe"
)

foreach ($path in $OpenSCADPaths) {
    if (Test-Path $path) {
        $OpenSCAD = $path
        break
    }
}

if (-not $OpenSCAD) {
    Write-Host "ERROR: OpenSCAD not found. Please install OpenSCAD." -ForegroundColor Red
    Write-Host "Download from: https://openscad.org/downloads.html" -ForegroundColor Yellow
    exit 1
}

Write-Host "Using OpenSCAD: $OpenSCAD" -ForegroundColor Green
Write-Host ""

# Function to generate STL from OpenSCAD
function Generate-STL {
    param(
        [string]$ScadFile,
        [string]$StlFile,
        [string]$Description,
        [hashtable]$Parameters = @{}
    )
    
    Write-Host "Generating: $Description" -ForegroundColor Cyan
    Write-Host "  SCAD: $ScadFile" -ForegroundColor Gray
    Write-Host "  STL:  $StlFile" -ForegroundColor Gray
    
    # Build parameter string
    $paramString = ""
    if ($Parameters.Count -gt 0) {
        $paramArray = @()
        foreach ($key in $Parameters.Keys) {
            $paramArray += "$key=$($Parameters[$key])"
        }
        $paramString = "-D " + ($paramArray -join " -D ")
    }
    
    # Generate STL
    $arguments = @()
    $arguments += "-o"
    $arguments += $StlFile
    if ($paramString -ne "") {
        $arguments += $paramString.Split(" ")
    }
    $arguments += $ScadFile
    
    $startTime = Get-Date
    try {
        $process = Start-Process -FilePath $OpenSCAD -ArgumentList $arguments -Wait -PassThru -WindowStyle Hidden
        $exitCode = $process.ExitCode
        if ($exitCode -eq 0) {
            $endTime = Get-Date
            $duration = ($endTime - $startTime).TotalSeconds
            Write-Host "   Generated in $([math]::Round($duration, 1))s" -ForegroundColor Green
        } else {
            Write-Host "   Generation failed" -ForegroundColor Red
        }
    } catch {
        $errorMsg = $_.Exception.Message
        Write-Host "   Error: $errorMsg" -ForegroundColor Red
    }
}

# Function to create OpenSCAD file from template
function New-ComponentSCAD {
    param(
        [string]$ComponentName,
        [hashtable]$Dimensions,
        [string]$ComponentType,
        [string]$OutputFile
    )
    
    $cleanName = $ComponentName.Replace('-','_').Replace(' ','_')
    $scadTemplate = @'
// P32 Animatronic Bot - {0}
// Auto-generated component for {1} subsystem
// Dimensions: {2}x{3}x{4}mm

// Component parameters  
component_width = {2};
component_height = {3};
component_depth = {4};
wall_thickness = 2.5;
mounting_hole_diameter = 3.2;

// Material specifications
print_resolution = 0.2; // Layer height in mm
support_angle = 45; // Support needed for overhangs > 45 degrees

module {5}() {{
    difference() {{
        // Main component body
        hull() {{
            for(x = [3, component_width-3]) {{
                for(y = [3, component_height-3]) {{
                    translate([x, y, 0])
                        cylinder(h=component_depth, d=6);
                }}
            }}
        }}
        
        // Internal cavity for weight reduction
        translate([wall_thickness, wall_thickness, wall_thickness])
            cube([component_width-2*wall_thickness, 
                  component_height-2*wall_thickness, 
                  component_depth-wall_thickness+1]);
        
        // Mounting holes (M3 bolts)
        mounting_positions = [
            [component_width*0.1, component_height*0.1],
            [component_width*0.9, component_height*0.1],
            [component_width*0.1, component_height*0.9], 
            [component_width*0.9, component_height*0.9]
        ];
        
        for(pos = mounting_positions) {{
            translate([pos[0], pos[1], -1])
                cylinder(h=component_depth+2, d=mounting_hole_diameter);
        }}
    }}
}}

// Generate the component
{5}();
'@ -f $ComponentName, $ComponentType, $Dimensions.width, $Dimensions.height, $Dimensions.depth, $cleanName

    $scadTemplate | Out-File -FilePath $OutputFile -Encoding UTF8
}

# Torso Components Generation
if ($TorsoComponents -or $AllSubsystems) {
    Write-Host "GENERATING TORSO COMPONENTS" -ForegroundColor Yellow
    Write-Host "============================" -ForegroundColor Yellow
    
    # Pelvis Base
    New-ComponentSCAD -ComponentName "Pelvis-Base" -ComponentType "Torso" -OutputFile "temp_pelvis_base.scad" -Dimensions @{
        width = 200; height = 150; depth = 50
    }
    Generate-STL -ScadFile "temp_pelvis_base.scad" -StlFile "$OutputDir/torso/pelvis_base.stl" -Description "Torso Pelvis Base"
    
    # Spine Vertebrae (6 copies)
    New-ComponentSCAD -ComponentName "Spine-Vertebra" -ComponentType "Torso" -OutputFile "temp_spine_vertebra.scad" -Dimensions @{
        width = 60; height = 60; depth = 30
    }
    Generate-STL -ScadFile "temp_spine_vertebra.scad" -StlFile "$OutputDir/torso/spine_vertebra.stl" -Description "Torso Spine Vertebra (Print 6x)"
    
    # Power Rail Mounts
    New-ComponentSCAD -ComponentName "Power-Rail-Mount" -ComponentType "Torso" -OutputFile "temp_power_rail_mount.scad" -Dimensions @{
        width = 80; height = 40; depth = 25
    }
    Generate-STL -ScadFile "temp_power_rail_mount.scad" -StlFile "$OutputDir/torso/power_rail_mount.stl" -Description "Torso Power Rail Mount"
    
    # ESP32-S3 Housing
    New-ComponentSCAD -ComponentName "ESP32-S3-Housing" -ComponentType "Torso" -OutputFile "temp_esp32_s3_housing.scad" -Dimensions @{
        width = 70; height = 55; depth = 25
    }
    Generate-STL -ScadFile "temp_esp32_s3_housing.scad" -StlFile "$OutputDir/torso/esp32_s3_housing.stl" -Description "Torso ESP32-S3 Controller Housing"
    
    # Battery Compartment
    New-ComponentSCAD -ComponentName "Battery-Compartment" -ComponentType "Torso" -OutputFile "temp_battery_compartment.scad" -Dimensions @{
        width = 180; height = 90; depth = 50
    }
    Generate-STL -ScadFile "temp_battery_compartment.scad" -StlFile "$OutputDir/torso/battery_compartment.stl" -Description "Torso Battery Compartment"
    
    Write-Host "Torso components complete!" -ForegroundColor Green
    Write-Host ""
}

# Arm Components Generation  
if ($ArmComponents -or $AllSubsystems) {
    Write-Host "GENERATING ARM COMPONENTS" -ForegroundColor Yellow
    Write-Host "==========================" -ForegroundColor Yellow
    
    # Shoulder Housing
    New-ComponentSCAD -ComponentName "Shoulder-Housing" -ComponentType "Arm" -OutputFile "temp_shoulder_housing.scad" -Dimensions @{
        width = 100; height = 80; depth = 60
    }
    Generate-STL -ScadFile "temp_shoulder_housing.scad" -StlFile "$OutputDir/arms/shoulder_housing.stl" -Description "Arm Shoulder Housing"
    
    # Upper Arm Structure
    New-ComponentSCAD -ComponentName "Upper-Arm-Structure" -ComponentType "Arm" -OutputFile "temp_upper_arm.scad" -Dimensions @{
        width = 250; height = 50; depth = 40
    }
    Generate-STL -ScadFile "temp_upper_arm.scad" -StlFile "$OutputDir/arms/upper_arm_structure.stl" -Description "Arm Upper Arm Structure"
    
    # Elbow Housing
    New-ComponentSCAD -ComponentName "Elbow-Housing" -ComponentType "Arm" -OutputFile "temp_elbow_housing.scad" -Dimensions @{
        width = 80; height = 80; depth = 50
    }
    Generate-STL -ScadFile "temp_elbow_housing.scad" -StlFile "$OutputDir/arms/elbow_housing.stl" -Description "Arm Elbow Joint Housing"
    
    # Forearm Structure
    New-ComponentSCAD -ComponentName "Forearm-Structure" -ComponentType "Arm" -OutputFile "temp_forearm.scad" -Dimensions @{
        width = 200; height = 40; depth = 30
    }
    Generate-STL -ScadFile "temp_forearm.scad" -StlFile "$OutputDir/arms/forearm_structure.stl" -Description "Arm Forearm Structure"
    
    # Wrist Assembly
    New-ComponentSCAD -ComponentName "Wrist-Assembly" -ComponentType "Arm" -OutputFile "temp_wrist_assembly.scad" -Dimensions @{
        width = 60; height = 60; depth = 40
    }
    Generate-STL -ScadFile "temp_wrist_assembly.scad" -StlFile "$OutputDir/arms/wrist_assembly.stl" -Description "Arm Wrist Assembly"
    
    # Servo Brackets (8 copies)
    New-ComponentSCAD -ComponentName "Servo-Bracket" -ComponentType "Arm" -OutputFile "temp_servo_bracket.scad" -Dimensions @{
        width = 40; height = 30; depth = 20
    }
    Generate-STL -ScadFile "temp_servo_bracket.scad" -StlFile "$OutputDir/arms/servo_bracket.stl" -Description "Arm Servo Bracket (Print 8x)"
    
    Write-Host "Arm components complete!" -ForegroundColor Green
    Write-Host ""
}

# Hand Components Generation
if ($HandComponents -or $AllSubsystems) {
    Write-Host "GENERATING HAND COMPONENTS" -ForegroundColor Yellow
    Write-Host "===========================" -ForegroundColor Yellow
    
    # Palm Base
    New-ComponentSCAD -ComponentName "Palm-Base" -ComponentType "Hand" -OutputFile "temp_palm_base.scad" -Dimensions @{
        width = 90; height = 130; depth = 25
    }
    Generate-STL -ScadFile "temp_palm_base.scad" -StlFile "$OutputDir/hands/palm_base.stl" -Description "Hand Palm Base"
    
    # Finger Segments (15 copies total - 3 per finger)
    New-ComponentSCAD -ComponentName "Finger-Segment" -ComponentType "Hand" -OutputFile "temp_finger_segment.scad" -Dimensions @{
        width = 18; height = 35; depth = 12
    }
    Generate-STL -ScadFile "temp_finger_segment.scad" -StlFile "$OutputDir/hands/finger_segment.stl" -Description "Hand Finger Segment (Print 15x)"
    
    # Thumb Segments (3 copies)
    New-ComponentSCAD -ComponentName "Thumb-Segment" -ComponentType "Hand" -OutputFile "temp_thumb_segment.scad" -Dimensions @{
        width = 22; height = 30; depth = 14
    }
    Generate-STL -ScadFile "temp_thumb_segment.scad" -StlFile "$OutputDir/hands/thumb_segment.stl" -Description "Hand Thumb Segment (Print 3x)"
    
    # ESP32-C3 Housing for Hand
    New-ComponentSCAD -ComponentName "ESP32-C3-Hand-Housing" -ComponentType "Hand" -OutputFile "temp_esp32_c3_hand.scad" -Dimensions @{
        width = 55; height = 40; depth = 20
    }
    Generate-STL -ScadFile "temp_esp32_c3_hand.scad" -StlFile "$OutputDir/hands/esp32_c3_housing.stl" -Description "Hand ESP32-C3 Controller Housing"
    
    # Servo Mounts (5 copies - one per finger)
    New-ComponentSCAD -ComponentName "Finger-Servo-Mount" -ComponentType "Hand" -OutputFile "temp_finger_servo_mount.scad" -Dimensions @{
        width = 25; height = 20; depth = 15
    }
    Generate-STL -ScadFile "temp_finger_servo_mount.scad" -StlFile "$OutputDir/hands/finger_servo_mount.stl" -Description "Hand Finger Servo Mount (Print 5x)"
    
    Write-Host "Hand components complete!" -ForegroundColor Green
    Write-Host ""
}

# Leg Components Generation
if ($LegComponents -or $AllSubsystems) {
    Write-Host "GENERATING LEG COMPONENTS" -ForegroundColor Yellow
    Write-Host "==========================" -ForegroundColor Yellow
    
    # Hip Joint Housing
    New-ComponentSCAD -ComponentName "Hip-Joint-Housing" -ComponentType "Leg" -OutputFile "temp_hip_housing.scad" -Dimensions @{
        width = 120; height = 100; depth = 80
    }
    Generate-STL -ScadFile "temp_hip_housing.scad" -StlFile "$OutputDir/legs/hip_joint_housing.stl" -Description "Leg Hip Joint Housing"
    
    # Upper Leg Structure
    New-ComponentSCAD -ComponentName "Upper-Leg-Structure" -ComponentType "Leg" -OutputFile "temp_upper_leg.scad" -Dimensions @{
        width = 60; height = 300; depth = 50
    }
    Generate-STL -ScadFile "temp_upper_leg.scad" -StlFile "$OutputDir/legs/upper_leg_structure.stl" -Description "Leg Upper Leg Structure"
    
    # Knee Joint Housing
    New-ComponentSCAD -ComponentName "Knee-Joint-Housing" -ComponentType "Leg" -OutputFile "temp_knee_housing.scad" -Dimensions @{
        width = 80; height = 80; depth = 60
    }
    Generate-STL -ScadFile "temp_knee_housing.scad" -StlFile "$OutputDir/legs/knee_joint_housing.stl" -Description "Leg Knee Joint Housing"
    
    # Lower Leg Structure
    New-ComponentSCAD -ComponentName "Lower-Leg-Structure" -ComponentType "Leg" -OutputFile "temp_lower_leg.scad" -Dimensions @{
        width = 50; height = 250; depth = 40
    }
    Generate-STL -ScadFile "temp_lower_leg.scad" -StlFile "$OutputDir/legs/lower_leg_structure.stl" -Description "Leg Lower Leg Structure"
    
    # Ankle Joint Housing
    New-ComponentSCAD -ComponentName "Ankle-Joint-Housing" -ComponentType "Leg" -OutputFile "temp_ankle_housing.scad" -Dimensions @{
        width = 70; height = 60; depth = 50
    }
    Generate-STL -ScadFile "temp_ankle_housing.scad" -StlFile "$OutputDir/legs/ankle_joint_housing.stl" -Description "Leg Ankle Joint Housing"
    
    # ESP32 Housing for Leg (Hip or Knee mounted)
    New-ComponentSCAD -ComponentName "ESP32-Leg-Housing" -ComponentType "Leg" -OutputFile "temp_esp32_leg.scad" -Dimensions @{
        width = 65; height = 50; depth = 25
    }
    Generate-STL -ScadFile "temp_esp32_leg.scad" -StlFile "$OutputDir/legs/esp32_housing.stl" -Description "Leg ESP32 Controller Housing"
    
    # Servo Mounts for Leg Joints (6 copies)
    New-ComponentSCAD -ComponentName "Leg-Servo-Mount" -ComponentType "Leg" -OutputFile "temp_leg_servo_mount.scad" -Dimensions @{
        width = 45; height = 35; depth = 25
    }
    Generate-STL -ScadFile "temp_leg_servo_mount.scad" -StlFile "$OutputDir/legs/leg_servo_mount.stl" -Description "Leg Servo Mount (Print 6x)"
    
    Write-Host "Leg components complete!" -ForegroundColor Green
    Write-Host ""
}

# Foot Components Generation
if ($FootComponents -or $AllSubsystems) {
    Write-Host "GENERATING FOOT COMPONENTS" -ForegroundColor Yellow
    Write-Host "===========================" -ForegroundColor Yellow
    
    # Foot Base Platform
    New-ComponentSCAD -ComponentName "Foot-Base-Platform" -ComponentType "Foot" -OutputFile "temp_foot_base.scad" -Dimensions @{
        width = 120; height = 200; depth = 30
    }
    Generate-STL -ScadFile "temp_foot_base.scad" -StlFile "$OutputDir/feet/foot_base_platform.stl" -Description "Foot Base Platform"
    
    # Toe Segments (5 copies - one per toe)
    New-ComponentSCAD -ComponentName "Toe-Segment" -ComponentType "Foot" -OutputFile "temp_toe_segment.scad" -Dimensions @{
        width = 20; height = 40; depth = 15
    }
    Generate-STL -ScadFile "temp_toe_segment.scad" -StlFile "$OutputDir/feet/toe_segment.stl" -Description "Foot Toe Segment (Print 5x)"
    
    # Heel Support Structure
    New-ComponentSCAD -ComponentName "Heel-Support" -ComponentType "Foot" -OutputFile "temp_heel_support.scad" -Dimensions @{
        width = 80; height = 100; depth = 40
    }
    Generate-STL -ScadFile "temp_heel_support.scad" -StlFile "$OutputDir/feet/heel_support.stl" -Description "Foot Heel Support Structure"
    
    # ESP32-C3 Housing for Foot (Ankle mounted)
    New-ComponentSCAD -ComponentName "ESP32-C3-Foot-Housing" -ComponentType "Foot" -OutputFile "temp_esp32_c3_foot.scad" -Dimensions @{
        width = 55; height = 40; depth = 20
    }
    Generate-STL -ScadFile "temp_esp32_c3_foot.scad" -StlFile "$OutputDir/feet/esp32_c3_housing.stl" -Description "Foot ESP32-C3 Controller Housing"
    
    # Pressure Sensor Mounts (8 copies)
    New-ComponentSCAD -ComponentName "Pressure-Sensor-Mount" -ComponentType "Foot" -OutputFile "temp_pressure_sensor_mount.scad" -Dimensions @{
        width = 25; height = 25; depth = 10
    }
    Generate-STL -ScadFile "temp_pressure_sensor_mount.scad" -StlFile "$OutputDir/feet/pressure_sensor_mount.stl" -Description "Foot Pressure Sensor Mount (Print 8x)"
    
    # Toe Servo Mounts (5 copies)
    New-ComponentSCAD -ComponentName "Toe-Servo-Mount" -ComponentType "Foot" -OutputFile "temp_toe_servo_mount.scad" -Dimensions @{
        width = 25; height = 20; depth = 12
    }
    Generate-STL -ScadFile "temp_toe_servo_mount.scad" -StlFile "$OutputDir/feet/toe_servo_mount.stl" -Description "Foot Toe Servo Mount (Print 5x)"
    
    Write-Host "Foot components complete!" -ForegroundColor Green
    Write-Host ""
}

# Electronics Mounts Generation
if ($ElectronicsMounts -or $AllSubsystems) {
    Write-Host "GENERATING ELECTRONICS MOUNTS" -ForegroundColor Yellow
    Write-Host "===============================" -ForegroundColor Yellow
    
    # Generic ESP32-S3 Mount
    New-ComponentSCAD -ComponentName "ESP32-S3-Generic-Mount" -ComponentType "Electronics" -OutputFile "temp_esp32_s3_generic.scad" -Dimensions @{
        width = 70; height = 55; depth = 25
    }
    Generate-STL -ScadFile "temp_esp32_s3_generic.scad" -StlFile "$OutputDir/electronics/esp32_s3_generic_mount.stl" -Description "Generic ESP32-S3 Mount"
    
    # Generic ESP32-C3 Mount
    New-ComponentSCAD -ComponentName "ESP32-C3-Generic-Mount" -ComponentType "Electronics" -OutputFile "temp_esp32_c3_generic.scad" -Dimensions @{
        width = 55; height = 40; depth = 20
    }
    Generate-STL -ScadFile "temp_esp32_c3_generic.scad" -StlFile "$OutputDir/electronics/esp32_c3_generic_mount.stl" -Description "Generic ESP32-C3 Mount"
    
    # Power Distribution Board Mount
    New-ComponentSCAD -ComponentName "Power-Distribution-Mount" -ComponentType "Electronics" -OutputFile "temp_power_dist_mount.scad" -Dimensions @{
        width = 100; height = 80; depth = 30
    }
    Generate-STL -ScadFile "temp_power_dist_mount.scad" -StlFile "$OutputDir/electronics/power_distribution_mount.stl" -Description "Power Distribution Board Mount"
    
    # Buck Converter Mounts (multiple copies)
    New-ComponentSCAD -ComponentName "Buck-Converter-Mount" -ComponentType "Electronics" -OutputFile "temp_buck_converter_mount.scad" -Dimensions @{
        width = 60; height = 40; depth = 20
    }
    Generate-STL -ScadFile "temp_buck_converter_mount.scad" -StlFile "$OutputDir/electronics/buck_converter_mount.stl" -Description "Buck Converter Mount (Print 4x)"
    
    # Servo Driver Board Mounts (PCA9685)
    New-ComponentSCAD -ComponentName "Servo-Driver-Mount" -ComponentType "Electronics" -OutputFile "temp_servo_driver_mount.scad" -Dimensions @{
        width = 65; height = 26; depth = 15
    }
    Generate-STL -ScadFile "temp_servo_driver_mount.scad" -StlFile "$OutputDir/electronics/servo_driver_mount.stl" -Description "Servo Driver Board Mount (Print 6x)"
    
    Write-Host "Electronics mounts complete!" -ForegroundColor Green
    Write-Host ""
}

# Clean up temporary SCAD files
Write-Host "Cleaning up temporary files..." -ForegroundColor Gray
Get-ChildItem -Path "." -Filter "temp_*.scad" | Remove-Item -Force

# Validate Generated Models
if ($ValidateModels -or $AllSubsystems) {
    Write-Host "VALIDATING GENERATED STL FILES" -ForegroundColor Yellow
    Write-Host "===============================" -ForegroundColor Yellow
    
    $stlFiles = Get-ChildItem -Path $OutputDir -Filter "*.stl" -Recurse
    foreach ($file in $stlFiles) {
        $sizeKB = [math]::Round($file.Length / 1KB, 1)
        if ($file.Length -gt 0) {
            Write-Host " $($file.Name) ($sizeKB KB)" -ForegroundColor Green
        } else {
            Write-Host " $($file.Name) (0 KB - Generation Failed)" -ForegroundColor Red
        }
    }
    Write-Host ""
}

# Summary Report
Write-Host "STL GENERATION COMPLETE" -ForegroundColor Green
Write-Host "=======================" -ForegroundColor Green

$totalFiles = (Get-ChildItem -Path $OutputDir -Filter "*.stl" -Recurse).Count
$totalSize = (Get-ChildItem -Path $OutputDir -Filter "*.stl" -Recurse | Measure-Object -Property Length -Sum).Sum
$totalSizeMB = [math]::Round($totalSize / 1MB, 1)

Write-Host "Generated $totalFiles STL files ($totalSizeMB MB total)" -ForegroundColor Cyan
Write-Host "Output directory: $OutputDir" -ForegroundColor Cyan
Write-Host ""
Write-Host "NEXT STEPS:" -ForegroundColor Yellow
Write-Host "1. Review STL files using: .\tools\launch-stl-viewer.ps1 -AllSubsystems" -ForegroundColor White
Write-Host "2. Begin 3D printing with recommended settings from documentation" -ForegroundColor White  
Write-Host "3. Follow visual assembly guides in docs\VISUAL_CONSTRUCTION_DOCUMENTATION.md" -ForegroundColor White
Write-Host "4. Document assembly process with photos for future reference" -ForegroundColor White
Write-Host ""