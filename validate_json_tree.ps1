# validate_json_tree.ps1
# Comprehensive JSON validation for P32 Animatronic Bot
# Validates all JSON files for consistency and reference integrity

Write-Host "=== P32 JSON Tree Validation ===" -ForegroundColor Cyan
Write-Host ""

$projectRoot = "F:\GitHub\p32-animatronic-bot"
$errors = @()
$warnings = @()
$stats = @{
    totalFiles = 0
    validFiles = 0
    invalidFiles = 0
    totalReferences = 0
    missingReferences = 0
    duplicateReferences = 0
}

# Find all JSON files in config/
Write-Host "Scanning JSON files..." -ForegroundColor Yellow
$jsonFiles = Get-ChildItem -Path "$projectRoot\config" -Filter "*.json" -Recurse -File

$stats.totalFiles = $jsonFiles.Count
Write-Host "Found $($stats.totalFiles) JSON files" -ForegroundColor Green
Write-Host ""

# Function to check if file has UTF-8 BOM
function Test-BOM {
    param([string]$filePath)
    $bytes = [IO.File]::ReadAllBytes($filePath)
    if ($bytes.Length -ge 3 -and $bytes[0] -eq 239 -and $bytes[1] -eq 187 -and $bytes[2] -eq 191) {
        return $true
    }
    return $false
}

# Function to validate JSON structure
function Test-JsonFile {
    param([string]$filePath)
    
    try {
        # Check for BOM
        if (Test-BOM $filePath) {
            return @{
                valid = $false
                error = "UTF-8 BOM detected (breaks JSON parser)"
            }
        }
        
        # Parse JSON
        $content = Get-Content $filePath -Raw
        $json = $content | ConvertFrom-Json
        
        # Check for relative_filename field
        if (-not $json.relative_filename) {
            return @{
                valid = $false
                error = "Missing 'relative_filename' field"
            }
        }
        
        # Verify relative_filename matches actual path
        $expectedPath = $filePath.Replace($projectRoot + "\", "").Replace("\", "/")
        if ($json.relative_filename -ne $expectedPath) {
            return @{
                valid = $false
                error = "relative_filename mismatch: expected '$expectedPath', got '$($json.relative_filename)'"
            }
        }
        
        return @{
            valid = $true
            json = $json
        }
    }
    catch {
        return @{
            valid = $false
            error = $_.Exception.Message
        }
    }
}

# First pass: Validate all JSON files
Write-Host "Phase 1: Validating JSON structure..." -ForegroundColor Yellow
foreach ($file in $jsonFiles) {
    $relativePath = $file.FullName.Replace($projectRoot + "\", "").Replace("\", "/")
    
    $result = Test-JsonFile $file.FullName
    
    if (-not $result.valid) {
        $stats.invalidFiles++
        $errors += "[INVALID] $relativePath : $($result.error)"
        Write-Host "  [X] $relativePath" -ForegroundColor Red
    }
    else {
        $stats.validFiles++
        Write-Host "  [OK] $relativePath" -ForegroundColor Green
    }
}

Write-Host ""
Write-Host "Phase 2: Checking component references..." -ForegroundColor Yellow

# Second pass: Check all component references
foreach ($file in $jsonFiles) {
    $result = Test-JsonFile $file.FullName
    
    if ($result.valid -and $result.json.components) {
        $relativePath = $file.FullName.Replace($projectRoot + "\", "").Replace("\", "/")
        
        # Track duplicates in this file
        $seenRefs = @{}
        
        foreach ($ref in $result.json.components) {
            $stats.totalReferences++
            
            # Check for duplicates
            if ($seenRefs.ContainsKey($ref)) {
                $stats.duplicateReferences++
                $warnings += "[DUPLICATE] $relativePath : duplicate reference to '$ref'"
                Write-Host "  [!] DUPLICATE in $relativePath : $ref" -ForegroundColor Yellow
            }
            else {
                $seenRefs[$ref] = $true
            }
            
            # Check if referenced file exists
            $refPath = Join-Path $projectRoot $ref
            $refPath = $refPath.Replace("/", "\")
            
            if (-not (Test-Path $refPath)) {
                $stats.missingReferences++
                $errors += "[MISSING REF] $relativePath : references non-existent '$ref'"
                Write-Host "  [X] MISSING in $relativePath : $ref" -ForegroundColor Red
            }
            else {
                Write-Host "  [OK] $relativePath -> $ref" -ForegroundColor Green
            }
        }
    }
}

# Check for old positioned/ references
Write-Host ""
Write-Host "Phase 3: Checking for obsolete 'positioned/' references..." -ForegroundColor Yellow
foreach ($file in $jsonFiles) {
    $content = Get-Content $file.FullName -Raw
    if ($content -match 'positioned/') {
        $relativePath = $file.FullName.Replace($projectRoot + "\", "").Replace("\", "/")
        $errors += "[OBSOLETE] $relativePath : contains 'positioned/' reference (folder deleted)"
        Write-Host "  [X] $relativePath contains obsolete 'positioned/' path" -ForegroundColor Red
    }
}

# Summary
Write-Host ""
Write-Host "=== VALIDATION SUMMARY ===" -ForegroundColor Cyan
Write-Host "Total JSON files:        $($stats.totalFiles)" -ForegroundColor White
Write-Host "Valid files:             $($stats.validFiles)" -ForegroundColor Green
Write-Host "Invalid files:           $($stats.invalidFiles)" -ForegroundColor $(if ($stats.invalidFiles -eq 0) { "Green" } else { "Red" })
Write-Host "Total references:        $($stats.totalReferences)" -ForegroundColor White
Write-Host "Missing references:      $($stats.missingReferences)" -ForegroundColor $(if ($stats.missingReferences -eq 0) { "Green" } else { "Red" })
Write-Host "Duplicate references:    $($stats.duplicateReferences)" -ForegroundColor $(if ($stats.duplicateReferences -eq 0) { "Green" } else { "Yellow" })
Write-Host ""

if ($errors.Count -gt 0) {
    Write-Host "=== ERRORS ($($errors.Count)) ===" -ForegroundColor Red
    foreach ($err in $errors) {
        Write-Host "  $err" -ForegroundColor Red
    }
    Write-Host ""
}

if ($warnings.Count -gt 0) {
    Write-Host "=== WARNINGS ($($warnings.Count)) ===" -ForegroundColor Yellow
    foreach ($warn in $warnings) {
        Write-Host "  $warn" -ForegroundColor Yellow
    }
    Write-Host ""
}

if ($errors.Count -eq 0 -and $warnings.Count -eq 0) {
    Write-Host "=== ALL CHECKS PASSED ===" -ForegroundColor Green
    exit 0
}
else {
    Write-Host "=== VALIDATION FAILED ===" -ForegroundColor Red
    exit 1
}
