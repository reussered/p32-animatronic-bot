$projectRoot = "f:\GitHub\p32-animatronic-bot"# Comprehensive Component Reference Validator

$allJsonFiles = Get-ChildItem -Path "$projectRoot\config" -Recurse -Filter "*.json" -File# Checks ALL JSON files in the entire project for missing component references



Write-Host ""$projectRoot = "f:\GitHub\p32-animatronic-bot"

Write-Host "========================================" -ForegroundColor Cyan$allJsonFiles = Get-ChildItem -Path "$projectRoot\config" -Recurse -Filter "*.json" -File

Write-Host "COMPREHENSIVE COMPONENT VALIDATION" -ForegroundColor Cyan

Write-Host "========================================" -ForegroundColor CyanWrite-Host "`n========================================" -ForegroundColor Cyan

Write-Host ""Write-Host "COMPREHENSIVE COMPONENT VALIDATION" -ForegroundColor Cyan

Write-Host "========================================`n" -ForegroundColor Cyan

$totalReferences = 0

$missingReferences = @()$totalReferences = 0

$checkedFiles = 0$missingReferences = @()

$checkedFiles = 0

foreach ($jsonFile in $allJsonFiles) {

    $checkedFiles++foreach ($jsonFile in $allJsonFiles) {

    Write-Progress -Activity "Validating component references" -Status "Checking $($jsonFile.Name)" -PercentComplete (($checkedFiles / $allJsonFiles.Count) * 100)    $checkedFiles++

        Write-Progress -Activity "Validating component references" -Status "Checking $($jsonFile.Name)" -PercentComplete (($checkedFiles / $allJsonFiles.Count) * 100)

    try {    

        $content = Get-Content $jsonFile.FullName -Raw | ConvertFrom-Json    try {

                $content = Get-Content $jsonFile.FullName -Raw | ConvertFrom-Json

        if ($content.components) {        

            foreach ($componentRef in $content.components) {        # Check 'components' array

                $totalReferences++        if ($content.components) {

                $refPath = $componentRef            foreach ($componentRef in $content.components) {

                if ($refPath -notmatch '^[a-zA-Z]:') {                $totalReferences++

                    $refPath = Join-Path $projectRoot $componentRef                

                }                # Resolve relative path

                                $refPath = $componentRef

                if (-not (Test-Path $refPath)) {                if ($refPath -notmatch '^[a-zA-Z]:') {

                    $missingReferences += [PSCustomObject]@{                    $refPath = Join-Path $projectRoot $componentRef

                        SourceFile = $jsonFile.FullName.Replace($projectRoot, "").TrimStart('\')                }

                        MissingComponent = $componentRef                

                        Type = "components[]"                if (-not (Test-Path $refPath)) {

                    }                    $missingReferences += [PSCustomObject]@{

                }                        SourceFile = $jsonFile.FullName.Replace($projectRoot, "").TrimStart('\')

            }                        MissingComponent = $componentRef

        }                        Type = "components[]"

                            }

        if ($content.component) {                }

            $totalReferences++            }

            $refPath = $content.component        }

            if ($refPath -notmatch '^[a-zA-Z]:') {        

                $refPath = Join-Path $projectRoot $content.component        # Check 'component' field (singular)

            }        if ($content.component) {

                        $totalReferences++

            if (-not (Test-Path $refPath)) {            $refPath = $content.component

                $missingReferences += [PSCustomObject]@{            if ($refPath -notmatch '^[a-zA-Z]:') {

                    SourceFile = $jsonFile.FullName.Replace($projectRoot, "").TrimStart('\')                $refPath = Join-Path $projectRoot $content.component

                    MissingComponent = $content.component            }

                    Type = "component"            

                }            if (-not (Test-Path $refPath)) {

            }                $missingReferences += [PSCustomObject]@{

        }                    SourceFile = $jsonFile.FullName.Replace($projectRoot, "").TrimStart('\')

                            MissingComponent = $content.component

        if ($content.body_sections) {                    Type = "component"

            foreach ($section in $content.body_sections) {                }

                if ($section.component) {            }

                    $totalReferences++        }

                    $refPath = $section.component        

                    if ($refPath -notmatch '^[a-zA-Z]:') {        # Check nested body_sections (for bot configs)

                        $refPath = Join-Path $projectRoot $section.component        if ($content.body_sections) {

                    }            foreach ($section in $content.body_sections) {

                                    if ($section.component) {

                    if (-not (Test-Path $refPath)) {                    $totalReferences++

                        $missingReferences += [PSCustomObject]@{                    $refPath = $section.component

                            SourceFile = $jsonFile.FullName.Replace($projectRoot, "").TrimStart('\')                    if ($refPath -notmatch '^[a-zA-Z]:') {

                            MissingComponent = $section.component                        $refPath = Join-Path $projectRoot $section.component

                            Type = "body_sections"                    }

                        }                    

                    }                    if (-not (Test-Path $refPath)) {

                }                        $missingReferences += [PSCustomObject]@{

                                            SourceFile = $jsonFile.FullName.Replace($projectRoot, "").TrimStart('\')

                if ($section.components) {                            MissingComponent = $section.component

                    foreach ($componentRef in $section.components) {                            Type = "body_sections"

                        $totalReferences++                        }

                        $refPath = $componentRef                    }

                        if ($refPath -notmatch '^[a-zA-Z]:') {                }

                            $refPath = Join-Path $projectRoot $componentRef                

                        }                if ($section.components) {

                                            foreach ($componentRef in $section.components) {

                        if (-not (Test-Path $refPath)) {                        $totalReferences++

                            $missingReferences += [PSCustomObject]@{                        $refPath = $componentRef

                                SourceFile = $jsonFile.FullName.Replace($projectRoot, "").TrimStart('\')                        if ($refPath -notmatch '^[a-zA-Z]:') {

                                MissingComponent = $componentRef                            $refPath = Join-Path $projectRoot $componentRef

                                Type = "body_sections.components[]"                        }

                            }                        

                        }                        if (-not (Test-Path $refPath)) {

                    }                            $missingReferences += [PSCustomObject]@{

                }                                SourceFile = $jsonFile.FullName.Replace($projectRoot, "").TrimStart('\')

            }                                MissingComponent = $componentRef

        }                                Type = "body_sections.components[]"

    }                            }

    catch {                        }

        Write-Host "Warning: Could not parse $($jsonFile.Name)" -ForegroundColor Yellow                    }

    }                }

}            }

        }

Write-Progress -Activity "Validating component references" -Completed    }

    catch {

Write-Host ""        Write-Host "Warning: Could not parse $($jsonFile.Name)" -ForegroundColor Yellow

Write-Host "========================================" -ForegroundColor Cyan    }

Write-Host "VALIDATION RESULTS" -ForegroundColor Cyan}

Write-Host "========================================" -ForegroundColor Cyan

Write-Host ""Write-Progress -Activity "Validating component references" -Completed



Write-Host "Files Checked:        $checkedFiles" -ForegroundColor GreenWrite-Host "`n========================================" -ForegroundColor Cyan

Write-Host "Total References:     $totalReferences" -ForegroundColor GreenWrite-Host "VALIDATION RESULTS" -ForegroundColor Cyan

if ($missingReferences.Count -eq 0) {Write-Host "========================================`n" -ForegroundColor Cyan

    Write-Host "Missing References:   0" -ForegroundColor Green

} else {Write-Host "Files Checked:        $checkedFiles" -ForegroundColor Green

    Write-Host "Missing References:   $($missingReferences.Count)" -ForegroundColor RedWrite-Host "Total References:     $totalReferences" -ForegroundColor Green

}Write-Host "Missing References:   $($missingReferences.Count)" -ForegroundColor $(if ($missingReferences.Count -eq 0) { "Green" } else { "Red" })



if ($missingReferences.Count -gt 0) {if ($missingReferences.Count -gt 0) {

    Write-Host ""    Write-Host "`n========================================" -ForegroundColor Red

    Write-Host "========================================" -ForegroundColor Red    Write-Host "MISSING COMPONENTS" -ForegroundColor Red

    Write-Host "MISSING COMPONENTS" -ForegroundColor Red    Write-Host "========================================`n" -ForegroundColor Red

    Write-Host "========================================" -ForegroundColor Red    

    Write-Host ""    $missingReferences | Sort-Object MissingComponent | Format-Table -AutoSize

        

    $missingReferences | Sort-Object MissingComponent | Format-Table -AutoSize    # Export to file

        $missingReferences | Export-Csv -Path "$projectRoot\missing_components_full.csv" -NoTypeInformation

    $missingReferences | Export-Csv -Path "$projectRoot\missing_components_full.csv" -NoTypeInformation    Write-Host "`nExported details to: missing_components_full.csv" -ForegroundColor Yellow

    Write-Host "Exported details to: missing_components_full.csv" -ForegroundColor Yellow}

} else {else {

    Write-Host ""    Write-Host "`n✓ All component references are valid!" -ForegroundColor Green

    Write-Host "All component references are valid!" -ForegroundColor Green}

}

Write-Host "`n========================================" -ForegroundColor Cyan

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
