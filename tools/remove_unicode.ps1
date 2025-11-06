#!/usr/bin/env powershell
<#
.SYNOPSIS
Remove all Unicode characters from all files in the project, converting to ASCII-safe equivalents
#>

param(
    [string]$ProjectRoot = "."
)

$ErrorActionPreference = "Continue"

# Map Unicode characters to ASCII replacements
$unicodeMap = @{
    '' = '[LIST]'
    '' = '[NAV]'
    '' = '[*]'
    '' = '[*]'
    '' = '[*]'
    '' = '[*]'
    '' = '[*]'
    '' = '[*]'
    '' = '[*]'
    '' = '[*]'
    '' = '[*]'
    '' = '[*]'
    '' = '[CHECK]'
    '' = '[*]'
    '' = '[*]'
    '' = '[*]'
    '' = '->'
    '' = '[down]'
    '' = '[check]'
    '' = '[x]'
    '' = '[warning]'
    '' = '[error]'
    '' = '[ok]'
    '' = '[star]'
    '' = '[rocket]'
    '' = '[note]'
    '' = '[link]'
    '' = '[chart]'
    '' = '[search]'
    '' = '[gear]'
    '' = '[package]'
    '' = '[tools]'
    '' = '[lightning]'
    '' = '[fire]'
    '' = '[idea]'
    '' = '[timer]'
    '' = '[calendar]'
    '' = '[target]'
    '' = '[pin]'
}

# Additional regex patterns for any remaining Unicode
$unicodePatterns = @(
    '[^\x00-\x7F]'  # Any non-ASCII character
)

Write-Host "Starting Unicode removal process..."
$filesProcessed = 0
$filesModified = 0
$totalReplacements = 0

# Get all files (except .git, .venv, node_modules, .pio)
$files = Get-ChildItem -Path $ProjectRoot -Recurse -File `
    | Where-Object { 
        $path = $_.FullName
        $path -notlike '*\.git*' -and `
        $path -notlike '*\.venv*' -and `
        $path -notlike '*\node_modules*' -and `
        $path -notlike '*\.pio*' -and `
        $path -notlike '*\build*' -and `
        $path -notlike '*\.pytest*'
    }

Write-Host "Processing $($files.Count) files..."

foreach ($file in $files) {
    $filesProcessed++
    
    if ($filesProcessed % 100 -eq 0) {
        Write-Host "  Progress: $filesProcessed/$($files.Count) files processed"
    }
    
    try {
        # Read file as ASCII to detect if it has Unicode
        $originalContent = Get-Content -Path $file.FullName -Raw -Encoding UTF8 -ErrorAction SilentlyContinue
        
        if ($null -eq $originalContent) {
            continue
        }
        
        $modifiedContent = $originalContent
        $fileModified = $false
        $replacementCount = 0
        
        # Replace mapped Unicode characters
        foreach ($unicode in $unicodeMap.Keys) {
            if ($modifiedContent -like "*$unicode*") {
                $modifiedContent = $modifiedContent -replace [regex]::Escape($unicode), $unicodeMap[$unicode]
                $fileModified = $true
                $replacementCount += ([regex]::Matches($originalContent, [regex]::Escape($unicode))).Count
            }
        }
        
        # Replace any remaining non-ASCII characters with [UNICODE]
        $hasNonAscii = $modifiedContent -match '[^\x00-\x7F]'
        if ($hasNonAscii) {
            $modifiedContent = $modifiedContent -replace '[^\x00-\x7F]', '[UNICODE]'
            $fileModified = $true
            $replacementCount++
        }
        
        # Write back if modified
        if ($fileModified) {
            # Determine correct encoding based on file type
            $encoding = [System.Text.Encoding]::ASCII
            
            # For .md and text files, use ASCII
            [System.IO.File]::WriteAllText($file.FullName, $modifiedContent, $encoding)
            
            $filesModified++
            $totalReplacements += $replacementCount
            
            Write-Host "  FIXED: $($file.FullName) ($replacementCount replacements)"
        }
    }
    catch {
        Write-Host "  ERROR processing $($file.FullName): $_"
    }
}

Write-Host ""
Write-Host "============================================"
Write-Host "UNICODE REMOVAL COMPLETE"
Write-Host "============================================"
Write-Host "Files processed: $filesProcessed"
Write-Host "Files modified: $filesModified"
Write-Host "Total replacements: $totalReplacements"
Write-Host "============================================"
