// Goblin Skull Framework - Anatomically Inspired Design
// Focus on proper skull-like proportions with component mounting

module goblin_skull_framework() {
    // Key dimensions from P32 specification
    eye_spacing = 76.2;  // 3.0 INCH
    skull_width = 106.7;   // eye_spacing * 1.4
    skull_length = 99.1;   // eye_spacing * 1.3  
    skull_height = 83.8;   // eye_spacing * 1.1
    wall_thickness = 3.0;
    
    difference() {
        // Outer skull shape
        skull_exterior();
        
        // Interior cavity
        translate([0, 0, 10])
            scale([0.85, 0.85, 0.9])
                skull_exterior();
        
        // Component mounting holes
        component_cutouts();
    }
}

module skull_exterior() {
    hull() {
        // Cranium (main skull dome)
        translate([0, -10, 25])
            scale([1.0, 0.8, 0.7])
                sphere(d=90);
                
        // Facial area (forward projection)
        translate([0, 25, 0])
            scale([0.8, 0.6, 0.6])
                sphere(d=70);
        
        // Lower jaw area
        translate([0, 15, -25])
            scale([0.9, 0.7, 0.4])
                sphere(d=60);
    }
    
    // Brow ridges (goblin characteristic)
    for(side = [-1, 1]) {
        translate([side * 25, 20, 15])
            scale([0.7, 0.4, 0.3])
                sphere(d=20);
    }
}

module component_cutouts() {
    // Eye sockets (anatomical shape)
    for(side = [-1, 1]) {
        translate([side * 38.1, 17.78, -8.89]) {  // From JSON coordinates
            rotate([15, 0, 0])
                cylinder(h=30, d1=32, d2=20);
        }
    }
    
    // Nasal cavity (sensor mount)
    translate([0, 30, 6.35]) {  // Nose position from JSON
        cube([25, 20, 15], center=true);
    }
    
    // Mouth opening (display mount) 
    translate([0, 10, -26.67]) {  // Mouth position adjusted
        cylinder(h=15, d=25);
    }
    
    // Wiring channels
    translate([0, 0, -10])
        cube([8, 80, 8], center=true);
}

goblin_skull_framework();