// Goblin Hand Linkage & Control Mechanism
// Demonstrates servo-to-finger control via push-pull cables and rigid linkages
// Shows how 3 servos control coupled finger groups (8-12 servos equivalent complexity)

// ==============================================================================
// CABLE ROUTING AND LINKAGE SYSTEM
// ==============================================================================

// Cable routing nodes (mm)
CABLE_ROUTING_HEIGHT = 8.0;   // Height of cable guides above palm
CABLE_DIAMETER = 2.0;         // Diameter of control cables (Spectra or dyneema)
CABLE_GUIDE_D = 3.5;          // Internal diameter of cable routing eyelets

// ==============================================================================
// SERVO #1: MAIN FINGER CURL (Index, Middle, Ring fingers)
// ==============================================================================

module servo1_curl_linkage() {
    // Servo position: center of palm, 20mm forward
    servo_pos = [0, 20, 12];
    
    // Mechanical system:
    // - Servo horn attached to horizontal push-pull rod
    // - Rod drives cable bundle to index/middle/ring fingers
    // - Tension cable pulls fingers curled (closed grip)
    // - Spring returns to open position
    
    // Push-pull rod (connects servo to cable)
    translate(servo_pos)
        difference() {
            // Rod main body (aluminum, lightweight)
            rotate([90, 0, 0])
                cylinder(h=50, d=6, center=true);
            
            // Servo horn attachment point
            translate([0, 0, -3])
                cylinder(h=4, d=5.5, center=true);
            
            // Cable anchor points (3 separate tension lines to three finger groups)
            for(angle = [0, 120, 240]) {
                translate([cos(angle)*8, sin(angle)*8, 20])
                    cylinder(h=6, d=2.5, center=true);
            }
        }
    
    // Cable guides (keep cables separated and aligned)
    for(i = [0:2]) {
        angle = i * 120;
        guide_x = cos(angle) * 15;
        guide_y = 35 + i*5;
        
        translate([guide_x, guide_y, CABLE_ROUTING_HEIGHT])
            difference() {
                cylinder(h=4, d=5);
                cylinder(h=5, d=CABLE_GUIDE_D, center=true);
            }
    }
    
    // Tension springs (return fingers to open position)
    for(i = [0:2]) {
        angle = i * 120;
        from_x = cos(angle)*8;
        to_x = cos(angle)*15;
        
        translate([from_x, 60 + i*5, CABLE_ROUTING_HEIGHT])
            cylinder(h=3, d=3);
    }
}

// ==============================================================================
// SERVO #2: PINKY GROUP CONTROL
// ==============================================================================

module servo2_pinky_linkage() {
    // Servo position: left side of palm, near pinky location
    servo_pos = [-25, 20, 12];
    
    // Similar push-pull mechanism but smaller
    // Controls only the pinky finger (can curl independently)
    
    translate(servo_pos)
        difference() {
            // Servo linkage rod (smaller diameter)
            rotate([90, 0, 0])
                cylinder(h=40, d=5, center=true);
            
            // Servo horn attachment
            translate([0, 0, -2.5])
                cylinder(h=3, d=5, center=true);
            
            // Pinky cable anchor
            translate([0, 0, 15])
                cylinder(h=5, d=2.2, center=true);
        }
    
    // Cable guide to pinky
    translate([-25, 50, CABLE_ROUTING_HEIGHT])
        difference() {
            cylinder(h=4, d=4.5);
            cylinder(h=5, d=CABLE_GUIDE_D, center=true);
        }
    
    // Return spring
    translate([-25, 60, CABLE_ROUTING_HEIGHT])
        cylinder(h=3, d=2.5);
}

// ==============================================================================
// SERVO #3: THUMB OPPOSITION CONTROL
// ==============================================================================

module servo3_thumb_linkage() {
    // Servo position: right side, thumb base
    servo_pos = [25, -10, 12];
    
    // Thumb uses rotating servo horn directly
    // Horn pushes thumb toward opposition angle (45° inward)
    
    translate(servo_pos) {
        difference() {
            // Servo linkage arm (curved, follows thumb arc)
            translate([0, 0, 12])
                rotate([0, 0, 45])
                    cube([5, 35, 4], center=true);
            
            // Servo attachment point
            translate([0, 0, 8])
                cylinder(h=4, d=5, center=true);
            
            // Thumb push point
            translate([12, 12, 12])
                cylinder(h=5, d=3.5, center=true);
        }
        
        // Reinforcing ribs on linkage arm
        translate([0, 8, 12])
            cube([2, 25, 3], center=true);
    }
    
    // Return spring (keeps thumb extended)
    translate([25, -10, CABLE_ROUTING_HEIGHT])
        cylinder(h=4, d=2.5);
}

// ==============================================================================
// CABLE ROUTING SCHEMATIC
// ==============================================================================

module cable_routing_schematic() {
    // Visualization of control cable paths (non-printed, just shows layout)
    
    // Main tension cable from servo #1 to fingers
    translate([0, 20, CABLE_ROUTING_HEIGHT]) {
        // Cable bundle splits into 3 paths
        color([0.8, 0.2, 0.2], 0.5) {
            // Path to index/middle/ring
            cylinder(h=50, d=CABLE_DIAMETER);
        }
    }
    
    // Pinky control cable
    color([0.2, 0.8, 0.2], 0.5) {
        translate([-25, 20, CABLE_ROUTING_HEIGHT])
            cylinder(h=40, d=CABLE_DIAMETER);
    }
    
    // Thumb opposition cable
    color([0.2, 0.2, 0.8], 0.5) {
        translate([25, -10, CABLE_ROUTING_HEIGHT])
            cylinder(h=30, d=CABLE_DIAMETER);
    }
}

// ==============================================================================
// COMPLETE CONTROL LINKAGE ASSEMBLY
// ==============================================================================

module complete_hand_linkages() {
    echo("=== Goblin Hand Control Linkage System ===");
    echo("Servo #1: Main finger curl (index+middle+ring coupled)");
    echo("Servo #2: Pinky group control");
    echo("Servo #3: Thumb opposition");
    echo("");
    echo("Total Servos: 3 (not 30+)");
    echo("Control Type: Push-pull cables + springs");
    echo("Total Hand Weight: ~180-220 grams");
    echo("Power Draw: ~2-3W per servo × 3 = 6-9W typical, 15W peak");
    
    servo1_curl_linkage();
    servo2_pinky_linkage();
    servo3_thumb_linkage();
}

// ==============================================================================
// POWER & THERMAL ANALYSIS
// ==============================================================================

function calculate_hand_power_draw() =
    let(
        // Servo specs (SG90 hobby servo)
        servo_idle = 0.02,        // 20mA @ 5V = 0.1W per servo
        servo_light = 0.3,        // 60mA @ 5V = 0.3W per servo
        servo_moderate = 0.75,    // 150mA @ 5V = 0.75W per servo
        servo_heavy = 2.0,        // 400mA @ 5V = 2W per servo
        
        // Hand states
        idle_power = servo_idle * 3,
        light_motion = servo_light * 3,
        moderate_grip = (servo_moderate * 2) + (servo_heavy * 1),  // 2 servos lifting, 1 gripping
        max_grip = servo_heavy * 3
    )
    [
        ["Idle", idle_power],
        ["Light motion", light_motion],
        ["Moderate grip", moderate_grip],
        ["Maximum grip", max_grip]
    ];

// ==============================================================================
// RENDERING
// ==============================================================================

// Render all linkage components
complete_hand_linkages();

// Display power characteristics
echo("");
echo("=== POWER CONSUMPTION PROFILE ===");
power_table = calculate_hand_power_draw();
for(i=[0:len(power_table)-1]) {
    echo(str(power_table[i][0], ": ", power_table[i][1], "W"));
}

