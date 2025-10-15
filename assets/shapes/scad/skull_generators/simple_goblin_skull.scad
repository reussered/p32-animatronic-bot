// Simple Goblin Skull Generator - Green and Menacing
// P32 Animatronic Bot - Visual Enhancement Version

// Goblin colors
goblin_green = [0.2, 0.5, 0.2];  // Forest green
goblin_dark = [0.15, 0.35, 0.15]; // Darker green for shadows

// P32 component positions (from goblin_full.json)
p32_left_eye_x = -26.67;   // -1.05"
p32_left_eye_y = 17.78;    // +0.7" 
p32_left_eye_z = -8.89;    // -0.35"
p32_right_eye_x = 26.67;   // +1.05"
p32_right_eye_y = 17.78;   // +0.7"
p32_right_eye_z = -8.89;   // -0.35"
p32_nose_x = 0;            // 0"
p32_nose_y = 0;            // 0"
p32_nose_z = 6.35;         // +0.25"
p32_mouth_x = 0;           // 0"
p32_mouth_y = -26.67;      // -1.05"
p32_mouth_z = 0;           // 0"

module goblin_skull() {
    color(goblin_green) {
        difference() {
            union() {
                // Main head - more elongated back, flatter top (goblin cranium)
                translate([0, -8, 8])
                    scale([1.0, 1.2, 0.75])
                    sphere(d=85);
                
                // MASSIVE brow ridge - key goblin feature
                translate([0, 25, 12])
                    scale([1.8, 0.6, 0.7])
                    sphere(d=58);
                
                // Protruding underbite jaw - very goblin
                translate([0, -32, -20])
                    scale([0.85, 1.3, 0.8])
                    sphere(d=68);
                
                // Sharp, angular cheekbones
                translate([-38, 8, -5])
                    rotate([0, 0, -25])
                    scale([0.5, 0.9, 1.1])
                    sphere(d=32);
                translate([38, 8, -5])
                    rotate([0, 0, 25])
                    scale([0.5, 0.9, 1.1])
                    sphere(d=32);
                
                // Long, pointed ears - very dramatic
                translate([-52, -5, 8])
                    rotate([0, -40, -20])
                    scale([0.35, 1.0, 2.0])
                    sphere(d=28);
                translate([52, -5, 8])
                    rotate([0, 40, 20])
                    scale([0.35, 1.0, 2.0])
                    sphere(d=28);
                
                // Sagittal crest (skull ridge)
                translate([0, -5, 32])
                    scale([0.25, 1.8, 0.4])
                    sphere(d=22);
            }
            
            // HUGE eye sockets - deep and intimidating
            translate([p32_left_eye_x, p32_left_eye_y, p32_left_eye_z])
                scale([1.4, 1.3, 1.4])
                sphere(d=30);
            translate([p32_right_eye_x, p32_right_eye_y, p32_right_eye_z])
                scale([1.4, 1.3, 1.4])
                sphere(d=30);
            
            // Flared nostril cavities - beast-like
            translate([p32_nose_x, p32_nose_y + 4, p32_nose_z])
                scale([1.2, 1.6, 1.3])
                sphere(d=16);
            
            // Wide, snarling mouth cavity
            translate([p32_mouth_x, p32_mouth_y, p32_mouth_z])
                scale([2.0, 1.0, 1.0])
                sphere(d=24);
            
            // Interior hollow
            translate([0, -5, 0])
                scale([0.85, 0.9, 0.9])
                sphere(d=70);
        }
    }
    
    // Darker details
    color(goblin_dark) {
        // Brow ridges over eyes
        translate([p32_left_eye_x, p32_left_eye_y + 5, p32_left_eye_z + 5])
            scale([0.4, 0.6, 0.3])
            sphere(d=15);
        translate([p32_right_eye_x, p32_right_eye_y + 5, p32_right_eye_z + 5])
            scale([0.4, 0.6, 0.3])
            sphere(d=15);
        
        // Nose bridge
        translate([0, 8, 2])
            scale([0.3, 1.0, 0.8])
            sphere(d=8);
    }
    
    // Component mounting rings (dark gray)
    color([0.3, 0.3, 0.3]) {
        // Eye display mounts
        translate([p32_left_eye_x, p32_left_eye_y, p32_left_eye_z])
            difference() {
                cylinder(h=3, d=24, center=true);
                cylinder(h=4, d=20, center=true);
            }
        translate([p32_right_eye_x, p32_right_eye_y, p32_right_eye_z])
            difference() {
                cylinder(h=3, d=24, center=true);
                cylinder(h=4, d=20, center=true);
            }
        
        // Mouth display mount
        translate([p32_mouth_x, p32_mouth_y, p32_mouth_z])
            difference() {
                cylinder(h=3, d=22, center=true);
                cylinder(h=4, d=18, center=true);
            }
        
        // Nose sensor mount
        translate([p32_nose_x, p32_nose_y, p32_nose_z])
            difference() {
                cube([20, 15, 8], center=true);
                cube([18, 13, 6], center=true);
            }
    }
}

// Generate the skull
goblin_skull();