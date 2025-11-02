# Pin Assignment Analyzer for ESP32 Components
# Simulates runtime pin allocation to show what pins will be assigned

param(
    [string]$SubsystemPath = "src/subsystems/test_head/test_head_component_functions.cpp",
    [string]$PinHeaderPath = "include/esp32_s3_r8n16_pin_assignments.h"
)

Write-Host "=== ESP32 Pin Assignment Analyzer ===" -ForegroundColor Cyan
Write-Host ""

# Define available pins from ESP32-S3-R8N16
$spi_assignable = @(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48)
$assigned_pins = @()

function Claim-NextPin {
    foreach ($pin in $spi_assignable) {
        if ($assigned_pins -notcontains $pin) {
            $script:assigned_pins += $pin
            return $pin
        }
    }
    return -1
}

# Read the component functions file to count how many displays/devices need pins
if (Test-Path $SubsystemPath) {
    $content = Get-Content $SubsystemPath -Raw
    
    # Count SPI display bus and SPI data bus init calls from dispatch table
    # spi_display_bus_init - for write-only displays (GC9A01, ST7789, etc.) - NO MISO
    # spi_data_bus_init - for sensors needing bidirectional comms - WITH MISO
    $displayBusCount = 0
    $dataBusCount = 0
    $dispatchPath = "src/subsystems/test_head/test_head_dispatch_tables.cpp"
    if (Test-Path $dispatchPath) {
        $dispatchContent = Get-Content $dispatchPath -Raw
        $displayMatches = [regex]::Matches($dispatchContent, 'spi_display_bus_init,')
        $displayBusCount = $displayMatches.Count
        $dataMatches = [regex]::Matches($dispatchContent, 'spi_data_bus_init,')
        $dataBusCount = $dataMatches.Count
        Write-Host "Reading dispatch table: $dispatchPath" -ForegroundColor DarkGray
    } else {
        Write-Host "Warning: Could not find dispatch table, results may be inaccurate" -ForegroundColor Yellow
    }
    
    # For this script, we focus on display buses
    $displayCount = $displayBusCount
    
    Write-Host "Found Components:" -ForegroundColor Yellow
    Write-Host "  - SPI Display Bus init calls: $displayBusCount (write-only displays)" -ForegroundColor White
    if ($dataBusCount -gt 0) {
        Write-Host "  - SPI Data Bus init calls: $dataBusCount (bidirectional sensors)" -ForegroundColor White
    }
    Write-Host ""
    
    Write-Host "Pin Assignment Simulation:" -ForegroundColor Yellow
    Write-Host "──────────────────────────────" -ForegroundColor DarkGray
    
    # First device initializes the shared SPI bus
    Write-Host "`nInitializing shared SPI Display Bus (first device)..." -ForegroundColor Green
    Write-Host "  (spi_display_bus is for write-only displays - NO MISO pin allocated)" -ForegroundColor DarkYellow
    $spi_mosi = Claim-NextPin
    $spi_clk = Claim-NextPin
    $spi_reset = Claim-NextPin
    
    Write-Host "  Shared Pins:" -ForegroundColor Cyan
    Write-Host "    MOSI (Master Out Slave In): GPIO $spi_mosi" -ForegroundColor White
    Write-Host "    SCK  (Serial Clock):         GPIO $spi_clk" -ForegroundColor White
    Write-Host "    RST  (Shared Reset):         GPIO $spi_reset" -ForegroundColor White
    Write-Host "    (No MISO - displays are write-only)" -ForegroundColor DarkGray
    
    # Allocate pins for each display device
    for ($i = 0; $i -lt $displayCount; $i++) {
        Write-Host "`nDisplay Device $i (GC9A01):" -ForegroundColor Green
        
        # Each device gets unique CS, DC, Backlight
        $cs = Claim-NextPin
        $dc = Claim-NextPin
        $backlight = Claim-NextPin
        
        Write-Host "  Unique Pins:" -ForegroundColor Cyan
        Write-Host "    CS  (Chip Select):       GPIO $cs" -ForegroundColor White
        Write-Host "    DC  (Data/Command):      GPIO $dc" -ForegroundColor White
        Write-Host "    BL  (Backlight):         GPIO $backlight" -ForegroundColor White
        Write-Host "  Shared Pins (used by display):" -ForegroundColor Cyan
        Write-Host "    MOSI: GPIO $spi_mosi, SCK: GPIO $spi_clk, RST: GPIO $spi_reset" -ForegroundColor DarkGray
    }
    
    Write-Host ""
    Write-Host "──────────────────────────────" -ForegroundColor DarkGray
    Write-Host "Summary:" -ForegroundColor Yellow
    Write-Host "  SPI Display Bus (write-only, no MISO):" -ForegroundColor Cyan
    Write-Host "    Total pins allocated: $($assigned_pins.Count)" -ForegroundColor White
    Write-Host "    Pins remaining: $($spi_assignable.Count - $assigned_pins.Count)" -ForegroundColor White
    Write-Host ""
    
    Write-Host "Wiring Instructions:" -ForegroundColor Yellow
    Write-Host "──────────────────────────────" -ForegroundColor DarkGray
    
    # Display 0 (Left Eye)
    $cs0 = $assigned_pins[3]  # First CS after shared pins (MOSI, SCK, RST = 3 pins)
    $dc0 = $assigned_pins[4]  # First DC
    Write-Host "`nLeft Eye Display:" -ForegroundColor Cyan
    Write-Host "  RST = GPIO $spi_reset" -ForegroundColor White
    Write-Host "  CS  = GPIO $cs0" -ForegroundColor Green
    Write-Host "  DC  = GPIO $dc0" -ForegroundColor Green
    Write-Host "  SDA = GPIO $spi_mosi" -ForegroundColor White
    Write-Host "  SCK = GPIO $spi_clk" -ForegroundColor White
    Write-Host "  GND = GND" -ForegroundColor DarkGray
    Write-Host "  VCC = 3.3V" -ForegroundColor DarkGray
    
    # Display 1 (Right Eye)
    if ($displayCount -gt 1) {
        $cs1 = $assigned_pins[6]  # Second CS
        $dc1 = $assigned_pins[7]  # Second DC
        Write-Host "`nRight Eye Display:" -ForegroundColor Cyan
        Write-Host "  RST = GPIO $spi_reset" -ForegroundColor White
        Write-Host "  CS  = GPIO $cs1" -ForegroundColor Green
        Write-Host "  DC  = GPIO $dc1" -ForegroundColor Green
        Write-Host "  SDA = GPIO $spi_mosi" -ForegroundColor White
        Write-Host "  SCK = GPIO $spi_clk" -ForegroundColor White
        Write-Host "  GND = GND" -ForegroundColor DarkGray
        Write-Host "  VCC = 3.3V" -ForegroundColor DarkGray
    }
    
    Write-Host ""
    Write-Host "Pin Usage Map:" -ForegroundColor Yellow
    Write-Host "──────────────────────────────" -ForegroundColor DarkGray
    $pinMap = @{}
    $pinMap[$spi_mosi] = "SPI Display MOSI (shared, connected to all displays)"
    $pinMap[$spi_clk] = "SPI Display CLK (shared, connected to all displays)"
    $pinMap[$spi_reset] = "SPI Display RST (shared, connected to all displays)"
    $pinMap[$assigned_pins[3]] = "Display 0 CS (Left Eye)"
    $pinMap[$assigned_pins[4]] = "Display 0 DC (Left Eye)"
    $pinMap[$assigned_pins[5]] = "Display 0 Backlight (Left Eye)"
    if ($displayCount -gt 1 -and $assigned_pins.Count -ge 9) {
        $pinMap[$assigned_pins[6]] = "Display 1 CS (Right Eye)"
        $pinMap[$assigned_pins[7]] = "Display 1 DC (Right Eye)"
        $pinMap[$assigned_pins[8]] = "Display 1 Backlight (Right Eye)"
    }
    
    foreach ($pin in ($pinMap.Keys | Sort-Object)) {
        Write-Host ("  GPIO {0,2} : {1}" -f $pin, $pinMap[$pin]) -ForegroundColor White
    }
    
} else {
    Write-Host "Error: Could not find $SubsystemPath" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Analysis complete!" -ForegroundColor Green
