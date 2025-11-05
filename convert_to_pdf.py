#!/usr/bin/env python3
"""
Convert flying dragon markdown documents to HTML (print-to-PDF ready).
"""

from pathlib import Path
import markdown

PROJECT_ROOT = Path("F:/GitHub/p32-animatronic-bot")
DOCS_DIR = PROJECT_ROOT / "docs"

DOCUMENTS = [
    ("FLYING_DRAGON_EXECUTIVE_SUMMARY.md", "Flying Dragon - Executive Summary"),
    ("FLYING_DRAGON_DEEP_DIVE.md", "Flying Dragon - Deep Dive Analysis"),
    ("FLYING_DRAGON_COMPONENT_SPEC.md", "Flying Dragon - Component Specifications"),
    ("FLYING_DRAGON_IMPLEMENTATION_GUIDE.md", "Flying Dragon - Implementation Guide"),
]

def md_to_html_file(md_file, html_output, title):
    """Convert markdown file to standalone HTML that can be printed."""
    md_path = PROJECT_ROOT / md_file
    
    if not md_path.exists():
        print(f"❌ Source not found: {md_path}")
        return False
    
    try:
        # Read markdown
        with open(md_path, 'r', encoding='utf-8') as f:
            md_content = f.read()
        
        # Convert markdown to HTML
        html_body = markdown.markdown(md_content, extensions=['extra', 'codehilite', 'toc', 'tables'])
        
        # Create complete HTML document with print-friendly styling
        html_doc = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{title}</title>
    <style>
        * {{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }}
        
        body {{
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            line-height: 1.7;
            color: #2c3e50;
            background: #ecf0f1;
            padding: 20px;
        }}
        
        .container {{
            max-width: 950px;
            margin: 0 auto;
            background: white;
            padding: 40px;
            box-shadow: 0 0 20px rgba(0,0,0,0.1);
        }}
        
        h1 {{
            font-size: 2.5em;
            color: #2980b9;
            margin: 40px 0 20px 0;
            padding-bottom: 15px;
            border-bottom: 3px solid #3498db;
        }}
        
        h2 {{
            font-size: 2em;
            color: #2980b9;
            margin: 35px 0 15px 0;
            padding-bottom: 10px;
            border-bottom: 2px solid #3498db;
        }}
        
        h3 {{
            font-size: 1.5em;
            color: #34495e;
            margin: 25px 0 10px 0;
        }}
        
        h4, h5, h6 {{
            color: #34495e;
            margin: 15px 0 8px 0;
        }}
        
        p {{
            margin: 12px 0;
            text-align: justify;
        }}
        
        a {{
            color: #3498db;
            text-decoration: none;
        }}
        
        a:hover {{
            text-decoration: underline;
        }}
        
        code {{
            background: #f4f4f4;
            padding: 3px 8px;
            border-radius: 3px;
            font-family: 'Courier New', monospace;
            font-size: 0.95em;
            color: #c7254e;
        }}
        
        pre {{
            background: #2c3e50;
            color: #ecf0f1;
            padding: 15px;
            border-radius: 5px;
            overflow-x: auto;
            margin: 15px 0;
            line-height: 1.4;
            font-size: 0.9em;
        }}
        
        pre code {{
            background: none;
            color: inherit;
            padding: 0;
            border-radius: 0;
        }}
        
        ul, ol {{
            margin: 15px 0 15px 30px;
        }}
        
        li {{
            margin: 8px 0;
        }}
        
        blockquote {{
            border-left: 4px solid #3498db;
            margin: 20px 0;
            padding-left: 20px;
            color: #7f8c8d;
            font-style: italic;
        }}
        
        table {{
            border-collapse: collapse;
            width: 100%;
            margin: 20px 0;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }}
        
        th {{
            background: #3498db;
            color: white;
            padding: 12px;
            text-align: left;
            font-weight: bold;
            border: 1px solid #2980b9;
        }}
        
        td {{
            padding: 10px 12px;
            border: 1px solid #bdc3c7;
        }}
        
        tr:nth-child(even) {{
            background: #f8f9fa;
        }}
        
        tr:hover {{
            background: #ecf0f1;
        }}
        
        .toc {{
            background: #ecf0f1;
            border: 1px solid #bdc3c7;
            border-radius: 5px;
            padding: 15px;
            margin: 20px 0;
        }}
        
        .toc ul {{
            margin: 10px 0 10px 20px;
        }}
        
        .toc a {{
            text-decoration: none;
        }}
        
        strong {{
            color: #2c3e50;
            font-weight: 600;
        }}
        
        em {{
            color: #34495e;
        }}
        
        hr {{
            border: none;
            height: 2px;
            background: #bdc3c7;
            margin: 30px 0;
        }}
        
        .page-break {{
            page-break-after: always;
        }}
        
        @media print {{
            body {{
                background: white;
                padding: 0;
            }}
            .container {{
                max-width: 100%;
                margin: 0;
                padding: 0;
                box-shadow: none;
                page-break-inside: avoid;
            }}
            h1, h2, h3 {{
                page-break-after: avoid;
                page-break-inside: avoid;
            }}
            pre, code, table {{
                page-break-inside: avoid;
            }}
        }}
        
        @page {{
            size: A4;
            margin: 2cm;
        }}
    </style>
</head>
<body>
    <div class="container">
        {html_body}
    </div>
</body>
</html>"""
        
        # Write HTML file
        with open(html_output, 'w', encoding='utf-8') as f:
            f.write(html_doc)
        
        size_kb = html_output.stat().st_size / 1024
        print(f"✅ Generated HTML: {html_output.name} ({size_kb:.0f} KB)")
        return True
        
    except Exception as e:
        print(f"❌ Error converting {md_file}: {e}")
        return False

def main():
    print("=" * 80)
    print("FLYING DRAGON REPORTS - HTML CONVERSION (Print-to-PDF Ready)")
    print("=" * 80)
    print(f"Output directory: {DOCS_DIR}\n")
    
    success_count = 0
    
    for md_file, display_name in DOCUMENTS:
        output_html = DOCS_DIR / md_file.replace(".md", ".html")
        print(f"Processing: {display_name}")
        
        if md_to_html_file(md_file, output_html, display_name):
            success_count += 1
        print()
    
    print("=" * 80)
    print(f"Conversion complete: {success_count}/{len(DOCUMENTS)} files generated")
    print("=" * 80)
    
    # List output files
    if success_count > 0:
        print("\n📄 Generated HTML files in /docs:")
        for html_file in sorted(DOCS_DIR.glob("FLYING_DRAGON*.html")):
            size_mb = html_file.stat().st_size / (1024 * 1024)
            print(f"  • {html_file.name} ({size_mb:.2f} MB)")
        
        print("\n📖 TO CREATE PDF FROM HTML:")
        print("  1. Open HTML file in web browser (Chrome/Firefox/Edge)")
        print("  2. Press Ctrl+P to open Print dialog")
        print("  3. Select 'Save as PDF' as destination")
        print("  4. Click 'Save' to generate PDF file")

if __name__ == "__main__":
    main()
