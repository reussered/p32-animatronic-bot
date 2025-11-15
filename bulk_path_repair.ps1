#!/usr/bin/env powershell

Write-Host "? Bulk Path Mismatch Repair" -ForegroundColor Blue

$configPath = Resolve-Path "config"
$jsonFiles = Get-ChildItem -Path "config" -Recurse -Filter "*.json" -File
$fixedCount = 0

foreach ($file in $jsonFiles) {
    try {
        $json = Get-Content $file.FullName | ConvertFrom-Json -ErrorAction Stop
        
        # If property present, remove it (the field is deprecated)
        if ($json.PSObject.Properties.Name -contains "relative_filename") {
            Write-Host "Removing deprecated relative_filename from: $($file.Name) '$($json.relative_filename)'" -ForegroundColor Yellow
            $json.PSObject.Properties.Remove('relative_filename') | Out-Null
            $json | ConvertTo-Json -Depth 10 | Set-Content $file.FullName -Encoding UTF8
            $fixedCount++
        }
    }
    catch {
        Write-Host "Skipped (invalid JSON): $($file.Name)" -ForegroundColor Red
    }
}

Write-Host "Fixed $fixedCount path mismatches" -ForegroundColor Green