#!/usr/bin/env powershell

# Manual repair test for one file
$file = Get-Item "config\components\hardware\tft_2p5x1p5_display.json"
$json = Get-Content $file.FullName | ConvertFrom-Json

Write-Host "Before: relative_filename = '$($json.relative_filename)'"

# Calculate correct path
$configPath = Resolve-Path "config"
$correctPath = $file.FullName.Replace($configPath.Path + [IO.Path]::DirectorySeparatorChar, "").Replace([IO.Path]::DirectorySeparatorChar, "/")
$json.relative_filename = $correctPath

Write-Host "After: relative_filename = '$($json.relative_filename)'"

# Write it back
$json | ConvertTo-Json -Depth 10 | Set-Content $file.FullName -Encoding UTF8

Write-Host "File updated successfully"