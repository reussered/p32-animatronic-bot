# Script to move .cpp files that have corresponding .src files to a zip archive
# This removes duplicate/fragment .cpp files, keeping only .src versions

$componentsDir = "src/components"
$zipPath = "cpp_fragments_backup.zip"
$filesToZip = @()

# Get all .src files
$srcFiles = Get-ChildItem -Path $componentsDir -Filter "*.src"

foreach ($srcFile in $srcFiles) {
    $baseName = $srcFile.BaseName
    $cppFile = Join-Path $componentsDir "$baseName.cpp"
    
    if (Test-Path $cppFile) {
        Write-Host "Found corresponding .cpp for $srcFile : $cppFile"
        $filesToZip += $cppFile
    } else {
        Write-Host "No corresponding .cpp file for $srcFile"
    }
}

if ($filesToZip.Count -gt 0) {
    Write-Host "Moving $($filesToZip.Count) .cpp files to $zipPath"
    Compress-Archive -Path $filesToZip -DestinationPath $zipPath -Force
    # Delete the original files after zipping
    foreach ($file in $filesToZip) {
        Remove-Item $file -Force
    }
    Write-Host "Moved and zipped successfully."
} else {
    Write-Host "No .cpp files to move."
}