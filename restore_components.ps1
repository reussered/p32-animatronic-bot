# Script to restore component arrays and use_fields from commit 018e3d2
# This systematically compares current JSON files with originals and restores missing data

$baseCommit = "018e3d2"
$goblinHeadDir = "config/bots/bot_families/goblins/head"

# Key files that MUST have components arrays
$filesToRestore = @(
    "goblin_left_eye.json",
    "goblin_right_eye.json",
    "goblin_speaker.json"
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "RESTORE COMPONENT ARRAYS FROM $baseCommit" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

foreach ($filename in $filesToRestore) {
    $filepath = "$goblinHeadDir/$filename"
    Write-Host "`nProcessing: $filename" -ForegroundColor Yellow
    
    # Get original from git
    $original = & git show "$baseCommit`:$filepath" 2>$null
    if ($?) {
        $originalJson = $original | ConvertFrom-Json -ErrorAction SilentlyContinue
        if ($originalJson) {
            Write-Host "  Components in original: $($originalJson.components -join ', ')"  
            Write-Host "  use_fields in original: $(($originalJson.use_fields | Get-Member -MemberType NoteProperty | Measure-Object).Count) fields"
            
            # Get current
            $current = Get-Content $filepath -Raw
            $currentJson = $current | ConvertFrom-Json -ErrorAction SilentlyContinue
            
            if ($currentJson) {
                $currentComponents = $currentJson.components
                $currentUseFields = $currentJson.use_fields
                
                Write-Host "  Components in current:  $($currentComponents -join ', ')"
                Write-Host "  use_fields in current:  $(($currentUseFields | Get-Member -MemberType NoteProperty | Measure-Object).Count) fields"
                
                if ((-not $currentComponents) -or ($currentComponents.Count -eq 0)) {
                    Write-Host "  STATUS: MISSING components array!" -ForegroundColor Red
                } else {
                    Write-Host "  STATUS: Components array OK" -ForegroundColor Green
                }
            }
        } else {
            Write-Host "  ERROR: Could not parse original JSON" -ForegroundColor Red
        }
    } else {
        Write-Host "  ERROR: File not found in commit $baseCommit" -ForegroundColor Red
    }
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "Report completed. Review above for changes needed." -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
