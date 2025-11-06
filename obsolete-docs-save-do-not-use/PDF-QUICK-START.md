# Quick Reference - PDF Generation Commands

## TL;DR - Generate PDFs Right Now

```powershell
# First time only (install Pandoc)
.\tools\generate-assembly-pdf.ps1 -InstallPandoc

# Every time after (generate all PDFs)
.\tools\generate-assembly-pdf.ps1
```

**Output**: `docs\assembly_guides\*.pdf`

---

## One-Liner Cheat Sheet

```powershell
# Install + Generate in one command
.\tools\generate-assembly-pdf.ps1 -InstallPandoc; .\tools\generate-assembly-pdf.ps1

# Generate and open output folder
.\tools\generate-assembly-pdf.ps1; explorer docs\assembly_guides

# Custom output location
.\tools\generate-assembly-pdf.ps1 -OutputDir "release\v1.0\docs"
```

---

## What Gets Generated

| Source Markdown | Output PDF |
|----------------|------------|
| `docs\GOBLIN-HEAD-BREADBOARD-WIRING-GUIDE.md` | `P32-Goblin-Head-Wiring-Guide.pdf` |
| `docs\WIRING-QUICK-REFERENCE.md` | `P32-Quick-Reference-Card.pdf` |
| `docs\WIRING-VISUAL-DIAGRAM.md` | `P32-Visual-Wiring-Diagrams.pdf` |

---

## Common Issues (Quick Fix)

### "Pandoc not found"
```powershell
.\tools\generate-assembly-pdf.ps1 -InstallPandoc
# Then restart PowerShell
```

### "wkhtmltopdf not found" (Optional)
```powershell
choco install wkhtmltopdf -y
# OR use HTML output and browser PrintPDF
```

### Need to update PDFs after editing markdown?
```powershell
# Just regenerate - it's fast!
.\tools\generate-assembly-pdf.ps1
```

---

## Manual Installation (If Auto-Install Fails)

**Pandoc**:
1. Download: https://github.com/jgm/pandoc/releases/latest
2. Run installer: `pandoc-X.X.X-windows-x86_64.msi`
3. Restart PowerShell

**wkhtmltopdf** (optional):
1. Download: https://wkhtmltopdf.org/downloads.html
2. Run installer: `wkhtmltox-X.X.X-msvc2015-win64.exe`
3. Restart PowerShell

---

## Verify Installation

```powershell
pandoc --version        # Should show version 3.x+
wkhtmltopdf --version   # Should show version 0.12.x
```

---

## Distribution Workflow

```powershell
# 1. Update markdown source files
code docs\GOBLIN-HEAD-BREADBOARD-WIRING-GUIDE.md

# 2. Generate fresh PDFs
.\tools\generate-assembly-pdf.ps1

# 3. Review output
explorer docs\assembly_guides

# 4. Test print one copy (check formatting)

# 5. Package with hardware kits
# Include all 3 PDFs with each kit shipment
```

---

## Full Documentation

For complete details, troubleshooting, and customization:

 **Complete Workflow**: `docs\PDF-GENERATION-WORKFLOW.md` (450 lines)
- Installation methods (4 different approaches)
- Customization guide (CSS, page layout, styling)
- Troubleshooting (6 common issues + fixes)
- Advanced automation (batch processing, watch mode)
- Best practices for markdown authoring

 **Power Distribution**: `docs\POWER-DISTRIBUTION-CLARIFICATION.md` (250 lines)
- ESP32-S3 3.3V output usage (no external regulator needed)
- Current budget calculations (265mA used / 450mA available)
- Common misconceptions clarified
- Troubleshooting power issues

 **Session Summary**: `docs\SESSION-PDF-GENERATION-SUMMARY.md` (350 lines)
- Complete record of this afternoon's work
- Files created and their purposes
- Integration with existing documentation

---

## Quick Troubleshooting Decision Tree

```
Can't generate PDFs?
    
     "Pandoc not found"
      > Run with -InstallPandoc flag
    
     "wkhtmltopdf not found"
      > Option A: Install wkhtmltopdf (better rendering)
      > Option B: Use HTML output, browser PrintPDF
    
     "PDF has bad formatting"
      > Check markdown source (proper heading hierarchy)
    
     "Script won't run"
       > Check PowerShell execution policy:
           Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned
```

---

## Remember

 **Update markdown  Regenerate PDFs** (it's automated!)  
 **Test print before mass distribution** (check formatting)  
 **Include all 3 PDFs with hardware kits** (complete assembly guides)  
 **This workflow is documented** (never trailblaze again!)

---

*Keep this file handy for quick reference!*  
*Last updated: October 15, 2025*
