// Parametric Skull Generator System
// Based on anatomical references and 3D scanning data

// =============================================================================
// HUMAN-BASED SKULL FAMILY (Goblins, Elves, Androids, Zombies)
// Reference: Stanford 3D Scanning Repository, medical anatomy atlases
// =============================================================================

// Base human skull proportions (adult male average)
human_skull_length = 185; // mm anterior-posterior
human_skull_width = 145;  // mm medial-lateral  
human_skull_height = 135; // mm superior-inferior
human_eye_spacing = 65;   // mm inter-orbital distance

module human_skull_base(scale_factor = 1.0) {
    scale([scale_factor, scale_factor, scale_factor]) {
        union() {
            // Cranium (main brain case)
            translate([0, 0, human_skull_height/3])
                ellipsoid([human_skull_width/2, human_skull_length/2, human_skull_height/2]);
            
            // Facial structure
            translate([0, -human_skull_length/4, 0])
                ellipsoid([human_skull_width/3, human_skull_length/3, human_skull_height/2.5]);
            
            // Jaw structure
            translate([0, -human_skull_length/3, -human_skull_height/4])
                ellipsoid([human_skull_width/2.5, human_skull_length/4, human_skull_height/4]);
        }
    }
}

// Goblin skull modifications
module goblin_skull(eye_spacing = 76, scale_factor = 1.0) {
    color([0.7, 0.6, 0.4]) { // Earthy brown-green tone
        difference() {
            // Base human skull with goblin proportions
            scale([0.9, 1.15, 0.85]) // Narrower, longer, shorter
                human_skull_base(scale_factor);
            
            // Enlarged eye sockets
            translate([-eye_spacing/2, -human_skull_length/6, human_skull_height/6])
                sphere(r = 18);
            translate([eye_spacing/2, -human_skull_length/6, human_skull_height/6])
                sphere(r = 18);
            
            // Nasal cavity
            translate([0, -human_skull_length/4, human_skull_height/8])
                ellipsoid([8, 15, 20]);
        }
        
        // Prominent brow ridge
        translate([0, -human_skull_length/5, human_skull_height/4])
            ellipsoid([eye_spacing + 20, 15, 8]);
        
        // Pointed ears attachment points
        translate([-human_skull_width/2.2, 0, human_skull_height/6])
            ellipsoid([8, 12, 15]);
        translate([human_skull_width/2.2, 0, human_skull_height/6])
            ellipsoid([8, 12, 15]);
    }
}

// Elf skull modifications  
module elf_skull(eye_spacing = 68, scale_factor = 1.0) {
    color([0.9, 0.85, 0.8]) { // Pale, refined tone
        difference() {
            // Base human skull with elf proportions
            scale([0.95, 1.05, 1.1]) // Slightly narrower, longer, taller
                human_skull_base(scale_factor);
            
            // Almond-shaped eye sockets
            translate([-eye_spacing/2, -human_skull_length/6, human_skull_height/5])
                ellipsoid([12, 20, 15]);
            translate([eye_spacing/2, -human_skull_length/6, human_skull_height/5])
                ellipsoid([12, 20, 15]);
            
            // Refined nasal cavity
            translate([0, -human_skull_length/4, human_skull_height/8])
                ellipsoid([6, 12, 18]);
        }
        
        // High cheekbones
        translate([-eye_spacing/2.5, -human_skull_length/8, human_skull_height/8])
            ellipsoid([8, 10, 6]);
        translate([eye_spacing/2.5, -human_skull_length/8, human_skull_height/8])
            ellipsoid([8, 10, 6]);
        
        // Pointed ear attachment points (elongated)
        translate([-human_skull_width/2.1, 5, human_skull_height/4])
            ellipsoid([6, 8, 25]);
        translate([human_skull_width/2.1, 5, human_skull_height/4])
            ellipsoid([6, 8, 25]);
    }
}

// =============================================================================
// FELINE SKULL FAMILY (All cat types)
// Reference: Comparative anatomy studies, big cat research databases
// =============================================================================

// Base feline skull proportions
feline_skull_length = 160; // mm (average domestic cat)
feline_skull_width = 110;  
feline_skull_height = 85;
feline_eye_spacing = 45;

module feline_skull_base(scale_factor = 1.0) {
    scale([scale_factor, scale_factor, scale_factor]) {
        union() {
            // Rounded cranium
            translate([0, 0, feline_skull_height/2])
                ellipsoid([feline_skull_width/2, feline_skull_length/2.5, feline_skull_height/2]);
            
            // Elongated muzzle
            translate([0, -feline_skull_length/2.8, 0])
                ellipsoid([feline_skull_width/4, feline_skull_length/3, feline_skull_height/3]);
            
            // Jaw structure  
            translate([0, -feline_skull_length/2.5, -feline_skull_height/5])
                ellipsoid([feline_skull_width/3, feline_skull_length/4, feline_skull_height/4]);
        }
    }
}

// House cat skull
module cat_domestic_skull(eye_spacing = 45, scale_factor = 1.0) {
    color([0.9, 0.88, 0.85]) { // Light bone color
        difference() {
            feline_skull_base(scale_factor);
            
            // Round eye sockets
            translate([-eye_spacing/2, -feline_skull_length/8, feline_skull_height/3])
                sphere(r = 12);
            translate([eye_spacing/2, -feline_skull_length/8, feline_skull_height/3])
                sphere(r = 12);
            
            // Nasal opening
            translate([0, -feline_skull_length/2.2, feline_skull_height/6])
                ellipsoid([4, 8, 6]);
        }
    }
}

// Lion skull (larger, more robust)
module lion_skull(eye_spacing = 85, scale_factor = 1.6) {
    color([0.85, 0.8, 0.7]) { // Weathered bone color
        difference() {
            scale([1.2, 1.1, 1.0]) // Broader, more robust
                feline_skull_base(scale_factor);
            
            // Larger eye sockets
            translate([-eye_spacing/2, -feline_skull_length/7, feline_skull_height/2.5])
                sphere(r = 18);
            translate([eye_spacing/2, -feline_skull_length/7, feline_skull_height/2.5])
                sphere(r = 18);
        }
        
        // Prominent sagittal crest (muscle attachment ridge)
        translate([0, -10, feline_skull_height * scale_factor - 5])
            ellipsoid([5, feline_skull_length/2, 8]);
        
        // Mane attachment ridges
        translate([0, 10, feline_skull_height/2])
            ellipsoid([feline_skull_width * scale_factor/3, 15, 10]);
    }
}

// Tiger skull (similar to lion but more streamlined)
module tiger_skull(eye_spacing = 90, scale_factor = 1.8) {
    color([0.88, 0.85, 0.8]) { // Slight ivory tint
        difference() {
            scale([1.15, 1.2, 1.05]) // Long and powerful
                feline_skull_base(scale_factor);
            
            // Tiger eye sockets (slightly forward-facing)
            translate([-eye_spacing/2, -feline_skull_length/6.5, feline_skull_height/2.3])
                ellipsoid([16, 18, 14]);
            translate([eye_spacing/2, -feline_skull_length/6.5, feline_skull_height/2.3])
                ellipsoid([16, 18, 14]);
        }
        
        // Sagittal crest (more pronounced than lion)
        translate([0, -5, feline_skull_height * scale_factor - 3])
            ellipsoid([4, feline_skull_length/1.8, 12]);
    }
}

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

module ellipsoid(radii) {
    scale(radii) sphere(r=1);
}

// Color reference system for different creature families
function get_skull_color(creature_family) =
    creature_family == "goblin" ? [0.7, 0.6, 0.4] :     // Earthy brown-green
    creature_family == "elf" ? [0.9, 0.85, 0.8] :       // Pale refined
    creature_family == "android" ? [0.8, 0.8, 0.85] :   // Metallic bone
    creature_family == "zombie" ? [0.6, 0.65, 0.5] :    // Decayed green-gray
    creature_family == "skeleton" ? [0.95, 0.92, 0.88] : // Pure bleached bone
    creature_family == "cat_domestic" ? [0.9, 0.88, 0.85] : // Light bone
    creature_family == "cat_big" ? [0.85, 0.8, 0.7] :   // Weathered bone
    creature_family == "dragon" ? [0.7, 0.75, 0.6] :    // Ancient bone-green
    [0.9, 0.9, 0.9]; // Default light gray

// =============================================================================
// EXAMPLE USAGE
// =============================================================================

// Generate goblin skull with specific parameters
// goblin_skull(eye_spacing = 76, scale_factor = 1.0);

// Generate lion skull  
// lion_skull(eye_spacing = 85, scale_factor = 1.6);

// Generate tiger skull
// tiger_skull(eye_spacing = 90, scale_factor = 1.8);