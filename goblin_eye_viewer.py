#!/usr/bin/env python3
"""
Goblin Eye Visualization Server
Receives display buffer data from ESP32 and shows it in a window
"""

import socket
import struct
import threading
import tkinter as tk
from tkinter import Canvas
import time
import sys

class GoblinEyeViewer:
    def __init__(self, host='0.0.0.0', port=8888):
        self.host = host
        self.port = port
        self.running = False
        
        # Display parameters (will be updated from packets)
        self.display_width = 240
        self.display_height = 320
        self.chunk_size = 0
        
        # GUI setup
        self.root = tk.Tk()
        self.root.title("Goblin Eye Viewer")
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
        
        # Create canvas for display
        self.canvas = Canvas(self.root, width=self.display_width*2, height=self.display_height,
                           bg='black')
        self.canvas.pack(padx=10, pady=10)
        
        # Status label
        self.status_label = tk.Label(self.root, text="Waiting for ESP32 connection...", 
                                   fg="orange")
        self.status_label.pack(pady=5)
        
        # Eye position labels
        self.position_label = tk.Label(self.root, text="Position: Unknown", fg="blue")
        self.position_label.pack(pady=2)
        
        # Chunk info label
        self.chunk_label = tk.Label(self.root, text="Chunk: 0/0", fg="green")
        self.chunk_label.pack(pady=2)
        
        # Pixel buffer for current display
        self.pixel_buffer = []
        
    def rgb565_to_rgb888(self, rgb565):
        """Convert RGB565 to RGB888 tuple"""
        r = ((rgb565 >> 11) & 0x1F) << 3  # 5 bits -> 8 bits
        g = ((rgb565 >> 5) & 0x3F) << 2   # 6 bits -> 8 bits  
        b = (rgb565 & 0x1F) << 3          # 5 bits -> 8 bits
        return (r, g, b)
    
    def parse_packet(self, data):
        """Parse incoming packet from ESP32"""
        if len(data) < 16:  # Minimum header size
            return None
            
        # Unpack header: magic, chunk_num, chunk_size, x, y, z
        magic, chunk_num, chunk_size, eye_x, eye_y, eye_z = struct.unpack('<IIIHHH', data[:16])
        
        if magic != 0xDEADBEEF:
            print(f"Invalid magic: 0x{magic:08X}")
            return None
            
        # Extract pixel data
        pixel_data = data[16:16+chunk_size]
        
        return {
            'chunk_num': chunk_num,
            'chunk_size': chunk_size,
            'eye_pos': (eye_x, eye_y, eye_z),
            'pixels': pixel_data
        }
    
    def update_display(self, packet):
        """Update the canvas with new pixel data"""
        if not packet:
            return
            
        chunk_num = packet['chunk_num']
        pixels = packet['pixels']
        eye_pos = packet['eye_pos']
        
        # Update status labels
        self.root.after(0, lambda: self.position_label.config(
            text=f"Position: ({eye_pos[0]}, {eye_pos[1]}, {eye_pos[2]})"))
        self.root.after(0, lambda: self.chunk_label.config(
            text=f"Chunk: {chunk_num}/{len(pixels)//2} pixels"))
        
        # Convert RGB565 pixels to display rectangles
        pixel_size = 2  # Scale factor for visibility
        
        for i in range(0, len(pixels), 2):
            if i + 1 >= len(pixels):
                break
                
            # Get RGB565 pixel (little endian)
            rgb565 = struct.unpack('<H', pixels[i:i+2])[0]
            r, g, b = self.rgb565_to_rgb888(rgb565)
            color = f"#{r:02x}{g:02x}{b:02x}"
            
            # Calculate pixel position in chunk
            pixel_index = i // 2
            
            # Assume pixels are arranged left-to-right, top-to-bottom
            pixels_per_row = self.display_width
            chunk_height = len(pixels) // (2 * pixels_per_row)
            
            if pixels_per_row > 0 and chunk_height > 0:
                x = pixel_index % pixels_per_row
                y = (chunk_num * chunk_height) + (pixel_index // pixels_per_row)
                
                # Draw pixel as small rectangle
                x1, y1 = x * pixel_size, y * pixel_size
                x2, y2 = x1 + pixel_size, y1 + pixel_size
                
                self.root.after(0, lambda x1=x1,y1=y1,x2=x2,y2=y2,color=color: 
                              self.canvas.create_rectangle(x1, y1, x2, y2, 
                                                         fill=color, outline=color))
    
    def handle_client(self, conn, addr):
        """Handle connection from ESP32"""
        print(f"ESP32 connected from {addr}")
        self.root.after(0, lambda: self.status_label.config(
            text=f"Connected to ESP32: {addr[0]}", fg="green"))
        
        try:
            buffer = b''
            while self.running:
                data = conn.recv(4096)
                if not data:
                    break
                    
                buffer += data
                
                # Try to parse complete packets
                while len(buffer) >= 16:  # Minimum header size
                    # Get packet size from header
                    if len(buffer) >= 12:
                        chunk_size = struct.unpack('<I', buffer[8:12])[0]
                        packet_size = 16 + chunk_size
                        
                        if len(buffer) >= packet_size:
                            # Complete packet received
                            packet_data = buffer[:packet_size]
                            buffer = buffer[packet_size:]
                            
                            # Parse and display
                            packet = self.parse_packet(packet_data)
                            if packet:
                                self.update_display(packet)
                        else:
                            break  # Wait for more data
                    else:
                        break  # Wait for more data
                        
        except Exception as e:
            print(f"Client error: {e}")
        finally:
            conn.close()
            print("ESP32 disconnected")
            self.root.after(0, lambda: self.status_label.config(
                text="ESP32 disconnected", fg="red"))
    
    def server_thread(self):
        """Main server thread"""
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.settimeout(1.0)  # Allow periodic checks for self.running
        
        try:
            sock.bind((self.host, self.port))
            sock.listen(1)
            print(f"Goblin Eye Server listening on {self.host}:{self.port}")
            
            while self.running:
                try:
                    conn, addr = sock.accept()
                    # Handle each ESP32 connection in the main thread for GUI updates
                    self.handle_client(conn, addr)
                except socket.timeout:
                    continue  # Check if still running
                except Exception as e:
                    if self.running:
                        print(f"Server error: {e}")
                    
        except Exception as e:
            print(f"Failed to start server: {e}")
        finally:
            sock.close()
    
    def start(self):
        """Start the server and GUI"""
        self.running = True
        
        # Start server in background thread
        server_thread = threading.Thread(target=self.server_thread, daemon=True)
        server_thread.start()
        
        # Start GUI main loop
        print("Starting Goblin Eye Viewer...")
        self.root.mainloop()
    
    def on_closing(self):
        """Clean shutdown"""
        print("Shutting down...")
        self.running = False
        self.root.destroy()

if __name__ == "__main__":
    viewer = GoblinEyeViewer()
    viewer.start()