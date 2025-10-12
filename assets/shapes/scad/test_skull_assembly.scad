// Test OpenSCAD File for Skull Assembly Generation
// Simple implementation to test STL generation without complex dependencies

// Parameters that can be set from command line
creature_type = "goblin";  // Can be overridden with -D parameter
show_components = true;
show_skull = true;

// Basic skull shapes for testing
module test_goblin_skull() {
    // Simple goblin skull approximation
    scale([1.3, 1.0, 1.2]) {
        difference() {
            // Main skull shape
            hull() {
                sphere(d=80);
                translate([0, 0, -20]) sphere(d=70);
                translate([0, -30, -10]) sphere(d=60);
            }
            
            // Eye sockets
            translate([-26.67, 17.78, -8.89]) sphere(d=35);
            translate([26.67, 17.78, -8.89]) sphere(d=35);
            
            // Mouth
            translate([0, -26.67, 0]) sphere(d=35);
            
            // Nasal cavity
            translate([0, 0, 6.35]) sphere(d=20);
        }
    }
}

module test_zombie_skull() {
    // Zombie skull with decay features
    scale([1.3, 1.0, 1.2]) {
        difference() {
            // Main skull shape (slightly larger than goblin)
            hull() {
                sphere(d=82);
                translate([0, 0, -22]) sphere(d=72);
                translate([0, -32, -12]) sphere(d=62);
            }
            
            // Eye sockets (slightly larger)
            translate([-26.67, 17.78, -8.89]) sphere(d=37);
            translate([26.67, 17.78, -8.89]) sphere(d=37);
            
            // Mouth (damaged/larger)
            translate([0, -26.67, 0]) sphere(d=40);
            
            // Nasal cavity (deteriorated)
            translate([0, 0, 6.35]) sphere(d=25);
            
            // Decay holes
            translate([-15, 10, 5]) sphere(d=8);
            translate([12, -5, 10]) sphere(d=6);
        }
    }
}

module test_android_skull() {
    // Android skull with geometric features
    scale([1.2, 0.95, 1.1]) {
        difference() {
            // Main skull shape (more geometric)
            hull() {
                sphere(d=78);
                translate([0, 0, -18]) sphere(d=68);
                translate([0, -28, -8]) sphere(d=58);
            }
            
            // Eye sockets (more angular)
            translate([-26.67, 17.78, -8.89]) {
                hull() {
                    sphere(d=33);
                    translate([0, 0, 5]) sphere(d=28);
                }
            }
            translate([26.67, 17.78, -8.89]) {
                hull() {
                    sphere(d=33);
                    translate([0, 0, 5]) sphere(d=28);
                }
            }
            
            // Mouth
            translate([0, -26.67, 0]) sphere(d=33);
            
            // Nasal cavity
            translate([0, 0, 6.35]) sphere(d=18);
            
            // Panel lines
            translate([-5, 20, 0]) cube([10, 1, 60], center=true);
            translate([0, 15, 15]) cube([80, 1, 10], center=true);
        }
    }
}

module test_cat_skull() {
    // Cat skull with feline proportions
    scale([0.8, 1.2, 0.9]) {
        difference() {
            // Main skull shape (elongated)
            hull() {
                sphere(d=60);
                translate([0, -25, -10]) sphere(d=45);
                translate([0, -40, -5]) sphere(d=35);
            }
            
            // Eye sockets (closer together)
            translate([-22, 10, -5]) sphere(d=25);
            translate([22, 10, -5]) sphere(d=25);
            
            // Mouth (smaller)
            translate([0, -35, -8]) sphere(d=20);
            
            // Nasal cavity (prominent)
            translate([0, -15, 0]) sphere(d=12);
        }
    }
}

// Component positions for testing
module test_components() {
    if (show_components) {
        // Display positions (simplified as spheres)
        color([0.8, 0.2, 0.2, 0.7]) {
            translate([-26.67, 17.78, -8.89]) sphere(d=20); // Left eye
        }
        color([0.2, 0.8, 0.2, 0.7]) {
            translate([26.67, 17.78, -8.89]) sphere(d=20); // Right eye  
        }
        color([0.2, 0.2, 0.8, 0.7]) {
            translate([0, -26.67, 0]) sphere(d=20); // Mouth
        }
        
        // Sensor position
        color([0.8, 0.8, 0.2, 0.7]) {
            translate([0, 0, 6.35]) cube([20, 8, 8], center=true); // Nose sensor
        }
        
        // Speaker positions  
        color([0.8, 0.2, 0.8, 0.7]) {
            translate([-38.1, 19.05, 12.7]) sphere(d=15); // Left speaker
        }
        color([0.2, 0.8, 0.8, 0.7]) {
            translate([38.1, 19.05, 12.7]) sphere(d=15); // Right speaker
        }
    }
}

// Main assembly
module skull_assembly() {
    if (show_skull) {
        if (creature_type == "goblin") {
            color([0.9, 0.85, 0.75]) test_goblin_skull();
        }
        else if (creature_type == "zombie") {
            color([0.7, 0.75, 0.6]) test_zombie_skull();
        }
        else if (creature_type == "android") {
            color([0.8, 0.8, 0.85]) test_android_skull();
        }
        else if (creature_type == "cat") {
            color([0.9, 0.88, 0.86]) test_cat_skull();
        }
        else if (creature_type == "lion") {
            color([0.85, 0.8, 0.7]) scale([1.4, 1.4, 1.4]) test_cat_skull();
        }
        else if (creature_type == "dragon_fire") {
            color([0.4, 0.3, 0.2]) scale([1.2, 1.5, 1.1]) test_goblin_skull();
        }
        else if (creature_type == "dragon_ice") {
            color([0.8, 0.85, 0.9]) scale([1.2, 1.5, 1.1]) test_goblin_skull();
        }
        else if (creature_type == "bear") {
            color([0.8, 0.75, 0.65]) scale([1.3, 1.2, 1.3]) test_goblin_skull();
        }
        else {
            // Default to goblin
            color([0.9, 0.85, 0.75]) test_goblin_skull();
        }
    }
    
    test_components();
}

// Render the assembly
skull_assembly();