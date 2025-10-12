// Integrated Skull Assembly System
// Combines anatomical skull shapes with actual component mounting requirements
// Takes into account display, sensor, and speaker placements

// Import the basic mounting systems
use <basic_mounts/display_basic_mount.scad>
use <basic_mounts/sensor_basic_mount.scad>
use <basic_mounts/speaker_basic_mount.scad>

// Import skull generation libraries
use <anatomical_skull_library.scad>
use <fantasy_skull_library.scad>

// Component mounting specifications from JSON positioned components
module component_positions() {
    // Left Eye Display: x=-1.05", y=+0.7", z=-0.35"
    translate([-26.67, 17.78, -8.89]) {
        rotate([0, 0, 0]) {
            color([0.8, 0.2, 0.2, 0.7]) // Red for left eye
                display_basic_mount();
        }
    }
    
    // Right Eye Display: x=+1.05", y=+0.7", z=-0.35"  
    translate([26.67, 17.78, -8.89]) {
        rotate([0, 0, 0]) {
            color([0.2, 0.8, 0.2, 0.7]) // Green for right eye
                display_basic_mount();
        }
    }
    
    // Mouth Display: x=0", y=-1.05", z=0"
    translate([0, -26.67, 0]) {
        rotate([0, 0, 0]) {
            color([0.2, 0.2, 0.8, 0.7]) // Blue for mouth
                display_basic_mount();
        }
    }
    
    // Nose Sensor: x=0", y=0", z=+0.25"
    translate([0, 0, 6.35]) {
        rotate([0, 0, 0]) {
            color([0.8, 0.8, 0.2, 0.7]) // Yellow for sensor
                sensor_basic_mount();
        }
    }
    
    // Left Ear Speaker: x=-1.5", y=+0.75", z=+0.5"
    translate([-38.1, 19.05, 12.7]) {
        rotate([0, 90, 0]) {
            color([0.8, 0.2, 0.8, 0.7]) // Magenta for left speaker
                speaker_basic_mount();
        }
    }
    
    // Right Ear Speaker: x=+1.5", y=+0.75", z=+0.5"
    translate([38.1, 19.05, 12.7]) {
        rotate([0, -90, 0]) {
            color([0.2, 0.8, 0.8, 0.7]) // Cyan for right speaker
                speaker_basic_mount();
        }
    }
}

// Complete skull assembly with integrated component mounting
module integrated_skull_assembly(creature_type = "goblin", show_components = true, show_skull = true) {
    
    if (show_skull) {
        // Generate the anatomical skull based on creature type
        if (creature_type == "goblin") {
            color([0.9, 0.85, 0.75, 0.8]) // Bone color with transparency
                goblin_skull_anatomical();
        }
        else if (creature_type == "zombie") {
            color([0.7, 0.75, 0.6, 0.8]) // Decayed bone color
                zombie_skull_anatomical();
        }
        else if (creature_type == "android") {
            color([0.8, 0.8, 0.85, 0.8]) // Metallic bone color
                android_skull_anatomical();
        }
        else if (creature_type == "cat") {
            color([0.9, 0.88, 0.86, 0.8]) // Cat bone color
                cat_skull_anatomical();
        }
        else if (creature_type == "lion") {
            color([0.85, 0.8, 0.7, 0.8]) // Weathered big cat bone
                lion_skull_anatomical();
        }
        else if (creature_type == "dragon_fire") {
            color([0.4, 0.3, 0.2, 0.8]) // Charred dragon bone
                fire_dragon_skull();
        }
        else if (creature_type == "dragon_ice") {
            color([0.8, 0.85, 0.9, 0.8]) // Pale ice dragon bone
                ice_dragon_skull();
        }
        else if (creature_type == "bear") {
            color([0.8, 0.75, 0.65, 0.8]) // Bear bone color
                bear_skull_anatomical();
        }
        else {
            // Default to human-based skull
            color([0.94, 0.92, 0.88, 0.8]) // Standard bone color
                human_skull_base();
        }
    }
    
    if (show_components) {
        component_positions();
    }
}

// Skull cavity generation for component clearance
module skull_component_cavities(creature_type = "goblin") {
    
    // Eye socket cavities (extended for display mounting)
    translate([-26.67, 17.78, -8.89]) {
        cylinder(h=15, d=45, center=true); // Left eye cavity
    }
    translate([26.67, 17.78, -8.89]) {
        cylinder(h=15, d=45, center=true); // Right eye cavity
    }
    
    // Mouth cavity (extended for display)
    translate([0, -26.67, 0]) {
        cylinder(h=12, d=45, center=true); // Mouth cavity
    }
    
    // Nasal cavity (for sensor)
    translate([0, 0, 6.35]) {
        hull() {
            sphere(d=15);
            translate([0, 10, 0]) sphere(d=12);
        }
    }
    
    // Ear canal cavities (for speakers)
    translate([-38.1, 19.05, 12.7]) {
        rotate([0, 90, 0]) {
            cylinder(h=25, d=20, center=true);
        }
    }
    translate([38.1, 19.05, 12.7]) {
        rotate([0, -90, 0]) {
            cylinder(h=25, d=20, center=true);
        }
    }
    
    // Cable routing channels
    // Main harness channel (from nose to back of skull)
    translate([0, 0, 0]) {
        hull() {
            translate([0, 0, 6.35]) sphere(d=8);  // From nose sensor
            translate([0, 15, -10]) sphere(d=8);   // To back cavity
        }
    }
    
    // Eye cable channels
    for (x_pos = [-26.67, 26.67]) {
        translate([x_pos, 17.78, -8.89]) {
            hull() {
                sphere(d=6); // At eye position
                translate([0, 15, -5]) sphere(d=6); // To main channel
            }
        }
    }
    
    // Mouth cable channel
    translate([0, -26.67, 0]) {
        hull() {
            sphere(d=6); // At mouth position
            translate([0, 15, -8]) sphere(d=6); // To main channel
        }
    }
}

// Printable skull assembly (hollow with mounting integration)
module printable_skull_assembly(creature_type = "goblin", wall_thickness = 2.5) {
    
    difference() {
        // Main skull shape
        integrated_skull_assembly(creature_type, show_components=false, show_skull=true);
        
        // Remove internal volume (make hollow)
        offset_skull_interior(creature_type, wall_thickness);
        
        // Component mounting cavities
        skull_component_cavities(creature_type);
    }
    
    // Add mounting reinforcement structures
    skull_mounting_reinforcements(creature_type);
}

// Internal skull volume for hollowing
module offset_skull_interior(creature_type, wall_thickness) {
    // Create an offset interior surface
    minkowski() {
        difference() {
            if (creature_type == "goblin") {
                goblin_skull_anatomical();
            }
            else if (creature_type == "zombie") {
                zombie_skull_anatomical();
            }
            else if (creature_type == "android") {
                android_skull_anatomical();
            }
            else if (creature_type == "cat") {
                cat_skull_anatomical();
            }
            else if (creature_type == "lion") {
                lion_skull_anatomical();
            }
            else if (creature_type == "dragon_fire") {
                fire_dragon_skull();
            }
            else if (creature_type == "bear") {
                bear_skull_anatomical();
            }
            else {
                human_skull_base();
            }
            
            // Remove areas that should remain solid for mounting
            skull_solid_zones();
        }
        sphere(r=wall_thickness);
    }
}

// Areas that must remain solid for mounting integrity
module skull_solid_zones() {
    // Eye socket rims (for display mounting)
    for (x_pos = [-26.67, 26.67]) {
        translate([x_pos, 17.78, -8.89]) {
            cylinder(h=20, d=50, center=true);
        }
    }
    
    // Mouth area rim
    translate([0, -26.67, 0]) {
        cylinder(h=15, d=50, center=true);
    }
    
    // Nasal bridge (for sensor mounting)
    translate([0, 0, 6.35]) {
        sphere(d=25);
    }
}

// Mounting reinforcement structures
module skull_mounting_reinforcements(creature_type) {
    
    // Eye socket reinforcement rings
    for (x_pos = [-26.67, 26.67]) {
        translate([x_pos, 17.78, -8.89]) {
            difference() {
                cylinder(h=4, d=42);
                cylinder(h=6, d=35, center=true);
            }
        }
    }
    
    // Mouth reinforcement ring
    translate([0, -26.67, 0]) {
        difference() {
            cylinder(h=4, d=42);
            cylinder(h=6, d=35, center=true);
        }
    }
    
    // Nose sensor bracket reinforcement
    translate([0, 0, 6.35]) {
        difference() {
            hull() {
                sphere(d=22);
                translate([0, 8, 0]) sphere(d=18);
            }
            hull() {
                sphere(d=16);
                translate([0, 8, 0]) sphere(d=12);
            }
        }
    }
    
    // Ear mounting reinforcement
    for (x_pos = [-38.1, 38.1]) {
        translate([x_pos, 19.05, 12.7]) {
            rotate([0, x_pos < 0 ? 90 : -90, 0]) {
                difference() {
                    cylinder(h=6, d=25);
                    cylinder(h=8, d=18, center=true);
                }
            }
        }
    }
}

// Test assemblies for different creature types
module test_all_assemblies() {
    
    spacing = 120;
    creatures = ["goblin", "zombie", "android", "cat", "dragon_fire"];
    
    for (i = [0:len(creatures)-1]) {
        translate([i * spacing, 0, 0]) {
            integrated_skull_assembly(creatures[i], show_components=true, show_skull=true);
        }
    }
}

// Usage examples:
// integrated_skull_assembly("goblin", show_components=true, show_skull=true);
// printable_skull_assembly("goblin", wall_thickness=2.5);
// test_all_assemblies();