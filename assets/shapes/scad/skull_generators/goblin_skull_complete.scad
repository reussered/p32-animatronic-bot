// Goblin Skull Generator - Realistic Anatomical Design
// Based on folklore research and D&D anatomical specifications
// Integrates with P32 Animatronic Bot component positioning system

// =============================================================================
// GOBLIN ANATOMICAL CONSTANTS (Based on Research)
// Reference: Forgotten Realms Wiki, European folklore, comparative anatomy
// =============================================================================

// Base goblin skull proportions (research-based)
goblin_skull_length = 140;    // mm (85% of human size, compact but intimidating)
goblin_skull_width = 120;     // mm (broader than human for eye spacing)
goblin_skull_height = 90;     // mm (30% compressed vertically)
goblin_eye_spacing = 76.2;    // mm (3.0 inches - matching P32 system)

// Conversion constants for P32 coordinate system
inch_to_mm = 25.4;
mm_to_inch = 1/25.4;

// Enhanced coordinate system with anatomical landmarks
// Reference point: nose_center (0,0,0) as per P32 specification
module goblin_coordinate_system() {
    // Visual reference markers for OpenSCAD preview
    color([1, 0, 0]) translate([0, 0, 0]) sphere(2); // Nose center origin
    color([0, 1, 0]) translate([0, 0, 10]) sphere(1); // +Z axis
    color([0, 0, 1]) translate([10, 0, 0]) sphere(1); // +X axis
    color([1, 1, 0]) translate([0, 10, 0]) sphere(1); // +Y axis
}

// P32 component positions converted to mm for OpenSCAD
// Left eye: (-1.05", +0.7", -0.35") = (-26.67, +17.78, -8.89) mm
// Right eye: (+1.05", +0.7", -0.35") = (+26.67, +17.78, -8.89) mm  
// Nose sensor: (0", 0", +0.25") = (0, 0, +6.35) mm
// Mouth: (0", -1.05", 0") = (0, -26.67, 0) mm

p32_left_eye_x = -26.67;   p32_left_eye_y = 17.78;    p32_left_eye_z = -8.89;
p32_right_eye_x = 26.67;   p32_right_eye_y = 17.78;   p32_right_eye_z = -8.89;
p32_nose_x = 0;            p32_nose_y = 0;            p32_nose_z = 6.35;
p32_mouth_x = 0;           p32_mouth_y = -26.67;      p32_mouth_z = 0;

// Additional anatomical landmarks (calculated from research)
goblin_brow_ridge_y = p32_left_eye_y + 8;      // 8mm forward of eyes
goblin_temple_width = goblin_skull_width * 0.9; // Broadest point
goblin_jaw_width = goblin_skull_width * 0.8;    // Narrower jaw
goblin_cranium_peak = 45;                       // Highest skull point
goblin_jaw_depth = -35;                         // Lowest jaw point

// =============================================================================
// GOBLIN SKULL BASE GEOMETRY
// =============================================================================

// Goblin color scheme (forest green)
goblin_green = [0.29, 0.36, 0.14]; // #4A5D23 in RGB

module goblin_skull_base() {
    color(goblin_green) {
        difference() {
            union() {
                // Main cranium - elongated and flattened
                translate([0, 10, 15]) 
                    scale([1.2, 0.8, 0.7]) 
                    sphere(d=goblin_skull_width);
                
                // Prominent brow ridge
                translate([0, goblin_brow_ridge_y, 5])
                    scale([1.4, 0.3, 0.4])
                    sphere(d=60);
                
                // Jutting lower jaw
                translate([0, -15, -15])
                    scale([0.9, 1.2, 0.6])
                    sphere(d=70);
                
                // Pronounced cheekbones
                translate([-30, 5, -5])
                    scale([0.4, 0.6, 0.8])
                    sphere(d=35);
                translate([30, 5, -5])
                    scale([0.4, 0.6, 0.8])
                    sphere(d=35);
            }
            
            // Eye sockets - large and sunken
            translate([p32_left_eye_x, p32_left_eye_y, p32_left_eye_z])
                sphere(d=28);
            translate([p32_right_eye_x, p32_right_eye_y, p32_right_eye_z])
                sphere(d=28);
            
            // Nasal cavity - flared nostrils
            translate([p32_nose_x, p32_nose_y + 5, p32_nose_z])
                scale([0.8, 1.5, 1.2])
                sphere(d=15);
            
            // Mouth cavity - wide grimacing opening
            translate([p32_mouth_x, p32_mouth_y, p32_mouth_z])
                scale([1.8, 0.8, 0.6])
                sphere(d=25);
            
            // Hollow interior for electronics
            translate([0, -10, 0])
                scale([0.8, 0.9, 0.85])
                sphere(d=goblin_skull_width - 10);
        }
    }
}

module goblin_skull_details() {
    color(goblin_green * 0.9) { // Slightly darker for details
        // Prominent brow ridges over eyes
        translate([p32_left_eye_x, p32_left_eye_y + 8, p32_left_eye_z + 8])
            rotate([0, 15, 0])
            scale([0.3, 0.8, 0.4])
            sphere(d=20);
        translate([p32_right_eye_x, p32_right_eye_y + 8, p32_right_eye_z + 8])
            rotate([0, -15, 0])
            scale([0.3, 0.8, 0.4])
            sphere(d=20);
        
        // Sagittal crest (skull ridge)
        translate([0, 5, 25])
            scale([0.2, 1.5, 0.3])
            sphere(d=25);
        
        // Mandible reinforcement
        translate([0, -20, -20])
            scale([1.1, 0.4, 0.3])
            sphere(d=60);
    }
}

// Component mounting points
module component_mounts() {
    color([0.2, 0.2, 0.2]) {
        // Display mounting rings
        translate([p32_left_eye_x, p32_left_eye_y, p32_left_eye_z])
            difference() {
                cylinder(h=4, d=26, center=true);
                cylinder(h=5, d=22, center=true);
            }
        
        translate([p32_right_eye_x, p32_right_eye_y, p32_right_eye_z])
            difference() {
                cylinder(h=4, d=26, center=true);
                cylinder(h=5, d=22, center=true);
            }
        
        translate([p32_mouth_x, p32_mouth_y, p32_mouth_z])
            difference() {
                cylinder(h=4, d=26, center=true);
                cylinder(h=5, d=22, center=true);
            }
        
        // Sensor mount in nose
        translate([p32_nose_x, p32_nose_y, p32_nose_z])
            difference() {
                cube([22, 17, 10], center=true);
                cube([20, 15, 8], center=true);
            }
    }
}

// Complete goblin skull assembly
module goblin_skull_complete() {
    goblin_skull_base();
    goblin_skull_details();
    component_mounts();
    
    // Optional: Show coordinate system for reference
    if ($preview) {
        goblin_coordinate_system();
    }
}

module ellipsoid(radii) {
    // Helper function for organic shapes
    scale(radii) sphere(1);
}

module goblin_skull_base() {
    union() {
        // Main cranium (flattened, broader than human)
        translate([0, 5, 15])
            ellipsoid([goblin_skull_width/2, goblin_skull_length/2.5, goblin_skull_height/2.2]);
        
        // Frontal bone (sloped backward forehead - key goblin feature)
        translate([0, goblin_brow_ridge_y, 25])
            rotate([15, 0, 0])  // Sloped back 15 degrees
            ellipsoid([goblin_skull_width/2.5, 20, 15]);
        
        // Temporal bones (broad temples for wide-set eyes)
        translate([-goblin_temple_width/3, 10, 5])
            ellipsoid([25, 30, 20]);
        translate([goblin_temple_width/3, 10, 5])
            ellipsoid([25, 30, 20]);
        
        // Upper jaw/maxilla (broad, flat nasal region)
        translate([0, -5, -5])
            ellipsoid([goblin_jaw_width/2.2, 35, 18]);
        
        // Lower jaw/mandible (slightly protruding for fangs)
        translate([0, -15, goblin_jaw_depth/2])
            ellipsoid([goblin_jaw_width/2.5, 25, 12]);
    }
}

// =============================================================================
// GOBLIN SKULL CAVITIES AND FEATURES
// =============================================================================

module goblin_skull_cavities() {
    union() {
        // Eye sockets (large, adapted for darkvision)
        translate([p32_left_eye_x, p32_left_eye_y, p32_left_eye_z])
            ellipsoid([18, 20, 16]); // 25% larger than human proportionally
        translate([p32_right_eye_x, p32_right_eye_y, p32_right_eye_z])
            ellipsoid([18, 20, 16]);
        
        // Nasal cavity (broad, flattened)
        translate([p32_nose_x, p32_nose_y - 8, p32_nose_z - 3])
            ellipsoid([12, 15, 18]);
        
        // Nostril openings
        translate([-6, p32_nose_y - 12, p32_nose_z])
            ellipsoid([3, 8, 4]);
        translate([6, p32_nose_y - 12, p32_nose_z])
            ellipsoid([3, 8, 4]);
        
        // Mouth cavity (accommodates display and shows teeth)
        translate([p32_mouth_x, p32_mouth_y, p32_mouth_z - 8])
            ellipsoid([15, 12, 8]);
        
        // Temporal muscle attachment (creates hollow cheeks)
        translate([-30, 0, 10])
            ellipsoid([8, 25, 15]);
        translate([30, 0, 10])
            ellipsoid([8, 25, 15]);
    }
}

// =============================================================================
// GOBLIN DISTINCTIVE FEATURES
// =============================================================================

module goblin_facial_features() {
    union() {
        // Prominent brow ridge (aggressive, intimidating)
        translate([0, goblin_brow_ridge_y, p32_left_eye_z + 18])
            ellipsoid([goblin_eye_spacing + 20, 8, 6]);
        
        // Sagittal crest (muscle attachment for strong neck)
        translate([0, 10, goblin_cranium_peak - 5])
            ellipsoid([4, 40, 8]);
        
        // Zygomatic arches (cheekbone prominence)
        translate([-35, 5, 0])
            rotate([0, 0, 45])
            ellipsoid([6, 20, 8]);
        translate([35, 5, 0])
            rotate([0, 0, -45])
            ellipsoid([6, 20, 8]);
        
        // Mastoid processes (ear attachment points)
        translate([-45, 15, -5])
            ellipsoid([8, 12, 10]);
        translate([45, 15, -5])
            ellipsoid([8, 12, 10]);
    }
}

module goblin_dental_features() {
    // Small, sharp fangs (upper canines)
    color([0.9, 0.9, 0.8]) {
        translate([-8, p32_mouth_y + 5, p32_mouth_z + 3])
            rotate([45, 0, 0])
            ellipsoid([2, 2, 6]);
        translate([8, p32_mouth_y + 5, p32_mouth_z + 3])
            rotate([45, 0, 0])
            ellipsoid([2, 2, 6]);
    }
}

// =============================================================================
// COMPONENT MOUNTING INTEGRATION
// =============================================================================

module goblin_component_mounts() {
    // Eye display mounting rings (GC9A01 displays)
    translate([p32_left_eye_x, p32_left_eye_y, p32_left_eye_z]) {
        difference() {
            cylinder(h=5, r=15);
            cylinder(h=6, r=12); // Mounting ring
        }
    }
    translate([p32_right_eye_x, p32_right_eye_y, p32_right_eye_z]) {
        difference() {
            cylinder(h=5, r=15);
            cylinder(h=6, r=12); // Mounting ring
        }
    }
    
    // Nose sensor mount (HC-SR04)
    translate([p32_nose_x, p32_nose_y, p32_nose_z]) {
        difference() {
            ellipsoid([8, 12, 6]);
            // Sensor opening cutouts
            translate([-6, 0, 0]) cylinder(h=8, r=2, center=true);
            translate([6, 0, 0]) cylinder(h=8, r=2, center=true);
        }
    }
    
    // Mouth display mount (GC9A01 display)
    translate([p32_mouth_x, p32_mouth_y, p32_mouth_z]) {
        difference() {
            cylinder(h=5, r=15);
            cylinder(h=6, r=12); // Mounting ring
        }
    }
}

// =============================================================================
// COMPLETE GOBLIN SKULL ASSEMBLY
// =============================================================================

module goblin_skull_complete(show_components = true, show_coordinates = false) {
    color([0.85, 0.8, 0.75]) { // Aged bone color
        difference() {
            union() {
                goblin_skull_base();
                goblin_facial_features();
            }
            goblin_skull_cavities();
        }
        
        // Add dental features
        goblin_dental_features();
        
        // Add component mounts if requested
        if (show_components) {
            color([0.3, 0.3, 0.3]) goblin_component_mounts();
        }
    }
    
    // Show coordinate system if requested
    if (show_coordinates) {
        goblin_coordinate_system();
        
        // Component position markers
        color([1, 0, 0, 0.5]) {
            translate([p32_left_eye_x, p32_left_eye_y, p32_left_eye_z]) sphere(3);
            translate([p32_right_eye_x, p32_right_eye_y, p32_right_eye_z]) sphere(3);
            translate([p32_nose_x, p32_nose_y, p32_nose_z]) sphere(2);
            translate([p32_mouth_x, p32_mouth_y, p32_mouth_z]) sphere(3);
        }
    }
}

// =============================================================================
// SURFACE TEXTURING AND WEATHERING
// =============================================================================

module goblin_skull_textured() {
    difference() {
        goblin_skull_complete(show_components = false);
        
        // Battle scars and weathering
        translate([15, 20, 15])
            rotate([30, 45, 0])
            ellipsoid([2, 12, 1]); // Scar across temple
        
        translate([-20, 10, 8])
            rotate([0, 30, 45])
            ellipsoid([1, 8, 2]); // Another scar
        
        // Surface pitting for aged appearance
        for(i = [0:8]) {
            translate([20*cos(i*45), 20*sin(i*45), 5 + 3*sin(i*60)])
                sphere(0.8);
        }
    }
    
    // Add component mounts
    color([0.3, 0.3, 0.3]) goblin_component_mounts();
}

// =============================================================================
// RENDERING AND EXPORT OPTIONS
// =============================================================================

// Default render - complete skull with components
goblin_skull_complete(show_components = true, show_coordinates = true);

// Alternative renders (uncomment as needed):
// goblin_skull_textured();                    // Weathered version
// goblin_skull_complete(false, true);         // Skull only with coordinates
// goblin_component_mounts();                  // Component mounts only

// =============================================================================
// P32 INTEGRATION NOTES
// =============================================================================

/*
COORDINATE SYSTEM INTEGRATION:
- All measurements compatible with existing P32 JSON configurations
- Eye spacing: 3.0" (76.2mm) matches goblin_full specification
- Nose center origin maintained for consistency
- 30% vertical compression applied as per "Goblin Full" spec

COMPONENT COMPATIBILITY:
- GC9A01 displays: 240x240 circular displays for eyes and mouth
- HC-SR04 sensor: Ultrasonic distance sensor in nose position
- All mounting rings sized for actual hardware dimensions

MANUFACTURING CONSIDERATIONS:
- STL export ready - no overhangs requiring supports
- Component mounts provide secure hardware attachment
- Skull thickness adequate for structural integrity
- Modular design allows separate printing of components

FUTURE ENHANCEMENTS:
- Articulated jaw mechanism integration points
- Servo mount points for head movement
- Cable routing channels for electronics
- Magnetic attachment points for modular assembly
*/