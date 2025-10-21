#!/usr/bin/env pwsh
# P32 Animatronic Bot - Build Script
# Cleans and builds the project

Write-Host "`n=== P32 BUILD SCRIPT ===" -ForegroundColor Cyan
Write-Host "Cleaning previous build..." -ForegroundColor Yellow

# Clean build artifacts
pio run -t clean

if ($LASTEXITCODE -ne 0)
{
    Write-Host "`n Clean failed!" -ForegroundColor Red
    exit 1
}

Write-Host "`nBuilding project..." -ForegroundColor Yellow

# Build project
pio run

if ($LASTEXITCODE -ne 0)
{
    Write-Host "`n Build failed!" -ForegroundColor Red
    exit 1
}

Write-Host "`n Build successful!" -ForegroundColor Green
Write-Host "`nFirmware ready at: .pio\build\esp32-s3-devkitc-1\firmware.bin" -ForegroundColor Cyan
