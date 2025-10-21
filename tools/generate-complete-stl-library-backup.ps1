?# P32 Animatronic Bot - Complete STL Generation System
# Generates all 3D printable components for humanoid subsystems

param(
    [switch]$TorsoComponents,
    [switch]$ArmComponents,  
    [switch]$HandComponents,
    [switch]$LegComponents,
    [switch]$FootComponents,
    [switch]$AllSubsystems,
    [switch]$GeneratePhotos,
    [switch]$ValidateModels,
    [string]$OutputDir = "assets/shapes/stl"
)

Write-Host "P32 Animatronic Bot - Complete STL Generation System" -ForegroundColor Green
Write-Host "=====================================================" -ForegroundColor Green
Write-Host ""

# Create output directory structure
$SubsystemDirs = @("torso", "arms", "hands", "legs", "feet", "electronics", "basic_mounts")
foreach ($dir in $SubsystemDirs) {
    $fullPath = Join-Path $OutputDir $dir
    if (-not (Test-Path $fullPath)) {
        New-Item -ItemType Directory -Path $fullPath -Force | Out-Null
        Write-Host "Created directory: $fullPath" -ForegroundColor Yellow
    }
}

# OpenSCAD executable detection
$OpenSCADPaths = @(
    "C:\Program Files\OpenSCAD\openscad.exe",
    "C:\Program Files (x86)\OpenSCAD\openscad.exe", 
    "$env:ProgramFiles\OpenSCAD\openscad.exe",
    "openscad.exe"
)

$OpenSCAD = $null
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
        $LASTEXITCODE = $process.ExitCode
        if ($LASTEXITCODE -eq 0) {
            $endTime = Get-Date
            $duration = ($endTime - $startTime).TotalSeconds
            Write-Host "   Generated in $([math]::Round($duration, 1))s" -ForegroundColor Green
        } else {
            Write-Host "   Generation failed" -ForegroundColor Red
        }
    } catch {
        Write-Host "   Error: $($_.Exception.Message)" -ForegroundColor Red
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
    $scadContent = @'
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

    $scadContent | Out-File -FilePath $OutputFile -Encoding UTF8
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
    
    # Palm Base (Flexible TPU)
    New-ComponentSCAD -ComponentName "Palm-Base" -ComponentType "Hand" -OutputFile "temp_palm_base.scad" -Dimensions @{
        width = 90; height = 70; depth = 20
    }
    Generate-STL -ScadFile "temp_palm_base.scad" -StlFile "$OutputDir/hands/palm_base.stl" -Description "Hand Palm Base (TPU)"
    
    # Thumb Assembly
    New-ComponentSCAD -ComponentName "Thumb-Assembly" -ComponentType "Hand" -OutputFile "temp_thumb_assembly.scad" -Dimensions @{
        width = 50; height = 20; depth = 15
    }
    Generate-STL -ScadFile "temp_thumb_assembly.scad" -StlFile "$OutputDir/hands/thumb_assembly.stl" -Description "Hand Thumb Assembly (TPU)"
    
    # Finger Set (4 fingers)
    New-ComponentSCAD -ComponentName "Finger-Set" -ComponentType "Hand" -OutputFile "temp_finger_set.scad" -Dimensions @{
        width = 40; height = 15; depth = 10
    }
    Generate-STL -ScadFile "temp_finger_set.scad" -StlFile "$OutputDir/hands/finger_set.stl" -Description "Hand Finger Set (Print 4x, TPU)"
    
    # Servo Mounts (5 copies)
    New-ComponentSCAD -ComponentName "Servo-Mount" -ComponentType "Hand" -OutputFile "temp_servo_mount.scad" -Dimensions @{
        width = 25; height = 20; depth = 15
    }
    Generate-STL -ScadFile "temp_servo_mount.scad" -StlFile "$OutputDir/hands/servo_mount.stl" -Description "Hand Servo Mount (Print 5x)"
    
    # ESP32-C3 Housing
    New-ComponentSCAD -ComponentName "ESP32-C3-Housing" -ComponentType "Hand" -OutputFile "temp_esp32c3_housing.scad" -Dimensions @{
        width = 50; height = 40; depth = 15
    }
    Generate-STL -ScadFile "temp_esp32c3_housing.scad" -StlFile "$OutputDir/hands/esp32c3_housing.stl" -Description "Hand ESP32-C3 Controller Housing"
    
    Write-Host "Hand components complete!" -ForegroundColor Green
    Write-Host ""
}

# Leg Components Generation
if ($LegComponents -or $AllSubsystems) {
    Write-Host "GENERATING LEG COMPONENTS" -ForegroundColor Yellow
    Write-Host "==========================" -ForegroundColor Yellow
    
    # Hip Housing
    New-ComponentSCAD -ComponentName "Hip-Housing" -ComponentType "Leg" -OutputFile "temp_hip_housing.scad" -Dimensions @{
        width = 120; height = 100; depth = 80
    }
    Generate-STL -ScadFile "temp_hip_housing.scad" -StlFile "$OutputDir/legs/hip_housing.stl" -Description "Leg Hip Joint Housing"
    
    # Thigh Structure
    New-ComponentSCAD -ComponentName "Thigh-Structure" -ComponentType "Leg" -OutputFile "temp_thigh.scad" -Dimensions @{
        width = 300; height = 60; depth = 50
    }
    Generate-STL -ScadFile "temp_thigh.scad" -StlFile "$OutputDir/legs/thigh_structure.stl" -Description "Leg Thigh Structure"
    
    # Knee Assembly
    New-ComponentSCAD -ComponentName "Knee-Assembly" -ComponentType "Leg" -OutputFile "temp_knee_assembly.scad" -Dimensions @{
        width = 100; height = 80; depth = 60
    }
    Generate-STL -ScadFile "temp_knee_assembly.scad" -StlFile "$OutputDir/legs/knee_assembly.stl" -Description "Leg Knee Joint Assembly"
    
    # Calf Structure
    New-ComponentSCAD -ComponentName "Calf-Structure" -ComponentType "Leg" -OutputFile "temp_calf.scad" -Dimensions @{
        width = 250; height = 50; depth = 40
    }
    Generate-STL -ScadFile "temp_calf.scad" -StlFile "$OutputDir/legs/calf_structure.stl" -Description "Leg Calf Structure"
    
    # Ankle Joint
    New-ComponentSCAD -ComponentName "Ankle-Joint" -ComponentType "Leg" -OutputFile "temp_ankle_joint.scad" -Dimensions @{
        width = 80; height = 60; depth = 50
    }
    Generate-STL -ScadFile "temp_ankle_joint.scad" -StlFile "$OutputDir/legs/ankle_joint.stl" -Description "Leg Ankle Joint"
    
    Write-Host "Leg components complete!" -ForegroundColor Green
    Write-Host ""
}

# Foot Components Generation
if ($FootComponents -or $AllSubsystems) {
    Write-Host "GENERATING FOOT COMPONENTS" -ForegroundColor Yellow
    Write-Host "============================" -ForegroundColor Yellow
    
    # Foot Base
    New-ComponentSCAD -ComponentName "Foot-Base" -ComponentType "Foot" -OutputFile "temp_foot_base.scad" -Dimensions @{
        width = 120; height = 200; depth = 30
    }
    Generate-STL -ScadFile "temp_foot_base.scad" -StlFile "$OutputDir/feet/foot_base.stl" -Description "Foot Base Structure"
    
    # Toe Segments (5 copies)
    New-ComponentSCAD -ComponentName "Toe-Segment" -ComponentType "Foot" -OutputFile "temp_toe_segment.scad" -Dimensions @{
        width = 25; height = 40; depth = 15
    }
    Generate-STL -ScadFile "temp_toe_segment.scad" -StlFile "$OutputDir/feet/toe_segment.stl" -Description "Foot Toe Segment (Print 5x, TPU)"
    
    # Heel Assembly
    New-ComponentSCAD -ComponentName "Heel-Assembly" -ComponentType "Foot" -OutputFile "temp_heel_assembly.scad" -Dimensions @{
        width = 80; height = 60; depth = 40
    }
    Generate-STL -ScadFile "temp_heel_assembly.scad" -StlFile "$OutputDir/feet/heel_assembly.stl" -Description "Foot Heel Assembly"
    
    # Sensor Mounts (8 copies)
    New-ComponentSCAD -ComponentName "Sensor-Mount" -ComponentType "Foot" -OutputFile "temp_sensor_mount.scad" -Dimensions @{
        width = 20; height = 20; depth = 10
    }
    Generate-STL -ScadFile "temp_sensor_mount.scad" -StlFile "$OutputDir/feet/sensor_mount.stl" -Description "Foot Pressure Sensor Mount (Print 8x)"
    
    Write-Host "Foot components complete!" -ForegroundColor Green
    Write-Host ""
}

# Generate Basic Electronics Mounts
if ($AllSubsystems) {
    Write-Host "GENERATING ELECTRONICS MOUNTS" -ForegroundColor Yellow
    Write-Host "===============================" -ForegroundColor Yellow
    
    # Use existing basic mounts from the project
    if (Test-Path "assets/shapes/scad/basic_mounts/display_basic_mount.scad") {
        Generate-STL -ScadFile "assets/shapes/scad/basic_mounts/display_basic_mount.scad" -StlFile "$OutputDir/basic_mounts/display_mount.stl" -Description "Basic Display Mount"
    }
    
    if (Test-Path "assets/shapes/scad/basic_mounts/sensor_basic_mount.scad") {
        Generate-STL -ScadFile "assets/shapes/scad/basic_mounts/sensor_basic_mount.scad" -StlFile "$OutputDir/basic_mounts/sensor_mount.stl" -Description "Basic Sensor Mount"
    }
    
    if (Test-Path "assets/shapes/scad/basic_mounts/speaker_basic_mount.scad") {
        Generate-STL -ScadFile "assets/shapes/scad/basic_mounts/speaker_basic_mount.scad" -StlFile "$OutputDir/basic_mounts/speaker_mount.stl" -Description "Basic Speaker Mount"
    }
    
    Write-Host "Electronics mounts complete!" -ForegroundColor Green
    Write-Host ""
}

# Validate Generated Models
if ($ValidateModels) {
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

# Generate Component Photos (Simulated)
if ($GeneratePhotos) {
    Write-Host "GENERATING COMPONENT DOCUMENTATION PHOTOS" -ForegroundColor Yellow
    Write-Host "===========================================" -ForegroundColor Yellow
    
    # Create photo directory structure
    $PhotoDirs = @("assets/photos/construction", "assets/photos/wiring", "assets/photos/tools", "assets/photos/workspace")
    foreach ($dir in $PhotoDirs) {
        if (-not (Test-Path $dir)) {
            New-Item -ItemType Directory -Path $dir -Force | Out-Null
        }
    }
    
    # Create placeholder photo descriptions
    $PhotoDescriptions = @(
        "torso_step_01_pelvis_base.jpg - Pelvis base installation with T-slot extrusions",
        "torso_step_02_power_rails.jpg - Power distribution system with buck converters", 
        "torso_step_03_esp32_controller.jpg - ESP32-S3 controller installation",
        "torso_step_04_spine_assembly.jpg - Flexible spine vertebrae assembly",
        "arm_step_01_shoulder_joint.jpg - 3DOF shoulder joint construction",
        "arm_step_02_upper_arm.jpg - Upper arm structure with cable management",
        "arm_step_03_elbow_joint.jpg - High-torque elbow assembly",
        "arm_step_04_wrist_hand.jpg - Wrist and hand integration",
        "hand_step_01_finger_servos.jpg - SG90 servo installation in fingers",
        "hand_step_02_tactile_sensors.jpg - FSR pressure sensor integration",
        "hand_step_03_controller.jpg - ESP32-C3 and PCA9685 setup",
        "hand_step_04_complete.jpg - Complete hand assembly testing",
        "leg_step_01_hip_joint.jpg - 3DOF hip joint construction",
        "leg_step_02_thigh.jpg - Thigh structure with embedded controller",
        "leg_step_03_knee_joint.jpg - High-torque knee assembly",
        "leg_step_04_calf_ankle.jpg - Calf and ankle integration"
    )
    
    foreach ($photo in $PhotoDescriptions) {
        $filename = $photo.Split(' - ')[0]
        $description = $photo.Split(' - ')[1]
        "# Photo Documentation: $description" | Out-File -FilePath "assets/photos/construction/$filename.txt"
    }
    
    Write-Host "Photo documentation structure created!" -ForegroundColor Green
    Write-Host "NOTE: Actual photos will be added during physical assembly process." -ForegroundColor Yellow
    Write-Host ""
}

# Cleanup temporary files
Get-ChildItem -Path "." -Filter "temp_*.scad" | Remove-Item -Force

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
