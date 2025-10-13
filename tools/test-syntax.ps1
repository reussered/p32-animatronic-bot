# Test script to identify syntax error
param(
    [switch]$Test
)

function Test-Function {
    try {
        Write-Host "Testing"
    } catch {
        Write-Host "Error: $($_.Exception.Message)"
    }
}

if ($Test) {
    Test-Function
}