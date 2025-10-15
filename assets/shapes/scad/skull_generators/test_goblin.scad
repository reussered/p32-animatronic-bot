// Test Goblin Head - Simple but Effective
$fn = 40;

// Make sure the color works
goblin_color = [0.2, 0.6, 0.2]; // RGB green

// Simple but clearly goblin-shaped head
difference() {
    // Main shapes
    color(goblin_color) union() {
        // Main skull - flattened and elongated
        scale([1.0, 1.1, 0.8]) 
            sphere(d=70);
        
        // Huge brow ridge
        translate([0, 20, 5])
            scale([1.5, 0.4, 0.6])
            cube([60, 20, 25], center=true);
        
        // Jutting jaw
        translate([0, -25, -15])
            scale([0.9, 1.2, 0.7])
            sphere(d=50);
        
        // Pointed ears
        translate([-40, 0, 0])
            rotate([0, -30, 0])
            cylinder(h=30, d1=8, d2=4, center=true);
        translate([40, 0, 0])
            rotate([0, 30, 0])
            cylinder(h=30, d1=8, d2=4, center=true);
    }
    
    // Eye holes - big and scary
    translate([-27, 18, -9]) sphere(d=22);
    translate([27, 18, -9]) sphere(d=22);
    
    // Nose hole
    translate([0, 0, 6]) sphere(d=10);
    
    // Mouth hole
    translate([0, -27, 0]) 
        scale([1.5, 0.8, 0.8])
        sphere(d=18);
        
    // Hollow interior
    scale([0.85, 0.85, 0.85]) sphere(d=65);
}