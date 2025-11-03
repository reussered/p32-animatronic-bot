# final_verification.ps1
# Comprehensive verification of P32 project after cleanup

$projectRoot = "F:\GitHub\p32-animatronic-bot"
Write-Host "=== P32 FINAL VERIFICATION ===" -ForegroundColor Cyan
Write-Host ""

$errors = @()
$warnings = @()

# 1. Verify no long display names exist
Write-Host "Phase 1: Checking for long display names..." -ForegroundColor Yellow
$longNames = @(
    "gc9a01_circular_lcd_display",
    "component_2_5x1_5_inch_tft_lcd_display",
    "hc_sr04_ultrasonic_distance_sensor"
)

foreach ($name in $longNames) {
    $files = Get-ChildItem -Path $projectRoot -Filter "*$name*" -Recurse -File -ErrorAction SilentlyContinue
    foreach ($file in $files) {
        # Allow in documentation/CSV files
        if ($file.Extension -match '\.(md|txt|csv|log)$') {
            $warnings += "Long name in doc: $($file.FullName)"
            Write-Host "  [!] $($file.FullName) (documentation)" -ForegroundColor Yellow
        }
        else {
            $errors += "Long name found: $($file.FullName)"
            Write-Host "  [X] $($file.FullName)" -ForegroundColor Red
        }
    }
}

if ($errors.Count -eq 0) {
    Write-Host "  [OK] No long display names in source files" -ForegroundColor Green
}

Write-Host ""

# 2. Verify short-named files exist
Write-Host "Phase 2: Verifying short-named hardware components..." -ForegroundColor Yellow
$shortNames = @("gc9a01", "st7735", "hc_sr04")
$hardwarePath = Join-Path $projectRoot "config\components\hardware"

foreach ($name in $shortNames) {
    $srcFile = Join-Path $hardwarePath "$name.src"
    $jsonFile = Join-Path $hardwarePath "$name.json"
    
    if (Test-Path $srcFile) {
        Write-Host "  [OK] $name.src exists" -ForegroundColor Green
    }
    else {
        $errors += "Missing: $srcFile"
        Write-Host "  [X] $name.src MISSING" -ForegroundColor Red
    }
    
    if (Test-Path $jsonFile) {
        Write-Host "  [OK] $name.json exists" -ForegroundColor Green
    }
    else {
        $errors += "Missing: $jsonFile"
        Write-Host "  [X] $name.json MISSING" -ForegroundColor Red
    }
}

Write-Host ""

# 3. Check for positioned/ references in component paths (not asset paths)
Write-Host "Phase 3: Checking for obsolete 'positioned/' references..." -ForegroundColor Yellow
$jsonFiles = Get-ChildItem -Path (Join-Path $projectRoot "config") -Filter "*.json" -Recurse -File
$positionedRefs = 0

foreach ($file in $jsonFiles) {
    $content = Get-Content $file.FullName -Raw
    # Check for positioned/ in component paths, but not in asset paths
    if ($content -match 'config[/\\]components[/\\]positioned[/\\]') {
        $positionedRefs++
        $relativePath = $file.FullName.Replace($projectRoot + "\", "")
        $errors += "Positioned component reference: $relativePath"
        Write-Host "  [X] $relativePath contains 'positioned/' in component path" -ForegroundColor Red
    }
}

if ($positionedRefs -eq 0) {
    Write-Host "  [OK] No obsolete 'positioned/' component references" -ForegroundColor Green
}

Write-Host ""

# 4. Verify component file organization
Write-Host "Phase 4: Verifying component file organization..." -ForegroundColor Yellow
$componentPath = Join-Path $projectRoot "config\components"

# Check drivers folder
$driversPath = Join-Path $componentPath "drivers"
if (Test-Path $driversPath) {
    $driverFiles = Get-ChildItem -Path $driversPath -Filter "*.src" -File
    Write-Host "  [OK] drivers/ folder exists ($($driverFiles.Count) .src files)" -ForegroundColor Green
}
else {
    $errors += "Missing: $driversPath"
    Write-Host "  [X] drivers/ folder MISSING" -ForegroundColor Red
}

# Check hardware folder
$hardwarePath = Join-Path $componentPath "hardware"
if (Test-Path $hardwarePath) {
    $hardwareFiles = Get-ChildItem -Path $hardwarePath -Filter "*.src" -File
    Write-Host "  [OK] hardware/ folder exists ($($hardwareFiles.Count) .src files)" -ForegroundColor Green
}
else {
    $errors += "Missing: $hardwarePath"
    Write-Host "  [X] hardware/ folder MISSING" -ForegroundColor Red
}

# Check creature_specific folder
$creaturePath = Join-Path $componentPath "creature_specific"
if (Test-Path $creaturePath) {
    $creatureFiles = Get-ChildItem -Path $creaturePath -Filter "*.src" -File
    Write-Host "  [OK] creature_specific/ folder exists ($($creatureFiles.Count) .src files)" -ForegroundColor Green
}
else {
    $errors += "Missing: $creaturePath"
    Write-Host "  [X] creature_specific/ folder MISSING" -ForegroundColor Red
}

# Verify positioned/ folder does NOT exist
$positionedPath = Join-Path $componentPath "positioned"
if (Test-Path $positionedPath) {
    $errors += "Obsolete folder exists: $positionedPath"
    Write-Host "  [X] positioned/ folder still exists (should be deleted)" -ForegroundColor Red
}
else {
    Write-Host "  [OK] positioned/ folder correctly deleted" -ForegroundColor Green
}

Write-Host ""

# 5. Test generate_tables.py
Write-Host "Phase 5: Testing generate_tables.py..." -ForegroundColor Yellow
try {
    $output = & python "tools\generate_tables.py" "config\bots\test_bot.json" "src" 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  [OK] generate_tables.py completed successfully" -ForegroundColor Green
        Write-Host "       $output" -ForegroundColor Gray
    }
    else {
        $errors += "generate_tables.py failed with exit code $LASTEXITCODE"
        Write-Host "  [X] generate_tables.py FAILED" -ForegroundColor Red
        Write-Host "       $output" -ForegroundColor Red
    }
}
catch {
    $errors += "generate_tables.py exception: $_"
    Write-Host "  [X] generate_tables.py EXCEPTION: $_" -ForegroundColor Red
}

Write-Host ""

# Summary
Write-Host "=== VERIFICATION SUMMARY ===" -ForegroundColor Cyan
Write-Host "Errors:   $($errors.Count)" -ForegroundColor $(if ($errors.Count -eq 0) { "Green" } else { "Red" })
Write-Host "Warnings: $($warnings.Count)" -ForegroundColor $(if ($warnings.Count -eq 0) { "Green" } else { "Yellow" })
Write-Host ""

if ($errors.Count -gt 0) {
    Write-Host "=== ERRORS ===" -ForegroundColor Red
    foreach ($err in $errors) {
        Write-Host "  $err" -ForegroundColor Red
    }
    Write-Host ""
}

if ($warnings.Count -gt 0) {
    Write-Host "=== WARNINGS ===" -ForegroundColor Yellow
    foreach ($warn in $warnings) {
        Write-Host "  $warn" -ForegroundColor Yellow
    }
    Write-Host ""
}

if ($errors.Count -eq 0) {
    Write-Host "=== ALL VERIFICATIONS PASSED ===" -ForegroundColor Green
    exit 0
}
else {
    Write-Host "=== VERIFICATION FAILED ===" -ForegroundColor Red
    exit 1
}
