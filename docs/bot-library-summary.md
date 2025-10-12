# P32 Animatronic Bot Library Summary

## Overview

This repository now contains **21 fully-configured animatronic bot definitions**, each with unique personality traits, skull dimensions, and behavioral characteristics. All bots follow the established skull_3d coordinate system architecture with proper hardware/interface component references.

## Bot Categories

### Fantasy Characters (6 bots)
- **Vampire** - Elegant, high hunger (50), gaunt appearance (2.8" eyes, 0.85 compression)
- **Dragon** - Fierce reptilian (4.0" eyes, 0.55 compression), very high anger (70)
- **Cyclops** - Single central eye (0" spacing), brutal nature, high hunger (70)
- **Elf** - Graceful and wise (2.6" eyes, 0.9 compression), high curiosity (75)
- **Dwarf** - Gruff craftsman (2.9" eyes), balanced grumpy/loyal personality
- **Orc** - Brutal warrior (3.4" eyes, 0.62 compression), very high anger (75)

### Animal Characters (6 bots)
- **Bear** - Friendly (3.5" wide eyes), high happiness (50) and affection (55)
- **Cat** - Curious feline (2.5" eyes), maximum curiosity (80)
- **Wolf** - Predatory (3.2" eyes), high hunger (60) and moderate anger (35)
- **Fox** - Clever (2.7" eyes, 0.68 compression), very high curiosity (85)
- **Owl** - Wise observer (2.0" close eyes), high contentment (75)
- **Raven** - Intelligent corvid (2.3" eyes), very high curiosity (88)

### Horror Characters (4 bots)
- **Zombie** - Undead shambler, maximum hunger (100), never sleeps
- **Ghost** - Ethereal haunter (2.8" eyes), high fear (40) and curiosity (60)
- **Skeleton** - Animated bones, cheerful undead, never sleeps
- **Demon** - Malevolent entity (3.3" eyes), very high anger (85) and hunger (75)

### Mechanical Characters (4 bots)
- **Robot** - Logical AI (2.8" eyes, 1.0 compression), very high curiosity (90)
- **Cyborg** - Human-machine hybrid (2.9" eyes), high curiosity (80), balanced emotions
- **Android** - Synthetic humanoid (2.75" eyes, 0.98 compression), very high curiosity (92)
- **Alien** - Extraterrestrial (4.5" wide eyes, 1.2 expansion), maximum curiosity (95)

### Original Character (1 bot)
- **Goblin Full** - Complete reference implementation with all documentation

## Architecture Highlights

### Coordinate System Diversity
- **Eye Spacing**: 0" (cyclops) to 4.5" (alien)
- **Compression**: 0.55 (dragon) to 1.2 (alien - expansion)
- All use skull_3d coordinate system with nose_center reference

### Mood System (8 Emotional States)
Each bot has unique mood_defaults profiles:
- FEAR: 0-40
- ANGER: 0-85
- IRRITATION: 0-60
- HAPPINESS: 0-60
- CONTENTMENT: 0-75
- HUNGER: 0-100
- CURIOSITY: 5-95
- AFFECTION: 0-55

### Component Architecture
Each bot includes 5 positioned components:
1. **Left Eye** - SPI_DEVICE_1 (except cyclops with center eye)
2. **Right Eye** - SPI_DEVICE_2 (except cyclops)
3. **Mouth Display** - SPI_DEVICE_3 (cyclops uses SPI_DEVICE_2)
4. **Speaker** - I2S_DEVICE_1
5. **Nose Sensor** - GPIO_PAIR_1

**Total Components Created**: 105 positioned component files

### Hardware Configuration Variations
- **Sample Rates**: 44100 Hz (standard) to 48000 Hz (android)
- **Display FPS**: 20-60 fps based on character movement style
- **Sensor Poll Rates**: 5-20 Hz optimized per character
- **Power Management**: low_power, balanced, performance

## Behavioral Diversity

### Sleep Patterns
- **Never Sleep** (0 sec): zombie, ghost, skeleton - undead never rest
- **Short Sleep** (120-300 sec): demon, goblin, orc - aggressive/alert
- **Medium Sleep** (600-1200 sec): most characters
- **Long Sleep** (1800-3600 sec): elf, owl, robot - patient observers

### Interaction Ranges
- **Close Range** (100-150 cm): zombie, dwarf - intimate interactions
- **Medium Range** (180-250 cm): most animals
- **Long Range** (300-400 cm): dragon, demon, alien - dominant presence

### Threat Response
- **Enabled**: Most characters (defensive capabilities)
- **Disabled**: zombie, ghost, bear - non-threatening or already dead

## File Organization

```
config/
├── bots/                    # 21 bot definition files
│   ├── alien.json
│   ├── android.json
│   ├── bear.json
│   ├── cat.json
│   ├── cyborg.json
│   ├── cyclops.json
│   ├── demon.json
│   ├── dragon.json
│   ├── dwarf.json
│   ├── elf.json
│   ├── fox.json
│   ├── ghost.json
│   ├── goblin_full.json
│   ├── orc.json
│   ├── owl.json
│   ├── raven.json
│   ├── robot.json
│   ├── skeleton.json
│   ├── vampire.json
│   ├── wolf.json
│   └── zombie.json
└── components/
    └── positioned/          # 105 positioned component files
        ├── {bot}_eye_left.json
        ├── {bot}_eye_right.json
        ├── {bot}_mouth.json
        ├── {bot}_speaker.json
        └── {bot}_nose.json
```

## Validation Status

✅ All 126 JSON files validated successfully
✅ Consistent schema across all categories
✅ Proper hardware/interface references
✅ Valid coordinate system specifications
✅ Complete component mappings

## Character Personality Profiles

### Most Curious
1. Alien - 95 (extraterrestrial intelligence)
2. Android - 92 (learning AI)
3. Robot - 90 (analytical logic)
4. Raven - 88 (corvid intelligence)

### Most Aggressive
1. Demon - 85 anger (malevolent)
2. Orc - 75 anger (warrior)
3. Dragon - 70 anger (territorial)
4. Cyclops - 60 anger (brutal)

### Most Hungry
1. Zombie - 100 (insatiable)
2. Demon - 75 (consuming)
3. Cyclops - 70 (voracious)
4. Orc - 65 (warrior appetite)

### Most Content
1. Owl - 75 (wise patience)
2. Elf - 70 (peaceful)
3. Bear - 60 (gentle nature)
4. Cat - 55 (satisfied lounging)

### Most Affectionate
1. Bear - 55 (friendly)
2. Elf - 50 (compassionate)
3. Cat - 45 (selective bonding)
4. Dwarf - 45 (loyal)

## Usage Examples

### Selecting a Bot for Your Project

**For Education/STEM**:
- Robot, Android, or Alien (high curiosity, non-threatening)

**For Entertainment/Parties**:
- Bear, Cat, or Owl (friendly, approachable)

**For Halloween/Horror**:
- Zombie, Ghost, Skeleton, or Demon

**For Fantasy Roleplay**:
- Dragon, Vampire, Elf, Dwarf, Orc, Cyclops

**For Wildlife Education**:
- Wolf, Fox, Raven (realistic animal behavior)

## Future Expansion

The architecture supports easy addition of:
- New bot types with unique personalities
- Additional hardware components (tentacles, wings, tails)
- Multi-bot swarm behaviors
- Dynamic mood transitions
- Learning/adaptation systems

## Technical Notes

- All bots use ESP32-S3 GPIO assignments
- SPI bus shared efficiently (3 shared pins + 1 CS per device)
- I2S audio system with spatial positioning
- HC-SR04 ultrasonic sensors for proximity
- GC9A01 circular displays for eyes/mouth
- Modular design allows component reuse across bots
