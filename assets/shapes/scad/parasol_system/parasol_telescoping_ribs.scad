/*
 * Solar Parasol Telescoping Rib System
 * P32 Animatronic Bot - OpenSCAD Shape Generation
 * 
 * Three-section telescoping ribs that deploy from central hub
 * Each rib extends from 200mm to 1000mm (5:1 ratio)
 */

include <../config/parasol_dimensions.scad>

// Rib specifications  
rib_sections = 3;           // telescoping sections
inner_diameter = 8;         // mm, innermost tube
wall_thickness = 1.5;       // mm
clearance = 0.3;            // mm between sections
extension_lengths = [200, 350, 450]; // mm per section
servo_mount_length = 40;    // mm

module telescoping_rib_section(section_num, length) {
    outer_d = inner_diameter + (section_num * 2 * (wall_thickness + clearance));
    inner_d = outer_d - 2 * wall_thickness;
    
    difference() {
        // Main tube
        cylinder(d=outer_d, h=length);
        
        // Inner hollow
        if (section_num > 0) {
            translate([0, 0, -1])
                cylinder(d=inner_d, h=length+2);
        }
        
        // Extension guide slots
        if (section_num < rib_sections-1) {
            for (i = [0:1]) {
                rotate([0, 0, i * 180]) {
                    translate([inner_d/2 - 0.5, 0, length - 30])
                        cube([1, outer_d + 2, 25], center=true);
                }
            }
        }
    }
    
    // Internal extension mechanism
    if (section_num < rib_sections-1) {
        translate([0, 0, length - 35]) {
            // Extension stopper
            difference() {
                cylinder(d=inner_d - 0.5, h=5);
                cylinder(d=2, h=6, center=true); // servo wire channel
            }
            
            // Thread attachment point
            translate([0, 0, -10])
                cylinder(d=4, h=15);
        }
    }
}

module telescoping_rib_complete() {
    // Section 0 (innermost, attaches to hub)
    color("blue") {
        telescoping_rib_section(0, extension_lengths[0]);
        
        // Hub attachment collar
        translate([0, 0, -20]) {
            difference() {
                cylinder(d=inner_diameter + 6, h=25);
                translate([0, 0, -1])
                    cylinder(d=inner_diameter, h=27);
                    
                // Attachment screws
                for (i = [0:2]) {
                    rotate([0, 0, i * 120])
                        translate([inner_diameter/2 + 2, 0, 12])
                            rotate([0, 90, 0])
                                cylinder(d=3.5, h=8);
                }
            }
        }
    }
    
    // Section 1 (middle)
    color("green") {
        translate([0, 0, extension_lengths[0] - 50])
            telescoping_rib_section(1, extension_lengths[1]);
    }
    
    // Section 2 (outermost, solar panel mount)
    color("red") {
        translate([0, 0, extension_lengths[0] + extension_lengths[1] - 100])
            telescoping_rib_section(2, extension_lengths[2]);
            
        // Solar panel mounting plate
        translate([0, 0, sum(extension_lengths) - 120]) {
            solar_panel_mount();
        }
    }
}

module solar_panel_mount() {
    mount_width = 60;
    mount_length = 80; 
    
    difference() {
        // Main mounting plate
        translate([0, 0, 5])
            cube([mount_width, mount_length, 10], center=true);
            
        // Solar panel attachment points (4 corners)
        for (x = [-1, 1]) {
            for (y = [-1, 1]) {
                translate([x * (mount_width/2 - 8), y * (mount_length/2 - 8), 1])
                    cylinder(d=3.5, h=12);
            }
        }
        
        // Rib attachment
        translate([0, 0, 0])
            cylinder(d=inner_diameter + 2 * (rib_sections-1) * (wall_thickness + clearance), h=12, center=true);
    }
    
    // Panel support ribs
    for (y = [-1, 1]) {
        translate([0, y * mount_length/3, 8])
            cube([mount_width - 10, 3, 6], center=true);
    }
}

module extension_servo_mount() {
    // Servo mount for rib extension
    difference() {
        cube([servo_mount_length, 25, 20], center=true);
        
        // Servo mounting holes
        for (x = [-1, 1]) {
            translate([x * 12, 0, 0])
                cylinder(d=3.5, h=25, center=true);
        }
        
        // Servo body clearance
        translate([0, 0, 5])
            cube([24, 15, 12], center=true);
    }
}

// Generate complete rib assembly
telescoping_rib_complete();

// Generate servo mount separately
translate([0, 80, 0])
    extension_servo_mount();