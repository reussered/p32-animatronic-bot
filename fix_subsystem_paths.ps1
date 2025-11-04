# Fix the relative_filename paths in the migrated subsystem files

$root = "f:\GitHub\p32-animatronic-bot"
$botFamiliesDir = "$root\config\bots\bot_families"

Write-Host "=== Fixing relative_filename paths ===" -ForegroundColor Cyan

$jsonFiles = Get-ChildItem -Path "$botFamiliesDir\*/subsystems/*.json" -Recurse

foreach ($jsonFile in $jsonFiles) {
    $relPath = $jsonFile.FullName.Substring($root.Length + 1).Replace("\", "/")
    
    # Read, fix, and write
    $content = Get-Content -Path $jsonFile.FullName -Raw
    $content = $content -replace '"relative_filename":\s*"[^"]*"', ('"relative_filename": "' + $relPath + '"')
    
    Set-Content -Path $jsonFile.FullName -Value $content -Encoding UTF8
    Write-Host "  Fixed: $relPath"
}

Write-Host "Done!" -ForegroundColor Green
