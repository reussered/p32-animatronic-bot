# P32 Animatronic Bot - STL Generation System (FIXED VERSION)
param(
    [switch]$AllSubsystems,
    [switch]$Test,
    [string]$OutputDir = "assets/shapes/stl"
)

Write-Host "P32 ANIMATRONIC BOT - STL GENERATION SYSTEM" -ForegroundColor Cyan
Write-Host "=============================================" -ForegroundColor Cyan
Write-Host ""

# Create output directories
$SubDirs = @("torso", "arms", "hands", "legs", "feet", "electronics")
foreach ($dir in $SubDirs) {
    $fullPath = Join-Path $OutputDir $dir
    if (-not (Test-Path $fullPath)) {
        New-Item -ItemType Directory -Path $fullPath -Force | Out-Null
        Write-Host "Created directory: $fullPath" -ForegroundColor Gray
    }
}

Write-Host "Output directory structure ready" -ForegroundColor Green
Write-Host ""

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
    Write-Host "Download from: https://openscad.org/downloads.html" -ForegroundColor Yellow
    exit 1
}

Write-Host "Found OpenSCAD: $OpenSCAD" -ForegroundColor Green
Write-Host ""

# STL generation function  
function New-STLFile {
    param([string]$ScadFile, [string]$StlFile, [string]$Description)
    
    Write-Host "Generating: $Description" -ForegroundColor Cyan
    Write-Host "  SCAD: $ScadFile" -ForegroundColor Gray
    Write-Host "  STL:  $StlFile" -ForegroundColor Gray
    
    try {
        $arguments = @("-o", $StlFile, $ScadFile)
        $process = Start-Process -FilePath $OpenSCAD -ArgumentList $arguments -Wait -PassThru -WindowStyle Hidden
        
        if ($process.ExitCode -eq 0 -and (Test-Path $StlFile)) {
            $fileSize = (Get-Item $StlFile).Length
            $sizeKB = [math]::Round($fileSize / 1KB, 1)
            Write-Host "  [SUCCESS] Generated successfully" -ForegroundColor Green
            return $true
        } else {
            Write-Host "  [FAILED] Generation failed" -ForegroundColor Red
            return $false
        }
    } catch {
        Write-Host "  [ERROR] Generation error" -ForegroundColor Red
        return $false
    }
}

# Create SCAD file function
function New-ComponentSCAD {
    param([string]$Name, [int]$Width, [int]$Height, [int]$Depth, [string]$OutputFile)
    
    $cleanName = $Name.Replace('-','_').Replace(' ','_')
    $scadContent = @'
// P32 Animatronic Bot - {0}
// Component dimensions: {1}x{2}x{3}mm

module {4}() {{
    difference() {{
        // Main body
        hull() {{
            for(x = [3, {1}-3]) {{
                for(y = [3, {2}-3]) {{
                    translate([x, y, 0])
                        cylinder(h={3}, d=6, center=false);
                }}
            }}
        }}
        
        // Internal cavity
        translate([2.5, 2.5, 2.5])
            cube([{1}-5, {2}-5, {3}-2.5]);
        
        // Mounting holes (M3)
        mounting_holes = [
            [5, 5], [{1}-5, 5], [5, {2}-5], [{1}-5, {2}-5]
        ];
        for(hole = mounting_holes) {{
            translate([hole[0], hole[1], -1])
                cylinder(h={3}+2, d=3.2, center=false);
        }}
    }}
}}

{4}();
'@ -f $Name, $Width, $Height, $Depth, $cleanName

    $scadContent | Out-File -FilePath $OutputFile -Encoding UTF8
    Write-Host "Created SCAD: $OutputFile" -ForegroundColor Gray
}

# Quick test mode
if ($Test) {
    Write-Host "QUICK TEST MODE" -ForegroundColor Yellow
    Write-Host "===============" -ForegroundColor Yellow
    
    New-ComponentSCAD -Name "Test-Component" -Width 50 -Height 30 -Depth 20 -OutputFile "test_component.scad"
    $success = New-STLFile -ScadFile "test_component.scad" -StlFile "$OutputDir/test_component.stl" -Description "Test Component"
    
    if ($success) {
        Write-Host "✓ STL generation system working!" -ForegroundColor Green
    } else {
        Write-Host "✗ STL generation failed" -ForegroundColor Red
    }
    
    # Clean up
    if (Test-Path "test_component.scad") {
        Remove-Item "test_component.scad" -Force
    }
    
    exit
}

# Generate all subsystem components
if ($AllSubsystems) {
    Write-Host "GENERATING ALL SUBSYSTEM COMPONENTS" -ForegroundColor Yellow
    Write-Host "====================================" -ForegroundColor Yellow
    
    $totalGenerated = 0
    $totalFailed = 0
    
    # Torso Components
    Write-Host ""
    Write-Host "TORSO SUBSYSTEM:" -ForegroundColor White
    
    New-ComponentSCAD -Name "Pelvis-Base" -Width 200 -Height 150 -Depth 50 -OutputFile "temp_pelvis.scad"
    if (New-STLFile -ScadFile "temp_pelvis.scad" -StlFile "$OutputDir/torso/pelvis_base.stl" -Description "Pelvis Base") { $totalGenerated++ } else { $totalFailed++ }
    
    New-ComponentSCAD -Name "Spine-Vertebra" -Width 60 -Height 60 -Depth 30 -OutputFile "temp_vertebra.scad"  
    if (New-STLFile -ScadFile "temp_vertebra.scad" -StlFile "$OutputDir/torso/spine_vertebra.stl" -Description "Spine Vertebra (Print 6x)") { $totalGenerated++ } else { $totalFailed++ }
    
    New-ComponentSCAD -Name "ESP32-S3-Housing" -Width 70 -Height 55 -Depth 25 -OutputFile "temp_esp32_s3.scad"
    if (New-STLFile -ScadFile "temp_esp32_s3.scad" -StlFile "$OutputDir/torso/esp32_s3_housing.stl" -Description "ESP32-S3 Controller Housing") { $totalGenerated++ } else { $totalFailed++ }
    
    New-ComponentSCAD -Name "Battery-Compartment" -Width 180 -Height 90 -Depth 50 -OutputFile "temp_battery.scad"
    if (New-STLFile -ScadFile "temp_battery.scad" -StlFile "$OutputDir/torso/battery_compartment.stl" -Description "Battery Compartment") { $totalGenerated++ } else { $totalFailed++ }
    
    # Arm Components  
    Write-Host ""
    Write-Host "ARM SUBSYSTEM:" -ForegroundColor White
    
    New-ComponentSCAD -Name "Shoulder-Housing" -Width 100 -Height 80 -Depth 60 -OutputFile "temp_shoulder.scad"
    if (New-STLFile -ScadFile "temp_shoulder.scad" -StlFile "$OutputDir/arms/shoulder_housing.stl" -Description "Shoulder Housing") { $totalGenerated++ } else { $totalFailed++ }
    
    New-ComponentSCAD -Name "Upper-Arm-Structure" -Width 250 -Height 50 -Depth 40 -OutputFile "temp_upper_arm.scad"
    if (New-STLFile -ScadFile "temp_upper_arm.scad" -StlFile "$OutputDir/arms/upper_arm_structure.stl" -Description "Upper Arm Structure") { $totalGenerated++ } else { $totalFailed++ }
    
    New-ComponentSCAD -Name "Elbow-Housing" -Width 80 -Height 80 -Depth 50 -OutputFile "temp_elbow.scad"
    if (New-STLFile -ScadFile "temp_elbow.scad" -StlFile "$OutputDir/arms/elbow_housing.stl" -Description "Elbow Housing") { $totalGenerated++ } else { $totalFailed++ }
    
    New-ComponentSCAD -Name "Forearm-Structure" -Width 200 -Height 40 -Depth 30 -OutputFile "temp_forearm.scad"
    if (New-STLFile -ScadFile "temp_forearm.scad" -StlFile "$OutputDir/arms/forearm_structure.stl" -Description "Forearm Structure") { $totalGenerated++ } else { $totalFailed++ }
    
    # Hand Components
    Write-Host ""
    Write-Host "HAND SUBSYSTEM:" -ForegroundColor White
    
    New-ComponentSCAD -Name "Palm-Base" -Width 90 -Height 130 -Depth 25 -OutputFile "temp_palm.scad"
    if (New-STLFile -ScadFile "temp_palm.scad" -StlFile "$OutputDir/hands/palm_base.stl" -Description "Palm Base") { $totalGenerated++ } else { $totalFailed++ }
    
    New-ComponentSCAD -Name "Finger-Segment" -Width 18 -Height 35 -Depth 12 -OutputFile "temp_finger.scad"
    if (New-STLFile -ScadFile "temp_finger.scad" -StlFile "$OutputDir/hands/finger_segment.stl" -Description "Finger Segment (Print 15x)") { $totalGenerated++ } else { $totalFailed++ }
    
    New-ComponentSCAD -Name "ESP32-C3-Hand-Housing" -Width 55 -Height 40 -Depth 20 -OutputFile "temp_esp32_c3_hand.scad"
    if (New-STLFile -ScadFile "temp_esp32_c3_hand.scad" -StlFile "$OutputDir/hands/esp32_c3_housing.stl" -Description "Hand ESP32-C3 Controller") { $totalGenerated++ } else { $totalFailed++ }
    
    # Leg Components
    Write-Host ""
    Write-Host "LEG SUBSYSTEM:" -ForegroundColor White
    
    New-ComponentSCAD -Name "Hip-Joint-Housing" -Width 120 -Height 100 -Depth 80 -OutputFile "temp_hip.scad"
    if (New-STLFile -ScadFile "temp_hip.scad" -StlFile "$OutputDir/legs/hip_joint_housing.stl" -Description "Hip Joint Housing") { $totalGenerated++ } else { $totalFailed++ }
    
    New-ComponentSCAD -Name "Upper-Leg-Structure" -Width 60 -Height 300 -Depth 50 -OutputFile "temp_upper_leg.scad"
    if (New-STLFile -ScadFile "temp_upper_leg.scad" -StlFile "$OutputDir/legs/upper_leg_structure.stl" -Description "Upper Leg Structure") { $totalGenerated++ } else { $totalFailed++ }
    
    New-ComponentSCAD -Name "Knee-Joint-Housing" -Width 80 -Height 80 -Depth 60 -OutputFile "temp_knee.scad"
    if (New-STLFile -ScadFile "temp_knee.scad" -StlFile "$OutputDir/legs/knee_joint_housing.stl" -Description "Knee Joint Housing") { $totalGenerated++ } else { $totalFailed++ }
    
    New-ComponentSCAD -Name "Lower-Leg-Structure" -Width 50 -Height 250 -Depth 40 -OutputFile "temp_lower_leg.scad"
    if (New-STLFile -ScadFile "temp_lower_leg.scad" -StlFile "$OutputDir/legs/lower_leg_structure.stl" -Description "Lower Leg Structure") { $totalGenerated++ } else { $totalFailed++ }
    
    # Foot Components
    Write-Host ""
    Write-Host "FOOT SUBSYSTEM:" -ForegroundColor White
    
    New-ComponentSCAD -Name "Foot-Base-Platform" -Width 120 -Height 200 -Depth 30 -OutputFile "temp_foot_base.scad"
    if (New-STLFile -ScadFile "temp_foot_base.scad" -StlFile "$OutputDir/feet/foot_base_platform.stl" -Description "Foot Base Platform") { $totalGenerated++ } else { $totalFailed++ }
    
    New-ComponentSCAD -Name "Toe-Segment" -Width 20 -Height 40 -Depth 15 -OutputFile "temp_toe.scad"
    if (New-STLFile -ScadFile "temp_toe.scad" -StlFile "$OutputDir/feet/toe_segment.stl" -Description "Toe Segment (Print 5x)") { $totalGenerated++ } else { $totalFailed++ }
    
    New-ComponentSCAD -Name "ESP32-C3-Foot-Housing" -Width 55 -Height 40 -Depth 20 -OutputFile "temp_esp32_c3_foot.scad"
    if (New-STLFile -ScadFile "temp_esp32_c3_foot.scad" -StlFile "$OutputDir/feet/esp32_c3_housing.stl" -Description "Foot ESP32-C3 Controller") { $totalGenerated++ } else { $totalFailed++ }
    
    # Electronics Mounts
    Write-Host ""
    Write-Host "ELECTRONICS MOUNTS:" -ForegroundColor White
    
    New-ComponentSCAD -Name "Power-Distribution-Mount" -Width 100 -Height 80 -Depth 30 -OutputFile "temp_power_dist.scad"
    if (New-STLFile -ScadFile "temp_power_dist.scad" -StlFile "$OutputDir/electronics/power_distribution_mount.stl" -Description "Power Distribution Mount") { $totalGenerated++ } else { $totalFailed++ }
    
    New-ComponentSCAD -Name "Buck-Converter-Mount" -Width 60 -Height 40 -Depth 20 -OutputFile "temp_buck.scad"
    if (New-STLFile -ScadFile "temp_buck.scad" -StlFile "$OutputDir/electronics/buck_converter_mount.stl" -Description "Buck Converter Mount") { $totalGenerated++ } else { $totalFailed++ }
    
    New-ComponentSCAD -Name "Servo-Driver-Mount" -Width 65 -Height 26 -Depth 15 -OutputFile "temp_servo_driver.scad"
    if (New-STLFile -ScadFile "temp_servo_driver.scad" -StlFile "$OutputDir/electronics/servo_driver_mount.stl" -Description "Servo Driver Mount") { $totalGenerated++ } else { $totalFailed++ }
    
    # Clean up temporary files
    Write-Host ""
    Write-Host "Cleaning up temporary files..." -ForegroundColor Gray
    Get-ChildItem -Path "." -Filter "temp_*.scad" | Remove-Item -Force -ErrorAction SilentlyContinue
    
    # Final summary  
    Write-Host ""
    Write-Host "STL GENERATION COMPLETE" -ForegroundColor Green
    Write-Host "=======================" -ForegroundColor Green
    Write-Host "Successfully generated: $totalGenerated files" -ForegroundColor Green
    Write-Host "Failed: $totalFailed files" -ForegroundColor $(if ($totalFailed -eq 0) { "Green" } else { "Red" })
    Write-Host "Output directory: $OutputDir" -ForegroundColor Cyan
    
    Write-Host ""
    Write-Host "NEXT STEPS:" -ForegroundColor Yellow
    Write-Host "1. Review STL files with 3D viewer" -ForegroundColor White
    Write-Host "2. Begin 3D printing with 0.2mm layer height" -ForegroundColor White
    Write-Host "3. Follow assembly documentation" -ForegroundColor White
    Write-Host "4. Test fit components before final assembly" -ForegroundColor White
}

Write-Host "Script execution complete." -ForegroundColor Green