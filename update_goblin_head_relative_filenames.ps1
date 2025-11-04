# Update relative_filename fields in all goblin head components
Get-ChildItem -Path "f:\GitHub\p32-animatronic-bot\config\bots\bot_families\goblins\head" -Filter "*.json" | ForEach-Object {
    $filePath = $_.FullName
    $fileName = $_.Name
    $content = Get-Content $filePath -Raw
    $newRelativeFilename = "config/bots/bot_families/goblins/head/$fileName"
    $newContent = $content -replace '"relative_filename":\s*"[^"]*"', "`"relative_filename`": `"$newRelativeFilename`""
    Set-Content -Path $filePath -Value $newContent
    Write-Host "Updated $fileName"
}