#!/usr/bin/env python3
"""
Automated PDF generation from HTML files using pyppeteer (headless Chrome).
This script converts the generated HTML files to PDF automatically.
"""

import asyncio
import sys
from pathlib import Path

try:
    from pyppeteer import launch
except ImportError:
    print("❌ pyppeteer not installed. Installing...")
    import subprocess
    subprocess.check_call([sys.executable, "-m", "pip", "install", "pyppeteer"])
    from pyppeteer import launch

PROJECT_ROOT = Path("F:/GitHub/p32-animatronic-bot")
DOCS_DIR = PROJECT_ROOT / "docs"

HTML_FILES = [
    "FLYING_DRAGON_EXECUTIVE_SUMMARY.html",
    "FLYING_DRAGON_DEEP_DIVE.html",
    "FLYING_DRAGON_COMPONENT_SPEC.html",
    "FLYING_DRAGON_IMPLEMENTATION_GUIDE.html",
]

async def html_to_pdf(html_file, pdf_output):
    """Convert HTML to PDF using Pyppeteer (headless Chrome)."""
    html_path = DOCS_DIR / html_file
    
    if not html_path.exists():
        print(f"❌ Source not found: {html_path}")
        return False
    
    try:
        browser = await launch(headless=True)
        page = await browser.newPage()
        
        # Navigate to local HTML file
        file_url = f"file:///{html_path.as_posix()}"
        await page.goto(file_url, {"waitUntil": "networkidle0"})
        
        # Generate PDF with print styling
        await page.pdf({
            "path": str(pdf_output),
            "format": "A4",
            "margin": {
                "top": "2cm",
                "right": "2cm",
                "bottom": "2cm",
                "left": "2cm"
            },
            "printBackground": True
        })
        
        await browser.close()
        
        size_mb = pdf_output.stat().st_size / (1024 * 1024)
        print(f"✅ Generated PDF: {pdf_output.name} ({size_mb:.2f} MB)")
        return True
        
    except Exception as e:
        print(f"❌ Error converting {html_file}: {e}")
        return False

async def main():
    print("=" * 80)
    print("FLYING DRAGON REPORTS - PDF GENERATION (Automated)")
    print("=" * 80)
    print(f"Output directory: {DOCS_DIR}\n")
    
    success_count = 0
    
    for html_file in HTML_FILES:
        pdf_file = html_file.replace(".html", ".pdf")
        output_pdf = DOCS_DIR / pdf_file
        display_name = html_file.replace("_", " ").replace(".html", "")
        
        print(f"Processing: {display_name}")
        
        if await html_to_pdf(html_file, output_pdf):
            success_count += 1
        print()
    
    print("=" * 80)
    print(f"PDF Generation complete: {success_count}/{len(HTML_FILES)} successful")
    print("=" * 80)
    
    # List output files
    if success_count > 0:
        print("\n📄 Generated PDF files in /docs:")
        for pdf_file in sorted(DOCS_DIR.glob("FLYING_DRAGON*.pdf")):
            size_mb = pdf_file.stat().st_size / (1024 * 1024)
            print(f"  • {pdf_file.name} ({size_mb:.2f} MB)")

if __name__ == "__main__":
    asyncio.run(main())
