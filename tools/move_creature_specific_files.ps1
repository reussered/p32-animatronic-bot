# Move creature-specific files to their proper locations in the bot_families tree structure

# Define the source and destination directories
$sourceDir = "F:\GitHub\p32-animatronic-bot\config\components\creature_specific"
$destinationDir = "F:\GitHub\p32-animatronic-bot\config\bots\bot_families"

# Define a mapping of creature types to their respective folders
$creatureMapping = @{
    "goblin" = "goblins\head"
    "cat" = "cats\head"
    "bear" = "bears\head"
    "cyclops" = "cyclops\head"
    "dragon" = "dragons\head"
    "elf" = "elves\head"
    "vampire" = "vampires\head"
    "wolf" = "wolves\head"
    "zombie" = "zombies\head"
}

# Function to move files based on their prefix
function Move-Files {
    param (
        [string]$prefix,
        [string]$destination
    )

    $files = Get-ChildItem -Path $sourceDir -Filter "$prefix*.*"
    foreach ($file in $files) {
        $targetPath = Join-Path -Path $destinationDir -ChildPath $destination
        if (-Not (Test-Path -Path $targetPath)) {
            New-Item -ItemType Directory -Path $targetPath
        }
        Move-Item -Path $file.FullName -Destination $targetPath
    }
}

# Iterate over the mapping and move files
foreach ($creature in $creatureMapping.Keys) {
    Move-Files -prefix $creature -destination $creatureMapping[$creature]
}

Write-Host "All files have been moved to their proper locations." -ForegroundColor Green