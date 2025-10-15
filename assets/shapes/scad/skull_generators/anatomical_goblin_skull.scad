// Anatomically Correct Goblin Skull
// Based on actual skull structure with goblin proportions

// Skull base structure
module skull_base() {
    // Main cranium (brain case)
    hull() {
        // Frontal bone area
        translate([0, 15, 10])
            scale([0.9, 0.8, 0.7])
            sphere(r=35);
        
        // Parietal bones
        translate([0, -10, 15])
            scale([1.0, 1.2, 0.8])
            sphere(r=32);
        
        // Occipital bone (back of skull)
        translate([0, -35, 5])
            scale([0.8, 0.6, 0.9])
            sphere(r=28);
    }
}

// Eye socket cavities (orbits)
module eye_sockets() {
    for(x = [-30, 30]) {
        translate([x, 20, 5]) {
            // Main orbital cavity
            rotate([10, 0, 0])
                cylinder(r=16, h=25, center=true);
            
            // Orbital rim bevel
            translate([0, 0, -8])
                cylinder(r1=18, r2=14, h=6);
        }
    }
}

// Nasal cavity
module nasal_cavity() {
    translate([0, 25, -5]) {
        // Main nasal opening
        scale([0.6, 1.0, 1.5])
            cylinder(r=8, h=20, center=true);
        
        // Nasal septum area
        translate([0, 3, 0])
            cube([12, 15, 18], center=true);
    }
}

// Mouth opening (maxilla area)
module mouth_cavity() {
    translate([0, -5, -25]) {
        // Main oral cavity
        scale([1.2, 0.8, 1.0])
            cylinder(r=18, h=15, center=true);
        
        // Dental arcade
        translate([0, 5, 0])
            cube([32, 8, 12], center=true);
    }
}

// Goblin character modifications
module goblin_features() {
    // Pronounced brow ridge
    translate([0, 22, 18])
        scale([1.4, 0.4, 0.6])
        sphere(r=15);
    
    // Enlarged eye sockets (goblin trait)
    for(x = [-30, 30]) {
        translate([x, 20, 5])
            cylinder(r=2, h=30, center=true);
    }
    
    // Protruding jaw (prognathism)
    translate([0, 5, -28])
        scale([0.9, 1.2, 0.7])
        sphere(r=20);
}

// Hardware mounting points
module mounting_features() {
    // Display mount recesses
    for(x = [-30, 30]) {
        translate([x, 25, 5]) {
            cylinder(r=14, h=3);  // Mount flange
            cylinder(r=12, h=8);  // Display opening
        }
    }
    
    // Mouth display recess
    translate([0, -5, -25]) {
        cylinder(r=14, h=3);
        cylinder(r=12, h=8);
    }
    
    // Sensor cavity (nose area)
    translate([0, 28, -5])
        cube([20, 10, 15], center=true);
    
    // Electronics cavity
    translate([0, -25, 0])
        cube([50, 25, 35], center=true);
}

// Complete skull assembly
module complete_goblin_skull() {
    difference() {
        union() {
            skull_base();
            goblin_features();
        }
        
        // Remove cavities
        eye_sockets();
        nasal_cavity(); 
        mouth_cavity();
        mounting_features();
        
        // Interior hollow (weight reduction)
        scale([0.8, 0.8, 0.85])
            skull_base();
    }
}

// Generate final skull
complete_goblin_skull();