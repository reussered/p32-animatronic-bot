// Goblin Hand Wrist Assembly
// Connects hand to arm, routes power/control, holds servo controller ESP32

// ==============================================================================
// DIMENSIONS
// ==============================================================================

// Wrist connector specs
WRIST_OD = 45.0;            // Outer diameter connection to arm
WRIST_ID = 28.0;            // Inner diameter (cable pass-through)
WRIST_LENGTH = 25.0;        // Depth of connector

// Servo controller PCB (custom board with servo drivers)
CONTROLLER_LENGTH = 50.0;   // PCB length
CONTROLLER_WIDTH = 35.0;    // PCB width
CONTROLLER_HEIGHT = 8.0;    // PCB height (with components)

// ESP32-S3 placement
ESP32_LENGTH = 27.0;        // Length of devkit
ESP32_WIDTH = 14.0;         // Width of devkit
ESP32_HEIGHT = 4.0;         // Height

// Power connector (5V input from arm ESP32)
POWER_CONNECTOR_D = 5.5;    // XT30 connector inner diameter
SIGNAL_CONNECTOR_D = 3.5;   // I2C/signal connector

// ==============================================================================
// WRIST CONNECTOR STRUCTURE
// ==============================================================================

module wrist_connector_housing(od, id, length) {
    difference() {
        // Outer cylindrical housing
        cylinder(h=length, d=od);
        
        // Inner cavity for cable pass-through
        cylinder(h=length+2, d=id, center=true);
        
        // Mounting holes (M3) - attach hand to arm
        for(angle = [0, 90, 180, 270]) {
            x = cos(angle) * (od*0.32);
            y = sin(angle) * (od*0.32);
            translate([x, y, length*0.5])
                rotate([0, 90, 0])
                    cylinder(h=od*0.15, d=3.5);
        }
        
        // Cable strain relief channels (prevent sharp bends)
        for(i = [0, 120, 240]) {
            angle = i;
            translate([cos(angle)*(od*0.25), sin(angle)*(od*0.25), 0])
                translate([0, 0, length*0.3])
                    rotate([45, 0, angle])
                        cube([3, 3, length*0.4], center=true);
        }
    }
    
    // Internal stiffening ribs (prevent collapse)
    for(angle = [0, 120, 240]) {
        translate([cos(angle)*(id*0.4), sin(angle)*(id*0.4), 0])
            cube([2, od*0.1, length], center=true);
    }
}

// ==============================================================================
// SERVO CONTROLLER PCB MOUNTING
// ==============================================================================

module servo_controller_bracket() {
    // Mounts inside wrist connector
    // Holds: custom servo driver PCB + ESP32-S3 + power connectors
    
    difference() {
        // Main bracket body
        translate([0, 0, 0])
            cube([CONTROLLER_WIDTH + 6, CONTROLLER_LENGTH + 6, 8], center=true);
        
        // PCB cavity
        translate([0, 0, 1])
            cube([CONTROLLER_WIDTH + 0.5, CONTROLLER_LENGTH + 0.5, CONTROLLER_HEIGHT + 1], center=true);
    }
    
    // PCB support posts (M2.5 threaded inserts)
    for(x = [-CONTROLLER_WIDTH*0.35, CONTROLLER_WIDTH*0.35]) {
        for(y = [-CONTROLLER_LENGTH*0.35, CONTROLLER_LENGTH*0.35]) {
            translate([x, y, -2])
                cylinder(h=3, d=3.5);
        }
    }
    
    // Heat dissipation: mounting for aluminum heatsink under power MOSFETs
    translate([0, CONTROLLER_LENGTH*0.35, -3])
        difference() {
            cube([CONTROLLER_WIDTH, 12, 3], center=true);
            cylinder(h=4, d=3.5);  // Screw hole
        }
}

// ==============================================================================
// POWER CONNECTOR MOUNT
// ==============================================================================

module power_connector_housing() {
    // Mounts XT30 connector (incoming 5V from arm)
    // Feeds: servo power distribution + ESP32 power
    
    difference() {
        // Housing block
        cube([25, 20, 15], center=true);
        
        // XT30 connector cavity
        translate([0, 0, 2])
            cylinder(h=8, d=POWER_CONNECTOR_D);
        
        // Backshell mounting hole
        translate([0, 0, -5])
            cylinder(h=3, d=6.0);
        
        // Cable routing channel
        translate([0, -8, 0])
            cube([5, 12, 12], center=true);
    }
    
    // Strain relief boot mounting points
    for(x = [-8, 8]) {
        translate([x, 0, 4])
            cylinder(h=3, d=2.5);
    }
}

// ==============================================================================
// I2C SIGNAL CONNECTOR
// ==============================================================================

module i2c_signal_connector_mount() {
    // Small connector for I2C communication with arm ESP32
    // Carries: SDA, SCL, GND from hand controller back to arm
    
    difference() {
        // Housing
        cube([15, 12, 10], center=true);
        
        // Connector cavity (small 4-pin)
        translate([0, 0, 2])
            cube([8, 8, 6], center=true);
        
        // Cable routing
        translate([0, -5, 0])
            cylinder(h=8, d=2.5, center=true);
    }
}

// ==============================================================================
// SERVO DISTRIBUTION BOARD
// ==============================================================================

module servo_pwm_distribution() {
    // Distributes 3 PWM signals from ESP32 to 3 servo motors
    // Also provides servo power filtering
    
    difference() {
        // Main board area
        cube([CONTROLLER_WIDTH, 25, 3], center=true);
        
        // Component mounting holes
        // 3× servo signal headers
        for(i = [0:2]) {
            y = -8 + i*8;
            translate([-CONTROLLER_WIDTH*0.3, y, 2])
                cylinder(h=4, d=2.5);
        }
        
        // Power distribution mosfet mounting
        translate([CONTROLLER_WIDTH*0.3, -8, 2])
            cylinder(h=4, d=4.5);
    }
    
    // Servo connectors (standard 3-pin headers)
    for(i = [0:2]) {
        y = -8 + i*8;
        translate([-CONTROLLER_WIDTH*0.3, y, 2])
            translate([0, 0, 1.5])
                cube([3, 2.54*3, 2], center=true);
    }
    
    // Power filtering capacitor mounts
    for(x = [CONTROLLER_WIDTH*0.1, CONTROLLER_WIDTH*0.4]) {
        translate([x, 8, 2])
            cylinder(h=3, d=6);
    }
}

// ==============================================================================
// COMPLETE WRIST & CONTROLLER ASSEMBLY
// ==============================================================================

module goblin_hand_wrist_assembly() {
    echo("=== Goblin Hand Wrist & Controller Assembly ===");
    echo("");
    echo("Components:");
    echo("1. Wrist connector (aluminum or PEEK)");
    echo("2. Servo controller PCB (custom PWM board)");
    echo("3. ESP32-S3 microcontroller");
    echo("4. Power input (XT30, from arm ESP32)");
    echo("5. I2C communication link (back to arm)");
    echo("");
    echo("Architecture:");
    echo("- Arm ESP32 broadcasts mood/gesture state via SharedMemory");
    echo("- Hand ESP32 reads SharedMemory state locally (no wiring)");
    echo("- Hand ESP32 computes servo PWM signals from state");
    echo("- Servo motors receive PWM -> perform grip/gesture");
    echo("- Hand ESP32 writes thermal/load feedback to SharedMemory");
    echo("");
    echo("Power Flow:");
    echo("- 5V comes from arm via XT30 (no long power wires!)");
    echo("- Hand PCB filters and distributes to 3 servo motors");
    echo("- Per-hand power consumption: 2-15W (thermal max 25W)");
    echo("");
    
    // Render components
    color([0.5, 0.5, 0.5]) {
        wrist_connector_housing(WRIST_OD, WRIST_ID, WRIST_LENGTH);
    }
    
    color([0.3, 0.6, 0.9]) {
        translate([0, 0, 12])
            servo_controller_bracket();
    }
    
    color([0.2, 0.4, 0.6]) {
        translate([0, 0, 15])
            servo_pwm_distribution();
    }
    
    color([0.7, 0.2, 0.2]) {
        translate([0, -8, 20])
            power_connector_housing();
    }
    
    color([0.2, 0.7, 0.2]) {
        translate([0, 8, 20])
            i2c_signal_connector_mount();
    }
}

// ==============================================================================
// WIRING DIAGRAM (Non-rendered, for documentation)
// ==============================================================================

/*
WRIST ELECTRICAL INTERFACE:

ARM ESP32 (Main Brain)
    │
    ├─ SharedMemory broadcast (ESP-NOW wireless)
    │   └─> Hand ESP32 receives mood/gesture state
    │
    └─ 5V power to hand (XT30 connector)
       │
       └─ WRIST CONNECTOR (Physical Interface)
          │
          ├─ 5V power input (XT30 Red/Black)
          ├─ I2C signal return (SDA, SCL, GND)
          │
          └─ HAND WRIST ASSEMBLY
             │
             ├─ Servo Controller PCB
             │  ├─ ESP32-S3 (hand local control)
             │  ├─ 3× PWM driver outputs
             │  ├─ Power filtering (10µF + 100µF caps)
             │  └─ Thermal sensor (DS18B20)
             │
             └─ Servo Power Distribution
                ├─ MOSFET gate drivers
                ├─ Current limiting resistors
                ├─ Servo #1 (main finger curl) → PWM ch0
                ├─ Servo #2 (pinky) → PWM ch1
                └─ Servo #3 (thumb opposition) → PWM ch2

CONTROL SIGNAL FLOW:
Arm ESP32 writes to SharedMemory:
  {hand_state: "gripping", grip_strength: 80%, target_temp: +15C}

Hand ESP32 reads locally (FAST, no wireless overhead):
  {hand_state, grip_strength} → compute servo angles

Hand servos move → gripping action

Hand ESP32 measures:
  - Servo current per motor
  - Palm temperature
  - Load force estimate
  
Hand ESP32 writes back to SharedMemory:
  {hand_temp: 32C, hand_current: 450mA, hand_load: 4.2N}

Arm ESP32 checks: if temp > 35C, reduce next animation

NO WIRES between arm and hand except 5V power!
ALL COORDINATION via ESP-NOW wireless mesh.
*/

// ==============================================================================
// WEIGHT & THERMAL ESTIMATES
// ==============================================================================

function estimate_wrist_assembly_weight() =
    let(
        wrist_connector = 15.0,      // Aluminum tube + fittings
        controller_pcb = 12.0,       // PCB + components
        esp32 = 20.0,                // ESP32-S3 devkit
        power_distribution = 8.0,    // Capacitors, MOSFETs, resistors
        connectors = 6.0,            // XT30, I2C connectors
        mounting_hardware = 5.0,     // Screws, heatsink, standoffs
        
        total = wrist_connector + controller_pcb + esp32 + 
                power_distribution + connectors + mounting_hardware
    )
    total;

function estimate_hand_thermal_performance() =
    let(
        // 3 servo motors, each generating 0-2W heat
        servo_max = 2.0 * 3,         // 6W maximum from servos
        pcb_dissipation = 1.0,       // 1W from controller
        linkage_friction = 0.5,      // 0.5W from cable/bearing friction
        
        // Thermal resistance (K/W) in confined hand space
        theta_ja = 4.5,              // Poor cooling (enclosed plastic)
        
        // Operating points
        idle_power = 0.1,
        idle_temp_rise = idle_power * theta_ja,
        
        moderate_power = 3.0,        // Typical gripping
        moderate_temp_rise = moderate_power * theta_ja,
        
        peak_power = 7.0,            // Brief maximum grip
        peak_temp_rise = peak_power * theta_ja
    )
    [
        ["Idle", idle_power, idle_temp_rise],
        ["Moderate grip", moderate_power, moderate_temp_rise],
        ["Peak grip", peak_power, peak_temp_rise]
    ];

// ==============================================================================
// RENDERING
// ==============================================================================

goblin_hand_wrist_assembly();

echo("");
echo(str("Wrist Assembly Weight: ", estimate_wrist_assembly_weight(), " grams"));
echo("");
echo("=== THERMAL PERFORMANCE ===");
thermal_table = estimate_hand_thermal_performance();
for(i=[0:len(thermal_table)-1]) {
    row = thermal_table[i];
    echo(str(row[0], ": ", row[1], "W → +", row[2], "°C rise"));
}

