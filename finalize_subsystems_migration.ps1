# Move base template subsystems and remove old subsystems directory

$root = "f:\GitHub\p32-animatronic-bot"
$subsystemsDir = "$root\config\subsystems"
$baseDir = "$subsystemsDir\base"
$templatesDir = "$root\config\templates\base"

Write-Host "=== Finalizing subsystems migration ===" -ForegroundColor Cyan

# Step 1: Move base template files
Write-Host "`n[Step 1] Moving base template files..." -ForegroundColor Yellow

if (Test-Path $baseDir) {
    if (-not (Test-Path $templatesDir)) {
        New-Item -ItemType Directory -Path $templatesDir -Force | Out-Null
        Write-Host "  Created: $templatesDir"
    }
    
    Get-ChildItem -Path $baseDir -Filter "*.json" | ForEach-Object {
        $sourceFile = $_.FullName
        $targetFile = "$templatesDir\$($_.Name)"
        Copy-Item -Path $sourceFile -Destination $targetFile -Force
        Write-Host "  Copied: $($_.Name) to templates/base/"
    }
}

# Step 2: Remove the old subsystems directory
Write-Host "`n[Step 2] Removing old subsystems directory..." -ForegroundColor Yellow

if (Test-Path $subsystemsDir) {
    Remove-Item -Path $subsystemsDir -Recurse -Force
    Write-Host "  Deleted: $subsystemsDir" -ForegroundColor Green
}

Write-Host "`n=== Migration Complete ===" -ForegroundColor Green
Write-Host "Next step: Run validation and generate_tables.py"
