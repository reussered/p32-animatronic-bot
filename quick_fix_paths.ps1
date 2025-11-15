Write-Host "QUICK FIX: Removing deprecated 'relative_filename' entries from JSON files" -ForegroundColor Green

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
        
        # Remove deprecated property rather than correcting it
        $json.PSObject.Properties.Remove('relative_filename') | Out-Null
        $json | ConvertTo-Json -Depth 10 | Set-Content $file.FullName -Encoding ASCII
        $fixedCount++
    }
    catch {
        Write-Host "Skipped invalid JSON: $($file.Name)" -ForegroundColor Red
    }
}

Write-Host "COMPLETE: Fixed $fixedCount path mismatches" -ForegroundColor Green