# One-time script to remove relative_filename field from all JSON files
# This field is no longer required and is critical to remove

$jsonFiles = Get-ChildItem -Path "config" -Recurse -Filter "*.json"
$processedCount = 0
$modifiedCount = 0

foreach ($file in $jsonFiles) {
    $processedCount++
    Write-Progress -Activity "Processing JSON files" -Status "File $processedCount of $($jsonFiles.Count)" -PercentComplete (($processedCount / $jsonFiles.Count) * 100)
    
    try {
        $content = Get-Content -Path $file.FullName -Raw
        
        if ($content -match '"relative_filename"') {
            Write-Host "Removing relative_filename from: $($file.FullName)"
            
            # Remove the relative_filename line (handles both comma patterns)
            $content = $content -replace '^\s*"relative_filename":\s*"[^"]*",?\s*\r?\n', ''
            $content = $content -replace ',\s*\r?\n\s*"relative_filename":\s*"[^"]*"', ''
            
            # Clean up any double commas that might result
            $content = $content -replace ',,', ','
            
            # Clean up trailing commas before closing braces
            $content = $content -replace ',(\s*[\]}])', '$1'
            
            Set-Content -Path $file.FullName -Value $content -NoNewline
            $modifiedCount++
        }
    }
    catch {
        Write-Warning "Failed to process $($file.FullName): $_"
    }
}

Write-Host "`nCompleted: Processed $processedCount files, Modified $modifiedCount files"