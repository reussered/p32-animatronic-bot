# P32 Development Cycle Script
# Combines clean, generate, build, upload, and monitor in one command

param(
    [string]$Environment = "test_head",
    [string]$Config = "config/subsystems/test_head.json",
    [switch]$NoMonitor
)

Write-Host "Starting P32 Development Cycle: $Environment" -ForegroundColor Green
Write-Host "Config: $Config" -ForegroundColor Yellow

# Step 1: Clean
Write-Host ""
Write-Host "Step 1: Cleaning..." -ForegroundColor Cyan
pio run -e $Environment -t clean

# Step 2: Generate tables
Write-Host ""
Write-Host "Step 2: Generating component tables..." -ForegroundColor Cyan
python tools/generate_tables.py $Config src

# Step 3: Build and upload
Write-Host ""
Write-Host "Step 3: Building and uploading..." -ForegroundColor Cyan
pio run -e $Environment -t upload

# Step 4: Monitor (optional)
if (-not $NoMonitor) {
    Write-Host ""
    Write-Host "Step 4: Starting serial monitor..." -ForegroundColor Cyan
    Write-Host "Press Ctrl+C to stop monitoring" -ForegroundColor Yellow
    pio device monitor
}
else {
    Write-Host ""
    Write-Host "Development cycle complete! (Monitor skipped)" -ForegroundColor Green
}