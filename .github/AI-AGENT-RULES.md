# AI AGENT IRONCLAD RULES - NEVER BREAK THESE


## RULE 1 - these rules are consistant.  
	- if a contradiction between one of these rules and another one of the rules in this list is detected, 
		these rules must be modified using feedback from the user to make them so before anything else is allowed to proceed.
	- These rules are not complete, in the sense additional consistant rules will be requirted as development proceeds
	- anytime the user requests an action or task not allowed by one or more of these rules, he must be warned exactly what rule conflicts with his directions.  at thast point the human will be allowed to do 1 of these options:
		a. change the agent-ai rule (this ruleset) either temporarily or permanently to allow the task to proceed
		b. change the requested task
		c. abort the task completely and do something else
		
## RULE 2 ONLY MODIFY FILES RELATED TO CURRENT TASK

- EVERY TIME A NEW TASK IS STARTED OR A SPECIFC AREA of the project IS BEING WORKED ON, THE EXACT FILES THAT WILL BE CHANGED WILL DE DECIDED UPON FIRST BY A DISCUSSION BETWEEN HUMAN
	AND AGENT.  THIS WILL INCLUDE ANY FILES THAT WILL BE MOVED FROM ONE FOLDER TO ANOTHER, OR the rules governing aNY FILES OR FOLDERS TO BE CREATED.  DOING THIS MAY OR MAY NOT REQUIRE MULTIPLE INTERACTIOPNS BETWEEN THE HUMAN AND AGENT.  BUTB IS NOT COM,PLETED UNTIL THE HUMAN ASGREES IT IS DONE.
	Once the task parameters have been defined, and the files to be worked on have been agreed to, the list of files is saved and explicit permission is given to work on all of those files and no other.  when given permission to create folders or files, explicvit permission is givemn to work on those fgolders and the newly created files join the FILE LIST FOR WHICH PERMISSION HAS BEEN SPEECIFIED.
	aT THE END OF THE TASK, THE AGENT WILL REPORT THE CHGANGES MADE AND THE FILES CHANGED. HUMAN may request to examine one or more file contents, then the contents are shown.  once the human agrees that the task was performed, a reoport is generated and appended to a file called task_reports.md. all changes will be commited to git upon which the task is declared completed and all file change permissions are removed. 
	Between the point in time between the git commit and the beginning of the next task, no filenasmes, file contents, or file locations can be changed.  the only exception to this rule is the creation of python scripts
	in the /tools folder. even in that case, scripts that are frequently used must be migrated to the /tools/save folder.  Scripts in this folder can be modified if errors are detected, but they cannot be deleted unless explicit permission is p7rovided by the human.
	all temporary files should be created directly in the root folder of the project f:/github/p32-animatronic-bot.  files directly in thios folder are specified as temprary files and they can be created, modifed or deleted as decided by the agent.  
	The agent should look at each instance however and decide before deleting it whiether the file should be moved to /docs or /tools/save to become part of the permanent repository.

	- no changes are allowed outside the boundaries of specific tasks.  this applies to files, rules and documentation 
		unless the user gives explicit permission for those changes.  if a fgile or folder is given permission, it becomes partb of the file list of the task for which explicit permission is provided.
	- the rules for creating both new files and new folders withing that task MUST BE DECIDED BEFORE STARTING WORK.	 it is always ok to create or change files within the root folder	
	- once the human and agent agree on the files to be changed, moved, or deleted, explicit permission is given to make the changes speciffied to them, FOR THE DURATION OF THE TASK.
	- ONCE THE TASK HAS BEEN COMPLETED, AND THE HUMAN AGREES IT HAS BEEN COMPLETED, THE CURRENT STATE OF THE SYSTEM IS COMMITED TO GIT, AFTER WHICH TIME THE LIST OF FILES AND FOLDER/FILE CREATION RULES ARE COMPLETELY CLEARED. AND PERMISSION TO CHANGE ANYTHING IS WITHDRAWN.
	- PERMISSIONS GRANTED IN THE PERFORMANCE OF ONE TASK MAY BE LEFT TO BE USED IN THE NEXT TASK AS THE HUMAN SPECIFIES.  BUT AT ANYB RATE, THIS SET OF PERMISSIONS BECOME THE STARTING POINT FOR THE DICUSSION STARTING THE NEXT TASK.  
	- THE HUMAN MAY WITYHDRAW PERMISSION TO CHANGE ANYTHING, OR EVERYTHING AT ANY TIME.
	
## RULE 3: RESPECT VERSION CONTROL

**Files in Git are DONE unless human says otherwise.**

- this rule should not be interpreted to conflict with rule #1

- Don't "fix" committed code
- Don't "improve" working code
- Don't "update" old code
- Don't "refactor" existing code

## RULE 4: FRAMEWORK IS ESP-IDF, NOT ARDUINO

**NEVER reference:**
- Arduino.h
- Adafruit libraries  
- Arduino IDE
- Arduino functions (delay(), pinMode(), etc.)

**ALWAYS use:**
- ESP-IDF native APIs
- FreeRTOS

## RULE 5 System uses components for all functionality.

- the ComponentName is called name here to simplify sermantics.
- all component names are unique
- each component has 2 functions, an init function named {name}_init( void ), and {name}_act(void ).
- the function names are always derived from the name, like name.cpp and name.hpp
- the files these two functions are defined in are named like name.hpp and name.cpp.  they are located in the /src/components and /include/components folders.
- for example, if the component name is GC9A01, the functions are gc9a01_init()  and gc9a01_act() located in src/components/gc9a01.cpp and /iclude/components/gc9a01.hpp
- while the names are derived by parsing the json files defining the component, once they have been created and their contents completed, 
	these files follow the same rules about changing as the rest of the system (i.e.rule #1) once they have been checked into git.


	
## RULE 6: ALWAYS USE BRACES (ALLMAN STYLE)

**ALL `for`, `while`, `if`, `else` statements MUST use braces, even for single-line bodies.**

**Braces MUST be on their own line at the same indentation level (Allman style).**

**CORRECT (Allman Style):**

```c
for (int i = 0; i < SIZE; i++)
{
    doSomething();
}

if (condition)
{
    doAction();
}

while (running)
{
    update();
}

if (error)
{
    handleError();
}
else
{
    continueWork();
}

void myFunction(void)
{
    // Function body
}
```

**WRONG:**

```c
for (int i = 0; i < SIZE; i++) doSomething();      // ❌ NO - missing braces
if (condition) doAction();                          // ❌ NO - missing braces
while (running) update();                           // ❌ NO - missing braces

for (int i = 0; i < SIZE; i++) {                   // ❌ NO - K&R style (brace on same line)
    doSomething();
}

if (condition) {                                    // ❌ NO - K&R style (brace on same line)
    doAction();
}
```

**Rationale:**

- Prevents bugs when adding debug statements
- Makes code structure visually clear
- Braces at same indent level show scope hierarchy
- Enforces consistent style
- Required for safety-critical embedded code

## RULE 7: ASK BEFORE ASSUMING

**If unclear:**

- ASK the human
- Don't guess
- Don't assume
- Don't improvise

**Better to ask than mess up code.**

## RULE 8: WHEN HUMAN SAYS "DON'T", REMEMBER IT

## RULE 9: JSON FILES MUST BE ASCII ENCODED (NO BOM)

**ALL JSON configuration files MUST be saved as pure ASCII without UTF-8 BOM.**

**PROBLEM:**
- UTF-8 BOM (bytes `EF BB BF`) at start of file breaks Python JSON parser
- Error: "Expecting value: line 1 column 1 (char 0)"
- We've debugged this multiple times

**CORRECT FILE FORMAT:**
```powershell
# Save JSON as ASCII without BOM
[System.IO.File]::WriteAllText($path, $content, [System.Text.Encoding]::ASCII)
```

**DETECTION:**
```powershell
# Check for BOM corruption
$bytes = [System.IO.File]::ReadAllBytes($jsonFile)
if ($bytes[0] -eq 239 -and $bytes[1] -eq 187 -and $bytes[2] -eq 191) {
    Write-Host "UTF-8 BOM DETECTED - File is corrupted!" -ForegroundColor Red
}
```

**When creating/editing JSON files:**
- Always use ASCII encoding
- Never use UTF-8 with BOM
- Verify first 3 bytes are NOT `239, 187, 191`
- First byte should be `123` (the `{` character)

**This has caused build failures multiple times. Check encoding EVERY time.**



	
