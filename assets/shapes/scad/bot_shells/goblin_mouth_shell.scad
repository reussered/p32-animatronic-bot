// Goblin Mouth Shell - Fanged maw incorporating display_basic_mount
// Character-specific decorative shell for goblin mouth display

use <../basic_mounts/display_basic_mount.scad>

module goblin_mouth_shell() {
    mouth_width = 55;
    mouth_height = 40;
    mouth_depth = 15;
    fang_count = 6;
    
    difference() {
        union() {
            // Main mouth frame
            difference() {
                // Outer mouth shape (slightly oval)
                scale([1.3, 1.0, 1])
                    cylinder(h=mouth_depth, d=mouth_width);
                
                // Inner cavity for basic mount
                translate([0,0,mouth_depth*0.3])
                    scale([1.2, 0.95, 1])
                        cylinder(h=mouth_depth*0.7+1, d=mouth_width*0.85);
            }
            
            // Fang details around the rim
            for(i = [0:fang_count-1]) {
                angle = i * (360/fang_count);
                // Skip bottom center (no fangs on lower lip center)
                if (angle < 150 || angle > 210) {
                    rotate([0, 0, angle]) {
                        translate([mouth_width*0.4, 0, mouth_depth*0.8])
                            rotate([random(15)+10, 0, 0])
                                cylinder(h=random(8)+4, d1=3, d2=1);
                    }
                }
            }
            
            // Lip ridges
            for(angle = [30:60:330]) {
                rotate([0, 0, angle]) {
                    translate([mouth_width*0.35, 0, mouth_depth*0.6])
                        rotate([0, 90, 0])
                            scale([0.6, 0.3, 1])
                                cylinder(h=6, d=4);
                }
            }
            
            // Corner warts
            for(side = [-1, 1]) {
                translate([side * mouth_width*0.5, mouth_width*0.1, mouth_depth*0.7])
                    sphere(d=5);
            }
        }
        
        // Mounting cavity for basic display mount
        translate([0,0,mouth_depth*0.3])
            scale([1.15, 0.9, 1])
                cylinder(h=mouth_depth*0.7+1, d=mouth_width*0.85);
        
        // Access holes for basic mount screws
        for(i=[0:3]) {
            rotate([0, 0, i*90 + 45]) {
                translate([mouth_width*0.3, 0, mouth_depth*0.6])
                    cylinder(h=mouth_depth*0.4+1, d=4);
            }
        }
        
        // Central display opening (slightly smaller for menacing look)
        translate([0,0,-1])
            scale([1.1, 0.85, 1])
                cylinder(h=mouth_depth+2, d=mouth_width*0.7);
    }
    
    // Include the basic mount (positioned inside shell)
    translate([0,0,mouth_depth*0.3])
        display_basic_mount();
}

// Generate the mouth shell
goblin_mouth_shell();

// Helper function for random values
function random(max_val) = max_val * (sin(*211.3 + max_val*11.7) + 1) / 2;
