# P32 Component Keyword Standardization Script
# This script standardizes all component reference keywords in JSON files
# Changes any keyword matching *component* to exactly "components"
# Preserves "family_components" as an exception

param(
    [string]$Path = ".",
    [switch]$WhatIf,
    [switch]$Verbose
)

Write-Host "P32 Component Keyword Standardization Script" -ForegroundColor Cyan
Write-Host "==============================================" -ForegroundColor Cyan

# Function to standardize component keywords in a single JSON file
function Update-ComponentKeywords {
    param([string]$FilePath)

    if ($Verbose) {
        Write-Host "Processing: $FilePath" -ForegroundColor Yellow
    }

    try {
        $content = Get-Content $FilePath -Raw -Encoding UTF8

        # Track changes made
        $changes = 0

        # Pattern to match component keywords (but exclude "family_components")
        $componentPatterns = @(
            '"Component"\s*:',      # "Component":
            '"base_components"\s*:', # "base_components":
            '"system_level_components"\s*:', # "system_level_components":
            '"standard_components"\s*:', # "standard_components":
            '"component"\s*:'       # "component":
        )

        foreach ($pattern in $componentPatterns) {
            # Find all matches of this pattern
            $patternMatches = [regex]::Matches($content, $pattern)

            foreach ($match in $patternMatches) {
                # Get the full context around this match
                $startIndex = $match.Index
                $endIndex = $content.IndexOf('}', $startIndex)

                if ($endIndex -gt $startIndex) {
                    $componentBlock = $content.Substring($startIndex, $endIndex - $startIndex + 1)

                    # Check if this is a single value or array
                    if ($componentBlock -match '"\s*:\s*"([^"]+)"') {
                        # Single component value - convert to array
                        $singleValue = $patternMatches[1].Groups[1].Value
                        $newBlock = $componentBlock -replace '"\s*:\s*"([^"]+)"', '"components": ["$1"]'
                        $content = $content.Replace($componentBlock, $newBlock)
                        $changes++
                        if ($Verbose) {
                            Write-Host "  Converted single component to array: $singleValue" -ForegroundColor Green
                        }
                    } elseif ($componentBlock -match '"\s*:\s*\[') {
                        # Already an array - just change the key
                        $newBlock = $componentBlock -replace $pattern, '"components":'
                        $content = $content.Replace($componentBlock, $newBlock)
                        $changes++
                        if ($Verbose) {
                            Write-Host "  Changed keyword to 'components' (array already)" -ForegroundColor Green
                        }
                    }
                }
            }
        }

        # Handle consolidation of multiple component sections within the same file
        # This is more complex and would require JSON parsing - for now, we'll note files that need manual consolidation

        if ($changes -gt 0) {
            if ($WhatIf) {
                Write-Host "  Would make $changes changes in: $FilePath" -ForegroundColor Magenta
            } else {
                $content | Set-Content $FilePath -Encoding UTF8
                Write-Host "  Made $changes changes in: $FilePath" -ForegroundColor Green
            }
        } elseif ($Verbose) {
            Write-Host "  No changes needed in: $FilePath" -ForegroundColor Gray
        }

        return $changes
    }
    catch {
        Write-Host "  Error processing $FilePath : $($_.Exception.Message)" -ForegroundColor Red
        return 0
    }
}

# Main script logic
$totalFiles = 0
$totalChanges = 0

# Find all JSON files recursively
$jsonFiles = Get-ChildItem -Path $Path -Filter "*.json" -Recurse -File

Write-Host "Found $($jsonFiles.Count) JSON files to process" -ForegroundColor Cyan

foreach ($file in $jsonFiles) {
    $changes = Update-ComponentKeywords -FilePath $file.FullName
    $totalChanges += $changes
    $totalFiles++
}

# Summary
Write-Host "`n==============================================" -ForegroundColor Cyan
Write-Host "Standardization Complete!" -ForegroundColor Green
Write-Host "Files processed: $totalFiles" -ForegroundColor White
Write-Host "Total changes made: $totalChanges" -ForegroundColor White

if ($WhatIf) {
    Write-Host "`nThis was a WHATIF run - no actual changes were made." -ForegroundColor Yellow
    Write-Host "Remove -WhatIf parameter to apply changes." -ForegroundColor Yellow
}

Write-Host "`nNote: Files with multiple component sections may need manual consolidation." -ForegroundColor Yellow
Write-Host "The 'family_components' keyword was preserved as an exception." -ForegroundColor Yellow