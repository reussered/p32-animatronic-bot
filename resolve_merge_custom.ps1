# Resolve merge conflicts with custom strategy
# Local changes override, except when both files have > few lines, then newer creation date wins

param(
    [int]$MinLines = 5  # "few lines" threshold
)

Write-Host "Resolving merge conflicts with custom strategy..." -ForegroundColor Yellow
Write-Host "Strategy: Local overrides, except when both files have >$MinLines lines, then newer creation date wins" -ForegroundColor Cyan

# Get all conflicted files
$conflictedFiles = git diff --name-only --diff-filter=U

if ($conflictedFiles.Count -eq 0) {
    Write-Host "No conflicted files found. Pulling remote changes..." -ForegroundColor Green
    git pull
    exit 0
}

Write-Host "Found $($conflictedFiles.Count) conflicted files" -ForegroundColor Yellow

foreach ($file in $conflictedFiles) {
    Write-Host "Processing: $file" -ForegroundColor White

    # Get local and remote versions
    $localContent = Get-Content $file -Raw -ErrorAction SilentlyContinue
    $remoteContent = git show "HEAD:$file" -ErrorAction SilentlyContinue

    if ($localContent -and $remoteContent) {
        $localLines = ($localContent -split "`n").Count
        $remoteLines = ($remoteContent -split "`n").Count

        Write-Host "  Local: $localLines lines, Remote: $remoteLines lines" -ForegroundColor Gray

        if ($localLines -le $MinLines -and $remoteLines -le $MinLines) {
            # Both have few lines - local wins
            Write-Host "   Using LOCAL (both have $MinLines lines)" -ForegroundColor Green
            git checkout --ours $file
        }
        elseif ($localLines -le $MinLines) {
            # Local has few lines - local wins
            Write-Host "   Using LOCAL (local has $MinLines lines)" -ForegroundColor Green
            git checkout --ours $file
        }
        elseif ($remoteLines -le $MinLines) {
            # Remote has few lines - local still wins per user request
            Write-Host "   Using LOCAL (remote has $MinLines lines, but local overrides)" -ForegroundColor Green
            git checkout --ours $file
        }
        else {
            # Both have many lines - compare creation dates
            $localDate = (Get-Item $file).CreationTime
            $remoteDate = git log -1 --format=%ci "HEAD:$file" | Get-Date

            Write-Host "  Local date: $localDate" -ForegroundColor Gray
            Write-Host "  Remote date: $remoteDate" -ForegroundColor Gray

            if ($localDate -gt $remoteDate) {
                Write-Host "   Using LOCAL (newer creation date)" -ForegroundColor Green
                git checkout --ours $file
            }
            else {
                Write-Host "   Using REMOTE (newer or equal creation date)" -ForegroundColor Yellow
                git checkout --theirs $file
            }
        }
    }
    elseif ($localContent) {
        # Only local exists
        Write-Host "   Using LOCAL (only local exists)" -ForegroundColor Green
        git checkout --ours $file
    }
    else {
        # Only remote exists
        Write-Host "   Using REMOTE (only remote exists)" -ForegroundColor Yellow
        git checkout --theirs $file
    }

    # Mark as resolved
    git add $file
}

Write-Host "All conflicts resolved. Committing..." -ForegroundColor Green
git commit -m "Resolve merge conflicts: local changes override, newer creation dates win for substantial files"

Write-Host "Merge resolution complete!" -ForegroundColor Green