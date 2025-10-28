# FATAL BUG DETECTION SCRIPT
# Scans for unknown components and incomplete JSON definitions
# IDENTIFIES ROOT CAUSES: Missing JSON sections, undefined components, incomplete configs

param(
    [switch]$Verbose,
    [switch]$FixMode,
    [switch]$DiagnosticMode
)

$ErrorActionPreference = "Stop"

Write-Host "COMPONENT INTEGRITY SCANNER - P32 ANIMATRONIC SYSTEM" -ForegroundColor Cyan -BackgroundColor Black
Write-Host "=" * 70 -ForegroundColor Cyan

$fatalErrors = @()
$configIssues = @()
$warningCount = 0

# SCAN 1: Unknown component source files (INDICATES INCOMPLETE JSON DEFINITIONS)
Write-Host "Scanning for unknown component files (JSON definition issues)..." -ForegroundColor Yellow
$unknownSources = Get-ChildItem -Path "src" -Recurse -Filter "*.cpp" -ErrorAction SilentlyContinue | Where-Object { 
    $_.Name -match "unknown.*\.cpp$"
} | ForEach-Object { $_ }

$unknownSrcSources = Get-ChildItem -Path "src" -Recurse -Filter "*.src" -ErrorAction SilentlyContinue | Where-Object { 
    $_.Name -match "unknown.*\.src$"
}

$unknownSources += $unknownSrcSources

if ($unknownSources) {
    $fatalErrors += "CRITICAL: Unknown component files detected (indicates incomplete JSON):"
    $unknownSources | ForEach-Object { 
        $fatalErrors += "   - $($_.FullName)" 
    }
    $configIssues += "ROOT_CAUSE: Component generation script found JSON files with missing definitions"
    
    if ($FixMode) {
        Write-Host "FIX MODE: Removing auto-generated unknown files..." -ForegroundColor Cyan
        $unknownSources | Remove-Item -Force
        Write-Host "Deleted $($unknownSources.Count) unknown source files" -ForegroundColor Green
        $configIssues += "ACTION_TAKEN: Deleted auto-generated unknown component files"
    }
}

# SCAN 2: Identify JSON files with incomplete definitions
Write-Host "Scanning JSON files for incomplete component definitions..." -ForegroundColor Yellow
$componentJsons = Get-ChildItem -Path "config/components" -Recurse -Filter "*.json" -ErrorAction SilentlyContinue
$incompleteJsons = @()

foreach ($jsonFile in $componentJsons) {
    try {
        $jsonContent = Get-Content $jsonFile.FullName | ConvertFrom-Json
        $isIncomplete = $false
        $missingFields = @()
        
        # Check for required fields based on file location
        if ($jsonFile.DirectoryName -like "*positioned*") {
            if (-not $jsonContent.component_name) { 
                $missingFields += "component_name" 
            }
            if (-not $jsonContent.hardware_type) { 
                $missingFields += "hardware_type" 
            }
            if (-not $jsonContent.position) { 
                $missingFields += "position" 
            }
        }
        
        if ($jsonFile.DirectoryName -like "*hardware*") {
            if (-not $jsonContent.hardware_type) { 
                $missingFields += "hardware_type" 
            }
            if (-not $jsonContent.component_name) { 
                $missingFields += "component_name" 
            }
        }
        
        if ($jsonFile.DirectoryName -like "*interfaces*") {
            if (-not $jsonContent.interface_id) { 
                $missingFields += "interface_id" 
            }
        }
        
        # Check for placeholder values
        $jsonString = Get-Content $jsonFile.FullName -Raw
        if ($jsonString -match "TODO|UNDEFINED|PLACEHOLDER|FIXME") {
            $isIncomplete = $true
        }
        
        if ($missingFields.Count -gt 0 -or $isIncomplete) {
            $incompleteJsons += @{
                File = $jsonFile.FullName
                MissingFields = $missingFields
                IsPlaceholder = $isIncomplete
            }
        }
    } catch {
        $configIssues += "JSON PARSE ERROR: $($jsonFile.FullName) - $($_.Exception.Message)"
        $warningCount++
    }
}

if ($incompleteJsons.Count -gt 0) {
    $configIssues += "INCOMPLETE JSON DEFINITIONS FOUND:"
    foreach ($incomplete in $incompleteJsons) {
        $configIssues += "   FILE: $($incomplete.File)"
        if ($incomplete.MissingFields.Count -gt 0) {
            $configIssues += "   MISSING: $($incomplete.MissingFields -join ', ')"
        }
        if ($incomplete.IsPlaceholder) {
            $configIssues += "   ISSUE: Contains placeholder/undefined values"
        }
    }
}

# SCAN 3: Component generation validation
Write-Host "Validating component generation results..." -ForegroundColor Yellow
$componentTables = Get-ChildItem -Path "src" -Recurse -Filter "*component*table*.cpp" -ErrorAction SilentlyContinue
foreach ($table in $componentTables) {
    try {
        $content = Get-Content $table.FullName -ErrorAction SilentlyContinue
        if ($content -match "unknown_component" -or $content -match "UNKNOWN_COMPONENT") {
            $fatalErrors += "CRITICAL: Component table contains unknown entries: $($table.FullName)"
            $configIssues += "ROOT_CAUSE: Component generation script couldn't resolve some JSON definitions"
        }
    } catch {
        $configIssues += "Unable to scan component table: $($table.FullName)"
        $warningCount++
    }
}

# SCAN 4: Header file validation
Write-Host "Checking for unknown component headers..." -ForegroundColor Yellow
$unknownHeaders = Get-ChildItem -Path "src" -Recurse -Filter "*.h" -ErrorAction SilentlyContinue | Where-Object { 
    $_.Name -match "unknown.*\.h$"
}

if ($unknownHeaders) {
    $fatalErrors += "CRITICAL: Unknown component headers detected:"
    $unknownHeaders | ForEach-Object { 
        $fatalErrors += "   - $($_.FullName)" 
    }
    
    if ($FixMode) {
        Write-Host "FIX MODE: Deleting unknown header files..." -ForegroundColor Cyan
        $unknownHeaders | Remove-Item -Force
        Write-Host "Deleted $($unknownHeaders.Count) unknown header files" -ForegroundColor Green
    }
}

# RESULTS AND RECOMMENDATIONS
Write-Host "`n" -ForegroundColor White
Write-Host "=" * 70 -ForegroundColor Cyan

if ($fatalErrors.Count -eq 0 -and $configIssues.Count -eq 0) {
    Write-Host "SYSTEM INTEGRITY: EXCELLENT" -ForegroundColor Green -BackgroundColor Black
    Write-Host "All component definitions are complete" -ForegroundColor Green
    Write-Host "No unknown components detected" -ForegroundColor Green
    Write-Host "JSON configurations are valid" -ForegroundColor Green
    Write-Host "Component generation is clean" -ForegroundColor Green
    
    if ($warningCount -gt 0) {
        Write-Host "$warningCount minor warnings (non-critical)" -ForegroundColor Yellow
    }
    
    Write-Host "`nSTATUS: PASSED" -ForegroundColor Green -BackgroundColor Black
    exit 0
}

if ($configIssues.Count -gt 0 -and $fatalErrors.Count -eq 0) {
    Write-Host "CONFIGURATION ISSUES DETECTED" -ForegroundColor Yellow -BackgroundColor Black
    Write-Host "These are likely the ROOT CAUSE of unknown components:" -ForegroundColor Yellow
    Write-Host ""
    
    foreach ($issue in $configIssues) {
        Write-Host $issue -ForegroundColor Yellow
    }
    
    Write-Host ""
    Write-Host "RECOMMENDED ACTIONS:" -ForegroundColor Cyan
    Write-Host "   1. Complete the incomplete JSON definitions listed above" -ForegroundColor White
    Write-Host "   2. Run: .\tools\generate_tables.py to regenerate components" -ForegroundColor White
    Write-Host "   3. Re-run this scanner to verify fixes" -ForegroundColor White
    Write-Host ""
    Write-Host "TIP: Unknown components are usually auto-generated when JSON is incomplete" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "STATUS: NEEDS_ATTENTION" -ForegroundColor Yellow -BackgroundColor Black
    exit 0
}

if ($fatalErrors.Count -gt 0) {
    Write-Host "CRITICAL SYSTEM ISSUES" -ForegroundColor Red -BackgroundColor Yellow
    Write-Host "$($fatalErrors.Count) CRITICAL PROBLEMS FOUND:" -ForegroundColor Red
    Write-Host ""
    
    foreach ($fatalError in $fatalErrors) {
        Write-Host $fatalError -ForegroundColor Red
    }
    
    if ($configIssues.Count -gt 0) {
        Write-Host ""
        Write-Host "UNDERLYING CONFIGURATION ISSUES:" -ForegroundColor Yellow
        foreach ($issue in $configIssues) {
            Write-Host $issue -ForegroundColor Yellow
        }
    }
    
    Write-Host ""
    Write-Host "CRITICAL REPAIR ACTIONS:" -ForegroundColor Red
    Write-Host "   1. Fix incomplete JSON definitions (listed above)" -ForegroundColor White
    Write-Host "   2. Delete unknown component files: .\tools\fatal_bug_scanner_clean.ps1 -FixMode" -ForegroundColor White
    Write-Host "   3. Regenerate components: .\tools\generate_tables.py" -ForegroundColor White
    Write-Host "   4. Clean build: pio run --clean-first" -ForegroundColor White
    Write-Host "   5. Re-run this scanner to verify" -ForegroundColor White
    Write-Host ""
    Write-Host "ROOT CAUSE: Unknown components = Incomplete JSON definitions" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "STATUS: FAILED" -ForegroundColor Red -BackgroundColor Yellow
    exit 1
}