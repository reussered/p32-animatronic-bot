// Proper Goblin Skull Generator
// Creates anatomically-inspired skull with distinctive goblin features

module goblin_skull_proper() {
    
    // Skull dimensions (in mm)
    skull_width = 100;
    skull_depth = 90; 
    skull_height = 80;
    
    // Eye socket specifications
    eye_socket_diameter = 35;
    eye_socket_depth = 15;
    eye_spacing = 60; // 3.0" converted to mm
    
    // Nose cavity
    nose_width = 20;
    nose_height = 25;
    nose_depth = 12;
    
    // Mouth cavity
    mouth_width = 40;
    mouth_height = 35;
    mouth_depth = 20;
    
    // Forehead slope (goblin characteristic)
    forehead_slope = 15; // degrees
    
    difference() {
        
        // Main skull shape - elongated and angular for goblin appearance
        hull() {
            // Lower skull base (wider)
            translate([0, -skull_depth/3, -skull_height/3])
                scale([1.2, 1, 0.8])
                sphere(d=skull_width*0.8);
            
            // Upper skull (narrower, sloped back)
            translate([0, skull_depth/4, skull_height/3])
                rotate([forehead_slope, 0, 0])
                scale([0.8, 0.9, 1])
                sphere(d=skull_width*0.6);
            
            // Jaw area (prominent for goblin)
            translate([0, -skull_depth/2, -skull_height/2])
                scale([0.9, 0.7, 0.5])
                sphere(d=skull_width*0.7);
        }
        
        // Cut out eye sockets (deep and angled)
        for(side = [-1, 1]) {
            translate([side * eye_spacing/2, skull_depth/6, skull_height/6]) {
                rotate([10, side*5, 0]) // Slight angle for character
                    cylinder(d=eye_socket_diameter, h=eye_socket_depth*2, center=true);
                
                // Eye socket bevel (realistic bone structure)
                translate([0, 0, -eye_socket_depth/2])
                    cylinder(d1=eye_socket_diameter+4, d2=eye_socket_diameter-2, h=6);
            }
        }
        
        // Nose cavity (rectangular, goblin-like)
        translate([0, skull_depth/4, -skull_height/8]) {
            // Main nasal cavity
            cube([nose_width, nose_depth*2, nose_height], center=true);
            
            // Nasal bridge undercut
            translate([0, nose_depth/2, nose_height/3])
                rotate([45, 0, 0])
                cube([nose_width-4, 8, 8], center=true);
        }
        
        // Mouth cavity (large opening)
        translate([0, -skull_depth/4, -skull_height/2]) {
            // Main mouth opening
            scale([1, 0.8, 1])
                cylinder(d=mouth_width, h=mouth_depth*2, center=true);
            
            // Tooth line undercuts (goblin dental features)
            for(tooth = [-1, 0, 1]) {
                translate([tooth*12, mouth_depth/2, mouth_height/4])
                    rotate([45, 0, 0])
                    cube([4, 6, 8], center=true);
            }
        }
        
        // Temporal fossae (side skull depressions)
        for(side = [-1, 1]) {
            translate([side * skull_width/3, 0, skull_height/6])
                rotate([0, side*20, 0])
                scale([0.6, 1.2, 0.8])
                sphere(d=25);
        }
        
        // Foramen magnum (base of skull opening)
        translate([0, -skull_depth/3, -skull_height/2])
            cylinder(d=15, h=10, center=true);
        
        // Interior hollowing (reduce weight, allow wiring)
        translate([0, 0, -5])
            scale([0.85, 0.85, 0.9])
            hull() {
                translate([0, -skull_depth/4, -skull_height/4])
                    sphere(d=skull_width*0.6);
                translate([0, skull_depth/6, skull_height/4])
                    sphere(d=skull_width*0.5);
            }
    }
    
    // Add goblin character features
    union() {
        // Prominent brow ridge
        translate([0, skull_depth/4, skull_height/3])
            rotate([0, 0, 0])
            scale([1.2, 0.3, 0.4])
            sphere(d=skull_width*0.4);
        
        // Cheekbone protrusions
        for(side = [-1, 1]) {
            translate([side * skull_width/3, skull_depth/8, -skull_height/8])
                rotate([0, side*10, 0])
                scale([0.3, 0.4, 0.6])
                sphere(d=20);
        }
        
        // Nasal bridge (prominent goblin nose)
        translate([0, skull_depth/3, 0])
            rotate([15, 0, 0])
            scale([0.4, 0.8, 1.2])
            sphere(d=18);
    }
}

// Add mounting features for hardware integration
module goblin_skull_with_mounts() {
    difference() {
        goblin_skull_proper();
        
        // Eye mount recesses (for display brackets)
        for(side = [-1, 1]) {
            translate([side * 30, 25, 15]) {
                cylinder(d=28, h=4);  // Recess for mount flange
                cylinder(d=24, h=8);  // Through hole for display
            }
        }
        
        // Mouth mount recess
        translate([0, -22, -40]) {
            cylinder(d=28, h=4);
            cylinder(d=24, h=8);
        }
        
        // Sensor mount cavity (in nose)
        translate([0, 30, -5])
            cube([22, 12, 17], center=true);
        
        // ESP32 cavity (back of skull)
        translate([0, -35, 0])
            cube([60, 30, 40], center=true);
        
        // Wiring channels
        translate([0, -20, -25])
            cube([4, 40, 4], center=true);
    }
}

// Generate the final skull
goblin_skull_with_mounts();