// GOBLIN Eye Shell - craggy_socket incorporating display_basic_mount
// Family-aware decorative shell for goblin eye displays

use <../basic_mounts/display_basic_mount.scad>

module goblin_eye_shell(is_left_eye = true) {
    eye_socket_diameter = 45;
    socket_depth = 12;
    
    // Eye style: craggy_socket
    // Surface texture: rough_organic
    // Color scheme: mystical_forest_green
    
    difference() {
        union() {
            // Main eye socket based on family aesthetic
            generate_eye_socket();
            
            // Family-specific surface details
            apply_surface_texture();
        }
        
        // Cutout for basic mount
        translate([0,0,socket_depth*0.4])
            mount_cutout();
    }
    
    // Include the basic mount (positioned inside shell)
    translate([0,0,socket_depth*0.4])
        display_basic_mount();
}

module generate_eye_socket() {
    // Implementation varies by eye_style
    if ("craggy_socket" == "craggy_socket") {
        // Goblin-style craggy implementation
        difference() {
            cylinder(h=socket_depth, d=eye_socket_diameter);
            translate([0,0,socket_depth*0.3])
                cylinder(h=socket_depth*0.7+1, d=eye_socket_diameter*0.8);
        }
    } else if ("craggy_socket" == "rounded_socket") {
        // Bear-style rounded implementation  
        hull() {
            cylinder(h=socket_depth, d=eye_socket_diameter);
            translate([0,0,socket_depth*0.5])
                sphere(d=eye_socket_diameter*0.9);
        }
    } else {
        // Default socket
        cylinder(h=socket_depth, d=eye_socket_diameter);
    }
}

module apply_surface_texture() {
    // Surface texture based on family profile
    if ("rough_organic" == "rough_organic") {
        // Add organic warty details
        for(i = [0:5]) {
            rotate([0, 0, i*60 + random(30)])
                translate([eye_socket_diameter*0.3, 0, random(socket_depth)])
                    sphere(d=random(4)+2);
        }
    }
}

module mount_cutout() {
    cylinder(h=socket_depth*0.6+1, d=35);
}

// Generate left and right eye shells
translate([-30, 0, 0]) goblin_eye_shell(true);   // Left eye
translate([30, 0, 0]) goblin_eye_shell(false);   // Right eye

// Helper function for random values
function random(max_val) = max_val * (sin($t*137.5 + max_val*7.3) + 1) / 2;
