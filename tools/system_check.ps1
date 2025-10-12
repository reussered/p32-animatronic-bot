Write-Host "P32 Animatronic Bot - Full System Verification" -ForegroundColor Green
Write-Host "=============================================" -ForegroundColor Green
Write-Host ""

# Humanoid Components
Write-Host "HUMANOID COMPONENTS - 16 required:" -ForegroundColor Cyan
$humanoidComponents = Get-ChildItem config/components/positioned/humanoid*.json
foreach ($comp in $humanoidComponents) {
    Write-Host "  OK $($comp.Name)" -ForegroundColor Green
}
Write-Host "Found: $($humanoidComponents.Count) of 16" -ForegroundColor $(if($humanoidComponents.Count -eq 16) {"Green"} else {"Red"})
Write-Host ""

# Servo Controllers  
Write-Host "SERVO CONTROLLERS - 12 required:" -ForegroundColor Cyan
$servoControllers = Get-ChildItem config/components/interfaces/servo_controller_*.json
foreach ($servo in $servoControllers) {
    Write-Host "  OK $($servo.Name)" -ForegroundColor Green
}
Write-Host "Found: $($servoControllers.Count) of 12" -ForegroundColor $(if($servoControllers.Count -eq 12) {"Green"} else {"Red"})
Write-Host ""

# Hardware Definitions
Write-Host "HARDWARE DEFINITIONS - 2 required:" -ForegroundColor Cyan
$hardwareFiles = Get-ChildItem config/components/hardware/ | Where-Object {$_.Name -match "(servo_assembly|shoulder_joint_assembly).json"}
foreach ($hw in $hardwareFiles) {
    Write-Host "  OK $($hw.Name)" -ForegroundColor Green
}
Write-Host "Found: $($hardwareFiles.Count) of 2" -ForegroundColor $(if($hardwareFiles.Count -eq 2) {"Green"} else {"Red"})
Write-Host ""

# Goblin Components
Write-Host "GOBLIN COMPONENTS:" -ForegroundColor Cyan
$goblinComponents = Get-ChildItem config/components/positioned/goblin*.json
foreach ($goblin in $goblinComponents) {
    Write-Host "  OK $($goblin.Name)" -ForegroundColor Green
}
Write-Host "Found: $($goblinComponents.Count)" -ForegroundColor Green
Write-Host ""

# Summary
Write-Host "=============================================" -ForegroundColor Green
Write-Host "FINAL VERIFICATION RESULTS:" -ForegroundColor White
Write-Host "=============================================" -ForegroundColor Green

$success = $true
if ($humanoidComponents.Count -eq 16) {
    Write-Host "SUCCESS: All 16 humanoid components present" -ForegroundColor Green
} else {
    Write-Host "ERROR: Missing humanoid components - $($humanoidComponents.Count) of 16 found" -ForegroundColor Red
    $success = $false
}

if ($servoControllers.Count -eq 12) {
    Write-Host "SUCCESS: All 12 servo controllers present" -ForegroundColor Green
} else {
    Write-Host "ERROR: Missing servo controllers - $($servoControllers.Count) of 12 found" -ForegroundColor Red
    $success = $false
}

if ($hardwareFiles.Count -eq 2) {
    Write-Host "SUCCESS: All 2 hardware definitions present" -ForegroundColor Green
} else {
    Write-Host "ERROR: Missing hardware definitions - $($hardwareFiles.Count) of 2 found" -ForegroundColor Red
    $success = $false
}

Write-Host "INFO: $($goblinComponents.Count) goblin components found" -ForegroundColor Green

Write-Host ""
if ($success) {
    Write-Host "OVERALL STATUS: ALL CORE COMPONENTS VERIFIED!" -ForegroundColor Green
    Write-Host "The full-body humanoid robot system is complete." -ForegroundColor Green
} else {
    Write-Host "OVERALL STATUS: MISSING COMPONENTS DETECTED!" -ForegroundColor Red
    Write-Host "Please check the errors above." -ForegroundColor Red
}

$timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
Write-Host ""
Write-Host "Verification completed: $timestamp" -ForegroundColor Gray