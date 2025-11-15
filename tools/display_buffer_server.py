#!/usr/bin/env python3
"""
ESP32 Display Buffer Visualization Server
Receives raw display buffer data from ESP32 via TCP socket on port 5555
"""

import socket
import struct
import tkinter as tk
from tkinter import ttk
from PIL import Image, ImageTk
import threading
import time

# Server configuration
HOST = '0.0.0.0'  # Listen on all interfaces
PORT = 5555

class DisplayWindow:
    """Window to display ESP32 framebuffer data"""
    
    def __init__(self, width, height, title="ESP32 Display"):
        self.width = width
        self.height = height
        self.title = title
        self.image_data = None
        self.frame_count = 0
        self.last_update = time.time()
        
        # Create window
        self.window = tk.Toplevel()
        self.window.title(title)
        self.window.configure(bg='black')
        
        # Info label
        self.info_label = tk.Label(
            self.window,
            text=f"{title} | {width}x{height} | 0 FPS",
            fg='white',
            bg='black',
            font=('Arial', 10)
        )
        self.info_label.pack()
        
        # Canvas for display
        self.canvas = tk.Canvas(
            self.window,
            width=width,
            height=height,
            bg='black',
            highlightthickness=0
        )
        self.canvas.pack()
        
        self.photo_image = None
        
    def update_buffer(self, buffer_data, bytes_per_pixel):
        """Update display with new buffer data"""
        try:
            # Create PIL image from buffer
            if bytes_per_pixel == 2:  # RGB565
                # Convert RGB565 to RGB888
                pixels = []
                for i in range(0, len(buffer_data), 2):
                    rgb565 = struct.unpack('<H', buffer_data[i:i+2])[0]
                    r = ((rgb565 >> 11) & 0x1F) << 3
                    g = ((rgb565 >> 5) & 0x3F) << 2
                    b = (rgb565 & 0x1F) << 3
                    pixels.extend([r, g, b])
                img = Image.frombytes('RGB', (self.width, self.height), bytes(pixels))
                
            elif bytes_per_pixel == 3:  # RGB666/RGB888
                img = Image.frombytes('RGB', (self.width, self.height), buffer_data)
                
            else:
                print(f"Unsupported bytes_per_pixel: {bytes_per_pixel}")
                return
            
            # Convert to PhotoImage and display
            self.photo_image = ImageTk.PhotoImage(img)
            self.canvas.delete("all")
            self.canvas.create_image(0, 0, anchor=tk.NW, image=self.photo_image)
            
            # Update stats
            self.frame_count += 1
            now = time.time()
            elapsed = now - self.last_update
            if elapsed >= 1.0:
                fps = self.frame_count / elapsed
                self.info_label.config(text=f"{self.title} | {self.width}x{self.height} | {fps:.1f} FPS")
                self.frame_count = 0
                self.last_update = now
                
        except Exception as e:
            print(f"Error updating display: {e}")


class DisplayServer:
    """TCP server to receive display data from ESP32"""
    
    def __init__(self, root):
        self.root = root
        self.root.title("ESP32 Display Server")
        self.root.geometry("400x200")
        
        # Status label
        self.status_label = tk.Label(
            root,
            text=f"Listening on {HOST}:{PORT}...",
            font=('Arial', 12),
            pady=20
        )
        self.status_label.pack()
        
        # Connection info
        self.info_text = tk.Text(root, height=8, width=50)
        self.info_text.pack(padx=10, pady=10)
        
        self.displays = {}  # display_id -> DisplayWindow
        self.server_thread = None
        self.running = False
        
    def log(self, message):
        """Add message to info text"""
        self.info_text.insert(tk.END, f"{time.strftime('%H:%M:%S')} - {message}\n")
        self.info_text.see(tk.END)
        
    def handle_client(self, conn, addr):
        """Handle incoming connection from ESP32"""
        self.log(f"Connection from {addr}")
        
        try:
            while self.running:
                # Read packet header
                # Packet format (from generic_spi_display.src):
                # uint32_t magic (0xDEADBEEF)
                # uint32_t frame_number
                # uint32_t width
                # uint32_t height
                # uint32_t bytes_per_pixel
                # uint8_t data[]
                
                header = conn.recv(20)
                if len(header) < 20:
                    break
                    
                magic, frame_num, width, height, bpp = struct.unpack('<5I', header)
                
                if magic != 0xDEADBEEF:
                    self.log(f"Invalid magic: 0x{magic:08X}")
                    continue
                
                # Read buffer data
                buffer_size = width * height * bpp
                buffer_data = b''
                while len(buffer_data) < buffer_size:
                    chunk = conn.recv(min(4096, buffer_size - len(buffer_data)))
                    if not chunk:
                        break
                    buffer_data += chunk
                
                if len(buffer_data) != buffer_size:
                    self.log(f"Incomplete buffer: {len(buffer_data)}/{buffer_size}")
                    continue
                
                # Create or update display window
                display_id = f"{width}x{height}"
                if display_id not in self.displays:
                    self.log(f"New display: {width}x{height} RGB{bpp*8}")
                    self.displays[display_id] = DisplayWindow(width, height, f"Display {display_id}")
                
                # Update display on main thread
                self.root.after(0, self.displays[display_id].update_buffer, buffer_data, bpp)
                
        except Exception as e:
            self.log(f"Error: {e}")
        finally:
            conn.close()
            self.log(f"Disconnected from {addr}")
    
    def server_loop(self):
        """Main server loop"""
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind((HOST, PORT))
        server_socket.listen(5)
        server_socket.settimeout(1.0)
        
        self.log(f"Server started on {HOST}:{PORT}")
        
        while self.running:
            try:
                conn, addr = server_socket.accept()
                # Handle each connection in a separate thread
                client_thread = threading.Thread(target=self.handle_client, args=(conn, addr))
                client_thread.daemon = True
                client_thread.start()
            except socket.timeout:
                continue
            except Exception as e:
                if self.running:
                    self.log(f"Server error: {e}")
        
        server_socket.close()
        self.log("Server stopped")
    
    def start(self):
        """Start the server"""
        self.running = True
        self.server_thread = threading.Thread(target=self.server_loop)
        self.server_thread.daemon = True
        self.server_thread.start()
    
    def stop(self):
        """Stop the server"""
        self.running = False
        if self.server_thread:
            self.server_thread.join(timeout=2.0)


def main():
    root = tk.Tk()
    server = DisplayServer(root)
    server.start()
    
    def on_closing():
        server.stop()
        root.destroy()
    
    root.protocol("WM_DELETE_WINDOW", on_closing)
    root.mainloop()


if __name__ == "__main__":
    print("ESP32 Display Buffer Visualization Server")
    print("=" * 50)
    print(f"Listening on port {PORT}")
    print("Waiting for ESP32 connections...")
    print()
    main()
