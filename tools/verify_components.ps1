# Component Verification Script
# Validates all required components for P32 Animatronic Bot

Write-Host "P32 Animatronic Bot - Component Verification" -ForegroundColor Green
Write-Host "===========================================" -ForegroundColor Green
Write-Host ""

$errors = @()
$warnings = @()

# Humanoid positioned components (16 required)
Write-Host "Checking Humanoid Components..." -ForegroundColor Yellow
$humanoidComponents = @(
    "head_assembly.json", "neck_assembly.json", "torso_assembly.json",
    "shoulder_left_assembly.json", "shoulder_right_assembly.json",
    "upper_arm_left_assembly.json", "upper_arm_right_assembly.json", 
    "elbow_left_assembly.json", "elbow_right_assembly.json",
    "forearm_left_assembly.json", "forearm_right_assembly.json",
    "wrist_left_assembly.json", "wrist_right_assembly.json",
    "hip_assembly.json", "ankle_left_assembly.json", "ankle_right_assembly.json"
)

$foundHumanoid = 0
foreach ($component in $humanoidComponents) {
    $path = "config/components/positioned/$component"
    if (Test-Path $path) {
        $foundHumanoid++
        Write-Host "  ✓ $component" -ForegroundColor Green
    } else {
        $errors += "Missing humanoid component: $component"
        Write-Host "  ✗ $component" -ForegroundColor Red
    }
}

Write-Host "Found $foundHumanoid/16 humanoid components" -ForegroundColor $(if($foundHumanoid -eq 16) {"Green"} else {"Red"})
Write-Host ""

# Servo controller interfaces (12 required)
Write-Host "Checking Servo Controllers..." -ForegroundColor Yellow
$servoCount = (Get-ChildItem config/components/interfaces/servo_controller_*.json -ErrorAction SilentlyContinue | Measure-Object).Count
Write-Host "Found $servoCount/12 servo controllers" -ForegroundColor $(if($servoCount -eq 12) {"Green"} else {"Red"})

if ($servoCount -ne 12) {
    $errors += "Expected 12 servo controller interfaces, found $servoCount"
}
Write-Host ""

# Hardware definitions
Write-Host "Checking Hardware Definitions..." -ForegroundColor Yellow
$hardwareFiles = @("servo_assembly.json", "shoulder_joint_assembly.json")
$foundHardware = 0
foreach ($file in $hardwareFiles) {
    $path = "config/components/hardware/$file"
    if (Test-Path $path) {
        $foundHardware++
        Write-Host "  ✓ $file" -ForegroundColor Green
    } else {
        $errors += "Missing hardware definition: $file"
        Write-Host "  ✗ $file" -ForegroundColor Red
    }
}

Write-Host "Found $foundHardware/2 hardware definitions" -ForegroundColor $(if($foundHardware -eq 2) {"Green"} else {"Red"})
Write-Host ""

# Existing goblin components
Write-Host "Checking Existing Goblin Components..." -ForegroundColor Yellow
$goblinComponents = Get-ChildItem config/components/positioned/goblin*.json -ErrorAction SilentlyContinue
Write-Host "Found $($goblinComponents.Count) goblin components" -ForegroundColor Green
foreach ($comp in $goblinComponents) {
    Write-Host "  ✓ $($comp.Name)" -ForegroundColor Green
}
Write-Host ""

# Bot definitions
Write-Host "Checking Bot Definitions..." -ForegroundColor Yellow
$botFiles = Get-ChildItem config/bots/*.json -ErrorAction SilentlyContinue
Write-Host "Found $($botFiles.Count) bot definition(s)" -ForegroundColor Green
foreach ($bot in $botFiles) {
    Write-Host "  ✓ $($bot.Name)" -ForegroundColor Green
}
Write-Host ""

# Summary
Write-Host "===========================================" -ForegroundColor Green
Write-Host "VERIFICATION SUMMARY" -ForegroundColor Green
Write-Host "===========================================" -ForegroundColor Green

if ($errors.Count -eq 0) {
    Write-Host "✓ ALL COMPONENTS VERIFIED" -ForegroundColor Green
    Write-Host "  - $foundHumanoid/16 Humanoid components" -ForegroundColor Green
    Write-Host "  - $servoCount/12 Servo controllers" -ForegroundColor Green  
    Write-Host "  - $foundHardware/2 Hardware definitions" -ForegroundColor Green
    Write-Host "  - $($goblinComponents.Count) Goblin components" -ForegroundColor Green
    Write-Host "  - $($botFiles.Count) Bot definitions" -ForegroundColor Green
} else {
    Write-Host "✗ ISSUES FOUND:" -ForegroundColor Red
    foreach ($err in $errors) {
        Write-Host "  - $err" -ForegroundColor Red
    }
}

if ($warnings.Count -gt 0) {
    Write-Host ""
    Write-Host "WARNINGS:" -ForegroundColor Yellow
    foreach ($warn in $warnings) {
        Write-Host "  - $warn" -ForegroundColor Yellow
    }
}

Write-Host ""
Write-Host "Verification completed at $(Get-Date)" -ForegroundColor Gray