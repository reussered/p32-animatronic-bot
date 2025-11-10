# Goblin Audio System - PC Integration Guide

This guide shows how to redirect ESP32 audio output to your PC speakers for testing and development.

## Overview

The goblin audio system includes:
- **22+ Goblin sound effects** (growls, roars, laughter, etc.)
- **Goblin speech synthesis** (phrases converted to "goblin language")
- **Emotional responses** with intensity levels
- **Proximity-triggered audio** based on sensor data
- **PC audio streaming** via serial connection

## Quick Setup

### 1. Install Python Dependencies

```powershell
pip install pyserial pygame numpy
```

### 2. Flash ESP32 with Audio Firmware

```powershell
# Regenerate tables with new audio components
python tools/generate_tables.py goblin_head src

# Build and flash
pio run -e goblin_head -t upload
```

### 3. Start PC Audio Capture

```powershell
# Replace COM3 with your ESP32's serial port
python tools/pc_audio_capture.py COM3
```

### 4. Run Audio Demo (Optional)

```powershell
# In another terminal window
python tools/goblin_audio_demo.py COM3
```

## Goblin Sound Library

### Vocalizations
- `goblin_growl_low` - Deep threatening growl (120Hz)
- `goblin_growl_med` - Warning growl (180Hz)
- `goblin_snarl` - Angry snarl (250Hz)
- `goblin_hiss` - Threatening hiss (400Hz)
- `goblin_roar_short` - Intimidating roar (180Hz)
- `goblin_roar_long` - Battle roar (160Hz)
- `goblin_howl` - Mournful howl (220Hz)
- `goblin_screech` - High-pitched screech (800Hz)

### Laughter & Amusement
- `goblin_cackle` - Evil cackling laugh (350Hz)
- `goblin_chuckle` - Amused chuckle (280Hz) 
- `goblin_giggle` - High-pitched giggle (450Hz)

### Communication
- `goblin_grunt_yes` - Affirmative grunt (200Hz)
- `goblin_grunt_no` - Negative grunt (150Hz)
- `goblin_question` - Questioning sound (300Hz)
- `goblin_surprise` - Surprised exclamation (600Hz)

## Goblin Speech Synthesis

The goblin can "speak" English phrases by converting them to goblin language:

| English | Goblin | Meaning |
|---------|--------|---------|
| "hello" | "Grrrak!" | Greeting |
| "warning" | "Krash grok!" | Danger alert |
| "attack" | "GRAAAHHH!" | Battle cry |
| "curious" | "Grok?" | What? |
| "yes" | "Grok grok!" | Agreement |
| "no" | "Grak! Grak!" | Disagreement |
| "hungry" | "Nom nom grak!" | Want food |
| "sleep" | "Zzzgrok..." | Tired |

### Usage Examples

```cpp
// In your goblin components
speaker_speak_goblin_phrase("hello");     // Goblin greets someone
speaker_speak_goblin_phrase("warning");   // Alert about danger
speaker_speak_goblin_phrase("curious");   // Wonder about something
```

## Emotional Responses

Goblins can express emotions with varying intensity (0.0 to 1.0):

```cpp
speaker_play_emotional_response("angry", 0.8f);    // Very angry
speaker_play_emotional_response("happy", 0.5f);    // Moderately happy
speaker_play_emotional_response("scared", 1.0f);   // Terrified
```

## Proximity-Based Audio

The goblin nose automatically triggers audio based on detected distances:

- ** 5cm**: Aggressive response ("angry" emotion + "warning" speech)
- ** 10cm**: Curious response ("surprised" emotion + "curious" speech)
- **Object leaves**: Relief sound ("goblin_grunt_yes")

## Audio Architecture

```
Goblin Component  Speaker Component  I2S Driver  PC via Serial
                                                       
                                            PC Audio Capture Tool
                                                       
                                            Your PC Speakers
```

### Debug Audio Format
- **Sample Rate**: 44.1kHz
- **Channels**: Mono (1 channel)
- **Bit Depth**: 16-bit signed integers
- **Streaming**: Every 16 samples via serial
- **Waveforms**: Complex synthesis for goblin sounds

## Troubleshooting

### No Audio Output
1. Check ESP32 serial connection
2. Verify Python dependencies installed
3. Ensure correct serial port (use Device Manager on Windows)
4. Check that goblin components are included in build

### Audio Quality Issues
1. Increase serial baud rate (115200 recommended)
2. Close other serial monitoring programs
3. Check PC audio system volume/settings

### ESP32 Build Errors
1. Run `python tools/generate_tables.py goblin_head src`
2. Check that speaker.hdr includes new function declarations
3. Verify all component headers are properly included

## Development Notes

### Adding New Sounds
1. Add entry to `sound_library` array in `speaker.src`
2. Update `get_sound_description()` in PC capture tool
3. Regenerate tables and reflash ESP32

### Custom Speech Phrases
1. Add mapping in `speaker_speak_goblin_phrase()` function
2. Update `goblin_translations` in PC capture tool
3. Test with audio demo script

### Hardware Mode
Set `DEBUG_AUDIO_MODE = 0` in `i2s_driver.src` to use real I2S hardware (GPIO 4,5,6) instead of PC streaming.

## Future Enhancements

- **WAV file playback** from SD card storage
- **Real-time audio effects** (reverb, echo)
- **Voice recognition** for interactive responses
- **MIDI integration** for musical goblin performances
- **Network audio streaming** (WebSocket/HTTP)