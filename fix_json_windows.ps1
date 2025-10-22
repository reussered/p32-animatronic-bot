# Mass JSON Component Fix Script - WINDOWS POWERSHELL NATIVE
Write-Host "Starting mass JSON component fixes..." -ForegroundColor Green

# Get all JSON files containing component_id
$jsonFiles = Get-ChildItem -Path "config" -Recurse -Filter "*.json" | Where-Object {
    $content = Get-Content $_.FullName -Raw -ErrorAction SilentlyContinue
    if ($content) {
        $content.Contains('"component_id"')
    }
}

Write-Host "Found $($jsonFiles.Count) JSON files to process" -ForegroundColor Yellow

$processedCount = 0
$errorCount = 0

foreach ($file in $jsonFiles) {
    try {
        Write-Host "Processing: $($file.Name)" -ForegroundColor Cyan
        
        $content = Get-Content $file.FullName -Raw
        
        # Skip if already has component_name but no component_id
        if ($content.Contains('"component_name"') -and (-not $content.Contains('"component_id"'))) {
            Write-Host "  Already processed" -ForegroundColor Gray
            continue
        }
        
        # Replace component_id with component_name
        $newContent = $content.Replace('"component_id":', '"component_name":')
        
        # Determine subsystem based on file path
        $pathLower = $file.FullName.ToLower()
        $subsystem = "UNKNOWN"
        
        if ($pathLower.Contains("head") -or $pathLower.Contains("eye") -or $pathLower.Contains("nose") -or $pathLower.Contains("mouth") -or $pathLower.Contains("ear") -or $pathLower.Contains("speaker")) {
            $subsystem = "HEAD"
        }
        elseif ($pathLower.Contains("torso") -or $pathLower.Contains("spine") -or $pathLower.Contains("power") -or $pathLower.Contains("controller")) {
            $subsystem = "TORSO"
        }
        elseif ($pathLower.Contains("arm") -or $pathLower.Contains("shoulder") -or $pathLower.Contains("elbow") -or $pathLower.Contains("wrist")) {
            $subsystem = "ARM"
        }
        elseif ($pathLower.Contains("hand") -or $pathLower.Contains("finger") -or $pathLower.Contains("thumb")) {
            $subsystem = "HAND"
        }
        elseif ($pathLower.Contains("leg") -or $pathLower.Contains("hip") -or $pathLower.Contains("knee") -or $pathLower.Contains("ankle")) {
            $subsystem = "LEG"
        }
        
        # Add subsystem if not present
        if (-not $newContent.Contains('"subsystem"')) {
            $lines = $newContent.Split([Environment]::NewLine)
            $newLines = @()
            
            foreach ($line in $lines) {
                $newLines += $line
                if ($line.Contains('"component_name":')) {
                    $newLines += "  `"subsystem`": `"$subsystem`","
                }
            }
            $newContent = $newLines -join [Environment]::NewLine
        }
        
        Set-Content -Path $file.FullName -Value $newContent -NoNewline
        Write-Host "  Success" -ForegroundColor Green
        $processedCount++
        
    } catch {
        Write-Host "  Error: $($_.Exception.Message)" -ForegroundColor Red
        $errorCount++
    }
}

Write-Host "Complete! Processed: $processedCount, Errors: $errorCount" -ForegroundColor Green