# PowerShell script for Windows environment
<#
.SYNOPSIS
    Remove Unicode characters from all files in the project
.DESCRIPTION
    This script scans all text files in the project and converts Unicode characters
    to ASCII equivalents or removes them entirely. Ensures compliance with project
    rule requiring ASCII encoding (no BOM).
    
.NOTES
    Based on project rule: All files must be ASCII encoded (no BOM)
    From .github/AI-AGENT-RULES.md
#>

param(
    [switch]$DryRun = $false,
    [switch]$Verbose = $false,
    [string[]]$FileTypes = @("*.cpp", "*.hpp", "*.h", "*.c", "*.py", "*.ps1", "*.md", "*.txt", "*.json", "*.ini", "*.scad", "*.yaml", "*.yml")
)

# Unicode to ASCII replacements
$UnicodeReplacements = @{
    # Smart quotes
    "\u201C" = '"'    # Left double quotation mark
    "\u201D" = '"'    # Right double quotation mark
    "\u2018" = "'"    # Left single quotation mark
    "\u2019" = "'"    # Right single quotation mark
    
    # Dashes
    "\u2013" = "-"    # En dash
    "\u2014" = "--"   # Em dash
    "\u2212" = "-"    # Minus sign
    
    # Ellipsis
    "\u2026" = "..."  # Horizontal ellipsis
    
    # Arrows
    "\u2192" = "->"   # Rightwards arrow
    "\u2190" = "<-"   # Leftwards arrow
    "\u2194" = "<->"  # Left right arrow
    
    # Bullet points
    "\u2022" = "*"    # Bullet
    "\u2023" = "*"    # Triangular bullet
    "\u25CF" = "*"    # Black circle
    
    # Common symbols
    "\u00A9" = "(c)"  # Copyright sign
    "\u00AE" = "(R)"  # Registered sign
    "\u2122" = "(TM)" # Trade mark sign
    "\u00B0" = " deg" # Degree sign
    "\u00B1" = "+/-"  # Plus-minus sign
    "\u00D7" = "x"    # Multiplication sign
    "\u00F7" = "/"    # Division sign
    
    # Currency
    "\u20AC" = "EUR"  # Euro sign
    "\u00A3" = "GBP"  # Pound sign
    "\u00A5" = "YEN"  # Yen sign
    
    # Fractions
    "\u00BD" = "1/2"  # Vulgar fraction one half
    "\u00BC" = "1/4"  # Vulgar fraction one quarter
    "\u00BE" = "3/4"  # Vulgar fraction three quarters
    
    # Mathematical
    "\u221E" = "inf"  # Infinity
    "\u2260" = "!="   # Not equal to
    "\u2264" = "<="   # Less-than or equal to
    "\u2265" = ">="   # Greater-than or equal to
    
    # Checkmarks and X marks
    "\u2713" = "?"    # Check mark (keep as common ASCII alternative)
    "\u2714" = "?"    # Heavy check mark
    "\u2717" = "X"    # Ballot X
    "\u2718" = "X"    # Heavy ballot X
    
    # Non-breaking space
    "\u00A0" = " "    # Non-breaking space
    
    # Zero-width characters (remove entirely)
    "\u200B" = ""     # Zero-width space
    "\u200C" = ""     # Zero-width non-joiner
    "\u200D" = ""     # Zero-width joiner
    "\uFEFF" = ""     # Zero-width no-break space (BOM)
}

# Find all files matching the specified types
$AllFiles = @()
foreach ($FileType in $FileTypes) {
    $Files = Get-ChildItem -Path "." -Recurse -Filter $FileType | Where-Object {
        # Exclude build directories, version control, and temp files
        $_.FullName -notmatch "\\.pio\\" -and 
        $_.FullName -notmatch "\\.git\\" -and
        $_.FullName -notmatch "\\node_modules\\" -and
        $_.FullName -notmatch "\\.vs\\" -and
        $_.FullName -notmatch "\\build\\" -and
        $_.FullName -notmatch "\\.cache\\" -and
        $_.FullName -notmatch "\\temp\\" -and
        $_.FullName -notmatch "\\.tmp"
    }
    $AllFiles += $Files
}

Write-Host "Found $($AllFiles.Count) files to process" -ForegroundColor Green

$TotalFilesModified = 0
$TotalUnicodeCharsReplaced = 0

foreach ($File in $AllFiles) {
    try {
        # Read file content as raw bytes to detect encoding
        $Bytes = [System.IO.File]::ReadAllBytes($File.FullName)
        
        # Skip binary files (simple heuristic: files with many null bytes)
        $NullCount = ($Bytes | Where-Object { $_ -eq 0 }).Count
        if ($NullCount -gt 10) {
            if ($Verbose) {
                Write-Host "Skipping binary file: $($File.FullName)" -ForegroundColor Gray
            }
            continue
        }
        
        # Read content as UTF-8
        $Content = [System.IO.File]::ReadAllText($File.FullName, [System.Text.Encoding]::UTF8)
        $FileModified = $false
        $UnicodeCharsInFile = 0
        
        if ($Verbose) {
            Write-Host "`nProcessing: $($File.FullName)" -ForegroundColor Cyan
        }
        
        # Check if file contains Unicode characters
        $HasUnicode = $false
        for ($i = 0; $i -lt $Content.Length; $i++) {
            $CharCode = [int][char]$Content[$i]
            if ($CharCode -gt 127) {
                $HasUnicode = $true
                break
            }
        }
        
        if (-not $HasUnicode) {
            if ($Verbose) {
                Write-Host "  No Unicode characters found" -ForegroundColor Gray
            }
            continue
        }
        
        # Apply Unicode replacements
        foreach ($Unicode in $UnicodeReplacements.Keys) {
            $Replacement = $UnicodeReplacements[$Unicode]
            if ($Content -match [regex]::Escape($Unicode)) {
                $Content = $Content -replace [regex]::Escape($Unicode), $Replacement
                $FileModified = $true
                $UnicodeCharsInFile++
                
                if ($Verbose) {
                    $UnicodeChar = [char]([int]$Unicode.Substring(2), 16)
                    Write-Host "  Replaced: '$UnicodeChar' ($Unicode) -> '$Replacement'" -ForegroundColor Yellow
                }
            }
        }
        
        # Remove any remaining Unicode characters (convert to ? or remove)
        $CleanedContent = ""
        for ($i = 0; $i -lt $Content.Length; $i++) {
            $Char = $Content[$i]
            $CharCode = [int][char]$Char
            
            if ($CharCode -le 127) {
                # ASCII character - keep as is
                $CleanedContent += $Char
            } elseif ($CharCode -le 255) {
                # Extended ASCII - convert to closest ASCII equivalent or remove
                switch ($CharCode) {
                    192 { $CleanedContent += "A" }    # ?
                    193 { $CleanedContent += "A" }    # ?
                    194 { $CleanedContent += "A" }    # ?
                    195 { $CleanedContent += "A" }    # ?
                    196 { $CleanedContent += "A" }    # ?
                    197 { $CleanedContent += "A" }    # ?
                    199 { $CleanedContent += "C" }    # ?
                    200 { $CleanedContent += "E" }    # ?
                    201 { $CleanedContent += "E" }    # ?
                    202 { $CleanedContent += "E" }    # ?
                    203 { $CleanedContent += "E" }    # ?
                    204 { $CleanedContent += "I" }    # ?
                    205 { $CleanedContent += "I" }    # ?
                    206 { $CleanedContent += "I" }    # ?
                    207 { $CleanedContent += "I" }    # ?
                    209 { $CleanedContent += "N" }    # ?
                    210 { $CleanedContent += "O" }    # ?
                    211 { $CleanedContent += "O" }    # ?
                    212 { $CleanedContent += "O" }    # ?
                    213 { $CleanedContent += "O" }    # ?
                    214 { $CleanedContent += "O" }    # ?
                    217 { $CleanedContent += "U" }    # ?
                    218 { $CleanedContent += "U" }    # ?
                    219 { $CleanedContent += "U" }    # ?
                    220 { $CleanedContent += "U" }    # ?
                    221 { $CleanedContent += "Y" }    # ?
                    224 { $CleanedContent += "a" }    # ?
                    225 { $CleanedContent += "a" }    # ?
                    226 { $CleanedContent += "a" }    # ?
                    227 { $CleanedContent += "a" }    # ?
                    228 { $CleanedContent += "a" }    # ?
                    229 { $CleanedContent += "a" }    # ?
                    231 { $CleanedContent += "c" }    # ?
                    232 { $CleanedContent += "e" }    # ?
                    233 { $CleanedContent += "e" }    # ?
                    234 { $CleanedContent += "e" }    # ?
                    235 { $CleanedContent += "e" }    # ?
                    236 { $CleanedContent += "i" }    # ?
                    237 { $CleanedContent += "i" }    # ?
                    238 { $CleanedContent += "i" }    # ?
                    239 { $CleanedContent += "i" }    # ?
                    241 { $CleanedContent += "n" }    # ?
                    242 { $CleanedContent += "o" }    # ?
                    243 { $CleanedContent += "o" }    # ?
                    244 { $CleanedContent += "o" }    # ?
                    245 { $CleanedContent += "o" }    # ?
                    246 { $CleanedContent += "o" }    # ?
                    249 { $CleanedContent += "u" }    # ?
                    250 { $CleanedContent += "u" }    # ?
                    251 { $CleanedContent += "u" }    # ?
                    252 { $CleanedContent += "u" }    # ?
                    253 { $CleanedContent += "y" }    # ?
                    255 { $CleanedContent += "y" }    # ?
                    default { 
                        # Skip other extended ASCII
                        $FileModified = $true
                        $UnicodeCharsInFile++
                        if ($Verbose) {
                            Write-Host "  Removed extended ASCII: char code $CharCode" -ForegroundColor Red
                        }
                    }
                }
            } else {
                # Unicode character - remove
                $FileModified = $true
                $UnicodeCharsInFile++
                if ($Verbose) {
                    Write-Host "  Removed Unicode: '$Char' (code $CharCode)" -ForegroundColor Red
                }
            }
        }
        
        $Content = $CleanedContent
        
        if ($FileModified) {
            $TotalFilesModified++
            $TotalUnicodeCharsReplaced += $UnicodeCharsInFile
            
            Write-Host "Modified: $($File.FullName) ($UnicodeCharsInFile Unicode chars processed)" -ForegroundColor Green
            
            if (-not $DryRun) {
                # Write as ASCII encoding (no BOM)
                $Encoding = New-Object System.Text.ASCIIEncoding
                [System.IO.File]::WriteAllText($File.FullName, $Content, $Encoding)
                Write-Host "  File saved as ASCII (no BOM)" -ForegroundColor Green
            } else {
                Write-Host "  DRY RUN: Changes not saved" -ForegroundColor Magenta
            }
        }
        elseif ($Verbose) {
            Write-Host "  No Unicode characters found after scanning" -ForegroundColor Gray
        }
    }
    catch {
        Write-Error "Error processing file $($File.FullName): $($_.Exception.Message)"
    }
}

# Summary
Write-Host "`nSUMMARY" -ForegroundColor Cyan
Write-Host "Files processed: $($AllFiles.Count)" -ForegroundColor White
Write-Host "Files modified: $TotalFilesModified" -ForegroundColor Green
Write-Host "Total Unicode characters processed: $TotalUnicodeCharsReplaced" -ForegroundColor Yellow

if ($DryRun) {
    Write-Host "`nDRY RUN MODE - No files were actually modified" -ForegroundColor Magenta
    Write-Host "Run without -DryRun to apply changes" -ForegroundColor Magenta
} else {
    Write-Host "`nUnicode cleanup complete!" -ForegroundColor Green
    Write-Host "All files now use ASCII encoding (no BOM)" -ForegroundColor White
}

# Show file types processed
Write-Host "`nFile types processed:" -ForegroundColor Cyan
foreach ($FileType in $FileTypes) {
    $Count = ($AllFiles | Where-Object { $_.Name -like $FileType }).Count
    if ($Count -gt 0) {
        Write-Host "  $FileType : $Count files" -ForegroundColor Gray
    }
}