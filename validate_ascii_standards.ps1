?# ASCII and Filename Validation Script
# Enforces project coding standards for character usage and file naming

param(
    [switch]$Fix = $false,
    [string]$Path = "."
)

$ErrorActionPreference = "Continue"

Write-Host "ASCII AND FILENAME VALIDATION SYSTEM" -ForegroundColor Green
Write-Host "Scanning for violations of project coding standards..." -ForegroundColor Yellow

# Initialize counters
$unicodeViolations = 0
$filenameViolations = 0
$filesScanned = 0

# Define file patterns to scan
$SCAN_PATTERNS = @("*.ps1", "*.py", "*.cpp", "*.hpp", "*.c", "*.h", "*.md", "*.txt", "*.json")

# Define directories to exclude
$EXCLUDE_DIRS = @(".git", ".pio", ".venv", "__pycache__", ".vs", "node_modules")

function Test-ASCII {
    param([string]$Content, [string]$FilePath)
    
    $violations = @()
    $lines = $Content -split "`n"
    
    for ($i = 0; $i -lt $lines.Count; $i++) {
        $line = $lines[$i]
        for ($j = 0; $j -lt $line.Length; $j++) {
            $char = $line[$j]
            $charCode = [int][char]$char
            
            # Check if character is outside standard ASCII range (32-126) plus common whitespace
            if ($charCode -gt 126 -or ($charCode -lt 32 -and $charCode -notin @(9, 10, 13))) {
                $violations += @{
                    Line = $i + 1
                    Column = $j + 1
                    Character = $char
                    CharCode = $charCode
                    Context = $line.Substring([Math]::Max(0, $j-5), [Math]::Min(10, $line.Length - [Math]::Max(0, $j-5)))
                }
            }
        }
    }
    
    return $violations
}

function Test-FilenameConvention {
    param([string]$FilePath)
    
    $fileName = Split-Path $FilePath -Leaf
    $extension = [System.IO.Path]::GetExtension($fileName)
    $baseName = [System.IO.Path]::GetFileNameWithoutExtension($fileName)
    
    # Check if file contains class definition
    $isClassFile = $false
    $expectedClassName = ""
    
    if ($extension -in @(".hpp", ".cpp", ".h")) {
        try {
            $content = Get-Content $FilePath -Raw -ErrorAction SilentlyContinue
            if ($content -match "class\s+(\w+)") {
                $isClassFile = $true
                $expectedClassName = $matches[1]
            }
        } catch {
            # Continue if file can't be read
        }
    }
    
    # Validate filename convention
    if ($isClassFile) {
        # Class files must match class name exactly
        if ($baseName -ne $expectedClassName) {
            return @{
                Violation = "Class filename mismatch"
                Expected = "$expectedClassName$extension"
                Actual = $fileName
                ClassName = $expectedClassName
            }
        }
    } else {
        # Non-class files must be lowercase
        if ($baseName -cne $baseName.ToLower()) {
            return @{
                Violation = "Filename not lowercase"
                Expected = "$($baseName.ToLower())$extension"
                Actual = $fileName
                ClassName = $null
            }
        }
    }
    
    return $null
}

function Get-AllFiles {
    param([string]$BasePath)
    
    $allFiles = @()
    
    foreach ($pattern in $SCAN_PATTERNS) {
        $files = Get-ChildItem -Path $BasePath -Filter $pattern -Recurse -File -ErrorAction SilentlyContinue
        
        # Filter out excluded directories
        $filteredFiles = $files | Where-Object {
            $exclude = $false
            foreach ($excludeDir in $EXCLUDE_DIRS) {
                if ($_.FullName -like "*\$excludeDir\*") {
                    $exclude = $true
                    break
                }
            }
            return -not $exclude
        }
        
        $allFiles += $filteredFiles
    }
    
    return $allFiles
}

# Main validation loop
Write-Host "Scanning files..." -ForegroundColor Cyan

$files = Get-AllFiles -BasePath $Path

foreach ($file in $files) {
    $filesScanned++
    $filePath = $file.FullName
    $relativePath = $filePath.Replace((Get-Location).Path + "\", "")
    
    Write-Progress -Activity "Validating Files" -Status $relativePath -PercentComplete (($filesScanned / $files.Count) * 100)
    
    # Check filename convention
    $filenameResult = Test-FilenameConvention -FilePath $filePath
    if ($filenameResult) {
        $filenameViolations++
        Write-Host "[FILENAME VIOLATION] $relativePath" -ForegroundColor Red
        Write-Host "  Issue: $($filenameResult.Violation)" -ForegroundColor Yellow
        Write-Host "  Expected: $($filenameResult.Expected)" -ForegroundColor Green
        Write-Host "  Actual: $($filenameResult.Actual)" -ForegroundColor Red
        
        if ($filenameResult.ClassName) {
            Write-Host "  Class: $($filenameResult.ClassName)" -ForegroundColor Cyan
        }
        
        if ($Fix) {
            $newPath = Join-Path (Split-Path $filePath -Parent) $filenameResult.Expected
            try {
                Rename-Item -Path $filePath -NewName $filenameResult.Expected -Force
                Write-Host "  FIXED: Renamed to $($filenameResult.Expected)" -ForegroundColor Green
            } catch {
                Write-Host "  ERROR: Could not rename file - $($_.Exception.Message)" -ForegroundColor Red
            }
        }
    }
    
    # Check ASCII content
    try {
        $content = Get-Content $filePath -Raw -Encoding UTF8 -ErrorAction SilentlyContinue
        if ($content) {
            $asciiViolations = Test-ASCII -Content $content -FilePath $filePath
            
            if ($asciiViolations.Count -gt 0) {
                $unicodeViolations += $asciiViolations.Count
                Write-Host "[UNICODE VIOLATION] $relativePath" -ForegroundColor Red
                
                foreach ($violation in $asciiViolations) {
                    Write-Host "  Line $($violation.Line), Col $($violation.Column): '$($violation.Character)' (Code: $($violation.CharCode))" -ForegroundColor Yellow
                    Write-Host "  Context: $($violation.Context)" -ForegroundColor Gray
                }
                
                if ($Fix) {
                    # Replace common Unicode characters with ASCII equivalents
                    $fixedContent = $content
                    $fixedContent = $fixedContent -replace "", "[WARNING]"
                    $fixedContent = $fixedContent -replace "", "[WARNING]"
                    $fixedContent = $fixedContent -replace "", "[SUCCESS]"
                    $fixedContent = $fixedContent -replace "", ""
                    $fixedContent = $fixedContent -replace "", ""
                    $fixedContent = $fixedContent -replace "", ""
                    $fixedContent = $fixedContent -replace "", ""
                    $fixedContent = $fixedContent -replace "", ""
                    $fixedContent = $fixedContent -replace """, '"'
                    $fixedContent = $fixedContent -replace """, '"'
                    $fixedContent = $fixedContent -replace "'", "'"
                    $fixedContent = $fixedContent -replace "'", "'"
                    $fixedContent = $fixedContent -replace "-", "-"
                    $fixedContent = $fixedContent -replace "-", "-"
                    
                    # Remove any remaining non-ASCII characters (keep only printable ASCII plus whitespace)
                    $asciiOnly = ""
                    foreach ($char in $fixedContent.ToCharArray()) {
                        $code = [int][char]$char
                        if (($code -ge 32 -and $code -le 126) -or $code -in @(9, 10, 13)) {
                            $asciiOnly += $char
                        }
                    }
                    
                    try {
                        Set-Content -Path $filePath -Value $asciiOnly -Encoding UTF8 -NoNewline
                        Write-Host "  FIXED: Removed Unicode characters" -ForegroundColor Green
                    } catch {
                        Write-Host "  ERROR: Could not fix file - $($_.Exception.Message)" -ForegroundColor Red
                    }
                }
            }
        }
    } catch {
        Write-Host "[READ ERROR] $relativePath - $($_.Exception.Message)" -ForegroundColor Red
    }
}

Write-Progress -Activity "Validating Files" -Completed

# Summary report
Write-Host ""
Write-Host "VALIDATION SUMMARY" -ForegroundColor Green -BackgroundColor Black
Write-Host "Files scanned: $filesScanned" -ForegroundColor Cyan
Write-Host "Unicode violations: $unicodeViolations" -ForegroundColor $(if ($unicodeViolations -eq 0) { "Green" } else { "Red" })
Write-Host "Filename violations: $filenameViolations" -ForegroundColor $(if ($filenameViolations -eq 0) { "Green" } else { "Red" })

if ($unicodeViolations -eq 0 -and $filenameViolations -eq 0) {
    Write-Host "ALL STANDARDS COMPLIANT!" -ForegroundColor Green -BackgroundColor Black
} else {
    Write-Host "VIOLATIONS FOUND - PROJECT NOT COMPLIANT" -ForegroundColor Red -BackgroundColor Yellow
    if (-not $Fix) {
        Write-Host "Run with -Fix to automatically correct issues" -ForegroundColor Yellow
    }
}

# Save detailed report
$timestamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"
$reportPath = "validation_report_$timestamp.txt"

$report = @"
ASCII AND FILENAME VALIDATION REPORT
Generated: $(Get-Date)
Path: $Path
Files Scanned: $filesScanned
Unicode Violations: $unicodeViolations
Filename Violations: $filenameViolations

RULES ENFORCED:
1. ASCII characters only (32-126 plus tab/newline/carriage return)
2. Lowercase filenames (except class definition files)
3. Class files must match class name exactly

EXCLUDED DIRECTORIES: $($EXCLUDE_DIRS -join ", ")
SCANNED PATTERNS: $($SCAN_PATTERNS -join ", ")

STATUS: $(if ($unicodeViolations -eq 0 -and $filenameViolations -eq 0) { "COMPLIANT" } else { "NON-COMPLIANT" })
"@

$report | Out-File -FilePath $reportPath -Encoding ASCII
Write-Host "Report saved: $reportPath" -ForegroundColor Green