# P32 File Checkout System
# Safely check out files for modification with automatic backup

param(
    [Parameter(Mandatory=$false)]
    [string[]]$Files,
    
    [Parameter(Mandatory=$false)]
    [switch]$AllProject,
    
    [Parameter(Mandatory=$false)]
    [switch]$ShowStatus,
    
    [Parameter(Mandatory=$false)]
    [switch]$Restore,
    
    [Parameter(Mandatory=$false)]
    [switch]$Commit
)

$WorkingFilesPath = ".working-files.txt"
$BackupDir = "temp-backup-$(Get-Date -Format 'yyyy-MM-dd-HHmm')"

function Show-Status {
    if (Test-Path $WorkingFilesPath) {
        Write-Host "🔧 Currently checked out files:" -ForegroundColor Yellow
        Get-Content $WorkingFilesPath | ForEach-Object { Write-Host "  - $_" }
        
        if (Test-Path "temp-backup-*") {
            Write-Host "`n💾 Backup directories:" -ForegroundColor Green
            Get-ChildItem "temp-backup-*" -Directory | ForEach-Object { Write-Host "  - $($_.Name)" }
        }
    } else {
        Write-Host "✅ No files currently checked out" -ForegroundColor Green
    }
}

function Start-Checkout {
    param([string[]]$FilesToCheckout)
    
    if (Test-Path $WorkingFilesPath) {
        Write-Host "⚠️  Files already checked out! Use -ShowStatus to see them" -ForegroundColor Red
        return
    }
    
    # Create working files list
    $FilesToCheckout | Out-File $WorkingFilesPath -Encoding UTF8
    
    Write-Host "📋 Created working file list with $($FilesToCheckout.Count) files" -ForegroundColor Green
    
    # Create backup directory
    New-Item -ItemType Directory -Path $BackupDir -Force | Out-Null
    Write-Host "📁 Created backup directory: $BackupDir" -ForegroundColor Green
    
    # Backup each file
    foreach ($file in $FilesToCheckout) {
        if (Test-Path $file) {
            $backupPath = Join-Path $BackupDir $file
            $backupDir = Split-Path $backupPath -Parent
            
            # Create directory structure in backup
            if (-not (Test-Path $backupDir)) {
                New-Item -ItemType Directory -Path $backupDir -Force | Out-Null
            }
            
            # Copy file to backup
            Copy-Item $file $backupPath -Force
            Write-Host "💾 Backed up: $file" -ForegroundColor Blue
        } else {
            Write-Host "⚠️  File not found: $file" -ForegroundColor Yellow
        }
    }
    
    Write-Host "`n✅ Files are now checked out and backed up!" -ForegroundColor Green
    Write-Host "🔧 You can now safely modify the checked-out files" -ForegroundColor Cyan
}

function Start-AllProjectCheckout {
    Write-Host "🚀 Checking out ENTIRE project..." -ForegroundColor Yellow
    
    $allFiles = Get-ChildItem -Recurse -File | Where-Object { 
        $_.Extension -match "\.(cpp|hpp|json|py|ps1|md|scad|stl)$" 
    } | ForEach-Object { 
        $_.FullName.Replace((Get-Location).Path + "\", "").Replace("\", "/")
    }
    
    Write-Host "📊 Found $($allFiles.Count) files to checkout" -ForegroundColor Cyan
    Start-Checkout -FilesToCheckout $allFiles
}

function Restore-Files {
    if (-not (Test-Path $WorkingFilesPath)) {
        Write-Host "❌ No files checked out to restore" -ForegroundColor Red
        return
    }
    
    $backupDirs = Get-ChildItem "temp-backup-*" -Directory | Sort-Object Name -Descending
    if ($backupDirs.Count -eq 0) {
        Write-Host "❌ No backup directories found" -ForegroundColor Red
        return
    }
    
    $latestBackup = $backupDirs[0].Name
    Write-Host "🔄 Restoring from: $latestBackup" -ForegroundColor Yellow
    
    Get-Content $WorkingFilesPath | ForEach-Object {
        $backupFile = Join-Path $latestBackup $_
        if (Test-Path $backupFile) {
            Copy-Item $backupFile $_ -Force
            Write-Host "✅ Restored: $_" -ForegroundColor Green
        } else {
            Write-Host "⚠️  Backup not found: $backupFile" -ForegroundColor Yellow
        }
    }
    
    Write-Host "`n🎉 Restore complete!" -ForegroundColor Green
}

function Commit-Changes {
    if (-not (Test-Path $WorkingFilesPath)) {
        Write-Host "❌ No files checked out to commit" -ForegroundColor Red
        return
    }
    
    Write-Host "🧪 Running validation tests..." -ForegroundColor Yellow
    
    # Run naming convention checks
    $jsonCheck = python tools/check_json_naming.py
    $cppCheck = python tools/check_cpp_naming.py
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✅ Naming conventions validated" -ForegroundColor Green
    } else {
        Write-Host "❌ Naming convention errors found. Fix before committing!" -ForegroundColor Red
        return
    }
    
    # Stage the checked-out files
    $files = Get-Content $WorkingFilesPath
    foreach ($file in $files) {
        git add $file
        Write-Host "📤 Staged: $file" -ForegroundColor Blue
    }
    
    # Commit
    $commitMessage = "Verified changes to checked-out files: $(Get-Date -Format 'yyyy-MM-dd HH:mm')"
    git commit -m $commitMessage
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✅ Changes committed successfully!" -ForegroundColor Green
        
        # Clean up
        Remove-Item $WorkingFilesPath -Force
        Get-ChildItem "temp-backup-*" -Directory | Remove-Item -Recurse -Force
        Write-Host "🧹 Cleaned up working files and backups" -ForegroundColor Cyan
    } else {
        Write-Host "❌ Commit failed!" -ForegroundColor Red
    }
}

# Main script logic
if ($ShowStatus) {
    Show-Status
} elseif ($Restore) {
    Restore-Files
} elseif ($Commit) {
    Commit-Changes
} elseif ($AllProject) {
    Start-AllProjectCheckout
} elseif ($Files) {
    Start-Checkout -FilesToCheckout $Files
} else {
    Write-Host @"
🔒 P32 File Checkout System

USAGE:
  .\tools\checkout-files.ps1 -Files "file1.cpp","file2.json"  # Check out specific files
  .\tools\checkout-files.ps1 -AllProject                      # Check out entire project
  .\tools\checkout-files.ps1 -ShowStatus                      # Show current status
  .\tools\checkout-files.ps1 -Restore                         # Restore from backup
  .\tools\checkout-files.ps1 -Commit                          # Commit and cleanup

EXAMPLES:
  # Check out a component for modification
  .\tools\checkout-files.ps1 -Files "src/components/SystemCore.cpp","include/components/SystemCore.hpp"
  
  # Check out JSON config
  .\tools\checkout-files.ps1 -Files "config/components/system/system_core.json"
  
  # Work on documentation  
  .\tools\checkout-files.ps1 -Files "docs/COMPONENT-FUNCTION-SIGNATURES.md"

WORKFLOW:
  1. Checkout files (creates backup automatically)
  2. Modify checked-out files safely
  3. Test changes thoroughly
  4. Commit (validates and cleans up)
  
  OR if something breaks:
  
  1. Restore from backup immediately
"@ -ForegroundColor Cyan
}