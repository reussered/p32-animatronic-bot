# OpenSCAD Utility Functions for P32 Tools
# Always use executable names from PATH to ensure permanent environment variables work

function Test-OpenSCADAvailable {
    <#
    .SYNOPSIS
    Tests if OpenSCAD is available in PATH
    .DESCRIPTION
    Uses Get-Command to check if openscad.exe is accessible without hardcoded paths
    .OUTPUTS
    Boolean - True if OpenSCAD is available, False otherwise
    #>
    try {
        $null = Get-Command "openscad.exe" -ErrorAction Stop
        return $true
    }
    catch {
        return $false
    }
}

function Invoke-OpenSCAD {
    <#
    .SYNOPSIS
    Executes OpenSCAD using PATH environment variable
    .PARAMETER InputFile
    Path to input .scad file
    .PARAMETER OutputFile  
    Path to output .stl/.3mf file
    .PARAMETER AdditionalArgs
    Additional OpenSCAD command line arguments
    .OUTPUTS
    Process exit code
    #>
    param(
        [Parameter(Mandatory)]
        [string]$InputFile,
        
        [Parameter(Mandatory)]
        [string]$OutputFile,
        
        [string[]]$AdditionalArgs = @()
    )
    
    if (-not (Test-OpenSCADAvailable)) {
        Write-Host "ERROR: openscad.exe not found in PATH" -ForegroundColor Red
        Write-Host "Please ensure OpenSCAD is installed and added to PATH environment variable" -ForegroundColor Yellow
        Write-Host "To add to PATH permanently:" -ForegroundColor Cyan
        Write-Host '  1. Add "C:\Program Files\OpenSCAD" to your PATH environment variable' -ForegroundColor White
        Write-Host "  2. Restart your terminal/VS Code" -ForegroundColor White
        Write-Host "  3. Test with: openscad.exe --version" -ForegroundColor White
        return 1
    }
    
    $arguments = @("-o", $OutputFile, $InputFile) + $AdditionalArgs
    
    Write-Host "Generating: $OutputFile" -ForegroundColor Green
    Write-Host "Command: openscad.exe $($arguments -join ' ')" -ForegroundColor Gray
    
    try {
        $process = Start-Process -FilePath "openscad.exe" -ArgumentList $arguments -Wait -PassThru -NoNewWindow
        return $process.ExitCode
    }
    catch {
        Write-Host "ERROR running OpenSCAD: $($_.Exception.Message)" -ForegroundColor Red
        return 1
    }
}

function Test-PermanentPATH {
    <#
    .SYNOPSIS
    Tests if tools are available via permanent PATH (not session modifications)
    .DESCRIPTION
    Warns user if PATH appears to be temporarily modified vs permanently set
    #>
    if (Test-OpenSCADAvailable) {
        Write-Host "✅ openscad.exe found in PATH" -ForegroundColor Green
        Write-Host "Note: This could be from session modification or permanent PATH" -ForegroundColor Yellow
        Write-Host "To verify permanent PATH: restart terminal and test again" -ForegroundColor Cyan
    } else {
        Write-Host "❌ openscad.exe not found in PATH" -ForegroundColor Red
        Write-Host "Add to permanent PATH or use session modification" -ForegroundColor Yellow
    }
}

# Export functions for use in other scripts
Export-ModuleMember -Function Test-OpenSCADAvailable, Invoke-OpenSCAD, Test-PermanentPATH