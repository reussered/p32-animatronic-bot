#!/usr/bin/env python3
"""
Motor Identification & Vendor Management System
P32 Animatronic Bot Project

This script helps identify unknown motors and manage vendor information.
"""

import json
import os
from datetime import datetime
from typing import Dict, List, Optional

class MotorIdentificationHelper:
    """Helps identify unknown motors and track them"""
    
    def __init__(self):
        self.unknown_motors = []
        self.vendor_list = []
    
    def add_unknown_motor(self, motor_id: str, description: str = "", photo_notes: str = ""):
        """Log an unknown motor for investigation"""
        motor = {
            "motor_id": motor_id,
            "date_found": datetime.now().isoformat(),
            "description": description,
            "photo_notes": photo_notes,
            "identified": False,
            "motor_type": None,  # "servo", "dc", "stepper", "brushless", etc.
            "voltage": None,
            "current": None,
            "torque": None,
            "speed": None,
            "manufacturer": None,
            "model_number": None,
            "sku": None,
            "estimated_cost": None,
            "vendors_available": [],
            "notes": ""
        }
        self.unknown_motors.append(motor)
        return motor
    
    def get_motor_characteristics(self, motor_type: str) -> Dict:
        """Get typical characteristics for common motor types"""
        motor_types = {
            "servo_micro": {
                "name": "Micro Servo (SG90 type)",
                "typical_voltage": "5V",
                "typical_torque": "1.5-2 kg-cm",
                "typical_speed": "0.1-0.12 sec/60",
                "connectors": "3-pin (signal, power, GND)",
                "shaft_type": "Splined",
                "typical_cost": 3.00,
                "common_models": ["SG90", "TowerPro SG92R", "MG90S"],
                "identification_tips": [
                    "Small plastic case, about 23x12x29mm",
                    "Rotating horn/arm on top",
                    "3 wires: red (5V), black (GND), orange/yellow (signal)",
                    "Often white or black plastic"
                ]
            },
            "servo_standard": {
                "name": "Standard Servo (MG996R type)",
                "typical_voltage": "4.8-6V",
                "typical_torque": "9-11 kg-cm",
                "typical_speed": "0.2 sec/60",
                "connectors": "3-pin (signal, power, GND)",
                "shaft_type": "Splined",
                "typical_cost": 8.00,
                "common_models": ["MG996R", "SG5010", "TowerPro MG996R"],
                "identification_tips": [
                    "Larger than micro servo, about 40x20x38mm",
                    "Metal case (usually aluminum)",
                    "Rotating horn/arm on top",
                    "3 wires standard"
                ]
            },
            "servo_large": {
                "name": "Large Servo (DS3218 type)",
                "typical_voltage": "6-7.4V",
                "typical_torque": "20+ kg-cm",
                "typical_speed": "0.16 sec/60",
                "connectors": "3-pin (signal, power, GND)",
                "shaft_type": "Splined or D-shaft",
                "typical_cost": 25.00,
                "common_models": ["DS3218", "LX-225MG", "Savox SV-1272SG"],
                "identification_tips": [
                    "Large metal case, 40x40x40mm+",
                    "Heavy (200+ grams)",
                    "Metal gears visible through case",
                    "Often has multiple mounting holes"
                ]
            },
            "dc_motor": {
                "name": "DC Motor",
                "typical_voltage": "6-12V",
                "typical_torque": "Varies widely",
                "typical_speed": "1000-3000 RPM typically",
                "connectors": "2-pin (+ and -)",
                "shaft_type": "Round shaft",
                "typical_cost": 5.00,
                "common_models": ["RS-385", "N20", "FF-180"],
                "identification_tips": [
                    "Cylindrical metal case",
                    "Only 2 wires",
                    "Simple commutator/brushes inside",
                    "Spins freely when unpowered, with friction"
                ]
            },
            "stepper_motor": {
                "name": "Stepper Motor",
                "typical_voltage": "12-24V",
                "typical_torque": "Varies",
                "typical_speed": "Can step precisely",
                "connectors": "4-6 wires",
                "shaft_type": "Round shaft, usually flat",
                "typical_cost": 10.00,
                "common_models": ["NEMA17", "NEMA23", "28BYJ-48"],
                "identification_tips": [
                    "Square case (NEMA standard)",
                    "4-6 wires coming out",
                    "Coils visible (copper wire)",
                    "Does NOT spin freely - has detent positions"
                ]
            },
            "brushless_motor": {
                "name": "Brushless Motor (BLDC)",
                "typical_voltage": "7.4-11.1V",
                "typical_torque": "Varies",
                "typical_speed": "Fast, 1000+ RPM",
                "connectors": "3-pin motor + 4-pin ESC",
                "shaft_type": "Round shaft",
                "typical_cost": 12.00,
                "common_models": ["A2212", "T-motor series"],
                "identification_tips": [
                    "Bell-shaped or cylindrical case",
                    "3 wires from motor",
                    "Requires ESC (Electronic Speed Controller)",
                    "Very light for power output"
                ]
            },
            "linear_actuator": {
                "name": "Linear Actuator",
                "typical_voltage": "12V or 24V",
                "typical_torque": "Not applicable - linear",
                "typical_speed": "Inches per second",
                "connectors": "2-pin (power)",
                "shaft_type": "Linear rod",
                "typical_cost": 30.00,
                "common_models": ["PQ12-49", "LD225"],
                "identification_tips": [
                    "Long cylindrical case",
                    "Rod extends/retracts",
                    "Only 2 wires",
                    "Usually black or aluminum"
                ]
            }
        }
        
        return motor_types.get(motor_type, {})
    
    def print_motor_identification_guide(self):
        """Print a quick reference guide for motor identification"""
        print("\n" + "="*80)
        print("MOTOR IDENTIFICATION QUICK GUIDE")
        print("="*80)
        
        motor_types = {
            "servo_micro": self.get_motor_characteristics("servo_micro"),
            "servo_standard": self.get_motor_characteristics("servo_standard"),
            "servo_large": self.get_motor_characteristics("servo_large"),
            "dc_motor": self.get_motor_characteristics("dc_motor"),
            "stepper_motor": self.get_motor_characteristics("stepper_motor"),
            "brushless_motor": self.get_motor_characteristics("brushless_motor"),
            "linear_actuator": self.get_motor_characteristics("linear_actuator")
        }
        
        for motor_type, info in motor_types.items():
            print(f"\n {info['name'].upper()}")
            print("-" * 80)
            print(f"  Voltage: {info.get('typical_voltage', 'N/A')}")
            print(f"  Torque: {info.get('typical_torque', 'N/A')}")
            print(f"  Speed: {info.get('typical_speed', 'N/A')}")
            print(f"  Connectors: {info.get('connectors', 'N/A')}")
            print(f"  Shaft Type: {info.get('shaft_type', 'N/A')}")
            print(f"  Typical Cost: ${info.get('typical_cost', 0):.2f}")
            print(f"  Common Models: {', '.join(info.get('common_models', []))}")
            print(f"  How to Identify:")
            for tip in info.get('identification_tips', []):
                print(f"     {tip}")
        
        print("\n" + "="*80)
    
    def create_vendor_template(self) -> List[Dict]:
        """Create a template of common electronics vendors"""
        vendors = [
            {
                "vendor_id": "vendor_aliexpress",
                "name": "AliExpress",
                "website": "https://www.aliexpress.com",
                "api_endpoint": None,
                "api_key": None,
                "shipping_cost_base": 2.50,
                "shipping_threshold_free": 50.00,
                "average_lead_time_days": 14,
                "reliability_score": 85.0,
                "accepted_payment_methods": ["credit_card", "paypal", "bitcoin"],
                "supported_components": ["displays", "motors", "drivers", "microcontrollers", "sensors", "connectors", "misc"],
                "notes": "Cheapest option, longest shipping, good for high-volume buys"
            },
            {
                "vendor_id": "vendor_amazon",
                "name": "Amazon",
                "website": "https://www.amazon.com",
                "api_endpoint": None,
                "api_key": None,
                "shipping_cost_base": 0.00,
                "shipping_threshold_free": 25.00,
                "average_lead_time_days": 2,
                "reliability_score": 95.0,
                "accepted_payment_methods": ["credit_card", "amazon_pay"],
                "supported_components": ["displays", "motors", "drivers", "microcontrollers", "sensors", "connectors", "misc"],
                "notes": "Fast shipping (Prime), higher prices, excellent returns"
            },
            {
                "vendor_id": "vendor_mouser",
                "name": "Mouser Electronics",
                "website": "https://www.mouser.com",
                "api_endpoint": "https://api.mouser.com/v1/",
                "api_key": None,
                "shipping_cost_base": 5.00,
                "shipping_threshold_free": 100.00,
                "average_lead_time_days": 3,
                "reliability_score": 98.0,
                "accepted_payment_methods": ["credit_card", "purchase_order"],
                "supported_components": ["drivers", "microcontrollers", "power", "connectors", "misc"],
                "notes": "Professional supplier, good for ICs and discrete components"
            },
            {
                "vendor_id": "vendor_digikey",
                "name": "Digi-Key",
                "website": "https://www.digikey.com",
                "api_endpoint": "https://api.digikey.com/",
                "api_key": None,
                "shipping_cost_base": 4.00,
                "shipping_threshold_free": 100.00,
                "average_lead_time_days": 2,
                "reliability_score": 97.0,
                "accepted_payment_methods": ["credit_card", "purchase_order"],
                "supported_components": ["drivers", "microcontrollers", "power", "connectors", "misc"],
                "notes": "Professional supplier, fast shipping, excellent documentation"
            },
            {
                "vendor_id": "vendor_ebay",
                "name": "eBay",
                "website": "https://www.ebay.com",
                "api_endpoint": None,
                "api_key": None,
                "shipping_cost_base": 3.00,
                "shipping_threshold_free": 75.00,
                "average_lead_time_days": 7,
                "reliability_score": 80.0,
                "accepted_payment_methods": ["credit_card", "paypal"],
                "supported_components": ["displays", "motors", "drivers", "microcontrollers", "sensors", "connectors", "misc"],
                "notes": "Marketplace, variable quality, good for used components"
            },
            {
                "vendor_id": "vendor_adafruit",
                "name": "Adafruit Industries",
                "website": "https://www.adafruit.com",
                "api_endpoint": None,
                "api_key": None,
                "shipping_cost_base": 6.00,
                "shipping_threshold_free": 75.00,
                "average_lead_time_days": 2,
                "reliability_score": 99.0,
                "accepted_payment_methods": ["credit_card"],
                "supported_components": ["displays", "motors", "drivers", "sensors", "connectors", "misc"],
                "notes": "Hobbyist friendly, excellent documentation, premium pricing"
            },
            {
                "vendor_id": "vendor_sparkfun",
                "name": "SparkFun Electronics",
                "website": "https://www.sparkfun.com",
                "api_endpoint": None,
                "api_key": None,
                "shipping_cost_base": 7.00,
                "shipping_threshold_free": 100.00,
                "average_lead_time_days": 2,
                "reliability_score": 96.0,
                "accepted_payment_methods": ["credit_card"],
                "supported_components": ["displays", "motors", "drivers", "sensors", "connectors", "misc"],
                "notes": "Open source friendly, tutorials available, higher prices"
            },
            {
                "vendor_id": "vendor_banggood",
                "name": "Banggood",
                "website": "https://www.banggood.com",
                "api_endpoint": None,
                "api_key": None,
                "shipping_cost_base": 3.00,
                "shipping_threshold_free": 60.00,
                "average_lead_time_days": 15,
                "reliability_score": 78.0,
                "accepted_payment_methods": ["credit_card", "paypal"],
                "supported_components": ["displays", "motors", "drivers", "microcontrollers", "sensors", "connectors", "misc"],
                "notes": "China-based, competitive pricing, slower shipping than AliExpress"
            }
        ]
        
        return vendors
    
    def save_vendors_to_inventory(self):
        """Save vendor list to inventory.json"""
        try:
            with open('manufacturing/inventory.json', 'r') as f:
                inventory = json.load(f)
            
            vendors = self.create_vendor_template()
            inventory['inventory']['vendors'] = vendors
            
            with open('manufacturing/inventory.json', 'w') as f:
                json.dump(inventory, f, indent=2)
            
            print(f"\n Added {len(vendors)} vendors to inventory.json")
            return True
        except Exception as e:
            print(f" Error saving vendors: {e}")
            return False
    
    def print_vendor_summary(self):
        """Print vendor information"""
        vendors = self.create_vendor_template()
        
        print("\n" + "="*80)
        print("AVAILABLE VENDORS")
        print("="*80)
        
        for vendor in vendors:
            print(f"\n {vendor['name']}")
            print(f"   Website: {vendor['website']}")
            print(f"   Lead Time: {vendor['average_lead_time_days']} days")
            print(f"   Reliability: {vendor['reliability_score']}%")
            print(f"   Shipping: ${vendor['shipping_cost_base']:.2f} (free over ${vendor['shipping_threshold_free']:.2f})")
            print(f"   Components: {', '.join(vendor['supported_components'])}")
            print(f"   Notes: {vendor['notes']}")

def main():
    """Main execution"""
    print("\n" + "="*80)
    print("P32 ANIMATRONIC BOT - MOTOR ID & VENDOR SYSTEM")
    print("="*80)
    
    helper = MotorIdentificationHelper()
    
    # Print motor identification guide
    helper.print_motor_identification_guide()
    
    # Print vendor summary
    helper.print_vendor_summary()
    
    # Save vendors to inventory
    print("\n" + "="*80)
    print("SAVING TO INVENTORY...")
    print("="*80)
    helper.save_vendors_to_inventory()
    
    print("\n" + "="*80)
    print("NEXT STEPS:")
    print("="*80)
    print("""
1. Use the Motor Identification Guide above to examine your motors
2. For each unknown motor:
   - Look for: size, connector type, color, markings
   - Check identification tips for your suspected motor type
   - Note any part numbers or markings
   - Count the wires and their colors

3. Once identified, we can:
   - Add to inventory
   - Find vendors with best pricing
   - Set up auto-reorder rules

4. When ready, provide motor info in format:
   Motor #1: Micro Servo (SG90), 50 units, $3.20 each, AliExpress
   Motor #2: DC Motor (N20), 20 units, $2.50 each, Amazon
   etc.
    """)

if __name__ == "__main__":
    main()
