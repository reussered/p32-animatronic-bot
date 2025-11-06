# P32 Component Architecture - Visual Diagram

## Pure Component-Driven Execution Model

```

                         ESP32 app_main()                            
                                                                     
  void app_main(void) {                                             
      // Initialize all components                                   
      for (i = 0; i < INIT_TABLE_SIZE; i++)                         
          initTable[i]();                                           
                                                                     
      // ONLY COMPONENT LOOP - NO APPLICATION LOGIC                 
      while (true) {                                                
          for (i = 0; i < ACT_TABLE_SIZE; i++) {                   
              if (loopCount % actTable[i].hitCount == 0)           
                  actTable[i].act_func(loopCount);                 
          }                                                         
          loopCount++;                                              
          vTaskDelay(100ms);                                        
      }                                                             
  }                                                                 

                                    
                                    

              Generated Component Tables (JSON  Code)               

                                                                     
  init_func_t initTable[INIT_TABLE_SIZE] = {                        
      system_core_init,         //  Level 1: System              
      network_monitor_init,     //  Level 1: System              
      goblin_personality_init,  //  Level 2: Family              
      goblin_mood_init,         //  Level 2: Family              
      goblin_eye_left_init,     //  Level 3: Bot-specific        
      goblin_eye_right_init,    //  Level 3: Bot-specific        
      goblin_nose_init          //  Level 3: Bot-specific        
  };                                                                 
                                                                     
  act_table_entry_t actTable[ACT_TABLE_SIZE] = {                    
      {system_core_act,        100, "system_core"},                 
      {network_monitor_act,     50, "network_monitor"},             
      {goblin_personality_act,  25, "goblin_personality"},          
      {goblin_mood_act,         15, "goblin_mood"},                 
      {goblin_eye_left_act,      5, "left_eye"},                    
      {goblin_eye_right_act,     5, "right_eye"},                   
      {goblin_nose_act,         15, "nose_sensor"}                  
  };                                                                 

                                    
                    
                                                  
```

## Three-Level Component Hierarchy

```

                     LEVEL 1: SYSTEM COMPONENTS                        
                       (Always Attached)                               

                                                                       
                    
   system_core       network        watchdog                  
   hitCount: 100   hitCount: 50    hitCount: 500              
                    
                                                                       
  Purpose: Core platform functionality (WiFi, Serial, Power Monitor)  
  Timing:  Slow execution (5-50 second intervals)                     
  Scope:   ALL bots regardless of type                                
                                                                       

                                    
                                    

                     LEVEL 2: FAMILY COMPONENTS                        
                 (Shared Across Bot Family)                            

                                                                       
                    
     goblin_         goblin_         goblin_                  
   personality         mood          behavior                 
   hitCount: 25    hitCount: 15    hitCount: 10               
                    
                                                                       
  Purpose: Shared behavior/personality traits for bot family          
  Timing:  Moderate execution (1-5 second intervals)                  
  Scope:   All GOBLIN bots (goblin_full, goblin_mini, etc.)          
                                                                       
  Defined in: config/bots/bot_families/fantasy/goblin_family.json     
                                                                       

                                    
                                    

                  LEVEL 3: BOT-SPECIFIC COMPONENTS                     
                    (Positioned Hardware)                              

                                                                       
                    
    left_eye        right_eye        nose                     
   GC9A01@SPI1     GC9A01@SPI2     HC-SR04@GPIO               
   [-26,17,-9]     [26,17,-9]       [0,0,6]                   
   hitCount: 5     hitCount: 5     hitCount: 15               
                    
                                                                       
  Purpose: Individual hardware components with spatial positions       
  Timing:  Fast execution (300ms-3 second intervals)                  
  Scope:   Specific to THIS bot instance (goblin_full)                
                                                                       
  Defined in: config/bots/goblin_full.json positioned_components[]    
                                                                       

```

## Component Execution Timeline

```
Loop Period: 100ms (10 Hz)

Time (seconds)  0.0    0.5    1.0    1.5    2.0    2.5    3.0    3.5    4.0
                                                                
                                                                

left_eye                                                        
(hitCount: 5)                                                   
                500ms

right_eye                                                       
(hitCount: 5)                                                   
                500ms

nose_sensor                                                         
(hitCount: 15)                                                      
                1.5s

mood                                                                
(hitCount: 15)                                                      
                1.5s

personality                                           
(hitCount: 25)                                        
                2.5s

network                                                
(hitCount: 50)                                         
                5.0s

system_core                                                            
(hitCount: 100)                                                        
                10s
```

**Execution Logic**: Component executes when `loopCount % hitCount == 0`

## Component Attachment Flow

```

                  JSON Configuration Files                           

                                
            
                                                  
    
  System Level        Family Level     Bot-Specific Level 
  (Hardcoded)         goblin_family     goblin_full.json  
     .json         
  system_core          positioned_        
  network_monitor    default_         components:        
  watchdog           components:       left_eye.json    
  telemetry           personality     right_eye.json   
    mood            nose.json        
                         behavior        mouth.json       
                         
                                
                                
            
              Python Component Table Generator    
              tools/generate_component_tables.py  
            
                                
                    
                                           
          
         p32_component_         component_tables.c  
         registry.h                                 
          
          initTable[N]         init_func_t         
          actTable[N]            initTable[N] = {  
          ACT_TABLE_SIZE           ...             
          Function             };                  
           declarations                             
           act_table_entry_t   
                                    actTable[N] = {   
                                      ...             
                                  };                  
                                 
                                
                                
                    
                       Compilation +      
                       Linking            
                     (PlatformIO/ESP-IDF) 
                    
                                
                                
                    
                      Firmware Binary     
                      (.bin file)         
                    
                                
                                
                    
                       Flash to ESP32     
                       Execute app_main() 
                    
```

## Adding New Functionality - REQUIRED STEPS

```

              "I want to add a new feature..."                       

                                
                                
                
                  NO: Add code to app_main() 
                  NO: Create standalone task 
                  NO: Add to existing loop   
                                               
                  YES: Create a COMPONENT    
                
                                
                                

                  STEP 1: Create Component JSON                      

                                                                     
  config/components/positioned/my_feature.json                       
  {                                                                  
    "component_name": "my_feature",                                 
    "description": "What this does",                                
    "timing": { "hitCount": 10 }                                    
  }                                                                  

                                
                                

                  STEP 2: Write Component Code                       

                                                                     
  src/components/my_feature.c                                        
                                                                     
  #ifdef ENABLE_{FAMILY}_COMPONENTS                                  
                                                                     
  esp_err_t my_feature_init(void) {                                 
      // Initialize component                                        
      return ESP_OK;                                                 
  }                                                                  
                                                                     
  void my_feature_act(uint32_t loopCount) {                         
      // Component logic executes here                              
  }                                                                  
                                                                     
  #endif                                                             

                                
                                

              STEP 3: Attach at Appropriate Level                    

                                                                     
  System Level:      Add to hardcoded system components (rare)      
  Family Level:      Add to goblin_family.json default_components   
  Bot-Specific:      Add to goblin_full.json positioned_components  

                                
                                

              STEP 4: Regenerate Component Tables                    

                                                                     
  python tools/generate_component_tables.py config/bots/my_bot.json 

                                
                                

                    STEP 5: Build and Flash                          

                                                                     
  pio run -t upload                                                  

```

## Component Categories and Timing

```

                       hitCount Guidelines                           

 Component     hitCount      Execution      Examples             
 Type          Range         Period                              

 System Core   100-500       10-50 sec      Watchdog, Health     
 Network       50-100        5-10 sec       WiFi, Telemetry      
 Personality   20-50         2-5 sec        Behavior Decisions   
 Mood          10-20         1-2 sec        Emotional States     
 Sensors       10-30         1-3 sec        Distance, Touch      
 Displays      3-10          300ms-1 sec    Animation Frames     
 Audio         5-15          500ms-1.5 sec  Sound Playback       


Loop Period: 100ms (10 Hz)
Execution Trigger: loopCount % hitCount == 0

Example hitCount values:
 hitCount:   1  Every 100ms ( Use sparingly - high CPU load)
 hitCount:   5  Every 500ms (Good for displays)
 hitCount:  10  Every 1 second (Good for sensors)
 hitCount:  50  Every 5 seconds (Good for network)
 hitCount: 100  Every 10 seconds (Good for housekeeping)
```

## Summary

**The P32 animatronic system uses a pure component-driven architecture:**

1.  **Components are the ONLY execution mechanism**
2.  **Three-level hierarchy**: System  Family  Bot-specific
3.  **JSON-driven configuration** defines all components
4.  **Timing via hitCount** controls execution frequency
5.  **init() + act() pattern** for every component
6.  **app_main() contains NO application logic**

**To add ANY functionality  Create a component  Attach at appropriate level**

 **[Complete Specification](THREE-LEVEL-COMPONENT-ATTACHMENT-SPEC.md)**
