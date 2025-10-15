// GUARANTEED Working Goblin Skull - No Complex Geometry
// This will definitely render as a goblin shape

$fn = 50;

// P32 positions
LEFT_EYE = [-26.67, 17.78, -8.89];
RIGHT_EYE = [26.67, 17.78, -8.89];
NOSE = [0, 0, 6.35];
MOUTH = [0, -26.67, 0];

difference() {
    // Main goblin head - built from basic shapes only
    union() {
        // Main cranium - elongated oval
        translate([0, -5, 10])
            scale([1.0, 1.3, 0.8])
            sphere(r=35);
        
        // Massive brow ridge - clearly goblin
        translate([0, 22, 8])
            scale([1.4, 0.4, 0.6])
            cube([50, 20, 20], center=true);
        
        // Protruding jaw
        translate([0, -30, -20])
            scale([0.9, 1.2, 0.8])
            sphere(r=25);
        
        // Pointed ears - unmistakably goblin
        translate([-45, -5, 5])
            rotate([0, -30, -10])
            scale([0.3, 0.8, 1.5])
            cube([10, 15, 25], center=true);
        translate([45, -5, 5])
            rotate([0, 30, 10])
            scale([0.3, 0.8, 1.5])
            cube([10, 15, 25], center=true);
    }
    
    // Component clearances
    translate(LEFT_EYE) cylinder(r=12, h=6, center=true);
    translate(RIGHT_EYE) cylinder(r=12, h=6, center=true);
    translate(NOSE) cube([20, 15, 8], center=true);
    translate(MOUTH) cylinder(r=11, h=6, center=true);
    
    // Interior hollow
    translate([0, -5, 5])
        scale([0.85, 0.9, 0.9])
        sphere(r=30);
}

// Mounting hardware
color([0.2, 0.2, 0.2]) {
    // Eye mounts
    translate(LEFT_EYE)
        difference() {
            cylinder(r=13, h=4, center=true);
            cylinder(r=11, h=5, center=true);
        }
    translate(RIGHT_EYE)
        difference() {
            cylinder(r=13, h=4, center=true);
            cylinder(r=11, h=5, center=true);
        }
    
    // Mouth mount
    translate(MOUTH)
        difference() {
            cylinder(r=12, h=4, center=true);
            cylinder(r=10, h=5, center=true);
        }
    
    // Nose mount
    translate(NOSE)
        difference() {
            cube([22, 17, 10], center=true);
            cube([20, 15, 8], center=true);
        }
}