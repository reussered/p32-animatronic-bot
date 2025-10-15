// Anatomical Goblin Skull - Based on Real Skull Bone Structure
// Proportionally accurate skull with goblin characteristics
// Uses P32 coordinate system with nose_center as (0,0,0)

// Component positioning from JSON configs (converted to OpenSCAD variables)
// Left Eye: (-1.05 INCH, +0.7 INCH, -0.35 INCH) = (-26.67, 17.78, -8.89) mm
// Right Eye: (+1.05 INCH, +0.7 INCH, -0.35 INCH) = (26.67, 17.78, -8.89) mm  
// Nose: (0 INCH, 0 INCH, +0.25 INCH) = (0, 0, 6.35) mm
// Mouth: (0 INCH, -1.05 INCH, 0 INCH) = (0, -26.67, 0) mm

// Skull proportions based on 3" eye spacing (goblin full specification)
eye_spacing = 76.2;  // 3.0 INCH in mm
skull_width = eye_spacing * 1.4;  // 106.68mm - wider than human
skull_length = eye_spacing * 1.3;  // 99.06mm - shorter than human (compressed)  
skull_height = eye_spacing * 1.1;  // 83.82mm - lower than human
cranium_thickness = 4.0;  // Wall thickness for 3D printing

// Goblin characteristics
goblin_brow_ridge_height = 8.0;
goblin_jaw_prognathism = 15.0;  // Forward-jutting jaw
goblin_cranial_ridges = true;
goblin_ear_point_height = 12.0;

module anatomical_goblin_skull() {
    difference() {
        union() {
            // Main cranium (skull vault)
            cranium_vault();
            
            // Facial skeleton
            facial_bones();
            
            // Goblin-specific features
            goblin_characteristics();
            
            // Component mounting reinforcements
            component_mounting_reinforcements();
        }
        
        // Subtract internal cavities and mounting holes
        union() {
            // Brain cavity
            brain_cavity();
            
            // Component mounting cavities
            eye_socket_cavities();
            nose_sensor_cavity();
            mouth_display_cavity();
            
            // Cable management channels
            internal_wiring_channels();
            
            // Access panels
            rear_access_panel();
        }
    }
}

module cranium_vault() {
    // Anatomically accurate skull vault with goblin proportions
    
    // Frontal bone (forehead)
    translate([0, skull_length*0.2, skull_height*0.3]) {
        scale([skull_width*0.9, skull_length*0.4, skull_height*0.4])
            sphere(d=skull_width*0.7);
    }
    
    // Parietal bones (top and sides of skull)
    translate([0, 0, skull_height*0.5]) {
        scale([skull_width*0.8, skull_length*0.7, skull_height*0.5])
            sphere(d=skull_width*0.8);
    }
    
    // Occipital bone (back of skull)
    translate([0, -skull_length*0.3, skull_height*0.2]) {
        scale([skull_width*0.7, skull_length*0.4, skull_height*0.6])
            sphere(d=skull_width*0.6);
    }
    
    // Temporal bones (sides, around ears)
    for(side = [-1, 1]) {
        translate([side * skull_width*0.4, -skull_length*0.1, skull_height*0.1]) {
            scale([skull_width*0.3, skull_length*0.4, skull_height*0.4])
                sphere(d=skull_width*0.4);
        }
    }
}

module facial_bones() {
    // Anatomical facial bone structure
    
    // Maxilla (upper jaw)
    translate([0, skull_length*0.3, -skull_height*0.1]) {
        scale([skull_width*0.6, skull_length*0.3, skull_height*0.3])
            sphere(d=skull_width*0.5);
    }
    
    // Mandible (lower jaw) - with goblin prognathism
    translate([0, skull_length*0.2 + goblin_jaw_prognathism, -skull_height*0.3]) {
        scale([skull_width*0.7, skull_length*0.3, skull_height*0.25])
            sphere(d=skull_width*0.6);
    }
    
    // Nasal bones (bridge of nose)
    translate([0, skull_length*0.35, skull_height*0.1]) {
        scale([skull_width*0.15, skull_length*0.2, skull_height*0.2])
            sphere(d=skull_width*0.2);
    }
    
    // Zygomatic bones (cheekbones)
    for(side = [-1, 1]) {
        translate([side * skull_width*0.3, skull_length*0.25, skull_height*0.05]) {
            scale([skull_width*0.2, skull_length*0.25, skull_height*0.2])
                sphere(d=skull_width*0.3);
        }
    }
}

module goblin_characteristics() {
    // Distinctive goblin skull features
    
    // Prominent brow ridges
    for(side = [-1, 1]) {
        translate([side * eye_spacing*0.4, skull_length*0.3, skull_height*0.2]) {
            scale([skull_width*0.2, skull_length*0.15, goblin_brow_ridge_height])
                sphere(d=skull_width*0.25);
        }
    }
    
    // Central brow ridge (unibrow effect)
    translate([0, skull_length*0.32, skull_height*0.25]) {
        scale([skull_width*0.4, skull_length*0.12, goblin_brow_ridge_height*0.7])
            sphere(d=skull_width*0.2);
    }
    
    // Sagittal ridge (top of skull)
    if (goblin_cranial_ridges) {
        translate([0, 0, skull_height*0.7]) {
            scale([skull_width*0.1, skull_length*0.8, skull_height*0.1])
                sphere(d=skull_width*0.15);
        }
    }
    
    // Pointed ear attachment points
    for(side = [-1, 1]) {
        translate([side * skull_width*0.45, -skull_length*0.05, skull_height*0.15 + goblin_ear_point_height]) {
            scale([skull_width*0.1, skull_length*0.15, skull_height*0.2])
                sphere(d=skull_width*0.12);
        }
    }
    
    // Enlarged canine root bulges (upper jaw)
    for(side = [-1, 1]) {
        translate([side * skull_width*0.15, skull_length*0.4, -skull_height*0.05]) {
            scale([skull_width*0.1, skull_length*0.1, skull_height*0.15])
                sphere(d=skull_width*0.12);
        }
    }
}

module component_mounting_reinforcements() {
    // Reinforced mounting areas for animatronic components
    
    // Eye socket reinforcement rings (26mm diameter per spec)
    for(side = [-1, 1]) {
        translate([side * eye_spacing*0.5, skull_length*0.28, skull_height*0.15]) {
            difference() {
                cylinder(h=8, d=32);  // 26mm + 6mm reinforcement
                cylinder(h=10, d=26, center=true);  // Standard eye ring
            }
        }
    }
    
    // Mouth reinforcement ring
    translate([0, skull_length*0.35, -skull_height*0.15]) {
        difference() {
            cylinder(h=6, d=28);  // Mouth display mounting
            cylinder(h=8, d=20, center=true);
        }
    }
    
    // Nose sensor bracket mount (22x17mm per spec)
    translate([0, skull_length*0.4, skull_height*0.1]) {
        difference() {
            cube([28, 23, 6], center=true);
            cube([22, 17, 8], center=true);
        }
    }
    
    // Ear mounting points (for speaker shells)
    for(side = [-1, 1]) {
        translate([side * skull_width*0.42, -skull_length*0.1, skull_height*0.15]) {
            difference() {
                sphere(d=20);
                sphere(d=14);
            }
        }
    }
}

module brain_cavity() {
    // Realistic brain cavity shape
    translate([0, -skull_length*0.05, skull_height*0.2]) {
        scale([skull_width*0.7, skull_length*0.6, skull_height*0.6])
            sphere(d=skull_width*0.75);
    }
}

module eye_socket_cavities() {
    // Anatomical eye socket (orbital) cavities
    for(side = [-1, 1]) {
        translate([side * eye_spacing*0.5, skull_length*0.28, skull_height*0.15]) {
            // Conical eye socket
            rotate([15, 0, 0]) {
                cylinder(h=25, d1=30, d2=20);
                
                // Optic canal (for wiring)
                translate([0, 0, 20])
                    cylinder(h=15, d=8);
            }
        }
    }
}

module nose_sensor_cavity() {
    // HC-SR04 sensor mounting cavity
    translate([0, skull_length*0.4, skull_height*0.1]) {
        // Sensor body cavity
        cube([50, 25, 20], center=true);
        
        // Transducer holes
        translate([-12, 0, 0])
            cylinder(h=25, d=16, center=true);
        translate([12, 0, 0])
            cylinder(h=25, d=16, center=true);
    }
}

module mouth_display_cavity() {
    // GC9A01 display mounting cavity
    translate([0, skull_length*0.35, -skull_height*0.15]) {
        cylinder(h=15, d=35);
        
        // Display wiring channel
        translate([0, -10, 0])
            cube([6, 20, 15], center=true);
    }
}

module internal_wiring_channels() {
    // Main wiring harness from components to neck
    
    // Central channel (nose to neck)
    translate([0, skull_length*0.2, 0]) {
        cube([8, skull_length*0.8, 8], center=true);
    }
    
    // Eye wiring channels
    for(side = [-1, 1]) {
        hull() {
            translate([side * eye_spacing*0.5, skull_length*0.28, skull_height*0.15])
                sphere(d=6);
            translate([0, skull_length*0.1, 0])
                sphere(d=6);
        }
    }
    
    // Mouth wiring channel
    hull() {
        translate([0, skull_length*0.35, -skull_height*0.15])
            sphere(d=6);
        translate([0, skull_length*0.1, 0])
            sphere(d=6);
    }
}

module rear_access_panel() {
    // Removable rear panel for maintenance
    translate([0, -skull_length*0.45, skull_height*0.1]) {
        scale([skull_width*0.5, cranium_thickness*2, skull_height*0.6])
            cube([skull_width*0.5, cranium_thickness*2, skull_height*0.6], center=true);
    }
    
    // Panel screw holes
    for(x = [-20, 20]) {
        for(z = [-15, 15]) {
            translate([x, -skull_length*0.45, skull_height*0.1 + z])
                rotate([90, 0, 0])
                    cylinder(h=cranium_thickness*3, d=3.2, center=true);
        }
    }
}

// Generate the skull
anatomical_goblin_skull();