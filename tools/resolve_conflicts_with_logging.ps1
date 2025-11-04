# This script resolves file conflicts between the source and destination directories.
# If files are duplicates, it deletes the source file. If they differ, it keeps the most recently modified file.
# Logs all actions to a report file.

param (
    [string]$SourceDir = "F:\GitHub\p32-animatronic-bot\config\components\creature_specific",
    [string]$DestDir = "F:\GitHub\p32-animatronic-bot\config\bots\bot_families",
    [string]$LogFile = "F:\GitHub\p32-animatronic-bot\conflict_resolution_log.txt"
)

# Ensure the log file is empty
if (Test-Path $LogFile) {
    Remove-Item $LogFile
}

# Resolve conflicts
Get-ChildItem -Path $SourceDir -Recurse -File | ForEach-Object {
    $relativePath = $_.FullName.Substring($SourceDir.Length)

    # Extract bot family from the file name (e.g., bear_eye -> bears)
    $botFamily = $relativePath -replace "^.*?_(.*?)_.*$", "$1" | ForEach-Object { $_.ToLower() }
    $botFamilyDir = Join-Path -Path $DestDir -ChildPath $botFamily

    # Ensure bot family directory exists
    if (-not (Test-Path $botFamilyDir)) {
        New-Item -ItemType Directory -Path $botFamilyDir
    }

    $destFile = Join-Path -Path $botFamilyDir -ChildPath $relativePath

    if (Test-Path $destFile) {
        $sourceContent = Get-Content -Path $_.FullName -Raw
        $destContent = Get-Content -Path $destFile -Raw

        if ($sourceContent -eq $destContent) {
            # Files are duplicates, delete the source file
            Remove-Item -Path $_.FullName
            Add-Content -Path $LogFile -Value "Deleted duplicate: $_.FullName"
        } else {
            # Files differ, keep the most recently modified file
            $sourceLastWrite = (Get-Item $_.FullName).LastWriteTime
            $destLastWrite = (Get-Item $destFile).LastWriteTime

            if ($sourceLastWrite -gt $destLastWrite) {
                # Source is newer, overwrite destination
                Copy-Item -Path $_.FullName -Destination $destFile -Force
                Remove-Item -Path $_.FullName
                Add-Content -Path $LogFile -Value "Replaced with newer file: $destFile"
            } else {
                # Destination is newer, delete the source
                Remove-Item -Path $_.FullName
                Add-Content -Path $LogFile -Value "Deleted older source file: $_.FullName"
            }
        }
    } else {
        # Destination does not exist, move the file
        Move-Item -Path $_.FullName -Destination $destFile
        Add-Content -Path $LogFile -Value "Moved file: $_.FullName to $destFile"
    }
}

Write-Host "Conflict resolution complete. Log generated at $LogFile"