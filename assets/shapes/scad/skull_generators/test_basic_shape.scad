// Test file to verify OpenSCAD is working correctly
// This should render as a red cube, not a sphere

color("red") 
cube([20, 20, 20], center=true);

// Add text to prove this is rendering correctly
color("white")
translate([0, 0, 12])
linear_extrude(height=2)
text("TEST", size=5, halign="center");