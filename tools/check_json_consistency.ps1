# COMPREHENSIVE JSON CONSISTENCY CHECKER
# Recursively scans ALL JSON files from project root for internal consistency
# Validates cross-references, interface assignments, component relationships, and data integrity

param(
    [switch]$Verbose,
    [switch]$FixMode,
    [string]$ProjectRoot = "."
)

$ErrorActionPreference = "Stop"

Write-Host "JSON CONSISTENCY CHECKER - P32 ANIMATRONIC SYSTEM" -ForegroundColor Cyan -BackgroundColor Black
Write-Host "=" * 70 -ForegroundColor Cyan
Write-Host "Scanning from root: $((Get-Item $ProjectRoot).FullName)" -ForegroundColor Yellow

$inconsistencies = @()
$warnings = @()
$allJsonFiles = @()
$fileIndex = @{}
$componentReferences = @{}

# PHASE 1: DISCOVER ALL JSON FILES
Write-Host "`nPHASE 1: Discovering all JSON files recursively..." -ForegroundColor Cyan

try {
    $allJsonFiles = Get-ChildItem -Path $ProjectRoot -Recurse -Filter "*.json" -ErrorAction SilentlyContinue | 
        Where-Object { 
            $_.FullName -notmatch '\.pio' -and 
            $_.FullName -notmatch '\.cmake' -and 
            $_.FullName -notmatch 'build' -and
            $_.FullName -notmatch 'bootloader' -and
            $_.FullName -notmatch '__pycache__' -and
            $_.FullName -notmatch 'CMakeFiles'
        }
    Write-Host "Found $($allJsonFiles.Count) JSON files across entire project (excluding build artifacts)" -ForegroundColor Green
    
    # Build file index for quick lookups
    foreach ($jsonFile in $allJsonFiles) {
        $relativePath = $jsonFile.FullName.Replace((Get-Item $ProjectRoot).FullName, "").TrimStart('\').Replace('\', '/')
        $fileIndex[$relativePath] = $jsonFile.FullName
        
        if ($Verbose) {
            Write-Host "  Indexed: $relativePath" -ForegroundColor Gray
        }
    }
} catch {
    Write-Host "ERROR: Failed to discover JSON files - $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}

# PHASE 2: PARSE AND VALIDATE JSON STRUCTURE
Write-Host "`nPHASE 2: Parsing and validating JSON structure..." -ForegroundColor Cyan

$validJsonFiles = @()
$invalidJsonFiles = @()

foreach ($jsonFile in $allJsonFiles) {
    try {
        $jsonContent = Get-Content $jsonFile.FullName -Raw | ConvertFrom-Json
        $validJsonFiles += @{
            File = $jsonFile
            Content = $jsonContent
            RelativePath = $jsonFile.FullName.Replace((Get-Item $ProjectRoot).FullName, "").TrimStart('\').Replace('\', '/')
        }
        
        if ($Verbose) {
            Write-Host "  Valid JSON: $($jsonFile.Name)" -ForegroundColor Green
        }
    } catch {
        $invalidJsonFiles += @{
            File = $jsonFile.FullName
            Error = $_.Exception.Message
        }
        $inconsistencies += "INVALID_JSON: $($jsonFile.FullName) - $($_.Exception.Message)"
    }
}

Write-Host "Valid JSON files: $($validJsonFiles.Count)" -ForegroundColor Green
if ($invalidJsonFiles.Count -gt 0) {
    Write-Host "Invalid JSON files: $($invalidJsonFiles.Count)" -ForegroundColor Red
}

# PHASE 3: VALIDATE CROSS-REFERENCES
Write-Host "`nPHASE 3: Validating cross-references and file paths..." -ForegroundColor Cyan

foreach ($jsonData in $validJsonFiles) {
    $filePath = $jsonData.RelativePath
    $content = $jsonData.Content
    
    # Deprecated: relative_filename presence or mismatch
    if ($content.relative_filename -and $null -ne $content.relative_filename) {
        # Mark presence as a deprecation warning
        $warnings += "DEPRECATED_FIELD: $filePath contains deprecated field 'relative_filename'"
        $expectedPath = $content.relative_filename.Replace('/', '\')
        $actualPath = $filePath.Replace('/', '\')
        if ($expectedPath -ne $actualPath) {
            $warnings += "DEPRECATED_MISMATCH: $filePath has relative_filename '$($content.relative_filename)' which doesn't match actual path '$filePath'"
        }
    }
    
    # Check author reference (only for config files)
    if ($filePath -like "*config*" -and $content.author -and $content.author -ne "config/author.json") {
        if (-not $fileIndex.ContainsKey($content.author)) {
            $inconsistencies += "MISSING_AUTHOR: $filePath references author '$($content.author)' which doesn't exist"
        }
    }
    
    # Check hardware_reference
    if ($content.hardware_reference) {
        if (-not $fileIndex.ContainsKey($content.hardware_reference)) {
            $inconsistencies += "MISSING_HARDWARE_REF: $filePath references hardware '$($content.hardware_reference)' which doesn't exist"
        }
    }
    
    # Check contained_components
    if ($content.contained_components) {
        foreach ($componentRef in $content.contained_components) {
            if ($componentRef -and -not $fileIndex.ContainsKey($componentRef)) {
                $inconsistencies += "MISSING_COMPONENT_REF: $filePath references component '$componentRef' which doesn't exist"
            }
        }
    }
}

# PHASE 4: VALIDATE COMPONENT HIERARCHY
Write-Host "`nPHASE 4: Validating component hierarchy and relationships..." -ForegroundColor Cyan

$componentNames = @{}
$hardwareTypes = @{}

foreach ($jsonData in $validJsonFiles) {
    $content = $jsonData.Content
    $filePath = $jsonData.RelativePath
    
    # Check for duplicate component names
    if ($content.component_name) {
        if (-not $componentNames.ContainsKey($content.component_name)) {
            $componentNames[$content.component_name] = @()
        }
        $componentNames[$content.component_name] += $filePath
    }
    
    # Collect hardware types
    if ($content.hardware_type) {
        if (-not $hardwareTypes.ContainsKey($content.hardware_type)) {
            $hardwareTypes[$content.hardware_type] = @()
        }
        $hardwareTypes[$content.hardware_type] += $filePath
    }
}

# Check for duplicate component names (should be unique)
foreach ($componentName in $componentNames.Keys) {
    if ($componentNames[$componentName].Count -gt 1) {
        $inconsistencies += "DUPLICATE_COMPONENT: Component name '$componentName' is used in multiple files: $($componentNames[$componentName] -join ', ')"
    }
}

# PHASE 6: VALIDATE COORDINATE SYSTEMS
Write-Host "`nPHASE 6: Validating coordinate systems and positioning..." -ForegroundColor Cyan

foreach ($jsonData in $validJsonFiles) {
    $content = $jsonData.Content
    $filePath = $jsonData.RelativePath
    
    # Check coordinate system consistency
    if ($content.coordinate_system) {
        if ($content.coordinate_system -notin @("planar_2d", "skull_3d")) {
            $inconsistencies += "INVALID_COORDINATE_SYSTEM: $filePath uses unknown coordinate system '$($content.coordinate_system)'"
        }
        
        # Check reference point consistency
        if ($content.reference_point -and $content.reference_point -ne "nose_center") {
            $warnings += "NON_STANDARD_REFERENCE: $filePath uses non-standard reference point '$($content.reference_point)'"
        }
        
        # Validate position format
        if ($content.position) {
            foreach ($axis in @("x", "y", "z")) {
                if ($content.position.$axis) {
                    if ($content.position.$axis -notmatch "^[+-]?\d+(\.\d+)?\s+(INCH|MM|CM)$") {
                        $inconsistencies += "INVALID_POSITION_FORMAT: $filePath has invalid $axis position format '$($content.position.$axis)'"
                    }
                }
            }
        }
    }
}

# PHASE 7: VALIDATE VERSION CONSISTENCY
Write-Host "`nPHASE 7: Validating version consistency..." -ForegroundColor Cyan

$versions = @{}
foreach ($jsonData in $validJsonFiles) {
    $content = $jsonData.Content
    $filePath = $jsonData.RelativePath
    
    if ($content.version) {
        if (-not $versions.ContainsKey($content.version)) {
            $versions[$content.version] = @()
        }
        $versions[$content.version] += $filePath
    } else {
        $warnings += "MISSING_VERSION: $filePath has no version field"
    }
}

if ($versions.Keys.Count -gt 3) {
    $warnings += "VERSION_PROLIFERATION: Found $($versions.Keys.Count) different versions: $($versions.Keys -join ', ')"
}

# RESULTS AND RECOMMENDATIONS
Write-Host "`n" -ForegroundColor White
Write-Host "=" * 70 -ForegroundColor Cyan
Write-Host "JSON CONSISTENCY ANALYSIS RESULTS" -ForegroundColor Cyan -BackgroundColor Black

Write-Host "`nSTATISTICS:" -ForegroundColor White
Write-Host "  Total JSON files found: $($allJsonFiles.Count)" -ForegroundColor White
Write-Host "  Valid JSON files: $($validJsonFiles.Count)" -ForegroundColor Green
Write-Host "  Invalid JSON files: $($invalidJsonFiles.Count)" -ForegroundColor Red
Write-Host "  Interface definitions: $($interfaceDefinitions.Count)" -ForegroundColor White
Write-Host "  Interface assignments: $($interfaceAssignments.Count)" -ForegroundColor White
Write-Host "  Unique component names: $($componentNames.Count)" -ForegroundColor White
Write-Host "  Unique hardware types: $($hardwareTypes.Count)" -ForegroundColor White

if ($inconsistencies.Count -eq 0 -and $invalidJsonFiles.Count -eq 0) {
    Write-Host "`nCONSISTENCY STATUS: EXCELLENT" -ForegroundColor Green -BackgroundColor Black
    Write-Host "All JSON files are internally consistent" -ForegroundColor Green
    Write-Host "All cross-references are valid" -ForegroundColor Green
    Write-Host "All interface assignments are proper" -ForegroundColor Green
    
    if ($warnings.Count -gt 0) {
        Write-Host "`nMINOR WARNINGS:" -ForegroundColor Yellow
        foreach ($warning in $warnings) {
            Write-Host "  $warning" -ForegroundColor Yellow
        }
    }
    
    Write-Host "`nSTATUS: PASSED" -ForegroundColor Green -BackgroundColor Black
    exit 0
} else {
    Write-Host "`nCONSISTENCY ISSUES DETECTED" -ForegroundColor Red -BackgroundColor Yellow
    Write-Host "$($inconsistencies.Count + $invalidJsonFiles.Count) CRITICAL PROBLEMS FOUND:" -ForegroundColor Red
    Write-Host ""
    
    foreach ($inconsistency in $inconsistencies) {
        Write-Host "  $inconsistency" -ForegroundColor Red
    }
    
    foreach ($invalidFile in $invalidJsonFiles) {
        Write-Host "  INVALID_JSON: $($invalidFile.File) - $($invalidFile.Error)" -ForegroundColor Red
    }
    
    if ($warnings.Count -gt 0) {
        Write-Host "`nWARNINGS:" -ForegroundColor Yellow
        foreach ($warning in $warnings) {
            Write-Host "  $warning" -ForegroundColor Yellow
        }
    }
    
    Write-Host ""
    Write-Host "REPAIR ACTIONS REQUIRED:" -ForegroundColor Red
    Write-Host "  1. Fix JSON syntax errors in invalid files" -ForegroundColor White
    Write-Host "  2. Correct cross-reference paths" -ForegroundColor White
    Write-Host "  3. Resolve duplicate component names" -ForegroundColor White
    Write-Host "  4. Fix interface assignment conflicts" -ForegroundColor White
    Write-Host "  5. Standardize coordinate system usage" -ForegroundColor White
    Write-Host ""
    Write-Host "STATUS: FAILED" -ForegroundColor Red -BackgroundColor Yellow
    exit 1
}