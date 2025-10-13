#!/usr/bin/env python3
"""
Test the ESP32 Display Server
Simulates ESP32 sending component registration and animation data
"""

import requests
import json
import time
import math

# Your PC server address
SERVER_URL = "http://localhost:3000"

def register_component(component_id, display_type, width, height, bot_name, component_name):
    """Register a component with the display server"""
    data = {
        "device_id": component_id,
        "display_type": display_type,
        "display_width": width,
        "display_height": height,
        "bot_name": bot_name,
        "component_name": component_name,
        "mac_address": "AABBCCDDEEFF"
    }
    
    try:
        response = requests.post(f"{SERVER_URL}/api/register-device", json=data)
        if response.status_code == 200:
            print(f"Registered: {component_id}")
            return True
        else:
            print(f"Registration failed for {component_id}: {response.status_code}")
            return False
    except Exception as e:
        print(f"Registration error for {component_id}: {e}")
        return False

def send_animation_data(component_id, animation_name, eye_openness, pupil_size, color, expression):
    """Send animation frame data"""
    data = {
        "device_id": component_id,
        "timestamp_ms": int(time.time() * 1000),
        "loop_count": int(time.time()) % 1000,
        "animation_name": animation_name,
        "eye_openness": eye_openness,
        "pupil_size": pupil_size,
        "color": color,
        "expression": expression,
        "animation_active": True,
        "custom_data": ""
    }
    
    try:
        response = requests.post(f"{SERVER_URL}/api/animation-data", json=data)
        return response.status_code == 200
    except Exception as e:
        print(f"Animation data error for {component_id}: {e}")
        return False

def main():
    print("ESP32 Display Server Test")
    print("Make sure esp32_display_server.py is running!")
    print()
    
    # Wait for server to be ready
    input("Press Enter when display server is running...")
    
    # Register components
    print("Registering components...")
    register_component("GOBLIN_LEFT_EYE", "EYE", 200, 200, "GOBLIN_FULL", "LEFT_EYE")
    register_component("GOBLIN_RIGHT_EYE", "EYE", 200, 200, "GOBLIN_FULL", "RIGHT_EYE")
    register_component("GOBLIN_MOUTH", "MOUTH", 300, 150, "GOBLIN_FULL", "MOUTH")
    
    time.sleep(2)
    
    # Send animation data
    print("Sending animation data...")
    print("Watch the draggable windows!")
    
    frame = 0
    try:
        while True:
            # Animate eyes
            blink_cycle = (math.sin(frame * 0.1) + 1) / 2  # 0 to 1
            pupil_cycle = (math.sin(frame * 0.05) + 1) / 2  # 0 to 1
            color_cycle = int((math.sin(frame * 0.02) + 1) * 127.5)  # 0 to 255
            
            # Left eye - blink animation
            send_animation_data(
                "GOBLIN_LEFT_EYE", 
                "blink", 
                blink_cycle,  # Eye openness
                0.3 + pupil_cycle * 0.4,  # Pupil size
                (color_cycle << 8) | 0x00FF,  # Green with variable red
                0
            )
            
            # Right eye - slightly different timing
            send_animation_data(
                "GOBLIN_RIGHT_EYE", 
                "blink", 
                (math.sin(frame * 0.1 + 0.5) + 1) / 2,  # Offset blink
                0.2 + pupil_cycle * 0.5,  # Different pupil size
                0x00FF00 | (color_cycle << 16),  # Green with variable blue
                0
            )
            
            # Mouth - expression changes
            expression = int(math.sin(frame * 0.03) * 2)  # -2 to 2
            send_animation_data(
                "GOBLIN_MOUTH", 
                "talk", 
                1.0,  # Not used for mouth
                1.0,  # Not used for mouth  
                0xFFFF00,  # Yellow
                expression
            )
            
            frame += 1
            time.sleep(0.05)  # 20 FPS
            
    except KeyboardInterrupt:
        print("\nTest stopped")

if __name__ == "__main__":
    main()