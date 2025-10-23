Write-Host "QUICK FIX: Adding config/ prefix to all relative_filename fields" -ForegroundColor Green

$configPath = Resolve-Path "config"
$jsonFiles = Get-ChildItem -Path "config" -Recurse -Filter "*.json" -File
$fixedCount = 0

foreach ($file in $jsonFiles) {
    try {
        $json = Get-Content $file.FullName | ConvertFrom-Json -ErrorAction Stop
        
        if (-not ($json.PSObject.Properties.Name -contains "relative_filename")) {
            continue
        }
        
        # Get the path relative to project root (includes config/)
        $projectRoot = Split-Path $configPath.Path -Parent
        $correctPath = $file.FullName.Replace($projectRoot + [IO.Path]::DirectorySeparatorChar, "").Replace([IO.Path]::DirectorySeparatorChar, "/")
        
        if ($json.relative_filename -ne $correctPath) {
            $json.relative_filename = $correctPath
            $json | ConvertTo-Json -Depth 10 | Set-Content $file.FullName -Encoding UTF8
            $fixedCount++
        }
    }
    catch {
        Write-Host "Skipped invalid JSON: $($file.Name)" -ForegroundColor Red
    }
}

Write-Host "COMPLETE: Fixed $fixedCount path mismatches" -ForegroundColor Green