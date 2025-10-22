# Mass JSON Component Fix Script - PROPER POWERSHELL SYNTAX
# Converts component_id -> component_name and adds subsystem keywords

Write-Host "Starting mass JSON component fixes..." -ForegroundColor Green

# Get all JSON files with component_id
$jsonFiles = Get-ChildItem -Path "config" -Recurse -Filter "*.json" | Where-Object {
    $content = Get-Content $_.FullName -Raw -ErrorAction SilentlyContinue
    if ($content) {
        $content.Contains('"component_id"')
    }
}

Write-Host "Found $($jsonFiles.Count) JSON files with component_id to process" -ForegroundColor Yellow

$processedCount = 0
$errorCount = 0

foreach ($file in $jsonFiles) {
    try {
        Write-Host "Processing: $($file.Name)" -ForegroundColor Cyan
        
        # Read file content
        $content = Get-Content $file.FullName -Raw
        
        # Skip if already processed
        if ($content.Contains('"component_name"') -and -not $content.Contains('"component_id"')) {
            Write-Host "  Already processed, skipping" -ForegroundColor Gray
            continue
        }
        
        # Convert component_id to component_name (simple string replacement)
        $newContent = $content.Replace('"component_id":', '"component_name":')
        
        # Determine subsystem based on file path
        $subsystem = "UNKNOWN"
        $pathLower = $file.FullName.ToLower()
        
        if ($pathLower.Contains("head_components") -or $pathLower.Contains("head.json") -or 
            $pathLower.Contains("eye") -or $pathLower.Contains("nose") -or 
            $pathLower.Contains("mouth") -or $pathLower.Contains("ear") -or 
            $pathLower.Contains("speaker")) {
            $subsystem = "HEAD"
        }
        elseif ($pathLower.Contains("torso_components") -or $pathLower.Contains("torso.json") -or 
                $pathLower.Contains("spine") -or $pathLower.Contains("power") -or 
                $pathLower.Contains("mesh") -or $pathLower.Contains("controller")) {
            $subsystem = "TORSO"
        }
        elseif ($pathLower.Contains("arm_components") -or $pathLower.Contains("arm.json") -or 
                $pathLower.Contains("shoulder") -or $pathLower.Contains("elbow") -or 
                $pathLower.Contains("wrist")) {
            $subsystem = "LEFT_ARM"
        }
        elseif ($pathLower.Contains("hand_components") -or $pathLower.Contains("hand.json") -or 
                $pathLower.Contains("finger") -or $pathLower.Contains("thumb")) {
            $subsystem = "LEFT_HAND"
        }
        elseif ($pathLower.Contains("leg_components") -or $pathLower.Contains("leg.json") -or 
                $pathLower.Contains("hip") -or $pathLower.Contains("knee") -or 
                $pathLower.Contains("ankle")) {
            $subsystem = "LEFT_LEG"
        }
        
        # Add subsystem keyword if not present
        if (-not $newContent.Contains('"subsystem"')) {
            # Find the component_name line and add subsystem after it
            $lines = $newContent -split "`r?`n"
            $newLines = @()
            
            foreach ($line in $lines) {
                $newLines += $line
                if ($line.Contains('"component_name":')) {
                    $newLines += "  `"subsystem`": `"$subsystem`","
                }
            }
            $newContent = $newLines -join "`n"
        }
        
        # Write back to file
        $newContent | Set-Content -Path $file.FullName -NoNewline
        Write-Host "  Done" -ForegroundColor Green
        $processedCount++
        
    } catch {
        Write-Host "  Error: $($_.Exception.Message)" -ForegroundColor Red
        $errorCount++
    }
}

Write-Host "`nProcessing complete!" -ForegroundColor Green
Write-Host "Files processed: $processedCount" -ForegroundColor Cyan
Write-Host "Errors: $errorCount" -ForegroundColor Red