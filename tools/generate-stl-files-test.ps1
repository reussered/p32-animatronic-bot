# Simple test version of STL generator
param(
    [switch]$BasicMounts
)

Write-Host "Simple STL Generator Test" -ForegroundColor Green

if ($BasicMounts) {
    Write-Host "Basic mounts requested" -ForegroundColor Yellow
}

Write-Host "Test complete" -ForegroundColor Cyan