// Display Mount Template for GC9A01 Circular Displays
// Parametric mounting bracket for animatronic face displays

module display_mount(diameter, thickness, mount_holes, hole_diameter, hole_spacing, tilt_angle, position) {
    translate(position) {
        rotate([tilt_angle, 0, 0]) {
            difference() {
                // Main mounting ring with reinforcement
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
    }
}

// Example usage for goblin eye
// display_mount(32.512, 2.032, 4, 2.54, 27.94, 15, [-26.67, 17.78, -8.89]);