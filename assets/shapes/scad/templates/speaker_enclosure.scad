// Speaker Enclosure Template for I2S Audio Modules
// Parametric enclosure for directional sound output

module speaker_enclosure(diameter, depth, port_diameter, position) {
    translate(position) {
        difference() {
            // Main cylindrical enclosure
            cylinder(h=depth, d=diameter);
            
            // Hollow interior (leaving wall thickness)
            translate([0, 0, 2])
                cylinder(h=depth-2, d=diameter-4);
            
            // Speaker driver cutout
            translate([0, 0, -1])
                cylinder(h=3, d=diameter*0.8);
            
            // Sound port
            translate([0, 0, depth-port_diameter/2])
                cylinder(h=port_diameter/2+1, d=port_diameter);
            
            // Mounting holes for speaker
            for(i = [0:3]) {
                rotate([0, 0, i*90]) {
                    translate([diameter*0.3, 0, -1])
                        cylinder(h=depth+2, d=3.0);
                }
            }
            
            // Cable entry hole
            translate([diameter*0.4, 0, depth*0.5])
                rotate([0, 90, 0])
                    cylinder(h=diameter*0.2, d=6);
        }
        
        // Internal sound baffles
        for(i = [0:2]) {
            rotate([0, 0, i*120]) {
                translate([diameter*0.15, 0, depth*0.3])
                    cube([2, diameter*0.6, depth*0.4], center=true);
            }
        }
        
        // Mounting flanges
        for(i = [0:1]) {
            rotate([0, 0, i*180]) {
                translate([diameter*0.4, 0, depth*0.8])
                    difference() {
                        cylinder(h=3, d=8);
                        translate([0, 0, -1])
                            cylinder(h=5, d=3.2);
                    }
            }
        }
    }
}

// Example usage for goblin speaker
// speaker_enclosure(38.1, 7.62, 12, [-12.7, 12.7, -25.4]);