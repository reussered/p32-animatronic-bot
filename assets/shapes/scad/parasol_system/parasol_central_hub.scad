/*
 * Solar Parasol Central Hub Assembly
 * P32 Animatronic Bot - OpenSCAD Shape Generation
 * 
 * Central hub that contains MPPT controller, servo drivers,
 * and provides mounting interface for telescoping ribs
 */

// Import configuration
include <../config/parasol_dimensions.scad>
include <../basic_mounts/servo_mount.scad>
include <../basic_mounts/electronics_enclosure.scad>

// Hub specifications
hub_diameter = 120;  // mm
hub_height = 80;     // mm
wall_thickness = 3;  // mm
rib_count = 8;       // telescoping ribs
servo_mount_count = 2; // tracking servos

module parasol_central_hub() {
    difference() {
        // Main hub body
        cylinder(d=hub_diameter, h=hub_height, center=true);
        
        // Back mount interface (50mm tube socket)
        translate([0, 0, -hub_height/2 - 1])
            cylinder(d=52, h=15);
        
        // Internal cavity for electronics
        translate([0, 0, 2])
            cylinder(d=hub_diameter-2*wall_thickness, h=hub_height-2, center=true);
        
        // Rib mounting sockets (8 radial positions)
        for (i = [0:rib_count-1]) {
            rotate([0, 0, i * 360/rib_count]) {
                translate([hub_diameter/2 - 10, 0, 0]) {
                    // Rib socket
                    cylinder(d=12, h=hub_height+2, center=true);
                    
                    // Servo wire channels
                    translate([0, 0, -hub_height/2])
                        cylinder(d=4, h=15);
                }
            }
        }
        
        // Cable management channels
        for (i = [0:3]) {
            rotate([0, 0, i * 90 + 45]) {
                translate([25, 0, 0])
                    cylinder(d=8, h=hub_height+2, center=true);
            }
        }
        
        // Top servo mount (tilt axis)
        translate([0, 0, hub_height/2 - 5])
            cube([40, 20, 12], center=true);
            
        // Side servo mount (rotation axis) 
        translate([hub_diameter/2 - 15, 0, 0])
            rotate([0, 90, 0])
                cylinder(d=20, h=25, center=true);
    }
    
    // Internal mounting posts for electronics
    for (i = [0:3]) {
        rotate([0, 0, i * 90]) {
            translate([25, 0, -hub_height/2 + 5])
                cylinder(d=6, h=10);
        }
    }
    
    // Rib socket reinforcements
    for (i = [0:rib_count-1]) {
        rotate([0, 0, i * 360/rib_count]) {
            translate([hub_diameter/2 - 15, 0, 0]) {
                difference() {
                    cylinder(d=20, h=hub_height-10, center=true);
                    cylinder(d=12, h=hub_height, center=true);
                }
            }
        }
    }
}

module parasol_hub_complete() {
    parasol_central_hub();
    
    // Add cable strain reliefs
    for (i = [0:3]) {
        rotate([0, 0, i * 90 + 45]) {
            translate([35, 0, -hub_height/2])
                cable_strain_relief();
        }
    }
}

module cable_strain_relief() {
    difference() {
        cylinder(d=15, h=8);
        translate([0, 0, -1])
            cylinder(d=8, h=10);
    }
}

// Generate the hub
parasol_hub_complete();