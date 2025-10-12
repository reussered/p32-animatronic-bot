// Fantasy Creature Skull Generators
// Based on reptilian, avian, and extinct creature anatomical references

// =============================================================================
// DRAGON SKULL FAMILY (Fire, Ice, Ancient types)
// Reference: Crocodilian skulls, bird skull features, dinosaur fossils
// =============================================================================

// Base dragon skull proportions (combination of reptilian + avian features)
dragon_skull_length = 220; // mm (larger than human for intimidation)
dragon_skull_width = 140;  
dragon_skull_height = 110;
dragon_eye_spacing = 80;

module dragon_skull_base(scale_factor = 1.0) {
    scale([scale_factor, scale_factor, scale_factor]) {
        union() {
            // Elongated cranium (reptilian)
            translate([0, 15, dragon_skull_height/2])
                ellipsoid([dragon_skull_width/2, dragon_skull_length/2.2, dragon_skull_height/2]);
            
            // Extended snout/muzzle
            translate([0, -dragon_skull_length/2.2, dragon_skull_height/6])
                ellipsoid([dragon_skull_width/3.5, dragon_skull_length/2.5, dragon_skull_height/2.8]);
            
            // Lower jaw (powerful)
            translate([0, -dragon_skull_length/2.8, -dragon_skull_height/6])
                ellipsoid([dragon_skull_width/3, dragon_skull_length/3, dragon_skull_height/3]);
        }
    }
}

// Fire dragon skull (aggressive, angular features)
module dragon_fire_skull(eye_spacing = 80, scale_factor = 1.5) {
    color([0.4, 0.3, 0.2]) { // Dark charred bone
        difference() {
            dragon_skull_base(scale_factor);
            
            // Forward-facing predator eye sockets
            translate([-eye_spacing/2, -dragon_skull_length/8, dragon_skull_height/2.5])
                ellipsoid([15, 20, 18]);
            translate([eye_spacing/2, -dragon_skull_length/8, dragon_skull_height/2.5])
                ellipsoid([15, 20, 18]);
            
            // Nostril openings (large for fire breathing)
            translate([-8, -dragon_skull_length/1.8, dragon_skull_height/4])
                ellipsoid([6, 12, 8]);
            translate([8, -dragon_skull_length/1.8, dragon_skull_height/4])
                ellipsoid([6, 12, 8]);
        }
        
        // Prominent brow ridges (angular/aggressive)
        translate([0, -dragon_skull_length/12, dragon_skull_height/1.8])
            ellipsoid([eye_spacing + 30, 12, 8]);
        
        // Horn attachment points
        translate([-dragon_skull_width/3, 20, dragon_skull_height * scale_factor - 15])
            ellipsoid([8, 8, 15]);
        translate([dragon_skull_width/3, 20, dragon_skull_height * scale_factor - 15])
            ellipsoid([8, 8, 15]);
        
        // Sagittal crest (muscle attachment for powerful neck)
        translate([0, 0, dragon_skull_height * scale_factor - 8])
            ellipsoid([6, dragon_skull_length/2.5, 12]);
    }
}

// Ice dragon skull (smoother, crystalline features)
module dragon_ice_skull(eye_spacing = 75, scale_factor = 1.2) {
    color([0.8, 0.85, 0.9]) { // Pale blue-white bone
        difference() {
            scale([1.0, 1.1, 0.95]) // Slightly more elongated, less tall
                dragon_skull_base(scale_factor);
            
            // Almond-shaped eye sockets
            translate([-eye_spacing/2, -dragon_skull_length/8, dragon_skull_height/2.3])
                ellipsoid([14, 22, 16]);
            translate([eye_spacing/2, -dragon_skull_length/8, dragon_skull_height/2.3])
                ellipsoid([14, 22, 16]);
            
            // Smaller nostril openings
            translate([-6, -dragon_skull_length/1.9, dragon_skull_height/4])
                ellipsoid([4, 8, 6]);
            translate([6, -dragon_skull_length/1.9, dragon_skull_height/4])
                ellipsoid([4, 8, 6]);
        }
        
        // Smoother brow ridge
        translate([0, -dragon_skull_length/15, dragon_skull_height/1.9])
            ellipsoid([eye_spacing + 20, 8, 6]);
        
        // Crystalline horn formations
        translate([0, 25, dragon_skull_height * scale_factor - 10])
            ellipsoid([6, 6, 20]); // Central horn
        translate([-dragon_skull_width/4, 15, dragon_skull_height * scale_factor - 12])
            ellipsoid([5, 5, 12]);
        translate([dragon_skull_width/4, 15, dragon_skull_height * scale_factor - 12])
            ellipsoid([5, 5, 12]);
    }
}

// =============================================================================
// UNDEAD SKULL VARIATIONS (Skeleton, Zombie)
// Reference: Human skull with decay/damage patterns
// =============================================================================

// Skeleton skull (clean, bleached bone)
module skeleton_skull(eye_spacing = 65, scale_factor = 1.0) {
    color([0.95, 0.92, 0.88]) { // Pure bleached bone white
        difference() {
            human_skull_base(scale_factor);
            
            // Standard human eye sockets
            translate([-eye_spacing/2, -human_skull_length/6, human_skull_height/6])
                sphere(r = 16);
            translate([eye_spacing/2, -human_skull_length/6, human_skull_height/6])
                sphere(r = 16);
            
            // Nasal cavity
            translate([0, -human_skull_length/4, human_skull_height/8])
                ellipsoid([7, 12, 18]);
            
            // Temporal openings (more pronounced in undead)
            translate([-human_skull_width/2.5, -10, human_skull_height/4])
                ellipsoid([12, 15, 10]);
            translate([human_skull_width/2.5, -10, human_skull_height/4])
                ellipsoid([12, 15, 10]);
        }
        
        // Suture lines (bone plate connections)
        translate([0, 0, human_skull_height/1.5])
            ellipsoid([human_skull_width * scale_factor, 2, 2]);
        translate([0, -20, human_skull_height/1.2])
            ellipsoid([human_skull_width * scale_factor * 0.8, 2, 2]);
    }
}

// Zombie skull (decayed, damaged bone)
module zombie_skull(eye_spacing = 68, scale_factor = 1.0, decay_level = 0.3) {
    color([0.6, 0.65, 0.5]) { // Decayed green-gray bone
        difference() {
            // Base human skull with slight deformation
            scale([1.02, 0.98, 1.01]) // Slightly asymmetric from decay
                human_skull_base(scale_factor);
            
            // Larger, more ragged eye sockets
            translate([-eye_spacing/2, -human_skull_length/6, human_skull_height/6])
                ellipsoid([18, 20, 18]); // Larger than normal
            translate([eye_spacing/2, -human_skull_length/6, human_skull_height/6])
                ellipsoid([17, 19, 17]); // Slightly asymmetric
            
            // Enlarged nasal cavity (decay damage)
            translate([0, -human_skull_length/4, human_skull_height/8])
                ellipsoid([10, 15, 22]);
            
            // Decay holes and damage
            if (decay_level > 0.2) {
                translate([human_skull_width/4, 10, human_skull_height/2])
                    sphere(r = 8); // Decay hole
            }
            if (decay_level > 0.4) {
                translate([-human_skull_width/3, -5, human_skull_height/1.8])
                    ellipsoid([6, 8, 10]); // Additional damage
            }
        }
        
        // Protruding bone fragments (damage)
        translate([human_skull_width/3.5, -human_skull_length/8, human_skull_height/3])
            ellipsoid([3, 8, 4]);
    }
}

// =============================================================================
// ANDROID/ROBOT SKULL VARIATIONS
// Reference: Human skull with mechanical modifications
// =============================================================================

// Android skull (human-like but with subtle mechanical elements)
module android_skull(eye_spacing = 62, scale_factor = 1.0) {
    color([0.8, 0.8, 0.85]) { // Metallic bone color
        difference() {
            // Slightly more geometric human skull
            scale([0.98, 1.0, 1.02]) // Subtle geometric proportions
                human_skull_base(scale_factor);
            
            // Precise circular eye sockets
            translate([-eye_spacing/2, -human_skull_length/6, human_skull_height/6])
                cylinder(h=20, r=15, center=true);
            translate([eye_spacing/2, -human_skull_length/6, human_skull_height/6])
                cylinder(h=20, r=15, center=true);
            
            // Geometric nasal opening
            translate([0, -human_skull_length/4, human_skull_height/8])
                cube([12, 8, 16], center=true);
        }
        
        // Subtle panel lines
        translate([0, -5, human_skull_height/1.3])
            cube([human_skull_width * scale_factor * 0.9, 1, 1], center=true);
        translate([0, 15, human_skull_height/1.5])
            cube([human_skull_width * scale_factor * 0.7, 1, 1], center=true);
        
        // Port/connector points
        translate([human_skull_width/2.8, 5, human_skull_height/3])
            cylinder(h=4, r=3, center=true);
        translate([-human_skull_width/2.8, 5, human_skull_height/3])
            cylinder(h=4, r=3, center=true);
    }
}

// =============================================================================
// COLOR AND TEXTURE REFERENCE SYSTEM
// =============================================================================

function get_creature_skull_color(creature_type) =
    creature_type == "dragon_fire" ? [0.4, 0.3, 0.2] :      // Charred black-brown
    creature_type == "dragon_ice" ? [0.8, 0.85, 0.9] :      // Pale blue-white
    creature_type == "dragon_ancient" ? [0.5, 0.45, 0.3] :  // Ancient weathered
    creature_type == "skeleton" ? [0.95, 0.92, 0.88] :      // Bleached bone white
    creature_type == "zombie" ? [0.6, 0.65, 0.5] :          // Decayed green-gray
    creature_type == "android" ? [0.8, 0.8, 0.85] :         // Metallic bone
    creature_type == "goblin" ? [0.7, 0.6, 0.4] :           // Earthy brown-green
    creature_type == "elf" ? [0.9, 0.85, 0.8] :             // Pale refined
    [0.85, 0.82, 0.78]; // Default aged bone

// Texture mapping for different age/condition states
function get_bone_texture_roughness(age_state) =
    age_state == "fresh" ? 0.1 :      // Smooth, recently cleaned
    age_state == "weathered" ? 0.4 :  // Natural aging
    age_state == "ancient" ? 0.7 :    // Very old, pitted
    age_state == "decayed" ? 0.8 :    // Zombie-like decay
    0.3; // Default moderate aging

// =============================================================================
// EXAMPLE USAGE AND TESTING
// =============================================================================

// Fire dragon skull for large bot
// dragon_fire_skull(eye_spacing = 95, scale_factor = 1.8);

// Skeleton warrior skull
// skeleton_skull(eye_spacing = 68, scale_factor = 1.1);

// Zombie with heavy decay
// zombie_skull(eye_spacing = 70, scale_factor = 1.0, decay_level = 0.6);