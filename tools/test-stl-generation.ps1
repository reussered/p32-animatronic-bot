# Simple STL Generator Test
param([switch]$Test)

Write-Host "STL Generation Test" -ForegroundColor Green

if ($Test) {
    # Create a simple SCAD file
    $scadContent = @'
// Simple cube
cube([10, 10, 10]);
'@
    
    $scadContent | Out-File -FilePath "test.scad" -Encoding UTF8
    Write-Host "Created test.scad file" -ForegroundColor Cyan
    
    # Try to find OpenSCAD
    $openscad = "C:\Program Files\OpenSCAD\openscad.exe"
    if (Test-Path $openscad) {
        Write-Host "Found OpenSCAD at: $openscad" -ForegroundColor Green
        
        # Create output directory
        if (-not (Test-Path "assets\shapes\stl")) {
            New-Item -ItemType Directory -Path "assets\shapes\stl" -Force | Out-Null
        }
        
        # Generate STL
        try {
            Write-Host "Generating STL..." -ForegroundColor Yellow
            $result = & $openscad -o "assets\shapes\stl\test.stl" "test.scad"
            if (Test-Path "assets\shapes\stl\test.stl") {
                Write-Host "SUCCESS: Generated test.stl" -ForegroundColor Green
            } else {
                Write-Host "FAILED: STL file not created" -ForegroundColor Red
            }
        } catch {
            Write-Host "ERROR: $($_.Exception.Message)" -ForegroundColor Red
        }
    } else {
        Write-Host "OpenSCAD not found at expected location" -ForegroundColor Red
    }
    
    # Clean up
    if (Test-Path "test.scad") {
        Remove-Item "test.scad"
    }
}

Write-Host "Test complete" -ForegroundColor Green