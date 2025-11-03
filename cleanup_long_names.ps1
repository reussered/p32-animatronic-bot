# cleanup_long_names.ps1
# Clean up old long-named files and keep only short names

$projectRoot = "F:\GitHub\p32-animatronic-bot"
Write-Host "=== Cleaning Up Long Display Names ===" -ForegroundColor Cyan
Write-Host ""

# Define the mappings from long names to short names
$mappings = @{
    "gc9a01_circular_lcd_display" = "gc9a01"
    "component_2_5x1_5_inch_tft_lcd_display" = "st7735"
    "hc_sr04_ultrasonic_distance_sensor" = "hc_sr04"
}

$deletedFiles = @()
$errors = @()

# Step 1: Find all files with long names
Write-Host "Phase 1: Finding files with long names..." -ForegroundColor Yellow
foreach ($longName in $mappings.Keys) {
    $shortName = $mappings[$longName]
    Write-Host "  Mapping: $longName -> $shortName" -ForegroundColor White
    
    # Find .src files
    $srcFiles = Get-ChildItem -Path $projectRoot -Filter "$longName*.src" -Recurse -File -ErrorAction SilentlyContinue
    foreach ($file in $srcFiles) {
        Write-Host "    Found: $($file.FullName)" -ForegroundColor Gray
    }
    
    # Find .hdr files
    $hdrFiles = Get-ChildItem -Path $projectRoot -Filter "$longName*.hdr" -Recurse -File -ErrorAction SilentlyContinue
    foreach ($file in $hdrFiles) {
        Write-Host "    Found: $($file.FullName)" -ForegroundColor Gray
    }
    
    # Find .json files
    $jsonFiles = Get-ChildItem -Path $projectRoot -Filter "$longName*.json" -Recurse -File -ErrorAction SilentlyContinue
    foreach ($file in $jsonFiles) {
        Write-Host "    Found: $($file.FullName)" -ForegroundColor Gray
    }
}

Write-Host ""
Write-Host "Phase 2: Verifying short-named files exist..." -ForegroundColor Yellow

$shortFilesExist = @{}
foreach ($longName in $mappings.Keys) {
    $shortName = $mappings[$longName]
    
    # Check for short-named files in config/components/hardware/
    $shortSrc = "$projectRoot\config\components\hardware\$shortName.src"
    $shortHdr = "$projectRoot\config\components\hardware\$shortName.hdr"
    $shortJson = "$projectRoot\config\components\hardware\$shortName.json"
    
    if (Test-Path $shortSrc) {
        Write-Host "  [OK] $shortSrc exists" -ForegroundColor Green
        $shortFilesExist[$shortName] = $true
    } else {
        Write-Host "  [!] $shortSrc MISSING" -ForegroundColor Red
        $shortFilesExist[$shortName] = $false
    }
    
    if (Test-Path $shortHdr) {
        Write-Host "  [OK] $shortHdr exists" -ForegroundColor Green
    } else {
        Write-Host "  [!] $shortHdr MISSING (may be optional)" -ForegroundColor Yellow
    }
    
    if (Test-Path $shortJson) {
        Write-Host "  [OK] $shortJson exists" -ForegroundColor Green
    } else {
        Write-Host "  [!] $shortJson MISSING" -ForegroundColor Red
        $shortFilesExist[$shortName] = $false
    }
}

Write-Host ""
$proceed = Read-Host "Delete all long-named files? (yes/no)"
if ($proceed -ne "yes") {
    Write-Host "Aborted by user" -ForegroundColor Yellow
    exit 0
}

Write-Host ""
Write-Host "Phase 3: Deleting long-named files..." -ForegroundColor Yellow

foreach ($longName in $mappings.Keys) {
    $shortName = $mappings[$longName]
    
    if (-not $shortFilesExist[$shortName]) {
        Write-Host "  [SKIP] $longName - short name files don't exist, keeping long names" -ForegroundColor Yellow
        continue
    }
    
    # Delete .src files
    $srcFiles = Get-ChildItem -Path $projectRoot -Filter "$longName*.src" -Recurse -File -ErrorAction SilentlyContinue
    foreach ($file in $srcFiles) {
        try {
            Remove-Item $file.FullName -Force
            $deletedFiles += $file.FullName
            Write-Host "  [DELETED] $($file.FullName)" -ForegroundColor Red
        }
        catch {
            $errors += "Failed to delete $($file.FullName): $_"
            Write-Host "  [ERROR] $($file.FullName): $_" -ForegroundColor Red
        }
    }
    
    # Delete .hdr files
    $hdrFiles = Get-ChildItem -Path $projectRoot -Filter "$longName*.hdr" -Recurse -File -ErrorAction SilentlyContinue
    foreach ($file in $hdrFiles) {
        try {
            Remove-Item $file.FullName -Force
            $deletedFiles += $file.FullName
            Write-Host "  [DELETED] $($file.FullName)" -ForegroundColor Red
        }
        catch {
            $errors += "Failed to delete $($file.FullName): $_"
            Write-Host "  [ERROR] $($file.FullName): $_" -ForegroundColor Red
        }
    }
    
    # Delete .json files (but NOT in config/components/hardware/ - those should be renamed not deleted)
    $jsonFiles = Get-ChildItem -Path $projectRoot -Filter "$longName*.json" -Recurse -File -ErrorAction SilentlyContinue
    foreach ($file in $jsonFiles) {
        if ($file.FullName -notlike "*\config\components\hardware\*") {
            try {
                Remove-Item $file.FullName -Force
                $deletedFiles += $file.FullName
                Write-Host "  [DELETED] $($file.FullName)" -ForegroundColor Red
            }
            catch {
                $errors += "Failed to delete $($file.FullName): $_"
                Write-Host "  [ERROR] $($file.FullName): $_" -ForegroundColor Red
            }
        }
    }
}

Write-Host ""
Write-Host "Phase 4: Verifying cleanup..." -ForegroundColor Yellow

# Walk the tree to verify only short names remain
$remainingLongFiles = @()
foreach ($longName in $mappings.Keys) {
    $srcFiles = Get-ChildItem -Path $projectRoot -Filter "$longName*.src" -Recurse -File -ErrorAction SilentlyContinue
    $hdrFiles = Get-ChildItem -Path $projectRoot -Filter "$longName*.hdr" -Recurse -File -ErrorAction SilentlyContinue
    $jsonFiles = Get-ChildItem -Path $projectRoot -Filter "$longName*.json" -Recurse -File -ErrorAction SilentlyContinue
    
    foreach ($file in ($srcFiles + $hdrFiles + $jsonFiles)) {
        $remainingLongFiles += $file.FullName
        Write-Host "  [!] STILL EXISTS: $($file.FullName)" -ForegroundColor Yellow
    }
}

Write-Host ""
Write-Host "=== CLEANUP SUMMARY ===" -ForegroundColor Cyan
Write-Host "Files deleted:        $($deletedFiles.Count)" -ForegroundColor $(if ($deletedFiles.Count -gt 0) { "Green" } else { "White" })
Write-Host "Files remaining:      $($remainingLongFiles.Count)" -ForegroundColor $(if ($remainingLongFiles.Count -eq 0) { "Green" } else { "Yellow" })
Write-Host "Errors:               $($errors.Count)" -ForegroundColor $(if ($errors.Count -eq 0) { "Green" } else { "Red" })

if ($errors.Count -gt 0) {
    Write-Host ""
    Write-Host "=== ERRORS ===" -ForegroundColor Red
    foreach ($err in $errors) {
        Write-Host "  $err" -ForegroundColor Red
    }
}

if ($remainingLongFiles.Count -gt 0) {
    Write-Host ""
    Write-Host "=== REMAINING LONG NAMES ===" -ForegroundColor Yellow
    foreach ($file in $remainingLongFiles) {
        Write-Host "  $file" -ForegroundColor Yellow
    }
}

if ($remainingLongFiles.Count -eq 0 -and $errors.Count -eq 0) {
    Write-Host ""
    Write-Host "=== CLEANUP COMPLETE ===" -ForegroundColor Green
}
