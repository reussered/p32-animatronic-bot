// CLEAN Goblin Skull - No Z-Fighting, Proper Hulling
// Fixes: Splotchy rendering caused by overlapping geometry
// Design: Single hull() operations, proper additive/subtractive order

// P32 Component Mounting Positions (in mm from nose center)
LEFT_EYE_POS = [-26.67, 17.78, -8.89];    // Left eye display (GC9A01)
RIGHT_EYE_POS = [26.67, 17.78, -8.89];    // Right eye display (GC9A01)
NOSE_POS = [0, 0, 6.35];                  // Nose sensor (HC-SR04)
MOUTH_POS = [0, -26.67, 0];               // Mouth display (GC9A01)

// Display/sensor dimensions (ACTUAL HARDWARE MEASUREMENTS)
// GC9A01: Total PCB is 32.5mm dia, visible screen is 27.9mm dia
EYE_DISPLAY_DIA = 32.5;    // GC9A01 PCB outer diameter (1.28")
EYE_SCREEN_DIA = 27.9;     // GC9A01 visible screen diameter
MOUTH_DISPLAY_DIA = 32.5;  // GC9A01 PCB outer diameter
MOUTH_SCREEN_DIA = 27.9;   // GC9A01 visible screen diameter
SENSOR_WIDTH = 45;         // HC-SR04 width
SENSOR_HEIGHT = 20;        // HC-SR04 height

// Rendering quality
$fn = 100;  // High quality for final render (was 60 for preview)

// Wall thickness for hollow skull
WALL_THICKNESS = 3;  // 3mm shell thickness

// Angular sharpness control (lower = sharper edges)
CURVE_SMOOTHNESS = 12;  // REDUCED from 20 for much sharper angular goblin features

// ===== SINGLE-HULL SKULL BASE (NO OVERLAPS) =====

module solid_skull_exterior() {
    // ONE continuous hull - no overlapping surfaces
    hull() {
        // Forehead (frontal bone)
        translate([0, 22, 12])
            scale([0.95, 0.7, 0.65])  // Compressed for goblin
            sphere(r=35);
        
        // Top of skull (parietal)
        translate([0, -5, 22])
            scale([1.0, 1.0, 0.7])
            sphere(r=34);
        
        // Back of skull (occipital)
        translate([0, -32, 8])
            scale([0.85, 0.65, 0.8])
            sphere(r=30);
        
        // Upper jaw (maxilla) - forward projection
        translate([0, 18, -12])
            scale([0.75, 0.65, 0.55])
            sphere(r=20);
        
        // Lower jaw (mandible) - protruding goblin chin
        translate([0, 12, -30])
            scale([0.85, 1.2, 0.6])
            sphere(r=20);
        
        // Cheekbones (zygomatic) - left and right
        translate([-26, 12, -2])
            sphere(r=11);
        translate([26, 12, -2])
            sphere(r=11);
        
        // Jaw hinges (temporal)
        translate([-20, -8, -18])
            sphere(r=9);
        translate([20, -8, -18])
            sphere(r=9);
        
        // EAR ATTACHMENT POINTS (connect ears to skull)
        // Left ear base
        translate([-48, -5, 10])
            sphere(r=6);
        
        // Right ear base
        translate([48, -5, 10])
            sphere(r=6);
    }
}

module solid_brow_ridge() {
    // Massive goblin brow - SHARP angular ridge for aggressive look
    translate([0, 25, 10])
        rotate([10, 0, 0])
        scale([1.4, 0.4, 0.6])
        sphere(r=16, $fn=CURVE_SMOOTHNESS);  // Sharp angular facets
}

module solid_pointed_ears() {
    // Bat-like goblin ears
    for(x = [-1, 1]) {
        translate([x * 48, -5, 10]) {
            rotate([0, x * 22, x * 12])
                hull() {
                    sphere(r=6);
                    translate([0, 0, 24])
                        sphere(r=3);
                }
        }
    }
}

// ===== SUBTRACTIVE FEATURES (CAVITIES) =====

module eye_sockets() {
    // Deep sunken eye sockets for menacing look
    for(x = [-26.67, 26.67]) {
        translate([x, 22, -6]) {
            rotate([12, 0, 0])
                scale([1.0, 1.2, 1.0])
                sphere(r=17);
        }
    }
}

module nasal_cavity() {
    // Upturned pig-like goblin nose
    translate([0, 28, -3]) {
        rotate([-12, 0, 0])
            scale([0.65, 0.9, 1.2])
            cylinder(r=9, h=16, center=true);
    }
}

module mouth_cavity() {
    // Wide gaping mouth opening
    translate([0, 0, -26]) {
        rotate([8, 0, 0])
            scale([1.25, 0.85, 0.75])
            sphere(r=18);
    }
}

module interior_hollowing() {
    // Shell out interior for weight reduction
    // Offset INWARD from exterior by WALL_THICKNESS
    offset_scale = (1 - WALL_THICKNESS / 35);  // Proportional scaling
    
    translate([0, 0, -3])  // Shift slightly down
        scale([offset_scale, offset_scale, offset_scale])
        solid_skull_exterior();
}

// ===== COMPONENT MOUNTING HARDWARE =====

module component_mounting_recesses() {
    // GC9A01 Eye displays - SMALL circular holes (just screen visible, not full PCB)
    for(pos = [LEFT_EYE_POS, RIGHT_EYE_POS]) {
        translate(pos) {
            // Hole for visible screen only (27.9mm + small clearance)
            rotate([90, 0, 0])
                cylinder(r=EYE_SCREEN_DIA/2 + 0.5, h=15, center=false);
            
            // Wire routing channel (drops down into interior)
            translate([0, 5, -8])
                cube([6, 10, 16], center=true);
        }
    }
    
    // GC9A01 Mouth display - SMALL circular hole (just screen visible)
    translate(MOUTH_POS) {
        // Hole for visible screen only (27.9mm + small clearance)
        rotate([90, 0, 0])
            cylinder(r=MOUTH_SCREEN_DIA/2 + 0.5, h=15, center=false);
        
        // Wire routing channel (drops down into interior)
        translate([0, 5, -8])
            cube([6, 10, 16], center=true);
    }
    
    // HC-SR04 Nose sensor - RECTANGULAR hole for sensor "eyes"
    translate(NOSE_POS) {
        // Main sensor body cavity (rectangular box)
        cube([SENSOR_WIDTH + 1, SENSOR_HEIGHT + 1, 12], center=true);
        
        // Front opening for ultrasonic "eyes" (two circular transducers)
        translate([0, -SENSOR_HEIGHT/2 - 1, 0]) {
            // Two circular openings for ultrasonic transducers (16mm dia each)
            translate([-10, 0, 0])
                rotate([90, 0, 0])
                cylinder(r=8.5, h=6, center=true);
            translate([10, 0, 0])
                rotate([90, 0, 0])
                cylinder(r=8.5, h=6, center=true);
        }
        
        // Wire routing channel
        translate([0, 8, -6])
            cube([6, 16, 12], center=true);
    }
}

module component_mounting_rings() {
    // Physical mounting rings for GC9A01 displays
    // Screen visible through center, PCB GLUED to ring surface (no screws)
    ring_height = 3;
    ring_thickness = 3;  // Wide flat surface for glue bonding
    
    // GC9A01 Eye mounting rings (circular, facing forward)
    for(pos = [LEFT_EYE_POS, RIGHT_EYE_POS]) {
        translate(pos + [0, 1, 0]) {
            rotate([90, 0, 0])
            difference() {
                // Outer ring (wide flat surface for gluing PCB)
                cylinder(r=EYE_DISPLAY_DIA/2 + ring_thickness, 
                    h=ring_height, center=true);
                
                // Inner hole (screen shows through) - matches screen size
                cylinder(r=EYE_SCREEN_DIA/2 + 0.3, 
                    h=ring_height + 1, center=true);
                
                // NO SCREW HOLES - glue bond only
            }
        }
    }
    
    // GC9A01 Mouth mounting ring (circular, facing forward)
    translate(MOUTH_POS + [0, 1, 0]) {
        rotate([90, 0, 0])
        difference() {
            // Outer ring (wide flat surface for gluing PCB)
            cylinder(r=MOUTH_DISPLAY_DIA/2 + ring_thickness, 
                h=ring_height, center=true);
            
            // Inner hole (screen shows through) - matches screen size
            cylinder(r=MOUTH_SCREEN_DIA/2 + 0.3, 
                h=ring_height + 1, center=true);
            
            // NO SCREW HOLES - glue bond only
        }
    }
    
    // HC-SR04 Nose mounting bracket (22x17mm standard)
    translate(NOSE_POS + [0, 1, 0]) {
        rotate([90, 0, 0])
        difference() {
            // Outer bracket frame (wide flat surface for gluing)
            cube([SENSOR_WIDTH + 6, SENSOR_HEIGHT + 6, ring_height], center=true);
            
            // Inner clearance for sensor body
            cube([SENSOR_WIDTH - 2, SENSOR_HEIGHT - 2, ring_height + 1], center=true);
            
            // NO SCREW HOLES - glue bond only
        }
    }
}

module wiring_channels() {
    // Internal channels for wire routing
    channel_dia = 4;
    
    // Eye to electronics bay
    for(x = [-26.67, 26.67]) {
        hull() {
            translate([x, 17.78, -8.89])
                sphere(r=channel_dia/2);
            translate([x * 0.6, 0, -8])
                sphere(r=channel_dia/2);
            translate([0, -15, -8])
                sphere(r=channel_dia/2);
        }
    }
    
    // Mouth to electronics bay
    hull() {
        translate([0, -26.67, 0])
            sphere(r=channel_dia/2);
        translate([0, -20, -6])
            sphere(r=channel_dia/2);
    }
    
    // Nose to electronics bay
    hull() {
        translate([0, 0, 6.35])
            sphere(r=channel_dia/2);
        translate([0, -10, 0])
            sphere(r=channel_dia/2);
    }
}

module electronics_bay() {
    // Large cavity in rear for ESP32, wiring
    translate([0, -22, -6])
        scale([0.8, 0.9, 0.85])
        sphere(r=28);
}

module neck_mounting_platform() {
    // Flat platform at base of skull for gluing to neck servo
    // No screw holes - plastic cement bond
    translate([0, 0, -38])
        cylinder(r=26, h=4, center=true);
}

// ===== FINAL ASSEMBLY =====

module clean_goblin_skull() {
    difference() {
        // ADDITIVE PARTS (solid positive geometry)
        union() {
            // Main skull shape
            solid_skull_exterior();
            
            // Brow ridge (attached to exterior)
            solid_brow_ridge();
            
            // Ears (attached to exterior)
            solid_pointed_ears();
            
            // NO mounting rings here - they create recesses instead
        }
        
        // SUBTRACTIVE PARTS (negative space)
        union() {
            // Facial cavities
            eye_sockets();
            nasal_cavity();
            mouth_cavity();
            
            // Component mounting (RECESSED surfaces)
            component_mounting_recesses();
            
            // Internal hollowing (weight reduction)
            interior_hollowing();
            
            // Wire routing
            wiring_channels();
            
            // Electronics bay
            electronics_bay();
        }
        
        // ADDITIVE: Neck mounting platform (bottom of skull)
        neck_mounting_platform();
    }
}

// ===== DEBUG VISUALIZATION =====

module show_component_positions() {
    // Visual guide for component placement (transparent overlays)
    
    color([1, 0, 0, 0.4]) {
        // Eyes
        translate(LEFT_EYE_POS)
            cylinder(r=EYE_DISPLAY_DIA/2, h=2);
        translate(RIGHT_EYE_POS)
            cylinder(r=EYE_DISPLAY_DIA/2, h=2);
        
        // Mouth
        translate(MOUTH_POS)
            cylinder(r=MOUTH_DISPLAY_DIA/2, h=2);
    }
    
    color([0, 0, 1, 0.4]) {
        // Nose sensor
        translate(NOSE_POS)
            cube([SENSOR_WIDTH, SENSOR_HEIGHT, 2], center=true);
    }
}

module cross_section_view() {
    // Cut skull in half to see internal structure
    difference() {
        clean_goblin_skull();
        translate([0, -100, -50])
            cube([200, 200, 200]);
    }
}

// ===== RENDER MODES =====

// NORMAL MODE: Full skull
clean_goblin_skull();

// DEBUG MODE: Show component positions (uncomment)
// #show_component_positions();

// SECTION VIEW: See interior structure (uncomment)
// cross_section_view();

// WIREFRAME MODE: Set View > Show Edges in OpenSCAD

// ===== PRINT NOTES =====
// 
// Print Settings:
// - Layer Height: 0.2mm
// - Infill: 15% (hollow interior already provides structure)
// - Supports: AUTO (for brow overhang and ears)
// - Orientation: Upright (skull facing up, chin down)
// - Material: PLA or PETG
// - Print Time: ~8-12 hours depending on size
// 
// Assembly with Plastic Glue (NO SCREWS):
// - Displays: Apply cyanoacrylate (super glue) to mounting rings
// - Sensor: Apply plastic cement to nose bracket frame
// - Neck: Apply plastic cement to base platform, press onto servo horn
// 
// Recommended Adhesives:
// - Gorilla Super Glue Gel (thick, won't run)
// - Testors Plastic Cement (melts PLA/PETG for permanent bond)
// - Loctite Ultra Gel Control (precise application)
// 
// Post-Processing:
// - Remove supports carefully from eye sockets
// - Sand mounting ring surfaces flat for good glue contact
// - Test-fit all components BEFORE gluing (glue is permanent!)
// - Hold displays in place 30 seconds while glue sets
//
