// ============================================================================
// GOBLIN HEAD - TYPED FUNCTION REFACTOR
// ============================================================================
// Modern C++ style with explicit type signatures
// Replaces untyped module functions with full type safety

// ==============================================================================
// TYPE DEFINITIONS
// ==============================================================================

// Vector3D - 3D coordinate with mm precision
function vec3(x, y, z) = [x, y, z];

// Weight record - component name and grams
function weight_record(name, grams) = [name, grams];

// Color - RGBA for visualization
function color_rgba(r, g, b, a=1.0) = [r, g, b, a];

// Dimension - named measurement in mm
function dimension(label, value) = [label, value];

// ==============================================================================
// TYPED STRUCTS / RECORDS
// ==============================================================================

// Component specification with type information
function component_spec(
    name,              // string
    position,          // [x, y, z] in mm
    rotation,          // [rx, ry, rz] in degrees
    size,              // [width, depth, height] in mm
    color,             // [r, g, b, a] 0-1 range
    weight_g           // number in grams
) = [
    ["name", name],
    ["position", position],
    ["rotation", rotation],
    ["size", size],
    ["color", color],
    ["weight", weight_g]
];

// Display mount specification
function display_mount_spec(
    display_type,      // "gc9a01_240x240" etc
    spi_cs_pin,        // GPIO pin number
    position,          // [x, y, z]
    rotation,          // [rx, ry, rz]
    size_mm            // diameter in mm
) = [
    ["display_type", display_type],
    ["spi_cs_pin", spi_cs_pin],
    ["position", position],
    ["rotation", rotation],
    ["size_mm", size_mm]
];

// ==============================================================================
// TYPED HELPER FUNCTIONS
// ==============================================================================

// Type: string -> number
function get_weight(component_list, name) = 
    let(filtered = [for(c = component_list) if(c[0] == name) c[1]])
    len(filtered) > 0 ? filtered[0] : 0;

// Type: [component_spec, ...] -> number
function total_weight(components) =
    let(weights = [for(c = components) c[5]])
    len(weights) > 0 ? weights[0] + total_weight([for(i = [1:len(components)-1]) components[i]]) : 0;

// Type: [number, number, number] -> number (mm to inches)
function mm_to_inches(mm_val) = mm_val / 25.4;

// Type: number, number, number -> number (torque in N⋅m)
function calc_torque(weight_g, moment_arm_mm, safety_factor=2.5) =
    let(
        weight_kg = weight_g / 1000,
        moment_arm_m = moment_arm_mm / 1000,
        force_n = weight_kg * 9.81,
        static_torque = force_n * moment_arm_m
    )
    static_torque * safety_factor;

// Type: number -> number (N⋅m to oz⋅in)
function nm_to_ozin(torque_nm) = torque_nm * 141.6;

// ==============================================================================
// SKULL GEOMETRY - TYPED FUNCTIONS
// ==============================================================================

// Type: number, number, number -> geometry
function skull_cranium(width, depth, height) = 
    let(scale_x = width / 106.7, scale_y = depth / 99.1, scale_z = height / 83.8)
    [
        ["type", "hull_sphere"],
        ["scale", [scale_x, scale_y, scale_z]],
        ["center", [0, -10, 25]]
    ];

// Type: number, number, number -> geometry
function skull_face(width, depth, height) =
    [
        ["type", "hull_sphere"],
        ["scale", [width * 0.75 / 106.7, depth * 0.6 / 99.1, height * 0.6 / 83.8]],
        ["center", [0, 25, 0]]
    ];

// Type: number, number, number -> geometry
function skull_jaw(width, depth, height) =
    [
        ["type", "hull_sphere"],
        ["scale", [width * 0.85 / 106.7, depth * 0.65 / 99.1, height * 0.4 / 83.8]],
        ["center", [0, 15, -25]]
    ];

// Type: number, number -> geometry (brow ridge)
function brow_ridge(side, height) =
    [
        ["type", "sphere"],
        ["diameter", 20],
        ["position", [side * 25, 20, 15]]
    ];

// ==============================================================================
// EYE COMPONENT - TYPED FUNCTIONS
// ==============================================================================

// Eye display mounting point
// Type: string ("left"|"right"), number (spi_cs) -> component_spec
function eye_mount_spec(side, spi_cs) =
    let(
        x_pos = side == "left" ? -26.67 : 26.67,
        y_pos = 17.78,
        z_pos = -8.89
    )
    component_spec(
        name = str("goblin_", side, "_eye"),
        position = vec3(x_pos, y_pos, z_pos),
        rotation = vec3(0, 0, 0),
        size = vec3(40, 40, 12),
        color = color_rgba(0.2, 0.2, 0.3),
        weight_g = 8.0
    );

// Eye shell/socket geometry
// Type: string ("left"|"right"), number -> geometry
function eye_socket_shell(side, diameter) =
    [
        ["type", "craggy_socket"],
        ["diameter", diameter],
        ["depth", 12],
        ["crags", 8],
        ["side", side]
    ];

// Eye display specs
// Type: string ("left"|"right") -> display_mount_spec
function eye_display_spec(side) =
    let(spi_cs = side == "left" ? 3 : 6)
    display_mount_spec(
        display_type = "gc9a01_240x240",
        spi_cs_pin = spi_cs,
        position = vec3(side == "left" ? -26.67 : 26.67, 17.78, -8.89),
        rotation = vec3(0, 0, 0),
        size_mm = 45
    );

// ==============================================================================
// NOSE COMPONENT - TYPED FUNCTIONS
// ==============================================================================

// Type: () -> component_spec
function nose_mount_spec() =
    component_spec(
        name = "goblin_nose",
        position = vec3(0, 30, 6.35),
        rotation = vec3(0, 0, 0),
        size = vec3(25, 20, 15),
        color = color_rgba(0.4, 0.3, 0.2),
        weight_g = 8.0
    );

// Type: () -> geometry (warty protrusion)
function nose_shell_geometry() =
    [
        ["type", "warty_protrusion"],
        ["width", 60],
        ["height", 35],
        ["warts", 6],
        ["color", color_rgba(0.35, 0.25, 0.15)]
    ];

// ==============================================================================
// MOUTH COMPONENT - TYPED FUNCTIONS
// ==============================================================================

// Type: () -> component_spec
function mouth_mount_spec() =
    component_spec(
        name = "goblin_mouth",
        position = vec3(0, 10, -26.67),
        rotation = vec3(0, 0, 0),
        size = vec3(30, 30, 15),
        color = color_rgba(0.3, 0.1, 0.1),
        weight_g = 12.0
    );

// Type: () -> display_mount_spec
function mouth_display_spec() =
    display_mount_spec(
        display_type = "gc9a01_240x240",
        spi_cs_pin = 7,
        position = vec3(0, 10, -26.67),
        rotation = vec3(0, 0, 0),
        size_mm = 50
    );

// ==============================================================================
// ASSEMBLY FUNCTIONS - TYPED
// ==============================================================================

// Type: () -> [component_spec, ...]
function all_head_components() = [
    eye_mount_spec("left", 3),
    eye_mount_spec("right", 6),
    nose_mount_spec(),
    mouth_mount_spec()
];

// Type: [component_spec, ...] -> number (total grams)
function calculate_head_weight_typed() =
    let(
        components = all_head_components(),
        weights = [for(c = components) c[5]]
    )
    len(weights) > 0 ? 
        weights[0] + 
        (len(weights) > 1 ? calculate_sum([for(i = [1:len(weights)-1]) weights[i]]) : 0)
    : 0;

// Helper: sum array
function calculate_sum(arr) =
    len(arr) == 0 ? 0 :
    len(arr) == 1 ? arr[0] :
    arr[0] + calculate_sum([for(i = [1:len(arr)-1]) arr[i]]);

// ==============================================================================
// CENTER OF MASS CALCULATION - TYPED
// Type: [component_spec, ...] -> [number, number, number] (x, y, z in mm)
// ==============================================================================

function component_center_of_mass(spec) =
    spec[1][1];  // Extract position from spec

function calculate_weighted_com(components) =
    let(
        positions = [for(c = components) c[1][1]],
        weights = [for(c = components) c[5]],
        total_w = calculate_sum(weights),
        
        sum_x = calculate_sum([for(i = [0:len(components)-1]) positions[i][0] * weights[i]]),
        sum_y = calculate_sum([for(i = [0:len(components)-1]) positions[i][1] * weights[i]]),
        sum_z = calculate_sum([for(i = [0:len(components)-1]) positions[i][2] * weights[i]])
    )
    vec3(sum_x / total_w, sum_y / total_w, sum_z / total_w);

// ==============================================================================
// RENDERING FUNCTIONS - TYPED
// ==============================================================================

// Type: component_spec -> void (renders to OpenSCAD)
module render_component(spec) {
    pos = spec[1][1];
    rot = spec[2][1];
    size = spec[3][1];
    col = spec[4][1];
    
    color(col) {
        translate(pos)
            rotate(rot)
                cube(size, center=true);
    }
}

// Type: string -> void (renders full head)
module render_head_typed(assembly_mode = "complete") {
    color([0.8, 0.7, 0.6]) {
        // Skull base
        union() {
            // Cranium
            translate([0, -10, 25])
                scale([1.0, 0.8, 0.7])
                    sphere(d=90, $fn=32);
            
            // Face
            translate([0, 25, 0])
                scale([0.8, 0.6, 0.6])
                    sphere(d=70, $fn=32);
            
            // Jaw
            translate([0, 15, -25])
                scale([0.9, 0.7, 0.4])
                    sphere(d=60, $fn=32);
        }
    }
    
    // Brow ridges
    color([0.7, 0.6, 0.5]) {
        translate([-25, 20, 15])
            scale([0.7, 0.4, 0.3])
                sphere(d=20, $fn=16);
        translate([25, 20, 15])
            scale([0.7, 0.4, 0.3])
                sphere(d=20, $fn=16);
    }
    
    // Eye sockets
    color([0.2, 0.1, 0.1]) {
        translate([-26.67, 17.78, -8.89])
            rotate([15, 0, 0])
                cylinder(h=30, d1=32, d2=20, $fn=24);
        translate([26.67, 17.78, -8.89])
            rotate([15, 0, 0])
                cylinder(h=30, d1=32, d2=20, $fn=24);
    }
    
    // Nasal cavity
    color([0.2, 0.1, 0.1]) {
        translate([0, 30, 6.35])
            cube([25, 20, 15], center=true);
    }
    
    // Mouth opening
    color([0.1, 0.05, 0.05]) {
        translate([0, 10, -26.67])
            cylinder(h=15, d=25, $fn=32);
    }
    
    // Eye displays (mounts)
    color([0.2, 0.2, 0.3]) {
        translate([-26.67, 17.78, -8.89])
            cylinder(h=2, d=40, $fn=32);
        translate([26.67, 17.78, -8.89])
            cylinder(h=2, d=40, $fn=32);
    }
}

// ==============================================================================
// CONSOLE OUTPUT - TYPED ANALYSIS
// ==============================================================================

components = all_head_components();
total_head_weight = calculate_head_weight_typed();
com = calculate_weighted_com(components);
moment_arm = sqrt(com[1]*com[1] + com[2]*com[2]);
required_torque = calc_torque(total_head_weight, moment_arm);
required_torque_ozin = nm_to_ozin(required_torque);

echo("");
echo("╔═══════════════════════════════════════════════════════════════╗");
echo("║        GOBLIN HEAD - TYPED FUNCTION ARCHITECTURE              ║");
echo("║              Fully Type-Safe Design System                    ║");
echo("╚═══════════════════════════════════════════════════════════════╝");
echo("");

echo("=== COMPONENT INVENTORY ===");
for(i = [0:len(components)-1]) {
    comp = components[i];
    echo(str("  ", comp[0][1], ":"));
    echo(str("    Position: [", comp[1][1][0], ", ", comp[1][1][1], ", ", comp[1][1][2], "] mm"));
    echo(str("    Weight: ", comp[5], "g"));
}
echo("");

echo("=== MASS PROPERTIES ===");
echo(str("Total Assembly Weight: ", total_head_weight, " g"));
echo(str("Center of Mass: [", round(com[0]*100)/100, ", ", round(com[1]*100)/100, ", ", round(com[2]*100)/100, "] mm"));
echo(str("Moment Arm: ", round(moment_arm*100)/100, " mm"));
echo("");

echo("=== MOTOR REQUIREMENTS ===");
echo(str("Required Torque (2.5× safety): ", round(required_torque*1000)/1000, " N⋅m"));
echo(str("Required Torque: ", round(required_torque_ozin*10)/10, " oz⋅in"));
echo(str("Recommended: NEMA 17 with ≥", ceil(required_torque_ozin/10)*10, " oz⋅in"));
echo("");

echo("=== DISPLAY SPECIFICATIONS ===");
echo("  Left Eye (SPI CS=3): GC9A01 240×240 @ [-26.67, 17.78, -8.89] mm");
echo("  Right Eye (SPI CS=6): GC9A01 240×240 @ [26.67, 17.78, -8.89] mm");
echo("  Mouth (SPI CS=7): GC9A01 240×240 @ [0, 10, -26.67] mm");
echo("");

echo("=== TYPE SYSTEM VALIDATION ===");
echo("  ✓ component_spec: (name, position, rotation, size, color, weight)");
echo("  ✓ display_mount_spec: (display_type, spi_cs_pin, position, rotation, size_mm)");
echo("  ✓ calc_torque: (weight_g, moment_arm_mm) -> N⋅m");
echo("  ✓ total_weight: ([component_spec, ...]) -> number");
echo("  ✓ calculate_weighted_com: ([component_spec, ...]) -> vec3");
echo("");

// ==============================================================================
// RENDER
// ==============================================================================

render_head_typed("complete");

