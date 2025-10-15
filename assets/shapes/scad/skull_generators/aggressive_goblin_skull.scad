// Aggressive Goblin Skull - Anatomically Correct with Fantasy Features
// Reference: Mix of human skull anatomy + classic fantasy goblin proportions
// Design: Exaggerated brow, sunken eyes, protruding lower jaw, pointed ears

// P32 Component Mounting Positions (in mm from nose center)
LEFT_EYE_POS = [-26.67, 17.78, -8.89];    // Left eye display (GC9A01)
RIGHT_EYE_POS = [26.67, 17.78, -8.89];    // Right eye display (GC9A01)
NOSE_POS = [0, 0, 6.35];                  // Nose sensor (HC-SR04)
MOUTH_POS = [0, -26.67, 0];               // Mouth display (GC9A01)

// Display/sensor dimensions
EYE_DISPLAY_DIA = 28;      // GC9A01 display diameter (27.9mm actual)
MOUTH_DISPLAY_DIA = 28;    // GC9A01 display diameter
SENSOR_WIDTH = 45;         // HC-SR04 width
SENSOR_HEIGHT = 20;        // HC-SR04 height

// Rendering quality
$fn = 80;

// ===== ANATOMICAL SKULL BASE =====

module cranium_vault() {
    // Human-like brain case with goblin compression
    hull() {
        // Frontal bone (forehead)
        translate([0, 20, 15])
            scale([0.95, 0.75, 0.7])  // Compressed for goblin look
            sphere(r=38);
        
        // Parietal bones (top/sides)
        translate([0, -5, 20])
            scale([1.0, 1.1, 0.75])
            sphere(r=36);
        
        // Occipital bone (back)
        translate([0, -30, 10])
            scale([0.85, 0.7, 0.85])
            sphere(r=32);
    }
}

module facial_skeleton() {
    hull() {
        // Maxilla (upper jaw)
        translate([0, 15, -10])
            scale([0.8, 0.7, 0.6])
            sphere(r=22);
        
        // Zygomatic bones (cheekbones) - prominent
        for(x = [-25, 25]) {
            translate([x, 10, 0])
                sphere(r=12);
        }
        
        // Nasal bridge
        translate([0, 22, 0])
            sphere(r=8);
    }
}

module mandible_lower_jaw() {
    // Protruding goblin jaw (prognathism)
    hull() {
        // Jaw body
        translate([0, 5, -28])
            scale([0.9, 1.3, 0.6])  // Extended forward
            sphere(r=22);
        
        // Chin point
        translate([0, 15, -32])
            sphere(r=8);
        
        // Ramus connection points
        for(x = [-18, 18]) {
            translate([x, -10, -22])
                sphere(r=10);
        }
    }
}

// ===== GOBLIN CHARACTER FEATURES =====

module massive_brow_ridge() {
    // Exaggerated supraorbital ridge (trademark goblin feature)
    translate([0, 24, 12])
        rotate([0, 0, 0])
        scale([1.5, 0.45, 0.65])
        difference() {
            sphere(r=18);
            // Scowl indentation
            translate([0, 5, -2])
                cube([40, 15, 10], center=true);
        }
}

module deep_eye_sockets() {
    // Sunken orbital cavities - creates menacing look
    for(x = [-26.67, 26.67]) {
        translate([x, 20, -5]) {
            // Main socket depth
            rotate([15, 0, 0])
                cylinder(r=17, h=28, center=true);
            
            // Socket rim bevel
            translate([0, 0, -10])
                cylinder(r1=19, r2=15, h=8);
            
            // Deep recess
            translate([0, 5, 0])
                sphere(r=18);
        }
    }
}

module upturned_nasal_cavity() {
    // Pig-like upturned nose (classic goblin trait)
    translate([0, 26, -2]) {
        // Nasal aperture (upturned 15 degrees)
        rotate([-15, 0, 0]) {
            // Wide nostrils
            scale([0.7, 1.0, 1.3])
                cylinder(r=10, h=18, center=true);
            
            // Septum area
            translate([0, 2, 0])
                cube([14, 12, 16], center=true);
        }
    }
}

module wide_mouth_opening() {
    // Wide gaping mouth for menacing expression
    translate([0, -8, -25]) {
        // Oral cavity
        scale([1.3, 0.9, 0.8])
            cylinder(r=20, h=12, center=true);
        
        // Dental arch
        translate([0, 3, 0])
            cube([38, 10, 10], center=true);
    }
}

module pointed_ears() {
    // Bat-like pointed ears (unmistakably goblin)
    for(x = [-1, 1]) {
        translate([x * 50, -8, 8]) {
            rotate([0, x * 25, x * 8])
                difference() {
                    // Ear cone
                    scale([0.35, 0.9, 1.6])
                        cylinder(r=12, h=30, center=true);
                    
                    // Inner depression
                    translate([x * -2, 2, 0])
                        scale([0.25, 0.7, 1.4])
                        cylinder(r=10, h=28, center=true);
                }
        }
    }
}

module cheekbone_definition() {
    // Prominent zygomatic arches
    for(x = [-1, 1]) {
        translate([x * 28, 8, -2])
            rotate([0, 0, x * -10])
            scale([0.5, 1.2, 0.7])
            sphere(r=14);
    }
}

// ===== COMPONENT MOUNTING HARDWARE =====

module component_clearances() {
    // Eye display recesses
    for(pos = [LEFT_EYE_POS, RIGHT_EYE_POS]) {
        translate(pos) {
            // Display clearance
            cylinder(r=EYE_DISPLAY_DIA/2, h=8, center=true);
            // Wire channel
            translate([0, -5, 0])
                cube([5, 10, 8], center=true);
        }
    }
    
    // Mouth display recess
    translate(MOUTH_POS) {
        cylinder(r=MOUTH_DISPLAY_DIA/2, h=8, center=true);
        // Wire channel
        translate([0, -8, 0])
            cube([5, 15, 8], center=true);
    }
    
    // Nose sensor cavity
    translate(NOSE_POS) {
        cube([SENSOR_WIDTH, SENSOR_HEIGHT, 12], center=true);
        // Sensor port (front opening)
        translate([0, SENSOR_HEIGHT/2 + 3, 0])
            cube([42, 8, 10], center=true);
    }
}

module mounting_rings() {
    // Eye mounting rings (26mm standard)
    for(pos = [LEFT_EYE_POS, RIGHT_EYE_POS]) {
        translate(pos) {
            difference() {
                cylinder(r=14.5, h=3, center=true);
                cylinder(r=13, h=4, center=true);
            }
        }
    }
    
    // Mouth mounting ring
    translate(MOUTH_POS) {
        difference() {
            cylinder(r=14.5, h=3, center=true);
            cylinder(r=13, h=4, center=true);
        }
    }
    
    // Nose mounting bracket (22x17mm standard)
    translate(NOSE_POS) {
        difference() {
            cube([SENSOR_WIDTH + 4, SENSOR_HEIGHT + 4, 8], center=true);
            cube([SENSOR_WIDTH - 2, SENSOR_HEIGHT - 2, 10], center=true);
        }
    }
}

module electronics_cavity() {
    // Internal space for wiring, ESP32, power
    translate([0, -20, -5])
        scale([0.85, 1.0, 0.9])
        hull() {
            sphere(r=32);
            translate([0, -15, -10])
                sphere(r=28);
        }
}

module wiring_channels() {
    // Channel from eyes to electronics bay
    for(x = [-26.67, 26.67]) {
        hull() {
            translate([x, 17.78, -8.89])
                cylinder(r=3, h=5, center=true);
            translate([x * 0.5, 0, -5])
                cylinder(r=3, h=5, center=true);
        }
    }
    
    // Channel from mouth to electronics bay
    hull() {
        translate([0, -26.67, 0])
            cylinder(r=3, h=5, center=true);
        translate([0, -15, -5])
            cylinder(r=3, h=5, center=true);
    }
    
    // Channel from nose to electronics bay
    hull() {
        translate([0, 0, 6.35])
            cube([5, 5, 5], center=true);
        translate([0, -10, 0])
            cube([5, 5, 5], center=true);
    }
}

// ===== STRUCTURAL REINFORCEMENT =====

module mounting_posts() {
    // Internal support posts for component mounting
    // Eye support posts
    for(pos = [LEFT_EYE_POS, RIGHT_EYE_POS]) {
        translate(pos + [0, -5, -15]) {
            cylinder(r=3, h=20);
        }
    }
    
    // Mouth support post
    translate(MOUTH_POS + [0, -5, -15]) {
        cylinder(r=3, h=20);
    }
}

// ===== FINAL ASSEMBLY =====

module complete_goblin_skull() {
    difference() {
        union() {
            // Base anatomical structure
            cranium_vault();
            facial_skeleton();
            mandible_lower_jaw();
            
            // Goblin character features (additive)
            massive_brow_ridge();
            cheekbone_definition();
            pointed_ears();
            
            // Mounting hardware (additive)
            color([0.3, 0.3, 0.3])
                mounting_rings();
        }
        
        // Remove cavities (subtractive)
        deep_eye_sockets();
        upturned_nasal_cavity();
        wide_mouth_opening();
        component_clearances();
        electronics_cavity();
        wiring_channels();
        
        // Shell out interior (weight reduction)
        translate([0, -5, 5])
            scale([0.82, 0.85, 0.88])
            union() {
                cranium_vault();
                facial_skeleton();
            }
    }
    
    // Internal structural support
    color([0.3, 0.3, 0.3])
        mounting_posts();
}

// ===== VISUALIZATION AIDS =====

module show_component_positions() {
    // Debug view - show where components mount
    color([1, 0, 0, 0.5]) {
        translate(LEFT_EYE_POS)
            cylinder(r=EYE_DISPLAY_DIA/2, h=2);
        translate(RIGHT_EYE_POS)
            cylinder(r=EYE_DISPLAY_DIA/2, h=2);
        translate(MOUTH_POS)
            cylinder(r=MOUTH_DISPLAY_DIA/2, h=2);
    }
    
    color([0, 0, 1, 0.5]) {
        translate(NOSE_POS)
            cube([SENSOR_WIDTH, SENSOR_HEIGHT, 2], center=true);
    }
}

// ===== RENDER MODES =====

// Default: Complete skull
complete_goblin_skull();

// Debug mode (uncomment to visualize component positions)
// show_component_positions();

// Section view (uncomment to see internal structure)
// difference() {
//     complete_goblin_skull();
//     translate([0, -100, -100])
//         cube([200, 200, 200]);
// }
