# Complete build sequence for P32 Animatronic Bot
# Usage: .\dobuild.ps1 <subsystem_name>
# Example: .\dobuild.ps1 goblin_head

param(
    [Parameter(Mandatory=$true)]
    [string]$Subsystem
)

$ErrorActionPreference = "Stop"

Write-Host ""
Write-Host "=== P32 Complete Build Sequence ===" -ForegroundColor Cyan
Write-Host "Subsystem: $Subsystem" -ForegroundColor Cyan
Write-Host ""

# Step 1: Generate tables
Write-Host "[1/4] Generating dispatch tables..." -ForegroundColor Yellow
python tools\generate_tables.py $Subsystem src
if ($LASTEXITCODE -ne 0) {
    Write-Host "Table generation failed" -ForegroundColor Red
    exit 1
}
Write-Host "Table generation complete" -ForegroundColor Green
Write-Host ""

# Step 2: Clean build directory
Write-Host "[2/4] Cleaning build directory..." -ForegroundColor Yellow
pio run -e $Subsystem -t clean 2>&1 | Out-Null
if ($LASTEXITCODE -ne 0) {
    Write-Host "Clean failed" -ForegroundColor Red
    exit 1
}
Write-Host "Clean complete" -ForegroundColor Green
Write-Host ""

# Step 3: Compile firmware
Write-Host "[3/4] Compiling firmware..." -ForegroundColor Yellow
$buildOutput = & { pio run -e $Subsystem 2>&1 }
$buildOutput | Out-File "build_$Subsystem.log" -Encoding ascii

# Check for errors
$errors = $buildOutput | Select-String "error:|undefined|multiple definition"
if ($errors) {
    Write-Host "Compilation failed with errors:" -ForegroundColor Red
    Write-Host ""
    $errors | Select-Object -First 10 | ForEach-Object { Write-Host $_.Line -ForegroundColor Red }
    Write-Host ""
    Write-Host "Full log: build_$Subsystem.log" -ForegroundColor Yellow
    exit 1
}

# Check for success
$success = $buildOutput | Select-String "SUCCESS"
if (-not $success) {
    Write-Host "Compilation failed (no SUCCESS message)" -ForegroundColor Red
    $buildOutput | Select-Object -Last 20 | ForEach-Object { Write-Host $_ }
    exit 1
}

# Show memory usage
$ramUsage = $buildOutput | Select-String "RAM:"
$flashUsage = $buildOutput | Select-String "Flash:"
Write-Host "Compilation successful" -ForegroundColor Green
Write-Host ""
if ($ramUsage) { Write-Host $ramUsage.Line -ForegroundColor Cyan }
if ($flashUsage) { Write-Host $flashUsage.Line -ForegroundColor Cyan }
Write-Host ""

# Step 4: Upload firmware
Write-Host "[4/4] Uploading firmware..." -ForegroundColor Yellow
$uploadOutput = & { pio run -e $Subsystem -t upload 2>&1 }
$uploadOutput | Out-File "upload_$Subsystem.log" -Encoding ascii

# Check for upload success
$uploadSuccess = $uploadOutput | Select-String "Writing at|Hard resetting"
if ($uploadSuccess) {
    Write-Host "Upload successful" -ForegroundColor Green
    Write-Host ""
    $uploadSuccess | Select-Object -Last 5 | ForEach-Object { Write-Host $_.Line -ForegroundColor Cyan }
}
else {
    Write-Host "Upload may have failed" -ForegroundColor Red
    $uploadOutput | Select-Object -Last 10 | ForEach-Object { Write-Host $_ }
    Write-Host ""
    Write-Host "Full log: upload_$Subsystem.log" -ForegroundColor Yellow
    exit 1
}

Write-Host ""
Write-Host "=== Build Complete ===" -ForegroundColor Green
Write-Host "Logs: build_$Subsystem.log, upload_$Subsystem.log" -ForegroundColor Cyan
