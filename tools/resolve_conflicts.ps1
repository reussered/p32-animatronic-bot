# This script resolves file conflicts between the source and destination directories.
# If files are duplicates, it deletes the source file. If they differ, it keeps the most recently modified file.

param (
    [string]$SourceDir = "F:\GitHub\p32-animatronic-bot\config\components\creature_specific",
    [string]$DestDir = "F:\GitHub\p32-animatronic-bot\config\bots\bot_families"
)

# Resolve conflicts
Get-ChildItem -Path $SourceDir -Recurse -File | ForEach-Object {
    $relativePath = $_.FullName.Substring($SourceDir.Length)
    $destFile = Join-Path -Path $DestDir -ChildPath $relativePath

    if (Test-Path $destFile) {
        $sourceContent = Get-Content -Path $_.FullName -Raw
        $destContent = Get-Content -Path $destFile -Raw

        if ($sourceContent -eq $destContent) {
            # Files are duplicates, delete the source file
            Remove-Item -Path $_.FullName
            Write-Host "Deleted duplicate: $_.FullName"
        } else {
            # Files differ, keep the most recently modified file
            $sourceLastWrite = (Get-Item $_.FullName).LastWriteTime
            $destLastWrite = (Get-Item $destFile).LastWriteTime

            if ($sourceLastWrite -gt $destLastWrite) {
                # Source is newer, overwrite destination
                Copy-Item -Path $_.FullName -Destination $destFile -Force
                Remove-Item -Path $_.FullName
                Write-Host "Replaced with newer file: $destFile"
            } else {
                # Destination is newer, delete the source
                Remove-Item -Path $_.FullName
                Write-Host "Deleted older source file: $_.FullName"
            }
        }
    }
}