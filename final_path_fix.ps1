Write-Host "Final Path Correction - Adding config/ prefix" -ForegroundColor Blue

$configPath = Resolve-Path "config"
$jsonFiles = Get-ChildItem -Path "config" -Recurse -Filter "*.json" -File
$fixedCount = 0

foreach ($file in $jsonFiles) {
    try {
        $json = Get-Content $file.FullName | ConvertFrom-Json -ErrorAction Stop
        
        if (-not ($json.PSObject.Properties.Name -contains "relative_filename")) {
            continue
        }

        # Remove deprecated relative_filename field
        Write-Host "Removing deprecated relative_filename from: $($file.Name) '$($json.relative_filename)'" -ForegroundColor Yellow
        $json.PSObject.Properties.Remove('relative_filename') | Out-Null
        $json | ConvertTo-Json -Depth 10 | Set-Content $file.FullName -Encoding ASCII
        $fixedCount++
    }
    catch {
        Write-Host "Skipped (invalid JSON): $($file.Name)" -ForegroundColor Red
    }
}

Write-Host "Fixed $fixedCount final path mismatches" -ForegroundColor Green