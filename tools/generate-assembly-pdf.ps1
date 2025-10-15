# P32 Animatronic Bot - Assembly Guide PDF Generator
# Converts markdown wiring guides to professional PDF with diagrams
# Uses Pandoc for markdown→HTML→PDF conversion

param(
    [switch]$InstallPandoc,
    [switch]$GenerateAll,
    [string]$OutputDir = "docs\assembly_guides_pdf"
)

$ErrorActionPreference = "Stop"

# Color output functions
function Write-Success { param([string]$msg) Write-Host "✓ $msg" -ForegroundColor Green }
function Write-Info { param([string]$msg) Write-Host "ℹ $msg" -ForegroundColor Cyan }
function Write-Warning { param([string]$msg) Write-Host "⚠ $msg" -ForegroundColor Yellow }
function Write-Error { param([string]$msg) Write-Host "✗ $msg" -ForegroundColor Red }

Write-Host "`n═══════════════════════════════════════════════════════" -ForegroundColor Magenta
Write-Host "  P32 Assembly Guide PDF Generator" -ForegroundColor Magenta
Write-Host "═══════════════════════════════════════════════════════`n" -ForegroundColor Magenta

# Check for Pandoc installation
function Test-Pandoc {
    try {
        $pandocVersion = & pandoc --version 2>&1 | Select-Object -First 1
        Write-Success "Pandoc found: $pandocVersion"
        return $true
    }
    catch {
        Write-Warning "Pandoc not found"
        return $false
    }
}

# Install Pandoc via Chocolatey or Winget
function Install-Pandoc {
    Write-Info "Installing Pandoc..."
    
    # Try Chocolatey first
    if (Get-Command choco -ErrorAction SilentlyContinue) {
        Write-Info "Using Chocolatey to install Pandoc..."
        choco install pandoc -y
        Write-Success "Pandoc installed via Chocolatey"
        return $true
    }
    
    # Try Winget
    if (Get-Command winget -ErrorAction SilentlyContinue) {
        Write-Info "Using Winget to install Pandoc..."
        winget install --id JohnMacFarlane.Pandoc -e
        Write-Success "Pandoc installed via Winget"
        return $true
    }
    
    # Manual instructions
    Write-Error "No package manager found (Chocolatey or Winget)"
    Write-Info "Please install Pandoc manually from: https://pandoc.org/installing.html"
    Write-Info "Recommended: Download Windows installer from https://github.com/jgm/pandoc/releases"
    return $false
}

# Create CSS for professional PDF styling
function New-PDFStylesheet {
    $cssPath = "$OutputDir\assembly-guide-style.css"
    
    $css = @"
/* P32 Assembly Guide PDF Styling */

@page {
    size: letter;
    margin: 0.75in;
}

body {
    font-family: 'Segoe UI', Arial, sans-serif;
    font-size: 10pt;
    line-height: 1.5;
    color: #333;
    max-width: 100%;
}

h1 {
    color: #2c3e50;
    font-size: 24pt;
    font-weight: bold;
    border-bottom: 3px solid #e74c3c;
    padding-bottom: 10px;
    margin-top: 0;
    page-break-before: always;
}

h1:first-of-type {
    page-break-before: avoid;
}

h2 {
    color: #34495e;
    font-size: 16pt;
    font-weight: bold;
    margin-top: 20pt;
    margin-bottom: 10pt;
    border-bottom: 2px solid #95a5a6;
    padding-bottom: 5px;
}

h3 {
    color: #555;
    font-size: 13pt;
    font-weight: bold;
    margin-top: 15pt;
    margin-bottom: 8pt;
}

pre {
    background-color: #f8f9fa;
    border: 1px solid #dee2e6;
    border-left: 4px solid #007bff;
    padding: 12px;
    font-family: 'Consolas', 'Courier New', monospace;
    font-size: 8pt;
    line-height: 1.4;
    overflow-x: auto;
    page-break-inside: avoid;
}

code {
    background-color: #f1f3f5;
    padding: 2px 6px;
    border-radius: 3px;
    font-family: 'Consolas', 'Courier New', monospace;
    font-size: 9pt;
}

pre code {
    background-color: transparent;
    padding: 0;
}

table {
    border-collapse: collapse;
    width: 100%;
    margin: 15px 0;
    page-break-inside: avoid;
    font-size: 9pt;
}

th {
    background-color: #34495e;
    color: white;
    padding: 10px;
    text-align: left;
    font-weight: bold;
}

td {
    border: 1px solid #dee2e6;
    padding: 8px;
    text-align: left;
}

tr:nth-child(even) {
    background-color: #f8f9fa;
}

ul, ol {
    margin: 10px 0;
    padding-left: 25px;
}

li {
    margin: 5px 0;
}

/* Checkbox styling for checklists */
li:has(input[type="checkbox"]) {
    list-style: none;
    margin-left: -20px;
}

strong {
    color: #2c3e50;
    font-weight: 600;
}

em {
    color: #555;
}

blockquote {
    border-left: 4px solid #e74c3c;
    padding-left: 15px;
    margin: 15px 0;
    color: #555;
    font-style: italic;
}

hr {
    border: none;
    border-top: 2px solid #dee2e6;
    margin: 20px 0;
}

/* Warning/Safety boxes */
.warning, .danger, .caution {
    background-color: #fff3cd;
    border-left: 5px solid #ffc107;
    padding: 15px;
    margin: 15px 0;
    page-break-inside: avoid;
}

/* Footer with page numbers */
@page {
    @bottom-right {
        content: "Page " counter(page) " of " counter(pages);
        font-size: 8pt;
        color: #777;
    }
    @bottom-left {
        content: "P32 Animatronic Bot - Assembly Guide";
        font-size: 8pt;
        color: #777;
    }
}

/* Print optimizations */
@media print {
    body {
        print-color-adjust: exact;
        -webkit-print-color-adjust: exact;
    }
    
    h1, h2, h3 {
        page-break-after: avoid;
    }
    
    pre, table, blockquote {
        page-break-inside: avoid;
    }
}
"@
    
    $css | Out-File -FilePath $cssPath -Encoding UTF8
    Write-Success "Created PDF stylesheet: $cssPath"
    return $cssPath
}

# Generate PDF from markdown
function Convert-MarkdownToPDF {
    param(
        [string]$InputMarkdown,
        [string]$OutputPDF,
        [string]$CSSPath,
        [string]$Title
    )
    
    Write-Info "Converting: $(Split-Path $InputMarkdown -Leaf) → $(Split-Path $OutputPDF -Leaf)"
    
    # Pandoc command with professional settings
    $pandocArgs = @(
        $InputMarkdown,
        "-o", $OutputPDF,
        "--pdf-engine=wkhtmltopdf",
        "--css=$CSSPath",
        "--metadata", "title=$Title",
        "--metadata", "author=P32 Animatronic Bot Project",
        "--metadata", "date=$(Get-Date -Format 'MMMM dd, yyyy')",
        "--toc",
        "--toc-depth=2",
        "--number-sections",
        "--highlight-style=tango",
        "--standalone",
        "--variable", "geometry:margin=0.75in",
        "--variable", "fontsize=10pt",
        "--variable", "papersize=letter"
    )
    
    try {
        & pandoc $pandocArgs 2>&1 | Out-Null
        
        if (Test-Path $OutputPDF) {
            $size = (Get-Item $OutputPDF).Length / 1KB
            Write-Success "Created PDF: $OutputPDF ($([math]::Round($size, 1)) KB)"
            return $true
        }
        else {
            Write-Error "PDF generation failed - file not created"
            return $false
        }
    }
    catch {
        Write-Error "Pandoc error: $_"
        return $false
    }
}

# Alternative: Generate HTML (if wkhtmltopdf not available)
function Convert-MarkdownToHTML {
    param(
        [string]$InputMarkdown,
        [string]$OutputHTML,
        [string]$CSSPath,
        [string]$Title
    )
    
    Write-Info "Converting to HTML: $(Split-Path $InputMarkdown -Leaf) → $(Split-Path $OutputHTML -Leaf)"
    
    $pandocArgs = @(
        $InputMarkdown,
        "-o", $OutputHTML,
        "--css=$CSSPath",
        "--metadata", "title=$Title",
        "--metadata", "author=P32 Animatronic Bot Project",
        "--metadata", "date=$(Get-Date -Format 'MMMM dd, yyyy')",
        "--toc",
        "--toc-depth=2",
        "--number-sections",
        "--highlight-style=tango",
        "--standalone",
        "--self-contained"
    )
    
    try {
        & pandoc $pandocArgs 2>&1 | Out-Null
        
        if (Test-Path $OutputHTML) {
            $size = (Get-Item $OutputHTML).Length / 1KB
            Write-Success "Created HTML: $OutputHTML ($([math]::Round($size, 1)) KB)"
            Write-Info "Open in browser and use 'Print to PDF' for final PDF"
            return $true
        }
        else {
            Write-Error "HTML generation failed"
            return $false
        }
    }
    catch {
        Write-Error "Pandoc error: $_"
        return $false
    }
}

# Main execution
try {
    # Handle installation request
    if ($InstallPandoc) {
        if (Install-Pandoc) {
            Write-Success "Pandoc installation complete"
            Write-Info "Please restart PowerShell and run this script again"
            exit 0
        }
        else {
            Write-Error "Failed to install Pandoc"
            exit 1
        }
    }
    
    # Check for Pandoc
    if (-not (Test-Pandoc)) {
        Write-Warning "Pandoc is required for PDF generation"
        Write-Info "Run with -InstallPandoc flag to auto-install:"
        Write-Host "  .\generate-assembly-pdf.ps1 -InstallPandoc" -ForegroundColor Yellow
        exit 1
    }
    
    # Create output directory
    if (-not (Test-Path $OutputDir)) {
        New-Item -ItemType Directory -Path $OutputDir | Out-Null
        Write-Success "Created output directory: $OutputDir"
    }
    
    # Generate CSS stylesheet
    $cssPath = New-PDFStylesheet
    
    # Define source markdown files and their titles
    $guides = @(
        @{
            Source = "docs\GOBLIN-HEAD-BREADBOARD-WIRING-GUIDE.md"
            Output = "$OutputDir\P32-Goblin-Head-Wiring-Guide.pdf"
            HTML   = "$OutputDir\P32-Goblin-Head-Wiring-Guide.html"
            Title  = "P32 Goblin Head - Breadboard Wiring Guide"
        },
        @{
            Source = "docs\WIRING-QUICK-REFERENCE.md"
            Output = "$OutputDir\P32-Quick-Reference-Card.pdf"
            HTML   = "$OutputDir\P32-Quick-Reference-Card.html"
            Title  = "P32 Goblin Head - Quick Reference Card"
        },
        @{
            Source = "docs\WIRING-VISUAL-DIAGRAM.md"
            Output = "$OutputDir\P32-Visual-Wiring-Diagrams.pdf"
            HTML   = "$OutputDir\P32-Visual-Wiring-Diagrams.html"
            Title  = "P32 Goblin Head - Visual Wiring Diagrams"
        }
    )
    
    # Check for wkhtmltopdf
    $hasWkhtmltopdf = $null -ne (Get-Command wkhtmltopdf -ErrorAction SilentlyContinue)
    
    if (-not $hasWkhtmltopdf) {
        Write-Warning "wkhtmltopdf not found - will generate HTML files instead"
        Write-Info "Install wkhtmltopdf for direct PDF generation: https://wkhtmltopdf.org/downloads.html"
        Write-Info "Or use browser 'Print to PDF' from generated HTML files"
    }
    
    Write-Host "`n" + ("─" * 60) + "`n"
    
    # Process each guide
    $successCount = 0
    foreach ($guide in $guides) {
        if (-not (Test-Path $guide.Source)) {
            Write-Warning "Source file not found: $($guide.Source)"
            continue
        }
        
        if ($hasWkhtmltopdf) {
            # Direct PDF generation
            if (Convert-MarkdownToPDF -InputMarkdown $guide.Source -OutputPDF $guide.Output -CSSPath $cssPath -Title $guide.Title) {
                $successCount++
            }
        }
        else {
            # HTML generation (user can print to PDF)
            if (Convert-MarkdownToHTML -InputMarkdown $guide.Source -OutputHTML $guide.HTML -CSSPath $cssPath -Title $guide.Title) {
                $successCount++
            }
        }
    }
    
    Write-Host "`n" + ("─" * 60) + "`n"
    
    # Summary
    Write-Success "Generation complete: $successCount of $($guides.Count) guides processed"
    Write-Info "Output directory: $OutputDir"
    
    if ($hasWkhtmltopdf) {
        Write-Info "PDF files ready for printing and distribution"
    }
    else {
        Write-Info "HTML files generated - open in browser and use 'Print to PDF'"
    }
    
    # Open output directory
    Write-Host "`nOpening output directory..." -ForegroundColor Cyan
    Start-Process explorer.exe $OutputDir
    
}
catch {
    Write-Error "Script error: $_"
    Write-Host $_.ScriptStackTrace -ForegroundColor Red
    exit 1
}
