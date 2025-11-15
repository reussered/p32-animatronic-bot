# Fix JSON metadata: ensure name matches filename and relative_filename is correct
# This script updates all JSON files in config/ to have correct metadata

$fixed = 0
$skipped = 0
$errors = 0

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "JSON Metadata Fixer" -ForegroundColor Cyan
Write-Host "========================================`n" -ForegroundColor Cyan

Get-ChildItem -Path "f:\GitHub\p32-animatronic-bot\config" -Recurse -Filter "*.json" -File | ForEach-Object {
    $file = $_
    $filename = $file.BaseName
    # Use case-insensitive replace for both drive letter cases
    $relativePath = $file.FullName -replace '^[fF]:\\GitHub\\p32-animatronic-bot\\', '' -replace '\\', '/'
    
    try {
        # Read JSON
        $content = Get-Content $file.FullName -Raw -ErrorAction Stop
        $json = $content | ConvertFrom-Json -ErrorAction Stop
        
        $needsUpdate = $false
        
        # Check if name field exists and matches filename
        if (-not $json.name) {
            # Add name field
            $json | Add-Member -MemberType NoteProperty -Name "name" -Value $filename -Force
            $needsUpdate = $true
            Write-Host "  [ADD NAME] $relativePath" -ForegroundColor Green
        }
        elseif ($json.name -ne $filename) {
            # Update name field
            $json.name = $filename
            $needsUpdate = $true
            Write-Host "  [FIX NAME] $relativePath (was: $($json.name))" -ForegroundColor Yellow
        }
        
        # Deprecated: remove any relative_filename entries
        if ($json.PSObject.Properties.Name -contains "relative_filename") {
            Write-Host "  [REMOVE PATH METADATA] $relativePath (was: $($json.relative_filename))" -ForegroundColor Yellow
            $json.PSObject.Properties.Remove('relative_filename') | Out-Null
            $needsUpdate = $true
        }
        
        # Save if changes were made
        if ($needsUpdate) {
            $json | ConvertTo-Json -Depth 100 | Set-Content $file.FullName -Encoding UTF8
            $fixed++
        }
        else {
            $skipped++
        }
    }
    catch {
        Write-Host "  [ERROR] $relativePath : $_" -ForegroundColor Red
        $errors++
    }
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "Summary:" -ForegroundColor Cyan
Write-Host "  Fixed: $fixed files" -ForegroundColor Green
Write-Host "  Skipped (already correct): $skipped files" -ForegroundColor Gray
Write-Host "  Errors: $errors files" -ForegroundColor Red
Write-Host "========================================`n" -ForegroundColor Cyan
