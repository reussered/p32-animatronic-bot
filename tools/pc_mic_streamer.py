#!/usr/bin/env python3
"""
PC Microphone Streamer for ESP32 I2S Debug Driver

Captures audio from PC microphone and streams it to ESP32 via UART
for testing goblin ear audio processing without physical I2S microphones.

Usage: python tools/pc_mic_streamer.py [COM_PORT]
"""

import sys
import time
import struct
import serial
import pyaudio
import threading
from queue import Queue

# Audio configuration
SAMPLE_RATE = 16000
CHANNELS = 1
SAMPLE_WIDTH = 2  # 16-bit
CHUNK_SIZE = 512  # Samples per packet
UART_BAUD = 921600

# Packet format: [0xAA][0x55][size_low][size_high][audio_data...]
PACKET_HEADER = b'\xAA\x55'

class PCMicrophoneStreamer:
    def __init__(self, com_port='COM3'):
        self.com_port = com_port
        self.audio = None
        self.stream = None
        self.serial_port = None
        self.audio_queue = Queue(maxsize=10)
        self.running = False
        self.bytes_sent = 0
        self.packets_sent = 0
        
    def initialize_audio(self):
        """Initialize PyAudio for microphone capture"""
        try:
            self.audio = pyaudio.PyAudio()
            
            # Find default input device
            default_input = self.audio.get_default_input_device_info()
            print(f"Using microphone: {default_input['name']}")
            
            self.stream = self.audio.open(
                format=pyaudio.paInt16,
                channels=CHANNELS,
                rate=SAMPLE_RATE,
                input=True,
                frames_per_buffer=CHUNK_SIZE,
                stream_callback=self.audio_callback)
                
            print(f"Audio initialized: {SAMPLE_RATE}Hz, {CHANNELS} channel(s), 16-bit")
            return True
            
        except Exception as e:
            print(f"Failed to initialize audio: {e}")
            return False
    
    def initialize_serial(self):
        """Initialize serial connection to ESP32"""
        try:
            self.serial_port = serial.Serial(
                port=self.com_port,
                baudrate=UART_BAUD,
                bytesize=8,
                parity='N',
                stopbits=1,
                timeout=0.1)
                
            print(f"Serial port opened: {self.com_port} at {UART_BAUD} baud")
            return True
            
        except Exception as e:
            print(f"Failed to open serial port {self.com_port}: {e}")
            return False
    
    def audio_callback(self, in_data, frame_count, time_info, status):
        """PyAudio callback - called when audio data is available"""
        if not self.audio_queue.full():
            self.audio_queue.put(in_data)
        return (None, pyaudio.paContinue)
    
    def create_audio_packet(self, audio_data):
        """Create UART packet with audio data"""
        # Packet format: [0xAA][0x55][size_low][size_high][audio_data...]
        size = len(audio_data)
        size_bytes = struct.pack('<H', size)  # Little-endian 16-bit size
        packet = PACKET_HEADER + size_bytes + audio_data
        return packet
    
    def serial_sender_thread(self):
        """Thread that sends audio packets via serial"""
        while self.running:
            try:
                # Get audio data from queue (blocking)
                audio_data = self.audio_queue.get(timeout=1.0)
                
                # Create and send packet
                packet = self.create_audio_packet(audio_data)
                self.serial_port.write(packet)
                
                # Update statistics
                self.bytes_sent += len(packet)
                self.packets_sent += 1
                
                # Print progress every 100 packets (~3 seconds)
                if self.packets_sent % 100 == 0:
                    print(f"Sent {self.packets_sent} packets ({self.bytes_sent} bytes)")
                    
            except Exception as e:
                if self.running:  # Only print errors if we're supposed to be running
                    print(f"Serial send error: {e}")
                break
    
    def start_streaming(self):
        """Start microphone capture and streaming"""
        print("Starting PC microphone streaming to ESP32...")
        print("Press Ctrl+C to stop")
        
        if not self.initialize_audio():
            return False
            
        if not self.initialize_serial():
            return False
        
        self.running = True
        
        # Start audio capture
        self.stream.start_stream()
        
        # Start serial sender thread
        sender_thread = threading.Thread(target=self.serial_sender_thread)
        sender_thread.daemon = True
        sender_thread.start()
        
        try:
            # Keep main thread alive
            while self.running:
                time.sleep(0.1)
                
        except KeyboardInterrupt:
            print("\nStopping microphone streaming...")
            
        finally:
            self.stop_streaming()
            
        return True
    
    def stop_streaming(self):
        """Stop streaming and cleanup"""
        self.running = False
        
        if self.stream:
            self.stream.stop_stream()
            self.stream.close()
            
        if self.audio:
            self.audio.terminate()
            
        if self.serial_port:
            self.serial_port.close()
            
        print(f"Streaming stopped. Sent {self.packets_sent} packets ({self.bytes_sent} bytes)")

def main():
    com_port = sys.argv[1] if len(sys.argv) > 1 else 'COM3'
    
    print("=== PC Microphone Streamer for ESP32 ===")
    print(f"Target: ESP32 I2S Debug Driver via {com_port}")
    print(f"Audio: {SAMPLE_RATE}Hz, 16-bit mono, {CHUNK_SIZE} samples/packet")
    print("")
    
    streamer = PCMicrophoneStreamer(com_port)
    
    try:
        streamer.start_streaming()
    except Exception as e:
        print(f"Streaming failed: {e}")
        return 1
        
    return 0

if __name__ == "__main__":
    sys.exit(main())