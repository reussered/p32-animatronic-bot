# CRITICAL DIRECTORY MONITORING SYSTEM
# Detects and prevents dangerous repository nesting/duplication
# Prevents the "nested repository" problem that caused work loss

$ErrorActionPreference = "Stop"

Write-Host "DIRECTORY MONITORING SYSTEM" -ForegroundColor Cyan -BackgroundColor Black
Write-Host "Scanning for dangerous repository patterns..." -ForegroundColor Yellow

# Check for multiple project instances
$INSTANCES = @()

# Check common locations where duplicates might appear
$SEARCH_PATHS = @(
    "F:\",
    "F:\GitHub\",
    "C:\",
    "C:\GitHub\",
    "D:\",
    "D:\GitHub\"
)

Write-Host "Scanning for p32-animatronic-bot instances..." -ForegroundColor Cyan

foreach ($path in $SEARCH_PATHS) {
    if (Test-Path $path) {
        try {
            $found = Get-ChildItem -Path $path -Directory -ErrorAction SilentlyContinue | Where-Object { 
                $_.Name -like "*p32*animatronic*" -or $_.Name -eq "p32-animatronic-bot" 
            }
            
            foreach ($instance in $found) {
                $fullPath = $instance.FullName
                $fileCount = 0
                $size = 0
                
                if (Test-Path "$fullPath\src") {
                    try {
                        $files = Get-ChildItem -Path "$fullPath\src" -Recurse -File -ErrorAction SilentlyContinue
                        $fileCount = ($files | Measure-Object).Count
                        $size = ($files | Measure-Object -Property Length -Sum).Sum
                    } catch {
                        $fileCount = 0
                        $size = 0
                    }
                }
                
                $INSTANCES += @{
                    Path = $fullPath
                    LastModified = $instance.LastWriteTime
                    Size = $size
                    FileCount = $fileCount
                }
                Write-Host "Found: $fullPath" -ForegroundColor Yellow
            }
        } catch {
            # Silently continue if path is inaccessible
        }
    }
}

Write-Host ""
Write-Host "INSTANCE ANALYSIS:" -ForegroundColor Green

if ($INSTANCES.Count -eq 0) {
    Write-Host "No instances found - this might indicate a problem!" -ForegroundColor Red
} elseif ($INSTANCES.Count -eq 1) {
    Write-Host "Single instance found - GOOD!" -ForegroundColor Green
    $instance = $INSTANCES[0]
    Write-Host "   Path: $($instance.Path)" -ForegroundColor Cyan
    Write-Host "   Files: $($instance.FileCount)" -ForegroundColor Cyan
    Write-Host "   Last Modified: $($instance.LastModified)" -ForegroundColor Cyan
} else {
    Write-Host "MULTIPLE INSTANCES DETECTED - DANGER!" -ForegroundColor Red -BackgroundColor Yellow
    Write-Host "This is how work gets lost!" -ForegroundColor Red
    
    foreach ($i in 0..($INSTANCES.Count-1)) {
        $instance = $INSTANCES[$i]
        Write-Host ""
        Write-Host "Instance $($i+1):" -ForegroundColor Yellow
        Write-Host "   Path: $($instance.Path)" -ForegroundColor White
        Write-Host "   Files: $($instance.FileCount)" -ForegroundColor White
        Write-Host "   Size: $([math]::Round($instance.Size/1MB, 2)) MB" -ForegroundColor White
        Write-Host "   Last Modified: $($instance.LastModified)" -ForegroundColor White
    }
    
    # Find the most recent/largest instance
    $primary = $INSTANCES | Sort-Object { $_.FileCount, $_.LastModified } -Descending | Select-Object -First 1
    Write-Host ""
    Write-Host "RECOMMENDED PRIMARY INSTANCE:" -ForegroundColor Green
    Write-Host "   $($primary.Path)" -ForegroundColor Green
    Write-Host "   (Most files: $($primary.FileCount), Latest: $($primary.LastModified))" -ForegroundColor Green
    
    Write-Host ""
    Write-Host "RECOMMENDED ACTIONS:" -ForegroundColor Yellow
    Write-Host "1. Run EMERGENCY_BACKUP_SYSTEM.ps1 to backup ALL instances" -ForegroundColor White
    Write-Host "2. Compare instances to ensure no unique work exists in others" -ForegroundColor White
    Write-Host "3. Consolidate to the primary instance" -ForegroundColor White
    Write-Host "4. Remove duplicate instances after backup verification" -ForegroundColor White
}

# Check for nested repositories (the specific problem we encountered)
Write-Host ""
Write-Host "Checking for nested repositories..." -ForegroundColor Cyan

foreach ($instance in $INSTANCES) {
    $path = $instance.Path
    if (Test-Path $path) {
        $nested = Get-ChildItem -Path $path -Directory -ErrorAction SilentlyContinue | Where-Object { 
            $_.Name -like "*p32*" -or $_.Name -eq "p32-animatronic-bot" 
        }
        
        if ($nested) {
            Write-Host "NESTED REPOSITORY DETECTED!" -ForegroundColor Red -BackgroundColor Yellow
            Write-Host "   Parent: $path" -ForegroundColor Red
            foreach ($nest in $nested) {
                Write-Host "   Nested: $($nest.FullName)" -ForegroundColor Red
            }
            Write-Host "   THIS CAUSES WORK LOSS - IMMEDIATE ACTION REQUIRED!" -ForegroundColor Red
        }
    }
}

Write-Host ""
Write-Host "MONITORING COMPLETE" -ForegroundColor Green -BackgroundColor Black

# Save monitoring report
$timestamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"
$reportPath = "F:\DIRECTORY_MONITORING_REPORT_$timestamp.txt"

$report = @"
DIRECTORY MONITORING REPORT
Generated: $(Get-Date)

INSTANCES FOUND: $($INSTANCES.Count)

"@

foreach ($i in 0..($INSTANCES.Count-1)) {
    $instance = $INSTANCES[$i]
    $report += @"
Instance $($i+1):
  Path: $($instance.Path)
  Files: $($instance.FileCount)
  Size: $([math]::Round($instance.Size/1MB, 2)) MB
  Last Modified: $($instance.LastModified)

"@
}

$report | Out-File -FilePath $reportPath -Encoding UTF8
Write-Host "Report saved: $reportPath" -ForegroundColor Green