# WiFi Setup for Display Debug Mode

To see the ESP32 display output on your PC, you need to configure WiFi credentials.

## Steps:

1. **Edit WiFi Settings**
   Open: `config/components/drivers/generic_spi_display.src`
   
   Find these lines near the top (around line 16-18):
   ```cpp
   #define WIFI_SSID "YOUR_WIFI_SSID"
   #define WIFI_PASS "YOUR_WIFI_PASSWORD"
   #define SERVER_IP "192.168.1.100"  // Your PC's IP address
   ```

2. **Replace with your WiFi credentials:**
   ```cpp
   #define WIFI_SSID "ATT7b9q3Ku"
   #define WIFI_PASS "89cqy6d7jjd7"
   #define SERVER_IP "192.168.1.XXX"  // Your PC's actual IP
   ```

3. **Find your PC's IP address:**
   
   In PowerShell run:
   ```powershell
   ipconfig | Select-String "IPv4"
   ```
   
   Look for the IP address on your WiFi adapter (usually starts with 192.168.x.x or 10.x.x.x)

4. **Rebuild and upload:**
   ```powershell
   python tools\generate_tables.py goblin_head src
   pio run -e goblin_head -t upload
   ```

5. **Start the display server:**
   ```powershell
   python tools\display_buffer_server.py
   ```

6. **Watch the display server window** - you should see:
   - "Connection from (ESP32_IP)" 
   - Display windows pop up showing the eye buffers in real-time!

## Troubleshooting:

- **ESP32 not connecting?** 
  - Check WiFi credentials are correct
  - Make sure PC and ESP32 are on same network
  - Check firewall isn't blocking port 5555

- **Connection but no display?**
  - Check serial monitor for errors: `pio device monitor`
  - Verify debug=true in `config/bots/bot_families/goblins/head/goblin_head.json`
