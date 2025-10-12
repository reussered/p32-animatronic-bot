Write-Host "P32 Animatronic Bot - Component Verification" -ForegroundColor Green
Write-Host "===========================================" -ForegroundColor Green

# Check humanoid components (16 required)
Write-Host "Checking humanoid components..." -ForegroundColor Yellow
$humanoidCount = (Get-ChildItem config/components/positioned/humanoid*.json | Measure-Object).Count
Write-Host "Found $humanoidCount/16 humanoid components" -ForegroundColor $(if($humanoidCount -eq 16) {"Green"} else {"Red"})

# Check servo controllers (12 required) 
Write-Host "Checking servo controllers..." -ForegroundColor Yellow
$servoCount = (Get-ChildItem config/components/interfaces/servo_controller_*.json | Measure-Object).Count
Write-Host "Found $servoCount/12 servo controllers" -ForegroundColor $(if($servoCount -eq 12) {"Green"} else {"Red"})

# Check hardware definitions (2 required)
Write-Host "Checking hardware definitions..." -ForegroundColor Yellow
$hardwareCount = (Get-ChildItem config/components/hardware/ | Where-Object {$_.Name -match '(servo_assembly|shoulder_joint_assembly)\.json'} | Measure-Object).Count
Write-Host "Found $hardwareCount/2 hardware definitions" -ForegroundColor $(if($hardwareCount -eq 2) {"Green"} else {"Red"})

# Check goblin components
Write-Host "Checking goblin components..." -ForegroundColor Yellow
$goblinCount = (Get-ChildItem config/components/positioned/goblin*.json | Measure-Object).Count
Write-Host "Found $goblinCount goblin components" -ForegroundColor Green

# Check bot definitions
Write-Host "Checking bot definitions..." -ForegroundColor Yellow
$botCount = (Get-ChildItem config/bots/*.json | Measure-Object).Count
Write-Host "Found $botCount bot definitions" -ForegroundColor Green

Write-Host "===========================================" -ForegroundColor Green
if ($humanoidCount -eq 16 -and $servoCount -eq 12 -and $hardwareCount -eq 2) {
    Write-Host "ALL CORE COMPONENTS VERIFIED!" -ForegroundColor Green
} else {
    Write-Host "MISSING COMPONENTS DETECTED!" -ForegroundColor Red
}
Write-Host "===========================================" -ForegroundColor Green