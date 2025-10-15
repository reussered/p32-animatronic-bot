// Dramatic Goblin Skull - Menacing and Green
// P32 Animatronic Bot - Enhanced Goblin Design

$fn = 50; // Smooth curves

// Goblin green color (RGB values 0-1)
goblin_green = [0.25, 0.4, 0.15];  // Dark forest green
goblin_accent = [0.15, 0.25, 0.1]; // Darker green for details

// P32 component positions (converted from inches to mm)
left_eye_x = -26.67;   // -1.05"
left_eye_y = 17.78;    // +0.7" 
left_eye_z = -8.89;    // -0.35"
right_eye_x = 26.67;   // +1.05"
right_eye_y = 17.78;   // +0.7"
right_eye_z = -8.89;   // -0.35"
nose_x = 0;            // 0"
nose_y = 0;            // 0"
nose_z = 6.35;         // +0.25"
mouth_x = 0;           // 0"
mouth_y = -26.67;      // -1.05"
mouth_z = 0;           // 0"

// Main goblin skull with exaggerated features
module dramatic_goblin_skull() {
    color(goblin_green) {
        difference() {
            union() {
                // Main cranium - elongated and pointed back
                translate([0, -5, 8])
                    scale([1.0, 1.3, 0.85])
                    sphere(d=75);
                
                // Massive jutting brow ridge
                translate([0, 25, 8])
                    scale([1.6, 0.5, 0.6])
                    sphere(d=55);
                
                // Aggressive lower jaw - protruding
                translate([0, -35, -18])
                    scale([0.9, 1.4, 0.8])
                    sphere(d=65);
                
                // Sharp cheekbones
                translate([-35, 5, -8])
                    rotate([0, 0, -20])
                    scale([0.6, 0.8, 1.0])
                    sphere(d=30);
                translate([35, 5, -8])
                    rotate([0, 0, 20])
                    scale([0.6, 0.8, 1.0])
                    sphere(d=30);
                
                // Pointed ears - very goblin-like
                translate([-50, 0, 5])
                    rotate([0, -45, -15])
                    scale([0.4, 1.2, 1.8])
                    sphere(d=22);
                translate([50, 0, 5])
                    rotate([0, 45, 15])
                    scale([0.4, 1.2, 1.8])
                    sphere(d=22);
            }
            
            // Large, menacing eye sockets
            translate([left_eye_x, left_eye_y, left_eye_z])
                scale([1.3, 1.2, 1.3])
                sphere(d=28);
            translate([right_eye_x, right_eye_y, right_eye_z])
                scale([1.3, 1.2, 1.3])
                sphere(d=28);
            
            // Flared nostril cavity
            translate([nose_x, nose_y + 3, nose_z])
                scale([1.0, 1.8, 1.4])
                sphere(d=14);
            
            // Wide, grimacing mouth cavity
            translate([mouth_x, mouth_y, mouth_z])
                scale([2.2, 1.0, 1.0])
                sphere(d=22);
            
            // Hollow interior for electronics
            translate([0, -8, 0])
                scale([0.8, 0.85, 0.9])
                sphere(d=68);
        }
    }
    
    // Darker green details for depth
    color(goblin_accent) {
        // Heavy brow ridges over each eye
        translate([left_eye_x, left_eye_y + 8, left_eye_z + 8])
            rotate([0, 20, 0])
            scale([0.5, 0.8, 0.4])
            sphere(d=18);
        translate([right_eye_x, right_eye_y + 8, right_eye_z + 8])
            rotate([0, -20, 0])
            scale([0.5, 0.8, 0.4])
            sphere(d=18);
        
        // Prominent nose bridge
        translate([0, 12, 4])
            scale([0.4, 1.2, 0.9])
            sphere(d=10);
        
        // Sagittal crest (skull ridge) - very goblin
        translate([0, 0, 28])
            scale([0.3, 2.0, 0.4])
            sphere(d=20);
        
        // Jaw muscle attachment points
        translate([-25, -15, -10])
            scale([0.6, 0.8, 0.5])
            sphere(d=12);
        translate([25, -15, -10])
            scale([0.6, 0.8, 0.5])
            sphere(d=12);
    }
    
    // Component mounting hardware (dark metallic)
    color([0.2, 0.2, 0.2]) {
        // Eye display mounting rings
        translate([left_eye_x, left_eye_y, left_eye_z])
            difference() {
                cylinder(h=4, d=26, center=true);
                cylinder(h=6, d=22, center=true);
            }
        
        translate([right_eye_x, right_eye_y, right_eye_z])
            difference() {
                cylinder(h=4, d=26, center=true);
                cylinder(h=6, d=22, center=true);
            }
        
        // Mouth display mount
        translate([mouth_x, mouth_y, mouth_z])
            difference() {
                cylinder(h=4, d=24, center=true);
                cylinder(h=6, d=20, center=true);
            }
        
        // Nose sensor bracket
        translate([nose_x, nose_y, nose_z])
            difference() {
                cube([22, 16, 10], center=true);
                cube([20, 14, 8], center=true);
            }
    }
}

// Generate the dramatic goblin skull
dramatic_goblin_skull();