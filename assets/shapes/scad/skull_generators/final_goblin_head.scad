// DEFINITIVE Goblin Skull Generator
// Creates an unmistakably goblin-shaped head

$fn = 60; // High resolution

// P32 positions in mm
LEFT_EYE = [-26.67, 17.78, -8.89];
RIGHT_EYE = [26.67, 17.78, -8.89];
NOSE = [0, 0, 6.35];
MOUTH = [0, -26.67, 0];

module goblin_head_final() {
    // GOBLIN SKULL FRAMEWORK - provides mounting points for detailed components
    difference() {
        // Main goblin skull shape - defines creature silhouette only
        union() {
            // Elongated cranium (flatter, longer than human)
            translate([0, -10, 10])
                scale([1.0, 1.4, 0.7])
                ellipsoid([35, 35, 35]);
            
            // MASSIVE brow ridge - key goblin characteristic
            translate([0, 25, 5])
                scale([1.6, 0.5, 0.8])
                ellipsoid([30, 15, 12]);
            
            // Protruding lower jaw structure - underbite framework
            translate([0, -35, -25])
                scale([0.8, 1.5, 0.9])
                ellipsoid([28, 20, 15]);
            
            // Sharp cheekbone definition
            translate([-32, 5, -8])
                rotate([0, 0, -20])
                scale([0.6, 1.0, 1.2])
                ellipsoid([12, 18, 15]);
            translate([32, 5, -8])
                rotate([0, 0, 20])
                scale([0.6, 1.0, 1.2])
                ellipsoid([12, 18, 15]);
            
            // Ear mounting areas - NOTE: Detailed ear shape comes from ear components
            translate([-48, -8, 2])
                rotate([0, -35, -15])
                scale([0.4, 1.2, 2.5])
                ellipsoid([8, 12, 15]);
            translate([48, -8, 2])
                rotate([0, 35, 15])
                scale([0.4, 1.2, 2.5])
                ellipsoid([8, 12, 15]);
            
            // Skull ridge (sagittal crest) - goblin cranium characteristic
            translate([0, -5, 30])
                scale([0.2, 2.0, 0.5])
                ellipsoid([5, 25, 8]);
        }
        
        // MOUNTING CLEARANCES ONLY - detailed features come from components
        
        // Eye component clearance (GC9A01 displays + goblin eye shells attach here)
        translate(LEFT_EYE)
            cylinder(r=13, h=8, center=true);
        translate(RIGHT_EYE)
            cylinder(r=13, h=8, center=true);
        
        // Nose component clearance (HC-SR04 sensor + goblin nose shell attaches here) 
        translate(NOSE)
            cube([22, 17, 10], center=true);
        
        // Mouth component clearance (GC9A01 display + goblin mouth shell attaches here)
        translate(MOUTH)
            cylinder(r=12, h=8, center=true);
        
        // Interior hollow for electronics
        translate([0, -8, 0])
            scale([0.8, 0.85, 0.9])
            sphere(r=32);
    }
    
    // UNIVERSAL P32 MOUNTING SYSTEM - Compatible with all component types
    color([0.2, 0.2, 0.2]) {
        // Standard eye component mounting rings (26mm OD, 22mm ID per spec)
        translate(LEFT_EYE)
            difference() {
                cylinder(h=4, r=13, center=true);   // 26mm diameter mounting ring
                cylinder(h=5, r=11, center=true);   // 22mm clearance for GC9A01
            }
        translate(RIGHT_EYE)
            difference() {
                cylinder(h=4, r=13, center=true);   
                cylinder(h=5, r=11, center=true);   
            }
        
        // Standard mouth component mounting ring (26mm OD, 22mm ID per spec)
        translate(MOUTH)
            difference() {
                cylinder(h=4, r=13, center=true);   
                cylinder(h=5, r=11, center=true);   
            }
        
        // Standard nose component mounting bracket (22x17mm per spec)
        translate(NOSE)
            difference() {
                cube([22, 17, 10], center=true);    // Sensor bracket outer
                cube([20, 15, 8], center=true);     // HC-SR04 clearance
            }
        
        // Ear component mounting points (creature-specific positioning)
        translate([-48, -8, 2])
            difference() {
                sphere(r=8);
                sphere(r=6);
            }
        translate([48, -8, 2])
            difference() {
                sphere(r=8);
                sphere(r=6);
            }
    }
}

// Helper module for ellipsoids
module ellipsoid(size) {
    scale(size) sphere(r=1);
}

// Generate the final goblin head
goblin_head_final();