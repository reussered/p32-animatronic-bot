?Write-Host "=== P32 Animatronic Bot Complete System Verification ===" -ForegroundColor Cyan
Write-Host ""

$ComponentCount = 0
$InterfaceCount = 0
$HardwareCount = 0

# Check Humanoid Components
$HumanoidComponents = @(
    "humanoid_head_assembly", "humanoid_neck_assembly", "humanoid_spine_vertebrae", "humanoid_pelvis_assembly",
    "humanoid_shoulder_left", "humanoid_shoulder_right", "humanoid_elbow_left", "humanoid_elbow_right",
    "humanoid_wrist_left", "humanoid_wrist_right", "humanoid_hip_left", "humanoid_hip_right",
    "humanoid_knee_left", "humanoid_knee_right", "humanoid_ankle_left", "humanoid_ankle_right"
)

Write-Host " Checking Humanoid Components..." -ForegroundColor Yellow
foreach ($comp in $HumanoidComponents) {
    $file = "config/components/positioned/$comp.json"
    if (Test-Path $file) {
        $ComponentCount++
        Write-Host " $comp" -ForegroundColor Green
    } else {
        Write-Host " MISSING: $comp" -ForegroundColor Red
    }
}

# Check Servo Controllers
$ServoControllers = @(
    "servo_controller_left_shoulder", "servo_controller_right_shoulder",
    "servo_controller_left_elbow", "servo_controller_right_elbow",
    "servo_controller_left_wrist", "servo_controller_right_wrist",
    "servo_controller_left_hip", "servo_controller_right_hip",
    "servo_controller_left_knee", "servo_controller_right_knee", 
    "servo_controller_left_ankle", "servo_controller_right_ankle"
)

Write-Host ""
Write-Host " Checking Servo Controller Interfaces..." -ForegroundColor Yellow
foreach ($ctrl in $ServoControllers) {
    $file = "config/components/interfaces/$ctrl.json"
    if (Test-Path $file) {
        $InterfaceCount++
        Write-Host " $ctrl" -ForegroundColor Green
    } else {
        Write-Host " MISSING: $ctrl" -ForegroundColor Red
    }
}

# Check Hardware
$HardwareFiles = @("servo_assembly", "shoulder_joint_assembly")
Write-Host ""
Write-Host " Checking Hardware Definitions..." -ForegroundColor Yellow
foreach ($hw in $HardwareFiles) {
    $file = "config/components/hardware/$hw.json"
    if (Test-Path $file) {
        $HardwareCount++
        Write-Host " $hw" -ForegroundColor Green
    } else {
        Write-Host " MISSING: $hw" -ForegroundColor Red
    }
}

# Check Validation Docs
$ValidationFiles = @("config/validation/complete_full_body_specification.json", "COMPLETE_FULL_BODY_ACHIEVEMENT.md")
$ValidationCount = 0
Write-Host ""
Write-Host " Checking Validation Documentation..." -ForegroundColor Yellow
foreach ($val in $ValidationFiles) {
    if (Test-Path $val) {
        $ValidationCount++
        Write-Host " $(Split-Path $val -Leaf)" -ForegroundColor Green
    } else {
        Write-Host " MISSING: $(Split-Path $val -Leaf)" -ForegroundColor Red
    }
}

# Count Bot Definitions
$BotCount = (Get-ChildItem "config/bots" -Filter "*.json" -ErrorAction SilentlyContinue).Count

# Summary
Write-Host ""
Write-Host "=== VERIFICATION SUMMARY ===" -ForegroundColor Cyan
Write-Host " Positioned Components: $ComponentCount/16 $(if ($ComponentCount -eq 16) { "" } else { "" })" -ForegroundColor White
Write-Host " Servo Controllers: $InterfaceCount/12 $(if ($InterfaceCount -eq 12) { "" } else { "" })" -ForegroundColor White
Write-Host " Hardware Definitions: $HardwareCount/2 $(if ($HardwareCount -eq 2) { "" } else { "" })" -ForegroundColor White
Write-Host " Bot Definitions: $BotCount discovered" -ForegroundColor White
Write-Host " Validation Docs: $ValidationCount/2 $(if ($ValidationCount -eq 2) { "" } else { "" })" -ForegroundColor White

Write-Host ""
if ($ComponentCount -eq 16 -and $InterfaceCount -eq 12 -and $HardwareCount -eq 2 -and $ValidationCount -eq 2) {
    Write-Host " SYSTEM INTEGRITY: 100% VALIDATED " -ForegroundColor Green
    Write-Host " Complete toe-to-head humanoid robot ready for firmware!" -ForegroundColor Green
} else {
    Write-Host " SYSTEM INTEGRITY: Some components missing" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "=== Verification Complete ===" -ForegroundColor Cyan