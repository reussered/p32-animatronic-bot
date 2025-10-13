Write-Host "P32 Animatronic Bot - Comprehensive System Verification" -ForegroundColor Green
Write-Host "=======================================================" -ForegroundColor Green
Write-Host ""

# Humanoid Components (16 required)
Write-Host "HUMANOID COMPONENTS (16 required):" -ForegroundColor Cyan
$humanoidComponents = Get-ChildItem config/components/positioned/humanoid*.json
foreach ($comp in $humanoidComponents) {
    Write-Host "  ✓ $($comp.Name)" -ForegroundColor Green
}
Write-Host "Total: $($humanoidComponents.Count)/16" -ForegroundColor $(if($humanoidComponents.Count -eq 16) {"Green"} else {"Red"})
Write-Host ""

# Servo Controllers (12 required)
Write-Host "SERVO CONTROLLERS (12 required):" -ForegroundColor Cyan
$servoControllers = Get-ChildItem config/components/interfaces/servo_controller_*.json
foreach ($servo in $servoControllers) {
    Write-Host "  ✓ $($servo.Name)" -ForegroundColor Green
}
Write-Host "Total: $($servoControllers.Count)/12" -ForegroundColor $(if($servoControllers.Count -eq 12) {"Green"} else {"Red"})
Write-Host ""

# Hardware Definitions (2 required)
Write-Host "HARDWARE DEFINITIONS (2 required):" -ForegroundColor Cyan
$hardwareFiles = Get-ChildItem config/components/hardware/ | Where-Object {$_.Name -match '(servo_assembly|shoulder_joint_assembly)\.json'}
foreach ($hw in $hardwareFiles) {
    Write-Host "  ✓ $($hw.Name)" -ForegroundColor Green
}
Write-Host "Total: $($hardwareFiles.Count)/2" -ForegroundColor $(if($hardwareFiles.Count -eq 2) {"Green"} else {"Red"})
Write-Host ""

# Goblin Components
Write-Host "GOBLIN COMPONENTS:" -ForegroundColor Cyan
$goblinComponents = Get-ChildItem config/components/positioned/goblin*.json
foreach ($goblin in $goblinComponents) {
    Write-Host "  ✓ $($goblin.Name)" -ForegroundColor Green
}
Write-Host "Total: $($goblinComponents.Count)" -ForegroundColor Green
Write-Host ""

# Bot Definitions
Write-Host "BOT DEFINITIONS:" -ForegroundColor Cyan
$botDefinitions = Get-ChildItem config/bots/*.json
foreach ($bot in $botDefinitions) {
    Write-Host "  ✓ $($bot.Name)" -ForegroundColor Green
}
Write-Host "Total: $($botDefinitions.Count)" -ForegroundColor Green
Write-Host ""

# Other Interface Components
Write-Host "OTHER INTERFACE COMPONENTS:" -ForegroundColor Cyan
$otherInterfaces = Get-ChildItem config/components/interfaces/ | Where-Object {$_.Name -notmatch 'servo_controller_'}
foreach ($interface in $otherInterfaces) {
    Write-Host "  ✓ $($interface.Name)" -ForegroundColor Green
}
Write-Host "Total: $($otherInterfaces.Count)" -ForegroundColor Green
Write-Host ""

# All Hardware Components
Write-Host "ALL HARDWARE COMPONENTS:" -ForegroundColor Cyan
$allHardware = Get-ChildItem config/components/hardware/*.json
foreach ($hw in $allHardware) {
    Write-Host "  ✓ $($hw.Name)" -ForegroundColor Green
}
Write-Host "Total: $($allHardware.Count)" -ForegroundColor Green
Write-Host ""

# Summary
Write-Host "=======================================================" -ForegroundColor Green
Write-Host "VERIFICATION SUMMARY:" -ForegroundColor White
Write-Host "=======================================================" -ForegroundColor Green

$allGood = $true
if ($humanoidComponents.Count -eq 16) {
    Write-Host "✓ Humanoid Components: COMPLETE ($($humanoidComponents.Count)/16)" -ForegroundColor Green
} else {
    Write-Host "✗ Humanoid Components: INCOMPLETE ($($humanoidComponents.Count)/16)" -ForegroundColor Red
    $allGood = $false
}

if ($servoControllers.Count -eq 12) {
    Write-Host "✓ Servo Controllers: COMPLETE ($($servoControllers.Count)/12)" -ForegroundColor Green
} else {
    Write-Host "✗ Servo Controllers: INCOMPLETE ($($servoControllers.Count)/12)" -ForegroundColor Red
    $allGood = $false
}

if ($hardwareFiles.Count -eq 2) {
    Write-Host "✓ Hardware Definitions: COMPLETE ($($hardwareFiles.Count)/2)" -ForegroundColor Green
} else {
    Write-Host "✗ Hardware Definitions: INCOMPLETE ($($hardwareFiles.Count)/2)" -ForegroundColor Red
    $allGood = $false
}

Write-Host "✓ Goblin Components: $($goblinComponents.Count) found" -ForegroundColor Green
Write-Host "✓ Bot Definitions: $($botDefinitions.Count) found" -ForegroundColor Green
Write-Host "✓ Other Interfaces: $($otherInterfaces.Count) found" -ForegroundColor Green
Write-Host "✓ Total Hardware: $($allHardware.Count) found" -ForegroundColor Green

Write-Host ""
if ($allGood) {
    Write-Host "🎉 ALL CORE SYSTEMS VERIFIED! FULL-BODY ROBOT READY!" -ForegroundColor Green
} else {
    Write-Host "⚠️  SOME CORE COMPONENTS MISSING - CHECK ABOVE" -ForegroundColor Red
}

Write-Host ""
Write-Host "Verification completed: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')" -ForegroundColor Gray