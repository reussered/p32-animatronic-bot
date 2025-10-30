#!/usr/bin/env python3
"""
ESP32 Pin Assignment Generator for Bus Types and Controllers
Reads ESP32 chip specification or controller specification and creates pin assignment arrays.

Usage:
    python esp32_pin_assigner.py <chip_spec.json> <output_header.h>
    python esp32_pin_assigner.py <controller_spec.json> <output_header.h>

For chip specifications: Generates arrays of all assignable pins by bus type
For controller specifications: Generates controller-specific pin arrays
"""

import json
import sys
from pathlib import Path
from typing import Dict, List, Set, Any

class ESP32PinAssigner:
    def __init__(self, chip_spec_path: str):
        self.chip_spec_path = Path(chip_spec_path)
        self.chip_data = None
        self.bus_assignments = {}

    def load_chip_spec(self) -> None:
        """Load ESP32 chip specification or controller specification from JSON file"""
        try:
            with open(self.chip_spec_path, 'r', encoding='ascii') as f:
                self.chip_data = json.load(f)

            # Check if this is a controller spec or chip spec
            if 'pin_assignments' in self.chip_data:
                self.spec_type = 'controller'
                print(f"Loaded controller specification: {self.chip_data.get('name', 'Unknown')}")
            elif 'exposed_pins' in self.chip_data:
                self.spec_type = 'chip'
                print(f"Loaded chip specification: {self.chip_data.get('board_name', 'Unknown')}")
            else:
                raise ValueError("Unknown specification format - missing 'pin_assignments' or 'exposed_pins'")

        except FileNotFoundError:
            print(f"Error: Specification file not found: {self.chip_spec_path}")
            sys.exit(1)
        except json.JSONDecodeError as e:
            print(f"Error: Invalid JSON in specification: {e}")
            sys.exit(1)

    def analyze_pin_functions(self) -> Dict[str, List[int]]:
        """Analyze all pins and categorize them by bus type capabilities"""
        if self.spec_type == 'controller':
            # For controller specs, return the predefined pin assignments
            pin_assignments = self.chip_data.get('pin_assignments', {})
            # Convert GPIO_NUM_x strings to integers for consistency
            result = {}
            for bus_type, pins in pin_assignments.items():
                result[bus_type] = [self._gpio_string_to_num(pin) for pin in pins]
            return result

        elif self.spec_type == 'chip':
            # For chip specs, analyze the pins as before
            if not self.chip_data or 'exposed_pins' not in self.chip_data:
                print("Error: No exposed pins found in chip specification")
                return {}

            # Initialize bus type collections
            bus_types = {
                'spi': set(),
                'i2c': set(),
                'i2s': set(),
                'adc': set(),
                'pwm': set(),  # LEDC PWM - any GPIO can be used
                'gpio': set()
            }

            for pin in self.chip_data['exposed_pins']:
                pin_name = pin['name']
                functions = pin.get('functions', [])
                can_be_shared = pin.get('can_be_shared', True)  # Default to True if not specified
                gpio_num = self._extract_gpio_number(pin_name)

                if gpio_num is None or not can_be_shared:
                    continue

                # Categorize by function capabilities
                for func in functions:
                    func_upper = func.upper()

                    # SPI functions
                    if 'SPI' in func_upper:
                        bus_types['spi'].add(gpio_num)

                    # I2C functions (ESP32-S3 doesn't have explicit I2C labels, but any GPIO can be I2C)
                    elif 'I2C' in func_upper or func_upper in ['SDA', 'SCL']:
                        bus_types['i2c'].add(gpio_num)

                    # I2S functions
                    elif 'I2S' in func_upper:
                        bus_types['i2s'].add(gpio_num)

                    # ADC functions
                    elif 'ADC' in func_upper:
                        bus_types['adc'].add(gpio_num)

                    # GPIO functions (most pins can be GPIO)
                    elif func_upper == 'GPIO':
                        bus_types['gpio'].add(gpio_num)

                # PWM: All GPIO pins can be used for PWM via LEDC
                if gpio_num is not None:
                    bus_types['pwm'].add(gpio_num)

            # For ESP32-S3, add standard bus capabilities that aren't explicitly labeled
            # SPI: Any GPIO can be used for SPI (3 controllers available: FSPI, HSPI, MSPI)
            # I2C: Any GPIO can be used for I2C (2 controllers available)
            # I2S: Any GPIO can be used for I2S (2 controllers available)
            all_gpio_pins = bus_types['gpio'].copy()
            bus_types['spi'] = all_gpio_pins.copy()
            bus_types['i2c'] = all_gpio_pins.copy()
            bus_types['i2s'] = all_gpio_pins.copy()

            return bus_types

        else:
            print(f"Error: Unknown specification type: {self.spec_type}")
            return {}

    def _gpio_string_to_num(self, gpio_string: str) -> int:
        """Convert GPIO_NUM_x string to integer (e.g., 'GPIO_NUM_4' -> 4)"""
        if gpio_string.startswith('GPIO_NUM_'):
            return int(gpio_string[9:])  # Remove 'GPIO_NUM_' prefix
        else:
            raise ValueError(f"Invalid GPIO string format: {gpio_string}")

    def create_bus_arrays(self, bus_types: Dict[str, Set[int]]) -> Dict[str, List[int]]:
        """Create ordered arrays for each bus type with multi-assignable pins at the end"""
        arrays = {}

        for bus_type, pins in bus_types.items():
            # Convert to sorted list
            pin_list = sorted(list(pins))

            # For pins that can be used in multiple buses, put them at the end
            # This requires analyzing which pins appear in multiple bus types
            multi_assignable = set()
            single_assignable = set()

            for pin in pin_list:
                # Count how many bus types this pin can be used for
                count = sum(1 for bus_pins in bus_types.values() if pin in bus_pins)
                if count > 1:
                    multi_assignable.add(pin)
                else:
                    single_assignable.add(pin)

            # Create ordered array: single-purpose pins first, then multi-purpose
            ordered_pins = sorted(list(single_assignable)) + sorted(list(multi_assignable))
            arrays[f"{bus_type}_assignable"] = ordered_pins

        return arrays

    def generate_output(self, arrays: Dict[str, List[int]]) -> str:
        """Generate C-style array declarations with dynamic pin assignment tracking"""
        output = []
        output.append("// ESP32 Pin Assignment Arrays")
        output.append("// Generated for: " + self.chip_data.get('board_name', 'Unknown'))
        output.append("// Chip: " + self.chip_data.get('chip_family', 'Unknown'))
        output.append("")

        # Calculate total number of assignable pins across all bus types
        all_pins = set()
        for pins in arrays.values():
            all_pins.update(pins)
        total_pins = len(all_pins)

        # Generate assigned_pins array to track pin assignments at runtime
        output.append("// Runtime pin assignment tracking")
        output.append(f"static int assigned_pins[{total_pins}];")
        output.append(f"static size_t assigned_pins_count = 0;")
        output.append("")

        # Generate assignable pin arrays
        for array_name, pins in arrays.items():
            if not pins:
                continue

            # Convert to C-style array
            pin_strs = [f"GPIO_NUM_{pin}" for pin in pins]
            array_content = ", ".join(pin_strs)

            output.append(f"const int {array_name}[] = {{")
            output.append(f"    {array_content}")
            output.append("};")
            output.append(f"const size_t {array_name}_count = sizeof({array_name}) / sizeof({array_name}[0]);")
            output.append("")

        # Generate pin assignment helper functions
        output.append("// Pin assignment helper function")
        output.append("static int assign_pin(const int* assignable_array, size_t array_size) {")
        output.append("    for (size_t i = 0; i < array_size; i++) {")
        output.append("        int pin = assignable_array[i];")
        output.append("        // Check if pin is already assigned")
        output.append("        bool already_assigned = false;")
        output.append("        for (size_t j = 0; j < assigned_pins_count; j++) {")
        output.append("            if (assigned_pins[j] == pin) {")
        output.append("                already_assigned = true;")
        output.append("                break;")
        output.append("            }")
        output.append("        }")
        output.append("        if (!already_assigned && assigned_pins_count < sizeof(assigned_pins)/sizeof(assigned_pins[0])) {")
        output.append("            assigned_pins[assigned_pins_count++] = pin;")
        output.append("            return pin;")
        output.append("        }")
        output.append("    }")
        output.append("    return -1; // No available pins")
        output.append("}")
        output.append("")

        return "\n".join(output)

    def save_output(self, output: str, output_path: str) -> None:
        """Save generated output to file"""
        output_file = Path(output_path)
        output_file.parent.mkdir(parents=True, exist_ok=True)

        with open(output_file, 'w') as f:
            f.write(output)

        print(f"Generated pin assignment arrays saved to: {output_file}")

def main():
    if len(sys.argv) != 3:
        print("Usage: python esp32_pin_assigner.py <chip_spec.json> <output_file>")
        print("Example: python esp32_pin_assigner.py config/components/hardware/esp32_s3_devkit_chip.json include/esp32_pin_assignments.h")
        sys.exit(1)

    chip_spec_path = sys.argv[1]
    output_path = sys.argv[2]

    assigner = ESP32PinAssigner(chip_spec_path)
    assigner.load_chip_spec()

    if assigner.spec_type == 'controller':
        # For controller specs, pin assignments are already properly formatted
        arrays = assigner.analyze_pin_functions()
    else:
        # For chip specs, analyze and create bus arrays
        bus_types = assigner.analyze_pin_functions()
        arrays = assigner.create_bus_arrays(bus_types)

    output = assigner.generate_output(arrays)
    assigner.save_output(output, output_path)

    # Print summary
    print("\nPin Assignment Summary:")
    for array_name, pins in arrays.items():
        print(f"  {array_name}: {len(pins)} pins")

if __name__ == "__main__":
    main()