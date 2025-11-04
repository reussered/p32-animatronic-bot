# Update relative_filename fields in all component JSON files recursively
Get-ChildItem -Path "f:\GitHub\p32-animatronic-bot\config\bots\bot_families" -Filter "*.json" -Recurse | ForEach-Object {
    $filePath = $_.FullName
    $fileName = $_.Name

    # Get the relative path from config/bots/bot_families/
    $relativePath = $filePath -replace "f:\\GitHub\\p32-animatronic-bot\\config\\bots\\bot_families\\", ""

    # Skip if it's a family or bot definition file (not a component)
    if ($relativePath -notmatch "\\") {
        return
    }

    $newRelativeFilename = "config/bots/bot_families/$relativePath"

    $content = Get-Content $filePath -Raw
    $newContent = $content -replace '"relative_filename":\s*"[^"]*"', "`"relative_filename`": `"$newRelativeFilename`""
    Set-Content -Path $filePath -Value $newContent
    Write-Host "Updated $fileName -> $newRelativeFilename"
}