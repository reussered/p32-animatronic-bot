# Global Shared Memory API - Simple Read/Write Interface

## Design Philosophy

**Simple API for complex distributed system:**
- Components allocate named memory blocks during `init()`
- Components read/write using string names
- SharedMemory class automatically synchronizes across all subsystems
- Zero pointers, zero serialization all transfers using binary

## CRITICAL Architecture Understanding

### read() Does NOT Use Mesh Network!

**`read( string name)` just returns the address of the class variable in global memory.  No memcpy or imnvocation of sync unless it doesn't exist, in which case its created automatically.
**`write( string name)` makes sure the shared mnemory in every core has the same value, 

```cpp
Mood *mood = read( "g_Mood" );  if the Mood doesn't exist, Mood() is created and automatically syncronized across all subsystems

changes to mood, i.e. mood->ANGER = 0; mood->CONTENTMENT = 100;	write directly into the memory buffer allocated for     

**`write( "g_Mood" )` triggers mesh broadcast** - synchronizes to all other subsystems:


_esp_err_t write(const char *name );
	// pushes the current Mood into all other subsytems withiout affecting anything else.

```
```

**Key Insight**: Components can call `read()` **as often as needed** (even every loop!) with ZERO network overhead. Only `write()` triggers mesh traffic.

### Use Cases

**Fast reads inside personality functions**:
```cpp
void goblin_personality_act(void) 
{
    Environment *envir = SharedMemory.read("g_Envir" );
    
    Mood *currentMood = SharedMemory.read("g_MOOD" );
	
	// Mood is changed depending on the temp/humnidity
	if( envir->temperature_c > 80 && envir->humidity > 75 && currentMood->CONTENTMENT > 20)
	{
		currentMood->CONTENTMENT -= ( envir->temperature_c - 80);
		SharedMemory.write( "G_mOOD" );
	}
}
```

**Eye detects mood changes for animation recalculation** (yesterday's implementation):
```cpp

// saves Mood value in the global store.
Mood lastMood = memcpy( SharedMemory.read("g_Mood" ), sizeof(Mood));
 
void goblin_eye_left_act(void) {
    Mood *currentMood = SharedMemory.read("g_Mood" );
    
    if (*currentMood != lastMood) {
        // Mood changed! Recalculate animation frames
        generate_blink_animation();
    }
}


**Example**:
```cpp
void goblin_eye_left_act(void) 
{
    Environment *envir = SharedMemory.read("g_Envir" );

    Mood *currentMood = SharedMemory.read( "g_Mood" );'

    int8_t happiness = currentMood->components[MOOD_HAPPINESS];
    int8_t anger = currentMood->components[MOOD_ANGER];
        
    render_eye_expression(happiness, anger);

	if (envir->distance_cm < 30) 
	{
		dilate_pupils();  // React to close object
    }
}
```

### Write Access (in act() functions)

```c
esp_err_t write( string name);
```


**Example**:
```cpp
void nose_sensor_act(void)
{
    Environment *envir = SharedMemory.read("g_Envir" );
	
    // Local variable for sensor reading
    uint8_t distance_cm = measure_distance();   // uses sensor
    
	if( distance_cm != envir->distance_cm )
	{
		envir->distance_cm = distance_cm;
		SharedMemory.write( "g_Envir" );
	}
}
```

### Complete Pattern: Read-Modify-Write (with change detection)

``

