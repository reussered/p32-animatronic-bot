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
        
        # Calculate correct path that includes "config/" prefix
        $fullRelativePath = $file.FullName.Replace($configPath.Path.Replace("config", "") + [IO.Path]::DirectorySeparatorChar, "").Replace([IO.Path]::DirectorySeparatorChar, "/")
        
        if ($json.relative_filename -ne $fullRelativePath) {
            Write-Host "Fixing: $($file.Name) '$($json.relative_filename)' -> '$fullRelativePath'" -ForegroundColor Yellow
            $json.relative_filename = $fullRelativePath
            $json | ConvertTo-Json -Depth 10 | Set-Content $file.FullName -Encoding UTF8
            $fixedCount++
        }
    }
    catch {
        Write-Host "Skipped (invalid JSON): $($file.Name)" -ForegroundColor Red
    }
}

Write-Host "Fixed $fixedCount final path mismatches" -ForegroundColor Green