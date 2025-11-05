# Flying Dragon Reports - Print to PDF Instructions

**Status**: HTML files generated and ready to convert to PDF  
**Generated**: November 5, 2025  
**Location**: `/docs/` directory

---

## 📄 Available Reports

The following reports have been generated as HTML files and are ready to be converted to PDF:

### 1. **FLYING_DRAGON_EXECUTIVE_SUMMARY.html**
- Overview of the flying dragon project
- Key findings and design decisions
- Timeline and budget summary
- **Size**: ~18 KB
- **Purpose**: Quick reference for stakeholders

### 2. **FLYING_DRAGON_DEEP_DIVE.html**
- Complete technical feasibility analysis
- Mass and thrust calculations
- Aerodynamics and hybrid propulsion design
- Control system architecture
- 5-phase implementation roadmap
- **Size**: ~51 KB
- **Purpose**: Detailed technical reference

### 3. **FLYING_DRAGON_COMPONENT_SPEC.html**
- JSON component definitions for P32 integration
- Hardware templates and specifications
- Motor mixing equations
- Calibration procedures
- **Size**: ~32 KB
- **Purpose**: Developer reference guide

### 4. **FLYING_DRAGON_IMPLEMENTATION_GUIDE.html**
- Practical build checklist
- BOM with costs
- Assembly steps and procedures
- Firmware skeleton code
- Testing procedures
- **Size**: ~29 KB
- **Purpose**: Build team implementation guide

---

## 🖨️ How to Convert HTML to PDF

### Option 1: Chrome/Edge (Recommended)

**Steps**:
1. Open the HTML file in Google Chrome or Microsoft Edge
2. Press `Ctrl+P` (Windows) or `Cmd+P` (Mac)
3. In the Print dialog, select **"Save as PDF"** from the destination dropdown
4. Adjust margins if needed (Recommended: "Minimal")
5. Click **"Save"** and choose output location
6. Name the file: `FLYING_DRAGON_[REPORT_NAME].pdf`

**Settings for best results**:
- Destination: Save as PDF
- Paper size: A4
- Margins: Minimal
- Headers/footers: OFF
- Background graphics: ON (for blue styling)

### Option 2: Firefox

**Steps**:
1. Open the HTML file in Mozilla Firefox
2. Press `Ctrl+P` (Windows) or `Cmd+P` (Mac)
3. Select **"Print to File"** or **"Save as PDF"**
4. Follow same configuration as above

### Option 3: Safari (Mac)

**Steps**:
1. Open the HTML file in Safari
2. Press `Cmd+P`
3. Click **"PDF"** dropdown → **"Save as PDF"**
4. Follow same configuration as above

### Option 4: Automated (Windows Only - PowerShell)

If you have PowerShell and wkhtmltopdf installed:

```powershell
# Install wkhtmltopdf first from: https://wkhtmltopdf.org/

$htmlFiles = @(
    "FLYING_DRAGON_EXECUTIVE_SUMMARY.html",
    "FLYING_DRAGON_DEEP_DIVE.html",
    "FLYING_DRAGON_COMPONENT_SPEC.html",
    "FLYING_DRAGON_IMPLEMENTATION_GUIDE.html"
)

foreach ($file in $htmlFiles) {
    $htmlPath = "F:\GitHub\p32-animatronic-bot\docs\$file"
    $pdfPath = $htmlPath -replace "\.html$", ".pdf"
    
    Write-Host "Converting: $file → $(Split-Path $pdfPath -Leaf)"
    
    & "C:\Program Files\wkhtmltopdf\bin\wkhtmltopdf.exe" `
        --margin-top 2cm `
        --margin-bottom 2cm `
        --margin-left 2cm `
        --margin-right 2cm `
        --enable-local-file-access `
        "$htmlPath" "$pdfPath"
    
    if ($LASTEXITCODE -eq 0) {
        $sizeMB = (Get-Item $pdfPath).Length / 1MB
        Write-Host "✓ Generated: $(Split-Path $pdfPath -Leaf) ($([math]::Round($sizeMB, 2)) MB)`n"
    }
}
```

---

## 📊 File Sizes & Estimated Print Pages

| Report | HTML Size | Est. Pages | PDF Size* |
|--------|-----------|-----------|-----------|
| Executive Summary | 18 KB | 10–15 | 200–300 KB |
| Deep Dive | 51 KB | 30–40 | 600–900 KB |
| Component Spec | 32 KB | 20–25 | 400–600 KB |
| Implementation | 29 KB | 18–22 | 350–550 KB |
| **TOTAL** | **130 KB** | **78–102** | **1.5–2.5 MB** |

*Estimated sizes; actual PDF sizes vary based on browser and compression

---

## 💾 Batch Conversion Using Python

If you prefer automated batch conversion, use one of these Python solutions:

### Using built-in `pyppeteer` (if Chrome/Chromium installed):

```python
import asyncio
from pyppeteer import launch
from pathlib import Path

async def convert_html_to_pdf(html_path, pdf_path):
    browser = await launch(headless=True)
    page = await browser.newPage()
    await page.goto(f"file:///{html_path.as_posix()}")
    await page.pdf({"path": str(pdf_path), "format": "A4"})
    await browser.close()

asyncio.run(convert_html_to_pdf(
    Path("docs/FLYING_DRAGON_EXECUTIVE_SUMMARY.html"),
    Path("docs/FLYING_DRAGON_EXECUTIVE_SUMMARY.pdf")
))
```

### Using `pandoc` (if installed):

```bash
pandoc FLYING_DRAGON_EXECUTIVE_SUMMARY.html -o FLYING_DRAGON_EXECUTIVE_SUMMARY.pdf
```

### Using `wkhtmltopdf`:

```bash
wkhtmltopdf --margin-top 2cm --margin-bottom 2cm FLYING_DRAGON_EXECUTIVE_SUMMARY.html FLYING_DRAGON_EXECUTIVE_SUMMARY.pdf
```

---

## ✅ Quality Checklist

After generating PDFs, verify:

- [ ] All text is readable and not cut off
- [ ] Headers are properly formatted (blue styling visible)
- [ ] Code blocks display with syntax highlighting
- [ ] Tables render correctly
- [ ] Images/diagrams are included (if any)
- [ ] Page numbers visible (if desired)
- [ ] Hyperlinks are preserved
- [ ] No blank pages at end
- [ ] File size reasonable (not >5 MB)

---

## 📝 Recommended Distribution

**For team sharing**:
1. Generate all 4 PDFs
2. Create a `FLYING_DRAGON_REPORTS_BUNDLE.zip` containing all 4 PDFs
3. Share via email, GitHub release, or shared drive

**For printing**:
1. Merge all 4 PDFs into single document (using PDF merge tool)
2. Print double-sided on A4 paper (expected: 80–100 pages)
3. Bind with 2-hole punch and fastener

---

## 🔧 Troubleshooting

### "Fonts look wrong"
- Ensure "Background graphics" is enabled in Print settings
- Use Chrome or Firefox (they have better HTML rendering)

### "Colors are faded"
- Enable "Print backgrounds" or "Graphics" in Print dialog
- Check browser's Print settings for "More settings" → "Background graphics"

### "Images not showing"
- Enable "Print background images" in Print settings
- Refresh HTML page before printing

### "Page breaks awkward"
- This is normal for converted HTML
- Consider using "Fit to page" or "Shrink to fit" option
- Adjust zoom percentage (85–100% recommended)

### "File too large"
- Reduce PDF quality in Print dialog
- Use "Shrink to fit" to avoid extra blank pages
- Enable PDF compression if available

---

## 📎 Document Metadata

When saving PDFs, suggested naming:

```
FLYING_DRAGON_EXECUTIVE_SUMMARY_v1.0_2025-11-05.pdf
FLYING_DRAGON_DEEP_DIVE_v1.0_2025-11-05.pdf
FLYING_DRAGON_COMPONENT_SPEC_v1.0_2025-11-05.pdf
FLYING_DRAGON_IMPLEMENTATION_GUIDE_v1.0_2025-11-05.pdf
```

---

## 📞 Questions?

For questions about report content, see the original markdown files:
- `FLYING_DRAGON_EXECUTIVE_SUMMARY.md`
- `FLYING_DRAGON_DEEP_DIVE.md`
- `FLYING_DRAGON_COMPONENT_SPEC.md`
- `FLYING_DRAGON_IMPLEMENTATION_GUIDE.md`

All located in project root directory.

---

**Generated**: November 5, 2025  
**Last Updated**: November 5, 2025  
**Status**: Ready for PDF conversion
