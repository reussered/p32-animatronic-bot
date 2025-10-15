// Goblin Nose Shell - Warty protrusion incorporating sensor_basic_mount
// Character-specific decorative shell for goblin nose sensor

use <../basic_mounts/sensor_basic_mount.scad>

module goblin_nose_shell() {
    nose_width = 60;
    nose_height = 35;
    nose_depth = 25;
    wart_count = 12;
    
    difference() {
        union() {
            // Main warty nose shape
            hull() {
                // Base attachment
                translate([0, 0, 0])
                    cylinder(h=5, d=nose_width*0.8);
                // Nose tip
                translate([0, nose_height*0.6, nose_depth*0.8])
                    sphere(d=nose_width*0.4);
                // Bridge
                translate([0, nose_height*0.3, nose_depth*0.4])
                    scale([0.8, 0.6, 0.5])
                        sphere(d=nose_width*0.7);
            }
            
            // Warty surface details
            for(i = [0:wart_count-1]) {
                wart_x = (random(i, nose_width) - nose_width/2) * 0.8;
                wart_y = random(i+10, nose_height) * 0.7;
                wart_z = random(i+20, nose_depth) * 0.6 + 3;
                
                translate([wart_x, wart_y, wart_z])
                    sphere(d=random(i+30, 4)+2);
            }
            
            // Nostril ridges
            for(side = [-1, 1]) {
                translate([side * nose_width*0.15, nose_height*0.7, nose_depth*0.6])
                    rotate([0, side*15, 0])
                        scale([0.3, 0.4, 0.6])
                            cylinder(h=8, d=6);
            }
        }
        
        // Sensor cavity (positioned for optimal detection)
        translate([0, nose_height*0.4, nose_depth*0.3])
            rotate([15, 0, 0]) {
                // Cavity for basic sensor mount
                translate([-22.5, -10, -7.5])
                    cube([45, 20, 15]);
                
                // Sensor transducer openings
                translate([-12, 0, 0])
                    cylinder(h=20, d=17, center=true);
                translate([12, 0, 0])
                    cylinder(h=20, d=17, center=true);
            }
        
        // Mounting screw access
        translate([0, nose_height*0.4, nose_depth*0.3])
            rotate([15, 0, 0]) {
                for(side = [-1, 1]) {
                    translate([side * 18, 0, 0])
                        rotate([0, 90, 0])
                            cylinder(h=15, d=4, center=true);
                }
            }
        
        // Nostrils (for character authenticity)
        for(side = [-1, 1]) {
            translate([side * nose_width*0.12, nose_height*0.75, nose_depth*0.7])
                rotate([45, side*10, 0])
                    cylinder(h=8, d=4);
        }
    }
    
    // Include the basic sensor mount (positioned inside shell)
    translate([0, nose_height*0.4, nose_depth*0.3])
        rotate([15, 0, 0])
            translate([-22.5, -10, -7.5])
                sensor_basic_mount();
}

// Generate the nose shell
goblin_nose_shell();

// Helper function for random values
function random(seed, max_val) = max_val * (sin(seed * 173.7 + max_val * 7.3) + 1) / 2;
