# Hardware Cost/Performance Update Management Script
# Manages versioned tracking of hardware specifications with historical preservation
# FATAL ERROR DETECTION: Terminates on unknown components or corrupted build state

param(
    [string]$ComponentPath = "config/components/hardware",
    [switch]$CheckAll,
    [switch]$UpdateScheduled,
    [string]$SpecificComponent,
    [switch]$ForceUpdate,
    [switch]$ShowDue
)

$ErrorActionPreference = "Stop"

# FATAL BUG DETECTION FUNCTIONS
function Test-FatalBugs {
    Write-Host "? SCANNING FOR FATAL BUGS..." -ForegroundColor Red
    
    $fatalErrors = @()
    
    # Check for unknown components in source code
    $unknownCppComponents = Get-ChildItem -Path "src" -Recurse -Filter "*.cpp" | Where-Object { 
        $_.Name -match "unknown.*\.cpp$" -or (Get-Content $_.FullName -ErrorAction SilentlyContinue | Select-String "unknown_component")
    }
    
    $unknownSrcComponents = Get-ChildItem -Path "src" -Recurse -Filter "*.src" | Where-Object { 
        $_.Name -match "unknown.*\.src$" -or (Get-Content $_.FullName -ErrorAction SilentlyContinue | Select-String "unknown_component")
    }
    
    $unknownComponents = $unknownCppComponents + $unknownSrcComponents
    
    if ($unknownComponents) {
        $fatalErrors += "? FATAL: Unknown component files detected:"
        $unknownComponents | ForEach-Object { $fatalErrors += "   - $($_.FullName)" }
    }
    
    # Check for unknown references in JSON files
    $jsonFiles = Get-ChildItem -Path "config" -Recurse -Filter "*.json"
    foreach ($jsonFile in $jsonFiles) {
        try {
            $content = Get-Content $jsonFile.FullName -Raw
            if ($content -match '"unknown[^"]*"' -or $content -match '"UNKNOWN[^"]*"') {
                $fatalErrors += "? FATAL: Unknown references in JSON: $($jsonFile.FullName)"
            }
        }
        catch {
            $fatalErrors += "? FATAL: Corrupted JSON file: $($jsonFile.FullName)"
        }
    }
    
    # Check for auto-generated cruft in includes
    $includeFiles = Get-ChildItem -Path "include" -Recurse -Filter "*.hpp" | Where-Object { 
        (Get-Content $_.FullName -ErrorAction SilentlyContinue | Select-String "unknown_component")
    }
    
    if ($includeFiles) {
        $fatalErrors += "? FATAL: Unknown component references in headers:"
        $includeFiles | ForEach-Object { $fatalErrors += "   - $($_.FullName)" }
    }
    
    # Check for broken component registrations
    $componentTables = Get-ChildItem -Path "src" -Recurse -Filter "*component*table*.cpp"
    foreach ($table in $componentTables) {
        $content = Get-Content $table.FullName -ErrorAction SilentlyContinue
        if ($content -match "unknown_component" -or $content -match "UNKNOWN_COMPONENT") {
            $fatalErrors += "? FATAL: Broken component table: $($table.FullName)"
        }
    }
    
    if ($fatalErrors.Count -gt 0) {
        Write-Host "`n" -ForegroundColor Red
        Write-Host "??? FATAL BUGS DETECTED - TERMINATING ???" -ForegroundColor Red -BackgroundColor Yellow
        Write-Host "=" * 60 -ForegroundColor Red
        foreach ($fatalError in $fatalErrors) {
            Write-Host $fatalError -ForegroundColor Red
        }
        Write-Host "=" * 60 -ForegroundColor Red
        Write-Host "`n? BUILD STATE CORRUPTED - FIX IMMEDIATELY" -ForegroundColor Red -BackgroundColor Yellow
        Write-Host "? REQUIRED ACTIONS:" -ForegroundColor Yellow
        Write-Host "   1. Delete all files containing 'unknown_component'" -ForegroundColor White
        Write-Host "   2. Run component generation scripts to rebuild" -ForegroundColor White
        Write-Host "   3. Verify all JSON files for unknown references" -ForegroundColor White
        Write-Host "   4. Re-run this script after cleanup" -ForegroundColor White
        Write-Host "`n? SCRIPT TERMINATED - STATUS: FAILED" -ForegroundColor Red -BackgroundColor Yellow
        exit 1
    }
    
    Write-Host "? No fatal bugs detected - system clean" -ForegroundColor Green
}

function Get-ComponentFiles {
    return Get-ChildItem -Path $ComponentPath -Filter "*.json" | Where-Object { $_.Name -notlike "*template*" }
}

function Test-UpdateDue {
    param($ComponentData)
    
    if (-not $ComponentData.cost_performance_history) { return $true }
    
    $lastUpdate = [DateTime]::Parse($ComponentData.cost_performance_history.last_updated)
    $intervalDays = $ComponentData.cost_performance_history.update_schedule.automatic_check_interval_days
    $nextCheck = $lastUpdate.AddDays($intervalDays)
    
    return (Get-Date) -gt $nextCheck
}

function Update-ComponentData {
    param(
        [string]$FilePath,
        [hashtable]$NewData,
        [string]$UpdateReason = "SCHEDULED_UPDATE"
    )
    
    $currentData = Get-Content $FilePath | ConvertFrom-Json -AsHashtable
    
    # Preserve existing data as historical if it's different
    if ($currentData.cost_performance_history -and 
        $currentData.cost_performance_history.current_data) {
        
        $oldData = $currentData.cost_performance_history.current_data
        $oldData.archived_reason = $UpdateReason
        
        # Only archive if data actually changed
        $hasChanges = $false
        foreach ($key in $NewData.Keys) {
            if ($oldData.$key -ne $NewData.$key) {
                $hasChanges = $true
                break
            }
        }
        
        if ($hasChanges) {
            if (-not $currentData.cost_performance_history.historical_data) {
                $currentData.cost_performance_history.historical_data = @()
            }
            $currentData.cost_performance_history.historical_data += $oldData
        }
    }
    
    # Update current data
    if (-not $currentData.cost_performance_history) {
        $currentData.cost_performance_history = @{}
    }
    
    $currentData.cost_performance_history.current_data = $NewData
    $currentData.cost_performance_history.last_updated = (Get-Date).ToString("yyyy-MM-dd")
    
    # Calculate next check date
    $intervalDays = $currentData.cost_performance_history.update_schedule.automatic_check_interval_days
    if (-not $intervalDays) { $intervalDays = 30 }
    $nextCheck = (Get-Date).AddDays($intervalDays).ToString("yyyy-MM-dd")
    $currentData.cost_performance_history.update_schedule.next_scheduled_check = $nextCheck
    
    # Write back to file
    $currentData | ConvertTo-Json -Depth 10 | Set-Content $FilePath
    
    Write-Host "Updated: $FilePath" -ForegroundColor Green
}

function Show-UpdatesDue {
    $components = Get-ComponentFiles
    $dueComponents = @()
    
    foreach ($component in $components) {
        try {
            $data = Get-Content $component.FullName | ConvertFrom-Json
            if (Test-UpdateDue $data) {
                $lastUpdate = if ($data.cost_performance_history.last_updated) {
                    $data.cost_performance_history.last_updated
                } else {
                    "NEVER"
                }
                
                $dueComponents += [PSCustomObject]@{
                    Component = $component.BaseName
                    LastUpdate = $lastUpdate
                    Status = if ($lastUpdate -eq "NEVER") { "MISSING_DATA" } else { "UPDATE_DUE" }
                }
            }
        }
        catch {
            $dueComponents += [PSCustomObject]@{
                Component = $component.BaseName
                LastUpdate = "ERROR"
                Status = "PARSE_ERROR"
            }
        }
    }
    
    if ($dueComponents.Count -eq 0) {
        Write-Host "All components are up to date!" -ForegroundColor Green
    } else {
        Write-Host "`nComponents requiring updates:" -ForegroundColor Yellow
        $dueComponents | Format-Table -AutoSize
        
        Write-Host "`nTo update all due components, run:" -ForegroundColor Cyan
        Write-Host ".\update_hardware_data.ps1 -UpdateScheduled" -ForegroundColor White
    }
    
    return $dueComponents
}

function Request-ManualUpdate {
    param($ComponentName)
    
    Write-Host "`nManual data collection required for: $ComponentName" -ForegroundColor Yellow
    Write-Host "Please research and provide the following information:" -ForegroundColor Cyan
    
    $newData = @{
        date_valid = (Get-Date).ToString("yyyy-MM-dd")
        data_source = "MANUAL_RESEARCH"
        verification_status = "PENDING"
    }
    
    # Interactive data collection would go here
    # For now, just prompt user to manually edit
    Write-Host "`nPlease manually update the component file with current data." -ForegroundColor White
    Write-Host "Template available at: config/templates/versioned_cost_performance_template.json" -ForegroundColor Gray
    
    return $newData
}

# Main execution
# ALWAYS run fatal bug detection first
Test-FatalBugs

switch ($true) {
    $ShowDue {
        Show-UpdatesDue
        exit 0
    }
    
    $UpdateScheduled {
        $components = Get-ComponentFiles
        $updatedCount = 0
        
        foreach ($component in $components) {
            try {
                $data = Get-Content $component.FullName | ConvertFrom-Json
                if (Test-UpdateDue $data -or $ForceUpdate) {
                    Write-Host "Processing: $($component.BaseName)" -ForegroundColor Yellow
                    $newData = Request-ManualUpdate $component.BaseName
                    # Update-ComponentData $component.FullName $newData "SCHEDULED_UPDATE"
                    $updatedCount++
                }
            }
            catch {
                Write-Warning "Failed to process $($component.Name): $($_.Exception.Message)"
            }
        }
        
        Write-Host "`nProcessed $updatedCount components for updates." -ForegroundColor Green
    }
    
    $SpecificComponent {
        $componentFile = Get-ComponentFiles | Where-Object { $_.BaseName -eq $SpecificComponent }
        if (-not $componentFile) {
            Write-Error "Component '$SpecificComponent' not found in $ComponentPath"
            exit 1
        }
        
        Write-Host "Updating specific component: $SpecificComponent" -ForegroundColor Yellow
        $newData = Request-ManualUpdate $SpecificComponent
        # Update-ComponentData $componentFile.FullName $newData "MANUAL_REQUEST"
    }
    
    default {
        Write-Host "Hardware Cost/Performance Update Manager" -ForegroundColor Cyan
        Write-Host "=========================================" -ForegroundColor Cyan
        Write-Host ""
        Write-Host "Usage examples:" -ForegroundColor Yellow
        Write-Host "  .\update_hardware_data.ps1 -ShowDue                    # Show components needing updates"
        Write-Host "  .\update_hardware_data.ps1 -UpdateScheduled            # Update all due components"
        Write-Host "  .\update_hardware_data.ps1 -SpecificComponent ssd1351  # Update specific component"
        Write-Host "  .\update_hardware_data.ps1 -CheckAll -ForceUpdate      # Force update all components"
        Write-Host ""
        
        Show-UpdatesDue | Out-Null
    }
}