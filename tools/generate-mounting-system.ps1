# Generate Two-Tier Mounting System for P32 Animatronic Bot
# Tier 1: Basic hardware mounting brackets (generic, reusable)
# Tier 2: Bot-specific decorative shells (character-themed, incorporates brackets)

param(
    [string]$BotType = "goblin",
    [switch]$GenerateBasicMounts = $true,
    [switch]$GenerateBotShells = $true,
    [switch]$OpenSCADPreview = $false
)

Write-Host "=== P32 Mounting System Generator ===" -ForegroundColor Cyan
Write-Host "Bot Type: $BotType" -ForegroundColor Yellow
Write-Host "Basic Mounts: $GenerateBasicMounts | Bot Shells: $GenerateBotShells" -ForegroundColor Yellow

# Define hardware mounting specifications
$MountingSpecs = @{
    "gc9a01_display" = @{
        "diameter" = 32.512  # 1.28 INCH in mm
        "thickness" = 2.032  # 0.08 INCH in mm
        "mount_holes" = 4
        "hole_diameter" = 2.54  # 0.1 INCH in mm
        "hole_spacing" = 27.94  # 1.1 INCH in mm
        "bracket_name" = "display_basic_mount"
    }
    "hc_sr04_sensor" = @{
        "width" = 45.0
        "height" = 20.0
        "depth" = 15.0
        "transducer_diameter" = 16.0
        "transducer_spacing" = 24.0
        "bracket_name" = "sensor_basic_mount"
    }
    "speaker" = @{
        "diameter" = 40.0  # 40mm speaker
        "depth" = 20.0
        "mount_holes" = 4
        "hole_diameter" = 3.0
        "hole_spacing" = 35.0
        "bracket_name" = "speaker_basic_mount"
    }
}

# Define bot-specific shell characteristics
$BotShells = @{
    "goblin" = @{
        "eye_style" = "craggy_socket"
        "nose_style" = "warty_protrusion"
        "mouth_style" = "fanged_maw"
        "surface_texture" = "rough_organic"
        "color_scheme" = "dark_green"
    }
    "bear" = @{
        "eye_style" = "rounded_socket"
        "nose_style" = "bear_snout"
        "mouth_style" = "gentle_curve"
        "surface_texture" = "fur_pattern"
        "color_scheme" = "brown"
    }
    "cat" = @{
        "eye_style" = "almond_socket"
        "nose_style" = "feline_triangle"
        "mouth_style" = "whisker_frame"
        "surface_texture" = "smooth_fur"
        "color_scheme" = "orange_tabby"
    }
    "cyclops" = @{
        "eye_style" = "massive_central"
        "nose_style" = "minimal_ridge"
        "mouth_style" = "wide_grimace"
        "surface_texture" = "scarred_flesh"
        "color_scheme" = "pale_gray"
    }
}

# Create directories
$ScadBasicDir = "assets\shapes\scad\basic_mounts"
$ScadBotShellsDir = "assets\shapes\scad\bot_shells"
$StlBasicDir = "assets\shapes\stl\basic_mounts"
$StlBotShellsDir = "assets\shapes\stl\bot_shells"

@($ScadBasicDir, $ScadBotShellsDir, $StlBasicDir, $StlBotShellsDir) | ForEach-Object {
    if (-not (Test-Path $_)) {
        New-Item -ItemType Directory -Path $_ -Force | Out-Null
        Write-Host "Created directory: $_" -ForegroundColor Green
    }
}

# Generate Basic Mounting Brackets (Tier 1)
if ($GenerateBasicMounts) {
    Write-Host "`n=== Generating Basic Mounting Brackets ===" -ForegroundColor Cyan
    
    # Display Basic Mount
    $DisplayMountScad = @"
// Basic Display Mount - Generic GC9A01 Mounting Bracket
// Reusable mounting solution for 1.28" circular displays
// Multi-Color 3D Printing: Uses HTML named colors for 4-color system

// Color palette parameters (can be overridden via command line)
PRIMARY_COLOR = "DarkOliveGreen";      // Color 1: Main mounting ring
SECONDARY_COLOR = "SaddleBrown";       // Color 2: Reinforcement tabs  
ACCENT_COLOR = "Goldenrod";            // Color 3: (unused in basic mount)
HIGHLIGHT_COLOR = "OrangeRed";         // Color 4: (unused in basic mount)

module display_basic_mount() {
    diameter = $($MountingSpecs.gc9a01_display.diameter);
    thickness = $($MountingSpecs.gc9a01_display.thickness);
    mount_holes = $($MountingSpecs.gc9a01_display.mount_holes);
    hole_diameter = $($MountingSpecs.gc9a01_display.hole_diameter);
    hole_spacing = $($MountingSpecs.gc9a01_display.hole_spacing);
    
    difference() {
        // Main mounting ring with reinforcement tabs
        union() {
            cylinder(h=thickness*3, d=diameter+8);
            // Reinforcement tabs for case mounting
            for(i=[0:3]) {
                rotate([0, 0, i*90 + 45]) {
                    translate([(diameter+8)/2, 0, 0])
                        cylinder(h=thickness*2, d=6);
                }
            }
        }
        
        // Display cutout (tight fit)
        translate([0,0,-1]) 
            cylinder(h=thickness*3+2, d=diameter+0.2);
        
        // Display mounting holes
        for(i=[0:mount_holes-1]) {
            rotate([0, 0, i*(360/mount_holes)]) {
                translate([hole_spacing/2, 0, -1])
                    cylinder(h=thickness*3+2, d=hole_diameter);
            }
        }
        
        // Case mounting holes (M3 screws in reinforcement tabs)
        for(i=[0:3]) {
            rotate([0, 0, i*90 + 45]) {
                translate([(diameter+8)/2, 0, thickness*2.5])
                    cylinder(h=thickness+1, d=3.2);
                // Countersink
                translate([(diameter+8)/2, 0, thickness*2.5])
                    cylinder(h=2, d=6);
            }
        }
        
        // Cable management channel
        translate([diameter/2 + 4, 0, thickness])
            cube([4, 3, thickness*2], center=true);
    }
}

// Generate the mount with primary color
color(PRIMARY_COLOR) display_basic_mount();
"@
    
    $DisplayMountPath = "$ScadBasicDir\display_basic_mount.scad"
    $DisplayMountScad | Out-File -FilePath $DisplayMountPath -Encoding ASCII
    Write-Host "Generated: $DisplayMountPath" -ForegroundColor Green
    
    # Sensor Basic Mount
    $SensorMountScad = @"
// Basic Sensor Mount - Generic HC-SR04 Mounting Bracket
// Reusable mounting solution for ultrasonic proximity sensors

module sensor_basic_mount() {
    width = $($MountingSpecs.hc_sr04_sensor.width);
    height = $($MountingSpecs.hc_sr04_sensor.height);
    depth = $($MountingSpecs.hc_sr04_sensor.depth);
    trans_diameter = $($MountingSpecs.hc_sr04_sensor.transducer_diameter);
    trans_spacing = $($MountingSpecs.hc_sr04_sensor.transducer_spacing);
    
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
"@
    
    $SensorMountPath = "$ScadBasicDir\sensor_basic_mount.scad"
    $SensorMountScad | Out-File -FilePath $SensorMountPath -Encoding ASCII
    Write-Host "Generated: $SensorMountPath" -ForegroundColor Green
    
    # Speaker Basic Mount
    $SpeakerMountScad = @"
// Basic Speaker Mount - Generic 40mm Speaker Mounting Bracket
// Reusable mounting solution for round speakers

module speaker_basic_mount() {
    diameter = $($MountingSpecs.speaker.diameter);
    depth = $($MountingSpecs.speaker.depth);
    mount_holes = $($MountingSpecs.speaker.mount_holes);
    hole_diameter = $($MountingSpecs.speaker.hole_diameter);
    hole_spacing = $($MountingSpecs.speaker.hole_spacing);
    
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
"@
    
    $SpeakerMountPath = "$ScadBasicDir\speaker_basic_mount.scad"
    $SpeakerMountScad | Out-File -FilePath $SpeakerMountPath -Encoding ASCII
    Write-Host "Generated: $SpeakerMountPath" -ForegroundColor Green
}

# Generate Bot-Specific Decorative Shells (Tier 2)
if ($GenerateBotShells) {
    Write-Host "`n=== Generating Bot-Specific Decorative Shells ===" -ForegroundColor Cyan
    
    $BotConfig = $BotShells[$BotType]
    if (-not $BotConfig) {
        Write-Host "ERROR: Unknown bot type '$BotType'" -ForegroundColor Red
        return
    }
    
    # Goblin Eye Shell (incorporates display_basic_mount)
    if ($BotType -eq "goblin") {
        $GoblinEyeShell = @"
// Goblin Eye Shell - Craggy socket incorporating display_basic_mount
// Character-specific decorative shell for goblin eye displays

use <../basic_mounts/display_basic_mount.scad>

module goblin_eye_shell(is_left_eye = true) {
    eye_socket_diameter = 45;
    socket_depth = 12;
    crag_count = 8;
    
    difference() {
        union() {
            // Main craggy eye socket
            difference() {
                // Outer socket shape
                hull() {
                    cylinder(h=socket_depth, d=eye_socket_diameter);
                    // Slightly oval shape
                    scale([1.1, 0.9, 1])
                        cylinder(h=socket_depth*0.8, d=eye_socket_diameter*0.9);
                }
                
                // Inner socket cavity for basic mount
                translate([0,0,socket_depth*0.4])
                    cylinder(h=socket_depth*0.6+1, d=eye_socket_diameter*0.75);
            }
            
            // Craggy surface details
            for(i = [0:crag_count-1]) {
                rotate([0, 0, i*(360/crag_count) + random(30)]) {
                    translate([eye_socket_diameter*0.4, 0, random(socket_depth*0.3)])
                        rotate([random(20), random(20), 0])
                            cylinder(h=random(4)+2, d=random(3)+2);
                }
            }
            
            // Brow ridge (more pronounced for left eye)
            if (is_left_eye) {
                translate([0, eye_socket_diameter*0.3, socket_depth*0.8])
                    rotate([15, 0, 0])
                        scale([1.2, 0.3, 0.4])
                            cylinder(h=6, d=eye_socket_diameter*0.8);
            } else {
                translate([0, eye_socket_diameter*0.25, socket_depth*0.8])
                    rotate([10, 0, 0])
                        scale([1.1, 0.25, 0.35])
                            cylinder(h=5, d=eye_socket_diameter*0.75);
            }
        }
        
        // Mounting cavity for basic display mount
        translate([0,0,socket_depth*0.4])
            cylinder(h=socket_depth*0.6+1, d=eye_socket_diameter*0.75);
        
        // Access holes for basic mount screws
        for(i=[0:3]) {
            rotate([0, 0, i*90 + 45]) {
                translate([eye_socket_diameter*0.25, 0, socket_depth*0.6])
                    cylinder(h=socket_depth*0.4+1, d=4);
            }
        }
    }
    
    // Include the basic mount (positioned inside shell)
    translate([0,0,socket_depth*0.4])
        display_basic_mount();
}

// Generate left and right eye shells
translate([-30, 0, 0]) goblin_eye_shell(true);   // Left eye
translate([30, 0, 0]) goblin_eye_shell(false);   // Right eye

// Helper function for random values (simplified)
function random(max_val) = max_val * (sin($t*137.5) + 1) / 2;
"@
        
        $GoblinEyePath = "$ScadBotShellsDir\goblin_eye_shells.scad"
        $GoblinEyeShell | Out-File -FilePath $GoblinEyePath -Encoding ASCII
        Write-Host "Generated: $GoblinEyePath" -ForegroundColor Green
        
        # Goblin Nose Shell (incorporates sensor_basic_mount)
        $GoblinNoseShell = @"
// Goblin Nose Shell - Warty protrusion incorporating sensor_basic_mount
// Character-specific decorative shell for goblin nose sensor

use <../basic_mounts/sensor_basic_mount.scad>

module goblin_nose_shell() {
    nose_width = 60;
    nose_height = 35;
    nose_depth = 25;
    wart_count = 12;
    
    difference() {
        union() {
            // Main warty nose shape
            hull() {
                // Base attachment
                translate([0, 0, 0])
                    cylinder(h=5, d=nose_width*0.8);
                // Nose tip
                translate([0, nose_height*0.6, nose_depth*0.8])
                    sphere(d=nose_width*0.4);
                // Bridge
                translate([0, nose_height*0.3, nose_depth*0.4])
                    scale([0.8, 0.6, 0.5])
                        sphere(d=nose_width*0.7);
            }
            
            // Warty surface details
            for(i = [0:wart_count-1]) {
                wart_x = (random(nose_width) - nose_width/2) * 0.8;
                wart_y = random(nose_height) * 0.7;
                wart_z = random(nose_depth) * 0.6 + 3;
                
                translate([wart_x, wart_y, wart_z])
                    sphere(d=random(4)+2);
            }
            
            // Nostril ridges
            for(side = [-1, 1]) {
                translate([side * nose_width*0.15, nose_height*0.7, nose_depth*0.6])
                    rotate([0, side*15, 0])
                        scale([0.3, 0.4, 0.6])
                            cylinder(h=8, d=6);
            }
        }
        
        // Sensor cavity (positioned for optimal detection)
        translate([0, nose_height*0.4, nose_depth*0.3])
            rotate([15, 0, 0]) {
                // Cavity for basic sensor mount
                translate([-22.5, -10, -7.5])
                    cube([45, 20, 15]);
                
                // Sensor transducer openings
                translate([-12, 0, 0])
                    cylinder(h=20, d=17, center=true);
                translate([12, 0, 0])
                    cylinder(h=20, d=17, center=true);
            }
        
        // Mounting screw access
        translate([0, nose_height*0.4, nose_depth*0.3])
            rotate([15, 0, 0]) {
                for(side = [-1, 1]) {
                    translate([side * 18, 0, 0])
                        rotate([0, 90, 0])
                            cylinder(h=15, d=4, center=true);
                }
            }
        
        // Nostrils (for character authenticity)
        for(side = [-1, 1]) {
            translate([side * nose_width*0.12, nose_height*0.75, nose_depth*0.7])
                rotate([45, side*10, 0])
                    cylinder(h=8, d=4);
        }
    }
    
    // Include the basic sensor mount (positioned inside shell)
    translate([0, nose_height*0.4, nose_depth*0.3])
        rotate([15, 0, 0])
            translate([-22.5, -10, -7.5])
                sensor_basic_mount();
}

// Generate the nose shell
goblin_nose_shell();

// Helper function for random values
function random(max_val) = max_val * (sin($t*173.7 + max_val*7.3) + 1) / 2;
"@
        
        $GoblinNosePath = "$ScadBotShellsDir\goblin_nose_shell.scad"
        $GoblinNoseShell | Out-File -FilePath $GoblinNosePath -Encoding ASCII
        Write-Host "Generated: $GoblinNosePath" -ForegroundColor Green
        
        # Goblin Mouth Shell (incorporates display_basic_mount)
        $GoblinMouthShell = @"
// Goblin Mouth Shell - Fanged maw incorporating display_basic_mount
// Character-specific decorative shell for goblin mouth display

use <../basic_mounts/display_basic_mount.scad>

module goblin_mouth_shell() {
    mouth_width = 55;
    mouth_height = 40;
    mouth_depth = 15;
    fang_count = 6;
    
    difference() {
        union() {
            // Main mouth frame
            difference() {
                // Outer mouth shape (slightly oval)
                scale([1.3, 1.0, 1])
                    cylinder(h=mouth_depth, d=mouth_width);
                
                // Inner cavity for basic mount
                translate([0,0,mouth_depth*0.3])
                    scale([1.2, 0.95, 1])
                        cylinder(h=mouth_depth*0.7+1, d=mouth_width*0.85);
            }
            
            // Fang details around the rim
            for(i = [0:fang_count-1]) {
                angle = i * (360/fang_count);
                // Skip bottom center (no fangs on lower lip center)
                if (angle < 150 || angle > 210) {
                    rotate([0, 0, angle]) {
                        translate([mouth_width*0.4, 0, mouth_depth*0.8])
                            rotate([random(15)+10, 0, 0])
                                cylinder(h=random(8)+4, d1=3, d2=1);
                    }
                }
            }
            
            // Lip ridges
            for(angle = [30:60:330]) {
                rotate([0, 0, angle]) {
                    translate([mouth_width*0.35, 0, mouth_depth*0.6])
                        rotate([0, 90, 0])
                            scale([0.6, 0.3, 1])
                                cylinder(h=6, d=4);
                }
            }
            
            // Corner warts
            for(side = [-1, 1]) {
                translate([side * mouth_width*0.5, mouth_width*0.1, mouth_depth*0.7])
                    sphere(d=5);
            }
        }
        
        // Mounting cavity for basic display mount
        translate([0,0,mouth_depth*0.3])
            scale([1.15, 0.9, 1])
                cylinder(h=mouth_depth*0.7+1, d=mouth_width*0.85);
        
        // Access holes for basic mount screws
        for(i=[0:3]) {
            rotate([0, 0, i*90 + 45]) {
                translate([mouth_width*0.3, 0, mouth_depth*0.6])
                    cylinder(h=mouth_depth*0.4+1, d=4);
            }
        }
        
        // Central display opening (slightly smaller for menacing look)
        translate([0,0,-1])
            scale([1.1, 0.85, 1])
                cylinder(h=mouth_depth+2, d=mouth_width*0.7);
    }
    
    // Include the basic mount (positioned inside shell)
    translate([0,0,mouth_depth*0.3])
        display_basic_mount();
}

// Generate the mouth shell
goblin_mouth_shell();

// Helper function for random values
function random(max_val) = max_val * (sin($t*211.3 + max_val*11.7) + 1) / 2;
"@
        
        $GoblinMouthPath = "$ScadBotShellsDir\goblin_mouth_shell.scad"
        $GoblinMouthShell | Out-File -FilePath $GoblinMouthPath -Encoding ASCII
        Write-Host "Generated: $GoblinMouthPath" -ForegroundColor Green
    }
}

Write-Host "`n=== Mounting System Generation Complete ===" -ForegroundColor Cyan
Write-Host "Basic Mounts: $ScadBasicDir" -ForegroundColor Yellow
Write-Host "Bot Shells: $ScadBotShellsDir" -ForegroundColor Yellow

if ($OpenSCADPreview -and (Get-Command "openscad" -ErrorAction SilentlyContinue)) {
    Write-Host "`nOpening OpenSCAD preview..." -ForegroundColor Green
    Start-Process "openscad" "$ScadBotShellsDir\goblin_eye_shells.scad"
}

Write-Host "`nNext steps:" -ForegroundColor Cyan
Write-Host "1. Review generated .scad files" -ForegroundColor White
Write-Host "2. Run STL generation: openscad -o file.stl file.scad" -ForegroundColor White  
Write-Host "3. Test fit with actual hardware" -ForegroundColor White
Write-Host "4. Print and assemble two-tier mounting system" -ForegroundColor White
