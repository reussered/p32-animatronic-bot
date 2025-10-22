# Mass JSON Component Fix Script
# Converts component_id -> component_name and adds subsystem keywords

Write-Host "Starting mass JSON component fixes..." -ForegroundColor Green

# Get all JSON files with component_id
$jsonFiles = Get-ChildItem -Path "config" -Recurse -Filter "*.json" | Where-Object {
    $content = Get-Content $_.FullName -Raw -ErrorAction SilentlyContinue
    $content -match '"component_id"'
}

Write-Host "Found $($jsonFiles.Count) JSON files with component_id to process" -ForegroundColor Yellow

$processedCount = 0
$errorCount = 0

foreach ($file in $jsonFiles) {
    try {
        Write-Host "Processing: $($file.FullName)" -ForegroundColor Cyan
        
        # Read file content
        $content = Get-Content $file.FullName -Raw
        
        # Skip if already processed
        if ($content -match '"component_name"' -and $content -notmatch '"component_id"') {
            Write-Host "  Already processed, skipping" -ForegroundColor Gray
            continue
        }
        
        # Convert component_id to component_name
        $newContent = $content -replace '"component_id":', '"component_name":'
        
        # Determine subsystem based on file path
        $subsystem = "UNKNOWN"
        if ($file.FullName -match "head_components|head\.json|eye|nose|mouth|ear|speaker") {
            $subsystem = "HEAD"
        }
        elseif ($file.FullName -match "torso_components|torso\.json|spine|power|mesh|controller") {
            $subsystem = "TORSO"
        }
        elseif ($file.FullName -match "arm_components|arm\.json|shoulder|elbow|wrist") {
            $subsystem = "LEFT_ARM"
        }
        elseif ($file.FullName -match "hand_components|hand\.json|finger|thumb") {
            $subsystem = "LEFT_HAND"
        }
        elseif ($file.FullName -match "leg_components|leg\.json|hip|knee|ankle") {
            $subsystem = "LEFT_LEG"
        }
        
        # Add subsystem keyword if not present
        if ($newContent -notmatch '"subsystem"') {
            $newContent = $newContent -replace '("component_name":\s*"[^"]*",)', "`$1`n  `"subsystem`": `"$subsystem`","
        }
        
        # Write back to file
        Set-Content -Path $file.FullName -Value $newContent -NoNewline
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