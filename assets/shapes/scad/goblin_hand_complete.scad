// FINAL GOBLIN HAND - Complete Assembly
// Combines: base structure + linkage mechanism + wrist controller
// Ready for 3D printing and servo integration

include <goblin_hand_base.scad>
include <goblin_hand_linkage.scad>
include <goblin_hand_wrist_assembly.scad>

// ==============================================================================
// MASTER ASSEMBLY
// ==============================================================================

module complete_goblin_hand_assembly(show_exploded=false, hand_side="left") {
    echo("");
    echo("╔════════════════════════════════════════════════════════════╗");
    echo("║         COMPLETE GOBLIN HAND ASSEMBLY                     ║");
    echo("║  Final Design: 3 Servos, Push-Pull Cable Control          ║");
    echo("║  Thermal Budget: 25W max per hand                         ║");
    echo("║  Power Draw: 2-15W typical, 20W peak                      ║");
    echo("╚════════════════════════════════════════════════════════════╝");
    echo("");
    
    // Part 1: Hand Base Structure
    color([0.9, 0.8, 0.7]) {
        if(show_exploded) {
            translate([0, 0, 0])
                goblin_hand_assembly(hand_side);
        } else {
            goblin_hand_assembly(hand_side);
        }
    }
    
    // Part 2: Control Linkage & Cables
    color([0.5, 0.3, 0.3], 0.7) {
        if(show_exploded) {
            translate([0, 0, 30])
                complete_hand_linkages();
        } else {
            complete_hand_linkages();
        }
    }
    
    // Part 3: Wrist Assembly & Controller
    color([0.4, 0.4, 0.4]) {
        if(show_exploded) {
            translate([0, -40, 0])
                goblin_hand_wrist_assembly();
        } else {
            goblin_hand_wrist_assembly();
        }
    }
}

// ==============================================================================
// COMPONENT BREAKDOWN FOR MANUFACTURING
// ==============================================================================

echo("");
echo("=== MANUFACTURING BILL OF MATERIALS ===");
echo("");
echo("3D PRINTED PARTS (PLA, 20% Infill):");
echo("  1× goblin_hand_palm_base.stl");
echo("     └─ Print time: ~2 hours, Weight: ~25g");
echo("  1× goblin_hand_fingers.stl");
echo("     └─ Print time: ~1.5 hours, Weight: ~18g");
echo("  1× goblin_hand_thumb.stl");
echo("     └─ Print time: ~45 minutes, Weight: ~8g");
echo("  1× wrist_connector.stl");
echo("     └─ Print time: ~1 hour, Weight: ~12g");
echo("  3× servo_mounting_bracket.stl");
echo("     └─ Print time: ~30 min each, Weight: ~3g each");
echo("");

echo("ELECTRONICS:");
echo("  3× SG90 Servo Motor (22.8×11.6×10.2mm)");
echo("     └─ Cost: ~$3 each, Weight: ~9g each");
echo("  1× ESP32-S3 DevKit");
echo("     └─ Cost: ~$8, Weight: ~20g");
echo("  1× Custom Servo Driver PCB");
echo("     └─ 3× PWM outputs, Power filtering, Thermal sensor");
echo("     └─ Cost: ~$12, Weight: ~8g");
echo("  1× XT30 Power Connector (5V in)");
echo("  1× 4-pin I2C Connector (signal return)");
echo("");

echo("HARDWARE:");
echo("  12× M3 Screws (6-10mm)");
echo("  6× M3 Threaded Inserts");
echo("  3× Spectra/Dyneema Cable (2mm diameter, 2-3 meters)");
echo("  3× Springs (return, ~1-2N tension)");
echo("  12× Washers");
echo("  3× M3 Nuts");
echo("");

echo("TOTALS:");
hand_weight = estimate_hand_weight(3);
wrist_weight = estimate_wrist_assembly_weight();
total_weight = hand_weight + wrist_weight;

echo(str("  Hand Structure Weight: ", hand_weight, "g"));
echo(str("  Wrist/Controller Weight: ", wrist_weight, "g"));
echo(str("  Total Hand Assembly: ", total_weight, "g"));
echo("");

// ==============================================================================
// SPECIFICATIONS SUMMARY
// ==============================================================================

echo("=== GOBLIN HAND SPECIFICATIONS ===");
echo("");
echo("DIMENSIONS:");
echo("  Palm Length: 95mm");
echo("  Palm Width: 65mm");
echo("  Palm Thickness: 18mm");
echo("  Finger Length: 72mm");
echo("  Thumb Length: 55mm");
echo("  Overall Reach (wrist to fingertip): 120mm");
echo("");

echo("MOTION CAPABILITIES:");
echo("  - Coupled finger curl (all fingers 0-90° together)");
echo("  - Independent pinky curl (0-90°)");
echo("  - Thumb opposition (45° angle to palm)");
echo("  - Wrist pitch (±45°, via arm ESP32)");
echo("  - Wrist yaw (±45°, via arm ESP32)");
echo("  - Wrist roll (±30°, via arm ESP32)");
echo("");

echo("GRIP STRENGTH:");
echo("  - Estimated maximum: 3-5 N grip force");
echo("  - Sufficient to grip: axe handle, sword, tool");
echo("  - Can hold indefinitely: yes (servos hold under load)");
echo("  - Load limit: thermal (25W → speed reduction)");
echo("");

echo("CONTROL:");
echo("  - Servo #1: Main finger curl (index+middle+ring)");
echo("  - Servo #2: Pinky curl");
echo("  - Servo #3: Thumb opposition");
echo("  - Response time: <500ms per servo command");
echo("  - Coordination: Local ESP32 (no latency)");
echo("");

echo("POWER & THERMAL:");
echo("  - Idle power: 0.1W");
echo("  - Light motion: 0.3W");
echo("  - Moderate grip: 2.3W");
echo("  - Peak grip: 6-7W");
echo("  - Thermal limit: 25W before speed reduction");
echo("  - Ambient temp: 18-27°C for full performance");
echo("");

echo("COMMUNICATION:");
echo("  - Input: Reads SharedMemory state (hand_gesture, grip_strength)");
echo("  - Output: Writes SharedMemory (hand_temp, hand_load, hand_status)");
echo("  - Protocol: ESP-NOW wireless mesh (no inter-hand wiring)");
echo("  - Latency: <100ms local compute, wireless ~10-50ms");
echo("");

echo("RELIABILITY:");
echo("  - Springs return fingers on power loss (safe)");
echo("  - Thermal throttling prevents plastic melt");
echo("  - Current limiting prevents motor damage");
echo("  - Resettable fuse protects circuits (5A rated)");
echo("");

// ==============================================================================
// ASSEMBLY INSTRUCTIONS (Text Output)
// ==============================================================================

echo("");
echo("=== ASSEMBLY SEQUENCE ===");
echo("");
echo("STEP 1: Print Components");
echo("  □ Print hand_palm_base.stl at 0.2mm layer height");
echo("  □ Print fingers (index+middle+ring bundle)");
echo("  □ Print thumb");
echo("  □ Print wrist_connector");
echo("  □ Print 3× servo_mounting_brackets");
echo("");

echo("STEP 2: Prepare Hand Base");
echo("  □ Remove print supports, sand smooth");
echo("  □ Install M3 threaded inserts at servo mount points (3 locations)");
echo("  □ Install M3 threaded inserts at wrist connection (4 locations)");
echo("  □ Test fit fingers into palm base (should slide smoothly)");
echo("");

echo("STEP 3: Install Servo Motors & Linkages");
echo("  □ Mount Servo #1 (main finger curl) at palm center");
echo("  □ Attach push-pull rod to servo horn");
echo("  □ Thread tension cables through cable guides");
echo("  □ Attach cable springs (return fingers to open)");
echo("  □ Mount Servo #2 (pinky) on left side");
echo("  □ Mount Servo #3 (thumb opposition) on right side");
echo("");

echo("STEP 4: Wire Electronic Components");
echo("  □ Mount ESP32-S3 onto servo controller PCB");
echo("  □ Solder 3× servo connectors to PWM outputs");
echo("  □ Install power filtering capacitors");
echo("  □ Mount thermal sensor (DS18B20) on PCB");
echo("  □ Test continuity: power pins, signal pins");
echo("");

echo("STEP 5: Install Wrist Assembly");
echo("  □ Install controller PCB inside wrist connector");
echo("  □ Route 5V power wire through wrist cavity");
echo("  □ Route I2C signal wire through wrist cavity");
echo("  □ Connect XT30 power connector to hand PCB");
echo("  □ Connect I2C connector to hand PCB");
echo("  □ Secure wrist connector to hand base (M3 screws)");
echo("");

echo("STEP 6: Connect Servos");
echo("  □ Connect Servo #1 to PWM ch0 (orange=signal, red=5V, black=GND)");
echo("  □ Connect Servo #2 to PWM ch1");
echo("  □ Connect Servo #3 to PWM ch2");
echo("  □ Test servo movement (all should respond to commands)");
echo("");

echo("STEP 7: Calibration & Testing");
echo("  □ Center all servos (90° neutral position)");
echo("  □ Adjust cable tension so fingers can fully open and close");
echo("  □ Test thermal sensor output (should read ~22-25°C at room temp)");
echo("  □ Test current sensor (should read 0A at idle, <0.5A per servo moving)");
echo("  □ Perform full range-of-motion test");
echo("");

echo("STEP 8: Final Assembly");
echo("  □ Secure all connectors with cable ties");
echo("  □ Verify no exposed wires or pinch points");
echo("  □ Test wrist connector securely fastened to arm");
echo("  □ Verify I2C communication working (no errors on serial monitor)");
echo("  □ READY FOR DEPLOYMENT");
echo("");

// ==============================================================================
// RENDER
// ==============================================================================

// Normal view (all components assembled)
complete_goblin_hand_assembly(false, "left");

// Uncomment below to see exploded view
// complete_goblin_hand_assembly(true, "left");

