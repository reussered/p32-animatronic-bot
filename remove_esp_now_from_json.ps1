# PowerShell script for Windows environment
<#
.SYNOPSIS
    Remove ESP-NOW mesh configuration variables from all JSON files
.DESCRIPTION
    This script removes obsolete ESP-NOW mesh configuration variables from JSON files
    throughout the project. These variables are now encapsulated in the SharedMemory class.
    
    Variables removed:
    - mesh_role
    - mesh_capabilities  
    - ESP_NOW_MASTER
    - ESP_NOW_SLAVE
    - SLAVE_NODE_HIGH_PRIORITY
    - mesh_network_config
    - esp_now_channel
    - mesh_node_count
    - mesh_heartbeat_hitCount
    - esp_now_enabled
    
.NOTES
    Based on project rule: ESP-NOW mesh cleanup from .github/consistant_project_rules.md
#>

param(
    [switch]$DryRun = $false,
    [switch]$Verbose = $false
)

# ESP-NOW variables to remove (from project rules)
$EspNowVariables = @(
    "mesh_role",
    "mesh_capabilities", 
    "ESP_NOW_MASTER",
    "ESP_NOW_SLAVE",
    "SLAVE_NODE_HIGH_PRIORITY",
    "mesh_network_config",
    "esp_now_channel",
    "mesh_node_count", 
    "mesh_heartbeat_hitCount",
    "esp_now_enabled"
)

# Find all JSON files in the project
$JsonFiles = Get-ChildItem -Path "." -Recurse -Filter "*.json" | Where-Object {
    # Exclude build directories and node_modules  
    $_.FullName -notmatch "\\.pio\\" -and 
    $_.FullName -notmatch "\\node_modules\\" -and
    $_.FullName -notmatch "\\.git\\"
}

Write-Host "Found $($JsonFiles.Count) JSON files to process" -ForegroundColor Green

$TotalFilesModified = 0
$TotalVariablesRemoved = 0

foreach ($File in $JsonFiles) {
    try {
        $Content = Get-Content -Path $File.FullName -Raw -Encoding UTF8
        $OriginalContent = $Content
        $FileModified = $false
        $VariablesRemovedInFile = 0
        
        if ($Verbose) {
            Write-Host "`nProcessing: $($File.FullName)" -ForegroundColor Cyan
        }
        
        # Parse JSON to validate structure
        try {
            ConvertFrom-Json $Content -ErrorAction Stop | Out-Null
        }
        catch {
            Write-Warning "Skipping invalid JSON: $($File.FullName) - $($_.Exception.Message)"
            continue
        }
        
        # Remove each ESP-NOW variable
        foreach ($Variable in $EspNowVariables) {
            # Simple pattern matching for JSON key removal
            $PatternWithComma = '"' + $Variable + '"\s*:\s*[^,}]+,\s*'
            $PatternWithoutComma = ',\s*"' + $Variable + '"\s*:\s*[^,}]+'
            $PatternAlone = '"' + $Variable + '"\s*:\s*[^,}]+\s*'
            
            # Try patterns in order
            if ($Content -match $PatternWithComma) {
                $Content = $Content -replace $PatternWithComma, ''
                $FileModified = $true
                $VariablesRemovedInFile++
                
                if ($Verbose) {
                    Write-Host "  Removed: $Variable" -ForegroundColor Yellow
                }
            }
            elseif ($Content -match $PatternWithoutComma) {
                $Content = $Content -replace $PatternWithoutComma, ''
                $FileModified = $true
                $VariablesRemovedInFile++
                
                if ($Verbose) {
                    Write-Host "  Removed: $Variable" -ForegroundColor Yellow
                }
            }
            elseif ($Content -match $PatternAlone) {
                $Content = $Content -replace $PatternAlone, ''
                $FileModified = $true
                $VariablesRemovedInFile++
                
                if ($Verbose) {
                    Write-Host "  Removed: $Variable" -ForegroundColor Yellow
                }
            }
        }
        
        if ($FileModified) {
            $TotalFilesModified++
            $TotalVariablesRemoved += $VariablesRemovedInFile
            
            Write-Host "Modified: $($File.FullName) ($VariablesRemovedInFile variables removed)" -ForegroundColor Green
            
            if (-not $DryRun) {
                # Validate JSON after modifications
                try {
                    ConvertFrom-Json $Content -ErrorAction Stop | Out-Null
                    
                    # Write the cleaned content back
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
            Write-Host "  No ESP-NOW variables found" -ForegroundColor Gray
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
Write-Host "Total variables removed: $TotalVariablesRemoved" -ForegroundColor Yellow

if ($DryRun) {
    Write-Host "`nDRY RUN MODE - No files were actually modified" -ForegroundColor Magenta
    Write-Host "Run without -DryRun to apply changes" -ForegroundColor Magenta
}
else {
    Write-Host "`nESP-NOW cleanup complete!" -ForegroundColor Green
    Write-Host "Components now use SharedMemory class for ESP-NOW communication" -ForegroundColor White
}

# Show ESP-NOW variables that were targeted
Write-Host "`nESP-NOW variables removed:" -ForegroundColor Cyan
foreach ($Variable in $EspNowVariables) {
    Write-Host "  - $Variable" -ForegroundColor Gray
}