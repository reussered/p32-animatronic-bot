# ASCII Validation Script - Pure ASCII Only
# Enforces project coding standards

param(
    [switch]$Fix = $false,
    [string]$Path = "."
)

Write-Host "ASCII VALIDATION SYSTEM" -ForegroundColor Green
Write-Host "Scanning for Unicode violations..." -ForegroundColor Yellow

$unicodeCount = 0
$filenameCount = 0
$filesScanned = 0

$PATTERNS = @("*.ps1", "*.py", "*.cpp", "*.hpp", "*.c", "*.h", "*.md", "*.txt", "*.json")
$EXCLUDE = @(".git", ".pio", ".venv", "__pycache__", ".vs")

function Test-ASCIIContent {
    param([string]$Content, [string]$FilePath)
    
    $violations = @()
    $lines = $Content -split "`n"
    
    for ($i = 0; $i -lt $lines.Count; $i++) {
        $line = $lines[$i]
        for ($j = 0; $j -lt $line.Length; $j++) {
            $char = $line[$j]
            $code = [int][char]$char
            
            # Allow printable ASCII (32-126) plus tab(9), newline(10), carriage return(13)
            if ($code -gt 126 -or ($code -lt 32 -and $code -notin @(9, 10, 13))) {
                $violations += @{
                    Line = $i + 1
                    Column = $j + 1
                    Character = $char
                    Code = $code
                }
            }
        }
    }
    
    return $violations
}

function Test-FileName {
    param([string]$FilePath)
    
    $fileName = Split-Path $FilePath -Leaf
    $extension = [System.IO.Path]::GetExtension($fileName)
    $baseName = [System.IO.Path]::GetFileNameWithoutExtension($fileName)
    
    # Check for class definition
    $hasClass = $false
    $className = ""
    
    if ($extension -in @(".hpp", ".cpp", ".h")) {
        try {
            $content = Get-Content $FilePath -Raw -ErrorAction SilentlyContinue
            if ($content -match "class\s+(\w+)") {
                $hasClass = $true
                $className = $matches[1]
            }
        } catch {
            # Continue if cannot read
        }
    }
    
    if ($hasClass) {
        # Class file must match class name
        if ($baseName -ne $className) {
            return @{
                Issue = "Class name mismatch"
                Expected = "$className$extension"
                Actual = $fileName
                Class = $className
            }
        }
    } else {
        # Non-class file must be lowercase
        if ($baseName -cne $baseName.ToLower()) {
            return @{
                Issue = "Not lowercase"
                Expected = "$($baseName.ToLower())$extension"
                Actual = $fileName
                Class = $null
            }
        }
    }
    
    return $null
}

# Get all files to scan
$allFiles = @()
foreach ($pattern in $PATTERNS) {
    $files = Get-ChildItem -Path $Path -Filter $pattern -Recurse -File -ErrorAction SilentlyContinue
    
    $filtered = $files | Where-Object {
        $isExcluded = $false
        foreach ($exclude in $EXCLUDE) {
            if ($_.FullName -like "*\$exclude\*") {
                $isExcluded = $true
                break
            }
        }
        return -not $isExcluded
    }
    
    $allFiles += $filtered
}

Write-Host "Found $($allFiles.Count) files to scan" -ForegroundColor Cyan

# Scan each file
foreach ($file in $allFiles) {
    $filesScanned++
    $filePath = $file.FullName
    $relativePath = $filePath.Replace((Get-Location).Path + "\", "")
    
    if ($filesScanned % 10 -eq 0) {
        Write-Host "Progress: $filesScanned/$($allFiles.Count)" -ForegroundColor Gray
    }
    
    # Check filename
    $filenameIssue = Test-FileName -FilePath $filePath
    if ($filenameIssue) {
        $filenameCount++
        Write-Host "[FILENAME] $relativePath" -ForegroundColor Red
        Write-Host "  Issue: $($filenameIssue.Issue)" -ForegroundColor Yellow
        Write-Host "  Expected: $($filenameIssue.Expected)" -ForegroundColor Green
        Write-Host "  Actual: $($filenameIssue.Actual)" -ForegroundColor Red
        
        if ($Fix) {
            try {
                $newName = $filenameIssue.Expected
                $newPath = Join-Path (Split-Path $filePath -Parent) $newName
                Rename-Item -Path $filePath -NewName $newName -Force
                Write-Host "  FIXED: Renamed to $newName" -ForegroundColor Green
            } catch {
                Write-Host "  ERROR: Cannot rename - $($_.Exception.Message)" -ForegroundColor Red
            }
        }
    }
    
    # Check content
    try {
        $content = Get-Content $filePath -Raw -Encoding UTF8 -ErrorAction SilentlyContinue
        if ($content) {
            $violations = Test-ASCIIContent -Content $content -FilePath $filePath
            
            if ($violations.Count -gt 0) {
                $unicodeCount += $violations.Count
                Write-Host "[UNICODE] $relativePath" -ForegroundColor Red
                
                foreach ($v in $violations) {
                    Write-Host "  Line $($v.Line), Col $($v.Column): Char '$($v.Character)' Code=$($v.Code)" -ForegroundColor Yellow
                }
                
                if ($Fix) {
                    # Convert to pure ASCII
                    $cleanContent = ""
                    foreach ($char in $content.ToCharArray()) {
                        $code = [int][char]$char
                        if (($code -ge 32 -and $code -le 126) -or $code -in @(9, 10, 13)) {
                            $cleanContent += $char
                        } else {
                            # Replace common Unicode with ASCII equivalents
                            switch ($code) {
                                8220 { $cleanContent += '"' }  # Left double quote
                                8221 { $cleanContent += '"' }  # Right double quote
                                8216 { $cleanContent += "'" }  # Left single quote
                                8217 { $cleanContent += "'" }  # Right single quote
                                8211 { $cleanContent += "-" }  # En dash
                                8212 { $cleanContent += "-" }  # Em dash
                                default { 
                                    # Skip other Unicode characters
                                }
                            }
                        }
                    }
                    
                    try {
                        Set-Content -Path $filePath -Value $cleanContent -Encoding UTF8 -NoNewline
                        Write-Host "  FIXED: Converted to ASCII" -ForegroundColor Green
                    } catch {
                        Write-Host "  ERROR: Cannot fix - $($_.Exception.Message)" -ForegroundColor Red
                    }
                }
            }
        }
    } catch {
        Write-Host "[ERROR] Cannot read $relativePath - $($_.Exception.Message)" -ForegroundColor Red
    }
}

# Summary
Write-Host ""
Write-Host "VALIDATION COMPLETE" -ForegroundColor Green -BackgroundColor Black
Write-Host "Files scanned: $filesScanned" -ForegroundColor Cyan
Write-Host "Unicode violations: $unicodeCount" -ForegroundColor $(if ($unicodeCount -eq 0) { "Green" } else { "Red" })
Write-Host "Filename violations: $filenameCount" -ForegroundColor $(if ($filenameCount -eq 0) { "Green" } else { "Red" })

$timestamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"
$reportFile = "ascii_validation_$timestamp.txt"

$report = @"
ASCII VALIDATION REPORT
Generated: $(Get-Date)
Files Scanned: $filesScanned
Unicode Violations: $unicodeCount  
Filename Violations: $filenameCount
Status: $(if ($unicodeCount -eq 0 -and $filenameCount -eq 0) { "COMPLIANT" } else { "NON-COMPLIANT" })

RULES:
1. Only ASCII characters 32-126 plus tab/newline/return
2. Lowercase filenames except class definition files
3. Class files must match class name exactly
"@

$report | Out-File -FilePath $reportFile -Encoding ASCII
Write-Host "Report: $reportFile" -ForegroundColor Green

if ($unicodeCount -eq 0 -and $filenameCount -eq 0) {
    Write-Host "PROJECT IS COMPLIANT!" -ForegroundColor Green -BackgroundColor Black
} else {
    Write-Host "VIOLATIONS FOUND - Run with -Fix to correct" -ForegroundColor Yellow
}