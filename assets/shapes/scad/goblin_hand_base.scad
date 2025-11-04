// Goblin Hand Base Structure - Mechanical Framework
// Parametric palm frame with servo mounting points
// Supports 8-12 servo motors for coupled finger/wrist control
// Weight optimized with hollow interior

// ==============================================================================
// DIMENSIONAL PARAMETERS (mm)
// ==============================================================================

// Palm dimensions (actual goblin proportions)
PALM_LENGTH = 95.0;          // Palm depth (wrist to fingertip center)
PALM_WIDTH = 65.0;           // Palm width (thumb to pinky edge)
PALM_THICKNESS = 18.0;       // Palm main body thickness

// Finger dimensions (coupled groups)
FINGER_LENGTH = 72.0;        // Proximal + middle finger length
FINGER_WIDTH = 18.0;         // Single finger width
FINGER_HEIGHT = 15.0;        // Finger height (extrusion)

// Thumb specifications
THUMB_LENGTH = 55.0;         // Thumb length (shorter, opposable)
THUMB_WIDTH = 16.0;
THUMB_ANGLE = 45.0;          // Angle from palm

// Servo mounting points
SERVO_MOUNTING_HEIGHT = 12.0; // Height of servo motor centerline above palm
SERVO_HOLE_D = 3.5;          // M3 mounting hole diameter
SERVO_HOLE_SPACING = 32.0;   // Standard servo mounting spacing

// Wrist interface
WRIST_DIAMETER = 45.0;       // Connection to arm
WRIST_DEPTH = 20.0;          // Wrist stub into hand
WRIST_SERVO_HEIGHT = 15.0;   // Height of wrist servo axis

// Material properties
PLA_DENSITY = 1.25;          // g/cm³ for weight calculation
WALL_THICKNESS = 2.5;        // Minimum wall thickness
INFILL_FACTOR = 0.20;        // 20% infill for hollow parts

// ==============================================================================
// PALM BASE MODULE
// ==============================================================================

module palm_base(width, length, thickness) {
    difference() {
        // Outer shape: tapered palm with rounded edges
        hull() {
            // Wrist end (wider)
            translate([0, 0, 0])
                cube([width, 25, thickness], center=true);
            
            // Finger ends (narrower, rounded)
            translate([0, length*0.5 - 10, 0])
                scale([0.85, 1.0, 1.0])
                    cube([width, 20, thickness], center=true);
        }
        
        // Hollow interior (reduces weight, prevents warping)
        translate([0, 0, 0])
            cube([width - WALL_THICKNESS*2, length - WALL_THICKNESS*2, thickness - WALL_THICKNESS], center=true);
        
        // Bottom drainage holes (prevent water pooling in confined spaces)
        for(x = [-15, 0, 15]) {
            for(y = [-10, 10, 30]) {
                translate([x, y, -thickness*0.4])
                    cylinder(h=thickness*0.3, d=4);
            }
        }
    }
}

// ==============================================================================
// FINGER STRUCTURES (Coupled Groups for Control)
// ==============================================================================

module finger_group(name, position, length, width, height, linkage_type) {
    // Linkage types:
    // "coupled" = all digits move together (index+middle+ring via single servo)
    // "pinky" = pinky group (separate servo)
    // "thumb" = opposition servo
    
    translate(position) {
        // Main finger structure (hollow, ribbed)
        difference() {
            // Outer shape: tapered finger
            hull() {
                translate([0, 0, 0])
                    cube([width, 20, height], center=true);
                translate([0, length*0.4, 0])
                    scale([0.9, 1.0, 0.8])
                        cube([width, 15, height], center=true);
            }
            
            // Hollow interior
            translate([0, 0, 0])
                cube([width - WALL_THICKNESS, length - WALL_THICKNESS*2, height - WALL_THICKNESS], center=true);
        }
        
        // Reinforcement ribs inside (for stiffness without weight)
        for(i = [0:2]) {
            translate([0, (length*0.2) + i*(length*0.3), 0])
                cube([1.0, length*0.15, height*0.6], center=true);
        }
        
        // Servo linkage attachment point
        translate([0, -length*0.3, height*0.4])
            cylinder(h=6, d=SERVO_HOLE_D, center=true);
    }
}

// ==============================================================================
// SERVO MOUNTING BRACKET
// ==============================================================================

module servo_mounting_bracket(servo_type) {
    // servo_type: "sg90" (standard hobby servo 22.8×11.6×10.2mm)
    //             "micro" (micro servo 16×8×8mm)
    
    servo_length = (servo_type == "sg90") ? 22.8 : 16.0;
    servo_width = (servo_type == "sg90") ? 11.6 : 8.0;
    servo_height = (servo_type == "sg90") ? 10.2 : 8.0;
    
    difference() {
        // Main bracket body
        hull() {
            translate([-servo_width*0.5 - 2, 0, 0])
                cylinder(h=SERVO_MOUNTING_HEIGHT, d=4);
            translate([servo_width*0.5 + 2, 0, 0])
                cylinder(h=SERVO_MOUNTING_HEIGHT, d=4);
            translate([0, servo_length*0.4, 0])
                cylinder(h=SERVO_MOUNTING_HEIGHT, d=4);
        }
        
        // Servo motor cavity
        translate([0, 0, SERVO_MOUNTING_HEIGHT - servo_height - 0.5])
            cube([servo_width + 0.5, servo_length + 0.5, servo_height + 1], center=true);
        
        // M3 mounting holes for servo flange
        for(y = [-servo_length*0.35, servo_length*0.35]) {
            translate([-servo_width*0.35, y, SERVO_MOUNTING_HEIGHT - 2])
                cylinder(h=4, d=SERVO_HOLE_D);
            translate([servo_width*0.35, y, SERVO_MOUNTING_HEIGHT - 2])
                cylinder(h=4, d=SERVO_HOLE_D);
        }
        
        // Cable routing channel
        translate([0, -servo_length*0.5, SERVO_MOUNTING_HEIGHT*0.5])
            cube([4, servo_length*0.3, SERVO_MOUNTING_HEIGHT*0.4], center=true);
    }
}

// ==============================================================================
// WRIST INTERFACE CONNECTOR
// ==============================================================================

module wrist_connector(diameter, depth, servo_height) {
    difference() {
        // Main connector body (cylinder)
        cylinder(h=depth, d=diameter);
        
        // Internal passage for cables from arm ESP32
        translate([0, 0, -1])
            cylinder(h=depth+2, d=diameter*0.6);
        
        // M3 mounting holes (attach hand to arm)
        for(angle = [0, 90, 180, 270]) {
            translate([cos(angle)*diameter*0.3, sin(angle)*diameter*0.3, depth*0.5])
                rotate([0, 90, 0])
                    cylinder(h=diameter*0.2, d=SERVO_HOLE_D);
        }
        
        // Servo axis cavity (for wrist rotation servo)
        translate([0, 0, depth - servo_height - 1])
            cylinder(h=servo_height + 1, d=8.0); // 8mm servo horn clearance
    }
    
    // Reinforcing ribs
    for(angle = [0, 120, 240]) {
        translate([cos(angle)*diameter*0.25, sin(angle)*diameter*0.25, 0])
            cube([2, diameter*0.15, depth], center=true);
    }
}

// ==============================================================================
// COMPLETE HAND ASSEMBLY (LEFT or RIGHT)
// ==============================================================================

module goblin_hand_assembly(hand_side) {
    // hand_side: "left" or "right" (affects mirror/orientation)
    
    mirror_x = (hand_side == "right") ? [-1, 0, 0] : [1, 0, 0];
    
    // Palm base structure
    palm_base(PALM_WIDTH, PALM_LENGTH, PALM_THICKNESS);
    
    // Finger groups (3 groups for coupled control = 3 servos needed)
    
    // Group 1: Index + Middle + Ring fingers (coupled curl via servo #1)
    finger_group(
        "imr_group",
        [0, PALM_LENGTH*0.25, PALM_THICKNESS*0.3],
        FINGER_LENGTH,
        FINGER_WIDTH,
        FINGER_HEIGHT,
        "coupled"
    );
    
    // Group 2: Pinky finger (separate servo #2)
    finger_group(
        "pinky",
        [-PALM_WIDTH*0.3, PALM_LENGTH*0.25, PALM_THICKNESS*0.3],
        FINGER_LENGTH * 0.9,
        FINGER_WIDTH * 0.85,
        FINGER_HEIGHT * 0.85,
        "pinky"
    );
    
    // Thumb (opposing servo #3)
    rotate([0, 0, THUMB_ANGLE])
        translate([PALM_WIDTH*0.3, -PALM_LENGTH*0.1, 0])
            finger_group(
                "thumb",
                [0, 0, 0],
                THUMB_LENGTH,
                THUMB_WIDTH,
                FINGER_HEIGHT,
                "thumb"
            );
    
    // Servo mounting brackets (3 primary servo motors)
    
    // Servo #1: Main finger curl (coupled IMR group) - positioned at palm center
    translate([0, 20, SERVO_MOUNTING_HEIGHT])
        servo_mounting_bracket("sg90");
    
    // Servo #2: Pinky group - positioned at pinky location
    translate([-PALM_WIDTH*0.3, 20, SERVO_MOUNTING_HEIGHT])
        servo_mounting_bracket("sg90");
    
    // Servo #3: Thumb opposition - positioned at thumb base
    translate([PALM_WIDTH*0.3, -PALM_LENGTH*0.1, SERVO_MOUNTING_HEIGHT])
        servo_mounting_bracket("sg90");
    
    // Wrist interface connector
    translate([0, -PALM_LENGTH*0.35, PALM_THICKNESS*0.5])
        wrist_connector(WRIST_DIAMETER, WRIST_DEPTH, WRIST_SERVO_HEIGHT);
}

// ==============================================================================
// WEIGHT CALCULATION
// ==============================================================================

function estimate_hand_weight(servo_count=3) =
    let(
        // 3D printed components (PLA, 20% infill)
        palm_volume = (PALM_LENGTH * PALM_WIDTH * PALM_THICKNESS) * INFILL_FACTOR,
        fingers_volume = (FINGER_LENGTH * FINGER_WIDTH * FINGER_HEIGHT * 3) * INFILL_FACTOR,
        thumb_volume = (THUMB_LENGTH * THUMB_WIDTH * FINGER_HEIGHT) * INFILL_FACTOR,
        wrist_volume = (3.14159 * (WRIST_DIAMETER*0.5)*(WRIST_DIAMETER*0.5) * WRIST_DEPTH) * INFILL_FACTOR,
        
        total_pla_volume = palm_volume + fingers_volume + thumb_volume + wrist_volume,
        pla_mass = total_pla_volume * PLA_DENSITY,
        
        // Hardware and electronics
        servo_mass = servo_count * 9.0,     // ~9g per SG90 servo
        brackets_mass = servo_count * 3.0,  // ~3g per bracket
        hardware_mass = 8.0,                // Screws, inserts, connectors
        
        total_mass = pla_mass + servo_mass + brackets_mass + hardware_mass
    )
    total_mass;

// ==============================================================================
// ASSEMBLY RENDERING
// ==============================================================================

// Render complete left goblin hand with 3 servos
goblin_hand_assembly("left");

// Display weight estimate
echo(str("Goblin Hand Weight: ", estimate_hand_weight(3), " grams"));

