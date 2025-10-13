// Back Mount System for Parasol Deployment
// Part of P32 Animatronic Bot Project
// Mounts on robot's back spine rail system

// Main Parameters
mount_plate_width = 203.2;      // 8.0 INCH
mount_plate_height = 304.8;     // 12.0 INCH
mount_plate_thickness = 12.7;   // 0.5 INCH

spine_rail_width = 25.4;        // 1.0 INCH standard
clamp_gap = 27.4;              // 1.08 INCH for clearance

servo_mount_spacing = 60;       // Distance between servo centers
pivot_shaft_diameter = 8;       // 8mm hardened steel shaft

// Servo Specifications (MG996R)
servo_body_length = 40.7;
servo_body_width = 19.7; 
servo_body_height = 42.9;
servo_horn_diameter = 25;

module back_mount_base_plate() {
    difference() {
        // Main mounting plate
        cube([mount_plate_width, mount_plate_height, mount_plate_thickness]);
        
        // Spine rail clamp channels (dual rail system)
        translate([mount_plate_width/2 - spine_rail_width/2 - 30, 20, -1])
            cube([spine_rail_width, mount_plate_height - 40, mount_plate_thickness + 2]);
        
        translate([mount_plate_width/2 + spine_rail_width/2 + 5, 20, -1])
            cube([spine_rail_width, mount_plate_height - 40, mount_plate_thickness + 2]);
        
        // Lightening holes for weight reduction
        for(x = [50, 100, 150]) {
            for(y = [60, 120, 180, 240]) {
                translate([x, y, -1])
                    cylinder(d=20, h=mount_plate_thickness + 2);
            }
        }
        
        // Servo mounting holes
        translate([mount_plate_width/2, mount_plate_height/2, -1]) {
            // Primary pivot servo mount
            translate([-servo_mount_spacing/2, 0, 0])
                servo_mounting_holes();
            
            // Secondary stabilizer servo mount  
            translate([servo_mount_spacing/2, 0, 0])
                servo_mounting_holes();
        }
        
        // Cable routing channels
        translate([10, mount_plate_height - 30, mount_plate_thickness/2])
            cube([mount_plate_width - 20, 8, mount_plate_thickness]);
    }
    
    // Reinforcement ribs
    translate([20, 0, mount_plate_thickness]) {
        for(y = [40, 120, 200, 280]) {
            translate([0, y, 0])
                cube([mount_plate_width - 40, 4, 8]);
        }
    }
}

module servo_mounting_holes() {
    // Standard MG996R mounting pattern
    servo_hole_spacing_x = 30;
    servo_hole_spacing_y = 40;
    
    for(x = [-servo_hole_spacing_x/2, servo_hole_spacing_x/2]) {
        for(y = [-servo_hole_spacing_y/2, servo_hole_spacing_y/2]) {
            translate([x, y, 0])
                cylinder(d=3.2, h=20);  // M3 screw holes
        }
    }
    
    // Central shaft hole for servo horn
    cylinder(d=servo_horn_diameter + 2, h=20);
}

module pivot_mechanism_housing() {
    difference() {
        // Main housing body
        translate([mount_plate_width/2 - 40, mount_plate_height/2 - 30, mount_plate_thickness])
            cube([80, 60, 35]);
        
        // Servo cavity (primary)
        translate([mount_plate_width/2 - servo_mount_spacing/2, mount_plate_height/2, mount_plate_thickness + 5]) {
            translate([-servo_body_length/2, -servo_body_width/2, 0])
                cube([servo_body_length, servo_body_width, servo_body_height]);
        }
        
        // Servo cavity (secondary)
        translate([mount_plate_width/2 + servo_mount_spacing/2, mount_plate_height/2, mount_plate_thickness + 5]) {
            translate([-servo_body_length/2, -servo_body_width/2, 0])
                cube([servo_body_length, servo_body_width, servo_body_height]);
        }
        
        // Pivot shaft channel
        translate([mount_plate_width/2, mount_plate_height/2, mount_plate_thickness - 1])
            rotate([0, 0, 0])
                cylinder(d=pivot_shaft_diameter + 0.2, h=50);
        
        // Access panels for servo wiring
        translate([mount_plate_width/2 - 35, mount_plate_height/2 + 20, mount_plate_thickness + 25])
            cube([70, 8, 15]);
    }
    
    // Bearing mounts for pivot shaft
    translate([mount_plate_width/2, mount_plate_height/2, mount_plate_thickness + 32]) {
        difference() {
            cylinder(d=20, h=8);
            cylinder(d=pivot_shaft_diameter + 0.1, h=10);
        }
    }
}

module attachment_platform() {
    platform_width = 150;
    platform_depth = 100;
    platform_thickness = 8;
    
    difference() {
        // Platform base
        translate([mount_plate_width/2 - platform_width/2, mount_plate_height - platform_depth, 
                   mount_plate_thickness + 35])
            cube([platform_width, platform_depth, platform_thickness]);
        
        // Parasol central mount (standard 50mm tube interface)
        translate([mount_plate_width/2, mount_plate_height - platform_depth/2, mount_plate_thickness + 30])
            cylinder(d=52, h=15);
        
        // Bolt pattern for equipment attachment
        for(x = [-40, 0, 40]) {
            for(y = [-20, 20]) {
                translate([mount_plate_width/2 + x, mount_plate_height - platform_depth/2 + y, 
                          mount_plate_thickness + 30])
                    cylinder(d=5.2, h=15);  // M5 bolts
            }
        }
        
        // Cable pass-through
        translate([mount_plate_width/2 + 30, mount_plate_height - platform_depth + 10, 
                   mount_plate_thickness + 30])
            cylinder(d=15, h=15);
    }
    
    // Reinforcement brackets
    for(side = [-1, 1]) {
        translate([mount_plate_width/2 + side * platform_width/2 - side * 5, 
                   mount_plate_height - platform_depth, mount_plate_thickness + 35]) {
            cube([5, platform_depth, platform_thickness + 15]);
        }
    }
}

module locking_mechanism() {
    // Spring-loaded transport locks
    lock_positions = [
        [30, mount_plate_height/2 - 80],
        [mount_plate_width - 30, mount_plate_height/2 - 80],
        [30, mount_plate_height/2 + 80], 
        [mount_plate_width - 30, mount_plate_height/2 + 80]
    ];
    
    for(pos = lock_positions) {
        translate([pos[0], pos[1], mount_plate_thickness]) {
            difference() {
                cylinder(d=20, h=12);
                cylinder(d=6.2, h=15);  // Spring-loaded pin
            }
            
            // Spring housing
            translate([0, 0, 12])
                cylinder(d=12, h=8);
        }
    }
}

// Assembly
module complete_back_mount_system() {
    color("silver") back_mount_base_plate();
    color("darkblue") pivot_mechanism_housing();  
    color("orange") attachment_platform();
    color("red") locking_mechanism();
}

// Render complete system
complete_back_mount_system();

// Print-ready individual components
if(false) {  // Set to true to generate individual STL files
    translate([0, -400, 0]) back_mount_base_plate();
    translate([250, -400, 0]) pivot_mechanism_housing();
    translate([0, -250, 0]) attachment_platform();
    translate([200, -250, 0]) locking_mechanism();
}