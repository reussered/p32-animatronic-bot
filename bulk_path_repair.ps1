#!/usr/bin/env powershell

Write-Host "🔧 Bulk Path Mismatch Repair" -ForegroundColor Blue

$configPath = Resolve-Path "config"
$jsonFiles = Get-ChildItem -Path "config" -Recurse -Filter "*.json" -File
$fixedCount = 0

foreach ($file in $jsonFiles) {
    try {
        $json = Get-Content $file.FullName | ConvertFrom-Json -ErrorAction Stop
        
        # Skip if no relative_filename property
        if (-not ($json.PSObject.Properties.Name -contains "relative_filename")) {
            continue
        }
        
        # Calculate correct path
        $correctPath = $file.FullName.Replace($configPath.Path + [IO.Path]::DirectorySeparatorChar, "").Replace([IO.Path]::DirectorySeparatorChar, "/")
        
        # Check if it needs fixing
        if ($json.relative_filename -ne $correctPath) {
            Write-Host "Fixing: $($file.Name) '$($json.relative_filename)' -> '$correctPath'" -ForegroundColor Yellow
            $json.relative_filename = $correctPath
            $json | ConvertTo-Json -Depth 10 | Set-Content $file.FullName -Encoding UTF8
            $fixedCount++
        }
    }
    catch {
        Write-Host "Skipped (invalid JSON): $($file.Name)" -ForegroundColor Red
    }
}

Write-Host "Fixed $fixedCount path mismatches" -ForegroundColor Green