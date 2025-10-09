# -----------------------------------------------
# Script: Recursive File/Folder Scanner with Ignore List + JSON Content Display
# Purpose: Identify files with legal extensions and folders that are empty or contain such files
#          while ignoring specified folders and displaying JSON file contents
# Author: Edward J. Reusser (via Copilot)
# -----------------------------------------------

# Define the root directory as the location of this script
$root = Split-Path -Parent $MyInvocation.MyCommand.Definition

# -----------------------------------------------
# Step 1: Load legal extensions
# First argument = file with extensions (one per line)
# Default: .json, .md, .xml
# -----------------------------------------------
$defaultExtensions = @('.json', '.md', '.xml')
$extensions = if ($args.Count -ge 1 -and (Test-Path $args[0])) {
    Get-Content $args[0] | ForEach-Object { $_.Trim().ToLower() }
} else {
    $defaultExtensions
}

# Normalize extensions to start with dot
$extensions = $extensions | ForEach-Object {
    if ($_ -match '^\.') { $_ } else { ".$_" }
}

# -----------------------------------------------
# Step 2: Load ignore folder list
# Second argument = file with folder names to ignore
# Default: .pio
# -----------------------------------------------
$defaultIgnoreFolders = @('.pio')
$ignoreFolders = if ($args.Count -ge 2 -and (Test-Path $args[1])) {
    Get-Content $args[1] | ForEach-Object { $_.Trim().ToLower() }
} else {
    $defaultIgnoreFolders
}

# -----------------------------------------------
# Step 3: Initialize counters and logs
# -----------------------------------------------
$matchedFiles = @()
$matchedFolders = @()
$log = @()

# -----------------------------------------------
# Step 4: Folder Evaluation Function
# Determines if folder is empty or contains matching files
# Ignores folders listed in ignoreFolders
# -----------------------------------------------
function Test-Folder {
    param ($folderPath, $extensions)

    # Check if any segment of the path matches an ignored folder
    $pathSegments = $folderPath.ToLower() -split '[\\/]'
    if ($pathSegments | Where-Object { $ignoreFolders -contains $_ }) {
        return $false  # Skip ignored folders
    }

    # Get all items in folder
    $items = Get-ChildItem -Path $folderPath -ErrorAction SilentlyContinue

    # If no items, folder is empty
    if ($items.Count -eq 0) {
        return $true
    }

    # If it contains subfolders, it's not empty
    $hasSubfolder = $items | Where-Object { $_.PSIsContainer }
    if ($hasSubfolder.Count -gt 0) {
        return $false
    }

    # Check for matching files
    $hasMatchingFile = $items | Where-Object {
        -not $_.PSIsContainer -and ($extensions -contains $_.Extension.ToLower())
    }

    return $hasMatchingFile.Count -gt 0
}

# -----------------------------------------------
# Step 5: Recursive Scan
# Walk through all files and folders under $root
# Output matching files and folders to stdout
# Log everything for optional review
# -----------------------------------------------
Get-ChildItem -Path $root -Recurse -ErrorAction SilentlyContinue | ForEach-Object {
    try {
        # Check if any segment of the path matches an ignored folder
        $pathSegments = $_.FullName.ToLower() -split '[\\/]'
        if ($pathSegments | Where-Object { $ignoreFolders -contains $_ }) {
            $log += "Skipped ignored folder: $($_.FullName)"
            return
        }

        # Folder match logic
        if ($_.PSIsContainer) {
            if (Test-Folder $_.FullName $extensions) {
                $matchedFolders += $_.FullName
                Write-Output $_.FullName
                $log += "Folder match: $($_.FullName)"
            }
        }
        # File match logic
        elseif ($extensions -contains $_.Extension.ToLower()) {
            $matchedFiles += $_.FullName
            Write-Output $_.FullName
            $log += "File match: $($_.FullName)"

            # If it's a JSON file, display its contents
            if ($_.Extension.ToLower() -eq '.json') {
                try {
                    $jsonContent = Get-Content $_.FullName -Raw | ConvertFrom-Json
                    $formatted = $jsonContent | ConvertTo-Json -Depth 10
                    Write-Output "`nContents of $($_.Name):"
                    Write-Output "{"
                    Write-Output $formatted
                    Write-Output "}`n"
                } catch {
                    Write-Warning "Failed to parse JSON: $($_.FullName)"
                    $log += "Failed to parse JSON: $($_.FullName)"
                }
            }
        }
    } catch {
        # Log errors to stderr and internal log
        Write-Error "Error processing $_.FullName: $_"
        $log += "Error: $_"
    }
}

# -----------------------------------------------
# Step 6: Summary Report
# -----------------------------------------------
Write-Output "`n--- Summary ---"
Write-Output "Matching files: $($matchedFiles.Count)"
Write-Output "Matching folders: $($matchedFolders.Count)"
Write-Output "Ignored folders: $($ignoreFolders.Count)"

# -----------------------------------------------
# Optional: Save log to file
# Uncomment the line below to enable logging
# -----------------------------------------------
# $log | Set-Content "$root\scan_log.txt"
