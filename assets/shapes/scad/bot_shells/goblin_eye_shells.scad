// Goblin Eye Shell - Craggy socket incorporating display_basic_mount
// Character-specific decorative shell for goblin eye displays

use <../basic_mounts/display_basic_mount.scad>

module goblin_eye_shell(is_left_eye = true) {
    eye_socket_diameter = 45;
    socket_depth = 12;
    crag_count = 8;
    
    difference() {
        union() {
            // Main craggy eye socket
            difference() {
                // Outer socket shape
                hull() {
                    cylinder(h=socket_depth, d=eye_socket_diameter);
                    // Slightly oval shape
                    scale([1.1, 0.9, 1])
                        cylinder(h=socket_depth*0.8, d=eye_socket_diameter*0.9);
                }
                
                // Inner socket cavity for basic mount
                translate([0,0,socket_depth*0.4])
                    cylinder(h=socket_depth*0.6+1, d=eye_socket_diameter*0.75);
            }
            
            // Craggy surface details
            for(i = [0:crag_count-1]) {
                rotate([0, 0, i*(360/crag_count) + random(i, 30)]) {
                    translate([eye_socket_diameter*0.4, 0, random(i+10, socket_depth*0.3)])
                        rotate([random(i+20, 20), random(i+30, 20), 0])
                            cylinder(h=random(i+40, 4)+2, d=random(i+50, 3)+2);
                }
            }
            
            // Brow ridge (more pronounced for left eye)
            if (is_left_eye) {
                translate([0, eye_socket_diameter*0.3, socket_depth*0.8])
                    rotate([15, 0, 0])
                        scale([1.2, 0.3, 0.4])
                            cylinder(h=6, d=eye_socket_diameter*0.8);
            } else {
                translate([0, eye_socket_diameter*0.25, socket_depth*0.8])
                    rotate([10, 0, 0])
                        scale([1.1, 0.25, 0.35])
                            cylinder(h=5, d=eye_socket_diameter*0.75);
            }
        }
        
        // Mounting cavity for basic display mount
        translate([0,0,socket_depth*0.4])
            cylinder(h=socket_depth*0.6+1, d=eye_socket_diameter*0.75);
        
        // Access holes for basic mount screws
        for(i=[0:3]) {
            rotate([0, 0, i*90 + 45]) {
                translate([eye_socket_diameter*0.25, 0, socket_depth*0.6])
                    cylinder(h=socket_depth*0.4+1, d=4);
            }
        }
    }
    
    // Include the basic mount (positioned inside shell)
    translate([0,0,socket_depth*0.4])
        display_basic_mount();
}

// Generate left and right eye shells
translate([-30, 0, 0]) goblin_eye_shell(true);   // Left eye
translate([30, 0, 0]) goblin_eye_shell(false);   // Right eye

// Helper function for random values (simplified)
function random(seed, max_val) = max_val * (sin(seed * 137.5) + 1) / 2;
