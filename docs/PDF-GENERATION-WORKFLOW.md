# PDF Assembly Guide Generation - Complete Documentation

## Overview

This document explains how to generate professional PDF assembly guides from the P32 project's markdown documentation. This workflow was established to create print-ready documentation for distribution with physical kits.

## Why PDF Generation?

- **Professional documentation** for kit assembly
- **Printable reference cards** for workbench use
- **Offline access** during physical assembly
- **Consistent formatting** across all guides
- **Easy distribution** with hardware kits

## Tools Required

### Primary: Pandoc
- **Purpose**: Universal document converter (Markdown  HTML  PDF)
- **Website**: https://pandoc.org/
- **Installation**: See installation section below

### Secondary: wkhtmltopdf (Optional but Recommended)
- **Purpose**: HTML to PDF converter with better rendering
- **Website**: https://wkhtmltopdf.org/
- **Installation**: See installation section below

## Installation Methods

### Method 1: Automated (Recommended)

Run the generator script with the `-InstallPandoc` flag:

```powershell
.\tools\generate-assembly-pdf.ps1 -InstallPandoc
```

This will attempt to install Pandoc using:
1. **Chocolatey** (if available)
2. **Winget** (if Chocolatey not available)
3. Manual instructions if neither available

### Method 2: Chocolatey (Windows Package Manager)

```powershell
# Install Chocolatey first (if not already installed)
Set-ExecutionPolicy Bypass -Scope Process -Force
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# Install Pandoc and wkhtmltopdf
choco install pandoc -y
choco install wkhtmltopdf -y
```

### Method 3: Winget (Built into Windows 11)

```powershell
# Install Pandoc
winget install --id JohnMacFarlane.Pandoc -e

# Install wkhtmltopdf
winget install --id wkhtmltopdf.wkhtmltopdf -e
```

### Method 4: Manual Download

**Pandoc:**
1. Visit https://github.com/jgm/pandoc/releases
2. Download `pandoc-X.X.X-windows-x86_64.msi`
3. Run installer
4. Restart PowerShell

**wkhtmltopdf:**
1. Visit https://wkhtmltopdf.org/downloads.html
2. Download Windows installer (e.g., `wkhtmltox-0.12.6-1.msvc2015-win64.exe`)
3. Run installer
4. Add to PATH: `C:\Program Files\wkhtmltopdf\bin`
5. Restart PowerShell

## Verify Installation

```powershell
# Check Pandoc
pandoc --version

# Check wkhtmltopdf
wkhtmltopdf --version
```

Expected output:
```
pandoc 3.x.x
wkhtmltopdf 0.12.6 (with patched qt)
```

## Usage

### Basic: Generate All PDFs

```powershell
.\tools\generate-assembly-pdf.ps1
```

This generates PDFs for:
1. **GOBLIN-HEAD-BREADBOARD-WIRING-GUIDE.md**  `P32-Goblin-Head-Wiring-Guide.pdf`
2. **WIRING-QUICK-REFERENCE.md**  `P32-Quick-Reference-Card.pdf`
3. **WIRING-VISUAL-DIAGRAM.md**  `P32-Visual-Wiring-Diagrams.pdf`

Output location: `docs\assembly_guides\`

### Without wkhtmltopdf: Generate HTML

If wkhtmltopdf is not installed, the script will generate HTML files instead:

```powershell
.\tools\generate-assembly-pdf.ps1
# Generates: P32-*.html files
```

You can then use your browser's "Print to PDF" feature:
1. Open HTML file in browser
2. Press `Ctrl+P`
3. Select "Save as PDF"
4. Adjust margins/layout as needed
5. Save

### Custom Output Directory

```powershell
.\tools\generate-assembly-pdf.ps1 -OutputDir "custom\path"
```

## PDF Features

### Professional Styling
- **Letter-size pages** (8.5"  11")
- **0.75" margins** for comfortable reading
- **Table of Contents** with page numbers
- **Section numbering** for easy reference
- **Syntax highlighting** for code blocks
- **Professional fonts** (Segoe UI, Consolas)
- **Color-coded headers** (red accent, dark blue text)
- **Zebra-striped tables** for readability

### Print-Optimized Layout
- **Page breaks** before major sections
- **No broken tables/diagrams** across pages
- **Header/footer** with document title and page numbers
- **High-contrast text** for printer-friendly output

### Content Preservation
- **ASCII art diagrams** render correctly
- **Wire color tables** maintain formatting
- **Checklists** with checkboxes
- **Safety warnings** highlighted

## File Structure

```
p32-animatronic-bot/
 docs/
    GOBLIN-HEAD-BREADBOARD-WIRING-GUIDE.md   Source
    WIRING-QUICK-REFERENCE.md                Source
    WIRING-VISUAL-DIAGRAM.md                 Source
    assembly_guides/                         Output
        assembly-guide-style.css             Generated CSS
        P32-Goblin-Head-Wiring-Guide.pdf    PDF Output
        P32-Quick-Reference-Card.pdf         PDF Output
        P32-Visual-Wiring-Diagrams.pdf       PDF Output
 tools/
     generate-assembly-pdf.ps1                Generator Script
```

## Customization

### Modify PDF Styling

Edit the CSS section in `generate-assembly-pdf.ps1`:

```powershell
# Line ~90: New-PDFStylesheet function
# Modify CSS variables:
$css = @"
h1 {
    color: #2c3e50;        # Change heading color
    font-size: 24pt;       # Change heading size
    border-bottom: 3px solid #e74c3c;  # Change accent color
}
"@
```

### Add More Guides

Edit the `$guides` array in the script:

```powershell
$guides = @(
    @{
        Source = "docs\YOUR-NEW-GUIDE.md"
        Output = "$OutputDir\Your-New-PDF.pdf"
        HTML   = "$OutputDir\Your-New-HTML.html"
        Title  = "Your Guide Title"
    }
)
```

### Change Page Size

Modify Pandoc arguments:

```powershell
# Line ~210: Convert-MarkdownToPDF function
"--variable", "papersize=a4"    # Instead of "letter"
"--variable", "geometry:margin=1in"  # Adjust margins
```

## Troubleshooting

### Issue: "Pandoc not found"

**Solution**: Install Pandoc using one of the methods above, then restart PowerShell.

```powershell
.\tools\generate-assembly-pdf.ps1 -InstallPandoc
```

### Issue: "wkhtmltopdf not found"

**Solution 1**: Install wkhtmltopdf (recommended)
```powershell
choco install wkhtmltopdf -y
# OR
winget install --id wkhtmltopdf.wkhtmltopdf -e
```

**Solution 2**: Use HTML output and browser print
```powershell
.\tools\generate-assembly-pdf.ps1  # Generates HTML
# Open .html files in browser
# Use Ctrl+P  Save as PDF
```

### Issue: "PDF has formatting issues"

**Solution**: Try different PDF engine:

```powershell
# Edit script line ~210, change:
"--pdf-engine=wkhtmltopdf"
# To:
"--pdf-engine=weasyprint"  # Requires Python + WeasyPrint
# Or:
"--pdf-engine=prince"      # Commercial option
```

### Issue: "ASCII diagrams broken in PDF"

**Solution**: Ensure monospace font is used:

```css
/* In CSS section of script */
pre {
    font-family: 'Consolas', 'Courier New', monospace;
    font-size: 8pt;
    white-space: pre;
}
```

### Issue: "Tables split across pages"

**Solution**: Add page-break-inside CSS:

```css
table {
    page-break-inside: avoid;
}
```

## Distribution Workflow

### For Kit Assembly Documentation

1. **Generate PDFs** from latest markdown:
   ```powershell
   .\tools\generate-assembly-pdf.ps1
   ```

2. **Review output** in `docs\assembly_guides\`:
   - Check all diagrams render correctly
   - Verify wire colors are visible
   - Ensure tables fit on pages
   - Validate page breaks

3. **Test print** one copy:
   - Print to physical printer
   - Verify readability at actual size
   - Check margins and binding space

4. **Package with hardware**:
   - Include all 3 PDFs with each kit
   - Consider laminating quick reference card
   - Add project URL for latest updates

### For Online Documentation

1. **Generate HTML** versions:
   ```powershell
   .\tools\generate-assembly-pdf.ps1
   ```

2. **Host HTML files** on project website/GitHub Pages

3. **Link from README.md**:
   ```markdown
   ## Assembly Guides
   - [Wiring Guide (PDF)](docs/assembly_guides/P32-Goblin-Head-Wiring-Guide.pdf)
   - [Quick Reference (PDF)](docs/assembly_guides/P32-Quick-Reference-Card.pdf)
   - [Visual Diagrams (PDF)](docs/assembly_guides/P32-Visual-Wiring-Diagrams.pdf)
   ```

## Advanced: Batch Generation

### Generate PDFs for All Documentation

Create a script to process all markdown files:

```powershell
# Generate PDFs for entire docs folder
Get-ChildItem docs\*.md | ForEach-Object {
    $output = "docs\assembly_guides\$($_.BaseName).pdf"
    pandoc $_.FullName -o $output --pdf-engine=wkhtmltopdf
}
```

### Watch Mode (Auto-Regenerate on Changes)

```powershell
# Watch for markdown changes and auto-regenerate PDFs
$watcher = New-Object System.IO.FileSystemWatcher
$watcher.Path = "docs"
$watcher.Filter = "*.md"
$watcher.IncludeSubdirectories = $false
$watcher.EnableRaisingEvents = $true

$action = {
    Write-Host "Detected change in $($Event.SourceEventArgs.Name), regenerating PDF..."
    .\tools\generate-assembly-pdf.ps1
}

Register-ObjectEvent $watcher "Changed" -Action $action
```

## Best Practices

### Markdown Authoring for PDF

1. **Use semantic headings** (H1, H2, H3 hierarchy)
2. **Keep ASCII diagrams** to 80 columns max
3. **Use tables** for structured data
4. **Add blank lines** around code blocks
5. **Use bold** for important warnings
6. **Test rendering** frequently

### Version Control

1. **Commit source markdown** to Git
2. **Gitignore generated PDFs** (large binary files)
3. **Regenerate PDFs** for each release
4. **Tag releases** with PDF generation date

```gitignore
# .gitignore
docs/assembly_guides/*.pdf
docs/assembly_guides/*.html
```

### Release Checklist

- [ ] Update markdown sources with latest changes
- [ ] Run `.\tools\generate-assembly-pdf.ps1`
- [ ] Review all PDFs for correctness
- [ ] Test print one physical copy
- [ ] Verify file sizes reasonable (<5MB each)
- [ ] Add PDFs to release assets
- [ ] Update documentation links

## Performance Notes

- **Small guides** (<100 KB markdown)  <5 seconds
- **Large guides** (>500 KB markdown)  15-30 seconds
- **With images**  Longer (image processing)
- **First run**  Slower (Pandoc initialization)

## Future Enhancements

### Potential Improvements

1. **Image generation** for OpenSCAD renders
2. **Auto-screenshot** of actual hardware
3. **QR codes** linking to video tutorials
4. **Multi-language** support (i18n)
5. **Interactive PDF** with fillable checklists
6. **BOM integration** with pricing
7. **Revision tracking** in PDF metadata

### Automation Ideas

1. **GitHub Actions** workflow to auto-generate PDFs on commit
2. **Pre-commit hook** to regenerate PDFs before push
3. **Release pipeline** to attach PDFs to GitHub releases
4. **CI validation** to check PDF generation succeeds

## Summary

This PDF generation system provides:
-  **Professional documentation** for kit builders
-  **Automated workflow** for consistent output
-  **Easy customization** via CSS and Pandoc options
-  **Multiple output formats** (PDF, HTML)
-  **Print-optimized** layout and styling
-  **Repeatable process** for future updates

**Next time you need PDFs**: Just run `.\tools\generate-assembly-pdf.ps1` 

---

*Documentation Version: 1.0*  
*Last Updated: October 15, 2025*  
*Tested on: Windows 11, PowerShell 5.1*
