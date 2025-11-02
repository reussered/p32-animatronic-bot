# Convert all JSON files from UTF-8 BOM to UTF-8 without BOM (clean ASCII-compatible)

$projectRoot = "f:\GitHub\p32-animatronic-bot"
$allJsonFiles = Get-ChildItem -Path "$projectRoot\config" -Recurse -Filter "*.json" -File

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "JSON ENCODING CONVERTER" -ForegroundColor Cyan
Write-Host "Converting UTF-8 BOM to UTF-8 (no BOM)" -ForegroundColor Cyan
Write-Host "========================================`n" -ForegroundColor Cyan

$converted = 0
$skipped = 0
$errors = 0

foreach ($file in $allJsonFiles) {
    try {
        # Read with UTF-8 BOM detection
        $content = Get-Content $file.FullName -Raw -Encoding UTF8
        
        # Write back as UTF-8 without BOM
        $utf8NoBom = New-Object System.Text.UTF8Encoding $false
        [System.IO.File]::WriteAllText($file.FullName, $content, $utf8NoBom)
        
        $converted++
        if ($converted % 50 -eq 0) {
            Write-Host "Converted $converted files..." -ForegroundColor Yellow
        }
    }
    catch {
        Write-Host "ERROR: $($file.Name) - $($_.Exception.Message)" -ForegroundColor Red
        $errors++
    }
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "CONVERSION COMPLETE" -ForegroundColor Cyan
Write-Host "========================================`n" -ForegroundColor Cyan

Write-Host "Files Converted:  $converted" -ForegroundColor Green
Write-Host "Errors:           $errors" -ForegroundColor $(if ($errors -eq 0) { "Green" } else { "Red" })

Write-Host "`n========================================`n" -ForegroundColor Cyan
