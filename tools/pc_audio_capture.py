#!/usr/bin/env python3
"""
P32 Animatronic Bot - PC Audio Capture Tool
Captures audio stream from ESP32 via serial and plays it on PC speakers

Usage:
    python tools/pc_audio_capture.py COM3
    python tools/pc_audio_capture.py /dev/ttyUSB0

Requires:
    pip install pyserial pygame numpy
"""

import sys
import time
import serial
import pygame
import numpy as np
import threading
import queue
from dataclasses import dataclass
from typing import Optional

@dataclass
class AudioConfig:
    sample_rate: int = 44100
    channels: int = 1
    buffer_size: int = 1024
    format: str = "INT16"

class ESP32AudioCapture:
    def __init__(self, serial_port: str, baud_rate: int = 115200):
        self.serial_port = serial_port
        self.baud_rate = baud_rate
        self.serial_conn: Optional[serial.Serial] = None
        self.audio_config = AudioConfig()
        self.audio_queue = queue.Queue(maxsize=100)
        self.running = False
        
        # Initialize pygame mixer
        pygame.mixer.pre_init(
            frequency=self.audio_config.sample_rate,
            size=-16,  # 16-bit signed
            channels=self.audio_config.channels,
            buffer=512
        )
        pygame.mixer.init()
        
        print(f"PC Audio Capture initialized")
        print(f"Serial: {serial_port} @ {baud_rate}")
        print(f"Audio: {self.audio_config.sample_rate}Hz, {self.audio_config.channels}ch")
        
    def connect_serial(self) -> bool:
        """Connect to ESP32 serial port"""
        try:
            self.serial_conn = serial.Serial(
                self.serial_port, 
                self.baud_rate, 
                timeout=1.0
            )
            print(f" Connected to {self.serial_port}")
            return True
        except Exception as e:
            print(f" Failed to connect to {self.serial_port}: {e}")
            return False
    
    def parse_audio_header(self, line: str) -> bool:
        """Parse audio configuration from ESP32"""
        if line.startswith("SAMPLE_RATE="):
            self.audio_config.sample_rate = int(line.split("=")[1])
        elif line.startswith("CHANNELS="):
            self.audio_config.channels = int(line.split("=")[1])
        elif line.startswith("BUFFER_SIZE="):
            self.audio_config.buffer_size = int(line.split("=")[1])
        elif line.startswith("FORMAT="):
            self.audio_config.format = line.split("=")[1]
        elif line == "AUDIO_HEADER_END":
            print(f" Audio config: {self.audio_config}")
            return True
        return False
    
    def serial_reader_thread(self):
        """Thread to read serial data from ESP32"""
        print("Serial reader thread started")
        header_mode = False
        
        while self.running:
            try:
                if not self.serial_conn:
                    time.sleep(0.1)
                    continue
                    
                line = self.serial_conn.readline().decode('utf-8').strip()
                if not line:
                    continue
                
                # Handle audio stream start
                if line == "AUDIO_STREAM_START":
                    print(" ESP32 audio stream detected")
                    header_mode = True
                    continue
                
                # Parse audio configuration
                if header_mode:
                    if self.parse_audio_header(line):
                        header_mode = False
                        # Reinitialize pygame with new settings
                        pygame.mixer.quit()
                        pygame.mixer.pre_init(
                            frequency=self.audio_config.sample_rate,
                            size=-16,
                            channels=self.audio_config.channels,
                            buffer=512
                        )
                        pygame.mixer.init()
                    continue
                
                # Handle audio data
                if line.startswith("AUDIO_DATA:"):
                    sample_str = line.split(":")[1]
                    try:
                        sample_value = int(sample_str)
                        if not self.audio_queue.full():
                            self.audio_queue.put(sample_value)
                    except ValueError:
                        pass
                
                # Handle audio events
                elif line.startswith("AUDIO_EVENT:"):
                    event_data = line.split(":", 1)[1]
                    self.handle_audio_event(event_data)
                
                # Handle speech events
                elif line.startswith("SPEECH_EVENT:"):
                    speech_data = line.split(":", 1)[1]
                    self.handle_speech_event(speech_data)
                    
                # Handle regular ESP32 log output
                else:
                    # Print non-audio serial output for debugging
                    if any(tag in line for tag in ["speaker", "i2s_driver", "goblin_nose"]):
                        print(f"ESP32: {line}")
                        
            except Exception as e:
                if self.running:
                    print(f"Serial read error: {e}")
                time.sleep(0.1)
    
    def handle_audio_event(self, event_data: str):
        """Handle audio events from ESP32"""
        if event_data.startswith("PLAY="):
            # Parse PLAY=goblin_growl,FREQ=150.0,VOL=0.40,TYPE=VOCALIZATION
            parts = event_data.split(",")
            sound_name = parts[0].split("=")[1]
            freq = float(parts[1].split("=")[1]) if len(parts) > 1 else 440.0
            volume = float(parts[2].split("=")[1]) if len(parts) > 2 else 0.3
            sound_type = parts[3].split("=")[1] if len(parts) > 3 else "UNKNOWN"
            
            # Enhanced logging with goblin sound descriptions
            emoji = self.get_sound_emoji(sound_name, sound_type)
            description = self.get_sound_description(sound_name)
            
            print(f"{emoji} {description}")
            print(f"     {sound_name} ({freq:.1f}Hz, {volume:.2f} vol, {sound_type})")
            
        elif event_data == "STOP":
            print(" Audio stopped")
    
    def get_sound_emoji(self, sound_name: str, sound_type: str) -> str:
        """Get appropriate emoji for sound type"""
        if sound_type == "SPEECH":
            return ""
        elif sound_type == "VOCALIZATION":
            if "growl" in sound_name or "roar" in sound_name:
                return ""
            elif "laugh" in sound_name or "giggle" in sound_name:
                return ""
            else:
                return ""
        elif sound_type == "EMOTION":
            return "" if "angry" in sound_name else ""
        elif sound_type == "ALERT":
            return ""
        else:
            return ""
    
    def get_sound_description(self, sound_name: str) -> str:
        """Get human-readable description of goblin sounds"""
        descriptions = {
            "goblin_growl_low": "Goblin makes deep threatening growl",
            "goblin_cackle": "Goblin cackles with evil laughter",
            "goblin_speech_greetings": "Goblin says 'Grrrak!' (Hello)",
            "goblin_speech_warning": "Goblin shouts 'Krash grok!' (Danger!)",
            "goblin_speech_question": "Goblin asks 'Grok?' (What?)",
            "goblin_emotional_angry": "Goblin expresses anger",
            "goblin_emotional_happy": "Goblin sounds happy",
            "proximity_close": "Goblin detects nearby movement",
            "system_boot": "Goblin system starting up"
        }
        
        return descriptions.get(sound_name, f"Goblin makes {sound_name.replace('_', ' ')}")
    
    def handle_speech_event(self, speech_data: str):
        """Handle speech synthesis events from ESP32"""
        if speech_data.startswith("PHRASE="):
            phrase = speech_data.split("=")[1]
            goblin_translations = {
                "hello": "Grrrak!",
                "warning": "Krash grok!",
                "attack": "GRAAAHHH!",
                "curious": "Grok?",
                "yes": "Grok grok!",
                "no": "Grak! Grak!",
                "hungry": "Nom nom grak!",
                "sleep": "Zzzgrok..."
            }
            
            goblin_text = goblin_translations.get(phrase, "Grk grk grok!")
            print(f" Goblin Speech: '{phrase}'  '{goblin_text}'")
    
    def audio_player_thread(self):
        """Thread to play audio samples"""
        print("Audio player thread started")
        samples_buffer = []
        
        while self.running:
            try:
                # Collect samples for playback
                while len(samples_buffer) < 128 and not self.audio_queue.empty():
                    sample = self.audio_queue.get_nowait()
                    samples_buffer.append(sample)
                
                # Play buffer when we have enough samples
                if len(samples_buffer) >= 128:
                    # Convert to numpy array and normalize
                    audio_array = np.array(samples_buffer, dtype=np.int16)
                    
                    # Create pygame Sound object and play
                    if self.audio_config.channels == 1:
                        # Mono: duplicate for stereo playback
                        stereo_array = np.column_stack((audio_array, audio_array))
                    else:
                        stereo_array = audio_array.reshape(-1, 2)
                    
                    try:
                        sound = pygame.sndarray.make_sound(stereo_array)
                        sound.play()
                    except Exception as e:
                        print(f"Audio playback error: {e}")
                    
                    samples_buffer.clear()
                
                time.sleep(0.01)  # 10ms delay
                
            except Exception as e:
                if self.running:
                    print(f"Audio playback error: {e}")
                time.sleep(0.1)
    
    def start_capture(self):
        """Start audio capture and playback"""
        if not self.connect_serial():
            return False
        
        self.running = True
        
        # Start threads
        serial_thread = threading.Thread(target=self.serial_reader_thread, daemon=True)
        audio_thread = threading.Thread(target=self.audio_player_thread, daemon=True)
        
        serial_thread.start()
        audio_thread.start()
        
        print("\n PC Audio Capture running...")
        print("Press Ctrl+C to stop")
        
        try:
            while self.running:
                time.sleep(1)
                # Print queue status periodically
                if self.audio_queue.qsize() > 50:
                    print(f"Audio buffer: {self.audio_queue.qsize()}/100")
                    
        except KeyboardInterrupt:
            print("\n Stopping audio capture...")
            self.running = False
            
        return True
    
    def stop_capture(self):
        """Stop audio capture"""
        self.running = False
        if self.serial_conn:
            self.serial_conn.close()
        pygame.mixer.quit()

def main():
    if len(sys.argv) != 2:
        print("Usage: python pc_audio_capture.py <serial_port>")
        print("Example: python pc_audio_capture.py COM3")
        print("Example: python pc_audio_capture.py /dev/ttyUSB0")
        sys.exit(1)
    
    serial_port = sys.argv[1]
    
    try:
        capture = ESP32AudioCapture(serial_port)
        capture.start_capture()
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()