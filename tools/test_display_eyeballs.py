!/usr/bin/env python3
"""Test the display server with fake eyeball data"""
import requests
import time
import math

SERVER = 'http://localhost:3000'

# Register the eyeballs
left_eye = {
    "device_id": "test_left_eye",
    "display_type": "eye",
    "display_width": 240,
    "display_height": 240,
    "bot_name": "Test Goblin",
    "component_name": "Left Eye"
}

right_eye = {
    "device_id": "test_right_eye",
    "display_type": "eye",
    "display_width": 240,
    "display_height": 240,
    "bot_name": "Test Goblin",
    "component_name": "Right Eye"
}

print("Registering eyes...")
requests.post(f'{SERVER}/api/register-device', json=left_eye)
requests.post(f'{SERVER}/api/register-device', json=right_eye)
print("Eyes registered! Watch the display server window...")

# Animate the eyes
frame = 0
while True:
    t = frame * 0.1
    
    # Pulsing eyeballs
    eye_openness = 0.7 + 0.3 * math.sin(t)
    pupil_size = 0.4 + 0.2 * math.sin(t * 0.5)
    expression = int(math.sin(t * 0.3) * 2)
    
    # Color cycles through emotions
    if (frame // 50) % 3 == 0:
        color = 0x00FF00  # Green - happy
    elif (frame // 50) % 3 == 1:
        color = 0xFF4444  # Red - angry
    else:
        color = 0x4444FF  # Blue - fearful
    
    data = {
        "device_id": "test_left_eye",
        "animation_name": "test",
        "eye_openness": eye_openness,
        "pupil_size": pupil_size,
        "color": color,
        "expression": expression
    }
    
    requests.post(f'{SERVER}/api/animation-data', json=data)
    
    data["device_id"] = "test_right_eye"
    requests.post(f'{SERVER}/api/animation-data', json=data)
    
    frame += 1
    time.sleep(0.1)
    
    if frame % 10 == 0:
        print(f"Frame {frame}: openness={eye_openness:.2f}, pupil={pupil_size:.2f}, color=0x{color:06X}")
