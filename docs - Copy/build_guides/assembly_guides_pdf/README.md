# P32 Assembly Guides - PDF Distribution Files

This directory contains **finished, print-ready PDF files** for hardware kit distribution.

##  What's Included

### Complete Assembly Guides (3 PDFs)

1. **P32-Goblin-Head-Wiring-Guide.pdf** (~2-3 MB)
   - Complete step-by-step breadboard assembly instructions
   - Hardware inventory with BOM
   - GPIO pin assignments table
   - Display pin translation (SCK/SDACLK/MOSI)
   - Breadboard layout diagrams
   - Connection checklist with checkboxes
   - Testing procedures (4 phases)
   - Troubleshooting guide (5 common issues)
   - Safety warnings highlighted

2. **P32-Quick-Reference-Card.pdf** (~500 KB)
   - Printable single-page workbench reference
   - GPIO assignments at-a-glance
   - Wire color guide
   - Left/right eye wiring tables
   - Sensor wiring table
   - Critical safety notes
   - Quick troubleshooting
   - Component layout diagram
   - **Laminate this for workbench use!**

3. **P32-Visual-Wiring-Diagrams.pdf** (~1-2 MB)
   - ASCII art system diagrams
   - Complete system overview
   - Breadboard top view (component placement)
   - Left eye detailed connections (7 wires)
   - Right eye detailed connections (7 wires)
   - Nose sensor detailed connections (4 wires)
   - Power distribution diagram
   - Signal wire routing strategies
   - 3D perspective views
   - Wiring sequence with checkboxes

##  Source Files

These PDFs are generated from markdown files in the parent `docs/` directory:

| PDF File | Generated From | Lines |
|----------|---------------|-------|
| P32-Goblin-Head-Wiring-Guide.pdf | GOBLIN-HEAD-BREADBOARD-WIRING-GUIDE.md | 650 |
| P32-Quick-Reference-Card.pdf | WIRING-QUICK-REFERENCE.md | 235 |
| P32-Visual-Wiring-Diagrams.pdf | WIRING-VISUAL-DIAGRAM.md | 440 |

##  How to Regenerate

If you update the source markdown files, regenerate PDFs with:

```powershell
# From project root
.\tools\generate-assembly-pdf.ps1

# PDFs will be created in docs\assembly_guides\ (temp location)
# Then copy finished PDFs to this directory for distribution
```

**Note**: The script currently outputs to `docs\assembly_guides\`. After reviewing the PDFs, copy them here for distribution:

```powershell
# Review generated PDFs
explorer docs\assembly_guides

# Copy to distribution directory
Copy-Item docs\assembly_guides\*.pdf docs\assembly_guides_pdf\
```

##  Distribution Workflow

### For Physical Hardware Kits

1. **Print all 3 PDFs** (color recommended but not required)
2. **Laminate the Quick Reference Card** for durability
3. **Include all 3 in each kit** (stapled or bound together)
4. **Consider spiral binding** for Wiring Guide (lay-flat usage)

### For Digital Distribution

1. **Zip all 3 PDFs together**: `P32-Goblin-Head-Assembly-Guides.zip`
2. **Upload to GitHub Releases** as release assets
3. **Link from main README.md** for easy download
4. **Host on project website** if available

### Printing Recommendations

- **Paper**: 8.5"  11" letter size (US) or A4 (international)
- **Color**: Recommended for wire color identification
- **Quality**: 300 DPI minimum for clear diagrams
- **Binding**: 
  - Wiring Guide: Spiral or comb binding (lay-flat)
  - Quick Reference: Laminate and hole-punch
  - Visual Diagrams: Stapled is sufficient

##  PDF Features

All PDFs include:
-  Professional styling with table of contents
-  Page numbers on every page
-  Section numbering for easy reference
-  Syntax-highlighted code blocks
-  Print-optimized layout (0.75" margins)
-  ASCII diagrams preserved correctly
-  Zebra-striped tables for readability
-  Safety warnings highlighted in yellow

##  File Sizes

Typical sizes (may vary with content updates):

- **Wiring Guide**: 2-3 MB (many diagrams)
- **Quick Reference**: 500 KB - 1 MB (single page)
- **Visual Diagrams**: 1-2 MB (ASCII art)
- **All 3 Combined**: ~4-6 MB

##  Version Control

**Git Strategy**:
-  **Commit source markdown** (`.md` files) to Git
-  **Commit these finished PDFs** to Git (for releases)
-  **Don't commit temp PDFs** from `docs\assembly_guides\` (add to `.gitignore`)

**Rationale**: These PDFs are **finished distribution files** intended for end users, so they belong in version control. The temp PDFs in `assembly_guides\` are intermediate build artifacts.

### Suggested .gitignore Entry

```gitignore
# Ignore temporary PDF build artifacts
docs/assembly_guides/*.pdf
docs/assembly_guides/*.html
docs/assembly_guides/*.css

# But keep finished distribution PDFs
!docs/assembly_guides_pdf/*.pdf
```

##  Quality Checklist

Before distributing PDFs, verify:

- [ ] All 3 PDFs open correctly
- [ ] Table of contents is clickable
- [ ] Page numbers appear on all pages
- [ ] ASCII diagrams display correctly (monospace font)
- [ ] Tables fit on pages (not awkwardly split)
- [ ] Code blocks have proper formatting
- [ ] Safety warnings are highlighted (yellow background)
- [ ] Wire color codes are visible
- [ ] Test print looks professional
- [ ] File sizes are reasonable (<5 MB each)

##  Version History

Track major updates to the assembly guides here:

### v1.0 - October 15, 2025
- Initial release
- Goblin Head v1.0 breadboard assembly
- Components: 2x GC9A01 displays, 1x HC-SR04 sensor
- GPIO assignments: 8 pins used (14, 13, 26, 27, 15, 33, 32, 25, 34)
- Power: ESP32-S3 3.3V direct (no external regulator)

### Future Versions

When updating:
1. Update source markdown files
2. Regenerate PDFs with `.\tools\generate-assembly-pdf.ps1`
3. Review all 3 PDFs carefully
4. Test print one copy
5. Copy to this directory
6. Commit to Git with version tag
7. Create GitHub release

##  Related Documentation

- **PDF Generation Workflow**: `../PDF-GENERATION-WORKFLOW.md` (450 lines)
- **Quick Start Guide**: `../PDF-QUICK-START.md` (150 lines)
- **Documentation Index**: `../PDF-DOCUMENTATION-INDEX.md` (300 lines)
- **Power Distribution**: `../POWER-DISTRIBUTION-CLARIFICATION.md` (250 lines)

##  Tips for Kit Builders

### For First-Time Assembly

1. **Start with Quick Reference Card** - Get familiar with pin layout
2. **Read Visual Diagrams** - Understand overall system architecture
3. **Follow Wiring Guide step-by-step** - Don't skip ahead!
4. **Use checklist format** - Check off each connection as you go
5. **Test each phase** - Power, displays, sensor (incrementally)

### Common Mistakes to Avoid

 **Don't use 5V on display VCC pins** - Displays are 3.3V only!  
 **Don't connect GPIO 34 as output** - It's input-only  
 **Don't forget shared pins** - GPIO 14/13/26 connect to BOTH eyes  
 **Don't skip power rail setup** - Foundation for everything  
 **Don't hot-swap components** - Power off before changes  

##  Next Steps After Assembly

Once you've successfully wired the breadboard:

1. **Flash firmware** to ESP32-S3
2. **Test displays** with initialization code
3. **Verify sensor readings** in serial monitor
4. **Calibrate mood system** (sensor triggers expressions)
5. **3D print enclosure** (goblin skull + component shells)
6. **Transfer to final assembly** (from breadboard to skull)

##  Support

If you encounter issues during assembly:

1. **Check troubleshooting sections** in Wiring Guide (page references)
2. **Review Visual Diagrams** for connection verification
3. **Consult Quick Reference** for pin assignments
4. **Visit project repository**: https://github.com/reussered/p32-animatronic-bot
5. **Check documentation**: All source files in `docs/` directory

---

##  File Listing

Expected files in this directory:

```
assembly_guides_pdf/
 README.md                               This file
 P32-Goblin-Head-Wiring-Guide.pdf       Complete assembly (2-3 MB)
 P32-Quick-Reference-Card.pdf            Workbench reference (500 KB)
 P32-Visual-Wiring-Diagrams.pdf          System diagrams (1-2 MB)
```

**Total**: ~4-6 MB for complete kit documentation

---

*Assembly Guides Version: 1.0*  
*Target Hardware: ESP32-S3 DevKitC-1 + GC9A01 Displays + HC-SR04 Sensor*  
*Last Updated: October 15, 2025*  
*Maintained by: P32 Animatronic Bot Project*
