// Basic Display Mount - Generic GC9A01 Mounting Bracket
// Reusable mounting solution for 1.28" circular displays
// Multi-Color 3D Printing: Uses HTML named colors for 4-color system

// Color palette parameters (can be overridden via command line)
PRIMARY_COLOR = "DarkOliveGreen";      // Color 1: Main mounting ring
SECONDARY_COLOR = "SaddleBrown";       // Color 2: Reinforcement tabs  
ACCENT_COLOR = "Goldenrod";            // Color 3: (unused in basic mount)
HIGHLIGHT_COLOR = "OrangeRed";         // Color 4: (unused in basic mount)

module display_basic_mount() {
    diameter = 32.512;
    thickness = 2.032;
    mount_holes = 4;
    hole_diameter = 2.54;
    hole_spacing = 27.94;
    
    difference() {
        // Main mounting ring with reinforcement tabs
        union() {
            cylinder(h=thickness*3, d=diameter+8);
            // Reinforcement tabs for case mounting
            for(i=[0:3]) {
                rotate([0, 0, i*90 + 45]) {
                    translate([(diameter+8)/2, 0, 0])
                        cylinder(h=thickness*2, d=6);
                }
            }
        }
        
        // Display cutout (tight fit)
        translate([0,0,-1]) 
            cylinder(h=thickness*3+2, d=diameter+0.2);
        
        // Display mounting holes
        for(i=[0:mount_holes-1]) {
            rotate([0, 0, i*(360/mount_holes)]) {
                translate([hole_spacing/2, 0, -1])
                    cylinder(h=thickness*3+2, d=hole_diameter);
            }
        }
        
        // Case mounting holes (M3 screws in reinforcement tabs)
        for(i=[0:3]) {
            rotate([0, 0, i*90 + 45]) {
                translate([(diameter+8)/2, 0, thickness*2.5])
                    cylinder(h=thickness+1, d=3.2);
                // Countersink
                translate([(diameter+8)/2, 0, thickness*2.5])
                    cylinder(h=2, d=6);
            }
        }
        
        // Cable management channel
        translate([diameter/2 + 4, 0, thickness])
            cube([4, 3, thickness*2], center=true);
    }
}

// Generate the mount with primary color
color(PRIMARY_COLOR) display_basic_mount();
