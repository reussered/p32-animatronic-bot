// Basic Sensor Mount - Generic HC-SR04 Mounting Bracket
// Reusable mounting solution for ultrasonic proximity sensors

module sensor_basic_mount() {
    width = 45;
    height = 20;
    depth = 15;
    trans_diameter = 16;
    trans_spacing = 24;
    
    difference() {
        // Main bracket body with mounting flanges
        union() {
            // Central body
            hull() {
                for(x = [3, width-3]) {
                    for(y = [3, height-3]) {
                        translate([x, y, 0])
                            cylinder(h=depth, d=6);
                    }
                }
            }
            // Mounting flanges
            for(i = [0:1]) {
                translate([width*0.1 + i*width*0.8, height*0.5, depth*0.5])
                    rotate([0, 90, 0])
                        cylinder(h=8, d=depth*1.2, center=true);
            }
        }
        
        // Sensor transducer holes
        translate([width*0.5 - trans_spacing*0.5, height*0.5, -1])
            cylinder(h=depth+2, d=trans_diameter); // Left transducer
        translate([width*0.5 + trans_spacing*0.5, height*0.5, -1])  
            cylinder(h=depth+2, d=trans_diameter); // Right transducer
            
        // PCB mounting holes
        for(i = [0:1]) {
            for(j = [0:1]) {
                translate([width*0.2 + i*width*0.6, height*0.2 + j*height*0.6, -1])
                    cylinder(h=depth+2, d=2.5);
            }
        }
        
        // Case mounting holes (M3 screws through flanges)
        for(i = [0:1]) {
            translate([width*0.1 + i*width*0.8, height*0.5, depth*0.5])
                rotate([0, 90, 0])
                    cylinder(h=12, d=3.2, center=true);
        }
        
        // Wire management slot
        translate([width*0.5, 0, depth*0.4])
            cube([6, height*0.3, depth*0.6], center=true);
    }
}

// Generate the mount
sensor_basic_mount();
