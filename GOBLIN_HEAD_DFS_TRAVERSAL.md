# GOBLIN_HEAD Controller - Depth-First Component Tree Traversal

## DFS Traversal Order (Following components[] arrays recursively)

Starting from: `config/bots/bot_families/goblins/head/goblin_head.json`

### Traversal Path:

```
1. goblin_head (CONTROLLER)
    components[0]: "config/bots/bot_families/goblins/head/goblin_left_eye.json"
      2. goblin_left_eye (POSITIONED_COMPONENT)
          components[0]: "config/components/creature_specific/goblin_eye.json"
            3. goblin_eye (DISPLAY_DRIVER)
                components[0]: "config/components/hardware/gc9a01.json"
                  4. gc9a01 (DISPLAY_DRIVER - Hardware)
                      components[0]: "config/components/drivers/spi_display_bus.json"
                        5. spi_display_bus (BUS_INTERFACE)
                            components[0]: "config/components/drivers/generic_spi_display.json"
                              6. generic_spi_display (DRIVER)
                                  components[0]: "config/components/interfaces/spi_bus.json"
                                    7. spi_bus (BUS_INTERFACE)
                                        components[]: EMPTY - BACKTRACK
                                    [END OF DEPTH PATH 1]
                        [NO MORE COMPONENTS - BACKTRACK]
                     [NO MORE COMPONENTS - BACKTRACK]
                  [NO MORE COMPONENTS - BACKTRACK]
               [NO MORE COMPONENTS - BACKTRACK]
            [NO MORE COMPONENTS - BACKTRACK]
         [NO MORE COMPONENTS - BACKTRACK]
      [NEXT: components[1]]

    components[1]: "config/bots/bot_families/goblins/head/goblin_right_eye.json"
      8. goblin_right_eye (POSITIONED_COMPONENT)
          components[0]: "config/components/creature_specific/goblin_eye.json"
            [DUPLICATE: Already visited as #3 - SKIP or REPROCESS?]
            
            NOTE: In the generated code, duplicate component references cause multiple
            dispatch table entries. So goblin_eye appears TWICE in execution order.
            
            [For DFS, we still traverse but note the duplicate]
         [NO MORE COMPONENTS - BACKTRACK]
      [NEXT: components[2]]

    components[2]: "config/bots/bot_families/goblins/head/goblin_nose.json"
      9. goblin_nose (POSITIONED_COMPONENT)
          components[0]: "config/components/hardware/hc_sr04_sensor.json"
            10. hc_sr04_sensor (HARDWARE - Ultrasonic)
                 software.components[0]: "config/components/drivers/gpio_bus.json"
                   11. gpio_bus (BUS_INTERFACE)
                        components[0]: "config/components/drivers/gpio_pair_driver.json"
                          12. gpio_pair_driver (DRIVER)
                               components[]: EMPTY - BACKTRACK
                          [END OF DEPTH PATH 2A]
                        components[1]: "config/components/drivers/gpio_single_driver.json"
                          13. gpio_single_driver (DRIVER)
                               components[]: EMPTY - BACKTRACK
                          [END OF DEPTH PATH 2B]
                      [NO MORE COMPONENTS - BACKTRACK]
                  [NO MORE COMPONENTS - BACKTRACK]
               [NO MORE COMPONENTS - BACKTRACK]
            [NO MORE COMPONENTS - BACKTRACK]
         [NEXT: components[3]]

    components[3]: "config/bots/bot_families/goblins/head/goblin_mouth.json"
      14. goblin_mouth (POSITIONED_COMPONENT)
           components[0]: "config/components/creature_specific/goblin_mouth.json"
             [SELF-REFERENCE! goblin_mouth contains goblin_mouth]
             
             *** CIRCULAR DEPENDENCY DETECTED ***
             This is a BUG - goblin_mouth should reference hardware (gc9a01)
             not itself.
             
             [To avoid infinite loop, treat as LEAF - BACKTRACK]
          [NO MORE COMPONENTS - BACKTRACK]
         [NEXT: components[4]]

    components[4]: "config/bots/bot_families/goblins/head/goblin_speaker.json"
      15. goblin_speaker (POSITIONED_COMPONENT)
           components[0]: "config/components/hardware/speaker.json"
             16. speaker (HARDWARE - I2S Speaker)
                  components[]: EMPTY or NOT LISTED - BACKTRACK
                 [END OF DEPTH PATH 3]
             [NO MORE COMPONENTS - BACKTRACK]
          [NEXT: components[5]]

    components[5]: "config/bots/bot_families/goblins/head/goblin_left_ear.json"
      17. goblin_left_ear (POSITIONED_COMPONENT)
           components[0]: "config/components/hardware/servo_sg90_micro.json"
            18. servo_sg90_micro (HARDWARE - Servo Motor)
                 software.components[0]: "config/components/drivers/pwm_bus.json"
                   19. pwm_bus (BUS_INTERFACE)
                        components[0]: "config/components/drivers/pwm_driver.json"
                          20. pwm_driver (DRIVER)
                               components[]: EMPTY - BACKTRACK
                              [END OF DEPTH PATH 4]
                          [NO MORE COMPONENTS - BACKTRACK]
                       [NO MORE COMPONENTS - BACKTRACK]
                    [NO MORE COMPONENTS - BACKTRACK]
                   [FINISHED DEPTH - BACKTRACK]
          
           components[1]: "config/components/hardware/hw496_microphone.json"
             21. hw496_microphone (HARDWARE - MEMS Microphone)
                  dependencies[0]: "config/components/drivers/adc_bus.json"
                    22. adc_bus (BUS_INTERFACE) [DEPENDENCY, NOT components[]]
                         components[]: [NOT YET READ]
                        [END OF DEPTH PATH 5A]
                    [NO MORE COMPONENTS IN hw496_microphone - BACKTRACK]
                  dependencies[1]: "config/components/drivers/generic_mic_driver.json"
                    23. generic_mic_driver (DRIVER) [DEPENDENCY]
                         components[]: [NOT YET READ]
                        [END OF DEPTH PATH 5B]
                    [NO MORE COMPONENTS - BACKTRACK]
             [NO MORE COMPONENTS - BACKTRACK]
          [FINISHED COMPONENT ARRAY - BACKTRACK]
      [NEXT: components[6]]

    components[6]: "config/bots/bot_families/goblins/head/goblin_right_ear.json"
      24. goblin_right_ear (POSITIONED_COMPONENT)
           components[0]: "config/components/hardware/servo_sg90_micro.json"
            [DUPLICATE: Already visited as #18 - SKIP or REPROCESS?]
            
            [For DFS, we still traverse - dispatch table will have duplicate entry]
          
           components[1]: "config/components/hardware/hw496_microphone.json"
             [DUPLICATE: Already visited as #21 - SKIP or REPROCESS?]
             [For DFS, we still traverse - dispatch table will have duplicate entry]
          [NO MORE COMPONENTS - BACKTRACK]
      [NEXT: components[7]]

    components[7]: "config/subsystems/goblin_head.json"
      25. goblin_head (SUBSYSTEM COPY)
          [SUBSYSTEM CONFIG - This is a DUPLICATE reference]
          [Purpose: Provides alternative JSON entry point for subsystem]
           components[]: [Similar to #1 - items 2-6]
          [CIRCULAR REFERENCE TO SELF - BACKTRACK]
      [NEXT: components[8]]

    components[8]: "config/subsystems/goblin_torso.json"
      26. goblin_torso (EXTERNAL SUBSYSTEM)
          [EXTERNAL SUBSYSTEM - Different controller]
          [This is a cross-subsystem reference]
           This would have its own DFS tree
          [OUTSIDE SCOPE OF goblin_head SUBSYSTEM - STOP]

[END OF DFS TRAVERSAL]
```

---

## DFS Visit Order (Numbered)

| # | Component Name | File | Type | Parent | Children in components[] |
|---|---|---|---|---|---|
| 1 | goblin_head | config/bots/bot_families/goblins/head/goblin_head.json | CONTROLLER | - | 8 children |
| 2 | goblin_left_eye | config/bots/bot_families/goblins/head/goblin_left_eye.json | POSITIONED | goblin_head[0] | goblin_eye |
| 3 | goblin_eye | config/components/creature_specific/goblin_eye.json | DISPLAY_DRIVER | goblin_left_eye[0] | gc9a01 |
| 4 | gc9a01 | config/components/hardware/gc9a01.json | DISPLAY_DRIVER | goblin_eye[0] | spi_display_bus |
| 5 | spi_display_bus | config/components/drivers/spi_display_bus.json | BUS_INTERFACE | gc9a01[0] | generic_spi_display |
| 6 | generic_spi_display | config/components/drivers/generic_spi_display.json | DRIVER | spi_display_bus[0] | spi_bus |
| 7 | spi_bus | config/components/interfaces/spi_bus.json | BUS_INTERFACE | generic_spi_display[0] | (EMPTY) |
| 8 | goblin_right_eye | config/bots/bot_families/goblins/head/goblin_right_eye.json | POSITIONED | goblin_head[1] | goblin_eye (DUP) |
| 9 | goblin_nose | config/bots/bot_families/goblins/head/goblin_nose.json | POSITIONED | goblin_head[2] | hc_sr04_sensor |
| 10 | hc_sr04_sensor | config/components/hardware/hc_sr04_sensor.json | HARDWARE | goblin_nose[0] | gpio_bus (software) |
| 11 | gpio_bus | config/components/drivers/gpio_bus.json | BUS_INTERFACE | hc_sr04_sensor.software[0] | 2 drivers |
| 12 | gpio_pair_driver | config/components/drivers/gpio_pair_driver.json | DRIVER | gpio_bus[0] | (EMPTY) |
| 13 | gpio_single_driver | config/components/drivers/gpio_single_driver.json | DRIVER | gpio_bus[1] | (EMPTY) |
| 14 | goblin_mouth | config/bots/bot_families/goblins/head/goblin_mouth.json | POSITIONED | goblin_head[3] | goblin_mouth (SELF) |
| 15 | goblin_speaker | config/bots/bot_families/goblins/head/goblin_speaker.json | POSITIONED | goblin_head[4] | speaker |
| 16 | speaker | config/components/hardware/speaker.json | HARDWARE | goblin_speaker[0] | (EMPTY) |
| 17 | goblin_left_ear | config/bots/bot_families/goblins/head/goblin_left_ear.json | POSITIONED | goblin_head[5] | 2 hardware items |
| 18 | servo_sg90_micro | config/components/hardware/servo_sg90_micro.json | HARDWARE | goblin_left_ear[0] | pwm_bus (software) |
| 19 | pwm_bus | config/components/drivers/pwm_bus.json | BUS_INTERFACE | servo_sg90_micro.software[0] | pwm_driver |
| 20 | pwm_driver | config/components/drivers/pwm_driver.json | DRIVER | pwm_bus[0] | (EMPTY) |
| 21 | hw496_microphone | config/components/hardware/hw496_microphone.json | HARDWARE | goblin_left_ear[1] | 2 dependencies |
| 22 | adc_bus | config/components/drivers/adc_bus.json | BUS_INTERFACE | hw496_microphone.dependencies[0] | (NOT TRACED) |
| 23 | generic_mic_driver | config/components/drivers/generic_mic_driver.json | DRIVER | hw496_microphone.dependencies[1] | (NOT TRACED) |
| 24 | goblin_right_ear | config/bots/bot_families/goblins/head/goblin_right_ear.json | POSITIONED | goblin_head[6] | servo (DUP), microphone (DUP) |
| 25 | goblin_head | config/subsystems/goblin_head.json | SUBSYSTEM_COPY | goblin_head[7] | (CIRCULAR) |
| 26 | goblin_torso | config/subsystems/goblin_torso.json | EXTERNAL | goblin_head[8] | (DIFFERENT SUBSYSTEM) |

---

## Key DFS Findings

### Depth Reaches
- **Maximum Depth**: 7 levels (goblin_head  goblin_left_eye  goblin_eye  gc9a01  spi_display_bus  generic_spi_display  spi_bus)
- **Most Shallow**: servo_sg90_micro  pwm_bus  pwm_driver (3 levels to leaf)

### Duplicate References (Reused Components)
| Component | Visited At | Referenced Again At | Times in Dispatch |
|---|---|---|---|
| goblin_eye | #3 | #8 (goblin_right_eye) | 2x in init/act tables |
| servo_sg90_micro | #18 | #24 (goblin_right_ear) | 2x in init/act tables |
| hw496_microphone | #21 | #24 (goblin_right_ear) | 2x in init/act tables |

### Dead Ends (Leaf Nodes)
- spi_bus (components[]: empty)
- gpio_pair_driver (components[]: empty)
- gpio_single_driver (components[]: empty)
- pwm_driver (components[]: empty)
- speaker (components[]: empty)
- **adc_bus** (not yet traced)
- **generic_mic_driver** (not yet traced)

### Errors/Issues
1. **CIRCULAR: goblin_mouth**  goblin_mouth (self-reference)
   - Should be: goblin_mouth  gc9a01
   
2. **CROSS-SUBSYSTEM: goblin_head[8]**  goblin_torso
   - This creates inter-subsystem dependency
   - goblin_torso has different controller (likely ESP32-TORSO or similar)

3. **DUPLICATE PATH**: goblin_head[7]  goblin_head
   - Self-reference creates circular traversal
   - config/subsystems/goblin_head.json mirrors config/bots/bot_families/goblins/head/goblin_head.json

---

## DFS Execution Order for Code Generation

When the Python generation script performs DFS, dispatch tables are populated in this order:

```
INIT TABLE:
  [0] goblin_left_eye_init
  [1] goblin_eye_init
  [2] gc9a01_init
  [3] spi_display_bus_init
  [4] generic_spi_display_init
  [5] spi_bus_init
  [6] goblin_right_eye_init
  [7] goblin_eye_init         DUPLICATE (component visited twice)
  [8] gc9a01_init             DUPLICATE
  [9] spi_display_bus_init    DUPLICATE
  [10] generic_spi_display_init  DUPLICATE
  [11] spi_bus_init           DUPLICATE
  [12] goblin_nose_init
  [13] hc_sr04_sensor_init
  [14] gpio_bus_init
  [15] gpio_pair_driver_init
  [16] gpio_single_driver_init
  [17] goblin_mouth_init      SELF-REFERENCE BUG
  [18] goblin_mouth_init      SELF-REFERENCE BUG
  [19] goblin_speaker_init
  [20] speaker_init
  [21] goblin_left_ear_init
  [22] servo_sg90_micro_init
  [23] pwm_bus_init
  [24] pwm_driver_init
  [25] hw496_microphone_init
  [26] adc_bus_init
  [27] generic_mic_driver_init
  [28] goblin_right_ear_init
  [29] servo_sg90_micro_init  DUPLICATE
  [30] pwm_bus_init           DUPLICATE
  [31] pwm_driver_init        DUPLICATE
  [32] hw496_microphone_init  DUPLICATE
  [33] adc_bus_init           DUPLICATE
  [34] generic_mic_driver_init  DUPLICATE
  
  (Plus circular references from goblin_head[7] and cross-subsystem goblin_torso[8])
```

Same order for ACT table and hitcount table.

---

This is a **true DFS** - following each path to its leaf before backtracking.
