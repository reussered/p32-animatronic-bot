param(
    [string]$Character = "goblin",
    [string]$Message = "",
    [switch]$AllCharacters,
    [switch]$TestPhrase
)

$CharacterVoices = @{
    goblin = @{ Rate = 2; Volume = 100; TestPhrase = "Greetings human! I am the mighty goblin! Heh heh heh!" }
    bear = @{ Rate = -2; Volume = 100; TestPhrase = "Hello there, small one. Welcome to my forest home." }
    cat = @{ Rate = 3; Volume = 100; TestPhrase = "Mrow? Oh, it's just you. Have you brought me treats?" }
    cyclops = @{ Rate = -3; Volume = 100; TestPhrase = "Who dares enter my domain? State your business, mortal!" }
}

function New-CharacterVoice {
    param([string]$Character, [string]$Text, [string]$OutputFile)
    
    $config = $CharacterVoices[$Character]
    if (-not $config) {
        Write-Host "Unknown character: $Character" -ForegroundColor Red
        return $false
    }
    
    try {
        $voice = New-Object -ComObject SAPI.SpVoice
        $filestream = New-Object -ComObject SAPI.SpFileStream
        
        $voice.Volume = $config.Volume
        $voice.Rate = $config.Rate
        
        New-Item -Path (Split-Path $OutputFile) -ItemType Directory -Force | Out-Null
        $filestream.Open($OutputFile, 3)
        $voice.AudioOutputStream = $filestream
        $voice.Speak($Text, 1)
        Start-Sleep -Seconds 4
        $filestream.Close()
        
        $fileSize = (Get-Item $OutputFile).Length
        Write-Host "Success $Character`: $OutputFile ($([math]::Round($fileSize/1024,1))KB)" -ForegroundColor Green
        return $true
    }
    catch {
        Write-Host "Failed to generate $Character voice: $($_.Exception.Message)" -ForegroundColor Red
        return $false
    }
}

Write-Host "P32 Animatronic Sound Generator" -ForegroundColor Cyan

if ($TestPhrase) {
    foreach ($char in $CharacterVoices.Keys) {
        $outputFile = "assets\sounds\$char\test_phrase.wav"
        $phrase = $CharacterVoices[$char].TestPhrase
        New-CharacterVoice -Character $char -Text $phrase -OutputFile $outputFile
    }
    exit
}

if ($AllCharacters -and $Message) {
    foreach ($char in $CharacterVoices.Keys) {
        $outputFile = "assets\sounds\$char\custom_message.wav"
        New-CharacterVoice -Character $char -Text $Message -OutputFile $outputFile
    }
    exit
}

if ($Message) {
    $outputFile = "assets\sounds\$Character\custom_message.wav"
    New-CharacterVoice -Character $Character -Text $Message -OutputFile $outputFile
    exit
}

Write-Host "Usage: .\tools\quick-sound.ps1 -TestPhrase"
Write-Host "Characters: $($CharacterVoices.Keys -join ', ')" -ForegroundColor Cyan