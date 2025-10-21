# Canon Printer Keep-Alive Script
# Keeps LBP632C awake by pinging it every 90 seconds

param(
    [string]$PrinterName = "LBP632C",
    [int]$IntervalSeconds = 90
)

Write-Host " Canon Printer Keep-Alive Started" -ForegroundColor Green
Write-Host "Printer: $PrinterName" -ForegroundColor Cyan
Write-Host "Ping interval: $IntervalSeconds seconds" -ForegroundColor Cyan
Write-Host "Press Ctrl+C to stop" -ForegroundColor Yellow
Write-Host ""

$counter = 0

while ($true) {
    try {
        $counter++
        $timestamp = Get-Date -Format "HH:mm:ss"
        
        # Try to get printer status
        $printer = Get-Printer -Name $PrinterName -ErrorAction Stop
        
        if ($printer.PrinterStatus -eq "Normal") {
            Write-Host "[$timestamp]  Printer online (ping #$counter)" -ForegroundColor Green
        } else {
            Write-Host "[$timestamp]  Printer status: $($printer.PrinterStatus) (ping #$counter)" -ForegroundColor Yellow
        }
        
        # Send a small print job to keep it awake (just get print queue)
        Get-PrintJob -PrinterName $PrinterName -ErrorAction SilentlyContinue | Out-Null
        
    } catch {
        Write-Host "[$timestamp]  Printer offline or error: $($_.Exception.Message)" -ForegroundColor Red
    }
    
    # Wait for next ping
    Start-Sleep $IntervalSeconds
}