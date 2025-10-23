#!/usr/bin/env powershell

$file = Get-Item "config\components\hardware\tft_2p5x1p5_display.json"
$json = Get-Content $file.FullName | ConvertFrom-Json

Write-Host "File path: $($file.FullName)"
Write-Host "Current relative_filename: '$($json.relative_filename)'"

# Test the path calculation logic
$configPath = Resolve-Path "config"
Write-Host "Config path: $($configPath.Path)"

$actualPath = $file.FullName.Replace($configPath.Path + [IO.Path]::DirectorySeparatorChar, "").Replace([IO.Path]::DirectorySeparatorChar, "/")
Write-Host "Calculated path: '$actualPath'"

Write-Host "Are they equal? $($json.relative_filename -eq $actualPath)"                                                                                                                                     zzzzzzzz