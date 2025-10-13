#!/usr/bin/env python3
"""
ESP32 Animation Display Server
Creates draggable Windows for ESP32 components
Run with: python esp32_display_server.py
"""

import tkinter as tk
from tkinter import ttk
import json
import threading
import time
from http.server import HTTPServer, BaseHTTPRequestHandler
import urllib.parse
from collections import defaultdict
import colorsys
import math
import random

class AnimationWindow:
    """Individual draggable window for one ESP32 component"""
    
    def __init__(self, component_id, display_type, width, height, bot_name, component_name):
        self.component_id = component_id
        self.display_type = display_type
        self.bot_name = bot_name
        self.component_name = component_name
        
        # Create window
        self.window = tk.Toplevel()
        self.window.title(f"{bot_name} - {component_name}")
        self.window.geometry(f"{width}x{height+50}")  # Extra space for info
        self.window.configure(bg='black')
        
        # Info label
        self.info_label = tk.Label(
            self.window, 
            text=f"{component_name} ({display_type})",
            fg='white', 
            bg='black',
            font=('Arial', 8)
        )
        self.info_label.pack()
        
        # Canvas for animation
        self.canvas = tk.Canvas(
            self.window, 
            width=width, 
            height=height, 
            bg='black',
            highlightthickness=0
        )
        self.canvas.pack()
        
        # Animation state
        self.current_animation = "idle"
        self.eye_openness = 1.0
        self.pupil_size = 0.5
        self.color = 0x00FF00  # Green default
        self.expression = 0
        self.animation_active = False
        self.frame_count = 0
        self.distance = 100.0  # Current distance in cm
        
        print(f"Created window: {component_id} ({width}x{height})")
        
    def update_distance(self, distance_cm):
        """Update distance and adjust animation accordingly"""
        self.distance = distance_cm
        
        # Adjust pupil size based on distance (closer = larger pupils)
        if distance_cm < 30:  # Very close
            self.pupil_size = 0.8 + 0.2 * math.sin(time.time() * 3)  # Dilated + nervous flutter
        elif distance_cm < 60:  # Medium distance  
            self.pupil_size = 0.6
        else:  # Far away
            self.pupil_size = 0.3
            
        # Adjust eye openness (closer = more alert)
        if distance_cm < 20:  # Very close - wide eyes
            self.eye_openness = 0.9 + 0.1 * math.sin(time.time() * 2)
        elif distance_cm < 50:  # Medium - normal
            self.eye_openness = 0.8
        else:  # Far - sleepy
            self.eye_openness = 0.6
            
        # Color changes with proximity (green->yellow->red as you get closer)
        if distance_cm < 25:
            self.color = 0xFF4444  # Red - too close!
        elif distance_cm < 50:
            self.color = 0xFFAA00  # Orange-yellow
        else:
            self.color = 0x00FF44  # Green
            
        # Redraw with new distance-based parameters
        self.draw_frame()
    
    def update_animation_data(self, data):
        """Update animation state and redraw"""
        self.current_animation = data.get('animation_name', 'idle')
        self.eye_openness = data.get('eye_openness', 1.0)
        self.pupil_size = data.get('pupil_size', 0.5)
        self.color = data.get('color', 0x00FF00)
        self.expression = data.get('expression', 0)
        self.animation_active = data.get('animation_active', False)
        self.frame_count += 1
        
        # Update info
        status = "ACTIVE" if self.animation_active else "IDLE"
        self.info_label.config(text=f"{self.component_name} - {self.current_animation} ({status}) Frame:{self.frame_count}")
        
        # Redraw animation
        self.draw_frame()
    
    def draw_frame(self):
        """Draw current animation frame"""
        self.canvas.delete("all")
        
        if self.display_type == "EYE":
            self.draw_eye()
        elif self.display_type == "MOUTH":
            self.draw_mouth()
        else:
            self.draw_generic()
    
    def draw_eye(self):
        """Draw eye animation"""
        canvas_width = self.canvas.winfo_width() or 200
        canvas_height = self.canvas.winfo_height() or 200
        center_x = canvas_width // 2
        center_y = canvas_height // 2
        
        # Eye outline (affected by openness)
        eye_height = int(80 * self.eye_openness)
        eye_width = 80
        
        if eye_height > 5:  # Eye is open enough to see
            # Iris
            iris_color = f"#{self.color:06x}"
            self.canvas.create_oval(
                center_x - eye_width//2, center_y - eye_height//2,
                center_x + eye_width//2, center_y + eye_height//2,
                fill=iris_color, outline='white'
            )
            
            # Pupil
            pupil_size = int(30 * self.pupil_size)
            self.canvas.create_oval(
                center_x - pupil_size//2, center_y - pupil_size//2,
                center_x + pupil_size//2, center_y + pupil_size//2,
                fill='black'
            )
            
            # Highlight
            highlight_size = max(5, pupil_size // 3)
            self.canvas.create_oval(
                center_x - pupil_size//4, center_y - pupil_size//4,
                center_x - pupil_size//4 + highlight_size, center_y - pupil_size//4 + highlight_size,
                fill='white'
            )
        else:
            # Closed eye - just a line
            self.canvas.create_line(
                center_x - eye_width//2, center_y,
                center_x + eye_width//2, center_y,
                fill='white', width=3
            )
            
        # Distance indicator for eyes
        self.canvas.create_text(
            10, 10, 
            text=f"{self.distance:.0f}cm",
            fill='white', anchor='nw'
        )
    
    def draw_mouth(self):
        """Draw mouth animation with distance responsiveness"""
        canvas_width = self.canvas.winfo_width() or 300
        canvas_height = self.canvas.winfo_height() or 150
        center_x = canvas_width // 2
        center_y = canvas_height // 2
        
        # Distance-based mouth reactions
        if self.distance < 20:  # Very close - surprised/shocked
            # Wide open mouth
            self.canvas.create_oval(
                center_x - 30, center_y - 20,
                center_x + 30, center_y + 20,
                fill='black', outline='white', width=3
            )
            # Teeth
            for i in range(-20, 21, 8):
                self.canvas.create_rectangle(
                    center_x + i - 2, center_y - 15,
                    center_x + i + 2, center_y - 5,
                    fill='white'
                )
        elif self.distance < 40:  # Medium close - talking/animated
            # Animated talking mouth
            talk_cycle = math.sin(time.time() * 6) * 0.5 + 0.5
            mouth_height = int(15 * talk_cycle + 5)
            
            self.canvas.create_arc(
                center_x - 40, center_y - mouth_height,
                center_x + 40, center_y + mouth_height,
                start=0, extent=180,
                outline='orange', width=4, fill='black'
            )
        elif self.distance < 80:  # Normal distance - expression-based
            if self.expression >= 0:  # Happy/Neutral
                self.canvas.create_arc(
                    center_x - 50, center_y - 10,
                    center_x + 50, center_y + 20,
                    start=0, extent=180,
                    outline='yellow', width=3
                )
            else:  # Sad/Angry
                self.canvas.create_arc(
                    center_x - 40, center_y + 10,
                    center_x + 40, center_y + 30,
                    start=180, extent=180,
                    outline='red', width=3
                )
        else:  # Far away - sleepy/closed
            # Just a line (closed mouth)
            self.canvas.create_line(
                center_x - 30, center_y,
                center_x + 30, center_y,
                fill='gray', width=2
            )
            
        # Distance indicator
        self.canvas.create_text(
            10, 10, 
            text=f"{self.distance:.0f}cm",
            fill='white', anchor='nw'
        )
    
    def draw_generic(self):
        """Draw generic component visualization"""
        canvas_width = self.canvas.winfo_width() or 200
        canvas_height = self.canvas.winfo_height() or 200
        
        # Simple pulsing circle
        radius = int(50 + 20 * abs(self.eye_openness - 0.5))
        color = f"#{self.color:06x}"
        
        self.canvas.create_oval(
            canvas_width//2 - radius, canvas_height//2 - radius,
            canvas_width//2 + radius, canvas_height//2 + radius,
            fill=color, outline='white'
        )

class ESP32DisplayServer:
    """HTTP server that receives ESP32 data and manages windows"""
    
    def __init__(self, port=3000):
        self.port = port
        self.windows = {}  # component_id -> AnimationWindow
        self.root = tk.Tk()
        self.root.title("ESP32 Display Server")
        self.root.geometry("400x300")
        
        # Distance sensor simulation
        self.simulated_distance = 100.0  # cm
        self.distance_trend = -1  # -1 approaching, +1 retreating
        
        # Server status
        self.status_label = tk.Label(
            self.root, 
            text=f"ESP32 Server Ready\nListening on port {port}",
            font=('Arial', 12)
        )
        self.status_label.pack(pady=10)
        
        # Distance display
        self.distance_frame = tk.Frame(self.root)
        self.distance_frame.pack(pady=10)
        
        tk.Label(self.distance_frame, text="Simulated Distance:").pack()
        self.distance_scale = tk.Scale(
            self.distance_frame,
            from_=5, to=200,
            orient=tk.HORIZONTAL,
            length=300,
            command=self.on_distance_change
        )
        self.distance_scale.set(100)
        self.distance_scale.pack()
        
        self.distance_label = tk.Label(
            self.distance_frame,
            text="Distance: 100 cm"
        )
        self.distance_label.pack()
        
        # Auto distance animation
        self.auto_distance = tk.BooleanVar(value=True)
        tk.Checkbutton(
            self.root,
            text="Auto-animate distance",
            variable=self.auto_distance
        ).pack()
        
        self.component_count = tk.Label(
            self.root,
            text="Components: 0",
            font=('Arial', 10)
        )
        self.component_count.pack()
        
        # Start HTTP server in background
        self.start_server()
        
        # Start distance animation
        self.animate_distance()
        
    def on_distance_change(self, value):
        """Handle manual distance slider changes"""
        self.simulated_distance = float(value)
        self.distance_label.config(text=f"Distance: {self.simulated_distance:.0f} cm")
        self.update_all_windows()
        
    def animate_distance(self):
        """Animate distance sensor simulation"""
        if self.auto_distance.get():
            # Update distance with a sine wave pattern
            time_factor = time.time() * 0.5  # Slow oscillation
            base_distance = 50 + 40 * math.sin(time_factor)  # 10-90 cm range
            
            # Add some randomness
            noise = random.uniform(-5, 5)
            self.simulated_distance = max(5, min(200, base_distance + noise))
            
            # Update GUI
            self.distance_scale.set(self.simulated_distance)
            self.distance_label.config(text=f"Distance: {self.simulated_distance:.1f} cm")
            
            # Update all windows with new distance
            self.update_all_windows()
        
        # Schedule next update
        self.root.after(100, self.animate_distance)
        
    def update_all_windows(self):
        """Update all animation windows with current distance"""
        for window in self.windows.values():
            window.update_distance(self.simulated_distance)
    
    def start_server(self):
        """Start HTTP server in background thread"""
        def run_server():
            server = HTTPServer(('localhost', self.port), self.make_handler())
            print(f"ESP32 Display Server running on http://localhost:{self.port}")
            server.serve_forever()
        
        server_thread = threading.Thread(target=run_server, daemon=True)
        server_thread.start()
    
    def make_handler(self):
        """Create HTTP request handler with access to server instance"""
        server_instance = self
        
        class RequestHandler(BaseHTTPRequestHandler):
            def do_POST(self):
                if self.path == '/api/register-device':
                    self.handle_register_device(server_instance)
                elif self.path == '/api/animation-data':
                    self.handle_animation_data(server_instance)
                else:
                    self.send_response(404)
                    self.end_headers()
            
            def handle_register_device(self, server):
                content_length = int(self.headers['Content-Length'])
                post_data = self.rfile.read(content_length)
                data = json.loads(post_data.decode('utf-8'))
                
                component_id = data['device_id']
                display_type = data['display_type']
                width = data['display_width']
                height = data['display_height']
                bot_name = data['bot_name']
                component_name = data['component_name']
                
                # Create window on main thread
                def create_window():
                    server.windows[component_id] = AnimationWindow(
                        component_id, display_type, width, height, bot_name, component_name
                    )
                    server.component_count.config(text=f"Components: {len(server.windows)}")
                
                server.root.after(0, create_window)
                
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps({'status': 'registered'}).encode())
            
            def handle_animation_data(self, server):
                content_length = int(self.headers['Content-Length'])
                post_data = self.rfile.read(content_length)
                data = json.loads(post_data.decode('utf-8'))
                
                component_id = data['device_id']
                
                if component_id in server.windows:
                    # Update window on main thread
                    def update_window():
                        server.windows[component_id].update_animation_data(data)
                    
                    server.root.after(0, update_window)
                
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps({'status': 'received'}).encode())
            
            def log_message(self, format, *args):
                pass  # Suppress HTTP logs
        
        return RequestHandler
    
    def run(self):
        """Run the GUI main loop"""
        print("ESP32 Display Server GUI started")
        print("Waiting for ESP32 components to register...")
        self.root.mainloop()

if __name__ == "__main__":
    server = ESP32DisplayServer(port=3000)
    server.run()