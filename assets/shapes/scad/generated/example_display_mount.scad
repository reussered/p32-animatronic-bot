// Example STL Generator for Testing
// Creates a simple display mount for testing the STL viewer

// Parameters for GC9A01 display mount
diameter = 32.512;  // 1.28 inches in mm
thickness = 2.032;  // 0.08 inches in mm
mount_holes = 4;
hole_diameter = 2.54; // 0.1 inches in mm
hole_spacing = 27.94;  // 1.1 inches in mm

module example_display_mount() {
    difference() {
        // Main mounting ring
        union() {
            cylinder(h=thickness*2, d=diameter+5);
            // Reinforcement ring
            cylinder(h=thickness*0.5, d=diameter+8);
        }
        
        // Display cutout
        translate([0,0,-1]) 
            cylinder(h=thickness*2+2, d=diameter);
        
        // Mounting holes for display
        for(i=[0:mount_holes-1]) {
            rotate([0, 0, i*(360/mount_holes)]) {
                translate([hole_spacing/2, 0, -1])
                    cylinder(h=thickness*2+2, d=hole_diameter);
            }
        }
        
        // Case mounting holes (M3 screws)
        for(i=[0:3]) {
            rotate([0, 0, i*90 + 45]) {
                translate([(diameter+6)/2, 0, thickness])
                    cylinder(h=thickness+1, d=3.2);
            }
        }
    }
    
    // Cable management channel
    translate([diameter/2 + 2, 0, thickness*0.5])
        cube([3, 2, thickness], center=true);
}

// Generate the example mount
example_display_mount();