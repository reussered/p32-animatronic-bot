# This script compares files in the source and destination directories where conflicts occurred.
# It generates a report of differences for review.

param (
    [string]$SourceDir = "F:\GitHub\p32-animatronic-bot\config\components\creature_specific",
    [string]$DestDir = "F:\GitHub\p32-animatronic-bot\config\bots\bot_families",
    [string]$ReportFile = "F:\GitHub\p32-animatronic-bot\conflict_report.txt"
)

# Ensure the report file is empty
if (Test-Path $ReportFile) {
    Remove-Item $ReportFile
}

# Compare files
Get-ChildItem -Path $SourceDir -Recurse -File | ForEach-Object {
    $relativePath = $_.FullName.Substring($SourceDir.Length)
    $destFile = Join-Path -Path $DestDir -ChildPath $relativePath

    if (Test-Path $destFile) {
        $sourceContent = Get-Content -Path $_.FullName -Raw
        $destContent = Get-Content -Path $destFile -Raw

        if ($sourceContent -ne $destContent) {
            Add-Content -Path $ReportFile -Value "Conflict: $relativePath"
            Add-Content -Path $ReportFile -Value "Source: $_.FullName"
            Add-Content -Path $ReportFile -Value "Destination: $destFile"
            Add-Content -Path $ReportFile -Value "---"
        }
    }
}

Write-Host "Comparison complete. Report generated at $ReportFile"