#!/usr/bin/env python3
"""
P32 Goblin Audio Demo
Interactive demonstration of goblin sounds and speech

Usage:
    python tools/goblin_audio_demo.py COM3

This script sends commands to the ESP32 to trigger various goblin sounds and speech.
"""

import sys
import time
import serial
import threading

class GoblinAudioDemo:
    def __init__(self, serial_port: str, baud_rate: int = 115200):
        self.serial_port = serial_port
        self.baud_rate = baud_rate
        self.serial_conn = None
        self.running = False
        
    def connect_serial(self) -> bool:
        """Connect to ESP32 serial port"""
        try:
            self.serial_conn = serial.Serial(
                self.serial_port, 
                self.baud_rate, 
                timeout=1.0
            )
            print(f" Connected to ESP32 at {self.serial_port}")
            return True
        except Exception as e:
            print(f" Failed to connect: {e}")
            return False
    
    def send_command(self, command: str):
        """Send command to ESP32"""
        if self.serial_conn:
            self.serial_conn.write(f"{command}\n".encode())
            print(f"Sent: {command}")
    
    def monitor_serial(self):
        """Monitor ESP32 output"""
        while self.running:
            try:
                if self.serial_conn and self.serial_conn.in_waiting:
                    line = self.serial_conn.readline().decode('utf-8').strip()
                    if line:
                        print(f"ESP32: {line}")
            except Exception as e:
                if self.running:
                    print(f"Serial monitor error: {e}")
            time.sleep(0.01)
    
    def run_demo(self):
        """Run interactive goblin audio demonstration"""
        if not self.connect_serial():
            return
        
        self.running = True
        
        # Start serial monitor thread
        monitor_thread = threading.Thread(target=self.monitor_serial, daemon=True)
        monitor_thread.start()
        
        print("\n" + "="*60)
        print(" P32 GOBLIN AUDIO DEMONSTRATION")
        print("="*60)
        print("\nAvailable Commands:")
        print("  sounds    - Play different goblin sound effects")
        print("  speech    - Demonstrate goblin speech synthesis")
        print("  emotions  - Show emotional responses")
        print("  proximity - Simulate proximity detection")
        print("  demo      - Auto-play demonstration sequence")
        print("  quit      - Exit demonstration")
        print("\nNote: Run 'python tools/pc_audio_capture.py {0}' in another terminal".format(self.serial_port))
        print("      to hear the actual audio on your PC speakers.\n")
        
        try:
            while self.running:
                command = input(" Enter command: ").strip().lower()
                
                if command == "quit" or command == "exit":
                    break
                elif command == "sounds":
                    self.demo_sound_effects()
                elif command == "speech":
                    self.demo_speech_synthesis()
                elif command == "emotions":
                    self.demo_emotional_responses()
                elif command == "proximity":
                    self.demo_proximity_detection()
                elif command == "demo":
                    self.auto_demonstration()
                elif command == "help":
                    self.show_help()
                else:
                    print("Unknown command. Type 'help' for available commands.")
                
        except KeyboardInterrupt:
            pass
        
        print("\n Stopping goblin audio demo...")
        self.running = False
        if self.serial_conn:
            self.serial_conn.close()
    
    def demo_sound_effects(self):
        """Demonstrate various goblin sound effects"""
        print("\n Goblin Sound Effects Demo")
        sounds = [
            ("goblin_growl_low", "Deep threatening growl"),
            ("goblin_cackle", "Evil cackling laugh"),
            ("goblin_roar_short", "Intimidating roar"),
            ("goblin_hiss", "Threatening hiss"),
            ("goblin_giggle", "Amused giggle"),
            ("goblin_howl", "Mournful howl")
        ]
        
        for sound_name, description in sounds:
            print(f"  Playing: {description}")
            # Send command to trigger sound (you'll need to implement command interface)
            time.sleep(2)
    
    def demo_speech_synthesis(self):
        """Demonstrate goblin speech synthesis"""
        print("\n Goblin Speech Synthesis Demo")
        phrases = [
            ("hello", "Goblin greeting"),
            ("warning", "Danger alert"),
            ("curious", "Questioning sound"),
            ("yes", "Agreement"),
            ("no", "Disagreement"),
            ("attack", "Battle cry")
        ]
        
        for phrase, description in phrases:
            print(f"  Speaking: '{phrase}' ({description})")
            # Send speech command
            time.sleep(2)
    
    def demo_emotional_responses(self):
        """Demonstrate emotional audio responses"""
        print("\n Goblin Emotional Responses Demo")
        emotions = [
            ("angry", 0.8, "High intensity anger"),
            ("happy", 0.6, "Moderate happiness"),
            ("scared", 0.9, "Very frightened"),
            ("surprised", 0.5, "Mild surprise"),
            ("sad", 0.4, "Low intensity sadness")
        ]
        
        for emotion, intensity, description in emotions:
            print(f"  Emotion: {emotion} (intensity: {intensity}) - {description}")
            # Send emotion command
            time.sleep(2)
    
    def demo_proximity_detection(self):
        """Simulate proximity detection responses"""
        print("\n Proximity Detection Simulation")
        distances = [50, 30, 15, 8, 3, 1, 5, 20, 50]
        
        for distance in distances:
            print(f"  Simulating object at {distance}cm...")
            if distance <= 5:
                print("     Very close! Angry response")
            elif distance <= 10:
                print("     Close! Curious response")
            elif distance <= 20:
                print("     Alert range")
            else:
                print("     Safe distance")
            time.sleep(1.5)
    
    def auto_demonstration(self):
        """Run automatic demonstration sequence"""
        print("\n Auto-Demonstration Sequence")
        
        sequences = [
            ("System Boot", 2),
            ("Idle breathing sounds", 3),
            ("Object approaches from far", 2),
            ("Goblin becomes curious", 2),
            ("Object gets very close", 1),
            ("Goblin gets angry and warns", 3),
            ("Object retreats", 2),
            ("Goblin celebrates victory", 3),
            ("Returns to idle state", 2)
        ]
        
        for description, duration in sequences:
            print(f"   {description}")
            time.sleep(duration)
        
        print("   Demonstration complete!")
    
    def show_help(self):
        """Show detailed help information"""
        print("\n Goblin Audio System Help")
        print("-" * 40)
        print("This demo controls the P32 Animatronic Goblin's audio system.")
        print("\nThe goblin can:")
        print("   Make various growls, roars, and vocalizations")
        print("   Speak simple phrases in 'goblin language'")
        print("   Express emotions with different intensities")
        print("   React to proximity detection with appropriate sounds")
        print("\nFor best experience:")
        print("  1. Run the PC audio capture tool in another terminal")
        print("  2. Ensure your ESP32 is flashed with the latest firmware")
        print("  3. Use a serial port monitor to see ESP32 responses")

def main():
    if len(sys.argv) != 2:
        print("Usage: python goblin_audio_demo.py <serial_port>")
        print("Example: python goblin_audio_demo.py COM3")
        sys.exit(1)
    
    serial_port = sys.argv[1]
    demo = GoblinAudioDemo(serial_port)
    demo.run_demo()

if __name__ == "__main__":
    main()