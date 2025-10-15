# Directory Organization Update - PDF Distribution

## Changes Made

Created a dedicated **distribution directory** for finished PDF files.

### New Directory Structure

```
docs/
├── assembly_guides_pdf/              ← NEW: Finished PDFs for distribution
│   ├── README.md                     ← Distribution guide (280 lines)
│   ├── P32-Goblin-Head-Wiring-Guide.pdf
│   ├── P32-Quick-Reference-Card.pdf
│   └── P32-Visual-Wiring-Diagrams.pdf
│
├── GOBLIN-HEAD-BREADBOARD-WIRING-GUIDE.md  ← Source markdown
├── WIRING-QUICK-REFERENCE.md               ← Source markdown
├── WIRING-VISUAL-DIAGRAM.md                ← Source markdown
├── PDF-GENERATION-WORKFLOW.md              ← How to generate
├── PDF-QUICK-START.md                      ← Quick commands
└── PDF-DOCUMENTATION-INDEX.md              ← Master index
```

## Rationale

### Before (Unclear Organization)

```
docs/
├── assembly_guides/    ← Temporary build artifacts?
│   ├── *.pdf          ← Should these be in Git?
│   ├── *.html         ← Temp files?
│   └── *.css          ← Build artifacts?
```

**Problems**:
- Unclear if PDFs should be committed to Git
- Mixed temp files with distribution files
- No clear "release ready" location

### After (Clear Separation)

```
docs/
├── assembly_guides_pdf/    ← ✅ FINISHED: Commit to Git
│   ├── README.md           ← Distribution guide
│   └── *.pdf               ← Release-ready PDFs
│
├── assembly_guides/        ← ❌ TEMP: Ignored by Git
│   ├── *.html              ← Build artifacts
│   └── *.css               ← Build artifacts
```

**Benefits**:
- Clear distinction between temp and finished files
- Easy to identify release-ready PDFs
- README explains distribution workflow
- Git tracks only finished PDFs

## What Changed

### 1. Created New Directory

**`docs/assembly_guides_pdf/`** - Distribution directory for finished PDFs

Contains:
- `README.md` (280 lines) - Complete distribution guide
- Space for 3 finished PDFs (committed to Git)

### 2. Updated PDF Generator Script

**`tools/generate-assembly-pdf.ps1`** - Changed default output

```powershell
# OLD: Output to assembly_guides (temp)
[string]$OutputDir = "docs\assembly_guides"

# NEW: Output to assembly_guides_pdf (distribution)
[string]$OutputDir = "docs\assembly_guides_pdf"
```

Now PDFs are generated directly to the distribution directory.

### 3. Updated .gitignore

Added rules to handle temp vs. finished files:

```gitignore
# PDF Generation - Ignore temporary build artifacts
docs/assembly_guides/*.pdf
docs/assembly_guides/*.html
docs/assembly_guides/*.css

# But keep finished distribution PDFs
!docs/assembly_guides_pdf/*.pdf
```

**Effect**:
- Temp files in `assembly_guides/` are ignored
- Finished PDFs in `assembly_guides_pdf/` are tracked

### 4. Created Distribution README

**`docs/assembly_guides_pdf/README.md`** (280 lines)

Comprehensive guide covering:
- What's included (3 PDFs)
- Source files reference
- How to regenerate
- Distribution workflow (physical & digital)
- Printing recommendations
- Quality checklist
- Version history
- Tips for kit builders

## Usage

### Generate PDFs (Now Outputs to Distribution Directory)

```powershell
# First time
.\tools\generate-assembly-pdf.ps1 -InstallPandoc

# Every time
.\tools\generate-assembly-pdf.ps1

# Output: docs\assembly_guides_pdf\*.pdf
```

### Commit Finished PDFs

```powershell
# PDFs are now in distribution directory
git add docs\assembly_guides_pdf\*.pdf
git commit -m "Update assembly guide PDFs to v1.0"
git tag v1.0
git push origin main --tags
```

### Distribute with Hardware Kits

```powershell
# PDFs are release-ready in assembly_guides_pdf/
explorer docs\assembly_guides_pdf

# Print all 3 PDFs
# Include with each hardware kit
```

## Git Strategy

### What Gets Committed

✅ **Source markdown** (`docs/*.md`) - Always commit  
✅ **Finished PDFs** (`docs/assembly_guides_pdf/*.pdf`) - Commit for releases  
✅ **Distribution README** (`docs/assembly_guides_pdf/README.md`) - Always commit  
✅ **Generator script** (`tools/generate-assembly-pdf.ps1`) - Always commit  

### What Gets Ignored

❌ **Temp HTML** (`docs/assembly_guides/*.html`) - Build artifact  
❌ **Temp CSS** (`docs/assembly_guides/*.css`) - Build artifact  
❌ **Temp PDFs** (`docs/assembly_guides/*.pdf`) - Intermediate build  

## Benefits

### Clear Organization

```
📁 docs/
  ├── 📁 assembly_guides_pdf/     ← "Ship this!"
  │   ├── 📄 README.md            ← Distribution guide
  │   └── 📄 *.pdf                ← Release-ready
  │
  └── 📁 assembly_guides/         ← "Ignore this!"
      ├── 📄 *.html               ← Temp build artifacts
      └── 📄 *.css                ← Temp build artifacts
```

### Easy Releases

```powershell
# All release files in one place
cd docs\assembly_guides_pdf
ls *.pdf  # Ready to distribute!
```

### Version Control Clarity

```bash
# Only finished PDFs tracked
git status
  modified:   docs/assembly_guides_pdf/P32-Goblin-Head-Wiring-Guide.pdf
  # Temp files ignored automatically
```

## Migration Notes

### If You Have Old PDFs

If PDFs exist in `docs/assembly_guides/`:

```powershell
# Move to distribution directory
Move-Item docs\assembly_guides\*.pdf docs\assembly_guides_pdf\

# Verify
explorer docs\assembly_guides_pdf
```

### If You Have Temp Files

If HTML/CSS exists in `docs/assembly_guides/`:

```powershell
# Leave them - they'll be ignored by Git
# Or delete if desired:
Remove-Item docs\assembly_guides\*.html, docs\assembly_guides\*.css
```

## Documentation Updates

Updated these files to reflect new structure:

1. **PDF-DOCUMENTATION-INDEX.md** - Updated output paths
2. **.gitignore** - Added temp file rules
3. **tools/generate-assembly-pdf.ps1** - Changed default output dir

## Summary

✅ **Created**: `docs/assembly_guides_pdf/` - Distribution directory  
✅ **Created**: `docs/assembly_guides_pdf/README.md` - Distribution guide (280 lines)  
✅ **Updated**: `tools/generate-assembly-pdf.ps1` - Output to distribution dir  
✅ **Updated**: `.gitignore` - Ignore temp files, track finished PDFs  
✅ **Updated**: `PDF-DOCUMENTATION-INDEX.md` - Document new structure  

**Result**: Clear separation between temporary build artifacts and release-ready distribution files.

---

*Directory Organization Update*  
*Date: October 15, 2025*  
*Purpose: Clarify temp vs. distribution files*
