# PowerShell script for Windows environment
<#
.SYNOPSIS
    Update JSON files to use new contained_components keyword
.DESCRIPTION
    This script updates all JSON files in the project to replace the old
    "positioned_components" keyword with the new "contained_components" keyword.
    
    This aligns with the updated project rules requiring recursive component
    composition using the contained_components array.
    
.NOTES
    Based on project rule: Use contained_components array for hierarchical 
    component inclusion from .github/consistant_project_rules.md
#>

param(
    [switch]$DryRun = $false,
    [switch]$Verbose = $false
)

# Find all JSON files in the project
$JsonFiles = Get-ChildItem -Path "." -Recurse -Filter "*.json" | Where-Object {
    # Exclude build directories and version control
    $_.FullName -notmatch "\\.pio\\" -and 
    $_.FullName -notmatch "\\.git\\" -and
    $_.FullName -notmatch "\\node_modules\\" -and
    $_.FullName -notmatch "\\.vs\\" -and
    $_.FullName -notmatch "\\build\\"
}

Write-Host "Found $($JsonFiles.Count) JSON files to process" -ForegroundColor Green

$TotalFilesModified = 0
$TotalReplacementsMade = 0

foreach ($File in $JsonFiles) {
    try {
        $Content = Get-Content -Path $File.FullName -Raw -Encoding UTF8
        $OriginalContent = $Content
        $FileModified = $false
        $ReplacementsInFile = 0
        
        if ($Verbose) {
            Write-Host "`nProcessing: $($File.FullName)" -ForegroundColor Cyan
        }
        
        # Parse JSON to validate structure first
        try {
            ConvertFrom-Json $Content -ErrorAction Stop | Out-Null
        }
        catch {
            Write-Warning "Skipping invalid JSON: $($File.FullName) - $($_.Exception.Message)"
            continue
        }
        
        # Check if file contains positioned_components
        if ($Content -match '"positioned_components"') {
            # Replace positioned_components with contained_components
            $UpdatedContent = $Content -replace '"positioned_components"', '"contained_components"'
            
            # Count replacements made
            $FoundMatches = [regex]::Matches($Content, '"positioned_components"')
            $ReplacementsInFile = $FoundMatches.Count
            
            if ($ReplacementsInFile -gt 0) {
                $Content = $UpdatedContent
                $FileModified = $true
                
                if ($Verbose) {
                    Write-Host "  Replaced $ReplacementsInFile occurrence(s) of 'positioned_components'" -ForegroundColor Yellow
                }
            }
        }
        
        if ($FileModified) {
            $TotalFilesModified++
            $TotalReplacementsMade += $ReplacementsInFile
            
            Write-Host "Modified: $($File.FullName) ($ReplacementsInFile replacements)" -ForegroundColor Green
            
            if (-not $DryRun) {
                # Validate JSON after modifications
                try {
                    ConvertFrom-Json $Content -ErrorAction Stop | Out-Null
                    
                    # Write the updated content back
                    Set-Content -Path $File.FullName -Value $Content -Encoding UTF8 -NoNewline
                    Write-Host "  File saved successfully" -ForegroundColor Green
                }
                catch {
                    Write-Error "JSON validation failed after modification: $($File.FullName) - $($_.Exception.Message)"
                    # Restore original content
                    Set-Content -Path $File.FullName -Value $OriginalContent -Encoding UTF8 -NoNewline
                    Write-Host "  Original content restored" -ForegroundColor Yellow
                }
            }
            else {
                Write-Host "  DRY RUN: Changes not saved" -ForegroundColor Magenta
            }
        }
        elseif ($Verbose) {
            Write-Host "  No positioned_components found" -ForegroundColor Gray
        }
    }
    catch {
        Write-Error "Error processing file $($File.FullName): $($_.Exception.Message)"
    }
}

# Summary
Write-Host "`nSUMMARY" -ForegroundColor Cyan
Write-Host "Files processed: $($JsonFiles.Count)" -ForegroundColor White
Write-Host "Files modified: $TotalFilesModified" -ForegroundColor Green
Write-Host "Total replacements made: $TotalReplacementsMade" -ForegroundColor Yellow

if ($DryRun) {
    Write-Host "`nDRY RUN MODE - No files were actually modified" -ForegroundColor Magenta
    Write-Host "Run without -DryRun to apply changes" -ForegroundColor Magenta
}
else {
    Write-Host "`nKeyword update complete!" -ForegroundColor Green
    Write-Host "All JSON files now use 'contained_components' for recursive composition" -ForegroundColor White
}

# Show examples of what was changed
if ($TotalReplacementsMade -gt 0) {
    Write-Host "`nKeyword Change:" -ForegroundColor Cyan
    Write-Host "  OLD: 'positioned_components'" -ForegroundColor Red
    Write-Host "  NEW: 'contained_components'" -ForegroundColor Green
    Write-Host "`nThis aligns with project rule for hierarchical component inclusion" -ForegroundColor White
}