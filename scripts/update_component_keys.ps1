<#
.SYNOPSIS
    Update old component JSON keys across the repository.

.DESCRIPTION
    Scans files under a root path for occurrences of the legacy keys
    `component_name` and `component_type` and updates them to the
    shorter keys `name` and `type` respectively.

    The script only processes files that actually contain the legacy
    tokens and creates a timestamped backup for each modified file.

.NOTES
    - Runs on Windows PowerShell (tested with PowerShell 5.1).
    - Writes output files as ASCII (no BOM) to follow project rules.
    - By default operates in-place; use -WhatIf for a dry run.

.PARAMETER Root
    Root directory to scan. Defaults to the script's parent directory.

.PARAMETER WhatIf
    If specified, prints the files that would be changed without
    modifying them.

.PARAMETER NoBackup
    If specified, skips creating backup copies of changed files.

EXAMPLE
    .\update_component_keys.ps1 -Root .. -WhatIf

#>

param(
    [string]$Root = (Split-Path -Parent $MyInvocation.MyCommand.Path),
    [switch]$WhatIf,
    [switch]$NoBackup
)

Write-Host "Scanning root: $Root" -ForegroundColor Cyan

$extensions = @('*.json','*.md','*.c','*.cpp','*.h','*.hpp','*.py','*.ps1','*.txt')

# Find files that contain either legacy token; Select-String is fast
$matches = @()
foreach ($ext in $extensions) {
    $matches += Select-String -Path (Join-Path $Root "**\$ext") -Pattern 'component_name|component_type' -SimpleMatch -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Path -Unique
}

if (-not $matches) {
    Write-Host "No files found containing 'component_name' or 'component_type'." -ForegroundColor Yellow
    return
}

Write-Host "Found $($matches.Count) candidate file(s)." -ForegroundColor Green

foreach ($file in $matches) {
    try {
        $text = Get-Content -Raw -Encoding UTF8 -LiteralPath $file -ErrorAction Stop
    } catch {
        Write-Warning "Failed to read $file: $_"
        continue
    }

    $orig = $text

    # Perform conservative replacements only on JSON keys and exact quoted tokens.
    # This avoids accidental renames inside larger strings (like paths) or identifiers.

    # JSON key replacements: "component_name" :  --> "name" :
    $text = $text -replace '"component_name"\s*:', '"name":'
    $text = $text -replace "'component_name'\s*:", "'name':"

    # Exact quoted token replacements: "component_name" -> "name"
    $text = $text -replace '"component_name"', '"name"'
    $text = $text -replace "'component_name'", "'name'"

    # component_type -> type (same conservative approach)
    $text = $text -replace '"component_type"\s*:', '"type":'
    $text = $text -replace "'component_type'\s*:", "'type':"
    $text = $text -replace '"component_type"', '"type"'
    $text = $text -replace "'component_type'", "'type'"

    if ($orig -ne $text) {
        if ($WhatIf) {
            Write-Host "[DRY RUN] Would update: $file" -ForegroundColor Yellow
            continue
        }

        if (-not $NoBackup) {
            $bak = "$file.bak_$(Get-Date -Format 'yyyyMMddHHmmss')"
            try {
                Copy-Item -LiteralPath $file -Destination $bak -ErrorAction Stop
                Write-Host "Backup created: $bak" -ForegroundColor DarkCyan
            } catch {
                Write-Warning "Failed to create backup for $file: $_"
            }
        }

        try {
            # Write ASCII without BOM per project requirements
            $text | Out-File -LiteralPath $file -Encoding Ascii -Force
            Write-Host "Updated: $file" -ForegroundColor Green
        } catch {
            Write-Warning "Failed to write file $file: $_"
        }
    } else {
        Write-Host "No relevant replacements in: $file" -ForegroundColor Gray
    }
}

Write-Host "Done." -ForegroundColor Cyan
