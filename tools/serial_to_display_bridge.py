#!/usr/bin/env python3
"""
Serial to Display Server Bridge
Reads JSON from ESP32 serial output and forwards to display server
"""
import serial
import requests
import json
import sys
import time

SERIAL_PORT = 'COM3'
BAUD_RATE = 115200
DISPLAY_SERVER = 'http://localhost:3000'

def main():
    print(f"Connecting to {SERIAL_PORT} at {BAUD_RATE} baud...")
    
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print("Serial connected! Waiting for ESP32 data...")
        
        # Track registered devices
        registered = set()
        
        while True:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            
            if not line:
                continue
                
            # Look for JSON lines starting with specific markers
            if line.startswith('DISPLAY_REGISTER:'):
                try:
                    json_str = line.split('DISPLAY_REGISTER:', 1)[1]
                    data = json.loads(json_str)
                    
                    # Forward to server
                    resp = requests.post(f'{DISPLAY_SERVER}/api/register-device', json=data, timeout=2)
                    if resp.status_code == 200:
                        registered.add(data['device_id'])
                        print(f"✓ Registered: {data['component_name']} ({data['device_id']})")
                    else:
                        print(f"✗ Registration failed: {resp.status_code}")
                        
                except Exception as e:
                    print(f"Error registering device: {e}")
                    
            elif line.startswith('DISPLAY_DATA:'):
                try:
                    json_str = line.split('DISPLAY_DATA:', 1)[1]
                    data = json.loads(json_str)
                    
                    # Forward to server
                    resp = requests.post(f'{DISPLAY_SERVER}/api/animation-data', json=data, timeout=2)
                    if resp.status_code != 200:
                        print(f"✗ Data send failed: {resp.status_code}")
                        
                except Exception as e:
                    print(f"Error sending data: {e}")
            
    except serial.SerialException as e:
        print(f"Serial error: {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\nStopping bridge...")
        sys.exit(0)

if __name__ == '__main__':
    main()
