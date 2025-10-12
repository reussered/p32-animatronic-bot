// Goblin Nose Sensor Bracket
// Self-contained HC-SR04 mounting bracket

// HC-SR04 sensor dimensions in mm
width = 45.72;   // 1.8 inches
height = 17.78;  // 0.7 inches  
depth = 15.24;   // 0.6 inches

// Generate the sensor bracket
difference() {
    // Main bracket body with rounded edges
    hull() {
        for(x = [2, width-2]) {
            for(y = [2, height-2]) {
                translate([x, y, 0])
                    cylinder(h=depth, d=4);
            }
        }
    }
    
    // Sensor mounting holes (HC-SR04 transducers)
    translate([width*0.25, height*0.5, -1])
        cylinder(h=depth+2, d=16); // Left transducer
    translate([width*0.75, height*0.5, -1])  
        cylinder(h=depth+2, d=16); // Right transducer
        
    // Sensor PCB mounting holes
    for(i = [0:1]) {
        translate([width*0.15 + i*width*0.7, height*0.15, -1])
            cylinder(h=depth+2, d=2.5);
        translate([width*0.15 + i*width*0.7, height*0.85, -1])
            cylinder(h=depth+2, d=2.5);
    }
    
    // Case mounting holes (M3 screws)  
    for(i = [0:1]) {
        translate([width*0.1 + i*width*0.8, height*0.5, depth*0.5])
            rotate([0, 90, 0])
                cylinder(h=width*0.15, d=3.2);
    }
    
    // Wire management slot
    translate([width*0.5, 0, depth*0.3])
        cube([4, height*0.2, depth*0.4], center=true);
}

// Reinforcement ribs
for(i = [0:2]) {
    translate([width*0.2 + i*width*0.3, height*0.5, 0])
        cube([1, height*0.6, depth*0.3], center=true);
}