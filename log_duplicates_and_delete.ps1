# PowerShell script to identify duplicate JSON files, delete them, and log the results

# Define paths
$checksumFile = "f:\GitHub\p32-animatronic-bot\json_files_with_checksums.txt"
$logFile = "f:\GitHub\p32-animatronic-bot\duplicates_deleted.log"

# Clear previous log
if (Test-Path $logFile) {
    Remove-Item -Path $logFile -Force
}

# Log header
"Duplicate JSON Files Log - $(Get-Date)" | Out-File -FilePath $logFile -Encoding ASCII

# Process the checksum file
Get-Content -Path $checksumFile |
ForEach-Object {
    # Split the line into file path and checksum
    $lineParts = $_ -split ' '
    [PSCustomObject]@{
        FilePath = $lineParts[0]
        Checksum = $lineParts[1]
    }
} |
Group-Object -Property Checksum |
Where-Object { $_.Count -gt 1 } |
ForEach-Object {
    # Log duplicates
    "Duplicates for checksum: $($_.Name)" | Out-File -Append -FilePath $logFile -Encoding ASCII
    $_.Group | ForEach-Object { $_.FilePath | Out-File -Append -FilePath $logFile -Encoding ASCII }

    # Delete duplicates (keep the first file)
    $_.Group | Select-Object -Skip 1 | ForEach-Object {
        "Deleting: $($_.FilePath)" | Out-File -Append -FilePath $logFile -Encoding ASCII
        Remove-Item -Path $_.FilePath -Force
    }
}

"Task completed successfully." | Out-File -Append -FilePath $logFile -Encoding ASCII