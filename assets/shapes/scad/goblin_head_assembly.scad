// Goblin Head Assembly - Complete Integration with Weight Analysis
// Shows realistic goblin skull with all P32 components mounted
// Demonstrates how electronics integrate with anatomical design
// Includes automated weight calculations and motor load analysis

include <goblin_skull_complete.scad>

// =============================================================================
// WEIGHT ANALYSIS AND MOTOR CALCULATIONS 
// =============================================================================

// Material properties for weight calculations
pla_density = 1.25; // g/cm³

// Component weight database (grams) - based on actual measurements and estimates
component_weights = [
    // 3D Printed Components (PLA, 20% infill unless noted)
    ["skull_base", 57.5],           // Main skull structure
    ["mounting_rings", 31.25],      // Component mount rings (100% infill)
    ["eye_shells", 15.0],           // Decorative eye shells (pair)
    ["nose_shell", 8.0],            // Decorative nose shell
    ["mouth_shell", 12.0],          // Decorative mouth shell
    // Electronics (manufacturer specifications)
    ["gc9a01_display", 8.0],        // Single display unit
    ["hc_sr04_sensor", 9.0],        // Ultrasonic sensor
    ["esp32_s3_devkit", 25.0],      // Main controller
    ["wire_harness", 30.0],         // Complete wiring with connectors
    // Hardware (standard fasteners)
    ["m3_screws", 15.0],            // All M3 screws in assembly
    ["m3_nuts", 8.0],               // All M3 nuts
    ["washers", 3.0],               // All washers
    ["threaded_inserts", 12.0],     // All threaded inserts
    // Motors for complete system
    ["nema17_motor", 280.0],        // Single NEMA 17 stepper motor
    ["motor_mount", 35.0],          // Motor mounting bracket
    ["gear_reduction", 45.0]        // Optional gear reduction unit
];

// Weight calculation functions
function get_component_weight(name) = 
    let(matches = [for(i=[0:len(component_weights)-1]) 
                  if(component_weights[i][0] == name) component_weights[i][1]])
    len(matches) > 0 ? matches[0] : 0;

function calculate_head_weight() = 
    get_component_weight("skull_base") +
    get_component_weight("mounting_rings") +
    get_component_weight("eye_shells") +
    get_component_weight("nose_shell") + 
    get_component_weight("mouth_shell") +
    (get_component_weight("gc9a01_display") * 3) + // 3 displays
    get_component_weight("hc_sr04_sensor") +
    get_component_weight("esp32_s3_devkit") +
    get_component_weight("wire_harness") +
    get_component_weight("m3_screws") +
    get_component_weight("m3_nuts") +
    get_component_weight("washers") +
    get_component_weight("threaded_inserts");

function calculate_system_weight(motors=2) = 
    calculate_head_weight() +
    (get_component_weight("nema17_motor") * motors) +
    (get_component_weight("motor_mount") * motors);

// Center of mass calculation (mm from nose_center origin)
function calculate_center_of_mass() = 
    let(// Component positions and weights
        skull_weight = 123.75,      // All 3D printed components
        skull_com = [0, -5, 10],    // Skull CoM relative to nose
        
        elec_weight = 88.0,         // All electronics  
        elec_com = [0, -15, -20],   // Electronics CoM (controller in back)
        
        hardware_weight = 38.0,     // All fasteners
        hardware_com = [0, -8, -5], // Hardware distributed through skull
        
        total_weight = skull_weight + elec_weight + hardware_weight)
    // Weighted average of component centers
    [(skull_com[0]*skull_weight + elec_com[0]*elec_weight + hardware_com[0]*hardware_weight) / total_weight,
     (skull_com[1]*skull_weight + elec_com[1]*elec_weight + hardware_com[1]*hardware_weight) / total_weight,
     (skull_com[2]*skull_weight + elec_com[2]*elec_weight + hardware_com[2]*hardware_weight) / total_weight];

// Motor torque calculations
function calculate_static_torque(weight_grams, moment_arm_mm) =
    let(weight_kg = weight_grams / 1000,
        moment_arm_m = moment_arm_mm / 1000,
        force_n = weight_kg * 9.81)           // Gravitational force
    force_n * moment_arm_m;                   // Torque in N⋅m

function calculate_dynamic_torque(weight_grams, moment_arm_mm, angular_accel_rad_s2) =
    let(weight_kg = weight_grams / 1000,
        moment_arm_m = moment_arm_mm / 1000,
        moment_of_inertia = weight_kg * moment_arm_m * moment_arm_m * 1.3) // Shape factor
    moment_of_inertia * angular_accel_rad_s2; // Torque in N⋅m

function calculate_required_motor_torque(weight_grams, moment_arm_mm, safety_factor=2.5) =
    let(static_torque = calculate_static_torque(weight_grams, moment_arm_mm),
        dynamic_torque = calculate_dynamic_torque(weight_grams, moment_arm_mm, 12.57), // 90° in 0.5s
        max_torque = max(static_torque, dynamic_torque))
    max_torque * safety_factor;

// Convert N⋅m to oz⋅in for motor specifications
function nm_to_ozin(torque_nm) = torque_nm * 141.6;

// Display calculations in console
head_weight = calculate_head_weight();
system_weight = calculate_system_weight(2);
center_of_mass = calculate_center_of_mass();
moment_arm = 59.1; // mm from neck joint to CoM (measured)
required_torque_nm = calculate_required_motor_torque(head_weight, moment_arm);
required_torque_ozin = nm_to_ozin(required_torque_nm);

echo("=== GOBLIN HEAD ASSEMBLY WEIGHT ANALYSIS ===");
echo(str("Head Assembly Weight: ", head_weight, " grams (", round(head_weight*0.0353*100)/100, " oz)"));
echo(str("Complete System Weight: ", system_weight, " grams (", round(system_weight*0.002205*100)/100, " lbs)"));
echo(str("Center of Mass: [", center_of_mass[0], ", ", center_of_mass[1], ", ", center_of_mass[2], "] mm from nose"));
echo(str("Moment Arm Length: ", moment_arm, " mm"));
echo(str("Required Motor Torque: ", round(required_torque_nm*1000)/1000, " N⋅m (", round(required_torque_ozin*10)/10, " oz⋅in)"));
echo(str("Recommended Motor: NEMA 17 with ≥", ceil(required_torque_ozin/10)*10, " oz⋅in holding torque"));
echo("============================================");
use <../bot_shells/goblin_eye_shells.scad>
use <../bot_shells/goblin_nose_shell.scad>
use <../bot_shells/goblin_mouth_shell.scad>
use <../basic_mounts/display_basic_mount.scad>
use <../basic_mounts/sensor_basic_mount.scad>

// =============================================================================
// ASSEMBLY MODES
// =============================================================================

assembly_mode = "complete"; // Options: "skull_only", "components_only", "complete", "exploded"
show_electronics = true;    // Show simplified electronics representations
show_wiring = false;        // Show wire routing paths

// =============================================================================
// ELECTRONIC COMPONENT REPRESENTATIONS
// =============================================================================

module gc9a01_display() {
    // Simplified GC9A01 240x240 circular display
    color([0.1, 0.1, 0.1]) {
        cylinder(h=2, r=12, center=true);
        translate([0, 0, 1]) cylinder(h=0.5, r=10, center=true);
    }
    // Active display area
    color([0.2, 0.6, 1.0, 0.8]) {
        translate([0, 0, 1.5]) cylinder(h=0.2, r=9, center=true);
    }
}

module hc_sr04_sensor() {
    // Simplified HC-SR04 ultrasonic sensor
    color([0.2, 0.4, 0.1]) {
        cube([20, 15, 8], center=true);
        // Ultrasonic transducers
        translate([-6, 0, 4]) cylinder(h=3, r=3);
        translate([6, 0, 4]) cylinder(h=3, r=3);
    }
}

module esp32_s3_devkit() {
    // Simplified ESP32-S3-DevKitC-1 representation
    color([0.1, 0.2, 0.6]) {
        cube([55, 28, 8], center=true);
        // USB connector
        translate([27, 0, 2]) cube([8, 12, 4], center=true);
    }
}

module wire_harness() {
    // Simplified wire routing representation
    if (show_wiring) {
        color([0.8, 0.1, 0.1, 0.6]) {
            // Power distribution
            translate([0, -40, -20]) cylinder(h=40, r=1);
            // SPI bus
            translate([-10, -40, -15]) cylinder(h=35, r=0.5);
            translate([0, -40, -15]) cylinder(h=35, r=0.5);
            translate([10, -40, -15]) cylinder(h=35, r=0.5);
        }
    }
}

// =============================================================================
// COMPONENT POSITIONING
// =============================================================================

module positioned_left_eye() {
    translate([p32_left_eye_x, p32_left_eye_y, p32_left_eye_z]) {
        rotate([0, 0, 0]) {
            if (assembly_mode != "skull_only") {
                goblin_eye_shell(is_left_eye = true);
                if (show_electronics) {
                    translate([0, 5, 0]) gc9a01_display();
                }
            }
        }
    }
}

module positioned_right_eye() {
    translate([p32_right_eye_x, p32_right_eye_y, p32_right_eye_z]) {
        rotate([0, 0, 0]) {
            if (assembly_mode != "skull_only") {
                goblin_eye_shell(is_left_eye = false);
                if (show_electronics) {
                    translate([0, 5, 0]) gc9a01_display();
                }
            }
        }
    }
}

module positioned_nose() {
    translate([p32_nose_x, p32_nose_y, p32_nose_z]) {
        rotate([0, 0, 0]) {
            if (assembly_mode != "skull_only") {
                goblin_nose_shell();
                if (show_electronics) {
                    translate([0, -5, 0]) hc_sr04_sensor();
                }
            }
        }
    }
}

module positioned_mouth() {
    translate([p32_mouth_x, p32_mouth_y, p32_mouth_z]) {
        rotate([0, 0, 0]) {
            if (assembly_mode != "skull_only") {
                // Note: goblin_mouth_shell not yet created - placeholder
                color([0.6, 0.4, 0.2]) {
                    difference() {
                        ellipsoid([18, 15, 8]);
                        ellipsoid([15, 12, 6]);
                    }
                }
                if (show_electronics) {
                    translate([0, 5, 0]) gc9a01_display();
                }
            }
        }
    }
}

// =============================================================================
// MAIN CONTROLLER POSITIONING
// =============================================================================

module positioned_main_controller() {
    // Position ESP32-S3 in skull base area
    translate([0, -20, -30]) {
        rotate([0, 0, 0]) {
            if (show_electronics && assembly_mode != "skull_only") {
                esp32_s3_devkit();
            }
        }
    }
}

// =============================================================================
// ASSEMBLY RENDERING
// =============================================================================

module goblin_head_exploded() {
    // Exploded view for assembly documentation
    
    // Skull (moved down)
    translate([0, 0, -30]) {
        goblin_skull_complete(show_components = false, show_coordinates = true);
    }
    
    // Components (moved outward)
    translate([-60, 0, 0]) positioned_left_eye();
    translate([60, 0, 0]) positioned_right_eye();
    translate([0, 40, 0]) positioned_nose();
    translate([0, -60, 0]) positioned_mouth();
    translate([0, 0, -60]) positioned_main_controller();
    
    // Assembly arrows (visual aids)
    color([0.8, 0.8, 0.8, 0.5]) {
        translate([-30, 0, -15]) rotate([0, 90, 0]) cylinder(h=20, r=1);
        translate([30, 0, -15]) rotate([0, -90, 0]) cylinder(h=20, r=1);
        translate([0, 20, -15]) rotate([90, 0, 0]) cylinder(h=20, r=1);
        translate([0, -30, -15]) rotate([-90, 0, 0]) cylinder(h=20, r=1);
    }
}

module goblin_head_complete() {
    // Complete assembled head
    
    if (assembly_mode != "components_only") {
        goblin_skull_complete(
            show_components = true, 
            show_coordinates = false
        );
    }
    
    positioned_left_eye();
    positioned_right_eye();
    positioned_nose();
    positioned_mouth();
    positioned_main_controller();
    
    wire_harness();
}

// =============================================================================
// RENDER SELECTION
// =============================================================================

if (assembly_mode == "exploded") {
    goblin_head_exploded();
} else if (assembly_mode == "skull_only") {
    goblin_skull_complete(show_components = true, show_coordinates = true);
} else if (assembly_mode == "components_only") {
    positioned_left_eye();
    positioned_right_eye();
    positioned_nose();
    positioned_mouth();
    positioned_main_controller();
} else {
    // Default: complete assembly
    goblin_head_complete();
}

// =============================================================================
// BUILD INSTRUCTIONS INTEGRATION
// =============================================================================

/*
ASSEMBLY SEQUENCE:

1. SKULL PREPARATION:
   - 3D print goblin_skull_complete.scad
   - Clean support material from component mount areas
   - Test-fit all mounting rings

2. ELECTRONICS PREPARATION:
   - Flash ESP32-S3-DevKitC-1 with P32 firmware
   - Test all displays and sensor independently
   - Prepare wire harness according to zero-crossover diagram

3. COMPONENT ASSEMBLY:
   - Install displays in eye positions first
   - Mount HC-SR04 sensor in nose position
   - Install mouth display
   - Route wiring through skull cavity

4. SHELL INSTALLATION:
   - Attach goblin_eye_shells over display mounts
   - Install goblin_nose_shell with sensor cutouts
   - Attach mouth shell (when created)

5. FINAL INTEGRATION:
   - Connect all wiring to ESP32-S3
   - Test complete system functionality
   - Verify all 9 emotion states work correctly

6. HEAD-BODY CONNECTION:
   - Prepare neck joint for torso attachment
   - Route control cables to body controller
   - Test mesh networking with body subsystems

COORDINATE SYSTEM NOTES:
- All measurements in this file are in millimeters for OpenSCAD
- P32 JSON configurations remain in inches
- Conversion factor: 25.4mm per inch
- Origin (0,0,0) = nose_center in both systems

MANUFACTURING NOTES:
- Print skull in PLA or PETG for strength
- Component shells can be printed in flexible TPU for character expression
- Consider printing in sections for large printers
- Post-processing: light sanding and bone-colored paint
*/