#!/usr/bin/env powershell

# Manual repair test for one file
$file = Get-Item "config\components\hardware\tft_2p5x1p5_display.json"
$json = Get-Content $file.FullName | ConvertFrom-Json

if ($json.PSObject.Properties.Name -contains 'relative_filename') {
	Write-Host "Before: relative_filename = '$($json.relative_filename)'"
} else {
	Write-Host "Before: no relative_filename present"
}

# Calculate correct path
$configPath = Resolve-Path "config"
$correctPath = $file.FullName.Replace($configPath.Path + [IO.Path]::DirectorySeparatorChar, "").Replace([IO.Path]::DirectorySeparatorChar, "/")
# Remove deprecated property
$json.PSObject.Properties.Remove('relative_filename') | Out-Null

Write-Host "After: relative_filename = '$($json.relative_filename)'"

# Write it back
$json | ConvertTo-Json -Depth 10 | Set-Content $file.FullName -Encoding ASCII

Write-Host "File updated successfully"