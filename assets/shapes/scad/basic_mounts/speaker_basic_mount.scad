// Basic Speaker Mount - Generic 40mm Speaker Mounting Bracket
// Reusable mounting solution for round speakers

module speaker_basic_mount() {
    diameter = 40;
    depth = 20;
    mount_holes = 4;
    hole_diameter = 3;
    hole_spacing = 35;
    
    difference() {
        // Main mounting cylinder with back reinforcement
        union() {
            cylinder(h=depth*0.8, d=diameter+10);
            // Back reinforcement ring
            cylinder(h=depth*0.3, d=diameter+16);
        }
        
        // Speaker cutout
        translate([0,0,depth*0.3]) 
            cylinder(h=depth*0.5+1, d=diameter+0.5);
        
        // Speaker mounting holes
        for(i=[0:mount_holes-1]) {
            rotate([0, 0, i*(360/mount_holes)]) {
                translate([hole_spacing/2, 0, -1])
                    cylinder(h=depth*0.8+2, d=hole_diameter);
            }
        }
        
        // Case mounting holes (M3 screws)
        for(i=[0:5]) {
            rotate([0, 0, i*60]) {
                translate([(diameter+14)/2, 0, depth*0.15])
                    cylinder(h=depth*0.2+1, d=3.2);
            }
        }
        
        // Audio port (center back)
        translate([0,0,-1])
            cylinder(h=depth*0.3+2, d=diameter*0.6);
        
        // Wire slot
        translate([diameter/2 + 5, 0, depth*0.15])
            cube([4, 3, depth*0.3], center=true);
    }
}

// Generate the mount
speaker_basic_mount();
