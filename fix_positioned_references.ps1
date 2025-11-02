# fix_positioned_references.ps1
# Fix all positioned/ references in JSON files

$projectRoot = "F:\GitHub\p32-animatronic-bot"
Write-Host "=== Fixing positioned/ References ===" -ForegroundColor Cyan
Write-Host ""

$fixed = 0
$errors = @()

# Find all JSON files in config/
$jsonFiles = Get-ChildItem -Path "$projectRoot\config" -Filter "*.json" -Recurse -File

Write-Host "Found $($jsonFiles.Count) JSON files to check" -ForegroundColor Yellow
Write-Host ""

foreach ($file in $jsonFiles) {
    try {
        $content = Get-Content $file.FullName -Raw -Encoding UTF8
        
        if ($content -match 'positioned/') {
            $originalContent = $content
            
            # Replace all variations of positioned/ path
            $content = $content -replace 'config/components/positioned/', 'config/components/creature_specific/'
            $content = $content -replace 'config\\components\\positioned\\', 'config\components\creature_specific\'
            $content = $content -replace '"positioned/', '"creature_specific/'
            
            if ($content -ne $originalContent) {
                # Save as ASCII to prevent BOM issues
                [IO.File]::WriteAllText($file.FullName, $content, [Text.Encoding]::ASCII)
                $fixed++
                $relativePath = $file.FullName.Replace($projectRoot + "\", "")
                Write-Host "[FIXED] $relativePath" -ForegroundColor Green
            }
        }
    }
    catch {
        $errors += "Failed to process $($file.FullName): $_"
        Write-Host "[ERROR] $($file.FullName): $_" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "=== SUMMARY ===" -ForegroundColor Cyan
Write-Host "Files fixed: $fixed" -ForegroundColor Green
Write-Host "Errors:      $($errors.Count)" -ForegroundColor $(if ($errors.Count -eq 0) { "Green" } else { "Red" })

if ($errors.Count -gt 0) {
    Write-Host ""
    Write-Host "=== ERRORS ===" -ForegroundColor Red
    foreach ($err in $errors) {
        Write-Host "  $err" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "Verifying fix..." -ForegroundColor Yellow
$remaining = 0
foreach ($file in $jsonFiles) {
    $content = Get-Content $file.FullName -Raw
    if ($content -match 'positioned/') {
        $remaining++
        Write-Host "[!] Still contains positioned/: $($file.Name)" -ForegroundColor Yellow
    }
}

if ($remaining -eq 0) {
    Write-Host "[OK] All positioned/ references removed!" -ForegroundColor Green
} else {
    Write-Host "[!] $remaining files still contain positioned/ references" -ForegroundColor Yellow
}
